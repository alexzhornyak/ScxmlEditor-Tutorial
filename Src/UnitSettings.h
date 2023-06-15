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

#ifndef UnitSettingsH
#define UnitSettingsH

#include <XMLDoc.hpp>
#include <TeeTree.hpp>
#include <TreeEdNew.hpp>

#include "LMDDsgPropInsp.hpp"

#include "UnitSettingsBase.h"
#include "ScxmlMsgTypes.h"
#include "CommonConsts.h"
#include "UnitSpawnAndRedirect.h"
#include "UnitCompleteItems.h"
#include "UnitTestSettings.h"
#include "UnitSettingsBinaryItems.h"
#include "UnitSVGSettings.h"
#include "UnitTestingApplicationsPresets.h"
#include "UnitThemeSettings.h"
//---------------------------------------------------------------------------

#define DEFAULT_HIDE_WINDOWS_STATUS		(TTreeEditWindows() << teInspector << teFont << teFormat)

typedef enum { ctLocal, ctGlobal } TClipboardType;

typedef std::map <UnicodeString /*connection name*/,
					std::pair< bool/*Self-Connection*/, UnicodeString /*serialized class*/> > TMapConnections;

typedef enum { alsOrtho, alsPolyline } TAutoLayoutSpline;

typedef enum { seNone, seDoubleClick, seClick } TTestSendEventMode;

class TSettingsData: public TSettingsBase
{
__published:	// IDE-managed Components

	/// это событие вызывается для проверки фильтрации свойств
	bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);

	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);

	/// method to support ScxmlEditor 1.0 settings
	bool __fastcall OnSerializeCustomGetProp(TObject *Sender, const UnicodeString &sPropName, _di_IXMLNode APropertyNode);

