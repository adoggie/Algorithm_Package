// TChartDecTree.cpp: implementation of the CTChartDecTree class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartDecTree.h"
#include "DecTreeWnd.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartDecTree::CTChartDecTree()
{
	m_ChartKind = DecTree;
}

CTChartDecTree::~CTChartDecTree()
{

}

void CTChartDecTree::SetDataofTree(CPtrArray *pArrNode)
{
	ManagerNode Manager;
	//决策树数据
	Manager.SetData(pArrNode);
	//得到图形数据
	tagDataPointDecTree *pDecTree = new tagDataPointDecTree;
	Manager.GetData(pDecTree);
	//Data
	SetData(pDecTree,1,EDecTree,FriArr);
	delete pDecTree;
	pDecTree = NULL;
}