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

#include "UnitTreeEditorForTransition.h"

#include "UnitScxmlBaseShape.h"
#include "UnitChildScxmlBaseShapes.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TTransitionStateMachineEditor::TTransitionStateMachineEditor(TComponent *AOwner) : TStateMachineEditor(AOwner, NULL, false) {
	FRootBaseShape = NULL;

	this->BorderStyle = bsNone;
	TTreeEditWindows HideWindows = TTreeEditWindows() << teInspector << teToolbar << teToolShapes << teEditors << teFont << teFormat <<
	teMainMenu << teSideToolbar << teNodeTree
	// << teRulers << teStatus << teModeTabs
	;

	this->TreeHideEditorPanels(HideWindows);

	this->TheTree->GlobalFormat.ImageIndex = tiNone;

	FRootBaseShape = dynamic_cast<TTransitionXMLShape*>(this->TheTree->AddShapeClass(0, 0,
			TScxmlBaseShape::TypeToXMLNodeName(sctTransitionXML), NULL, __classid(TTransitionXMLShape)));
	assert(FRootBaseShape != NULL);

	FRootBaseShape->Expanded = true;
}

//---------------------------------------------------------------------------
void __fastcall TTransitionStateMachineEditor::TheTreeDeletingShapes(TSelectedShapeList* Sender, bool &AllowDelete) {
	if (Sender && Sender->Shapes->IndexOf(FRootBaseShape) != -1) {
		AllowDelete = false;
		return;
	}

	TStateMachineEditor::TheTreeDeletingShapes(Sender, AllowDelete);
}
//---------------------------------------------------------------------------
