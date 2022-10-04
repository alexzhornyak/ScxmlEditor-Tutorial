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

#include <vcl.h>
#pragma hdrstop

#include "UnitSyntaxEditor.h"

#include <boost/regex.hpp>

#include <StrUtils.hpp>
#include "Log4cpp_VCL.hpp"
#include "UnitSyntaxFunctionList.h"
#include "UnitSettings.h"
#include "UnitHTMLUtils.h"
#include "UnitSyntaxUtils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDSedView"
#pragma resource "*.dfm"
TFrameSyntaxEditor *FrameSyntaxEditor = NULL;

//---------------------------------------------------------------------------
//--------------------------- TLMDEditView ----------------------------------
//---------------------------------------------------------------------------
void __fastcall TLMDEditView::WMKillFocus(Messages::TWMKillFocus &Msg) {
	TFrameSyntaxEditor * AFrameSyntaxEditor = dynamic_cast<TFrameSyntaxEditor*>(this->Owner);
	if (AFrameSyntaxEditor) {
		AFrameSyntaxEditor->DestroySyntaxFunctionList();
	}
}

//---------------------------------------------------------------------------
//----------------------------- TFrameSyntaxEditor --------------------------
//---------------------------------------------------------------------------
__fastcall TFrameSyntaxEditor::TFrameSyntaxEditor(TComponent* Owner) : TFrame(Owner), FOnAfterCompletionReplace(NULL) {
	FLastPressedKey = 0;
	FLastKeyDown = 0;

	FListBoxWidth = 100;
	FListBoxHeight = 50;

	// должно быть пустым по умолчанию
	FSyntaxFileExtension = L"";

	if (SettingsData) {
		for (int i = 0; i < EditView->ControlCount; i++) {
			if (EditView->Controls[i]->ClassNameIs("TComplList")) {
				TCustomListBox * ACustomListBox = dynamic_cast<TCustomListBox*>(EditView->Controls[i]);
				if (ACustomListBox) {

					FListBoxWidth = ACustomListBox->Width;
					FListBoxHeight = ACustomListBox->Height;

					ACustomListBox->Width = SettingsData->TempRegistry->Values["CompletionList.Width"].ToIntDef(ACustomListBox->Width);
					ACustomListBox->Height = SettingsData->TempRegistry->Values["CompletionList.Height"].ToIntDef(ACustomListBox->Height);

				}
				break;
			}
		}
	}
}

