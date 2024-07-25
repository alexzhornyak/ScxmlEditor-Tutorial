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

#ifndef UnitScxmlBaseShapeH
#define UnitScxmlBaseShapeH
//---------------------------------------------------------------------------

// без этого будет ругаться на DrawTextA
#undef DrawText // exclude Winapi DrawText

#include <map>
#include <set>

#include <TeeTree.hpp>
#include <LMDXml.hpp>

#include <boost/function.hpp>
#include <LmdInsPropInsp.hpp>
#include "LMDInsPropEditors.hpp"

#include "Log4cpp_VCL.hpp"
#include "UnitSyntaxString.h"
#include "UnitUniqueUtils.h"
#include "PersistentRect.h"
#include "PersistentPoint.h"
#include "UnitInheritanceResolver.h"
#include "UnitTreeEditorStateMachine.h"

#define SCXML_ERROR_NAME_DUPLICATED		100
#define SCXML_ERROR_WRONG_CHILD_TYPE	101
#define SCXML_ERROR_SRC_NOT_FOUND		102

#define MAX_SIMPLETEXT_LENGTH			40

typedef enum {sctScxml, sctState, sctParallel, sctTransition, sctInitial, sctFinal,
				sctOnentry, sctOnexit, sctHistory,
				sctDatamodel, sctData, sctAssign, sctDonedata, sctContent, sctParam, sctScript,
				sctRaise, sctIf, sctElseif, sctElse, sctForeach, sctLog,
				sctSend, sctCancel, sctInvoke, sctFinalize, sctComment, sctExtraContent,
				sctProtocolBinding,
				sctWeakProtocolBinding,
				sctSetValue,
				sctSessionState,
				sctVirtualFolder,
				sctVirtual,
				sctTrigger, sctWatch,
				sctTransitionXML,
				sctMAXSIZE} TStateChildType;
typedef std::set<TStateChildType> TStateChildTypes;

static const TStateChildType __STATE_TYPES_EXECUTABLE_CONTENT[] = { sctRaise,sctIf,sctElseif,sctElse,
																	sctForeach,sctLog,sctAssign,sctScript,sctSend,sctCancel,
																	sctSetValue,sctSessionState };
static const TStateChildTypes STATE_TYPES_EXECUTABLE_CONTENT(__STATE_TYPES_EXECUTABLE_CONTENT, __STATE_TYPES_EXECUTABLE_CONTENT + ARRAYSIZE(__STATE_TYPES_EXECUTABLE_CONTENT));

static const TStateChildType __STATE_TYPES_VISUAL_SHAPES[] = { sctScxml,sctState,sctParallel,sctInitial,sctFinal,sctHistory,sctVirtual };
static const TStateChildTypes STATE_TYPES_VISUAL_SHAPES(__STATE_TYPES_VISUAL_SHAPES, __STATE_TYPES_VISUAL_SHAPES + ARRAYSIZE(__STATE_TYPES_VISUAL_SHAPES));

static const TStateChildType  __STATE_TYPES_CLUSTERABLE_SHAPES[] = { sctScxml,sctState,sctParallel };
static const TStateChildTypes STATE_TYPES_CLUSTERABLE_SHAPES(__STATE_TYPES_CLUSTERABLE_SHAPES, __STATE_TYPES_CLUSTERABLE_SHAPES + ARRAYSIZE(__STATE_TYPES_CLUSTERABLE_SHAPES));

static const TStateChildType  __STATE_TYPES_SELFCONNECTION_SHAPES[] = { sctState,sctParallel, sctFinal, sctVirtual };
static const TStateChildTypes STATE_TYPES_SELFCONNECTION_SHAPES(__STATE_TYPES_SELFCONNECTION_SHAPES, __STATE_TYPES_SELFCONNECTION_SHAPES + ARRAYSIZE(__STATE_TYPES_SELFCONNECTION_SHAPES));

