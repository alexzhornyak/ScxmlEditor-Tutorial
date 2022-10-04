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

#include "UnitDialogRegExp.h"

#include <boost/regex.hpp>

#include "Log4cpp_VCL.hpp"

#include "UnitPropInspectorExtensions.h"
#include "UnitSettings.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDialogRegExp *DialogRegExp = NULL;

//---------------------------------------------------------------------------
__fastcall TDialogRegExp::TDialogRegExp(TComponent* Owner) : TForm(Owner) {
	if (SettingsData) {
		MemoText->Lines->BeginUpdate();
		MemoText->Lines->Text = SettingsData->TempRegistry->Values[this->Name];
		MemoText->Lines->EndUpdate();
	}

	this->SetRegExecType(retRegexMatch);
}

//---------------------------------------------------------------------------
void __fastcall TDialogRegExp::SetRegExecType(const TRegexExecuteType val) {
	FRegExecType = val;

	this->LabelRegularExpression->Caption = L"Regular expression";

	switch(FRegExecType) {
	case retRegexMatch: {
			this->LabelRegularExpression->Caption = this->LabelRegularExpression->Caption + L" (boost::regex_match)";
		}break;
	case retRegexSearch: {
			this->LabelRegularExpression->Caption = this->LabelRegularExpression->Caption + L" (boost::regex_search)";
		}break;
	}
}

//---------------------------------------------------------------------------
bool TDialogRegExp::Validate() {
	MemoGroups->Clear();
	try {
		if (!EditRegExp->Text.Trim().IsEmpty()) {
			boost::wregex e = CheckBox1->Checked ? boost::wregex(EditRegExp->Text.Trim().c_str()) : boost::wregex
			(EditRegExp->Text.Trim().c_str(), boost::regex::icase);
			boost::wsmatch what;

			std::wstring wstr = MemoText->Lines->Text.Trim().c_str();
			if (FRegExecType == retRegexMatch ? boost::regex_match(wstr, what, e) : boost::regex_search(wstr, what, e)) {

				MemoGroups->Lines->Add("Success!");

				std::auto_ptr<TStringList>AStringListPtr(new TStringList());
				for (std::size_t i = 0; i < what.size(); i++) {
					MemoGroups->Lines->Add(UnicodeString().sprintf(L"Group %d>%s", i, std::wstring(what[i]).c_str()));
				}

				return true;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	catch(std::exception & e) {
		LOG_ERROR("%s> %s", __FUNCTION__, e.what());
	}

	MemoGroups->Lines->Add("Fail!");
	return false;
}

//---------------------------------------------------------------------------
void __fastcall TDialogRegExp::BtnValidateClick(TObject *Sender) {
	Validate();
}

//---------------------------------------------------------------------------
void __fastcall TDialogRegExp::FormClose(TObject *Sender, TCloseAction &Action) {
	if (SettingsData) {
		SettingsData->TempRegistry->Values[this->Name] = MemoText->Lines->Text;
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogRegExp::EditRegExpKeyPress(TObject *Sender, wchar_t &Key) {
	switch(Key) {
	case VK_RETURN:
		Key = 0;
		this->ModalResult = mrOk;
		break;
	}
}

//---------------------------------------------------------------------------
//------------------- TRegExpStringDialogPropEditor -------------------------
//---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TRegExpStringDialogPropEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

//---------------------------------------------------------------------------
void __fastcall TRegExpStringDialogPropEditor::Edit(void) {
	try {
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			const UnicodeString sValue = this->GetStrValue(0);
			std::auto_ptr<TDialogRegExp>ADialogPtr(new TDialogRegExp(Application));
			ADialogPtr->EditRegExp->Text = sValue;
			if (ADialogPtr->ShowModal() == mrOk) {
				this->SetStrValue(ADialogPtr->EditRegExp->Text.Trim());
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
