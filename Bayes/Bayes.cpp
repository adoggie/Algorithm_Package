// Bayes.cpp : Defines the initialization routines for the DLL.
//

//#include "stdafx.h"
// #include <afxdllx.h>
// #include "BayesDlg.h"
#include "../DataInterface/DataInterface.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../TLAdVN/Analyze.h"

#include "BNet.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"

#include "../arithInterface/EnvParam.h"
#include <string>
using namespace std;

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/*
static AFX_EXTENSION_MODULE BayesDLL = { NULL, NULL };

CList<CRect,CRect> rectrect;

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("BAYES.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(BayesDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(BayesDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("BAYES.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(BayesDLL);
	}
	return 1;   // ok
}
*/

extern "C" _declspec(dllexport) CResult * OnBayesAnalysize(CDataInterface *pDataInterface/*,CWnd *pParent*/)
{
	CResult *pResult = NULL;

//	CBayesDlg dlg(pDataInterface,pParent);
//	dlg.m_pResult = NULL;
//	if (dlg.DoModal()==IDOK)
//		pResult = dlg.m_pResult;
	return pResult;
}



extern "C" __declspec(dllexport) 
int BayesNetTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
						 string VO, string &ppOutStruct,CResult **ppRst)
{
	//	±´Ò¶Ë¹VOÉèÖÃ
	//	BAYESNET
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/MISSMETHOD DELRECORD
	//	/INDEX arglist
	
	CBNet	bNet;
	CBayesOutput *pOut = new CBayesOutput;
	CBayesVO  bayesVO;
	bayesVO.Compile(VO);

	*ppRst = bNet.Train(pData, pEnv, itt,&bayesVO, *pOut);

	if (*ppRst != NULL)
		(*ppRst)->Print();

	return 0;
}


extern "C" __declspec(dllexport) 
int BayesNetPred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
				string VO, string &ppOutStruct,CResult **ppRst)
{
	CBNet	bNet;
	TPredVO bysVO;
	CBayesOutput Out;

	bysVO.Compile(VO);
	Out.Import(ppOutStruct);

	*ppRst = bNet.Output(pData,pEnv,itt,&bysVO,&Out);
	
	return 0;
}


















