#ifndef __TREE_NODE_LIST_ACCESS__
#define __TREE_NODE_LIST_ACCESS__

#include "TeeTree.hpp"

class TNodeListAccess : public TNodeShapeList {
public:
	void __fastcall Exchange(int Index1, int Index2) {
		TNodeShapeList::Exchange(Index1, Index2);
	}
	void __fastcall Move(int CurIndex, int NewIndex) {
		TNodeShapeList::Move(CurIndex, NewIndex);
	}
};

class TConnectionListAccess : public TNodeConnectionList {
public:
	void __fastcall Exchange(int Index1, int Index2) {
		TNodeConnectionList::Exchange(Index1, Index2);
	}
	void __fastcall Move(int CurIndex, int NewIndex) {
		TNodeConnectionList::Move(CurIndex, NewIndex);
	}
};

#endif
