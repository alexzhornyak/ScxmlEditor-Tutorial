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

#ifndef UnitSpawnAndRedirectH
#define UnitSpawnAndRedirectH
//---------------------------------------------------------------------------

#include <boost/thread.hpp>

#include "ConvertEncodingTypes.h"

typedef void __fastcall (__closure *TBeforeLineReadEvent)(UnicodeString &sText);
typedef void __fastcall (__closure *TLineReadEvent)(const UnicodeString &sText);

class TSpawnAndRedirect
{
private:

	HWND FhStopDebuggingWnd;
	DWORD FdwStopMsg;

	UnicodeString FCommandLine;
	UnicodeString FWorkDir;

	TLineReadEvent 			FOnLineRead;
	TBeforeLineReadEvent 	FOnBeforeLineRead;

	std::auto_ptr<TStringList>	FReadListPtr;

	STARTUPINFOW m_si_process;
	PROCESS_INFORMATION m_pi_process;
	HANDLE m_hInputWrite;
	HANDLE m_hOutputRead;
	HWND m_hConsoleWnd;

	TConvertOutputEncoding FConvertOutputEncoding;

    std::auto_ptr<boost::thread> FThreadProcessPtr;

	void PrepAndLaunchRedirectedChild(wchar_t * commandLine, wchar_t *wchDir,
					HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr);

	void DoStopProcess();

	void ThreadReadAndHandleOutput();

	void __fastcall UpdateReadList(); // safe
	void __fastcall LogLastSysError(); // safe

	TCriticalSection *FLock;

public:
	TSpawnAndRedirect(const UnicodeString &sCommandLine, const UnicodeString &sWorkDir, HWND hStopDebuggingWnd = NULL, DWORD dwStopMsg = 0);
	~TSpawnAndRedirect();

	void __fastcall StartProcess();
	void __fastcall StartProcessInfinite();
	void __fastcall StopProcess();

	void __fastcall InitWrite(void); // будет использоваться критическая секция, поэтому только по запросу
	void __fastcall WriteLine(const UnicodeString &sMsg);

	__property TConvertOutputEncoding 	ConvertOutputEncoding = {read=FConvertOutputEncoding, write=FConvertOutputEncoding};
	__property TBeforeLineReadEvent 	OnBeforeLineRead = {read=FOnBeforeLineRead, write=FOnBeforeLineRead};
	__property TLineReadEvent 			OnLineRead = {read=FOnLineRead, write=FOnLineRead};
};

extern void SpawnInfinite(const UnicodeString &sCmd, const UnicodeString sWorkDir = L"", const bool bHide = false);

#endif
