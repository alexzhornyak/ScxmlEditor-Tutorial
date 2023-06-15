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

#ifndef UnitMainH
#define UnitMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDDckSite.hpp"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ActnList.hpp>
#include "LMDIdeProjMgr.hpp"
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include "LMDDsgPropInsp.hpp"
#include "LMDInsPropInsp.hpp"
#include "LMDInsPropPage.hpp"
#include "LMDDsgObjects.hpp"
#include <Grids.hpp>
#include "ScxmlMsgTypes.h"
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdUDPBase.hpp"
#include "IdUDPServer.hpp"
#include "LMDSedDocument.hpp"
#include "LMDSedView.hpp"
#include "ElComponent.hpp"
#include "ElDragDrop.hpp"
#include <AppEvnts.hpp>
#include <StdActns.hpp>
#include "IdThreadComponent.hpp"
#include <Buttons.hpp>
#include "TeeProcs.hpp"
#include "TeeTree.hpp"
#include "ElListBox.hpp"
#include "ElXPThemedControl.hpp"
#include "HTMLLbx.hpp"
#include "ElListBox.hpp"
#include "ElXPThemedControl.hpp"
#include "HTMLLbx.hpp"

#include <memory>
#include <set>
#include <queue>
#include "SafeQueue.hpp"
#include "CommonConsts.h"
#include "LMDInsPropEditors.hpp"

typedef enum {csctScxmlType, csctMsg, csctStatemachineName, csctIdentifier} TCallStackColumnType;

class TStateMachineEditorUnit;
class TStateMachineDockPanel;
class TStateMachineEditor;
class TStateMachineProject;
class TSpawnAndRedirect;
class TFrameSyntaxScxmlXMLView;

// должно быть в hpp, иначе пишет Not Accessible в Release
class AccessDockPanel : public TLMDDockPanel {
public:
	void __fastcall SetMouseActivateEvent(const TMouseActivateEvent &AOnMouseActivate);
	void __fastcall GetFixAdjustClientRect(TRect &ARect);
};

