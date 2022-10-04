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

#ifndef UnitExternalCommunicationShapesH
#define UnitExternalCommunicationShapesH
//---------------------------------------------------------------------------
#include "UnicodeStringHash.h"
#include <boost/tuple/tuple.hpp>

#include "UnitScxmlBaseShape.h"
#include "UnitProtocolBinding.h"
#include "LMDInsPropEditors.hpp"

class TSendTypePropEditor : public TLMDStringPropEditor
{
protected:
	virtual Lmdinspropinsp::TLMDPropAttrs __fastcall GetAttrs(void);
	virtual void __fastcall GetValues(Classes::TStrings* AValues);

};

UnicodeString SendTypeToSetValueType(const UnicodeString &sSendType);
UnicodeString SetValueTypeToSendType(const UnicodeString &sSetValueType);

//---------------------------------------------------------------------------
class TSendShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FEvent;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Event,EventExpr);
	UnicodeString FEventExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(EventExpr,Event);

	UnicodeString FTarget;
	void __fastcall SetTarget(UnicodeString val);
	UnicodeString FTargetExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(TargetExpr,Target);

	UnicodeString FType;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Type,TypeExpr)
	UnicodeString FTypeExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(TypeExpr,Type);

	UnicodeString FId;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Id,IdLocation)
	UnicodeString FIdLocation;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(IdLocation,Id);

	UnicodeString FDelay;
	void __fastcall SetDelay(UnicodeString val);
	UnicodeString FDelayExpr;
	void __fastcall SetDelayExpr(UnicodeString val);

	UnicodeString FNameList;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(NameList);

	TProtocolControlBinding *FProtocolControlBinding;
	inline void __fastcall SetProtocolControlBinding(TProtocolControlBinding *val) { FProtocolControlBinding->Assign(val); };

	enum {typesColor, typesFontStyles};
	typedef boost::unordered_map< UnicodeString, boost::tuple< TColor, TFontStyles >, boost::hash<UnicodeString> > TypeSettings;
	TypeSettings FTypeSettings;

protected:

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctSend; }

	virtual UnicodeString __fastcall GetOutValue();

	virtual TColor __fastcall GetNormalFontColor(void);
	virtual TFontStyles __fastcall GetNormalFontStyle();

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

	inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TSendShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TSendShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	virtual void __fastcall ValidateChild(TScxmlBaseShape *AChild);

__published:
	__property UnicodeString Event = {read=FEvent,write=SetEvent};
	__property UnicodeString Eventexpr = {read=FEventExpr,write=SetEventExpr};
	__property UnicodeString Target = {read=FTarget,write=SetTarget};
	__property UnicodeString Targetexpr = {read=FTargetExpr,write=SetTargetExpr};
	__property UnicodeString Type = {read=FType,write=SetType};
	__property UnicodeString Typeexpr = {read=FTypeExpr,write=SetTypeExpr};
	__property UnicodeString Id = {read=FId,write=SetId};
	__property UnicodeString Idlocation = {read=FIdLocation,write=SetIdLocation};
	__property UnicodeString Delay = {read=FDelay,write=SetDelay};
	__property UnicodeString Delayexpr = {read=FDelayExpr,write=SetDelayExpr};
	__property UnicodeString Namelist = {read=FNameList,write=SetNameList};
	__property TProtocolControlBinding *	ProtocolControlBinding = {read=FProtocolControlBinding, write=SetProtocolControlBinding};
};

//---------------------------------------------------------------------------
class TCancelShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FSendId;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(SendId,SendIdExpr);
	UnicodeString FSendIdExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(SendIdExpr,SendId);
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctCancel; }

	virtual UnicodeString __fastcall GetOutValue();

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TCancelShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TCancelShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

    void EditSendIDLineProperty(const UnicodeString &sPropName);

__published:
	__property UnicodeString Sendid = {read=FSendId,write=SetSendId};
	__property UnicodeString Sendidexpr = {read=FSendIdExpr,write=SetSendIdExpr};
};

//---------------------------------------------------------------------------
class TInvokeShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor);
private:
	bool FDisableFlatBuild;

	UnicodeString FSrc;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Src,SrcExpr);
	UnicodeString FSrcExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(SrcExpr,Src);

	UnicodeString FType;
	SET_SHAPE_VALUE_PAIR(Type,TypeExpr,false);
	UnicodeString FTypeExpr;
	SET_SHAPE_VALUE_PAIR(TypeExpr,Type,false);

	UnicodeString FId;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Id,IdLocation);
	UnicodeString FIdLocation;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(IdLocation,Id);

	UnicodeString FNameList;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(NameList);

	bool FAutoForward;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT_TYPE(bool,AutoForward);

	UnicodeString __fastcall GetLocalSrc(void);

protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctInvoke; }

	virtual UnicodeString __fastcall GetOutValue();

	virtual TColor __fastcall GetNormalFontColor(void);
	virtual TFontStyles __fastcall GetNormalFontStyle();
	virtual TColor __fastcall GetNormalColor(void);

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TInvokeShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TInvokeShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	void EditSrc(void);

	__property UnicodeString LocalSrc = {read=GetLocalSrc};

__published:
	__property UnicodeString Type = {read=FType,write=SetType};
	__property UnicodeString Typeexpr = {read=FTypeExpr,write=SetTypeExpr};
	__property UnicodeString Src = {read=FSrc,write=SetSrc};
	__property UnicodeString Srcexpr = {read=FSrcExpr,write=SetSrcExpr};
	__property UnicodeString Id = {read=FId,write=SetId};
	__property UnicodeString Idlocation = {read=FIdLocation,write=SetIdLocation};
	__property UnicodeString Namelist = {read=FNameList,write=SetNameList};
	__property bool Autoforward = {read=FAutoForward,write=SetAutoForward, default=false};
	__property bool DisableFlatBuild = {read=FDisableFlatBuild, write=FDisableFlatBuild, default=false};
};

//---------------------------------------------------------------------------
class TFinalizeShape: public TChildScxmlBaseShape {
private:
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctFinalize; }

	inline virtual bool __fastcall GetOccursOnce(void) { return true; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TFinalizeShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TFinalizeShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
};


#endif
