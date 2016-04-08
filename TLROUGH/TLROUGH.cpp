// tlRough.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
#include "RudeMathematics.h"
#include "../arithInterface/EnvParam.h"
#include <string>

extern "C" __declspec( dllexport ) 
int RoughRun(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 std::string VO, std::string  &ppOutStruct,CResult **ppRst)
{
	//	´Ö²Ú¼¯VOÉèÖÃ
	//	ROUGHRUN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARSPECINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD		
	
	
	CRudeMathematics rude;
	CRoughtVO rouVO;
	
	rouVO.Compile(VO);
	
	(*ppRst) = rude.RunTrain(pData,pEnv,itt,&rouVO);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}
