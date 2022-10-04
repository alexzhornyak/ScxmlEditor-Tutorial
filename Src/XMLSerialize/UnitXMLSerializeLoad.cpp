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

#include "UnitXMLSerializeLoad.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Xmlserialize {

	// ---------------------------------------------------------------------------
	void XMLDeserializeProperty(_di_IXMLNode APropertyNode, TObject *AObject, ON_ERROR_GET_FUNC OnErrorGet /* = 0 */ ) {
		if (APropertyNode->NodeType == ntComment) {
			return;
		}
		if (!AObject)
			throw Exception(__FUNCTION__ " AObject is NULL!");

		const UnicodeString sPropName = APropertyNode->NodeName;

		/// возможность переопределить загрузку свойства
		System::TMethod AMethod;
		AMethod.Data = AObject;
		AMethod.Code = AObject->MethodAddress(ON_SERIALIZE_CUSTOM_GET_PROP);
		if (AMethod.Code) {
			ON_CUSTOM_GET_PROP OnMethod = *(ON_CUSTOM_GET_PROP*) & AMethod;
			if (OnMethod(AObject, sPropName, APropertyNode)) {
				return;
			}
		}

		if (IsPublishedProp(AObject, sPropName)) {
			TPropInfo *APropInfo = GetPropInfo(AObject, sPropName);
			if (APropInfo->SetProc) {

				try {
					TTypeInfo *ATypeInfo = *(APropInfo->PropType);
					const UnicodeString sTypeKindName = AnsiString(ATypeInfo->Name);

					if (sTypeKindName == "TDate") {
						const TDate ADate = StrToDate(APropertyNode->Text, GetSerializeFormatSettings());
						SetPropValue(AObject, sPropName, ADate);
					}
					else if (sTypeKindName == "TTime") {
						const TTime ATime = StrToTime(APropertyNode->Text, GetSerializeFormatSettings());
						SetPropValue(AObject, sPropName, ATime);
					}
					else if (sTypeKindName == "TDateTime") {
						const TDateTime ADateTime = StrToDate(APropertyNode->Text, GetSerializeFormatSettings());
						SetPropValue(AObject, sPropName, ADateTime);
					}
					else if (sTypeKindName == "TCursor") {
						const TCursor ACursor = StringToCursor(APropertyNode->Text);
						SetPropValue(AObject, sPropName, ACursor);
					}
					else if (sTypeKindName == "TColor") {
						const TColor AColor = StringToColor(APropertyNode->Text);
						SetPropValue(AObject, sPropName, AColor);
					}
					else {

						switch(ATypeInfo->Kind) {
						case tkClass: {
								TObject *AGetObject = GetObjectProp(AObject, sPropName);
								if (AGetObject) {
									if (AGetObject->InheritsFrom(__classid(TCollection))) {
										TCollection *ACollection = dynamic_cast<TCollection*>(AGetObject);
										if (ACollection) {
											// чистим коллекцию, чтобы не догружались лишние узлы
											ACollection->Clear();
											for (int k = 0; k < APropertyNode->ChildNodes->Count; k++) {
												XMLDeserializeProperty(APropertyNode->ChildNodes->Nodes[k], AGetObject, OnErrorGet);
											}
										}
									}
									else if (AGetObject->InheritsFrom(__classid(TStrings))) {
										TStrings *AStrings = dynamic_cast<TStrings*>(AGetObject);
										if (AStrings) {
											AStrings->Text = APropertyNode->Text;
										}
									}
									else if (AGetObject->InheritsFrom(__classid(TPersistent))) {
										bool bDoSerialize = true;

										TComponent *AComponent = dynamic_cast<TComponent*>(AGetObject);
										if (AComponent) {
											// если ссылка на компонент, тогда не будем сериализовать,
											// а если этот компонент указан членом класса, тогда да
											if (!AComponent->ComponentStyle.Contains(csSubComponent)) {
												bDoSerialize = false;
											}
										}

										if (bDoSerialize) {
											for (int k = 0; k < APropertyNode->ChildNodes->Count; k++) {
												XMLDeserializeProperty(APropertyNode->ChildNodes->Nodes[k], AGetObject, OnErrorGet);
											}
										}
									}
									else {
										if (OnErrorGet) {
											OnErrorGet
												(__FUNCTION__ "> Unknown class prop object <" + sPropName + "> <" + sTypeKindName + ">");
											OnErrorGet(PrintXMLNodeString(APropertyNode));
										}
									}
								}
								else {
									TComponent *AComponent = GetComponentFromPathName(APropertyNode->Text);
									if (AComponent) {
										SetObjectProp(AObject, sPropName, AComponent);
									}
									else {
										if (OnErrorGet) {
											OnErrorGet
												(__FUNCTION__ "> Path <" + APropertyNode->Text + "> not found <" + sPropName +
												"> <" + sTypeKindName + ">");
											OnErrorGet(PrintXMLNodeString(APropertyNode));
										}
									}
								}

							}break;
						default: {
								SetPropValue(AObject, sPropName, APropertyNode->Text);
							}
						}
					}
				}
				catch(Exception * E) {
					if (OnErrorGet) {
						OnErrorGet(__FUNCTION__ "> " + E->Message);
						OnErrorGet(PrintXMLNodeString(APropertyNode));
					}
				}

			}
			else {
				if (OnErrorGet) {
					OnErrorGet(__FUNCTION__ "> Property has no SetProc <" + sPropName + ">");
					OnErrorGet(PrintXMLNodeString(APropertyNode));
				}
			}
		}
		else if (APropertyNode->NodeName == XML_COLLECTION_ITEM_NAME) {
			TCollection * ACollection = dynamic_cast<TCollection*>(AObject);
			if (ACollection) {
				TCollectionItem *AItem = ACollection->Add();
				for (int k = 0; k < APropertyNode->ChildNodes->Count; k++) {
					XMLDeserializeProperty(APropertyNode->ChildNodes->Nodes[k], AItem, OnErrorGet);
				}
			}
		}
		else {
			if (OnErrorGet) {
				OnErrorGet(__FUNCTION__ "> Unknown property <" + sPropName + ">");
				OnErrorGet(PrintXMLNodeString(APropertyNode));
			}
		}
	}

	// ---------------------------------------------------------------------------
	void ComponentFromXML(_di_IXMLNode ANode, TComponent *AComponent, TComponent *AOwner, ON_ERROR_GET_FUNC OnErrorGet /* = 0 */ ) {

		if (!ANode) {
			if (OnErrorGet) {
				OnErrorGet(__FUNCTION__ "> Node is empty!");
			}
			return;
		}

		if (!AComponent) {

			if (ANode->HasAttribute("ClassName")) {
				const UnicodeString sClassName = ANode->GetAttribute("ClassName");
				TMetaClass *AMetaClass = GetClass(sClassName);
				if (AMetaClass) {
					Application->CreateForm(AMetaClass, &AComponent);

					if (ANode->HasAttribute("Name")) {
						try {
							AComponent->Name = ANode->GetAttribute("Name");
						}
						catch(Exception * E) {
							if (OnErrorGet) {
								OnErrorGet(__FUNCTION__ "> " + E->Message);
								OnErrorGet(PrintXMLNodeString(ANode));
							}
						}
					}

					if (AOwner) {
						AOwner->InsertComponent(AComponent);
					}
				}
				else {
					if (OnErrorGet) {
						OnErrorGet(__FUNCTION__ "> TMetaClass <" + sClassName + "> is not found!");
						OnErrorGet(PrintXMLNodeString(ANode));
					}
				}
			}

		}

		if (AComponent) {

			if (ANode->HasAttribute("Parent")) {
				try {
					TControl * AControl = dynamic_cast<TControl*>(AComponent);
					if (AControl) {
						AControl->Parent = dynamic_cast<TWinControl*>(GetComponentFromPathName(ANode->GetAttribute("Parent")));
						AControl->Visible = true;
					}
				}
				catch(Exception * E) {
					if (OnErrorGet) {
						OnErrorGet(__FUNCTION__ "> " + E->Message);
						OnErrorGet(PrintXMLNodeString(ANode));
					}
				}

			}

			for (int i = 0; i < ANode->ChildNodes->Count; i++) {
				_di_IXMLNode AChildNode = ANode->ChildNodes->Nodes[i];
				if (AChildNode->NodeName == XML_NODE_OBJECT_NAME) {
					ComponentFromXML(AChildNode, 0, AComponent, OnErrorGet);
				}
				else {
					XMLDeserializeProperty(AChildNode, AComponent, OnErrorGet);
				}
			}
		}
	}

};
