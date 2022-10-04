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

#include "UnitSVGSettings.h"

#include <XMLDoc.hpp>
#include <Character.hpp>

#include "Log4cpp_VCL.hpp"

#include "UnitStringEditorEx.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterExportSvgSettings() {
#pragma startup RegisterExportSvgSettings

	Classes::RegisterClassA(__classid(TExportSvgSettings));
}

//---------------------------------------------------------------------------
__fastcall TExportSvgSettings::TExportSvgSettings() {
	FExportSvgShapeRendering = TExportSvgShapeRendering::ssrAuto;
	FExportSvgTextRendering = TExportSvgTextRendering::strAuto;
	FExportSvgTextFontUsePoints = false;
	FExportSvgExtraParams = L"";
}

// ---------------------------------------------------------------------------
void __fastcall TExportSvgSettings::Assign(Classes::TPersistent* Source) {
	if (Source) {
		// ********************************************* //

		TExportSvgSettings * AExportSvgSettings = dynamic_cast<TExportSvgSettings*>(Source);
		if (AExportSvgSettings) {
			FExportSvgShapeRendering = AExportSvgSettings->FExportSvgShapeRendering;
			FExportSvgTextRendering = AExportSvgSettings->FExportSvgTextRendering;
			FExportSvgTextFontUsePoints = AExportSvgSettings->FExportSvgTextFontUsePoints;
			FExportSvgExtraParams = AExportSvgSettings->FExportSvgExtraParams;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TExportSvgSettings::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();
		if (sPropName == L"ExportSvgExtraParams") {
			AEditorClass = __classid(TXMLStringPropEditor);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TExportSvgSettings::SetExportSvgExtraParams(UnicodeString val) {
	try {
		if (!val.IsEmpty() && val != FExportSvgExtraParams) {
			LoadXMLData("<svg " + val + "/>");
		}

		FExportSvgExtraParams = val;
	}
	catch(Exception * E) {
		WLOG_ERROR(L"Set SVG params> %s", E->Message.c_str());
	}

}

//---------------------------------------------------------------------------
UnicodeString __fastcall TExportSvgSettings::GetSvgParams(void) {
	UnicodeString sOut = L"";
	if (FExportSvgShapeRendering != ssrAuto) {
		UnicodeString sVal = StringReplace(GetEnumProp(this, "ExportSvgShapeRendering"), "ssr", "", TReplaceFlags());
		sVal[1] = ToLower(sVal[1]);

		sOut = sOut + "shape-rendering=\"" + sVal + "\" ";
	}
	if (FExportSvgTextRendering != ssrAuto) {
		UnicodeString sVal = StringReplace(GetEnumProp(this, "ExportSvgTextRendering"), "str", "", TReplaceFlags());
		sVal[1] = ToLower(sVal[1]);

		sOut = sOut + "text-rendering=\"" + sVal + "\" ";
	}

	sOut = sOut + FExportSvgExtraParams;

	return sOut;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TExportSvgSettingsPropEditor::GetValue(void) {
	UnicodeString sText = L"";

	try {
		TExportSvgSettings *ASource = dynamic_cast<TExportSvgSettings*>(this->GetObjectValue(0));
		if (!ASource)
			throw Exception("Can not cast to <TExportSvgSettings>!");

		sText = ASource->GetSvgParams();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	return sText;
}
//---------------------------------------------------------------------------
