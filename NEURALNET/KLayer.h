//////////////////////////////////////////////////////////////////////
// KLayer.h:                                                        //
//																	//
// Define:        Kohonen算法Layer类定义                            //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _KLAYER_H_
#define _KLAYER_H_

#include "KNode.h"

//层数
class CKLayer  
{
public:
	double GetNodeWeight( int nCluster, int nDimension )//获得节点权值
	{ return m_aKNode[nCluster].GetWeight( nDimension ); }
	void SetNodeWeight( int nCluster, int nDimension, double dValue )
	{ m_aKNode[nCluster].SetWeight( nDimension, dValue ); }
	void UpdateNodeWeights( int nIndex, double dInterimRate )
	{ m_aKNode[nIndex].UpdateWeights( dInterimRate ); }
	double GetNodeOutputValue( int nIndex )
	{ return m_aKNode[nIndex].GetNodeOutputValue(); }
	void CalculateNodeSumSquareEuclideanDistance( int nCluster )
	{ m_aKNode[nCluster].CalculateSumSquareEuclideanDistance(); }
	void SetNodeInputValue( int nCluster, int nDimension, double dValue )
	{ m_aKNode[nCluster].SetInputValue( nDimension, dValue ); }

	CKLayer( int nDimension, int nClusterNum );
	virtual ~CKLayer();

private:
	CKNode * m_aKNode;//节点
	int m_nNodeNum;//节点个数
};

#endif // !defined(_KLAYER_H_)
