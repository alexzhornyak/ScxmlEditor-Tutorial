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

#include "UnitTransitionEditor.h"

#include "Log4cpp_VCL.hpp"

#include "UnitStateMachineConnection.h"
#include "UnitTreeEditorForTransition.h"
#include "UnitChildScxmlBaseShapes.h"
#include "UnitStateMachineParser.h"
#include "UnitMain.h"
#include "UnitScxmlShape.h"
#include "UnitStateShape.h"
#include "UnitSyntaxEditorForm.h"
#include "UnitSettings.h"
#include "UnitCustomUtils.h"
#include "UnitTransitionXMLEditor.h"
#include "UnitLineStringEditor.h"
#include "UnitComboStringEditor.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDSedDocument"
#pragma link "LMDSedView"
#pragma resource "*.dfm"
TFormTransitionEditor *FormTransitionEditor = 0;

// ---------------------------------------------------------------------------
__fastcall TFormTransitionEditor::TFormTransitionEditor(TComponent* Owner, TStateMachineConnection *AStateMachineConnection) : TForm
(Owner), FStateMachineConnection(AStateMachineConnection) {

	/* –≈ƒ¿ “Œ– –¿«–¿¡Œ“¿Õ œŒ œ–»Õ÷»œ”, ◊“Œ ÃŒ∆≈“ ¡€“‹ œŒ ¿«¿ÕŒ Õ≈— ŒÀ‹ Œ –≈ƒ¿ “Œ–Œ¬ */
	/* œ–»—¬Œ≈Õ»≈ ¬—≈’ ƒ¿ÕÕ€’ Õ¿ ‘Œ–Ã≈ ƒ≈À¿“‹ ◊≈–≈« 'OnShow' */

	if (!FStateMachineConnection)
		throw Exception("PROGRAM LOGIC ERROR> StateMachineConnection == NULL !!!");

	if (!TransitionXMLEditor)
		throw Exception(L"PROGRAM LOGIC ERROR> TransitionXMLEditor == NULL !!!");

	FStateMachineEditor = new TTransitionStateMachineEditor(this);
	FStateMachineEditor->Parent = this;
	FStateMachineEditor->Align = alClient;

	FStateMachineEditor->Show();
}

// ---------------------------------------------------------------------------
void __fastcall TFormTransitionEditor::OnTreeModified(TObject * Sender) {
	ActionSave->Update();
}

