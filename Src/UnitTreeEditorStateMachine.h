/***********************************************************************************
	BSD 3-Clause License

	Copyright (c) 2018, https://github.com/alexzhornyak, alexander.zhornyak@gmail.com
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
***************************************************************************************/

#ifndef UnitTreeEditorStateMachineH
#define UnitTreeEditorStateMachineH
//---------------------------------------------------------------------------
#include "LMDIdeProjMgr.hpp"
#include "LMDDckSite.hpp"
#include "LMDDsgObjects.hpp"
#include "LMDDsgPropInsp.hpp"
#include "LMDSedDocument.hpp"

#include <map>
#include <set>
#include <boost/tuple/tuple.hpp>

#include "TreeEditor/TreeEditorEx.h"
#include "CommonConsts.h"

//---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ----------------------------- AccessNodeShape -----------------------------
// ---------------------------------------------------------------------------

class TAccessNodeShape : public TTreeNodeShape {
public:
	inline __fastcall virtual TAccessNodeShape(Classes::TComponent* AOwner) : TTreeNodeShape(AOwner) {
	}

	inline __fastcall virtual ~TAccessNodeShape(void) {
	}

	virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R) {
		TTreeNodeShape::DrawShapeCanvas(ACanvas, R);
	}
};

#define TRANSITION_FIND_SOURCE		0
#define TRANSITION_FIND_TARGET		1

#define SCXML_VIEW__DO_NOT_FORCE_UPDATE			false
#define SCXML_VIEW__FORCE_UPDATE				true

// ---------------------------------------------------------------------------
// ----------------------------- TStateMachineEditor -------------------------
// ---------------------------------------------------------------------------
class TFrameSyntaxEditorEx;
class TScxmlBaseShape;
class TVisualScxmlBaseShape;
class TStateMachineConnection;
class TStateMachineDockPanel;
class TStateMachineEditorUnit;
class TStateMachineProject;
class TFormReplaceDlg;
class TScxmlShape;
class TTextDummyShape;
class TTreeEx;
class TFrameTransitionTypes;
class EScxmlDuplicateStateIDException;

typedef enum { fstOpen, fstLoadText, fstLoadStream } TFinalizeScxmlType;
UnicodeString FinalizeScxmlTypeToString(const TFinalizeScxmlType AType) {
	switch (AType) {
		case fstOpen: return "Open";
		case fstLoadText: return "LoadText";
		case fstLoadStream: return "LoadStream";
	}
	return L"";
}

class TStateMachineEditor : public TTreeEditorEx {
__published:
	void __fastcall Open1Click(System::TObject* Sender); // диалог открытия файла

	void __fastcall ButtonNewClick(System::TObject* Sender); // очистка документа
	void __fastcall ButtonSaveClick(System::TObject* Sender);

	void __fastcall Saveas1Click(System::TObject* Sender);
	void __fastcall PopupNodePopup(System::TObject* Sender);

	void __fastcall NodeTreeStopEditing(Teetree::TTreeNodeShape* Sender);
	void __fastcall SpeedLinkClick(System::TObject* Sender);

	void __fastcall PopupConnPopup(System::TObject* Sender);
	void __fastcall PopupPointPopup(System::TObject* Sender);

	void __fastcall AddChild1Click(System::TObject* Sender);

	void __fastcall TabControl1Change(System::TObject* Sender);

    virtual void __fastcall EditAppearanceClick(System::TObject* Sender);

private:
    	/* VARIABLES */
	int m_i_found;
	int m_i_connect_found;

	int m_iFlash;
	TTimer *FSearchTimer;

	bool FIsReloadingTree; // в данный момент перезагружается дерево

	TStateMachineEditorUnit *FStateMachineEditorUnit;

	bool FForbidInternalAdd; // флаг для запрета создавать соединения штатными средствами (так как пропадают множественные соединения)

	TFrameSyntaxEditorEx *FSyntaxEditorEx;
	TLMDEditDocument *FEditDocRaw;

    TFrameTransitionTypes *FFrameTransitionTypes;

    std::auto_ptr<TLMDPropPageStateObject> FPropInspStateObjectPtr;
	TLMDDesignObjects *FDesignObjects;

	TLMDDesignObjects *__fastcall GetDesignObjects(void); // работать только через этот !!!
	void __fastcall ClearDesignObjects(void); // очистит выделенные элементы

	TLMDPropertyInspector *FPropSettingsInspector;
	TSplitter *FPropSplitter;

	TLMDPropertyInspector *__fastcall GetPropSettingsInspector(void); // работать только через этот !!!
	void __fastcall PropSettingsInspectorChange(TObject *Sender);

