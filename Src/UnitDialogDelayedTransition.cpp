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

#include "UnitDialogDelayedTransition.h"
#include "KeyboardUtils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDialogDelayedTransition *DialogDelayedTransition = NULL;

//---------------------------------------------------------------------------
__fastcall TDialogDelayedTransition::TDialogDelayedTransition(TComponent* Owner) : TForm(Owner) {
}

//---------------------------------------------------------------------------
void __fastcall TDialogDelayedTransition::EditDelayChange(TObject *Sender) {
	if (!EditDelay->Tag) {
		EditDelayExpr->Tag = 1;

		EditDelayExpr->Text = L"";

		EditDelayExpr->Tag = 0;
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogDelayedTransition::EditDelayExprChange(TObject *Sender) {
	if (!EditDelayExpr->Tag) {
		EditDelay->Tag = 1;

		EditDelay->Text = L"";

		EditDelay->Tag = 0;
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogDelayedTransition::FormCloseQuery(TObject *Sender, bool &CanClose) {
	if (ModalResult == mrOk) {

		const UnicodeString Caption = L"ERROR";
		UnicodeString Text = L"";

		if (EditEvent->Text.Trim().IsEmpty()) {
			Text = L"'Event' must not be empty!";
		}
		else if (EditDelay->Text.Trim().IsEmpty() && EditDelayExpr->Text.Trim().IsEmpty()) {
			Text = L"'Delay' must not be empty!";
		}

		if (!Text.IsEmpty()) {
			MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OK | MB_ICONERROR);
			CanClose = false;
		}
	}
}

//---------------------------------------------------------------------------

void __fastcall TDialogDelayedTransition::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
	if (Shift.Contains(ssCtrl) && Key == VK_RETURN) {
		this->ModalResult = mrOk;
	}
}
//---------------------------------------------------------------------------