//---------------------------------------------------------------------------
__fastcall TFrameSyntaxEditor::~TFrameSyntaxEditor() {
	if (SettingsData) {
		for (int i = 0; i < EditView->ControlCount; i++) {
			if (EditView->Controls[i]->ClassNameIs("TComplList")) {
				TCustomListBox * ACustomListBox = dynamic_cast<TCustomListBox*>(EditView->Controls[i]);
				if (ACustomListBox) {
					if (ACustomListBox->Width != FListBoxWidth) {
						SettingsData->TempRegistry->Values["CompletionList.Width"] = ACustomListBox->Width;
					}

					if (ACustomListBox->Height != FListBoxHeight) {
						SettingsData->TempRegistry->Values["CompletionList.Height"] = ACustomListBox->Height;
					}
				}
				break;
			}
		}
	}

	// уничтожает объект
	DestroySyntaxFunctionList();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ShowSearchNotFound() {
	TMsgDlgButtons Btns;
	Btns << mbOK;

	MessageDlg(UnicodeString().sprintf(L"Search string \"%s\" not found", EditView->SearchLastArgs.Search.c_str()), mtInformation, Btns,
		-1);
}

//---------------------------------------------------------------------------
TLMDSpecialCharKinds __fastcall TFrameSyntaxEditor::GetShowSpecKinds(TAction* Act) {
	TLMDSpecialCharKinds Result;

	if (Act == actToggleShowSpaces)
		Result << skSpace;
	else if (Act == actToggleShowTabs)
		Result << skTabTail << skTabHead;
	else if (Act == actToggleShowLineEnds)
		Result << skCR << skLF << skCRLF << skLineWrap;

	return Result;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionCopyExecute(TObject *Sender) {
	EditView->CopySelection();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionCopyUpdate(TObject *Sender) {
	SetPropValue(Sender, "Enabled", EditView->CanCopy);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionSearchExecute(TObject *Sender) {
	if (LMDEditExecFindDialog("Search text", EditView, FSearchOptions) == srNotFound)
		ShowSearchNotFound();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionReplaceExecute(TObject *Sender) {
	if (EditView->SelAvail)
		FSearchOptions.ReplaceAllBounds = rbSelection;
	else
		FSearchOptions.ReplaceAllBounds = rbAllText;

	TLMDEditSearchResult Res = LMDEditExecReplaceDialog("Replace text", EditView, FSearchOptions);

	if (Res == srNotFound) {
		ShowSearchNotFound();
	}
	else if ((Res == srFound) && (FSearchOptions.WasReplaceAllChosen) && (FSearchOptions.LastNumberOfReplacements > 0)) {
		TMsgDlgButtons btns;
		btns << mbOK;

		MessageDlg(UnicodeString().sprintf(L"Ok. Number of replacements: %d", FSearchOptions.LastNumberOfReplacements), mtInformation,
			btns, -1);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionSearchNextExecute(TObject *Sender) {
	bool Ok = true;

	switch(EditView->SearchState) {
	case stInSearch:
		Ok = EditView->SearchNext();
		break;
	case stInReplace:
		Ok = EditView->ReplaceNext();
		break;
	default:
		Beep();
	}

	if (!Ok)
		ShowSearchNotFound();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionSearchNextUpdate(TObject *Sender) {
	SetPropValue(Sender, "Enabled", !EditView->Document->ReadOnly || EditView->SearchState != stInReplace);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionPasteExecute(TObject *Sender) {
	EditView->Paste();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionPasteUpdate(TObject *Sender) {
	SetPropValue(Sender, "Enabled", EditView->CanPaste && !EditView->Document->ReadOnly);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionDeleteExecute(TObject *Sender) {
	EditView->DeleteSelected();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionUndoExecute(TObject *Sender) {
	EditView->Undo();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionUndoUpdate(TObject *Sender) {
	SetPropValue(Sender, "Enabled", EditView->CanUndo && !EditView->Document->ReadOnly);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionRedoExecute(TObject *Sender) {
	EditView->Redo();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionRedoUpdate(TObject *Sender) {
	SetPropValue(Sender, "Enabled", EditView->CanRedo && !EditView->Document->ReadOnly);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionCutExecute(TObject *Sender) {
	EditView->CutSelection();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionCutUpdate(TObject *Sender) {
	SetPropValue(Sender, "Enabled", EditView->CanCut && !EditView->Document->ReadOnly);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::ActionSelectAllExecute(TObject *Sender) {
	EditView->SelectAll();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actFoldTopsExecute(TObject *Sender) {
	EditView->FoldAllIn(-1, true);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actUnfoldTopsExecute(TObject *Sender) {
	EditView->UnfoldAllIn(-1, true);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actFoldAllExecute(TObject *Sender) {
	EditView->FoldAllIn(-1, false);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actUnfoldAllExecute(TObject *Sender) {
	EditView->UnfoldAllIn(-1, false);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actFoldCurrentTopExecute(TObject *Sender) {
	TLMDEditParserBase* Parser = EditView->Document->GetParser();

	int Ph = EditView->ScrollToPhysical(EditView->CursorPos.y);

	int Parent = Parser->GetParentLine(Ph);
	if (Parent == -1)
		Beep();
	else
		EditView->Fold(Parent, false);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actFoldAllInCurrentExecute(TObject *Sender) {
	TLMDEditParserBase* Parser = EditView->Document->GetParser();
	int Ph = EditView->ScrollToPhysical(EditView->CursorPos.y);

	int Current = Parser->GetParentLine(Ph);

	if (Sender == actFoldAllInCurrent)
		EditView->FoldAllIn(Current, false);
	else if (Sender == actFoldTopsInCurrent)
		EditView->FoldAllIn(Current, true);
	else if (Sender == actUnfoldAllInCurrent)
		EditView->UnfoldAllIn(Current, false);
	else if (Sender == actUnfoldTopsInCurrent)
		EditView->UnfoldAllIn(Current, true);
	else
		throw Exception("Undefined Action sender!");
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actToggleShowLineEndsExecute(TObject *Sender) {
	TAction* Act = dynamic_cast<TAction*>(Sender);
	if (Act) {
		Act->Checked = !Act->Checked;

		TLMDSpecialCharKinds Kinds = GetShowSpecKinds(Act);
		TLMDSpecialCharKinds Sets = EditView->SpecialChars->ShowCharKinds;

		if (Sets * Kinds == TLMDSpecialCharKinds())
			Sets = Sets + Kinds;
		else
			Sets = Sets - Kinds;

		EditView->SpecialChars->ShowCharKinds = Sets;
	}

}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actNoWrapExecute(TObject *Sender) {
	if (Sender == actNoWrap)
		EditView->WrapMode = wmNoWrap;
	else if (Sender == actWrapToRuler)
		EditView->WrapMode = wmWrapToRuler;
	else if (Sender == actWrapToWindow)
		EditView->WrapMode = wmWrapToWindow;
	else
		throw Exception("Unknown Wrap action");
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actToggleUseTabsExecute(TObject *Sender) {
	if (EditView->ViewSettings.Contains(vsUseTabs))
		EditView->ViewSettings = EditView->ViewSettings >> vsUseTabs;
	else
		EditView->ViewSettings = EditView->ViewSettings << vsUseTabs;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actToggleAutoIndentExecute(TObject *Sender) {
	if (EditView->ViewSettings.Contains(vsAutoIndent))
		EditView->ViewSettings = EditView->ViewSettings >> vsAutoIndent;
	else
		EditView->ViewSettings = EditView->ViewSettings << vsAutoIndent;
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actToggleCursorBoundExecute(TObject *Sender) {
	if (EditView->ViewSettings.Contains(vsCursorBoundToChars))
		EditView->ViewSettings = EditView->ViewSettings >> vsCursorBoundToChars;
	else
		EditView->ViewSettings = EditView->ViewSettings << vsCursorBoundToChars;
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actToggleShowRulerExecute(TObject *Sender) {
	if (EditView->ViewSettings.Contains(vsShowWrapRuler))
		EditView->ViewSettings = EditView->ViewSettings >> vsShowWrapRuler;
	else
		EditView->ViewSettings = EditView->ViewSettings << vsShowWrapRuler;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actToggleShowLineEndsUpdate(TObject *Sender) {
	actToggleShowLineEnds->Checked = (EditView->SpecialChars->ShowCharKinds * GetShowSpecKinds(actToggleShowLineEnds))
		!= TLMDSpecialCharKinds();
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actToggleShowTabsUpdate(TObject *Sender) {
	actToggleShowTabs->Checked = (EditView->SpecialChars->ShowCharKinds * GetShowSpecKinds(actToggleShowTabs)) != TLMDSpecialCharKinds();
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actToggleShowRulerUpdate(TObject *Sender) {
	actToggleShowRuler->Checked = EditView->ViewSettings.Contains(vsShowWrapRuler);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actToggleCursorBoundUpdate(TObject *Sender) {
	actToggleCursorBound->Checked = EditView->ViewSettings.Contains(vsCursorBoundToChars);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actToggleUseTabsUpdate(TObject *Sender) {
	actToggleUseTabs->Checked = EditView->ViewSettings.Contains(vsUseTabs);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actNoWrapUpdate(TObject *Sender) {
	actNoWrap->Checked = EditView->WrapMode == wmNoWrap;
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actWrapToWindowUpdate(TObject *Sender) {
	actWrapToWindow->Checked = EditView->WrapMode == wmWrapToWindow;
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actWrapToRulerUpdate(TObject *Sender) {
	actWrapToRuler->Checked = EditView->WrapMode == wmWrapToRuler;
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actToggleAutoIndentUpdate(TObject *Sender) {
	actToggleAutoIndent->Checked = EditView->ViewSettings.Contains(vsAutoIndent);
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditor::actToggleShowSpacesUpdate(TObject *Sender) {
	actToggleShowSpaces->Checked = (EditView->SpecialChars->ShowCharKinds * GetShowSpecKinds(actToggleShowSpaces)) != TLMDSpecialCharKinds
		();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::SyntaxSchemeClick(TObject *Sender) {
	if (EditView->Document) {
		SyntaxScheme->Clear();

		for (int i = 0; i < EditView->Document->SyntaxSchemes->Count; i++) {
			TMenuItem *AItem = new TMenuItem(this);
			AItem->Caption = EditView->Document->SyntaxSchemes->Items[i]->Name;
			AItem->Checked = EditView->Document->ActiveSyntaxScheme == EditView->Document->SyntaxSchemes->Items[i]->Name;
			AItem->OnClick = &OnSelectSyntaxScheme;

			SyntaxScheme->Add(AItem);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::OnSelectSyntaxScheme(TObject *Sender) {
	TMenuItem *AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && EditView->Document) {

		EditView->Document->ActiveSyntaxScheme = StripHotkey(AMenuItem->Caption);
		EditView->Document->ActiveColorScheme = EditView->Document->ColorSchemes->FindItem(EditView->Document->ActiveSyntaxScheme)
			? EditView->Document->ActiveSyntaxScheme : UnicodeString(L"default");

		bool bWasModified = EditView->Document->Modified;

		EditView->Document->Modified = true;
		// если уже был модифицирован, то необходимо вызвать обработчик изменения в любом случае
		if (bWasModified && EditView->Document->OnDocChanged) {
			EditView->Document->OnDocChanged(EditView->Document, TLMDDocStatusChanges() << dcModifiedPropertyChanged);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actCommentExecute(TObject *Sender) {
	if (EditView->Document) {
		try {
			EditView->Document->BeginUpdate();

			const int iStart = std::min(EditView->SelectionStart.y, EditView->SelectionEnd.y);
			const int iEnd = std::max(EditView->SelectionStart.y, EditView->SelectionEnd.y);

			UnicodeString sStartComment, sEndComment;
			Syntaxutils::GetSyntaxComments(EditView->Document->ActiveSyntaxScheme, sStartComment, sEndComment);

			for (int i = iStart; i <= iEnd; i++) {
				if (!sStartComment.IsEmpty()) {
					EditView->Document->Insert(EditView->Document->LineSegments[i].Start, sStartComment);

					if (!sEndComment.IsEmpty()) {
						EditView->Document->Insert(EditView->Document->LineSegments[i].Start + EditView->Document->LineSegments[i].Count,
							sEndComment);
					}
				}
			}
		}
		__finally {
			EditView->Document->EndUpdate();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actUnCommentExecute(TObject *Sender) {
	if (EditView->Document) {
		try {
			EditView->Document->BeginUpdate();

			const int iStart = std::min(EditView->SelectionStart.y, EditView->SelectionEnd.y);
			const int iEnd = std::max(EditView->SelectionStart.y, EditView->SelectionEnd.y);

			UnicodeString sStartComment, sEndComment;
			Syntaxutils::GetSyntaxComments(EditView->Document->ActiveSyntaxScheme, sStartComment, sEndComment);

			for (int i = iStart; i <= iEnd; i++) {
				if (!sStartComment.IsEmpty() && EditView->Document->Lines->Strings[i].TrimLeft().Pos(sStartComment) == 1) {
					EditView->Document->ReplacePatternFirst(EditView->Document->LineSegments[i].Start,
						EditView->Document->LineSegments[i].Count, sStartComment, "", TLMDEditSearchOptions());

					if (!sEndComment.IsEmpty()) {
						EditView->Document->ReplacePatternFirst
							(EditView->Document->LineSegments[i].Start + EditView->Document->LineSegments[i].Count,
							EditView->Document->LineSegments[i].Count, sEndComment, "", TLMDEditSearchOptions(),
							TLMDEditSearchDirection::sdBackward);
					}
				}
			}
		}
		__finally {
			EditView->Document->EndUpdate();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::EditViewKeyPressW(TObject *Sender, wchar_t &Key) {
	FLastPressedKey = Key;
#if 0
	int iKey = Key;
	WLOG_DEBUG(L"Last pressed key:[%d]:[%c]", iKey, Key);
#endif
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::EditViewCompletionReplace(TObject *Sender, TStrings *Items, int ItemIndex, TPoint &CursorPoint,
	TLMDEditReplaceCompletionCause Cause, bool &Handled) {

	TLMDEditView * ALMDEditView = dynamic_cast<TLMDEditView*>(Sender);
	if (ALMDEditView) {
		// запрещаем дополнение по разделителю слова
		const UnicodeString sWordSeparators = ALMDEditView->Document ? ALMDEditView->Document->WordSeparators : UnicodeString
			("[]();,'\"-+/\\:{} ");

#if 0
		WLOG_DEBUG(L"Last key down:[%d]:[%c] Last key press:[%d]:[%c]", (int)FLastKeyDown, FLastKeyDown, (int)FLastPressedKey,
			FLastPressedKey);
#endif

		Handled = sWordSeparators.Pos(FLastPressedKey);
	}

	if (FOnAfterCompletionReplace) {
		FOnAfterCompletionReplace(Sender, Items, ItemIndex, CursorPoint, Cause, Handled);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::EditViewKeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
	FLastKeyDown = Key;

#if 0 // !!! Key down - текущее значение клавишы без учета Shift
	int iKey = Key;
	WLOG_DEBUG(L"Last key down:[%d]:[%c] Shortcut:[%s]", iKey, Key, ShortCutToText(ShortCut(Key, Shift)).c_str());
#endif

	if (Key == VK_SPACE && Shift == (TShiftState() << ssCtrl << ssShift)) {

		if (EditView->Document) {
			try {
				if (!FSyntaxFunctionsList) {
					FSyntaxFunctionsList = new TSyntaxFunctionsList(NULL);
					FSyntaxFunctionsList->Visible = false;
					FSyntaxFunctionsList->Parent = this;
					FSyntaxFunctionsList->Width = 400;
					FSyntaxFunctionsList->HorizontalScroll = true;
					FSyntaxFunctionsList->WordWrap = true;
					FSyntaxFunctionsList->Height = abs(EditView->Font->Height) * 2;
				}

				UnicodeString Result = L"";
				UnicodeString SecondWord = L"";

				const int iOffset = EditView->CursorOffset;

				Lmdsedutils::TLMDSegment Seg = EditView->Document->FindNearWord(iOffset, -1, true);
				if (!LMDSegIsBad(Seg)) {
					if (EditView->CursorPosY < EditView->Document->LinesCount) {
						Lmdsedutils::TLMDSegment ALineSeg = EditView->Document->LineSegments[EditView->CursorPosY];
						if (!LMDSegIsBad(ALineSeg)) {
							TLMDSegment ATillCursor = LMDSegment(ALineSeg.Start, (Seg.Start + Seg.Count) - ALineSeg.Start, ALineSeg.Source);
							if (!LMDSegIsBad(ATillCursor)) {
								UnicodeString sKeyWord = LMDSegToString(ATillCursor);
								const int iPos = sKeyWord.LastDelimiter(L"() \t");
								if (iPos) {
									sKeyWord = sKeyWord.Delete(1, iPos);
								}
#if 0 // для глубокой отладки
								WLOG_DEBUG(L"Keyword=[%s]", sKeyWord.c_str());
#endif

								TAutoCompleteItem *AItem = SettingsData->AutoCompleteItems->IndexOfSyntaxScheme
									(EditView->Document->ActiveSyntaxScheme);
								if (AItem) {

									TAutoCompleteKeywordHintItem *AKeywordItem = AItem->KeywordHints->IndexOfKeyword(sKeyWord);
									if (!AKeywordItem) {
										const int iDotPos = sKeyWord.Pos(L".");
										if (iDotPos) {
											sKeyWord = sKeyWord.SubString(iDotPos + 1, sKeyWord.Length() - iDotPos + 1);
#if 0 // для глубокой отладки
											WLOG_DEBUG(L"Dot Keyword=[%s]", sKeyWord.c_str());
#endif
											AKeywordItem = AItem->KeywordHints->IndexOfKeyword(sKeyWord);
										}
									}
									if (AKeywordItem) {

										FSyntaxFunctionsList->Items->Clear();

										FSyntaxFunctionsList->Items->Add(UnicodeString().sprintf(L"%s %s (%s)", //
												Htmlutils::StrToHTMLColorStr(AKeywordItem->RetVal, clMaroon, false), //
												Htmlutils::StrToHTMLColorStr(AKeywordItem->Keyword, clBlack, true), //
												Htmlutils::StrToHTMLColorStr(StringReplace(AKeywordItem->Params, ";", "  ",
														TReplaceFlags() << rfReplaceAll), clBlue, false)));

										int iMax = 400;
										std::auto_ptr<TStringList>ADescriptionPtr(new TStringList());
										ADescriptionPtr->Text = AKeywordItem->Description;
										for (int i = 0; i < ADescriptionPtr->Count; i++) {
											const int iLineWidth = FSyntaxFunctionsList->Canvas->TextWidth(ADescriptionPtr->Strings[i]);
											if (iLineWidth > iMax) {
												iMax = iLineWidth;
											}
										}

										const int iScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
										const int iScrollHeight = GetSystemMetrics(SM_CXHSCROLL);

										FSyntaxFunctionsList->Width = iMax + iScrollWidth + 9;
										FSyntaxFunctionsList->Items->AddStrings(ADescriptionPtr.get());

										// FIX: LMD wrong line break
										Htmlutils::ChangeEmptyLinesToLineBreaks(FSyntaxFunctionsList->Items);

										int iListHeight = abs(EditView->Font->Height) + iScrollHeight;
										for (int i = 0; i < FSyntaxFunctionsList->Items->Count; i++) {
											iListHeight = iListHeight + abs(EditView->Font->Height);
										}

										FSyntaxFunctionsList->Height = iListHeight;

										TPoint APtScroll = EditView->OffsetToScrollPos(EditView->CursorOffset);
										EditView->ScrollToPos(APtScroll);

										bool OutOf;
										TRect ARect;
										ARect = EditView->ScrollPosToClient(APtScroll, OutOf);
										ARect.Top += abs(EditView->Font->Height);

										const int dr = ClientWidth - (ARect.Left + FSyntaxFunctionsList->Width);
										const int du = ARect.Top - FSyntaxFunctionsList->Height;
										const int db = ClientHeight - (ARect.Bottom + FSyntaxFunctionsList->Height);

										TPoint APtTopLeft(0, 0);

										const bool CrBottom = db >= du;
										APtTopLeft.y = CrBottom ? ARect.Bottom : (ARect.Top - FSyntaxFunctionsList->Height);

										const bool CrRight = dr > 0;
										APtTopLeft.x = CrRight ? ARect.Left : (ARect.Left - FSyntaxFunctionsList->Width);

										APtTopLeft = EditView->ClientToScreen(APtTopLeft);

										FSyntaxFunctionsList->Show(true, APtTopLeft.x, APtTopLeft.y);
										return;
									}

								}
							}
						}
					}
				}
			}
			catch(Exception * E) {
				LOG_ERROR(LOG_ERROR_MSG);
			}

		}
	}

	this->DestroySyntaxFunctionList();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::MenuEncodingClick(TObject *Sender) {
	if (EditView->Document) {
		MenuEncoding->Clear();
		TLMDEncodingDescrArray AEncodingArray = TLMDEncoding::GetDescriptions(true);

		int iRootIndex = 0;

		std::vector<TMenuItem*>AVecItems;

		for (int k = 0; k < AEncodingArray.Length; k++) {

			boost::wregex e(L"^([^\\s]*)[^\\(]*\\(*([^\\)]*)\\)*$");
			boost::wsmatch what;

			const std::wstring wstr = AEncodingArray[k].Description.c_str();
			if (boost::regex_match(wstr, what, e)) {
				if (what.size() == 3) {

					const UnicodeString sGroup = Trim(std::wstring(what[1]).c_str());
					const UnicodeString sName = Trim(std::wstring(what[2]).c_str());

					TMenuItem *AGroup = NULL;
					for (int i = 0; i < MenuEncoding->Count; i++) {
						const UnicodeString sItemGroup = StripHotkey(MenuEncoding->Items[i]->Caption);
						if (sItemGroup.Pos(sGroup) || sGroup.Pos(sItemGroup)) {
							AGroup = MenuEncoding->Items[i];
							break;
						}
					}

					if (!AGroup) {
						AGroup = new TMenuItem(MenuEncoding);
						AGroup->Caption = sGroup.Trim();
						MenuEncoding->Add(AGroup);
					}

					TMenuItem *AEncoding = new TMenuItem(AGroup);
					AEncoding->Caption = AEncodingArray[k].Description;
					AEncoding->Tag = AEncodingArray[k].CodePage;
					AGroup->Add(AEncoding);
					AEncoding->Checked = EditView->Document->CodePage == AEncodingArray[k].CodePage;
					AEncoding->OnClick = OnEncodingClick;

					std::auto_ptr<TLMDEncoding>ALMDEncodingPtr(TLMDEncoding::GetEncoding(AEncodingArray[k].CodePage));
					if (ALMDEncodingPtr.get()) {
						if (AEncodingArray[k].CodePage == 1251 || AEncodingArray[k].CodePage == 0 || AEncodingArray[k].CodePage == 65001) {
							TMenuItem *ARootMenu = new TMenuItem(MenuEncoding);
							ARootMenu->Caption = AEncodingArray[k].Description;
							ARootMenu->Tag = AEncodingArray[k].CodePage;
							ARootMenu->Checked = EditView->Document->CodePage == AEncodingArray[k].CodePage;
							ARootMenu->OnClick = OnEncodingClick;

							AVecItems.push_back(ARootMenu);
						}
					}
				}
			}
		}

		if (AVecItems.size()) {
			for (std::size_t i = 0; i < AVecItems.size(); i++) {
				MenuEncoding->Insert(i, AVecItems[i]);

				if (i == AVecItems.size() - 1) {
					MenuEncoding->InsertNewLineAfter(AVecItems[i]);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::OnEncodingClick(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem && EditView->Document) {
		std::auto_ptr<TMemoryStream>AMemoryStreamPtr(new TMemoryStream());
		EditView->Document->SaveToStream(AMemoryStreamPtr.get(), EditView->Document->CodePage);

		AMemoryStreamPtr->Position = 0;

		EditView->Document->CodePage = AMenuItem->Tag;
		EditView->Document->LoadFromStream(AMemoryStreamPtr.get(), AMenuItem->Tag);

		EditView->Document->Modified = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::EditViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	DestroySyntaxFunctionList();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::DestroySyntaxFunctionList(void) {
	if (FSyntaxFunctionsList) {
		delete FSyntaxFunctionsList;
		FSyntaxFunctionsList = NULL;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::EditViewMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled)
{
	if (Shift == TShiftState() << ssCtrl) {
		EditView->Font->Size += (WheelDelta > 0 ? 1 : -1);
		SettingsData->TempRegistry->Values["FrameSyntaxEditor.Font.Size"] = EditView->Font->Size;
		Handled = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::Font1Click(TObject *Sender) {
	bool bSizeFound = false;

	for (int i = 0; i < Font1->Count; i++) {
		Font1->Items[i]->Checked = EditView->Font->Size == StripHotkey(Font1->Items[i]->Caption).ToIntDef(0);
		if (Font1->Items[i]->Checked) {
			bSizeFound = true;
		}
	}

	MenuFontSizeCustom->Caption = bSizeFound ? UnicodeString(L"Custom") : (L"Custom (" + UnicodeString(EditView->Font->Size) + L")");
	MenuFontSizeCustom->Checked = !bSizeFound;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::MenuFontSizeCustomClick(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {
		const UnicodeString sCaption = StripHotkey(AMenuItem->Caption);
		if (sCaption.Pos("Custom")) {
			UnicodeString sValue(EditView->Font->Size);
			if (InputQuery("Edit font size", "Enter font size in px", sValue)) {
				EditView->Font->Size = sValue.ToIntDef(EditView->Font->Size);
			}
		}
		else {
			EditView->Font->Size = sCaption.ToIntDef(EditView->Font->Size);
		}

		SettingsData->TempRegistry->Values["FrameSyntaxEditor.Font.Size"] = EditView->Font->Size;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actLoadAccept(TObject *Sender) {
	try {
		if (EditView->Document) {
			EditView->Document->LoadFromFile(actLoad->Dialog->FileName);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"LOAD SYNTAX> %s", E->Message.c_str());
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actSaveAsAccept(TObject *Sender) {
	try {
		if (EditView->Document) {
			EditView->Document->SaveToFile(actSaveAs->Dialog->FileName);
			WLOG_INFO(L"File <%s> was successfully saved!", actSaveAs->Dialog->FileName.c_str());
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"SAVE SYNTAX> %s", E->Message.c_str());
	}
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TFrameSyntaxEditor::GetSyntaxFileExtenstion(void) {
	if (FSyntaxFileExtension.IsEmpty() && EditView->Document) {
		return Syntaxutils::SyntaxSchemeToExtension(EditView->Document->ActiveSyntaxScheme);
	}
	return FSyntaxFileExtension;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actLoadBeforeExecute(TObject *Sender) {
	actLoad->Dialog->DefaultExt = GetSyntaxFileExtenstion();
	actLoad->Dialog->Filter = Syntaxutils::GetDialogFilterByExtension(actLoad->Dialog->DefaultExt);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::actSaveAsBeforeExecute(TObject *Sender) {
	actSaveAs->Dialog->DefaultExt = GetSyntaxFileExtenstion();
	actSaveAs->Dialog->Filter = Syntaxutils::GetDialogFilterByExtension(actSaveAs->Dialog->DefaultExt);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditor::EditViewGutterClick(TLMDCustomEditView *AView, const TLMDGutterClickParams &AParams, bool &Handled) {
	try {
		if (AView->Document) {
			AView->Document->Breakpoints->DeleteMarksAtLine(AParams.ScrollLine);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"GUTTER Click> %s", E->Message.c_str());
	}
}
//---------------------------------------------------------------------------
