// SurvivalLT.h: interface for the CSurvivalLT class.
//                             ��������� 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURVIVALLT_H__5AEFC5D6_10AE_4635_991A_53CC9B4E8794__INCLUDED_)
#define AFX_SURVIVALLT_H__5AEFC5D6_10AE_4635_991A_53CC9B4E8794__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"
/*
* ����Ľṹ��������Ҫ���ɵ��ֶ�
*/
struct TLiftTbl
{
	//��һ�ű���Ҫ�Ĳ���
	double dIntStartTime;	//Intrvl Start Time
	double dThisIntrvl;		//Number Entrng this Intrvl
	double dDuringIntrvl;	//Number Wdrawn During Intrvl
	double dToRisk;			//Number Exposd to Risk
	double dTermnlEvents;	//Number of Termnl Events
	double dTerminating;	//Propn Terminating
	double dSurviving;		//Propn Surviving  
	double dSurvAtEnd;		//Cumul Propn Surv at End
	double dDensty;			//Probability Density
	double dRate;			//Hazard Rate
	double dSecs;			//SE of Cumul Surviving
	double dSepd;			//SE of Probability Density
	double dSehr;			//SE of Hazard Rate

	TLiftTbl()
	{
		 dIntStartTime = 0;	
		 dThisIntrvl = 0;
		 dDuringIntrvl = 0;
		 dToRisk = 0;		
		 dTermnlEvents = 0;
		 dTerminating = 0;	
		 dSurviving = 0;
		 dSurvAtEnd = 0;
		 dDensty = 0;
		 dRate = 0;	
		 dSecs = 0;	
		 dSepd = 0;
		 dSehr = 0;	
	}

};


//���������
class AFX_EXT_CLASS CSurvivalLT : public CAlgorithmBase
{
public:
	CResult * OnSurvivalLT(CDataInterface * pDataInterface, string VO);
	CSurvivalLT();
	virtual ~CSurvivalLT();

private:
	TLiftTbl *m_ptLiftTbl;

	////////////////////////////////////////////////////////////
	// ��ӣ����
	// Ŀ�ģ����û����õĳ�Ա����
	int		m_nTimeField;				// ʱ�������Ӧ�������ֶ�
	int		m_nStatusField;				// ״̬������Ӧ�������ֶ�
	bool	m_bSingleEventValue;		// ״̬�ж��Ƿ�ʹ�õ�һֵ
	double	m_dSingleValue;				// ״̬�ж�ʹ�õĵ�һֵ
	double	m_dRangeFrom;				// ״̬�ж�ʹ�õķ�Χ��ʼֵ
	double	m_dRangeTo;					// ״̬�ж�ʹ�õķ�Χ��ֵֹ
	double	m_dMaxTime;					// ��ʾ���ʱ��
	double	m_dStep;					// ��ʾʱ��Ĳ���
	//
	////////////////////////////////////////////////////////////

	bool m_bBeyond;		// ��¼ʱ���Ƿ񳬳��û����õ����ʱ�䣿
	int m_nPeriodNum;

private:
	bool GetValueFromVO(CDataInterface * pDataInterface, string VO);
	CResult * OutputTable();
	virtual bool Initialize(void);
	virtual CResult *Analyze(void);
};

#endif // !defined(AFX_SURVIVALLT_H__5AEFC5D6_10AE_4635_991A_53CC9B4E8794__INCLUDED_)
