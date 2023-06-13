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

#ifndef UnitStateShapeH
#define UnitStateShapeH
//---------------------------------------------------------------------------

#include "UnitScxmlBaseShape.h"

typedef enum {shallow,deep} THistoryStateType;

class TStateShape : public TVisualScxmlBaseShape {
__published:
	//void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctState; }
public:

	__fastcall virtual TStateShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TStateShape();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString Id = {read=GetSimpleText,write=SetStateId};
	__property Initial;
	__property IsInitial;
};

//---------------------------- TParallelShape -------------------------------
class TParallelShape : public TVisualScxmlBaseShape {
__published:
	//void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:

protected:
	virtual TColor __fastcall GetNormalColor(void);
	virtual TColor __fastcall GetHeadColor(void);

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctParallel; }
public:

	__fastcall virtual TParallelShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TParallelShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString Id = {read=GetSimpleText,write=SetStateId};
	__property IsInitial;
};

//------------------------------ TFinalShape --------------------------------
class TFinalShape : public TVisualScxmlBaseShape {
__published:
	//void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:

protected:

	virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R);
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctFinal; }
	virtual TColor __fastcall GetNormalColor(void);
	virtual TTreeShapeStyle __fastcall GetNormalStyle(void);
public:

	__fastcall virtual TFinalShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TFinalShape() {};

    // устанавливает размеры по умолчанию
	virtual void __fastcall SetDefaultSize();

    virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property UnicodeString Id = {read=GetSimpleText,write=SetStateId};
};

//------------------------------ TFinalShape --------------------------------
class THistoryShape : public TVisualScxmlBaseShape {
__published:
	//void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	THistoryStateType FHistoryType;
    void __fastcall SetHistoryType(THistoryStateType val);
protected:
	virtual TColor __fastcall GetNormalColor(void);
	virtual TTreeShapeStyle __fastcall GetNormalStyle(void);
	virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R);
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctHistory; }
public:

	__fastcall virtual THistoryShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~THistoryShape() {};

	// устанавливает размеры по умолчанию
	virtual void __fastcall SetDefaultSize();
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	virtual bool __fastcall IsCluster(void);
__published:
	__property UnicodeString Id = {read=GetSimpleText,write=SetStateId};
	__property THistoryStateType Type = {read=FHistoryType,write=SetHistoryType,default=THistoryStateType::shallow};
};

//---------------------------------------------------------------------------
/* TInitialShape */
class TInitialShape : public TVisualScxmlBaseShape {
__published:
	//void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
protected:
	virtual TColor __fastcall GetNormalColor(void);
	virtual TTreeShapeStyle __fastcall GetNormalStyle(void);
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctInitial; }

    inline virtual bool __fastcall GetOccursOnce(void) { return true; }

public:

	__fastcall virtual TInitialShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TInitialShape();

	// устанавливает размеры по умолчанию
	virtual void __fastcall SetDefaultSize();

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	virtual bool __fastcall IsCluster(void);

	inline virtual bool __fastcall MustSimpleTextBeUnique(void) { return false;} /* override */

__published:
};

//---------------------------- TVirtualShape -------------------------------
class TVirtualShape : public TVisualScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString & sPropName);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FSrc;
	void __fastcall SetSrc(UnicodeString val) { FSrc = val; UpdateView(); }

	UnicodeString FAlias;
	UnicodeString FAliasParams;
	UnicodeString __fastcall GetAliasVar(void);

protected:
	virtual TColor __fastcall GetNormalColor(void);
	virtual TColor __fastcall GetHeadColor(void);

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctVirtual; }

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	virtual UnicodeString __fastcall GetFullPath(void);
	virtual TStateMachineEditorUnit * __fastcall GetSrcUnit(void);

	inline virtual UnicodeString __fastcall GetFileName(void) { return ExtractFileName(Src); }
	virtual void __fastcall DrawShapeCanvas(Tecanvas::TCanvas3D * ACanvas, const Types::TRect & R);

public:

	__fastcall virtual TVirtualShape(Classes::TComponent* AOwner);
	__fastcall virtual ~TVirtualShape();

	__property UnicodeString FileName = {read=GetFileName};
	__property UnicodeString FullPath = {read=GetFullPath};
	__property TStateMachineEditorUnit *SrcUnit = {read=GetSrcUnit};

	bool __fastcall EditSrc(void);
	TStateMachineEditorUnit * __fastcall NewVirtualUnit(void);

	virtual void __fastcall ValidateShape(); /* override */

    bool __fastcall IsSrcValid();

	UnicodeString __fastcall GetAliasedText(UnicodeString sText);
	UnicodeString __fastcall GetAliasedParamsText(UnicodeString sText);

__published:
	/* SCXML properties */
	__property UnicodeString Id = {read=GetSimpleText,write=SetStateId};
	__property UnicodeString Src = {read=FSrc,write=SetSrc};
	__property IsInitial;

	/* Virtual Alias properties */
	__property UnicodeString AliasVar = {read=GetAliasVar, stored=false};
	__property UnicodeString Alias = {read=FAlias, write=FAlias};
	__property UnicodeString AliasParams = {read=FAliasParams, write=FAliasParams};
};

#endif
