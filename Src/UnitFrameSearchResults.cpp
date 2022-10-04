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

#undef DrawText

#include "UnitFrameSearchResults.h"

#include "Log4cpp_VCL.hpp"

#include "UnitHTMLUtils.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitStateMachineUnit.h"
#include "UnitTreeEditorUtils.h"
#include "UnitTreeEx.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TeeProcs"
#pragma link "TeeTree"
#pragma resource "*.dfm"
TFrameSearchResults *FrameSearchResults = NULL;

class TMaskedFindTextShape : public TTreeNodeShape {
protected:

	UnicodeString FPropertyName;
	TMatchFind FMatchFind;

	virtual void __fastcall DrawRotatedLabel(Tecanvas::TCanvas3D* ACanvas, int iX, int iY, int iAngle, System::UnicodeString sText) {
		if (FMatchFind.IsEmpty()) {
			/* Do not use native draw!!! Text Antialias problem */
			ACanvas->TextOut3D(iX, iY, TeeTreeZ, sText);
		}
		else {

			int iTextOffset = 0;

			ACanvas->Font->Style = TFontStyles();

			if (!FPropertyName.IsEmpty()) {
				ACanvas->Font->Color = clBlue;

				ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, FPropertyName);
				iTextOffset += ACanvas->TextWidth(FPropertyName);
			}

			ACanvas->Font->Color = clWindowText;
			ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, L"=");
			iTextOffset += ACanvas->TextWidth(UnicodeString(L"="));

			if (!FMatchFind.sLeft.IsEmpty()) {
				ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, FMatchFind.sLeft);

				iTextOffset += ACanvas->TextWidth(FMatchFind.sLeft);
			}

			if (!FMatchFind.sWhat.IsEmpty()) {
				ACanvas->Font->Style = TFontStyles() << fsBold << fsUnderline;

				ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, FMatchFind.sWhat);

				iTextOffset += ACanvas->TextWidth(FMatchFind.sWhat);
			}

			if (!FMatchFind.sRight.IsEmpty()) {
				ACanvas->Font->Style = TFontStyles();

				ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, FMatchFind.sRight);
			}
		}
	}

public:
	__fastcall TMaskedFindTextShape(Classes::TComponent* AOwner) : TTreeNodeShape(AOwner) {
	}

	void __fastcall SetMatchFind(const UnicodeString &sPropName, const TMatchFind &AMatchFind) {
		FMatchFind = AMatchFind;
		FPropertyName = sPropName;
	}

};

//---------------------------------------------------------------------------
__fastcall TFrameSearchResults::TFrameSearchResults(TComponent* Owner) : TFrame(Owner) {

	HTMLSearch->GlobalFormat.Border->Visible = false;
	HTMLSearch->GlobalFormat.Transparent = false;
	HTMLSearch->GlobalFormat.NodeClass = __classid(TMaskedFindTextShape);
	HTMLSearch->GlobalFormat.ImageIndex = tiNone;
	HTMLSearch->NoOwnerShapes = true;
	HTMLSearch->AssignParent = false;

	TTreeExplorerAlignChild *AChildManager = dynamic_cast<TTreeExplorerAlignChild*>(HTMLSearch->GlobalFormat.ChildManager);
	if (AChildManager) {
		AChildManager->VertMargin = AChildManager->VertMargin + 1;

		std::auto_ptr<TPen>AChartPenPtr(new TPen);
		HTMLSearch->GlobalFormat.Connection->Border->Assign(AChartPenPtr.get());
		HTMLSearch->GlobalFormat.Connection->ArrowTo->Style = casNone;
	}

}

//---------------------------------------------------------------------------
TTreeNodeShape *TFrameSearchResults::EnsureRootNode(const UnicodeString &sName) {
	return EnsureNode(NULL, sName);
}

//---------------------------------------------------------------------------
TTreeNodeShape *TFrameSearchResults::EnsureNode(TTreeNodeShape *ANodeParent, const UnicodeString &sName) {
	TTreeNodeShape *ANode = FindNode(ANodeParent, sName);

	if (!ANode) {
		ANode = HTMLSearch->Items->AddChild(ANodeParent, sName);
		ANode->Font->Style = TFontStyles() << fsBold;
	}

	return ANode;
}

//---------------------------------------------------------------------------
TTreeNodeShape *TFrameSearchResults::EnsureNodeStart(TTreeNodeShape *ANodeParent, const UnicodeString &sName,
	const UnicodeString &sStartText) {
	TTreeNodeShape *ANode = FindNodeStart(ANodeParent, sStartText);

	if (!ANode) {
		ANode = HTMLSearch->Items->AddChild(ANodeParent, sName);
	}

	return ANode;
}

//---------------------------------------------------------------------------
TTreeNodeShape *TFrameSearchResults::FindNode(TTreeNodeShape *ANodeParent, const UnicodeString &sName) {
	TTreeNodeShape *ANode = NULL;
	if (ANodeParent) {
		if (ANodeParent->Children->Count) {
			ANode = ANodeParent->Children->First();
		}
	}
	else {
		if (HTMLSearch->Items->Count) {
			ANode = HTMLSearch->Items->First();
		}
	}

	while (ANode) {

		if (SameText(ANode->SimpleText, sName)) {
			break;
		}

		ANode = Editorutils::GetNextSibling(ANode);
	}
	return ANode;
}

