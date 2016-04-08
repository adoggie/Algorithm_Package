// Emp.cpp: implementation of the CEmp class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Emp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEmp::CEmp()
{
	m_szErrMsg = "";
	m_nDivNum = 0;    //分块数
	m_bExp = false;   //经验图
	m_bDen = false;   //密度图
	m_nDistType = 0;
}

CEmp::~CEmp()
{
	
}


bool CEmp::Verify()
{
	int i = 0;
	int nColIndex = 0, nDataType = 0;
	CTString szFldName("");
	
	if (m_tVarX.iCount < 1)
	{
		m_szErrMsg.Format("%s中的轴变量参数没有指定，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
		return false;
	}
	else
	{
		for (i=0; i<m_tVarX.iCount; i++)
		{
			nColIndex = m_tVarX.pValue[i];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			if (nDataType == fString)
			{ 
				m_szErrMsg.Format("%s中的值变量参数类型不能为字符型，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
				return false;
			}
		}
	}
	if (m_nDivNum < 1)
	{
		m_szErrMsg.Format("%s中的分组参数小于1，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
		return false;
	}
	if (!m_bExp && !m_bDen)
	{
		m_szErrMsg.Format("%s中的输出图形没有指定，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
		return false;
	}
	return true;
}

CResult *CEmp::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//图形
	SetChartType(CHART_EMP);
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
	bRet = Emp();
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

bool CEmp::GetValueFromVo(string VO)
{
	if (!ParseTypeMode(VO)) return false;
	if (!ParseDataType(VO)) return false;
	if (!ParseAxis(VO)) return false;
	if (!ParseVar(VO)) return false;
	if (!ParseDefLine(VO)) return false;
	if (!ParseWeight(VO)) return false;
	if (!ParseParam(VO))  return false;
	if (!ParseOutput(VO)) return false;
	return true;
}


bool CEmp::ParseTypeMode(string VO)
{
	return true;
}

bool CEmp::ParseParam(string VO)
{
	int i=0, nSize=0;
	CTString szTemp;
	CTStringArray szArrName;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	//解析
	szArrName.RemoveAll();
	if (AnaSyntax::exist(DIV_STR,VOstr)) 
	{
		AnaSyntax::getVAR(DIV_STR, VOstr, szArrName);
		int nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("%s中的分块参数设置错误，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		m_nDivNum = atoi(szTemp);
	}
	//图形
	szArrName.RemoveAll();
	if (AnaSyntax::exist(PLOT_STR,VOstr)) 
	{
		AnaSyntax::getVAR(PLOT_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize < 1)
		{
			m_szErrMsg.Format("%s中的输出图形不确定，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<nSize; i++)
		{
			szTemp = szArrName.GetAt(i);
			szTemp.MakeUpper();
			if (szTemp == EMP_EXP_STR)
			{
				m_bExp = true;
			}
			else if (szTemp == EMP_DEN_STR)
			{
				m_bDen = true;
			}
			else
			{
				m_szErrMsg.Format("%s中的输出图表类型错误－%s，请检查！Line=%d,File=%s",m_szChartName.GetData(),szTemp.GetData(),__LINE__,__FILE__);
				return false;
			}
		}
	}
	return true;
}

bool CEmp::Emp()
{
	int i=0, j=0;
	int nColIndex=0, nRowCount=0;
	CTString szTemp("");
	CDWordArray arrCol;
	CDoubleVector v;
	CDoubleMatrix dataMatrix;	

	for (i=0; i<m_tVarX.iCount; i++)
	{
		nColIndex = m_tVarX.pValue[i];
		//释放
		arrCol.RemoveAll();
		dataMatrix.destroy();
		//添加
		arrCol.Add(nColIndex);
		nRowCount = m_pDataInterface->GetColsData(arrCol,dataMatrix,2);
		if (nRowCount < 1)
		{
			szTemp.Format("%s中的数据不存在，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			m_szErrMsg += szTemp;
		}
		v = dataMatrix(0);
		if (m_bExp)
		{
			Exp(v,nColIndex);
		}
		if (m_bDen)
		{
			Den(v,nColIndex,0);
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

//概率密度分布
void CEmp::Den(CDoubleVector &v, int nColIndex, int nMethod)
{
	int i=0, nRowCount=0;
	double fMin=0, fMax=0, fDelta=0;
	CTString szFldName("");
	CTChartLine        *pLine = NULL;
	CTChartScatterPlot *pScatter = NULL;
	CRsltElementChart  *pChart = NULL;
	TDataPointValVal   *pDPVV = NULL;
	CDoubleVector vect;

	v.MaxMin(fMax,fMin);
	szFldName = m_pDataInterface->GetFieldName(nColIndex);
	switch(nMethod)
	{
	case 0:
		{
			fDelta = (fMax - fMin)/m_nDivNum;
			vect = Histogram(v,fMin,fMax,fDelta);
			nRowCount = vect.vlen();
			//组成图形输出  直方图
			pDPVV = new TDataPointValVal[nRowCount*2+2];
			(pDPVV+0)->fYVal = 0;
			(pDPVV+0)->fXVal = fMin;
			for (i=0;i<nRowCount;i++)
			{
				(pDPVV+i*2+1)->fYVal = vect(i);
				(pDPVV+i*2+1)->fXVal = fMin + fDelta * i;
				(pDPVV+i*2+2)->fYVal = vect(i);
				(pDPVV+i*2+2)->fXVal = fMin + fDelta * (i + 1);
			}
			(pDPVV+nRowCount*2+1)->fYVal = 0;
			(pDPVV+nRowCount*2+1)->fXVal = fMin + fDelta * nRowCount;
			pLine = new CTChartLine;
			pLine->SetLine(pDPVV,nRowCount*2+2,ValVal);
			pLine->SetXAxilLabel(szFldName);
			pLine->SetYAxilLabel("值");
			pLine->SetTitle("经验密度函数");
			pChart = new CRsltElementChart("经验密度函数",pLine);
			m_pResult->SetName("经验图"); 
			m_pResult->Add(pChart);
			delete []pDPVV;
		}
		break;
	case 1:
		{
			fDelta = 1.0/sqrt(v.vlen());
			vect = Kernal(v,fMin,fMax,fDelta);
			nRowCount = vect.vlen();
			//组成图形输出  点图
			pDPVV = new TDataPointValVal[nRowCount];	
			for(i=0; i<nRowCount; i++)
			{
				(pDPVV+i)->fXVal = i;
				(pDPVV+i)->fYVal = vect(i);
			}
			pScatter = new CTChartScatterPlot;
			pScatter->SetScatter(pDPVV,nRowCount,ValVal);
			pScatter->SetXAxilLabel(szFldName);
			pScatter->SetYAxilLabel("值");
			pLine->SetTitle("经验密度函数");
			pChart = new CRsltElementChart("经验密度函数",pLine);
			m_pResult->SetName("经验图"); 
			m_pResult->Add(pChart);
			delete []pDPVV;
		}
		break;
	case 2:
		{
			int nStep = (int)sqrt(v.vlen());
			vect = NearestNeigh(v,fMin,fMax,nStep);
			nRowCount = vect.vlen();
			//组成图形输出  点图
			pDPVV = new TDataPointValVal[nRowCount];	
			for(i=0; i<nRowCount; i++)
			{
				(pDPVV+i)->fXVal = i;
				(pDPVV+i)->fYVal = vect(i);
			}
			pScatter = new CTChartScatterPlot;
			pScatter->SetScatter(pDPVV,nRowCount,ValVal);
			pScatter->SetXAxilLabel(szFldName);
			pScatter->SetYAxilLabel("值");
			pLine->SetTitle("经验密度函数");
			pChart = new CRsltElementChart("经验密度函数",pLine);
			m_pResult->SetName("经验图"); 
			m_pResult->Add(pChart);
			delete []pDPVV;
		}
		break;
	default:
		break;
	}
}

//经验累积分布
void CEmp::Exp(CDoubleVector &v, int nColIndex)
{
	int i=0, j=0;
	int nCount=0, nRowCount=0;
	double fValue=0, fMin=0, fMax=0, fDelta=0;
	CTString szFldName("");
	CTChartLine        *pLine = NULL;
	CRsltElementChart  *pChart = NULL;
	TDataPointValVal   *pDPVV = NULL;
	
	int nVlen = v.vlen();
	v.MaxMin(fMax,fMin);
	fDelta =  (fMax - fMin)/m_nDivNum;
	nRowCount = (fMax - fMin)/fDelta;
	//创建向量
	CDoubleVector vect(nRowCount+2);
	
	for (i=0; i<nRowCount+2; i++)
	{
		nCount = 0;
		fValue = fMin + fDelta * i;
		for (j=0; j<nVlen; j++)
		{
			if (v(j) < fValue + fDelta)
				nCount++;
		}
		vect(i) = 1.0*nCount/nVlen;
	}
	//经验累积线图
	szFldName = m_pDataInterface->GetFieldName(nColIndex);
	pDPVV = new TDataPointValVal[nRowCount*2+1];
	(pDPVV+0)->fYVal = 0;
	(pDPVV+0)->fXVal = fMin;
	for (i=0; i<nRowCount; i++)
	{
		(pDPVV+i*2+1)->fYVal = vect(i);
		(pDPVV+i*2+1)->fXVal = fMin + fDelta * i;
		(pDPVV+i*2+2)->fYVal = vect(i);
		(pDPVV+i*2+2)->fXVal = fMin + fDelta * (i+1);
	}
	pLine = new CTChartLine;
	pLine->SetLine(pDPVV,nRowCount*2+1,ValVal);
	pLine->SetXAxilLabel(szFldName);
	pLine->SetYAxilLabel("值");
	pLine->SetTitle("经验分布函数");
	pChart = new CRsltElementChart("经验分布函数",pLine);
	m_pResult->SetName("经验图"); 
	m_pResult->Add(pChart);
	delete []pDPVV;
}

//直方图估计
CDoubleVector CEmp::Histogram(CDoubleVector &v, double fMin, double fMax, double fDelta)
{
	int i = 0, nOff = 0;
	double fVal = 0;

	int nVlen = v.vlen();
	int nLen = (fMax - fMin)/fDelta;
	CDoubleVector vect(nLen+1);
	//计算赋值
	for (i=0; i<nVlen; i++)
	{
		nOff = (int)((v(i) - fMin)/fDelta);
		vect(nOff) += 1;
	}
	for (i=0; i<nLen+1; i++)
	{
		fVal = vect(i)/(nVlen * nLen);
		vect(i) = fVal;
	}
	return vect;
}

//核估计
CDoubleVector CEmp::Kernal(CDoubleVector &v,double fMin,double fMax, double fDelta)
{
	int i=0, j=0;
	int nVlen = v.vlen();
	int  nLen = (fMax - fMin)/fDelta;
	double fValue = nVlen * fDelta;
	CDoubleVector Vect(nLen);
	CDoubleVector tVect(nVlen);

	switch(m_nDistType)
	{
	case 0://正态分布
		for (i=0; i<nLen; i++)
		{
			double fVal1 = fMin + fDelta * i + fDelta * 0.5;
			for (j= 0; j<nVlen; j++)
			{
				double fTmp = (fVal1 - v(j))/fDelta;
				fTmp = exp(-0.5*fTmp*fTmp)/sqrt(2*PI);
				tVect(j) = fTmp;
			}
			Vect(i) = sum(tVect)/fValue;
		}
		break;
	case 1://均值分布
		for (i=0; i<nLen; i++)
		{
			double fVal1 = fMin + fDelta * i + fDelta * 0.5;
			for (j=0; j<nVlen; j++)
			{
				double fTmp = (fVal1 - v(j))/fDelta;
				fTmp = 1.0/(fMax - fMin);
				tVect(i) = fTmp;
			}
			Vect(j) = sum(tVect)/fValue;
		}
		break;
	default:
		break;
	}
	return Vect;
}

//近邻密度估计
CDoubleVector CEmp::NearestNeigh(CDoubleVector &v, double fMin, double fMax, int nStep)
{
	int i=0;
	int nVlen = v.vlen();
	CDoubleVector Vect(nVlen);

	for (i=0; i<nVlen; i++)
	{
		double fValue = ((v(i) - v((i+nStep)/nVlen))*nVlen);
		if (fValue >= -DOUBLE_ZERO && fValue <= DOUBLE_ZERO)
		{
			Vect(i) = 1;
		}
		else
		{
			Vect(i) = 0.5*nStep/fValue;
		}
	}

	return Vect;
}