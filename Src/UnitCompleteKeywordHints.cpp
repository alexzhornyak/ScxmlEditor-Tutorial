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

#include "UnitCompleteKeywordHints.h"

#include <memory>

#include "Log4cpp_VCL.hpp"

#include <XMLDoc.hpp>
#include "UnitSyntaxEditorForm.h"
#include "UnitCompleteItems.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

__fastcall TAutoCompleteKeywordHintItem::TAutoCompleteKeywordHintItem(TCollection* Collection) : TCollectionItem(Collection) {
	FKeyword = L"";
	FIsFunction = false;
	FRetVal = L"";
	FDescription = L"";
	FParams = L"";
}

// ---------------------------------------------------------------------------
__fastcall TAutoCompleteKeywordHintItem::~TAutoCompleteKeywordHintItem() {

}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteKeywordHintItem::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	const UnicodeString sPropName = APropInfo->Name();

	std::auto_ptr<TStringList>AUnicodeEditorsPtr(new TStringList());
	AUnicodeEditorsPtr->CaseSensitive = true;
	AUnicodeEditorsPtr->Add(L"RetVal");
	AUnicodeEditorsPtr->Add(L"Description");
	AUnicodeEditorsPtr->Add(L"Params");

	if (AUnicodeEditorsPtr->IndexOf(sPropName) != -1) {
		AEditorClass = __classid(TUnicodeStringTextDialogEditor);
	}
}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TAutoCompleteKeywordHintItem::GetDisplayName(void) {
	return "Item " + UnicodeString(Index) + " {" + Keyword + "}";
}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteKeywordHintItem::Assign(TPersistent* Source) {
	if (Source) {
		TAutoCompleteKeywordHintItem * AItem = dynamic_cast<TAutoCompleteKeywordHintItem*>(Source);
		if (AItem) {
			FKeyword = AItem->FKeyword;
			FIsFunction = AItem->FIsFunction;
			FRetVal = AItem->FRetVal;
			FDescription = AItem->FDescription;
			FParams = AItem->FParams;
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void TAutoCompleteKeywordHintItem::Clear() {
	std::auto_ptr<TAutoCompleteKeywordHintItem>ACompleteKeywordHintItemPtr(new TAutoCompleteKeywordHintItem(NULL));
	this->Assign(ACompleteKeywordHintItemPtr.get());
}

// ---------------------------------------------------------------------------
// --------------------------- COLLECTION ------------------------------------
// ----------------------- TAutoCompleteKeywordHintItems -------------------------------
// ---------------------------------------------------------------------------
__fastcall TAutoCompleteKeywordHintItems::TAutoCompleteKeywordHintItems(TPersistent *AOwner) : TOwnedCollection(AOwner,
	__classid(TAutoCompleteKeywordHintItem)) {

}

// ---------------------------------------------------------------------------
System::UnicodeString __fastcall TAutoCompleteKeywordHintItems::GetDisplayName(void) {
	return UnicodeString().sprintf(L"[%d] item(s)", this->Count);
}

// ---------------------------------------------------------------------------
TAutoCompleteKeywordHintItem * __fastcall TAutoCompleteKeywordHintItems::IndexOfKeyword(const UnicodeString &sKeyword) {
	for (int i = 0; i < this->Count; i++) {
		if (SameText(this->Items[i]->Keyword, sKeyword)) {
			return this->Items[i];
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TAutoCompleteKeywordHintItem* __fastcall TAutoCompleteKeywordHintItems::Add(void) {
	return dynamic_cast<TAutoCompleteKeywordHintItem*>(TOwnedCollection::Add());
}

// ---------------------------------------------------------------------------
TAutoCompleteKeywordHintItem* __fastcall TAutoCompleteKeywordHintItems::Insert(int Index) {
	return dynamic_cast<TAutoCompleteKeywordHintItem*>(TOwnedCollection::Insert(Index));
}

// ---------------------------------------------------------------------------
TAutoCompleteKeywordHintItem* __fastcall TAutoCompleteKeywordHintItems::GetItem(int Index) {
	return dynamic_cast<TAutoCompleteKeywordHintItem*>(TOwnedCollection::GetItem(Index));
}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteKeywordHintItems::SetItem(int Index, TAutoCompleteKeywordHintItem* Value) {
	TOwnedCollection::SetItem(Index, Value);
}

// ---------------------------------------------------------------------------
void __fastcall TAutoCompleteKeywordHintItems::Assign(TPersistent* Source) {
	if (Source) {
		TAutoCompleteKeywordHintItems * AItems = dynamic_cast<TAutoCompleteKeywordHintItems*>(Source);
		if (AItems) {
			TOwnedCollection::Assign(Source);

			// здесь применяем настройки непосредственно этого класса
			// ПРИМЕР:

		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void __fastcall TFormAutoCompleteKeywordHintCollectionEditor::OnImportButtonClick(TObject *Sender) {
	try {
		LMDDesignObjects1->Clear();

		UnicodeString sFileName = L"";
		if (Dialogs::PromptForFileName(sFileName, L"Notepad files (*.xml)|*.xml|All files (*.*)|*.*", L"xml", L"Select file for import",
				L"", false)) {

			Xmlintf::_di_IXMLDocument ADoc = LoadXMLDocument(sFileName);

			if (ADoc->DocumentElement) {

				this->MarkModified();

				/* Be Careful! May be empty! */
				TAutoCompleteItem *ACompleteItem = dynamic_cast<TAutoCompleteItem*>(this->FSourceCollection->Owner());

				for (int i = 0; i < ADoc->DocumentElement->ChildNodes->Count; i++) {
					_di_IXMLNode AutoCompleteNode = ADoc->DocumentElement->ChildNodes->Nodes[i];
					if (AutoCompleteNode->NodeName == L"AutoComplete") {
						for (int k = 0; k < AutoCompleteNode->ChildNodes->Count; k++) {
							_di_IXMLNode AKeywordNode = AutoCompleteNode->ChildNodes->Nodes[k];
							if (AKeywordNode->NodeName == L"KeyWord") {
								TAutoCompleteKeywordHintItems * ACompleteKeywordHintItems = dynamic_cast<TAutoCompleteKeywordHintItems*>
									(OwnedCollection);
								if (ACompleteKeywordHintItems) {

									const UnicodeString sKeyword = AKeywordNode->HasAttribute("name") ? AKeywordNode->GetAttribute("name")
										: System::OleVariant(L"");

									if (sKeyword.IsEmpty())
										continue;

									TAutoCompleteKeywordHintItem *AItem = ACompleteKeywordHintItems->IndexOfKeyword(sKeyword);
									if (!AItem) {
										AItem = ACompleteKeywordHintItems->Add();
									}
									else {
										AItem->Clear();
									}

									AItem->Keyword = sKeyword;

									if (ACompleteItem) {
										if (ACompleteItem->Default->IndexOf(sKeyword) == -1 && ACompleteItem->Custom->IndexOf(sKeyword)
											== -1) {
											ACompleteItem->Custom->Add(sKeyword);
										}
									}

									if (AKeywordNode->HasAttribute("func")) {
										const UnicodeString sIsFunc = AKeywordNode->GetAttribute("func");
										AItem->IsFunction = SameText(sIsFunc, "yes");
									}

									for (int n = 0; n < AKeywordNode->ChildNodes->Count; n++) {
										_di_IXMLNode AOverloadNode = AKeywordNode->ChildNodes->Nodes[n];
										if (AOverloadNode) {
											if (AOverloadNode->HasAttribute("retVal")) {
												AItem->RetVal = AItem->RetVal + (AItem->RetVal.IsEmpty() ? L"" : L";")
													+ AOverloadNode->GetAttribute("retVal");
											}
											if (AOverloadNode->HasAttribute("descr")) {
												AItem->Description = AItem->Description + (AItem->Description.IsEmpty() ? L"" : L"\n\n")
													+ AOverloadNode->GetAttribute("descr");
											}

											for (int m = 0; m < AOverloadNode->ChildNodes->Count; m++) {
												_di_IXMLNode AParamNode = AOverloadNode->ChildNodes->Nodes[m];
												if (AParamNode) {
													if (AParamNode->HasAttribute("name")) {
														AItem->Params = AItem->Params + (AItem->Params.IsEmpty() ? L"" : L";")
															+ AParamNode->GetAttribute("name");
													}
												}
											}

										}
									}

								}
							}
						}
					}
				}

			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	FillListBox();
}

// ---------------------------------------------------------------------------
