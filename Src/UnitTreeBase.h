// ---------------------------------------------------------------------------

#ifndef UnitTreeBaseH
#define UnitTreeBaseH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>
#include "TeeTree.hpp"
#include <LMDDsgObjects.hpp>
#include <LMDDsgPropInsp.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include "TeeNavigator.hpp"
#include "TreeNavigator.hpp"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <StdActns.hpp>
#include <Menus.hpp>
#include "UnitDModule.h"
#include "AdvEdBtn.hpp"
#include "AdvEdit.hpp"
#include "AdvFileNameEdit.hpp"
#include "AdvToolBtn.hpp"
#include <memory>
#include <deque>

const int i_MAX_HISTORY = 100;

const int i_MIN_ZOOM_FOR_GRID = 50;

const UnicodeString s_NEW_PROJECT_NAME = L"новый проект.ttr";

class TProtocolControlBinding;
class TFrameParam;
class TFrameTriggerParam;

typedef std::pair<UnicodeString, TMemoryStream*>TUndoPair;
typedef std::deque<TUndoPair>TUndoQueue;

#define WM_SAFE_DELETE		(WM_USER + 1)

// ---------------------------------------------------------------------------
class TFrameTreeBase : public TFrame {
__published: // IDE-managed Components
	TActionList *ActionList1;
	TFileSaveAs *ActionSaveTreeAs;
	TAction *ActionClose;
	TAction *ActionZoomIn;
	TAction *ActionZoomOut;
	TAction *ActionLoadTree;
	TAction *ActionSaveTree;
	TAction *ActionUndo;
	TFileSaveAs *FileSaveAsPicture;
	TAction *ActionPrintPreview;
	TAction *ActionCopySelected;
	TAction *ActionPasteSelected;
	TAction *ActionDeleteSelected;
	TAction *ActionEditSelected;
	TAction *ActionBringToFront;
	TAction *ActionSendToBack;
	TFileOpen *FileMerge;
	TPanel *PanelTools;
	TCheckBox *CheckDesignMode;
	TTreePageNavigator *TreePageNavigator1;
	TPanel *PanelButtons;
	TBitBtn *BitBtnTreeNodeShape;
	TBitBtn *BitBtnEventShape;
	TBitBtn *BitBtnPluginShape;
	TPanel *Panel4;
	TLabel *LabelZoom;
	TTrackBar *TrackBarZoom;
	TPopupMenu *PopupMenuTree;
	TMenuItem *MenuEdit;
	TMenuItem *N19;
	TMenuItem *MenuCopy;
	TMenuItem *MenuPaste;
	TMenuItem *MenuDelete;
	TMenuItem *N20;
	TMenuItem *N26;
	TMenuItem *N27;
	TMenuItem *N28;
	TMenuItem *N21;
	TMenuItem *N22;
	TAction *ActionCheckDesign;
	TSpeedButton *SpeedBtnConnection;
	TAction *ActionEditConnection;
	TMenuItem *MenuEditConnection;
	TAction *ActionEditDSP;
	TMenuItem *Editstate1;
	TAction *ActionEditLevels;
	TMenuItem *Editlevels1;
	TMenuItem *N1;
	TAction *ActionNewProject;
	TAction *ActionTesterSync;
	TAction *ActionRedo;
	TAction *ActionFindElement;
	TAction *ActionUpdateColors;
	TMenuItem *Updatecolors1;
	TMenuItem *N2;
	TPanel *Panel2;
	TTreeRuler *TreeRuler1;
	TTreeRuler *TreeRuler2;
	TTree *TreeBase;
	TTreeNodeShape *TreeNodeShape1;
	TTreeNodeShape *TreeNodeShape2;
	TPanel *PanelSideTools;
	TAdvToolButton *AdvToolButton1;
	TAdvToolButton *AdvToolButton2;
	TPopupMenu *PopupUndoRedo;
	TAdvToolButton *AdvToolButton3;
	TAction *ActionZoomToWidth;
	TAdvToolButton *AdvToolButton4;
	TAdvToolButton *AdvToolButton5;
	TAction *ActionZoomToHeight;
	TAction *ActionZoomToFitAll;
	TAdvToolButton *AdvToolButton6;
	TAdvToolButton *AdvToolButton7;
	TAdvToolButton *AdvToolButton8;
	TAction *ActionZoom100;
	TAdvToolButton *AdvToolButton9;
	TBitBtn *BitBtnInputChannel;
	TAction *ActionZoomInCenter;
	TMenuItem *N3;
	TAdvToolButton *AdvToolButton10;
	TBitBtn *BitBtnNetwork;
	TAction *ActionTestingEnabled;
	TSpeedButton *BtnTriggersEnabled;
	TSpeedButton *BtnTestingEnabled;
	TAction *ActionAddState;
	TMenuItem *ActionAddState1;
	TAction *ActionOpenInclude;
	TMenuItem *Openinclude1;
	TTreeNodeShape *TreeNodeShape3;
	TTreeConnection *TreeNodeShape1_TreeNodeShape3;
	TTreeNodeShape *TreeNodeShape4;
	TTreeConnection *TreeNodeShape1_TreeNodeShape4;
	TTreeNodeShape *TreeNodeShape5;
	TTreeNodeShape *TreeNodeShape6;

