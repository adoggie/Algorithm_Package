// Evaluate.cpp: implementation of the CEvaluate class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Evaluate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEvaluate::CEvaluate(CDecTree *pDecTree)
{
	m_pDecTree = pDecTree;
}

CEvaluate::~CEvaluate()
{

}

void CEvaluate::EV(CDoubleMatrix &m, CResult *pResult)
{
	double fSum = 0;
	int i=0, j=0, k=0, nSize=0, nRowIndex=0;
	CTString szValue("");//�ַ���
	CTArray<int, int>arrFreq;
	CDoubleVector  v;   
	CPtrArray arr;
	tagDataPointValVal *pDPVV1 = NULL;
	tagDataPointValVal *pDPVV2 = NULL;
	tagDataPointValVal *pDPVV3 = NULL;

	//����Ƶ��
	v = m.col(0);
	arrFreq = CalcFrequence(v);
	int n = arrFreq.GetSize();
	//�������ֵ
	CDoubleMatrix me(n,n);    
	CalcEvalue(m,arrFreq,me);
	//��񼰻�ͼ
	CTLTable *pTable = new CTLTable;
	pTable->SetTitle("������Ԥ��ȶԱ�");
	for (i=0; i<n+2; i++)
	{
		if (i==0)
		{
			szValue = "";
		}
		else if(i<n+1)
		{
			szValue = GetLabel(arrFreq.GetAt(i-1));
		}
		else
		{
			szValue = "�ϼ�";
		}
		if (i > 0 && i < n+1)
			pTable->InsertColumn(i,"Ԥ��_"+szValue);
		else
			pTable->InsertColumn(i,szValue);
		if (i > 0) pTable->InsertItem(i-1,szValue);
	}
	for (i=0; i<n+1; i++)
	{
		for (j=0; j<n+1; j++)
		{
			if (i == n && j != n)
			{
				fSum = 0;
				for (k=0; k<n; k++)
					fSum += me(k)(j);
			}
			else if (i != n && j == n)
			{
				fSum = me(i).sum();
			}
			else if (i==n && j== n)
			{
				fSum = 0;
				for (k=0; k<n; k++)
					fSum += me(k).sum();
			}
			else
			{
				fSum = me(i)(j);
			}
			pTable->SetItemText(i,j+1,fSum);
		}
	}
	//��ӱ�
	CRsltElementTable *pETable = new CRsltElementTable("������Ԥ��ȶԱ�",pTable);//����һ���������Ķ���
	pResult->Add(pETable);
	//�ı�
	CalcText(me,arrFreq,v);
	CRsltElementText *pText = new CRsltElementText("������Ϣ");
	szValue.Format("��ȷ��Ϊ��%.2f��",v(4)<DOUBLE_ZERO ? 0 : 100*v(1)/v(4));
	pText->AddString(szValue);
	szValue.Format("������Ϊ��%.2f��",v(2)<DOUBLE_ZERO ? 0 : 100*v(0)/v(2));
	pText->AddString(szValue);
	szValue.Format("׼ȷ��Ϊ��%.2f��",v(3)<DOUBLE_ZERO ? 0 : 100*v(0)/v(3));
	pText->AddString(szValue);
	pResult->Add(pText);
	//����ͼ��
	CalcChart(m, &arr);
	//����
	pTable = new CTLTable;
	pTable->SetTitle("������Ԥ��ͼ��ֵ");
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"�ٷ�λ");
	pTable->InsertColumn(2,"Gainֵ");
	pTable->InsertColumn(3,"Liftֵ");
	j = 0;
	nSize = arr.GetSize();
	pDPVV1 = new tagDataPointValVal[nSize];
	pDPVV2 = new tagDataPointValVal[2];
	pDPVV3 = new tagDataPointValVal[nSize];
	for (i=nSize-1; i>=0; i--)
	{
		CEv *pEv = (CEv *)arr.GetAt(i);
		pDPVV1[j].fXVal = pEv->fX;
		pDPVV1[j].fYVal = pEv->fY;
		pDPVV3[j].fXVal = pEv->fX;
		pDPVV3[j].fYVal = pEv->fY/pEv->fX;
		szValue.Format("%d",j);
		pTable->InsertItem(j,szValue);
		pTable->SetItemText(j,1,pEv->fX);
		pTable->SetItemText(j,2,pEv->fY);
		pTable->SetItemText(j,3,pEv->fY/pEv->fX);
		j ++;
	}
	//ֱ��
	pDPVV2[0].fXVal = pDPVV1[0].fXVal;
	pDPVV2[0].fYVal = pDPVV1[0].fYVal;
	pDPVV2[1].fXVal = pDPVV1[nSize-1].fXVal;
	pDPVV2[1].fYVal = pDPVV1[nSize-1].fYVal;
	//���ͼ��
	pETable = new CRsltElementTable("������Ԥ��ͼ��ֵ",pTable);//����һ���������Ķ���
	pResult->Add(pETable);
	//��ͼ-Gain
	CTChartLine *pLine = new CTChartLine;
	pLine->SetLine(pDPVV1, nSize, ValVal);
	pLine->SetXAxilLabel("�ٷ�λ��");
	pLine->SetYAxilLabel("Gainֵ");
	CRsltElementChart *pChart = new CRsltElementChart("Gainͼ",pLine);
	pResult->SetName("Gainͼ"); 
	//ֱ��
	pLine->SetLine(pDPVV2, 2, ValVal);
	pResult->Add(pChart);
	//Liftͼ
	pLine = new CTChartLine;
	pLine->SetLine(pDPVV3, nSize, ValVal);
	pLine->SetXAxilLabel("�ٷ�λ��");
	pLine->SetYAxilLabel("Liftֵ");
	pChart = new CRsltElementChart("Liftͼ",pLine);
	pResult->SetName("Liftͼ"); 
	pResult->Add(pChart);
	//�ͷ��ڴ�
	delete []pDPVV1;
	delete []pDPVV2;
	delete []pDPVV3;
	Release(&arr);
}

