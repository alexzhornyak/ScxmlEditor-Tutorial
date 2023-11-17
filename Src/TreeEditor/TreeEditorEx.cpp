/** *********************************************************************************
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
 ************************************************************************************** */

#include <vcl.h>
#pragma hdrstop

#include "TreeEditorEx.h"

#include <set>
#include <math.hpp>
#include <IOUtils.hpp>

#include "TreeClipboardEx.h"
#include "Log4cpp_VCL.hpp"
#include "UnitTreeEx.h"
#include "UnitTreeEditorUtils.h"
#include "UnitSettings.h"
#include "UnitScxmlBaseShape.h"
#include "UnitStateMachineConnection.h"
#include "UnitTreeLoader.h"
#include "KeyboardUtils.h"
#include "UnitMaskedTextShape.h"
#include "ImageUtils.h"
#include "UnicodeStringHash.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterTreeEditorEx() {
#pragma startup RegisterTreeEditorEx

	Classes::RegisterClassA(__classid(TTreeEditorEx));
}

#define LITERAL_REVERT_FROM_CACHE		L"Revert From Cache"

void SetConnectionXYPointByIOType(TTreeConnection *AConnection, const Editorutils::TIOConnectionType AIOType,
	const Editorutils::TDirectionConnectionType AIODirection, const int iPointIndex) {
	using namespace Editorutils;

	const TConnectionPointStyle APointStyle = (AIODirection == Editorutils::cdtFrom) ? cpsFromPercent : cpsToPercent;

	switch(AIOType) {
	case ccftLeft:
		AConnection->Points->ChangeXStyle(iPointIndex, APointStyle);
		AConnection->Points->Item[iPointIndex].XValue = 0;
		AConnection->Points->ChangeYStyle(iPointIndex, APointStyle);
		AConnection->Points->Item[iPointIndex].YValue = 50;
		break;
	case ccftRight:
		AConnection->Points->ChangeXStyle(iPointIndex, APointStyle);
		AConnection->Points->Item[iPointIndex].XValue = 100;
		AConnection->Points->ChangeYStyle(iPointIndex, APointStyle);
		AConnection->Points->Item[iPointIndex].YValue = 50;
		break;
	case ccftTop:
		AConnection->Points->ChangeXStyle(iPointIndex, APointStyle);
		AConnection->Points->Item[iPointIndex].XValue = 50;
		AConnection->Points->ChangeYStyle(iPointIndex, APointStyle);
		AConnection->Points->Item[iPointIndex].YValue = 0;
		break;
	case ccftBottom:
		AConnection->Points->ChangeXStyle(iPointIndex, APointStyle);
		AConnection->Points->Item[iPointIndex].XValue = 50;
		AConnection->Points->ChangeYStyle(iPointIndex, APointStyle);
		AConnection->Points->Item[iPointIndex].YValue = 100;
		break;
	}
}

using namespace Keyboardutils;

// ---------------------------------------------------------------------------
/* ВНИМАНИЕ! Есть баг на большом количестве элементов, когда Shape->Image->Assign(Image) дает AccessViolation */
/* поэтому приходится делать очередной костыль через TreeImagePool */
std::map<TMetaClass*, int>g_MapImageIndexes;

