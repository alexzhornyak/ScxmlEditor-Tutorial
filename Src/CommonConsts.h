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

#ifndef __COMMON_CONSTS_H__
#define	__COMMON_CONSTS_H__

#define WM_SCXML_STOP_DEBUGGING						WM_USER+100
#define WM_SCXML_DELETE_OBJECTS						WM_USER+101
#define WM_SCXML_POST_EDIT_CHANGE					WM_USER+102 	// fix: last letter does not exist in OnChange Edit
#define WM_SCXML_LUAC_CONVERTED						WM_USER+103
#define WM_SCXML_ANSI_C_CONVERTED					WM_USER+104
#define WM_SCXML_QT_CONVERTED           			WM_USER+105
#define WM_SCXML_DELETE_TREE_SHAPE                  WM_USER+106
#define WM_SCXML_ADD_OR_OPEN_EXISTING_UNIT			WM_USER+107
#define WM_SCXML_UPDATE_DBG_MSG_QUEUE				WM_USER+108

#include <map>

typedef enum {
	estStateText, estTransitionText, estAllProperties,
	estWholeWord, estCaseSensitive, estTrim,
	estSelectedStatesOnly, estUseRegexp} TEditorSearchType;
typedef Set<TEditorSearchType,estStateText,estUseRegexp> TEditorSearchTypes;

typedef enum {esdtCurrentDoc, esdtOpenedDocs, esdtProject} TEditorSearchDocumentType;

typedef std::map<UnicodeString, UnicodeString> TUnicodeStringMap;

typedef enum { oresSuccess, oresError, oresCancel } TOperationResult;

struct TMatchFind {
	UnicodeString sLeft;
	UnicodeString sWhat;
	UnicodeString sRight;
	UnicodeString sHTMLOut;

	TMatchFind() {
	   sLeft = L"";
	   sWhat = L"";
	   sRight = L"";
	   sHTMLOut = L"";
	}

	UnicodeString GetText(void) const {
		if (sHTMLOut.IsEmpty()) {
			return sLeft + sWhat + sRight;
		}

		return sHTMLOut;
	}

	bool IsEmpty() {
		return sLeft.IsEmpty() && sWhat.IsEmpty() && sRight.IsEmpty();
	}
};

#define SCXML_INTERPROCESS_COMMAND			L"command"
#define SCXML_INTERPROCESS_BUFFER_SIZE      4096

struct INTERPROCESS_COPYDATA
{
	wchar_t wchCommand[SCXML_INTERPROCESS_BUFFER_SIZE];
	wchar_t wchArg1[SCXML_INTERPROCESS_BUFFER_SIZE];
	wchar_t wchArg2[SCXML_INTERPROCESS_BUFFER_SIZE];
	wchar_t wchArg3[SCXML_INTERPROCESS_BUFFER_SIZE];
};

#endif
