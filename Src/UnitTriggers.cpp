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

#include "UnitTriggers.h"

#include <memory>

#include "UnitStringEditorEx.h"
#include "UnitStateMachineConnection.h"
#include "UnitProjectTriggers.h"
#include "UnitFrameTrigger.h"
#include "UnitSyntaxEditorForm.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

__fastcall TContentTrigger::TContentTrigger(TObject *AParent) : FParent(AParent), FExpr(L""), FTriggerType(cttDefault), FEnabled(false),
FAppearance(ctaButton), FValues(new TStringList), FXMLText(new TSyntaxString(0, L"")) {
	FMin = L"";
	FMax = L"";
	FOn = L"";
	FOff = L"";
	FEvent = L"";
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TContentTrigger::GetEvent(void) {
	if (FEvent.IsEmpty()) {
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(FParent);
		if (AConnection && !AConnection->Event.IsEmpty()) {
			return AConnection->Event;
		}
		TProjectTriggerItem *AItem = dynamic_cast<TProjectTriggerItem*>(FParent);
		if (AItem && !AItem->Event.IsEmpty()) {
			return AItem->Event;
		}
		TFrameTrigger *AFrameTrigger = dynamic_cast<TFrameTrigger*>(FParent);
		if (AFrameTrigger && !AFrameTrigger->EventName.IsEmpty()) {
			return AFrameTrigger->EventName;
		}
		TProtocolBindingShape *ABindingShape = dynamic_cast<TProtocolBindingShape*>(FParent);
		if (ABindingShape && !ABindingShape->ProtocolControlBinding->ScxmlName.IsEmpty()) {
			return ABindingShape->ProtocolControlBinding->ScxmlName;
		}
	}

	return FEvent;
}

//---------------------------------------------------------------------------
void __fastcall TContentTrigger::SetEnabled(bool val) {
	FEnabled = val;

	// дальше всё будет через RETURN !!!
	TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(FParent);
	if (AConnection) {
		AConnection->UpdateAppearance();
		return;
	}
	TProtocolBindingShape *ABindingShape = dynamic_cast<TProtocolBindingShape*>(FParent);
	if (ABindingShape) {
		ABindingShape->UpdateSimpleText();
		return;
	}
}

//---------------------------------------------------------------------------
__fastcall TContentTrigger::~TContentTrigger() {
	if (FValues) {
		delete FValues;
		FValues = 0;
	}
	if (FXMLText) {
		delete FXMLText;
		FXMLText = 0;
	}
}

//---------------------------------------------------------------------------
void __fastcall TContentTrigger::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TContentTrigger * AContent = dynamic_cast<TContentTrigger*>(Source);
		if (AContent) {
			FEvent = AContent->FEvent;
			FExpr = AContent->FExpr;
			FXMLText->Assign(AContent->FXMLText);
			FTriggerType = AContent->FTriggerType;
			FAppearance = AContent->FAppearance;
			FEnabled = AContent->FEnabled;

			FMin = AContent->FMin;
			FMax = AContent->FMax;
			FOn = AContent->FOn;
			FOff = AContent->FOff;

			FValues->Assign(AContent->FValues);
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

//---------------------------------------------------------------------------
void __fastcall TContentTrigger::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == "XMLText") {
			AEditorClass = __classid(TSyntaxStringClassEditor);
		}
		else if (sPropName == L"Values") {
			AEditorClass = __classid(TMultiDisplayStringsEditorEx);
		}
		else if (sPropName == L"Expr") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
		}

	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TContentTrigger::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {

	if (sPropName == L"Event") {
		return //
		"Specifies the name of the event.\n" //
		;
	}
	if (sPropName == L"Enabled") {
		return //
		"Creates trigger in runtime.\n" //
		;
	}
	if (sPropName == L"Expr") {
		return //
		"<B>'_event.data'</B> will be filled by <B>'Expr'</B> value.\n" //
		;
	}
	if (sPropName == L"XMLText") {
		return //
		"<B>'_event.data'</B> will be filled by <B>'XMLText.Text'</B> value.\n" //
		"Mostly used for <B>multiline</B> strings.";
	}
	if (sPropName == L"TriggerType") {
		return //
		"Specifies the behaviour of trigger and will be sent to testing app\n" //
		"For example: if <B>'TriggerType==cttString'</B> then testing app <B>will not apply evaluation</B> before assignment of data\n";
	}
	if (sPropName == L"Appearance") {
		return //
		"Visual representation of trigger\n" //
		"Available controls: Button, CheckBox, TrackBar, Combo, Edit, Memo";
	}
	if (sPropName == L"Min") {
		return //
		"Used as <B>minimum</B> value for such controls as TrackBar etc.\n" //
		;
	}
	if (sPropName == L"Max") {
		return //
		"Used as <B>maximum</B> value for such controls as TrackBar etc.\n" //
		;
	}
	if (sPropName == L"On") {
		return //
		"Used for matching of CheckBox <B>checked</B> state\n" //
		;
	}
	if (sPropName == L"Off") {
		return //
		"Used for matching of CheckBox <B>unchecked</B> state\n" //
		;
	}
	if (sPropName == L"Values") {
		return //
		"Used for setting <B>ComboBox</B> values\n" //
		;
	}
	return L"";
}

