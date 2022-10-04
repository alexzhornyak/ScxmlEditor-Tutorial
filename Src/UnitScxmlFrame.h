//---------------------------------------------------------------------------

#ifndef UnitScxmlFrameH
#define UnitScxmlFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvToolBtn.hpp"
#include "TeeNavigator.hpp"
#include "TeeProcs.hpp"
#include "TeeTree.hpp"
#include "TreeNavigator.hpp"
#include <ActnList.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <StdActns.hpp>
//---------------------------------------------------------------------------
class TFrameScxmlBase : public TFrame
{
__published:	// IDE-managed Components
	TActionList *ActionList1;
	TFileSaveAs *ActionSaveTreeAs;
	TAction *ActionClose;
	TAction *ActionZoomIn;
	TAction *ActionZoomOut;
	TAction *ActionUndo;
	TAction *ActionRedo;
	TAction *ActionLoadTree;
	TAction *ActionSaveTree;
	TFileSaveAs *FileSaveAsPicture;
	TAction *ActionPrintPreview;
	TAction *ActionCopySelected;
	TAction *ActionPasteSelected;
	TAction *ActionDeleteSelected;
	TAction *ActionEditSelected;
	TAction *ActionBringToFront;
	TAction *ActionSendToBack;
	TFileOpen *FileMerge;
	TAction *ActionCheckDesign;
	TAction *ActionEditConnection;
	TAction *ActionEditDSP;
	TAction *ActionEditLevels;
	TAction *ActionNewProject;
	TAction *ActionTesterSync;
	TAction *ActionFindElement;
	TAction *ActionUpdateColors;
	TAction *ActionZoomToWidth;
	TAction *ActionZoomToHeight;
	TAction *ActionZoomToFitAll;
	TAction *ActionZoom100;
	TAction *ActionZoomInCenter;
	TAction *ActionTestingEnabled;
	TPanel *Panel2;
	TTreeRuler *TreeRuler1;
	TTreeRuler *TreeRuler2;
	TTree *TreeBase;
	TTreeNodeShape *TreeNodeShape1;
	TTreeNodeShape *TreeNodeShape2;
	TPanel *PanelSideTools;
	TAdvToolButton *AdvToolButton1;
	TAdvToolButton *AdvToolButton2;
	TAdvToolButton *AdvToolButton3;
	TAdvToolButton *AdvToolButton4;
	TAdvToolButton *AdvToolButton5;
	TAdvToolButton *AdvToolButton6;
	TAdvToolButton *AdvToolButton7;
	TAdvToolButton *AdvToolButton8;
	TAdvToolButton *AdvToolButton9;
	TAdvToolButton *AdvToolButton10;
	TSpeedButton *BtnTriggersEnabled;
	TSpeedButton *BtnTestingEnabled;
	TPanel *PanelTools;
	TCheckBox *CheckDesignMode;
	TTreePageNavigator *TreePageNavigator1;
	TPanel *PanelButtons;
	TSpeedButton *SpeedBtnConnection;
	TBitBtn *BitBtnTreeNodeShape;
	TBitBtn *BitBtnEventShape;
	TBitBtn *BitBtnPluginShape;
	TBitBtn *BitBtnInputChannel;
	TBitBtn *BitBtnNetwork;
	TPanel *Panel4;
	TLabel *LabelZoom;
	TTrackBar *TrackBarZoom;
	TPopupMenu *PopupMenuTree;
	TMenuItem *MenuEdit;
	TMenuItem *Editstate1;
	TMenuItem *N1;
	TMenuItem *MenuEditConnection;
	TMenuItem *Editlevels1;
	TMenuItem *N19;
	TMenuItem *MenuCopy;
	TMenuItem *MenuPaste;
	TMenuItem *MenuDelete;
	TMenuItem *N20;
	TMenuItem *N26;
	TMenuItem *N27;
	TMenuItem *N3;
	TMenuItem *N28;
	TMenuItem *N21;
	TMenuItem *N22;
	TMenuItem *N2;
	TMenuItem *Updatecolors1;
	TPopupMenu *PopupUndoRedo;
private:	// User declarations
public:		// User declarations
	__fastcall TFrameScxmlBase(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameScxmlBase *FrameScxmlBase;
//---------------------------------------------------------------------------
#endif
