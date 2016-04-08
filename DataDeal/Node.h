// Node.h: interface for the CNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODE_H__DE8E4DD5_E8B9_4165_8963_59F2A28DAC28__INCLUDED_)
#define AFX_NODE_H__DE8E4DD5_E8B9_4165_8963_59F2A28DAC28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AnalyzeWord.h"
#include <stack>
#include <vector>
#include <string>

class CNode  
{
	std::vector<CNode*>m_Child;
	ItemType m_nodeType;
	std::string m_strNodeValue;
	static int GetLeftMaxOp(std::vector<CNode*>&copyAry);

public:

	void BackTrace(std::vector<CNode*>&stk);
	bool BuildBinaryTree();
	static int GetPrivilege(std::string &sOp);

	bool BuildTree(const CAnalyzeWord &aw,int nFrom,int nTo);
	void print(int nLevel =0);
	ItemType GetNodeType(){return m_nodeType;
	}
	std::string GetNodeValue(){return m_strNodeValue;
	}

	std::vector<CNode*> & GetChild(){
		return m_Child;
	}

	CNode(ItemType it,const std::string &strValue);
	virtual ~CNode();

protected:
	bool CheckChild();
	static int FindNestedOneOf(const CAnalyzeWord& aw,std::string strWord,int nFromPos=0);
	static int FindNestedBracketPos(const CAnalyzeWord& aw,int nFromPos=0);
};

class RItem
{
public:
	ItemType m_nodeType;
	std::string strValue;
	OperationType m_ot;
	double dbvalue;
	void copyfrom(CNode*pNode);
	RItem()
	{
		dbvalue  = 0;
		strValue = "";
	};
	~RItem()
	{
		dbvalue  = 0;
		strValue = "";
	}
	RItem(double value)
	{
		m_nodeType = itNode;
		dbvalue = value;
	}
};
#endif // !defined(AFX_NODE_H__DE8E4DD5_E8B9_4165_8963_59F2A28DAC28__INCLUDED_)
