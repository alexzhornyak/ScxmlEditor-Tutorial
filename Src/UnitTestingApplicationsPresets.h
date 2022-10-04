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

#ifndef UnitTestingApplicationsPresetsH
#define UnitTestingApplicationsPresetsH
//---------------------------------------------------------------------------

#include "UnitFormCollectionEditor.h"

#include "UnitTestSettings.h"

#define APP_TESTER_PRESET_USCXML			L"USCXML Tester"
#define APP_TESTER_PRESET_QT				L"Qt SCXML Tester"
#define APP_TESTER_PRESET_SCION				L"SCION CLI"

class TTestApplicationPresetItem : public TCollectionItem
{
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	UnicodeString 		FPresetName;
	bool 				FEnabled;

	TTestSettings *		FTestSettings;
	inline void __fastcall SetTestSettings(TTestSettings *val) { FTestSettings->Assign(val); }

protected:
     virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TTestApplicationPresetItem(TCollection* Collection);
	 virtual __fastcall ~TTestApplicationPresetItem();

	 virtual void __fastcall Assign(TPersistent* Source);

__published:

	 // НЕ ЗАБЫВАТЬ ДОБАВЛЯТЬ СВОЙСТВА в Assign !!!
	__property bool 					Enabled = {read=FEnabled, write=FEnabled,default=true};
	__property UnicodeString 			PresetName = {read=FPresetName,write=FPresetName};
	__property TTestSettings *			TestSettings = {read=FTestSettings, write=SetTestSettings};

};

class TTestApplicationPresetItems: public TOwnedCollection
{
private:
protected:
	TTestApplicationPresetItem* __fastcall GetItem(int Index);
	void __fastcall SetItem(int Index, TTestApplicationPresetItem* Value);
	virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TTestApplicationPresetItems(TPersistent *AOwner);

	 virtual void __fastcall Assign(TPersistent* Source);

	 TTestApplicationPresetItem* __fastcall Add(void);
	 TTestApplicationPresetItem* __fastcall Insert(int Index);
	 __property TTestApplicationPresetItem* Items[int Index] = {read=GetItem, write=SetItem};

     TTestApplicationPresetItem* __fastcall GetItemByPresetName(const UnicodeString &sPresetName);

__published:

	 __property UnicodeString DisplayName = {read=GetDisplayName};
};

// ---------------------------------------------------------------------------
// Переопределение настройки коллекции
// ---------------------------------------------------------------------------
class TFormTestApplicationPresetsCollectionEditor : public TFormCollectionEditor {
protected:

	// эту функцию надо будет переопределить, если понадобятся специальные настройки коллекции
	virtual void __fastcall CloneSourceCollection() {
		assert(FSourceCollection != NULL);
		FOwnedCollection = new TTestApplicationPresetItems(this);
		FOwnedCollection->Assign(FSourceCollection);

	}

public: // User declarations
	__fastcall TFormTestApplicationPresetsCollectionEditor(TComponent* Owner, TOwnedCollection *AOwnedCollection) : TFormCollectionEditor(Owner,
		AOwnedCollection) {
	}

};

// ---------------------------------------------------------------------------
class TTestApplicationPresetesPropEditor : public TOwnedCollectionPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		TTestApplicationPresetItems * ATestApplicationPresetItems = dynamic_cast<TTestApplicationPresetItems*>(this->GetObjectValue(0));
		if (ATestApplicationPresetItems) {
			return ATestApplicationPresetItems->DisplayName;
		}

		return TOwnedCollectionPropEditor::GetValue();
	}

	// должна создаться форма с редактором
	virtual TFormCollectionEditor *__fastcall DoCreateCollectionEditor(TOwnedCollection *ACollection) {
		return new TFormTestApplicationPresetsCollectionEditor(Application, ACollection);
	}
};

#endif
