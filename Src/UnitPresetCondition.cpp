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

#include "UnitPresetCondition.h"

#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPresetCondition *PresetCondition = 0;

//---------------------------------------------------------------------------
__fastcall TPresetCondition::TPresetCondition(TComponent* Owner) : TForm(Owner) {
}

//---------------------------------------------------------------------------
void __fastcall TPresetCondition::FormCreate(TObject *Sender) {
	try {
		this->CheckAutoLayout->Checked = StrToBoolDef(SettingsData->TempRegistry->Values["PresetCondition.CheckAutoLayout.Checked"],
			this->CheckAutoLayout->Checked);
		if (!SettingsData->TempRegistry->Values["PresetCondition.EditScxmlEvent.Text"].IsEmpty()) {
			this->EditScxmlEvent->Text = SettingsData->TempRegistry->Values["PresetCondition.EditScxmlEvent.Text"];
		}
		if (!SettingsData->TempRegistry->Values["PresetCondition.EditStateName.Text"].IsEmpty()) {
			this->EditStateName->Text = SettingsData->TempRegistry->Values["PresetCondition.EditStateName.Text"];
		}
		EditCasesCount->Text = SettingsData->TempRegistry->Values["PresetCondition.EditCasesCount.Text"].ToIntDef(2);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
//---------------------------------------------------------------------------

void __fastcall TPresetCondition::BtnOKClick(TObject *Sender) {
	try {
		SettingsData->TempRegistry->Values["PresetCondition.CheckAutoLayout.Checked"] = BoolToStr(this->CheckAutoLayout->Checked, true);
		SettingsData->TempRegistry->Values["PresetCondition.EditScxmlEvent.Text"] = this->EditScxmlEvent->Text;
		SettingsData->TempRegistry->Values["PresetCondition.EditStateName.Text"] = this->EditStateName->Text;
		SettingsData->TempRegistry->Values["PresetCondition.EditCasesCount.Text"] = this->EditCasesCount->Text;

		SettingsData->Save();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
//---------------------------------------------------------------------------
