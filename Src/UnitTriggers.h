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

#ifndef UnitTriggersH
#define UnitTriggersH
//---------------------------------------------------------------------------
#include "UnitSyntaxString.h"
#include "UnitFormCollectionEditor.h"
//---------------------------------------------------------------------------

typedef enum { cttDefault, cttBool, cttInteger, cttDouble, cttString, cttJson, cttUserData} TContentTriggerType;
typedef enum { ctaButton, ctaCheckBox, ctaTrackBar, ctaCombo, ctaEdit, ctaMemo} TContentTriggerAppearance;

#define DISPLAY_PARAM_TRIGGER_WIDTH		100

class TContentTrigger: public TPersistent {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	bool FEnabled;
	void __fastcall SetEnabled(bool val);

	UnicodeString FEvent;
	UnicodeString __fastcall GetEvent(void);
	inline bool __fastcall IsEventStored() { return !FEvent.IsEmpty(); }

	TContentTriggerType FTriggerType;
	TContentTriggerAppearance FAppearance;
	UnicodeString FExpr;
	TSyntaxString* FXMLText;
	inline void __fastcall SetXMLText(TSyntaxString *val) { FXMLText->Assign(val); }

	UnicodeString	   FMin;
	UnicodeString	   FMax;
	UnicodeString	   FOn;
	UnicodeString	   FOff;

	TStrings *FValues;
	inline void __fastcall SetValues(TStrings *val) { FValues->Assign(val); }

protected:
	TObject *FParent;
public:

	__fastcall virtual TContentTrigger(TObject *AParent);
	__fastcall virtual ~TContentTrigger();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString				Event = {read=GetEvent,write=FEvent,stored=IsEventStored};
	__property bool                		 	Enabled = {read=FEnabled,write=SetEnabled,default=false};
	__property UnicodeString 				Expr = {read=FExpr,write=FExpr};
	__property TSyntaxString*				XMLText = {read=FXMLText,write=SetXMLText};
	__property TContentTriggerType 			TriggerType = {read=FTriggerType, write=FTriggerType, default=cttDefault};
	__property TContentTriggerAppearance 	Appearance = {read=FAppearance,write=FAppearance,default=ctaButton};
	__property UnicodeString				Min = {read=FMin, write=FMin};
	__property UnicodeString				Max = {read=FMax, write=FMax};
	__property UnicodeString				On = {read=FOn, write=FOn};
	__property UnicodeString				Off = {read=FOff, write=FOff};
	__property TStrings	*					Values = {read=FValues,write=SetValues};
};

class TParamTriggerItem : public TCollectionItem
{
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:

	UnicodeString FParamName;
	UnicodeString FLocation;
	UnicodeString FExpr;
	TContentTriggerType FTriggerType;
	int FDisplayWidth;

protected:
     virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TParamTriggerItem(TCollection* Collection);
	 virtual __fastcall ~TParamTriggerItem();

	 virtual void __fastcall Assign(TPersistent* Source);

__published:

	 // Ќ≈ «јЅџ¬ј“№ ƒќЅј¬Ћя“№ —¬ќ…—“¬ј в Assign !!!

	__property UnicodeString 				ParamName = {read=FParamName,write=FParamName};
	__property UnicodeString 				Location = {read=FLocation,write=FLocation};
	__property UnicodeString 				Expr = {read=FExpr,write=FExpr};
	__property TContentTriggerType 			TriggerType = {read=FTriggerType, write=FTriggerType, default=cttDefault};
	__property int				 			DisplayWidth = {read=FDisplayWidth, write=FDisplayWidth, default=DISPLAY_PARAM_TRIGGER_WIDTH};
};

class TParamTriggerItems: public TOwnedCollection
{
private:
	bool FEnabled;
	bool __fastcall GetEnabled(void) { return FEnabled && this->Count>0; }
protected:
	TParamTriggerItem* __fastcall GetItem(int Index);
	void __fastcall SetItem(int Index, TParamTriggerItem* Value);
	virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TParamTriggerItems(TComponent *AOwner);

	 virtual void __fastcall Assign(TPersistent* Source);

	 TParamTriggerItem* __fastcall Add(void);
	 TParamTriggerItem* __fastcall Insert(int Index);
	 __property TParamTriggerItem* Items[int Index] = {read=GetItem, write=SetItem};

__published:

	 // ѕ–»ћ≈–: «десь можно объ€вить свои свойства, если необходимо, чтобы они сохран€лись в конфиг
	 __property bool Enabled = {read=GetEnabled, write=FEnabled,default=false};
	 __property UnicodeString DisplayName = {read=GetDisplayName};
};

// ---------------------------------------------------------------------------
// ѕереопределение настройки коллекции
// ---------------------------------------------------------------------------
class TFormParamTriggersCollectionEditor : public TFormCollectionEditor {
protected:

	// эту функцию надо будет переопределить, если понадоб€тс€ специальные настройки коллекции
	virtual void __fastcall CloneSourceCollection() {
		assert(FSourceCollection != NULL);
		FOwnedCollection = new TParamTriggerItems(this);
		FOwnedCollection->Assign(FSourceCollection);

	}

public: // User declarations
	__fastcall TFormParamTriggersCollectionEditor(TComponent* Owner, TOwnedCollection *AOwnedCollection) : TFormCollectionEditor(Owner,
		AOwnedCollection) {
	}

};

// ---------------------------------------------------------------------------
class TParamTriggersPropEditor : public TOwnedCollectionPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		TParamTriggerItems * AParamTriggerItems = dynamic_cast<TParamTriggerItems*>(this->GetObjectValue(0));
		if (AParamTriggerItems) {
			return AParamTriggerItems->DisplayName;
		}

		return TOwnedCollectionPropEditor::GetValue();
	}

	// должна создатьс€ форма с редактором
	virtual TFormCollectionEditor *__fastcall DoCreateCollectionEditor(TOwnedCollection *ACollection) {
		return new TFormParamTriggersCollectionEditor(Application, ACollection);
	}
};

class TContentPropEditor : public TLMDClassPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		TContentTrigger * AContentTrigger = dynamic_cast<TContentTrigger*>(this->GetObjectValue(0));
		if (AContentTrigger) {
			const UnicodeString sTriggerType = StringReplace(GetEnumProp(AContentTrigger, "TriggerType"), "ctt", "", TReplaceFlags());
			const UnicodeString sAppearance = StringReplace(GetEnumProp(AContentTrigger, "Appearance"), "cta", "", TReplaceFlags());

			return(AContentTrigger->Enabled ? L"Enabled>" : L"Disabled>") + sTriggerType + " " + sAppearance;
		}
		return L"";
	}
};

#endif
