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

#include "UnitProtocolBindingImportForm.h"

#include "Log4cpp_VCL.hpp"
#include <StrUtils.hpp>
#include "UnitSettings.h"
#include "UnitTreeLoader.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDSedDocument"
#pragma link "LMDSedView"
#pragma resource "*.dfm"
TBindingsImporter *BindingsImporter = 0;

// ---------------------------------------------------------------------------
// не поддерживает билдеровский парсер XML DocType
void SkipDocType(TStrings *AStrings) {
	for (int i = 0; i < AStrings->Count; i++) {
		if (AStrings->Strings[i].Trim().Pos("<!DOCTYPE")) {
			AStrings->Delete(i);
			break;
		}
	}
}

TColor ControlTypeToColor(const UnicodeString &sType) {
	TColor AColor = clBlack;
	if (SameText(sType, "Input"))
		AColor = clBlue;
	else if (SameText(sType, "Output"))
		AColor = clRed;
	return AColor;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TBindingImportingShape::TBindingImportingShape(TComponent* Owner) : TTreeNodeShape(Owner) {
	FAviaName = "";
	FSignal = 0;
	FType = "";
	FParam = "";
	FOffset = 0;
	FMin = 0;
	FMax = 0;
	FLength = 0;
	FHwMin = 0;
	FHwMax = 0;
	FSbit = 0;
	FCalibration = "";
	FChannel = "";

	Fnumber = 0;
	Fdiscription = "";
}

// ---------------------------------------------------------------------------
void __fastcall TBindingImportingShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TTreeNodeShape *AShape = dynamic_cast<TTreeNodeShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TTreeNodeShape::Assign(AShape);
			// ********************************************* //

			TBindingImportingShape * AImportShape = dynamic_cast<TBindingImportingShape*>(Source);
			if (AImportShape) {
				FAviaName = AImportShape->FAviaName;
				FSignal = AImportShape->FSignal;
				FType = AImportShape->FType;
				FParam = AImportShape->FParam;
				FOffset = AImportShape->FOffset;
				FMin = AImportShape->FMin;
				FMax = AImportShape->FMax;
				FLength = AImportShape->FLength;
				FHwMin = AImportShape->FHwMin;
				FHwMax = AImportShape->FHwMax;
				FSbit = AImportShape->FSbit;
				FCalibration = AImportShape->FCalibration;
				FChannel = AImportShape->FChannel;

				Fnumber = AImportShape->Fnumber;
				Fdiscription = AImportShape->Fdiscription;

			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TBindingImportingShape::AssignFromCalibratorElement(Calibrator::IXMLcontrolType*element) {
	if (FAviaName.IsEmpty()) {
		FAviaName = element->AVIAName;
	}
	Fnumber = element->number;
	Fdiscription = element->discription;
}

// ---------------------------------------------------------------------------
void __fastcall TBindingImportingShape::AssignFromCanserviceElement(Canservice::IXMLitemType *element) {
	if (FAviaName.IsEmpty()) {
		FAviaName = element->Name;
	}
	FSignal = element->Signal;
	FType = element->Type;
	if (element->HasAttribute("Datatype")) {
		FParam = element->GetAttribute("Datatype");
	}
	else {
		FParam = element->Param;
	}
	if (element->HasAttribute("Channel")) {
		FChannel = element->GetAttribute("Channel");
	}
	FOffset = element->Offset;
	FMin = element->Min;
	FMax = element->Max;
	FLength = element->Length;
	FHwMin = element->HwMin;
	FHwMax = element->HwMax;
	FSbit = element->Sbit;
	FCalibration = element->Calibration;
}

// ---------------------------------------------------------------------------
void __fastcall TBindingImportingShape::AssignFromModelElement(Model::IXMLitemType *element) {
	FAviaName = element->Name;
	FType = element->Type;
	FParam = element->Datatype.IsEmpty() ? element->Param : element->Datatype;
	FChannel = element->Channel;
	Fdiscription = element->Description;

	FMin = StrToFloatDef(element->Min, 0.0f);
	FMax = StrToFloatDef(element->Max, 0.0f);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TBindingsImporter::TBindingsImporter(TComponent* Owner) : TForm(Owner) {
	FTreeBase = new TTree(Panel1);

	FDevConfig = 0;
	FCanserviceConfig = 0;
	FModelConfig = 0;

	Tree1->NoOwnerShapes = false;

	Classes::RegisterClassA(__classid(TBindingImportingShape));
}

// ---------------------------------------------------------------------------
__fastcall TBindingsImporter::~TBindingsImporter() {
	Edit1->OnChange = NULL;
}

// ---------------------------------------------------------------------------
Canservice::IXMLitemType *TBindingsImporter::FindCanserviceControl(const UnicodeString &sName) {
	if (FCanserviceConfig && FCanserviceConfig->container) {
		for (int i = 0; i < FCanserviceConfig->container->Count; i++) {
			if (SameText(sName, FCanserviceConfig->container->item[i]->Name)) {
				return FCanserviceConfig->container->item[i];
			}
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TBindingsImporter::BtnValidateModelClick(TObject *Sender) {
	SettingsData->ProfilerReset(__FUNCTION__);
	Screen->Cursor = crHourGlass;

	Tree1->Clear();
	FTreeBase->Clear();

	Tree1->BeginUpdate();

	try {
		// сначала парсим конфиг FCalibrator
		{
			SkipDocType(EditDocModel->Lines);

			Xmlintf::_di_IXMLDocument ADoc = LoadXMLData(EditDocModel->Lines->Text);
			if (!ADoc)
				throw Exception("XMLDocument in Model config is empty!");

			FModelConfig = Model::GetConfig(ADoc);
			if (!FModelConfig)
				throw Exception("<Config> in Model config is empty!");
		}

		for (int i = 0; i < FModelConfig->Count; i++) {

			TTreeNodeShape *APanelShape = Tree1->Add(FModelConfig->container[i]->NAME);
			APanelShape->ImageIndex = tiUnChecked;
			APanelShape->Font->Style = TFontStyles() << fsBold;
			APanelShape->Font->Color = clMaroon;

			for (int k = 0; k < FModelConfig->container[i]->item->Count; k++) {
				Model::_di_IXMLitemType AItem = FModelConfig->container[i]->item->Items[k];
				if (AItem->Name.IsEmpty())
					continue;

				if (SameText(AItem->Type, "Input") && CheckInput->Checked || SameText(AItem->Type, "Output") && CheckOutput->Checked) {

					Tree1->GlobalFormat.NodeClass = __classid(TBindingImportingShape);

					TBindingImportingShape *AControlShape = dynamic_cast<TBindingImportingShape*>(Tree1->Add(AItem->Name, APanelShape));
					if (!AControlShape)
						throw Exception("Can not cast to <TBindingImportingShape>!");

					AControlShape->AssignFromModelElement(AItem);

					AControlShape->ImageIndex = tiUnChecked;
					AControlShape->Font->Color = ControlTypeToColor(AControlShape->Type);

					Tree1->GlobalFormat.NodeClass = __classid(TTreeNodeShape);

				}
			}
		}

		std::auto_ptr<TMemoryStream>AInStream(new TMemoryStream());

		SaveTreeToStream(Tree1, AInStream.get());
		AInStream->Position = 0;

		LoadTreeFromStreamEx(FTreeBase, AInStream.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	Tree1->EndUpdate();

	Screen->Cursor = crDefault;

	SettingsData->ProfileOperation(__FUNCTION__);
}

// ---------------------------------------------------------------------------
void TBindingsImporter::ValidateBothCanFiles() {
	SettingsData->ProfilerReset(__FUNCTION__);

	// сначала парсим конфиг FCalibrator
	{
		SkipDocType(EditDocCalibrator->Lines);
		Xmlintf::_di_IXMLDocument ADoc = LoadXMLData(EditDocCalibrator->Lines->Text);
		if (!ADoc)
			throw Exception("XMLDocument in FCalibrator config is empty!");

		FDevConfig = Calibrator::GetdevConfig(ADoc);
		if (!FDevConfig)
			throw Exception("<DevConfig> in FCalibrator config is empty!");
	}

	// парсим конфиг Canservice
	{
		SkipDocType(EditDocCanservice->Lines);
		Xmlintf::_di_IXMLDocument ADoc = LoadXMLData(EditDocCanservice->Lines->Text);
		if (!ADoc)
			throw Exception("XMLDocument in FCalibrator config is empty!");

		FCanserviceConfig = Canservice::GetConfig(ADoc);
		if (!FCanserviceConfig)
			throw Exception("<DevConfig> in FCalibrator config is empty!");
	}

	for (int i = 0; i < FDevConfig->panel->Count; i++) {
		TTreeNodeShape *APanelShape = Tree1->Add(UnicodeString().sprintf(L"%s [%s] [%d]", FDevConfig->panel->Items[i]->name.c_str(),
				FDevConfig->panel->Items[i]->discription.c_str(), FDevConfig->panel->Items[i]->number));
		APanelShape->ImageIndex = tiUnChecked;
		APanelShape->Font->Style = TFontStyles() << fsBold;
		APanelShape->Font->Color = clMaroon;

		for (int k = 0; k < FDevConfig->panel->Items[i]->Count; k++) {
			Calibrator::_di_IXMLdeviceType device = FDevConfig->panel->Items[i]->device[k];

			TTreeNodeShape *ADeviceShape = Tree1->Add(UnicodeString().sprintf(L"%s [%s] [%d]", device->name.c_str(),
					device->discription.c_str(), device->number), APanelShape);
			ADeviceShape->ImageIndex = tiUnChecked;
			ADeviceShape->Font->Style = TFontStyles() << fsBold;

			for (int n = 0; n < device->Count; n++) {

				Calibrator::_di_IXMLcontrolType control = device->control[n];

				Tree1->GlobalFormat.NodeClass = __classid(TBindingImportingShape);

				TBindingImportingShape *AControlShape = dynamic_cast<TBindingImportingShape*>
					(Tree1->Add(UnicodeString().sprintf(L"%s [%s]", control->AVIAName.c_str(), control->discription.c_str()),
						ADeviceShape));
				if (!AControlShape)
					throw Exception("Can not cast to <TBindingImportingShape>!");

				AControlShape->AssignFromCalibratorElement(control);

				// пытаемся найти соответствующий кановский контрол
				Canservice::IXMLitemType *ACanserviceElement = FindCanserviceControl(control->AVIAName);
				if (!ACanserviceElement)
					WLOG_WARNING(L"Can not find corresponding Canservice control [%s]! Full XML[%s]", control->AVIAName.c_str(),
					control->XML.c_str());
				else {
					AControlShape->AssignFromCanserviceElement(ACanserviceElement);
					if (ACanserviceElement->ParentNode) {
						// убираем уже найденные для увеличения производительности
						ACanserviceElement->ParentNode->ChildNodes->Remove(ACanserviceElement);
					}
				}

				AControlShape->ImageIndex = tiUnChecked;
				AControlShape->Font->Color = ControlTypeToColor(AControlShape->Type);

				Tree1->GlobalFormat.NodeClass = __classid(TTreeNodeShape);

				// если определен тип
				if (!AControlShape->Type.IsEmpty()) {
					if ((SameText(AControlShape->Type, "Input") && CheckInput->Checked || SameText(AControlShape->Type,
								"Output") && CheckOutput->Checked) == false) {
						delete AControlShape;
					}
				}
			}
		}
	}
	SettingsData->ProfileOperation(__FUNCTION__);
}

// ---------------------------------------------------------------------------
void TBindingsImporter::ValidateCanservice() {
	SettingsData->ProfilerReset(__FUNCTION__);

	// парсим конфиг Canservice
	SkipDocType(EditDocCanservice->Lines);

	Xmlintf::_di_IXMLDocument ADoc = LoadXMLData(EditDocCanservice->Lines->Text);
	if (!ADoc)
		throw Exception("XMLDocument in FCalibrator config is empty!");

	FCanserviceConfig = Canservice::GetConfig(ADoc);
	if (!FCanserviceConfig)
		throw Exception("<DevConfig> in FCalibrator config is empty!");

	TTreeNodeShape *APanelShape = Tree1->Add(FCanserviceConfig->container->NAME);
	APanelShape->ImageIndex = tiUnChecked;
	APanelShape->Font->Style = TFontStyles() << fsBold;
	APanelShape->Font->Color = clMaroon;

	for (int i = 0; i < FCanserviceConfig->container->Count; i++) {

		Canservice::IXMLitemType *ACanserviceElement = FCanserviceConfig->container->item[i];

		if (SameText(ACanserviceElement->Type, "Input") && CheckInput->Checked || SameText(ACanserviceElement->Type,
				"Output") && CheckOutput->Checked) {

			Tree1->GlobalFormat.NodeClass = __classid(TBindingImportingShape);

			TBindingImportingShape *AControlShape = dynamic_cast<TBindingImportingShape*>(Tree1->Add(ACanserviceElement->Name,
					APanelShape));
			if (!AControlShape)
				throw Exception("Can not cast to <TBindingImportingShape>!");

			AControlShape->AssignFromCanserviceElement(ACanserviceElement);

			AControlShape->ImageIndex = tiUnChecked;
			AControlShape->Font->Color = ControlTypeToColor(AControlShape->Type);

			Tree1->GlobalFormat.NodeClass = __classid(TTreeNodeShape);
		}
	}

	APanelShape->Expanded = true;

	SettingsData->ProfileOperation(__FUNCTION__);
}

// ---------------------------------------------------------------------------
void __fastcall TBindingsImporter::BtnValidateCanserviceClick(TObject *Sender) {
	Screen->Cursor = crHourGlass;

	Tree1->Clear();
	FTreeBase->Clear();

	Tree1->BeginUpdate();

	try {
		// в калибраторном файле есть описания, но нужен кановский для типов сигналов
		if (!EditDocCalibrator->Lines->Text.Trim().IsEmpty()) {
			ValidateBothCanFiles();
		}
		else {
			// если только кановский нужен
			ValidateCanservice();
		}

		std::auto_ptr<TMemoryStream>AInStream(new TMemoryStream());

		SaveTreeToStream(Tree1, AInStream.get());
		AInStream->Position = 0;

		LoadTreeFromStreamEx(FTreeBase, AInStream.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	Tree1->EndUpdate();

	Screen->Cursor = crDefault;
}

// ---------------------------------------------------------------------------
void RecursiveSetImageIndex(TTreeNodeShape *AShape, TTreeNodeImageIndex AImageIndex) {
	AShape->ImageIndex = AImageIndex;

	for (int i = 0; i < AShape->Children->Count; i++) {
		RecursiveSetImageIndex(AShape->Children->Items[i], AImageIndex);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TBindingsImporter::MenuCheckClick(TObject *Sender) {

	for (int i = 0; i < Tree1->Selected->Count(); i++) {
		RecursiveSetImageIndex(Tree1->Selected->Items[i], tiChecked);
	}

}

// ---------------------------------------------------------------------------
void __fastcall TBindingsImporter::MenuUncheckClick(TObject *Sender) {
	for (int i = 0; i < Tree1->Selected->Count(); i++) {
		RecursiveSetImageIndex(Tree1->Selected->Items[i], tiUnChecked);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TBindingsImporter::TabControl1Change(TObject *Sender) {
	switch(TabControl1->TabIndex) {
	case 0:
		EditView->Document = EditDocCalibrator;
		break;
	case 1:
		EditView->Document = EditDocCanservice;
		break;
	case 2:
		EditView->Document = EditDocModel;
		break;
	}

}

// ---------------------------------------------------------------------------
bool IsAnyChildVisible(TTreeNodeShape *AShape) {
	for (int i = 0; i < AShape->Count(); i++) {
		if (AShape->Children->Items[i]->Visible)
			return true;

		if (IsAnyChildVisible(AShape->Children->Items[i]))
			return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
bool RecursiveSetVisibility(TNodeShapeList * AShapeList, const UnicodeString &sEditText) {
	bool bIsAnyVisible = false;
	for (int i = 0; i < AShapeList->Count; i++) {
		TTreeNodeShape *AChildShape = AShapeList->Items[i];

		AChildShape->Visible = sEditText.IsEmpty() || ContainsText(AChildShape->Text->Text, sEditText);

		if (RecursiveSetVisibility(AChildShape->Children, sEditText)) {
			AChildShape->Visible = true;
			bIsAnyVisible = true;
		}

		if (AChildShape->Visible) {
			bIsAnyVisible = true;

			if (AChildShape->Parent) {
				AChildShape->Parent->Visible = true;
			}
		}
	}
	return bIsAnyVisible;
}

// ---------------------------------------------------------------------------
void RecursiveDelete(TCustomTree*ADestTree, TNodeShapeList * AShapeList) {
	for (int i = AShapeList->Count - 1; i >= 0; i--) {
		TTreeNodeShape *AChildShape = AShapeList->Items[i];

		if (!AChildShape->Visible) {
			ADestTree->DeleteShape(AChildShape);
		}
		else {
			RecursiveDelete(ADestTree, AChildShape->Children);
		}
	}
}

// ---------------------------------------------------------------------------
void RecursiveCopy(TCustomTree*ADestTree, TNodeShapeList *ASourceList, TTreeNodeShape *ATargetShape) {
	for (int i = 0; i < ASourceList->Count; i++) {
		if (ASourceList->Items[i]->Visible) {
			ADestTree->GlobalFormat.NodeClass = ASourceList->Items[i]->ClassType();

			TTreeNodeShape *ANewShape = ADestTree->Add(ASourceList->Items[i]->Text->Text, ATargetShape);
			ANewShape->Assign(ASourceList->Items[i]);

			ADestTree->GlobalFormat.NodeClass = __classid(TTreeNodeShape);

			RecursiveCopy(ADestTree, ASourceList->Items[i]->Children, ANewShape);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TBindingsImporter::OnPostEditChange(TMessage &msg) {
	// подстрахуемся, чтобы от левых приложений не словить сигнал
	if (msg.WParam != (int)this->Handle)
		return;

	static int iEditLength = 0;

	Tree1->BeginUpdate();

	// пользователь добавляет символы, значит уже ничего не добавится
	if (Edit1->Text.Length() > iEditLength) {
		RecursiveSetVisibility(Tree1->Roots, Edit1->Text);

		RecursiveDelete(Tree1, Tree1->Roots);

		// иногда бывает, что неправильно менеджер позицию рассчитывает,
		// а сворачивание-разворачивание поправляет
		Tree1->FullExpandCollapse(false);
	}
	else {
		Tree1->Clear();

		RecursiveSetVisibility(FTreeBase->Roots, Edit1->Text);

		RecursiveCopy(Tree1, FTreeBase->Roots, NULL);
	}

	Tree1->FullExpandCollapse(!Edit1->Text.IsEmpty());

	Tree1->EndUpdate();

	iEditLength = Edit1->Text.Length();
}

// ---------------------------------------------------------------------------
void __fastcall TBindingsImporter::Edit1Change(TObject *Sender) {
	// не приходит последняя буква, делаем с задержкой
	PostMessage(this->Handle, WM_SCXML_POST_EDIT_CHANGE, (int)this->Handle, 0);
}

// ---------------------------------------------------------------------------
void __fastcall TBindingsImporter::MenuCheckAllClick(TObject * Sender) {
	Tree1->BeginUpdate();
	for (int i = 0; i < Tree1->Shapes->Count; i++) {
		Tree1->Shapes->Items[i]->ImageIndex = tiChecked;
	}
	Tree1->EndUpdate();
}
// ---------------------------------------------------------------------------

void __fastcall TBindingsImporter::Uncheckall1Click(TObject * Sender) {
	Tree1->BeginUpdate();
	for (int i = 0; i < Tree1->Shapes->Count; i++) {
		Tree1->Shapes->Items[i]->ImageIndex = tiUnChecked;
	}
	Tree1->EndUpdate();
}

//---------------------------------------------------------------------------
