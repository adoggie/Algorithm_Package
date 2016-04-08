//////////////////////////////////////////////////////////////////////
// SurvivalKM.h:                                                    //
//																	//
// Define:        生存分析KM过程算法定义                            //
// Creator:       张法明、屈韬                                      //
// Date:          2002.11.25                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _SURVIVALKM_H_
#define _SURVIVALKM_H_

#include "AlgorithmBase.h"

struct TPeriodTable{
	double dTime;			// 时间段
	int nDiedNum;			// 死亡人数
	int nConsoredNum;		// 失访人数
	double dCS;				// 累计事件
	double dSE;				// 标准差

	TPeriodTable()
	{
		dTime = 0;
		nDiedNum = 0;
		nConsoredNum = 0;
		dCS = 0;
		dSE = 0;
	}
};

struct TKMTable{
	double dFactorValue;		// 因素值
	CTString strFactorLabel;		// 因素值标签
	int nCaseNum;
	int nEventNum;
	double dMeanTime;
	double dMedianTime;
	CTArray<TPeriodTable*, TPeriodTable*> aPeriodTable; // 该因素类型的时间段表
	
	TKMTable()
	{
		dFactorValue = 0;
		nCaseNum = 0;
		nEventNum = 0;
		dMeanTime = 0;
		dMedianTime = 0;
		strFactorLabel = _T("");
	}

	~TKMTable()
	{
		for( int i=0; i<aPeriodTable.GetSize(); i++ )
			delete aPeriodTable[i];
		aPeriodTable.RemoveAll();
	}
};


class  AFX_EXT_CLASS CSurvivalKM : public CAlgorithmBase  
{
public:
	CSurvivalKM();
	~CSurvivalKM();
	CResult * OnSurvivalKM(CDataInterface * pDataInterface, string VO);

private:
	bool GetValueFromVO(CDataInterface * pDataInterface, string VO);
	bool Initialize();
	bool OutputTables();
	CResult *Analyze();

	CTArray< TKMTable*, TKMTable* > m_aKMTable;
	
	int         m_nFactorPos; //Factor字段的位置
	int         m_nTimePos;   //Sur_Time字段的位置
	int         m_nStatusPos; //Status字段的位置
	int         m_nFactorNum;  //Status字段 标签总数
	bool        m_bIsRange;
	double      m_dSingle;
	double      m_dRangeLow;
	double      m_dRangeHigh;

	CTString		m_strTimeFieldName;
	CTString		m_strTimeFieldLable;
};

#endif // !defined(_SURVIVALKM_H_)
