// Bar.cpp: implementation of the CBar class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Bar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBar::CBar()
{
	m_szErrMsg = "";
	m_bStacked = false;
}

CBar::~CBar()
{

}

bool CBar::Verify()
{
	int i = 0;
	int nColIndex = 0, nDataType = 0;
	CTString szFldName("");

	switch(m_eType)
	{
	case SIMPLEBAR:
		{
			switch(m_eMode)
			{
			case CLASS:
				{
					if (m_tVarX.iCount != 1)
					{
						m_szErrMsg.Format("��״ͼ�е������δָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_nAVType & AXIS_DESC_INT)
					{
						m_szErrMsg.Format("��״ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if ((m_nAVType & VAR_DESC_INT  && m_tVarY.iCount == 1) ||
						(m_nAVType & VAR_NDESC_INT && m_tVarY.iCount != 1))
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_tVarY.iCount == 1)
					{
						nColIndex = m_tVarY.pValue[0];
						nDataType = m_pDataInterface->GetFieldType(nColIndex);
						if (nDataType == fString)
						{ 
							m_szErrMsg.Format("��״ͼ�е�ֵ�����������Ͳ���Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
							return false;
						}
					}
				}
				break;
			case VARI:
				{
					if (m_nAVType & AXIS_DESC_INT)
					{
						m_szErrMsg.Format("��״ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_tVarX.iCount < 2)
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ�������������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_arrX.GetSize() < 1)
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ������������û�ж��壬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					for (i=0; i<m_tVarX.iCount; i++)
					{
						nColIndex = m_tVarX.pValue[i];
						nDataType = m_pDataInterface->GetFieldType(nColIndex);
						if (nDataType == fString)
						{ 
							szFldName = m_pDataInterface->GetFieldName(nColIndex);
							m_szErrMsg.Format("��״ͼ�е�������������Ͳ���Ϊ�ַ���-%s���޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
							return false;
						}
					}
				}
				break;
			case VALUE:
				{
					if (m_nAVType & VAR_DESC_INT)
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ�����������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_tVarY.iCount != 1)
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ����������Ҫָ��һ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if ((m_nAVType & AXIS_DESC_INT  && m_tVarX.iCount == 1) ||
						(m_nAVType & AXIS_NDESC_INT && m_tVarX.iCount != 1))
					{
						m_szErrMsg.Format("��״ͼ�е��������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					nColIndex = m_tVarY.pValue[0];
					nDataType = m_pDataInterface->GetFieldType(nColIndex);
					if (nDataType == fString)
					{ 
						m_szErrMsg.Format("��״ͼ�е�ֵ�����������Ͳ���Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
				}
				break;
			default: return false;
			}
		}
		break;
	case GROUPBAR:
	case SEGMENTBAR:
		{
			switch(m_eMode)
			{
			case CLASS:
				{
					if (m_tVarX.iCount != 1)
					{
						m_szErrMsg.Format("��״ͼ�е������δָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_nAVType & AXIS_DESC_INT)
					{
						m_szErrMsg.Format("��״ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if ((m_nAVType & VAR_DESC_INT  && m_tVarY.iCount == 1) ||
						(m_nAVType & VAR_NDESC_INT && m_tVarY.iCount != 1))
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_tVarD.iCount != 1)
					{
						m_szErrMsg.Format("��״ͼ�е��Զ����б�����������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_tVarY.iCount == 1)
					{
						nColIndex = m_tVarY.pValue[0];
						nDataType = m_pDataInterface->GetFieldType(nColIndex);
						if (nDataType == fString)
						{ 
							m_szErrMsg.Format("��״ͼ�е�ֵ�����������Ͳ���Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
							return false;
						}
					}
				}
				break;
			case VARI:
				{
					if (m_nAVType & AXIS_DESC_INT)
					{
						m_szErrMsg.Format("��״ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_tVarX.iCount != 1)
					{
						m_szErrMsg.Format("��״ͼ�е������û��ָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_tVarY.iCount < 2)
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ�������������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_arrY.GetSize() < 1)
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ������������û�ж��壬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					for (i=0; i<m_tVarY.iCount; i++)
					{
						nColIndex = m_tVarY.pValue[i];
						nDataType = m_pDataInterface->GetFieldType(nColIndex);
						if (nDataType == fString)
						{ 
							szFldName = m_pDataInterface->GetFieldName(nColIndex);
							m_szErrMsg.Format("��״ͼ�е�ֵ�����������Ͳ���Ϊ�ַ���-%s���޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
							return false;
						}
					}
				}
				break;
			case VALUE:
				{
					if (m_nAVType & VAR_DESC_INT)
					{
						m_szErrMsg.Format("��״ͼ�е�ֵ�����������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					if (m_tVarX.iCount == 1)
					{
						if (m_tVarY.iCount < 1)
						{
							m_szErrMsg.Format("��״ͼ�е�ֵ��������δָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
							return false;
						}
					}
					else
					{
						if (m_tVarY.iCount < 2)
						{
							m_szErrMsg.Format("��״ͼ�е�ֵ�������������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
							return false;
						}
					}
					if ((m_nAVType & AXIS_DESC_INT  && m_tVarX.iCount == 1) ||
						(m_nAVType & AXIS_NDESC_INT && m_tVarX.iCount != 1))
					{
						m_szErrMsg.Format("��״ͼ�е��������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
						return false;
					}
					for (i=0; i<m_tVarY.iCount; i++)
					{
						nColIndex = m_tVarY.pValue[i];
						nDataType = m_pDataInterface->GetFieldType(nColIndex);
						if (nDataType == fString)
						{ 
							szFldName = m_pDataInterface->GetFieldName(nColIndex);
							m_szErrMsg.Format("��״ͼ�е�ֵ�����������Ͳ���Ϊ�ַ���-%s���޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
							return false;
						}
					}
				}
				break;
			default: return false;
			}
		}
		break;
	default: return false;
	}
	return true;
}

bool CBar::GetValueFromVo(string VO)
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

bool CBar::ParseTypeMode(string VO)
{
	int nSize=0;
	CTString szTemp;
	CTStringArray szArrName;

	AnaWord VOstr;
	VOstr.Import(VO);
	//����
	if (AnaSyntax::exist(TYPE_STR,VOstr)) //��״ͼ|��״ͼ|����ͼ|ɢ��ͼ|����ͼ|
	{
		AnaSyntax::getVAR(TYPE_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("��״ͼ���Ͳ�ȷ�������飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		szTemp.MakeUpper();
		if (szTemp == SIMPLEBAR_STR)//����״
		{
			m_eType = SIMPLEBAR;
		}
		else if (szTemp == GROUPBAR_STR)//������״
		{
			m_eType = GROUPBAR;
		}
		else if (szTemp == SEGMENTBAR_STR)//�ֶ���״
		{
			m_eType = SEGMENTBAR;
		}
		else
		{
			m_szErrMsg.Format("��״ͼ�в�������Ӧ�����ͣ�%s�����飡Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("��״ͼ������û�б�ָ�������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	szArrName.RemoveAll();
	if (AnaSyntax::exist(MODE_STR,VOstr)) 
	{
		AnaSyntax::getVAR(MODE_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("��״ͼ�е��������Ͳ�ȷ�������飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		szTemp.MakeUpper();
		if (szTemp == CLASS_STR)
		{
			m_eMode = CLASS;
		}
		else if (szTemp == VARIABLE_STR)
		{
			m_eMode = VARI;
		}
		else if (szTemp == VALUE_STR)
		{
			m_eMode = VALUE;
		}
		else
		{
			m_szErrMsg.Format("��״ͼ�е��������Ͳ����ڣ�%s�����飡Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("��״ͼ����������û�б�ָ�������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	return true;
}

CResult *CBar::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//ͼ��
	SetChartType(CHART_BAR);
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
	if (m_eType == SIMPLEBAR)
	{
		if (m_eMode == CLASS)
			bRet = CalcSimpleBar1();
		else if (m_eMode == VARI)
			bRet = CalcSimpleBar2();
		else if (m_eMode == VALUE)
			bRet = CalcSimpleBar3();
	}
	else if (m_eType == GROUPBAR)
	{
		if (m_eMode == CLASS)
			bRet = CalcGroupBar1();
		else if (m_eMode == VARI)
			bRet = CalcGroupBar2();
		else if (m_eMode == VALUE)
			bRet = CalcGroupBar3();
	}
	else if (m_eType == SEGMENTBAR)
	{
		if (m_eMode == CLASS)
			bRet = CalcSegmentBar1();
		else if (m_eMode == VARI)
			bRet = CalcSegmentBar2();
		else if (m_eMode == VALUE)
			bRet = CalcSegmentBar3();
	}
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

//�õ���ͬ�㷨�Ľ��
bool CBar::GetValForFunc(int nGroupD, int nGroupX, EData eData, int &nDataCntY, TDataSet *pDataSetY, int &nRowCntY, TGroupInfor *pGroupInforD, TDataSet *pDataSetYGroup, TDataSet *pDataSetXNoSort, TGroupInfor* pGroupInforX, CStatistics& statistics, double &fValTempY, CTString& szYAxisLabel, bool bString)
{
	int i=0, j=0;
	fValTempY = 0;
	CTArray<int,int> arrGroupIndex;

	nDataCntY = CStatistics::GetGroupData(pDataSetY, nRowCntY, pGroupInforD[nGroupD].pIndex, pDataSetYGroup);
	for (i=0; i<nDataCntY; i++)
	{
		if (bString)
		{
			for (j=0; j<nRowCntY; j++)
			{
				if(pDataSetXNoSort[j].iIndex == abs(pDataSetYGroup[i].iIndex))
				{
					if (pDataSetXNoSort[j].strVal  == pGroupInforX[nGroupX].strVal)
						arrGroupIndex.Add(i); 
				}
			}
		}
		else
		{
			for (j=0; j<nRowCntY; j++)
			{
				if(pDataSetXNoSort[j].iIndex == abs(pDataSetYGroup[i].iIndex))
				{
					if (pDataSetXNoSort[j].dValue == pGroupInforX[nGroupX].dValue)
						arrGroupIndex.Add(i); 
				}
			}
		}
	}
	//����
	int nSize = arrGroupIndex.GetSize();
	if (nSize < 1) return true;
	TDataSet *pDataSet = new TDataSet[nSize];
	
	for (i=0; i<nSize; i++)
	{
		pDataSet[i]=pDataSetYGroup[arrGroupIndex[i]];
	}
	//��������
	statistics.SetData(pDataSet,nSize);
	if (!GetStatistical(statistics,eData,nSize,fValTempY))
	{
		delete []pDataSet;
		arrGroupIndex.RemoveAll();
		m_szErrMsg.Format("��״ͼ�е��ú�����ֵʱ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (!GetAxisLabel(eData,szYAxisLabel))
	{
		delete []pDataSet;
		arrGroupIndex.RemoveAll();
		m_szErrMsg.Format("��״ͼ�е��ú�����ֵʱ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	delete []pDataSet;
	return true;
}

bool CBar::CalcSimpleBar1()
{
	double fTotalCnt =0;
	int i=0, j=0, nWeight= 0;
	int nColIndexX=0, nDataTyX=0, nRowCntX=0;
	int nColIndexY=0, nDataTyY=0, nRowCntY=0;
	int nDataCntY=0, nRowValidCnt=0, nTotalCnt=0;
	int *pIndex = NULL;
	TDataSet *pDataSetX = NULL;
	TDataSet *pDataSetY = NULL;
	TDataSet *pDataSetGroup=NULL;
	TDataSet *pTempData = NULL;
	TDataSet *pTempDataY = NULL;
	TGroupInfor *pGroupInfor = NULL;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	CTString szYAxisLabel("");
	CStatistics statistics;
	EData eData;
	
	//��֤
	if (m_tVarX.iCount != 1)
	{
		m_szErrMsg.Format("��״ͼ����û�б�ָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//��������
	if (!GetDataType(0,YAXIS_INT,eData))
	{
		return false;
	}
	//������׼��
	nDataTyX = fString;
	nColIndexX = m_tVarX.pValue[0];
	szFldNameX = m_pDataInterface->GetFieldName(nColIndexX); 
	if(m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndexX))
	{
		nRowCntX=	m_pDataInterface->GetTLDBColDataLabel(nColIndexX,pDataSetX,1);
	}
	else
	{
		nRowCntX = m_pDataInterface->GetColData(nColIndexX,pDataSetX,1);
		nDataTyX = m_pDataInterface->GetFieldType(nColIndexX); 
	}
	//Y�ᣭ/VAR
	nRowValidCnt = 0;
	if(m_tVarY.iCount == 1)
	{
		pIndex = new int[nRowCntX];
		nColIndexY = m_tVarY.pValue[0];
		nDataTyY = m_pDataInterface->GetFieldType(nColIndexY); 
		szFldNameY = m_pDataInterface->GetFieldName(nColIndexY); 
		nRowCntY = m_pDataInterface->GetColData(nColIndexY,pDataSetY,1);
		
		for (i=0 ; i<nRowCntY; i++)
		{
			if (pDataSetY[i].iIndex > 0)
			{			
				pIndex[i] = 1;
				nRowValidCnt ++;
			}
			else
			{
				pIndex[i] = 0;
			}
		}
		//����	
		pTempData  = new TDataSet[nRowValidCnt];
		pTempDataY = new TDataSet[nRowValidCnt];
		int nID = 0;
		for (i= 0; i<nRowCntX; i++)
		{
			if(pIndex[i] == 1)
			{
				pTempData[nID].dValue = pDataSetX[i].dValue;
				pTempData[nID].strVal = pDataSetX[i].strVal;
				pTempData[nID].iIndex = pDataSetX[i].iIndex;
				printf("%d=%f\t",pDataSetX[i].iIndex-1,pDataSetX[i].dValue);
				pTempDataY[nID].dValue = pDataSetY[i].dValue;
				pTempDataY[nID].strVal = pDataSetY[i].strVal;
				pTempDataY[nID].iIndex = pDataSetY[i].iIndex;
				
				nID++;
			}
		}
		delete []pDataSetX;
		pDataSetX = NULL;
		delete []pDataSetY;
		pDataSetY = NULL;
	}
	else
	{
		pTempData = pDataSetX;
		pTempDataY = pDataSetY;
		nRowValidCnt = nRowCntX;
	}
	//���鴦��
	int nGroupCount = CStatistics::GetGroupingData(pTempData,nRowValidCnt,pGroupInfor);
	if (nGroupCount > MAX_VALID_INT)
	{	
		if (pTempData != NULL)  delete []pTempData;
		if (pTempDataY != NULL) delete []pTempDataY;
		for (i=0; i<nGroupCount; i++)
		{
			pGroupInfor[i].DeleteObject();
		}
		delete []pGroupInfor;
		delete []pIndex;	
		m_szErrMsg.Format("��״ͼ�ķ������ݴ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT,__LINE__,__FILE__);
		return false;
	}
	//ͼ������
	TDataPointStrVal *pDPSV = new TDataPointStrVal[nGroupCount];
	if (m_bWeight)
	{
		nRowCntX = 0;
		//����Ȩ��
		m_pDataInterface->SetWeightNumb(m_nWeight);
		for (i=0; i<nGroupCount; i++)
		{
			for (j=0; j<pGroupInfor[j].pIndex.iCount; j++)
			{
				int nID = pGroupInfor[i].pIndex.pValue[j];
				nRowCntX += m_pDataInterface->GetWeightData(nID);				
			}
		}
	}
	//����������
	for (i=0; i<nGroupCount; i++)
	{
		nWeight = 0;
		if (nDataTyX != fString)
		{
			if(pGroupInfor[i].pIndex.pValue[0] > 0)
				szTemp.Format("%5.2f",pGroupInfor[i].dValue); 		
			else
				szTemp = "ȱʧ";
			pDPSV[i].fXVal = szTemp;
		}
		else
		{
			if(pGroupInfor[i].pIndex.pValue[0] > 0)
				pDPSV[i].fXVal = pGroupInfor[i].strVal ;
			else
				pDPSV[i].fXVal = "ȱʧ";
		}
		//����
		int nY = pGroupInfor[i].pIndex.iCount;
		if (m_bWeight)
		{
			int nID = 0;
			for (j=0; j<nY; j++)
			{
				nID = pGroupInfor[i].pIndex.pValue[j];
				nWeight += m_pDataInterface->GetWeightData(nID);
			}					
			nY = nWeight;
		}
		//��������
		switch (eData)
		{
		case REC:
			{
				pDPSV[i].fYVal = nY; 
			}
			break;
		case RECSCALE:
			{
				if (nRowCntX)
					pDPSV[i].fYVal = 100.0*nY/double(nRowCntX);
				else
					pDPSV[i].fYVal = 0;
			}
			break;
		case RECTOTAL:
			{
				pDPSV[i].fYVal = nY + nTotalCnt;
				nTotalCnt += nY;
			}
			break;
		case RECTOTALSCALE:			
			{
				double fPercent = 0.0;
				if (nRowCntX) fPercent = 100.0*nY/double(nRowCntX);
				pDPSV[i].fYVal = fPercent + fTotalCnt;
				fTotalCnt += fPercent;
			}
			break;
		default:
			{
				nDataCntY = CStatistics::GetGroupData(pTempDataY,nRowValidCnt,pGroupInfor[i].pIndex ,pDataSetGroup);
				statistics.SetData(pDataSetGroup,nDataCntY);
				if (!GetStatistical(statistics,eData,nDataCntY,pDPSV[i].fYVal))
				{
					if (pDPSV != NULL)      delete []pDPSV;
					if (pTempData != NULL)  delete []pTempData;
					if (pTempDataY != NULL) delete []pTempDataY;
					if (pDataSetGroup != NULL) delete []pDataSetGroup;
					for (i=0; i<nGroupCount; i++)
					{
						pGroupInfor[i].DeleteObject();
					}
					delete []pGroupInfor;
					delete []pIndex;
					return false;
				}
				if (!GetAxisLabel(eData,szYAxisLabel))
				{
					if (pDPSV != NULL)      delete []pDPSV;
					if (pTempData != NULL)  delete []pTempData;
					if (pTempDataY != NULL) delete []pTempDataY;
					if (pDataSetGroup != NULL) delete []pDataSetGroup;
					for (i=0; i<nGroupCount; i++)
					{
						pGroupInfor[i].DeleteObject();
					}
					delete []pGroupInfor;
					delete []pIndex;
					return false;
				}
				if (pDataSetGroup != NULL) delete []pDataSetGroup;
				szYAxisLabel += " " + szFldNameY;  
			}
			break;
		}
	}
	//����
	switch (eData)
	{
	case REC:
	case RECSCALE:
	case RECTOTAL:
	case RECTOTALSCALE:
		GetAxisLabel(eData, szYAxisLabel);
		break;
	}
	//ͼ
	CTChartBar  *pBar = new CTChartBar;
	pBar->SetBar(pDPSV,nGroupCount,StrVal);
	pBar->SetXAxilLabel(szFldNameX);
	pBar->SetYAxilLabel(szYAxisLabel);
	CRsltElementChart *pChart = new CRsltElementChart("��״ͼ",pBar);
	m_pResult->SetName("��״ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	if (pDPSV != NULL)      delete []pDPSV;
	if (pTempData != NULL)  delete []pTempData;
	if (pTempDataY != NULL) delete []pTempDataY;
	for (i=0; i<nGroupCount; i++)
	{
		pGroupInfor[i].DeleteObject();
	}
	delete []pGroupInfor;
	delete []pIndex;
	return true;
}

bool CBar::CalcSimpleBar2()
{
	int i=0, j=0;
	int nColIndex=0, nRowCount=0, nRealCount=0;
	int *pIndex = NULL;
	TDataSet* pDataSet = NULL;
	TDataSet *pTempData = NULL;
	TDataPointStrVal* pDPSV = NULL;
	CStatistics statistics;
	CTString szTemp("");
	CTString szFldName("");
	CTString szYAxisLabel("");
	EData eData;
	CTArray<TDataSet*,TDataSet*> arrDataSet;

	int nVarCount = m_tVarX.iCount; 
	if (nVarCount < 2)
	{
		m_szErrMsg.Format("���ݲ���ȫ��������Ҫ�������������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//�Ƿ������ͬ�ĺ�������
	bool bExistSameFunc = IsSameCallFunc(XAXIS_INT);
	//�����ڴ�
	pDPSV       = new TDataPointStrVal[nVarCount];
	nRowCount   = m_pDataInterface->GetRowCount();
	pIndex      = new int[nRowCount];
	//���ݴ���
	for (i=0; i<nVarCount; i++)
	{
		nRealCount = 0;
		nColIndex = m_tVarX.pValue[i];
		nRowCount = m_pDataInterface->GetColData(nColIndex,pTempData,1); 
		for (j=0; j<nRowCount; j++)
		{
			if(pTempData[j].iIndex < 0)
			{
				pIndex[j] = 0;				
			}
			else
			{
				if(i == 0)
				{
					pIndex[j] = 1;
					nRealCount++;
				}
				else if(pIndex[j] != 0)
				{
					pIndex[j] = 1;
					nRealCount++;
				}
			}
		}
		arrDataSet.Add(pTempData);
	}
	
	if (nRealCount == 0)
	{
		m_szErrMsg.Format("����ͼ�е����ݲ��������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		int nSize = arrDataSet.GetSize();
		for (i=0; i<nSize; i++)
		{
			delete []arrDataSet.GetAt(i);
		}
		arrDataSet.RemoveAll();
		delete []pIndex;
		delete []pDPSV;
		return false;
	}
	for (i=0; i<nVarCount; i++)
	{
		nColIndex = m_tVarX.pValue[i];
		szFldName = m_pDataInterface->GetFieldName(nColIndex); 		

		TDataSet *pT = new TDataSet[nRealCount];
		pDataSet = arrDataSet.GetAt(i);
		int nID = 0;
		for (j=0; j<nRowCount; j++)
		{
			if(pIndex[j] == 1)
			{
				pT[nID].dValue = pDataSet[j].dValue;
				pT[nID].strVal = pDataSet[j].strVal;
				pT[nID].iIndex = pDataSet[j].iIndex;
				nID++;
			}
		}
		//�������������
		if (!GetDataType(i,XAXIS_INT,eData))
		{
			int nSize = arrDataSet.GetSize();
			for (i=0; i<nSize; i++)
			{
				delete []arrDataSet.GetAt(i);
			}
			arrDataSet.RemoveAll();
			delete []pT;
			delete []pIndex;
			delete []pDPSV;
			return false;
		}
		//��ֵ
		nRowCount = nRealCount;
		statistics.SetData(pT,nRowCount);
		if (!GetStatistical(statistics,eData,nRowCount,pDPSV[i].fYVal))
		{
			int nSize = arrDataSet.GetSize();
			for (i=0; i<nSize; i++)
			{
				delete []arrDataSet.GetAt(i);
			}
			arrDataSet.RemoveAll();
			delete []pT;
			delete []pIndex;
			delete []pDPSV;
			return false;
		}
		if (!GetAxisLabel(eData,szYAxisLabel))
		{
			int nSize = arrDataSet.GetSize();
			for (i=0; i<nSize; i++)
			{
				delete []arrDataSet.GetAt(i);
			}
			arrDataSet.RemoveAll();
			delete []pT;
			delete []pIndex;
			delete []pDPSV;
			return false;
		}
		if (!bExistSameFunc)
			szFldName = szYAxisLabel + " " + szFldName.GetData();
		pDPSV[i].fXVal= szFldName;  
		delete []pT;
	}
	//ͼ��
	CTChartBar  *pBar = new CTChartBar;
	pBar->SetBar(pDPSV,nVarCount,StrVal);
	pBar->SetXAxilLabel("����");
	pBar->SetYAxilLabel(bExistSameFunc ? szYAxisLabel: "�ܼ�");
	CRsltElementChart *pChart = new CRsltElementChart("��״ͼ",pBar);
	m_pResult->SetName("��״ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	int nSize = arrDataSet.GetSize();
	for (i=0; i<nSize; i++)
	{
		delete []arrDataSet.GetAt(i);
	}
	arrDataSet.RemoveAll();
	delete []pIndex;
	delete []pDPSV;
	return true;
}

bool CBar::CalcSimpleBar3()
{
	int i=0, nSize=0;
	int nRowCntX=0, nRowCntY=0;
	int nColIndX=0, nColIndY=0;
	int nDataTyX=0, nDataTyY=0;
	
	TDataSet *pDataSetX = NULL;
	TDataSet *pDataSetY = NULL;
	CTString szTemp = "";
	CTString szFldNameY = "";
	CTString szFldNameX = "";
	TDataPointStrVal *pDPSV = NULL;
	
	if (m_tVarY.iCount != 1) 
	{
		m_szErrMsg.Format("ֱ��ͼ��û��ָ��Y��������ݣ����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	nColIndY = m_tVarY.pValue[0];
	nDataTyY = m_pDataInterface->GetFieldType(nColIndY); 
	nRowCntY = m_pDataInterface->GetColData(nColIndY,pDataSetY,1);
	szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 
	if (m_tVarX.iCount == 1) //��Ϊ������ʽ
	{
		nColIndX = m_tVarX.pValue[0]; 
		nDataTyX = fString;
		szFldNameX = m_pDataInterface->GetFieldName(nColIndX); 
		if(m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndX))
		{
			nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndX,pDataSetX,1);
		}
		else
		{
			nRowCntX = m_pDataInterface->GetColData(nColIndX,pDataSetX,1);
			nDataTyX = m_pDataInterface->GetFieldType(nColIndX);
		}
		nSize	= (nRowCntY < nRowCntX) ? nRowCntY : nRowCntX;
		pDPSV = new TDataPointStrVal[nRowCntX];		
	}
	else
	{
		if (nDataTyY == fString) //����ֵ����Ϊ�ַ���
		{
			if (pDataSetY != NULL) delete pDataSetY;
			m_szErrMsg.Format("��״ͼ�еı���ֵ����Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		nSize = nRowCntY;
		pDPSV = new TDataPointStrVal[nRowCntY];
	}
	//����
	if (nRowCntY > MAX_VALID_INT || nRowCntX > MAX_VALID_INT)
	{	
		//�ͷ��ڴ�
		if (pDataSetX != NULL) delete pDataSetX;
		if (pDataSetY != NULL) delete pDataSetY;
		m_szErrMsg.Format("��״ͼ�ķ��������� %d �飬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//����
	if (m_tVarX.iCount == 0)
	{
		for (i=0; i<nRowCntY; i++)
		{
			if (pDataSetY[i].iIndex > 0)
				pDPSV[i].fYVal = pDataSetY[i].dValue;			
			else
				pDPSV[i].fYVal = 0;
			szTemp.Format("%d",(i+1));
			szTemp.TrimLeft();
			pDPSV[i].fXVal = szTemp;
		}	
	}
	else if(m_tVarX.iCount ==1)
	{
		for (i=0; i<nSize; i++)
		{
			//Y�᷽��
			if (nDataTyY != fString)
			{
				if(pDataSetY[i].iIndex > 0) 
					pDPSV[i].fYVal = pDataSetY[i].dValue;			
				else
					pDPSV[i].fYVal = 0;
			}
			else
			{
				pDPSV[i].fYVal =0;
			}
			//X�᷽��
			if (nDataTyX == fString)
			{
				if(pDataSetX[i].iIndex > 0)
					pDPSV[i].fXVal = pDataSetX[i].strVal;
				else
					pDPSV[i].fXVal = "ȱʧ";
			}
			else
			{
				if(pDataSetX[i].iIndex > 0)
					pDPSV[i].fXVal.Format("%5.1f", pDataSetX[i].dValue);
				else
					pDPSV[i].fXVal = "ȱʧ";
			}
		}
	}		
	//ͼ��
	CTChartBar *pBar = new CTChartBar;
	pBar->SetBar(pDPSV,nSize,StrVal);
	pBar->SetXAxilLabel(szFldNameX);
	pBar->SetYAxilLabel(szFldNameY);
	CRsltElementChart *pChart = new CRsltElementChart("��״ͼ",pBar);
	m_pResult->SetName("��״ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	if (pDPSV != NULL) delete []pDPSV;
	if (pDataSetX != NULL) delete []pDataSetX;
	if (pDataSetY != NULL) delete []pDataSetY;
	return true;
}

bool CBar::CalcBarClustered1(bool bStacked)
{
	int i=0, j=0, k=0;
	int nColIndX=0, nColIndD=0, nColIndY=0;
	int nRowCntX=0, nRowCntD=0, nRowCntY=0;
	int nDataTyX=0, nDataTyD=0, nDataTyY=0;
	int nID=0, nTempCnt=0, nDataCntY=0;
	int nGroupCntX=0, nGroupCntD=0;
	int nCountSub=0, nTotalCount=0, nTempCountSub=0;
	double fValTempY = 0, fTotalCount=0;
	TDataSet *pDataSetX=NULL; 
	TDataSet *pDataSetXNoSort=NULL; 
	TDataSet *pDataSetD=NULL;
	TDataSet *pDataSetY=NULL;
	TDataSet* pDataSetYGroup=NULL;
	TDataSet* pDataSetSub=NULL;
	TDataSet *pTempDataSetX=NULL;
	TDataSet *pTempDataSetXNoSort=NULL;
	TDataSet *pTempDataSetD=NULL;
	TDataSet *pTempDataSetY=NULL;
	TGroupInfor *pGroupInforX=NULL;
	TGroupInfor *pGroupInforD=NULL;
	CTString szTemp("");
	CTString szLegend("");
	CTString szFldNameX("");
	CTString szFldNameD("");
	CTString szFldNameY("");
	CTString szYAxisLabel("");
	CStatistics statistics;

	
	//��־λ
	m_bStacked = bStacked;
	if (m_tVarX.iCount != 1)
	{
		m_szErrMsg.Format("������״ͼ�е�������������ڣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (m_tVarD.iCount != 1)
	{
		m_szErrMsg.Format("������״ͼ�е��Զ����б��������ڣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//Y������
	EData eData = m_arrY.GetAt(0);
	//����Ȩ��
	if (m_bWeight) m_pDataInterface->SetWeightNumb(m_nWeight);
	//X��
	nDataTyX = fString;
	nColIndX = m_tVarX.pValue[0];
	szFldNameX = m_pDataInterface->GetFieldName(nColIndX); 
	if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndX))
	{
		m_pDataInterface->GetTLDBColDataLabel(nColIndX,pDataSetXNoSort,1);
		nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndX,pDataSetX,1);
	}
	else
	{
		m_pDataInterface->GetColData(nColIndX,pDataSetXNoSort,1);
		nRowCntX = m_pDataInterface->GetColData(nColIndX,pDataSetX,1);
		nDataTyX = m_pDataInterface->GetFieldType(nColIndX); 
	}
	//�Զ�����
	nDataTyD = fString;
	nColIndD = m_tVarD.pValue[0];
	szFldNameD = m_pDataInterface->GetFieldName(nColIndD);
	if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndD))
	{
		nRowCntD = m_pDataInterface->GetTLDBColDataLabel(nColIndD,pDataSetD,1);
	}
	else
	{
		nRowCntD = m_pDataInterface->GetColData(nColIndD,pDataSetD,1);
		nDataTyD = m_pDataInterface->GetFieldType(nColIndD); 
	}
		
	int *pIndex = new int[nRowCntX];
	if (m_tVarY.iCount == 1) //���ں���
	{
		nColIndY = m_tVarY.pValue[0];
		nRowCntY = m_pDataInterface->GetColData(nColIndY,pDataSetY,1);
		szFldNameY = m_pDataInterface->GetFieldName(nColIndY);
		for (i=0; i<nRowCntY; i++)
		{
			if (pDataSetY[i].iIndex > 0)
			{
				pIndex[i] = 1;
				nTempCnt++;
			}
			else
			{
				pIndex[i] = 0;
			}
		}
		//���ٴ����ռ�
		pTempDataSetX = new TDataSet[nTempCnt];
		pTempDataSetD = new TDataSet[nTempCnt];
		pTempDataSetY = new TDataSet[nTempCnt];
		pTempDataSetXNoSort = new TDataSet[nTempCnt];
		//ȱʧֵ����
		for (i=0; i<nRowCntY; i++)
		{
			if(pIndex[i] == 1)
			{
				pTempDataSetX[nID].iIndex = pDataSetX[i].iIndex;
				pTempDataSetX[nID].dValue = pDataSetX[i].dValue;
				pTempDataSetX[nID].strVal = pDataSetX[i].strVal;
				
				pTempDataSetXNoSort[nID].iIndex = pDataSetXNoSort[i].iIndex;
				pTempDataSetXNoSort[nID].dValue = pDataSetXNoSort[i].dValue;
				pTempDataSetXNoSort[nID].strVal = pDataSetXNoSort[i].strVal;
				
				pTempDataSetD[nID].iIndex = pDataSetD[i].iIndex;
				pTempDataSetD[nID].dValue = pDataSetD[i].dValue;
				pTempDataSetD[nID].strVal = pDataSetD[i].strVal;
				
				pTempDataSetY[nID].iIndex = pDataSetY[i].iIndex;
				pTempDataSetY[nID].dValue = pDataSetY[i].dValue;
				pTempDataSetY[nID].strVal = pDataSetY[i].strVal;
				
				nID++;
			}
		}
		//���¸�ֵ
		nRowCntY = nTempCnt;
		delete []pDataSetX;
		delete []pDataSetXNoSort;
		delete []pDataSetD;
		delete []pDataSetY;
	}
	else
	{
		pTempDataSetX = pDataSetX;
		pTempDataSetXNoSort = pDataSetXNoSort;
		pTempDataSetD = pDataSetD;
		pTempDataSetY = pDataSetY;
		nTempCnt = nRowCntX;
	}
	//X-Group
	nGroupCntX = CStatistics::GetGroupingData(pTempDataSetX,nTempCnt,pGroupInforX);
	if (nGroupCntX > MAX_MULTI_INT)
	{		
		delete []pTempDataSetX;
		delete []pTempDataSetXNoSort;
		delete []pTempDataSetD;
		delete []pTempDataSetY;	
		for (i=0; i<nGroupCntX; i++)
		{
			pGroupInforX[i].DeleteObject();
		}
		delete []pGroupInforX;
		delete []pIndex;
		m_szErrMsg.Format("��״ͼ�е������������ݴ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_MULTI_INT,__LINE__,__FILE__);
		return false;
	}
	//D-Group
	nGroupCntD = CStatistics::GetGroupingData(pTempDataSetD,nTempCnt,pGroupInforD);
	//��״ͼ
	CTChartBar *pBar = new CTChartBar;
	CRsltElementChart *pChart = new CRsltElementChart("��״ͼ",pBar);
	m_pResult->SetName("��״ͼ"); 
	m_pResult->Add(pChart);

	double *pValTotal = new double[nGroupCntX];
	for (i=0; i<nGroupCntX; i++)
		pValTotal[i]=0;
	//���ݴ���
	for (i=0; i<nGroupCntD; i++)
	{
		nTempCountSub = 0;
		if (nDataTyD == fString)
		{
			if (pGroupInforD[i].pIndex.pValue[0] > 0)
				szLegend = pGroupInforD[i].strVal;
			else
				szLegend = "ȱʧ";
		}
		else
		{
			if (pGroupInforD[i].pIndex.pValue[0] > 0)
				szLegend.Format("%5.1f", pGroupInforD[i].dValue);
			else
				szLegend = "ȱʧ";
		}
		szLegend.TrimLeft(); 
		
		TDataPointStrVal *pDPSV = new TDataPointStrVal[nGroupCntX];
		int nDataCountSub = CStatistics::GetGroupData(pTempDataSetX,nTempCnt,pGroupInforD[i].pIndex ,pDataSetSub);
		if (m_bWeight)
		{
			for (j=0; j<nDataCountSub; j++)
			{
				nID = pDataSetSub[j].iIndex;
				nTempCountSub += m_pDataInterface->GetWeightData(nID);
			}
		}
		nTotalCount=0;
		fTotalCount=0.0;	
		for (j=0; j<nGroupCntX; j++)
		{
			nCountSub=0;
			fValTempY=0;		
			if (nDataTyX != fString)
			{
				if (pGroupInforX[j].pIndex.pValue[0] > 0)
					szTemp.Format("%5.1f",pGroupInforX[j].dValue); 
				else
					szTemp = "ȱʧ";
				
				szTemp.TrimLeft();
				pDPSV[j].fXVal = szTemp;
			}
			else
			{
				if (pGroupInforX[j].pIndex.pValue[0] > 0)
					pDPSV[j].fXVal = pGroupInforX[j].strVal ;				
				else
					pDPSV[j].fXVal = "ȱʧ";
			}
			
			for (k=0; k<nDataCountSub; k++)
			{
				if (nDataTyX == fString)
				{
					if (pGroupInforX[j].strVal == pDataSetSub[k].strVal )
					{
						if (m_tVarY.iCount == 1) //���ں���
						{
							if (!GetValForFunc(i,j,eData,nDataCntY,pTempDataSetY,nRowCntY,pGroupInforD,
								pDataSetYGroup,pTempDataSetXNoSort,pGroupInforX,statistics,fValTempY,szYAxisLabel,true))
							{
								//�ͷ��ڴ�
								delete []pTempDataSetX;
								delete []pTempDataSetXNoSort;
								delete []pTempDataSetD;
								delete []pTempDataSetY;		
								for (i=0; i<nGroupCntX; i++)
								{
									pGroupInforX[i].DeleteObject();
								}
								delete []pGroupInforX;
								delete []pIndex;	
								delete []pValTotal;
								delete []pDataSetYGroup;
								for (i=0; i<nGroupCntD; i++)
								{
									pGroupInforD[i].DeleteObject();
								}
								delete []pGroupInforD;
								return	false;
							}
						//	szYAxisLabel += " " + szFldNameY;  
						}
						else
						{
							if (m_bWeight)
							{
								nID = pDataSetSub[k].iIndex;
								nCountSub += m_pDataInterface->GetWeightData(nID);
							}
							else
							{
								nCountSub ++;
							}
						}
					}
				}
				else
				{
					if (pGroupInforX[j].dValue == pDataSetSub[k].dValue)
					{
						if (m_tVarY.iCount == 1) //���ں���
						{							
							if (!GetValForFunc(i,j,eData,nDataCntY,pTempDataSetY,nRowCntY,pGroupInforD,
								pDataSetYGroup,pTempDataSetXNoSort,pGroupInforX,statistics,fValTempY,szYAxisLabel,false))
							{
								//�ͷ��ڴ�
								delete []pTempDataSetX;
								delete []pTempDataSetXNoSort;
								delete []pTempDataSetD;
								delete []pTempDataSetY;		
								for (i=0; i<nGroupCntX; i++)
								{
									pGroupInforX[i].DeleteObject();
								}
								delete []pGroupInforX;
								delete []pIndex;	
								delete []pValTotal;
								delete []pDataSetYGroup;
								for (i=0; i<nGroupCntD; i++)
								{
									pGroupInforD[i].DeleteObject();
								}
								delete []pGroupInforD;
								return	false;
							}
						}
						else
						{
							if (m_bWeight)
							{
								nID = pDataSetSub[k].iIndex;;
								nCountSub += m_pDataInterface->GetWeightData(nID);												
							}
							else
							{
								nCountSub ++;
							}
						}
					}
				}
			}
			//��������
			switch (eData)
			{
			case REC:
				if (m_bStacked)
				{
					pDPSV[j].fYVal = pValTotal[j]+nCountSub;
					pValTotal[j]   = pDPSV[j].fYVal;
				}
				else
				{
					pDPSV[j].fYVal =nCountSub;
				}
				break;
			case RECSCALE:
				if (m_bStacked)
				{
					if (m_bWeight)
					{
						if (nTempCountSub > 0)
							pDPSV[j].fYVal = pValTotal[j]+nCountSub /double(nTempCountSub)*100;
						else
							pDPSV[j].fYVal = 0;
					}
					else
					{
						pDPSV[j].fYVal = pValTotal[j]+nCountSub /double(nDataCountSub)*100;
					}
					
					pValTotal[j]=pDPSV[j].fYVal ;
				}
				else
				{
					if (m_bWeight)
					{
						if (nTempCountSub > 0)
							pDPSV[j].fYVal =nCountSub /double(nTempCountSub)*100;
						else
							pDPSV[j].fYVal = 0;
					}
					else
					{
						pDPSV[j].fYVal =nCountSub /double(nDataCountSub)*100;
					}
				}
				break;
			case RECTOTAL:
				if (m_bStacked)
				{
					pDPSV[j].fYVal = nCountSub+nTotalCount+pValTotal[j];
					pValTotal[j]   = pDPSV[j].fYVal ;
				}
				else
				{
					pDPSV[j].fYVal = nCountSub+nTotalCount;
				}
				nTotalCount += nCountSub;
				break;
			case RECTOTALSCALE:			
				{
					double fPercent = 0;
					if (m_bStacked)
					{
						if (m_bWeight)
						{
							if (nTempCountSub)
								fPercent=nCountSub /double(nTempCountSub)*100;
							else
								fPercent = 0;		
						}
						pDPSV[j].fYVal = fPercent+fTotalCount+pValTotal[j];
						pValTotal[j]   = pDPSV[j].fYVal;
					}
					else
					{
						if(m_bWeight)
						{
							if(nTempCountSub)
								fPercent=nCountSub /double(nTempCountSub)*100;
							else
								fPercent = 0;
						}
						else
						{
							fPercent = nCountSub /double(nDataCountSub)*100;
						}
						
						pDPSV[j].fYVal = fPercent+fTotalCount;									
					}
					fTotalCount += fPercent;
				}
				break;
			default: //��Ҫ���ú���
				{
					if (m_bStacked)
					{
						pDPSV[j].fYVal = pValTotal[j]+fValTempY;
						pValTotal[j]   = pDPSV[j].fYVal;
					}
					else
					{
						pDPSV[j].fYVal = fValTempY; 
					}				
				}
				break;
			}
		}
		//�����״ͼ
		pBar->SetBar(pDPSV,nGroupCntX,StrVal,szLegend);
		delete []pDPSV;
		delete []pDataSetSub;
	}
	//������
	pBar->SetXAxilLabel(szFldNameX);
	pBar->SetYAxilLabel(szYAxisLabel);
	//�ͷ��ڴ�
	delete []pValTotal;
	delete []pTempDataSetX;
	delete []pTempDataSetXNoSort;
	delete []pTempDataSetD;
	delete []pTempDataSetY;		
	for (i=0; i<nGroupCntX; i++)
	{
		pGroupInforX[i].DeleteObject();
	}
	delete []pGroupInforX;
	delete []pIndex;	
	delete []pDataSetYGroup;
	for (i=0; i<nGroupCntD; i++)
	{
		pGroupInforD[i].DeleteObject();
	}
	delete []pGroupInforD;
	return true;
}

bool CBar::CalcBarClustered2(bool bStacked)
{
	int i=0, j=0, k=0;
	int nColIndX=0, nRowCntX=0, nDataTyX=0;
	int nColIndY=0, nRowCntY=0, nDataTyY=0;
	int nID=0, nDataCntX=0, nTempCount=0;
	int *pIndex=NULL;
	double *pValTotal=NULL;
	TDataSet *pDataSetX=NULL;
	TDataSet *pDataSetY=NULL;
	TDataSet *pDataSetSub=NULL;
	TDataSet *pTempDataSetX=NULL;
	TDataSet *pTempDataSetY=NULL;
	TGroupInfor *pGroupInforX=NULL;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	CTString szYAxisLabel("");
	CTArray<TDataSet*,TDataSet*>arrDataSetY;
	EData eData;
	CStatistics statistics;
	
	//��־λ
	m_bStacked = bStacked;
	if (m_tVarY.iCount < 2)
	{
		m_szErrMsg.Format("��״ͼ�еı��������������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (m_tVarX.iCount != 1)
	{
		m_szErrMsg.Format("��״ͼ�е�X���������ڣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//�Ƿ������ͬ�ĺ�������
	bool bExistSameFunc = IsSameCallFunc(YAXIS_INT);
	//X
	nColIndX = m_tVarX.pValue[0];  
	nDataTyX = fString;
	szFldNameX = m_pDataInterface->GetFieldName(nColIndX);
	if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndX))
	{
		nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndX,pDataSetX,1);
	}
	else
	{
		nRowCntX = m_pDataInterface->GetColData(nColIndX,pDataSetX,1);
		nDataTyX = m_pDataInterface->GetFieldType(nColIndX); 
	}
	//Y
	for (i=0; i<m_tVarY.iCount; i++)
	{
		nTempCount = 0;
		nColIndY = m_tVarY.pValue[i];
		nDataTyY = m_pDataInterface->GetFieldType(nColIndY);		
		nRowCntY = m_pDataInterface->GetColData(nColIndY,pTempDataSetY,1); 
		if (i == 0)
			pIndex = new int[nRowCntY];
		for (j=0; j<nRowCntY; j++)
		{
			if(pTempDataSetY[j].iIndex < 0)
			{
				pIndex[j] = 0;				
			}
			else
			{
				if (i == 0)
				{
					pIndex[j] = 1;
					nTempCount++;
				}
				else if(pIndex[j] != 0)
				{
					pIndex[j] = 1;
					nTempCount++;
				}
			}
		}
		arrDataSetY.Add(pTempDataSetY);
	}
	//��Ч����
	if (nTempCount == 0)
	{
		m_szErrMsg.Format("��״ͼ������Ч���ݣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		for (i=0; i<arrDataSetY.GetSize(); i++)
		{
			TDataSet *p = arrDataSetY.GetAt(i);
			delete []p;
		}
		arrDataSetY.RemoveAll();
		delete []pIndex;
		delete []pDataSetX;
		return false;
	}
	//��Ч����
	pTempDataSetX = new TDataSet[nTempCount];
	for (i=0; i<nRowCntY; i++)
	{
		if (pIndex[i] == 1)
		{
			pTempDataSetX[nID].dValue = pDataSetX[i].dValue;
			pTempDataSetX[nID].iIndex = pDataSetX[i].iIndex;
			pTempDataSetX[nID].strVal = pDataSetX[i].strVal;
			nID++;
		}
	}
	delete []pDataSetX;
	pDataSetX = pTempDataSetX;
	nDataCntX = CStatistics::GetGroupingData(pDataSetX,nTempCount,pGroupInforX); 
	if (nDataCntX > MAX_VALID_INT)
	{		
		m_szErrMsg.Format("��״ͼ�ڷ�������ݴ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		for (i=0; i<nDataCntX; i++)
		{
			pGroupInforX[k].DeleteObject();
		}
		delete []pGroupInforX;
		delete []pIndex;
		delete []pDataSetX;
		for (i=0; i<arrDataSetY.GetSize(); i++)
		{
			TDataSet *p = arrDataSetY.GetAt(i);
			delete []p;
		}
		arrDataSetY.RemoveAll();
		return false;
	}
	//ͼ��
	CTChartBar *pBar = new CTChartBar;
	CRsltElementChart *pChart = new CRsltElementChart("��״ͼ",pBar);
	m_pResult->SetName("��״ͼ"); 
	m_pResult->Add(pChart);

	pValTotal = new double[nDataCntX];
	for (i=0; i<nDataCntX; i++)
		pValTotal[i]=0;
	//ȱʧ����
	for (i=0; i<arrDataSetY.GetSize(); i++)
	{
		TDataPointStrVal *pDPSV = new TDataPointStrVal[nDataCntX];
		nColIndY = m_tVarY.pValue[i];
		nDataTyY = m_pDataInterface->GetFieldType(nColIndY);
		szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 
		pDataSetY = arrDataSetY.GetAt(i);
		//��������
		if (!GetDataType(i,YAXIS_INT,eData))
		{
			for (i=0; i<nDataCntX; i++)
			{
				pGroupInforX[i].DeleteObject();
			}
			delete []pGroupInforX;
			delete []pDPSV;
			delete []pIndex;
			delete []pValTotal;
			delete []pDataSetX;
			for (i=0; i<arrDataSetY.GetSize(); i++)
			{
				TDataSet *p = arrDataSetY.GetAt(i);
				delete []p;
			}
			arrDataSetY.RemoveAll();
			return false;
		}
		for (j=0; j<nDataCntX; j++)
		{
			int nDataCount = CStatistics::GetGroupData(pDataSetY,nRowCntY,pGroupInforX[j].pIndex,pDataSetSub);
			statistics.SetData(pDataSetSub,nDataCount);
			if (!GetStatistical(statistics,eData,nDataCount,pDPSV[j].fYVal))
			{
				for (i=0; i<nDataCntX; i++)
				{
					pGroupInforX[i].DeleteObject();
				}
				delete []pGroupInforX;
				delete []pDPSV;
				delete []pIndex;
				delete []pValTotal;
				delete []pDataSetX;
				delete []pDataSetSub;
				for (i=0; i<arrDataSetY.GetSize(); i++)
				{
					TDataSet *p = arrDataSetY.GetAt(i);
					delete []p;
				}
				arrDataSetY.RemoveAll();
				return false;
			}
			if (!GetAxisLabel(eData,szYAxisLabel))
			{
				for (i=0; i<nDataCntX; i++)
				{
					pGroupInforX[i].DeleteObject();
				}
				delete []pGroupInforX;
				delete []pDPSV;
				delete []pIndex;
				delete []pValTotal;
				delete []pDataSetX;
				delete []pDataSetSub;
				for (i=0; i<arrDataSetY.GetSize(); i++)
				{
					TDataSet *p = arrDataSetY.GetAt(i);
					delete []p;
				}
				arrDataSetY.RemoveAll();
				return false;
			}
			if(m_bStacked)
			{
				pDPSV[j].fYVal += pValTotal[j]; 
				pValTotal[j] = pDPSV[j].fYVal; 
			}
			if (nDataTyX == fString)
			{
				if (pGroupInforX[j].pIndex.pValue[0] > 0)
					pDPSV[j].fXVal = pGroupInforX[j].strVal; 
				else
					pDPSV[j].fXVal = "ȱʧ";
			}
			else
			{
				if (pGroupInforX[j].pIndex.pValue[0] > 0)
					pDPSV[j].fXVal.Format("%5.1f",pGroupInforX[j].dValue );  
				else
					pDPSV[j].fXVal = "ȱʧ";
			}
			pDPSV[j].fXVal.TrimLeft(); 
			delete []pDataSetSub;
		}
		if (!bExistSameFunc)
			szFldNameY = szYAxisLabel + " " + szFldNameY.GetData();
		pBar->SetBar(pDPSV,nDataCntX,StrVal,szFldNameY);
		//�ͷ��ڴ�
		delete []pDPSV;
	}
	//ͼ��
	pBar->SetXAxilLabel(szFldNameX);
	//�ͷ��ڴ�
	for (i=0; i<nDataCntX; i++)
	{
		pGroupInforX[i].DeleteObject();
	}
	delete []pGroupInforX;
	delete []pIndex;
	delete []pValTotal;
	delete []pDataSetX;
	for (i=0; i<arrDataSetY.GetSize(); i++)
	{
		TDataSet *p = arrDataSetY.GetAt(i);
		delete []p;
	}
	arrDataSetY.RemoveAll();
	return true;
}

bool CBar::CalcBarClustered3(bool bStacked)
{
	int i=0, j=0;
	int nColIndX=0, nRowCntX=0, nDataTyX=0;
	int nColIndY=0, nRowCntY=0, nDataTyY=0;
	TDataSet *pDataSetX=NULL;
	TDataSet *pDataSetY=NULL;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	
	//��־λ
	m_bStacked = bStacked;
	if (m_tVarY.iCount < 2)
	{
		m_szErrMsg.Format("��״ͼ�еı��������������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (m_tVarX.iCount > 1)
	{
		m_szErrMsg.Format("��״ͼ�е���������ܶ���һ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//���ݴ���
	if (m_tVarX.iCount == 0)
	{
		nColIndX = m_tVarY.pValue[0];//��ʱ
		nRowCntX = m_pDataInterface->GetRowCount();
	}
	else
	{
		nColIndX = m_tVarX.pValue[0];
		nDataTyX = fString;
		szFldNameX = m_pDataInterface->GetFieldName(nColIndX); 
		if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndX))
		{
			nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndX,pDataSetX,1);
		}
		else
		{
			nRowCntX = m_pDataInterface->GetColData(nColIndX,pDataSetX,1);
			nDataTyX = m_pDataInterface->GetFieldType(nColIndX); 
		}
	}
	//ͼ��
	CTChartBar *pBar = new CTChartBar;
	CRsltElementChart *pChart = new CRsltElementChart("��״ͼ",pBar);
	m_pResult->SetName("��״ͼ"); 
	m_pResult->Add(pChart);
	//��������
	double *pValTotal = new  double[nRowCntX];
	for (i=0; i<nRowCntX; i++)
		pValTotal[i]=0;
	for (i=0; i<m_tVarY.iCount; i++)
	{
		nColIndY = m_tVarY.pValue[i];
		nDataTyY = m_pDataInterface->GetFieldType(nColIndY);
		nRowCntY = m_pDataInterface->GetColData(nColIndY,pDataSetY,1); 
		szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 

		TDataPointStrVal *pDPSV = new TDataPointStrVal[nRowCntY];
		for (j=0; j<nRowCntY; j++)
		{
			if (m_tVarX.iCount == 0)
			{
				pDPSV[j].fXVal.Format("%d",j+1);  
			}
			else
			{	
				if (nDataTyX == fString)				
				{
					if (pDataSetX[j].iIndex > 0)
						pDPSV[j].fXVal = pDataSetX[j].strVal ;				
					else
						pDPSV[j].fXVal = "ȱʧ";
				}
				else
				{
					if (pDataSetX[j].iIndex > 0)
						pDPSV[j].fXVal.Format("%5.1f",pDataSetX[j].dValue ) ;
					else
						pDPSV[j].fXVal = "ȱʧ";
				}
			}
			if (m_bStacked)
			{
				if(pDataSetY[j].iIndex > 0)
					pDPSV[j].fYVal = pDataSetY[j].dValue +pValTotal[j];
				else
					pDPSV[j].fYVal = pValTotal[j];
				pValTotal[j] = pDPSV[j].fYVal ;
			}
			else
			{
				if (pDataSetY[j].iIndex > 0)
					pDPSV[j].fYVal = pDataSetY[j].dValue; 
				else
					pDPSV[j].fYVal = 0;	
			}
		}
		//���ͼ��
		pBar->SetBar(pDPSV,nRowCntY,StrVal,szFldNameY);
		delete []pDPSV;
		delete []pDataSetY;
	}
	//������
	pBar->SetXAxilLabel(szFldNameX);
	//�ͷ��ڴ�
	delete []pValTotal;
	delete []pDataSetX;
	return true;
}

bool CBar::CalcGroupBar1()
{	
	return CalcBarClustered1(false);
}

bool CBar::CalcGroupBar2()
{
	return CalcBarClustered2(false);
}

bool CBar::CalcGroupBar3()
{
	return CalcBarClustered3(false);
}

bool CBar::CalcSegmentBar1()
{
	return CalcBarClustered1(true);
}

bool CBar::CalcSegmentBar2()
{	
	return CalcBarClustered2(true);
}

bool CBar::CalcSegmentBar3()
{
	return CalcBarClustered3(true);
}