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

#include "UnitDialogQScxmlComiler.h"

#include "UnitSettings.h"
#include "UnitStringsEditor.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDialogQScxmlCompiler *DialogQScxmlCompiler;

//---------------------------------------------------------------------------
__fastcall TDialogQScxmlCompiler::TDialogQScxmlCompiler(TComponent* Owner) : TForm(Owner) {
	if (SettingsData) {
		for (int i = 0; i < this->ComponentCount; i++) {
			TComboBox * AComboBox = dynamic_cast<TComboBox*>(this->Components[i]);
			if (AComboBox) {
				SettingsData->LoadComboBoxDataFromRegistry(AComboBox);
				continue;
			}
			TCheckBox * ACheckBox = dynamic_cast<TCheckBox*>(this->Components[i]);
			if (ACheckBox) {
				SettingsData->LoadCheckBoxDataFromRegistry(ACheckBox);
				continue;
			}
		}
	}

}

//---------------------------------------------------------------------------
void __fastcall TDialogQScxmlCompiler::BtnOkClick(TObject *Sender) {
	if (SettingsData) {
		for (int i = 0; i < this->ComponentCount; i++) {
			TComboBox * AComboBox = dynamic_cast<TComboBox*>(this->Components[i]);
			if (AComboBox) {
				SettingsData->SaveComboBoxDataToRegistry(AComboBox);
				continue;
			}
			TCheckBox * ACheckBox = dynamic_cast<TCheckBox*>(this->Components[i]);
			if (ACheckBox) {
				SettingsData->SaveCheckBoxDataToRegistry(ACheckBox);
				continue;
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogQScxmlCompiler::BtnEditClick(TObject *Sender) {
	TButton * AButton = dynamic_cast<TButton*>(Sender);
	if (AButton) {
		for (int i = 0; i < AButton->Parent->ControlCount; i++) {
			TComboBox * AComboBox = dynamic_cast<TComboBox*>(AButton->Parent->Controls[i]);
			if (AComboBox) {
				std::auto_ptr<TStringEditorForm>AStringListPtr(new TStringEditorForm(this));
				AStringListPtr->sef_Memo->Lines->Text = AComboBox->Items->Text.Trim();
				if (AStringListPtr->ShowModal() == mrOk) {
					AComboBox->Items->Text = AStringListPtr->sef_Memo->Lines->Text;
				}
				break;
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogQScxmlCompiler::CheckNamespaceClick(TObject *Sender) {
	TCheckBox * ACheckBox = dynamic_cast<TCheckBox*>(Sender);
	if (ACheckBox) {
		TPanel * APanel = dynamic_cast<TPanel*>(ACheckBox->Parent);
		if (APanel) {
			APanel->Font->Color = ACheckBox->Checked ? clWindowText : clHighlight;
		}
	}
}
//---------------------------------------------------------------------------