static const TStateChildType  __STATE_TYPES_AVIA_EXTENSIONS[] = { sctProtocolBinding,sctWeakProtocolBinding,sctSetValue,sctSessionState };
static const TStateChildTypes STATE_TYPES_AVIA_EXTENSIONS(__STATE_TYPES_AVIA_EXTENSIONS, __STATE_TYPES_AVIA_EXTENSIONS + ARRAYSIZE(__STATE_TYPES_AVIA_EXTENSIONS));

static const TStateChildType  __STATE_TYPES_VIRTUAL_SCXML_SHAPES[] = { 	sctState, sctParallel, sctVirtualFolder,
																		sctProtocolBinding, sctWeakProtocolBinding,
																		sctComment, sctTrigger, sctWatch 	};
static const TStateChildTypes STATE_TYPES_VIRTUAL_SCXML_SHAPES(__STATE_TYPES_VIRTUAL_SCXML_SHAPES, __STATE_TYPES_VIRTUAL_SCXML_SHAPES + ARRAYSIZE(__STATE_TYPES_VIRTUAL_SCXML_SHAPES));

static const TColor COLOR_DISABLE_FLAT_BUILD = TColor(RGB(225,240,255));

#define SET_SHAPE_VALUE_SIMPLE_TEXT(X)      			void __fastcall Set##X(UnicodeString val) { F##X=val; SimpleText=XMLName + " {" + F##X + "}"; }
#define SET_SHAPE_VALUE_UPDATESIMPLETEXT(X) 			void __fastcall Set##X(UnicodeString val) { F##X=val; UpdateSimpleText(); }
#define SET_SHAPE_VALUE_UPDATESIMPLETEXT_TYPE(type,X) 	void __fastcall Set##X(type val) { F##X=val; UpdateSimpleText(); }