// ---------------------------------------------------------------------------
void __fastcall TFormTransitionEditor::ActionSaveExecute(TObject * Sender) {
	try {
		FStateMachineConnection->XML = Statemachine::InvisibleShapeChildrenToXMLString(FStateMachineEditor->RootBaseShape);
		FStateMachineEditor->RootBaseShape->SetErrorState(false);

		FStateMachineConnection->Event = EditTransitionName->Text;
		FStateMachineConnection->Condition = EditCondition->Text;

		TStateMachineEditor *AEditor = dynamic_cast<TStateMachineEditor*>(FStateMachineConnection->Owner);
		if (AEditor && AEditor->PropSettingsInspector) {
			AEditor->PropSettingsInspector->UpdateContent();
			AEditor->TeeModified(true, 1,
				"Connection [" + FStateMachineConnection->Name + "]:[" + FStateMachineConnection->SimpleText + "] changed");
		}

		FStateMachineEditor->TeeModified(false);

		if (TransitionXMLEditor->LinkPropPath == Customutils::GetFullPropPath(FStateMachineConnection)) {
			TransitionXMLEditor->SetStateMachineConnection(FStateMachineConnection);
		}
	}
	catch(Exception * E) {
		FStateMachineEditor->RootBaseShape->SetErrorState(true);
		WLOG_ERROR(L"TransitionEditor> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormTransitionEditor::ActionSaveUpdate(TObject * Sender) {
	ActionSave->Enabled = FStateMachineEditor->IsModified();
	this->Caption = UnicodeString().sprintf(L"%sEdit transition [%s]", FStateMachineEditor->IsModified() ? L"*" : L"",
		EditTransitionName->Text.c_str());
}
// ---------------------------------------------------------------------------

void __fastcall TFormTransitionEditor::EditTransitionNameChange(TObject * Sender) {
	FStateMachineEditor->TeeModified(true, 2, __FUNCTION__);
}

// ---------------------------------------------------------------------------
void __fastcall TFormTransitionEditor::EditConditionChange(TObject * Sender) {
	FStateMachineEditor->TeeModified(true, 2, __FUNCTION__);
}

// ---------------------------------------------------------------------------
bool __fastcall TFormTransitionEditor::IsModified(void) {
	return FStateMachineEditor->IsModified();
}

// ---------------------------------------------------------------------------
void __fastcall TFormTransitionEditor::FormCloseQuery(TObject * Sender, bool&CanClose) {
	if (!TFormSyntaxEditorForm::CloseAssociatedSavedDockPanels(this)) {
		CanClose = false;
		return;
	}

	if (FStateMachineEditor->IsModified()) {
		String Caption = "WARNING";
		String Text = "You have unsaved modifications! Are you sure to quit?";
		CanClose = Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormTransitionEditor::FormShow(TObject * Sender) {
	FStateMachineEditor->RootBaseShape->Clear();

	FStateMachineEditor->DesignObjects->Clear();

	// «¿œŒÀÕﬂ≈Ã —¬Œ…—“¬¿ ¬—≈’ œŒÀ≈…
	try {
		Statemachine::XMLStringToInvisibleShapeChildren(FStateMachineEditor->RootBaseShape, FStateMachineConnection->XML);
		FStateMachineEditor->RootBaseShape->SetErrorState(false);
	}
	catch(Exception * E) {
		FStateMachineEditor->RootBaseShape->SetErrorState(true);
		LOG_ERROR(LOG_ERROR_MSG);
	}
	EditTransitionName->Text = FStateMachineConnection->Event;
	EditCondition->Text = FStateMachineConnection->Condition;

	SpBtnSave->Caption = L""; // Û·Ë‡ÂÏ Ì‡‰ÔËÒ¸ ÓÚ TAction

	for (int i = 0; i < FStateMachineEditor->TheTree->Shapes->Count; i++) {
		TChildScxmlBaseShape *AChildShape = dynamic_cast<TChildScxmlBaseShape*>(FStateMachineEditor->TheTree->Shapes->Items[i]);
		if (AChildShape) {
			AChildShape->ClipOutValue = false;
		}
	}

	FStateMachineEditor->FillNodeTree(); //
	FStateMachineEditor->TheTree->FullExpandCollapse(true);
	FStateMachineEditor->NodeTree->FullExpandCollapse(true);
	FStateMachineEditor->TeeModified(false);
	FStateMachineEditor->OnTreeEditorModified = OnTreeModified;
}

//---------------------------------------------------------------------------
void __fastcall TFormTransitionEditor::BtnEditConditionClick(TObject *Sender) {
	const UnicodeString sInstPropPath = Customutils::GetFullPropPath(FStateMachineConnection);
	if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sInstPropPath, L"Condition")) {
		std::auto_ptr<TFormSyntaxEditorForm>AStrEditForm(new TFormSyntaxEditorForm(Application, SettingsData->ActiveSyntaxScheme,
				EditCondition->Text, "Edit 'Condition'"));

		const int iModalResult = AStrEditForm->ShowModal();
		if (iModalResult == mrOk) {
			EditCondition->Text = AStrEditForm->EditDoc1->Lines->Text.Trim();
		}
		if (iModalResult == mrOk || iModalResult == mrYes) {
			AStrEditForm->CreateSyntaxDockPanel(sInstPropPath, "Condition");
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormTransitionEditor::BtnEditEventClick(TObject *Sender) {
	if (FStateMachineConnection) {
		const UnicodeString sValue = FStateMachineConnection->Event;

		try {
			std::auto_ptr<TComboStringEditor>AComboStringEditorPtr(new TComboStringEditor(Application));
			AComboStringEditorPtr->Caption = "Transition [Event] Editor";

			FStateMachineConnection->FillEventNamesList(AComboStringEditorPtr->Lines);

			AComboStringEditorPtr->SetEditText(sValue);

			// ÓÚÍ˚‚‡ÂÏ ‚ Î˛·ÓÏ ÒÎÛ˜‡Â, ‡ ÔË¯ÂÏ ÚÓÎ¸ÍÓ, ÂÒÎË Writable
			const int iModalResult = AComboStringEditorPtr->ShowModal();
			if (iModalResult == mrOk) {
				EditTransitionName->Text = AComboStringEditorPtr->Edit1->Text;
			}

		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}
//---------------------------------------------------------------------------
