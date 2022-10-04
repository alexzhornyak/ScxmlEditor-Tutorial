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

#ifndef UnitSyntaxStringH
#define UnitSyntaxStringH
//---------------------------------------------------------------------------

#include <LmdInsPropInsp.hpp>
#include "LMDInsPropEditors.hpp"
#include "LMDXml.hpp"

typedef enum {xttText, xttCDATA, xttXML, xttSCXML} TXMLTextType;

//---------------------------------------------------------------------------
class TSyntaxString : public TPersistent
{
__published:
	void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FText;
	inline void __fastcall SetText(UnicodeString val) { FText = val; if (OnTextChanged) OnTextChanged(this); }

	UnicodeString FSyntaxScheme;
	void __fastcall SetSyntaxScheme(UnicodeString val);

	TXMLTextType FType;
	inline void __fastcall SetType(TXMLTextType val) { if (!FIsSchemeLocked) { FType=val; if (OnTextChanged) OnTextChanged(this); } }

	void __fastcall ReadScxmlStreamProc(TStream* Stream);
	void __fastcall WriteScxmlStreamProc(TStream* Stream);

	bool __fastcall GetIsScxmlGui(void) { return FScxmlStream->Size>0; }

    /* специальная хрень для записи из редактора */
	TComponent *FOwnerInstance;
	UnicodeString FOwnerPropertyName;

protected:
	TMemoryStream *FScxmlStream;
	bool FIsSchemeLocked;
	bool __fastcall IsSyntaxSchemeStored(void);
	UnicodeString __fastcall GetSyntaxScheme(void);

	virtual void __fastcall DefineProperties(Classes::TFiler* Filer);

	friend class TSyntaxStringPropEditor;
	friend class TSyntaxStringClassEditor;
public:
	__fastcall TSyntaxString(TComponent *AOwner, const UnicodeString &sOwnerPropName);
	__fastcall TSyntaxString(TComponent *AOwner, const UnicodeString &sOwnerPropName, bool bLocked, const UnicodeString &SyntaxScheme, const TXMLTextType AType=TXMLTextType::xttText);
	__fastcall virtual ~TSyntaxString(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);

	DYNAMIC TPersistent* __fastcall GetOwner(void) { return FOwnerInstance;} /* override*/

	bool __fastcall EditXMLText();

	void __fastcall AppendToElement(ILMDXmlElement *ANode);

	void __fastcall Clear();

	void __fastcall OnScxmlTreeEditorModfied(TObject *Sender);

	TNotifyEvent OnTextChanged;

	__property TMemoryStream *ScxmlStream = {read=FScxmlStream};

__published:
	__property bool IsSchemeLocked={read=FIsSchemeLocked};
	__property bool IsScxmlGui = {read=GetIsScxmlGui};
	__property UnicodeString Text = {read=FText, write=SetText};
	__property TXMLTextType Type = {read=FType,write=SetType,default=TXMLTextType::xttText};
	__property UnicodeString SyntaxScheme = {read=GetSyntaxScheme,write=SetSyntaxScheme, stored=IsSyntaxSchemeStored};
};

//---------------------------------------------------------------------------
class TSyntaxStringClassEditor : public TLMDClassPropEditor {
private:

protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
    virtual Lmdtypes::TLMDString __fastcall GetValue(void);
public:
	virtual void __fastcall Edit(void);
};

//---------------------------------------------------------------------------
class TSyntaxSchemeComboEditor : public TLMDStringPropEditor
{
protected:
	virtual void __fastcall SetValue(const Lmdtypes::TLMDString Value);
	virtual void __fastcall GetValues(TLMDWideStrings* AValues);
	virtual TLMDPropAttrs __fastcall GetAttrs();
public:
};

//---------------------------------------------------------------------------
class TSyntaxStringPropEditor : public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
};

//---------------------------------------------------------------------------


#endif
