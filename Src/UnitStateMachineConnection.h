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

#ifndef UnitStateMachineConnectionH
#define UnitStateMachineConnectionH
// ---------------------------------------------------------------------------

// без этого будет ругаться на TTreeConnection->DrawTextA
#undef DrawText // exclude Winapi DrawText

#include <TeeTree.hpp>
#include <LmdInsPropInsp.hpp>
#include "LMDInsPropEditors.hpp"
#include "UnitTriggers.h"
#include "UnitProtocolBinding.h"
#include "UnitInheritanceResolver.h"

class TFormTransitionEditor;
class TVisualScxmlBaseShape;

namespace Lmdxml{
	class ILMDXmlElement;
}

typedef enum {external,internal} TScxmlTransitionType;

typedef enum {stdtEvent, stdtCondition, stdtXML, stdtBindingName, stdtDescription, stdtMAXSIZE} TScxmlTransitionDisplayType;
typedef Set<TScxmlTransitionDisplayType, stdtEvent, stdtMAXSIZE> TScxmlTransitionDisplayTypes;

typedef enum { scaTop, scaBottom, scaLeft, scaRight, scaMAXSIZE }TSelfConnectionAlign;

typedef enum { cttLeft, cttRight, cttMiddle } TClipTextType;

typedef enum { tstNONE, tstINVERT_CONDITION, tstSIMPLE } TTransitionSwitchType;

UnicodeString TransitionSwitchTypeToString(const TTransitionSwitchType val) {
	switch(val) {
	case tstINVERT_CONDITION:
		return L"INVERT CONDITION";
	case tstSIMPLE:
		return L"SIMPLE";
	}
	return L"NONE";
}

UnicodeString SelfConnectionAlignToString(const TSelfConnectionAlign val) {
	switch(val) {
	case scaTop:
		return L"Top";
	case scaBottom:
		return L"Bottom";
	case scaLeft:
		return L"Left";
	case scaRight:
		return L"Right";
	}
	return L"UNKNOWN";
}

typedef enum {cotOverTop, cotOverBottom, cotOverLeft, cotOverRight} TConnectionOverType;

struct TSelfConnectionInfo {
	TSelfConnectionAlign Align;
	int LengthOffsetBegin;
	int LengthOffsetEnd;
	int TextOffset;
};

typedef struct {
	int MaxWidth;
	int MaxHeight;
	int TextX;
	int TextY;

	TPoint Polygon[4];
	TRect Bounds;
} TDrawTextData;

