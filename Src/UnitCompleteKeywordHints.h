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

#ifndef UnitCompleteKeywordHintsH
#define UnitCompleteKeywordHintsH
//---------------------------------------------------------------------------

#include "UnitFormCollectionEditor.h"

class TAutoCompleteKeywordHintItem : public TCollectionItem
{
__published:

	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);

private:
	UnicodeString 	FKeyword;
	bool 			FIsFunction;
	UnicodeString	FRetVal;
	UnicodeString	FDescription;
	UnicodeString	FParams;

protected:
	 virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TAutoCompleteKeywordHintItem(TCollection* Collection);
	 virtual __fastcall ~TAutoCompleteKeywordHintItem();

	 virtual void __fastcall Assign(TPersistent* Source);

	 void Clear();

__published:

	 // Ќ≈ «јЅџ¬ј“№ ƒќЅј¬Ћя“№ —¬ќ…—“¬ј в Assign !!!

	__property UnicodeString	Keyword = {read=FKeyword, write=FKeyword};
	__property bool				IsFunction = {read=FIsFunction, write=FIsFunction, default=false};
	__property UnicodeString	RetVal = {read=FRetVal, write=FRetVal};
	__property UnicodeString	Description = {read=FDescription, write=FDescription};
	__property UnicodeString	Params = {read=FParams, write=FParams};

};

class TAutoCompleteKeywordHintItems: public TOwnedCollection
{
private:
protected:
	TAutoCompleteKeywordHintItem* __fastcall GetItem(int Index);
	void __fastcall SetItem(int Index, TAutoCompleteKeywordHintItem* Value);
	virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TAutoCompleteKeywordHintItems(TPersistent *AOwner);

	 virtual void __fastcall Assign(TPersistent* Source);

	 TAutoCompleteKeywordHintItem* __fastcall Add(void);
	 TAutoCompleteKeywordHintItem* __fastcall Insert(int Index);
	 __property TAutoCompleteKeywordHintItem* Items[int Index] = {read=GetItem, write=SetItem};

	 TAutoCompleteKeywordHintItem * __fastcall IndexOfKeyword(const UnicodeString &sKeyword);

__published:

	 // ѕ–»ћ≈–: «десь можно объ€вить свои свойства, если необходимо, чтобы они сохран€лись в конфиг
	 __property UnicodeString DisplayName = {read=GetDisplayName};
};

// ---------------------------------------------------------------------------
// ѕереопределение настройки коллекции
// ---------------------------------------------------------------------------
class TFormAutoCompleteKeywordHintCollectionEditor : public TFormCollectionEditor {
protected:

	// эту функцию надо будет переопределить, если понадоб€тс€ специальные настройки коллекции
	virtual void __fastcall CloneSourceCollection() {
		assert(FSourceCollection != NULL);
		FOwnedCollection = new TAutoCompleteKeywordHintItems(this);
		FOwnedCollection->Assign(FSourceCollection);

	}

	void __fastcall OnImportButtonClick(TObject *Sender);

public: // User declarations
	__fastcall TFormAutoCompleteKeywordHintCollectionEditor(TComponent* Owner, TOwnedCollection *AOwnedCollection) : TFormCollectionEditor(Owner,
		AOwnedCollection) {
		TButton *AButton = new TButton(this);
		AButton->Parent = Panel1;
		AButton->Align = alRight;
		AButton->AlignWithMargins = true;
		AButton->Caption = "Import";
		AButton->OnClick = OnImportButtonClick;


	}

};

// ---------------------------------------------------------------------------
class TAutoCompleteKeywordHintPropEditor : public TOwnedCollectionPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		TAutoCompleteKeywordHintItems * AAutoCompleteKeywordHintItems = dynamic_cast<TAutoCompleteKeywordHintItems*>(this->GetObjectValue(0));
		if (AAutoCompleteKeywordHintItems) {
			return AAutoCompleteKeywordHintItems->DisplayName;
		}

		return TOwnedCollectionPropEditor::GetValue();
	}

	// должна создатьс€ форма с редактором
	virtual TFormCollectionEditor *__fastcall DoCreateCollectionEditor(TOwnedCollection *ACollection) {
		return new TFormAutoCompleteKeywordHintCollectionEditor(Application, ACollection);
	}
};

#endif
