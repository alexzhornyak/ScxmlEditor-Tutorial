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

#ifndef UnitProjectTriggersH
#define UnitProjectTriggersH
//---------------------------------------------------------------------------
#include "UnitTriggers.h"

class TProjectTriggerItem : public TCollectionItem
{
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:

	UnicodeString FEvent;
	void __fastcall SetEvent(UnicodeString val);

	TContentTrigger *FContentTrigger;
	inline void __fastcall SetContentTrigger(TContentTrigger *val) { FContentTrigger->Assign(val); };

	TParamTriggerItems *FParams;
	inline void __fastcall SetParams(TParamTriggerItems *val) { FParams->Assign(val); };

	inline bool __fastcall GetEnabled(void) { return ContentTrigger->Enabled; }
	inline void __fastcall SetEnabled(bool val) { ContentTrigger->Enabled = val; }

protected:
	 virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TProjectTriggerItem(TCollection* Collection);
	 virtual __fastcall ~TProjectTriggerItem();

	 virtual void __fastcall Assign(TPersistent* Source);

__published:

	 // Ќ≈ «јЅџ¬ј“№ ƒќЅј¬Ћя“№ —¬ќ…—“¬ј в Assign !!!

	__property UnicodeString Event = {read=FEvent, write=SetEvent};
	__property bool Enabled = {read=GetEnabled, write=SetEnabled, stored=false};
	__property TContentTrigger *ContentTrigger = {read=FContentTrigger, write=SetContentTrigger};
	__property TParamTriggerItems *Params = {read=FParams, write=SetParams};
};

class TProjectTriggerItems: public TOwnedCollection
{
private:
	bool FEnabled;
	bool __fastcall GetEnabled(void) { return FEnabled && this->Count>0; }
protected:
	TProjectTriggerItem* __fastcall GetItem(int Index);
	void __fastcall SetItem(int Index, TProjectTriggerItem* Value);
	virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TProjectTriggerItems(TComponent *AOwner);

	 virtual void __fastcall Assign(TPersistent* Source);

	 TProjectTriggerItem* __fastcall Add(void);
	 TProjectTriggerItem* __fastcall Insert(int Index);
	 __property TProjectTriggerItem* Items[int Index] = {read=GetItem, write=SetItem};

__published:

	 // ѕ–»ћ≈–: «десь можно объ€вить свои свойства, если необходимо, чтобы они сохран€лись в конфиг
	 __property bool Enabled = {read=GetEnabled, write=FEnabled,default=false};
	 __property UnicodeString DisplayName = {read=GetDisplayName};
};

// ---------------------------------------------------------------------------
// ѕереопределение настройки коллекции
// ---------------------------------------------------------------------------
class TFormProjectTriggersCollectionEditor : public TFormCollectionEditor {
protected:

	// эту функцию надо будет переопределить, если понадоб€тс€ специальные настройки коллекции
	virtual void __fastcall CloneSourceCollection() {
		assert(FSourceCollection != NULL);
		FOwnedCollection = new TProjectTriggerItems(this);
		FOwnedCollection->Assign(FSourceCollection);

	}

public: // User declarations
	__fastcall TFormProjectTriggersCollectionEditor(TComponent* Owner, TOwnedCollection *AOwnedCollection) : TFormCollectionEditor(Owner,
		AOwnedCollection) {
	}

};

// ---------------------------------------------------------------------------
class TPropTriggersPropEditor : public TOwnedCollectionPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		TProjectTriggerItems * AProjectTriggerItems = dynamic_cast<TProjectTriggerItems*>(this->GetObjectValue(0));
		if (AProjectTriggerItems) {
			return AProjectTriggerItems->DisplayName;
		}

		return TOwnedCollectionPropEditor::GetValue();
	}

	// должна создатьс€ форма с редактором
	virtual TFormCollectionEditor *__fastcall DoCreateCollectionEditor(TOwnedCollection *ACollection) {
		return new TFormProjectTriggersCollectionEditor(Application, ACollection);
	}
};

#endif
