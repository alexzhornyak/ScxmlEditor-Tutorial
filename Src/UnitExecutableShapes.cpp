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

#include "UnitExecutableShapes.h"

#include <PNGImage.hpp>
#include "UnitStringEditorEx.h"
#include "UnitSettings.h"
#include "UnitSyntaxEditorForm.h"
#include "UnitExternalCommunicationShapes.h"
#include "VectorDrawUtils.h"
#include "UnitDataModelStateShape.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterExecutableShapes() {
#pragma startup RegisterExecutableShapes

	Classes::RegisterClassA(__classid(TRaiseShape));
	Classes::RegisterClassA(__classid(TIfShape));
	Classes::RegisterClassA(__classid(TElseIfShape));
	Classes::RegisterClassA(__classid(TElseShape));
	Classes::RegisterClassA(__classid(TForeachShape));
	Classes::RegisterClassA(__classid(TLogShape));
	Classes::RegisterClassA(__classid(TAssignShape));

}

// ---------------------------------------------------------------------------
// -------------------------- TRaiseShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TRaiseShape::TRaiseShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FEvent(L"") {
	this->FStateAttrMap["Event"] = true;

	FConvertibleTypes.insert(sctSend);

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngRaise");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TRaiseShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			if (TRaiseShape * ARaiseShape = dynamic_cast<TRaiseShape*>(Source)) {
				Event = ARaiseShape->Event;
			}
			/* ПРЕОБРАЗОВАНИЯ ИЗ ДРУГИХ ТИПОВ */
			else if (TSendShape * ASendShape = dynamic_cast<TSendShape*>(Source)) {
				Event = ASendShape->Event.IsEmpty() ? ASendShape->Eventexpr : ASendShape->Event;

				if (!ASendShape->Eventexpr.IsEmpty()) {
					WLOG_WARNING(
						L"CONVERSION> <raise> 'event' was directly set from <send> 'eventexpr'=[%s], please perform manual conversion!",
						ASendShape->Eventexpr.c_str());
				}

				while (ASendShape->Children->Count) {
					delete ASendShape->Children->Items[0];
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
UnicodeString __fastcall TRaiseShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Event") {
		return //
		"Specifies the name of the event.\n" //
		"This will be matched against the <B>'event'</B> attribute of transitions.";
	}

	return L"";
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TRaiseShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Event", boost::bind(&TRaiseShape::EditEventIDLineProperty, this, L"Event")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TRaiseShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"Event") {
			AEditorClass = __classid(TEventIDPropEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
void __fastcall TRaiseShape::DrawVectorImage(const TRect &AImageBounds) {
	Vectordrawutils::DrawRaise(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TIfShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TIfShape::TIfShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FCond(L"") {
	FAvailableTypes.insert(STATE_TYPES_EXECUTABLE_CONTENT.begin(), STATE_TYPES_EXECUTABLE_CONTENT.end());

	this->FStateAttrMap["Cond"] = true;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngIf");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TIfShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TIfShape * AIfShape = dynamic_cast<TIfShape*>(Source);
			if (AIfShape) {
				Cond = AIfShape->Cond;
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TIfShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		if (APropInfo->Name() == L"Cond") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TIfShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Cond", boost::bind(&TIfShape::EditUnicodeStringSyntaxProperty, this, L"Cond")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TIfShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Cond") {
		return //
		"A valid conditional <B>boolean expression</B>";
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TIfShape::DrawVectorImage(const TRect &AImageBounds) {
	Vectordrawutils::DrawIf(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TElseIfShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TElseIfShape::TElseIfShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FCond(L"") {
	FAvailableTypes.insert(STATE_TYPES_EXECUTABLE_CONTENT.begin(), STATE_TYPES_EXECUTABLE_CONTENT.end());

	this->FStateAttrMap["Cond"] = true;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngIf");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TElseIfShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TElseIfShape * AElseIfShape = dynamic_cast<TElseIfShape*>(Source);
			if (AElseIfShape) {
				Cond = AElseIfShape->Cond;
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TElseIfShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		if (APropInfo->Name() == L"Cond") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TElseIfShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Cond") {
		return //
		"A valid conditional <B>boolean expression</B>";
	}

	return L"";
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TElseIfShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Cond", boost::bind(&TElseIfShape::EditUnicodeStringSyntaxProperty, this, L"Cond")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TElseIfShape::DrawVectorImage(const TRect &AImageBounds) {
	Vectordrawutils::DrawIf(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TElseShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TElseShape::TElseShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner) {
	FAvailableTypes.insert(STATE_TYPES_EXECUTABLE_CONTENT.begin(), STATE_TYPES_EXECUTABLE_CONTENT.end());

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngIf");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TElseShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			// TElseShape * AElseShape = dynamic_cast<TElseShape*>(Source);
			// if (AElseShape) {
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
void __fastcall TElseShape::DrawVectorImage(const TRect &AImageBounds) {
	Vectordrawutils::DrawIf(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TForeachShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TForeachShape::TForeachShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FArray(L""), FForeachItem(L""),
FForeachIndex(L"") {
	this->FStateAttrMap["Array"] = true;
	this->FStateAttrMap["ForeachItem"] = true;
	this->FStateAttrMap["ForeachIndex"] = false;

	FAvailableTypes.insert(STATE_TYPES_EXECUTABLE_CONTENT.begin(), STATE_TYPES_EXECUTABLE_CONTENT.end());

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngForeach");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TForeachShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TForeachShape * AForeachShape = dynamic_cast<TForeachShape*>(Source);
			if (AForeachShape) {
				FArray = AForeachShape->Array;
				FForeachItem = AForeachShape->FForeachItem;
				FForeachIndex = AForeachShape->FForeachIndex;

				UpdateSimpleText();
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TForeachShape::XMLAttrNameToPropName(UnicodeString sAttrName) {
	sAttrName = TChildScxmlBaseShape::XMLAttrNameToPropName(sAttrName);
	if (sAttrName == L"Item") {
		return L"ForeachItem";
	}
	if (sAttrName == L"Index") {
		return L"ForeachIndex";
	}
	return sAttrName;
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TForeachShape::PropNameToXMLAttrName(UnicodeString sPropName) {
	sPropName = TChildScxmlBaseShape::PropNameToXMLAttrName(sPropName);
	if (sPropName == L"foreachitem") {
		return L"item";
	}
	if (sPropName == L"foreachindex") {
		return L"index";
	}
	return sPropName;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TForeachShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Array") {
		return //
		"A value expression that evaluates to an iterable collection.\n" //
		"The &lt;foreach&gt; element will iterate over a shallow copy of this collection.";
	}

	if (sPropName == L"ForeachItem") {
		return //
		"A variable that stores a different item of the collection in each iteration of the loop."; ;
	}

	if (sPropName == L"ForeachIndex") {
		return //
		"A variable that stores the current iteration index upon each iteration of the foreach loop.";
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TForeachShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		std::auto_ptr<TStringList>AUnicodeLineListPtr(new TStringList());
		AUnicodeLineListPtr->CaseSensitive = true;
		AUnicodeLineListPtr->CommaText = L"Array,ForeachItem,ForeachIndex";

		if (AUnicodeLineListPtr->IndexOf(sPropName) != -1) {
			AEditorClass = __classid(TLineEditStringPropEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
void __fastcall TForeachShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawForeach(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TLogShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TLogShape::TLogShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FLabel(L""), FExpression(L"") {
	this->FStateAttrMap["Label"] = false;
	this->FStateAttrMap["Expr"] = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngLog");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TLogShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TLogShape * ALogShape = dynamic_cast<TLogShape*>(Source);
			if (ALogShape) {
				FLabel = ALogShape->FLabel;
				FExpression = ALogShape->FExpression;

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
UnicodeString __fastcall TLogShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Label") {
		return //
		"A character string with an implementation-dependent interpretation.\n" //
		"It is intended to provide meta-data about the log string specified by 'expr'.";
	}

	if (sPropName == L"Expr") {
		return //
		"An expression returning the value to be logged.\n" //
		"The nature of the logging mechanism is implementation-dependent. " //
		"For example, the SCXML processor may convert this value to a convenient format before logging it.";
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TLogShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();
		if (sPropName == L"Expr") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
		if (sPropName == L"Label") {
			AEditorClass = __classid(TLineEditStringPropEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TLogShape::GetOutValue() {
	return UnicodeString().sprintf(L"%s:%s", FLabel.c_str(), FExpression.c_str());
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TLogShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Expr", boost::bind(&TLogShape::EditUnicodeStringSyntaxProperty, this, L"Expr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TLogShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawLog(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
// -------------------------- TAssignShape -----------------------------
// ---------------------------------------------------------------------------

__fastcall TAssignShape::TAssignShape(Classes::TComponent* AOwner) : TChildScxmlBaseShape(AOwner), FLocation(L""), FExpression(L"") {
	this->FStateAttrMap["Location"] = true;
	this->FStateAttrMap["Expr"] = false;

	try {
		std::auto_ptr<TPngImage>APNGImage(new TPngImage());
		APNGImage->LoadFromResourceName(reinterpret_cast<unsigned>(HInstance), L"PngAssign");

		this->Image->Assign(APNGImage.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TAssignShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TChildScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TAssignShape * AssignShape = dynamic_cast<TAssignShape*>(Source);
			if (AssignShape) {
				FLocation = AssignShape->Location;
				FExpression = AssignShape->Expr;

				UpdateSimpleText();
			}
			else if (TDataChildShape * ADataChildShape = dynamic_cast<TDataChildShape*>(Source)) {
				FLocation = ADataChildShape->Id;
				FExpression = ADataChildShape->Expr;

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
void __fastcall TAssignShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"Expr" || sPropName == L"Location") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
			return;
		}
	}

	/* inherited */
	TChildScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TAssignShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TChildScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Location") {
		return //
		"The location in the data model into which to insert the new value.";
	}

	if (sPropName == L"Expr") {
		return //
		"An expression returning the value to be assigned." //
		"This attribute <B>MUST NOT occur</B> in an &lt;assign&gt; element <B>that has children.</B>";
	}

	return L"";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TAssignShape::GetOutValue() {
	return UnicodeString().sprintf(L"%s=%s", FLocation.c_str(), FExpression.IsEmpty() ? XMLText->Text.c_str() : FExpression.c_str());
}

// ---------------------------------------------------------------------------
std::vector<TDefaultEditorPair>TAssignShape::GetEditByDefaultEditors(void) {
	std::vector<TDefaultEditorPair>AVecEditors;
	AVecEditors.push_back(std::make_pair(L"Expr", boost::bind(&TAssignShape::EditUnicodeStringSyntaxProperty, this, L"Expr")));
	return AVecEditors;
}

// ---------------------------------------------------------------------------
void __fastcall TAssignShape::ValidateXMLText(void) {
	if (!XMLText->Text.IsEmpty()) {
		if (!Expr.IsEmpty())
			throw EScxmlBaseShapeException(this, "'Expr' and 'XMLText'", "'Expr' must not occur with 'XMLText'!");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TAssignShape::DrawVectorImage(const TRect &AImageBounds) {

	Vectordrawutils::DrawAssign(Tree->Canvas, AImageBounds.left, AImageBounds.top);
}

// ---------------------------------------------------------------------------
