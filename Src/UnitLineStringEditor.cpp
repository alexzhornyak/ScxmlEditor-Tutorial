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

#include "UnitLineStringEditor.h"

#include "Log4cpp_VCL.hpp"

#include "UnitSettings.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLineStringEditor *LineStringEditor = NULL;

//---------------------------------------------------------------------------
__fastcall TLineStringEditor::TLineStringEditor(TComponent* Owner) : TForm(Owner) {
	try {
		this->Width = SettingsData->TempRegistry->Values[this->Name + ".Width"].ToIntDef(this->Width);
		this->Height = SettingsData->TempRegistry->Values[this->Name + ".Height"].ToIntDef(this->Height);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TLineStringEditor::FormClose(TObject *Sender, TCloseAction &Action) {
	try {
		SettingsData->TempRegistry->Values[this->Name + ".Width"] = this->Width;
		SettingsData->TempRegistry->Values[this->Name + ".Height"] = this->Height;
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TLineStringEditor::Edit1KeyPress(TObject *Sender, wchar_t &Key) {
	switch(Key) {
	case VK_RETURN:
		Key = 0;
		this->ModalResult = mrOk;
		break;
	}
}

//---------------------------------------------------------------------------
void __fastcall TLineStringEditor::FormCanResize(TObject *Sender, int &NewWidth, int &NewHeight, bool &Resize) {

	const int iCaptionHeight = this->Height -this->ClientHeight;

	if ((NewHeight - iCaptionHeight) < 80) {
		NewHeight = 80 + iCaptionHeight;
	}
}

//---------------------------------------------------------------------------
void __fastcall TLineStringEditor::Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
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
