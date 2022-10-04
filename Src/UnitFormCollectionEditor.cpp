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

#include "UnitFormCollectionEditor.h"

#include <memory>

#include <Clipbrd.hpp>
#include <IOUtils.hpp>
#include "XMLSerialize/UnitXMLSerializeLoad.h"
#include "XMLSerialize/UnitXMLSerializeSave.h"
#include "Log4cpp_VCL.hpp"

#include "UnitRegisterDefaultEditors.h"
#include "UnitSettings.h"
#include "UnitPropInspectorExtensions.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDDsgObjects"
#pragma link "LMDDsgPropInsp"
#pragma link "LMDInsPropInsp"
#pragma link "LMDInsPropPage"
#pragma resource "*.dfm"

const Typinfo::TTypeKinds g_StringTypeKinds = Typinfo::TTypeKinds() << tkString << tkLString << tkWString << tkUString;

// ---------------------------------------------------------------------------
void __fastcall TOwnedCollectionPropEditor::Edit(void) {
	TObject *AObject = this->GetObjectValue(0);
	if (AObject) {
		TOwnedCollection *ACollection = dynamic_cast<TOwnedCollection*>(AObject);
		if (ACollection) {
			std::auto_ptr<TFormCollectionEditor>AFormCollectionEditor(DoCreateCollectionEditor(ACollection));

			if (AFormCollectionEditor->ShowModal() == mrOk) {

				ACollection->Assign(AFormCollectionEditor->OwnedCollection);
				this->SetObjectValue(AFormCollectionEditor->OwnedCollection);

			}
		}
	}
}

// ---------------------------------------------------------------------------
TFormCollectionEditor *__fastcall TOwnedCollectionPropEditor::DoCreateCollectionEditor(TOwnedCollection *ACollection) {
	return new TFormCollectionEditor(Application, ACollection);
}

// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TOwnedCollectionPropEditor::GetAttrs() {
	return TLMDPropAttrs() << praDialog << praReadOnly;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TOwnedCollectionPropEditor::GetValue(void) {

	if (this->AllEqual()) {
		TObject *AObject = this->GetObjectValue(0);
		if (AObject) {
			TOwnedCollection *ACollection = dynamic_cast<TOwnedCollection*>(AObject);

			if (ACollection) {
				return ACollection->Count == 1 ? UnicodeString(L"1 item") : (UnicodeString(ACollection->Count) + L" items");
			}
		}
	}

	return "";
}

// ---------------------------------------------------------------------------
__fastcall TFormCollectionEditor::TFormCollectionEditor(TComponent* Owner, TOwnedCollection *ASourceCollection) : TForm(Owner),
FOwnedCollection(0), m_b_modified(false), FSourceCollection(ASourceCollection), FPropStageObject(0) {

}

// ---------------------------------------------------------------------------
__fastcall TFormCollectionEditor::~TFormCollectionEditor() {
	if (FOwnedCollection) {
		delete FOwnedCollection;
		FOwnedCollection = 0;
	}
	// этот объект создается методом SaveState,
	// по-этому он может быть и не удален
	if (FPropStageObject) {
		delete FPropStageObject;
		FPropStageObject = 0;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::FormCreate(TObject *Sender) {

	CloneSourceCollection();

	assert(FOwnedCollection != NULL);

	PropInspectorCollection->Selection->Add(FOwnedCollection);

	FillListBox();

	RegisterDefaultPropertyEditors(PropertyInspector1);
	RegisterDefaultPropertyEditors(PropInspectorCollection);

	OnLanguageChanged();
}

// ---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::SetModified(bool bVal) {
	m_b_modified = bVal;
	this->Caption = (m_b_modified ? L"*" : L"") + FFormCaption;
}

// ---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::CloneSourceCollection() {
	assert(FSourceCollection != NULL);
	FOwnedCollection = new TOwnedCollection(this, FSourceCollection->ItemClass);
	FOwnedCollection->Assign(FSourceCollection);
}

