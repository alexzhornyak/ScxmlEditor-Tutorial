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

#ifndef UnitProtocolBindingH
#define UnitProtocolBindingH
//---------------------------------------------------------------------------

#include <LMDXML.hpp>
#include <LMDInsPropEditors.hpp>
#include "UnitScxmlBaseShape.h"
#include "UnitTriggers.h"

typedef enum {pctLogic, pctInteger, pctDouble, pctString, pctIntegerArray, pctDoubleArray, pctOctetArray} TProtocolControlType;
typedef enum {pbtConsumer, pbtSupplier} TProtocolBindType;
typedef Set<TProtocolBindType, pbtConsumer, pbtSupplier> TProtocolBindTypes;

typedef enum {ctTSComm, ctZMQ, ctMulticast} TChannelType;

// ---------------------------------------------------------------------------
class TProtocolControlBinding : public TPersistent
{
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:

	bool FActive;
	void __fastcall SetActive(bool bVal);
	TProtocolControlType FProtocolControlType;
	UnicodeString FControlName;
	void __fastcall SetControlName(UnicodeString sVal);

	void __fastcall SetScxmlName(UnicodeString sVal);
	UnicodeString __fastcall GetScxmlName();
	bool __fastcall IsScxmlNameStored(void) { return !FScxmlName.IsEmpty(); }

	int FArraySize;
	int FArrayElement;
	TProtocolBindTypes FBindTypes;
	TProtocolBindTypes FTestTypes;

	TChannelType FChannelType;
	void __fastcall SetChannelType(TChannelType val);

	bool FUseSessionServer;
	AnsiString FSessionNodePath;
	AnsiString FSessionNodeAttribute;

	UnicodeString FComment;
	UnicodeString FMin;
	UnicodeString FMax;
	UnicodeString FCharset;

    UnicodeString FCustomAttributes;

	void __fastcall SetScxmlInvokerIDs(UnicodeString val);

	UnicodeString FStateMachineID;
	void __fastcall SetStateMachineID(UnicodeString val);

	bool FDoNotSendToScxml;
	void __fastcall SetDoNotSendToScxml(bool val);

	UnicodeString __fastcall GetStateMachineName(void);
	void __fastcall SetStateMachineName(UnicodeString val);
	bool __fastcall	IsStateMachineNameStored(void);

	bool __fastcall GetActive(void);
	UnicodeString __fastcall GetControlName(void);

	UnicodeString __fastcall GetSoftConsoleParam();

	TObject *FParent;

	void __fastcall SaveXMLItem(ILMDXmlElement *ANodeContainer, const UnicodeString &sType);

	void __fastcall UpdateParentSimpleText(void);
	void __fastcall ResetToDefaultClick(TObject *Sender);

protected:
	UnicodeString FStateMachineName;
	UnicodeString FScxmlInvokerIDs;
	UnicodeString FScxmlName;

	friend class TProtocolWeakBindingShape;

public:

	__fastcall TProtocolControlBinding(TObject *AParent);

	virtual void __fastcall Assign(TPersistent* Source);

	// краткая информация о подписке
	virtual const UnicodeString __fastcall GetDisplayName(void);

	UnicodeString __fastcall GetSessionServerType();

	void __fastcall ToXML(ILMDXmlElement *ANodeContainer);

	void __fastcall OnCopyFromClipboardClick(TObject *Sender);
	void __fastcall OnCopyToClipboardClick(TObject *Sender);

	__property TObject *ParentObject = {read=FParent};

    bool EditBinding();

	// SG_XXX_ID будет заменено на Inp.XXX
	void __fastcall MakeScxmlNameFromControlName(const bool bIgnoreCanSuffix);

__published:

