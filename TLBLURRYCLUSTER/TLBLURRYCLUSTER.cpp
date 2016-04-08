// tlBlurryCluster.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
// #include <afxdllx.h>
#include "BlurryInterface.h"
#include "../arithInterface/EnvParam.h"


/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


/*
static AFX_EXTENSION_MODULE TlBlurryClusterDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("TLBLURRYCLUSTER.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(TlBlurryClusterDLL, hInstance))
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

		new CDynLinkLibrary(TlBlurryClusterDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("TLBLURRYCLUSTER.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(TlBlurryClusterDLL);
	}
	return 1;   // ok
}
*/

/*
#define  AFX_EXT_API __declspec(dllexport)

CTArray<CBlurryInterface*, CBlurryInterface*> g_arComponent;

extern "C" AFX_EXT_API bool CreateComponent(UINT nComponentID, 
											CTString& strProjectPath, 
											bool bNewComponent)
{
	CBlurryInterface* object = new CBlurryInterface( bNewComponent);

	g_arComponent.Add( object );

	return true;
}

extern "C" AFX_EXT_API bool DeleteComponent( UINT nComponentID )
{
	for( int i=0; i<g_arComponent.GetSize(); i++ )
		if( g_arComponent[i]->GetComponentID()==nComponentID )
		{
			delete g_arComponent[i];
			g_arComponent.RemoveAt( i );
			return true;
		}

	return true;
}

extern "C" AFX_EXT_API void Free()
{
	for( int i=0; i<g_arComponent.GetSize(); i++ )
		delete g_arComponent[i];

	g_arComponent.RemoveAll();
}

/ *
extern "C" AFX_EXT_API CPropertyPage * GetPropertyPage( UINT nComponentID )
{
	for( int i=0; i<g_arComponent.GetSize(); i++ )
		if( g_arComponent[i]->GetComponentID()==nComponentID )
		{
			if( g_arComponent[i]->m_pPageBlurry!=NULL )
				return g_arComponent[i]->m_pPageBlurry;
		}

	return NULL;
}
* /


extern "C" AFX_EXT_API bool Run( UINT ID,
								CDataInterface *pDataInterface,
								CPtrArray *pPtrArray/ *,
								CWnd * pMsgWnd, CWnd *pProcedureWnd* /)
{
	for( int i=0; i<g_arComponent.GetSize(); i++ )
		if( g_arComponent[i]->GetComponentID()==ID )
		{
			g_arComponent[i]->Run(pDataInterface,pPtrArray/ *,pMsgWnd ,pProcedureWnd* /);

			return true;
		}

	return false;
}

extern "C" AFX_EXT_API bool SetDataSource( UINT ID, CDataInterface * pData, CPtrArray & arFieldRole )
{
	for( int i=0; i<g_arComponent.GetSize(); i++ )
		if( g_arComponent[i]->GetComponentID()==ID )
		{
			g_arComponent[i]->SetDataSource( pData, &arFieldRole );
			return true;
		}

	return false;
}

extern "C" AFX_EXT_API void Abort(UINT ID)
{
		for( int i=0; i<g_arComponent.GetSize(); i++ )
		if( g_arComponent[i]->GetComponentID()==ID )
		{
			g_arComponent[i]->SetAbortAttib();
		}
}
*/

extern "C" __declspec( dllexport ) 
int BlurryRun(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 std::string VO, std::string &ppOutStruct, CResult **ppRst)
{
	//	/BLURRY
	//	/LEVEL [{VARMAX};{VARDIFF}]
	//	/OUTPUT [{ICICLE};{PEDIGREE}]
	//	/PARAM CREDIT({0.8})
	
	CBlurryInterface blurry;
	CBlurryVO blurryVO;
	
	blurryVO.Compile(VO);
	
	(*ppRst) = blurry.RunTrain(pData,*pEnv,itt,&blurryVO);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	return 0;
}
