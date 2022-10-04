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

#include "UniTComboStringEditor.h"

#include "Log4cpp_VCL.hpp"

#include <StrUtils.hpp>

#include "UnitSettings.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "HTMLLbx"
#pragma resource "*.dfm"
TComboStringEditor *ComboStringEditor = NULL;

//---------------------------------------------------------------------------
__fastcall TComboStringEditor::TComboStringEditor(TComponent* Owner) : TForm(Owner), FLines(NULL), FOriginalCaption(L"") {
	try {
		this->Width = SettingsData->TempRegistry->Values[this->Name + ".Width"].ToIntDef(this->Width);
		this->Height = SettingsData->TempRegistry->Values[this->Name + ".Height"].ToIntDef(this->Height);

		ListBox1->Hint = "Press 'Enter' or 'Dbl Click' to select\n" "Press 'Ctrl + Enter' to apply";
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::FormClose(TObject *Sender, TCloseAction &Action) {
	try {
		SettingsData->TempRegistry->Values[this->Name + ".Width"] = this->Width;
		SettingsData->TempRegistry->Values[this->Name + ".Height"] = this->Height;
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::AppendLine(const UnicodeString &sLine) {
	FLines->Add(sLine);
}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::Edit1KeyPress(TObject *Sender, wchar_t &Key) {
	switch(Key) {
	case VK_RETURN:
		Key = 0;
		this->ModalResult = mrOk;
		break;
	}
}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::FormCanResize(TObject *Sender, int &NewWidth, int &NewHeight, bool &Resize) {

	const int iCaptionHeight = this->Height -this->ClientHeight;

	if ((NewHeight - iCaptionHeight) < 80) {
		NewHeight = 80 + iCaptionHeight;
	}
}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
	switch(Key) {
	case VK_RETURN: {
			if (Shift.Contains(ssCtrl)) {
				Key = 0;
				this->ModalResult = mrOk;
			}
		}break;
	default: ;
	}
}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::Edit1Change(TObject *Sender) {
	if (Edit1->Tag == 0) {
		ListBox1->Items->Clear();

		const UnicodeString sText = Edit1->Text;

		ListBox1->Items->BeginUpdate();

		for (int i = 0; i < FLines->Count; i++) {
			UnicodeString sNodeText = FLines->Strings[i];

			bool bAdd = false;

			if (!sText.IsEmpty()) {
				const int iMatchPos = sNodeText.UpperCase().Pos(sText.UpperCase());
				const int iMatchLength = sText.Length();

				if (iMatchPos) {
					const UnicodeString sReplacement = sNodeText.SubString(iMatchPos, iMatchLength);
					sNodeText = StringReplace(sNodeText, sReplacement, "<b>" + sReplacement + "</b>", TReplaceFlags());
					bAdd = true;
				}
			}
			else {
				bAdd = true;
			}

			if (bAdd) {
				ListBox1->Items->AddObject(sNodeText, reinterpret_cast<TObject*>(i));
			}
		}

		ListBox1->Items->EndUpdate();

		if (FOriginalCaption.IsEmpty()) {
			FOriginalCaption = this->Caption;
		}

		if (FLines->Count != ListBox1->Items->Count) {
			this->Caption = UnicodeString().sprintf(L"%s: %d of %d", FOriginalCaption.c_str(), ListBox1->Items->Count, FLines->Count);
		}
		else {
			this->Caption = FOriginalCaption;
		}
	}

}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::SetEditText(const UnicodeString &sText) {
	if (Edit1->Text == sText) {
		Edit1Change(Edit1);
	}
	else {
		Edit1->Text = sText;
	}
}

//---------------------------------------------------------------------------

void __fastcall TComboStringEditor::FormCreate(TObject *Sender) {
	FLines = new TStringList();
	FLines->Sorted = true;
	FLines->Duplicates = dupIgnore;
}
//---------------------------------------------------------------------------

void __fastcall TComboStringEditor::FormDestroy(TObject *Sender) {
	if (FLines) {
		delete FLines;
		FLines = 0;
	}
}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::Button1Click(TObject *Sender) {
	Edit1->Text = L"";
}

//---------------------------------------------------------------------------
void __fastcall TComboStringEditor::ListBox1DblClick(TObject *Sender) {
	if (ListBox1->ItemIndex != -1) {
		try {
			const int iRow = reinterpret_cast<int>(ListBox1->Items->Objects[ListBox1->ItemIndex]);
			Edit1->Tag = 1;
			Edit1->Text = FLines->Strings[iRow];
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
		Edit1->Tag = 0;
	}
}
//---------------------------------------------------------------------------

void __fastcall TComboStringEditor::ListBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
	switch(Key) {
	case VK_RETURN: {
			if (Shift.Contains(ssCtrl)) {
				Key = 0;

				ListBox1DblClick(Sender);

				this->ModalResult = mrOk;
			}
		}break;
	default: ;
	}
}
//---------------------------------------------------------------------------

void __fastcall TComboStringEditor::ListBox1KeyPress(TObject *Sender, wchar_t &Key) {
	if (Key == VK_RETURN) {
		ListBox1DblClick(Sender);
	}
}
//---------------------------------------------------------------------------
