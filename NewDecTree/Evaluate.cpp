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
	CTString szValue("");//字符串
	CTArray<int, int>arrFreq;
	CDoubleVector  v;   
	CPtrArray arr;
	tagDataPointValVal *pDPVV1 = NULL;
	tagDataPointValVal *pDPVV2 = NULL;
	tagDataPointValVal *pDPVV3 = NULL;

	//计算频率
	v = m.col(0);
	arrFreq = CalcFrequence(v);
	int n = arrFreq.GetSize();
	//评估结果值
	CDoubleMatrix me(n,n);    
	CalcEvalue(m,arrFreq,me);
	//表格及画图
	CTLTable *pTable = new CTLTable;
	pTable->SetTitle("决策树预测比对表");
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
			szValue = "合计";
		}
		if (i > 0 && i < n+1)
			pTable->InsertColumn(i,"预测_"+szValue);
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
	//添加表
	CRsltElementTable *pETable = new CRsltElementTable("决策树预测比对表",pTable);//申请一个表对象类的对象
	pResult->Add(pETable);
	//文本
	CalcText(me,arrFreq,v);
	CRsltElementText *pText = new CRsltElementText("描述信息");
	szValue.Format("正确率为：%.2f％",v(4)<DOUBLE_ZERO ? 0 : 100*v(1)/v(4));
	pText->AddString(szValue);
	szValue.Format("覆盖率为：%.2f％",v(2)<DOUBLE_ZERO ? 0 : 100*v(0)/v(2));
	pText->AddString(szValue);
	szValue.Format("准确率为：%.2f％",v(3)<DOUBLE_ZERO ? 0 : 100*v(0)/v(3));
	pText->AddString(szValue);
	pResult->Add(pText);
	//计算图表
	CalcChart(m, &arr);
	//画表
	pTable = new CTLTable;
	pTable->SetTitle("决策树预测图表值");
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"百分位");
	pTable->InsertColumn(2,"Gain值");
	pTable->InsertColumn(3,"Lift值");
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
	//直线
	pDPVV2[0].fXVal = pDPVV1[0].fXVal;
	pDPVV2[0].fYVal = pDPVV1[0].fYVal;
	pDPVV2[1].fXVal = pDPVV1[nSize-1].fXVal;
	pDPVV2[1].fYVal = pDPVV1[nSize-1].fYVal;
	//添加图形
	pETable = new CRsltElementTable("决策树预测图表值",pTable);//申请一个表对象类的对象
	pResult->Add(pETable);
	//画图-Gain
	CTChartLine *pLine = new CTChartLine;
	pLine->SetLine(pDPVV1, nSize, ValVal);
	pLine->SetXAxilLabel("百分位数");
	pLine->SetYAxilLabel("Gain值");
	CRsltElementChart *pChart = new CRsltElementChart("Gain图",pLine);
	pResult->SetName("Gain图"); 
	//直线
	pLine->SetLine(pDPVV2, 2, ValVal);
	pResult->Add(pChart);
	//Lift图
	pLine = new CTChartLine;
	pLine->SetLine(pDPVV3, nSize, ValVal);
	pLine->SetXAxilLabel("百分位数");
	pLine->SetYAxilLabel("Lift值");
	pChart = new CRsltElementChart("Lift图",pLine);
	pResult->SetName("Lift图"); 
	pResult->Add(pChart);
	//释放内存
	delete []pDPVV1;
	delete []pDPVV2;
	delete []pDPVV3;
	Release(&arr);
}

//double值不能为目标变量,故用int值计算
void CEvaluate::CalcEvalue(CDoubleMatrix &m, CTArray<int,int>arrFreq, CDoubleMatrix &me)
{
	int i=0, j=0, nSize=0, nRows=0;
	int nSV=0, nDV=0, nEV=0, nRI=0, nCI=0;
	
	nSize = arrFreq.GetSize();
	nRows  = m.mrow();
	
	//计算
	for (i=0; i<nRows; i++)
	{
		nSV = m(0)(i); //实际值
		nDV = m(1)(i); //预测值
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
	int nEnd = arrFreq.GetSize()-1;//Array已有序
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
	//插入
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

//目标变量的Label.
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

//从小到大排
void CEvaluate::CalcConfidence(double fValue, double fConfidence, CPtrArray *pArr)
{	
	int nTemp = 0;
	CEv *pEv = NULL;
	double fTmp = 0;
	int nStart = 0;
	int nEnd = pArr->GetSize()-1;//Array已有序
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
			nTemp = In(fValue) ? 1 : 0; //是否在实际范围内，否则不加
			pEv->fY += nTemp;
			m_fValCnt += nTemp;
			return;
		}
	}
	//插入
	pEv = new CEv;
	pEv->fConfidence  = fConfidence;
	pEv->fX = 1;
	nTemp = In(fValue) ? 1 : 0; //是否在实际范围内，否则不加
	pEv->fY += nTemp;
	m_fValCnt += nTemp;
	//插入
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
//实际值－预测值－置信度
void CEvaluate::CalcChart(CDoubleMatrix &m, CPtrArray *pArr)
{
	int nValue = 0;
	int i=0, j=0, nSize=0, nRow=0;
	double fSum = 0;
	CDoubleVector v;
	
	m_fValCnt = 0;
	bool bString = m_pDecTree->m_TargetField.cFieldType == fString ? true : false;
	//转化非Hit值的置信度
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
			nValue = m(1)(i);//预测值
			if (In(nValue)) continue;
			m(2)(i) = 1-m(2)(i); //置信度
		}
	}
	//计算Value
	for (i=0; i<nRow; i++)
	{
		CalcConfidence(m(0)(i),m(2)(i),pArr);
	}
	//换算
	double fSX = 0, fSY=0;
	nSize = pArr->GetSize();
	for (i=nSize-1; i>=0; i--)
	{
		CEv *pEv = (CEv *)pArr->GetAt(i);
		fSX += pEv->fX;
		fSY += pEv->fY;
		pEv->fX = fSX;
		pEv->fY = fSY;
		pEv->fX /= nRow;             //转换为百分位数
		pEv->fY /= m_fValCnt;        //转换为Gain值
	}
}

void CEvaluate::CalcText(CDoubleMatrix &m, CTArray<int,int>arrFreq, CDoubleVector &v)
{
	int i=0, j=0, k=0, nValue=0;
	int nSize=0, nRow=0, nCol=0;
	double fTemp=0, fSum=0, fDiag=0, fLine=0, fCol=0, fIn=0;
	//创建
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
		//计算某行
		for (j=0; j<nCol; j++)
		{
			fTemp = m(k)(j);
			if (j==k) fIn += fTemp;
			fLine += fTemp;
		}
		//计算某列
		for (i=0; i<nRow; i++)
			fCol += m(i)(k);
	}
	//赋值
	v(0) = fIn;
	v(1) = fDiag;
	v(2) = fLine;
	v(3) = fCol;
	v(4) = fSum;
}