#define SET_SHAPE_VALUE_PAIR(VAL1,VAL2, simpletext) void __fastcall Set##VAL1(UnicodeString val) {      				\
	if (F##VAL2.IsEmpty()||val.IsEmpty()) {                                                                             \
		F##VAL1 = val;                                                                                                	\
		if (simpletext) this->SimpleText = this->XMLName + " {" + (F##VAL1.IsEmpty() ? F##VAL2 : F##VAL1) + "}";        \
	} else WLOG_ERROR(L"'%s'=[%s] must not occur with '%s'=[%s] elem:[%s]",                                             \
		UnicodeString(#VAL1).c_str(),val.c_str(),UnicodeString(#VAL2).c_str(),F##VAL2.c_str(), this->Name.c_str()); }

#define SET_SHAPE_VALUE_PAIR_UPDATESIMPLETEXT(VAL1,VAL2) void __fastcall Set##VAL1(UnicodeString val) {      			\
	if (F##VAL2.IsEmpty()||val.IsEmpty()) {                                                                             \
		F##VAL1 = val;                                                                                                	\
		UpdateSimpleText();        																						\
	} else WLOG_ERROR(L"'%s'=[%s] must not occur with '%s'=[%s] elem:[%s]",                                             \
		UnicodeString(#VAL1).c_str(),val.c_str(),UnicodeString(#VAL2).c_str(),F##VAL2.c_str(), this->Name.c_str()); }

template <typename T>
T * __fastcall AddScxmlClassShape(TTreeNodeShape *AShape, const UnicodeString &sName, bool bOnce) {
	T *ARetShape = NULL;
	if (AShape->Tree) {
		bool bIsFound = false;
		if (bOnce) {
			for (int i = 0; i < AShape->Count(); i++) {
				T *AExistAlreadyShape = dynamic_cast<T* >(AShape->Children->Items[i]);
				if (AExistAlreadyShape) {
					// может быть только один экземпляр
					WLOG_WARNING(L"Shape with class <%s> name <%s> id <%s> already exists", AExistAlreadyShape->ClassName().c_str(),
					AExistAlreadyShape->Name.c_str(), AExistAlreadyShape->SimpleText.c_str());
					bIsFound = true;
					break;
				}
			}
		}
		if (!bIsFound) {

			const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(AShape->Tree->Owner, __classid(T));

			if (__classid(T)->InheritsFrom(__classid(TVisualScxmlBaseShape))) {
				AShape->Expanded = true;
				ARetShape = new T(AShape->Tree->Owner);
				ARetShape->Name = AUniqueName.second;
				ARetShape->SimpleText = AUniqueName.first;
				ARetShape->Tree = AShape->Tree;
				ARetShape->Parent = AShape;
				ARetShape->Expanded = true;
			} else {
				TMetaClass *APrevClass = AShape->Tree->GlobalFormat.NodeClass;
				const TTreeNodeImageIndex iPrevIndex = AShape->Tree->GlobalFormat.ImageIndex;
				AShape->Tree->GlobalFormat.ImageIndex = TTreeNodeImageIndex::tiNone;
				AShape->Tree->GlobalFormat.NodeClass = __classid(T);

                // просчитываем индекс, чтобы фигура была до визуальных компонентов в иерархии
				int iIndex = 0;
				for (int i = 0; i < AShape->Children->Count; i++) {
					if (AShape->Children->Items[i]->InheritsFrom(__classid(TVisualScxmlBaseShape))) {
						iIndex = i;
						break;
					}
					iIndex++;
				}

				// просчитываем координаты, где должен создаться дочерний элемент
				// вообще координаты просчитывает TScxmlAlignChildEx,
				// но если здесь их не указать такими же, то будет эффект "прыжка фигуры"
				int iLeft = 0;
				int iTop = 0;
				if (TVisualScxmlBaseShape *AVisualParent = dynamic_cast<TVisualScxmlBaseShape *>(AShape)) {
					iLeft = AVisualParent->ChildrenAlignX == scaxRight ? AShape->X1 : (AShape->X0 + TreeHorizMarginDefault);
					iTop = AVisualParent->MaxHeightExpandedChildsEx();
				} else {
					iLeft = AShape->X0 + TreeHorizMarginDefault;
					iTop = AShape->MaxHeightExpandedChilds();
				}

				ARetShape = dynamic_cast<T*>(AShape->Tree->AddShapeClass(iLeft,iTop, sName, AShape, __classid(T)));
				if (ARetShape) {
					ARetShape->Name = AUniqueName.second;

					Unique::UpdateUniqueChildConnectionName(ARetShape);
				}

				AShape->Tree->GlobalFormat.NodeClass = APrevClass;
				AShape->Tree->GlobalFormat.ImageIndex = iPrevIndex;

				// индексы TChildScxmlBaseShape должны сформироваться по порядку в начале и отдельно от TVisualScxmlBaseShape
				if (ARetShape && ARetShape->ChildIndex != iIndex) {
					ARetShape->ChildIndex = iIndex;
				}
			}
		}
	}
	return ARetShape;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef std::map<UnicodeString,bool/*required or default*/> TStateAttributesMap;
typedef std::map<std::size_t, UnicodeString> TStateErrorMap;

#define SCXML_EDITBYDEFAULT_AUTO			L"(AUTO)"
#define SCXML_EDITBYDEFAULT_XML				L"XML"

typedef std::pair < UnicodeString, boost::function < void() > > TDefaultEditorPair;

class TVisualScxmlBaseShape;

class TStateMachineEditor;
class TStateMachineEditorUnit;

class TScxmlBaseShape: public TTreeNodeShape {
__published:
	// это событие вызывается для проверки фильтрации свойств
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual void __fastcall OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString __fastcall GetXMLName(void);

	UnicodeString FExtraParams;

	TSyntaxString* FExtraContent;

	TSyntaxString* FComments;

	TSyntaxString* FXMLText;

	bool FExcludeFromSave;

	GUID * FGuid;
	UnicodeString FGuidStr;

	UnicodeString FExcludeConditions;

	bool FLocked;
	bool FInheritanceMismatch;
	void __fastcall SetInheritanceMismatch(bool val);
	TInheritanceResolver * FInheritanceResolver;
	inline void __fastcall SetInheritanceResolver(TInheritanceResolver *val) { FInheritanceResolver->Assign(val); }

	bool __fastcall GetHasSelfConnections(void);

	bool __fastcall GetHasNonVisualChildren(void);

	UnicodeString __fastcall GetScxmlSyntaxScheme(void);

protected:

	TStateAttributesMap FStateAttrMap; // ключ 'имя аттрибута', значение 'required or default'
	TStateErrorMap FErrorMap;

	virtual TStateChildType __fastcall GetStateChildType(void) = 0; // абстрактный
	TStateChildTypes FAvailableTypes;
	TStateChildTypes FConvertibleTypes;

    virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R); /* inherited */

	TScxmlBaseShape *__fastcall GetBaseParent(void);
	TVisualScxmlBaseShape *__fastcall GetVisualParent(void);
	TVisualScxmlBaseShape *__fastcall GetFirstVisualParent(void);
	TVisualScxmlBaseShape *__fastcall GetFirstVisualChild(void);

	void __fastcall SetBrotherIndexEx(int val);
	inline int __fastcall GetBrotherIndexEx() { return BrotherIndex; }

	virtual bool __fastcall ShouldDrawCross(void); /* override */

	inline virtual void __fastcall SetExtraContent(TSyntaxString* val) { FExtraContent->Assign(val); }
	inline virtual void __fastcall SetComments(TSyntaxString *val) { FComments->Assign(val); }
	inline virtual void __fastcall SetXMLText(TSyntaxString *val) { FXMLText->Assign(val); }

	inline virtual void __fastcall OnXMLTextChanged(TObject *Sender) {};

	inline virtual void __fastcall SetExcludeFromSave(bool val) { FExcludeFromSave = val; }
	bool __fastcall GetExcludeFromSave(void);
	inline bool __fastcall IsExcludeFromSaveStored(void) { return FExcludeFromSave!=false; }

	TStateMachineEditorUnit *__fastcall GetStateMachineEditorUnit(void);
	TStateMachineEditor * __fastcall GetStateMachineEditor(void);

	inline virtual void __fastcall SetExcludeConditions(UnicodeString val) { FExcludeConditions = val; }

	virtual bool __fastcall IsExcludedByCondition();

	// по умолчанию возвращает SimpleText
	inline virtual UnicodeString __fastcall GetClipboardInfo(void) { return SimpleText; }

	inline virtual bool __fastcall GetOccursOnce(void) { return false; }

    virtual void __fastcall DeleteAllToConnections(void);

public:
	__fastcall virtual TScxmlBaseShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TScxmlBaseShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);
    DYNAMIC TTreeShapeHandle __fastcall GetResizingHandle(int x, int y);

	// определяем глобаные события,
	// в дочерних при унаследовании этого метода НИЧЕГО НЕ РИСОВАТЬ !
	// пользоваться DrawShapeCanvas и т.д.
	virtual void __fastcall Draw(void);

	// является ли канва для TCanvasSVG
	bool __fastcall IsCanvasSVG(void);

	bool __fastcall ShouldDrawVectorImages(void);

	virtual UnicodeString __fastcall GetErrorDescription(const int iErrCode);
	virtual TScxmlBaseShape * __fastcall AddStateChild(TStateChildType AChildType, bool bCheckBounds);

	static TStateChildType __fastcall XMLNodeNameToType(const UnicodeString &sNodeName);
	static UnicodeString __fastcall TypeToName(const TStateChildType AType);
	static UnicodeString __fastcall TypeToXMLNodeName(const TStateChildType AType);

	virtual UnicodeString __fastcall XMLAttrNameToPropName(UnicodeString sAttrName);
	virtual UnicodeString __fastcall PropNameToXMLAttrName(UnicodeString sPropName);
	virtual void __fastcall AttributesToProps(ILMDXmlElement *ANode);
	virtual void __fastcall PropsToAttributes(ILMDXmlElement *ANode);

	bool __fastcall EditRawXMLContent(void);

	void EditUnicodeStringSyntaxProperty(const UnicodeString &sPropName);
	void EditUnicodeStringLineProperty(const UnicodeString &sPropName);
	void EditEventIDLineProperty(const UnicodeString &sPropName);

	virtual void __fastcall ValidateShape();
	static void __fastcall ValidateSimpleText(TCustomTree *ATree);
	inline virtual bool __fastcall MustSimpleTextBeUnique(void) { return false;}

	// тестирует фигуру, использует ли она переменную
	bool __fastcall AreConditionalDefinesUsed(TStrings *ADefines);

	// проверяет по цепочке родителей
	bool __fastcall HasAsParent(TTreeNodeShape *AShape);

	/* для наследования */
	void __fastcall StoreInheritedVisualsParentOffsets(void); // сохранит положение дочерних относительно родителя
	void __fastcall RestoreInheritedVisualsParentOffsets(void); // восстановит

	/* проверяем, совместимы ли дети и аттрибуты */
	/* например: Data Src,Expr,Children */
	/* генерируем исключение */
	virtual void __fastcall ValidateChild(TScxmlBaseShape *AChild);
	inline virtual void __fastcall ValidateXMLText(void) { }

	__property int 							ChildIndex = {read=GetBrotherIndexEx,write=SetBrotherIndexEx,stored=false};
	__property UnicodeString 				XMLName = {read=GetXMLName};
	__property TSyntaxString* 				XMLText = {read=FXMLText,write=SetXMLText};
	__property TSyntaxString* 				ExtraContent = {read=FExtraContent, write=SetExtraContent};
	__property TSyntaxString* 				Comments = {read=FComments, write=SetComments};

	__property TScxmlBaseShape *			BaseParent = {read=GetBaseParent}; // прямой родитель, приведенный к 'TScxmlBaseShape'
	__property TVisualScxmlBaseShape *		VisualParent = {read=GetVisualParent}; // прямой родитель, приведенный к 'TVisualScxmlBaseShape'
	__property TVisualScxmlBaseShape *		FirstVisualParent = {read=GetFirstVisualParent}; // первый рекурсивный родитель, приведенный к 'TVisualScxmlBaseShape'
	__property TVisualScxmlBaseShape *		FirstVisualChild = {read=GetFirstVisualChild}; // первый визуальный дочерний элемент, который доступен
	__property TStateChildTypes 			AvailableTypes = {read=FAvailableTypes};
	__property TStateChildTypes 			ConvertibleTypes = {read=FConvertibleTypes};

	__property TStateMachineEditor * 		StateMachineEditor = {read=GetStateMachineEditor};
	__property TStateMachineEditorUnit *	StateMachineEditorUnit = {read=GetStateMachineEditorUnit};

	__property GUID	*						Guid = {read=FGuid};
	__property UnicodeString				GuidStr = {read=FGuidStr};

	__property bool							HasSelfConnections = {read=GetHasSelfConnections};
	__property bool							HasNonVisualChildren = {read=GetHasNonVisualChildren};
	__property bool							OccursOnce = {read=GetOccursOnce};
	// краткая важная информация, которую можно скопировать в буфер обмена
	// по умолчанию SimpleText
	__property UnicodeString				ClipboardInfo = {read=GetClipboardInfo};

	__property UnicodeString 				ScxmlSyntaxScheme = {read=GetScxmlSyntaxScheme};

__published:
	__property TStateChildType 				StateChildType = {read=GetStateChildType};
	__property UnicodeString 				ExtraParams = {read=FExtraParams, write=FExtraParams};
	__property bool							ExcludeFromSave = {read=GetExcludeFromSave, write=SetExcludeFromSave, stored=IsExcludeFromSaveStored, default=false};
	__property UnicodeString				ExcludeConditions = {read=FExcludeConditions, write=SetExcludeConditions};
	__property bool 						Locked = {read=FLocked, write=FLocked, default=false};
	__property bool 						InheritanceMismatch = {read=FInheritanceMismatch, write=SetInheritanceMismatch, default=false};
	__property TInheritanceResolver * 		InheritanceResolver = {read=FInheritanceResolver, write=SetInheritanceResolver};

};

typedef enum { scaxLeft, scaxRight, scaxCenter } TShapeChildrenAlignX;
typedef enum { scayClusterTop, scayAlwaysBottom, scayAlwaysTop } TShapeChildrenAlignY;

//---------------------------------------------------------------------------
class TVisualScxmlBaseShape : public TScxmlBaseShape {
__published:
	// это событие вызывается для проверки фильтрации свойств
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
    virtual void __fastcall OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor);

    virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);

private:

	bool FEntered;
	UnicodeString FAliasID;
	bool FExamined;
	bool FBreakpointSet;
	bool FSkipDebugging;
	bool FAutoVertTextAlign;
	TShapeChildrenAlignX FChildrenAlignX;
	TShapeChildrenAlignY FChildrenAlignY;
    int FChildrenAlignXOffset;

	TPersistentRect *FSelfConnectionTextOffsets;
	void __fastcall SetSelfConnectionTextOffsets(TPersistentRect *val);

	bool FSelfConnectionInside;
	void __fastcall SetSelfConnectionInside(bool val);

	TPersistentPoint *FParentOffsetStored;
	void __fastcall SetParentOffsetStored(TPersistentPoint *val);

	int __fastcall GetParentOffsetX(void);
	int __fastcall GetParentOffsetY(void);

protected:

	virtual TColor __fastcall GetNormalColor(void);
	virtual TColor __fastcall GetHeadColor(void);
	virtual TColor __fastcall GetClusterColor(void);

	virtual TTreeShapeStyle __fastcall GetNormalStyle(void);
	virtual TTreeShapeStyle __fastcall GetClusterStyle(void);

	virtual void __fastcall SetNormalAppearance(void);
	virtual void __fastcall SetErrorAppearance(void);

	virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R); /* inherited */
	virtual TVertTextAlign __fastcall GetVertTextAlign(void); /* inhrerited */
	virtual void __fastcall SetVertTextAlign(TVertTextAlign val);  /* inhrerited */

	UnicodeString FInitial;
	virtual UnicodeString __fastcall GetInitial(void);

	bool __fastcall GetIsInitial(void);
	void __fastcall SetIsInitial(bool val);

	bool __fastcall GetIsInitialDeep(void);
	void __fastcall SetIsInitialDeep(bool val);

	virtual void __fastcall SetEntered(bool val);
	virtual void __fastcall SetExamined(bool val);
	virtual void __fastcall SetBreakpointSet(bool val);
	virtual void __fastcall SetSkipDebugging(bool val);

	virtual void __fastcall SetExcludeFromSave(bool val); /* override */

	// обновит отображение информации о точке останова: либо активная иконка или неактивная, или без иконки
	void __fastcall UpdateBreakpointView(); /* не переносить в public и пользоваться UpdateAppearance ! */

public:

	__fastcall virtual TVisualScxmlBaseShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TVisualScxmlBaseShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	// устанавливает размеры по умолчанию
	virtual void __fastcall SetDefaultSize();

	int __fastcall CalculateChildrenTop(int iDefaultTop);
	void __fastcall UpdateStateChildren(void);

	// игнорируем визуальные фигуры
	int __fastcall MaxHeightExpandedChildsEx(void);

	int __fastcall GetClusterHeight(void);

	// по умолчанию, если есть хотя бы один дочерний такого типа
	virtual bool __fastcall IsCluster(void);

	static void __fastcall SetChildrenEntered(TNodeShapeList *AShapes, bool bVal, bool bRecursive=true);

   	inline virtual void __fastcall SetAliasID(UnicodeString val) { FAliasID = val; }

	// упорядочивание с учетом всех смещений
	void __fastcall ArrangeSelfConnections(void);

	void __fastcall MoveToOtherShapeCenter(TVisualScxmlBaseShape *ASourceShape);

	// сброс по умолчанию по ширине фигуры
	void __fastcall ArrangeDefaultSelfConnections(void);

	// проверяет, что компонент доступен и обновляет вид
	virtual void __fastcall UpdateView(void);

	// обновляет данные по компоненту
	void __fastcall UpdateAppearance(void);

	// установит SimpleText == sVal и пометит StateMachineEditor для проверки валидности имени
	virtual void __fastcall SetStateId(UnicodeString sVal);

	inline virtual bool __fastcall MustSimpleTextBeUnique(void) { return true;} /* override */

	bool FindChildInitial(TVisualScxmlBaseShape *AShape, const UnicodeString &sInitial);

	// возвращает первое соединение, которое выходит наружу изнутри, или приходит внутрь
	// гарантирует FromShape, ToShape != NULL
	TStateMachineConnection * __fastcall HasInternalInOutConnections(void);

   	/* является ли фигурой, на которой сработала точка останова при Дебаге */
	bool __fastcall IsActiveDebugShape(void);

	__property bool Entered = {read=FEntered, write=SetEntered, default=false};
	__property bool Examined = {read=FExamined, write=SetExamined, default=false};
	__property bool BreakpointSet = {read=FBreakpointSet, write=SetBreakpointSet, default=false};
	__property bool IsInitial = {read=GetIsInitial,write=SetIsInitial};
	__property bool IsInitialDeep = {read=GetIsInitialDeep,write=SetIsInitialDeep};
	__property UnicodeString Initial = {read=GetInitial, write=FInitial};
	__property int ParentOffsetX = { read=GetParentOffsetX };
	__property int ParentOffsetY = { read=GetParentOffsetY };

__published:

	__property UnicodeString AliasID = {read=FAliasID, stored=false};
	__property TPersistentRect *SelfConnectionTextOffsets = {read=FSelfConnectionTextOffsets, write=SetSelfConnectionTextOffsets};
	__property bool SelfConnectionInside = {read=FSelfConnectionInside, write=SetSelfConnectionInside, default=false};
	__property TPersistentPoint *ParentOffsetStored = {read=FParentOffsetStored, write=SetParentOffsetStored};
	__property ChildIndex;
	__property TShapeChildrenAlignX ChildrenAlignX = {read=FChildrenAlignX, write=FChildrenAlignX, default=scaxLeft};
	__property TShapeChildrenAlignY ChildrenAlignY = {read=FChildrenAlignY, write=FChildrenAlignY, default=scayClusterTop};
	__property int ChildrenAlignXOffset = {read=FChildrenAlignXOffset, write=FChildrenAlignXOffset, default=0};
	__property bool SkipDebugging = {read=FSkipDebugging, write=SetSkipDebugging, default=false};
	__property bool AutoVertTextAlign = {read=FAutoVertTextAlign, write=FAutoVertTextAlign, default=true};
	__property VertTextAlign;
	__property HorizTextAlign;

};

//---------------------------------------------------------------------------
class TChildScxmlBaseShape : public TScxmlBaseShape {
__published:
	// это событие вызывается для проверки фильтрации свойств
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);

    virtual void __fastcall OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor);

	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);

	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);

