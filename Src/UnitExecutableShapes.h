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

#ifndef UnitExecutableShapesH
#define UnitExecutableShapesH
//---------------------------------------------------------------------------
#include "UnitScxmlBaseShape.h"

//---------------------------------------------------------------------------
class TRaiseShape: public TChildScxmlBaseShape {
__published:
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	UnicodeString FEvent;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Event);
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctRaise; }

	virtual UnicodeString __fastcall GetOutValue() { return Event; }

    virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);
	virtual UnicodeString __fastcall GetEditByDefaultValue(void) { return L"Event"; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TRaiseShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TRaiseShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

    void EditEvent(void);

__published:
	__property UnicodeString Event = {read=FEvent,write=SetEvent};
};

//---------------------------------------------------------------------------
class TIfShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FCond;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Cond);
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctIf; }
	virtual UnicodeString __fastcall GetOutValue() { return Cond; }
	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);
	virtual UnicodeString __fastcall GetEditByDefaultValue(void) { return L"Cond"; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TIfShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TIfShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString Cond = {read=FCond,write=SetCond};
};


//---------------------------------------------------------------------------
class TElseIfShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FCond;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Cond);
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctElseif; }
	virtual UnicodeString __fastcall GetOutValue() { return Cond; }
	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);
	virtual UnicodeString __fastcall GetEditByDefaultValue(void) { return L"Cond"; }

	inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TElseIfShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TElseIfShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString Cond = {read=FCond,write=SetCond};
};

//---------------------------------------------------------------------------
class TElseShape: public TChildScxmlBaseShape {
private:
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctElse; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TElseShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TElseShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
};

//---------------------------------------------------------------------------
class TForeachShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FArray;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Array);
	UnicodeString FForeachItem;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(ForeachItem);
	UnicodeString FForeachIndex;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(ForeachIndex);
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctForeach; }
	virtual UnicodeString __fastcall GetOutValue() { return Array+";"+ForeachItem+";"+ForeachIndex; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TForeachShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TForeachShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);
	virtual UnicodeString __fastcall XMLAttrNameToPropName(UnicodeString sAttrName);
	virtual UnicodeString __fastcall PropNameToXMLAttrName(UnicodeString sPropName);

__published:
	__property UnicodeString Array = {read=FArray,write=SetArray};
	__property UnicodeString ForeachItem = {read=FForeachItem,write=SetForeachItem};
	__property UnicodeString ForeachIndex = {read=FForeachIndex,write=SetForeachIndex};
};

//---------------------------------------------------------------------------
class TLogShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FLabel;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Label);
	UnicodeString FExpression;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Expression);

protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctLog; }
	virtual UnicodeString __fastcall GetOutValue();

    virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);
	virtual UnicodeString __fastcall GetEditByDefaultValue(void) { return L"Expr"; }

	inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TLogShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TLogShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString Label = {read=FLabel,write=SetLabel};
	__property UnicodeString Expr = {read=FExpression,write=SetExpression};
};

//---------------------------------------------------------------------------
class TAssignShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FLocation;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Location);

	UnicodeString FExpression;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Expression);

protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctAssign; }
	virtual UnicodeString __fastcall GetOutValue();

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

	inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TAssignShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TAssignShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

    virtual void __fastcall ValidateXMLText(void);

__published:
	__property UnicodeString Location = {read=FLocation,write=SetLocation};
	__property UnicodeString Expr = {read=FExpression,write=SetExpression};
	__property XMLText;
};

#endif
