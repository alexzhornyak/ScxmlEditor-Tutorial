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

#include "UnitTreeLoader.h"

#include "Log4cpp_VCL.hpp"
#include "UnitTreeEx.h"
#include "UnitSettings.h"
#include "UnitCustomUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool g_PARSING_ERRORS = false;

bool __fastcall TLogTreeReader::Error(const System::UnicodeString Message) {
	bool bRes = TReader::Error(Message);
	if (!bRes) {
		g_PARSING_ERRORS = true;
		WLOG_ERROR(L"StreamReader: %s", Message.c_str());
	}
	return true;
}

void __fastcall LoadTreeFromStreamEx(TCustomTree* &ATree, Classes::TStream* AStream) {
	g_PARSING_ERRORS = false;

	LoadTreeFromStream(ATree, AStream, __classid(TLogTreeReader));

	if (g_PARSING_ERRORS) {
		CheckTreeVersion(ATree);
	}
}

void __fastcall CheckTreeVersion(TCustomTree* ATree) {
	TTreeEx * ATreeEx = dynamic_cast<TTreeEx*>(ATree);
	if (ATreeEx) {
		unsigned __int64 ATreeVersion = Customutils::ConvertVersionToNumber(ATreeEx->ApplicationVersion);
		unsigned __int64 AppVersion = Customutils::ConvertVersionToNumber(SettingsData->AppVersion);

		if (ATreeVersion != AppVersion) {
			WLOG_ERROR(L"Parsing errors could be caused by version mismatch! AppVersion:[%s] %s ChartVersion:[%s]",
				SettingsData->AppVersion.c_str(), //
				AppVersion > ATreeVersion ? L">" : L"<", //
				ATreeVersion == 0 ? L"UNVERSIONED" : ATreeEx->ApplicationVersion.c_str());
		}
	}
}