// ---------------------------------------------------------------------------
// ----------------------- TParamTriggerItem --------------------------------
// ---------------------------------------------------------------------------
__fastcall TParamTriggerItem::TParamTriggerItem(TCollection* Collection) : TCollectionItem(Collection), FParamName(L""), FLocation(L""),
FExpr(L""), FTriggerType(cttDefault), FDisplayWidth(DISPLAY_PARAM_TRIGGER_WIDTH){
}

// ---------------------------------------------------------------------------
__fastcall TParamTriggerItem::~TParamTriggerItem() {

}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TParamTriggerItem::GetDisplayName(void) {
	return "Item " + UnicodeString(Index) + " {"+this->ParamName + ":"+this->Expr + "}";
}

// ---------------------------------------------------------------------------
void __fastcall TParamTriggerItem::Assign(TPersistent* Source) {
	if (Source) {
		TParamTriggerItem * AParamTriggerItem = dynamic_cast<TParamTriggerItem*>(Source);
		if (AParamTriggerItem) {

			FParamName = AParamTriggerItem->FParamName;
			FLocation = AParamTriggerItem->FLocation;
			FExpr = AParamTriggerItem->FExpr;
			FTriggerType = AParamTriggerItem->FTriggerType;
			FDisplayWidth = AParamTriggerItem->FDisplayWidth;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

//---------------------------------------------------------------------------
void __fastcall TParamTriggerItem::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass)
{
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"Expr") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
		}

	}
}

// ---------------------------------------------------------------------------
// --------------------------- COLLECTION ------------------------------------
// ----------------------- TParamTriggerItems -------------------------------
// ---------------------------------------------------------------------------
__fastcall TParamTriggerItems::TParamTriggerItems(TComponent *AOwner) : TOwnedCollection(AOwner, __classid(TParamTriggerItem)),
FEnabled(false) {

}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TParamTriggerItems::GetDisplayName(void) {
	std::auto_ptr<TStringList>AStringList(new TStringList());
	for (int i = 0; i < this->Count; i++) {
		AStringList->Add(this->Items[i]->ParamName);
	}
	return(this->Enabled ? "Enabled>" : "Disabled>") + AStringList->CommaText;
}

// ---------------------------------------------------------------------------
TParamTriggerItem* __fastcall TParamTriggerItems::Add(void) {
	return dynamic_cast<TParamTriggerItem*>(TOwnedCollection::Add());
}

// ---------------------------------------------------------------------------
TParamTriggerItem* __fastcall TParamTriggerItems::Insert(int Index) {
	return dynamic_cast<TParamTriggerItem*>(TOwnedCollection::Insert(Index));
}

// ---------------------------------------------------------------------------
TParamTriggerItem* __fastcall TParamTriggerItems::GetItem(int Index) {
	if (Index < 0 || Index >= this->Count)
		throw Exception("Item:[" + UnicodeString(Index) + "] is out of range [0..." + UnicodeString(this->Count) + "]");
	return dynamic_cast<TParamTriggerItem*>(TOwnedCollection::GetItem(Index));
}

// ---------------------------------------------------------------------------
void __fastcall TParamTriggerItems::SetItem(int Index, TParamTriggerItem* Value) {
	TOwnedCollection::SetItem(Index, Value);
}

// ---------------------------------------------------------------------------
void __fastcall TParamTriggerItems::Assign(TPersistent* Source) {
	if (Source) {
		TParamTriggerItems * ASeriesCompareItems = dynamic_cast<TParamTriggerItems*>(Source);
		if (ASeriesCompareItems) {
			TOwnedCollection::Assign(Source);

			// здесь применяем настройки непосредственно этого класса
			// ПРИМЕР:
			FEnabled = ASeriesCompareItems->FEnabled;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");

}
