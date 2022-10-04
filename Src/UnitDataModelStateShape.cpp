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

#include "UnitDataModelStateShape.h"

#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <PngImage.hpp>
#include <IOUtils.hpp>

#include "UnitSyntaxEditorForm.h"
#include "UnitExternalCommunicationShapes.h"
#include "UnitSettings.h"
#include "UnitSyntaxTextEditor.h"
#include "UnitStateMachineUnit.h"
#include "VISUAL_GLOBALS.h"
#include "UnitCustomUtils.h"
#include "UnitStringEditorEx.h"
#include "UnitTreeEditorUtils.h"
#include "VectorDrawUtils.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterDatamodelShapes() {
#pragma startup RegisterDatamodelShapes

	Classes::RegisterClassA(__classid(TDatamodelStateShape));
	Classes::RegisterClassA(__classid(TDataChildShape));
	Classes::RegisterClassA(__classid(TDonedataShape));
	Classes::RegisterClassA(__classid(TContentShape));
	Classes::RegisterClassA(__classid(TParamShape));
}

// ---------------------------------------------------------------------------
// -------------------------- TDatamodelStateShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TDatamodelStateShape::TDatamodelStateShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {

	FAvailableTypes.insert(sctData);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngDatamodel");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TDatamodelStateShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			// TDatamodelStateShape * ABaseShape = dynamic_cast<TDatamodelStateShape*>(Source);
			// if (ABaseShape) {
			// FDataModel = ABaseShape->FDataModel;
			// }
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TDatamodelStateShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawDatamodel(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TDataChildShape -----------------------------
// ---------------------------------------------------------------------------
__fastcall TDataChildShape::TDataChildShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FDataId(L""), FSrc(L""),
FExpr(L"") {

	FDisableFlatBuild = false;

	this->FStateAttrMap["Id"] = true;
	this->FStateAttrMap["Src"] = false;
	this->FStateAttrMap["Expr"] = false;
	this->FStateAttrMap["DisableFlatBuild"] = false;

	FConvertibleTypes.insert(sctParam);
	FConvertibleTypes.insert(sctAssign);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngData");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TDataChildShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"Id") {
			AEditorClass = __classid(TLineEditStringPropEditor);
			return;
		}
		if (sPropName == L"Expr") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
		if (sPropName == L"Src") {
			AEditorClass = __classid(TSyntaxSrcDialogEditorImpl);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
void __fastcall TDataChildShape::OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor) {
	if (AItem->PropName == L"DisableFlatBuild") {
		AColor = clTeal;
	}
	else {
		TChildScxmlBaseShape::OnGetPropCaptionColor(Sender, AItem, AColor);
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TDataChildShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Id") {
		return //
		"The name of the data item.\n" //
		"This name <B>CAN NOT BE EMPTY!!!</B>";
	}

	if (sPropName == L"Src") {
		return //
		"A valid URI\n" //
		"Gives the location from which the data object should be fetched.\n" //
		"'src', 'expr' and children are <B>mutually exclusive</B> in the &lt;data&gt; element";
	}

	if (sPropName == L"Expr") {
		return //
		"Any valid value expression\n" //
		"Evaluates to provide the value of the data item.\n" //
		"'src', 'expr' and children are <B>mutually exclusive</B> in the &lt;data&gt; element";
	}

	if (sPropName == L"DisableFlatBuild") {
		return //
		"If <b>'DisableFlatBuild==false'</b> then <b>'src'</b> is loaded into scxml during flat build\n" //
		"otherwise it remains as is";
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TDataChildShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса   ! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			if (TDataChildShape * ADataChildShape = dynamic_cast<TDataChildShape*>(Source)) {
				FDataId = ADataChildShape->FDataId;
				FSrc = ADataChildShape->FSrc;
				FExpr = ADataChildShape->FExpr;
				FDisableFlatBuild = ADataChildShape->FDisableFlatBuild;

				UpdateSimpleText();
			}
			else if (TParamShape * AParamShape = dynamic_cast<TParamShape*>(Source)) {
				FDataId = AParamShape->ParamName;
				FSrc = L"";
				XMLText->Clear();
				if (!AParamShape->Location.IsEmpty()) {
					FExpr = AParamShape->Location;
				}
				else if (!AParamShape->Expr.IsEmpty()) {
					FExpr = AParamShape->Expr;
				}
				else {
					FExpr = L"";
				}

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
UnicodeString __fastcall TDataChildShape::GetOutValue() {
	UnicodeString sOut = "";
	if (!FExpr.IsEmpty()) {
		sOut = FExpr;
	}
	else if (!FSrc.IsEmpty()) {
		sOut = FSrc;
	}
	else if (!XMLText->Text.IsEmpty()) {
		sOut = XMLText->Text.Trim();
	}

	return UnicodeString().sprintf(L"%s=%s", FDataId.c_str(), sOut.c_str());
}

// ---------------------------------------------------------------------------
void __fastcall TDataChildShape::SetXMLText(TSyntaxString *val) {
	TChildScxmlBaseShape::SetXMLText(val);
	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
TColor __fastcall TDataChildShape::GetNormalColor(void) {
	return DisableFlatBuild ? COLOR_DISABLE_FLAT_BUILD : TChildScxmlBaseShape::GetNormalColor();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TDataChildShape::GetClipboardInfo(void) {
	if (!Id.IsEmpty()) {
		return Id;
	}
	if (!Src.IsEmpty()) {
		return Src;
	}
	if (!Expr.IsEmpty()) {
		return Expr;
	}
	return TChildScxmlBaseShape::GetClipboardInfo();
}

// ---------------------------------------------------------------------------
void TDataChildShape::EditSrc(void) {
	const UnicodeString sPropName = L"Src";
	if (EditSyntaxSrcProperty(this, sPropName)) {
		if (StateMachineEditor) {
			StateMachineEditor->MarkModified(1, UnicodeString().sprintf(L"Edit '%s':[%s]", sPropName.c_str(),
					Customutils::ClippedText(this->SimpleText, 40).c_str()), false);
		}
	}
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TDataChildShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Src", boost::bind(&TDataChildShape::EditSrc, this)));
	AVecEditors.push_back(std::make_pair(L"Expr", boost::bind(&TDataChildShape::EditUnicodeStringSyntaxProperty, this, L"Expr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TDataChildShape::ValidateXMLText(void) {
	if (!XMLText->Text.IsEmpty()) {
		if (!Src.IsEmpty())
			throw EScxmlBaseShapeException(this, L"'Src' and 'XMLText'", "'Src' must not occur with 'XMLText'!");

		if (!Expr.IsEmpty())
			throw EScxmlBaseShapeException(this, L"'Expr' and 'XMLText'", "'Expr' must not occur with 'XMLText'!");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TDataChildShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawData(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TDonedataShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TDonedataShape::TDonedataShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {

	FAvailableTypes.insert(sctContent);
	FAvailableTypes.insert(sctParam);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngDoneData");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TDonedataShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса   ! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			// TDonedataShape * ADonedataShape = dynamic_cast<TDonedataShape*>(Source);
			// if (ADonedataShape) {
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
void __fastcall TDonedataShape::ValidateChild(TScxmlBaseShape *AChild) {
	/* inherited */
	TChildScxmlBaseShape::ValidateChild(AChild);

	if (AChild) {
		switch(AChild->StateChildType) {
		case sctContent: {
				if (TParamShape * AParamShape = Editorutils::FindShapeByType<TParamShape>(this))
					throw EScxmlBaseShapeException(AParamShape, L"Conflicts with <content>", L"<content> must not occur with <param>!");
			}break;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TDonedataShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawDoneData(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TContentShape -----------------------------
// ---------------------------------------------------------------------------
__fastcall TContentShape::TContentShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FExpr(L"") {
	this->FStateAttrMap["Expr"] = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngContent");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TContentShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		if (APropInfo->Name() == L"Expr") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TContentShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Expr") {
		return //
		"Any valid value expression\n" //
		"<B>Must not occur with child content</B>\n";
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TContentShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса   ! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TContentShape * AContentShape = dynamic_cast<TContentShape*>(Source);
			if (AContentShape) {
				FExpr = AContentShape->FExpr;

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
UnicodeString __fastcall TContentShape::GetOutValue() {
	UnicodeString sOut = "";
	if (!FExpr.IsEmpty()) {
		sOut = FExpr;
	}
	else if (!XMLText->Text.IsEmpty()) {
		sOut = XMLText->Text;
	}

	return sOut;
}

// ---------------------------------------------------------------------------
void __fastcall TContentShape::SetXMLText(TSyntaxString *val) {
	TChildScxmlBaseShape::SetXMLText(val);
	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TContentShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Expr", boost::bind(&TDataChildShape::EditUnicodeStringSyntaxProperty, this, L"Expr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TContentShape::ValidateXMLText(void) {
	if (!XMLText->Text.IsEmpty()) {
		if (!Expr.IsEmpty())
			throw EScxmlBaseShapeException(this, "'Expr' and 'XMLText'", "'Expr' must not occur with 'XMLText'!");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TContentShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawContent(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TParamShape -----------------------------
// ---------------------------------------------------------------------------
__fastcall TParamShape::TParamShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FParamName(L""), FLocation(L""),
FExpr(L"") {
	this->FStateAttrMap["ParamName"] = true;
	this->FStateAttrMap["Expr"] = false;
	this->FStateAttrMap["Location"] = false;

	FConvertibleTypes.insert(sctData);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngParam");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TParamShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"ParamName") {
			AEditorClass = __classid(TLineEditStringPropEditor);
			return;
		}

		if (sPropName == L"Expr" || sPropName == L"Location") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
	}

	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TParamShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"ParamName") {
		return //
		"A string literal\n" //
		"The name of the key.\n" //
		"This name <B>CAN NOT BE EMPTY!</B>";
	}

	if (sPropName == L"Expr") {
		return //
		"Valid value expression\n" //
		"that is evaluated to provide the value.\n" //
		"<B>May not occur with 'location'</B>";
	}

	if (sPropName == L"Location") {
		return //
		"A location expression\n" //
		"that specifies the location in the datamodel to retrieve the value from.\n" //
		"<B>May not occur with 'expr'</B>";
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TParamShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса   ! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			if (TParamShape * AParamShape = dynamic_cast<TParamShape*>(Source)) {
				FParamName = AParamShape->ParamName;
				FLocation = AParamShape->Location;
				FExpr = AParamShape->Expr;

				UpdateSimpleText();
			}
			else if (TDataChildShape * ADataChildShape = dynamic_cast<TDataChildShape*>(Source)) {

				if (!ADataChildShape->Src.IsEmpty())
					throw Exception("Can not convert data with non-empty 'Src':[" + ADataChildShape->Src + "]!");

				FParamName = ADataChildShape->Id;
				FLocation = L"";
				if (!ADataChildShape->Expr.IsEmpty()) {
					FExpr = ADataChildShape->Expr;
				}
				else if (!ADataChildShape->XMLText->Text.IsEmpty()) {
					FExpr = ADataChildShape->XMLText->Text;
				}
				else {
					FExpr = L"";
					XMLText->Clear();
				}

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
UnicodeString __fastcall TParamShape::XMLAttrNameToPropName(UnicodeString sAttrName) {
	sAttrName = TChildScxmlBaseShape::XMLAttrNameToPropName(sAttrName);
	return sAttrName == L"Name" ? L"ParamName" : sAttrName.c_str();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TParamShape::PropNameToXMLAttrName(UnicodeString sPropName) {
	sPropName = TChildScxmlBaseShape::PropNameToXMLAttrName(sPropName);
	return sPropName == L"paramname" ? L"name" : sPropName.c_str();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TParamShape::GetOutValue() {
	std::vector<std::wstring>VecLeftPart;
	if (!ParamName.IsEmpty()) {
		VecLeftPart.push_back(ParamName.c_str());
	}
	if (!Location.IsEmpty()) {
		VecLeftPart.push_back(Location.c_str());
	}

	std::vector<std::wstring>VecOut;
	if (!VecLeftPart.empty()) {
		VecOut.push_back(boost::algorithm::join(VecLeftPart, L";"));
	}
	if (!Expr.IsEmpty()) {
		VecOut.push_back(Expr.c_str());
	}

	return boost::algorithm::join(VecOut, L"=").c_str();
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TParamShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Location", boost::bind(&TParamShape::EditUnicodeStringSyntaxProperty, this, L"Location")));
	AVecEditors.push_back(std::make_pair(L"Expr", boost::bind(&TParamShape::EditUnicodeStringSyntaxProperty, this, L"Expr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TParamShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawParam(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
