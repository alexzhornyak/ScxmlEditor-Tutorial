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

#include "UnitHTMLUtils.h"

#include <XMLDoc.hpp>
#include "Log4cpp_VCL.hpp"
//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Htmlutils {

	const UnicodeString CreateHTMLErrorStr(const UnicodeString &sString) {
		return "<FONT color=" + AnsiQuotedStr(ColorToWebColorStr(clRed), '"') + ">" + sString + "</FONT>";
	}

	// ---------------------------------------------------------------------------
	const UnicodeString StrToHTMLColorStr(const UnicodeString &sString, const TColor AColor, const bool bBold/* = false*/) {
		const UnicodeString sOutput = bBold ? StrToBoldStr(sString) : StrXHtmlEncodeEx(sString);
		return "<FONT color=" + AnsiQuotedStr(ColorToWebColorStr(AColor), '"') + ">" + sOutput + "</FONT>";
	}

	// ---------------------------------------------------------------------------
	const UnicodeString StrToHTMLColorStr2(const UnicodeString & sString, const TColor AColor/* = clNone*/,
		const UnicodeString sFontName/* = L"" */, const bool bBold/* = false*/, const int iSize/* = -1*/) {
		const UnicodeString sOutput = bBold ? StrToBoldStr(sString) : StrXHtmlEncodeEx(sString);

		return UnicodeString().sprintf(L"<FONT%s%s%s>%s</FONT>",
			AColor == Graphics::clNone ? L"" : (UnicodeString(L" color=") + AnsiQuotedStr(ColorToWebColorStr(AColor), L'"')).c_str(), //
			sFontName.IsEmpty() ? L"" : (UnicodeString(L" face=") + AnsiQuotedStr(sFontName, L'"')).c_str(), //
			iSize == -1 ? L"" : (UnicodeString(L" size=") + AnsiQuotedStr(iSize, L'"')).c_str(), sOutput.c_str());
	}

	// ---------------------------------------------------------------------------
	const UnicodeString BoldStrToStr(const UnicodeString & sBoldStr) {
		UnicodeString sText = StringReplace(sBoldStr, "<b>", "", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
		sText = StringReplace(sText, "</b>", "", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
		return StrXHtmlDecodeEx(sText).Trim();
	}

	// ---------------------------------------------------------------------------
	const UnicodeString StrToBoldStr(const UnicodeString & sStr) {
		return "<b>" + StrXHtmlEncodeEx(sStr) + "</b>";
	}

	// ---------------------------------------------------------------------------
	const UnicodeString HTMLColorStrToStr(const UnicodeString &sHtml) {
		try {
			_di_IXMLDocument ADoc = LoadXMLData(sHtml);
			if (!ADoc)
				throw Exception("Can not load XML [" + sHtml + "]");

			if (!ADoc->DocumentElement)
				throw Exception("DocumentElement is NULL in XML [" + sHtml + "]");

			if (!SameText(ADoc->DocumentElement->NodeName, "Font"))
				throw Exception("Require <Font> to be as DocumentElement but got <" + ADoc->DocumentElement->NodeName + ">!");

			for (int i = 0; i < ADoc->DocumentElement->ChildNodes->Count; i++) {
				_di_IXMLNode ABoldNode = ADoc->DocumentElement->ChildNodes->Nodes[i];
				if (SameText(ABoldNode->NodeName, "B")) {
					return ABoldNode->Text;
				}
			}

			return ADoc->DocumentElement->Text;

		}
		catch(Exception * E) {
			WLOG_WARNING(L"HTMLColorStrToStr> %s", E->Message.c_str());
		}
		return sHtml;
	}

	// ---------------------------------------------------------------------------
	const UnicodeString StrXHtmlEncodeEx(const UnicodeString &sSource) {
		UnicodeString sResult = StringReplace(sSource, "&", "&amp;", TReplaceFlags() << rfReplaceAll);
		sResult = StringReplace(sResult, "<", "&lt;", TReplaceFlags() << rfReplaceAll);
		sResult = StringReplace(sResult, ">", "&gt;", TReplaceFlags() << rfReplaceAll);
		sResult = StringReplace(sResult, '"', "&quot;", TReplaceFlags() << rfReplaceAll);
		sResult = StringReplace(sResult, "'", "&apos;", TReplaceFlags() << rfReplaceAll);
		return sResult;
	}

	const UnicodeString StrXHtmlDecodeEx(const UnicodeString &sSource) {
		UnicodeString sResult = StringReplace(sSource, "&apos;", "'", TReplaceFlags() << rfReplaceAll);
		sResult = StringReplace(sResult, "&quot;", '"', TReplaceFlags() << rfReplaceAll);
		sResult = StringReplace(sResult, "&gt;", ">", TReplaceFlags() << rfReplaceAll);
		sResult = StringReplace(sResult, "&lt;", "<", TReplaceFlags() << rfReplaceAll);
		sResult = StringReplace(sResult, "&amp;", "&", TReplaceFlags() << rfReplaceAll);
		return sResult;
	}

	const UnicodeString HTMLStrWithImageIndex(const int iImageIndex) {
		return UnicodeString().sprintf(L"<IMG src=\"idx:%d\"/>", iImageIndex);
	}

	const UnicodeString StrWithBoldSearchText(const UnicodeString &sText, const UnicodeString &sSearch, int * iFindPos/* = NULL*/) {

		if (iFindPos) {
			*iFindPos = 0;
		}

		if (!sSearch.IsEmpty()) {
			const int iMatchPos = sText.UpperCase().Pos(sSearch.UpperCase());
			const int iMatchLength = sSearch.Length();

			if (iMatchPos) {

				if (iFindPos) {
					*iFindPos = iMatchPos;
				}
				const UnicodeString sReplacement = sText.SubString(iMatchPos, iMatchLength);
				return StringReplace(sText, sReplacement, Htmlutils::StrToBoldStr(sReplacement), TReplaceFlags());
			}
		}

		return sText;
	}

	void ChangeEmptyLinesToLineBreaks(TStrings *AStrings) {
		for (int i = 0; i < AStrings->Count; i++) {
			if (AStrings->Strings[i].IsEmpty()) {
				// LMD FIX: Needs 1 space
             	AStrings->Strings[i] = L" <BR>";
			}
		}
	}
};
