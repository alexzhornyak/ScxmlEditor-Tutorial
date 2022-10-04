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

#ifndef UnitDialogQScxmlComilerH
#define UnitDialogQScxmlComilerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TDialogQScxmlCompiler : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TLabel *Label1;
	TComboBox *ComboNamespace;
	TLabel *Label2;
	TCheckBox *CheckNamespace;
	TPanel *Panel2;
	TLabel *Label3;
	TLabel *Label4;
	TComboBox *ComboOutName;
	TPanel *Panel3;
	TLabel *Label5;
	TLabel *Label6;
	TComboBox *ComboClassName;
	TCheckBox *CheckClassName;
	TPanel *Panel4;
	TLabel *Label7;
	TLabel *Label8;
	TComboBox *ComboImpl;
	TCheckBox *CheckImpl;
	TPanel *Panel5;
	TLabel *Label9;
	TLabel *Label10;
	TComboBox *ComboHeader;
	TCheckBox *CheckHeader;
	TPanel *Panel6;
	TLabel *Label11;
	TLabel *Label12;
	TCheckBox *CheckStateMethods;
	TPanel *Panel7;
	TButton *BtnOk;
	TButton *BtnCancel;
	TButton *Button1;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	TButton *Button5;
	TButton *BtnEdit;
	void __fastcall BtnOkClick(TObject *Sender);
	void __fastcall BtnEditClick(TObject *Sender);
	void __fastcall CheckNamespaceClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TDialogQScxmlCompiler(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDialogQScxmlCompiler *DialogQScxmlCompiler;
//---------------------------------------------------------------------------
#endif
