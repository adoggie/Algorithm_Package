// tlDtmn.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
//#include <afxdllx.h>
#include "SingleRuleReduction.h"
//#include "MultiDimRule.h"
//#include "Association.h"

extern "C" __declspec( dllexport ) 
int DtmnRun(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			  string VO, string &ppOutStruct, CResult **ppRst)
{
	//	关联规则VO设置
	//	DTMNRUN
	//	/DATA = FILE
	//	/VARINPUT arglistg
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/ASSOTYPE [{SINGLE**},{MULTI}]
	//	/MINSUPPORT SUPPORT(0.1**)
	//	/ MINCREDIT CREDIT (0.5**)		
	
	CSingleRuleReduction assco;
	
	(*ppRst) = assco.Run(pData,pEnv,itt,VO);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}

