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

#include "UnitTriggerShape.h"

#include <memory>
#include <PngImage.hpp>
#include "UnitSyntaxEditorForm.h"
 #include "UnitStringEditorEx.h"
#include "VectorDrawUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// -------------------------- TTriggerShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TTriggerShape::TTriggerShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes = TStateChildTypes();

	FContentTrigger = new TContentTrigger(this);
	FParams = new TParamTriggerItems(this);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngTrigger");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
_fastcall TTriggerShape::~TTriggerShape() {
	if (FContentTrigger) {
		delete FContentTrigger;
	}
	if (FParams) {
		delete FParams;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTriggerShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {

		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"Event") {
			AEditorClass = __classid(TEventIDPropEditor);
			return;
		}

		if (sPropName == L"Params") {
			AEditorClass = __classid(TParamTriggersPropEditor);
			return;
		}

		if (sPropName == L"ContentTrigger") {
			AEditorClass = __classid(TContentPropEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
void __fastcall TTriggerShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TTriggerShape * AProtocolShape = dynamic_cast<TTriggerShape*>(Source);
			if (AProtocolShape) {
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
TFontStyles __fastcall TTriggerShape::GetNormalFontStyle() {
	return this->ContentTrigger->Enabled ? (TFontStyles() << fsBold) : TChildScxmlBaseShape::GetNormalFontStyle();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TTriggerShape::GetOutValue() {
	std::auto_ptr<TStringList>AParamsListPtr(new TStringList());
	if (Params->Enabled) {
		for (int i = 0; i < Params->Count; i++) {
			AParamsListPtr->Add(Params->Items[i]->DisplayName);
		}
	}
	return Event + (AParamsListPtr->Count ? (" " + AParamsListPtr->CommaText) : UnicodeString(L""));
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TTriggerShape::GetEvent(void) {
	return FContentTrigger->Event;
}

// ---------------------------------------------------------------------------
void __fastcall TTriggerShape::SetEvent(UnicodeString val) {
	FContentTrigger->Event = val;

	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
bool __fastcall TTriggerShape::GetEnabled(void) {
	return FContentTrigger->Enabled;
}

// ---------------------------------------------------------------------------
void __fastcall TTriggerShape::SetEnabled(bool val) {
	FContentTrigger->Enabled = val;

	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TTriggerShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Event") {
		return //
		"Specifies the name of the <b>event</b> for trigger.\n" //
		"This will be matched against the <B>'event'</B> attribute of transitions.";
	}

	if (sPropName == L"Enabled") {
		return //
		"If is set to <b>'true'</b> then trigger will be created in runtime\n" //
		;
	}

	if (sPropName == L"ContentTrigger") {
		return //
		"The trigger is used for sending events to the SCXML Processor\n"//
		"P.S. Do not forget to <b>enable it for use!</b>";
	}

	if (sPropName == L"Params") {
		return //
		"Params of the event\n"//
		"For example: you are planning to send <b>'_event.data.nParam==1'</b>\n"//
		"In this case you should create a param with the name <b>'nParam'</b> and set the expr <b>'1'</b>\n"//
		"P.S. <b>ContentTrigger</b> - should be <b>enabled</b> to activate trigger itself\n" //
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TTriggerShape::SetContentTrigger(TContentTrigger *val) {
	FContentTrigger->Assign(val);

	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TTriggerShape::SetParams(TParamTriggerItems *val) {
	FParams->Assign(val);

	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TTriggerShape::DefaultEditProperty(void) {

	EditEventIDLineProperty(L"Event");

}

// ---------------------------------------------------------------------------
void __fastcall TTriggerShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawTrigger(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
