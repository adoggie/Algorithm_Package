// Area.cpp: implementation of the CArea class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Area.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArea::CArea()
{
	m_szErrMsg = "";
}

CArea::~CArea()
{

}

bool CArea::Verify()
{
	int i = 0;
	int nColIndex = 0, nDataType = 0;
	CTString szFldName("");

	if (m_eMode == CLASS)
	{
		if (m_tVarX.iCount != 1)
		{
			m_szErrMsg.Format("���ͼ�е������δָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_nAVType & AXIS_DESC_INT)
		{
			m_szErrMsg.Format("���ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if ((m_nAVType & VAR_DESC_INT  && m_tVarY.iCount == 1) ||
			(m_nAVType & VAR_NDESC_INT && m_tVarY.iCount != 1))
		{
			m_szErrMsg.Format("���ͼ�е�ֵ������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_tVarY.iCount == 1)
		{
			nColIndex = m_tVarY.pValue[0];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			if (nDataType == fString)
			{ 
				m_szErrMsg.Format("���ͼ�е�ֵ�����������Ͳ���Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
		}
	}
	else if (m_eMode == VARI)
	{
		if (m_nAVType & AXIS_DESC_INT)
		{
			m_szErrMsg.Format("���ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_tVarX.iCount < 2)
		{
			m_szErrMsg.Format("���ͼ�е���������������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_arrX.GetSize() < 1)
		{
			m_szErrMsg.Format("���ͼ�е��������������û�ж��壬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<m_tVarX.iCount; i++)
		{
			nColIndex = m_tVarX.pValue[i];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			if (nDataType == fString)
			{ 
				szFldName = m_pDataInterface->GetFieldName(nColIndex);
				m_szErrMsg.Format("���ͼ�е�������������Ͳ���Ϊ�ַ���-%s���޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
				return false;
			}
		}
	}
	else if (m_eMode == VALUE)
	{
		if (m_nAVType & VAR_DESC_INT)
		{
			m_szErrMsg.Format("���ͼ�е�ֵ�����������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_tVarY.iCount != 1)
		{
			m_szErrMsg.Format("���ͼ�е�ֵ����������Ҫָ��һ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if ((m_nAVType & AXIS_DESC_INT  && m_tVarX.iCount == 1) ||
			(m_nAVType & AXIS_NDESC_INT && m_tVarX.iCount != 1))
		{
			m_szErrMsg.Format("���ͼ�е��������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		nColIndex = m_tVarY.pValue[0];
		nDataType = m_pDataInterface->GetFieldType(nColIndex);
		if (nDataType == fString)
		{ 
			m_szErrMsg.Format("���ͼ�е�ֵ�����������Ͳ���Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

CResult *CArea::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//ͼ��
	SetChartType(CHART_AREA);
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
	if (m_eMode == CLASS) bRet = CalcArea1();
	else if (m_eMode == VARI) bRet = CalcArea2();
	else if (m_eMode == VALUE) bRet = CalcArea3();
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

bool CArea::GetValueFromVo(string VO)
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

bool CArea::ParseTypeMode(string VO)
{
	int nSize=0;
	CTString szTemp;
	CTStringArray szArrName;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	//�����������ͣ�ֻ��һ�����ͼ
	szArrName.RemoveAll();
	if (AnaSyntax::exist(MODE_STR,VOstr)) 
	{
		AnaSyntax::getVAR(MODE_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("���ͼ�е��������Ͳ�ȷ�������飡Line=%d,File=%s",__LINE__,__FILE__);
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
			m_szErrMsg.Format("���ͼ�е��������Ͳ����ڣ�%s�����飡Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("���ͼ����������û�б�ָ�������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	return true;
}

bool CArea::CalcArea1()
{
	int i=0, j=0, k=0;
	int nID=0, nTmp=0, nTempCnt=0;
	int nColIndX=0, nColIndY=0;
	int nRowCntX=0, nRowCntY=0;
	int nDataTyX=0, nDataTyY=0, nDataCntY=0;
	int nWeight=0, nTotalCount=0, nGroupCntX=0;
	double fTemp=0, fTotalCount=0, fTotalPercentEx=0;
	TDataSet *pDataSetX= NULL;
	TDataSet *pDataSetY= NULL;
	TDataSet *pDataSetYGroup=NULL;
	TGroupInfor *pGroupInfor=NULL;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	CTString szYAxisLabel("");
	EData eData;
	CStatistics statistics;
	
	//����Ȩ��
	if (m_bWeight) m_pDataInterface->SetWeightNumb(m_nWeight);
	//��������
	nDataTyX = fString;
	nColIndX = m_tVarX.pValue[0];
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
	if (nRowCntX < 1)
	{
		m_szErrMsg.Format("���ͼ������Ч�����ݣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//Y����������
	if (!GetDataType(0,YAXIS_INT,eData)) 
	{
		return false;
	}
	//Y��Label.
	switch (eData)
	{
	case REC:
		szYAxisLabel = "��¼��";
		break;
	case RECSCALE:
		szYAxisLabel = "�ٷֱ�";
		break;
	case RECTOTAL:
		szYAxisLabel = "�ۼƼ�¼��";
		break;
	case RECTOTALSCALE:
		szYAxisLabel = "�ۼưٷֱ�";
		break;
	default:
		break;
	}
	//ɾ��Y��ȱʧ��Ӧ��X������
	if (m_tVarY.iCount == 1)
	{
		nColIndY = m_tVarY.pValue[0];
		nDataTyY = m_pDataInterface->GetFieldType(nColIndY); 
		szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 	
		nRowCntY = m_pDataInterface->GetColData(nColIndY, pDataSetY, 1);	
		//�����Ч���ݸ���
		nTmp = 0;
		for (i=0; i<nRowCntY; i++)
		{
			if (pDataSetY->iIndex > 0)//������1��ʼ
				nTmp ++;
		}
		TDataSet *pDataSet = new TDataSet[nTmp];
		int nTempID = 0;
		for (i=0; i<nRowCntY; i++)
		{
			nID = pDataSetY[i].iIndex;
			if( nID > 0)
			{				
				pDataSet[nTempID].dValue = pDataSetX[nID-1].dValue;
				pDataSet[nTempID].iIndex = pDataSetX[nID-1].iIndex;
				pDataSet[nTempID].strVal = pDataSetX[nID-1].strVal;
				nTempID ++;
			}
		}
		delete []pDataSetX;
		pDataSetX = pDataSet;
		nGroupCntX = CStatistics::GetGroupingData(pDataSetX,nTmp,pGroupInfor);
	}	
	else
	{
		nGroupCntX = CStatistics::GetGroupingData(pDataSetX,nRowCntX,pGroupInfor);
	}
	//����
	if (nGroupCntX > MAX_VALID_INT)
	{
		m_szErrMsg.Format("���ͼ�ڷ�������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		for (i=0; i<nGroupCntX; i++)
		{
			pGroupInfor[i].DeleteObject();
		}
		delete []pGroupInfor;
		if (pDataSetX != NULL) delete []pDataSetX;
		if (pDataSetY != NULL) delete []pDataSetY;
		return false;
	}
	if (nGroupCntX <= 1) 
	{			
		m_szErrMsg.Format("���ͼ�ڷ��������̫�٣��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		for (i=0; i<nGroupCntX; i++)
		{
			pGroupInfor[i].DeleteObject();
		}
		delete []pGroupInfor;
		if (pDataSetX != NULL) delete []pDataSetX;
		if (pDataSetY != NULL) delete []pDataSetY;
		return false;
	}
	//����
	int nValidCount = nGroupCntX + 2;
	if (nValidCount > MAX_VALID_INT)
	{
		m_szErrMsg.Format("���ͼ�����ݳ��� %d ���޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		delete []pGroupInfor;
		if (pDataSetX != NULL) delete []pDataSetX;
		if (pDataSetY != NULL) delete []pDataSetY;
		return false;
	}
	TDataPointStrVal *pDPSV = new TDataPointStrVal[nValidCount];	
	if (m_bWeight)
	{
		nRowCntX = 0;
		for (i=0; i<nGroupCntX; i++)
		{
			for (j=0; j<pGroupInfor[i].pIndex.iCount; j++)
			{
				nID = pGroupInfor[i].pIndex.pValue[j];
				nRowCntX += m_pDataInterface->GetWeightData(nID);				
			}
		}
	}
	//��������
	for (i=0; i<nValidCount; i++)
	{	
		if (i == 0)
		{
			if (nDataTyX != fString)
			{
				if (pGroupInfor[i].pIndex.pValue[0] > 0)
				{
					szTemp.Format("%5.1f",pGroupInfor[i].dValue); 
					szTemp.TrimLeft();
				}
				else
					szTemp = "ȱʧ";
				pDPSV[i].fXVal = szTemp;
			}
			else
			{
				if (pGroupInfor[i].pIndex.pValue[0] > 0)
					pDPSV[i].fXVal = pGroupInfor[i].strVal ;
				else
					pDPSV[i].fXVal = "ȱʧ" ;
			}
			
			nTempCnt = pGroupInfor[i].pIndex.iCount;
			if (m_bWeight)
			{
				nWeight = 0;
				for (k=0; k<nTempCnt; k++)
				{
					nID = pGroupInfor[i].pIndex.pValue[k];	
					nWeight += m_pDataInterface->GetWeightData(nID); 
				}
				nTempCnt = nWeight;
			}
			//����
			switch (eData)
			{
			case REC:
				pDPSV[i].fYVal = nTempCnt;
				break;
			case RECSCALE:	
				{
					if (nRowCntX)
						pDPSV[i].fYVal = (int)(nTempCnt*100/nRowCntX);
					else
						pDPSV[i].fYVal = 0.0;
				}
				break;
			case RECTOTAL:		
				{					
					pDPSV[i].fYVal = nTmp;
				}
				break;
			case RECTOTALSCALE:
				{
					if (nRowCntX)
						pDPSV[i].fYVal = (int)((nTmp*100)/nRowCntX);
					else
						pDPSV[i].fYVal = 0.0;
				}
				break;
			default:
				{
					nDataCntY = CStatistics::GetGroupData(pDataSetY,nRowCntY,pGroupInfor[i].pIndex ,pDataSetYGroup);
					statistics.SetData(pDataSetYGroup,nDataCntY);
					if (!GetStatistical(statistics,eData,nDataCntY,pDPSV[i].fYVal))
					{
						if (pDataSetX != NULL) delete []pDataSetX;
						if (pDataSetY != NULL) delete []pDataSetY;
						for (i=0; i<nGroupCntX; i++)
						{
							pGroupInfor[i].DeleteObject();
						}
						if (pGroupInfor != NULL) delete []pGroupInfor;
						if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
						return false;
					}
					if (!GetAxisLabel(eData,szYAxisLabel))
					{
						if (pDataSetX != NULL) delete []pDataSetX;
						if (pDataSetY != NULL) delete []pDataSetY;
						for (i=0; i<nGroupCntX; i++)
						{
							pGroupInfor[i].DeleteObject();
						}
						if (pGroupInfor != NULL) delete []pGroupInfor;
						if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
						return false;
					}
					szYAxisLabel += " " + szFldNameY;  
					if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
					pDataSetYGroup = NULL;
				}
				break;
			}
		}
		else if (i == nGroupCntX + 1)
		{
			if (nDataTyX != fString)
			{
				if (pGroupInfor[i-2].pIndex.pValue[0] > 0)
				{
					szTemp.Format("%5.1f",pGroupInfor[i-2].dValue); 
					szTemp.TrimLeft();
				}
				else
				{
					szTemp = "ȱʧ";
				}
				pDPSV[i].fXVal = szTemp;
			}
			else
			{
				if (pGroupInfor[i-2].pIndex.pValue[0] > 0)
					pDPSV[i].fXVal = pGroupInfor[i-2].strVal;
				else
					pDPSV[i].fXVal = "ȱʧ" ;
			}
			
			nTempCnt = pGroupInfor[i-2].pIndex.iCount;
			
			if (m_bWeight)
			{
				for (k=0; k<nTempCnt; k++)
				{
					nID = pGroupInfor[i-2].pIndex.pValue[k];	
					nWeight += m_pDataInterface->GetWeightData(nID); 
				}
				nTempCnt = nWeight;
			}
			//����
			switch (eData)
			{
			case REC:
				pDPSV[i].fYVal = nTempCnt;
				break;
			case RECSCALE:	
				{
					if(nRowCntX)
						pDPSV[i].fYVal = (int)(nTempCnt*100/nRowCntX);
					else
						pDPSV[i].fYVal = 0.0;
				}
				break;
			case RECTOTAL:				
				{					
					pDPSV[i].fYVal = nTmp;
				}
				break;
			case RECTOTALSCALE:
				{
					if (nRowCntX)
						pDPSV[i].fYVal = (int)((nTmp*100)/nRowCntX);
					else
						pDPSV[i].fYVal = 0.0;
				}
				break;
			default:
				{
					nDataCntY = CStatistics::GetGroupData(pDataSetY,nRowCntY,pGroupInfor[i-2].pIndex ,pDataSetYGroup);
					statistics.SetData(pDataSetYGroup,nDataCntY);
					if (!GetStatistical(statistics,eData,nDataCntY,pDPSV[i].fYVal))
					{
						if (pDataSetX != NULL) delete []pDataSetX;
						if (pDataSetY != NULL) delete []pDataSetY;
						for (i=0; i<nGroupCntX; i++)
						{
							pGroupInfor[i].DeleteObject();
						}
						if (pGroupInfor != NULL) delete []pGroupInfor;
						if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
						return false;
					}
					if (!GetAxisLabel(eData,szYAxisLabel))
					{
						if (pDataSetX != NULL) delete []pDataSetX;
						if (pDataSetY != NULL) delete []pDataSetY;
						for (i=0; i<nGroupCntX; i++)
						{
							pGroupInfor[i].DeleteObject();
						}
						if (pGroupInfor != NULL) delete []pGroupInfor;
						if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
						return false;
					}
					szYAxisLabel += " " + szFldNameY;  
					if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
					pDataSetYGroup = NULL;
				}
				break;
			}
		}
		else		
		{
			if (nDataTyX != fString)
			{
				if(pGroupInfor[i-1].pIndex.pValue[0] > 0)
				{
					szTemp.Format("%5.1f",pGroupInfor[i-1].dValue); 
					szTemp.TrimLeft();
				}
				else
					szTemp = "ȱʧ";
				
				pDPSV[i].fXVal = szTemp;
			}
			else
			{
				if (pGroupInfor[i-1].pIndex.pValue[0] > 0)
					pDPSV[i].fXVal = pGroupInfor[i-1].strVal ;
				else
					pDPSV[i].fXVal = "ȱʧ";
			}
			
			nTempCnt = pGroupInfor[i-1].pIndex.iCount;
			if (m_bWeight)
			{
				nWeight = 0;
				for (k=0; k<nTempCnt; k++)
				{
					nID = pGroupInfor[i-1].pIndex.pValue[k];	
					nWeight += m_pDataInterface->GetWeightData(nID); 
				}
				nTempCnt = nWeight;
			}
			//����
			switch (eData)
			{
			case REC:
				pDPSV[i].fYVal = nTempCnt;
				break;
			case RECSCALE:	
				{
					if (nRowCntX)
						pDPSV[i].fYVal = nTempCnt*100.0/nRowCntX;				
					else
						pDPSV[i].fYVal = 0.0;
				}
				break;
			case RECTOTAL:				
				{					
					nTmp += nTempCnt;
					pDPSV[i].fYVal = nTmp;
				}
				break;
			case RECTOTALSCALE:
				{					
					nTmp += nTempCnt;
					if (nRowCntX)
						pDPSV[i].fYVal = (int)((nTmp*100)/nRowCntX);
					else
						pDPSV[i].fYVal = 0.0;
				}
				break;
			default:
				{
					nDataCntY = CStatistics::GetGroupData(pDataSetY,nRowCntY,pGroupInfor[i-1].pIndex ,pDataSetYGroup);
					statistics.SetData(pDataSetYGroup,nDataCntY);
					if (!GetStatistical(statistics,eData,nDataCntY,pDPSV[i].fYVal))
					{
						if (pDataSetX != NULL) delete []pDataSetX;
						if (pDataSetY != NULL) delete []pDataSetY;
						for (i=0; i<nGroupCntX; i++)
						{
							pGroupInfor[i].DeleteObject();
						}
						if (pGroupInfor != NULL) delete []pGroupInfor;
						if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
						return false;
					}
					if (!GetAxisLabel(eData,szYAxisLabel))
					{
						if (pDataSetX != NULL) delete []pDataSetX;
						if (pDataSetY != NULL) delete []pDataSetY;
						for (i=0; i<nGroupCntX; i++)
						{
							pGroupInfor[i].DeleteObject();
						}
						if (pGroupInfor != NULL) delete []pGroupInfor;
						if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
						return false;
					}
					szYAxisLabel += " " + szFldNameY;
					if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
					pDataSetYGroup = NULL;
				}
				break;
			}
		}
	}
	//ͼ��	
	CTChartArea *pArea = new CTChartArea;
	CRsltElementChart *pChart = new CRsltElementChart("���ͼ",pArea);
	pArea->SetArea(pDPSV,nValidCount,StrVal);
	pArea->SetXAxilLabel(szFldNameX);
	pArea->SetYAxilLabel(szYAxisLabel);
	m_pResult->SetName("���ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	if (pDPSV != NULL)     delete []pDPSV;
	if (pDataSetX != NULL) delete []pDataSetX;
	if (pDataSetY != NULL) delete []pDataSetY;
	for (i=0; i<nGroupCntX; i++)
	{
		pGroupInfor[i].DeleteObject();
	}
	if (pGroupInfor != NULL) delete []pGroupInfor;
	return true;
}

bool CArea::CalcArea2()
{
	int i=0, j=0, nCalcCnt=0;
	int nColIndex=0, nRowCount=0, nRealCount=0;
	int *pIndex = NULL;
	CTString szFldNameX("");
	CTString szYAxisLabel("");
	TDataSet* pDataSet=NULL;
	TDataSet *pTempDataSet=NULL;
	EData eData;
	CStatistics statistics;
	CTArray<TDataSet*,TDataSet*> arrDataSet;
	
	int nVarCount = m_tVarX.iCount; 
	if (nVarCount < 2)
	{
		m_szErrMsg.Format("���ͼ�����ݲ�������������Ҫ�������������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (nVarCount > MAX_VALID_INT/10)
	{
		m_szErrMsg.Format("���ͼ�еķ������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT/10,__LINE__,__FILE__);	
		return false;
	}
	//�Ƿ������ͬ�ĺ�������
	bool bExistSameFunc = IsSameCallFunc(XAXIS_INT);
	//������
	int nSize = nVarCount + 2;
	if (nSize > MAX_VALID_INT)
	{
		m_szErrMsg.Format("���ͼ�еķ������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT,__LINE__,__FILE__);	
		return false;
	}
	nCalcCnt = 0;
	for (i=0; i<nSize; i++)
	{
		nCalcCnt = 0;
		if (i == 0)
		{
			nColIndex = m_tVarX.pValue[i]; 
		}
		else if(i == nSize-1)
		{
			nColIndex = m_tVarX.pValue[i-2]; 
		}
		else
		{
			nColIndex = m_tVarX.pValue[i-1]; 
		}
		nRowCount = m_pDataInterface->GetColData(nColIndex,pTempDataSet,1);
		if (i == 0)
			pIndex = new int[nRowCount];
		for (j=0; j<nRowCount; j++)
		{			
			if(pTempDataSet[j].iIndex < 0)
			{			
				pIndex[j] = 0;
			}
			else
			{
				if (i == 0)
				{
					pIndex[j] = 1;
					nCalcCnt ++;			
				}
				else if(pIndex[j] != 0)
				{
					pIndex[j] = 1;
					nCalcCnt ++;			
				}
			}
		}		
		arrDataSet.Add(pTempDataSet);				
	}
	//��֤
	if (nCalcCnt < 1)
	{
		m_szErrMsg.Format("���ͼ������Ч�����ݣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		if (pIndex != NULL) delete pIndex;
		pIndex = NULL;
		for (i=0; i<arrDataSet.GetSize(); i++)
		{			
			TDataSet *p = arrDataSet.GetAt(i);
			delete []p;		
		}
		arrDataSet.RemoveAll();
		return false;
	}
	//����
	nRowCount = m_pDataInterface->GetRowCount();
	//׼������
	TDataPointStrVal* pDPSV = new TDataPointStrVal[nSize];
	for (i=0;i<nSize;i++)
	{		
		if (i == 0)
		{
			nColIndex = m_tVarX.pValue[i]; 
			pDataSet = arrDataSet.GetAt(i);
		}
		else if(i == nSize-1)
		{
			nColIndex = m_tVarX.pValue[i-2]; 
			pDataSet = arrDataSet.GetAt(i-2);
		}
		else
		{
			nColIndex = m_tVarX.pValue[i-1]; 
			pDataSet = arrDataSet.GetAt(i-1);
		}
		szFldNameX = m_pDataInterface->GetFieldName(nColIndex);
		TDataSet *pTempData = new TDataSet[nCalcCnt];
		int nID = 0;
		for (j=0; j<nRowCount; j++)
		{
			if (pIndex[j] == 1)
			{
				pTempData[nID].dValue = pDataSet[j].dValue;
				pTempData[nID].strVal = pDataSet[j].strVal;
				pTempData[nID].iIndex = pDataSet[j].iIndex;
				nID++;
			}
		}		
		//�������������
		if (!GetDataType(i,XAXIS_INT,eData))
		{
			for (i=0; i<arrDataSet.GetSize(); i++)
			{			
				TDataSet *p = arrDataSet.GetAt(i);
				delete []p;		
			}
			arrDataSet.RemoveAll();
			if (pIndex != NULL) delete []pIndex;
			pIndex = NULL;
			if (pTempData != NULL) delete []pTempData;
			pTempData = NULL;
			return false;
		}
		//��ֵ
		statistics.SetData(pTempData,nCalcCnt);
		if (!GetStatistical(statistics,eData,nCalcCnt,pDPSV[i].fYVal))
		{
			for (i=0; i<arrDataSet.GetSize(); i++)
			{			
				TDataSet *p = arrDataSet.GetAt(i);
				delete []p;		
			}
			arrDataSet.RemoveAll();
			if (pIndex != NULL) delete []pIndex;
			pIndex = NULL;
			if (pTempData != NULL) delete []pTempData;
			pTempData = NULL;
			return false;
		}
		if (!GetAxisLabel(eData,szYAxisLabel))
		{
			for (i=0; i<arrDataSet.GetSize(); i++)
			{			
				TDataSet *p = arrDataSet.GetAt(i);
				delete []p;		
			}
			arrDataSet.RemoveAll();
			if (pIndex != NULL) delete []pIndex;
			pIndex = NULL;
			if (pTempData != NULL) delete []pTempData;
			pTempData = NULL;
			return false;
		}
		if (!bExistSameFunc) 
			szFldNameX = szYAxisLabel + " " + szFldNameX.GetData();
		pDPSV[i].fXVal = szFldNameX; 	
		if (pTempData != NULL) delete []pTempData;
			pTempData = NULL;
	}
	//ͼ��
	CTChartArea *pArea = new CTChartArea;
	CRsltElementChart *pChart = new CRsltElementChart("���ͼ",pArea);
	pArea->SetArea(pDPSV,nSize,StrVal);
	m_pResult->SetName("���ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	for (i=0; i<arrDataSet.GetSize(); i++)
	{			
		TDataSet *p = arrDataSet.GetAt(i);
		delete []p;		
	}
	arrDataSet.RemoveAll();
	if (pDPSV != NULL) delete []pDPSV;
	pDPSV = NULL;
	if (pIndex != NULL) delete []pIndex;
	pIndex = NULL;
	return true;
}

bool CArea::CalcArea3()
{
	int i=0, j=0, nCalcCnt=0;
	int nRowCntX=0, nRowCntY=0;
	int nColIndX=0, nColIndY=0;
	int nDataTyX=0, nDataTyY=0;
	TDataSet *pDataSetX = NULL;
	TDataSet *pDataSetY = NULL;
	CTString szTemp("");
	CTString szFldNameY("");
	CTString szFldNameX("");
	CStatistics statistics;
	
	if (m_tVarY.iCount != 1) 
	{
		m_szErrMsg.Format("���ͼ��û��ָ������������ݣ����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//ȡY������(���m_nSel==0��X������ţ����m_nSel == 1��X������һ������)
	szFldNameX = "Number";
	nColIndY = m_tVarY.pValue[0];
	nRowCntY = m_pDataInterface->GetColData(nColIndY,pDataSetY,1);
	nDataTyY = m_pDataInterface->GetFieldType(nColIndY); 
	szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 
	int nSize = nRowCntY + 2;
	if (nSize > MAX_VALID_INT)
	{
		m_szErrMsg.Format("���ͼ�еķ������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT,__LINE__,__FILE__);	
		if (pDataSetY != NULL) delete []pDataSetY;
		return false;
	}
	if (m_tVarX.iCount == 1)//������ʽ
	{
		nDataTyX = fString;
		nColIndX = m_tVarX.pValue[0];
		szFldNameX = m_pDataInterface->GetFieldName(nColIndX); 
		if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndX))
		{
			m_pDataInterface->GetTLDBColDataLabel(nColIndX,pDataSetX,1);
		}
		else
		{
			m_pDataInterface->GetColData(nColIndX,pDataSetX,1);
			nDataTyX = m_pDataInterface->GetFieldType(nColIndX); 
		}
	}
	//׼������
	TDataPointStrVal *pDPSV = new TDataPointStrVal[nSize];
	for (i=0; i<nSize; i++)
	{
		if(i == 0)
		{
			if (m_tVarX.iCount == 0)
			{				
				szTemp.Format("%d",(i+1));
				pDPSV[i].fXVal = szTemp;								
				pDPSV[i].fYVal = pDataSetY[i].dValue;
			}
			else
			{
				int nIndex = pDataSetY[i].iIndex;				
				if (pDataSetX[nIndex-1].iIndex > 0)
				{
					if (nDataTyX == fString)
						pDPSV[i].fXVal = pDataSetX[nIndex-1].strVal;
					else
					{
						szTemp.Format("%5.1f",pDataSetX[nIndex-1].dValue);
						pDPSV[i].fXVal = szTemp;
					}
				}
				else
				{
					pDPSV[i].fXVal = "ȱʧ";
				}
			}
			if (pDataSetY[i].iIndex > 0)
				pDPSV[i].fYVal = pDataSetY[i].dValue;
			else
				pDPSV[i].fYVal = 0;
		}
		else if (i == nSize-1)
		{
			if (m_tVarX.iCount == 0)
			{			
				szTemp.Format("%d",(i-1));
				pDPSV[i].fXVal = szTemp;			
			}
			else
			{
				int nIndex = pDataSetY[i-2].iIndex;
				if(pDataSetX[nIndex-1].iIndex > 0)
				{
					if (nDataTyX == fString)
						pDPSV[i].fXVal = pDataSetX[nIndex-1].strVal;
					else
					{
						szTemp.Format("%5.1f", pDataSetX[nIndex-1].dValue);
						pDPSV[i].fXVal = szTemp;
					}
				}
				else
				{
					pDPSV[i].fXVal = "ȱʧ";
				}
			}
			pDPSV[i].fYVal = pDataSetY[i-2].dValue;
		}		
		else
		{
			if (m_tVarX.iCount == 0)
			{			
				szTemp.Format("%d",i);
				pDPSV[i].fXVal = szTemp;
			}
			else
			{
				int nIndex = pDataSetY[i-1].iIndex;
				if (pDataSetX[nIndex-1].iIndex > 0)
				{
					if (nDataTyX == fString)
						pDPSV[i].fXVal = pDataSetX[nIndex-1].strVal;
					else
					{
						szTemp.Format("%5.1f", pDataSetX[nIndex-1].dValue);
						pDPSV[i].fXVal = szTemp;
					}
				}
				else
				{
					pDPSV[i].fXVal = "ȱʧ";
				}
			}
			if(pDataSetY[i-1].iIndex>0)
				pDPSV[i].fYVal = pDataSetY[i-1].dValue;
			else
				pDPSV[i].fYVal = 0;
		}
	}	
	//ͼ��
	CTChartArea *pArea = new CTChartArea;
	CRsltElementChart *pChart = new CRsltElementChart("���ͼ",pArea);
	pArea->SetArea(pDPSV,nSize,StrVal);
	pArea->SetXAxilLabel(szFldNameX);
	pArea->SetYAxilLabel(szFldNameY);
	m_pResult->SetName("���ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	delete []pDataSetX;
	delete []pDataSetY;
	delete []pDPSV;
	return true;
}