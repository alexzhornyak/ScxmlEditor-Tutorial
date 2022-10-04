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

#ifndef UnitStringPairsEditorH
#define UnitStringPairsEditorH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDGrid.hpp"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>

// ---------------------------------------------------------------------------
class TFormStringPairsEditor : public TForm {
__published: // IDE-managed Components
	TPanel *Panel1;
	TButton *BtnCancel;
	TButton *BtnOk;
	TImageList *ImageList1;
	TButton *BtnDeleteRow;
	TButton *BtnAddRow;
	TPageControl *PageControl1;
	TTabSheet *TabSheetTable;
	TTabSheet *TabSheetMemo;
	TMemo *MemoSource;
	TLMDGrid *GridPairs;
	TLMDGridTextColumn *LMDGridTextColumn1;
	TLMDGridTextColumn *LMDGridTextColumn2;
	void __fastcall BtnAddRowClick(TObject *Sender);
	void __fastcall BtnDeleteRowClick(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);




private: // User declarations

public: // User declarations
	__fastcall TFormStringPairsEditor(TComponent* Owner);

	void __fastcall AssignPairStrings(TStrings *ALines);

	void __fastcall AssignPairText(const UnicodeString &sText);
};

// ---------------------------------------------------------------------------

#endif