private:	// User declarations

	TStrings * FRecentFiles;
	inline void __fastcall SetRecentFiles(TStrings *val) { FRecentFiles->Assign(val); }
	TStrings * FEditorRecentFiles;
	inline void __fastcall SetEditorRecentFiles(TStrings *val) { FEditorRecentFiles->Assign(val); }
	int FMaxRecentCount;

	TTestSettings *	FTestSettings;
	inline void __fastcall SetTestSettings(TTestSettings *val) { FTestSettings->Assign(val); }

	TTestApplicationPresetItems * FTestApplicationPresets;
	inline void __fastcall SetTestApplicationPresets(TTestApplicationPresetItems *val) {
		FTestApplicationPresets->Assign(val); this->AddDefaultTestingAppsCollectionItems(); }

	TExportSvgSettings *	FExportSvgSettings;
	inline void __fastcall SetExportSvgSettings(TExportSvgSettings *val) { FExportSvgSettings->Assign(val); }

	TThemeSettings *	FThemeSettings;
	inline void __fastcall SetThemeSettings(TThemeSettings *val) { FThemeSettings->Assign(val); }

	TEditorSearchTypes FSearchTypes;
	TEditorSearchDocumentType FSearchDocumentType;

    TStrings * FTempRegistry;

	TSettingsRegistryItems *   FRegistryItems;
	inline void __fastcall SetSettingsRegistryItems(TSettingsRegistryItems *val) { FRegistryItems->Assign(val); }

	TStrings *FSearchLines;
	void __fastcall SetSearchLines(TStrings *val) { FSearchLines->Assign(val); }

	TStrings *FReplaceLines;
	void __fastcall SetReplaceLines(TStrings *val) { FReplaceLines->Assign(val); }

	bool FCheckPositionOnLoadScxml;
	bool FSaveCommentsAnywayOnLoad;
	bool FParserDoNotLoadQtEditorInfo;

	int 			FConnectionTextAngle;
	TVertTextAlign 	FConnectionVertTextAlign;
	THorizTextAlign FConnectionHorizTextAlign;
	int 			FConnectionVertOffset;
	int 			FConnectionHorizOffset;
	int				FConnectionSelfDefaultHeight;

	int				FEditorNodeTreeWidth;
	int			 	FPropInspGutterWidth;
	int				FPropInspInfoHeight;

	UnicodeString FSystemLanguage;
	UnicodeString FAppVersion;

	int FHintHidePause;
	int FHintPause;
	int FHintShortPause;

	bool FAutoSaveCache;

	bool FZoomHintsEnabled;
	int FZoomHintsStartLevel;
	int FZoomHintsFontSizeEnlargePercents;
	int FZoomHintsShapeWidthMaxPercents;

	TAutoCompleteItems *   FAutoCompleteItems;
	inline void __fastcall SetAutoCompleteItems(TAutoCompleteItems *val) {
		FAutoCompleteItems->Assign(val); this->AddDefaultAutoCompleteCollectionItems(); }

	UnicodeString __fastcall GetActiveSyntaxScheme(void);
	UnicodeString __fastcall GetWordSeparators(void);

	TStrings * FLoggingProperties;
	UnicodeString FDefaultLoggingProperties;
	inline void __fastcall SetLoggingProperties(TStrings *val) { FLoggingProperties->Assign(val); }
	inline bool __fastcall IsLoggingPropertiesStored(void) { return FLoggingProperties->Text.Trim() != FDefaultLoggingProperties; }

	UnicodeString 	FExternScriptEditorFileName;
	bool 			FExternScriptEditorEnabled;
	UnicodeString	FExternScriptEditorExtension;
	UnicodeString	__fastcall GetExternScriptEditorExtension(void);
	bool 			__fastcall IsExternScriptEditorExtensionStored(void);

	UnicodeString	FVirtualAliasVariable;
	UnicodeString	__fastcall GetVirtualAliasVariable(void);
	bool 			__fastcall IsVirtualAliasVariableStored(void);

	TTreeEditWindows FHideWindows;
	inline bool __fastcall IsHideWindowsStored(void) { return FHideWindows != DEFAULT_HIDE_WINDOWS_STATUS; }

	bool FTestingShowCallStack;
	bool FTestingDebugReceivedBuffer;
	bool FTestingSwitchToPanel;
	bool FTestingScrollChartToViewShape;
	TTestSendEventMode FTestingSendEventClickMode;
	bool FTestingAutoOpenUnit;

	bool FTranslateEveryChangeToScxmlView;

	bool FWarningOnInheritanceMismatch;
	bool FWarningOnBindingsNotFound;

	bool FUseVectorImages;

	bool FValidateInPredicate;
	bool FValidateChildren;
	bool FValidateXMLText;
	bool FValidateEmptyTransitions;

	UnicodeString FLuautilsLuacExe;
	inline UnicodeString __fastcall GetLuautilsLuacExe(void) { return FLuautilsLuacExe.IsEmpty() ? UnicodeString(L"luac53.exe") : FLuautilsLuacExe; }
	inline bool __fastcall IsLuautilsLuacExeStored(void) { return !SameText(LuautilsLuacExe, L"luac53.exe"); }

	UnicodeString FLuautilsBinaryConverter;
	inline UnicodeString __fastcall GetLuautilsBinaryConverter(void) { return FLuautilsBinaryConverter.IsEmpty() ? UnicodeString(L"LuaBin2c.exe") : FLuautilsBinaryConverter; }
	inline bool __fastcall IsLuautilsBinaryConverterStored(void) { return !SameText(LuautilsBinaryConverter,L"LuaBin2c.exe"); }

	UnicodeString FLuautilsLuacSyntaxCheckParamsCMD;
	inline UnicodeString __fastcall GetLuautilsLuacSyntaxCheckParamsCMD(void) { return FLuautilsLuacSyntaxCheckParamsCMD.IsEmpty() ? UnicodeString(L"-s -v -p") : FLuautilsLuacSyntaxCheckParamsCMD; }
	inline bool __fastcall IsLuautilsLuacSyntaxCheckParamsCMDStored(void) { return LuautilsLuacSyntaxCheckParamsCMD != L"-s -v -p"; }

	TScxmlMsgTypes FScxmlMsgTypes;
	inline bool __fastcall IsScxmlMsgTypesStored(void) { return FScxmlMsgTypes!=DEFAULT_SETTINGS_SCXML_MSG_TYPES; }

    bool FSkipCommentsInRawScxml;
	bool FForceSaveDefaultScxmlValues;

	bool FAviaExtensionEnabled;
	bool FProfilingEnabled;

	bool FConnectionAdditionalHighlight;
	bool FConnectionFromToShapesHighlight;

	std::map<UnicodeString, TDateTime> FProfilerMap;

	bool FLockUI;

	TStringList *FConnectionInvertedConditionMacro;
	TStringList *FDefaultConnectionInvertedConditionMacro;
	inline TStrings * __fastcall GetConnectionInvertedConditionMacro(void) {
		return FConnectionInvertedConditionMacro->Text.Trim().IsEmpty() ? FDefaultConnectionInvertedConditionMacro : FConnectionInvertedConditionMacro; }
	inline void __fastcall SetConnectionInvertedConditionMacro(TStrings *val) { FConnectionInvertedConditionMacro->Assign(val); }
	inline bool __fastcall IsConnectionInvertedConditionMacro(void) { return !FConnectionInvertedConditionMacro->Text.Trim().IsEmpty(); }

	TAutoLayoutSpline FAutoLayoutSplines;
	bool FAutoLayoutUseNativeLabelPos;
	bool FAutoLayoutCheckOverlappedLabel;

	bool __fastcall IsSearchTypesStored(void);
	TEditorSearchTypes __fastcall GetDefaultSearchTypes(void);

	TTestSettings *__fastcall GetActiveTestSettings(void);

	void AddDefaultTestingAppsCollectionItems(void);
	void AddDefaultAutoCompleteCollectionItems(void);

	bool	FChartFocusEnabled;
	int		FChartFocusLineWidth;
	TColor	FChartFocusLineColor;
	TColor	FChartActiveStateColor;
	TColor	FChartTestCoverageColor;

