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

#include "UnitXMLSerializeSave.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Xmlserialize {

	//---------------------------------------------------------------------------
	void __fastcall XMLSerializeObject(TObject *AObject, _di_IXMLNode AParentNode) {
		if (!AObject)
			throw Exception("XMLSerializeObject> AObject==NULL!!!");
		if (!AParentNode) {
			return;
		}

		/// возможность переопределить запись объекта
		{
			System::TMethod AMethod;
			AMethod.Data = AObject;
			AMethod.Code = AObject->MethodAddress(ON_SERIALIZE_CUSTOM_SET_OBJECT);
			if (AMethod.Code) {
				ON_CUSTOM_SET_OBJECT OnMethod = *(ON_CUSTOM_SET_OBJECT*) & AMethod;
				if (OnMethod(AObject, AParentNode)) {
					return;
				}
			}
		}

		TPropList APropList;

		TTypeKinds TypeKinds = TTypeKinds() << tkInteger << tkChar << tkEnumeration << tkFloat << tkString << tkSet << tkClass <<
			tkWChar << tkLString << tkWString << tkVariant << tkArray << tkRecord << tkInterface << tkInt64 << tkDynArray << tkUString <<
			tkClassRef << tkPointer << tkProcedure;

		const int iRes = GetPropList(PTypeInfo(AObject->ClassInfo()), TypeKinds, (TPropList*)(&APropList), false);

		for (int i = 0; i < iRes; i++) {
			const UnicodeString sPropName = AnsiString(APropList[i]->Name);

			/// возможность переопределить запись определенного свойства
			{
				System::TMethod AMethod;
				AMethod.Data = AObject;
				AMethod.Code = AObject->MethodAddress(ON_SERIALIZE_CUSTOM_SET_PROP);
				if (AMethod.Code) {
					ON_CUSTOM_SET_PROP OnMethod = *(ON_CUSTOM_SET_PROP*) & AMethod;
					if (OnMethod(AObject, sPropName, APropList[i], AParentNode)) {
						return;
					}
				}
			}

			TTypeInfo *ATypeInfo = *APropList[i]->PropType;

			const UnicodeString sTypeKindName = AnsiString(ATypeInfo->Name);

			if (APropList[i]->SetProc && IsStoredProp(AObject, sPropName)) {

				if (sTypeKindName == "TDate") {
					const TDate ADate = GetPropValue(AObject, sPropName);
					AParentNode->ChildValues[sPropName] = DateToStr(ADate, GetSerializeFormatSettings());
				}
				else if (sTypeKindName == "TTime") {
					const TTime ADate = GetPropValue(AObject, sPropName);
					AParentNode->ChildValues[sPropName] = TimeToStr(ADate, GetSerializeFormatSettings());
				}
				else if (sTypeKindName == "TDateTime") {
					const TDateTime ADate = GetPropValue(AObject, sPropName);
					AParentNode->ChildValues[sPropName] = DateTimeToStr(ADate, GetSerializeFormatSettings());
				}
				else if (sTypeKindName == "TCursor") {
					const int ACursor = GetPropValue(AObject, sPropName);
					AParentNode->ChildValues[sPropName] = CursorToString(TCursor(ACursor));
				}
				else if (sTypeKindName == "TColor") {
					const int AColor = GetPropValue(AObject, sPropName);
					AParentNode->ChildValues[sPropName] = ColorToString(TColor(AColor));
				}
				else {

					switch(ATypeInfo->Kind) {
					case tkClass: {
							TObject *AGetObject = GetObjectProp(AObject, sPropName);
							if (AGetObject) {
								if (AGetObject->InheritsFrom(__classid(TCollection))) {
									TCollection * ACollection = dynamic_cast<TCollection*>(AGetObject);

									_di_IXMLNode ACollectionNode = AParentNode->AddChild(sPropName);
									XMLSerializeObject(ACollection, ACollectionNode);

									for (int k = 0; k < ACollection->Count; k++) {
										_di_IXMLNode ACollectionItemNode = ACollectionNode->AddChild(XML_COLLECTION_ITEM_NAME);
										XMLSerializeObject(ACollection->Items[k], ACollectionItemNode);
									}
								}
								else if (AGetObject->InheritsFrom(__classid(TStrings))) {
									TStrings *AStrings = dynamic_cast<TStrings*>(AGetObject);
									if (AStrings) {
										AParentNode->ChildValues[sPropName] = AStrings->Text;
									}
								}
								else if (AGetObject->InheritsFrom(__classid(TComponent))) {
									TComponent *AComponent = dynamic_cast<TComponent*>(AGetObject);

									// как мы сериализуем компонент
									if (AComponent->ComponentStyle.Contains(csSubComponent)) {

										_di_IXMLNode AChildNode = AParentNode->AddChild(sPropName);
										XMLSerializeObject(AGetObject, AChildNode);
									}
									else {
										AParentNode->ChildValues[sPropName] = GetPathNameFromComponent(AComponent);
									}
								}
								else if (AGetObject->InheritsFrom(__classid(TPersistent))) {

									_di_IXMLNode AChildNode = AParentNode->AddChild(sPropName);
									XMLSerializeObject(AGetObject, AChildNode);
								}
							}
							else {
								// Empty Object
							}

						}break;
					case tkInteger:
					case tkEnumeration:
					case tkSet: {
							const int iDefault = APropList[i]->Default;
							const int iOrdProp = GetOrdProp(AObject, sPropName);
							if (iDefault != iOrdProp) {
								AParentNode->ChildValues[sPropName] = GetPropValue(AObject, sPropName);
							}
						}break;
					default: {
							AParentNode->ChildValues[sPropName] = GetPropValue(AObject, sPropName);
						}
					}
				}
			}

		}
	}

	//---------------------------------------------------------------------------
	void SetParentAttribute(TComponent * AComponent, _di_IXMLNode AComponentNode) {
		TControl *AControl = dynamic_cast<TControl*>(AComponent);
		if (AControl) {
			TWinControl *AWinControl = AControl->Parent;
			if (AWinControl) {
				AComponentNode->SetAttribute("Parent", GetPathNameFromComponent(AWinControl));
			}
		}
	}

	//---------------------------------------------------------------------------
	_di_IXMLNode __fastcall XMLSerializeComponent(TComponent * AComponent, _di_IXMLNode ARootNode) {

		assert(AComponent != NULL);

		_di_IXMLNode AOwnerNode = ARootNode->AddChild(XML_NODE_OBJECT_NAME);
		AOwnerNode->SetAttribute("Name", AComponent->Name);
		AOwnerNode->SetAttribute("ClassName", AComponent->ClassName());
		SetParentAttribute(AComponent, AOwnerNode);

		return AOwnerNode;
	}

};
