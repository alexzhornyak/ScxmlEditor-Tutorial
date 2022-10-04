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

#include "UnitSyntaxString.h"

#include <memory>

#include "UnitStringEditorEx.h"
#include "Log4cpp_VCL.hpp"
#include "UnitSyntaxEditorForm.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitStateMachineParser.h"
#include "UnitCustomUtils.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitScxmlBaseShape.h"
#include "UnitSettings.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
__fastcall TSyntaxString::TSyntaxString(TComponent *AOwner, const UnicodeString &sOwnerPropName) : FOwnerInstance(AOwner),
FOwnerPropertyName(sOwnerPropName), FType(TXMLTextType::xttText), FText(L""), FSyntaxScheme(L""), FIsSchemeLocked(false) {
	FScxmlStream = new TMemoryStream;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSyntaxString::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	if (sPropName == "IsSchemeLocked") {
		return //
		"Shows whether type of text could be changed or not\n" //
		"Read-only: application defined value" //
		;
	}

	if (sPropName == "IsScxmlGui") {
		return //
		"If is set to <b>'true'</b> then text is edited with <b>Scxml Editor</b>\n" //
		"Read-only: depends on <b>'Type'==xttSCXML</b>" //
		;
	}

	if (sPropName == "Text") {
		return //
		"String value that depends on 'Type' and 'SyntaxScheme'.\n" //
		;
	}

	if (sPropName == "Type") {
		return //
		"1) <b>xttText</b> - simple XML text\n" //
		"2) <b>xttCDATA</b> - XML text in CDATA representation\n" //
		"3) <b>xttXML</b> - nested XML nodes\n" //
		"4) <b>xttSCXML</b> - SCXML stream\n" //
		;
	}

	if (sPropName == "SyntaxScheme") {
		return //
		"Syntax scheme will be applied in syntax editor of value 'Text'.\n" //
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::Clear() {
	FIsSchemeLocked = false;
	const bool bTextWasChanged = FText.Length() > 0;
	FText = L"";

	FType = TXMLTextType::xttText;
	FSyntaxScheme = L"";

	FScxmlStream->Clear();

	if (bTextWasChanged && OnTextChanged)
		OnTextChanged(this);
}

// ---------------------------------------------------------------------------
__fastcall TSyntaxString::TSyntaxString(TComponent *AOwner, const UnicodeString &sOwnerPropName, bool bLocked,
	const UnicodeString &SyntaxScheme, const TXMLTextType AType /* =TXMLTextType::xttText */ ) : FOwnerInstance(AOwner),
FOwnerPropertyName(sOwnerPropName), FType(AType), FSyntaxScheme(SyntaxScheme), FIsSchemeLocked(bLocked), FText(L"") {
	FScxmlStream = new TMemoryStream;
}

// ---------------------------------------------------------------------------
__fastcall TSyntaxString::~TSyntaxString(void) {
	delete FScxmlStream;
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo && APropInfo->PropType()->Kind == tkUString) {
		// иначе начнут неправильно другие строковые отображаться
		if (APropInfo->Name() == L"Text") {
			AEditorClass = __classid(TSyntaxStringPropEditor);
		}
		else if (APropInfo->Name() == L"SyntaxScheme") {
			AEditorClass = __classid(TSyntaxSchemeComboEditor);
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TSyntaxString::IsSyntaxSchemeStored(void) {
	return !FSyntaxScheme.IsEmpty() && FSyntaxScheme != L"Text";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSyntaxString::GetSyntaxScheme(void) {

	UnicodeString sSyntaxScheme = FSyntaxScheme;

	if (sSyntaxScheme.IsEmpty()) {
		if (TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(this->FOwnerInstance)) {
			sSyntaxScheme = GetTreeSyntaxScheme(AScxmlBaseShape->Tree);
		}

		// для случаев, где редактирование в других редакторах
		if (sSyntaxScheme.IsEmpty()) {
			sSyntaxScheme = SettingsData->ActiveSyntaxScheme;
		}
	}

	return sSyntaxScheme.IsEmpty() ? L"Text" : sSyntaxScheme.c_str();
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::SetSyntaxScheme(UnicodeString val) {
	if (!FIsSchemeLocked && !SameText(GetSyntaxScheme(), val))
		FSyntaxScheme = val;
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::Assign(Classes::TPersistent * Source) {
	if (Source) {
		TSyntaxString * ASyntaxString = dynamic_cast<TSyntaxString*>(Source);
		if (ASyntaxString) {
			FIsSchemeLocked = ASyntaxString->FIsSchemeLocked;
			FText = ASyntaxString->FText;

			FType = ASyntaxString->FType;
			FSyntaxScheme = ASyntaxString->FSyntaxScheme;

			FScxmlStream->Clear();
			if (ASyntaxString->FScxmlStream->Size > 0) {
				ASyntaxString->FScxmlStream->SaveToStream(FScxmlStream);
			}

			if (OnTextChanged)
				OnTextChanged(this);
		}
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::DefineProperties(Classes::TFiler* Filer) {
	TPersistent::DefineProperties(Filer);

	Filer->DefineBinaryProperty("ScxmlData", ReadScxmlStreamProc, WriteScxmlStreamProc, this->Type == xttSCXML);
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::ReadScxmlStreamProc(TStream* Stream) {
	FScxmlStream->LoadFromStream(Stream);
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::WriteScxmlStreamProc(TStream* Stream) {
	FScxmlStream->SaveToStream(Stream);
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::OnScxmlTreeEditorModfied(TObject *Sender) {
	TStateMachineEditor * AEditor = dynamic_cast<TStateMachineEditor*>(Sender);
	if (AEditor) {

		const UnicodeString sInstPropPath = Customutils::GetFullPropPath(this->FOwnerInstance);
		AEditor->Caption = UnicodeString().sprintf(L"Edit [%s%s]", sInstPropPath.c_str(), AEditor->IsModified() ? L"*" : L"");

	}
}

// ---------------------------------------------------------------------------
bool __fastcall TSyntaxString::EditXMLText() {
	bool bRetVal = false;
	try {
		const UnicodeString sInstPropPath = Customutils::GetFullPropPath(this->FOwnerInstance);
		if (this->Type != xttSCXML) {

			if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, this->FOwnerPropertyName)) {
				std::auto_ptr<TFormSyntaxEditorForm>ASyntaxForm(new TFormSyntaxEditorForm(Application, this->SyntaxScheme, this->Text,
						UnicodeString().sprintf(L"Edit '%s'", this->FOwnerPropertyName.c_str())));

				const int iModalResult = ASyntaxForm->ShowModal();
				if (iModalResult == mrOk) {
					this->Text = ASyntaxForm->EditDoc1->Lines->Text.TrimRight();
					this->SyntaxScheme = ASyntaxForm->EditDoc1->ActiveSyntaxScheme;

					bRetVal = true;
				}

				if (iModalResult == mrOk || iModalResult == mrYes) {
					ASyntaxForm->CreateSyntaxDockPanel(sInstPropPath, this->FOwnerPropertyName);
				}
			}
		}
		else {
			try {
				std::auto_ptr<TStateMachineEditor>AStateMachineEditorForm(new TStateMachineEditor(Application, NULL, false));
				if (this->FScxmlStream->Size != 0) {
					AStateMachineEditorForm->LoadScxmlStream(this->FScxmlStream, "Scxml editor");
				}
				else {
					if (this->FText.IsEmpty()) {
						AStateMachineEditorForm->LoadScxmlData(L"<scxml version=\"1.0\" xmlns=\"http://www.w3.org/2005/07/scxml\"/>",
							"Scxml editor");
					}
					else {
						AStateMachineEditorForm->LoadScxmlData(this->FText, "Scxml editor");
					}
				}
				AStateMachineEditorForm->Caption = "Edit [" + sInstPropPath + "]";
				AStateMachineEditorForm->OnTreeEditorModified = OnScxmlTreeEditorModfied;

				if (AStateMachineEditorForm->ShowModal() == mrOk) {
					// только формируем текст
					std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));
					Statemachine::SaveTreeToScxml(AStateMachineEditorForm->TheTree, AStreamPtr.get(), false, true);

					std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
					SaveTreeToStream(AStateMachineEditorForm->TheTree, AMemoryStream.get());

					this->Text = AStreamPtr->DataString;

					this->FScxmlStream->Clear();
					AMemoryStream->SaveToStream(this->FScxmlStream);

					bRetVal = true;
				}
			}
			__finally {
				// bugfix: почему-то уходит на задний план
				Application->BringToFront();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	return bRetVal;
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxString::AppendToElement(ILMDXmlElement *ANode) {
	switch(this->Type) {
	case xttCDATA:
		ANode->AppendCDATA(this->Text);
		break;
	case xttSCXML:
	case xttXML: {
			_di_ILMDXmlDocument ADoc = LMDLoadXmlDocumentFromXML(this->Text);
			if (ADoc) {
				for (int i = 0; i < ADoc->ChildNodes->Count; i++) {
					ANode->AppendChild(ADoc->ChildNodes->Item[i]);
				}
			}
		}break;
	default:
		ANode->AppendText(this->Text);
	}
}

// ---------------------------------------------------------------------------
// -------------------------TTSyntaxStringClassEditor-------------------------
// ---------------------------------------------------------------------------

void __fastcall TSyntaxStringClassEditor::Edit(void) {
	TSyntaxString *ASyntaxString = dynamic_cast<TSyntaxString*>(this->GetObjectValue(0));
	if (ASyntaxString) {
		try {
			const UnicodeString sInstPropPath = Customutils::GetFullPropPath(ASyntaxString->FOwnerInstance);
			if (ASyntaxString->Type != xttSCXML) {
				if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, ASyntaxString->FOwnerPropertyName)) {

					UnicodeString sPreCaption = L"Edit";
					if (!ASyntaxString->FOwnerPropertyName.IsEmpty()) {
						sPreCaption = UnicodeString().sprintf(L"%s '%s'", sPreCaption, ASyntaxString->FOwnerPropertyName.c_str());
					}

					std::auto_ptr<TFormSyntaxEditorForm>ASyntaxForm(new TFormSyntaxEditorForm(Application, ASyntaxString->SyntaxScheme,
							ASyntaxString->Text, sPreCaption));

					const int iModalResult = ASyntaxForm->ShowModal();
					if (Propinspext::IsWritablePropertyOfPropEditor(this)) {

						if (iModalResult == mrOk) {
							// временный класс, чтобы назначить другие свойства
							std::auto_ptr<TSyntaxString>ATempString(new TSyntaxString(dynamic_cast<TComponent*>(this->GetInstance(0)),
									L""));
							ATempString->Assign(ASyntaxString);
							// непоср. текст
							ATempString->Text = ASyntaxForm->EditDoc1->Lines->Text.TrimRight();

							if (!ATempString->IsSchemeLocked) {
								ATempString->SyntaxScheme = ASyntaxForm->EditDoc1->ActiveSyntaxScheme;
							}

							// назначаем всем
							this->SetObjectValue(ATempString.get());
						}

						if (iModalResult == mrOk || iModalResult == mrYes) {
							ASyntaxForm->CreateSyntaxDockPanel(sInstPropPath, ASyntaxString->FOwnerPropertyName);
						}
					}
				}
			}
			else {
				try {
					std::auto_ptr<TStateMachineEditor>AStateMachineEditorForm(new TStateMachineEditor(Application, NULL, false));
					if (ASyntaxString->FScxmlStream->Size != 0) {
						AStateMachineEditorForm->LoadScxmlStream(ASyntaxString->FScxmlStream, "Scxml editor");
					}
					else {
						if (ASyntaxString->Text.IsEmpty()) {
							AStateMachineEditorForm->LoadScxmlData(L"<scxml version=\"1.0\" xmlns=\"http://www.w3.org/2005/07/scxml\"/>",
								"Scxml editor");
						}
						else {
							AStateMachineEditorForm->LoadScxmlData(ASyntaxString->Text, L"Scxml editor");
						}
					}
					AStateMachineEditorForm->Caption = "Edit [" + sInstPropPath + "]";
					AStateMachineEditorForm->OnTreeEditorModified = ASyntaxString->OnScxmlTreeEditorModfied;

					if (AStateMachineEditorForm->ShowModal() == mrOk && Propinspext::IsWritablePropertyOfPropEditor(this)) {
						// отдельно текст
						std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));
						Statemachine::SaveTreeToScxml(AStateMachineEditorForm->TheTree, AStreamPtr.get(), false, true);

						// отдельно дерево
						std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
						SaveTreeToStream(AStateMachineEditorForm->TheTree, AMemoryStream.get());

						// временный класс, чтобы назначить другие свойства
						std::auto_ptr<TSyntaxString>ATempString(new TSyntaxString(dynamic_cast<TComponent*>(this->GetInstance(0)), L""));
						ATempString->Assign(ASyntaxString);
						// непоср. текст
						ATempString->Text = AStreamPtr->DataString;
						// дерево
						ATempString->FScxmlStream->Clear();
						AMemoryStream->SaveToStream(ATempString->FScxmlStream);

						// назначаем всем
						this->SetObjectValue(ATempString.get());
					}
				}
				__finally {
					// bugfix: почему-то уходит на задний план
					Application->BringToFront();
				}

			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TSyntaxStringClassEditor::GetAttrs() {
	return TLMDClassPropEditor::GetAttrs() << praDialog << praReadOnly << praMultiSelect;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TSyntaxStringClassEditor::GetValue(void) {
	UnicodeString sText = L"";
	TSyntaxString *ASyntaxString = dynamic_cast<TSyntaxString*>(this->GetObjectValue(0));
	if (ASyntaxString) {

		sText = ASyntaxString->Text;

		//		const int i_MAX_LENGTH = 60;
		//		if (sText.Length() > i_MAX_LENGTH) {
		//			sText = sText.SetLength(i_MAX_LENGTH) + "...";
		//		}
	}
	return sText;
}

// ---------------------------------------------------------------------------
// --------------------------- TSyntaxSchemeComboEditor ---------------------------
// ---------------------------------------------------------------------------

void __fastcall TSyntaxSchemeComboEditor::GetValues(TLMDWideStrings* AValues) {
	std::auto_ptr<TStringList>AStringList(new TStringList());
	AStringList->Add("Text");
	AStringList->Add("Lua");
	AStringList->Add("XML");
	AStringList->Add("C");
	AStringList->Add("CPP");
	AStringList->Add("JavaScript");
	AStringList->Add("EcmaScript");
	AValues->AddStrings(AStringList.get());
}

TLMDPropAttrs __fastcall TSyntaxSchemeComboEditor::GetAttrs() {
	return TLMDPropAttrs() << praMultiSelect << praReadOnly << praValueList << praSortList;
}

void __fastcall TSyntaxSchemeComboEditor::SetValue(const Lmdtypes::TLMDString Value) {
	SetStrValue(Value);
}

// ---------------------------------------------------------------------------
// --------------------------  TSyntaxPropEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TSyntaxStringPropEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxStringPropEditor::Edit(void) {

	const UnicodeString sValue = this->GetStrValue(0);

	TSyntaxString *ASyntaxString = dynamic_cast<TSyntaxString*>(this->GetInstance(0));
	if (ASyntaxString) {
		try {
			const UnicodeString sInstPropPath = Customutils::GetFullPropPath(ASyntaxString->FOwnerInstance);
			if (ASyntaxString->Type != xttSCXML) {
				if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, ASyntaxString->FOwnerPropertyName)) {
					const UnicodeString sSyntaxScheme = ASyntaxString->SyntaxScheme;

					UnicodeString sPreCaption = L"Edit";
					if (!ASyntaxString->FOwnerPropertyName.IsEmpty()) {
						sPreCaption = UnicodeString().sprintf(L"%s '%s'", sPreCaption, ASyntaxString->FOwnerPropertyName.c_str());
					}

					std::auto_ptr<TFormSyntaxEditorForm>ASyntaxForm(new TFormSyntaxEditorForm(Application, sSyntaxScheme, sValue,
							sPreCaption));

					const int iModalResult = ASyntaxForm->ShowModal();
					if (Propinspext::IsWritablePropertyOfPropEditor(this)) {

						if (iModalResult == mrOk) {
							this->SetStrValue(ASyntaxForm->EditDoc1->Lines->Text);

							if (!ASyntaxString->IsSchemeLocked) {
								ASyntaxString->SyntaxScheme = ASyntaxForm->EditDoc1->ActiveSyntaxScheme;
							}
						}

						if (iModalResult == mrOk || iModalResult == mrYes) {
							ASyntaxForm->CreateSyntaxDockPanel(sInstPropPath, ASyntaxString->FOwnerPropertyName);
						}
					}
				}
			}
			else {
				try {
					std::auto_ptr<TStateMachineEditor>AStateMachineEditorForm(new TStateMachineEditor(Application, NULL, false));
					if (ASyntaxString->FScxmlStream->Size != 0) {
						AStateMachineEditorForm->LoadScxmlStream(ASyntaxString->FScxmlStream, "Scxml editor");
					}
					else {
						if (sValue.IsEmpty()) {
							AStateMachineEditorForm->LoadScxmlData(L"<scxml version=\"1.0\" xmlns=\"http://www.w3.org/2005/07/scxml\"/>",
								"Scxml editor");
						}
						else {
							AStateMachineEditorForm->LoadScxmlData(sValue, "Scxml editor");
						}
					}
					AStateMachineEditorForm->Caption = "Edit [" + sInstPropPath + "]";
					AStateMachineEditorForm->OnTreeEditorModified = ASyntaxString->OnScxmlTreeEditorModfied;

					if (AStateMachineEditorForm->ShowModal() == mrOk && Propinspext::IsWritablePropertyOfPropEditor(this)) {
						// только формируем текст
						std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));
						Statemachine::SaveTreeToScxml(AStateMachineEditorForm->TheTree, AStreamPtr.get(), false, true);

						this->SetStrValue(AStreamPtr->DataString);

						std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
						SaveTreeToStream(AStateMachineEditorForm->TheTree, AMemoryStream.get());

						for (int i = 0; i < this->PropCount; i++) {
							TSyntaxString *ASyntaxString = dynamic_cast<TSyntaxString*>(this->GetInstance(i));
							if (ASyntaxString) {
								ASyntaxString->FScxmlStream->Clear();
								AMemoryStream->SaveToStream(ASyntaxString->FScxmlStream);
							}
						}
					}
				}
				__finally {
					// bugfix: почему-то уходит на задний план
					Application->BringToFront();
				}
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}

}
