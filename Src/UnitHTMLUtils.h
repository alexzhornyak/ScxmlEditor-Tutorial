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

#ifndef UnitHTMLUtilsH
#define UnitHTMLUtilsH
//---------------------------------------------------------------------------

namespace Htmlutils {

	const UnicodeString BoldStrToStr(const UnicodeString & sBoldStr);

	const UnicodeString StrToBoldStr(const UnicodeString & sStr);

	const UnicodeString CreateHTMLErrorStr(const UnicodeString & sString);

	const UnicodeString StrToHTMLColorStr(const UnicodeString & sString, const TColor AColor,
		const bool bBold = false);

	const UnicodeString StrToHTMLColorStr2(const UnicodeString & sString, const TColor AColor = Graphics::clNone,
		const UnicodeString sFontName = L"",
		const bool bBold = false, const int iSize = -1);

	const UnicodeString HTMLColorStrToStr(const UnicodeString &sHtml);

	// "<" convert to "&lt;" etc.
	const UnicodeString StrXHtmlEncodeEx(const UnicodeString & sSource);

	// "&lt;" convert to "<" etc.
	const UnicodeString StrXHtmlDecodeEx(const UnicodeString & sSource);

	const UnicodeString HTMLStrWithImageIndex(const int iImageIndex);

	const UnicodeString StrWithBoldSearchText(const UnicodeString &sText, const UnicodeString &sSearch, int * iFindPos = NULL);

	void ChangeEmptyLinesToLineBreaks(TStrings *AStrings);
};

#endif
