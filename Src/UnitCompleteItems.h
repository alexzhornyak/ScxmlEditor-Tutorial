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

#ifndef UnitCompleteItemsH
#define UnitCompleteItemsH
//---------------------------------------------------------------------------

#define AUTO_COMPLETE_WORDS_SEPARATORS		L"[]();,'\"-+/\\:{}"

#include "UnitFormCollectionEditor.h"
#include "UnitCompleteKeywordHints.h"

class TAutoCompleteItem : public TCollectionItem
{
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:

	TStrings *FDefault;

	TStringList *FDefaultLua;
	TStringList *FDefaultEcma;
	TStringList *FDefaultCPP;
	TStringList *FDefaultPython;

	TStrings *__fastcall GetDefault(void);

	TStringList *FCustom;
	TStringList * __fastcall GetCustom(void) { return FCustom; }
	void __fastcall SetCustom(TStrings *val);

	UnicodeString FSyntaxScheme;
	void __fastcall SetSyntaxScheme(UnicodeString val);

	UnicodeString FWordSeparators;
	bool __fastcall IsWordSeparatorsStored(void) { return FWordSeparators!=DefaultWordsSeparators; }

	TAutoCompleteKeywordHintItems *FKeywordHints;
	void __fastcall SetKeywordHints(TAutoCompleteKeywordHintItems *val) { FKeywordHints->Assign(val); };

protected:
	 virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TAutoCompleteItem(TCollection* Collection);
	 virtual __fastcall ~TAutoCompleteItem();

	 virtual void __fastcall Assign(TPersistent* Source);

	 static UnicodeString DefaultWordsSeparators;

__published:

	 // Ќ≈ «јЅџ¬ј“№ ƒќЅј¬Ћя“№ —¬ќ…—“¬ј в Assign !!!

	__property TStrings *		Default = {read=GetDefault};
	__property TStrings *		Custom = {read=GetCustom, write=SetCustom};
	__property UnicodeString	WordSeparators = {read=FWordSeparators, write=FWordSeparators, stored=IsWordSeparatorsStored};
	__property UnicodeString 	SyntaxScheme = {read=FSyntaxScheme, write=FSyntaxScheme};
	__property TAutoCompleteKeywordHintItems *KeywordHints = {read=FKeywordHints, write=SetKeywordHints};
};

class TAutoCompleteItems: public TOwnedCollection
{
private:
protected:
	TAutoCompleteItem* __fastcall GetItem(int Index);
	void __fastcall SetItem(int Index, TAutoCompleteItem* Value);
	virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TAutoCompleteItems(TPersistent *AOwner);

	 virtual void __fastcall Assign(TPersistent* Source);

	 TAutoCompleteItem* __fastcall Add(void);
	 TAutoCompleteItem* __fastcall Insert(int Index);
	 __property TAutoCompleteItem* Items[int Index] = {read=GetItem, write=SetItem};

	 TAutoCompleteItem * __fastcall IndexOfSyntaxScheme(const UnicodeString &sSyntaxScheme);

__published:

	 // ѕ–»ћ≈–: «десь можно объ€вить свои свойства, если необходимо, чтобы они сохран€лись в конфиг
	 __property UnicodeString DisplayName = {read=GetDisplayName};
};

// ---------------------------------------------------------------------------
// ѕереопределение настройки коллекции
// ---------------------------------------------------------------------------
class TFormAutoCompleteCollectionEditor : public TFormCollectionEditor {
protected:

	// эту функцию надо будет переопределить, если понадоб€тс€ специальные настройки коллекции
	virtual void __fastcall CloneSourceCollection() {
		assert(FSourceCollection != NULL);
		FOwnedCollection = new TAutoCompleteItems(this);
		FOwnedCollection->Assign(FSourceCollection);

	}

public: // User declarations
	__fastcall TFormAutoCompleteCollectionEditor(TComponent* Owner, TOwnedCollection *AOwnedCollection) : TFormCollectionEditor(Owner,
		AOwnedCollection) {
	}

};

// ---------------------------------------------------------------------------
class TAutoCompletePropEditor : public TOwnedCollectionPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		TAutoCompleteItems * AAutoCompleteItems = dynamic_cast<TAutoCompleteItems*>(this->GetObjectValue(0));
		if (AAutoCompleteItems) {
			return AAutoCompleteItems->DisplayName;
		}

		return TOwnedCollectionPropEditor::GetValue();
	}

	// должна создатьс€ форма с редактором
	virtual TFormCollectionEditor *__fastcall DoCreateCollectionEditor(TOwnedCollection *ACollection) {
		return new TFormAutoCompleteCollectionEditor(Application, ACollection);
	}
};

#endif