	TFormReplaceDlg *FFormReplaceDlg;

	/* Работа в OnApplicationIdle*/
	std::set<TCustomTreeElement*> FSaveSelectionElements;
	bool FTreeSelectionChanged;
	std::auto_ptr<TApplicationEvents> FAppEventsPtr;

	TMenuItem *FMenuShapeEdit;
	TMenuItem *FMenuPasteConnection;
	TMenuItem *FMenuAddNonVisualChild;
	TMenuItem *FMenuIsInitial;
	TMenuItem *FMenuMakeSelfConnection;
	TMenuItem *FMenuConvertTo;
	TMenuItem *FMenuTextAlign;

	TMenuItem *FMenuConvertFromRaw;

	TMenuItem *FMenuMoveBrotherUp;
	TMenuItem *FMenuMoveBrotherDown;

	TMenuItem *FMenuReplaceText;

	TMenuItem *FMenuImportBindingsAndFolders;

	TMenuItem *FMenuSelectItemWithChildren;
	TMenuItem *FMenuSelectChildren;
	TMenuItem *FMenuSelectSiblings;

	TMenuItem *FMenuExportAsImage;
	TMenuItem *FMenuCollectOutputs;
	TMenuItem *FMenuArrangeLayout;
	TMenuItem *FMenuPresets;
	TMenuItem *FMenuShapeTextAutoSize;
	TMenuItem *FMenuSetBreakpoint;
	TMenuItem *FMenuSkipDebugging;
	TMenuItem *FMenuArrangeSelfConnections;
	TMenuItem *FMenuResetChildShapePos;
	TMenuItem *FMenuCopyPropertyToClipboard;
	TMenuItem *FMenuCopyTransition;
	TMenuItem *FMenuFilterSelectionByProperty;
	TMenuItem *FMenuSwapVisualBrothers;
	TMenuItem *FMenuChangeXMLTextType;
	TMenuItem *FMenuInheritance;
	TMenuItem *FMenuSetInheritance;
	TMenuItem *FMenuResetInheritance;
	TMenuItem *FMenuRevertToInheritedProps;
	TMenuItem *FMenuRevertRecursive;
	TMenuItem *FMenuRevertInheritedGeometry;
	TMenuItem *FMenuRevertInOutConnections;
	TMenuItem *FMenuInheritanceDumpDifference;
	TMenuItem *FMenuInheritanceResolveFirstMismatch;
	TMenuItem *FMenuInheritanceResolveAllMismatches;
	TMenuItem *FMenuInheritanceResolveClearAll;

	TMenuItem *FMenuRevertInInheritedUnits;

	TMenuItem *FMenuEditTransition;
	TMenuItem *FMenuTransitionTextMove;

	TMenuItem *FMenuExpandAll;
	TMenuItem *FMenuExpandLevel;
	TMenuItem *FMenuCollapseAll;
	TMenuItem *FMenuCollapseLevel;

	TMenuItem *FMenuTransitionTextAlign;
	TMenuItem *FMenuTransitionSwap;
	TMenuItem *FMenuTransitionMovePoints;
	TMenuItem *FMenuSelfConnectionAlign;
	TMenuItem *FMenuConnectionOverTop;
	TMenuItem *FMenuConnectionOverLeft;
	TMenuItem *FMenuConnectionOverRight;
	TMenuItem *FMenuConnectionOverBottom;
	TMenuItem *FMenuTransitionInheritance;
	TMenuItem *FMenuTransitionSetInheritance;
	TMenuItem *FMenuTransitionResetInheritance;
	TMenuItem *FMenuTransitionRevertToInheritedProps;
	TMenuItem *FMenuTransitionSwitchSimple;
	TMenuItem *FMenuTransitionSwitchInvertCondition;
	TMenuItem *FMenuTransitionInheritanceDumpDifference;
	TMenuItem *FMenuTransitionInheritanceResolveFirstMismatch;
	TMenuItem *FMenuTransitionInheritanceResolveAllMismatches;
	TMenuItem *FMenuTransitionInheritanceResolveClearAll;
	TMenuItem *FMenuTransitionRevertInInheritedUnits;
	TMenuItem *FMenuTransitionConvertTo;
	TMenuItem *FMenuTransitionEnableTrigger;
	TMenuItem *FMenuTransitionSetBreakpoint;

	/* PopupPoint menus */
	TMenuItem *FMenuPointArrangeMirror;
	TMenuItem *FMenuPointArrangeSingle;
	TMenuItem *FMenuPointPin;
	TMenuItem *FMenuPointAlignOppositeHorizontally;
	TMenuItem *FMenuPointAlignOppositeVertically;

