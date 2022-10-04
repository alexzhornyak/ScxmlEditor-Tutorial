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

#include "UnitTransitionXMLEditor.h"

#include "Log4cpp_VCL.hpp"
#include "UnitScxmlBaseShape.h"
#include "UnitChildScxmlBaseShapes.h"
#include "UnitStateMachineParser.h"
#include "UnitStateMachineConnection.h"
#include "UnitCustomUtils.h"
#include "UnitTreeEditorUtils.h"
#include "UnitTreeEx.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

TTransitionXMLEditor *TransitionXMLEditor = NULL;

//---------------------------------------------------------------------------
__fastcall TTransitionXMLEditor::TTransitionXMLEditor(TComponent *AOwner) : TStateMachineEditor(AOwner, NULL, true) {
	FRootBaseShape = NULL;

	this->BorderStyle = bsNone;
	TTreeEditWindows HideWindows = TTreeEditWindows() << teInspector << teToolbar << teToolShapes << teEditors << teFont << teFormat <<
	teMainMenu << teSideToolbar << teRulers << TTreeEditWindow::teStatus << teModeTabs << teNodeTree;

	this->TreeHideEditorPanels(HideWindows);

	this->TheTree->GlobalFormat.ImageIndex = tiNone;
	this->TheTree->Grid->Visible = false;

	FRootBaseShape = dynamic_cast<TTransitionXMLShape*>(this->TheTree->AddShapeClass(0, 0,
			TScxmlBaseShape::TypeToXMLNodeName(sctTransitionXML), NULL, __classid(TTransitionXMLShape)));
	assert(FRootBaseShape != NULL);

	FRootBaseShape->Expanded = true;
	FRootBaseShape->Visible = false;
	FLinkPropPath = L"";

	this->Caption = L"Transition XML";
	this->TheTreeEx->DisableFocus = true;
}

//---------------------------------------------------------------------------
void __fastcall TTransitionXMLEditor::TheTreeDeletingShapes(TSelectedShapeList* Sender, bool &AllowDelete) {
	if (Sender && Sender->Shapes->IndexOf(FRootBaseShape) != -1) {
		AllowDelete = false;
		return;
	}

	TStateMachineEditor::TheTreeDeletingShapes(Sender, AllowDelete);
}

//---------------------------------------------------------------------------
void __fastcall TTransitionXMLEditor::SetStateMachineConnection(TStateMachineConnection *AConnection) {
	FLinkPropPath = L"";
	if (AConnection) {
		this->OnTreeEditorModified = NULL;

		FLinkPropPath = Customutils::GetFullPropPath(AConnection);

		Editorutils::TreeUpdateLock ALock(TheTree);
		TheTree->View3DOptions->HorizOffset = 0;
		TheTree->View3DOptions->VertOffset = 0;

		// ÇÀÏÎËÍßÅÌ ÑÂÎÉÑÒÂÀ ÂÑÅÕ ÏÎËÅÉ
		try {
			Statemachine::XMLStringToInvisibleShapeChildren(this->RootBaseShape, AConnection->XML);
			FRootBaseShape->SetErrorState(false);
		}
		catch(Exception * E) {
			WLOG_ERROR(L"Transition[%s] XML Check> %s", AConnection->Name.c_str(), E->Message.c_str());
			FRootBaseShape->SetErrorState(true);
		}

		for (int i = 0; i < this->TheTree->Shapes->Count; i++) {
			TChildScxmlBaseShape *AChildShape = dynamic_cast<TChildScxmlBaseShape*>(this->TheTree->Shapes->Items[i]);
			if (AChildShape) {
				AChildShape->ClipOutValue = false;
			}
		}

		FRootBaseShape->Visible = true;

		TheTree->FullExpandCollapse(true);
		TeeModified(false);

		this->OnTreeEditorModified = OnSaveTreeModified;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTransitionXMLEditor::OnSaveTreeModified(TObject * Sender) {
	this->OnTreeEditorModified = NULL;
	try {
		boost::tuple<TComponent*, TStateMachineEditor*, TStateMachineDockPanel*>ALinkInstances = ParseInstancePropPath(FLinkPropPath);

		TComponent *AInstance = ALinkInstances.get<0>();
		TStateMachineEditor *AEditor = ALinkInstances.get<1>();
		TStateMachineDockPanel *ADockPanel = ALinkInstances.get<2>();

		TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(AInstance);
		if (!AStateMachineConnection)
			throw Exception("Can not cast [" + FLinkPropPath + "] to TStateMachineConnection!");

		if (AStateMachineConnection->IsEditorOpened()) {
			AStateMachineConnection->ShowEditor();
			WLOG_ERROR(L"CONFLICT> 'TransitionEditor form' is opened! [%s]", AStateMachineConnection->Name.c_str());
		}

		if (this->RootBaseShape) {
			try {
				AStateMachineConnection->XML = Statemachine::InvisibleShapeChildrenToXMLString(this->RootBaseShape);
				FRootBaseShape->SetErrorState(false);
			}
			catch(Exception * E) {
				FRootBaseShape->SetErrorState(true);
				WLOG_WARNING(L"TransitionXML> %s", E->Message.c_str());
			}
		}

		if (AEditor) {
			AEditor->MarkModified(1,
				"Connection [" + AStateMachineConnection->Name + "]:[" + AStateMachineConnection->SimpleText + "] changed", false);
		}
	}
	catch(Exception * E) {
		FRootBaseShape->SetErrorState(true);
		WLOG_ERROR(L"TransitionXML> %s", E->Message.c_str());
	}
	this->TeeModified(false);
	this->OnTreeEditorModified = OnSaveTreeModified;
}

//---------------------------------------------------------------------------
void __fastcall TTransitionXMLEditor::ResetStateMachineConnection(void) {
	try {
		this->OnTreeEditorModified = NULL;
		FLinkPropPath = L"";
		if (this->RootBaseShape) {
			this->RootBaseShape->Visible = false;
			this->RootBaseShape->Clear();
			// íå äåëàòü 'RootBaseShape->Select = false' ! Òàê êàê ïðè ýòîì î÷èñòÿòñÿ ñ îïîçäàíèåì DesignObjects
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"TransitionXML> %s", E->Message.c_str());
	}
}

//---------------------------------------------------------------------------
bool __fastcall TTransitionXMLEditor::IsDesignObjectsOwner(void) {
	bool bRes = false;
	if (DesignObjects) {
		for (int i = 0; i < DesignObjects->Count; i++) {
			if (TheTree->Items->IndexOf(DesignObjects->Item[i]) == -1) {
				return false;
			}
			else {
				bRes = true;
			}
		}
	}
	return bRes;
}
//---------------------------------------------------------------------------
