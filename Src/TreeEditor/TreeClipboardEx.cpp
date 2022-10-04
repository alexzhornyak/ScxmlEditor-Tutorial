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

#include "TreeClipboardEx.h"

#include <memory>

#include <map>

#include <Clipbrd.hpp>

#include "Log4cpp_VCL.hpp"
#include "UnitScxmlBaseShape.h"
#include "UnitTreeEx.h"
#include "UnitTreeEditorUtils.h"
#include "UnitUniqueUtils.h"
#include "UnitStateMachineConnection.h"
#include "UnitTreeLoader.h"

typedef std::map<TTreeNodeShape*, TTreeNodeShape*>TAccordingMap;

// ---------------------------------------------------------------------------

#pragma package(smart_init)

System::Word CF_CUSTOM_TREE = 0, CF_CUSTOM_CONNECTION = 0;

void RegisterTreeClipboardEx() {
#pragma startup RegisterTreeClipboardEx

	Classes::RegisterClassA(__classid(TTreeClipboardEx));

	CF_CUSTOM_TREE = RegisterClipboardFormat("TreeClipboardEx");
}

// ---------------------------------------------------------------------------
void CopyTreeToClipboard(TCustomTree *ATree) {

	TClipboard* AClipboard = Clipboard();

	if (!AClipboard)
		throw Exception("Clipboard()==NULL!!!");

	std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
	SaveTreeToStream(ATree, AMemoryStream.get());

	AClipboard->Open();
	try {

		AClipboard->Clear();

		// {-- Записываем данные в mstream. --}
		HGLOBAL hbuf = GlobalAlloc(GMEM_MOVEABLE, AMemoryStream->Size);
		if (hbuf) {
			void *bufptr = GlobalLock(hbuf);
			if (bufptr) {
				try {
					memmove(bufptr, AMemoryStream->Memory, AMemoryStream->Size);
					AClipboard->SetAsHandle(CF_CUSTOM_TREE, (UINT)hbuf);
				}
				__finally {
					GlobalUnlock(hbuf);
				}
			}
		}
	}
	catch(...) {
		LOG_ERROR("%s> Clipboard is broken!", __FUNCTION__);
	}
	AClipboard->Close();
}

// ---------------------------------------------------------------------------
void PasteTreeFromClipboard(TCustomTree *ATree) {
	TClipboard* AClipboard = Clipboard();

	if (!AClipboard)
		throw Exception("Clipboard()==NULL!!!");

	try {
		void* hbuf = (void*)AClipboard->GetAsHandle(CF_CUSTOM_TREE);
		if (hbuf) {
			void *bufptr = GlobalLock(hbuf);
			if (bufptr) {
				try {
					std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
					AMemoryStream->WriteBuffer(bufptr, GlobalSize(hbuf));
					AMemoryStream->Position = 0;

					try {
						LoadTreeFromStreamEx(ATree, AMemoryStream.get());
					}
					catch(...) {
					}

				}
				__finally {
					GlobalUnlock(hbuf);
				}
			}
		}
	}
	catch(...) {
		LOG_ERROR("%s> Clipboard is broken!", __FUNCTION__);
	}
	AClipboard->Close();
}

// ---------------------------------------------------------------------------
TTreeNodeShape * CloneShapeEx(TComponent *AOwner, TCustomTree *ATree, TTreeNodeShape *AShape, TAccordingMap &AMap) {
	assert(AShape != NULL);
	assert(AOwner != NULL);

	TTreeConnection* tmpConn = NULL;

	TTreeNodeShape * result = NULL;
	Application->CreateForm(AShape->ClassType(), &result);

	result->Tree = ATree;

	result->Assign(AShape);

	// попробуем сначала дать оригинальное имя
	if (AOwner->FindComponent(AShape->Name)) {
		// если существует, тогда сгенерируем новое имя
		const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(AOwner, AShape->ClassType());
		result->Name = AUniqueName.second;
	}
	else {
		// обязательно имя присваиваем здесь, так как в Assign не происходит почему-то присвоение
		result->Name = AShape->Name;
	}

	result->Visible = false;

	AOwner->InsertComponent(result);

	AMap[AShape] = result;

	// { Clone Children nodes }
	for (int t = 0; t < AShape->Children->Count; t++) {
		TTreeNodeShape* tmpShape = CloneShapeEx(AOwner, ATree, AShape->Children->Items[t], AMap); // tom:11/10/2002;

		tmpShape->Parent = result;

		tmpShape->AutoPosition->Assign(AShape->Children->Items[t]->AutoPosition);

	}
	return result;
}

