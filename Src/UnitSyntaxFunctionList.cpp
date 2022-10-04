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

#include "UnitSyntaxFunctionList.h"

#include <LMDUnicodeControl.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

__fastcall TSyntaxFunctionsList::TSyntaxFunctionsList(Classes::TComponent* aOwner) : TElHTMLListBox(aOwner) {

	ControlStyle = ControlStyle << csNoDesignVisible;
	ParentColor = False;

	Sorted = false;
	WordWrap = true;
	//ScrollHorizontal = true;
	//ExtendedSelect = false;
}

//---------------------------------------------------------------------------
void __fastcall TSyntaxFunctionsList::CreateParams(Controls::TCreateParams &Params) {
	/* inherited */
	TElHTMLListBox::CreateParams(Params);

	Params.Style = Params.Style | WS_BORDER;
	Params.ExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
	AddBiDiModeExStyle(Params.ExStyle);
	Params.WindowClass.style = CS_SAVEBITS;
}

//---------------------------------------------------------------------------

void __fastcall TSyntaxFunctionsList::CreateWnd(void) {
	/* inherited */
	TElHTMLListBox::CreateWnd();

	::SetParent(this->Handle, 0);
}

//---------------------------------------------------------------------------
void __fastcall TSyntaxFunctionsList::WMMouseActivate(Messages::TWMMouseActivate &Message) {
	Message.Result = MA_NOACTIVATE;
}

//---------------------------------------------------------------------------
void __fastcall TSyntaxFunctionsList::WndProc(Messages::TMessage &Message) {
	switch(Message.Msg) {
	case WM_MOUSEACTIVATE:
		Message.Result = MA_NOACTIVATE;
		break;
	default:
		TElHTMLListBox::WndProc(Message);
	}
}

//---------------------------------------------------------------------------
void __fastcall TSyntaxFunctionsList::CreateWindowHandle(const TCreateParams &Params) {
	LMDCreateControlHandle(this, Params, L"LISTBOX");
}

//---------------------------------------------------------------------------
void __fastcall TSyntaxFunctionsList::Show(bool AVisible, int X, int Y) {
	if (AVisible) {
		SetWindowPos(Handle, HWND_TOP, X, Y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	}
	else {
		SetWindowPos(Handle, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_HIDEWINDOW);
	}
}
//---------------------------------------------------------------------------
