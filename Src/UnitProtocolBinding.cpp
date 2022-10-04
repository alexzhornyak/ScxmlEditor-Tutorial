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

#include "UnitProtocolBinding.h"

/* STL */
#include <memory>

/* Borland */
#include <PngImage.hpp>

/* https://github.com/badunius/myJSON */
/* MIT License */
#include "uJSON.hpp"

#include "UnitStateMachineConnection.h"
#include "UnitExternalCommunicationShapes.h"
#include "SaveSimplePersistent.h"
#include "UnitSettingsEditor.h"
#include "UnitSyntaxEditorForm.h"
#include "UnitProtocolBindingImportForm.h"
#include "UnitStateMachineUnit.h"
#include "UnitScxmlShape.h"
#include "UnitStringEditorEx.h"
#include "UnitLineStringEditor.h"
#include "UnitDataModelStateShape.h"
#include "UnitSettings.h"
#include "VectorDrawUtils.h"

// ---------------------------------------------------------------------------
void RegisterProtocolControlItems() {
#pragma startup RegisterProtocolControlItems

	Classes::RegisterClass(__classid(TProtocolControlBinding));
	Classes::RegisterClass(__classid(TProtocolBindingShape));
	Classes::RegisterClass(__classid(TProtocolWeakBindingShape));
	Classes::RegisterClass(__classid(TSetValueShape));
	Classes::RegisterClass(__classid(TSessionStateShape));
	Classes::RegisterClass(__classid(TVirtualFolderShape));
}

// ---------------------------------------------------------------------------
UnicodeString GetRootScxmlName(TCustomTree *ATree, const UnicodeString &sDefault) {
	if (ATree && ATree->Items->Count) {
		TScxmlShape *AScxmlShape = dynamic_cast<TScxmlShape*>(ATree->Items->First());
		if (AScxmlShape) {
			return AScxmlShape->SimpleText;
		}
	}
	return sDefault;
}

// ---------------------------------------------------------------------------
// -----------------------ProtocolControlBindingControlNameEditor-------------
// ---------------------------------------------------------------------------
Lmdinspropinsp::TLMDPropAttrs __fastcall ProtocolControlBindingControlNameEditor::GetAttrs(void) {
	return TLMDStringPropEditor::GetAttrs() << praValueList;
}

// ---------------------------------------------------------------------------
void __fastcall ProtocolControlBindingControlNameEditor::GetValues(Classes::TStrings* AValues) {
	AValues->Add("%_SERVER_SESSION_DUMMY_%");
}

// ---------------------------------------------------------------------------
// -----------------------ProtocolControlBindingCharsetEditor-------------
// ---------------------------------------------------------------------------
Lmdinspropinsp::TLMDPropAttrs __fastcall ProtocolControlBindingCharsetEditor::GetAttrs(void) {
	return TLMDStringPropEditor::GetAttrs() << praValueList;
}

