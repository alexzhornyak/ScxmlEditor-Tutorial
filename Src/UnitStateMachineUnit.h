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

#ifndef UnitStateMachineUnitH
#define UnitStateMachineUnitH
//---------------------------------------------------------------------------

#include <set>
#include <vector>
#include <LMDXML.hpp>
#include <Lmdideprojmgr.hpp>
#include "UnitProjectProperties.h"
#include "UnitPostBuildInterface.h"

typedef enum {smutNormal, smutVirtual} TStateMachineUnitType;

// ---------------------------------------------------------------------------
class TStateMachineDockPanel;

class TStateMachineEditorUnit;

/* TUnitPostSave */
class TUnitPostSave: public IPostBuildInterface {
protected:
	TStateMachineEditorUnit *FUnit;
	virtual UnicodeString __fastcall GetMacros(void) const;
public:
	__fastcall TUnitPostSave(TStateMachineEditorUnit *AUnit);
};

/* TStateMachineEditorUnit */
class TStateMachineEditorUnit : public Lmdideprojmgr::TLMDProjDocument
{
private:

	bool FEnabled; // сделаем возможность исключить

	TStateMachineProject * __fastcall GetStateMachineProject(void);

	UnicodeString FScxmlName;
	UnicodeString __fastcall GetScxmlName(void);
	inline UnicodeString __fastcall GetScxmlNameFromXML(void) { return FScxmlName; } // показывает, что у нас реально в XML сохранено

	_di_ILMDXmlDocument FBindingsDoc;

	UnicodeString __fastcall GetBindingsName(void);

	ILMDXmlElement * __fastcall GetBindingsContainerElement();

	TUnitPostSave *	FPostSave;

	UnicodeString FInherited;

	inline bool __fastcall GetIsInherited(void) { return !FInherited.IsEmpty(); }

	bool FBindingsDisabled;

    bool FIsBuildRoot;

	TStateMachineUnitType FStateMachineUnitType;

	inline bool __fastcall GetIsVirtual(void) { return FStateMachineUnitType == smutVirtual; }

	TStringList *FVirtualDependencies;
	inline void __fastcall SetVirtualDependencies(TStringList *val) { FVirtualDependencies->Assign(val); }

	inline UnicodeString __fastcall GetFileName(void) { return ExtractFileName(FilePath); }

	inline UnicodeString __fastcall GetPath(void) { return ExtractFilePath(FilePath); }

	bool __fastcall GetIsBase(void);

protected:
	virtual void __fastcall DrawImage(Graphics::TCanvas* ACanvas, const Types::TRect &ARect);
	virtual void __fastcall DoRead(Lmdxml::_di_ILMDXmlElement AElem, bool AIsParentXml);
	virtual void __fastcall DoWrite(Lmdxml::_di_ILMDXmlElement AElem, bool AIsParentXml);
	virtual void __fastcall DoLoad(const Lmdtypes::TLMDString AFilePath);
	virtual void __fastcall DoSave(const Lmdtypes::TLMDString AFilePath);
	virtual Classes::TStream* __fastcall DoOpenFileStream(const Lmdtypes::TLMDString AFileIdent);
	virtual Lmdtypes::TLMDString __fastcall DoGetDefaultBaseName(void);
	virtual Lmdtypes::TLMDString __fastcall DoGetFileExt(void);
	virtual Lmdtypes::TLMDString __fastcall DoGetDisplayName(void);

	virtual bool __fastcall GetEnabled(void);

	// им€ проекта мен€ем на ".bindings.xml"
	static UnicodeString MakeBindingFileName(const UnicodeString &sFileOrigin);

	friend class TFormScxmlGui;
public:
	__fastcall TStateMachineEditorUnit(Lmdideprojmgr::TLMDProjectManager* AProjMgr, Lmdideprojmgr::TLMDProjNode* AParent);
	__fastcall virtual ~TStateMachineEditorUnit(void);

   std::vector<TStateMachineEditorUnit*> __fastcall GetInheritedUnitsFromThis(void);

