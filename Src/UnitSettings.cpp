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

#include "UnitSettings.h"

#include <memory>

#include <IOUtils.hpp>

#include "UnitCustomUtils.h"
#include "Log4cpp_VCL.hpp"
#include "UnitDModule.h"
#include "UnitMain.h"
#include "UnitSyntaxEditorForm.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitStringEditorEx.h"
#include "UnitStateMachineUnit.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

TSettingsData *SettingsData = NULL;

// ---------------------------------------------------------------------------
void RegisterSettingsData() {
#pragma startup RegisterSettingsData
	Classes::RegisterClass(__classid(TSettingsData));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TSettingsData::TSettingsData(TComponent* Owner) : TSettingsBase(Owner) {

	IsTesterWorking = false;

	FLockUI = false;

	FMaxRecentCount = 15;
	FConnectionTextAngle = 90;
	FConnectionVertTextAlign = vtaTop;
	FConnectionHorizTextAlign = htaCenter;
	FConnectionVertOffset = 20;
	FConnectionHorizOffset = 0;
	FConnectionAdditionalHighlight = true;
	FConnectionFromToShapesHighlight = true;
	FPropInspGutterWidth = 150;
	FPropInspInfoHeight = 100;
	FHintHidePause = 2500;
	FHintPause = 50;
	FHintShortPause = 0;

	FAutoSaveCache = true;

	FExternScriptEditorFileName = L"";
	FExternScriptEditorEnabled = false;
	FExternScriptEditorExtension = L"";
	FVirtualAliasVariable = L"";
	FCheckPositionOnLoadScxml = true;
	FSaveCommentsAnywayOnLoad = false;
	FParserDoNotLoadQtEditorInfo = true;
	FParserSaveQtTransitionOffsetMargin = 5;
	FChartFocusEnabled = true;
	FChartFocusLineWidth = 2;
	FChartFocusLineColor = clHighlight;
	FChartActiveStateColor = Graphics::clDefault;
	FChartTestCoverageColor = Graphics::clPurple;

	FScxmlMsgTypes = DEFAULT_SETTINGS_SCXML_MSG_TYPES;
	FSkipCommentsInRawScxml = true;
	FForceSaveDefaultScxmlValues = false;

	FTestingShowCallStack = true;
	FTestingDebugReceivedBuffer = false;
	FTestingSwitchToPanel = true;
	FTestingScrollChartToViewShape = true;
	FTestingSendEventClickMode = seDoubleClick;
	FTestingAutoOpenUnit = true;

	FTranslateEveryChangeToScxmlView = false;
	FLuautilsLuacExe = L"";

	FZoomHintsEnabled = true;
	FZoomHintsStartLevel = 90;
	FZoomHintsFontSizeEnlargePercents = 120;

	FHideWindows = DEFAULT_HIDE_WINDOWS_STATUS;

	FAviaExtensionEnabled = false;
	FProfilingEnabled = false;

	FAutoLayoutSplines = alsOrtho;
	FAutoLayoutUseNativeLabelPos = false;
	FAutoLayoutCheckOverlappedLabel = true;

	FWarningOnInheritanceMismatch = true;
	FWarningOnBindingsNotFound = true;

	FUseVectorImages = false;
	FCompletionTimeoutEnabled = true;
	FCompletionTimeoutMsec = 50;

	FValidateInPredicate = false;
	FValidateChildren = true;
	FValidateXMLText = true;
	FValidateEmptyTransitions = true;

	FConnectionSelfDefaultHeight = 30;

	FSearchTypes = GetDefaultSearchTypes();
	FSearchDocumentType = esdtCurrentDoc;

	FRecentFiles = new TStringList;
	FEditorRecentFiles = new TStringList;
	FSearchLines = new TStringList;
	FReplaceLines = new TStringList;

	FAutoCompleteItems = new TAutoCompleteItems(this);
	this->AddDefaultAutoCompleteCollectionItems();

	FLoggingProperties = new TStringList;
	FTempRegistry = new TStringList;
	FRegistryItems = new TSettingsRegistryItems(this);

	FTestSettings = new TTestSettings(tsbtApp);
	FExportSvgSettings = new TExportSvgSettings();
	FThemeSettings = new TThemeSettings();

	FConnectionInvertedConditionMacro = new TStringList;

	FDefaultConnectionInvertedConditionMacro = new TStringList;
	FDefaultConnectionInvertedConditionMacro->Values[L"Lua"] = L"not (%COND%)";
	FDefaultConnectionInvertedConditionMacro->Values[L"CPP"] = L"! (%COND%)";
	FDefaultConnectionInvertedConditionMacro->Values[L"C"] = L"! (%COND%)";
	FDefaultConnectionInvertedConditionMacro->Values[L"EcmaScript"] = L"! (%COND%)";
	FDefaultConnectionInvertedConditionMacro->Values[L"JavaScript"] = L"! (%COND%)";
	FDefaultConnectionInvertedConditionMacro->Values[L"Python"] = L"not (%COND%)";

	{
		std::auto_ptr<TResourceStream>AResStreamPtr(new TResourceStream((int)HInstance, L"ResLogProperties", MAKEINTRESOURCEW(10)));
		std::auto_ptr<TStringStream>AStrStreamPtr(new TStringStream());
		AResStreamPtr->SaveToStream(AStrStreamPtr.get());
		AStrStreamPtr->Position = 0;
		FDefaultLoggingProperties = AStrStreamPtr->DataString.Trim();
		FLoggingProperties->Text = FDefaultLoggingProperties;
	}

	FEditorNodeTreeWidth = 160;

	Customutils::GetAppVersion(FAppVersion, FSystemLanguage);

	const UnicodeString sLayoutDir = GetAppLayoutDir();
	if (!DirectoryExists(sLayoutDir)) {
		TDirectory::CreateDirectoryA(sLayoutDir);
	}

	FTestApplicationPresets = new TTestApplicationPresetItems(this);
	this->AddDefaultTestingAppsCollectionItems();
}

// ---------------------------------------------------------------------------
__fastcall TSettingsData::~TSettingsData() {
	delete FRecentFiles;
	delete FEditorRecentFiles;
	delete FAutoCompleteItems;
	delete FSearchLines;
	delete FReplaceLines;
	delete FTempRegistry;
	delete FLoggingProperties;
	delete FTestSettings;
	delete FExportSvgSettings;
	delete FThemeSettings;
	delete FConnectionInvertedConditionMacro;
	delete FDefaultConnectionInvertedConditionMacro;
	delete FRegistryItems;
	delete FTestApplicationPresets;
}

// ---------------------------------------------------------------------------
TComponent * __fastcall TSettingsData::DoCreateDefaultInstance() {
	return new TSettingsData(this->Owner);
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsData::Assign(TPersistent* Source) {
	if (Source) {
		TSettingsData * ASettingsData = dynamic_cast<TSettingsData*>(Source);
		if (ASettingsData) {
			this->FRecentFiles->Assign(ASettingsData->FRecentFiles);
			this->FEditorRecentFiles->Assign(ASettingsData->FEditorRecentFiles);
			this->FMaxRecentCount = ASettingsData->FMaxRecentCount;

			FSearchTypes = ASettingsData->FSearchTypes;
			FSearchDocumentType = ASettingsData->FSearchDocumentType;
			FSearchLines->Assign(ASettingsData->FSearchLines);
			FReplaceLines->Assign(ASettingsData->FReplaceLines);

			FLoggingProperties->Assign(ASettingsData->FLoggingProperties);

			FConnectionTextAngle = ASettingsData->FConnectionTextAngle;
			FConnectionVertTextAlign = ASettingsData->FConnectionVertTextAlign;
			FConnectionHorizTextAlign = ASettingsData->FConnectionHorizTextAlign;
			FConnectionVertOffset = ASettingsData->FConnectionVertOffset;
			FConnectionHorizOffset = ASettingsData->FConnectionHorizOffset;
			FConnectionSelfDefaultHeight = ASettingsData->FConnectionSelfDefaultHeight;
			FConnectionAdditionalHighlight = ASettingsData->FConnectionAdditionalHighlight;
			FConnectionFromToShapesHighlight = ASettingsData->FConnectionFromToShapesHighlight;

			FEditorNodeTreeWidth = ASettingsData->FEditorNodeTreeWidth;

			FPropInspGutterWidth = ASettingsData->FPropInspGutterWidth;
			FPropInspInfoHeight = ASettingsData->FPropInspInfoHeight;

			FHintHidePause = ASettingsData->FHintHidePause;
			FHintPause = ASettingsData->FHintPause;
			FHintShortPause = ASettingsData->FHintShortPause;

			FAutoSaveCache = ASettingsData->FAutoSaveCache;

			FAutoCompleteItems->Assign(ASettingsData->FAutoCompleteItems);

			FExternScriptEditorFileName = ASettingsData->FExternScriptEditorFileName;
			FExternScriptEditorEnabled = ASettingsData->FExternScriptEditorEnabled;
			FExternScriptEditorExtension = ASettingsData->FExternScriptEditorExtension;
			FVirtualAliasVariable = ASettingsData->FVirtualAliasVariable;

			FHideWindows = ASettingsData->FHideWindows;

			FAutoLayoutSplines = ASettingsData->FAutoLayoutSplines;

			FTranslateEveryChangeToScxmlView = ASettingsData->FTranslateEveryChangeToScxmlView;
			FWarningOnInheritanceMismatch = ASettingsData->FWarningOnInheritanceMismatch;
			FWarningOnBindingsNotFound = ASettingsData->FWarningOnBindingsNotFound;
			FUseVectorImages = ASettingsData->FUseVectorImages;
			FCompletionTimeoutEnabled = ASettingsData->FCompletionTimeoutEnabled;
			FCompletionTimeoutMsec = ASettingsData->FCompletionTimeoutMsec;
			FValidateInPredicate = ASettingsData->FValidateInPredicate;
			FValidateChildren = ASettingsData->FValidateChildren;
			FValidateXMLText = ASettingsData->FValidateXMLText;
			FValidateEmptyTransitions = ASettingsData->FValidateEmptyTransitions;
			FCheckPositionOnLoadScxml = ASettingsData->FCheckPositionOnLoadScxml;
			FSaveCommentsAnywayOnLoad = ASettingsData->FSaveCommentsAnywayOnLoad;
			FParserDoNotLoadQtEditorInfo = ASettingsData->FParserDoNotLoadQtEditorInfo;
			FParserSaveQtTransitionOffsetMargin = ASettingsData->FParserSaveQtTransitionOffsetMargin;
			FChartFocusEnabled = ASettingsData->FChartFocusEnabled;
			FChartFocusLineWidth = ASettingsData->FChartFocusLineWidth;
			FChartFocusLineColor = ASettingsData->FChartFocusLineColor;
			FChartActiveStateColor = ASettingsData->FChartActiveStateColor;
			FChartTestCoverageColor = ASettingsData->FChartTestCoverageColor;

			FScxmlMsgTypes = ASettingsData->FScxmlMsgTypes;
			FSkipCommentsInRawScxml = ASettingsData->FSkipCommentsInRawScxml;
			FForceSaveDefaultScxmlValues = ASettingsData->FForceSaveDefaultScxmlValues;

			FTestingShowCallStack = ASettingsData->FTestingShowCallStack;
			FTestingDebugReceivedBuffer = ASettingsData->FTestingDebugReceivedBuffer;
			FTestingSwitchToPanel = ASettingsData->FTestingSwitchToPanel;
			FTestingScrollChartToViewShape = ASettingsData->FTestingScrollChartToViewShape;
			FTestingSendEventClickMode = ASettingsData->FTestingSendEventClickMode;
			FTestingAutoOpenUnit = ASettingsData->FTestingAutoOpenUnit;

			FLuautilsLuacExe = ASettingsData->FLuautilsLuacExe;
			FLuautilsBinaryConverter = ASettingsData->FLuautilsBinaryConverter;
			FLuautilsLuacSyntaxCheckParamsCMD = ASettingsData->FLuautilsLuacSyntaxCheckParamsCMD;

			FAviaExtensionEnabled = ASettingsData->FAviaExtensionEnabled;
			FProfilingEnabled = ASettingsData->FProfilingEnabled;
			FAutoLayoutUseNativeLabelPos = ASettingsData->FAutoLayoutUseNativeLabelPos;
			FAutoLayoutCheckOverlappedLabel = ASettingsData->FAutoLayoutCheckOverlappedLabel;

			FZoomHintsEnabled = ASettingsData->FZoomHintsEnabled;
			FZoomHintsStartLevel = ASettingsData->FZoomHintsStartLevel;
			FZoomHintsFontSizeEnlargePercents = ASettingsData->FZoomHintsFontSizeEnlargePercents;
			FZoomHintsShapeWidthMaxPercents = ASettingsData->FZoomHintsShapeWidthMaxPercents;

			FTempRegistry->Assign(ASettingsData->FTempRegistry);
			FRegistryItems->Assign(ASettingsData->FRegistryItems);

			FTestSettings->Assign(ASettingsData->FTestSettings);
			FExportSvgSettings->Assign(ASettingsData->FExportSvgSettings);
			FThemeSettings->Assign(ASettingsData->FThemeSettings);

			FConnectionInvertedConditionMacro->Assign(ASettingsData->FConnectionInvertedConditionMacro);

			FLockUI = ASettingsData->FLockUI;

			FTestApplicationPresets->Assign(ASettingsData->FTestApplicationPresets);

			/* вызывать в самом конце */
			this->AddDefaultTestingAppsCollectionItems();
			this->AddDefaultAutoCompleteCollectionItems();
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void TSettingsData::AddDefaultTestingAppsCollectionItems(void) {

	{
		const UnicodeString sPresetName = APP_TESTER_PRESET_USCXML;
		TTestApplicationPresetItem *ATesterItem = FTestApplicationPresets->GetItemByPresetName(sPresetName);
		if (!ATesterItem) {
			ATesterItem = FTestApplicationPresets->Add();
			ATesterItem->PresetName = sPresetName;
		}
		ATesterItem->TestSettings->ExeName = USCXML_TESTER_EXE;
		ATesterItem->TestSettings->Enabled = true;
	}

	{
		const UnicodeString sPresetName = APP_TESTER_PRESET_QT;
		TTestApplicationPresetItem *ATesterItem = FTestApplicationPresets->GetItemByPresetName(sPresetName);
		if (!ATesterItem) {
			ATesterItem = FTestApplicationPresets->Add();
			ATesterItem->PresetName = sPresetName;
		}
		ATesterItem->TestSettings->ExeName = QT_SCXML_TESTER_EXE;
		ATesterItem->TestSettings->WorkDir = L"$(UnitFileDir)";
		ATesterItem->TestSettings->Enabled = true;
	}

	{
		const UnicodeString sPresetName = APP_TESTER_PRESET_SCION;
		TTestApplicationPresetItem *ATesterItem = FTestApplicationPresets->GetItemByPresetName(sPresetName);
		if (!ATesterItem) {
			ATesterItem = FTestApplicationPresets->Add();
			ATesterItem->PresetName = sPresetName;
			ATesterItem->TestSettings->ExeName = SCION_SCXML_TESTER_EXE;
			ATesterItem->TestSettings->WorkDir = L"$(UnitFileDir)";
			ATesterItem->TestSettings->UseCustomParamsOnly = true;
			ATesterItem->TestSettings->CMDParams = L"run \"$(UnitFilePath)\"";
			ATesterItem->TestSettings->TriggersMsgFormat = tmfJSON;
			ATesterItem->TestSettings->TriggersProtocol = tpPipes;
			ATesterItem->TestSettings->ConvertOutputEncoding = coeUTF8;

			{
				TCaptureOutputRegexItem *AItem = ATesterItem->TestSettings->CaptureOutputRegExps->Add();
				AItem->RegExp = L"^entering state\\s(.*)$";
				AItem->GroupIdentifier = 1;
				AItem->CaptureMsgType = smttBeforeEnter;
			}

			{
				TCaptureOutputRegexItem *AItem = ATesterItem->TestSettings->CaptureOutputRegExps->Add();
				AItem->RegExp = L"^exiting state\\s(.*)$";
				AItem->GroupIdentifier = 1;
				AItem->CaptureMsgType = smttBeforeExit;
			}

			ATesterItem->TestSettings->CaptureOutputRegExps->Enabled = true;
		}
		ATesterItem->TestSettings->Enabled = true;
	}

}

// ---------------------------------------------------------------------------
void TSettingsData::AddDefaultAutoCompleteCollectionItems(void) {
	/* AutoCompleteItems */

	TAutoCompleteItem *ALuaItem = FAutoCompleteItems->IndexOfSyntaxScheme(L"Lua");
	if (!ALuaItem) {
		ALuaItem = FAutoCompleteItems->Add();
		ALuaItem->SyntaxScheme = L"Lua";
	}

	TAutoCompleteItem *AEcmaItem = FAutoCompleteItems->IndexOfSyntaxScheme(L"EcmaScript");
	if (!AEcmaItem) {
		AEcmaItem = FAutoCompleteItems->Add();
		AEcmaItem->SyntaxScheme = L"EcmaScript";
	}

	TAutoCompleteItem *ACppItem = FAutoCompleteItems->IndexOfSyntaxScheme(L"CPP");
	if (!ACppItem) {
		ACppItem = FAutoCompleteItems->Add();
		ACppItem->SyntaxScheme = L"CPP";
	}

	TAutoCompleteItem *APythonItem = FAutoCompleteItems->IndexOfSyntaxScheme(L"Python");
	if (!APythonItem) {
		APythonItem = FAutoCompleteItems->Add();
		APythonItem->SyntaxScheme = L"Python";
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsData::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();
		if (sPropName == L"LoggingProperties") {
			AEditorClass = __classid(TIniStringsClassEditor);
		}
		else if (sPropName == L"AutoCompleteItems") {
			AEditorClass = __classid(TAutoCompletePropEditor);
		}
		else if (sPropName == L"TestSettings") {
			AEditorClass = __classid(TTestSettingsPropEditor);
		}
		else if (sPropName == L"ConnectionInvertedConditionMacro") {
			AEditorClass = __classid(TStringsPairsEditor);
		}
		else if (sPropName == L"ExportSvgSettings") {
			AEditorClass = __classid(TExportSvgSettingsPropEditor);
		}
		else if (sPropName == L"ThemeSettings") {
			AEditorClass = __classid(TThemeSettingsPropEditor);
		}
		else if (sPropName == L"TestApplicationPresets") {
			AEditorClass = __classid(TTestApplicationPresetesPropEditor);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsData::PropSettingsRegisterCategories(TLMDPropertyInspector *APropSettingsInspector) {
	APropSettingsInspector->RegisterPropCategory(L"Common", L"AppPath");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"AppSettingsFile");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"AppLayoutDir");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"AppLayoutFile");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"AppLayoutDefault");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"SystemLanguage");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"AppVersion");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"RecentFiles");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"MaxRecentCount");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"HintHidePause");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"HintPause");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"HintShortPause");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"AutoSaveCache");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"AviaExtensionEnabled");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"LoggingProperties");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"ProfilingEnabled");
	APropSettingsInspector->RegisterPropCategory(L"Common", L"LockUI");

	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"EditorRecentFiles");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"HideWindows");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"EditorNodeTreeWidth");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"PropInspGutterWidth");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"PropInspInfoHeight");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"SkipCommentsInRawScxml");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"AutoCompleteItems");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"ActiveSyntaxScheme");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"SearchLines");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"ReplaceLines");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"WordSeparators");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"SearchTypes");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"SearchDocumentType");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"TranslateEveryChangeToScxmlView");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"ConnectionAdditionalHighlight");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"ConnectionFromToShapesHighlight");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"WarningOnInheritanceMismatch");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"WarningOnBindingsNotFound");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"UseVectorImages");
	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"CompletionTimeoutEnabled");
	//	APropSettingsInspector->RegisterPropCategory(L"Scxml editor", L"CompletionTimeoutMsec");

	APropSettingsInspector->RegisterPropCategory(L"Visual", L"ChartFocusEnabled");
	APropSettingsInspector->RegisterPropCategory(L"Visual", L"ChartFocusLineWidth");
	APropSettingsInspector->RegisterPropCategory(L"Visual", L"ChartFocusLineColor");
	APropSettingsInspector->RegisterPropCategory(L"Visual", L"ChartActiveStateColor");
	APropSettingsInspector->RegisterPropCategory(L"Visual", L"ChartTestCoverageColor");
	APropSettingsInspector->RegisterPropCategory(L"Visual", L"ThemeSettings");

	APropSettingsInspector->RegisterPropCategory(L"Scxml AutoLayout", L"AutoLayoutSplines");
	APropSettingsInspector->RegisterPropCategory(L"Scxml AutoLayout", L"AutoLayoutUseNativeLabelPos");
	APropSettingsInspector->RegisterPropCategory(L"Scxml AutoLayout", L"AutoLayoutCheckOverlappedLabel");

	APropSettingsInspector->RegisterPropCategory(L"Extern editor", L"ExternScriptEditorFileName");
	APropSettingsInspector->RegisterPropCategory(L"Extern editor", L"ExternScriptEditorExtension");
	APropSettingsInspector->RegisterPropCategory(L"Extern editor", L"ExternScriptEditorEnabled");

	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"CheckPositionOnLoadScxml");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"SaveCommentsAnywayOnLoad");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ParserDoNotLoadQtEditorInfo");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ParserSaveQtTransitionOffsetMargin");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ConnectionTextAngle");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ConnectionVertTextAlign");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ConnectionHorizTextAlign");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ConnectionVertOffset");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ConnectionHorizOffset");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ConnectionSelfDefaultHeight");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ConnectionInvertedConditionMacro");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ValidateInPredicate");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ValidateChildren");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ValidateXMLText");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ValidateEmptyTransitions");
	APropSettingsInspector->RegisterPropCategory(L"Scxml parser", L"ForceSaveDefaultScxmlValues");
	APropSettingsInspector->RegisterPropCategory(L"Extern editor", L"VirtualAliasVariable");

	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingShowCallStack");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingDebugReceivedBuffer");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"CallStackTypes");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingStopOnExit");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingSwitchToPanel");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingScrollChartToViewShape");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingAutoOpenUnit");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingSendEventClickMode");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestSettings");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestApplicationPresets");

	APropSettingsInspector->RegisterPropCategory(L"Lua utils", L"LuautilsLuacExe");
	APropSettingsInspector->RegisterPropCategory(L"Lua utils", L"LuautilsLuacSyntaxCheckParamsCMD");
	APropSettingsInspector->RegisterPropCategory(L"Lua utils", L"LuautilsBinaryConverter");

	APropSettingsInspector->RegisterPropCategory(L"Zoom", L"ZoomHintsEnabled");
	APropSettingsInspector->RegisterPropCategory(L"Zoom", L"ZoomHintsStartLevel");
	APropSettingsInspector->RegisterPropCategory(L"Zoom", L"ZoomHintsFontSizeEnlargePercents");

	APropSettingsInspector->RegisterPropCategory(L"SVG", L"ExportSvgSettings");

	/* DEPRECATED */
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingEnabled");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestExeName");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestWorkDir");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestCMDParams");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestUseCustomParamsOnly");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestRemoteHost");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestRemotePort");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestLocalPort");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingScxmlMonitorLog");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingStopOnExit");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestingCheckIssues");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestHttpEnabled");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestHttpListenPort");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestHttpWebSocket");
	APropSettingsInspector->RegisterPropCategory(L"Scxml tester", L"TestHideDebugInfo");
}

