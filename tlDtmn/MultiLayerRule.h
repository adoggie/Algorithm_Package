// MultiLayerRule.h: interface for the CMultiLayerRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTILAYERRULE_H__70E02EFB_557E_4CD5_9E3C_75F0ABAEA81B__INCLUDED_)
#define AFX_MULTILAYERRULE_H__70E02EFB_557E_4CD5_9E3C_75F0ABAEA81B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SingleRuleReduction.h"

struct TTransRelation
{
	long lProductID;
	char szProductName[64];
	long lParentID;
	int  lLever;		
	TTransRelation()
	{
		memset(this,0,sizeof(TTransRelation));
	}
};



class AFX_EXT_CLASS CMultiLayerRule : public CSingleRuleReduction  
{
public:
	CMultiLayerRule();
	virtual ~CMultiLayerRule();

};

#endif // !defined(AFX_MULTILAYERRULE_H__70E02EFB_557E_4CD5_9E3C_75F0ABAEA81B__INCLUDED_)
