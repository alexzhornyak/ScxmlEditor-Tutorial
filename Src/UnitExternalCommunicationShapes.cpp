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

#include "UnitExternalCommunicationShapes.h"

#include <boost/algorithm/string.hpp>
#include <vector>

#include <PngImage.hpp>
#include <IOUtils.hpp>

#include "UnitSyntaxEditorForm.h"
#include "UnitDataModelStateShape.h"
#include "UnitSettings.h"
#include "UnitStringEditorEx.h"
#include "UnitMain.h"
#include "UnitStateMachineUnit.h"
#include "UnitDialogFileNotFound.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitCustomUtils.h"
#include "UnitExecutableShapes.h"
#include "UrlUtils.h"
#include "UnitTreeEditorUtils.h"
#include "VectorDrawUtils.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterExternCommunicationShapes() {
#pragma startup RegisterExternCommunicationShapes

	Classes::RegisterClassA(__classid(TSendShape));
	Classes::RegisterClassA(__classid(TCancelShape));
	Classes::RegisterClassA(__classid(TInvokeShape));
	Classes::RegisterClassA(__classid(TFinalizeShape));
}

UnicodeString SendTypeToSetValueType(const UnicodeString &sSendType) {
	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	AStringListPtr->Values["avia_protocol"] = "1";
	AStringListPtr->Values["zmq_protocol"] = "2";
	AStringListPtr->Values["avia_plugin"] = "3";
	AStringListPtr->Values["multicast_protocol"] = "4";

	return AStringListPtr->Values[sSendType];
}

UnicodeString SetValueTypeToSendType(const UnicodeString &sSetValueType) {
	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	AStringListPtr->Values["1"] = "avia_protocol";
	AStringListPtr->Values["2"] = "zmq_protocol";
	AStringListPtr->Values["3"] = "avia_plugin";
	AStringListPtr->Values["4"] = "multicast_protocol";

	return AStringListPtr->Values[sSetValueType];
}

// ---------------------------------------------------------------------------
// ----------------------- TSendTypePropEditor -------------------------------
// ---------------------------------------------------------------------------
Lmdinspropinsp::TLMDPropAttrs __fastcall TSendTypePropEditor::GetAttrs(void) {
	return TLMDStringPropEditor::GetAttrs() << praValueList;
}

void __fastcall TSendTypePropEditor::GetValues(Classes::TStrings* AValues) {
	// расширения выходящие за рамки стандарта SCXML
	if (SettingsData->AviaExtensionEnabled) {
		AValues->Add("avia_protocol");
		AValues->Add("zmq_protocol");
		AValues->Add("avia_state_machine");
		AValues->Add("global_data");
		AValues->Add("avia_plugin");
		AValues->Add("multicast_protocol");
	}

	AValues->Add("http://www.w3.org/TR/scxml/#SCXMLEventProcessor");
	AValues->Add("http://www.w3.org/TR/scxml/#BasicHTTPEventProcessor");
}