// ---------------------------------------------------------------------------
bool __fastcall TSettingsData::OnSerializeCustomGetProp(TObject *Sender, const UnicodeString &sPropName, _di_IXMLNode APropertyNode) {
	try {
		if (Sender->ClassType() == __classid(TSettingsData)) {
			if (SameStr(sPropName, L"TestingEnabled")) {
				this->TestSettings->Enabled = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestExeName")) {
				this->TestSettings->ExeName = APropertyNode->Text;
				return true;
			}
			if (SameStr(sPropName, L"TestWorkDir")) {
				this->TestSettings->WorkDir = APropertyNode->Text;
				return true;
			}
			if (SameStr(sPropName, L"TestCMDParams")) {
				this->TestSettings->CMDParams = APropertyNode->Text;
				return true;
			}
			if (SameStr(sPropName, L"TestUseCustomParamsOnly")) {
				this->TestSettings->UseCustomParamsOnly = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestRemoteHost")) {
				this->TestSettings->RemoteHost = APropertyNode->Text;
				return true;
			}
			if (SameStr(sPropName, L"TestRemotePort")) {
				this->TestSettings->RemotePort = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestLocalPort")) {
				this->TestSettings->LocalPort = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestingScxmlMonitorLog")) {
				this->TestSettings->ScxmlMonitorLog = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestingStopOnExit")) {
				this->TestSettings->StopOnExit = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestingCheckIssues")) {
				this->TestSettings->CheckIssues = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestHttpEnabled")) {
				this->TestSettings->HttpEnabled = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestHttpListenPort")) {
				this->TestSettings->HttpListenPort = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestHttpWebSocket")) {
				this->TestSettings->HttpWebSocket = APropertyNode->NodeValue;
				return true;
			}
			if (SameStr(sPropName, L"TestHideDebugInfo")) {
				this->TestSettings->HideDebugInfo = APropertyNode->NodeValue;
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
UnicodeString __fastcall TSettingsData::GetAppPath(void) {
	return ExtractFilePath(Application->ExeName);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetAppSettingsFile(void) {
	return TPath::Combine(AppPath, TPath::GetFileNameWithoutExtension(Application->ExeName) + L".settings.xml");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetAppLayoutDir(void) {
	return TPath::Combine(AppPath, L"Layouts");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetAppLayoutFile(void) {
	return TPath::Combine(AppLayoutDir, L"layout_currrent.xml");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetAppLayoutDefaultFile(void) {
	return TPath::Combine(AppLayoutDir, L"layout_default.xml");
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsData::AddRecentFileName(const UnicodeString &sFileName) {
	int iRecentIndex = -1;
	for (int i = 0; i < this->RecentFiles->Count; i++) {
		if (this->RecentFiles->Strings[i].UpperCase() == sFileName.UpperCase()) {
			iRecentIndex = i;
			break;
		}
	}
	if (iRecentIndex == -1) {
		this->RecentFiles->Insert(0, sFileName);
		for (int i = this->RecentFiles->Count - 1; i > FMaxRecentCount; i--) {
			this->RecentFiles->Delete(i);
		}
	}
	else {
		this->RecentFiles->Move(iRecentIndex, 0);
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TSettingsData::OnFilterPropEvent(const UnicodeString &sPropName) {
	std::auto_ptr<TStringList>AStringList(new TStringList());
	AStringList->CaseSensitive = true;
	AStringList->Add(L"Name");
	AStringList->Add(L"Tag");
	AStringList->Add(L"TempRegistry");
	AStringList->Add(L"TempRegistryItems");

	return AStringList->IndexOf(sPropName) == -1;
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsData::SetMaxRecentCount(int val) {
	if (val < 1 || val > 20)
		throw Exception(UnicodeString().sprintf(L"Value [%d] must be in range [1-20]", val));

	FMaxRecentCount = val;
}

// ---------------------------------------------------------------------------
bool __fastcall TSettingsData::EditStringsWithExternEditor(TStrings *AStrings, const UnicodeString &sFileCaption) {
	assert(AStrings != NULL);

	try {
		if (SettingsData->ExternScriptEditorEnabled) {

			const UnicodeString sTempFile = TPath::Combine(TPath::GetTempPathA(), sFileCaption + L".lua");
			AStrings->SaveToFile(sTempFile);

			SpawnInfinite(AnsiQuotedStr(SettingsData->ExternScriptEditorFileName, L'"') + L" " + AnsiQuotedStr(sTempFile, L'"'));

			AStrings->LoadFromFile(sTempFile);

			DeleteFileW(sTempFile.c_str());
			return true;
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	return false;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetExternScriptEditorExtension(void) {
	return FExternScriptEditorExtension.IsEmpty() ? L"lua" : FExternScriptEditorExtension.c_str();
}

// ---------------------------------------------------------------------------
bool __fastcall TSettingsData::IsExternScriptEditorExtensionStored(void) {
	return ExternScriptEditorExtension != L"lua";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetVirtualAliasVariable(void) {
	return FVirtualAliasVariable.IsEmpty() ? L"%1" : FVirtualAliasVariable.c_str();
}

// ---------------------------------------------------------------------------
bool __fastcall TSettingsData::IsVirtualAliasVariableStored(void) {
	return VirtualAliasVariable != L"%1";
}

// ---------------------------------------------------------------------------
void TSettingsData::SendEvent(const UnicodeString &sMsg) {
	if (IsTesterWorking) {
		switch(this->ActiveTestSettings->TriggersProtocol) {
		case TTriggersProtocol::
			tpPipes : {

				if (FormScxmlGui) {
					FormScxmlGui->SendPipeEvent(sMsg);
				}

			}break;
		default: {
				DModule->UDPClient1->Send(sMsg, TEncoding::UTF8);
			}break;
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetActiveSyntaxScheme(void) {
	if (FormScxmlGui) {
		TStateMachineDockPanel *AStateMachinePanel = FormScxmlGui->ActiveEditorDockPanel;
		if (AStateMachinePanel) {
			return AStateMachinePanel->StateMachineEditor->SyntaxScheme;
		}
	}
	return L"";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetWordSeparators(void) {
	TAutoCompleteItem *AItem = FAutoCompleteItems->IndexOfSyntaxScheme(ActiveSyntaxScheme);

	return AItem ? AItem->WordSeparators : TAutoCompleteItem::DefaultWordsSeparators;
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsData::FillCompletionList(Classes::TStrings* Items, const UnicodeString &LeftPart,
	const UnicodeString &sActiveSyntaxScheme) {

	const UnicodeString sLeftPartUpper = LeftPart.UpperCase();

	UnicodeString sAfterDot = L"";
	UnicodeString sBeforeDot = L"";
	int iPosDot = 0;

	const bool bIsEcma = SameText(sActiveSyntaxScheme, "ecmascript");
	std::auto_ptr<TStringList>ADotIgnoredListPtr(new TStringList());
	if (bIsEcma) {

		ADotIgnoredListPtr->Add("Math");
		ADotIgnoredListPtr->Add("JSON");
		ADotIgnoredListPtr->Add("Date");

		iPosDot = sLeftPartUpper.Pos(L".");
		if (iPosDot) {
			sAfterDot = sLeftPartUpper.SubString(iPosDot, sLeftPartUpper.Length() - iPosDot + 1);
			sBeforeDot = LeftPart.SubString(1, iPosDot - 1);
#if 0 // для глубокой отладки
			WLOG_DEBUG(L"After dot:[%s]", sAfterDot.c_str());
#endif
		}
	}

	TAutoCompleteItem *AItem = FAutoCompleteItems->IndexOfSyntaxScheme
		(sActiveSyntaxScheme.IsEmpty() ? ActiveSyntaxScheme : sActiveSyntaxScheme);
	if (AItem) {
		for (int i = 0; i < AItem->Default->Count; i++) {
			if (!AItem->Default->Strings[i].IsEmpty()) {

				const UnicodeString sUpperCaseItem = AItem->Default->Strings[i].UpperCase();

				const bool bIsItemDot = sUpperCaseItem[1] == L'.';

				if (bIsItemDot) {
					if (iPosDot > 1 && sUpperCaseItem.Pos(sAfterDot) == 1) {
						if (!bIsEcma || ADotIgnoredListPtr->IndexOf(sBeforeDot) == -1) {
							Items->Add(sBeforeDot + AItem->Default->Strings[i]);
						}
					}
				}
				else if (sLeftPartUpper.IsEmpty() || sUpperCaseItem.Pos(sLeftPartUpper) == 1) {
					Items->Add(AItem->Default->Strings[i]);
				}
			}

		}
		for (int i = 0; i < AItem->Custom->Count; i++) {
			if (!AItem->Default->Strings[i].IsEmpty()) {

				const UnicodeString sUpperCaseItem = AItem->Custom->Strings[i].UpperCase();

				const bool bIsItemDot = sUpperCaseItem[1] == L'.';

				if (bIsItemDot) {
					if (iPosDot > 1 && sUpperCaseItem.Pos(sAfterDot) == 1) {
						if (!bIsEcma || ADotIgnoredListPtr->IndexOf(sBeforeDot) == -1) {
							Items->Add(sBeforeDot + AItem->Custom->Strings[i]);
						}
					}
				}
				else if (sLeftPartUpper.IsEmpty() || sUpperCaseItem.Pos(sLeftPartUpper) == 1) {
					Items->Add(AItem->Custom->Strings[i]);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsData::Save() {
	this->SaveToFile(this->AppSettingsFile);
	WLOG_INFO(L"Successfully save settings <%s>", this->AppSettingsFile.c_str());
}

// ---------------------------------------------------------------------------
void TSettingsData::ProfilerReset(const UnicodeString &sCategory) {
	if (!FProfilingEnabled)
		return;

	FProfilerMap[sCategory] = Time();
}

// ---------------------------------------------------------------------------
void TSettingsData::ProfileOperation(const UnicodeString &sCategory, const UnicodeString &sMessage) {
	if (!FProfilingEnabled)
		return;

	std::map<UnicodeString, TDateTime>::iterator it = FProfilerMap.find(sCategory);
	if (it != FProfilerMap.end()) {
		TTime tDiff = Time() - it->second;
		if (sMessage.IsEmpty())
			WLOG_INFO(L"PROFILE:[%s] [%s] sec", sCategory.c_str(), tDiff.FormatString(L"hh:mm:ss.zzz"));
		else
			WLOG_INFO(L"PROFILE:[%s] [%s] [%s] sec", sCategory.c_str(), sMessage.c_str(), tDiff.FormatString(L"hh:mm:ss.zzz"));
	}
	else {
		WLOG_ERROR(L"Profile operation for category <%s> was not previously reseted!", sCategory.c_str());
	}
}

// ---------------------------------------------------------------------------
void TSettingsData::ProfileOperationReset(const UnicodeString &sCategory, const UnicodeString &sMessage) {
	if (!FProfilingEnabled)
		return;

	ProfileOperation(sCategory, sMessage);
	ProfilerReset(sCategory);
}

// ---------------------------------------------------------------------------
void TSettingsData::LoadComboBoxDataFromRegistry(TComboBox *AComboBox) {
	AComboBox->Items->CommaText = this->TempRegistry->Values[AComboBox->Owner->Name + L"." + AComboBox->Name + L".Items"];
	const int iItemIndex = this->TempRegistry->Values[AComboBox->Owner->Name + L"." + AComboBox->Name + L".Index"].ToIntDef(-1);
	if (iItemIndex >= 0 && iItemIndex < AComboBox->Items->Count) {
		AComboBox->ItemIndex = iItemIndex;
	}
}

// ---------------------------------------------------------------------------
bool TSettingsData::SaveComboBoxDataToRegistry(TComboBox *AComboBox) {
	bool bNeedToSave = false;
	if (AComboBox->Items->IndexOf(AComboBox->Text) == -1) {
		AComboBox->Items->Insert(0, AComboBox->Text);
		SettingsData->TempRegistry->Values[AComboBox->Owner->Name + L"." + AComboBox->Name + L".Items"] = AComboBox->Items->CommaText;
		bNeedToSave = true;
	}
	if (SettingsData->TempRegistry->Values[AComboBox->Owner->Name + L"." + AComboBox->Name + L".Index"].ToIntDef(-1)
		!= AComboBox->ItemIndex) {
		SettingsData->TempRegistry->Values[AComboBox->Owner->Name + L"." + AComboBox->Name + L".Index"] = AComboBox->ItemIndex;
		bNeedToSave = true;
	}
	return bNeedToSave;
}

// ---------------------------------------------------------------------------
void TSettingsData::LoadCheckBoxDataFromRegistry(TCheckBox *ACheckBox) {
	const UnicodeString sPath = ACheckBox->Owner->Name + L"." + ACheckBox->Name + L".Checked";
	ACheckBox->Checked = StrToBoolDef(this->TempRegistry->Values[sPath], ACheckBox->Checked);
}

// ---------------------------------------------------------------------------
void TSettingsData::SaveCheckBoxDataToRegistry(TCheckBox *ACheckBox) {
	this->TempRegistry->Values[ACheckBox->Owner->Name + L"." + ACheckBox->Name + L".Checked"] = BoolToStr(ACheckBox->Checked, true);
}

// ---------------------------------------------------------------------------
TEditorSearchTypes __fastcall TSettingsData::GetDefaultSearchTypes(void) {
	return TEditorSearchTypes() << estStateText << estTransitionText << estAllProperties << estTrim;
}

// ---------------------------------------------------------------------------
bool __fastcall TSettingsData::IsSearchTypesStored(void) {
	return FSearchTypes != GetDefaultSearchTypes();
}

// ---------------------------------------------------------------------------
TTestSettings *__fastcall TSettingsData::GetActiveTestSettings(void) {
	if (FormScxmlGui) {
		if (FormScxmlGui->IsTestingBasedOnProjectSettings())
			return FormScxmlGui->StateMachineProject->ProjectProperties->TestSettings;
	}

	return this->TestSettings;
}

// ---------------------------------------------------------------------------
bool __fastcall TSettingsData::IsTestCoverageEnabled(void) {
	if (FormScxmlGui) {
		return FormScxmlGui->actTestCoverage->Checked;
	}
	return false;
}

// ---------------------------------------------------------------------------
bool __fastcall TSettingsData::IsTesterWorkingInDebugMode(void) {
	if (FormScxmlGui) {
		return IsTesterWorking && !FormScxmlGui->actExternTesting->Checked;
	}
	return false;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TSettingsData::GetCacheDir(void) {
	return TPath::Combine(TPath::GetTempPath(), L"ScxmlEditor");
}

// ---------------------------------------------------------------------------
