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

#include "UnitStateMachineUnit.h"

#include <memory>

#include <boost/tuple/tuple.hpp>

#include <IOUtils.hpp>
#include <imagehlp.h>

#include "Log4cpp_VCL.hpp"
#include "UnitTreeEditorStateMachine.h"
#include "UnitSettings.h"
#include "UnitMain.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

TStateMachineUnitType GlobalStateMachineUnitType = smutNormal;

/* LMD_PROJPROJECT.bmp */
static const unsigned char B_LMD_PROJPROJECT[] = {
	66, 77, 246, 0, 0, 0, 0, 0, 0, 0, 118, 0, 0, 0, 40, 0, 0, 0, 16, 0, 0, 0, 16, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 0, 0, 128, 0, 0, 0, 128, 128, 0, 128, 0, 0, 0, 128, 0, 128, 0, 128,
	128, 0, 0, 128, 128, 128, 0, 192, 192, 192, 0, 0, 0, 255, 0, 0, 255, 0, 0, 0, 255, 255, 0, 255, 0, 0, 0, 255, 0, 255, 0, 255, 255, 0,
	0, 255, 255, 255, 0, 221, 0, 0, 0, 0, 221, 221, 221, 221, 120, 136, 136, 128, 221, 221, 221, 0, 127, 68, 68, 128, 221, 221, 221, 123,
	127, 255, 255, 128, 0, 0, 0, 127, 127, 68, 68, 136, 136, 136, 128, 123, 127, 255, 255, 143, 143, 143, 128, 127, 127, 68, 68, 136, 248,
	248, 128, 123, 127, 255, 255, 143, 143, 143, 128, 127, 119, 119, 119, 120, 248, 248, 128, 123, 251, 251, 247, 143, 143, 143, 128, 119,
	119, 119, 119, 248, 248, 248, 128, 221, 127, 143, 143, 143, 143, 143, 128, 221, 116, 68, 68, 68, 68, 68, 128, 221, 116, 68, 68, 68, 68,
	68, 128, 221, 119, 119, 119, 119, 119, 119, 112, 221, 221, 221, 221, 221, 221, 221, 221
};

/* LMD_PROJUNIT_DISABLED.bmp */
static const unsigned char B_LMD_PROJUNIT_DISABLED[] = {
	66, 77, 252, 1, 0, 0, 0, 0, 0, 0, 250, 0, 0, 0, 40, 0, 0, 0, 16, 0, 0, 0, 16, 0, 0, 0, 1, 0, 8, 0, 0, 0, 0, 0, 2, 1, 0, 0, 18, 11, 0,
	0, 18, 11, 0, 0, 49, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 88, 38, 229, 0, 108, 58, 244, 0, 122, 72, 255, 0, 128, 20,
	231, 0, 157, 49, 253, 0, 170, 16, 240, 0, 186, 31, 252, 0, 190, 36, 255, 0, 255, 0, 255, 0, 76, 109, 251, 0, 85, 119, 255, 0, 0, 34,
	204, 0, 5, 39, 208, 0, 8, 32, 152, 0, 12, 46, 213, 0, 17, 51, 215, 0, 19, 53, 219, 0, 23, 57, 221, 0, 28, 62, 225, 0, 29, 63, 226, 0,
	39, 73, 233, 0, 40, 74, 234, 0, 45, 79, 238, 0, 49, 83, 240, 0, 37, 61, 178, 0, 54, 88, 243, 0, 52, 85, 232, 0, 56, 90, 246, 0, 63,
	97, 251, 0, 68, 102, 255, 0, 51, 76, 192, 0, 54, 79, 198, 0, 56, 80, 201, 0, 53, 70, 156, 0, 84, 109, 225, 0, 87, 111, 231, 0, 108,
	132, 244, 0, 95, 112, 191, 0, 83, 95, 155, 0, 166, 178, 237, 0, 170, 182, 240, 0, 186, 197, 252, 0, 192, 192, 192, 0, 128, 128, 128, 0,
	102, 102, 102, 0, 77, 77, 77, 0, 255, 255, 255, 0, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 10, 45, 44, 44, 44, 44, 44,
	44, 44, 44, 44, 44, 44, 0, 10, 10, 10, 45, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 44, 0, 10, 10, 10, 45, 1, 1, 47, 47, 47, 47, 47, 47,
	47, 1, 44, 0, 10, 10, 10, 45, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 44, 0, 10, 10, 10, 45, 1, 1, 44, 1, 47, 47, 47, 47, 47, 1, 44, 0, 10,
	10, 10, 40, 15, 13, 13, 34, 41, 44, 1, 44, 1, 44, 44, 0, 10, 10, 5, 14, 24, 31, 31, 24, 18, 35, 46, 46, 46, 1, 44, 0, 10, 7, 16, 28,
	13, 44, 1, 36, 27, 19, 42, 44, 1, 44, 44, 0, 10, 2, 23, 31, 12, 13, 44, 1, 32, 23, 26, 47, 47, 1, 44, 0, 10, 21, 23, 45, 31, 12, 13,
	44, 1, 23, 21, 44, 1, 44, 44, 0, 10, 22, 20, 45, 1, 31, 12, 13, 47, 20, 22, 1, 0, 0, 0, 0, 10, 3, 20, 33, 1, 44, 31, 12, 13, 20, 38,
	44, 44, 1, 45, 10, 10, 8, 24, 17, 37, 1, 1, 31, 11, 29, 43, 1, 44, 45, 10, 10, 10, 10, 6, 25, 19, 13, 13, 19, 30, 39, 45, 45, 45, 10,
	10, 10, 10, 10, 10, 9, 4, 31, 31, 4, 9, 10, 10, 10, 10, 10, 10, 10, 10, 0, 0,
};

// ---------------------------------------------------------------------------
void RegisterStateMachineIdent() {
#pragma startup RegisterStateMachineIdent
	TStateMachineProject::SetNodeClassIdent("scxmlproject");
	TStateMachineEditorUnit::SetNodeClassIdent("scxmlunit");
}

// ---------------------------------------------------------------------------
UnicodeString GetFileCheckSum(const UnicodeString &sFileName) {
	DWORD HeaderSum = 0;
	DWORD CheckSum = 0;

	DWORD dwRes = MapFileAndCheckSumW(sFileName.c_str(), &HeaderSum, &CheckSum);
	switch(dwRes) {
	case CHECKSUM_SUCCESS:
		return UnicodeString().sprintf(L"0x%X", CheckSum);
	case CHECKSUM_OPEN_FAILURE:
		WLOG_ERROR(L"Could not open the file [%s]", sFileName.c_str());
		break;
	case CHECKSUM_MAP_FAILURE:
		WLOG_ERROR(L"Could not map the file [%s]", sFileName.c_str());
		break;
	case CHECKSUM_MAPVIEW_FAILURE:
		WLOG_ERROR(L"Could not map a view of the file [%s]", sFileName.c_str());
		break;
	case CHECKSUM_UNICODE_FAILURE:
		WLOG_ERROR(L"Could not convert the file name to Unicode [%s]", sFileName.c_str());
		break;
	}
	return L"";
}

// ---------------------------------------------------------------------------
// TUnitPostSave
// ---------------------------------------------------------------------------

