// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

// #include "UnitTranslate.h"

#include "UnitLuaEditor.h"

#include <boost/regex.hpp>

#include <StrUtils.hpp>
#include <Character.hpp>

#include "UnitSyntaxEditor.h"
#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"
#include "UnitScxmlBaseShape.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDSedView"
#pragma link "LMDSedDocument"
#pragma resource "*.dfm"

// ---------------------------------------------------------------------------
__fastcall TFormLuaEditor::TFormLuaEditor(TComponent* Owner) : TForm(Owner), FCompleteTable(new TStringList) {
	FSyntaxEditor = new TFrameSyntaxEditor(this);
	FSyntaxEditor->Parent = this;
	FSyntaxEditor->Align = alClient;
	FSyntaxEditor->EditView->Document = EditDocumentLua;
	FSyntaxEditor->EditView->OnFillCompletionList = &OnFillCompletionListEvent;
	FSyntaxEditor->EditView->OnCompletionReplace = &EditViewCompletionReplace;

	FSyntaxEditor->EditView->CompletionSettings->CompletionType = ctCustom;
	FSyntaxEditor->EditView->CompletionSettings->WordSeparators = "[]();,'\"-+/\{}";
	// FSyntaxEditor->EditView->CompletionSettings->DontShowComboForOnlyMatch = false;
}

// ---------------------------------------------------------------------------
bool MatchesSecondLevel(const UnicodeString &sText, UnicodeString &sCaption, UnicodeString &sDelimiter) {
	boost::wregex e(L"^([^\\.\\:]*)([\\.\\:])$");
	boost::wsmatch what;

	std::wstring wstr = sText.c_str();
	if (boost::regex_match(wstr, what, e)) {
		if (what.size() == 3) {
			sCaption = std::wstring(what[1]).c_str();
			sDelimiter = std::wstring(what[2]).c_str();
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TFormLuaEditor::OnFillCompletionListEvent(System::TObject* Sender, Classes::TStrings* Items,
	const Types::TPoint &CursorPoint, TLMDEditFillCompletionCause Cause, const Lmdtypes::TLMDString LeftPart, Types::TPoint &PosForCombo,
	bool &Handled) {

	for (int i = 0; i < SettingsData->CompletionWords->Count; i++) {
		if (SettingsData->CompletionWords->Strings[i].Pos(LeftPart) == 1) {
			Items->Add(SettingsData->CompletionWords->Strings[i]);
		}
	}
}

UnicodeString CompletionCauseToString(TLMDEditReplaceCompletionCause Cause) {
	switch(Cause) {
	case rcCompletionList:
		return L"rcCompletionList";
	case rcWordEnd:
		return L"rcWordEnd";
	}
	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TFormLuaEditor::EditViewCompletionReplace(System::TObject* Sender, Classes::TStrings* Items, int ItemIndex,
	const Types::TPoint &CursorPoint, TLMDEditReplaceCompletionCause Cause, bool &Handled) {

	WLOG_INFO(L"Item=[%s],Cause=[%s]", Items->Strings[ItemIndex].c_str(), CompletionCauseToString(Cause).c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TFormLuaEditor::FileOpen1Accept(TObject *Sender) {
	EditDocumentLua->LoadFromFile(FileOpen1->Dialog->FileName);
	EditDocumentLua->Modified = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFormLuaEditor::FileSaveAs1Accept(TObject *Sender) {
	EditDocumentLua->SaveToFile(FileSaveAs1->Dialog->FileName);
}

// ---------------------------------------------------------------------------
void __fastcall TLuaStringsEditorEx::Edit(void) {
	TObject *AObject = this->GetObjectValue(0);

	if (AObject->InheritsFrom(__classid(TStrings))) {
		std::auto_ptr<TFormLuaEditor>AStrEditForm(new TFormLuaEditor(Application));
		AStrEditForm->EditDocumentLua->Lines->Assign(dynamic_cast<TStrings*>(AObject));
		if (AStrEditForm->ShowModal() == mrOk) {

			std::auto_ptr<TStringList>AStrings(new TStringList());
			AStrings->Assign(AStrEditForm->EditDocumentLua->Lines);

			this->SetObjectValue(AStrings.get());
		}
	}
}

// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TLuaStringsEditorEx::GetAttrs() {
	return TLMDPropAttrs() << praDialog << praReadOnly << praMultiSelect;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TLuaStringsEditorEx::GetValue(void) {

	TObject *AObject = this->GetObjectValue(0);

	UnicodeString sText = L"";

	if (AObject->InheritsFrom(__classid(TStrings))) {
		TStrings * AStrings = dynamic_cast<TStrings*>(AObject);
		if (AStrings) {
			const int iCount = AStrings->Count;
			if (iCount) {
				sText = AStrings->Strings[0];

				const int i_MAX_LENGTH = 60;
				if (sText.Length() > i_MAX_LENGTH) {
					sText = sText.SetLength(i_MAX_LENGTH) + "...";
				}
				else if (iCount > 1) {
					sText = sText + "...";
				}
			}
		}
	}
	return sText;
}

// ---------------------------------------------------------------------------
// ---------------------------- TFormLuaEditor -------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
void __fastcall TFormLuaEditor::FormCloseQuery(TObject *Sender, bool &CanClose) {
	if (EditDocumentLua->Modified) {
		// String Caption = _("ID_48", "WARNING!");
		// String Text = _("ID_328", "Settings are modified! Are you sure to quit?");
		// CanClose = Application->MessageBoxA(Text.w_str(), Caption.w_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK;

		String Caption = "WARNING!";
		String Text = "Settings are modified! Are you sure to quit?";
		CanClose = Application->MessageBoxA(Text.w_str(), Caption.w_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK;
	}
}

// ---------------------------------------------------------------------------
void TFormLuaEditor::OnLanguageChanged() {
	// this->Caption = _("ID_329", this->Caption);
	//
	// FileOpen1->Caption = _("ID_87", FileOpen1->Caption);
	// FileSaveAs1->Caption = _("ID_88", FileSaveAs1->Caption);
	// BtnCancel->Caption = _("ID_333", BtnCancel->Caption);

}

// ---------------------------------------------------------------------------
void __fastcall TFormLuaEditor::FormCreate(TObject *Sender) {
	OnLanguageChanged();
}

// ---------------------------------------------------------------------------
void __fastcall TFormLuaEditor::BtnOkClick(TObject *Sender) {
	EditDocumentLua->Modified = false;
}

// ---------------------------------------------------------------------------
void __fastcall TFormLuaEditor::FormShow(TObject *Sender) {
	EditDocumentLua->Modified = false;
}

// ---------------------------------------------------------------------------
