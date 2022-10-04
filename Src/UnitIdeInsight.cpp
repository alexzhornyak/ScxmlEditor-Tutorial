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

#include "UnitIdeInsight.h"

#include "Log4cpp_VCL.hpp"

#include <StrUtils.hpp>
#include "UnitHTMLUtils.h"
#include "UnitSettings.h"
#include "UnitMain.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitStateMachineUnit.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitFrameGifMaker.h"
#include "UnitMaskedTextShape.h"
#include "UnitTreeEx.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDialogInsight *DialogInsight = NULL;

class TInsightTextShape : public TMaskedTextShape {
private:

	TIdeNodeDataType FType;
	TObject *FItemObject;

public:
	__fastcall TInsightTextShape(Classes::TComponent* AOwner) : TMaskedTextShape(AOwner) {
		FType = indtLevel;
		FItemObject = NULL;
	}

	void __fastcall SetMask(const TIdeNodeDataType AType, const UnicodeString & sMask, //
		TObject * AItemObject = NULL, int iMatchPos = 0, const bool bAlwaysBold = false) {

		FType = AType;
		FItemObject = AItemObject;

		TMaskedTextShape::SetMask(sMask, iMatchPos, bAlwaysBold);
	}

	__property TIdeNodeDataType Type = {
		read = FType
	};
	__property TObject * ItemObject = {
		read = FItemObject
	};
};

//---------------------------------------------------------------------------
int InsightNodeMatchPos(TTreeNodeShape *ANode) {
	TInsightTextShape *AInsightNode = dynamic_cast<TInsightTextShape*>(ANode);
	if (AInsightNode) {
		return AInsightNode->MaskPos;
	}
	return 0;
}

//---------------------------------------------------------------------------
UnicodeString GetContainedPropName(TTreeNodeShape *ANode, const TIdeNodeDataType ADataType) {
	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	AStringListPtr->Delimiter = L'.';
	AStringListPtr->StrictDelimiter = true;

	while (ANode) {
		TInsightTextShape *AInsightNode = dynamic_cast<TInsightTextShape*>(ANode);
		if (!AInsightNode)
			break;
		if (ADataType != AInsightNode->Type)
			break;

		AStringListPtr->Insert(0, AInsightNode->SimpleText);

		ANode = ANode->Parent;
	}

	return AStringListPtr->DelimitedText;
}

//---------------------------------------------------------------------------
UnicodeString GetFullTreeName(TTreeNodeShape *ANode) {
	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	AStringListPtr->Delimiter = L'.';
	AStringListPtr->StrictDelimiter = true;

	while (ANode) {
		AStringListPtr->Insert(0, ANode->SimpleText);

		ANode = ANode->Parent;
	}

	return AStringListPtr->DelimitedText;
}

