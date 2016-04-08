// tlDecTree.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/FileReadWrite.h"

#include "DecTreeThread.h"
#include <string>
using namespace std;


extern "C" __declspec( dllexport ) 
int DecTreeTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{
	//	/DECTREE
	//	/VARLIST	[arglist]
	//	/VARLIST	[varlist]
	//	/GENTREEMETHOD	[{INFOGAIN};{GINIRED}]
	//	/PRUNMETHOD [{NOTPRUNE};{MAXINFOGAIN({0.06})};{MAXGINIRED({0.15})};{MAXDEPTH({5**})};{MAXERRORRATE({0.3})}]
	//  /GENTREECON MINSAMPLE({2**}) MAXBRANCH({3})
	
	CDecTreeThread decTree;
	CDecTreeVO decVO;
	decVO.Compile(VO);
	CDecTreeOutput *pOut=NULL;

	(*ppRst) = decTree.RunTrain(pData,pEnv,itt,&decVO,pOut);
	if (*ppRst != NULL)
		(*ppRst)->Print();

	if (pOut)
	{
		pOut->Export(string(decVO.strModelName));
		delete pOut;
	}
	return 0;
}


extern "C" __declspec( dllexport ) 
int DecTreePred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			string VO, string &ppOutStruct,CResult **ppRst)
{
	CDecTreeThread decTree;
	TPredVO decVO;
	decVO.Compile(VO);
	CDecTreeOutput Out;
	decTree.InitInstance(Out);
	if (!Out.Import(decVO.m_ModelFile.GetData()))
		return 0;

	(*ppRst) = decTree.RunPred(pData,pEnv,itt,&decVO,&Out);
	if (ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}