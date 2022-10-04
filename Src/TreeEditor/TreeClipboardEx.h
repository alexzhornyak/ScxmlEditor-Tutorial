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

#ifndef TreeClipboardExH
#define TreeClipboardExH
//---------------------------------------------------------------------------
#include <set>
#include <vector>
#include "TeeTree.hpp"

class TTreeClipboardEx : public TCustomTree {
private:
	TCustomTree *FIsCut;
protected:

public:
	__fastcall TTreeClipboardEx(Classes::TComponent* AOwner);
	virtual __fastcall ~TTreeClipboardEx();

	void __fastcall Copy(TCustomTree *SourceTree);
	void __fastcall Cut(TCustomTree *SourceTree);
	bool __fastcall Paste(TCustomTree *DestTree, bool UseCursorPos);

};

void CloneSelectedShapes(TCustomTree *SourceTree, std::vector<TTreeNodeShape*> &VecNewShapes);
void CloneBrotherShapes(TComponent *AOwner, TCustomTree *SourceTree, TCustomTree *TargetTree, TTreeNodeShape *AParent,
						const std::set<TTreeNodeShape*> &AShapes, std::set<TTreeNodeShape*> &NewShapes);

extern System::Word CF_CUSTOM_TREE;

#endif
