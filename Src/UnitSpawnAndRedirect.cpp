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

#include "UnitSpawnAndRedirect.h"

#include "Log4cpp_VCL.hpp"

#include <DateUtils.hpp>

#include <boost/bind.hpp>
#include "UnitMain.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// ------------------------- TSpawnAndRedirect -------------------------------
// ---------------------------------------------------------------------------

TSpawnAndRedirect::TSpawnAndRedirect(const UnicodeString &sCommandLine, const UnicodeString &sWorkDir, HWND hStopDebuggingWnd,
	DWORD dwStopMsg) {

	FLock = NULL;

	FdwStopMsg = dwStopMsg;
	FhStopDebuggingWnd = hStopDebuggingWnd;
	FCommandLine = sCommandLine;
	FWorkDir = sWorkDir;
	m_hInputWrite = NULL;
	m_hOutputRead = NULL;
	FOnLineRead = NULL;
	FOnBeforeLineRead = NULL;
	ZeroMemory(&m_pi_process, sizeof(m_pi_process));
	ZeroMemory(&m_si_process, sizeof(m_si_process));

	FConvertOutputEncoding = coeDefault;
}

// ---------------------------------------------------------------------------
TSpawnAndRedirect::~TSpawnAndRedirect() {
	StopProcess();
	if (FLock) {
		delete FLock;
		FLock = NULL;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSpawnAndRedirect::StartProcess() {
	HANDLE hChildProcess = NULL;

	HANDLE hOutputReadTmp, hOutputWrite;
	HANDLE hInputWriteTmp, hInputRead;
	HANDLE hErrorWrite;
	HANDLE hThread;
	DWORD ThreadId;
	SECURITY_ATTRIBUTES sa;

	// Set up the security attributes struct.
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	m_hConsoleWnd = NULL;

	// Create the child output pipe.
	if (!CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 0))
		throw Exception("CreatePipe" + SysErrorMessage(GetLastError()));

	// Create a duplicate of the output write handle for the std error
	// write handle. This is necessary in case the child application
	// closes one of its std output handles.
	if (!DuplicateHandle(GetCurrentProcess(), hOutputWrite, GetCurrentProcess(), &hErrorWrite, 0, TRUE, DUPLICATE_SAME_ACCESS))
		throw Exception("DuplciateHandle>" + SysErrorMessage(GetLastError()));

	// Create the child input pipe.
	if (!CreatePipe(&hInputRead, &hInputWriteTmp, &sa, 0))
		throw Exception("CreatePipe" + SysErrorMessage(GetLastError()));

	// Create new output read handle and the input write handles. Set
	// the Properties to FALSE. Otherwise, the child inherits the
	// properties and, as a result, non-closeable handles to the pipes
	// are created.
	if (!DuplicateHandle(GetCurrentProcess(), hOutputReadTmp, GetCurrentProcess(), &m_hOutputRead, // Address of new handle.
			0, FALSE, // Make it uninheritable.
			DUPLICATE_SAME_ACCESS))
		throw Exception("DuplicateHandle>" + SysErrorMessage(GetLastError()));

	if (!DuplicateHandle(GetCurrentProcess(), hInputWriteTmp, GetCurrentProcess(), &m_hInputWrite, // Address of new handle.
			0, FALSE, // Make it uninheritable.
			DUPLICATE_SAME_ACCESS))
		throw Exception("DuplicateHandle>" + SysErrorMessage(GetLastError()));

	// Close inheritable copies of the handles you do not want to be
	// inherited.
	if (!CloseHandle(hOutputReadTmp))
		throw Exception("CloseHandle>" + SysErrorMessage(GetLastError()));
	if (!CloseHandle(hInputWriteTmp))
		throw Exception("CloseHandle>" + SysErrorMessage(GetLastError()));

	PrepAndLaunchRedirectedChild(FCommandLine.c_str(), FWorkDir.IsEmpty() ? 0 : FWorkDir.c_str(), hOutputWrite, hInputRead, hErrorWrite);

	// Close pipe handles (do not continue to modify the parent).
	// You need to make sure that no handles to the write end of the
	// output pipe are maintained in this process or else the pipe will
	// not close when the child process exits and the ReadFile will hang.
	if (!CloseHandle(hOutputWrite))
		throw Exception("CloseHandle>" + SysErrorMessage(GetLastError()));
	if (!CloseHandle(hInputRead))
		throw Exception("CloseHandle>" + SysErrorMessage(GetLastError()));
	if (!CloseHandle(hErrorWrite))
		throw Exception("CloseHandle>" + SysErrorMessage(GetLastError()));

	// Read the child's output.
	FThreadProcessPtr.reset(new boost:: thread(boost::bind(&TSpawnAndRedirect::ThreadReadAndHandleOutput, this)));
	// Redirection is complete

}

// ---------------------------------------------------------------------------
void __fastcall TSpawnAndRedirect::StartProcessInfinite() {
	StartProcess();

	while (m_pi_process.hProcess) {
		Application->ProcessMessages();
	}

}

// ---------------------------------------------------------------------------
void TSpawnAndRedirect::DoStopProcess() {
	if (m_hOutputRead && m_hInputWrite) {
		if (!CloseHandle(m_hOutputRead))
			throw Exception("CloseHandle>" + SysErrorMessage(GetLastError()));
		if (!CloseHandle(m_hInputWrite))
			throw Exception("CloseHandle>" + SysErrorMessage(GetLastError()));

		m_hInputWrite = 0;
		m_hOutputRead = 0;
	}

	if (m_pi_process.hProcess) {
		DWORD dwExitCode;
		if (GetExitCodeProcess(m_pi_process.hProcess, &dwExitCode) && dwExitCode == STILL_ACTIVE) {
			TerminateProcess(m_pi_process.hProcess, STILL_ACTIVE);
		}

		CloseHandle(m_pi_process.hProcess);

		ZeroMemory(&m_pi_process, sizeof(m_pi_process));
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSpawnAndRedirect::StopProcess() {
	// уже остановлено
	if (!m_pi_process.hProcess)
		return;

	if (FThreadProcessPtr.get()) {
		FThreadProcessPtr->interrupt();
		FThreadProcessPtr->timed_join(boost::posix_time::seconds(5));
	}

	DoStopProcess();
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PrepAndLaunchRedirectedChild
// Sets up STARTUPINFO structure, and launches redirected child.
///////////////////////////////////////////////////////////////////////
void TSpawnAndRedirect::PrepAndLaunchRedirectedChild(wchar_t * commandLine, wchar_t *wchDir, HANDLE hChildStdOut, HANDLE hChildStdIn,
	HANDLE hChildStdErr) {
	ZeroMemory(&m_pi_process, sizeof(PROCESS_INFORMATION));
	// Set up the start up info struct.
	ZeroMemory(&m_si_process, sizeof(STARTUPINFOW));
	m_si_process.cb = sizeof(STARTUPINFOW);
	m_si_process.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	m_si_process.hStdOutput = hChildStdOut;
	m_si_process.hStdInput = hChildStdIn;
	m_si_process.hStdError = hChildStdErr;
	// Use this if you want to hide the child:
	m_si_process.wShowWindow = SW_HIDE;
	// Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
	// use the wShowWindow flags.

	// Launch the process that you want to redirect (in this case,
	// Child.exe). Make sure Child.exe is in the same directory as
	// redirect.c launch redirect from a command line to prevent location
	// confusion.
	if (!CreateProcessW(NULL, commandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, wchDir, &m_si_process, &m_pi_process))
		throw Exception(UnicodeString().sprintf(L"CreateProcess> cmd=[%s] workdir=[%s] errormsg=%s", commandLine, wchDir,
			SysErrorMessage(GetLastError()).c_str()));

	// Close any unnecessary handles.
	if (!CloseHandle(m_pi_process.hThread))
		throw Exception(UnicodeString().sprintf(L"CloseHandle> cmd=[%s] workdir=[%s] errormsg=%s", commandLine, wchDir,
			SysErrorMessage(GetLastError()).c_str()));
}

///////////////////////////////////////////////////////////////////////
// ReadAndHandleOutput
// Monitors handle for input. Exits when child exits or pipe breaks.
///////////////////////////////////////////////////////////////////////
void TSpawnAndRedirect::ThreadReadAndHandleOutput() {
	try {
		const int i_BUFF_SIZE = 1024;

		TDateTime dtLastReadTime = Now();

		// while the process is running
		int iPipeState = ERROR_SUCCESS;
		while (iPipeState == ERROR_SUCCESS) {
			DWORD nBytesRead;
			DWORD nBytesAvail;
			CHAR lpBuffer[i_BUFF_SIZE] = "";

			if (FLock) {
				FLock->Acquire();
			}

			if (PeekNamedPipe(m_hOutputRead, lpBuffer, i_BUFF_SIZE, &nBytesRead, &nBytesAvail, NULL) && nBytesRead != 0) {
				FReadListPtr.reset(new TStringList());
				do {
					if (ReadFile(m_hOutputRead, lpBuffer, i_BUFF_SIZE, &nBytesRead, NULL) && nBytesRead != 0) {
						lpBuffer[nBytesRead] = '\0';
						const int iSize = strlen(lpBuffer);
						char chBuff[i_BUFF_SIZE + 1] = "";

						// если в режиме CMD, тогда конвертируем из OEM
						switch(FConvertOutputEncoding) {
						case coeOEM866: {
								if (OemToCharA(lpBuffer, chBuff)) {
									FReadListPtr->Text = FReadListPtr->Text + chBuff;
								}
							}break;
						case coeUTF8: {
								FReadListPtr->Text = FReadListPtr->Text + UTF8ToUnicodeString(lpBuffer);
							}break;
						default:
							// текущая кодировка приложения
							FReadListPtr->Text = FReadListPtr->Text + UnicodeString(lpBuffer);
						}
					}
					else {
						break;
					}
				}
				while (nBytesRead >= i_BUFF_SIZE);

				if (FLock) {
					FLock->Release();
				}

				TThread::Synchronize(NULL, &UpdateReadList);

				dtLastReadTime = Now();
			}
			else {

				if (FLock) {
					FLock->Release();
				}

				iPipeState = GetLastError();

				if (iPipeState != ERROR_SUCCESS && iPipeState != ERROR_BROKEN_PIPE) {
					// что-то нехорошее происходит
					TThread::Synchronize(NULL, &LogLastSysError);

					dtLastReadTime = Now();
				}
			}

			// создаем умный слип, то есть, если сообщения валят постоянным потоком, тогда отображаем без Sleep
			// а если приложение простаивает, тогда через Sleep
			if (MilliSecondsBetween(Now(), dtLastReadTime) > 16) {
				Sleep(1);
			}

			boost::this_thread::interruption_point();
		}

		DoStopProcess();

		if (FhStopDebuggingWnd) {
			PostMessage(FhStopDebuggingWnd, FdwStopMsg, 0, 0);
		}

	}
	catch(boost::thread_interrupted & e) {
		// прервано пользователем
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSpawnAndRedirect::InitWrite(void) {
	if (!FLock) {
		FLock = new TCriticalSection;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSpawnAndRedirect::WriteLine(const UnicodeString &sMsg) {
	if (!FLock) {
		LOG_ERROR_SS << "Call 'SpawnAndRedirect->InitWrite' to initialize console write option!";
		return;
	}

	if (sMsg.IsEmpty()) {
		return;
	}

	FLock->Acquire();
	try {
		ULONG nBytes = 0;

		bool bRes = false;

		switch(FConvertOutputEncoding) {
		case coeOEM866: {
				std::unique_ptr<char, std::default_delete<char[]> >chBuffPtr(new char[sMsg.Length() + 1]);
				memset(chBuffPtr.get(), 0, sizeof(char) * (sMsg.Length() + 1));
				if (CharToOemW(sMsg.c_str(), chBuffPtr.get())) {
					bRes = WriteFile(m_hInputWrite, chBuffPtr.get(), strlen(chBuffPtr.get()), &nBytes, NULL);
				}
			}break;
		case coeUTF8: {
				const UTF8String sOutput = UTF8Encode(sMsg);
				bRes = WriteFile(m_hInputWrite, sOutput.c_str(), sOutput.Length(), &nBytes, NULL);
			}break;
		default: {
				const AnsiString sOutput = sMsg;
				bRes = WriteFile(m_hInputWrite, sOutput.c_str(), sOutput.Length(), &nBytes, NULL);
			}break;
		}

		if (!bRes) {
			WLOG_ERROR(L"Failed to WriteFile. Text:%s", sMsg.c_str());
		}
		else {
			FormScxmlGui->EditDocTester->BeginUpdate();
			try {
				FormScxmlGui->EditDocTester->Insert(FormScxmlGui->EditDocTester->CharsCount, sMsg.TrimRight());
			}
			__finally {
				FormScxmlGui->EditDocTester->EndUpdate();
			}
			if (FormScxmlGui->TabControlLog->TabIndex == 1) {
				FormScxmlGui->ScrollLog();
			}
		}
	}
	__finally {
		FLock->Release();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSpawnAndRedirect::LogLastSysError() {
	WLOG_WARNING(L"SpawnAndRedirect> %s", SysErrorMessage(GetLastError()).c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TSpawnAndRedirect::UpdateReadList() {
	for (int i = 0; i < FReadListPtr->Count; i++) {

		if (!FReadListPtr->Strings[i].IsEmpty()) {
			UnicodeString sLine = L"";
			if (FOnBeforeLineRead) {
				sLine = FReadListPtr->Strings[i];
				FOnBeforeLineRead(sLine);
			}

			FormScxmlGui->EditDocTester->AppendLine(FOnBeforeLineRead ? sLine : FReadListPtr->Strings[i]);
			if (FormScxmlGui->TabControlLog->TabIndex == 1) {
				FormScxmlGui->ScrollLog();
			}

			if (FOnLineRead) {
				FOnLineRead(FOnBeforeLineRead ? sLine : FReadListPtr->Strings[i]);
			}
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void SpawnInfinite(const UnicodeString &sCmd, const UnicodeString sWorkDir/* = L""*/, const bool bHide/* = false*/) {
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	if (bHide) {
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process.
	if (!CreateProcessW(NULL, // No module name (use command line)
			sCmd.c_str(), // Command line
			NULL, // Process handle not inheritable
			NULL, // Thread handle not inheritable
			FALSE, // Set handle inheritance to FALSE
			0, // No creation flags
			NULL, // Use parent's environment block
			sWorkDir.IsEmpty() ? NULL : sWorkDir.c_str(), // Use parent's starting directory
			&si, // Pointer to STARTUPINFO structure
			&pi) // Pointer to PROCESS_INFORMATION structure
		) {
		throw Exception(SysErrorMessage(GetLastError()));
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
