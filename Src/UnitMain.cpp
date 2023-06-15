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
// header
#include "UnitMain.h"
// STL
#include <memory>
// boost
#include <boost/regex.hpp>
#include <boost/scoped_array.hpp>
// WinApi
#include "ole2.h"
#include <Shlobj.h>
// Borland
#include <IOUtils.hpp>
#include <StrUtils.hpp>
#include <DateUtils.hpp>
/* https://github.com/orocos-toolchain/log4cpp */
/* LGPL-2.1 */
#include "Log4cpp_VCL.hpp"
// local
#include "XMLSerialize/UnitXMLSerializeLoad.h"
#include "UnitPropertyConfiguratorEx.h"
#include "UnitSettings.h"
#include "UnitCustomUtils.h"
#include "UnitStateMachineParser.h"
#include "UnitStateMachineUnit.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitSettingsEditor.h"
#include "UnitStateMachineConnection.h"
#include "UnitSearchForm.h"
#include "UnitScxmlShape.h"
#include "UnitTreeEx.h"
#include "UnitSpawnAndRedirect.h"
#include "SaveSimplePersistent.h"
#include "UnitSimpleHelp.h"
#include "UnitSyntaxScxmlXMLView.h"
#include "UnitSyntaxTextEditor.h"
#include "UnitDialogQScxmlComiler.h"
#include "UnitHotKeysHelp.h"
#include "WildCardUtils.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitMacroComposer.h"
#include "UnitFrameSearchResults.h"
#include "UnitFrameGifMaker.h"
#include "UnitStateShape.h"
#include "UnitDialogWait.h"
#include "UnitTreeEditorUtils.h"
#include "UnitDialogFileNotFound.h"
#include "UnitTriggerShape.h"
#include "UnitWatchShape.h"
#include "UnitDialogRegExp.h"
#include "UnitTransitionXMLEditor.h"
#include "CursorConsts.h"
#include "TrayIconErrorPriority.h"
#include "UnitIdeInsight.h"
#include "UnitGraphViz.h"
#include "UrlUtils.h"
#include "UnitTransitionEditor.h"
#include "UnitTreeEditorForTransition.h"
#include "UnitDialogRegisterScxmlUnitsAndProjects.h"
#include "UnitCreateTriggers.h"
#include "UnitMaskedTextShape.h"
#include "UnitDModule.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TeeProcs"
#pragma link "TeeTree"
#pragma link "ElListBox"
#pragma link "ElXPThemedControl"
#pragma link "HTMLLbx"
#pragma resource "*.dfm"
TFormScxmlGui *FormScxmlGui = 0;

// ----------------------- FORWARD DECLARATIONS -------------------------------
void __fastcall DoAddAllExistingInvokeSrc(TLMDProjectManager * AProjectManager, TStateMachineEditorUnit * AUnit);

// ---------------------------------------------------------------------------
// ------------------------------ AccessDockPanel ----------------------------
// ---------------------------------------------------------------------------
void __fastcall AccessDockPanel::SetMouseActivateEvent(const TMouseActivateEvent &AOnMouseActivate) {
	this->OnMouseActivate = AOnMouseActivate;
}

