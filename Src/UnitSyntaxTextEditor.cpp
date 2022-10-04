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

#include "UnitSyntaxTextEditor.h"

#include <IOUtils.hpp>

#include "UnitPropInspectorExtensions.h"
#include "UnitScxmlBaseShape.h"
#include "UnitStateMachineUnit.h"
#include "UnitSettings.h"
#include "VISUAL_GLOBALS.h"
#include "UnitDialogFileNotFound.h"
#include "UrlUtils.h"
#include "UnitSyntaxUtils.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
__fastcall TSyntaxTextEditorDockPanel::TSyntaxTextEditorDockPanel(TComponent* Owner, const UnicodeString &sSyntaxScheme,
	const UnicodeString &ADockPanelCaption, const UnicodeString &sText /* = L"" */ ) : TSyntaxDockPanel(Owner, sSyntaxScheme,
	sText), FDockPanelCaption(ADockPanelCaption) {

	this->Caption = FDockPanelCaption;

	SyntaxForm->IsFileMode = true;
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxTextEditorDockPanel::OnUpdateSyntaxFormCaption(TObject *Sender) {
	UnicodeString APanelCaption = FDockPanelCaption;
	if (!SyntaxForm->ActiveFileName.IsEmpty()) {
		// полный путь, так как может быть и текстовый редактор
		APanelCaption = APanelCaption + " [" + SyntaxForm->ActiveFileName + "]";
	}
	this->Caption = SyntaxForm->EditDoc1->Modified ? (APanelCaption + L"*") : APanelCaption;
}

// ---------------------------------------------------------------------------
// -----------------------  TSyntaxSrcDialogEditorImpl -----------------
// ---------------------------------------------------------------------------
TLMDPropAttrs __fastcall TSyntaxSrcDialogEditorImpl::GetAttrs() {
	return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
}

// ---------------------------------------------------------------------------
void __fastcall TSyntaxSrcDialogEditorImpl::Edit(void) {
	try {
		if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
			UnicodeString sValue = this->GetStrValue(0);

			TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(this->GetInstance(0));
			if (AScxmlBaseShape && AScxmlBaseShape->StateMachineEditorUnit) {
				bool bIsNew = false;

				UnicodeString sFileName = TPath::Combine(AScxmlBaseShape->StateMachineEditorUnit->Path, sValue);

				if (sValue.IsEmpty() || !FileExists(sFileName)) {
					const UnicodeString sText = "'"+this->PropName + "' file [" + sValue + "] is not found!";

					std::auto_ptr<TDialogFileNotFound>ADialogFileNotFoundPtr(new TDialogFileNotFound(AScxmlBaseShape->Owner));
					ADialogFileNotFoundPtr->LabelText->Caption = sText;
					switch(ADialogFileNotFoundPtr->ShowModal()) {
					case CUSTOM_MODAL_RESULT_CREATE: {
							if (sValue.IsEmpty()) {
								if (InputQuery("EDIT '"+this->PropName + "'", "Type the name of '"+this->PropName + "' file", sValue)) {
									this->SetStrValue(sValue);
								}
								else {
									throw Exception("Cancelled by user");
								}
							}

							bIsNew = true;
						}break;
					case CUSTOM_MODAL_RESULT_FIND: {
							UnicodeString sTempFileName = ExcludeTrailingPathDelimiter(sFileName);

							// убираем путь из Dialog
							const UnicodeString sDir = ExtractFileDir(AScxmlBaseShape->StateMachineEditorUnit->Path);
							if (SameText(sTempFileName, sDir)) {
								sTempFileName = L"";
							}

							UnicodeString sFileExtension = L"";
							// сначала пытаемс€ получить по пути заложенного файла
							if (!sValue.IsEmpty()) {
								sFileExtension = ExtractFileExt(sValue);
								if (!sFileExtension.IsEmpty()) {
									sFileExtension = sFileExtension.Delete(1, 1);
								}
							}

							if (sFileExtension.IsEmpty()) {
								// если файл не указан, тогда попробуем по схеме Scxml
								sFileExtension = Syntaxutils::SyntaxSchemeToExtension(AScxmlBaseShape->ScxmlSyntaxScheme);
							}

							if (PromptForFileName(sTempFileName, // FileName
									Syntaxutils::GetDialogFilterByExtension(sFileExtension), // Filter
									sFileExtension, // DefaultExt
									L"Find '"+this->PropName + "'", // Title
									sDir, // InitialDir
									false // Is SaveDialog
								)) {

								sFileName = sTempFileName;
							}
							else
								throw Exception("Cancelled by user");

							sValue = ExtractRelativePath(AScxmlBaseShape->StateMachineEditorUnit->Path, sFileName);

							this->SetStrValue(sValue);
						}break;
					default:
						throw Exception("Cancelled by user!");
					}
				}

				/* ѕќ—Ћ≈ Ё“ќ√ќ ћ≈—“ј Ќ≈ »—ѕќЋ№«ќ¬ј“№ 'SetStrValue' !!!*/
				if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sFileName)) {
					TSyntaxTextEditorDockPanel *ADockPanel = new TSyntaxTextEditorDockPanel(GlobalMainForm(),
						AScxmlBaseShape->ScxmlSyntaxScheme, L"'Src' editor");

					try {
						ADockPanel->SyntaxForm->IsFileMode = true;
						ADockPanel->SyntaxForm->ActiveFileName = sFileName;

						if (FileExists(sFileName)) {
							ADockPanel->SyntaxForm->EditDoc1->LoadFromFile(sFileName);
							ADockPanel->SyntaxForm->EditDoc1->Modified = false;
						}
						else {
							// иначе даем пользователю возможность потом сохранить этот пустой файл
							ADockPanel->SyntaxForm->EditDoc1->Modified = true;
							if (bIsNew) {
								GlobalSendBallonMsg(UnicodeString().sprintf(L"Press 'OK' for saving <%s>!", sFileName.c_str()), bfInfo);
							}
							else {
								WLOG_ERROR(L"File <%s> is not found!", sFileName.c_str());
							}
						}
					}
					__finally {
						ADockPanel->DockToGlobalRootZone(AScxmlBaseShape->Owner);
					}

				}
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"SYNTAX EDIT[%s]> %s", this->PropName.c_str(), E->Message.c_str());
	}

}

