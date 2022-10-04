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

#include "UnitCompleteItems.h"

#include <memory>

//---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// ----------------------- TAutoCompleteItem --------------------------------
// ---------------------------------------------------------------------------

UnicodeString TAutoCompleteItem::DefaultWordsSeparators = AUTO_COMPLETE_WORDS_SEPARATORS;

__fastcall TAutoCompleteItem::TAutoCompleteItem(TCollection* Collection) : TCollectionItem(Collection) {
	FDefault = new TStringList;

	FDefaultLua = new TStringList;
	FDefaultLua->Sorted = true;

	FDefaultEcma = new TStringList;
	FDefaultEcma->Sorted = true;

	FDefaultCPP = new TStringList;
	FDefaultCPP->Sorted = true;

	FDefaultPython = new TStringList;
	FDefaultPython->Sorted = true;

	FCustom = new TStringList;
	FCustom->Sorted = true;

	FWordSeparators = AUTO_COMPLETE_WORDS_SEPARATORS;

	FKeywordHints = new TAutoCompleteKeywordHintItems(this);

	// lua
	{
		std::auto_ptr<TStringList>ACompleteStrings(new TStringList());
		ACompleteStrings->CommaText = "CompleteMath,CompleteString,CompleteTable,CompleteBasic,CompleteDebug,CompleteOs,CompleteUtf8";

		for (int i = 0; i < ACompleteStrings->Count; i++) {
			std::auto_ptr<TResourceStream>ACompleteResStream(new TResourceStream((int)HInstance, ACompleteStrings->Strings[i].c_str(),
					MAKEINTRESOURCEW(10)));

			std::auto_ptr<TStringList>ACombineStream(new TStringList());
			ACombineStream->LoadFromStream(ACompleteResStream.get());

			FDefaultLua->AddStrings(ACombineStream.get());
		}
	}

	// ecmascript
	{
		std::auto_ptr<TResourceStream>ACompleteResStream(new TResourceStream((int)HInstance, L"CompleteEcmaScript", MAKEINTRESOURCEW(10)));
		FDefaultEcma->LoadFromStream(ACompleteResStream.get());
	}

	// cpp
	{
		std::auto_ptr<TResourceStream>ACompleteResStream(new TResourceStream((int)HInstance, L"CompleteCPP", MAKEINTRESOURCEW(10)));
		FDefaultCPP->LoadFromStream(ACompleteResStream.get());
	}

	// python
	{
		std::auto_ptr<TResourceStream>ACompleteResStream(new TResourceStream((int)HInstance, L"CompletePython", MAKEINTRESOURCEW(10)));
		FDefaultPython->LoadFromStream(ACompleteResStream.get());
	}
}

// ---------------------------------------------------------------------------
__fastcall TAutoCompleteItem::~TAutoCompleteItem() {
	delete FDefault;

	delete FDefaultLua;
	delete FDefaultEcma;
	delete FDefaultCPP;
	delete FDefaultPython;

	delete FCustom;

	delete FKeywordHints;
}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteItem::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass)
{
	if (APropInfo) {
		if (APropInfo->Name() == L"KeywordHints") {
			AEditorClass = __classid(TAutoCompleteKeywordHintPropEditor);
		}
	}
}

// ---------------------------------------------------------------------------
TStrings * __fastcall TAutoCompleteItem::GetDefault(void) {
	if (SameText(SyntaxScheme, "Lua")) {
		return FDefaultLua;
	}
	if (SameText(SyntaxScheme, "EcmaScript")) {
		return FDefaultEcma;
	}
	if (SameText(SyntaxScheme, "CPP")) {
		return FDefaultCPP;
	}
	if (SameText(SyntaxScheme, "Python")) {
		return FDefaultPython;
	}
	return FDefault;
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TAutoCompleteItem::GetDisplayName(void) {
	return "Item " + UnicodeString(Index) + " {" + SyntaxScheme + "}";
}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteItem::Assign(TPersistent* Source) {
	if (Source) {
		TAutoCompleteItem * AItem = dynamic_cast<TAutoCompleteItem*>(Source);
		if (AItem) {
			FCustom->Assign(AItem->FCustom);

			FSyntaxScheme = AItem->FSyntaxScheme;
			FWordSeparators = AItem->FWordSeparators;

			FKeywordHints->Assign(AItem->FKeywordHints);
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteItem::SetCustom(TStrings *val) {
	FCustom->Assign(val);
}

// ---------------------------------------------------------------------------
// --------------------------- COLLECTION ------------------------------------
// ----------------------- TAutoCompleteItems -------------------------------
// ---------------------------------------------------------------------------
__fastcall TAutoCompleteItems::TAutoCompleteItems(TPersistent *AOwner) : TOwnedCollection(AOwner, __classid(TAutoCompleteItem)) {

}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TAutoCompleteItems::GetDisplayName(void) {
	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	for (int i = 0; i < this->Count; i++) {
		AStringListPtr->Add(this->Items[i]->SyntaxScheme);
	}

	return AStringListPtr->CommaText;
}

// ---------------------------------------------------------------------------
TAutoCompleteItem * __fastcall TAutoCompleteItems::IndexOfSyntaxScheme(const UnicodeString &sSyntaxScheme) {
	for (int i = 0; i < this->Count; i++) {
		if (SameText(this->Items[i]->SyntaxScheme, sSyntaxScheme)) {
			return this->Items[i];
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TAutoCompleteItem* __fastcall TAutoCompleteItems::Add(void) {
	return dynamic_cast<TAutoCompleteItem*>(TOwnedCollection::Add());
}

// ---------------------------------------------------------------------------
TAutoCompleteItem* __fastcall TAutoCompleteItems::Insert(int Index) {
	return dynamic_cast<TAutoCompleteItem*>(TOwnedCollection::Insert(Index));
}

// ---------------------------------------------------------------------------
TAutoCompleteItem* __fastcall TAutoCompleteItems::GetItem(int Index) {
	return dynamic_cast<TAutoCompleteItem*>(TOwnedCollection::GetItem(Index));
}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteItems::SetItem(int Index, TAutoCompleteItem* Value) {
	TOwnedCollection::SetItem(Index, Value);
}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteItems::Assign(TPersistent* Source) {
	if (Source) {
		TAutoCompleteItems * AItems = dynamic_cast<TAutoCompleteItems*>(Source);
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
