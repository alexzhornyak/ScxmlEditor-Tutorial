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

#include "UnitImagePreviewer.h"

#include <memory>

#include "Log4cpp_VCL.hpp"
#include <IOUtils.hpp>
#include <PNGImage.hpp>
#include <JPEG.hpp>
#include "VISUAL_GLOBALS.h"
#include "UnitSettings.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitScxmlBaseShape.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElBtnEdit"
#pragma link "ElEdits"
#pragma link "ElNameEdits"
#pragma link "ElXPThemedControl"
#pragma link "ElSpin"
#pragma resource "*.dfm"
TImagePreviewer *ImagePreviewer;

//---------------------------------------------------------------------------
__fastcall TImagePreviewer::TImagePreviewer(TStateMachineEditor * AStateMachineEditor, TScxmlBaseShape * AScxmlBaseShape) : TForm
(FStateMachineEditor), FStateMachineEditor(AStateMachineEditor), FScxmlBaseShape(AScxmlBaseShape) {

	if (!FStateMachineEditor)
		throw Exception(L"Program Logic Error! Owner is not a <TStateMachineEditor>!");

	if (SettingsData) {

		{
			const UnicodeString sValue = SettingsData->TempRegistry->Values[this->Name + ".SpinPNGCompressionLevel"];
			if (!sValue.IsEmpty()) {
				this->SpinPNGCompressionLevel->Value = sValue.ToIntDef(this->SpinPNGCompressionLevel->Value);
			}
		}

		for (int i = 0; i < this->ComponentCount; i++) {
			TCheckBox * ACheckBox = dynamic_cast<TCheckBox*>(this->Components[i]);
			if (ACheckBox) {
				const UnicodeString sValue = SettingsData->TempRegistry->Values[this->Name + "." + ACheckBox->Name];
				if (!sValue.IsEmpty()) {
					ACheckBox->Checked = StrToBoolDef(sValue, ACheckBox->Checked);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TImagePreviewer::ActionStretchExecute(TObject *Sender) {
	Image1->Stretch = !Image1->Stretch;
}

//---------------------------------------------------------------------------
void __fastcall TImagePreviewer::ActionStretchUpdate(TObject *Sender) {
	ActionStretch->Checked = Image1->Stretch;
}

//---------------------------------------------------------------------------
void __fastcall TImagePreviewer::ActionKeepAspectRatioExecute(TObject *Sender) {
	Image1->Proportional = !Image1->Proportional;
}
//---------------------------------------------------------------------------

void __fastcall TImagePreviewer::ActionKeepAspectRatioUpdate(TObject *Sender) {
	ActionKeepAspectRatio->Checked = !Image1->Proportional;
}

//---------------------------------------------------------------------------
void __fastcall TImagePreviewer::ActionSaveExecute(TObject *Sender) {
	try {

		const UnicodeString sFile = TPath::Combine(EditDirectory->Text, EditFile->Text);

		if (CheckPNG->Checked && IsFileSavingAllowed(sFile + ".png")) {
			std::auto_ptr<TPngImage>APngImagePtr(new TPngImage());
			APngImagePtr->Assign(Image1->Picture->Bitmap);
			APngImagePtr->CompressionLevel = SpinPNGCompressionLevel->Value;
			APngImagePtr->SaveToFile(sFile + ".png");
			WLOG_INFO(L"Sucessfully saved [%s.png] with compression level [%d]", sFile.c_str(), APngImagePtr->CompressionLevel);
		}
		if (CheckBMP->Checked && IsFileSavingAllowed(sFile + ".bmp")) {
			Image1->Picture->Bitmap->SaveToFile(sFile + ".bmp");
			WLOG_INFO(L"Sucessfully saved [%s.bmp]", sFile.c_str());
		}
		if (CheckJPG->Checked && IsFileSavingAllowed(sFile + ".jpg")) {
			std::auto_ptr<TJPEGImage>AImagePtr(new TJPEGImage());
			AImagePtr->Assign(Image1->Picture->Bitmap);
			AImagePtr->SaveToFile(sFile + ".jpg");
			WLOG_INFO(L"Sucessfully saved [%s.jpg]", sFile.c_str());
		}
		if (CheckSVG->Checked && IsFileSavingAllowed(sFile + ".svg")) {
			FStateMachineEditor->SaveToSVG(sFile + ".svg", FScxmlBaseShape);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"SAVE IMAGE> %s", E->Message.c_str());
	}

}

//---------------------------------------------------------------------------
void __fastcall TImagePreviewer::UpdateCaption() {
	const UnicodeString sFile = TPath::Combine(EditDirectory->Text, EditFile->Text);
	this->Caption = "ImagePreviewer [" + sFile + "]";

	std::auto_ptr<TStringList>AStringListPtr(new TStringList());
	AStringListPtr->Delimiter = L' ';
	if (CheckPNG->Checked) {
		AStringListPtr->Add("[PNG]");
	}
	if (CheckJPG->Checked) {
		AStringListPtr->Add("[JPG]");
	}
	if (CheckBMP->Checked) {
		AStringListPtr->Add("[BMP]");
	}
	if (CheckSVG->Checked) {
		AStringListPtr->Add("[SVG]");
	}
	if (AStringListPtr->Count) {
		this->Caption = this->Caption + "   " + AStringListPtr->DelimitedText;
	}
}

//---------------------------------------------------------------------------
void __fastcall TImagePreviewer::HandlerUpdateCaption(TObject *Sender) {
	UpdateCaption();
}
//---------------------------------------------------------------------------

void __fastcall TImagePreviewer::ActionCopyToClipboardExecute(TObject *Sender) {
	try {
		Clipboard()->Assign(Image1->Picture);
		GlobalSendBallonMsg("Image was saved to clipboard!", bfInfo);
	}
	catch(Exception * E) {
		WLOG_ERROR(L"COPY IMAGE TO CLIPBOARD> %s", E->Message.c_str());
	}
}

//---------------------------------------------------------------------------
void __fastcall TImagePreviewer::FormClose(TObject *Sender, TCloseAction &Action) {
	if (SettingsData) {

		{
			SettingsData->TempRegistry->Values[this->Name + ".SpinPNGCompressionLevel"] = this->SpinPNGCompressionLevel->Value;
		}

		for (int i = 0; i < this->ComponentCount; i++) {
			TCheckBox * ACheckBox = dynamic_cast<TCheckBox*>(this->Components[i]);
			if (ACheckBox) {
				SettingsData->TempRegistry->Values[this->Name + "." + ACheckBox->Name] = BoolToStr(ACheckBox->Checked);
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TImagePreviewer::ActionCopySVGToClipboardExecute(TObject *Sender) {
	try {
		std::auto_ptr<TStringList>AOutListPtr(new TStringList());
		FStateMachineEditor->SaveToSVG(AOutListPtr.get(), FScxmlBaseShape);

		Clipboard()->AsText = AOutListPtr->Text;
		GlobalSendBallonMsg("Image was saved to clipboard as SVG!", bfInfo);
	}
	catch(Exception * E) {
		WLOG_ERROR(L"COPY SVG TO CLIPBOARD> %s", E->Message.c_str());
	}
}

//---------------------------------------------------------------------------
bool __fastcall TImagePreviewer::IsFileSavingAllowed(const UnicodeString &sFileName) {
	if (CheckProtectOverwrite->Checked && FileExists(sFileName)) {
		const UnicodeString Caption = L"WARNING";
		const UnicodeString Text = L"Are you sure to overwrite <" + sFileName + "> ?";
		if (MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) != IDOK)
			return false;
	}
	return true;
}
//---------------------------------------------------------------------------