	void __fastcall OnMenuAddSubChildClick(TObject *Sender);
	void __fastcall OnMenuIsInitialClick(TObject *Sender);
	void __fastcall OnMenuEditTransitionClick(TObject *Sender);
	void __fastcall OnMenuConvertToRawClick(TObject *Sender);
	void __fastcall OnEditShapeClick(System::TObject* Sender); // редактор ноды
	void __fastcall OnMenuConvertToClick(TObject *Sender);
	void __fastcall OnMenuTransitionTextMoveClick(TObject *Sender);
	void __fastcall OnMenuTextAlignClick(TObject *Sender);
	void __fastcall OnMenuMakeSelfConnection(TObject *Sender);
	void __fastcall OnActionMakeSelfConnectionUpdate(TObject *Sender);
	void __fastcall OnMenuSelfConnectionAlign(TObject *Sender);
	void __fastcall OnMenuMoveBrotherUp(TObject *Sender);
	void __fastcall OnMenuMoveBrotherDown(TObject *Sender);
	void __fastcall OnMenuReplaceText(TObject *Sender);
	void __fastcall OnMenuImportBindingsAndFolders(TObject *Sender);
	void __fastcall OnMenuSelectItemWithChildren(TObject *Sender);
	void __fastcall OnMenuSelectChildren(TObject *Sender);
	void __fastcall OnMenuSelectSiblings(TObject *Sender);
	void __fastcall OnMenuConnectionOver(TObject *Sender);
	void __fastcall OnMenuExportAsImage(TObject *Sender);
	void __fastcall OnMenuCollectOutputs(TObject *Sender);
	void __fastcall OnMenuArrangeLayout(TObject *Sender);
	void __fastcall OnMenuPresetCondition(TObject *Sender);
	void __fastcall OnMenuPresetOnOff(TObject *Sender);
	void __fastcall OnMenuShapeTextAutoSize(TObject *Sender);
	void __fastcall OnMenuSetBreakpoint(TObject *Sender);
	void __fastcall OnMenuSkipDebugging(TObject *Sender);
	void __fastcall OnMenuPasteConnection(TObject *Sender);
	void __fastcall OnMenuArrangeSelfConnections(TObject *Sender);
	void __fastcall OnMenuResetChildShapePos(TObject *Sender);
	void __fastcall OnMenuCopyPropertyToClipboard(TObject *Sender);
	void __fastcall OnMenuFilterSelectionByProperty(TObject *Sender);
	void __fastcall OnMenuSwapVisualBrothers(TObject *Sender);
	void __fastcall OnMenuChangeXMLTextType(TObject *Sender);
	void __fastcall OnMenuRevertToInheritedProps(TObject *Sender);
	void __fastcall OnMenuSetInheritance(TObject *Sender);
	void __fastcall OnMenuRevertRecursive(TObject *Sender);
	void __fastcall OnMenuRevertInheritedGeometry(TObject *Sender);
	void __fastcall OnMenuInheritanceDumpDifference(TObject *Sender);
	void __fastcall OnMenuRevertInOutConnections(TObject *Sender);
	void __fastcall OnMenuInheritanceResolveFirstMismatch(TObject *Sender);
	void __fastcall OnMenuInheritanceResolveAllMismatches(TObject *Sender);
	void __fastcall OnMenuInheritanceResolveClearAll(TObject *Sender);
	void __fastcall OnMenuExpandCollapseChildren(TObject *Sender);
	void __fastcall OnMenuRevertInInheritedUnits(TObject *Sender);

	void __fastcall OnMenuTransitionMovePoints(TObject *Sender);
	void __fastcall OnMenuTransitionCopy(TObject *Sender);
	void __fastcall OnMenuClearConnectionClipboardMap(TObject *Sender);
	void __fastcall OnMenuTransitionTextAlign(TObject *Sender);
	void __fastcall OnMenuTransitionFindSourceTarget(TObject *Sender);
	void __fastcall OnMenuTransitionSwap(TObject *Sender);
	void __fastcall OnMenuTransitionSwitch(TObject *Sender);
	void __fastcall OnMenuTransitionConvertToClick(TObject *Sender);
	void __fastcall OnMenuTransitionEnableTriggerClick(TObject *Sender);

	void __fastcall OnMenuPointArrangeMirror(TObject *Sender);
	void __fastcall OnMenuPointArrangeSingle(TObject *Sender);
	void __fastcall OnMenuPointPinClick(TObject *Sender);
	void __fastcall OnMenuPointAlignOppositeHorizontallyClick(TObject *Sender);
	void __fastcall OnMenuPointAlignOppositeVerticallyClick(TObject *Sender);

