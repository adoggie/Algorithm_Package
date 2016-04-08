// CrossCorrelation.cpp: implementation of the CCrossCorrelation class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "CrossCorrelation.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrossCorrelation::CCrossCorrelation()
{

}

CCrossCorrelation::~CCrossCorrelation()
{

}

CResult *CCrossCorrelation::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//图形
	SetChartType(CHART_SELFCORRELATIONS);
	//解析
	if (!GetValueFromVo(VO))
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
		return m_pResult;
	}
	//验证
	if (!Verify())
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
		return m_pResult;
	}
	//初始化
	Init();
	bRet = CalcCorrelation();
	if (!bRet)
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
		return m_pResult;
	}
	return m_pResult;
}

bool CCrossCorrelation::CalcCorrelation()
{
	int i=0, j=0, k=0, nRowCount=0;
	int nColIndex1=0, nDataType1=0;
	int nColIndex2=0, nDataType2=0;
	CDWordArray arrCol;
	CDoubleVector v1,v2;
	CDoubleMatrix dataMatrix;	
	TDataPointValVal *pDPVV = NULL;	
	CRsltElementChart *pChart = NULL;		
	CTChartCrossCorrelations *pCross = NULL;
	CTString szTitle("");
	CTString szFldName1("");
	CTString szFldName2("");

	int nCount = m_tVarY.iCount;
	for (i=0; i<nCount; i++)
	{			
		for (j=i; j<nCount-1; j++)
		{
			int nSize = 0;
			arrCol.RemoveAll();
			dataMatrix.destroy();
			nColIndex1 = m_tVarY.pValue[i]; 		
			szFldName1 = m_pDataInterface->GetFieldName(nColIndex1);
			nColIndex2 = m_tVarY.pValue[j+1];  		
			szFldName2 = m_pDataInterface->GetFieldName(nColIndex2);
			if (!IsValidType(nColIndex1)) continue;
			if (!IsValidType(nColIndex2)) continue;
			nRowCount = m_pDataInterface->GetColsData(arrCol,dataMatrix,1);
			if (!IsValidData(nRowCount, nColIndex1, nColIndex2)) continue;
			v1 = dataMatrix(0);
			v2 = dataMatrix(1);
			if (abs(m_nKappa) <= (nRowCount-2-m_nDiff))
			{				
				nSize = 2*m_nKappa + 1;			
				pDPVV = new TDataPointValVal[nSize];
				CalcCorrelation(v1,v2,pDPVV);
			}
			else
			{
				nSize = 2*(nRowCount-m_nDiff)-3;
				pDPVV = new TDataPointValVal[nSize];
				CalcCorrelation(v1,v2,pDPVV);
			}
			//排序
			QuickSort(pDPVV,nSize);
			szTitle = szFldName1 + " 与 " + szFldName2;
			pCross = new CTChartCrossCorrelations;
			pCross->SetCrossCorrelations(pDPVV,1,ValVal);
			pCross->SetYAxilLabel("互相关图");
			pCross->SetTitle(szTitle);
			pChart = new CRsltElementChart(szTitle+"互相关图",pCross);
			m_pResult->SetName("互相关图");
			m_pResult->Add(pChart);
			delete []pDPVV;	
		}			
	}	
	//添加错误信息
	if (GetLastErrMsg().GetLength() >0)
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
	}
	return true;
}

void CCrossCorrelation::CalcCorrelation(CDoubleVector &v1, CDoubleVector &v2, TDataPointValVal *pDPVV)
{
	int i=0, j=0, nLen=0;
	double fStd1=0, fStd2=0;
	double fMean1=0, fMean2=0;
	double fTempSum = 0;
	
	//长度
	nLen = v1.vlen();
	//差分
	for (i=0; i<m_nDiff; i++)
	{
		for (j=0; j<nLen-i-1; j++)
		{
			v1(j) = v1(j+1) - v1(j);
			v2(j) = v2(j+1) - v2(j);
		}
	}
	//差分后实际长度
	nLen -= m_nDiff;
	v1.resize(nLen);
	v2.resize(nLen);
	//计算
	v1.sumvar(fMean1,fStd1);
	fMean1 /= nLen;
	fStd1 = sqrt(fStd1);
	v2.sumvar(fMean2,fStd2);
	fMean2 /= nLen;
	fStd2 = sqrt(fStd2);
	//计算
	int nKappa = (m_nKappa <= nLen-2) ? m_nKappa : (nLen-2);
	for (i=0; i<=nKappa; i++)
	{
		fTempSum = 0;	
		for (j=0; j<nLen-i; j++)
		{
			fTempSum += (v1(j)-fMean1)*(v2(j+i)-fMean2);				
		}
		pDPVV[i].fYVal = (fTempSum/nLen)/(fStd1*fStd2);
		pDPVV[i].fXVal = i;
	}
	for (i=-1; abs(i)<=nKappa; i--)
	{
		fTempSum = 0;	
		for (j=0; j<nLen+i; j++)
		{
			fTempSum += (v1(j-i)-fMean1) * (v2(j)-fMean2);			
		}	
		pDPVV[m_nKappa-i].fYVal = (fTempSum/nLen)/(fStd1*fStd2);
		pDPVV[m_nKappa-i].fXVal = i;
	}
}