// ---------------------------------------------------------------------------
// -------------------------- TSendShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TSendShape::TSendShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FEvent(L""), FEventExpr(L""), FTarget(L""),
FTargetExpr(L""), FType(L""), FTypeExpr(L""), FId(L""), FIdLocation(L""), FDelay(L""), FDelayExpr(L""), FNameList(L""),
FProtocolControlBinding(0) {

	FAvailableTypes.insert(sctParam);
	FAvailableTypes.insert(sctContent);

	if (SettingsData->AviaExtensionEnabled) {
		FConvertibleTypes.insert(sctSetValue);
	}
	FConvertibleTypes.insert(sctRaise);

	this->FStateAttrMap["Event"] = false;
	this->FStateAttrMap["Eventexpr"] = false;
	this->FStateAttrMap["Target"] = false;
	this->FStateAttrMap["Targetexpr"] = false;
	this->FStateAttrMap["Type"] = false;
	this->FStateAttrMap["Typeexpr"] = false;
	this->FStateAttrMap["Id"] = false;
	this->FStateAttrMap["Idlocation"] = false;
	this->FStateAttrMap["Delay"] = false;
	this->FStateAttrMap["Delayexpr"] = false;
	this->FStateAttrMap["Namelist"] = false;

	FTypeSettings[L"avia_protocol"] = boost::make_tuple(clMaroon, TFontStyles() << fsBold);
	FTypeSettings[L"zmq_protocol"] = boost::make_tuple(clTeal, TFontStyles() << fsBold);
	FTypeSettings[L"global_data"] = boost::make_tuple(clPurple, TFontStyles() << fsBold);
	FTypeSettings[L"avia_plugin"] = boost::make_tuple(clNavy, TFontStyles() << fsBold);
	FTypeSettings[L"multicast_protocol"] = boost::make_tuple(clPurple, TFontStyles() << fsBold);
	FTypeSettings[L"http://www.w3.org/TR/scxml/#BasicHTTPEventProcessor"] = boost::make_tuple(clOlive, TFontStyles() << fsBold);

	FProtocolControlBinding = new TProtocolControlBinding(this);
	FProtocolControlBinding->BindTypes = TProtocolBindTypes() << pbtSupplier;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngSend");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
__fastcall TSendShape::~TSendShape() {
	if (FProtocolControlBinding) {
		delete FProtocolControlBinding;
		FProtocolControlBinding = NULL;
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSendShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Event") {
		return //
		"A string indicating the name of message being generated.\n" //
		"MUST NOT occur with 'eventexpr'.\n" //
		"If the type is <B>http://www.w3.org/TR/scxml/#SCXMLEventProcessor</B>, either <B>this attribute or 'eventexpr'</B> MUST BE present."
		//
		;
	}

	if (sPropName == L"Eventexpr") {
		return //
		"A dynamic alternative to 'event'.\n" //
		"If this attribute is present, the SCXML Processor must evaluate it " //
		"when the parent &lt;send&gt; element is evaluated and treat the result as if it had been entered as the value of 'event'.\n" //
		"MUST NOT occur with 'event'.\n" //
		"If the type is <B>http://www.w3.org/TR/scxml/#SCXMLEventProcessor</B>, either <B>this attribute or 'event'</B> MUST BE present.\n"
		//
		;
	}

	if (sPropName == L"Type" || sPropName == L"Typeexpr") {
		std::auto_ptr<TStringList>AHelpListPtr(new TStringList());
		if (SettingsData->AviaExtensionEnabled) {

			AHelpListPtr->Add("<B>avia_protocol</B> - value will be send to the AVIA protocol. " //
				"Value in the field <B>event</B> or <B>eventexpr</B> must be associated with <B>TProtocolControlBinding</B> previously!!!");
			AHelpListPtr->Add("<B>zmq_protocol</B> - value will be send to the ZMQ protocol");
			AHelpListPtr->Add(
				"<B>avia_state_machine</B> - value in the field <B>event</B> or <B>eventexpr</B> will be send to the Scxml App. " //
				"Possible values: SHUTDOWN, LOAD, ONLINE, TRAININGINIT, TRAININGPAUSED, TRAININGREADY, TRAININGACTIVE, DEBRIEFINIT, DEBRIEFPAUSED, DEBRIEFREADY, DEBRIEFACTIVE, STOPPED, ERROR");
			AHelpListPtr->Add
				("<B>global_data</B> - value in the field <B>event</B> or <B>eventexpr</B> will be put into GlobalData storage");
			AHelpListPtr->Add("<B>avia_plugin</B> - value in the field <B>event</B> or <B>eventexpr</B> " //
				"will be put on input of Avia custom plugin");
			AHelpListPtr->Add("<B>multicast_protocol</B> - value will be send to the Multicast protocol.<br>" //
				"<b>Notice:</b> You must declare 'Udpenvvariable' attribute<br>" //
				"and set multicast settings if format: GROUP:PORT>INTERFACE<br>" //
				"For example:225.5.5.5:11000>10.10.10.255");

		}

		AHelpListPtr->Add(
			"<B>http://www.w3.org/TR/scxml/#SCXMLEventProcessor</B> - Target is an SCXML session. The transport mechanism is platform-specific.");

		AHelpListPtr->Add("<B>http://www.w3.org/TR/scxml/#BasicHTTPEventProcessor</B> - Target is a URL. Data is sent via HTTP POST.");

		for (int i = 0; i < AHelpListPtr->Count; i++) {
			AHelpListPtr->Strings[i] = UnicodeString(i + 1) + L") " + AHelpListPtr->Strings[i];
		}

		return AHelpListPtr->Text;

	}
	if (sPropName == L"Target" || sPropName == L"Targetexpr") {
		UnicodeString sOut = "#_internal\n" //
		"#_scxml_sessionid\n" //
		"#_parent\n" //
		"#_invokeid\n";

		return sOut;
	}
	if (sPropName == L"Delay" || sPropName == L"Delayexpr") {
		return //
		"Indicates how long the processor should wait before dispatching the message.\n" //
		"<B>'delay'</B> MUST NOT OCCUR with <B>'delayexpr'</B> or when the attribute <B>'target'</B> has the value <B>'#_internal'</B>\n" //
		"Format:\n" "<B>#.#s</B> - seconds\n" //
		"<B>#.#ms</B> - miliseconds\n" //
		"Example: <B>'5s'</B> or <B>'5000ms'</B>"; //
	}
	if (sPropName == L"Id") {
		return //
		"A string literal to be used as the identifier for this instance of &lt;send&gt;.\n" //
		"MUST NOT occur with 'idlocation'." //
		;
	}
	if (sPropName == L"Idlocation") {
		return //
		"Any location expression evaluating to a data model location in which a system-generated id can be stored.\n" //
		"MUST NOT occur with 'id'." //
		;
	}
	if (sPropName == L"Namelist") {
		return //
		"A space-separated list of one or more data model locations to be included as attribute/value pairs with the message. " //
		"(The name of the location is the attribute and the value stored at the location is the value.) \n" //
		"Must not be specified in conjunction with &lt;content&gt; element." //
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TSendShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		std::auto_ptr<TStringList>AUnicodeSyntaxList(new TStringList());
		AUnicodeSyntaxList->CaseSensitive = true;
		AUnicodeSyntaxList->CommaText = L"Eventexpr,Targetexpr,Typeexpr,Delayexpr";

		std::auto_ptr<TStringList>AUnicodeLineListPtr(new TStringList());
		AUnicodeLineListPtr->CaseSensitive = true;
		AUnicodeLineListPtr->CommaText = L"Target,Id,Delay,Namelist";

		if (sPropName == L"ProtocolControlBinding") {
			AEditorClass = __classid(TProtocolControlBindingPropertyEditor);
			return;
		}
		if (AUnicodeSyntaxList->IndexOf(sPropName) != -1) {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
		if (sPropName == L"Type") {
			AEditorClass = __classid(TSendTypePropEditor);
			return;
		}
		if (sPropName == L"Event") {
			AEditorClass = __classid(TEventIDPropEditor);
			return;
		}
		if (AUnicodeLineListPtr->IndexOf(sPropName) != -1) {
			AEditorClass = __classid(TLineEditStringPropEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSendShape::GetOutValue() {
	UnicodeString sDescription = FEvent.IsEmpty() ? FEventExpr : FEvent;

	UnicodeString sDelay = Delay.IsEmpty() ? Delayexpr : Delay;
	if (!sDelay.IsEmpty()) {
		sDescription = sDescription + "[" + sDelay + "]";
	}

	UnicodeString sID = Id.IsEmpty() ? Idlocation : Id;
	if (!sID.IsEmpty()) {
		sDescription = sDescription + "[" + sID + "]";
	}

	if ((Type == "avia_protocol" || Type == "zmq_protocol" || Type == "multicast_protocol") && ProtocolControlBinding->Active) {
		sDescription = sDescription + "[" + ProtocolControlBinding->ControlName + "]";
	}
	if (!Target.IsEmpty()) {
		sDescription = sDescription + "[" + Target + "]";
	}
	else if (!Targetexpr.IsEmpty()) {
		sDescription = sDescription + "[" + Targetexpr + "]";
	}

	std::vector<std::wstring>VecRightPart;
	if (!this->Namelist.IsEmpty()) {
		VecRightPart.push_back(this->Namelist.c_str());
	}

	for (int i = 0; i < this->Children->Count; i++) {
		if (this->Children->Items[i]->InheritsFrom(__classid(TContentShape))) {
			TContentShape *AContentShape = dynamic_cast<TContentShape*>(this->Children->Items[i]);
			if (AContentShape && !AContentShape->OutValue.IsEmpty()) {
				VecRightPart.push_back(AContentShape->OutValue.c_str());
			}
		}
		else if (this->Children->Items[i]->InheritsFrom(__classid(TParamShape))) {
			TParamShape *AParamShape = dynamic_cast<TParamShape*>(this->Children->Items[i]);
			if (AParamShape && !AParamShape->OutValue.IsEmpty()) {
				VecRightPart.push_back(AParamShape->OutValue.c_str());
			}
		}

	}

	std::vector<std::wstring>VecOut;
	VecOut.push_back(sDescription.c_str());
	if (!VecRightPart.empty()) {
		VecOut.push_back(L"[" + boost::algorithm::join(VecRightPart, L";") + L"]");
	}

	return boost::algorithm::join(VecOut, L"=").c_str();
}

// ---------------------------------------------------------------------------
TColor __fastcall TSendShape::GetNormalFontColor(void) {
	TypeSettings::iterator it = FTypeSettings.find(Type);
	return it != FTypeSettings.end() ? it->second.get<typesColor>() : TChildScxmlBaseShape::GetNormalFontColor();
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TSendShape::GetNormalFontStyle() {
	const TFontStyles ANormalFontStyles = TChildScxmlBaseShape::GetNormalFontStyle();
	TypeSettings::iterator it = FTypeSettings.find(Type);
	return it != FTypeSettings.end() ? it->second.get<typesFontStyles>() : ANormalFontStyles;
}

// ---------------------------------------------------------------------------
void __fastcall TSendShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			if (TSendShape * ASendShape = dynamic_cast<TSendShape*>(Source)) {
				FEvent = ASendShape->Event;
				FEventExpr = ASendShape->Eventexpr;
				FTarget = ASendShape->Target;
				FTargetExpr = ASendShape->Targetexpr;
				FType = ASendShape->Type;
				FTypeExpr = ASendShape->Typeexpr;
				FId = ASendShape->Id;
				FIdLocation = ASendShape->Idlocation;
				FDelay = ASendShape->Delay;
				FDelayExpr = ASendShape->Delayexpr;

				FNameList = ASendShape->Namelist;

				FProtocolControlBinding->Assign(ASendShape->FProtocolControlBinding);

				UpdateSimpleText();
			}
			/* ПРЕОБРАЗОВАНИЯ ИЗ ДРУГИХ ТИПОВ */
			else if (TSetValueShape * ASetValueShape = dynamic_cast<TSetValueShape*>(Source)) {
				this->FEvent = ASetValueShape->Target;
				this->FEventExpr = ASetValueShape->Targetexpr;

				this->FType = SetValueTypeToSendType(ASetValueShape->Type);
				if (!ASetValueShape->Typeexpr.IsEmpty()) {
					this->FTypeExpr = "!REQUIRE CONVERSION!=" + ASetValueShape->Typeexpr;
					WLOG_ERROR(L"Attr 'Typeexpr' with value '%s' does not support conversion!", ASetValueShape->Typeexpr.c_str());
				}

				if (!ASetValueShape->Path.IsEmpty()) {
					WLOG_ERROR(L"Attr 'Path' with value '%s' does not support conversion!", ASetValueShape->Path.c_str());
				}

				if (!ASetValueShape->Pathexpr.IsEmpty()) {
					WLOG_ERROR(L"Attr 'Pathexpr' with value '%s' does not support conversion!", ASetValueShape->Pathexpr.c_str());
				}

				const UnicodeString sValue = ASetValueShape->Value.IsEmpty() ? ASetValueShape->Valueexpr : ASetValueShape->Value;

				if (!sValue.IsEmpty()) {
					TContentShape * AContentShape = NULL;
					for (int i = 0; i < this->Children->Count; i++) {
						AContentShape = dynamic_cast<TContentShape*>(this->Children->Items[i]);
						if (AContentShape)
							break;
					}

					if (!AContentShape) {
						AContentShape = dynamic_cast<TContentShape*>(this->AddStateChild(sctContent, false));
					}

					if (!AContentShape)
						throw Exception("Can not create <content> element in <send>!");

					AContentShape->XMLText->Text = sValue;
				}

				FProtocolControlBinding->Assign(ASetValueShape->ProtocolControlBinding);

				UpdateSimpleText();
			}
			else if (TRaiseShape * ARaiseShape = dynamic_cast<TRaiseShape*>(Source)) {
				FEvent = ARaiseShape->Event;

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
std::vector<TDefaultEditorPair>TSendShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Event", boost::bind(&TSendShape::EditEventIDLineProperty, this, L"Event")));
	AVecEditors.push_back(std::make_pair(L"Eventexpr", boost::bind(&TSendShape::EditUnicodeStringSyntaxProperty, this, L"Eventexpr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TSendShape::SetDelay(UnicodeString val) {
	try {
		if (!val.IsEmpty()) {
			if (!FDelayExpr.IsEmpty())
				throw Exception("'Delayexpr'=[" + FDelayExpr + "]");

			if (FTarget == "#_internal")
				throw Exception("'Target'=[" + FTarget + "]");
		}

		FDelay = val;
		UpdateSimpleText();
	}
	catch(Exception * E) {
		WLOG_ERROR(L"'Delay'=[%s] must not occur with %s", val.c_str(), E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSendShape::SetDelayExpr(UnicodeString val) {
	try {
		if (!val.IsEmpty()) {
			if (!FDelay.IsEmpty())
				throw Exception("'Delay'=[" + FDelay + "]");

			if (FTarget == "#_internal")
				throw Exception("'Target'=[" + FTarget + "]");
		}

		FDelayExpr = val;
		UpdateSimpleText();
	}
	catch(Exception * E) {
		WLOG_ERROR(L"'Delayexpr'=[%s] must not occur with %s", val.c_str(), E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSendShape::SetTarget(UnicodeString val) {
	try {
		if (!val.IsEmpty()) {
			if (!FTargetExpr.IsEmpty())
				throw Exception("'Targetexpr'=[" + FDelayExpr + "]");

			if (val == "#_internal") {
				if (!FDelay.IsEmpty())
					throw Exception("'Delay'=[" + FDelay + "]");

				if (!FDelayExpr.IsEmpty())
					throw Exception("'Delayexpr'=[" + FDelayExpr + "]");
			}
		}

		FTarget = val;
		UpdateSimpleText();
	}
	catch(Exception * E) {
		WLOG_ERROR(L"'Target'=[%s] must not occur with %s", val.c_str(), E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSendShape::ValidateChild(TScxmlBaseShape *AChild) {
	/* inherited */
	TChildScxmlBaseShape::ValidateChild(AChild);

	if (AChild) {
		switch(AChild->StateChildType) {
		case sctContent: {
				if (TParamShape * AParamShape = Editorutils::FindShapeByType<TParamShape>(this))
					throw EScxmlBaseShapeException(AParamShape, L"Conflicts with <content>", L"<content> must not occur with <param>!");

				if (!Namelist.IsEmpty())
					throw EScxmlBaseShapeException(AChild, L"Conflicts with <send> 'namelist'",
					L"<content> must not occur with 'namelist'!");
			}break;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSendShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawSend(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// -------------------------- TCancelShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TCancelShape::TCancelShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FSendId(L""), FSendIdExpr(L"") {
	this->FStateAttrMap["Sendid"] = false;
	this->FStateAttrMap["Sendidexpr"] = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngCancel");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TCancelShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TCancelShape * ASendIdShape = dynamic_cast<TCancelShape*>(Source);
			if (ASendIdShape) {
				FSendId = ASendIdShape->FSendId;
				FSendIdExpr = ASendIdShape->FSendIdExpr;

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
UnicodeString __fastcall TCancelShape::GetOutValue() {
	return Sendid.IsEmpty() ? Sendidexpr : Sendid;
}

// ---------------------------------------------------------------------------
void __fastcall TCancelShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();
		if (sPropName == L"Sendidexpr") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
		if (sPropName == L"Sendid") {
			AEditorClass = __classid(TSendIDPropEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TCancelShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Sendid") {
		return //
		"The ID of the event(s) to be cancelled.\n" //
		"If multiple delayed events have this sendid, the Processor will cancel them all.\n" //
		"MUST NOT occur with 'sendidexpr'." //
		;
	}

	if (sPropName == L"Sendidexpr") {
		return //
		"A dynamic alternative to 'sendid'.\n" //
		"If this attribute is present, the SCXML Processor must evaluate it " //
		"when the parent &lt;cancel&gt; element is evaluated and treat the result as if it had been entered as the value of 'sendid'.\n" //
		"MUST NOT occur with 'sendid'." //
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TCancelShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Sendid", boost::bind(&TCancelShape::EditSendIDLineProperty, this, L"Sendid")));
	AVecEditors.push_back(std::make_pair(L"Sendidexpr", boost::bind(&TCancelShape::EditUnicodeStringSyntaxProperty, this, L"Sendidexpr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void TCancelShape::EditSendIDLineProperty(const UnicodeString &sPropName) {
	if (__EditSendIDLineProperty(this, sPropName)) {
		if (StateMachineEditor) {
			StateMachineEditor->MarkModified(1, UnicodeString().sprintf(L"Edit '%s':[%s]", sPropName.c_str(),
					Customutils::ClippedText(this->SimpleText, 40).c_str()), false);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TCancelShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawCancel(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// ------------------------- TInvokeSrcDialogEditorImpl ----------------------
// ---------------------------------------------------------------------------

UnicodeString __fastcall __EditInvokeSrc(TInvokeShape *AShape) {

	UnicodeString sValue = AShape->Src;

	if (AShape && AShape->StateMachineEditorUnit) {

		const UnicodeString sLocalSrc = AShape->LocalSrc;

		UnicodeString sFileName = TPath::IsPathRooted(sLocalSrc) ? sLocalSrc : TPath::Combine(AShape->StateMachineEditorUnit->Path,
			sLocalSrc);

		if (sValue.IsEmpty() || !FileExists(sFileName)) {

			const UnicodeString sText = L"Invoke SCXML source [" + (sValue.IsEmpty() ? sValue : sFileName) + "] is not found!\n" //
			"'Find' - will find a new unit\n\n" //
			"P.S. Only SCXML unit support!";

			std::auto_ptr<TDialogFileNotFound>ADialogFileNotFoundPtr(new TDialogFileNotFound(AShape->Owner));
			ADialogFileNotFoundPtr->LabelText->Caption = sText;
			ADialogFileNotFoundPtr->BtnCreate->Visible = false;
			switch(ADialogFileNotFoundPtr->ShowModal()) {
			case CUSTOM_MODAL_RESULT_FIND: {
					UnicodeString sTempFileName = sFileName;
					if (PromptForFileName(sTempFileName, L"SCXML files(*.scxml)|*.scxml", "scxml", L"Find invoke src unit",
							ExtractFileDir(AShape->StateMachineEditorUnit->Path), false)) {

						sFileName = sTempFileName;
					}

					if (!FileExists(sFileName))
						throw Exception("File <" + sFileName + "> does not exist!");

					sValue = ExtractRelativePath(AShape->StateMachineEditorUnit->Path, sFileName);
				}break;
			default:
				throw Exception("Cancelled by user!");
			}

		}

		const UnicodeString sGivenExtension = ExtractFileExt(sValue);
		if (!SameText(sGivenExtension, ".scxml"))
			throw Exception("Extension <" + sGivenExtension + "> in <" + sValue + "> is not supported for GUI edit! Use only '.scxml'!");

		GlobalPostAddOrOpenExistingUnit(AShape->StateMachineEditorUnit, sFileName);
	}
	return sValue;
}

/* PROP EDITORS */
class TInvokeSrcDialogEditorImpl : public TLMDStringPropEditor {
protected:

	TLMDPropAttrs __fastcall GetAttrs() {
		return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
	}

	void __fastcall Edit(void) {
		try {
			if (Propinspext::IsWritablePropertyOfPropEditor(this)) {

				const UnicodeString sValue = this->GetStrValue(0);

				TInvokeShape * AShape = dynamic_cast<TInvokeShape*>(this->GetInstance(0));
				if (AShape) {
					const UnicodeString sNewValue = __EditInvokeSrc(AShape);
					if (sNewValue != sValue) {
						this->SetStrValue(sNewValue);
					}
				}
			}
		}
		catch(Exception * E) {
			WLOG_ERROR(L"Edit Invoke Src> %s", E->Message.c_str());
		}
	}

};

// ---------------------------------------------------------------------------
// -------------------------- TInvokeShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TInvokeShape::TInvokeShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FSrc(L""), FSrcExpr(L""), FType(L""),
FTypeExpr(L""), FId(L""), FIdLocation(L""), FAutoForward(false), FNameList(L""), FDisableFlatBuild(false) {

	FAvailableTypes.insert(sctParam);
	FAvailableTypes.insert(sctFinalize);
	FAvailableTypes.insert(sctContent);

	this->FStateAttrMap["Type"] = false;
	this->FStateAttrMap["Typeexpr"] = false;
	this->FStateAttrMap["Src"] = false;
	this->FStateAttrMap["Srcexpr"] = false;
	this->FStateAttrMap["Id"] = false;
	this->FStateAttrMap["Idlocation"] = false;
	this->FStateAttrMap["Namelist"] = false;
	this->FStateAttrMap["Autoforward"] = false;
	this->FStateAttrMap["DisableFlatBuild"] = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngInvoke");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TInvokeShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		std::auto_ptr<TStringList>AUnicodeSyntaxList(new TStringList());
		AUnicodeSyntaxList->CaseSensitive = true;
		AUnicodeSyntaxList->CommaText = L"Srcexpr,Typeexpr";

		std::auto_ptr<TStringList>AUnicodeLineList(new TStringList());
		AUnicodeLineList->CaseSensitive = true;
		AUnicodeLineList->CommaText = L"Type,Id,Namelist";

		if (AUnicodeSyntaxList->IndexOf(sPropName) != -1) {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}

		if (AUnicodeLineList->IndexOf(sPropName) != -1) {
			AEditorClass = __classid(TLineEditStringPropEditor);
			return;
		}

		if (sPropName == L"Src") {
			AEditorClass = __classid(TInvokeSrcDialogEditorImpl);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
void __fastcall TInvokeShape::OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor) {
	if (AItem->PropName == L"DisableFlatBuild") {
		AColor = clTeal;
	}
	else {
		TChildScxmlBaseShape::OnGetPropCaptionColor(Sender, AItem, AColor);
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TInvokeShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Type") {
		UnicodeString sHTML = //
		"A URI specifying the type of the external service.\n" //
		"http://www.w3.org/TR/scxml/,\n" //
		"http://www.w3.org/TR/ccxml/,\n" //
		"http://www.w3.org/TR/voicexml30/,\n" //
		"http://www.w3.org/TR/voicexml21/\n" //
		"plus other platform-specific values.\n" //
		"MUST NOT occur with 'typeexpr'.\n" //
		;
		if (SettingsData->AviaExtensionEnabled) {
			sHTML = sHTML + //
			"<B>AviaModelPlugin</B> - plugin with strict inputs and outputs\n" //
			"<B>AviaScxmlPlugin</B> - scxml based plugin\n";
		}

		return sHTML;
	}

	if (sPropName == L"Typeexpr") {
		return //
		"A dynamic alternative to 'type'.\n" //
		"If this attribute is present, the SCXML Processor must evaluate it " //
		"when the parent &lt;invoke&gt; element is evaluated and treat the result as if it had been entered as the value of 'type'.\n" //
		"MUST NOT occur with 'type'." //
		;
	}

	if (sPropName == L"Src") {
		return //
		"A URI to be passed to the external service.\n" //
		"MUST NOT occur with the 'srcexpr' attribute or the &lt;content&gt; element." //
		;
	}

	if (sPropName == L"Srcexpr") {
		return //
		"A dynamic alternative to 'src'.\n" //
		"If this attribute is present, the SCXML Processor must evaluate it " //
		"when the parent &lt;invoke&gt; element is evaluated and treat the result as if it had been entered as the value of 'src'.\n" //
		"MUST NOT occur with the 'src' attribute or the &lt;content&gt; element." //
		;
	}

	if (sPropName == L"Id") {
		return //
		"A string literal to be used as the identifier for this instance of &lt;invoke&gt;.\n" //
		"MUST NOT occur with the 'idlocation' attribute." //
		;
	}

	if (sPropName == L"Idlocation") {
		return //
		"Any data model expression evaluating to a data model location.\n" //
		"MUST NOT occur with the 'id' attribute." //
		;
	}

	if (sPropName == L"Namelist") {
		return //
		"A space-separated list of one or more data model locations to be passed " //
		"as attribute/value pairs to the invoked service. " //
		"(The name of the location is the attribute and the value stored at the location is the value.)\n" //
		"MUST NOT occur with the &lt;param&gt; element." //
		;
	}

	if (sPropName == L"Autoforward") {
		return //
		"A flag indicating whether to forward events to the invoked process." //
		;
	}

	if (sPropName == L"DisableFlatBuild") {
		return //
		"If <b>'DisableFlatBuild==false'</b> then <b>'src'</b> is loaded into scxml during flat build\n" //
		"otherwise it remains as is";
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TInvokeShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TInvokeShape * AInvokeShape = dynamic_cast<TInvokeShape*>(Source);
			if (AInvokeShape) {
				FSrc = AInvokeShape->FSrc;
				FSrcExpr = AInvokeShape->FSrcExpr;
				FType = AInvokeShape->FType;
				FTypeExpr = AInvokeShape->FTypeExpr;
				FId = AInvokeShape->FId;
				FIdLocation = AInvokeShape->FIdLocation;
				FAutoForward = AInvokeShape->FAutoForward;
				FNameList = AInvokeShape->FNameList;
				FDisableFlatBuild = AInvokeShape->FDisableFlatBuild;

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
TColor __fastcall TInvokeShape::GetNormalFontColor(void) {
	return Autoforward ? clMaroon : TChildScxmlBaseShape::GetNormalFontColor();
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TInvokeShape::GetNormalFontStyle() {
	return Autoforward ? (TChildScxmlBaseShape::GetNormalFontStyle() << fsBold) : TChildScxmlBaseShape::GetNormalFontStyle();
}

// ---------------------------------------------------------------------------
TColor __fastcall TInvokeShape::GetNormalColor(void) {
	return DisableFlatBuild ? COLOR_DISABLE_FLAT_BUILD : TChildScxmlBaseShape::GetNormalColor();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TInvokeShape::GetOutValue() {
	UnicodeString sOut = L"";

	if (!Src.IsEmpty()) {
		sOut = Src;
	}
	else if (!Srcexpr.IsEmpty()) {
		sOut = Srcexpr;
	}

	if (!Id.IsEmpty()) {
		sOut = sOut + "=" + Id;
	}
	else if (!Idlocation.IsEmpty()) {
		sOut = sOut + "=" + Idlocation;
	}

	if (!Namelist.IsEmpty()) {
		sOut = sOut + " [" + StringReplace(Namelist, " ", "; ", TReplaceFlags() << rfReplaceAll) + "]";
	}

	return sOut;
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TInvokeShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Src", boost::bind(&TInvokeShape::EditSrc, this)));
	AVecEditors.push_back(std::make_pair(L"Srcexpr", boost::bind(&TInvokeShape::EditUnicodeStringSyntaxProperty, this, L"Srcexpr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void TInvokeShape::EditSrc(void) {
	const UnicodeString sNewValue = __EditInvokeSrc(this);
	if (sNewValue != this->Src) {

		this->Src = sNewValue;

		if (StateMachineEditor) {
			StateMachineEditor->MarkModified(1, UnicodeString().sprintf(L"Edit 'Src':[%s]", //
					Customutils::ClippedText(this->SimpleText, 40).c_str()), false);
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TInvokeShape::GetLocalSrc(void) {
	return Url::UrlToLocalFile(this->Src);
}

// ---------------------------------------------------------------------------
void __fastcall TInvokeShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawInvoke(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TFinalizeShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TFinalizeShape::TFinalizeShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes.insert(STATE_TYPES_EXECUTABLE_CONTENT.begin(), STATE_TYPES_EXECUTABLE_CONTENT.end());
	FAvailableTypes.erase(sctRaise);
	FAvailableTypes.erase(sctSend);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngFinalize");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

void __fastcall TFinalizeShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			// TFinalizeShape * AElseShape = dynamic_cast<TFinalizeShape*>(Source);
			// if (AElseShape) {
			//
			// }
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TFinalizeShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawFinalize(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
