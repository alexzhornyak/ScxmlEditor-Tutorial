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

#ifndef UnitImagePreviewerH
#define UnitImagePreviewerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <StdActns.hpp>
#include "ElBtnEdit.hpp"
#include "ElEdits.hpp"
#include "ElNameEdits.hpp"
#include "ElXPThemedControl.hpp"
#include "ElSpin.hpp"
//---------------------------------------------------------------------------

class TStateMachineEditor;
class TScxmlBaseShape;

class TImagePreviewer : public TForm
{
__published:	// IDE-managed Components
	TImage *Image1;
	TPanel *Panel1;
	TActionList *ActionList1;
	TImageList *ImageList1;
	TFileSaveAs *SaveAs;
	TAction *ActionStretch;
	TCheckBox *CheckBox1;
	TAction *ActionKeepAspectRatio;
	TCheckBox *CheckBox2;
	TAction *ActionSave;
	TButton *Button2;
	TCheckBox *CheckPNG;
	TCheckBox *CheckJPG;
	TCheckBox *CheckBMP;
	TPanel *Panel2;
	TLabeledEdit *EditFile;
	TAction *ActionCopyToClipboard;
	TButton *Button1;
	TLabel *Label1;
	TCheckBox *CheckSVG;
	TButton *Button3;
	TAction *ActionCopySVGToClipboard;
	TCheckBox *CheckProtectOverwrite;
	TLabel *Label2;
	TCheckBox *CheckAutoGenerateFileName;
	TElFolderNameEdit *EditDirectory;
	TLabel *Label3;
	TElSpinEdit *SpinPNGCompressionLevel;
	TLabel *Label4;
	void __fastcall ActionStretchExecute(TObject *Sender);
	void __fastcall ActionStretchUpdate(TObject *Sender);
	void __fastcall ActionKeepAspectRatioExecute(TObject *Sender);
	void __fastcall ActionKeepAspectRatioUpdate(TObject *Sender);
	void __fastcall ActionSaveExecute(TObject *Sender);
	void __fastcall HandlerUpdateCaption(TObject *Sender);
	void __fastcall ActionCopyToClipboardExecute(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ActionCopySVGToClipboardExecute(TObject *Sender);
private:	// User declarations
	void __fastcall UpdateCaption();

	bool __fastcall IsFileSavingAllowed(const UnicodeString &sFileName);

	TStateMachineEditor *FStateMachineEditor;
	TScxmlBaseShape *FScxmlBaseShape;

public:		// User declarations
	__fastcall TImagePreviewer(TStateMachineEditor *AStateMachineEditor, TScxmlBaseShape *AScxmlBaseShape);
};
//---------------------------------------------------------------------------
extern PACKAGE TImagePreviewer *ImagePreviewer;
//---------------------------------------------------------------------------
#endif