//---------------------------------------------------------------------------
class TFormScxmlGui : public TForm
{
__published:	// IDE-managed Components
	TControlBar *ControlBar1;
	TToolBar *ReportToolbar;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TLMDDockSite *LMDDockSite1;
	TLMDDockPanel *DockPanelProjectTree;
	TLMDDockPanel *DockPanelLog;
	TLMDDockPanel *DockPanelPropInsp;
	TLMDDockPanel *DockPanelFrameRoot;
	TMainMenu *MainMenu1;
	TLMDDockManager *LMDDockManager1;
	TMenuItem *File1;
	TMenuItem *Newproject1;
	TSplitter *Splitter1;
	TMenuItem *View;
	TMenuItem *MenuDockpanels;
	TLMDProjectManager *ProjectManager1;
	TLMDProjectManagerView *ProjectManagerView1;
	TMenuItem *ActionCloseProject1;
	TPopupMenu *PopupProjView;
	TMenuItem *MenuProjViewClose;
	TMenuItem *MenuProjViewOpen;
	TSaveDialog *SaveDialog;
	TActionList *ActionList;
	TAction *actNewProj;
	TAction *actAddUnit;
	TAction *actSave;
	TAction *actSaveAs;
	TAction *actSaveProjectAs;
	TAction *actSaveAll;
	TAction *actClose;
	TAction *actCloseAll;
	TAction *actOpenProject;
	TAction *actOptions;
	TAction *actRun;
	TAction *actStop;
	TImageList *ActionsImages;
	TMenuItem *AddUnit1;
	TMenuItem *Save1;
	TMenuItem *SaveAs1;
	TMenuItem *SaveProjectAs1;
	TOpenDialog *OpenDialog;
	TMenuItem *Close2;
	TMenuItem *Open2;
	TMenuItem *Open3;
	TMenuItem *N1;
	TAction *actRemoveUnit;
	TMenuItem *Removeunit1;
	TMenuItem *MenuProjViewSave;
	TMenuItem *MenuProjViewSaveAs;
	TMenuItem *MenuProjViewRemove;
	TMenuItem *MenuProjViewAddUnit;
	TMenuItem *N2;
	TMenuItem *N3;
	TToolButton *ToolButton4;
	TToolButton *ToolButton5;
	TAction *actAddExisting;
	TMenuItem *AddExistingUnit1;
	TMenuItem *MenuProjAddExisting;
	TMenuItem *N4;
	TToolButton *ToolButton6;
	TToolButton *ToolButton7;
	TAction *actDefaultLayout;
	TMenuItem *Defaultlayout1;
	TMenuItem *N5;
	TMenuItem *MenuRecentLine;
	TMenuItem *MenuTools;
	TMenuItem *PropjectOptions1;
	TLMDDesignObjects *DesignObjects1;
	TAction *actOpenUnit;
	TMenuItem *Openunit1;
	TToolButton *ToolButton8;
	TToolButton *ToolButton9;
	TToolButton *ToolButton10;
	TAction *actPause;
	TToolButton *ToolButton11;
	TAction *actNextStep;
	TToolButton *ToolButton12;
	TLMDDockPanel *DockPanelTriggers;
	TScrollBox *ScrollBoxTriggers;
	TLMDDockPanel *DockPanelCallStack;
	TIdUDPServer *UDPServer1;
	TAction *actAppSettings;
	TMenuItem *Applicationsettings1;
	TMenuItem *N6;
	TAction *actNewUnit;
	TMenuItem *NewUnit1;
	TAction *actExportToRawScxml;
	TMenuItem *Exporttorawscxml1;
	TAction *actExportToHPP;
	TMenuItem *ExporttoHPP1;
	TTabControl *TabControlLog;
	TLMDEditView *EditViewLog;
	TLMDEditDocument *EditDocDebug;
	TLMDEditDocument *EditDocTester;
	TMenuItem *MenuProjPathToClipboard;
	TMenuItem *N7;
	TElDropTarget *ElDropTarget1;
	TAction *actShowCallStack;
	TApplicationEvents *ApplicationEvents1;
	TToolButton *ToolButton13;
	TToolButton *ToolButton14;
	TToolButton *ToolButton15;
	TLMDDockPanel *DockPanelBindings;
	TAction *actSearch;
	TMenuItem *MenuEdit;
	TMenuItem *Search1;
	TAction *actSaveFlat;
	TMenuItem *Saveflat1;
	TMenuItem *N8;
	TAction *actExportToDFM;
	TMenuItem *ExporttoPas1;
	TMenuItem *MenuRun;
	TMenuItem *Run1;
	TMenuItem *actPause1;
	TMenuItem *Stop1;
	TMenuItem *Nextstep1;
	TAction *actBuildProject;
	TMenuItem *N9;
	TMenuItem *Build1;
	TMenuItem *Build2;
	TAction *actExternTesting;
	TToolButton *ToolButton16;
	TPopupMenu *PopupMenuStack;
	TAction *actStackClear;
	TMenuItem *Clearallstackitems1;
	TToolButton *ToolButton17;
	TAction *actSetPanelForeground;
	TAction *actShapeScrollToView;
	TToolButton *ToolButton18;
	TMenuItem *Scrollcharttoviewshape1;
	TMenuItem *Setpanelforeground1;
	TMenuItem *MenuSpacerBeforeTester;
	TMenuItem *Clearallstackitems2;
	TAction *actClearEnteredStates;
	TToolButton *ToolButton19;
	TMenuItem *Clearenteredstates1;
	TFileSaveAs *actExportAnsiC;
	TMenuItem *ExportasANSIC1;
	TMenuItem *Help1;
	TMenuItem *MenuLuaviaHelp;
	TPopupMenu *PopupMenuBindings;
	TMenuItem *CopyScxmlNametoClipboard1;
	TMenuItem *CopyAviaNametoClipboard1;
	TAction *actPresetsCondition;
	TMenuItem *WizardCondition1;
	TMenuItem *N11;
	TMenuItem *MenuPresets;
	TAction *actPresetsOnOff;
	TAction *actForceSaveAllOpened;
	TMenuItem *ForceSaveAllOpened;
	TIdThreadComponent *ThreadTesterComponent;
	TMenuItem *MenuProjUnitScxmlName;
	TMenuItem *MenuStackCopyMsg;
	TMenuItem *MenuStackCopyScxmlName;
	TLMDDockPanel *DockPanelScxmlView;
	TAction *actDockPanelScxmlViewVisible;
	TToolButton *ToolButton20;
	TMenuItem *Scxmlviewshowhide1;
	TToolButton *ToolButton21;
	TAction *actTranslateChangesToScxmlView;
	TFileOpen *actLoadLayout;
	TFileSaveAs *actSaveLayout;
	TMenuItem *Loadlayout1;
	TMenuItem *Savelayoutas1;
	TMenuItem *OnOff1;
	TMenuItem *Startlisteningfromexternaltester1;
	TMenuItem *N12;
	TAction *actClearAllBreakpoints;
	TMenuItem *Clearallbreakpoints1;
	TAction *actUpdateBindings;
	TMenuItem *MenuStackFilterMsg;
	TToolButton *ToolButton22;
	TAction *actClearClipboardConnections;
	TMenuItem *Clearclipboardconnections1;
	TToolButton *ToolButton23;
	TToolButton *ToolButton24;
	TAction *actShowTextEditor;
	TMenuItem *N13;
	TMenuItem *exteditor1;
	TPopupMenu *PopupMenuLog;
	TAction *actClearLog;
	TMenuItem *Clearlog1;
	TMenuItem *Clearlog2;
	TToolButton *ToolButton25;
	TAction *actPanelPropPath;
	TMenuItem *Propertypathpanel2;
	TFileSaveAs *actExportQt;
	TMenuItem *Exportusingqscxmlc1;
	TMenuItem *SelectWithoutScrollChart;
	TMenuItem *N14;
	TMenuItem *MenuProjUnitEnabled;
	TMenuItem *N15;
	TMenuItem *Hotkeys1;
	TAction *actEnableZoomHints;
	TAction *actAutoOpenUnit;
	TToolButton *ToolButton26;
	TMenuItem *Export1;
	TMenuItem *Importstatesconfiguration1;
	TMenuItem *N16;
	TMenuItem *MenuProjEditOrder;
	TFileOpen *actImportConfiguration;
	TMenuItem *MenuProjCloneUnit;
	TMenuItem *MenuProjAddAllInvokeSrc;
	TMenuItem *MenuProjFileNameToClipboard;
	TMenuItem *MenuProjAddExistingUnitWithAllNestedUnits;
	TMenuItem *MenuProjEditPostSave;
	TMenuItem *MenuProjEditInherited;
	TMenuItem *MenuProjBindingsDisabled;
	TMenuItem *N17;
	TMenuItem *MenuProjRoot4Build;
	TLMDDockPanel *DockPanelSearch;
	TAction *actSearchAll;
	TMenuItem *SearchAll1;
	TLMDDockPanel *DockPanelGifMaker;
	TAction *actLockDocking;
	TToolButton *ToolButton27;
	TToolButton *ToolButton28;
	TMenuItem *LockUI1;
	TMenuItem *N18;
	TMenuItem *MenuProjVirtual;
	TMenuItem *N19;
	TMenuItem *MenuProjOptions;
	TAction *actGetScreenCursorCoordinates;
	TMenuItem *GetMousePos1;
	TMenuItem *N20;
	TAction *actRegExpDialog;
	TMenuItem *estregularexpression1;
	TLMDDockPanel *DockPanelTransitionXML;
	TAction *actXMLTransitionView;
	TToolButton *ToolButton29;
	TToolButton *ToolButton30;
	TPanel *PanelBindingsRoot;
	TEdit *EditBindings;
	TButton *BtnRefreshBindings;
	TPanel *PanelCallStackRoot;
	TPanel *PanelPropInspRoot;
	TPanel *PanelPropInspPath;
	TEdit *EditPropInspPath;
	TButton *BtnClosePanelPropPath;
	TCheckBox *CheckFilterProp;
	TLMDPropertyInspector *PropSettingsInspector;
	TSplitter *Splitter2;
	TMenuItem *MenuProjViewReload;
	TMenuItem *MenuProjInheritanceMismatches;
	TListView *ListViewCallStack;
	TMenuItem *MenuProjMoveUp;
	TMenuItem *MenuProjMoveDown;
	TMenuItem *MenuProjForceSaveUnit;
	TMenuItem *MenuProjForceSaveAllInherited;
	TMenuItem *MenuProjOpenAllInherited;
	TAction *actForceSaveAll;
	TMenuItem *ForceSaveAll;
	TAction *actIDEInsight;
	TMenuItem *N21;
	TMenuItem *IDEInsight1;
	TMenuItem *Showcallstack1;
	TMenuItem *N22;
	TMenuItem *MenuZoomHintsEnabled;
	TToolButton *ToolButton31;
	TToolButton *ToolButton32;
	TToolButton *ToolButton33;
	TAction *actAutoLayoutOrthoSplines;
	TAction *actAutoLayoutPolySplines;
	TMenuItem *N23;
	TMenuItem *OrthographicSplinesAutoLayout1;
	TMenuItem *PolylineSplinesAutoLayout1;
	TFileSaveAs *actExportGraphDot;
	TMenuItem *ExportasgraphDOT1;
	TFileSaveAs *actExportGraphDotPlusPng;
	TMenuItem *ExportasgraphDOTPng1;
	TLabel *LabelIDEInsight;
	TToolButton *ToolButton35;
	TAction *actRegisterScxmlFiles;
	TMenuItem *RegisterSCXMLprojectandunitfileextensions1;
	TFileSaveAs *actExportSVG;
	TMenuItem *ExportasSVG1;
	TMenuItem *MenuProjOpenShell;
	TMenuItem *MenuSpacerAfterTester;
	TTimer *TimerControlBar;
	TLabel *LabelDbgMsgCount;
	TCheckBox *CheckBox1;
	TMenuItem *MenuStackCopyID;
	TAction *actSyncInvoked;
	TMenuItem *SyncInvoked1;
	TTree *HTMLRecent;
	TPanel *Panel1;
	TTree *TreeViewBindings;
	TPanel *Panel2;
	TElHTMLListBox *HTMLPropInfo;
	TToolButton *ToolButton34;
	TAction *actTestCoverage;
	TMenuItem *estCoverage1;
	TMenuItem *MenuProjRename;
	TAction *actRevert;
	TMenuItem *Revert1;
	void __fastcall LMDDockManager1WriteAppInfo(TObject *Sender, const ILMDXmlDocument *Xml);
	void __fastcall LMDDockManager1BeforeRead(TObject *Sender, const ILMDXmlDocument *Xml);
	void __fastcall ProjectManager1Open(TObject *Sender, bool AShowEditor, TLMDProjDocOpenContext *AContext);
	void __fastcall ProjectManager1SaveQuery(TObject *Sender, TLMDProjDocument *ADocument, TLMDProjDocumentSaveQueryRes &ASave);
	void __fastcall ProjectManager1SaveAs(TObject *Sender, TLMDProjDocument *ADocument, TLMDString &AFilePath, bool &ASave);
	void __fastcall actNewProjExecute(TObject *Sender);
	void __fastcall actCloseAllExecute(TObject *Sender);
	void __fastcall actCloseAllUpdate(TObject *Sender);
	void __fastcall actAddUnitExecute(TObject *Sender);
	void __fastcall actAddUnitUpdate(TObject *Sender);
	void __fastcall actSaveExecute(TObject *Sender);
	void __fastcall actGetActiveEditorUpdate(TObject *Sender);
	void __fastcall actSaveAsExecute(TObject *Sender);
	void __fastcall actSaveProjectAsExecute(TObject *Sender);
	void __fastcall actProjectIsOpenedUpdate(TObject *Sender);
	void __fastcall actSaveAllExecute(TObject *Sender);
	void __fastcall actSaveAllUpdate(TObject *Sender);
	void __fastcall actOpenProjectExecute(TObject *Sender);
	void __fastcall ProjectManagerView1DblClick(TObject *Sender);
	void __fastcall ViewClick(TObject *Sender);
	void __fastcall MenuProjViewOpenClick(TObject *Sender);
	void __fastcall PopupProjViewPopup(TObject *Sender);
	void __fastcall MenuProjViewCloseClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall actRemoveUnitExecute(TObject *Sender);
	void __fastcall actCloseExecute(TObject *Sender);
	void __fastcall MenuProjViewRemoveClick(TObject *Sender);
	void __fastcall MenuProjViewSaveAsClick(TObject *Sender);
	void __fastcall MenuProjViewSaveClick(TObject *Sender);
	void __fastcall actAddExistingExecute(TObject *Sender);
	void __fastcall actDefaultLayoutExecute(TObject *Sender);
	void __fastcall File1Click(TObject *Sender);
	void __fastcall ProjectManager1FilePathChanged(TObject *Sender, TLMDProjDocument *ADocument);
	void __fastcall actOptionsExecute(TObject *Sender);
	void __fastcall MenuProjAddExistingClick(TObject *Sender);
	void __fastcall MenuProjViewAddUnitClick(TObject *Sender);
	void __fastcall ProjectManagerView1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall PropSettingsInspectorChange(TObject *Sender);
	void __fastcall actOpenUnitExecute(TObject *Sender);
	void __fastcall actRunExecute(TObject *Sender);
	void __fastcall actGetActivePanelAndTestingUpdate(TObject *Sender);
	void __fastcall actStopExecute(TObject *Sender);
	void __fastcall ListViewCallStackAdvancedCustomDrawSubItem(TCustomListView *Sender, TListItem *Item, int SubItem, TCustomDrawState State, TCustomDrawStage Stage,
          bool &DefaultDraw);
	void __fastcall ListViewCallStackAdvancedCustomDrawItem(TCustomListView *Sender, TListItem *Item, TCustomDrawState State, TCustomDrawStage Stage,
		  bool &DefaultDraw);
	void __fastcall actIsTesterWorkingUpdate(TObject *Sender);
	void __fastcall UDPServer1UDPRead(TIdUDPListenerThread *AThread, TBytes AData, TIdSocketHandle *ABinding);
	void __fastcall actAppSettingsExecute(TObject *Sender);
	void __fastcall actOptionsUpdate(TObject *Sender);
	void __fastcall actNewUnitExecute(TObject *Sender);
	void __fastcall actExportToRawScxmlExecute(TObject *Sender);
	void __fastcall actExportToHPPExecute(TObject *Sender);
	void __fastcall TabControlLogChange(TObject *Sender);
	void __fastcall MenuProjPathToClipboardClick(TObject *Sender);
	void __fastcall ElDropTarget1TargetDrag(TObject *Sender, TDragState State, TOleDragObject *Source, TShiftState Shift, int X, int Y, TDragType &DragType);
	void __fastcall ElDropTarget1TargetDrop(TObject *Sender, TOleDragObject *Source, TShiftState Shift, int X, int Y, TDragType &DragType);
	void __fastcall actShowCallStackExecute(TObject *Sender);
	void __fastcall actShowCallStackUpdate(TObject *Sender);
	void __fastcall ApplicationEvents1Activate(TObject *Sender);
	void __fastcall DockPanelFrameRootEnter(TObject *Sender);
	void __fastcall actSearchExecute(TObject *Sender);
	void __fastcall actSearchUpdate(TObject *Sender);
	void __fastcall actSaveFlatExecute(TObject *Sender);
	void __fastcall ApplicationEvents1Exception(TObject *Sender, Exception *E);
	void __fastcall actExportToDFMExecute(TObject *Sender);
	void __fastcall actBuildProjectExecute(TObject *Sender);
	void __fastcall actExternTestingExecute(TObject *Sender);
	void __fastcall actStackClearExecute(TObject *Sender);
	void __fastcall actSetPanelForegroundUpdate(TObject *Sender);
	void __fastcall actSetPanelForegroundExecute(TObject *Sender);
	void __fastcall actStopUpdate(TObject *Sender);
	void __fastcall actPauseExecute(TObject *Sender);
	void __fastcall actPauseUpdate(TObject *Sender);
	void __fastcall actNextStepUpdate(TObject *Sender);
	void __fastcall actNextStepExecute(TObject *Sender);
	void __fastcall actShapeScrollToViewExecute(TObject *Sender);
	void __fastcall actShapeScrollToViewUpdate(TObject *Sender);
	void __fastcall actClearEnteredStatesExecute(TObject *Sender);
	void __fastcall actExportAnsiCAccept(TObject *Sender);
	void __fastcall EditBindingsChange(TObject *Sender);
	void __fastcall TreeViewBindingsDblClick(TObject *Sender);
	void __fastcall PropSettingsInspectorClick(TObject *Sender);
	void __fastcall MenuLuaviaHelpClick(TObject *Sender);
	void __fastcall PopupMenuBindingsPopup(TObject *Sender);
	void __fastcall CopyNametoClipboard1Click(TObject *Sender);
	void __fastcall actPresetsConditionExecute(TObject *Sender);
	void __fastcall actPresetsOnOffExecute(TObject *Sender);
	void __fastcall actForceSaveAllOpenedExecute(TObject *Sender);
	void __fastcall actForceSaveAllOpenedUpdate(TObject *Sender);
	void __fastcall ThreadTesterComponentRun(TIdThreadComponent *Sender);
	void __fastcall MenuProjUnitScxmlNameClick(TObject *Sender);
	void __fastcall PopupMenuStackPopup(TObject *Sender);
	void __fastcall MenuStackCopyMsgClick(TObject *Sender);
	void __fastcall actDockPanelScxmlViewVisibleUpdate(TObject *Sender);
	void __fastcall actDockPanelScxmlViewVisibleExecute(TObject *Sender);
	void __fastcall actTranslateChangesToScxmlViewUpdate(TObject *Sender);
	void __fastcall actTranslateChangesToScxmlViewExecute(TObject *Sender);
	void __fastcall actLoadLayoutAccept(TObject *Sender);
	void __fastcall actSaveLayoutAccept(TObject *Sender);
	void __fastcall UpdateProjectsClosed(TObject *Sender);
	void __fastcall actLayoutBeforeExecute(TObject *Sender);
	void __fastcall actClearAllBreakpointsExecute(TObject *Sender);
	void __fastcall actGetActivePanelUpdate(TObject *Sender);
	void __fastcall actUpdateBindingsExecute(TObject *Sender);
	void __fastcall MenuStackFilterMsgClick(TObject *Sender);
	void __fastcall actClearClipboardConnectionsExecute(TObject *Sender);
	void __fastcall actClearClipboardConnectionsUpdate(TObject *Sender);
	void __fastcall actShowTextEditorExecute(TObject *Sender);
	void __fastcall actClearLogExecute(TObject *Sender);
	void __fastcall actPanelPropPathExecute(TObject *Sender);
	void __fastcall actPanelPropPathUpdate(TObject *Sender);
	void __fastcall PropSettingsInspectorFilterProp(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, bool AIsSubProp, bool &AIncludeProp);
	void __fastcall PropSettingsInspectorGetEditorClass(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass);
	void __fastcall CheckFilterPropClick(TObject *Sender);
	void __fastcall EditPropInspPathChange(TObject *Sender);
	void __fastcall actExportQtAccept(TObject *Sender);
	void __fastcall SelectWithoutScrollChartClick(TObject *Sender);
	void __fastcall MenuProjUnitEnabledClick(TObject *Sender);
	void __fastcall Hotkeys1Click(TObject *Sender);
	void __fastcall actSaveUpdate(TObject *Sender);
	void __fastcall actEnableZoomHintsExecute(TObject *Sender);
	void __fastcall actEnableZoomHintsUpdate(TObject *Sender);
	void __fastcall actAutoOpenUnitExecute(TObject *Sender);
	void __fastcall actAutoOpenUnitUpdate(TObject *Sender);
	void __fastcall ApplicationEvents1Idle(TObject *Sender, bool &Done);
	void __fastcall actImportConfigurationAccept(TObject *Sender);
	void __fastcall MenuProjEditOrderClick(TObject *Sender);
	void __fastcall MenuProjCloneUnitClick(TObject *Sender);
	void __fastcall PropSettingsInspectorGetCaptionColor(TObject *Sender, TLMDPropertyInspectorItem *AItem, TColor &AColor);
	void __fastcall MenuProjAddAllInvokeSrcClick(TObject *Sender);
	void __fastcall MenuProjFileNameToClipboardClick(TObject *Sender);
	void __fastcall MenuProjAddExistingUnitWithAllNestedUnitsClick(TObject *Sender);
	void __fastcall MenuProjEditPostSaveClick(TObject *Sender);
	void __fastcall MenuProjBindingsDisabledClick(TObject *Sender);
	void __fastcall MenuProjRoot4BuildClick(TObject *Sender);
	void __fastcall actSearchAllExecute(TObject *Sender);
	void __fastcall PropSettingsInspectorCompare(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo1, ILMDProperty *APropInfo2, int &ACompare);
	void __fastcall actLockDockingExecute(TObject *Sender);
	void __fastcall actLockDockingUpdate(TObject *Sender);
	void __fastcall CheckImageAdvancedDrawItem(TObject *Sender, TCanvas *ACanvas, const TRect &ARect, TOwnerDrawState State);
	void __fastcall MenuProjVirtualClick(TObject *Sender);
	void __fastcall ControlBar1Paint(TObject *Sender);
	void __fastcall actGetScreenCursorCoordinatesExecute(TObject *Sender);
	void __fastcall actRegExpDialogExecute(TObject *Sender);
	void __fastcall actExternTestingUpdate(TObject *Sender);
	void __fastcall DockPanelTransitionXMLExit(TObject *Sender);
	void __fastcall DockPanelTransitionXMLEnter(TObject *Sender);
	void __fastcall actCloseUpdate(TObject *Sender);
	void __fastcall LMDDockSite1UnDock(TObject *Sender, TControl *Client, TWinControl *NewTarget, bool &Allow);
	void __fastcall actXMLTransitionViewExecute(TObject *Sender);
	void __fastcall actXMLTransitionViewUpdate(TObject *Sender);
	void __fastcall DockPanelFixResize(TObject *Sender);
	void __fastcall ApplicationEvents1Restore(TObject *Sender);
	void __fastcall ApplicationEvents1Minimize(TObject *Sender);
	void __fastcall MenuProjViewReloadClick(TObject *Sender);
	void __fastcall MenuProjInheritanceMismatchesClick(TObject *Sender);
	void __fastcall MenuProjMoveUpClick(TObject *Sender);
	void __fastcall MenuProjMoveDownClick(TObject *Sender);
	void __fastcall MenuProjForceSaveUnitClick(TObject *Sender);
	void __fastcall MenuProjForceSaveAllInheritedClick(TObject *Sender);
	void __fastcall MenuProjOpenAllInheritedClick(TObject *Sender);
	void __fastcall actForceSaveAllExecute(TObject *Sender);
	void __fastcall actForceSaveAllUpdate(TObject *Sender);
	void __fastcall actIDEInsightExecute(TObject *Sender);
	void __fastcall actAutoLayoutOrthoSplinesExecute(TObject *Sender);
	void __fastcall actAutoLayoutOrthoSplinesUpdate(TObject *Sender);
	void __fastcall actAutoLayoutPolySplinesExecute(TObject *Sender);
	void __fastcall actAutoLayoutPolySplinesUpdate(TObject *Sender);
	void __fastcall actExportGraphDotAccept(TObject *Sender);
	void __fastcall actExportGraphDotPlusPngAccept(TObject *Sender);
	void __fastcall LabelIDEInsightMouseEnter(TObject *Sender);
	void __fastcall LabelIDEInsightMouseLeave(TObject *Sender);
	void __fastcall actRegisterScxmlFilesExecute(TObject *Sender);
	void __fastcall actExportDefExtBeforeExecute(TObject *Sender);
	void __fastcall actExportSVGAccept(TObject *Sender);
	void __fastcall MenuProjOpenShellClick(TObject *Sender);
	void __fastcall MenuRunClick(TObject *Sender);
	void __fastcall MenuPresetTesterClick(TObject *Sender);
	void __fastcall MenuStackCopyIDClick(TObject *Sender);
	void __fastcall actSyncInvokedExecute(TObject *Sender);
	void __fastcall actSyncInvokedUpdate(TObject *Sender);
	void __fastcall HTMLRecentClickShape(TTreeNodeShape *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall HTMLRecentMouseEnterShape(TTreeNodeShape *Sender, TShiftState Shift, int X, int Y);
	void __fastcall HTMLRecentMouseLeaveShape(TTreeNodeShape *Sender, TShiftState Shift, int X, int Y);
	void __fastcall actTestCoverageExecute(TObject *Sender);
	void __fastcall MenuProjRenameClick(TObject *Sender);
	void __fastcall actRevertExecute(TObject *Sender);
	void __fastcall actRevertUpdate(TObject *Sender);


private:	// User declarations

	void __fastcall InternalRevertSaveQuery(TObject * Sender, TLMDProjDocument * ADocument,
		TLMDProjDocumentSaveQueryRes & ASave);

	bool __fastcall ProjCloseQuery();

	void __fastcall DeleteAllProjects();

	void __fastcall ViewPanelClick(TObject * Sender);

	void __fastcall DoRemoveUnit(TLMDProjDocument *ANode);

    void __fastcall DoOpenProject(const UnicodeString &sFileName);  // !!! эта функция ничего не проверяет

	void __fastcall OnRecentFileClick(TObject * Sender);

	void __fastcall DoOpenUnit(const UnicodeString &sFileName);

	void __fastcall DoCloseUnit(TStateMachineEditorUnit *AUnit);

	void __fastcall DoCollectAllBindings(void);

	void __fastcall RecursiveScanUnitBindings(TLMDProjDocument *AProjDocument, TTreeNodeShape * ATreeNode);

	void __fastcall SetDockPanelStackCaption(const int iFilteredCount);

    void __fastcall ParseReceivedUDPTesterCommand(const std::wstring &wstr);

	void __fastcall SelectBinding(const bool bScrollInView);

	void __fastcall DrawAdditionalFormCaption(void);

	void __fastcall OnWatchLineReadEvent(const UnicodeString &sText);
	bool __fastcall IsTesterReadLineEventUsed(void);

	/* Inheritance */
	void __fastcall OnMenuProjResetInheritance(TObject * Sender);
	void __fastcall OnMenuProjAddInheritance(TObject * Sender);

	volatile unsigned char FBreakpointSync;

	SafeQueue<std::wstring> FLockFreeQueue;
	/* QUEUED EVENTS */
	void __fastcall OnExternTesterMsgReceived(void);
	void __fastcall StopTesting(void);

	void __fastcall UpdateControlBar(void);

	TLMDDockZone* FTriggersPrevZone;
	TLMDDockZone* FCallBackPrevZone;

	std::auto_ptr<TSpawnAndRedirect> FExternTesterSpawnPtr;
	std::auto_ptr<TSpawnAndRedirect> FConvertToAnsiCSpawnPtr;
	std::auto_ptr<TSpawnAndRedirect> FConvertToQtSpawnPtr;

	TFrameSyntaxScxmlXMLView *FSyntaxXMLView;

	std::auto_ptr<TStringList>FFilterPropsListPtr;
	bool __fastcall IsPropIncluded(const UnicodeString &sPropName, bool bIsClass);

    TUnicodeStringMap FInspectorProps;

	bool FFilterPropFlag;
	bool FScrollLogFlag;
	bool FExternTestListeningFlag;
	bool FUpdateSyntaxViewFlag;
	bool FScxmlViewVisibleFlag;
	std::set<TLMDDockPanel*> FDockPanelResizeSet;
	std::set<TStateMachineDockPanel*> FUpdateCaptionPanelsSet;
	TLMDDockZone * FBeforeUndockZone;
	TLMDDockPanel *FUndockPanel;
	bool FApplicationMinimizedFlag;
	bool FBreakpointForcedFlag;
	bool FSelectProjectManagerViewFlag;

	std::auto_ptr<TLMDPropPageStateObject> FPropInspStateSettingsPtr;
	std::auto_ptr<TLMDPropPageStateObject> FPropInspStateOptionsPtr;

    void __fastcall FixDockPanelSize(TLMDDockPanel *ADockPanel);

	void __fastcall MoveProjectNodeUpDown(const bool bUp);

	UnicodeString FRunScxmlName;

protected:

	TStateMachineDockPanel *FActiveEditorDockPanel;

	// по умолчанию имя возьмем из активного редактора
	// ScxmlGui version 1.0.0.0 [] [sDisplayName]
	void __fastcall UpdateCaption(const UnicodeString sDisplayName = L"");

	void __fastcall OnConvertedToAnsiC(TMessage &msg);
	void __fastcall OnConvertedToQt(TMessage &msg);
	void __fastcall OnStopDebugging(TMessage &msg);
	void __fastcall OnDeleteObjects(TMessage &msg);
	void __fastcall OnMsgAddOrOpenExistingUnit(TMessage &msg);
	void __fastcall OnMsgUpdateDebugQueue(TMessage &msg);
	void __fastcall OnMsgCopyData(TWMCopyData &msg);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_SCXML_STOP_DEBUGGING, TMessage, OnStopDebugging)
		VCL_MESSAGE_HANDLER(WM_SCXML_ANSI_C_CONVERTED, TMessage, OnConvertedToAnsiC)
		VCL_MESSAGE_HANDLER(WM_SCXML_QT_CONVERTED, TMessage, OnConvertedToQt)
		VCL_MESSAGE_HANDLER(WM_SCXML_DELETE_OBJECTS, TMessage, OnDeleteObjects)
		VCL_MESSAGE_HANDLER(WM_SCXML_ADD_OR_OPEN_EXISTING_UNIT, TMessage, OnMsgAddOrOpenExistingUnit)
		VCL_MESSAGE_HANDLER(WM_SCXML_UPDATE_DBG_MSG_QUEUE, TMessage, OnMsgUpdateDebugQueue)
		VCL_MESSAGE_HANDLER(WM_COPYDATA, TWMCopyData, OnMsgCopyData)
	END_MESSAGE_MAP(TForm)