// ---------------------------------------------------------------------------
// ---------------------------- TTreeEditorEx --------------------------------
// ---------------------------------------------------------------------------
__fastcall TTreeEditorEx::TTreeEditorEx(TComponent *AOwner) : TTreeEditorNew(AOwner), FMaxHistoryCount(256),
FResizingFlag(false), FMovingFlag(false), FIntermediateUndoStreamPtr(new TMemoryStream), FOnDownSelectedConn(NULL),
FKeyboardKey(0), FFullScreenForm(NULL), m_LockAddSelectionUndo(false) {
	FCliboardEditorEx = new TTreeClipboardEx(PanelTree);

	FTreeEnsureSelectedInView = false;
	FTreeInitConnectionFlag = false;
	FTreeUndoDescriptionFlag = L"";
	FTreeInvalidateFlag = false;

	FLockClearUndo = false;

	m_undo_it = m_UndoQueue.end();
	m_selection_it = m_SelectionQueue.end();

	/* DEBUGGING */
	// FCliboardEditorEx->Parent = this;
	// FCliboardEditorEx->Align = alTop;
	// FCliboardEditorEx->Visible = true;

	this->Cut2->OnClick = Cut1Click;
	this->Copy2->OnClick = Copy1Click;
	this->Paste2->OnClick = Paste1Click;

	// Undo-Redo and restore last modified
	Edit4->NewTopLine();
	FActionEditorRestoreFromCache = new TAction(this);
	FActionEditorRestoreFromCache->Caption = LITERAL_REVERT_FROM_CACHE;
	FActionEditorRestoreFromCache->OnExecute = OnMenuRestoreFromCache;
	FActionEditorRestoreFromCache->OnUpdate = OnActionRestoreFromCacheUpdate;
	FActionEditorRestoreFromCache->Category = L"Edit";

	FActionEditorRestoreFromCache->ActionList = ActionListEditor;

	FMenuEditorRestoreFromCache = new TMenuItem(this);
	FMenuEditorRestoreFromCache->Action = FActionEditorRestoreFromCache;

	Edit4->Insert(0, FMenuEditorRestoreFromCache);

	Edit4->NewTopLine();

	FMenuEditorSelectionUndo = NewItem("Selection Undo", 0, false, true, 0, 0, "MenuEditorSelectionUndo");
	Edit4->Insert(0, FMenuEditorSelectionUndo);

	FMenuEditorUndo = NewItem("Undo", 0, false, true, 0, 0, "MenuEditorUndo");
	Edit4->Insert(0, FMenuEditorUndo);

	OnShapesAlign = EditorOnShapesAligned;

	Cut1->ImageIndex = 108;
	Cut1->Caption = Cut1->Caption + " (Ctrl+X)";
	Copy1->ImageIndex = 110;
	Copy1->Caption = Copy1->Caption + " (Ctrl+C)";
	Paste1->ImageIndex = 112;
	Paste1->Caption = Paste1->Caption + " (Ctrl+V)";
	Delete1->ImageIndex = 116;
	Delete1->Caption = Delete1->Caption + " (Del)";

	FMenuClone = new TMenuItem(this);
	FMenuClone->Caption = "Clone (Ctrl+K)";
	FMenuClone->OnClick = OnMenuClone;
	FMenuClone->ImageIndex = 114;
	PopupNode->Items->Insert(Cut1->MenuIndex + 1, FMenuClone);

	for (int i = 0; i < PanelBookmarks->ControlCount; i++) {
		TSpeedButton * ASpeedButton = dynamic_cast<TSpeedButton*>(PanelBookmarks->Controls[i]);
		if (ASpeedButton && ASpeedButton->Name.Pos("BtnBookmark")) {
			FBookmarkButtons.push_back(ASpeedButton);
			ASpeedButton->OnClick = OnBookmarkBtnClick;
			ASpeedButton->Hint = GetBookmarkHelp(ASpeedButton);
		}
	}

	FHorizOffsetFix = 0;
	FVertOffsetFix = 0;

	FAppEventsPtr.reset(new TApplicationEvents(this));
	FAppEventsPtr->OnIdle = DoOnEditorApplicationEventsIdle;
	FAppEventsPtr->OnModalEnd = DoOnEditorApplicationEventsModalEnd;

	EditNodeTreeSearch->OnChange = OnEditNodeTreeSearchChange;
	EditNodeTreeSearch->OnKeyPress = OnEditNodeTreeKeyPress;
	EditNodeTreeSearch->OnKeyDown = OnEditNodeTreeKeyDown;

	BtnToolUndo->OnDropDownClick = ButtonUndoRedoDropDownClick;
	BtnToolUndo->OnClick = ExecuteUndo;
	BtnToolRedo->OnDropDownClick = ButtonUndoRedoDropDownClick;
	BtnToolRedo->OnClick = ExecuteRedo;

	BtnSelectBack->OnDropDownClick = ButtonSelectionDropDownClick;
	BtnSelectBack->OnClick = ExecuteUndoSelection;
	BtnSelectForward->OnDropDownClick = ButtonSelectionDropDownClick;
	BtnSelectForward->OnClick = ExecuteRedoSelection;

	NodeTree->OnAfterDraw = NodeTreeAfterDraw;
	NodeTree->OnEnter = NodeTreeOnEnter;
	NodeTree->OnExit = NodeTreeOnExit;

	this->OnActivate = OnActivateTreeEditor;
	this->OnDeactivate = OnDectivateTreeEditor;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnActivateTreeEditor(TObject *Sender) {
	DoInvalidateTrees();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnDectivateTreeEditor(TObject *Sender) {
	DoInvalidateTrees();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoInvalidateTrees() {
	if (NodeTree && IsWindowVisible(NodeTree->Handle)) {
		NodeTree->Invalidate();
	}
	DoInvalidateFocusedTheTree();
}

// ---------------------------------------------------------------------------
__fastcall TTreeEditorEx::~TTreeEditorEx() {
	ClearAllUndoRedo();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::Copyaspicture1Click(System::TObject* Sender) {
	TTreeEx *ATreeEx = dynamic_cast<TTreeEx*>(TheTree);
	if (ATreeEx) {
		const bool bWasFocusDisabled = ATreeEx->DisableFocus;
		try {
			ATreeEx->DisableFocus = true;
			ATreeEx->CopyToClipboardBitmap();
		}
		__finally {
			ATreeEx->DisableFocus = bWasFocusDisabled;
		}
	}
	else {
		TTreeEditorNew::Copyaspicture1Click(Sender);
	}
}

// ---------------------------------------------------------------------------
TTreeClass __fastcall TTreeEditorEx::TeeTreeClass() {
	return __classid(TTreeEx);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::NodeTreeOnEnter(System::TObject* Sender) {
	TCustomTree *ACustomTree = dynamic_cast<TCustomTree*>(Sender);
	if (ACustomTree) {
		ACustomTree->Invalidate();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::NodeTreeOnExit(System::TObject* Sender) {
	TCustomTree *ACustomTree = dynamic_cast<TCustomTree*>(Sender);
	if (ACustomTree) {
		ACustomTree->Invalidate();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeExit(TObject *Sender) {
	TCustomTree *ACustomTree = dynamic_cast<TCustomTree*>(Sender);
	if (ACustomTree) {
		ACustomTree->Invalidate();
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TTreeEditorEx::GetModifiedCacheFile(void)const {
	if (this->CurrentFile.IsEmpty()) {
		return L"";
	}
	std::wstringstream wss;
	const UnicodeString sFileName = TPath::GetFileNameWithoutExtension(this->CurrentFile);
	wss << std::hex << L"autosave_" << sFileName.c_str() << L"_" << boost::hash<std::wstring>()
		(this->CurrentFile.c_str());
	const UnicodeString sHash = wss.str().c_str();
	return TPath::Combine(SettingsData->GetCacheDir(), sHash);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnMenuRestoreFromCache(TObject *Sender) {
	try {
		const UnicodeString sCacheFile = this->GetModifiedCacheFile();
		if (!sCacheFile.IsEmpty() && FileExists(sCacheFile)) {
			std::auto_ptr<TMemoryStream>AMemoryStreamPtr(new TMemoryStream());
			AMemoryStreamPtr->LoadFromFile(sCacheFile);
			AddUndo(LITERAL_REVERT_FROM_CACHE, AMemoryStreamPtr.get(), false);

			DoUndo();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnActionRestoreFromCacheUpdate(TObject *Sender) {
	bool bEnabled = false;
	const UnicodeString sFileCache = this->GetModifiedCacheFile();
	UnicodeString sTimeStamp = L"";
	if (!sFileCache.IsEmpty()) {
		if (FileExists(sFileCache)) {
			bEnabled = true;
			const TDateTime dtLastWrite = TFile::GetLastWriteTime(sFileCache);
			sTimeStamp = L" [" + dtLastWrite.FormatString("DD.MM.YYYY hh:mm:ss") + "]";
		}
	}
	FActionEditorRestoreFromCache->Enabled = bEnabled;
	FActionEditorRestoreFromCache->Caption = LITERAL_REVERT_FROM_CACHE + sTimeStamp;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ActionAlignToLeftExecute(System::TObject* Sender) {
	const int iSelectedCount = TheTree->Selected->Count();
	if (iSelectedCount > 1) {
		const int tmp = TheTree->Selected->First()->X0;
		for (int i = 1; i < iSelectedCount; i++) {
			const bool bMoveChilds = !IsControlPressed();

			int iOffset = 0;
			if (CheckUseAlignOffset->Checked) {
				iOffset = SpinAlignOffset->Value -
					(TheTree->Selected->Items[i]->Y0 - TheTree->Selected->Items[i - 1]->Y1);
			}

			TheTree->Selected->Items[i]->MoveRelative(tmp - TheTree->Selected->Items[i]->X0, iOffset, bMoveChilds);
		}

		if (OnShapesAlign) {
			OnShapesAlign(Sender);
		}

		TeeModified(True, 1, "Align to left [" + TheTree->Selected->First()->SimpleText + "]");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ActionAlignToRightExecute(System::TObject* Sender) {
	const int iSelectedCount = TheTree->Selected->Count();
	if (iSelectedCount > 1) {
		const int tmp = TheTree->Selected->First()->X1;
		for (int i = 1; i < iSelectedCount; i++) {
			const bool bMoveChilds = !IsControlPressed();

			int iOffset = 0;
			if (CheckUseAlignOffset->Checked) {
				iOffset = SpinAlignOffset->Value -
					(TheTree->Selected->Items[i]->Y0 - TheTree->Selected->Items[i - 1]->Y1);
			}

			TheTree->Selected->Items[i]->MoveRelative(tmp - TheTree->Selected->Items[i]->X1, iOffset, bMoveChilds);
		}

		if (OnShapesAlign) {
			OnShapesAlign(Sender);
		}

		TeeModified(True, 1, "Align to right [" + TheTree->Selected->First()->SimpleText + "]");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ActionAlignToTopExecute(System::TObject* Sender) {
	const int iSelectedCount = TheTree->Selected->Count();
	if (iSelectedCount > 1) {
		const int tmp = TheTree->Selected->First()->Y0;
		for (int i = 1; i < iSelectedCount; i++) {
			const bool bMoveChilds = !IsControlPressed();

			int iOffset = 0;
			if (CheckUseAlignOffset->Checked) {
				iOffset = SpinAlignOffset->Value -
					(TheTree->Selected->Items[i]->X0 - TheTree->Selected->Items[i - 1]->X1);
			}

			TheTree->Selected->Items[i]->MoveRelative(iOffset, tmp - TheTree->Selected->Items[i]->Y0, bMoveChilds);

		}

		if (OnShapesAlign) {
			OnShapesAlign(Sender);
		}

		TeeModified(True, 1, "Align to top [" + TheTree->Selected->First()->SimpleText + "]");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ActionAlignToBottomExecute(System::TObject* Sender) {
	const int iSelectedCount = TheTree->Selected->Count();
	if (iSelectedCount > 1) {
		const int tmp = TheTree->Selected->First()->Y1;
		for (int i = 1; i < iSelectedCount; i++) {
			const bool bMoveChilds = !IsControlPressed();

			int iOffset = 0;
			if (CheckUseAlignOffset->Checked) {
				iOffset = SpinAlignOffset->Value -
					(TheTree->Selected->Items[i]->X0 - TheTree->Selected->Items[i - 1]->X1);
			}

			TheTree->Selected->Items[i]->MoveRelative(iOffset, tmp - TheTree->Selected->Items[i]->Y1, bMoveChilds);
		}

		if (OnShapesAlign) {
			OnShapesAlign(Sender);
		}

		TeeModified(True, 1, "Align to bottom [" + TheTree->Selected->First()->SimpleText + "]");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ActionAlignHorCenterExecute(System::TObject* Sender) {
	const int iSelectedCount = TheTree->Selected->Count();
	if (iSelectedCount > 1) {
		const int tmp = TheTree->Selected->First()->XCenter();
		for (int i = 1; i < iSelectedCount; i++) {
			const bool bMoveChilds = !IsControlPressed();

			int iOffset = 0;
			if (CheckUseAlignOffset->Checked) {
				iOffset = SpinAlignOffset->Value -
					(TheTree->Selected->Items[i]->Y0 - TheTree->Selected->Items[i - 1]->Y1);
			}

			TheTree->Selected->Items[i]->MoveRelative(tmp - TheTree->Selected->Items[i]->XCenter(), iOffset,
				bMoveChilds);
		}

		if (OnShapesAlign) {
			OnShapesAlign(Sender);
		}

		TeeModified(True, 1, "Align to horizontal center [" + TheTree->Selected->First()->SimpleText + "]");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ActionAlignVertCenterExecute(System::TObject* Sender) {
	const int iSelectedCount = TheTree->Selected->Count();
	if (iSelectedCount > 1) {
		const int tmp = TheTree->Selected->First()->YCenter();
		for (int i = 1; i < iSelectedCount; i++) {
			const bool bMoveChilds = !IsControlPressed();

			int iOffset = 0;
			if (CheckUseAlignOffset->Checked) {
				iOffset = SpinAlignOffset->Value -
					(TheTree->Selected->Items[i]->X0 - TheTree->Selected->Items[i - 1]->X1);
			}

			TheTree->Selected->Items[i]->MoveRelative(iOffset, tmp - TheTree->Selected->Items[i]->YCenter(),
				bMoveChilds);

		}

		if (OnShapesAlign) {
			OnShapesAlign(Sender);
		}

		TeeModified(True, 1, "Align to vertical center [" + TheTree->Selected->First()->SimpleText + "]");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ActionConnAdjustSidesExecute(System::TObject* Sender) {
	/* inherited */
	TTreeEditorNew::ActionConnAdjustSidesExecute(Sender);

	if (TheTree->Connections->Selected) {
		Editorutils::AdjustSidesConnection(TheTree->Connections->Selected);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::EditorOnShapesAligned(TObject *Sender) {
	AdjustConnectionsOfSelectedShapes();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::AddMenuUndoItems(TMenuItem *AParentItem) {
	bool bAfterIterator = false;

	AParentItem->Clear();

	for (std::size_t i = 0; i != m_UndoQueue.size(); i++) {
		TMenuItem *AMenuItem = new TMenuItem(AParentItem);
		AMenuItem->Caption = m_UndoQueue[i].first;
		AMenuItem->OnClick = UndoRedoClick;
		AMenuItem->Tag = i;

		AParentItem->Add(AMenuItem);

		if (m_undo_it == m_UndoQueue.begin() + i) {
			bAfterIterator = true;
		}

		if (bAfterIterator) {
			AMenuItem->Checked = true;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::AddMenuSelectionItems(TMenuItem *AParentItem) {
	bool bAfterIterator = false;

	AParentItem->Clear();

	CheckForMissedSelectionElements();

	for (std::size_t i = 0; i != m_SelectionQueue.size(); i++) {
		TMenuItem *AMenuItem = new TMenuItem(AParentItem);
		AMenuItem->Caption = m_SelectionQueue[i].first;
		AMenuItem->OnClick = UndoRedoSelectionClick;
		AMenuItem->Tag = i;

		AParentItem->Add(AMenuItem);

		if (m_selection_it == m_SelectionQueue.begin() + i) {
			bAfterIterator = true;
		}

		if (bAfterIterator) {
			AMenuItem->Checked = true;
		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::Edit4Click(System::TObject* Sender) {
	TTreeEditorNew::Edit4Click(Sender);

	Paste2->Enabled = FCliboardEditorEx->Roots->Count > 0;

	// основной Undo
	AddMenuUndoItems(FMenuEditorUndo);

	// SelectionUndo
	AddMenuSelectionItems(FMenuEditorSelectionUndo);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ClearAllUndoRedo(void) {
	if (!FLockClearUndo) {
		while (!m_UndoQueue.empty()) {
			delete m_UndoQueue.front().second;
			m_UndoQueue.pop_front();
		}

		m_undo_it = m_UndoQueue.end();

		if (FIntermediateUndoStreamPtr.get()) {
			FIntermediateUndoStreamPtr->Clear();
		}

		m_SelectionQueue.clear();
		m_selection_it = m_SelectionQueue.end();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::UndoRedoClick(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {
		m_undo_it = m_UndoQueue.begin() + AMenuItem->Tag;
		DoUndo();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::UndoRedoSelectionClick(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {
		m_selection_it = m_SelectionQueue.begin() + AMenuItem->Tag;
		DoUndoSelection();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::AddUndo(const UnicodeString &sDescription, TMemoryStream *ATreeStream /* = NULL */ ,
	bool bSaveCache /* = true */ ) {
	if (TheTree && FIntermediateUndoStreamPtr.get()) {
		SettingsData->ProfilerReset(__FUNCTION__);

		if (FIntermediateUndoStreamPtr->Size && !ATreeStream) {
			AddUndo("UndoRedo fixed", FIntermediateUndoStreamPtr.get(), false);
			FIntermediateUndoStreamPtr->Clear();
		}

		if (m_UndoQueue.size() >= FMaxHistoryCount) {
			TMemoryStream *AStream = m_UndoQueue.back().second;
			delete AStream;
			m_UndoQueue.pop_back();
		}

		TMemoryStream *ANewStream = new TMemoryStream;
		if (!ATreeStream) {
			// bugfix: может записаться не дефолтный курсор
			const TCursor ACursor = TheTree->Cursor;
			TheTree->Cursor = crDefault;
			SaveTreeToStream(TheTree, ANewStream);
			TheTree->Cursor = ACursor;
		}
		else {
			ATreeStream->Position = 0;
			ANewStream->LoadFromStream(ATreeStream);
		}

		if (bSaveCache && SettingsData->AutoSaveCache) {
			try {
				const UnicodeString sCacheFile = this->GetModifiedCacheFile();
				if (!sCacheFile.IsEmpty()) {
					ANewStream->SaveToFile(sCacheFile);
				}
			}
			catch(Exception * E) {
				LOG_ERROR(LOG_ERROR_MSG);
			}
		}

		m_UndoQueue.push_front(std::make_pair(sDescription, ANewStream));
		m_undo_it = m_UndoQueue.begin();

		SettingsData->ProfileOperation(__FUNCTION__);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::AddSelectionUndo(const UnicodeString &ADescription, GUID* AGuid) {
	if (AGuid) {

		if (m_SelectionQueue.size() > 20) {
			if (m_selection_it == (m_SelectionQueue.end() - 1)) {
				m_selection_it = m_SelectionQueue.end() - 2;
			}
			m_SelectionQueue.pop_back();
		}

		// проверим, что предыдущее выделение не эта же фигура
		const bool bPreviousSelectionIsTheSame = m_SelectionQueue.size() && m_SelectionQueue.begin()->second == AGuid;
		// текущее выделение не эта фигура
		const bool bCurrentSelectionIsTheSame = m_selection_it != m_SelectionQueue.end()
			&& m_selection_it->second == AGuid;
		if (!bPreviousSelectionIsTheSame && !bCurrentSelectionIsTheSame) {
			m_SelectionQueue.push_front(std::make_pair(ADescription, AGuid));
			m_selection_it = m_SelectionQueue.begin();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ExecuteUndo(System::TObject* Sender) {
	if (m_UndoQueue.size()) {
		// так как будем приращивать на единицу, то конец очереди - 1
		const TUndoQueue::const_iterator it_end = m_UndoQueue.end() - 1;

		if (m_undo_it != it_end) {
			++m_undo_it; // приращиваем сначала, так как последний undo - это наше состояние на момент undo
			DoUndo();
		}
		else {
			ShowMessage("Undo queue is finished!");
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ExecuteRedo(System::TObject* Sender) {
	if (m_UndoQueue.size()) {
		if (m_undo_it != m_UndoQueue.begin()) {
			--m_undo_it;
			DoUndo();
		}
		else {
			ShowMessage("Redo queue is finished!");
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ExecuteUndoSelection(System::TObject* Sender) {
	if (m_SelectionQueue.size()) {

		// здесь может поменяться размер
		CheckForMissedSelectionElements();

		if (m_SelectionQueue.size()) {

			// так как будем приращивать на единицу, то конец очереди - 1
			const TSelectionQueue::const_iterator it_end = m_SelectionQueue.end() - 1;

			if (m_selection_it != it_end) {
				++m_selection_it; // приращиваем сначала, так как последний undo - это наше состояние на момент undo
				DoUndoSelection();
			}
			else {
				ShowMessage("Undo selection queue is finished!");
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ExecuteRedoSelection(System::TObject* Sender) {
	if (m_SelectionQueue.size()) {

		CheckForMissedSelectionElements();

		if (m_SelectionQueue.size()) {
			if (m_selection_it != m_SelectionQueue.begin()) {
				--m_selection_it;
				DoUndoSelection();
			}
			else {
				ShowMessage("Redo selection queue is finished!");
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoUndo(void) {
	(*m_undo_it).second->Position = 0;
	if ((*m_undo_it).second->Size) {

		{
			const Editorutils::TreeUpdateLock ATreeLock(TheTree);

			// запоминаем смещение и зум
			const int iHorzOffset = TheTree->View3DOptions->HorizOffset;
			const int iVertOffset = TheTree->View3DOptions->VertOffset;
			const int iZoom = TheTree->View3DOptions->Zoom;

			LoadTreeFromStreamEx(TheTree, (*m_undo_it).second);

			// запоминаем и в случае, если новое событие после отката,
			(*m_undo_it).second->Position = 0;
			FIntermediateUndoStreamPtr->LoadFromStream((*m_undo_it).second);

			DeleteTemporaryShapes();

			// восстанавливаем смещение и зум
			this->ChangeZoom(iZoom);
			TheTree->View3DOptions->HorizOffset = iHorzOffset;
			TheTree->View3DOptions->VertOffset = iVertOffset;
		}

		TheTree->Cursor = crDefault; // bugfix with cursor
		TheTree->OriginalCursor = crDefault;

		FillNodeTree();
		TeeModified();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::CheckForMissedSelectionElements() {
	for (TSelectionQueue::iterator i = m_SelectionQueue.begin(); i != m_SelectionQueue.end();
		/* NOTE: no incrementation of the iterator here */ ) {
		if (Editorutils::IndexOfShapeByGuid(TheTree, i->second) == -1 && Editorutils::IndexOfConnectionByGuid(TheTree,
				i->second) == -1) {

			bool bRequireChange = false;
			if (m_selection_it == i) {
				bRequireChange = true;
			}

			i = m_SelectionQueue.erase(i); // erase returns the next iterator

			if (bRequireChange) {
				m_selection_it = (i != m_SelectionQueue.end() && i != m_SelectionQueue.begin()) ? (i - 1) : i;
			}
		}
		else
			++i; // otherwise increment it by yourself
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoUndoSelection(void) {
	int iIndex = Editorutils::IndexOfShapeByGuid(TheTree, m_selection_it->second);
	if (iIndex != -1) {
		m_LockAddSelectionUndo = true;
		SelectShape(TheTree->Shapes->Items[iIndex], true);
		m_LockAddSelectionUndo = false;
	}
	else {

		iIndex = Editorutils::IndexOfConnectionByGuid(TheTree, m_selection_it->second);
		if (iIndex != -1) {
			m_LockAddSelectionUndo = true;
			SelectConnection(TheTree->Connections->Items[iIndex], true);
			m_LockAddSelectionUndo = false;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::SetNewTreeSettings(void) {
	TTreeEditorNew::SetNewTreeSettings(); // inherited

	TheTree->OnKeyDown = TheTreeKeyDown;
	TheTree->OnResizingShape = TheTreeResizingShape;
	TheTree->OnMovingShape = TheTreeMovingShape;
	TheTree->OnMouseDown = TheTreeMouseDown;
	TheTree->OnMouseUp = TheTreeMouseUp;
	TheTree->OnClickConnection = TheTreeClickConnection;
	TheTree->OnSelectShape = TheTreeSelectShape;
	TheTree->OnSelectConnection = TheTreeSelectConnection;
	TheTree->OnAfterDraw = TheTreeAfterDraw;
	TTreeEx *ATreeEx = dynamic_cast<TTreeEx*>(TheTree);
	if (ATreeEx) {
		ATreeEx->OnWheelZoom = DoOnWheelZoom;
		ATreeEx->OnKeyUp = TheTreeKeyUp; // он в Protected, поэтому объявил в TreeEx
		ATreeEx->OnExit = TheTreeExit;
	}

	TheTree->Selected->HandleColor = clSilver;
	TheTree->Selected->HandlePen->Visible = true;

	ClearAllUndoRedo();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoAddFillNodePicture(TTreeNodeShape *AToShape, TCustomTreeElement* AFromElement) {
	if (TTreeNodeShape * AFromShape = dynamic_cast<TTreeNodeShape*>(AFromElement)) {
		if (AFromShape->ImageIndex == tiNone && AFromShape->Image) {
			// особый случай фигуры с картинками, необходимо их загружать каждый раз,
			// так как могут активно меняться и будут индивидуальными
			if (TImageShape * AImageShape = dynamic_cast<TImageShape*>(AFromShape)) {
				AToShape->Image->Assign(AFromShape->Image);
			}
			else {
				// оптимизация, добавляем в Pool картинку такого типа,
				// чтобы потом только индекс указывать
				std::map<TMetaClass*, int>::iterator it = g_MapImageIndexes.find(AFromShape->ClassType());
				if (it == g_MapImageIndexes.end()) {
					// если картинка будет более 16 на 16, тогда будет растягивать элемент в списке,
					// поэтому ужимаем
					if (AFromShape->Image->Width > 16 || AFromShape->Image->Height > 16) {
						std::auto_ptr<TPicture>APicturePtr(new TPicture());
						Imgutils::ResizeGraphicSmooth(APicturePtr->Bitmap, AFromShape->Image->Graphic, 16, 16);

						AToShape->ImageIndex = TreeImagePool->Add(APicturePtr.get());
					}
					else {
						AToShape->ImageIndex = TreeImagePool->Add(AFromShape->Image);
					}

					g_MapImageIndexes.insert(std::make_pair(AFromShape->ClassType(), AToShape->ImageIndex));
				}
				else {
					AToShape->ImageIndex = TTreeNodeImageIndex(it->second);
				}
			}
		}
		else {
			AToShape->ImageIndex = AFromShape->ImageIndex;
		}
	}
}

// ---------------------------------------------------------------------------
TTreeNodeShape * __fastcall TTreeEditorEx::DoProcessAddFillNode(TTreeNodeShape *AToNode,
	TCustomTreeElement * AFromElement) {
	if (AFromElement) {
		TTreeNodeShape *tmpNode = NodeTree->AddShape(0, 0, NodeTreeText(AFromElement), AToNode);

		tmpNode->Tag = reinterpret_cast<int>(AFromElement);

		DoAddFillNodePicture(tmpNode, AFromElement);

		return tmpNode;
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoAddFillNodeChilds(TTreeNodeShape *AToNode, TNodeShapeList * AFromNodes) {
	for (int t = 0; t < AFromNodes->Count; t++) {
		TTreeNodeShape *tmpNode = DoProcessAddFillNode(AToNode, AFromNodes->Items[t]);
		if (tmpNode) {
			DoAddFillNodeChilds(tmpNode, AFromNodes->Items[t]->Children);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::FillNodeTree(void) {

	if (NodeTheTree->Checked) {
		Selecting = true;
		NodeTree->BeginUpdate();
		EditNodeTreeSearch->Tag = 1;
		try {
			NodeTree->GlobalFormat.Border->Visible = false;
			NodeTree->GlobalFormat.Transparent = true;
			NodeTree->GlobalFormat.NodeClass = __classid(TMaskedTextShape);
			NodeTree->Clear();
			if (SettingsData) {
				NodeTree->View3DOptions->HorizOffset = SettingsData->ChartFocusLineWidth;
				NodeTree->View3DOptions->VertOffset = SettingsData->ChartFocusLineWidth;
			}

			const UnicodeString sText = EditNodeTreeSearch->Text.UpperCase();

			DoAddFillNodeChilds(NULL, TheTree->Roots);

			for (int t = 0; t < NodeTree->Roots->Count; t++) {
				NodeTree->Roots->Items[t]->Expanded = true;
			}

			if (!EditNodeTreeSearch->Text.IsEmpty()) {
				const UnicodeString sText = EditNodeTreeSearch->Text.UpperCase();

				std::set<TTreeNodeShape*>AVisibleShapes;
				std::set<TTreeNodeShape*>ATrueVisibleShapes;

				for (int i = 0; i < NodeTree->Shapes->Count; i++) {
					TMaskedTextShape *AMaskedShape = dynamic_cast<TMaskedTextShape*>(NodeTree->Shapes->Items[i]);
					if (!AMaskedShape)
						throw Exception("Program logic error! <NodeTree> require <TMaskedTextShape> as node class!");

					const int iMaskPos = NodeTree->Shapes->Items[i]->SimpleText.UpperCase().Pos(sText);
					if (iMaskPos) {
						AVisibleShapes.insert(NodeTree->Shapes->Items[i]);
						ATrueVisibleShapes.insert(NodeTree->Shapes->Items[i]);
						AMaskedShape->SetMask(sText, iMaskPos);

						TTreeNodeShape *AParent = NodeTree->Shapes->Items[i]->Parent;
						while (AParent) {

							AParent->Expanded = true;

							AVisibleShapes.insert(AParent);

							AParent = AParent->Parent;
						}
					}
				}

				for (int i = NodeTree->Shapes->Count - 1; i >= 0; i--) {
					if (AVisibleShapes.find(NodeTree->Shapes->Items[i]) == AVisibleShapes.end()) {
						delete NodeTree->Shapes->Items[i];
					}
					else {
						if (ATrueVisibleShapes.find(NodeTree->Shapes->Items[i]) == ATrueVisibleShapes.end()) {
							NodeTree->Shapes->Items[i]->Font->Color = clGray;
							NodeTree->Shapes->Items[i]->Font->Style = TFontStyles();
						}
					}
				}
			}
		}
		__finally {
			Selecting = false;
			NodeTree->EndUpdate();
			ResetNodeTreeSelected();
			EditNodeTreeSearch->Tag = 0;
		}
	}

	TTreeEx * ATreeEx = dynamic_cast<TTreeEx*>(TheTree);
	if (ATreeEx) {

		for (std::size_t i = 0; i < FBookmarkButtons.size(); i++) {
			const UnicodeString sShapeName = ATreeEx->Bookmarks->Values[FBookmarkButtons[i]->Name];
			if (sShapeName.IsEmpty()) {
				if (FBookmarkButtons[i]->Tag) {
					DoSetBookmark(FBookmarkButtons[i], NULL);
				}
			}
			else {
				TScxmlBaseShape *AShape = dynamic_cast<TScxmlBaseShape*>(this->FindComponent(sShapeName));
				if (AShape) {
					if (FBookmarkButtons[i]->Tag != reinterpret_cast<int>(AShape->Guid)) {
						DoSetBookmark(FBookmarkButtons[i], AShape);
					}
				}
				else {
					DoSetBookmark(FBookmarkButtons[i], NULL);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ResetTree(void) {
	try {
		if (TheTree->Connecting) {
			TheTree->StopConnecting();
		}
		if (TheTree->Editing) {
			TheTree->StopEditing();
		}

		ClearFlags();

		TTreeEditorNew::ResetTree();
		/* inherited */

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ClearFlags(void) {
	FTreeEnsureSelectedInView = false;
	FTreeInitConnectionFlag = false;
	FTreeUndoDescriptionFlag = L"";
	FTreeInvalidateFlag = false;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TeeModified(bool YesNo /* = true */ , int ACat /* = 0x0 */ ,
	const System::UnicodeString ADescription /* = L"" */ ) {

	TTreeEditorNew::TeeModified(YesNo, ACat, ADescription);

	switch(ACat) {
	case 0: {
		}break;
	case 1: {
			// тяжелая операция на больших схемах
			FTreeUndoDescriptionFlag = ADescription;
		}break;
	case 2: {
		}break;
	}

	if (OnTreeEditorModified) {
		OnTreeEditorModified(this);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::Cut1Click(System::TObject* Sender) {
	DoCut();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoCut(void) {
	try {
		FCliboardEditorEx->Cut(this->TheTree);
		FillNodeTree();
		TeeModified(true, 1, "Cut");
	}
	catch(Exception * E) {
		WLOG_ERROR(L"CUT> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::Copy1Click(System::TObject* Sender) {
	try {
		FCliboardEditorEx->Copy(this->TheTree);
	}
	catch(Exception * E) {
		WLOG_ERROR(L"COPY> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoPaste(bool UseCursorPos) {
	try {
		// если что-нибудь вставлено
		if (FCliboardEditorEx->Paste(this->TheTree, UseCursorPos)) {
			FillNodeTree();
			TeeModified(true, 1, "Paste");

			EnsureSelectedInView();
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"PASTE> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::Paste1Click(System::TObject* Sender) {
	DoPaste(false);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::PopupNodePopup(System::TObject* Sender) {
	TTreeEditorNew::PopupNodePopup(Sender);

	Paste1->Enabled = Clipboard()->HasFormat( //
		// или если дерево
		CF_CUSTOM_TREE) || //
	// или если в буфере какой-то текст
	(Clipboard()->HasFormat(CF_TEXT) && (TheTree->Connections->Selected || TheTree->Selected->Count()) && //
		IsStateMachineConnectionClipboardFormat(Clipboard()->AsText));
	FMenuClone->Enabled = TheTree->Selected->Count() != 0;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoInvalidateFocusedTheTree(void) {
	if (TheTree && IsWindowVisible(TheTree->Handle) && TheTree->Focused()) {
		this->InvalidateTheTreeWithTemp();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::InvalidateTheTreeWithTemp(void) {
	const TTemporarySelectedRectangle AWasTemporarySelectedRectangle = FTemporarySelectedRectangle;
	TheTree->Invalidate();
	FTemporarySelectedRectangle = AWasTemporarySelectedRectangle;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::RefreshTheTreeWithTemp(void) {
	const TTemporarySelectedRectangle AWasTemporarySelectedRectangle = FTemporarySelectedRectangle;
	TheTree->Refresh();
	FTemporarySelectedRectangle = AWasTemporarySelectedRectangle;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoOnEditorApplicationEventsModalEnd(TObject *Sender) {
	DoInvalidateFocusedTheTree();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoOnEditorApplicationEventsIdle(TObject *Sender, bool &Done) {

	if (FTreeEnsureSelectedInView) {
		FTreeEnsureSelectedInView = false;

		if (this->TheTree->Selected->Count()) {
			// у не визуальных фигур в режиме OnIdle может поменяться дочерний индекс,
			// поэтому вызываем обновление, чтобы сформировались границы
			if (!this->TheTree->Selected->Items[0]->InheritsFrom(__classid(TVisualScxmlBaseShape))) {
				this->TheTree->Refresh();
			}

			EnsureInView(this->TheTree->Selected->Items[0]);
		}
	}

	// инициирование связывания фигур на графике
	if (FTreeInitConnectionFlag) {
		FTreeInitConnectionFlag = false;

		SpeedLink->Down = true;
		SpeedLink->Click();
	}

	/* на очень больших схемах Undo занимает много времени */
	if (!FTreeUndoDescriptionFlag.IsEmpty()) {
		try {
			AddUndo(FTreeUndoDescriptionFlag);
		}
		__finally {
			FTreeUndoDescriptionFlag = L""; // обнулять после
		}
	}

	/* перерисовка дерева */
	/* лечим глюки, связанные с артефактами от инверсных отрисовок типа DrawHandles и т.д. */
	if (FTreeInvalidateFlag) {
		FTreeInvalidateFlag = false;

		this->InvalidateTheTreeWithTemp();
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TTreeEditorEx::IsModified(void) {
	return this->FModified;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::MoveConnectionPoints(TTreeConnection *AConnection, const TPointPinType AMoveType) {
	if (AConnection) {
		int iDeltaX = 0;
		int iDeltaY = 0;

		switch(AMoveType) {
		case pptLeft:
			iDeltaX = -1;
			break;
		case pptTop:
			iDeltaY = -1;
			break;
		case pptRight:
			iDeltaX = 1;
			break;
		case pptBottom:
			iDeltaY = 1;
			break;
		}

		for (int i = 0; i < AConnection->Points->Count(); i++) {

			switch(AConnection->Points->Item[i].YStyle) {
			case cpsAutoFrom:
				AConnection->Points->ChangeYStyle(i, cpsFromRel);
				break;
			case cpsAutoTo:
				AConnection->Points->ChangeYStyle(i, cpsToRel);
				break;
			}

			switch(AConnection->Points->Item[i].XStyle) {
			case cpsAutoFrom:
				AConnection->Points->ChangeXStyle(i, cpsFromRel);
				break;
			case cpsAutoTo:
				AConnection->Points->ChangeXStyle(i, cpsToRel);
				break;
			}

			AConnection->Points->Move(i, iDeltaX, iDeltaY);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnFullScreenFormCloseQuery(TObject * Sender, bool&CanClose) {
	CanClose = false;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoSelectAllShapes() {
	TheTree->Selected->SelectAll();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeKeyDown(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift) {
	// стираем временный прямоугольник
	ForceResetTemporarySelected();

	bool bNeedToDeleteFullScreenForm = false;

	switch(Key) {
	case VK_BACK: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				ExecuteUndoSelection(Sender);
			}
			else if (Shift == Classes::TShiftState() << ssShift << ssCtrl) {
				ExecuteRedoSelection(Sender);
			}
		}break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5': {

			int iIndex = 0;
			switch(Key) {
			case '1':
				iIndex = 1;
				break;
			case '2':
				iIndex = 2;
				break;
			case '3':
				iIndex = 3;
				break;
			case '4':
				iIndex = 4;
				break;
			case '5':
				iIndex = 5;
				break;
			}

			iIndex--; // так как в векторе нумерация с нуля

			try {
				if (Shift == Classes::TShiftState() << ssCtrl) {
					SelectShapeFromBookmark(FBookmarkButtons.at(iIndex));
				}
				else if (Shift == Classes::TShiftState() << ssCtrl << ssShift) {
					if (TheTree->Selected->Count()) {
						TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->First());
						if (AScxmlBaseShape) {
							SetBookmark(FBookmarkButtons.at(iIndex),
								FBookmarkButtons.at(iIndex)->Tag == reinterpret_cast<int>(AScxmlBaseShape->Guid)
								? NULL : AScxmlBaseShape);
						}
						else
							LOG_ERROR_SS << "Only scxml shapes are avialable!";
					}
					else {
						LOG_ERROR_SS << "Shape must be selected before!";
					}
				}
			}
			catch(std::exception & e) {
				LOG_ERROR("Set Shift+Ctrl+%d bookmark: %s", iIndex, e.what());
			}

		}break;

	case 'X': {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				Cut1Click(Cut1);
			}
			else if (Shift == Classes::TShiftState()) {
				ActionLockAxisX->Execute();
			}
		}break;
	case 'Y': {
			if (Shift == Classes::TShiftState()) {
				ActionLockAxisY->Execute();
			}
		}break;
	case 'V':
		if (Shift == Classes::TShiftState() << ssCtrl) {
			DoPaste(true);
		}
		break;
	case 'K':
		if (Shift == Classes::TShiftState() << ssCtrl) {
			OnMenuClone(FMenuClone);
		}
		break;
	case 'C':
		if (Shift == Classes::TShiftState() << ssCtrl) {
			Copy1Click(Copy1);
		}
		else if (Shift == Classes::TShiftState() << ssCtrl << ssShift) {
			if (TheTree->Connections->Selected) {
				Clipboard()->Clear();
				if (TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
					(TheTree->Connections->Selected)) {
					if (!AStateMachineConnection->Event.IsEmpty()) {
						Clipboard()->AsText = AStateMachineConnection->Event;
					}
					else if (!AStateMachineConnection->Condition.IsEmpty()) {
						Clipboard()->AsText = AStateMachineConnection->Condition;
					}
					else {
						Clipboard()->AsText = AStateMachineConnection->Text->Text;
					}
				}
				else {
					Clipboard()->AsText = TheTree->Connections->Selected->Text->Text;
				}
			}
			else if (TheTree->Selected->Count()) {
				Clipboard()->Clear();
				TTreeNodeShape *AShape = TheTree->Selected->First();
				if (TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(AShape)) {
					Clipboard()->AsText = AScxmlBaseShape->ClipboardInfo;
				}
				else {
					Clipboard()->AsText = AShape->Text->Text;
				}
			}
		}
		break;
	case 'S':
		if (Shift == Classes::TShiftState() << ssCtrl) {
			this->Save1->Click();
		}
		break;
	case TeeTree_DeleteKey:
		if (TheTree->Connections->Selected) {

			// даем возможность пользователю удалять соединение
			bool bAllowDelete = true;
			TheTreeDeletingConnection(TheTree->Connections->Selected, bAllowDelete);

			if (bAllowDelete) {
				std::auto_ptr<TStringList>AStringListPtr(new TStringList());
				if (TheTree->Connections->Selected->FromShape) {
					AStringListPtr->Values["From"] = TheTree->Connections->Selected->FromShape->SimpleText;
				}
				if (TheTree->Connections->Selected->ToShape) {
					AStringListPtr->Values["To"] = TheTree->Connections->Selected->FromShape->SimpleText;
				}

				delete TheTree->Connections->Selected;
				FillNodeTree();
				TeeModified(True, 1, "Tree connections deleted:" + AStringListPtr->CommaText);
			}

		}
		break;
	case TeeTree_InsertKey:
		if (Shift == Classes::TShiftState() << ssCtrl) {
			Copy1Click(Copy1);
		}
		else if (Shift == Classes::TShiftState() << ssShift) {
			Paste1Click(Paste1);
		}
		break;
	case 'Z':
		if (Shift == Classes::TShiftState() << ssCtrl) {
			ExecuteUndo(Sender);
		}
		else if (Shift == Classes::TShiftState() << ssCtrl << ssShift) {
			ExecuteRedo(Sender);
		}
		break;
	case 'A':
		if (Shift == Classes::TShiftState() << ssCtrl) {
			DoSelectAllShapes();
		}
		break;
	case 'M':
		if (Shift == Classes::TShiftState() << ssCtrl) {
			ActionZoomInCenter->Execute();
		}
		else if (Shift == Classes::TShiftState() << ssShift) {
			ZoomToFitSelection(zftAll, false);
		}
		else if (Shift == Classes::TShiftState() << ssShift << ssCtrl) {
			ZoomToFitSelection(zftWidth, false);
		}
		break;
	case VK_F3:
		if (Shift == Classes::TShiftState() << ssCtrl) {
			ActionZoomToWidth->Execute();
		}
		else if (Shift == Classes::TShiftState() << ssShift) {
			ActionZoomToHeight->Execute();
		}
		else {
			if (TheTree->View3DOptions->Zoom == 100) {
				ActionZoomToFitAll->Execute();
			}
			else {
				ActionZoom100->Execute();
			}
		}
		break;
	case VK_DOWN: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				MoveConnectionPoints(TheTree->Connections->Selected, pptBottom);
			}
		}break;
	case VK_UP: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				MoveConnectionPoints(TheTree->Connections->Selected, pptTop);
			}
		}break;
	case VK_LEFT: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				MoveConnectionPoints(TheTree->Connections->Selected, pptLeft);
			}
		}break;
	case VK_RIGHT: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				MoveConnectionPoints(TheTree->Connections->Selected, pptRight);
			}
		}break;
	case VK_F12: {
			if (!FFullScreenForm) {
				FFullScreenForm = new TForm(this->Owner);
				FFullScreenForm->BorderStyle = bsNone;
				FFullScreenForm->WindowState = wsMaximized;
				this->TheTree->Parent = FFullScreenForm;

				FFullScreenForm->OnCloseQuery = OnFullScreenFormCloseQuery;
				FFullScreenForm->Show();
			}
			else {
				bNeedToDeleteFullScreenForm = true;
			}
		}break;
	case VK_ESCAPE: {
			bNeedToDeleteFullScreenForm = true;
		}break;
	}

	if (bNeedToDeleteFullScreenForm) {
		if (FFullScreenForm) {
			this->TheTree->Parent = PanelTree;
			delete FFullScreenForm;
			FFullScreenForm = NULL;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::NodeTreeKeyDown(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift) {
	try {
		switch(Key) {
		case 'X':
			if (Shift == Classes::TShiftState() << ssCtrl) {
				Cut1Click(Cut1);
			}
			break;
		case 'V':
			if (Shift == Classes::TShiftState() << ssCtrl) {
				DoPaste(true);
			}
			break;
		case 'K':
			if (Shift == Classes::TShiftState() << ssCtrl) {
				OnMenuClone(FMenuClone);
			}
			break;
		case 'C': {
				if (Shift == Classes::TShiftState() << ssCtrl) {
					Copy1Click(Copy1);
				}
				else if (Shift == Classes::TShiftState() << ssCtrl << ssShift) {
					if (TheTree->Connections->Selected) {
						Clipboard()->Clear();
						if (TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
							(TheTree->Connections->Selected)) {
							if (!AStateMachineConnection->Event.IsEmpty()) {
								Clipboard()->AsText = AStateMachineConnection->Event;
							}
							else if (!AStateMachineConnection->Condition.IsEmpty()) {
								Clipboard()->AsText = AStateMachineConnection->Condition;
							}
							else {
								Clipboard()->AsText = AStateMachineConnection->Text->Text;
							}
						}
						else {
							Clipboard()->AsText = TheTree->Connections->Selected->Text->Text;
						}
					}
					else if (TheTree->Selected->Count()) {
						Clipboard()->Clear();
						TTreeNodeShape *AShape = TheTree->Selected->First();
						if (TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(AShape)) {
							Clipboard()->AsText = AScxmlBaseShape->ClipboardInfo;
						}
						else {
							Clipboard()->AsText = AShape->Text->Text;
						}
					}
				}
			}break;
		case TeeTree_DeleteKey:
			if (TheTree->Connections->Selected) {

				std::auto_ptr<TStringList>AStringListPtr(new TStringList());
				if (TheTree->Connections->Selected->FromShape) {
					AStringListPtr->Values["From"] = TheTree->Connections->Selected->FromShape->SimpleText;
				}
				if (TheTree->Connections->Selected->ToShape) {
					AStringListPtr->Values["To"] = TheTree->Connections->Selected->FromShape->SimpleText;
				}

				delete TheTree->Connections->Selected;
				FillNodeTree();
				TeeModified(True, 1, "Tree connections deleted:" + AStringListPtr->CommaText);
			}
			break;
		case TeeTree_InsertKey:
			if (Shift == Classes::TShiftState() << ssCtrl) {
				Copy1Click(Copy1);
			}
			else if (Shift == Classes::TShiftState() << ssShift) {
				Paste1Click(Paste1);
			}
			break;
		case 'Z':
			if (Shift == Classes::TShiftState() << ssCtrl) {
				ExecuteUndo(Sender);
			}
			else if (Shift == Classes::TShiftState() << ssCtrl << ssShift) {
				ExecuteRedo(Sender);
			}
			break;
		case 'A':
			if (Shift == Classes::TShiftState() << ssCtrl) {
				DoSelectAllShapes();
			}
			break;
		case 'M':
			if (Shift == Classes::TShiftState() << ssCtrl) {
				ActionZoomInCenter->Execute();
			}
			else if (Shift == Classes::TShiftState() << ssShift) {
				ZoomToFitSelection(zftAll, false);
			}
			else if (Shift == Classes::TShiftState() << ssShift << ssCtrl) {
				ZoomToFitSelection(zftWidth, false);
			}
			break;
		case VK_F3:
			if (Shift == Classes::TShiftState() << ssCtrl) {
				ActionZoomToWidth->Execute();
			}
			else if (Shift == Classes::TShiftState() << ssShift) {
				ActionZoomToHeight->Execute();
			}
			else {
				if (TheTree->View3DOptions->Zoom == 100) {
					ActionZoomToFitAll->Execute();
				}
				else {
					ActionZoom100->Execute();
				}
			}
			break;
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeKeyUp(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift) {

	switch(Key) {
	case VK_DOWN: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				if (TheTree->Connections->Selected) {
					TeeModified(True, 1, TheTree->Connections->Selected->SimpleText + "> All points moved DOWN");
				}
			}
		}break;
	case VK_UP: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				if (TheTree->Connections->Selected) {
					TeeModified(True, 1, TheTree->Connections->Selected->SimpleText + "> All points moved UP");
				}
			}
		}break;
	case VK_LEFT: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				if (TheTree->Connections->Selected) {
					TeeModified(True, 1, TheTree->Connections->Selected->SimpleText + "> All points moved LEFT");
				}
			}
		}break;
	case VK_RIGHT: {
			if (Shift == Classes::TShiftState() << ssCtrl) {
				if (TheTree->Connections->Selected) {
					TeeModified(True, 1, TheTree->Connections->Selected->SimpleText + "> All points moved RIGHT");
				}
			}
		}break;
	}
	ShapeMoveResizeModified(Key, -1, Shift);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeClickConnection(Teetree::TTreeConnection* Sender, Controls::TMouseButton Button,
	Classes::TShiftState Shift, int X, int Y) {

	TTreeEditorNew::TheTreeClickConnection(Sender, Button, Shift, X, Y);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ForceResetTemporarySelected(void) {
	if (FTemporarySelectedRectangle.Visible) {
		TheTree->Invalidate();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeMouseDown(System::TObject* Sender, Controls::TMouseButton Button,
	Classes::TShiftState Shift, int X, int Y) {
	FMouseShift = Shift;
	// стираем временный прямоугольник
	ForceResetTemporarySelected();

	// необходимо отследить факт добавления точки на выделенное соединение
	FWasConnSelectedOnDown = false;
	FConnSelectedPoints.Length = 0;

	if (TheTree && TheTree->Connections->Selected) {
		FWasConnSelectedOnDown = true;
		FOnDownSelectedConn = TheTree->Connections->Selected;
		FConnSelectedPoints.Length = TheTree->Connections->Selected->Points->Count();
		for (int i = 0; i < FConnSelectedPoints.Length; i++) {
			FConnSelectedPoints[i].XStyle = TheTree->Connections->Selected->Points->Item[i].XStyle;
			FConnSelectedPoints[i].YStyle = TheTree->Connections->Selected->Points->Item[i].YStyle;
			FConnSelectedPoints[i].XValue = TheTree->Connections->Selected->Points->Item[i].XValue;
			FConnSelectedPoints[i].YValue = TheTree->Connections->Selected->Points->Item[i].YValue;
		}
	}
	/* inherited */
	TTreeEditorNew::TheTreeMouseDown(Sender, Button, Shift, X, Y);

	// запоминаем, чтобы отследить смещение графика
	FHorizOffsetFix = TheTree->View3DOptions->HorizOffset;
	FVertOffsetFix = TheTree->View3DOptions->VertOffset;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeMouseUp(System::TObject* Sender, Controls::TMouseButton Button,
	Classes::TShiftState Shift, int X, int Y) {

	FMouseShift = TShiftState();

	/* inherited */
	/// TTreeEditorNew::TheTreeMouseUp(Sender, Button, Shift, X, Y);
	/// в родном редакторе вызывается только TryPopup
	/// так как были замечены ошибки в вызове, то переопределяем ниже

	switch(Button) {
	case mbLeft: {
			// отслеживаем перемещение средних точек
			bool bMediumPointMoved = false;
			bool bIsReconnected = false;
			bool bPointsChanged = false;
			// проверяем, добавилась ли точка
			if (TheTree && TheTree->Connections->Selected && (TheTree->Connections->Selected == FOnDownSelectedConn)
				&& FWasConnSelectedOnDown) {
				if (TheTree->Connections->Selected->Points->Count() != FConnSelectedPoints.Length) {
					bPointsChanged = true;
				}
				else {
					for (int i = 0; i < TheTree->Connections->Selected->Points->Count(); i++) {
						if (TheTree->Connections->Selected->Points->Item[i].XStyle != FConnSelectedPoints[i]
							.XStyle || TheTree->Connections->Selected->Points->Item[i].YStyle != FConnSelectedPoints
							[i].YStyle || TheTree->Connections->Selected->Points->Item[i].XValue != FConnSelectedPoints
							[i].XValue || TheTree->Connections->Selected->Points->Item[i].YValue != FConnSelectedPoints
							[i].YValue) {
							bPointsChanged = true;

							bMediumPointMoved = i != 0 && i != TheTree->Connections->Selected->Points->Count() - 1;

							break;
						}
					}
				}
			}

			UnicodeString sModifiedDesc = L"";
			TTreeConnection* ASelectedConnection = TheTree->Connections->Selected;
			if (TheTree && ASelectedConnection && !TheTree->Connecting && !bMediumPointMoved) {

				const int iCount = ASelectedConnection->Points->Count();
				// если не менее двух точек, тогда может отсоединить последнюю или первую
				if (iCount >= 2) {
					TTreeNodeShape *AShape = TheTree->Shapes->Clicked(X, Y);
					if (AShape) {

						const int iPointIndex = ASelectedConnection->Points->Clicked(X, Y);
						if (iPointIndex != -1) {
							const TConnectionPoint &APoint = ASelectedConnection->Points->Item[iPointIndex];

							const TPoint AConnectionPoint(APoint.X, APoint.Y);

							const int iOffset = 5;

							// проверяем, тянем ли последнюю точку
							if (iPointIndex == iCount - 1) {

								const TRect ARect = ASelectedConnection->ToShape->Bounds();

								const bool bIsInOutsideRect = Editorutils::IsPointInOutsideOffsetRect(AConnectionPoint,
									ARect, iOffset);

								// WLOG_DEBUG(L"Point (xV=%d,yV=%d)(x=%d,y=%d) ToShape(l=%d,t=%d,r=%d,b=%d) IsInPrevRect=%s", APoint.XValue,
								// APoint.YValue, APoint.X, APoint.Y, ARect.Left, ARect.Top, ARect.Right, ARect.Bottom,
								// bIsInOutsideRect ? L"true" : L"false");

								if (ASelectedConnection->ToShape != AShape && !bIsInOutsideRect) {
									ASelectedConnection->ToShape = AShape;

									if (ASelectedConnection->Style == csSides || ASelectedConnection->Style ==
										csInvertedSides) {
										ASelectedConnection->Points->ChangeXStyle(iPointIndex, cpsToRel);
										ASelectedConnection->Points->ChangeYStyle(iPointIndex, cpsToRel);
										ASelectedConnection->Draw();
									}
									else {
										ASelectedConnection->Points->ChangeXStyle(iPointIndex, cpsAutoTo);
										ASelectedConnection->Points->ChangeYStyle(iPointIndex, cpsAutoTo);
										ASelectedConnection->Draw();
									}

									bIsReconnected = true;

									sModifiedDesc = "Reconnected " + ASelectedConnection->FromShape->SimpleText +
										" to " + ASelectedConnection->ToShape->SimpleText;
								}
								else {
									if (ASelectedConnection->Style == csLine) {
										Editorutils::ClipLastConnectionPointToShapeBounds(ASelectedConnection);
									}
								}
							}
							else if (iPointIndex == 0) {

								const TRect ARect = ASelectedConnection->FromShape->Bounds();

								const bool bIsInOutsideRect = Editorutils::IsPointInOutsideOffsetRect(AConnectionPoint,
									ARect, iOffset);

								// WLOG_DEBUG(L"Point (xV=%d,yV=%d)(x=%d,y=%d) ToShape(l=%d,t=%d,r=%d,b=%d) IsInPrevRect=%s", APoint.XValue,
								// APoint.YValue, APoint.X, APoint.Y, ARect.Left, ARect.Top, ARect.Right, ARect.Bottom,
								// bIsInOutsideRect ? L"true" : L"false");

								if (ASelectedConnection->FromShape != AShape && !bIsInOutsideRect) {
									ASelectedConnection->FromShape = AShape;

									if (ASelectedConnection->Style == csSides || ASelectedConnection->Style ==
										csInvertedSides) {
										ASelectedConnection->Points->ChangeXStyle(iPointIndex, cpsFromRel);
										ASelectedConnection->Points->ChangeYStyle(iPointIndex, cpsFromRel);
										ASelectedConnection->Draw();
									}
									else {
										ASelectedConnection->Points->ChangeXStyle(iPointIndex, cpsAutoFrom);
										ASelectedConnection->Points->ChangeYStyle(iPointIndex, cpsAutoFrom);
										ASelectedConnection->Draw();
									}

									bIsReconnected = true;

									sModifiedDesc = "Reconnected " + ASelectedConnection->FromShape->SimpleText +
										" to " + ASelectedConnection->ToShape->SimpleText;
								}
								else {
									if (ASelectedConnection->Style == csLine) {
										Editorutils::ClipFirstConnectionPointToShapeBounds(ASelectedConnection);
									}
								}
							}
						}
					}
				}
			}
			if (bIsReconnected) {
				OnConnectionReconnected(ASelectedConnection);

				TheTree->Refresh();
				FillNodeTree();
				TeeModified(true, 1, sModifiedDesc);
			}
			else if (bPointsChanged) {
				TeeModified(true, 1, "Points changed in [" + ASelectedConnection->Name + "]");
			}
			if (bIsReconnected || bPointsChanged) {
				// соединение может оказаться в одинаковых точках, поэтому будем смещать, пока не окажется в уникальных
				Editorutils::AdjustSidesConnection(ASelectedConnection);
				Editorutils::ProtectConnectionToBeInSamePoints(ASelectedConnection);
			}
			ShapeMoveResizeModified(0, Button, Shift);
		}break;
	case mbRight: {
			// раньше определялось по 'CancelMouse', но были замечены ошибки в работе, поэтому ориентируюсь по Panning
			const bool bCancel = TheTree->View3DOptions->HorizOffset != FHorizOffsetFix ||
				TheTree->View3DOptions->VertOffset != FVertOffsetFix;

			if (!bCancel && !TheTree->Connecting && TheTree->Designing) {
				if (Shift == TShiftState()) {
					const TPoint APtScreen = TheTree->ClientToScreen(TPoint(X, Y));
					TryPopup(TheTree, X, Y, APtScreen.x, APtScreen.y);
				}
				else if (Shift == (TShiftState() << ssCtrl)) {
					// отпускаем событие MouseUp
					FTreeInitConnectionFlag = true;
				}

			}
		}break;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnConnectionReconnected(TTreeConnection *AConnection) {

}

// ---------------------------------------------------------------------------
void MoveShapeWithMovingLayout(Teetree::TTreeNodeShape* Sender, const int DeltaX, const int DeltaY,
	std::set<TTreeConnection*> &AConnections) {
	if (Sender && Sender->Parent) {

		/* This callback is called for every shape in the selection, so we should handle only the first one */
		int iSelIndex = -1;
		int iShapeSelIndex = -1;

		for (int i = 0; i < Sender->Parent->Children->Count; i++) {
			TTreeNodeShape *AChildShape = Sender->Parent->Children->Items[i];

			if (AChildShape->Selected) {
				iSelIndex++;
			}

			if (AChildShape != Sender) {
				if (DeltaY > 0) {
					if (AChildShape->Y0 > Sender->Y1 - DeltaY) {
						AChildShape->MoveRelative(0, DeltaY, true);
					}
				}

				if (DeltaX > 0) {
					if (AChildShape->X0 > Sender->X1 - DeltaX) {
						AChildShape->MoveRelative(DeltaX, 0, true);
					}
				}

				if (DeltaY < 0) {
					if (AChildShape->Y1 < Sender->Y0 - DeltaY) {
						AChildShape->MoveRelative(0, DeltaY, true);
					}
				}

				if (DeltaX < 0) {
					if (AChildShape->X1 > Sender->X0 - DeltaX) {
						AChildShape->MoveRelative(DeltaX, 0, true);
					}
				}

				for (int k = 0; k < AChildShape->Connections->Count; k++) {
					AConnections.insert(AChildShape->Connections->Items[k]);
				}
			}
			else {
				iShapeSelIndex = iSelIndex;
			}
		}

		if (iShapeSelIndex == 0) {
			if (DeltaX < 0) {
				const int iNewX = Sender->Parent->X0 + DeltaX;
				if (iNewX > 0) {
					Sender->Parent->X0 = iNewX;
				}
			}
			if (DeltaX > 0) {
				Sender->Parent->X1 = Sender->Parent->X1 + DeltaX;
			}
			if (DeltaY < 0) {
				const int iNewY = Sender->Parent->Y0 + DeltaY;
				if (iNewY > 0) {
					Sender->Parent->Y0 = iNewY;
				}
			}
			if (DeltaY > 0) {
				Sender->Parent->Y1 = Sender->Parent->Y1 + DeltaY;
			}

			for (int k = 0; k < Sender->Parent->Connections->Count; k++) {
				AConnections.insert(Sender->Parent->Connections->Items[k]);
			}

			MoveShapeWithMovingLayout(Sender->Parent, DeltaX, DeltaY, AConnections);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeMovingShape(Teetree::TTreeNodeShape* Sender, int &DeltaX, int &DeltaY) {
	// В РОДИТЕЛЬСКОМ КЛАССЕ в этом методе вызывается TeeModified, что тормозит программу
	// TTreeEditorNew::TheTreeMovingShape(Sender, DeltaX, DeltaY);

	FMovingFlag = true;

	if (IsShiftPressed() && !IsAltPressed() && !IsControlPressed()) {
		std::set<TTreeConnection*>AConnections;
		MoveShapeWithMovingLayout(Sender, DeltaX, DeltaY, AConnections);
		for (std::set<TTreeConnection*>::iterator it = AConnections.begin(); it != AConnections.end(); ++it) {
			Editorutils::AdjustSidesConnection(*it);
		}
	}
}

// ---------------------------------------------------------------------------
void ResizeShapeWithMovingLayout(Teetree::TTreeNodeShape* Sender, const int DeltaX, const int DeltaY,
	std::set<TTreeConnection*> &AConnections) {
	if (Sender && Sender->Parent) {

		/* This callback is called for every shape in the selection, so we should handle only the first one */
		int iSelIndex = -1;
		int iShapeSelIndex = -1;

		for (int i = 0; i < Sender->Parent->Children->Count; i++) {
			TTreeNodeShape *AChildShape = Sender->Parent->Children->Items[i];

			if (AChildShape->Selected) {
				iSelIndex++;
			}

			if (AChildShape != Sender) {
				if (DeltaY != 0) {
					if (AChildShape->Y0 > Sender->Y1 - DeltaY /* край на момент сдвига */ ) {
						AChildShape->MoveRelative(0, DeltaY, true);
					}
				}

				if (DeltaX != 0) {
					if (AChildShape->X0 > Sender->X1 - DeltaX /* край на момент сдвига */ ) {
						AChildShape->MoveRelative(DeltaX, 0, true);
					}
				}

				for (int k = 0; k < AChildShape->Connections->Count; k++) {
					AConnections.insert(AChildShape->Connections->Items[k]);
				}
			}
			else {
				iShapeSelIndex = iSelIndex;
			}
		}

		if (iShapeSelIndex == 0) {
			Sender->Parent->X1 = Sender->Parent->X1 + DeltaX;
			Sender->Parent->Y1 = Sender->Parent->Y1 + DeltaY;

			for (int k = 0; k < Sender->Parent->Connections->Count; k++) {
				AConnections.insert(Sender->Parent->Connections->Items[k]);
			}

			ResizeShapeWithMovingLayout(Sender->Parent, DeltaX, DeltaY, AConnections);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeResizingShape(Teetree::TTreeNodeShape* Sender, Teetree::TTreeShapeHandle ACorner,
	int &DeltaX, int &DeltaY) {
	// В РОДИТЕЛЬСКОМ КЛАССЕ в этом методе вызывается TeeModified, что тормозит программу
	// TTreeEditorNew::TheTreeResizingShape(Sender, ACorner, DeltaX, DeltaY);

	Autosize1->Checked = false;

	FResizingFlag = true;

	if (IsShiftPressed() && !IsAltPressed()) {
		std::set<TTreeConnection*>AConnections;
		ResizeShapeWithMovingLayout(Sender, DeltaX, DeltaY, AConnections);
		for (std::set<TTreeConnection*>::iterator it = AConnections.begin(); it != AConnections.end(); ++it) {
			Editorutils::AdjustSidesConnection(*it);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::AdjustConnectionsOfSelectedShapes() {
	for (int k = 0; k < TheTree->Connections->Count; k++) {

		for (int i = 0; i < TheTree->Selected->Count(); i++) {

			TTreeNodeShape *AShape = TheTree->Selected->Items[i];

			if (TheTree->Connections->Items[k]->FromShape == AShape || TheTree->Connections->Items[k]
				->ToShape == AShape) {
				Editorutils::AdjustSidesConnection(TheTree->Connections->Items[k]);
				Editorutils::ProtectConnectionToBeInSamePoints(TheTree->Connections->Items[k]);
				break;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnShapeMoveResized() {
	if (TheTree->Selected->Count()) {
		const UnicodeString sMsg = UnicodeString(FMovingFlag ? L"[Moved]" : L"") + UnicodeString
			(FResizingFlag ? L"[Resized]" : L"");
		const UnicodeString sShapeText = TheTree->Selected->First()->SimpleText;

		AdjustConnectionsOfSelectedShapes();

		TeeModified(true, 1, "Shape [" + sShapeText + "] " + sMsg);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ShapeMoveResizeModified(const System::Word AKey, const int AMouseButton,
	const Classes::TShiftState AShift) {
	if (FMovingFlag || FResizingFlag) {
		OnShapeMoveResized();

		FMovingFlag = false;
		FResizingFlag = false;
	}
	else {
		// bugfix: не всегда правильно отрисовываются хэндлы выделения
		// тестируем, что обновление по клику мыши только
		// !!!
		// не убирать, так как проявляется редко и может показаться, что не проявляется
		// четко проявляется в след. ситуации - выделить несколько фигур и вызвать popup-menu,
		// после скрытия фигур, нарушается правильность выделения
		if (AKey == 0) {
			FTreeInvalidateFlag = true;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::MenuHideWindow(const TTreeEditWindows HideWindows, const TTreeEditWindow AOption,
	TMenuItem * AMenu, TNotifyEvent AProc) {
	if (HideWindows.Contains(AOption)) {
		if (AMenu->Checked) {
			AProc(this);
		}
	}
	else {
		if (!AMenu->Checked) {
			AProc(this);
		}
	}
}

// ---------------------------------------------------------------------------
TTreeEditWindows __fastcall TTreeEditorEx::GetHideWindowsStatus() {
	TTreeEditWindows HideWindows = TTreeEditWindows() << Treeednew::teInspector;
	if (!Fonttoolbar1->Checked) {
		HideWindows = HideWindows << Treeednew::teFont;
	}
	if (!Bordertoolbar1->Checked) {
		HideWindows = HideWindows << Treeednew::teFormat;
	}
	if (!Rulers1->Checked) {
		HideWindows = HideWindows << Treeednew::teRulers;
	}
	if (!Shapetabs1->Checked) {
		HideWindows = HideWindows << Treeednew::teToolShapes;
	}
	if (!Toolbar1->Checked) {
		HideWindows = HideWindows << Treeednew::teToolbar;
	}
	if (!NodeTheTree->Checked) {
		HideWindows = HideWindows << Treeednew::teNodeTree;
	}
	if (!Statusbar2->Checked) {
		HideWindows = HideWindows << Treeednew::teStatus;
	}
	if (!Editortabs1->Checked) {
		HideWindows = HideWindows << Treeednew::teEditors;
	}
	if (!MenuSideToolbar->Checked) {
		HideWindows = HideWindows << Treeednew::teSideToolbar;
	}
	if (Menu == NULL) {
		HideWindows = HideWindows << Treeednew::teMainMenu;
	}
	return HideWindows;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TreeHideEditorPanels(const TTreeEditWindows HideWindows) {
	TabControl1->Visible = !HideWindows.Contains(teModeTabs);

	const bool bWasPersistent = this->PersistOptions;

	try {

		this->PersistOptions = false;

		MenuHideWindow(HideWindows, Treeednew::teFont, Fonttoolbar1, Fonttoolbar1Click);
		MenuHideWindow(HideWindows, Treeednew::teFormat, Bordertoolbar1, Bordertoolbar1Click);
		MenuHideWindow(HideWindows, Treeednew::teRulers, Rulers1, Rulers1Click);
		MenuHideWindow(HideWindows, Treeednew::teToolShapes, Shapetabs1, Shapetabs1Click);
		MenuHideWindow(HideWindows, Treeednew::teToolbar, Toolbar1, Toolbar1Click);
		MenuHideWindow(HideWindows, Treeednew::teNodeTree, NodeTheTree, NodeTheTreeClick);
		MenuHideWindow(HideWindows, Treeednew::teStatus, Statusbar2, Statusbar2Click);
		MenuHideWindow(HideWindows, Treeednew::teEditors, Editortabs1, Editortabs1Click);
		MenuHideWindow(HideWindows, Treeednew::teSideToolbar, MenuSideToolbar, MenuSideToolbarClick);

		if (HideWindows.Contains(Treeednew::teMainMenu))
			Menu = NULL;
	}
	__finally {
		this->PersistOptions = bWasPersistent;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::Curve1Click(System::TObject * Sender) {
	TTreeEditorNew::Curve1Click(Sender);
}

// ---------------------------------------------------------------------------
class AdjustedConnectionLock {
	TStateMachineConnection *FStateMachineConnection;
	bool FAdjusted;

public:
	AdjustedConnectionLock(TTreeConnection *AConnection) : FStateMachineConnection
		(dynamic_cast<TStateMachineConnection *>(AConnection)) {
		FAdjusted = false;
		if (FStateMachineConnection) {
			FAdjusted = FStateMachineConnection->AdjustableSides;
			FStateMachineConnection->AdjustableSides = false;
		}
	}

	~AdjustedConnectionLock() {
		if (FStateMachineConnection) {
			FStateMachineConnection->AdjustableSides = FAdjusted;
		}
	}
};

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnMenuChangeConnectionType(System::TObject * Sender) {
	TComponent * AComponent = dynamic_cast<TComponent*>(Sender);
	if (!AComponent)
		return;

	TTreeConnectionStyle AStyle = TTreeConnectionStyle(AComponent->Tag);

	// подстраховываюсь, так как при перерисовках может сняться выделение
	TTreeConnection *ASelectedConnection = TheTree->Connections->Selected;
	if (ASelectedConnection) {

		using namespace Editorutils;

		TTreeConnectionStyle APreviousStyle = ASelectedConnection->Style;

		bool bSpecialApplyStyle = false;

		if ((APreviousStyle == csSides || APreviousStyle == csLine) && AStyle == csInvertedSides) {
			const int iPointsCount = ASelectedConnection->Points->Count();
			if (iPointsCount == 2 && ASelectedConnection->FromShape && ASelectedConnection->ToShape) {
				const TIOConnectionType AFromIOType = GetIOConnectionType2(ASelectedConnection, cdtFrom);
				const TIOConnectionType AToIOType = GetIOConnectionType2(ASelectedConnection, cdtTo);

				switch(AFromIOType) {
				case ccftLeft:
				case ccftRight: {

						const int iPercentFromHeight15 = (double)ASelectedConnection->FromShape->Height * 0.1f;

						const bool bTopIsPossible = ASelectedConnection->ToShape->Y0 -
							ASelectedConnection->FromShape->Y0 > iPercentFromHeight15;
						const bool bBottomIsPossible = ASelectedConnection->FromShape->Y1 -
							ASelectedConnection->ToShape->Y1 > iPercentFromHeight15;

						const bool bTopSideCloser = ASelectedConnection->Points->Item[iPointsCount - 1]
							.Y - ASelectedConnection->ToShape->Y0 <= ASelectedConnection->ToShape->Y1 -
							ASelectedConnection->Points->Item[iPointsCount - 1].Y;

						const int iPercentX = AToIOType == ccftLeft ? 100 : 0;
						const int iTopPercentY = ASelectedConnection->FromShape->Y1 <
							ASelectedConnection->ToShape->Y0 ? 50 : 10;
						const int iBottomPercentY = ASelectedConnection->ToShape->Y1 <
							ASelectedConnection->FromShape->Y0 ? 50 : 90;

						// ближе к верхней стороне
						if (bTopSideCloser) {
							if (bTopIsPossible) {

								ASelectedConnection->Points->ChangeXStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].XValue = iPercentX;
								ASelectedConnection->Points->ChangeYStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].YValue = iTopPercentY;

								SetConnectionXYPointByIOType(ASelectedConnection, ccftTop, cdtTo, iPointsCount - 1);

								bSpecialApplyStyle = true;
							}
							else if (bBottomIsPossible) {

								ASelectedConnection->Points->ChangeXStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].XValue = iPercentX;
								ASelectedConnection->Points->ChangeYStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].YValue = iBottomPercentY;

								SetConnectionXYPointByIOType(ASelectedConnection, ccftBottom, cdtTo, iPointsCount - 1);

								bSpecialApplyStyle = true;
							}
						}
						// ближе к правой стороне
						else {
							if (bBottomIsPossible) {

								ASelectedConnection->Points->ChangeXStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].XValue = iPercentX;
								ASelectedConnection->Points->ChangeYStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].YValue = iBottomPercentY;

								SetConnectionXYPointByIOType(ASelectedConnection, ccftBottom, cdtTo, iPointsCount - 1);

								bSpecialApplyStyle = true;
							}
							else if (bTopIsPossible) {

								ASelectedConnection->Points->ChangeXStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].XValue = iPercentX;
								ASelectedConnection->Points->ChangeYStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].YValue = iTopPercentY;

								SetConnectionXYPointByIOType(ASelectedConnection, ccftTop, cdtTo, iPointsCount - 1);

								bSpecialApplyStyle = true;
							}
						}
					}

					break;
				}
			}

		}
		else if ((APreviousStyle == csInvertedSides || APreviousStyle == csLine) && AStyle == csSides) {
			const int iPointsCount = ASelectedConnection->Points->Count();
			if (iPointsCount == 2 && ASelectedConnection->FromShape && ASelectedConnection->ToShape) {
				const TIOConnectionType AFromIOType = GetIOConnectionType2(ASelectedConnection, cdtFrom);
				const TIOConnectionType AToIOType = GetIOConnectionType2(ASelectedConnection, cdtTo);

				switch(AFromIOType) {
				case ccftTop:
				case ccftBottom: {

						const int iPercentFromWidth15 = (double)ASelectedConnection->FromShape->Width * 0.1f;

						const bool bLeftIsPossible = ASelectedConnection->ToShape->X0 -
							ASelectedConnection->FromShape->X0 > iPercentFromWidth15;
						const bool bRightIsPossible = ASelectedConnection->FromShape->X1 -
							ASelectedConnection->ToShape->X1 > iPercentFromWidth15;

						const bool bLeftSideCloser = ASelectedConnection->Points->Item[iPointsCount - 1]
							.X - ASelectedConnection->ToShape->X0 <= ASelectedConnection->ToShape->X1 -
							ASelectedConnection->Points->Item[iPointsCount - 1].X;

						const int iPercentY = AToIOType == ccftTop ? 100 : 0;
						const int iLeftPercentX = ASelectedConnection->FromShape->X1 <
							ASelectedConnection->ToShape->X0 ? 50 : 10;
						const int iRightPercentX = ASelectedConnection->ToShape->X1 <
							ASelectedConnection->FromShape->X0 ? 50 : 90;

						// ближе к левой стороне
						if (bLeftSideCloser) {
							if (bLeftIsPossible) {

								ASelectedConnection->Points->ChangeXStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].XValue = iLeftPercentX;
								ASelectedConnection->Points->ChangeYStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].YValue = iPercentY;

								SetConnectionXYPointByIOType(ASelectedConnection, ccftLeft, cdtTo, iPointsCount - 1);

								bSpecialApplyStyle = true;
							}
							else if (bRightIsPossible) {

								ASelectedConnection->Points->ChangeXStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].XValue = iRightPercentX;
								ASelectedConnection->Points->ChangeYStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].YValue = iPercentY;

								SetConnectionXYPointByIOType(ASelectedConnection, ccftRight, cdtTo, iPointsCount - 1);

								bSpecialApplyStyle = true;
							}
						}
						// ближе к правой стороне
						else {
							if (bRightIsPossible) {

								ASelectedConnection->Points->ChangeXStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].XValue = iRightPercentX;
								ASelectedConnection->Points->ChangeYStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].YValue = iPercentY;

								SetConnectionXYPointByIOType(ASelectedConnection, ccftRight, cdtTo, iPointsCount - 1);

								bSpecialApplyStyle = true;
							}
							else if (bLeftIsPossible) {

								ASelectedConnection->Points->ChangeXStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].XValue = iLeftPercentX;
								ASelectedConnection->Points->ChangeYStyle(0, cpsFromPercent);
								ASelectedConnection->Points->Item[0].YValue = iPercentY;

								SetConnectionXYPointByIOType(ASelectedConnection, ccftLeft, cdtTo, iPointsCount - 1);

								bSpecialApplyStyle = true;
							}
						}
					}

					break;
				}
			}
		}

		// 10.12.2019 Пробуем специальную подстройку, чтобы можно было назначить csSides, csInvertedSides, если они неправильные
		if (bSpecialApplyStyle) {
			AdjustedConnectionLock ALock(ASelectedConnection);
			Editorutils::CalculatePointsPosition(ASelectedConnection);
			TConnectionPointArray AConnSelectedPoints;
			Editorutils::RememberPoints(ASelectedConnection, AConnSelectedPoints);
			ASelectedConnection->Style = AStyle;
			Editorutils::RestorePoints(ASelectedConnection, AConnSelectedPoints);
		}
		else {

			ASelectedConnection->Style = AStyle;

			const int iPointsCount = ASelectedConnection->Points->Count();

			if (iPointsCount >= 2) {

				const TIOConnectionType AFromIOType = GetIOConnectionType2(ASelectedConnection, cdtFrom);
				const TIOConnectionType AToIOType = GetIOConnectionType2(ASelectedConnection, cdtTo);

				// устанавливаем в средину, а потом смещаем

				switch(ASelectedConnection->Style) {
				case csInvertedSides:
				case csSides:
					SetConnectionXYPointByIOType(ASelectedConnection, AFromIOType, cdtFrom, 0);
					SetConnectionXYPointByIOType(ASelectedConnection, AToIOType, cdtTo,
						ASelectedConnection->Points->Count() - 1);
					break;
				}
			}
		}

		// вызываем для потомков
		OnConnectionStyleChanged(ASelectedConnection);

		AdjustSidesConnection(ASelectedConnection);

		// соединение может оказаться в одинаковых точках, поэтому будем смещать, пока не окажется в уникальных
		Editorutils::ProtectConnectionToBeInSamePoints(ASelectedConnection);

		AdjustSidesStyle(ASelectedConnection);

		TeeModified(True, 1, "Change connection [" + ASelectedConnection->Name + "] style");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnConnectionStyleChanged(TTreeConnection *AConnection) {

}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoOnWheelZoom(TObject *Sender) {
	this->TreeZoomWasChanged();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TreeZoomWasChanged(void) {

	TTreeEditorNew::TreeZoomWasChanged();

	if (TheTree->Grid->Visible) {
		if (TheTree->View3DOptions->Zoom < 100) {
			TheTree->Grid->Step = UpDownGridStep->Position * 100 / TheTree->View3DOptions->Zoom;
		}
		else {
			TheTree->Grid->Step = UpDownGridStep->Position;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnMenuClone(TObject *Sender) {
	try {
		if (TheTree->Selected->Count()) {
			const UnicodeString sFirstCaption = TheTree->Selected->First()->SimpleText;

			std::vector<TTreeNodeShape*>VecNewShapes;
			CloneSelectedShapes(TheTree, VecNewShapes);

			TheTree->Selected->Clear();

			FillNodeTree();

			for (std::size_t i = 0; i < VecNewShapes.size(); i++) {
				VecNewShapes[i]->Selected = true;
			}

			TeeModified(True, 1, "Clone selected: " + sFirstCaption);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoSetBookmark(TSpeedButton *ABookmarkBtn, TScxmlBaseShape *ASetShape) {
	TTreeEx * ATreeEx = dynamic_cast<TTreeEx*>(TheTree);
	if (ABookmarkBtn && ATreeEx) {
		if (ASetShape) {
			ABookmarkBtn->Tag = reinterpret_cast<int>(ASetShape->Guid);
			ABookmarkBtn->Font->Style = ABookmarkBtn->Font->Style << fsBold;
			ABookmarkBtn->Hint = //
			"Shape [" + ASetShape->SimpleText + "]\n" //
			"Press 'Ctrl+1' on chart to center in view";
			ATreeEx->Bookmarks->Values[ABookmarkBtn->Name] = ASetShape->Name;
		}
		else {
			ABookmarkBtn->Tag = 0;
			ABookmarkBtn->Font->Style = ABookmarkBtn->Font->Style >> fsBold;
			ABookmarkBtn->Hint = GetBookmarkHelp(ABookmarkBtn);
			ATreeEx->Bookmarks->Values[ABookmarkBtn->Name] = L"";
		}
		// для того, чтобы появились надписи
		ATreeEx->Invalidate();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::SetBookmark(TSpeedButton *ABookmarkBtn, TScxmlBaseShape *ASetShape) {
	TTreeEx * ATreeEx = dynamic_cast<TTreeEx*>(TheTree);
	if (ABookmarkBtn && ATreeEx) {
		if (ASetShape) {
			// если на какой-то кнопке такое назначено, тогда сбросим
			for (std::size_t i = 0; i < FBookmarkButtons.size(); i++) {
				if (FBookmarkButtons[i] != ABookmarkBtn && FBookmarkButtons[i]->Tag == reinterpret_cast<int>
					(ASetShape->Guid)) {
					ResetBookmark(FBookmarkButtons[i]);
				}
			}

			DoSetBookmark(ABookmarkBtn, ASetShape);

		}
		else {
			DoSetBookmark(ABookmarkBtn, 0);
		}
		TeeModified(true, 2, "Set bookmark " + UnicodeString(ABookmarkBtn->HelpContext));
	}
}

// ---------------------------------------------------------------------------
const UnicodeString TTreeEditorEx::GetBookmarkHelp(TSpeedButton *ABookmarkButton, const bool bMultiline /* = true */ ) {
	UnicodeString sHelp = L"Click 'Ctrl' + 'Left Mouse Btn'";
	sHelp = sHelp + (bMultiline ? L"\n" : L" ");
	sHelp = sHelp + "or press 'Shift+Ctrl+" + UnicodeString(ABookmarkButton->HelpContext) +
		"' on selected shape to SET/RESET a bookmark";
	return sHelp;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::SelectShapeFromBookmark(TSpeedButton * ABookmarkBtn) {
	if (ABookmarkBtn) {
		if (ABookmarkBtn->Tag) {
			const int iShapeIndex = Editorutils::IndexOfShapeByGuid(TheTree,
				reinterpret_cast<GUID*>(ABookmarkBtn->Tag));
			// проверяем, существует ли до сих пор фигура
			if (iShapeIndex != -1) {
				TTreeNodeShape *AShape = TheTree->Shapes->Items[iShapeIndex];
				Editorutils::ExpandAllParents(AShape);
				SelectShape(AShape, SCROLL_CHART_IN_VIEW);
				const TRect ARect = Editorutils::IncrementRectCopy(AShape->Bounds(), 15);
				SelectTemporaryRectangle(ARect);
			}
			else {
				ResetBookmark(ABookmarkBtn);
			}
		}
		else {
			LOG_ERROR_SS << "No bookmark is set!";
			WLOG_ERROR(L"%s", GetBookmarkHelp(ABookmarkBtn, false).c_str());
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnBookmarkBtnClick(TObject * Sender) {
	TSpeedButton * ASpeedButton = dynamic_cast<TSpeedButton*>(Sender);
	if (ASpeedButton) {
		const bool bIsCtrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
		if (bIsCtrlPressed) {
			if (TheTree->Selected->Count()) {
				TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(TheTree->Selected->First());
				if (AScxmlBaseShape) {
					SetBookmark(ASpeedButton, ASpeedButton->Tag == reinterpret_cast<int>(AScxmlBaseShape->Guid)
						? NULL : AScxmlBaseShape);
					TheTree->Invalidate();
				}
				else {
					LOG_ERROR_SS << "Only Scxml shapes are available!";
				}
			}
			else {
				LOG_ERROR_SS << "Shape must be selected before!";
			}
		}
		else {
			SelectShapeFromBookmark(ASpeedButton);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::SelectConnection(TTreeConnection * AConnection,
	const bool bScrollInView /* = SCROLL_CHART_IN_VIEW */ ) {
	TheTree->Selected->Clear();
	TheTree->SelectConnection(AConnection);

	if (bScrollInView && AConnection->FromShape) {
		Editorutils::SetChartPointInView(TheTree, CenterPoint(AConnection->GetBounds()), Editorutils::cpvtCenter);
	}

	TheTreeSelectConnection(AConnection);
}

// ---------------------------------------------------------------------------
bool __fastcall TTreeEditorEx::EnsureInView(TTreeNodeShape *AShape, const TRect &ABounds) {

	return EnsureInView(TPoint(AShape->X0, AShape->Y0), ABounds);
}

// ---------------------------------------------------------------------------
bool __fastcall TTreeEditorEx::EnsureInView(const TPoint &APoint, const TRect &ABounds) {

	TPoint P = TheTree->Canvas->Calculate3DPosition(APoint.x, APoint.y, TeeTreeZ);

	int iTmpX = TheTree->ChartXCenter;
	int iTmpY = TheTree->ChartYCenter;

	P.x = P.x - iTmpX;
	P.y = P.y - iTmpY;

	const int iVisX = APoint.x + 15;
	const int iVisY = APoint.y + 15;

	bool bRes = true;
	if (iVisX < ABounds.Left || iVisX > ABounds.Right) {
		TheTree->View3DOptions->HorizOffset = TheTree->View3DOptions->HorizOffset - P.x;
		bRes = false;
	}

	if (iVisY < ABounds.Top || iVisY > ABounds.Bottom) {
		TheTree->View3DOptions->VertOffset = TheTree->View3DOptions->VertOffset - P.y;
		bRes = false;
	}

	return bRes;
}

// ---------------------------------------------------------------------------
bool __fastcall TTreeEditorEx::EnsureInView(TTreeNodeShape *AShape) {
	int A, B;
	TTreeEx *ATree = dynamic_cast<TTreeEx*>(TheTree);
	if (ATree) {
		const TRect iBounds2D = dynamic_cast<TTreeEx*>(TheTree)->PrepareGrid(A, B);
		return EnsureInView(AShape, iBounds2D);
	}
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::CenterInView(TTreeNodeShape *AShape) {
	// если дочерний узел свернут, то у него нет координат на момент выделения
	if (Editorutils::ExpandAllParents(AShape)) {
		// если было разворачивание, тогда необходимо перерисовать, чтобы получить координаты
		TheTree->Refresh();
	}

	Editorutils::SetChartPointInView(TheTree, AShape->X0, AShape->Y0, Editorutils::cpvtCenter);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::SelectShape(TTreeNodeShape * AShape,
	const bool bScrollInView /* = SCROLL_CHART_IN_VIEW */ ) {
	TheTree->Selected->Clear();
	AShape->Selected = true;

	if (bScrollInView) {
		CenterInView(AShape);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoTheTreeSelectShape(Teetree::TTreeNodeShape * Sender) {
	TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(Sender);
	if (AScxmlBaseShape && !m_LockAddSelectionUndo) {
		AddSelectionUndo(AScxmlBaseShape->SimpleText, AScxmlBaseShape->Guid);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::DoTheTreeSelectConnection(Teetree::TTreeConnection* Sender) {
	TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(Sender);
	if (AStateMachineConnection) {
		if (!m_LockAddSelectionUndo) {
			AddSelectionUndo(AStateMachineConnection->SimpleText, AStateMachineConnection->Guid);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeSelectShape(Teetree::TTreeNodeShape * Sender) {
	TTreeEditorNew::TheTreeSelectShape(Sender);

	DoTheTreeSelectShape(Sender);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeSelectConnection(Teetree::TTreeConnection* Sender) {
	TTreeEditorNew::TheTreeSelectConnection(Sender);

	DoTheTreeSelectConnection(Sender);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ZoomToFit(const Types::TRect &ABounds, TZoomFitType AZoomFitType, bool bFromZero) {
	if (ABounds.Width() > 0 && ABounds.Height() > 0) {

		const int iHorzSize = GetSystemMetrics(SM_CYHSCROLL) + 6;
		const int iVertSize = GetSystemMetrics(SM_CXVSCROLL) + 6;

		const int iTotalBoundsWidth = bFromZero == false ? ABounds.Width() : ABounds.Right;
		const int iTotalBoundsHeight = bFromZero == false ? ABounds.Height() : ABounds.Bottom;

		const int iWidth = TheTree->Width < iTotalBoundsWidth ? TheTree->HorzScrollBar->PageSize :
			(TheTree->Width - iHorzSize);
		const int iHeight = TheTree->Height < iTotalBoundsHeight ? TheTree->VertScrollBar->PageSize :
			(TheTree->Height - iHorzSize);

		if (AZoomFitType == zftAll) {
			const double dTreeRatio = (double)(TheTree->Width) / (double)(TheTree->Height);
			const double dTotalRatio = (double)(iTotalBoundsWidth) / (double)(iTotalBoundsHeight);

			if (TheTree->Width < TheTree->Height) {
				if (iTotalBoundsWidth < iTotalBoundsHeight) {
					AZoomFitType = dTreeRatio < dTotalRatio ? zftWidth : zftHeight;
				}
				else {
					AZoomFitType = dTreeRatio < dTotalRatio ? zftWidth : zftHeight;
				}
			}
			else {
				if (iTotalBoundsWidth < iTotalBoundsHeight) {
					AZoomFitType = dTreeRatio < dTotalRatio ? zftWidth : zftHeight;
				}
				else {
					AZoomFitType = dTreeRatio < dTotalRatio ? zftWidth : zftHeight;
				}
			}
		}

		double dZoomRatio = 1.0f;
		switch(AZoomFitType) {
		case zftHeight: {
				dZoomRatio = (double)(iTotalBoundsHeight) / (double)(iHeight);
				double dZoom = 100.0;
				if (dZoomRatio > 1.0) // увеличение запрещено
					dZoom = 100.0 / dZoomRatio;
				else
					dZoomRatio = 1.0;

				TheTree->View3DOptions->ZoomFloat = dZoom;
			}break;
		case zftWidth: {
				dZoomRatio = (double)(iTotalBoundsWidth) / (double)(iWidth);
				double dZoom = 100.0f;
				if (dZoomRatio > 1.0) // увеличение запрещено
					dZoom = 100.0 / dZoomRatio;
				else
					dZoomRatio = 1.0;

				TheTree->View3DOptions->ZoomFloat = dZoom;
			}break;
		}

		// нет механизмов, чтобы не делать Refresh, так как там рассчитываются координаты ChartBounds
		TheTree->Refresh();

		using namespace Editorutils;

		if (bFromZero) {
			SetChartPointInView(TheTree, 0, 0, cpvtZeroXY);
		}
		else {
			SetChartPointInView(TheTree, CenterPoint(ABounds), cpvtCenter);
		}

		TreeZoomWasChanged();

	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ZoomToFitSelection(TZoomFitType AZoomFitType, bool bFromZero) {

	Types::TRect ASelectedBounds(0, 0, 0, 0);

	if (TheTree->Selected->Count() > 0) {
		ASelectedBounds = TheTree->Selected->First()->Bounds();
	}
	else if (TheTree->Connections->Selected) {
		ASelectedBounds = TheTree->Connections->Selected->GetBounds();
	}

	if (!IsRectEmpty(ASelectedBounds)) {
		// сделаем прямоугольник больше, чтобы четко было видно, какая фигура выделена
		Editorutils::IncrementRect(ASelectedBounds, 15);

		ZoomToFit(ASelectedBounds, AZoomFitType, False);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::SelectTemporaryRectangle(const TRect &ABounds, const UnicodeString sMsg /* = L"" */ ) {
	FTemporarySelectedRectangle.Bounds = ABounds;
	FTemporarySelectedRectangle.Enabled = true;
	FTemporarySelectedRectangle.Text = sMsg;
	TheTree->Invalidate();
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::NodeTreeAfterDraw(System::TObject* Sender) {
	TTreeEx::DrawChartFocus(NodeTree);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::TheTreeAfterDraw(System::TObject * Sender) {
	/* inherited */
	TTreeEditorNew::TheTreeAfterDraw(Sender);

	// возможность дополнительно выделить красным прямоугольником какую-то область,
	// чтобы привлечь внимание пользователя
	if (FTemporarySelectedRectangle.Enabled) {
		FTemporarySelectedRectangle.Enabled = false;
		// будет служить флагом, что прямоугольник отрисован,
		// а затем по нажатию клавиши, если флаг установлен, то вызовется Tree->Invalidate и сотрет прямоугольник
		FTemporarySelectedRectangle.Visible = true;

		TheTree->Canvas->Pen->Color = clRed;
		TheTree->Canvas->Pen->Width = 1;
		TheTree->Canvas->Brush->Style = bsClear;

		TheTree->Canvas->RectangleWithZ(FTemporarySelectedRectangle.Bounds, TeeTreeZ);

		if (!FTemporarySelectedRectangle.Text.IsEmpty()) {
			TheTree->Canvas->Brush->Color = TheTree->Color;

			TheTree->Canvas->Font->Color = clRed;
			TheTree->Canvas->Font->Size = 8;
			TheTree->Canvas->Font->Style = TFontStyles();
			TheTree->Canvas->TextAlign = TA_LEFT;
			TheTree->Canvas->TextOut3D(FTemporarySelectedRectangle.Bounds.left,
				FTemporarySelectedRectangle.Bounds.top - 14, TeeTreeZ, FTemporarySelectedRectangle.Text);
		}
	}
	else {
		FTemporarySelectedRectangle.Visible = false;
	}

	TTreeEx * ATreeEx = static_cast<TTreeEx*>(TheTree);
	if (ATreeEx) {
		const TRect ABounds2D = ATreeEx->Bounds2D;

		if (ATreeEx->Selected->Count()) {

			TRect ASelectionBounds = ATreeEx->Selected->First()->Bounds();

			if (ATreeEx->DragAxis == axisX) {
				TheTree->Canvas->Pen->Color = clGreen;
				TheTree->Canvas->Pen->Width = 1;

				const int iLeft = Min(static_cast<int>(ABounds2D.Left), static_cast<int>(ASelectionBounds.Left));
				const int iTop = ASelectionBounds.top;
				const int iBottom = iTop;
				const int iRight = Max(static_cast<int>(ABounds2D.Right), static_cast<int>(ASelectionBounds.Right));

				TheTree->Canvas->LineWithZ(iLeft, iTop, iRight, iBottom, TeeTreeZ);

				TheTree->Canvas->Brush->Color = clWhite;
				TheTree->Canvas->Font->Color = clGreen;
				TheTree->Canvas->Font->Size = 8;
				TheTree->Canvas->Font->Style = TFontStyles();
				TheTree->Canvas->TextAlign = TA_LEFT;
				TheTree->Canvas->TextOut3D(ASelectionBounds.Left, ASelectionBounds.Top - 18, TeeTreeZ,
					L"Press X to unlock axis");
			}
			else if (ATreeEx->DragAxis == axisY) {
				TheTree->Canvas->Pen->Color = clBlue;
				TheTree->Canvas->Pen->Width = 1;

				const int iLeft = ASelectionBounds.Left;
				const int iTop = Min(static_cast<int>(ABounds2D.Top), static_cast<int>(ASelectionBounds.Top));
				const int iBottom = Max(static_cast<int>(ABounds2D.Bottom), static_cast<int>(ASelectionBounds.Bottom));
				const int iRight = iLeft;

				TheTree->Canvas->LineWithZ(iLeft, iTop, iRight, iBottom, TeeTreeZ);

				TheTree->Canvas->Brush->Color = clWhite;
				TheTree->Canvas->Font->Color = clBlue;
				TheTree->Canvas->Font->Size = 8;
				TheTree->Canvas->Font->Style = TFontStyles();
				TheTree->Canvas->TextAlign = TA_LEFT;
				TheTree->Canvas->TextOut3D(ASelectionBounds.Left, ASelectionBounds.Top - 18, TeeTreeZ,
					L"Press Y to unlock axis");
			}
		}
	}

	TTreeEx::DrawChartFocus(TheTree);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnEditNodeTreeSearchChange(TObject *Sender) {
	if (!EditNodeTreeSearch->Tag) {
		BtnClearNodeTreeFilter->Visible = !EditNodeTreeSearch->Text.IsEmpty();
		FillNodeTree();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnEditNodeTreeKeyPress(System::TObject* Sender, System::WideChar &Key) {
	switch(Key) {
	case VK_RETURN:
		Key = 0;

		LeaveEditAndEnterNodeTree();

		break;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::OnEditNodeTreeKeyDown(System::TObject* Sender, System::Word &Key,
	Classes::TShiftState Shift) {
	switch(Key) {
	case VK_DOWN:
		Key = 0;

		LeaveEditAndEnterNodeTree();

		break;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::LeaveEditAndEnterNodeTree(void) {
	try {
		NodeTree->SetFocus();
		if (!NodeTree->Selected->Count()) {
			if (NodeTree->Shapes->Count) {
				NodeTree->Shapes->Items[0]->Selected = true;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
TCustomTreeElement * __fastcall TTreeEditorEx::NodeTreeShapeToTreeElement(TTreeNodeShape *ANodeTreeShape) {
	if (ANodeTreeShape) {
		TCustomTreeElement * AElement = reinterpret_cast<TCustomTreeElement*>(ANodeTreeShape->Tag);
		if (TheTree->Shapes->IndexOf(AElement) != -1 || TheTree->Connections->IndexOf(AElement) != -1) {
			return AElement;
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ButtonUndoRedoDropDownClick(TObject *Sender) {
	AddMenuUndoItems(PopupMenuBtnUndoRedo->Items);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEditorEx::ButtonSelectionDropDownClick(TObject *Sender) {
	AddMenuSelectionItems(PopupMenuBtnSelect->Items);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
