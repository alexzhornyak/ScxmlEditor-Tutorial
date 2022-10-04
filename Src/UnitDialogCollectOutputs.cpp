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

#include "UnitDialogCollectOutputs.h"

#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDialogCollectOutputs *DialogCollectOutputs = 0;

//---------------------------------------------------------------------------
__fastcall TDialogCollectOutputs::TDialogCollectOutputs(TComponent* Owner) : TForm(Owner) {
}

//---------------------------------------------------------------------------
void __fastcall TDialogCollectOutputs::Clone1Click(TObject *Sender) {
	if (ListViewOutputs->Selected) {
		TListItem *AItem = this->ListViewOutputs->Items->Insert(ListViewOutputs->Selected->Index + 1);
		AItem->Checked = ListViewOutputs->Selected->Checked;

		AItem->Caption = ListViewOutputs->Selected->Caption;
		AItem->SubItems->Add(ListViewOutputs->Selected->SubItems->Strings[0]);

		ListViewOutputs->ClearSelection();
		AItem->Selected = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogCollectOutputs::FormCreate(TObject *Sender) {
	try {
		this->CheckComment->Checked = StrToBoolDef(SettingsData->TempRegistry->Values["DialogCollectOutputs.CheckComment.Checked"],
			this->CheckComment->Checked);
		this->CheckWeakBinding->Checked = StrToBoolDef(SettingsData->TempRegistry->Values["DialogCollectOutputs.CheckWeakBinding.Checked"],
			this->CheckWeakBinding->Checked);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

}
//---------------------------------------------------------------------------

void __fastcall TDialogCollectOutputs::Button1Click(TObject *Sender) {
	try {
		SettingsData->TempRegistry->Values["DialogCollectOutputs.CheckComment.Checked"] = BoolToStr(this->CheckComment->Checked, true);
		SettingsData->TempRegistry->Values["DialogCollectOutputs.CheckWeakBinding.Checked"] = BoolToStr(this->CheckWeakBinding->Checked,
			true);

		SettingsData->Save();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
//---------------------------------------------------------------------------