// ---------------------------------------------------------------------------
void RepairConnections(TComponent *AOwner, TCustomTree *ASourceTree, TCustomTree *ATargetTree, TAccordingMap &AMap) {
	assert(AOwner != NULL);
	for (int i = 0; i < ASourceTree->Connections->Count; i++) {

		TTreeConnection *ASourceConnection = ASourceTree->Connections->Items[i];

		TAccordingMap::iterator it_from = AMap.find(ASourceConnection->FromShape);
		TAccordingMap::iterator it_to = AMap.find(ASourceConnection->ToShape);
		if (it_from != AMap.end() && it_to != AMap.end()) {

			// если коннект частного типа, то будем править возможность множественного добавления
			TTreeConnection *AConnection = NULL;
#if 0 // Deep Debug !
			WLOG_DEBUG(L"ClassType=%s,Name=%s", ASourceConnection->ClassName().c_str(), ASourceConnection->Name.c_str());
#endif

			if (ASourceConnection->ClassType() != __classid(TTreeConnection)) {
				Application->CreateForm(ASourceConnection->ClassType(), &AConnection);
				AConnection->FromShape = it_from->second;
				AConnection->ToShape = it_to->second;
				AConnection->Tree = ATargetTree;

				// если такой компонент уже есть, тогда сгенерируем новое имя
				if (AOwner->FindComponent(ASourceConnection->Name)) {
					const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(AOwner, AConnection->ClassType());
					AConnection->Name = AUniqueName.second;
				}
				else {
					AConnection->Name = ASourceConnection->Name;
				}

				AOwner->InsertComponent(AConnection);

				AConnection->Assign(ASourceConnection);
				// bug fix: какого-то хрена не назначаются точки при множественном назначении
				AConnection->Style = ASourceConnection->Style;
				AConnection->Points->Clear();

				for (int i = 0; i < ASourceConnection->Points->Count(); i++) {
					AConnection->Points->Add(ASourceConnection->Points->Item[i].XStyle, ASourceConnection->Points->Item[i].XValue,
						ASourceConnection->Points->Item[i].YStyle, ASourceConnection->Points->Item[i].YValue);
				}
			}
			else {
				// подумать, что делать при копировании обычных связей
				AConnection = it_from->second->AddConnection(it_to->second);
				AConnection->Assign(ASourceConnection);
				// AConnection->Name = TeeGetUniqueName(ASourceTree->Owner, AConnection->ClassName().Delete(1, 1));
			}
		}
	}
}

// ---------------------------------------------------------------------------
__fastcall TTreeClipboardEx::TTreeClipboardEx(Classes::TComponent* AOwner) : TCustomTree(AOwner) {
	this->Page->UsePrinter = false;
	//	this->NoOwnerShapes = true;
	// this->AssignParent = false;
}

// ---------------------------------------------------------------------------
__fastcall TTreeClipboardEx::~TTreeClipboardEx() {
}

