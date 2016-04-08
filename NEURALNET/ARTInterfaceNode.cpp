//////////////////////////////////////////////////////////////////////
// ARTInterfaceNode.cpp                                             //
//																	//
// Define:        ART算法接口层结点类实现                           //
// Creator:       屈韬                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ARTInterfaceNode.h"

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

CARTInterfaceNode::CARTInterfaceNode() : CARTNode()
{
	m_nClusterNum = 0;
}


CARTInterfaceNode::~CARTInterfaceNode()
{
	
}

void CARTInterfaceNode::EstablishArrays( int nClusterNum )
{
	m_nClusterNum = nClusterNum;

	m_adInputValue.create(m_nClusterNum+1);
	m_adOutputValue.create(m_nClusterNum);
	m_adWeight.create(m_nClusterNum);
}


void CARTInterfaceNode::Initialize()
{
	int i;
  for(  i=0; i<m_nClusterNum; i++ )
  	  m_adWeight[i] = 1.0;
  
  for( i=0; i<m_nClusterNum+1; i++)
	  m_adInputValue[i] = 0.0;
  
  m_dActivation = 0.0;
}


void CARTInterfaceNode::CalculateOutputValue(int nGainOne)
{
	double dFeedbackSignal = 0.0, dNodeOutput, dTwoThirdsRule;
	int i;

	// calculate feedback signal through use of weighted sum
	for( i=0; i<m_nClusterNum; i++ )
		dFeedbackSignal += m_adWeight[i] * m_adInputValue[i+1];

	dTwoThirdsRule = dFeedbackSignal + m_adInputValue[0] + nGainOne;

	// use Two Thirds Rule to determine node output
	if( dTwoThirdsRule>=2.0 )
		dNodeOutput = 1.0;
	else 
		dNodeOutput = 0.0;

	// establish output vector to cluster units
	for( i=0; i<m_nClusterNum; i++)
		m_adOutputValue[i] = dNodeOutput;
}

double CARTInterfaceNode::GetOutputValue(int nCluster)
{
	return m_adOutputValue[nCluster];
}

void CARTInterfaceNode::RecomputeActivation(int nChampCluster)
{
	m_dActivation = m_adInputValue[0] * m_adWeight[nChampCluster];
}
