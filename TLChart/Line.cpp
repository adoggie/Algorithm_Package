// Line.cpp: implementation of the CLine class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Line.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLine::CLine()
{
	m_szErrMsg = "";
}

CLine::~CLine()
{
}

bool CLine::Verify()
{
	int i = 0;
	int nColIndex = 0, nDataType = 0;
	CTString szFldName("");

	if (m_eMode == CLASS)
	{
		if (m_tVarX.iCount != 1)
		{
			m_szErrMsg.Format("ֱ��ͼ�е������δָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_nAVType & AXIS_DESC_INT)
		{
			m_szErrMsg.Format("ֱ��ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if ((m_nAVType & VAR_DESC_INT  && m_tVarY.iCount == 1) ||
			(m_nAVType & VAR_NDESC_INT && m_tVarY.iCount != 1))
		{
			m_szErrMsg.Format("ֱ��ͼ�е�ֵ������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_eType == MULTIPLELINE)
		{
			if (m_tVarD.iCount != 1)
			{
				m_szErrMsg.Format("ֱ��ͼ�е��Զ����б�������δָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
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
		if (m_eType == SIMPLELINE)
		{
			if (m_nAVType & AXIS_DESC_INT)
			{
				m_szErrMsg.Format("ֱ��ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
			if (m_tVarX.iCount < 2)
			{
				m_szErrMsg.Format("ֱ��ͼ�е�����������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
			if (m_arrX.GetSize() < 1)
			{
				m_szErrMsg.Format("ֱ��ͼ�е�ֵ��������û�ж��壬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
			for (i=0; i<m_tVarX.iCount; i++)
			{
				nColIndex = m_tVarX.pValue[i];
				nDataType = m_pDataInterface->GetFieldType(nColIndex);
				if (nDataType == fString)
				{ 
					szFldName = m_pDataInterface->GetFieldName(nColIndex);
					m_szErrMsg.Format("ֱ��ͼ�е�������������Ͳ���Ϊ�ַ���-%s���޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
					return false;
				}
			}
		}
		else
		{
			if (m_nAVType & AXIS_DESC_INT)
			{
				m_szErrMsg.Format("ֱ��ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
			if (m_tVarX.iCount != 1)
			{
				m_szErrMsg.Format("ֱ��ͼ�е������û��ָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
			if (m_tVarY.iCount < 2)
			{
				m_szErrMsg.Format("ֱ��ͼ�е�ֵ�������������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
			if (m_arrY.GetSize() < 1)
			{
				m_szErrMsg.Format("ֱ��ͼ�е�ֵ������������û�ж��壬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
			for (i=0; i<m_tVarY.iCount; i++)
			{
				nColIndex = m_tVarY.pValue[i];
				nDataType = m_pDataInterface->GetFieldType(nColIndex);
				if (nDataType == fString)
				{ 
					szFldName = m_pDataInterface->GetFieldName(nColIndex);
					m_szErrMsg.Format("ֱ��ͼ�е�ֵ�����������Ͳ���Ϊ�ַ���-%s���޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
					return false;
				}
			}
		}
	}
	else if (m_eMode == VALUE)
	{
		if (m_nAVType & VAR_DESC_INT)
		{
			m_szErrMsg.Format("ֱ��ͼ�е�ֵ�����������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_eType == SIMPLELINE && m_tVarY.iCount != 1)
		{
			m_szErrMsg.Format("ֱ��ͼ�е�ֵ����������Ҫָ��һ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_eType == MULTIPLELINE && m_tVarY.iCount < 2)
		{
			m_szErrMsg.Format("ֱ��ͼ�е�ֵ�������������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if ((m_nAVType & AXIS_DESC_INT  && m_tVarX.iCount == 1) ||
			(m_nAVType & AXIS_NDESC_INT && m_tVarX.iCount != 1))
		{
			m_szErrMsg.Format("ֱ��ͼ�е��������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<m_tVarY.iCount; i++)
		{
			nColIndex = m_tVarY.pValue[i];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			if (nDataType == fString)
			{ 
				szFldName = m_pDataInterface->GetFieldName(nColIndex);
				m_szErrMsg.Format("ֱ��ͼ�е�ֵ�����������Ͳ���Ϊ�ַ���-%s���޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
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

bool CLine::GetValueFromVo(string VO)
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

bool CLine::ParseTypeMode(string VO)
{
	int nSize=0;
	CTString szTemp;
	CTStringArray szArrName;

	AnaWord VOstr;
	VOstr.Import(VO);
	//����
	if (AnaSyntax::exist(TYPE_STR,VOstr)) //ֱ��ͼ|��״ͼ|����ͼ|ɢ��ͼ|����ͼ|
	{
		AnaSyntax::getVAR(TYPE_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("ֱ��ͼ���Ͳ�ȷ�������飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);
		szTemp.MakeUpper();
		if (szTemp == SIMPLELINE_STR)
		{
			m_eType = SIMPLELINE;
		}
		else if (szTemp == MULTIPLELINE_STR)
		{
			m_eType = MULTIPLELINE;
		}
		else
		{
			m_szErrMsg.Format("ֱ��ͼ�в�������Ӧ�����ͣ�%s�����飡Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("ֱ��ͼ������û�б�ָ�������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	szArrName.RemoveAll();
	if (AnaSyntax::exist(MODE_STR,VOstr)) 
	{
		AnaSyntax::getVAR(MODE_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("ֱ��ͼ�е��������Ͳ�ȷ�������飡Line=%d,File=%s",__LINE__,__FILE__);
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
			m_szErrMsg.Format("ֱ��ͼ�е��������Ͳ����ڣ�%s�����飡Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("ֱ��ͼ����������û�б�ָ�������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	return true;
}

CResult *CLine::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//ͼ��
	SetChartType(CHART_LINE);
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
	if (m_eType == SIMPLELINE)
	{
		if (m_eMode == CLASS) 
			bRet = CalcSimpleLine1();
		else if (m_eMode == VARI) 
			bRet = CalcSimpleLine2();
		else if (m_eMode == VALUE)
			bRet = CalcSimpleLine3();
	}
	else
	if (m_eType == MULTIPLELINE)
	{
		if (m_eMode == CLASS) 
			bRet = CalcMultipleLine1();
		else if (m_eMode == VARI) 
			bRet = CalcMultipleLine2();
		else if (m_eMode == VALUE)
			bRet = CalcMultipleLine3();
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

bool CLine::CalcSimpleLine1()
{
	int i=0, j=0, k=0, nTmp=0;
	int nColIndX=0, nColIndY=0;
	int nRowCntX=0, nRowCntY=0;
	int nDataTyX=0, nDataTyY=0;
	int nWeight=0, nTotalCount=0;
	double fTotalCount = 0;
	TDataSet *pDataSetX = NULL;
	TDataSet *pDataSetY = NULL;
	TDataSet* pDataSetYGroup = NULL;
	CTString szTemp = "";
	CTString szFldNameX = "";
	CTString szFldNameY = "";
	CTString szYAxisLabel = "";
	TDataPointStrVal* pDPSV = NULL;
	CStatistics statistics;
	EData eData;

	//X�����Ϊ����
	if (m_tVarX.iCount != 1)
	{
		m_szErrMsg.Format("ֱ��ͼ��û��ָ��X�����ݣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//Y����������
	if (!GetDataType(0,YAXIS_INT,eData)) 
	{
		return false;
	}
	//ȡX������
	nColIndX = m_tVarX.pValue[0];
	nDataTyX = m_pDataInterface->GetFieldType(nColIndX); 
	szFldNameX = m_pDataInterface->GetFieldName(nColIndX); 
	if(m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndX))
		nRowCntX =	m_pDataInterface->GetTLDBColDataLabel(nColIndX,pDataSetX,1);
	else
		nRowCntX = m_pDataInterface->GetColData(nColIndX,pDataSetX,1);
	//���pDataSetY����ȱʧֵ���������ݽ���ɾ��
	if(m_tVarY.iCount == 1)
	{
		int nRealCntY=0,nRealIndY=1;
		nColIndY = m_tVarY.pValue[0];
		nRowCntY = m_pDataInterface->GetColData(nColIndY,pDataSetY,1);
		szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 
		//����pDataSetY����ȱʧ��ʱ�򣬴���pDataSetX�ж�Ӧ������
		for (i=0; i<nRowCntY; i++)
		{
			if( pDataSetY[i].iIndex > 0) 
				nRealCntY ++;
		}
		TDataSet *pDataSetX1 = new TDataSet[nRealCntY];
		TDataSet *pDataSetY1 = new TDataSet[nRealCntY];
		for (i = 0; i<nRowCntY; i++)
		{
			if( pDataSetY[i].iIndex > 0) 
			{
				pDataSetX1[nRealIndY-1] = pDataSetX[i];
				pDataSetY1[nRealIndY-1]  = pDataSetY[i];
				pDataSetX1[nRealIndY-1].iIndex = (pDataSetX[i].iIndex > 0) ? nRealIndY : -nRealIndY;
				pDataSetY1[nRealIndY-1].iIndex = nRealIndY;
				nRealIndY ++;
			}
		}
		//�ͷ��ڴ�
		delete []pDataSetX;
		delete []pDataSetY;
		pDataSetX = pDataSetX1;
		pDataSetY = pDataSetY1;
		nRowCntX = nRealCntY;
		nRowCntY = nRealCntY;
	}
	//����������ݴ�С
	TGroupInfor* pGroupInfor = NULL;
	int nGroupCount = CStatistics::GetGroupingData(pDataSetX,nRowCntX,pGroupInfor);
	if (nGroupCount > MAX_VALID_INT)
	{
		m_szErrMsg.Format("�������ݴ��� %d �飬��ǰ�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT,__LINE__,__FILE__);
		for (i=0; i<nGroupCount; i++)
		{
			pGroupInfor[i].DeleteObject();
		}
		if (pDataSetX != NULL) delete []pDataSetX;
		if (pDataSetY != NULL) delete []pDataSetY;
		if (pGroupInfor != NULL) delete []pGroupInfor;
		return false;
	}
	//����
	pDPSV = new TDataPointStrVal[nGroupCount];
	if (m_bWeight)
	{
		nRowCntX = 0;
		//����Ȩ��
		m_pDataInterface->SetWeightNumb(m_nWeight);
		for (j=0; j<nGroupCount; j++)
		{
			for (k=0; k<pGroupInfor[j].pIndex.iCount; k++)
			{
				nTmp = pGroupInfor[j].pIndex.pValue[k];
				nRowCntX += m_pDataInterface->GetWeightData(nTmp);				
			}
		}
	}
	
	for (i=0; i<nGroupCount; i++)
	{		
		nWeight = 0;
		if (nDataTyX != fString)
		{
			if (pGroupInfor[i].pIndex.pValue[0] > 0) 
			{
				szTemp.Format("%5.2f",pGroupInfor[i].dValue); 
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
			if (pGroupInfor[i].pIndex.pValue[0] > 0) 
				pDPSV[i].fXVal = pGroupInfor[i].strVal ;
			else
				pDPSV[i].fXVal = "ȱʧ";        
		}
		double fGroupCntY = pGroupInfor[i].pIndex.iCount;
		if (m_bWeight)
		{
			for (j=0; j<fGroupCntY; j++)
			{
				nTmp = pGroupInfor[i].pIndex.pValue[j];
				nWeight += m_pDataInterface->GetWeightData(nTmp);
			}					
			fGroupCntY = (double)nWeight;
		}
		switch (eData)
		{
		case REC:
			{				
				pDPSV[i].fYVal = fGroupCntY;
			}
			break;
		case RECSCALE:
			{
				if(nRowCntX)
					pDPSV[i].fYVal = 100*fGroupCntY/double(nRowCntX);
				else
					pDPSV[i].fYVal = 0;
			}
			break;
		case RECTOTAL:
			{
				pDPSV[i].fYVal = fGroupCntY + nTotalCount;
				nTotalCount += fGroupCntY;
			}
			break;
		case RECTOTALSCALE:					
			{
				double fPercent = 0;
				if(nRowCntX)
					fPercent = 100*fGroupCntY/double(nRowCntX);
				pDPSV[i].fYVal = fPercent + fTotalCount;
				fTotalCount+=fPercent;
			}
			break;
		default:
			{
				int nGroupDataCount = CStatistics::GetGroupData(pDataSetY,nRowCntY,pGroupInfor[i].pIndex ,pDataSetYGroup,true);
				statistics.SetData(pDataSetYGroup,nGroupDataCount);
				if (!GetStatistical(statistics,eData,nGroupDataCount,pDPSV[i].fYVal))
				{
					if (pDataSetX != NULL) delete []pDataSetX;
					if (pDataSetY != NULL) delete []pDataSetY;
					for (i=0; i<nGroupCount; i++)
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
					for (i=0; i<nGroupCount; i++)
					{
						pGroupInfor[i].DeleteObject();
					}
					if (pGroupInfor != NULL) delete []pGroupInfor;
					if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
					return false;
				}
				szYAxisLabel += " " + szFldNameY; 
				delete []pDataSetYGroup;
				pDataSetYGroup = NULL;
			}
			break;
		}
	}
	//Y����������
	switch(eData)
	{
	case REC:
	case RECSCALE:
	case RECTOTAL:
	case RECTOTALSCALE:
		GetAxisLabel(eData, szYAxisLabel);
		break;
	}
	//ͼ��
	CTChartLine *pLine = new CTChartLine;
	pLine->SetLine(pDPSV, nGroupCount, StrVal);
	pLine->SetXAxilLabel(szFldNameX);
	pLine->SetYAxilLabel(szYAxisLabel);
	CRsltElementChart *pChart = new CRsltElementChart("ֱ��ͼ",pLine);
	m_pResult->SetName("ֱ��ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	for (i=0; i<nGroupCount; i++)
	{
		pGroupInfor[i].DeleteObject();
	}
	delete[] pDPSV;
	if (pDataSetX != NULL) delete []pDataSetX;
	if (pDataSetY != NULL) delete []pDataSetY;
	if (pGroupInfor != NULL) delete []pGroupInfor;
	return true;
}

bool CLine::CalcSimpleLine2()
{
	double fTemp=0;
	int i=0, j=0;
	int nColIndex=0, nRowCount=0, nRealCount=0;
	CTString szTemp = "";
	CTString szFldName    = "";
	CTString szYAxisLabel = "";
	TDataSet* pDataSet = NULL;
	TDataSet *pTempData = NULL;
	TDataPointStrVal* pDPSV = NULL;
	EData eData;
	CStatistics statistics;
	
	int nVarCount = m_tVarX.iCount; 
	if (nVarCount < 2)
	{
		m_szErrMsg.Format("ֱ��ͼ�е����ݲ�������������Ҫ�������������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//�Ƿ������ͬ�ĺ�������
	bool bExistSameFunc = IsSameCallFunc(XAXIS_INT);
	//�����ڴ�
	nRowCount   = m_pDataInterface->GetRowCount();
	pDPSV       = new TDataPointStrVal[nVarCount];
	int *pValue = new int[nRowCount];
	//��ʼ����(û��ȱʧ)
	for (i=0; i<nRowCount; i++)
		*(pValue+i) = 1;
	nRealCount = nRowCount;
	//�������е�������
	for (i=0; i<nVarCount; i++)
	{
		nColIndex =  m_tVarX.pValue[i];
		nRowCount = m_pDataInterface->GetColData(nColIndex,pDataSet,1);
		for (j=0; j<nRowCount; j++)
		{
			if (pDataSet[j].iIndex < 0) 
			{
				if ( pValue[j] == 1) nRealCount --;
				pValue[j] = 0;
			}
		}
		delete []pDataSet;
		pDataSet = NULL;
	}
	//�Ƿ���ںϷ�����
	if (nRealCount < 1)
	{		
		m_szErrMsg.Format("����ѡ���������Ϊ0����ȱʧֵ̫�࣬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		delete []pDPSV;
		delete []pValue;
		return false;
	}
	if (nRealCount != nRowCount)
	{
		m_szErrMsg.Format("ע�����ڴ���ȱʧֵ����%d����¼��ɾ�������飡Line=%d,File=%s",nRowCount - nRealCount,__LINE__,__FILE__);
		CRsltElementText *pRslt = new CRsltElementText("����");
		pRslt->AddString(m_szErrMsg);	
		m_pResult->Add(pRslt);
	}
	for (i=0; i<nVarCount; i++)
	{
		int nRealIndex = 1;
		fTemp = 0.0;
		nColIndex = m_tVarX.pValue[i];
		szFldName = m_pDataInterface->GetFieldName(nColIndex); 
		nRowCount = m_pDataInterface->GetColData(nColIndex,pDataSet,1);
		pTempData = new TDataSet[nRealCount];
		for (j=0; j<nRowCount; j++)
		{
			if (pValue[j] > 0)
			{
				pTempData[nRealIndex-1] = pDataSet[j];
				nRealIndex ++;
			}
		}
		delete []pDataSet;
		pDataSet = pTempData;
		//�������������
		if (!GetDataType(i,XAXIS_INT,eData))
		{
			delete []pDPSV;
			delete []pValue;
			delete []pDataSet;
			return false;
		}
		//��ֵ
		nRowCount = nRealCount;
		statistics.SetData(pDataSet,nRowCount);
		if (!GetStatistical(statistics,eData,nRowCount,fTemp))
		{
			delete []pDPSV;
			delete []pValue;
			delete []pDataSet;
			return false;
		}
		if (!GetAxisLabel(eData,szYAxisLabel))
		{
			delete []pDPSV;
			delete []pValue;
			delete []pDataSet;
			return false;
		}
		szTemp.Format("%5.1f",fTemp); 
		szTemp.TrimLeft();
		pDPSV[i].fYVal = fTemp;
		if (!bExistSameFunc) szFldName = szYAxisLabel + " " + szFldName.GetData();
		pDPSV[i].fXVal = szFldName;
		delete []pDataSet;
	}	
	//ͼ��
	CTChartLine* pLine = new CTChartLine;
	pLine->SetLine(pDPSV,nVarCount,StrVal);
	pLine->SetYAxilLabel(bExistSameFunc ? szYAxisLabel : "�ܼ�");
	CRsltElementChart* pChart = new CRsltElementChart("ֱ��ͼ",pLine);
	m_pResult->SetName("ֱ��ͼ");
	m_pResult->Add(pChart);
	delete []pDPSV;
	delete []pValue;
	return true;
}

//��ͼ��ģʽΪVALUE,Y:Vari,X:Vari-REC
bool CLine::CalcSimpleLine3()
{	
	bool bFirst = false;
	int nRowCntX=0, nRowCntY=0;
	int nColIndX=0, nColIndY=0;
	int nDataTyX=0, nDataTyY=0;
	int i=0, nSize=0, nMissMinY=0, nMissMaxY=0, nRealCntY=0;

	TDataSet *pDataSetX = NULL;
	TDataSet *pDataSetY = NULL;
	CTChartLine *pLine  = NULL;
	CRsltElementChart* pChart = NULL;
	CTString szTemp = "";
	CTString szFldNameY = "";
	CTString szFldNameX = "";
	TDataPointStrVal* pDPSV=NULL;
	
	if (m_tVarY.iCount != 1) 
	{
		m_szErrMsg.Format("ֱ��ͼ��û��ָ��Y��������ݣ����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//ȡY������(���m_nSel==0��X������ţ����m_nSel == 1��X������һ������)
	nColIndY = m_tVarY.pValue[0];
	nRowCntY = m_pDataInterface->GetColData(nColIndY,pDataSetY,1);
	nDataTyY = m_pDataInterface->GetFieldType(nColIndY); 
	szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 
	
	//�����һ�������һ���ǿ�ֵ����ȱʧֵ����ͬʱ������Чֵ�ĸ���
	for (i=0; i<nRowCntY; i++)
	{
		if( pDataSetY[i].iIndex > 0)
		{
			nRealCntY ++;
			if(!bFirst)
			{
				nMissMinY = i,nMissMaxY = i,bFirst = true;
			}
			else
			{
				nMissMaxY = i;
			}
		}
	}
	if (nRealCntY == 0)
	{
		if( pDataSetY != NULL) delete []pDataSetY;
		m_szErrMsg.Format("ֱ��ͼ��������Ч�����ݣ��޷�����ͼ�Σ����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	else if (nRealCntY > 1) //���в�ֵ����
	{
		CStatistics::LinearInsert(pDataSetY, nRowCntY);
	}
	//������ֵ���ʵ����Ч����
	nRealCntY = nMissMaxY - nMissMinY + 1;
	//X�������ʽ
	if(m_tVarX.iCount == 1) //������ʽ
	{
		nColIndX = m_tVarX.pValue[0];
		nDataTyX = m_pDataInterface->GetFieldType(nColIndX);
		if(m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndX))
			nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndX,pDataSetX,1);
		else
			nRowCntX = m_pDataInterface->GetColData(nColIndX,pDataSetX,1);
		if (nRowCntX < 1)
		{
			m_szErrMsg.Format("ֱ��ͼ��X������Ч�����ݣ��޷�����ͼ�Σ����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		pDPSV = new TDataPointStrVal[nRowCntX];
	}
	else if (m_tVarX.iCount == 0)
	{
		pDPSV = new TDataPointStrVal[nRealCntY];
	}
	else
	{
		m_szErrMsg.Format("ֱ��ͼ��X������ݹ��࣬�޷�����ͼ�Σ����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if(m_tVarX.iCount == 1)
		nSize	= (nRowCntX < nRowCntY) ? nRowCntX : nRowCntY;
	else
		nSize = nRealCntY;
	//������ݵĴ�С
	if (nRowCntX>MAX_VALID_INT || nRowCntY>MAX_VALID_INT)
	{
		delete []pDPSV;
		if (pDataSetX != NULL) delete []pDataSetX;
		if (pDataSetY != NULL) delete []pDataSetY;
		m_szErrMsg.Format("ֱ��ͼ�з������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT,__LINE__,__FILE__);
		return false;
	}
	int nTotalCount=0;
	double fTotalCount=0.0;
	double TempSV = 0.0;	
	if(m_tVarX.iCount == 0)
	{			
		for (i=nMissMinY; i<=nMissMaxY; i++)
		{
			if (nDataTyY != fString)
			{
				pDPSV[i-nMissMinY].fYVal = pDataSetY[i].dValue;	
			}
			else
			{
				pDPSV[i-nMissMinY].fYVal = 0;
			}
			szTemp.Format("%d",(i+1));
			szTemp.TrimLeft();
			pDPSV[i-nMissMinY].fXVal = szTemp;
		}
	}
	if(m_tVarX.iCount == 1)
	{		
		szFldNameX = m_pDataInterface->GetFieldName(nColIndX); 
		for(int j=0;j<nSize;j++)
		{
			//Y��
			if (nDataTyY != fString)
			{
				pDPSV[j].fYVal = pDataSetY[j].dValue;
			}
			else
			{
				pDPSV[j].fYVal = 0;
			}		
			//X��
			if (nDataTyX == fString)
			{
				if (pDataSetX[j].iIndex > 0)
					pDPSV[j].fXVal = pDataSetX[j].strVal;
				else
					pDPSV[j].fXVal = "ȱʧ";
			}
			else
			{
				if (pDataSetX[j].iIndex > 0)
					pDPSV[j].fXVal.Format("%5.1f", pDataSetX[j].dValue);
				else
					pDPSV[j].fXVal = "ȱʧ";
			}
		}		
	}
	//ͼ��
	pLine  = new CTChartLine;
	pLine->SetLine(pDPSV,nSize,StrVal);
	pLine->SetXAxilLabel(szFldNameX);
	pLine->SetYAxilLabel(szFldNameY);
	pChart = new CRsltElementChart("ֱ��ͼ",pLine);
	m_pResult->SetName("ֱ��ͼ");
	m_pResult->Add(pChart);
	delete []pDPSV;
	if (pDataSetX != NULL) delete []pDataSetX;
	if (pDataSetY != NULL) delete []pDataSetY;
	return true;
}

bool CLine::CalcMultipleLine1()
{
	int i=0, j=0, k=0, l=0, nSum = 0, nWeight=0;
	int nDataTyX=0, nDataTyY=0, nDataTyD=0;
	int nRowCntX=0, nRowCntY=0, nRowCntD=0;
	int nTmpCntX=0, nTmpCntY=0, nTmpCntD=0;
	int nColIndexX=0, nColIndexY=0, nColIndexD=0;
	int nDataCntY=0, nRealIndex=0, nRowCntValid=0;
	TDataSet *pDataSetX = NULL;
	TDataSet *pTmpDataX = NULL;
	TDataSet *pDataSetY = NULL;
	TDataSet *pTmpDataY = NULL;
	TDataSet *pDataSetD = NULL;
	TDataSet *pTmpDataD = NULL;
	TDataSet *pDataSetYGroup = NULL;
	CTString szFldNameX("");
	CTString szFldNameY("");
	CTString szFldNameD("");
	CTString szYAxisLabel("");
	CTString szXVal(""),szDVal(""),szYVal("");
	CStatistics statistics;
	TDataPointStrVal *pDPSV= NULL;
	int *pValue  = NULL;
	
	//��֤����������
	if (m_tVarX.iCount != 1)
	{
		m_szErrMsg.Format("ֱ��ͼ��û�ж���X��������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (m_tVarD.iCount != 1)
	{
		m_szErrMsg.Format("ֱ��ͼ��û�ж����Զ����У��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (m_tVarY.iCount > 1)
	{
		m_szErrMsg.Format("ֱ��ͼ�ж���Y������ݱ���̫�࣬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//Y������
	EData eData = m_arrY.GetAt(0);
	//X��
	nDataTyX = fString;
	nColIndexX = m_tVarX.pValue[0];
	if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndexX))
	{
		nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndexX,pDataSetX,1);
		nTmpCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndexX,pTmpDataX,1);
	}
	else
	{
		nRowCntX = m_pDataInterface->GetColData(nColIndexX,pDataSetX,1);
		nTmpCntX = m_pDataInterface->GetColData(nColIndexX,pTmpDataX,1);
		nDataTyX = m_pDataInterface->GetFieldType(nColIndexX); 
	}
	//�Զ�����
	nDataTyD = fString;
	nColIndexD = m_tVarD.pValue[0];
	if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndexD))
	{
		nRowCntD = m_pDataInterface->GetTLDBColDataLabel(nColIndexD,pDataSetD,1);
		nTmpCntD = m_pDataInterface->GetTLDBColDataLabel(nColIndexD,pTmpDataD,1);
	}
	else
	{
		nRowCntD = m_pDataInterface->GetColData(nColIndexD,pDataSetD,1);
		nTmpCntD = m_pDataInterface->GetColData(nColIndexD,pTmpDataD,1);
		nDataTyD = m_pDataInterface->GetFieldType(nColIndexD); 
	}
	//����
	szFldNameX = m_pDataInterface->GetFieldName(nColIndexX);
	szFldNameD = m_pDataInterface->GetFieldName(nColIndexD);
	//Y��
	if(m_tVarY.iCount == 1)
	{
		nColIndexY = m_tVarY.pValue[0];
		nRowCntY   = m_pDataInterface->GetColData(nColIndexY,pDataSetY,1);
		szFldNameY = m_pDataInterface->GetFieldName(nColIndexY);
		pValue = new int[nRowCntY];
		assert(nRowCntY == nRowCntX);
		for (i=0; i<nRowCntY; i++)
		{
			if (pDataSetY[i].iIndex > 0) 
			{
				pValue[i] = 1;
				nRowCntValid ++;
			}
			else
			{
				pValue[i] = 0;
			}
		}
		if (nRowCntY == nRowCntValid)
		{
			delete []pValue;
			pValue = NULL;
		}
		else
		{	
			nRealIndex = 1;
			TDataSet *pDataSetX1 = new TDataSet[nRowCntValid];
			TDataSet *pDataSetD1 = new TDataSet[nRowCntValid];
			TDataSet *pDataSetY1 = new TDataSet[nRowCntValid];
			for (i=0; i<nRowCntY; i++)
			{
				if (pValue[i] > 0)
				{
					pDataSetX1[nRealIndex-1] = pDataSetX[i];
					pDataSetD1[nRealIndex-1] = pDataSetD[i];
					pDataSetY1[nRealIndex-1] = pDataSetY[i];
					pDataSetX1[nRealIndex-1].iIndex = (pDataSetX[i].iIndex > 0) ? nRealIndex : -nRealIndex;
					pDataSetD1[nRealIndex-1].iIndex = (pDataSetD[i].iIndex > 0) ? nRealIndex : -nRealIndex;
					pDataSetY1[nRealIndex-1].iIndex = nRealIndex;
					nRealIndex ++;
				}
			}
			delete []pValue;
			pValue = NULL;
			delete []pDataSetX;
			delete []pDataSetD;
			delete []pDataSetY;
			pDataSetX = pDataSetX1;
			pDataSetD = pDataSetD1;
			pDataSetY = pDataSetY1;
			nRowCntX = nRowCntValid;
			nRowCntY = nRowCntValid;
			nRowCntD = nRowCntValid;
		}	
	}
	//��������Ϣ
	TGroupInfor *pGroupInfoX = NULL;
	TGroupInfor *pGroupInfoD = NULL;
	int nGroupCntX = CStatistics::GetGroupingData(pDataSetX,nRowCntX,pGroupInfoX);						
	int nGroupCntD = CStatistics::GetGroupingData(pDataSetD,nRowCntD,pGroupInfoD);
	//�������
	if (nRowCntX<1 || nRowCntD<1)
	{
		if (pDataSetX != NULL) delete []pDataSetX;
		if (pTmpDataX != NULL) delete []pTmpDataX;
		if (pDataSetD != NULL) delete []pDataSetD;
		if (pTmpDataD != NULL) delete []pTmpDataD;
		if (pDataSetY != NULL) delete []pDataSetY;
		m_szErrMsg.Format("����������̫�٣��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (nGroupCntX>MAX_MULTI_INT || nGroupCntD>MAX_MULTI_INT)
	{
		if (pDataSetX != NULL) delete []pDataSetX;
		if (pTmpDataX != NULL) delete []pTmpDataX;
		if (pDataSetD != NULL) delete []pDataSetD;
		if (pTmpDataD != NULL) delete []pTmpDataD;
		if (pDataSetY != NULL) delete []pDataSetY;
		for (i=0; i<nGroupCntX; i++)
		{
			pGroupInfoX[i].DeleteObject();
		}
		delete []pGroupInfoX;
		
		for (i=0; i<nGroupCntD; i++)
		{
			pGroupInfoD[k].DeleteObject();
		}
		delete []pGroupInfoD;
		m_szErrMsg.Format("ֱ��ͼ�ڷ��������ݳ����� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_MULTI_INT,__LINE__,__FILE__);
		
		return false;
	}
	//ֱ��ͼ
	CTChartLine *pLine = new CTChartLine;
	CRsltElementChart *pChart = new CRsltElementChart("ֱ��ͼ",pLine);
	m_pResult->SetName("ֱ��ͼ"); 
	m_pResult->Add(pChart);
	szYAxisLabel = "�ܼ�";
	//����Ȩ��
	if (m_bWeight) m_pDataInterface->SetWeightNumb(m_nWeight);
	//��������
	for (i=0; i<nGroupCntD; i++)
	{				
		if (m_tVarY.iCount == 0) //���躯������
		{
			pDPSV = new TDataPointStrVal[nGroupCntX];
			for (j=0; j<nGroupCntX; j++)
			{
				int nYVal = 0;
				if (nDataTyX != fString)
				{
					if (pGroupInfoX[j].pIndex.pValue[0] > 0)
						szXVal.Format("%5.2f",pGroupInfoX[j].dValue);					
					else
						szXVal = "ȱʧ";
				}
				else
				{
					if (pGroupInfoX[j].pIndex.pValue[0] > 0)
						szXVal = pGroupInfoX[j].strVal;
					else
						szXVal = "ȱʧ";
				}
				if (nDataTyD != fString)
				{
					if (pGroupInfoD[i].pIndex.pValue[0] > 0)
						szDVal.Format("%5.2f",pGroupInfoD[i].dValue);
					else
						szDVal = "ȱʧ";
				}
				else
				{
					if (pGroupInfoD[i].pIndex.pValue[0] > 0)
						szDVal = pGroupInfoD[i].strVal;
					else
						szDVal = "ȱʧ";
				}
				int nTempIndex = 0;
				int nTotalWeight = 0;
				int nSizeD = pGroupInfoD[i].pIndex.iCount;
				if (m_bWeight)
				{					
					for (k=0; k<nSizeD; k++)
					{
						nTempIndex = pGroupInfoD[i].pIndex.pValue[k];
						nTotalWeight += m_pDataInterface->GetWeightData(nTempIndex);
					}					
				}
				for (k=0; k<nSizeD; k++)
				{
					nTempIndex = abs(pGroupInfoD[i].pIndex.pValue[k]);
					if (nDataTyX != fString)
					{
						if (pTmpDataX[nTempIndex-1].iIndex > 0)
							szYVal.Format("%5.2f", pTmpDataX[nTempIndex-1].dValue);
						else
							szYVal = "ȱʧ";
					}
					else
					{
						if (pTmpDataX[nTempIndex-1].iIndex > 0)
							szYVal = pTmpDataX[nTempIndex-1].strVal;
						else
							szYVal = "ȱʧ";
					}
					szXVal.TrimLeft();
					szYVal.TrimLeft();
					if (m_bWeight)
					{
						if (strcmp(szXVal,szYVal) == 0)	
							nYVal += m_pDataInterface->GetWeightData(nTempIndex);
					}
					else
					{
						if (strcmp(szXVal,szYVal) == 0)
							nYVal++;
					}
				}
				//��������
				switch (eData)
				{
				case REC:
					break;
				case RECSCALE:
					{
						if (m_bWeight)
						{
							if (nTotalWeight > 0)
								nYVal = nYVal*100/nTotalWeight;
							else
								nYVal = 0;
						}
						else
							nYVal = nYVal*100/nSizeD;
						szYAxisLabel = "�ٷֱ�";
					}
					break;
				case RECTOTAL:
					{
						nSum += nYVal;
						nYVal = nSum;
						szYAxisLabel = "�ۼ�����";
					}
					break;
				case RECTOTALSCALE:
					{
						nSum += nYVal;
						nYVal = nSum;
						nYVal = (nYVal*100)/nSizeD;
						szYAxisLabel = "�ۼưٷֱ�";
					}
					break;
				case 4:
					break;
				}
				pDPSV[j].fXVal = szXVal;
				pDPSV[j].fYVal = nYVal;
			}
			//�����
			pLine->SetLine(pDPSV,nGroupCntX,StrVal,szDVal);		
			delete []pDPSV;
		}
		else
		{
			TIntStruct tGroup;
			double *pdblVal  = NULL;
			CTString *pszVal = NULL;
			CPtrArray arrVal;
			CPtrArray arrInd;
			int nSubGroup  = 0;
			int nTempIndex = 0;
			int nSizeD = pGroupInfoD[i].pIndex.iCount;
			//����
			if (nDataTyX != fString)
			{
				pdblVal  = new double[nSizeD];
				for (k=0; k<nSizeD; k++)
				{	 
					nTempIndex = abs(pGroupInfoD[i].pIndex.pValue[k]);
					pdblVal[k] = pTmpDataX[nTempIndex-1].dValue;				 
				}
				QuickSort(pdblVal,nSizeD);
			}
			else
			{
				pszVal = new CTString[nSizeD];
				for (k=0; k<nSizeD; k++)
				{
					nTempIndex = pGroupInfoD[i].pIndex.pValue[k];
					pszVal[k] = pTmpDataX[nTempIndex-1].strVal;				  				
				}			
				QuickSort(pszVal,nSizeD);
			}
			if (nDataTyX != fString)
			{
				double *p = &pdblVal[0];
				arrVal.Add(p);
				for (k=0; k<nSizeD-1; k++)
				{
					if (pdblVal[k] != pdblVal[k+1])
						arrVal.Add(&pdblVal[k+1]);
				}
			}
			else
			{
				CTString *p = &pszVal[0];
				arrVal.Add(p);
				for (k=0; k<nSizeD-1; k++)
				{
					if (strcmp(pszVal[k], pszVal[k+1]))
						arrVal.Add(&pszVal[k+1]);
				}
			}
			//����
			nSubGroup = arrVal.GetSize();
			int *pIntTmp      = NULL;
			double *pdblTmp  = NULL;
			CTString* pszTmp = NULL;
			pDPSV = new TDataPointStrVal[nSubGroup];
			CTString szTemp("");
			CTString szGroupVal("");
			CTString szTempData("");
			CTString szGroupInfo("");
			bool bSameType = false ;
			for (j=0; j<nSubGroup; j++)
			{
				if (nDataTyX != fString)
				{
					pdblTmp = (double*)arrVal.GetAt(j);
				}
				else
				{
					pszTmp =(CTString*)arrVal.GetAt(j);
				}
				
				for (k=0; k<nGroupCntX; k++)
				{
					bSameType = false;
					if (nDataTyX != fString)
					{
						szGroupVal.Format("%5.10f",pGroupInfoX[k].dValue);
						szTemp.Format("%5.10f",*pdblTmp);
						if (strcmp(szGroupVal,szTemp) == 0)
							bSameType = true;
					}					  
					else
					{
						szGroupVal = pGroupInfoX[k].strVal;
						if (strcmp(szGroupVal, *pszTmp)==0)
							bSameType = true;
					}
					
					if (bSameType)
					{
						int nSizeX = pGroupInfoX[k].pIndex.iCount;
						for (l=0; l<nSizeX; l++)
						{
							int nValX = pGroupInfoX[k].pIndex.pValue[l];
							if (nDataTyD != fString)
							{
								szTempData.Format("%5.10f",pTmpDataD[nValX-1].dValue);
								szGroupInfo.Format("%5.10f",pGroupInfoD[i].dValue);					
							}
							else
							{
								szTempData  = pTmpDataD[nValX-1].strVal;
								szGroupInfo = pGroupInfoD[i].strVal;
							}
							if (strcmp(szTempData,szGroupInfo) == 0)
							{						  
								arrInd.Add(&pGroupInfoX[k].pIndex.pValue[l]);
							}
						}
					}
				}
				//Index
				int nSizeInd = arrInd.GetSize();
				pIntTmp = new int[nSizeInd];
				for (l=0; l<nSizeInd; l++)
				{
					pIntTmp[l] = *(int*)arrInd.GetAt(l);				 
				}
				tGroup.pValue = pIntTmp;
				tGroup.iCount = nSizeInd;
				nDataCntY = CStatistics::GetGroupData(pDataSetY,nRowCntY,tGroup,pDataSetYGroup);
				if (nDataCntY < 1)
				{
					pDPSV[j].fYVal = 0.0;
				}
				else
				{
					statistics.SetData(pDataSetYGroup,nDataCntY);
					if (!GetStatistical(statistics,eData,nDataCntY,pDPSV[j].fYVal))
					{
						for (i=0; i<nGroupCntX; i++)
						{
							pGroupInfoX[k].DeleteObject();
						}
						for (i=0; i<nGroupCntD; i++)
						{
							pGroupInfoD[k].DeleteObject();
						}
						if (pDataSetX != NULL) delete []pDataSetX;
						if (pTmpDataX != NULL) delete []pTmpDataX;
						if (pDataSetY != NULL) delete []pDataSetY;
						if (pDataSetD != NULL) delete []pDataSetD;
						if (pTmpDataD != NULL) delete []pTmpDataD;
						if (pGroupInfoX != NULL) delete []pGroupInfoX;
						if (pGroupInfoD != NULL) delete []pGroupInfoD;
						if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
						if (pDPSV   != NULL) delete []pDPSV;
						if (pIntTmp != NULL) delete []pIntTmp;
						return false;
					}
					if (!GetAxisLabel(eData,szYAxisLabel))
					{
						for (i=0; i<nGroupCntX; i++)
						{
							pGroupInfoX[k].DeleteObject();
						}
						for (i=0; i<nGroupCntD; i++)
						{
							pGroupInfoD[k].DeleteObject();
						}
						if (pDataSetX != NULL) delete []pDataSetX;
						if (pTmpDataX != NULL) delete []pTmpDataX;
						if (pDataSetY != NULL) delete []pDataSetY;
						if (pDataSetD != NULL) delete []pDataSetD;
						if (pTmpDataD != NULL) delete []pTmpDataD;
						if (pGroupInfoX != NULL) delete []pGroupInfoX;
						if (pGroupInfoD != NULL) delete []pGroupInfoD;
						if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
						if (pDPSV   != NULL) delete []pDPSV;
						if (pIntTmp != NULL) delete []pIntTmp;
						return false;
					}
				}
				if (!nDataCntY)
				{
					pDPSV[j].fXVal = "ȱʧ";
				}
				else if (nDataTyX != fString)
				{
					if(nDataTyX == fInt)
						pDPSV[j].fXVal.Format("%5.0f",*pdblTmp);
					else
						pDPSV[j].fXVal.Format("%5.2f",*pdblTmp);
				}
				else
				{
					pDPSV[j].fXVal = *pszTmp;
				}
				if (pIntTmp != NULL) delete []pIntTmp;
				arrInd.RemoveAll();
			}
			//Y-Label
			szYAxisLabel += " " + szFldNameY;	
			if (nDataTyD != fString)
			{
				if (pGroupInfoD[i].pIndex.pValue[0] > 0)
					szDVal.Format("%5.2f",pGroupInfoD[i].dValue);
				else
					szDVal = "ȱʧ";
			}
			else
			{
				if (pGroupInfoD[i].pIndex.pValue[0] > 0)
					szDVal = pGroupInfoD[i].strVal;
				else
					szDVal = "ȱʧ";
			}
			//�����
			pLine->SetLine(pDPSV,nSubGroup,StrVal,szDVal);
			arrVal.RemoveAll();
			delete []pdblVal;
			if (pDPSV   != NULL) delete []pDPSV;
		} 			
	}	
	//�ͷ��ڴ�
	for (i=0; i<nGroupCntX; i++)
	{
		pGroupInfoX[i].DeleteObject();
	}
	for (i=0; i<nGroupCntD; i++)
	{
		pGroupInfoD[i].DeleteObject();
	}
	if (pDataSetX != NULL) delete []pDataSetX;
	if (pTmpDataX != NULL) delete []pTmpDataX;
	if (pDataSetY != NULL) delete []pDataSetY;
	if (pDataSetD != NULL) delete []pDataSetD;
	if (pTmpDataD != NULL) delete []pTmpDataD;
	if (pGroupInfoX != NULL) delete []pGroupInfoX;
	if (pGroupInfoD != NULL) delete []pGroupInfoD;
	if (pDataSetYGroup != NULL) delete []pDataSetYGroup;
	//���ʶ
	pLine->SetXAxilLabel(szFldNameX);
	pLine->SetYAxilLabel(szYAxisLabel);
	return true;
}

bool CLine::CalcMultipleLine2()
{
	int i=0, j=0, k=0, nFldCnt=0;
	int nDataTyX=0, nColIndexX=0, nRowCntX=0;
	int nDataTyY=0, nColIndexY=0, nRowCntY=0;
	int nRealIndex=0, nRealValidCnt=0, nDataCntY=0;
	TDataSet *pDataSetX = NULL;
	TDataSet *pDataSetY = NULL;
	TDataSet *pDataSetGroup = NULL;
	CTChartLine *pLine = NULL;
	CRsltElementChart *pChart = NULL;
	TDataPointStrVal *pDPSV = NULL;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	CTString szYAxisLabel("");
	CStatistics statistics;
	int *pValue = NULL;
	EData eData;

	if (m_tVarX.iCount != 1)
	{
		m_szErrMsg.Format("ֱ��ͼ��X��ı���δ��ָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (m_tVarY.iCount < 2)
	{
		m_szErrMsg.Format("ֱ��ͼ��Y��ı��������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//����
	nDataTyX   = fString;
	nColIndexX = m_tVarX.pValue[0];
	szFldNameX = m_pDataInterface->GetFieldName(nColIndexX);
	if(m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndexX))
	{
		nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndexX,pDataSetX,1);
	}
	else
	{
		nRowCntX = m_pDataInterface->GetColData(nColIndexX,pDataSetX,1);
		nDataTyX = m_pDataInterface->GetFieldType(nColIndexX);
	}
	//������
	nFldCnt = m_pDataInterface->GetRowCount();
	pValue  = new int[nFldCnt];
	//��ʼ����1(��ʾû��ȱʧ)
	for (i=0; i<nFldCnt; i++)
		pValue[i] = 1;
	nRealValidCnt = nFldCnt;
	//�������е�������
	for (i=0; i<m_tVarY.iCount; i++)
	{
		nColIndexY = m_tVarY.pValue[i];
		nRowCntY = m_pDataInterface->GetColData(nColIndexY,pDataSetY,1);
		for (j=0; j<nRowCntY; j++)
		{
			if (pDataSetY[j].iIndex < 0) 
			{
				if (pValue[j] == 1) nRealValidCnt --;
				pValue[j] = 0;
			}
		}
		delete []pDataSetY;
		pDataSetY = NULL;
	}
	//��֤����
	if (nRealValidCnt < 1) 
	{		
		delete []pDataSetX;
		pDataSetX = NULL;
		delete []pValue;
		m_szErrMsg.Format("ֱ��ͼ�е���������Ϊ0����ȱʧֵ̫�࣬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (nRealValidCnt != nRowCntY)
	{
		m_szErrMsg.Format("ע�����ڴ���ȱʧֵ���� %d ����¼��ɾ�������飡Line=%d,File=%s",nRealValidCnt-nRowCntY,__LINE__,__FILE__);
		CRsltElementText *pElementText = new CRsltElementText("����");
		pElementText->AddString(m_szErrMsg);	
		m_pResult->Add(pElementText);
	}
	//׼������
	nRealIndex = 1;
	TDataSet *pTempData = new TDataSet[nRealValidCnt];
	for (i=0; i<nRowCntX; i++)
	{
		if (pValue[i] > 0)
		{
			pTempData[nRealIndex-1] = pDataSetX[i];
			nRealIndex ++;
		}
	}
	delete []pDataSetX;
	nRowCntX  = nRealValidCnt;
	pDataSetX = pTempData;
	//����
	TGroupInfor* pGroupInfo = NULL;
	int nGroupCount = CStatistics::GetGroupingData(pDataSetX,nRowCntX,pGroupInfo);
	if (nGroupCount <= 1)
	{
		m_szErrMsg.Format("ֱ��ͼ�ڷ��������̫�٣��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		delete []pDataSetX;
		delete []pValue;

		for (j=0; j<nGroupCount; j++)
		{
			pGroupInfo[j].DeleteObject();
		}
		delete []pGroupInfo;
		return false;
	}
	//ͼ������
	pLine  = new CTChartLine;
	pChart = new CRsltElementChart("ֱ��ͼ",pLine);
	m_pResult->SetName("ֱ��ͼ");
	m_pResult->Add(pChart);
	//�Ƿ������ͬ�ĺ�������
	bool bExistSameFunc = IsSameCallFunc(YAXIS_INT);
	for (i=0; i<m_tVarY.iCount; i++)
	{	
		nColIndexY = m_tVarY.pValue[i];
		if (pDataSetY != NULL) delete []pDataSetY;
		pDataSetY = NULL;
		
		nRowCntY   = m_pDataInterface->GetColData(nColIndexY,pDataSetY,1);
		szFldNameY = m_pDataInterface->GetFieldName(nColIndexY); 
		nDataTyY = m_pDataInterface->GetFieldType(nColIndexY); 
		pDPSV = new TDataPointStrVal[nRowCntY];
		//����ȱʧ��
		nRealIndex = 1;
		pTempData = new TDataSet[nRealValidCnt];
		for (j=0; j<nRowCntY; j++)
		{
			if (pValue[j] > 0)
			{
				pTempData[nRealIndex-1] = pDataSetY[j];
				nRealIndex ++;
			}
		}
		delete []pDataSetY;
		pDataSetY = pTempData;
		nRowCntY  = nRealIndex;	
		//�������������
		if (!GetDataType(i,YAXIS_INT,eData))
		{
			delete []pDPSV;
			delete []pValue;
			delete []pDataSetX;
			delete []pDataSetY;
			for (j=0; j<nGroupCount; j++)
			{
				pGroupInfo[j].DeleteObject();
			}
			delete []pGroupInfo;
			return false;
		}
		if (nDataTyY != fString)
		{
			for (j=0; j<nGroupCount; j++)
			{			
				if (nDataTyX != fString)
				{
					if (pGroupInfo[j].pIndex.pValue[0] > 0)
						szTemp.Format("%5.1f",pGroupInfo[j].dValue);
					else
						szTemp = "ȱʧ";
				}
				else
				{
					if (pGroupInfo[j].pIndex.pValue[0] > 0)
						szTemp = pGroupInfo[j].strVal;
					else
						szTemp = "ȱʧ";
				}
				szTemp.TrimLeft();
				//Call Func.
				nDataCntY = CStatistics::GetGroupData(pDataSetY,nRowCntY,pGroupInfo[j].pIndex,pDataSetGroup);
				statistics.SetData(pDataSetGroup,nDataCntY);
				if (!GetStatistical(statistics,eData,nDataCntY,pDPSV[j].fYVal))
				{
					for (k=0; k<nGroupCount; k++)
					{
						pGroupInfo[k].DeleteObject();
					}
					delete []pDPSV;
					delete []pValue;
					delete []pDataSetX;
					delete []pDataSetY;
					return false;
				}
				if (!GetAxisLabel(eData,szYAxisLabel))
				{
					for (k=0; k<nGroupCount; k++)
					{
						pGroupInfo[k].DeleteObject();
					}
					delete []pDPSV;
					delete []pValue;
					delete []pDataSetX;
					delete []pDataSetY;
					return false;
				}				
				pDPSV[j].fXVal = szTemp;
				delete []pDataSetGroup;
			}
		}
		//�Ƿ������ͬ�ĺ���
		if (!bExistSameFunc)
			szFldNameY = szYAxisLabel + " " + szFldNameY.GetData();
		pLine->SetLine(pDPSV,nGroupCount,StrVal,szFldNameY);
		delete []pDPSV;
	}
	//�ͷ��ڴ�	
	delete []pValue;
	delete []pDataSetX;
	delete []pDataSetY;
	for (i=0; i<nGroupCount; i++)
	{
		pGroupInfo[i].DeleteObject();
	}
	delete []pGroupInfo;
	//��ӵ������
	pLine->SetXAxilLabel(szFldNameX);
	pLine->SetYAxilLabel(szYAxisLabel);
	return true;
}

bool CLine::CalcMultipleLine3()
{
	int i=0, j=0, nTemp=0;
	int nColIndexX=0, nRowCntX=0, nDataTyX=0;
	int nColIndexY=0, nRowCntY=0, nDataTyY=0;
	TDataSet *pDataSetX = NULL;
	TDataSet *pDataSetY = NULL;
	TDataSet *pTempData = NULL;
	CTChartLine* pLine = NULL;
	CRsltElementChart* pChart = NULL;
	CTString  szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	TDataPointStrVal *pDPSV = NULL;
	
	int nVarCntY = m_tVarY.iCount;
	if (nVarCntY < 2)
	{
		m_szErrMsg.Format("ֱ��ͼ�����ݱ��������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (m_tVarX.iCount > 1)
	{
		m_szErrMsg.Format("ֱ��ͼ��X�����󶨵ı�������һ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	double *pdblCntY = new double[nVarCntY];
	for (i=0; i<nVarCntY; i++)
	{
		nColIndexY = m_tVarY.pValue[i]; 
		pdblCntY[i] = m_pDataInterface->GetColData(nColIndexY,pTempData,1);
		if (pTempData != NULL )
		{
			delete []pTempData;
			pTempData = NULL;
		}
	}
	//����
	QuickSort(pdblCntY,nVarCntY,false);	
	int nItemCount = (int)pdblCntY[0];
	nDataTyX = fString;
	if (m_tVarX.iCount)
	{
		nColIndexX = m_tVarX.pValue[0];
		szFldNameX = m_pDataInterface->GetFieldName(nColIndexX);
		if(m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndexX))
		{
			nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndexX,pDataSetX,1);
		}
		else
		{
			nRowCntX = m_pDataInterface->GetColData(nColIndexX,pDataSetX,1);
			nDataTyX = m_pDataInterface->GetFieldType(nColIndexX);
		}
	}
	nItemCount = nItemCount>nRowCntX ? nItemCount : nRowCntX;
	//ͼ��
	pLine = new CTChartLine;
	pChart = new CRsltElementChart("ֱ��ͼ",pLine);
	m_pResult->SetName("ֱ��ͼ"); 
	m_pResult->Add(pChart);
	//���ݴ���
	for (i=0; i<nVarCntY; i++)
	{
		nColIndexY = m_tVarY.pValue[i]; 
		nRowCntY   = m_pDataInterface->GetColData(nColIndexY,pDataSetY,1);
		szFldNameY = m_pDataInterface->GetFieldName(nColIndexY);
		nDataTyY = m_pDataInterface->GetFieldType(nColIndexY);
		pDPSV = new TDataPointStrVal[nItemCount];
		//���Բ�ֵ
		CStatistics::LinearInsert(pDataSetY, nRowCntY);
		if (m_tVarX.iCount == 1)
		{
			for (j=0; j<nRowCntX; j++)
			{
				if (nDataTyX != fString)		
				{
					if (pDataSetX[j].iIndex > 0)
						szTemp.Format("%5.1f",pDataSetX[j].dValue);
					else
						szTemp = "ȱʧ";
				}
				else
				{
					if (pDataSetX[j].iIndex > 0)
						szTemp = pDataSetX[j].strVal;
					else
						szTemp = "ȱʧ";
				}
				pDPSV[j].fXVal = szTemp;
			}	
		}
		//Y������
		nTemp = 0;
		for (j=0; j<nRowCntY; j++)
		{
			if (m_tVarX.iCount == 0)
			{
				nTemp ++;
				szTemp.Format("%d",nTemp);
				szTemp.TrimLeft();
				pDPSV[j].fXVal = szTemp;
			}
			pDPSV[j].fYVal = pDataSetY[j].dValue;
		}
		//�����
		pLine->SetLine(pDPSV,nRowCntY,StrVal,szFldNameY);
		delete []pDPSV;
		delete []pDataSetY;
	}
	//�ڴ��ͷ�
	delete []pdblCntY;
	if (pDataSetX != NULL) delete []pDataSetX;
	pLine->SetXAxilLabel(szFldNameX);
	pLine->SetYAxilLabel("ֵ");
	return true;
}