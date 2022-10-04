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

#include "UnitDialogRegisterScxmlUnitsAndProjects.h"

#include <memory>
#include <Shlobj.h>
#include "Log4cpp_VCL.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDialogRegisterScxmlFiles *DialogRegisterScxmlFiles;

//---------------------------------------------------------------------------
__fastcall TDialogRegisterScxmlFiles::TDialogRegisterScxmlFiles(TComponent* Owner) : TForm(Owner) {
}

//---------------------------------------------------------------------------
bool __fastcall TDialogRegisterScxmlFiles::DoOpenKey(TRegistry *ARegistry, const UnicodeString &sPath) {
	if (ARegistry->OpenKey(sPath, true))
		return true;

	WLOG_ERROR(L"Can not open [%s%s]", RadioSelectUser->Items->Strings[RadioSelectUser->ItemIndex].c_str(), sPath.c_str());
	return false;
}

//---------------------------------------------------------------------------
bool __fastcall TDialogRegisterScxmlFiles::DoRegisterExtension(TRegistry *ARegistry, const UnicodeString &sExtension,
	const UnicodeString &sDescription, const int iResID) {

	const UnicodeString sKeyClass = L"ScxmlEditor" + sExtension;

	if (DoOpenKey(ARegistry, L"\\Software\\Classes\\" + sExtension)) {
		ARegistry->WriteString(L"", sKeyClass);

		if (DoOpenKey(ARegistry, L"\\Software\\Classes\\" + sKeyClass)) {
			ARegistry->WriteString(L"", sDescription);

			if (DoOpenKey(ARegistry, L"\\Software\\Classes\\" + sKeyClass + L"\\DefaultIcon")) {
				ARegistry->WriteString(L"", L"\"" + Application->ExeName + L"\"," + UnicodeString(iResID));

				if (DoOpenKey(ARegistry, L"\\Software\\Classes\\" + sKeyClass + L"\\shell\\open\\command")) {
					ARegistry->WriteString(L"", Application->ExeName + L" \"%1\"");

					return true;
				}
			}
		}
	}

	return false;
}

//---------------------------------------------------------------------------
bool __fastcall TDialogRegisterScxmlFiles::Execute(void) {
	if (this->ShowModal() == mrOk) {
		std::auto_ptr<TRegistry>ARegistryPtr(new TRegistry());

		ARegistryPtr->RootKey = RadioSelectUser->ItemIndex == 1 ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;

		if (CheckRegisterScxmlProject->Checked) {
			if (!DoRegisterExtension(ARegistryPtr.get(), L".sproj", L"SCXML Editor Project", 1)) {
				return false;
			}
		}
		if (CheckRegisterScxmlUnit->Checked) {
			if (!DoRegisterExtension(ARegistryPtr.get(), L".scxml", L"SCXML Editor Unit", 2)) {
				return false;
			}
		}

		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
