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

#include "UnitSettingsBinaryItems.h"

#include <memory>

//---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// ----------------------- TSettingsRegistryItem --------------------------------
// ---------------------------------------------------------------------------

__fastcall TSettingsRegistryItem::TSettingsRegistryItem(TCollection* Collection) : TCollectionItem(Collection) {
	FKey = L"";
	FData = new TStringList;
}

// ---------------------------------------------------------------------------
__fastcall TSettingsRegistryItem::~TSettingsRegistryItem() {
	delete FData;
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsRegistryItem::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	//	if (APropInfo) {
	//		if (APropInfo->Name() == L"KeywordHints") {
	//			AEditorClass = __classid(TSettingsRegistryKeywordHintPropEditor);
	//		}
	//	}
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TSettingsRegistryItem::GetDisplayName(void) {
	return "Item " + UnicodeString(Index) + " {" + Key + "}";
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsRegistryItem::Assign(TPersistent* Source) {
	if (Source) {
		TSettingsRegistryItem * AItem = dynamic_cast<TSettingsRegistryItem*>(Source);
		if (AItem) {
			FKey = AItem->FKey;

			FData->Assign(AItem->FData);
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
// --------------------------- COLLECTION ------------------------------------
// ----------------------- TSettingsRegistryItems -------------------------------
// ---------------------------------------------------------------------------
__fastcall TSettingsRegistryItems::TSettingsRegistryItems(TComponent *AOwner) : TOwnedCollection(AOwner, __classid(TSettingsRegistryItem)) {

}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TSettingsRegistryItems::GetDisplayName(void) {
	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	for (int i = 0; i < this->Count; i++) {
		AStringListPtr->Add(this->Items[i]->Key);
	}

	return AStringListPtr->CommaText;
}

// ---------------------------------------------------------------------------
TSettingsRegistryItem* __fastcall TSettingsRegistryItems::GetMap(UnicodeString Key) {
	for (int i = 0; i < this->Count; i++) {
		if (this->Items[i]->Key == Key) {
			return this->Items[i];
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsRegistryItems::SetMap(UnicodeString Key, TSettingsRegistryItem* Value) {
	TSettingsRegistryItem * AItem = Map[Key];
	if (!AItem) {
		AItem = this->Add();
	}
	AItem->Assign(Value);
	AItem->Key = Key;
}

// ---------------------------------------------------------------------------
TSettingsRegistryItem* __fastcall TSettingsRegistryItems::Add(void) {
	return dynamic_cast<TSettingsRegistryItem*>(TOwnedCollection::Add());
}

// ---------------------------------------------------------------------------
TSettingsRegistryItem* __fastcall TSettingsRegistryItems::Insert(int Index) {
	return dynamic_cast<TSettingsRegistryItem*>(TOwnedCollection::Insert(Index));
}

// ---------------------------------------------------------------------------
TSettingsRegistryItem* __fastcall TSettingsRegistryItems::GetItem(int Index) {
	return dynamic_cast<TSettingsRegistryItem*>(TOwnedCollection::GetItem(Index));
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsRegistryItems::SetItem(int Index, TSettingsRegistryItem* Value) {
	TOwnedCollection::SetItem(Index, Value);
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsRegistryItems::Assign(TPersistent* Source) {
	if (Source) {
		TSettingsRegistryItems * AItems = dynamic_cast<TSettingsRegistryItems*>(Source);
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
