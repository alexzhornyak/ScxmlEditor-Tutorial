// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

// header
#include "UnitTreeBase.h"

// standard
#include <vector>
#include <map>
#include <algorithm>

#include <boost/bind.hpp>

// vcl
#include <TreeShed.hpp>
#include <TeePrevi.hpp>
#include <TreeExport.hpp>
#include <XMLDoc.hpp>
#include <IOUtils.hpp>
#include <TreeCoEd.hpp>
#include <IOUtils.hpp>

#include <MaZhor/MConvertComponent.hpp>
#include <MaZhor/StringListEx.hpp>
#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"
#include "UnitStateMachineParser.h"
#include "UnitMain.h"

// local

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TeeProcs"
#pragma link "TeeTree"
#pragma link "TeeNavigator"
#pragma link "TreeNavigator"
#pragma link "TeeNavigator"
#pragma link "TeeProcs"
#pragma link "TeeTree"
#pragma link "TreeNavigator"
#pragma link "AdvEdBtn"
#pragma link "AdvEdit"
#pragma link "AdvFileNameEdit"
#pragma link "AdvToolBtn"
#pragma resource "*.dfm"

// ---------------------------------------------------------------------------
TTreeNodeShape * GetShapeFromGuid(TTree *ATree, const UnicodeString &sGuid) {
	if (!sGuid.IsEmpty()) {
		for (int i = 0; i < ATree->Shapes->Count; i++) {
			if (ATree->Shapes->Items[i]->Text->Text.Pos(sGuid)) {
				return ATree->Shapes->Items[i];
			}
		}
	}
	return 0;
}

// ---------------------------------------------------------------------------
UnicodeString GetShapeFirstStr(TTreeNodeShape *AShape) {
	if (!AShape)
		return "";
	if (AShape->Text->Count) {
		return AShape->Text->Strings[0];
	}
	return AShape->Name;
}

// ---------------------------------------------------------------------------
TTreeConnection *GetConnectionFromGuid(TTree *ATree, const UnicodeString &sGuid) {
	if (!sGuid.IsEmpty()) {
		for (int i = 0; i < ATree->Connections->Count; i++) {
			if (ATree->Connections->Items[i]->Text->Text.Pos(sGuid)) {
				return ATree->Connections->Items[i];
			}
		}
	}
	return 0;
}

// ---------------------------------------------------------------------------
void SelectShapeInCenter(TCustomTree *ATree, TTreeNodeShape *ANode) {

	ATree->Selected->Clear();

	ANode->Selected = true;

	ATree->Update();

	TPoint P = ATree->Canvas->Calculate3DPosition(ANode->XCenter(), ANode->YCenter(), TeeTreeZ);

	int iTmpX = ATree->ChartXCenter;
	int iTmpY = ATree->ChartYCenter;

	P.x = P.x - iTmpX;
	P.y = P.y - iTmpY;

	ATree->View3DOptions->HorizOffset = ATree->View3DOptions->HorizOffset - P.x;
	ATree->View3DOptions->VertOffset = ATree->View3DOptions->VertOffset - P.y;

}

// ---------------------------------------------------------------------------
void SelectConnectionInCenter(TCustomTree *ATree, TTreeConnection *AConnection) {
	ATree->SelectConnection(AConnection);

	if (AConnection->FromShape) {
		TPoint P = ATree->Canvas->Calculate3DPosition(AConnection->FromShape->XCenter(), AConnection->FromShape->YCenter(), TeeTreeZ);

		int iTmpX = ATree->ChartXCenter;
		int iTmpY = ATree->ChartYCenter;

		P.x = P.x - iTmpX;
		P.y = P.y - iTmpY;

		ATree->View3DOptions->HorizOffset = ATree->View3DOptions->HorizOffset - P.x;
		ATree->View3DOptions->VertOffset = ATree->View3DOptions->VertOffset - P.y;
	}
}

// ---------------------------------------------------------------------------
class TTreeAccess : public TCustomTree {
public:
	void SetPolygonClass(TPolygonShapeClass APolygonMode) {
		this->IPolygonMode = APolygonMode;
	}

	void SetPolygonShape(TPolygonShape* AShape) {
		this->IPolygonShape = AShape;
	}

	void SetZoomPriority(const bool bPriority) {
		this->IZoomPriority = bPriority;
	}
};

// ---------------------------------------------------------------------------
__fastcall TFrameTreeBase::TFrameTreeBase(TComponent* Owner) : TFrame(Owner), m_sOpenedFileName(""), g_b_resizing_flag(false),
g_b_moving_flag(false) {

	assert(SettingsData != NULL);

	g_i_history_count = 0;

	g_b_allow_history_save = true;
	g_b_modified = false;

	// тип класса, который будет создаваться на плане
	g_ShapeClass = NULL;
	m_undo_it = m_UndoQueue.end();
	m_ClipboardStreamPtr.reset(new TMemoryStream);

	TreeBase->OnZoomedArea = &OnZoomedAreaEvent;

	CheckDesignMode->Checked = true;

	m_NewProjectStreamPtr.reset(new TMemoryStream);
	SaveTreeToStream(TreeBase, m_NewProjectStreamPtr.get());

	TreeBase->Clear();

	TreeBase->Color = clSilver;

	ClearModifications();

	m_sTempClipboardFile = TPath::Combine(TPath::GetTempPathA(), "dspcreator_clipboard.bin");
}

