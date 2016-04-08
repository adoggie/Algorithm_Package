// SurvivalCox.h: interface for the CSurvivalCox class.
//         生存分析 比例风险模型 
//			标注： 余卫军
//          时间： 2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURVIVALCOX_H__7E177F30_E0F9_42D5_9B80_9993FB4AD2F8__INCLUDED_)
#define AFX_SURVIVALCOX_H__7E177F30_E0F9_42D5_9B80_9993FB4AD2F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
		
#include "AlgorithmBase.h"

//比例风险模型 
class AFX_EXT_CLASS CSurvivalCox : public CAlgorithmBase  
{
public:
	CResult *OnSurvivalCox(CDataInterface* pDataInterface, string VO);
	int DelCol(double *pData,int nRowNum,int nColNum, int nColNO);
	CSurvivalCox();
	virtual ~CSurvivalCox();

private:
	CResult			* m_pResult;
	CDataInterface	* m_pDataInterface;

	int* m_pCovarValid;
	bool m_bDispBaseLn;
	double m_dbW;
	int m_nWeightFNO;
	bool m_bWeighted;
	double* m_pWeight;

	double m_dbPrecision;
	double m_Eps ;
	// get from db
	int m_nRowNum;
	int m_nColNum;
	double* m_pData;
	double *m_pSurvTime;
	int *m_pStatus;

	// get from dialog
	bool m_bSingleEventValue;//事件的单值情况
	double m_dSingleValue; //给定单值
	double m_dRangeFrom;//区间范围的起始值
	double m_dRangeTo;  //区间范围的终点值
	int m_nStatusFNO;   //状态变量下标
	int m_nTimeFNO;		//时间变量下标
	int* m_pCovarFNOs;	//协变量下标

	bool GetValueFromVO(CDataInterface * pDataIngerface, string VO);
	bool Initialize();
	CResult  * Analyze();
	double Sigdot(double x);
};

#endif // !defined(AFX_SURVIVALCOX_H__7E177F30_E0F9_42D5_9B80_9993FB4AD2F8__INCLUDED_)
