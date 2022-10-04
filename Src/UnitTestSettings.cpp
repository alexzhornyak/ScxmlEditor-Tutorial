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

#include "UnitTestSettings.h"

#include <IOUtils.hpp>
#include "Log4cpp_VCL.hpp"

#include "UnitMain.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitSettings.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterTestSettings() {
#pragma startup RegisterTestSettings

	Classes::RegisterClassA(__classid(TTestSettings));
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TExternTesterExePropEditor : public TLMDStringPropEditor {
protected:
	virtual Lmdinspropinsp::TLMDPropAttrs __fastcall GetAttrs(void) {
		return TLMDStringPropEditor::GetAttrs() << praValueList;
	}

	virtual void __fastcall GetValues(Classes::TStrings* AValues) {
		AValues->Add(USCXML_TESTER_EXE);
		AValues->Add(QT_SCXML_TESTER_EXE);
	}
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TTestSettings::TTestSettings(const TTestSettingsBaseType ABaseType) : BaseType(ABaseType) {
	FTestRemoteHost = "localhost";
	FTestRemotePort = 11001;
	FTestLocalPort = 11005;
	FTestHttpEnabled = false;
	FTestHttpListenPort = 7080;
	FTestHttpWebSocket = 7443;
	FTestHideDebugInfo = false;
	FTestingEnabled = false;
	FTestingCheckIssues = true;
	FTestUseCustomParams = false;
	FTestingScxmlMonitorLog = false;
	FTestingStopOnExit = false;
	FConvertOemToAnsi = false;
	FConvertOutputEncoding = coeDefault;
	FTriggersProtocol = tpUDP;
	FTriggersMsgFormat = tmfXML;

	FCaptureOutputRegExps = new TCaptureOutputRegexItems(this);
}

// ---------------------------------------------------------------------------
__fastcall TTestSettings::~TTestSettings(void) {
	if (FCaptureOutputRegExps) {
		delete FCaptureOutputRegExps;
		FCaptureOutputRegExps = NULL;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTestSettings::Assign(Classes::TPersistent* Source) {
	if (Source) {
		// ********************************************* //

		TTestSettings * ATestSettings = dynamic_cast<TTestSettings*>(Source);
		if (ATestSettings) {
			FTestRemoteHost = ATestSettings->FTestRemoteHost;
			FTestRemotePort = ATestSettings->FTestRemotePort;
			FTestLocalPort = ATestSettings->FTestLocalPort;
			FTestingEnabled = ATestSettings->FTestingEnabled;
			FTestingScxmlMonitorLog = ATestSettings->FTestingScxmlMonitorLog;
			FTestingStopOnExit = ATestSettings->FTestingStopOnExit;
			FTestUseCustomParams = ATestSettings->FTestUseCustomParams;
			FTestingCheckIssues = ATestSettings->FTestingCheckIssues;
			FTestHideDebugInfo = ATestSettings->FTestHideDebugInfo;
			FTestExeName = ATestSettings->FTestExeName;
			FTestWorkDir = ATestSettings->FTestWorkDir;
			FTestCMDParams = ATestSettings->FTestCMDParams;
			FTestHttpEnabled = ATestSettings->FTestHttpEnabled;
			FTestHttpListenPort = ATestSettings->FTestHttpListenPort;
			FTestHttpWebSocket = ATestSettings->FTestHttpWebSocket;
			FConvertOemToAnsi = ATestSettings->FConvertOemToAnsi;
			FConvertOutputEncoding = ATestSettings->FConvertOutputEncoding;
			FTriggersProtocol = ATestSettings->FTriggersProtocol;
			FTriggersMsgFormat = ATestSettings->FTriggersMsgFormat;

			FCaptureOutputRegExps->Assign(ATestSettings->FCaptureOutputRegExps);
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
bool TTestSettings::IsInheritedFromProjectOptions() {
	if (BaseType == tsbtApp && FormScxmlGui && FormScxmlGui->IsTestingBasedOnProjectSettings()) {
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
bool __fastcall TTestSettings::OnFilterPropEvent(const UnicodeString &sPropName) {
	if (sPropName == L"ConvertOemToAnsi") {
		return false;
	}

	// 1) не показываем 'Enabled' в режиме основных свойств
	// 2) если в проекте включены настройки, то переопределяем и скрываем

	return(BaseType == tsbtProject) ? true : (!SameStr(sPropName, "Enabled") && !IsInheritedFromProjectOptions());
}

// ---------------------------------------------------------------------------
void __fastcall TTestSettings::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();
		if (sPropName == L"ExeName") {
			AEditorClass = __classid(TExternTesterExePropEditor);
		}
		else if (sPropName == L"CaptureOutputRegExps") {
			AEditorClass = __classid(TCaptureOutputRegexesPropEditor);
		}
		else if (sPropName == L"WorkDir") {
			AEditorClass = __classid(TTestingCMDStringPropEditor);
		}
		else if (sPropName == L"CMDParams") {
			AEditorClass = __classid(TTestingCMDStringPropEditor);
		}
	}
}

//---------------------------------------------------------------------------
UnicodeString TTestSettings::GetExeFullPath(UnicodeString sExeName) {
	if (sExeName.Pos(L"%")) {
		sExeName = Unicodeutils::ExpandEnvironmentStringsDef(sExeName);
	}
	return TPath::IsPathRooted(sExeName) ? sExeName : TPath::GetFullPath(TPath::Combine(ExtractFilePath(ParamStr(0)), sExeName));
}

//---------------------------------------------------------------------------
UnicodeString TTestSettings::GetExpandedCMD(UnicodeString sCMD, const UnicodeString sBaseFilePath) {
	const UnicodeString sDir = ExtractFileDir(sBaseFilePath);
	const UnicodeString sFileName = ExtractFileName(sBaseFilePath);
	const UnicodeString sFileNameOnly = TPath::GetFileNameWithoutExtension(sFileName);

	sCMD = StringReplace(sCMD, L"$(UnitFilePath)", sBaseFilePath, TReplaceFlags() << rfReplaceAll);
	sCMD = StringReplace(sCMD, L"$(UnitFileDir)", sDir, TReplaceFlags() << rfReplaceAll);
	sCMD = StringReplace(sCMD, L"$(UnitFileName)", sFileName, TReplaceFlags() << rfReplaceAll);
	sCMD = StringReplace(sCMD, L"$(UnitName)", sFileNameOnly, TReplaceFlags() << rfReplaceAll);

	sCMD = StringReplace(sCMD, L"$(AppDir)", SettingsData->AppPath, TReplaceFlags() << rfReplaceAll);

	return Unicodeutils::ExpandEnvironmentStringsDef(sCMD);
}

//---------------------------------------------------------------------------
//---------------------- TTestSettingsPropEditor ----------------------------
//---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TTestSettingsPropEditor::GetValue(void) {
	UnicodeString sText = L"";

	try {
		TTestSettings *ASource = dynamic_cast<TTestSettings*>(this->GetObjectValue(0));
		if (!ASource)
			throw Exception("Can not cast to <TTestSettings>!");

		if (ASource->IsInheritedFromProjectOptions()) {
			sText = "%(Inherited from Project Options)";
		}
		else {
			if (ASource->BaseType == tsbtApp || ASource->Enabled) {
				sText = UnicodeString().sprintf(L"%s R:%d L:%d", ExtractFileName(ASource->ExeName).c_str(), ASource->RemotePort,
					ASource->LocalPort);
				if (ASource->HttpEnabled) {
					sText = sText + UnicodeString().sprintf(L" HttpL:%d HttpWeb:%d", ASource->HttpListenPort, ASource->HttpWebSocket);
				}
				if (!ASource->CMDParams.IsEmpty()) {
					sText = sText + " " + ASource->CMDParams;
				}
			}
			else {
				sText = L"Disabled";
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	return sText;
}

//---------------------------------------------------------------------------
//------------------- TTestingCMDStringPropEditor -------------------------
//---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TTestingCMDStringPropEditor::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

//---------------------------------------------------------------------------
void __fastcall TTestingCMDStringPropEditor::Edit(void) {
	try {
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			const UnicodeString sValue = this->GetStrValue(0);
			std::auto_ptr<TMacroComposer>ADialogPtr(new TMacroComposer(Application));

			ADialogPtr->EditDocSystem->AppendLine(L"%SystemRoot%=" + Unicodeutils::ExpandEnvironmentStringsDef("%SystemRoot%"));
			ADialogPtr->EditDocSystem->AppendLine(L"%ProgramFiles%=" + Unicodeutils::ExpandEnvironmentStringsDef("%ProgramFiles%"));
			ADialogPtr->EditDocSystem->AppendLine(L"%APPDATA%=" + Unicodeutils::ExpandEnvironmentStringsDef("%APPDATA%"));

			ADialogPtr->EditDocUser->AppendLine(L"$(AppDir)=" + SettingsData->AppPath);
			ADialogPtr->EditDocUser->AppendLine(L"$(UnitFilePath)=Path\\To\\ActiveUnitFileName.scxml");
			ADialogPtr->EditDocUser->AppendLine(L"$(UnitFileName)=ActiveUnitFileName.scxml");
			ADialogPtr->EditDocUser->AppendLine(L"$(UnitFileDir)=Path\\To");
			ADialogPtr->EditDocUser->AppendLine(L"$(UnitName)=ActiveUnitFileName");

			ADialogPtr->EditDocOriginal->Lines->Text = sValue;
			if (ADialogPtr->ShowModal() == mrOk) {
				this->SetStrValue(ADialogPtr->EditDocOriginal->Lines->Text.Trim());
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
