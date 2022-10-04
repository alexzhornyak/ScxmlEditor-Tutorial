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

#ifndef UnitSyntaxEditorH
#define UnitSyntaxEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <LMDSedFindDialog.hpp>
#include "LMDSedView.hpp"
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <StdActns.hpp>
#include <ToolWin.hpp>
//---------------------------------------------------------------------------

class TSyntaxFunctionsList;

namespace Editviewex {

	class TLMDEditView : public Lmdsedview::TLMDEditView
	{
	protected:

		MESSAGE void __fastcall WMKillFocus(Messages::TWMKillFocus &Msg);

	public:
		__fastcall virtual TLMDEditView(Classes::TComponent* AOwner)
			: Lmdsedview::TLMDEditView(AOwner) {}

		BEGIN_MESSAGE_MAP
		  MESSAGE_HANDLER(WM_KILLFOCUS, TWMKillFocus, WMKillFocus);
		END_MESSAGE_MAP(Lmdsedview::TLMDEditView);

	};
}

#define TLMDEditView	Editviewex::TLMDEditView

class TFrameSyntaxEditor : public TFrame
{
__published:	// IDE-managed Components
	TLMDEditView *EditView;
	TImageList *ImageList1;
	TPopupMenu *PopupMenu1;
	TMenuItem *Copy1;
	TMenuItem *Paste1;
	TMenuItem *Cut1;
	TMenuItem *Delete1;
	TMenuItem *N1;
	TMenuItem *Search1;
	TMenuItem *Replace1;
	TMenuItem *Searchnext1;
	TMenuItem *N2;
	TMenuItem *Selectall1;
	TMenuItem *N3;
	TMenuItem *Undo1;
	TMenuItem *Redo1;
	TToolBar *ToolBar1;
	TMainMenu *MainMenu1;
	TMenuItem *Fold1;
	TActionList *ActionsSyntax;
	TAction *actBmkSet_0;
	TAction *actBmkSet_9;
	TAction *actBmkSet_8;
	TAction *actBmkSet_7;
	TAction *actBmkSet_6;
	TAction *actBmkSet_5;
	TEditUndo *actUndo;
	TEditUndo *actRedo;
	TAction *actBreakSet;
	TAction *actBreakDeleteLast;
	TAction *actBreakToggleLast;
	TAction *actBmkSet_4;
	TAction *actToggleShowLineEnds;
	TAction *actToggleShowTabs;
	TAction *actBreakDeleteAll;
	TAction *actNoWrap;
	TAction *actWrapToWindow;
	TAction *actWrapToRuler;
	TAction *actShowLog;
	TAction *actBmkSet;
	TAction *actBmkDeleteLast;
	TAction *actBmkDeleteAll;
	TAction *actToggleUseTabs;
	TAction *actToggleShowSpaces;
	TAction *actToggleAutoIndent;
	TAction *actGoToLine;
	TAction *actToggleSplit;
	TAction *actFoldTops;
	TAction *actUnfoldTops;
	TAction *actFoldAll;
	TAction *actUnfoldAll;
	TAction *actFoldCurrentTop;
	TAction *actFoldAllInCurrent;
	TAction *actFoldTopsInCurrent;
	TAction *actUnfoldTopsInCurrent;
	TAction *actUnfoldAllInCurrent;
	TAction *actSearch;
	TAction *actReplace;
	TAction *actBmkSet_1;
	TAction *actBmkSet_2;
	TAction *actBmkSet_3;
	TAction *actBmkGoto_1;
	TAction *actBmkGoto_2;
	TAction *actBmkGoto_3;
	TAction *actBmkGoto_4;
	TAction *actBmkGoto_5;
	TAction *actBmkGoto_6;
	TAction *actBmkGoto_7;
	TAction *actBmkGoto_8;
	TAction *actBmkGoto_9;
	TAction *actBmkGoto_0;
	TAction *actCopy;
	TAction *actPaste;
	TAction *actCut;
	TAction *actRefreshSchemes;
	TAction *actBreakToggleHere;
	TAction *actSearchNext;
	TAction *actToggleReadOnly;
	TAction *actExit;
	TAction *actKeybindings;
	TAction *actPrint;
	TAction *actPrintPreview;
	TAction *actPrinterSetup;
	TAction *actPageSetup;
	TAction *actLoadEncoded;
	TAction *actSaveEncoded;
	TAction *actToggleCursorBound;
	TAction *actToggleShowRuler;
	TAction *actFont;
	TMenuItem *Edit1;
	TMenuItem *Cut2;
	TMenuItem *Copy2;
	TMenuItem *Paste2;
	TMenuItem *Foldalltopfolds1;
	TMenuItem *Unfoldalltopfolds1;
	TMenuItem *Foldall1;
	TMenuItem *actFoldCurrentTop1;
	TMenuItem *Foldallhere1;
	TMenuItem *Foldallhere2;
	TMenuItem *Unfoldtopshere1;
	TMenuItem *Unfoldallhere1;
	TMenuItem *Settings1;
	TMenuItem *Showtabs1;
	TMenuItem *Nowrap1;
	TMenuItem *Wraptowindow1;
	TMenuItem *Wraptoruler1;
	TMenuItem *Wrap1;
	TMenuItem *Usetabs1;
	TMenuItem *Showwhitespaces1;
	TMenuItem *Autoindent1;
	TMenuItem *Cursorboundtochars1;
	TMenuItem *Showruler1;
	TMenuItem *Showlineends1;
	TMenuItem *SyntaxScheme;
	TAction *actComment;
	TMenuItem *Commentselected1;
	TMenuItem *Commentselected2;
	TAction *actUnComment;
	TMenuItem *Uncommentselected1;
	TMenuItem *Uncommentselected2;
	TMenuItem *MenuEncoding;
	TMenuItem *Font1;
	TMenuItem *MenuFontSizeCustom;
	TMenuItem *MenuFontSizeSeparators;
	TMenuItem *MenuFontSize8;
	TMenuItem *MenuFontSize10;
	TMenuItem *MenuFontSize12;
	TMenuItem *MenuFontSize14;
	TMenuItem *MenuFontSize16;
	TMenuItem *MenuFontSize18;
	TMenuItem *MenuFontSize20;
	TMenuItem *MenuFontSize24;
	TMenuItem *MenuFontSize36;
	TMenuItem *MenuFontSize48;
	TMenuItem *MenuFile;
	TFileOpen *actLoad;
	TFileSaveAs *actSaveAs;
	TMenuItem *Open1;
	TMenuItem *SaveAs1;
	void __fastcall ActionCopyExecute(TObject *Sender);
	void __fastcall ActionCopyUpdate(TObject *Sender);
	void __fastcall ActionSearchExecute(TObject *Sender);
	void __fastcall ActionReplaceExecute(TObject *Sender);
	void __fastcall ActionSearchNextExecute(TObject *Sender);
	void __fastcall ActionSearchNextUpdate(TObject *Sender);
	void __fastcall ActionPasteExecute(TObject *Sender);
	void __fastcall ActionPasteUpdate(TObject *Sender);
	void __fastcall ActionDeleteExecute(TObject *Sender);
	void __fastcall ActionUndoExecute(TObject *Sender);
	void __fastcall ActionUndoUpdate(TObject *Sender);
	void __fastcall ActionRedoExecute(TObject *Sender);
	void __fastcall ActionRedoUpdate(TObject *Sender);
	void __fastcall ActionCutExecute(TObject *Sender);
	void __fastcall ActionCutUpdate(TObject *Sender);
	void __fastcall ActionSelectAllExecute(TObject *Sender);
	void __fastcall actFoldTopsExecute(TObject *Sender);
	void __fastcall actUnfoldTopsExecute(TObject *Sender);
	void __fastcall actFoldAllExecute(TObject *Sender);
	void __fastcall actUnfoldAllExecute(TObject *Sender);
	void __fastcall actFoldCurrentTopExecute(TObject *Sender);
	void __fastcall actFoldAllInCurrentExecute(TObject *Sender);
	void __fastcall actToggleShowLineEndsExecute(TObject *Sender);
	void __fastcall actNoWrapExecute(TObject *Sender);
	void __fastcall actToggleUseTabsExecute(TObject *Sender);
	void __fastcall actToggleAutoIndentExecute(TObject *Sender);
	void __fastcall actToggleCursorBoundExecute(TObject *Sender);
	void __fastcall actToggleShowRulerExecute(TObject *Sender);
	void __fastcall actToggleShowLineEndsUpdate(TObject *Sender);
	void __fastcall actToggleShowTabsUpdate(TObject *Sender);
	void __fastcall actToggleShowRulerUpdate(TObject *Sender);
	void __fastcall actToggleCursorBoundUpdate(TObject *Sender);
	void __fastcall actToggleUseTabsUpdate(TObject *Sender);
	void __fastcall actNoWrapUpdate(TObject *Sender);
	void __fastcall actWrapToWindowUpdate(TObject *Sender);
	void __fastcall actWrapToRulerUpdate(TObject *Sender);
	void __fastcall actToggleAutoIndentUpdate(TObject *Sender);
	void __fastcall actToggleShowSpacesUpdate(TObject *Sender);
	void __fastcall SyntaxSchemeClick(TObject *Sender);
	void __fastcall actCommentExecute(TObject *Sender);
	void __fastcall actUnCommentExecute(TObject *Sender);
	void __fastcall EditViewKeyPressW(TObject *Sender, wchar_t &Key);
	void __fastcall EditViewCompletionReplace(TObject *Sender, TStrings *Items, int ItemIndex, TPoint &CursorPoint, TLMDEditReplaceCompletionCause Cause,
          bool &Handled);
	void __fastcall EditViewKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall MenuEncodingClick(TObject *Sender);
	void __fastcall EditViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall EditViewMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled);
	void __fastcall Font1Click(TObject *Sender);
	void __fastcall MenuFontSizeCustomClick(TObject *Sender);
	void __fastcall actLoadAccept(TObject *Sender);
	void __fastcall actSaveAsAccept(TObject *Sender);
	void __fastcall actLoadBeforeExecute(TObject *Sender);
	void __fastcall actSaveAsBeforeExecute(TObject *Sender);
	void __fastcall EditViewGutterClick(TLMDCustomEditView *AView, const TLMDGutterClickParams &AParams, bool &Handled);
