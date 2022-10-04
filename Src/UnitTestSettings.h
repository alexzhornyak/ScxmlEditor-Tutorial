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

#ifndef UnitTestSettingsH
#define UnitTestSettingsH
//---------------------------------------------------------------------------

#include "LMDInsPropEditors.hpp"
#include "UnitCaptureOutputRegExps.h"
#include "ConvertEncodingTypes.h"
#include "UnicodeStringUtils.h"
#include "UnitMacroComposer.h"

#define USCXML_TESTER_EXE		L".\\UScxmlTester\\uscxml_tester.exe"
#define USCXML_OLD_TESTER_EXE	L"uscxml_tester_2.exe"
#define QT_SCXML_TESTER_EXE		L".\\QtScxmlTester\\QtScxmlTester.exe"
#define SCION_SCXML_TESTER_EXE	L"%APPDATA%\\npm\\scion.cmd"

typedef enum {tsbtApp, tsbtProject} TTestSettingsBaseType;

typedef enum { tpUDP, tpPipes } TTriggersProtocol;

typedef enum { tmfXML, tmfJSON } TTriggersMsgFormat;

class TTestSettings: public TPersistent {
__published:
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	bool 			FTestingEnabled;
	bool __fastcall GetTestingEnabled(void);
	void __fastcall SetTestingEnabled(bool val);

	UnicodeString 	FTestWorkDir;
	UnicodeString 	FTestCMDParams;
	bool 			FTestUseCustomParams;
	bool 			FTestingScxmlMonitorLog;
	AnsiString 		FTestRemoteHost;
	inline bool __fastcall IsRemoteHostStored(void) { return !SameText(FTestRemoteHost, L"localhost"); }
	int 			FTestRemotePort;
	int 			FTestLocalPort;

	bool 			FTestingStopOnExit;
	bool 			FTestingCheckIssues;

	UnicodeString 	FTestExeName;
	inline UnicodeString __fastcall GetTestExeName(void) { return FTestExeName.IsEmpty() ? UnicodeString(USCXML_TESTER_EXE) : FTestExeName; }
	inline bool __fastcall IsTestExeNameStored(void) { return !SameText(ExeName, USCXML_TESTER_EXE); }

	bool 	FTestHttpEnabled;
	int		FTestHttpListenPort;
	int		FTestHttpWebSocket;
	bool 	FTestHideDebugInfo;

	bool	FConvertOemToAnsi;

	inline UnicodeString __fastcall GetFullPath(void) { return GetExeFullPath(ExeName); }

	TTriggersProtocol  FTriggersProtocol;
	TTriggersMsgFormat FTriggersMsgFormat;

	TCaptureOutputRegexItems *	FCaptureOutputRegExps;
	inline void __fastcall SetCaptureOutputRegExps(TCaptureOutputRegexItems *val) { FCaptureOutputRegExps->Assign(val); }

	TConvertOutputEncoding FConvertOutputEncoding;

protected:

public:
	__fastcall TTestSettings(const TTestSettingsBaseType ABaseType);
	virtual __fastcall ~TTestSettings(void);

	virtual void __fastcall Assign(TPersistent* Source);

    bool IsInheritedFromProjectOptions();

	const TTestSettingsBaseType BaseType;

	static UnicodeString GetExeFullPath(UnicodeString sExeName);

	static UnicodeString GetExpandedCMD(UnicodeString sCMD, const UnicodeString sBaseFilePath);

__published:
	__property bool 			            Enabled = {read=FTestingEnabled, write=FTestingEnabled, default=false};
	__property UnicodeString 	            ExeName = {read=GetTestExeName, write=FTestExeName, stored=IsTestExeNameStored};
	__property UnicodeString	            FullPath = {read=GetFullPath};
	__property UnicodeString 	            WorkDir = {read=FTestWorkDir, write=FTestWorkDir};
	__property UnicodeString 	            CMDParams = {read=FTestCMDParams, write=FTestCMDParams};
	__property bool 			            UseCustomParamsOnly = {read=FTestUseCustomParams, write=FTestUseCustomParams, default=false};
	__property AnsiString 		            RemoteHost = {read=FTestRemoteHost, write=FTestRemoteHost, stored=IsRemoteHostStored};
	__property int		  		            RemotePort = {read=FTestRemotePort, write=FTestRemotePort, default=11001};
	__property int		  		            LocalPort = {read=FTestLocalPort, write=FTestLocalPort, default=11005};
	__property TConvertOutputEncoding       ConvertOutputEncoding = {read=FConvertOutputEncoding, write=FConvertOutputEncoding, default=coeDefault};
	__property bool				            HttpEnabled	= {read=FTestHttpEnabled, write=FTestHttpEnabled, default=false};
	__property int		  		            HttpListenPort = {read=FTestHttpListenPort, write=FTestHttpListenPort, default=7080};
	__property int		  		            HttpWebSocket = {read=FTestHttpWebSocket, write=FTestHttpWebSocket, default=7443};
	__property bool				            HideDebugInfo = {read=FTestHideDebugInfo, write=FTestHideDebugInfo, default=false};
	__property bool 			            ScxmlMonitorLog = {read=FTestingScxmlMonitorLog, write=FTestingScxmlMonitorLog,default=false};
	__property bool 			            StopOnExit = {read=FTestingStopOnExit, write=FTestingStopOnExit,default=false};
	__property bool 			            CheckIssues = {read=FTestingCheckIssues, write=FTestingCheckIssues, default=true};
	__property TTriggersProtocol	        TriggersProtocol = {read=FTriggersProtocol, write=FTriggersProtocol, default=tpUDP};
	__property TTriggersMsgFormat	        TriggersMsgFormat = {read=FTriggersMsgFormat, write=FTriggersMsgFormat, default=tmfXML};
	__property TCaptureOutputRegexItems *	CaptureOutputRegExps = {read=FCaptureOutputRegExps, write=SetCaptureOutputRegExps};

	/* DEPRECATED */
	__property bool ConvertOemToAnsi = {read=FConvertOemToAnsi, write=FConvertOemToAnsi, default=false};
};

/* PROP EDITORS */
//---------------------------------------------------------------------------
class TTestSettingsPropEditor : public TLMDClassPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
};

class TTestingCMDStringPropEditor : public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();

	virtual void __fastcall Edit(void);
};

#endif
