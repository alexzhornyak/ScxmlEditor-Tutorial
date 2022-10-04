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

// #include "UnitTranslate.h"
#include "UnitStringEditorEx.h"
//#include "UnitClassInspectorEditors.h"
//#include "UnitSettingsPath.h"
//#include "UnitProtocolSettings.h"
//#include "UnitMinMaxValue.h"

#include "UnitRegisterDefaultEditors.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void __fastcall RegisterDefaultPropertyEditors(Lmdinspropinsp::TLMDCustomPropertyInspector *APropertyInspector) {
	APropertyInspector->RegisterPropEditor(__typeinfo(TStrings), 0, "", __classid(TMultiDisplayStringsEditorEx));
//	APropertyInspector->RegisterPropEditor(__typeinfo(TStringDisplayText), 0, "", __classid(TStringDisplayEditor));
//	APropertyInspector->RegisterPropEditor(__typeinfo(TSettingsPath), 0, "", __classid(TSettingsPathEditor));
}
