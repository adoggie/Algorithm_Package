//////////////////////////////////////////////////////////////////////
// ARTClusterLayer.h                                                //
//																	//
// Define:        ART算法聚类层类定义                               //
// Creator:       屈韬                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#ifndef _ARTCLUSTERLAYER_H_
#define _ARTCLUSTERLAYER_H_

#include "ARTClusterNode.h"

class CARTClusterLayer  
{
public:
	void OutputNodeWeight(CResult *pResult);//输出节点的权重
	void SetLayerInputValue( CDoubleVector& pData );//设置输入层的值
	void EstablishNodeOutputValue( int nIndex );
	double CalculateNetInput( int nCluster );//计算输入层

	void SetNodeInputValue( int nCluster, int nDimension, double dValue )//设置输入节点的初始值
	{ m_aClusterNode[nCluster].SetInputValue( nDimension, dValue ); }
	double GetNodeOutputValue( int nCluster, int nDimension )
	{ return m_aClusterNode[nCluster].GetOutputValue( nDimension ); }
	double GetActivation( int nIndex )
	{ return m_aClusterNode[nIndex].GetActivation(); }
	void SetActivation( int nIndex, double dActivation )
	{ m_aClusterNode[nIndex].SetActivation( dActivation ); }
	void SetNodeClusterTag( int nIndex, int nTag )
	{ m_aClusterNode[nIndex].SetClusterTag( nTag ); }
	int GetNodeClusterTag( int nIndex ) 
	{ return m_aClusterNode[nIndex].GetClusterTag(); }
	void SetNodeWeight( int nCluster, int nDimension, double dValue )
	{ m_aClusterNode[nCluster].SetWeight( nDimension, dValue ); }
	double GetNodeWeight( int nCluster, int nDimension )
	{ return m_aClusterNode[nCluster].GetWeight( nDimension ); }

	CARTClusterLayer( int nDimension, int ClusterNum );
	virtual ~CARTClusterLayer();

private:
	CARTClusterNode * m_aClusterNode;
	int m_nNodeNum;
	int m_nDimension;
};

#endif // !defined(_ARTCLUSTERLAYER_H_)
