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

#ifndef UnitTestingMsgH
#define UnitTestingMsgH
//---------------------------------------------------------------------------

#include "ScxmlMsgTypes.h"
#include "UnitFormCollectionEditor.h"

typedef enum {mmtScxmlName, mmtMsg, mmtIdentifier, mmtType, mmtMAXSIZE} TMsgMatchType;
typedef Set<TMsgMatchType, mmtScxmlName, mmtMAXSIZE> TMsgMatchTypes;

class TTestingMsgItem : public TCollectionItem
{
__published:
private:

	UnicodeString FScxmlName;
	UnicodeString FMsg;
	UnicodeString FIdentifier;
	TScxmlMsgType FType;
	TMsgMatchTypes FMatch;

protected:
	 virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TTestingMsgItem(TCollection* Collection);
	 virtual __fastcall ~TTestingMsgItem();

	 virtual void __fastcall Assign(TPersistent* Source);

	 bool IsMatched(const UnicodeString &sMsg, const UnicodeString &sScxmlName,
	 					const UnicodeString &sIdentifier, const TScxmlMsgType Type);

__published:

	 // Ќ≈ «јЅџ¬ј“№ ƒќЅј¬Ћя“№ —¬ќ…—“¬ј в Assign !!!

	__property UnicodeString ScxmlName = {read=FScxmlName, write=FScxmlName};
	__property UnicodeString Msg = {read=FMsg, write=FMsg};
	__property UnicodeString Identifier = {read=FIdentifier, write=FIdentifier};
	__property TScxmlMsgType Type = {read=FType, write=FType, default=smttUnknown};
	__property TMsgMatchTypes Match = {read=FMatch, write=FMatch, default=8};
};

class TTestingMsgItems: public TOwnedCollection
{
private:
protected:
	TTestingMsgItem* __fastcall GetItem(int Index);
	void __fastcall SetItem(int Index, TTestingMsgItem* Value);
	virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TTestingMsgItems(TComponent *AOwner);

	 virtual void __fastcall Assign(TPersistent* Source);

	 TTestingMsgItem* __fastcall Add(void);
	 TTestingMsgItem* __fastcall Insert(int Index);
	 __property TTestingMsgItem* Items[int Index] = {read=GetItem, write=SetItem};

	 // возвращает индекс первого найденного вхождени€
	 int IndexOfAnyMatched(const UnicodeString &sMsg, const UnicodeString &sScxmlName,
							const UnicodeString &sIdentifier, const TScxmlMsgType Type);

__published:

	 // ѕ–»ћ≈–: «десь можно объ€вить свои свойства, если необходимо, чтобы они сохран€лись в конфиг
	 __property UnicodeString DisplayName = {read=GetDisplayName};
};

// ---------------------------------------------------------------------------
// ѕереопределение настройки коллекции
// ---------------------------------------------------------------------------
class TFormTestingMsgCollectionEditor : public TFormCollectionEditor {
protected:

	// эту функцию надо будет переопределить, если понадоб€тс€ специальные настройки коллекции
	virtual void __fastcall CloneSourceCollection() {
		assert(FSourceCollection != NULL);
		FOwnedCollection = new TTestingMsgItems(this);
		FOwnedCollection->Assign(FSourceCollection);

	}

public: // User declarations
	__fastcall TFormTestingMsgCollectionEditor(TComponent* Owner, TOwnedCollection *AOwnedCollection) : TFormCollectionEditor(Owner,
		AOwnedCollection) {
	}

};

// ---------------------------------------------------------------------------
class TTestingMsgPropEditor : public TOwnedCollectionPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		TTestingMsgItems * ATestingMsgItems = dynamic_cast<TTestingMsgItems*>(this->GetObjectValue(0));
		if (ATestingMsgItems) {
			return ATestingMsgItems->DisplayName;
		}

		return TOwnedCollectionPropEditor::GetValue();
	}

	// должна создатьс€ форма с редактором
	virtual TFormCollectionEditor *__fastcall DoCreateCollectionEditor(TOwnedCollection *ACollection) {
		return new TFormTestingMsgCollectionEditor(Application, ACollection);
	}
};

#endif
