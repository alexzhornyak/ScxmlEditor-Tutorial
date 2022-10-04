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

#ifndef UnitSearchFormH
#define UnitSearchFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "ElHTMLLbl.hpp"
#include "ElXPThemedControl.hpp"
#include "ElCLabel.hpp"
#include <vector>
#include "CommonConsts.h"

namespace Lmdideprojmgr {
	class TLMDProjDocument;
};

class TStateMachineEditorUnit;

//---------------------------------------------------------------------------
class TFormSearch : public TForm {
__published: // IDE-managed Components
	TButton *BtnCancel;
	TEdit *EditSearch;
	TButton *BtnNext;
	TLabel *Label1;
	TLabel *LabelInfo;
	TElHTMLLabel *HTMLSearchAddInfo;
	TGroupBox *GroupBox1;
	TCheckBox *CheckTrim;
	TCheckBox *CheckMatchCase;
	TCheckBox *CheckBoxWholeWord;
	TGroupBox *GroupBox2;
	TCheckBox *CheckCloseTabIfNotFound;
	TGroupBox *GroupBox3;
	TCheckBox *CheckStateText;
	TCheckBox *CheckTransitionText;
	TCheckBox *CheckAllProperties;
	TButton *BtnDefault;
	TGroupBox *GroupBox5;
	TRadioButton *RadioDocTypeOpened;
	TRadioButton *RadioDocTypeCurrent;
	TRadioButton *RadioDocTypeWholeProject;
	TGroupBox *GroupBox6;
	TRadioButton *RadioModeSelectedStates;
	TRadioButton *RadioModeNormal;
	TPanel *Panel1;
	TCheckBox *CheckUseRegExp;
	TButton *BtnRegExpValidate;
	TCheckBox *CheckShowResultsInSeparateWindow;

	void __fastcall EditSearchChange(TObject *Sender);
	void __fastcall EditSearchKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BtnCancelClick(TObject *Sender);
	void __fastcall BtnNextClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall RadioGroupDocTypeClick(TObject *Sender);
	void __fastcall BtnDefaultClick(TObject *Sender);
	void __fastcall OnChangeValue(TObject *Sender);
	void __fastcall BtnRegExpValidateClick(TObject *Sender);
	void __fastcall RadioModeSelectedStatesClick(TObject *Sender);
	void __fastcall CheckShowResultsInSeparateWindowClick(TObject *Sender);

private: // User declarations

	int m_iDockPanel;
	std::size_t m_iUnit;
	bool m_b_FromBeginning;

	std::map<TComponent *, Variant> FDefaultValues;

public: // User declarations
	__fastcall TFormSearch(TComponent* Owner);

	TNotifyEvent FOnSearchTextChanged;

	TEditorSearchTypes ToSearchTypes();
	void FromSearchTypes(const TEditorSearchTypes ATypes);

	TEditorSearchDocumentType GetEditorSearchType(void);
	void SetEditorSearchType(const TEditorSearchDocumentType val);
};

//---------------------------------------------------------------------------
bool StrMatchWhat(const UnicodeString &sOriginalText, const UnicodeString &sFindText, const TEditorSearchTypes ATypes,
	TMatchFind &sMatchOut);

//---------------------------------------------------------------------------
extern PACKAGE TFormSearch *FormSearch;
//---------------------------------------------------------------------------
#endif
