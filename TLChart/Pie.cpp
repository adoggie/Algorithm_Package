// Pie.cpp: implementation of the CPie class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Pie.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPie::CPie()
{
	m_szErrMsg = "";
}

CPie::~CPie()
{

}

bool CPie::Verify()
{
	int i = 0;
	int nColIndex = 0, nDataType = 0;
	CTString szFldName("");

	if (m_eMode == CLASS)
	{
		if (m_tVarX.iCount != 1)
		{
			m_szErrMsg.Format("Բ��ͼ�е������δָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_nAVType & AXIS_DESC_INT)
		{
			m_szErrMsg.Format("Բ��ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if ((m_nAVType & VAR_DESC_INT  && m_tVarY.iCount == 1) ||
			(m_nAVType & VAR_NDESC_INT && m_tVarY.iCount != 1))
		{
			m_szErrMsg.Format("Բ��ͼ�е�ֵ������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_tVarY.iCount == 1)
		{
			nColIndex = m_tVarY.pValue[0];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			if (nDataType == fString)
			{ 
				m_szErrMsg.Format("Բ��ͼ�е�ֵ�����������Ͳ���Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
				return false;
			}
		}
	}
	else if (m_eMode == VARI)
	{
		if (m_nAVType & AXIS_DESC_INT)
		{
			m_szErrMsg.Format("Բ��ͼ�е�������������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_tVarX.iCount < 2)
		{
			m_szErrMsg.Format("Բ��ͼ�е�����������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_arrX.GetSize() < 1)
		{
			m_szErrMsg.Format("Բ��ͼ�е�ֵ��������û�ж��壬�޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<m_tVarX.iCount; i++)
		{
			nColIndex = m_tVarX.pValue[i];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			if (nDataType == fString)
			{ 
				szFldName = m_pDataInterface->GetFieldName(nColIndex);
				m_szErrMsg.Format("Բ��ͼ�е�������������Ͳ���Ϊ�ַ���-%s���޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
				return false;
			}
		}
	}
	else if (m_eMode == VALUE)
	{
		if (m_nAVType & VAR_DESC_INT)
		{
			m_szErrMsg.Format("Բ��ͼ�е�ֵ�����������Ͳ���Ϊ�����ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_tVarY.iCount != 1)
		{
			m_szErrMsg.Format("Բ��ͼ�е�ֵ����������Ҫָ��һ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if ((m_nAVType & AXIS_DESC_INT  && m_tVarX.iCount == 1) ||
			(m_nAVType & AXIS_NDESC_INT && m_tVarX.iCount != 1))
		{
			m_szErrMsg.Format("Բ��ͼ�е��������������ָ�������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		nColIndex = m_tVarY.pValue[0];
		nDataType = m_pDataInterface->GetFieldType(nColIndex);
		if (nDataType == fString)
		{ 
			m_szErrMsg.Format("Բ��ͼ�е�ֵ�����������Ͳ���Ϊ�ַ��ͣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}
 
CResult *CPie::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//ͼ��
	SetChartType(CHART_PIE);
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
	if (m_eMode == CLASS) bRet = CalcPie1();
	else if (m_eMode == VARI) bRet = CalcPie2();
	else if (m_eMode == VALUE) bRet = CalcPie3();
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

bool CPie::GetValueFromVo(string VO)
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

bool CPie::ParseTypeMode(string VO)
{
	int nSize=0;
	CTString szTemp;
	CTStringArray szArrName;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	//�����������ͣ�ֻ��һ��Բ��ͼ
	szArrName.RemoveAll();
	if (AnaSyntax::exist(MODE_STR,VOstr)) 
	{
		AnaSyntax::getVAR(MODE_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			m_szErrMsg.Format("Բ��ͼ�е��������Ͳ�ȷ�������飡Line=%d,File=%s",__LINE__,__FILE__);
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
			m_szErrMsg.Format("Բ��ͼ�е��������Ͳ����ڣ�%s�����飡Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	else
	{
		m_szErrMsg.Format("Բ��ͼ����������û�б�ָ�������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	return true;
}

bool CPie::CalcPie1()
{
	int i=0, j=0, k=0, nTmp=0;
	int nColIndX=0, nColIndY=0;
	int nRowCntX=0, nRowCntY=0;
	int nDataTyX=0, nDataTyY=0, nDataCntY=0;
	int nWeight=0, nTotalCount=0;
	double fTemp=0, fTotalCount=0, fTotalPercentEx=0;
	TDataSet *pDataSetX= NULL;
	TDataSet *pDataSetY= NULL;
	TDataSet *pDataSetYGroup=NULL;
	TGroupInfor *pGroupInfor=NULL;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	CTString szYAxisLabel("");
	CStatistics statistics;
	EData eData;
	
	//Y����������
	if (!GetDataType(0,YAXIS_INT,eData)) 
	{
		return false;
	}
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
	if (m_tVarY.iCount == 1) //���ڱ�����ʽ- ����ȱʧֵ
	{		
		nColIndY = m_tVarY.pValue[0];
		nRowCntY = m_pDataInterface->GetColData(nColIndY,pDataSetY,1);
		szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 
		//����ȱʧֵ
		int *pFlag = new int[nRowCntY];//���Դ�����־�Ƿ�ɾ������
		int nCalcCnt = 0;              //�洢ɾ����Ԫ�ظ���
		j=0;
		for (i=0; i<nRowCntY; i++)
		{
			if (pDataSetY[i].iIndex > 0)
			{
				pFlag[i] = 1;
				nCalcCnt ++;
			}
			else
			{
				pFlag[i] = 0;
			}
		}
		//��ʱ
		TDataSet *pDataSetTX = new TDataSet[nCalcCnt];
		TDataSet *pDataSetTY = new TDataSet[nCalcCnt]; 
		for (i=0; i<nRowCntY; i++)
		{
			if (pFlag[i] == 1)
			{
				pDataSetTX[j] = pDataSetX[i];//�ѿ�ֵ�ٵ�
				pDataSetTY[j] = pDataSetY[i];//�ѿ�ֵ�ٵ�
				if (pDataSetX[i].iIndex>0)
					pDataSetTX[j].iIndex = j+1;//�ı�����ֵ
				else
					pDataSetTX[j].iIndex = -1*(j+1);
				pDataSetTY[j].iIndex = j+1;
				j = j+1;
			}
		}
		nRowCntX = nCalcCnt;//�����³���
		delete []pDataSetX;
		delete []pDataSetY;
		delete []pFlag;
		pDataSetX = pDataSetTX;
		pDataSetY = pDataSetTY;
		pFlag = NULL;
	}
	//����Ч����
	if (nRowCntX == 0)
	{
		m_szErrMsg.Format("Բ��ͼ������Ч�ķ������ݣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		delete []pDataSetX;
		delete []pDataSetY;
		return false;
	}
	int nGroupCntX = CStatistics::GetGroupingData(pDataSetX,nRowCntX,pGroupInfor);
	if (nGroupCntX > MAX_VALID_INT)
	{
		m_szErrMsg.Format("Բ��ͼ�з������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT,__LINE__,__FILE__);
		delete []pDataSetX;
		delete []pDataSetY;
		for (i=0; i<nGroupCntX; i++)
		{
			pGroupInfor[i].DeleteObject();
		}
		delete []pGroupInfor;
		return false;
	}
	//׼������
	TDataPointValStr *pDPSV = new TDataPointValStr[nGroupCntX];
	for (i=0; i<nGroupCntX; i++)
	{
		if (nDataTyX != fString)
		{
			if (pGroupInfor[i].pIndex.pValue[0]>0)
			{
				szTemp.Format("%5.4f",pGroupInfor[i].dValue); 
				szTemp.TrimLeft();
			}
			else
			{
				szTemp = "ȱʧ"; 
			}
			pDPSV[i].fYVal = szTemp;
		}
		else
		{
			if (pGroupInfor[i].pIndex.pValue[0] > 0)
				pDPSV[i].fYVal = pGroupInfor[i].strVal ;
			else
				pDPSV[i].fYVal = "ȱʧ" ;
		}
		//Y����������
		switch (eData)
		{
		case REC:	
		case RECSCALE:
			pDPSV[i].fXVal = pGroupInfor[i].pIndex.iCount; 
			pDPSV[i].fXVal /= double(nRowCntX);
			if (i == nGroupCntX-1)
			{
				pDPSV[i].fXVal = 1-fTotalPercentEx; 
			}
			else
			{
				fTotalPercentEx += pDPSV[i].fXVal; 
			}
			break;
			
		case RECTOTAL:				
		case RECTOTALSCALE:					
			pDPSV[i].fXVal = pGroupInfor[i].pIndex.iCount + nTotalCount;
			fTotalCount += pDPSV[i].fXVal;
			nTotalCount += pGroupInfor[i].pIndex.iCount;
			break;
		default:
			{	
				nDataCntY = CStatistics::GetGroupData(pDataSetY,nRowCntY,pGroupInfor[i].pIndex ,pDataSetYGroup);
				statistics.SetData(pDataSetYGroup,nDataCntY);
				if (!GetStatistical(statistics,eData,nDataCntY,fTemp))
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
				//Y
				pDPSV[i].fYVal.Format("%5.4f",fTemp);
				fTotalCount += pDPSV[i].fXVal; 
				delete []pDataSetYGroup;
				pDataSetYGroup = NULL;
			}
			break;
		}
	}
	//����
	switch (eData)
	{
	case REC:
	case RECSCALE:
		break;
	default:
		{
			for(i=0; i<nGroupCntX; i++)
			{
				if (i == nGroupCntX-1)
					pDPSV[i].fXVal = 1-fTotalPercentEx ;
				else
				{
					pDPSV[i].fXVal /= fTotalCount;
					fTotalPercentEx += pDPSV[i].fXVal ;
				}
			}
		}
		break;
	}
	//ͼ��
	CTChartPie *pPie = new CTChartPie;
	CRsltElementChart *pChart = new CRsltElementChart("Բ��ͼ",pPie);
	pPie->SetPie(pDPSV,nGroupCntX,ValStr);
	m_pResult->SetName("Բ��ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	delete []pDPSV;
	delete []pDataSetX;
	delete []pDataSetY;
	for (i=0; i<nGroupCntX; i++)
	{
		pGroupInfor[i].DeleteObject();
	}
	delete []pGroupInfor;
	return true;
}

bool CPie::CalcPie2()
{
	int i=0, j=0, nCalcCnt=0;
	int nColIndex=0, nRowCount=0, nRealCount=0;
	double fSum=0,fSumTotal=0, fPercentTotal=0;
	CTString szLegendName("");
	CTString szYAxisLabel("");
	TDataSet* pDataSet=NULL;
	TDataSet *pTempDataSet=NULL;
	TDataPointStrVal* pDPSV=NULL;
	EData eData;
	CStatistics statistics;
	
	int nVarCount = m_tVarX.iCount; 
	if (nVarCount > MAX_VALID_INT/10)
	{
		m_szErrMsg.Format("Բ��ͼ�еķ������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT/10,__LINE__,__FILE__);	
		return false;
	}
	//�Ƿ������ͬ�ĺ�������
	bool bExistSameFunc = IsSameCallFunc(XAXIS_INT);
	//ȱʧֵ����
	nRowCount = m_pDataInterface->GetRowCount();
	int *pFlag = new int[nRowCount];
	for (i=0; i<nRowCount; i++)
		pFlag[i]=1;
	nCalcCnt=0;//�洢ɾ����Ԫ�ظ���
	for (i=0; i<nVarCount; i++)
	{
		nColIndex = m_tVarX.pValue[i]; 
		nRowCount = m_pDataInterface->GetColData(nColIndex,pDataSet,1);
		for (j=0; j<nRowCount; j++)
		{
			if (pFlag[j] == 1)
			{
				if (pDataSet[j].iIndex < 0)
					pFlag[j]=0;
			}
		}
		if (pDataSet != NULL)
		{
			delete []pDataSet;
			pDataSet = NULL;
		}
	}
	//��Ч��������
	for (i=0; i<nRowCount; i++)
	{	
		if (pFlag[i] == 1)
			nCalcCnt ++;
	}
	nRowCount = nCalcCnt;
	if (nRowCount == 0)
	{
		m_szErrMsg.Format("Բ��ͼ�е�����Ч�ķ������ݣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		delete []pFlag;
		return false;		
	}	
	//���ݴ���
	double *pfPercent = new double[nVarCount];
	TDataPointValStr* pDPVS = new TDataPointValStr[nVarCount];
	pTempDataSet = new TDataSet[nCalcCnt];//�����ʱ������
	for (i=0; i<nVarCount; i++)
	{
		nColIndex = m_tVarX.pValue[i]; 
		nCalcCnt  = m_pDataInterface->GetColData(nColIndex,pDataSet,1); 
		int k = 0;
		for (j=0;j<nCalcCnt; j++)
		{
			if (pFlag[j] == 1)
			{
				pTempDataSet[k] = pDataSet[j];//�ѿ�ֵ�ٵ�
				pTempDataSet[k].iIndex = k+1;
				k ++;
			}
		}
		delete []pDataSet;
		pDataSet = pTempDataSet;
		pDPVS[i].fYVal = m_pDataInterface->GetFieldName(nColIndex);
		//�������������
		if (!GetDataType(i,XAXIS_INT,eData))
		{
			delete []pFlag;
			delete []pfPercent;
			delete []pDPVS;
			delete []pTempDataSet;
			return false;
		}
		//��ֵ
		statistics.SetData(pDataSet,nRowCount);
		if (!GetStatistical(statistics,eData,nRowCount,fSum))
		{
			delete []pFlag;
			delete []pfPercent;
			delete []pDPVS;
			delete []pTempDataSet;
			return false;
		}
		if (!GetAxisLabel(eData,szYAxisLabel))
		{
			delete []pFlag;
			delete []pfPercent;
			delete []pDPVS;
			delete []pTempDataSet;
			return false;
		}
		pfPercent[i] = fSum;
		fSumTotal += fSum;
		if (szLegendName.IsEmpty()) 
			szLegendName = szYAxisLabel;
		else
			szLegendName = szLegendName + "\n" + szYAxisLabel.GetData();
	}
	for (i=0; i<nVarCount-1; i++)
	{
		pfPercent[i] = pfPercent[i]/fSumTotal;
		fPercentTotal += pfPercent[i];
		pDPVS[i].fXVal = pfPercent[i];
	}
	pfPercent[i] = 1-fPercentTotal;
	pDPVS[i].fXVal = pfPercent[i];
	//ͼ��	
	CTChartPie *pPie = new CTChartPie;
	CRsltElementChart *pChart = new CRsltElementChart("Բ��ͼ",pPie);
	pPie->SetPie(pDPVS,nVarCount,ValStr,szLegendName);
	m_pResult->SetName("Բ��ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	delete []pFlag;
	delete []pDPVS;
	delete []pfPercent;
	delete []pDataSet;
	return true;
}

bool CPie::CalcPie3()
{
	int i=0, j=0, nCalcCnt=0;
	int nRowCntX=0, nRowCntY=0;
	int nColIndX=0, nColIndY=0;
	int nDataTyX=0, nDataTyY=0;
	double fSum,fPercentTotal=0;
	TDataSet *pDataSetX = NULL;
	TDataSet *pDataSetY = NULL;
	TDataSet* pDataSetTX = NULL;
	TDataSet* pDataSetTY = NULL;
	CTString szTemp("");
	CTString szFldNameY("");
	CTString szFldNameX("");
	CStatistics statistics;
	
	//ȡY������(���m_nSel==0��X������ţ����m_nSel == 1��X������һ������)
	nColIndY = m_tVarY.pValue[0];
	nRowCntY = m_pDataInterface->GetColData(nColIndY,pDataSetY,1);
	nDataTyY = m_pDataInterface->GetFieldType(nColIndY); 
	szFldNameY = m_pDataInterface->GetFieldName(nColIndY); 
	if (nRowCntY > MAX_VALID_INT/10)
	{
		m_szErrMsg.Format("Բ��ͼ�еķ������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT/10,__LINE__,__FILE__);	
		if (pDataSetY != NULL) delete []pDataSetY;
		return false;
	}
	if (m_tVarX.iCount == 1)//������ʽ
	{
		nDataTyX = fString;
		nColIndX = m_tVarX.pValue[0];
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
	int *pFlag = new int[nRowCntY];//���Դ�����־�Ƿ�ɾ������
	nCalcCnt = 0;                  //�洢ɾ����Ԫ�ظ���
	for (i=0; i<nRowCntY; i++)
	{
		if (pDataSetY[i].iIndex>0)
		{
			pFlag[i] = 1;
			nCalcCnt ++;
		}
		else
		{
			pFlag[i] = 0;
		}
	}
	//��������
	j = 0;
	pDataSetTY = new TDataSet[nCalcCnt];
	if (m_tVarX.iCount == 1) //������ʽ
	{
		pDataSetTX = new TDataSet[nCalcCnt];
		for (i=0; i<nRowCntY; i++)
		{
			if (pFlag[i] == 1)
			{
				pDataSetTX[j] = pDataSetX[i];//�ѿ�ֵ�ٵ�
				pDataSetTY[j] = pDataSetY[i];//�ѿ�ֵ�ٵ�
				if (pDataSetX[i].iIndex>0)
					pDataSetTX[j].iIndex = j+1;//�ı�����ֵ
				else
					pDataSetTX[j].iIndex = -1*(j+1);
				pDataSetTY[j].iIndex=j+1;
				j = j+1;
			}	
		}
		delete []pDataSetY;
		delete []pDataSetX;
		pDataSetX = pDataSetTX;
		pDataSetY = pDataSetTY;
	}
	else
	{
		for (i=0; i<nRowCntY; i++)
		{
			if (pFlag[i] == 1)
			{
				pDataSetTY[j] = pDataSetY[i];//�ѿ�ֵ�ٵ�����ֵ����
				pDataSetTY[j].iIndex = i+1;
				j = j+1;
			}
		}
		delete []pDataSetY;
		pDataSetY = pDataSetTY;
	}
	//�ͷ��ڴ�
	delete []pFlag;
	pFlag = NULL;
	nRowCntY = nCalcCnt;
	if (nRowCntY == 0)
	{
		m_szErrMsg.Format("Բ��ͼ����������̫�٣��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		if (pDataSetY != NULL) delete []pDataSetY;
		if (pDataSetX != NULL) delete []pDataSetX;
		return false;
	}
	//���ݴ���
	statistics.SetData(pDataSetY,nRowCntY); 
	fSum = statistics.f_sum();
	TDataPointValStr *pDPVS = new TDataPointValStr[nRowCntY];
	for (i=0; i<nRowCntY-1; i++)
	{
		pDPVS[i].fXVal = pDataSetY[i].dValue/fSum;
		fPercentTotal += pDPVS[i].fXVal;
		if (m_tVarX.iCount == 1)
		{
			if (nDataTyX == fString) 
			{
				if (pDataSetX[i].iIndex > 0)
					pDPVS[i].fYVal = pDataSetX[i].strVal ;
				else
					pDPVS[i].fYVal = "ȱʧ";	
			}
			else
			{
				if(pDataSetX[i].iIndex > 0)
					pDPVS[i].fYVal.Format("%5.1f",pDataSetX[i].dValue);
				else
					pDPVS[i].fYVal="ȱʧ";
			}
		}
		else
		{
			pDPVS[i].fYVal.Format("%d",pDataSetY[i].iIndex);  
		}
	}
	//X
	pDPVS[i].fXVal = 1-fPercentTotal;
	if (m_tVarX.iCount == 1)
	{
		if (nDataTyX == fString)
		{
			if(pDataSetX[i].iIndex>0)
				pDPVS[i].fYVal =pDataSetX[i].strVal ;
			else
				pDPVS[i].fYVal ="ȱʧ";					
		}
		else
		{
			if(pDataSetX[i].iIndex>0)
				pDPVS[i].fYVal.Format("%5.4f",pDataSetX[i].dValue);
			else
				pDPVS[i].fYVal ="ȱʧ";
		}
	}
	else
	{
		pDPVS[i].fYVal.Format("%d",pDataSetY[i].iIndex);  
	}
	//ͼ��
	CTChartPie *pPie = new CTChartPie;
	CRsltElementChart *pChart = new CRsltElementChart("Բ��ͼ",pPie);
	pPie->SetPie(pDPVS,nRowCntY,ValStr);
	m_pResult->SetName("Բ��ͼ"); 
	m_pResult->Add(pChart);
	//�ͷ��ڴ�
	delete []pDPVS;
	if (pDataSetY != NULL) delete []pDataSetY;
	if (pDataSetX != NULL) delete []pDataSetX;
	return true;
}