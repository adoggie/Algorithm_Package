//////////////////////////////////////////////////////////////////////
// KNode.h:                                                         //
//																	//
// Define:        Kohonen算法节点类定义                             //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _KNODE_H_
#define _KNODE_H_
#include "../cpknl/cpknl.h"
class CKNode  
{
public:
	double GetWeight( int nDimension )
	{ return m_adKWeight[nDimension]; }
	void SetWeight( int nDimension, double dValue )
	{ m_adKWeight[nDimension] = dValue; }
	void UpdateWeights( double dInterimRate );
	double GetNodeOutputValue()
	{ return m_dKOutput; }
	void CalculateSumSquareEuclideanDistance();
	void SetInputValue( int nDimension, double dValue )
	{ m_adKSignal[nDimension] = dValue; }
	void Initialize();
	void EstablishArrays( int nArrayNum );
	CKNode();
	virtual ~CKNode();

private:
	int m_nDimension;//维数
	CDoubleVector m_adKWeight;//权值
	double m_dKOutput;//
	CDoubleVector m_adKSignal;
};

#endif // !defined(_KNODE_H_)





















