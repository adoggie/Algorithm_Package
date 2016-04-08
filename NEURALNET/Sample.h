//////////////////////////////////////////////////////////////////////
// Sample.h:                                                        //
//																	//
// Define:        输入采样数据类定义                                //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _SAMPLE_H_
#define _SAMPLE_H_
#include "../cpknl/cpknl.h"
#define DOUBLETYPE 0
#define INTEGERTYPE 1
#define BOOLEANTYPE 2

class CSample  
{
private:
	int m_nDemision;//维数
	int m_nTargetNum;//目标
	int m_nSampleNum;//样本
	int m_nDataType;//类型

	//数据
	CDoubleVector  m_aDoubleData;
	CIntVector  m_aIntData;
	CIntVector  m_aBoolData;

public:
	void SetSampleDemision( int nDimesion ) { m_nDemision = nDimesion; };
	unsigned int GetSampleDemision() { return m_nDemision; }
	void SetSampleNum( int nSampleNum ) { m_nSampleNum = nSampleNum; };
	unsigned int GetSampleNum() { return m_nSampleNum; }
	CSample( int nDataType, int nSampleNum, int nDemision );
	virtual ~CSample();

	void SetTargetNum( int nTargetNum )
	{ m_nTargetNum = nTargetNum; }
	int GetTargetNum()
	{ return m_nTargetNum; }
	CDoubleVector &GetBuffer()
	{
		return m_aDoubleData;
	}
};

#endif // !defined(_SAMPLE_H_)
