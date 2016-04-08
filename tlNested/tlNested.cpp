// tlNested.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
// #include <afxdllx.h>
// #include "NestedInterface.h"
#include "NestedLoop.h"
#include <string>


extern "C" __declspec( dllexport ) 
int NestedRun(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			   std::string VO, std::string &ppOutStruct,CResult **ppRst)
{
//	¹ÂÁ¢µãVOÉèÖÃ
//	NESTEDRUN
//	/DATA = FILE
//	/VARINPUT arglist
//	/MISSMETHOD DELRECORD
//	/WRAPDISTANCE DISTANCE(1)
//	/NESTEDCOUNT COUNT(1)
//	/METHOD [{DIVRCD**},{DIVFIELD}]		

	CNestedLoop nest;
	CNestedVO nestVO;
	
	nestVO.Compile(VO);
	
	(*ppRst) = nest.RunTrain(pData,pEnv,itt,&nestVO);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}