	 __property bool Active = {read=GetActive, write=SetActive, default=false};
	 __property UnicodeString ControlName = {read=GetControlName, write=SetControlName};
	 __property UnicodeString Comment = {read=FComment, write=FComment};
	 __property UnicodeString ScxmlName = {read=GetScxmlName, write=SetScxmlName, stored=IsScxmlNameStored};
	 __property UnicodeString StateMachineName = {read=GetStateMachineName, write=SetStateMachineName, stored=IsStateMachineNameStored};
	 __property UnicodeString StateMachineID = {read=FStateMachineID, write=SetStateMachineID};
	 __property UnicodeString ScxmlInvokerIDs = {read=FScxmlInvokerIDs, write=SetScxmlInvokerIDs};
	 __property bool DoNotSendToScxml = {read=FDoNotSendToScxml, write=SetDoNotSendToScxml, default=false};
	 __property TProtocolControlType ProtocolControlType = {read=FProtocolControlType, write=FProtocolControlType, default=pctInteger};
	 __property int ArraySize = {read=FArraySize, write=FArraySize, default=0};
	 __property int ArrayElement = {read=FArrayElement, write=FArrayElement, default=-1};
	 __property TProtocolBindTypes BindTypes = {read=FBindTypes, write=FBindTypes, default=1};
	 __property TChannelType ChannelType = {read=FChannelType, write=SetChannelType, default=ctTSComm};
	 __property bool UseSessionServer = {read=FUseSessionServer, write=FUseSessionServer, default=false}; // использовать подписку в сервере сессий
	 __property AnsiString SessionNodePath = {read=FSessionNodePath, write=FSessionNodePath}; // путь в дереве сессии
	 __property AnsiString SessionNodeAttribute = {read=FSessionNodeAttribute, write=FSessionNodeAttribute}; // название аттрибута в дереве сессии
	 __property UnicodeString Min = {read=FMin, write=FMin};
	 __property UnicodeString Max = {read=FMax, write=FMax};
	 __property UnicodeString Charset = {read=FCharset, write=FCharset};
	 __property UnicodeString CustomAttributes = {read=FCustomAttributes, write=FCustomAttributes};
	 __property UnicodeString SoftConsoleParam = {read=GetSoftConsoleParam};
};

class TProtocolControlBindingPropertyEditor : public TLMDClassPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit();
};

class ProtocolControlBindingControlNameEditor : public TLMDStringPropEditor
{
protected:
	virtual Lmdinspropinsp::TLMDPropAttrs __fastcall GetAttrs(void);
	virtual void __fastcall GetValues(Classes::TStrings* AValues);

};

class ProtocolControlBindingCharsetEditor : public TLMDStringPropEditor
{
protected:
	virtual Lmdinspropinsp::TLMDPropAttrs __fastcall GetAttrs(void);
	virtual void __fastcall GetValues(Classes::TStrings* AValues);

};

//---------------------------------------------------------------------------
class TProtocolBindingShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	TProtocolControlBinding *FProtocolControlBinding;
	inline void __fastcall SetProtocolControlBinding(TProtocolControlBinding *val) { FProtocolControlBinding->Assign(val); };

    TContentTrigger *FContentTrigger;
	inline void __fastcall SetContentTrigger(TContentTrigger *val) { FContentTrigger->Assign(val); };

	TParamTriggerItems *FParams;
	inline void __fastcall SetParams(TParamTriggerItems *val) { FParams->Assign(val); };

protected:

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctProtocolBinding; }

	virtual UnicodeString __fastcall GetOutValue();
	virtual UnicodeString __fastcall GetClipboardInfo(void);

	virtual TColor __fastcall GetNormalColor(void);
	virtual TColor __fastcall GetNormalFontColor(void);
	virtual TFontStyles __fastcall GetNormalFontStyle();
	virtual TColor __fastcall GetNormalBorderColor(void);

	virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R); /* inherited */

public:

	__fastcall virtual TProtocolBindingShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TProtocolBindingShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

   	virtual void __fastcall UpdateSimpleText();

	void __fastcall MakeDefaultContentTriggersByControlType(void);

__published:
	__property TProtocolControlBinding *	ProtocolControlBinding = {read=FProtocolControlBinding, write=SetProtocolControlBinding};
	__property TContentTrigger *			ContentTrigger = {read=FContentTrigger, write=SetContentTrigger};
	__property TParamTriggerItems *			Params = {read=FParams, write=SetParams};
};

//---------------------------------------------------------------------------
class TProtocolWeakBindingShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:

	UnicodeString FScxmlName;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT_TYPE(UnicodeString,ScxmlName);

	UnicodeString FStateMachineName;
	UnicodeString __fastcall GetStateMachineName(void);
	SET_SHAPE_VALUE_UPDATESIMPLETEXT_TYPE(UnicodeString,StateMachineName);
	bool __fastcall IsStateMachineStored(void);

	UnicodeString FStateMachineID;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT_TYPE(UnicodeString,StateMachineID);

    UnicodeString FScxmlInvokerIDs;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT_TYPE(UnicodeString,ScxmlInvokerIDs);

protected:

	virtual TColor __fastcall GetNormalFontColor(void);
	virtual TFontStyles __fastcall GetNormalFontStyle();
	virtual TColor __fastcall GetNormalBorderColor(void);

	inline virtual TStateChildType __fastcall GetStateChildType(void) { return sctWeakProtocolBinding; }

	virtual UnicodeString __fastcall GetOutValue();
	virtual UnicodeString __fastcall GetClipboardInfo(void);

	bool __fastcall GetActive(void) { return !ScxmlName.IsEmpty(); }

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

