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

#include "UnitSyntaxEditorForm.h"

#include <algorithm>
#include <IOUtils.hpp>
#include <StrUtils.hpp>

#include "UnitSyntaxEditorEx.h"
#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"
#include "UnitSyntaxString.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitCustomUtils.h"
#include "UnitPropInspectorExtensions.h"
#include "VISUAL_GLOBALS.h"
#include "UnitTransitionXMLEditor.h"
#include "UnitStateMachineUnit.h"
#include "UnitSyntaxUtils.h"
#include "UnitScxmlBaseShape.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDSedDocument"
#pragma resource "*.dfm"
TFormSyntaxEditorForm *FormSyntaxEditorForm;

static UnicodeString ReplaceCompletionCauseToString(TLMDEditReplaceCompletionCause Cause) {
	switch(Cause) {
	case rcCompletionList:
		return L"rcCompletionList";
	case rcWordEnd:
		return L"rcWordEnd";
	}
	return L"UNKNOWN";
}

// ---------------------------------------------------------------------------
static UnicodeString FillCompletionCauseToString(TLMDEditFillCompletionCause ACause) {
	switch(ACause) {
	case fcForced:
		return "Forced";
	case fcTimeout:
		return "Timeout";
	case fcWordEnd:
		return "WordEnd";
	}

	return L"UNKNOWN";
}

