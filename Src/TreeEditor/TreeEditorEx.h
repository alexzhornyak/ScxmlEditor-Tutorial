/***********************************************************************************
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
***************************************************************************************/

#ifndef TreeEditorExH
#define TreeEditorExH
//---------------------------------------------------------------------------

#include <vector>
#include <deque>
#include <memory>
#include <map>

#include <AppEvnts.hpp>

#undef DrawText

#include "TeeTree.hpp"
#include "TreeEdNew.hpp"

class TTreeClipboardEx;
class TScxmlBaseShape;

typedef std::pair<UnicodeString/*Description*/, TMemoryStream*>TUndoPair;
typedef std::deque<TUndoPair>TUndoQueue;

typedef std::deque< std::pair<UnicodeString/*Description*/, GUID*> > TSelectionQueue;

typedef enum {pptLeft, pptTop, pptRight, pptBottom, pptMAXSIZE} TPointPinType;

static const UnicodeString g_PointPins[pptMAXSIZE] = {
	L"Left", L"Top", L"Right", L"Bottom"
};

#define SCROLL_CHART_IN_VIEW			true
#define DO_NOT_SCROLL_CHART_IN_VIEW		false

struct TTemporarySelectedRectangle {
	bool Enabled;
	bool Visible;
	TRect Bounds;
	UnicodeString Text;

	void Clear() {
		Enabled = false;
		Bounds = TRect(0, 0, 0, 0);
		Visible = false;
		Text = L"";
	}

	TTemporarySelectedRectangle(void) {
		Clear();
	}
};

class TTreeEditorEx : public TTreeEditorNew
{
__published:
	void __fastcall Cut1Click(System::TObject* Sender);
	void __fastcall Copy1Click(System::TObject* Sender);
	void __fastcall Paste1Click(System::TObject* Sender);
	void __fastcall PopupNodePopup(System::TObject* Sender);
	void __fastcall Edit4Click(System::TObject* Sender);
	void __fastcall Curve1Click(System::TObject* Sender);
	void __fastcall OnMenuChangeConnectionType(System::TObject* Sender);
	virtual void __fastcall ActionAlignToLeftExecute(System::TObject* Sender);
	virtual void __fastcall ActionAlignToRightExecute(System::TObject* Sender);
	virtual void __fastcall ActionAlignToTopExecute(System::TObject* Sender);
	virtual void __fastcall ActionAlignToBottomExecute(System::TObject* Sender);
	virtual void __fastcall ActionAlignHorCenterExecute(System::TObject* Sender);
	virtual void __fastcall ActionAlignVertCenterExecute(System::TObject* Sender);

	virtual void __fastcall ActionConnAdjustSidesExecute(System::TObject* Sender);
	virtual void __fastcall NodeTreeKeyDown(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift);

	virtual void __fastcall Copyaspicture1Click(System::TObject* Sender);

private:

	TMenuItem *FMenuEditorUndo;
	TMenuItem *FMenuEditorSelectionUndo;

	TMenuItem *FMenuEditorRestoreFromCache;
	TAction *FActionEditorRestoreFromCache;

	TMenuItem *FMenuClone;

	std::auto_ptr<TMemoryStream> FIntermediateUndoStreamPtr;

	TUndoQueue m_UndoQueue;
	TUndoQueue::iterator m_undo_it;

	TSelectionQueue::iterator m_selection_it;
	TSelectionQueue m_SelectionQueue;
	bool m_LockAddSelectionUndo;
	void __fastcall CheckForMissedSelectionElements();

	bool FMovingFlag;
	bool FResizingFlag;
	// если мышь не нажата, то AMouseButton==-1
	void __fastcall ShapeMoveResizeModified(const System::Word AKey, const int AMouseButton, const Classes::TShiftState AShift);
	bool FWasConnSelectedOnDown;
	TTreeConnection *FOnDownSelectedConn;
	TConnectionPointArray FConnSelectedPoints;

	TShiftState FKeyboardShift;
	System::Word FKeyboardKey;

	TShiftState  FMouseShift;

	TForm *FFullScreenForm;
	void __fastcall OnFullScreenFormCloseQuery(TObject * Sender, bool&CanClose);

	void __fastcall DoSetBookmark(TSpeedButton *ABookmarkBtn, TScxmlBaseShape *ASetShape);

	// для запоминания смещения при нажатии MouseDown
	int FHorizOffsetFix;
	int FVertOffsetFix;

	std::auto_ptr<TApplicationEvents> FAppEventsPtr;

	/* Флаги (не забываем добавлять в ClearFlags) */
	bool FTreeEnsureSelectedInView;
	bool FTreeInitConnectionFlag;
	UnicodeString FTreeUndoDescriptionFlag;
	bool FTreeInvalidateFlag;

	TTemporarySelectedRectangle FTemporarySelectedRectangle;

	// вызываем принудительно перерисовку, которая уберет временное выделение
	void __fastcall ForceResetTemporarySelected(void);