/* TStateMachineConnection */
class TStateMachineConnection : public TTreeConnection {
__published:
	// это событие вызывается для проверки фильтрации свойств
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor);
private:
	TFormTransitionEditor *FTransitionEditor;

	GUID * FGuid;
	UnicodeString FGuidStr;

   	bool FLocked;

	bool FInheritanceMismatch;
	void __fastcall SetInheritanceMismatch(bool val);
	TInheritanceResolver * FInheritanceResolver;
	inline void __fastcall SetInheritanceResolver(TInheritanceResolver *val) { FInheritanceResolver->Assign(val); }

  	UnicodeString FExtraParams;

	UnicodeString FXML;
	void __fastcall SetXML(UnicodeString val);

	void __fastcall ParseXML(const UnicodeString &sData);

	UnicodeString __fastcall GetTarget(void);

	UnicodeString FEvent;
	void __fastcall SetEvent(UnicodeString val);

	UnicodeString FCondition;
	void __fastcall SetCondition(UnicodeString val);

	UnicodeString FConditionBack;
	UnicodeString __fastcall GetConditionBack(void);
	UnicodeString __fastcall GetConditionBackFromSettingsThrow(const UnicodeString &sSyntaxScheme);
	void __fastcall SetConditionBack(UnicodeString val);
	inline bool __fastcall IsConditionBackStored(void) { return !FConditionBack.IsEmpty(); }
	UnicodeString __fastcall GetSyntaxSchemeThrow(void);

	UnicodeString FDescription;
	void __fastcall SetDescription(UnicodeString val);

    bool FAdjustableSides;

	bool FIsTargetStored;
	inline bool __fastcall GetIsTargetStored(void) { return (!IsSelfConnection) || FIsTargetStored; }
	inline void __fastcall SetIsTargetStored(bool val) { FIsTargetStored = val; UpdateAppearance(); }
	inline bool __fastcall IsTargetPropStored(void) { return IsSelfConnection  && FIsTargetStored; }

	bool __fastcall GetIsTargetDescendant(void) { return this->FromShape!=NULL && this->ToShape!=NULL && (this->ToShape->Parent == this->FromShape); }

	TScxmlTransitionType FType;
	void __fastcall SetType(TScxmlTransitionType val) { FType = val; UpdateAppearance(); }

	TContentTrigger *FContentTrigger;
	inline void __fastcall SetContentTrigger(TContentTrigger *val) { FContentTrigger->Assign(val); };

	TParamTriggerItems *FParams;
	inline void __fastcall SetParams(TParamTriggerItems *val) { FParams->Assign(val); };

	TProtocolControlBinding *FProtocolControlBinding;
	inline void __fastcall SetProtocolControlBinding(TProtocolControlBinding *val) { FProtocolControlBinding->Assign(val); };

	TScxmlTransitionDisplayTypes FScxmlTransitionDisplayTypes;
	void __fastcall SetScxmlTransitionDisplayTypes(TScxmlTransitionDisplayTypes ATypes);
	bool __fastcall IsScxmlTransitionDisplayTypesStored(void);

	int __fastcall GetTransitionIndex(void);
	void __fastcall SetTransitionIndex(int val);

	int				FClipOutMax;		void __fastcall SetClipOutMax(int val) { FClipOutMax=val; UpdateTransitionText(); }

	bool 			FClipOutCondition; 	void __fastcall SetClipOutCondition(bool val) { FClipOutCondition=val; UpdateTransitionText(); }
	bool 			FClipOutXML;       	void __fastcall SetClipOutXML(bool val) { FClipOutXML=val; UpdateTransitionText(); }
	TClipTextType 	FClipTypeCondition; void __fastcall SetClipTypeCondition(TClipTextType val) { FClipTypeCondition=val; UpdateTransitionText(); }
	TClipTextType 	FClipTypeXML;       void __fastcall SetClipTypeXML(TClipTextType val) { FClipTypeXML=val; UpdateTransitionText(); }

	bool __fastcall GetIsSelfConnection(void) { return (this->FromShape != NULL) && (this->FromShape == this->ToShape); }
	TSelfConnectionInfo __fastcall GetSelfConnectionInfo();

	TStateMachineEditor * __fastcall GetStateMachineEditor(void);
	TStateMachineEditorUnit *__fastcall GetStateMachineEditorUnit(void);

	TTransitionSwitchType FSwitch;
	void __fastcall SetSwitch(TTransitionSwitchType val);

	TDrawTextData FDrawTextData;

	bool FVerticalCentering;
	void __fastcall SetVerticalCentering(bool val);

	inline bool __fastcall GetAreInheritedPointsIgnored(void) { return FInheritanceResolver->ResolvedProps->IndexOf("Points")!=-1; }

	bool __fastcall GetExcludeFromSave(void);

	bool FSkipAutolayout;
	void __fastcall SetSkipAutolayout(bool val);

	bool FBreakpointSet;


	// текст в соединении может иметь строки с разными цветами
	// поэтому в зависимости от типа соединения, формируем набор цветов
	void __fastcall FillTextColorQueue(std::deque<TColor> &AColorQueue);

	// эта функция может как отрисовать текст, так и просто просчитать размеры
	void __fastcall DoDrawTextInternal(const bool bDraw, int tmpCount, int tmpX, int tmpOffX, int tmpY, int tmpOffY, int Angle, int tmpH);

	// расчет положения текста, в зависимости от центровки и количества точек
	TPoint __fastcall GetTextAbsolutePosWithoutCentering(void);

    inline UnicodeString __fastcall GetCaption(void) { return this->SimpleText.IsEmpty() ? this->Name : this->SimpleText; }

	inline bool __fastcall GetIsContentEmpty(void) { return this->Event.IsEmpty() && this->Condition.IsEmpty() && this->XML.IsEmpty(); }

	TVisualScxmlBaseShape *	__fastcall GetVisualFrom(void);
	TVisualScxmlBaseShape *	__fastcall GetVisualTo(void);

    virtual void __fastcall SetBreakpointSet(bool val);

