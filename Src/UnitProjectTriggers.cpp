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

#include "UnitProjectTriggers.h"

#include <memory>

#include <TeeProcs.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// ----------------------- TProjectTriggerItem --------------------------------
// ---------------------------------------------------------------------------
__fastcall TProjectTriggerItem::TProjectTriggerItem(TCollection* Collection) : TCollectionItem(Collection), FEvent(L""),
FContentTrigger(new TContentTrigger(this)), FParams(new TParamTriggerItems(dynamic_cast<TComponent *>(this->GetOwner()))) {
}

// ---------------------------------------------------------------------------
__fastcall TProjectTriggerItem::~TProjectTriggerItem() {
	if (FContentTrigger) {
		delete FContentTrigger;
		FContentTrigger = 0;
	}
	if (FParams) {
		delete FParams;
		FParams = 0;
	}
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TProjectTriggerItem::GetDisplayName(void) {
	return "Item " + UnicodeString(Index) + " {"+this->Event + "}";
}

// ---------------------------------------------------------------------------
void __fastcall TProjectTriggerItem::Assign(TPersistent* Source) {
	if (Source) {
		TProjectTriggerItem * AParamTriggerItem = dynamic_cast<TProjectTriggerItem*>(Source);
		if (AParamTriggerItem) {
			FEvent = AParamTriggerItem->FEvent;
			FContentTrigger->Assign(AParamTriggerItem->FContentTrigger);
			FParams->Assign(AParamTriggerItem->FParams);
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TProjectTriggerItem::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		if (APropInfo->Name() == L"Params") {
			AEditorClass = __classid(TParamTriggersPropEditor);
		}
		else if (APropInfo->Name() == L"ContentTrigger") {
			AEditorClass = __classid(TContentPropEditor);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TProjectTriggerItem::SetEvent(UnicodeString val) {
	FEvent = val;
	if (!FEvent.IsEmpty()) {
		ContentTrigger->Event = FEvent;
	}
}

// ---------------------------------------------------------------------------
// --------------------------- COLLECTION ------------------------------------
// ----------------------- TProjectTriggerItems -------------------------------
// ---------------------------------------------------------------------------
__fastcall TProjectTriggerItems::TProjectTriggerItems(TComponent *AOwner) : TOwnedCollection(AOwner, __classid(TProjectTriggerItem)),
FEnabled(false) {

}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TProjectTriggerItems::GetDisplayName(void) {
	std::auto_ptr<TStringList>AStringList(new TStringList());
	for (int i = 0; i < this->Count; i++) {
		AStringList->Add(this->Items[i]->Event);
	}
	return(this->Enabled ? "Enabled>" : "Disabled>") + AStringList->CommaText;
}

// ---------------------------------------------------------------------------
TProjectTriggerItem* __fastcall TProjectTriggerItems::Add(void) {
	return dynamic_cast<TProjectTriggerItem*>(TOwnedCollection::Add());
}

// ---------------------------------------------------------------------------
TProjectTriggerItem* __fastcall TProjectTriggerItems::Insert(int Index) {
	return dynamic_cast<TProjectTriggerItem*>(TOwnedCollection::Insert(Index));
}

// ---------------------------------------------------------------------------
TProjectTriggerItem* __fastcall TProjectTriggerItems::GetItem(int Index) {
	return dynamic_cast<TProjectTriggerItem*>(TOwnedCollection::GetItem(Index));
}

// ---------------------------------------------------------------------------
void __fastcall TProjectTriggerItems::SetItem(int Index, TProjectTriggerItem* Value) {
	TOwnedCollection::SetItem(Index, Value);
}

// ---------------------------------------------------------------------------
void __fastcall TProjectTriggerItems::Assign(TPersistent* Source) {
	if (Source) {
		TProjectTriggerItems * ASeriesCompareItems = dynamic_cast<TProjectTriggerItems*>(Source);
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