	void __fastcall LeaveEditAndEnterNodeTree(void);

	void __fastcall AddMenuUndoItems(TMenuItem *AParentItem);

	void __fastcall AddMenuSelectionItems(TMenuItem *AParentItem);

	void __fastcall ButtonUndoRedoDropDownClick(TObject *Sender);
	void __fastcall ButtonSelectionDropDownClick(TObject *Sender);

	void __fastcall OnEnterExitPanelTree(TObject *Sender);

	void __fastcall DoInvalidateTrees();

	void __fastcall DoInvalidateFocusedTheTree(void);

	void __fastcall OnMenuRestoreFromCache(TObject *Sender);
	void __fastcall OnActionRestoreFromCacheUpdate(TObject *Sender);

protected:

	TTreeClipboardEx *FCliboardEditorEx;

	std::vector<TSpeedButton *> FBookmarkButtons;

   	// особый флаг для блокировки очистки Undo
	bool FLockClearUndo;

	virtual void __fastcall OnActivateTreeEditor(TObject *Sender);
	virtual void __fastcall OnDectivateTreeEditor(TObject *Sender);

	virtual void __fastcall DoUndo(void);
	virtual void __fastcall DoUndoSelection(void);
	virtual void __fastcall DoPaste(bool UseCursorPos);
	virtual void __fastcall DoCut(void);
	const unsigned int FMaxHistoryCount;
	void __fastcall AddUndo(const UnicodeString &sDescription, TMemoryStream *ATreeStream = NULL, bool bSaveCache = true);
	void __fastcall AddSelectionUndo(const UnicodeString &ADescription, GUID* AGuid);
	void __fastcall ExecuteUndo(System::TObject* Sender);
	void __fastcall ExecuteRedo(System::TObject* Sender);
	void __fastcall ExecuteUndoSelection(System::TObject* Sender);
	void __fastcall ExecuteRedoSelection(System::TObject* Sender);
	void __fastcall ClearAllUndoRedo(void);
	void __fastcall UndoRedoClick(TObject *Sender);
	void __fastcall UndoRedoSelectionClick(TObject *Sender);
	void __fastcall MoveConnectionPoints(TTreeConnection *AConnection, const TPointPinType AMoveType);
	virtual void __fastcall OnMenuClone(TObject *Sender);

	virtual void __fastcall EditorOnShapesAligned(TObject *Sender);
	virtual void __fastcall AdjustConnectionsOfSelectedShapes();

	void __fastcall MenuHideWindow(const TTreeEditWindows HideWindows, const TTreeEditWindow AOption, TMenuItem *AMenu, TNotifyEvent AProc);
	void __fastcall TreeHideEditorPanels(const TTreeEditWindows HideWindows);
	TTreeEditWindows __fastcall GetHideWindowsStatus(void);
	virtual void __fastcall DoOnWheelZoom(TObject *Sender);
	virtual void __fastcall OnShapeMoveResized();
	virtual void __fastcall OnConnectionReconnected(TTreeConnection *AConnection);
	virtual void __fastcall OnConnectionStyleChanged(TTreeConnection *AConnection);
	virtual void __fastcall DoSelectAllShapes(); // возможно это у потомков что-то длительное

	virtual void __fastcall DoTheTreeSelectShape(Teetree::TTreeNodeShape* Sender);
	virtual void __fastcall DoTheTreeSelectConnection(Teetree::TTreeConnection* Sender);

	virtual	void __fastcall DoOnEditorApplicationEventsModalEnd(TObject *Sender);
	virtual	void __fastcall DoOnEditorApplicationEventsIdle(TObject *Sender, bool &Done);
	virtual void __fastcall ClearFlags(void); // уберет все флаги в OnIdle

    virtual void __fastcall DeleteTemporaryShapes(void) {} // удаляет временные фигуры

	/* BOOKMARKS */
	void __fastcall SetBookmark(TSpeedButton *ABookmarkBtn, TScxmlBaseShape *ASetShape);
	inline void __fastcall ResetBookmark(TSpeedButton *ABookmarkBtn) { SetBookmark(ABookmarkBtn, NULL); }
	void __fastcall SelectShapeFromBookmark(TSpeedButton *ABookmarkBtn);
	const UnicodeString GetBookmarkHelp(TSpeedButton *ABookmarkButton, const bool bMultiline = true);
	virtual void __fastcall OnBookmarkBtnClick(TObject *Sender);
	__classmethod virtual TMetaClass * __fastcall GetBookmarkClass() { return __classid(TTreeNodeShape); }

