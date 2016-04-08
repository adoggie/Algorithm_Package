// TChartDecTree.h: interface for the CTChartDecTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTDECTREE_H__20747AFC_31D4_476D_B87A_E9839FF0FB91__INCLUDED_)
#define AFX_TCHARTDECTREE_H__20747AFC_31D4_476D_B87A_E9839FF0FB91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TChart1.h"

class AFX_EXT_CLASS CTChartDecTree : public CTChart  
{
public:
	CTChartDecTree();
	virtual ~CTChartDecTree();
	void SetDataofTree(CPtrArray *pNodeList);

};

#endif // !defined(AFX_TCHARTDECTREE_H__20747AFC_31D4_476D_B87A_E9839FF0FB91__INCLUDED_)
