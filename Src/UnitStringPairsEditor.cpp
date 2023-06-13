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

#include <memory>

#include "Log4cpp_VCL.hpp"

#include "UnitStringPairsEditor.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDGrid"
#pragma resource "*.dfm"

namespace Pairseditor {
	typedef enum {
		ctKey, ctValue
	}TCellType;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ------------------------ TFormStringPairsEditor ---------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TFormStringPairsEditor::TFormStringPairsEditor(TComponent* Owner) : TForm(Owner) {

}

// ---------------------------------------------------------------------------
void __fastcall TFormStringPairsEditor::BtnAddRowClick(TObject *Sender) {
	GridPairs->DataRowCount = GridPairs->DataRowCount + 1;
}
// ---------------------------------------------------------------------------

void __fastcall TFormStringPairsEditor::BtnDeleteRowClick(TObject *Sender) {
	if (GridPairs->CurrentRow != -1) {
		const int iRow = GridPairs->CurrentRow;
		GridPairs->DeleteDataRow(GridPairs->CurrentRow);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormStringPairsEditor::PageControl1Change(TObject *Sender) {
	try {
		switch(PageControl1->ActivePageIndex) {
		case 0: {

				GridPairs->DataRowCount = MemoSource->Lines->Count;

				for (int i = 0; i < MemoSource->Lines->Count; i++) {
					GridPairs->Cells[Pairseditor::ctKey][i] = MemoSource->Lines->Names[i];
					GridPairs->Cells[Pairseditor::ctValue][i] = MemoSource->Lines->ValueFromIndex[i];
				}

			}break;
		case 1: {

				MemoSource->Clear();

				for (int i = 0; i < GridPairs->DataRowCount; i++) {
					UnicodeString sKey = L"";
					try {
						sKey = GridPairs->Cells[Pairseditor::ctKey][i];
					}
					catch(...) {
					}

					if (!sKey.IsEmpty()) {

						UnicodeString sValue = L"";
						try {
							sValue = GridPairs->Cells[Pairseditor::ctValue][i];
						}
						catch(...) {
						}

						MemoSource->Lines->Add(sKey + L"=" + sValue);
					}

				}

			}break;
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormStringPairsEditor::FormClose(TObject *Sender, TCloseAction &Action) {
	if (PageControl1->ActivePageIndex != 1) {
		PageControl1->ActivePageIndex = 1;
		PageControl1Change(PageControl1);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormStringPairsEditor::AssignPairStrings(TStrings *ALines) {
	PageControl1->ActivePageIndex = 0;
	MemoSource->Lines->Assign(ALines);
	PageControl1Change(PageControl1);
}

//---------------------------------------------------------------------------
void __fastcall TFormStringPairsEditor::AssignPairText(const UnicodeString &sText) {
	PageControl1->ActivePageIndex = 0;
	MemoSource->Lines->Text = sText;
	PageControl1Change(PageControl1);
}
//---------------------------------------------------------------------------