	TStateMachineEditorUnit* __fastcall GetActiveUnit();
	TStateMachineDockPanel * __fastcall GetActiveEditorDockPanel();
	TStateMachineEditor * __fastcall GetActiveStateMachineEditor();
	TStateMachineProject * __fastcall GetStateMachineProject();


	void __fastcall FixDockPanelActiveState(TLMDDockPanel *APanelActive);
	void __fastcall OnFixDockPanelMouseActivate(System::TObject* Sender, TMouseButton Button, Classes::TShiftState Shift,
													int X, int Y, int HitTest, TMouseActivate &MouseActivate);

	void __fastcall DoEditSettings(const UnicodeString sItemName = L"");
	void __fastcall DoEditProjectOptions(const UnicodeString sItemName = L"");

	friend class TStateMachineDockPanel;
	friend class TStateMachineEditor;
	friend class TInvokeSrcDialogEditorImpl;
	friend class TDialogInsight;

	void __fastcall ForceShowDockPanelInRootZone(TLMDDockPanel *ADockPanel);

	inline void __fastcall SetSelectProjectManagerViewFlag(void) { FSelectProjectManagerViewFlag=true; }

public:		// User declarations
	__fastcall TFormScxmlGui(TComponent* Owner);
	virtual __fastcall ~TFormScxmlGui();

