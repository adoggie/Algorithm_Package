// SurvivalLT.h: interface for the CSurvivalLT class.
//                             生命表分析 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURVIVALLT_H__5AEFC5D6_10AE_4635_991A_53CC9B4E8794__INCLUDED_)
#define AFX_SURVIVALLT_H__5AEFC5D6_10AE_4635_991A_53CC9B4E8794__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"
/*
* 定义的结构：包括需要生成的字段
*/
struct TLiftTbl
{
	//第一张表需要的参数
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


//生命表分析
class AFX_EXT_CLASS CSurvivalLT : public CAlgorithmBase
{
public:
	CResult * OnSurvivalLT(CDataInterface * pDataInterface, string VO);
	CSurvivalLT();
	virtual ~CSurvivalLT();

private:
	TLiftTbl *m_ptLiftTbl;

	////////////////////////////////////////////////////////////
	// 添加：屈韬
	// 目的：需用户设置的成员变量
	int		m_nTimeField;				// 时间参数对应的数据字段
	int		m_nStatusField;				// 状态参数对应的数据字段
	bool	m_bSingleEventValue;		// 状态判断是否使用单一值
	double	m_dSingleValue;				// 状态判断使用的单一值
	double	m_dRangeFrom;				// 状态判断使用的范围起始值
	double	m_dRangeTo;					// 状态判断使用的范围终止值
	double	m_dMaxTime;					// 显示最大时间
	double	m_dStep;					// 显示时间的步长
	//
	////////////////////////////////////////////////////////////

	bool m_bBeyond;		// 记录时间是否超出用户设置的最大时间？
	int m_nPeriodNum;

private:
	bool GetValueFromVO(CDataInterface * pDataInterface, string VO);
	CResult * OutputTable();
	virtual bool Initialize(void);
	virtual CResult *Analyze(void);
};

#endif // !defined(AFX_SURVIVALLT_H__5AEFC5D6_10AE_4635_991A_53CC9B4E8794__INCLUDED_)
