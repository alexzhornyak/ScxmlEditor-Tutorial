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

#include "UnitChildScxmlBaseShapes.h"

#include <memory>

#include <LMDXML.hpp>
#include <IOUtils.hpp>
#include <PngImage.hpp>

#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"
#include "UnitSyntaxTextEditor.h"
#include "UnitStateMachineUnit.h"
#include "VISUAL_GLOBALS.h"
#include "UnitCustomUtils.h"
#include "UnitTreeEditorUtils.h"
#include "VectorDrawUtils.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterBaseChildShapes() {
#pragma startup RegisterBaseChildShapes

	Classes::RegisterClassA(__classid(TScriptShape));
	Classes::RegisterClassA(__classid(TOnExitStateShape));
	Classes::RegisterClassA(__classid(TOnEntryStateShape));
	Classes::RegisterClassA(__classid(TCommentShape));
	Classes::RegisterClassA(__classid(TExtraContentShape));
}

// ---------------------------------------------------------------------------
// -------------------------- TOnEntryStateShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TOnEntryStateShape::TOnEntryStateShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes.insert(STATE_TYPES_EXECUTABLE_CONTENT.begin(), STATE_TYPES_EXECUTABLE_CONTENT.end());

	FConvertibleTypes.insert(sctOnexit);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngOnEntry");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TOnEntryStateShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			/* при добавлении свойств раскомментарить
			TOnEntryStateShape * ABaseShape = dynamic_cast<TOnEntryStateShape*>(Source);
			if (ABaseShape) { } */
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TOnEntryStateShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::ResetPenBrush(Tree->Canvas);

	Tree->Canvas->Brush->Color = TColor(RGB(0, 170, 0));
	Tree->Canvas->EllipseWithZ(AImageBounds, TeeTreeZ);
}

// ---------------------------------------------------------------------------
// -------------------------- TOnExitStateShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TOnExitStateShape::TOnExitStateShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes.insert(STATE_TYPES_EXECUTABLE_CONTENT.begin(), STATE_TYPES_EXECUTABLE_CONTENT.end());

	FConvertibleTypes.insert(sctOnentry);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngOnExit");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TOnExitStateShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			/* при добавлении свойств раскомментарить
			TOnExitStateShape * ABaseShape = dynamic_cast<TOnExitStateShape*>(Source);
			if (ABaseShape) { }
			 */
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TOnExitStateShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::ResetPenBrush(Tree->Canvas);

	Tree->Canvas->Brush->Color = TColor(RGB(225, 0, 0));
	Tree->Canvas->EllipseWithZ(AImageBounds, TeeTreeZ);
}