   std::set<TStateMachineEditorUnit *> __fastcall GetVirtualParents(void);

   UnicodeString __fastcall GetScxmlNameFromSource(const UnicodeString &sPath);

   TStateMachineDockPanel *StateMachineDockPanel;

   static const Lmdtypes::TLMDString FileExt_;

   static const UnicodeString BindingsSuffix_;

   static const UnicodeString OpenDialogFilter;

   static const UnicodeString DefaultUnitBaseName;

   static const UnicodeString DefaultVirtualUnitBaseName;

   __property bool 						Enabled = { read=GetEnabled, write=FEnabled, default=true };

   // если открыт редактор, то ScxmlName из редактора, иначе из XML
   __property UnicodeString 			ScxmlName = {read=GetScxmlName, write=FScxmlName};

   __property UnicodeString 			FileName = {read=GetFileName};

   // показывает, что у нас реально в XML сохранено
   __property UnicodeString 			ScxmlNameFromXML = {read=GetScxmlNameFromXML};

   __property UnicodeString 			BindingsName = {read=GetBindingsName};

   __property ILMDXmlElement *			BindingsContainer = {read=GetBindingsContainerElement};

   __property TStateMachineProject * 	StateMachineProject = {read=GetStateMachineProject};

   __property TUnitPostSave *			PostSave = {read=FPostSave, write=FPostSave};

   __property UnicodeString				Inherited = {read=FInherited, write=FInherited};

   __property bool						IsBuildRoot = {read=FIsBuildRoot, write=FIsBuildRoot, default=false};

   __property bool 						IsInherited = {read=GetIsInherited};

   __property bool						IsVirtual = {read=GetIsVirtual};

   __property bool						IsBase = {read=GetIsBase};

   __property bool 						BindingsDisabled = { read=FBindingsDisabled, write=FBindingsDisabled, default=false };

   __property TStateMachineUnitType 	StateMachineUnitType = {read=FStateMachineUnitType, write=FStateMachineUnitType, default=smutNormal};

   __property TStringList *				VirtualDependencies = {read=FVirtualDependencies, write=SetVirtualDependencies};

   __property UnicodeString				Path = {read=GetPath}; // путь к файлу с обратным слешем
};

class TStateMachineProject : public Lmdideprojmgr::TLMDProjDocument
{
	typedef Lmdideprojmgr::TLMDProjDocument inherited;

private:
	TProjectProperties *FProjectProperties;
	void __fastcall SetProjectProperties(TProjectProperties *val) { FProjectProperties->Assign(val); }

	void __fastcall ValidateControlBindings();

	TStateMachineEditorUnit *__fastcall GetRootForBuild(void);
	TStateMachineEditorUnit * __fastcall GetProjectRoot(void);

	int FLoadError;

protected:
	virtual void __fastcall DrawImage(Graphics::TCanvas* ACanvas, const Types::TRect &ARect);
	virtual void __fastcall DoRead(Lmdxml::_di_ILMDXmlElement AElem, bool AIsParentXml);
	virtual void __fastcall DoWrite(Lmdxml::_di_ILMDXmlElement AElem, bool AIsParentXml);
	virtual void __fastcall DoLoad(const Lmdtypes::TLMDString AFilePath);
	virtual void __fastcall DoSave(const Lmdtypes::TLMDString AFilePath);
	virtual Lmdtypes::TLMDString __fastcall DoGetDefaultBaseName(void);
	virtual Lmdtypes::TLMDString __fastcall DoGetFileExt(void);

	UnicodeString __fastcall GetPropertiesFileName(const UnicodeString sFilePath=L"");
	void __fastcall SaveProperties(const UnicodeString &sFileName);

	friend class TFormScxmlGui;
public:
	__fastcall TStateMachineProject(Lmdideprojmgr::TLMDProjectManager* AProjMgr, Lmdideprojmgr::TLMDProjNode* AParent);
	__fastcall virtual ~TStateMachineProject(void);

