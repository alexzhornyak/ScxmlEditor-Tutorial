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

#include "UnitMacroComposer.h"

#include "Log4cpp_VCL.hpp"

#include "UnitSettings.h"
#include "UnitPostBuildInterface.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDSedView"
#pragma link "LMDSedDocument"
#pragma resource "*.dfm"
TMacroComposer *MacroComposer;

// ---------------------------------------------------------------------------
__fastcall TMacroComposer::TMacroComposer(TComponent* Owner, const IPostBuildInterface *APostBuildInterface) : TForm(Owner) {
	if (!APostBuildInterface) {
		throw Exception("PostBuildInterface can not be NULL!");
	}

	EditDocOriginal->Lines->Text = APostBuildInterface->PostBuildCMD->Text;
	EditDocUser->Lines->Text = APostBuildInterface->UserMacros->Text;
	EditDocEvaluated->Lines->Text = APostBuildInterface->PostBuildEvaluated;
	EditDocSystem->Lines->Text = APostBuildInterface->Macros;

	DoCreateInstance();
}

// ---------------------------------------------------------------------------
__fastcall TMacroComposer::TMacroComposer(TComponent* Owner) : TForm(Owner) {

	EditDocOriginal->Clear();
	EditDocUser->Clear();
	EditDocEvaluated->Clear();
	EditDocSystem->Clear();

	DoCreateInstance();
}

// ---------------------------------------------------------------------------
void __fastcall TMacroComposer::DoCreateInstance(void) {

	FLastPressedKey = 0;

	for (int i = 0; i < this->ComponentCount; i++) {
		TLMDEditDocument * ALMDEditDocument = dynamic_cast<TLMDEditDocument*>(this->Components[i]);
		if (ALMDEditDocument) {
			ALMDEditDocument->ClearUndo();
			ALMDEditDocument->Modified = false;
		}
	}

	EditView->CompletionSettings->WordSeparators = EditDocOriginal->WordSeparators;
}

// ---------------------------------------------------------------------------
void __fastcall TMacroComposer::TabControl1Change(TObject *Sender) {
	UpdateEvaluated(EditDocEvaluated->Lines, EditDocOriginal->Lines, EditDocSystem->Lines, EditDocUser->Lines);
	switch(TabControl1->TabIndex) {
	case 0: {
			EditView->Document = EditDocOriginal;
		};
		break;
	case 1: {
			EditView->Document = EditDocEvaluated;
		};
		break;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TMacroComposer::FormCloseQuery(TObject *Sender, bool &CanClose) {
	if (this->ModalResult != mrOk && (EditDocOriginal->Modified || EditDocUser->Modified)) {
		String Caption = "WARNING!!!";
		String Text = "You have modified data! Do you want to quit?";
		CanClose = Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TMacroComposer::EditDocOriginalDocChanged(TLMDEditDocument *Sender, TLMDDocStatusChanges AChanges) {
	if (AChanges.Contains(dcModifiedPropertyChanged)) {
		this->Caption = ((EditDocOriginal->Modified || EditDocUser->Modified) ? L"*" : L"") + UnicodeString("MacroComposer");
	}
}
// ---------------------------------------------------------------------------

void __fastcall TMacroComposer::EditViewFillCompletionList(TObject *Sender, TStrings *Items, TPoint &CursorPoint,
	TLMDEditFillCompletionCause Cause, const TLMDString LeftPart, TPoint &PosForCombo, bool &Handled) {

	// устраняем баг связанный с тем, что если пустое левая часть, то выдаётся Exception
	// 'This operation requires given offset to be visible. Offset: -1';
	if (LeftPart.IsEmpty()) {
		Handled = true;
		return;
	}

	for (int i = 0; i < EditDocSystem->Lines->Count; i++) {
		if (EditDocSystem->Lines->Names[i].LowerCase().Pos(LeftPart.LowerCase()) == 1) {
			Items->Add(EditDocSystem->Lines->Names[i]);
		}
	}

	for (int i = 0; i < EditDocUser->Lines->Count; i++) {
		if (EditDocUser->Lines->Names[i].LowerCase().Pos(LeftPart.LowerCase()) == 1) {
			Items->Add(EditDocUser->Lines->Names[i]);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TMacroComposer::EditViewCompletionReplace(TObject *Sender, TStrings *Items, int ItemIndex, TPoint &CursorPoint,
	TLMDEditReplaceCompletionCause Cause, bool &Handled) {

	TLMDEditView * ALMDEditView = dynamic_cast<TLMDEditView*>(Sender);
	if (ALMDEditView) {
		// запрещаем дополнение по разделителю слова
		static const UnicodeString sWordSeparators = ".[];,'\"-+/\\:{} ";
		Handled = sWordSeparators.Pos(FLastPressedKey);
	}

}
// ---------------------------------------------------------------------------

void __fastcall TMacroComposer::EditViewKeyPressW(TObject *Sender, wchar_t &Key) {
	FLastPressedKey = Key;
}

// ---------------------------------------------------------------------------
