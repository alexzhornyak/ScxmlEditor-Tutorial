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

#ifndef UnitDialogRegExpH
#define UnitDialogRegExpH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include "LMDInsPropEditors.hpp"

typedef enum {retRegexMatch, retRegexSearch} TRegexExecuteType;

//---------------------------------------------------------------------------
class TDialogRegExp : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TEdit *EditRegExp;
	TMemo *MemoText;
	TMemo *MemoGroups;
	TPanel *Panel2;
	TButton *Button1;
	TButton *Button2;
	TButton *BtnValidate;
	TImageList *ImageList1;
	TLabel *Label2;
	TPanel *Panel3;
	TLabel *LabelRegularExpression;
	TCheckBox *CheckBox1;
	void __fastcall BtnValidateClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall EditRegExpKeyPress(TObject *Sender, wchar_t &Key);
private:	// User declarations

	TRegexExecuteType FRegExecType;

public:		// User declarations

	__fastcall TDialogRegExp(TComponent* Owner);

	bool Validate();

	void __fastcall SetRegExecType(const TRegexExecuteType val);

};

/* PROP EDITORS */
class TRegExpStringDialogPropEditor : public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();

	virtual void __fastcall Edit(void);
};

//---------------------------------------------------------------------------
extern PACKAGE TDialogRegExp *DialogRegExp;
//---------------------------------------------------------------------------
#endif
