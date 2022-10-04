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

#include "UnitTestingApplicationsPresets.h"

#include <memory>

#include "Log4cpp_VCL.hpp"
//---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// ----------------------- TTestApplicationPresetItem --------------------------------
// ---------------------------------------------------------------------------
__fastcall TTestApplicationPresetItem::TTestApplicationPresetItem(TCollection* Collection) : TCollectionItem(Collection) {
	FPresetName = L"";
	FEnabled = true;
	FTestSettings = new TTestSettings(tsbtProject);
}

// ---------------------------------------------------------------------------
__fastcall TTestApplicationPresetItem::~TTestApplicationPresetItem() {

}

// ---------------------------------------------------------------------------
void __fastcall TTestApplicationPresetItem::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"TestSettings") {
			AEditorClass = __classid(TTestSettingsPropEditor);
		}
	}
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TTestApplicationPresetItem::GetDisplayName(void) {
	return UnicodeString().sprintf(L"Item %d {%s}", Index, FPresetName.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TTestApplicationPresetItem::Assign(TPersistent* Source) {
	if (Source) {
		TTestApplicationPresetItem * ATestApplicationPresetItem = dynamic_cast<TTestApplicationPresetItem*>(Source);
		if (ATestApplicationPresetItem) {
			FPresetName = ATestApplicationPresetItem->FPresetName;
			FEnabled = ATestApplicationPresetItem->FEnabled;
			FTestSettings = ATestApplicationPresetItem->FTestSettings;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
// --------------------------- COLLECTION ------------------------------------
// ----------------------- TTestApplicationPresetItems -------------------------------
// ---------------------------------------------------------------------------
__fastcall TTestApplicationPresetItems::TTestApplicationPresetItems(TPersistent *AOwner) : TOwnedCollection(AOwner,
	__classid(TTestApplicationPresetItem)) {

}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TTestApplicationPresetItems::GetDisplayName(void) {
	std::auto_ptr<TStringList>AStringList(new TStringList());
	for (int i = 0; i < this->Count; i++) {
		if (this->Items[i]->Enabled) {
			AStringList->Add(this->Items[i]->PresetName);
		}
	}
	return AStringList->CommaText;
}

// ---------------------------------------------------------------------------
TTestApplicationPresetItem* __fastcall TTestApplicationPresetItems::Add(void) {
	return dynamic_cast<TTestApplicationPresetItem*>(TOwnedCollection::Add());
}

// ---------------------------------------------------------------------------
TTestApplicationPresetItem* __fastcall TTestApplicationPresetItems::Insert(int Index) {
	return dynamic_cast<TTestApplicationPresetItem*>(TOwnedCollection::Insert(Index));
}

// ---------------------------------------------------------------------------
TTestApplicationPresetItem* __fastcall TTestApplicationPresetItems::GetItem(int Index) {
	return dynamic_cast<TTestApplicationPresetItem*>(TOwnedCollection::GetItem(Index));
}

// ---------------------------------------------------------------------------
void __fastcall TTestApplicationPresetItems::SetItem(int Index, TTestApplicationPresetItem* Value) {
	TOwnedCollection::SetItem(Index, Value);
}

// ---------------------------------------------------------------------------
void __fastcall TTestApplicationPresetItems::Assign(TPersistent* Source) {
	if (Source) {
		TTestApplicationPresetItems * ASeriesCompareItems = dynamic_cast<TTestApplicationPresetItems*>(Source);
		if (ASeriesCompareItems) {
			TOwnedCollection::Assign(Source);

			// здесь применяем настройки непосредственно этого класса
			// ПРИМЕР:
			// FEnabled = ASeriesCompareItems->FEnabled;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");

}

// ---------------------------------------------------------------------------
TTestApplicationPresetItem* __fastcall TTestApplicationPresetItems::GetItemByPresetName(const UnicodeString &sPresetName) {
	for (int i = 0; i < this->Count; i++) {
		if (this->Items[i]->PresetName == sPresetName) {
			return this->Items[i];
		}
	}
	return NULL;
}