	void __fastcall OnMenuDummyTextReset(System::TObject* Sender);
	void __fastcall OnMenuDummyTextEdit(System::TObject* Sender);
	void __fastcall OnMenuDummyTextAlign(System::TObject* Sender);
	void __fastcall OnMenuDummyTextEditCondition(System::TObject* Sender);
	void __fastcall OnMenuDummyTextRotate90Left(System::TObject* Sender);
	void __fastcall OnMenuDummyTextRotate90Right(System::TObject* Sender);
	void __fastcall OnMenuDummyTextRotateCustom(System::TObject* Sender);
	void __fastcall OnMenuDummyTextVerticalCentering(System::TObject* Sender);

	void __fastcall OnMenuChangeConnectionTypeFrameTransition(System::TObject* Sender);
	void __fastcall OnMenuConnectionOverFrameTransition(System::TObject* Sender);

	void __fastcall OnMenuChartVersion(System::TObject* Sender);
	void __fastcall OnMenuChartStatistics(System::TObject* Sender);

	void __fastcall CheckImageAdvancedDrawItem(TObject *Sender, TCanvas *ACanvas, const TRect &ARect, TOwnerDrawState State);

	std::map<UnicodeString, int>FShapeImagesMap;

	template<typename T>
	void AddShapeImageToMap() {
		std::auto_ptr<T>AShape(new T(this));
		TPicture *APicture = AShape->GetPicture();
		if (APicture) {
			std::auto_ptr<Graphics::TBitmap>ABitmap(new Graphics::TBitmap());
			ABitmap->Assign(APicture->Graphic);
			// если размер будет другим, то будет ошибка
			ABitmap->Height = 16;
			ABitmap->Width = 16;

			FShapeImagesMap[AShape->XMLName] = this->ImageList1->Add(ABitmap.get(), 0);
		}
	}

	template<typename T>
	void AddVisualShapeImageToMap() {
		std::auto_ptr<T>AShape(new T(this));
		std::auto_ptr<Graphics::TBitmap>ABitmap(new Graphics::TBitmap());
		ABitmap->Width = 16;
		ABitmap->Height = 16;
		ABitmap->PixelFormat = pf24bit;
		ABitmap->Transparent = true;

		TRect tmpR = TRect(0, 0, 16, 16);

		std::auto_ptr<TTeeCanvas3D>ACanvas3D(new TTeeCanvas3D());
		ACanvas3D->UseBuffer = false;
		ACanvas3D->InitWindow(ABitmap->Canvas, NULL, clWindow, false, tmpR);

		TRect tmpR2 = TRect(1, 1, 15, 15);
		reinterpret_cast<TAccessNodeShape*>(AShape.get())->DrawShapeCanvas(ACanvas3D.get(), tmpR2);

		FShapeImagesMap[AShape->XMLName] = this->ImageList1->Add(ABitmap.get(), 0);
	}

	void __fastcall AddAllShapeImages(void);

	void __fastcall OnSearchShapeTimer(TObject *Sender);

	void __fastcall UpdateAllChildrenSimpleText();

	bool __fastcall CollectOutputs(TScxmlBaseShape *AParent);

	bool __fastcall GetIsStandalone(void);

	bool IsValidForConvertToParallel();

	UnicodeString __fastcall GetUnitFileName(void);

	TTreeEx * __fastcall GetTheTreeEx(void); // throws an exception if NULL

	TStateMachineDockPanel * __fastcall GetStateMachineDockPanel(void);

	std::set<TScxmlBaseShape*> 				FInheritanceCheckShapes;
	std::set<TStateMachineConnection*> 		FInheritanceCheckConnections;
	std::set<TCustomTreeElement*>			FUpdateNodeTreeElements;
	std::pair<bool, TDateTime>				FMouseWheelEventTimedFlag; // информация о том, что двигалось колесо мыши (будет сброшено в OnIdle)
	bool									FTreeModifiedForInheritanceFlag;
	bool									FTreeNewConnectionToolbarFlag;
	bool									FTreeValidateSimpleTextFlag;
	bool									FTreeFillNodeTreeFlag;
	bool									FRestoreInspectorStateFlag;

	TOperationResult __fastcall ReloadInheritedTree(bool bCheckWasModified = false);
	void __fastcall DoCheckInheritanceAndMarkVisibly(bool &bRequireInvalidateTree, int &iNotFoundBaseCount);
	void __fastcall CheckInheritedMismatches();

    void __fastcall CreateConnectionToolBar(TStateMachineConnection *AConnection);

	// убрал из доступа, так как в некоторых случаях срабатывает AccessViolation,
	// поэтому использовать функцией RestoreInspectorStateDelayed
	void __fastcall RestoreInspectorState(void);

