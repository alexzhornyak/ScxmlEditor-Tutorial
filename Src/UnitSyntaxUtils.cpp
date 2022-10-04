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

#include "UnitSyntaxUtils.h"
#include <Character.hpp>
// ---------------------------------------------------------------------------

#include <memory>

#pragma package(smart_init)

namespace Syntaxutils {

	// ---------------------------------------------------------------------------
	UnicodeString __fastcall ExtensionToSyntaxScheme(UnicodeString sExt) {
		std::auto_ptr<TStringList>AExtensionPtr(new TStringList);
		AExtensionPtr->Values["lua"] = "Lua";
		AExtensionPtr->Values["txt"] = "Text";
		AExtensionPtr->Values["xml"] = "XML";
		AExtensionPtr->Values["c"] = "C";
		AExtensionPtr->Values["cpp"] = "CPP";
		AExtensionPtr->Values["js"] = "EcmaScript";
		AExtensionPtr->Values["ini"] = "INI";
		AExtensionPtr->Values["dfm"] = "DFM";
		AExtensionPtr->Values["cmd"] = "Batch";

		if (sExt.Pos(".") == 1) {
			sExt = sExt.Delete(1, 1);
		}

		return AExtensionPtr->Values[sExt.LowerCase()];
	}

	// ---------------------------------------------------------------------------
	UnicodeString __fastcall SyntaxSchemeToExtension(const UnicodeString &sSyntaxScheme) {
		std::auto_ptr<TStringList>AExtensionPtr(new TStringList);
		AExtensionPtr->Values["Lua"] = "lua";
		AExtensionPtr->Values["Text"] = "txt";
		AExtensionPtr->Values["XML"] = "xml";
		AExtensionPtr->Values["C"] = "c";
		AExtensionPtr->Values["CPP"] = "cpp";
		AExtensionPtr->Values["EcmaScript"] = "js";
		AExtensionPtr->Values["INI"] = "ini";
		AExtensionPtr->Values["DFM"] = "dfm";
		AExtensionPtr->Values["Batch"] = "cmd";
		AExtensionPtr->Values["JavaScript"] = "js";

		return AExtensionPtr->Values[sSyntaxScheme];
	}

	//---------------------------------------------------------------------------
	void __fastcall GetSyntaxComments(const UnicodeString &sSyntaxScheme, UnicodeString & /*out*/ sStart, UnicodeString & /*out*/ sEnd) {
		sStart = L"";
		sEnd = L"";

		std::auto_ptr<TStringList>AStartCommentListPtr(new TStringList());
		AStartCommentListPtr->Values[L"XML"] = L"<!--";
		AStartCommentListPtr->Values[L"Lua"] = L"--";
		AStartCommentListPtr->Values[L"JavaScript"] = L"//";
		AStartCommentListPtr->Values[L"EcmaScript"] = L"//";
		AStartCommentListPtr->Values[L"C"] = L"//";
		AStartCommentListPtr->Values[L"CPP"] = L"//";
		AStartCommentListPtr->Values[L"Batch"] = L"REM";
		AStartCommentListPtr->Values[L"INI"] = L";";

		std::auto_ptr<TStringList>AEndCommentListPtr(new TStringList());
		AEndCommentListPtr->Values[L"XML"] = L"-->";

		sStart = AStartCommentListPtr->Values[sSyntaxScheme];
		sEnd = AEndCommentListPtr->Values[sSyntaxScheme];
	}

	//---------------------------------------------------------------------------
	UnicodeString __fastcall GetDialogFilterByExtension(const UnicodeString &sFileExt) {
		std::auto_ptr<TStringList>AFiltersListPtr(new TStringList());
		AFiltersListPtr->StrictDelimiter = true;
		AFiltersListPtr->Delimiter = L'|';

		if (!sFileExt.IsEmpty()) {
			/* CamelCase */
			//			UnicodeString sExtFiles = sFileExt.LowerCase();
			//			sExtFiles[1] = ToLower(sExtFiles[1]);
			AFiltersListPtr->Add("Files (*." + sFileExt + ")");
			AFiltersListPtr->Add("*." + sFileExt);
		}
		AFiltersListPtr->Add("All files (*.*)");
		AFiltersListPtr->Add("*.*");

		return AFiltersListPtr->DelimitedText;
	}

}
