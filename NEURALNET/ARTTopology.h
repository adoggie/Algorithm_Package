//////////////////////////////////////////////////////////////////////
// ARTTopology.h:                                                   //
//																	//
// Define:        ART算法拓扑结构类定义                             //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _ARTTOPOLOGY_H_
#define _ARTTOPOLOGY_H_

#include "ARTInterfaceNode.h"
#include "ARTInterfaceLayer.h"
#include "ARTClusterNode.h"
#include "ARTClusterLayer.h"
#include "Topology.h"
#include "../Public/TString.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"

class CARTTopology : public CTopology  
{
private:
	bool ValidateInputData( CDoubleVector &adData);
	void ClusterNodesCompeteForActivation( bool bTraining );
	void BroadcastOutputToClusterLayer();

	CARTInterfaceLayer * m_pInterfaceLayer;
	CARTClusterLayer * m_pClusterLayer;
	
	int m_nResetCount;
	int m_nClusterCount;
	int m_nClusterRange;
	double m_dVigilance;
	int m_nChampCluster;
	bool m_bReset;
public:
	void SetClusterTag( CIntVector &anTag);
	CARTTopology();
	CARTTopology( int nType, int nSampleSize, int nDimension, int nClusterNum,
				 long lRuntime, int nIterateNum, double fVigilance );
	virtual ~CARTTopology();

	void SetClusterCount( int nClusterCount ) { m_nClusterCount = nClusterCount; }
	void SetClusterRange( int nClusterRange ) { m_nClusterRange = nClusterRange; }
	void SetInterfaceLayerWeight( CDoubleVector &adWeight); 
	void SetClusterLayerWeight( CDoubleVector &adWeight);

	void UpdateNetwork( double dNormInputVector, double dNormActivationVector );

	virtual void SaveNetwork();
	virtual bool Testing( CDataInterface * pData, CTStringArray &arFieldRole, CResult * pResult, bool bOutputInfo );
	virtual bool Training( CDataInterface * pData, CTStringArray &arFieldRole, CResult *& pResult, bool bOutputInfo );
};

#endif // !defined(_ARTTOPOLOGY_H_)





















