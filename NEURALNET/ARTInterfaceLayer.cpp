//////////////////////////////////////////////////////////////////////
// ARTInterfaceLayer.cpp                                            //
//																	//
// Define:        ART算法接口层类实现                               //
// Creator:       屈韬                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Topology.h"
#include "ARTInterfaceLayer.h"

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
CARTInterfaceLayer::CARTInterfaceLayer( int nDimension, int nClusterNum )
{
	m_nNodeNum = nDimension;
	m_nClusterNum = nClusterNum;
	m_nDimension = nDimension;

	m_aInterfaceNode = NULL;

	m_aInterfaceNode = new CARTInterfaceNode[m_nNodeNum];
	assert( m_aInterfaceNode!= NULL );

	for( int i=0; i<m_nNodeNum; i++)
	{
		m_aInterfaceNode[i].EstablishArrays( nClusterNum );
		m_aInterfaceNode[i].Initialize();
	}
}

CARTInterfaceLayer::~CARTInterfaceLayer()
{
	if( m_aInterfaceNode!=NULL )
		delete[] m_aInterfaceNode;
}

void CARTInterfaceLayer::SetLayerInputValue(CDoubleVector &pData )
{
	for( int i=0; i<m_nNodeNum; i++ )
	{
		m_aInterfaceNode[i].SetInputValue( 0, pData[i] );
		m_aInterfaceNode[i].SetActivation( pData[i] );
	}
}

void CARTInterfaceLayer::CalculateOutputValue(int nGainOne)
{
	for( int i=0; i<m_nNodeNum; i++ )
		m_aInterfaceNode[i].CalculateOutputValue( nGainOne );
}

double CARTInterfaceLayer::GetNodeOutputValue(int nDimension, int nCluster)
{
	return m_aInterfaceNode[nDimension].GetOutputValue( nCluster );
}

void CARTInterfaceLayer::RecomputeActivation(int nChampCluster)
{
	for( int i=0; i<m_nNodeNum; i++ )
		m_aInterfaceNode[i].RecomputeActivation( nChampCluster );
}

void CARTInterfaceLayer::OutputNodeWeight(CResult *pResult)
{
	CTLTable* pTable=new CTLTable;
	CTString string;
	int i, j;

	pTable->CreateTable( m_nClusterNum+1,m_nDimension+2 );
	string.Format( "%s", "Interface层权值表" );
	pTable->SetTitle(string);

	pTable->InsertColumn(0, "");
	for( i=0; i<m_nDimension; i++ )
	{
		string.Format( "矢量%d", i+1 );
		pTable->InsertColumn( i+1, string );
	}

	for( j=0; j<m_nClusterNum; j++ )
	{
		string.Format( "聚类%d", j+1 );
		pTable->InsertItem( j, string);
	
		for( i=0; i<m_nDimension; i++ )
			pTable->SetItemText( j, i+1, GetNodeWeight( i, j ));
	}

	CRsltElementTable *pVariable=new CRsltElementTable( "ART神经网络权值表", pTable );
	pResult->Add(pVariable);
	
}





















