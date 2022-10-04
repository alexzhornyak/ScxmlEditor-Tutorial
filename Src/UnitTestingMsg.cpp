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

#include "UnitTestingMsg.h"

#include <memory>

// ---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// ----------------------- TTestingMsgItem --------------------------------
// ---------------------------------------------------------------------------
__fastcall TTestingMsgItem::TTestingMsgItem(TCollection* Collection) : TCollectionItem(Collection) {
	FScxmlName = L"";
	FIdentifier = L"";
	FMsg = L"";
	FType = smttUnknown;
	FMatch = TMsgMatchTypes() << mmtMAXSIZE;
}

// ---------------------------------------------------------------------------
__fastcall TTestingMsgItem::~TTestingMsgItem() {
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TTestingMsgItem::GetDisplayName(void) {
	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	AStringListPtr->Delimiter = L';';
	AStringListPtr->StrictDelimiter = true;

	if (!FScxmlName.IsEmpty()) {
		AStringListPtr->Add(FScxmlName);
	}
	if (!FMsg.IsEmpty()) {
		AStringListPtr->Add(FMsg);
	}
	if (!FIdentifier.IsEmpty()) {
		AStringListPtr->Add(FIdentifier);
	}

	if (FType != smttUnknown) {
		const UnicodeString sType = ScxmlMsgTypeToString(FType);
		if (!sType.IsEmpty()) {
			AStringListPtr->Add(sType);
		}
	}

	AStringListPtr->Add(GetSetProp(this, "Match", true));

	return "Item " + UnicodeString(Index) + " {" + AStringListPtr->DelimitedText + "}";
}

// ---------------------------------------------------------------------------
void __fastcall TTestingMsgItem::Assign(TPersistent* Source) {
	if (Source) {
		TTestingMsgItem * AItem = dynamic_cast<TTestingMsgItem*>(Source);
		if (AItem) {
			FScxmlName = AItem->FScxmlName;
			FIdentifier = AItem->FIdentifier;
			FMsg = AItem->FMsg;
			FType = AItem->FType;
			FMatch = AItem->FMatch;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
bool TTestingMsgItem::IsMatched(const UnicodeString &sMsg, const UnicodeString &sScxmlName, const UnicodeString &sIdentifier,
	const TScxmlMsgType Type) {
	bool bMatchMsg = true;
	if (Match.Contains(mmtMsg) || Match.Contains(mmtMAXSIZE)) {
		bMatchMsg = SameText(sMsg, this->Msg);
	}
	bool bMatchScxmlName = true;
	if (Match.Contains(mmtScxmlName) || Match.Contains(mmtMAXSIZE)) {
		bMatchScxmlName = SameText(sScxmlName, this->ScxmlName);
	}
	bool bMatchIdentifier = true;
	if (Match.Contains(mmtIdentifier) || Match.Contains(mmtMAXSIZE)) {
		bMatchIdentifier = SameText(sIdentifier, this->Identifier);
	}
	bool bMatchType = true;
	if (this->Type != smttUnknown && (Match.Contains(mmtType) || Match.Contains(mmtMAXSIZE))) {
		bMatchType = this->Type == Type;
	}
	return bMatchMsg && bMatchScxmlName && bMatchIdentifier && bMatchType;
}

// ---------------------------------------------------------------------------
// --------------------------- COLLECTION ------------------------------------
// ----------------------- TTestingMsgItems -------------------------------
// ---------------------------------------------------------------------------
__fastcall TTestingMsgItems::TTestingMsgItems(TComponent *AOwner) : TOwnedCollection(AOwner, __classid(TTestingMsgItem)) {

}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TTestingMsgItems::GetDisplayName(void) {
	return UnicodeString(this->Count) + L" item(s)";
}

// ---------------------------------------------------------------------------
TTestingMsgItem* __fastcall TTestingMsgItems::Add(void) {
	return dynamic_cast<TTestingMsgItem*>(TOwnedCollection::Add());
}

// ---------------------------------------------------------------------------
TTestingMsgItem* __fastcall TTestingMsgItems::Insert(int Index) {
	return dynamic_cast<TTestingMsgItem*>(TOwnedCollection::Insert(Index));
}

// ---------------------------------------------------------------------------
TTestingMsgItem* __fastcall TTestingMsgItems::GetItem(int Index) {
	return dynamic_cast<TTestingMsgItem*>(TOwnedCollection::GetItem(Index));
}

// ---------------------------------------------------------------------------
void __fastcall TTestingMsgItems::SetItem(int Index, TTestingMsgItem* Value) {
	TOwnedCollection::SetItem(Index, Value);
}

// ---------------------------------------------------------------------------
void __fastcall TTestingMsgItems::Assign(TPersistent* Source) {
	if (Source) {
		TTestingMsgItems * AItems = dynamic_cast<TTestingMsgItems*>(Source);
		if (AItems) {
			TOwnedCollection::Assign(Source);

			// здесь применяем настройки непосредственно этого класса
			// ПРИМЕР:

		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");

}

// ---------------------------------------------------------------------------
int TTestingMsgItems::IndexOfAnyMatched(const UnicodeString &sMsg, const UnicodeString &sScxmlName, const UnicodeString &sIdentifier,
	const TScxmlMsgType Type) {
	for (int i = 0; i < this->Count; i++) {
		if (this->Items[i]->IsMatched(sMsg, sScxmlName, sIdentifier, Type)) {
			return i;
		}
	}
	return -1;
}

// ---------------------------------------------------------------------------
