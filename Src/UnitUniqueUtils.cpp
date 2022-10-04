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

#include "UnitUniqueUtils.h"

#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"
#include "UnitScxmlBaseShape.h"
#include "UnitStateMachineConnection.h"
#include "UnitTreeEditorUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Unique {

	// ---------------------------------------------------------------------------
	UnicodeString GetUniqueNameTimeAddon(const UnicodeString &sBase) {
		return sBase + "__" + UnicodeString().sprintf(L"%llX", (double)Now());
	}

	// ---------------------------------------------------------------------------
	TUniquePair GetUniqueName(TComponent *AOwner, TMetaClass *AClass) {
		if (AOwner) {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->CaseSensitive = true;

			const UnicodeString sBase = AClass->ClassName().Delete(1, 1);
			for (int i = 0; i < AOwner->ComponentCount; i++) {
				if (AOwner->Components[i]->Name.Pos(sBase)) {
					const int iSeparator = AOwner->Components[i]->Name.Pos("__");
					if (iSeparator) {
						AStringListPtr->Add(AOwner->Components[i]->Name.SubString(1, iSeparator - 1));
					}
					else {
						AStringListPtr->Add(AOwner->Components[i]->Name);
					}
				}
			}
			int iCount = 1;
			UnicodeString sNewName = L"";
			do {
				sNewName = sBase + UnicodeString(iCount++);
			}
			while (AStringListPtr->IndexOf(sNewName) != -1);

			return std::make_pair(sNewName, GetUniqueNameTimeAddon(sNewName));
		}
		return std::make_pair(L"", L"");
	}

	// ---------------------------------------------------------------------------
	UnicodeString CreateUniqueSimpleText(TTreeNodeShape *AShape, TMetaClass *ABaseClass/* = __classid(TTreeNodeShape)*/) {
		if (AShape && AShape->Tree) {
			const UnicodeString sBase = AShape->ClassName().Delete(1, 1);
			int iCount = 1;
			UnicodeString sNewName = L"";

			std::auto_ptr<TStringList>AStringListPtr(new TStringList());

			for (int i = 0; i < AShape->Tree->Shapes->Count; i++) {
				if (AShape != AShape->Tree->Shapes->Items[i] && AShape->Tree->Shapes->Items[i]->InheritsFrom(ABaseClass)) {
					AStringListPtr->Add(AShape->Tree->Shapes->Items[i]->SimpleText);
				}
			}

			do {
				sNewName = sBase + UnicodeString(iCount++);
			}
			while (AStringListPtr->IndexOf(sNewName) != -1);

			return sNewName;
		}
		return L"";
	}

	// ---------------------------------------------------------------------------
	void UpdateUniqueChildConnectionName(TTreeNodeShape *AShape) {
		for (int k = 0; k < AShape->Parents->Count; k++) {
			TTreeNodeShape *AParent = AShape->Parents->Items[k];
			for (int i = 0; i < AParent->Connections->Count; i++) {
				TTreeConnection *AConnection = AParent->Connections->Items[i];
				if (AConnection->ToShape == AShape) {
					AConnection->Name = AConnection->FromShape->Name + "_" + AConnection->ToShape->Name;
					return;
				}
			}
		}
	}

	// ---------------------------------------------------------------------------
	bool CompareObjects(TObject *AObject1, TObject *AObject2, TStrings *ASkippedProperties, const UnicodeString &sPropPath,
		CompareError &ACompareError, bool bCompareWithReadonlyProps/* = false*/) {

		ACompareError.Clear();

		if (AObject1 == AObject2)
			return true;

		if (!AObject1 || !AObject2) {
			ACompareError.PropName = sPropPath;
			ACompareError.Mismatch = L"One of compared objects is NULL";
			ACompareError.Code = cecOneObjectsIsNULL;
			return false;
		}

		TComponent * AComponent = dynamic_cast<TComponent*>(AObject1);
		const UnicodeString sObjectName = AComponent ? AComponent->Name : AObject1->ClassName();

		if (AObject1->ClassType() != AObject2->ClassType()) {
			ACompareError.ObjectName = sObjectName;
			ACompareError.PropName = sPropPath;
			ACompareError.Mismatch = UnicodeString().sprintf(L"Object1:[%s]!=Object2:[%s]", AObject1->ClassName().c_str(),
				AObject2->ClassName().c_str());
			ACompareError.Code = cecClassTypeMismatch;
			return false;
		}

		// TStrings - сравнивание текста
		if (AObject1->ClassType()->InheritsFrom(__classid(TStrings))) {
			const UnicodeString &sText1 = dynamic_cast<TStrings*>(AObject1)->Text;
			const UnicodeString &sText2 = dynamic_cast<TStrings*>(AObject2)->Text;

			const bool bEqual = sText1 == sText2;
			if (!bEqual) {
				ACompareError.ObjectName = sObjectName;
				ACompareError.PropName = sPropPath + L".Text";
				ACompareError.Mismatch = UnicodeString().sprintf(L"Text:[%s]!=[%s]", sText1.c_str(), sText2.c_str());
				ACompareError.Code = cecStringsTextMismatch;
			}
			return bEqual;
		}

		// коллекция
		if (AObject1->ClassType()->InheritsFrom(__classid(TCollection))) {

			TCollection * ACollection1 = dynamic_cast<TCollection*>(AObject1);
			TCollection * ACollection2 = dynamic_cast<TCollection*>(AObject2);

			if (!ACollection1 || !ACollection2) {
				ACompareError.ObjectName = sObjectName;
				ACompareError.PropName = sPropPath;
				ACompareError.Mismatch = L"One of compared objects is NULL";
				ACompareError.Code = cecOneOfCollectionIsNULL;
				return false;
			}

			if (ACollection1->Count != ACollection2->Count) {
				ACompareError.ObjectName = sObjectName;
				ACompareError.PropName = sPropPath;
				ACompareError.Mismatch = UnicodeString().sprintf(L"Collection1 count:[%d] but Collection2 count:[%d]!",
					ACollection1->Count, ACollection2->Count);
				ACompareError.Code = cecCollectionCountMismatch;
				return false;
			}

			for (int n = 0; n < ACollection1->Count; n++) {
				if (!CompareObjects(ACollection1->Items[n], ACollection2->Items[n], ASkippedProperties, sPropPath, ACompareError)) {
					return false;
				}
			}
		}

		TPropList APropList;

		TTypeKinds TypeKinds = TTypeKinds() << tkInteger << tkChar << tkEnumeration << tkFloat << tkString << tkSet << tkClass <<
			tkWChar << tkLString << tkWString << tkVariant << tkArray << tkRecord << tkInterface << tkInt64 << tkDynArray << tkUString <<
			tkClassRef << tkPointer << tkProcedure;

		const int iRes = GetPropList(PTypeInfo(AObject1->ClassInfo()), TypeKinds, (TPropList*)(&APropList), false);

		for (int i = 0; i < iRes; i++) {
			TTypeInfo *ATypeInfo = *APropList[i]->PropType;

			const UnicodeString sPropName = AnsiString(APropList[i]->Name);

			const UnicodeString sPrefix = sPropPath.IsEmpty() ? sPropName : (sPropPath + L"." + sPropName);

			if (ASkippedProperties->IndexOf(sPrefix) != -1)
				continue;

			const UnicodeString sTypeKindName = AnsiString(ATypeInfo->Name);

			if (bCompareWithReadonlyProps || (APropList[i]->SetProc && //
					(IsStoredProp(AObject1, sPropName) || IsStoredProp(AObject2, sPropName)))) {

				switch(ATypeInfo->Kind) {
				case tkClass: {
						TObject *AGetObject1 = GetObjectProp(AObject1, sPropName);
						TObject *AGetObject2 = GetObjectProp(AObject2, sPropName);

						if (AGetObject1) {

							// особый случай - компоненты
							if (AGetObject1->ClassType()->InheritsFrom(__classid(TComponent))) {

								if (TComponent * AComponent1 = dynamic_cast<TComponent*>(AGetObject1)) {
									if (!AComponent1->ComponentStyle.Contains(csSubComponent)) {
										if (TComponent * AComponent2 = dynamic_cast<TComponent*>(AGetObject2)) {
											if (AComponent1->Name != AComponent2->Name) {
												ACompareError.ObjectName = sObjectName;
												ACompareError.PropName = sPrefix;
												ACompareError.Mismatch = UnicodeString().sprintf(L"ComponentName:[%s]!=[%s]",
													AComponent1->Name.c_str(), AComponent2->Name.c_str());
												ACompareError.Code = cecComponentNameMismatch;
												return false;
											}
										}
									}
								}

								continue;
							}

						}

						if (!CompareObjects(AGetObject1, AGetObject2, ASkippedProperties, sPrefix, ACompareError))
							return false;

					}break;
				default: {

						Variant AVar1 = GetPropValue(AObject1, sPropName);
						Variant AVar2 = GetPropValue(AObject2, sPropName);

						if (AVar1 != AVar2) {

							const UnicodeString sVar1 = AVar1;
							const UnicodeString sVar2 = AVar2;

							ACompareError.ObjectName = sObjectName;
							ACompareError.PropName = sPrefix;
							ACompareError.Mismatch = UnicodeString().sprintf(L"Val:[%s]!=[%s]", sVar1.c_str(), sVar2.c_str());
							ACompareError.Code = cecPropertyMismatch;

							return false;
						}
					}
				}
			}
		}

		return true;
	}

	void DeepCompareObjects(TObject *AObject1, TObject *AObject2, TStrings *ASkippedProperties, const UnicodeString &sPropPath,
		std::vector<CompareError> &AMismatch) {

		if (AObject1 == AObject2)
			return;

		if (!AObject1 || !AObject2) {
			CompareError ACompareError;
			ACompareError.PropName = sPropPath;
			ACompareError.Mismatch = L"One of compared objects is NULL";
			ACompareError.Code = cecOneObjectsIsNULL;
			AMismatch.push_back(ACompareError);
			return;
		}

		TComponent * AComponent = dynamic_cast<TComponent*>(AObject1);
		const UnicodeString sObjectName = AComponent ? AComponent->Name : AObject1->ClassName();

		if (AObject1->ClassType() != AObject2->ClassType()) {
			CompareError ACompareError;
			ACompareError.ObjectName = sObjectName;
			ACompareError.PropName = sPropPath;
			ACompareError.Mismatch = UnicodeString().sprintf(L"Object1:[%s]!=Object2:[%s]", AObject1->ClassName().c_str(),
				AObject2->ClassName().c_str());
			ACompareError.Code = cecClassTypeMismatch;
			AMismatch.push_back(ACompareError);
			return;
		}

		// TStrings - сравнивание текста
		if (AObject1->ClassType()->InheritsFrom(__classid(TStrings))) {
			const UnicodeString &sText1 = dynamic_cast<TStrings*>(AObject1)->Text;
			const UnicodeString &sText2 = dynamic_cast<TStrings*>(AObject2)->Text;

			const bool bEqual = sText1 == sText2;
			if (!bEqual) {
				CompareError ACompareError;
				ACompareError.ObjectName = sObjectName;
				ACompareError.PropName = sPropPath + L".Text";
				ACompareError.Mismatch = UnicodeString().sprintf(L"Text:[%s]!=[%s]", sText1.c_str(), sText2.c_str());
				ACompareError.Code = cecStringsTextMismatch;
				AMismatch.push_back(ACompareError);
			}
			return;
		}

		// коллекция
		if (AObject1->ClassType()->InheritsFrom(__classid(TCollection))) {

			TCollection * ACollection1 = dynamic_cast<TCollection*>(AObject1);
			TCollection * ACollection2 = dynamic_cast<TCollection*>(AObject2);

			if (!ACollection1 || !ACollection2) {
				CompareError ACompareError;
				ACompareError.ObjectName = sObjectName;
				ACompareError.PropName = sPropPath;
				ACompareError.Mismatch = L"One of compared objects is NULL";
				ACompareError.Code = cecOneOfCollectionIsNULL;
				AMismatch.push_back(ACompareError);
				return;
			}

			if (ACollection1->Count != ACollection2->Count) {
				CompareError ACompareError;
				ACompareError.ObjectName = sObjectName;
				ACompareError.PropName = sPropPath;
				ACompareError.Mismatch = UnicodeString().sprintf(L"Collection1 count:[%d] but Collection2 count:[%d]!",
					ACollection1->Count, ACollection2->Count);
				ACompareError.Code = cecCollectionCountMismatch;
				AMismatch.push_back(ACompareError);
				return;
			}

			for (int n = 0; n < ACollection1->Count; n++) {
				DeepCompareObjects(ACollection1->Items[n], ACollection2->Items[n], ASkippedProperties, sPropPath, AMismatch);
			}
		}

		TPropList APropList;

		TTypeKinds TypeKinds = TTypeKinds() << tkInteger << tkChar << tkEnumeration << tkFloat << tkString << tkSet << tkClass <<
			tkWChar << tkLString << tkWString << tkVariant << tkArray << tkRecord << tkInterface << tkInt64 << tkDynArray << tkUString <<
			tkClassRef << tkPointer << tkProcedure;

		const int iRes = GetPropList(PTypeInfo(AObject1->ClassInfo()), TypeKinds, (TPropList*)(&APropList), false);

		for (int i = 0; i < iRes; i++) {
			TTypeInfo *ATypeInfo = *APropList[i]->PropType;

			const UnicodeString sPropName = AnsiString(APropList[i]->Name);

			const UnicodeString sPrefix = sPropPath.IsEmpty() ? sPropName : (sPropPath + L"." + sPropName);

			if (ASkippedProperties->IndexOf(sPrefix) != -1)
				continue;

			const UnicodeString sTypeKindName = AnsiString(ATypeInfo->Name);

			if (APropList[i]->SetProc && (IsStoredProp(AObject1, sPropName) || IsStoredProp(AObject2, sPropName))) {

				switch(ATypeInfo->Kind) {
				case tkClass: {
						TObject *AGetObject1 = GetObjectProp(AObject1, sPropName);
						TObject *AGetObject2 = GetObjectProp(AObject2, sPropName);

						if (AGetObject1) {

							// особый случай - компоненты
							if (AGetObject1->ClassType()->InheritsFrom(__classid(TComponent))) {

								if (TComponent * AComponent1 = dynamic_cast<TComponent*>(AGetObject1)) {
									if (!AComponent1->ComponentStyle.Contains(csSubComponent)) {
										if (TComponent * AComponent2 = dynamic_cast<TComponent*>(AGetObject2)) {
											if (AComponent1->Name != AComponent2->Name) {
												CompareError ACompareError;
												ACompareError.ObjectName = sObjectName;
												ACompareError.PropName = sPrefix;
												ACompareError.Mismatch = UnicodeString().sprintf(L"ComponentName:[%s]!=[%s]",
													AComponent1->Name.c_str(), AComponent2->Name.c_str());
												ACompareError.Code = cecComponentNameMismatch;
												AMismatch.push_back(ACompareError);
											}
										}
									}
								}

								continue;
							}

						}

						DeepCompareObjects(AGetObject1, AGetObject2, ASkippedProperties, sPrefix, AMismatch);

					}break;
				default: {

						Variant AVar1 = GetPropValue(AObject1, sPropName);
						Variant AVar2 = GetPropValue(AObject2, sPropName);

						if (AVar1 != AVar2) {

							const UnicodeString sVar1 = AVar1;
							const UnicodeString sVar2 = AVar2;

							CompareError ACompareError;
							ACompareError.ObjectName = sObjectName;
							ACompareError.PropName = sPrefix;
							ACompareError.Mismatch = UnicodeString().sprintf(L"Val:[%s]!=[%s]", sVar1.c_str(), sVar2.c_str());
							ACompareError.Code = cecPropertyMismatch;
							AMismatch.push_back(ACompareError);
						}
					}
				}
			}
		}
	}

	bool CompareInheritedObjects(TObject *AObject1, TObject *AObject2, TStrings *ASkippedProperties, CompareError &ACompareError) {

		const bool bEqual = CompareObjects(AObject1, AObject2, ASkippedProperties, L"", ACompareError);

		if (!bEqual && SettingsData->WarningOnInheritanceMismatch) {
			WLOG_WARNING(L"INHERITANCE> Mismatch:[%s] Prop:[%s] %s", ACompareError.ObjectName.c_str(), ACompareError.PropName.c_str(),
				ACompareError.Mismatch.c_str());
		}

		return bEqual;
	}

	void DeepCompareInheritedObjects(TObject *AObject1, TObject *AObject2, TStrings *ASkippedProperties, TStrings *AMismatch) {

		std::vector<CompareError>AVecMismatch;
		DeepCompareObjects(AObject1, AObject2, ASkippedProperties, L"", AVecMismatch);

		for (std::size_t i = 0; i < AVecMismatch.size(); i++) {
			AMismatch->Add(UnicodeString().sprintf(L"Object:[%s] Prop:[%s] Mismatch:[%s]", //
					AVecMismatch[i].ObjectName.c_str(), AVecMismatch[i].PropName.c_str(), AVecMismatch[i].Mismatch.c_str()));
		}
	}

	bool CompareStreams(TStream *AStream1, TStream *AStream2) {
		if (AStream1->Size != AStream2->Size) {
			return false;
		}

		AStream1->Position = 0;
		AStream2->Position = 0;

		const int i_MAX_BUFF = 4096;

		unsigned char uchBuffer1[i_MAX_BUFF];
		unsigned char uchBuffer2[i_MAX_BUFF];

		while (AStream1->Position < AStream1->Size) {

			memset(uchBuffer1, 0, sizeof(unsigned char) * i_MAX_BUFF);
			memset(uchBuffer2, 0, sizeof(unsigned char) * i_MAX_BUFF);

			const int iBufferLength = AStream1->Read(uchBuffer1, i_MAX_BUFF);
			AStream2->Read(uchBuffer2, i_MAX_BUFF);

			if (!CompareMem(uchBuffer1, uchBuffer2, iBufferLength))
				return false;
		}

		return true;
	}

	// ---------------------------------------------------------------------------
	void CompareInheritedShape(TScxmlBaseShape * ACheckShape, TCustomTree *AInheritedTree, CompareError &ACompareError) {
		if (TScxmlBaseShape * AOriginShape = Editorutils::FindShapeByName<TScxmlBaseShape>(AInheritedTree, ACheckShape->Name)) {
			ACheckShape->InheritanceMismatch = !Unique::CompareInheritedObjects(AOriginShape, ACheckShape,
				ACheckShape->InheritanceResolver->SkippedProps, ACompareError);

			if (!ACheckShape->InheritanceMismatch) {
				ACheckShape->InheritanceMismatch = !CompareParentsName(AOriginShape, ACheckShape);

				if (ACheckShape->InheritanceMismatch) {
					ACompareError.ObjectName = ACheckShape->Name;
					ACompareError.PropName = L"Parent";
					ACompareError.Mismatch = UnicodeString().sprintf(L"Parent:[%s]!=[%s]", //
						(ACheckShape->Parent ? ACheckShape->Parent->Name.c_str() : L""), //
						(AOriginShape->Parent ? AOriginShape->Parent->Name.c_str() : L""));
					ACompareError.Code = cecParentsMismatch;

					if (SettingsData->WarningOnInheritanceMismatch) {
						WLOG_WARNING(L"INHERITANCE> Mismatch:[%s] %s", ACompareError.ObjectName.c_str(), ACompareError.Mismatch.c_str());
					}

				}
			}
		}
		else {
			if (SettingsData->WarningOnInheritanceMismatch) {
				// Удален объект в базовом классе, снимаем блокировку и помечаем для возможности позже разрешить конфликт
				WLOG_WARNING(L"INHERITANCE> Mismatch:[%s] Can not find shape origin!", ACheckShape->Name.c_str());
			}

			ACheckShape->Locked = false;
			ACheckShape->InheritanceMismatch = true;

			ACompareError.ObjectName = ACheckShape->Name;
			ACompareError.PropName = L"BaseShape";
			ACompareError.Mismatch = L"(Not Found)";
			ACompareError.Code = cecOriginShapeNotFound;
		}
	}

	void DeepCompareInheritedShape(TScxmlBaseShape * ACheckShape, TCustomTree *AInheritedTree, std::vector<CompareError> &AMismatch) {
		if (TScxmlBaseShape * AOriginShape = Editorutils::FindShapeByName<TScxmlBaseShape>(AInheritedTree, ACheckShape->Name)) {

			Unique::DeepCompareObjects(AOriginShape, ACheckShape, ACheckShape->InheritanceResolver->DefaultSkippedProps, L"", AMismatch);

			const bool AreParentsEqual = !CompareParentsName(AOriginShape, ACheckShape);

			if (!AreParentsEqual) {
				CompareError ACompareError;
				ACompareError.ObjectName = ACheckShape->Name;
				ACompareError.PropName = L"Parent";
				ACompareError.Mismatch = UnicodeString().sprintf(L"Parent:[%s]!=[%s]", //
					(ACheckShape->Parent ? ACheckShape->Parent->Name.c_str() : L""), //
					(AOriginShape->Parent ? AOriginShape->Parent->Name.c_str() : L""));
				ACompareError.Code = cecParentsMismatch;
				AMismatch.push_back(ACompareError);
			}
		}
		else {
			if (SettingsData->WarningOnInheritanceMismatch) {
				// Удален объект в базовом классе, снимаем блокировку и помечаем для возможности позже разрешить конфликт
				WLOG_WARNING(L"INHERITANCE> Mismatch:[%s] Can not find shape origin!", ACheckShape->Name.c_str());
			}

			ACheckShape->Locked = false;
			ACheckShape->InheritanceMismatch = true;

			CompareError ACompareError;
			ACompareError.ObjectName = ACheckShape->Name;
			ACompareError.PropName = L"BaseShape";
			ACompareError.Mismatch = L"(Not Found)";
			ACompareError.Code = cecOriginShapeNotFound;
			AMismatch.push_back(ACompareError);
		}
	}

	void CompareInheritedConnection(TStateMachineConnection * ACheckConnection, TCustomTree *AInheritedTree,
		Unique::CompareError &ACompareError) {
		if (TStateMachineConnection * AOriginConnection = Editorutils::FindConnectionByName<TStateMachineConnection>(AInheritedTree,
				ACheckConnection->Name)) {

			ACheckConnection->InheritanceMismatch = !Unique::CompareInheritedObjects(AOriginConnection, ACheckConnection,
				ACheckConnection->InheritanceResolver->SkippedProps, ACompareError);

			if (!ACheckConnection->InheritanceMismatch && !ACheckConnection->AreInheritedPointsIgnored) {
				TConnectionPointArray ASourcePoints;
				Editorutils::RememberPoints(AOriginConnection, ASourcePoints);

				TConnectionPointArray ATargetPoints;
				Editorutils::RememberPoints(ACheckConnection, ATargetPoints);

				ACheckConnection->InheritanceMismatch = !Editorutils::AreEqualPoints(ASourcePoints, ATargetPoints);
				if (ACheckConnection->InheritanceMismatch) {
					if (SettingsData->WarningOnInheritanceMismatch) {
						WLOG_WARNING(L"INHERITANCE> Mismatch:[%s] Points position!", ACheckConnection->Name.c_str());
					}

					ACompareError.ObjectName = ACheckConnection->Name;
					ACompareError.PropName = L"Points";
					ACompareError.Mismatch = L"Position not matched";
					ACompareError.Code = cecConnectionPointsMismatch;
				}
			}

		}
		else {
			if (SettingsData->WarningOnInheritanceMismatch) {
				WLOG_WARNING(L"INHERITANCE> Can not find base connection [%s]!", ACheckConnection->Name.c_str());
			}

			ACheckConnection->Locked = false;
			ACheckConnection->InheritanceMismatch = true;

			ACompareError.ObjectName = ACheckConnection->Name;
			ACompareError.PropName = L"BaseConnection";
			ACompareError.Mismatch = L"(Not Found)";
			ACompareError.Code = cecOriginConnectionNotFound;
		}
	}

	void DeepCompareInheritedConnection(TStateMachineConnection * ACheckConnection, TCustomTree *AInheritedTree,
		std::vector<CompareError> &AMismatch) {
		if (TStateMachineConnection * AOriginConnection = Editorutils::FindConnectionByName<TStateMachineConnection>(AInheritedTree,
				ACheckConnection->Name)) {

			Unique::DeepCompareObjects(AOriginConnection, ACheckConnection, ACheckConnection->InheritanceResolver->DefaultSkippedProps,
				L"", AMismatch);

			TConnectionPointArray ASourcePoints;
			Editorutils::RememberPoints(AOriginConnection, ASourcePoints);

			TConnectionPointArray ATargetPoints;
			Editorutils::RememberPoints(ACheckConnection, ATargetPoints);

			if (!Editorutils::AreEqualPoints(ASourcePoints, ATargetPoints)) {
				CompareError ACompareError;
				ACompareError.ObjectName = ACheckConnection->Name;
				ACompareError.PropName = L"Points";
				ACompareError.Mismatch = L"Position not matched";
				ACompareError.Code = cecConnectionPointsMismatch;
				AMismatch.push_back(ACompareError);
			}
		}
		else {
			if (SettingsData->WarningOnInheritanceMismatch) {
				WLOG_WARNING(L"INHERITANCE> Can not find base connection [%s]", ACheckConnection->Name.c_str());
			}

			ACheckConnection->Locked = false;
			ACheckConnection->InheritanceMismatch = true;

			CompareError ACompareError;
			ACompareError.ObjectName = ACheckConnection->Name;
			ACompareError.PropName = L"BaseConnection";
			ACompareError.Mismatch = L"(Not Found)";
			ACompareError.Code = cecOriginConnectionNotFound;
			AMismatch.push_back(ACompareError);
		}
	}

	// ---------------------------------------------------------------------------
	bool CompareParentsName(TTreeNodeShape *AOrigin, TTreeNodeShape *AThis) {
		// у обоих нет родителей
		if (!AOrigin->Parent && !AThis->Parent)
			return true;

		// у одного родитель пустой, у другого нет
		if (!AOrigin->Parent || !AThis->Parent)
			return false;

		return AOrigin->Parent->Name == AThis->Parent->Name;
	}

}