__fastcall TUnitPostSave::TUnitPostSave(TStateMachineEditorUnit *AUnit) : FUnit(AUnit) {

}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TUnitPostSave::GetMacros(void)const {
	std::auto_ptr<TStringList>AMacrosPtr(new TStringList);
	if (FUnit) {

		AMacrosPtr->Values["$(AppDir)"] = ExtractFileDir(Application->ExeName);
		AMacrosPtr->Values["$(AppFilePath)"] = Application->ExeName;
		AMacrosPtr->Values["$(AppHandle)"] = UnicodeString((int)Application->MainFormHandle);

		if (FUnit->StateMachineProject) {
			AMacrosPtr->Values["$(ProjectDir)"] = ExtractFileDir(FUnit->StateMachineProject->FilePath);
			AMacrosPtr->Values["$(ProjectName)"] = TPath::GetFileNameWithoutExtension(FUnit->StateMachineProject->FilePath);
		}

		AMacrosPtr->Values["$(UnitFilePath)"] = FUnit->FilePath;
		AMacrosPtr->Values["$(UnitFileName)"] = ExtractFileName(FUnit->FilePath);
		AMacrosPtr->Values["$(UnitFileDir)"] = ExtractFileDir(FUnit->FilePath);
		AMacrosPtr->Values["$(UnitName)"] = TPath::GetFileNameWithoutExtension(FUnit->FilePath);

		if (FUnit->StateMachineDockPanel) {
			AMacrosPtr->Values["$(UnitHandle)"] = UnicodeString((int)FUnit->StateMachineDockPanel->StateMachineEditor->Handle);
		}
	}

	return AMacrosPtr->Text;
}

// ---------------------------------------------------------------------------
// TStateMachineEditorUnit
// ---------------------------------------------------------------------------
__fastcall TStateMachineEditorUnit::TStateMachineEditorUnit(Lmdideprojmgr::TLMDProjectManager* AProjMgr,
	Lmdideprojmgr::TLMDProjNode* AParent) : Lmdideprojmgr::TLMDProjDocument(AProjMgr, AParent), StateMachineDockPanel(0), FScxmlName(L""),
FEnabled(true), FVirtualDependencies(NULL) {

	FBindingsDisabled = false;

	FInherited = L"";

	FPostSave = new TUnitPostSave(this);

	FIsBuildRoot = false;

	FBindingsDoc = LMDCreateXmlDocument(L"Config", L"1.0", L"UTF-8");
	FBindingsDoc->DocumentElement->AppendElement("container");

	FStateMachineUnitType = smutNormal;

	FVirtualDependencies = new TStringList;
}