private:	// User declarations

	wchar_t FLastPressedKey;
	wchar_t FLastKeyDown;

	TLMDEditSearchDlgOptions FSearchOptions;

	void __fastcall ShowSearchNotFound();
	TLMDSpecialCharKinds __fastcall GetShowSpecKinds(TAction* Act);

	int FListBoxWidth;
	int FListBoxHeight;

	// âîçìîæíîñòü ïåðåîïðåäåëèòü ðàçðåøåíèå ôàéëà äëÿ ñõåìû
	// ÎÏÅÐÀÖÈÈ Ñ ÐÀÑØÈÐÅÍÈÅÌ 'ÁÅÇ ÒÎ×ÊÈ Â ÍÀ×ÀËÅ' !!!
	// Íàïðììåð: 'xml'
	UnicodeString FSyntaxFileExtension;
	UnicodeString __fastcall GetSyntaxFileExtenstion(void);

	TLMDEditReplaceForCompletionResultEvent FOnAfterCompletionReplace;

protected:

	TSyntaxFunctionsList *FSyntaxFunctionsList;

	virtual void __fastcall OnSelectSyntaxScheme(TObject *Sender);

	virtual void __fastcall OnEncodingClick(TObject *Sender);

public:		// User declarations

	__fastcall TFrameSyntaxEditor(TComponent* Owner);
	__fastcall virtual ~TFrameSyntaxEditor();

	void __fastcall DestroySyntaxFunctionList(void);

	__property UnicodeString SyntaxFileExtenstion = { read=GetSyntaxFileExtenstion, write=FSyntaxFileExtension };
	__property TLMDEditReplaceForCompletionResultEvent OnAfterCompletionReplace = { read=FOnAfterCompletionReplace, write=FOnAfterCompletionReplace };
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameSyntaxEditor *FrameSyntaxEditor;
//---------------------------------------------------------------------------
#endif
