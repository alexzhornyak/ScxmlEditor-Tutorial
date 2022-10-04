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

#include "SaveSimplePersistent.h"

#include <memory>

#include <XMLDoc.hpp>
#include "Log4cpp_VCL.hpp"
#include "UnitPropInspectorExtensions.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

namespace MUtils {

	// ---------------------------------------------------------------------------
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

	// ---------------------------------------------------------------------------
	void DoStringToPersistent(TObject *ARootObject, const UnicodeString &sText, ON_ERROR_GET_FUNC AOnError) {
		if (!ARootObject)
			return;

		std::auto_ptr<TXMLDocument>AXMLDocumentPtr(new TXMLDocument(Application));
		AXMLDocumentPtr->XML->Text = sText;
		AXMLDocumentPtr->Active = true;

		if (AXMLDocumentPtr->DocumentElement) {
			for (int i = 0; i < AXMLDocumentPtr->DocumentElement->ChildNodes->Count; i++) {
				_di_IXMLNode ANode = AXMLDocumentPtr->DocumentElement->ChildNodes->Nodes[i];

				if (!ANode->HasAttribute("Name"))
					throw Exception("Node require attr 'Name'. Full context:" + ANode->XML);

				UnicodeString sPropName = ANode->GetAttribute("Name");

				std::auto_ptr<TStringList>AObjectsList(new TStringList());
				AObjectsList->Delimiter = L'.';
				AObjectsList->StrictDelimiter = true;
				AObjectsList->DelimitedText = sPropName;

				TObject *APersistentObject = ARootObject;

				if (AObjectsList->Count > 1) {
					for (int i = 0; i < AObjectsList->Count - 1; i++) {
						APersistentObject = GetObjectProp(APersistentObject, AObjectsList->Strings[i]);
					}
					if (!APersistentObject)
						continue;
					sPropName = AObjectsList->Strings[AObjectsList->Count - 1];
				}

				UnicodeString sValue = L"";
				if (ANode->IsTextElement) {
					sValue = ANode->NodeValue;
				}

				if (IsPublishedProp(APersistentObject, sPropName)) {
					TPropInfo *APropInfo = GetPropInfo(APersistentObject, sPropName);
					if (APropInfo->SetProc) {

						try {
							TTypeInfo *ATypeInfo = *(APropInfo->PropType);
							const UnicodeString sTypeKindName = AnsiString(ATypeInfo->Name);

							if (sTypeKindName == "TDate") {
								const TDate ADate = StrToDate(sValue, GetSerializeFormatSettings());
								SetPropValue(APersistentObject, sPropName, ADate);
							}
							else if (sTypeKindName == "TTime") {
								const TTime ATime = StrToTime(sValue, GetSerializeFormatSettings());
								SetPropValue(APersistentObject, sPropName, ATime);
							}
							else if (sTypeKindName == "TDateTime") {
								const TDateTime ADateTime = StrToDate(sValue, GetSerializeFormatSettings());
								SetPropValue(APersistentObject, sPropName, ADateTime);
							}
							else if (sTypeKindName == "TCursor") {
								const TCursor ACursor = StringToCursor(sValue);
								SetPropValue(APersistentObject, sPropName, ACursor);
							}
							else if (sTypeKindName == "TColor") {
								const TColor AColor = StringToColor(sValue);
								SetPropValue(APersistentObject, sPropName, AColor);
							}
							else {

								switch(ATypeInfo->Kind) {
								case tkClass: {
										throw Exception("Class properties are not supported!");
									}break;
								default: {
										SetPropValue(APersistentObject, sPropName, sValue);
									}
								}
							}
						}
						catch(Exception * E) {
							if (AOnError) {
								AOnError(__FUNCTION__ "> " + E->Message);
								// AOnError(PrintXMLNodeString(APropertyNode));
							}
						}

					}
					else {
						if (AOnError) {
							AOnError(__FUNCTION__ "> Property has no SetProc <" + sPropName + ">");
							// AOnError(PrintXMLNodeString(APropertyNode));
						}
					}
				}
				// не публичное свойство
				else {
					if (AOnError) {
						AOnError(__FUNCTION__ "> Unknown property <" + sPropName + ">");
						// AOnError(PrintXMLNodeString(APropertyNode));
					}
				}
			}
		}

	}

