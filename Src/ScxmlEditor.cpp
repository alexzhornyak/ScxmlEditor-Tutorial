/***********************************************************************************
BSD 3-Clause License

Copyright (c) 2018, https://github.com/alexzhornyak, alexander.zhornyak@gmail.com
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
#include <tchar.h>
#include <wchar.h>

#include "CommonConsts.h"

//---------------------------------------------------------------------------
USEFORM("UnitDialogCollectOutputs.cpp", DialogCollectOutputs);
USEFORM("UnitPresetCondition.cpp", PresetCondition);
USEFORM("UnitSettingsEditor.cpp", FormSettingsEditor);
USEFORM("UnitTransitionEditor.cpp", FormTransitionEditor);
USEFORM("UnitDialogTreeSelect.cpp", DialogTreeSelect);
USEFORM("UnitSearchForm.cpp", FormSearch);
USEFORM("UnitDialogRegExp.cpp", DialogRegExp);
USEFORM("UnitDialogFileNotFound.cpp", DialogFileNotFound);
USEFORM("UnitStringPairsEditor.cpp", FormStringPairsEditor);
USEFORM("UnitSyntaxEditorForm.cpp", FormSyntaxEditorForm);
USEFORM("UnitReplaceForm.cpp", FormReplaceDlg);
USEFORM("UnitDialogSelectFromToShape.cpp", DialogSelectShape);
USEFORM("UnitFormCollectionEditor.cpp", FormCollectionEditor);
USEFORM("UnitImagePreviewer.cpp", ImagePreviewer);
USEFORM("UnitMacroComposer.cpp", MacroComposer);
USEFORM("UnitDialogFilterProperty.cpp", DialogFilterProperty);
USEFORM("UnitDialogQScxmlComiler.cpp", DialogQScxmlCompiler);
USEFORM("UnitComboStringEditor.cpp", ComboStringEditor);
USEFORM("UnitSyntaxEditor.cpp", FrameSyntaxEditor); /* TFrame: File Type */
USEFORM("UnitDialogCollectProperty.cpp", DialogCopyProperty);
USEFORM("UnitStringsEditor.cpp", StringEditorForm);
USEFORM("UnitSimpleReplaceDialog.cpp", SimpleReplaceDialog);
USEFORM("UnitPresetOnOff.cpp", PresetOnOff);
USEFORM("UnitHotKeysHelp.cpp", FormHotKeysHelp);
USEFORM("UnitDModule.cpp", DModule); /* TDataModule: File Type */
USEFORM("UnitSimpleHelp.cpp", FormSimpleHelp);
USEFORM("UnitFrameSearchResults.cpp", FrameSearchResults); /* TFrame: File Type */
USEFORM("UnitLineStringEditor.cpp", LineStringEditor);
USEFORM("UnitFrameTransitionTypes.cpp", FrameTransitionTypes); /* TFrame: File Type */
USEFORM("UnitReplaceRefactorForm.cpp", ReplaceRefactor);
USEFORM("UnitDialogDelayedTransition.cpp", DialogDelayedTransition);
USEFORM("UnitIdeInsight.cpp", DialogInsight);
USEFORM("UnitDialogWait.cpp", DialogWait);
USEFORM("UnitDialogRegisterScxmlUnitsAndProjects.cpp", DialogRegisterScxmlFiles);
USEFORM("UnitFrameScxmlLiveEdit.cpp", FrameScxmlLiveEdit); /* TFrame: File Type */
USEFORM("UnitProtocolBindingImportForm.cpp", BindingsImporter);
USEFORM("UnitSyntaxEditorEx.cpp", FrameSyntaxEditorEx); /* TFrame: File Type */
USEFORM("UnitFrameGifMaker.cpp", FrameGifMaker); /* TFrame: File Type */
USEFORM("UnitMain.cpp", FormScxmlGui);
USEFORM("UnitFrameTrigger.cpp", FrameTrigger); /* TFrame: File Type */


//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR argv, int argc) {
	try {

		/*
			This code is intended to use from command line
			for sending commands to other active ScxmlEditors
		*/
		const int iArgCount = ParamCount();
		if (iArgCount >= 3) {
			if (ParamStr(1) == SCXML_INTERPROCESS_COMMAND) {
				const HWND hwndHandle = (HWND)ParamStr(2).ToIntDef(0);

				const UnicodeString sCommand = ParamStr(3);

				INTERPROCESS_COPYDATA ACopyData;
				memset(&ACopyData, 0, sizeof(INTERPROCESS_COPYDATA));

				wcscpy_s(ACopyData.wchCommand, SCXML_INTERPROCESS_BUFFER_SIZE, sCommand.c_str());

				/* Optional arguments */
				if (iArgCount >= 4) {
					const UnicodeString sArg1 = ParamStr(4);
					wcscpy_s(ACopyData.wchArg1, SCXML_INTERPROCESS_BUFFER_SIZE, sArg1.c_str());
				}

				if (iArgCount >= 5) {
					const UnicodeString sArg2 = ParamStr(5);
					wcscpy_s(ACopyData.wchArg2, SCXML_INTERPROCESS_BUFFER_SIZE, sArg2.c_str());
				}

				if (iArgCount >= 6) {
					const UnicodeString sArg3 = ParamStr(6);
					wcscpy_s(ACopyData.wchArg3, SCXML_INTERPROCESS_BUFFER_SIZE, sArg3.c_str());
				}

				COPYDATASTRUCT MyCDS;
				MyCDS.dwData = 0; // function identifier
				MyCDS.cbData = sizeof(ACopyData); // size of data
				MyCDS.lpData = &ACopyData; // data structure
				SendMessage(hwndHandle, WM_COPYDATA, (WPARAM)0, (LPARAM)(LPVOID) & MyCDS);

				return 0;
			}
		}

		/* Application main thread */
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->Title = "ScxmlEditor";
		Application->CreateForm(__classid(TDModule), &DModule);
		Application->CreateForm(__classid(TFormScxmlGui), &FormScxmlGui);
		Application->Run();
	}
	catch(Exception & exception) {
		Application->ShowException(&exception);
	}
	catch(...) {
		try {
			throw Exception("");
		}
		catch(Exception & exception) {
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
