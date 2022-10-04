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

#include "UnitPostBuildInterface.h"

#include <memory>

#include "Log4cpp_VCL.hpp"

#include "UnitMacroComposer.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void UpdateEvaluated(TStrings *EvalCMD, TStrings *OrigCMD, TStrings *Macros, TStrings *UserMacros) {
	EvalCMD->Text = OrigCMD->Text;
	for (int i = 0; i < Macros->Count; i++) {
		if (!Macros->Names[i].IsEmpty()) {
			EvalCMD->Text = StringReplace(EvalCMD->Text, Macros->Names[i], Macros->ValueFromIndex[i], TReplaceFlags() << rfReplaceAll);
		}
	}
	for (int i = 0; i < UserMacros->Count; i++) {
		if (!UserMacros->Names[i].IsEmpty()) {
			EvalCMD->Text = StringReplace(EvalCMD->Text, UserMacros->Names[i], UserMacros->ValueFromIndex[i],
				TReplaceFlags() << rfReplaceAll);
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TIPostBuildInterfaceClassPropEditor : public TLMDClassPropEditor {
private:

protected:
	virtual TLMDPropAttrs __fastcall GetAttrs() {
		return TLMDClassPropEditor::GetAttrs() << praDialog << praReadOnly >> praMultiSelect;
	}

	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		UnicodeString sText = L"";

		try {
			TStrings *AStrings = dynamic_cast<TStrings*>(this->GetObjectValue(0));
			if (!AStrings)
				throw Exception("Can not cast to <TStrings>!");

			sText = AStrings->Text;
			const int i_MAX_LENGTH = 60;
			if (sText.Length() > i_MAX_LENGTH) {
				sText = sText.SetLength(i_MAX_LENGTH).Trim() + "...";
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}

		return sText;

	}

public:
	virtual void __fastcall Edit(void) {
		try {
			IPostBuildInterface *APostBuildInterface = dynamic_cast<IPostBuildInterface*>(this->GetInstance(0));
			if (!APostBuildInterface)
				throw Exception("Can not cast to <IPostBuildInterface>!");
			std::auto_ptr<TMacroComposer>AMacroComposer(new TMacroComposer(Application, APostBuildInterface));
			if (AMacroComposer->ShowModal() == mrOk) {
				APostBuildInterface->PostBuildCMD->Text = AMacroComposer->EditDocOriginal->Lines->Text;
				APostBuildInterface->UserMacros->Text = AMacroComposer->EditDocUser->Lines->Text;
			}

			if (this->PropInsp) {
				this->PropInsp->UpdateContent();
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
};

// ---------------------------------------------------------------------------
// ------------------------- TMacroComposerPropEditor ------------------------
// ---------------------------------------------------------------------------

void __fastcall TMacroComposerPropEditor::Edit(void) {
	try {
		IPostBuildInterface *APostBuildInterface = dynamic_cast<IPostBuildInterface*>(this->GetObjectValue(0));
		if (!APostBuildInterface)
			throw Exception("Can not cast to <IPostBuildInterface>!");
		std::auto_ptr<TMacroComposer>AMacroComposer(new TMacroComposer(Application, APostBuildInterface));
		if (AMacroComposer->ShowModal() == mrOk) {
			APostBuildInterface->PostBuildCMD->Text = AMacroComposer->EditDocOriginal->Lines->Text;
			APostBuildInterface->UserMacros->Text = AMacroComposer->EditDocUser->Lines->Text;
		}

		if (this->PropInsp) {
			this->PropInsp->UpdateContent();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TMacroComposerPropEditor::GetAttrs() {
	return TLMDClassPropEditor::GetAttrs() << praDialog << praReadOnly >> praMultiSelect;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TMacroComposerPropEditor::GetValue(void) {
	UnicodeString sText = L"";

	try {
		IPostBuildInterface *APostBuildInterface = dynamic_cast<IPostBuildInterface*>(this->GetObjectValue(0));
		if (!APostBuildInterface)
			throw Exception("Can not cast to <IPostBuildInterface>!");

		sText = APostBuildInterface->PostBuildEvaluated;
		const int i_MAX_LENGTH = 60;
		if (sText.Length() > i_MAX_LENGTH) {
			sText = sText.SetLength(i_MAX_LENGTH).Trim() + "...";
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	return sText;
}

// ---------------------------------------------------------------------------
// --------------------------- IPostBuildInterface ---------------------------
// ---------------------------------------------------------------------------
__fastcall IPostBuildInterface::IPostBuildInterface() {
	FPostBuildCMD = new TStringList;
	FUserMacros = new TStringList;
}

// ---------------------------------------------------------------------------
__fastcall IPostBuildInterface::~IPostBuildInterface() {
	if (FPostBuildCMD) {
		delete FPostBuildCMD;
	}
	if (FUserMacros) {
		delete FUserMacros;
	}
}

// ---------------------------------------------------------------------------
void __fastcall IPostBuildInterface::SetPostBuildCMD(TStrings *val) {
	FPostBuildCMD->Assign(val);
}

// ---------------------------------------------------------------------------
void __fastcall IPostBuildInterface::Assign(TPersistent* Source) {
	if (Source) {
		IPostBuildInterface * AInstance = dynamic_cast<IPostBuildInterface*>(Source);
		if (AInstance) {
			FPostBuildCMD->Assign(AInstance->FPostBuildCMD);
			FUserMacros->Assign(AInstance->FUserMacros);
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall IPostBuildInterface::GetPostBuildEvaluated() const {
	std::auto_ptr<TStringList>APostBuildEvaluatedPtr(new TStringList());
	std::auto_ptr<TStringList>AMacrosPtr(new TStringList());
	AMacrosPtr->Text = Macros;
	UpdateEvaluated(APostBuildEvaluatedPtr.get(), FPostBuildCMD, AMacrosPtr.get(), FUserMacros);
	return APostBuildEvaluatedPtr->Text;
}

// ---------------------------------------------------------------------------
void __fastcall IPostBuildInterface::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {

		const UnicodeString sPropName = APropInfo->Name();

		std::auto_ptr<TStringList>APostBuildClassEditor(new TStringList());
		APostBuildClassEditor->CaseSensitive = true;
		APostBuildClassEditor->Add("PostBuildCMD");
		APostBuildClassEditor->Add("UserMacros");

		if (APostBuildClassEditor->IndexOf(sPropName) != -1) {
			AEditorClass = __classid(TIPostBuildInterfaceClassPropEditor);
		}
	}
}