//---------------------------------------------------------------------------
//---------------------------- TDialogInsight -------------------------------
//---------------------------------------------------------------------------
__fastcall TDialogInsight::TDialogInsight(TComponent* Owner) : TForm(Owner), FStateMachineEditor(NULL) {
	FDoNotRestorePropertyInspector = false;

	TreeViewInsight->GlobalFormat.Border->Visible = false;
	TreeViewInsight->GlobalFormat.Transparent = false;
	TreeViewInsight->GlobalFormat.NodeClass = __classid(TInsightTextShape);
	TreeViewInsight->GlobalFormat.ImageIndex = tiNone;
	TreeViewInsight->NoOwnerShapes = true;
	TreeViewInsight->AssignParent = false;

	TTreeExplorerAlignChild *AChildManager = dynamic_cast<TTreeExplorerAlignChild*>(TreeViewInsight->GlobalFormat.ChildManager);
	if (AChildManager) {
		AChildManager->VertMargin = AChildManager->VertMargin + 1;

		std::auto_ptr<TPen>AChartPenPtr(new TPen);
		TreeViewInsight->GlobalFormat.Connection->Border->Assign(AChartPenPtr.get());
		TreeViewInsight->GlobalFormat.Connection->ArrowTo->Style = casNone;
	}
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TDialogInsight::GetSearchText(void) {
	return EditInsight->Text.Trim();
}

//---------------------------------------------------------------------------
TTreeNodeShape * __fastcall TDialogInsight::DoAddChildNode(TTreeNodeShape *AParent, const TIdeNodeDataType ADataType,
	const UnicodeString &sFullText, const UnicodeString &sSearchText, TCustomImageList *AFromImages/*=NULL*/,
	const int iImageIndex/*=-1*/, TObject *AItemObject /*=NULL*/, const bool bForceAdd/* = false*/) {

	TTreeNodeShape *AChildNode = NULL;

	const int iFindPos = sFullText.UpperCase().Pos(sSearchText.UpperCase());

	const bool bAdd = sSearchText.IsEmpty() || iFindPos;

	if (bAdd || bForceAdd || InsightNodeMatchPos(AParent)) {

		AChildNode = TreeViewInsight->Items->AddChild(AParent, sFullText);
		TInsightTextShape *AInsightNode = dynamic_cast<TInsightTextShape*>(AChildNode);
		if (!AInsightNode)
			throw Exception(L"Program Logic Error! Can not cast to <TInsightTextShape>!");

		if (AFromImages && iImageIndex != -1) {
			AInsightNode->ImageListIndex = ImageList1->AddImage(AFromImages, iImageIndex);
		}
		else {
			AInsightNode->ImageIndex = tiNone;
		}

		AInsightNode->SetMask(ADataType, sSearchText, AItemObject, iFindPos);

#if 0 // для глубокой отладки
		WLOG_DEBUG(L"Added node:[%s]", GetFullTreeName(AChildNode).c_str());
#endif
	}

	return AChildNode;
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::DoAddMenuItem(TTreeNodeShape *AParent, TMenuItem* AItem, TCustomImageList *AMenuImages) {
	if (AItem->Visible && AItem->Enabled && !AItem->IsLine()) {

		const UnicodeString sFullText = StripHotkey(AItem->Caption);

		TCustomImageList *AImages = AItem->ImageIndex == -1 ? ImageConsts : AMenuImages;
		const int iImageIndex = AItem->ImageIndex == -1 ? (AItem->Count == 0 ? (AItem->Checked ? 4 : 5) : -1) : AItem->ImageIndex;

		TTreeNodeShape *AChildNode = NULL;

		if (AItem->Count) {

			// возможно по клику дочерние узлы переорганизовываются
			if (AItem->OnClick) {
				AItem->OnClick(AItem);
			}

			AChildNode = DoAddChildNode(AParent, indtMenuItem, sFullText, GetSearchText(), AImages, iImageIndex, AItem, true);
		}
		else {
			AChildNode = DoAddChildNode(AParent, indtMenuItem, sFullText, GetSearchText(), AImages, iImageIndex, AItem);
		}

		if (AChildNode) {
			DoAddMenuItems(AChildNode, AItem, AMenuImages);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::DoAddMenuItems(TTreeNodeShape *AParent, TMenuItem* AItems, TCustomImageList *AMenuImages) {
	for (int i = 0; i < AItems->Count; i++) {
		DoAddMenuItem(AParent, AItems->Items[i], AMenuImages);
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::DoAddActionItems(TTreeNodeShape *AParent, TActionList *AListActions) {

	std::map<UnicodeString, TTreeNodeShape*>AMapCategory;

	for (int i = 0; i < AListActions->ActionCount; i++) {
		TCustomAction * ACustomAction = dynamic_cast<TCustomAction*>(AListActions->Actions[i]);
		if (ACustomAction) {

			// необходимо вызвать все OnUpdate методы, так как могут поменяться названия или
			// сами действия могут быть скрыты после этого
			if (ACustomAction->OnUpdate) {
				ACustomAction->OnUpdate(ACustomAction);
			}

			if (ACustomAction->Enabled && ACustomAction->Visible) {

				const UnicodeString sFullText = ACustomAction->Hint.IsEmpty() ? ACustomAction->Caption : ACustomAction->Hint;

				TCustomImageList *AImages = ACustomAction->ImageIndex == -1 ? ImageConsts : AListActions->Images;
				const int iImageIndex = ACustomAction->ImageIndex == -1 ? (ACustomAction->Checked ? 4 : 5) : ACustomAction->ImageIndex;

				TTreeNodeShape *ANewParent = AParent;
				if (!ACustomAction->Category.IsEmpty()) {
					std::map<UnicodeString, TTreeNodeShape*>::iterator it = AMapCategory.find(ACustomAction->Category);
					if (it != AMapCategory.end()) {
						ANewParent = it->second;
					}
					else {
						ANewParent = DoAddChildNode(AParent, indtLevel, ACustomAction->Category, GetSearchText(), NULL, -1, NULL, true);
						if (ANewParent) {
							AMapCategory.insert(std::make_pair(ACustomAction->Category, ANewParent));
						}
					}
				}

				DoAddChildNode(ANewParent, indtAction, sFullText, GetSearchText(), AImages, iImageIndex, ACustomAction);
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::DoAddPersistentItems(TTreeNodeShape *AParent, TObject *AInstance, const TIdeNodeDataType ADataType) {
	if (AInstance) {
		TTypeKinds ATypeKinds = TTypeKinds() << tkInteger << tkChar << tkEnumeration << tkFloat << tkString << tkSet << tkClass <<
			tkWChar << tkLString << tkWString << tkVariant << tkArray << tkRecord << tkInterface << tkInt64 << tkDynArray <<
			tkUString << tkClassRef << tkPointer << tkProcedure;

		TPropList APropList;
		const int iRes = GetPropList(PTypeInfo(AInstance->ClassInfo()), ATypeKinds, (TPropList*)(&APropList), false);

		for (int i = 0; i < iRes; i++) {
			const UnicodeString sPropName = AnsiString(APropList[i]->Name); // через Ansi, так как ShortString

			bool bIsPropIncluded = true;
			Propinspext::IsPropIncluded(AInstance, sPropName, bIsPropIncluded);
			if (bIsPropIncluded) {

				TTypeInfo *ATypeInfo = *APropList[i]->PropType;

				TTreeNodeShape *AChildNode = DoAddChildNode(AParent, ADataType, sPropName, GetSearchText(), ImageConsts, 2);

				switch(ATypeInfo->Kind) {
				case tkClass: {
						TObject *AGetObject = GetObjectProp(AInstance, sPropName);
						if (AGetObject) {

							if (!AChildNode) {
								AChildNode = DoAddChildNode(AParent, ADataType, sPropName, GetSearchText(), ImageConsts, 2, NULL, true);
							}

							DoAddPersistentItems(AChildNode, AGetObject, ADataType);
						}

					}break;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::DoAddPropInspItem(TTreeNodeShape *AParent, TLMDPropertyInspectorItem* AItem) {
	if (AItem) {

		AItem->Expand();

		TTreeNodeShape *AChildNode = NULL;
		if (AItem->PropInfo[0]->IsWritable()) {
			AChildNode = DoAddChildNode(AParent, indtPropInspector, AItem->PropName, GetSearchText(), ImageConsts, 2, AItem);
		}

		if (AItem->Count) {
			if (!AChildNode) {
				AChildNode = DoAddChildNode(AParent, indtPropInspector, AItem->PropName, GetSearchText(), ImageConsts, 2, AItem, true);
			}

			for (int i = 0; i < AItem->Count; i++) {
				DoAddPropInspItem(AChildNode, AItem->Items[i]);
			}
		}
	}
}

//---------------------------------------------------------------------------
TTreeNodeShape *AddTreeChildObjectData(TTree *ATree, TTreeNodeShape *AParent, const TIdeNodeDataType AType, const UnicodeString & sText,
	const UnicodeString & sSearchText, TObject * AItemObject = NULL, int iMatchPos = 0) {

	TTreeNodeShape *ANode = ATree->Items->AddChild(AParent, sText);

	TInsightTextShape *AInsighNode = dynamic_cast<TInsightTextShape*>(ANode);
	if (!AInsighNode)
		throw Exception(L"Program Logic Error! Can not cast to <TInsightTextShape>!");

	AInsighNode->SetMask(AType, sSearchText, AItemObject, iMatchPos, true);

	return ANode;
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::EditInsightChange(TObject *Sender) {
	TTreeNodeShape *ASelectedShape = NULL;

	try {
		TreeViewInsight->BeginUpdate();
		TreeViewInsight->Clear();
		ImageList1->Clear();

		const UnicodeString sText = GetSearchText();
		const UnicodeString sTextUpper = sText.UpperCase();

		/* Property Inspector */
		TLMDPropertyInspector *APropInspector = FStateMachineEditor ?
			FStateMachineEditor->PropSettingsInspector : FormScxmlGui->PropSettingsInspector;
		if (APropInspector && APropInspector->Items->Count) {
			const UnicodeString sLevelName = "Propety Inspector";
			TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, NULL, indtLevel, sLevelName, sTextUpper, NULL,
				sLevelName.UpperCase().Pos(sTextUpper)

				);

			for (int i = 0; i < APropInspector->Items->Count; i++) {
				DoAddPropInspItem(ALevelNode, APropInspector->Items->Items[i]);
			}

		}

		if (FStateMachineEditor) {
			const UnicodeString sUnitFileName = FStateMachineEditor->UnitFileName;
			UnicodeString sEditorLevelName = "Chart: [" + (sUnitFileName.IsEmpty() ? FStateMachineEditor->Caption : sUnitFileName) + "]";

			TTreeNodeShape *AEditorLevelNode = AddTreeChildObjectData(TreeViewInsight, NULL, indtLevel, sEditorLevelName, sTextUpper, NULL,
				sEditorLevelName.UpperCase().Pos(sTextUpper));

			/* Tree Popups */
			const TPoint APtTreeClient = FStateMachineEditor->TheTree->ScreenToClient(FCursorPoint);
			if (TPopupMenu * APopupMenu = FStateMachineEditor->GetPreparedPopupMenu(FStateMachineEditor->TheTree, APtTreeClient.x,
					APtTreeClient.y, FCursorPoint.x, FCursorPoint.y)) {
				if (APopupMenu->OnPopup) {
					APopupMenu->OnPopup(APopupMenu);
				}

				UnicodeString sLevelName = L"Menu Shape";
				if (APopupMenu->Name.Pos("Point")) {
					sLevelName = L"Menu Point";
				}
				else if (APopupMenu->Name.Pos("Conn")) {
					sLevelName = L"Menu Connection";
				}
				else if (APopupMenu->Name.Pos("Dummy")) {
					sLevelName = L"Menu Connection Text";
				}

				TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, AEditorLevelNode, indtLevel, sLevelName, sTextUpper,
					NULL, sLevelName.UpperCase().Pos(sTextUpper));

				DoAddMenuItems(ALevelNode, APopupMenu->Items, APopupMenu->Images);
			}

			/* TreeActions */ {
				UnicodeString sLevelName = L"Actions";
				TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, AEditorLevelNode, indtLevel, sLevelName, sTextUpper);

				DoAddActionItems(ALevelNode, FStateMachineEditor->ActionListEditor);

				DoAddMenuItems(ALevelNode, FStateMachineEditor->Tools1, FStateMachineEditor->ImageList1);
			}

			/* Tree View */ {
				UnicodeString sLevelName = L"View";
				TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, AEditorLevelNode, indtLevel, sLevelName, sTextUpper,
					NULL, sLevelName.UpperCase().Pos(sTextUpper));

				DoAddMenuItems(ALevelNode, FStateMachineEditor->View1, FStateMachineEditor->ImageList1);
			}
		}

		/* Main Menu */ {
			const UnicodeString sLevelName = "Main Menu";
			TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, NULL, indtLevel, sLevelName, sTextUpper, NULL,
				sLevelName.UpperCase().Pos(sTextUpper));

			DoAddMenuItems(ALevelNode, FormScxmlGui->MainMenu1->Items, FormScxmlGui->MainMenu1->Images);
		}

		/* Project View */ {
			UnicodeString sProjectViewName = L"";
			if (FormScxmlGui->ProjectManagerView1->Selected) {
				sProjectViewName = L": [" + FormScxmlGui->ProjectManagerView1->Selected->DisplayName + "]";
			}

			const UnicodeString sLevelName = "Project Menu" + sProjectViewName;
			TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, NULL, indtLevel, sLevelName, sTextUpper, NULL,
				sLevelName.UpperCase().Pos(sTextUpper));

			if (FormScxmlGui->PopupProjView->OnPopup) {
				FormScxmlGui->PopupProjView->OnPopup(FormScxmlGui->PopupProjView);
			}

			DoAddMenuItems(ALevelNode, FormScxmlGui->PopupProjView->Items, FormScxmlGui->PopupProjView->Images);
		}

		/* Application Settings */ {
			const UnicodeString sLevelName = "Application Settings";
			TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, NULL, indtLevel, sLevelName, sTextUpper, NULL,
				sLevelName.UpperCase().Pos(sTextUpper));

			DoAddPersistentItems(ALevelNode, SettingsData, indtAppSettings);
		}

		/* Project Options */
		if (TStateMachineProject * AProject = FormScxmlGui->StateMachineProject) {
			const UnicodeString sLevelName = "Project Options: [" + AProject->DisplayName + "]";
			TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, NULL, indtLevel, sLevelName, sTextUpper, NULL,
				sLevelName.UpperCase().Pos(sTextUpper));

			DoAddPersistentItems(ALevelNode, AProject->ProjectProperties, indtProjectOptions);
		}

		/* Call Stack */ {
			const UnicodeString sLevelName = "Call Stack";
			TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, NULL, indtLevel, sLevelName, sTextUpper, NULL,
				sLevelName.UpperCase().Pos(sTextUpper));

			if (FormScxmlGui->PopupMenuStack->OnPopup) {
				FormScxmlGui->PopupMenuStack->OnPopup(FormScxmlGui->PopupMenuStack);
			}

			DoAddMenuItems(ALevelNode, FormScxmlGui->PopupMenuStack->Items, FormScxmlGui->PopupMenuStack->Images);
		}

		/* GIF Settings */
		if (FrameGifMaker) {
			const UnicodeString sLevelName = "GIF Maker Settings";
			TTreeNodeShape *ALevelNode = AddTreeChildObjectData(TreeViewInsight, NULL, indtLevel, sLevelName, sTextUpper, NULL,
				sLevelName.UpperCase().Pos(sTextUpper));

			DoAddPersistentItems(ALevelNode, FrameGifMaker->FSettings, indtGifMaker);
		}

		// узел, в котором совпадение найдено с первой позиции
		TTreeNodeShape *ANodeToSelectMatchFirst = NULL;
		// узел, в котором совпадение найдено в средине
		TTreeNodeShape *ANodeToSelect = NULL;

		// если пустой текст, тогда покажем всё, возможно пользователю нужен весь список
		if (!sText.IsEmpty()) {
			// удалим узлы, в которых нет данных
			for (int i = TreeViewInsight->Shapes->Count - 1; i >= 0; i--) {
				TTreeNodeShape *ANode = TreeViewInsight->Shapes->Items[i];

				const int iNodeMatchPos = InsightNodeMatchPos(ANode);

				// условие удаления:
				// 1) нет дочерних узлов с действиями
				// 2) узел дочерний, но нет совпадения
				// 3) у родителя нет совпадений (такой вариант удобен, например, при меню Add, чтобы посмотреть список возможных дочерных вариантов
				if (!ANode->Count() && !iNodeMatchPos && !InsightNodeMatchPos(ANode->Parent)) {
					delete ANode;
					continue;
				}

				if (iNodeMatchPos) {
					TInsightTextShape *AInsightNode = dynamic_cast<TInsightTextShape*>(ANode);
					if (!AInsightNode)
						throw Exception(L"Program Logic Error! Can not cast to <TInsightTextShape>!");

					if (AInsightNode->Type != indtLevel) {
						// запоминаем то, что найдено по 1 символу
						if (iNodeMatchPos == 1) {
							ANodeToSelectMatchFirst = ANode;
						}
						else {
							// иначе то, что найдено в средине
							ANodeToSelect = ANode;
						}
					}

				}

				// используем только этот метод Expand, иначе будет подергиваться при прокрутке
				ANode->Expanded = true;
			}
		}

		// выделяем в порядке приоритета, сначала то, что найдено с 1 позиции
		if (ANodeToSelectMatchFirst) {
			TreeViewInsight->Selected->Clear();
			ASelectedShape = ANodeToSelectMatchFirst;
		}
		else if (ANodeToSelect) {
			TreeViewInsight->Selected->Clear();
			ASelectedShape = ANodeToSelect;
		}
	}
	__finally {
		TreeViewInsight->EndUpdate();
	}

	if (ASelectedShape) {
		/* Otherwise node can be not inside viewport */
		TreeViewInsight->Refresh();

		ASelectedShape->Selected = true;
	}

}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::DoExecuteNode(TTreeNodeShape *ANode) {

	FDoNotRestorePropertyInspector = false;

	// разрешаем только нодам, которые не являются узлами
	if (ANode) {

		TInsightTextShape *AInsightNode = dynamic_cast<TInsightTextShape*>(ANode);
		if (!AInsightNode)
			throw Exception(L"Program Logic Error! Can not cast to <TInsightTextShape>!");

		switch(AInsightNode->Type) {
		case indtAppSettings: {
				// передаем в составной путь в формате "PropName.SubPropName"
				FormScxmlGui->DoEditSettings(GetContainedPropName(ANode, AInsightNode->Type));

				this->ModalResult = mrOk;
			}break;
		case indtProjectOptions: {
				// передаем в составной путь в формате "PropName.SubPropName"
				FormScxmlGui->DoEditProjectOptions(GetContainedPropName(ANode, AInsightNode->Type));

				this->ModalResult = mrOk;
			}break;
		case indtGifMaker: {
				// передаем в составной путь в формате "PropName.SubPropName"
				if (FrameGifMaker) {
					FrameGifMaker->DoEditSettings(GetContainedPropName(ANode, AInsightNode->Type));
				}

				this->ModalResult = mrOk;
			}break;
		case indtMenuItem: {
				TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(AInsightNode->ItemObject);
				// для менюшек только конечные элементы
				if (AMenuItem && !ANode->Count()) {

					if (AMenuItem->AutoCheck) {
						AMenuItem->Checked = !AMenuItem->Checked;
					}

					if (AMenuItem->Action) {

						if (AMenuItem->Action->OnUpdate) {
							AMenuItem->Action->OnUpdate(AMenuItem->Action);
						}

						TFileAction * AFileAction = dynamic_cast<TFileAction*>(AMenuItem->Action);
						if (AFileAction && AFileAction->Enabled && AFileAction->BeforeExecute) {
							AFileAction->BeforeExecute(AFileAction);
						}

						AMenuItem->Action->Execute();

					}
					else if (AMenuItem->OnClick) {
						AMenuItem->OnClick(AMenuItem);
					}

					this->ModalResult = mrOk;
				}
			}break;
		case indtAction: {
				TCustomAction * ACustomAction = dynamic_cast<TCustomAction*>(AInsightNode->ItemObject);
				if (ACustomAction) {

					if (ACustomAction->AutoCheck) {
						ACustomAction->Checked = !ACustomAction->Checked;
					}

					ACustomAction->Execute();

					this->ModalResult = mrOk;
				}
			}break;
		case indtPropInspector: {
				TLMDPropertyInspectorItem* AItem = dynamic_cast<TLMDPropertyInspectorItem*>(AInsightNode->ItemObject);
				if (AItem) {

					if (AItem->CanEdit()) {
						AItem->Edit();
					}
					else {
						AItem->Expand();
						AItem->Active = true;

						FormScxmlGui->ForceShowPropInspPanel();
					}

					FDoNotRestorePropertyInspector = true;

					this->ModalResult = mrOk;
				}
			}break;
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::DoExecuteSelectedNode(void) {
	this->DoExecuteNode(GetSafeTreeListFirst(TreeViewInsight->Selected->Shapes));
}

//---------------------------------------------------------------------------

void __fastcall TDialogInsight::FormShow(TObject *Sender) {
	// позиция курсора нужна будет для PopupMenu
	GetCursorPos(&FCursorPoint);

	if (FStateMachineEditor) {
		FStateMachineEditor->SaveInspectorState();
	}

	EditInsightChange(EditInsight);
	try {
		EditInsight->SelectAll();
		EditInsight->SetFocus();
	}
	catch(...) {
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::EditInsightKeyPress(TObject *Sender, wchar_t &Key) {
	try {
		// в KeyDown не работают
		switch(Key) {
		case VK_RETURN:
			Key = 0;
			DoExecuteSelectedNode();
			break;
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::FormClose(TObject *Sender, TCloseAction &Action) {
	// редактировалось какое-то значение из инспектора,
	// поэтому не восстанавливаем и сохраняем выделение на нем
	if (!FDoNotRestorePropertyInspector) {
		if (FStateMachineEditor) {
			FStateMachineEditor->RestoreInspectorStateDelayed();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::EditInsightKeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
	if (Key == VK_UP || Key == VK_DOWN) {
		PostMessage(TreeViewInsight->Handle, WM_KEYDOWN, Key, MapVirtualKey(Key, MAPVK_VK_TO_VSC));
		PostMessage(TreeViewInsight->Handle, WM_KEYUP, Key, MapVirtualKey(Key, MAPVK_VK_TO_VSC));
		Key = 0;
	}
}

//---------------------------------------------------------------------------
int __fastcall TDialogInsight::Execute(TStateMachineEditor *AStateMachineEditor) {
	FStateMachineEditor = AStateMachineEditor;

	return this->ShowModal();
}

//---------------------------------------------------------------------------
void __fastcall TDialogInsight::TreeViewInsightDblClickShape(TTreeNodeShape *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	this->DoExecuteNode(Sender);
}
//---------------------------------------------------------------------------
