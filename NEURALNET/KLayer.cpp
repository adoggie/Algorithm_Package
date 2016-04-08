//////////////////////////////////////////////////////////////////////
// KLayer.cpp:                                                      //
//																	//
// Define:        Kohonen算法Layer类实现                            //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KLayer.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKLayer::CKLayer( int nDimension, int nClusterNum )
{
	m_nNodeNum = nClusterNum;
	m_aKNode = NULL;

	m_aKNode = new CKNode[m_nNodeNum];

	for( int i=0; i<m_nNodeNum; i++ )
	{
		m_aKNode[i].EstablishArrays( nDimension );
		m_aKNode[i].Initialize();
	}
}

CKLayer::~CKLayer()
{
	if( m_aKNode!= NULL )
		delete[] m_aKNode;
}
