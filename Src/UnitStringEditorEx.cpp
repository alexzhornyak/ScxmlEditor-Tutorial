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

#include <memory>
// local
#include "UnitStringsEditor.h"

// header
#include "UnitStringEditorEx.h"
#include "UnitStringPairsEditor.h"
#include "Log4cpp_VCL.hpp"
#include "UnitSyntaxEditorForm.h"
#include "UnitCustomUtils.h"
#include "UnitLineStringEditor.h"
#include "UnitComboStringEditor.h"
#include "UnitPropInspectorExtensions.h"
#include "ScxmlShapes.hpp"
#include "UnitStateMachineConnection.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void __fastcall TStringsEditorEx::Edit(void) {
	TObject *AObject = this->GetObjectValue(0);

	if (AObject->InheritsFrom(__classid(TStrings))) {
		std::auto_ptr<TStringEditorForm>AStrEditForm(new TStringEditorForm(Application));
		AStrEditForm->sef_Memo->Lines->Assign(dynamic_cast<TStrings*>(AObject));
		if (AStrEditForm->ShowModal() == mrOk && Propinspext::IsWritablePropertyOfPropEditor(this)) {
			// какая-то хрень, если property, не TStrings, тогда через прокладку
			if (AObject->ClassType() != __classid(TStrings)) {
				std::auto_ptr<TStringList>AStringListPtr(new TStringList());
				AStringListPtr->Assign(AStrEditForm->sef_Memo->Lines);

				this->SetObjectValue(AStringListPtr.get());
			}
			else {
				this->SetObjectValue(AStrEditForm->sef_Memo->Lines);
			}
		}
	}
}

// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TStringsEditorEx::GetAttrs() {
	return TLMDPropAttrs() << praDialog << praReadOnly;
}

// ---------------------------------------------------------------------------
// ----------------------  TMultiStringsEditorEx  ----------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TMultiStringsEditorEx::GetAttrs() {
	return TLMDPropAttrs() << praDialog << praReadOnly << praMultiSelect;
}

// ---------------------------------------------------------------------------
// ----------------------  TMultiDisplayStringsEditorEx ----------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TMultiDisplayStringsEditorEx::GetValue(void) {

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
					sText = sText.SetLength(i_MAX_LENGTH).Trim() + "...";
				}
				else if (iCount > 1) {
					sText = AStrings->CommaText;
				}
			}
		}
	}
	return sText;
}

// ---------------------------------------------------------------------------
// --------------------------  TStringDialogEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TStringDialogEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TStringDialogEditor::Edit(void) {
	const UnicodeString sValue = this->GetStrValue(0);

	std::auto_ptr<TStringEditorForm>AStrEditForm(new TStringEditorForm(Application));
	AStrEditForm->sef_Memo->Lines->Text = sValue;

	if (AStrEditForm->ShowModal() == mrOk && Propinspext::IsWritablePropertyOfPropEditor(this)) {
		this->SetStrValue(AStrEditForm->sef_Memo->Lines->Text);
	}
}

// ---------------------------------------------------------------------------
// --------------------------  TStringsPairsEditor ---------------------------
// ---------------------------------------------------------------------------
void __fastcall TStringsPairsEditor::Edit(void) {
	TObject *AObject = this->GetObjectValue(0);

	if (AObject->InheritsFrom(__classid(TStrings))) {
		TStrings *AObjectStrings = dynamic_cast<TStrings*>(AObject);
		std::auto_ptr<TFormStringPairsEditor>AStrEditForm(new TFormStringPairsEditor(Application));
		AStrEditForm->AssignPairStrings(AObjectStrings);

		// для наследников, которым нужны будут спец.свойства
		OnFormCreated(AStrEditForm.get());

		if (AStrEditForm->ShowModal() == mrOk) {

			std::auto_ptr<TStringList>AStrings(new TStringList());
			AStrings->Assign(AStrEditForm->MemoSource->Lines);

			this->SetObjectValue(AStrings.get());
		}
	}
}

// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TStringsPairsEditor::GetAttrs() {
	return TLMDPropAttrs() << praDialog << praReadOnly << praMultiSelect;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TStringsPairsEditor::GetValue(void) {

	TObject *AObject = this->GetObjectValue(0);

	UnicodeString sText = L"";

	if (AObject->InheritsFrom(__classid(TStrings))) {
		TStrings * AStrings = dynamic_cast<TStrings*>(AObject);
		if (AStrings) {
			const int iCount = AStrings->Count;
			if (iCount) {
				sText = AStrings->Strings[0];

				const int i_MAX_LENGTH = 20;
				if (sText.Length() > i_MAX_LENGTH) {
					sText = sText.SetLength(i_MAX_LENGTH).Trim() + "...";
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
// --------------------------  TStringDialogEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TStringTextPairsEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TStringTextPairsEditor::Edit(void) {
	const UnicodeString sValue = this->GetStrValue(0);

	std::auto_ptr<TFormStringPairsEditor>AStrEditForm(new TFormStringPairsEditor(Application));
	AStrEditForm->AssignPairText(sValue);

	if (AStrEditForm->ShowModal() == mrOk && Propinspext::IsWritablePropertyOfPropEditor(this)) {
		this->SetStrValue(AStrEditForm->MemoSource->Lines->Text);
	}
}

// ---------------------------------------------------------------------------
// --------------------------  TXMLStringPropEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TXMLStringPropEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TXMLStringPropEditor::Edit(void) {

	const UnicodeString sValue = this->GetStrValue(0);
	try {
		TComponent *AInstance = dynamic_cast<TComponent*>(this->GetInstance(0));
		const UnicodeString sInstPropPath = Customutils::GetFullPropPath(AInstance);
		if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, PropName)) {
			std::auto_ptr<TFormSyntaxEditorForm>ASyntaxForm(new TFormSyntaxEditorForm(Application, "XML", sValue,
					UnicodeString().sprintf(L"Edit '%s'", PropName.c_str())));

			// открываем в любом случае, а пишем только, если Writable
			const int iModalResult = ASyntaxForm->ShowModal();
			if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
				if (iModalResult == mrOk) {
					this->SetStrValue(ASyntaxForm->EditDoc1->Lines->Text);
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
// --------------------------  TLineEditStringPropEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TLineEditStringPropEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TLineEditStringPropEditor::Edit(void) {

	const UnicodeString sValue = this->GetStrValue(0);

	try {
		std::auto_ptr<TLineStringEditor>ALineStringEditorPtr(new TLineStringEditor(Application));
		ALineStringEditorPtr->Caption = "LineStringEditor [" + PropName + "]";
		ALineStringEditorPtr->Edit1->Text = sValue;

		// открываем в любом случае, а пишем только, если Writable
		const int iModalResult = ALineStringEditorPtr->ShowModal();
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			if (iModalResult == mrOk) {
				this->SetStrValue(ALineStringEditorPtr->Edit1->Text);
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
bool __fastcall __EditUnicodeStringLineProperty(TPersistent *AInstance, const UnicodeString &sPropName) {
	std::auto_ptr<TLineStringEditor>ALineStringEditorPtr(new TLineStringEditor(Application));
	ALineStringEditorPtr->Caption = "Edit '" + sPropName + "'";
	ALineStringEditorPtr->Edit1->Text = GetPropValue(AInstance, sPropName);

	if (ALineStringEditorPtr->ShowModal() == mrOk) {
		SetPropValue(AInstance, sPropName, ALineStringEditorPtr->Edit1->Text);
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// --------------------------  TEventIDPropEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TEventIDPropEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TEventIDPropEditor::Edit(void) {

	const UnicodeString sValue = this->GetStrValue(0);

	try {
		std::auto_ptr<TComboStringEditor>AComboStringEditorPtr(new TComboStringEditor(Application));
		AComboStringEditorPtr->Caption = "Event ID Editor [" + PropName + "]";

		TScxmlBaseShape *ABaseShape = dynamic_cast<TScxmlBaseShape*>(this->GetInstance(0));
		if (ABaseShape && ABaseShape->Tree) {
			for (int i = 0; i < ABaseShape->Tree->Connections->Count; i++) {
				TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(ABaseShape->Tree->Connections->Items[i]);
				if (AConnection && !AConnection->Event.IsEmpty()) {
					AComboStringEditorPtr->AppendLine(AConnection->Event);
				}
			}
		}

		AComboStringEditorPtr->SetEditText(sValue);

		// открываем в любом случае, а пишем только, если Writable
		const int iModalResult = AComboStringEditorPtr->ShowModal();
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			if (iModalResult == mrOk) {
				this->SetStrValue(AComboStringEditorPtr->Edit1->Text);
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool __fastcall __EditEventIDLineProperty(TPersistent *AInstance, const UnicodeString &sPropName) {
	std::auto_ptr<TComboStringEditor>AComboStringEditorPtr(new TComboStringEditor(Application));
	AComboStringEditorPtr->Caption = "Edit '" + sPropName + "'";

	TScxmlBaseShape *ABaseShape = dynamic_cast<TScxmlBaseShape*>(AInstance);
	if (ABaseShape && ABaseShape->Tree) {
		for (int i = 0; i < ABaseShape->Tree->Connections->Count; i++) {
			TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(ABaseShape->Tree->Connections->Items[i]);
			if (AConnection && !AConnection->Event.IsEmpty()) {
				AComboStringEditorPtr->AppendLine(AConnection->Event);
			}
		}
	}

	AComboStringEditorPtr->SetEditText(GetPropValue(AInstance, sPropName));

	if (AComboStringEditorPtr->ShowModal() == mrOk) {
		SetPropValue(AInstance, sPropName, AComboStringEditorPtr->Edit1->Text);
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// --------------------------  TSendIDPropEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TSendIDPropEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TSendIDPropEditor::Edit(void) {

	const UnicodeString sValue = this->GetStrValue(0);

	try {
		std::auto_ptr<TComboStringEditor>AComboStringEditorPtr(new TComboStringEditor(Application));
		AComboStringEditorPtr->Caption = "Send ID Editor [" + PropName + "]";

		TScxmlBaseShape *ABaseShape = dynamic_cast<TScxmlBaseShape*>(this->GetInstance(0));
		if (ABaseShape && ABaseShape->Tree) {
			for (int i = 0; i < ABaseShape->Tree->Shapes->Count; i++) {
				TSendShape *ASendShape = dynamic_cast<TSendShape*>(ABaseShape->Tree->Shapes->Items[i]);
				if (ASendShape && !ASendShape->Id.IsEmpty()) {
					AComboStringEditorPtr->AppendLine(ASendShape->Id);
				}
			}
		}

		AComboStringEditorPtr->SetEditText(sValue);

		// открываем в любом случае, а пишем только, если Writable
		const int iModalResult = AComboStringEditorPtr->ShowModal();
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			if (iModalResult == mrOk) {
				this->SetStrValue(AComboStringEditorPtr->Edit1->Text);
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool __fastcall __EditSendIDLineProperty(TPersistent *AInstance, const UnicodeString &sPropName) {
	std::auto_ptr<TComboStringEditor>AComboStringEditorPtr(new TComboStringEditor(Application));
	AComboStringEditorPtr->Caption = "Edit '" + sPropName + "'";

	TScxmlBaseShape *ABaseShape = dynamic_cast<TScxmlBaseShape*>(AInstance);
	if (ABaseShape && ABaseShape->Tree) {
		for (int i = 0; i < ABaseShape->Tree->Shapes->Count; i++) {
			TSendShape *ASendShape = dynamic_cast<TSendShape*>(ABaseShape->Tree->Shapes->Items[i]);
			if (ASendShape && !ASendShape->Id.IsEmpty()) {
				AComboStringEditorPtr->AppendLine(ASendShape->Id);
			}
		}
	}

	AComboStringEditorPtr->SetEditText(GetPropValue(AInstance, sPropName));

	if (AComboStringEditorPtr->ShowModal() == mrOk) {
		SetPropValue(AInstance, sPropName, AComboStringEditorPtr->Edit1->Text);
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// --------------------------  TTransitionEventPropEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TTransitionEventPropEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TTransitionEventPropEditor::Edit(void) {

	const UnicodeString sValue = this->GetStrValue(0);

	try {
		std::auto_ptr<TComboStringEditor>AComboStringEditorPtr(new TComboStringEditor(Application));
		AComboStringEditorPtr->Caption = "Transition [" + PropName + "] Editor";

		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(this->GetInstance(0));
		if (AConnection && AConnection->Tree) {
			AConnection->FillEventNamesList(AComboStringEditorPtr->Lines);
		}

		AComboStringEditorPtr->SetEditText(sValue);

		// открываем в любом случае, а пишем только, если Writable
		const int iModalResult = AComboStringEditorPtr->ShowModal();
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			if (iModalResult == mrOk) {
				this->SetStrValue(AComboStringEditorPtr->Edit1->Text);
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
