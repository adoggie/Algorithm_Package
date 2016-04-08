//////////////////////////////////////////////////////////////////////
// NeuralNetDLL.cpp                                                 //
//																	//
// Define:        DLL初始化例程实现                                 //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NeuralNetInterface.h"
#include "../Public/FileReadWrite.h"

/*
#include <afxdllx.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/*

static AFX_EXTENSION_MODULE NeuralNetDLLDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("NEURALNETDLL.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(NeuralNetDLLDLL, hInstance))
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

		new CDynLinkLibrary(NeuralNetDLLDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("NEURALNETDLL.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(NeuralNetDLLDLL);
	}
	return 1;   // ok
}
*/

/*
vector<CNeuralNetInterface*> g_arComponent;

extern "C" / *AFX_EXT_API* / bool CreateComponent(unsigned int nComponentID, CTString& strProjectPath, bool bNewComponent )
{
	CNeuralNetInterface* object = new CNeuralNetInterface( nComponentID, strProjectPath, bNewComponent );

	g_arComponent.push_back( object );

	return true;
}

extern "C" / *AFX_EXT_API* / bool DeleteComponent( unsigned int nComponentID )
{
	for( int i=0; i<g_arComponent.size(); i++ )
		if( g_arComponent[i]->GetComponentID()==nComponentID )
		{
			delete g_arComponent[i];
			g_arComponent.erase(g_arComponent.begin() + i );
			return true;
		}

	return true;
}

extern "C" / *AFX_EXT_API* / void Free()
{
	for( int i=0; i<g_arComponent.size(); i++ )
		delete g_arComponent[i];

	g_arComponent.clear();
}

extern "C" / *AFX_EXT_API* / bool SetDataSource( unsigned int ID, CDataInterface * pData, const vector<void *> & arFieldRole )
{
	for( int i=0; i<g_arComponent.size(); i++ )
		if( g_arComponent[i]->GetComponentID()==ID )
		{
			return g_arComponent[i]->SetDataSource( pData, arFieldRole );
		}

	return false;
}

extern "C" / *AFX_EXT_API* / bool Run( unsigned int ID)
{
	for( int i=0; i<g_arComponent.size(); i++ )
		if( g_arComponent[i]->GetComponentID()==ID )
		{
			g_arComponent[i]->Run();

			return true;
		}

	return false;
}

extern "C" / *AFX_EXT_API* / bool Abort( unsigned int ID )
{
	for( int i=0; i<g_arComponent.size(); i++ )
		if( g_arComponent[i]->GetComponentID()==ID )
		{
			g_arComponent[i]->Abort();

			return true;
		}

	return false;
}
*/


extern "C" __declspec( dllexport ) 
int NeuralNetTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{
	//神经网络VO设置
	//	NEURALNETTRAIN
	//	/DATA=[FILE]
	//	/VARINPUT = arglist
	//	/MISSMETHOD [{DELRECORD},{AVEFILL}]
	//	/NETSTRUCTURE =  [{ART},{KOHONEN},{HOPFIELD**}]
	//	/PROPERTY = WARNINGRATE(0.5) MAXCLUSTER(5) MAXLEARNEDRATE(0.4) MINLEARNEDRATE(0.01)
	//	/MAXRUNTIME = [{10MIN},{1HUR},{2HUR},{7HUR},{1DAY},{2DAY},{7DAY},{INF}]
	//	/TRAINTIME = TIME(50000)
	
	CNeuralNet  neural;
	CNeuralNetVO neuVO;
	CNeuralNetOutput *pOut=NULL;
	
	neuVO.Compile(VO);
	
	(*ppRst) = neural.RunTrain(pData,*pEnv,*itt,&neuVO,pOut);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	pOut->Export(neuVO.strModelName.GetData());
	delete pOut;
	pOut = NULL;
	return 0;
}


extern "C" __declspec( dllexport ) 
int NeuralNetPred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			string VO, string &ppOutStruct,CResult **ppRst)
{
	CNeuralNet  neural;
	TPredVO neuVO;
	CNeuralNetOutput Out;
	
	neuVO.Compile(VO);
	if (neuVO.m_ModelFile.GetLength() > 2)
	{	
		Out.Import(neuVO.m_ModelFile.GetData());
	}
	else
	{
		return 0;
	}
	//else
	//	Out.Import(ppOutStruct);//没有接口
	
	(*ppRst) = neural.RunPred(pData,*pEnv,*itt,&neuVO,&Out);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}


