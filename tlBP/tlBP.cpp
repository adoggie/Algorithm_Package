// tlBP.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
// #include <afxdllx.h>
#include "BPNetInterface.h"
#include "../DataInterface/DataInterface.h"
#include "../Public/TString.h"
#include "../Public/FileReadWrite.h"


extern "C" __declspec( dllexport ) 
int BPNetTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{
	//	/BPNET
	//	/SAMPLEMODE [{PERCENT(80)};{FRONTSAMPLE(1000)}]
	//	/PARAM SYSTEMACUATE(0.001) TRAINCOUNT(100) LEARNRATE(0.1) HIDENEUCOUNT(4)
	//	/OUPUTTYPE [{CLASSIFY};{CALCULATE}]
	
	
	CBPNet  bpnet;
	
	(*ppRst) = bpnet.RunTrain(pData,pEnv,itt,VO);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	//导出模型
	if (bpnet.m_ModelFile.GetLength() < 1)
		return 0;
	bpnet.Export(bpnet.m_ModelFile.GetData());
	return 0;
}

extern "C" __declspec( dllexport ) 
int BPNetPred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			   string VO, string &ppOutStruct,CResult **ppRst)
{
//	BPPRED
//		/DATA=[FILE]
//		/VARINPUT [varlist]
//		/MODELFROMFILE [FILE]
	
	CBPNet  bpnet;

	(*ppRst) = bpnet.RunPred(pData,pEnv,itt,VO,ppOutStruct);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}