// ---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::FillListBox() {
	ListBox1->Clear();

	for (int i = 0; i < FOwnedCollection->Count; i++) {
		ListBox1->AddItem(FOwnedCollection->Items[i]->DisplayName, FOwnedCollection->Items[i]);
	}

	PropInspectorCollection->UpdateContent();
}

// ---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::ListBox1Click(TObject *Sender) {

	FPropStageObject = PropertyInspector1->SaveState();

	LMDDesignObjects1->Clear();
	for (int i = 0; i < ListBox1->Count; i++) {
		if (ListBox1->Selected[i]) {
			TObject *AObject = ListBox1->Items->Objects[i];
			if (AObject) {
				TPersistent * APersistent = dynamic_cast<TPersistent*>(AObject);
				if (APersistent) {
					LMDDesignObjects1->Add(APersistent);
				}
			}
		}
	}
	UpdateDisplayName();

	if (SpBtnAutoExpand->Down) {
		SpBtnExpand->Click();
	}
	else {
		PropertyInspector1->RestoreState(FPropStageObject, true);
		// обнуляем указатель, так как функция RestoreState удалит объект
		FPropStageObject = 0;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::UpdateDisplayName() {
	for (int i = 0; i < ListBox1->Count; i++) {

		TObject *AObject = ListBox1->Items->Objects[i];
		if (AObject) {
			TCollectionItem * AItem = dynamic_cast<TCollectionItem*>(AObject);
			if (AItem) {
				const bool bSelected = ListBox1->Selected[i];
				if (ListBox1->Items->Strings[i] != AItem->DisplayName) {
					ListBox1->Items->Strings[i] = AItem->DisplayName;
					// какого-то хера сбрасывается выделение, поэтому восстанавливаем выделение
					if (bSelected != ListBox1->Selected[i]) {
						ListBox1->Selected[i] = bSelected;
					}
				}
			}
		}

	}
	PropInspectorCollection->UpdateContent();
}

// ---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::PropertyInspector1Change(TObject *Sender) {
	UpdateDisplayName();
	SetModified(true);
}

