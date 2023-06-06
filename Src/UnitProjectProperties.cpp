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

#include "UnitProjectProperties.h"

#include "Log4cpp_VCL.hpp"
#include <IOUtils.hpp>

#include "UnitStateMachineUnit.h"
#include "UnitSettings.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterProjectProperties() {
#pragma startup RegisterProjectProperties

	Classes::RegisterClassA(__classid(TProjectProperties));
	Classes::RegisterClassA(__classid(TProjectTriggerItem));
	Classes::RegisterClassA(__classid(TProjectTriggerItems));
}

// ---------------------------------------------------------------------------
// -------------------------- TProjectPostBuild ------------------------------
// ---------------------------------------------------------------------------

__fastcall TProjectPostBuild::TProjectPostBuild(TStateMachineProject *AProject) : FProject(AProject) {

}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TProjectPostBuild::GetMacros(void)const {
	std::auto_ptr<TStringList>AMacrosPtr(new TStringList);
	if (FProject) {
		AMacrosPtr->Values["$(ProjectFilePath)"] = FProject->FilePath;
		AMacrosPtr->Values["$(DisplayName)"] = FProject->DisplayName;
		AMacrosPtr->Values["$(ProjectDir)"] = ExtractFileDir(FProject->FilePath);
		AMacrosPtr->Values["$(ProjectName)"] = TPath::GetFileNameWithoutExtension(FProject->FilePath);
		AMacrosPtr->Values["$(AppDir)"] = ExtractFileDir(Application->ExeName);
		AMacrosPtr->Values["$(AppFilePath)"] = Application->ExeName;
		AMacrosPtr->Values["$(AppHandle)"] = UnicodeString((int)Application->MainFormHandle);

		if (FProject && FProject->ProjectRoot) {
			const UnicodeString sFlatPath = ChangeFileExt(FProject->ProjectRoot->FilePath, ".flat.scxml");
			AMacrosPtr->Values["$(FlatFilePath)"] = sFlatPath;
			AMacrosPtr->Values["$(FlatDisplayName)"] = ExtractFileName(sFlatPath);
			AMacrosPtr->Values["$(FlatDir)"] = ExtractFileDir(sFlatPath);
			// здесь именно оригинальное имя, так как иначе обрежет только крайнее расширение
			AMacrosPtr->Values["$(FlatName)"] = TPath::GetFileNameWithoutExtension(FProject->ProjectRoot->FilePath);
		}
	}

	return AMacrosPtr->Text;
}

// ---------------------------------------------------------------------------
// ---------------------------- TProjectProperties ---------------------------
// ---------------------------------------------------------------------------
__fastcall TProjectProperties::TProjectProperties(TComponent* Owner, TStateMachineProject *AProject) : TSettingsBase(Owner),
FProject(AProject) {
	FTriggers = new TProjectTriggerItems(this);

	FFilteredMsgs = new TTestingMsgItems(this);
	FTestSettings = new TTestSettings(tsbtProject);
	FProjectPostBuild = new TProjectPostBuild(FProject);

	FBuildTypes = TProjectBuildTypes() << pbtSaveAll << pbtSaveFlat << pbtPostBuild;

	FCalibratorCfg = "";
	FCanserviceCfg = "";

	FDisableSameControls = false;

	FProjectDefines = L"";

	FVerifyChecksums = true;
}

// ---------------------------------------------------------------------------
__fastcall TProjectProperties::~TProjectProperties() {
	if (FTriggers) {
		delete FTriggers;
		FTriggers = 0;
	}
	if (FFilteredMsgs) {
		delete FFilteredMsgs;
		FFilteredMsgs = 0;
	}
	if (FTestSettings) {
		delete FTestSettings;
		FTestSettings = 0;
	}
	if (FProjectPostBuild) {
		delete FProjectPostBuild;
		FProjectPostBuild = 0;
	}
}

// ---------------------------------------------------------------------------
TComponent * __fastcall TProjectProperties::DoCreateDefaultInstance() {
	return new TProjectProperties(this->Owner, FProject);
}

// ---------------------------------------------------------------------------
void __fastcall TProjectProperties::Assign(TPersistent* Source) {
	if (Source) {
		TProjectProperties * AProjectProperties = dynamic_cast<TProjectProperties*>(Source);
		if (AProjectProperties) {
			FTriggers->Assign(AProjectProperties->FTriggers);
			FFilteredMsgs->Assign(AProjectProperties->FFilteredMsgs);
			FTestSettings->Assign(AProjectProperties->FTestSettings);
			FProjectPostBuild->Assign(AProjectProperties->FProjectPostBuild);

			FCalibratorCfg = AProjectProperties->FCalibratorCfg;
			FCanserviceCfg = AProjectProperties->FCanserviceCfg;

			FDisableSameControls = AProjectProperties->FDisableSameControls;

			FBuildTypes = AProjectProperties->FBuildTypes;

			FProjectDefines = AProjectProperties->FProjectDefines;
			FVerifyChecksums = AProjectProperties->FVerifyChecksums;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TProjectProperties::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {

		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"Triggers") {
			AEditorClass = __classid(TPropTriggersPropEditor);
		}
		else if (sPropName == L"FilteredMsgs") {
			AEditorClass = __classid(TTestingMsgPropEditor);
		}
		else if (sPropName == L"ProjectPostBuild") {
			AEditorClass = __classid(TMacroComposerPropEditor);
		}
		else if (sPropName == L"TestSettings") {
			AEditorClass = __classid(TTestSettingsPropEditor);
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TProjectProperties::OnFilterPropEvent(const UnicodeString &sPropName) {
	if (!SettingsData->AviaExtensionEnabled) {
		std::auto_ptr<TStringList>AStringListPtr(new TStringList());
		AStringListPtr->CaseSensitive = true;
		AStringListPtr->Add("CalibratorCfg");
		AStringListPtr->Add("CanserviceCfg");
		AStringListPtr->Add("DisableSameControls");
		if (AStringListPtr->IndexOf(sPropName) != -1)
			return false;
	}
	return !IsPublishedProp(__classid(TComponent), sPropName);
}

// ---------------------------------------------------------------------------
void __fastcall TProjectProperties::SetTestSettings(TTestSettings *val) {
	FTestSettings->Assign(val);
}

// ---------------------------------------------------------------------------
void __fastcall TProjectProperties::SetProjectPostBuild(TProjectPostBuild *val) {
	FProjectPostBuild->Assign(val);
}

// ---------------------------------------------------------------------------
bool __fastcall TProjectProperties::OnSerializeCustomGetProp(TObject *Sender, const UnicodeString &sPropName, _di_IXMLNode APropertyNode) {
	try {
		if (Sender->ClassType() == __classid(TProjectProperties)) {
			if (SameStr(sPropName, "PostBuildCMD")) {
				this->ProjectPostBuild->PostBuildCMD->Text = APropertyNode->Text;
				return true;
			}
			if (SameStr(sPropName, "UserMacros")) {
				this->ProjectPostBuild->UserMacros->Text = APropertyNode->Text;
				return true;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	return false;
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
