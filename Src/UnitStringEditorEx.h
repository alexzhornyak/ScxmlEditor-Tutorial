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

#ifndef UnitStringEditorExH
#define UnitStringEditorExH
//---------------------------------------------------------------------------

#include "LMDInsPropEditors.hpp"

// возможность редактирования строк только у одного объекта
class PACKAGE TStringsEditorEx : public TLMDClassPropEditor {
private:

protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();

public:
	virtual void __fastcall Edit(void);
};

// возможность редактирования строк у многих объектов
class PACKAGE TMultiStringsEditorEx : public TStringsEditorEx {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
};

// редактирование у многих + показывание того, что в данный момент отмечено
class PACKAGE TMultiDisplayStringsEditorEx : public TMultiStringsEditorEx {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
};

class TStringDialogEditor : public TLMDStringPropEditor
{
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
public:
	inline __fastcall virtual TStringDialogEditor(Lmdinspropinsp::TLMDCustomPropertyInspector* APropInsp, Lmdinspropinsp::TLMDPropEditor* AParentObjEditor, int APropCount, void * APropList)
	: TLMDStringPropEditor(APropInsp, AParentObjEditor, APropCount, APropList) { }

};

// ---------------------------------------------------------------------------
class TFormStringPairsEditor;

// возможность редактирования ключ-значение
class TStringsPairsEditor : public TLMDClassPropEditor {
private:

protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();

	virtual void __fastcall OnFormCreated(TFormStringPairsEditor *AFormEditor) {
		// здесь можно назначить свои редакторы
	};

public:
	virtual void __fastcall Edit(void);
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
};

// ---------------------------------------------------------------------------
class TStringTextPairsEditor : public TLMDStringPropEditor
{
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
public:
	inline __fastcall virtual TStringTextPairsEditor(Lmdinspropinsp::TLMDCustomPropertyInspector* APropInsp, Lmdinspropinsp::TLMDPropEditor* AParentObjEditor, int APropCount, void * APropList)
	: TLMDStringPropEditor(APropInsp, AParentObjEditor, APropCount, APropList) { }

};

// ---------------------------------------------------------------------------
class TXMLStringPropEditor : public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
};

class TLineEditStringPropEditor: public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
};

class TEventIDPropEditor: public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
};

class TSendIDPropEditor: public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
};

class TTransitionEventPropEditor: public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

extern bool __fastcall __EditUnicodeStringLineProperty(TPersistent *AInstance, const UnicodeString &sPropName);

extern bool __fastcall __EditEventIDLineProperty(TPersistent *AInstance, const UnicodeString &sPropName);

extern bool __fastcall __EditSendIDLineProperty(TPersistent *AInstance, const UnicodeString &sPropName);

#endif
