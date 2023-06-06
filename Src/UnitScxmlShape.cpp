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

#include "UnitScxmlShape.h"

#include "UnitDataModelStateShape.h"
#include "UnitChildScxmlBaseShapes.h"
#include "UnitStateMachineUnit.h"
#include "VISUAL_GLOBALS.h"
#include "UnitSettings.h"
#include "UnitLineStringEditor.h"
#include "UnitPropInspectorExtensions.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

bool FindChildInitial(TVisualScxmlBaseShape *AShape, const UnicodeString &sInitial) {
	for (int i = 0; i < AShape->Children->Count; i++) {
		TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape->Children->Items[i]);
		if (AVisualScxmlBaseShape) {
			if ((AVisualScxmlBaseShape->SimpleText == sInitial) ||
				// рекурсивно поиск по вложенным
				FindChildInitial(AVisualScxmlBaseShape, sInitial)) {
				return true;
			}
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TScxmlShape::TScxmlShape(Classes::TComponent* AOwner) : TVisualScxmlBaseShape(AOwner), FDatamodel(L""),
FBinding(TScxmlBinding::early), FInvokeID(L""), FTempInvokeID(L"") {

	FAvailableTypes.insert(sctDatamodel);
	FAvailableTypes.insert(sctScript);
	FAvailableTypes.insert(sctState);
	FAvailableTypes.insert(sctParallel);
	FAvailableTypes.insert(sctFinal);
	FAvailableTypes.insert(sctVirtual);

	FStateAttrMap["ScxmlName"] = false;
	FStateAttrMap["Initial"] = false;
	FStateAttrMap["Xmlns"] = true;
	FStateAttrMap["Version"] = true;
	FStateAttrMap["Datamodel"] = false;
	FStateAttrMap["Binding"] = false;

	this->SetBounds(TRect(X0, Y0, X0 + 500, Y0 + 500));
}

// ---------------------------------------------------------------------------
__fastcall TScxmlShape::~TScxmlShape() {

}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlShape::GetInitial(void) {
	// проверка, является ли валидным указатель на начальный элемент
	if (!FInitial.IsEmpty()) {
		// если классический Initial с указанием только одного элемента
		if (!FInitial.Pos(" ")) {
			// если есть хоть в одном из дочерних
			if (FindChildInitial(this, FInitial)) {
				return FInitial;
			}

			WLOG_WARNING(L"Initial element [%s] is not a valid child element of [%s]!", FInitial.c_str(), this->SimpleText.c_str());

			FInitial = L""; // не знаю, правильно или нет
		}
		else {
			// иначе просто возвращаем список без всяких проверок
			return FInitial;
		}

	}
	// в этом случае невозвращаем начальный элемент, но пока сохраняем FInitial
	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {

	TVisualScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);

	if (APropInfo) {
		const UnicodeString sPropName(APropInfo->Name());
		if (sPropName == L"Datamodel") {
			AEditorClass = __classid(TScxmlDatamodelPropEditor);
		}
		else if (sPropName == L"InvokeID") {
			AEditorClass = __classid(TScxmlInvokeIDPropEditor);
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {

	const UnicodeString sInfo = TVisualScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == "Datamodel") {
		return //
		"<B>The datamodel that this document requires.</B>\n" //
		"<B>null</B> - Null datamodel\n" //
		"<B>luavia</B> - model with AVIA protocol support(UscxmlTester). 'AviaExtensionEnabled' option must be set in app settings\n" //
		"<B>lua</B> - standard lua model(UscxmlTester)\n" //
		"<B>ecmascript</B> - ECMAScript data model, as described in SCXML Specification - B.2 The ECMAScript Data Model(QtScxmlTester)";
	}

	if (sPropName == "Initial") {
		return //
		"The id of the initial state(s) for the document.\n" //
		"If not specified, the default initial state is the first child state in document order.\n" //
		;
	}

	if (sPropName == "ScxmlName") {
		return //
		"The name of this state machine.\n" //
		"It is for purely informational purposes.\n" //
		;
	}

	if (sPropName == "InvokeID") {
		return //
		"Identifier for debug multiple invoked parallel machines.\n" //
		"If 'InvokeID' is set, only messages with same intepreter id will come.\n" //
		"<b>NOTICE:</b> 'InvokeID' may be stored or temporary.\n" //
		"You can select it in dialog property editor!\n" //
		;
	}

	if (sPropName == "Binding") {
		return //
		"The data binding to use.\n" //
		"<B>early</B> - the SCXML Processor must create all data elements and assign their initial values <B>at document initialization time.</B>\n"
		//
		"<B>late</B> - the SCXML Processor must create the data elements at document initialization time, " //
		"but must assign the specified initial value to a given data element only " //
		"when the state that contains it is entered for the first time, before any &lt;onentry&gt; markup." //
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::SetDefaultSize() {
	TVisualScxmlBaseShape::SetDefaultSize();

	this->SetBounds(TRect(X0, Y0, X0 + 500, Y0 + 500));
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlShape::IsCluster(void) {
	return true;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TVisualScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TScxmlShape * AScxmlShape = dynamic_cast<TScxmlShape*>(Source);
			if (AScxmlShape) {
				FDatamodel = AScxmlShape->FDatamodel;
				FInvokeID = AScxmlShape->FInvokeID;
				FBinding = AScxmlShape->FBinding;
				FVersion = AScxmlShape->FVersion;

				UpdateView();
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
TColor __fastcall TScxmlShape::GetHeadColor(void) {
	return(this->StateMachineEditorUnit && this->StateMachineEditorUnit->IsVirtual)
		? SettingsData->ThemeSettings->VirtualNormalColor : SettingsData->ThemeSettings->ScxmlNormalHeadColor;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlShape::GetXmlns(void) {
	return L"http://www.w3.org/2005/07/scxml";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlShape::GetVersion(void) {
	return FVersion.IsEmpty() ? L"1.0" : FVersion.c_str();
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlShape::IsVersionStored(bool) {
	return GetVersion() != L"1.0";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlShape::XMLAttrNameToPropName(UnicodeString sAttrName) {
	sAttrName = TVisualScxmlBaseShape::XMLAttrNameToPropName(sAttrName);
	return sAttrName == L"Name" ? L"ScxmlName" : sAttrName.c_str();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlShape::PropNameToXMLAttrName(UnicodeString sPropName) {
	sPropName = TVisualScxmlBaseShape::PropNameToXMLAttrName(sPropName);
	return sPropName == L"scxmlname" ? L"name" : sPropName.c_str();
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::SaveScxmlNameToProject() {
	if (this->Tree) {
		TStateMachineEditor *AEditor = dynamic_cast<TStateMachineEditor*>(this->Tree->Owner);
		if (AEditor && AEditor->StateMachineEditorUnit) {
			if (AEditor->StateMachineEditorUnit->ScxmlNameFromXML != this->SimpleText) {
				AEditor->StateMachineEditorUnit->ScxmlName = this->SimpleText;
				// пометим, что необходимо перезаписать настройки проекта
				// иначе без этого не будет вызываться метод DoWrite
				if (AEditor->StateMachineEditorUnit->ProjMgr && AEditor->StateMachineEditorUnit->StateMachineProject) {
					if (!AEditor->StateMachineEditorUnit->StateMachineProject->IsModified(false)) {
						AEditor->StateMachineEditorUnit->StateMachineProject->MarkModified();
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::SetStateId(UnicodeString sVal) {
	TVisualScxmlBaseShape::SetStateId(sVal);

	SaveScxmlNameToProject();
}

// ---------------------------------------------------------------------------
TColor __fastcall TScxmlShape::GetClusterColor(void) {
	return IsDebugTracingMode() ? clInfoBk : TVisualScxmlBaseShape::GetClusterColor();
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::DrawShapeCanvas(Tecanvas::TCanvas3D * ACanvas, const Types::TRect & R) {
	TVisualScxmlBaseShape::DrawShapeCanvas(ACanvas, R);

	std::auto_ptr<TFont>AFont(new TFont());
	ACanvas->Font->Assign(AFont.get());

	const bool bIsDatamodelEmpty = this->Datamodel.IsEmpty();
	if (!bIsDatamodelEmpty) {
		ACanvas->BackMode = cbmTransparent;
		ACanvas->TextAlign = TA_RIGHT;
		ACanvas->Font->Color = SettingsData->ThemeSettings->ScxmlDatamodelFontColor;
		ACanvas->TextOut3D(this->X1 - 3, this->Y0 + 3, TeeTreeZ, this->Datamodel);
	}

	const UnicodeString sTargetInvokeID = this->InvokeID;
	if (!sTargetInvokeID.IsEmpty()) {

		ACanvas->BackMode = cbmTransparent;
		ACanvas->TextAlign = this->ChildrenAlignX == scaxLeft ? TA_RIGHT : TA_LEFT;
		ACanvas->Font->Color = this->IsInvokeIdStored() ? clNavy : clDkGray;
		ACanvas->Font->Style = ACanvas->Font->Style << fsBold;

		ACanvas->TextOut3D(this->ChildrenAlignX == scaxLeft ? (this->X1 - 3) : (this->X0 + 3) //
			, this->Y0 + 3 + GetClusterHeight(), TeeTreeZ, L"InvokeID:[" + sTargetInvokeID + "]");
	}

	if (this->StateMachineEditorUnit) {
		if (this->StateMachineEditorUnit->IsVirtual) {
			ACanvas->BackMode = cbmTransparent;
			ACanvas->TextAlign = TA_LEFT;
			ACanvas->Font->Color = SettingsData->ThemeSettings->ScxmlVirtualFontColor;
			ACanvas->Font->Style = TFontStyles();
			ACanvas->TextOut3D(R.Left + 3, R.Top + 3, TeeTreeZ, L"VIRTUAL");
		}
		else if (this->StateMachineEditorUnit->IsInherited) {
			ACanvas->BackMode = cbmTransparent;
			ACanvas->TextAlign = TA_LEFT;
			ACanvas->Font->Color = SettingsData->ThemeSettings->ScxmlVirtualFontColor;
			ACanvas->TextOut3D(R.Left + 3, R.Top + 3, TeeTreeZ, L"Inherited: "+this->StateMachineEditorUnit->Inherited);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::SetDatamodel(UnicodeString val) {
	FDatamodel = val;

	UpdateView();
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::SetInvokeID(UnicodeString val) {
	if (FInvokeID != val) {

		FInvokeID = val;

		UpdateView();

		if (!this->ComponentState.Contains(csReading) && !this->ComponentState.Contains(csLoading)) {
			SyncByInvokeID();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::SyncByInvokeID(void) {
	try {
		if (SettingsData->IsTesterWorking) {
			const UnicodeString sFinalInvokeID = GetFinalInvokeID();
			if (!sFinalInvokeID.IsEmpty()) {
				SettingsData->SendEvent("SYNC_INVOKED@" + sFinalInvokeID);
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"Set Sync Invoked> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlShape::SetTempInvokeID(UnicodeString val) {
	if (FTempInvokeID != val) {
		FTempInvokeID = val;

		UpdateView();
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlShape::GetSyntaxScheme(void) {
	if (SameText(FDatamodel, L"luavia")) {
		return L"Lua";
	}
	else if (FDatamodel.LowerCase().Pos(L"cplusplus")) {
		return L"CPP";
	}

	return FDatamodel;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlShape::GetFinalInvokeID(void) {
	if (!FInvokeID.IsEmpty()) {
		return FInvokeID;
	}

	return FTempInvokeID;
}

// ---------------------------------------------------------------------------
// -----------------------TScxmlDatamodelPropEditor-------------
// ---------------------------------------------------------------------------
Lmdinspropinsp::TLMDPropAttrs __fastcall TScxmlDatamodelPropEditor::GetAttrs(void) {
	return TLMDStringPropEditor::GetAttrs() << praValueList;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlDatamodelPropEditor::GetValues(Classes::TStrings* AValues) {
	AValues->Add("lua");
	AValues->Add("luavia");
	AValues->Add("ecmascript");
}

// ---------------------------------------------------------------------------
// --------------------------  TScxmlInvokeIDPropEditor ---------------------------
// ---------------------------------------------------------------------------

TLMDPropAttrs __fastcall TScxmlInvokeIDPropEditor::GetAttrs() {
	return TLMDPropAttrs() << praReadOnly << praDialog;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TScxmlInvokeIDPropEditor::GetValue(void) {
	const UnicodeString sVal = TLMDStringPropEditor::GetValue();
	if (sVal.IsEmpty()) {
		return L"{ Not set }";
	}

	return sVal;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlInvokeIDPropEditor::Edit(void) {

	const UnicodeString sValue = this->GetStrValue(0);

	try {

		TScxmlShape *AScxmlShape = dynamic_cast<TScxmlShape*>(this->GetInstance(0));
		if (!AScxmlShape)
			throw Exception("Can not cast Instance to <TScxmlShape>!");

		std::auto_ptr<TLineStringEditor>ALineStringEditorPtr(new TLineStringEditor(Application));
		ALineStringEditorPtr->Caption = "LineStringEditor [" + PropName + "]";
		ALineStringEditorPtr->Edit1->Text = sValue;

		TCheckBox *ACheckBox = new TCheckBox(ALineStringEditorPtr.get());
		ACheckBox->Caption = L"Temporary";
		ACheckBox->Parent = ALineStringEditorPtr->sef_Panel;
		ACheckBox->Align = alLeft;
		ACheckBox->AlignWithMargins = true;

		ACheckBox->Checked = (!AScxmlShape->IsInvokeIdStored() && !AScxmlShape->TempInvokeID.IsEmpty()) || //
		(!AScxmlShape->IsInvokeIdStored() && AScxmlShape->TempInvokeID.IsEmpty() && SettingsData->IsTesterWorking);

		// открываем в любом случае, а пишем только, если Writable
		const int iModalResult = ALineStringEditorPtr->ShowModal();
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			if (iModalResult == mrOk) {
				UnicodeString sInvokeID = ALineStringEditorPtr->Edit1->Text.Trim();

				if (ACheckBox->Checked) {
					AScxmlShape->TempInvokeID = sInvokeID;
					sInvokeID = L"";
				}
				else {
					AScxmlShape->TempInvokeID = L"";
				}

				if (sInvokeID != AScxmlShape->StoredInvokeID) {
					this->SetStrValue(sInvokeID);
				}
				else {
					AScxmlShape->SyncByInvokeID();

					if (this->PropInsp) {
						this->PropInsp->UpdateContent();
					}
				}
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
