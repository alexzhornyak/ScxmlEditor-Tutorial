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

#ifndef UnitMacroComposerH
#define UnitMacroComposerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDSedView.hpp"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "LMDSedDocument.hpp"

class IPostBuildInterface;

//---------------------------------------------------------------------------
class TMacroComposer : public TForm
{
__published:	// IDE-managed Components
	TLMDEditView *LMDEditView1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLMDEditView *LMDEditView2;
	TTabControl *TabControl1;
	TLMDEditView *EditView;
	TSplitter *Splitter1;
	TSplitter *Splitter2;
	TPanel *Panel1;
	TButton *Button1;
	TButton *Button2;
	TLMDEditDocument *EditDocOriginal;
	TLMDEditDocument *EditDocEvaluated;
	TLMDEditDocument *EditDocSystem;
	TLMDEditDocument *EditDocUser;
	void __fastcall TabControl1Change(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall EditDocOriginalDocChanged(TLMDEditDocument *Sender, TLMDDocStatusChanges AChanges);
	void __fastcall EditViewFillCompletionList(TObject *Sender, TStrings *Items, TPoint &CursorPoint, TLMDEditFillCompletionCause Cause, const TLMDString LeftPart,
          TPoint &PosForCombo, bool &Handled);
	void __fastcall EditViewCompletionReplace(TObject *Sender, TStrings *Items, int ItemIndex, TPoint &CursorPoint, TLMDEditReplaceCompletionCause Cause,
          bool &Handled);
	void __fastcall EditViewKeyPressW(TObject *Sender, wchar_t &Key);

private:	// User declarations
	wchar_t FLastPressedKey;

	void __fastcall DoCreateInstance(void);

public:		// User declarations
	__fastcall TMacroComposer(TComponent* Owner, const IPostBuildInterface *APostBuildInterface);
	__fastcall TMacroComposer(TComponent* Owner);
};

//---------------------------------------------------------------------------
extern PACKAGE TMacroComposer *MacroComposer;
//---------------------------------------------------------------------------
#endif