protected:

	virtual void __fastcall SetMaxRecentCount(int val);

	virtual UnicodeString __fastcall GetAppPath(void);
	virtual UnicodeString __fastcall GetAppSettingsFile(void);
	virtual UnicodeString __fastcall GetAppLayoutFile(void);
	virtual UnicodeString __fastcall GetAppLayoutDefaultFile(void);
	virtual UnicodeString __fastcall GetAppLayoutDir(void);

public:		// User declarations

	__fastcall TSettingsData(TComponent* Owner);

	virtual __fastcall ~TSettingsData();

	virtual void __fastcall Assign(TPersistent* Source);

	// добавить имя файла в недавние
	void __fastcall AddRecentFileName(const UnicodeString &sFileName);

	// функция должна создать новый экземпляр текущего класса
	virtual TComponent *__fastcall DoCreateDefaultInstance();

	bool __fastcall EditStringsWithExternEditor(TStrings *AStrings, const UnicodeString &sFileCaption);

	void SendEvent(const UnicodeString &sMsg);

	bool __fastcall IsTestCoverageEnabled(void);

	/* Tester can send messages to testing applications */
	bool __fastcall IsTesterWorkingInDebugMode(void);

	void __fastcall FillCompletionList(Classes::TStrings* Items, const UnicodeString &LeftPart, const UnicodeString &sActiveSyntaxScheme);

	void __fastcall PropSettingsRegisterCategories(TLMDPropertyInspector *APropSettingsInspector);

	// writes properties to 'Settings->AppSettingsFile'
	void __fastcall Save();

	void ProfilerReset(const UnicodeString &sCategory);

	void ProfileOperation(const UnicodeString &sCategory, const UnicodeString &sMessage = L"");

	void ProfileOperationReset(const UnicodeString &sCategory, const UnicodeString &sMessage = L"");

	void LoadComboBoxDataFromRegistry(TComboBox *AComboBox);
	bool SaveComboBoxDataToRegistry(TComboBox *AComboBox);

	void LoadCheckBoxDataFromRegistry(TCheckBox *ACheckBox);
	void SaveCheckBoxDataToRegistry(TCheckBox *ACheckBox);

	bool IsTesterWorking;

	TMapConnections ClipboardConnectionsMap;

	// Switchable props: 1) Global, 2) local
	// P.S. Do not move to __published !!!
	__property TTestSettings *ActiveTestSettings = { read=GetActiveTestSettings };

	UnicodeString __fastcall GetCacheDir(void);

