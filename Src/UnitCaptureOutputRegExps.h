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

#ifndef UnitCaptureOutputRegExpsH
#define UnitCaptureOutputRegExpsH
//---------------------------------------------------------------------------

#include "UnitFormCollectionEditor.h"

#include "ScxmlMsgTypes.h"

class TCaptureOutputRegexItem : public TCollectionItem
{
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	UnicodeString 		FRegExp;
	int 				FGroupMachine;
	int 				FGroupIdentifier;
	TScxmlMsgType 		FCaptureMsgType;

	inline bool __fastcall GetEnabled(void) { return !FRegExp.IsEmpty() && FCaptureMsgType!=smttUnknown && FGroupIdentifier!=-1; }

protected:
     virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TCaptureOutputRegexItem(TCollection* Collection);
	 virtual __fastcall ~TCaptureOutputRegexItem();

	 virtual void __fastcall Assign(TPersistent* Source);

	 bool __fastcall Parse(const UnicodeString &sText, UnicodeString &sMachineName, UnicodeString &sIdentifier);

__published:

	 // Ќ≈ «јЅџ¬ј“№ ƒќЅј¬Ћя“№ —¬ќ…—“¬ј в Assign !!!
	__property bool						Enabled = {read=GetEnabled};
	__property UnicodeString 			RegExp = {read=FRegExp,write=FRegExp};
	__property int			 			GroupMachine = { read=FGroupMachine, write=FGroupMachine, default=-1};
	__property int			 			GroupIdentifier = { read=FGroupIdentifier, write=FGroupIdentifier, default=-1};
	__property TScxmlMsgType 			CaptureMsgType = {read=FCaptureMsgType, write=FCaptureMsgType, default=smttUnknown};

};

class TCaptureOutputRegexItems: public TOwnedCollection
{
private:
	bool FEnabled;
	bool __fastcall GetEnabled(void);
protected:
	TCaptureOutputRegexItem* __fastcall GetItem(int Index);
	void __fastcall SetItem(int Index, TCaptureOutputRegexItem* Value);
	virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TCaptureOutputRegexItems(TPersistent *AOwner);

	 virtual void __fastcall Assign(TPersistent* Source);

	 TCaptureOutputRegexItem* __fastcall Add(void);
	 TCaptureOutputRegexItem* __fastcall Insert(int Index);
	 __property TCaptureOutputRegexItem* Items[int Index] = {read=GetItem, write=SetItem};

__published:

	 // ѕ–»ћ≈–: «десь можно объ€вить свои свойства, если необходимо, чтобы они сохран€лись в конфиг
	 __property bool Enabled = {read=GetEnabled, write=FEnabled,default=false};
	 __property UnicodeString DisplayName = {read=GetDisplayName};
};

// ---------------------------------------------------------------------------
// ѕереопределение настройки коллекции
// ---------------------------------------------------------------------------
class TFormCaptureOutputRegexesCollectionEditor : public TFormCollectionEditor {
protected:

	// эту функцию надо будет переопределить, если понадоб€тс€ специальные настройки коллекции
	virtual void __fastcall CloneSourceCollection() {
		assert(FSourceCollection != NULL);
		FOwnedCollection = new TCaptureOutputRegexItems(this);
		FOwnedCollection->Assign(FSourceCollection);

	}

public: // User declarations
	__fastcall TFormCaptureOutputRegexesCollectionEditor(TComponent* Owner, TOwnedCollection *AOwnedCollection) : TFormCollectionEditor(Owner,
		AOwnedCollection) {
	}

};

// ---------------------------------------------------------------------------
class TCaptureOutputRegexesPropEditor : public TOwnedCollectionPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		TCaptureOutputRegexItems * ACaptureOutputRegexItems = dynamic_cast<TCaptureOutputRegexItems*>(this->GetObjectValue(0));
		if (ACaptureOutputRegexItems) {
			return ACaptureOutputRegexItems->DisplayName;
		}

		return TOwnedCollectionPropEditor::GetValue();
	}

	// должна создатьс€ форма с редактором
	virtual TFormCollectionEditor *__fastcall DoCreateCollectionEditor(TOwnedCollection *ACollection) {
		return new TFormCaptureOutputRegexesCollectionEditor(Application, ACollection);
	}
};

#endif
