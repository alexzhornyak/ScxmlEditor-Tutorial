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
//---------------------------------------------------------------------------
USEFORM("UnitLineStringEditor.cpp", LineStringEditor);
USEFORM("UnitDialogDelayedTransition.cpp", DialogDelayedTransition);
USEFORM("UnitReplaceRefactorForm.cpp", ReplaceRefactor);
USEFORM("UnitIdeInsight.cpp", DialogInsight);
USEFORM("UnitFrameTransitionTypes.cpp", FrameTransitionTypes); /* TFrame: File Type */
USEFORM("UnitDModule.cpp", DModule); /* TDataModule: File Type */
USEFORM("UnitSimpleHelp.cpp", FormSimpleHelp);
USEFORM("UnitFrameSearchResults.cpp", FrameSearchResults); /* TFrame: File Type */
USEFORM("UnitSyntaxEditorEx.cpp", FrameSyntaxEditorEx); /* TFrame: File Type */
USEFORM("UnitFrameTrigger.cpp", FrameTrigger); /* TFrame: File Type */
USEFORM("UnitMain.cpp", FormScxmlGui);
USEFORM("UnitFrameGifMaker.cpp", FrameGifMaker); /* TFrame: File Type */
USEFORM("UnitDialogWait.cpp", DialogWait);
USEFORM("UnitDialogRegisterScxmlUnitsAndProjects.cpp", DialogRegisterScxmlFiles);
USEFORM("UnitFrameScxmlLiveEdit.cpp", FrameScxmlLiveEdit); /* TFrame: File Type */
USEFORM("UnitProtocolBindingImportForm.cpp", BindingsImporter);
USEFORM("UnitHotKeysHelp.cpp", FormHotKeysHelp);
USEFORM("UnitDialogSelectFromToShape.cpp", DialogSelectShape);
USEFORM("UnitSyntaxEditorForm.cpp", FormSyntaxEditorForm);
USEFORM("UnitReplaceForm.cpp", FormReplaceDlg);
USEFORM("UnitMacroComposer.cpp", MacroComposer);
USEFORM("UnitImagePreviewer.cpp", ImagePreviewer);
USEFORM("UnitFormCollectionEditor.cpp", FormCollectionEditor);
USEFORM("UnitPresetCondition.cpp", PresetCondition);
USEFORM("UnitDialogCollectOutputs.cpp", DialogCollectOutputs);
USEFORM("UnitDialogRegExp.cpp", DialogRegExp);
USEFORM("UnitStringPairsEditor.cpp", FormStringPairsEditor);
USEFORM("UnitDialogFileNotFound.cpp", DialogFileNotFound);
USEFORM("UnitSearchForm.cpp", FormSearch);
USEFORM("UnitSettingsEditor.cpp", FormSettingsEditor);
USEFORM("UnitTransitionEditor.cpp", FormTransitionEditor);
USEFORM("UnitDialogTreeSelect.cpp", DialogTreeSelect);
USEFORM("UnitSimpleReplaceDialog.cpp", SimpleReplaceDialog);
USEFORM("UnitStringsEditor.cpp", StringEditorForm);
USEFORM("UnitDialogCollectProperty.cpp", DialogCopyProperty);
USEFORM("UnitPresetOnOff.cpp", PresetOnOff);
USEFORM("UnitComboStringEditor.cpp", ComboStringEditor);
USEFORM("UnitDialogQScxmlComiler.cpp", DialogQScxmlCompiler);
USEFORM("UnitDialogFilterProperty.cpp", DialogFilterProperty);
USEFORM("UnitSyntaxEditor.cpp", FrameSyntaxEditor); /* TFrame: File Type */
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->Title = "ScxmlEditor";
		Application->CreateForm(__classid(TDModule), &DModule);
		Application->CreateForm(__classid(TFormScxmlGui), &FormScxmlGui);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
