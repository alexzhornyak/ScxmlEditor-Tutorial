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

#include "UnitWatchShape.h"

#include <boost/regex.hpp>

#include <PngImage.hpp>
#include "Log4cpp_VCL.hpp"
#include "UnitDialogRegExp.h"
#include "UnitStringEditorEx.h"
#include "VectorDrawUtils.h"
#include "UnitSVGExporter.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
//-------------------------- TWatchLabel ------------------------------------
//---------------------------------------------------------------------------
__fastcall TWatchLabel::TWatchLabel(TComponent *AOwner, const UnicodeString &sLabel, const UnicodeString &sRegExp) : TLabel(AOwner) {
	FWatchLabel = sLabel;
	FRegExp = sRegExp;
}

//---------------------------------------------------------------------------
void __fastcall TWatchLabel::Parse(const UnicodeString &sText) {
	try {
		if (!FRegExp.IsEmpty()) {
			boost::wregex e(FRegExp.c_str());
			boost::wsmatch what;

			std::wstring wstr = sText.c_str();
			if (boost::regex_match(wstr, what, e)) {
				std::auto_ptr<TStringList>AStringListPtr(new TStringList());
				for (std::size_t i = 1; i < what.size(); i++) {
					AStringListPtr->Add(std::wstring(what[i]).c_str());
				}
				this->Caption = FWatchLabel + L":" + AStringListPtr->Text.Trim();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	catch(std::exception & e) {
		LOG_ERROR("%s> %s", __FUNCTION__, e.what());
	}
}

// ---------------------------------------------------------------------------
// -------------------------- TWatchShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TWatchShape::TWatchShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {

	FRegExp = L"";

	this->FStateAttrMap["RegExp"] = false;
	FLabel = L"";

	this->FStateAttrMap["Label"] = false;
	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngWatch");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TWatchShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"Label") {
			AEditorClass = __classid(TLineEditStringPropEditor);
			return;
		}

		if (sPropName == L"RegExp") {
			AEditorClass = __classid(TRegExpStringDialogPropEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TWatchShape::GetOutValue() {
	return(!FLabel.IsEmpty() ? (FLabel + L":") : UnicodeString(L"")) + RegExp;
}

// ---------------------------------------------------------------------------
void __fastcall TWatchShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TWatchShape * AWatchShape = dynamic_cast<TWatchShape*>(Source);
			if (AWatchShape) {
				FRegExp = AWatchShape->FRegExp;
				FLabel = AWatchShape->FLabel;

				UpdateSimpleText();
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
bool __fastcall TWatchShape::EditRegExp(void) {
	std::auto_ptr<TDialogRegExp>ADialogPtr(new TDialogRegExp(this->Owner));
	ADialogPtr->EditRegExp->Text = this->RegExp;
	if (ADialogPtr->ShowModal() == mrOk) {
		this->RegExp = ADialogPtr->EditRegExp->Text.Trim();
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TWatchShape::GetNormalFontStyle() {
	return !FRegExp.IsEmpty() ? (TFontStyles() << fsBold) : TChildScxmlBaseShape::GetNormalFontStyle();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TWatchShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Label") {
		return //
		"Will be added to captured caption of <b>WatchLabel</b>\n" //
		;
	}

	if (sPropName == L"RegExp") {
		return //
		"<b>SLOW PERFORMANCE WARNING!</b>\n" //
		"If <b>'RegExp'</b> is not empty then <b>every log line</b> in CMD output will be checked for matching.\n" //
		"If it matchs then capture groups wiil be displayed in <b>WatchLabels</b> on <b>'Triggers panel'</b> in runtime\n";
	}

	return L"";
}

// ---------------------------------------------------------------------------
bool __fastcall TWatchShape::HasVectorImage(void) {

	// родная канва криво работает при смене четных и нечетных размеров,
	// поэтому только для SVG
	TTreeSVGCanvasEx * ATreeSVGCanvasEx = dynamic_cast<TTreeSVGCanvasEx*>(Tree->Canvas);
	if (ATreeSVGCanvasEx) {
		return true;
	}

	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TWatchShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawWatch(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
