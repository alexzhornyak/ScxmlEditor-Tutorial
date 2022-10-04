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

#include "UnitSettingsBase.h"

#include <memory>

#include "XMLSerialize/UnitXMLSerialize.h"

#include "Log4cpp_VCL.hpp"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void __fastcall SendLogError(const UnicodeString &sText) {
	WLOG_ERROR(L"%s", sText.c_str());
}

// ---------------------------------------------------------------------------
__fastcall TSettingsBase::TSettingsBase(TComponent* AOwner) : TComponent(AOwner) {

}

// ---------------------------------------------------------------------------
TComponent *__fastcall TSettingsBase::DoCreateDefaultInstance() {
	return new TSettingsBase(this->Owner);
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsBase::LoadFromFile(const UnicodeString &sFileName) {
	try {
		std::auto_ptr<TComponent>AComponentPtr(DoCreateDefaultInstance());

		Xmlserialize::LoadComponentFromXML(AComponentPtr.get(), sFileName, SendLogError);
		this->Assign(AComponentPtr.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsBase::ResetToDefault(void) {
	try {
		std::auto_ptr<TComponent>AComponentPtr(DoCreateDefaultInstance());

		this->Assign(AComponentPtr.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsBase::SaveToFile(const UnicodeString &sFileName) {
	try {
		Xmlserialize::SaveComponentToXML(this, sFileName, L"windows-1251", SendLogError);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsBase::LoadFromStrings(TStrings *AStrings) {
	try {
		std::auto_ptr<TComponent>AComponentPtr(DoCreateDefaultInstance());

		std::auto_ptr<TStringStream>AStringStreamPtr(new TStringStream(AStrings->Text));
		AStringStreamPtr->Position = 0;

		Xmlserialize::LoadComponentFromStreamXML(AComponentPtr.get(), AStringStreamPtr.get(), Xmlintf::TXMLEncodingType(0), SendLogError);
		this->Assign(AComponentPtr.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsBase::SaveToStrings(TStrings *AStrings) {
	try {
		std::auto_ptr<TMemoryStream>AMemoryStreamPtr(new TMemoryStream());
		Xmlserialize::SaveComponentToStreamXML(this, L"", AMemoryStreamPtr.get(), SendLogError);
		AMemoryStreamPtr->Position = 0;
		AStrings->LoadFromStream(AMemoryStreamPtr.get());
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsBase::ImportFromFile(const UnicodeString &sFileName) {
	try {
		Xmlserialize::LoadComponentFromXML(this, sFileName, SendLogError);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
std::pair<TObject*, UnicodeString>GetPropertyPair(TObject *AInstance, const UnicodeString &sPropertyPath) {
	if (!AInstance)
		throw Exception("AInstance==NULL! Path [" + sPropertyPath + "]");

	UnicodeString sPropPath = sPropertyPath.Trim();

	std::auto_ptr<TStringList>AObjectsList(new TStringList());
	AObjectsList->Delimiter = L'.';
	AObjectsList->StrictDelimiter = true;
	AObjectsList->DelimitedText = sPropPath;

	TObject *APersistentObject = AInstance;

	// значит свойство составное
	if (AObjectsList->Count > 1) {
		for (int i = 0; i < AObjectsList->Count - 1; i++) {
			if (!Typinfo::PropIsType(APersistentObject, AObjectsList->Strings[i], Typinfo::tkClass))
				throw Exception(UnicodeString().sprintf(L"[%s] is not class property of [%s]", AObjectsList->Strings[i].c_str(),
					APersistentObject->ClassName().c_str()));

			if (!IsPublishedProp(APersistentObject, AObjectsList->Strings[i]))
				throw Exception(UnicodeString().sprintf(L"[%s] is not published property of [%s]", AObjectsList->Strings[i].c_str(),
					AInstance->ClassName().c_str()));

			APersistentObject = GetObjectProp(APersistentObject, AObjectsList->Strings[i]);
			if (!APersistentObject)
				throw Exception(UnicodeString().sprintf(L"Object with PropName [%s] is NULL!", AObjectsList->Strings[i].c_str()));
		}
		sPropPath = AObjectsList->Strings[AObjectsList->Count - 1];
	}

	if (!IsPublishedProp(APersistentObject, sPropPath))
		throw Exception(UnicodeString().sprintf(L"[%s] is not published property of [%s]", sPropPath.c_str(),
			APersistentObject->ClassName().c_str()));

	return std::make_pair(APersistentObject, sPropPath);
}

// ---------------------------------------------------------------------------
void __fastcall TSettingsBase::ResetPropertyToDefault(const UnicodeString &sPropertyPath) {
	try {
		std::auto_ptr<TComponent>AComponentPtr(DoCreateDefaultInstance());

		std::pair<TObject*, UnicodeString>ADefaultObject = GetPropertyPair(AComponentPtr.get(), sPropertyPath);
		std::pair<TObject*, UnicodeString>ACurrentObject = GetPropertyPair(this, sPropertyPath);

		TPropInfo *APropInfo = GetPropInfo(ACurrentObject.first, ACurrentObject.second);
		if (!APropInfo || !APropInfo->SetProc)
			throw Exception("Read-only property [" + sPropertyPath + "]!");

		const Typinfo::TTypeKind ATypeKind = (*(APropInfo->PropType))->Kind;
		if (ATypeKind == Typinfo::tkClass) {
			TPersistent *ADefaultClassValue = dynamic_cast<TPersistent*>(GetObjectProp(ADefaultObject.first, ADefaultObject.second));
			TPersistent *ACurrentClassValue = dynamic_cast<TPersistent*>(GetObjectProp(ACurrentObject.first, ACurrentObject.second));
			if (ADefaultClassValue && ACurrentClassValue) {
				ACurrentClassValue->Assign(ADefaultClassValue);
			}
		}
		else {
			const System::Variant ADefault = GetPropValue(ADefaultObject.first, ADefaultObject.second);
			SetPropValue(ACurrentObject.first, ACurrentObject.second, ADefault);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}
// ---------------------------------------------------------------------------