	UnicodeString __fastcall GetDataModel();
	UnicodeString __fastcall GetSyntaxScheme();

   	// фигура, по которой только что сработал BeforeEnter
	TVisualScxmlBaseShape *FActiveDebugShape;
	TStateMachineConnection *FActiveDebugConnection;

	std::set<TVisualScxmlBaseShape *> FEnteredDebugShapes;
	std::set<TStateMachineConnection *> FEnteredStateMachineConnections;

protected:
	friend class TStateMachineDockPanel;
	friend class TFormStateEditor;
	friend class TFormTransitionEditor;
	friend class TScxmlBaseShape;
	friend class TStateMachineConnection;
	friend class TSyntaxStringClassEditor;
	friend class TSyntaxStringPropEditor;
	friend class TSyntaxString;
	friend class TFormReplaceDlg;
	friend class TFormScxmlGui;
	friend class TDialogInsight;

	void __fastcall SaveInspectorState(void);
	inline void __fastcall RestoreInspectorStateDelayed(void) { FRestoreInspectorStateFlag = true; }

	virtual void __fastcall LoadTree(const System::UnicodeString AFileName);
	virtual void __fastcall LoadScxmlData(const UnicodeString &sData, const UnicodeString sEditorCaption);
	virtual void __fastcall LoadScxmlStream(TStream *AStream, const UnicodeString sEditorCaption);
	virtual void __fastcall ResetTree(void);
	virtual void __fastcall FillNodeTree(void);
	virtual void __fastcall FinalizeLoadTree(const UnicodeString sEditorCaption, const TFinalizeScxmlType AFinalizeType);
	virtual void __fastcall TeeModified(bool YesNo = true, int ACat = 0x0, const System::UnicodeString ADescription = L"");
	virtual System::UnicodeString __fastcall NodeTreeText(Teetree::TCustomTreeElement* AElement);

	virtual	void __fastcall DoOnEditorApplicationEventsIdle(TObject *Sender, bool &Done);
	virtual void __fastcall DoTheTreeSelectConnection(Teetree::TTreeConnection* Sender);
	virtual void __fastcall ClearFlags(void); // уберет все флаги в OnIdle

	void ResetInheritance();
	void SetNewInheritance();
	inline void __fastcall MarkModifiedForInherited(void) { FTreeModifiedForInheritanceFlag = true; }

	DYNAMIC void __fastcall SetNewTreeSettings(void);
	DYNAMIC void __fastcall LoadEditorParameters(void) {} // убираем родную, так как переопределно в виртуальном классе
    // не делать виртуальным, вызывается из конструктора
	void __fastcall LoadEditorParametersEx(void);	// делаем свой вызов
	DYNAMIC void __fastcall SaveEditorParameters(void);

	virtual Teetree::TTreeNodeShape* __fastcall AddNewShape(PTreeTagShape AClass, const Types::TRect &R);

