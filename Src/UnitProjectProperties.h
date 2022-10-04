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

#ifndef UnitProjectPropertiesH
#define UnitProjectPropertiesH
//---------------------------------------------------------------------------

#include <memory>
#include "UnitSettingsBase.h"
#include "UnitProjectTriggers.h"
#include "UnitTestingMsg.h"
#include "LMDInsPropEditors.hpp"
#include "UnitTestSettings.h"
#include "UnitPostBuildInterface.h"
#include <XMLDoc.hpp>

class TStateMachineProject;

typedef enum {pbtSaveAll,pbtSaveFlat,pbtPostBuild} TProjectBuildType;
typedef Set<TProjectBuildType, pbtSaveAll, pbtPostBuild> TProjectBuildTypes;

#define DEFAULT_PROJECT_BUILD_TYPES		(TProjectBuildTypes() << pbtSaveAll << pbtSaveFlat << pbtPostBuild)

class TProjectPostBuild: public IPostBuildInterface {
protected:
	TStateMachineProject *FProject;
	virtual UnicodeString __fastcall GetMacros(void) const;
public:
	__fastcall TProjectPostBuild(TStateMachineProject *AProject);
};

class TProjectProperties: public TSettingsBase
{
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);
	/// переопределение загрузки свойств (используетс€ дл€ чтени€ старых свойств)
	virtual bool __fastcall OnSerializeCustomGetProp(TObject *Sender, const UnicodeString &sPropName, _di_IXMLNode APropertyNode);
private:

	TTestingMsgItems *FFilteredMsgs;
	inline void __fastcall SetFilteredMsgs(TTestingMsgItems *val) { FFilteredMsgs->Assign(val); }

	TProjectTriggerItems *FTriggers;
	inline void __fastcall SetTriggers(TProjectTriggerItems *val) { FTriggers->Assign(val); }

	TProjectBuildTypes FBuildTypes;
	bool __fastcall IsBuildTypesStored() { return FBuildTypes != DEFAULT_PROJECT_BUILD_TYPES; }

	UnicodeString FCalibratorCfg;
	UnicodeString FCanserviceCfg;

	bool __fastcall IsCalibratorCfgStored(void) { return !FCalibratorCfg.IsEmpty(); }
	bool __fastcall IsCanserviceCfgStored(void) { return !FCanserviceCfg.IsEmpty(); }

	bool FDisableSameControls;

	TTestSettings *	FTestSettings;
	void __fastcall SetTestSettings(TTestSettings *val);

	TProjectPostBuild *	FProjectPostBuild;
	void __fastcall SetProjectPostBuild(TProjectPostBuild *val);

	UnicodeString FProjectDefines;

    bool FVerifyChecksums;

protected:

	TStateMachineProject *FProject;

public:		// User declarations

	__fastcall TProjectProperties(TComponent* Owner, TStateMachineProject *AProject);
	virtual __fastcall ~TProjectProperties();

	virtual void __fastcall Assign(TPersistent* Source);

   	// функци€ должна создать новый экземпл€р текущего класса
	virtual TComponent *__fastcall DoCreateDefaultInstance();

__published:
	__property UnicodeString			ProjectDefines= {read=FProjectDefines, write=FProjectDefines};
	__property TTestSettings *			TestSettings = {read=FTestSettings, write=SetTestSettings};
	__property TProjectTriggerItems *	Triggers = {read=FTriggers, write=SetTriggers};
	__property TProjectPostBuild *		ProjectPostBuild = {read=FProjectPostBuild, write=SetProjectPostBuild};
	__property TTestingMsgItems *		FilteredMsgs = {read=FFilteredMsgs, write=SetFilteredMsgs};
	__property UnicodeString 			CalibratorCfg = {read=FCalibratorCfg, write=FCalibratorCfg, stored=IsCalibratorCfgStored};
	__property UnicodeString 			CanserviceCfg = {read=FCanserviceCfg, write=FCanserviceCfg, stored=IsCanserviceCfgStored};
	__property TProjectBuildTypes 		BuildTypes = {read=FBuildTypes, write=FBuildTypes, stored=IsBuildTypesStored};
	__property bool 					DisableSameControls = {read=FDisableSameControls, write=FDisableSameControls, default=false };
	__property bool 					VerifyChecksums = {read=FVerifyChecksums, write=FVerifyChecksums, default=true };
};


#endif
