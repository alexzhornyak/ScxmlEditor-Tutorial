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

#include "UnitCustomUtils.h"

#include <memory>

#include "Log4cpp_VCL.hpp"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Customutils {

	//---------------------------------------------------------------------------
	const UnicodeString RectToStr(const TRect &ARect) {
		return UnicodeString(ARect.left) + "," + UnicodeString(ARect.top) + "," + UnicodeString(ARect.right) + "," + UnicodeString
		(ARect.bottom);
	}

	//---------------------------------------------------------------------------
	const TRect StrToRect(const UnicodeString &sText, const TRect ADefaultRect/* = TRect(0, 0, 0, 0)*/) {
		TRect ARetRect = ADefaultRect;

		std::auto_ptr<TStringList>AStringList(new TStringList());
		AStringList->CommaText = sText;

		if (AStringList->Count == 4) {
			int iLeft, iTop, iRight, iBottom;

			if (TryStrToInt(AStringList->Strings[0], iLeft) &&

				TryStrToInt(AStringList->Strings[1], iTop) &&

				TryStrToInt(AStringList->Strings[2], iRight) &&

				TryStrToInt(AStringList->Strings[3], iBottom)) {

				ARetRect.left = iLeft;
				ARetRect.top = iTop;
				ARetRect.right = iRight;
				ARetRect.bottom = iBottom;
			}
		}
		return ARetRect;
	}

	// ---------------------------------------------------------------------------
	void GetVersionOfFileW(wchar_t * pszAppName, // file
		wchar_t * pszVerBuff, // receives version
		int iVerBuffLen, // size of buffer
		wchar_t * pszLangBuff, // receives language
		int iLangBuffLen) // size of buffer
	{
		DWORD dwScratch;
		DWORD * pdwLangChar;
		DWORD dwInfSize;
		UINT uSize;
		BYTE * pbyInfBuff;
		wchar_t szVersion[32];
		wchar_t szResource[80];
		wchar_t * pszVersion = szVersion;

		dwInfSize = GetFileVersionInfoSizeW(pszAppName, &dwScratch);

		if (dwInfSize) {
			pbyInfBuff = new BYTE[dwInfSize];
			for (DWORD i = 0; i < dwInfSize; i++) {
				pbyInfBuff[i] = 0;
			}
			memset(pbyInfBuff, 0, dwInfSize);

			if (GetFileVersionInfoW(pszAppName, 0, dwInfSize, pbyInfBuff)) {
				if (VerQueryValueW(pbyInfBuff, L"\\VarFileInfo\\Translation", (void**)(&pdwLangChar), &uSize)) {
					if (VerLanguageNameW(LOWORD(*pdwLangChar), szResource, sizeof(szResource))) {
						wcsncpy(pszLangBuff, szResource, iLangBuffLen);
					}
					wsprintfW(szResource, L"\\StringFileInfo\\%04X%04X\\FileVersion", LOWORD(*pdwLangChar), HIWORD(*pdwLangChar));

					if (VerQueryValueW(pbyInfBuff, szResource, (void**)(&pszVersion), &uSize)) {
						wcsncpy(pszVerBuff, pszVersion, iVerBuffLen - 1);
					}
				}
			}
			delete[]pbyInfBuff;
		}
	}

	void GetAppVersion(UnicodeString &sVersion, UnicodeString &sLanguage) {
		wchar_t wchVersion[MAX_PATH];
		wchar_t wchLanguage[MAX_PATH];
		GetVersionOfFileW(Application->ExeName.c_str(), wchVersion, MAX_PATH, wchLanguage, MAX_PATH);

		sVersion = wchVersion;
		sLanguage = wchLanguage;
	}

	void GetFullPropPathList(TComponent *AComponent, TStrings *AOutList) {
		while (AComponent && AOutList) {
			if (!AComponent->ClassNameIs("TApplication")) {
				AOutList->Insert(0, AComponent->Name);
			}

			AComponent = AComponent->Owner;
		}
	}

	UnicodeString GetFullPropPath(TComponent *AComponent, const wchar_t wchDelimiter/* = L'.'*/ ) {
		std::auto_ptr<TStringList>AStringListPtr(new TStringList());
		AStringListPtr->StrictDelimiter = true;
		AStringListPtr->Delimiter = wchDelimiter;

		GetFullPropPathList(AComponent, AStringListPtr.get());

		return AStringListPtr->DelimitedText;
	}

	bool ComponentIsPresentInPath(TComponent *AComponent, const UnicodeString &sGivenPath, const wchar_t wchDelimiter/* = L'.'*/) {
		if (!AComponent)
			return false;

		std::auto_ptr<TStringList>ASourceListPtr(new TStringList());
		ASourceListPtr->StrictDelimiter = true;
		ASourceListPtr->Delimiter = wchDelimiter;
		ASourceListPtr->DelimitedText = sGivenPath;

		std::auto_ptr<TStringList>ATargetListPtr(new TStringList());
		ATargetListPtr->StrictDelimiter = true;
		ATargetListPtr->Delimiter = wchDelimiter;
		GetFullPropPathList(AComponent, ATargetListPtr.get());

		if (!ATargetListPtr->Count || !ASourceListPtr->Count)
			return false;

		for (int i = 0; i < ASourceListPtr->Count; i++) {
			if (ATargetListPtr->Count <= i)
				break;

			if (ASourceListPtr->Strings[i] != ATargetListPtr->Strings[i])
				return false;
		}

		return true;
	}

	UnicodeString ClippedText(const UnicodeString &sText, const int iMaxSymbols) {
		if (sText.Length() > iMaxSymbols) {
			return sText.SubString(1, iMaxSymbols) + L"...";
		}
		else {
			return sText;
		}
	}

	TControl *GetLastControl(TWinControl *AParent) {
		if (!AParent)
			throw Exception(UnicodeString(__FUNCTION__) + L"> Parent can not be NULL!");

		if (AParent->ControlCount) {
			return AParent->Controls[AParent->ControlCount - 1];
		}

		return NULL;
	}

	int GetWinControlAlignedChildrenHeight(TWinControl *AParent) {
		if (!AParent)
			throw Exception(UnicodeString(__FUNCTION__) + L"> Parent can not be NULL!");

		int iTop = 0;
		for (int i = 0; i < AParent->ControlCount; i++) {
			iTop += AParent->Controls[i]->Height;
			if (AParent->Controls[i]->Align && AParent->Controls[i]->AlignWithMargins) {
				iTop += AParent->Controls[i]->Margins->Top;
				iTop += AParent->Controls[i]->Margins->Bottom;
			}
		}

		return iTop;
	}

	unsigned __int64 ConvertVersionToNumber(const UnicodeString &sVersion) {
		std::auto_ptr<TStringList>AStringListPtr(new TStringList());
		AStringListPtr->Delimiter = L'.';
		AStringListPtr->StrictDelimiter = true;
		AStringListPtr->DelimitedText = sVersion;
		if (AStringListPtr->Count == 4) {

			union {
				union {
					unsigned short int V1;
					unsigned short int V2;
					unsigned short int V3;
					unsigned short int V4;
				}VSplitNumbers;

				unsigned __int64 V;
			}AVersion;

			AVersion.VSplitNumbers.V1 = AStringListPtr->Strings[0].ToIntDef(0);
			AVersion.VSplitNumbers.V2 = AStringListPtr->Strings[1].ToIntDef(0);
			AVersion.VSplitNumbers.V3 = AStringListPtr->Strings[2].ToIntDef(0);
			AVersion.VSplitNumbers.V4 = AStringListPtr->Strings[3].ToIntDef(0);

			return AVersion.V;
		}

		return 0;
	}
};
