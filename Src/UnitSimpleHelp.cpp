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

#include "UnitSimpleHelp.h"

#include <IOUtils.hpp>
#include "Log4cpp_VCL.hpp"

#include "UnitSettings.h"
#include "VISUAL_GLOBALS.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSimpleHelp *FormSimpleHelp = NULL;

const UnicodeString TFormSimpleHelp::FDockPanelName = L"DockPanelLuaviaHelp";

//---------------------------------------------------------------------------
__fastcall TFormSimpleHelp::TFormSimpleHelp(TComponent* Owner) : TForm(Owner) {
	try {
		const UnicodeString sHelpFile = TPath::Combine(SettingsData->AppPath, "LuaviaHelp.rtf");
		if (!FileExists(sHelpFile))
			throw Exception("Help file [" + sHelpFile + "] is not found!");

		RichHelp->Lines->LoadFromFile(sHelpFile);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	try {
		this->Width = SettingsData->TempRegistry->Values[this->Name + ".Width"].ToIntDef(this->Width);
		this->Height = SettingsData->TempRegistry->Values[this->Name + ".Height"].ToIntDef(this->Height);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormSimpleHelp::FormClose(TObject *Sender, TCloseAction &Action) {
	try {
		SettingsData->TempRegistry->Values[this->Name + ".Width"] = this->Width;
		SettingsData->TempRegistry->Values[this->Name + ".Height"] = this->Height;
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormSimpleHelp::BtnOkClick(TObject *Sender) {
	this->Close();
}

//---------------------------------------------------------------------------
bool __fastcall TFormSimpleHelp::IsDockPanelFound(void) {
	for (int i = 0; i < GlobalMainForm()->ComponentCount; i++) {
		TLMDDockPanel * ADocLuaAviaHelp = dynamic_cast<TLMDDockPanel*>(GlobalMainForm()->Components[i]);
		if (ADocLuaAviaHelp && ADocLuaAviaHelp->Name == FDockPanelName) {

			ADocLuaAviaHelp->PanelVisible = true;

			if (ADocLuaAviaHelp->Zone && ADocLuaAviaHelp->Zone->Parent) {
				ADocLuaAviaHelp->Zone->Parent->ActivePage = ADocLuaAviaHelp->Zone;
			}

			GlobalSendBallonMsg("Panel [Luavia help] is already open in editor!", bfInfo);

			// ВНИМАНИЕ ПАНЕЛЬ С ЭТИМ ПУТЁМ УЖЕ СУЩЕСТВУЕТ!!!
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
void __fastcall TFormSimpleHelp::BtnConvertToDocPanelClick(TObject *Sender) {
	if (!IsDockPanelFound()) {
		TLMDDockPanel *ADocLuaAviaHelp = new TLMDDockPanel(GlobalMainForm());
		ADocLuaAviaHelp->Caption = "Datamodel 'luavia' help";
		ADocLuaAviaHelp->Name = FDockPanelName;

		std::auto_ptr<TMemoryStream>AMemoryStreamPtr(new TMemoryStream());
		AMemoryStreamPtr->WriteComponent(this->RichHelp);
		AMemoryStreamPtr->Position = 0;

		TRichEdit *ANewRich = new TRichEdit(ADocLuaAviaHelp);
		ANewRich->Parent = ADocLuaAviaHelp;
		AMemoryStreamPtr->ReadComponent(ANewRich);

		// не запоминает форматированный текст, поэтому дополнительно преобразовываем
		std::auto_ptr<TStringStream>AStringStreamPtr(new TStringStream());
		this->RichHelp->Lines->SaveToStream(AStringStreamPtr.get());
		AStringStreamPtr->Position = 0;
		ANewRich->Lines->LoadFromStream(AStringStreamPtr.get());

		GlobalDockSite()->DockControl(ADocLuaAviaHelp, GlobalDockPanelRoot()->Zone, alClient);
	}

	this->Close();
}
//---------------------------------------------------------------------------
