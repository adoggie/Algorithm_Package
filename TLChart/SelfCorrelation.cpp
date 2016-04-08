// SelfCorrelation.cpp: implementation of the CSelfCorrelation class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "SelfCorrelation.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSelfCorrelation::CSelfCorrelation()
{

}

CSelfCorrelation::~CSelfCorrelation()
{

}

CResult *CSelfCorrelation::OnChart(CDataInterface *pDataInterface, string VO)
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

bool CSelfCorrelation::CalcCorrelation()
{
	int i=0, j=0, nLen=0;
	int nColIndex=0,nRowCount=0, nDataType=0;
	CDWordArray arrCol;
	CDoubleMatrix dataMatrix;
	CTString szTemp("");
	CTString szDiff("");
	CTString szFldName("");
	CTString szTitle("");
	TDataPointValVal *pDPVV = NULL;
	CRsltElementChart *pChart = NULL;	
	CTChartAutocorrelations *pSelf =NULL;
	
	//图形
	if (m_bPartial)
	{
		szTitle += "偏";
	}
	if (m_bSelf)
	{
		szTitle += "自";
	}
	szTitle += "相关图";
	m_pResult->SetName(szTitle);
	//计算数据
	for(i=0; i<m_tVarY.iCount; i++)
	{	
		arrCol.RemoveAll();
		dataMatrix.destroy();
		nColIndex = m_tVarY.pValue[i];
		arrCol.Add(nColIndex);
		szFldName = m_pDataInterface->GetFieldName(nColIndex); 
		nRowCount = m_pDataInterface->GetColsData(arrCol,dataMatrix,1);
		if (!IsValidType(nColIndex)) continue;
		if (!IsValidData(nRowCount,nColIndex)) continue;
		//图形包
		CRsltVariable *pRsltVariable = new CRsltVariable(szFldName);
		m_pResult->Add(pRsltVariable);
		//处理数据
		int nKappa = m_nKappa;
		if (nKappa > nRowCount-m_nKappa)
		{
			nKappa = nRowCount-m_nKappa-2;
		}
		//计算相关性
		CDoubleVector v_s(nKappa);//自相关
		CDoubleVector v_p(nKappa);//偏相关
		CalcCorrelation(dataMatrix(0),v_s,v_p);
		//差分
		if(m_nDiff > 0)
		{
          szDiff.Format("，差分阶数Difference = %d", m_nDiff);
		}
		//偏相关
		if(m_bPartial)
		{
			nLen = v_p.vlen();
			if (nLen < 1) 
			{
				szTemp.Format("自相关图中的偏自相关图数据不存在，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
				m_szErrMsg += szTemp;
			}
			else
			{
				pDPVV = new TDataPointValVal[nLen];
				for (j=0; j<nLen; j++)
				{
					pDPVV[j].fYVal = v_p[j];
					pDPVV[j].fXVal = j+1;
				}
				pSelf = new CTChartAutocorrelations;
				pSelf->SetAutoCorrelations(pDPVV,1,ValVal);
				pSelf->SetTitle(szFldName);
				pSelf->SetYAxilLabel("偏自相关图");
				pChart = new CRsltElementChart("偏相关图" + szDiff,pSelf);
				pRsltVariable->Add(pChart);
				delete []pDPVV;
				pDPVV = NULL;
			}
		}
		//自相关图
		if (m_bSelf)
		{
			nLen = v_s.vlen();
			if (nLen < 1) 
			{
				szTemp.Format("自相关图中的自相关图数据不存在，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
				m_szErrMsg += szTemp;
			}
			else
			{
				pDPVV = new TDataPointValVal[nLen];
				for (j=0; j<nLen; j++)
				{
					pDPVV[j].fYVal = v_s[j];
					pDPVV[j].fXVal = j+1;
				}
				pSelf = new CTChartAutocorrelations;
				pSelf->SetAutoCorrelations(pDPVV,1,ValVal);
				pSelf->SetTitle(szFldName);
				pSelf->SetYAxilLabel("自相关图");
				pChart = new CRsltElementChart("自相关图" + szDiff,pSelf);
				pRsltVariable->Add(pChart);
				delete []pDPVV;
				pDPVV = NULL;
			}
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

void CSelfCorrelation::CalcCorrelation(CDoubleVector v, CDoubleVector &v_s, CDoubleVector &v_p)
{
	int i=0, j=0, k=0, nLen=0;
	double fTempA=0, fTempB=0, f_Average=0;
	CDoubleVector v_gamma(m_nKappa+1,0);
	CDoubleMatrix m_triangle(m_nKappa,m_nKappa);
	
	nLen = v.vlen();
	//差分
	for (i=0; i<m_nDiff; i++)
	{
		for (j=0; j<nLen-i-1; j++)
			v(j) = v(j+1) - v(j);
	}
	//差分后实际长度
	nLen -= m_nDiff;
	v.resize(nLen);

	k    = v_p.vlen();
	//计算平均值
	f_Average = v.mean();
	for(i=0; i<nLen; i++)
	{
		v[i] -= f_Average;
	}
	//计算样本协方差。
	for (i=0; i<k+1; i++)
	{
		for (j=0; j<nLen-i; j++)
		{
			v_gamma[i] += v[j]*v[j+i];
		}
		v_gamma[i] /= nLen;
	}
	//计算样本自相关函数。
	for (i=0; i<k; i++)
	{
		v_s[i] = v_gamma[i+1]/v_gamma[0];
	}
	//计算样本偏自相关函数。
	m_triangle(0)(0) = v_s[0];
	for (i=0;i<k-1;i++)
	{
		m_triangle(i+1,i+1)=0;
		fTempA = 0,fTempB = 0;
		for(j=0;j<=i;j++)
		{
			fTempA += v_s[i-j] * m_triangle(i)(j);
			fTempB += v_s[j] * m_triangle(i)(j);
		}
		m_triangle(i+1,i+1) = (v_s[i+1]-fTempA)/(1-fTempB);
		for(j=0;j<=i;j++)
		{
			m_triangle(i+1,j) = m_triangle(i,j) - m_triangle(i+1,i+1) * m_triangle(i,i-j);
		}
	}
	//取PHI_TriangleMatrix对角线元素。
	for(i=0;i<k;i++)
	{
		v_p[i] = m_triangle(i,i);
	}
}