private:
	int 	FClipOutMax;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT_TYPE(int,ClipOutMax);
	bool 	FClipOutValue;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT_TYPE(bool,ClipOutValue);

	UnicodeString FDescription;
	UnicodeString FEditByDefault;

	inline UnicodeString __fastcall GetEditByDefault(void) { return FEditByDefault.IsEmpty() ? GetEditByDefaultValue() : FEditByDefault; }
	inline bool __fastcall IsEditByDefaultStored(void) { return !SameText(GetEditByDefault(), GetEditByDefaultValue()); }

protected:

	inline virtual TColor __fastcall GetFinalColor(void) { return GetNormalColor(); }

	virtual TColor __fastcall GetNormalColor(void);

	virtual TColor __fastcall GetFinalFontColor(void);

	virtual TColor __fastcall GetNormalFontColor(void);

	inline virtual TFontStyles __fastcall GetFinalFontStyle(void) { return GetNormalFontStyle(); }

	inline virtual TFontStyles __fastcall GetNormalFontStyle();

	inline virtual TColor __fastcall GetFinalBorderColor(void) { return GetNormalBorderColor(); }

	// делаем цвет чуть светлее, чтобы текст читался крупнее
	virtual TColor __fastcall GetNormalBorderColor(void);

	virtual TPenStyle __fastcall GetFinalBorderStyle(void);

	virtual TPenStyle __fastcall GetNormalBorderStyle(void);

	// здесь дописываем и рисуем по уже сформированной канве
	virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R); /* inherited */

	inline virtual void __fastcall OnXMLTextChanged(TObject *Sender) { UpdateSimpleText(); }

	// здесь определяем настройки канвы до рисования
	virtual void __fastcall Draw(void);

	virtual void __fastcall DrawImage(void);

	inline virtual bool __fastcall HasVectorImage(void) { return false; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds) { };

	inline virtual UnicodeString __fastcall GetOutValue() { return L""; }

	virtual void __fastcall UpdateParentSimpleText();

	virtual void __fastcall Loaded(void);

	virtual void __fastcall SetExcludeFromSave(bool val); /* override */

	virtual	void __fastcall SetExcludeConditions(UnicodeString val);

	virtual void __fastcall SetDescription(UnicodeString val);

	virtual UnicodeString __fastcall GetEditByDefaultValue(void);
	virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void) { return std::vector<TDefaultEditorPair>(); }
	bool __fastcall HasDefaultEditableProps(void);