//---------------------------------------------------------------------------
TTreeNodeShape *TFrameSearchResults::FindNodeStart(TTreeNodeShape *ANodeParent, const UnicodeString &sText) {

	TTreeNodeShape *ANode = NULL;
	if (ANodeParent) {
		if (ANodeParent->Children->Count) {
			ANode = ANodeParent->Children->First();
		}
	}
	else {
		if (HTMLSearch->Items->Count) {
			ANode = HTMLSearch->Items->First();
		}
	}

	while (ANode) {

		if (ANode->SimpleText.Pos(sText) == 1) {
			break;
		}

		ANode = Editorutils::GetNextSibling(ANode);
	}
	return ANode;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSearchResults::AddMatchNode(const UnicodeString &sUnit, const UnicodeString &sComponentName,
	const UnicodeString &sSimpleText, const UnicodeString &sPropName, const TMatchFind &AMatch) {
	TTreeNodeShape *ARootNode = EnsureRootNode(sUnit);
	if (ARootNode) {
		TTreeNodeShape *AShapeChild = EnsureNode(ARootNode, sComponentName);
		if (AShapeChild) {
			if (AShapeChild->Font->Color != clMaroon) {
				AShapeChild->Font->Color = clMaroon;
			}

			const UnicodeString sPropStart = sPropName + L"=";

			if (!FindNodeStart(AShapeChild, sPropStart)) {
				TTreeNodeShape *APropChild = HTMLSearch->Items->AddChild(AShapeChild, sPropStart + AMatch.GetText());
				TMaskedFindTextShape * AMaskedFindTextShape = dynamic_cast<TMaskedFindTextShape*>(APropChild);
				if (AMaskedFindTextShape) {
					AMaskedFindTextShape->SetMatchFind(sPropName, AMatch);
				}
			}

			AShapeChild->Expanded = true;
		}

		ARootNode->Expanded = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSearchResults::ExpandAll1Click(TObject *Sender) {
	if (HTMLSearch->Selected->Count()) {
		HTMLSearch->FullExpandCollapse(true);
	}
}

//---------------------------------------------------------------------------

void __fastcall TFrameSearchResults::CollapseAll1Click(TObject *Sender) {
	if (HTMLSearch->Selected->Count()) {
		HTMLSearch->FullExpandCollapse(false);
	}
}

//---------------------------------------------------------------------------

void __fastcall TFrameSearchResults::Collapse1Click(TObject *Sender) {
	if (HTMLSearch->Selected->Count()) {
		HTMLSearch->Selected->First()->Collapse(false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFrameSearchResults::Expand1Click(TObject *Sender) {
	if (HTMLSearch->Selected->Count()) {
		HTMLSearch->Selected->First()->Expanded = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSearchResults::DoSelectTreeNode(TTreeNodeShape *ANode, const bool bClear, const bool bScrollChart) {
	if (ANode) {
		UnicodeString sUnitName = L"";
		UnicodeString sComponentName = L"";
		switch(ANode->Level()) {
		case 1: {
				sComponentName = ANode->SimpleText;

				if (ANode->Parent) {
					sUnitName = ANode->Parent->SimpleText;
				}
			}break;
		case 2: {
				if (ANode->Parent) {
					sComponentName = ANode->Parent->SimpleText;

					if (ANode->Parent->Parent) {
						sUnitName = ANode->Parent->Parent->SimpleText;
					}
				}
			}break;
		}

		if (!sUnitName.IsEmpty()) {
			TStateMachineEditor *AFindEditor = FindStateMachineEditorByFileName(sUnitName);
			if (AFindEditor) {

				if (AFindEditor->StateMachineEditorUnit && AFindEditor->StateMachineEditorUnit->StateMachineDockPanel) {
					if (AFindEditor->StateMachineEditorUnit->StateMachineDockPanel->Zone &&
						AFindEditor->StateMachineEditorUnit->StateMachineDockPanel->Zone->Parent) {
						AFindEditor->StateMachineEditorUnit->StateMachineDockPanel->Zone->Parent->ActivePage =
							AFindEditor->StateMachineEditorUnit->StateMachineDockPanel->Zone;
					}
				}

				if (!sComponentName.IsEmpty()) {
					TComponent *AComponent = AFindEditor->FindComponent(sComponentName);
					if (TTreeNodeShape * ATreeNodeShape = dynamic_cast<TTreeNodeShape*>(AComponent)) {
						AFindEditor->SelectMultipleShape(ATreeNodeShape, bClear, bScrollChart);
					}
					else if (TTreeConnection * ATreeConnection = dynamic_cast<TTreeConnection*>(AComponent)) {
						AFindEditor->SelectConnection(ATreeConnection, bScrollChart);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSearchResults::Select1Click(TObject *Sender) {
	DoSelectTreeNode(GetSafeTreeListFirst(HTMLSearch->Selected->Shapes), true, true);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSearchResults::SelectChildren1Click(TObject *Sender) {
	if (HTMLSearch->Selected) {
		TTreeNodeShape *ANode = NULL;
		TTreeNodeShape *ALast = NULL;

		TTreeNodeShape *ASelected = GetSafeTreeListFirst(HTMLSearch->Selected->Shapes);
		if (ASelected) {
			if (ASelected->Children->Count) {
				ANode = ASelected->Children->First();
				ALast = ASelected->Children->Last();
			}
		}

		const TTreeNodeShape *AFirstNode = ANode;
		const TTreeNodeShape *ALastNode = ALast;

		while (ANode) {

			DoSelectTreeNode(ANode, ANode == AFirstNode, ANode == ALastNode);

			ANode = Editorutils::GetNextSibling(ANode);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSearchResults::HTMLSearchDblClickShape(TTreeNodeShape *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	DoSelectTreeNode(Sender, true, true);

	HTMLSearch->CancelMouse = true;
}
//---------------------------------------------------------------------------