static TRect g_DefaultBounds = TRect(0, 0, 900, 504);

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TFormSyntaxEditorForm::TFormSyntaxEditorForm(TComponent* Owner, const UnicodeString &sSyntaxScheme,
	const UnicodeString &sText /* = L"" */ , const UnicodeString &sUserPreCaption/* = L""*/) : TForm(Owner), OnUpdateCaption(NULL) {

	FLinkInstancePropPath = L"";
	FLinkProperty = L"";
	FLinkUnit = L"";
	FLinkInstance = L"";

	FUserPreCaption = sUserPreCaption;

	FWasLeftPart = L"";

	const TLMDViewCompletionType ACompletionType = (sSyntaxScheme == "Text" || sSyntaxScheme == "XML") ? ctDocumentText : ctCustom;

	// Порядок вызова важен !!!
	// 1) активация синтаксической схемы у EditDoc
	EditDoc1->Lines->Text = sText;
	ApplySyntaxScheme(sSyntaxScheme);
	EditDoc1->Modified = false;

	IsFileMode = false;
	FActiveFileName = L"";

	// 2) формирование визуального представления
	FSyntaxEditor = new TFrameSyntaxEditorEx(this);
	FSyntaxEditor->Parent = this;
	FSyntaxEditor->Align = alClient;
	// после назначения EditDoc сформируются разрешения и их можно получать
	FSyntaxEditor->EditView->Document = EditDoc1;
	FSyntaxEditor->EditView->OnKeyUp = EditViewKeyUp;
	FSyntaxEditor->UpdateAllUtilsActions();
	// фокус по умолчанию, чтобы мы могли сразу начать редактировать
	FSyntaxEditor->TabOrder = 0;

	// здесь можно работать с FSyntaxEditor
	FileOpen1->Dialog->Filter = FSyntaxEditor->actLoad->Dialog->Filter;
	FileOpen1->Dialog->DefaultExt = FSyntaxEditor->actLoad->Dialog->DefaultExt;

	FileSaveAs1->Dialog->Filter = FSyntaxEditor->actLoad->Dialog->Filter;
	FileSaveAs1->Dialog->DefaultExt = FSyntaxEditor->actLoad->Dialog->DefaultExt;

	if (ACompletionType == ctCustom) {
		FSyntaxEditor->EditView->OnFillCompletionList = OnFillCompletionListEvent;

		FSyntaxEditor->EditView->CompletionSettings->CompletionType = ctCustom;
		// здесь убрана точка, так как подсказка может быть в виде составных слов с точкой
		FSyntaxEditor->EditView->CompletionSettings->WordSeparators = SettingsData->WordSeparators;

		FSyntaxEditor->OnAfterCompletionReplace = OnReplaceForCompletionResultEvent;
	}

	this->BoundsRect = g_DefaultBounds;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::UpdateCaption(void) {
	const UnicodeString sSyntaxScheme = this->EditDoc1->ActiveSyntaxScheme;
	DisplayName = UserPreCaption + L" [" + (sSyntaxScheme.IsEmpty() ? UnicodeString(L"Text") : sSyntaxScheme) + "]";
	if (IsFileMode && !ActiveFileName.IsEmpty()) {
		DisplayName = DisplayName + " [" + ActiveFileName + "]";
	}

	this->Caption = this->EditDoc1->Modified ? (DisplayName + L"*") : DisplayName;

	if (OnUpdateCaption) {
		OnUpdateCaption(this);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::OnReplaceForCompletionResultEvent(System::TObject* Sender, Classes::TStrings* Items, int ItemIndex,
	const Types::TPoint &CursorPoint, TLMDEditReplaceCompletionCause Cause, bool &Handled) {
#if 0 // для глубокой отладки
	WLOG_DEBUG(L"Completion replace> index:[%d] cause:[%s] handled:[%s]", ItemIndex,
		Cause == rcCompletionList ? L"CompletionList" : L"WordEnd", Handled ? L"true" : L"false");
#endif
	// блокировки на уровне SyntaxEditor
	if (Handled && !FWasLeftPart.IsEmpty()) {
		return;
	}

	// bugfix: при пустой строке

	TLMDEditView * AEditView = dynamic_cast<TLMDEditView*>(Sender);
	if (AEditView) {
		if (ItemIndex != -1 && Cause == rcCompletionList && FWasLeftPart.IsEmpty()) {
			const int iOffset = AEditView->ScrollPosToNearestOffset(CursorPoint);
			if (iOffset > -1) {
				EditDoc1->Insert(iOffset, Items->Strings[ItemIndex]);
				Handled = true;
			}
		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::OnFillCompletionListEvent(System::TObject* Sender, Classes::TStrings* Items,
	const Types::TPoint &CursorPoint, TLMDEditFillCompletionCause Cause, const Lmdtypes::TLMDString LeftPart, Types::TPoint &PosForCombo,
	bool &Handled) {

	FWasLeftPart = LeftPart;

#if 0 // для глубокой отладки
	WLOG_DEBUG(L"LeftPart:[%s] Cause:[%s]", LeftPart.c_str(), FillCompletionCauseToString(Cause).c_str());
#endif

	// ничего не вызываем, если просто печатаем текст и пустое значение
	if (LeftPart.IsEmpty() && Cause != fcForced) {
		Handled = true;
		return;
	}

	std::auto_ptr<TStringList>ACompleteListPtr(new TStringList());
	ACompleteListPtr->Sorted = true;
	ACompleteListPtr->Duplicates = dupIgnore;
	ACompleteListPtr->Text = Items->Text;

	try {

		TStateMachineDockPanel*ADockPanel = GetStateMachineDockPanel();
		if (ADockPanel) {
			const UnicodeString LeftPartUpper = LeftPart.UpperCase();
#if 0 // для глубокой отладки
			WLOG_DEBUG(L"LeftPartUpper:[%s]", LeftPartUpper.c_str());
#endif
			const int iKeywordX = CursorPoint.x - LeftPart.Length() - 1;
			if (CursorPoint.y >= 0 && CursorPoint.y < EditDoc1->LinesCount && iKeywordX > 0) {

				Lmdsedutils::TLMDSegment ALineSeg = EditDoc1->LineSegments[CursorPoint.y];
				if (!LMDSegIsBad(ALineSeg) && ALineSeg.Count > iKeywordX) {
					TLMDSegment AWordCursor = EditDoc1->FindPrevWord(ALineSeg.Start + iKeywordX, iKeywordX + 1);
					if (!LMDSegIsBad(AWordCursor)) {
						TLMDSegment ATillCursor = LMDSegment(AWordCursor.Start, (ALineSeg.Start + CursorPoint.x) - AWordCursor.Start,
							ALineSeg.Source);
						if (!LMDSegIsBad(ATillCursor)) {
							const UnicodeString sKeyWord = LMDSegToString(ATillCursor);
#if 0 // для глубокой отладки
							WLOG_DEBUG(L"Keyword:[%s]", sKeyWord.c_str());
#endif

							if (sKeyWord.Pos("In('") || sKeyWord.Pos("In(\"")) {

								if (!FStateIDs.get()) {
									FStateIDs.reset(new TStringList);
									FStateIDs->Sorted = true;
									FStateIDs->Duplicates = dupIgnore;

									ADockPanel->StateMachineEditor->GetStateNames(FStateIDs.get());
								}

								for (int i = 0; i < FStateIDs->Count; i++) {
									if (LeftPartUpper.IsEmpty() || FStateIDs->Strings[i].UpperCase().Pos(LeftPartUpper) == 1) {
										// здесь именно 'Items', чтобы не было лишнего копирования !
										Items->Add(FStateIDs->Strings[i]);
									}
								}

								Handled = true; // это даст команду дальше остановить все добавления в Complete

								return;
							}

						}

					}
				}
			}

			if (!FDataIDs.get()) {
				FDataIDs.reset(new TStringList);
				FDataIDs->Sorted = true;
				FDataIDs->Duplicates = dupIgnore;

				std::set<TStateMachineEditor*>ACompleteEditors;
				ACompleteEditors.insert(ADockPanel->StateMachineEditor);

				std::set<TStateMachineEditorUnit*>AVirtualParents = ADockPanel->StateMachineEditorUnit->GetVirtualParents();
				for (std::set<TStateMachineEditorUnit*>::iterator it = AVirtualParents.begin(); it != AVirtualParents.end(); ++it) {
					TStateMachineDockPanel *AVirtualPanel = (*it)->StateMachineDockPanel;
					if (AVirtualPanel) {
						ACompleteEditors.insert(AVirtualPanel->StateMachineEditor);
					}
				}

				for (std::set<TStateMachineEditor*>::iterator it = ACompleteEditors.begin(); it != ACompleteEditors.end(); ++it) {
					(*it)->GetDataIDs(FDataIDs.get());
				}
			}

			for (int i = 0; i < FDataIDs->Count; i++) {
				if (LeftPartUpper.IsEmpty() || FDataIDs->Strings[i].UpperCase().Pos(LeftPartUpper) == 1) {
					ACompleteListPtr->Add(FDataIDs->Strings[i]);
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	try {
		SettingsData->FillCompletionList(ACompleteListPtr.get(), LeftPart, this->EditDoc1->ActiveSyntaxScheme);

		TLMDEditView * ALMDEditView = dynamic_cast<TLMDEditView*>(Sender);
		if (ALMDEditView) {

			/* fix: Cursor.x == 0 gives an Exception 'invalid offset -1' */
			TPoint ACorrectedCursorPoint = CursorPoint;
			if (ACorrectedCursorPoint.x < 1) {
				ACorrectedCursorPoint.x = 1;
			}
#if 0
			WLOG_DEBUG(L"CursorPoint=[%d, %d] PosForCombo=[%d, %d]", CursorPoint.x, CursorPoint.y, PosForCombo.x, PosForCombo.y);
#endif
			ALMDEditView->CompletionFillListDef(ACompleteListPtr.get(), ACorrectedCursorPoint, Cause, PosForCombo);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	Items->Text = ACompleteListPtr->Text;

	/* Обязательно вызывать, чтобы дальше не применялся 'CompletionFillListDef' */
	Handled = true;
}

// ---------------------------------------------------------------------------
TStateMachineDockPanel *__fastcall TFormSyntaxEditorForm::GetStateMachineDockPanel(void) {
	TStateMachineDockPanel*ADockPanel = NULL;
	try {

		if (LinkInstancePropPath.IsEmpty()) {
			ADockPanel = GlobalActiveEditorDockPanel();
		}
		else {
			boost::tuple<TComponent*, TStateMachineEditor*,
			TStateMachineDockPanel*>ALinkInstances = ParseInstancePropPath(LinkInstancePropPath);
			ADockPanel = boost::get<2>(ALinkInstances);
		}
	}
	catch(Exception * E) {
		// может уже не существовать, это нормально
	}
	return ADockPanel;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::SetActiveFileName(UnicodeString val) {
	FActiveFileName = val;

	const UnicodeString sExt = ExtractFileExt(ActiveFileName);
	const UnicodeString sSyntax = Syntaxutils::ExtensionToSyntaxScheme(sExt);

	if (!sSyntax.IsEmpty()) {
		ApplySyntaxScheme(sSyntax);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::FileSaveAs1Accept(TObject *Sender) {
	EditDoc1->SaveToFile(FileSaveAs1->Dialog->FileName);
	if (IsFileMode) {
		ActiveFileName = FileOpen1->Dialog->FileName;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::FileOpen1Accept(TObject *Sender) {
	EditDoc1->LoadFromFile(FileOpen1->Dialog->FileName);
	if (IsFileMode) {
		ActiveFileName = FileOpen1->Dialog->FileName;
	}
	EditDoc1->Modified = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::FormCloseQuery(TObject *Sender, bool &CanClose) {
	if (EditDoc1->Modified) {
		String sCaption = "WARNING!";
		String sText = "Text is modified! Are you sure to quit?";
		CanClose = MessageBoxW(this->Handle, sText.c_str(), sCaption.c_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::ApplySyntaxScheme(const UnicodeString &sSyntaxScheme) {
	try {

		UnicodeString sSyntaxCase = L"Text";

		for (int i = 0; i < this->EditDoc1->SyntaxSchemes->Count; i++) {
			if (SameText(this->EditDoc1->SyntaxSchemes->Items[i]->Name, sSyntaxScheme)) {
				sSyntaxCase = this->EditDoc1->SyntaxSchemes->Items[i]->Name;
				break;
			}
		}

		this->EditDoc1->ActiveSyntaxScheme = sSyntaxCase;

		if (this->EditDoc1->ColorSchemes->FindItem(sSyntaxScheme)) {
			this->EditDoc1->ActiveColorScheme = sSyntaxScheme;
		}

		UpdateCaption();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::EditDoc1DocChanged(TLMDEditDocument *Sender, TLMDDocStatusChanges AChanges) {
	UpdateCaption();
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::ActionSaveExecute(TObject *Sender) {
	EditDoc1->Modified = false;
	ModalResult = mrOk;

	if (!LinkInstancePropPath.IsEmpty() && !LinkProperty.IsEmpty()) {
		try {

			boost::tuple<TComponent*, TStateMachineEditor*,
			TStateMachineDockPanel*>ALinkInstances = ParseInstancePropPath(LinkInstancePropPath);

			TComponent *AInstance = ALinkInstances.get<0>();
			TStateMachineEditor *AEditor = ALinkInstances.get<1>();
			TStateMachineDockPanel *ADockPanel = ALinkInstances.get<2>();

			if (IsPublishedProp(AInstance, LinkProperty)) {
				PPropInfo APropInfo = GetPropInfo(AInstance, LinkProperty);
				if (APropInfo) {
					const AnsiString sName = (*APropInfo->PropType)->Name;
					LOG_DEBUG("PropType=%s", sName.c_str());

					if (sName == "TSyntaxString") {
						TSyntaxString *ASyntaxString = dynamic_cast<TSyntaxString*>(GetObjectProp(AInstance, LinkProperty));
						if (ASyntaxString) {
							ASyntaxString->Text = EditDoc1->Lines->Text;
						}
					}
					else if (sName == "UnicodeString") {
						SetStrProp(AInstance, LinkProperty, EditDoc1->Lines->Text.TrimRight());
					}
					else if (sName == "TStrings" || sName == "TStringList") {
						TStrings * AStrings = dynamic_cast<TStrings*>(GetObjectProp(AInstance, LinkProperty));
						if (AStrings) {
							AStrings->Text = EditDoc1->Lines->Text;
						}
					}

					UnicodeString sVal = EditDoc1->Lines->Text.Trim();
					if (sVal.Length() > 40) {
						sVal = sVal.SetLength(40) + "...";
					}

					// помечаем измененным редактор
					if (AEditor) {
						AEditor->MarkModified(1, "Property [" + LinkProperty + "] was changed to [" + sVal + "]!", false);

						if (AEditor->PropSettingsInspector) {
							AEditor->PropSettingsInspector->UpdateContent();
						}
					}

					// обновляем надпись у панели
					if (ADockPanel) {
						ADockPanel->UpdateCaption();
					}
				}
			}

		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}

	if (IsFileMode && !ActiveFileName.IsEmpty()) {
		try {
			EditDoc1->SaveToFile(ActiveFileName);
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::FormClose(TObject *Sender, TCloseAction &Action) {
	g_DefaultBounds = this->BoundsRect;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::CheckSwitchToDocPanelClick(TObject *Sender) {
	if (CheckSwitchToDocPanel->Checked) {
		if (EditDoc1->Modified) {
			String Caption = "WARNING!!!";
			String Text = "Text was modified!\n" //
			"This operation will save all modifications!\n" //
			"Are you sure to continue?";
			if (Application->MessageBoxA(Text.w_str(), Caption.w_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK) {
				// здесь установится this->ModalResult == mrOk
				// и окно закроется с этим значением
				ActionSave->Execute();
			}
			else {
				// юзер отменил
				// перестраховываемся, чтобы не срабатывал обработчик
				CheckSwitchToDocPanel->OnClick = NULL;
				CheckSwitchToDocPanel->Checked = false;
				CheckSwitchToDocPanel->OnClick = CheckSwitchToDocPanelClick;
				return;
			}
		}

		// если дошли сюда, значит помечаем, что дальше в проекте не было изменений
		// эта хрень закроет модально окно
		this->ModalResult = mrYes;
	}
}

// ---------------------------------------------------------------------------
TStateMachineDockPanel * GetOwnerDockPanel(TComponent *AComponent) {
	while (AComponent) {
		TStateMachineDockPanel *AParentPanel = dynamic_cast<TStateMachineDockPanel*>(AComponent);
		if (AParentPanel) {
			return AParentPanel;
		}
		AComponent = AComponent->Owner;
	}
	return NULL;
}

// ---------------------------------------------------------------------------
bool __fastcall TFormSyntaxEditorForm::CloseAssociatedSavedDockPanels(TComponent *AInstance) {
	for (int i = GlobalMainForm()->ComponentCount - 1; i >= 0; i--) {
		TSyntaxDockPanel * ASyntaxDockPanel = dynamic_cast<TSyntaxDockPanel*>(GlobalMainForm()->Components[i]);
		if (ASyntaxDockPanel && Customutils::ComponentIsPresentInPath(AInstance, ASyntaxDockPanel->SyntaxForm->LinkInstancePropPath)) {

			if (ASyntaxDockPanel->SyntaxForm->IsModified()) {
				const UnicodeString Caption = L"WARNING!";
				const UnicodeString Text = UnicodeString().sprintf(L"Property [%s] is modified!\n" //
					"Dock panel [%s]\n" //
					"Are you sure to continue?", //
					ASyntaxDockPanel->SyntaxForm->LinkProperty.c_str(), //
					ASyntaxDockPanel->SyntaxForm->LinkInstancePropPath.c_str() //
					);
				if (Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONWARNING) != IDOK) {

					ASyntaxDockPanel->PanelVisible = true;

					if (ASyntaxDockPanel->Zone && ASyntaxDockPanel->Zone->Parent) {
						ASyntaxDockPanel->Zone->Parent->ActivePage = ASyntaxDockPanel->Zone;
					}

					return false;
				}
			}

			delete ASyntaxDockPanel;
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
bool __fastcall TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(const UnicodeString &sInstancePropPath, const UnicodeString &sPropName) {
	for (int i = 0; i < GlobalMainForm()->ComponentCount; i++) {
		TSyntaxDockPanel * ASyntaxDockPanel = dynamic_cast<TSyntaxDockPanel*>(GlobalMainForm()->Components[i]);
		if (ASyntaxDockPanel && ASyntaxDockPanel->SyntaxForm->LinkInstancePropPath == sInstancePropPath &&
			ASyntaxDockPanel->SyntaxForm->LinkProperty == sPropName) {

			ASyntaxDockPanel->PanelVisible = true;

			if (ASyntaxDockPanel->Zone && ASyntaxDockPanel->Zone->Parent) {
				ASyntaxDockPanel->Zone->Parent->ActivePage = ASyntaxDockPanel->Zone;
			}

			GlobalSendBallonMsg(UnicodeString().sprintf(L"Property [%s] is already opened in text editor!", sPropName.c_str()), bfInfo);

			ASyntaxDockPanel->SyntaxForm->SyntaxEditor->EditView->SetFocus();

			// ВНИМАНИЕ ПАНЕЛЬ С ЭТИМ ПУТЁМ УЖЕ СУЩЕСТВУЕТ!!!
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
bool __fastcall TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(const UnicodeString &sActiveFileName) {
	for (int i = 0; i < GlobalMainForm()->ComponentCount; i++) {
		TSyntaxDockPanel * ASyntaxDockPanel = dynamic_cast<TSyntaxDockPanel*>(GlobalMainForm()->Components[i]);
		if (ASyntaxDockPanel && SameText(ASyntaxDockPanel->SyntaxForm->ActiveFileName, sActiveFileName)) {

			ASyntaxDockPanel->PanelVisible = true;

			if (ASyntaxDockPanel->Zone && ASyntaxDockPanel->Zone->Parent) {
				ASyntaxDockPanel->Zone->Parent->ActivePage = ASyntaxDockPanel->Zone;
			}

			GlobalSendBallonMsg(UnicodeString().sprintf(L"File [%s] is already opened in text editor!", sActiveFileName.c_str()), bfInfo);

			ASyntaxDockPanel->SyntaxForm->SyntaxEditor->EditView->SetFocus();

			// ВНИМАНИЕ ПАНЕЛЬ С ЭТИМ ПУТЁМ УЖЕ СУЩЕСТВУЕТ!!!
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::CreateSyntaxDockPanel(const UnicodeString &sInstancePropPath, const UnicodeString &sPropName) {
	if (this->CheckSwitchToDocPanel->Checked) {

		if (CheckSyntaxDockPanelExists(sInstancePropPath, sPropName)) {
			return;
		}

		boost::tuple<TComponent*, TStateMachineEditor*, TStateMachineDockPanel*>ALinkInstances = ParseInstancePropPath(sInstancePropPath);

		TComponent *AInstance = ALinkInstances.get<0>();
		TStateMachineEditor *AEditor = ALinkInstances.get<1>();
		if (AEditor && AEditor == TransitionXMLEditor) {
			LOG_ERROR_SS << "DISABLED! Use menu 'Edit Transition' please!";
			return;
		}

		TSyntaxDockPanel *ADockPanel = new TSyntaxDockPanel(GlobalMainForm(), EditDoc1->ActiveSyntaxScheme, EditDoc1->Lines->Text);

		ADockPanel->SyntaxForm->LinkInstancePropPath = sInstancePropPath;
		ADockPanel->SyntaxForm->LinkProperty = sPropName;
		ADockPanel->Caption = ADockPanel->SyntaxForm->GetLinkCaption();

		ADockPanel->DockToGlobalRootZone(AInstance);
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TFormSyntaxEditorForm::GetLinkCaption(void) {
	return UnicodeString().sprintf(L"%s:%s.%s", LinkUnit.c_str(), LinkInstance.c_str(), LinkProperty.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::SetLinkInstancePropPath(UnicodeString val) {
	FLinkInstancePropPath = val;

	boost::tuple<TComponent*, TStateMachineEditor*, TStateMachineDockPanel*>ALinkInstances = ParseInstancePropPath(FLinkInstancePropPath);

	TComponent *AInstance = ALinkInstances.get<0>();
	TStateMachineEditor *AEditor = ALinkInstances.get<1>();

	if (AEditor && AEditor->StateMachineEditorUnit) {
		FLinkUnit = AEditor->StateMachineEditorUnit->FileName.c_str();
	}
	if (AInstance) {
		FLinkInstance = AInstance->Name.c_str();
	}
}

//---------------------------------------------------------------------------

void __fastcall TFormSyntaxEditorForm::EditViewKeyUp(TObject *Sender, WORD &Key, TShiftState Shift) {
	if (Key == VK_RETURN && Shift == TShiftState() << ssCtrl) {
		ActionSave->Execute();
	}
}

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// -----------------------  TUnicodeStringSyntaxDialogEditor -----------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TUnicodeStringSyntaxDialogEditorImpl::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TUnicodeStringSyntaxDialogEditorImpl::Edit(void) {
	const UnicodeString sValue = this->GetStrValue(0);

	TComponent *AInstance = dynamic_cast<TComponent*>(this->GetInstance(0));
	const UnicodeString sInstPropPath = Customutils::GetFullPropPath(AInstance);
	if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, PropName)) {
		std::auto_ptr<TFormSyntaxEditorForm>AStrEditForm(new TFormSyntaxEditorForm(Application, GetSyntaxScheme(), sValue,
				UnicodeString().sprintf(L"Edit '%s'", PropName.c_str())));

		// открываем в любом случае, а пишем только, если Writable
		const int iModalResult = AStrEditForm->ShowModal();
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			if (iModalResult == mrOk) {
				this->SetStrValue(AStrEditForm->EditDoc1->Lines->Text.Trim());

				// если это свойство Text у TSyntaxString
				TSyntaxString *ATempString = dynamic_cast<TSyntaxString*>(this->GetInstance(0));
				if (ATempString && !ATempString->IsSchemeLocked) {
					ATempString->SyntaxScheme = AStrEditForm->EditDoc1->ActiveSyntaxScheme;
				}
			}

			if (iModalResult == mrOk || iModalResult == mrYes) {
				AStrEditForm->CreateSyntaxDockPanel(sInstPropPath, PropName);
			}
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
bool __fastcall __EditUnicodeStringSyntaxProperty(TPersistent *AInstance, const UnicodeString &sPropName) {
	bool bRes = false;

	TComponent *AComponentInstance = dynamic_cast<TComponent*>(AInstance);
	const UnicodeString sInstPropPath = Customutils::GetFullPropPath(AComponentInstance);
	if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, sPropName)) {
		if (IsPublishedProp(AInstance, sPropName)) {

			const UnicodeString sValue = GetPropValue(AInstance, sPropName);

			std::auto_ptr<TFormSyntaxEditorForm>AStrEditForm(new TFormSyntaxEditorForm(Application, SettingsData->ActiveSyntaxScheme,
					sValue, UnicodeString().sprintf(L"Edit '%s'", sPropName.c_str())));
			const int iModalResult = AStrEditForm->ShowModal();
			if (iModalResult == mrOk) {

				bRes = true;

				SetPropValue(AInstance, sPropName, AStrEditForm->EditDoc1->Lines->Text.Trim());

				// если это свойство Text у TSyntaxString
				TSyntaxString *ATempString = dynamic_cast<TSyntaxString*>(AInstance);
				if (ATempString && !ATempString->IsSchemeLocked) {
					ATempString->SyntaxScheme = AStrEditForm->EditDoc1->ActiveSyntaxScheme;
				}
			}

			if (iModalResult == mrOk || iModalResult == mrYes) {
				AStrEditForm->CreateSyntaxDockPanel(sInstPropPath, sPropName);
			}

		}
	}

	return bRes;
}

// ---------------------------------------------------------------------------
// -------------------------TUnicodeStringSyntaxDialogEditor------------------
// ---------------------------------------------------------------------------
Lmdtypes::TLMDString TUnicodeStringSyntaxDialogEditor::GetSyntaxScheme(void) {
	return SettingsData->ActiveSyntaxScheme;
}

// ---------------------------------------------------------------------------
// -------------------------TStringsSyntaxClassEditorImpl-------------------------
// ---------------------------------------------------------------------------

void __fastcall TStringsSyntaxClassEditorImpl::Edit(void) {
	try {
		TStrings *AStrings = dynamic_cast<TStrings*>(this->GetObjectValue(0));
		if (!AStrings)
			throw Exception("Can not cast to <TStrings>!");

		TComponent * AInstance = dynamic_cast<TComponent*>(this->GetInstance(0));
		const UnicodeString sInstPropPath = Customutils::GetFullPropPath(AInstance);
		if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, PropName)) {
			std::auto_ptr<TFormSyntaxEditorForm>ASyntaxForm(new TFormSyntaxEditorForm(Application, GetSyntaxScheme(), AStrings->Text,
					UnicodeString().sprintf(L"Edit '%s'", PropName.c_str())));

			const int iModalResult = ASyntaxForm->ShowModal();
			if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
				if (iModalResult == mrOk) {
					// уберем пробелы в конце
					ASyntaxForm->EditDoc1->Lines->Text = ASyntaxForm->EditDoc1->Lines->Text.TrimRight();
					this->SetObjectValue(ASyntaxForm->EditDoc1->Lines);
				}

				if (iModalResult == mrOk || iModalResult == mrYes) {
					ASyntaxForm->CreateSyntaxDockPanel(sInstPropPath, PropName);
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TStringsSyntaxClassEditorImpl::GetAttrs() {
	return TLMDClassPropEditor::GetAttrs() << praDialog << praReadOnly << praMultiSelect;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TStringsSyntaxClassEditorImpl::GetValue(void) {
	UnicodeString sText = L"";

	try {
		TStrings *AStrings = dynamic_cast<TStrings*>(this->GetObjectValue(0));
		if (!AStrings)
			throw Exception("Can not cast to <TStrings>!");

		sText = AStrings->Text;
		const int i_MAX_LENGTH = 60;
		if (sText.Length() > i_MAX_LENGTH) {
			sText = sText.SetLength(i_MAX_LENGTH).Trim() + "...";
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	return sText;
}

// ---------------------------------------------------------------------------
// ---------------------- TSyntaxDockPanel -----------------------------------
// ---------------------------------------------------------------------------
__fastcall TSyntaxDockPanel::TSyntaxDockPanel(Classes::TComponent* AOwner, const UnicodeString &sSyntaxScheme,
	const UnicodeString &sText /* = L"" */ ) : TLMDDockPanel(AOwner), SyntaxForm(0) {

	SyntaxForm = new TFormSyntaxEditorForm(this, sSyntaxScheme, sText);
	SyntaxForm->CheckSwitchToDocPanel->Visible = false;
	SyntaxForm->Parent = this;
	SyntaxForm->BorderStyle = bsNone;
	SyntaxForm->Align = alClient;
	SyntaxForm->OnUpdateCaption = OnUpdateSyntaxFormCaption;
	SyntaxForm->BtnCancel->OnClick = OnSyntaxPanelClose;

	/* делаем свою кнопку закрытия, чтобы контролировать при закрытии несохраненные данные */
	FCloseButton = new TSpeedButton(this);
	FCloseButton->Flat = true;

	FCloseButton->Width = 22;
	FCloseButton->Height = 20;
	FCloseButton->Transparent = true;

	std::auto_ptr<Graphics::TBitmap>ABitmapPtr(new Graphics::TBitmap());
	ABitmapPtr->PixelFormat = pf24bit;
	ABitmapPtr->Width = 16;
	ABitmapPtr->Height = 16;
	SyntaxForm->ImageList1->GetBitmap(32, ABitmapPtr.get());
	FCloseButton->Glyph->Assign(ABitmapPtr.get());

	FCloseButton->OnClick = OnSyntaxPanelClose;
	FCloseButton->Parent = this;

	SyntaxForm->Show();

}

// ---------------------------------------------------------------------------
__fastcall TSyntaxDockPanel::~TSyntaxDockPanel(void) {
	this->OnHide = NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxDockPanel::AdjustClientRect(Types::TRect &Rect) {
	TLMDDockPanel::AdjustClientRect(Rect);

	FCloseButton->Left = Rect.Width() - FCloseButton->Width;

	// только в этом месте, иначе, если объявить в конструкторе, то вылетает с AccessViolation
	if (this->Buttons != TLMDDockPanelBtnKinds()) {
		this->Buttons = TLMDDockPanelBtnKinds();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxDockPanel::OnUpdateSyntaxFormCaption(TObject *Sender) {
	this->Caption = SyntaxForm->EditDoc1->Modified ? (SyntaxForm->LinkCaption + L"*") : SyntaxForm->LinkCaption;
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxDockPanel::OnSyntaxPanelClose(TObject *Sender) {
	if (SyntaxForm->CloseQuery()) {
		PostMessage(GlobalMainForm()->Handle, WM_SCXML_DELETE_OBJECTS, 0, (LPARAM)this);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxDockPanel::DockToGlobalRootZone(TComponent *AInstance) {

	GlobalDockSite()->DockControl(this, GlobalDockPanelRoot()->Zone, alClient);
	// запомним, есть ли в родителях док.панель
	TStateMachineDockPanel *AParentPanel = GetOwnerDockPanel(AInstance);
	// если родитель в той же зоне, что и созданная панель, тогда разместим после этой панели
	if (AParentPanel && AParentPanel->Zone && this->Zone && AParentPanel->Zone->Parent == this->Zone->Parent) {
		this->Zone->Index = AParentPanel->Zone->Index;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSyntaxEditorForm::FormDeactivate(TObject *Sender) {
	FDataIDs.reset();
	FStateIDs.reset();
}
//---------------------------------------------------------------------------
