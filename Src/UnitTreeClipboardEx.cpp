//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitTreeClipboardEx.h"

#include <map>

typedef std::map<TTreeNodeShape*, TTreeNodeShape*>TAccordingMap;

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool IsParentSelected(TTreeNodeShape *ANode) {
	TTreeNodeShape *tmpNode;
	bool bRes = false;
	tmpNode = ANode->Parent;
	while (tmpNode) {
		bRes = tmpNode->Selected;
		if (bRes)
			break;
		tmpNode = tmpNode->Parent;
	}
	return bRes;
}

//---------------------------------------------------------------------------
TTreeNodeShape * CloneShapeEx(TCustomTree * ATree, TTreeNodeShape * AShape, TAccordingMap & AMap);

//---------------------------------------------------------------------------
//create, add and return a new shape, clone of AShape parameter
TTreeNodeShape * AddCloneShapeEx(TCustomTree *ATree, TTreeNodeShape *AShape, TAccordingMap &AMap) {

	TTreeNodeShape *result = CloneShapeEx(ATree, AShape, AMap);
	result->Visible = true;

	if (!ATree->NoOwnerShapes) {
		result->Name = ATree->FindFreeName(result);
		if (result->Text->Count == 0) {
			result->SimpleText = result->Name;
		}
	}
	return result;
}

//---------------------------------------------------------------------------
TTreeNodeShape * CloneShapeEx(TCustomTree *ATree, TTreeNodeShape *AShape, TAccordingMap &AMap) {

	//	TNodeConnectionList* AConnections = AShape->Connections;
	//	for (int i = 0; i < AConnections->Count; i++) {
	//		OutputDebugStringW(UnicodeString().sprintf(L"[%d] name [%s] from [%s] to [%s]", i, AShape->Text->Text.Trim().c_str(),
	//				AConnections->Items[i]->FromShape->Text->Text.Trim().c_str(), AConnections->Items[i]->ToShape->Text->Text.Trim().c_str())
	//			.c_str());
	//
	//	}

	TTreeNodeShape* tmpShape = NULL;
	TTreeConnection* tmpConn = NULL;

	TTreeNodeShape * result = NULL;
	TMetaClass *clsid = Classes::FindClass(AShape->ClassName());
	Application->CreateForm(clsid, &result);

	result->Assign(AShape);
	result->Tree = ATree;
	result->Visible = false;

	AMap[AShape] = result;

	// { Clone Children nodes }
	for (int t = 0; t < AShape->Children->Count; t++) {
		tmpShape = AddCloneShapeEx(ATree, AShape->Children->Items[t], AMap); //tom:11/10/2002;

		// when NoOwnerShape = False
		// these childs should get a name...
		tmpShape->Parent = result;

		// исправляем неправильное смещение
		tmpShape->Left = AShape->Children->Items[t]->Left;
		tmpShape->Top = AShape->Children->Items[t]->Top;

		// { if we have a connection to this child node, add it }
//				if (AShape->Connections)
//					tmpConn = AShape->Connections->ToShape(AShape->Children->Items[t]);
//
//				if (tmpConn)
//					result->AddConnection(tmpShape)->Assign(tmpConn);

	}
	return result;
}

//---------------------------------------------------------------------------
__fastcall TTreeClipboardEx::TTreeClipboardEx(Classes::TComponent* AOwner) : TCustomTree(AOwner) {

}

//---------------------------------------------------------------------------
void RepairConnections(TNodeConnectionList* AConnections, TAccordingMap &AMap) {
	for (int i = 0; i < AConnections->Count; i++) {
		OutputDebugStringW(UnicodeString().sprintf(L"[%d] from [%s] to [%s]", i,
				AConnections->Items[i]->FromShape->Text->Text.Trim().c_str(), AConnections->Items[i]->ToShape->Text->Text.Trim().c_str())
			.c_str());

		TAccordingMap::iterator it_from = AMap.find(AConnections->Items[i]->FromShape);
		TAccordingMap::iterator it_to = AMap.find(AConnections->Items[i]->ToShape);
		if (it_from != AMap.end() && it_to != AMap.end()) {
			TTreeConnection *AConnection = it_from->second->AddConnection(it_to->second);
			AConnection->Name = it_from->second->Name + "_" + it_to->second->Name;
			AConnection->Assign(AConnections->Items[i]);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TTreeClipboardEx::Copy(TCustomTree *SourceTree) {
	TTreeNodeShape *tmp = NULL;
	FIsCut = NULL;
	if (SourceTree->Selected->Count() > 0) {
		this->Clear();

		TAccordingMap AMap;

		for (int t = 0; t < SourceTree->Selected->Count(); t++) {
			if (!IsParentSelected(SourceTree->Selected->Items[t])) {
				// tmp = CloneShape(SourceTree->Selected->Items[t]);
				tmp = CloneShapeEx(this, SourceTree->Selected->Items[t], AMap);

				//tom:11/10/2002 keep AutoPosition information; which is not kept by Assign method (for other reasons:eg AddChild)
				tmp->AutoPosition->Assign(SourceTree->Selected->Items[t]->AutoPosition);
				if (tmp->SimpleText == SourceTree->Selected->Items[t]->Name)
					tmp->Text->Clear();

				tmp->Parent = NULL;
				tmp->Visible = true;
				tmp->Tree = this;
			}
		}

		RepairConnections(SourceTree->Connections, AMap);
	}

}

//---------------------------------------------------------------------------
void __fastcall TTreeClipboardEx::Cut(TCustomTree *SourceTree) {
	Copy(SourceTree);
	if (SourceTree->Selected)
		SourceTree->Selected->Delete();
}

//---------------------------------------------------------------------------
void __fastcall TTreeClipboardEx::Paste(TCustomTree *DestTree) {
	DestTree->Selected->Clear();

	TAccordingMap AMap;

	for (int t = 0; t < this->Roots->Count; t++) {

		TTreeNodeShape *tmp = AddCloneShapeEx(DestTree, this->Roots->Items[t], AMap);
		tmp->AutoPosition->Assign(this->Roots->Items[t]->AutoPosition);

		//		if (!tmp->AutoPosition->Left)
		//			tmp->X0 = Roots->Items[t]->X0 + 15;
		//
		//		if (!tmp->AutoPosition->Top)
		//			tmp->Y0 = Roots->Items[t]->Y0 + 15;

		if (!tmp->AutoPosition->Left)
			tmp->Left = Roots->Items[t]->Left/* + 15*/;

		if (!tmp->AutoPosition->Top)
			tmp->Top = Roots->Items[t]->Top/* + 15*/;

		tmp->MoveRelative(15, 15, true);

		tmp->Selected = True;
	}

	RepairConnections(this->Connections, AMap);

}