	// ---------------------------------------------------------------------------
	void StringToPersistent(TPersistent *APersistent, const UnicodeString &sText, ON_ERROR_GET_FUNC AOnError) {
		if (!APersistent)
			throw Exception("Persistent object can not be NULL!");
		DoStringToPersistent(APersistent, sText, AOnError);
	}

	void DoPersistentToString(TObject * ARootObject, const UnicodeString & sPropPath, _di_IXMLNode ANode, bool bSaveNonStored) {
		if (!ARootObject) {
			return;
		}
		if (!ANode)
			throw Exception("ANode==NULL!!! Path:" + sPropPath);

		TPropList APropList;

		TTypeKinds TypeKinds = TTypeKinds() << tkInteger << tkChar << tkEnumeration << tkFloat << tkString << tkSet << tkClass <<
			tkWChar << tkLString << tkWString << tkVariant << tkArray << tkRecord << tkInterface << tkInt64 << tkDynArray << tkUString <<
			tkClassRef << tkPointer << tkProcedure;

		const int iRes = GetPropList(PTypeInfo(ARootObject->ClassInfo()), TypeKinds, (TPropList*)(&APropList), false);

		for (int i = 0; i < iRes; i++) {
			TTypeInfo *ATypeInfo = *APropList[i]->PropType;

			const UnicodeString sPropName = AnsiString(APropList[i]->Name);

			std::auto_ptr<TStringList>APathList(new TStringList());
			APathList->StrictDelimiter = true;
			APathList->Delimiter = L'.';
			APathList->DelimitedText = sPropPath;
			APathList->Add(sPropName);

			const UnicodeString sOutPropName = APathList->DelimitedText;
			const UnicodeString sTypeKindName = AnsiString(ATypeInfo->Name);

			if (APropList[i]->SetProc && (bSaveNonStored || IsStoredProp(ARootObject, sPropName))) {

				if (sTypeKindName == "TDate") {
					const TDate ADate = GetPropValue(ARootObject, sPropName);
					_di_IXMLNode AItem = ANode->AddChild("item");
					AItem->SetAttribute("Name", sOutPropName);
					AItem->SetText(DateToStr(ADate, GetSerializeFormatSettings()));
				}
				else if (sTypeKindName == "TTime") {
					const TTime ADate = GetPropValue(ARootObject, sPropName);
					_di_IXMLNode AItem = ANode->AddChild("item");
					AItem->SetAttribute("Name", sOutPropName);
					AItem->SetText(TimeToStr(ADate, GetSerializeFormatSettings()));
				}
				else if (sTypeKindName == "TDateTime") {
					const TDateTime ADate = GetPropValue(ARootObject, sPropName);
					_di_IXMLNode AItem = ANode->AddChild("item");
					AItem->SetAttribute("Name", sOutPropName);
					AItem->SetText(DateTimeToStr(ADate, GetSerializeFormatSettings()));
				}
				else if (sTypeKindName == "TCursor") {
					const int ACursor = GetPropValue(ARootObject, sPropName);
					_di_IXMLNode AItem = ANode->AddChild("item");
					AItem->SetAttribute("Name", sOutPropName);
					AItem->SetText(CursorToString(TCursor(ACursor)));
				}
				else if (sTypeKindName == "TColor") {
					const int AColor = GetPropValue(ARootObject, sPropName);
					_di_IXMLNode AItem = ANode->AddChild("item");
					AItem->SetAttribute("Name", sOutPropName);
					AItem->SetText(ColorToString(TColor(AColor)));
				}
				else {

					switch(ATypeInfo->Kind) {
					case tkClass: {
							TObject *AGetObject = GetObjectProp(ARootObject, sPropName);
							if (AGetObject) {
								if (AGetObject->InheritsFrom(__classid(TCollection))) {
									// коллекции не поддерживаются
								}
								else if (AGetObject->InheritsFrom(__classid(TStrings))) {
									TStrings *AStrings = dynamic_cast<TStrings*>(AGetObject);
									if (AStrings) {
										_di_IXMLNode AItem = ANode->AddChild("item");
										AItem->SetAttribute("Name", sOutPropName);
										AItem->SetText(AStrings->CommaText);
									}
								}
								else if (AGetObject->InheritsFrom(__classid(TComponent))) {
									TComponent *AComponent = dynamic_cast<TComponent*>(AGetObject);

									// как мы сериализуем компонент
									if (AComponent->ComponentStyle.Contains(csSubComponent)) {
										DoPersistentToString(AGetObject, sOutPropName, ANode, bSaveNonStored);
									}
									else {
										// здесь можно только имя компонента вывести
									}
								}
								else if (AGetObject->InheritsFrom(__classid(TPersistent))) {
									DoPersistentToString(AGetObject, sOutPropName, ANode, bSaveNonStored);
								}
							}
							else {
								// объект пустой
							}
						}break;
					case tkInteger:
					case tkEnumeration:
					case tkSet: {
							const int iDefault = APropList[i]->Default;
							const int iOrdProp = GetOrdProp(ARootObject, sPropName);
							if (bSaveNonStored || (iDefault != iOrdProp)) {
								_di_IXMLNode AItem = ANode->AddChild("item");
								AItem->SetAttribute("Name", sOutPropName);
								AItem->SetText(GetPropValue(ARootObject, sPropName));
							}
						}break;
					default: {
							_di_IXMLNode AItem = ANode->AddChild("item");
							AItem->SetAttribute("Name", sOutPropName);
							AItem->SetText(GetPropValue(ARootObject, sPropName));
						}
					}
				}
			}
		}
	}

