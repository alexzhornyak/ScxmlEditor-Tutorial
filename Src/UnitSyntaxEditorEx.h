//---------------------------------------------------------------------------

#ifndef UnitSyntaxEditorExH
#define UnitSyntaxEditorExH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDSedView.hpp"
#include "UnitSyntaxEditor.h"
#include <ActnList.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <StdActns.hpp>
#include <ToolWin.hpp>
#include <memory>
#include "CommonConsts.h"
//---------------------------------------------------------------------------

class TSpawnAndRedirect;

class TFrameSyntaxEditorEx : public TFrameSyntaxEditor
{
__published:	// IDE-managed Components
	TAction *actEditScript;
	TMenuItem *Editscript1;
	TMenuItem *Editscript2;
	TMenuItem *Luautils1;
	TMenuItem *asdf1;
	TAction *actLuaUtils;
	TFileSaveAs *actLuaConvertToBinaryChunk;
	TAction *actLuaCheckSyntax;
	TMenuItem *Checksyntax1;
	TAction *actLuaCheckDataSyntax;
	TMenuItem *Checksyntaxasdata1;
	TMenuItem *N4;
	TFileSaveAs *actLuaConvertToCPPBin;
	TMenuItem *ConverttoCPPbinaryluabuffer1;
	TMenuItem *Clearbreakpoints1;
	TMenuItem *MenuXMLUtils;
	TAction *actXMLCheckSyntax;
	TMenuItem *Checksyntax2;
	TAction *actXMLUtils;
	TAction *actXMLFormat;
	TMenuItem *FormatXML1;
	TMenuItem *MenuEcmautils;
	TMenuItem *MenuEcmaChecksyntax;
	TMenuItem *MenuEcmaChecksyntaxasdata;
	TAction *actEcmaUtils;
	TAction *actEcmaCheckSyntax;
	TAction *actEcmaCheckSyntaxData;
	void __fastcall actEditScriptExecute(TObject *Sender);
	void __fastcall actLuaUtilsUpdate(TObject *Sender);
	void __fastcall actLuaUtilsExecute(TObject *Sender);
	void __fastcall actLuaConvertToBinaryChunkAccept(TObject *Sender);
	void __fastcall actLuaCheckSyntaxExecute(TObject *Sender);
	void __fastcall actLuaConvertToCPPBinAccept(TObject *Sender);
	void __fastcall actBreakDeleteAllExecute(TObject *Sender);
	void __fastcall actXMLCheckSyntaxExecute(TObject *Sender);
	void __fastcall actXMLUtilsExecute(TObject *Sender);
	void __fastcall actXMLUtilsUpdate(TObject *Sender);
	void __fastcall actXMLFormatExecute(TObject *Sender);
	void __fastcall actEcmaUtilsExecute(TObject *Sender);
	void __fastcall actEcmaUtilsUpdate(TObject *Sender);
	void __fastcall actEcmaCheckSyntaxExecute(TObject *Sender);
private:	// User declarations
	UnicodeString FCaption;
	std::auto_ptr<TSpawnAndRedirect> FConvertLuacSpawnPtr;
	std::auto_ptr<TSpawnAndRedirect> FConvertEcmaSpawnPtr;

	void __fastcall OnSyntaxCheckBeforeLineRead(UnicodeString &sText);
	void __fastcall OnLuacConverted(TMessage &msg);

	void __fastcall CreateErrorBookmark(int iRow, int iOffset = -1);

    void __fastcall OnEcmaSyntaxCheckBeforeLineRead(UnicodeString &sText);

protected:
	virtual void __fastcall OnSelectSyntaxScheme(TObject *Sender);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_SCXML_LUAC_CONVERTED, TMessage, OnLuacConverted)
	END_MESSAGE_MAP(TFrameSyntaxEditor)

	bool IsXMLSyntaxValid(void);

public:		// User declarations
	__fastcall TFrameSyntaxEditorEx(TComponent* Owner);

	void __fastcall UpdateAllUtilsActions();

   	void __fastcall EditDocumentScript(const UnicodeString & sEditCaption);

	__property UnicodeString Caption = {read=FCaption,write=FCaption};
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameSyntaxEditorEx *FrameSyntaxEditorEx;
//---------------------------------------------------------------------------
#endif
