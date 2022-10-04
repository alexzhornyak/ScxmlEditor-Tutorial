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

#ifndef UnitDataModelStateShapeH
#define UnitDataModelStateShapeH
//---------------------------------------------------------------------------
#include "UnitScxmlBaseShape.h"
//---------------------------------------------------------------------------
class TDatamodelStateShape : public TChildScxmlBaseShape {
protected:
	inline virtual TStateChildType __fastcall GetStateChildType(void) { return sctDatamodel; }

	inline virtual bool __fastcall GetOccursOnce(void) { return true; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TDatamodelStateShape(Classes::TComponent* AOwner);

	virtual void __fastcall Assign(Classes::TPersistent* Source);
};


//---------------------------------------------------------------------------
class TDataChildShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor);
private:
    bool FDisableFlatBuild;

	UnicodeString FDataId;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(DataId);

	UnicodeString FSrc;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Src,Expr);

	UnicodeString FExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Expr,Src);

protected:

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctData; }

	virtual void __fastcall SetXMLText(TSyntaxString *val); /*overloaded*/

	virtual UnicodeString __fastcall GetOutValue();

	virtual TColor __fastcall GetNormalColor(void);

	virtual UnicodeString __fastcall GetClipboardInfo(void);

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

	virtual void __fastcall ValidateXMLText(void);

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TDataChildShape(Classes::TComponent* AOwner);

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	void EditSrc(void);

__published:
	__property UnicodeString Id = {read=FDataId,write=SetDataId};
	__property UnicodeString Src = {read=FSrc,write=SetSrc};
	__property UnicodeString Expr = {read=FExpr,write=SetExpr};
	__property XMLText;
	__property bool DisableFlatBuild = {read=FDisableFlatBuild, write=FDisableFlatBuild, default=false};
};

//---------------------------------------------------------------------------
class TDonedataShape: public TChildScxmlBaseShape {
private:
protected:
	inline virtual TStateChildType __fastcall GetStateChildType(void) { return sctDonedata; }

	inline virtual bool __fastcall GetOccursOnce(void) { return true; }

	inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TDonedataShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TDonedataShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	virtual void __fastcall ValidateChild(TScxmlBaseShape *AChild);

__published:
};

//---------------------------------------------------------------------------
class TContentShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FExpr;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Expr);

protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctContent; }

    virtual void __fastcall SetXMLText(TSyntaxString *val); /*overloaded*/

	UnicodeString __fastcall GetOutValue();

    virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

	inline virtual bool __fastcall GetOccursOnce(void) { return true; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TContentShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TContentShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

    virtual void __fastcall ValidateXMLText(void);

__published:
	__property UnicodeString Expr = {read=FExpr,write=SetExpr};
	__property XMLText;
};


//---------------------------------------------------------------------------
class TParamShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FParamName;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(ParamName);
	UnicodeString FLocation;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Location,Expr);
	UnicodeString FExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Expr,Location);
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctParam; }
	UnicodeString __fastcall GetOutValue();

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TParamShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TParamShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);
	virtual UnicodeString __fastcall XMLAttrNameToPropName(UnicodeString sAttrName);
	virtual UnicodeString __fastcall PropNameToXMLAttrName(UnicodeString sPropName);

__published:
	__property UnicodeString ParamName = {read=FParamName,write=SetParamName};
	__property UnicodeString Location = {read=FLocation,write=SetLocation};
	__property UnicodeString Expr = {read=FExpr,write=SetExpr};
};

#endif
