//////////////////////////////////////////////////////////////////////
// KohonenTopology.h:                                               //
//																	//
// Define:        Kohonen算法拓扑结构类定义                         //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _KOHONENTOPOLOGY_H_
#define _KOHONENTOPOLOGY_H_


#include "KLayer.h"
#include "Topology.h"

typedef struct tagMaxMin{
	bool bNorm;
	double dMax;
	double dMin;
} NormInfo;

class CKohonenTopology : public CTopology  
{
private:
	void UpdateNetwork( int ep );//更新网络
	void NodesCompeteForActivation();
	bool NormalizeSampleData( CDataInterface * pData, CTStringArray& arFieldRole, bool bOverload, NormInfo * aNormInfo, CIntVector& abValid, CDoubleMatrix &aDataAll );//归一化数据

	double m_dInterimRate;
	int m_nChampCluster;
	double m_dMinRate;
	double m_dMaxRate;
	CKLayer * m_pKLayer;

public:
	void SetLayerWeight( CDoubleVector& adWeight);
	CDoubleVector GetLayerWeight();
	virtual bool Testing(CDataInterface * pData, CTStringArray &arFieldRole, CResult * pResult, bool bOutputInfo);
	virtual bool Training(CDataInterface * pData, CTStringArray &arFieldRole, CResult *& pResult, bool bOutputInfo);

	CKohonenTopology( int nType, int nSampleSize, int nDimension, int nClusterNum,
					 long lRuntime, int nIterateNum, double dMaxRate, double dMinRate );
	virtual ~CKohonenTopology();
};

#endif // !defined(_KOHONENTOPOLOGY_H_)





















