// Correlation.cpp: implementation of the CCorrelation class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Correlation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCorrelation::CCorrelation()
{
	m_szErrMsg = "";
	m_nDiff    = 0;    //差分阶数
	m_nKappa   = 12;   //最大滞后阶数
	m_bSelf    = true; //自相关图
	m_bPartial = false; //偏相关图
}

CCorrelation::~CCorrelation()
{

}

bool CCorrelation::Verify()
{
	if (m_szChartType == CHART_SELFCORRELATIONS)
	{
		if (m_tVarY.iCount < 1)
		{
			m_szErrMsg.Format("%s中的变量参数不存在，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		if (m_nDiff < SELFDIFF_MIN_INT || m_nDiff > SELFDIFF_MAX_INT)
		{
			m_szErrMsg.Format("%s中的差分阶数不符合要求（%d－%d），无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),SELFDIFF_MIN_INT,SELFDIFF_MAX_INT,__LINE__,__FILE__);
			return false;
		}
		if (m_nKappa < SELFMAXLAG_MIN_INT || m_nKappa > SELFMAXLAG_MAX_INT)
		{
			m_szErrMsg.Format("%s中的最大滞后阶数不符合要求（%d－%d），无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),SELFMAXLAG_MIN_INT,SELFMAXLAG_MAX_INT,__LINE__,__FILE__);
			return false;
		}
	}
	else if (m_szChartType == CHART_CROSSCORRELATIONS)
	{
		if (m_tVarY.iCount < 2)
		{
			m_szErrMsg.Format("%s中的变量参数不能少于两个，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		if (m_nDiff < CROSSDIFF_MIN_INT || m_nDiff > CROSSDIFF_MAX_INT)
		{
			m_szErrMsg.Format("%s中的差分阶数不符合要求（%d－%d），无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),CROSSDIFF_MIN_INT,CROSSDIFF_MAX_INT,__LINE__,__FILE__);
			return false;
		}
		if (m_nKappa < CROSSMAXLAG_MIN_INT || m_nKappa > CROSSMAXLAG_MAX_INT)
		{
			m_szErrMsg.Format("%s中的最大滞后阶数不符合要求（%d－%d），无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),CROSSMAXLAG_MIN_INT,CROSSMAXLAG_MAX_INT,__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("错误的图形类型，无法绘图，请检查！Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	return true;
}

bool CCorrelation::GetValueFromVo(string VO)
{
	if (!ParseTypeMode(VO)) return false;
	if (!ParseDataType(VO)) return false;
	if (!ParseAxis(VO)) return false;
	if (!ParseVar(VO)) return false;
	if (!ParseParam(VO)) return false;
	if (!ParseDefLine(VO)) return false;
	if (!ParseWeight(VO)) return false;
	if (!ParseOutput(VO)) return false;
	return true;
}

bool CCorrelation::ParseTypeMode(string VO)
{
	return true;
}

bool CCorrelation::ParseParam(string VO)
{
	int i=0, nSize=0;
	CTString szTemp;
	CTStringArray szArrName;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	//解析－无类型，只有一种散点图
	szArrName.RemoveAll();
	if (AnaSyntax::exist(DIFFRANK_STR,VOstr)) 
	{
		AnaSyntax::getVAR(DIFFRANK_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("%s中的差分阶数不确定，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		m_nDiff = atoi(szTemp) > 0 ? atoi(szTemp) : 0;
	}
	//最大滞后阶数
	szArrName.RemoveAll();
	if (AnaSyntax::exist(MAXLAGRANK_STR,VOstr)) 
	{
		AnaSyntax::getVAR(MAXLAGRANK_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("%s中的最大滞后阶数不确定，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		m_nKappa = atoi(szTemp) > 0 ? atoi(szTemp) : 0;
	}
	//图形
	szArrName.RemoveAll();
	if (AnaSyntax::exist(PLOT_STR,VOstr)) 
	{
		AnaSyntax::getVAR(PLOT_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("%s中的输出图形不确定，请检查！Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<nSize; i++)
		{
			szTemp = szArrName.GetAt(i);
			szTemp.MakeUpper();
			if (szTemp == SELFCORR_STR)
			{
				m_bSelf = true;
			}
			else if (szTemp == PARTIALCORR_STR)
			{
				m_bPartial = true;
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

bool CCorrelation::IsValidType(int nColIndex)
{
	int nDataType=0;
	CTString szTemp("");
	CTString szFldName("");
	
	nDataType = m_pDataInterface->GetFieldType(nColIndex);
	szFldName = m_pDataInterface->GetFieldName(nColIndex);
	if (nDataType != fInt && nDataType != fDouble && 
		nDataType != fBoolean && nDataType != fCurrency)
	{
		szTemp.Format("%s中的变量参数（%s）不是数字型，无法绘图，请检查！Line=%d,File=%s\n",m_szChartName.GetData(),szFldName.GetData(),__LINE__,__FILE__);
		m_szErrMsg += szTemp;
		return false;
	}
	return true;
}

bool CCorrelation::IsValidData(int nRowCount,int nColIndex1, int nColIndex2)
{
	int nDataType=0;
	CTString szTemp("");
	CTString szFldName1("");
	CTString szFldName2("");
	
	szFldName1 = m_pDataInterface->GetFieldName(nColIndex1);
	if (nColIndex2 > -1)
	{
		if (nRowCount <= m_nDiff + 1 || nRowCount < 3)
		{
			szFldName2 = m_pDataInterface->GetFieldName(nColIndex2);
			szTemp.Format("%s中的变量参数（%s-%s）样本数太少，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),szFldName1.GetData(),szFldName2.GetData(),__LINE__,__FILE__);
			m_szErrMsg += szTemp;
			return false;
		}
	}
	else
	{
		if (nRowCount <= m_nDiff + 1)
		{
			szTemp.Format("%s中的变量参数（%s）样本数太少，无法绘图，请检查！Line=%d,File=%s",m_szChartName.GetData(),szFldName1.GetData(),__LINE__,__FILE__);
			m_szErrMsg += szTemp;
			return false;
		}
	}
	return true;
}