// ---------------------------------------------------------------------------
void CloneSelectedShapes(TCustomTree *SourceTree, std::vector<TTreeNodeShape*> &VecNewShapes) {
	if (!SourceTree || SourceTree->Selected->Count() == 0)
		return;

	TAccordingMap AMap;

	for (int t = 0; t < SourceTree->Selected->Count(); t++) {
		if (!Editorutils::IsParentSelected(SourceTree->Selected->Items[t])) {
			TTreeNodeShape *tmp = CloneShapeEx(SourceTree->Owner, SourceTree, SourceTree->Selected->Items[t], AMap);

			// tom:11/10/2002 keep AutoPosition information; which is not kept by Assign method (for other reasons:eg AddChild)
			tmp->AutoPosition->Assign(SourceTree->Selected->Items[t]->AutoPosition);

			tmp->Tree = SourceTree;
			tmp->Parent = SourceTree->Selected->Items[t]->Parent;
			tmp->Visible = true;

			// к дочерним должны идти родные прямоугольные связи
			if (!tmp->InheritsFrom(__classid(TVisualScxmlBaseShape))) {
				const int iNewIndex = SourceTree->Selected->Items[t]->BrotherIndex + 1;
				// так как при установке BrotherIndex происходит Exchange, поэтому надо поэтапно пройтись по всем нодам
				while (tmp->BrotherIndex > iNewIndex) {
					tmp->BrotherIndex = tmp->BrotherIndex - 1;
				}

				SourceTree->Selected->Items[t]->Parent->AddConnection(tmp);
			}

			VecNewShapes.push_back(tmp);
		}
	}

	RepairConnections(SourceTree->Owner, SourceTree, SourceTree, AMap);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeClipboardEx::Copy(TCustomTree *SourceTree) {
	FIsCut = NULL;
	if (SourceTree->Selected->Count() > 0) {
		this->Clear();

		TAccordingMap AMap;

		for (int t = 0; t < SourceTree->Selected->Count(); t++) {
			if (!Editorutils::IsParentSelected(SourceTree->Selected->Items[t])) {
				// tmp = CloneShape(SourceTree->Selected->Items[t]);
				TTreeNodeShape *tmp = CloneShapeEx(this->Owner, this, SourceTree->Selected->Items[t], AMap);

				// tom:11/10/2002 keep AutoPosition information; which is not kept by Assign method (for other reasons:eg AddChild)
				tmp->AutoPosition->Assign(SourceTree->Selected->Items[t]->AutoPosition);
				// if (tmp->SimpleText == SourceTree->Selected->Items[t]->Name)
				// tmp->Text->Clear();

				tmp->Parent = NULL;
				tmp->Visible = true;
				tmp->Tree = this;
			}
		}

		RepairConnections(this->Owner, SourceTree, this, AMap);

		try {

			CopyTreeToClipboard(this);

			//			this->Clear();
		}
		catch(...) {
		}
	}
	else if (SourceTree->Connections->Selected) {
		TClipboard* AClipboard = Clipboard();

		if (!AClipboard)
			throw Exception("Clipboard()==NULL!!!");

		AClipboard->Clear();

		TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(SourceTree->Connections->Selected);
		if (AStateMachineConnection) {
			AClipboard->AsText = AStateMachineConnection->ToClipboardText();
		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TTreeClipboardEx::Cut(TCustomTree *SourceTree) {
	Copy(SourceTree);
	if (SourceTree->Connections->Selected) {
		delete SourceTree->Connections->Selected;
	}
	else if (SourceTree->Selected) {
		SourceTree->Selected->Delete();
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TTreeClipboardEx::Paste(TCustomTree *DestTree, bool UseCursorPos) {
	bool bRetVal = false;

	if (!DestTree)
		throw Exception(__FUNCTION__ "> DestTree==NULL");

	TClipboard* AClipboard = Clipboard();

	if (!AClipboard)
		throw Exception("Clipboard()==NULL!!!");

	if (AClipboard->HasFormat(CF_CUSTOM_TREE)) {

		bRetVal = true;

		this->Clear();

		PasteTreeFromClipboard(this);

		TPoint P = TPoint(0, 0);
		const TPoint ptCurPos = DestTree->GetCursorPos();
		TTreeNodeShape *tmpClicked = GetSafeTreeListFirst(DestTree->Selected->Shapes);
		if (tmpClicked) {
			P.x = tmpClicked->X0 + 15;
			P.y = tmpClicked->Y0 + 15;

			// позицию курсора будем использовать только в том случае, если он в пределах выделенной фигуры
			if (UseCursorPos) {
				const TPoint LeftUpP = DestTree->Canvas->Calculate3DPosition(tmpClicked->X0, tmpClicked->Y0, TeeTreeZ);
				const TPoint RightDownP = DestTree->Canvas->Calculate3DPosition(tmpClicked->X1, tmpClicked->Y1, TeeTreeZ);
				if (!Editorutils::PtInRectEx(TRect(LeftUpP, RightDownP), ptCurPos)) {
					UseCursorPos = false;
				}
			}
		}

		DestTree->Selected->Clear();

		TAccordingMap AMap;

		// устраняем проблему при нажатии 'Ctrl+V'

		if (UseCursorPos) {
			tmpClicked = DestTree->ClickedShape(ptCurPos.x, ptCurPos.y);
			if (tmpClicked) {
				const TPoint LeftUpP = DestTree->Canvas->Calculate3DPosition(tmpClicked->X0, tmpClicked->Y0, TeeTreeZ);
				const TPoint RightDownP = DestTree->Canvas->Calculate3DPosition(tmpClicked->X1, tmpClicked->Y1, TeeTreeZ);

				const double dXRatio = (double)(RightDownP.x - LeftUpP.x) / (double)(tmpClicked->X1 - tmpClicked->X0);
				const double dYRatio = (double)(RightDownP.y - LeftUpP.y) / (double)(tmpClicked->Y1 - tmpClicked->Y0);

				int iOffsetX = ptCurPos.x - LeftUpP.x;
				int iOffsetY = ptCurPos.y - LeftUpP.y;

				P.x = tmpClicked->X0 + ((double)iOffsetX / (dXRatio != 0.0f ? dXRatio : 1.0f));
				P.y = tmpClicked->Y0 + ((double)iOffsetY / (dYRatio != 0.0f ? dYRatio : 1.0f));
			}

		}

		TPoint ptOffset;

		for (int t = 0; t < this->Roots->Count; t++) {

			TTreeNodeShape *tmp = CloneShapeEx(DestTree->Owner, DestTree, this->Roots->Items[t], AMap);

			tmp->Parent = tmpClicked;

			if (tmpClicked && tmp->InheritsFrom(__classid(TChildScxmlBaseShape))) {
				TTreeConnection *AConnection = tmpClicked->AddConnection(tmp);
			}
			else {
				if (t == 0) {
					ptOffset.x = P.x - tmp->X0;
					ptOffset.y = P.y - tmp->Y0;
				}
				tmp->MoveRelative(ptOffset.x, ptOffset.y, true);
			}

			tmp->AutoPosition->Assign(this->Roots->Items[t]->AutoPosition);

			tmp->Selected = True;
			tmp->Visible = true;
		}

		RepairConnections(DestTree->Owner, this, DestTree, AMap);

		/* устраняем баг, что при вставке может свернуться */
		if (tmpClicked) {
			tmpClicked->Expanded = true;
		}

		DestTree->Cursor = crDefault;
	}
	else if (AClipboard->HasFormat(CF_TEXT)) {
		this->Clear();
		const UnicodeString sClipboardText = AClipboard->AsText;
		if (IsStateMachineConnectionClipboardFormat(sClipboardText)) {
			bRetVal = true;
			PasteStateMachineConnectionFromText(DestTree, sClipboardText);
		}
	}

	return bRetVal;
}

void CloneBrotherShapes(TComponent *AOwner, TCustomTree *SourceTree, TCustomTree *TargetTree, TTreeNodeShape *AParent,
	const std::set<TTreeNodeShape*> &AShapes, std::set<TTreeNodeShape*> &NewShapes) {
	if (!AOwner)
		return;
	if (!SourceTree)
		return;

	TAccordingMap AMap;

	for (std::set<TTreeNodeShape*>::const_iterator it = AShapes.begin(); it != AShapes.end(); ++it) {
		TTreeNodeShape *ASourceShape = (*it);
		if (!ASourceShape)
			continue;

		if (!Editorutils::IsParentSelected(ASourceShape)) {
			TTreeNodeShape *tmp = CloneShapeEx(AOwner, TargetTree, ASourceShape, AMap);

			// tom:11/10/2002 keep AutoPosition information; which is not kept by Assign method (for other reasons:eg AddChild)
			tmp->AutoPosition->Assign(ASourceShape->AutoPosition);

			tmp->Tree = TargetTree;
			tmp->Parent = AParent;
			tmp->Visible = true;

			// к дочерним должны идти родные прямоугольные связи
			if (!tmp->InheritsFrom(__classid(TVisualScxmlBaseShape))) {
				const int iNewIndex = ASourceShape->BrotherIndex + 1;
				// так как при установке BrotherIndex происходит Exchange, поэтому надо поэтапно пройтись по всем нодам
				while (tmp->BrotherIndex > iNewIndex) {
					tmp->BrotherIndex = tmp->BrotherIndex - 1;
				}

				AParent->AddConnection(tmp);
			}

			NewShapes.insert(tmp);
		}
	}

	RepairConnections(AOwner, SourceTree, TargetTree, AMap);
}
