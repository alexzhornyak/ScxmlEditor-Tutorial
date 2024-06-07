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

#ifndef UnitScxmlShapeH
#define UnitScxmlShapeH

//---------------------------------------------------------------------------
#include "UnitScxmlBaseShape.h"
//---------------------------------------------------------------------------

typedef enum {early, late} TScxmlBinding;

class TScxmlShape : public TVisualScxmlBaseShape {
__published:
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	UnicodeString FVersion;
	bool __fastcall IsVersionStored(bool);

	UnicodeString FDatamodel;
	void __fastcall SetDatamodel(UnicodeString val);

	TScxmlBinding FBinding;

    UnicodeString FTempInvokeID;
	void __fastcall SetTempInvokeID(UnicodeString val);

	UnicodeString FInvokeID;
	UnicodeString __fastcall GetFinalInvokeID(void);
	void __fastcall SetInvokeID(UnicodeString val);

	UnicodeString __fastcall GetXmlns(void);
	UnicodeString __fastcall GetVersion(void);

	UnicodeString __fastcall GetSyntaxScheme(void);

protected:
	virtual TColor __fastcall GetClusterColor(void);	/* override */
	virtual TColor __fastcall GetHeadColor(void);	/* override */
	virtual bool __fastcall IsCluster(void);        /* override */

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctScxml; }

	virtual void __fastcall SetStateId(UnicodeString sVal);

	virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D * ACanvas, const Types::TRect & R);

    inline virtual bool __fastcall GetOccursOnce(void) { return true; }

	void __fastcall SyncByInvokeID(void);

	friend class TScxmlInvokeIDPropEditor;

public:

	__fastcall virtual TScxmlShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TScxmlShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

    virtual void __fastcall SetDefaultSize();

    void __fastcall SaveScxmlNameToProject();

	virtual UnicodeString __fastcall XMLAttrNameToPropName(UnicodeString sAttrName);
	virtual UnicodeString __fastcall PropNameToXMLAttrName(UnicodeString sPropName);

	inline bool __fastcall IsInvokeIdStored(void) const { return !FInvokeID.IsEmpty(); }

	__property UnicodeString SyntaxScheme = {read=GetSyntaxScheme};
	__property UnicodeString TempInvokeID = {read=FTempInvokeID, write=SetTempInvokeID};
	__property UnicodeString StoredInvokeID = {read=FInvokeID};

__published:
	__property UnicodeString ScxmlName = {read=GetSimpleText,write=SetStateId};
	__property Initial;
	__property UnicodeString Xmlns = {read=GetXmlns};
	__property UnicodeString Version = {read=GetVersion, write=FVersion, stored=IsVersionStored};
	__property UnicodeString Datamodel = {read=FDatamodel,write=SetDatamodel};
	__property TScxmlBinding Binding = {read=FBinding,write=FBinding, default=TScxmlBinding::early};

	__property UnicodeString InvokeID = {read=GetFinalInvokeID,write=SetInvokeID, stored=IsInvokeIdStored};
};

class TScxmlDatamodelPropEditor : public TLMDStringPropEditor
{
protected:
	virtual Lmdinspropinsp::TLMDPropAttrs __fastcall GetAttrs(void);
	virtual void __fastcall GetValues(Classes::TStrings* AValues);
};

class TScxmlInvokeIDPropEditor : public TLMDStringPropEditor
{
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
	virtual Lmdinspropinsp::TLMDPropAttrs __fastcall GetAttrs(void);
	virtual void __fastcall Edit(void);
};

#endif
