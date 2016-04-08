//////////////////////////////////////////////////////////////////////
// ARTClusterNode.cpp                                               //
//																	//
// Define:        ART算法聚类层结点类实现                           //
// Creator:       屈韬                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ARTClusterNode.h"

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

CARTClusterNode::CARTClusterNode() : CARTNode()
{
	m_nDimension = 0;
	m_nClusterTag = 0;
}

CARTClusterNode::~CARTClusterNode()
{

}

void CARTClusterNode::EstablishArrays( int nNumber)
{
	m_nDimension = nNumber;

	m_adInputValue.create(m_nDimension);
	m_adOutputValue.create(m_nDimension+1);
	
	///////???????//////////////
	m_adWeight.create(m_nDimension); 
	///////???????//////////////
}

void CARTClusterNode::Initialize()
{
	int i;
	for(  i=0; i<m_nDimension; i++ )
		m_adWeight[i] = 1.0 / (1.0 + m_nDimension);

 /***************????????**************/
	for( i=0; i<m_nDimension; i++ )
		m_adInputValue[i] = 0.0;
/***************??????????**************/
	m_dActivation = 0.0;
}

double CARTClusterNode::CalculateNetInput()
{
	double dNetInput = 0.0;

	for(int n=0; n<m_nDimension; n++)
		dNetInput += m_adInputValue[n] * m_adWeight[n];
		
	return dNetInput;
}

void CARTClusterNode::EstablishOutputValue()
{
	for( int i=0; i<m_nDimension; i++ )
		if( m_dActivation>=0.0 )
			m_adOutputValue[i] = m_dActivation;
		else
			m_adOutputValue[i] = 0.0;
}

