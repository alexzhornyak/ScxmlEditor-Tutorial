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

#include "UnitPropInspectorExtensions.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Propinspext {

	// ---------------------------------------------------------------------------
	void __fastcall IsPropIncluded(TObject *AInstance, const UnicodeString &sPropName, bool &AIncludeProp) {
		if (AInstance) {
			System::TMethod AMethod;
			AMethod.Data = AInstance;
			AMethod.Code = AInstance->MethodAddress(ON_FILTER_PROP_EVENT);
			if (AMethod.Code) {
				TSettingsFilterPropEvent OnFilterPropEvent = *(TSettingsFilterPropEvent*) & AMethod;
				AIncludeProp = OnFilterPropEvent(sPropName);
			}
		}
	}

	// ---------------------------------------------------------------------------
	void __fastcall PropSettingsInspectorFilterProp(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, bool AIsSubProp,
		bool &AIncludeProp) {

		IsPropIncluded(AInstance, APropInfo->Name(), AIncludeProp);

	}

	// ---------------------------------------------------------------------------
	void __fastcall PropSettingsInspectorGetEditorClass(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo,
		TLMDPropEditorClass &AEditorClass)

	{
		TLMDPropertyInspector * APropInsp = dynamic_cast<TLMDPropertyInspector*>(Sender);
		if (APropInsp) {
			// проверяем, задан ли пользовательский редактор для данного свойства
			System::TMethod AMethod;
			AMethod.Data = AInstance;
			AMethod.Code = AInstance->MethodAddress(ON_GET_PROP_EDITOR_CLASS);
			if (AMethod.Code) {
				TSettingsOnGetPropEditorClass OnGetPropEditorClass = *(TSettingsOnGetPropEditorClass*) & AMethod;
				OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
			}

			// bugfix: у множества, enum, boolean SetValue у Read-Only вызывает AccessViolation
			if (!AEditorClass && APropInfo) {
				const Typinfo::TTypeKind ATypeKind = APropInfo->PropType()->Kind;
				switch(ATypeKind) {
					// enum и boolean
				case tkEnumeration: {
						AEditorClass = __classid(TSafeEnumPropEditor);
					}break;
				case tkSet: {
						AEditorClass = __classid(TSafeSetPropEditor);
					}break;
				}
			}

			// если класс переопределен, то нам надо выполнить проверку на фильтрацию здесь, так как дальше она не будет вызываться
			if (AEditorClass && APropInsp->OnFilterProp) {
				bool AIncludeProp = true;
				// внимание! не поддерживаются доп. свойства!
				APropInsp->OnFilterProp(Sender, AInstance, APropInfo, false /* доп.свойство */, AIncludeProp);
				if (!AIncludeProp) {
					AEditorClass = NULL;
				}
			}

		}
		else {
			UnicodeString sClassName = L"";
			if (Sender) {
				sClassName = Sender->ClassName();
			}
			throw Exception("Can not cast Sender:[" + sClassName + "] to PropertyInspector!");
		}
	}

	// ---------------------------------------------------------------------------

	void __fastcall PropSettingsInspectorGetCaptionColor(TObject *Sender, TLMDPropertyInspectorItem *AItem, TColor &AColor) {
		TLMDPropertyInspector *APropInspector = dynamic_cast<TLMDPropertyInspector*>(Sender);
		if (APropInspector) {

			if (!AItem->PropInfo[0]->IsWritable()) {
				AColor = clGray;
			}

			if (APropInspector->ArrangeKind != akByName) {
				return;
			}

			if (AItem->TypeInfo->Kind == tkClass) {
				AColor = clMaroon;
			}

			if (APropInspector->Selection && APropInspector->Selection->Count) {
				System::TMethod AMethod;
				AMethod.Data = APropInspector->Selection->Item[0];
				AMethod.Code = APropInspector->Selection->Item[0]->MethodAddress(ON_GET_PROP_CAPTION_COLOR);
				if (AMethod.Code) {
					TSettingsOnGetCaptionColor OnGetPropCaptionColor = *(TSettingsOnGetCaptionColor*) & AMethod;
					OnGetPropCaptionColor(Sender, AItem, AColor);
				}
			}
		}
	}

	// ---------------------------------------------------------------------------
	UnicodeString __fastcall PropSettingsInspectorGetHTMLInfo(TObject *Sender, TObject *AInstance, const UnicodeString &sPropName) {
		if (AInstance) {
			System::TMethod AMethod;
			AMethod.Data = AInstance;
			AMethod.Code = AInstance->MethodAddress(ON_GET_HTML_PROP_INFO);
			if (AMethod.Code) {
				TSettingsOnGetHTMLPropertyInfo OnGetHTMLPropertyInfo = *(TSettingsOnGetHTMLPropertyInfo*) & AMethod;
				return OnGetHTMLPropertyInfo(sPropName);
			}
		}
		return L"";
	}

	bool IsWritablePropertyOfPropEditor(TLMDPropEditor *AEditor) {
		bool bRes = true;
		for (int i = 0; i < AEditor->PropCount; i++) {
			if (!AEditor->GetPropInfo(i)->IsWritable())
				return false;
		}
		return bRes && AEditor->PropCount;
	}

	/* TSafeEnumPropEditor */
	void __fastcall TSafeEnumPropEditor::SetValue(const Lmdtypes::TLMDString Value) {
		if (IsWritablePropertyOfPropEditor(this)) {
			Lmdinspropeditors::TLMDEnumPropEditor::SetValue(Value);
		}
	}

	/* TSafeSetElemPropEditorSetValue */
	void __fastcall TSafeSetElemPropEditor::SetValue(const Lmdtypes::TLMDString Value) {
		if (IsWritablePropertyOfPropEditor(this)) {
			Lmdinspropeditors::TLMDSetElemPropEditor::SetValue(Value);
		}
	}

	/* TSafeSetPropEditor:: */
	void __fastcall TSafeSetPropEditor::SetValue(const Lmdtypes::TLMDString Value) {
		if (IsWritablePropertyOfPropEditor(this)) {
			Lmdinspropeditors::TLMDSetPropEditor::SetValue(Value);
		}
	}

	void __fastcall TSafeSetPropEditor::GetSubProps(Classes::TList* AResult) {
		/// если доступно для записи, тогда воспользуемся родным редактором
		if (IsWritablePropertyOfPropEditor(this)) {
			Lmdinspropeditors::TLMDSetPropEditor::GetSubProps(AResult);
		}
		else {
			/// иначе используем безопасный редактор элементов
			// получение MinValue,MaxValue обязательно через CompType !
			TTypeData *ATypeData = GetTypeData(*(GetTypeData(PropTypeInfo)->CompType));
			if (ATypeData) {
				for (int i = ATypeData->MinValue; i <= ATypeData->MaxValue; i++) {
					AResult->Add(new TSafeSetElemPropEditor(this, i));
				}
			}
		}
	}
}