protected:

	virtual void __fastcall DoAppendExtraParamsAndChildren(Lmdxml::ILMDXmlElement *AElement, Lmdxml::ILMDXmlElement *AParentNode);

	virtual void __fastcall DoInternalDrawEx(void);

	virtual void __fastcall DoPreparePoints(void);

	virtual void __fastcall DoPrepareIBounds(void);

	virtual void __fastcall DoDrawText(int tmpCount, int tmpX, int tmpOffX, int tmpY, int tmpOffY, int Angle, int tmpH);

public:
	__fastcall TStateMachineConnection(Classes::TComponent* AOwner);
	__fastcall virtual ~TStateMachineConnection(void);

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	// здесь данные без визуального представления (точек, типа связи и т.д.)
	virtual void __fastcall AssignStateMachineConnection(TStateMachineConnection *AStateMachineConnection);

	// полная перерисовка
	virtual bool __fastcall Draw(void);

	// отрисовка текста
	virtual void __fastcall DrawText(int Angle);

	// эта функция вызывается, если текст ещё ни разу не рисовался,
	// но нужны координаты границ
	// не использовать для текста, который будет поворачиваться !!!
	void __fastcall CalculateTextPosWithoutDraw(void);

	// показать редактор соединения
	void __fastcall ShowEditor(void);

	// если редактор открыт, сохранить данные
	void __fastcall SaveEditor(void);

	// есть ли изменения в редакторе
	bool __fastcall IsEditorModified(void);

	// открыт ли вообще редактор
	bool __fastcall IsEditorOpened(void);

	// добавляет или просто проверяет содержимое
	void __fastcall AppendTransitionElement(Lmdxml::ILMDXmlElement *AParentNode);

	void __fastcall AppendTransitionSwitchElement(Lmdxml::ILMDXmlElement *AParentNode, const UnicodeString &sTarget);

	void __fastcall MakeSelfConnection(void);

	void __fastcall UpdateAppearance(void);

	void __fastcall UpdateDummyText(void);

	void __fastcall AlignSelfConnection(const TSelfConnectionAlign AConnectionAlign,
		const int iLengthOffsetBegin = 0,
		const int iLengthOffsetEnd = 0,
		const int iTextOffset = 0);

	void __fastcall SetConnectionOverStyle(const TConnectionOverType AType);

	void __fastcall UpdateTransitionText(void);

	bool __fastcall IsTextEmpty(void);
	bool __fastcall IsPointInTextPolygon(const TPoint &pt);
	bool __fastcall IsPointInTextPolygon(const int iX, const int iY);
	void __fastcall AssignTextPolygonToPoints(TPointCollection * APoints);

	void __fastcall FillEventNamesList(TStrings *AList);

	// границы соединения посчитаются только при Draw, поэтому самостоятельно пересчитываем
	void __fastcall ForceCalculateBounds(void);

	// координаты точек текста на соединении
	// так как текст может быть провернут на какой-то угол,
	// поэтому это будут координаты полигона
	inline const TDrawTextData &GetDrawTextData(void) const { return FDrawTextData; }

	UnicodeString __fastcall ToClipboardText(void);

	__property bool							IsTargetDescendant = {read=GetIsTargetDescendant};
	__property bool 						IsSelfConnection = {read=GetIsSelfConnection};
	__property TSelfConnectionInfo 			SelfConnectionInfo = {read=GetSelfConnectionInfo};
	__property GUID	*						Guid = {read=FGuid};
	__property UnicodeString				GuidStr = {read=FGuidStr};
	__property TStateMachineEditor * 		StateMachineEditor = {read=GetStateMachineEditor};
	__property TStateMachineEditorUnit *	StateMachineEditorUnit = {read=GetStateMachineEditorUnit};
	__property bool 						AreInheritedPointsIgnored = {read=GetAreInheritedPointsIgnored};
	__property bool							ExcludeFromSave = {read=GetExcludeFromSave};
	__property bool							IsContentEmpty = {read=GetIsContentEmpty};
	__property int							TextCount = {read=TextLinesCount};
	__property UnicodeString				Caption = {read=GetCaption}; // или SimpleText, или Name
	__property TVisualScxmlBaseShape *		VisualFrom = {read=GetVisualFrom};
	__property TVisualScxmlBaseShape *		VisualTo = {read=GetVisualTo};

