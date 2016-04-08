//////////////////////////////////////////////////////////////////////
// SurvivalKM.h:                                                    //
//																	//
// Define:        �������KM�����㷨����                            //
// Creator:       �ŷ��������                                      //
// Date:          2002.11.25                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _SURVIVALKM_H_
#define _SURVIVALKM_H_

#include "AlgorithmBase.h"

struct TPeriodTable{
	double dTime;			// ʱ���
	int nDiedNum;			// ��������
	int nConsoredNum;		// ʧ������
	double dCS;				// �ۼ��¼�
	double dSE;				// ��׼��

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
	double dFactorValue;		// ����ֵ
	CTString strFactorLabel;		// ����ֵ��ǩ
	int nCaseNum;
	int nEventNum;
	double dMeanTime;
	double dMedianTime;
	CTArray<TPeriodTable*, TPeriodTable*> aPeriodTable; // ���������͵�ʱ��α�
	
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
	
	int         m_nFactorPos; //Factor�ֶε�λ��
	int         m_nTimePos;   //Sur_Time�ֶε�λ��
	int         m_nStatusPos; //Status�ֶε�λ��
	int         m_nFactorNum;  //Status�ֶ� ��ǩ����
	bool        m_bIsRange;
	double      m_dSingle;
	double      m_dRangeLow;
	double      m_dRangeHigh;

	CTString		m_strTimeFieldName;
	CTString		m_strTimeFieldLable;
};

#endif // !defined(_SURVIVALKM_H_)
