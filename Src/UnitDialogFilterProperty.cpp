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

#include "UnitDialogFilterProperty.h"

#include "UnitSettings.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UnitDialogCollectProperty"
#pragma resource "*.dfm"
TDialogFilterProperty *DialogFilterProperty = NULL;

//---------------------------------------------------------------------------
__fastcall TDialogFilterProperty::TDialogFilterProperty(TComponent* Owner) : TDialogCopyProperty(Owner) {

	{
		const int iItemIndex = SettingsData->TempRegistry->Values[this->Name + ".ComboFilterCondition.Index"].ToIntDef
		(this->ComboFilterCondition->ItemIndex);
		if (iItemIndex >= 0 && iItemIndex < this->ComboFilterCondition->Items->Count) {
			this->ComboFilterCondition->ItemIndex = iItemIndex;
		}
	}

	{
		this->ComboCompare->Items->CommaText = SettingsData->TempRegistry->Values[this->Name + ".ComboCompare.Items"];
		const int iItemIndex = SettingsData->TempRegistry->Values[this->Name + ".ComboCompare.Index"].ToIntDef
		(this->ComboCompare->ItemIndex);
		if (iItemIndex >= 0 && iItemIndex < this->ComboCompare->Items->Count) {
			this->ComboCompare->ItemIndex = iItemIndex;
		}
	}

	CheckCaseSensitive->Checked = StrToBoolDef(SettingsData->TempRegistry->Values[this->Name + ".CheckCaseSensitive"],
		CheckCaseSensitive->Checked);
}

//---------------------------------------------------------------------------
bool __fastcall TDialogFilterProperty::SaveSettings() {
	bool bNeedToSave = TDialogCopyProperty::SaveSettings();

	if (SettingsData->TempRegistry->Values[this->Name + ".ComboFilterCondition.Index"].ToIntDef(this->ComboFilterCondition->ItemIndex)
		!= this->ComboFilterCondition->ItemIndex) {
		SettingsData->TempRegistry->Values[this->Name + ".ComboFilterCondition.Index"] = this->ComboFilterCondition->ItemIndex;
		bNeedToSave = true;
	}

	if (this->ComboCompare->Items->IndexOf(this->ComboCompare->Text) == -1) {
		this->ComboCompare->Items->Insert(0, this->ComboCompare->Text);
		SettingsData->TempRegistry->Values[this->Name + ".ComboCompare.Items"] = this->ComboCompare->Items->CommaText;
		bNeedToSave = true;
	}
	if (SettingsData->TempRegistry->Values[this->Name + ".ComboCompare.Index"].ToIntDef(this->ComboCompare->ItemIndex)
		!= this->ComboCompare->ItemIndex) {
		SettingsData->TempRegistry->Values[this->Name + ".ComboCompare.Index"] = this->ComboCompare->ItemIndex;
		bNeedToSave = true;
	}
	if (StrToBoolDef(SettingsData->TempRegistry->Values[this->Name + ".CheckCaseSensitive"],
			CheckCaseSensitive->Checked) != CheckCaseSensitive->Checked) {
		SettingsData->TempRegistry->Values[this->Name + ".CheckCaseSensitive"] = BoolToStr(CheckCaseSensitive->Checked);
		bNeedToSave = true;
	}

	return bNeedToSave;
}
//---------------------------------------------------------------------------
