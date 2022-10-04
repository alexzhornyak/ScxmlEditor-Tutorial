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

#include "UnitCaptureOutputRegExps.h"

#include <memory>

#include <boost/regex.hpp>

#include "Log4cpp_VCL.hpp"

#include "UnitDialogRegExp.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
// ----------------------- TCaptureOutputRegexItem --------------------------------
// ---------------------------------------------------------------------------
__fastcall TCaptureOutputRegexItem::TCaptureOutputRegexItem(TCollection* Collection) : TCollectionItem(Collection) {
	FRegExp = L"";
	FGroupMachine = -1;
	FGroupIdentifier = -1;
	FCaptureMsgType = smttUnknown;
}

// ---------------------------------------------------------------------------
__fastcall TCaptureOutputRegexItem::~TCaptureOutputRegexItem() {

}

// ---------------------------------------------------------------------------
void __fastcall TCaptureOutputRegexItem::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();

		if (sPropName == L"RegExp") {
			AEditorClass = __classid(TRegExpStringDialogPropEditor);
			return;
		}
	}
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TCaptureOutputRegexItem::GetDisplayName(void) {
	return UnicodeString().sprintf(L"Item %d {%s:%d:%d}", Index, FRegExp.c_str(), FGroupMachine, FGroupIdentifier);
}

// ---------------------------------------------------------------------------
void __fastcall TCaptureOutputRegexItem::Assign(TPersistent* Source) {
	if (Source) {
		TCaptureOutputRegexItem * ACaptureOutputRegexItem = dynamic_cast<TCaptureOutputRegexItem*>(Source);
		if (ACaptureOutputRegexItem) {
			FRegExp = ACaptureOutputRegexItem->FRegExp;
			FGroupMachine = ACaptureOutputRegexItem->FGroupMachine;
			FGroupIdentifier = ACaptureOutputRegexItem->FGroupIdentifier;
			FCaptureMsgType = ACaptureOutputRegexItem->FCaptureMsgType;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
bool __fastcall TCaptureOutputRegexItem::Parse(const UnicodeString &sText, UnicodeString &sMachineName, UnicodeString &sIdentifier) {
	if (!sText.IsEmpty() && this->Enabled) {
		try {
			boost::wregex e(FRegExp.c_str());
			boost::wsmatch what;

			std::wstring wstr = sText.c_str();
			if (boost::regex_match(wstr, what, e)) {
				for (std::size_t i = 0; i < what.size(); i++) {
					const int iIndex = static_cast<int>(i);
					if (this->FGroupIdentifier == iIndex) {
						sIdentifier = std::wstring(what[i]).c_str();
					}
					else if (this->FGroupMachine == iIndex) {
						sMachineName = std::wstring(what[i]).c_str();
					}
				}

				return true;
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
		catch(std::exception & e) {
			LOG_ERROR("%s> %s", __FUNCTION__, e.what());
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
// --------------------------- COLLECTION ------------------------------------
// ----------------------- TCaptureOutputRegexItems -------------------------------
// ---------------------------------------------------------------------------
__fastcall TCaptureOutputRegexItems::TCaptureOutputRegexItems(TPersistent *AOwner) : TOwnedCollection(AOwner,
	__classid(TCaptureOutputRegexItem)), FEnabled(false) {

}

// ---------------------------------------------------------------------------
bool __fastcall TCaptureOutputRegexItems::GetEnabled(void) {
	if (FEnabled) {
		for (int i = 0; i < this->Count; i++) {
			if (this->Items[i]->Enabled) {
				return true;
			}
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TCaptureOutputRegexItems::GetDisplayName(void) {
	std::auto_ptr<TStringList>AStringList(new TStringList());
	for (int i = 0; i < this->Count; i++) {
		if (this->Items[i]->Enabled) {
			AStringList->Add("[" + UnicodeString(i) + "]"+this->Items[i]->RegExp);
		}
	}
	return(this->Enabled ? "Enabled>" : "Disabled>") + AStringList->CommaText;
}

// ---------------------------------------------------------------------------
TCaptureOutputRegexItem* __fastcall TCaptureOutputRegexItems::Add(void) {
	return dynamic_cast<TCaptureOutputRegexItem*>(TOwnedCollection::Add());
}

// ---------------------------------------------------------------------------
TCaptureOutputRegexItem* __fastcall TCaptureOutputRegexItems::Insert(int Index) {
	return dynamic_cast<TCaptureOutputRegexItem*>(TOwnedCollection::Insert(Index));
}

// ---------------------------------------------------------------------------
TCaptureOutputRegexItem* __fastcall TCaptureOutputRegexItems::GetItem(int Index) {
	return dynamic_cast<TCaptureOutputRegexItem*>(TOwnedCollection::GetItem(Index));
}

// ---------------------------------------------------------------------------
void __fastcall TCaptureOutputRegexItems::SetItem(int Index, TCaptureOutputRegexItem* Value) {
	TOwnedCollection::SetItem(Index, Value);
}

// ---------------------------------------------------------------------------
void __fastcall TCaptureOutputRegexItems::Assign(TPersistent* Source) {
	if (Source) {
		TCaptureOutputRegexItems * ASeriesCompareItems = dynamic_cast<TCaptureOutputRegexItems*>(Source);
		if (ASeriesCompareItems) {
			TOwnedCollection::Assign(Source);

			// здесь применяем настройки непосредственно этого класса
			// ПРИМЕР:
			FEnabled = ASeriesCompareItems->FEnabled;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");

}