// ---------------------------------------------------------------------------
void __fastcall ProtocolControlBindingCharsetEditor::GetValues(Classes::TStrings* AValues) {
	AValues->Add("UTF8");
	AValues->Add("CP1251");
	AValues->Add("CP866");
	AValues->Add("KOI8-R");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void __fastcall OnStringToPersistentError(const UnicodeString & sMsg) {
	WLOG_ERROR(L"%s", sMsg.c_str());
}

// ---------------------------------------------------------------------------
std::pair<TProtocolControlType, int>SplitProtocolControlType(const UnicodeString &sType) {
	TSysCharSet scsSep = TSysCharSet() << '[' << ']';
	TSysCharSet scsWhSp;
	std::auto_ptr<TStringList>AStringList(new TStringList());
	ExtractStrings(scsSep, scsWhSp, sType.c_str(), AStringList.get());
	TProtocolControlType AType = TProtocolControlType::pctInteger;
	int iArraySize = 0;

	UnicodeString sOutType = sType;
	if (AStringList->Count == 2) {
		sOutType = AStringList->Strings[0];
		iArraySize = AStringList->Strings[1].ToInt();
	}

	if (sOutType == "Logic") {
		AType = TProtocolControlType::pctLogic;
	}
	else if (sOutType == "Integer") {
		AType = iArraySize == 0 ? TProtocolControlType::pctInteger : TProtocolControlType::pctIntegerArray;
	}
	else if (sOutType == "Analog") {
		AType = iArraySize == 0 ? TProtocolControlType::pctDouble : TProtocolControlType::pctDoubleArray;
	}
	else if (sOutType == "Octet") {
		AType = TProtocolControlType::pctOctetArray;
	}
	else if (sOutType == "String") {
		AType = TProtocolControlType::pctString;
	}
	return std::make_pair(AType, iArraySize);
}

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// ----------------------- TProtocolControlBinding ---------------------------
// ---------------------------------------------------------------------------

__fastcall TProtocolControlBinding::TProtocolControlBinding(TObject *AParent) : FArraySize(0), FArrayElement(-1),
FProtocolControlType(pctInteger), FControlName(""), FActive(false), FBindTypes(1), FUseSessionServer(false), FSessionNodePath(""),
FSessionNodeAttribute(""), FParent(AParent), FScxmlName(""), FComment(""), FMin(""), FMax(""), FScxmlInvokerIDs(""), FCharset(""),
FStateMachineName(""), FDoNotSendToScxml(false), FChannelType(ctTSComm), FCustomAttributes(""), FStateMachineID("") {

}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolControlBinding::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	if (sPropName == L"ControlName") {
		return //
		"1) if simple control is used, just type its case insesitive name, for example <B>SG_AccBus1</B><BR>\n" //
		"2) if control is planning for binding to session, use format: <B>SERVICE_NAME.CONTROL_NAME</B><BR>\n" //
		"in program the part <B>[SERVICE_NAME]</B> will be replaced by actual name of service, for example: <B>TSSCXML_MODEL.SG_LOADALL</B>\n"
		//
		"3) if you want to use only output session control, for example: to change only attribute in session, use special name <B>%_SERVER_SESSION_DUMMY_%</B>"
		//
		;
	}
	if (sPropName == L"DoNotSendToScxml") {
		return //
		"If <B>DoNotSendToScxml==true</B> then event is only put to Global Data Storage, otherwise it sends to StateMachine also\n" //
		"<b>Warning!</b> Does not affect on 'ScxmlInvokerIDs'!";
	}
	if (sPropName == L"StateMachineName") {
		return //
		"Event will be enqueued to the StateMachine with the corresponding name.\n" //
		"For example: if <B>StateMachineName==ScxmlASP17</B>, event will be send only to the StateMachine with the name <B>ScxmlASP17</B>.\n"
		//
		"If <B>StateMachineName</B> is NOT matched than comparison will perform on <b>'StateMachineID'</b>!!!\n";
	}
	if (sPropName == L"StateMachineID") {
		return //
		"Event will be enqueued to the interpreter with the corresponding <B>InvokeID</B>.\n" //
		"<b>Warning!</b> Depends on 'DoNotSendToScxml', 'StateMachineName'\n" //
		"If <B>StateMachineName</B> is matched than <b>'StateMachineID' is not compared</b>!!!\n" //
		"If you want to compare only <b>'StateMachineID'</b>, set <b>'StateMachineName=*'</b>\n" //
		"If <B>StateMachineID</B> is empty, then comparing of IDs is skipped.\n" //
		"If <B>StateMachineID</B> is not empty, then this ID is compared with <B>InvokeID</B> and the event is sent only if IDs match each other";
	}
	if (sPropName == L"ScxmlInvokerIDs") {
		return //
		"List of InvokerIDs separated by <B>semicolon</B>\n" //
		"Used for such invokers as <B>AviaScxmlPlugin</B>, <B>AviaModelPlugin</B>\n" //
		"Does not depend on 'DoNotSendToScxml', 'StateMachineName', 'StateMachineID'\n" //
		"For example: 'ID_SUB1;ID_SUB2' etc.\n" //
		"Event will be sent directly to Invokers using method 'eventFromScxml'\n" //
		"If <B>ScxmlInvokerIDs</B> is empty, then comparing of IDs is skipped.\n" //
		"If <B>ScxmlInvokerIDs</B> is not empty, then this ID is compared with InvokeID of interpreter invokers and the event is sent only if IDs match each other";
	}
	if (sPropName == L"ChannelType") {
		return //
		"<B>ctTSComm</B> - event is defined for TSComm protocol\n" //
		"<B>ctZMQ</B> - event is defined for ZMQ protocol (DO NOT FORGET TO ENABLE ZMQ IN SCXMLMODEL)\n" //
		"<B>ctMulticast</B> - event is defined for Multicast protocol\n" //
		;
	}
	if (sPropName == L"ArraySize") {
		return //
		"<B>Size of array</B> - used in <B>pctIntegerArray, pctDoubleArray</B>\n" //
		;
	}
	if (sPropName == L"ArrayElement") {
		return //
		"<B>not used</B> - reserved for future extensions\n" //
		;
	}
	if (sPropName == L"BindTypes") {
		return //
		"<B>pbtConsumer</B> - for INPUTS\n" //
		"<B>pbtSupplier</B> - for OUTPUTS\n" //
		"<B>Must not be empty!!!</B>\n" //
		;
	}
	if (sPropName == L"CustomAttributes") {
		return //
		"You can add custom attributes in format <B>'key=value'</B>\n" //
		"For example: <B>Respond=1</B>\n" //
		"Every attribute's 'key=value' pair <B>must be in a new line</B>\n" //
		;
	}
	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {

	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();
		if (sPropName == L"ControlName") {
			AEditorClass = __classid(ProtocolControlBindingControlNameEditor);
		}
		else if (sPropName == L"Charset") {
			AEditorClass = __classid(ProtocolControlBindingCharsetEditor);
		}
		else if (sPropName == L"Comment") {
			AEditorClass = __classid(TUnicodeStringTextDialogEditor);
		}
		else if (sPropName == L"CustomAttributes") {
			AEditorClass = __classid(TStringTextPairsEditor);
		}
		else if (sPropName == L"ScxmlInvokerIDs") {
			AEditorClass = __classid(TLineEditStringPropEditor);
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolControlBinding::GetScxmlName() {
	if (!FScxmlName.IsEmpty())
		return FScxmlName;

	TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(FParent);
	if (AConnection) {
		return AConnection->Event;
	}
	else {

		{
			TSendShape *AShape = dynamic_cast<TSendShape*>(FParent);
			if (AShape && !AShape->Event.IsEmpty()) /* Только, если явно указано имя Евента, чтобы EventExpr не писался */ {
				return AShape->Event;
			}
		}

		{
			TSetValueShape *AShape = dynamic_cast<TSetValueShape*>(FParent);
			if (AShape && !AShape->Target.IsEmpty()) /* Только, если не пустой Target */ {
				return AShape->Target;
			}
		}
	}
	return L"";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolControlBinding::GetSessionServerType() {
	switch(ProtocolControlType) {
	case pctInteger:
	case pctIntegerArray:
		return "INTEGER";
	case pctDouble:
	case pctDoubleArray:
		return "ANALOG";
	case pctString:
		return "STRING";
	}
	return "STRING";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolControlBinding::GetSoftConsoleParam() {
	UnicodeString sOut = L"";
	switch(ProtocolControlType) {
	case pctLogic:
		sOut = "Logic";
		break;
	case pctInteger:
	case pctIntegerArray:
		sOut = "Integer";
		break;
	case pctDouble:
	case pctDoubleArray:
		sOut = "Analog";
		break;
	case pctString:
		sOut = "String";
		break;
	case pctOctetArray:
		sOut = "Octet";
		break;
	}

	if (ArraySize > 0) {
		sOut = sOut + "[" + UnicodeString(ArraySize) + "]";
	}

	if (this->UseSessionServer) {
		sOut = sOut + ", "+this->SessionNodePath + ", "+this->SessionNodeAttribute;
	}

	return sOut;
}

// ---------------------------------------------------------------------------
bool __fastcall TProtocolControlBinding::GetActive(void) {
	return FActive && !FControlName.IsEmpty();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::UpdateParentSimpleText(void) {
	TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(FParent);
	if (AConnection) {
		AConnection->UpdateTransitionText();
	}
	else {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(FParent);
		if (AShape) {
			AShape->UpdateSimpleText();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SetActive(bool bVal) {
	FActive = bVal;

	UpdateParentSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SetChannelType(TChannelType val) {
	FChannelType = val;

	UpdateParentSimpleText();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolControlBinding::GetControlName(void) {
	return FControlName.Trim() /* .UpperCase() */ ;
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SetControlName(UnicodeString sVal) {
	FControlName = sVal;

	UpdateParentSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SetScxmlName(UnicodeString sVal) {
	FScxmlName = sVal;

	UpdateParentSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SetScxmlInvokerIDs(UnicodeString val) {
	FScxmlInvokerIDs = val;

	UpdateParentSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SetStateMachineID(UnicodeString val) {
	FStateMachineID = val;

	UpdateParentSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SetDoNotSendToScxml(bool val) {
	FDoNotSendToScxml = val;

	UpdateParentSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SetStateMachineName(UnicodeString val) {
	FStateMachineName = val;

	UpdateParentSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::Assign(TPersistent* Source) {
	if (Source) {
		TProtocolControlBinding * AProtocolControlBinding = dynamic_cast<TProtocolControlBinding*>(Source);
		if (AProtocolControlBinding) {

			this->FArraySize = AProtocolControlBinding->FArraySize;
			this->FArrayElement = AProtocolControlBinding->FArrayElement;
			this->FProtocolControlType = AProtocolControlBinding->FProtocolControlType;
			this->FControlName = AProtocolControlBinding->FControlName;
			this->FActive = AProtocolControlBinding->FActive;
			this->FBindTypes = AProtocolControlBinding->FBindTypes;
			this->FUseSessionServer = AProtocolControlBinding->FUseSessionServer;
			this->FSessionNodePath = AProtocolControlBinding->FSessionNodePath;
			this->FSessionNodeAttribute = AProtocolControlBinding->FSessionNodeAttribute;
			this->FScxmlName = AProtocolControlBinding->FScxmlName;
			this->FComment = AProtocolControlBinding->FComment;
			this->FMin = AProtocolControlBinding->FMin;
			this->FMax = AProtocolControlBinding->FMax;
			this->FStateMachineID = AProtocolControlBinding->FStateMachineID;
			this->FScxmlInvokerIDs = AProtocolControlBinding->FScxmlInvokerIDs;
			this->FCharset = AProtocolControlBinding->FCharset;
			this->FStateMachineName = AProtocolControlBinding->FStateMachineName;
			this->FDoNotSendToScxml = AProtocolControlBinding->FDoNotSendToScxml;
			this->FChannelType = AProtocolControlBinding->FChannelType;
			this->FCustomAttributes = AProtocolControlBinding->FCustomAttributes;

			UpdateParentSimpleText();
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
const UnicodeString __fastcall TProtocolControlBinding::GetDisplayName(void) {
	return GetActive() ? ("[" + FControlName + "] size [" + UnicodeString(FArraySize) + "] elem [" + UnicodeString(FArrayElement) + "]")
		: UnicodeString(L"not active");
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::SaveXMLItem(ILMDXmlElement *ANodeContainer, const UnicodeString &sType) {
	_di_ILMDXmlElement ANodeItem = ANodeContainer->AppendElement("item");
	if (ANodeItem) {
		ANodeItem->SetAttr("Type", sType);
		ANodeItem->SetAttr("Name", this->ControlName);
		ANodeItem->SetAttr("Param", this->SoftConsoleParam);
		ANodeItem->SetAttr("ScxmlName", this->ScxmlName);

		ANodeItem->SetAttr("StateMachineName", this->StateMachineName);

		if (!this->Comment.IsEmpty()) {
			ANodeItem->SetAttr("Description", this->Comment);
		}
		if (!this->Min.IsEmpty()) {
			ANodeItem->SetAttr("Min", this->Min);
		}
		if (!this->Max.IsEmpty()) {
			ANodeItem->SetAttr("Max", this->Max);
		}
		if (!this->ScxmlInvokerIDs.IsEmpty()) {
			ANodeItem->SetAttr("ScxmlInvokerIDs", this->ScxmlInvokerIDs);
		}
		if (!this->StateMachineID.IsEmpty()) {
			ANodeItem->SetAttr("StateMachineID", this->StateMachineID);
		}
		if (!this->Charset.IsEmpty()) {
			ANodeItem->SetAttr("Charset", this->Charset);
		}
		if (this->DoNotSendToScxml) {
			ANodeItem->SetAttr("DoNotSendToScxml", "true");
		}
		/* Channel type */
		if (this->ChannelType == ctZMQ) {
			ANodeItem->SetAttr("Channel", "ZMQ");
		}
		else if (this->ChannelType == ctMulticast) {
			ANodeItem->SetAttr("Channel", "Multicast");
		}

		if (!this->CustomAttributes.IsEmpty()) {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->Text = this->CustomAttributes.Trim();
			for (int i = 0; i < AStringListPtr->Count; i++) {
				if (!AStringListPtr->Names[i].IsEmpty()) {
					ANodeItem->SetAttr(AStringListPtr->Names[i], AStringListPtr->ValueFromIndex[i]);
				}
			}
		}

		// запишем имя
		TComponent *AComponent = dynamic_cast<TComponent*>(FParent);
		if (AComponent) {
			ANodeItem->SetAttr("ShapeType", AComponent->ClassName());
			ANodeItem->SetAttr("ShapeName", AComponent->Name);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::ToXML(ILMDXmlElement *ANodeContainer) {
	// запишем имя
	UnicodeString sParentName = L"";
	TComponent *AComponent = dynamic_cast<TComponent*>(FParent);
	if (AComponent) {
		sParentName = AComponent->Name;
	}

	const UnicodeString sContainerName = ANodeContainer->GetAttr("NAME", "!!!UNKNOWN!!!");

	if (this->ControlName.IsEmpty())
		throw Exception(UnicodeString().sprintf(L"[ProtocolControlBinding->ControlName==NULL] Container[%s] Parent[%s]",
			sContainerName.c_str(), sParentName.c_str()));

	if (this->ScxmlName.IsEmpty())
		throw Exception(UnicodeString().sprintf(L"[ProtocolControlBinding->ScxmlName==NULL] Container[%s] ControlName[%s]",
			sContainerName.c_str(), this->ControlName.c_str()));

	/* Будет записано 2 узла */

	// 1) Input
	if (this->BindTypes.Contains(pbtConsumer)) {
		SaveXMLItem(ANodeContainer, "Input");
	}

	// 2) Output
	if (this->BindTypes.Contains(pbtSupplier)) {
		SaveXMLItem(ANodeContainer, "Output");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::OnCopyFromClipboardClick(TObject *Sender) {
	try {
		MUtils::StringToPersistent(this, Clipboard()->AsText, OnStringToPersistentError);

		TButton * AButton = dynamic_cast<TButton*>(Sender);
		if (AButton && Clipboard()->HasFormat(CF_TEXT)) {
			TFormSettingsEditor *AForm = dynamic_cast<TFormSettingsEditor*>(AButton->Owner);
			if (AForm) {
				AForm->PropSettingsInspector->UpdateContent();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::OnCopyToClipboardClick(TObject *Sender) {
	try {
		Clipboard()->AsText = MUtils::PersistentToString(this, true);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::ResetToDefaultClick(TObject *Sender) {
	try {
		std::auto_ptr<TProtocolControlBinding>AProtocolControlBindingPtr(new TProtocolControlBinding(NULL));

		this->Assign(AProtocolControlBindingPtr.get());

		TButton * AButton = dynamic_cast<TButton*>(Sender);
		if (AButton) {
			TFormSettingsEditor *AForm = dynamic_cast<TFormSettingsEditor*>(AButton->Owner);
			if (AForm) {
				AForm->PropSettingsInspector->UpdateContent();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
bool TProtocolControlBinding::EditBinding() {
	try {
		std::auto_ptr<TProtocolControlBinding>AProtocolControlBindingPtr(new TProtocolControlBinding(NULL));
		AProtocolControlBindingPtr->Assign(this);

		std::auto_ptr<TFormSettingsEditor>AFormSettingsEditorPtr(new TFormSettingsEditor(NULL, AProtocolControlBindingPtr.get()));

		{
			TButton *AButton = new TButton(AFormSettingsEditorPtr.get());
			AButton->Parent = AFormSettingsEditorPtr->Panel1;
			AButton->Align = alLeft;
			AButton->AlignWithMargins = true;
			AButton->Width = 145;
			AButton->Caption = "From clipboard";
			AButton->OnClick = AProtocolControlBindingPtr->OnCopyFromClipboardClick;
		}

		{
			TButton *AButton = new TButton(AFormSettingsEditorPtr.get());
			AButton->Parent = AFormSettingsEditorPtr->Panel1;
			AButton->Align = alLeft;
			AButton->AlignWithMargins = true;
			AButton->Width = 145;
			AButton->Caption = "To clipboard";
			AButton->OnClick = AProtocolControlBindingPtr->OnCopyToClipboardClick;
		}

		// иначе не видно кнопку "Сброс по умаолчанию"
		AFormSettingsEditorPtr->Width = AFormSettingsEditorPtr->Width + 145 * 2;

		AFormSettingsEditorPtr->BtnResetToDefault->Action = NULL;
		AFormSettingsEditorPtr->BtnResetToDefault->OnClick = AProtocolControlBindingPtr->ResetToDefaultClick;

		if (AFormSettingsEditorPtr->ShowModal() == mrOk) {
			this->Assign(AProtocolControlBindingPtr.get());
			return true;
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	return false;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolControlBinding::GetStateMachineName(void) {
	if (FStateMachineName.IsEmpty()) {
		TCustomTreeElement *AElement = dynamic_cast<TCustomTreeElement*>(FParent);
		if (AElement) {
			return GetRootScxmlName(AElement->Tree, FStateMachineName);
		}
	}
	return FStateMachineName;
}

// ---------------------------------------------------------------------------
bool __fastcall TProtocolControlBinding::IsStateMachineNameStored(void) {
	return !FStateMachineName.IsEmpty();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBinding::MakeScxmlNameFromControlName(const bool bIgnoreCanSuffix) {
	UnicodeString sScxmlPri = L"";
	if (BindTypes.Contains(pbtConsumer) && BindTypes.Contains(pbtSupplier)) {
		sScxmlPri = L"IO.";
	}
	else if (BindTypes.Contains(pbtConsumer)) {
		sScxmlPri = L"Inp.";
	}
	else if (BindTypes.Contains(pbtSupplier)) {
		sScxmlPri = L"Out.";
	}

	UnicodeString sScxmlName = sScxmlPri + StringReplace(this->ControlName, "_", ".", TReplaceFlags() << rfReplaceAll);
	if (bIgnoreCanSuffix) {
		std::auto_ptr<TStringList>AScxmlListPtr(new TStringList());
		AScxmlListPtr->Delimiter = L'.';
		AScxmlListPtr->StrictDelimiter = true;
		AScxmlListPtr->DelimitedText = sScxmlName;

		std::auto_ptr<TStringList>ASuffixListPtr(new TStringList());
		ASuffixListPtr->Add("ID");
		ASuffixListPtr->Add("IL");
		ASuffixListPtr->Add("OD");
		ASuffixListPtr->Add("OL");
		ASuffixListPtr->Add("SC");

		if (AScxmlListPtr->Count > 1 && ASuffixListPtr->IndexOf(AScxmlListPtr->Strings[AScxmlListPtr->Count - 1]) != -1) {
			AScxmlListPtr->Delete(AScxmlListPtr->Count - 1);
		}

		sScxmlName = AScxmlListPtr->DelimitedText;
	}

	this->ScxmlName = sScxmlName;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TProtocolControlBindingPropertyEditor::GetAttrs() {
	return TLMDClassPropEditor::GetAttrs() << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolControlBindingPropertyEditor::Edit() {
	// Function is used to show editor dialod.
	// It will be executed when editor button (with dotes)
	// is clicked or when Ctrl+Enter button is pressed.
	try {

		TProtocolControlBinding *ABinding = dynamic_cast<TProtocolControlBinding*>(this->GetObjectValue(0));
		if (ABinding) {
			ABinding->EditBinding();
			if (this->PropInsp)
				this->PropInsp->UpdateContent();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TProtocolControlBindingPropertyEditor::GetValue(void) {
	TProtocolControlBinding *ABinding = dynamic_cast<TProtocolControlBinding*>(this->GetObjectValue(0));
	if (ABinding) {
		return ABinding->GetDisplayName();
	}
	return TLMDClassPropEditor::GetValue();
}

// ---------------------------------------------------------------------------
// -------------------------- TProtocolBindingShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TProtocolBindingShape::TProtocolBindingShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes = TStateChildTypes();

	FConvertibleTypes.insert(sctWeakProtocolBinding);

	FProtocolControlBinding = new TProtocolControlBinding(this);
	FContentTrigger = new TContentTrigger(this);
	FParams = new TParamTriggerItems(this);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngProtocol");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
_fastcall TProtocolBindingShape::~TProtocolBindingShape() {
	if (FProtocolControlBinding) {
		delete FProtocolControlBinding;
	}
	if (FContentTrigger) {
		delete FContentTrigger;
	}
	if (FParams) {
		delete FParams;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolBindingShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {

	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);

	if (APropInfo) {
		if (APropInfo->Name() == L"ProtocolControlBinding") {
			AEditorClass = __classid(TProtocolControlBindingPropertyEditor);
		}
		else if (APropInfo->Name() == L"Params") {
			AEditorClass = __classid(TParamTriggersPropEditor);
		}
		else if (APropInfo->Name() == L"ContentTrigger") {
			AEditorClass = __classid(TContentPropEditor);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolBindingShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TProtocolBindingShape * AProtocolShape = dynamic_cast<TProtocolBindingShape*>(Source);
			if (AProtocolShape) {
				FProtocolControlBinding->Assign(AProtocolShape->FProtocolControlBinding);
				FContentTrigger->Assign(AProtocolShape->FContentTrigger);
				FParams->Assign(AProtocolShape->FParams);

				UpdateSimpleText();
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
TColor __fastcall TProtocolBindingShape::GetNormalFontColor(void) {
	TColor AChannelColor = TChildScxmlBaseShape::GetNormalFontColor();

	if (this->ProtocolControlBinding->BindTypes.Contains(pbtConsumer) && this->ProtocolControlBinding->BindTypes.Contains(pbtSupplier)) {
		AChannelColor = clPurple;
	}
	else if (this->ProtocolControlBinding->BindTypes.Contains(pbtConsumer)) {
		AChannelColor = clBlue;
	}

	TColor ActiveColor = this->ProtocolControlBinding->DoNotSendToScxml ? clGreen : AChannelColor;

	return this->ProtocolControlBinding->Active ? ActiveColor : clLtGray;
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TProtocolBindingShape::GetNormalFontStyle() {
	return this->ProtocolControlBinding->Active ? TChildScxmlBaseShape::GetNormalFontStyle() : (TFontStyles() << fsItalic);
}

// ---------------------------------------------------------------------------
TColor __fastcall TProtocolBindingShape::GetNormalBorderColor(void) {
	return this->ContentTrigger->Enabled ? TColor(0x004080FF) : clBlack;
}

// ---------------------------------------------------------------------------
TColor __fastcall TProtocolBindingShape::GetNormalColor(void) {
	return this->ContentTrigger->Enabled ? clInfoBk : TChildScxmlBaseShape::GetNormalColor();
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolBindingShape::UpdateSimpleText() {
	if (!ComponentState.Contains(csLoading) && !ComponentState.Contains(csReading)) {

		UnicodeString sOut = GetOutValue();
		if (ClipOutValue && sOut.Length() > ClipOutMax) {
			sOut = sOut.SetLength(ClipOutMax).Trim() + " ...";
		}

		SimpleText = sOut;

		UpdateParentSimpleText();

		if (StateMachineEditor && StateMachineEditor->NodeTheTree->Checked) {
			StateMachineEditor->MarkUpdateNodeTreeElement(this);
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolBindingShape::GetOutValue() {
	std::auto_ptr<TStringList>AExtraInfoPtr(new TStringList());
	if (!FProtocolControlBinding->StateMachineName.IsEmpty()) {
		AExtraInfoPtr->Add(FProtocolControlBinding->StateMachineName);
	}
	if (!FProtocolControlBinding->StateMachineID.IsEmpty()) {
		AExtraInfoPtr->Add(FProtocolControlBinding->StateMachineID);
	}
	if (!FProtocolControlBinding->ScxmlInvokerIDs.IsEmpty()) {
		AExtraInfoPtr->Add(FProtocolControlBinding->ScxmlInvokerIDs);
	}

	UnicodeString sOut = FProtocolControlBinding->ScxmlName + " {" + FProtocolControlBinding->ControlName + "}";
	if (AExtraInfoPtr->Count) {
		sOut = sOut + UnicodeString().sprintf(L" [%s]", AExtraInfoPtr->CommaText.c_str());
	}

	return sOut;
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolBindingShape::DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R) /* inherited */ {
	TScxmlBaseShape::DrawShapeCanvas(ACanvas, R);

	ACanvas->TextAlign = 0; // без этого может позиционироваться по центру
	int iDescriptionOffset = 0;

	ACanvas->BackMode = cbmTransparent; // без этого может задний фонт быть непрозрачным
	ACanvas->Font->Style = TFontStyles();

	if (ProtocolControlBinding->Active) {
		if (ProtocolControlBinding->BindTypes.ToInt() == 0) {
			ACanvas->Font->Color = clMaroon;
			const UnicodeString sText = L"! BindTypes !";
			ACanvas->TextOut3D(R.Right + 3, R.Top, TeeTreeZ, sText);
			iDescriptionOffset = ACanvas->TextWidth(sText);
		}
		else {
			switch(ProtocolControlBinding->ChannelType) {
			case ctZMQ: {
					ACanvas->Font->Color = clTeal;
					ACanvas->Font->Size = 6;
					const UnicodeString sText = L"ZMQ";
					ACanvas->TextOut3D(R.Right + 3, R.Top + 3, TeeTreeZ, sText);
					iDescriptionOffset = ACanvas->TextWidth(sText);
				}break;
			case ctMulticast: {
					ACanvas->Font->Color = clPurple;
					ACanvas->Font->Size = 6;
					const UnicodeString sText = L"MULTICAST";
					ACanvas->TextOut3D(R.Right + 3, R.Top + 3, TeeTreeZ, sText);
					iDescriptionOffset = ACanvas->TextWidth(sText);
				}break;
			}
		}
	}

	if (!Description.IsEmpty()) {
		ACanvas->Font->Color = clGreen;
		ACanvas->Font->Size = 8;
		ACanvas->TextOut3D(R.Right + 3 + iDescriptionOffset + 3, R.Top, TeeTreeZ, Description);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolBindingShape::MakeDefaultContentTriggersByControlType(void) {
	switch(this->ProtocolControlBinding->ProtocolControlType) {
	case pctLogic:
		this->ContentTrigger->Off = "0";
		this->ContentTrigger->On = "1";
		this->ContentTrigger->Appearance = ctaCheckBox;
		break;
	case pctInteger:
		this->ContentTrigger->Appearance = ctaTrackBar;
		this->ContentTrigger->TriggerType = cttInteger;
		break;
	case pctDouble:
		this->ContentTrigger->Appearance = ctaTrackBar;
		this->ContentTrigger->TriggerType = cttDouble;
		break;
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolBindingShape::GetClipboardInfo(void) {
	if (!ProtocolControlBinding->ScxmlName.IsEmpty()) {
		return ProtocolControlBinding->ScxmlName;
	}
	if (!ProtocolControlBinding->ControlName.IsEmpty()) {
		return ProtocolControlBinding->ControlName;
	}
	return TChildScxmlBaseShape::GetClipboardInfo();
}

// ---------------------------------------------------------------------------
// -------------------------- TProtocolWeakBindingShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TProtocolWeakBindingShape::TProtocolWeakBindingShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FScxmlName = L"";
	FStateMachineName = L"";
	FStateMachineID = L"";
	FScxmlInvokerIDs = L"";

	FAvailableTypes = TStateChildTypes();

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngProtocolWeak");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
_fastcall TProtocolWeakBindingShape::~TProtocolWeakBindingShape() {
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolWeakBindingShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {

	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolWeakBindingShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			// особый случай
			TProtocolBindingShape * AProtocolBindingShape = dynamic_cast<TProtocolBindingShape*>(Source);
			if (AProtocolBindingShape) {

				FStateMachineName = AProtocolBindingShape->ProtocolControlBinding->FStateMachineName;
				FStateMachineID = AProtocolBindingShape->ProtocolControlBinding->FStateMachineID;
				FScxmlName = AProtocolBindingShape->ProtocolControlBinding->FScxmlName;
				FScxmlInvokerIDs = AProtocolBindingShape->ProtocolControlBinding->FScxmlInvokerIDs;

				UpdateSimpleText();

				return;
			}

			TProtocolWeakBindingShape * AProtocolShape = dynamic_cast<TProtocolWeakBindingShape*>(Source);
			if (AProtocolShape) {
				FStateMachineName = AProtocolShape->FStateMachineName;
				FStateMachineID = AProtocolShape->FStateMachineID;
				FScxmlName = AProtocolShape->FScxmlName;
				FScxmlInvokerIDs = AProtocolShape->FScxmlInvokerIDs;

				UpdateSimpleText();
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
TColor __fastcall TProtocolWeakBindingShape::GetNormalFontColor(void) {
	return this->ScxmlName.IsEmpty() == false ? TChildScxmlBaseShape::GetNormalFontColor() : clRed;
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TProtocolWeakBindingShape::GetNormalFontStyle() {
	return this->ScxmlName.IsEmpty() == false ? TChildScxmlBaseShape::GetNormalFontStyle() : (TFontStyles() << fsItalic);
}

// ---------------------------------------------------------------------------
TColor __fastcall TProtocolWeakBindingShape::GetNormalBorderColor(void) {
	return clGreen;
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolWeakBindingShape::UpdateSimpleText() {
	if (!ComponentState.Contains(csLoading) && !ComponentState.Contains(csReading)) {

		UnicodeString sOut = GetOutValue();
		if (ClipOutValue && sOut.Length() > ClipOutMax) {
			sOut = sOut.SetLength(ClipOutMax).Trim() + " ...";
		}

		SimpleText = sOut;

		UpdateParentSimpleText();

		if (StateMachineEditor && StateMachineEditor->NodeTheTree->Checked) {
			StateMachineEditor->MarkUpdateNodeTreeElement(this);
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TProtocolWeakBindingShape::IsStateMachineStored(void) {
	return !FStateMachineName.IsEmpty();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolWeakBindingShape::GetStateMachineName(void) {
	if (FStateMachineName.IsEmpty())
		return GetRootScxmlName(Tree, FStateMachineName);
	return FStateMachineName;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolWeakBindingShape::GetOutValue() {
	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	if (!StateMachineName.IsEmpty()) {
		AStringListPtr->Add(StateMachineName);
	}
	if (!StateMachineID.IsEmpty()) {
		AStringListPtr->Add(StateMachineID);
	}
	if (!ScxmlInvokerIDs.IsEmpty()) {
		AStringListPtr->Add(ScxmlInvokerIDs);
	}
	return UnicodeString().sprintf(L"%s {%s}", ScxmlName.c_str(), AStringListPtr->CommaText.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TProtocolWeakBindingShape::ToXML(ILMDXmlElement *ANodeContainer) {
	const UnicodeString sContainerName = ANodeContainer->GetAttr("NAME", "!!!UNKNOWN!!!");

	_di_ILMDXmlElement ANodeItem = ANodeContainer->AppendElement("item");
	if (ANodeItem) {

		ANodeItem->SetAttr("ScxmlName", this->ScxmlName);
		ANodeItem->SetAttr("StateMachineName", this->StateMachineName);

		if (!this->StateMachineID.IsEmpty()) {
			ANodeItem->SetAttr("StateMachineID", this->StateMachineID);
		}
		if (!this->ScxmlInvokerIDs.IsEmpty()) {
			ANodeItem->SetAttr("ScxmlInvokerIDs", this->ScxmlInvokerIDs);
		}
		// запишем имя

		ANodeItem->SetAttr("ShapeType", this->ClassName());
		ANodeItem->SetAttr("ShapeName", this->Name);

	}
	else
		throw Exception("Can not append <item> node!");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProtocolWeakBindingShape::GetClipboardInfo(void) {
	if (!ScxmlName.IsEmpty()) {
		return ScxmlName;
	}
	return TChildScxmlBaseShape::GetClipboardInfo();
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TProtocolWeakBindingShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"ScxmlName", boost::bind(&TProtocolWeakBindingShape::EditUnicodeStringLineProperty, this,
				L"ScxmlName")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
// -------------------------- TSetValueShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TSetValueShape::TSetValueShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FProtocolControlBinding(0) {
	FTarget = L"";
	FTargetExpr = L"";
	FValue = L"";
	FValueExpr = L"";
	FType = L"";
	FTypeExpr = L"";
	FPath = L"";
	FPathExpr = L"";

	this->FStateAttrMap["Target"] = false;
	this->FStateAttrMap["Targetexpr"] = false;
	this->FStateAttrMap["Value"] = false;
	this->FStateAttrMap["Valueexpr"] = false;
	this->FStateAttrMap["Type"] = false;
	this->FStateAttrMap["Typeexpr"] = false;
	this->FStateAttrMap["Path"] = false;
	this->FStateAttrMap["Pathexpr"] = false;

	FProtocolControlBinding = new TProtocolControlBinding(this);
	FProtocolControlBinding->BindTypes = TProtocolBindTypes() << pbtSupplier;

	FConvertibleTypes.insert(sctSend);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngSetValue");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
__fastcall TSetValueShape::~TSetValueShape() {
	if (FProtocolControlBinding) {
		delete FProtocolControlBinding;
		FProtocolControlBinding = 0;
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSetValueShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {

	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == "Type" || sPropName == "Typeexpr") {
		return //
		"<B>1</B> - in this case value will be sent to AVIA protocol<BR>\n" //
		"<B>2</B> - in this case value will be sent to ZMQ protocol<BR>\n" //
		"<B>3</B> - in this case value will be sent to AviaPlugin<BR>\n" //
		"<B>4</B> - in this case value will be sent to Multicast<BR>\n" //
		"By default value is sent to <B>Global Data Storage</B> only!";
	}
	if (sPropName == "Target" || sPropName == "Targetexpr") {
		return //
		"This means <B>ScxmlName</B> of value for sending\n" //
		"If this element is sent to AVIA protocol it must be previously registered via <B>TProtocolControlBinding</B>";
	}
	if (sPropName == "Path" || sPropName == "Pathexpr") {
		return //
		"<B>Global Data Storage</B> is indexed by key using <B>ScxmlName</B> but additionally can have tree data structure inside\n" //
		"For example: key <B>Inp.AccBus1</B> can have subvalue <B>Root</B>, this subvalue can have subvalue <B>Child1</B>\n" //
		"So, if you are planning to access <B>Child1</B> subvalue, you must write in the <B>Path</B> field value <B>Root.Child1</B>";
	}
	if (sPropName == "Value") {
		return //
		"Value is interpreted by the next code:\n" "<B>uscxml::Data data(ATTR(node, X(\"value\")), uscxml::Data::INTERPRETED);</B>\n" //
		"That's why it is strongly recommended to use it for simple types: <B>bool,int,double</B>!\n" //
		"Type <B>string</B> MUST BE ESCAPED WITH QUOTES!!!\n" //
		"For example: int=1, double=0.1, string=\"escaped string!!!\" \n" //;
		"P.S. It is better to send <B>TABLE</B> values via <B>valuexpr</B> attr.";

	}
	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TSetValueShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {

	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);

	if (APropInfo) {
		std::auto_ptr<TStringList>AUnicodeSyntaxList(new TStringList());
		AUnicodeSyntaxList->CaseSensitive = true;
		AUnicodeSyntaxList->Text = L"Targetexpr\n" "Valueexpr\n" "Typeexpr\n" "Pathexpr";

		if (APropInfo->Name() == L"ProtocolControlBinding") {
			AEditorClass = __classid(TProtocolControlBindingPropertyEditor);
		}
		else if (APropInfo->Name() == L"Type") {
			AEditorClass = __classid(SetValueTypePropEditor);
		}
		else if (AUnicodeSyntaxList->IndexOf(APropInfo->Name()) != -1) {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
		}
	}
}

// ---------------------------------------------------------------------------
TColor __fastcall TSetValueShape::GetNormalFontColor(void) {
	if (Type == "2")
		return clTeal;
	if (Type == "3")
		return clNavy;
	if (Type == "4")
		return clPurple;
	return TChildScxmlBaseShape::GetNormalFontColor();
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TSetValueShape::GetNormalFontStyle() {
	TFontStyles ANormalFontStyles = TChildScxmlBaseShape::GetNormalFontStyle();
	return(Type == "1" || Type == "2" || Type == "3" || Type == "4") ? (ANormalFontStyles << fsBold) : ANormalFontStyles;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSetValueShape::GetOutValue() {
	UnicodeString sPath = FPath.IsEmpty() ? FPathExpr : FPath;
	if (!sPath.IsEmpty()) {
		sPath = L"\\" + sPath;
	}
	const UnicodeString sBindingName = FProtocolControlBinding->Active ? (L" {" + FProtocolControlBinding->ControlName + "}")
		: UnicodeString(L"");

	return UnicodeString().sprintf(L"%s%s=[%s]%s", !FTarget.IsEmpty() ? FTarget.c_str() : FTargetExpr.c_str(), sPath.c_str(),
		!FValue.IsEmpty() ? FValue.c_str() : FValueExpr.c_str(), sBindingName.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TSetValueShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			if (TSetValueShape * ASetValueShape = dynamic_cast<TSetValueShape*>(Source)) {
				FTarget = ASetValueShape->Target;
				FTargetExpr = ASetValueShape->Targetexpr;
				FValue = ASetValueShape->Value;
				FValueExpr = ASetValueShape->Valueexpr;
				FPath = ASetValueShape->Path;
				FPathExpr = ASetValueShape->Pathexpr;
				FType = ASetValueShape->Type;
				FTypeExpr = ASetValueShape->Typeexpr;

				FProtocolControlBinding->Assign(ASetValueShape->FProtocolControlBinding);

				UpdateSimpleText();
			} /* ПРЕОБРАЗОВАНИЯ ИЗ ДРУГИХ ТИПОВ */
			else if (TSendShape * ASendShape = dynamic_cast<TSendShape*>(Source)) {

				this->FTarget = ASendShape->Event;
				this->FTargetExpr = ASendShape->Eventexpr;

				this->FType = SendTypeToSetValueType(ASendShape->Type);
				if (!ASendShape->Typeexpr.IsEmpty()) {
					this->FTypeExpr = "!REQUIRE CONVERSION!=" + ASendShape->Typeexpr;
					WLOG_ERROR(L"Attr 'Typeexpr' with value '%s' does not support conversion!", ASendShape->Typeexpr.c_str());
				}

				TContentShape * AContentShape = NULL;
				for (int i = 0; i < ASendShape->Children->Count; i++) {
					AContentShape = dynamic_cast<TContentShape*>(ASendShape->Children->Items[i]);
					if (AContentShape)
						break;
				}

				if (AContentShape) {
					const UnicodeString sValue = AContentShape->Expr.IsEmpty() ? AContentShape->XMLText->Text : AContentShape->Expr;

					bool bIsOrd = false;

					double Val;
					if (TryStrToFloat(sValue, Val)) {
						bIsOrd = true;
					}
					else if (sValue == L"true" || sValue == L"false") {
						bIsOrd = true;
					}

					if (bIsOrd) {
						this->FValue = sValue;
					}
					else {
						this->FValueExpr = sValue;
					}
				}

				while (ASendShape->Children->Count) {
					delete ASendShape->Children->Items[0];
				}

				FProtocolControlBinding->Assign(ASendShape->ProtocolControlBinding);

				UpdateSimpleText();
			}

		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TSetValueShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Value", boost::bind(&TSetValueShape::EditUnicodeStringLineProperty, this, L"Value")));
	AVecEditors.push_back(std::make_pair(L"Valueexpr", boost::bind(&TSetValueShape::EditUnicodeStringSyntaxProperty, this, L"Valueexpr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
// -----------------------SetValueTypePropEditor-------------
// ---------------------------------------------------------------------------
Lmdinspropinsp::TLMDPropAttrs __fastcall SetValueTypePropEditor::GetAttrs(void) {
	return TLMDStringPropEditor::GetAttrs() << praValueList;
}

// ---------------------------------------------------------------------------
void __fastcall SetValueTypePropEditor::GetValues(Classes::TStrings* AValues) {
	AValues->Add("1");
	AValues->Add("2");
	AValues->Add("3");
}

// ---------------------------------------------------------------------------
// -------------------------- TSessionStateShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TSessionStateShape::TSessionStateShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes = FAvailableTypes /* << sctParam << sctContent */ ;

	FEvent = L"";
	FEventExpr = L"";

	this->FStateAttrMap["Event"] = false;
	this->FStateAttrMap["Eventexpr"] = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngSessionState");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSessionStateShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {

	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);

	if (APropInfo) {
		std::auto_ptr<TStringList>AUnicodeSyntaxList(new TStringList());
		AUnicodeSyntaxList->CaseSensitive = true;
		AUnicodeSyntaxList->Text = L"Eventexpr";

		if (APropInfo->Name() == L"ProtocolControlBinding") {
			AEditorClass = __classid(TProtocolControlBindingPropertyEditor);
		}
		else if (AUnicodeSyntaxList->IndexOf(APropInfo->Name()) != -1) {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSessionStateShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {

	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == "Event" || sPropName == "Eventexpr") {
		return //
		"Possible values: SHUTDOWN, LOAD, ONLINE, TRAININGINIT, TRAININGPAUSED, TRAININGREADY, TRAININGACTIVE, DEBRIEFINIT, DEBRIEFPAUSED, DEBRIEFREADY, DEBRIEFACTIVE, STOPPED, ERROR<BR>\n";
	}

	return L"";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSessionStateShape::GetOutValue() {
	return Event.IsEmpty() ? Eventexpr : Event;
}

// ---------------------------------------------------------------------------
void __fastcall TSessionStateShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TSessionStateShape * ASetValueShape = dynamic_cast<TSessionStateShape*>(Source);
			if (ASetValueShape) {
				FEvent = ASetValueShape->Event;
				FEventExpr = ASetValueShape->Eventexpr;

				UpdateSimpleText();
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TSessionStateShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Event", boost::bind(&TSessionStateShape::EditUnicodeStringLineProperty, this, L"Event")));
	AVecEditors.push_back(std::make_pair(L"Eventexpr", boost::bind(&TSessionStateShape::EditUnicodeStringSyntaxProperty, this,
				L"Eventexpr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
// -------------------------- TVirtualFolderShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TVirtualFolderShape::TVirtualFolderShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {

	FAvailableTypes.insert(sctProtocolBinding);
	FAvailableTypes.insert(sctWeakProtocolBinding);
	FAvailableTypes.insert(sctVirtualFolder);
	FAvailableTypes.insert(sctTrigger);
	FAvailableTypes.insert(sctWatch);

	FCaption = L"";

	this->FStateAttrMap["Caption"] = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngVirtualFolder");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
TColor __fastcall TVirtualFolderShape::GetNormalFontColor(void) {
	return clMaroon;
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TVirtualFolderShape::GetNormalFontStyle() {
	return TFontStyles() << fsBold;
}

// ---------------------------------------------------------------------------
void __fastcall TVirtualFolderShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {

	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);

	if (APropInfo) {
		if (APropInfo->Name() == L"Caption") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVirtualFolderShape::UpdateSimpleText() {
	if (!ComponentState.Contains(csLoading) && !ComponentState.Contains(csReading)) {

		// почему то без переменной XMLName возвращает NULL !
		const UnicodeString &sXMLName = XMLName;

		UnicodeString sOut = Caption.IsEmpty() ? sXMLName : Caption;
		if (ClipOutValue && sOut.Length() > ClipOutMax) {
			sOut = sOut.SetLength(ClipOutMax).Trim() + " ...";
		}
		// здесь используем полный текст, так как возможно хотим видеть многострочный комментарий!!!
		this->Text->Text = sOut;

		UpdateParentSimpleText();

		if (StateMachineEditor && StateMachineEditor->NodeTheTree->Checked) {
			StateMachineEditor->MarkUpdateNodeTreeElement(this);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVirtualFolderShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TVirtualFolderShape * AVirtualFolderShape = dynamic_cast<TVirtualFolderShape*>(Source);
			if (AVirtualFolderShape) {
				FCaption = AVirtualFolderShape->Caption;

				UpdateSimpleText();
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

void TVirtualFolderShape::RecursiveAddProtocolBindingsAndFolders(TScxmlBaseShape *AParent, TNodeShapeList *AShapeList, int iRecursiveLevel,
	bool bIgnoreCanSuffix) {
	if (!AParent)
		throw Exception("Parent can not be NULL!");

	for (int i = 0; i < AShapeList->Count; i++) {
		TTreeNodeShape *AShape = AShapeList->Items[i];

		TScxmlBaseShape *ANewParentShape = AParent;

		if (AShape->ImageIndex == tiChecked) {

			// новая импортируемая нода
			TScxmlBaseShape *ANewImportedShape = NULL;

			TBindingImportingShape *ABindingImportingShape = dynamic_cast<TBindingImportingShape*>(AShapeList->Items[i]);
			if (ABindingImportingShape) {
				if (!ABindingImportingShape->AviaName.IsEmpty()) {
					TProtocolBindingShape *ABindingShape = dynamic_cast<TProtocolBindingShape*>(AParent->AddStateChild(sctProtocolBinding,
							false));
					if (ABindingShape) {

						ANewImportedShape = ABindingShape;

						ABindingShape->ClipOutValue = false;
						ABindingShape->ProtocolControlBinding->ControlName = ABindingImportingShape->AviaName;
						ABindingShape->ProtocolControlBinding->Comment = ABindingImportingShape->discription;

						if (SameText(ABindingImportingShape->Channel, "Zmq")) {
							ABindingShape->ProtocolControlBinding->ChannelType = ctZMQ;
						}
						else if (SameText(ABindingImportingShape->Channel, "Multicast")) {
							ABindingShape->ProtocolControlBinding->ChannelType = ctMulticast;
						}

						// игнорируем мин-максы для Logic'а
						if (ABindingImportingShape->Min || ABindingImportingShape->Max) {
							ABindingShape->ProtocolControlBinding->Min = FormatFloat("0.####", ABindingImportingShape->Min);
							ABindingShape->ContentTrigger->Min = ABindingShape->ProtocolControlBinding->Min;
							ABindingShape->ProtocolControlBinding->Max = FormatFloat("0.####", ABindingImportingShape->Max);
							ABindingShape->ContentTrigger->Max = ABindingShape->ProtocolControlBinding->Max;
						}

						/* Consumer-Supplier*/
						TProtocolBindTypes ATypes;
						if (ABindingImportingShape->Type == "Output") {
							ATypes = ATypes << pbtSupplier;
						}
						if (ABindingImportingShape->Type == "Input") {
							ATypes = ATypes << pbtConsumer;
						}
						ABindingShape->ProtocolControlBinding->BindTypes = ATypes;

						/* Тип контрола: Int, Double, String ... */
						std::pair<TProtocolControlType, int>AControlTypePair = SplitProtocolControlType(ABindingImportingShape->Param);

						ABindingShape->ProtocolControlBinding->ProtocolControlType = AControlTypePair.first;
						ABindingShape->ProtocolControlBinding->ArraySize = AControlTypePair.second;

						ABindingShape->MakeDefaultContentTriggersByControlType();

						ABindingShape->ProtocolControlBinding->MakeScxmlNameFromControlName(bIgnoreCanSuffix);

						ABindingShape->ProtocolControlBinding->Active = true;

					}
				}

			}
			else {
				TVirtualFolderShape *AVirtFolderShape = dynamic_cast<TVirtualFolderShape*>(AParent->AddStateChild(sctVirtualFolder, true));
				if (AVirtFolderShape) {

					// в этом случае меняем родителя
					ANewParentShape = AVirtFolderShape;

					ANewImportedShape = AVirtFolderShape;

					AVirtFolderShape->Caption = AShape->Text->Text.Trim();
				}

			}

			// если что-то добавлено (Binding или VirtualFolder
			if (ANewImportedShape) {
				//if (iRecursiveLevel == 0) {
				ANewImportedShape->Selected = true;
				//}
			}

		}

		// рекурсивно обходим дерево дальше
		RecursiveAddProtocolBindingsAndFolders(ANewParentShape, AShape->Children, iRecursiveLevel + 1, bIgnoreCanSuffix);
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TVirtualFolderShape::ImportBindingsAndFolders(TScxmlBaseShape *ARoot) {
	if (!BindingsImporter) {
		BindingsImporter = new TBindingsImporter(Application);
	}

	if (ARoot->StateMachineEditorUnit && ARoot->StateMachineEditorUnit->StateMachineProject) {
		bool bSuccessLoad = true;
		if (FileExists(ARoot->StateMachineEditorUnit->StateMachineProject->ProjectProperties->CalibratorCfg)) {
			BindingsImporter->EditDocCalibrator->Lines->LoadFromFile
				(ARoot->StateMachineEditorUnit->StateMachineProject->ProjectProperties->CalibratorCfg);
		}
		else {
			bSuccessLoad = false;
			WLOG_WARNING(L"Calibrator config file [%s] is not found!",
				ARoot->StateMachineEditorUnit->StateMachineProject->ProjectProperties->CalibratorCfg.c_str());

		}
		if (FileExists(ARoot->StateMachineEditorUnit->StateMachineProject->ProjectProperties->CanserviceCfg)) {
			BindingsImporter->EditDocCanservice->Lines->LoadFromFile
				(ARoot->StateMachineEditorUnit->StateMachineProject->ProjectProperties->CanserviceCfg);
		}
		else {
			bSuccessLoad = false;
			WLOG_WARNING(L"Canservice config file [%s] is not found!",
				ARoot->StateMachineEditorUnit->StateMachineProject->ProjectProperties->CanserviceCfg.c_str());
		}
		if (bSuccessLoad) {
			BindingsImporter->BtnValidateCanserviceClick(BindingsImporter->BtnValidateCanservice);
		}
	}

	if (BindingsImporter->ShowModal() == mrOk) {
		try {
			ARoot->Selected = false;

			RecursiveAddProtocolBindingsAndFolders(ARoot, BindingsImporter->Tree1->Roots, 0,
				BindingsImporter->CheckIgnoreCanSuffix->Checked);

			ARoot->Expanded = true;

			return true;
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
void TVirtualFolderShape::EditBindings(void) {
	if (ImportBindingsAndFolders(this)) {
		if (StateMachineEditor) {
			StateMachineEditor->MarkModified(1, UnicodeString().sprintf(L"Edit 'VirtualFolder Bindings':[%s]", this->Caption.c_str()),
				true);
		}
	}
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TVirtualFolderShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Caption", boost::bind(&TVirtualFolderShape::EditUnicodeStringLineProperty, this, L"Caption")));

	if (SettingsData->AviaExtensionEnabled) {
		AVecEditors.push_back(std::make_pair(L"Bindings", boost::bind(&TVirtualFolderShape::EditBindings, this)));
	}

	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TVirtualFolderShape::DefaultEditProperty(void) {
	if (SameText(EditByDefault, SCXML_EDITBYDEFAULT_AUTO)) {
		EditUnicodeStringLineProperty(L"Caption");
	}
	else {
		std::vector<TDefaultEditorPair>AVecEditors = GetEditByDefaultEditors();

		for (std::size_t i = 0; i < AVecEditors.size(); i++) {
			if (SameText(AVecEditors[i].first, EditByDefault)) {
				AVecEditors[i].second();
				return;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVirtualFolderShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawVirtualFolder(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
std::vector<TProtocolBindingShape *>__fastcall CreateProtocolBindingShapeFromJSON(TScxmlBaseShape *AParent, const UnicodeString &sJSONText)
{
	std::vector<TProtocolBindingShape*>AVecShapes;

	if (AParent) {
		std::auto_ptr<myJSONItem>AJSONItemPtr(new myJSONItem());
		AJSONItemPtr->Code = sJSONText;

		for (int i = 0; i < AJSONItemPtr->Count(); i++) {

			myJSONItem *AControl = AJSONItemPtr->Value[i];

			TProtocolBindingShape *ABindingShape = dynamic_cast<TProtocolBindingShape*>(AParent->AddStateChild(sctProtocolBinding, false));
			if (ABindingShape) {

				ABindingShape->ClipOutValue = false;
				ABindingShape->ProtocolControlBinding->BindTypes = TProtocolBindTypes();

				std::auto_ptr<TStringList>ACustomParamsPtr(new TStringList());

				for (int k = 0; k < AControl->Count(); k++) {
					const UnicodeString sName = AControl->Value[k]->Name;
					if (SameText(sName, L"name")) {
						ABindingShape->ProtocolControlBinding->ControlName = AControl->Value[k]->getStr();
					}
					else if (SameText(sName, L"description")) {
						ABindingShape->ProtocolControlBinding->Comment = AControl->Value[k]->getStr();
					}
					else if (SameText(sName, L"channel")) {
						const UnicodeString sChannel = AControl->Value[k]->getStr();
						if (SameText(sChannel, L"Zmq")) {
							ABindingShape->ProtocolControlBinding->ChannelType = ctZMQ;
						}
						else if (SameText(sChannel, L"Multicast")) {
							ABindingShape->ProtocolControlBinding->ChannelType = ctMulticast;
						}
					}
					else if (SameText(sName, L"type")) {
						if (SameText(AControl->Value[k]->getStr(), L"Input")) {
							ABindingShape->ProtocolControlBinding->BindTypes = ABindingShape->ProtocolControlBinding->BindTypes <<
								pbtConsumer;
						}
						else if (SameText(AControl->Value[k]->getStr(), L"Output")) {
							ABindingShape->ProtocolControlBinding->BindTypes = ABindingShape->ProtocolControlBinding->BindTypes <<
								pbtSupplier;
						}
					}
					else if (SameText(sName, L"dataType")) {
						/* Тип контрола: Int, Double, String ... */
						std::pair<TProtocolControlType, int>AControlTypePair = SplitProtocolControlType(AControl->Value[k]->getStr());

						ABindingShape->ProtocolControlBinding->ProtocolControlType = AControlTypePair.first;
						ABindingShape->ProtocolControlBinding->ArraySize = AControlTypePair.second;

					}
					else if (SameText(sName, L"other")) {

						for (int n = 0; n < AControl->Value[k]->Count(); n++) {
							const UnicodeString sOtherName = AControl->Value[k]->Value[n]->Name;
							if (SameText(sOtherName, L"Max")) {
								ABindingShape->ProtocolControlBinding->Max = AControl->Value[k]->Value[n]->getInt();
							}
							else if (SameText(sOtherName, L"Min")) {
								ABindingShape->ProtocolControlBinding->Min = AControl->Value[k]->Value[n]->getInt();
							}
							else if (SameText(sOtherName, L"ScxmlName")) {
								ABindingShape->ProtocolControlBinding->ScxmlName = AControl->Value[k]->Value[n]->getStr();
							}
							else if (SameText(sOtherName, L"Udpenvvariable")) {
								ACustomParamsPtr->Values[L"Udpenvvariable"] = AControl->Value[k]->Value[n]->getStr();
							}
						}

					}
					else if (SameText(sName, L"params")) {
						for (int n = 0; n < AControl->Value[k]->Count(); n++) {
							switch(n) {
							case 0:
								ABindingShape->ProtocolControlBinding->SessionNodePath = StringReplace
									(AControl->Value[k]->Value[n]->getStr(), L"\\\\", L"\\", TReplaceFlags() << rfReplaceAll);
								break;
							case 1:
								ABindingShape->ProtocolControlBinding->SessionNodeAttribute = AControl->Value[k]->Value[n]->getStr();
								break;
							}
						}

						if (!ABindingShape->ProtocolControlBinding->SessionNodePath.IsEmpty()) {
							ABindingShape->ProtocolControlBinding->UseSessionServer = true;
						}
					}
				}

				ABindingShape->ProtocolControlBinding->CustomAttributes = ACustomParamsPtr->Text.Trim();

				ABindingShape->MakeDefaultContentTriggersByControlType();

				// создаем сами, если не было указано в конфиге
				if (ABindingShape->ProtocolControlBinding->ScxmlName.IsEmpty()) {
					ABindingShape->ProtocolControlBinding->MakeScxmlNameFromControlName(true);
				}

				ABindingShape->ProtocolControlBinding->Active = true;

				AVecShapes.push_back(ABindingShape);
			}
			else {
				WLOG_ERROR(L"Can not create binding at parent[%s]", AParent->SimpleText);
			}

		}

	}

	return AVecShapes;
}
