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

#ifndef UnitSettingsBinaryItemsH
#define UnitSettingsBinaryItemsH
//---------------------------------------------------------------------------
#include "UnitFormCollectionEditor.h"

class TSettingsRegistryItem : public TCollectionItem
{
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:

	UnicodeString FKey;

	TStringList *FData;
	TStringList * __fastcall GetData(void) { return FData; }
	void __fastcall SetData(TStrings *val) { FData->Assign(val); }

protected:
	 virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TSettingsRegistryItem(TCollection* Collection);
	 virtual __fastcall ~TSettingsRegistryItem();

	 virtual void __fastcall Assign(TPersistent* Source);

__published:

	 // НЕ ЗАБЫВАТЬ ДОБАВЛЯТЬ СВОЙСТВА в Assign !!!
	__property UnicodeString Key = {read=FKey, write=FKey};
	__property TStrings * Data = {read=GetData, write=SetData};
};

class TSettingsRegistryItems: public TOwnedCollection
{
private:
	TSettingsRegistryItem* __fastcall GetMap(UnicodeString Key);
	void __fastcall SetMap(UnicodeString Key, TSettingsRegistryItem* Value);

protected:
	TSettingsRegistryItem* __fastcall GetItem(int Index);
	void __fastcall SetItem(int Index, TSettingsRegistryItem* Value);
	virtual System::UnicodeString __fastcall GetDisplayName(void);
public:

	 __fastcall TSettingsRegistryItems(TComponent *AOwner);

	 virtual void __fastcall Assign(TPersistent* Source);

	 TSettingsRegistryItem* __fastcall Add(void);
	 TSettingsRegistryItem* __fastcall Insert(int Index);

	 __property TSettingsRegistryItem* Items[int Index] = {read=GetItem, write=SetItem};
	 __property TSettingsRegistryItem* Map[UnicodeString Key] = {read=GetMap, write=SetMap};

__published:

	 // ПРИМЕР: Здесь можно объявить свои свойства, если необходимо, чтобы они сохранялись в конфиг
	 __property UnicodeString DisplayName = {read=GetDisplayName};
};

#endif