	void __fastcall TreeBaseDblClickShape(TTreeNodeShape *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TreeBaseDeletingShapes(TSelectedShapeList *Sender, bool &AllowDelete);
	void __fastcall TreeBaseKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall TreeBaseMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TreeBaseMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TreeBaseNewShape(TCustomTree *Sender, TTreeNodeShape *NewShape);
	void __fastcall TreeBaseSelectShape(TTreeNodeShape *Sender);
	void __fastcall TreeBaseUnSelectShape(TTreeNodeShape *Sender);
	void __fastcall ActionCopySelectedExecute(TObject *Sender);
	void __fastcall ActionPasteSelectedExecute(TObject *Sender);
	void __fastcall ActionZoomInExecute(TObject *Sender);
	void __fastcall ActionZoomOutExecute(TObject *Sender);
	void __fastcall ActionUndoExecute(TObject *Sender);
	void __fastcall ActionDeleteSelectedExecute(TObject *Sender);
	void __fastcall ActionEditSelectedExecute(TObject *Sender);
	void __fastcall ActionBringToFrontExecute(TObject *Sender);
	void __fastcall ActionSendToBackExecute(TObject *Sender);
	void __fastcall PopupMenuTreePopup(TObject *Sender);
	void __fastcall TrackBarZoomChange(TObject *Sender);
	void __fastcall ActionSaveTreeAsBeforeExecute(TObject *Sender);
	void __fastcall ActionSaveTreeAsAccept(TObject *Sender);
	void __fastcall ActionCloseExecute(TObject *Sender);
	void __fastcall ActionLoadTreeExecute(TObject *Sender);
	void __fastcall FileSaveAsPictureAccept(TObject *Sender);
	void __fastcall FileMergeAccept(TObject *Sender);
	void __fastcall ActionPrintPreviewExecute(TObject *Sender);
	void __fastcall ActionSaveTreeExecute(TObject *Sender);
	void __fastcall BitBtnTreeNodeShapeClick(TObject *Sender);
	void __fastcall BitBtnEventShapeClick(TObject *Sender);
	void __fastcall BitBtnPluginShapeClick(TObject *Sender);
	void __fastcall ActionCheckDesignExecute(TObject *Sender);
	void __fastcall TreeBaseMovingShape(TTreeNodeShape *Sender, int &DeltaX, int &DeltaY);
	void __fastcall TreeBaseResizingShape(TTreeNodeShape *Sender, TTreeShapeHandle ACorner, int &DeltaX, int &DeltaY);
	void __fastcall TreeBaseDblClickConnection(TTreeConnection *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall SpeedBtnConnectionClick(TObject *Sender);
	void __fastcall TreeBaseClickConnection(TTreeConnection *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ActionEditConnectionExecute(TObject *Sender);
	void __fastcall ActionEditLevelsExecute(TObject *Sender);
	void __fastcall ActionNewProjectExecute(TObject *Sender);
	void __fastcall ActionRedoExecute(TObject *Sender);
	void __fastcall ActionFindElementExecute(TObject *Sender);
	void __fastcall ActionUpdateColorsExecute(TObject *Sender);
	void __fastcall TreeBaseStopEditing(TTreeNodeShape *Sender);
	void __fastcall PopupUndoRedoPopup(TObject *Sender);
	void __fastcall TreeBaseAfterDraw(TObject *Sender);
	void __fastcall ActionZoom100Execute(TObject *Sender);
	void __fastcall BitBtnInputChannelClick(TObject *Sender);
	void __fastcall ActionZoomInCenterExecute(TObject *Sender);
	void __fastcall BitBtnNetworkClick(TObject *Sender);
	void __fastcall ActionOpenIncludeExecute(TObject *Sender);

private: // User declarations

	void UpdateToUndoIterator(void);

	void __fastcall OnUndoRedoClicked(TObject *Sender);

	void __fastcall OnTriggerFrameEnter(TObject *Sender);

	void AssignProtocolBindingToFrameParam(TProtocolControlBinding *ABinding, TFrameTriggerParam *AFrameParam);

	void __fastcall OnTriggerParamChanged(TObject *Sender);

	void __fastcall MemoDescriptionKeyPress(TObject *Sender, wchar_t &Key);

	void __fastcall AddShapeToTree(TMetaClass *AClass, const TRect &ARect);

	void __fastcall OnZoomedAreaEvent(const Types::TRect &R);

	void __fastcall SaveTreeProject(const UnicodeString &sFileName);

	void __fastcall LoadTreeProject(const UnicodeString &sFileName);

	// загружаем из потока, если указано выделять, выделяем, каждый элемент
	void __fastcall LoadSelectedFromStream(TStream *AStream, const int iOffsetX = 0, const int iOffsetY = 0, const bool bSelect = true);

	void __fastcall SaveSelectedToStream(TStream *AStream);

	void __fastcall DoModified(const UnicodeString &sDescription);

	void __fastcall ClearModifications(void);

	// увеличивает или уменьшает зум
	void __fastcall DoZoom(const int iValue);

	void ParseTree(TTree *ATree, const UnicodeString &sFileName);

	// инспектор свойств
	TLMDDesignObjects *FLMDDesignObjects;
	TLMDPropertyInspector *FLMDPropertyInspector;
	TListBox * FListBoxPayments;

	UnicodeString m_sOpenedFileName;

	UnicodeString m_sTempClipboardFile;

	int g_i_history_count;

	TUndoQueue m_UndoQueue;

	TUndoQueue::iterator m_undo_it;

	std::auto_ptr<TMemoryStream>m_ClipboardStreamPtr;

	std::auto_ptr<TMemoryStream>m_NewProjectStreamPtr;

	bool g_b_allow_history_save;
	bool g_b_modified;

	bool g_b_resizing_flag;
	bool g_b_moving_flag;

	// по этой точке смотрим, было ли смещение при движении мышкой
	TPoint gMovePt;

	// тип класса, который будет создаваться на плане
	TPolygonShapeClass g_ShapeClass;

	void DoSetZoom(const int iZoom); /* overload */
	void DoSetZoom(const double dZoom);

protected:

	virtual void __fastcall WndProc(Messages::TMessage &Message);

public: // User declarations

	__fastcall TFrameTreeBase(TComponent* Owner);
	virtual __fastcall ~TFrameTreeBase();

	TNotifyEvent OnProjectSaved;

	// если дерево изменено, будет вызвано это событие
	TNotifyEvent OnTreeBaseModified;

	// проверяет, были ли изменения
	const bool __fastcall CheckModified(void);

	// закрывает все без изменения
	void __fastcall DoCloseAll(void);

	// возвращает название файла проекта
	UnicodeString __fastcall GetFileName();

	// если имя файла пустое, будет вызван диалог для выбора имени
	void __fastcall LoadProject(const UnicodeString &sFileName);

	const bool IsModified(void) {
		return g_b_modified;
	}

	// внешнее изменение
	void __fastcall ExternModified();

	// устанавливает величину зума от 10 до 1000
	void SetFixedZoom(const int iZoom); /* overload */

	// устанавливает величину зума от 10 до 1000
	void SetFixedZoom(const double dZoom);

};

#endif
