// TimeSeansonal.h: interface for the CTimeSeansonal class.
//                     时间序列季节解构
//                    标注： 余卫军
//					  时间： 2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMESEANSONAL_H__D5543201_BB57_44E6_9F07_70648653422A__INCLUDED_)
#define AFX_TIMESEANSONAL_H__D5543201_BB57_44E6_9F07_70648653422A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"


class AFX_EXT_CLASS CTimeSeansonal : public CAlgorithmBase  
{
public:

	CResult * CTimeSeansonal::OnTimeSeasonal(CDataInterface *pDataInterface, string VO);
	CDoubleMatrix CTimeSeansonal::GetSAF(CDoubleVector vector,int dP,int nMorA);
	CTimeSeansonal();
	virtual ~CTimeSeansonal();

private:
	int m_nMethod;//0－乘法 ，1－加法
	int m_nPeriod;//0=4 ,1-12
	CDataInterface * m_pDataInterface;
	CResult		   * m_pResult;
	CRsltElementText *m_pRsltElementText;
    CRsltElementTable * m_pRsltVariable;
	TIntStruct m_tVarInfo;
	int m_nCurVarNum;
	
	CTLTable* briefrTable;
	
	void CTimeSeansonal::CreateResullForSeason(CDoubleMatrix matrix_Z,CDoubleMatrix matrix_SI,CDoubleMatrix matrix_SAF,CDoubleMatrix matrix_SAS,CDoubleMatrix matrix_STC,CDoubleMatrix matrix_I,int period);
	bool GetValueFromVO(CDataInterface * pDataInterface, string VO);
	bool CTimeSeansonal::Season(CDoubleVector vector,int dP,int nMorA,int nCol1);
	CDoubleVector CTimeSeansonal::GetSingleColData(int index,bool &bIsToEnd);
	CDoubleMatrix CTimeSeansonal::Mov_ave(CDoubleMatrix matrix_X,int nT3orT12);
	double CTimeSeansonal::Medial_average(CDoubleMatrix matrix_X);
	
};

#endif // !defined(AFX_TIMESEANSONAL_H__D5543201_BB57_44E6_9F07_70648653422A__INCLUDED_)
