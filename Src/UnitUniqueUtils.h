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

#ifndef UnitUniqueUtilsH
#define UnitUniqueUtilsH
//---------------------------------------------------------------------------

#include <vector>
#include <map>

#include <TeeTree.hpp>

class TScxmlBaseShape;
class TStateMachineConnection;

namespace Unique {

	typedef enum { cecNONE, cecOneObjectsIsNULL, cecClassTypeMismatch, cecStringsTextMismatch, cecOneOfCollectionIsNULL,
					cecCollectionCountMismatch, cecComponentNameMismatch, cecPropertyMismatch,
					cecParentsMismatch, cecOriginShapeNotFound, cecOriginConnectionNotFound, cecConnectionPointsMismatch } TCompareErrorCode;

	struct CompareError {
		UnicodeString ObjectName;
		UnicodeString PropName;
		UnicodeString Mismatch;
		TCompareErrorCode Code;

		CompareError(void) { Clear(); }

		void Clear(void) {
			ObjectName = L"";
			PropName = L"";
			Mismatch = L"";
			Code = cecNONE;
		}
	};

	typedef std::pair<UnicodeString, UnicodeString> TUniquePair;

	TUniquePair GetUniqueName(TComponent *AOwner, TMetaClass *AClass);

	void UpdateUniqueChildConnectionName(TTreeNodeShape *AShape);

	UnicodeString CreateUniqueSimpleText(TTreeNodeShape *AShape, TMetaClass *ABaseClass = __classid(TTreeNodeShape));

	bool CompareParentsName(TTreeNodeShape *AOrigin, TTreeNodeShape *AThis);

	bool CompareObjects(TObject *AObject1, TObject *AObject2, TStrings *ASkippedProperties, const UnicodeString &sPropPath,
		CompareError &ACompareError, bool bCompareWithReadonlyProps = false);

	void DeepCompareObjects(TObject *AObject1, TObject *AObject2, TStrings *ASkippedProperties, const UnicodeString &sPropPath,
		std::vector<CompareError> &AMismatch);

	bool CompareInheritedObjects(TObject *AObject1, TObject *AObject2, TStrings *ASkippedProperties, CompareError &ACompareError);

	void DeepCompareInheritedObjects(TObject *AObject1, TObject *AObject2, TStrings *ASkippedProperties, TStrings *AMismatch);

	void CompareInheritedShape(TScxmlBaseShape * ACheckShape, TCustomTree *AInheritedTree, Unique::CompareError &ACompareError);

	void DeepCompareInheritedShape(TScxmlBaseShape * ACheckShape, TCustomTree *AInheritedTree, std::vector<CompareError> &AMismatch);

	void CompareInheritedConnection(TStateMachineConnection * ACheckConnection, TCustomTree *AInheritedTree, Unique::CompareError &ACompareError);

	void DeepCompareInheritedConnection(TStateMachineConnection * ACheckConnection, TCustomTree *AInheritedTree, std::vector<CompareError> &AMismatch);

	bool CompareStreams(TStream *AStream1, TStream *AStream2);
}

#endif
