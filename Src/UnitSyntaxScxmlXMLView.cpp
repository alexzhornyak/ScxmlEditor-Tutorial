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

#include "UnitSyntaxScxmlXMLView.h"

#include <memory>
#include "UnitDModule.h"
#include "UnitTreeEditorStateMachine.h"
#include "Log4cpp_VCL.hpp"

//---------------------------------------------------------------------------

#pragma package(smart_init)

__fastcall TFrameSyntaxScxmlXMLView::TFrameSyntaxScxmlXMLView(TComponent* Owner) : TFrameSyntaxEditorEx(Owner) {
	FEditDocRaw = new TLMDEditDocument(this);

	FEditDocRaw->ColorSchemes->Assign(DModule->EditDoc1->ColorSchemes);
	FEditDocRaw->SyntaxSchemes->Assign(DModule->EditDoc1->SyntaxSchemes);
	FEditDocRaw->ActiveSyntaxScheme = L"XML";
	FEditDocRaw->ActiveColorScheme = L"default";

	this->EditView->Document = FEditDocRaw;
	this->EditView->OnKeyUp = EditViewKeyUp;

	// включаем breakpoint bar
	EditView->Gutter->Bars->Items[0]->Visible = true;

	FFrameScxmlLiveEdit = new TFrameScxmlLiveEdit(this);
	FFrameScxmlLiveEdit->Align = alTop;
	FFrameScxmlLiveEdit->Parent = this;
	FFrameScxmlLiveEdit->ActionConvert->OnExecute = OnConvertButtonClick;
	// необходимо так делать, иначе вылезет над тулбаром
	FFrameScxmlLiveEdit->Top = ToolBar1->BoundsRect.Bottom + 1;

	// сделает видимыми XML Utils
	UpdateAllUtilsActions();

	this->EditView->TabOrder = 0;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxScxmlXMLView::SetText(const UnicodeString &sText) {
	FEditDocRaw->Lines->Text = sText;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxScxmlXMLView::SetBreakpointToStateId(const UnicodeString &sStateId) {

	FEditDocRaw->Breakpoints->Clear();

	for (int i = 0; i < FEditDocRaw->Lines->Count; i++) {
		const UnicodeString sSearch = "id=" + AnsiQuotedStr(sStateId, L'"');
		if (FEditDocRaw->Lines->Strings[i].Pos(sSearch)) {

			_di_ILMDMark Brk = FEditDocRaw->Breakpoints->CreateMarkAtLine(i);
			FEditDocRaw->SetBreakpointKind(Brk, mkBreakActive);

			EditView->GotoPhysLine(i);

			break;
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxScxmlXMLView::ClearBreakpoints(void) {
	FEditDocRaw->Breakpoints->Clear();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxScxmlXMLView::ClearAll(void) {
	FEditDocRaw->Breakpoints->Clear();
	FEditDocRaw->Clear();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxScxmlXMLView::OnConvertButtonClick(TObject *Sender) {
	if (!IsViewOnly()) {
		TStateMachineDockPanel * ADockPanel = GlobalActiveEditorDockPanel();
		if (ADockPanel) {
			try {
				// предварительно проверим синтаксис
				if (IsXMLSyntaxValid()) {
					ADockPanel->StateMachineEditor->ConvertScxmlDataToDesign(FEditDocRaw->Lines->Text);
					EditView->SetFocus();
				}
			}
			catch(Exception * E) {
				LOG_ERROR(LOG_ERROR_MSG);
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxScxmlXMLView::EditViewKeyUp(TObject *Sender, WORD &Key, TShiftState Shift) {
	switch(Key) {
	case VK_RETURN: {
			if (Shift == TShiftState() << ssCtrl) {
				FFrameScxmlLiveEdit->ActionConvert->Execute();
			}
		}break;
	case VK_F5: {
			TStateMachineDockPanel * ADockPanel = GlobalActiveEditorDockPanel();
			if (ADockPanel) {
				ADockPanel->UpdateSyntaxView();
			}
		}break;
	}
}