public:

	__fastcall virtual TChildScxmlBaseShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TChildScxmlBaseShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	virtual void __fastcall UpdateSimpleText();

	virtual void __fastcall DefaultEditProperty(void);
	virtual void __fastcall SetDefaultEditableProps(TStrings *AList);

	__property UnicodeString OutValue = {read=GetOutValue};

__published:
	__property ChildIndex;
	__property int ClipOutMax = {read=FClipOutMax,write=SetClipOutMax,default=MAX_SIMPLETEXT_LENGTH};
	__property bool ClipOutValue = {read=FClipOutValue,write=SetClipOutValue,default=true};
	__property UnicodeString Description = {read=FDescription, write=SetDescription};
	__property UnicodeString EditByDefault = {read=GetEditByDefault, write=FEditByDefault, stored=IsEditByDefaultStored};
};

class EScxmlDuplicateStateIDException: public Exception {
private:
	const UnicodeString FStateID;
	const UnicodeString FScxmlName;
public:
	__fastcall EScxmlDuplicateStateIDException(const UnicodeString &sMsg, const UnicodeString &sScxmlName, const UnicodeString &sID);

	__property UnicodeString StateID = {read=FStateID};
	__property UnicodeString ScxmlName = {read=FScxmlName};
};

class EScxmlBaseShapeException: public Exception {
private:
	TScxmlBaseShape *FShape;
	UnicodeString FCategory;
public:
	__fastcall EScxmlBaseShapeException(TScxmlBaseShape *AShape, const UnicodeString &sCategory, const UnicodeString &sMsg);

	__property TScxmlBaseShape *Shape = {read=FShape};
	__property UnicodeString Category = {read=FCategory};
};

/* Функции проверяют, не пересекаются ли элементы и сдвигают всё последовательно */
extern void CheckVisualScxmlShapePos(TCustomTree *ATree, TVisualScxmlBaseShape *AVisualShape, int iVisualIndex);
extern void CheckVisualScxmlStateBounds(TCustomTree *ATree);
extern UnicodeString GetTreeSyntaxScheme(TCustomTree *ATree);

#endif