	void __fastcall SendBallonMsg(const UnicodeString &sMessage, const TBalloonFlags AFlags);

	void __fastcall UpdateHtmlRecentFiles();

	void __fastcall SwitchToProgramLog(const bool bClear = false);
	void __fastcall SwitchToTesterLog(const bool bClear = false);

	void __fastcall ScrollLog(void) { FScrollLogFlag = true; }

	void __fastcall ForceShowLogPanel(void);
	void __fastcall ForceShowPropInspPanel(void);

	void __fastcall InvalidateAllEditors(void);

	void __fastcall InvalidateAllEditorsAndResetDebug(void);

    void __fastcall SendPipeEvent(const UnicodeString &sMsg);

	TLMDProjDocument * __fastcall AddNewUnit(TLMDProjNode *AParent);

	TLMDProjDocument * __fastcall AddExistingUnit(TLMDProjNode *AParent, const UnicodeString &sFileName, bool bCheckIsOpened,
			const bool bCollectBindings = true);

	TLMDProjDocument * __fastcall FindAndAddExistingUnit(TLMDProjNode *AParent, const UnicodeString &sFileName, bool bCheckIsOpened,
			const bool bCollectBindings = true);

	inline void UpdateSyntaxView(void) { FUpdateSyntaxViewFlag = true; }
    // можно передать NULL, чтобы просто обновить заглавие формы
	inline void UpdateUnitCaption(TStateMachineDockPanel *ADockPanel) { FUpdateCaptionPanelsSet.insert(ADockPanel); }

	// тестирование может быть на базе основных свойств или свойств проекта
	bool IsTestingBasedOnProjectSettings(void);

	__property TStateMachineEditorUnit*		ActiveUnit = {read=GetActiveUnit};

	__property TStateMachineDockPanel*		ActiveEditorDockPanel = {read=GetActiveEditorDockPanel};

	__property TStateMachineEditor*			ActiveStateMachineEditor = {read=GetActiveStateMachineEditor};

	__property TStateMachineProject*        StateMachineProject = {read=GetStateMachineProject};

};

//---------------------------------------------------------------------------
extern PACKAGE TFormScxmlGui *FormScxmlGui;
//---------------------------------------------------------------------------
#endif