public:

	__fastcall TProtocolWeakBindingShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TProtocolWeakBindingShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	virtual void __fastcall UpdateSimpleText();

	void __fastcall ToXML(ILMDXmlElement *ANodeContainer);

__published:
	__property bool Active = {read=GetActive};
	__property UnicodeString ScxmlName = {read=FScxmlName,write=SetScxmlName};
	__property UnicodeString StateMachineName = {read=GetStateMachineName, write=SetStateMachineName, stored=IsStateMachineStored};
	__property UnicodeString StateMachineID = {read=FStateMachineID, write=SetStateMachineID};
	__property UnicodeString ScxmlInvokerIDs = {read=FScxmlInvokerIDs, write=SetScxmlInvokerIDs};
};

//---------------------------------------------------------------------------
class TSetValueShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FTarget;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Target,TargetExpr);
	UnicodeString FTargetExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(TargetExpr,Target);

	UnicodeString FType;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Type,TypeExpr);
	UnicodeString FTypeExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(TypeExpr,Type);

	UnicodeString FValue;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Value,ValueExpr);
	UnicodeString FValueExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(ValueExpr,Value);

	UnicodeString FPath;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Path,PathExpr);
	UnicodeString FPathExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(PathExpr,Path);

	TProtocolControlBinding *FProtocolControlBinding;
	inline void __fastcall SetProtocolControlBinding(TProtocolControlBinding *val) { FProtocolControlBinding->Assign(val); };

protected:

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctSetValue; }

	virtual UnicodeString __fastcall GetOutValue();

	virtual TColor __fastcall GetNormalFontColor(void);
	virtual TFontStyles __fastcall GetNormalFontStyle();

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

public:

	__fastcall virtual TSetValueShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TSetValueShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString Target = {read=FTarget,write=SetTarget};
	__property UnicodeString Targetexpr = {read=FTargetExpr,write=SetTargetExpr};
	__property UnicodeString Type = {read=FType,write=SetType};
	__property UnicodeString Typeexpr = {read=FTypeExpr,write=SetTypeExpr};
    __property UnicodeString Path = {read=FPath,write=SetPath};
	__property UnicodeString Pathexpr = {read=FPathExpr,write=SetPathExpr};
	__property UnicodeString Value = {read=FValue,write=SetValue};
	__property UnicodeString Valueexpr = {read=FValueExpr,write=SetValueExpr};

	__property TProtocolControlBinding *	ProtocolControlBinding = {read=FProtocolControlBinding, write=SetProtocolControlBinding};
};

class SetValueTypePropEditor : public TLMDStringPropEditor
{
protected:
	virtual Lmdinspropinsp::TLMDPropAttrs __fastcall GetAttrs(void);
	virtual void __fastcall GetValues(Classes::TStrings* AValues);
};

//---------------------------------------------------------------------------
class TSessionStateShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FEvent;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(Event,EventExpr);
	UnicodeString FEventExpr;
	SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(EventExpr,Event);
protected:

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctSessionState; }

	virtual UnicodeString __fastcall GetOutValue();

    virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

public:

	__fastcall virtual TSessionStateShape(Classes::TComponent* AOwner);

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString Event = {read=FEvent,write=SetEvent};
	__property UnicodeString Eventexpr = {read=FEventExpr,write=SetEventExpr};
};

//---------------------------------------------------------------------------
class TVirtualFolderShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	UnicodeString FCaption;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Caption);
protected:

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctVirtualFolder; }

	virtual TColor __fastcall GetNormalFontColor(void);
	virtual TFontStyles __fastcall GetNormalFontStyle();

	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

	inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TVirtualFolderShape(Classes::TComponent* AOwner);

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	virtual void __fastcall UpdateSimpleText();

	void EditBindings(void);

	virtual void __fastcall DefaultEditProperty(void);

	static bool __fastcall ImportBindingsAndFolders(TScxmlBaseShape *ARoot);

	static void RecursiveAddProtocolBindingsAndFolders(TScxmlBaseShape *AParent,
													   TNodeShapeList *AShapeList,
													   int iRecursiveLevel,
													   bool bIgnoreCanSuffix);

__published:
	__property UnicodeString Caption = {read=FCaption,write=SetCaption};
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern std::vector<TProtocolBindingShape *> __fastcall CreateProtocolBindingShapeFromJSON(TScxmlBaseShape *AParent, const UnicodeString &sJSONText);

#endif