// ---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::FormCloseQuery(TObject *Sender, bool &CanClose) {
	if (m_b_modified) {
		String Caption = "WARNING!";
		String Text = "Settings are modified! Are you sure to quit?";
		CanClose = Application->MessageBoxA(Text.w_str(), Caption.w_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::PropInspectorCollectionChange(TObject *Sender) {
	SetModified(true);
}
// ---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::BtnOkClick(TObject *Sender) {
	SetModified(false);
}

// ---------------------------------------------------------------------------
void TFormCollectionEditor::OnLanguageChanged() {
	this->Caption = this->Caption;
	FFormCaption = this->Caption;

	BtnAdd->Caption = "";

	BtnDelete->Caption = "";

	BtnUp->Caption = "";

	BtnDown->Caption = "";

	BtnCopy->Caption = "";

	BtnPaste->Caption = "";

	BtnLoad->Caption = "";

	BtnSave->Caption = "";

}

// ---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::ListBox1DrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State) {
	TListBox *AListBox = dynamic_cast<TListBox*>(Control);
	if (AListBox) {
		bool bEnabled = true;
		if (Index != -1) {
			if (AListBox->Items->Objects[Index]) {
				if (IsPublishedProp(AListBox->Items->Objects[Index], "Enabled")) {
					bEnabled = GetPropValue(AListBox->Items->Objects[Index], "Enabled");
				}
			}
		}

		// Выделена?
		if (State.Contains(odSelected)) {
			AListBox->Canvas->Brush->Color = clHighlight;
			AListBox->Canvas->Font->Color = clHighlightText;
		}
		else // Нет
		{
			AListBox->Canvas->Brush->Color = clWindow;
			if (bEnabled) {
				AListBox->Canvas->Font->Color = clWindowText;
			}
			else {
				AListBox->Canvas->Font->Color = clGrayText;
			}

		}
		AListBox->Canvas->FillRect(Rect);

		if (Index != -1) {
			AListBox->Canvas->TextOut(Rect.Left, Rect.Top, AListBox->Items->Strings[Index]);
		}

		if (State.Contains(odFocused)) {
			AListBox->Canvas->Brush->Color = AListBox->Color;
			AListBox->Canvas->DrawFocusRect(Rect);
		}
	}

}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::ActionAddExecute(TObject *Sender) {
	TCollectionItem *AItem = FOwnedCollection->Add();
	ListBox1->AddItem(AItem->DisplayName, AItem);
	ListBox1->ClearSelection();
	ListBox1->Selected[ListBox1->Count - 1] = true;
	ListBox1Click(ListBox1);

	PropInspectorCollection->UpdateContent();
}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::ActionDeleteExecute(TObject *Sender) {
	for (int i = ListBox1->Count - 1; i >= 0; i--) {
		if (ListBox1->Selected[i]) {
			TObject *AObject = ListBox1->Items->Objects[i];
			if (AObject) {
				TPersistent * APersistent = dynamic_cast<TPersistent*>(AObject);
				if (APersistent) {
					const int iIndex = LMDDesignObjects1->IndexOf(APersistent);
					if (iIndex != -1) {
						LMDDesignObjects1->Delete(iIndex);
					}
				}
				TCollectionItem * ACollectionItem = dynamic_cast<TCollectionItem*>(AObject);
				if (AObject) {
					FOwnedCollection->Delete(ACollectionItem->Index);
					SetModified(true);
				}
			}
			ListBox1->Items->Delete(i);
		}
	}
	UpdateDisplayName();
}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::ActionUpExecute(TObject *Sender) {
	for (int i = 0; i < ListBox1->Count; i++) {

		if (ListBox1->Selected[i]) {

			TObject *AObject = ListBox1->Items->Objects[i];
			if (AObject) {
				TCollectionItem * AItem = dynamic_cast<TCollectionItem*>(AObject);
				if (AItem) {

					if (AItem->Index > 0) {
						AItem->Index = AItem->Index - 1;
						ListBox1->Items->Exchange(i, i - 1);

						ListBox1->Selected[i - 1] = true;
						SetModified(true);
					}
					else
						break;

				}
			}
		}

	}
}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::ActionDownExecute(TObject *Sender) {
	for (int i = ListBox1->Count - 1; i >= 0; i--) {
		// анализ последнего элемента
		if (i == (ListBox1->Count - 1)) {
			// если не выделен, то есть куда передвигать, пропускаем этот шаг, иначе выходим
			if (ListBox1->Selected[i])
				break;
			else
				continue;
		}

		if (ListBox1->Selected[i]) {
			TObject *AObject = ListBox1->Items->Objects[i];
			if (AObject) {

				TCollectionItem * AItem = dynamic_cast<TCollectionItem*>(AObject);
				if (AItem) {

					AItem->Index = AItem->Index + 1;
					ListBox1->Items->Exchange(i, i + 1);

					ListBox1->Selected[i + 1] = true;
					SetModified(true);

				}
			}
		}

	}
}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::ActionCopyExecute(TObject *Sender) {
	try {
		std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(this));
		AXMLDocument->Active = true;
		_di_IXMLNode ANode = AXMLDocument->AddChild("root");
		for (int i = 0; i < ListBox1->Count; i++) {
			if (ListBox1->Selected[i]) {
				TObject *AObject = ListBox1->Items->Objects[i];
				if (AObject) {
					_di_IXMLNode ACollectionItemNode = ANode->AddChild(XML_COLLECTION_ITEM_NAME);
					Xmlserialize::XMLSerializeObject(AObject, ACollectionItemNode);
				}
			}
		}
		Clipboard()->Open();
		Clipboard()->SetTextBuf(AXMLDocument->XML->Text.c_str());
		Clipboard()->Close();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall OnErrorGetCallback(const UnicodeString &sText) {
	WLOG_ERROR(sText.c_str());
}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::ActionPasteExecute(TObject *Sender) {
	const int iPrevCount = ListBox1->Count;
	try {
		UnicodeString sText = "";
		Clipboard()->Open();
		if (Clipboard()->HasFormat(CF_TEXT)) {
			sText = Clipboard()->AsText;
		}
		Clipboard()->Close();

		if (!sText.IsEmpty()) {

			SetModified(true);

			std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(this));
			AXMLDocument->XML->Text = sText;
			AXMLDocument->Active = true;

			if (AXMLDocument->DocumentElement) {
				for (int i = 0; i < AXMLDocument->DocumentElement->ChildNodes->Count; i++) {
					_di_IXMLNode AChildNode = AXMLDocument->DocumentElement->ChildNodes->Nodes[i];
					Xmlserialize::XMLDeserializeProperty(AChildNode, FOwnedCollection, OnErrorGetCallback);
				}
			}
		}
		else
			throw Exception("Clipboard does not contain text format!");
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	// пересоздадим листбокс с новыми элементами
	FillListBox();

	// теперь выделим новые элементы
	const int iDiff = ListBox1->Count - iPrevCount;
	if (iDiff > 0) {
		for (int i = iPrevCount; i < ListBox1->Count; i++) {
			ListBox1->Selected[i] = true;
		}
	}
	ListBox1Click(ListBox1);
}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::FileOpen1Accept(TObject *Sender) {
	try {
		if (FileExists(FileOpen1->Dialog->FileName)) {

			std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(this));
			AXMLDocument->XML->LoadFromFile(FileOpen1->Dialog->FileName);
			AXMLDocument->Active = true;

			SetModified(true);

			FOwnedCollection->Clear();
			ListBox1->Clear();
			LMDDesignObjects1->Clear();

			if (AXMLDocument->DocumentElement) {
				for (int i = 0; i < AXMLDocument->DocumentElement->ChildNodes->Count; i++) {
					_di_IXMLNode AChildNode = AXMLDocument->DocumentElement->ChildNodes->Nodes[i];
					Xmlserialize::XMLDeserializeProperty(AChildNode, FOwnedCollection, OnErrorGetCallback);
				}
			}
		}
		else
			throw Exception("File <" + FileOpen1->Dialog->FileName + "> does not exist!");
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	// пересоздадим листбокс с новыми элементами
	FillListBox();
}

//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::FileSaveAs1Accept(TObject *Sender) {
	try {
		std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(this));
		AXMLDocument->Active = true;
		_di_IXMLNode ANode = AXMLDocument->AddChild("root");
		for (int i = 0; i < ListBox1->Count; i++) {
			TObject *AObject = ListBox1->Items->Objects[i];
			if (AObject) {
				_di_IXMLNode ACollectionItemNode = ANode->AddChild(XML_COLLECTION_ITEM_NAME);
				Xmlserialize::XMLSerializeObject(AObject, ACollectionItemNode);
			}
		}
		AXMLDocument->XML->SaveToFile(FileSaveAs1->Dialog->FileName);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::FileOpen1BeforeExecute(TObject *Sender) {
	//	try {
	//		TScenaryOwnedCollection * AScenaryOwnedCollection = dynamic_cast<TScenaryOwnedCollection*>(FOwnedCollection);
	//		if (AScenaryOwnedCollection) {
	//			if (!DirectoryExists(AScenaryOwnedCollection->ScenaryDir->Path)) {
	//				if (!ForceDirectories(AScenaryOwnedCollection->ScenaryDir->Path))
	//					throw Exception("Can not create directory <" + AScenaryOwnedCollection->ScenaryDir->Path + ">");
	//			}
	//			FileOpen1->Dialog->InitialDir = AScenaryOwnedCollection->ScenaryDir->Path;
	//		}
	//	}
	//	catch(Exception * E) {
	//		SendLogError(LOG_ERROR_MSG);
	//	}

}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::FileSaveAs1BeforeExecute(TObject *Sender) {
	//	try {
	//		TScenaryOwnedCollection * AScenaryOwnedCollection = dynamic_cast<TScenaryOwnedCollection*>(FOwnedCollection);
	//		if (AScenaryOwnedCollection) {
	//			if (!DirectoryExists(AScenaryOwnedCollection->ScenaryDir->Path)) {
	//				if (!ForceDirectories(AScenaryOwnedCollection->ScenaryDir->Path))
	//					throw Exception("Can not create directory <" + AScenaryOwnedCollection->ScenaryDir->Path + ">");
	//			}
	//			FileSaveAs1->Dialog->InitialDir = AScenaryOwnedCollection->ScenaryDir->Path;
	//		}
	//	}
	//	catch(Exception * E) {
	//		SendLogError(LOG_ERROR_MSG);
	//	}
}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::OnPresetItemClick(TObject *ASender) {
	//	try {
	//		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(ASender);
	//		if (AMenuItem) {
	//			TScenaryOwnedCollection * AScenaryOwnedCollection = dynamic_cast<TScenaryOwnedCollection*>(FOwnedCollection);
	//			if (AScenaryOwnedCollection) {
	//				if (DirectoryExists(AScenaryOwnedCollection->ScenaryDir->Path)) {
	//					const UnicodeString sFullPath = TPath::Combine(AScenaryOwnedCollection->ScenaryDir->Path,
	//						StripHotkey(AMenuItem->Caption) + ".xml");
	//
	//					FileOpen1->Dialog->FileName = sFullPath;
	//					FileOpen1Accept(FileOpen1);
	//				}
	//			}
	//		}
	//	}
	//	catch(Exception * E) {
	//		LOG_ERROR(LOG_ERROR_MSG);
	//	}
}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::PopupMenu1Popup(TObject *Sender) {
	try {
		MenuPresets->Clear();

		//		TScenaryOwnedCollection * AScenaryOwnedCollection = dynamic_cast<TScenaryOwnedCollection*>(FOwnedCollection);
		//		if (AScenaryOwnedCollection) {
		//			if (DirectoryExists(AScenaryOwnedCollection->ScenaryDir->Path)) {
		//				TStringDynArray ADynArray = TDirectory::GetFiles(AScenaryOwnedCollection->ScenaryDir->Path, "*.xml");
		//				for (int i = 0; i < ADynArray.Length; i++) {
		//					TMenuItem *AItem = new TMenuItem(this);
		//					AItem->Caption = TPath::GetFileNameWithoutExtension(ADynArray[i]);
		//					AItem->OnClick = &OnPresetItemClick;
		//
		//					MenuPresets->Add(AItem);
		//				}
		//			}
		//
		//		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::SpBtnCollapseClick(TObject *Sender) {
	if (PropertyInspector1->ArrangeKind == akByCategory) {
		PropertyInspector1->CollapseAllCategories();
	}
	else {
		for (int i = 0; i < PropertyInspector1->Items->Count; i++) {
			PropertyInspector1->Items->Items[i]->Collapse();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::SpBtnExpandClick(TObject *Sender) {
	if (PropertyInspector1->ArrangeKind == akByCategory) {
		PropertyInspector1->ExpandAllCategories();
	}
	else {
		for (int i = 0; i < PropertyInspector1->Items->Count; i++) {
			PropertyInspector1->Items->Items[i]->Expand();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::BtnArrangeTypeClick(TObject *Sender) {
	PropertyInspector1->ArrangeKind = BtnArrangeType->Down ? akByCategory : akByName;
}

//---------------------------------------------------------------------------
void FillListWithProperties(TLMDPropertyInspectorItem* AItem, TStringList *AList) {
	do {
		AList->Insert(0, AItem->PropName);

		AItem = AItem->Parent;
	}
	while (AItem);
}

//---------------------------------------------------------------------------
UnicodeString GetPropertyPath(TLMDPropertyInspectorItem* AItem) {
	std::auto_ptr<TStringList>APropertyListPtr(new TStringList());
	APropertyListPtr->Delimiter = L'.';
	APropertyListPtr->StrictDelimiter = true;

	FillListWithProperties(AItem, APropertyListPtr.get());

	return APropertyListPtr->DelimitedText;
}

//---------------------------------------------------------------------------
bool TFormCollectionEditor::IsActiveItemWritableString(void) {
	return PropertyInspector1->ActiveItem && PropertyInspector1->ActiveItem->PropInfo[0]->IsWritable() && g_StringTypeKinds.Contains
		(PropertyInspector1->ActiveItem->PropInfo[0]->PropType()->Kind);
}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::OnStringPropReplace(TObject *ASender) {
	try {
		if (IsActiveItemWritableString()) {
			std::auto_ptr<TStringList>APropertyListPtr(new TStringList());
			FillListWithProperties(PropertyInspector1->ActiveItem, APropertyListPtr.get());
			// если есть хотя бы одно свойство
			if (APropertyListPtr->Count) {
				for (int i = 0; i < ListBox1->Count; i++) {
					if (ListBox1->Selected[i]) {
						if (ListBox1->Items->Objects[i]) {
							TCollectionItem * AItem = dynamic_cast<TCollectionItem*>(ListBox1->Items->Objects[i]);
							if (AItem) {
								TObject *AObject = AItem;
								for (int k = 0; k < APropertyListPtr->Count - 1; k++) {
									AObject = GetObjectProp(AObject, APropertyListPtr->Strings[k]);
									if (!AObject)
										throw Exception("Can not get Object from path [" + APropertyListPtr->Strings[k] + "]");
								}
								const Typinfo::TTypeKind AKind = PropType(AObject, APropertyListPtr->Strings[APropertyListPtr->Count - 1]);
								if (g_StringTypeKinds.Contains(AKind)) {

									UnicodeString sText = GetStrProp(AObject, APropertyListPtr->Strings[APropertyListPtr->Count - 1]);
									TReplaceFlags AReplaceFlags;
									if (ReplaceDialog1->Options.Contains(frReplaceAll)) {
										AReplaceFlags = AReplaceFlags << rfReplaceAll;
									}
									if (!ReplaceDialog1->Options.Contains(frMatchCase)) {
										AReplaceFlags = AReplaceFlags << rfIgnoreCase;
									}
									sText = StringReplace(sText, ReplaceDialog1->FindText, ReplaceDialog1->ReplaceText, AReplaceFlags);
									SetStrProp(AObject, APropertyListPtr->Strings[APropertyListPtr->Count - 1], sText);

									SetModified(true);
									UpdateDisplayName();

									PropertyInspector1->UpdateContent();
								}
								else
									throw Exception("Class <" + AObject->ClassName() + "> property <" + APropertyListPtr->Strings
									[APropertyListPtr->Count - 1] + "> is not of type <string>");
							}
						}

					}
				}

			}

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::ActionReplaceValuesExecute(TObject *Sender) {
	if (IsActiveItemWritableString()) {
		ReplaceDialog1->OnReplace = &OnStringPropReplace;
		ReplaceDialog1->Execute(this->Handle);
	}
	else {
		LOG_ERROR("Can not replace string value in non-writable or non-string properties!");
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormCollectionEditor::PropertyInspector1FilterProp(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo,
	bool AIsSubProp, bool &AIncludeProp) {
	Propinspext::PropSettingsInspectorFilterProp(Sender, AInstance, APropInfo, AIsSubProp, AIncludeProp);
}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::PropertyInspector1GetCaptionColor(TObject *Sender, TLMDPropertyInspectorItem *AItem, TColor &AColor)
{
	Propinspext::PropSettingsInspectorGetCaptionColor(Sender, AItem, AColor);
}
//---------------------------------------------------------------------------

void __fastcall TFormCollectionEditor::PropertyInspector1GetEditorClass(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass)

{
	Propinspext::PropSettingsInspectorGetEditorClass(Sender, AInstance, APropInfo, AEditorClass);
}
//---------------------------------------------------------------------------
