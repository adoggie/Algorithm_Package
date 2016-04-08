//////////////////////////////////////////////////////////////////////
// ARTClusterNode.h                                                 //
//																	//
// Define:        ART算法聚类层结点类定义                           //
// Creator:       屈韬                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#ifndef _ARTCLUSTERNODE_H_
#define _ARTCLUSTERNODE_H_

#include "ARTNode.h"

class CARTClusterNode : public CARTNode
{
public:
	CARTClusterNode();
	virtual ~CARTClusterNode();

	virtual void EstablishArrays( int nNumber);
	virtual void Initialize();
	void SetClusterTag( int nTag ) 
	{ m_nClusterTag = nTag; };
	int GetClusterTag() { return m_nClusterTag; }
	double GetOutputValue( int nDimension )
	{ return m_adOutputValue[nDimension]; }
	void EstablishOutputValue();
	double CalculateNetInput();
private:
	int m_nDimension;
	int m_nClusterTag;
};

#endif // !defined(_ARTCLUSTERNODE_H_)
