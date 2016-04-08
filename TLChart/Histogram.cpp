// Histogram.cpp: implementation of the CHistogram class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Histogram.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHistogram::CHistogram()
{
	m_nDivNum = DIV_MIN_INT; //分块数
	m_bNormal = false;//是否需要正态曲线
	m_szErrMsg = "";
	m_nMaxGroup = DIV_MAX_INT;//分组直方图组数
}

CHistogram::~CHistogram()
{

}

bool CHistogram::Verify()
{
	int i = 0;
	int nColIndex = 0, nDataType = 0;
	CTString szFldName("");

	if (m_tVarX.iCount < 1)
	{
		m_szErrMsg.Format("直方图中的轴变量参数没有指定，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	for (i=0; i<m_tVarX.iCount; i++)
	{
		nColIndex = m_tVarX.pValue[i];
		nDataType = m_pDataInterface->GetFieldType(nColIndex);
		if (nDataType == fString)
		{ 
			m_szErrMsg.Format("盒状图中的轴变量参数类型不能为字符型，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
	}
	if (m_nMaxGroup < 1)
	{
		m_szErrMsg.Format("直方图中的分组参数太小，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	else if (m_nMaxGroup > DIV_MAX_INT)
	{
		m_szErrMsg.Format("直方图中的分组参数太大（%d），无法绘图，请检查！Line=%d,File=%s",DIV_MAX_INT,__LINE__,__FILE__);
		return false;
	}
	return true;
}

CResult *CHistogram::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//图形
	SetChartType(CHART_HISTOGRAM);
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
	bRet = CalcHistogram();
	if (!bRet || m_szErrMsg.GetLength() > 0)
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
		return m_pResult;
	}
	return m_pResult;
}

bool CHistogram::GetValueFromVo(string VO)
{
	if (!ParseTypeMode(VO)) return false;
	if (!ParseDataType(VO)) return false;
	if (!ParseAxis(VO)) return false;
	if (!ParseVar(VO)) return false;
	if (!ParseDefLine(VO)) return false;
	if (!ParseWeight(VO)) return false;
	if (!ParseOutput(VO)) return false;
	if (!ParseParam(VO)) return false;
	return true;
}

bool CHistogram::ParseTypeMode(string VO)
{
	return true;
}
// /DIV 2 /PLOT NORMAL 
bool CHistogram::ParseParam(string VO)
{
	CTString szTemp("");
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
			m_szErrMsg.Format("直方图的分块参数设置错误，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		m_nDivNum = atoi(szTemp);
		if (m_nDivNum < DIV_MIN_INT)
			m_nDivNum = DIV_MIN_INT;
		if (m_nDivNum > DIV_MAX_INT)
			m_nDivNum = DIV_MAX_INT;
	}
	//Normal
	if (AnaSyntax::findstr(PLOT_STR, NORMAL_STR,VOstr))
		m_bNormal = true;
	if (AnaSyntax::exist(MAXNUM_STR,VOstr)) 
	{
		AnaSyntax::getVAR(MAXNUM_STR, VOstr, szArrName);
		int nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("直方图的最大个数设置错误，请检查！Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		m_nMaxGroup = atoi(szTemp);
	}
	return true;
}

bool CHistogram::CalcHistogram()
{
	int i=0, j=0, k=0;
	int nIndex=0, nPtCnt=0, nGroupIndex=0;
	int nColIndex=0, nRowCount=0, nDataType=0;
	double fTmp=0;
	CDWordArray arrCol;
	CMapManager dataMap;
	CDoubleMatrix dataMatrix;
	CIntVector vIndex;
	CDoubleVector v;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");

	if (m_tVarY.iCount > 0)
	{
		for (i=0; i<m_tVarY.iCount; i++)
		{
			nGroupIndex = m_tVarY.pValue[i];
			szFldNameY = m_pDataInterface->GetFieldName(nGroupIndex); 
			for (j=0; j<m_tVarX.iCount; j++)
			{
				nColIndex = m_tVarX.pValue[j];
				arrCol.Add(nGroupIndex);
				arrCol.Add(nColIndex);
				szFldNameX = m_pDataInterface->GetFieldName(nColIndex); 
				nRowCount = m_pDataInterface->GetColsData(arrCol,dataMatrix,dataMap);
				if (nRowCount < 1)
				{
					szTemp.Format("%s中的有效数据太少（分组变量＝%s，数据变量＝%s），无法绘图，请检查,Line=%d,File=%s",m_szChartName.GetData(),szFldNameY.GetData(),szFldNameX.GetData(),__LINE__,__FILE__);
					m_szErrMsg += szTemp;
					arrCol.RemoveAll();
					dataMap.Clear();
					dataMatrix.destroy();
					continue;
				}
				//有效数据
				v.create(nRowCount);
				CDoubleVector v1 = dataMatrix(0);
				CDoubleVector v2 = dataMatrix(1);
				v1.Sort(vIndex);
				fTmp = v1(0);
				nIndex = 0;
				for (k=0; k<nRowCount; k++)
				{
					if (fTmp == v1(k))
					{
						v(nIndex) = v2(vIndex(k)-1);
						nIndex ++;
						continue;
					}
					szTemp = GetLabel(dataMap,0,nGroupIndex,fTmp);
					szTemp = "\n" + szTemp;
					//直方图
					v.resize(nIndex);
					CalcHistogram(v,szFldNameX+szTemp);
					v.destroy();
					v.create(nRowCount);
					fTmp = v1(k);
					nIndex =0;
					v(nIndex) = v2(vIndex(k)-1);
				}
				//最后
				szTemp = GetLabel(dataMap,0,nGroupIndex,fTmp);
				szTemp = "\n" + szTemp;
				//直方图
				v.resize(nIndex);
				CalcHistogram(v,szFldNameX+szTemp);
				v.destroy();
				v.create(nRowCount);
				//释放内存
				vIndex.destroy();
				dataMap.Clear();
				arrCol.RemoveAll();
				dataMatrix.destroy();
			}
		}
	}
	else
	{
		for (i=0; i<m_tVarX.iCount; i++)
		{
			nColIndex = m_tVarX.pValue[i];
			arrCol.Add(nColIndex);
			szFldNameX = m_pDataInterface->GetFieldName(nColIndex); 
			nRowCount = m_pDataInterface->GetColsData(arrCol,dataMatrix,0);
			if (nRowCount < 1)
			{
				szTemp.Format("%s中的有效数据太少（%s），无法绘图，请检查,Line=%d,File=%s",m_szChartName.GetData(),szFldNameX.GetData(),__LINE__,__FILE__);
				m_szErrMsg += szTemp;
				arrCol.RemoveAll();
				dataMatrix.destroy();
				continue;
			}
			//有效数据
			v = dataMatrix(0);
			//直方图
			CalcHistogram(v,szFldNameX);
			//释放内存
			arrCol.RemoveAll();
			dataMatrix.destroy();
		}
	}
	return true;
}
bool CHistogram::CalcHistogram(CDoubleVector &v, CTString szXAxisLabel)
{
	int nPtCnt=0;
	double fMean=0, fStdv=0, fSum=0;
	void *pDPSV1=NULL, *pDPSV2=NULL;
	CTString szTemp("");

	//排序
	v.Sort();
	//直方图
	CTChartHistogram *pHistogram= new CTChartHistogram;
	pHistogram->SetNoraml(true);
	pHistogram->SetDiviCount(m_nDivNum);
	pHistogram->CalcXYAxis(v,&pDPSV1,m_nDivNum);  //直方图
	pHistogram->CalcNormalCurve(v,&pDPSV2,nPtCnt);//曲线图
	pHistogram->GetParam(fMean,fStdv,fSum);
	szTemp.Format("均值：%.2f\n标准差：%.2f\n记录数：%.0f",fMean,fStdv,fSum);
	pHistogram->SetHistogram(pDPSV1,m_nDivNum,ValVal,szTemp);
	if (m_bNormal)	pHistogram->SetData(pDPSV2,nPtCnt,ELine,ValVal);
	pHistogram->SetXAxilLabel(szXAxisLabel);
	pHistogram->SetYAxilLabel("频数");
	pHistogram->SetTitle("直方图");
	CRsltElementChart *pChart = new CRsltElementChart("直方图",pHistogram);
	m_pResult->SetName("直方图"); 
	m_pResult->Add(pChart);
	return true;
}
