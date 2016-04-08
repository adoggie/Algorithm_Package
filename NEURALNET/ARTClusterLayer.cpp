//////////////////////////////////////////////////////////////////////
// ARTClusterLayer.cpp                                              //
//																	//
// Define:        ART�㷨�������ʵ��                               //
// Creator:       ���                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Topology.h"
#include "ARTClusterLayer.h"

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

CARTClusterLayer::CARTClusterLayer( int nDimension, int nClusterNum )
{
	m_nNodeNum = nClusterNum;
	m_nDimension = nDimension;

	m_aClusterNode = new CARTClusterNode[m_nNodeNum];
	assert( m_aClusterNode!= NULL );

	for( int i=0; i<m_nNodeNum; i++)
	{
		m_aClusterNode[i].EstablishArrays( m_nDimension );
		m_aClusterNode[i].Initialize();
	}

}

CARTClusterLayer::~CARTClusterLayer()
{
	if( m_aClusterNode!=NULL )
		delete[] m_aClusterNode;
}

double CARTClusterLayer::CalculateNetInput(int nCluster)
{
	return m_aClusterNode[nCluster].CalculateNetInput();
}

void CARTClusterLayer::EstablishNodeOutputValue(int nIndex)
{
	m_aClusterNode[nIndex].EstablishOutputValue();
}

void CARTClusterLayer::SetLayerInputValue(CDoubleVector &pData)
{
	int clu, dim;

	for( dim=0; dim<m_nDimension; dim++)
		for( clu=0; clu<m_nNodeNum; clu++ )
			m_aClusterNode[clu].SetInputValue( dim,pData[dim] );
}

void CARTClusterLayer::OutputNodeWeight(CResult *pResult)
{
	CTLTable* pTable = new CTLTable;
	CTString string;
	int i, j;

	pTable->CreateTable(m_nNodeNum+1, m_nDimension+2);
	string.Format( "%s", "Cluster��Ȩֵ��" );
	pTable->SetTitle(string);

	pTable->InsertColumn(0, "");
	for( i=0; i<m_nDimension; i++ )
	{
		string.Format( "ʸ��%d", i+1 );
		pTable->InsertColumn( i+1, string );
	}

	for( j=0; j<m_nNodeNum; j++ )
	{
		string.Format( "����%d", m_aClusterNode[j].GetClusterTag() );
		pTable->InsertItem( j, string);
	
		for( i=0; i<m_nDimension; i++ )
			pTable->SetItemText( j, i+1, GetNodeWeight( j, i ));
	}

	CRsltElementTable *pVariable=new CRsltElementTable( "ART������Cluster��Ȩֵ��", pTable );
	pResult->Add(pVariable);

}





