//doubleֵ����ΪĿ�����,����intֵ����
void CEvaluate::CalcEvalue(CDoubleMatrix &m, CTArray<int,int>arrFreq, CDoubleMatrix &me)
{
	int i=0, j=0, nSize=0, nRows=0;
	int nSV=0, nDV=0, nEV=0, nRI=0, nCI=0;
	
	nSize = arrFreq.GetSize();
	nRows  = m.mrow();
	
	//����
	for (i=0; i<nRows; i++)
	{
		nSV = m(0)(i); //ʵ��ֵ
		nDV = m(1)(i); //Ԥ��ֵ
		nRI=nCI=0;
		for (j=0; j<nSize; j++)
		{
			nEV = arrFreq.GetAt(j);
			if (nSV == nEV)
			{
				nRI = j;
			}
			if (nDV == nEV)
			{
				nCI = j;
			}
		}
		me(nRI)(nCI) += 1;
	}
}

void CEvaluate::CalcFrequence(int nValue, CTArray<int,int>&arrFreq)
{
	int nTmp = 0;
	int nStart = 0;
	int nEnd = arrFreq.GetSize()-1;//Array������
	int nMid = 0;
	while (nStart <= nEnd)
	{
		nMid = (nEnd + nStart)/2;
		nTmp = arrFreq.GetAt(nMid) - nValue;
		if (nTmp < 0)
		{
			nStart = nMid+1;
			nMid += 1;
		}
		else if (nTmp > 0)
		{
			nEnd = nMid-1;
		}
		else
		{
			return;
		}
	}
	//����
	arrFreq.InsertAt(nMid,nValue);
}

CTArray<int,int> CEvaluate::CalcFrequence(CDoubleVector &v)
{
	int i=0, nVlen=0;
	CTArray<int,int>arrFreq;
	
	nVlen = v.vlen();
	if (nVlen < 1) return arrFreq;
	for (i=0; i<nVlen; i++)
	{
		CalcFrequence(v(i),arrFreq);
	}
	return arrFreq;
}

//Ŀ�������Label.
CTString CEvaluate::GetLabel(int nValue)
{
	int i=0, nSize=0;
	CTString szLabel("");
	
	nSize = m_pDecTree->m_TargetField.FieldLabel.size();
	for (i=0; i<nSize; i++)
	{
		if (nValue == m_pDecTree->m_TargetField.FieldLabel.at(i)->varValue.nVal)
		{
			szLabel = m_pDecTree->m_TargetField.FieldLabel.at(i)->strLabelName;
		}
	}
	if (szLabel.GetLength() < 1)
	{
		szLabel.Format("%d",nValue);
	}
	return szLabel;
}

inline bool CEvaluate::In(int nValue)
{
	int i=0, nSize=0;
	
	nSize = m_pDecTree->m_szArrHitRange.GetSize();
	if (nSize < 1)
	{
		return (nValue == 1 ? true : false);
	}
	int nMin = atoi(m_pDecTree->m_szArrHitRange.GetAt(0));
	int nMax = atoi(m_pDecTree->m_szArrHitRange.GetAt(nSize-1));
	return ((nValue >= nMin && nValue <= nMax) ? true : false);
}

