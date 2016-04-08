// Box.cpp: implementation of the CBox class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Box.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBox::CBox()
{
	m_szErrMsg = "";
}

CBox::~CBox()
{

}

bool CBox::Verify()
{
	int i = 0;
	int nColIndex = 0, nDataType = 0;
	CTString szFldName("");

	if (m_eMode == CLASS)
	{
		if (m_tVarX.iCount != 1)
		{
			m_szErrMsg.Format("��״ͼ�е����������û��ָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (m_tVarY.iCount != 1)
		{
			m_szErrMsg.Format("��״ͼ�е�ֵ��������û��ָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
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
	else if (m_eMode == VARI)
	{
		if (m_tVarX.iCount < 1)
		{
			m_szErrMsg.Format("��״ͼ�е����������û��ָ�����޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
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
	else
	{
		return false;
	}
	return true;
}

CResult *CBox::OnChart(CDataInterface *pDataInterface, string VO)
{
	bool bRet = false;
	m_pDataInterface = pDataInterface;
	//ͼ��
	SetChartType(CHART_BOXPLOT);
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
	if (m_eMode == CLASS) bRet = CalcBox1();
	else if (m_eMode == VARI) bRet = CalcBox2();
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

bool CBox::GetValueFromVo(string VO)
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

bool CBox::ParseTypeMode(string VO)
{
	int nSize=0;
	CTString szTemp;
	CTStringArray szArrName;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	//�����������ͣ�ֻ��һ�ֺ�״ͼ
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

bool CBox::CalcBox1()
{
	int i=0, j=0;
	int nID=0, nWeight=0, nTempCnt=0;
	int nColIndX=0, nDataTyX=0, nRowCntX=0;
	int nColIndY=0, nDataTyY=0, nRowCntY=0;
	CTString szTemp("");
	CTString szFldNameX("");
	CTString szFldNameY("");
	TDataSet *pDataSetX=NULL;
	TDataSet *pDataSetY=NULL;
	TDataSet *pTmpDataX=NULL;
	TDataSet *pTmpDataY=NULL;
	TDataSet *pDataSetGroupY=NULL;
	TGroupInfor *pGroupInforX=NULL;
	TStatBoxPlotS tStatPlotS;
	CTChartBoxplot *pCB=NULL;
	CRsltElementChart *pREC=NULL;
	CStatistics statistics;
	
	if (m_tVarX.iCount != 1) //����
	{
		m_szErrMsg.Format("��״ͼ��û��ָ������������޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	if (m_tVarY.iCount != 1) //����
	{
		m_szErrMsg.Format("��״ͼ��û��ָ���������ݣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	//����Ȩ��
	if (m_bWeight) m_pDataInterface->SetWeightNumb(m_nWeight);
	//ȡ�����ӱ���������
	nDataTyX = fString;
	nColIndX = m_tVarX.pValue[0];
	szFldNameX = m_pDataInterface->GetFieldName(nColIndX);
	if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndX))
	{
		nRowCntX = m_pDataInterface->GetTLDBColDataLabel(nColIndX,pTmpDataX,1);
	}
	else
	{
		nRowCntX = m_pDataInterface->GetColData(nColIndX,pTmpDataX,1);
		nDataTyX = m_pDataInterface->GetFieldType(nColIndX); 
	}
	//ȡ����������
	nDataTyY = fString;
	nColIndY = m_tVarY.pValue[0];
	szFldNameY = m_pDataInterface->GetFieldName(nColIndY);
	if (m_pDataInterface->GetShowValueLabel() && m_pDataInterface->bHaveTLDBFieldVL(nColIndY))
	{
		nRowCntY = m_pDataInterface->GetTLDBColDataLabel(nColIndY,pTmpDataY,1);
	}
	else
	{
		nRowCntY = m_pDataInterface->GetColData(nColIndY,pTmpDataY,1);
		nDataTyY = m_pDataInterface->GetFieldType(nColIndY); 
	}
	//��������
	int *pIndex = new int[nRowCntX];
	for (i=0; i<nRowCntX; i++)
	{
		if(pTmpDataY[i].iIndex > 0)
		{
			pIndex[i] = 1;
			nTempCnt++;
		}
		else
		{
			pIndex[i] = 0;
		}
	}
	//���
	pDataSetX = new TDataSet[nTempCnt];
	pDataSetY = new TDataSet[nTempCnt];
	for(i=0; i<nRowCntX; i++)
	{
		if (pIndex[i] == 1)
		{
			pDataSetX[nID].iIndex = pTmpDataX[i].iIndex;
			pDataSetX[nID].strVal = pTmpDataX[i].strVal;
			pDataSetX[nID].dValue = pTmpDataX[i].dValue;
			
			pDataSetY[nID].iIndex = pTmpDataY[i].iIndex;
			pDataSetY[nID].strVal = pTmpDataY[i].strVal;
			pDataSetY[nID].dValue = pTmpDataY[i].dValue;
			
			nID++;
		}
	}
	nRowCntX = nTempCnt;
	nRowCntY = nTempCnt;
	delete []pTmpDataX;
	delete []pTmpDataY;
	//���鴦��
	int nGroupCntX = CStatistics::GetGroupingData(pDataSetX,nRowCntX,pGroupInforX);
	if (nTempCnt < 1 || nGroupCntX < 1)
	{
		m_szErrMsg.Format("��״ͼ�ڷ��������̫�٣��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
		delete []pIndex;
		delete []pDataSetX;
		delete []pDataSetY;
		for (i=0; i<nGroupCntX; i++)
		{
			pGroupInforX->DeleteObject();
		}
		if (pGroupInforX) delete []pGroupInforX;
		return false;
	}
	if (nGroupCntX > MAX_VALID_INT/15)
	{
		m_szErrMsg.Format("��״ͼ�ڷ�������ݳ��� %d �飬�޷���ͼ�����飡Line=%d,File=%s",MAX_VALID_INT/15,__LINE__,__FILE__);
		delete []pIndex;
		delete []pDataSetX;
		delete []pDataSetY;
		for (i=0; i<nGroupCntX; i++)
		{
			pGroupInforX->DeleteObject();
		}
		if (pGroupInforX) delete []pGroupInforX;
		return false;
	}
	//����һ�����ӱ����Ľ������
	tStatPlotS = TStatBoxPlotS(nGroupCntX);
	for (i=0; i<nGroupCntX; i++)
	{
		nWeight = nTempCnt = 0;
		int nSubCnt = pGroupInforX[i].pIndex.iCount;
		if (nSubCnt < 1) continue;
		if (nDataTyX != fString)	
		{
			if (pGroupInforX[i].pIndex.pValue[0] > 0)
				szTemp.Format("%.2f",pGroupInforX[i].dValue);
			else
				szTemp = "ȱʧ";
		}						
		else
		{
			if (pGroupInforX[i].pIndex.pValue[0] > 0)
				szTemp = pGroupInforX[i].strVal;
			else
				szTemp = "ȱʧ";
		}
		//Y��
		int nDataCntY = CStatistics::GetGroupData(pDataSetY,nRowCntY,pGroupInforX[i].pIndex,pDataSetGroupY);
		//Ȩ��
		if (m_bWeight)
		{							
			for (j=0; j<nDataCntY; j++)
			{
				nID = pDataSetGroupY[j].iIndex;
				nWeight += 	m_pDataInterface->GetWeightData(nID);	
			}
		}
		//��״ͼ����
		TStatBoxPlot tStatPlot;
		statistics.SetData(pDataSetGroupY,nDataCntY);
		if (m_bWeight)
			tStatPlot.nCount = nWeight;
		else
			tStatPlot.nCount = nDataCntY;
		TPlotReco pr;
		tStatPlot.pPlotVal.nCount = statistics.f_plotval(tStatPlot.pPlotVal.dValue,false,pr);
		tStatPlot.strName = szTemp;
		tStatPlotS.pStatPlot[i] = tStatPlot;	
		//�ڴ��ͷ�
		if (pDataSetGroupY)
		{
			delete []pDataSetGroupY;
			pDataSetGroupY = NULL;
		}
	}
	//���״ͼ	
	TDataPointStrArr *pDPFB = NULL;
	tStatPlotS.strName = szFldNameX;
	tStatPlotS.tPlotReco = statistics.GetCoordinate(pDataSetY,nRowCntY);
	CTChartBoxplot *pBox = new CTChartBoxplot;
	for (i=0; i<tStatPlotS.nStatPlot; i++)
	{
		pDPFB = new tagDataPointStrArr;
		szTemp.Format("%d",tStatPlotS.pStatPlot[i].nCount);
		pDPFB->strName = szTemp;
		pDPFB->strName += SEP_RISER + tStatPlotS.pStatPlot[i].strName;
		for (j=0; j<tStatPlotS.pStatPlot[i].pPlotVal.nCount; j++)
			pDPFB->arrYVal.Add(tStatPlotS.pStatPlot[i].pPlotVal.dValue[j]);
		//Add box
		pBox->SetBoxplot(pDPFB,1,StrArr);
		delete pDPFB;
	}	
	//����
	pBox->SetXAxilLabel(szFldNameX);
	pBox->SetYAxilLabel(szFldNameY);
	CRsltElementChart *pChart = new CRsltElementChart("��״ͼ",pBox);
	m_pResult->SetName("��״ͼ"); 
	m_pResult->Add(pChart); 
	//�ͷ��ڴ�
	if (pDataSetX != NULL)
	{
		delete []pDataSetX;
		pDataSetX = NULL;
	}
	if (pDataSetY != NULL)
	{
		delete []pDataSetY;
		pDataSetY = NULL;
	}
	if (pGroupInforX != NULL)
	{
		for (i=0; i<nGroupCntX; i++)
		{	
			tStatPlotS.pStatPlot[i].DeleteObject();
			pGroupInforX[i].DeleteObject();				
		}
		delete []pGroupInforX;
		pGroupInforX=NULL;
	}
	delete []pIndex;
	tStatPlotS.DeleteObject();
	return true;
}

bool CBox::CalcBox2()
{
	int i=0, nCount=0;
	int nDataType = 0, nColIndex=0, nRowCount=0;
	CDWordArray arrCol;
	CDoubleMatrix dataMatrix;
	CTString szTemp("");
	CTString szFldName("");
	bool bHasBox=false;
	CTStringArray arrFldName;
	
	if (m_tVarX.iCount < 1)
	{
		m_szErrMsg.Format("��״ͼ�е����ݲ�������������Ҫһ�����������飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	m_szErrMsg.Empty();
	//���ݴ���
	TDataPointStrArr *pDPFB = NULL;
	CTChartBoxplot *pBox = new CTChartBoxplot;
	if (m_bSplit) //�ֿ�
	{
		for (i=0; i<m_tVarX.iCount; i++)
		{
			nColIndex = m_tVarX.pValue[i];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			szFldName = m_pDataInterface->GetFieldName(nColIndex);
			if (nDataType != fInt && nDataType != fDouble && 
				nDataType != fBoolean && nDataType != fCurrency)
			{
				szTemp.Format("��״ͼ�е�X�����(%s)���������ͣ��޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
				m_szErrMsg += szTemp;
				continue;
			}
			arrCol.RemoveAll();
			dataMatrix.destroy();
			arrCol.Add(nColIndex);
			nRowCount = m_pDataInterface->GetColsData(arrCol,dataMatrix,0);
			if (nRowCount < 1)
			{
				szTemp.Format("��״ͼ�е�X�����(%s)��Ч����̫�٣��޷���ͼ�����飡Line=%d,File=%s",szFldName.GetData(),__LINE__,__FILE__);
				m_szErrMsg += szTemp;
				continue;
			}
			//�����״ͼ
			CIntVector vecInd;
			CDoubleVector w;
			CDoubleVector v = dataMatrix(0);
			pBox->SetParam(false,false,false);
			pBox->CalcPercent(v,w,vecInd);
			pBox->CalcBoxPlot(v,(void **)&pDPFB,nCount);
			pDPFB->strName += SEP_RISER + szFldName;
			pBox->SetBoxplot(pDPFB,1,StrArr);
			delete pDPFB;
			bHasBox = true;
		}
	}
	else
	{
		for (i=0; i<m_tVarX.iCount; i++)
		{
			nColIndex = m_tVarX.pValue[i];
			nDataType = m_pDataInterface->GetFieldType(nColIndex);
			szFldName = m_pDataInterface->GetFieldName(nColIndex);
			if (nDataType != fInt && nDataType != fDouble && 
				nDataType != fBoolean && nDataType != fCurrency)
			{
				szTemp.Format("��״ͼ�е�X�����(%s)���������ͣ��޷���ͼ�����飡Line=%d,File=%s\n",szFldName.GetData(),__LINE__,__FILE__);
				m_szErrMsg += szTemp;
				continue;
			}
			arrCol.Add(nColIndex);
			arrFldName.Add(szFldName);
		}
		if (arrCol.GetSize() < 1)
		{
			m_szErrMsg.Format("��״ͼ�е�X�������Ϊ�������ͣ��޷���ͼ�����飡Line=%d,File=%s\n",__LINE__,__FILE__);
			return false;
		}
		//������
		nRowCount = m_pDataInterface->GetColsData(arrCol,dataMatrix,0);
		if (nRowCount < 1)
		{
			m_szErrMsg.Format("��״ͼ�е�X�������Ч����̫�٣��޷���ͼ�����飡Line=%d,File=%s\n",__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<arrCol.GetSize(); i++)
		{
			//�����״ͼ
			CIntVector vecInd;
			CDoubleVector w;
			CDoubleVector v = dataMatrix(i);
			pBox->SetParam(false,false,false);
			pBox->CalcPercent(v,w,vecInd);
			pBox->CalcBoxPlot(v,(void **)&pDPFB,nCount);
			pDPFB->strName += SEP_RISER + arrFldName.GetAt(i);
			pBox->SetBoxplot(pDPFB,1,StrArr);
			delete pDPFB;
			bHasBox = true;
		}
	}
	if (m_szErrMsg.GetLength() > 0)
	{
		CRsltElementText *pText = new CRsltElementText(MSG_TITLE_STR);
		CTString szMsg = GetLastErrMsg();
		pText->AddString(szMsg);
		m_pResult->Add(pText);
	}
	//Add
	if (bHasBox)
	{
		CRsltElementChart *pChart = new CRsltElementChart("��״ͼ",pBox);
		pBox->SetXAxilLabel("����");
		m_pResult->SetName("��״ͼ"); 
		m_pResult->Add(pChart); 
	}
	else
	{
		delete pBox;
		pBox = NULL;
	}
	return true;
}