bool __fastcall EditSyntaxSrcProperty(TPersistent *AInstance, const UnicodeString &sPropName) {
	bool bRes = false;
	try {
		UnicodeString sValue = Url::UrlToLocalFile(GetPropValue(AInstance, sPropName));

		TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(AInstance);
		if (AScxmlBaseShape && AScxmlBaseShape->StateMachineEditorUnit) {
			bool bIsNew = false;

			UnicodeString sFileName = TPath::IsPathRooted(sValue) ? sValue : //
			TPath::Combine(AScxmlBaseShape->StateMachineEditorUnit->Path, sValue);

			if (sValue.IsEmpty() || !FileExists(sFileName)) {
				const UnicodeString sText = "'" + sPropName + "' file [" + sValue + "] is not found!";

				std::auto_ptr<TDialogFileNotFound>ADialogFileNotFoundPtr(new TDialogFileNotFound(AScxmlBaseShape->Owner));
				ADialogFileNotFoundPtr->LabelText->Caption = sText;
				switch(ADialogFileNotFoundPtr->ShowModal()) {
				case CUSTOM_MODAL_RESULT_CREATE: {
						if (sValue.IsEmpty()) {
							if (InputQuery("EDIT '" + sPropName + "'", "Type the name of '" + sPropName + "' file", sValue)) {
								SetPropValue(AInstance, sPropName, sValue);
								bRes = true;
							}
							else {
								throw Exception("Cancelled by user");
							}
						}

						bIsNew = true;
					}break;
				case CUSTOM_MODAL_RESULT_FIND: {

						UnicodeString sTempFileName = ExcludeTrailingPathDelimiter(sFileName);

						// убираем путь из Dialog
						const UnicodeString sDir = ExtractFileDir(AScxmlBaseShape->StateMachineEditorUnit->Path);
						if (SameText(sTempFileName, sDir)) {
							sTempFileName = L"";
						}

						UnicodeString sFileExtension = L"";
						// сначала пытаемс€ получить по пути заложенного файла
						if (!sValue.IsEmpty()) {
							sFileExtension = ExtractFileExt(sValue);
							if (!sFileExtension.IsEmpty()) {
								sFileExtension = sFileExtension.Delete(1, 1);
							}
						}

						if (sFileExtension.IsEmpty()) {
							// если файл не указан, тогда попробуем по схеме Scxml
							sFileExtension = Syntaxutils::SyntaxSchemeToExtension(AScxmlBaseShape->ScxmlSyntaxScheme);
						}

						if (PromptForFileName(sTempFileName, // FileName
								Syntaxutils::GetDialogFilterByExtension(sFileExtension), // Filter
								sFileExtension, // DefaultExt
								L"Find '" + sPropName + "'", // Title
								sDir, // InitialDir
								false // Is SaveDialog
							)) {

							sFileName = sTempFileName;
						}
						else
							throw Exception("Cancelled by user");

						sValue = ExtractRelativePath(AScxmlBaseShape->StateMachineEditorUnit->Path, sFileName);
						SetPropValue(AInstance, sPropName, sValue);
						bRes = true;
					}break;
				default:
					throw Exception("Cancelled by user!");
				}
			}

			if (!TFormSyntaxEditorForm::CheckSyntaxDockPanelExists(sFileName)) {
				TSyntaxTextEditorDockPanel *ADockPanel = new TSyntaxTextEditorDockPanel(GlobalMainForm(),
					AScxmlBaseShape->ScxmlSyntaxScheme, L"'Src' editor");

				try {
					ADockPanel->SyntaxForm->IsFileMode = true;
					ADockPanel->SyntaxForm->ActiveFileName = sFileName;

					if (FileExists(sFileName)) {
						ADockPanel->SyntaxForm->EditDoc1->LoadFromFile(sFileName);
						ADockPanel->SyntaxForm->EditDoc1->Modified = false;
					}
					else {
						// иначе даем пользователю возможность потом сохранить этот пустой файл
						ADockPanel->SyntaxForm->EditDoc1->Modified = true;
						if (bIsNew) {
							GlobalSendBallonMsg(UnicodeString().sprintf(L"Press 'OK' for saving <%s>!", sFileName.c_str()), bfInfo);
						}
						else {
							WLOG_ERROR(L"File <%s> is not found!", sFileName.c_str());
						}
					}
				}
				__finally {
					ADockPanel->DockToGlobalRootZone(AScxmlBaseShape->Owner);
				}

			}
		}

	}
	catch(Exception * E) {
		WLOG_ERROR(L"SYNTAX EDIT[%s]> %s", sPropName.c_str(), E->Message.c_str());
	}
	return bRes;
}

// ---------------------------------------------------------------------------