// ---------------------------------------------------------------------------
__fastcall TFrameTreeBase::~TFrameTreeBase() {
	while (!m_UndoQueue.empty()) {
		delete m_UndoQueue.front().second;
		m_UndoQueue.pop_front();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::OnZoomedAreaEvent(const Types::TRect &R) {
	if (g_ShapeClass && TreeBase->Designing) {
		AddShapeToTree(g_ShapeClass, R);
		g_ShapeClass = 0;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ExternModified() {
	g_b_modified = true;

	if (OnTreeBaseModified) {
		OnTreeBaseModified(this);
	}
}

// ---------------------------------------------------------------------------
const bool __fastcall TFrameTreeBase::CheckModified(void) {
	if (g_b_modified) {

		String Caption = "WARNING!";
		String Text = "File <" + m_sOpenedFileName + "> is changed!\n" "Are you sure to quit?";
		if (Application->MessageBoxA(Text.w_str(), Caption.w_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) != IDOK) {
			return false;
		}
	}
	return true;
}

// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::AddShapeToTree(TMetaClass *AClass, const TRect &ARect) {
	TTreeNodeShape* AShape = TreeBase->AddShapeClass(ARect.Left, ARect.Top, "", 0, AClass);

	AShape->ImageIndex = tiNone;

	if (ARect.Left != 0) {
		AShape->X0 = ARect.Left;
	}
	if (ARect.Top != 0) {
		AShape->Y0 = ARect.Top;
	}
	if ((ARect.Right - ARect.Left) > 0 && (ARect.Bottom - ARect.Top) > 0) {
		AShape->X1 = ARect.Right;
		AShape->Y1 = ARect.Bottom;
	}
	TreeBase->Selected->Clear();
	AShape->Shadow->Visible = false;
	AShape->Selected = true;
	AShape->Expanded = true;

	UnicodeString sDescription = "[Element]";
	if (AShape->ClassNameIs("TMixerShape")) {
		AShape->Text->Clear();
		AShape->Text->Values["id"] = StringReplace(AShape->Name, "TreeNodeShape", "DSP ", TReplaceFlags());
		AShape->Text->Values["type"] = "mixer";
		sDescription = "[mixer]";
	}
	else if (AShape->ClassNameIs("TInputChannelShape")) {
		AShape->Text->Clear();
		AShape->Text->Values["id"] = "InputChannel_NNN";
		AShape->Text->Values["type"] = "mixer";
		sDescription = "[mixer]";
	}
	else if (AShape->ClassNameIs("TEventSystemShape")) {
		AShape->Text->Clear();
		AShape->Text->Values["id"] = StringReplace(AShape->Name, "TreeNodeShape", "DSP ", TReplaceFlags());
		AShape->Text->Values["type"] = "EVENT_SYSTEM";
		sDescription = "[EVENT_SYSTEM]";
	}
	else if (AShape->ClassNameIs("TPluginShape")) {
		AShape->Text->Clear();
		AShape->Text->Values["id"] = StringReplace(AShape->Name, "TreeNodeShape", "DSP ", TReplaceFlags());
		AShape->Text->Values["type"] = "PLUGIN";
		sDescription = "[PLUGIN]";
	}
	else if (AShape->ClassNameIs("TNetworkShape")) {
		AShape->Text->Clear();
		AShape->Text->Values["id"] = StringReplace(AShape->Name, "TreeNodeShape", "DSP ", TReplaceFlags());
		AShape->Text->Values["type"] = "NETWORK";
		sDescription = "[NETWORK]";
	}

	Screen->Cursor = crDefault;

	DoModified("Add shape " + sDescription);
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::DoModified(const UnicodeString &sDescription) {
	if (g_b_allow_history_save) {

		if (m_UndoQueue.size() >= i_MAX_HISTORY) {
			TMemoryStream *AStream = m_UndoQueue.back().second;
			delete AStream;
			m_UndoQueue.pop_back();
		}

		m_UndoQueue.push_front(std::make_pair(sDescription, new TMemoryStream));
		m_undo_it = m_UndoQueue.begin();

		SaveTreeToStream(TreeBase, m_UndoQueue.front().second);

	}
	g_b_modified = true;

	if (OnTreeBaseModified) {
		OnTreeBaseModified(this);
	}

}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::SaveTreeProject(const UnicodeString &sFileName) {
	try {

		const UnicodeString sFilePath = ExtractFilePath(sFileName);

		m_sOpenedFileName = sFileName;

		// приводим цвета к тем, которые запланированы в зависимости от типа блока
		ActionUpdateColors->Execute();

		SaveTreeToFile(TreeBase, m_sOpenedFileName);

		// пусть остаётся неочищенной очередь undo, снимем только флаг, о том, что происходили изменения
		// ClearModifications();
		g_b_modified = false;
		if (OnTreeBaseModified) {
			OnTreeBaseModified(this);
		}

		if (OnProjectSaved) {
			OnProjectSaved(this);
		}

		ParseTree(TreeBase, ChangeFileExt(GetFileName(), ".xml"));
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::TreeBaseDblClickShape(TTreeNodeShape *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	if (Button == mbLeft) {
		// EditTreeShape(this, TreeBase->Selected->First());
		ActionEditDSP->Execute();
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::TreeBaseDeletingShapes(TSelectedShapeList *Sender, bool &AllowDelete) {
	// не разрешаем ничего удалять из режима просмотра
	if (!TreeBase->Designing) {
		AllowDelete = false;
		return;
	}
	//	for (int i = 0; i < Sender->Count(); i++) {
	//		 TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape *>(Sender->Items[i]);
	//		 if (AShape) {
	//
	//		 }
	//	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::TreeBaseKeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
	if (Key == 'C' && Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift)) {
		ActionCopySelectedExecute(ActionCopySelected);
	}
	else if (Key == 'V' && Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift)) {
		ActionPasteSelectedExecute(ActionPasteSelected);
	}
	else if (Key == 'F' && Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift)) {
		ActionFindElementExecute(ActionFindElement);
	}
	else if (Key == 'M' && Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift)) {
		ActionZoomInCenter->Execute();
	}
	else if (Key == VK_F3 && !Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift)) {
		ActionZoomToFitAll->Execute();
	}
	else if (Key == VK_F3 && Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift)) {
		ActionZoomToHeight->Execute();
	}
	else if (Key == VK_F3 && !Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && Shift.Contains(ssShift)) {
		ActionZoomToWidth->Execute();
	}
	else if (Key == VK_DELETE && !Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift)) {
		// отсюда нельзя удалять объект
		PostMessage(this->Handle, WM_SAFE_DELETE, 0, 0);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::TreeBaseMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	gMovePt.x = X;
	gMovePt.y = Y;

	DoModified("Before Shape changed");
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::TreeBaseMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {

	bool bIsModifiedHere = false;
	bool bNeedToRunTester = false;

	UnicodeString sModifiedDesc = "";

	if (Button == mbRight && (gMovePt.x == X && gMovePt.y == Y)) {
		TPoint P;
		GetCursorPos(&P);
		PopupMenuTree->Popup(P.x, P.y);
	}
	else if (Button == mbLeft) {
		if (TreeBase->Connections->Selected) {
			const int iCount = TreeBase->Connections->Selected->Points->Count();
			// если не менее двух точек, тогда может отсоединить последнюю или первую
			if (iCount >= 2) {
				TTreeNodeShape *AShape = TreeBase->Shapes->Clicked(X, Y);
				if (AShape) {
					// проверяем, тянем ли последнюю точку
					const int iPointIndex = TreeBase->Connections->Selected->Points->Clicked(X, Y);
					if (iPointIndex == iCount - 1) {

						if (TreeBase->Connections->Selected->ToShape != AShape) {
							TreeBase->Connections->Selected->ToShape = AShape;

							TreeBase->Connections->Selected->Points->Item[TreeBase->Connections->Selected->Points->Count() - 1]
								.XStyle = cpsAutoTo;
							TreeBase->Connections->Selected->Points->Item[TreeBase->Connections->Selected->Points->Count() - 1]
								.YStyle = cpsAutoTo;

							TreeBase->Refresh();

							bIsModifiedHere = true;
							bNeedToRunTester = true;

							sModifiedDesc = "Reconnected " + GetShapeFirstStr(TreeBase->Connections->Selected->FromShape)
								+ " to " + GetShapeFirstStr(TreeBase->Connections->Selected->ToShape);
						}

					}
					else if (iPointIndex == 0) {
						if (TreeBase->Connections->Selected->FromShape != AShape) {
							TreeBase->Connections->Selected->FromShape = AShape;

							TreeBase->Connections->Selected->Points->Item[0].XStyle = cpsAutoFrom;
							TreeBase->Connections->Selected->Points->Item[0].YStyle = cpsAutoFrom;

							TreeBase->Refresh();

							bIsModifiedHere = true;
							bNeedToRunTester = true;

							sModifiedDesc = "Reconnected " + GetShapeFirstStr(TreeBase->Connections->Selected->FromShape)
								+ " to " + GetShapeFirstStr(TreeBase->Connections->Selected->ToShape);
						}
					}
				}
			}
		}
	}
	if (g_b_resizing_flag || g_b_moving_flag) {

		bIsModifiedHere = true;
		bNeedToRunTester = false;

		g_b_resizing_flag = false;
		g_b_moving_flag = false;

		sModifiedDesc = "Shape changed";
	}
	if (bIsModifiedHere) {
		DoModified(sModifiedDesc);
	}
	else {
		if (!m_UndoQueue.empty()) {
			delete m_UndoQueue.front().second;
			m_UndoQueue.pop_front();
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::TreeBaseNewShape(TCustomTree *Sender, TTreeNodeShape *NewShape) {

	if (Sender->Owner) {

		NewShape->Name = TeeGetUniqueName(this, "TreeNodeShape");
		if (NewShape->Text->Text.IsEmpty()) {
			NewShape->Text->Text = NewShape->Name;
		}

		this->InsertComponent(NewShape);

		g_b_modified = true;
	}
	else {
		WLOG_ERROR(L"[%s] owner is empty!", Sender->Name.c_str());
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::TreeBaseSelectShape(TTreeNodeShape *Sender) {
	//
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::TreeBaseUnSelectShape(TTreeNodeShape *Sender) {
	//

}

// ---------------------------------------------------------------------------
void OnNodeCopy(TTreeNodeShape *AShape, _di_IXMLNode AParentNode, TSelectedShapeList *ANodeList, _di_IXMLNode ATransitionsNode) {
	assert(AShape->Owner != NULL);
	if (AShape->Name.IsEmpty()) {
		AShape->Name = TeeGetUniqueName(AShape->Owner, "TreeNodeShape");
	}
	_di_IXMLNode AChildNode = AParentNode->AddChild(AShape->Name);
	_di_IXMLNode AComponentNode = AChildNode->AddChild("ComponentNode");

	AComponentNode->SetText(MUtils::ComponentToString(AShape));

	for (int i = 0; i < AShape->Connections->Count; i++) {
		TTreeConnection *AConnection = AShape->Connections->Items[i];

		TTreeNodeShape *AFromShape = AConnection->FromShape;
		TTreeNodeShape *AToShape = AConnection->ToShape;

		if (AFromShape == AShape) {
			for (int k = 0; k < ANodeList->Count(); k++) {
				if (AToShape == ANodeList->Items[k]) {

					_di_IXMLNode AConnectionNode = ATransitionsNode->AddChild("Connection");
					AConnectionNode->SetAttribute("FromName", AFromShape->Name);
					AConnectionNode->SetAttribute("ToName", AToShape->Name);

					AConnectionNode->SetText(MUtils::ComponentToString(AConnection));

				}
			}
		}
	}

	_di_IXMLNode AChildrenNode = AChildNode->AddChild("Children");
	for (int i = 0; i < AShape->Count(); i++) {
		OnNodeCopy(AShape->Children->Items[i], AChildrenNode, ANodeList, ATransitionsNode);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::SaveSelectedToStream(TStream *AStream) {
	try {
		std::auto_ptr<TXMLDocument>AXMLDoc(new TXMLDocument(this));
		AXMLDoc->Active = true;
		_di_IXMLNode NodeRoot = AXMLDoc->AddChild("Root");

		_di_IXMLNode NodeStates = NodeRoot->AddChild("States");
		_di_IXMLNode NodeConnections = NodeRoot->AddChild("Connections");

		for (int i = 0; i < TreeBase->Selected->Count(); i++) {

			OnNodeCopy(TreeBase->Selected->Items[i], NodeStates, TreeBase->Selected, NodeConnections);

		}
		AXMLDoc->SaveToStream(m_ClipboardStreamPtr.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void OnPasteChildren(TTreeNodeShape *AParentShapeNode, _di_IXMLNode AParentNode, std::map<UnicodeString, TTreeNodeShape*> &AMapNodeMatch) {

	_di_IXMLNode AChildrenNode = AParentNode->ChildNodes->FindNode("Children");
	if (AChildrenNode) {
		for (int k = 0; k < AChildrenNode->ChildNodes->Count; k++) {
			_di_IXMLNode AXMLNode = AChildrenNode->ChildNodes->Nodes[k];
			if (AXMLNode->NodeType == ntElement) {

				_di_IXMLNode AComponentNode = AXMLNode->ChildNodes->FindNode("ComponentNode");

				if (AComponentNode) {

					TComponent *AComponent = MUtils::StringToComponent(0, AComponentNode->Text);

					if (AComponent) {
						TTreeNodeShape *AShapeNode = dynamic_cast<TTreeNodeShape*>(AComponent);

						const UnicodeString sOldName = AShapeNode->Name;

						TTreeNodeShape *AChildShapeNode = AParentShapeNode->AddChild("");
						AChildShapeNode->Text->Assign(AShapeNode->Text);

						if (!sOldName.IsEmpty()) {
							AMapNodeMatch[sOldName] = AChildShapeNode;
						}

						delete AShapeNode;
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::LoadSelectedFromStream(TStream *AStream, const int iOffsetX /* = 0 */ , const int iOffsetY /* = 0 */ ,
	const bool bSelect /* = true */ ) {

	TreeBase->Selected->Clear();

	std::auto_ptr<TXMLDocument>AXMLDoc(new TXMLDocument(this));
	AStream->Position = 0;
	AXMLDoc->LoadFromStream(AStream);

	if (!AXMLDoc->DocumentElement)
		throw Exception("Document element is empty!");

	_di_IXMLNode NodeStates = AXMLDoc->DocumentElement->ChildNodes->FindNode("States");
	if (!NodeStates) {
		throw Exception("Node <States> element is empty!");
	}
	_di_IXMLNode NodeConnections = AXMLDoc->DocumentElement->ChildNodes->FindNode("Connections");
	if (!NodeConnections) {
		throw Exception("Node <Connections> element is empty!");
	}

	std::map<UnicodeString, TTreeNodeShape*>MapNodeMatch;

	for (int i = 0; i < NodeStates->ChildNodes->Count; i++) {
		_di_IXMLNode AXMLNode = NodeStates->ChildNodes->Nodes[i];
		if (AXMLNode->NodeType == ntElement) {

			_di_IXMLNode AComponentNode = AXMLNode->ChildNodes->FindNode("ComponentNode");

			if (AComponentNode) {

				TComponent *AComponent = MUtils::StringToComponent(0, AComponentNode->Text);

				if (AComponent) {
					TTreeNodeShape *AShapeNode = dynamic_cast<TTreeNodeShape*>(AComponent);
					AShapeNode->Tree = TreeBase;

					AShapeNode->MoveRelative(iOffsetX, iOffsetY, true);

					if (!AShapeNode->Name.IsEmpty()) {
						MapNodeMatch[AShapeNode->Name] = AShapeNode;
					}

					// в любом случае даём новое имя, чтобы не повторялось на форме
					AShapeNode->Name = TeeGetUniqueName(this, "TreeNodeShape");

					this->InsertComponent(AShapeNode);

					if (bSelect) {
						TreeBase->Selected->Add(AShapeNode);
					}

					OnPasteChildren(AShapeNode, AXMLNode, MapNodeMatch);
				}
			}
		}
	}

	for (int i = 0; i < NodeConnections->ChildNodes->Count; i++) {
		_di_IXMLNode AConnectionNode = NodeConnections->ChildNodes->Nodes[i];
		const UnicodeString sFromName = AConnectionNode->GetAttribute("FromName");
		const UnicodeString sToName = AConnectionNode->GetAttribute("ToName");

		std::map<UnicodeString, TTreeNodeShape*>::iterator it_from = MapNodeMatch.find(sFromName);
		if (it_from != MapNodeMatch.end()) {
			std::map<UnicodeString, TTreeNodeShape*>::iterator it_where = MapNodeMatch.find(sToName);
			if (it_where != MapNodeMatch.end()) {

				TComponent *AComponent = MUtils::StringToComponent(0, AConnectionNode->Text);
				if (AComponent) {
					TTreeConnection *AConnectionXML = dynamic_cast<TTreeConnection*>(AComponent);

					TTreeConnection* AConnection = it_from->second->AddConnection(it_where->second);
					AConnection->Assign(AConnectionXML);

					delete AConnectionXML;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionCopySelectedExecute(TObject *Sender) {
	try {
		m_ClipboardStreamPtr->Clear();

		this->SaveSelectedToStream(m_ClipboardStreamPtr.get());
		if (SettingsData->ClipboardType == ctGlobal) {
			m_ClipboardStreamPtr->Position = 0;
			m_ClipboardStreamPtr->SaveToFile(m_sTempClipboardFile);
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionPasteSelectedExecute(TObject *Sender) {
	try {
		if (SettingsData->ClipboardType == ctGlobal) {
			m_ClipboardStreamPtr->LoadFromFile(m_sTempClipboardFile);
			m_ClipboardStreamPtr->Position = 0;
		}
		if (m_ClipboardStreamPtr->Size) {
			LoadSelectedFromStream(m_ClipboardStreamPtr.get(), 15, 15);

			DoModified("Paste");
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::DoZoom(const int iValue) {
	TrackBarZoom->Position = TrackBarZoom->Position + iValue;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionZoomInExecute(TObject *Sender) {
	DoZoom(10);
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionZoomOutExecute(TObject *Sender) {
	DoZoom(-10);
}

// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionRedoExecute(TObject *Sender) {
	g_b_allow_history_save = false;

	if (m_UndoQueue.size()) {
		if (m_undo_it != m_UndoQueue.begin()) {
			--m_undo_it;

			UpdateToUndoIterator();
		}
	}

	g_b_allow_history_save = true;
}

// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionUndoExecute(TObject *Sender) {

	g_b_allow_history_save = false;

	if (m_UndoQueue.size()) {
		TUndoQueue::iterator it_end = m_UndoQueue.end();
		--it_end;
		if (m_undo_it != it_end && m_undo_it != m_UndoQueue.end()) {
			++m_undo_it;
		}
		if (m_undo_it != m_UndoQueue.end()) {
			UpdateToUndoIterator();
		}

	}

	g_b_allow_history_save = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::PopupUndoRedoPopup(TObject *Sender) {
	PopupUndoRedo->Items->Clear();

	bool bAfterIterator = false;

	for (std::size_t i = 0; i != m_UndoQueue.size(); i++) {
		TMenuItem *AMenuItem = new TMenuItem(PopupUndoRedo);
		AMenuItem->Caption = m_UndoQueue[i].first;
		AMenuItem->OnClick = &OnUndoRedoClicked;
		AMenuItem->Tag = i;

		PopupUndoRedo->Items->Add(AMenuItem);

		if (m_undo_it == m_UndoQueue.begin() + i) {
			bAfterIterator = true;
		}

		if (bAfterIterator) {
			AMenuItem->Checked = true;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::OnUndoRedoClicked(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {
		m_undo_it = m_UndoQueue.begin() + AMenuItem->Tag;
		UpdateToUndoIterator();
	}
}

// ---------------------------------------------------------------------------
void TFrameTreeBase::UpdateToUndoIterator(void) {
	(*m_undo_it).second->Position = 0;
	if ((*m_undo_it).second->Size) {
		TCustomTree *ATree = TreeBase;
		LoadTreeFromStream(ATree, (*m_undo_it).second);
		TreeBase->Refresh();
		TreeBase->Cursor = crDefault;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionDeleteSelectedExecute(TObject *Sender) {

	DoModified("Before deleting...");

	std::vector<TTreeNodeShape*>VecShape;

	std::auto_ptr<TStringList>ANamesList(new TStringList());
	ANamesList->StrictDelimiter = true;
	ANamesList->Delimiter = L',';

	// возможно есть какие-то нестыковки с выделением, поэтому удаляем либо Shape либо Connection
	bool bShapeDeleting = false;

	for (int i = 0; i < TreeBase->Selected->Count(); i++) {
		VecShape.push_back(TreeBase->Selected->Items[i]);
	}

	for (std::size_t i = 0; i < VecShape.size(); i++) {
		ANamesList->Add(GetShapeFirstStr(VecShape[i]));
		TreeBase->Connections->DeleteAllTo(VecShape[i]);
		delete VecShape[i];

		bShapeDeleting = true;
	}

	if (TreeBase->Connections->Selected) {

		ANamesList->Add("connection from [" + GetShapeFirstStr(TreeBase->Connections->Selected->FromShape) + "] to [" + GetShapeFirstStr
			(TreeBase->Connections->Selected->ToShape) + "]");

		const int iIndex = TreeBase->Connections->Selected->Points->Clicked(gMovePt.x, gMovePt.y);
		if (iIndex != -1) {
			TreeBase->Connections->Selected->Points->Delete(iIndex);
		}
		else {
			TreeBase->Connections->Selected->Free();
		}
	}

	// ограничиваем длину, чтобы в Popup не было очень длинных строк
	if (ANamesList->DelimitedText.Length() > 50) {
		UnicodeString sText = ANamesList->DelimitedText;
		sText.SetLength(50);
		ANamesList->DelimitedText = sText + " ...";
	}

	DoModified("Delete " + ANamesList->DelimitedText);
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionEditSelectedExecute(TObject *Sender) {
	if (TreeBase->Selected->Count()) {
		EditTreeShape(this, TreeBase->Selected->First());
		DoModified("Edit shape");
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionBringToFrontExecute(TObject *Sender) {
	for (int i = 0; i < TreeBase->Selected->Count(); i++) {
		TreeBase->Selected->Items[i]->BringToFront();
	}
	DoModified("Bring to front");
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionSendToBackExecute(TObject *Sender) {
	for (int i = 0; i < TreeBase->Selected->Count(); i++) {
		TreeBase->Selected->Items[i]->SendToBack();
	}
	DoModified("Send to back");
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::PopupMenuTreePopup(TObject *Sender) {
	const bool bIsSelected = TreeBase->Selected->Count() > 0;

	MenuEditConnection->Enabled = TreeBase->Connections->Selected;
	ActionEditLevels->Enabled = TreeBase->Connections->Selected;
	ActionEditDSP->Enabled = bIsSelected;
	ActionPasteSelected->Enabled = m_ClipboardStreamPtr->Size != 0;
	MenuEdit->Enabled = bIsSelected;

	MenuCopy->Enabled = bIsSelected;
	MenuDelete->Enabled = (bIsSelected || TreeBase->Connections->Selected) && TreeBase->Designing;
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::TrackBarZoomChange(TObject *Sender) {
	if (TrackBarZoom->Tag == 0) {
		DoSetZoom(TrackBarZoom->Position);
	}
	LabelZoom->Caption = "Zoom: " + UnicodeString(TrackBarZoom->Position) + " %";
}

// ---------------------------------------------------------------------------
void TFrameTreeBase::DoSetZoom(const int iZoom) {
	TreeBase->View3DOptions->Zoom = iZoom;
	if (TreeBase->View3DOptions->Zoom < i_MIN_ZOOM_FOR_GRID) {
		TreeBase->Grid->Visible = false;
	}
	else {
		if (TreeBase->Designing) {
			TreeBase->Grid->Visible = true;
		}
	}
}

// ---------------------------------------------------------------------------
void TFrameTreeBase::DoSetZoom(const double dZoom) {
	TreeBase->View3DOptions->ZoomFloat = dZoom;
	if (TreeBase->View3DOptions->ZoomFloat < i_MIN_ZOOM_FOR_GRID) {
		TreeBase->Grid->Visible = false;
	}
	else {
		if (TreeBase->Designing) {
			TreeBase->Grid->Visible = true;
		}
	}
}

// ---------------------------------------------------------------------------
// устанавливает величину зума от 10 до 1000
void TFrameTreeBase::SetFixedZoom(const int iZoom) {
	TrackBarZoom->Tag = 1; // block
	TrackBarZoom->Position = iZoom;
	DoSetZoom(iZoom);
	TrackBarZoom->Tag = 0; // unblock
}

// ---------------------------------------------------------------------------
// устанавливает величину зума от 10 до 1000
void TFrameTreeBase::SetFixedZoom(const double dZoom) {
	TrackBarZoom->Tag = 1; // block
	DoSetZoom(dZoom);
	// здесь применяю именно int zoom, так как может быть разница с float на единицу
	TrackBarZoom->Position = TreeBase->View3DOptions->Zoom;

	TrackBarZoom->Tag = 0; // unblock
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionSaveTreeAsBeforeExecute(TObject *Sender) {

	if (m_sOpenedFileName != s_NEW_PROJECT_NAME) {
		ActionSaveTreeAs->Dialog->FileName = m_sOpenedFileName;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionSaveTreeAsAccept(TObject *Sender) {
	SaveTreeProject(ActionSaveTreeAs->Dialog->FileName);
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ClearModifications(void) {
	while (!m_UndoQueue.empty()) {
		delete m_UndoQueue.front().second;
		m_UndoQueue.pop_front();
	}
	// запишем исходный вид дерева
	m_UndoQueue.push_front(std::make_pair("Clear all", new TMemoryStream));
	m_undo_it = m_UndoQueue.begin();
	SaveTreeToStream(TreeBase, m_UndoQueue.front().second);

	g_b_modified = false;
	if (OnTreeBaseModified) {
		OnTreeBaseModified(this);
	}

	g_b_resizing_flag = false;
	g_b_moving_flag = false;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::DoCloseAll(void) {

	while (TreeBase->Shapes->Count) {
		TreeBase->Shapes->Items[0]->Free();
	}

	m_sOpenedFileName = "";
	ClearModifications();

	TTreeAccess *ATreeAccess = (TTreeAccess*)TreeBase;
	ATreeAccess->SetPolygonClass(0);
	ATreeAccess->SetZoomPriority(false);

	this->Enabled = false;
	PanelTools->Visible = false;
	PanelSideTools->Visible = false;

	g_b_resizing_flag = false;
	g_b_moving_flag = false;

	TreeBase->Color = clSilver;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionCloseExecute(TObject *Sender) {
	if (CheckModified()) {

		DoCloseAll();

	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionLoadTreeExecute(TObject *Sender) {
	LoadProject("" /* ПЕРЕДАЕМ ПУСТОЕ ИМЯ, ЧТОБЫ ВЫЗВАТЬ ДИАЛОГ ВЫБОРА */ );
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::LoadProject(const UnicodeString &sFileName) {
	try {

		if (CheckModified()) {

			UnicodeString sInitialDir = "";

			UnicodeString sOpenFileName = sFileName;
			// если передано пустое имя, вызываем диалог
			if (sOpenFileName.IsEmpty()) {
				PromptForFileName(sOpenFileName, L"Scxml files (*.scxml)|*.scxml|Tree files (*.ttr)|*.ttr|All files (*.*)|*.*", L"ttr",
					L"", sInitialDir, false);
			}

			if (!sOpenFileName.IsEmpty()) {
				LoadTreeProject(sOpenFileName);
			}

			int iRecentIndex = -1;
			for (int i = 0; i < SettingsData->RecentFiles->Count; i++) {
				if (SettingsData->RecentFiles->Strings[i].UpperCase() == sOpenFileName.UpperCase()) {
					iRecentIndex = i;
					break;
				}
			}
			if (iRecentIndex == -1) {
				SettingsData->RecentFiles->Insert(0, sOpenFileName);
				for (int i = SettingsData->RecentFiles->Count - 1; i > SettingsData->MaxRecentCount; i--) {
					SettingsData->RecentFiles->Delete(i);
				}
			}
			else {
				SettingsData->RecentFiles->Move(iRecentIndex, 0);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::LoadTreeProject(const UnicodeString & sFileName) {

	try {
		if (CheckModified()) {
			g_b_allow_history_save = false;
			DoCloseAll();

			this->Enabled = true;
			PanelTools->Visible = true;
			PanelSideTools->Visible = true;

			WLOG_INFO(L"Open file <%s>", sFileName.c_str());

			m_sOpenedFileName = sFileName;

			if (OnTreeBaseModified) {
				OnTreeBaseModified(this);
			}

			const UnicodeString sExt = ExtractFileExt(sFileName);
			if (sExt.UpperCase() == L".TTR") {
				TCustomTree *ATree = TreeBase;
				LoadTreeFromFile(ATree, m_sOpenedFileName);
			}
			else {
				Statemachine::LoadTreeFromScxml(TreeBase, m_sOpenedFileName);
			}

			// приходится смещать относительно 0 при масштабе=100%, так как иначе схема сбивается и не находится в нуле
			const int iRealZoom = TreeBase->View3DOptions->Zoom;
			TreeBase->View3DOptions->Zoom = 100;
			if (SettingsData->MoveToZeroAtStartup) {
				TreeBase->View3DOptions->HorizOffset = 0;
				TreeBase->View3DOptions->VertOffset = 0;
			}

			TreeBase->View3DOptions->Zoom = iRealZoom;
			TreeBase->Color = clWhite;

			ClearModifications();

			TrackBarZoom->Tag = 1;
			TrackBarZoom->Position = TreeBase->View3DOptions->Zoom;
			TrackBarZoom->Tag = 0;
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	g_b_allow_history_save = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionNewProjectExecute(TObject *Sender) {
	g_b_allow_history_save = false;

	try {

		DoCloseAll();

		this->Enabled = true;
		PanelTools->Visible = true;
		PanelSideTools->Visible = true;

		if (OnTreeBaseModified) {
			OnTreeBaseModified(this);
		}
		TCustomTree *ATree = TreeBase;
		m_NewProjectStreamPtr->Position = 0;
		LoadTreeFromStream(ATree, m_NewProjectStreamPtr.get());

		TreeBase->Refresh();

		SetFixedZoom(100);
		TreeBase->View3DOptions->HorizOffset = 0;
		TreeBase->View3DOptions->VertOffset = 0;

		m_sOpenedFileName = s_NEW_PROJECT_NAME;
		ClearModifications();

		TreeBase->Color = clWhite;
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	g_b_allow_history_save = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::FileSaveAsPictureAccept(TObject * Sender) {
	try {

		//		std::auto_ptr<TFormPictureData>AFormPictureData(new TFormPictureData(this));
		//		if (AFormPictureData->ShowModal() == mrOk) {
		//
		//			if (AFormPictureData->CheckCenterChart->Checked) {
		//				TreeBase->View3DOptions->HorizOffset = 0;
		//				TreeBase->View3DOptions->VertOffset = 0;
		//			}
		//
		//			TreeBase->SaveToBitmapFile(FileSaveAsPicture->Dialog->FileName, TRect(AFormPictureData->EditLeft->IntValue,
		//					AFormPictureData->EditTop->IntValue, AFormPictureData->EditWidth->IntValue, AFormPictureData->EditHeight->IntValue));
		//		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::FileMergeAccept(TObject * Sender) {
	// try {
	//
	// std::auto_ptr<TFormPictureData>AFormPictureData(new TFormPictureData(this));
	// AFormPictureData->EditWidth->Visible = false;
	// AFormPictureData->EditHeight->Visible = false;
	// AFormPictureData->CheckCenterChart->Visible = true;
	// AFormPictureData->CheckCenterChart->Checked = true;
	//
	// AFormPictureData->CheckCenterChart->ShowHint = true;
	// AFormPictureData->CheckCenterChart->Hint = "Будет найден самый правый объект \n"
	//
	// "и все новые объекты будут вставлены справа от него";
	//
	// AFormPictureData->CheckCenterChart->Caption = "Автосмещение вправо";
	// AFormPictureData->Caption = "СМЕЩЕНИЕ";
	//
	// if (AFormPictureData->ShowModal() == mrOk) {
	// int iMaxRight = 0;
	// if (AFormPictureData->CheckCenterChart->Checked) {
	// for (int i = 0; i < TreeBase->Shapes->Count; i++) {
	// if (TreeBase->Shapes->Items[i]->Bounds().Right > iMaxRight) {
	// iMaxRight = TreeBase->Shapes->Items[i]->Bounds().Right;
	// }
	// }
	// AFormPictureData->EditLeft->IntValue = iMaxRight + 100;
	// }
	//
	// std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
	// AMemoryStream->LoadFromFile(FileMerge->Dialog->FileName);
	// AMemoryStream->Position = 0;
	// LoadSelectedFromStream(AMemoryStream.get(), AFormPictureData->EditLeft->IntValue, AFormPictureData->EditTop->IntValue, false);
	//
	// TreeBase->View3DOptions->HorizOffset = -AFormPictureData->EditLeft->IntValue;
	// TreeBase->View3DOptions->VertOffset = -AFormPictureData->EditTop->IntValue;
	//
	// DoModified();
	//
	// g_b_modified = true;
	// if (OnTreeBaseModified) {
	// OnTreeBaseModified(this);
	// }
	//
	// String Caption = "WARNING!!!";
	// String Text = "You combined 2 database!\n"
	//
	// "Would you like to save one with new name?";
	//
	// if (Application->MessageBoxA(Text.w_str(), Caption.w_str(), MB_YESNO | MB_ICONWARNING) == IDYES) {
	// ActionSaveTreeAs->Execute();
	// }
	// }
	// }
	// catch(Exception * E) {
	// LOG_ERROR(LOG_ERROR_MSG);
	// }
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionPrintPreviewExecute(TObject * Sender) {

	TeePreview(this, TreeBase);
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionSaveTreeExecute(TObject * Sender) {

	SaveTreeProject(m_sOpenedFileName);
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::BitBtnTreeNodeShapeClick(TObject * Sender) {
	TTreeAccess *ATreeAccess = (TTreeAccess*)TreeBase;
	ATreeAccess->SetPolygonClass(0);
	ATreeAccess->SetZoomPriority(false);

}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::BitBtnInputChannelClick(TObject *Sender) {
	TTreeAccess *ATreeAccess = (TTreeAccess*)TreeBase;
	ATreeAccess->SetPolygonClass(0);
	ATreeAccess->SetZoomPriority(false);

	//	g_ShapeClass = __classid(TInputChannelShape);
}

// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::BitBtnEventShapeClick(TObject * Sender) {
	TTreeAccess *ATreeAccess = (TTreeAccess*)TreeBase;
	ATreeAccess->SetPolygonClass(0);
	ATreeAccess->SetZoomPriority(false);

	//	g_ShapeClass = __classid(TEventSystemShape);
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::BitBtnNetworkClick(TObject *Sender) {
	TTreeAccess *ATreeAccess = (TTreeAccess*)TreeBase;
	ATreeAccess->SetPolygonClass(0);
	ATreeAccess->SetZoomPriority(false);

	//	g_ShapeClass = __classid(TNetworkShape);
}

// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::BitBtnPluginShapeClick(TObject * Sender) {
	TTreeAccess *ATreeAccess = (TTreeAccess*)TreeBase;
	ATreeAccess->SetPolygonClass(0);
	ATreeAccess->SetZoomPriority(false);

	//	g_ShapeClass = __classid(TPluginShape);
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionCheckDesignExecute(TObject * Sender) {
	TreeBase->Designing = ActionCheckDesign->Checked;
	TreeBase->Grid->Visible = ActionCheckDesign->Checked;

	TreeBase->Refresh();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TFrameTreeBase::GetFileName() {
	return m_sOpenedFileName;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::TreeBaseMovingShape(TTreeNodeShape * Sender, int&DeltaX, int&DeltaY) {
	g_b_modified = true;
	g_b_moving_flag = true;
	if (OnTreeBaseModified) {
		OnTreeBaseModified(this);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::TreeBaseResizingShape(TTreeNodeShape * Sender, TTreeShapeHandle ACorner, int&DeltaX, int&DeltaY) {
	g_b_modified = true;
	g_b_resizing_flag = true;
	if (OnTreeBaseModified) {
		OnTreeBaseModified(this);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::TreeBaseDblClickConnection(TTreeConnection *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	// EditTreeConnection(this, Sender);
	ActionEditLevels->Execute();
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::SpeedBtnConnectionClick(TObject *Sender) {
	if (SpeedBtnConnection->Down) {
		TreeBase->Cursor = crCross;
		TreeBase->OriginalCursor = crCross;
		TTreeConnection *AConnection = TreeBase->StartConnecting();
		if (AConnection) {

			AConnection->Style = csLine;
			AConnection->Border->Color = clRed;
			AConnection->Text->Angle = SettingsData->ConnectionTextAngle;
			AConnection->Text->VertAlign = SettingsData->ConnectionVertTextAlign;
			AConnection->Text->VertOffset = SettingsData->ConnectionVertOffset;
			AConnection->Text->HorizAlign = SettingsData->ConnectionHorizTextAlign;
			AConnection->Text->HorizOffset = SettingsData->ConnectionHorizOffset;

			const UnicodeString sModifiedDesc = "Connected " + GetShapeFirstStr(AConnection->FromShape) + " to " + GetShapeFirstStr
				(AConnection->ToShape);

			DoModified(sModifiedDesc);
		}
		SpeedBtnConnection->Down = false;
	}
	else {
		TreeBase->StopConnecting();
	}
	TreeBase->Cursor = crDefault;
	TreeBase->OriginalCursor = crDefault;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::TreeBaseClickConnection(TTreeConnection *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	TreeBase->SelectConnection(Sender);

	//	if (FormTriggers && FormTriggers->Showing) {
	//		std::auto_ptr<TStringListEx>AStringListEx(new TStringListEx());
	//		AStringListEx->Text = Sender->Text->Text;
	//
	//		FormTriggers->SelectTriggerByGuid(AStringListEx->ReadString("VOLUME_TRIGGER", "guid", ""));
	//	}

}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionEditConnectionExecute(TObject *Sender) {
	if (TreeBase->Connections->Selected) {
		EditTreeConnection(this, TreeBase->Connections->Selected);
		DoModified("Connection modified");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionEditLevelsExecute(TObject *Sender) {
	//	if (TreeBase->Connections->Selected) {
	//		std::auto_ptr<TStringListEx>ATransitionList(new TStringListEx());
	//		ATransitionList->Text = TreeBase->Connections->Selected->Text->Text;
	//
	//		std::auto_ptr<TStringListEx>ALevelsList(new TStringListEx());
	//		ATransitionList->ReadSectionEx(ALevelsList.get(), "LEVELS");
	//
	//		std::auto_ptr<TStringListEx>ATriggerVolumeList(new TStringListEx());
	//		ATransitionList->ReadSectionEx(ATriggerVolumeList.get(), "VOLUME_TRIGGER");
	//
	//		std::auto_ptr<TFormLevelsEditor>AFormLevelsEditor(new TFormLevelsEditor(this, ALevelsList->Text));
	//		AFormLevelsEditor->EditMinMax->FloatValue = ATransitionList->ReadDouble("VOLUME", "Value", 1.0f);
	//		AFormLevelsEditor->EditTriggerConnectionVol->Hint = ATriggerVolumeList->Text;
	//		AFormLevelsEditor->EditTriggerConnectionVol->Text = ATriggerVolumeList->Values["trigger"];
	//
	//		if (AFormLevelsEditor->ShowModal() == mrOk) {
	//			ATransitionList->Clear();
	//
	//			ALevelsList->Text = AFormLevelsEditor->GetLevelsStr();
	//			if (!ALevelsList->Text.IsEmpty()) {
	//				ATransitionList->WriteSection(ALevelsList.get(), "LEVELS");
	//			}
	//
	//			if (!AFormLevelsEditor->EditTriggerConnectionVol->Hint.IsEmpty()) {
	//				ATriggerVolumeList->Text = AFormLevelsEditor->EditTriggerConnectionVol->Hint;
	//				ATransitionList->WriteSection(ATriggerVolumeList.get(), "VOLUME_TRIGGER");
	//			}
	//
	//			if (AFormLevelsEditor->EditMinMax->FloatValue != 1.0f) {
	//				ATransitionList->WriteDouble("VOLUME", "Value", AFormLevelsEditor->EditMinMax->FloatValue);
	//			}
	//
	//			TreeBase->Connections->Selected->Text->Text = ATransitionList->Text;
	//
	//			DoModified("Connection levels modified");
	//			RunTester();
	//		}
	//	}
}

// ---------------------------------------------------------------------------
bool IsSameFrameParam(UnicodeString sSource, UnicodeString sTarget) {
	//	std::auto_ptr<TProtocolControlBinding>AProtocolBinding(new TProtocolControlBinding(dctActive));
	//	// dctActive - переопределится в назначении SetText
	//	AProtocolBinding->SetShapeText(sSource);
	//
	//	return AProtocolBinding->GetControlNameWithArrayPart() == sTarget;
}

// ---------------------------------------------------------------------------
void TFrameTreeBase::ParseTree(TTree *ATree, const UnicodeString &sFileName) {

	//	if (FormTriggers) {
	//		FormTriggers->CheckConnectToProtocol->Checked = false;
	//	}
	//
	//	std::vector<UnicodeString>VecActions;
	//
	//	// проверяем, нет ли дубликатов гуидов
	//	std::auto_ptr<TStringList>AGuidList(new TStringList());
	//
	//	std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(this));
	//	AXMLDocument->Active = true;
	//	_di_IXMLNode ADocNode = AXMLDocument->AddChild("root");
	//
	//	_di_IXMLNode ASettingsNode = ADocNode->AddChild("settings");
	//	_di_IXMLNode AStatesNode = ADocNode->AddChild("states");
	//	_di_IXMLNode ATransitionsNode = ADocNode->AddChild("transitions");
	//	_di_IXMLNode ATriggersNode = ADocNode->AddChild("actions");
	//
	//	std::auto_ptr<TStringList>ADuplicatesList(new TStringList());
	//
	//	for (int i = 0; i < ATree->Shapes->Count; i++) {
	//		TTreeNodeShape *AShape = ATree->Shapes->Items[i];
	//
	//		TProjectOptionsShape *AOptionsShape = dynamic_cast<TProjectOptionsShape*>(AShape);
	//		if (AOptionsShape) {
	//			ASettingsNode->ChildValues["record_driver"] = AOptionsShape->RecordDriverId;
	//			ASettingsNode->ChildValues["replay_driver"] = AOptionsShape->ReplayDriverId;
	//			ASettingsNode->ChildValues["output_type"] = AOptionsShape->OutputType;
	//			ASettingsNode->ChildValues["speaker_mode"] = AOptionsShape->SpeakerMode;
	//			ASettingsNode->ChildValues["channel_count"] = AOptionsShape->ChannelsCount;
	//			ASettingsNode->ChildValues["project"] = AOptionsShape->EventProjectName;
	//			ASettingsNode->ChildValues["sample_rate"] = AOptionsShape->SampleRate;
	//			ASettingsNode->ChildValues["sound_format"] = AOptionsShape->SoundFormat;
	//			ASettingsNode->ChildValues["record_buffer"] = AOptionsShape->RecordBuffer;
	//			ASettingsNode->ChildValues["record_latency"] = AOptionsShape->RecordLatency;
	//			ASettingsNode->ChildValues["dsp_buff_length"] = AOptionsShape->DSPBufferLength;
	//			ASettingsNode->ChildValues["dsp_num_buffers"] = AOptionsShape->DSPNumBuffers;
	//			continue;
	//		}
	//
	//		const UnicodeString sId = AShape->Text->Values["id"];
	//		const UnicodeString sTrigger = AShape->Text->Values["trigger"];
	//		if (!sId.IsEmpty()) {
	//			const UnicodeString sType = AShape->Text->Values["type"];
	//
	//			_di_IXMLNode AStateNode = AStatesNode->AddChild("state");
	//			AStateNode->SetAttribute("id", sId);
	//			AStateNode->SetAttribute("type", sType.IsEmpty() ? UnicodeString("mixer") : sType);
	//			AStateNode->SetAttribute("active", StrToBoolDef(AShape->Text->Values["active"], true));
	//			AStateNode->SetAttribute("bypass", StrToBoolDef(AShape->Text->Values["bypass"], false));
	//			AStateNode->SetAttribute("channels", AShape->Text->Values["channels"].ToIntDef(0));
	//			if (!AShape->Text->Values["params"].IsEmpty()) {
	//				AStateNode->SetAttribute("params", AShape->Text->Values["params"]);
	//			}
	//			if (!AShape->Text->Values["groups"].IsEmpty()) {
	//				AStateNode->ChildValues["groups"] = AShape->Text->Values["groups"];
	//			}
	//			if (!AShape->Text->Values["startup"].IsEmpty()) {
	//				AStateNode->ChildValues["startup"] = AShape->Text->Values["startup"];
	//			}
	//			if (!AShape->Text->Values["models"].IsEmpty()) {
	//				AStateNode->ChildValues["models"] = AShape->Text->Values["models"];
	//			}
	//			if (!AShape->Text->Values["plugin_name"].IsEmpty()) {
	//				AStateNode->ChildValues["plugin_name"] = AShape->Text->Values["plugin_name"];
	//			}
	//
	//			if (!AShape->Text->Values["network_type"].IsEmpty()) {
	//				AStateNode->ChildValues["network_type"] = AShape->Text->Values["network_type"].ToIntDef(0);
	//			}
	//			if (!AShape->Text->Values["multicast_address"].IsEmpty()) {
	//				AStateNode->ChildValues["multicast_address"] = AShape->Text->Values["multicast_address"];
	//			}
	//			if (!AShape->Text->Values["multicast_port"].IsEmpty()) {
	//				AStateNode->ChildValues["multicast_port"] = AShape->Text->Values["multicast_port"].ToIntDef(0);
	//			}
	//			if (!AShape->Text->Values["listen_address"].IsEmpty()) {
	//				AStateNode->ChildValues["listen_address"] = AShape->Text->Values["listen_address"].ToIntDef(0);
	//			}
	//			if (!AShape->Text->Values["record_channels"].IsEmpty()) {
	//				AStateNode->ChildValues["record_channels"] = AShape->Text->Values["record_channels"];
	//			}
	//
	//			if (ADuplicatesList->IndexOf(sId) != -1) {
	//				AShape->ImageIndex = tiNetShare;
	//				AShape->Text->Values["id"] = StringReplace(AShape->Name, "TreeNodeShape", "DSP ", TReplaceFlags());
	//				Log("Duplicated id <" + sId + ">",slWarning);
	//			}
	//			else {
	//				AShape->ImageIndex = tiNone;
	//				ADuplicatesList->Add(sId);
	//			}
	//		}
	//		if (!sTrigger.IsEmpty()) {
	//			_di_IXMLNode ATriggerNode = ATriggersNode->AddChild("action");
	//			if (AShape->Parent) {
	//				ATriggerNode->SetAttribute("DSP", AShape->Parent->Text->Values["id"]);
	//			}
	//			for (int k = 0; k < AShape->Text->Count; k++) {
	//				if (!AShape->Text->Names[k].IsEmpty() && !AShape->Text->ValueFromIndex[k].IsEmpty()) {
	//					ATriggerNode->ChildValues[AShape->Text->Names[k]] = AShape->Text->ValueFromIndex[k];
	//				}
	//			}
	//
	//			if (AGuidList->IndexOf(AShape->Text->Values["guid"]) != -1) {
	//				const UnicodeString sGuid = AShape->Text->Values["guid"];
	//				_GUID AGuid;
	//				CreateGUID(AGuid);
	//				AShape->Text->Values["guid"] = GUIDToString(AGuid);
	//				Log("Trigger <" + sTrigger + "> duplicated guid <" + sGuid + "> changed to <" + AShape->Text->Values["guid"] + ">",slWarning);
	//			}
	//			AGuidList->Add(AShape->Text->Values["guid"]);
	//
	//			VecActions.push_back(AShape->Text->Text);
	//		}
	//	}
	//
	//	for (int i = 0; i < ATree->Connections->Count; i++) {
	//		TTreeConnection *AConnection = ATree->Connections->Items[i];
	//
	//		if (AConnection->FromShape && AConnection->ToShape) {
	//			const UnicodeString sSource = AConnection->FromShape->Text->Values["id"];
	//			const UnicodeString sTarget = AConnection->ToShape->Text->Values["id"];
	//			if (!sSource.IsEmpty() && !sTarget.IsEmpty()) {
	//				_di_IXMLNode ATransitionNode = ATransitionsNode->AddChild("transition");
	//				ATransitionNode->SetAttribute("source", sSource);
	//				ATransitionNode->SetAttribute("target", sTarget);
	//
	//				std::auto_ptr<TStringListEx>ATextList(new TStringListEx());
	//				ATextList->Text = AConnection->Text->Text;
	//
	//				if (ATextList->SectionExists("LEVELS")) {
	//					std::auto_ptr<TStringList>ALevelsList(new TStringList());
	//					ATextList->ReadSectionEx(ALevelsList.get(), "LEVELS");
	//					ATransitionNode->ChildValues["levels"] = ALevelsList->Text.Trim();
	//				}
	//				ATransitionNode->SetAttribute("volume", ATextList->ReadDouble("VOLUME", "Value", 1.0));
	//
	//				if (ATextList->SectionExists("VOLUME_TRIGGER")) {
	//					std::auto_ptr<TStringList>AVolTriggerList(new TStringList());
	//					ATextList->ReadSectionEx(AVolTriggerList.get(), "VOLUME_TRIGGER");
	//
	//					_di_IXMLNode ATriggerNode = ATriggersNode->AddChild("action");
	//
	//					ATriggerNode->SetAttribute("DSP", sSource);
	//					ATriggerNode->SetAttribute("TargetDSP", sTarget);
	//
	//					for (int k = 0; k < AVolTriggerList->Count; k++) {
	//						if (!AVolTriggerList->Names[k].IsEmpty() && !AVolTriggerList->ValueFromIndex[k].IsEmpty()) {
	//							ATriggerNode->ChildValues[AVolTriggerList->Names[k]] = AVolTriggerList->ValueFromIndex[k];
	//						}
	//					}
	//
	//					UnicodeString sGuid = AVolTriggerList->Values["guid"];
	//					if (AGuidList->IndexOf(sGuid) != -1) {
	//						_GUID AGuid;
	//						CreateGUID(AGuid);
	//						const UnicodeString sNewGuid = GUIDToString(AGuid);
	//						ATextList->WriteString("VOLUME_TRIGGER", "guid", sNewGuid);
	//						Log("Trigger <" + AVolTriggerList->Values["trigger"] + "> duplicated guid <" + sGuid + "> changed to <" +
	//							sNewGuid + ">",slWarning);
	//						sGuid = sNewGuid;
	//
	//						AConnection->Text->Text = ATextList->Text;
	//					}
	//					AGuidList->Add(sGuid);
	//
	//					VecActions.push_back(AVolTriggerList->Text);
	//				}
	//			}
	//		}
	//	}
	//	AXMLDocument->XML->Text = FormatXMLData(AXMLDocument->XML->Text);
	//	AXMLDocument->Active = true;
	//
	//	AXMLDocument->SaveToFile(sFileName);
	//
	//	if (FormTriggers) {
	//
	//		for (int i = (FormTriggers->ScrollBox1->ComponentCount - 1); i >= 0; i--) {
	//			TFrameTriggerParam * AFrameParam = dynamic_cast<TFrameTriggerParam*>(FormTriggers->ScrollBox1->Components[i]);
	//			if (AFrameParam) {
	//				std::vector<UnicodeString>::iterator it = std::find_if(VecActions.begin(), VecActions.end(),
	//					boost::bind(IsSameFrameParam, _1, AFrameParam->m_sControlNameWithArrayPart));
	//				if (it != VecActions.end()) {
	//					std::auto_ptr<TProtocolControlBinding>AProtocolBinding(new TProtocolControlBinding(dctActive));
	//					// dctActive - переопределится в назначении SetText
	//					AProtocolBinding->SetShapeText(*it);
	//					this->AssignProtocolBindingToFrameParam(AProtocolBinding.get(), AFrameParam);
	//					VecActions.erase(it);
	//				}
	//				else {
	//					delete AFrameParam;
	//				}
	//			}
	//		}
	//
	//		for (std::size_t i = 0; i < VecActions.size(); i++) {
	//
	//			std::auto_ptr<TProtocolControlBinding>AProtocolBinding(new TProtocolControlBinding(dctActive));
	//			// dctActive - переопределится в назначении SetText
	//			AProtocolBinding->SetShapeText(VecActions[i]);
	//
	//			if (AProtocolBinding->Active) {
	//
	//				bool bAlreadyBinded = false;
	//				for (int k = 0; k < FormTriggers->ScrollBox1->ComponentCount; k++) {
	//					const UnicodeString sIterTrigger = GetPropValue(FormTriggers->ScrollBox1->Components[k], "Hint", true);
	//
	//					if (sIterTrigger == AProtocolBinding->GetControlNameWithArrayPart()) {
	//						bAlreadyBinded = true;
	//						break;
	//					}
	//				}
	//				if (!bAlreadyBinded) {
	//					TFrameTriggerParam *AFrameParam = new TFrameTriggerParam(FormTriggers->ScrollBox1);
	//					AFrameParam->Name = TeeGetUniqueName(FormTriggers->ScrollBox1, "FrameParam");
	//
	//					this->AssignProtocolBindingToFrameParam(AProtocolBinding.get(), AFrameParam);
	//				}
	//
	//			}
	//		}
	//	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::MemoDescriptionKeyPress(TObject *Sender, wchar_t &Key) {
	//	if (Key == VK_RETURN) {
	//		if (DModule && SettingsData) {
	//
	//			TMemo * AMemo = dynamic_cast<TMemo*>(Sender);
	//			if (AMemo) {
	//				TFrameTriggerParam * AFrameParam = dynamic_cast<TFrameTriggerParam*>(AMemo->Owner);
	//				if (AFrameParam) {
	//
	//					TTreeNodeShape *ATriggerShape = GetShapeFromGuid(TreeBase, AFrameParam->m_sGuid);
	//
	//					if (ATriggerShape) {
	//						if (ATriggerShape->Parent) {
	//
	//							if (AFrameParam->m_DspControlType == dctActive || AFrameParam->m_DspControlType == dctBypass) {
	//
	//								std::auto_ptr<TProtocolControlBinding>AProtocolControlBinding(new TProtocolControlBinding(dctActive));
	//								AProtocolControlBinding->SetShapeText(ATriggerShape->Text->Text);
	//
	//								const bool bIsOn = AMemo->Text.Trim() == AProtocolControlBinding->ValueOn;
	//
	//								if (AFrameParam->m_DspControlType == dctActive) {
	//									ATriggerShape->Parent->Font->Color = bIsOn ? clBlack : clLtGray;
	//									ATriggerShape->Parent->Border->Color = bIsOn ? clBlack : clRed;
	//								}
	//								if (AFrameParam->m_DspControlType == dctBypass) {
	//									ATriggerShape->Parent->Border->Style = bIsOn ? psDot : psSolid;
	//								}
	//
	//							}
	//
	//						}
	//					}
	//
	//					if (SettingsData->TestingEnabled) {
	//						switch(AFrameParam->m_ProtocolControlType) {
	//						case pctString: {
	//								const UnicodeString sMsg = "@SEND_STRING@" + AFrameParam->m_sControlNameWithArrayPart + "=" + AMemo->Text;
	//								DModule->UdpSocket1->Sendln(sMsg);
	//								Log(sMsg,slDebug);
	//							}break;
	//						}
	//					}
	//
	//				}
	//			}
	//
	//		}
	//		Key = 0;
	//	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::OnTriggerParamChanged(TObject *Sender) {
	//
	//	TFrameTriggerParam * AFrameParam = dynamic_cast<TFrameTriggerParam*>(Sender);
	//	if (!AFrameParam)
	//		return;
	//
	//	TTreeNodeShape *ATriggerShape = GetShapeFromGuid(TreeBase, AFrameParam->m_sGuid);
	//
	//	if (ATriggerShape) {
	//		if (ATriggerShape->Parent) {
	//
	//			if (AFrameParam->m_DspControlType == dctActive || AFrameParam->m_DspControlType == dctBypass) {
	//
	//				std::auto_ptr<TProtocolControlBinding>AProtocolControlBinding(new TProtocolControlBinding(dctActive));
	//				AProtocolControlBinding->SetShapeText(ATriggerShape->Text->Text);
	//
	//				bool bIsOn = false;
	//
	//				switch(AFrameParam->m_ProtocolControlType) {
	//				case pctInteger:
	//				case pctIntegerArray: {
	//						int i_val = AFrameParam->EditMinMax->FloatValue;
	//						if (AProtocolControlBinding->ReceiveFunctionTypes.Contains(crftInvertOne)) {
	//							if (i_val == 0) {
	//								i_val = 1;
	//							}
	//							else if (i_val == 1) {
	//								i_val = 0;
	//							}
	//						}
	//
	//						bIsOn = AProtocolControlBinding->ValueOn == UnicodeString(i_val);
	//
	//					}break;
	//				case pctDouble:
	//				case pctDoubleArray: {
	//
	//						double d_val = AFrameParam->EditMinMax->FloatValue;
	//						if (true) {
	//							d_val = 1 - d_val;
	//						}
	//
	//						bIsOn = AProtocolControlBinding->ValueOn == UnicodeString(d_val);
	//					}break;
	//				}
	//
	//				if (AFrameParam->m_DspControlType == dctActive) {
	//					ATriggerShape->Parent->Font->Color = bIsOn ? clBlack : clLtGray;
	//					ATriggerShape->Parent->Border->Color = bIsOn ? clBlack : clRed;
	//				}
	//				if (AFrameParam->m_DspControlType == dctBypass) {
	//					ATriggerShape->Parent->Border->Style = bIsOn ? psDot : psSolid;
	//				}
	//
	//			}
	//
	//		}
	//	}
	//
	//	if (DModule && SettingsData) {
	//		if (SettingsData->TestingEnabled) {
	//			UnicodeString sMsg = "";
	//
	//			switch(AFrameParam->m_ProtocolControlType) {
	//			case pctInteger: {
	//					sMsg = "@SEND_INTEGER@" + AFrameParam->m_sControlNameWithArrayPart + "=" + UnicodeString
	//						((int)AFrameParam->EditMinMax->FloatValue);
	//				}break;
	//			case pctDouble: {
	//					sMsg = "@SEND_DOUBLE@" + AFrameParam->m_sControlNameWithArrayPart + "=" + UnicodeString
	//						(AFrameParam->EditMinMax->FloatValue);
	//				}break;
	//			case pctIntegerArray: {
	//					sMsg = "@SEND_INTEGER_ARRAY@" + AFrameParam->m_sControlNameWithArrayPart + "=" + UnicodeString
	//						((int)AFrameParam->EditMinMax->FloatValue);
	//				}break;
	//			case pctDoubleArray: {
	//					sMsg = "@SEND_DOUBLE_ARRAY@" + AFrameParam->m_sControlNameWithArrayPart + "=" + UnicodeString
	//						(AFrameParam->EditMinMax->FloatValue);
	//				}break;
	//			}
	//
	//			if (!sMsg.IsEmpty()) {
	//				DModule->UdpSocket1->Sendln(sMsg);
	//#ifdef _DEBUG
	//				Log(sMsg,slDebug);
	//#endif
	//			}
	//
	//		}
	//	}
}

// ---------------------------------------------------------------------------
void TFrameTreeBase::AssignProtocolBindingToFrameParam(TProtocolControlBinding *ABinding, TFrameTriggerParam *AFrameParam) {

	//	if (FormTriggers->ScrollBox1->ControlCount && AFrameParam->Parent != FormTriggers->ScrollBox1) {
	//		AFrameParam->Top = FormTriggers->ScrollBox1->Controls[FormTriggers->ScrollBox1->ControlCount - 1]->BoundsRect.Bottom + 3;
	//	}
	//
	//	AFrameParam->m_sControlName = SettingsData->MacroToText(ABinding->ControlName);
	//	AFrameParam->m_sControlNameWithArrayPart = SettingsData->MacroToText(ABinding->GetControlNameWithArrayPart());
	//	AFrameParam->m_iArrayElement = ABinding->ArrayElement;
	//	AFrameParam->m_iArraySize = ABinding->ArraySize;
	//	AFrameParam->m_ReceiveFunctionTypes = ABinding->ReceiveFunctionTypes;
	//	AFrameParam->m_ProtocolControlType = ABinding->ProtocolControlType;
	//	AFrameParam->m_DspControlType = ABinding->DspControlType;
	//	AFrameParam->m_sGuid = ABinding->Guid;
	//
	//	const UnicodeString sNotesPart = ABinding->Notes.IsEmpty() ? UnicodeString("") : (" { " + ABinding->Notes + " }");
	//	AFrameParam->LabelParameterInfo->Caption = AFrameParam->m_sControlNameWithArrayPart + sNotesPart;
	//	AFrameParam->EditTriggerParam->Visible = false;
	//	AFrameParam->MemoDescription->Visible = ABinding->ProtocolControlType == pctString;
	//	AFrameParam->Panel2->Visible = ABinding->ProtocolControlType != pctString;
	//
	//	AFrameParam->Panel1->AutoSize = true;
	//	AFrameParam->Height = AFrameParam->Panel2->BoundsRect.Bottom;
	//
	//	AFrameParam->Parent = FormTriggers->ScrollBox1;
	//
	//	AFrameParam->EditMinMax->MinFloatValue = StrToFloatDef(ABinding->Min, 0.0f);
	//	AFrameParam->EditMinMax->MaxFloatValue = StrToFloatDef(ABinding->Max, 0.0f);
	//
	//	AFrameParam->OnEnter = &OnTriggerFrameEnter;
	//
	//	float fStep = (AFrameParam->EditMinMax->MaxFloatValue - AFrameParam->EditMinMax->MinFloatValue) / 100.0f;
	//	AFrameParam->EditMinMax->IncrementFloat = RoundTo(fStep, -3);
	//	AFrameParam->Precision = 1.0f;
	//	if (ABinding->ProtocolControlType == pctDouble || ABinding->ProtocolControlType == pctDoubleArray) {
	//		if (fStep < 1) {
	//			UnicodeString sPrecision = FormatFloat("0.#########", fStep);
	//			sPrecision = StringReplace(sPrecision, "0.", "", TReplaceFlags());
	//			int iCount = 1;
	//			for (int i = 1; i <= sPrecision.Length(); i++) {
	//				if (sPrecision[i] == '0') {
	//					iCount++;
	//				}
	//			}
	//			AFrameParam->Precision = AFrameParam->Precision / (Power(10.0f, iCount));
	//		}
	//	}
	//	else {
	//		// fStep = RoundTo(fStep * 100.0f, 0);
	//		AFrameParam->EditMinMax->IncrementFloat = fStep;
	//	}
	//
	//	AFrameParam->TrackValue->Min = AFrameParam->EditMinMax->MinFloatValue / AFrameParam->Precision;
	//	AFrameParam->TrackValue->Max = AFrameParam->EditMinMax->MaxFloatValue / AFrameParam->Precision;
	//	AFrameParam->TrackValue->Frequency = fStep / AFrameParam->Precision;
	//
	//	if (ABinding->ProtocolControlType == pctString) {
	//		AFrameParam->MemoDescription->OnKeyPress = &MemoDescriptionKeyPress;
	//	}
	//	else {
	//		AFrameParam->OnParameterChanged = &OnTriggerParamChanged;
	//	}
	//
	//	AFrameParam->Align = alTop;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::WndProc(Messages::TMessage &Message) {
	TFrame::WndProc(Message);

	switch(Message.Msg) {
	case WM_SAFE_DELETE: {

			ActionDeleteSelected->Execute();

		}break;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::OnTriggerFrameEnter(TObject *Sender) {
	//	TFrameTriggerParam * AFrameParam = dynamic_cast<TFrameTriggerParam*>(Sender);
	//	if (!AFrameParam)
	//		return;
	//
	//	// если указано смещать в центр
	//	if (FormTriggers->CheckCenterSelected->Checked) {
	//		TTreeNodeShape *ATriggerShape = GetShapeFromGuid(TreeBase, AFrameParam->m_sGuid);
	//
	//		if (ATriggerShape) {
	//			if (ATriggerShape->Parent) {
	//				if (AFrameParam->m_DspControlType == dctActive || AFrameParam->m_DspControlType == dctBypass) {
	//					ATriggerShape = ATriggerShape->Parent;
	//				}
	//				else {
	//					ATriggerShape->Parent->Expanded = true;
	//				}
	//				SelectShapeInCenter(TreeBase, ATriggerShape);
	//			}
	//		}
	//		else {
	//			TTreeConnection *ATriggerConnection = GetConnectionFromGuid(TreeBase, AFrameParam->m_sGuid);
	//			if (ATriggerConnection) {
	//				SelectConnectionInCenter(TreeBase, ATriggerConnection);
	//			}
	//		}
	//	}

}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionFindElementExecute(TObject *Sender) {
	//	assert(FormSearch != NULL);
	//
	//	const int iHalfWidth = this->ClientWidth / 2;
	//	const int iHalfHeight = this->ClientHeight / 2;
	//
	//	const int iSearchWidth = FormSearch->Width / 2;
	//
	//	TPoint ptPos, ptPosRes;
	//	ptPos.x = iHalfWidth - iSearchWidth;
	//	ptPos.y = iHalfHeight - FormSearch->Height * 2;
	//
	//	ptPosRes = this->ClientToScreen(ptPos);
	//
	//	FormSearch->SetBounds(ptPosRes.x, ptPosRes.y, FormSearch->Width, FormSearch->Height);
	//
	//	FormSearch->Show();
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionUpdateColorsExecute(TObject *Sender) {
	//	for (int i = 0; i < TreeBase->Shapes->Count; i++) {
	//		if (TreeBase->Shapes->Items[i]->Level() == 0) {
	//			bool b_is_head = TreeBase->Shapes->Items[i]->Text->Values["id"] == DSP_HEAD;
	//			bool b_is_channel_mix = TreeBase->Shapes->Items[i]->Text->Values["id"] == DSP_CHANNEL_MIX;
	//			bool b_is_program_options = TreeBase->Shapes->Items[i]->Text->Text.Pos(DSP_PROGRAM_OPTIONS) == 1;
	//
	//			if (!b_is_head && !b_is_channel_mix && !b_is_program_options) {
	//				const bool b_active = StrToBoolDef(TreeBase->Shapes->Items[i]->Text->Values["active"], true);
	//				const bool b_bypass = StrToBoolDef(TreeBase->Shapes->Items[i]->Text->Values["bypass"], false);
	//
	//				TreeBase->Shapes->Items[i]->Font->Color = b_active ? clBlack : clLtGray;
	//				TreeBase->Shapes->Items[i]->Border->Color = b_active ? clBlack : clRed;
	//				TreeBase->Shapes->Items[i]->Border->Style = b_bypass ? psDot : psSolid;
	//			}
	//
	//		}
	//	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::TreeBaseStopEditing(TTreeNodeShape *Sender) {
	DoModified("DSP direct text modified");
}

// ---------------------------------------------------------------------------


void __fastcall TFrameTreeBase::TreeBaseAfterDraw(TObject *Sender) {
	TreeBase->Canvas->Pen->Color = clBlack;

	TreeBase->Canvas->HorizLine3D(0, Max((double)TreeBase->HorzScrollBar->Max,
			(double)TreeBase->Width / ((double)TreeBase->View3DOptions->Zoom / 100.0f)), 0, TeeTreeZ);
	TreeBase->Canvas->VertLine3D(0, 0, Max((double)TreeBase->VertScrollBar->Max,
			(double)TreeBase->Height / ((double)TreeBase->View3DOptions->Zoom / 100.0f)), TeeTreeZ);

	// TRect tmpBounds = TeeRect(70, 70, 150, 150);
	//
	// std::auto_ptr<Graphics::TBitmap>ABufferBitmap(new Graphics::TBitmap());
	// TeeSetBitmapSize(ABufferBitmap.get(), tmpBounds.Width(), tmpBounds.Height());
	//
	// TTeeCanvas3D *ACanvas = dynamic_cast<TTeeCanvas3D*>(TreeBase->Canvas);
	// ABufferBitmap->Canvas->CopyRect(TeeRect(0, 0, tmpBounds.Width(), tmpBounds.Height()), ACanvas->Bitmap->Canvas, tmpBounds);
	//
	// std::auto_ptr<TZoomFilter>AZoomFilter(new TZoomFilter(0));
	// AZoomFilter->Smooth = true;
	// AZoomFilter->Percent = 50;
	//
	// AZoomFilter->Apply(ABufferBitmap.get());
	//
	// int iTransparency = 0;
	// TTeeBlend*tmpBlend = 0;
	// if (iTransparency)
	// tmpBlend = TreeBase->Canvas->BeginBlending(tmpBounds, iTransparency);
	// else
	// tmpBlend = 0;
	//
	// TreeBase->Canvas->Draw(tmpBounds.Left, tmpBounds.Top, ABufferBitmap.get());
	//
	// if (tmpBlend)
	// TreeBase->Canvas->EndBlending(tmpBlend);
	//
	// TreeBase->Canvas->Pen->Color = clBlack;
	//
	// TreeBase->Canvas->Brush->Style = bsClear;
	//
	// TreeBase->Canvas->Rectangle(tmpBounds);

}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionZoom100Execute(TObject *Sender) {
	SetFixedZoom(100);
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTreeBase::ActionZoomInCenterExecute(TObject *Sender) {
	if (TreeBase->Selected->Count()) {
		SetFixedZoom(100);
		SelectShapeInCenter(TreeBase, TreeBase->Selected->First());
	}
	else if (TreeBase->Connections->Selected) {
		SetFixedZoom(100);
		TreeBase->Refresh();
		SelectConnectionInCenter(TreeBase, TreeBase->Connections->Selected);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFrameTreeBase::ActionOpenIncludeExecute(TObject *Sender) {
//	if (TreeBase->Selected->Count()) {
//
//		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>(TreeBase->Selected->First());
//		if (AShape) {
//			if (AShape->StateTypes.Contains(sstSource)) {
//
//				TFrameTreeBase *AFrameTreeBase = new TFrameTreeBase(FormScxmlGui);
//				AFrameTreeBase->Name = TeeGetUniqueName(FormScxmlGui, "FrameTreeBase");
//				FormScxmlGui->_ProjectList->Add(AFrameTreeBase);
//
//				TLMDDockPanel *AUnitDockPanel = new TLMDDockPanel(this);
//
//				FormScxmlGui->LMDDockSite1->DockControl(AUnitDockPanel, FormScxmlGui->DockPanelFrameRoot->Zone, alClient);
//
//				AFrameTreeBase->Parent = AUnitDockPanel;
//				AFrameTreeBase->Align = alClient;
//
//				AFrameTreeBase->LoadProject(TPath::Combine(AShape->SrcPath, AShape->Src));
//				AUnitDockPanel->Caption = AShape->Src;
//
//			}
//		}
//	}

}

//---------------------------------------------------------------------------

