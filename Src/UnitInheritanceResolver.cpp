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

#include <vcl.h>
#pragma hdrstop

#include "UnitInheritanceResolver.h"
#include "UnitSettings.h"
#include "UnitStateMachineUnit.h"
#include "UnitStringsEditor.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitTreeEditorStateMachine.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterInheritanceResolver() {
#pragma startup RegisterInheritanceResolver

	Classes::RegisterClassA(__classid(TInheritanceResolver));
}

// ---------------------------------------------------------------------------
__fastcall TInheritanceResolver::TInheritanceResolver(TGetStateMachineEditorUnit AGetStateMachineEditorUnit) {
	FIgnoreControlBinding = false;
	FResolvedProps = new TStringList;
	FSkippedProps = NULL;
	FDefaultSkippedProps = NULL;
	FGetStateMachineEditorUnit = AGetStateMachineEditorUnit;
}

// ---------------------------------------------------------------------------
__fastcall TInheritanceResolver::~TInheritanceResolver() {
	if (FResolvedProps) {
		delete FResolvedProps;
		FResolvedProps = NULL;
	}
	if (FSkippedProps) {
		delete FSkippedProps;
		FSkippedProps = NULL;
	}
	if (FDefaultSkippedProps) {
		delete FDefaultSkippedProps;
		FDefaultSkippedProps = NULL;
	}
}

//---------------------------------------------------------------------------
void __fastcall TInheritanceResolver::AssignDefaultProps(TStrings * ADefaultSkippedProps) {
	ADefaultSkippedProps->Add("Locked");
	ADefaultSkippedProps->Add("InheritanceMismatch");
	ADefaultSkippedProps->Add("InheritanceResolver");
	ADefaultSkippedProps->Add("Color");
	ADefaultSkippedProps->Add("BackColor");
	ADefaultSkippedProps->Add("Style");
	ADefaultSkippedProps->Add("Text");
	ADefaultSkippedProps->Add("ArrowFrom");
	ADefaultSkippedProps->Add("ArrowTo");
	ADefaultSkippedProps->Add("Border");
	ADefaultSkippedProps->Add("Brush");
	ADefaultSkippedProps->Add("Gradient");
	ADefaultSkippedProps->Add("Shadow");
	ADefaultSkippedProps->Add("Font");
	ADefaultSkippedProps->Add("Transparent");
}

// ---------------------------------------------------------------------------
TStrings * __fastcall TInheritanceResolver::GetDefaultSkippedProps(void) {

	if (!FDefaultSkippedProps) {
		FDefaultSkippedProps = new TStringList;

		AssignDefaultProps(FDefaultSkippedProps);
	}

	const bool bIgnoreControlBinding = (!SettingsData->AviaExtensionEnabled) ||
	(FGetStateMachineEditorUnit && FGetStateMachineEditorUnit()->BindingsDisabled);
	if (FIgnoreControlBinding != bIgnoreControlBinding) {
		FIgnoreControlBinding = bIgnoreControlBinding;

		const int iIndex = FDefaultSkippedProps->IndexOf("ProtocolControlBinding");
		if (FIgnoreControlBinding) {
			if (iIndex == -1) {
				FDefaultSkippedProps->Add("ProtocolControlBinding");
			}
		}
		else {
			if (iIndex != -1) {
				FDefaultSkippedProps->Delete(iIndex);
			}
		}
	}

	return FDefaultSkippedProps;
}

// ---------------------------------------------------------------------------
TStrings * __fastcall TInheritanceResolver::GetSkippedProps(void) {
	if (!FSkippedProps) {
		FSkippedProps = new TStringList;
	}

	FSkippedProps->Clear();

	FSkippedProps->AddStrings(GetDefaultSkippedProps());
	FSkippedProps->AddStrings(FResolvedProps);

	return FSkippedProps;
}

// ---------------------------------------------------------------------------
void __fastcall TInheritanceResolver::SetResolvedProps(TStrings *val) {
	FResolvedProps->Assign(val);
}

// ---------------------------------------------------------------------------
void __fastcall TInheritanceResolver::Assign(Classes::TPersistent * Source) {
	if (Source) {
		TInheritanceResolver *AResolver = dynamic_cast<TInheritanceResolver*>(Source);
		if (AResolver) {
			FResolvedProps->Assign(AResolver->FResolvedProps);
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void __fastcall TInheritanceResolverPropEditor::Edit(void) {
	TInheritanceResolver * AInheritanceResolver = dynamic_cast<TInheritanceResolver*>(this->GetObjectValue(0));
	if (AInheritanceResolver) {
		std::auto_ptr<TStringEditorForm>AStrEditForm(new TStringEditorForm(Application));
		AStrEditForm->sef_Memo->Lines->Assign(AInheritanceResolver->ResolvedProps);
		if (AStrEditForm->ShowModal() == mrOk && Propinspext::IsWritablePropertyOfPropEditor(this)) {

			for (int i = 0; i < this->PropCount; i++) {
				TInheritanceResolver * AInheritanceResolver = dynamic_cast<TInheritanceResolver*>(this->GetObjectValue(i));
				if (AInheritanceResolver) {
					AInheritanceResolver->ResolvedProps->Text = AStrEditForm->sef_Memo->Lines->Text.Trim();
				}
			}

			if (AInheritanceResolver->FGetStateMachineEditorUnit) {
				TStateMachineEditorUnit *StateMachineEditorUnit = AInheritanceResolver->FGetStateMachineEditorUnit();
				if (StateMachineEditorUnit && StateMachineEditorUnit->StateMachineDockPanel) {
					StateMachineEditorUnit->StateMachineDockPanel->InvalidateTree();
				}
			}

			if (this->PropInsp) {
				this->PropInsp->UpdateContent();
			}
		}
	}
}

//---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TInheritanceResolverPropEditor::GetAttrs() {
	return TLMDPropAttrs() << praDialog << praReadOnly << praMultiSelect;
}

// ---------------------------------------------------------------------------
bool __fastcall TInheritanceResolverPropEditor::AllEqual(void) {
	UnicodeString sTemplate = L"";
	for (int i = 0; i < this->PropCount; i++) {
		TInheritanceResolver * AInheritanceResolver = dynamic_cast<TInheritanceResolver*>(this->GetObjectValue(i));
		if (!AInheritanceResolver)
			return false;

		if (i == 0) {
			sTemplate = AInheritanceResolver->ResolvedProps->Text;
		}
		else {
			if (sTemplate != AInheritanceResolver->ResolvedProps->Text)
				return false;
		}
	}
	return true;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TInheritanceResolverPropEditor::GetValue(void) {

	TInheritanceResolver * AInheritanceResolver = dynamic_cast<TInheritanceResolver*>(this->GetObjectValue(0));
	if (AInheritanceResolver) {
		return AInheritanceResolver->ResolvedProps->CommaText;
	}
	return L"";
}
