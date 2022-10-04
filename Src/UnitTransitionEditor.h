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

#ifndef UnitTransitionEditorH
#define UnitTransitionEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "LMDSedDocument.hpp"
#include "LMDSedView.hpp"
//---------------------------------------------------------------------------

class TStateMachineConnection;
class TTransitionStateMachineEditor;
class TScxmlBaseShape;
class TStateShape;

class TFormTransitionEditor : public TForm
{
__published:	// IDE-managed Components
	TActionList *ActionList1;
	TAction *ActionSave;
	TPanel *Panel1;
	TEdit *EditTransitionName;
	TEdit *EditCondition;
	TLabel *Label1;
	TImageList *ImageList1;
	TSpeedButton *SpBtnSave;
	TAction *ActionEditScript;
	TButton *BtnEditCondition;
	TButton *BtnEditEvent;
	void __fastcall ActionSaveExecute(TObject *Sender);
	void __fastcall ActionSaveUpdate(TObject *Sender);
	void __fastcall EditTransitionNameChange(TObject *Sender);
	void __fastcall EditConditionChange(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BtnEditConditionClick(TObject *Sender);
	void __fastcall BtnEditEventClick(TObject *Sender);
private:	// User declarations
	TTransitionStateMachineEditor *FStateMachineEditor;
	TStateMachineConnection *FStateMachineConnection;

   	void __fastcall OnTreeModified(TObject *Sender);

public:		// User declarations
	__fastcall TFormTransitionEditor(TComponent* Owner, TStateMachineConnection *AStateMachineConnection);

	bool __fastcall IsModified(void);

	__property TTransitionStateMachineEditor *StateMachineEditor = { read=FStateMachineEditor };
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTransitionEditor *FormTransitionEditor;
//---------------------------------------------------------------------------
#endif
