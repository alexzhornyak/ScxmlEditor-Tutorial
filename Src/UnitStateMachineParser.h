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

#ifndef UnitStateMachineParserH
#define UnitStateMachineParserH
//---------------------------------------------------------------------------

#include "TeeTree.hpp"

#include "UnicodeStringHash.h"

class TScxmlBaseShape;
class TStateMachineEditor;
class TStateMachineConnection;

namespace Statemachine {

	typedef enum {
		istAttributes, istXmlText, istComments, istInvisibleChilds, istVisibleChilds, istConnections, istExtraContent, istMAXSIZE
	}TIterateSaveType;
	typedef Set<TIterateSaveType, istAttributes, istMAXSIZE>TIterateSaveTypes;

	TIterateSaveTypes MaxPossibleTypes() {
		TIterateSaveTypes ATypes;
		for (int i = 0; i < istMAXSIZE; i++) {
			ATypes = ATypes << TIterateSaveType(i);
		}
		return ATypes;
	}

	void SyntaxXMLCheck(const UnicodeString &sScxmlData);

	UnicodeString GetRawScxml(TCustomTree *ATree, bool bSkipComments);

	void SaveTreeToScxml(TCustomTree *ATree, const UnicodeString &sFileName,
		bool bAppendGui=true, bool bSkipVersionEncoding=false, const TIterateSaveTypes ASaveTypes = TIterateSaveTypes() << istMAXSIZE);
	void SaveTreeToScxml(TCustomTree *ATree, Classes::TStream *AStream,
		bool bAppendGui=true, bool bSkipVersionEncoding=false, const TIterateSaveTypes ASaveTypes = TIterateSaveTypes() << istMAXSIZE); /* overload */

	void SaveScxmlAsFlat(const UnicodeString &sFileName, const UnicodeString &sFlatFileName);

	void SaveInheritedToScxml(TCustomTree *AEditorRoot, TCustomTree *AEditorAncestor, TStream *AStream, const UnicodeString &sInherited);

	void LoadTreeFromScxml(TCustomTree *&ATree, const UnicodeString &sFileName);
	void LoadTreeFromScxmlData(TCustomTree *&ATree, const UnicodeString &sScxmlData);

	void LoadTreeFromEditor(TStateMachineEditor *ATreeEditor, const UnicodeString &sFileName);

	void LoadTreeFromScxml(TCustomTree *&ATree, const UnicodeString &sFileName, TCustomTree *ATreeAncestor);
	void LoadTreeFromScxmlData(TCustomTree *&ATree, const UnicodeString &sScxmlData, TCustomTree *ATreeAncestor);

	UnicodeString InvisibleShapeChildrenToXMLString(TScxmlBaseShape *AShape);
	void XMLStringToInvisibleShapeChildren(TScxmlBaseShape *AShape, const UnicodeString &sXMLString);

	/* INHERITANCE */
	void CheckAndFixInheritedParent(TComponent *ABaseOwner, TScxmlBaseShape *AScxmlBaseShape, TScxmlBaseShape * AOriginShape);

	void SetInheritance(TScxmlBaseShape *AScxmlBaseShape, bool bSet, bool bWithConnections=true);

	void SetInheritance(TStateMachineConnection *AConnection, bool bSet); /* overload */

	extern UnicodeString LastError;
}

#endif
