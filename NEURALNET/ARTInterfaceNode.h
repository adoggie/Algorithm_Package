//////////////////////////////////////////////////////////////////////
// ARTInterfaceNode.h                                               //
//																	//
// Define:        ART算法接口层结点类定义                           //
// Creator:       屈韬                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#ifndef _ARTINTERFACENODE_H_
#define _ARTINTERFACENODE_H_

#include "ARTNode.h"

class CARTInterfaceNode : public CARTNode 
{
public:
	void RecomputeActivation( int nChampCluster );
	double GetOutputValue( int nCluster );
	void CalculateOutputValue( int nGainOne );
	CARTInterfaceNode();
	virtual ~CARTInterfaceNode();

	virtual void EstablishArrays( int nNumber);
	virtual void Initialize();

private:
	int m_nClusterNum;
};

#endif // !defined(_ARTINTERFACENODE_H_)
