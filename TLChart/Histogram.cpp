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
	m_nDivNum = DIV_MIN_INT; //�ֿ���
	m_bNormal = false;//�Ƿ���Ҫ��̬����
	m_szErrMsg = "";
	m_nMaxGroup = DIV_MAX_INT;//����ֱ��ͼ����
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
		m_szErrMsg.Format("ֱ��ͼ�е����������û��ָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	for (i=0; i<m_tVarX.iCount; i++)
	{
		nColIndex = m_tVarX.pValue[i];
		nDataType = m_pDataInterface->GetFieldType(nColIndex);
		if (nDataType == fString)
		{ 
			m_szErrMsg.Format("��״ͼ�е�������������Ͳ���Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
	}
	if (m_nMaxGroup < 1)
	{
		m_szErrMsg.Format("ֱ��ͼ�еķ������̫С���޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	else if (m_nMaxGroup > DIV_MAX_INT)
	{
		m_szErrMsg.Format("ֱ��ͼ�еķ������̫��%d�����޷���ͼ�����飡Line=%d,File=%s",DIV_MAX_INT,__LINE__,__FILE__);
		return false;
	}
	return true;
}

CResult *CHistogram::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//ͼ��
	SetChartType(CHART_HISTOGRAM);
	//����
	if (!GetValueFromVo(VO))
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
		return m_pResult;
	}
	//��֤
	if (!Verify())
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
		return m_pResult;
	}
	//��ʼ��
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
	
	//����
	szArrName.RemoveAll();
	if (AnaSyntax::exist(DIV_STR,VOstr)) 
	{
		AnaSyntax::getVAR(DIV_STR, VOstr, szArrName);
		int nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("ֱ��ͼ�ķֿ�������ô������飡Line=%d,File=%s",__LINE__,__FILE__);
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
			m_szErrMsg.Format("ֱ��ͼ�����������ô������飡Line=%d,File=%s",__LINE__,__FILE__);
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
					szTemp.Format("%s�е���Ч����̫�٣����������%s�����ݱ�����%s�����޷���ͼ������,Line=%d,File=%s",m_szChartName.GetData(),szFldNameY.GetData(),szFldNameX.GetData(),__LINE__,__FILE__);
					m_szErrMsg += szTemp;
					arrCol.RemoveAll();
					dataMap.Clear();
					dataMatrix.destroy();
					continue;
				}
				//��Ч����
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
					//ֱ��ͼ
					v.resize(nIndex);
					CalcHistogram(v,szFldNameX+szTemp);
					v.destroy();
					v.create(nRowCount);
					fTmp = v1(k);
					nIndex =0;
					v(nIndex) = v2(vIndex(k)-1);
				}
				//���
				szTemp = GetLabel(dataMap,0,nGroupIndex,fTmp);
				szTemp = "\n" + szTemp;
				//ֱ��ͼ
				v.resize(nIndex);
				CalcHistogram(v,szFldNameX+szTemp);
				v.destroy();
				v.create(nRowCount);
				//�ͷ��ڴ�
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
				szTemp.Format("%s�е���Ч����̫�٣�%s�����޷���ͼ������,Line=%d,File=%s",m_szChartName.GetData(),szFldNameX.GetData(),__LINE__,__FILE__);
				m_szErrMsg += szTemp;
				arrCol.RemoveAll();
				dataMatrix.destroy();
				continue;
			}
			//��Ч����
			v = dataMatrix(0);
			//ֱ��ͼ
			CalcHistogram(v,szFldNameX);
			//�ͷ��ڴ�
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

	//����
	v.Sort();
	//ֱ��ͼ
	CTChartHistogram *pHistogram= new CTChartHistogram;
	pHistogram->SetNoraml(true);
	pHistogram->SetDiviCount(m_nDivNum);
	pHistogram->CalcXYAxis(v,&pDPSV1,m_nDivNum);  //ֱ��ͼ
	pHistogram->CalcNormalCurve(v,&pDPSV2,nPtCnt);//����ͼ
	pHistogram->GetParam(fMean,fStdv,fSum);
	szTemp.Format("��ֵ��%.2f\n��׼�%.2f\n��¼����%.0f",fMean,fStdv,fSum);
	pHistogram->SetHistogram(pDPSV1,m_nDivNum,ValVal,szTemp);
	if (m_bNormal)	pHistogram->SetData(pDPSV2,nPtCnt,ELine,ValVal);
	pHistogram->SetXAxilLabel(szXAxisLabel);
	pHistogram->SetYAxilLabel("Ƶ��");
	pHistogram->SetTitle("ֱ��ͼ");
	CRsltElementChart *pChart = new CRsltElementChart("ֱ��ͼ",pHistogram);
	m_pResult->SetName("ֱ��ͼ"); 
	m_pResult->Add(pChart);
	return true;
}