	/* inherited functions */
	__classmethod virtual TTreeClass __fastcall TeeTreeClass();
	virtual void __fastcall TeeModified(bool YesNo = true, int ACat = 0x0, const System::UnicodeString ADescription = L"");
	DYNAMIC void __fastcall SetNewTreeSettings(void);
	virtual void __fastcall FillNodeTree(void);
	virtual void __fastcall ResetTree(void);
	virtual void __fastcall TheTreeKeyDown(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift);
	virtual void __fastcall TheTreeKeyUp(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift);
	virtual void __fastcall TheTreeMovingShape(Teetree::TTreeNodeShape* Sender, int &DeltaX, int &DeltaY);
	virtual void __fastcall TheTreeResizingShape(Teetree::TTreeNodeShape* Sender, Teetree::TTreeShapeHandle ACorner, int &DeltaX, int &DeltaY);
	virtual void __fastcall TheTreeMouseDown(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall TheTreeMouseUp(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall TheTreeClickConnection(Teetree::TTreeConnection* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall TheTreeSelectShape(Teetree::TTreeNodeShape* Sender);
	virtual void __fastcall TheTreeSelectConnection(Teetree::TTreeConnection* Sender);
	virtual void __fastcall TheTreeAfterDraw(System::TObject* Sender);
	virtual void __fastcall TheTreeExit(System::TObject* Sender);
	/* end of inherited */

	virtual void __fastcall TheTreeDeletingConnection(Teetree::TTreeConnection* Sender, bool &AllowDelete) {}

	/* Fill Node Tree Extra */
	virtual void __fastcall DoAddFillNodeChilds(TTreeNodeShape *AToNode, TNodeShapeList * AFromNodes);
	virtual void __fastcall DoAddFillNodePicture(TTreeNodeShape *AToShape, TCustomTreeElement* AFromElement);
	virtual TTreeNodeShape * __fastcall DoProcessAddFillNode(TTreeNodeShape *AToNode, TCustomTreeElement * AFromElement);
	virtual TCustomTreeElement * __fastcall NodeTreeShapeToTreeElement(TTreeNodeShape *ANodeTreeShape);

	virtual void __fastcall OnEditNodeTreeSearchChange(TObject *Sender);
	virtual void __fastcall OnEditNodeTreeKeyPress(System::TObject* Sender, System::WideChar &Key);
	virtual void __fastcall OnEditNodeTreeKeyDown(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift);

	virtual void __fastcall NodeTreeAfterDraw(System::TObject* Sender);
	virtual void __fastcall NodeTreeOnEnter(System::TObject* Sender);
	virtual void __fastcall NodeTreeOnExit(System::TObject* Sender);

public:
	__fastcall TTreeEditorEx(TComponent *AOwner);
	virtual __fastcall ~TTreeEditorEx();

	const TTemporarySelectedRectangle &GetTemporarySelectedRectangle(void) const {
		return FTemporarySelectedRectangle;
	}

	void SetTemporarySelectedRectangle(const TTemporarySelectedRectangle &ATemporarySelectedRectangle) {
		FTemporarySelectedRectangle = ATemporarySelectedRectangle;
	}

	UnicodeString __fastcall GetModifiedCacheFile(void) const;

	TNotifyEvent OnTreeEditorModified;
	virtual bool __fastcall IsModified(void);

	virtual void __fastcall ZoomToFitSelection(const TZoomFitType AZoomFitType, bool bFromZero);

	virtual void __fastcall ZoomToFit(const Types::TRect &ABounds, TZoomFitType AZoomFitType, bool bFromZero) /* overload */;

	virtual void __fastcall TreeZoomWasChanged(void); /* inherited */

	inline void __fastcall Paste(bool UseCursorPos) { DoPaste(UseCursorPos); }
	inline void __fastcall Cut(void) { DoCut(); }

	virtual void __fastcall CenterInView(TTreeNodeShape *AShape);

	virtual void __fastcall SelectConnection(TTreeConnection *AConnection, const bool bScrollInView = SCROLL_CHART_IN_VIEW);
	virtual void __fastcall SelectShape(TTreeNodeShape *AShape, const bool bScrollInView = SCROLL_CHART_IN_VIEW);
	void __fastcall SelectTemporaryRectangle(const TRect &ABounds, const UnicodeString sMsg = L"");

	virtual bool __fastcall EnsureInView(TTreeNodeShape *AShape, const TRect &ABounds);
	virtual bool __fastcall EnsureInView(TTreeNodeShape *AShape);
	virtual bool __fastcall EnsureInView(const TPoint &APoint, const TRect &ABounds);
	virtual void __fastcall EnsureSelectedInView() { FTreeEnsureSelectedInView=true; }

	void __fastcall InvalidateTheTreeWithTemp(void);
	void __fastcall RefreshTheTreeWithTemp(void);

	void __fastcall EnterFullScreenMode(void);
	void __fastcall ExitFullScreenMode(void);
	void __fastcall ToggleFullScreenMode(void);

	__property TTreeEditWindows HideWindowsStatus = {read=GetHideWindowsStatus};

__published:

};

/* ВНИМАНИЕ! Есть баг на большом количестве элементов, когда Shape->Image->Assign(Image) дает AccessViolation */
/* поэтому приходится делать очередной костыль через TreeImagePool */
extern std::map<TMetaClass*, int> g_MapImageIndexes;

#endif
