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

#include <memory>

#include "UnitXMLSerializeUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Xmlserialize {

	const UnicodeString PrintXMLNodeString(_di_IXMLNode ANode) {
		return "--XML-- [" + ANode->XML + "]";
	}

	//---------------------------------------------------------------------------
	const UnicodeString GetPathNameFromComponent(TComponent *AComponent) {
		if (!AComponent) {
			return L"";
		}

		std::auto_ptr<TStringList>AStringList(new TStringList());
		AStringList->Delimiter = L'.';
		AStringList->StrictDelimiter = true;
		AStringList->Insert(0, AComponent->Name);

		TComponent *AOwner = AComponent->Owner;
		while (AOwner) {
			if (!AOwner->Name.IsEmpty()) {
				AStringList->Insert(0, AOwner->Name);
				AOwner = AOwner->Owner;
			}
			else {
				break;
			}
		}
		return AStringList->DelimitedText;
	}

	//---------------------------------------------------------------------------
	TComponent *GetComponentFromPathName(const UnicodeString &sPathName) {
		std::auto_ptr<TStringList>AStringList(new TStringList());
		AStringList->Delimiter = L'.';
		AStringList->StrictDelimiter = true;
		AStringList->DelimitedText = sPathName;
		TComponent *AOwner = Application;
		TComponent *AComponent = 0;
		for (int i = 0; i < AStringList->Count; i++) {
			if (AOwner) {
				AComponent = AOwner->FindComponent(AStringList->Strings[i]);
				AOwner = AComponent;
			}
			else {
				return 0;
			}
		}
		return AComponent;
	}

	//---------------------------------------------------------------------------
	const TFormatSettings GetSerializeFormatSettings() {
		TFormatSettings AFormatSettings;
		AFormatSettings.DecimalSeparator = L'.';
		AFormatSettings.DateSeparator = L'.';
		AFormatSettings.TimeSeparator = L':';
		AFormatSettings.ShortDateFormat = L"dd.mm.yyyy";
		AFormatSettings.LongDateFormat = L"dd.mm.yyyy";
		AFormatSettings.ShortTimeFormat = L"hh:mm:ss";
		AFormatSettings.LongTimeFormat = L"hh:mm:ss.zzz";

		return AFormatSettings;
	}

};