	virtual void __fastcall TheTreeStartEdit(TTreeNodeShape* Shape, bool &AllowEditing);
	virtual void __fastcall TheTreeStopEditing(TTreeNodeShape *Sender);
	virtual void __fastcall TheTreeNewConnection(TCustomTree *Sender, TTreeConnection *NewConnection);
	virtual void __fastcall TheTreeOnAddingConnection(TTreeNodeShape* Node1, TTreeNodeShape* Node2, bool &Add);
	virtual void __fastcall TheTreeDeletingShapes(TSelectedShapeList* Sender, bool &AllowDelete);
	virtual void __fastcall TheTreeDeletedShapes(TObject *Sender);
	virtual void __fastcall TheTreeSelectShape(Teetree::TTreeNodeShape* Sender);
	virtual void __fastcall TheTreeUnSelectShape(Teetree::TTreeNodeShape* Sender);
	virtual void __fastcall TheTreeSelectConnection(Teetree::TTreeConnection* Sender);
	virtual void __fastcall TheTreeUnSelectConnection(Teetree::TTreeConnection* Sender);
	virtual void __fastcall TheTreeDblClickShape(Teetree::TTreeNodeShape* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall TheTreeDblClickConnection(Teetree::TTreeConnection* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall TheTreeExpandingCollapsing(TTreeNodeShape *Sender, bool &Expand);
	virtual void __fastcall TheTreeAfterDraw(System::TObject* Sender);
	virtual void __fastcall TheTreeSelectingConnectionOnMouseUp(Teetree::TTreeConnection* Sender, bool &CanSelect);
	virtual void __fastcall TheTreeMovingShape(Teetree::TTreeNodeShape* Sender, int &DeltaX, int &DeltaY);
	virtual void __fastcall TheTreeResizingShape(Teetree::TTreeNodeShape* Sender, Teetree::TTreeShapeHandle ACorner, int &DeltaX, int &DeltaY);
	virtual void __fastcall TheTreeKeyDown(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift);
    virtual void __fastcall TheTreeMouseDown(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall TheTreeMouseUp(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall TheTreeMouseWheelEvent(System::TObject* Sender, Classes::TShiftState Shift, int WheelDelta, const Types::TPoint &MousePos, bool &Handled);
	virtual void __fastcall TheTreeDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
	virtual void __fastcall TheTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    virtual void __fastcall TheTreeDeletingConnection(Teetree::TTreeConnection* Sender, bool &AllowDelete);

	virtual void __fastcall TryPopup(Teetree::TCustomTree* ATree, int x, int y, int xScreen, int yScreen);
	TPopupMenu * __fastcall GetPreparedPopupMenu(Teetree::TCustomTree* ATree,
		const int X, const int Y, const int iXScreen, const int iYScreen);

	virtual void __fastcall DoUndo(void);
	virtual void __fastcall DoPaste(bool UseCursorPos);
	virtual void __fastcall DoEditScxmlShapes(TScxmlBaseShape *ABaseShape);
	virtual void __fastcall OnShapeMoveResized();
	virtual void __fastcall OnConnectionReconnected(TTreeConnection *AConnection);
	virtual void __fastcall OnConnectionStyleChanged(TTreeConnection *AConnection);
	virtual void __fastcall OnMenuClone(TObject *Sender);

	void __fastcall EnqueeScxmlShapeForInheritanceCheck(TScxmlBaseShape *AShape);
	void __fastcall EnqueeScxmlConnectionForInheritanceCheck(TStateMachineConnection *AConnection);

	void __fastcall HandleExceptionDuplicateStateID(EScxmlDuplicateStateIDException * E);

	void __fastcall ExamChangeParent(Teetree::TTreeNodeShape* Sender, int DeltaX, int DeltaY);
	UnicodeString __fastcall GetScxmlName(void);
	TScxmlShape * __fastcall GetRootScxml(void);
	TScxmlBaseShape *__fastcall GetFirstSelected(void);
	TTextDummyShape *__fastcall GetTextDummyShape(void);

	/* NodeTree Section */
	virtual void __fastcall DoAddFillNodePicture(TTreeNodeShape *AToShape, TCustomTreeElement* AFromElement);
	virtual TTreeNodeShape * __fastcall DoProcessAddFillNode(TTreeNodeShape *AToNode, TCustomTreeElement * AFromElement);
    virtual void __fastcall NodeTreeMouseDown(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall NodeTreeDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
	virtual void __fastcall NodeTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y);
	bool __fastcall IsNodeTreeSelectionDragReady();

	__classmethod virtual TSpeedButtonClass __fastcall TreeSpeedButtonClass();

	void __fastcall OnActionConnectionHighlightExecute(TObject *Sender);
	void __fastcall OnActionConnectionHighlightUpdate(TObject *Sender);

	void __fastcall OnMsgDeleteTreeShape(TMessage &AMsg);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_SCXML_DELETE_TREE_SHAPE, TMessage, OnMsgDeleteTreeShape)
	END_MESSAGE_MAP(TTreeEditorEx)

public:
	__fastcall TStateMachineEditor(TComponent *AOwner,
								   TStateMachineEditorUnit *AStateMachineEditorUnit,
								   bool AUseGlobalPropInspector);
	virtual __fastcall ~TStateMachineEditor();

	virtual void __fastcall MarkModified(int iCategory, const UnicodeString &sMsg, bool bFillNodeTree);
	inline void __fastcall MarkValidateSimpleText(void) { FTreeValidateSimpleTextFlag = true; }
	void __fastcall MarkUpdateNodeTreeElement(TCustomTreeElement *AElement);

	static UnicodeString __fastcall GetOpenDialogFilter(void);

	static void __fastcall ConvertTextToHPP(TStrings *AScxmlLines, const UnicodeString &sHeaderName);

	void __fastcall ConvertScxmlDataToDesign(const UnicodeString &sScxmlData);

	bool __fastcall FindAndSelect(const UnicodeString &sWhat, const TEditorSearchTypes ATypes, bool bFromBeginning, const bool bSelectAndStop);

	bool __fastcall AreAllSelectedOfSameClass();
	bool __fastcall AreAllSelectedOfInheritedClass(TMetaClass *AClass);
	bool __fastcall AreAllSelectedBrothers();

	// удаляет dummy shape и т.д.
	virtual void __fastcall DeleteTemporaryShapes(void);

	void __fastcall ResetActiveDebugShape();

	bool WasEnteredShape(TVisualScxmlBaseShape *AShape);
	bool WasEnteredConnection(TStateMachineConnection *AConnection);

	void __fastcall ClearWasEnteredElements(void);

	virtual void __fastcall SelectConnection(TTreeConnection *AConnection, const bool bScrollInView = SCROLL_CHART_IN_VIEW); /* override */
	virtual void __fastcall SelectShape(TTreeNodeShape *AShape, const bool bScrollInView = SCROLL_CHART_IN_VIEW); /* override */
	void __fastcall SelectMultipleShape(TTreeNodeShape *AShape, const bool bClear, const bool bScrollInView);

	void __fastcall SaveToSVG(TStringList *AOutputList, TTreeNodeShape *ARootShape = NULL);
	void __fastcall SaveToSVG(const UnicodeString &sFileName, TTreeNodeShape *ARootShape = NULL);

	void __fastcall GetDataIDs(TStrings *AIDList);
	void __fastcall GetStateNames(TStrings *AStateNamesList);

	/* VIRTUAL */
	void __fastcall SaveVirtualDependencies(TStrings *ADepsList);

	/* PRESETS */
	void __fastcall AddShapesFromPresetCondition();

	void __fastcall AddShapesFromPresetOnOff();

	/* PROPERTIES */

	__property bool							IsReloadingTree = {read=FIsReloadingTree};

	__property TTreeEx *					TheTreeEx = {read=GetTheTreeEx};

	__property TLMDPropertyInspector * 		PropSettingsInspector = {read = GetPropSettingsInspector};

	__property TLMDDesignObjects * 			DesignObjects = {read = GetDesignObjects};

	__property TStateMachineEditorUnit * 	StateMachineEditorUnit = {read = FStateMachineEditorUnit};

	__property TStateMachineDockPanel *		StateMachineDockPanel = {read = GetStateMachineDockPanel};

	__property bool 						IsStandalone = {read=GetIsStandalone};

	__property TScxmlShape *				RootScxml = {read=GetRootScxml};

	__property TScxmlBaseShape *			FirstSelected = {read=GetFirstSelected};

	__property TTextDummyShape *			TextDummyShape = {read=GetTextDummyShape};

	// получаем у первого TScxmlShape, которые встретится в проекте
	__property UnicodeString 				ScxmlName = {read=GetScxmlName};
	__property UnicodeString 				DataModel = {read=GetDataModel};
	__property UnicodeString 				SyntaxScheme = {read=GetSyntaxScheme};

	__property UnicodeString 				UnitFileName = {read=GetUnitFileName};

	__property TVisualScxmlBaseShape *		ActiveDebugShape = {read=FActiveDebugShape};
};

#define FIX_PANEL_ENTERED		HelpContext

class TStateMachineDockPanel : public TLMDDockPanel {
private:
	void __fastcall OpenedDocClose(TObject *Sender);
	void __fastcall OpenedDocShow(TObject *Sender);
	void __fastcall OpenedDocOpening(TObject *Sender, TLMDProjDocument *ADocument);
	void __fastcall OpenedDocAfterSave(TObject *Sender, const TLMDString AFilePath);
	void __fastcall OpenedDocOnSave(System::TObject* Sender, const Lmdtypes::TLMDString AFileIdent, Classes::TStream* AFileStream,
		bool &AHandled);

	TStateMachineEditor *__fastcall GetInheritedEditor(void);
	TStateMachineEditorUnit *__fastcall GetInheritedUnit(void);

	TStateMachineEditor *FStateMachineEditor;
	TStateMachineEditorUnit *FStateMachineEditorUnit;
	TToolBar *FToolBar;
	TPanel *FMainPanel; // добавлена из-за глюка отрисовки TToolBar

	bool __fastcall CloseQuery();

	void __fastcall OnTreeModified(TObject *Sender);
	void __fastcall OnStateMachineDockPanelEnter(TObject *Sender);
	void __fastcall OnStateMachineDockPanelExit(TObject *Sender);
	void __fastcall OnMouseActivateEvent(System::TObject* Sender, TMouseButton Button, Classes::TShiftState Shift, int X, int Y, int HitTest, TMouseActivate &MouseActivate);

	TDateTime FExitTime; // время последней активности панели

	UnicodeString __fastcall GetRawScxml();

	UnicodeString __fastcall GetRawScxmlForceComments();

	UnicodeString __fastcall GetScxmlName();

	UnicodeString __fastcall GetInvokeID();

	UnicodeString __fastcall GetDataModel();

	void __fastcall CollectShapeBindings(TCustomTree *ATree, ILMDXmlElement *ANodeContainer);

    TRect __fastcall GetAdjustedClientRect(void);

	bool FReloadInheritanceFlag;

	std::map<UnicodeString, TStateMachineConnection *> FTakingTransitionCacheMap;

	void __fastcall ProcessTakenTransition(TStateMachineConnection *AConnection);

protected:

	friend class TStateMachineEditor;

	void __fastcall MarkReloadInheritance(void);

public:
	__fastcall virtual TStateMachineDockPanel(Classes::TComponent* AOwner, TStateMachineEditorUnit *AStateMachineEditorUnit);
	__fastcall virtual ~TStateMachineDockPanel(void);

	TLMDOpenedDocument *OpenedDoc;

	__property TDateTime ExitTime = {read = FExitTime};

	inline void __fastcall InvalidateTree() { if (FStateMachineEditor) FStateMachineEditor->InvalidateTheTreeWithTemp(); };

	inline void __fastcall RefreshTree() { if (FStateMachineEditor) FStateMachineEditor->RefreshTheTreeWithTemp(); };

	// помечает редактор измененнным + обновляет Caption
	void __fastcall MarkModified(int iCategory, const UnicodeString &sMsg, bool bFillNodeTree);

	std::pair<TStateMachineDockPanel*, TTreeNodeShape *> __fastcall EnterStateNode(const UnicodeString &sId, const bool bEntered,
		const bool bOpenClosedUnits);

	std::pair<TStateMachineDockPanel*, TTreeConnection *> __fastcall TakingTransition(
		const UnicodeString &sFromShape, const int iTransitionID,
		const bool bOpenClosedUnits
	);

	void __fastcall ClearEnteredStates(void);

	void __fastcall ClearAllBreakpoints(void);

	void __fastcall SaveRawScxmlToFile(const UnicodeString &sFileName);
	void __fastcall SaveRawScxmlToHPP(const UnicodeString &sFileName);
	void __fastcall SaveScxmlToPas(const UnicodeString &sFileName);

	void __fastcall UpdateCaption(void);
	void __fastcall UpdateSyntaxView(void);

	// если это вкладка, то переключить на нее
	void __fastcall ActivateZonePage(void);
	
	__property UnicodeString 				RawScxml = {read = GetRawScxml}; // отображение комментариев указывается в настройках
	__property UnicodeString 				RawScxmlForceComments = {read = GetRawScxmlForceComments}; // всегда отображать
	__property TStateMachineEditor *		StateMachineEditor = {read = FStateMachineEditor};
	__property TStateMachineEditorUnit *	StateMachineEditorUnit = {read=FStateMachineEditorUnit};
	__property TStateMachineEditor *		InheritedEditor = {read = GetInheritedEditor};
	__property TStateMachineEditorUnit *	InheritedUnit = {read = GetInheritedUnit};

	// получаем у первого TScxmlShape, которые встретится в проекте
	__property UnicodeString 				ScxmlName = {read=GetScxmlName};
	__property UnicodeString 				InvokeID = {read=GetInvokeID};
	__property UnicodeString 				DataModel = {read=GetDataModel};
	// создаем панельку, чтобы корректно ToolBar отображался
	__property TPanel *						MainPanel = {read=FMainPanel};
};

class TScxmlSpeedButton : public Buttons::TSpeedButton {
	typedef Buttons::TSpeedButton inherited;

protected:

	virtual void __fastcall Paint(void);

public:
	/* TSpeedButton.Create */
	inline __fastcall virtual TScxmlSpeedButton(Classes::TComponent* AOwner) : Buttons::TSpeedButton(AOwner) {
	}

	/* TSpeedButton.Destroy */ inline __fastcall virtual ~TScxmlSpeedButton(void) {
	}

};

/* Units */
extern TStateMachineEditorUnit *FindStateMachineUnitByFileName(const UnicodeString &sFileName);
extern TStateMachineEditor *FindStateMachineEditorByFileName(const UnicodeString &sFileName);
extern TStateMachineEditor *FindOpenedStateMachineEditorByFileName(const UnicodeString &sFileName);

/* Global */
extern TStateMachineDockPanel * __fastcall GlobalActiveEditorDockPanel();
extern TCustomTree * __fastcall GlobalActiveEditorTree();
extern TScxmlShape * __fastcall GlobalActiveEditorRootScxml();

/* Editor */
extern boost::tuple<TComponent *, TStateMachineEditor *, TStateMachineDockPanel *> ParseInstancePropPath(const UnicodeString &sInstancePropPath);

/* TreeImagePool indexes */
extern int g_TransitionSimpleIndex;
extern int g_TransitionSelfIndex;
extern int g_BreakpointActiveIndex;
extern int g_BreakpointInactiveIndex;

#endif