// ---------------------------------------------------------------------------
// -------------------------- TScriptShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TScriptShape::TScriptShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FSrc = L"";
	FDisableFlatBuild = false;

	this->FStateAttrMap["Src"] = false;
	this->FStateAttrMap["DisableFlatBuild"] = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngScript");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScriptShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TScriptShape * AScriptShape = dynamic_cast<TScriptShape*>(Source);
			if (AScriptShape) {
				FSrc = AScriptShape->FSrc;
				FDisableFlatBuild = AScriptShape->FDisableFlatBuild;

				UpdateSimpleText();
			}
			else {
				TExtraContentShape * AExtraContentShape = dynamic_cast<TExtraContentShape*>(Source);
				if (AExtraContentShape) {
					this->XMLText->Text = AExtraContentShape->ExtraContent->Text;
					this->ExtraContent->Clear();
				}
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScriptShape::GetOutValue() {
	if (!Src.IsEmpty()) {
		return Src;
	}
	if (!XMLText->Text.IsEmpty()) {
		return XMLText->Text.Trim();
	}
	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TScriptShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {

	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);

	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"Src") {
			AEditorClass = __classid(TSyntaxSrcDialogEditorImpl);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScriptShape::OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor) {
	if (AItem->PropName == L"DisableFlatBuild") {
		AColor = clTeal;
	}
	else {
		TChildScxmlBaseShape::OnGetPropCaptionColor(Sender, AItem, AColor);
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScriptShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Src") {
		return //
		"A valid URI\n" //
		"Gives the location from which the script should be downloaded.";
	}

	if (sPropName == L"DisableFlatBuild") {
		return //
		"If <b>'DisableFlatBuild==false'</b> then <b>'src'</b> is loaded into scxml during flat build\n" //
		"otherwise it remains as is";
	}

	return L"";
}

// ---------------------------------------------------------------------------
TColor __fastcall TScriptShape::GetNormalColor(void) {
	return DisableFlatBuild ? COLOR_DISABLE_FLAT_BUILD : TChildScxmlBaseShape::GetNormalColor();
}

// ---------------------------------------------------------------------------
void TScriptShape::EditSrc(void) {
	const UnicodeString sPropName = L"Src";
	if (EditSyntaxSrcProperty(this, sPropName)) {
		if (StateMachineEditor) {
			StateMachineEditor->MarkModified(1, UnicodeString().sprintf(L"Edit '%s':[%s]", sPropName.c_str(),
					Customutils::ClippedText(this->SimpleText, 40).c_str()), false);
		}
	}
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TScriptShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Src", boost::bind(&TScriptShape::EditSrc, this)));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TScriptShape::ValidateXMLText(void) {
	if (!XMLText->Text.IsEmpty()) {
		if (!Src.IsEmpty())
			throw EScxmlBaseShapeException(this, "'Src' and 'XMLText'", "'Src' must not occur with 'XMLText'!");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScriptShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawScript2(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TCommentShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TCommentShape::TCommentShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes = TStateChildTypes();

	Comments->OnTextChanged = OnCommentsChanged;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngComment");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
TColor __fastcall TCommentShape::GetNormalFontColor(void) {
	return clGreen;
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TCommentShape::GetNormalFontStyle() {
	return TFontStyles() << fsBold;
}

// ---------------------------------------------------------------------------
void __fastcall TCommentShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TExtraContentShape * AExtraContentShape = dynamic_cast<TExtraContentShape*>(Source);
			if (AExtraContentShape) {
				this->Comments->Text = AExtraContentShape->ExtraContent->Text;
				this->ExtraContent->Clear();
			}

			UpdateSimpleText();
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

void __fastcall TCommentShape::OnCommentsChanged(TObject *Sender) {
	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TCommentShape::UpdateSimpleText() {
	if (!ComponentState.Contains(csLoading) && !ComponentState.Contains(csReading)) {

		UnicodeString sOut = this->Comments->Text;
		if (ClipOutValue && sOut.Length() > ClipOutMax) {
			sOut = sOut.SetLength(ClipOutMax).Trim() + " ...";
		}
		// здесь используем полный текст, так как возможно хотим видеть многострочный комментарий!!!
		this->Text->Text = sOut;

		UpdateParentSimpleText();

		if (StateMachineEditor && StateMachineEditor->NodeTheTree->Checked) {
			StateMachineEditor->MarkUpdateNodeTreeElement(this);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TCommentShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawComments(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TExtraContentShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TExtraContentShape::TExtraContentShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes = TStateChildTypes();

	FConvertibleTypes.insert(sctScript);
	FConvertibleTypes.insert(sctComment);

	ExtraContent->OnTextChanged = OnExtraContextChanged;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngExtraContent");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TExtraContentShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			// TExtraContentShape * AScriptShape = dynamic_cast<TExtraContentShape*>(Source);
			// if (AScriptShape) {
			// FSrc = AScriptShape->FSrc;
			// }
			UpdateSimpleText();
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

void __fastcall TExtraContentShape::OnExtraContextChanged(TObject *Sender) {
	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TExtraContentShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawExtraContent(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TTransitionXMLShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TTransitionXMLShape::TTransitionXMLShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes.insert(STATE_TYPES_EXECUTABLE_CONTENT.begin(), STATE_TYPES_EXECUTABLE_CONTENT.end());

	FConvertibleTypes.insert(sctOnexit);

	FErrorState = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngTransitionXML");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTransitionXMLShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			// TTransitionXMLShape * ABaseShape = dynamic_cast<TTransitionXMLShape*>(Source);
			// if (ABaseShape) {
			//
			// }
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
bool __fastcall TTransitionXMLShape::OnFilterPropEvent(const UnicodeString & sPropName) {
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TTransitionXMLShape::UpdateSimpleText() {
	if (!ComponentState.Contains(csLoading) && !ComponentState.Contains(csReading)) {
		SimpleText = L"XML";
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTransitionXMLShape::SetErrorState(bool bError) {
	if (FErrorState != bError) {
		FErrorState = bError;

		try {
			std::auto_ptr<TPngImage>APNGImage(new TPngImage());

			APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), FErrorState ? L"PngError" : L"PngTransitionXML");

			this->Image->Assign(APNGImage.get());
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTransitionXMLShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawTransitionXML(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