	_di_ILMDXmlDocument ControlsXMLDoc;

    // выдает предупреждение, если одинаковые имена
	bool __fastcall CheckAllUnitsForSameScxmlNames(void);

	// собирает все юниты, которые унаследованы от кого-то
	void CollectInheritedUnits(std::vector<TStateMachineEditorUnit*> &AVecUnits);

	UnicodeString __fastcall GetControlBindingFileName(const UnicodeString sFilePath=L"");
	void __fastcall SaveControlBindings(const UnicodeString &sFileName);

	void __fastcall SaveProperties();

	/* доступные свойства */
	__property TProjectProperties *ProjectProperties = {read=FProjectProperties, write=SetProjectProperties};

	// это строго файл отмеченный галочкой RootForBuild
	__property TStateMachineEditorUnit *RootForBuild = {read=GetRootForBuild};

	// здесь проверитс€ в первую очередь файл отмеченный галочкой, если не найдет, тогда первый доступный
	__property TStateMachineEditorUnit *ProjectRoot = {read=GetProjectRoot};

	__property int LoadError = {read=FLoadError};  // вводим свойство об ошибках при загрузке

	/* статические свойства */
	static const Lmdtypes::TLMDString FileExt_;

	static const UnicodeString OpenDialogFilter;

	static const int LOAD_ERROR_CHECKSUM_MISMATCH = 1;
};

extern TStateMachineUnitType GlobalStateMachineUnitType;
// эти методы нельз€ вызывать из PropertyInspector!!!
// так как они могут переключить на другую панель и начать мен€ть содержимое инспектора
extern TStateMachineEditorUnit *__fastcall GlobalAddExistingStateMachineUnit(TLMDProjNode *AParent, const UnicodeString &sFileName, bool bCheckIsOpened, const bool bCollectBindings = true);
extern TStateMachineEditorUnit *__fastcall GlobalAddNewStateMachineUnit(TLMDProjNode *AParent);
extern TStateMachineEditorUnit *__fastcall GlobalFindAndAddExistingUnit(TLMDProjNode *AParent, const UnicodeString &sFileName, bool bCheckIsOpened, const bool bCollectBindings = true);


extern void __fastcall GlobalPostAddOrOpenExistingUnit(TLMDProjNode *AParent, const UnicodeString &sFileName);

/* —март-—труктура */
struct GlobalVirtualUnitLock {
	GlobalVirtualUnitLock() {
		GlobalStateMachineUnitType = smutVirtual;
	}
	~GlobalVirtualUnitLock() {
		GlobalStateMachineUnitType = smutNormal;
	}
};

extern void CollectAllEnabledUnits(Lmdideprojmgr::TLMDProjDocument *ARoot, std::vector<TStateMachineEditorUnit*> &AVecUnits);

extern TStateMachineEditorUnit *FindUnitByFileName(Lmdideprojmgr::TLMDProjDocument *ARoot, const UnicodeString &sFileName);

extern TStateMachineEditorUnit *FindUnitByFilePath(Lmdideprojmgr::TLMDProjDocument *ARoot, const UnicodeString &sFilePath);

extern void GlobalFullExpandProjectView(void); // закрыты все механизмы дл€ разворачивани€ нод, поэтому только глобально раскрываем

extern TStateMachineEditorUnit* __fastcall GlobalActiveUnit();

extern TStateMachineProject * __fastcall GlobalStateMachineProject();

extern bool __fastcall IsScxmlNameUnique(TStateMachineEditorUnit *AUnit, const UnicodeString &sNewName);

extern UnicodeString __fastcall GetUniqueScxmlName(TStateMachineEditorUnit *AUnit);

extern bool __fastcall IsUnitModifiedRecursive(Lmdideprojmgr::TLMDProjNode *ARoot);

extern void GetUnitVirtualParents(const std::vector<TStateMachineEditorUnit*> &AVecUnits, TStateMachineEditorUnit *AUnit,
	std::set<TStateMachineEditorUnit *> &AVirtualParents);

#endif
