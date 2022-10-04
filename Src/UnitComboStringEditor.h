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

#ifndef UniTComboStringEditorH
#define UniTComboStringEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "ElListBox.hpp"
#include "ElXPThemedControl.hpp"
#include "HTMLLbx.hpp"
//---------------------------------------------------------------------------
class TComboStringEditor : public TForm
{
__published:	// IDE-managed Components
	TPanel *sef_Panel;
	TButton *sef_BtnCncl;
	TButton *sef_BtnOk;
	TPanel *Panel1;
	TEdit *Edit1;
	TButton *Button1;
	TElHTMLListBox *ListBox1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Edit1KeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall FormCanResize(TObject *Sender, int &NewWidth, int &NewHeight, bool &Resize);
	void __fastcall Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Edit1Change(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ListBox1DblClick(TObject *Sender);
	void __fastcall ListBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ListBox1KeyPress(TObject *Sender, wchar_t &Key);
private:	// User declarations

	TStringList *FLines;

    UnicodeString FOriginalCaption;

public:		// User declarations
	__fastcall TComboStringEditor(TComponent* Owner);

	void __fastcall AppendLine(const UnicodeString &sLine);

	void __fastcall SetEditText(const UnicodeString &sText);

	__property TStringList *Lines = {read=FLines, write=FLines};
};
//---------------------------------------------------------------------------
extern PACKAGE TComboStringEditor *ComboStringEditor;
//---------------------------------------------------------------------------
#endif
