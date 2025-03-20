/** *********************************************************************************
BSD 3-Clause License

Copyright (c) 2018, https://github.com/alexzhornyak
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ************************************************************************************** */

#include <vcl.h>
#pragma hdrstop

#include "UnitTreeEditorStateMachine.h"

#include <TreeShed.hpp>
#include <TreeCoEd.hpp>
#include <XMLDoc.hpp>
#include <PngImage.hpp>
#include <IOUtils.hpp>
#include <Character.hpp>
#include <DateUtils.hpp>

#include <boost/regex.hpp>

#include "Log4cpp_VCL.hpp"

#include "MConvertComponent.hpp"
#include "UTF8NoBomEncoding.hpp"

#include "UnitMain.h"
#include "UnitStateMachineParser.h"
#include "UnitStateMachineConnection.h"
#include "UnitSettings.h"
#include "UnitStateMachineUnit.h"
#include "UnitTreeChildManagers.h"
#include "UnitTreeEx.h"
#include "ScxmlShapes.hpp"
#include "UnitSyntaxEditorEx.h"
#include "UnitSyntaxEditorForm.h"
#include "UnitTreeEditorUtils.h"
#include "UnitProtocolBinding.h"
#include "TreeNodeListAccess.h"
#include "UnitReplaceForm.h"
#include "SaveSimplePersistent.h"
#include "UnitImagePreviewer.h"
#include "UnitDialogCollectOutputs.h"
#include "UnitPresetCondition.h"
#include "UnitPresetOnOff.h"
#include "UnitSyntaxScxmlXMLView.h"
#include "UnitCustomUtils.h"
#include "UnitDialogSelectFromToShape.h"
#include "UnitDialogCollectProperty.h"
#include "UnitDialogFilterProperty.h"
#include "UnitSearchForm.h"
#include "WildCardUtils.h"
#include "UnitPropInspectorExtensions.h"
#include "TreeClipboardEx.h"
#include "UnitFrameSearchResults.h"
#include "UnitDialogWait.h"
#include "UnitTransitionXMLEditor.h"
#include "VISUAL_GLOBALS.h"
#include "UnitFrameTransitionTypes.h"
#include "UnitDummyShape.h"
#include "UnitTreeLoader.h"
#include "UnitDModule.h"
#include "UnitGraphViz.h"
#include "UnitDialogTreeSelect.h"
#include "UnitDialogDelayedTransition.h"
#include "KeyboardUtils.h"
#include "UnitSVGExporter.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

#define MAX_FLASH_TIMES		6

const int stPanelName = 0;
const int stPanelText = 1;
const int stPanelCoords = 2;
const int stPanelPage = 3;
const int stPanelModified = 4;
const int stPanelCaps = 5;
const int stPanelNum = 6;
const int stPanelScr = 7;

static const UnicodeString s_CAPTION_RAW_SCXML(L"Raw scxml");
static const UnicodeString s_CAPTION_HPP_SCXML(L"HPP scxml");
static const UnicodeString s_CAPTION_DFM(L"DFM");
static const UnicodeString s_CAPTION_SVG(L"SVG");

typedef enum {
	ecctExpandAll, ecctExpandLevel, ecctCollapseAll, ecctCollapseLevel
}TExpandCollapseChildType;

typedef enum {
	ctatCenterCenter, ctatCenterTop, ctatCenterBottom, ctatLeftCenter, ctatRightCenter, ctatMAXSIZE
}TConnectionTextAlignType;

static UnicodeString ConnectionTextAlignToString(const TConnectionTextAlignType val) {
	switch(val) {
	case ctatCenterCenter:
		return L"H: Center, V: Center";
	case ctatCenterTop:
		return L"H: Center, V: Top";
	case ctatCenterBottom:
		return L"H: Center, V: Bottom";
	case ctatLeftCenter:
		return L"H: Left, V: Center";
	case ctatRightCenter:
		return L"H: Right, V: Center";
	}
	return L"UNKNOWN";
}

typedef enum {
	crlEventless, crlDelayed, crlDonestate, crlMAXSIZE
}TConnectionRole;

static UnicodeString ConnectionRoleToString(const TConnectionRole val) {
	switch(val) {
	case crlEventless:
		return L"Eventless (Automatic)";
	case crlDelayed:
		return L"Delayed";
	case crlDonestate:
		return L"'done.state.'";
	}
	return L"UNKNOWN";
}

/* Индексы для TreeImagePool */
int g_TransitionSimpleIndex = -1;
int g_TransitionSelfIndex = -1;
int g_BreakpointActiveIndex = -1;
int g_BreakpointInactiveIndex = -1;

// ---------------------------------------------------------------------------
bool SortVisualShapesByLevel(TVisualScxmlBaseShape *i, TVisualScxmlBaseShape * j) {
	return(i->Level() < j->Level());
}

// ---------------------------------------------------------------------------
void CollectPossibleParentsAndChildrenForExchange(TVisualScxmlBaseShape *AVisualShape,
	std::vector<TVisualScxmlBaseShape*> &VecPossibleParents,
	std::vector<TVisualScxmlBaseShape*> &VecPossibleChildren,
	std::vector<TVisualScxmlBaseShape*> &VecPossibleLostChildren, int DeltaX = 0, int DeltaY = 0) {

	// AVisualShape - это то, что мы тянем в этот момент

	if (AVisualShape && AVisualShape->Tree) {

		TCustomTree *ATree = AVisualShape->Tree;

		TRect ASenderRect = AVisualShape->Bounds();
		OffsetRect(ASenderRect, DeltaX, DeltaY);

		TRect OutRect;
		const bool bShapeInsideParent = AVisualShape->VisualParent ? IntersectRect(OutRect,
			AVisualShape->VisualParent->Bounds(), ASenderRect) : false;

		for (int i = ATree->Shapes->Count - 1; i >= 0; i--) {
			TVisualScxmlBaseShape * AExaminedShape = dynamic_cast<TVisualScxmlBaseShape*>(ATree->Shapes->Items[i]);
			if (AExaminedShape) {

				if (AExaminedShape != AVisualShape) {
					// нельзя для TScxmlShape назначать новых родителей
					if (!AVisualShape->InheritsFrom(__classid(TScxmlShape))
						&& AExaminedShape != AVisualShape->VisualParent) {
						const bool bLessParentLevel = AVisualShape->VisualParent ?
							(bShapeInsideParent && AExaminedShape->Level()
							< AVisualShape->VisualParent->Level()) : false;

						if (Editorutils::RectInsideRect(AExaminedShape->Bounds(), ASenderRect) && !bLessParentLevel) {
							VecPossibleParents.push_back(AExaminedShape);
						}
					}

					// TScxmlShape не может терять дочерних за пределами
					if (!AVisualShape->InheritsFrom(__classid(TScxmlShape)) && (AExaminedShape->Parent == AVisualShape)
						&& !IntersectRect(OutRect, AExaminedShape->Bounds(), ASenderRect)) {
						VecPossibleLostChildren.push_back(AExaminedShape);
					}
					else {
						if (AExaminedShape->Parent == AVisualShape->Parent && Editorutils::RectInsideRect(ASenderRect,
								AExaminedShape->Bounds())) {
							VecPossibleChildren.push_back(AExaminedShape);
						}
					}
				}

				AExaminedShape->Examined = false;
			}
		}
		std::sort(VecPossibleParents.begin(), VecPossibleParents.end(), SortVisualShapesByLevel);
	}
}

void AssertCreatedShape(TTreeNodeShape *AShape) {
	if (!AShape)
		throw Exception("Can not create shape! Check that parent is available for such type!");
}

void RegisterTextDummyShape() {
#pragma startup RegisterTextDummyShape

	Classes::RegisterClassA(__classid(TTextDummyShape));
}

void AppendImageToTreeImagePool(TImageList *AImageList, const int iStoredIndex, int &iNewIndex) {
	if (iNewIndex == -1) {
		std::auto_ptr<TIcon>AIconPtr(new TIcon());
		AImageList->GetIcon(iStoredIndex, AIconPtr.get());
		std::auto_ptr<TPicture>APicturePtr(new TPicture());
		APicturePtr->Icon = AIconPtr.get();
		iNewIndex = TreeImagePool->Add(APicturePtr.get());
#ifdef _DEBUG
		WLOG_DEBUG(L"TreeImagePool TreeEditor image:[%d] was mapped to global:[%d]", iStoredIndex, iNewIndex);
#endif
	}
}

// ---------------------------------------------------------------------------
// ----------------------------- TStateMachineEditor -------------------------
// ---------------------------------------------------------------------------
__fastcall TStateMachineEditor::TStateMachineEditor(TComponent * AOwner,
	TStateMachineEditorUnit * AStateMachineEditorUnit, bool AUseGlobalPropInspector) : TTreeEditorEx(AOwner),
FStateMachineEditorUnit(AStateMachineEditorUnit), FForbidInternalAdd(false), FPropSettingsInspector(NULL),
FDesignObjects(NULL), FSyntaxEditorEx(new TFrameSyntaxEditorEx(this)), FEditDocRaw(new TLMDEditDocument(this)),
FPropSplitter(NULL), FFormReplaceDlg(NULL) {

	FActiveDebugShape = NULL;
	FActiveDebugConnection = NULL;

	FFrameTransitionTypes = NULL;
	FIsReloadingTree = false;

	FMouseWheelEventTimedFlag = std::make_pair(false, Now());
	FTreeModifiedForInheritanceFlag = false;
	FTreeNewConnectionToolbarFlag = false;
	FTreeValidateSimpleTextFlag = false;
	FTreeFillNodeTreeFlag = false;
	FRestoreInspectorStateFlag = false;

	FTreeSelectionChanged = false;

	m_i_found = 0;
	m_i_connect_found = 0;

	m_iFlash = MAX_FLASH_TIMES;

	FSearchTimer = new TTimer(this);
	FSearchTimer->Enabled = false;
	FSearchTimer->Interval = 500;

	if (!AUseGlobalPropInspector) {
		FDesignObjects = new TLMDDesignObjects(this);

		FPropSettingsInspector = new Lmddsgpropinsp::TSettingsPropertyInspector(this);
		FPropSettingsInspector->Parent = this;
		FPropSettingsInspector->Align = alRight;
		FPropSettingsInspector->Selection = FDesignObjects;
		FPropSettingsInspector->OnChange = this->PropSettingsInspectorChange;
		/// сортируем так, чтобы свойства scxml были сверху
		FPropSettingsInspector->SortType = Lmdinspropinsp::stCustom;
		FPropSettingsInspector->OnCompare = FormScxmlGui->PropSettingsInspector->OnCompare;
		FPropSettingsInspector->ExpandedCategories = true;
		FPropSettingsInspector->FlatSelectionBackgroundColor = TColor(0x00E5E5E5);
		FPropSettingsInspector->FlatSelectionColor = clWindowText;
		FPropSettingsInspector->GridVisible = false;
		FPropSettingsInspector->GutterVisible = true;
		FPropSettingsInspector->CursorVisible = true;
		FPropSettingsInspector->SelectionStyle = TLMDPropsPageSelectionStyle::ssFlat;
		FPropSettingsInspector->CategoryBackgroundColor = TColor(15066597);
		FPropSettingsInspector->CategoryFont->Color = clMaroon;
		FPropSettingsInspector->CategoryFont->Height = -11;
		FPropSettingsInspector->CategoryFont->Style = TFontStyles() << fsBold;
		FPropSettingsInspector->Color = clWindow;
		FPropSettingsInspector->Font->Color = clWindowText;
		FPropSettingsInspector->Font->Height = -11;

		FPropSettingsInspector->Width = SettingsData->TempRegistry->Values
			[this->ClassName() + ".PropSettingsInspector.Width"].ToIntDef(FPropSettingsInspector->Width);
		FPropSettingsInspector->Splitter = SettingsData->TempRegistry->Values
			[this->ClassName() + ".PropSettingsInspector.Splitter"].ToIntDef(FPropSettingsInspector->Splitter);

		FPropSplitter = new TSplitter(this);
		FPropSplitter->Parent = this;
		FPropSplitter->Align = alRight;
	}

	UnicodeString sUnitFileExt = TStateMachineEditorUnit::FileExt_;
	sUnitFileExt = sUnitFileExt.Delete(1, 1);
	this->OpenDialog1->DefaultExt = sUnitFileExt;
	this->SaveDialog1->DefaultExt = sUnitFileExt;
	this->OpenDialog1->Filter = GetOpenDialogFilter();
	this->SaveDialog1->Filter = GetOpenDialogFilter();

	this->Save1->OnClick = ButtonSaveClick;
	this->New1->OnClick = ButtonNewClick;

	ActionLockAxisX->OnExecute = OnActionLockAxisExecute;
	ActionLockAxisY->OnExecute = OnActionLockAxisExecute;

	ActionLockAxisX->OnUpdate = OnActionLockAxisUpdate;
	ActionLockAxisY->OnUpdate = OnActionLockAxisUpdate;

	MenuDummyTextReset->OnClick = OnMenuDummyTextReset;
	MenuDummyTextEdit->OnClick = OnMenuDummyTextEdit;
	MenuDummyTextAlignToLeft->OnClick = OnMenuDummyTextAlign;
	MenuDummyTextAlignToTop->OnClick = OnMenuDummyTextAlign;
	MenuDummyTextAlignToRight->OnClick = OnMenuDummyTextAlign;
	MenuDummyTextAlignToBottom->OnClick = OnMenuDummyTextAlign;
	MenuDummyTextEditCondition->OnClick = OnMenuDummyTextEditCondition;
	MenuDummyTextRotate90Left->OnClick = OnMenuDummyTextRotate90Left;
	MenuDummyTextRotate90Right->OnClick = OnMenuDummyTextRotate90Right;
	MenuDummyTextRotateCustom->OnClick = OnMenuDummyTextRotateCustom;
	MenuDummyTextVerticalCentering->OnClick = OnMenuDummyTextVerticalCentering;

	// запрещаем менюшки, так как они избыточны для этого проекта
	Close1->Visible = false;
	N7->Visible = false;
	Insert1->Visible = false;

	// эта функция разрывает связь между дочерним и родительским элементом, поэтому запрещаем её
	Prune1->Visible = false;
	Prunenode1->Visible = false;

	Sidesstyle1->OnAdvancedDrawItem = CheckImageAdvancedDrawItem;
	Linestyle1->OnAdvancedDrawItem = CheckImageAdvancedDrawItem;
	Invertedsidesstyle1->OnAdvancedDrawItem = CheckImageAdvancedDrawItem;
	Curve2->OnAdvancedDrawItem = CheckImageAdvancedDrawItem;

	MenuChartVersion->OnClick = OnMenuChartVersion;
	MenuChartStatistics->OnClick = OnMenuChartStatistics;

	MenuResetToDefault->OnClick = OnMenuResetViewToDefault;
	MenuViewPresetsMinimal->OnClick = OnMenuViewPresetsMinimal;
	MenuViewPresetsCustom->OnClick = OnMenuViewPresetsCustom;
	MenuViewPresets->OnClick = OnMenuViewPresets;
	MenuViewFullScreen->OnClick = OnMenuViewFullScreen;

	for (int i = 0; i < File1->Count; i++) {
		File1->Items[i]->ShortCut = 0;
	}

	N3->Visible = false; // Help

	AddChild1->Caption = "Add visual child";
	AddChild1->ImageIndex = 81;

	FMenuAddNonVisualChild = new TMenuItem(this);
	FMenuAddNonVisualChild->Name = "MenuAddNonVisualChild";
	FMenuAddNonVisualChild->Caption = "Add non-visual child";
	FMenuAddNonVisualChild->ImageIndex = 83;

	PopupNode->Items->Insert(AddChild1->MenuIndex + 1, FMenuAddNonVisualChild);

	FMenuConvertTo = new TMenuItem(this);
	FMenuConvertTo->Caption = "Convert to";
	FMenuConvertTo->ImageIndex = 101;
	PopupNode->Items->Insert(AddChild1->MenuIndex + 2, FMenuConvertTo);

	FMenuShapeEdit = new TMenuItem(this);
	FMenuShapeEdit->Name = "MenuShapeEdit";
	FMenuShapeEdit->Caption = "Edit...";
	FMenuShapeEdit->OnClick = OnEditShapeClick;
	FMenuShapeEdit->Default = true;
	FMenuShapeEdit->ImageIndex = 44;
	PopupNode->Items->Insert(Edit1->MenuIndex, FMenuShapeEdit);
	Edit1->Default = false;
	Edit1->ImageIndex = 80;

	int iMenuAddIndex = 1;
	const int iStartMenuAddIndex = MenuDividerAddChildren->MenuIndex;

	FMenuIsInitial = new TMenuItem(this);
	FMenuIsInitial->Caption = "Initial";
	FMenuIsInitial->OnClick = OnMenuIsInitialClick;
	FMenuIsInitial->AutoCheck = true;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuIsInitial);

	FMenuSetBreakpoint = new TMenuItem(this);
	FMenuSetBreakpoint->Caption = "Breakpoint";
	FMenuSetBreakpoint->OnClick = OnMenuSetBreakpoint;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuSetBreakpoint);

	FMenuSkipDebugging = new TMenuItem(this);
	FMenuSkipDebugging->Caption = "Skip debugging";
	FMenuSkipDebugging->OnClick = OnMenuSkipDebugging;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuSkipDebugging);

	PopupNode->Items->InsertNewLineAfter(FMenuSkipDebugging);
	iMenuAddIndex++;

	FMenuChangeXMLTextType = new TMenuItem(this);
	FMenuChangeXMLTextType->Caption = "XMLType [Text]";
	FMenuChangeXMLTextType->ImageIndex = 137;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuChangeXMLTextType);
	for (int i = xttText; i <= xttSCXML; i++) {
		TMenuItem * AMenuItem = new TMenuItem(this);
		AMenuItem->Tag = i;
		TPropInfo *AInfo = GetPropInfo(__classid(TSyntaxString), "Type");
		if (AInfo) {
			const UnicodeString sEnumName = GetEnumName(*AInfo->PropType, i);
			AMenuItem->Caption = StringReplace(sEnumName, "xtt", "", TReplaceFlags());
		}
		AMenuItem->OnClick = OnMenuChangeXMLTextType;
		FMenuChangeXMLTextType->Add(AMenuItem);
	}

	FMenuTextAlign = new TMenuItem(this);
	FMenuTextAlign->Caption = "Text align";
	FMenuTextAlign->ImageIndex = 60;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuTextAlign);

	FMenuShapeTextAutoSize = new TMenuItem(this);
	FMenuShapeTextAutoSize->Caption = "AutoSize";
	FMenuShapeTextAutoSize->ImageIndex = 31;
	FMenuShapeTextAutoSize->OnClick = OnMenuShapeTextAutoSize;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuShapeTextAutoSize);

	FMenuResetChildShapePos = new TMenuItem(this);
	FMenuResetChildShapePos->Caption = "Reset position";
	FMenuResetChildShapePos->ImageIndex = 88;
	FMenuResetChildShapePos->OnClick = OnMenuResetChildShapePos;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuResetChildShapePos);

	FMenuMakeSelfConnection = new TMenuItem(this);
	FMenuMakeSelfConnection->Caption = "Make self connection";
	FMenuMakeSelfConnection->OnClick = OnMenuMakeSelfConnection;
	FMenuMakeSelfConnection->ImageIndex = ActionMakeSelfConnection->ImageIndex;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuMakeSelfConnection);

	ActionMakeSelfConnection->OnExecute = OnMenuMakeSelfConnection;
	ActionMakeSelfConnection->OnUpdate = OnActionMakeSelfConnectionUpdate;

	FMenuPasteConnection = new TMenuItem(this);
	FMenuPasteConnection->Caption = "Paste connection";
	FMenuPasteConnection->ImageIndex = 94;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuPasteConnection);

	FMenuArrangeSelfConnections = new TMenuItem(this);
	FMenuArrangeSelfConnections->Caption = "Arrange self-connections";
	FMenuArrangeSelfConnections->OnClick = OnMenuArrangeSelfConnections;
	FMenuArrangeSelfConnections->ImageIndex = 69;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuArrangeSelfConnections);

	FMenuArrangeLayout = new TMenuItem(this);
	FMenuArrangeLayout->Caption = "Auto Arrange Layout";
	FMenuArrangeLayout->OnClick = OnMenuArrangeLayout;
	FMenuArrangeLayout->ImageIndex = 56;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuArrangeLayout);

	FMenuMoveBrotherUp = new TMenuItem(this);
	FMenuMoveBrotherUp->Caption = "Move up";
	FMenuMoveBrotherUp->OnClick = OnMenuMoveBrotherUp;
	FMenuMoveBrotherUp->ImageIndex = 48;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuMoveBrotherUp);

	FMenuMoveBrotherDown = new TMenuItem(this);
	FMenuMoveBrotherDown->Caption = "Move down";
	FMenuMoveBrotherDown->OnClick = OnMenuMoveBrotherDown;
	FMenuMoveBrotherDown->ImageIndex = 50;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuMoveBrotherDown);

	FMenuReplaceText = new TMenuItem(this);
	FMenuReplaceText->Caption = "Replace text (Ctrl+R)";
	FMenuReplaceText->OnClick = OnMenuReplaceText;
	FMenuReplaceText->ImageIndex = 58;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuReplaceText);

	FMenuSwapVisualBrothers = new TMenuItem(this);
	FMenuSwapVisualBrothers->Caption = "Swap brothers";
	FMenuSwapVisualBrothers->OnClick = OnMenuSwapVisualBrothers;
	FMenuSwapVisualBrothers->ImageIndex = 77;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuSwapVisualBrothers);

	FMenuImportBindingsAndFolders = new TMenuItem(this);
	FMenuCollectOutputs = new TMenuItem(this);

	if (SettingsData->AviaExtensionEnabled) {
		FMenuImportBindingsAndFolders->Caption = "Import bindings";
		FMenuImportBindingsAndFolders->ImageIndex = 62;
		FMenuImportBindingsAndFolders->OnClick = OnMenuImportBindingsAndFolders;
		PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuImportBindingsAndFolders);

		FMenuCollectOutputs->Caption = "Collect outputs";
		FMenuCollectOutputs->OnClick = OnMenuCollectOutputs;
		FMenuCollectOutputs->ImageIndex = 57;
		PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuCollectOutputs);
	}

	/* НАСЛЕДОВАНИЕ */
	// для Shapes
	FMenuInheritance = new TMenuItem(this);
	FMenuInheritance->Caption = "Inheritance";
	FMenuInheritance->ImageIndex = 139;

	{
		FMenuSetInheritance = new TMenuItem(this);
		FMenuSetInheritance->Caption = "Set inheritance";
		FMenuSetInheritance->OnClick = OnMenuSetInheritance;
		FMenuSetInheritance->Tag = 0;
		FMenuSetInheritance->ImageIndex = 140;

		FMenuResetInheritance = new TMenuItem(this);
		FMenuResetInheritance->Caption = "Reset inheritance";
		FMenuResetInheritance->OnClick = OnMenuSetInheritance;
		FMenuResetInheritance->Tag = 1;
		FMenuResetInheritance->ImageIndex = 142;

		FMenuRevertToInheritedProps = new TMenuItem(this);
		FMenuRevertToInheritedProps->Caption = "Revert inherited properties";
		FMenuRevertToInheritedProps->OnClick = OnMenuRevertToInheritedProps;
		FMenuRevertToInheritedProps->ImageIndex = 138;

		FMenuRevertInheritedGeometry = new TMenuItem(this);
		FMenuRevertInheritedGeometry->Caption = "Revert geometry only";
		FMenuRevertInheritedGeometry->OnClick = OnMenuRevertInheritedGeometry;
		FMenuRevertInheritedGeometry->ImageIndex = 155;

		FMenuRevertInOutConnections = new TMenuItem(this);
		FMenuRevertInOutConnections->Caption = "Revert I/O inherited connections";
		FMenuRevertInOutConnections->OnClick = OnMenuRevertInOutConnections;
		FMenuRevertInOutConnections->ImageIndex = 154;

		FMenuRevertRecursive = new TMenuItem(this);
		FMenuRevertRecursive->Caption = "Total Revert (children and connections)";
		FMenuRevertRecursive->OnClick = OnMenuRevertRecursive;
		FMenuRevertRecursive->ImageIndex = 145;

		FMenuInheritanceDumpDifference = new TMenuItem(this);
		FMenuInheritanceDumpDifference->Caption = "Dump all mismatches";
		FMenuInheritanceDumpDifference->OnClick = OnMenuInheritanceDumpDifference;
		FMenuInheritanceDumpDifference->ImageIndex = 153;

		FMenuInheritanceResolveFirstMismatch = new TMenuItem(this);
		FMenuInheritanceResolveFirstMismatch->Caption = "Resolve Mismatch";
		FMenuInheritanceResolveFirstMismatch->OnClick = OnMenuInheritanceResolveFirstMismatch;
		FMenuInheritanceResolveFirstMismatch->ImageIndex = 160;

		FMenuInheritanceResolveAllMismatches = new TMenuItem(this);
		FMenuInheritanceResolveAllMismatches->Caption = "Resolve All Mismatches";
		FMenuInheritanceResolveAllMismatches->OnClick = OnMenuInheritanceResolveAllMismatches;
		FMenuInheritanceResolveAllMismatches->ImageIndex = 161;

		FMenuInheritanceResolveClearAll = new TMenuItem(this);
		FMenuInheritanceResolveClearAll->Caption = "Clear All Resolved";
		FMenuInheritanceResolveClearAll->OnClick = OnMenuInheritanceResolveClearAll;
		FMenuInheritanceResolveClearAll->ImageIndex = 162;

		FMenuRevertInInheritedUnits = new TMenuItem(this);
		FMenuRevertInInheritedUnits->Caption = "Synchronize Selected (in all inherited)";
		FMenuRevertInInheritedUnits->OnClick = OnMenuRevertInInheritedUnits;
		FMenuRevertInInheritedUnits->ImageIndex = 167;

		FMenuInheritance->Add(FMenuInheritanceDumpDifference);
		FMenuInheritance->NewBottomLine();
		FMenuInheritance->Add(FMenuInheritanceResolveFirstMismatch);
		FMenuInheritance->Add(FMenuInheritanceResolveAllMismatches);
		FMenuInheritance->Add(FMenuInheritanceResolveClearAll);
		FMenuInheritance->NewBottomLine();
		FMenuInheritance->Add(FMenuSetInheritance);
		FMenuInheritance->Add(FMenuResetInheritance);
		FMenuInheritance->NewBottomLine();
		FMenuInheritance->Add(FMenuRevertToInheritedProps);
		FMenuInheritance->Add(FMenuRevertInheritedGeometry);
		FMenuInheritance->Add(FMenuRevertInOutConnections);
		FMenuInheritance->NewBottomLine();
		FMenuInheritance->Add(FMenuRevertRecursive);

		FMenuInheritance->NewBottomLine();
		FMenuInheritance->Add(FMenuRevertInInheritedUnits);
	}

	// для Transitions
	FMenuTransitionInheritance = new TMenuItem(this);
	FMenuTransitionInheritance->Caption = "Inheritance";
	FMenuTransitionInheritance->ImageIndex = 139;

	{
		FMenuTransitionSetInheritance = new TMenuItem(this);
		FMenuTransitionSetInheritance->Caption = "Set inheritance";
		FMenuTransitionSetInheritance->OnClick = OnMenuSetInheritance;
		FMenuTransitionSetInheritance->Tag = 0;
		FMenuTransitionSetInheritance->ImageIndex = 140;

		FMenuTransitionResetInheritance = new TMenuItem(this);
		FMenuTransitionResetInheritance->Caption = "Reset inheritance";
		FMenuTransitionResetInheritance->OnClick = OnMenuSetInheritance;
		FMenuTransitionResetInheritance->Tag = 1;
		FMenuTransitionResetInheritance->ImageIndex = 142;

		FMenuTransitionRevertToInheritedProps = new TMenuItem(this);
		FMenuTransitionRevertToInheritedProps->Caption = "Revert inherited properties";
		FMenuTransitionRevertToInheritedProps->OnClick = OnMenuRevertToInheritedProps;
		FMenuTransitionRevertToInheritedProps->ImageIndex = 138;

		FMenuTransitionInheritanceDumpDifference = new TMenuItem(this);
		FMenuTransitionInheritanceDumpDifference->Caption = "Dump mismatches";
		FMenuTransitionInheritanceDumpDifference->OnClick = OnMenuInheritanceDumpDifference;
		FMenuTransitionInheritanceDumpDifference->ImageIndex = 153;

		FMenuTransitionInheritanceResolveFirstMismatch = new TMenuItem(this);
		FMenuTransitionInheritanceResolveFirstMismatch->Caption = "Resolve Mismatch";
		FMenuTransitionInheritanceResolveFirstMismatch->OnClick = OnMenuInheritanceResolveFirstMismatch;
		FMenuTransitionInheritanceResolveFirstMismatch->ImageIndex = 160;

		FMenuTransitionInheritanceResolveAllMismatches = new TMenuItem(this);
		FMenuTransitionInheritanceResolveAllMismatches->Caption = "Resolve All Mismatches";
		FMenuTransitionInheritanceResolveAllMismatches->OnClick = OnMenuInheritanceResolveAllMismatches;
		FMenuTransitionInheritanceResolveAllMismatches->ImageIndex = 161;

		FMenuTransitionInheritanceResolveClearAll = new TMenuItem(this);
		FMenuTransitionInheritanceResolveClearAll->Caption = "Clear All Resolved";
		FMenuTransitionInheritanceResolveClearAll->OnClick = OnMenuInheritanceResolveClearAll;
		FMenuTransitionInheritanceResolveClearAll->ImageIndex = 162;

		FMenuTransitionRevertInInheritedUnits = new TMenuItem(this);
		FMenuTransitionRevertInInheritedUnits->Caption = "Synchronize Selected (in all inherited)";
		FMenuTransitionRevertInInheritedUnits->OnClick = OnMenuRevertInInheritedUnits;
		FMenuTransitionRevertInInheritedUnits->ImageIndex = 167;

		FMenuTransitionInheritance->Add(FMenuTransitionInheritanceDumpDifference);
		FMenuTransitionInheritance->NewBottomLine();
		FMenuTransitionInheritance->Add(FMenuTransitionInheritanceResolveFirstMismatch);
		FMenuTransitionInheritance->Add(FMenuTransitionInheritanceResolveAllMismatches);
		FMenuTransitionInheritance->Add(FMenuTransitionInheritanceResolveClearAll);
		FMenuTransitionInheritance->NewBottomLine();
		FMenuTransitionInheritance->Add(FMenuTransitionSetInheritance);
		FMenuTransitionInheritance->Add(FMenuTransitionResetInheritance);
		FMenuTransitionInheritance->Add(FMenuTransitionRevertToInheritedProps);
		FMenuTransitionInheritance->NewBottomLine();
		FMenuTransitionInheritance->Add(FMenuTransitionRevertInInheritedUnits);
	}

	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuInheritance);

	/* ПРЕСЕТЫ */
	FMenuPresets = new TMenuItem(this);
	FMenuPresets->Caption = "Presets";
	FMenuPresets->ImageIndex = 63;

	TMenuItem *AMenuPresetCondition = new TMenuItem(this);
	AMenuPresetCondition->Caption = "Condition";
	AMenuPresetCondition->ImageIndex = 64;
	AMenuPresetCondition->OnClick = OnMenuPresetCondition;
	FMenuPresets->Add(AMenuPresetCondition);

	TMenuItem *AMenuPresetOnOff = new TMenuItem(this);
	AMenuPresetOnOff->Caption = "On-Off";
	AMenuPresetOnOff->ImageIndex = 66;
	AMenuPresetOnOff->OnClick = OnMenuPresetOnOff;
	FMenuPresets->Add(AMenuPresetOnOff);

	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuPresets);

	PopupNode->Items->InsertNewLineAfter(FMenuPresets);
	iMenuAddIndex++;

	FMenuSelectItemWithChildren = new TMenuItem(this);
	FMenuSelectItemWithChildren->ImageIndex = 144;
	FMenuSelectItemWithChildren->Caption = "Select with children";
	FMenuSelectItemWithChildren->OnClick = OnMenuSelectItemWithChildren;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuSelectItemWithChildren);

	FMenuSelectChildren = new TMenuItem(this);
	FMenuSelectChildren->ImageIndex = 95;
	FMenuSelectChildren->Caption = "Select children only";
	FMenuSelectChildren->OnClick = OnMenuSelectChildren;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuSelectChildren);

	FMenuSelectSiblings = new TMenuItem(this);
	FMenuSelectSiblings->ImageIndex = 96;
	FMenuSelectSiblings->Caption = "Select siblings";
	FMenuSelectSiblings->OnClick = OnMenuSelectSiblings;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuSelectSiblings);

	PopupNode->Items->InsertNewLineAfter(FMenuSelectSiblings);
	iMenuAddIndex++;

	MenuSelectSimilar->OnClick = OnMenuSelectSimilar;

	TMenuItem * AMenuSelectItemWithChildren2 = new TMenuItem(this);
	AMenuSelectItemWithChildren2->ImageIndex = 144;
	AMenuSelectItemWithChildren2->Caption = "Select with children";
	AMenuSelectItemWithChildren2->OnClick = OnMenuSelectItemWithChildren;
	MenuSelectSimilar->Parent->Insert(2, AMenuSelectItemWithChildren2);

	TMenuItem * AMenuSelectChildren2 = new TMenuItem(this);
	AMenuSelectChildren2->ImageIndex = 95;
	AMenuSelectChildren2->Caption = "Select children only";
	AMenuSelectChildren2->OnClick = OnMenuSelectChildren;
	MenuSelectSimilar->Parent->Insert(3, AMenuSelectChildren2);

	TMenuItem * AMenuSelectSiblings2 = new TMenuItem(this);
	AMenuSelectSiblings2->ImageIndex = 96;
	AMenuSelectSiblings2->Caption = "Select siblings";
	AMenuSelectSiblings2->OnClick = OnMenuSelectSiblings;
	MenuSelectSimilar->Parent->Insert(4, AMenuSelectSiblings2);

	MenuSelectSimilar->Parent->InsertNewLineAfter(AMenuSelectSiblings2);

	MenuSelectVisual->OnClick = OnMenuSelectVisual;
	MenuSelectNonVisual->OnClick = OnMenuSelectNonVisual;

	for (int i = sctScxml; i < TStateChildType::sctMAXSIZE; i++) {
		const TStateChildType AStateType = TStateChildType(i);
		const UnicodeString sName = TScxmlBaseShape::TypeToName(AStateType);

		TMenuItem *AMenuSelectItem = new TMenuItem(this);
		AMenuSelectItem->Caption = L"Select " + sName;
		AMenuSelectItem->Tag = i;
		AMenuSelectItem->OnClick = OnMenuSelectCategory;
		MenuSelectCategory->Add(AMenuSelectItem);
	}

	MenuClearSelection->OnClick = OnMenuClearSelection;
	MenuSelectAtomic->OnClick = OnMenuSelectAtomic;
	MenuSelectCompound->OnClick = OnMenuSelectCompound;
	MenuSelectStatesandParallel->OnClick = OnMenuSelectStatesandParallel;
	MenuSelectExecutableContent->OnClick = OnMenuSelectExecutableContent;

	FMenuCollapseAll = new TMenuItem(this);
	FMenuCollapseAll->ImageIndex = 163;
	FMenuCollapseAll->Caption = "Collapse all";
	FMenuCollapseAll->OnClick = OnMenuExpandCollapseChildren;
	FMenuCollapseAll->Tag = ecctCollapseAll;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuCollapseAll);

	FMenuCollapseLevel = new TMenuItem(this);
	FMenuCollapseLevel->ImageIndex = 164;
	FMenuCollapseLevel->Caption = "Collapse level";
	FMenuCollapseLevel->OnClick = OnMenuExpandCollapseChildren;
	FMenuCollapseLevel->Tag = ecctCollapseLevel;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuCollapseLevel);

	FMenuExpandAll = new TMenuItem(this);
	FMenuExpandAll->ImageIndex = 165;
	FMenuExpandAll->Caption = "Expand all";
	FMenuExpandAll->Tag = ecctExpandAll;
	FMenuExpandAll->OnClick = OnMenuExpandCollapseChildren;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuExpandAll);

	FMenuExpandLevel = new TMenuItem(this);
	FMenuExpandLevel->ImageIndex = 166;
	FMenuExpandLevel->Caption = "Expand level";
	FMenuExpandLevel->Tag = ecctExpandLevel;
	FMenuExpandLevel->OnClick = OnMenuExpandCollapseChildren;
	PopupNode->Items->Insert(iStartMenuAddIndex + iMenuAddIndex++, FMenuExpandLevel);

	PopupNode->Items->InsertNewLineAfter(FMenuExpandLevel);
	iMenuAddIndex++;
	FMenuCopyPropertyToClipboard = new TMenuItem(this);
	FMenuCopyPropertyToClipboard->Caption = "Copy property";
	FMenuCopyPropertyToClipboard->ImageIndex = 90;
	FMenuCopyPropertyToClipboard->OnClick = OnMenuCopyPropertyToClipboard;
	PopupNode->Items->Add(FMenuCopyPropertyToClipboard);

	FMenuFilterSelectionByProperty = new TMenuItem(this);
	FMenuFilterSelectionByProperty->Caption = "Filter selection";
	FMenuFilterSelectionByProperty->ImageIndex = 105;
	FMenuFilterSelectionByProperty->OnClick = OnMenuFilterSelectionByProperty;
	PopupNode->Items->Add(FMenuFilterSelectionByProperty);

	FMenuExportAsImage = new TMenuItem(this);
	FMenuExportAsImage->Caption = "Export As Image";
	FMenuExportAsImage->OnClick = OnMenuExportAsImage;
	FMenuExportAsImage->ImageIndex = 46;
	PopupNode->Items->Add(FMenuExportAsImage);

	PopupNode->Items->InsertNewLineBefore(FMenuExportAsImage);

	for (int i = 0; i < Editorutils::dtMAXSIZE; i++) {
		TMenuItem *MenuSubTextAlign = new TMenuItem(this);
		MenuSubTextAlign->Caption = Editorutils::DirectionTypeToString(Editorutils::TDirectionType(i));
		MenuSubTextAlign->OnClick = OnMenuTextAlignClick;
		MenuSubTextAlign->Tag = i;
		MenuSubTextAlign->ImageIndex = 32 + i * 2;
		FMenuTextAlign->Add(MenuSubTextAlign);
	}

	FMenuEditTransition = new TMenuItem(this);
	FMenuEditTransition->Caption = "Edit Transition";
	FMenuEditTransition->OnClick = OnMenuEditTransitionClick;
	FMenuEditTransition->Default = true;
	FMenuEditTransition->ImageIndex = 44;

	EditAppearance->Default = false;
	EditAppearance->ImageIndex = 80;

	FMenuTransitionTextMove = new TMenuItem(this);
	FMenuTransitionTextMove->Caption = "Move text";
	FMenuTransitionTextMove->ImageIndex = 68;
	FMenuTransitionTextMove->OnClick = OnMenuTransitionTextMoveClick;

	FMenuSelfConnectionAlign = new TMenuItem(this);
	FMenuSelfConnectionAlign->Caption = "Self connection align";
	FMenuSelfConnectionAlign->ImageIndex = 98;

	std::map<TSelfConnectionAlign, int>AMapAlignImages;
	AMapAlignImages[scaLeft] = 32;
	AMapAlignImages[scaTop] = 34;
	AMapAlignImages[scaRight] = 36;
	AMapAlignImages[scaBottom] = 38;

	for (int i = 0; i < scaMAXSIZE; i++) {
		TMenuItem *AMenuSelfAlignItem = new TMenuItem(this);

		const TSelfConnectionAlign ASelfConnAlign = TSelfConnectionAlign(i);

		AMenuSelfAlignItem->Caption = SelfConnectionAlignToString(ASelfConnAlign);
		AMenuSelfAlignItem->ImageIndex = AMapAlignImages[ASelfConnAlign];
		AMenuSelfAlignItem->Tag = i;
		AMenuSelfAlignItem->OnClick = OnMenuSelfConnectionAlign;
		FMenuSelfConnectionAlign->Add(AMenuSelfAlignItem);
	}

	FMenuConnectionOverTop = new TMenuItem(this);
	FMenuConnectionOverTop->Caption = "Connection over top";
	FMenuConnectionOverTop->OnClick = OnMenuConnectionOver;
	FMenuConnectionOverTop->Tag = cotOverTop;
	FMenuConnectionOverTop->ImageIndex = 53;

	FMenuConnectionOverLeft = new TMenuItem(this);
	FMenuConnectionOverLeft->Caption = "Connection over left";
	FMenuConnectionOverLeft->OnClick = OnMenuConnectionOver;
	FMenuConnectionOverLeft->Tag = cotOverLeft;
	FMenuConnectionOverLeft->ImageIndex = 52;

	FMenuConnectionOverRight = new TMenuItem(this);
	FMenuConnectionOverRight->Caption = "Connection over right";
	FMenuConnectionOverRight->OnClick = OnMenuConnectionOver;
	FMenuConnectionOverRight->Tag = cotOverRight;
	FMenuConnectionOverRight->ImageIndex = 54;

	FMenuConnectionOverBottom = new TMenuItem(this);
	FMenuConnectionOverBottom->Caption = "Connection over bottom";
	FMenuConnectionOverBottom->OnClick = OnMenuConnectionOver;
	FMenuConnectionOverBottom->Tag = cotOverBottom;
	FMenuConnectionOverBottom->ImageIndex = 55;

	FMenuTransitionTextAlign = new TMenuItem(this);
	FMenuTransitionTextAlign->Caption = "Connection text align";
	FMenuTransitionTextAlign->ImageIndex = 99;
	for (int i = 0; i < TConnectionTextAlignType::ctatMAXSIZE; i++) {
		TMenuItem *AMenuTextAlignItem = new TMenuItem(this);
		AMenuTextAlignItem->Caption = ConnectionTextAlignToString(TConnectionTextAlignType(i));
		AMenuTextAlignItem->Tag = i;
		AMenuTextAlignItem->OnClick = OnMenuTransitionTextAlign;
		FMenuTransitionTextAlign->Add(AMenuTextAlignItem);
	}

	FMenuTransitionSwap = new TMenuItem(this);
	FMenuTransitionSwap->Caption = "Swap source and target";
	FMenuTransitionSwap->OnClick = OnMenuTransitionSwap;
	FMenuTransitionSwap->ImageIndex = 67;

	TMenuItem *AMenuTransitionFind = new TMenuItem(this);
	AMenuTransitionFind->Caption = L"Find shape";
	AMenuTransitionFind->ImageIndex = 8;

	TMenuItem *AMenuTransitionFindFrom = new TMenuItem(this);
	AMenuTransitionFindFrom->Caption = L"From";
	AMenuTransitionFindFrom->OnClick = OnMenuTransitionFindSourceTarget;
	AMenuTransitionFindFrom->ImageIndex = 106;
	AMenuTransitionFindFrom->Tag = TRANSITION_FIND_SOURCE;

	TMenuItem *AMenuTransitionFindTo = new TMenuItem(this);
	AMenuTransitionFindTo->Caption = L"To";
	AMenuTransitionFindTo->OnClick = OnMenuTransitionFindSourceTarget;
	AMenuTransitionFindTo->ImageIndex = 107;
	AMenuTransitionFindTo->Tag = TRANSITION_FIND_TARGET;

	AMenuTransitionFind->Add(AMenuTransitionFindFrom);
	AMenuTransitionFind->Add(AMenuTransitionFindTo);

	FMenuCopyTransition = new TMenuItem(this);
	FMenuCopyTransition->Caption = "Copy to clipboard";
	FMenuCopyTransition->ImageIndex = 93;
	FMenuCopyTransition->OnClick = OnMenuTransitionCopy;

	FMenuTransitionSwitchSimple = new TMenuItem(this);
	FMenuTransitionSwitchSimple->Caption = "Switch: SIMPLE";
	FMenuTransitionSwitchSimple->OnClick = OnMenuTransitionSwitch;

	FMenuTransitionSwitchInvertCondition = new TMenuItem(this);
	FMenuTransitionSwitchInvertCondition->Caption = "Switch: INVERT CONDITION";
	FMenuTransitionSwitchInvertCondition->OnClick = OnMenuTransitionSwitch;

	FMenuTransitionConvertTo = new TMenuItem(this);
	FMenuTransitionConvertTo->Caption = "Convert to";
	FMenuTransitionConvertTo->ImageIndex = 101;
	for (int i = 0; i < TConnectionRole::crlMAXSIZE; i++) {
		TMenuItem *AMenuItem = new TMenuItem(FMenuTransitionConvertTo);
		AMenuItem->Caption = ConnectionRoleToString(TConnectionRole(i));
		AMenuItem->Tag = i;
		AMenuItem->ImageIndex = 182 + i;
		AMenuItem->OnClick = OnMenuTransitionConvertToClick;
		FMenuTransitionConvertTo->Add(AMenuItem);
	}

	PopupConn->Items->Insert(0, FMenuEditTransition);

	int iTransitionIndex = EditAppearance->MenuIndex;

	FMenuTransitionEnableTrigger = new TMenuItem(this);
	FMenuTransitionEnableTrigger->Caption = "Enable trigger";
	FMenuTransitionEnableTrigger->OnClick = OnMenuTransitionEnableTriggerClick;
	FMenuTransitionEnableTrigger->ImageIndex = 197;
	FMenuTransitionEnableTrigger->OnAdvancedDrawItem = CheckImageAdvancedDrawItem;
	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionEnableTrigger);

	FMenuTransitionSetBreakpoint = new TMenuItem(this);
	FMenuTransitionSetBreakpoint->Caption = "Breakpoint";
	FMenuTransitionSetBreakpoint->OnClick = OnMenuSetBreakpoint;
	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionSetBreakpoint);

	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionTextMove);
	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionTextAlign);
	PopupConn->Items->InsertNewLineAfter(FMenuTransitionTextAlign);
	iTransitionIndex++;

	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionConvertTo);
	PopupConn->Items->InsertNewLineAfter(FMenuTransitionConvertTo);
	iTransitionIndex++;

	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionInheritance);
	PopupConn->Items->Insert(++iTransitionIndex, FMenuCopyTransition);
	PopupConn->Items->Insert(++iTransitionIndex, FMenuSelfConnectionAlign);
	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionSwap);
	PopupConn->Items->Insert(++iTransitionIndex, AMenuTransitionFind);
	PopupConn->Items->InsertNewLineAfter(AMenuTransitionFind);
	iTransitionIndex++;

	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionSwitchSimple);
	PopupConn->Items->Insert(++iTransitionIndex, FMenuTransitionSwitchInvertCondition);
	PopupConn->Items->InsertNewLineAfter(FMenuTransitionSwitchInvertCondition);
	iTransitionIndex++;

	PopupConn->Items->Insert(++iTransitionIndex, FMenuConnectionOverTop);
	PopupConn->Items->Insert(++iTransitionIndex, FMenuConnectionOverLeft);
	PopupConn->Items->Insert(++iTransitionIndex, FMenuConnectionOverRight);
	PopupConn->Items->Insert(++iTransitionIndex, FMenuConnectionOverBottom);

	FMenuTransitionMovePoints = new TMenuItem(this);
	FMenuTransitionMovePoints->ImageIndex = 91;
	FMenuTransitionMovePoints->Caption = "Move all points";
	const UnicodeString AMoveConsts[pptMAXSIZE] = {
		L"Left", L"Up", L"Right", L"Down"
	};
	for (int i = pptLeft; i < pptMAXSIZE; i++) {
		TMenuItem *AMenuMovePointsItem = new TMenuItem(this);
		AMenuMovePointsItem->Caption = AMoveConsts[i] + " (" + "Ctrl+" + AMoveConsts[i] + ")";
		AMenuMovePointsItem->Tag = i;
		AMenuMovePointsItem->ImageIndex = 32 + i * 2;
		AMenuMovePointsItem->OnClick = OnMenuTransitionMovePoints;
		FMenuTransitionMovePoints->Add(AMenuMovePointsItem);
	}

	PopupConn->Items->Add(FMenuTransitionMovePoints);

	PopupPoint->Items->NewBottomLine();

	FMenuPointArrangeMirror = new TMenuItem(this);
	FMenuPointArrangeMirror->Caption = "Arrange - mirror";
	FMenuPointArrangeMirror->OnClick = OnMenuPointArrangeMirror;
	FMenuPointArrangeMirror->ImageIndex = 77;

	FMenuPointArrangeSingle = new TMenuItem(this);
	FMenuPointArrangeSingle->Caption = "Arrange - single";
	FMenuPointArrangeSingle->OnClick = OnMenuPointArrangeSingle;
	FMenuPointArrangeSingle->ImageIndex = 76;

	PopupPoint->Items->Add(FMenuPointArrangeMirror);
	PopupPoint->Items->Add(FMenuPointArrangeSingle);

	PopupPoint->Items->NewBottomLine();

	FMenuPointAlignOppositeVertically = new TMenuItem(this);
	FMenuPointAlignOppositeVertically->Caption = "Align opposite point - vertical";
	FMenuPointAlignOppositeVertically->OnClick = OnMenuPointAlignOppositeVerticallyClick;
	FMenuPointAlignOppositeVertically->ImageIndex = 104;
	PopupPoint->Items->Add(FMenuPointAlignOppositeVertically);

	FMenuPointAlignOppositeHorizontally = new TMenuItem(this);
	FMenuPointAlignOppositeHorizontally->Caption = "Align opposite point - horizontal";
	FMenuPointAlignOppositeHorizontally->OnClick = OnMenuPointAlignOppositeHorizontallyClick;
	FMenuPointAlignOppositeHorizontally->ImageIndex = 103;
	PopupPoint->Items->Add(FMenuPointAlignOppositeHorizontally);

	PopupPoint->Items->NewBottomLine();

	FMenuPointPin = new TMenuItem(this);
	FMenuPointPin->Caption = "Pin";
	FMenuPointPin->ImageIndex = 74;

	for (int i = 0; i < pptMAXSIZE; i++) {
		TMenuItem *AMenuPinItem = new TMenuItem(this);
		AMenuPinItem->Caption = g_PointPins[i];
		AMenuPinItem->Tag = i;
		AMenuPinItem->ImageIndex = 32 + i * 2;
		AMenuPinItem->OnClick = OnMenuPointPinClick;
		FMenuPointPin->Add(AMenuPinItem);
	}

	PopupPoint->Items->Add(FMenuPointPin);

	this->OnSelectingConnectionOnMouseUp = TheTreeSelectingConnectionOnMouseUp;

	TSpeedButton *ABtnEnableZoomHints = new TSpeedButton(this);
	ABtnEnableZoomHints->Parent = PanelTreeToolbar;
	ABtnEnableZoomHints->Align = alTop;
	ABtnEnableZoomHints->AlignWithMargins = true;
	ABtnEnableZoomHints->Top = SpeedButton10->BoundsRect.Bottom + 1;
	ABtnEnableZoomHints->Margins->Assign(SpeedButton10->Margins);
	ABtnEnableZoomHints->AllowAllUp = true;
	ABtnEnableZoomHints->GroupIndex = 15;
	ABtnEnableZoomHints->Flat = true;
	ABtnEnableZoomHints->ShowHint = true;
	if (FormScxmlGui) {
		ABtnEnableZoomHints->Action = FormScxmlGui->actEnableZoomHints;
		ABtnEnableZoomHints->Down = FormScxmlGui->actEnableZoomHints->Checked;
	}

	ActionConnectionHighlight->OnExecute = OnActionConnectionHighlightExecute;
	ActionConnectionHighlight->OnUpdate = OnActionConnectionHighlightUpdate;

	AppendImageToTreeImagePool(ImageList1, 52, g_TransitionSimpleIndex);
	AppendImageToTreeImagePool(ImageList1, 187, g_TransitionSelfIndex);
	AppendImageToTreeImagePool(ImageList1, 198, g_BreakpointActiveIndex);
	AppendImageToTreeImagePool(ImageList1, 199, g_BreakpointInactiveIndex);

#if 0 // внимательно при добавлении TreeImagePool, чтобы не получить утечку, так как это глобальный объект
	WLOG_DEBUG(L"TreeImagePool Count=%d", TreeImagePool->Count());
#endif

	AddAllShapeImages();

	FEditDocRaw->ColorSchemes->Assign(DModule->EditDoc1->ColorSchemes);
	FEditDocRaw->SyntaxSchemes->Assign(DModule->EditDoc1->SyntaxSchemes);
	FEditDocRaw->ActiveSyntaxScheme = L"XML";
	FEditDocRaw->ActiveColorScheme = L"default";

	FMenuConvertFromRaw = new TMenuItem(this);
	FMenuConvertFromRaw->Name = "MenuConvertFromRaw";
	FMenuConvertFromRaw->Caption = "Convert to visual SCXML";
	FMenuConvertFromRaw->OnClick = OnMenuConvertToRawClick;

	FSyntaxEditorEx->Parent = PanelTree;
	FSyntaxEditorEx->Visible = false;
	FSyntaxEditorEx->Align = alClient;
	FSyntaxEditorEx->EditView->Document = FEditDocRaw;
	FSyntaxEditorEx->Edit1->Add(FMenuConvertFromRaw);

	TabControl1->Tabs->Append(s_CAPTION_RAW_SCXML);
	TabControl1->Tabs->Append(s_CAPTION_HPP_SCXML);
	TabControl1->Tabs->Append(s_CAPTION_DFM);
	TabControl1->Tabs->Append(s_CAPTION_SVG);

	for (int i = 0; i < PageShapes->PageCount; i++) {
		if (PageShapes->Pages[i]->Caption == L"SCXML") {

			for (int k = 0; k < PageShapes->Pages[i]->ControlCount; k++) {
				TScxmlSpeedButton *AScxmlSpeedButton = dynamic_cast<TScxmlSpeedButton*>
					(PageShapes->Pages[i]->Controls[k]);
				if (AScxmlSpeedButton) {
					TTreeTagShape *ATreeTagShape = reinterpret_cast<TTreeTagShape*>(AScxmlSpeedButton->Tag);
					if (ATreeTagShape) {
						if (ATreeTagShape->AClass == __classid(TVirtualShape)) {
							AScxmlSpeedButton->Visible = StateMachineEditorUnit &&
								StateMachineEditorUnit->StateMachineProject;
							break;
						}
					}
				}
			}

			PageShapes->ActivePage = PageShapes->Pages[i];
			PageShapes->Pages[i]->PageIndex = 0;
			PageShapesChange(PageShapes);
			break;
		}
	}

	LoadEditorParametersEx();

	ButtonNew->Visible = false;
	ButtonOpen->Visible = false;
	ButtonSave->Visible = false;

	File1->Visible = false;

	if (IsStandalone && this->Owner == Application) {
		TMenuItem *AMenuIdeInsight = new TMenuItem(this);
		AMenuIdeInsight->Action = FormScxmlGui->actIDEInsight;
		AMenuIdeInsight->ImageIndex = -1;
		this->MainMenu1->Items->Add(AMenuIdeInsight);

		TRect ASettingsRect = Customutils::StrToRect
			(SettingsData->TempRegistry->Values[this->ClassName() + ".BoundsRect"], this->BoundsRect);
		TRect AResultRect;
		// проверяем, чтобы сохранённые в конфиге координаты не вылезли за пределы экрана
		if (IntersectRect(AResultRect, ASettingsRect, Screen->DesktopRect)) {
			SetWindowPos(this->Handle, NULL, ASettingsRect.left, ASettingsRect.top, ASettingsRect.Width(),
				ASettingsRect.Height(), SWP_ASYNCWINDOWPOS);
		}
		else {
			WLOG_WARNING(L"Config application rect <%s> does not intersect with desktop rect <%s>",
				SettingsData->TempRegistry->Values[this->ClassName() + ".BoundsRect"].c_str(),
				Customutils::RectToStr(Screen->DesktopRect).c_str());
		}

	}

	NodeTree->UseNativeDrag = false;
	NodeTree->OnDragDrop = NodeTreeDragDrop;
	NodeTree->OnDragOver = NodeTreeDragOver;
	NodeTree->OnMouseDown = NodeTreeMouseDown;
}

// ---------------------------------------------------------------------------
__fastcall TStateMachineEditor::~TStateMachineEditor() {
	if (SettingsData) {

		if (FPropSettingsInspector) {
			SettingsData->TempRegistry->Values[this->ClassName() + ".PropSettingsInspector.Width"]
				= FPropSettingsInspector->Width;
			SettingsData->TempRegistry->Values[this->ClassName() + ".PropSettingsInspector.Splitter"]
				= FPropSettingsInspector->Splitter;
		}

		if (IsStandalone) {
			SettingsData->TempRegistry->Values[this->ClassName() + ".BoundsRect"] = Customutils::RectToStr
				(this->BoundsRect);
		}
	}

	if (DesignObjects) {
		DesignObjects->Clear();
	}
	if (FDesignObjects) {
		delete FDesignObjects;
		FDesignObjects = 0;
	}
	if (FPropSettingsInspector) {
		delete FPropSettingsInspector;
		FPropSettingsInspector = 0;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::AddAllShapeImages(void) {
	try {
		this->AddShapeImageToMap<TOnEntryStateShape>();
		this->AddShapeImageToMap<TOnExitStateShape>();
		this->AddShapeImageToMap<TDatamodelStateShape>();
		this->AddShapeImageToMap<TDataChildShape>();
		this->AddShapeImageToMap<TAssignShape>();
		this->AddShapeImageToMap<TDonedataShape>();
		this->AddShapeImageToMap<TContentShape>();
		this->AddShapeImageToMap<TParamShape>();
		this->AddShapeImageToMap<TScriptShape>();
		this->AddShapeImageToMap<TRaiseShape>();
		this->AddShapeImageToMap<TIfShape>();
		this->AddShapeImageToMap<TElseIfShape>();
		this->AddShapeImageToMap<TElseShape>();
		this->AddShapeImageToMap<TForeachShape>();
		this->AddShapeImageToMap<TLogShape>();
		this->AddShapeImageToMap<TSendShape>();
		this->AddShapeImageToMap<TCancelShape>();
		this->AddShapeImageToMap<TInvokeShape>();
		this->AddShapeImageToMap<TFinalizeShape>();
		this->AddShapeImageToMap<TCommentShape>();
		this->AddShapeImageToMap<TExtraContentShape>();
		this->AddShapeImageToMap<TProtocolBindingShape>();
		this->AddShapeImageToMap<TProtocolWeakBindingShape>();
		this->AddShapeImageToMap<TSetValueShape>();
		this->AddShapeImageToMap<TSessionStateShape>();
		this->AddShapeImageToMap<TVirtualFolderShape>();
		this->AddShapeImageToMap<TTriggerShape>();
		this->AddShapeImageToMap<TWatchShape>();

		FShapeImagesMap["scxml"] = 191; // this->AddVisualShapeImageToMap<TScxmlShape>();
		FShapeImagesMap["state"] = 188; // this->AddVisualShapeImageToMap<TStateShape>();
		FShapeImagesMap["parallel"] = 189; // this->AddVisualShapeImageToMap<TParallelShape>();
		this->AddVisualShapeImageToMap<TInitialShape>();
		this->AddVisualShapeImageToMap<TFinalShape>();
		this->AddVisualShapeImageToMap<THistoryShape>();
		FShapeImagesMap["virtual"] = 190; // this->AddVisualShapeImageToMap<TVirtualShape>();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditor::GetIsStandalone(void) {
	return FStateMachineEditorUnit == NULL;
}

// ---------------------------------------------------------------------------
TTreeEx * __fastcall TStateMachineEditor::GetTheTreeEx(void) {
	TTreeEx *ATreeEx = dynamic_cast<TTreeEx*>(TheTree);
	if (!ATreeEx)
		throw Exception("PROGRAM LOGIC FAILT! TheTreeEx==NULL!!!");
	return ATreeEx;
}

// ---------------------------------------------------------------------------
TStateMachineDockPanel * __fastcall TStateMachineEditor::GetStateMachineDockPanel(void) {
	return dynamic_cast<TStateMachineDockPanel*>(this->Owner);
}

// ---------------------------------------------------------------------------
TPopupMenu * __fastcall TStateMachineEditor::GetPreparedPopupMenu(Teetree::TCustomTree* ATree, const int X,
	const int Y, const int iXScreen, const int iYScreen) {

	SpeedNormal->Down = True;

	// по умолчанию редактор фигуры
	TPopupMenu *tmpPopup = PopupNode;

	if (TheTree->Connections->Selected) {
		// Show connection popup, or connection point popup
		PopupPoint->Tag = TheTree->Connections->Selected->Points->Clicked(X, Y);
		if (PopupPoint->Tag != -1)
			tmpPopup = PopupPoint;
		else
			tmpPopup = PopupConn;
	}

	// редактор соединений и точек, только, если Popup вызыван из TheTree
	if (ATree == TheTree) {
		if (TextDummyShape) {
			tmpPopup = PopupDummyText;
			if (TextDummyShape->StateMachineConnection) {
				MenuDummyTextVerticalCentering->Checked = TextDummyShape->StateMachineConnection->VerticalCentering;
				MenuDummyTextRotateCustom->Caption = UnicodeString().sprintf(L"Rotate Custom (%d°)",
					TextDummyShape->StateMachineConnection->Text->Angle);
			}
		}
	}

	// Show menu
	tmpPopup->PopupComponent = ATree;

	OldCursorPos = TPoint(iXScreen, iYScreen); // Hold position of cursor before popupmenu

	return tmpPopup;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TryPopup(Teetree::TCustomTree* ATree, int x, int y, int xScreen, int yScreen) {
	TPopupMenu* APopup = GetPreparedPopupMenu(ATree, x, y, xScreen, yScreen);
	if (APopup) {
		APopup->Popup(xScreen, yScreen);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::CheckImageAdvancedDrawItem(TObject *Sender, TCanvas *ACanvas, const TRect &ARect,
	TOwnerDrawState State) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {
		TAdvancedMenuDrawItemEvent AHandler = AMenuItem->OnAdvancedDrawItem;
		AMenuItem->OnAdvancedDrawItem = NULL;

		DrawMenuItem(AMenuItem, ACanvas, ARect, State);

		if (AMenuItem->Checked) {
			ACanvas->Pen->Color = clBlue;
			ACanvas->Rectangle(ARect.Left, ARect.Top, ARect.Left + 21, ARect.Top + 21);
		}

		AMenuItem->OnAdvancedDrawItem = AHandler;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuDummyTextReset(System::TObject* Sender) {
	TTextDummyShape *ADummy = TextDummyShape;
	if (ADummy && ADummy->Connection) {
		ADummy->Connection->HorizTextAlign = htaCenter;
		ADummy->Connection->VertTextAlign = vtaCenter;
		ADummy->Connection->Text->HorizOffset = 0;
		ADummy->Connection->Text->VertOffset = 0;

		ADummy->Connection->Draw();
		ADummy->UpdatePosition();

		const UnicodeString sName = ADummy->Connection->Name;

		TeeModified(true, 1, L"Connection[" + sName + "] text was reseted!");

		if (PropSettingsInspector) {
			PropSettingsInspector->UpdateContent();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuDummyTextEdit(System::TObject* Sender) {
	TTextDummyShape *ADummy = TextDummyShape;
	if (ADummy && ADummy->StateMachineConnection) {
		ADummy->StateMachineConnection->ShowEditor();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuDummyTextAlign(System::TObject* Sender) {
	TTextDummyShape *ADummy = TextDummyShape;
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && ADummy && ADummy->Connection) {

		ADummy->Connection->Text->HorizOffset = 0;
		ADummy->Connection->Text->VertOffset = 0;

		if (ADummy->Connection->Points->Count() >= 2) {

			const int iIndexLast = ADummy->Connection->Points->Count() / 2;
			const int iIndexFirst = iIndexLast - 1;

			// устраняем баг в либе, когда перепутаны названия Лево-Право, Вверх-Вниз
			const bool bTrueTop = ADummy->Connection->Points->Item[iIndexFirst].Y < ADummy->Connection->Points->Item
				[iIndexLast].Y;

			const bool bTrueLeft = ADummy->Connection->Points->Item[iIndexFirst].X < ADummy->Connection->Points->Item
				[iIndexLast].X;

			switch(AMenuItem->Tag) {
			case pptLeft:
				ADummy->Connection->HorizTextAlign = bTrueLeft ? htaLeft : htaRight;
				ADummy->Connection->VertTextAlign = vtaCenter;
				break;
			case pptTop:
				ADummy->Connection->HorizTextAlign = htaCenter;
				ADummy->Connection->VertTextAlign = bTrueTop ? vtaTop : vtaBottom;
				break;
			case pptRight:
				ADummy->Connection->HorizTextAlign = bTrueLeft ? htaRight : htaLeft;
				ADummy->Connection->VertTextAlign = vtaCenter;
				break;
			case pptBottom:
				ADummy->Connection->HorizTextAlign = htaCenter;
				ADummy->Connection->VertTextAlign = bTrueTop ? vtaBottom : vtaTop;
				break;
			}
		}
		else {
			ADummy->Connection->HorizTextAlign = htaCenter;
			ADummy->Connection->VertTextAlign = vtaCenter;
		}

		const UnicodeString sName = ADummy->Connection->Name;

		ADummy->Connection->Draw();
		ADummy->UpdatePosition();

		TeeModified(true, 1, L"Connection[" + sName + "] text align was changed!");

		if (PropSettingsInspector) {
			PropSettingsInspector->UpdateContent();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuDummyTextEditCondition(System::TObject* Sender) {
	TTextDummyShape *ADummy = TextDummyShape;
	if (ADummy && ADummy->StateMachineConnection) {

		const UnicodeString sInstPropPath = Customutils::GetFullPropPath(ADummy->StateMachineConnection);
		if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, L"Condition")) {
			std::auto_ptr<TFormSyntaxEditorForm>AStrEditForm(new TFormSyntaxEditorForm(Application, SyntaxScheme,
					ADummy->StateMachineConnection->Condition, "Edit 'Condition'"));

			const int iModalResult = AStrEditForm->ShowModal();
			if (iModalResult == mrOk) {
				ADummy->StateMachineConnection->Condition = AStrEditForm->EditDoc1->Lines->Text.Trim();
			}
			if (iModalResult == mrOk || iModalResult == mrYes) {
				AStrEditForm->CreateSyntaxDockPanel(sInstPropPath, "Condition");
			}
		}

		const UnicodeString sName = ADummy->Connection->Name;
		ADummy->Connection->Draw();
		ADummy->UpdatePosition();

		TeeModified(true, 1, L"Connection[" + sName + "] 'Condition' was changed!");

		if (PropSettingsInspector) {
			PropSettingsInspector->UpdateContent();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuDummyTextRotate90Left(System::TObject* Sender) {
	TTextDummyShape *ADummy = TextDummyShape;
	if (ADummy && ADummy->StateMachineConnection) {

		int iAngle = ADummy->StateMachineConnection->Text->Angle + 90;
		if (iAngle >= 360) {
			iAngle -= 360;
		}

		ADummy->StateMachineConnection->Text->Angle = iAngle;
		ADummy->StateMachineConnection->Draw();
		ADummy->UpdatePosition();

		TeeModified(true, 1, L"Connection[" + ADummy->StateMachineConnection->Name + "] Rotated to " + UnicodeString
			(iAngle) + "°!");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuDummyTextRotate90Right(System::TObject* Sender) {
	TTextDummyShape *ADummy = TextDummyShape;
	if (ADummy && ADummy->StateMachineConnection) {

		int iAngle = ADummy->StateMachineConnection->Text->Angle - 90;
		if (iAngle < 0) {
			iAngle += 360;
		}

		ADummy->StateMachineConnection->Text->Angle = iAngle;
		ADummy->StateMachineConnection->Draw();
		ADummy->UpdatePosition();

		TeeModified(true, 1, L"Connection[" + ADummy->StateMachineConnection->Name + "] Rotated to " + UnicodeString
			(iAngle) + "°!");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuDummyTextRotateCustom(System::TObject* Sender) {
	TTextDummyShape *ADummy = TextDummyShape;
	if (ADummy && ADummy->StateMachineConnection) {

		UnicodeString sAngle(ADummy->StateMachineConnection->Text->Angle);
		if (InputQuery("EDIT TEXT ANGLE", "Enter value from 0 to 359°", sAngle)) {
			const int iAngle = sAngle.ToIntDef(-1);
			if (iAngle < 0 || iAngle >= 360) {
				WLOG_ERROR(L"Enter value from 0 to 359°! Current:[%s]", sAngle.c_str());
				return;
			}

			ADummy->StateMachineConnection->Text->Angle = iAngle;
			ADummy->StateMachineConnection->Draw();
			ADummy->UpdatePosition();

			TeeModified(true, 1, L"Connection[" + ADummy->StateMachineConnection->Name + "] Rotated to " + UnicodeString
				(iAngle) + "°!");
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuDummyTextVerticalCentering(System::TObject* Sender) {
	TTextDummyShape *ADummy = TextDummyShape;
	if (ADummy && ADummy->StateMachineConnection) {

		ADummy->StateMachineConnection->VerticalCentering = !ADummy->StateMachineConnection->VerticalCentering;
		ADummy->StateMachineConnection->Draw();
		ADummy->UpdatePosition();

		TeeModified(true, 1, L"Connection[" + ADummy->StateMachineConnection->Name + "] VerticalCentering:[" + BoolToStr
			(ADummy->StateMachineConnection->VerticalCentering, true) + "]!");

		if (PropSettingsInspector) {
			PropSettingsInspector->UpdateContent();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuClone(TObject *Sender) {
	if (TextDummyShape)
		return;

	TTreeEditorEx::OnMenuClone(Sender);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuChartVersion(System::TObject* Sender) {

	unsigned __int64 ATreeVersion = Customutils::ConvertVersionToNumber(TheTreeEx->ApplicationVersion);
	unsigned __int64 AppVersion = Customutils::ConvertVersionToNumber(SettingsData->AppVersion);

	WLOG_INFO(L"AppVersion:[%s] %s ChartVersion:[%s]", SettingsData->AppVersion.c_str(), //
		AppVersion == ATreeVersion ? L"==" : (AppVersion > ATreeVersion ? L">" : L"<"), //
		ATreeVersion == 0 ? L"UNVERSIONED" : TheTreeEx->ApplicationVersion.c_str());

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuChartStatistics(System::TObject* Sender) {
	FormScxmlGui->SwitchToTesterLog(true);

	FormScxmlGui->EditDocTester->Lines->Append("***** State Chart Statistics *****");

	FormScxmlGui->EditDocTester->Lines->Append
		(UnicodeString().sprintf(L"TotalBounds Left=%d Top=%d Width=%d Height=%d", TheTree->TotalBounds.left,
			TheTreeEx->TotalBounds.top, TheTree->TotalBounds.Width(), TheTreeEx->TotalBounds.Height()));

	FormScxmlGui->EditDocTester->Lines->Append(UnicodeString().sprintf(L"Total Shapes Count=%d",
			TheTree->Shapes->Count));

	FormScxmlGui->EditDocTester->Lines->Append(UnicodeString().sprintf(L"Total Connections Count=%d",
			TheTree->Connections->Count));

	FormScxmlGui->EditDocTester->Lines->Append("");

	std::map<TStateChildType, int>AMapStats;
	for (std::size_t i = 0; i < ARRAYSIZE(__STATE_TYPES_VISUAL_SHAPES); i++) {
		const TStateChildType AStateChildType = __STATE_TYPES_VISUAL_SHAPES[i];
		AMapStats.insert(std::make_pair(AStateChildType, 0));
	}

	int iTotalStates = 0;
	for (int i = 0; i < TheTree->Shapes->Count; i++) {
		TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Shapes->Items[i]);
		if (AScxmlBaseShape) {
			if (STATE_TYPES_VISUAL_SHAPES.find(AScxmlBaseShape->StateChildType) != STATE_TYPES_VISUAL_SHAPES.end()) {
				AMapStats[AScxmlBaseShape->StateChildType] = AMapStats[AScxmlBaseShape->StateChildType] + 1;
				iTotalStates++;
			}
		}
	}

	FormScxmlGui->EditDocTester->Lines->Append("****** Total StateChart States=[" + UnicodeString(iTotalStates)
		+ "] ******");
	for (std::size_t i = 0; i < ARRAYSIZE(__STATE_TYPES_VISUAL_SHAPES); i++) {
		const TStateChildType AStateChildType = __STATE_TYPES_VISUAL_SHAPES[i];
		const int iElementCount = AMapStats[AStateChildType];
		if (iElementCount != 0) {
			const UnicodeString sElement = TScxmlBaseShape::TypeToXMLNodeName(AStateChildType);
			FormScxmlGui->EditDocTester->Lines->Append(UnicodeString().sprintf(L"\t* <%s>=%d", sElement.c_str(),
					iElementCount));
		}
	}

	FormScxmlGui->EditDocTester->Lines->Append("");

	int iSelfConnectionsStats = 0;
	int iNotSelfConnectionsStats = 0;

	for (int i = 0; i < TheTree->Connections->Count; i++) {
		TStateMachineConnection *AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
			(TheTree->Connections->Items[i]);
		if (AStateMachineConnection) {
			if (AStateMachineConnection->IsSelfConnection) {
				iSelfConnectionsStats++;
			}
			else {
				iNotSelfConnectionsStats++;
			}
		}
	}

	const int iTotalTransitions = iSelfConnectionsStats + iNotSelfConnectionsStats;
	FormScxmlGui->EditDocTester->Lines->Append("****** Total StateChart Transitions=[" + UnicodeString
		(iTotalTransitions) + "] ******");
	FormScxmlGui->EditDocTester->Lines->Append(UnicodeString().sprintf(L"\t* Self-Transitions=%d",
			iSelfConnectionsStats));
	FormScxmlGui->EditDocTester->Lines->Append(UnicodeString().sprintf(L"\t* Other Transitions=%d",
			iNotSelfConnectionsStats));
}

// ---------------------------------------------------------------------------
bool TStateMachineEditor::WasEnteredShape(TVisualScxmlBaseShape *AShape) {
	return SettingsData->IsTestCoverageEnabled() && (FEnteredDebugShapes.find(AShape) != FEnteredDebugShapes.end());
}

// ---------------------------------------------------------------------------
bool TStateMachineEditor::WasEnteredConnection(TStateMachineConnection *AConnection) {
	return SettingsData->IsTestCoverageEnabled() && //
	(FEnteredStateMachineConnections.find(AConnection) != FEnteredStateMachineConnections.end());
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ClearWasEnteredElements(void) {
	FEnteredDebugShapes.clear();
	FEnteredStateMachineConnections.clear();
}

// ---------------------------------------------------------------------------
TScxmlBaseShape *__fastcall TStateMachineEditor::GetFirstSelected(void) {
	return dynamic_cast<TScxmlBaseShape*>(GetSafeTreeListFirst(TheTree->Selected->Shapes));
}

// ---------------------------------------------------------------------------
TTextDummyShape *__fastcall TStateMachineEditor::GetTextDummyShape(void) {
	return dynamic_cast<TTextDummyShape*>(GetSafeTreeListFirst(TheTree->Selected->Shapes));
}

// ---------------------------------------------------------------------------
TScxmlShape * __fastcall TStateMachineEditor::GetRootScxml(void) {
	if (TheTree) {
		for (int i = 0; i < TheTree->Shapes->Count; i++) {
			TScxmlShape * AScxmlShape = dynamic_cast<TScxmlShape*>(TheTree->Shapes->Items[i]);
			if (AScxmlShape) {
				return AScxmlShape;
			}
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineEditor::GetScxmlName() {
	TScxmlShape *ARootScxml = GetRootScxml();
	return ARootScxml ? ARootScxml->SimpleText : UnicodeString(L"");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineEditor::GetUnitFileName(void) {
	return StateMachineEditorUnit ? StateMachineEditorUnit->FileName : UnicodeString(L"");
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::MarkModified(int iCategory, const UnicodeString &sMsg, bool bFillNodeTree) {
	if (bFillNodeTree) {
		this->FillNodeTree();
	}
	this->TeeModified(true, iCategory, sMsg);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::PropSettingsInspectorChange(TObject *Sender) {
	TeeModified(true, 1, "Properties changed!");
}

// ---------------------------------------------------------------------------
void TStateMachineEditor::ResetInheritance() {
	for (int i = 0; i < TheTree->Connections->Count; i++) {
		TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
			(TheTree->Connections->Items[i]);
		if (AStateMachineConnection) {
			Statemachine::SetInheritance(AStateMachineConnection, false);
		}
	}
	for (int i = 0; i < TheTree->Shapes->Count; i++) {
		TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Shapes->Items[i]);
		if (AScxmlBaseShape) {
			Statemachine::SetInheritance(AScxmlBaseShape, false, false);
		}
	}
	this->InvalidateTheTreeWithTemp();
	TeeModified(true, 1, "Inheritance was reseted!");
}

// ---------------------------------------------------------------------------
void TStateMachineEditor::SetNewInheritance() {
	for (int i = 0; i < TheTree->Connections->Count; i++) {
		TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
			(TheTree->Connections->Items[i]);
		if (AStateMachineConnection) {
			AStateMachineConnection->Locked = true;
			AStateMachineConnection->InheritanceMismatch = false;
		}
	}
	for (int i = 0; i < TheTree->Shapes->Count; i++) {
		TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Shapes->Items[i]);
		if (AScxmlBaseShape) {
			AScxmlBaseShape->Locked = true;
			AScxmlBaseShape->InheritanceMismatch = false;
		}
	}
	this->InvalidateTheTreeWithTemp();
	TeeModified(true, 1, "Inheritance was set!");
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ClearDesignObjects(void) {
	if (DesignObjects && DesignObjects->Count) {
		SaveInspectorState();
		DesignObjects->Clear();
	}

	if (FormScxmlGui) {
		FormScxmlGui->HTMLPropInfo->Items->Clear();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ClearFlags(void) {
	/* inherited */
	TTreeEditorEx::ClearFlags();

	FInheritanceCheckShapes.clear();
	FInheritanceCheckConnections.clear();
	FUpdateNodeTreeElements.clear();

	FMouseWheelEventTimedFlag = std::make_pair(false, Now());
	FTreeModifiedForInheritanceFlag = false;
	FTreeNewConnectionToolbarFlag = false;
	FTreeValidateSimpleTextFlag = false;
	FTreeFillNodeTreeFlag = false;
	FTreeSelectionChanged = false;
	FRestoreInspectorStateFlag = false;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveInspectorState(void) {
	FPropInspStateObjectPtr.reset(PropSettingsInspector->SaveState());
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::RestoreInspectorState(void) {
	if (FPropInspStateObjectPtr.get()) {
		try {
			PropSettingsInspector->BeginUpdate();
			// необходимо предварительно свернуть все категории
			// так как при восстановлении только разворачиваются узлы
			PropSettingsInspector->CollapseAllCategories();
			PropSettingsInspector->RestoreState(FPropInspStateObjectPtr.get(), false);
		}
		__finally {
			PropSettingsInspector->EndUpdate();
		}

		// проверяем, назначен ли вообще OnClick
		if (PropSettingsInspector->OnClick) {
			PropSettingsInspector->OnClick(PropSettingsInspector);
		}

	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnActionConnectionHighlightExecute(TObject *Sender) {
	if (SettingsData) {
		SettingsData->ConnectionAdditionalHighlight = !SettingsData->ConnectionAdditionalHighlight;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnActionConnectionHighlightUpdate(TObject *Sender) {
	if (SettingsData) {
		ActionConnectionHighlight->Checked = SettingsData->ConnectionAdditionalHighlight;
		SpeedButtonConnectionHighlight->Down = SettingsData->ConnectionAdditionalHighlight;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuConvertToRawClick(TObject *Sender) {
	try {
		FLockClearUndo = true;

		try {
			if (TabControl1->TabIndex != -1) {
#if 0
				if (this->IsModified()) {
					const UnicodeString Caption = L"WARNING!";
					const UnicodeString Text = L"You have unsaved modifications!\n" //
					"This operation may lead to lose them!\n" //
					"Are you sure to continue?";
					if (Application->MessageBoxA(Text.c_str(), Caption.c_str(),
							MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) != IDOK)
						return;
				}
#endif

				if (TabControl1->Tabs->Strings[TabControl1->TabIndex] == s_CAPTION_RAW_SCXML) {
					LoadScxmlData(FSyntaxEditorEx->EditView->Document->Lines->Text, CurrentFile);
				}
				else if (TabControl1->Tabs->Strings[TabControl1->TabIndex] == s_CAPTION_DFM) {

					std::auto_ptr<TMemoryStream>AInStreamPtr(new TMemoryStream());
					std::auto_ptr<TMemoryStream>AOutStreamPtr(new TMemoryStream());

					FSyntaxEditorEx->EditView->Document->Lines->SaveToStream(AInStreamPtr.get());

					AInStreamPtr->Position = 0;

					ObjectTextToBinary(AInStreamPtr.get(), AOutStreamPtr.get());

					LoadScxmlStream(AOutStreamPtr.get(), CurrentFile);
				}
				else if (TabControl1->Tabs->Strings[TabControl1->TabIndex] == s_CAPTION_SVG) {
					throw Exception(L"SVG is not supported!");
				}
			}
		}
		catch(Exception * E) {
			WLOG_ERROR(L"Conversion from raw> %s", E->Message.c_str());
		}
	}
	__finally {
		FLockClearUndo = false;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ConvertScxmlDataToDesign(const UnicodeString &sScxmlData) {
	try {
		FLockClearUndo = true;

		LoadScxmlData(sScxmlData, CurrentFile);

		this->MarkModified(1, "Manual Edit", true);
	}
	__finally {
		FLockClearUndo = false;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnActionMakeSelfConnectionUpdate(TObject *Sender) {
	TAction * AAction = dynamic_cast<TAction*>(Sender);
	if (AAction) {
		TVisualScxmlBaseShape *ASelectedShape = dynamic_cast<TVisualScxmlBaseShape*>(this->FirstSelected);
		if (ASelectedShape) {
			AAction->Enabled = STATE_TYPES_SELFCONNECTION_SHAPES.find(ASelectedShape->StateChildType)
				!= STATE_TYPES_SELFCONNECTION_SHAPES.end();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuMakeSelfConnection(TObject *Sender) {
	try {
		TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(this->FirstSelected);
		if (AVisualShape && //
			STATE_TYPES_SELFCONNECTION_SHAPES.find(AVisualShape->StateChildType)
			!= STATE_TYPES_SELFCONNECTION_SHAPES.end()) {
			TheTree->Selected->Clear();

			TStateMachineConnection * ANewConnection = new TStateMachineConnection(this);
			const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, __classid(TStateMachineConnection));
			ANewConnection->Name = AUniqueName.second;
			ANewConnection->FromShape = AVisualShape;
			ANewConnection->ToShape = AVisualShape;

			ANewConnection->Tree = TheTree;

			ANewConnection->Style = csLine;

			ANewConnection->MakeSelfConnection();

			TheTree->Connections->Selected = ANewConnection;

			FillNodeTree();
			TeeModified(True, 1, "Make self connection:" + AVisualShape->SimpleText);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuClearConnectionClipboardMap(TObject *Sender) {
	SettingsData->ClipboardConnectionsMap.clear();
	LOG_INFO_SS << "Clipboard connections are cleared!";
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionCopy(TObject *Sender) {
	TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
		(TheTree->Connections->Selected);
	if (TheTree && AStateMachineConnection) {

		const UnicodeString sClipboardText = AStateMachineConnection->ToClipboardText();

		try {
			TClipboard* AClipboard = Clipboard();

			if (!AClipboard)
				throw Exception("Clipboard()==NULL!!!");

			AClipboard->Clear();
			AClipboard->AsText = sClipboardText;
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}

		SettingsData->ClipboardConnectionsMap.insert
			(std::make_pair(AStateMachineConnection->Name + "=" + AStateMachineConnection->SimpleText,
				std::make_pair(AStateMachineConnection->IsSelfConnection, sClipboardText)));
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuPasteConnection(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			const UnicodeString sKey = StripHotkey(AMenuItem->Caption);
			TMapConnections::iterator it = SettingsData->ClipboardConnectionsMap.find(sKey);
			if (it == SettingsData->ClipboardConnectionsMap.end())
				throw Exception("Can not find [" + sKey + "]!");

			if (IsStateMachineConnectionClipboardFormat(it->second.second)) {
				PasteStateMachineConnectionFromText(TheTree, it->second.second);
				TeeModified(True, 1, "Paste connection:" + sKey);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

}

// ---------------------------------------------------------------------------
TOperationResult __fastcall TStateMachineEditor::ReloadInheritedTree(bool bCheckWasModified /* = false */ ) {
	TOperationResult AResult = oresSuccess;

	try {
		if (StateMachineEditorUnit && StateMachineEditorUnit->IsInherited) {

			if (bCheckWasModified && this->IsModified()) {

				if (this->StateMachineDockPanel) {
					this->StateMachineDockPanel->Show();
				}

				const UnicodeString Caption = L"WARNING";
				const UnicodeString Text = L"This operation will reload [" + StateMachineEditorUnit->DisplayName +
					"]!\n" //
				"All unsaved modifications will be lost!\n" //
				"Are you sure to continue?";
				if (MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(),
						MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) != IDOK) {
					WLOG_ERROR(L"Inherited Unit:[%s] has unsaved modifications",
						StateMachineEditorUnit->DisplayName.c_str());
					return oresCancel;
				}
			}

			FIsReloadingTree = true;

			if (StateMachineDockPanel) {
				SendMessage(StateMachineDockPanel->Handle, WM_SETREDRAW, 0, 0);
				StateMachineDockPanel->DisableAlign();
			}

			try {

				this->LoadTree(StateMachineEditorUnit->FilePath);
			}
			__finally {
				if (StateMachineDockPanel) {
					StateMachineDockPanel->EnableAlign();
					SendMessage(StateMachineDockPanel->Handle, WM_SETREDRAW, 1, 0);
				}
			}

			// нормальная загрузка
			if (Statemachine::LastError.IsEmpty()) {
				this->TeeModified(false);
			}
			else {
				// возможно какие-то ошибки при загрузке
				this->TeeModified(true, 1, Statemachine::LastError);
				AResult = oresError;
			}

			this->Invalidate();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
		AResult = oresError;
	}

	FIsReloadingTree = false;

	return AResult;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuInheritanceDumpDifference(TObject *Sender) {
	try {
		TStateMachineEditor *AInheritedEditor = StateMachineDockPanel ? StateMachineDockPanel->InheritedEditor : NULL;
		if (AInheritedEditor) {

			std::auto_ptr<TStringList>AMismatchListPtr(new TStringList());

			if (TStateMachineConnection * ACheckConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected)) {
				if (TStateMachineConnection * AOriginConnection =
					Editorutils::FindConnectionByName<TStateMachineConnection>(AInheritedEditor->TheTree,
						ACheckConnection->Name)) {

					Unique::DeepCompareInheritedObjects(AOriginConnection, ACheckConnection,
						ACheckConnection->InheritanceResolver->DefaultSkippedProps, AMismatchListPtr.get());

					TConnectionPointArray ASourcePoints;
					Editorutils::RememberPoints(AOriginConnection, ASourcePoints);

					TConnectionPointArray ATargetPoints;
					Editorutils::RememberPoints(ACheckConnection, ATargetPoints);

					if (!Editorutils::AreEqualPoints(ASourcePoints, ATargetPoints)) {
						AMismatchListPtr->Add(L"Transition:[" + ACheckConnection->Name + L"] points position mismatch");
					}
				}
			}
			else {
				for (int i = 0; i < TheTree->Selected->Count(); i++) {
					if (TScxmlBaseShape * ACheckShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i])) {
						if (TScxmlBaseShape * AOriginShape = Editorutils::FindShapeByName<TScxmlBaseShape>
							(AInheritedEditor->TheTree, ACheckShape->Name)) {

							Unique::DeepCompareInheritedObjects(AOriginShape, ACheckShape,
								ACheckShape->InheritanceResolver->DefaultSkippedProps, AMismatchListPtr.get());

							if (!Unique::CompareParentsName(AOriginShape, ACheckShape)) {
								AMismatchListPtr->Add(L"Shape:[" + ACheckShape->Name + "] parent mismatch");
							}
						}
					}
				}
			}

			if (AMismatchListPtr->Count) {
				GlobalSendBallonMsg(AMismatchListPtr->Strings[0] + "\nSee log for details!", bfInfo);
				WLOG_WARNING(L"INHERITANCE> Dump mismatched properties:\n%s", AMismatchListPtr->Text.c_str());
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuRevertRecursive(TObject *Sender) {
	using namespace Editorutils;
	try {
		if (StateMachineEditorUnit && StateMachineEditorUnit->IsInherited) {
			if (AreAllSelectedBrothers()) {

				TStateMachineEditor *AInheritedEditor = FindStateMachineEditorByFileName
					(StateMachineEditorUnit->Inherited);

				if (!AInheritedEditor)
					throw Exception("Can not find inherited module [" + StateMachineEditorUnit->Inherited + "]!");

				std::auto_ptr<TStringList>AOutListPtr(new TStringList());
				std::set<TScxmlBaseShape*>AThisShapes;
				std::set<TTreeNodeShape*>AOriginShapes;

				TTreeNodeShape *AParent = NULL;

				TConnectionFromToMap AWasConnections;

				for (int i = 0; i < TheTree->Selected->Count(); i++) {
					TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
					if (AScxmlBaseShape && AScxmlBaseShape->Locked) {
						AThisShapes.insert(AScxmlBaseShape);

						CollectInOutConnections(AScxmlBaseShape, AWasConnections);

						if (!AParent) {
							AParent = AScxmlBaseShape->Parent;
						}

						if (TTreeNodeShape * AOriginShape = FindShapeByName<TTreeNodeShape>(AInheritedEditor->TheTree,
								AScxmlBaseShape->Name)) {
							AOriginShapes.insert(AOriginShape);
						}
					}
				}

				// очищаем все связи, которые были
				for (TConnectionFromToMap::iterator it = AWasConnections.begin(); it != AWasConnections.end(); ++it) {
					delete it->first;
				}

				TheTree->Selected->Clear();

				// удаляем предыдущие
				for (std::set<TScxmlBaseShape*>::iterator it = AThisShapes.begin(); it != AThisShapes.end(); ++it) {
					AOutListPtr->Add((*it)->SimpleText);
					delete(*it);
				}

				std::set<TTreeNodeShape*>ANewShapes;
				CloneBrotherShapes(this, AInheritedEditor->TheTree, TheTree, AParent, AOriginShapes, ANewShapes);

				// получаем список всех связей у базовых соединений
				TConnectionNameFromToMap ANamedConnections;
				for (std::set<TTreeNodeShape*>::iterator it = AOriginShapes.begin(); it != AOriginShapes.end(); ++it) {
					CollectInOutNamedConnections(*it, ANamedConnections, __classid(TStateMachineConnection));
				}

				RevertInOutNamedConnections(AInheritedEditor->TheTree, this->TheTree, ANamedConnections);

				for (std::set<TTreeNodeShape*>::iterator it = ANewShapes.begin(); it != ANewShapes.end(); ++it) {
					Statemachine::SetInheritance(dynamic_cast<TScxmlBaseShape*>(*it), true);
					TheTree->Selected->Add(*it);
				}

				if (AOutListPtr->Count) {
					TheTree->Invalidate();
					FillNodeTree(); // так как может поменяться родитель
					TeeModified(True, 1, "Reverted shapes: [" + Customutils::ClippedText(AOutListPtr->CommaText,
							50) + "]");
				}
			}

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuRevertInOutConnections(TObject *Sender) {
	try {
		if (StateMachineEditorUnit && StateMachineEditorUnit->IsInherited) {
			TStateMachineEditor *AInheritedEditor = FindStateMachineEditorByFileName(StateMachineEditorUnit->Inherited);

			if (!AInheritedEditor)
				throw Exception("Can not find inherited module [" + StateMachineEditorUnit->Inherited + "]!");

			std::auto_ptr<TStringList>AOutListPtr(new TStringList());

			using namespace Editorutils;

			TConnectionNameFromToMap ANamedConnections;
			for (int i = 0; i < TheTree->Selected->Count(); i++) {
				TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
				if (AScxmlBaseShape && AScxmlBaseShape->Locked) {
					CollectInOutNamedConnections(AScxmlBaseShape, ANamedConnections,
						__classid(TStateMachineConnection));
				}
			}

			RevertInOutNamedConnections(AInheritedEditor->TheTree, this->TheTree, ANamedConnections);

			if (AOutListPtr->Count) {
				TheTree->Invalidate();
				TeeModified(True, 1, "Reverted I/O connections:" + Customutils::ClippedText(AOutListPtr->CommaText,
						50));
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuRevertInInheritedUnits(TObject *Sender) {
	try {
		if (!StateMachineEditorUnit)
			return;

		std::vector<TStateMachineEditorUnit*>AVecUnits = StateMachineEditorUnit->GetInheritedUnitsFromThis();
		if (AVecUnits.size() == 0)
			return;

		// будет определено в 1 юните
		UnicodeString sInheritanceMenuCaption = L"";

		const UnicodeString sIgnoredCaption = TheTree->Connections->Selected ? //
		StripHotkey(FMenuTransitionRevertInInheritedUnits->Caption) : //
		StripHotkey(FMenuRevertInInheritedUnits->Caption);

		for (std::size_t i = 0; i < AVecUnits.size(); i++) {

			if (!AVecUnits[i]->Opened) {
				AVecUnits[i]->Open(true);
			}

			if (!AVecUnits[i]->StateMachineDockPanel)
				throw Exception(AVecUnits[i]->DisplayName + "> StateMachine is NULL!");

			AVecUnits[i]->StateMachineDockPanel->Show();

			TCustomTree *ATargetTree = AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->TheTree;

			TMenuItem *AInheritanceMenu = TheTree->Connections->Selected ? //
			AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->FMenuTransitionInheritance : //
			AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->FMenuInheritance;

			// выберем удаленную операцию на 1 юните
			if (i == 0) {
				std::auto_ptr<TDialogTreeSelect>ADialogTreeSelect(new TDialogTreeSelect(this));

				const UnicodeString sLastSelected = SettingsData->TempRegistry->Values
					["MenuRevertInInheritedUnits.LastSelected"];

				for (int n = 0; n < AInheritanceMenu->Count; n++) {
					if (!AInheritanceMenu->Items[n]->IsLine()) {
						const UnicodeString sMenuCaption = StripHotkey(AInheritanceMenu->Items[n]->Caption);
						if (sMenuCaption.Pos("Revert") && //
							// обязательно через StripHotkey !
							sMenuCaption != sIgnoredCaption) {

							const int iImageIndex = ADialogTreeSelect->ImageList1->AddImage(PopupNode->Images,
								AInheritanceMenu->Items[n]->ImageIndex);
							TTreeNode *ATreeNode = ADialogTreeSelect->TreeView1->Items->Add(NULL, sMenuCaption);
							ATreeNode->ImageIndex = iImageIndex;
							ATreeNode->SelectedIndex = iImageIndex;

							ATreeNode->Selected = sMenuCaption == sLastSelected;
						}
					}
				}

				// нет смысла для 1 элемента делать выбор
				if (ADialogTreeSelect->TreeView1->Items->Count == 1) {
					sInheritanceMenuCaption = ADialogTreeSelect->TreeView1->Items->Item[0]->Text;
				}
				else {
					if (ADialogTreeSelect->ShowModal() == mrOk && ADialogTreeSelect->TreeView1->Selected) {
						sInheritanceMenuCaption = ADialogTreeSelect->TreeView1->Selected->Text;
						SettingsData->TempRegistry->Values["MenuRevertInInheritedUnits.LastSelected"]
							= sInheritanceMenuCaption;
					}
					else {
						break;
					}
				}

			}

			// ничего не было выбрано юзером
			if (sInheritanceMenuCaption.IsEmpty()) {
				break;
			}

			TMenuItem *ATargetMenuItem = AInheritanceMenu->Find(sInheritanceMenuCaption);
			if (ATargetMenuItem) {

				if (TheTree->Connections->Selected) {

					TStateMachineConnection *ATargetConnection =
						Editorutils::FindConnectionByName<TStateMachineConnection>(ATargetTree,
						TheTree->Connections->Selected->Name);
					if (ATargetConnection) {
						AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->SelectConnection(ATargetConnection,
							true);
					}
				}
				else {

					const int iSelectedCount = TheTree->Selected->Count();
					if (iSelectedCount == 0)
						break;

					for (int k = 0; k < iSelectedCount; k++) {
						TScxmlBaseShape *ATargetShape = Editorutils::FindShapeByName<TScxmlBaseShape>(ATargetTree,
							TheTree->Selected->Items[k]->Name);
						if (ATargetShape) {
							AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->SelectMultipleShape(ATargetShape,
								k == 0, k == iSelectedCount);
						}
					}

				}

				ATargetMenuItem->OnClick(ATargetMenuItem);

				// снимаем флаг о том, что необходимо перегрузить, иначе будет выдавать ошибки об изменении
				AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->FTreeModifiedForInheritanceFlag = false;
			}
			else {
				WLOG_WARNING(L"Menu:[%s] is not found in unit:[%s]", sInheritanceMenuCaption.c_str(),
					AVecUnits[i]->DisplayName.c_str());
			}

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuRevertToInheritedProps(TObject *Sender) {
	try {
		using namespace Editorutils;

		if (StateMachineEditorUnit && StateMachineEditorUnit->IsInherited) {
			TStateMachineEditor *AInheritedEditor = FindStateMachineEditorByFileName(StateMachineEditorUnit->Inherited);

			if (!AInheritedEditor)
				throw Exception("Can not find inherited module [" + StateMachineEditorUnit->Inherited + "]!");

			if (TheTree->Connections->Selected) {
				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
					(TheTree->Connections->Selected);
				if (AStateMachineConnection && AStateMachineConnection->Locked) {
					TStateMachineConnection *ASource = Editorutils::FindConnectionByName<TStateMachineConnection>
						(AInheritedEditor->TheTree, AStateMachineConnection->Name);
					if (!ASource) {
						if (AStateMachineConnection->Locked) {
							AStateMachineConnection->InheritanceMismatch = true;
						}
						AStateMachineConnection->Locked = false;
						throw Exception(L"INHERITANCE> [" + AStateMachineConnection->Name +
							"] Can not find origin connection!");
					}

					if (!ASource->FromShape || !ASource->ToShape) {
						if (AStateMachineConnection->Locked) {
							AStateMachineConnection->InheritanceMismatch = true;
						}
						AStateMachineConnection->Locked = false;
						throw Exception(L"INHERITANCE> [" + AStateMachineConnection->Name +
							"] FromShape==NULL and ToShape==NULL !");

					}

					TScxmlBaseShape *AFromShape = FindShapeByName<TScxmlBaseShape>(TheTree, ASource->FromShape->Name);
					if (!AFromShape) {
						if (AStateMachineConnection->Locked) {
							AStateMachineConnection->InheritanceMismatch = true;
						}
						AStateMachineConnection->Locked = false;
						throw Exception
							(L"INHERITANCE> [" + AStateMachineConnection->Name + "] FromShape:[" +
							ASource->FromShape->Name + "] is not found!");
					}

					TScxmlBaseShape *AToShape = FindShapeByName<TScxmlBaseShape>(TheTree, ASource->ToShape->Name);
					if (!AToShape) {
						if (AStateMachineConnection->Locked) {
							AStateMachineConnection->InheritanceMismatch = true;
						}
						AStateMachineConnection->Locked = false;
						throw Exception
							(L"INHERITANCE> [" + AStateMachineConnection->Name + "] ToShape:[" +
							ASource->ToShape->Name + "] is not found!");
					}

					TConnectionPointArray AConnSelectedPoints;
					Editorutils::RememberPoints(ASource, AConnSelectedPoints);

					AStateMachineConnection->Assign(ASource);
					AStateMachineConnection->FromShape = AFromShape;
					AStateMachineConnection->ToShape = AToShape;

					Editorutils::RestorePoints(AStateMachineConnection, AConnSelectedPoints);

					AStateMachineConnection->Locked = true;
					AStateMachineConnection->InheritanceMismatch = false;

					FillNodeTree();
					TeeModified(True, 1, "Reverted:" + AStateMachineConnection->SimpleText);

					if (PropSettingsInspector) {
						PropSettingsInspector->UpdateContent();
					}

					TransitionXMLEditor->SetStateMachineConnection(AStateMachineConnection);
				}
			}
			else {
				std::auto_ptr<TStringList>AOutListPtr(new TStringList());
				for (int i = 0; i < TheTree->Selected->Count(); i++) {
					TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
					if (AScxmlBaseShape && AScxmlBaseShape->Locked) {
						TComponent *ASource = AInheritedEditor->FindComponent(AScxmlBaseShape->Name);
						if (!ASource) {
							AScxmlBaseShape->Locked = false;
							AScxmlBaseShape->InheritanceMismatch = true;
							throw Exception("Can not find base component [" + AScxmlBaseShape->Name + "]");
						}

						Statemachine::CheckAndFixInheritedParent(this, AScxmlBaseShape,
							dynamic_cast<TScxmlBaseShape*>(ASource));

						AScxmlBaseShape->StoreInheritedVisualsParentOffsets();

						AScxmlBaseShape->Assign(ASource);
						AScxmlBaseShape->Locked = true;
						AScxmlBaseShape->InheritanceMismatch = false;
						AOutListPtr->Add(AScxmlBaseShape->SimpleText);

						AScxmlBaseShape->RestoreInheritedVisualsParentOffsets();

						// после того, как мы сдвинули фигуру, также проверим все соединения, возможно они уже поправились
						Editorutils::TConnectionFromToMap AConnections;
						Editorutils::CollectInOutConnections(AScxmlBaseShape, AConnections);

						for (Editorutils::TConnectionFromToMap::iterator it = AConnections.begin();
							it != AConnections.end(); ++it) {
							TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
								(it->first);
							if (AStateMachineConnection && AStateMachineConnection->Locked) {
								// пересчитываем точки, так как при сдвиге родителей их значения поменялись
								Editorutils::CalculatePointsPosition(AStateMachineConnection);

								Unique::CompareError ACompareError;
								Unique::CompareInheritedConnection(AStateMachineConnection, AInheritedEditor->TheTree,
									ACompareError);
							}
						}

					}
				}

				if (AOutListPtr->Count) {
					TheTree->Invalidate();
					FillNodeTree(); // так как может поменяться родитель
					TeeModified(True, 1, "Reverted shapes:" + Customutils::ClippedText(AOutListPtr->CommaText, 50));
					if (PropSettingsInspector) {
						PropSettingsInspector->UpdateContent();
					}
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSetInheritance(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			const bool bIsInherited = StateMachineEditorUnit && StateMachineEditorUnit->IsInherited;

			std::auto_ptr<TStringList>AOutListPtr(new TStringList());

			// можно либо присвоить, либо снять наследование
			const bool bSet = AMenuItem->Tag == 0;

			if (TheTree->Connections->Selected) {
				Statemachine::SetInheritance(dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Selected),
					bSet);
				AOutListPtr->Add(TheTree->Connections->Selected->SimpleText);
			}
			else {
				for (int i = 0; i < TheTree->Selected->Count(); i++) {
					TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
					if (AScxmlBaseShape) {

						Statemachine::SetInheritance(AScxmlBaseShape, bSet);

						AOutListPtr->Add(AScxmlBaseShape->SimpleText);
					}
				}
			}

			if (AOutListPtr->Count) {

				TheTree->Invalidate();
				TeeModified(True, 1, (bIsInherited ? L"Inheritance " : L"Lock ") +
					(bSet ? UnicodeString("Set") : UnicodeString("Reset")) + ":" + Customutils::ClippedText
					(AOutListPtr->CommaText, 50));
				if (PropSettingsInspector) {
					PropSettingsInspector->UpdateContent();
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuRevertInheritedGeometry(TObject *Sender) {
	try {
		if (StateMachineEditorUnit && StateMachineEditorUnit->IsInherited) {
			TStateMachineEditor *AInheritedEditor = FindStateMachineEditorByFileName(StateMachineEditorUnit->Inherited);

			if (!AInheritedEditor)
				throw Exception("Can not find inherited module [" + StateMachineEditorUnit->Inherited + "]!");

			std::auto_ptr<TStringList>AOutListPtr(new TStringList());
			for (int i = 0; i < TheTree->Selected->Count(); i++) {
				TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
				if (AScxmlBaseShape && AScxmlBaseShape->Locked) {
					TComponent *ASource = AInheritedEditor->FindComponent(AScxmlBaseShape->Name);
					if (!ASource) {
						AScxmlBaseShape->Locked = false;
						throw Exception(L"Can not find base component [" + AScxmlBaseShape->Name + "]");
					}

					TScxmlBaseShape * ASourceShape = dynamic_cast<TScxmlBaseShape*>(ASource);
					if (!ASourceShape)
						throw Exception(L"Can not cast <" + ASource->Name + "> class:<" + ASource->ClassName()
						+ "> to <TScxmlBaseShape>");

					Statemachine::CheckAndFixInheritedParent(this, AScxmlBaseShape,
						dynamic_cast<TScxmlBaseShape*>(ASource));

					AScxmlBaseShape->StoreInheritedVisualsParentOffsets();

					AScxmlBaseShape->X0 = ASourceShape->X0;
					AScxmlBaseShape->X1 = ASourceShape->X1;
					AScxmlBaseShape->Y0 = ASourceShape->Y0;
					AScxmlBaseShape->Y1 = ASourceShape->Y1;

					AScxmlBaseShape->Locked = true;
					AScxmlBaseShape->InheritanceMismatch = false;
					AOutListPtr->Add(AScxmlBaseShape->SimpleText);

					AScxmlBaseShape->RestoreInheritedVisualsParentOffsets();

					// после того, как мы сдвинули фигуру, также проверим все соединения, возможно они уже поправились
					Editorutils::TConnectionFromToMap AConnections;
					Editorutils::CollectInOutConnections(AScxmlBaseShape, AConnections);

					for (Editorutils::TConnectionFromToMap::iterator it = AConnections.begin();
						it != AConnections.end(); ++it) {
						TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
							(it->first);
						if (AStateMachineConnection && AStateMachineConnection->Locked) {
							// пересчитываем точки, так как при сдвиге родителей их значения поменялись
							Editorutils::CalculatePointsPosition(AStateMachineConnection);

							Unique::CompareError ACompareError;
							Unique::CompareInheritedConnection(AStateMachineConnection, AInheritedEditor->TheTree,
								ACompareError);
						}
					}
				}
			}

			if (AOutListPtr->Count) {
				TheTree->Invalidate();
				FillNodeTree(); // так как может поменяться родитель
				TeeModified(True, 1, "Reverted Geometry:" + Customutils::ClippedText(AOutListPtr->CommaText, 50));
				if (PropSettingsInspector) {
					PropSettingsInspector->UpdateContent();
				}
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuInheritanceResolveFirstMismatch(TObject *Sender) {
	try {
		TStateMachineEditor *AInheritedEditor = StateMachineDockPanel ? StateMachineDockPanel->InheritedEditor : NULL;
		if (AInheritedEditor) {

			std::auto_ptr<TStringList>AMismatchListPtr(new TStringList());

			if (TStateMachineConnection * ACheckConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected)) {
				if (ACheckConnection->Locked) {

					Unique::CompareError ACompareError;
					Unique::CompareInheritedConnection(ACheckConnection, AInheritedEditor->TheTree, ACompareError);
					switch(ACompareError.Code) {
					case Unique::
						cecStringsTextMismatch :
					case Unique::
						cecPropertyMismatch :
					case Unique::
						cecConnectionPointsMismatch : {
							if (ACheckConnection->InheritanceResolver->ResolvedProps->IndexOf(ACompareError.PropName)
								== -1) {
								WLOG_WARNING(L"INHERITANCE> Object:[%s] Prop:[%s] resolved!",
									ACompareError.ObjectName.c_str(), ACompareError.PropName.c_str());
								AMismatchListPtr->Add(ACompareError.PropName);
								ACheckConnection->InheritanceResolver->ResolvedProps->Add(ACompareError.PropName);
								ACheckConnection->InheritanceMismatch = false;
							}
						}break;
					}
				}
			}
			else {
				for (int i = 0; i < TheTree->Selected->Count(); i++) {
					TScxmlBaseShape * ACheckShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
					if (ACheckShape && ACheckShape->Locked) {

						Unique::CompareError ACompareError;
						Unique::CompareInheritedShape(ACheckShape, AInheritedEditor->TheTree, ACompareError);
						switch(ACompareError.Code) {
						case Unique::
							cecStringsTextMismatch :
						case Unique::
							cecPropertyMismatch : {
								if (ACheckShape->InheritanceResolver->ResolvedProps->IndexOf(ACompareError.PropName)
									== -1) {
									WLOG_WARNING(L"INHERITANCE> Object:[%s] Prop:[%s] resolved!",
										ACompareError.ObjectName.c_str(), ACompareError.PropName.c_str());
									AMismatchListPtr->Add(ACompareError.PropName);
									ACheckShape->InheritanceResolver->ResolvedProps->Add(ACompareError.PropName);
									ACheckShape->InheritanceMismatch = false;
								}
							}break;
						}

					}
				}
			}

			if (AMismatchListPtr->Count) {
				TheTree->Invalidate();
				FillNodeTree(); // так как может поменяться родитель
				TeeModified(True, 1, "Resoved Properties:" + Customutils::ClippedText(AMismatchListPtr->CommaText, 50));
				if (PropSettingsInspector) {
					PropSettingsInspector->UpdateContent();
				}
			}

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuInheritanceResolveAllMismatches(TObject *Sender) {
	try {
		TStateMachineEditor *AInheritedEditor = StateMachineDockPanel ? StateMachineDockPanel->InheritedEditor : NULL;
		if (AInheritedEditor) {

			std::auto_ptr<TStringList>AMismatchListPtr(new TStringList());

			if (TStateMachineConnection * ACheckConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected)) {
				if (ACheckConnection->Locked) {

					std::vector<Unique::CompareError>AVecErrors;
					Unique::DeepCompareInheritedConnection(ACheckConnection, AInheritedEditor->TheTree, AVecErrors);

					std::auto_ptr<TStringList>AResolvedListPtr(new TStringList());
					AResolvedListPtr->Sorted = true;
					AResolvedListPtr->Duplicates = dupIgnore;

					for (std::size_t i = 0; i < AVecErrors.size(); i++) {
						switch(AVecErrors[i].Code) {
						case Unique::
							cecStringsTextMismatch :
						case Unique::
							cecPropertyMismatch :
						case Unique::
							cecConnectionPointsMismatch : {
								AResolvedListPtr->Add(AVecErrors[i].PropName);
								WLOG_WARNING(L"INHERITANCE> Object:[%s] Prop:[%s] resolved!",
									AVecErrors[i].ObjectName.c_str(), AVecErrors[i].PropName.c_str());
							}break;
						}
					}

					AMismatchListPtr->AddStrings(AResolvedListPtr.get());

					ACheckConnection->Locked = true;
					ACheckConnection->InheritanceResolver->ResolvedProps->Text = AResolvedListPtr->Text;
					ACheckConnection->InheritanceMismatch = false;
				}
			}
			else {
				for (int i = 0; i < TheTree->Selected->Count(); i++) {
					TScxmlBaseShape * ACheckShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
					if (ACheckShape && ACheckShape->Locked) {

						std::auto_ptr<TStringList>AResolvedListPtr(new TStringList());
						AResolvedListPtr->Sorted = true;
						AResolvedListPtr->Duplicates = dupIgnore;

						std::vector<Unique::CompareError>AVecErrors;
						Unique::DeepCompareInheritedShape(ACheckShape, AInheritedEditor->TheTree, AVecErrors);

						for (std::size_t k = 0; k < AVecErrors.size(); k++) {
							switch(AVecErrors[k].Code) {
							case Unique::
								cecStringsTextMismatch :
							case Unique::
								cecPropertyMismatch : {
									AResolvedListPtr->Add(AVecErrors[k].PropName);
									WLOG_WARNING(L"INHERITANCE> Object:[%s] Prop:[%s] resolved!",
										AVecErrors[k].ObjectName.c_str(), AVecErrors[k].PropName.c_str());
								}break;
							}
						}

						AMismatchListPtr->AddStrings(AResolvedListPtr.get());

						ACheckShape->Locked = true;
						ACheckShape->InheritanceResolver->ResolvedProps->Text = AResolvedListPtr->Text;
						ACheckShape->InheritanceMismatch = false;
					}
				}
			}

			if (AMismatchListPtr->Count) {
				TheTree->Invalidate();
				FillNodeTree(); // так как может поменяться родитель
				TeeModified(True, 1, "Resolved Properties:" + Customutils::ClippedText(AMismatchListPtr->CommaText,
						50));
				if (PropSettingsInspector) {
					PropSettingsInspector->UpdateContent();
				}
			}

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuInheritanceResolveClearAll(TObject *Sender) {
	try {
		TStateMachineEditor *AInheritedEditor = StateMachineDockPanel ? StateMachineDockPanel->InheritedEditor : NULL;
		if (AInheritedEditor) {

			if (TStateMachineConnection * ACheckConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected)) {
				if (ACheckConnection->Locked) {
					ACheckConnection->InheritanceResolver->ResolvedProps->Clear();
					ACheckConnection->InheritanceMismatch = false;
				}
			}
			else {
				for (int i = 0; i < TheTree->Selected->Count(); i++) {
					TScxmlBaseShape * ACheckShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
					if (ACheckShape && ACheckShape->Locked) {
						ACheckShape->InheritanceResolver->ResolvedProps->Clear();
						ACheckShape->InheritanceMismatch = false;
					}
				}
			}

			TheTree->Invalidate();
			FillNodeTree(); // так как может поменяться родитель
			TeeModified(True, 1, "Cleared all resolved");
			if (PropSettingsInspector) {
				PropSettingsInspector->UpdateContent();
			}

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuMoveBrotherUp(TObject *Sender) {
	try {
		TTreeNodeShape *AShape = GetSafeTreeListFirst(TheTree->Selected->Shapes);
		if (AShape) {
			if (AShape->PreviousBrother) {

				AShape->BrotherIndex = AShape->BrotherIndex - 1;

				FillNodeTree();
				TeeModified(True, 1, "Move brother up:" + AShape->SimpleText);
			}

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuMoveBrotherDown(TObject *Sender) {
	try {
		TTreeNodeShape *AShape = GetSafeTreeListFirst(TheTree->Selected->Shapes);
		if (AShape && AShape->Parent) {
			const int iShapeIndex = AShape->Parent->Children->IndexOf(AShape);
			const int iShapeNextIndex = iShapeIndex + 1;

			if (iShapeNextIndex < AShape->Parent->Children->Count) {
				AShape->BrotherIndex = AShape->BrotherIndex + 1;

				FillNodeTree();
				TeeModified(True, 1, "Move brother down:" + AShape->SimpleText);
			}

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuReplaceText(TObject *Sender) {
	try {
		TTreeNodeShape *AShape = GetSafeTreeListFirst(TheTree->Selected->Shapes);
		if (AShape) {

			if (!FFormReplaceDlg) {
				FFormReplaceDlg = new TFormReplaceDlg(this);
			}

			FFormReplaceDlg->ExecuteDialog(AShape);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSwapVisualBrothers(TObject *Sender) {
	try {
		if (TheTree->Selected->Count() == 2) {
			TTreeNodeShape *ASourceShape = TheTree->Selected->Items[0];
			TTreeNodeShape *ATargetShape = TheTree->Selected->Items[1];

			if (ASourceShape->Parent == ATargetShape->Parent) {
				using namespace Editorutils;

				TConnectionFromToMap ASourceConnections;
				Editorutils::CollectInOutConnections(ASourceShape, ASourceConnections);

				TConnectionFromToMap ATargetConnections;
				Editorutils::CollectInOutConnections(ATargetShape, ATargetConnections);

				Editorutils::ClearFromToInConnections(ASourceConnections);
				Editorutils::ClearFromToInConnections(ATargetConnections);

				const int iOffsetX = ATargetShape->X0 - ASourceShape->X0;
				const int iOffsetY = ATargetShape->Y0 - ASourceShape->Y0;

				ASourceShape->MoveRelative(iOffsetX, iOffsetY, true);
				ATargetShape->MoveRelative(-iOffsetX, -iOffsetY, true);

				// объединяем контейнеры, чтобы исключить двойное переназначение связей
				ASourceConnections.insert(ATargetConnections.begin(), ATargetConnections.end());

				for (TConnectionFromToMap::iterator it = ASourceConnections.begin(); it != ASourceConnections.end();
					++it) {

					TTreeNodeShape *AFromShape = boost::get<0>(it->second);
					TTreeNodeShape *AToShape = boost::get<1>(it->second);

					if (AFromShape->InheritsFrom(__classid(TVisualScxmlBaseShape)) && AToShape->InheritsFrom
						(__classid(TVisualScxmlBaseShape))) {
						if (AFromShape == ASourceShape) {
							AFromShape = ATargetShape;
						}
						else if (AFromShape == ATargetShape) {
							AFromShape = ASourceShape;
						}

						if (AToShape == ASourceShape) {
							AToShape = ATargetShape;
						}
						else if (AToShape == ATargetShape) {
							AToShape = ASourceShape;
						}
					}

					it->first->FromShape = AFromShape;
					it->first->ToShape = AToShape;

					AdjustSidesConnection(it->first);
				}

				const int iFromBrotherIndex = ASourceShape->BrotherIndex;
				const int iToBrotherIndex = ATargetShape->BrotherIndex;
				ASourceShape->BrotherIndex = iToBrotherIndex;
				ATargetShape->BrotherIndex = iFromBrotherIndex;

				FillNodeTree();

				TeeModified(True, 1, "Swap [" + ASourceShape->SimpleText + "] with [" + ATargetShape->SimpleText + "]");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuExportAsImage(TObject *Sender) {
	try {
		TScxmlBaseShape *AShape = dynamic_cast<TScxmlBaseShape*>(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape) { // AShape->AdjustedRectangle

			const int iOldHorizOffset = TheTree->View3DOptions->HorizOffset;
			const int iOldVertOffset = TheTree->View3DOptions->VertOffset;
			const bool bOldDesign = TheTree->Designing;
			const bool bOldGridVisible = TheTree->Grid->Visible;

			// prepare

			const bool bWasFocusDisabled = TheTreeEx->DisableFocus;
			TheTreeEx->DisableFocus = true;

			try {
				TheTree->View3DOptions->HorizOffset = 0;
				TheTree->View3DOptions->VertOffset = 0;
				TheTree->Designing = false;
				TheTree->Grid->Visible = false;

				const int i_OFFSET = 3;

				TRect ARectSource(0, 0, AShape->X1 + i_OFFSET, AShape->Y1 + i_OFFSET);

				TRect ARectShape(AShape->X0, AShape->Y0, AShape->X1 + i_OFFSET, AShape->Y1 + i_OFFSET);
				std::auto_ptr<Graphics::TBitmap>ABmpSourcePtr(TheTree->TeeCreateBitmap(clWhite, ARectSource));

				TRect ADestSource(0, 0, AShape->Width + i_OFFSET, AShape->Height + i_OFFSET);
				std::auto_ptr<Graphics::TBitmap>ABmpDest(new Graphics::TBitmap);
				ABmpDest->PixelFormat = ABmpSourcePtr->PixelFormat;
				ABmpDest->Width = ADestSource.Width();
				ABmpDest->Height = ADestSource.Height();

				ABmpDest->Canvas->CopyRect(ADestSource, ABmpSourcePtr->Canvas, ARectShape);

				TheTree->View3DOptions->HorizOffset = iOldHorizOffset;
				TheTree->View3DOptions->VertOffset = iOldVertOffset;
				TheTree->Designing = bOldDesign;
				TheTree->Grid->Visible = bOldGridVisible;

				std::auto_ptr<TImagePreviewer>AImagePreviewerPtr(new TImagePreviewer(this, AShape));
				/* предустановка значений */
				if (AImagePreviewerPtr->CheckAutoGenerateFileName->Checked) {
					AImagePreviewerPtr->EditDirectory->Text = StateMachineEditorUnit ? ExtractFileDir
						(StateMachineEditorUnit->FilePath) : UnicodeString(L"");
					AImagePreviewerPtr->EditFile->Text = StateMachineEditorUnit ? TPath::GetFileNameWithoutExtension
						(StateMachineEditorUnit->FilePath) : UnicodeString(L"");
				}
				else {
					AImagePreviewerPtr->EditDirectory->Text = SettingsData->TempRegistry->Values
						[AImagePreviewerPtr->Name + ".EditDirectory.Text"];
					AImagePreviewerPtr->EditFile->Text = SettingsData->TempRegistry->Values
						[AImagePreviewerPtr->Name + ".EditFile.Text"];
				}

				AImagePreviewerPtr->Image1->Picture->Bitmap->Assign(ABmpDest.get());

				AImagePreviewerPtr->ShowModal();

				// запоминаем значения
				if (!AImagePreviewerPtr->CheckAutoGenerateFileName->Checked) {
					SettingsData->TempRegistry->Values[AImagePreviewerPtr->Name + ".EditDirectory.Text"]
						= AImagePreviewerPtr->EditDirectory->Text;
					SettingsData->TempRegistry->Values[AImagePreviewerPtr->Name + ".EditFile.Text"]
						= AImagePreviewerPtr->EditFile->Text;
				}
			}
			__finally {
				TheTreeEx->DisableFocus = bWasFocusDisabled;
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"EXPORT AS IMAGE> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuImportBindingsAndFolders(TObject *Sender) {
	try {
		TScxmlBaseShape *AShape = dynamic_cast<TScxmlBaseShape*>(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape) {
			TheTree->Selected->Clear();

			if (TVirtualFolderShape::ImportBindingsAndFolders(AShape)) {
				FillNodeTree();
				TeeModified(True, 1, "Import protocol bindings:" + AShape->SimpleText);
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"IMPORT BINDINGS> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuCollectOutputs(TObject *Sender) {
	try {
		TScxmlBaseShape *AShape = dynamic_cast<TScxmlBaseShape*>(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape) {
			TheTree->Selected->Clear();

			if (CollectOutputs(AShape)) {
				FillNodeTree();
				TeeModified(True, 1, "Collect outputs:" + AShape->SimpleText);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuArrangeSelfConnections(TObject *Sender) {
	try {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape) {

			AShape->ArrangeSelfConnections();

			TeeModified(True, 1, "Arrange self connections:" + AShape->SimpleText);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuResetChildShapePos(TObject *Sender) {
	try {
		for (int i = 0; i < TheTree->Selected->Count(); i++) {
			TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(TheTree->Selected->Items[i]);
			if (AShape) {
				AShape->AutoPosition->Left = true;
				AShape->AutoPosition->Top = true;
			}
		}

		TeeModified(True, 1, "Reset child shape position");
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void ScanTextForOutputs(const UnicodeString &sText, TStrings *OutputsExistent, TListView *AListViewOutputs,
	const UnicodeString &sDescription) {
	boost::wregex e(L"\\bSetGlobal\\(([^,]*),");
	boost::wsmatch what;

	std::wstring wstr = sText.c_str();
	while (boost::regex_search(wstr, what, e)) {
		// for (std::size_t i = 0; i < what.size(); i++) {
		// WLOG_INFO(L"Regex_search %d=%s", i, std::wstring(what[i]).c_str());
		// }

		if (what.size() >= 2) {
			const UnicodeString sScxmlName(std::wstring(what[1]).c_str());

			if (OutputsExistent->IndexOf(sScxmlName) == -1) {
				TListItem *AItem = AListViewOutputs->Items->Add();
				AItem->Checked = true;

				AItem->Caption = sScxmlName;
				AItem->SubItems->Add(sScxmlName);
				AItem->SubItems->Add(sDescription);

				// добавим в базу, чтобы дальше в поиске не фигурировало
				OutputsExistent->Add(sScxmlName);
			}

		}

		wstr = what.suffix().str();
	}
}

void ConvertShapeToListViewOutput(TTreeNodeShape *ACurrentShape, TStrings *OutputsExistent,
	TListView *AListViewOutputs) {
	// TSendShape
	{
		TSendShape *AShape = dynamic_cast<TSendShape*>(ACurrentShape);
		if (AShape) {
			const UnicodeString sScxmlName = AShape->Event.IsEmpty() ? AShape->Eventexpr : AShape->Event;

			if (AShape->Type == "avia_protocol") {

				if (OutputsExistent->IndexOf(sScxmlName) == -1) {
					TListItem *AItem = AListViewOutputs->Items->Add();
					AItem->Checked = true;

					AItem->Caption = sScxmlName;
					AItem->SubItems->Add(sScxmlName);
					AItem->SubItems->Add(AShape->Text->Text);

					// добавим в базу, чтобы дальше в поиске не фигурировало
					OutputsExistent->Add(sScxmlName);
				}

			}
			return;
		}
	}
	// TSetValueShape
	{
		TSetValueShape *AShape = dynamic_cast<TSetValueShape*>(ACurrentShape);
		if (AShape) {
			const UnicodeString sScxmlName = AShape->Target.IsEmpty() ? AShape->Targetexpr : AShape->Target;

			if (AShape->Type == "1") {
				if (OutputsExistent->IndexOf(sScxmlName) == -1) {
					TListItem *AItem = AListViewOutputs->Items->Add();
					AItem->Checked = true;

					AItem->Caption = sScxmlName;
					AItem->SubItems->Add(sScxmlName);
					AItem->SubItems->Add(AShape->Text->Text);

					// добавим в базу, чтобы дальше в поиске не фигурировало
					OutputsExistent->Add(sScxmlName);

				}
			}

			return;
		}
	}
	// TDataShape
	{
		TDataChildShape *AShape = dynamic_cast<TDataChildShape*>(ACurrentShape);
		if (AShape) {

			ScanTextForOutputs(AShape->Expr.IsEmpty() ? AShape->XMLText->Text : AShape->Expr, OutputsExistent,
				AListViewOutputs, AShape->Text->Text);

			return;
		}
	}
	// TScriptShape
	{
		TScriptShape * AShape = dynamic_cast<TScriptShape*>(ACurrentShape);
		if (AShape) {
			ScanTextForOutputs(AShape->XMLText->Text, OutputsExistent, AListViewOutputs, AShape->Text->Text);

			return;
		}
	}
}

void ConvertShapeChildrenToListViewOutput(TTreeNodeShape *ACurrentShape, TStrings *OutputsExistent,
	TListView *AListViewOutputs) {
	for (int i = 0; i < ACurrentShape->Children->Count; i++) {
		ConvertShapeToListViewOutput(ACurrentShape->Children->Items[i], OutputsExistent, AListViewOutputs);

		// вызываем рекурсивно
		ConvertShapeChildrenToListViewOutput(ACurrentShape->Children->Items[i], OutputsExistent, AListViewOutputs);
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditor::CollectOutputs(TScxmlBaseShape *AParent) {

	std::auto_ptr<TStringList>AOutputsExistentListPtr(new TStringList());

	// СОБИРАЕМ ПРЕДВАРИТЕЛЬНО ВСЕ ПОДПИСКИ НА ЛИСТЕ
	for (int i = 0; i < TheTree->Connections->Count; i++) {
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Items[i]);
		if (AConnection && AConnection->ProtocolControlBinding->Active) {
			AOutputsExistentListPtr->Add(AConnection->ProtocolControlBinding->ScxmlName);
		}
	}

	for (int i = 0; i < TheTree->Shapes->Count; i++) {
		// TProtocolBindingShape
		{
			TProtocolBindingShape *AShape = dynamic_cast<TProtocolBindingShape*>(TheTree->Shapes->Items[i]);
			if (AShape) {
				if (AShape->ProtocolControlBinding->Active) {
					AOutputsExistentListPtr->Add(AShape->ProtocolControlBinding->ScxmlName);
				}
				continue;
			}

		}
		// TProtocolWeakBindingShape
		{
			TProtocolWeakBindingShape *AShape = dynamic_cast<TProtocolWeakBindingShape*>(TheTree->Shapes->Items[i]);
			if (AShape) {
				AOutputsExistentListPtr->Add(AShape->ScxmlName);
				continue;
			}
		}
		// TSendShape
		{
			TSendShape *AShape = dynamic_cast<TSendShape*>(TheTree->Shapes->Items[i]);
			if (AShape) {
				if (AShape->ProtocolControlBinding->Active) {
					AOutputsExistentListPtr->Add(AShape->ProtocolControlBinding->ScxmlName);
				}

				continue;
			}
		}
		// TSetValueShape
		{
			TSetValueShape *AShape = dynamic_cast<TSetValueShape*>(TheTree->Shapes->Items[i]);
			if (AShape) {
				if (AShape->ProtocolControlBinding->Active) {
					AOutputsExistentListPtr->Add(AShape->ProtocolControlBinding->ScxmlName);
				}

				continue;
			}
		}
	}

	// ТЕПЕРЬ ФОРМИРУЕМ ВЫХОДЫ
	std::auto_ptr<TDialogCollectOutputs>ADialogCollectOutputsPtr(new TDialogCollectOutputs(this));

	for (int i = 0; i < TheTree->Connections->Count; i++) {
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Items[i]);
		if (AConnection) {
			std::auto_ptr<TOnEntryStateShape>AShapeConnectionPtr(new TOnEntryStateShape(this));
			AShapeConnectionPtr->Tree = this->TheTree;
			Statemachine::XMLStringToInvisibleShapeChildren(AShapeConnectionPtr.get(), AConnection->XML);

			ConvertShapeChildrenToListViewOutput(AShapeConnectionPtr.get(), AOutputsExistentListPtr.get(),
				ADialogCollectOutputsPtr->ListViewOutputs);
		}
	}

	for (int i = 0; i < TheTree->Shapes->Count; i++) {
		ConvertShapeToListViewOutput(TheTree->Shapes->Items[i], AOutputsExistentListPtr.get(),
			ADialogCollectOutputsPtr->ListViewOutputs);
	}

	if (ADialogCollectOutputsPtr->ShowModal() == mrOk) {

		bool bOnce = false;

		for (int i = 0; i < ADialogCollectOutputsPtr->ListViewOutputs->Items->Count; i++) {
			if (ADialogCollectOutputsPtr->ListViewOutputs->Items->Item[i]->Checked) {
				TScxmlBaseShape *ANewBaseShape = NULL;

				if (ADialogCollectOutputsPtr->CheckComment->Checked) {
					ANewBaseShape = AParent->AddStateChild(TStateChildType::sctComment, false);
					TCommentShape * ACommentShape = dynamic_cast<TCommentShape*>(ANewBaseShape);
					if (ACommentShape) {
						ACommentShape->Comments->Text = ADialogCollectOutputsPtr->ListViewOutputs->Items->Item[i]
							->Caption;
					}
				}
				if (ADialogCollectOutputsPtr->CheckWeakBinding->Checked) {
					ANewBaseShape = AParent->AddStateChild(TStateChildType::sctWeakProtocolBinding, false);

					TProtocolWeakBindingShape * AWeakShape = dynamic_cast<TProtocolWeakBindingShape*>(ANewBaseShape);
					if (AWeakShape) {
						AWeakShape->ScxmlName = ADialogCollectOutputsPtr->ListViewOutputs->Items->Item[i]->Caption;
					}
				}

				if (ANewBaseShape) {
					AParent->Expanded = true;

					if (!bOnce) {

						TheTree->View3DOptions->HorizOffset = 0;
						TheTree->View3DOptions->VertOffset = 0;
						TheTree->Update();

						bOnce = true;

						TPoint P = TheTree->Canvas->Calculate3DPosition(ANewBaseShape->X0, ANewBaseShape->Y0, TeeTreeZ);

						int iTmpX = TheTree->ChartXCenter;
						int iTmpY = TheTree->ChartYCenter;

						P.x = P.x - iTmpX;
						P.y = P.y - iTmpY;

						TheTree->View3DOptions->HorizOffset = TheTree->View3DOptions->HorizOffset - P.x;
						TheTree->View3DOptions->VertOffset = TheTree->View3DOptions->VertOffset - P.y;
					}

					ANewBaseShape->Selected = true;
					TheTreeSelectShape(ANewBaseShape);

				}
			}

		}

		return true;
	}

	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectItemWithChildren(TObject *Sender) {
	try {
		for (int i = 0; i < TheTree->Selected->Count(); i++) {
			TheTree->Selected->Items[i]->SelectChilds();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectChildren(TObject *Sender) {
	try {
		TScxmlBaseShape *AShape = dynamic_cast<TScxmlBaseShape*>(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape) {
			TheTree->Selected->Clear();
			AShape->SelectChilds();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectSiblings(TObject *Sender) {
	try {
		TScxmlBaseShape *AShape = dynamic_cast<TScxmlBaseShape*>(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape && AShape->Parent) {
			for (int i = 0; i < AShape->Parent->Children->Count; i++) {
				AShape->Parent->Children->Items[i]->Selected = true;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuCopyPropertyToClipboard(TObject *Sender) {
	try {
		std::auto_ptr<TDialogCopyProperty>ADialogCopyPropertyPtr(new TDialogCopyProperty(this));
		if (ADialogCopyPropertyPtr->ShowModal() == mrOk) {

			std::auto_ptr<TStringList>AClipboardListPtr(new TStringList());

			for (int i = 0; i < TheTree->Selected->Count(); i++) {
				AClipboardListPtr->Add(MUtils::GetContainedPropValue(TheTree->Selected->Items[i],
						ADialogCopyPropertyPtr->ComboProperty->Text));
			}

			Clipboard()->AsText = AClipboardListPtr->Text.Trim();
			WLOG_INFO(L"Property [%s] copied to clipboard!\n%s", ADialogCopyPropertyPtr->ComboProperty->Text.c_str(),
				AClipboardListPtr->Text.c_str());
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuChangeXMLTextType(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			for (int i = 0; i < TheTree->Selected->Count(); i++) {
				TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
				if (AScxmlBaseShape && IsPublishedProp(AScxmlBaseShape, L"XMLText")) {
					AScxmlBaseShape->XMLText->Type = TXMLTextType(AMenuItem->Tag);
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuExpandCollapseChildren(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			for (int i = 0; i < TheTree->Selected->Count(); i++) {
				switch(AMenuItem->Tag) {
				case ecctExpandAll: {
						Editorutils::ExpandAll(TheTree->Selected->Items[i]);
					}break;
				case ecctExpandLevel: {
						TheTree->Selected->Items[i]->Expanded = true;
					}break;
				case ecctCollapseAll: {
						Editorutils::CollapseAll(TheTree->Selected->Items[i]);
					}break;
				case ecctCollapseLevel: {
						TheTree->Selected->Items[i]->Expanded = false;
					}break;
				}
			}

			TeeModified(true, 1, "Expand/collapse children");
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

void __fastcall TStateMachineEditor::OnMenuFilterSelectionByProperty(TObject *Sender) {
	try {
		std::auto_ptr<TDialogFilterProperty>ADialogFilterPropertyPtr(new TDialogFilterProperty(this));
		if (ADialogFilterPropertyPtr->ShowModal() == mrOk) {

			Editorutils::TreeUpdateLock ALock(TheTree);

			std::auto_ptr<TStringList>AClipboardListPtr(new TStringList());

			std::map<TTreeNodeShape*, bool>AShapes;

			for (int i = 0; i < TheTree->Selected->Count(); i++) {
				const UnicodeString sPropValue = MUtils::GetContainedPropValue(TheTree->Selected->Items[i],
					ADialogFilterPropertyPtr->ComboProperty->Text);

				switch(ADialogFilterPropertyPtr->ComboFilterCondition->ItemIndex) {
				case fpctEqual:
					if (Wildcardutils::WildCardMatch(ADialogFilterPropertyPtr->ComboCompare->Text, sPropValue,
							ADialogFilterPropertyPtr->CheckCaseSensitive->Checked)) {
						AShapes.insert(std::make_pair(TheTree->Selected->Items[i], true));
						continue;
					}
					break;
				case fpctNotEqual:
					if (!Wildcardutils::WildCardMatch(ADialogFilterPropertyPtr->ComboCompare->Text, sPropValue,
							ADialogFilterPropertyPtr->CheckCaseSensitive->Checked)) {
						AShapes.insert(std::make_pair(TheTree->Selected->Items[i], true));
						continue;
					}
					break;
				}

				AShapes.insert(std::make_pair(TheTree->Selected->Items[i], false));
			}

			Editorutils::UniversalUpdateLock<TLMDPropertyInspector>APropLock(PropSettingsInspector);

			for (std::map<TTreeNodeShape*, bool>::iterator it = AShapes.begin(); it != AShapes.end(); ++it) {
				it->first->Selected = it->second;
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelfConnectionAlign(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (TheTree && AMenuItem) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AConnection) {

				const TSelfConnectionAlign AConnectionAlign = TSelfConnectionAlign(AMenuItem->Tag);

				{
					Editorutils::TreeUpdateLock ALock(TheTree);

					AConnection->AlignSelfConnection(AConnectionAlign);
				}

				TeeModified(true, 1,
					"Self-connection [" + AConnection->SimpleText + "] align set to [" + SelfConnectionAlignToString
					(AConnectionAlign) + "]");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionSwitch(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (TheTree && AMenuItem) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AConnection) {

				if (AMenuItem == FMenuTransitionSwitchSimple) {
					AConnection->SWITCH = AConnection->SWITCH == tstSIMPLE ? tstNONE : tstSIMPLE;
				}
				else if (AMenuItem == FMenuTransitionSwitchInvertCondition) {
					AConnection->SWITCH = AConnection->SWITCH == tstINVERT_CONDITION ? tstNONE : tstINVERT_CONDITION;
				}
				else
					throw Exception(L"PROGRAM LOGIC ERROR!");

				TeeModified(true, 1, "Transition [" + AConnection->SimpleText + "] SWITCH [" +
					TransitionSwitchTypeToString(AConnection->SWITCH) + "]");

				if (PropSettingsInspector) {
					PropSettingsInspector->UpdateItems();
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionFindSourceTarget(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (TheTree && AMenuItem && TheTree->Connections->Selected) {
			switch(AMenuItem->Tag) {
			case TRANSITION_FIND_SOURCE: {
					TTreeNodeShape *AShape = TheTree->Connections->Selected->FromShape;
					if (AShape) {
						this->SelectShape(AShape);
					}
				}break;
			case TRANSITION_FIND_TARGET: {
					TTreeNodeShape *AShape = TheTree->Connections->Selected->ToShape;
					if (AShape) {
						this->SelectShape(AShape);
					}
				}break;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionTextAlign(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (TheTree && AMenuItem) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AConnection) {

				bool bTrueTop = true;
				bool bTrueLeft = true;
				// текст располагается по центральной точке
				if (AConnection->Points->Count() / 2) {
					const int iIndexLast = AConnection->Points->Count() / 2;
					const int iIndexFirst = iIndexLast - 1;

					// устраняем баг в либе, когда перепутаны названия Лево-Право, Вверх-Вниз
					bTrueTop = AConnection->Points->Item[iIndexFirst].Y < AConnection->Points->Item[iIndexLast].Y;
					bTrueLeft = AConnection->Points->Item[iIndexFirst].X < AConnection->Points->Item[iIndexLast].X;
				}

				switch(AMenuItem->Tag) {
				case TConnectionTextAlignType::
					ctatCenterCenter : {
						AConnection->Text->HorizAlign = htaCenter;
						AConnection->Text->HorizOffset = 0;
						AConnection->Text->VertAlign = vtaCenter;
						AConnection->Text->VertOffset = 0;
					}break;
				case ctatCenterTop: {
						AConnection->Text->HorizAlign = htaCenter;
						AConnection->Text->HorizOffset = 0;
						AConnection->Text->VertAlign = bTrueTop ? vtaTop : vtaBottom;
						AConnection->Text->VertOffset = 0;
					}break;
				case ctatCenterBottom: {
						AConnection->Text->HorizAlign = htaCenter;
						AConnection->Text->HorizOffset = 0;
						AConnection->Text->VertAlign = bTrueTop ? vtaBottom : vtaTop;
						AConnection->Text->VertOffset = 0;
					}break;
				case ctatLeftCenter: {
						AConnection->Text->HorizAlign = bTrueLeft ? htaLeft : htaRight;
						AConnection->Text->HorizOffset = 0;
						AConnection->Text->VertAlign = vtaCenter;
						AConnection->Text->VertOffset = 0;
					}break;
				case ctatRightCenter: {
						AConnection->Text->HorizAlign = bTrueLeft ? htaRight : htaLeft;
						AConnection->Text->HorizOffset = 0;
						AConnection->Text->VertAlign = vtaCenter;
						AConnection->Text->VertOffset = 0;
					}break;
				}

				TeeModified(true, 1, "Text align changed [" + AConnection->SimpleText + "]");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuConnectionOver(TObject *Sender) {
	try {
		TComponent * AComponent = dynamic_cast<TComponent*>(Sender);
		if (TheTree && AComponent) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AConnection && AConnection->FromShape && AConnection->ToShape) {

				AConnection->SetConnectionOverStyle(TConnectionOverType(AComponent->Tag));

				Editorutils::ProtectConnectionToBeInSamePoints(AConnection);

				AConnection->AdjustableSides = true;

				TeeModified(true, 1, "Connection over applied [" + AConnection->SimpleText + "]");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuPointArrangeMirror(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (TheTree && AMenuItem) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AConnection && AConnection->IsSelfConnection) {

				if (AConnection->Points->Count() == 4 && (PopupPoint->Tag == 1 || PopupPoint->Tag == 2)) {

					Editorutils::TreeUpdateLock ALock(TheTree);

					TSelfConnectionInfo AInfo = AConnection->SelfConnectionInfo;

					const int iOffset = PopupPoint->Tag == 1 ? AInfo.LengthOffsetBegin : AInfo.LengthOffsetEnd;
					AConnection->AlignSelfConnection(AInfo.Align, iOffset, iOffset);

				}

				TeeModified(true, 1, "Point - mirror arrange [" + AConnection->SimpleText + "]");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuPointArrangeSingle(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (TheTree && AMenuItem) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AConnection && AConnection->IsSelfConnection) {

				if (AConnection->Points->Count() == 4 && (PopupPoint->Tag == 1 || PopupPoint->Tag == 2)) {

					Editorutils::TreeUpdateLock ALock(TheTree);

					TSelfConnectionInfo AInfo = AConnection->SelfConnectionInfo;

					const int iOffsetBegin = PopupPoint->Tag == 1 ? AInfo.LengthOffsetBegin : 0;
					const int iOffsetEnd = PopupPoint->Tag == 2 ? AInfo.LengthOffsetEnd : 0;

					AConnection->AlignSelfConnection(AInfo.Align, iOffsetBegin, iOffsetEnd);
				}

				TeeModified(true, 1, "Point - single arrange [" + AConnection->SimpleText + "]");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuPointAlignOppositeVerticallyClick(TObject *Sender) {
	try {
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Selected);
		if (AConnection && AConnection->Points->Count() > 1) {

			const int iSourceIndex = PopupPoint->Tag;
			const int iTargetIndex = PopupPoint->Tag == 0 ? (AConnection->Points->Count() - 1) : 0;

			const int iHorzDiff = AConnection->Points->Item[iSourceIndex].X - AConnection->Points->Item[iTargetIndex].X;
			if (abs(iHorzDiff) > 10) {
				const int iDiff = AConnection->Points->Item[iSourceIndex].Y - AConnection->Points->Item[iTargetIndex].Y;
				AConnection->Points->Move(iTargetIndex, 0, iDiff);

				TeeModified(true, 1, "Point - align opposite vertically [" + AConnection->SimpleText + "]");
			}
			else
				WLOG_ERROR(L"Axis X points difference (%d px) is too small for vertical alignment!", iHorzDiff);

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuPointAlignOppositeHorizontallyClick(TObject *Sender) {
	try {
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Selected);
		if (AConnection && AConnection->Points->Count() > 1) {

			const int iSourceIndex = PopupPoint->Tag;
			const int iTargetIndex = PopupPoint->Tag == 0 ? (AConnection->Points->Count() - 1) : 0;

			const int iVertDiff = AConnection->Points->Item[iSourceIndex].Y - AConnection->Points->Item[iTargetIndex].Y;
			if (abs(iVertDiff) > 10) {
				const int iDiff = AConnection->Points->Item[iSourceIndex].X - AConnection->Points->Item[iTargetIndex].X;
				AConnection->Points->Move(iTargetIndex, iDiff, 0);

				TeeModified(true, 1, "Point - align opposite horizontally [" + AConnection->SimpleText + "]");
			}
			else
				WLOG_ERROR(L"Axis Y difference (%d px) is too small for horizontal alignment!", iVertDiff);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
TTreeNodeShape *FindNodeForFixedPoint(TTreeConnection *AConnection, const int iPointIndex) {
	TTreeNodeShape *AShape = NULL;

	if (AConnection->FromShape && AConnection->ToShape) {

		if (AConnection->FromShape != AConnection->ToShape) {
			const TPoint ptSelected(AConnection->Points->Item[iPointIndex].XValue,
				AConnection->Points->Item[iPointIndex].YValue);
			if (Editorutils::PtInRectEx(AConnection->FromShape->Bounds(), ptSelected)) {
				AShape = AConnection->FromShape;
			}
			else if (Editorutils::PtInRectEx(AConnection->ToShape->Bounds(), ptSelected)) {
				AShape = AConnection->ToShape;
			}
			else {

				std::auto_ptr<TDialogSelectShape>ADialogSelectShapePtr(new TDialogSelectShape(AConnection->Owner));
				if (ADialogSelectShapePtr->ShowModal() == mrOk) {
					AShape = ADialogSelectShapePtr->RadioGroup1->ItemIndex == 0 ?
						AConnection->FromShape : AConnection->ToShape;
				}
			}
		}
		else {
			AShape = AConnection->FromShape;
		}

	}

	return AShape;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuPointPinClick(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {

		TTreeConnection *AConnection = dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Selected);
		if (AConnection) {
			const int iPointIndex = PopupPoint->Tag;

			switch(AMenuItem->Tag) {
			case pptLeft: {
					switch(AConnection->Points->Item[iPointIndex].XStyle) {
					case cpsFromRel:
					case cpsToRel:
					case cpsFromPercent:
					case cpsToPercent: {
							AConnection->Points->Item[iPointIndex].XValue = 0;
						}break;
					case cpsFixed: {
							TTreeNodeShape *AShape = FindNodeForFixedPoint(AConnection, iPointIndex);
							if (AShape) {
								AConnection->Points->Item[iPointIndex].XValue = AConnection->FromShape->X0;
							}
						}break;
					}
				}break;
			case pptTop: {
					switch(AConnection->Points->Item[iPointIndex].YStyle) {
					case cpsFromRel:
					case cpsToRel:
					case cpsFromPercent:
					case cpsToPercent: {
							AConnection->Points->Item[iPointIndex].YValue = 0;
						}break;
					case cpsFixed: {
							TTreeNodeShape *AShape = FindNodeForFixedPoint(AConnection, iPointIndex);
							if (AShape) {
								AConnection->Points->Item[iPointIndex].YValue = AConnection->FromShape->Y0;
							}
						}break;
					}
				}break;
			case pptRight: {
					switch(AConnection->Points->Item[iPointIndex].XStyle) {
					case cpsFromRel: {
							if (AConnection->FromShape) {
								AConnection->Points->Item[iPointIndex].XValue = AConnection->FromShape->Width;
							}
						}break;
					case cpsToRel: {
							if (AConnection->ToShape) {
								AConnection->Points->Item[iPointIndex].XValue = AConnection->ToShape->Width;
							}
						}break;
					case cpsFromPercent:
					case cpsToPercent: {
							AConnection->Points->Item[iPointIndex].XValue = 100;
						}break;
					case cpsFixed: {
							TTreeNodeShape *AShape = FindNodeForFixedPoint(AConnection, iPointIndex);
							if (AShape) {
								AConnection->Points->Item[iPointIndex].XValue = AConnection->FromShape->X1;
							}
						}break;
					}
				}break;
			case pptBottom: {
					switch(AConnection->Points->Item[iPointIndex].YStyle) {
					case cpsFromRel: {
							if (AConnection->FromShape) {
								AConnection->Points->Item[iPointIndex].YValue = AConnection->FromShape->Height;
							}
						}break;
					case cpsToRel: {
							if (AConnection->ToShape) {
								AConnection->Points->Item[iPointIndex].YValue = AConnection->ToShape->Height;
							}
						}break;
					case cpsFromPercent:
					case cpsToPercent: {
							AConnection->Points->Item[iPointIndex].YValue = 100;
						}break;
					case cpsFixed: {
							TTreeNodeShape *AShape = FindNodeForFixedPoint(AConnection, iPointIndex);
							if (AShape) {
								AConnection->Points->Item[iPointIndex].YValue = AConnection->FromShape->Y1;
							}
						}break;
					}
				}break;
			}

			TheTree->Refresh();

			TeeModified(true, 1, "Point - pinned [" + AConnection->SimpleText + "]");
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TabControl1Change(System::TObject * Sender) {
	try {
		TTreeEditorEx::TabControl1Change(Sender);

		std::set<int>ATreeTabs;
		ATreeTabs.insert(0);
		ATreeTabs.insert(1);
		TheTree->Visible = ATreeTabs.find(TabControl1->TabIndex) != ATreeTabs.end();

		std::set<int>ASyntaxEditableTabs;
		ASyntaxEditableTabs.insert(2);
		ASyntaxEditableTabs.insert(3);
		ASyntaxEditableTabs.insert(4);
		ASyntaxEditableTabs.insert(5);

		FSyntaxEditorEx->Visible = ASyntaxEditableTabs.find(TabControl1->TabIndex) != ASyntaxEditableTabs.end();
		FSyntaxEditorEx->SyntaxFileExtenstion = L""; // по умолчанию у всех должно соответствовать расширению

		switch(TabControl1->TabIndex) {
		case 2: {
				FEditDocRaw->ActiveSyntaxScheme = "XML";
				FEditDocRaw->Lines->Text = Statemachine::GetRawScxml(TheTree, SettingsData->SkipCommentsInRawScxml);
			}break;
		case 3: {
				FEditDocRaw->ActiveSyntaxScheme = "CPP";
				std::auto_ptr<TStringList>AStringList(new TStringList());
				AStringList->Text = Statemachine::GetRawScxml(TheTree, SettingsData->SkipCommentsInRawScxml);

				TStateMachineEditor::ConvertTextToHPP(AStringList.get(),
					TPath::GetFileNameWithoutExtension(this->CurrentFile));
				FEditDocRaw->Lines->Text = AStringList->Text;
			}break;
		case 4: {
				FEditDocRaw->ActiveSyntaxScheme = "DFM";
				std::auto_ptr<TMemoryStream>AMemoryStreamPtr(new TMemoryStream());
				SaveTreeToStream(TheTree, AMemoryStreamPtr.get());

				std::auto_ptr<TStringStream>AOutStreamPtr(new TStringStream());

				AMemoryStreamPtr->Position = 0;
				ObjectBinaryToText(AMemoryStreamPtr.get(), AOutStreamPtr.get());
				AOutStreamPtr->Position = 0;

				FEditDocRaw->Lines->Text = AOutStreamPtr->DataString;
			}break;
		case 5: { /* SVG */
				FEditDocRaw->ActiveSyntaxScheme = "XML";
				FSyntaxEditorEx->SyntaxFileExtenstion = L"svg";

				std::auto_ptr<TStringList>AOutputListPtr(new TStringList());
				SaveToSVG(AOutputListPtr.get());

				_di_ILMDXmlDocument AXmlDoc = LMDLoadXmlDocumentFromXML(AOutputListPtr->Text);

				FEditDocRaw->Lines->Text = AXmlDoc ? AXmlDoc->XML : UnicodeString(L"");
			}break;
		}
	}
	catch(EStateMachineConnectionException * E) {
		this->SelectConnection(E->StateMachineConnection, SCROLL_CHART_IN_VIEW);
		this->SelectTemporaryRectangle(Editorutils::IncrementRectCopy(E->StateMachineConnection->GetBounds(), 15),
			"Error: " + E->Category);
		WLOG_ERROR(L"CHANGE VIEW> %s", E->Message.c_str());
	}
	catch(EScxmlBaseShapeException * E) {
		this->SelectShape(E->Shape, SCROLL_CHART_IN_VIEW);
		this->SelectTemporaryRectangle(Editorutils::IncrementRectCopy(E->Shape->Bounds(), 15), "Error: " + E->Category);
		WLOG_ERROR(L"CHANGE VIEW> %s", E->Message.c_str());
	}
	catch(EScxmlDuplicateStateIDException * E) {
		HandleExceptionDuplicateStateID(E);
		WLOG_ERROR(L"CHANGE VIEW> %s", E->Message.c_str());
	}
	catch(Exception * E) {
		WLOG_ERROR(L"CHANGE VIEW> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ConvertTextToHPP(TStrings * AScxmlLines, const UnicodeString & sHeaderName) {
	AScxmlLines->Text = StringReplace(AScxmlLines->Text, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
	AScxmlLines->Text = StringReplace(AScxmlLines->Text, '"', UnicodeString("\\") + UnicodeString('"'),
		TReplaceFlags() << rfReplaceAll);

	for (int i = AScxmlLines->Count - 1; i >= 0; i--) {
		if (!AScxmlLines->Strings[i].IsEmpty()) {
			AScxmlLines->Strings[i] = UnicodeString("\t\"") + AScxmlLines->Strings[i] + UnicodeString('"');
		}
		else {
			AScxmlLines->Delete(i);
		}
	}

	AScxmlLines->Text = AScxmlLines->Text.TrimRight() + ";";

	UnicodeString sFileNameOnly = sHeaderName.UpperCase();
	for (int i = sFileNameOnly.Length(); i >= 1; i--) {
		if (!IsAlphaNumeric(sFileNameOnly[i])) {
			sFileNameOnly[i] = L'_';
		}
	}

	const UnicodeString sNamespace = "Scxml" + sFileNameOnly.LowerCase();
	sFileNameOnly = "SCXML__" + sFileNameOnly;

	AScxmlLines->Insert(0, "#ifndef __" + sFileNameOnly + "__");
	AScxmlLines->Insert(1, "#define __" + sFileNameOnly + "__");
	AScxmlLines->Insert(2, "");
	AScxmlLines->Insert(3, "namespace " + sNamespace + " {");
	AScxmlLines->Insert(4, "\tstatic const char *chScxml =");

	AScxmlLines->Add("} // end namespace: " + sNamespace);
	AScxmlLines->Add("#endif");
}

// ---------------------------------------------------------------------------
TLMDDesignObjects *__fastcall TStateMachineEditor::GetDesignObjects(void) {
	return FDesignObjects ? FDesignObjects : FormScxmlGui->DesignObjects1;
}

// ---------------------------------------------------------------------------
TLMDPropertyInspector *__fastcall TStateMachineEditor::GetPropSettingsInspector(void) {
	return FPropSettingsInspector ? FPropSettingsInspector : FormScxmlGui->PropSettingsInspector;
}

// ---------------------------------------------------------------------------
TSpeedButtonClass __fastcall TStateMachineEditor::TreeSpeedButtonClass() {
	return __classid(TScxmlSpeedButton);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::PopupNodePopup(System::TObject * Sender) {
	TTreeEditorEx::PopupNodePopup(Sender);

	AddChild1->Visible = false;
	AddBrother1->Visible = false;
	AddnewRoot2->Visible = false;

	FMenuArrangeLayout->Visible = TheTree != NULL;

	if (TheTree && TheTree->Selected) {

		TTreeNodeShape *ASelectedShape = GetSafeTreeListFirst(TheTree->Selected->Shapes);

		const bool bSingleSelection = TheTree->Selected->Count() == 1;
		const bool bIsVisualScxmlShape = ASelectedShape && ASelectedShape->InheritsFrom
			(__classid(TVisualScxmlBaseShape));
		const bool bIsChildScxmlShape = ASelectedShape && ASelectedShape->InheritsFrom(__classid(TChildScxmlBaseShape));
		const bool bAreAllVisual = AreAllSelectedOfInheritedClass(__classid(TVisualScxmlBaseShape));
		const bool bAreAllBrothers = AreAllSelectedBrothers();
		const bool bAreAllOfSameClass = AreAllSelectedOfSameClass();
		const bool bIsInherited = StateMachineEditorUnit && StateMachineEditorUnit->IsInherited;
		const bool bIsBase = StateMachineEditorUnit && StateMachineEditorUnit->IsBase;

		const bool bBindingAvailable = SettingsData->AviaExtensionEnabled && ASelectedShape &&
			(ASelectedShape->InheritsFrom(__classid(TVisualScxmlBaseShape)) || ASelectedShape->InheritsFrom
			(__classid(TVirtualFolderShape)));

		FMenuImportBindingsAndFolders->Visible = bBindingAvailable;
		FMenuCollectOutputs->Visible = bBindingAvailable;

		FMenuFilterSelectionByProperty->Visible = TheTree->Selected->Count() > 1;

		// NOTE: IsInitialDeep is not published !
		FMenuIsInitial->Visible = ASelectedShape && IsPublishedProp(ASelectedShape, "IsInitial");
		if (FMenuIsInitial->Visible) {
			TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelectedShape);
			if (AVisualShape) {
				FMenuIsInitial->Checked = AVisualShape->IsInitialDeep;
			}
		}

		FMenuConvertTo->Clear();
		AddChild1->Clear();
		FMenuAddNonVisualChild->Clear();

		FMenuInheritance->Caption = (bIsInherited || bIsBase) ? L"Inheritance" : L"Lock settings";
		FMenuSetInheritance->Caption = bIsInherited ? L"Set Inheritance" : L"Lock";
		FMenuResetInheritance->Caption = bIsInherited ? L"Reset Inheritance" : L"Unlock";
		FMenuRevertToInheritedProps->Visible = bIsInherited;
		FMenuRevertInheritedGeometry->Visible = bIsInherited;
		FMenuRevertInOutConnections->Visible = bIsInherited;
		FMenuRevertRecursive->Visible = bIsInherited && AreAllSelectedBrothers();
		FMenuInheritanceDumpDifference->Visible = bIsInherited;
		FMenuInheritanceResolveFirstMismatch->Visible = bIsInherited;
		FMenuInheritanceResolveAllMismatches->Visible = bIsInherited;
		FMenuInheritanceResolveClearAll->Visible = bIsInherited;
		FMenuRevertInInheritedUnits->Visible = bIsBase;

		TScxmlBaseShape *ABaseShape = dynamic_cast<TScxmlBaseShape*>(ASelectedShape);
		if (ABaseShape) {
			for (int i = 0; i < TStateChildType::sctMAXSIZE; i++) {
				const TStateChildType AStateType = TStateChildType(i);

				if (ABaseShape->AvailableTypes.count(AStateType)) {
					// ограничим авиа дополнения
					if (!SettingsData->AviaExtensionEnabled && STATE_TYPES_AVIA_EXTENSIONS.count(AStateType)) {
						continue;
					}

					/* особый случай, когда добавляем Virtual */
					if (AStateType == sctVirtual &&
						(!StateMachineEditorUnit || !StateMachineEditorUnit->StateMachineProject)) {
						continue;
					}

					/* особый случай, когда в SCXML Virtual пытаемся добвлять элементы */
					if (ABaseShape->StateChildType == sctScxml && StateMachineEditorUnit &&
						StateMachineEditorUnit->IsVirtual) {
						if (STATE_TYPES_VIRTUAL_SCXML_SHAPES.find(AStateType) != STATE_TYPES_VIRTUAL_SCXML_SHAPES.end()
							) {
							continue;
						}
					}

					TMenuItem *MenuAddSubChild = new TMenuItem(this);

					const UnicodeString sName = TScxmlBaseShape::TypeToXMLNodeName(AStateType);
					MenuAddSubChild->Caption = sName;
					MenuAddSubChild->Tag = i;
					MenuAddSubChild->OnClick = OnMenuAddSubChildClick;

					std::map<UnicodeString, int>::iterator it = FShapeImagesMap.find(sName);
					if (it != FShapeImagesMap.end()) {
						MenuAddSubChild->ImageIndex = it->second;
					}

					if (STATE_TYPES_VISUAL_SHAPES.count(AStateType)) {
						AddChild1->Add(MenuAddSubChild);
					}
					else {
						FMenuAddNonVisualChild->Add(MenuAddSubChild);
					}
				}
			}
		}

		bool bShapeHasSelfConnections = false;

		TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelectedShape);
		if (AVisualShape) {

			bShapeHasSelfConnections = AVisualShape->HasSelfConnections;

			FMenuSetBreakpoint->Checked = AVisualShape->BreakpointSet;
			FMenuSkipDebugging->Checked = AVisualShape->SkipDebugging;

			for (int i = 0; i < TStateChildType::sctMAXSIZE; i++) {
				const TStateChildType AStateType = TStateChildType(i);

				bool bForbiddenWithCluster =
					(AStateType == sctHistory || AStateType == sctFinal || AStateType == sctInitial)
					&& AVisualShape->IsCluster();

				if (AVisualShape->ConvertibleTypes.count(AStateType)
					&& AVisualShape->VisualParent && AVisualShape->VisualParent->AvailableTypes.count(AStateType)
					&& !bForbiddenWithCluster) {

					if (AStateType == sctVirtual &&
						(!StateMachineEditorUnit || !StateMachineEditorUnit->StateMachineProject)) {
						continue;
					}

					TMenuItem *MenuAddSubChild = new TMenuItem(this);

					const UnicodeString sName = TScxmlBaseShape::TypeToXMLNodeName(AStateType);
					MenuAddSubChild->Caption = sName;
					MenuAddSubChild->Tag = i;
					std::map<UnicodeString, int>::iterator it = FShapeImagesMap.find(sName);
					if (it != FShapeImagesMap.end()) {
						MenuAddSubChild->ImageIndex = it->second;
					}
					MenuAddSubChild->OnClick = OnMenuConvertToClick;
					FMenuConvertTo->Add(MenuAddSubChild);
				}
			}
		}

		if (bIsChildScxmlShape && ABaseShape->BaseParent && bAreAllOfSameClass) {
			for (int i = 0; i < TStateChildType::sctMAXSIZE; i++) {
				const TStateChildType AStateType = TStateChildType(i);

				if (ABaseShape->ConvertibleTypes.count(AStateType) && ABaseShape->BaseParent->AvailableTypes.count
					(AStateType)) {

					TMenuItem *MenuAddSubChild = new TMenuItem(this);

					const UnicodeString sName = TScxmlBaseShape::TypeToXMLNodeName(AStateType);
					MenuAddSubChild->Caption = sName;
					MenuAddSubChild->Tag = i;
					std::map<UnicodeString, int>::iterator it = FShapeImagesMap.find(sName);
					if (it != FShapeImagesMap.end()) {
						MenuAddSubChild->ImageIndex = it->second;
					}
					MenuAddSubChild->OnClick = OnMenuConvertToClick;
					FMenuConvertTo->Add(MenuAddSubChild);
				}
			}
		}

		FMenuPasteConnection->Clear();
		if (bAreAllVisual) {
			for (TMapConnections::iterator it = SettingsData->ClipboardConnectionsMap.begin();
				it != SettingsData->ClipboardConnectionsMap.end(); ++it) {
				if (bSingleSelection == it->second.first) {
					TMenuItem *AMenuClipboardConnection = new TMenuItem(this);
					AMenuClipboardConnection->Caption = it->first;
					AMenuClipboardConnection->OnClick = OnMenuPasteConnection;
					FMenuPasteConnection->Add(AMenuClipboardConnection);
				}
			}

			if (FMenuPasteConnection->Count) {
				TMenuItem *AMenuClearClipboardConnections = new TMenuItem(this);
				AMenuClearClipboardConnections->Caption = "Clear all";
				AMenuClearClipboardConnections->ImageIndex = 97;
				AMenuClearClipboardConnections->OnClick = OnMenuClearConnectionClipboardMap;
				FMenuPasteConnection->Insert(0, AMenuClearClipboardConnections);

				FMenuPasteConnection->InsertNewLineAfter(AMenuClearClipboardConnections);
			}
		}
		FMenuPasteConnection->Visible = FMenuPasteConnection->Count != 0;

		FMenuTextAlign->Visible = AVisualShape &&
			(AVisualShape->StateChildType == sctHistory || AVisualShape->StateChildType == sctFinal);

		FMenuMakeSelfConnection->Visible = bIsVisualScxmlShape && STATE_TYPES_SELFCONNECTION_SHAPES.find
			(AVisualShape->StateChildType) != STATE_TYPES_SELFCONNECTION_SHAPES.end();
		FMenuSetBreakpoint->Visible = bIsVisualScxmlShape;
		FMenuSkipDebugging->Visible = bIsVisualScxmlShape;
		FMenuArrangeSelfConnections->Visible = bShapeHasSelfConnections;

		FMenuPresets->Visible = AVisualShape && STATE_TYPES_CLUSTERABLE_SHAPES.count(AVisualShape->StateChildType);
		FMenuShapeTextAutoSize->Visible = AVisualShape && AVisualShape->StateChildType == sctState &&
			!AVisualShape->IsCluster();

		FMenuMoveBrotherUp->Visible = bIsChildScxmlShape;
		FMenuMoveBrotherDown->Visible = bIsChildScxmlShape;
		FMenuResetChildShapePos->Visible = bIsChildScxmlShape;

		AddChild1->Visible = ASelectedShape && (AddChild1->Count != 0);
		AddBrother1->Visible = ASelectedShape && (ABaseShape == NULL);
		AddnewRoot2->Visible = ASelectedShape && (ABaseShape == NULL);

		FMenuAddNonVisualChild->Visible = FMenuAddNonVisualChild->Count != 0;
		FMenuConvertTo->Visible = FMenuConvertTo->Count != 0;

		FMenuSwapVisualBrothers->Visible = TheTree->Selected->Count() == 2 && bAreAllVisual && bAreAllBrothers;

		FMenuChangeXMLTextType->Visible = ABaseShape && IsPublishedProp(ABaseShape, "XMLText") && bAreAllOfSameClass;
		if (ABaseShape) {
			for (int i = 0; i < FMenuChangeXMLTextType->Count; i++) {
				FMenuChangeXMLTextType->Items[i]->Checked = FMenuChangeXMLTextType->Items[i]
					->Tag == ABaseShape->XMLText->Type;
				if (FMenuChangeXMLTextType->Items[i]->Checked) {
					FMenuChangeXMLTextType->Caption = "XMLType [" + StripHotkey
						(FMenuChangeXMLTextType->Items[i]->Caption) + "]";
				}
			}
		}

		FMenuCollapseAll->Visible = Editorutils::IsAnyChildShapeExpanded(TheTree->Selected->Shapes, true);
		FMenuCollapseLevel->Visible = Editorutils::IsAnyChildShapeExpanded(TheTree->Selected->Shapes, false);
		FMenuExpandAll->Visible = Editorutils::IsAnyChildShapeCollapsed(TheTree->Selected->Shapes, true);
		FMenuExpandLevel->Visible = Editorutils::IsAnyChildShapeCollapsed(TheTree->Selected->Shapes, false);

		FMenuSelectChildren->Visible = Editorutils::IsAnyShapeHasChildren(TheTree->Selected->Shapes);
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::PopupConnPopup(System::TObject * Sender) {
	TTreeEditorEx::PopupConnPopup(Sender);

	if (TheTree) {
		const bool bIsInherited = StateMachineEditorUnit && StateMachineEditorUnit->IsInherited;
		const bool bIsBase = StateMachineEditorUnit && StateMachineEditorUnit->IsBase;

		FMenuTransitionInheritance->Caption = (bIsInherited || bIsBase) ? L"Inheritance" : L"Lock settings";
		FMenuTransitionSetInheritance->Caption = bIsInherited ? L"Set Inheritance" : L"Lock";
		FMenuTransitionResetInheritance->Caption = bIsInherited ? L"Reset Inheritance" : L"Unlock";
		FMenuTransitionRevertToInheritedProps->Visible = bIsInherited;
		FMenuTransitionInheritanceDumpDifference->Visible = bIsInherited;
		FMenuTransitionInheritanceResolveFirstMismatch->Visible = bIsInherited;
		FMenuTransitionInheritanceResolveAllMismatches->Visible = bIsInherited;
		FMenuTransitionInheritanceResolveClearAll->Visible = bIsInherited;
		FMenuTransitionRevertInInheritedUnits->Visible = bIsBase;

		TStateMachineConnection* AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
			(TheTree->Connections->Selected);
		FMenuEditTransition->Enabled = AStateMachineConnection != NULL;

		FMenuSelfConnectionAlign->Visible = AStateMachineConnection != NULL && AStateMachineConnection->FromShape !=
			NULL && AStateMachineConnection->FromShape == AStateMachineConnection->ToShape;

		const bool bTwoShapesConnected = AStateMachineConnection != NULL && AStateMachineConnection->FromShape !=
			NULL && AStateMachineConnection->ToShape != NULL && AStateMachineConnection->FromShape !=
			AStateMachineConnection->ToShape;

		FMenuTransitionSwitchSimple->Visible = bTwoShapesConnected;
		FMenuTransitionSwitchInvertCondition->Visible = bTwoShapesConnected;

		bool bLeftRightOver = false;
		bool bTopBottomOver = false;
		if (bTwoShapesConnected /* уже подразумевает, что AStateMachineConnection != NULL */ ) {
			FMenuTransitionSwitchSimple->Checked = AStateMachineConnection->SWITCH == tstSIMPLE;
			FMenuTransitionSwitchInvertCondition->Checked = AStateMachineConnection->SWITCH == tstINVERT_CONDITION;

			bTopBottomOver = abs(AStateMachineConnection->FromShape->XCenter()
				- AStateMachineConnection->ToShape->XCenter()) > 25.0f;
			bLeftRightOver = abs(AStateMachineConnection->FromShape->YCenter()
				- AStateMachineConnection->ToShape->YCenter()) > 25.0f;

			// направление иконок для Sides и InvertedSides
			bool bLeft = true;
			bool bTop = true;

			const TRect AConnectionBounds = AStateMachineConnection->GetBounds();
			const int iPointsCount = AStateMachineConnection->Points->Count();
			if (iPointsCount) {
				const int iTo = iPointsCount - 1;

				bLeft = AStateMachineConnection->Points->Item[iTo].X == AConnectionBounds.Left;
				bTop = AStateMachineConnection->Points->Item[iTo].Y == AConnectionBounds.Top;
			}

			Sidesstyle1->ImageIndex = bLeft ? 156 : 158;
			Invertedsidesstyle1->ImageIndex = bTop ? 157 : 159;
		}

		FMenuTransitionEnableTrigger->Visible = AStateMachineConnection != NULL;
		FMenuTransitionEnableTrigger->Checked = AStateMachineConnection &&
			AStateMachineConnection->ContentTrigger->Enabled;

		FMenuTransitionConvertTo->Visible = AStateMachineConnection != NULL;

		FMenuTransitionSetBreakpoint->Visible = AStateMachineConnection != NULL;

		FMenuConnectionOverTop->Visible = bTwoShapesConnected && bTopBottomOver;
		FMenuConnectionOverBottom->Visible = bTwoShapesConnected && bTopBottomOver;

		FMenuConnectionOverLeft->Visible = bTwoShapesConnected && bLeftRightOver;
		FMenuConnectionOverRight->Visible = bTwoShapesConnected && bLeftRightOver;

		FMenuTransitionSwap->Visible = bTwoShapesConnected;

		/* TRANSITION TEXT ALIGN */ {
			bool bTrueTop = true;
			bool bTrueLeft = true;
			// текст располагается по центральной точке
			if (AStateMachineConnection && AStateMachineConnection->Points->Count() / 2) {
				const int iIndexLast = AStateMachineConnection->Points->Count() / 2;
				const int iIndexFirst = iIndexLast - 1;

				// устраняем баг в либе, когда перепутаны названия Лево-Право, Вверх-Вниз
				bTrueTop = AStateMachineConnection->Points->Item[iIndexFirst].Y < AStateMachineConnection->Points->Item
					[iIndexLast].Y;
				bTrueLeft = AStateMachineConnection->Points->Item[iIndexFirst].X < AStateMachineConnection->Points->Item
					[iIndexLast].X;
			}

			for (int i = 0; i < FMenuTransitionTextAlign->Count; i++) {
				switch(FMenuTransitionTextAlign->Items[i]->Tag) {
				case ctatCenterCenter:
					FMenuTransitionTextAlign->Items[i]->Checked = AStateMachineConnection &&
						AStateMachineConnection->Text->HorizAlign == htaCenter &&
						AStateMachineConnection->Text->VertAlign == vtaCenter;
					break;
				case ctatCenterTop:
					FMenuTransitionTextAlign->Items[i]->Checked = AStateMachineConnection &&
						AStateMachineConnection->Text->HorizAlign == htaCenter &&
						AStateMachineConnection->Text->VertAlign == (bTrueTop ? vtaTop : vtaBottom);
					break;
				case ctatCenterBottom:
					FMenuTransitionTextAlign->Items[i]->Checked = AStateMachineConnection &&
						AStateMachineConnection->Text->HorizAlign == htaCenter &&
						AStateMachineConnection->Text->VertAlign == (bTrueTop ? vtaBottom : vtaTop);
					break;
				case ctatLeftCenter:
					FMenuTransitionTextAlign->Items[i]->Checked = AStateMachineConnection &&
						AStateMachineConnection->Text->HorizAlign == (bTrueLeft ? htaLeft : htaRight)
						&& AStateMachineConnection->Text->VertAlign == vtaCenter;
					break;
				case ctatRightCenter:
					FMenuTransitionTextAlign->Items[i]->Checked = AStateMachineConnection &&
						AStateMachineConnection->Text->HorizAlign == (bTrueLeft ? htaRight : htaLeft)
						&& AStateMachineConnection->Text->VertAlign == vtaCenter;
					break;
				default:
					FMenuTransitionTextAlign->Items[i]->Checked = false;
				}
			}
		} /* END */
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::PopupPointPopup(System::TObject* Sender) {
	TTreeEditorEx::PopupPointPopup(Sender);

	bool bArrangeAvailable = false;
	bool bAlignOpposites = false;

	TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Selected);
	if (AConnection) {
		if (AConnection->IsSelfConnection) {
			if (AConnection->Points->Count() == 4 && (PopupPoint->Tag == 1 || PopupPoint->Tag == 2)) {
				bArrangeAvailable = true;
			}
		}
		else {
			if (AConnection->Points->Count() > 1 && (PopupPoint->Tag == 0 || PopupPoint->Tag ==
					(AConnection->Points->Count() - 1))) {
				bAlignOpposites = true;
			}
		}
	}
	FMenuPointAlignOppositeHorizontally->Visible = bAlignOpposites;
	FMenuPointAlignOppositeVertically->Visible = bAlignOpposites;

	FMenuPointArrangeMirror->Visible = bArrangeAvailable;
	FMenuPointArrangeSingle->Visible = bArrangeAvailable;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuIsInitialClick(TObject * Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>
				(GetSafeTreeListFirst(TheTree->Selected->Shapes));
			// NOTE: IsInitialDeep is not published !
			if (AVisualShape && IsPublishedProp(AVisualShape, "IsInitial")) {
				AVisualShape->IsInitialDeep = AMenuItem->Checked;

				if (PropSettingsInspector) {
					PropSettingsInspector->UpdateContent();
				}

				TheTree->Invalidate();

				TeeModified(true, 1, "Changed initial state [" + AVisualShape->SimpleText + "]");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuAddSubChildClick(TObject * Sender) {
	try {
		TMenuItem *AItem = dynamic_cast<TMenuItem*>(Sender);
		if (AItem) {
			TScxmlBaseShape *AParentShape = dynamic_cast<TScxmlBaseShape*>
				(GetSafeTreeListFirst(TheTree->Selected->Shapes));
			if (AParentShape) {
				TheTree->Selected->Clear();

				TScxmlBaseShape *ANewBaseShape = AParentShape->AddStateChild(TStateChildType(AItem->Tag), true);
				if (ANewBaseShape) {
					AParentShape->Expanded = true;
					ANewBaseShape->Selected = true;

					EnsureSelectedInView();
				}

				TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ANewBaseShape);
				if (AVisualScxmlBaseShape && AVisualScxmlBaseShape->MustSimpleTextBeUnique()) {
					AVisualScxmlBaseShape->SetStateId(Unique::CreateUniqueSimpleText(AVisualScxmlBaseShape,
							__classid(TVisualScxmlBaseShape)));
				}

				FillNodeTree();
				TeeModified(true, 1, "Added child to <" + AParentShape->Name + ">");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuEditTransitionClick(TObject * Sender) {
	if (TheTree) {
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Selected);
		if (AConnection) {
			AConnection->ShowEditor();
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditor::AreAllSelectedOfSameClass() {
	TMetaClass *AClass = NULL;
	for (int i = 0; i < TheTree->Selected->Count(); i++) {
		if (i == 0) {
			AClass = TheTree->Selected->Items[i]->ClassType();
		}
		else {
			if (AClass != TheTree->Selected->Items[i]->ClassType()) {
				return false;
			}
		}
	}
	return AClass != NULL;
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditor::AreAllSelectedOfInheritedClass(TMetaClass *AClass) {
	return Editorutils::AreAllShapesOfInheritedClass(TheTree->Selected->Shapes, AClass);
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditor::AreAllSelectedBrothers() {
	return Editorutils::AreAllShapesBrothers(TheTree->Selected->Shapes);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionEnableTriggerClick(TObject * Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AConnection) {
				AConnection->ContentTrigger->Enabled = !AConnection->ContentTrigger->Enabled;

				if (PropSettingsInspector) {
					PropSettingsInspector->UpdateContent();
				}

				TheTree->Invalidate();

				MarkModified(1, "Trigger <" + AConnection->Caption + "> enabled", false);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionConvertToClick(TObject * Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AConnection) {
				try {
					TVisualScxmlBaseShape * AFromShape = dynamic_cast<TVisualScxmlBaseShape*>(AConnection->FromShape);
					if (!AFromShape)
						throw Exception("Visual FromShape is required!");

					const TConnectionRole ARole = static_cast<TConnectionRole>(AMenuItem->Tag);
					switch(ARole) {
					case crlEventless: {
							AConnection->Event = L"";
							AConnection->Condition = L"";
						}break;
					case crlDelayed: {
							TDialogDelayedTransition *ADialogDelayedTransition = new TDialogDelayedTransition(this);
							ADialogDelayedTransition->EditEvent->Text = AConnection->Event;
							ADialogDelayedTransition->EditDelay->Text = L"1s";
							if (ADialogDelayedTransition->ShowModal() == mrOk) {

								AConnection->Event = ADialogDelayedTransition->EditEvent->Text;

								TOnEntryStateShape*AOnEntryShape = Editorutils::FindShapeByType<TOnEntryStateShape>
									(AFromShape);
								if (!AOnEntryShape)
									AOnEntryShape = dynamic_cast<TOnEntryStateShape*>
										(AFromShape->AddStateChild(sctOnentry, false));

								if (!AOnEntryShape)
									throw Exception("Can not create 'onentry'!");

								TOnExitStateShape*AOnExitShape = Editorutils::FindShapeByType<TOnExitStateShape>
									(AFromShape);
								if (!AOnExitShape)
									AOnExitShape = dynamic_cast<TOnExitStateShape*>
										(AFromShape->AddStateChild(sctOnexit, false));

								if (!AOnExitShape)
									throw Exception("Can not create 'onexit'!");

								if (AOnEntryShape->ChildIndex > AOnExitShape->ChildIndex) {
									AOnEntryShape->ChildIndex = AOnExitShape->ChildIndex;
								}

								TSendShape*ASendShape = dynamic_cast<TSendShape*>(AOnEntryShape->AddStateChild(sctSend,
										false));
								if (!ASendShape)
									throw Exception("Can not create 'send'!");

								TCancelShape*ACancelShape = dynamic_cast<TCancelShape*>
									(AOnExitShape->AddStateChild(sctCancel, false));
								if (!ACancelShape)
									throw Exception("Can not create 'cancel'!");

								ASendShape->Event = AConnection->Event;
								ASendShape->Delay = ADialogDelayedTransition->EditDelay->Text;
								ASendShape->Delayexpr = ADialogDelayedTransition->EditDelayExpr->Text;
								ASendShape->Id = L"ID." + AConnection->Event;

								ACancelShape->Sendid = ASendShape->Id;

								AOnEntryShape->Expanded = true;
								AOnExitShape->Expanded = true;
							}

						}break;
					case crlDonestate: {
							AConnection->Event = "done.state." + AConnection->FromShape->SimpleText;
						}break;
					}

					MarkModified(1, "Converted <" + AConnection->Caption + "> to <" + AMenuItem->Caption + ">", true);
				}
				catch(Exception * E) {
					throw Exception(L"[" + AConnection->Caption + "] " + E->Message);
				}
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"CONVERSION ERROR> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuConvertToClick(TObject * Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			/* TVisualScxmlBaseShape */
			if (TVisualScxmlBaseShape * AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
				(GetSafeTreeListFirst(TheTree->Selected->Shapes))) {
				if (AVisualBaseShape->VisualParent) {

					const TStateChildType AStateType = TStateChildType(AMenuItem->Tag);

					// особый случай - перенос на новый лист,
					// поэтому предварительно копируем именно в этом месте
					if (AStateType == sctVirtual) {

						if (TStateMachineConnection * AIOConnection = AVisualBaseShape->HasInternalInOutConnections()) {
							throw Exception(UnicodeString().sprintf(
									L"Can not convert to virtual with internal connection:[%s] from:[%s] to:[%s]", //
									AIOConnection->Caption.c_str(), AIOConnection->FromShape->SimpleText.c_str(),
									AIOConnection->ToShape->SimpleText.c_str()));
						}

						FCliboardEditorEx->Copy(this->TheTree);
					}

					TScxmlBaseShape *ABaseShape = AVisualBaseShape->VisualParent->AddStateChild(AStateType, false);
					if (ABaseShape) {

						TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(ABaseShape);
						assert(AVisualShape != NULL);

						AVisualShape->Tree = AVisualBaseShape->Tree;
						const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, AVisualShape->ClassType());
						AVisualShape->Name = AUniqueName.second;

						AVisualShape->Assign(AVisualBaseShape);

						AVisualShape->Parent = AVisualBaseShape->Parent;

						if (AStateType != sctVirtual) {
							while (AVisualBaseShape->Children->Count) {
								// так как теряются связи типа TTreeConnection, используем специальную функцию назначения родителя
								Editorutils::SetShapeParentEx(AVisualBaseShape->Children->Items[0], AVisualShape);
							}
						}

						for (int i = 0; i < TheTree->Connections->Count; i++) {
							if (TheTree->Connections->Items[i]->FromShape == AVisualBaseShape) {
								TheTree->Connections->Items[i]->FromShape = AVisualShape;
							}
							if (TheTree->Connections->Items[i]->ToShape == AVisualBaseShape) {
								TheTree->Connections->Items[i]->ToShape = AVisualShape;
							}
						}

						TheTree->Selected->Clear();
						delete AVisualBaseShape;

						/* особый случай виртуальная конвертация */
						if (TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>(ABaseShape)) {

							TStateMachineEditorUnit *ANewUnit = AVirtualShape->NewVirtualUnit();
							if (ANewUnit) {
								ANewUnit->Open(true);

								TStateMachineDockPanel *ADockPanel = ANewUnit->StateMachineDockPanel;
								if (ADockPanel) {
									TScxmlShape *AScxmlShape = ADockPanel->StateMachineEditor->RootScxml;
									if (AScxmlShape) {
										AScxmlShape->ScxmlName = L"Scxml" + AVirtualShape->SimpleText;
										if (this->RootScxml) {
										AScxmlShape->Datamodel = this->RootScxml->Datamodel;
										}
										AScxmlShape->Selected = true;

										FCliboardEditorEx->Paste(ADockPanel->StateMachineEditor->TheTree, false);

										TVisualScxmlBaseShape *AVisualRoot = AScxmlShape->FirstVisualChild;
										if (AVisualRoot) {
										AVisualRoot->MoveRelative(25, 25, true);
										AScxmlShape->X1 = AVisualRoot->X1 + 50;
										AScxmlShape->Y1 = AVisualRoot->Y1 + 50;
										}

										ADockPanel->StateMachineEditor->MarkModified(1, "Virtual shapes created", true);
									}
								}
							}

							// удаляем само-соединения для виртуальной фигуры, так как они будут перенесены в виртуальный юнит
							for (int i = AVirtualShape->Connections->Count - 1; i >= 0; i--) {
								if (AVirtualShape->Connections->Items[i]->FromShape == AVirtualShape &&
									AVirtualShape->Connections->Items[i]->ToShape == AVirtualShape) {
									delete AVirtualShape->Connections->Items[i];
								}
							}
						}
						else {
							AVisualShape->Selected = true;
						}

						AVisualShape->ValidateShape();
						TScxmlBaseShape::ValidateSimpleText(TheTree);

						FillNodeTree();

						TeeModified(true, 1, "Converted <" + AVisualShape->Name + ">");

						if (AStateType == sctVirtual) {
							TheTree->Refresh();
						}
					}
				}
			}
			/* TChildScxmlBaseShape */
			else if (AreAllSelectedOfSameClass()) {
				std::auto_ptr<TStringList>AShapesListPtr(new TStringList());

				std::map<int, TScxmlBaseShape*>AMapShapes;
				std::vector<TScxmlBaseShape*>AVecShapesForDelete;

				for (int i = 0; i < TheTree->Selected->Count(); i++) {
					TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->Items[i]);
					if (AScxmlBaseShape && AScxmlBaseShape->BaseParent) {

						const TStateChildType AStateChildType = TStateChildType(AMenuItem->Tag);

						if (!AScxmlBaseShape->BaseParent->AvailableTypes.count(AStateChildType))
							throw Exception
								(UnicodeString().sprintf
							(L"Element [%s] can not contain TStateChildType=[%d]",
								AScxmlBaseShape->BaseParent->ClassName().c_str(), AMenuItem->Tag));

						TScxmlBaseShape *ABaseShape = AScxmlBaseShape->BaseParent->AddStateChild(AStateChildType,
							false);
						if (ABaseShape) {
							std::auto_ptr<TTreePicture>ATreePicturePtr(new TTreePicture);
							ATreePicturePtr->Assign(ABaseShape->Image);

							AShapesListPtr->Add(ABaseShape->Name);
							ABaseShape->Assign(AScxmlBaseShape);
							ABaseShape->Image->Assign(ATreePicturePtr.get());

							// переподсоединяем всех детей
							while (AScxmlBaseShape->Children->Count) {
								AScxmlBaseShape->Children->Items[0]->Parent = ABaseShape;
							}

							AMapShapes.insert(std::make_pair(AScxmlBaseShape->ChildIndex, ABaseShape));
							AVecShapesForDelete.push_back(AScxmlBaseShape);
						}

					}
				}

				TheTree->Selected->Clear();

				for (std::size_t i = 0; i < AVecShapesForDelete.size(); i++) {
					delete AVecShapesForDelete[i];
				}

				for (std::map<int, TScxmlBaseShape*>::iterator it = AMapShapes.begin(); it != AMapShapes.end(); ++it) {
					it->second->ChildIndex = it->first;
					it->second->Selected = true;
				}

				FillNodeTree();
				TeeModified(true, 1, "Converted <" + AShapesListPtr->CommaText + ">");
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"CONVERSION ERROR> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionTextMoveClick(TObject * Sender) {
	TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
		(TheTree->Connections->Selected);
	if (AStateMachineConnection && !AStateMachineConnection->IsTextEmpty()) {
		CreateDummyShape(this, TheTree, AStateMachineConnection);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionSwap(TObject * Sender) {
	TTreeConnection *AConnection = TheTree->Connections->Selected;
	if (TheTree && AConnection && AConnection->FromShape && AConnection->ToShape && AConnection->Points->Count() >= 2) {

		{
			const int iPtCount = AConnection->Points->Count();

			const Editorutils::TreeUpdateLock ATreeLock(TheTree);

			/* запоминаем положение начальной-конечной точек в процентах относительно родителя */
			TTreeNodeShape *AFromShape = AConnection->FromShape;
			TTreeNodeShape *AToShape = AConnection->ToShape;

			const int iFirstX = AConnection->Points->Item[0].X;
			const int iFirstY = AConnection->Points->Item[0].Y;

			const int iLastX = AConnection->Points->Item[iPtCount - 1].X;
			const int iLastY = AConnection->Points->Item[iPtCount - 1].Y;

			const int iPtPercentFirstX = (iFirstX - AFromShape->X0) * 100 / AFromShape->Width;
			const int iPtPercentFirstY = (iFirstY - AFromShape->Y0) * 100 / AFromShape->Height;

			const int iPtPercentLastX = (iLastX - AToShape->X0) * 100 / AToShape->Width;
			const int iPtPercentLastY = (iLastY - AToShape->Y0) * 100 / AToShape->Height;

			/* здесь непосредственно смена входа-выхода соединения */
			AConnection->FromShape = AToShape;
			AConnection->ToShape = AFromShape;

			/* восстанавливаем точки так, как они были относительно входов-выходов */
			AConnection->Points->ChangeXStyle(0, cpsFromPercent);
			AConnection->Points->ChangeYStyle(0, cpsFromPercent);
			AConnection->Points->ChangeXStyle(iPtCount - 1, cpsToPercent);
			AConnection->Points->ChangeYStyle(iPtCount - 1, cpsToPercent);

			AConnection->Points->Item[0].XValue = iPtPercentLastX;
			AConnection->Points->Item[0].YValue = iPtPercentLastY;

			AConnection->Points->Item[iPtCount - 1].XValue = iPtPercentFirstX;
			AConnection->Points->Item[iPtCount - 1].YValue = iPtPercentFirstY;

		}

		Editorutils::AdjustSidesStyle(AConnection); // это перепроверит положение текста
		Editorutils::AdjustSidesConnection(AConnection); // это подстроит настраиваемые связи

		MarkModified(1, "FromShape and ToShape <" + AConnection->SimpleText + "> swapped", true);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTransitionMovePoints(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {
		TTreeConnection *AConnection = TheTree->Connections->Selected;
		if (TheTree && AConnection) {
			MoveConnectionPoints(AConnection, TPointPinType(AMenuItem->Tag));

			TeeModified(true, 1, "Transition <" + AConnection->SimpleText + "> points moved!");
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuTextAlignClick(TObject * Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && TheTree) {
		TTreeNodeShape *AShape = GetSafeTreeListFirst(TheTree->Selected->Shapes);
		if (AShape) {

			const Editorutils::TDirectionType AType = AMenuItem->Tag;
			Editorutils::SetShapeTextAlign(AShape, AType);

			AShape->Draw();

			TeeModified(true, 1, "Text align:[" + AMenuItem->Caption + "] changed <" + AShape->SimpleText + ">");
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuArrangeLayout(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && TheTree) {
		try {
			Graphviz::ArrangeTreeLayout(TheTree);

			TeeModified(true, 1, "Layout arranged!");
		}
		catch(Exception * E) {
			WLOG_ERROR(L"ARRANGE LAYOUT> %s", E->Message.c_str());
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuPresetCondition(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && TheTree) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape) {
			AddShapesFromPresetCondition();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuPresetOnOff(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && TheTree) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape) {
			AddShapesFromPresetOnOff();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuShapeTextAutoSize(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && TheTree) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape && !AShape->IsCluster()) {

			TheTree->Canvas->Font->Assign(AShape->Font);

			const int iTextWidth = TheTree->Canvas->TextWidth(AShape->SimpleText);
			const int iTextHeight = TheTree->Canvas->TextHeight(AShape->SimpleText);
			AShape->X1 = AShape->X0 + iTextWidth + 20;
			AShape->Y1 = AShape->Y0 + (float)iTextHeight * 2.5f;

			AShape->ArrangeSelfConnections();

			TeeModified(true, 1, "Shape <" + AShape->SimpleText + "> was autosized");
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSetBreakpoint(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && TheTree) {

		if (TheTree->Connections->Selected) {
			TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Selected);
			if (AStateMachineConnection) {
				AStateMachineConnection->BreakpointSet = !AStateMachineConnection->BreakpointSet;
			}
		}
		else {
			TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
				(GetSafeTreeListFirst(TheTree->Selected->Shapes));
			if (AShape) {
				AShape->BreakpointSet = !AShape->BreakpointSet;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSkipDebugging(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && TheTree) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(GetSafeTreeListFirst(TheTree->Selected->Shapes));
		if (AShape) {
			AShape->SkipDebugging = !AShape->SkipDebugging;

			if (PropSettingsInspector) {
				PropSettingsInspector->UpdateContent();
			}
			TeeModified(true, 1, "Shape <" + AShape->SimpleText + "> SkipDebugging option is set to <" + BoolToStr
				(AShape->SkipDebugging, true) + ">!");
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineEditor::GetOpenDialogFilter(void) {
	return L"Scxml files (*.scxml)|*.scxml|Tree files (*.str)|*.str|Text files (*.txt)|*.txt";
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::DeleteTemporaryShapes(void) {
	for (int i = this->TheTree->Shapes->Count - 1; i >= 0; i--) {
		if (TTextDummyShape * ATextDummyShape = dynamic_cast<TTextDummyShape*>(this->TheTree->Shapes->Items[i])) {
			delete ATextDummyShape;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::FillNodeTree(void) {
	// переносим через флаг, чтобы уже выполнялась после всех перестроений
	FTreeFillNodeTreeFlag = true;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ResetTree(void) {
	try {
		// first reset the Tree (empty)
		ClearDesignObjects();

		/* inherited */
		TTreeEditorEx::ResetTree();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::FinalizeLoadTree(const UnicodeString sEditorCaption,
	const TFinalizeScxmlType AFinalizeType) {
	try {
		SetTreeProperties();

		CurrentFile = sEditorCaption;

		MainSetCaption(); // Editor Form Caption

		DeleteTemporaryShapes(); // dummy shapes

		UpdateAllChildrenSimpleText(); // корректируем все значения в SimpleText

		FillNodeTree();

		// add the loaded filename to "recent files" menu
		// FRecentFiles->Add(CurrentFile);

		/* Set Design mode */ TabControl1->TabIndex = 0;
		TabControl1Change(TabControl1);

		/* mark the Tree as "not modified" */ TeeModified(False);

		ClearAllUndoRedo();
		const UnicodeString sUndoType = FinalizeScxmlTypeToString(AFinalizeType);
		AddUndo(sUndoType + " [" + ExtractFileName(sEditorCaption) + "]", NULL, false);

		// fix bug: может записаться не дефолтный курсор
		TheTree->Cursor = crDefault;
		TheTree->OriginalCursor = crDefault;

#if 0 /* Тестируем без, так как убрал таймер */
		// специальная затычка, чтобы убирался фокус,
		// иначе без этого вообще невозможно применить фокус к дереву
		// он постоянно сбрасывается без этого
		TStateMachineDockPanel *APanel = dynamic_cast<TStateMachineDockPanel*>(this->Owner);
		if (APanel) {
			// здесь теряем фокус
			APanel->Deactivate();
			// здесь активируем и при активации запустится таймер
			APanel->Activate();
		}
#endif

		// предупреждения по игнору дебага
		for (int i = 0; i < TheTree->Items->Count; i++) {
			/* визуальные фигуры */
			if (TVisualScxmlBaseShape * AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
				(TheTree->Items->Items[i])) {
				// предупреждение, что исключено из отладки
				if (AVisualBaseShape->SkipDebugging) {
					WLOG_WARNING(L"Shape:[%s] Statemachine:[%s] is skipped for DEBUGGING!",
						AVisualBaseShape->SimpleText.c_str(), ScxmlName.c_str());
				}
			}
			/* дочерние элементы */
			else if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>
				(TheTree->Items->Items[i])) {
				// bugfix: могут не прорисовываться связи у дочерних элементов
				if (AChildScxmlBaseShape->Parent && !AChildScxmlBaseShape->Parents->Count) {
					WLOG_WARNING(L"Connection:[%s] Statemachine:[%s] is not connected to parent!",
						AChildScxmlBaseShape->Name.c_str(), ScxmlName.c_str());
					AChildScxmlBaseShape->Parent->AddConnection(AChildScxmlBaseShape);
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::LoadTree(const System::UnicodeString AFileName) {
	/* load a Tree from file, and setup several things */

	try {
		try {
			this->ResetTree();

			// bool bChangeExt = true;
			if (ExtractFileExt(AFileName).UpperCase() == ".STR") {
				LoadTreeFromFile(TheTree, AFileName, LoadTreeProc, __classid(TLogTreeReader));
				// bChangeExt = false;
			}
			else if (ExtractFileExt(AFileName).LowerCase() == TStateMachineEditorUnit::FileExt_.LowerCase()) {

				Statemachine::LoadTreeFromEditor(this, AFileName);

			}
			else {
				TheTree->LoadFromTextFile(AFileName);
			}
		}
		__finally {
			FinalizeLoadTree(AFileName, fstOpen);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"LOAD_SCXML> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::LoadScxmlData(const UnicodeString & sData, const UnicodeString sEditorCaption) {
	try {
		try {
			this->ResetTree();

			Statemachine::LoadTreeFromScxmlData(TheTree, sData);
		}
		__finally {
			FinalizeLoadTree(sEditorCaption, fstLoadText);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"LOAD_SCXML_DATA> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::LoadScxmlStream(TStream * AStream, const UnicodeString sEditorCaption) {
	try {
		this->ResetTree();

		AStream->Position = 0;
		LoadTreeFromStreamEx(this->TheTree, AStream);

		FinalizeLoadTree(sEditorCaption, fstLoadStream);
	}
	catch(Exception * E) {
		WLOG_ERROR(L"LOAD_SCXML_STREAM> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::Open1Click(System::TObject * Sender) // диалог открытия файла
{
	if (CheckSave()) {
		if (OpenDialog1->Execute(this->Handle)) {
			LoadTree(OpenDialog1->FileName);
			TeeModified(true, 1, "Open file [" + ExtractFileName(OpenDialog1->FileName) + "] from menu");
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnEditShapeClick(System::TObject * Sender) {
	if (TheTree->Selected->Count() > 0) {
		TTreeNodeShape *AShape = TheTree->Selected->First();
		if (AShape->InheritsFrom(__classid(TScxmlBaseShape))) {
			DoEditScxmlShapes(dynamic_cast<TScxmlBaseShape*>(AShape));
		}
		else {
			EditTreeShape(this, AShape);

			FillNodeTree();
			SetToolBars();
			TeeModified(true, 1, "Shape edited:" + AShape->SimpleText);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ButtonSaveClick(System::TObject * Sender) {
	try {
		if (FStateMachineEditorUnit) {
			FStateMachineEditorUnit->ProjMgr->Save(FStateMachineEditorUnit, false);
			WLOG_DEBUG(L"Save [%s]", FStateMachineEditorUnit->FileName);
		}
		else {
			// может произойти такое, что вызванный модально редактор содержит ошибки,
			// тогда он просто их проигнорирует, поэтому необходимо предварительно протестить на валидность
			// поэтому тестово записываем XML
			try {
				const Statemachine::TIterateSaveTypes ATypes = Statemachine::MaxPossibleTypes()
					>> Statemachine::istComments;

				// только формируем текст
				std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));
				Statemachine::SaveTreeToScxml(TheTree, AStreamPtr.get(), false, true, ATypes);

				this->TeeModified(false);
				this->ModalResult = mrOk;
			}
			catch(Exception * E) {

				WLOG_ERROR(L"MODAL SAVE> %s", E->Message.c_str());

				const UnicodeString Caption = L"SCXML contains errors!";
				const UnicodeString Text = L"Your modifications may be lost! Are you sure to close?";
				if (MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(),
						MB_OKCANCEL | MB_ICONERROR | MB_DEFBUTTON2) == IDOK) {
					this->TeeModified(false);
					this->ModalResult = mrOk;
				}
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"MODAL SAVE> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ButtonNewClick(System::TObject * Sender) {
	// ResetTree();
	ClearFlags();
	ClearDesignObjects();
	TheTree->Clear();
	FillNodeTree();

	// SetInspectorProperties();

	// помечаем, чтобы видно было, что произошло изменение дерева
	TeeModified(true, 1, "Clear all");
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuResetViewToDefault(TObject *Sender) {
	TTreeEditWindows HideWindows = TTreeEditWindows() << teEditors << teFont << teFormat;

	this->TreeHideEditorPanels(HideWindows);

	TheTree->Shapes->Visible = true;
	TheTree->Connections->Visible = true;
	TheTree->CrossBox->Visible = true;
	TheTree->ShowImages = true;
	TheTree->ShowText = true;
	TheTree->CrossBox->Visible = true;

	this->SaveEditorParameters();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuViewPresetsMinimal(TObject *Sender) {
	TTreeEditWindows HideWindows = TTreeEditWindows() << teInspector << teNodeTree << teToolbar << teToolShapes <<
		teEditors << teFont << teFormat << teRulers << TTreeEditWindow::teStatus << teModeTabs << teMainMenu <<
		teSideToolbar;

	this->TreeHideEditorPanels(HideWindows);

	TheTree->Shapes->Visible = true;
	TheTree->Connections->Visible = true;
	TheTree->CrossBox->Visible = true;
	TheTree->ShowImages = true;
	TheTree->ShowText = true;
	TheTree->CrossBox->Visible = true;

	this->SaveEditorParameters();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuViewPresetsCustom(TObject *Sender) {
	std::auto_ptr<TForm>AFormPtr(new TForm(this));
	AFormPtr->Caption = L"View Presets Editor";

	TPanel *APanel = new TPanel(AFormPtr.get());
	APanel->Parent = AFormPtr.get();
	APanel->ShowCaption = false;
	APanel->Height = 40;
	APanel->Align = alTop;
	APanel->AlignWithMargins = true;

	TButton *AButtonAdd = new TButton(AFormPtr.get());
	AButtonAdd->Parent = APanel;
	AButtonAdd->Caption = L"Add";
	AButtonAdd->Align = alLeft;
	AButtonAdd->AlignWithMargins = true;
	AButtonAdd->ModalResult = mrYes;

	TButton *AButtonRemove = new TButton(AFormPtr.get());
	AButtonRemove->Parent = APanel;
	AButtonRemove->Caption = L"Remove";
	AButtonRemove->Align = alLeft;
	AButtonRemove->AlignWithMargins = true;
	AButtonRemove->ModalResult = mrNo;

	TButton *AButtonCancel = new TButton(AFormPtr.get());
	AButtonCancel->Parent = APanel;
	AButtonCancel->Caption = L"Cancel";
	AButtonCancel->Align = alRight;
	AButtonCancel->AlignWithMargins = true;
	AButtonCancel->ModalResult = mrCancel;

	TComboBox *AComboEdit = new TComboBox(AFormPtr.get());
	AComboEdit->Parent = AFormPtr.get();
	AComboEdit->Align = alTop;
	AComboEdit->AlignWithMargins = true;

	// NOTE: use this to garantee Combo over Panel
	AComboEdit->Top = 0;
	APanel->Top = AComboEdit->Height;

	for (int k = 0; k < SettingsData->TempRegistry->Count; k++) {
		const UnicodeString sPrefix = this->ClassName() + ".ViewPresets.";
		if (SettingsData->TempRegistry->Names[k].Pos(sPrefix) == 1) {
			AComboEdit->Items->Add(StringReplace(SettingsData->TempRegistry->Names[k], sPrefix, L"",
					TReplaceFlags() << rfIgnoreCase));
		}
	}

	AFormPtr->AutoSize = true;

	const int iRes = AFormPtr->ShowModal();

	const UnicodeString sPresetName = AComboEdit->Text.Trim();
	if (!sPresetName.IsEmpty()) {
		const UnicodeString sRegKey = this->ClassName() + ".ViewPresets." + sPresetName;

		switch(iRes) {
		case mrYes: {
				SettingsData->TempRegistry->Values[sRegKey] = UnicodeString().sprintf(L"%d;%d",
					this->HideWindowsStatus.ToInt(), PanelNodes->Width);
			}break;
		case mrNo: {
				const int iIndex = SettingsData->TempRegistry->IndexOfName(sRegKey);
				if (iIndex != -1) {
					SettingsData->TempRegistry->Delete(iIndex);
				}
			}break;
		}
	}
	else {
		LOG_WARNING_SS << "Preset name can not be empty!";
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuViewPresetsApply(TObject *Sender) {
	try {
		TMenuItem *AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			const UnicodeString sRegKey = this->ClassName() + ".ViewPresets." + StripHotkey(AMenuItem->Caption);
			const int iIndex = SettingsData->TempRegistry->IndexOfName(sRegKey);
			if (iIndex != -1) {
				std::auto_ptr<TStringList>ADataPtr(new TStringList);
				ADataPtr->StrictDelimiter = true;
				ADataPtr->Delimiter = L';';
				ADataPtr->DelimitedText = SettingsData->TempRegistry->ValueFromIndex[iIndex];

				const TTreeEditWindows AHideWindows(ADataPtr->Strings[0].ToInt());
				TreeHideEditorPanels(AHideWindows);

				PanelNodes->Width = ADataPtr->Strings[1].ToInt();

				this->SaveEditorParameters();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuViewPresets(TObject *Sender) {

	int iIndex = -1;
	for (int i = MenuViewPresets->Count - 1; i > 0; i--) {
		if (MenuViewPresets->Items[i]->Name == L"MenuViewPresetsMinimal") {
			iIndex = i;
			break;
		}
		else {
			MenuViewPresets->Remove(MenuViewPresets->Items[i]);
		}
	}

	for (int k = 0; k < SettingsData->TempRegistry->Count; k++) {
		const UnicodeString sPrefix = this->ClassName() + ".ViewPresets.";
		if (SettingsData->TempRegistry->Names[k].Pos(sPrefix) == 1) {
			TMenuItem *AMenuItem = new TMenuItem(this);
			AMenuItem->Caption = StringReplace(SettingsData->TempRegistry->Names[k], sPrefix, L"",
				TReplaceFlags() << rfIgnoreCase);
			AMenuItem->OnClick = OnMenuViewPresetsApply;
			MenuViewPresets->Insert(++iIndex, AMenuItem);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuViewFullScreen(TObject *Sender) {
	this->ToggleFullScreenMode();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnActionLockAxisExecute(TObject *Sender) {
	TAction *AnAction = dynamic_cast<TAction*>(Sender);
	if (AnAction) {
		TTreeEx *ATree = this->TheTreeEx;
		if (ATree) {

			if (AnAction == ActionLockAxisX) {
				ATree->DragAxis = ATree->DragAxis == axisX ? axisAll : axisX;
			}
			else {
				ATree->DragAxis = ATree->DragAxis == axisY ? axisAll : axisY;
			}

			ATree->Invalidate();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnActionLockAxisUpdate(TObject *Sender) {
	TAction *AnAction = dynamic_cast<TAction*>(Sender);
	if (AnAction) {
		TTreeEx *ATree = this->TheTreeEx;
		if (ATree) {
			if (AnAction == ActionLockAxisX) {
				ActionLockAxisX->Checked = ATree->DragAxis == axisX;
				BtnConstraintX->Down = ActionLockAxisX->Checked;
				BtnConstraintX->Font->Style = ActionLockAxisX->Checked ? (TFontStyles() << fsBold) : TFontStyles();
			}
			else {
				ActionLockAxisY->Checked = ATree->DragAxis == axisY;
				BtnConstraintY->Down = ActionLockAxisY->Checked;
				BtnConstraintY->Font->Style = ActionLockAxisY->Checked ? (TFontStyles() << fsBold) : TFontStyles();
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::Saveas1Click(System::TObject * Sender) {
	try {
		if (FStateMachineEditorUnit) {
			if (FStateMachineEditorUnit->StateMachineProject) {
				FStateMachineEditorUnit->StateMachineProject->MarkModified();
			}
			if (FStateMachineEditorUnit->ProjMgr) {
				FStateMachineEditorUnit->ProjMgr->Save(FStateMachineEditorUnit, true);
			}
			WLOG_DEBUG(L"Save [%s]", FStateMachineEditorUnit->FileName);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuClearSelection(TObject *Sender) {
	TheTree->Selected->Clear();
	TheTree->Connections->Selected = NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectSimilar(TObject *Sender) {
	try {
		std::set<TMetaClass*>AMetaTypes;
		for (int i = 0; i < TheTree->Selected->Count(); i++) {
			AMetaTypes.insert(TheTree->Selected->Items[i]->ClassType());
		}

		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (AMetaTypes.find(TheTree->Items->Items[i]->ClassType()) != AMetaTypes.end()) {
				TheTree->Items->Items[i]->Selected = true;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectVisual(TObject *Sender) {
	try {
		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (TVisualScxmlBaseShape * AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
				(TheTree->Items->Items[i])) {
				AVisualBaseShape->Selected = true;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectNonVisual(TObject *Sender) {
	try {
		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (TChildScxmlBaseShape * AShape = dynamic_cast<TChildScxmlBaseShape*>(TheTree->Items->Items[i])) {
				AShape->Selected = true;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectCategory(TObject *Sender) {
	try {
		TMenuItem *AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			for (int i = 0; i < TheTree->Items->Count; i++) {
				if (TScxmlBaseShape * AShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Items->Items[i])) {
					if (AShape->StateChildType == TStateChildType(AMenuItem->Tag)) {
						AShape->Selected = true;
					}
				}
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectAtomic(TObject *Sender) {
	try {
		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (TVisualScxmlBaseShape * AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
				(TheTree->Items->Items[i])) {
				if (!AVisualBaseShape->IsCluster()) {
					AVisualBaseShape->Selected = true;
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectCompound(TObject *Sender) {
	try {
		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (TVisualScxmlBaseShape * AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
				(TheTree->Items->Items[i])) {
				if (AVisualBaseShape->IsCluster()) {
					AVisualBaseShape->Selected = true;
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectStatesandParallel(TObject *Sender) {
	try {
		std::set<TStateChildType>ATypes;
		ATypes.insert(sctState);
		ATypes.insert(sctParallel);
		ATypes.insert(sctVirtual);

		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (TVisualScxmlBaseShape * AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
				(TheTree->Items->Items[i])) {
				if (ATypes.find(TStateChildType(AVisualBaseShape->StateChildType)) != ATypes.end()) {
					AVisualBaseShape->Selected = true;
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuSelectExecutableContent(TObject *Sender) {
	try {
		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (TScxmlBaseShape * AShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Items->Items[i])) {
				if (STATE_TYPES_EXECUTABLE_CONTENT.find(TStateChildType(AShape->StateChildType))
					!= STATE_TYPES_EXECUTABLE_CONTENT.end()) {
					AShape->Selected = true;
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeOnAddingConnection(TTreeNodeShape * Node1, TTreeNodeShape * Node2,
	bool&Add) {
#ifdef _PANIC_DEBUG_
	WLOG_DEBUG(L"Adding node1 [%s] to node2 [%s], forbidden=%s", Node1->SimpleText.c_str(), Node2->SimpleText.c_str(),
		FForbidInternalAdd ? L"true" : L"false");
#endif
	if (FForbidInternalAdd) {
		Add = false;
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeExpandingCollapsing(TTreeNodeShape * Sender, bool&Expand) {
	TVisualScxmlBaseShape *AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(Sender);
	if (AVisualBaseShape) {
		Expand = true;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnShapeMoveResized() {
	bool bNeedToFillNodeTree = false;

	for (int i = 0; i < TheTree->Selected->Count(); i++) {
		TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(TheTree->Selected->Items[i]);
		if (AVisualShape) {

			std::vector<TVisualScxmlBaseShape*>VecPossibleParents;
			std::vector<TVisualScxmlBaseShape*>VecPossibleChildren;
			std::vector<TVisualScxmlBaseShape*>VecPossibleLostChildren;

			CollectPossibleParentsAndChildrenForExchange(AVisualShape, VecPossibleParents, VecPossibleChildren,
				VecPossibleLostChildren, 0, 0);

			if (!VecPossibleParents.empty()) {
				bNeedToFillNodeTree = true;
				Editorutils::SetShapeParentEx(AVisualShape, VecPossibleParents.back());
			}
			else {
				for (std::size_t i = 0; i < VecPossibleChildren.size(); i++) {
					bNeedToFillNodeTree = true;
					Editorutils::SetShapeParentEx(VecPossibleChildren[i], AVisualShape);
				}

				if (AVisualShape->VisualParent) {
					for (std::size_t i = 0; i < VecPossibleLostChildren.size(); i++) {
						bNeedToFillNodeTree = true;
						Editorutils::SetShapeParentEx(VecPossibleLostChildren[i], AVisualShape->Parent);
					}
				}
			}
		}
	}

	if (bNeedToFillNodeTree) {
		FillNodeTree();
	}

	TTreeEditorEx::OnShapeMoveResized();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnConnectionReconnected(TTreeConnection * AConnection) {
	TStateMachineConnection *AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(AConnection);
	if (AStateMachineConnection && AStateMachineConnection->IsSelfConnection) {
		AStateMachineConnection->MakeSelfConnection();
	}

	TTreeEditorEx::OnConnectionReconnected(AConnection);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnConnectionStyleChanged(TTreeConnection * AConnection) {
	TStateMachineConnection *AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(AConnection);
	if (AStateMachineConnection && AStateMachineConnection->IsSelfConnection) {
		AStateMachineConnection->MakeSelfConnection();
	}

	TTreeEditorEx::OnConnectionStyleChanged(AConnection);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ExamChangeParent(Teetree::TTreeNodeShape * Sender, int DeltaX, int DeltaY) {
	TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(Sender);
	if (AVisualShape) {
		std::vector<TVisualScxmlBaseShape*>VecPossibleParents;
		std::vector<TVisualScxmlBaseShape*>VecPossibleChildren;
		std::vector<TVisualScxmlBaseShape*>VecPossibleLostChildren;

		CollectPossibleParentsAndChildrenForExchange(AVisualShape, VecPossibleParents, VecPossibleChildren,
			VecPossibleLostChildren, DeltaX, DeltaY);

		if (!VecPossibleParents.empty()) {
			VecPossibleParents.back()->Examined = true;
		}
		AVisualShape->Examined = !VecPossibleChildren.empty();

		for (std::size_t i = 0; i < VecPossibleLostChildren.size(); i++) {
			VecPossibleLostChildren[i]->Examined = true;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeMovingShape(Teetree::TTreeNodeShape * Sender, int&DeltaX, int&DeltaY) {
	// запрещаем смещение дочерних фигур
	TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(Sender);
	if (AChildScxmlBaseShape) {
		DeltaX = 0;
		DeltaY = 0;
		return;
	}

	TTextDummyShape *ADummyShape = dynamic_cast<TTextDummyShape*>(Sender);

	if (ADummyShape && TTextDummyShape::IsClickSendEnabled()) {
		DeltaX = 0;
		DeltaY = 0;
		return;
	}

	TTreeEx *ATreeEx = this->TheTreeEx;
	if (ATreeEx) {
		if (ATreeEx->DragAxis == axisY) {
			DeltaX = 0;
		}
		else if (ATreeEx->DragAxis == axisX) {
			DeltaY = 0;
		}
	}

	TTreeEditorEx::TheTreeMovingShape(Sender, DeltaX, DeltaY);

	if (ADummyShape && ADummyShape->Connection) {
		ADummyShape->Connection->Text->HorizOffset = ADummyShape->Connection->Text->HorizOffset + DeltaX;
		ADummyShape->Connection->Text->VertOffset = ADummyShape->Connection->Text->VertOffset + DeltaY;
		// здесь изменение можно не вызывать, оно добавится при отпускании мыши или клавиши
	}

	ExamChangeParent(Sender, DeltaX, DeltaY);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeResizingShape(Teetree::TTreeNodeShape * Sender,
	Teetree::TTreeShapeHandle ACorner, int&DeltaX, int&DeltaY) {

	// запрещаем изменение размера дочерних фигур
	if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(Sender)) {
		DeltaX = 0;
		DeltaY = 0;
		return;
	}

	TTreeEditorEx::TheTreeResizingShape(Sender, ACorner, DeltaX, DeltaY);

	ExamChangeParent(Sender, DeltaX, DeltaY);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeKeyDown(System::TObject* Sender, System::Word &Key,
	Classes::TShiftState Shift) {
	try {

		if (FFrameTransitionTypes && FFrameTransitionTypes->Visible) {
			FFrameTransitionTypes->Visible = false;
		}

		switch(Key) {
		case VK_RETURN: {
				if (Shift == Classes::TShiftState() << ssCtrl) {
					if (TScxmlBaseShape * AScxmlBaseShape = FirstSelected) {
						// NOTE: we need to disable expand-collapse on press 'Ctrl+Enter'
						Key = 0;
					}
				}
			}break;
		}

		TTreeEditorEx::TheTreeKeyDown(Sender, Key, Shift);

		switch(Key) {
		case 'R':
			if (Shift.Contains(ssCtrl)) {
				OnMenuReplaceText(FMenuReplaceText);

				if (Shift.Contains(ssShift)) {
					TTreeNodeShape *AShape = GetSafeTreeListFirst(TheTree->Selected->Shapes);
					if (AShape && FFormReplaceDlg) {
						FFormReplaceDlg->ComboSearch->Text = AShape->SimpleText;
					}
				}
			}
			break;
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"KEY DOWN> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeKeyUp(System::TObject* Sender, System::Word &Key,
	Classes::TShiftState Shift) {
	switch(Key) {
	case VK_RETURN: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				if (TScxmlBaseShape * AScxmlBaseShape = FirstSelected) {
					PostMessage(this->Handle, WM_SCXML_EDIT_TREE_SHAPE, (WPARAM)this, (LPARAM)AScxmlBaseShape);
				}
			}
		}break;
	}

	TTreeEditorEx::TheTreeKeyUp(Sender, Key, Shift);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeMouseDown(System::TObject* Sender, Controls::TMouseButton Button,
	Classes::TShiftState Shift, int X, int Y) {

	int iChartX = X;
	int iChartY = Y;

	if (FFrameTransitionTypes && FFrameTransitionTypes->Visible) {
		FFrameTransitionTypes->Visible = false;
	}

	TTreeEditorEx::TheTreeMouseDown(Sender, Button, Shift, X, Y);

	if (!this->TheTree->CancelMouse && !this->TheTree->Connecting && Button ==
		mbLeft /* && Shift == (TShiftState() << ssLeft) */ ) {
		// ЗДЕСЬ ИЗМЕНЯТСЯ 'X', 'Y' !!!
		TheTree->Canvas->Calculate2DPosition(iChartX, iChartY, TeeTreeZ);

		// выделяем текст на соединении
		if (TheTree->View3DOptions->ZoomFloat > 50) {
			for (int i = 0; i < TheTree->Connections->Count; i++) {
				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
					(TheTree->Connections->Items[i]);
				if (AStateMachineConnection && !AStateMachineConnection->IsTextEmpty() && Intersect
					(AStateMachineConnection->GetBounds(), TheTreeEx->Bounds2D)) {

					if (AStateMachineConnection->IsPointInTextPolygon(iChartX, iChartY)) {
						CreateDummyShape(this, TheTree, AStateMachineConnection);
						break;
					}
				}
			}
		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeMouseUp(System::TObject* Sender, Controls::TMouseButton Button,
	Classes::TShiftState Shift, int X, int Y) {

	// пока что нужны только в одном месте, преобразуем там,
	// но если понадобится в нескольких местах, то перенести 'Calculate2DPosition' сюда
	int iChartX = X;
	int iChartY = Y;

	TTreeEditorEx::TheTreeMouseUp(Sender, Button, Shift, X, Y);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State,
	bool &Accept) {
	const bool bSelectionDragReady = TheTree->Selected->Count() && AreAllSelectedOfInheritedClass
		(__classid(TChildScxmlBaseShape)) && AreAllSelectedBrothers();

	TChildScxmlBaseShape * ATargetShape = dynamic_cast<TChildScxmlBaseShape*>(TheTree->ClickedShape(X, Y));

	Accept = bSelectionDragReady && ATargetShape && //
	ATargetShape->Parent && //
	(ATargetShape->Parent == TheTree->Selected->First()->Parent) && //
	(TheTree->Selected->Shapes->IndexOf(ATargetShape) == -1);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y) {
	try {
		const bool bSelectionDragReady = TheTree->Selected->Count() && AreAllSelectedOfInheritedClass
			(__classid(TChildScxmlBaseShape)) && AreAllSelectedBrothers();

		if (bSelectionDragReady) {

			TChildScxmlBaseShape * ATargetShape = dynamic_cast<TChildScxmlBaseShape*>(TheTree->ClickedShape(X, Y));

			const bool bAccept = bSelectionDragReady && ATargetShape && //
			ATargetShape->Parent && //
			(ATargetShape->Parent == TheTree->Selected->First()->Parent) && //
			TheTree->Selected->Shapes->IndexOf(ATargetShape) == -1;

			if (bAccept) {
				std::vector<TTreeNodeShape*>AShapes;

				// выделение может быть вперемешку, поэтому сортируем по индексу
				std::map<int, TTreeNodeShape*>ASortSelections;
				for (int k = 0; k < TheTree->Selected->Count(); k++) {
					ASortSelections[TheTree->Selected->Items[k]->BrotherIndex] = TheTree->Selected->Items[k];
				}

				for (int i = 0; i < ATargetShape->Parent->Children->Count; i++) {
					if (TheTree->Selected->Shapes->IndexOf(ATargetShape->Parent->Children->Items[i]) != -1)
						continue;

					// 1) если снизу вверх
					const bool bDirectionLess = static_cast<int>(AShapes.size()) < ATargetShape->Parent->Children->Items
						[i]->BrotherIndex;
					if (bDirectionLess) {
						AShapes.push_back(ATargetShape->Parent->Children->Items[i]);
					}

					// вставляем отсортированные фигуры из выделения
					if (ATargetShape->Parent->Children->Items[i] == ATargetShape) {
						for (std::map<int, TTreeNodeShape*>::iterator it = ASortSelections.begin();
							it != ASortSelections.end(); ++it) {
							AShapes.push_back(it->second);
						}
					}

					// 2) если сверху вниз
					if (!bDirectionLess) {
						AShapes.push_back(ATargetShape->Parent->Children->Items[i]);
					}
				}

				// теперь приводим все индексы по порядку
				for (std::size_t n = 0; n < AShapes.size(); n++) {
					AShapes[n]->BrotherIndex = n;
				}

				FillNodeTree();
				TeeModified(True, 1, "Drag 'n Drop to :" + ATargetShape->SimpleText);

			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeDeletingConnection(Teetree::TTreeConnection* Sender, bool &AllowDelete) {
	TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(Sender);
	if (AStateMachineConnection && AStateMachineConnection->Locked) {
		WLOG_ERROR(L"Can not delete inherited or locked connection:[%s]! Reset inheritance or lock to delete!",
			AStateMachineConnection->Name.c_str());
		AllowDelete = false;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeMouseWheelEvent(System::TObject* Sender, Classes::TShiftState Shift,
	int WheelDelta, const Types::TPoint &MousePos, bool &Handled) {

	if (FFrameTransitionTypes && FFrameTransitionTypes->Visible) {
		FFrameTransitionTypes->Visible = false;
	}

	// каждое движение колеса мыши вызывает перерисовку, что влечет за собой проверку на наследование
	FMouseWheelEventTimedFlag = std::make_pair(true, Dateutils::IncMilliSecond(Now(), 64));
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SetNewTreeSettings(void) {

	/* inherited */
	TTreeEditorEx::SetNewTreeSettings(); // Base Class

	TheTree->OnStartEditing = TheTreeStartEdit;
	TheTree->OnStopEditing = TheTreeStopEditing;
	TheTree->OnNewConnection = TheTreeNewConnection;
	TheTree->OnAddingConnection = TheTreeOnAddingConnection;

	TheTree->OnMovingShape = TheTreeMovingShape;
	TheTree->OnResizingShape = TheTreeResizingShape;
	TheTree->OnDeletingShapes = TheTreeDeletingShapes;
	TheTree->OnDeletedShapes = TheTreeDeletedShapes;
	TheTree->OnSelectShape = TheTreeSelectShape;
	TheTree->OnUnSelectShape = TheTreeUnSelectShape;
	TheTree->OnSelectConnection = TheTreeSelectConnection;
	TheTree->OnUnSelectConnection = TheTreeUnSelectConnection;
	TheTree->OnDblClickShape = TheTreeDblClickShape;
	TheTree->OnDblClickConnection = TheTreeDblClickConnection;
	TheTree->OnExpandingCollapsing = TheTreeExpandingCollapsing;
	TheTree->OnAfterDraw = TheTreeAfterDraw;
	TheTree->OnZoom = NULL; // в родительском классе при изменении масштаба дерево маркируется измененным состоянием, поэтому убираем

	/* TheTree - это TCustomTree, у которого нет доступа к некоторым свойствам */
	TTree *ATree = dynamic_cast<TTree*>(TheTree);
	if (ATree) {
		ATree->OnDragOver = TheTreeDragOver;
		ATree->OnDragDrop = TheTreeDragDrop;
		ATree->OnMouseWheel = TheTreeMouseWheelEvent;
	}

	TheTree->ChangeManager(new TScxmlAlignChildEx);

	TheTree->GlobalFormat.ConnectionClass = __classid(TStateMachineConnection);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::LoadEditorParametersEx(void) {
	if (IsStandalone) {
		const TTreeEditWindows AHideWindows(SettingsData->TempRegistry->Values[this->ClassName() + ".HideWindows"]
			.ToIntDef(DEFAULT_HIDE_WINDOWS_STATUS.ToInt()));
		TreeHideEditorPanels(AHideWindows);

		PanelNodes->Width = SettingsData->TempRegistry->Values[this->ClassName() + ".PanelNodes.Width"].ToIntDef
			(PanelNodes->Width);
	}
	else {
		TreeHideEditorPanels(SettingsData->HideWindows);

		PanelNodes->Width = SettingsData->EditorNodeTreeWidth;
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveEditorParameters(void) {
	// запомним редакторы только, если в режиме юнита
	if (IsStandalone) {
		SettingsData->TempRegistry->Values[this->ClassName() + ".HideWindows"] = this->HideWindowsStatus.ToInt();
		SettingsData->TempRegistry->Values[this->ClassName() + ".PanelNodes.Width"] = PanelNodes->Width;
	}
	else {
		SettingsData->HideWindows = this->HideWindowsStatus;
		SettingsData->EditorNodeTreeWidth = PanelNodes->Width;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::UpdateAllChildrenSimpleText() {
	if (TheTree) {
		const Editorutils::TreeUpdateLock ATreeLock(TheTree);

		for (int i = 0; i < TheTree->Items->Count; i++) {
			TChildScxmlBaseShape * AShape = dynamic_cast<TChildScxmlBaseShape*>(TheTree->Items->Items[i]);
			// проверяем, чтобы не было дочерних, тогда вверх по цепочке всё проверится
			if (AShape && !AShape->Children->Count) {
				AShape->UpdateSimpleText();
			}
		}

	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::DoUndo(void) {
	// обязательно убираем выделение, так как восстановятся новые объекты
	ClearDesignObjects();

	ClearFlags();

	TTreeEditorEx::DoUndo(); // inherited

	RestoreInspectorState();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeStartEdit(TTreeNodeShape* Shape, bool &AllowEditing) {
	if (Shape) {
		std::set<TMetaClass*>AForbiddenTypes;
		AForbiddenTypes.insert(__classid(THistoryShape));
		AForbiddenTypes.insert(__classid(TFinalShape));
		AForbiddenTypes.insert(__classid(TInitialShape));
		AForbiddenTypes.insert(__classid(TTextDummyShape));

		// специально запрещенные
		if (AForbiddenTypes.find(Shape->ClassType()) != AForbiddenTypes.end()) {
			AllowEditing = false;
		}
		else if (Shape->InheritsFrom(__classid(TChildScxmlBaseShape))) {
			AllowEditing = false;
		}
		else {
			if (TheTree->TextEditor->Memo) {
				TheTree->TextEditor->Memo->OnKeyPress = TheTreeEx->OnEditorBlockReturnKeyPress;

				TheTree->TextEditor->Memo->Font->Size = TheTree->View3DOptions->Zoom > 100 ? //
				((double)Shape->Font->Size * (double)TheTree->View3DOptions->Zoom * 0.01f) : Shape->Font->Size;

			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeStopEditing(TTreeNodeShape * Sender) {
	TTreeEditorEx::TheTreeStopEditing(Sender);

	TeeModified(true, 1, "New name [" + (Sender ? Sender->SimpleText : UnicodeString("")) + "]");

	if (PropSettingsInspector) {
		PropSettingsInspector->UpdateContent();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::NodeTreeStopEditing(Teetree::TTreeNodeShape * Sender) {
	TTreeEditorEx::NodeTreeStopEditing(Sender);

	TeeModified(true, 1, "New name [" + Sender->SimpleText + "]");
	// modify уже внутри базового класса
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeNewConnection(TCustomTree * Sender, TTreeConnection * NewConnection) {
	TStateMachineConnection* AConnection = dynamic_cast<TStateMachineConnection*>(NewConnection);
	if (AConnection) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>(AConnection->ToShape);
		if (AShape) {
			// на себя
			if (AShape == AConnection->FromShape) {
				AConnection->MakeSelfConnection();
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SpeedLinkClick(System::TObject * Sender) {
	const bool bPrevNoOwnerShapes = TheTree->NoOwnerShapes;

	// указатель для соединения, связывающего 2 визуальные фигуры
	TStateMachineConnection * ANewConnection = NULL;
	bool bIsSelfConnection = false;

	// set the Tree in "connecting" mode...
	if (SpeedLink->Down) {
		// start connecting two shapes...
		StatusBar1->Panels->Items[stPanelText]->Text = "Connecting two shapes. Press 'ESC' to cancel";
		StatusBar1->Update();

		// запрещаем внутреннее создание узлов, иначе невозможно создать множественные связи
		FForbidInternalAdd = true;

		// помечаем курсор, чтобы было видно, что операция блокирует Tree
		Screen->Cursor = crAppStart;

		// эта часть касается варианта, когда вызвано по Ctrl+Right
		int iX = 0;
		int iY = 0;
		TVisualScxmlBaseShape *AVisualFromShape = NULL;
		if (TheTree->MouseInClient) {
			const TPoint APtCursor = TheTree->GetCursorPos();
			iX = APtCursor.x;
			iY = APtCursor.y;
			TheTree->Canvas->Calculate2DPosition(iX, iY, TeeTreeZ);
			AVisualFromShape = dynamic_cast<TVisualScxmlBaseShape*>(TheTree->ClickedShape(APtCursor.x, APtCursor.y));
		}

		// Initiate mouse linking of nodes...
		TTreeConnection *AConnecting = TheTree->StartConnecting(iX, iY, AVisualFromShape);

		// if user clicked on two nodes, link them...
		// разрешаем связывать только Scxml фигуры
		if (TheTree->ConnectingShape1 && TheTree->ConnectingShape2 && TheTree->ConnectingShape1->InheritsFrom
			(__classid(TVisualScxmlBaseShape)) && TheTree->ConnectingShape2->InheritsFrom
			(__classid(TVisualScxmlBaseShape))) {

			// самостоятельное определение связи,
			// иначе множественные невозможны
			ANewConnection = new TStateMachineConnection(this);
			const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, ANewConnection->ClassType());
			ANewConnection->Name = AUniqueName.second;
			ANewConnection->FromShape = TheTree->ConnectingShape1;
			ANewConnection->ToShape = TheTree->ConnectingShape2;

			ANewConnection->Tree = TheTree;

			if (ANewConnection) {
				ANewConnection->Style = csLine;

				TheTree->Selected->Clear();
				TheTree->Connections->Selected = ANewConnection;
			}

			// так как запрещаем "родное добавление", то не будет работать событие OnNewConnection
			if (TheTree->ConnectingShape1 == TheTree->ConnectingShape2) {
				TheTreeNewConnection(TheTree, ANewConnection);
				bIsSelfConnection = true;
			}
		}

	}

	Screen->Cursor = crDefault;
	StopConnecting();

	SpeedNormal->Down = True;
	FForbidInternalAdd = false;

	if (ANewConnection) {

		// при множественных соединениях, новое может создаться в одинаковых точках с другими, если они по центру
		if (!bIsSelfConnection) {
			Editorutils::ProtectConnectionToBeInSamePoints(ANewConnection);
			// смещаем, пока соединение не попадет в отдельные координаты

			FTreeNewConnectionToolbarFlag = true;
		}

		const UnicodeString sFrom = ANewConnection->FromShape ? ANewConnection->FromShape->SimpleText : UnicodeString
			(L"");
		const UnicodeString sTo = ANewConnection->ToShape ? ANewConnection->ToShape->SimpleText : UnicodeString(L"");
		FillNodeTree();
		TeeModified(True, 1, UnicodeString().sprintf(L"Connecting shape[%s] with shape[%s]", sFrom.c_str(),
				sTo.c_str()));
	}
	else {
		// эта операция сотрет индикацию связывания, если ничего не создано
		TheTree->Invalidate();
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuChangeConnectionTypeFrameTransition(System::TObject* Sender) {
	OnMenuChangeConnectionType(Sender);
	if (FFrameTransitionTypes) {
		FFrameTransitionTypes->Visible = false;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMenuConnectionOverFrameTransition(System::TObject* Sender) {
	OnMenuConnectionOver(Sender);
	if (FFrameTransitionTypes) {
		FFrameTransitionTypes->Visible = false;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeDeletingShapes(TSelectedShapeList * Sender, bool&AllowDelete) {
	if (Sender) {
		// запрет удаления наследованных фигур
		for (int i = 0; i < Sender->Count(); i++) {
			TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(Sender->Items[i]);
			if (AScxmlBaseShape && AScxmlBaseShape->Locked) {
				AllowDelete = false;
				WLOG_ERROR(L"Shape [%s] is locked and can not be deleted!", AScxmlBaseShape->SimpleText.c_str());
				return; // запрещаем удаление
			}
		}

		// в форме для замены могут содержаться указатели на фигуры, поэтому при удалении, проверим
		if (FFormReplaceDlg) {
			for (int i = 0; i < Sender->Count(); i++) {
				if (FFormReplaceDlg->ReplaceShape == Sender->Items[i]) {
					delete FFormReplaceDlg;
					FFormReplaceDlg = NULL;
					break;
				}
			}
		}
	}

	ClearDesignObjects();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeDeletedShapes(TObject * Sender) {
	SettingsData->ProfilerReset(__FUNCTION__);

	TTreeEditorEx::TheTreeDeletedShapes(Sender);

	UpdateAllChildrenSimpleText();

	TScxmlBaseShape::ValidateSimpleText(TheTree);

	SettingsData->ProfileOperationReset(__FUNCTION__);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeSelectShape(Teetree::TTreeNodeShape * Sender) {
	bool bIsNotDummyShape = Sender && !Sender->InheritsFrom(__classid(TTextDummyShape));

	// блокируем возможность группового выделения с Dummy
	if (bIsNotDummyShape) {
		std::set<TTextDummyShape*>ADummyShapes;
		for (int i = 0; i < TheTree->Selected->Count(); i++) {
			if (TTextDummyShape * ATTextDummyShape = dynamic_cast<TTextDummyShape*>(TheTree->Selected->Items[i])) {
				ADummyShapes.insert(ATTextDummyShape);
			}
		}

		for (std::set<TTextDummyShape*>::iterator it = ADummyShapes.begin(); it != ADummyShapes.end(); ++it) {
			(*it)->Selected = false;
		}
	}

	FTreeSelectionChanged = true;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeUnSelectShape(Teetree::TTreeNodeShape * Sender) {
	if (dynamic_cast<TTextDummyShape*>(Sender)) {
		PostMessage(this->Handle, WM_SCXML_DELETE_TREE_SHAPE, (WPARAM)this, (LPARAM)Sender);
	}

	if (FFrameTransitionTypes && FFrameTransitionTypes->Visible) {
		FFrameTransitionTypes->Visible = false;
	}

	FTreeSelectionChanged = true;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeSelectConnection(Teetree::TTreeConnection * Sender) {
	FTreeSelectionChanged = true;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeUnSelectConnection(Teetree::TTreeConnection * Sender) {
	/* не пользоваться Sender - он всегда NULL !!! */
	FTreeSelectionChanged = true;

	if (FFrameTransitionTypes && FFrameTransitionTypes->Visible) {
		FFrameTransitionTypes->Visible = false;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMsgDeleteTreeShape(TMessage &AMsg) {
	if (!AMsg.LParam)
		return;

	if ((int)AMsg.WParam != (int)this)
		return;

	TTreeNodeShape *AShape = reinterpret_cast<TTreeNodeShape*>(AMsg.LParam);
	if (TheTree->Shapes->IndexOf(AShape) != -1) {
		delete AShape;
		FillNodeTree();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMsgEditTreeShape(TMessage &AMsg) {
	if (!AMsg.LParam)
		return;

	if ((int)AMsg.WParam != (int)this)
		return;

	TScxmlBaseShape *AScxmlBaseShape = reinterpret_cast<TScxmlBaseShape*>(AMsg.LParam);
	if (TheTree->Shapes->IndexOf(AScxmlBaseShape) != -1) {
		DoEditScxmlShapes(AScxmlBaseShape);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SelectConnection(TTreeConnection * AConnection,
	const bool bScrollInView /* = SCROLL_CHART_IN_VIEW */ ) {
	ClearDesignObjects();

	TTreeEditorEx::SelectConnection(AConnection, bScrollInView);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::DoTheTreeSelectConnection(Teetree::TTreeConnection* Sender) {

	TTreeEditorEx::DoTheTreeSelectConnection(Sender);

	TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(Sender);
	if (AStateMachineConnection) {

		if (this != TransitionXMLEditor) {
			TransitionXMLEditor->SetStateMachineConnection(AStateMachineConnection);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::CheckInheritedMismatches() {
	if (!FrameSearchResults)
		throw Exception(L"PROGRAM LOGIC ERROR> FrameSearchResults == NULL!");

	FrameSearchResults->HTMLSearch->Clear();

	TStateMachineEditor *AInheritedEditor = StateMachineDockPanel ? StateMachineDockPanel->InheritedEditor : NULL;
	if (AInheritedEditor) {

		/* Check Inherited Shapes */
		for (int i = 0; i < TheTree->Shapes->Count; i++) {
			if (TScxmlBaseShape * ACheckShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Shapes->Items[i])) {

				// значит не наследованная фигура, пропускаем
				if (!ACheckShape->Locked && !ACheckShape->InheritanceMismatch)
					continue;

				Unique::CompareError ACompareError;
				Unique::CompareInheritedShape(ACheckShape, AInheritedEditor->TheTree, ACompareError);

				if (ACheckShape->InheritanceMismatch) {
					TMatchFind AMatch;
					AMatch.sLeft = ACompareError.Mismatch;

					FrameSearchResults->AddMatchNode(this->UnitFileName, ACheckShape->Name, ACheckShape->SimpleText,
						ACompareError.PropName, AMatch);
				}
			}
		}

		/* Check Inherited Connections */
		for (int i = 0; i < TheTree->Connections->Count; i++) {

			if (TStateMachineConnection * ACheckConnection = dynamic_cast<TStateMachineConnection*>
				(TheTree->Connections->Items[i])) {

				// значит не наследованное соединение, пропускаем
				if (!ACheckConnection->Locked && !ACheckConnection->InheritanceMismatch)
					continue;

				Unique::CompareError ACompareError;
				Unique::CompareInheritedConnection(ACheckConnection, AInheritedEditor->TheTree, ACompareError);

				if (ACheckConnection->InheritanceMismatch) {
					TMatchFind AMatch;
					AMatch.sLeft = ACompareError.Mismatch;

					FrameSearchResults->AddMatchNode(this->UnitFileName, ACheckConnection->Name,
						ACheckConnection->SimpleText, ACompareError.PropName, AMatch);
				}
			}
		}
	}

	// необходимо перерисовать изменения по соответствиям
	TheTree->Invalidate();

	if (!FormScxmlGui->DockPanelSearch->Parent) {
		FormScxmlGui->LMDDockSite1->DockControl(FormScxmlGui->DockPanelSearch, FormScxmlGui->LMDDockSite1->RootZone,
			alBottom);
		WLOG_DEBUG(L"Dock panel [%s] is undocked! Dock alBottom!", FormScxmlGui->DockPanelSearch->Caption.c_str());
	}
	FormScxmlGui->DockPanelSearch->Show();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::DoCheckInheritanceAndMarkVisibly(bool &bRequireInvalidateTree,
	int &iNotFoundBaseCount) {
	TStateMachineEditor *AInheritedEditor = StateMachineDockPanel ? StateMachineDockPanel->InheritedEditor : NULL;
	if (AInheritedEditor) {
		// Для глубокой отладки
		// WLOG_DEBUG(L"Check inheritance match=%s Zoom=%f", UnitFileName.c_str(), TheTree->View3DOptions->ZoomFloat);

		/* Check Inherited Shapes */
		for (std::set<TScxmlBaseShape*>::iterator it = FInheritanceCheckShapes.begin();
			it != FInheritanceCheckShapes.end(); ++it) {
			if (TScxmlBaseShape * ACheckShape = (*it)) {
				// PERFORMANCE WARNING! Убрать, если упадет быстродействие
				// проверка, существует ли фигура
				if (TheTree->Shapes->IndexOf(ACheckShape) == -1) {
					continue;
				}

				// WLOG_DEBUG(L"INHERITANCE> Check shape=%s", ACheckShape->SimpleText.c_str());

				const bool bWasMismatch = ACheckShape->InheritanceMismatch;
				const bool bWasLocked = ACheckShape->Locked;

				Unique::CompareError ACompareError;
				Unique::CompareInheritedShape(ACheckShape, AInheritedEditor->TheTree, ACompareError);
				if (ACompareError.Code == Unique::cecOriginShapeNotFound) {
					iNotFoundBaseCount++;
				}

				if (bWasMismatch != ACheckShape->InheritanceMismatch || bWasLocked != ACheckShape->Locked) {
					bRequireInvalidateTree = true;
				}
			}
		}

		/* Check Inherited Connections */
		for (std::set<TStateMachineConnection*>::iterator it = FInheritanceCheckConnections.begin();
			it != FInheritanceCheckConnections.end(); ++it) {

			if (TStateMachineConnection * ACheckConnection = (*it)) {

				// PERFORMANCE WARNING! Убрать, если упадет быстродействие
				// проверка, существует ли соединение
				if (TheTree->Connections->IndexOf(ACheckConnection) == -1) {
					continue;
				}

				const bool bWasMismatch = ACheckConnection->InheritanceMismatch;
				const bool bWasLocked = ACheckConnection->Locked;

				Unique::CompareError ACompareError;
				Unique::CompareInheritedConnection(ACheckConnection, AInheritedEditor->TheTree, ACompareError);
				if (ACompareError.Code == Unique::cecOriginConnectionNotFound) {
					iNotFoundBaseCount++;
				}

				if (bWasMismatch != ACheckConnection->InheritanceMismatch || bWasLocked != ACheckConnection->Locked) {
					bRequireInvalidateTree = true;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::DoOnEditorApplicationEventsIdle(TObject *Sender, bool &Done) {

	if (FTreeSelectionChanged) {
		FTreeSelectionChanged = false;

		try {

			bool bAddedNewSelections = false;

			{
				Editorutils::UniversalUpdateLock<TLMDDesignObjects>ADesignObjectsLock(DesignObjects);

				bool bIsConnectionSelected = false;

				std::set<TPersistent*>ASetSelections;
				if (TheTree->Connections->Selected) {
					ASetSelections.insert(TheTree->Connections->Selected);
					bIsConnectionSelected = true;
				}
				else {
					for (int i = 0; i < TheTree->Selected->Count(); i++) {
						if (TTextDummyShape * ADummyShape = dynamic_cast<TTextDummyShape*>(TheTree->Selected->Items[i])
							) {
							if (this != TransitionXMLEditor && TheTree->Connections->IndexOf(ADummyShape->Connection)
								!= -1) {
								ASetSelections.insert(ADummyShape->Connection);
								bIsConnectionSelected = true;
							}
						}
						else {
							ASetSelections.insert(TheTree->Selected->Items[i]);
						}
					}
				}

				if (this != TransitionXMLEditor && !bIsConnectionSelected) {
					TransitionXMLEditor->ResetStateMachineConnection();
				}

				if (DesignObjects->Count) {
					SaveInspectorState();
				}

				for (int i = DesignObjects->Count - 1; i >= 0; i--) {
					if (ASetSelections.find(DesignObjects->Item[i]) == ASetSelections.end()) {
						DesignObjects->Delete(i);
					}
				}

				bool bFirstAddShape = false;

				for (std::set<TPersistent*>::iterator it = ASetSelections.begin(); it != ASetSelections.end(); ++it) {

					if (int iIndex = DesignObjects->IndexOf(*it) == -1) {

						if (TTreeConnection * ANewConnection = dynamic_cast<TTreeConnection*>(*it)) {

							if (ANewConnection->InheritsFrom(__classid(TStateMachineConnection))) {
								DesignObjects->Add(ANewConnection);
								DoTheTreeSelectConnection(ANewConnection);
								bAddedNewSelections = true;
							}
						}
						else if (TTreeNodeShape * ANewShape = dynamic_cast<TTreeNodeShape*>(*it)) {

							if (TTextDummyShape * ADummyShape = dynamic_cast<TTextDummyShape*>(ANewShape)) {
								// специальный случай, игнорируется здесь
							}
							else {
								DesignObjects->Add(ANewShape);
								bAddedNewSelections = true;
								if (!bFirstAddShape) {
									DoTheTreeSelectShape(ANewShape);
									bFirstAddShape = true;
								}
							}

						}
					}
				}

			}

			ResetNodeTreeSelected();
			SetToolBars();

			if (bAddedNewSelections) {
				RestoreInspectorState();
			}

			// если объектов нет, тогда очистим HTMLInfo
			if (!DesignObjects->Count) {
				if (FormScxmlGui) {
					FormScxmlGui->HTMLPropInfo->Items->Clear();
				}
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}

	/* INHERITANCE MISMATCH CHECK */
	if (!FInheritanceCheckShapes.empty() || !FInheritanceCheckConnections.empty()) {

		// перерисовываем только, если что-то поменялось
		bool bRequireInvalidateTree = false;

		int iNotFoundBaseCount = 0;

		static TDateTime dtLastInheritanceCheck = Now();

		// если не находимся в режиме тестирования
		// если в данный момент ничего не нажато и не было предварительно движение колесом
		if (!SettingsData->IsTesterWorking && !Keyboardutils::IsAnyKeyOrMousePressed()
			&& !FMouseWheelEventTimedFlag.first) {

			const double d_CHECK_MINIMUM_ZOOM = 60.0f;
			const __int64 i_CHECK_MINIMUM_UPDATE_TIME_MSEC = 250;

			// отсекаем мелкие зумы, там уже не видно
			if (TheTree->View3DOptions->ZoomFloat > d_CHECK_MINIMUM_ZOOM && //
				// смотрим по времени, что предыдущее обновление было раньше, чем 250 мсек
				(Dateutils::MilliSecondsBetween(Now(), dtLastInheritanceCheck) > i_CHECK_MINIMUM_UPDATE_TIME_MSEC)) {

				dtLastInheritanceCheck = Now();

				DoCheckInheritanceAndMarkVisibly(bRequireInvalidateTree, iNotFoundBaseCount);
			}

			FInheritanceCheckShapes.clear();
			FInheritanceCheckConnections.clear();
		}
#if 0 // для глубокой отладки
		else {
			WLOG_DEBUG(L"Skipping check inheritance. ShapesCount=%d", FInheritanceCheckConnections.size());
		}
#endif

		if (bRequireInvalidateTree) {
			this->InvalidateTheTreeWithTemp();
		}

		if (iNotFoundBaseCount) {
			GlobalSendBallonMsg(L"INHERITANCE> " + UnicodeString(iNotFoundBaseCount) +
				" base elements were not found!\nSee log for details!", bfWarning);
		}
	}

	// флаг с задержкой в 64 мсек, информирующий о том, что двигалось колесо мыши
	if (FMouseWheelEventTimedFlag.first) {
		if (Now() >= FMouseWheelEventTimedFlag.second) {
			FMouseWheelEventTimedFlag.first = false;
		}
	}

	// извещаем все, кто унаследованы от этого класса о том, что произошли изменения
	if (FTreeModifiedForInheritanceFlag) {
		FTreeModifiedForInheritanceFlag = false;

		if (StateMachineEditorUnit && StateMachineEditorUnit->StateMachineProject) {

			/* inheritance */ {
				std::vector<TStateMachineEditorUnit*>AVecUnits;
				StateMachineEditorUnit->StateMachineProject->CollectInheritedUnits(AVecUnits);

				for (std::size_t i = 0; i < AVecUnits.size(); i++) {
					if (AVecUnits[i] != StateMachineEditorUnit && SameText(AVecUnits[i]->Inherited,
							StateMachineEditorUnit->FileName)) {
						if (AVecUnits[i]->Opened) {
							if (AVecUnits[i]->StateMachineDockPanel && AVecUnits[i]
								->StateMachineDockPanel->StateMachineEditor) {
								if (AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->IsModified()) {
									WLOG_ERROR(
										L"INHERITANCE> You have unsaved modifications in inherited:[%s] from base:[%s]! It may lead to conflicts!", AVecUnits[i]->FileName.c_str(), StateMachineEditorUnit->FileName.c_str());
								}
								else {
									AVecUnits[i]->StateMachineDockPanel->MarkReloadInheritance();
								}
							}
						}
					}
				}
			} /* end - inheritance */

			/* при редактировании виртуального извещаем родителя */
			if (StateMachineEditorUnit->IsVirtual) {
				std::vector<TStateMachineEditorUnit*>AVecUnits;
				CollectAllEnabledUnits(StateMachineEditorUnit->StateMachineProject, AVecUnits);

				for (std::size_t i = 0; i < AVecUnits.size(); i++) {
					if (AVecUnits[i] == StateMachineEditorUnit)
						continue;

					// на нас ссылается
					if (AVecUnits[i]->VirtualDependencies->IndexOf(StateMachineEditorUnit->FileName) != -1) {
						if (!AVecUnits[i]->Opened) {

							const UnicodeString sMsg = UnicodeString().sprintf
								(L"Virtual unit [%s] requires parent [%s] to be opened also!",
								StateMachineEditorUnit->FileName.c_str(), AVecUnits[i]->FileName.c_str());

							WLOG_INFO(L"%s", sMsg.c_str());

							std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(this));
							ADialogWaitPtr->ShowDialog(sMsg);

							AVecUnits[i]->Open(true);

							// переключаем обратно на наш узел
							TStateMachineDockPanel *AOwnDockPanel = StateMachineEditorUnit->StateMachineDockPanel;
							if (AOwnDockPanel) {
								AOwnDockPanel->Show();
							}
						}

						TStateMachineDockPanel *ADockPanel = AVecUnits[i]->StateMachineDockPanel;
						if (ADockPanel) {
							if (!ADockPanel->StateMachineEditor->IsModified()) {
								ADockPanel->StateMachineEditor->MarkModified(1,
									L"Virtual machine:[" + StateMachineEditorUnit->FileName + "] was changed!", false);
							}
						}
					}
				}
			} /* end - virtual */
		}

	}

	/* дана команда создать ToolBar с типами соединения */
	if (FTreeNewConnectionToolbarFlag) {
		FTreeNewConnectionToolbarFlag = false;

		CreateConnectionToolBar(dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Selected));

	} /* FTreeNewConnectionToolbarFlag */

	/* обновить надписи на фигурах */
	if (FTreeValidateSimpleTextFlag) {
		FTreeValidateSimpleTextFlag = false;

		TScxmlBaseShape::ValidateSimpleText(TheTree);
	}

	/* FillNodeTree */
	if (FTreeFillNodeTreeFlag) {
		FUpdateNodeTreeElements.clear(); // надписи итак обновятся
		FTreeFillNodeTreeFlag = false;

		// выравниваем индексы по иерархии
		Editorutils::FixUpShapeListIndexes(TheTree);

		TTreeEditorEx::FillNodeTree();

		for (int i = 0; i < TheTree->Items->Count; i++) {
			// если родитель был визуальным проверим детей
			if (TVisualScxmlBaseShape * AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
				(TheTree->Items->Items[i])) {
				AVisualBaseShape->UpdateStateChildren();
			}
		}
	}

	// обновляем надписи в NodeTree, если фигуры были отмечены
	if (NodeTheTree->Checked) {
		for (std::set<TCustomTreeElement*>::iterator it = FUpdateNodeTreeElements.begin();
			it != FUpdateNodeTreeElements.end(); ++it) {
			UpdateNodeTreeElement(*it);
		}
	}
	FUpdateNodeTreeElements.clear();

	/* может вызывать AccessViolation, поэтому в OnIdle */
	if (FRestoreInspectorStateFlag) {
		FRestoreInspectorStateFlag = false;

		RestoreInspectorState();
	}

	/* INHERITED */
	TTreeEditorEx::DoOnEditorApplicationEventsIdle(Sender, Done);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::GetDataIDs(TStrings *AIDList) {
	if (AIDList) {
		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (TDataChildShape * ADataChildShape = dynamic_cast<TDataChildShape*>(TheTree->Items->Items[i])) {
				if (!ADataChildShape->ExcludeFromSave && !ADataChildShape->Id.IsEmpty()) {
					AIDList->Add(ADataChildShape->Id);
				}
			}
		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::GetStateNames(TStrings *AStateNamesList) {
	if (AStateNamesList) {
		AStateNamesList->Clear();
		for (int i = 0; i < TheTree->Items->Count; i++) {
			if (TVisualScxmlBaseShape * AVisualBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
				(TheTree->Items->Items[i])) {
				if (!AVisualBaseShape->ExcludeFromSave && !AVisualBaseShape->SimpleText.IsEmpty()
					&& STATE_TYPES_SELFCONNECTION_SHAPES.find(AVisualBaseShape->StateChildType)
					!= STATE_TYPES_SELFCONNECTION_SHAPES.end()) {
					AStateNamesList->Add(AVisualBaseShape->SimpleText);
				}
			}
		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::MarkUpdateNodeTreeElement(TCustomTreeElement *AElement) {
	FUpdateNodeTreeElements.insert(AElement);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::CreateConnectionToolBar(TStateMachineConnection *AConnection) {
	if (TheTreeEx && AConnection && AConnection->FromShape && AConnection->ToShape) {

		if (AConnection->Points->Count() >= 2) {
			const int iLast = AConnection->Points->Count() - 1;
			if (!EnsureInView(TPoint(AConnection->Points->Item[iLast].X, AConnection->Points->Item[iLast].Y),
					TheTreeEx->Bounds2D)) {
				TheTreeEx->Refresh();
			}
		}

		if (!FFrameTransitionTypes) {
			FFrameTransitionTypes = new TFrameTransitionTypes(this);
			FFrameTransitionTypes->Height = FFrameTransitionTypes->ToolBar1->Height;
			FFrameTransitionTypes->Parent = this;
			FFrameTransitionTypes->BtnAlignLines->OnClick = OnMenuChangeConnectionTypeFrameTransition;
			FFrameTransitionTypes->BtnAlignSides->OnClick = OnMenuChangeConnectionTypeFrameTransition;
			FFrameTransitionTypes->BtnAlignInvertedSides->OnClick = OnMenuChangeConnectionTypeFrameTransition;
			FFrameTransitionTypes->BtnAlignCurve->OnClick = OnMenuChangeConnectionTypeFrameTransition;

			FFrameTransitionTypes->BtnAlignOverTop->OnClick = OnMenuConnectionOverFrameTransition;
			FFrameTransitionTypes->BtnAlignOverBottom->OnClick = OnMenuConnectionOverFrameTransition;
			FFrameTransitionTypes->BtnAlignOverLeft->OnClick = OnMenuConnectionOverFrameTransition;
			FFrameTransitionTypes->BtnAlignOverRight->OnClick = OnMenuConnectionOverFrameTransition;

		}

		const bool bLeftRightOver = abs(AConnection->FromShape->YCenter() - AConnection->ToShape->YCenter()) > 25.0f;
		const bool bTopBottomOver = abs(AConnection->FromShape->XCenter() - AConnection->ToShape->XCenter()) > 25.0f;

		FFrameTransitionTypes->BtnAlignOverTop->Visible = bTopBottomOver;
		FFrameTransitionTypes->BtnAlignOverBottom->Visible = bTopBottomOver;
		FFrameTransitionTypes->BtnAlignOverLeft->Visible = bLeftRightOver;
		FFrameTransitionTypes->BtnAlignOverRight->Visible = bLeftRightOver;

		FFrameTransitionTypes->AdjustWidth();

		// без этого может неправильно рассчитать позицию FFrameTransitionTypes
		AConnection->ForceCalculateBounds();

		TPoint APtCursor;
		GetCursorPos(&APtCursor);
		APtCursor = this->ScreenToClient(APtCursor);

		const TRect AConnectionBounds = Editorutils::IncrementRectCopy(AConnection->GetBounds(), 15);
		const TPoint AOffsetToParent = Editorutils::GetNestedParentTopLeft(TheTree, this);

		TPoint APixelsTopLeft = TheTreeEx->ConvertToPixels(AConnectionBounds.Left, AConnectionBounds.Top);
		APixelsTopLeft.x += AOffsetToParent.x;
		APixelsTopLeft.y += AOffsetToParent.y;

		TPoint APixelsTopRight = TheTreeEx->ConvertToPixels(AConnectionBounds.Right, AConnectionBounds.Top);
		APixelsTopRight.x += AOffsetToParent.x;
		APixelsTopRight.y += AOffsetToParent.y;

		TPoint APixelsBottomLeft = TheTreeEx->ConvertToPixels(AConnectionBounds.Left, AConnectionBounds.Bottom);
		APixelsBottomLeft.x += AOffsetToParent.x;
		APixelsBottomLeft.y += AOffsetToParent.y;

		TPoint APixelsBottomRight = TheTreeEx->ConvertToPixels(AConnectionBounds.Right, AConnectionBounds.Bottom);
		APixelsBottomRight.x += AOffsetToParent.x;
		APixelsBottomRight.y += AOffsetToParent.y;

		const int iDistanceTL = Editorutils::DistanceBetweenPoints(APixelsTopLeft.x, APixelsTopLeft.y, APtCursor.x,
			APtCursor.y);
		const int iDistanceTR = Editorutils::DistanceBetweenPoints(APixelsTopRight.x, APixelsTopRight.y, APtCursor.x,
			APtCursor.y);
		const int iDistanceBL = Editorutils::DistanceBetweenPoints(APixelsBottomLeft.x, APixelsBottomLeft.y,
			APtCursor.x, APtCursor.y);
		const int iDistanceBR = Editorutils::DistanceBetweenPoints(APixelsBottomRight.x, APixelsBottomRight.y,
			APtCursor.x, APtCursor.y);

		int iXPos = APixelsTopLeft.x;
		int iYPos = APixelsTopLeft.y - FFrameTransitionTypes->Height;

		TPoint ANewCursor = APixelsTopLeft;

		if (AConnection->Points->Count() >= 2) {
			const int iLast = AConnection->Points->Count() - 1;

			const bool bToLeft = AConnection->Points->Item[0].X > AConnection->Points->Item[iLast].X;
			const bool bToTop = AConnection->Points->Item[0].Y > AConnection->Points->Item[iLast].Y;

			FFrameTransitionTypes->BtnAlignSides->ImageIndex = bToLeft ?
				i_SIDES_LEFT_DIRECTION : i_SIDES_RIGHT_DIRECTION;
			FFrameTransitionTypes->BtnAlignInvertedSides->ImageIndex = bToTop ?
				i_INVERTED_SIDES_TOP_DIRECTION : i_INVERTED_SIDES_BOTTOM_DIRECTION;
		}

		int iDistance = iDistanceTL;
		if (iDistanceTR < iDistance) {
			iDistance = iDistanceTR;
			iXPos = APixelsTopRight.x - FFrameTransitionTypes->Width;
			iYPos = APixelsTopRight.y - FFrameTransitionTypes->Height;
			ANewCursor = APixelsTopRight;
		}
		if (iDistanceBL < iDistance) {
			iDistance = iDistanceBL;
			iXPos = APixelsBottomLeft.x;
			iYPos = APixelsBottomLeft.y;
			ANewCursor = TPoint(APixelsBottomLeft.x, APixelsBottomLeft.y + FFrameTransitionTypes->Height);
		}
		if (iDistanceBR < iDistance) {
			iDistance = iDistanceBR;
			iXPos = APixelsBottomRight.x - FFrameTransitionTypes->Width;
			iYPos = APixelsBottomRight.y;
			ANewCursor = TPoint(APixelsBottomRight.x, APixelsBottomRight.y + FFrameTransitionTypes->Height);
		}

		FFrameTransitionTypes->Left = iXPos;
		FFrameTransitionTypes->Top = iYPos;
		FFrameTransitionTypes->SetAlignType(csLine);

		ANewCursor = this->ClientToScreen(ANewCursor);
		SetCursorPos(ANewCursor.x, ANewCursor.y);

		FFrameTransitionTypes->Visible = true;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SelectShape(TTreeNodeShape * AShape,
	const bool bScrollInView /* = SCROLL_CHART_IN_VIEW */ ) {
	ClearDesignObjects();

	TTreeEditorEx::SelectShape(AShape, bScrollInView);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SelectMultipleShape(TTreeNodeShape *AShape, const bool bClear,
	const bool bScrollInView) {
	if (bClear) {
		ClearDesignObjects();

		TheTree->Selected->Clear();
	}

	AShape->Selected = true;

	if (bScrollInView) {
		CenterInView(AShape);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::DoEditScxmlShapes(TScxmlBaseShape * ABaseShape) {
	try {
		/* ВИЗУАЛЬНЫЕ ФИГУРЫ */
		if /* ОСОБЫЙ СЛУЧАЙ! VIRTUAL */ (ABaseShape->InheritsFrom(__classid(TVirtualShape))) {
			TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>(ABaseShape);
			if (AVirtualShape) {
				if (AVirtualShape->EditSrc()) {
					TeeModified(True, 1, "Virtual Unit:" + AVirtualShape->Src);
				}
			}
		}
		else if (ABaseShape->InheritsFrom(__classid(TVisualScxmlBaseShape))) {
			TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(ABaseShape);
			if (AVisualShape) {
				if (AVisualShape->EditRawXMLContent()) {
					AVisualShape->UpdateStateChildren();
					FillNodeTree();
					TeeModified(True, 1, "Edit RAW XML:" + AVisualShape->SimpleText);
				}
			}
		}
		/* НЕВИЗУАЛЬНЫЕ ФИГУРЫ */
		else if (ABaseShape->InheritsFrom(__classid(TChildScxmlBaseShape))) {
			TChildScxmlBaseShape *AChildShape = dynamic_cast<TChildScxmlBaseShape*>(ABaseShape);
			if (AChildShape) {
				/* НАЗНАЧЕНЫ РЕДАКТОРЫ */
				if (AChildShape->ClassNameIs("TCommentShape")) {
					if (AChildShape->Comments->EditXMLText()) {
						TeeModified(True, 1, "Edit Comments:" + AChildShape->SimpleText);
					}
				}
				else if (AChildShape->ClassNameIs("TExtraContentShape")) {
					if (AChildShape->ExtraContent->EditXMLText()) {
						TeeModified(True, 1, "Edit ExtraContent:" + AChildShape->SimpleText);
					}
				}
				else if (AChildShape->ClassNameIs("TProtocolBindingShape")) {
					TProtocolBindingShape *ABindingShape = dynamic_cast<TProtocolBindingShape*>(AChildShape);
					if (ABindingShape && ABindingShape->ProtocolControlBinding->EditBinding()) {
						TeeModified(True, 1, "Edit ProtocolBindingShape:" + AChildShape->SimpleText);
					}
				}
				else if (ABaseShape->InheritsFrom(__classid(TWatchShape))) {
					TWatchShape * AWatchShape = dynamic_cast<TWatchShape*>(ABaseShape);
					if (AWatchShape) {
						if (AWatchShape->EditRegExp()) {
							TeeModified(True, 1, "Edit Watch RegExp:" + AWatchShape->RegExp);
						}

					}
				}
				/* здесь все фигуры, для которых может быть XMLText и дочерние */
				else {

					/* все остальные фигуры */
					AChildShape->DefaultEditProperty();
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	/// !!! не знаю, зачем я это добавил, но не удалять,
	/// возможно где-то виртуальный редактируется
	GlobalStateMachineUnitType = smutNormal;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveVirtualDependencies(TStrings *ADepsList) {
	if (ADepsList) {
		ADepsList->Clear();
		for (int n = 0; n < TheTree->Shapes->Count; n++) {
			TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>(TheTree->Shapes->Items[n]);
			if (AVirtualShape && !AVirtualShape->ExcludeFromSave && !AVirtualShape->FileName.IsEmpty()) {
				ADepsList->Add(AVirtualShape->FileName);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TeeModified(bool YesNo /* = true */ , int ACat /* = 0x0 */ ,
	const System::UnicodeString ADescription /* = L"" */ ) {
	/* inherited */
	TTreeEditorEx::TeeModified(YesNo, ACat, ADescription);

	if (YesNo && (ACat == 1) && StateMachineEditorUnit) {
		if (StateMachineEditorUnit->StateMachineProject) {
			// выносим через флаг, так как при этой процедуре может произойти удаление выделенного элемента
			/* пометки связанных модулей наследования и Virtual */
			FTreeModifiedForInheritanceFlag = true;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeDblClickShape(Teetree::TTreeNodeShape * Sender,
	Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y) {

	if (TheTree->Designing && (TheTree->Selected->Count() > 0) && !TheTree->Connections->Clicked(X, Y)) {

		// здесь изменятся X, Y
		TheTree->Canvas->Calculate2DPosition(X, Y, TeeTreeZ);

		TheTree->CancelMouse = true; /* to not expand / collapse the node... */

		if (TScxmlBaseShape * AScxmlBaseShape = FirstSelected) {
			DoEditScxmlShapes(AScxmlBaseShape);
		}
		else if (TTextDummyShape * ADummy = TextDummyShape) {

			if (!TTextDummyShape::IsClickSendEnabled()) {

				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
					(ADummy->Connection);
				if (AStateMachineConnection) {
					AStateMachineConnection->ShowEditor();
				}
			}
		}
		else {

			TTreeNodeShape *ASelectedShape = TheTree->Selected->First();

			if (ASelectedShape->ClickedImage(X, Y))
				EditTreeShapePage(this, ASelectedShape, stImage, true);
			else
				EditTreeShape(this, ASelectedShape);

			SetToolBars();
			TeeModified(True, 1, "Edit:" + ASelectedShape->SimpleText);
		}

		// обновим редактор
		if (PropSettingsInspector) {
			PropSettingsInspector->UpdateContent();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeAfterDraw(System::TObject * Sender) {
	TTreeEx * ATreeEx = static_cast<TTreeEx*>(TheTree);
	if (ATreeEx) {
		if (ATreeEx->TotalBounds.Width() > ATreeEx->Page->Width) {
			ATreeEx->Page->Width = ATreeEx->TotalBounds.Width() + 20; // bugfix: правая граница была меньше
		}
		if (ATreeEx->TotalBounds.Height() > ATreeEx->Page->Height) {
			ATreeEx->Page->Height = ATreeEx->TotalBounds.Height() + 20;
		}

		const TRect iBounds2D = ATreeEx->Bounds2D;

		TheTree->Canvas->Pen->Color = clLtGray;
		TheTree->Canvas->Brush->Color = clLtGray;

		if (iBounds2D.left < 0) {
			TheTree->Canvas->RectangleWithZ(TRect(iBounds2D.left, 0, 0, iBounds2D.bottom), TeeTreeZ);
		}
		if (iBounds2D.top < 0) {
			TheTree->Canvas->RectangleWithZ(TRect(iBounds2D.left, iBounds2D.top, iBounds2D.right, 0), TeeTreeZ);
		}
		if (iBounds2D.right > ATreeEx->Page->Width) {
			TheTree->Canvas->RectangleWithZ(TRect(ATreeEx->Page->Width, 0, iBounds2D.right, ATreeEx->Page->Height),
				TeeTreeZ);
		}
		if (iBounds2D.bottom > ATreeEx->Page->Height) {
			TheTree->Canvas->RectangleWithZ(TRect(0, ATreeEx->Page->Height, iBounds2D.right, iBounds2D.bottom),
				TeeTreeZ);
		}

		/* при маленьких зумах невозможно ничего прочитать, поэтому делает подсказки */
		const bool bTextIsUnreadable = ATreeEx->View3DOptions->Zoom < SettingsData->ZoomHintsStartLevel;
		if (SettingsData->ZoomHintsEnabled && bTextIsUnreadable) {

			// набор уже отрисованных подсказок
			std::deque<TRect>ARects;

			// подготовка канвы
			ATreeEx->Canvas->Font->Color = clBlack;
			ATreeEx->Canvas->Font->Style = TFontStyles();
			ATreeEx->Canvas->BackMode = cbmOpaque;
			ATreeEx->Canvas->BackColor = clInfoBk;
			ATreeEx->Canvas->TextAlign = 0;

			for (int i = 0; i < ATreeEx->Shapes->Count; i++) {

				TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
					(ATreeEx->Shapes->Items[i]);
				if (AVisualScxmlBaseShape) {

					const TRect R = AVisualScxmlBaseShape->Bounds();

					// находится ли в пределах видимой области
					if (PointInRect(iBounds2D, R.Left, R.Top)) {
						ATreeEx->Canvas->Font->Size = (double)AVisualScxmlBaseShape->Font->Size * (double)
							SettingsData->ZoomHintsFontSizeEnlargePercents / (double)ATreeEx->View3DOptions->Zoom;

						const int iTextWidth = ATreeEx->Canvas->TextWidth(AVisualScxmlBaseShape->SimpleText);
						const int iTextHeight = ATreeEx->Canvas->TextHeight(AVisualScxmlBaseShape->SimpleText);

						const TRect ACaptionRect(R.Left - 3, R.Top - 3, R.Left + iTextWidth + 6,
							R.Top + iTextHeight + 6);

						bool bOverlapped = false;
						for (std::deque<TRect>::iterator it = ARects.begin(); it != ARects.end(); ++it) {
							TRect AIntersect;
							bOverlapped = IntersectRect(AIntersect, *it, ACaptionRect);
							if (bOverlapped) {
								break;
							}
						}

						if (!bOverlapped) {
							ATreeEx->Canvas->TextOut3D(R.Left, R.Top, TeeTreeZ, AVisualScxmlBaseShape->SimpleText);

							ARects.push_back(ACaptionRect);
						}
					}
				}
			}
		}

		// красный прямоугольник вокруг выделенного соединения
		// лучше индентифицирует выделение
		TTreeConnection *AConnection = TheTree->Connections->Selected;

		// показываем выделение также, если нажали на Text
		bool bDummyConnectionSelected = false;
		if (!AConnection && TextDummyShape) {
			bDummyConnectionSelected = true;
			AConnection = TextDummyShape->Connection;
		}

		if (AConnection && SettingsData->ConnectionAdditionalHighlight) {

			if (SettingsData->ConnectionFromToShapesHighlight) {

				const TRect AConnectionRect = AConnection->GetBounds();

				const int i_SELECT_SHAPES_THRESHOLD = 75;
				// проверим размеры соединения, если очень короткое, то не выделяем, иначе нагромождение
				if ((AConnectionRect.Width() > i_SELECT_SHAPES_THRESHOLD || AConnectionRect.Height()
						> i_SELECT_SHAPES_THRESHOLD) && //
					// само-соединения не показываем
					AConnection->ToShape != AConnection->FromShape) {
					if (AConnection->FromShape) {
						const TRect AShapeRect(AConnection->FromShape->X0, AConnection->FromShape->Y0,
							AConnection->FromShape->X1, AConnection->FromShape->Y1);
						const TRect ARect = Editorutils::IncrementRectCopy(AShapeRect, 15);

						TheTree->Canvas->Pen->Color = TColor(RGB(0, 190, 190));
						TheTree->Canvas->Pen->Width = 1;
						TheTree->Canvas->Pen->Style = psDot;
						TheTree->Canvas->Brush->Style = bsClear;
						TheTree->Canvas->RectangleWithZ(ARect, TeeTreeZ);

						// заливка сплошным, если мелки зум, иначе нечитабельно
						TheTree->Canvas->Brush->Style = bTextIsUnreadable ? bsSolid : bsClear;
						// корректируем размер шрифта, чтобы читался на мелком зуме
						TheTree->Canvas->Font->Size = (double)AConnection->FromShape->Font->Size * (double)
							SettingsData->ZoomHintsFontSizeEnlargePercents / (double)
							TheTree->View3DOptions->Zoom;
						TheTree->Canvas->Font->Style = TFontStyles() << fsBold;
						TheTree->Canvas->Font->Color = clTeal;
						TheTree->Canvas->TextOut3D(ARect.Left + 2, ARect.Top + 2, TeeTreeZ, L"From");
					}

					if (AConnection->ToShape && AConnection->ToShape != AConnection->FromShape) {
						const TRect AShapeRect(AConnection->ToShape->X0, AConnection->ToShape->Y0,
							AConnection->ToShape->X1, AConnection->ToShape->Y1);
						const TRect ARect = Editorutils::IncrementRectCopy(AShapeRect, 15);

						TheTree->Canvas->Brush->Style = bsSolid;
						TheTree->Canvas->Pen->Color = TColor(RGB(255, 70, 70));
						TheTree->Canvas->Pen->Width = 1;
						TheTree->Canvas->Pen->Style = psDot;
						TheTree->Canvas->Brush->Style = bsClear;
						TheTree->Canvas->RectangleWithZ(ARect, TeeTreeZ);

						// заливка сплошным, если мелки зум, иначе нечитабельно
						TheTree->Canvas->Brush->Style = bTextIsUnreadable ? bsSolid : bsClear;
						// корректируем размер шрифта, чтобы читался на мелком зуме
						TheTree->Canvas->Font->Size = (double)AConnection->ToShape->Font->Size * (double)
							SettingsData->ZoomHintsFontSizeEnlargePercents / (double)TheTree->View3DOptions->Zoom;
						TheTree->Canvas->Font->Style = TFontStyles() << fsBold;
						TheTree->Canvas->Font->Color = clMaroon;
						TheTree->Canvas->TextOut3D(ARect.Left + 2, ARect.Top + 2, TeeTreeZ, L"To");
					}
				}

			}

			TheTree->Canvas->Pen->Style = psSolid;
			TheTree->Canvas->Pen->Color = bDummyConnectionSelected ? TColor(RGB(255, 128, 128)) : clRed;
			TheTree->Canvas->Pen->Width = 1;
			const TRect ARect = Editorutils::IncrementRectCopy(AConnection->GetBounds(), 15);
			TheTree->Canvas->Brush->Style = bsClear;
			TheTree->Canvas->RectangleWithZ(ARect, TeeTreeZ);
		}
	}

	/* inherited */
	TTreeEditorEx::TheTreeAfterDraw(Sender);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeDblClickConnection(Teetree::TTreeConnection* Sender,
	Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y) {
	if (TheTree) {
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(Sender);
		if (AConnection) {
			AConnection->ShowEditor();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::TheTreeSelectingConnectionOnMouseUp(Teetree::TTreeConnection * Sender,
	bool&CanSelect) {
	CanSelect = Sender != NULL && Sender->ClassType() == __classid(TStateMachineConnection);
}

// ---------------------------------------------------------------------------
Teetree::TTreeNodeShape* __fastcall TStateMachineEditor::AddNewShape(PTreeTagShape AClass, const Types::TRect & R) {
	const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, AClass->AClass);

	TPoint APt = TheTree->GetCursorPos();
	TTreeNodeShape *tmpClicked = TheTree->ClickedShape(APt.x, APt.y);

	if (tmpClicked && tmpClicked->InheritsFrom(__classid(TTextDummyShape))) {
		delete tmpClicked;
		FillNodeTree();
	}

	Teetree::TTreeNodeShape* AShape = TTreeEditorEx::AddNewShape(AClass, R);

	if (AShape) {
		const bool bNewNameType = AShape->Name.Pos("__");
		if (!bNewNameType) {
			AShape->Name = AUniqueName.second;
		}

		TVisualScxmlBaseShape *ABaseShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape);
		if (ABaseShape && ABaseShape->MustSimpleTextBeUnique()) {
			// возможно для копирования проверка
			if (!bNewNameType) {
				ABaseShape->SetStateId(Unique::CreateUniqueSimpleText(ABaseShape, __classid(TVisualScxmlBaseShape)));
			}
			// вызовем сразу, чтобы при отрисовке не устанавливались значения
			ABaseShape->SetDefaultSize();
		}
	}

	return AShape;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::AddChild1Click(System::TObject * Sender) {
	//
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::DoPaste(bool UseCursorPos) {
	TTreeEditorEx::DoPaste(UseCursorPos);

	PropSettingsInspector->UpdateContent();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::DoAddFillNodePicture(TTreeNodeShape *AToShape, TCustomTreeElement * AFromElement) {
	if (TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AFromElement)) {

		std::map<TMetaClass*, int>::iterator it = g_MapImageIndexes.find(AVisualShape->ClassType());
		if (it == g_MapImageIndexes.end()) {

			const UnicodeString sName = TScxmlBaseShape::TypeToXMLNodeName(AVisualShape->StateChildType);

			std::map<UnicodeString, int>::iterator itImg = FShapeImagesMap.find(sName);
			if (itImg != FShapeImagesMap.end()) {

				std::auto_ptr<TIcon>AIconPtr(new TIcon());
				ImageList1->GetIcon(itImg->second, AIconPtr.get());
				std::auto_ptr<TPicture>APicturePtr(new TPicture());
				APicturePtr->Icon = AIconPtr.get();

				AToShape->ImageIndex = TreeImagePool->Add(APicturePtr.get());
				g_MapImageIndexes.insert(std::make_pair(AVisualShape->ClassType(), AToShape->ImageIndex));
			}
			else {
				// не должно такого быть
				AToShape->ImageIndex = tiHardDisk;
			}
		}
		else {
			AToShape->ImageIndex = TTreeNodeImageIndex(it->second);
		}

		AToShape->Font->Style = AToShape->Font->Style << fsBold;
	}
	else if (TStateMachineConnection * AConnection = dynamic_cast<TStateMachineConnection*>(AFromElement)) {
		AToShape->ImageIndex = TTreeNodeImageIndex
			(AConnection->IsSelfConnection ? g_TransitionSelfIndex : g_TransitionSimpleIndex);
		AToShape->Font->Color = clNavy;
	}
	else {
		TTreeEditorEx::DoAddFillNodePicture(AToShape, AFromElement);
	}
}

// ---------------------------------------------------------------------------
TTreeNodeShape * __fastcall TStateMachineEditor::DoProcessAddFillNode(TTreeNodeShape *AToNode,
	TCustomTreeElement * AFromElement) {
	TTextDummyShape * ATextDummyShape = dynamic_cast<TTextDummyShape*>(AFromElement);
	if (!ATextDummyShape) {

		TTreeNodeShape *tmpNode = TTreeEditorEx::DoProcessAddFillNode(AToNode, AFromElement);

		if (tmpNode && AFromElement) {
			if (TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AFromElement)) {

				for (int t = 0; t < AVisualShape->Connections->Count; t++) {

					TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
						(AVisualShape->Connections->Items[t]);
					if (AStateMachineConnection) {

						TTreeEditorEx::DoProcessAddFillNode(tmpNode, AStateMachineConnection);

					}

				}
			}
		}

		return tmpNode;
	}

	return NULL;
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TStateMachineEditor::NodeTreeText(Teetree::TCustomTreeElement* AElement) {

	TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(AElement);
	if (AStateMachineConnection) {
		if (ShowNames1->Checked) {
			return AStateMachineConnection->Name;
		}
		else {
			if (AStateMachineConnection->SimpleText.IsEmpty())
				return L"transition";
			else
				return L"transition {" + AStateMachineConnection->SimpleText + "}";
		}
	}

	return TTreeEditorEx::NodeTreeText(AElement);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnSearchShapeTimer(TObject * Sender) {
	// только через Shape, так как у соединения нет свойства Visible!!!
	TTreeNodeShape *AElement = reinterpret_cast<TTreeNodeShape*>(FSearchTimer->Tag);
	if (AElement) {
		// проверим на валидность
		if (TheTree->Shapes->IndexOf(AElement) != -1) {
			if (m_iFlash > 0) {
				AElement->Visible = m_iFlash % 2;
				m_iFlash--;
			}
			else {
				FSearchTimer->Enabled = false;
				AElement->Visible = true;
				FSearchTimer->Tag = 0;
			}
		}
		else {
			FSearchTimer->Enabled = false;
			FSearchTimer->Tag = 0;
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditor::FindAndSelect(const UnicodeString & sWhat, const TEditorSearchTypes ATypes,
	bool bFromBeginning, const bool bSelectAndStop) {
	if (bFromBeginning) {
		m_i_found = 0;
		m_i_connect_found = 0;
	}

	if (FormSearch && IsWindowVisible(FormSearch->Handle)) {
		FormSearch->HTMLSearchAddInfo->Caption = L"";
	}

	UnicodeString sFindText = (ATypes.Contains(estCaseSensitive) || ATypes.Contains(estUseRegexp))
		? sWhat : sWhat.UpperCase();
	if (ATypes.Contains(estTrim)) {
		sFindText = sFindText.Trim();
	}

	/* нет смысла искать пустой текст */
	if (sFindText.IsEmpty()) {
		return false;
	}

	bool bFind = false;

	// только через Shape, так как у соединения нет свойства Visible!!!
	TTreeNodeShape *AFlashShape = NULL;

	if (ATypes.Contains(estStateText) || ATypes.Contains(estAllProperties)) {
		for (int i = m_i_found; i < TheTree->Shapes->Count; i++) {

			TTreeNodeShape *AShape = TheTree->Shape[i];

			if (FormSearch && IsWindowVisible(FormSearch->Handle)) {
				FormSearch->LabelInfo->Caption = UnicodeString().sprintf(L"[%d] of [%d] Shape[%s]", i,
					TheTree->Shapes->Count, AShape->SimpleText.c_str());
				Application->ProcessMessages();
			}
			else
				break;

			// ! Для глубокой отладки !
			// WLOG_DEBUG(L"[%d] of [%d] Shape[%s]", i, TheTree->Shapes->Count, AShape->SimpleText.c_str());

			if (ATypes.Contains(estSelectedStatesOnly)) {
				if (!Editorutils::IsNodeOrParentSelected(AShape)) {
					continue;
				}
			}

			if (ATypes.Contains(estStateText)) {
				TMatchFind AMatchOut;
				bFind = StrMatchWhat(AShape->Text->Text, sFindText, ATypes, AMatchOut);
				if (bFind) {
					if (FormSearch && IsWindowVisible(FormSearch->Handle)) {
						FormSearch->HTMLSearchAddInfo->Caption = UnicodeString().sprintf
							(L"Match in <B>State</B> text [%s]", AMatchOut.sHTMLOut.c_str());
					}
					else
						break;

					if (!bSelectAndStop) {
						FrameSearchResults->AddMatchNode(this->UnitFileName, AShape->Name, AShape->SimpleText, "Text",
							AMatchOut);

						bFind = false;
					}
				}
			}

			if (!bFind && ATypes.Contains(estAllProperties)) {
				std::auto_ptr<TXMLDocument>AOutListPtr(new TXMLDocument(Application));
				AOutListPtr->Active = true;
				_di_IXMLNode ARoot = AOutListPtr->AddChild("root");
				MUtils::PersistentToXML(AShape, AOutListPtr.get(), true);
				for (int k = 0; k < ARoot->ChildNodes->Count; k++) {
					_di_IXMLNode ANode = ARoot->ChildNodes->Nodes[k];
					if (!ANode->HasAttribute("Name"))
						throw Exception("Node require attr 'Name'. Full context:" + ANode->XML);

					const UnicodeString sName = ARoot->ChildNodes->Nodes[k]->GetAttribute("Name");
					const UnicodeString sValue = ARoot->ChildNodes->Nodes[k]->Text;

					TMatchFind AMatchOut;
					bFind = StrMatchWhat(sValue, sFindText, ATypes, AMatchOut);
					if (bFind) {
						if (FormSearch && IsWindowVisible(FormSearch->Handle)) {
							FormSearch->HTMLSearchAddInfo->Caption = UnicodeString().sprintf
								(L"Match in prop[<B>%s</B>] val[%s]", sName.c_str(), AMatchOut.sHTMLOut.c_str());
						}
						else
							break;

						if (bSelectAndStop) {
							break; // выход из поиска по свойствам
						}
						else {
							FrameSearchResults->AddMatchNode(this->UnitFileName, AShape->Name, AShape->SimpleText,
								sName, AMatchOut);
						}
					}
					Application->ProcessMessages();
				}
			}

			if (bFind) {
				m_i_found = i + 1;

				if (bSelectAndStop) {
					this->SelectShape(AShape);

					AFlashShape = AShape;
				}
				else {
					bFind = false;
				}
				break;
			}
		}
	}

	if (!bFind && (ATypes.Contains(estTransitionText) || ATypes.Contains(estAllProperties))) {
		// если дошли сюда, значит все фигуры просмотрены, там искать больше нечего,
		// поэтому блокируем поиск фигур
		m_i_found = TheTree->Shapes->Count;

		UnicodeString sText = "";
		for (int i = m_i_connect_found; i < TheTree->Connections->Count; i++) {
			TTreeConnection *AConnection = TheTree->Connections->Items[i];
			if (AConnection->InheritsFrom(__classid(TStateMachineConnection))) {
				if (FormSearch && IsWindowVisible(FormSearch->Handle)) {
					FormSearch->LabelInfo->Caption = UnicodeString().sprintf(L"[%d] of [%d] Transition[%s] ", i,
						TheTree->Shapes->Count, AConnection->SimpleText.c_str());
					Application->ProcessMessages();
				}
				else
					break;
				// ! Для очень глубокой отладки ! WLOG_DEBUG(L"[%d] of [%d] Connection[%s] ", i, TheTree->Shapes->Count, AConnection->SimpleText.c_str());

				if (ATypes.Contains(estSelectedStatesOnly)) {
					if (!Editorutils::IsConnectionOrParentsSelected(AConnection)) {
						continue;
					}
				}

				if (ATypes.Contains(estTransitionText)) {
					TMatchFind AMatchOut;
					bFind = StrMatchWhat(AConnection->Text->Text, sFindText, ATypes, AMatchOut);
					if (bFind) {
						if (FormSearch && IsWindowVisible(FormSearch->Handle)) {
							FormSearch->HTMLSearchAddInfo->Caption = UnicodeString().sprintf
								(L"Match in <B>Transition</B> text [%s]", AMatchOut.sHTMLOut.c_str());
						}
						else
							break;
						if (!bSelectAndStop) {
							FrameSearchResults->AddMatchNode(this->UnitFileName, AConnection->Name,
								AConnection->SimpleText, L"Text", AMatchOut);

							bFind = false;
						}
					}
				}
				if (!bFind && ATypes.Contains(estAllProperties)) {

					std::auto_ptr<TXMLDocument>AOutListPtr(new TXMLDocument(Application));
					AOutListPtr->Active = true;
					_di_IXMLNode ARoot = AOutListPtr->AddChild("root");
					MUtils::PersistentToXML(AConnection, AOutListPtr.get(), true);

					for (int k = 0; k < ARoot->ChildNodes->Count; k++) {
						_di_IXMLNode ANode = ARoot->ChildNodes->Nodes[k];
						if (!ANode->HasAttribute("Name"))
							throw Exception("Node require attr 'Name'. Full context:" + ANode->XML);

						const UnicodeString sName = ARoot->ChildNodes->Nodes[k]->GetAttribute("Name");
						const UnicodeString sValue = ARoot->ChildNodes->Nodes[k]->Text;

						TMatchFind AMatchOut;
						bFind = StrMatchWhat(sValue, sFindText, ATypes, AMatchOut);
						if (bFind) {
							if (FormSearch && IsWindowVisible(FormSearch->Handle)) {
								FormSearch->HTMLSearchAddInfo->Caption = UnicodeString().sprintf
									(L"Match in prop[<B>%s</B>] val[%s]", sName.c_str(), AMatchOut.sHTMLOut.c_str());
							}
							else
								break;

							if (bSelectAndStop) {
								break; // выход из поиска по свойствам
							}
							else {
								FrameSearchResults->AddMatchNode(this->UnitFileName, AConnection->Name,
									AConnection->SimpleText, sName, AMatchOut);

								bFind = false;
							}
						}
						Application->ProcessMessages();
					}
				}

				if (bFind) {

					m_i_connect_found = i + 1;

					if (bSelectAndStop) {
						this->SelectConnection(AConnection);

						AFlashShape = AConnection->FromShape;
					}

					break;
				}
			}
		}
	}

	if (AFlashShape) {

		Editorutils::ExpandAllParents(AFlashShape);

		// выключим предыдущий таймер и восстановим видимость фигуры
		FSearchTimer->Enabled = false;
		TTreeNodeShape *AOldShape = reinterpret_cast<TTreeNodeShape*>(FSearchTimer->Tag);
		if (FSearchTimer->Tag != 0 && TheTree->Shapes->IndexOf(AOldShape) != -1) {
			AOldShape->Visible = true;
		}

		FSearchTimer->Tag = reinterpret_cast<int>(AFlashShape);
		FSearchTimer->OnTimer = OnSearchShapeTimer;
		m_iFlash = MAX_FLASH_TIMES;
		FSearchTimer->Enabled = true;
	}

	if (!bFind) {
		m_i_found = 0;
		m_i_connect_found = 0;
	}
	return bFind;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::AddShapesFromPresetCondition() {
	try {
		if (!this->TheTree->Selected->Count())
			throw Exception("You should select [TVisualScxmlBaseShape] at first!");

		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>(this->TheTree->Selected->First());
		if (!AShape)
			throw Exception("Can not convert Selected Shape to <TVisualScxmlBaseShape>!");

		std::auto_ptr<TPresetCondition>APresetConditionPtr(new TPresetCondition(this));
		if (APresetConditionPtr->ShowModal() == mrOk) {

			const UnicodeString sBackCondition = UnicodeString().sprintf(L"In(\"%s_\"..GetGlobalDef(\"%s\",0))==false",
				APresetConditionPtr->EditStateName->Text.c_str(), APresetConditionPtr->EditScxmlEvent->Text.c_str());

			const int iCasesCount = APresetConditionPtr->EditCasesCount->Text.ToIntDef(0);

			if (iCasesCount < 2)
				throw Exception(UnicodeString().sprintf(L"Cases count [%d] is less than [2]!!!", iCasesCount));

			TStateShape *ARootShape = dynamic_cast<TStateShape*>(AShape->AddStateChild(sctState, true));
			AssertCreatedShape(ARootShape);
			ARootShape->Id = APresetConditionPtr->EditStateName->Text;
			ARootShape->Y0 = ARootShape->Y0 + 50;

			TStateShape *ACheckShape = dynamic_cast<TStateShape*>(ARootShape->AddStateChild(sctState, true));
			AssertCreatedShape(ACheckShape);
			ACheckShape->Id = APresetConditionPtr->EditStateName->Text + "_Check";
			ACheckShape->X1 = ACheckShape->X0 + TheTree->Canvas->TextWidth(ACheckShape->SimpleText) + 20;

			TStateShape *AHolderShape = dynamic_cast<TStateShape*>(ARootShape->AddStateChild(sctState, true));
			AssertCreatedShape(AHolderShape);
			AHolderShape->Id = APresetConditionPtr->EditStateName->Text + "_Holder";
			AHolderShape->X0 = ACheckShape->X1 + TheTree->Canvas->TextWidth(sBackCondition) + 30;
			AHolderShape->Y0 = ACheckShape->Y0;

			for (int i = 0; i < iCasesCount; i++) {
				TStateShape *AChildShape = dynamic_cast<TStateShape*>(AHolderShape->AddStateChild(sctState, true));
				AssertCreatedShape(AChildShape);
				AChildShape->Id = APresetConditionPtr->EditStateName->Text + "_" + UnicodeString(i);
				AChildShape->X1 = AChildShape->X0 + TheTree->Canvas->TextWidth(AChildShape->SimpleText) + 20;
				if (AHolderShape->X1 < AChildShape->X1 + 10) {
					AHolderShape->X1 = AChildShape->X1 + 10;
				}
				AChildShape->Height = AChildShape->Height / 2;

				TStateMachineConnection * ANewConnection = new TStateMachineConnection(this);
				ANewConnection->Tree = TheTree;
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, ANewConnection->ClassType());
				ANewConnection->Name = AUniqueName.second;
				ANewConnection->FromShape = ACheckShape;
				ANewConnection->ToShape = AChildShape;
				ANewConnection->Condition = UnicodeString().sprintf(L"GetGlobalDef(\"%s\",0)==%d",
					APresetConditionPtr->EditScxmlEvent->Text.c_str(), i);
				ANewConnection->Style = csSides;
				if (ANewConnection->Points->Count() == 2) {
					/* FROM */
					ANewConnection->Points->ChangeXStyle(0, cpsFromPercent);
					ANewConnection->Points->Item[0].XValue = 100.0f * (float)(iCasesCount - i) / (float)iCasesCount;

					ANewConnection->Points->ChangeYStyle(0, cpsFromPercent);
					ANewConnection->Points->Item[0].YValue = 100;

					/* TO */
					ANewConnection->Points->ChangeYStyle(1, cpsToPercent);
					ANewConnection->Points->Item[1].YValue = 50;

					ANewConnection->Points->ChangeXStyle(1, cpsToPercent);
					ANewConnection->Points->Item[1].XValue = 0;
				}

				ANewConnection->Text->VertAlign = TVertTextAlign::vtaBottom;
				ANewConnection->Text->HorizAlign = THorizTextAlign::htaCenter;
			}

			ACheckShape->BringToFront();

			ARootShape->X1 = AHolderShape->X1 + 10;
			ARootShape->Y1 = AHolderShape->Y1 + 10;

			TStateMachineConnection * ABackConnection = new TStateMachineConnection(this);
			ABackConnection->Tree = TheTree;

			{
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, ABackConnection->ClassType());
				ABackConnection->Name = AUniqueName.second;
			}

			ABackConnection->FromShape = AHolderShape;
			ABackConnection->ToShape = ACheckShape;
			ABackConnection->Condition = sBackCondition;
			ABackConnection->Style = csSides;
			if (ABackConnection->Points->Count() == 2) {
				/* FROM */
				ABackConnection->Points->ChangeXStyle(0, cpsFromPercent);
				ABackConnection->Points->Item[0].XValue = 0;

				ABackConnection->Points->ChangeYStyle(0, cpsFromRel);
				ABackConnection->Points->Item[0].YValue = 25;

				/* TO */
				ABackConnection->Points->ChangeYStyle(1, cpsToPercent);
				ABackConnection->Points->Item[1].YValue = 50;

				ABackConnection->Points->ChangeXStyle(1, cpsToPercent);
				ABackConnection->Points->Item[1].XValue = 100;
			}

			TStateMachineConnection * ASelfConnection = new TStateMachineConnection(this);
			ASelfConnection->Tree = TheTree;

			{
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, ASelfConnection->ClassType());
				ASelfConnection->Name = AUniqueName.second;
			}

			ASelfConnection->FromShape = ARootShape;
			ASelfConnection->ToShape = ARootShape;
			ASelfConnection->Event = APresetConditionPtr->EditScxmlEvent->Text;
			ASelfConnection->Style = csLine;
			ASelfConnection->MakeSelfConnection();

			ACheckShape->IsInitial = true;

			if (APresetConditionPtr->CheckAutoLayout->Checked) {
				CheckVisualScxmlStateBounds(TheTree);
			}

			FillNodeTree();
			TeeModified(True, 1, "Preset Condition:" + APresetConditionPtr->EditStateName->Text);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"PRESETS> %s", E->Message.c_str());
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::AddShapesFromPresetOnOff() {
	try {
		if (!this->TheTree->Selected->Count())
			throw Exception("You should select [TVisualScxmlBaseShape] at first!");

		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>(this->FirstSelected);
		if (!AShape)
			throw Exception("Can not convert Selected Shape to <TVisualScxmlBaseShape>!");

		std::auto_ptr<TPresetOnOff>APresetPtr(new TPresetOnOff(this));
		if (APresetPtr->ShowModal() == mrOk) {

			const UnicodeString sBackCondition = UnicodeString().sprintf(L"GetGlobal(\"%s\")~=%s",
				APresetPtr->EditScxmlEvent->Text.c_str(), APresetPtr->EditEqualValue->Text.c_str());
			const UnicodeString sToCondition = UnicodeString().sprintf(L"GetGlobal(\"%s\")==%s",
				APresetPtr->EditScxmlEvent->Text.c_str(), APresetPtr->EditEqualValue->Text.c_str());

			TStateShape *ARootShape = dynamic_cast<TStateShape*>(AShape->AddStateChild(sctState, true));
			AssertCreatedShape(ARootShape);
			ARootShape->Id = APresetPtr->EditStateName->Text;
			ARootShape->Y0 = ARootShape->Y0 + 50;

			TStateShape *AOffShape = dynamic_cast<TStateShape*>(ARootShape->AddStateChild(sctState, true));
			AssertCreatedShape(AOffShape);
			AOffShape->Id = APresetPtr->EditStateName->Text + "_Off";
			AOffShape->X1 = AOffShape->X0 + TheTree->Canvas->TextWidth(AOffShape->SimpleText) + 20;

			TStateShape *AOnShape = dynamic_cast<TStateShape*>(ARootShape->AddStateChild(sctState, true));
			AssertCreatedShape(AOnShape);
			AOnShape->Id = APresetPtr->EditStateName->Text + "_On";
			AOnShape->X0 = AOffShape->X1 + TheTree->Canvas->TextWidth(sBackCondition) + 30;
			AOnShape->X1 = AOnShape->X0 + TheTree->Canvas->TextWidth(AOnShape->SimpleText) + 20;
			AOnShape->Y0 = AOffShape->Y0;
			AOnShape->Y1 = AOnShape->Y0 + 30;
			AOnShape->AddStateChild(sctOnentry, false);
			AOnShape->AddStateChild(sctOnexit, false);
			AOnShape->Expanded = true;

			AOffShape->Y0 = AOnShape->Y1 + 60;
			AOffShape->Y1 = AOffShape->Y0 + 30;

			ARootShape->X1 = AOnShape->X1 + 10;
			ARootShape->Y1 = AOnShape->Y1 + 10;

			TStateMachineConnection * AToConnection = new TStateMachineConnection(this);
			AToConnection->Tree = TheTree;

			{
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, AToConnection->ClassType());
				AToConnection->Name = AUniqueName.second;
			}

			AToConnection->FromShape = AOffShape;
			AToConnection->ToShape = AOnShape;
			AToConnection->Condition = sToCondition;
			AToConnection->Style = csSides;
			if (AToConnection->Points->Count() == 2) {
				/* FROM */
				AToConnection->Points->ChangeXStyle(0, cpsFromPercent);
				AToConnection->Points->Item[0].XValue = 50;

				AToConnection->Points->ChangeYStyle(0, cpsFromPercent);
				AToConnection->Points->Item[0].YValue = 0;

				/* TO */
				AToConnection->Points->ChangeYStyle(1, cpsToPercent);
				AToConnection->Points->Item[1].YValue = 50;

				AToConnection->Points->ChangeXStyle(1, cpsToPercent);
				AToConnection->Points->Item[1].XValue = 0;

				AToConnection->Text->VertAlign = TVertTextAlign::vtaBottom;
				AToConnection->Text->HorizAlign = THorizTextAlign::htaCenter;
			}

			TStateMachineConnection * ABackConnection = new TStateMachineConnection(this);
			ABackConnection->Tree = TheTree;

			{
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, ABackConnection->ClassType());
				ABackConnection->Name = AUniqueName.second;
			}

			ABackConnection->FromShape = AOnShape;
			ABackConnection->ToShape = AOffShape;
			ABackConnection->Condition = sBackCondition;
			ABackConnection->Style = csSides;
			if (ABackConnection->Points->Count() == 2) {
				/* FROM */
				ABackConnection->Points->ChangeXStyle(0, cpsFromPercent);
				ABackConnection->Points->Item[0].XValue = 50;

				ABackConnection->Points->ChangeYStyle(0, cpsFromPercent);
				ABackConnection->Points->Item[0].YValue = 100;

				/* TO */
				ABackConnection->Points->ChangeYStyle(1, cpsToPercent);
				ABackConnection->Points->Item[1].YValue = 50;

				ABackConnection->Points->ChangeXStyle(1, cpsToPercent);
				ABackConnection->Points->Item[1].XValue = 100;

				ABackConnection->Text->VertAlign = TVertTextAlign::vtaBottom;
				ABackConnection->Text->HorizAlign = THorizTextAlign::htaCenter;
			}

			TStateMachineConnection * ASelfConnection = new TStateMachineConnection(this);
			ASelfConnection->Tree = TheTree; {
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(this, ASelfConnection->ClassType());
				ASelfConnection->Name = AUniqueName.second;
			}ASelfConnection->FromShape = ARootShape;
			ASelfConnection->ToShape = ARootShape;
			ASelfConnection->Event = APresetPtr->EditScxmlEvent->Text;
			ASelfConnection->Style = csLine;
			ASelfConnection->MakeSelfConnection();

			AOffShape->IsInitial = true;

			if (APresetPtr->CheckAutoLayout->Checked) {
				CheckVisualScxmlStateBounds(TheTree);
			}

			FillNodeTree();
			TeeModified(True, 1, "Preset OnOff:" + APresetPtr->EditStateName->Text);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"PRESETS> %s", E->Message.c_str());
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::EnqueeScxmlShapeForInheritanceCheck(TScxmlBaseShape *AShape) {
	FInheritanceCheckShapes.insert(AShape);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::EnqueeScxmlConnectionForInheritanceCheck(TStateMachineConnection *AConnection) {
	FInheritanceCheckConnections.insert(AConnection);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::HandleExceptionDuplicateStateID(EScxmlDuplicateStateIDException * E) {
	if (this->ScxmlName == E->ScxmlName) {
		TTreeNodeShape *AShapeSearch = Editorutils::FindShapeByStateID(TheTree, E->StateID);
		if (AShapeSearch) {
			SelectShape(AShapeSearch, SCROLL_CHART_IN_VIEW);
			const TRect ARect = Editorutils::IncrementRectCopy(AShapeSearch->Bounds(), 15);
			SelectTemporaryRectangle(ARect);
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineEditor::GetDataModel() {
	TScxmlShape *AScxmlShape = this->RootScxml;
	if (AScxmlShape) {
		return AScxmlShape->Datamodel;
	}

	return L"";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineEditor::GetSyntaxScheme() {
	TScxmlShape *AScxmlShape = this->RootScxml;
	if (AScxmlShape) {
		return AScxmlShape->SyntaxScheme;
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToSVG(TStringList *AOutputList, TTreeNodeShape *ARootShape /* =NULL */ ) {
	if (AOutputList) {

		TRect ACaptureRect(0, 0, 0, 0);

		if (!ARootShape) {
			ARootShape = this->RootScxml;
		}

		if (ARootShape) {
			ACaptureRect = ARootShape->Bounds();
			Editorutils::IncrementRect(ACaptureRect, 3);
		}
		else {
			ACaptureRect = TheTree->TotalBounds;
			ACaptureRect.right += 5;
			ACaptureRect.bottom += 5;
		}

		const bool bWasDesigning = TheTree->Designing;
		const bool bWasGridVisible = TheTree->Grid->Visible;

		try {
			TheTree->Designing = false;
			TheTree->Grid->Visible = false;

			SaveTreeToSvg(TheTree, UnitFileName, ACaptureRect, AOutputList);
		}
		__finally {
			TheTree->Designing = bWasDesigning;
			TheTree->Grid->Visible = bWasGridVisible;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToSVG(const UnicodeString &sFileName,
	TTreeNodeShape *ARootShape /* = NULL */ ) {
	std::auto_ptr<TStringList>AOutputPtr(new TStringList());

	SaveToSVG(AOutputPtr.get(), ARootShape);

	AOutputPtr->SaveToFile(sFileName, TEncoding::UTF8);
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditor::IsNodeTreeSelectionDragReady() {
	return /* Shapes */ (FirstSelected && //
		(AreAllSelectedOfInheritedClass(__classid(TChildScxmlBaseShape)) || //
			AreAllSelectedOfInheritedClass(__classid(TVisualScxmlBaseShape))) && AreAllSelectedBrothers()) ||
	/* Connections */
		TheTree->Connections->Selected;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::NodeTreeMouseDown(System::TObject* Sender, Controls::TMouseButton Button,
	Classes::TShiftState Shift, int X, int Y) {

	/* Drag 'n Drop */
	if (!NodeTree->CancelMouse && Button == mbLeft) {

		if (IsNodeTreeSelectionDragReady()) {
			NodeTree->BeginDrag(false, 5);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::NodeTreeDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State,
	bool &Accept) {
	Accept = false;

	if (IsNodeTreeSelectionDragReady()) {
		TCustomTreeElement *ATarget = NodeTreeShapeToTreeElement(NodeTree->ClickedShape(X, Y));
		if (ATarget) {
			if (TheTree->Connections->Selected) {
				if (TStateMachineConnection * ASourceConnection = dynamic_cast<TStateMachineConnection*>
					(TheTree->Connections->Selected)) {
					TStateMachineConnection * ATargetConnection = dynamic_cast<TStateMachineConnection*>(ATarget);
					if (ATargetConnection && ASourceConnection != ATargetConnection) {
						Accept = ATargetConnection->FromShape == TheTree->Connections->Selected->FromShape;
					}
				}
			}
			else {
				if (TScxmlBaseShape * AFirstSourceShape = FirstSelected) {
					const bool bMatchBaseClass = (AFirstSourceShape->InheritsFrom(__classid(TChildScxmlBaseShape)) && //
						ATarget->InheritsFrom(__classid(TChildScxmlBaseShape))) //
					|| //
					(AFirstSourceShape->InheritsFrom(__classid(TVisualScxmlBaseShape)) && //
						ATarget->InheritsFrom(__classid(TVisualScxmlBaseShape)));

					if (bMatchBaseClass) {
						TScxmlBaseShape * ATargetShape = dynamic_cast<TScxmlBaseShape*>(ATarget);
						if (ATargetShape && ATargetShape != AFirstSourceShape) {
							Accept = ATargetShape->Parent && //
							(ATargetShape->Parent == AFirstSourceShape->Parent);
						}
					}
				}

			}

		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::NodeTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y) {
	try {
		bool Accept = false;

		if (IsNodeTreeSelectionDragReady()) {
			TCustomTreeElement *ATarget = NodeTreeShapeToTreeElement(NodeTree->ClickedShape(X, Y));
			if (ATarget) {

				bool bDragCompleted = false;
				UnicodeString sDragCaption = L"";

				/* CONNECTIONS */
				if (TheTree->Connections->Selected) {
					if (TStateMachineConnection * ASourceConnection = //
						dynamic_cast<TStateMachineConnection*>(TheTree->Connections->Selected)) {

						TStateMachineConnection * ATargetConnection = dynamic_cast<TStateMachineConnection*>(ATarget);
						if (ATargetConnection && ATargetConnection != ASourceConnection) {

							Accept = ATargetConnection->FromShape == ASourceConnection->FromShape;

							if (Accept && ATargetConnection->FromShape) {
								const Editorutils::TreeUpdateLock ATreeLock(TheTree);

								// меняем глобально в дереве
								{
									const int iIndex1 = TheTree->Connections->IndexOf(ASourceConnection);
									const int iIndex2 = TheTree->Connections->IndexOf(ATargetConnection);

									TConnectionListAccess *AListAccess = reinterpret_cast<TConnectionListAccess*>
										(TheTree->Connections);
									AListAccess->Exchange(iIndex1, iIndex2);
								}

								// теперь в списке у состояния
								{
									const int iIndex1 = ATargetConnection->FromShape->Connections->IndexOf
										(ASourceConnection);
									const int iIndex2 = ATargetConnection->FromShape->Connections->IndexOf
										(ATargetConnection);

									TConnectionListAccess *ATreeList = reinterpret_cast<TConnectionListAccess*>
										(ATargetConnection->FromShape->Connections);
									ATreeList->Exchange(iIndex1, iIndex2);
								}

								sDragCaption = ATargetConnection->Caption;
								bDragCompleted = true;
							}
						}
					}
				}

				/* SHAPES */
				else {

					TScxmlBaseShape * ATargetShape = dynamic_cast<TScxmlBaseShape*>(ATarget);
					TScxmlBaseShape * AFirstSourceShape = FirstSelected;
					if (ATargetShape && AFirstSourceShape && (ATargetShape != AFirstSourceShape)
						&& ATargetShape->Parent && //
						(ATargetShape->Parent == AFirstSourceShape->Parent)) {

						/* Visual Shapes */
						if ((AFirstSourceShape->InheritsFrom(__classid(TVisualScxmlBaseShape)) && //
								ATargetShape->InheritsFrom(__classid(TVisualScxmlBaseShape))) ||

							(AFirstSourceShape->InheritsFrom(__classid(TChildScxmlBaseShape)) && //
								ATargetShape->InheritsFrom(__classid(TChildScxmlBaseShape)))

							) {

							const Editorutils::TreeUpdateLock ATreeLock(TheTree);

							const int iIndex1 = TheTree->Shapes->IndexOf(AFirstSourceShape);
							const int iIndex2 = TheTree->Shapes->IndexOf(ATargetShape);

							// необходимо поменять индексы в дереве, чтобы при записи соответствовало BrotherIndex
							if (iIndex1 != -1 && iIndex2 != -1) {
								TNodeListAccess *AListAccess = reinterpret_cast<TNodeListAccess*>(TheTree->Shapes);
								AListAccess->Exchange(iIndex1, iIndex2);
							}

							ATargetShape->BrotherIndex = AFirstSourceShape->BrotherIndex;
#if 0 // для глубокой отладки
							WLOG_DEBUG(L"Source:[%s] Index:[%d] -> Target:[%s] Index:[%d]", //
								AFirstSourceShape->SimpleText.c_str(), AFirstSourceShape->BrotherIndex, //
								ATargetShape->SimpleText.c_str(), ATargetShape->BrotherIndex);
#endif
							sDragCaption = ATargetShape->SimpleText;
							bDragCompleted = true;

						}

					}

				}

				/* успешно завершено перетаскивание */
				if (bDragCompleted) {
					TheTree->Refresh();
					if (PropSettingsInspector) {
						PropSettingsInspector->UpdateContent();
					}
					FillNodeTree();
					TeeModified(True, 1, "Drag 'n Drop to :" + sDragCaption);
				}
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"Structure View Drag'n'Drop> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ResetActiveDebugShape() {
	TVisualScxmlBaseShape *AWasShape = FActiveDebugShape;
	FActiveDebugShape = NULL;
	if (AWasShape && this->TheTree->Items->IndexOf(AWasShape) != -1) {
		AWasShape->UpdateAppearance();
	}

	TStateMachineConnection *AWasConnection = FActiveDebugConnection;
	FActiveDebugConnection = NULL;
	if (AWasConnection && this->TheTree->Items->IndexOf(AWasConnection) != -1) {
		AWasConnection->UpdateAppearance();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::EditAppearanceClick(System::TObject* Sender) {
	EditTreeConnection(this, TheTree->Connections->Selected, false, true, true);
	this->SetToolBars();
	this->TeeModified(true, 1, "Edit tree connections");
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::Edit1Click(System::TObject* Sender) {
	if (TheTree->Selected->Count() > 0) {

		InternalEditTreeShapePage(this, TheTree->Selected->First(), stFormat, true, this->OnEditNodeTreeShow);

		SetToolBars();
		TeeModified(true, 1, "Shape appearance");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnEditNodeTreeShow(System::TObject* Sender) {
	TNodeTreeEditor * ANodeTreeEditor = dynamic_cast<TNodeTreeEditor*>(Sender);
	if (ANodeTreeEditor) {
		TTreeNodeShape *ASelected = GetSafeTreeListFirst(TheTree->Selected->Shapes);
		if (ASelected) {
			/* Setup Theme Colors */
			if (TScxmlShape * AScxmlShape = dynamic_cast<TScxmlShape*>(ASelected)) {
				ANodeTreeEditor->Shape4->Brush->Color = SettingsData->ThemeSettings->ScxmlNormalHeadColor;
			}
			else if (TParallelShape * AParallelShape = dynamic_cast<TParallelShape*>(ASelected)) {
				ANodeTreeEditor->Shape4->Brush->Color = SettingsData->ThemeSettings->ParallelNormalColor;
			}
			else if (TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>(ASelected)) {
				ANodeTreeEditor->Shape4->Brush->Color = SettingsData->ThemeSettings->VirtualNormalColor;
			}
			else if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelected)) {
				ANodeTreeEditor->Shape4->Brush->Color = SettingsData->ThemeSettings->StateNormalColor;
				ANodeTreeEditor->Shape5->Brush->Color = SettingsData->ThemeSettings->StateNormalBorderColor;
			}
			else if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(ASelected)) {
				ANodeTreeEditor->Shape4->Brush->Color = SettingsData->ThemeSettings->ChildNormalColor;
				ANodeTreeEditor->Shape5->Brush->Color = SettingsData->ThemeSettings->ChildNormalBorderColor;
			}

			/* All Visual Shapes */
			if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelected)) {
				ANodeTreeEditor->ShapeClusterColor->Brush->Color = SettingsData->ThemeSettings->StateClusterColor;
				ANodeTreeEditor->ShapeClusterColor->Visible = true;
				ANodeTreeEditor->LabelClusterColor->Visible = true;

				/* Override Color Handlers */
				ANodeTreeEditor->Shape4->OnMouseUp = this->ThemeStateColorMouseUp;
				ANodeTreeEditor->Shape5->OnMouseUp = this->ThemeBorderNormalColorMouseUp;
				ANodeTreeEditor->ShapeClusterColor->OnMouseUp = this->ThemeStateClusterColorMouseUp;
				ANodeTreeEditor->Button1->OnClick = this->ThemeBorderClick;
			}
			else {
				ANodeTreeEditor->ShapeClusterColor->Visible = false;
				ANodeTreeEditor->LabelClusterColor->Visible = false;
			}

			/* All Child Shapes */
			if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(ASelected)) {
				/* Override Color Handlers */
				ANodeTreeEditor->Shape4->OnMouseUp = this->ThemeStateColorMouseUp;
				ANodeTreeEditor->Shape5->OnMouseUp = this->ThemeBorderNormalColorMouseUp;
				ANodeTreeEditor->Button1->OnClick = this->ThemeBorderClick;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ThemeStateColorMouseUp(System::TObject* Sender, Controls::TMouseButton Button,
	Classes::TShiftState Shift, int X, int Y) {

	TShape * AShape = dynamic_cast<TShape*>(Sender);
	if (AShape) {
		TTreeNodeShape *ASelected = GetSafeTreeListFirst(TheTree->Selected->Shapes);
		if (ASelected) {

			TColor AOldColor = AShape->Brush->Color;
			TColor ANewColor = EditColor(this, AOldColor);
			if (AOldColor != ANewColor) {
				AShape->Brush->Color = ANewColor;

				if (TScxmlShape * AScxmlShape = dynamic_cast<TScxmlShape*>(ASelected)) {
					SettingsData->ThemeSettings->ScxmlNormalHeadColor = ANewColor;
				}
				else if (TParallelShape * AParallelShape = dynamic_cast<TParallelShape*>(ASelected)) {
					SettingsData->ThemeSettings->ParallelNormalColor = ANewColor;
				}
				else if (TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>(ASelected)) {
					SettingsData->ThemeSettings->VirtualNormalColor = ANewColor;
				}
				else if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelected)
					) {
					SettingsData->ThemeSettings->StateNormalColor = ANewColor;
				}
				else if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(ASelected)) {
					SettingsData->ThemeSettings->ChildNormalColor = ANewColor;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ThemeStateClusterColorMouseUp(System::TObject* Sender,
	Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y) {

	TShape * AShape = dynamic_cast<TShape*>(Sender);
	if (AShape) {
		TTreeNodeShape *ASelected = GetSafeTreeListFirst(TheTree->Selected->Shapes);
		if (ASelected) {

			TColor AOldColor = AShape->Brush->Color;
			TColor ANewColor = EditColor(this, AOldColor);
			if (AOldColor != ANewColor) {
				AShape->Brush->Color = ANewColor;

				if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelected)) {
					SettingsData->ThemeSettings->StateClusterColor = ANewColor;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ThemeBorderNormalColorMouseUp(System::TObject* Sender,
	Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y) {

	TShape * AShape = dynamic_cast<TShape*>(Sender);
	if (AShape) {
		TTreeNodeShape *ASelected = GetSafeTreeListFirst(TheTree->Selected->Shapes);
		if (ASelected) {

			TColor AOldColor = AShape->Brush->Color;
			TColor ANewColor = EditColor(this, AOldColor);
			if (AOldColor != ANewColor) {
				AShape->Brush->Color = ANewColor;

				if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelected)) {
					SettingsData->ThemeSettings->StateNormalBorderColor = ANewColor;
				}
				else if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(ASelected)) {
					SettingsData->ThemeSettings->ChildNormalBorderColor = ANewColor;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ThemeBorderClick(System::TObject* Sender) {
	TTreeNodeShape *ASelected = GetSafeTreeListFirst(TheTree->Selected->Shapes);
	if (ASelected) {

		std::auto_ptr<TChartPen>AChartPenPtr(new TChartPen((TNotifyEvent)NULL));
		AChartPenPtr->Assign(ASelected->Border);

		if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelected)) {
			AChartPenPtr->Color = SettingsData->ThemeSettings->StateNormalBorderColor;
			AChartPenPtr->Style = SettingsData->ThemeSettings->StateNormalBorderStyle;
		}
		else if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(ASelected)) {
			AChartPenPtr->Color = SettingsData->ThemeSettings->ChildNormalBorderColor;
			AChartPenPtr->Style = SettingsData->ThemeSettings->ChildNormalBorderStyle;
		}

		if (EditChartPen(this, AChartPenPtr.get())) {
			if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ASelected)) {
				SettingsData->ThemeSettings->StateNormalBorderColor = AChartPenPtr->Color;
			}
			else if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(ASelected)) {
				SettingsData->ThemeSettings->ChildNormalBorderColor = AChartPenPtr->Color;
			}
			else {
				for (int i = 1; i < TheTree->Selected->Count(); i++) {
					TheTree->Selected->Items[i]->Border->Assign(AChartPenPtr.get());
				}
			}

			TNodeTreeEditor * ANodeTreeEditor = dynamic_cast<TNodeTreeEditor*>
				(dynamic_cast<TComponent*>(Sender)->Owner);
			if (ANodeTreeEditor) {
				ANodeTreeEditor->Shape5->Brush->Color = AChartPenPtr->Color;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::ExecuteInterprocessCommand(INTERPROCESS_COPYDATA *ACopyData) {
	if (SameText(ACopyData->wchCommand, L"SaveToSVG")) {
		this->SaveToSVG(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveRawScxmlToFile")) {
		this->SaveRawScxmlToFile(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveRawScxmlToHPP")) {
		this->SaveRawScxmlToHPP(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveScxmlToPas")) {
		this->SaveScxmlToPas(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveToDot")) {
		this->SaveToDot(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveToDotPlusPng")) {
		this->SaveToDotPlusPng(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveToBMP")) {
		this->SaveToBMP(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveToPNG")) {
		this->SaveToPNG(ACopyData->wchArg1);
	}

	else if (SameText(ACopyData->wchCommand, L"SaveToQtCreatorScxml")) {
		this->SaveToQtCreatorScxml(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveToVSCodeScxml")) {
		this->SaveToVSCodeScxml(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveToScxmlGui")) {
		this->SaveToScxmlGui(ACopyData->wchArg1);
	}
	else if (SameText(ACopyData->wchCommand, L"SaveToSimpleScxml")) {
		this->SaveToSimpleScxml(ACopyData->wchArg1);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::OnMsgCopyData(TWMCopyData &msg) {
	if (msg.CopyDataStruct && msg.CopyDataStruct->cbData == sizeof(INTERPROCESS_COPYDATA)) {
		INTERPROCESS_COPYDATA *ACopyData = reinterpret_cast<INTERPROCESS_COPYDATA*>(msg.CopyDataStruct->lpData);
		if (ACopyData) {
			WLOG_INFO(L"EDITOR> Copy data command:[%s] arg1:[%s] arg2:[%s] arg3:[%s]", //
				ACopyData->wchCommand, ACopyData->wchArg1, ACopyData->wchArg2, ACopyData->wchArg3);

			try {
				this->ExecuteInterprocessCommand(ACopyData);
			}
			catch(Exception * E) {
				WLOG_ERROR(L"EDITOR_COMMAND> %s", E->Message.c_str());
			}
		}
	}
	else {
		WLOG_ERROR(L"Invalid WM_COPYDATA");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToBMP(const UnicodeString &sFileName) {
	TScxmlShape *AScxmlRootShape = this->RootScxml;
	if (AScxmlRootShape) {
		const int i_OFFSET = 3;

		const int iOldHoriz = TheTree->View3DOptions->HorizOffset;
		const int iOldVert = TheTree->View3DOptions->VertOffset;

		const bool bWasFocusDisabled = TheTreeEx->DisableFocus;
		TheTreeEx->DisableFocus = true;

		try {
			TheTreeEx->View3DOptions->HorizOffset = 0;
			TheTreeEx->View3DOptions->VertOffset = 0;
			const TRect ARectSource(0, 0, AScxmlRootShape->X1 + i_OFFSET, AScxmlRootShape->Y1 + i_OFFSET);

			std::auto_ptr<Graphics::TBitmap>ABmpSourcePtr(TheTreeEx->TeeCreateBitmap(clWhite, ARectSource));

			ABmpSourcePtr->SaveToFile(sFileName);
		}
		__finally {
			TheTreeEx->DisableFocus = bWasFocusDisabled;
			TheTreeEx->View3DOptions->HorizOffset = iOldHoriz;
			TheTreeEx->View3DOptions->VertOffset = iOldVert;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToPNG(const UnicodeString &sFileName) {
	TScxmlShape *AScxmlRootShape = this->RootScxml;
	if (AScxmlRootShape) {
		const int i_OFFSET = 3;

		const int iOldHoriz = TheTree->View3DOptions->HorizOffset;
		const int iOldVert = TheTree->View3DOptions->VertOffset;

		const bool bWasFocusDisabled = TheTreeEx->DisableFocus;
		TheTreeEx->DisableFocus = true;

		try {
			TheTreeEx->View3DOptions->HorizOffset = 0;
			TheTreeEx->View3DOptions->VertOffset = 0;
			const TRect ARectSource(0, 0, AScxmlRootShape->X1 + i_OFFSET, AScxmlRootShape->Y1 + i_OFFSET);

			std::auto_ptr<Graphics::TBitmap>ABmpSourcePtr(TheTreeEx->TeeCreateBitmap(clWhite, ARectSource));

			const UnicodeString sValue = SettingsData->TempRegistry->Values[this->Name + ".SpinPNGCompressionLevel"];

			std::auto_ptr<TPngImage>APngImagePtr(new TPngImage());
			APngImagePtr->Assign(ABmpSourcePtr.get());
			APngImagePtr->CompressionLevel = sValue.ToIntDef(7);
			APngImagePtr->SaveToFile(sFileName);
		}
		__finally {
			TheTreeEx->DisableFocus = bWasFocusDisabled;
			TheTreeEx->View3DOptions->HorizOffset = iOldHoriz;
			TheTreeEx->View3DOptions->VertOffset = iOldVert;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveRawScxmlToFile(const UnicodeString & sFileName) {
	const Statemachine::TIterateSaveTypes ATypes = SettingsData->SkipCommentsInRawScxml == false ?
		Statemachine::MaxPossibleTypes() : (Statemachine::MaxPossibleTypes() >> Statemachine::istComments);

	Statemachine::SaveTreeToScxml(TheTree, sFileName, false, true, ATypes);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToQtCreatorScxml(const UnicodeString & sFileName) {
	const Statemachine::TIterateSaveTypes ATypes = (Statemachine::MaxPossibleTypes() >> Statemachine::istVirtual);
	const TVisualMetaInformationTypes AMetaTypes = TVisualMetaInformationTypes() << vmiQt;

	Statemachine::SaveTreeToScxml(TheTree, sFileName, false, false, ATypes, AMetaTypes);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToVSCodeScxml(const UnicodeString & sFileName) {
	const Statemachine::TIterateSaveTypes ATypes = (Statemachine::MaxPossibleTypes() >> Statemachine::istVirtual);
	const TVisualMetaInformationTypes AMetaTypes = TVisualMetaInformationTypes() << vmiVSCode;

	Statemachine::SaveTreeToScxml(TheTree, sFileName, false, false, ATypes, AMetaTypes);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToScxmlGui(const UnicodeString & sFileName) {
	const Statemachine::TIterateSaveTypes ATypes = (Statemachine::MaxPossibleTypes() >> Statemachine::istVirtual);
	const TVisualMetaInformationTypes AMetaTypes = TVisualMetaInformationTypes() << vmiScxmlGui;

	std::auto_ptr<TUTF8EncodingNoBOM>AUTF8EncodingPtr(new TUTF8EncodingNoBOM());
	std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", AUTF8EncodingPtr.get(), false));
	Statemachine::SaveTreeToScxml(TheTree, AStreamPtr.get(), false /* do not append gui */ ,
		true /* skip encoding inside */ , ATypes, AMetaTypes);
	AStreamPtr->SaveToFile(sFileName);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToSimpleScxml(const UnicodeString & sFileName) {
	const Statemachine::TIterateSaveTypes ATypes = (Statemachine::MaxPossibleTypes() >> Statemachine::istVirtual);
	const TVisualMetaInformationTypes AMetaTypes = TVisualMetaInformationTypes() << vmiSimple;

	Statemachine::SaveTreeToScxml(TheTree, sFileName, false, false, ATypes, AMetaTypes);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveRawScxmlToHPP(const UnicodeString & sFileName) {
	std::auto_ptr<TStringList>AStringList(new TStringList());
	AStringList->Text = Statemachine::GetRawScxml(TheTree, SettingsData->SkipCommentsInRawScxml);

	TStateMachineEditor::ConvertTextToHPP(AStringList.get(), TPath::GetFileNameWithoutExtension(sFileName));

	AStringList->SaveToFile(sFileName);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveScxmlToPas(const UnicodeString & sFileName) {
	std::auto_ptr<TMemoryStream>AMemoryStreamPtr(new TMemoryStream());
	SaveTreeToStream(TheTree, AMemoryStreamPtr.get());

	std::auto_ptr<TMemoryStream>AOutStreamPtr(new TMemoryStream());

	AMemoryStreamPtr->Position = 0;
	ObjectBinaryToText(AMemoryStreamPtr.get(), AOutStreamPtr.get());
	AOutStreamPtr->Position = 0;

	AOutStreamPtr->SaveToFile(sFileName);

	WLOG_INFO(L"Successfully saved <%s>!", sFileName.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToDot(const UnicodeString &sFileName) {
	std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(this));
	ADialogWaitPtr->ShowDialog(UnicodeString().sprintf(L"Exporting to DOT[%s] ...",
			ExtractFileName(sFileName).c_str()));
	Graphviz::ExportToDot(TheTree, sFileName);
	WLOG_INFO(L"Successfully saved <%s>", sFileName.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditor::SaveToDotPlusPng(const UnicodeString &sFileName) {
	std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(this));
	ADialogWaitPtr->ShowDialog(UnicodeString().sprintf(L"Exporting to DOT[%s] ...",
			ExtractFileName(sFileName).c_str()));
	Graphviz::ExportToDot(TheTree, sFileName);
	WLOG_INFO(L"Successfully saved <%s>", sFileName.c_str());

	const UnicodeString sFilePng = ChangeFileExt(sFileName, L".png");
	ADialogWaitPtr->ShowDialog(UnicodeString().sprintf(L"Exporting to PNG[%s] ...", ExtractFileName(sFilePng).c_str()));
	Graphviz::ConvertDotToPng(sFileName, sFilePng);
	WLOG_INFO(L"Successfully converted <%s> to <%s>", sFileName.c_str(), sFilePng.c_str());
}

// ---------------------------------------------------------------------------
// --------------------  TStateMachineDockPanel  -----------------------------
// ---------------------------------------------------------------------------
__fastcall TStateMachineDockPanel::TStateMachineDockPanel(Classes::TComponent * AOwner,
	TStateMachineEditorUnit * AStateMachineEditorUnit) : TLMDDockPanel(AOwner), FExitTime(Now()),
FStateMachineEditor(NULL), FMainPanel(NULL), FStateMachineEditorUnit(AStateMachineEditorUnit) {

	if (!FormScxmlGui)
		throw Exception(L"PROGRAM LOGIC ERROR> Can not get pointer [MainForm]!");

	if (!FStateMachineEditorUnit)
		throw Exception(L"PROGRAM LOGIC ERROR> Can not get pointer [StateMachineEditorUnit]!");

	if (!TransitionXMLEditor)
		throw Exception(L"PROGRAM LOGIC ERROR> TransitionXMLEditor == NULL !!!");

	FIX_PANEL_ENTERED = 0;

	// здесь указываем надпись, чтобы потом не моргало при обновлении
	this->Caption = FStateMachineEditorUnit->FileName;

	OpenedDoc = new TLMDOpenedDocument(this);
	OpenedDoc->OnClose = OpenedDocClose;
	OpenedDoc->OnShow = OpenedDocShow;
	OpenedDoc->OnOpening = OpenedDocOpening;
	OpenedDoc->OnAfterSave = OpenedDocAfterSave;
	OpenedDoc->OnSaveFile = OpenedDocOnSave;

	// создаем панельку, чтобы корректно ToolBar отображался
	FMainPanel = new TPanel(this);
	FMainPanel->ShowCaption = false;
	FMainPanel->Parent = this;
	FMainPanel->Align = alClient;

	FToolBar = new TToolBar(this);
	FToolBar->Parent = FMainPanel;

	FStateMachineEditor = new TStateMachineEditor(this, FStateMachineEditorUnit, true /* AppPropInsp */ );
	FStateMachineEditor->Left = 0;
	FStateMachineEditor->Top = 0;

	FStateMachineEditor->Name = TeeGetUniqueName(this, "TreeEditor");

	FStateMachineEditor->BorderStyle = bsNone;

	FStateMachineEditor->Parent = FMainPanel;
	FStateMachineEditor->Align = alClient;

	FToolBar->Menu = FStateMachineEditor->MainMenu1;

	FStateMachineEditor->OnTreeEditorModified = OnTreeModified;

	this->OnEnter = OnStateMachineDockPanelEnter;
	this->OnExit = OnStateMachineDockPanelExit;

	this->OnMouseActivate = OnMouseActivateEvent;

	FReloadInheritanceFlag = false;
}

// ---------------------------------------------------------------------------
__fastcall TStateMachineDockPanel::~TStateMachineDockPanel(void) {
	OpenedDoc->MarkClosed();
}

// ---------------------------------------------------------------------------
TRect __fastcall TStateMachineDockPanel::GetAdjustedClientRect(void) {
	TRect ARect = this->ClientRect;
	AdjustClientRect(ARect);
	return ARect;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OpenedDocOpening(TObject * Sender, TLMDProjDocument * ADocument) {
	try {
		// делаем именно в этом месте, чтобы исключить промаргивание
		// также необходимо вызвать метод Show до загрузки SCXML, иначе неправильно загружает унаследованные формы
		if (!StateMachineEditor->Visible) {
			StateMachineEditor->Show();
		}

		if (!ADocument->IsNew) {
			FStateMachineEditor->LoadTree(ADocument->FilePath);
			// нормальная загрузка
			if (Statemachine::LastError.IsEmpty()) {
				FStateMachineEditor->TeeModified(false);
			}
			else {
				// возможно какие-то ошибки при загрузке
				FStateMachineEditor->TeeModified(true, 1, Statemachine::LastError);
			}
		}
		else {
			TScxmlShape *AScxmlShape = new TScxmlShape(FStateMachineEditor);
			// делаем небольшой отступ, чтобы красивее смотрелось
			AScxmlShape->X0 = 8;
			AScxmlShape->Y0 = 8;
			AScxmlShape->AutoPosition->Left = false;
			AScxmlShape->AutoPosition->Top = false;
			AScxmlShape->SetDefaultSize();
			const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(FStateMachineEditor,
				AScxmlShape->ClassType());
			AScxmlShape->Name = AUniqueName.second;
			AScxmlShape->ScxmlName = GetUniqueScxmlName(StateMachineEditorUnit);
			AScxmlShape->Tree = FStateMachineEditor->TheTree;
			AScxmlShape->SaveScxmlNameToProject();
			AScxmlShape->Expanded = true;
			FStateMachineEditor->MarkModified(1, "Root created", true);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OpenedDocClose(TObject * Sender) {
	if (FStateMachineEditor->TheTree->Connecting) {
		const UnicodeString Caption = L"WARNING!";
		const UnicodeString Text = L"Press 'ESC' to cancel connecting shapes...";
		MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OK | MB_ICONWARNING);
	}
	else {
		if (CloseQuery()) {
			// убираем обработчик
			FStateMachineEditor->OnTreeEditorModified = NULL;

			FStateMachineEditor->TeeModified(false); // чтобы не спрашивал внутренний CloseQuery редактора

			if (FStateMachineEditor->DesignObjects) {
				FStateMachineEditor->DesignObjects->Clear();
				FStateMachineEditor->TheTree->Clear();
			}

			TCloseAction ACloseAction = caNone;
			FStateMachineEditor->FormClose(FStateMachineEditor, ACloseAction);

			FStateMachineEditorUnit->ResetModified();

			delete this;
			FStateMachineEditorUnit->StateMachineDockPanel = 0;

			FormScxmlGui->UpdateUnitCaption(NULL);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::ActivateZonePage(void) {
	if (this->Zone && this->Zone->Parent) {
		this->Zone->Parent->ActivePage = this->Zone;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OpenedDocShow(TObject * Sender) {
	if (OpenedDoc->Opened) {
		if (!this->PanelVisible) {
			this->PanelVisible = true;
		}

		ActivateZonePage();

		// необходимо, чтобы сделать панель активность,
		// иначе активной может быть какая-то другая панель
		this->Activate(true);
	}
	this->UpdateCaption();

	WLOG_DEBUG(L"OpenedDocShow=%s", OpenedDoc->Node->FilePath.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OpenedDocAfterSave(TObject * Sender, const TLMDString AFilePath) {
	this->UpdateCaption();
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineDockPanel::CloseQuery() {
	if (!TFormSyntaxEditorForm::CloseAssociatedSavedDockPanels(this))
		return false;

	bool res = true;
	if (OpenedDoc->Node->NeedSave) {
		switch(OpenedDoc->Node->ProjMgr->SaveQuery(OpenedDoc->Node)) {
		case sqrYes: {
				res = OpenedDoc->Node->ProjMgr->Save(OpenedDoc->Node, false);

			}break;
		case sqrNo: {
				// if (OpenedDoc->Node->IsNew) {
				// OpenedDoc->Node->Delete();
				// }

			}break;
		case sqrCancel: {
				res = false;

			}break;
		}
	}

	return res;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::UpdateSyntaxView(void) {
	FormScxmlGui->UpdateSyntaxView();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OnMouseActivateEvent(System::TObject* Sender, TMouseButton Button,
	Classes::TShiftState Shift, int X, int Y, int HitTest, TMouseActivate &MouseActivate) {

	FormScxmlGui->FixDockPanelActiveState(this);

	if (!this->FIX_PANEL_ENTERED) {
		WLOG_DEBUG(L"ENTER> Panel[%s] force enter!", this->Caption.c_str());
		this->OnEnter(this);
	}
}

// ---------------------------------------------------------------------------
bool DesignObjectsMatchesTree(TCustomTree *ATree, TLMDDesignObjects *ADesignObjects) {
	if (ADesignObjects && ATree) {
		if (ATree->Connections->Selected) {
			if (ADesignObjects->Count == 1) {
				return ATree->Connections->Selected == ADesignObjects->Item[0];
			}
		}
		else {
			if (ADesignObjects->Count == ATree->Selected->Count()) {
				for (int i = 0; i < ADesignObjects->Count; i++) {
					if (ADesignObjects->Item[i] != ATree->Selected->Items[i]) {
						return false;
					}
				}
				return true;
			}
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OnStateMachineDockPanelEnter(TObject * Sender) {
	// предотвращаем двойное исполнение
	// и также исключаем момент загрузки наследованных, так как он вызывает смену родителя
	if (!FIX_PANEL_ENTERED && !FStateMachineEditor->IsReloadingTree) {
		FIX_PANEL_ENTERED = true;

		// на этот момент мы должны выйти из всех других док. панелей
		FormScxmlGui->FixDockPanelActiveState(this);

		FormScxmlGui->FActiveEditorDockPanel = this;
		WLOG_DEBUG(L"Set main form active dock panel=%s", this->Caption.c_str());

		if (FReloadInheritanceFlag) {
			FReloadInheritanceFlag = false;

			// в этот момент сменит док. панель как родителя и снова назначит
			FStateMachineEditor->ReloadInheritedTree();

			// известить дальше по цепочке наследования
			FStateMachineEditor->MarkModifiedForInherited();
			WLOG_WARNING(L"INHERITANCE> Reloading chart:[%s] because parent:[%s] was modified!",
				StateMachineEditorUnit->FileName.c_str(),
				StateMachineEditorUnit->Inherited.c_str());

			// помечаем, что осталось в панели
			FIX_PANEL_ENTERED = true;
		}

		// если мы переключаемся не на TStateMachineDockPanel,
		// тогда у нас в DesignObjects остаются те же объекты и их не надо снова выделять
		if (!DesignObjectsMatchesTree(FStateMachineEditor->TheTree, FStateMachineEditor->DesignObjects)) {
			TLMDPropPageStateObject *AStateObject = FormScxmlGui->PropSettingsInspector->SaveState();

			TransitionXMLEditor->ResetStateMachineConnection();

			FormScxmlGui->DesignObjects1->Clear();

			for (int i = 0; i < FStateMachineEditor->TheTree->Selected->Count(); i++) {
				FormScxmlGui->DesignObjects1->Add(FStateMachineEditor->TheTree->Selected->Items[i]);
			}

			if (FStateMachineEditor->TheTree->Connections->Selected) {
				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
					(FStateMachineEditor->TheTree->Connections->Selected);
				if (AStateMachineConnection) {

					FormScxmlGui->DesignObjects1->Add(AStateMachineConnection);

					TransitionXMLEditor->SetStateMachineConnection(AStateMachineConnection);
				}
			}

			if (AStateObject) {
				FormScxmlGui->PropSettingsInspector->RestoreState(AStateObject, true);
			}
		}

		// пока только для унаследованных, но возможно и позже добавим
		if (StateMachineEditorUnit->IsInherited) {
			InvalidateTree();
		}

		this->UpdateSyntaxView();

		this->UpdateCaption();

		FormScxmlGui->SetSelectProjectManagerViewFlag();

		WLOG_DEBUG(L"Entered to panel <%s>", this->Caption.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OnStateMachineDockPanelExit(TObject * Sender) {
	FExitTime = Now();

	FIX_PANEL_ENTERED = false;
	WLOG_DEBUG(L"Exited from panel <%s>", this->Caption.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OnTreeModified(TObject * Sender) {
	/* эти две операции длительные, поэтому быстро отпускаем, чтобы не было тормозов на Chart'е */
	if (SettingsData->TranslateEveryChangeToScxmlView) {
		// перезапись XML
		this->UpdateSyntaxView();
	}
	// здесь полностью перерисовывается весь DockSite
	UpdateCaption();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::UpdateCaption(void) {
	FormScxmlGui->UpdateUnitCaption(this);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineDockPanel::GetScxmlName() {
	if (FStateMachineEditor) {
		return FStateMachineEditor->ScxmlName;
	}
	return L"";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineDockPanel::GetDataModel() {
	if (FStateMachineEditor) {
		FStateMachineEditor->DataModel;
	}
	return L"";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineDockPanel::GetInvokeID() {
	if (FStateMachineEditor) {
		TScxmlShape *ARootScxml = FStateMachineEditor->RootScxml;
		if (ARootScxml) {
			return ARootScxml->InvokeID;
		}
	}
	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::CollectShapeBindings(TCustomTree * ATree, ILMDXmlElement * ANodeContainer) {

	for (int i = 0; i < ATree->Connections->Count; i++) {
		TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
			(ATree->Connections->Items[i]);
		if (AStateMachineConnection) {
			if (AStateMachineConnection->ProtocolControlBinding->Active) {
				AStateMachineConnection->ProtocolControlBinding->ToXML(ANodeContainer);
			}
		}
	}

	for (int i = 0; i < ATree->Shapes->Count; i++) {
		TScxmlBaseShape *ABaseShape = dynamic_cast<TScxmlBaseShape*>(ATree->Shapes->Items[i]);
		if (ABaseShape && !ABaseShape->ExcludeFromSave) {
			if (ATree->Shapes->Items[i]->ClassType() == __classid(TSendShape)) {
				TSendShape *AShape = dynamic_cast<TSendShape*>(ATree->Shapes->Items[i]);
				if (AShape->ProtocolControlBinding->Active) {
					AShape->ProtocolControlBinding->ToXML(ANodeContainer);
				}
			}
			else if (ATree->Shapes->Items[i]->ClassType() == __classid(TSetValueShape)) {
				TSetValueShape *AShape = dynamic_cast<TSetValueShape*>(ATree->Shapes->Items[i]);
				if (AShape->ProtocolControlBinding->Active) {
					AShape->ProtocolControlBinding->ToXML(ANodeContainer);
				}
			}
			else if (ATree->Shapes->Items[i]->ClassType() == __classid(TProtocolBindingShape)) {
				TProtocolBindingShape *AShape = dynamic_cast<TProtocolBindingShape*>(ATree->Shapes->Items[i]);
				if (AShape->ProtocolControlBinding->Active) {
					AShape->ProtocolControlBinding->ToXML(ANodeContainer);
				}
			}
			else if (ATree->Shapes->Items[i]->ClassType() == __classid(TProtocolWeakBindingShape)) {
				TProtocolWeakBindingShape *AShape = dynamic_cast<TProtocolWeakBindingShape*>(ATree->Shapes->Items[i]);
				if (AShape->Active) {
					AShape->ToXML(ANodeContainer);
				}
			}
			else if (ATree->Shapes->Items[i]->ClassType() == __classid(TContentShape)) {
				TContentShape *AShape = dynamic_cast<TContentShape*>(ATree->Shapes->Items[i]);
				if (AShape && AShape->XMLText->Type == xttSCXML) {
					std::auto_ptr<TStateMachineEditor>AStateMachineEditorForm(new TStateMachineEditor(Application,
							NULL, false));
					if (AShape->XMLText->IsScxmlGui) {
						if (AShape->XMLText->ScxmlStream->Size) {
							AStateMachineEditorForm->LoadScxmlStream(AShape->XMLText->ScxmlStream, "Scxml editor");
						}
					}
					else {
						if (!AShape->XMLText->Text.IsEmpty()) {
							AStateMachineEditorForm->LoadScxmlData(AShape->XMLText->Text, "Scxml editor");
						}
					}
					// рекурсивно дальше
					CollectShapeBindings(AStateMachineEditorForm->TheTree, ANodeContainer);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::OpenedDocOnSave(System::TObject * Sender,
	const Lmdtypes::TLMDString AFileIdent, Classes::TStream * AFileStream, bool&AHandled) {

	SettingsData->ProfilerReset(__FUNCTION__);

	try {

		StateMachineEditor->TheTreeEx->ApplicationVersion = SettingsData->AppVersion;

		StateMachineEditor->DeleteTemporaryShapes(); // dummy shapes

		if (FormScxmlGui) {
			FormScxmlGui->SwitchToProgramLog();

			/* Сохраняем всё, что в док.панельных редакторах */
			for (int i = 0; i < FormScxmlGui->ComponentCount; i++) {
				TSyntaxDockPanel * ASyntaxDockPanel = dynamic_cast<TSyntaxDockPanel*>(FormScxmlGui->Components[i]);
				if (ASyntaxDockPanel && Customutils::ComponentIsPresentInPath(this,
						ASyntaxDockPanel->SyntaxForm->LinkInstancePropPath)) {
					ASyntaxDockPanel->SyntaxForm->ActionSave->Execute();
				}
			}

			// сохранение в док. панельных редакторах может вызвать изменение в редакторах соединений
			// поэтому записываем также все изменения в этих редакторах
			for (int i = 0; i < FStateMachineEditor->TheTree->Connections->Count; i++) {
				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
					(FStateMachineEditor->TheTree->Connections->Items[i]);
				if (AStateMachineConnection) {
					AStateMachineConnection->SaveEditor();
				}
			}
		}

		TVisualMetaInformationTypes AMetaTypes = TVisualMetaInformationTypes();

		/* ФИЧИ, СВЯЗАННЫЕ В РЕЖИМЕ ПРОЕКТА */
		TStateMachineProject *AStateMachineProject = StateMachineEditorUnit->StateMachineProject;
		if (AStateMachineProject) {

			AMetaTypes = AStateMachineProject->ProjectProperties->ProjectMetaInformationTypes;

			/* Bindings */
			if (SettingsData->AviaExtensionEnabled && !StateMachineEditorUnit->BindingsDisabled) {
				try {

					ILMDXmlElement *AContainerElement = StateMachineEditorUnit->BindingsContainer;
					if (!AContainerElement)
						throw Exception("Container [" + StateMachineEditorUnit->FileName + "] is empty!");

					const UnicodeString sXML = AContainerElement->XML;

					// удаление здесь, так как CollectShapeBindings рекурсивный
					AContainerElement->RemoveAllChilds();

					SettingsData->ProfileOperationReset(__FUNCTION__, "AContainerElement->RemoveAllChilds");

					CollectShapeBindings(FStateMachineEditor->TheTree, AContainerElement);

					SettingsData->ProfileOperationReset(__FUNCTION__, "CollectShapeBindings");

					// значит изменились подписки, пометим главный проект
					if (sXML != AContainerElement->XML) {
						// без детей, иначе сам же юнит дает флаг, что изменен
						if (!AStateMachineProject->IsModified(false)) {
							AStateMachineProject->MarkModified();
						}
					}

				}
				catch(Exception * E) {
					LOG_ERROR(LOG_ERROR_MSG);
				}
			}

			/* VIRTUAL */ {
				std::auto_ptr<TStringList>ADepsListPtr(new TStringList());
				StateMachineEditor->SaveVirtualDependencies(ADepsListPtr.get());

				if (StateMachineEditorUnit->VirtualDependencies->Text.Trim() != ADepsListPtr->Text.Trim()) {
					StateMachineEditorUnit->VirtualDependencies->Text = ADepsListPtr->Text;
					// без детей, иначе сам же юнит дает флаг, что изменен
					if (!AStateMachineProject->IsModified(false)) {
						AStateMachineProject->MarkModified();
					}
					WLOG_DEBUG(L"Virtual dependencies in [%s] were changed", OpenedDoc->Node->NewFilePath.c_str());
				}
			}
		}

		/* Scxml */
		/* НАСЛЕДОВАНИЕ */
		if (StateMachineEditorUnit && StateMachineEditorUnit->IsInherited) {

			TStateMachineEditor *AInheritedEditor = FindStateMachineEditorByFileName(StateMachineEditorUnit->Inherited);

			if (!AInheritedEditor)
				throw Exception("Can not find inherited module [" + StateMachineEditorUnit->Inherited + "]!");

			Statemachine::SaveInheritedToScxml(FStateMachineEditor->TheTree, AInheritedEditor->TheTree, AFileStream,
				StateMachineEditorUnit->Inherited, AMetaTypes);
		}
		/* ОБЫЧНАЯ ЗАПИСЬ */
		else {
			Statemachine::SaveTreeToScxml(FStateMachineEditor->TheTree, AFileStream, true, false,
				Statemachine::TIterateSaveTypes() << Statemachine::istMAXSIZE, AMetaTypes);
		}

		SettingsData->ProfileOperationReset(__FUNCTION__, "SaveTreeToScxml");

		FStateMachineEditor->TeeModified(false);
		AHandled = true;

		WLOG_INFO(L"Successfully saved <%s>!", OpenedDoc->Node->NewFilePath);

		SettingsData->ProfileOperationReset(__FUNCTION__, "TeeModified(false)");
	}
	catch(EStateMachineConnectionException * E) {
		ActivateZonePage();
		FStateMachineEditor->SelectConnection(E->StateMachineConnection, SCROLL_CHART_IN_VIEW);
		FStateMachineEditor->SelectTemporaryRectangle
			(Editorutils::IncrementRectCopy(E->StateMachineConnection->GetBounds(), 15), "Error: " + E->Category);
		WLOG_ERROR(L"SAVE UNIT[%s]> %s", FStateMachineEditorUnit->DisplayName.c_str(), E->Message.c_str());
	}
	catch(EScxmlBaseShapeException * E) {
		ActivateZonePage();
		FStateMachineEditor->SelectShape(E->Shape, SCROLL_CHART_IN_VIEW);
		FStateMachineEditor->SelectTemporaryRectangle(Editorutils::IncrementRectCopy(E->Shape->Bounds(), 15),
			"Error: " + E->Category);
		WLOG_ERROR(L"SAVE UNIT[%s]> %s", FStateMachineEditorUnit->DisplayName.c_str(), E->Message.c_str());
	}
	catch(EScxmlDuplicateStateIDException * E) {
		ActivateZonePage();
		FStateMachineEditor->HandleExceptionDuplicateStateID(E);
		WLOG_ERROR(L"SAVE UNIT[%s]> %s", FStateMachineEditorUnit->DisplayName.c_str(), E->Message.c_str());
	}
	catch(Exception * E) {
		ActivateZonePage();
		WLOG_ERROR(L"SAVE UNIT[%s]> %s", FStateMachineEditorUnit->DisplayName.c_str(), E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::MarkModified(int iCategory, const UnicodeString & sMsg, bool bFillNodeTree) {
	if (FStateMachineEditor) {
		FStateMachineEditor->MarkModified(iCategory, sMsg, bFillNodeTree);
		UpdateCaption();
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineDockPanel::GetRawScxml() {
	return Statemachine::GetRawScxml(FStateMachineEditor->TheTree, SettingsData->SkipCommentsInRawScxml);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineDockPanel::GetRawScxmlForceComments() {
	return Statemachine::GetRawScxml(FStateMachineEditor->TheTree, false /* do not skip comments */ );
}

// ---------------------------------------------------------------------------
std::pair<TStateMachineDockPanel*, TTreeNodeShape *>__fastcall TStateMachineDockPanel::EnterStateNode
	(const UnicodeString & sId, const bool bEntered, const bool bOpenClosedUnits, UnicodeString sAlias) {

	std::pair<TStateMachineDockPanel*, TTreeNodeShape*>AEnteredElements = std::make_pair((TStateMachineDockPanel*)NULL,
		(TTreeNodeShape*)NULL);

	/* VIRTUAL ALIAS */
	const bool bAliasUsed = !sAlias.IsEmpty();
	if (bAliasUsed && StateMachineEditorUnit->IsVirtual) {
		TScxmlShape *AScxmlShape = FStateMachineEditor->RootScxml;
		if (AScxmlShape && !AScxmlShape->InvokeID.IsEmpty()) {
			/* User can override alias with InvokeID */
			sAlias = AScxmlShape->InvokeID;
		}
	}

	for (int i = 0; i < FStateMachineEditor->TheTree->Shapes->Count; i++) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(FStateMachineEditor->TheTree->Shapes->Items[i]);
		if (AShape && !AShape->SkipDebugging && !AShape->ExcludeFromSave) {

			bool b_NAME_MATCH = false;
			if (bAliasUsed) {
				const UnicodeString sAliasID = StringReplace(AShape->SimpleText, SettingsData->VirtualAliasVariable,
					sAlias, TReplaceFlags() << rfReplaceAll);
				b_NAME_MATCH = sAliasID == sId;
				if (b_NAME_MATCH) {
					AShape->SetAliasID(sAliasID);
				}
			}
			else {
				b_NAME_MATCH = AShape->SimpleText == sId;
			}

			/* обычный режим */
			if (b_NAME_MATCH) {
				if (AShape->Entered != bEntered) {
					AShape->Entered = bEntered;
#if 0 // для глубокой отладки
					WLOG_DEBUG(L"Panel:[%s] Shape:[%s] entered=%d", this->Caption.c_str(), AShape->SimpleText.c_str(),
						AShape->Entered ? 1 : 0);
#endif

					TTreeEx * ATreeEx = dynamic_cast<TTreeEx*>(FStateMachineEditor->TheTree);
					if (ATreeEx && bEntered) {

						// при UnInvoke нет никакой информации о том, что мы вышли из состояний,
						// поэтому как выход - это при входе в состояние очистить все его дочерние
						TVisualScxmlBaseShape::SetChildrenEntered(AShape->Children, false, true);

						TRect AIntersect;
						if (SettingsData->TestingScrollChartToViewShape && //
							IsWindowVisible(ATreeEx->Handle) && // когда не видимо, то неправильные размеры
							!IsIconic(FormScxmlGui->Handle) && // когда приложение свернуто оно генерирует неправильные размеры формы
							!IntersectRect(AIntersect, ATreeEx->Bounds2D, AShape->AdjustedRectangle))
						// если фигура не видима в данный момент
						{

							ATreeEx->Refresh();
							TPoint P = ATreeEx->Canvas->Calculate3DPosition(AShape->XCenter(), AShape->YCenter(),
								TeeTreeZ);

							int iTmpX = ATreeEx->ChartXCenter;
							int iTmpY = ATreeEx->ChartYCenter;

							P.x = P.x - iTmpX;
							P.y = P.y - iTmpY;

							ATreeEx->View3DOptions->HorizOffset = ATreeEx->View3DOptions->HorizOffset - P.x;
							ATreeEx->View3DOptions->VertOffset = ATreeEx->View3DOptions->VertOffset - P.y;

						}
					}

					FStateMachineEditor->TheTree->Invalidate();
				}

				AEnteredElements = std::make_pair(this, AShape);

				TVisualScxmlBaseShape *AWasActiveDebugShape = FStateMachineEditor->FActiveDebugShape;

				FStateMachineEditor->FActiveDebugShape = bEntered ? AShape : NULL;

				if (bEntered) {
					FStateMachineEditor->FEnteredDebugShapes.insert(AShape);
				}

				if (AWasActiveDebugShape != FStateMachineEditor->FActiveDebugShape) {

					if (AWasActiveDebugShape && FStateMachineEditor->TheTree->Items->IndexOf(AWasActiveDebugShape)
						!= -1) {
						AWasActiveDebugShape->UpdateAppearance();
					}

					AShape->UpdateAppearance();
				}
			}

			/* особый случай поиск по составным SCXML */
			if (AShape->InheritsFrom(__classid(TVirtualShape))) {
				TVirtualShape *AVirtualShape = dynamic_cast<TVirtualShape*>(AShape);
				if (AVirtualShape) {
					TStateMachineEditorUnit *ASrcUnit = AVirtualShape->SrcUnit;
					if (ASrcUnit) {
						if (!ASrcUnit->Opened) {
							// иногда не нужно открывать редакторы при отладке, поэтому воспользуемся флагом
							if (bOpenClosedUnits) {
								ASrcUnit->Open(true);
							}
						}
						TStateMachineDockPanel *ASrcDockPanel = ASrcUnit->StateMachineDockPanel;
						if (ASrcDockPanel) {
							std::pair<TStateMachineDockPanel*,
							TTreeNodeShape*>AVirtualEnteredElements = ASrcDockPanel->EnterStateNode(sId, bEntered,
								bOpenClosedUnits, //
								StringReplace(AVirtualShape->Alias, SettingsData->VirtualAliasVariable, sAlias,
									TReplaceFlags() << rfReplaceAll));

							if (AVirtualEnteredElements.first && AVirtualEnteredElements.second) {
								return AVirtualEnteredElements;
							}
						}
					}
				}
			}

			// если дошло сюда и совпадало по имени, выходим
			if (b_NAME_MATCH)
				break;
		}
	}
	return AEnteredElements;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::ClearEnteredStates(void) {
	const Editorutils::TreeUpdateLock ATreeLock(FStateMachineEditor->TheTree);

	for (int i = 0; i < FStateMachineEditor->TheTree->Shapes->Count; i++) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(FStateMachineEditor->TheTree->Shapes->Items[i]);
		if (AShape) {
			AShape->Entered = false;
			AShape->SetAliasID(L"");
		}
	}

	FStateMachineEditor->ResetActiveDebugShape();

	FStateMachineEditor->ClearWasEnteredElements();

	if (FStateMachineEditor->RootScxml) {
		FStateMachineEditor->RootScxml->TempInvokeID = L"";
	}

	FTakingTransitionCacheMap.clear();
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::ProcessTakenTransition(TStateMachineConnection *AConnection) {
	TStateMachineConnection *AWasActiveDebugConnection = FStateMachineEditor->FActiveDebugConnection;

	FStateMachineEditor->FActiveDebugConnection = AConnection;

	if (AWasActiveDebugConnection != FStateMachineEditor->FActiveDebugConnection) {

		FStateMachineEditor->FEnteredStateMachineConnections.insert(AConnection);

		if (AWasActiveDebugConnection && FStateMachineEditor->TheTree->Items->IndexOf(AWasActiveDebugConnection) != -1)
		{
			AWasActiveDebugConnection->UpdateAppearance();
		}

		AConnection->UpdateAppearance();
	}
}

// ---------------------------------------------------------------------------
std::pair<TStateMachineDockPanel*, TTreeConnection *>__fastcall TStateMachineDockPanel::TakingTransition
	(const UnicodeString &sFromShape, const int iTransitionID, const bool bOpenClosedUnits, UnicodeString sAlias) {

	const UnicodeString sTransitionDescriptor = sFromShape + L"@" + UnicodeString(iTransitionID);

	std::map<UnicodeString, TStateMachineConnection*>::iterator it = FTakingTransitionCacheMap.find
		(sTransitionDescriptor);
	if (it != FTakingTransitionCacheMap.end()) {
		if (FStateMachineEditor->TheTree->Connections->IndexOf(it->second) != -1) {
			std::pair<TStateMachineDockPanel*, TTreeConnection*>AEnteredElements = std::make_pair(this, it->second);

			this->ProcessTakenTransition(it->second);

			/* дальше можно не выполнять */
			return AEnteredElements;
		}
		else {
			FTakingTransitionCacheMap.erase(it);
		}
	}

	/* VIRTUAL ALIAS */
	const bool bAliasUsed = !sAlias.IsEmpty();
	if (bAliasUsed && StateMachineEditorUnit->IsVirtual) {
		TScxmlShape *AScxmlShape = FStateMachineEditor->RootScxml;
		if (AScxmlShape && !AScxmlShape->InvokeID.IsEmpty()) {
			/* User can override alias with InvokeID */
			sAlias = AScxmlShape->InvokeID;
		}
	}

	for (int i = 0; i < FStateMachineEditor->TheTree->Shapes->Count; i++) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(FStateMachineEditor->TheTree->Shapes->Items[i]);
		if (AShape && !AShape->SkipDebugging && !AShape->ExcludeFromSave) {

			bool b_NAME_MATCH = false;
			if (bAliasUsed) {
				const UnicodeString sAliasID = StringReplace(AShape->SimpleText, SettingsData->VirtualAliasVariable,
					sAlias, TReplaceFlags() << rfReplaceAll);
				b_NAME_MATCH = sAliasID == sFromShape;
			}
			else {
				b_NAME_MATCH = AShape->SimpleText == sFromShape;
			}

			if (b_NAME_MATCH) {
				TStateMachineConnection *ATakenConnection = NULL;

				int iTransitionIterIndex = 0;

				// 1) Smart Transitions
				Editorutils::TConnectionFromToMap AInConnections;
				Editorutils::CollectInConnections(AShape, AInConnections);

				for (Editorutils::TConnectionFromToMap::iterator it = AInConnections.begin();
					it != AInConnections.end(); ++it) {
					TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(it->first);
					/* 'ExcludeFromSave==false' гарантирует VisualFrom и VisualTo */
					if (AConnection && !AConnection->ExcludeFromSave && AConnection->SWITCH != tstNONE) {

						if (iTransitionIterIndex == iTransitionID) {
							ATakenConnection = AConnection;
							break;
						}

						iTransitionIterIndex++;
					}
				}

				// 2) Regular Transitions
				if (!ATakenConnection) {
					for (int k = 0; k < AShape->Connections->Count; k++) {
						TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
							(AShape->Connections->Items[k]);
						/* 'ExcludeFromSave==false' гарантирует VisualFrom и VisualTo */
						if (AConnection && !AConnection->ExcludeFromSave) {

							if (iTransitionIterIndex == iTransitionID) {
								ATakenConnection = AConnection;
								break;
							}

							iTransitionIterIndex++;
						}
					}
				}

				/* Transition Found */
				if (ATakenConnection) {

					FTakingTransitionCacheMap.insert(std::make_pair(sTransitionDescriptor, ATakenConnection));

					std::pair<TStateMachineDockPanel*, TTreeConnection*>AEnteredElements = std::make_pair(this,
						ATakenConnection);

					this->ProcessTakenTransition(ATakenConnection);

					/* дальше можно не выполнять */
					return AEnteredElements;
				}
			}

			/* особый случай поиск по составным SCXML */
			if (AShape->InheritsFrom(__classid(TVirtualShape))) {
				TVirtualShape *AVirtualShape = dynamic_cast<TVirtualShape*>(AShape);
				if (AVirtualShape) {
					TStateMachineEditorUnit *ASrcUnit = AVirtualShape->SrcUnit;
					if (ASrcUnit) {
						if (!ASrcUnit->Opened) {
							// иногда не нужно открывать редакторы при отладке, поэтому воспользуемся флагом
							if (bOpenClosedUnits) {
								ASrcUnit->Open(true);
							}
						}
						TStateMachineDockPanel *ASrcDockPanel = ASrcUnit->StateMachineDockPanel;
						if (ASrcDockPanel) {

							std::pair<TStateMachineDockPanel*,
							TTreeConnection*>AVirtualEnteredElements = ASrcDockPanel->TakingTransition
								(sFromShape, iTransitionID, bOpenClosedUnits, //
								StringReplace(AVirtualShape->Alias, SettingsData->VirtualAliasVariable, sAlias,
									TReplaceFlags() << rfReplaceAll));

							if (AVirtualEnteredElements.first && AVirtualEnteredElements.second) {
								return AVirtualEnteredElements;
							}
						}
					}
				}
			}

			// если дошло сюда и совпадало по имени, выходим
			if (b_NAME_MATCH)
				break;
		}
	}

	return std::make_pair((TStateMachineDockPanel*)NULL, (TTreeConnection*)NULL); ;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::ClearAllBreakpoints(void) {
	const Editorutils::TreeUpdateLock ATreeLock(FStateMachineEditor->TheTree);

	for (int i = 0; i < FStateMachineEditor->TheTree->Shapes->Count; i++) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>
			(FStateMachineEditor->TheTree->Shapes->Items[i]);
		if (AShape) {
			AShape->BreakpointSet = false;
		}
	}

	for (int i = 0; i < FStateMachineEditor->TheTree->Connections->Count; i++) {
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
			(FStateMachineEditor->TheTree->Connections->Items[i]);
		if (AConnection) {
			AConnection->BreakpointSet = false;
		}
	}

	FStateMachineEditor->ResetActiveDebugShape();
}

// ---------------------------------------------------------------------------
TStateMachineEditor *__fastcall TStateMachineDockPanel::GetInheritedEditor(void) {
	if (StateMachineEditorUnit && !StateMachineEditorUnit->Inherited.IsEmpty()) {
		return FindOpenedStateMachineEditorByFileName(StateMachineEditorUnit->Inherited);
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit *__fastcall TStateMachineDockPanel::GetInheritedUnit(void) {
	if (StateMachineEditorUnit && !StateMachineEditorUnit->Inherited.IsEmpty()) {
		return FindStateMachineUnitByFileName(StateMachineEditorUnit->Inherited);
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineDockPanel::MarkReloadInheritance(void) {
	FReloadInheritanceFlag = true;
}

// ---------------------------------------------------------------------------
// ---------------------------- TScxmlSpeedButton ----------------------------
// ---------------------------------------------------------------------------
void __fastcall TScxmlSpeedButton::Paint(void) {
	TSpeedButton::Paint();

	TTreeTagShape *tmpP = reinterpret_cast<TTreeTagShape*>(Tag);
	if (tmpP) {
		if (tmpP->AInstance == NULL) {
			Application->CreateForm(tmpP->AClass, &tmpP->AInstance);
		}

		tmpP->AInstance->Shadow->HorizSize = 2;
		tmpP->AInstance->Shadow->VertSize = 2;
		tmpP->AInstance->Shadow->Color = clDkGray;

		// special trick for TPolygonShape
		if (tmpP->AInstance->InheritsFrom(__classid(TPolygonShape))) {
			TPolygonShape *APolygon = dynamic_cast<TPolygonShape*>(tmpP->AInstance);
			if (APolygon) {
				APolygon->FillSample();
			}
		}

		if (tmpP->UseStyle) {
			tmpP->AInstance->Style = tmpP->Style;
		}

		std::auto_ptr<TTeeCanvas3D>ACanvas3D(new TTeeCanvas3D());
		ACanvas3D->UseBuffer = false;
		ACanvas3D->InitWindow(Canvas, NULL, Color, false, TeeRect(0, 0, Width, Height));

		TRect tmpR = TeeRect(4, 4, 20, 20);
		if (tmpP->AInstance->InheritsFrom(__classid(TTextShape))) {
			ACanvas3D->Font->Style = TFontStyles() << fsBold;
			ACanvas3D->Font->Size = 16;
			ACanvas3D->TextOutA(5, 1, L"A");
		}
		else {
			((TAccessNodeShape*)tmpP->AInstance)->DrawShapeCanvas(ACanvas3D.get(), tmpR);
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TStateMachineEditorUnit *FindStateMachineUnitByFileName(const UnicodeString &sFileName) {
	if (FormScxmlGui) {
		TStateMachineEditorUnit *AUnit = FindUnitByFileName(FormScxmlGui->ProjectManager1->Root, sFileName);
		if (AUnit) {
			return AUnit;
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineEditor *FindStateMachineEditorByFileName(const UnicodeString &sFileName) {
	if (FormScxmlGui) {
		TStateMachineEditorUnit *AUnit = FindUnitByFileName(FormScxmlGui->ProjectManager1->Root, sFileName);
		if (AUnit) {
			if (!AUnit->Opened) {
				AUnit->Open(true);
			}
			if (AUnit->StateMachineDockPanel) {
				return AUnit->StateMachineDockPanel->StateMachineEditor;
			}
		}
	}

	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineEditor *FindOpenedStateMachineEditorByFileName(const UnicodeString &sFileName) {
	if (FormScxmlGui) {
		TStateMachineEditorUnit *AUnit = FindUnitByFileName(FormScxmlGui->ProjectManager1->Root, sFileName);
		if (AUnit) {
			if (AUnit->Opened && AUnit->StateMachineDockPanel) {
				return AUnit->StateMachineDockPanel->StateMachineEditor;
			}
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineDockPanel * __fastcall GlobalActiveEditorDockPanel() {

	return FormScxmlGui->ActiveEditorDockPanel;

}

// ---------------------------------------------------------------------------
TScxmlShape * __fastcall GlobalActiveEditorRootScxml() {
	TStateMachineDockPanel *ADockPanel = FormScxmlGui->ActiveEditorDockPanel;
	if (ADockPanel) {
		return ADockPanel->StateMachineEditor->RootScxml;
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TCustomTree * __fastcall GlobalActiveEditorTree() {
	TStateMachineDockPanel *ADockPanel = FormScxmlGui->ActiveEditorDockPanel;
	if (ADockPanel) {
		return ADockPanel->StateMachineEditor->TheTree;
	}
	return NULL;
}

// ---------------------------------------------------------------------------
boost::tuple<TComponent *, TStateMachineEditor *,
TStateMachineDockPanel *>ParseInstancePropPath(const UnicodeString &sInstancePropPath) {
	TComponent *AInstance = Application;

	TStateMachineEditor *AEditor = NULL;
	TStateMachineDockPanel *ADockPanel = NULL;

	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	AStringListPtr->Delimiter = L'.';
	AStringListPtr->StrictDelimiter = true;
	AStringListPtr->DelimitedText = sInstancePropPath;

	for (int i = 0; i < AStringListPtr->Count; i++) {
		AInstance = AInstance->FindComponent(AStringListPtr->Strings[i]);

		if (!AInstance)
			throw Exception(UnicodeString().sprintf(L"Instance [%s] no more longer valid!",
				AStringListPtr->Strings[i].c_str()));

		// запоминаем последний редактора
		TStateMachineEditor * AStateMachineEditor = dynamic_cast<TStateMachineEditor*>(AInstance);
		if (AStateMachineEditor) {
			AEditor = AStateMachineEditor;
		}

		// запоминаем последнюю док. панель
		TStateMachineDockPanel * AStateMachineDockPanel = dynamic_cast<TStateMachineDockPanel*>(AInstance);
		if (AStateMachineDockPanel) {
			ADockPanel = AStateMachineDockPanel;
		}
	}
	return boost::make_tuple(AInstance, AEditor, ADockPanel);
}
