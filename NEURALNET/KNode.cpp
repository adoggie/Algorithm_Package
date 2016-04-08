//////////////////////////////////////////////////////////////////////
// KNode.cpp:                                                       //
//																	//
// Define:        Kohonen�㷨�ڵ���ʵ��                             //
// Creator:       ���                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KNode.h"
#include "Bedlam.h"

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

CKNode::CKNode()
{
	m_dKOutput = 0.0;
	m_nDimension = 0;
}

CKNode::~CKNode()
{
	resetRandom();
}

//////////////////////////////////////////////////////////////////////////
//�������ƣ�EstablishArrays
//���������int nArrayNum��Ϊ�������Ĵ�С
//�������ܣ���������
//////////////////////////////////////////////////////////////////////////
void CKNode::EstablishArrays(int nArrayNum)
{
//  �ڽ�������ʱ Ϊm_adKWeight����ռ䣬�Ա���InitializeʱԽ�����
	if (m_nDimension!=nArrayNum){
	    m_nDimension = nArrayNum;
	    m_adKWeight.create(nArrayNum);
	}
}

//////////////////////////////////////////////////////////////////////////
//�������ƣ�Initialize
//���������û��
//���������û��
//�������ܣ���ʼ������ʼ��Ȩֵ
//////////////////////////////////////////////////////////////////////////
void CKNode::Initialize()
{
	m_adKSignal.create(m_nDimension);
	for( int i=0; i<m_nDimension; i++ )
		m_adKWeight[i] = bedlam();
}

//////////////////////////////////////////////////////////////////////////
//�������ƣ�CalculateSumSquareEuclideanDistance
//�������ܣ�����Euclidean����
//////////////////////////////////////////////////////////////////////////
void CKNode::CalculateSumSquareEuclideanDistance()
{
	double dSumSquare;

	m_dKOutput = 0.0;
	
	for( int k=0; k<m_nDimension; k++)
	{
		if( m_adKSignal[k] == 0.0)
		{
			dSumSquare = pow( m_adKWeight[k], 2.0 );
		}
		else
		{
			dSumSquare = pow( fabs(m_adKWeight[k] - m_adKSignal[k]), 2.0 );
		}
		m_dKOutput += dSumSquare;
	}

	m_dKOutput = sqrt( fabs(m_dKOutput) );
}


//////////////////////////////////////////////////////////////////////////
//�������ƣ�UpdateWeights
//���������double dInterimRate����������
//�������ܣ�����Ȩֵ
//////////////////////////////////////////////////////////////////////////
void CKNode::UpdateWeights(double dInterimRate)
{
	for( int k=0; k<m_nDimension; k++ )
		m_adKWeight[k] += dInterimRate * ( m_adKSignal[k] - m_adKWeight[k] );

}