// ---------------------------------------------------------------------------
__fastcall TStateMachineEditorUnit::~TStateMachineEditorUnit(void) {
	if (FPostSave) {
		delete FPostSave;
		FPostSave = 0;
	}

	if (FVirtualDependencies) {
		delete FVirtualDependencies;
		FVirtualDependencies = 0;
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditorUnit::GetEnabled(void) {
	if (FEnabled) {
		// если хотя бы один родитель заблокирован
		TStateMachineEditorUnit *AParentUnit = dynamic_cast<TStateMachineEditorUnit*>(this->Parent);
		if (AParentUnit && !AParentUnit->Enabled)
			return false;
	}
	return FEnabled;
}

// ---------------------------------------------------------------------------
ILMDXmlElement *__fastcall TStateMachineEditorUnit::GetBindingsContainerElement() {
	if (FBindingsDoc && FBindingsDoc->DocumentElement) {
		if (FBindingsDoc->DocumentElement->ChildNodes->Count) {
			return FBindingsDoc->DocumentElement->ChildNodes->Item[0]->AsElement();
		}
		else {
			return FBindingsDoc->DocumentElement->AppendElement("container");
		}
	}

	throw Exception(this->FileName + "> Container is NULL!");
}

// ---------------------------------------------------------------------------

const Lmdtypes::TLMDString TStateMachineEditorUnit::FileExt_ = L".scxml";

// ---------------------------------------------------------------------------
const UnicodeString TStateMachineEditorUnit::BindingsSuffix_ = L".bindings.xml";

// ---------------------------------------------------------------------------
const UnicodeString TStateMachineEditorUnit::OpenDialogFilter = L"Scxml files (*" + FileExt_ + ")|*" + FileExt_;

// ---------------------------------------------------------------------------
UnicodeString TStateMachineEditorUnit::MakeBindingFileName(const UnicodeString &sFileOrigin) {
	return ChangeFileExt(sFileOrigin, BindingsSuffix_);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditorUnit::DrawImage(Graphics::TCanvas* ACanvas, const Types::TRect &ARect) {

	std::auto_ptr<Graphics::TBitmap>ABitmapPtr(new Graphics::TBitmap());

	if (this->Enabled) {

		ABitmapPtr->Canvas->Brush->Color = clWhite;
		ABitmapPtr->Transparent = true;
		ABitmapPtr->SetSize(16, 16);

		ABitmapPtr->Canvas->Pen->Color = clBlack;
		ABitmapPtr->Canvas->Brush->Color = BindingsDisabled ? TColor(RGB(255, 255, 160)) : clWhite;

		TPoint APoints[5];
		APoints[0] = TPoint(2, 15);
		APoints[1] = TPoint(2, 1);
		APoints[2] = TPoint(11, 1);
		APoints[3] = TPoint(14, 4);
		APoints[4] = TPoint(14, 15);

		ABitmapPtr->Canvas->Polygon(APoints, 4);

		ABitmapPtr->Canvas->Pen->Color = clGray;
		ABitmapPtr->Canvas->MoveTo(2, 14);
		ABitmapPtr->Canvas->LineTo(2, 1);
		ABitmapPtr->Canvas->LineTo(11, 1);
		ABitmapPtr->Canvas->LineTo(13, 3);
		ABitmapPtr->Canvas->MoveTo(2, 8);
		ABitmapPtr->Canvas->LineTo(3, 8);

		ABitmapPtr->Canvas->Pen->Color = clSilver;
		ABitmapPtr->Canvas->MoveTo(3, 14);
		ABitmapPtr->Canvas->LineTo(13, 14);
		ABitmapPtr->Canvas->LineTo(13, 5);

		UnicodeString sMaskText = L"";

		if (!FileExists(this->FilePath)) {
			sMaskText = L"?";
		}
		else {
			if (StateMachineUnitType == smutVirtual) {
				sMaskText = L"V";
			}
		}

		const bool bHasTextInfo = !sMaskText.IsEmpty();
		if (bHasTextInfo) {
			ABitmapPtr->Canvas->Font->Color = clBlack;
			ABitmapPtr->Canvas->Font->Name = L"Tahoma";
			ABitmapPtr->Canvas->Brush->Style = bsClear;
			ABitmapPtr->Canvas->TextOut(6, 2, sMaskText);
		}
		else {
			ABitmapPtr->Canvas->Pen->Color = clBlack;
			ABitmapPtr->Canvas->MoveTo(11, 4);
			ABitmapPtr->Canvas->LineTo(14, 4);

			ABitmapPtr->Canvas->Pen->Color = clTeal;
			ABitmapPtr->Canvas->MoveTo(5, 4);
			ABitmapPtr->Canvas->LineTo(9, 4);
			ABitmapPtr->Canvas->MoveTo(7, 6);
			ABitmapPtr->Canvas->LineTo(11, 6);
			ABitmapPtr->Canvas->MoveTo(7, 10);
			ABitmapPtr->Canvas->LineTo(11, 10);
			ABitmapPtr->Canvas->MoveTo(5, 12);
			ABitmapPtr->Canvas->LineTo(11, 12);

			ABitmapPtr->Canvas->Pen->Color = clRed;
			ABitmapPtr->Canvas->MoveTo(5, 8);
			ABitmapPtr->Canvas->LineTo(11, 8);

			ABitmapPtr->Canvas->Pen->Color = clBlue;
			ABitmapPtr->Canvas->MoveTo(3, 7);
			ABitmapPtr->Canvas->LineTo(4, 8);
			ABitmapPtr->Canvas->LineTo(3, 9);
		}

		if (this->IsInherited) {
			ABitmapPtr->Canvas->Pen->Color = clNavy;
			ABitmapPtr->Canvas->Brush->Color = clTeal;

			TPoint APoints[3];
			APoints[0] = TPoint(0, 1);
			APoints[1] = TPoint(0, 11);
			APoints[2] = TPoint(5, 6);

			ABitmapPtr->Canvas->Polygon(APoints, 2);

			ABitmapPtr->Canvas->MoveTo(1, 1);
			ABitmapPtr->Canvas->LineTo(6, 6);
			ABitmapPtr->Canvas->LineTo(1, 11);
		}

		const bool bIsBase = IsBase;
		if (bIsBase) {
			ABitmapPtr->Canvas->Pen->Color = clBlack;
			ABitmapPtr->Canvas->Brush->Color = TColor(RGB(131, 191, 254));
			ABitmapPtr->Canvas->Brush->Style = bsSolid;

			const int iWidth = bHasTextInfo ? 6 : 8;
			ABitmapPtr->Canvas->Rectangle(ABitmapPtr->Width - iWidth, 0, ABitmapPtr->Width, iWidth);
		}

		if (IsBuildRoot) {
			ABitmapPtr->Canvas->Pen->Color = clBlack;
			ABitmapPtr->Canvas->Brush->Color = TColor(RGB(128, 255, 128));
			ABitmapPtr->Canvas->Brush->Style = bsSolid;

			const int iWidth = bHasTextInfo ? 6 : (bIsBase ? 8 : 10);
			const int iX = ABitmapPtr->Width - iWidth;
			const int iY = ABitmapPtr->Height - iWidth;
			ABitmapPtr->Canvas->Ellipse(iX, iY, iX + iWidth, iY + iWidth);
		}
	}
	else {
		std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
		AMemoryStream->WriteBuffer(B_LMD_PROJUNIT_DISABLED, sizeof(B_LMD_PROJUNIT_DISABLED));

		AMemoryStream->Position = 0;

		ABitmapPtr->LoadFromStream(AMemoryStream.get());
		ABitmapPtr->Transparent = true;
	}

	ACanvas->Draw(ARect.Left, ARect.Top, ABitmapPtr.get());

	// то, что за пределами (дополнительные пометки)
	if (this->IsModified(false)) {
		ACanvas->Brush->Style = bsClear;
		ACanvas->TextOut(ARect.Right + ACanvas->TextWidth(this->DisplayName + " "), ARect.Top, L"*");
	}
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TStateMachineEditorUnit::DoGetDisplayName(void) {
	return GetRelativeFilePath();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineEditorUnit::GetScxmlNameFromSource(const UnicodeString &sPath) {
	std::auto_ptr<TXMLDocument>ADocPtr(new TXMLDocument(Application));
	ADocPtr->LoadFromFile(sPath);
	if (!ADocPtr->DocumentElement)
		throw Exception("DocumentElement is not defined!");
	if (ADocPtr->DocumentElement->NodeName != L"scxml")
		throw Exception("Root node must have name 'scxml'");
	if (!ADocPtr->DocumentElement->HasAttribute(L"name"))
		throw Exception("Require scxml name");

	return ADocPtr->DocumentElement->GetAttribute(L"name");
}

// этот метод вызывается, когда загружается проект
// вычитывается узел XML с параметрами настройки юнита
// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditorUnit::DoRead(Lmdxml::_di_ILMDXmlElement AElem, bool AIsParentXml) {
	if (!AIsParentXml)
		throw Exception("Script unit have no self XML");

	const TLMDString filepath = MakeFullFilePath(AElem->GetAttr(L"path"));

	FInherited = AElem->GetAttr(L"inherited", L"");
	FBindingsDisabled = AElem->GetBoolAttr(L"bindings_disabled", false);
	FIsBuildRoot = AElem->GetBoolAttr(L"build_root", false);
	FStateMachineUnitType = TStateMachineUnitType(AElem->GetIntAttr(L"virtual", smutNormal));

	// FScxmlName - необходимо для того, чтобы знать имя Scxml при закрытом редакторе
	// вызывать до Load!
	// так как там будет происходить проверка, что ScxmlName не пустое
	FScxmlName = AElem->GetAttr(L"scxmlname", L"");

	FEnabled = AElem->GetBoolAttr(L"enabled", true);

	FPostSave->PostBuildCMD->Text = AElem->GetAttr(L"postsave_cmd", L"");
	FPostSave->UserMacros->Text = AElem->GetAttr(L"postsave_usermacros", L"");

	FVirtualDependencies->Text = AElem->GetAttr(L"virtual_dependencies", L"");

	// это общий метод для загрузки из проекта и любых дальнейших операций по добавлению, открытию и т.д.
	Load(filepath);

	// рекурсивно читаем все дочерние подузлы в проекте относительно текущего
	ReadChildren(AElem);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditorUnit::DoWrite(Lmdxml::_di_ILMDXmlElement AElem, bool AIsParentXml) {
	if (!AIsParentXml)
		throw Exception("Script unit have no self XML");

	Save(); // Save before, to get valid filepath.
	AElem->SetAttr(L"path", GetRelativeFilePath());
	AElem->SetAttr(L"scxmlname", ScxmlName);

	if (!FEnabled) {
		AElem->SetBoolAttr(L"enabled", FEnabled); // здесь именно так, чтобы не подтягивать дочерние
	}
	if (!Inherited.IsEmpty()) {
		AElem->SetAttr(L"inherited", Inherited);
	}
	if (BindingsDisabled) {
		AElem->SetBoolAttr(L"bindings_disabled", BindingsDisabled);
	}
	if (IsBuildRoot) {
		AElem->SetBoolAttr(L"build_root", IsBuildRoot);
	}

	if (FStateMachineUnitType != smutNormal) {
		AElem->SetIntAttr(L"virtual", FStateMachineUnitType);
	}

	if (!FPostSave->PostBuildCMD->Text.Trim().IsEmpty()) {
		AElem->SetAttr(L"postsave_cmd", FPostSave->PostBuildCMD->Text.Trim());
	}

	if (!FPostSave->UserMacros->Text.Trim().IsEmpty()) {
		AElem->SetAttr(L"postsave_usermacros", FPostSave->UserMacros->Text.Trim());
	}

	if (!FVirtualDependencies->Text.Trim().IsEmpty()) {
		AElem->SetAttr(L"virtual_dependencies", FVirtualDependencies->Text.Trim());
	}

	WriteChildren(AElem);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineEditorUnit::GetScxmlName(void) {
	// если редактор открыт, то берем значение из редактора, иначе из величины, которая получена из XML проекта
	return StateMachineDockPanel ? StateMachineDockPanel->ScxmlName : FScxmlName;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineEditorUnit::GetBindingsName(void) {
	return MakeBindingFileName(FilePath);
}

// этот метод вызывается при добавлении юнита, открытии, загрузке проекта
// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditorUnit::DoLoad(const Lmdtypes::TLMDString AFilePath) {
	/*
	Здесь ещё нельзя пользоваться 'this->FileName', так как оно ещё не определено
	 */
	const UnicodeString sFileName = ExtractFileName(AFilePath);

	// Do nothing.
	// Do not call inherited.
	const bool b_UNIT_EXISTS = FileExists(AFilePath);
	if (!b_UNIT_EXISTS)
		WLOG_ERROR(L"File <%s> is not found!", AFilePath.c_str());
	else {
		// если мы в режиме проекта, тогда нам необходимо проверить имя Scxml
		if (this->StateMachineProject) {
			if (ScxmlName.IsEmpty()) {
				const UnicodeString sFileName = ExtractFileName(AFilePath);

				try {
					const UnicodeString sScxmlName = GetScxmlNameFromSource(AFilePath);
					ScxmlName = sScxmlName;

					WLOG_INFO(L"Unit:[%s] ScxmlName was set to:[%s]. Save Project!", sFileName.c_str(), sScxmlName.c_str());
				}
				catch(Exception * E) {
					WLOG_WARNING(L"Unit:[%s] can not get ScxmlName! Error:[%s]", E->Message.c_str(), sFileName.c_str());
				}

				if (ScxmlName.IsEmpty()) {
					WLOG_WARNING(L"Unit:[%s] does not have stored ScxmlName! Thus Debugging is not available for this unit!",
						sFileName.c_str());
				}
			}
		}
	}

	if (BindingsContainer) {
		BindingsContainer->SetAttr("NAME", sFileName);
	}

	if (SettingsData->AviaExtensionEnabled && !BindingsDisabled) {
		try {
			const UnicodeString sBingingFile = MakeBindingFileName(AFilePath);
			if (FileExists(sBingingFile)) {
				// только через StringList, так как иначе неправильно записывается кодировка!!!
				std::auto_ptr<TStringList>AStringList(new TStringList());
				AStringList->LoadFromFile(sBingingFile, TEncoding::UTF8);
				FBindingsDoc->LoadXML(AStringList->Text);
			}
			else {
				// очень спорный момент, долго думал как реализовать,
				// если проект тесно связан с привязками, тогда критично отсутствие
				if (SettingsData->WarningOnBindingsNotFound) {
					WLOG_WARNING(L"File <%s> is not found!", sBingingFile.c_str());
				}
				// но также бывает, когда мы в расширенном режиме используем только функционал модели 'luavia',
				// тогда привязок нет
				else {
					WLOG_DEBUG(L"File <%s> is not found!", sBingingFile.c_str());
				}
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineEditorUnit::DoSave(const Lmdtypes::TLMDString AFilePath) {
	try {
		SaveAssociatedFile("code", AFilePath, FilePath);
	}
	catch(Exception * E) {
		// Fix: https://github.com/alexzhornyak/ScxmlEditor-Tutorial/issues/105
		if (StateMachineDockPanel) {
			StateMachineDockPanel->MarkModified(0, L"", false);
		}
		throw E;
	}

	const UnicodeString sFileName = ExtractFileName(AFilePath);

	if (SettingsData->AviaExtensionEnabled && !BindingsDisabled) {
		try {
			const UnicodeString sBindingsFile = MakeBindingFileName(AFilePath);
			// пишем только, если есть подписки
			if (BindingsContainer && BindingsContainer->ChildNodes->Count) {
				BindingsContainer->SetAttr("NAME", sFileName);
				FBindingsDoc->Save(sBindingsFile);
			}
			else {
				WLOG_DEBUG(L"There are no bindings yet for unit [%s]", sBindingsFile.c_str());
				if (FileExists(sBindingsFile)) {
					DeleteFile(sBindingsFile);
				}
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}

	try {
		if (!FPostSave->PostBuildCMD->Text.Trim().IsEmpty()) {
			FormScxmlGui->SwitchToTesterLog(true);

			FormScxmlGui->EditDocTester->Lines->Append("Post save [" + AFilePath + "] started>>>");
			FormScxmlGui->EditDocTester->Lines->Append("****************************************");

			const UnicodeString sTempDir = ExtractFilePath(AFilePath);
			const UnicodeString sTempFileName = L"~" + ExtractFileName(AFilePath) + L".cmd";
			const UnicodeString sTempFile = TPath::Combine(sTempDir, sTempFileName);

			DeleteFile(sTempFile);

			std::auto_ptr<TStringList>APostBuildPtr(new TStringList());
			APostBuildPtr->Text = FPostSave->PostBuildEvaluated;
			APostBuildPtr->SaveToFile(sTempFile, TEncoding::GetEncoding(866));

			TFileAttributes AFileAttr = TFile::GetAttributes(sTempFile);
			TFile::SetAttributes(sTempFile, AFileAttr << Ioutils::faHidden);

			// запуск скрипта на исполнение
			TSpawnAndRedirect ASpawner(AnsiQuotedStr(sTempFile, L'"'), ExtractFileDir(sTempFile));
			ASpawner.ConvertOutputEncoding = coeOEM866;
			ASpawner.StartProcessInfinite();

			DeleteFile(sTempFile);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	/* запись контрольной суммы в проект */
	/* P.S. Также контрольные суммы будут записаны при сохранении проекта */
	if (StateMachineProject && StateMachineProject->ProjectProperties->VerifyChecksums) {
		try {
			const UnicodeString sCheckSumFile = StateMachineProject->FilePath + ".checksum";
			std::auto_ptr<TStringList>ACheckSumListPtr(new TStringList());
			if (FileExists(sCheckSumFile)) {
				ACheckSumListPtr->LoadFromFile(sCheckSumFile);
			}

			ACheckSumListPtr->Values[sFileName] = GetFileCheckSum(AFilePath);
			ACheckSumListPtr->SaveToFile(sCheckSumFile);
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
Classes::TStream* __fastcall TStateMachineEditorUnit::DoOpenFileStream(const Lmdtypes::TLMDString AFileIdent) {
	if (LMDSameText(AFileIdent, "code"))
		return ProjMgr->CreateStream(FilePath, skRead);
	else
		return Lmdideprojmgr::TLMDProjDocument::DoOpenFileStream(AFileIdent);
}

/* Вспомогательные функции иммитирующие формирование имени в SDK */
// ---------------------------------------------------------------------------
bool _DoesParentUnitFileNameExist(TLMDProjDocument *ASelf, TLMDProjDocument *AParent, const UnicodeString &sFileName) {
	if (AParent) {
		for (int i = 0; i < AParent->DocumentCount; i++) {
			if (AParent->Documents[i] != ASelf && SameText(ExtractFileName(AParent->Documents[i]->FilePath), sFileName)) {
				return true;
			}
		}
	}

	return false;
}

// ---------------------------------------------------------------------------
std::pair<UnicodeString, int>_MakeParentUniqueFileName(TLMDProjDocument *ASelf, const UnicodeString &sBaseName, const UnicodeString &sExt) {
	UnicodeString sRes = L"";
	int iRes = 0;
	if (ASelf) {
		TLMDProjDocument *AParent = ASelf->FindParentDocument();

		int i = 1;
		do {
			sRes = sBaseName + UnicodeString(i) + sExt;
			iRes = i;
			i++;
		}
		while (_DoesParentUnitFileNameExist(ASelf, AParent, sRes));
	}
	return std::make_pair(sRes, iRes);
}

// ---------------------------------------------------------------------------
bool _DoesGlobalUnitFileNameExist(TStateMachineEditorUnit *AOwn, const UnicodeString &sFileName) {
	if (AOwn->ProjMgr && AOwn->ProjMgr->Root) {
		std::vector<TStateMachineEditorUnit*>AVecUnits;
		CollectAllEnabledUnits(AOwn->ProjMgr->Root, AVecUnits);
		for (std::size_t i = 0; i < AVecUnits.size(); i++) {
			if (AOwn != AVecUnits[i] && SameText(ExtractFileName(AVecUnits[i]->FilePath), sFileName)) {
				return true;
			}
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
const UnicodeString TStateMachineEditorUnit::DefaultUnitBaseName = L"state_machine";

// ---------------------------------------------------------------------------
const UnicodeString TStateMachineEditorUnit::DefaultVirtualUnitBaseName = L"virtual_machine";

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TStateMachineEditorUnit::DoGetDefaultBaseName(void) {
	// исправляем bugfix, когда может повториться имя при добавлении на разных уровнях вложенности
	// возможности исправить это переопределив что-то нет,
	// доступ к имени закрыт

	UnicodeString sBaseName = L"";
	switch(GlobalStateMachineUnitType) {
	case smutVirtual:
		sBaseName = DefaultVirtualUnitBaseName;
		break;
	default:
		sBaseName = DefaultUnitBaseName;
	}

	if (this->Parent && this->Parent->InheritsFrom(__classid(TStateMachineEditorUnit))) {
		// прогнозируемое имя, какое бы было создано по умолчанию
		std::pair<UnicodeString, int>AExpectedPairName = _MakeParentUniqueFileName(this, sBaseName, DoGetFileExt());

		if (_DoesGlobalUnitFileNameExist(this, AExpectedPairName.first)) {
			do {
				sBaseName = sBaseName + L"A";
			}
			while (_DoesGlobalUnitFileNameExist(this, sBaseName + UnicodeString(AExpectedPairName.second) + DoGetFileExt()));
		}
	}

	return sBaseName;
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TStateMachineEditorUnit::DoGetFileExt(void) {
	return FileExt_;
}

// ---------------------------------------------------------------------------
TStateMachineProject * __fastcall TStateMachineEditorUnit::GetStateMachineProject() {
	return this->ProjMgr ? dynamic_cast<TStateMachineProject*>(this->ProjMgr->Root) : NULL;
}

// ---------------------------------------------------------------------------
void GetInheritedUnitsFromUnit(TStateMachineEditorUnit * AUnit, const std::vector<TStateMachineEditorUnit*>&AVecUnits,
	std::vector<TStateMachineEditorUnit *> &AVecInherited) {

	for (std::size_t i = 0; i < AVecUnits.size(); i++) {
		if (AVecUnits[i]->Enabled && AVecUnits[i] != AUnit && SameText(AVecUnits[i]->Inherited, AUnit->FileName)) {

			if (std::find(AVecInherited.begin(), AVecInherited.end(), AVecUnits[i]) == AVecInherited.end()) {
				AVecInherited.push_back(AVecUnits[i]);

				GetInheritedUnitsFromUnit(AVecUnits[i], AVecUnits, AVecInherited);
			}
		}
	}
}

// ---------------------------------------------------------------------------
std::vector<TStateMachineEditorUnit*>__fastcall TStateMachineEditorUnit::GetInheritedUnitsFromThis(void) {
	std::vector<TStateMachineEditorUnit*>AVecOutUnits;
	if (StateMachineProject) {
		std::vector<TStateMachineEditorUnit*>AVecUnits;
		CollectAllEnabledUnits(StateMachineProject, AVecUnits);

		GetInheritedUnitsFromUnit(this, AVecUnits, AVecOutUnits);
	}
	return AVecOutUnits;
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineEditorUnit::GetIsBase(void) {
	if (StateMachineProject && !this->FileName.IsEmpty()) {
		std::vector<TStateMachineEditorUnit*>AVecUnits;
		CollectAllEnabledUnits(StateMachineProject, AVecUnits);

		for (std::size_t i = 0; i < AVecUnits.size(); i++) {
			if (AVecUnits[i] != this && SameText(AVecUnits[i]->Inherited, this->FileName)) {
				return true;
			}
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
std::set<TStateMachineEditorUnit *>__fastcall TStateMachineEditorUnit::GetVirtualParents(void) {
	std::set<TStateMachineEditorUnit*>AVirtualParents;
	if (StateMachineProject && this->IsVirtual) {
		std::vector<TStateMachineEditorUnit*>AVecUnits;
		CollectAllEnabledUnits(StateMachineProject, AVecUnits);

		GetUnitVirtualParents(AVecUnits, this, AVirtualParents);
	}
	return AVirtualParents;
}

// ---------------------------------------------------------------------------
/* TStateMachineProject */
// ---------------------------------------------------------------------------
__fastcall TStateMachineProject::TStateMachineProject(Lmdideprojmgr::TLMDProjectManager* AProjMgr, Lmdideprojmgr::TLMDProjNode* AParent)
	: Lmdideprojmgr::TLMDProjDocument(AProjMgr, AParent), FProjectProperties(new TProjectProperties(NULL, this)) {

	ControlsXMLDoc = LMDCreateXmlDocument(L"Config", L"1.0", L"UTF-8");

	FLoadError = 0;
}

// ---------------------------------------------------------------------------
__fastcall TStateMachineProject::~TStateMachineProject(void) {
	if (FProjectProperties) {
		delete FProjectProperties;
	}
}

// ---------------------------------------------------------------------------
const Lmdtypes::TLMDString TStateMachineProject::FileExt_ = L".sproj";

// ---------------------------------------------------------------------------
const UnicodeString TStateMachineProject::OpenDialogFilter = L"Scxml Projects (*" + FileExt_ + ")|*" + FileExt_;

// ---------------------------------------------------------------------------

void __fastcall TStateMachineProject::DrawImage(Graphics::TCanvas* ACanvas, const Types::TRect &ARect) {

	std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
	AMemoryStream->WriteBuffer(B_LMD_PROJPROJECT, sizeof(B_LMD_PROJPROJECT));
	AMemoryStream->Position = 0;

	std::auto_ptr<Graphics::TBitmap>ABitmap(new Graphics::TBitmap());
	ABitmap->LoadFromStream(AMemoryStream.get());
	ABitmap->Transparent = true;
	ABitmap->TransparentColor = ABitmap->Canvas->Pixels[0][0];

	ACanvas->Draw(ARect.Left, ARect.Top, ABitmap.get());

	if (this->IsModified(false)) {
		ACanvas->TextOutA(ARect.Right + ACanvas->TextWidth(this->DisplayName + " "), ARect.Top, L"*");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineProject::DoRead(Lmdxml::_di_ILMDXmlElement AElem, bool AIsParentXml) {
	// чтение дочерних элементов
	ReadChildren(AElem);
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineProject::DoWrite(Lmdxml::_di_ILMDXmlElement AElem, bool AIsParentXml) {
	WriteChildren(AElem);
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TStateMachineProject::DoGetDefaultBaseName(void) {
	return L"project";
}

// ---------------------------------------------------------------------------
Lmdtypes::TLMDString __fastcall TStateMachineProject::DoGetFileExt(void) {
	return FileExt_;
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineProject::DoLoad(const Lmdtypes::TLMDString AFilePath) {
	try {
		const UnicodeString sProps = GetPropertiesFileName(AFilePath);
		if (FileExists(sProps)) {
			FProjectProperties->LoadFromFile(sProps);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	try {
		if (SettingsData->AviaExtensionEnabled) {
			const UnicodeString sXMLControls = GetControlBindingFileName(AFilePath);
			if (FileExists(sXMLControls)) {
				// только через StringList, так как иначе неправильно записывается кодировка!!!
				std::auto_ptr<TStringList>AStringList(new TStringList());
				AStringList->LoadFromFile(sXMLControls, TEncoding::UTF8);
				ControlsXMLDoc->LoadXML(AStringList->Text);
			}
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	/* обязательно в таком порядке, чтобы сначала загрузились свойства проекта и подписки */
	LoadFromXml(AFilePath);

	/* проверка контрольных сумм проекта
	все юниты должны быть загружены в этот момент */
	try {
		if (FProjectProperties->VerifyChecksums) {
			const UnicodeString sCheckSumFile = AFilePath + ".checksum";
			std::auto_ptr<TStringList>ACheckSumListPtr(new TStringList());
			if (FileExists(sCheckSumFile)) {
				ACheckSumListPtr->LoadFromFile(sCheckSumFile);

				std::vector<TStateMachineEditorUnit*>AVecUnits;
				CollectAllEnabledUnits(this, AVecUnits);
				for (std::size_t i = 0; i < AVecUnits.size(); i++) {
					const UnicodeString sCheckSum = GetFileCheckSum(AVecUnits[i]->FilePath);
					if (ACheckSumListPtr->Values[AVecUnits[i]->FileName] != sCheckSum) {
						WLOG_ERROR(L"File:[%s] checksum:[%s] mismatch:[%s]!", AVecUnits[i]->FileName.c_str(),
							ACheckSumListPtr->Values[AVecUnits[i]->FileName].c_str(), sCheckSum.c_str());
						// помечаем с ошибкой, так как в функции Load будет сброшено свойство Modified,
						// а нам необходимо проинформировать пользователя
						FLoadError = LOAD_ERROR_CHECKSUM_MISMATCH;

						// теперь необходимо пометить все юниты, у которых есть виртуальные ссылки
						for (std::size_t k = 0; k < AVecUnits.size(); k++) {
							if (AVecUnits[k]->VirtualDependencies->IndexOf(AVecUnits[i]->FileName) != -1) {
								if (!AVecUnits[k]->Opened) {
									AVecUnits[k]->Open(true);
								}
								TStateMachineDockPanel *ADockPanel = AVecUnits[k]->StateMachineDockPanel;
								if (ADockPanel) {
									ADockPanel->MarkModified(1, L"Virtual file [" + AVecUnits[i]->FileName + "] has been changed!", false);
								}
							}
						}
					}
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineProject::DoSave(const Lmdtypes::TLMDString AFilePath) {
	/* записываем все юниты */
	SaveToXml(AFilePath);

	/* сбрасываем ошибку при загрузке */
	FLoadError = 0;

	try {
		FProjectProperties->SaveToFile(GetPropertiesFileName(AFilePath));
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	try {
		if (SettingsData->AviaExtensionEnabled) {
			SaveControlBindings(GetControlBindingFileName(AFilePath));
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	/*
	Так как одни и те же файлы могут использоваться в разных проектах, то необходимо иметь информацию о контрольных суммах,
	чтобы при изменениях в других проектах, заново сгенерировать подписки, наследования и т.д.
	 */
	try {
		if (FProjectProperties->VerifyChecksums) {
			// здесь перезапись всех контрольных сумм проекта
			const UnicodeString sCheckSumFile = AFilePath + ".checksum";
			std::auto_ptr<TStringList>ACheckSumListPtr(new TStringList());

			std::vector<TStateMachineEditorUnit*>AVecUnits;
			CollectAllEnabledUnits(this, AVecUnits);
			for (std::size_t i = 0; i < AVecUnits.size(); i++) {
				ACheckSumListPtr->Values[AVecUnits[i]->FileName] = GetFileCheckSum(AVecUnits[i]->FilePath);
			}

			ACheckSumListPtr->SaveToFile(sCheckSumFile);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	WLOG_INFO(L"Project [%s] was saved!", AFilePath.c_str());
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineProject::GetControlBindingFileName(const UnicodeString sFilePath /* =L"" */ ) {
	return ChangeFileExt(sFilePath.IsEmpty() ? this->FilePath : sFilePath, ".sproj.xml");
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineProject::ValidateControlBindings() {
	std::auto_ptr<TStringList>AControlsListPtr(new TStringList());

	typedef boost::tuple<UnicodeString /* name */ , UnicodeString /* type */ , UnicodeString /* param */ ,
	UnicodeString /* description */ , UnicodeString /* min */ , UnicodeString /* max */ ,
	UnicodeString /* respond */ >TProtocolControlTuple;
	enum {
		pcttName, pcttType, pcttParam, pcttDescription, pcttMin, pcttMax, pcttRespond
	};
	typedef std::map<UnicodeString /* ScxmlName */ , TProtocolControlTuple>TStrongProtocolControlMap;

	TStrongProtocolControlMap AStrongControlMap;

	if (ControlsXMLDoc->DocumentElement) {
		for (int i = 0; i < ControlsXMLDoc->DocumentElement->ChildNodes->Count; i++) {
			if (ControlsXMLDoc->DocumentElement->ChildNodes->Item[i]->NodeName == "container") {

				UnicodeString sContainerName = ControlsXMLDoc->DocumentElement->ChildNodes->Item[i]->GetAttr("NAME");

				for (int k = 0; k < ControlsXMLDoc->DocumentElement->ChildNodes->Item[i]->ChildNodes->Count; k++) {
					_di_ILMDXmlNode AItem = ControlsXMLDoc->DocumentElement->ChildNodes->Item[i]->ChildNodes->Item[k];
					if (AItem->NodeName == "item") {
						const UnicodeString sShapeType = AItem->GetAttr("ShapeType");
						const UnicodeString sScxmlName = AItem->GetAttr("ScxmlName");
						const UnicodeString sShape = AItem->GetAttr("ShapeName");
						if (sShapeType == "TProtocolWeakBindingShape") {

							TStrongProtocolControlMap::iterator it = AStrongControlMap.find(sScxmlName);

							if (it == AStrongControlMap.end()) {
								WLOG_ERROR(L"Shape[%s:%s] with name:[%s] is weak binding and require full binding previously!!!",
									sShape.c_str(), sContainerName.c_str(), sScxmlName.c_str());
							}
							else {
								AItem->SetAttr("Name", boost::get<pcttName>(it->second));
								AItem->SetAttr("Type", boost::get<pcttType>(it->second));
								AItem->SetAttr("Param", boost::get<pcttParam>(it->second));
								AItem->SetAttr("Description", boost::get<pcttDescription>(it->second));

								if (!boost::get<pcttMin>(it->second).IsEmpty()) {
									AItem->SetAttr("Min", boost::get<pcttMin>(it->second));
								}

								if (!boost::get<pcttMax>(it->second).IsEmpty()) {
									AItem->SetAttr("Max", boost::get<pcttMax>(it->second));
								}

								if (!boost::get<pcttRespond>(it->second).IsEmpty()) {
									AItem->SetAttr("Respond", boost::get<pcttRespond>(it->second));
								}
							}
						}
						else {
							// дальше предполагаем, что это полноценный TProtocolControlBinding
							const UnicodeString sRawControlName = AItem->GetAttr("Name");
							// отдельный случай Dummy
							if (!SameText(sRawControlName, L"%_SERVER_SESSION_DUMMY_%")) {

								const UnicodeString sName = sRawControlName + ";" + AItem->GetAttr("Type");

								const UnicodeString sValue = AItem->GetAttr("Param") + ";" + sScxmlName;

								// добававляем для проверки TProtocolWeakBindingShape
								AStrongControlMap.insert(std::make_pair(AItem->GetAttr("ScxmlName"),
										boost::make_tuple(AItem->GetAttr("Name"), AItem->GetAttr("Type"), AItem->GetAttr("Param"),
											AItem->GetAttr("Description"), AItem->GetAttr("Min"), AItem->GetAttr("Max"),
											AItem->GetAttr("Respond"))));

								const int iNameIndex = AControlsListPtr->IndexOfName(sName);
								if (iNameIndex == -1) {
									AControlsListPtr->Values[sName] = sValue;
								}
								else {

									if (sValue != AControlsListPtr->ValueFromIndex[iNameIndex]) {

										const UnicodeString sMsg = UnicodeString().sprintf
											(L"Shape[%s:%s] has same control[%s] for params[%s] and params[%s]!", sContainerName.c_str(),
											sShape.c_str(), sName.c_str(), sValue.c_str(),
											AControlsListPtr->ValueFromIndex[iNameIndex].c_str());

										if (ProjectProperties->DisableSameControls)
											throw Exception(sMsg);
										else
											WLOG_WARNING(L"%s", sMsg.c_str());
									}
									else {
										WLOG_WARNING(L"Shape[%s:%s] with params[%s] is duplicated!!!", sContainerName.c_str(),
											sShape.c_str(), sValue.c_str());
									}

								}
							}
						}
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void RecursiveSaveContainers(TLMDProjDocument *ARoot, ILMDXmlElement *AParentNode) {
	if (ARoot) {
		TStateMachineEditorUnit *AUnit = dynamic_cast<TStateMachineEditorUnit*>(ARoot);
		if (AUnit && AUnit->Enabled && !AUnit->BindingsDisabled) {
			ILMDXmlElement *AElemContainer = AUnit->BindingsContainer;
			// не пишем пустые контейнеры
			if (AElemContainer && AElemContainer->ChildNodes->Count) {
				AElemContainer->SetAttr("NAME", AUnit->FileName);

				_di_ILMDXmlNode ANode = AElemContainer->CloneNode(true);

				AParentNode->AppendChild(ANode);
			}
		}

		for (int i = 0; i < ARoot->DocumentCount; i++) {
			RecursiveSaveContainers(ARoot->Documents[i], AParentNode);
		}
	}

}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineProject::SaveControlBindings(const UnicodeString &sFileName) {
	try {
		UnicodeString sOldXML = L"";

		if (ControlsXMLDoc->DocumentElement) {
			sOldXML = ControlsXMLDoc->XML;

			ControlsXMLDoc->DocumentElement->RemoveAllChilds();

			RecursiveSaveContainers(this, (ILMDXmlElement*)ControlsXMLDoc->DocumentElement);
		}

		ValidateControlBindings();

		// если подписок нет, то вообще не формируем этот файл
		if (ControlsXMLDoc->DocumentElement->ChildNodes->Count) {
			// если записываем с новым именем или что-то поменялось
			if (GetControlBindingFileName(FilePath) != sFileName || sOldXML != ControlsXMLDoc->XML) {

				if (FormScxmlGui) {
					FormScxmlGui->actUpdateBindings->Execute();
				}

				ControlsXMLDoc->Save(sFileName);

				WLOG_INFO(L"Successfully saved xml controls <%s>", sFileName.c_str());
			}
		}
		else {
			WLOG_DEBUG(L"There are no bindings yet for project [%s]", sFileName.c_str());
			if (FileExists(sFileName)) {
				DeleteFile(sFileName);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TStateMachineProject::GetPropertiesFileName(const UnicodeString sFilePath /* =L"" */ ) {
	return ChangeFileExt(sFilePath.IsEmpty() ? this->FilePath : sFilePath, ".sproj.props");
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineProject::SaveProperties(const UnicodeString &sFileName) {
	try {
		FProjectProperties->SaveToFile(sFileName);
		WLOG_INFO(L"Successfully saved project options <%s>", sFileName.c_str());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TStateMachineProject::SaveProperties() {
	this->SaveProperties(this->GetPropertiesFileName());
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit *__fastcall TStateMachineProject::GetRootForBuild(void) {
	std::vector<TStateMachineEditorUnit*>AVecUnits;
	CollectAllEnabledUnits(this, AVecUnits);
	for (std::size_t i = 0; i < AVecUnits.size(); i++) {
		if (AVecUnits[i]->Enabled && AVecUnits[i]->IsBuildRoot) {
			return AVecUnits[i];
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit * __fastcall TStateMachineProject::GetProjectRoot(void) {
	std::vector<TStateMachineEditorUnit*>AVecUnits;
	CollectAllEnabledUnits(this, AVecUnits);
	// сначала первый помеченный IsBuildRoot
	for (std::size_t i = 0; i < AVecUnits.size(); i++) {
		if (AVecUnits[i]->IsBuildRoot) {
			return AVecUnits[i];
		}
	}
	// иначе первый доступный
	if (AVecUnits.size() != 0) {
		return AVecUnits[0];
	}
	return NULL;
}

// ---------------------------------------------------------------------------
bool __fastcall TStateMachineProject::CheckAllUnitsForSameScxmlNames(void) {
	bool bRes = false;
	std::auto_ptr<TStringList>ASetListPtr(new TStringList());

	std::vector<TStateMachineEditorUnit*>AVecUnits;
	CollectAllEnabledUnits(this, AVecUnits);
	for (std::size_t i = 0; i < AVecUnits.size(); i++) {
		const int iIndex = ASetListPtr->IndexOfName(AVecUnits[i]->ScxmlName);
		if (iIndex != -1) {
			bRes = true;
			WLOG_ERROR(L"SCXML Name [%s] in [%s] has the same name as in [%s]! It may lead to ambigudity during DEBUG!",
				AVecUnits[i]->ScxmlName.c_str(), AVecUnits[i]->FileName.c_str(), ASetListPtr->ValueFromIndex[iIndex]);
		}
		else {
			ASetListPtr->Values[AVecUnits[i]->ScxmlName] = AVecUnits[i]->FileName;
		}
	}
	return bRes;
}

// ---------------------------------------------------------------------------
void TStateMachineProject::CollectInheritedUnits(std::vector<TStateMachineEditorUnit*> &AVecUnits) {
	std::vector<TStateMachineEditorUnit*>AVecAllUnits;
	CollectAllEnabledUnits(this, AVecAllUnits);
	for (std::size_t i = 0; i < AVecAllUnits.size(); i++) {
		if (AVecAllUnits[i]->IsInherited) {
			AVecUnits.push_back(AVecAllUnits[i]);
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TStateMachineEditorUnit * __fastcall GlobalAddExistingStateMachineUnit(TLMDProjNode *AParent, const UnicodeString &sFileName,
	bool bCheckIsOpened, const bool bCollectBindings /* = true */ ) {
	if (FormScxmlGui) {
		return dynamic_cast<TStateMachineEditorUnit*>(FormScxmlGui->AddExistingUnit(AParent, sFileName, bCheckIsOpened, bCollectBindings));
	}
	return NULL;
}

TStateMachineEditorUnit *__fastcall GlobalAddNewStateMachineUnit(TLMDProjNode *AParent) {
	if (FormScxmlGui) {
		return dynamic_cast<TStateMachineEditorUnit*>(FormScxmlGui->AddNewUnit(AParent));
	}
	return NULL;
}

TStateMachineEditorUnit *__fastcall GlobalFindAndAddExistingUnit(TLMDProjNode *AParent, const UnicodeString &sFileName,
	bool bCheckIsOpened, const bool bCollectBindings /*= true*/) {
	if (FormScxmlGui) {
		return dynamic_cast<TStateMachineEditorUnit*>(FormScxmlGui->FindAndAddExistingUnit(AParent, sFileName, bCheckIsOpened,
				bCollectBindings));
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall GlobalPostAddOrOpenExistingUnit(TLMDProjNode *AParent, const UnicodeString &sFileName) {
	if (FormScxmlGui && !sFileName.IsEmpty()) {
		const int iSIZE = sFileName.Length() + 1;
		wchar_t *wchBuff = new wchar_t[iSIZE];
		memset(wchBuff, 0, sizeof(wchar_t) * iSIZE);
		wcscpy(wchBuff, sFileName.c_str());

		PostMessage(FormScxmlGui->Handle, WM_SCXML_ADD_OR_OPEN_EXISTING_UNIT, (WPARAM)AParent, (LPARAM)wchBuff);
	}

}

// ---------------------------------------------------------------------------
// ----------------------------------- Static --------------------------------
// ---------------------------------------------------------------------------
void CollectAllEnabledUnits(Lmdideprojmgr::TLMDProjDocument *ARoot, std::vector<TStateMachineEditorUnit*> &AVecUnits) {
	if (ARoot) {
		TStateMachineEditorUnit *AExamUnit = dynamic_cast<TStateMachineEditorUnit*>(ARoot);
		if (AExamUnit && AExamUnit->Enabled) {
			AVecUnits.push_back(AExamUnit);
		}

		for (int i = 0; i < ARoot->DocumentCount; i++) {
			CollectAllEnabledUnits(ARoot->Documents[i], AVecUnits);
		}
	}
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit * FindUnitByFileName(Lmdideprojmgr::TLMDProjDocument *ARoot, const UnicodeString &sFileName) {
	std::vector<TStateMachineEditorUnit*>AVecUnits;
	CollectAllEnabledUnits(ARoot, AVecUnits);
	for (std::size_t i = 0; i < AVecUnits.size(); i++) {
		if (SameText(AVecUnits[i]->FileName, sFileName)) {
			return AVecUnits[i];
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit * FindUnitByFilePath(Lmdideprojmgr::TLMDProjDocument *ARoot, const UnicodeString &sFilePath) {
	std::vector<TStateMachineEditorUnit*>AVecUnits;
	CollectAllEnabledUnits(ARoot, AVecUnits);
	for (std::size_t i = 0; i < AVecUnits.size(); i++) {
		if (SameText(AVecUnits[i]->FilePath, sFilePath)) {
			return AVecUnits[i];
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void GlobalFullExpandProjectView(void) {
	if (FormScxmlGui) {
		FormScxmlGui->ProjectManagerView1->FullExpand();
	}
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit* __fastcall GlobalActiveUnit() {
	return FormScxmlGui->ActiveUnit;
}

// ---------------------------------------------------------------------------

TStateMachineProject * __fastcall GlobalStateMachineProject() {
	return FormScxmlGui->StateMachineProject;
}

// ---------------------------------------------------------------------------
bool __fastcall IsScxmlNameUnique(TStateMachineEditorUnit *AUnit, const UnicodeString &sNewName) {
	if (AUnit && AUnit->StateMachineProject) {
		std::vector<TStateMachineEditorUnit*>AVecUnits;
		CollectAllEnabledUnits(AUnit->StateMachineProject, AVecUnits);
		for (std::size_t i = 0; i < AVecUnits.size(); i++) {
			if (AUnit != AVecUnits[i] && AVecUnits[i]->Enabled) {
				if (SameText(AVecUnits[i]->ScxmlName, sNewName)) {
					return false;
				}
			}
		}
	}
	return true;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall GetUniqueScxmlName(TStateMachineEditorUnit *AUnit) {
	if (AUnit) {

		const UnicodeString sBase = L"ScxmlShape";

		int iCount = 1;
		UnicodeString sNewName = L"";
		do {
			sNewName = sBase + UnicodeString(iCount++);
		}
		while (!IsScxmlNameUnique(AUnit, sNewName));

		return sNewName;
	}
	return L"";
}

// ---------------------------------------------------------------------------
bool __fastcall IsUnitModifiedRecursive(Lmdideprojmgr::TLMDProjNode *ARoot) {
	if (ARoot) {
		if (ARoot->IsModified(false))
			return true;

		TStateMachineEditorUnit * AStateMachineEditorUnit = dynamic_cast<TStateMachineEditorUnit*>(ARoot);
		if (AStateMachineEditorUnit && AStateMachineEditorUnit->StateMachineDockPanel &&
			AStateMachineEditorUnit->StateMachineDockPanel->StateMachineEditor->IsModified())
			return true;

		for (int i = 0; i < ARoot->NodeCount; i++) {
			if (IsUnitModifiedRecursive(ARoot->Nodes[i]))
				return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
void GetUnitVirtualParents(const std::vector<TStateMachineEditorUnit*> &AVecUnits, TStateMachineEditorUnit *AUnit,
	std::set<TStateMachineEditorUnit *> &AVirtualParents) {
	if (!AUnit)
		return;

	if (!AUnit->IsVirtual)
		return;

	if (AVirtualParents.find(AUnit) != AVirtualParents.end())
		return;

	for (std::vector<TStateMachineEditorUnit*>::const_iterator it = AVecUnits.begin(); it != AVecUnits.end(); ++it) {
		if (*it == AUnit) {
			continue;
		}

		if ((*it)->VirtualDependencies->IndexOf(AUnit->FileName) != -1) {
			AVirtualParents.insert(*it);

			GetUnitVirtualParents(AVecUnits, *it, AVirtualParents);
		}
	}
}
