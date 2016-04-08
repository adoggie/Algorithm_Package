//////////////////////////////////////////////////////////////////////
// SampleSet.h                                                      //
//																	//
// Define:        输入采样数据集类定义                              //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _SAMPLESET_H_
#define _SAMPLESET_H_

#include "Sample.h"
#include "../Result/Result.h"
#include "../DataInterface/DataInterface.h"
#define UNKNOWCOUNT 0x7FFFFFFF

class CSampleSet  
{
private:
	CSample* m_pSet;
public:
	unsigned int GetSampleDemision() { return m_pSet->GetSampleDemision(); }
	unsigned int GetSampleNum() { return m_pSet->GetSampleNum(); }
	CSampleSet();
	virtual ~CSampleSet();
	
	bool InitializeDataset( CDataInterface * pDataInterface, CTStringArray &strField, int nDataType );
	//数据初始化，并且把数据导入到m_pSet中去

	CDoubleVector& GetDataBuffer();
	//获得数据

	int GetSampleSize() { return m_pSet->GetSampleNum(); };
	int GetDemision() { return m_pSet->GetSampleDemision(); };
};

#endif // !defined(_SAMPLESET_H_)
