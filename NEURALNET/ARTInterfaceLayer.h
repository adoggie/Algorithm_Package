//////////////////////////////////////////////////////////////////////
// ARTInterfaceLayer.h                                              //
//																	//
// Define:        ART�㷨�ӿڲ��ඨ��                               //
// Creator:       ���                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#ifndef _ARTINTERFACELAYER_H_
#define _ARTINTERFACELAYER_H_

#include "ARTInterfaceNode.h"
#include "../Result/Result.h"
class CARTInterfaceLayer  
{
public:
	void OutputNodeWeight( CResult * pResult );//��ʾ�ڵ��Ȩֵ
	void RecomputeActivation( int nChampCluster );//���¼���ڵ�
	double GetNodeOutputValue( int nDimension, int nCluster );
	void CalculateOutputValue( int nGainOne );
	void SetLayerInputValue( CDoubleVector &pData );
	void SetNodeInputValue( int nDimension, int nCluster, double dData )
	{ m_aInterfaceNode[nDimension].SetInputValue( nCluster, dData ); }
	double GetActivation( int nIndex )
	{ return m_aInterfaceNode[nIndex].GetActivation(); }
	void SetNodeWeight( int nDimension, int nCluster, double dValue )
	{ m_aInterfaceNode[nDimension].SetWeight( nCluster, dValue ); }
	double GetNodeWeight( int nDimension, int nCluster )
	{ return m_aInterfaceNode[nDimension].GetWeight( nCluster ); }


	CARTInterfaceLayer( int nDimension, int nClusterNum);
	virtual ~CARTInterfaceLayer();

private:
	int m_nDimension;//����
	int m_nClusterNum;//������
	CARTInterfaceNode * m_aInterfaceNode;
	int m_nNodeNum;
};

#endif // !defined(_ARTINTERFACELAYER_H_)
