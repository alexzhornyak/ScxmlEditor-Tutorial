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

#ifndef UnitXMLSerializeUtilsH
#define UnitXMLSerializeUtilsH
//---------------------------------------------------------------------------

#include <XMLDoc.hpp>

#define XML_NODE_OBJECT_NAME			L"object"
#define XML_COLLECTION_ITEM_NAME 		L"item"

namespace Xmlserialize {

	typedef void(__fastcall*ON_ERROR_GET_FUNC)(const UnicodeString&);

    // если функция возвращает false, то разбор свойства продолжится штатными методами
	typedef bool __fastcall (__closure * ON_CUSTOM_GET_PROP)(TObject *Sender, const UnicodeString &sPropName, _di_IXMLNode APropertyNode);
	#define ON_SERIALIZE_CUSTOM_GET_PROP		"OnSerializeCustomGetProp"

	// функция на входе разбора свойств у объекта, 'false' продолжает разбор штатными методами
	typedef bool __fastcall (__closure * ON_CUSTOM_SET_OBJECT)(TObject *Sender, _di_IXMLNode AParentNode);
	#define ON_SERIALIZE_CUSTOM_SET_OBJECT		"OnSerializeCustomSetObject"

	// разбор конкретного свойства у объекта, 'false' продолжает разбор штатными методами
	typedef bool __fastcall (__closure * ON_CUSTOM_SET_PROP)(TObject *Sender, const UnicodeString &sPropName, TPropInfo *APropInfo, _di_IXMLNode AParentNode);
	#define ON_SERIALIZE_CUSTOM_SET_PROP		"OnSerializeCustomSetProp"

	const TFormatSettings GetSerializeFormatSettings();

	const UnicodeString GetPathNameFromComponent(TComponent * AComponent);

	TComponent *GetComponentFromPathName(const UnicodeString & sPathName);

	const UnicodeString PrintXMLNodeString(_di_IXMLNode ANode);

};

#endif
