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

#ifndef UnitInheritanceResolverH
#define UnitInheritanceResolverH
//---------------------------------------------------------------------------

#include "LMDInsPropEditors.hpp"

class TStateMachineEditorUnit;

typedef TStateMachineEditorUnit * __fastcall (__closure *TGetStateMachineEditorUnit)(void);

class TInheritanceResolver: public TPersistent {
private:
	/* Inheritance resolving */
	bool					FIgnoreControlBinding;
	TStrings *				FResolvedProps;
	TStrings *				FSkippedProps;
	TStrings *				FDefaultSkippedProps;
	TStrings * __fastcall 	GetSkippedProps(void);
	TStrings * __fastcall 	GetDefaultSkippedProps(void);
	void __fastcall			SetResolvedProps(TStrings *val);

protected:

	TGetStateMachineEditorUnit FGetStateMachineEditorUnit;

	virtual void __fastcall AssignDefaultProps(TStrings * ADefaultSkippedProps);

    friend class TInheritanceResolverPropEditor;

public:

	__fastcall TInheritanceResolver(TGetStateMachineEditorUnit AGetStateMachineEditorUnit);
	__fastcall ~TInheritanceResolver(void);

	virtual void __fastcall TInheritanceResolver::Assign(Classes::TPersistent * Source);

	__property TStrings * SkippedProps = {read=GetSkippedProps};
	__property TStrings * DefaultSkippedProps = {read=GetDefaultSkippedProps};

__published:
	__property TStrings * ResolvedProps = {read=FResolvedProps, write=SetResolvedProps};
};

//---------------------------------------------------------------------------
class TInheritanceResolverPropEditor : public TLMDClassPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
	virtual bool __fastcall AllEqual(void);
public:
	virtual void __fastcall Edit(void);
};

#endif
