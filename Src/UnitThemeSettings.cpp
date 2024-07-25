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

#include "UnitThemeSettings.h"

#include <IOUtils.hpp>
#include "Log4cpp_VCL.hpp"

#include "UnitMain.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitSettings.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterThemeSettings() {
#pragma startup RegisterThemeSettings

	Classes::RegisterClassA(__classid(TThemeSettings));
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TThemeSettings::TThemeSettings() {
	FChildNormalColor = clWhite;
	FChildNormalFontStyle = TFontStyles();
	FChildNormalBorderColor = g_CHILD_NORMAL_BORDER_COLOR;
	FChildNormalBorderStyle = psSolid;

	FStateNormalColor = g_STATE_NORMAL_COLOR;
	FStateClusterColor = clWhite;
	FStateInitialBorderColor = g_STATE_INITIAL_BORDER_COLOR;
	FStateExaminedBorderColor = clBlue;
	FStateNormalBorderColor = clBlack;
	FStateNormalBorderStyle = psSolid;
	FStateNormalFontStyle = TFontStyles();
	FStateSelfConnectionInsideFontStyle = g_STATE_SELF_CONNECTION_INSIDE_FONT_STYLE;

	FStateDefaultWidth = 100;
	FStateDefaultHeight = 50;

	FScxmlNormalHeadColor = clMoneyGreen;
	FScxmlDatamodelFontColor = clMaroon;
	FScxmlVirtualFontColor = clBlue;

	FParallelNormalColor = g_PARALLEL_NORMAL_COLOR;

	FVirtualNormalColor = clSilver;
	FVirtualHeadColor = clGray;

	FTransitionLineWidth = 1;
	FTransitionFromCircleSize = 10;
}

// ---------------------------------------------------------------------------
__fastcall TThemeSettings::~TThemeSettings(void) {

}

// ---------------------------------------------------------------------------
void __fastcall TThemeSettings::Assign(Classes::TPersistent* Source) {
	if (Source) {
		// ********************************************* //

		TThemeSettings * AThemeSettings = dynamic_cast<TThemeSettings*>(Source);
		if (AThemeSettings) {
			FChildNormalColor = AThemeSettings->FChildNormalColor;
			FChildNormalFontStyle = AThemeSettings->FChildNormalFontStyle;
			FChildNormalBorderColor = AThemeSettings->FChildNormalBorderColor;
			FChildNormalBorderStyle = AThemeSettings->FChildNormalBorderStyle;

			FStateNormalColor = AThemeSettings->FStateNormalColor;
			FStateClusterColor = AThemeSettings->FStateClusterColor;
			FStateInitialBorderColor = AThemeSettings->FStateInitialBorderColor;
			FStateExaminedBorderColor = AThemeSettings->FStateExaminedBorderColor;
			FStateNormalBorderColor = AThemeSettings->FStateNormalBorderColor;
			FStateNormalBorderStyle = AThemeSettings->FStateNormalBorderStyle;
			FStateNormalFontStyle = AThemeSettings->FStateNormalFontStyle;

			FStateDefaultWidth = AThemeSettings->FStateDefaultWidth;
			FStateDefaultHeight = AThemeSettings->FStateDefaultHeight;

			FStateSelfConnectionInsideFontStyle = AThemeSettings->FStateSelfConnectionInsideFontStyle;

			FScxmlNormalHeadColor = AThemeSettings->FScxmlNormalHeadColor;
			FScxmlDatamodelFontColor = AThemeSettings->FScxmlDatamodelFontColor;
			FScxmlVirtualFontColor = AThemeSettings->FScxmlVirtualFontColor;

			FParallelNormalColor = AThemeSettings->FParallelNormalColor;

			FVirtualNormalColor = AThemeSettings->FVirtualNormalColor;
			FVirtualHeadColor = AThemeSettings->FVirtualHeadColor;

			FTransitionLineWidth = AThemeSettings->FTransitionLineWidth;
			FTransitionFromCircleSize = AThemeSettings->FTransitionFromCircleSize;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
bool __fastcall TThemeSettings::OnFilterPropEvent(const UnicodeString &sPropName) {
	return true;
}

// ---------------------------------------------------------------------------
void __fastcall TThemeSettings::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
//	if (APropInfo) {
//		const UnicodeString sPropName = APropInfo->Name();
//		if (sPropName == L"ExeName") {
//			AEditorClass = __classid(TExternTesterExePropEditor);
//		}
//	}
}

//---------------------------------------------------------------------------
//---------------------- TThemeSettingsPropEditor ----------------------------
//---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TThemeSettingsPropEditor::GetValue(void) {
	UnicodeString sText = L"Theme Settings";

	try {
		TThemeSettings *ASource = dynamic_cast<TThemeSettings*>(this->GetObjectValue(0));
		if (!ASource)
			throw Exception("Can not cast to <TThemeSettings>!");

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	return sText;
}

