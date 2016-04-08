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
	m_nDiff    = 0;    //��ֽ���
	m_nKappa   = 12;   //����ͺ����
	m_bSelf    = true; //�����ͼ
	m_bPartial = false; //ƫ���ͼ
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
			m_szErrMsg.Format("%s�еı������������ڣ��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		if (m_nDiff < SELFDIFF_MIN_INT || m_nDiff > SELFDIFF_MAX_INT)
		{
			m_szErrMsg.Format("%s�еĲ�ֽ���������Ҫ��%d��%d�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),SELFDIFF_MIN_INT,SELFDIFF_MAX_INT,__LINE__,__FILE__);
			return false;
		}
		if (m_nKappa < SELFMAXLAG_MIN_INT || m_nKappa > SELFMAXLAG_MAX_INT)
		{
			m_szErrMsg.Format("%s�е�����ͺ����������Ҫ��%d��%d�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),SELFMAXLAG_MIN_INT,SELFMAXLAG_MAX_INT,__LINE__,__FILE__);
			return false;
		}
	}
	else if (m_szChartType == CHART_CROSSCORRELATIONS)
	{
		if (m_tVarY.iCount < 2)
		{
			m_szErrMsg.Format("%s�еı����������������������޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		if (m_nDiff < CROSSDIFF_MIN_INT || m_nDiff > CROSSDIFF_MAX_INT)
		{
			m_szErrMsg.Format("%s�еĲ�ֽ���������Ҫ��%d��%d�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),CROSSDIFF_MIN_INT,CROSSDIFF_MAX_INT,__LINE__,__FILE__);
			return false;
		}
		if (m_nKappa < CROSSMAXLAG_MIN_INT || m_nKappa > CROSSMAXLAG_MAX_INT)
		{
			m_szErrMsg.Format("%s�е�����ͺ����������Ҫ��%d��%d�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),CROSSMAXLAG_MIN_INT,CROSSMAXLAG_MAX_INT,__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("�����ͼ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
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
	//�����������ͣ�ֻ��һ��ɢ��ͼ
	szArrName.RemoveAll();
	if (AnaSyntax::exist(DIFFRANK_STR,VOstr)) 
	{
		AnaSyntax::getVAR(DIFFRANK_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("%s�еĲ�ֽ�����ȷ�������飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		m_nDiff = atoi(szTemp) > 0 ? atoi(szTemp) : 0;
	}
	//����ͺ����
	szArrName.RemoveAll();
	if (AnaSyntax::exist(MAXLAGRANK_STR,VOstr)) 
	{
		AnaSyntax::getVAR(MAXLAGRANK_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("%s�е�����ͺ������ȷ�������飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		m_nKappa = atoi(szTemp) > 0 ? atoi(szTemp) : 0;
	}
	//ͼ��
	szArrName.RemoveAll();
	if (AnaSyntax::exist(PLOT_STR,VOstr)) 
	{
		AnaSyntax::getVAR(PLOT_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("%s�е����ͼ�β�ȷ�������飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
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
				m_szErrMsg.Format("%s�е����ͼ�����ʹ���%s�����飡Line=%d,File=%s",m_szChartName.GetData(),szTemp.GetData(),__LINE__,__FILE__);
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
		szTemp.Format("%s�еı���������%s�����������ͣ��޷���ͼ�����飡Line=%d,File=%s\n",m_szChartName.GetData(),szFldName.GetData(),__LINE__,__FILE__);
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
			szTemp.Format("%s�еı���������%s-%s��������̫�٣��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),szFldName1.GetData(),szFldName2.GetData(),__LINE__,__FILE__);
			m_szErrMsg += szTemp;
			return false;
		}
	}
	else
	{
		if (nRowCount <= m_nDiff + 1)
		{
			szTemp.Format("%s�еı���������%s��������̫�٣��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),szFldName1.GetData(),__LINE__,__FILE__);
			m_szErrMsg += szTemp;
			return false;
		}
	}
	return true;
}
