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

#ifndef UnitWatchShapeH
#define UnitWatchShapeH
//---------------------------------------------------------------------------

#include "UnitScxmlBaseShape.h"

class TWatchLabel: public TLabel {
private:
	UnicodeString FWatchLabel;
	UnicodeString FRegExp;
public:
	__fastcall TWatchLabel(TComponent *AOwner, const UnicodeString &sLabel, const UnicodeString &sRegExp);

	void __fastcall Parse(const UnicodeString &sText);

	__property UnicodeString WatchLabel = { read=FWatchLabel };
	__property UnicodeString RegExp = {read=FRegExp };
};

class TWatchShape: public TChildScxmlBaseShape {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual UnicodeString __fastcall OnGetHTMLPropertyInfo(const UnicodeString &sPropName);
private:
	UnicodeString FRegExp;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(RegExp);

	UnicodeString FLabel;
	SET_SHAPE_VALUE_UPDATESIMPLETEXT(Label);
protected:

	virtual TStateChildType __fastcall GetStateChildType(void) { return sctWatch; }

	virtual UnicodeString __fastcall GetOutValue();

	virtual TFontStyles __fastcall GetNormalFontStyle();

	virtual bool __fastcall HasVectorImage(void);
	virtual void __fastcall DrawVectorImage(const TRect &AImageBounds);

public:

	__fastcall virtual TWatchShape(Classes::TComponent* AOwner);

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	bool __fastcall EditRegExp(void);

__published:

	__property UnicodeString Label = {read=FLabel,write=SetLabel};
	__property UnicodeString RegExp = {read=FRegExp,write=SetRegExp};

};

#endif
