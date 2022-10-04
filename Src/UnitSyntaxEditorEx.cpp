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

#include "UnitSyntaxEditorEx.h"

#include <IOUtils.hpp>
#include <boost/regex.hpp>
#include "Log4cpp_VCL.hpp"
#include <XMLDoc.hpp>

#include "UnitSyntaxUtils.h"
#include "UnitSpawnAndRedirect.h"
#include "UnitSettings.h"
#include "UnitMain.h"
#include "UnitSyntaxEditorForm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDSedView"
#pragma link "UnitSyntaxEditor"
#pragma resource "*.dfm"
TFrameSyntaxEditorEx *FrameSyntaxEditorEx;

#define ECMA_SYNTAX_CHECK_PRE "var __tmp=function() { return("

//---------------------------------------------------------------------------
__fastcall TFrameSyntaxEditorEx::TFrameSyntaxEditorEx(TComponent* Owner) : TFrameSyntaxEditor(Owner) {
	// этот костыль добавляет все меню в тулбар,
	// так как при наследовании они автоматом не добавляются
	this->ToolBar1->Menu = NULL;
	this->ToolBar1->Menu = MainMenu1;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actEditScriptExecute(TObject *Sender) {
	this->EditDocumentScript(Caption);
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actLuaUtilsUpdate(TObject *Sender) {
	actLuaUtils->Visible = EditView->Document && EditView->Document->ActiveSyntaxScheme == L"Lua";
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::OnSelectSyntaxScheme(TObject *Sender) {
	TFrameSyntaxEditor::OnSelectSyntaxScheme(Sender);

	UpdateAllUtilsActions();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::CreateErrorBookmark(int iRow, int iOffset /* = -1 */) {
	try {
		if (!EditView->Document)
			throw Exception("EditView->Document==NULL!");

		actUnfoldAll->Execute();

		/* необходимо указать существующий ряд, иначе генерируется Exception */
		if (iRow >= EditView->Document->LinesCount) {
			iRow = EditView->Document->LinesCount - 1;
		}

		if (iRow < 0) {
			iRow = 0;
		}

		EditView->CursorPosY = iRow;
		if (iOffset != -1) {
			EditView->CursorPosX = iOffset;
		}
		EditView->ScrollToCursor();

		EditView->Gutter->Bars->Items[0]->Visible = true;
		_di_ILMDMark AMark = EditView->Document->Breakpoints->CreateMarkAtLine(iRow);
		if (AMark) {
			EditView->Document->SetBreakpointKind(AMark, mkBreakActive);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditorEx::actLuaUtilsExecute(TObject *Sender) {
	// НЕ УДАЛЯТЬ! Чтобы редактор автоматом не стирал
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actLuaConvertToBinaryChunkAccept(TObject *Sender) {
	try {
		if (!EditView->Document)
			throw Exception("EditView->Document==NULL!");

		EditView->Document->Breakpoints->Clear();

		const UnicodeString sLuacHelp = "Available options are:\n" //
		"  -l       list (use -l for full listing)\n" //
		"  -p       parse only\n" //
		"  -s       strip debug information\n" //
		"  -v       show version information" //
		;

		UnicodeString sLuacParams = "-l -s -v";
		if (InputQuery("EDIT LUAC CMD", sLuacHelp, sLuacParams)) {

			const UnicodeString sTempFileName = TPath::Combine(TPath::GetTempPath(), "luac53.exe.temp.lua");
			EditView->Document->Lines->SaveToFile(sTempFileName);

			sLuacParams = sLuacParams + " -o " + AnsiQuotedStr(actLuaConvertToBinaryChunk->Dialog->FileName, L'"');
			const UnicodeString sCMD = AnsiQuotedStr(TPath::Combine(SettingsData->AppPath, SettingsData->LuautilsLuacExe), L'"')
				+ " " + sLuacParams + " " + AnsiQuotedStr(sTempFileName, L'"');

			if (FormScxmlGui) {
				FormScxmlGui->SwitchToTesterLog(true);
			}
			FConvertLuacSpawnPtr.reset(new TSpawnAndRedirect(sCMD, SettingsData->AppPath));
			FConvertLuacSpawnPtr->OnBeforeLineRead = OnSyntaxCheckBeforeLineRead;
			FConvertLuacSpawnPtr->StartProcess();
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actLuaCheckSyntaxExecute(TObject *Sender) {
	try {
		if (!EditView->Document)
			throw Exception("EditView->Document==NULL!");

		EditView->Document->Breakpoints->Clear();

		TAction * ACheckSyntaxAction = dynamic_cast<TAction*>(Sender);
		if (!ACheckSyntaxAction)
			throw Exception("Can not cast to <TAction>!");

		const UnicodeString sLuacParams = SettingsData->LuautilsLuacSyntaxCheckParamsCMD;

		const UnicodeString sTempFileName = TPath::Combine(TPath::GetTempPath(), "luac53.exe.temp.lua");
		if (ACheckSyntaxAction->Tag == 0) {
			EditView->Document->Lines->SaveToFile(sTempFileName);
		}
		else {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->Text = "return " + EditView->Document->Lines->Text;
			AStringListPtr->SaveToFile(sTempFileName);
		}

		const UnicodeString sCMD = AnsiQuotedStr(TPath::Combine(SettingsData->AppPath, SettingsData->LuautilsLuacExe), L'"')
			+ " " + sLuacParams + " " + AnsiQuotedStr(sTempFileName, L'"');

		if (FormScxmlGui) {
			FormScxmlGui->SwitchToTesterLog(true);
		}
		FConvertLuacSpawnPtr.reset(new TSpawnAndRedirect(sCMD, SettingsData->AppPath));
		FConvertLuacSpawnPtr->OnBeforeLineRead = OnSyntaxCheckBeforeLineRead;
		FConvertLuacSpawnPtr->StartProcess();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actLuaConvertToCPPBinAccept(TObject *Sender) {
	try {
		if (!EditView->Document)
			throw Exception("EditView->Document==NULL!");

		EditView->Document->Breakpoints->Clear();

		const UnicodeString sLuacHelp = "Available options are:\n" //
		"  -l       list (use -l for full listing)\n" //
		"  -p       parse only\n" //
		"  -s       strip debug information\n" //
		"  -v       show version information" //
		;

		UnicodeString sLuacParams = "-l -s -v";
		if (InputQuery("EDIT LUAC CMD", sLuacHelp, sLuacParams)) {

			const UnicodeString sLuacExe = ExtractFileName(SettingsData->LuautilsLuacExe);

			const UnicodeString sTempFileName = TPath::Combine(TPath::GetTempPath(), sLuacExe + ".temp.lua");
			EditView->Document->Lines->SaveToFile(sTempFileName);

			const UnicodeString sLuac = AnsiQuotedStr(actLuaConvertToCPPBin->Dialog->FileName, L'"');

			sLuacParams = sLuacParams + " -o " + sLuac;
			// convert to LUAC
			{
				const UnicodeString sCMD = AnsiQuotedStr(TPath::Combine(SettingsData->AppPath, SettingsData->LuautilsLuacExe), L'"')
					+ " " + sLuacParams + " " + AnsiQuotedStr(sTempFileName, L'"');
				if (FormScxmlGui) {
					FormScxmlGui->SwitchToTesterLog(true);
				}
				FConvertLuacSpawnPtr.reset(new TSpawnAndRedirect(sCMD, SettingsData->AppPath, this->Handle, WM_SCXML_LUAC_CONVERTED));
				FConvertLuacSpawnPtr->OnBeforeLineRead = OnSyntaxCheckBeforeLineRead;
				FConvertLuacSpawnPtr->StartProcess();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::OnLuacConverted(TMessage &msg) {
	try {
		if (!EditView->Document)
			throw Exception("EditView->Document==NULL!");

		const UnicodeString sLuac = AnsiQuotedStr(actLuaConvertToCPPBin->Dialog->FileName, L'"');

		// convert to CPP BIN
		const UnicodeString sCMD = AnsiQuotedStr(TPath::Combine(SettingsData->AppPath, SettingsData->LuautilsBinaryConverter), L'"')
			+ " " + sLuac;

		if (FormScxmlGui) {
			FormScxmlGui->SwitchToTesterLog(true);
		}
		// ТЕКУЩАЯ ДИРЕКТОРИЯ ДОЛЖНА БЫТЬ ИМЕННО ТАМ, ГДЕ ФАЙЛ !!!
		FConvertLuacSpawnPtr.reset(new TSpawnAndRedirect(sCMD, ExtractFileDir(actLuaConvertToCPPBin->Dialog->FileName)));
		FConvertLuacSpawnPtr->StartProcess();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::OnSyntaxCheckBeforeLineRead(UnicodeString &sText) {
	try {
		bool bIsErr = false;

		boost::wregex e(L"^.+\\.temp\\.lua:(\\d*):.*$");
		boost::wsmatch what;

		std::wstring wstr = sText.c_str();
		if (boost::regex_match(wstr, what, e)) {
			if (what.size() == 2) {

				sText = StringReplace(sText, SettingsData->AppPath, L"", TReplaceFlags());
				sText = StringReplace(sText, TPath::GetTempPath(), L"", TReplaceFlags());

				const int iLine = StrToInt(std::wstring(what[1]).c_str());

				CreateErrorBookmark(iLine - 1);

				bIsErr = true;
			}
		}
		sText = (bIsErr ? L"ERROR> " : L"DEBUG> ") + /*Time().FormatString("hh:mm:ss.zzz") + " " +*/ sText;
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	catch(std::exception & e) {
		WLOG_ERROR(L"%s", e.what());
	}

}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actBreakDeleteAllExecute(TObject *Sender) {
	if (EditView->Document) {
		EditView->Document->Breakpoints->Clear();
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actXMLCheckSyntaxExecute(TObject *Sender) {
	if (IsXMLSyntaxValid()) {
		WLOG_INFO(L"XML text is valid!");
	}
}

//---------------------------------------------------------------------------
bool TFrameSyntaxEditorEx::IsXMLSyntaxValid(void) {
	EditView->Document->Breakpoints->Clear();

	try {
		if (EditView->Document) {

			std::auto_ptr<TXMLDocument>AXMLDocumentPtr(new TXMLDocument(this));
			AXMLDocumentPtr->XML->Text = EditView->Document->Lines->Text;
			AXMLDocumentPtr->Active = true;

			return true;
		}
	}
	catch(EDOMParseError * E) {
		WLOG_ERROR(L"XML Syntax Check> %s", E->Message.c_str());

		CreateErrorBookmark(E->Line - 1, E->LinePos + 1);
	}
	catch(Exception * E) {
		WLOG_ERROR(L"XML Syntax Check> %s", E->Message.c_str());
	}

	return false;
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actXMLUtilsExecute(TObject *Sender) {
	// чтобы не удалял редактор
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditorEx::actXMLUtilsUpdate(TObject *Sender) {
	actXMLUtils->Visible = EditView->Document && SameText(EditView->Document->ActiveSyntaxScheme, L"XML");
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::UpdateAllUtilsActions() {
	actXMLUtils->Update();
	actLuaUtils->Update();
	actEcmaUtils->Update();
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actXMLFormatExecute(TObject *Sender) {
	EditView->Document->Breakpoints->Clear();

	try {
		if (EditView->Document) {

			std::auto_ptr<TXMLDocument>AXMLDocumentPtr(new TXMLDocument(this));
			AXMLDocumentPtr->XML->Text = EditView->Document->Lines->Text;
			AXMLDocumentPtr->Active = true;

			EditView->Document->Lines->Text = FormatXMLData(EditView->Document->Lines->Text);

			WLOG_INFO(L"Format completed!");
		}
	}
	catch(EDOMParseError * E) {
		WLOG_ERROR(L"XML Format Check> %s", E->Message.c_str());

	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::EditDocumentScript(const UnicodeString &sEditCaption) {
	// документ может быть не назначен
	if (!this->EditView->Document)
		return;

	int iStartLine = 0, Col = 0;
	this->EditView->ScrollPosToPhysical(this->EditView->SelectionStart, iStartLine, Col);

	if (this->EditView->Document->GetParser()->IsFoldStart(iStartLine) && this->EditView->Document->Lines->Strings[iStartLine].Pos
		(L"<script>")) {
		const int iEndLine = this->EditView->Document->GetParser()->GetFoldEnd(iStartLine);

		const int iStartFoldBlock = this->EditView->Document->LineSegments[iStartLine].Start;
		const int iEndFoldBlock = this->EditView->Document->LineSegments[iEndLine].Start +this->EditView->Document->LineSegments[iEndLine]
			.Count;

		std::auto_ptr<TStringList>AStringList(new TStringList());
		AStringList->Text = this->EditView->Document->Lines->Text.SubString(iStartFoldBlock, iEndFoldBlock - iStartFoldBlock + 1);

		this->EditView->Document->BeginUpdate();
		try {
			_di_ILMDXmlDocument ADoc = LMDLoadXmlDocumentFromXML(AStringList->Text);
			if (ADoc && ADoc->DocumentElement) {

				std::auto_ptr<TStringList>ATempList(new TStringList());
				ATempList->Text = ADoc->DocumentElement->Text.TrimLeft();

				if (!SettingsData->EditStringsWithExternEditor(ATempList.get(), sEditCaption)) {
					std::auto_ptr<TFormSyntaxEditorForm>AFormSyntaxEditor(new TFormSyntaxEditorForm(this->Owner,
							SettingsData->ActiveSyntaxScheme, ATempList->Text));
					AFormSyntaxEditor->CheckSwitchToDocPanel->Visible = false;
					AFormSyntaxEditor->Caption = sEditCaption;

					if (AFormSyntaxEditor->ShowModal() == mrOk) {
						ATempList->Text = AFormSyntaxEditor->EditDoc1->Lines->Text;
					}
				}

				if (ATempList->Text != ADoc->DocumentElement->Text.TrimLeft()) {
					// если пустой текст, то вообще script убираем
					if (ATempList->Text.Trim().IsEmpty()) {
						this->EditView->Document->Delete(iStartFoldBlock, iEndFoldBlock - iStartFoldBlock);
					}
					else {
						// добавляем перенос на новую строку, так как иначе сбивается CodeFold
						ADoc->DocumentElement->Text = L"\r\n" + ATempList->Text;

						this->EditView->Document->Replace(iStartFoldBlock, iEndFoldBlock - iStartFoldBlock,
							ADoc->DocumentElement->XML.Trim());
					}
				}
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
		this->EditView->Document->EndUpdate();
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actEcmaUtilsExecute(TObject *Sender) {
	// не удалять
}
//---------------------------------------------------------------------------

void __fastcall TFrameSyntaxEditorEx::actEcmaUtilsUpdate(TObject *Sender) {
	actEcmaUtils->Visible = EditView->Document && SameText(EditView->Document->ActiveSyntaxScheme, L"EcmaScript");
}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::OnEcmaSyntaxCheckBeforeLineRead(UnicodeString &sText) {
	try {
		bool bIsErr = false;

		const int iPosError = sText.UpperCase().Pos("ERROR");
		const int iSemicolon = sText.Pos(":");
		if (iPosError) {

			if (iSemicolon && iSemicolon < iPosError) {
				int iLine = -1;
				if (TryStrToInt(sText.SubString(1, iSemicolon - 1), iLine)) {
					CreateErrorBookmark(iLine - 1);
				}
			}

			bIsErr = true;
		}

		sText = (bIsErr ? L"ERROR> " : L"INFO> ") + /*Time().FormatString("hh:mm:ss.zzz") + " " +*/ sText;
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
	catch(std::exception & e) {
		WLOG_ERROR(L"%s", e.what());
	}

}

//---------------------------------------------------------------------------
void __fastcall TFrameSyntaxEditorEx::actEcmaCheckSyntaxExecute(TObject *Sender) {
	try {
		if (!EditView->Document)
			throw Exception("EditView->Document==NULL!");

		EditView->Document->Breakpoints->Clear();

		TAction * ACheckSyntaxAction = dynamic_cast<TAction*>(Sender);
		if (!ACheckSyntaxAction)
			throw Exception("Can not cast to <TAction>!");

		const UnicodeString sTempFileName = TPath::Combine(TPath::GetTempPath(),
			"jsc." + (ACheckSyntaxAction->Tag ? UnicodeString("expr") : UnicodeString("script")) + ".js");
		if (ACheckSyntaxAction->Tag == 0) {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->Text = "function __tmp(){" + EditView->Document->Lines->Text.TrimRight() + "};";
			AStringListPtr->SaveToFile(sTempFileName);
		}
		else {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->Text = ECMA_SYNTAX_CHECK_PRE + EditView->Document->Lines->Text.TrimRight() + "); };";
			AStringListPtr->SaveToFile(sTempFileName);
		}

		const UnicodeString sExeName = TPath::GetFullPath(TPath::Combine(SettingsData->AppPath, USCXML_TESTER_EXE));
		const UnicodeString sExeDir = ExtractFileDir(sExeName);
		const UnicodeString sExeParams = " -checkecmasyntax";

		const UnicodeString sCMD = AnsiQuotedStr(sExeName, L'"') + sExeParams + " " + AnsiQuotedStr(sTempFileName, L'"');

		if (FormScxmlGui) {
			FormScxmlGui->SwitchToTesterLog(true);
		}
		FConvertEcmaSpawnPtr.reset(new TSpawnAndRedirect(sCMD, sExeDir));
		FConvertEcmaSpawnPtr->OnBeforeLineRead = OnEcmaSyntaxCheckBeforeLineRead;
		FConvertEcmaSpawnPtr->StartProcess();
	}
	catch(Exception * E) {
		WLOG_ERROR(L"Ecma Syntax> %s", E->Message.c_str());
	}
}
//---------------------------------------------------------------------------