__published:
	__property UnicodeString 				Event = {read=FEvent, write=SetEvent};
	__property UnicodeString 				Condition = {read=FCondition, write=SetCondition};
	__property UnicodeString 				ConditionBack = {read=GetConditionBack, write=SetConditionBack, stored=IsConditionBackStored};
	__property UnicodeString 				XML = {read=FXML, write=SetXML};
	__property UnicodeString 				Target = {read=GetTarget};
	__property bool 						IsTargetStored = {read=GetIsTargetStored,write=SetIsTargetStored,stored=IsTargetPropStored};
	__property TScxmlTransitionType 		Type = {read=FType, write=SetType, default=external};
	__property UnicodeString 				ExtraParams = {read=FExtraParams, write=FExtraParams};	
	__property bool 						Locked = {read=FLocked, write=FLocked, default=false};
	__property bool 						InheritanceMismatch = {read=FInheritanceMismatch, write=SetInheritanceMismatch, default=false};
	__property TInheritanceResolver * 		InheritanceResolver = {read=FInheritanceResolver, write=SetInheritanceResolver};
	__property TTransitionSwitchType		SWITCH = {read=FSwitch, write=SetSwitch, default=tstNONE};
	__property int							TransitionIndex = {read=GetTransitionIndex,write=SetTransitionIndex,stored=false};
	__property bool 						AdjustableSides = {read=FAdjustableSides, write=FAdjustableSides, default=true};
	__property TContentTrigger *			ContentTrigger = {read=FContentTrigger, write=SetContentTrigger};
	__property TParamTriggerItems *			Params = {read=FParams, write=SetParams};
	__property TProtocolControlBinding *	ProtocolControlBinding = {read=FProtocolControlBinding, write=SetProtocolControlBinding};
	__property TScxmlTransitionDisplayTypes DisplayTypes = {read=FScxmlTransitionDisplayTypes, write=SetScxmlTransitionDisplayTypes,stored=IsScxmlTransitionDisplayTypesStored};
	__property int 							ClipOutMax = {read=FClipOutMax,write=SetClipOutMax,default=MAX_SIMPLETEXT_LENGTH};
	__property bool 						ClipOutCondition = {read=FClipOutCondition,write=SetClipOutCondition,default=false};
	__property bool 						ClipOutXML = {read=FClipOutXML,write=SetClipOutXML,default=true};
	__property TClipTextType				ClipTypeCondition = {read=FClipTypeCondition,write=SetClipTypeCondition,default=cttMiddle};
	__property TClipTextType				ClipTypeXML = {read=FClipTypeXML,write=SetClipTypeXML,default=cttLeft};
	__property bool							VerticalCentering = {read=FVerticalCentering, write=SetVerticalCentering, default=false};
	__property bool 						SkipAutolayout = {read=FSkipAutolayout, write=SetSkipAutolayout, default=false};
	__property bool 						BreakpointSet = {read=FBreakpointSet, write=SetBreakpointSet, default=false};
	__property UnicodeString				Description = {read=FDescription, write=SetDescription};

};

class EStateMachineConnectionException: public Exception {
private:
	TStateMachineConnection *FStateMachineConnection;
	UnicodeString FCategory;
public:
	__fastcall EStateMachineConnectionException(TStateMachineConnection *AStateMachineConnection,
												const UnicodeString &sCategory, const UnicodeString &sMsg);

	__property TStateMachineConnection *StateMachineConnection = {read=FStateMachineConnection};
	__property UnicodeString Category = {read=FCategory};
};

extern UnicodeString ClipText(const UnicodeString &sText, TClipTextType AType, int iClipMax);
extern bool IsStateMachineConnectionClipboardFormat(const UnicodeString &sText);
extern void PasteStateMachineConnectionFromText(TCustomTree *ADestTree, const UnicodeString &sText); // не проверяет на валидность

#endif