	// ---------------------------------------------------------------------------
	UnicodeString PersistentToString(TPersistent *APersistent, bool bSaveNonStored /* =false */ ) {
		if (!APersistent)
			throw Exception("Persistent object can not be NULL!");

		std::auto_ptr<TXMLDocument>AXMLDocumentPtr(new TXMLDocument(Application));
		AXMLDocumentPtr->Active = true;
		Xmlintf::_di_IXMLNode ANode = AXMLDocumentPtr->AddChild("Root");
		DoPersistentToString(APersistent, L"", ANode, bSaveNonStored);
		return AXMLDocumentPtr->XML->Text;
	}

	// ---------------------------------------------------------------------------
	void PersistentToXML(TPersistent * APersistent, TXMLDocument *ADocument, bool bSaveNonStored/* = false*/) {
		if (!APersistent)
			throw Exception("Persistent object can not be NULL!");

		if (!ADocument)
			throw Exception("XMLDoc object can not be NULL!");

		if (!ADocument->DocumentElement)
			throw Exception("XML require valid DocumentElement!");

		DoPersistentToString(APersistent, L"", ADocument->DocumentElement, bSaveNonStored);
	}

	// ---------------------------------------------------------------------------
	UnicodeString GetContainedPropValue(TObject *Sender, UnicodeString sPropPath) {

		UnicodeString sPropValue = L"";

		const TTypeKinds ATypeKinds = TTypeKinds() << tkInteger << tkChar << tkEnumeration << tkFloat << tkString << tkSet << tkClass <<
			tkWChar << tkLString << tkWString << tkVariant << tkArray << tkRecord << tkInterface << tkInt64 << tkDynArray << tkUString <<
			tkClassRef << tkPointer << tkProcedure;

		std::auto_ptr<TStringList>AObjectsList(new TStringList());
		AObjectsList->Delimiter = L'.';
		AObjectsList->StrictDelimiter = true;
		AObjectsList->DelimitedText = sPropPath.Trim();

		TObject *APersistentObject = Sender;

		// значит свойство составное
		if (AObjectsList->Count > 1) {
			for (int i = 0; i < AObjectsList->Count - 1; i++) {
				if (!Typinfo::PropIsType(APersistentObject, AObjectsList->Strings[i], Typinfo::tkClass))
					throw Exception(UnicodeString().sprintf(L"[%s] is not class property of[%s]", AObjectsList->Strings[i].c_str(),
						APersistentObject->ClassName().c_str()));

				if (!IsPublishedProp(APersistentObject, AObjectsList->Strings[i]))
					throw Exception(UnicodeString().sprintf(L"[%s] is not published property of [%s]", AObjectsList->Strings[i].c_str(),
						APersistentObject->ClassName().c_str()));

				APersistentObject = GetObjectProp(APersistentObject, AObjectsList->Strings[i]);
				if (!APersistentObject)
					throw Exception(UnicodeString().sprintf(L"Object with PropName [%s] is NULL!", AObjectsList->Strings[i].c_str()));
			}
			sPropPath = AObjectsList->Strings[AObjectsList->Count - 1];
		}

		if (!IsPublishedProp(APersistentObject, sPropPath))
			throw Exception(UnicodeString().sprintf(L"[%s] is not published property of [%s]", sPropPath.c_str(),
				APersistentObject->ClassName().c_str()));

		if (APersistentObject->InheritsFrom(__classid(TStrings))) {
			TStrings *AStrings = dynamic_cast<TStrings*>(APersistentObject);
			if (AStrings) {
				if (sPropPath == "Text") {
					sPropValue = AStrings->Text;
				}
			}
		}
		else {

			PPropInfo APropInfo = GetPropInfo(APersistentObject, sPropPath, ATypeKinds);
			if (APropInfo) {
				const UnicodeString sTypeKindName = AnsiString((*APropInfo->PropType)->Name);

				if (sTypeKindName == "TDate") {
					const TDate ADate = GetPropValue(APersistentObject, sPropPath);
					sPropValue = DateToStr(ADate, GetSerializeFormatSettings());
				}
				else if (sTypeKindName == "TTime") {
					const TTime ADate = GetPropValue(APersistentObject, sPropPath);
					sPropValue = TimeToStr(ADate, GetSerializeFormatSettings());
				}
				else if (sTypeKindName == "TDateTime") {
					const TDateTime ADate = GetPropValue(APersistentObject, sPropPath);
					sPropValue = DateTimeToStr(ADate, GetSerializeFormatSettings());
				}
				else if (sTypeKindName == "TCursor") {
					const int ACursor = GetPropValue(APersistentObject, sPropPath);
					sPropValue = CursorToString(TCursor(ACursor));
				}
				else if (sTypeKindName == "TColor") {
					const int AColor = GetPropValue(APersistentObject, sPropPath);
					sPropValue = ColorToString(TColor(AColor));
				}
				else {
					sPropValue = GetPropValue(APersistentObject, sPropPath);
				}
			}

		}

		return sPropValue;
	}