__published:	// PROPERTIES THAT ARE SEEN IN SETTINGS EDITOR

	/********************************************************************/
	/* DO NOT FORGET TO ADD NEW PROPERTIES TO ASSIGN AND CATEGORIES !!! */
	/********************************************************************/

	// common
	__property UnicodeString AppPath = {read=GetAppPath,stored=false};
	__property UnicodeString AppSettingsFile = {read=GetAppSettingsFile,stored=false};
	__property UnicodeString AppLayoutDir = {read=GetAppLayoutDir, stored=false};
	__property UnicodeString AppLayoutFile = {read=GetAppLayoutFile,stored=false};
	__property UnicodeString AppLayoutDefault = {read=GetAppLayoutDefaultFile,stored=false};

	__property UnicodeString SystemLanguage = {read=FSystemLanguage};
	__property UnicodeString AppVersion = {read=FAppVersion};

	__property int HintHidePause = {read=FHintHidePause, write=FHintHidePause, default=2500};
	__property int HintPause = {read=FHintPause, write=FHintPause, default=50};
	__property int HintShortPause = {read=FHintShortPause, write=FHintShortPause, default=0};

	__property bool AutoSaveCache = {read=FAutoSaveCache, write=FAutoSaveCache, default=true};

	__property TStrings * TempRegistry = {read=FTempRegistry, write=FTempRegistry}; // для всяких временных свойств НЕ ВИДЕН В РЕДАКТОРЕ!!!
	__property TSettingsRegistryItems * TempRegistryItems = {read=FRegistryItems, write=FRegistryItems}; // для всяких временных свойств НЕ ВИДЕН В РЕДАКТОРЕ!!!

	__property TEditorSearchTypes SearchTypes = {read=FSearchTypes, write=FSearchTypes, stored=IsSearchTypesStored};
	__property TEditorSearchDocumentType SearchDocumentType = {read=FSearchDocumentType, write=FSearchDocumentType, default=esdtCurrentDoc};
	__property TStrings * SearchLines = {read=FSearchLines, write=SetSearchLines};
	__property TStrings * ReplaceLines = {read=FReplaceLines, write=SetReplaceLines};

	__property TStrings * LoggingProperties = {read=FLoggingProperties, write=SetLoggingProperties, stored=IsLoggingPropertiesStored};

	__property TStrings * RecentFiles = {read=FRecentFiles, write=SetRecentFiles};
	__property TStrings * EditorRecentFiles = {read=FEditorRecentFiles, write=SetEditorRecentFiles};
	__property int MaxRecentCount = {read=FMaxRecentCount, write=SetMaxRecentCount,default=15};

	__property bool CheckPositionOnLoadScxml = {read=FCheckPositionOnLoadScxml, write=FCheckPositionOnLoadScxml, default=true};
	__property bool SaveCommentsAnywayOnLoad = {read=FSaveCommentsAnywayOnLoad, write=FSaveCommentsAnywayOnLoad, default=false};
	__property bool ParserDoNotLoadQtEditorInfo = {read=FParserDoNotLoadQtEditorInfo, write=FParserDoNotLoadQtEditorInfo, default=true};
	__property UnicodeString 	VirtualAliasVariable = {read=GetVirtualAliasVariable,write=FVirtualAliasVariable, stored=IsVirtualAliasVariableStored};

	__property bool 			AviaExtensionEnabled = {read=FAviaExtensionEnabled, write=FAviaExtensionEnabled, default=false};
	__property bool 			ProfilingEnabled = {read=FProfilingEnabled, write=FProfilingEnabled, default=false};

	__property int 				ConnectionTextAngle = {read=FConnectionTextAngle, write=FConnectionTextAngle, default=90};
	__property TVertTextAlign 	ConnectionVertTextAlign = {read=FConnectionVertTextAlign, write=FConnectionVertTextAlign, default=TVertTextAlign::vtaTop};
	__property THorizTextAlign 	ConnectionHorizTextAlign = {read=FConnectionHorizTextAlign, write=FConnectionHorizTextAlign, default=THorizTextAlign::htaCenter};
	__property int 				ConnectionVertOffset = {read=FConnectionVertOffset, write=FConnectionVertOffset, default=20};
	__property int 				ConnectionHorizOffset = {read=FConnectionHorizOffset, write=FConnectionHorizOffset, default=0};
	__property int				ConnectionSelfDefaultHeight = {read=FConnectionSelfDefaultHeight, write=FConnectionSelfDefaultHeight, default=30};
	__property bool				ConnectionAdditionalHighlight = {read=FConnectionAdditionalHighlight, write=FConnectionAdditionalHighlight, default=true};
	__property bool				ConnectionFromToShapesHighlight = {read=FConnectionFromToShapesHighlight, write=FConnectionFromToShapesHighlight, default=true};
	__property TStrings *		ConnectionInvertedConditionMacro = {read=GetConnectionInvertedConditionMacro, write=SetConnectionInvertedConditionMacro, stored=IsConnectionInvertedConditionMacro};
	__property bool				ValidateInPredicate = {read=FValidateInPredicate, write=FValidateInPredicate, default=false};
	__property bool				ValidateChildren = {read=FValidateChildren, write=FValidateChildren, default=true};
	__property bool				ValidateXMLText = {read=FValidateXMLText, write=FValidateXMLText, default=true};
	__property bool				ValidateEmptyTransitions = {read=FValidateEmptyTransitions, write=FValidateEmptyTransitions, default=true};
	__property bool 			ForceSaveDefaultScxmlValues = {read=FForceSaveDefaultScxmlValues, write=FForceSaveDefaultScxmlValues, default=false};

	__property bool				ZoomHintsEnabled = {read=FZoomHintsEnabled, write=FZoomHintsEnabled, default=true};
	__property int				ZoomHintsStartLevel = {read=FZoomHintsStartLevel, write=FZoomHintsStartLevel, default=90};
	__property int				ZoomHintsFontSizeEnlargePercents = {read=FZoomHintsFontSizeEnlargePercents, write=FZoomHintsFontSizeEnlargePercents, default=120};

	__property bool 			LockUI = {read=FLockUI, write=FLockUI, default=false};

	// editor
	__property TTreeEditWindows HideWindows={read=FHideWindows,write=FHideWindows, stored=IsHideWindowsStored };
	__property int		       	EditorNodeTreeWidth = {read=FEditorNodeTreeWidth,write=FEditorNodeTreeWidth,default=160};
	__property int			 	PropInspGutterWidth = {read=FPropInspGutterWidth, write=FPropInspGutterWidth, default=150};
	__property int				PropInspInfoHeight = {read=FPropInspInfoHeight, write=FPropInspInfoHeight, default=100};
	__property bool 			SkipCommentsInRawScxml = {read=FSkipCommentsInRawScxml, write=FSkipCommentsInRawScxml, default=true};
	__property bool				TranslateEveryChangeToScxmlView = {read=FTranslateEveryChangeToScxmlView, write=FTranslateEveryChangeToScxmlView, default=false};
	__property bool				WarningOnInheritanceMismatch = {read=FWarningOnInheritanceMismatch, write=FWarningOnInheritanceMismatch, default=true};
	__property bool				WarningOnBindingsNotFound = {read=FWarningOnBindingsNotFound, write=FWarningOnBindingsNotFound, default=true};
	__property bool				ChartFocusEnabled = {read=FChartFocusEnabled, write=FChartFocusEnabled, default=true};
	__property int				ChartFocusLineWidth = {read=FChartFocusLineWidth, write=FChartFocusLineWidth, default=2};
	__property TColor			ChartFocusLineColor = {read=FChartFocusLineColor, write=FChartFocusLineColor, default=clHighlight};
	__property TColor			ChartActiveStateColor = {read=FChartActiveStateColor, write=FChartActiveStateColor, default=Graphics::clDefault};
	__property TColor			ChartTestCoverageColor = {read=FChartTestCoverageColor, write=FChartTestCoverageColor, default=Graphics::clPurple};

	__property bool				UseVectorImages = {read=FUseVectorImages, write=FUseVectorImages, default=false};

	__property TAutoLayoutSpline		AutoLayoutSplines = {read=FAutoLayoutSplines, write=FAutoLayoutSplines, default=alsOrtho};
	__property bool 					AutoLayoutUseNativeLabelPos = {read=FAutoLayoutUseNativeLabelPos, write=FAutoLayoutUseNativeLabelPos, default=false};
	__property bool 					AutoLayoutCheckOverlappedLabel = {read=FAutoLayoutCheckOverlappedLabel, write=FAutoLayoutCheckOverlappedLabel, default=true};

	// completion
	__property TAutoCompleteItems *		AutoCompleteItems = {read=FAutoCompleteItems, write=SetAutoCompleteItems };
	__property UnicodeString			ActiveSyntaxScheme = {read=GetActiveSyntaxScheme};
	__property UnicodeString			WordSeparators = {read=GetWordSeparators};


	// extern editor
	__property UnicodeString 	ExternScriptEditorFileName = {read=FExternScriptEditorFileName,write=FExternScriptEditorFileName};
	__property UnicodeString 	ExternScriptEditorExtension = {read=GetExternScriptEditorExtension,write=FExternScriptEditorExtension,
																									stored=IsExternScriptEditorExtensionStored};
	__property bool		     	ExternScriptEditorEnabled = {read=FExternScriptEditorEnabled,write=FExternScriptEditorEnabled,default=false};

	// lua utils
	__property UnicodeString 	LuautilsLuacExe	= {read=GetLuautilsLuacExe, write=FLuautilsLuacExe, stored=IsLuautilsLuacExeStored};
	__property UnicodeString 	LuautilsLuacSyntaxCheckParamsCMD = {read=GetLuautilsLuacSyntaxCheckParamsCMD, write=FLuautilsLuacSyntaxCheckParamsCMD, stored=IsLuautilsLuacSyntaxCheckParamsCMDStored};
	__property UnicodeString 	LuautilsBinaryConverter	= {read=GetLuautilsBinaryConverter, write=FLuautilsBinaryConverter, stored=IsLuautilsBinaryConverterStored};

	__property TThemeSettings *	ThemeSettings = {read=FThemeSettings, write=SetThemeSettings};

	// SVG
	__property TExportSvgSettings *	ExportSvgSettings = {read=FExportSvgSettings, write=SetExportSvgSettings};

	// testing
	__property TTestSettings *	TestSettings = {read=FTestSettings, write=SetTestSettings};
	__property TTestApplicationPresetItems *	TestApplicationPresets = {read=FTestApplicationPresets, write=SetTestApplicationPresets};

	__property bool 				TestingDebugReceivedBuffer = {read=FTestingDebugReceivedBuffer, write=FTestingDebugReceivedBuffer, default=false};
	__property bool 				TestingShowCallStack = {read=FTestingShowCallStack, write=FTestingShowCallStack, default=true};
	__property bool					TestingSwitchToPanel = {read=FTestingSwitchToPanel, write=FTestingSwitchToPanel, default=true};
	__property bool					TestingAutoOpenUnit = {read=FTestingAutoOpenUnit, write=FTestingAutoOpenUnit, default=true};
	__property bool					TestingScrollChartToViewShape = {read=FTestingScrollChartToViewShape, write=FTestingScrollChartToViewShape, default=true};
	__property TTestSendEventMode	TestingSendEventClickMode = {read=FTestingSendEventClickMode, write=FTestingSendEventClickMode, default=seDoubleClick};
	__property TScxmlMsgTypes 		CallStackTypes = {read=FScxmlMsgTypes, write=FScxmlMsgTypes,stored=IsScxmlMsgTypesStored};


};

//---------------------------------------------------------------------------
extern PACKAGE TSettingsData *SettingsData;

#endif