//��С������
void CEvaluate::CalcConfidence(double fValue, double fConfidence, CPtrArray *pArr)
{	
	int nTemp = 0;
	CEv *pEv = NULL;
	double fTmp = 0;
	int nStart = 0;
	int nEnd = pArr->GetSize()-1;//Array������
	int nMid = 0;
	while (nStart <= nEnd)
	{
		nMid = (nEnd + nStart)/2;
		pEv = (CEv *)pArr->GetAt(nMid);
		fTmp = pEv->fConfidence-fConfidence;
		if (fTmp < -DOUBLE_ZERO)
		{
			nStart = nMid+1;
			nMid += 1;
		}
		else
		if (fTmp > DOUBLE_ZERO)
		{
			nEnd = nMid-1;
		}
		else
		{
			pEv->fX += 1;
			nTemp = In(fValue) ? 1 : 0; //�Ƿ���ʵ�ʷ�Χ�ڣ����򲻼�
			pEv->fY += nTemp;
			m_fValCnt += nTemp;
			return;
		}
	}
	//����
	pEv = new CEv;
	pEv->fConfidence  = fConfidence;
	pEv->fX = 1;
	nTemp = In(fValue) ? 1 : 0; //�Ƿ���ʵ�ʷ�Χ�ڣ����򲻼�
	pEv->fY += nTemp;
	m_fValCnt += nTemp;
	//����
	pArr->InsertAt(nMid,pEv);
}

void CEvaluate::Release(CPtrArray *pArr)
{
	int nSize = pArr->GetSize();

	for (int i=0; i<nSize; i++)
	{
		delete pArr->GetAt(i);
	}
	pArr->RemoveAll();
}
//ʵ��ֵ��Ԥ��ֵ�����Ŷ�
void CEvaluate::CalcChart(CDoubleMatrix &m, CPtrArray *pArr)
{
	int nValue = 0;
	int i=0, j=0, nSize=0, nRow=0;
	double fSum = 0;
	CDoubleVector v;
	
	m_fValCnt = 0;
	bool bString = m_pDecTree->m_TargetField.cFieldType == fString ? true : false;
	//ת����Hitֵ�����Ŷ�
	nRow = m.mrow();
	if (bString)
	{
		for (i=0; i<nRow; i++)
		{
		}
	}
	else
	{
		for (i=0; i<nRow; i++)
		{
			nValue = m(1)(i);//Ԥ��ֵ
			if (In(nValue)) continue;
			m(2)(i) = 1-m(2)(i); //���Ŷ�
		}
	}
	//����Value
	for (i=0; i<nRow; i++)
	{
		CalcConfidence(m(0)(i),m(2)(i),pArr);
	}
	//����
	double fSX = 0, fSY=0;
	nSize = pArr->GetSize();
	for (i=nSize-1; i>=0; i--)
	{
		CEv *pEv = (CEv *)pArr->GetAt(i);
		fSX += pEv->fX;
		fSY += pEv->fY;
		pEv->fX = fSX;
		pEv->fY = fSY;
		pEv->fX /= nRow;             //ת��Ϊ�ٷ�λ��
		pEv->fY /= m_fValCnt;        //ת��ΪGainֵ
	}
}

void CEvaluate::CalcText(CDoubleMatrix &m, CTArray<int,int>arrFreq, CDoubleVector &v)
{
	int i=0, j=0, k=0, nValue=0;
	int nSize=0, nRow=0, nCol=0;
	double fTemp=0, fSum=0, fDiag=0, fLine=0, fCol=0, fIn=0;
	//����
	v.create(5);
	nSize = arrFreq.GetSize();
	nRow = m.mrow();
	nCol = m.mcol();
	for (k=0; k<nSize; k++)
	{
		nValue = arrFreq.GetAt(k);
		fDiag += m(k)(k);
		fSum += m(k).sum();
		if (!In(nValue)) continue;
		//����ĳ��
		for (j=0; j<nCol; j++)
		{
			fTemp = m(k)(j);
			if (j==k) fIn += fTemp;
			fLine += fTemp;
		}
		//����ĳ��
		for (i=0; i<nRow; i++)
			fCol += m(i)(k);
	}
	//��ֵ
	v(0) = fIn;
	v(1) = fDiag;
	v(2) = fLine;
	v(3) = fCol;
	v(4) = fSum;
}