//////////////////////////////////////////////////////////////////////
// KNode.cpp:                                                       //
//																	//
// Define:        Kohonen算法节点类实现                             //
// Creator:       屈韬                                              //
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
//函数名称：EstablishArrays
//输入参数：int nArrayNum，为建立数的大小
//函数功能：建立数组
//////////////////////////////////////////////////////////////////////////
void CKNode::EstablishArrays(int nArrayNum)
{
//  在建立数组时 为m_adKWeight申请空间，以避免Initialize时越界操作
	if (m_nDimension!=nArrayNum){
	    m_nDimension = nArrayNum;
	    m_adKWeight.create(nArrayNum);
	}
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Initialize
//输入参数：没有
//输出参数：没有
//函数功能：初始化，初始化权值
//////////////////////////////////////////////////////////////////////////
void CKNode::Initialize()
{
	m_adKSignal.create(m_nDimension);
	for( int i=0; i<m_nDimension; i++ )
		m_adKWeight[i] = bedlam();
}

//////////////////////////////////////////////////////////////////////////
//函数名称：CalculateSumSquareEuclideanDistance
//函数功能：计算Euclidean距离
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
//函数名称：UpdateWeights
//输入参数：double dInterimRate，修正参数
//函数功能：更新权值
//////////////////////////////////////////////////////////////////////////
void CKNode::UpdateWeights(double dInterimRate)
{
	for( int k=0; k<m_nDimension; k++ )
		m_adKWeight[k] += dInterimRate * ( m_adKSignal[k] - m_adKWeight[k] );

}
