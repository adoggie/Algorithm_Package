// Scatter.cpp: implementation of the CScatter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Scatter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScatter::CScatter()
{

}

CScatter::~CScatter()
{

}

bool CScatter::Verify()
{
	int i = 0;
	int nColIndex = 0, nDataType = 0;
	CTString szFldName("");

	if (m_eType == SIMPLESCATTER)
	{
		if (m_tVarX.iCount != 1)
		{
			m_szErrMsg.Format("散点图中的X轴参数没有指定，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_tVarY.iCount != 1)
		{
			m_szErrMsg.Format("散点图中的Y轴参数没有指定，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		nColIndex = m_tVarX.pValue[0];
		nDataType = m_pDataInterface->GetFieldType(nColIndex);
		if (nDataType == fString)
		{ 
			m_szErrMsg.Format("散点图中的X轴变量参数类型不能为字符型，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		nColIndex = m_tVarY.pValue[0];
		nDataType = m_pDataInterface->GetFieldType(nColIndex);
		if (nDataType == fString)
		{ 
			m_szErrMsg.Format("散点图中的Y轴变量参数类型不能为字符型，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
	}
	else if (m_eType == SUPERSCATTER)
	{
		if (m_tVarY.iCount < 4 || m_tVarY.iCount % 4 != 0)
		{
			m_szErrMsg.Format("散点图中的成对变量参数设置错误(必须为四的倍数，且两两配对)，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<m_tVarY.iCount; i++)
		{
			nColIndex = m_tVarY.pValue[i];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			if (nDataType == fString)
			{ 
				szFldName = m_pDataInterface->GetFieldName(nColIndex);
				m_szErrMsg.Format("散点图中的成对变量参数类型不能为字符型-%s，无法绘图，请检查！Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}


CResult *CScatter::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//图形
	SetChartType(CHART_SCATTER);
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
	if (m_eType == SIMPLESCATTER) bRet = CalcScatter1();
	else if (m_eType == SUPERSCATTER) bRet = CalcScatter2();
	if (!bRet)
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
		return m_pResult;
	}
	//添加警告信息
	if (strlen(m_szErrMsg) > 0)
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
	}
	return m_pResult;
}

bool CScatter::GetValueFromVo(string VO)
{
	if (!ParseTypeMode(VO)) return false;
	if (!ParseDataType(VO)) return false;
	if (!ParseAxis(VO)) return false;
	if (!ParseVar(VO)) return false;
	if (!ParseDefLine(VO)) return false;
	if (!ParseWeight(VO)) return false;
	if (!ParseOutput(VO)) return false;
	return true;
}

bool CScatter::ParseTypeMode(string VO)
{
	int nSize=0;
	CTString szTemp;
	CTStringArray szArrName;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	//解析－无类型，只有一种散点图
	szArrName.RemoveAll();
	if (AnaSyntax::exist(TYPE_STR,VOstr)) 
	{
		AnaSyntax::getVAR(TYPE_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("散点图中的图表类型不确定，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		szTemp.MakeUpper();
		if (szTemp == SIMPLESCATTER_STR)
		{
			m_eType = SIMPLESCATTER;
		}
		else if (szTemp == SUPERSCATTER_STR)
		{
			m_eType = SUPERSCATTER;
		}
		else
		{
			m_szErrMsg.Format("散点图中的图表类型错误－%s，请检查！Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("散点图的图表类型没有被指定，请检查！Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	return true;
}

bool CScatter::CalcScatter1()
{
	int i=0, nRowCnt=0, nIndex=0;
	int nColIndX=0, nColIndY=0, nColIndD=0;
	int nDataTyX=0, nDataTyY=0, nDataTyD=0;
	CDWordArray arrCol;
	CMapManager dataMap;
	CDoubleMatrix dataMatrix;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	CTString szFldNameD("");

	//X轴数据
	nColIndX = m_tVarX.pValue[0];
	nDataTyX = m_pDataInterface->GetFieldType(nColIndX); 
	szFldNameX = m_pDataInterface->GetFieldName(nColIndX); 
	//Y轴数据
	nColIndY = m_tVarY.pValue[0];
	nDataTyY = m_pDataInterface->GetFieldType(nColIndY); 
	szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 
	if (nDataTyX != fInt && nDataTyX != fDouble && nDataTyX != fBoolean && nDataTyX != fCurrency)
	{
		m_szErrMsg.Format("散点图中的X轴参数不是数字型，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (nDataTyY != fInt && nDataTyY != fDouble && nDataTyY != fBoolean && nDataTyY != fCurrency)
	{
		m_szErrMsg.Format("散点图中的Y轴参数不是数字型，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	arrCol.Add(nColIndX);
	arrCol.Add(nColIndY);
	//点坐标
	if (m_tVarD.iCount == 1)
	{
		nDataTyD = fString;
		nColIndD = m_tVarD.pValue[0];
		nDataTyD = m_pDataInterface->GetFieldType(nColIndD); 
		arrCol.Add(nColIndD);
	}
	//得到数据
	nRowCnt = m_pDataInterface->GetColsData(arrCol,dataMatrix,dataMap);
	if (nRowCnt > MAX_VALID_INT)
	{
		m_szErrMsg.Format("散点图中的分组数据超过 %d 组，无法绘图，请检查！Line=%d,File=%s",MAX_VALID_INT,__LINE__,__FILE__);	
		return false;
	}
	//处理数据
	tagDataPointValVal *pDPSV = NULL;
	CTChartScatterPlot *pScatter = new CTChartScatterPlot;
	if (m_tVarD.iCount == 1)
	{
		CIntVector vecInd;
		CDoubleVector vt(dataMatrix(2));
		vt.Sort(vecInd);//点坐标排序
		pDPSV = new tagDataPointValVal[nRowCnt];
		double fD = dataMatrix(2)(0);
		for (i=0; i<nRowCnt; i++)
		{
			if (fD == dataMatrix(2)(i))
			{
				pDPSV[nIndex].fXVal = dataMatrix(0)(vecInd(i)-1);
				pDPSV[nIndex].fYVal = dataMatrix(1)(vecInd(i)-1);
				nIndex ++;
			}
			else
			{
				szTemp = GetLabel(dataMap,2,nColIndD,fD);
				pScatter->SetScatter(pDPSV,nIndex,ValVal,szTemp);
				delete []pDPSV;
				nIndex = 0;
				fD = dataMatrix(2)(i);
				pDPSV = new tagDataPointValVal[nRowCnt];
				pDPSV[nIndex].fXVal = dataMatrix(0)(vecInd(i)-1);
				pDPSV[nIndex].fYVal = dataMatrix(1)(vecInd(i)-1);
				nIndex ++;
			}
			if (i==nRowCnt-1)
			{
				szTemp = GetLabel(dataMap,2,nColIndD,fD);
				pScatter->SetScatter(pDPSV,nIndex,ValVal,szTemp);
				delete []pDPSV;
			}
		}
	}
	else
	{
		pDPSV = new tagDataPointValVal[nRowCnt];
		for (i=0; i<nRowCnt; i++)
		{
			pDPSV[i].fXVal = dataMatrix(0)(i);
			pDPSV[i].fYVal = dataMatrix(1)(i);
		}
		pScatter->SetScatter(pDPSV,nRowCnt,ValVal);
		delete []pDPSV;
	}
	//图形	
	pScatter->SetTitle("散点图");
	pScatter->SetXAxilLabel(szFldNameX);
	pScatter->SetYAxilLabel(szFldNameY);
	CRsltElementChart *pChart = new CRsltElementChart("散点图",pScatter);
	m_pResult->SetName("散点图"); 
	m_pResult->Add(pChart);
	return true;
}

bool CScatter::CalcScatter2()
{
	int i=0, j=0, nRowCnt=0;
	int nColIndex=0, nDataType=0;
	CDWordArray arrCol;
	CDoubleMatrix dataMatrix;
	CTString szTemp("");
	CTString szFldName1("");
	CTString szFldName2("");

	m_szErrMsg.Empty();
	//轴数据
	for (i=0; i<m_tVarY.iCount; i++)
	{
		nColIndex = m_tVarY.pValue[i];
		nDataType = m_pDataInterface->GetFieldType(nColIndex); 
		if (nDataType != fInt && nDataType != fDouble && nDataType != fBoolean && nDataType != fCurrency)
		{
			szTemp = m_pDataInterface->GetFieldName(nColIndex);
			m_szErrMsg.Format("散点图中的成对变量参数（%s）不是数字型，无法绘图，请检查！Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	//处理数据
	tagDataPointValVal *pDPSV = NULL;
	CTChartScatterPlot *pScatter = new CTChartScatterPlot;
	for (i=0; i<m_tVarY.iCount; i=i+2)
	{
		arrCol.RemoveAll();
		dataMatrix.destroy();
		nColIndex = m_tVarY.pValue[i];
		arrCol.Add(nColIndex);
		szFldName1 = m_pDataInterface->GetFieldName(nColIndex);
		nColIndex = m_tVarY.pValue[i+1];
		arrCol.Add(nColIndex);
		szFldName2 = m_pDataInterface->GetFieldName(nColIndex);
		//得到数据
		nRowCnt = m_pDataInterface->GetColsData(arrCol,dataMatrix,0);
		if (nRowCnt > MAX_VALID_INT)
		{
			szTemp.Format("(%s-%s)-散点图中的分组数据超过 %d 组，无法绘图，请检查！Line=%d,File=%s",szFldName1.GetData(),szFldName2.GetData(),MAX_VALID_INT,__LINE__,__FILE__);	
			m_szErrMsg += szTemp;
			continue;
		}
		//add
		if (nRowCnt < 1) continue;
		pDPSV = new tagDataPointValVal[nRowCnt];
		for (j=0; j<nRowCnt; j++)
		{
			pDPSV[j].fYVal = dataMatrix(0)(j);
			pDPSV[j].fXVal = dataMatrix(1)(j);
		}
		szTemp = szFldName1 + "-" + szFldName2;
		pScatter->SetScatter(pDPSV,nRowCnt,ValVal,szTemp);
		delete []pDPSV;
	}
	//图形	
	pScatter->SetTitle("散点图");
	CRsltElementChart *pChart = new CRsltElementChart("散点图",pScatter);
	m_pResult->SetName("散点图"); 
	m_pResult->Add(pChart);
	return true;
}