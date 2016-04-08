// PartialCorre.h: interface for the CPartialCorre class.
//				偏相关分析
//					马良庄    2006/03/21
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTIALCORRE_H__8AF65035_7552_47B2_BC14_5AF255CE2AD5__INCLUDED_)
#define AFX_PARTIALCORRE_H__8AF65035_7552_47B2_BC14_5AF255CE2AD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "tlcrnt.h"

class AFX_EXT_CLASS CPartialCorre  
{

public:
	CPartialCorre();
	virtual ~CPartialCorre();
	CResult* OnPartialCorre(CDataInterface * pDataInterface, string VO);

public:
	BOOL GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void CreateResult(CDataInterface* pDataInterface,CResult* &pResult);
	TIntStruct m_tVarStruct;//存放已选中的变量
    TIntStruct m_tContStruct;//存放选中的控制变量


private:
	double * m_adCorr;
	int * m_anNofCorr;
	double * m_adWeightofCorr;
	double * m_adPartialCorr;
	double * m_adPartialSig;
	int * m_anDF;
	bool m_bOneTail;
	bool m_bWeightMissing;
	int m_nWeight;//权重的下标
	double m_dCases;

	struct TTempCorr
		{
			bool bNeedCalc;
			bool bError;
			double dCorr;

			TTempCorr()
			{
				bNeedCalc = true;
				bError = false;
				dCorr = 0.0;
			}
		} * m_pTempCorr;

private:
	void Clear();
	void GetData( CDataInterface* pDataInterface, CPtrArray& arVarData, CPtrArray& arCtrlData, int nGetType );
	void Calculate(CPtrArray &arVarData, CPtrArray &arContData, double aWeight[], int n);
	void CalcZeroCorr( CPtrArray& arData, double aWeight[], int n );
	double CalcPartialCorr( CTArray<int, int>& arCol, int nVar, int nCtrl, bool& bError );
	double MinSumWeight( CTArray<int,int>& arCol, int k );
};

#endif // !defined(AFX_PARTIALCORRE_H__8AF65035_7552_47B2_BC14_5AF255CE2AD5__INCLUDED_)
