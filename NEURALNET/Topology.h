//////////////////////////////////////////////////////////////////////
// Topology.h:                                                      //
//																	//
// Define:        神经网络拓扑结构类定义                            //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _TOPOLOGY_H_
#define _TOPOLOGY_H_

#include "../Public/TString.h"
#include "../DataInterface/DataInterface.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
// Next default values for Application
//
#define DATAOVERLOAD				10000000	// 大数据界定
#define WEIGHT_UPDATE_PARAMETER		2.0		// ART算法所需的权值更新参数
#define FLOAT_PRECISION				4

///////////////////////////////////////
// 定义各算法代码
//
#define ARTTOPOLOGY					0x0001	
#define KOHONENTOPOLOGY				0x0002
#define BPTOPOLOGY					0x0004
#define RBFTOPOLOGY					0x0008
#define HOPTOPOLOGY					0x0010
#define GENICTOPOLOGY				0x0020
#define HOPANDGEN					0x0040

#define HANDLEDTOPOLOGYNUM			7

class CTopology  
{
public: 
	int m_nTopologyType;
public:
	void Abort() { m_bAbort = true; };
	virtual bool GetRowData( CDataInterface * pData, CTStringArray& arFieldRole, unsigned int nRow, CDoubleVector& adDataRow);
	virtual bool Training(CDataInterface * pData, CTStringArray& arFieldRole, CResult *& pResult, bool bOutputInfo) = 0;
	virtual bool Testing(CDataInterface * pData, CTStringArray& arFieldRole, CResult * pResult, bool bOutputInfo) = 0;
	static CTopology * Initialization( int nType, ...);
	CTopology();
	CTopology( int nType, int nSampleSize, int nDimension, int nClusterNum, 
					 long lRuntime, int nIterateNum );
	virtual ~CTopology();

protected:
	int   m_nClusterNum;
	int   m_nSampleSize;
	int   m_nDimension;
	bool m_bAbort;
	unsigned int m_dwRuntime;
	unsigned int  m_unIterateNum;
};

#endif // !defined(_TOPOLOGY_H_)
