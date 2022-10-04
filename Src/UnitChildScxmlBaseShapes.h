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

#ifndef UnitChildScxmlBaseShapesH
#define UnitChildScxmlBaseShapesH
//---------------------------------------------------------------------------

#include "UnitScxmlBaseShape.h"

//---------------------------------------------------------------------------
class TOnEntryStateShape : public TChildScxmlBaseShape {
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctOnentry; }

	inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TOnEntryStateShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TOnEntryStateShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
};

//---------------------------------------------------------------------------
class TOnExitStateShape : public TChildScxmlBaseShape {
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctOnexit; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TOnExitStateShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TOnExitStateShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
};

//---------------------------------------------------------------------------
class TScriptShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor);
private:

	bool FDisableFlatBuild;

	UnicodeString FSrc;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Src);
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctScript; }

	virtual UnicodeString __fastcall GetOutValue();

    virtual TColor __fastcall GetNormalColor(void);

    virtual std::vector<TDefaultEditorPair> GetEditByDefaultEditors(void);

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall TScriptShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TScriptShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

    virtual void __fastcall ValidateXMLText(void);

	void EditSrc(void);

__published:
	__property UnicodeString Src = {read=FSrc,write=SetSrc};
	__property XMLText;
	__property bool DisableFlatBuild = {read=FDisableFlatBuild, write=FDisableFlatBuild, default=false};
};

//---------------------------------------------------------------------------
class TCommentShape: public TChildScxmlBaseShape {
private:
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctComment; }
	void __fastcall OnCommentsChanged(TObject *Sender);

	virtual TColor __fastcall GetNormalFontColor(void);
	virtual TFontStyles __fastcall GetNormalFontStyle();

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall TCommentShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TCommentShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	virtual void __fastcall UpdateSimpleText();

__published:
	__property Comments;
};

//---------------------------------------------------------------------------
class TExtraContentShape: public TChildScxmlBaseShape {
private:
	void __fastcall OnExtraContextChanged(TObject *Sender);
protected:
	virtual TStateChildType __fastcall GetStateChildType(void) { return sctExtraContent; }
	virtual UnicodeString __fastcall GetOutValue(void) { return this->ExtraContent->Text; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall TExtraContentShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TExtraContentShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

__published:
	__property ExtraContent;
};

//---------------------------------------------------------------------------
class TTransitionXMLShape : public TChildScxmlBaseShape {
__published:
	// это событие вызывается для проверки фильтрации свойств
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);
private:

	bool FErrorState;

protected:

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctTransitionXML; }

	virtual void __fastcall UpdateSimpleText();

	inline virtual bool __fastcall GetOccursOnce(void) { return true; }

    inline virtual bool __fastcall HasVectorImage(void) { return true; };
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TTransitionXMLShape(Classes::TComponent* AOwner);
	inline __fastcall virtual ~TTransitionXMLShape() {};

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	void __fastcall SetErrorState(bool bError);

__published:
};

#endif
