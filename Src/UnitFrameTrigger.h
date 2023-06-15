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

#ifndef UnitFrameTriggerH
#define UnitFrameTriggerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Mask.hpp>

#include "UnitTriggers.h"
#include "ElBtnEdit.hpp"
#include "ElEdits.hpp"
#include "ElXPThemedControl.hpp"
#include <ExtCtrls.hpp>

namespace Ujson {
	class myJSONItem;
}

//---------------------------------------------------------------------------
class TFrameTrigger : public TFrame
{
__published:	// IDE-managed Components
	TButton *Button1;
	TCheckBox *CheckBox1;
	TTrackBar *TrackBar1;
	TLabel *LabelTrack;
	TLabel *LabelParams;
	TScrollBox *ScrollBoxParams;
	TElButtonEdit *EditBtn1;
	TLabel *LabelEdit;
	TComboBox *ComboBox1;
	TLabel *LabelCombo;
	TPanel *PanelMemo;
	TLabel *LabelMemo;
	TButton *BtnMemo;
	TMemo *Memo1;
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall TrackBar1Change(TObject *Sender);
	void __fastcall EditBtnKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall Button1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Button1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall EditBtn1Change(TObject *Sender);
	void __fastcall EditBtn1ButtonClick(TObject *Sender);
	void __fastcall BtnMemoClick(TObject *Sender);
	void __fastcall Memo1Change(TObject *Sender);
	void __fastcall Memo1KeyPress(TObject *Sender, wchar_t &Key);
private:	// User declarations

	/* те, которые были на момент создания */
	/* их нельзя менять */
	TContentTrigger *FOriginalContentTrigger;
	TParamTriggerItems *FOriginalParams;

	/* эти будут подстраиваться во время работы */
	TContentTrigger *FContentTrigger;
	TParamTriggerItems *FParams;

	UnicodeString FEventName;
	void __fastcall CreateSendText(void);
	double Precision;

    inline bool IsOnOffDeclared() { return !FContentTrigger->On.IsEmpty() && !FContentTrigger->Off.IsEmpty(); }

	Ujson::myJSONItem * __fastcall AssignJSONTriggerValue(Ujson::myJSONItem *AObj, const UnicodeString &sKey, const UnicodeString &sExpr,
											const TContentTriggerType ATriggerType);

	void __fastcall ValidateXMLTriggerValueThrow(const UnicodeString &sExpr, const TContentTriggerType ATriggerType);

	void __fastcall ScrollBoxParamsClickBtn(TObject *Sender);

	/* used for On-Off events */
	void __fastcall DoSendButtonDown(void);
	void __fastcall DoSendButtonUp(void);

public:		// User declarations
	// Event имя должно передаваться отдельно
	__fastcall TFrameTrigger(TComponent* Owner, TContentTrigger *AContentTrigger, TParamTriggerItems *AParams, const UnicodeString &sEventName);
	virtual __fastcall ~TFrameTrigger();

	bool __fastcall SendUniversalEvent(void);

	__property UnicodeString 			EventName = { read=FEventName };

	__property TContentTrigger *		OriginalContentTrigger = { read=FOriginalContentTrigger };
	__property TParamTriggerItems *		OriginalParams = { read=FOriginalParams };
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameTrigger *FrameTrigger;

//---------------------------------------------------------------------------
#endif
