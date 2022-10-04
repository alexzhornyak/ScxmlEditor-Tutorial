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

#ifndef UnitSyntaxEditorFormH
#define UnitSyntaxEditorFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDSedDocument.hpp"
#include "LMDSedView.hpp"
#include <ActnList.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <StdActns.hpp>
#include "LMDInsPropEditors.hpp"
#include "LMDDckSite.hpp"
#include <Buttons.hpp>
//---------------------------------------------------------------------------

#include <memory>

class TFrameSyntaxEditorEx;
class TStateMachineDockPanel;

class TFormSyntaxEditorForm : public TForm {
__published: // IDE-managed Components
	TActionList *ActionList1;
	TFileOpen *FileOpen1;
	TFileSaveAs *FileSaveAs1;
	TImageList *ImageList1;
	TLMDEditDocument *EditDoc1;
	TPanel *Panel1;
	TButton *BtnCancel;
	TButton *BtnOk;
	TButton *BtnOpen;
	TButton *BtnSave;
	TAction *ActionSave;
	TCheckBox *CheckSwitchToDocPanel;

	void __fastcall FileSaveAs1Accept(TObject *Sender);
	void __fastcall FileOpen1Accept(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall EditDoc1DocChanged(TLMDEditDocument *Sender, TLMDDocStatusChanges AChanges);
	void __fastcall ActionSaveExecute(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall CheckSwitchToDocPanelClick(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);

private: // User declarations

	void __fastcall ApplySyntaxScheme(const UnicodeString &sSyntaxScheme);
	void __fastcall OnFillCompletionListEvent(System::TObject* Sender, Classes::TStrings* Items, const Types::TPoint &CursorPoint,
		TLMDEditFillCompletionCause Cause, const Lmdtypes::TLMDString LeftPart, Types::TPoint &PosForCombo, bool &Handled);

	UnicodeString DisplayName;

	void __fastcall UpdateCaption(void);

	UnicodeString FActiveFileName;
	void __fastcall SetActiveFileName(UnicodeString val);

	UnicodeString FLinkUnit;                // юнит, в котором редактируется свойство
	UnicodeString FLinkInstance;			// объект, свойство которого редактируется
	UnicodeString FLinkInstancePropPath; 	// путь к Инстанс по именам объектов
	void __fastcall SetLinkInstancePropPath(UnicodeString val);
	UnicodeString FLinkProperty;			// свойство, по которому будет доступ

	UnicodeString FUserPreCaption;
	inline UnicodeString __fastcall GetUserPreCaption(void) { return FUserPreCaption.IsEmpty() ? UnicodeString(L"SyntaxEditor") : FUserPreCaption; }

    void __fastcall EditViewKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);

	TStateMachineDockPanel *__fastcall GetStateMachineDockPanel(void);

	std::unique_ptr<TStringList> FDataIDs;
	std::unique_ptr<TStringList> FStateIDs;

	UnicodeString FWasLeftPart;
	void __fastcall OnReplaceForCompletionResultEvent(System::TObject* Sender,
													  Classes::TStrings* Items, int ItemIndex, const Types::TPoint &CursorPoint,
													  TLMDEditReplaceCompletionCause Cause, bool &Handled);

protected:

	TFrameSyntaxEditorEx *FSyntaxEditor;

	virtual UnicodeString __fastcall GetLinkCaption(void);

public: // User declarations
	__fastcall TFormSyntaxEditorForm(TComponent* Owner,
				const UnicodeString &sSyntaxScheme,
				const UnicodeString &sText = L"",
				const UnicodeString &sUserPreCaption = L"");

	// закроет все открытые сохраненные редакторы, если что-то будет не сохранено, то вернёт ошибку
	static bool __fastcall CloseAssociatedSavedDockPanels(TComponent *AInstance);

	// проверяет, существует ли док. панель
	static bool __fastcall CheckSyntaxDockPanelExists(const UnicodeString &sInstancePropPath, const UnicodeString &sPropName);

	static bool __fastcall CheckSyntaxDockPanelExists(const UnicodeString &sActiveFileName);

	// создать можно только у компонента
	void __fastcall CreateSyntaxDockPanel(const UnicodeString &sInstancePropPath, const UnicodeString &sPropName);

	inline bool IsModified() { return EditDoc1->Modified; }

	__property TFrameSyntaxEditorEx *SyntaxEditor = {read=FSyntaxEditor};
	__property UnicodeString ActiveFileName = {read=FActiveFileName, write=SetActiveFileName};
	__property UnicodeString LinkCaption = {read=GetLinkCaption};
	__property UnicodeString LinkInstancePropPath = {read=FLinkInstancePropPath, write=SetLinkInstancePropPath}; // путь к Инстанс по именам объектов
	__property UnicodeString LinkProperty = {read=FLinkProperty, write=FLinkProperty};	// свойство, по которому будет доступ
	__property UnicodeString LinkUnit = {read=FLinkUnit};
	__property UnicodeString LinkInstance = {read=FLinkInstance};
	__property UnicodeString UserPreCaption = {read=GetUserPreCaption};

	bool IsFileMode;

	/* Events */
	TNotifyEvent OnUpdateCaption;
};

/************************/
/* Syntax Props Editors */
/************************/

// может использоваться как для обычных UnicodeString свойств, так и для TSyntaxString.XMLText
class TUnicodeStringSyntaxDialogEditorImpl : public TLMDStringPropEditor
{
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual void __fastcall Edit(void);
	virtual Lmdtypes::TLMDString GetSyntaxScheme(void)=0;
};

// автоопределение по SettingsData->ActiveSyntaxScheme
class TUnicodeStringSyntaxDialogEditor : public TUnicodeStringSyntaxDialogEditorImpl
{
protected:
	virtual Lmdtypes::TLMDString GetSyntaxScheme(void);
};

class TUnicodeStringTextDialogEditor : public TUnicodeStringSyntaxDialogEditorImpl
{
protected:
	virtual Lmdtypes::TLMDString GetSyntaxScheme(void) { return L"Text"; };
};

class TUnicodeStringLuaDialogEditor : public TUnicodeStringSyntaxDialogEditorImpl {
protected:
	virtual Lmdtypes::TLMDString GetSyntaxScheme(void) { return L"Lua"; };
};

extern bool __fastcall __EditUnicodeStringSyntaxProperty(TPersistent *AInstance, const UnicodeString &sPropName);

//---------------------------------------------------------------------------
//----------------------- TStringsSyntaxClassEditorImpl ---------------------
//---------------------------------------------------------------------------
class TStringsSyntaxClassEditorImpl : public TLMDClassPropEditor {
private:

protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
	virtual Lmdtypes::TLMDString GetSyntaxScheme(void)=0;
public:
	virtual void __fastcall Edit(void);
};

class TTextStringsClassEditor : public TStringsSyntaxClassEditorImpl {
protected:
	virtual Lmdtypes::TLMDString GetSyntaxScheme(void) { return L"Text"; };
};

class TIniStringsClassEditor : public TStringsSyntaxClassEditorImpl {
protected:
	virtual Lmdtypes::TLMDString GetSyntaxScheme(void) { return L"INI"; };
};

//---------------------------------------------------------------------------
//-----------------------------TSyntaxDockPanel -----------------------------
//---------------------------------------------------------------------------
class TSyntaxDockPanel : public TLMDDockPanel {
private:

	TSpeedButton *FCloseButton;

protected:
	virtual void __fastcall AdjustClientRect(Types::TRect &Rect);
	virtual	void __fastcall OnUpdateSyntaxFormCaption(TObject *Sender);
	virtual void __fastcall OnSyntaxPanelClose(TObject *Sender);
public:
	__fastcall virtual TSyntaxDockPanel(Classes::TComponent* AOwner, const UnicodeString &sSyntaxScheme, const UnicodeString &sText = L"");
	__fastcall virtual ~TSyntaxDockPanel(void);

    void __fastcall DockToGlobalRootZone(TComponent *AInstance);

	TFormSyntaxEditorForm *SyntaxForm;
};

//---------------------------------------------------------------------------
extern PACKAGE TFormSyntaxEditorForm *FormSyntaxEditorForm;
//---------------------------------------------------------------------------
#endif