void __fastcall AccessDockPanel::GetFixAdjustClientRect(TRect &ARect) {
	this->AdjustClientRect(ARect);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void __fastcall CheckForNewDockPanels(TComponent *AOwner, TLMDDockPanel *ARootPanel, TLMDDockSite *ARootSite) {

	TLMDDockZone *AZone = ARootPanel->PanelVisible ? ARootPanel->Zone : ARootSite->RootZone;

	for (int i = 0; i < AOwner->ComponentCount; i++) {
		if (AOwner->Components[i]->InheritsFrom(__classid(TLMDDockPanel))) {
			TLMDDockPanel *ADockPanel = dynamic_cast<TLMDDockPanel*>(AOwner->Components[i]);
			if (ADockPanel) {
				if (!ADockPanel->Parent) {
					ARootSite->DockControl(ADockPanel, AZone, TAlign::alClient);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall RecursiveCloseUnit(Lmdideprojmgr::TLMDProjDocument *ADocument) {
	if (ADocument) {

		if (ADocument->Opened) {

			if (DialogWait) {
				DialogWait->ShowDialog(UnicodeString().sprintf(L"Closing [%s] ...", ADocument->DisplayName.c_str()));
			}

			ADocument->Close();

			if (ADocument->Opened)
				return false;
		}

		// перебираем с конца, так как некоторые документы при закрытии удаляются !!!
		for (int i = ADocument->DocumentCount - 1; i >= 0; i--) {
			if (!RecursiveCloseUnit(ADocument->Documents[i]))
				return false;
		}
	}
	return true;
}

// штатная функция некорректно работает, поэтому подменяем своей
bool IsAnyDocumentModified(TLMDProjDocument *ARoot) {
	if (ARoot) {
		if (ARoot->NeedSave || ARoot->IsModified()) {
			return true;
		}
		for (int i = 0; i < ARoot->DocumentCount; i++) {
			if (IsAnyDocumentModified(ARoot->Documents[i])) {
				return true;
			}
		}
	}
	return false;
}

// записываем все файлы проекта
void RecursiveSaveAll(TLMDProjectManager *AProjectManager, TLMDProjDocument *ARoot) {
	if (ARoot) {
		for (int i = 0; i < ARoot->DocumentCount; i++) {
			RecursiveSaveAll(AProjectManager, ARoot->Documents[i]);
		}
		// записываем только открытые документы
		const bool bSaveTop = AProjectManager->Root == ARoot ? true : (ARoot->OpenedDocument && ARoot->OpenedDocument->Opened);
		if (bSaveTop && (ARoot->NeedSave || ARoot->IsModified(false))) {
			AProjectManager->Save(ARoot, false);
		}
	}
}

// ---------------------------------------------------------------------------
TLMDProjDocument * RecursiveSearchOpened(TLMDProjDocument *ARoot, const UnicodeString &sFileName) {
	if (ARoot) {
		const UnicodeString sRootPath = ExtractFilePath(ARoot->FilePath);

		TLMDProjDocument *AExamForm = dynamic_cast<TLMDProjDocument*>(ARoot);
		if (AExamForm) {

			const UnicodeString sExamFileName = TPath::GetFullPath(AExamForm->FilePath);

			if (SameText(sExamFileName, sFileName)) {
				return AExamForm;
			}
		}

		for (int i = 0; i < ARoot->DocumentCount; i++) {
			TLMDProjDocument *AExamForm = RecursiveSearchOpened(ARoot->Documents[i], sFileName);
			if (AExamForm) {
				return AExamForm;
			}
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall IterateInvokeSrcNode(TLMDProjectManager *AProjectManager, TStateMachineEditorUnit *AUnit, _di_IXMLNode ANode) {
	if (AUnit && ANode && ANode->NodeType == ntElement) {
		if (ANode->NodeName == L"invoke" && ANode->HasAttribute(L"src")) {
			try {
				const UnicodeString sSrc = Url::UrlToLocalFile(ANode->GetAttribute(L"src"));
				const UnicodeString sInvokeSrcFile = TPath::IsPathRooted(sSrc) ? sSrc : //
				TPath::Combine(ExtractFilePath(AUnit->FilePath), sSrc);

				if (SameText(sInvokeSrcFile, AUnit->FilePath))
					throw Exception("Can not add invoke src <" + sInvokeSrcFile + "> to itself!");

				if (!FileExists(sInvokeSrcFile))
					throw Exception("File <" + sInvokeSrcFile + "> does not exist!");

				const UnicodeString sGivenExtension = ExtractFileExt(sInvokeSrcFile);
				if (!SameText(sGivenExtension, ".scxml"))
					throw Exception("Extension <" + sGivenExtension + "> in <" + sSrc +
					"> is not supported for GUI edit! Use only '.scxml'!");

				WLOG_DEBUG(L"Adding [%s] to [%s] ...", sInvokeSrcFile.c_str(), AUnit->FileName.c_str());

				// сначала пытаемся найти в текущих документах проекта
				TStateMachineEditorUnit *ANewUnit = dynamic_cast<TStateMachineEditorUnit*>(RecursiveSearchOpened(AProjectManager->Root,
						sInvokeSrcFile));

				// если не нашли, то пытаемся открыть
				if (!ANewUnit) {
					ANewUnit = dynamic_cast<TStateMachineEditorUnit*>(AProjectManager->AddExisting(sInvokeSrcFile, AUnit,
							__classid(TStateMachineEditorUnit)));
					if (ANewUnit) {
						WLOG_INFO(L"Successfully added [%s] to [%s]", sInvokeSrcFile.c_str(), AUnit->FileName.c_str());
						if (!AProjectManager->Root->IsModified(false)) {
							AProjectManager->Root->MarkModified();
						}
					}
					else
						throw Exception(L"Could not add [" + sInvokeSrcFile + "]!");
				}

				if (ANewUnit) {
					DoAddAllExistingInvokeSrc(AProjectManager, ANewUnit);
				}
			}
			catch(Exception * E) {
				LOG_ERROR(LOG_ERROR_MSG);
			}
		}
		else {
			for (int i = 0; i < ANode->ChildNodes->Count; i++) {
				IterateInvokeSrcNode(AProjectManager, AUnit, ANode->ChildNodes->Nodes[i]);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall IterateVirtualSrcNode(TLMDProjectManager *AProjectManager, TStateMachineDockPanel *ADockPanel,
	TStateMachineEditorUnit *AUnit) {
	if (ADockPanel) {
		for (int i = 0; i < ADockPanel->StateMachineEditor->TheTree->Shapes->Count; i++) {
			TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>(ADockPanel->StateMachineEditor->TheTree->Shapes->Items[i]);
			if (AVirtualShape && !AVirtualShape->ExcludeFromSave) {
				if (!AVirtualShape->SrcUnit) {
					try {
						if (!FileExists(AVirtualShape->FullPath))
							throw Exception(L"Virtual machine unit [" + AVirtualShape->FullPath + "] is not found!");

						TStateMachineEditorUnit *AVirtualUnit = GlobalAddExistingStateMachineUnit(AUnit, AVirtualShape->FullPath, true);
						if (!AVirtualUnit)
							throw Exception(L"Virtual machine unit [" + AVirtualShape->FullPath + "] was not created!");

						AVirtualUnit->StateMachineUnitType = smutVirtual;
						AVirtualUnit->Open(true);

						DoAddAllExistingInvokeSrc(AProjectManager, AVirtualUnit);
					}
					catch(Exception * E) {
						LOG_ERROR(LOG_ERROR_MSG);
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall DoAddAllExistingInvokeSrc(TLMDProjectManager *AProjectManager, TStateMachineEditorUnit *AUnit) {
	try {
		if (AUnit) {
			if (!FileExists(AUnit->FilePath))
				throw Exception("File <" + AUnit->FilePath + "> does not exist!");

			_di_IXMLDocument ADoc = LoadXMLDocument(AUnit->FilePath);
			if (!ADoc || !ADoc->DocumentElement)
				throw Exception("Can not load [" + AUnit->FilePath + "]");

			// вложенность может быть путем <invoke 'src'/>
			IterateInvokeSrcNode(AProjectManager, AUnit, ADoc->DocumentElement);

			// и может быть путем простой разбивки
			IterateVirtualSrcNode(AProjectManager, AUnit->StateMachineDockPanel, AUnit);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
bool IsAviaClipboardData(TOleDragObject * Source, UnicodeString &sOutText) {
	bool bValidData = false;

	UTF8String sBuff;

	if (Source) {
		_di_IDataObject AData = Source->DataObject();
		if (AData) {

			IEnumFORMATETC *pefe;
			FORMATETC fe;

			if (SUCCEEDED(AData->EnumFormatEtc(DATADIR_GET, &pefe))) {
				ULONG nFetch = 0;
				do {
					pefe->Next(1, &fe, &nFetch);

					wchar_t wchBuff[4096] = {
						0
					};
					GetClipboardFormatNameW(fe.cfFormat, wchBuff, 4096);

					if (SameText(wchBuff, L"Avia_DataMonitor_ContainerName")) {
						STGMEDIUM stg;
						if (SUCCEEDED(AData->GetData(&fe, &stg))) {
							sBuff = UTF8String((char*)GlobalLock(stg.hGlobal), GlobalSize(stg.hGlobal));
							GlobalUnlock(stg.hGlobal);

							ReleaseStgMedium(&stg);
							bValidData = true;

							break;
						}
					}
				}
				while (nFetch);

				pefe->Release();
			}

		}
	}

	sOutText = UTF8ToString(sBuff);

	return bValidData;
}

// ---------------------------------------------------------------------------
int IndexOfComponentList(TComponent *AOwner, TComponent *AChild) {
	if (AOwner && AChild) {
		for (int i = 0; i < AOwner->ComponentCount; i++) {
			if (AOwner->Components[i] == AChild) {
				return i;
			}
		}
	}
	return -1;
}

// ---------------------------------------------------------------------------
void __fastcall OnErrorGetCallback(const UnicodeString &sText) {
	WLOG_ERROR(L"%s", sText.c_str());
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TFormScxmlGui::TFormScxmlGui(TComponent * Owner) : TForm(Owner), FTriggersPrevZone(0), FCallBackPrevZone(0),
FActiveEditorDockPanel(0), FBreakpointSync(false) {
	FRunScxmlName = L"";
	FFilterPropFlag = false;
	FScrollLogFlag = false;
	FExternTestListeningFlag = false;
	FUpdateSyntaxViewFlag = false;
	FScxmlViewVisibleFlag = false;
	FApplicationMinimizedFlag = false;
	FBeforeUndockZone = NULL;
	FUndockPanel = NULL;
	FBreakpointForcedFlag = false;
	FSelectProjectManagerViewFlag = false;

	ApplicationTrayIcon = new TTrayIconErrorPriority(this);
	ApplicationTrayIcon->BalloonTitle = L"SCXML Editor";
	ApplicationTrayIcon->Visible = true;

	SettingsData = new TSettingsData(this);

	FrameSearchResults = new TFrameSearchResults(this);
	FrameSearchResults->Parent = DockPanelSearch;
	FrameSearchResults->Align = alNone;

	UpdateCaption();

	FFilterPropsListPtr.reset(new TStringList);

	DockPanelScxmlView->PanelVisible = false;
	FSyntaxXMLView = new TFrameSyntaxScxmlXMLView(this);
	FSyntaxXMLView->Parent = DockPanelScxmlView;
	FSyntaxXMLView->Align = alNone;

	{
		TreeViewBindings->GlobalFormat.Border->Visible = false;
		TreeViewBindings->GlobalFormat.Transparent = false;
		TreeViewBindings->GlobalFormat.NodeClass = __classid(TMaskedTextShape);
		TreeViewBindings->NoOwnerShapes = true;
		TreeViewBindings->AssignParent = false;

		HTMLRecent->GlobalFormat.Border->Visible = false;
		HTMLRecent->GlobalFormat.Transparent = false;
		HTMLRecent->GlobalFormat.ImageIndex = tiNone;
		HTMLRecent->NoOwnerShapes = true;
		HTMLRecent->AssignParent = false;
		HTMLRecent->GlobalFormat.NodeClass = __classid(TAnchorTextShape);
	}

	ProjectManager1->RegisterNodeClass(__classid(TStateMachineEditorUnit));
	ProjectManager1->RegisterNodeClass(__classid(TStateMachineProject));

	// необходимо указывать здесь, так как переопределили неправильно свойство в производном классе
	ProjectManagerView1->HideSelection = false;

	try {
		const bool bSettingsExist = FileExists(SettingsData->AppSettingsFile);
		if (bSettingsExist) {
			SettingsData->LoadFromFile(SettingsData->AppSettingsFile);

			PanelPropInspPath->Visible = StrToBoolDef(SettingsData->TempRegistry->Values["PanelPropInspPath.Visible"], true);
			LMDDockSite1->LockMode = SettingsData->LockUI ? (LMDDockSite1->LockMode << dlmUndockFrom) :
				(LMDDockSite1->LockMode >> dlmUndockFrom);
		}

		// если первый запуск программы или список недавних пуст,
		// то добавим демо проекты
		if (!bSettingsExist || SettingsData->RecentFiles->Text.Trim().IsEmpty()) {

			const UnicodeString sExamplesPath = TPath::Combine(SettingsData->AppPath, L"Examples");
			TStringDynArray AProjects = TDirectory::GetFiles(sExamplesPath, L"*.sproj");
			TStringDynArray AUnits = TDirectory::GetFiles(sExamplesPath, L"*.scxml");

			// первые пять проектов
			for (int i = 0; i < AProjects.Length; i++) {
				SettingsData->RecentFiles->Add(AProjects[i]);
				if (i == 5)
					break;
			}

			// первые пять юнитов
			for (int i = 0; i < AUnits.Length; i++) {
				SettingsData->RecentFiles->Add(AUnits[i]);
				if (i == 5)
					break;
			}
		}
	}
	catch(Exception * E) {
		SendBallonMsg(E->Message, bfError);
	}

	try {
		std::auto_ptr<TStringList>AObjectList(new TStringList);
		AObjectList->AddObject(L"BalloonAppender", ApplicationTrayIcon);
		AObjectList->AddObject(L"EditDocumentAppender", EditDocDebug);

		/* bugfix: 'https://github.com/alexzhornyak/ScxmlEditor-Tutorial/issues/42' */
		std::auto_ptr<TStringList>ALoggingPropsPtr(new TStringList());
		ALoggingPropsPtr->Text = SettingsData->LoggingProperties->Text;
		const UnicodeString sWasFileName = ALoggingPropsPtr->Values["log4cpp.appender.file.fileName"];
		UnicodeString sLogFileName = sWasFileName.IsEmpty() ? UnicodeString(L"ScxmlEditor.log") : ExtractFileName(sWasFileName);
		UnicodeString sLogPath = ExtractFilePath(sLogFileName);
		if (sLogPath.IsEmpty()) {
			sLogPath = SettingsData->AppPath;
		}

		ALoggingPropsPtr->Values["log4cpp.appender.file.fileName"] = TPath::Combine(sLogPath, sLogFileName);

		const AnsiString sText = ALoggingPropsPtr->Text;
		std::stringstream ss;
		ss << sText.c_str();
		ss.flush();
		log4cpp::PropertyConfiguratorEx::configure(ss, AObjectList.get());

		LOG_DEBUG_SS << "Logger initialized!";
	}
	catch(std::exception & e) {
		SendBallonMsg(e.what(), bfError);
	}

	try {
		if (!DirectoryExists(SettingsData->GetCacheDir())) {
			ForceDirectories(SettingsData->GetCacheDir());
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	try {
		for (int i = 0; i < SettingsData->AutoCompleteItems->Count; i++) {
			if (!SettingsData->AutoCompleteItems->Items[i]->KeywordHints->Count) {
				const UnicodeString sFileName = TPath::Combine(SettingsData->AppPath,
					"AutoComplete\\KeywordHints." + SettingsData->AutoCompleteItems->Items[i]->SyntaxScheme + ".xml");
				if (FileExists(sFileName)) {
					std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(this));
					AXMLDocument->XML->LoadFromFile(sFileName);
					AXMLDocument->Active = true;

					if (AXMLDocument->DocumentElement) {
						for (int k = 0; k < AXMLDocument->DocumentElement->ChildNodes->Count; k++) {
							_di_IXMLNode AChildNode = AXMLDocument->DocumentElement->ChildNodes->Nodes[k];
							Xmlserialize::XMLDeserializeProperty(AChildNode, SettingsData->AutoCompleteItems->Items[i]->KeywordHints,
								OnErrorGetCallback);
						}
						WLOG_INFO(L"AutoComplete> Successfully loaded default [%s]", sFileName.c_str());
					}
				}
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"AutoComplete> %s", E->Message.c_str());
	}

	DockPanelGifMaker->PanelVisible = false;
	FrameGifMaker = new TFrameGifMaker(this);
	FrameGifMaker->Parent = DockPanelGifMaker;
	FrameGifMaker->Align = alNone;

	TransitionXMLEditor = new TTransitionXMLEditor(DockPanelTransitionXML);
	TransitionXMLEditor->Parent = DockPanelTransitionXML;
	TransitionXMLEditor->Align = alNone;
	AccessDockPanel *ADockPanelTransitionXML = reinterpret_cast<AccessDockPanel*>(DockPanelTransitionXML);
	ADockPanelTransitionXML->SetMouseActivateEvent(OnFixDockPanelMouseActivate);

	TransitionXMLEditor->Show();

	if (SettingsData->PropInspInfoHeight > HTMLPropInfo->Constraints->MinHeight) {
		HTMLPropInfo->Height = SettingsData->PropInspInfoHeight;
	}
	PropSettingsInspector->Splitter = SettingsData->PropInspGutterWidth;

	Application->HintShortPause = SettingsData->HintShortPause;
	Application->HintPause = SettingsData->HintPause;
	Application->HintHidePause = SettingsData->HintHidePause;

	/* применяем OnResize только в этом месте, так как, если назначать на форме, то OnResize уже вызывается ещё до того,
	как закончились все конструкторы */
	for (int i = 0; i < this->ComponentCount; i++) {
		TLMDDockPanel * ADockPanel = dynamic_cast<TLMDDockPanel*>(this->Components[i]);
		if (ADockPanel) {
			ADockPanel->OnResize = DockPanelFixResize;
		}
	}

	try {
		LMDDockManager1->SaveToFile(SettingsData->AppLayoutDefault);
		WLOG_DEBUG(L"Successfully saved default layout [%s]", SettingsData->AppLayoutDefault.c_str());

		if (FileExists(SettingsData->AppLayoutFile)) {
			LMDDockManager1->LoadFromFile(SettingsData->AppLayoutFile);
			WLOG_DEBUG(L"Successfully loaded layout [%s]", SettingsData->AppLayoutFile.c_str());
		}
		else {
			WLOG_WARNING(L"Layout file <%s> is not found!", SettingsData->AppLayoutFile.c_str());
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	try {
		Screen->Cursors[crPanningCursor] = LoadCursorW(HInstance, L"ResCurPan");
		if (!Screen->Cursors[crPanningCursor])
			throw Exception("Can not load panning cursor [ResCurPan]!");
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	UpdateHtmlRecentFiles();

	MenuLuaviaHelp->Visible = SettingsData->AviaExtensionEnabled;

	BtnRefreshBindings->Caption = L""; // reset Tool button caption set by TAction

	/* CMD start 'scxml' or 'sproj' */ try {
		if (ParamCount() >= 1) {
			const UnicodeString sFileName = ParamStr(1);
			/* 'command' is the reserved name for command options */
			if (sFileName != SCXML_INTERPROCESS_COMMAND) {
				if (FileExists(sFileName)) {
					const UnicodeString sExt = ExtractFileExt(sFileName);
					if (SameText(sExt, TStateMachineProject::FileExt_)) {
						DoOpenProject(sFileName);
					}
					else if (SameText(sExt, TStateMachineEditorUnit::FileExt_)) {
						DoOpenUnit(sFileName);
					}
					else
						throw Exception("CMD arg ext [" + sExt + "] is not supported! File [" + sFileName + "]");
				}
				else
					throw Exception("CMD arg file [" + sFileName + "] does not exist!");
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

}

// ---------------------------------------------------------------------------
__fastcall TFormScxmlGui::~TFormScxmlGui() {
	const log4cpp::AppenderSet ASet = log4cpp::Category::getRoot().getAllAppenders();
	/* delete custom Appender's, don't know why original lib does not do this */
	for (log4cpp::AppenderSet::const_iterator it = ASet.begin(); it != ASet.end(); ++it) {
		log4cpp::Appender *appender = *it;
		delete appender;
	}

	log4cpp::Category::shutdown();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::SendBallonMsg(const UnicodeString & sMessage, const TBalloonFlags AFlags) {
	if (ApplicationTrayIcon) {
		ApplicationTrayIcon->SendBallonMsg(sMessage, AFlags);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::LMDDockManager1WriteAppInfo(TObject * Sender, const ILMDXmlDocument * Xml) {
	ILMDXmlDocument *AXML = const_cast<ILMDXmlDocument*>(Xml);

	ILMDXmlElement *AFormPosNode = AXML->DocumentElement->AppendElement("FormSettings");
	AFormPosNode->SetAttr("BoundsRect", Customutils::RectToStr(this->BoundsRect));
	AFormPosNode->SetIntAttr("WindowState", this->WindowState);

	ILMDXmlElement *AControlBarNode = AXML->DocumentElement->AppendElement("ControlBarSettings");
	AControlBarNode->SetIntAttr("Height", ControlBar1->Height);
	for (int i = 0; i < ControlBar1->ControlCount; i++) {
		if (ControlBar1->Controls[i]->Name.IsEmpty()) {
			LOG_ERROR("Control bar item's name is empty");
			continue;
		}
		ILMDXmlElement *ANodeItem = AControlBarNode->AppendElement(ControlBar1->Controls[i]->Name);
		ANodeItem->SetAttr("BoundsRect", Customutils::RectToStr(ControlBar1->Controls[i]->BoundsRect));
		ANodeItem->SetBoolAttr("Visible", ControlBar1->Controls[i]->Visible);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::LMDDockManager1BeforeRead(TObject * Sender, const ILMDXmlDocument * Xml) {
	ILMDXmlDocument *AXML = const_cast<ILMDXmlDocument*>(Xml);
	if (AXML->DocumentElement) {
		ILMDXmlElement * AFormPosNode = AXML->DocumentElement->FindElement(L"FormSettings", L"", 0);
		if (AFormPosNode) {
			TWindowState AWindowState = TWindowState(AFormPosNode->GetIntAttr("WindowState", this->WindowState));

			if (this->WindowState != wsNormal) {
				this->WindowState = wsNormal;
			}

			if (AFormPosNode->AttrExists("BoundsRect")) {
				const UnicodeString sRect = AFormPosNode->GetAttr("BoundsRect");
				// координаты прямоугольника, которые мы получили из конфига
				const TRect ASettingsRect = Customutils::StrToRect(sRect, this->BoundsRect);
				TRect AResultRect;
				// проверяем, чтобы сохранённые в конфиге координаты не вылезли за пределы экрана
				if (IntersectRect(AResultRect, ASettingsRect, Screen->DesktopRect)) {
					this->BoundsRect = ASettingsRect;
				}
				else {
					WLOG_WARNING(L"Config application rect <%s> does not intersect with desktop rect <%s>", sRect.c_str(),
						Customutils::RectToStr(Screen->DesktopRect).c_str());
				}
			}

			this->WindowState = AWindowState;

			this->Visible = true;
		}

		ILMDXmlElement * AControlBarNode = AXML->DocumentElement->FindElement(L"ControlBarSettings", L"", 0);
		if (AControlBarNode) {
			ControlBar1->Height = AControlBarNode->GetIntAttr("Height", ControlBar1->Height);

			for (int i = 0; i < ControlBar1->ControlCount; i++) {
				ILMDXmlElement * ANodeItem = AControlBarNode->FindElement(ControlBar1->Controls[i]->Name, L"", 0);
				if (ANodeItem) {

					ControlBar1->Controls[i]->Visible = ANodeItem->GetBoolAttr("Visible", ControlBar1->Controls[i]->Visible);

					if (ANodeItem->AttrExists("BoundsRect")) {
						const UnicodeString sBoundsRect = ANodeItem->GetAttr("BoundsRect");
						ControlBar1->Controls[i]->BoundsRect = Customutils::StrToRect(sBoundsRect, ControlBar1->Controls[i]->BoundsRect);
						TToolBar *AToolBar = dynamic_cast<TToolBar*>(ControlBar1->Controls[i]);
						if (AToolBar) {
							AToolBar->AutoSize = true;
						}
					}
				}

			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::FixDockPanelSize(TLMDDockPanel *ADockPanel) {
	if (ADockPanel && ADockPanel->ControlCount) {
		AccessDockPanel *AFixPanel = reinterpret_cast<AccessDockPanel*>(ADockPanel);
		TRect ABounds;
		AFixPanel->GetFixAdjustClientRect(ABounds);

		ADockPanel->Controls[0]->BoundsRect = ABounds;

		// For Deep Debug
		// WLOG_DEBUG(L"Resize=%s", ADockPanel->Caption.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ProjectManager1Open(TObject * Sender, bool AShowEditor, TLMDProjDocOpenContext * AContext) {
	TStateMachineEditorUnit *AEditorUnit = dynamic_cast<TStateMachineEditorUnit*>(AContext->Document);
	if (AEditorUnit) {
		// если указано показать редактор, тогда формируем док.панель
		if (AShowEditor) {

			AEditorUnit->StateMachineDockPanel = new TStateMachineDockPanel(this, AEditorUnit);
			AEditorUnit->StateMachineDockPanel->Name = TeeGetUniqueName(this, "StateMachineDockPanel");

			// тестируем открытие контекста раньше
			AContext->Open(AEditorUnit->StateMachineDockPanel->OpenedDoc);

			LMDDockSite1->DockControl(AEditorUnit->StateMachineDockPanel, DockPanelFrameRoot->Zone, alClient);

			/* Выполняем манипуляции, чтобы предотвратить тормоза при Resize */
			/* По умолчанию MainPanel->Align = alClient */
			/* это должно быть, чтобы при докинге не было моргания при создании панели */

			SendMessage(AEditorUnit->StateMachineDockPanel->Handle, WM_SETREDRAW, 0, 0);
			AEditorUnit->StateMachineDockPanel->DisableAlign();

			try {
				AEditorUnit->StateMachineDockPanel->MainPanel->Align = alNone;
				FixDockPanelSize(AEditorUnit->StateMachineDockPanel);
			}
			__finally {
				AEditorUnit->StateMachineDockPanel->EnableAlign();
				SendMessage(AEditorUnit->StateMachineDockPanel->Handle, WM_SETREDRAW, 1, 0);
			}

			AEditorUnit->StateMachineDockPanel->OnResize = DockPanelFixResize;

			// предполагаем, что эта панель станет активной
			FActiveEditorDockPanel = AEditorUnit->StateMachineDockPanel;
		}
		// переключаем вкладки с инспектором и свойствами, если не в режиме тестирования
		if (!SettingsData->IsTesterWorking) {
			if (DockPanelProjectTree->Zone && DockPanelProjectTree->Zone->Parent) {
				DockPanelProjectTree->Zone->Parent->ActivePage = DockPanelProjectTree->Zone;
			}
			if (DockPanelPropInsp->Zone && DockPanelPropInsp->Zone->Parent) {
				DockPanelPropInsp->Zone->Parent->ActivePage = DockPanelPropInsp->Zone;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ProjectManager1SaveQuery(TObject * Sender, TLMDProjDocument * ADocument,
	TLMDProjDocumentSaveQueryRes & ASave) {
	UnicodeString msg = L"Save \"" + ADocument->DisplayName + L"\"?";

	if (!ADocument->IsNew)
		msg = L"File \"" + ADocument->DisplayName + L"\" has been modified!\n"

			L"Save Changes?";

	switch(MessageDlg(msg, mtConfirmation, (TMsgDlgButtons() << mbYes << mbNo << mbCancel), 0)) {
	case mrYes:
		ASave = sqrYes;
		break;
	case mrNo:
		ASave = sqrNo;
		break;
	default:
		ASave = sqrCancel;
		break;
	};
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::InternalRevertSaveQuery(TObject * Sender, TLMDProjDocument * ADocument,
	TLMDProjDocumentSaveQueryRes & ASave) {

	/* Special case for revert operation */
	ASave = sqrNo;

}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ProjectManager1SaveAs(TObject * Sender, TLMDProjDocument * ADocument, TLMDString & AFilePath, bool&ASave) {
	bool bIsValid = false;

	/* Execute these actions only if we are not in Replace Mode */
	if (SaveDialog->Tag != 1) {
		if (TStateMachineProject * AProject = dynamic_cast<TStateMachineProject*>(ADocument)) {
			SaveDialog->Filter = TStateMachineProject::OpenDialogFilter;
			SaveDialog->FileName = ChangeFileExt(AFilePath, TStateMachineProject::FileExt_);
			bIsValid = true;
		}
		else if (TStateMachineEditorUnit * AUnit = dynamic_cast<TStateMachineEditorUnit*>(ADocument)) {
			SaveDialog->Filter = TStateMachineEditorUnit::OpenDialogFilter;
			SaveDialog->FileName = ChangeFileExt(AFilePath, TStateMachineEditorUnit::FileExt_);
			bIsValid = true;
		}

		if (!bIsValid) {
			return;
		}

		// при переименованиях показывает туда, куда запомнило последний раз,
		// и часто надо очень долго искать путь
		UnicodeString sInitialDir = ExtractFileDir(AFilePath);

		// если текущая директория не определена, попробуем взять у родительского элемента
		if (sInitialDir.IsEmpty()) {
			if (ADocument) {
				TLMDProjDocument *AParentDocument = dynamic_cast<TLMDProjDocument*>(ADocument->Parent);
				if (AParentDocument) {
					sInitialDir = ExtractFileDir(AParentDocument->FilePath);
				}
			}
		}

		if (DirectoryExists(sInitialDir)) {
			SaveDialog->InitialDir = sInitialDir;
		}
	}

	/* 'Tag' is used to overrede rename request */
	if (SaveDialog->Tag == 1 || SaveDialog->Execute()) {
		SaveDialog->Tag = 0;

		// выдадим предупреждение пользователю, если расширение не совпадает с исходным
		const UnicodeString sOldFileExt = ExtractFileExt(AFilePath);
		const UnicodeString sNewFileExt = ExtractFileExt(SaveDialog->FileName);
		if (sOldFileExt.LowerCase() != sNewFileExt.LowerCase()) {
			const String Caption = "WARNING!!!";
			const String Text = UnicodeString().sprintf(L"New extension [*%s] does not match to old one [*%s],\n" //
				"and will be corrected according to original extension as [%s]\n\n" //
				"P.S. Dialog filename [%s]", sNewFileExt.c_str(), sOldFileExt.c_str(),
				ChangeFileExt(SaveDialog->FileName, sOldFileExt).c_str(), SaveDialog->FileName.c_str());
			if (Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING) != IDOK) {
				return;
			}

		}
		AFilePath = SaveDialog->FileName;
		ASave = true;

		SettingsData->AddRecentFileName(AFilePath);

		TStateMachineEditorUnit * AUnit = dynamic_cast<TStateMachineEditorUnit*>(ADocument);
		if (AUnit) {
			std::vector<TStateMachineEditorUnit*>AVecUnits;
			if (AUnit->IsVirtual) {
				CollectAllEnabledUnits(ProjectManager1->Root, AVecUnits);
				for (std::size_t i = 0; i < AVecUnits.size(); i++) {
					if (AVecUnits[i] == AUnit)
						continue;

					TStateMachineDockPanel *ADockPanel = AVecUnits[i]->StateMachineDockPanel;
					if (ADockPanel) {
						bool bTreeModified = false;
						for (int n = 0; n < ADockPanel->StateMachineEditor->TheTree->Shapes->Count; n++) {
							TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>
								(ADockPanel->StateMachineEditor->TheTree->Shapes->Items[n]);
							if (AVirtualShape) {
								if (SameText(AVirtualShape->FileName, AUnit->FileName)) {
									AVirtualShape->Src = ExtractRelativePath(AVecUnits[i]->FilePath, AFilePath);
									WLOG_DEBUG(L"Virtual path:[%s] was changed to:[%s]", AVecUnits[i]->FileName.c_str(),
										ExtractFileName(AFilePath).c_str());
									bTreeModified = true;
								}
							}
						}
						if (bTreeModified) {
							ADockPanel->StateMachineEditor->MarkModified(1, "New virtual path:" + AFilePath, false);
						}
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TFormScxmlGui::ProjCloseQuery() {
	bool res = true;

	if (ProjectManager1->Root == NULL)
		return res;
	else if (ProjectManager1->Root->Discardable) {
		/* ignore */
	}
	else if (ProjectManager1->Root->NeedSave) {
		switch(ProjectManager1->SaveQuery()) {
		case sqrYes: {
				res = ProjectManager1->Save(false);
				break;
			}
		case sqrNo: {
				if (ProjectManager1->Root->IsNew) {
					/* ignore */
				}
				break;
			}
		case sqrCancel: {
				res = false;
				break;
			}
		}
	}

	if (res && ProjectManager1->Root) {
		res = RecursiveCloseUnit(ProjectManager1->Root);
	}

	return res;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actNewProjExecute(TObject * Sender) {
	try {
		if (ProjCloseQuery()) {

			DeleteAllProjects();

			TStateMachineProject *AProject = dynamic_cast<TStateMachineProject*>
				(ProjectManager1->New(NULL, __classid(TStateMachineProject)));
			if (AProject) {
				TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManager1->New(AProject,
						__classid(TStateMachineEditorUnit)));
				if (AUnit) {
					AUnit->IsBuildRoot = true;
					AUnit->Open(true);
				}

				SetDockPanelStackCaption(0);
			}
			else
				throw Exception(L"Can not cast to <TStateMachineProject>");

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DeleteAllProjects() {
	if (ProjectManager1->Root) {
		RecursiveCloseUnit(ProjectManager1->Root);
		ProjectManager1->Root->Delete();
		FSyntaxXMLView->ClearAll();
		ListViewCallStack->Items->Clear();
		EditDocTester->Clear();
		FBreakpointSync = false;
		FLockFreeQueue.clear();
		TreeViewBindings->Clear();
		TThread::RemoveQueuedEvents(NULL, OnExternTesterMsgReceived);

		while (ScrollBoxTriggers->ControlCount) {
			delete ScrollBoxTriggers->Controls[0];
		}

		TransitionXMLEditor->ResetStateMachineConnection();
	}
	SetDockPanelStackCaption(0);
	SwitchToProgramLog(false);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actCloseAllExecute(TObject * Sender) {
	try {

		const int i_MAX_DISPLAY_WAIT_COUNT = 2;

		// процесс может быть затяжным, поэтому, если более чем i_MAX_DISPLAY_WAIT_COUNT,
		// тогда развлечем пользователя инфой
		std::vector<TStateMachineEditorUnit*>AVecUnits;
		CollectAllEnabledUnits(this->ProjectManager1->Root, AVecUnits);
		int iOpenCount = 0;
		for (std::size_t i = 0; i < AVecUnits.size(); i++) {
			if (AVecUnits[i]->Opened) {

				iOpenCount++;

				if (iOpenCount > i_MAX_DISPLAY_WAIT_COUNT) {

					if (!DialogWait) {
						DialogWait = new TDialogWait(this);
					}

					break;
				}
			}
		}

		if (DialogWait) {
			DialogWait->ShowDialog("Closing all projects ...");
		}

		/* Непосредственно сам процесс закрытия и удаления панелей */
		if (ProjCloseQuery()) {
			DeleteAllProjects();
			UpdateCaption();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	/* FINALIZE */
	if (DialogWait) {
		delete DialogWait;
		DialogWait = NULL;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actCloseAllUpdate(TObject * Sender) {
	actCloseAll->Enabled = (ProjectManager1->Root != NULL) && !SettingsData->IsTesterWorking;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actCloseUpdate(TObject *Sender) {
	TStateMachineEditorUnit *AUnit = GetActiveUnit();

	bool bDisabledByTesting = false;
	if (SettingsData->IsTesterWorking) {
		if (StateMachineProject) {
			bDisabledByTesting = StateMachineProject->ProjectRoot == AUnit;
		}
		else {
			bDisabledByTesting = true;
		}
	}
	actClose->Enabled = AUnit && !bDisabledByTesting;
}

// ---------------------------------------------------------------------------
TLMDProjDocument * __fastcall TFormScxmlGui::AddNewUnit(TLMDProjNode * AParent) {
	TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManager1->New(AParent,
			__classid(TStateMachineEditorUnit)));
	if (AUnit) {
		AUnit->Open(true);

		if (StateMachineProject && !StateMachineProject->IsModified(false)) {
			StateMachineProject->MarkModified();
		}

		ProjectManagerView1->FullExpand();
		return AUnit;
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actAddUnitExecute(TObject * Sender) {
	try {
		if (StateMachineProject) {
			AddNewUnit(StateMachineProject);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjViewAddUnitClick(TObject * Sender) {
	try {
		if (TStateMachineEditorUnit * AUnitRoot = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc)) {
			if (AUnitRoot->Enabled) {
				AddNewUnit(AUnitRoot);
			}
		}
		else {
			AddNewUnit(StateMachineProject);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actAddUnitUpdate(TObject * Sender) {
	actAddUnit->Enabled = StateMachineProject && !SettingsData->IsTesterWorking;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSaveExecute(TObject * Sender) {
	try {
		ProjectManager1->Save(GetActiveUnit(), false);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjViewSaveClick(TObject * Sender) {
	try {
		if (ProjectManagerView1->SelectedDoc) {
			ProjectManager1->Save(ProjectManagerView1->SelectedDoc, false);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjForceSaveUnitClick(TObject *Sender) {
	try {
		if (ProjectManagerView1->SelectedDoc) {

			if (!ProjectManagerView1->SelectedDoc->Opened) {
				ProjectManagerView1->SelectedDoc->Open(true);
			}
			else {
				TStateMachineEditorUnit * AUnitRoot = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
				if (AUnitRoot && AUnitRoot->StateMachineDockPanel) {
					AUnitRoot->StateMachineDockPanel->Show();
				}
			}

			ProjectManagerView1->SelectedDoc->MarkModified();

			ProjectManager1->Save(ProjectManagerView1->SelectedDoc, false);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"FORCE SAVE UNIT> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actGetActiveEditorUpdate(TObject * Sender) {
	TCustomAction *Action = dynamic_cast<TCustomAction*>(Sender);
	if (Action) {
		Action->Enabled = GetActiveUnit() != NULL && !SettingsData->IsTesterWorking;
	}
	else {
		WLOG_ERROR(L"Can not cast <%s> to <TCustomAction>", Sender ? Sender->ClassName().c_str() : L"NULL");
	}
}

// ---------------------------------------------------------------------------
TStateMachineDockPanel * __fastcall TFormScxmlGui::GetActiveEditorDockPanel() {
	TStateMachineDockPanel * AStateMachineDockPanel = NULL;

	for (int i = 0; i < this->ComponentCount; i++) {
		TStateMachineDockPanel *ACurrentDockPanel = dynamic_cast<TStateMachineDockPanel*>(this->Components[i]);
		if (ACurrentDockPanel) {
			if (ACurrentDockPanel == FActiveEditorDockPanel) {
				return ACurrentDockPanel;
			}
		}
	}

	return AStateMachineDockPanel;
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit* __fastcall TFormScxmlGui::GetActiveUnit() {
	TStateMachineDockPanel *ADockPanel = GetActiveEditorDockPanel();
	if (ADockPanel) {
		return dynamic_cast<TStateMachineEditorUnit*>(ADockPanel->OpenedDoc->Node);
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineEditor * __fastcall TFormScxmlGui::GetActiveStateMachineEditor() {
	TStateMachineDockPanel *ADockPanel = GetActiveEditorDockPanel();
	if (ADockPanel) {
		return ADockPanel->StateMachineEditor;
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineProject * __fastcall TFormScxmlGui::GetStateMachineProject() {
	return dynamic_cast<TStateMachineProject*>(ProjectManager1->Root);
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actSaveAsExecute(TObject * Sender) {
	try {
		ProjectManager1->Save(GetActiveUnit(), true);

		if (ProjectManager1->Root) {
			ProjectManager1->Root->MarkModified();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::MenuProjViewSaveAsClick(TObject * Sender) {
	try {
		if (ProjectManagerView1->SelectedDoc) {
			ProjectManager1->Save(ProjectManagerView1->SelectedDoc, true);

			if (ProjectManager1->Root) {
				ProjectManager1->Root->MarkModified();
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjCloneUnitClick(TObject * Sender) {
	try {
		TLMDProjDocument* AProjUnit = ProjectManagerView1->SelectedDoc;
		if (AProjUnit) {

			if (AProjUnit->IsModified(false)) {
				const UnicodeString Caption = L"WARNING";
				const UnicodeString Text = L"Do you want to save unit modifications?";
				if (MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OKCANCEL) == IDOK) {
					ProjectManager1->Save(AProjUnit, false);
				}
			}

			TLMDProjNode *AParent = AProjUnit->Parent;

			UnicodeString sFilePath = AProjUnit->FilePath;
			if (!PromptForFileName(sFilePath, TStateMachineEditor::GetOpenDialogFilter(), L"scxml", "Select name for unit to be cloned",
					ExtractFileDir(sFilePath), true))
				throw Exception("Cancelled by user!");

			CopyFileW(AProjUnit->FilePath.c_str(), sFilePath.c_str(), false);

			TLMDProjDocument *AClonedUnit = AddExistingUnit(AParent, sFilePath, false);
			if (!AClonedUnit)
				throw Exception("Can not add " + sFilePath);

			if (!AClonedUnit->Opened) {
				AClonedUnit->Open(true);
			}

			if (ProjectManager1->Root) {
				ProjectManager1->Root->MarkModified();
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"CLONE UNIT> %s", E->Message.c_str());
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjRenameClick(TObject *Sender) {
	try {
		if (ProjectManagerView1->SelectedDoc) {

			const UnicodeString sWasFile = ProjectManagerView1->SelectedDoc->FilePath;
			const UnicodeString sWasFilePart = TPath::GetFileNameWithoutExtension(ProjectManagerView1->SelectedDoc->FilePath);
			UnicodeString sFilePart = sWasFilePart;

			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->Add(L"You are about to " + StripHotkey(MenuProjRename->Caption).LowerCase());
			AStringListPtr->Add(L"MODIFICATIONS WILL BE SAVED!");

			if (InputQuery(L"RENAME", AStringListPtr->Text, sFilePart)) {
				if (SameText(sWasFilePart, sFilePart)) {
					WLOG_WARNING(L"Filename:[%s] was not changed!", sFilePart.c_str());
					return;
				}

				const UnicodeString sExt = ExtractFileExt(ProjectManagerView1->SelectedDoc->FilePath);
				const UnicodeString sFileDir = ExtractFileDir(ProjectManagerView1->SelectedDoc->FilePath);

				SaveDialog->FileName = TPath::Combine(sFileDir, sFilePart + sExt);
				SaveDialog->Tag = 1;

				ProjectManager1->Save(ProjectManagerView1->SelectedDoc, true);

				if (TStateMachineProject * AStateMachineProject = dynamic_cast<TStateMachineProject*>(ProjectManagerView1->SelectedDoc)) {
					DeleteFile(sWasFile);
					DeleteFile(sWasFile + L".checksum");
					DeleteFile(sWasFile + L".props");
					DeleteFile(sWasFile + L".xml");
				}
				else if (TStateMachineEditorUnit * AStateMachineEditorUnit = dynamic_cast<TStateMachineEditorUnit*>
					(ProjectManagerView1->SelectedDoc)) {

					TStateMachineProject* AStateMachineProject = this->StateMachineProject;
					if (AStateMachineProject) {
						AStateMachineProject->MarkModified();
						AStateMachineProject->Save();
					}

					DeleteFile(sWasFile);
					DeleteFile(TStateMachineEditorUnit::MakeBindingFileName(sWasFile));
				}

				UpdateCaption();
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	SaveDialog->Tag = 0;
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actSaveProjectAsExecute(TObject * Sender) {
	try {
		SaveDialog->FileName = ProjectManager1->Root->FilePath;
		ProjectManager1->Save(true);

		UpdateCaption();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actProjectIsOpenedUpdate(TObject * Sender) {
	TCustomAction * Action = dynamic_cast<TCustomAction*>(Sender);
	if (Action) {
		Action->Enabled = ProjectManager1->Root != NULL;
	}
	else {
		WLOG_ERROR(L"Can not cast <%s> to <TCustomAction>", Sender ? Sender->ClassName().c_str() : L"NULL");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSaveAllExecute(TObject * Sender) {
	SettingsData->ProfilerReset(__FUNCTION__);
	try {
		RecursiveSaveAll(ProjectManager1, ProjectManager1->Root);
		UpdateCaption();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	SettingsData->ProfileOperation(__FUNCTION__, L"");
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actSaveAllUpdate(TObject * Sender) {
	actSaveAll->Enabled = !SettingsData->IsTesterWorking && IsAnyDocumentModified(ProjectManager1->Root);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actForceSaveAllOpenedExecute(TObject * Sender) {
	try {
		if (ProjectManager1->Root) {
			// помечаем все открытые, чтобы пересоздались

			std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(this));
			ADialogWaitPtr->ShowDialog("Force saving all opened units ...");

			std::vector<TStateMachineEditorUnit*>AVecUnits;
			CollectAllEnabledUnits(ProjectManager1->Root, AVecUnits);

			for (std::size_t i = 0; i < AVecUnits.size(); i++) {
				if (AVecUnits[i]->Opened) {

					ADialogWaitPtr->ShowDialog(UnicodeString().sprintf(L"Saving [%s] %d from %d ...", AVecUnits[i]->DisplayName.c_str(),
							i + 1, AVecUnits.size()));

					if (!AVecUnits[i]->StateMachineDockPanel)
						throw Exception(AVecUnits[i]->DisplayName + "> StateMachineDockPanel==NULL!");

					AVecUnits[i]->StateMachineDockPanel->Show();

					if (AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->ReloadInheritedTree(true) != oresSuccess)
						throw Exception("Operation was cancelled! Resolve conflicts and try again!");

					AVecUnits[i]->MarkModified();

					ProjectManager1->Save(AVecUnits[i], false);
				}
			}

			UpdateCaption();
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actForceSaveAllOpenedUpdate(TObject * Sender) {
	actForceSaveAllOpened->Enabled = !SettingsData->IsTesterWorking && ProjectManager1->Root;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actForceSaveAllExecute(TObject *Sender) {
	SettingsData->ProfilerReset(__FUNCTION__);
	try {
		std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(NULL));

		std::vector<TStateMachineEditorUnit*>AVecUnits;
		CollectAllEnabledUnits(ProjectManager1->Root, AVecUnits);
		for (std::size_t i = 0; i < AVecUnits.size(); i++) {
			ADialogWaitPtr->ShowDialog(UnicodeString().sprintf(L"Saving [%s] %d from %d ...", AVecUnits[i]->FileName.c_str(), i + 1,
					AVecUnits.size()));

			const bool bWasOpened = AVecUnits[i]->Opened;
			if (!bWasOpened) {
				AVecUnits[i]->Open(true);
			}

			AVecUnits[i]->MarkModified();
			ProjectManager1->Save(AVecUnits[i], false);

			if (!bWasOpened) {
				AVecUnits[i]->Close();
			}
		}

		if (AVecUnits.size()) {

			if (StateMachineProject) {
				ADialogWaitPtr->ShowDialog("Final saving project [" + StateMachineProject->DisplayName + "] ...");
				StateMachineProject->MarkModified();
			}

			ProjectManager1->Save(false);

			UpdateCaption();
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	SettingsData->ProfileOperation(__FUNCTION__, L"");
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actForceSaveAllUpdate(TObject *Sender) {
	actForceSaveAll->Enabled = !SettingsData->IsTesterWorking && ProjectManager1->Root;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DoRemoveUnit(TLMDProjDocument * ANode) {
	try {

		if (ANode) {

			if (RecursiveCloseUnit(ANode)) {

				ANode->Delete();

				if (ProjectManager1->Root && !ProjectManager1->Root->IsModified(false)) {
					ProjectManager1->Root->MarkModified();
				}

				DoCollectAllBindings();
			}

			UpdateCaption();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actRemoveUnitExecute(TObject * Sender) {
	DoRemoveUnit(GetActiveUnit());
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::MenuProjViewRemoveClick(TObject * Sender) {
	DoRemoveUnit(ProjectManagerView1->SelectedDoc);
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::DoOpenProject(const UnicodeString & sFileName) {
	try {
		DeleteAllProjects();

		Screen->Cursor = crHourGlass;

		ProjectManager1->Load(sFileName, __classid(TStateMachineProject));

		if (!ProjectManager1->Root)
			throw Exception("Project is corrupted!");

		if (!StateMachineProject)
			throw Exception("Can not cast project root to <TStateMachineProject>");
		SetDockPanelStackCaption(StateMachineProject->ProjectProperties->FilteredMsgs->Count);

		/* возникли ошибки при загрузке проекта */
		if (StateMachineProject->LoadError) {
			StateMachineProject->MarkModified();
		}

		SettingsData->AddRecentFileName(sFileName);
		UpdateHtmlRecentFiles();

		/* эти манипуляции делаются для унаследованных корневых юнитов */

		// первый доступный редактор
		TStateMachineEditorUnit *ARootEditor = NULL;

		// открываем первый доступный
		for (int i = 0; i < ProjectManager1->Root->NodeCount; i++) {
			TStateMachineEditorUnit *AEditor = dynamic_cast<TStateMachineEditorUnit*>(ProjectManager1->Root->Nodes[i]);
			if (AEditor) {
				if (AEditor->Enabled) {
					ARootEditor = AEditor;
					AEditor->Open(true);
					break;
				}
			}
			else
				throw Exception("Can not cast <" + ProjectManager1->Root->Nodes[i]->ClassName() + "> to <TStateMachineEditorUnit>");
		}

		// если у нас указан основной модуль для Билда, тогда запускаем именно его
		// так как предполагаем, что он запускается корневым для State Machine
		TStateMachineEditorUnit *ARootForBuild = StateMachineProject->RootForBuild;
		if (ARootForBuild) {
			// если корневой файл итак является RootForBuild, тогда избегаем двойного действия
			if (ARootForBuild != ARootEditor) {
				ARootForBuild->Open(true);
			}
		}

		UpdateCaption();

		// fix view bug
		ProjectManagerView1->BeginUpdate();
		ProjectManagerView1->EndUpdate();

		DoCollectAllBindings();

		ProjectManagerView1->FullExpand();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	Screen->Cursor = crDefault;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actOpenProjectExecute(TObject * Sender) {
	try {
		OpenDialog->Filter = TStateMachineProject::OpenDialogFilter + //
		L"|" + TStateMachineEditorUnit::OpenDialogFilter + //
		L"|All files (*.*)|*.*";
		OpenDialog->FilterIndex = 0;
		if (ProjCloseQuery() && OpenDialog->Execute(this->Handle)) {

			const UnicodeString sExt = ExtractFileExt(OpenDialog->FileName);
			if (SameText(sExt, TStateMachineEditorUnit::FileExt_)) {

				const UnicodeString Caption = L"INFO";
				const UnicodeString Text = L"Do you want to create a project with [" + ExtractFileName(OpenDialog->FileName) + "] ?";
				if (MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_ICONINFORMATION | MB_YESNO) == IDYES) {
					DeleteAllProjects();

					TStateMachineProject *AProject = dynamic_cast<TStateMachineProject*>(ProjectManager1->New(NULL,
							__classid(TStateMachineProject)));
					if (AProject) {
						AddExistingUnit(AProject, OpenDialog->FileName, false);
					}
					else
						throw Exception(L"Can not cast to <TStateMachineProject>");
				}
				else {
					DoOpenUnit(OpenDialog->FileName);
				}
			}
			else {
				DoOpenProject(OpenDialog->FileName);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ProjectManagerView1DblClick(TObject * Sender) {
	TStateMachineEditorUnit *AEditor = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
	if (AEditor && AEditor->Enabled) {
		try {
			Screen->Cursor = crHourGlass;
			AEditor->Open(true);
		}
		__finally {
			Screen->Cursor = crDefault;
		}
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::ViewPanelClick(TObject * Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {

		const bool bVisible = !AMenuItem->Checked;

		TControl * AControl = (TControl*)AMenuItem->Tag;

		if (AControl) {

			TLMDDockPanel *ADockPanel = dynamic_cast<TLMDDockPanel*>(AControl);
			if (ADockPanel) {

				ADockPanel->PanelVisible = bVisible;

				if (!ADockPanel->Parent) {
					TLMDDockZone *AZone = DockPanelFrameRoot->PanelVisible ? DockPanelFrameRoot->Zone : this->LMDDockSite1->RootZone;
					this->LMDDockSite1->DockControl(ADockPanel, AZone, alClient);
					WLOG_DEBUG(L"Dock panel [%s] was undocked!", ADockPanel->Caption.c_str());
				}

				if (ADockPanel->PanelVisible) {
					ADockPanel->Show();
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ViewClick(TObject * Sender) {
	MenuDockpanels->Clear();

	// для того, чтобы найти даже панели те панели, которые находятся в отцепленном состоянии,
	// ищем по всем компонентам
	for (int i = 0; i < this->ComponentCount; i++) {
		if (this->Components[i]->InheritsFrom(__classid(TLMDDockPanel))) {
			TLMDDockPanel *ADockPanel = dynamic_cast<TLMDDockPanel*>(this->Components[i]);
			if (ADockPanel) {
				TMenuItem *item = new TMenuItem(this);
				item->Caption = ADockPanel->Caption;
				item->ImageIndex = ADockPanel->ImageIndex;
				item->Tag = (int)ADockPanel;
				item->OnClick = ViewPanelClick;
				item->Checked = ADockPanel->PanelVisible && ADockPanel->Parent;
				this->MenuDockpanels->Add(item);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::UpdateCaption(const UnicodeString sDisplayName /* = L"" */ ) {
	const UnicodeString sProject = StateMachineProject ?
		(IsAnyDocumentModified(StateMachineProject) ? StateMachineProject->DisplayName + L"*" : StateMachineProject->DisplayName)
		: UnicodeString(L"");

	TStateMachineEditorUnit* AUnit = GetActiveUnit();
	const UnicodeString sActiveScxml = sDisplayName.IsEmpty() ? (AUnit ? AUnit->FileName : UnicodeString(L"")) : sDisplayName;

	UnicodeString sCaption = UnicodeString().sprintf(L"SCXML editor version %s", SettingsData->AppVersion.c_str());

	if (!sProject.IsEmpty()) {
		sCaption = sCaption + L" [" + sProject + L"]";
	}

	if (!sActiveScxml.IsEmpty()) {
		sCaption = sCaption + L" [" + sActiveScxml + L"]";
	}

	if (StateMachineProject && !StateMachineProject->ProjectProperties->ProjectDefines.IsEmpty()) {
		sCaption = sCaption + L" [" + StateMachineProject->ProjectProperties->ProjectDefines + L"]";
	}

	this->Caption = sCaption;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjViewOpenClick(TObject * Sender) {
	TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
	if (AUnit && AUnit->Enabled) {
		try {
			Screen->Cursor = crHourGlass;
			AUnit->Open(true);
		}
		__finally {
			Screen->Cursor = crDefault;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjViewReloadClick(TObject *Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit && AUnit->Enabled && AUnit->IsInherited && AUnit->Opened) {

			if (!AUnit->StateMachineDockPanel)
				throw Exception("StateMachineDockPanel==NULL!");

			AUnit->StateMachineDockPanel->Show();

			AUnit->StateMachineDockPanel->StateMachineEditor->ReloadInheritedTree(true);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjInheritanceMismatchesClick(TObject *Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit && AUnit->Enabled && AUnit->IsInherited && AUnit->Opened) {

			if (!AUnit->StateMachineDockPanel)
				throw Exception("StateMachineDockPanel==NULL!");

			AUnit->StateMachineDockPanel->StateMachineEditor->CheckInheritedMismatches();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjForceSaveAllInheritedClick(TObject *Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit && AUnit->Enabled) {

			std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(this));
			ADialogWaitPtr->ShowDialog("Saving inherited units from [" + AUnit->FileName + "] ...");

			std::vector<TStateMachineEditorUnit*>AVecUnits = AUnit->GetInheritedUnitsFromThis();
			for (std::size_t i = 0; i < AVecUnits.size(); i++) {

				WLOG_DEBUG(L"Derived unit:[%s]", AVecUnits[i]->FileName.c_str());

				ADialogWaitPtr->ShowDialog(UnicodeString().sprintf(L"Saving [%s] %d from %d ...", AVecUnits[i]->FileName.c_str(), i + 1,
						AVecUnits.size()));

				if (!AVecUnits[i]->Opened) {
					AVecUnits[i]->Open(true);
				}

				if (!AVecUnits[i]->StateMachineDockPanel)
					throw Exception(AVecUnits[i]->DisplayName + "> StateMachineDockPanel==NULL!");

				AVecUnits[i]->StateMachineDockPanel->Show();

				if (AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->ReloadInheritedTree(true) != oresSuccess)
					throw Exception("Operation was cancelled! Resolve conflicts and try again!");

				AVecUnits[i]->MarkModified();

				ProjectManager1->Save(AVecUnits[i], false);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjOpenAllInheritedClick(TObject *Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit && AUnit->Enabled) {

			std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(this));
			ADialogWaitPtr->ShowDialog("Opening inherited units from [" + AUnit->FileName + "] ...");

			std::vector<TStateMachineEditorUnit*>AVecUnits = AUnit->GetInheritedUnitsFromThis();
			for (std::size_t i = 0; i < AVecUnits.size(); i++) {

				WLOG_DEBUG(L"Derived unit:[%s]", AVecUnits[i]->FileName.c_str());

				ADialogWaitPtr->ShowDialog(UnicodeString().sprintf(L"Opening [%s] %d from %d ...", AVecUnits[i]->FileName.c_str(), i + 1,
						AVecUnits.size()));

				if (!AVecUnits[i]->Opened) {
					AVecUnits[i]->Open(true);
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnMenuProjResetInheritance(TObject * Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit && AUnit->Enabled) {

			const UnicodeString Caption = L"WARNING";
			const UnicodeString Text = L"This operation will reset inheritance!\n" //
			"You WILL NOT be able to inherit again without loosing you data!\n" //
			"Are you sure to continue?";
			if (MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK) {
				// необходимо открыть, так как это должно прописаться в сам SCXML
				if (!AUnit->Opened) {
					AUnit->Open(true);
				}
				if (!AUnit->StateMachineDockPanel)
					throw Exception("StateMachineDockPanel==NULL!");

				AUnit->Inherited = L"";
				AUnit->StateMachineDockPanel->StateMachineEditor->ResetInheritance();

				if (StateMachineProject) {
					StateMachineProject->MarkModified();
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnMenuProjAddInheritance(TObject * Sender) {
	try {

		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {

			TStateMachineEditorUnit *AInheritedUnit = reinterpret_cast<TStateMachineEditorUnit*>(AMenuItem->Tag);

			if (!AInheritedUnit->Opened) {
				AInheritedUnit->Open(true);
			}

			if (!AInheritedUnit->StateMachineDockPanel)
				throw Exception("AInheritedUnit->StateMachineDockPanel==NULL!");

			TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
			if (AUnit && AUnit->Enabled) {

				// необходимо открыть, так как это должно прописаться в сам SCXML
				if (!AUnit->Opened) {
					AUnit->Open(true);
				}
				if (!AUnit->StateMachineDockPanel)
					throw Exception("StateMachineDockPanel==NULL!");

				// надо переключить на наш текущий юнит
				if (AUnit->StateMachineDockPanel->Zone && AUnit->StateMachineDockPanel->Zone->Parent) {
					AUnit->StateMachineDockPanel->Zone->Parent->ActivePage = AUnit->StateMachineDockPanel->Zone;
				}

				const UnicodeString Caption = L"WARNING";
				const UnicodeString Text = L"This operation will add inheritance from <" + AInheritedUnit->FileName + ">!\n" //
				"All your current shapes and connections WILL BE ERASED!\n" //
				"Are you sure to continue?";
				if (MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK) {

					std::auto_ptr<TMemoryStream>AMemoryStreamPtr(new TMemoryStream());
					AInheritedUnit->StateMachineDockPanel->StateMachineEditor->TheTree->Cursor = crDefault;
					SaveTreeToStream(AInheritedUnit->StateMachineDockPanel->StateMachineEditor->TheTree, AMemoryStreamPtr.get());

					AUnit->StateMachineDockPanel->StateMachineEditor->LoadScxmlStream(AMemoryStreamPtr.get(), AUnit->FilePath);
					AUnit->StateMachineDockPanel->StateMachineEditor->SetNewInheritance();
					AUnit->Inherited = AInheritedUnit->FileName;

					if (StateMachineProject) {
						StateMachineProject->MarkModified();
					}
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PopupProjViewPopup(TObject * Sender) {

	TStateMachineEditorUnit *AStateMachineEditorUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
	TStateMachineProject *AStateMachineProject = dynamic_cast<TStateMachineProject*>(ProjectManagerView1->SelectedDoc);

	const bool bIsStateMachineUnit = AStateMachineEditorUnit != NULL;
	const bool bIsStateMachineProject = AStateMachineProject != NULL;
	const bool bIsUnitEnabled = bIsStateMachineUnit && AStateMachineEditorUnit->Enabled;
	const bool bIsProjectMode = StateMachineProject != NULL;

	const bool bIsEnabledUnitInProject = bIsProjectMode && bIsUnitEnabled;

	MenuProjViewOpen->Enabled = bIsUnitEnabled;
	MenuProjViewSave->Enabled = bIsStateMachineUnit && ProjectManagerView1->SelectedDoc->IsModified(false);
	MenuProjViewSaveAs->Enabled = bIsStateMachineUnit;
	MenuProjForceSaveUnit->Enabled = ProjectManagerView1->SelectedDoc;
	MenuProjForceSaveUnit->Caption = AStateMachineProject ? L"Force Save Project" : L"Force Save Unit";

	MenuProjViewClose->Enabled = ProjectManagerView1->SelectedDoc != NULL && ProjectManagerView1->SelectedDoc->Opened;

	MenuProjRename->Visible = (AStateMachineEditorUnit && !AStateMachineEditorUnit->IsNew) || //
	(AStateMachineProject && !AStateMachineProject->IsNew);
	if (MenuProjRename->Visible) {
		MenuProjRename->Caption = bIsStateMachineProject ? L"Rename Project" : L"Rename Unit";
	}

	MenuProjCloneUnit->Visible = bIsEnabledUnitInProject;
	MenuProjViewRemove->Visible = bIsStateMachineUnit;
	MenuProjViewAddUnit->Visible = bIsEnabledUnitInProject || bIsStateMachineProject;
	MenuProjAddExisting->Visible = bIsEnabledUnitInProject || bIsStateMachineProject;
	MenuProjAddAllInvokeSrc->Visible = bIsEnabledUnitInProject;
	MenuProjAddExistingUnitWithAllNestedUnits->Visible = bIsEnabledUnitInProject || bIsStateMachineProject;

	MenuProjUnitScxmlName->Visible = bIsStateMachineUnit;
	MenuProjVirtual->Visible = bIsEnabledUnitInProject;

	MenuProjOptions->Visible = bIsProjectMode;

	MenuProjBindingsDisabled->Visible = bIsProjectMode && SettingsData->AviaExtensionEnabled && bIsStateMachineUnit;
	if (MenuProjBindingsDisabled->Visible) {
		MenuProjBindingsDisabled->Checked = AStateMachineEditorUnit->BindingsDisabled;
	}

	MenuProjRoot4Build->Visible = bIsEnabledUnitInProject;
	if (MenuProjRoot4Build->Visible) {
		MenuProjRoot4Build->Checked = AStateMachineEditorUnit->IsBuildRoot;
	}

	MenuProjEditOrder->Visible = bIsProjectMode;
	MenuProjEditOrder->Enabled = !SettingsData->IsTesterWorking && !IsAnyDocumentModified(ProjectManager1->Root);
	MenuProjEditOrder->Caption = MenuProjEditOrder->Enabled ? L"Edit Project Order" : L"Edit Project Order (Save all to enable)";

	const bool bIsInheritedOpened = bIsEnabledUnitInProject && AStateMachineEditorUnit->IsInherited &&
		AStateMachineEditorUnit->StateMachineDockPanel;

	MenuProjViewReload->Visible = bIsInheritedOpened;
	MenuProjInheritanceMismatches->Visible = bIsInheritedOpened;

	std::vector<TStateMachineEditorUnit*>AVecUnits, AVecInherited, AVecAvailableForInheritance;
	if (AStateMachineEditorUnit) {
		CollectAllEnabledUnits(this->ProjectManager1->Root, AVecUnits);
		AVecInherited = AStateMachineEditorUnit->GetInheritedUnitsFromThis();
	}

	const bool bIsBaseUnit = AVecInherited.size() != 0;
	MenuProjForceSaveAllInherited->Visible = bIsBaseUnit;
	MenuProjOpenAllInherited->Visible = bIsBaseUnit;

	// добавим также собственный юнит, чтобы он был исключен из сравнения
	if (AStateMachineEditorUnit) {
		if (std::find(AVecInherited.begin(), AVecInherited.end(), AStateMachineEditorUnit) == AVecInherited.end()) {
			AVecInherited.push_back(AStateMachineEditorUnit);
		}
	}

	std::sort(AVecUnits.begin(), AVecUnits.end());
	std::sort(AVecInherited.begin(), AVecInherited.end());

	std::set_symmetric_difference( //
		AVecUnits.begin(), AVecUnits.end(), //
		AVecInherited.begin(), AVecInherited.end(), //
		std::back_inserter(AVecAvailableForInheritance));

	MenuProjEditInherited->Visible = bIsEnabledUnitInProject;
	MenuProjEditInherited->Clear();
	if (MenuProjEditInherited->Visible) {
		MenuProjEditInherited->Caption = L"Inheritance" + (AStateMachineEditorUnit->Inherited.IsEmpty() ? UnicodeString(L"") :
			(L" [" + AStateMachineEditorUnit->Inherited + "]"));

		TMenuItem *AMenuResetInherited = new TMenuItem(this);
		AMenuResetInherited->Caption = "Reset Inheritance";
		AMenuResetInherited->ImageIndex = 71;
		AMenuResetInherited->OnClick = OnMenuProjResetInheritance;
		MenuProjEditInherited->Add(AMenuResetInherited);
		MenuProjEditInherited->NewBottomLine();

		for (std::size_t i = 0; i < AVecAvailableForInheritance.size(); i++) {

			TMenuItem *AMenuInherited = new TMenuItem(this);
			AMenuInherited->Caption = AVecAvailableForInheritance[i]->FileName;
			AMenuInherited->Tag = reinterpret_cast<int>(AVecAvailableForInheritance[i]);
			MenuProjEditInherited->Add(AMenuInherited);
			AMenuInherited->Checked = SameText(AVecAvailableForInheritance[i]->FileName, AStateMachineEditorUnit->Inherited);
			if (!AMenuInherited->Checked) {
				AMenuInherited->OnClick = OnMenuProjAddInheritance;
			}
		}
	}

	MenuProjUnitEnabled->Visible = bIsProjectMode && bIsStateMachineUnit;
	MenuProjEditPostSave->Visible = bIsEnabledUnitInProject;

	if (AStateMachineEditorUnit) {
		MenuProjUnitEnabled->Checked = AStateMachineEditorUnit->Enabled;
		MenuProjVirtual->Checked = AStateMachineEditorUnit->IsVirtual;

		UnicodeString sText = AStateMachineEditorUnit->PostSave->PostBuildEvaluated.Trim();
		const int i_MAX_LENGTH = 30;
		if (sText.Length() > i_MAX_LENGTH) {
			sText = sText.SetLength(i_MAX_LENGTH).Trim() + L"...";
		}
		MenuProjEditPostSave->Caption = L"Edit Post Save";
		if (!sText.IsEmpty()) {
			MenuProjEditPostSave->Caption = MenuProjEditPostSave->Caption + L" [" + sText + L"]";
		}
	}

	MenuProjMoveUp->Visible = bIsProjectMode && bIsStateMachineUnit;
	MenuProjMoveDown->Visible = bIsProjectMode && bIsStateMachineUnit;

	if (ProjectManagerView1->SelectedDoc) {
		MenuProjPathToClipboard->Caption = L"Path=" + ProjectManagerView1->SelectedDoc->FilePath;
		MenuProjPathToClipboard->Visible = true;

		MenuProjFileNameToClipboard->Caption = L"FileName=" + ExtractFileName(ProjectManagerView1->SelectedDoc->FilePath);
		MenuProjFileNameToClipboard->Visible = true;
	}
	else {
		MenuProjPathToClipboard->Visible = false;
		MenuProjFileNameToClipboard->Visible = false;
	}

	if (AStateMachineEditorUnit) {
		MenuProjUnitScxmlName->Caption = L"ScxmlName=" + AStateMachineEditorUnit->ScxmlName;
	}

}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjViewCloseClick(TObject * Sender) {
	if (ProjectManagerView1->SelectedDoc) {
		DoCloseUnit(dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc));
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MoveProjectNodeUpDown(const bool bUp) {
	if (ProjectManagerView1->SelectedDoc && ProjectManagerView1->SelectedDoc->Parent) {
		try {

			TLMDProjNode *AParentProjNode = ProjectManagerView1->SelectedDoc->Parent;

			for (int i = 0; i < AParentProjNode->NodeCount; i++) {
				if (IsUnitModifiedRecursive(AParentProjNode->Nodes[i]))
					throw Exception("Can not perform operation when there are any modified units in [" + //
					AParentProjNode->Nodes[i]->DisplayName + "]!");
			}

			TTreeNode *ATreeNodeParent = ProjectManagerView1->SelectionCount ? ProjectManagerView1->Selections[0]->Parent : NULL;

			int iIndex = -1;
			for (int i = 0; i < AParentProjNode->DocumentCount; i++) {
				if (ProjectManagerView1->SelectedDoc == AParentProjNode->Documents[i]) {
					iIndex = i;
					break;
				}
			}

			if (bUp) {
				if (iIndex < 1)
					return;
			}
			else {
				if (iIndex > (AParentProjNode->DocumentCount - 2))
					return;
			}

			Editorutils::UniversalUpdateLock<TLMDProjectManagerView>ALock(ProjectManagerView1);

			_di_ILMDXmlDocument ADoc = LMDCreateXmlDocument("root", "1.0", "UTF-8");

			/* записываем рекурсивно все подузлы */
			if (TStateMachineEditorUnit * AStateMachineEditorUnit = dynamic_cast<TStateMachineEditorUnit*>(AParentProjNode)) {
				AStateMachineEditorUnit->WriteChildren(ADoc->DocumentElement);
			}
			else if (TStateMachineProject * AStateMachineProject = dynamic_cast<TStateMachineProject*>(AParentProjNode)) {
				AStateMachineProject->WriteChildren(ADoc->DocumentElement);
			}

			// грубая проверка по кол-ву узлов
			if (ADoc->DocumentElement->ChildNodes->Count != AParentProjNode->DocumentCount)
				throw Exception(UnicodeString().sprintf(L"Store XML Generation mismatch! Wait[%d], got[%d]",
					AParentProjNode->DocumentCount, ADoc->DocumentElement->ChildNodes->Count));

			/* меняем местами */
			if (bUp) {
				_di_ILMDXmlNode ACurNode = ADoc->DocumentElement->ChildNodes->Item[iIndex];
				_di_ILMDXmlNode APrevNode = ADoc->DocumentElement->ChildNodes->Item[iIndex - 1];

				ADoc->DocumentElement->RemoveChild(ACurNode);
				ADoc->DocumentElement->InsertBefore(ACurNode, APrevNode);
			}
			else {
				_di_ILMDXmlNode ACurNode = ADoc->DocumentElement->ChildNodes->Item[iIndex];
				_di_ILMDXmlNode ANextNode = ADoc->DocumentElement->ChildNodes->Item[iIndex + 1];

				ADoc->DocumentElement->RemoveChild(ANextNode);
				ADoc->DocumentElement->InsertBefore(ANextNode, ACurNode);
			}

			/* удаляем все дочерние элементы */
			while (AParentProjNode->DocumentCount) {
				if (!RecursiveCloseUnit(AParentProjNode->Documents[0]))
					throw Exception("Can not perform operation on not closed unit! [" + AParentProjNode->Documents[0]->DisplayName + "]");

				AParentProjNode->Documents[0]->Delete();
			}

			/* на этот момент все дочерние должны быть удалены */
			/* считываем их из XML, рекурсивно добавляя */
			if (TStateMachineEditorUnit * AStateMachineEditorUnit = dynamic_cast<TStateMachineEditorUnit*>(AParentProjNode)) {

				AStateMachineEditorUnit->ReadChildren(ADoc->DocumentElement);
			}
			else if (TStateMachineProject * AStateMachineProject = dynamic_cast<TStateMachineProject*>(AParentProjNode)) {

				AStateMachineProject->ReadChildren(ADoc->DocumentElement);
			}

			/* изменение проекта */
			if (StateMachineProject && !StateMachineProject->IsModified(false)) {
				StateMachineProject->MarkModified();
			}

			if (bUp) {
				if (ATreeNodeParent && ATreeNodeParent->Count > (iIndex - 1)) {
					ProjectManagerView1->Select(ATreeNodeParent->Item[iIndex - 1], TShiftState());
				}
			}
			else {
				if (ATreeNodeParent && ATreeNodeParent->Count > (iIndex + 1)) {
					ProjectManagerView1->Select(ATreeNodeParent->Item[iIndex + 1], TShiftState());
				}
			}

		}
		catch(Exception * E) {
			WLOG_ERROR(L"Move Unit %s> %s", bUp ? L"Up" : L"Down", E->Message.c_str());
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjMoveUpClick(TObject *Sender) {
	MoveProjectNodeUpDown(true);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjMoveDownClick(TObject *Sender) {
	MoveProjectNodeUpDown(false);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::FormCloseQuery(TObject * Sender, bool&CanClose) {
	if (SettingsData->IsTesterWorking) {
		String Caption = "WARNING";
		String Text = "Debugging is executed now! Are you sure to quit?";
		if (Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK) {
			actStop->Execute();

			Application->ProcessMessages();
		}
		else {
			CanClose = false;
			return;
		}
	}

	// проверяем и закрываем все открытые панели
	CanClose = ProjCloseQuery();

	// теперь проверим простые текстовые редакторы
	if (CanClose) {
		for (int i = 0; i < this->ComponentCount; i++) {
			TSyntaxDockPanel * ASyntaxDockPanel = dynamic_cast<TSyntaxDockPanel*>(this->Components[i]);
			if (ASyntaxDockPanel && ASyntaxDockPanel->SyntaxForm->IsModified()) {
				const UnicodeString Caption = L"WARNING!";
				const UnicodeString Text = UnicodeString().sprintf(L"Text editor [%s] is modified!\n" //
					"Are you sure to continue?", ASyntaxDockPanel->Caption.c_str());
				CanClose = Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONWARNING) == IDOK;
				if (!CanClose) {
					ASyntaxDockPanel->PanelVisible = true;

					if (ASyntaxDockPanel->Zone && ASyntaxDockPanel->Zone->Parent) {
						ASyntaxDockPanel->Zone->Parent->ActivePage = ASyntaxDockPanel->Zone;
					}
				}
			}
		}
	}

	if (CanClose) {
		try {
			DeleteAllProjects();

			SettingsData->PropInspGutterWidth = PropSettingsInspector->Splitter;
			SettingsData->PropInspInfoHeight = HTMLPropInfo->Height;

			SettingsData->SaveToFile(SettingsData->AppSettingsFile);
			LMDDockManager1->SaveToFile(SettingsData->AppLayoutFile);
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DoCloseUnit(TStateMachineEditorUnit *AUnit) {
	if (AUnit) {
		try {
			std::vector<TStateMachineDockPanel*>ADockPanels;
			int iIndex = -1;
			for (int i = 0; i < this->ComponentCount; i++) {
				TStateMachineDockPanel * AIterDockPanel = dynamic_cast<TStateMachineDockPanel*>(this->Components[i]);
				if (AIterDockPanel) {
					if (AIterDockPanel == AUnit->StateMachineDockPanel) {
						iIndex = ADockPanels.size();
					}
					else {
						ADockPanels.push_back(AIterDockPanel);
					}
				}
			}

			AUnit->Close();

			const int iDockPanelsCount = ADockPanels.size();
			if (iDockPanelsCount) {
				// нулевая - это Welcome Page, поэтому попробуем переключиться на предыдущую или последнюю
				if (iIndex == -1) {
					iIndex = 0;
				}
				else if (iIndex >= iDockPanelsCount) {
					iIndex = iDockPanelsCount - 1;
				}

				if (ADockPanels[iIndex]->Zone && ADockPanels[iIndex]->Zone->Parent) {
					ADockPanels[iIndex]->Zone->Parent->ActivePage = ADockPanels[iIndex]->Zone;
				}

				ADockPanels[iIndex]->Activate(true);
			}

		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actCloseExecute(TObject * Sender) {
	DoCloseUnit(ActiveUnit);
}

// ---------------------------------------------------------------------------
void FillBindingsNodeTree(TTree * ATree, TTreeNodeShape * ANode, ILMDXmlNode * AXMLNode, const UnicodeString & sText) {
	if (AXMLNode) {

		TTreeNodeShape *AChildNode = NULL;

		if (AXMLNode->NodeName == "container") {
			AChildNode = ATree->Items->AddChild(ANode, AXMLNode->GetAttr("NAME", "!!!UNKNOWN!!!"));
			AChildNode->Font->Style = AChildNode->Font->Style << fsBold;
		}
		else if (AXMLNode->NodeName == "item") {
			const UnicodeString sShapeType = AXMLNode->GetAttr("ShapeType");
			const UnicodeString sScxmlName = AXMLNode->GetAttr("ScxmlName", "!!!UNKNOWN!!!");
			const UnicodeString sControlName = AXMLNode->GetAttr("Name", "!!!UNKNOWN!!!");
			const UnicodeString sShapeName = AXMLNode->GetAttr("ShapeName", "!!!UNKNOWN!!!");
			const UnicodeString sDescription = AXMLNode->GetAttr("Description", "");

			UnicodeString sNodeText = sShapeType == "TProtocolWeakBindingShape" ?

				UnicodeString().sprintf(L"%s [%s]", sScxmlName.c_str(), sShapeName.c_str()) :

				UnicodeString().sprintf(L"%s=%s[%s][%s]", sScxmlName.c_str(), sControlName.c_str(), sDescription.c_str(),
				sShapeName.c_str());

			bool bAdd = false;
			int iMaskPos = -1;

			if (!sText.IsEmpty()) {
				const int iMatchPos = sNodeText.UpperCase().Pos(sText.UpperCase());
				const int iMatchLength = sText.Length();

				if (iMatchPos) {
					iMaskPos = iMatchPos;
					bAdd = true;
				}
			}
			else {
				bAdd = true;
			}

			if (bAdd) {
				AChildNode = ATree->Items->AddChild(ANode, sNodeText);
				TMaskedTextShape *AMaskedShape = dynamic_cast<TMaskedTextShape*>(AChildNode);
				if (!AMaskedShape)
					throw Exception("Program logic error! <NodeTree> require <TMaskedTextShape> as node class!");
				AMaskedShape->SetMask(sText, iMaskPos);
			}

		}
		else if (AXMLNode->NodeName == "Config") {
			AChildNode = ATree->Items->AddChild(ANode, "root");
			AChildNode->Font->Style = AChildNode->Font->Style << fsBold;
		}

		if (AChildNode) {
			AChildNode->ImageIndex = tiNone;

			for (int i = 0; i < AXMLNode->ChildNodes->Count; i++) {
				FillBindingsNodeTree(ATree, AChildNode, AXMLNode->ChildNodes->Item[i], sText);
			}

			if (AChildNode->Level() == 0) {
				AChildNode->Expanded = true;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::RecursiveScanUnitBindings(TLMDProjDocument * AProjDocument, TTreeNodeShape * ATreeNode) {
	TStateMachineEditorUnit * AUnit = dynamic_cast<TStateMachineEditorUnit*>(AProjDocument);
	if (AUnit && AUnit->Enabled && !AUnit->BindingsDisabled) {
		ILMDXmlElement *AContainer = AUnit->BindingsContainer;
		if (AContainer && AContainer->ChildNodes->Count) {
			FillBindingsNodeTree(this->TreeViewBindings, ATreeNode, AUnit->BindingsContainer, EditBindings->Text);
		}
	}

	for (int i = 0; i < AProjDocument->DocumentCount; i++) {
		RecursiveScanUnitBindings(AProjDocument->Documents[i], ATreeNode);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DoCollectAllBindings(void) {
	TreeViewBindings->BeginUpdate();
	try {
		if (SettingsData->AviaExtensionEnabled) {
			if (ProjectManager1->Root) {
				TreeViewBindings->Clear();

				TTreeNodeShape *ANodeRoot = TreeViewBindings->Items->AddChild(NULL, "root");
				ANodeRoot->ImageIndex = tiNone;
				ANodeRoot->Font->Style = ANodeRoot->Font->Style << fsBold;

				RecursiveScanUnitBindings(ProjectManager1->Root, ANodeRoot);

				ANodeRoot->Expanded = true;

				if (!EditBindings->Text.IsEmpty())
					TreeViewBindings->FullExpandCollapse(true);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	TreeViewBindings->EndUpdate();
}

// ---------------------------------------------------------------------------
TLMDProjDocument * __fastcall TFormScxmlGui::AddExistingUnit(TLMDProjNode * AParent, const UnicodeString & sFileName, bool bCheckIsOpened,
	const bool bCollectBindings /* = true */ ) {
	if (!FileExists(sFileName))
		throw Exception("File <" + sFileName + "> does not exist!");

	TLMDProjDocument *AEditorForm = NULL;
	if (bCheckIsOpened) {
		AEditorForm = RecursiveSearchOpened(ProjectManager1->Root, sFileName);
	}

	if (!AEditorForm) {
		AEditorForm = dynamic_cast<TLMDProjDocument*>(ProjectManager1->AddExisting(sFileName, dynamic_cast<TLMDProjDocument*>(AParent),
				__classid(TStateMachineEditorUnit)));

		// помечаем, что в проект добавлен юнит
		if (StateMachineProject && !StateMachineProject->IsModified(false)) {
			StateMachineProject->MarkModified();
		}
	}

	if (AEditorForm) {
		if (AEditorForm->Open(true) && bCollectBindings) {
			TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(AEditorForm);
			if (AUnit && AUnit->StateMachineDockPanel) {
				DoCollectAllBindings();
			}
		}
	}

	ProjectManagerView1->FullExpand();

	return AEditorForm;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actAddExistingExecute(TObject * Sender) {
	try {
		OpenDialog->Filter = TStateMachineEditor::GetOpenDialogFilter();
		OpenDialog->FilterIndex = 0;
		if (OpenDialog->Execute(this->Handle)) {

			AddExistingUnit(GetActiveUnit(), OpenDialog->FileName, false);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
TLMDProjDocument * __fastcall TFormScxmlGui::FindAndAddExistingUnit(TLMDProjNode *AParent, const UnicodeString &sFileName,
	bool bCheckIsOpened, const bool bCollectBindings /* = true */ ) {

	UnicodeString sTrueFileName = sFileName;
	if (!FileExists(sTrueFileName)) {
		const UnicodeString Caption = L"WARNING";
		const UnicodeString Text = L"Base unit [" + sFileName + "] is not present in project!\n" //
		"Would you like to find it manually?";
		if (MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING) != IDOK)
			throw Exception("Base file [" + sFileName + "] is not found!");

		OpenDialog->Filter = TStateMachineEditor::GetOpenDialogFilter();
		OpenDialog->FilterIndex = 0;
		OpenDialog->FileName = sTrueFileName;
		if (OpenDialog->Execute(this->Handle)) {
			sTrueFileName = OpenDialog->FileName;
		}
	}

	return AddExistingUnit(AParent, sTrueFileName, bCheckIsOpened, bCollectBindings);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjAddExistingClick(TObject * Sender) {
	try {
		if (ProjectManagerView1->SelectedDoc) {
			OpenDialog->Filter = TStateMachineEditor::GetOpenDialogFilter();
			OpenDialog->FilterIndex = 0;
			if (OpenDialog->Execute(this->Handle)) {

				AddExistingUnit(ProjectManagerView1->SelectedDoc, OpenDialog->FileName, false);

			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjAddAllInvokeSrcClick(TObject * Sender) {
	try {
		if (ProjectManagerView1->SelectedDoc) {

			if (!ProjectManagerView1->SelectedDoc->Opened) {
				ProjectManagerView1->SelectedDoc->Open(true);
			}

			DoAddAllExistingInvokeSrc(ProjectManager1, dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc));

			/* bugfix: по отображению вложенных нод */ ProjectManagerView1->BeginUpdate();
			ProjectManagerView1->EndUpdate();

			DoCollectAllBindings();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjAddExistingUnitWithAllNestedUnitsClick(TObject * Sender) {
	try {
		if (ProjectManagerView1->SelectedDoc) {
			OpenDialog->Filter = TStateMachineEditor::GetOpenDialogFilter();
			OpenDialog->FilterIndex = 0;
			if (OpenDialog->Execute(this->Handle)) {

				TStateMachineEditorUnit *ANewUnit = dynamic_cast<TStateMachineEditorUnit*>
					(AddExistingUnit(ProjectManagerView1->SelectedDoc, OpenDialog->FileName, false, false));
				if (ANewUnit) {
					DoAddAllExistingInvokeSrc(ProjectManager1, ANewUnit);

					/* bugfix: по отображению вложенных нод */ ProjectManagerView1->BeginUpdate();
					ProjectManagerView1->EndUpdate();

					DoCollectAllBindings();
				}

			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actDefaultLayoutExecute(TObject * Sender) {
	try {
		LMDDockManager1->LoadFromFile(SettingsData->AppLayoutDefault);

		CheckForNewDockPanels(this, DockPanelFrameRoot, LMDDockSite1);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::File1Click(TObject * Sender) {
	// сначала убираем все предыдущие
	const int iIndex = File1->IndexOf(MenuRecentLine);
	if (iIndex != -1) {
		for (int i = File1->Count - 1; i > iIndex; i--) {
			delete File1->Items[i];
		}
	}

	int iCount = SettingsData->RecentFiles->Count;
	if (iCount > SettingsData->MaxRecentCount) {
		iCount = SettingsData->MaxRecentCount;
	}
	for (int i = 0; i < iCount; i++) {
		if (!SettingsData->RecentFiles->Strings[i].IsEmpty()) {
			TMenuItem *AMenuItem = new TMenuItem(this);
			AMenuItem->Caption = SettingsData->RecentFiles->Strings[i];
			AMenuItem->Enabled = FileExists(SettingsData->RecentFiles->Strings[i]);

			if (ProjectManager1->Root) {
				AMenuItem->Checked = SettingsData->RecentFiles->Strings[i].UpperCase() == ProjectManager1->Root->FilePath.UpperCase();
			}

			AMenuItem->OnClick = &OnRecentFileClick;
			File1->Add(AMenuItem);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnRecentFileClick(TObject * Sender) {

	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			// файл не открыт
			// AMenuItem->Checked показывает, что этот файл открыт в данный момент
			if (!AMenuItem->Checked) {
				const UnicodeString sFileName = StripHotkey(AMenuItem->Caption);

				const UnicodeString sExt = ExtractFileExt(sFileName).UpperCase();

				if (ProjCloseQuery()) {

					if (TStateMachineProject::FileExt_.LowerCase() == sExt.LowerCase()) {
						DoOpenProject(sFileName);
					}
					else {
						DoOpenUnit(sFileName);
					}
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ProjectManager1FilePathChanged(TObject * Sender, TLMDProjDocument * ADocument) {
	if (ADocument->InheritsFrom(__classid(TStateMachineProject))) {
		SettingsData->AddRecentFileName(ADocument->FilePath);
		UpdateHtmlRecentFiles();
		WLOG_DEBUG(L"Project filename changed to [%s]", ADocument->FilePath.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DoEditSettings(const UnicodeString sItemName /* == L"" */ ) {
	try {

		// копируем объект настроек
		std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
		AMemoryStream->WriteComponent(SettingsData);
		AMemoryStream->Position = 0;

		// отдаем смарт указателю для дальнейшего удаления
		std::auto_ptr<TComponent>AComponentPtr(SettingsData->DoCreateDefaultInstance());
		AMemoryStream->ReadComponent(AComponentPtr.get());
		AComponentPtr->Assign(SettingsData);

		std::auto_ptr<TFormSettingsEditor>AFormSettingsEditor(new TFormSettingsEditor(this, AComponentPtr.get()));
		SettingsData->PropSettingsRegisterCategories(AFormSettingsEditor->PropSettingsInspector); // обязательно перед Arrange change
		AFormSettingsEditor->BtnArrangeType->Down = true;
		AFormSettingsEditor->BtnArrangeTypeClick(AFormSettingsEditor->BtnArrangeType);

		AFormSettingsEditor->SelectItemOrRestoreState(sItemName, FPropInspStateSettingsPtr.get());

		if (AFormSettingsEditor->ShowModal() == mrOk) {

			SettingsData->Assign(AComponentPtr.get());

			SettingsData->Save();
		}

		FPropInspStateSettingsPtr.reset(AFormSettingsEditor->PropSettingsInspector->SaveState());

		// рисуется надпись "EXTENDED", если установлено AviaExtensionEnabled
		ControlBar1->Refresh();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DoEditProjectOptions(const UnicodeString sItemName /* = L"" */ ) {
	if (ProjectManager1->Root) {
		if (StateMachineProject) {
			try {

				// копируем объект настроек
				std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
				AMemoryStream->WriteComponent(StateMachineProject->ProjectProperties);
				AMemoryStream->Position = 0;

				// отдаем смарт указателю для дальнейшего удаления
				std::auto_ptr<TComponent>AComponentPtr(StateMachineProject->ProjectProperties->DoCreateDefaultInstance());
				AMemoryStream->ReadComponent(AComponentPtr.get());
				AComponentPtr->Assign(StateMachineProject->ProjectProperties);

				std::auto_ptr<TFormSettingsEditor>AFormSettingsEditor(new TFormSettingsEditor(this, AComponentPtr.get()));
				AFormSettingsEditor->LabelCaption->Caption = "Edit [" + StateMachineProject->DisplayName + "] properties";

				AFormSettingsEditor->SelectItemOrRestoreState(sItemName, FPropInspStateOptionsPtr.get());

				if (AFormSettingsEditor->ShowModal() == mrOk) {

					const UnicodeString sOldProjectDefines = StateMachineProject->ProjectProperties->ProjectDefines;

					StateMachineProject->ProjectProperties->Assign(AComponentPtr.get());
					StateMachineProject->SaveProperties();

					SetDockPanelStackCaption(StateMachineProject->ProjectProperties->FilteredMsgs->Count);

					if (StateMachineProject->ProjectProperties->ProjectDefines != sOldProjectDefines) {

						std::auto_ptr<TStringList>AOldDefinesPtr(new TStringList());
						AOldDefinesPtr->StrictDelimiter = true;
						AOldDefinesPtr->Delimiter = L';';
						AOldDefinesPtr->DelimitedText = sOldProjectDefines;
						AOldDefinesPtr->CaseSensitive = true;

						std::auto_ptr<TStringList>ANewDefinesPtr(new TStringList());
						ANewDefinesPtr->StrictDelimiter = true;
						ANewDefinesPtr->Delimiter = L';';
						ANewDefinesPtr->DelimitedText = StateMachineProject->ProjectProperties->ProjectDefines;
						ANewDefinesPtr->CaseSensitive = true;

						std::vector<TStateMachineEditorUnit*>AVecUnits;
						CollectAllEnabledUnits(ProjectManager1->Root, AVecUnits);

						bool bWarningForClosed = false;

						for (std::size_t n = 0; n < AVecUnits.size(); n++) {
							if (AVecUnits[n]->StateMachineDockPanel) {
								for (int i = 0; i < AVecUnits[n]->StateMachineDockPanel->StateMachineEditor->TheTree->Shapes->Count; i++) {
									if (TScxmlBaseShape * ABaseShape = dynamic_cast<TScxmlBaseShape*>
										(AVecUnits[n]->StateMachineDockPanel->StateMachineEditor->TheTree->Shapes->Items[i])) {
										if (ABaseShape->AreConditionalDefinesUsed(AOldDefinesPtr.get())
											|| ABaseShape->AreConditionalDefinesUsed(ANewDefinesPtr.get())) {

											AVecUnits[n]->StateMachineDockPanel->MarkModified(1, L"Project definitions were changed!",
												true);

											AVecUnits[n]->StateMachineDockPanel->InvalidateTree();

											break;
										}
									}
								}
							}
							else {
								bWarningForClosed = true;
							}
						}

						if (bWarningForClosed) {
							const UnicodeString sMsg =
								"Closed units are not automatically checked for project definitions! Do it manually!";
							WLOG_WARNING(L"%s", sMsg.c_str());
							SendBallonMsg(sMsg, bfWarning);
						}
					}

					/* надо именно в этом месте, так как иначе дерево не перерисовывается */
					TStateMachineDockPanel *ADockPanelEditor = GetActiveEditorDockPanel();
					if (ADockPanelEditor) {
						ADockPanelEditor->InvalidateTree();
					}

					/* для ProjectDefines */ UpdateCaption();
				}

				FPropInspStateOptionsPtr.reset(AFormSettingsEditor->PropSettingsInspector->SaveState());
			}
			catch(Exception * E) {
				LOG_ERROR(LOG_ERROR_MSG);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actOptionsExecute(TObject * Sender) {
	DoEditProjectOptions();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actOptionsUpdate(TObject * Sender) {
	actOptions->Enabled = StateMachineProject != NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ProjectManagerView1MouseMove(TObject * Sender, TShiftState Shift, int X, int Y) {
	ProjectManagerView1->Hint = L"";
	TTreeNode *ATreeNode = ProjectManagerView1->GetNodeAt(X, Y);
	if (ATreeNode) {

		TLMDProjDocument *ADocument = dynamic_cast<TLMDProjDocument*>(ProjectManagerView1->ConvertTreeNode(ATreeNode));
		if (ADocument) {
			ProjectManagerView1->Hint = ADocument->FilePath;
		}
	}
}

// ---------------------------------------------------------------------------
void EnumerateAllPropItems(TLMDPropertyInspectorItem * AItem, TUnicodeStringMap & AOutMap, const UnicodeString sPath = L"") {
	const UnicodeString sDot = sPath.IsEmpty() ? L"" : L".";
	const UnicodeString sFullPropName = sPath + sDot + AItem->PropName;
#if 0
	WLOG_DEBUG(L"Property:[%s] Val:[%s]", (sPath + sDot + AItem->PropName).c_str(), AItem->Value.c_str());
#endif
	AOutMap[sFullPropName] = AItem->Value;
	for (int i = 0; i < AItem->Count; i++) {
		EnumerateAllPropItems(AItem->Items[i], AOutMap, sPath + sDot + AItem->PropName);
	}
}

// ---------------------------------------------------------------------------
void DebugStringMap(const TUnicodeStringMap & AMap, TStringList * AOutList) {
	for (TUnicodeStringMap::const_iterator it = AMap.begin(); it != AMap.end(); ++it) {
		AOutList->Add(it->first + "=" + it->second);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PropSettingsInspectorChange(TObject * Sender) {
	UnicodeString sModifiedInfo = L"Properties are marked as changed!";

	TUnicodeStringMap APropsMap;
	for (int i = 0; i < PropSettingsInspector->Items->Count; i++) {
		EnumerateAllPropItems(PropSettingsInspector->Items->Items[i], APropsMap);
	}

	bool bChangedTransitionXML = false;

	try {

		for (TUnicodeStringMap::iterator it = FInspectorProps.begin(); it != FInspectorProps.end(); ++it) {

			TUnicodeStringMap::iterator it_new = APropsMap.find(it->first);

			if (it_new != APropsMap.end()) {
				if (it->second != it_new->second) {

					UnicodeString sObject = L"!NULL!";
					if (DesignObjects1->Count && DesignObjects1->Item[0]) {
						if (TCustomTreeElement * AElement = dynamic_cast<TCustomTreeElement*>(DesignObjects1->Item[0])) {
							sObject = AElement->SimpleText;

							TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(AElement);
							if (AStateMachineConnection && it->first == L"XML") {
								bChangedTransitionXML = true;
							}
						}
						else if (TComponent * AComponent = dynamic_cast<TComponent*>(DesignObjects1->Item[0])) {
							sObject = AComponent->Name;
						}
						else {
							sObject = DesignObjects1->Item[0]->ClassName();
						}
					}

					const int iClipMax = 40;

					sModifiedInfo = UnicodeString().sprintf(L"Object[%s] prop[%s] changed to[%s]",
						Customutils::ClippedText(sObject, iClipMax).c_str(), Customutils::ClippedText(it->first, iClipMax).c_str(),
						Customutils::ClippedText(it_new->second, iClipMax).c_str());
#ifdef _DEBUG
					WLOG_DEBUG(L"%s", sModifiedInfo.c_str());
#endif
					break;
				}
			}
			else {
				WLOG_DEBUG(L"Can not find property [%s] for comparison in prop inspector!", it->first.c_str());
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	FInspectorProps.swap(APropsMap);

	if (TransitionXMLEditor->IsDesignObjectsOwner()) {
		TransitionXMLEditor->MarkModified(1, sModifiedInfo, false);
	}
	else {
		TStateMachineDockPanel *ADockPanelEditor = GetActiveEditorDockPanel();
		if (ADockPanelEditor) {
			ADockPanelEditor->MarkModified(1, sModifiedInfo, false);
			ADockPanelEditor->InvalidateTree();

			// этот флаг предполагает, что DesignObjects содержат не менее 1 объекта
			if (bChangedTransitionXML) {
				TransitionXMLEditor->SetStateMachineConnection(dynamic_cast<TStateMachineConnection*>(DesignObjects1->Item[0]));
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actOpenUnitExecute(TObject * Sender) {
	try {
		OpenDialog->Filter = TStateMachineEditor::GetOpenDialogFilter();
		OpenDialog->FilterIndex = 0;
		if (ProjCloseQuery() && OpenDialog->Execute(this->Handle))
			DoOpenUnit(OpenDialog->FileName);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DoOpenUnit(const UnicodeString & sFileName) {
	if (!FileExists(sFileName))
		throw Exception("File <" + sFileName + ">does not exist!");

	DeleteAllProjects();

	try {
		Screen->Cursor = crHourGlass;
		TLMDProjDocument * AEditorForm = dynamic_cast<TLMDProjDocument*>(ProjectManager1->AddExisting(sFileName, NULL,
				__classid(TStateMachineEditorUnit)));

		if (AEditorForm) {
			ProjectManager1->Root->MarkModified();
			AEditorForm->Open(true);
			SettingsData->AddRecentFileName(sFileName);
			UpdateHtmlRecentFiles();
		}
	}
	__finally {
		Screen->Cursor = crDefault;
	}

}

// ---------------------------------------------------------------------------
bool TFormScxmlGui::IsTestingBasedOnProjectSettings(void) {
	return StateMachineProject && StateMachineProject->ProjectProperties->TestSettings->Enabled;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actRunExecute(TObject * Sender) {
	FRunScxmlName = L"";
	FBreakpointSync = false;
	FBreakpointForcedFlag = false;

	actStop->Tag = 0;

	FLockFreeQueue.clear();
	TThread::RemoveQueuedEvents(NULL, OnExternTesterMsgReceived);

	TTestSettings *ATestSettings = SettingsData->ActiveTestSettings;

	if (SettingsData->TestingDebugReceivedBuffer) {
		WLOG_WARNING(L"PERFORMANCE! 'TestingDebugReceivedBuffer' option is enabled!");
	}

	try {

		bool bHasErrors = false;

		TStateMachineDockPanel * AScxmlActivePanel = NULL;

		// сначала попробуем получить док. панельку, указанную пользователем
		TStateMachineProject * AStateMachineProject = this->StateMachineProject;
		if (AStateMachineProject) {

			// необходимо выдать предупреждение для юнитов с одинаковыми машинами состояний
			const bool bSameNames = AStateMachineProject->CheckAllUnitsForSameScxmlNames();
			if (bSameNames) {
				bHasErrors = true;
			}

			// возможно для запуска помечен SCXML, который находится в средине проекта
			TStateMachineEditorUnit *ARootForBuild = AStateMachineProject->RootForBuild;
			if (ARootForBuild) {

				if (!ARootForBuild->Opened) {
					ARootForBuild->Open(true);
				}

				AScxmlActivePanel = ARootForBuild->StateMachineDockPanel;
			}
		}

		// иначе просто текущая активная панель
		if (!AScxmlActivePanel) {
			AScxmlActivePanel = GetActiveEditorDockPanel();
		}

		if (AScxmlActivePanel) {

			if (AScxmlActivePanel->StateMachineEditor->RootScxml)
				FRunScxmlName = AScxmlActivePanel->StateMachineEditor->RootScxml->ScxmlName;

			if (!FExternTestListeningFlag) {
				if (SameText(AScxmlActivePanel->StateMachineEditor->SyntaxScheme, L"Lua") && ContainsText(ATestSettings->ExeName,
						L"QtScxmlTester")) {
					const String Caption = L"WARNING";
					const String Text = UnicodeString().sprintf(L"[%s] does not support <Lua> datamodel.\n" //
						"Switch to [uscxml_tester.exe]?", ExtractFileName(ATestSettings->ExeName).c_str());
					if (Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING) == IDOK) {
						const UnicodeString sNewTester = TTestSettings::GetExeFullPath(USCXML_TESTER_EXE);

						std::unique_ptr<TTestSettings>ATestSettingsPtr(new TTestSettings(tsbtApp));
						ATestSettingsPtr->Enabled = ATestSettings->Enabled;

						ATestSettings->Assign(ATestSettingsPtr.get());

						if (FileExists(sNewTester)) {
							ATestSettings->ExeName = USCXML_TESTER_EXE;
						}
						else {
							ATestSettings->ExeName = USCXML_OLD_TESTER_EXE;
						}
					}
				}
				else if (SameText(AScxmlActivePanel->StateMachineEditor->SyntaxScheme,
						L"ecmascript") && ContainsText(ATestSettings->ExeName, L"uscxml_tester_2")) {
					const String Caption = L"WARNING";
					const String Text = UnicodeString().sprintf(L"[%s] does not support <ecmascript> datamodel.\n" //
						"Switch to [QtScxmlTester.exe]?", ExtractFileName(ATestSettings->ExeName).c_str());
					if (Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING) == IDOK) {

						std::unique_ptr<TTestSettings>ATestSettingsPtr(new TTestSettings(tsbtApp));
						ATestSettingsPtr->Enabled = ATestSettings->Enabled;

						ATestSettings->Assign(ATestSettingsPtr.get());

						ATestSettings->ExeName = QT_SCXML_TESTER_EXE;
					}
				}
			}

			ListViewCallStack->Clear();

			AScxmlActivePanel->ClearEnteredStates();

			FTriggersPrevZone = NULL;
			if (DockPanelTriggers->Zone && DockPanelTriggers->Zone->Parent) {
				// запоминаем предыдущую зону
				FTriggersPrevZone = DockPanelTriggers->Zone->Parent->ActivePage;

				DockPanelTriggers->Zone->Parent->ActivePage = DockPanelTriggers->Zone;
			}

			FCallBackPrevZone = NULL;
			if (DockPanelCallStack->Zone && DockPanelCallStack->Zone->Parent) {
				FCallBackPrevZone = DockPanelCallStack->Zone->Parent->ActivePage;

				DockPanelCallStack->Zone->Parent->ActivePage = DockPanelCallStack->Zone;
			}

			// необходимо создавать Binding, так как, если использовать DefaultPort,
			// то смена порта возможна только через удаление/создание объекта
			if (UDPServer1->Bindings->Count) {
				UDPServer1->Bindings->Items[0]->Port = ATestSettings->LocalPort;
			}
			else {
				UDPServer1->DefaultPort = ATestSettings->LocalPort;
			}
			UDPServer1->OnUDPRead = UDPServer1UDPRead;

			if (FExternTestListeningFlag || ATestSettings->TriggersProtocol == TTriggersProtocol::tpUDP) {
				UDPServer1->Active = true;
				UDPServer1->ThreadedEvent = true;
			}

			ThreadTesterComponent->Active = true;

			SettingsData->IsTesterWorking = true;

			if (StateMachineProject) {
				for (int i = 0; i < StateMachineProject->ProjectProperties->FilteredMsgs->Count; i++) {
					TTestingMsgItem* AMsgFilterItem = StateMachineProject->ProjectProperties->FilteredMsgs->Items[i];
					WLOG_WARNING(L"Msg [%s,%s,%s] is filtered for debugging!", AMsgFilterItem->ScxmlName.c_str(),
						AMsgFilterItem->Msg.c_str(), ScxmlMsgTypeToString(AMsgFilterItem->Type).c_str());
				}
			}

			// если не в режиме внешнего тестирования
			if (!FExternTestListeningFlag) {
				ScrollBoxTriggers->Visible = false;

				while (ScrollBoxTriggers->ControlCount) {
					delete ScrollBoxTriggers->Controls[0];
				}

				for (int i = 0; i < this->ComponentCount; i++) {
					TStateMachineDockPanel *ADockPanel = dynamic_cast<TStateMachineDockPanel*>(this->Components[i]);
					if (ADockPanel) {

						const UnicodeString sUnitName = ADockPanel->StateMachineEditorUnit ?
							ADockPanel->StateMachineEditorUnit->FileName : ADockPanel->Caption;

						const int iOldHeight = Customutils::GetWinControlAlignedChildrenHeight(ScrollBoxTriggers);

						for (int k = 0; k < ADockPanel->StateMachineEditor->TheTree->Connections->Count; k++) {
							TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
								(ADockPanel->StateMachineEditor->TheTree->Connections->Items[k]);
							if (AConnection && !AConnection->Event.IsEmpty()) {
								CreateTrigger(ScrollBoxTriggers, AConnection->ContentTrigger, AConnection->Params);
							}
						}

						for (int k = 0; k < ADockPanel->StateMachineEditor->TheTree->Shapes->Count; k++) {
							// глобально проверим на факт того, что не исключено из теста
							if (TScxmlBaseShape * AShape = dynamic_cast<TScxmlBaseShape*>
								(ADockPanel->StateMachineEditor->TheTree->Shapes->Items[k])) {
								if (AShape->ExcludeFromSave) {
									continue;
								}
							}

							if (TProtocolBindingShape * AProtocolBindingShape = dynamic_cast<TProtocolBindingShape*>
								(ADockPanel->StateMachineEditor->TheTree->Shapes->Items[k])) {
								CreateTrigger(ScrollBoxTriggers, AProtocolBindingShape->ContentTrigger, AProtocolBindingShape->Params);
							}
							else if (TTriggerShape * ATriggerShape = dynamic_cast<TTriggerShape*>
								(ADockPanel->StateMachineEditor->TheTree->Shapes->Items[k])) {
								CreateTrigger(ScrollBoxTriggers, ATriggerShape->ContentTrigger, ATriggerShape->Params);
							}
							else if (TWatchShape * AWatchShape = dynamic_cast<TWatchShape*>
								(ADockPanel->StateMachineEditor->TheTree->Shapes->Items[k])) {

								if (!AWatchShape->RegExp.IsEmpty()) {

									bool bWatchLabelExists = false;

									for (int n = 0; n < ScrollBoxTriggers->ControlCount; n++) {
										TWatchLabel * ACurrentWatchLabel = dynamic_cast<TWatchLabel*>(ScrollBoxTriggers->Controls[n]);
										if (ACurrentWatchLabel) {
											if
												(ACurrentWatchLabel->WatchLabel == AWatchShape->Label &&
												ACurrentWatchLabel->RegExp == AWatchShape->RegExp) {
												bWatchLabelExists = true;
												break;
											}
										}
									}

									// проверяем, чтобы не дублировать с одинаковыми значениями,
									// например, при наследованиии
									if (!bWatchLabelExists) {
										TWatchLabel *AWatchLabel = new TWatchLabel(ScrollBoxTriggers, AWatchShape->Label,
											AWatchShape->RegExp);
										AWatchLabel->Name = TeeGetUniqueName(ScrollBoxTriggers, "WatchLabel");
										if (!AWatchShape->Label.IsEmpty()) {
											AWatchLabel->Caption = AWatchShape->Label + L":";
										}

										AWatchLabel->Align = alTop;
										AWatchLabel->AlignWithMargins = true;
										AWatchLabel->Top = Customutils::GetWinControlAlignedChildrenHeight(ScrollBoxTriggers);

										AWatchLabel->Parent = ScrollBoxTriggers;
									}

								}
							}
						}

						// здесь проверяем, не добавилось ли что-то новое
						if (Customutils::GetWinControlAlignedChildrenHeight(ScrollBoxTriggers) != iOldHeight) {
							TLabel *ALabel = new TLabel(ScrollBoxTriggers);
							ALabel->Caption = sUnitName;
							ALabel->Align = alTop;
							ALabel->AlignWithMargins = true;
							ALabel->Alignment = taCenter;
							ALabel->Font->Style = ALabel->Font->Style << fsBold;
							ALabel->Top = iOldHeight;
							ALabel->Parent = ScrollBoxTriggers;
						}
					}
				}

				if (StateMachineProject) {
					const int iOldHeight = Customutils::GetWinControlAlignedChildrenHeight(ScrollBoxTriggers);

					for (int i = 0; i < StateMachineProject->ProjectProperties->Triggers->Count; i++) {
						CreateTrigger(ScrollBoxTriggers, StateMachineProject->ProjectProperties->Triggers->Items[i]->ContentTrigger,
							StateMachineProject->ProjectProperties->Triggers->Items[i]->Params);
					}

					// здесь проверяем, не добавилось ли что-то новое
					if (Customutils::GetWinControlAlignedChildrenHeight(ScrollBoxTriggers) != iOldHeight) {
						TLabel *ALabel = new TLabel(ScrollBoxTriggers);
						ALabel->Caption = L"Project Triggers";
						ALabel->Align = alTop;
						ALabel->AlignWithMargins = true;
						ALabel->Alignment = taCenter;
						ALabel->Font->Style = ALabel->Font->Style << fsBold;
						ALabel->Top = iOldHeight;
						ALabel->Parent = ScrollBoxTriggers;
					}
				}

				ScrollBoxTriggers->Visible = true;

				DModule->UDPClient1->Port = ATestSettings->RemotePort;

				UnicodeString sTempFile = AScxmlActivePanel->OpenedDoc->Node->FilePath + "~";
				// возможно это новый проект
				if (!TPath::IsPathRooted(sTempFile)) {
					sTempFile = TPath::Combine(SettingsData->AppPath, sTempFile);
				}

				DeleteFile(sTempFile);

				try {
					AScxmlActivePanel->StateMachineEditor->SaveRawScxmlToFile(sTempFile);
				}
				catch(EStateMachineConnectionException * E) {
					AScxmlActivePanel->ActivateZonePage();
					AScxmlActivePanel->StateMachineEditor->SelectConnection(E->StateMachineConnection, SCROLL_CHART_IN_VIEW);
					AScxmlActivePanel->StateMachineEditor->SelectTemporaryRectangle
						(Editorutils::IncrementRectCopy(E->StateMachineConnection->GetBounds(), 15),
						"Error: " + E->Category);
					throw E;
				}
				catch(EScxmlBaseShapeException * E) {
					AScxmlActivePanel->ActivateZonePage();
					AScxmlActivePanel->StateMachineEditor->SelectShape(E->Shape, SCROLL_CHART_IN_VIEW);
					AScxmlActivePanel->StateMachineEditor->SelectTemporaryRectangle(Editorutils::IncrementRectCopy(E->Shape->Bounds(), 15),
						"Error: " + E->Category);
					throw E;
				}
				catch(EScxmlDuplicateStateIDException * E) {
					AScxmlActivePanel->ActivateZonePage();
					AScxmlActivePanel->StateMachineEditor->HandleExceptionDuplicateStateID(E);
					throw E;
				}

				TFileAttributes AFileAttr = TFile::GetAttributes(sTempFile);
				TFile::SetAttributes(sTempFile, AFileAttr << Ioutils::faHidden);

				UnicodeString sCMD = ATestSettings->FullPath;
				if (ATestSettings->UseCustomParamsOnly) {
					sCMD = sCMD + " " + TTestSettings::GetExpandedCMD(ATestSettings->CMDParams, sTempFile);
				}
				else {
					if (ATestSettings->ScxmlMonitorLog) {
						sCMD = sCMD + " -scxmlcoutlog true";
					}
					if (ATestSettings->StopOnExit) {
						sCMD = sCMD + " -exitstop true";
					}
					if (ATestSettings->CheckIssues) {
						sCMD = sCMD + " -issue true";
					}
					if (ATestSettings->HideDebugInfo) {
						sCMD = sCMD + " -hidedebuginfo true";
					}
					if (ATestSettings->HttpEnabled) {
						sCMD = sCMD + UnicodeString().sprintf(L" -usehttp true -httplistenport %d -httpwebsocketport %d",
							ATestSettings->HttpListenPort, ATestSettings->HttpWebSocket);
					}
					sCMD = UnicodeString().sprintf(L"%s -r %d -l %d -f %s", sCMD.c_str(), ATestSettings->LocalPort,
						ATestSettings->RemotePort, AnsiQuotedStr(sTempFile, '"').c_str());
					sCMD = sCMD + " " + ATestSettings->CMDParams;
				}

				WLOG_DEBUG(L"Tester CMD: %s", sCMD.c_str());

				FormScxmlGui->SwitchToTesterLog(true /* clear */ );

				FExternTesterSpawnPtr.reset(new TSpawnAndRedirect(sCMD, TTestSettings::GetExpandedCMD(ATestSettings->WorkDir, sTempFile),
						this->Handle, WM_SCXML_STOP_DEBUGGING));
				FExternTesterSpawnPtr->ConvertOutputEncoding = ATestSettings->ConvertOutputEncoding;

				if (ATestSettings->TriggersProtocol == TTriggersProtocol::tpPipes) {
					FExternTesterSpawnPtr->InitWrite();
				}

				// ускоряем, чтобы лишний раз не проверять каждую строку вывода
				if (IsTesterReadLineEventUsed()) {
					FExternTesterSpawnPtr->OnLineRead = OnWatchLineReadEvent;
				}

				FExternTesterSpawnPtr->StartProcess();
			}
			else {
				FormScxmlGui->SwitchToProgramLog( /* not clear */ );
				const UnicodeString sMsg = UnicodeString().sprintf(L"Start listening from external application.\nActive unit:[%s]",
					AScxmlActivePanel->StateMachineEditorUnit ? AScxmlActivePanel->StateMachineEditorUnit->FileName.c_str()
					: AScxmlActivePanel->ScxmlName.c_str());

				// иначе спрячет сообщение об ошибках
				if (!bHasErrors) {
					SendBallonMsg(sMsg, bfInfo);
				}

				WLOG_INFO(L"%s", sMsg.c_str());
			}

			InvalidateAllEditors();
		}
		else
			LOG_WARNING_SS << "Run> No active panel!";
	}
	catch(Exception * E) {
		std::map<TStateMachineEditor*, TTemporarySelectedRectangle>AWasMap;

		for (int i = 0; i < this->ComponentCount; i++) {
			TStateMachineDockPanel *APanel = dynamic_cast<TStateMachineDockPanel*>(this->Components[i]);
			if (APanel && APanel->StateMachineEditor) {
				AWasMap.insert(std::make_pair(APanel->StateMachineEditor, APanel->StateMachineEditor->GetTemporarySelectedRectangle()));
			}
		}

		WLOG_ERROR(L"RUN> %s", E->Message.c_str());
		// понаблюдаем, не приведет ли к конфликтам
		actStop->Execute();

		for (std::map<TStateMachineEditor*, TTemporarySelectedRectangle>::iterator it = AWasMap.begin(); it != AWasMap.end(); ++it) {
			it->first->SetTemporarySelectedRectangle(it->second);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::SendPipeEvent(const UnicodeString &sMsg) {
	if (FExternTesterSpawnPtr.get()) {
		FExternTesterSpawnPtr->WriteLine(sMsg + L"\r\n");
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TFormScxmlGui::IsTesterReadLineEventUsed(void) {
	if (SettingsData->ActiveTestSettings->CaptureOutputRegExps->Enabled) {
		return true;
	}
	for (int i = 0; i < ScrollBoxTriggers->ComponentCount; i++) {
		TWatchLabel * AWatchLabel = dynamic_cast<TWatchLabel*>(ScrollBoxTriggers->Components[i]);
		if (AWatchLabel && !AWatchLabel->RegExp.IsEmpty()) {
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnWatchLineReadEvent(const UnicodeString &sText) {

	std::size_t nMsgQueueSize = 0;

	TTestSettings *ATestSettings = SettingsData->ActiveTestSettings;
	if (ATestSettings->CaptureOutputRegExps->Enabled) {
		for (int i = 0; i < ATestSettings->CaptureOutputRegExps->Count; i++) {
			UnicodeString sIdentifier = L"";
			UnicodeString sMachine = FRunScxmlName;
#if 0 // для глубокой отладки
			WLOG_DEBUG(L"Capture=%s", sText.c_str());
#endif
			if (ATestSettings->CaptureOutputRegExps->Items[i]->Parse(sText, sMachine, sIdentifier)) {
				const int iType = ATestSettings->CaptureOutputRegExps->Items[i]->CaptureMsgType;

				nMsgQueueSize = FLockFreeQueue.enqueue(UnicodeString().sprintf(L"%d@%s@%s", iType, sMachine.c_str(),
						sIdentifier.c_str()).c_str());
			}
		}
	}

	for (int i = 0; i < ScrollBoxTriggers->ComponentCount; i++) {
		TWatchLabel * AWatchLabel = dynamic_cast<TWatchLabel*>(ScrollBoxTriggers->Components[i]);
		if (AWatchLabel) {
			AWatchLabel->Parse(sText);
		}
	}

	if (nMsgQueueSize && FBreakpointSync) {
		PostMessage(this->Handle, WM_SCXML_UPDATE_DBG_MSG_QUEUE, nMsgQueueSize, 0);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actGetActivePanelAndTestingUpdate(TObject * Sender) {
	actRun->Enabled = GetActiveEditorDockPanel() != NULL && !SettingsData->IsTesterWorking;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnDeleteObjects(TMessage & msg) {
	if (msg.LParam == 0)
		return;

	for (int i = 0; i < this->ComponentCount; i++) {
		if ((LPARAM)this->Components[i] == msg.LParam) {
			// заморочка с тем, чтобы при закрытии переключило на предыдущую вкладку
			TLMDDockPanel *APanel = dynamic_cast<TLMDDockPanel*>(this->Components[i]);
			if (APanel && APanel->Zone && APanel->Zone->Parent) {
				for (int i = 0; i < APanel->Zone->Parent->TabCount; i++) {
					if (i == APanel->Zone->Index - 1) {
						APanel->Zone->Parent->ActivePage = APanel->Zone->Parent->Tabs[i].Zone;
						break;
					}
				}
			}

			delete this->Components[i];
			break;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnStopDebugging(TMessage & msg) {
	actStop->Tag = 1;

	if (FLockFreeQueue.empty()) {
		actStop->Execute();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actStopExecute(TObject * Sender) {
	try {
		actStop->Tag = 0;

		if (FBreakpointForcedFlag) {
			if (actPause->Checked) {
				actPause->Checked = false;
			}
		}

		FBreakpointSync = false;
		FBreakpointForcedFlag = false;

		FSyntaxXMLView->ClearBreakpoints();

		if (!FLockFreeQueue.empty()) {
			TThread::RemoveQueuedEvents(NULL, OnExternTesterMsgReceived);
		}

		if (FExternTesterSpawnPtr.get()) {
			FExternTesterSpawnPtr->StopProcess();
			FExternTesterSpawnPtr.reset(NULL);
		}

		SettingsData->IsTesterWorking = false;

		UDPServer1->Active = false;
		ThreadTesterComponent->TerminateAndWaitFor();

		// могут поменяться хэндлы зон
		// делаем поиск через список компонентов,
		// т.к. DockSite после того, как панели отцеплены в отдельные окна, не хранит указатели на них
		for (int i = 0; i < this->ComponentCount; i++) {
			TLMDDockPanel *ADockPanel = dynamic_cast<TLMDDockPanel*>(this->Components[i]);
			if (ADockPanel) {
				ADockPanel->Name;

				if (FTriggersPrevZone && ADockPanel->Zone == FTriggersPrevZone && FTriggersPrevZone->Parent) {
					FTriggersPrevZone->Parent->ActivePage = FTriggersPrevZone;
				}
				else if (FCallBackPrevZone && ADockPanel->Zone == FCallBackPrevZone && FCallBackPrevZone->Parent) {
					FCallBackPrevZone->Parent->ActivePage = FCallBackPrevZone;
				}

				TStateMachineDockPanel *APanel = dynamic_cast<TStateMachineDockPanel*>(ADockPanel);
				if (APanel) {
					APanel->ClearEnteredStates();

					if (APanel->OpenedDoc->Node) {
						const UnicodeString sTempFile = APanel->OpenedDoc->Node->FilePath + "~";
						if (FileExists(sTempFile)) {
							if (!DeleteFile(sTempFile))
								WLOG_WARNING(L"Can not delete <%s>", sTempFile.c_str());
						}
					}
				}
			}
		}

		PropSettingsInspector->UpdateContent();
	}
	catch(Exception * E) {
		WLOG_ERROR(L"STOP> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void SetListViewCanvasSettings(TCustomListView * Sender, TListItem * Item) {
	if (Item->SubItems->Count < 1)
		return;
	if (Item->SubItems->Strings[0] == "BeforeEnter") {
		Sender->Canvas->Brush->Color = TColor(0x00D5FFEA);
	}
	else if (Item->SubItems->Strings[0] == "BeforeExit") {
		Sender->Canvas->Brush->Color = TColor(0x00D2D2FF);
	}
	else {
		Sender->Canvas->Brush->Color = clWindow;
	}

	if (Item->Index == 0) {
		Sender->Canvas->Brush->Color = clInactiveCaption;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ListViewCallStackAdvancedCustomDrawSubItem(TCustomListView * Sender, TListItem * Item, int SubItem,
	TCustomDrawState State, TCustomDrawStage Stage, bool&DefaultDraw) {
	if (SubItem == 2) {
		Sender->Canvas->Font->Style = TFontStyles() << fsBold;
	}
	else {
		Sender->Canvas->Font->Style = TFontStyles();
	}

	SetListViewCanvasSettings(Sender, Item);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ListViewCallStackAdvancedCustomDrawItem(TCustomListView * Sender, TListItem * Item, TCustomDrawState State,
	TCustomDrawStage Stage, bool&DefaultDraw) {

	SetListViewCanvasSettings(Sender, Item);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actIsTesterWorkingUpdate(TObject * Sender) {
	TCustomAction *ACustomAction = dynamic_cast<TCustomAction*>(Sender);
	if (ACustomAction) {
		ACustomAction->Enabled = !SettingsData->IsTesterWorking;
	}
	else {
		WLOG_ERROR(L"Can not cast <%s> to <TCustomAction>", Sender ? Sender->ClassName().c_str() : L"NULL");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ParseReceivedUDPTesterCommand(const std::wstring & wstr) {
	try {

		/* PERFORMANCE WARNING !!! Only for deep debug */
		if (SettingsData->TestingDebugReceivedBuffer) {
			WLOG_DEBUG(L"Received UDP:[%s]", wstr.c_str());
		}

		/* Clear All command */
		if (wstr.find(L"@@@") == 0) {

			UnicodeString sScxmlName = L"";
			UnicodeString sInvokeID = L"";

			std::auto_ptr<TStringList>AParseListPtr(new TStringList());
			AParseListPtr->StrictDelimiter = true;
			AParseListPtr->Delimiter = L'@';
			AParseListPtr->DelimitedText = StringReplace(wstr.c_str(), L"@@@", L"", TReplaceFlags()).Trim();

			for (int i = 0; i < AParseListPtr->Count; i++) {
				switch(i) {
				case 0: {
						sScxmlName = AParseListPtr->Strings[i];
					}break;
				case 1: {
						sInvokeID = AParseListPtr->Strings[i];
					}break;
				default:
					break;
				}
			}

			for (int i = 0; i < this->ComponentCount; i++) {
				TStateMachineDockPanel *APanel = dynamic_cast<TStateMachineDockPanel*>(this->Components[i]);
				if (APanel) {
					if (sScxmlName.IsEmpty() || //
						(APanel->ScxmlName == sScxmlName && //
							(APanel->InvokeID.IsEmpty() || APanel->InvokeID == sInvokeID)))

						APanel->ClearEnteredStates();
				}
			}

			return;
		}

		/* Process All commands */
		boost::wregex e(L"^(\\d+)@([^@]*)@([^@]*)@*(.*)$");
		boost::wsmatch what;

		if (boost::regex_match(wstr, what, e)) {
			if (what.size() == 5) {
				const TScxmlMsgType AType = TScxmlMsgType(StrToInt(std::wstring(what[1]).c_str()));
				if (SettingsData->CallStackTypes.Contains(AType)) {
					const UnicodeString sScxmlName(std::wstring(what[2]).c_str());
					const UnicodeString sMsg(std::wstring(what[3]).c_str());
					const UnicodeString sIdentifier(std::wstring(what[4]).c_str());

					UnicodeString sDesc = ScxmlMsgTypeToString(AType);
					sDesc = StringReplace(sDesc, "smtt", "", TReplaceFlags());

					UnicodeString sTransitionFrom = L"";
					int iTransitionID = -1;

					/* Since ScxmlEditor 2.3 */
					if (AType == smttBeforeTakingTransition) {
						std::auto_ptr<TStringList>ATransitionListPtr(new TStringList());
						ATransitionListPtr->Delimiter = L'|';
						ATransitionListPtr->StrictDelimiter = true;
						ATransitionListPtr->DelimitedText = sMsg;
						if (ATransitionListPtr->Count == 2) {
							sTransitionFrom = ATransitionListPtr->Strings[0];
							iTransitionID = ATransitionListPtr->Strings[1].ToIntDef(-1);
						}
					}

					if (StateMachineProject) {
						if (StateMachineProject->ProjectProperties->FilteredMsgs->IndexOfAnyMatched( //
								sMsg, sScxmlName, sIdentifier, AType) != -1) {
							return;
						}
					}

					// флаг, который установится, если для фигуры игнорируется Дебаг
					bool bDebugIgnore = false;

					std::vector<TStateMachineEditorUnit*>AVecUnits;

					// в этом случае визуально показываем выделение
					if (AType == smttBeforeEnter || AType == smttBeforeExit || AType == smttBeforeUnInvoke || //
						// we are ignoring anonymous from states
						(AType == smttBeforeTakingTransition && !sTransitionFrom.IsEmpty())) {
						// так как могут быть октрыты сторонние юниты или не определено имя, поэтому по умолчанию активный редактор
						TStateMachineDockPanel *ADockPanel = GetActiveEditorDockPanel();

						CollectAllEnabledUnits(FormScxmlGui->ProjectManager1->Root, AVecUnits);
						for (std::size_t i = 0; i < AVecUnits.size(); i++) {
							const UnicodeString &sUnitName = AVecUnits[i]->ScxmlName;
							if (sUnitName == sScxmlName) {
								if (!AVecUnits[i]->Opened) {
									// иногда не нужно открывать редакторы при отладке, поэтому воспользуемся флагом
									if (SettingsData->TestingAutoOpenUnit) {
										AVecUnits[i]->Open(true);
									}
									else {
										// в таком случае просто игнорируем это сообщение
										ADockPanel = NULL;
										break;
									}
								}

								if (AVecUnits[i]->StateMachineDockPanel) {

									// найден нужный редактор
									ADockPanel = AVecUnits[i]->StateMachineDockPanel;

									break;
								}
							}
						}

						bool bRequireProcessing = false;
						if (ADockPanel && !sMsg.IsEmpty()) {
							TScxmlShape *ARootScxml = ADockPanel->StateMachineEditor->RootScxml;
							if (ARootScxml) {

								if (AType == smttBeforeUnInvoke) {

									TStateMachineEditorUnit *AUninvokedUnit = NULL;
									for (std::size_t i = 0; i < AVecUnits.size(); i++) {
										const UnicodeString &sUnitName = AVecUnits[i]->ScxmlName;
										if (sUnitName == sMsg) {
											if (TStateMachineDockPanel * ASubDockPanel = AVecUnits[i]->StateMachineDockPanel) {
												if (TScxmlShape * ASubRootScxml = ASubDockPanel->StateMachineEditor->RootScxml) {
													if (ASubRootScxml->InvokeID == sIdentifier) {
														AUninvokedUnit = AVecUnits[i];
														ASubRootScxml->TempInvokeID = L"";
														break;
													}
												}
											}
										}
									}

									if (AUninvokedUnit) {
										for (std::size_t i = 0; i < AVecUnits.size(); i++) {
											if (AVecUnits[i] != AUninvokedUnit && //
												AUninvokedUnit->VirtualDependencies->IndexOf(AVecUnits[i]->FileName) != -1) {

												if (TStateMachineDockPanel * ASubDockPanel = AVecUnits[i]->StateMachineDockPanel) {
													ASubDockPanel->ClearEnteredStates();
												}
											}
										}
									}

								}
								else {
									/* происходит след. штука - по умолчанию, если нет ID, то запоминаем первый попавшийся во временную переменную */
									if (!sIdentifier.IsEmpty() && ARootScxml->InvokeID.IsEmpty()) {
										ARootScxml->TempInvokeID = sIdentifier;
									}

									if (ARootScxml->InvokeID.IsEmpty() || sIdentifier == ARootScxml->InvokeID) {
										bRequireProcessing = true;
									}
								}

							}
						}

						/* пока запрещаем пустые ID */
						if (bRequireProcessing) {

							TVisualScxmlBaseShape *AVisualShape = NULL;
							TStateMachineConnection *AConnection = NULL;

							switch(AType) {
							case smttBeforeEnter: {

									std::pair<TStateMachineDockPanel*,
									TTreeNodeShape*>AEnteredElements = ADockPanel->EnterStateNode(sMsg, true,
										SettingsData->TestingAutoOpenUnit, L"");

									ADockPanel = AEnteredElements.first;
									AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AEnteredElements.second);

									if (DockPanelScxmlView->PanelVisible) {
										FSyntaxXMLView->SetBreakpointToStateId(sMsg);
									}

									const bool bIsBreakpointSet = AVisualShape && AVisualShape->BreakpointSet;

									if (bIsBreakpointSet) {
										SendBallonMsg(UnicodeString().sprintf(L"Breakpoint for state:[%s] machine:[%s] is activated!",
												sMsg.c_str(), ADockPanel->ScxmlName.c_str()), bfInfo);

										if (!actPause->Checked) {
											FBreakpointForcedFlag = true;

											this->InvalidateAllEditors();

											actPause->Checked = true;
										}
										actPause->Update();
									}

									if (actPause->Checked) {
										FBreakpointSync = true;
										actNextStep->Enabled = true;
										actNextStep->Update();
									}
								}break;
							case smttBeforeExit: {

									std::pair<TStateMachineDockPanel*,
									TTreeNodeShape*>AEnteredElements = ADockPanel->EnterStateNode(sMsg, false,
										SettingsData->TestingAutoOpenUnit, L"");

									ADockPanel = AEnteredElements.first;
									AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AEnteredElements.second);
								}break;
							case smttBeforeTakingTransition: {

									std::pair<TStateMachineDockPanel*,
									TTreeConnection*>AEnteredElements = ADockPanel->TakingTransition(sTransitionFrom, iTransitionID,
										SettingsData->TestingAutoOpenUnit, L"");

									ADockPanel = AEnteredElements.first;
									AConnection = dynamic_cast<TStateMachineConnection*>(AEnteredElements.second);

									const bool bIsBreakpointSet = AConnection && AConnection->BreakpointSet;

									if (bIsBreakpointSet) {
										SendBallonMsg(UnicodeString().sprintf(L"Breakpoint for state:[%s] machine:[%s] is activated!",
												sMsg.c_str(), ADockPanel->ScxmlName.c_str()), bfInfo);

										if (!actPause->Checked) {
											FBreakpointForcedFlag = true;

											this->InvalidateAllEditors();

											actPause->Checked = true;
										}
										actPause->Update();
									}

									if (actPause->Checked) {
										FBreakpointSync = true;
										actNextStep->Enabled = true;
										actNextStep->Update();
									}

								}break;
							}

							// если определена фигура, и установлены разрешения, то переключим на соотв. док. панель
							if (AVisualShape || AConnection) {

								if (ADockPanel && (SettingsData->TestingSwitchToPanel && ADockPanel->Zone && ADockPanel->Zone->Parent) &&
									(ADockPanel->Zone->Parent->ActivePage != ADockPanel->Zone)) {
									// в этом методе будет переключено на передний план и обновиться SyntaxView

									ADockPanel->Show();
								}
							}
							else {

								bDebugIgnore = true;
							}

						}
					} /* End of Enter|Exit */

					if (!bDebugIgnore) {

						const bool b_IS_GIF_MAKER_ACTIVE = FrameGifMaker && FrameGifMaker->IsRecordMode();

						/* ПРЕДУПРЕЖДЕНИЕ! Эта операция очень затратная по ресурсам и времени,
						поэтому выполнять только по запросу! */
						// GIF IMAGE
						if (b_IS_GIF_MAKER_ACTIVE && (AType == smttBeforeEnter || AType == smttBeforeTakingTransition)) {

							TStateMachineDockPanel *ADockPanel = GetActiveEditorDockPanel();
							if (ADockPanel && (ADockPanel->ScxmlName == sScxmlName) && //
								(ADockPanel->InvokeID.IsEmpty() || ADockPanel->InvokeID == sIdentifier)) {

								const UnicodeString sImageName = (AType == smttBeforeEnter ? L"Enter: " : L"Transition: ") + sMsg;

								FrameGifMaker->CaptureImage(ADockPanel->StateMachineEditor->TheTree,
									ADockPanel->StateMachineEditor->RootScxml, AType, sImageName);

							}
						} /* End of GIF */

						/* CallStack */
						if (SettingsData->TestingShowCallStack) {
							TListItem *AItem = ListViewCallStack->Items->Insert(0);
							AItem->Caption = FormatDateTime(L"hh:mm:ss.zzz", Now());
							AItem->SubItems->Add(sDesc);
							AItem->SubItems->Add(sMsg);
							AItem->SubItems->Add(sScxmlName);
							AItem->SubItems->Add(sIdentifier);
						}
					} /* End of !DebugIgnore */
				}
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnExternTesterMsgReceived(void) {
	ParseReceivedUDPTesterCommand(FLockFreeQueue.dequeue());

	if (FLockFreeQueue.empty()) {
		if (actStop->Tag) {
			PostMessage(this->Handle, WM_SCXML_STOP_DEBUGGING, 0, 0);
		}
	}
	else {
		PostMessage(this->Handle, WM_SCXML_UPDATE_DBG_MSG_QUEUE, FLockFreeQueue.size(), 0);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ThreadTesterComponentRun(TIdThreadComponent * Sender) {
	while (!Sender->Terminated) {
		// если не стоит на точке останова
		if (!FBreakpointSync) {
			if (!FLockFreeQueue.empty()) {
				Sender->Synchronize(OnExternTesterMsgReceived);
				continue;
			}
		}

		// задержка именно в этом месте, что будет означать
		// что не было обработано никаких сообщений из очереди
		Sleep(1);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::UDPServer1UDPRead(TIdUDPListenerThread * AThread, TBytes AData, TIdSocketHandle * ABinding) {
	if (AData.Length > 1) {
		const RawByteString str(reinterpret_cast<char*>(&AData[0]), AData.Length);
		const std::size_t nSize = FLockFreeQueue.enqueue(UTF8ToUnicodeString(str).c_str());

		if (FBreakpointSync) {
			PostMessage(this->Handle, WM_SCXML_UPDATE_DBG_MSG_QUEUE, nSize, 0);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnMsgUpdateDebugQueue(TMessage &msg) {
	if (LabelDbgMsgCount->Tag != msg.WParam) {
		LabelDbgMsgCount->Tag = msg.WParam;

		const UnicodeString sMsg = L"Msg queue: " + UnicodeString(msg.WParam);
		ControlBar1->Canvas->Font->Assign(LabelDbgMsgCount->Font);
		const int iRawWidth = ControlBar1->Canvas->TextWidth(sMsg);
		// может возвращать 0 !!!
		if (iRawWidth > 10) {
			LabelDbgMsgCount->Width = iRawWidth + 5;
		}

		LabelDbgMsgCount->Caption = sMsg;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actAppSettingsExecute(TObject * Sender) {
	DoEditSettings();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actNewUnitExecute(TObject * Sender) {
	try {

		if (ProjCloseQuery()) {

			DeleteAllProjects();

			TStateMachineEditorUnit *AEditorForm = dynamic_cast<TStateMachineEditorUnit*>(ProjectManager1->New(NULL,
					__classid(TStateMachineEditorUnit)));
			if (AEditorForm) {
				AEditorForm->Open(true);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actExportToRawScxmlExecute(TObject * Sender) {
	try {
		TStateMachineDockPanel * AScxmlActivePanel = GetActiveEditorDockPanel();
		if (AScxmlActivePanel) {
			SaveDialog->Filter = TStateMachineEditorUnit::OpenDialogFilter;
			if (SaveDialog->Execute(this->Handle)) {
				AScxmlActivePanel->StateMachineEditor->SaveRawScxmlToFile(SaveDialog->FileName);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actExportToHPPExecute(TObject * Sender) {
	try {
		TStateMachineDockPanel * AScxmlActivePanel = GetActiveEditorDockPanel();
		if (AScxmlActivePanel) {
			SaveDialog->Filter = L"HPP files(*.hpp)|*.hpp|All files(*.*)|*.*";
			if (SaveDialog->Execute(this->Handle)) {
				AScxmlActivePanel->StateMachineEditor->SaveRawScxmlToHPP(SaveDialog->FileName);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::TabControlLogChange(TObject * Sender) {
	switch(TabControlLog->TabIndex) {
	case 0:
		EditViewLog->Document = EditDocDebug;
		break;
	case 1:
		EditViewLog->Document = EditDocTester;
		break;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ElDropTarget1TargetDrag(TObject * Sender, TDragState State, TOleDragObject * Source, TShiftState Shift,
	int X, int Y, TDragType & DragType) {

	switch(Source->Content) {
	case edcFileList: {
			for (int i = 0; i < Source->FileList->Count; i++) {
				// у проектов приоритет
				const UnicodeString sExt = ExtractFileExt(Source->FileList->Strings[i]).LowerCase();
				if (sExt == TStateMachineProject::FileExt_ && actOpenProject->Enabled) {
					DragType = dtCopy;
				}
				else if (sExt == TStateMachineEditorUnit::FileExt_) {
					// бросаем в дерево
					if (DockPanelProjectTree->Active && !SettingsData->IsTesterWorking) {
						DragType = dtCopy;
					}
					else if (actAddExisting->Enabled) {
						DragType = dtCopy;
					}
				}
			}
		}break;
	case edcText:
	case edcOther: {
			if (SettingsData->AviaExtensionEnabled) {
				TStateMachineDockPanel * AScxmlActivePanel = GetActiveEditorDockPanel();
				if (AScxmlActivePanel) {
					UnicodeString sText = L"";
					if (IsAviaClipboardData(Source, sText)) {
						DragType = dtCopy;
					}
				}
			}
		}break;
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::ElDropTarget1TargetDrop(TObject * Sender, TOleDragObject * Source, TShiftState Shift, int X, int Y,
	TDragType & DragType)

{
	try {
		switch(Source->Content) {
		case edcFileList: {
				TStateMachineEditorUnit *AProjectActiveUnit = GetActiveUnit();

				TLMDProjDocument *AProjSelectedDoc = ProjectManagerView1->SelectedDoc ?
					ProjectManagerView1->SelectedDoc : ProjectManager1->Root;
				const bool bDropToProjectTree = DockPanelProjectTree->Active;
				for (int i = 0; i < Source->FileList->Count; i++) {
					// у проектов приоритет
					const UnicodeString sExt = ExtractFileExt(Source->FileList->Strings[i]).LowerCase();
					if (sExt == TStateMachineProject::FileExt_ && actOpenProject->Enabled) {
						const UnicodeString sFileName = Source->FileList->Strings[i];
						if (ProjCloseQuery())
							DoOpenProject(sFileName);

						break;
					}
					else if (sExt == TStateMachineEditorUnit::FileExt_) {
						if (ProjectManager1->Root && ProjectManager1->Root->ClassType() == __classid(TStateMachineProject)) {
							// бросаем в дерево
							if (bDropToProjectTree && AProjSelectedDoc && !SettingsData->IsTesterWorking) {
								AddExistingUnit(AProjSelectedDoc, Source->FileList->Strings[i], false);
							}
							else if (AProjectActiveUnit && !SettingsData->IsTesterWorking) {
								AddExistingUnit(AProjectActiveUnit, Source->FileList->Strings[i], true);
							}
						}
						else {
							DoOpenUnit(Source->FileList->Strings[i]);
							break;
						}
					}
				}
			}break;
		case edcText:
		case edcOther: {
				if (SettingsData->AviaExtensionEnabled) {
					TStateMachineDockPanel * AScxmlActivePanel = GetActiveEditorDockPanel();
					if (AScxmlActivePanel) {
						UnicodeString sText = L"";
						if (IsAviaClipboardData(Source, sText)) {
							DragType = dtCopy;
#ifdef _DEBUG
							WLOG_DEBUG(L"%s", sText.c_str());
#endif

							TScxmlBaseShape *AParentShape = AScxmlActivePanel->StateMachineEditor->FirstSelected;

							if (!AParentShape) {
								AParentShape = AScxmlActivePanel->StateMachineEditor->RootScxml;
							}

							std::vector<TProtocolBindingShape*>AProtocolShapes = CreateProtocolBindingShapeFromJSON(AParentShape, sText);
							// имеет смысл выделять, если вообще хоть что-то добавлено
							if (AProtocolShapes.size()) {

								// необходимо вызвать для того, чтобы сформировались размеры фигур при отрисовке
								AScxmlActivePanel->StateMachineEditor->TheTree->Refresh();

								TRect ABounds(0, 0, 0, 0);
								for (std::size_t i = 0; i < AProtocolShapes.size(); i++) {
									if (i == 0) {
										ABounds = AProtocolShapes[i]->Bounds();
									}
									else {
										if (AProtocolShapes[i]->X0 < ABounds.Left) {
											ABounds.Left = AProtocolShapes[i]->X0;
										}
										if (AProtocolShapes[i]->Y0 < ABounds.Top) {
											ABounds.Top = AProtocolShapes[i]->Y0;
										}
										if (AProtocolShapes[i]->X1 > ABounds.Right) {
											ABounds.Right = AProtocolShapes[i]->X1;
										}
										if (AProtocolShapes[i]->Y1 > ABounds.Bottom) {
											ABounds.Bottom = AProtocolShapes[i]->Y1;
										}
									}
								}

								Editorutils::IncrementRect(ABounds, 5);

								AScxmlActivePanel->StateMachineEditor->SelectTemporaryRectangle(ABounds);

								AScxmlActivePanel->StateMachineEditor->CenterInView(AProtocolShapes.back());
							}

						}
					}
				}
			}break;
		}

	}
	catch(Exception * E) {
		WLOG_ERROR(L"DRAG'n'DROP> %s", E->Message.c_str());
	}

}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actShowCallStackExecute(TObject * Sender) {
	if (SettingsData) {
		SettingsData->TestingShowCallStack = actShowCallStack->Checked;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actShowCallStackUpdate(TObject * Sender) {
	if (SettingsData) {
		actShowCallStack->Checked = SettingsData->TestingShowCallStack;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ApplicationEvents1Activate(TObject * Sender) {
	// при переключении активности на наше приложение,
	// дать команду всем плавающим окнам переместиться на передний план
	if (!FApplicationMinimizedFlag) {
		for (int i = 0; i < LMDDockManager1->SiteCount; i++) {
			TLMDFloatingForm*AForm = LMDDockManager1->Sites[i]->FloatingForm;
			if (LMDDockManager1->Sites[i]->IsFloatingSite && AForm && AForm->Showing) {
				const TRect &ARect = AForm->BoundsRect;
				SetWindowPos(AForm->Handle, this->Handle, ARect.Left, ARect.Top, ARect.Width(), ARect.Height(),
					SWP_SHOWWINDOW | SWP_NOACTIVATE);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ApplicationEvents1Minimize(TObject *Sender) {
	FApplicationMinimizedFlag = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ApplicationEvents1Restore(TObject *Sender) {
	FApplicationMinimizedFlag = false;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::UpdateHtmlRecentFiles() {
	HTMLRecent->Clear();

	HTMLRecent->BeginUpdate();

	TTreeNodeShape *ARecentlyProjectsNode = HTMLRecent->Items->AddChild(NULL, "Recently opened projects");
	ARecentlyProjectsNode->Text->Add(L"");
	ARecentlyProjectsNode->Font->Size = 14;
	ARecentlyProjectsNode->Font->Style = ARecentlyProjectsNode->Font->Style << fsBold;

	try {
		int iCount = 0;

		int i = 0;
		while (i < SettingsData->RecentFiles->Count) {
			if (!FileExists(SettingsData->RecentFiles->Strings[i])) {
				WLOG_WARNING(L"Recent file <%s> is not found!", SettingsData->RecentFiles->Strings[i].c_str());
				SettingsData->RecentFiles->Delete(i);
			}
			else {
				if (SameText(ExtractFileExt(SettingsData->RecentFiles->Strings[i]), TStateMachineProject::FileExt_)) {
					UnicodeString sRef = UnicodeString().sprintf(L"<A href=\"%s\">%s</A>", SettingsData->RecentFiles->Strings[i].c_str(),
						SettingsData->RecentFiles->Strings[i].c_str());

					const int iActive = ProjectManager1->Root && SameText(SettingsData->RecentFiles->Strings[i],
						ProjectManager1->Root->FilePath);

					TAnchorTextShape * AnchorTextShape = dynamic_cast<TAnchorTextShape*>(HTMLRecent->Items->AddChild(NULL, L""));
					if (AnchorTextShape) {
						AnchorTextShape->SetFilePath(SettingsData->RecentFiles->Strings[i], iCount + 1, iActive);
						AnchorTextShape->Font->Size = 10;
					}

					iCount++;
				}
				i++;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	TTreeNodeShape *ARecentlyUnitsNode = HTMLRecent->Items->AddChild(NULL, "Recently opened units");
	ARecentlyUnitsNode->Text->Insert(0, L"");
	ARecentlyUnitsNode->Text->Add(L"");

	ARecentlyUnitsNode->Font->Size = 14;
	ARecentlyUnitsNode->Font->Style = ARecentlyUnitsNode->Font->Style << fsBold;

	try {
		int iCount = 0;

		for (int i = 0; i < SettingsData->RecentFiles->Count; i++) {
			if (!SameText(ExtractFileExt(SettingsData->RecentFiles->Strings[i]), TStateMachineProject::FileExt_)) {
				UnicodeString sRef = UnicodeString().sprintf(L"<A href=\"%s\">%s</A>", SettingsData->RecentFiles->Strings[i].c_str(),
					SettingsData->RecentFiles->Strings[i].c_str());

				const int iActive = ProjectManager1->Root && SameText(SettingsData->RecentFiles->Strings[i],
					ProjectManager1->Root->FilePath);

				TAnchorTextShape * AnchorTextShape = dynamic_cast<TAnchorTextShape*>(HTMLRecent->Items->AddChild(NULL, L""));
				if (AnchorTextShape) {
					AnchorTextShape->SetFilePath(SettingsData->RecentFiles->Strings[i], iCount + 1, iActive);
					AnchorTextShape->Font->Size = 10;
				}

				iCount++;
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	HTMLRecent->EndUpdate();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DockPanelFrameRootEnter(TObject * Sender) {
	UpdateHtmlRecentFiles();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSearchExecute(TObject * Sender) {
	if (!FormSearch) {
		FormSearch = new TFormSearch(this);
	}

	FormSearch->FromSearchTypes(SettingsData->SearchTypes);
	FormSearch->SetEditorSearchType(SettingsData->SearchDocumentType);
	FormSearch->CheckShowResultsInSeparateWindow->Checked = false;
	FormSearch->Show();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSearchAllExecute(TObject * Sender) {
	if (!FormSearch) {
		FormSearch = new TFormSearch(this);
	}

	FormSearch->FromSearchTypes(SettingsData->SearchTypes);
	FormSearch->SetEditorSearchType(SettingsData->SearchDocumentType);
	FormSearch->CheckShowResultsInSeparateWindow->Checked = true;
	FormSearch->Show();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSearchUpdate(TObject * Sender) {
	actSearch->Enabled = ProjectManager1->Root != NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSaveFlatExecute(TObject * Sender) {
	try {
		TStateMachineEditorUnit* AUnit = GetActiveUnit();
		if (AUnit) {
			const UnicodeString &sFilePath = AUnit->FilePath;
			Statemachine::SaveScxmlAsFlat(sFilePath, ChangeFileExt(sFilePath, ".flat.scxml"));
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::ApplicationEvents1Exception(TObject * Sender, Exception * E) {

	SwitchToProgramLog();

	// допустимые исключения
	std::set<TMetaClass*>ASetIgnoredExceptions;
	ASetIgnoredExceptions.insert(__classid(EHelpSystemException));
	ASetIgnoredExceptions.insert(__classid(EConvertError));
	ASetIgnoredExceptions.insert(__classid(ELMDPropEditor));

	static int i_HARD_EXCEPTION_COUNT = 0;

	UnicodeString sErr = L"ERROR";
	if (ASetIgnoredExceptions.find(E->ClassType()) == ASetIgnoredExceptions.end()) {
		sErr = L"GENERAL ERROR";

		LOG_ERROR_SS << "WE STRONGLY RECOMMEND TO SAVE ALL AND RESTART THE PROGRAM!!!";
		LOG_ERROR_SS << "WE STRONGLY RECOMMEND TO SAVE ALL AND RESTART THE PROGRAM!!!";
		LOG_ERROR_SS << "WE STRONGLY RECOMMEND TO SAVE ALL AND RESTART THE PROGRAM!!!";

		i_HARD_EXCEPTION_COUNT++;
	}

	if (i_HARD_EXCEPTION_COUNT > 50) {
		const UnicodeString Caption = L"CRITICAL ERROR!";
		const UnicodeString Text = //
		L"Too many critical errors!!!\n" //
		"It seems that something abnormal is going on!\n" //
		"We can't do anything except terminating an application, sorry ... \n";
		"See log for details.";

		MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OK | MB_ICONERROR);

		try {
			this->Close();
		}
		catch(...) {
			Application->Terminate();
		}
	}

	WLOG_ERROR(L"%s:[%s] ErrClass:[%s]", sErr.c_str(), E->Message.c_str(), E->ClassName().c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actExportToDFMExecute(TObject * Sender) {
	try {
		TStateMachineEditorUnit* AUnit = GetActiveUnit();
		if (AUnit && AUnit->StateMachineDockPanel) {
			const UnicodeString &sFilePath = AUnit->FilePath;
			AUnit->StateMachineDockPanel->StateMachineEditor->SaveScxmlToPas(ChangeFileExt(sFilePath, ".scxml.pas"));
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actBuildProjectExecute(TObject * Sender) {
	try {
		if (StateMachineProject) {
			if (StateMachineProject->ProjectProperties->BuildTypes.Contains(pbtSaveAll)) {
				actSaveAll->Execute();
			}
			if (StateMachineProject->ProjectProperties->BuildTypes.Contains(pbtSaveFlat)) {
				// или RootForBuild, или первый доступный Unit
				TStateMachineEditorUnit *AUnit = StateMachineProject->ProjectRoot;
				if (AUnit) {
					const UnicodeString &sFilePath = AUnit->FilePath;
					Statemachine::SaveScxmlAsFlat(sFilePath, ChangeFileExt(sFilePath, ".flat.scxml"));
				}
			}
			if (StateMachineProject->ProjectProperties->BuildTypes.Contains(pbtPostBuild)) {
				const UnicodeString sTempDir = ExtractFilePath(StateMachineProject->FilePath);
				const UnicodeString sTempFileName = L"~" + ExtractFileName(StateMachineProject->FilePath) + L".cmd";
				const UnicodeString sTempFile = TPath::Combine(sTempDir, sTempFileName);

				DeleteFile(sTempFile);

				if (!StateMachineProject->ProjectProperties->ProjectPostBuild->PostBuildEvaluated.IsEmpty()) {
					try {
						SwitchToTesterLog(true);

						this->EditDocTester->Lines->Append("Post build started>>>");
						this->EditDocTester->Lines->Append("*********************");

						// записываем скрипт в формате OEM 866, чтобы правильно кириллица читалась
						std::auto_ptr<TStringList>APostBuildPtr(new TStringList());
						APostBuildPtr->Text = StateMachineProject->ProjectProperties->ProjectPostBuild->PostBuildEvaluated;
						APostBuildPtr->SaveToFile(sTempFile, TEncoding::GetEncoding(866));

						TFileAttributes AFileAttr = TFile::GetAttributes(sTempFile);
						TFile::SetAttributes(sTempFile, AFileAttr << Ioutils::faHidden);

						// запуск скрипта на исполнение
						TSpawnAndRedirect ASpawner(sTempFile, ExtractFileDir(sTempFile));
						ASpawner.ConvertOutputEncoding = coeOEM866;
						ASpawner.StartProcessInfinite();
					}
					__finally {
						DeleteFile(sTempFile);
					}
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actExternTestingExecute(TObject * Sender) {
	FExternTestListeningFlag = !FExternTestListeningFlag;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actExternTestingUpdate(TObject *Sender) {
	actExternTesting->Checked = FExternTestListeningFlag;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actStackClearExecute(TObject * Sender) {
	ListViewCallStack->Items->Clear();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSetPanelForegroundUpdate(TObject * Sender) {
	actSetPanelForeground->Checked = SettingsData->TestingSwitchToPanel;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actSetPanelForegroundExecute(TObject * Sender) {
	SettingsData->TestingSwitchToPanel = !SettingsData->TestingSwitchToPanel;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::InvalidateAllEditors(void) {
	for (int i = 0; i < this->ComponentCount; i++) {
		TStateMachineDockPanel *APanel = dynamic_cast<TStateMachineDockPanel*>(this->Components[i]);
		if (APanel) {
			APanel->InvalidateTree();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::InvalidateAllEditorsAndResetDebug(void) {
	for (int i = 0; i < this->ComponentCount; i++) {
		TStateMachineDockPanel *APanel = dynamic_cast<TStateMachineDockPanel*>(this->Components[i]);
		if (APanel && APanel->StateMachineEditor) {
			APanel->StateMachineEditor->ResetActiveDebugShape();
			APanel->InvalidateTree();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actStopUpdate(TObject * Sender) {
	actStop->Enabled = SettingsData->IsTesterWorking;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actPauseExecute(TObject * Sender) {
	FBreakpointForcedFlag = false;
	InvalidateAllEditors();
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actPauseUpdate(TObject * Sender) {
	actPause->Enabled = GetActiveEditorDockPanel() != NULL;

	if (ToolButton11->Down != actPause->Checked) {
		ToolButton11->Down = actPause->Checked;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actNextStepExecute(TObject * Sender) {
	FBreakpointSync = false;

	InvalidateAllEditors();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actNextStepUpdate(TObject * Sender) {
	if (actNextStep->Enabled != FBreakpointSync) {
		actNextStep->Enabled = FBreakpointSync;

		if (!FBreakpointSync && SettingsData->IsTesterWorking) {

			if (FBreakpointForcedFlag) {
				FBreakpointForcedFlag = false;
				actPause->Checked = false;
			}
			InvalidateAllEditors();
		}
	}

	LabelDbgMsgCount->Visible = actNextStep->Enabled;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actShapeScrollToViewExecute(TObject * Sender) {
	SettingsData->TestingScrollChartToViewShape = !SettingsData->TestingScrollChartToViewShape;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actShapeScrollToViewUpdate(TObject * Sender) {
	actShapeScrollToView->Checked = SettingsData->TestingScrollChartToViewShape;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actClearEnteredStatesExecute(TObject * Sender) {
	TStateMachineDockPanel *APanel = GetActiveEditorDockPanel();
	if (APanel) {
		APanel->ClearEnteredStates();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnConvertedToAnsiC(TMessage & msg) {
	FConvertToAnsiCSpawnPtr.reset();

	WLOG_INFO(L"Scxml compiled to [%s]", actExportAnsiC->Dialog->FileName.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actExportAnsiCAccept(TObject * Sender) {
	try {
		if (FConvertToAnsiCSpawnPtr.get()) {
			String Caption = "WARNING!";
			String Text = "Previous operation wasn't finished! Are you sure to continue?";
			if (Application->MessageBoxA(Text.w_str(), Caption.w_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) != IDOK) {
				return;
			}
		}

		TStateMachineEditorUnit* AUnit = GetActiveUnit();
		if (AUnit && AUnit->StateMachineDockPanel) {
			const UnicodeString sTransformer = TPath::Combine(SettingsData->AppPath, "uscxml-transform.exe");
			FConvertToAnsiCSpawnPtr.reset(new TSpawnAndRedirect(UnicodeString().sprintf(L"%s -tc -i \"%s\" -o \"%s\"",
						sTransformer.c_str(), AUnit->FilePath.c_str(), actExportAnsiC->Dialog->FileName.c_str()), SettingsData->AppPath,
					this->Handle, WM_SCXML_ANSI_C_CONVERTED));
			FConvertToAnsiCSpawnPtr->StartProcess();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnMsgCopyData(TWMCopyData &msg) {
	if (msg.CopyDataStruct && msg.CopyDataStruct->cbData == sizeof(INTERPROCESS_COPYDATA)) {
		INTERPROCESS_COPYDATA *ACopyData = reinterpret_cast<INTERPROCESS_COPYDATA*>(msg.CopyDataStruct->lpData);
		if (ACopyData) {
			WLOG_INFO(L"MAIN_FORM> Copy data command:[%s] arg1:[%s] arg2:[%s] arg3:[%s]", //
				ACopyData->wchCommand, ACopyData->wchArg1, ACopyData->wchArg2, ACopyData->wchArg3);

			try {
				/* Commands related to Active Editor */
				TStateMachineDockPanel* ADockPanel = ActiveEditorDockPanel;
				if (ADockPanel) {
					ADockPanel->StateMachineEditor->ExecuteInterprocessCommand(ACopyData);
				}

				/* Commands related to Main Form */
				if (SameText(ACopyData->wchCommand, L"SwitchLog")) {

					const bool bClear = StrToBoolDef(ACopyData->wchArg2, false);

					if (SameText(ACopyData->wchArg1, L"Debug")) {
						this->SwitchToProgramLog(bClear);
					}
					else if (SameText(ACopyData->wchArg1, L"CMD")) {
						this->SwitchToTesterLog(bClear);
					}
				}
			}
			catch(Exception * E) {
				WLOG_ERROR(L"MAIN_FORM> %s", E->Message.c_str());
			}

		}
	}
	else {
		WLOG_ERROR(L"Invalid WM_COPYDATA");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnConvertedToQt(TMessage & msg) {
	FConvertToQtSpawnPtr.reset();

	WLOG_INFO(L"Scxml compiled to [%s]", actExportQt->Dialog->FileName.c_str());
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actExportQtAccept(TObject * Sender) {
	try {
		TStateMachineEditorUnit* AUnit = GetActiveUnit();
		if (AUnit && AUnit->StateMachineDockPanel) {
			if (FConvertToQtSpawnPtr.get()) {
				String Caption = "WARNING!";
				String Text = "Previous operation wasn't finished! Are you sure to continue?";
				if (Application->MessageBoxA(Text.w_str(), Caption.w_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) != IDOK) {
					return;
				}
			}

			const UnicodeString sOutpath = TPath::Combine(ExtractFilePath(actExportQt->Dialog->FileName),
				TPath::GetFileNameWithoutExtension(actExportQt->Dialog->FileName));

			std::auto_ptr<TDialogQScxmlCompiler>ADialogPtr(new TDialogQScxmlCompiler(this));
			ADialogPtr->ComboOutName->Text = sOutpath;
			if (ADialogPtr->ShowModal() == mrOk) {
				const UnicodeString sTransformer = TPath::Combine(SettingsData->AppPath, "QtScxmlTester\\qscxmlc.exe");

				UnicodeString sCMD = UnicodeString().sprintf(L"%s \"%s\" -o \"%s\"", sTransformer.c_str(), AUnit->FilePath.c_str(),
					ADialogPtr->ComboOutName->Text.IsEmpty() ? sOutpath.c_str() : ADialogPtr->ComboOutName->Text.c_str());

				if (ADialogPtr->CheckNamespace->Checked && !ADialogPtr->ComboNamespace->Text.IsEmpty()) {
					sCMD = sCMD + " --namespace " + AnsiQuotedStr(ADialogPtr->ComboNamespace->Text, L'"');
				}
				if (ADialogPtr->CheckHeader->Checked && !ADialogPtr->ComboHeader->Text.IsEmpty()) {
					sCMD = sCMD + " --header " + AnsiQuotedStr(ADialogPtr->ComboHeader->Text, L'"');
				}
				if (ADialogPtr->CheckImpl->Checked && !ADialogPtr->ComboImpl->Text.IsEmpty()) {
					sCMD = sCMD + " --impl " + AnsiQuotedStr(ADialogPtr->ComboImpl->Text, L'"');
				}
				if (ADialogPtr->CheckClassName->Checked && !ADialogPtr->ComboClassName->Text.IsEmpty()) {
					sCMD = sCMD + " --classname " + AnsiQuotedStr(ADialogPtr->ComboClassName->Text, L'"');
				}
				if (ADialogPtr->CheckStateMethods->Checked) {
					sCMD = sCMD + " --statemethods";
				}

				FConvertToQtSpawnPtr.reset(new TSpawnAndRedirect(sCMD.c_str(), SettingsData->AppPath, this->Handle, WM_SCXML_QT_CONVERTED));
				FConvertToQtSpawnPtr->StartProcess();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::EditBindingsChange(TObject * Sender) {
	DoCollectAllBindings();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::SelectBinding(const bool bScrollInView) {
	TTreeNodeShape *ASelectedShape = GetSafeTreeListFirst(TreeViewBindings->Selected->Shapes);
	if (ASelectedShape && ASelectedShape->Level() == 2) {
		const UnicodeString sStrippedText = ASelectedShape->SimpleText;

		try {
			boost::wregex e(L"^.*\\[([^\\]]*)\\]$");
			boost::wsmatch what;

			std::wstring wstr = sStrippedText.c_str();
			if (boost::regex_match(wstr, what, e)) {
				if (what.size() == 2) {
					const UnicodeString sShapeName = std::wstring(what[1]).c_str();

					std::vector<TStateMachineEditorUnit*>AVecUnits;
					CollectAllEnabledUnits(FormScxmlGui->ProjectManager1->Root, AVecUnits);

					const UnicodeString sParentText = ASelectedShape->Parent->SimpleText;

					for (std::size_t i = 0; i < AVecUnits.size(); i++) {

						if (SameText(AVecUnits[i]->FileName, sParentText)) {

							if (!AVecUnits[i]->Opened) {
								AVecUnits[i]->Open(true);
							}

							if (AVecUnits[i]->StateMachineDockPanel) {

								if
									((bScrollInView && AVecUnits[i]->StateMachineDockPanel->Zone && AVecUnits[i]
										->StateMachineDockPanel->Zone->Parent) &&
									(AVecUnits[i]->StateMachineDockPanel->Zone->Parent->ActivePage != AVecUnits[i]
										->StateMachineDockPanel->Zone)) {
									// в этом методе будет переключено на передний план и обновится SyntaxView
									AVecUnits[i]->StateMachineDockPanel->Show();
								}

								TComponent *AComponent = AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->FindComponent(sShapeName);
								if (!AComponent)
									throw Exception("Can not find component with name [" + sShapeName + "]");

								TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(AComponent);
								if (AStateMachineConnection) {
									// дизайн объекты можно предварительно не чистить в этом методе само почистит
									AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->SelectConnection(AStateMachineConnection,
										bScrollInView);
									return;
								}

								TTreeNodeShape * ATreeNodeShape = dynamic_cast<TTreeNodeShape*>(AComponent);
								if (ATreeNodeShape) {
									// дизайн объекты можно предварительно не чистить в этом методе само почистит
									AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->SelectShape(ATreeNodeShape, bScrollInView);
									// дополнительно выделим фигуру, так как иногда сложно сразу найти
									AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->SelectTemporaryRectangle
										(Editorutils::IncrementRectCopy(ATreeNodeShape->Bounds(), 10));
									return;
								}
							}

							return;
						}

					}

					throw Exception("Unit with FileName [" + sParentText + "] is not found!");
				}
			}

			throw Exception("Regular expression is not matched! [" + sStrippedText + "]");
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::TreeViewBindingsDblClick(TObject * Sender) {
	SelectBinding(SCROLL_CHART_IN_VIEW);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PropSettingsInspectorClick(TObject * Sender) {
	try {
		TLMDPropertyInspector * ALMDPropertyInspector = dynamic_cast<TLMDPropertyInspector*>(Sender);
		if (ALMDPropertyInspector && ALMDPropertyInspector->ActiveItem && ALMDPropertyInspector->Selection &&
			ALMDPropertyInspector->Selection->Count) {

			HTMLPropInfo->Items->Clear();

			// это элемент инспектора, он знает только RTTI информацию о свойстве
			TLMDPropertyInspectorItem *AItem = ALMDPropertyInspector->ActiveItem;
			const UnicodeString sPropName = ALMDPropertyInspector->ActiveItem->PropName;

			// первый объект из списка объектов, которое редактируется инспектором в данный момент
			TObject *AInstance = ALMDPropertyInspector->Selection->Item[0];

			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->StrictDelimiter = true;
			AStringListPtr->Delimiter = L'.';

			while (AItem) {
				AStringListPtr->Insert(0, AItem->PropName);
				AItem = AItem->Parent;
			}

			if (!CheckFilterProp->Checked) {
				EditPropInspPath->Text = AStringListPtr->DelimitedText;
			}

			// обязательно на -1, так как если будет ActiveItem == tkClass, то мы неверно определим Instance
			for (int i = 0; i < AStringListPtr->Count - 1; i++) {

				if (AInstance && //
					(PropIsType(AInstance, AStringListPtr->Strings[i], tkClass) || PropIsType(AInstance, AStringListPtr->Strings[i],
							tkClassRef)) //
					) {
					AInstance = GetObjectProp(AInstance, AStringListPtr->Strings[i]);
				}
				else {
					break;
				}
			}

			if (AInstance) {
				HTMLPropInfo->Items->Text = Propinspext::PropSettingsInspectorGetHTMLInfo(Sender, AInstance, sPropName);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuLuaviaHelpClick(TObject * Sender) {
	if (!TFormSimpleHelp::IsDockPanelFound()) {
		if (!FormSimpleHelp) {
			FormSimpleHelp = new TFormSimpleHelp(this);
		}
		FormSimpleHelp->Show();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::Hotkeys1Click(TObject * Sender) {
	std::auto_ptr<TFormHotKeysHelp>AFormHotKeysHelpPtr(new TFormHotKeysHelp(this));
	AFormHotKeysHelpPtr->ShowModal();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PopupMenuBindingsPopup(TObject * Sender) {

	CopyScxmlNametoClipboard1->Visible = false;
	CopyAviaNametoClipboard1->Visible = false;

	TTreeNodeShape *ASelectedShape = GetSafeTreeListFirst(TreeViewBindings->Selected->Shapes);
	if (ASelectedShape && ASelectedShape->Level() == 2) {
		const UnicodeString sStrippedText = ASelectedShape->SimpleText;
		const std::wstring wstr = sStrippedText.c_str();

		try {
			const boost::wregex e(L"^([^=]*).*\\[[^\\]]*\\]$");
			boost::wsmatch what;

			if (boost::regex_match(wstr, what, e) && what.size() == 2) {
				const UnicodeString sScxmlName = std::wstring(what[1]).c_str();

				CopyScxmlNametoClipboard1->Caption = UnicodeString().sprintf(L"Copy [%s] to Clipboard", sScxmlName.Trim().c_str());

				CopyScxmlNametoClipboard1->Visible = true;
			}
			else
				throw Exception("Regular expression is not matched! [" + sStrippedText + "]");
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}

		// если WeakBinding, то может и не быть контрола
		try {
			const boost::wregex e(L"^[^=]*=([^\\[]*).*$");
			boost::wsmatch what;

			if (boost::regex_match(wstr, what, e) && what.size() == 2) {

				const UnicodeString sName = std::wstring(what[1]).c_str();

				CopyAviaNametoClipboard1->Caption = UnicodeString().sprintf(L"Copy [%s] to Clipboard", sName.Trim().c_str());

				CopyAviaNametoClipboard1->Visible = true;
			}
			else
				throw Exception("Regular expression is not matched! [" + sStrippedText + "]");
		}
		catch(Exception * E) {
			LOG_DEBUG(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::SelectWithoutScrollChartClick(TObject * Sender) {
	SelectBinding(DO_NOT_SCROLL_CHART_IN_VIEW);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::CopyNametoClipboard1Click(TObject * Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (!AMenuItem)
			throw Exception("Can not cast to <TMenuItem>");

		const boost::wregex e(L"^Copy \\[([^\\[\\]]+)\\] to Clipboard$");
		boost::wsmatch what;

		const std::wstring wstr = StripHotkey(AMenuItem->Caption).c_str();
		if (boost::regex_match(wstr, what, e)) {
			if (what.size() == 2) {
				const UnicodeString sName = std::wstring(what[1]).c_str();

				Clipboard()->AsText = sName.Trim();

				return; // ВНИМАНИЕ!!! ВЫХОД
			}
		}

		throw Exception("Regular expression is not matched! [" + StripHotkey(AMenuItem->Caption) + "]");
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actPresetsConditionExecute(TObject * Sender) {
	try {
		TStateMachineDockPanel * AScxmlActivePanel = GetActiveEditorDockPanel();
		if (AScxmlActivePanel && AScxmlActivePanel->StateMachineEditor) {
			AScxmlActivePanel->StateMachineEditor->AddShapesFromPresetCondition();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actPresetsOnOffExecute(TObject * Sender) {
	try {
		TStateMachineDockPanel * AScxmlActivePanel = GetActiveEditorDockPanel();
		if (AScxmlActivePanel && AScxmlActivePanel->StateMachineEditor) {
			AScxmlActivePanel->StateMachineEditor->AddShapesFromPresetOnOff();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjPathToClipboardClick(TObject * Sender) {
	if (ProjectManagerView1->SelectedDoc) {
		try {
			Clipboard()->AsText = ProjectManagerView1->SelectedDoc->FilePath;
			WLOG_DEBUG(L"Path <%s> copied to clipboard", ProjectManagerView1->SelectedDoc->FilePath.c_str());
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjFileNameToClipboardClick(TObject * Sender) {
	if (ProjectManagerView1->SelectedDoc) {
		try {
			const UnicodeString sFileName = ExtractFileName(ProjectManagerView1->SelectedDoc->FilePath);
			Clipboard()->AsText = sFileName;
			WLOG_DEBUG(L"FileName <%s> copied to clipboard", sFileName.c_str());
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjUnitScxmlNameClick(TObject * Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			const UnicodeString sScxmlName = StringReplace(StripHotkey(AMenuItem->Caption), L"ScxmlName=", L"", TReplaceFlags());
			Clipboard()->AsText = sScxmlName;
			WLOG_DEBUG(L"ScxmlName <%s> copied to clipboard", sScxmlName.c_str());
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjEditPostSaveClick(TObject * Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit) {
			std::auto_ptr<TMacroComposer>AMacroComposer(new TMacroComposer(Application, AUnit->PostSave));
			if (AMacroComposer->ShowModal() == mrOk) {
				AUnit->PostSave->PostBuildCMD->Text = AMacroComposer->EditDocOriginal->Lines->Text;
				AUnit->PostSave->UserMacros->Text = AMacroComposer->EditDocUser->Lines->Text;

				if (StateMachineProject) {
					StateMachineProject->MarkModified();
				}

				UpdateCaption();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjBindingsDisabledClick(TObject * Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit) {
			AUnit->BindingsDisabled = !AUnit->BindingsDisabled;

			if (StateMachineProject)
				StateMachineProject->MarkModified();

			UpdateCaption();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjRoot4BuildClick(TObject * Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit) {
			AUnit->IsBuildRoot = !AUnit->IsBuildRoot;

			// сбросим во всех остальных
			if (AUnit->IsBuildRoot) {
				std::vector<TStateMachineEditorUnit*>AVecUnits;
				CollectAllEnabledUnits(StateMachineProject, AVecUnits);
				for (std::size_t i = 0; i < AVecUnits.size(); i++) {
					if (AVecUnits[i] != AUnit) {
						AVecUnits[i]->IsBuildRoot = false;
					}
				}
			}

			if (StateMachineProject)
				StateMachineProject->MarkModified();

			UpdateCaption();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjUnitEnabledClick(TObject * Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit) {
			AUnit->Enabled = !AUnit->Enabled;

			if (StateMachineProject)
				StateMachineProject->MarkModified();

			UpdateCaption();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjVirtualClick(TObject * Sender) {
	try {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ProjectManagerView1->SelectedDoc);
		if (AUnit) {
			AUnit->StateMachineUnitType = AUnit->StateMachineUnitType == smutNormal ? smutVirtual : smutNormal;

			if (StateMachineProject)
				StateMachineProject->MarkModified();

			UpdateCaption();
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuStackCopyMsgClick(TObject * Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			const UnicodeString sMsg = StringReplace(StripHotkey(AMenuItem->Caption), L"Msg=", L"", TReplaceFlags());
			Clipboard()->AsText = sMsg;
			WLOG_DEBUG(L"Msg <%s> copied to clipboard", sMsg.c_str());
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuStackCopyIDClick(TObject *Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			const UnicodeString sVal = StringReplace(StripHotkey(AMenuItem->Caption), L"ID=", L"", TReplaceFlags());
			Clipboard()->AsText = sVal;
			WLOG_DEBUG(L"ID <%s> copied to clipboard", sVal.c_str());
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuStackFilterMsgClick(TObject * Sender) {
	try {
		TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
		if (AMenuItem) {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->CommaText = AMenuItem->Checked ? StringReplace(StripHotkey(AMenuItem->Caption), L"Remove filter=", L"",
				TReplaceFlags()) : StringReplace(StripHotkey(AMenuItem->Caption), L"Add filter=", L"", TReplaceFlags());

			if (AStringListPtr->Count != 4)
				throw Exception(L"Wrong filter stack msg format: " + AStringListPtr->CommaText);

			const UnicodeString sScxmlMachineName = AStringListPtr->Strings[0];
			const UnicodeString sID = AStringListPtr->Strings[1];
			const UnicodeString sStateName = AStringListPtr->Strings[2];
			const TScxmlMsgType AScxmlMsgType = StringToScxmlMsgType(L"smtt" + AStringListPtr->Strings[3]);

			if (StateMachineProject) {

				bool bProjectRequireSave = false;

				const int iIndex = StateMachineProject->ProjectProperties->FilteredMsgs->IndexOfAnyMatched(sStateName, sScxmlMachineName,
					sID, AScxmlMsgType);
				if (AMenuItem->Checked) {
					if (iIndex != -1) {
						bProjectRequireSave = true;
						StateMachineProject->ProjectProperties->FilteredMsgs->Delete(iIndex);

						WLOG_DEBUG(L"Filter <%s> was removed!", AStringListPtr->CommaText.c_str());
					}
				}
				else {
					if (iIndex == -1) {
						bProjectRequireSave = true;
						TTestingMsgItem *AFilterItem = StateMachineProject->ProjectProperties->FilteredMsgs->Add();
						AFilterItem->ScxmlName = sScxmlMachineName;
						AFilterItem->Msg = sStateName;
						AFilterItem->Type = AScxmlMsgType;

						WLOG_DEBUG(L"Filter <%s> was added!", AStringListPtr->CommaText.c_str());
					}
				}

				if (bProjectRequireSave) {
					StateMachineProject->SaveProperties();
					SetDockPanelStackCaption(StateMachineProject->ProjectProperties->FilteredMsgs->Count);
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PopupMenuStackPopup(TObject * Sender) {

	MenuStackCopyMsg->Visible = false;
	MenuStackCopyScxmlName->Visible = false;
	MenuStackCopyID->Visible = false;
	MenuStackFilterMsg->Visible = false;

	if (ListViewCallStack->ItemIndex != -1) {
		TListItem* AItem = ListViewCallStack->Items->Item[ListViewCallStack->ItemIndex];
		if (AItem->SubItems->Count == 4) {
			MenuStackCopyMsg->Caption = L"Msg=" + AItem->SubItems->Strings[TCallStackColumnType::csctMsg];
			MenuStackCopyMsg->Visible = true;

			MenuStackCopyScxmlName->Caption = L"ScxmlName=" + AItem->SubItems->Strings[TCallStackColumnType::csctStatemachineName];
			MenuStackCopyScxmlName->Visible = true;

			MenuStackCopyID->Caption = L"ID=" + AItem->SubItems->Strings[TCallStackColumnType::csctIdentifier];
			MenuStackCopyID->Visible = true;

			if (StateMachineProject) {
				const TScxmlMsgType AScxmlMsgType = StringToScxmlMsgType
					(L"smtt" + AItem->SubItems->Strings[TCallStackColumnType::csctScxmlType]);

				const int iIndex = StateMachineProject->ProjectProperties->FilteredMsgs->IndexOfAnyMatched
					(AItem->SubItems->Strings[TCallStackColumnType::csctMsg], //
					AItem->SubItems->Strings[TCallStackColumnType::csctStatemachineName],
					AItem->SubItems->Strings[TCallStackColumnType::csctIdentifier], //
					AScxmlMsgType);

				std::auto_ptr<TStringList>AStringListPtr(new TStringList());
				AStringListPtr->Add(AItem->SubItems->Strings[TCallStackColumnType::csctStatemachineName]);
				AStringListPtr->Add(AItem->SubItems->Strings[TCallStackColumnType::csctIdentifier]);
				AStringListPtr->Add(AItem->SubItems->Strings[TCallStackColumnType::csctMsg]);
				AStringListPtr->Add(AItem->SubItems->Strings[TCallStackColumnType::csctScxmlType]);

				MenuStackFilterMsg->Caption = (iIndex == -1 ? L"Add filter=" : L"Remove filter=") + AStringListPtr->CommaText;
				MenuStackFilterMsg->Visible = true;
				MenuStackFilterMsg->Checked = iIndex != -1;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::SetDockPanelStackCaption(const int iFilteredCount) {
	DockPanelCallStack->Caption = iFilteredCount ? UnicodeString().sprintf(L"Call stack - filtered [%d]", iFilteredCount) : UnicodeString
		("Call stack");
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actDockPanelScxmlViewVisibleUpdate(TObject * Sender) {
	actDockPanelScxmlViewVisible->Checked = DockPanelScxmlView->PanelVisible && DockPanelScxmlView->Zone;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actDockPanelScxmlViewVisibleExecute(TObject * Sender) {
	const bool bWasVisible = DockPanelScxmlView->PanelVisible && DockPanelScxmlView->Zone;
	DockPanelScxmlView->PanelVisible = !bWasVisible;

	if (!DockPanelScxmlView->Zone) {
		if (DockPanelFrameRoot->PanelVisible && DockPanelFrameRoot->Zone) {
			this->LMDDockSite1->DockControl(DockPanelScxmlView, DockPanelFrameRoot->Zone, alClient);
		}
		else {
			this->LMDDockSite1->DockControl(DockPanelScxmlView, this->LMDDockSite1->RootZone, alClient);
		}
	}

	if (DockPanelScxmlView->PanelVisible) {
		DockPanelScxmlView->Show();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ForceShowDockPanelInRootZone(TLMDDockPanel *ADockPanel) {
	if (!ADockPanel)
		return;

	const bool bWasVisible = ADockPanel->PanelVisible && ADockPanel->Zone;
	if (!bWasVisible) {
		ADockPanel->PanelVisible = true;

		if (!ADockPanel->Zone) {
			if (DockPanelFrameRoot->PanelVisible && DockPanelFrameRoot->Zone) {
				this->LMDDockSite1->DockControl(ADockPanel, DockPanelFrameRoot->Zone, alClient);
			}
			else {
				this->LMDDockSite1->DockControl(ADockPanel, this->LMDDockSite1->RootZone, alClient);
			}
		}

		ADockPanel->Show();
	}

	if (ADockPanel->Zone && ADockPanel->Zone->Parent && ADockPanel->Zone->Parent->ActivePage != ADockPanel->Zone) {
		ADockPanel->Zone->Parent->ActivePage = ADockPanel->Zone;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ForceShowLogPanel(void) {
	ForceShowDockPanelInRootZone(DockPanelLog);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ForceShowPropInspPanel(void) {
	ForceShowDockPanelInRootZone(DockPanelPropInsp);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actXMLTransitionViewExecute(TObject *Sender) {
	const bool bWasVisible = DockPanelTransitionXML->PanelVisible && DockPanelTransitionXML->Zone;
	DockPanelTransitionXML->PanelVisible = !bWasVisible;

	if (!DockPanelTransitionXML->Zone) {
		if (DockPanelProjectTree->PanelVisible && DockPanelProjectTree->Zone) {
			this->LMDDockSite1->DockControl(DockPanelTransitionXML, DockPanelProjectTree->Zone, alBottom);
		}
		else if (DockPanelFrameRoot->PanelVisible && DockPanelFrameRoot->Zone) {
			this->LMDDockSite1->DockControl(DockPanelTransitionXML, DockPanelFrameRoot->Zone, alBottom);
		}
		else {
			this->LMDDockSite1->DockControl(DockPanelTransitionXML, this->LMDDockSite1->RootZone, alBottom);
		}
	}

	if (DockPanelTransitionXML->PanelVisible) {
		DockPanelTransitionXML->Show();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actXMLTransitionViewUpdate(TObject *Sender) {
	actXMLTransitionView->Checked = DockPanelTransitionXML->PanelVisible && DockPanelTransitionXML->Zone;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actTranslateChangesToScxmlViewUpdate(TObject * Sender) {
	actTranslateChangesToScxmlView->Checked = SettingsData->TranslateEveryChangeToScxmlView;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actTranslateChangesToScxmlViewExecute(TObject * Sender) {
	SettingsData->TranslateEveryChangeToScxmlView = !SettingsData->TranslateEveryChangeToScxmlView;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actLoadLayoutAccept(TObject * Sender) {
	try {
		if (FileExists(actLoadLayout->Dialog->FileName)) {
			LMDDockManager1->LoadFromFile(actLoadLayout->Dialog->FileName);
			WLOG_DEBUG(L"Successfully loaded layout [%s]", actLoadLayout->Dialog->FileName.c_str());
		}
		else {
			WLOG_WARNING(L"Layout file <%s> is not found!", actLoadLayout->Dialog->FileName.c_str());
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actSaveLayoutAccept(TObject * Sender) {
	try {
		LMDDockManager1->SaveToFile(actSaveLayout->Dialog->FileName);
		WLOG_DEBUG(L"Successfully saved default layout [%s]", actSaveLayout->Dialog->FileName.c_str());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::UpdateProjectsClosed(TObject * Sender) {
	TCustomAction * ACustomAction = dynamic_cast<TCustomAction*>(Sender);
	if (ACustomAction) {
		ACustomAction->Enabled = ProjectManager1->Root == NULL;
		ACustomAction->Caption = ACustomAction->Hint + (ACustomAction->Enabled ? L"" : L" (Close all to enable)");
	}
	else {
		WLOG_ERROR(L"Can not cast <%s> to <TCustomAction>", Sender ? Sender->ClassName().c_str() : L"NULL");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actLayoutBeforeExecute(TObject * Sender) {
	actLoadLayout->Dialog->InitialDir = SettingsData->AppLayoutDir;
}

// ------------------------------------------------------------------
void __fastcall TFormScxmlGui::actClearAllBreakpointsExecute(TObject * Sender) {
	TStateMachineDockPanel *ADockPanel = GetActiveEditorDockPanel();
	if (ADockPanel) {
		ADockPanel->ClearAllBreakpoints();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actGetActivePanelUpdate(TObject * Sender) {
	TCustomAction * ACustomAction = dynamic_cast<TCustomAction*>(Sender);
	if (ACustomAction) {
		ACustomAction->Enabled = GetActiveEditorDockPanel();
	}
	else {
		WLOG_ERROR(L"Can not cast <%s> to <TCustomAction>", Sender ? Sender->ClassName().c_str() : L"NULL");
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actUpdateBindingsExecute(TObject * Sender) {
	DoCollectAllBindings();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actClearClipboardConnectionsExecute(TObject * Sender) {
	const std::size_t nSize = SettingsData->ClipboardConnectionsMap.size();
	SettingsData->ClipboardConnectionsMap.clear();
	WLOG_INFO(L"Delete - %d clipboard connection(s)", nSize);
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actClearClipboardConnectionsUpdate(TObject * Sender) {
	actClearClipboardConnections->Enabled = SettingsData->ClipboardConnectionsMap.size();
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actShowTextEditorExecute(TObject * Sender) {
	int iCount = 0;
	for (int i = 0; i < this->ComponentCount; i++) {
		TSyntaxTextEditorDockPanel * ASyntaxTextEditorDockPanel = dynamic_cast<TSyntaxTextEditorDockPanel*>(this->Components[i]);
		if (ASyntaxTextEditorDockPanel) {
			iCount++;
		}
	}

	TSyntaxTextEditorDockPanel *ADockPanel = new TSyntaxTextEditorDockPanel(this, SettingsData->ActiveSyntaxScheme,
		"Text editor №" + UnicodeString(++iCount));

	this->LMDDockSite1->DockControl(ADockPanel, this->DockPanelFrameRoot->Zone, alClient);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actClearLogExecute(TObject * Sender) {
	if (EditViewLog->Document) {
		EditViewLog->Document->Clear();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actPanelPropPathExecute(TObject * Sender) {
	PanelPropInspPath->Visible = !PanelPropInspPath->Visible;
	SettingsData->TempRegistry->Values["PanelPropInspPath.Visible"] = BoolToStr(PanelPropInspPath->Visible);
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actPanelPropPathUpdate(TObject * Sender) {
	actPanelPropPath->Checked = PanelPropInspPath->Visible;
}

// ---------------------------------------------------------------------------
bool __fastcall TFormScxmlGui::IsPropIncluded(const UnicodeString & sPropName, bool bIsClass) {
	if (EditPropInspPath->Text.IsEmpty())
		return true;

	bool AIncludeProp = false;
	for (int i = FFilterPropsListPtr->Count - 1; i >= 0; i--) {
		if (!bIsClass && !AnsiEndsStr(sPropName, FFilterPropsListPtr->Strings[i]))
			continue;

		if (Wildcardutils::WildCardMatch(EditPropInspPath->Text.c_str(), FFilterPropsListPtr->Strings[i].c_str())) {
			AIncludeProp = true;
			break;
		}
	}
	return AIncludeProp;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PropSettingsInspectorFilterProp(TObject * Sender, TPersistent * AInstance, ILMDProperty * APropInfo,
	bool AIsSubProp, bool&AIncludeProp) {

	if (SettingsData) {
		// сначала базовый вызов
		Propinspext::PropSettingsInspectorFilterProp(Sender, AInstance, APropInfo, AIsSubProp, AIncludeProp);

		// теперь по фильтру из приложения
		if (CheckFilterProp->Checked && AIncludeProp && APropInfo) {
			TLMDPropertyInspector * ALMDPropertyInspector = dynamic_cast<TLMDPropertyInspector*>(Sender);
			if (ALMDPropertyInspector && ALMDPropertyInspector->Selection && ALMDPropertyInspector->Selection->Count) {
				static TMetaClass *AClass = NULL;
				if (!AClass || ALMDPropertyInspector->Selection->Item[0]->ClassType() != AClass) {
					FFilterPropsListPtr->Clear();
					MUtils::GetAllNestedPublishedProps(ALMDPropertyInspector->Selection->Item[0], FFilterPropsListPtr.get(), L"");
					AClass = ALMDPropertyInspector->Selection->Item[0]->ClassType();
				}
			}
			AIncludeProp = IsPropIncluded(APropInfo->Name(), APropInfo->PropType()->Kind == tkClass);
		}
	}
	FFilterPropFlag = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PropSettingsInspectorGetEditorClass(TObject * Sender, TPersistent * AInstance, ILMDProperty * APropInfo,
	TLMDPropEditorClass & AEditorClass)

{
	if (SettingsData) {
		const UnicodeString sPropName = APropInfo->Name();
		bool AIncludeProp = true;

		if (CheckFilterProp->Checked && APropInfo) {
			AIncludeProp = IsPropIncluded(sPropName, APropInfo->PropType()->Kind == tkClass);
		}

		if (AIncludeProp) {

			// Внимание!!! Двойная фильтрация, позже подумать, как оптимизировать
			Propinspext::PropSettingsInspectorGetEditorClass(Sender, AInstance, APropInfo, AEditorClass);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PropSettingsInspectorGetCaptionColor(TObject * Sender, TLMDPropertyInspectorItem * AItem, TColor & AColor) {
	Propinspext::PropSettingsInspectorGetCaptionColor(Sender, AItem, AColor);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::CheckFilterPropClick(TObject * Sender) {
	PropSettingsInspector->UpdateItems();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::EditPropInspPathChange(TObject * Sender) {
	if (CheckFilterProp->Checked) {
		PropSettingsInspector->UpdateItems();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::SwitchToProgramLog(const bool bClear /* = false */ ) {
	if (bClear) {
		EditDocDebug->Clear();
	}
	TabControlLog->TabIndex = 0;
	TabControlLogChange(FormScxmlGui->TabControlLog);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::SwitchToTesterLog(const bool bClear /* = false */ ) {
	if (bClear) {
		EditDocTester->Clear();
	}
	TabControlLog->TabIndex = 1;
	TabControlLogChange(FormScxmlGui->TabControlLog);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSaveUpdate(TObject * Sender) {
	TStateMachineEditorUnit *AUnit = GetActiveUnit();
	actSave->Enabled = !SettingsData->IsTesterWorking && AUnit != NULL && AUnit->NeedSave;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actEnableZoomHintsExecute(TObject * Sender) {
	SettingsData->ZoomHintsEnabled = !SettingsData->ZoomHintsEnabled;
	TStateMachineDockPanel *ADockPanel = GetActiveEditorDockPanel();
	if (ADockPanel) {
		ADockPanel->StateMachineEditor->TheTree->Refresh();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actEnableZoomHintsUpdate(TObject * Sender) {
	actEnableZoomHints->ImageIndex = SettingsData->ZoomHintsEnabled ? 73 : 74;
	actEnableZoomHints->Hint = SettingsData->ZoomHintsEnabled ?
		L"Shape hints on small zoom are enabled" : L"Shape hints on small zoom are disabled";
	actEnableZoomHints->Checked = SettingsData->ZoomHintsEnabled;

	MenuZoomHintsEnabled->Caption = L"Zoom hints - enable/disable";
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actAutoOpenUnitExecute(TObject * Sender) {
	SettingsData->TestingAutoOpenUnit = !SettingsData->TestingAutoOpenUnit;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actAutoOpenUnitUpdate(TObject * Sender) {
	actAutoOpenUnit->Checked = SettingsData->TestingAutoOpenUnit;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ApplicationEvents1Idle(TObject * Sender, bool&Done) {
	/* Property Inspector */
	if (FFilterPropFlag) {
		FInspectorProps.clear();
		for (int i = 0; i < PropSettingsInspector->Items->Count; i++) {
			EnumerateAllPropItems(PropSettingsInspector->Items->Items[i], FInspectorProps);
		}
		FFilterPropFlag = false;
	}

	/* SCROLL Logger */
	if (FScrollLogFlag) {

		this->EditViewLog->CursorPos = TPoint(0, this->EditViewLog->Document->LinesCount);
		this->EditViewLog->ScrollToCursor();

		FScrollLogFlag = false;
	}

	/* Syntax View */ {
		/* отслеживание видимости DockPanelScxmlView */
		bool bScxmlViewVisible = IsWindowVisible(DockPanelScxmlView->Handle);

		// панель была невидимой и стала видимой
		if (FScxmlViewVisibleFlag == false && bScxmlViewVisible == true) {

			UpdateSyntaxView();

			FScxmlViewVisibleFlag = true;
		}

		FScxmlViewVisibleFlag = bScxmlViewVisible;

		if (FUpdateSyntaxViewFlag) {
			if (this->FSyntaxXMLView->IsViewOnly() && IsWindowVisible(DockPanelScxmlView->Handle)) {
				TStateMachineDockPanel *ADockPanel = ActiveEditorDockPanel;
				if (ADockPanel) {
					try {
						WLOG_DEBUG(L"Update SCXML view=%s", ADockPanel->Caption.c_str());
						this->FSyntaxXMLView->SetText(ADockPanel->RawScxmlForceComments);
					}
					catch(Exception * E) {
						// надо уведомить пользователя, так как при некоторых критических ошибках,
						// может выделять ошибки
						WLOG_WARNING(L"SCXML VIEW> %s", E->Message.c_str());
					}
					catch(...) {
					}
				}
			}

			FUpdateSyntaxViewFlag = false;
		}
	}

	/* Update Caption */
	if (!FUpdateCaptionPanelsSet.empty()) {

		for (std::set<TStateMachineDockPanel*>::iterator it = FUpdateCaptionPanelsSet.begin(); it != FUpdateCaptionPanelsSet.end(); ++it) {
			TStateMachineDockPanel *ADockPanel = *it;
			// внимание, в служебных целях, мы можем передать ADockPanel == NULL, чтобы инициировать обновление заглавия формы
			// уже может и не существовать
			if (ADockPanel && IndexOfComponentList(this, ADockPanel) != -1) {
				if (ADockPanel->StateMachineEditorUnit) {
					if (ADockPanel->StateMachineEditor->IsModified()) {
						ADockPanel->Caption = ADockPanel->StateMachineEditorUnit->DisplayName + "*";
						// здесь надо 'false', иначе кто-то из детей может вернуть
						if (!ADockPanel->StateMachineEditorUnit->IsModified(false)) {
							ADockPanel->StateMachineEditorUnit->MarkModified();
						}
					}
					else {
						ADockPanel->Caption = ADockPanel->StateMachineEditorUnit->DisplayName;
						ADockPanel->StateMachineEditorUnit->ResetModified();
					}
				}
			}
		}

		UnicodeString sCaption = L"";

		TStateMachineDockPanel *ADockPanel = ActiveEditorDockPanel;
		if (ADockPanel) {
			sCaption = ADockPanel->Caption;
		}
		else {
			// особый случай, если не проект
			if (!StateMachineProject && ProjectManager1->Root) {
				sCaption = ProjectManager1->Root->FilePath;
			}
		}

		this->UpdateCaption(sCaption);

		FUpdateCaptionPanelsSet.clear();
	}

	/* после Undock, делаем Fix, чтобы переключало на предыдущую вкладку */
	if (FBeforeUndockZone) {
		if (FBeforeUndockZone->Parent) {

			bool bCancelled = false;
			if (IndexOfComponentList(this, FUndockPanel) != -1) {
				bCancelled = FUndockPanel->Zone && DockPanelFrameRoot->Zone->Parent && FUndockPanel->Zone->Parent ==
					DockPanelFrameRoot->Zone->Parent;
			}

			if (!bCancelled && FBeforeUndockZone->Parent->ActivePage != FBeforeUndockZone) {
				FBeforeUndockZone->Parent->ActivePage = FBeforeUndockZone;
			}
		}
		FBeforeUndockZone = NULL;
	}

	if (!FDockPanelResizeSet.empty()) {

		for (std::set<TLMDDockPanel*>::iterator it = FDockPanelResizeSet.begin(); it != FDockPanelResizeSet.end(); ++it) {
			TLMDDockPanel *ADockPanel = *it;
			// уже может и не существовать
			if (ADockPanel && IndexOfComponentList(this, ADockPanel) != -1) {
				FixDockPanelSize(ADockPanel);
			}
		}

		FDockPanelResizeSet.clear();
	}

	if (FSelectProjectManagerViewFlag) {
		FSelectProjectManagerViewFlag = false;

		if (ActiveUnit) {
			const bool bMismatch = ActiveUnit != ProjectManagerView1->SelectedDoc;
			if (bMismatch) {
#if 0 // для глубокой отладки
				WLOG_WARNING(L"Unit mismatch Active[%s] Selected[%s]", ActiveUnit->DisplayName.c_str(),
					ProjectManagerView1->SelectedDoc ? ProjectManagerView1->SelectedDoc->DisplayName.c_str() : L"NULL");
#endif

				TTreeNode *AItem = ProjectManagerView1->TopItem;
				while (AItem) {
					TLMDProjNode *AProjNode = ProjectManagerView1->ConvertTreeNode(AItem);
					if (AProjNode == ActiveUnit) {
						ProjectManagerView1->Select(AItem);
						break;
					}

					AItem = AItem->GetNext();
				}

			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actImportConfigurationAccept(TObject * Sender) {
	try {
		FExternTestListeningFlag = true;
		actRun->Execute();

		const UnicodeString &sConfiguration = actImportConfiguration->Dialog->FileName;

		std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(this));
		ADialogWaitPtr->ShowDialog(UnicodeString().sprintf(L"Importing state machine configuration [%s] ...",
				ExtractFileName(sConfiguration).c_str()));

		std::auto_ptr<TStringList>AStringListPtr(new TStringList());
		AStringListPtr->LoadFromFile(sConfiguration);
		for (int i = 0; i < AStringListPtr->Count; i++) {
			UnicodeString sInterpreter = AStringListPtr->Names[i];
			UnicodeString sInvokeID = "";

			boost::wregex e(L"^([^\\[]+)\\[([^\\]]+)\\]$");
			boost::wsmatch what;

			std::wstring wstr = sInterpreter.c_str();
			if (boost::regex_match(wstr, what, e)) {
				if (what.size() == 3) {

					sInterpreter = std::wstring(what[1]).c_str();
					sInvokeID = std::wstring(what[2]).c_str();
				}
			}

			const UnicodeString &sState = AStringListPtr->ValueFromIndex[i];

			if (!sInterpreter.IsEmpty() && !sState.IsEmpty()) {
				TStateMachineDockPanel *ADockPanel = GetActiveEditorDockPanel();

				std::vector<TStateMachineEditorUnit*>AVecUnits;
				CollectAllEnabledUnits(FormScxmlGui->ProjectManager1->Root, AVecUnits);
				for (std::size_t i = 0; i < AVecUnits.size(); i++) {
					const UnicodeString &sUnitName = AVecUnits[i]->ScxmlName;
					if (sUnitName == sInterpreter) {
						if (!AVecUnits[i]->Opened) {
							AVecUnits[i]->Open(true);

							ADialogWaitPtr->Refresh();
						}

						if (AVecUnits[i]->StateMachineDockPanel) {

							// найден нужный редактор
							ADockPanel = AVecUnits[i]->StateMachineDockPanel;

							break;
						}
					}
				}

				if (ADockPanel) {

					TScxmlShape *ARootScxml = ADockPanel->StateMachineEditor->RootScxml;
					if (ARootScxml) {

						if (ARootScxml->InvokeID.IsEmpty() || ARootScxml->InvokeID == sInvokeID) {
							std::pair<TStateMachineDockPanel*,
							TTreeNodeShape*>AEnteredElements = ADockPanel->EnterStateNode(sState, true,
								true /* принудительно открываем все узлы */ , L"");

							ADockPanel = AEnteredElements.first;
							TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AEnteredElements.second);

							// если определена фигура, и установлены разрешения, то переключим на соотв. док. панель
							if (AVisualShape) {

								if (ADockPanel && (ADockPanel->Zone && ADockPanel->Zone->Parent) &&
									(ADockPanel->Zone->Parent->ActivePage != ADockPanel->Zone)) {
									// в этом методе будет переключено на передний план и обновиться SyntaxView

									ADockPanel->Show();

									ADialogWaitPtr->Refresh();
								}
							}
							else {
								WLOG_WARNING(L"Can not find shape [%s] in unit [%s]", sState.c_str(), sInterpreter.c_str());
							}
						}

					}
					else {
						WLOG_WARNING(L"Can not find Root Scxml [%s]", sInterpreter.c_str());
					}
				}
				else {
					WLOG_WARNING(L"Can not find unit [%s]", sInterpreter.c_str());
				}
			}
		}
		WLOG_INFO(L"Configuration [%s] was loaded!", sConfiguration.c_str());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuProjEditOrderClick(TObject * Sender) {
	try {
		if (StateMachineProject) {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->LoadFromFile(StateMachineProject->FilePath);

			std::auto_ptr<TFormSyntaxEditorForm>ASyntaxForm(new TFormSyntaxEditorForm(Application, "XML", AStringListPtr->Text,
					"Edit Project Order"));
			if (ASyntaxForm->ShowModal() == mrOk) {
				TFile::Copy(StateMachineProject->FilePath, StateMachineProject->FilePath + ".BAK", true);

				ASyntaxForm->EditDoc1->Lines->SaveToFile(StateMachineProject->FilePath);

				WLOG_INFO(L"Backup project was saved as [%s.BAK]!", StateMachineProject->FilePath.c_str());
				SendBallonMsg(L"Backup project was saved as [" + StateMachineProject->FilePath + ".BAK]!", bfInfo);

				if (ProjCloseQuery()) {
					DoOpenProject(StateMachineProject->FilePath);
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
int GetPropertyLevel(TPersistent * AInstance, ILMDProperty * APropInfo) {
	if (AInstance) {
		if (AInstance->InheritsFrom(__classid(TScxmlBaseShape))) {
			const UnicodeString sPropName = APropInfo->Name();
			if (IsPublishedProp(__classid(TTreeNodeShape), sPropName)) {
				return 0;
			}
			if (IsPublishedProp(__classid(TScxmlBaseShape), sPropName)) {
				return 3;
			}
			if (IsPublishedProp(__classid(TVisualScxmlBaseShape), sPropName)) {
				return 1;
			}
			if (IsPublishedProp(__classid(TChildScxmlBaseShape), sPropName)) {
				return 1;
			}
			return 4;
		}
	}
	return 0;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::PropSettingsInspectorCompare(TObject * Sender, TPersistent * AInstance, ILMDProperty * APropInfo1,
	ILMDProperty * APropInfo2, int&ACompare) {
	// эту процедуру необходимо для всех делать, так как иначе будут по алфавиту сортироваться остальные
	if (AInstance) {

		// для фигур - особый случай - чтобы scxml свойства были вверху
		const int iLevel1 = GetPropertyLevel(AInstance, APropInfo1);
		const int iLevel2 = GetPropertyLevel(AInstance, APropInfo2);

		if (iLevel1 < iLevel2) {
			ACompare = 1;
		}
		else if (iLevel1 > iLevel2) {
			ACompare = -1;
		}
		else {
			const short sh1 = APropInfo1->RTTIPropInfo()->NameIndex;
			const short sh2 = APropInfo2->RTTIPropInfo()->NameIndex;

			if (sh1 < sh2) {
				ACompare = -1;
			}
			else if (sh1 > sh2) {
				ACompare = 1;
			}
			else {
				ACompare = 0;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actLockDockingExecute(TObject * Sender) {
	SettingsData->LockUI = !SettingsData->LockUI;

	LMDDockSite1->LockMode = SettingsData->LockUI ? (LMDDockSite1->LockMode << dlmUndockFrom) : (LMDDockSite1->LockMode >> dlmUndockFrom);
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actLockDockingUpdate(TObject * Sender) {
	actLockDocking->Checked = SettingsData->LockUI;
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::CheckImageAdvancedDrawItem(TObject * Sender, TCanvas * ACanvas, const TRect & ARect, TOwnerDrawState State) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {
		TAdvancedMenuDrawItemEvent AHandler = AMenuItem->OnAdvancedDrawItem;
		AMenuItem->OnAdvancedDrawItem = NULL;

		DrawMenuItem(AMenuItem, ACanvas, ARect, State);

		if (AMenuItem->Checked) {
			ACanvas->Pen->Color = clBlack;
			ACanvas->Rectangle(ARect.Left, ARect.Top, ARect.Left + 21, ARect.Top + 21);
		}

		AMenuItem->OnAdvancedDrawItem = AHandler;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::ControlBar1Paint(TObject * Sender) {
	if (SettingsData->AviaExtensionEnabled) {
		SetTextAlign(ControlBar1->Canvas->Handle, TA_RIGHT);
		ControlBar1->Canvas->Font->Color = clBlue;
		ControlBar1->Canvas->Font->Size = 10;
		ControlBar1->Canvas->Font->Name = L"Arial";
		ControlBar1->Canvas->Font->Style = ControlBar1->Canvas->Font->Style << fsBold;
		ControlBar1->Canvas->TextOut(ControlBar1->Width - 5, 5, "EXTENDED");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actGetScreenCursorCoordinatesExecute(TObject * Sender) {
	TPoint APoint;
	if (GetCursorPos(&APoint)) {
		WLOG_INFO(L"Cursor pos x=[%d] y=[%d]", APoint.x, APoint.y);
		SendBallonMsg(UnicodeString().sprintf(L"Cursor pos [%d, %d]", APoint.x, APoint.y), bfInfo);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actRegExpDialogExecute(TObject *Sender) {
	std::auto_ptr<TDialogRegExp>ADlgRegExpPtr(new TDialogRegExp(this));
	ADlgRegExpPtr->ShowModal();
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DockPanelTransitionXMLExit(TObject *Sender) {
	DockPanelTransitionXML->FIX_PANEL_ENTERED = false;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DockPanelTransitionXMLEnter(TObject *Sender) {
	FixDockPanelActiveState(DockPanelTransitionXML);

	DockPanelTransitionXML->FIX_PANEL_ENTERED = true;

	this->DesignObjects1->Clear();

	if (TransitionXMLEditor) {
		for (int i = 0; i < TransitionXMLEditor->TheTree->Selected->Count(); i++) {
			this->DesignObjects1->Add(TransitionXMLEditor->TheTree->Selected->Items[i]);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::FixDockPanelActiveState(TLMDDockPanel *APanelActive) {
	for (int i = 0; i < this->ComponentCount; i++) {
		TLMDDockPanel * AIterDockPanel = dynamic_cast<TLMDDockPanel*>(this->Components[i]);
		if (AIterDockPanel && AIterDockPanel != APanelActive) {
			if (AIterDockPanel->FIX_PANEL_ENTERED) {
				WLOG_DEBUG(L"EXIT> Panel[%s] force exit!", AIterDockPanel->Caption.c_str());
				AIterDockPanel->OnExit(AIterDockPanel);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::OnFixDockPanelMouseActivate(System::TObject* Sender, TMouseButton Button, Classes::TShiftState Shift, int X,
	int Y, int HitTest, TMouseActivate &MouseActivate) {

	FixDockPanelActiveState(DockPanelTransitionXML);

	if (!DockPanelTransitionXML->FIX_PANEL_ENTERED) {
		WLOG_DEBUG(L"ENTER> Panel[%s] force enter!", DockPanelTransitionXML->Caption.c_str());
		DockPanelTransitionXML->OnEnter(this);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::LMDDockSite1UnDock(TObject *Sender, TControl *Client, TWinControl *NewTarget, bool &Allow) {
	TLMDDockPanel * ADockPanel = dynamic_cast<TLMDDockPanel*>(Client);

	// специальная подстройка, чтобы при отстыковке панелей, мы попадали на предыдущую
	if (ADockPanel && ADockPanel->Zone && ADockPanel->Zone->Parent && //

		DockPanelFrameRoot->Zone && (DockPanelFrameRoot->Zone->Parent == ADockPanel->Zone->Parent)) {

		if (ADockPanel->Zone->Parent) {

			for (int i = ADockPanel->Zone->Index - 1; i > 0; i--) {
				if (ADockPanel->Zone->Parent->Zones[i]->Panel && ADockPanel->Zone->Parent->Zones[i]->Panel->PanelVisible) {
					FBeforeUndockZone = ADockPanel->Zone->Parent->Zones[i];
					FUndockPanel = ADockPanel;

					break;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::DockPanelFixResize(TObject *Sender) {
	TLMDDockPanel * ALMDDockPanel = dynamic_cast<TLMDDockPanel*>(Sender);
	if (ALMDDockPanel) {
		FDockPanelResizeSet.insert(ALMDDockPanel);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actIDEInsightExecute(TObject *Sender) {

	TStateMachineEditor *AEditorActive = NULL;

	// вызываем форму
	HWND hWnd = Application->ActiveFormHandle;
	if (hWnd) {

		TWinControl *AWinControl = FindControl(hWnd);
		if (AWinControl) {
			if (TStateMachineEditor * AModalEditor = dynamic_cast<TStateMachineEditor*>(AWinControl)) {
				AEditorActive = AModalEditor;
			}
			else if (TFormTransitionEditor * AFormTransitionEditor = dynamic_cast<TFormTransitionEditor*>(AWinControl)) {
				AEditorActive = AFormTransitionEditor->StateMachineEditor;
			}
		}
	}

	if (!DialogInsight) {
		DialogInsight = new TDialogInsight(Application);
	}

	if (!AEditorActive) {
		AEditorActive = DockPanelTransitionXML->FIX_PANEL_ENTERED ? TransitionXMLEditor : GetActiveStateMachineEditor();
#if 0 // для глубокой отладки
		WLOG_INFO(L"ActiveEditor=[%s]", AEditorActive->StateMachineEditorUnit ? AEditorActive->StateMachineEditorUnit->DisplayName.c_str()
			: AEditorActive->ClassName().c_str());
#endif
	}

	DialogInsight->Execute(AEditorActive);
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actAutoLayoutOrthoSplinesExecute(TObject *Sender) {
	SettingsData->AutoLayoutSplines = alsOrtho;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actAutoLayoutOrthoSplinesUpdate(TObject *Sender) {
	actAutoLayoutOrthoSplines->Checked = SettingsData->AutoLayoutSplines == alsOrtho;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actAutoLayoutPolySplinesExecute(TObject *Sender) {
	SettingsData->AutoLayoutSplines = alsPolyline;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actAutoLayoutPolySplinesUpdate(TObject *Sender) {
	actAutoLayoutPolySplines->Checked = SettingsData->AutoLayoutSplines == alsPolyline;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actExportGraphDotAccept(TObject *Sender) {
	try {
		TStateMachineDockPanel* ADockPanel = ActiveEditorDockPanel;
		if (ADockPanel) {
			ADockPanel->StateMachineEditor->SaveToDot(actExportGraphDot->Dialog->FileName);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actExportGraphDotPlusPngAccept(TObject *Sender) {
	try {
		TStateMachineDockPanel* ADockPanel = ActiveEditorDockPanel;
		if (ADockPanel) {
			ADockPanel->StateMachineEditor->SaveToDotPlusPng(actExportGraphDotPlusPng->Dialog->FileName);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::OnMsgAddOrOpenExistingUnit(TMessage &msg) {
	if (msg.Msg == WM_SCXML_ADD_OR_OPEN_EXISTING_UNIT) {
		TStateMachineEditorUnit *AUnit = reinterpret_cast<TStateMachineEditorUnit*>(msg.WParam);
		wchar_t *wchBuff = reinterpret_cast<wchar_t*>(msg.LParam);
		const UnicodeString sFileName(wchBuff);
		delete[]wchBuff;

		FormScxmlGui->AddExistingUnit(AUnit, sFileName, true);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::LabelIDEInsightMouseEnter(TObject *Sender) {
	LabelIDEInsight->Font->Style = LabelIDEInsight->Font->Style << fsBold;
}
// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::LabelIDEInsightMouseLeave(TObject *Sender) {
	LabelIDEInsight->Font->Style = LabelIDEInsight->Font->Style >> fsBold;
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actRegisterScxmlFilesExecute(TObject *Sender) {
	try {
		std::auto_ptr<TDialogRegisterScxmlFiles>ADialogRegisterScxmlFilesPtr(new TDialogRegisterScxmlFiles(this));
		ADialogRegisterScxmlFilesPtr->Execute();
		SendBallonMsg("Successfully registered extensions!", bfInfo);
	}
	catch(Exception * E) {
		WLOG_ERROR(L"REGISTER_EXT> %s", E->Message.c_str());
	}

}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actExportDefExtBeforeExecute(TObject *Sender) {
	try {
		TFileSaveAs * AFileSaveAs = dynamic_cast<TFileSaveAs*>(Sender);
		if (AFileSaveAs) {
			TStateMachineEditorUnit* AUnit = GetActiveUnit();
			if (AUnit && AUnit->StateMachineDockPanel) {
				AFileSaveAs->Dialog->InitialDir = ExtractFileDir(AUnit->FilePath);
				AFileSaveAs->Dialog->FileName = ExtractFileName(ChangeFileExt(AUnit->FilePath, L"." + AFileSaveAs->Dialog->DefaultExt));
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actExportSVGAccept(TObject *Sender) {
	try {
		TStateMachineDockPanel* ADockPanel = ActiveEditorDockPanel;
		if (ADockPanel) {
			const UnicodeString sFile = actExportSVG->Dialog->FileName;

			ADockPanel->StateMachineEditor->SaveToSVG(sFile, ADockPanel->StateMachineEditor->RootScxml);

		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::MenuProjOpenShellClick(TObject *Sender) {
	if (ProjectManagerView1->SelectedDoc) {
		try {
			if (!FileExists(ProjectManagerView1->SelectedDoc->FilePath))
				throw Exception("File does not exist!");

			const UnicodeString sDirectory = ExtractFileDir(ProjectManagerView1->SelectedDoc->FilePath);

			PIDLIST_ABSOLUTE pidl;
			if (SUCCEEDED(SHParseDisplayName(sDirectory.c_str(), 0, &pidl, 0, 0))) {

				try {
					ITEMIDLIST *ASelected = ILCreateFromPathW(ProjectManagerView1->SelectedDoc->FilePath.c_str());
					if (ASelected) {
						try {
							LPCITEMIDLIST pidlNull[1] = {
								ASelected
							};

							if (FAILED(SHOpenFolderAndSelectItems(pidl, 1, pidlNull, 0)))
								throw Exception(SysErrorMessage(GetLastError()));
						}
						__finally {
							ILFree(ASelected);
						}
					}
					else
						throw Exception(SysErrorMessage(GetLastError()));
				}
				__finally {
					ILFree(pidl);
				}
			}
			else
				throw Exception(SysErrorMessage(GetLastError()));

		}
		catch(Exception * E) {
			WLOG_ERROR(L"OPEN SHELL[%s]> %s", ProjectManagerView1->SelectedDoc->FilePath.c_str(), E->Message.c_str());
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::MenuRunClick(TObject *Sender) {
	const int iBefore = MenuRun->IndexOf(MenuSpacerBeforeTester);
	const int iAfter = MenuRun->IndexOf(MenuSpacerAfterTester);

	for (int i = iAfter - 1; i > iBefore; i--) {
		delete MenuRun->Items[i];
	}

	TTestSettings *ATestSettings = SettingsData->ActiveTestSettings;

	bool bFound = false;

	std::map<int, TMenuItem*>AMenus;

	int iInsertIndex = iBefore;

	for (int i = 0; i < SettingsData->TestApplicationPresets->Count; i++) {
		if (SettingsData->TestApplicationPresets->Items[i]->Enabled) {
			TMenuItem *AMenuItem = new TMenuItem(this);
			AMenuItem->Caption = SettingsData->TestApplicationPresets->Items[i]->PresetName;
			AMenuItem->OnClick = MenuPresetTesterClick;
			AMenuItem->Tag = i;

			std::auto_ptr<TStringList>ASkippedPropsPtr(new TStringList());
			Unique::CompareError ACompareError;
			AMenuItem->Checked = Unique::CompareObjects(ATestSettings, SettingsData->TestApplicationPresets->Items[i]->TestSettings,
				ASkippedPropsPtr.get(), L"", ACompareError);
			AMenuItem->Enabled = FileExists(SettingsData->TestApplicationPresets->Items[i]->TestSettings->FullPath);
			if (AMenuItem->Checked) {
				bFound = true;
			}

			MenuRun->Insert(++iInsertIndex, AMenuItem);

			AMenus.insert(std::make_pair(i, AMenuItem));
		}
	}

	if (!bFound) {
		for (int i = 0; i < SettingsData->TestApplicationPresets->Count; i++) {
			if (SettingsData->TestApplicationPresets->Items[i]->Enabled) {
				if (SameText(ATestSettings->ExeName, SettingsData->TestApplicationPresets->Items[i]->TestSettings->ExeName)) {
					std::map<int, TMenuItem*>::iterator it = AMenus.find(i);
					if (it != AMenus.end()) {
						it->second->Checked = true;
						break;
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::MenuPresetTesterClick(TObject *Sender) {
	TMenuItem * AMenuItem = dynamic_cast<TMenuItem*>(Sender);
	if (AMenuItem) {
		try {
			const UnicodeString sPreset = StripHotkey(AMenuItem->Caption);
			TTestApplicationPresetItem*AItem = SettingsData->TestApplicationPresets->GetItemByPresetName(sPreset);
			if (!AItem)
				throw Exception(L"Can not find Testing Application Preset:[" + sPreset + "]");

			const int iItemIndex = AMenuItem->Tag;

			if (IsTestingBasedOnProjectSettings()) {
				std::auto_ptr<TStringList>ASkippedPropsPtr(new TStringList());
				Unique::CompareError ACompareError;
				if (!Unique::CompareObjects(StateMachineProject->ProjectProperties->TestSettings,
						SettingsData->TestApplicationPresets->Items[iItemIndex]->TestSettings, ASkippedPropsPtr.get(), L"",
						ACompareError)) {

					StateMachineProject->ProjectProperties->TestSettings->Assign(AItem->TestSettings);
					StateMachineProject->SaveProperties();
				}
			}
			else {
				std::auto_ptr<TStringList>ASkippedPropsPtr(new TStringList());
				Unique::CompareError ACompareError;
				if (!Unique::CompareObjects(SettingsData->TestSettings,
						SettingsData->TestApplicationPresets->Items[iItemIndex]->TestSettings, ASkippedPropsPtr.get(), L"",
						ACompareError)) {
					SettingsData->TestSettings->Assign(AItem->TestSettings);
					SettingsData->Save();
				}
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}

}

//---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSyncInvokedExecute(TObject *Sender) {
	try {
		UnicodeString sInvoked = "";
		if (InputQuery("SYNC", "Synchronize invoked session by ID", sInvoked)) {
			if (sInvoked.IsEmpty())
				throw Exception("Invoked ID is empty!");

			SettingsData->SendEvent("SYNC_INVOKED@" + sInvoked);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"Sync Invoked> %s", E->Message.c_str());
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actSyncInvokedUpdate(TObject *Sender) {
	actSyncInvoked->Enabled = SettingsData->IsTesterWorking;
}

//---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::HTMLRecentClickShape(TTreeNodeShape *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	try {
		TAnchorTextShape * AnchorTextShape = dynamic_cast<TAnchorTextShape*>(Sender);
		if (AnchorTextShape && !AnchorTextShape->ItemSelected && !AnchorTextShape->FilePath.IsEmpty()) {
			const UnicodeString sFileName = AnchorTextShape->FilePath;

			const UnicodeString sExt = ExtractFileExt(sFileName).UpperCase();

			if (ProjCloseQuery()) {

				if (TStateMachineProject::FileExt_.LowerCase() == sExt.LowerCase()) {
					DoOpenProject(sFileName);
				}
				else {
					DoOpenUnit(sFileName);
				}
			}

		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::HTMLRecentMouseEnterShape(TTreeNodeShape *Sender, TShiftState Shift, int X, int Y) {
	if (Sender) {
		TAnchorTextShape * AnchorTextShape = dynamic_cast<TAnchorTextShape*>(Sender);
		if (AnchorTextShape && !AnchorTextShape->FilePath.IsEmpty()) {
			Sender->Cursor = crHandPoint;
		}
		else {
			Sender->Cursor = crDefault;
		}
	}

}

//---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::HTMLRecentMouseLeaveShape(TTreeNodeShape *Sender, TShiftState Shift, int X, int Y) {
	if (Sender) {
		Sender->Cursor = crDefault;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormScxmlGui::actTestCoverageExecute(TObject *Sender) {
	// Do not remove this handler !
}

//---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actRevertExecute(TObject *Sender) {
	TStateMachineProject * AStateMachineProject = this->StateMachineProject;
	TStateMachineEditorUnit* AUnit = ActiveUnit;

	UnicodeString sProjectFileName = L"";
	UnicodeString sUnitFileName = L"";
	if (AStateMachineProject) {
		if (!AStateMachineProject->IsNew) {
			sProjectFileName = AStateMachineProject->FilePath;
		}
	}
	else {
		if (AUnit && !AUnit->IsNew) {
			sUnitFileName = AUnit->FilePath;
		}
	}

	if (!sProjectFileName.IsEmpty() || !sUnitFileName.IsEmpty()) {

		UnicodeString sText = UnicodeString().sprintf(L"Revert %s: %s?\nMODIFICATIONS WILL BE LOST!",
			sProjectFileName.IsEmpty() ? L"unit" : L"project", //
			ExtractFileName(sProjectFileName.IsEmpty() ? sUnitFileName : sProjectFileName).c_str());

		if (MessageBoxW(this->Handle, sText.c_str(), L"WARNING", MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) != IDOK)
			return;

		try {

			const int i_MAX_DISPLAY_WAIT_COUNT = 2;

			UnicodeString sActiveUnit = L"";
			if (AUnit && !AUnit->IsNew) {
				sActiveUnit = AUnit->FileName;
			}

			std::vector<TStateMachineEditorUnit*>AVecUnits;
			CollectAllEnabledUnits(this->ProjectManager1->Root, AVecUnits);
			int iOpenCount = 0;
			for (std::size_t i = 0; i < AVecUnits.size(); i++) {
				if (AVecUnits[i]->Opened) {
					iOpenCount++;

					if (iOpenCount > i_MAX_DISPLAY_WAIT_COUNT) {

						if (!DialogWait) {
							DialogWait = new TDialogWait(this);
						}

						break;
					}
				}
			}

			if (DialogWait) {
				DialogWait->ShowDialog("Closing all projects ...");
			}

			try {
				/* this will cancel all user message dialogs */
				ProjectManager1->OnSaveQuery = this->InternalRevertSaveQuery;

				DeleteAllProjects();
			}
			__finally {
				ProjectManager1->OnSaveQuery = this->ProjectManager1SaveQuery;
			}

			if (!sProjectFileName.IsEmpty()) {
				DoOpenProject(sProjectFileName);

				if (!sActiveUnit.IsEmpty()) {
					AVecUnits.clear();
					CollectAllEnabledUnits(this->ProjectManager1->Root, AVecUnits);
					for (std::size_t i = 0; i < AVecUnits.size(); i++) {
						if (SameText(AVecUnits[i]->FileName, sActiveUnit)) {
							if (!AVecUnits[i]->Opened) {
								AVecUnits[i]->Open(true);
							}

							break;
						}
					}
				}
			}
			else if (!sUnitFileName.IsEmpty()) {
				DoOpenUnit(sUnitFileName);
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}

		/* FINALIZE */
		if (DialogWait) {
			delete DialogWait;
			DialogWait = NULL;
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormScxmlGui::actRevertUpdate(TObject *Sender) {
	actRevert->Enabled = (ProjectManager1->Root != NULL) && !SettingsData->IsTesterWorking;
}
//---------------------------------------------------------------------------