	void GetAllNestedPublishedProps(TObject *Sender, TStrings *APublishedProps, const UnicodeString &sPath) {
		if (!Sender)
			return;

		TPropList APropList;

		TTypeKinds TypeKinds = TTypeKinds() << tkInteger << tkChar << tkEnumeration << tkFloat << tkString << tkSet << tkClass <<
			tkWChar << tkLString << tkWString << tkVariant << tkArray << tkRecord << tkInterface << tkInt64 << tkDynArray << tkUString <<
			tkClassRef << tkPointer << tkProcedure;

		const int iRes = GetPropList(PTypeInfo(Sender->ClassInfo()), TypeKinds, (TPropList*)(&APropList), false);

		for (int i = 0; i < iRes; i++) {
			TTypeInfo *ATypeInfo = *APropList[i]->PropType;

			const UnicodeString sPropName = AnsiString(APropList[i]->Name);

			{
				bool AIncludeProp = true;
				Propinspext::IsPropIncluded(Sender, sPropName, AIncludeProp);

				if (!AIncludeProp)
					continue;
			}

			const UnicodeString sPrefix = sPath.IsEmpty() ? sPropName : (sPath + L"." + sPropName);

			APublishedProps->Add(sPrefix);

			// WLOG_DEBUG(L"%s", sPrefix.c_str());

			if (ATypeInfo->Kind == tkClass) {
				TObject *AChildObject = GetObjectProp(Sender, sPropName);

				TComponent * AComponent = dynamic_cast<TComponent*>(AChildObject);
				if (AComponent && AComponent->Owner != AChildObject) {
					continue;
				}
				GetAllNestedPublishedProps(AChildObject, APublishedProps, sPrefix);
			}
		}
	}
}
