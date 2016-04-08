//////////////////////////////////////////////////////////////////////
// ARTNode.h                                                        //
//																	//
// Define:        ART算法结点基类定义                               //
// Creator:       屈韬                                              //
// Date:          2002.10.29                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#ifndef _ARTNODE_H_
#define _ARTNODE_H_
#include "../cpknl/cpknl.h"

class CARTNode  
{
public:
	CARTNode();
	virtual ~CARTNode();

	virtual void Initialize()=0;
	virtual void EstablishArrays( int nNumber)=0;
	void SetActivation( double dActivation ) { m_dActivation = dActivation; }
	double GetActivation() { return m_dActivation; }
	void SetWeight( int nIndex, double dValue )	{ m_adWeight[nIndex] = dValue; }
	double GetWeight( int nIndex ) { return m_adWeight[nIndex]; }
	void SetInputValue( int nIndex, double dValue )
	{ m_adInputValue[nIndex] = dValue; }

protected:
	CDoubleVector  m_adWeight;
	CDoubleVector  m_adInputValue;
	CDoubleVector  m_adOutputValue;
	double m_dActivation;
};

#endif // !defined(_ARTNODE_H_)





















