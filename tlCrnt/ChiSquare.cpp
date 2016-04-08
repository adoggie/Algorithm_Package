// ChiSquare.cpp: implementation of the CChiSquare class.
//				卡方检验
//					马良庄    2006/03/15
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlcrnt.h"
#include "ChiSquare.h"
//#include "Regression.h"

//#include "TLTable.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChiSquare::CChiSquare()
{
	m_bWithWeight = FALSE;//是否有权重
	m_bOutputFreqTable = FALSE;//是否输出频率分析详细表格

	m_bCheckDiscrib = FALSE;//由用户输入离散的给定概率的分布

	m_bCheckBin = TRUE;
	m_Bin_n = 1;//>=1;
	m_Bin_p = 0.5;//0.001< <0.999
	m_bGetVoBin = FALSE;
	m_bCheckNegativBin = FALSE;//负二项式分布, 没有负二项式分布的逆函数, 暂时不算他.
	m_bCheckPoisson = TRUE;
	m_bGetVoPoisson = FALSE;
	m_Poisson_lambda = 1;// >0.001
	m_bCheckGeo = TRUE;
	m_bGetVoGeo = FALSE;
	m_Geo_p = 0.5;// 0.001< <0.999
	m_bCheckFreq = FALSE;
	
	m_bCheckNormal = TRUE;
	m_bGetVoNormal = FALSE;
	m_Normal_mu = 0;
	m_Normal_sigma = 1;//>0.001
	m_bCheckAverage = TRUE;
	m_bGetVoAverage = FALSE;
	m_Average_low = 0;
	m_Average_up = 1;// m_Average_up>m_Average_low
	m_bCheckExp = TRUE;
	m_bGetVoExp = FALSE;
	m_Exp_lambda = 1;//>0.001
	m_bCheckLogNormal = FALSE;
	m_bGetVoLogNormal = TRUE;
	m_lognormalmu = 0;// == 0
	m_lognormalsigma = 1;// >0.001
	m_bCheckLogist = TRUE;
	m_bGetVoLogist = FALSE;
	m_Logist_m = 0;
	m_Logist_b = 1; //> 0.001
	m_bCheckGam = TRUE;
	m_bGetVoGam = FALSE;
	m_Gam_alpha = 1;// >0.001;
	m_Gam_beta = 1;// > 0.001;
	m_bCheckHalfNor = TRUE;
	m_bGetVoHalfNor = FALSE;
	m_HalfNor_mu = 0;// ==0
	m_HalfNor_sigma = 1;// >0.001
}

CChiSquare::~CChiSquare()
{
	m_tVarInfo.DeleteObject();
}

BOOL CChiSquare::GetData(CDataInterface *pDataInterface, CDoubleMatrix &Matrix_Data, int index)
{
	CDWordArray arCol;
	if (m_tVarInfo.pValue[index] < 0)
		return FALSE;
	arCol.Add(m_tVarInfo.pValue[index]);
	if(m_bWithWeight)//添加权重变量下标
       arCol.Add(m_nWeight);
	m_nRow = pDataInterface->GetColsData(arCol, Matrix_Data, 0);//取回数据
	return TRUE;
}

BOOL CChiSquare::CountFreq(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, CDoubleVector & vec_crit, CDoubleVector & vec_count)
{
	int dCase = vec_crit.vlen()+1;
	int length = vec_x.vlen();
	int step = max(m_nRow/dCase/4, 3);
	int sstep = max(m_nRow/dCase/24, 2);
	int nid = -1;
	int noldid = -1;
	double tempfortest;
	int kk;
	int i;
	for (i=0; i<dCase-1; i++)
	{
		tempfortest = vec_crit(i);
		noldid = nid;
		if (nid+1 < length)
		{
			while (vec_x(nid+1) < tempfortest)
			{
				if (nid+1+step >= length)
					break;
				nid += step;
			}
			while (vec_x(nid+1) > tempfortest)
			{
				if (nid+1-sstep <0)
					break;
				nid -= sstep;
			}
			if (nid+1-sstep <0)
				nid = -1;
			while (vec_x(nid+1) < tempfortest)
			{
				nid ++;
				if (nid+1 >= length)
					break;
			}

			if (m_bWithWeight) 
				for (kk=noldid+1; kk<nid+1; kk++)
					vec_count(i) += vec_weight(vec_index(kk)-1);
			else
				vec_count(i) = nid - noldid;
			if ((vec_count(i)) < 0)
				vec_count(i) = m_nRow / dCase;
		}
	}
	if (m_bWithWeight)
		for (kk=nid+1; kk<m_nRow; kk++)
			vec_count(i) += vec_weight(vec_index(kk)-1);
	else
		vec_count(i) = m_nRow - nid - 1;
	return TRUE;
}

double CChiSquare::countfordis(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, int i)
{
	int length = vec_x.vlen();
	if (m_nlastcountto+1 >= length )
		return 0;
	int step = max(sqrt(m_nRow), 3);
	int sstep = max(sqrt(m_nRow)/7, 2);
	while (vec_x(m_nlastcountto+1) < (i - EPS_ACCUR))
	{	
		m_nlastcountto++;
		if (m_nlastcountto+1 >= length )
			return 0;
	}
	int oldcount = m_nlastcountto;
	while (vec_x(m_nlastcountto+1) < (i+EPS_ACCUR))
	{
		if (m_nlastcountto+1+step >= length)
			break;
		m_nlastcountto += step;
	}
	while (vec_x(m_nlastcountto+1) > (i - EPS_ACCUR))
	{
		if (m_nlastcountto+1-sstep <0)
			break;
		m_nlastcountto -= sstep;
	}
	if (m_nlastcountto+1-sstep <0)
		m_nlastcountto = -1;
	while (vec_x(m_nlastcountto+1) < (i+EPS_ACCUR))
	{
		m_nlastcountto ++;
		if (m_nlastcountto+1 >= length)
			break;
	}
	if (m_bWithWeight) 
	{
		double count_num = 0;
		for (int j=oldcount; j<m_nlastcountto; j++)
			count_num += vec_weight(vec_index(j+1)-1);
		return count_num;
	}
	else
	{
		double count_num = m_nlastcountto - oldcount;
		return count_num;
	}
}

double CChiSquare::countfordis(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, double i)
{
	int length = vec_x.vlen();
	if (m_nlastcountto+1 >= length )
		return 0;
	int step = max(sqrt(m_nRow), 3);
	int sstep = max(sqrt(m_nRow)/7, 2);
	while (vec_x(m_nlastcountto+1) < (i - EPS_ACCUR))
	{	
		m_nlastcountto++;
		if (m_nlastcountto+1 >= length )
			return 0;
	}
	int oldcount = m_nlastcountto;
	while (vec_x(m_nlastcountto+1) < (i+EPS_ACCUR))
	{
		if (m_nlastcountto+1+step >= length)
			break;
		m_nlastcountto += step;
	}
	while (vec_x(m_nlastcountto+1) > (i - EPS_ACCUR))
	{
		if (m_nlastcountto+1-sstep <0)
			break;
		m_nlastcountto -= sstep;
	}
	if (m_nlastcountto+1-sstep <0)
		m_nlastcountto = -1;
	while (vec_x(m_nlastcountto+1) < (i+EPS_ACCUR))
	{
		m_nlastcountto ++;
		if (m_nlastcountto+1 >= length)
			break;
	}
	if (m_bWithWeight) 
	{
		double count_num = 0;
		for (int j=oldcount; j<m_nlastcountto; j++)
			count_num += vec_weight(vec_index(j+1)-1);
		return count_num;
	}
	else
	{
		double count_num = m_nlastcountto - oldcount;
		return count_num;
	}
}

int  CChiSquare::FindStartIndex(CDoubleVector &vec_x,int nVxInd,int nVtData,int nStep_g,int nStep_b)
{
	int nVxLen = vec_x.vlen();

	if (nVxInd >= nVxLen ) return nVxLen;
	
	if (vec_x(nVxInd) >= nVtData-EPS_ACCUR)//找到索引
		return nVxInd;

	while (nVxInd < nVxLen && vec_x(nVxInd) < nVtData-EPS_ACCUR)
	{
		nVxInd += nStep_g;
	}
	if (nVxInd >= nVxLen) nVxInd -= nStep_g;
	while (nVxInd > 0 && nVxInd < nVxLen && vec_x(nVxInd) > nVtData+EPS_ACCUR)
	{
		nVxInd -= nStep_b;
	}
	if (nVxInd < 0) nVxInd = 0;
	if (vec_x(nVxInd) < nVtData-EPS_ACCUR) //后退小于当前值
	{
		while (nVxInd < nVxLen && vec_x(nVxInd) < nVtData-EPS_ACCUR)
		{
			nVxInd++;
		}
	}
	else                         //后退等于当前值      
	{
		while (nVxInd > 0 && vec_x(nVxInd) > nVtData-EPS_ACCUR)
		{
			nVxInd --;
		}
	}
	return nVxInd >= nVxLen ? nVxInd-1 : nVxInd;
}

int  CChiSquare::FindEndIndex(CDoubleVector &vec_x,int nVxInd,int nVtData,int nStep_g,int nStep_b)
{
	int nVxLen = vec_x.vlen();
	
	if (nVxInd >= nVxLen ) return nVxLen;
	
	if (vec_x(nVxInd) > nVtData+EPS_ACCUR)
		return nVxInd;

	while (nVxInd < nVxLen && vec_x(nVxInd) <= nVtData+EPS_ACCUR)
	{
		nVxInd += nStep_g;
	}
	if (nVxInd >= nVxLen) nVxInd -= nStep_g;
	while (nVxInd >= 0 && nVxInd < nVxLen && vec_x(nVxInd) > nVtData+EPS_ACCUR)
	{
		nVxInd -= nStep_b;
	}
	if (nVxInd < 0) nVxInd = 0;
	while (nVxInd < nVxLen && vec_x(nVxInd) <= nVtData+EPS_ACCUR)
	{
		nVxInd ++;
	}
	return nVxInd > nVxLen ? nVxInd-1 : nVxInd;
}

BOOL CChiSquare::_CountFreq(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, CDoubleVector & vec_crit, CDoubleVector & vec_count)
{
	int i=0, j=0, k=0;
	int nLen=0, nCase=0, nStartData=0;
	int nStart=0, nEnd=0, nStep_g=0, nStep_b=0;
	nLen    = vec_x.vlen();
	nCase   = vec_crit.vlen();
	nStep_g = max(m_nRow/nCase/4, 3); //前进的步伐
	nStep_b = max(m_nRow/nCase/24, 2);//后退的步伐
	if (nCase < 1 ) return FALSE;
	
	nStartData = vec_crit(0);
	for (i=0; i<nCase; i++) 
	{
		if (nStart >= nLen)//分组结束
		{
			vec_count(i) = 0;
			continue;
		}
		for (i==0 ? j=int(vec_x(0)) : j=nStartData; j<=vec_crit(i); j++)
		{
			nStart = FindStartIndex(vec_x,nStart,j,nStep_g,nStep_b);
			nEnd = FindEndIndex(vec_x,nStart,j,nStep_g,nStep_b);
			////////////////////////////////////////////////////
			if (m_bWithWeight) 
			{
				for (k=nStart; k<nEnd; k++)
					vec_count(i) += vec_weight(vec_index(k-1));
			}
			else
			{
				vec_count(i) += nEnd-nStart;
			}
			/////////////////////////////////////////////////////
			nStart = nEnd;
		}
		nStartData = vec_crit(i)+1;
	}
	//循环结束后，尾部全部算做另一个划分-拿出来，为了减少逻辑判断
	for (j=nStartData; j<=vec_x(nLen-1); j++)
	{
		nStart = FindStartIndex(vec_x,nStart,j,nStep_g,nStep_b);
		nEnd = FindEndIndex(vec_x,nStart,j,nStep_g,nStep_b);
		////////////////////////////////////////////////////
		if (m_bWithWeight) 
		{
			for (k=nStart; k<nEnd; k++)
				vec_count(i) += vec_weight(vec_index(k-1));
		}
		else
		{
			vec_count(i) += nEnd-nStart;
		}
		if ( vec_count(i) < 0)
			vec_count(i) = m_nRow/nCase;
		/////////////////////////////////////////////////////
		nStart = nEnd;
		nStartData += 1;
	}
	return TRUE;
}

int  CChiSquare::FindNextIndex(CDoubleVector &vec_x, int nVxInd, int nStep_g, int nStep_b)
{ 	
	
	int nVxLen = vec_x.vlen();
	double fVtData = vec_x(nVxInd);//初值
	while (nVxInd < nVxLen && 
		   vec_x(nVxInd) >= fVtData-EPS_ACCUR &&
		   vec_x(nVxInd) <= fVtData+EPS_ACCUR)
	{
		nVxInd += nStep_g;
	}
	if (nVxInd >= nVxLen) nVxInd -= nStep_g;
	while (nVxInd >= 0     && 
		   nVxInd < nVxLen && 
		   vec_x(nVxInd) > fVtData+EPS_ACCUR)
	{
		nVxInd -= nStep_b;
	}
	if (nVxInd < 0) nVxInd = 0;
	while (nVxInd < nVxLen && //后退小于或等于当前值
		   vec_x(nVxInd) <= fVtData+EPS_ACCUR)
	{
		nVxInd++;
	}
	return nVxInd;
}

//统计频数
BOOL CChiSquare::CalcFreq(CDoubleVector & vec_x, CDoubleVector & vec_w, CIntVector & vec_index, CDoubleMatrix &frq_matrix)
{
	int nLen=0, nRow=0, nStart=0, nEnd=0, nStep_g=0, nStep_b=0;
	
	nLen    = vec_x.vlen();
	if (m_nRow < 100)
	{
		nStep_g = 2; //前进的步伐
		nStep_b = 1; //后退的步伐
	}
	else
	if (m_nRow < 1000)
	{
		nStep_g = max(sqrt(m_nRow)/6, 3); //前进的步伐
		nStep_b = max(sqrt(m_nRow)/8, 2); //后退的步伐
	}
	else
	if (m_nRow < 5000)
	{
		nStep_g = max(sqrt(m_nRow)/10, 5); //前进的步伐
		nStep_b = max(sqrt(m_nRow)/15, 3); //后退的步伐
	}
	else
	{
		nStep_g = max(sqrt(m_nRow)/20, 8); //前进的步伐
		nStep_b = max(sqrt(m_nRow)/30, 4); //后退的步伐
	}
	//外部没有分配空间
	if (frq_matrix.mrow()<1 || frq_matrix.mcol()<1)
		frq_matrix.create(nLen,2);
	//有权重，需要循环，同时减少逻辑判断，所以分开做。
	if (m_bWithWeight) 
	{
		double fInitData = vec_x(0);
		for (nStart=0; nStart<nLen; nStart++)
		{
			if (vec_x(nStart) < fInitData-EPS_ACCUR ||
				vec_x(nStart) > fInitData+EPS_ACCUR)
			{
				nRow++;
				fInitData = vec_x(nStart);
			}
			frq_matrix(nRow,0) = fInitData;
			frq_matrix(nRow,1) += vec_w(vec_index(nStart-1));
		}
	}
	else
	{
		nRow = -1;
		for (nStart=0; nStart<nLen; nStart=nEnd)
		{
			nEnd = FindNextIndex(vec_x,nStart,nStep_g,nStep_b);
			frq_matrix(nRow,0) = vec_x(nStart);
			frq_matrix(nRow,1) = nEnd-nStart;
			nRow++;
		}
	}//循环结束
	frq_matrix.rerowsize(nRow);
	return TRUE;
}
//vec_x:输入的向量，fPercent:需要缩减的百分数
void CChiSquare::ReSize(CDoubleVector & vec_x,double fPercent)
{

	int nRow     = vec_x.vlen();
	int nPercent = nRow * fPercent/100.0; 
	if (nPercent<=0 || nPercent>=nRow)
		return;
	for (int i=nPercent/2;i<nRow-nPercent/2;i++)//仅算偶数位。
	{
		vec_x(i-nPercent/2)=vec_x(i);
	}
	vec_x.resize(nRow-nPercent/2-nPercent/2);
	m_nRow = vec_x.vlen();
}

void CChiSquare::ReSize(CIntVector & vec_x,double fPercent)
{

	int nRow     = vec_x.vlen();
	int nPercent = nRow * fPercent; 
	if (nPercent<=0 || nPercent>=nRow)
		return;
	for (int i=nPercent/2;i<nRow-nPercent/2;i++)//仅算偶数位。
	{
		vec_x(i-nPercent/2)=vec_x(i);
	}
	vec_x.resize(nRow-nPercent/2-nPercent/2);
	m_nRow = vec_x.vlen();
}

void CChiSquare::DrawResult(CDoubleVector vec_crit, 
							CDoubleVector vec_count, 
							CDoubleVector vec_expect, 
							CDoubleVector vec_res,
							CResult *pResult)
{
	if (!m_bOutputFreqTable)
		return;
	CTLTable *pTable11 = NULL;
	CRsltElementTable *pETable11=NULL;
	
	pTable11 = new CTLTable;
	pTable11->CreateTable(2, 2);	
	pTable11->SetTitle(m_str_x+"   "+m_str_FunInfo);
	pTable11->SetRows(2);
	pTable11->SetCols(2);
	pTable11->InsertColumn(0, "序号");
	pTable11->InsertColumn(1, "观测区间");
	pTable11->InsertColumn(2, "观测频数");
	pTable11->InsertColumn(3, "期望频数");
	pTable11->InsertColumn(4, "残差");
	
	int i = 0;
	CTString str;
	CTString strValue;
	for(i=0; i<vec_crit.vlen()+1; i++)
	{
		str.Format("%d", i + 1);
		pTable11->InsertItem(i, str);
		str.Format("%.4f", vec_crit(0));
		if(i == 0)
		{
			str.Format("%.4f", vec_crit(0));
			strValue = "-∞ ~ " + str;
		}
		else if(i == vec_crit.vlen())
		{
			str.Format("%.4f", vec_crit(vec_crit.vlen()-1));
			strValue = str + " ~ +∞";
		}
		else
		{
			CTString str1;
			str.Format("%.4f", vec_crit(i-1));
			str1.Format("%.4f", vec_crit(i));
			strValue = str + " ~ " + str1;
		}
		pTable11->SetItemText(i, 1, strValue);
		pTable11->SetItemText(i, 2, vec_count(i));
		pTable11->SetItemText(i, 3, vec_expect(i));
		pTable11->SetItemText(i, 4, vec_res(i));		
	}
	pTable11->InsertItem(i, "");
	pTable11->SetItemText(i, 1, "总计");
	pTable11->SetItemText(i, 2, sum(vec_count));
	pTable11->SetItemText(i, 3, vec_expect.sum());
	
	pETable11 = new CRsltElementTable(m_str_x+" "+m_str_FunInfo, pTable11);//申请一个表对象类的对象
	pResult->Add(pETable11);
}

void CChiSquare::DrawResultInt(CDoubleVector vec_crit, 
							CDoubleVector vec_count, 
							CDoubleVector vec_expect, 
							CDoubleVector vec_res,
							CResult *pResult)
{
	if (!m_bOutputFreqTable)
		return;
	CTLTable *pTable11 = NULL;
	CRsltElementTable *pETable11=NULL;
	
	pTable11 = new CTLTable;
	pTable11->CreateTable(2, 2);	
	pTable11->SetTitle(m_str_x+"   "+m_str_FunInfo);
	pTable11->SetRows(2);
	pTable11->SetCols(2);
	pTable11->InsertColumn(0, "序号");
	pTable11->InsertColumn(1, "观测区间");
	pTable11->InsertColumn(2, "观测频数");
	pTable11->InsertColumn(3, "期望频数");
	pTable11->InsertColumn(4, "残差");
	
	int i = 0;
	CTString str;
	CTString strValue;
	for(i=0; i<vec_crit.vlen()+1; i++)
	{
		str.Format("%d", i + 1);
		pTable11->InsertItem(i, str);
		str.Format("%.0f", vec_crit(0)-1);
		if(i == 0)
		{
			str.Format("%.0f", vec_crit(0));
			strValue = "-∞ ~ " + str;
		}
		else if(i == vec_crit.vlen())
		{
			str.Format("%.0f", vec_crit(vec_crit.vlen()-1)+1);
			strValue = str + " ~ +∞";
		}
		else
		{
			CTString str1;
			str.Format("%.0f", vec_crit(i-1)+1);
			str1.Format("%.0f", vec_crit(i));
			strValue = str + " ~ " + str1;
		}
		pTable11->SetItemText(i, 1, strValue);
		pTable11->SetItemText(i, 2, vec_count(i));
		pTable11->SetItemText(i, 3, vec_expect(i));
		pTable11->SetItemText(i, 4, vec_res(i));		
	}
	pTable11->InsertItem(i, "");
	pTable11->SetItemText(i, 1, "总计");
	pTable11->SetItemText(i, 2, sum(vec_count));
	pTable11->SetItemText(i, 3, vec_expect.sum());
	
	pETable11 = new CRsltElementTable(m_str_x+" "+m_str_FunInfo, pTable11);//申请一个表对象类的对象
	pResult->Add(pETable11);
}

void CChiSquare::DrawResultBin(CDoubleVector vec_crit, 
							CDoubleVector vec_count, 
							CDoubleVector vec_expect, 
							CDoubleVector vec_res,
							CResult *pResult)
{
	if (!m_bOutputFreqTable)
		return;
	CTLTable *pTable11 = NULL;
	CRsltElementTable *pETable11=NULL;
	
	pTable11 = new CTLTable;
	pTable11->CreateTable(2, 2);	
	pTable11->SetTitle(m_str_x+"   "+m_str_FunInfo);
	pTable11->SetRows(2);
	pTable11->SetCols(2);
	pTable11->InsertColumn(0, "序号");
	pTable11->InsertColumn(1, "观测区间");
	pTable11->InsertColumn(2, "观测频数");
	pTable11->InsertColumn(3, "期望频数");
	pTable11->InsertColumn(4, "残差");
	
	int i = 0;
	CTString str;
	CTString strValue;
	for(i=0; i<vec_crit.vlen(); i++)
	{
		str.Format("%d", i + 1);
		pTable11->InsertItem(i, str);
		str.Format("%.0f", vec_crit(0)-1);
		if(i == 0)
		{
			str.Format("%.0f", vec_crit(0));
			strValue = "0 ~ " + str;
		}
		else if(i == vec_crit.vlen())
		{
			str.Format("%.0f", vec_crit(vec_crit.vlen()-1)+1);
			strValue = str + " ~ +∞";
		}
		else
		{
			CTString str1;
			str.Format("%.0f", vec_crit(i-1)+1);
			str1.Format("%.0f", vec_crit(i));
			strValue = str + " ~ " + str1;
		}
		pTable11->SetItemText(i, 1, strValue);
		pTable11->SetItemText(i, 2, vec_count(i));
		pTable11->SetItemText(i, 3, vec_expect(i));
		pTable11->SetItemText(i, 4, vec_res(i));		
	}
	pTable11->InsertItem(i, "");
	pTable11->SetItemText(i, 1, "总计");
	pTable11->SetItemText(i, 2, sum(vec_count));
	pTable11->SetItemText(i, 3, vec_expect.sum());
	
	pETable11 = new CRsltElementTable(m_str_x+" "+m_str_FunInfo, pTable11);//申请一个表对象类的对象
	pResult->Add(pETable11);
}

void CChiSquare::DrawResultFreq(CDoubleVector vec_crit, 
							CDoubleVector vec_count, 
							CDoubleVector vec_expect, 
							CDoubleVector vec_res,
							CResult *pResult)
{
	if (!m_bOutputFreqTable)
		return;
	CTLTable *pTable11 = NULL;
	CRsltElementTable *pETable11=NULL;
	
	pTable11 = new CTLTable;
	pTable11->CreateTable(2, 2);	
	pTable11->SetTitle(m_str_x+"   "+m_str_FunInfo);
	pTable11->SetRows(2);
	pTable11->SetCols(2);
	pTable11->InsertColumn(0, "序号");
	pTable11->InsertColumn(1, "观测区间");
	pTable11->InsertColumn(2, "观测频数");
	pTable11->InsertColumn(3, "期望频数");
	pTable11->InsertColumn(4, "残差");
	if (vec_crit.vlen() > 10000)//频率的种类过多，超过10000，不输出频率分析表
	{
		pTable11->InsertItem(1, "观测值种类 \r\n超过10000，\r\n不输出频率分析表");
		pETable11 = new CRsltElementTable(m_str_x+" "+"注意 "+m_str_FunInfo, pTable11);//申请一个表对象类的对象
		pResult->Add(pETable11);
		return;
	}
	int i = 0;
	CTString str;
	CTString strValue;
	for(i=0; i<vec_crit.vlen(); i++)
	{
		str.Format("%d", i + 1);
		pTable11->InsertItem(i, str);
		pTable11->SetItemText(i, 1, vec_crit(i));
		pTable11->SetItemText(i, 2, vec_count(i));
		pTable11->SetItemText(i, 3, vec_expect(i));
		pTable11->SetItemText(i, 4, vec_res(i));		
	}
	pTable11->InsertItem(i, "");
	pTable11->SetItemText(i, 1, "总计");
	pTable11->SetItemText(i, 2, sum(vec_count));
	pTable11->SetItemText(i, 3, vec_expect.sum());

	pETable11 = new CRsltElementTable(m_str_x+" "+m_str_FunInfo, pTable11);//申请一个表对象类的对象
	pResult->Add(pETable11);
	return;
}

BOOL CChiSquare::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
//CHISQUARETEST
//	/DATA=[FILE]
//	/VARLIST [varlist]
//	/WEIGHT [varname]
//	/TEST [NORMAL] [NORMALPARA({0},{1})] [UNIFORMCON] [UNIFORMCONPARA({0},{1})] [EXP] [EXPPARA({1})]
//	 [GAMMA] [GAMMAPARA({1},{1})] [HALFNOR] [HALFNORPARA({0},{1})] [LOGISTIC] [LOGISTICPARA({0},{1})] 
//	 [LOG] [LOGPARA({0},{1})] [BIN] [BINPARA({1},{0.5})] [GEO] [GEOPARA({0.5})]
//	 [POISSON] [POISSONPARA({1})] [SAMEPRO]
//	/GIVENDIS DIS({#,#,#,...})
//	/GIVENPROB PRO({#,#,#,...})
//	/OUTPUT [PARTICULAR]
//	/METHOD [CUTEND({0.03})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	
	if (AnaSyntax::exist("/WEIGHT", VOstr))
	{
		m_bWithWeight = TRUE;
		CTStringArray ynames;
		AnaSyntax::getVAR("/WEIGHT", VOstr, ynames);
		if ((ynames.GetSize())!=1) 
			return FALSE;
		CTString yname = ynames.GetAt(0);
		m_nWeight = pDataInterface->IndexByName(yname);
	}
	else
		m_bWithWeight = FALSE;
//////////////////////////////////////////////////////////////////////////
//读取是否检验分布的参数
////////////////////////////////////////////////////////////////////////////

//正态
	if (AnaSyntax::findstr("/TEST", "NORMALPARA", VOstr) )
	{
		m_bCheckNormal = TRUE;
		m_bGetVoNormal = TRUE;
		m_Normal_mu = AnaSyntax::getvalue("/TEST", "NORMALPARA", VOstr);
		m_Normal_sigma = AnaSyntax::getvalue("/TEST", "NORMALPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "NORMAL", VOstr) )
		m_bCheckNormal = TRUE;
	else
		m_bCheckNormal = FALSE;
//均匀分布
	if (AnaSyntax::findstr("/TEST", "UNIFORMCONPARA", VOstr) )
	{
		m_bCheckAverage = TRUE;
		m_bGetVoAverage = TRUE;
		m_Average_low = AnaSyntax::getvalue("/TEST", "UNIFORMCONPARA", VOstr);
		m_Average_up = AnaSyntax::getvalue("/TEST", "UNIFORMCONPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "UNIFORMCON", VOstr) )
		m_bCheckAverage = TRUE;
	else
		m_bCheckAverage = FALSE;
//指数分布
	if (AnaSyntax::findstr("/TEST", "EXPPARA", VOstr) )
	{
		m_bCheckExp = TRUE;
		m_bGetVoExp = TRUE;
		m_Exp_lambda = AnaSyntax::getvalue("/TEST", "EXPPARA", VOstr);
	}
	else if (AnaSyntax::findstr("/TEST", "EXP", VOstr) )
		m_bCheckExp = TRUE;
	else
		m_bCheckExp = FALSE;
//	 Gamma分布
	if (AnaSyntax::findstr("/TEST", "GAMMAPARA", VOstr) )
	{
		m_bCheckGam = TRUE;
		m_bGetVoGam = TRUE;
		m_Gam_alpha = AnaSyntax::getvalue("/TEST", "GAMMAPARA", VOstr);
		m_Gam_beta = AnaSyntax::getvalue("/TEST", "GAMMAPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "GAMMA", VOstr) )
		m_bCheckGam = TRUE;
	else
		m_bCheckGam = FALSE;
//	半正态分布
	if (AnaSyntax::findstr("/TEST", "HALFNORPARA", VOstr) )
	{
		m_bCheckHalfNor = TRUE;
		m_bGetVoHalfNor = TRUE;
		m_HalfNor_mu = AnaSyntax::getvalue("/TEST", "HALFNORPARA", VOstr);
		m_HalfNor_sigma = AnaSyntax::getvalue("/TEST", "HALFNORPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "HALFNOR", VOstr) )
		m_bCheckHalfNor = TRUE;
	else
		m_bCheckHalfNor = FALSE;
//逻辑
	if (AnaSyntax::findstr("/TEST", "LOGISTICPARA", VOstr) )
	{
		m_bCheckLogist = TRUE;
		m_bGetVoLogist = TRUE;
		m_Logist_m = AnaSyntax::getvalue("/TEST", "LOGISTICPARA", VOstr);
		m_Logist_b = AnaSyntax::getvalue("/TEST", "LOGISTICPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "LOGISTIC", VOstr) )
		m_bCheckLogist = TRUE;
	else
		m_bCheckLogist = FALSE;
//对数正态
	if (AnaSyntax::findstr("/TEST", "LOGPARA", VOstr) )
	{
		m_bCheckLogNormal = TRUE;
		m_bGetVoLogNormal = TRUE;
		m_lognormalmu = AnaSyntax::getvalue("/TEST", "LOGPARA", VOstr);
		m_lognormalsigma = AnaSyntax::getvalue("/TEST", "LOGPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "LOG", VOstr) )
		m_bCheckLogNormal = TRUE;
	else
		m_bCheckLogNormal = FALSE;
//////////////////////////////////////////////////////////////////////////
//离散的
//////////////////////////////////////////////////////////////////////////

//二项式
	//	[GEO] [GEOPARA({0.5})]
//	 [POISSON] [POISSONPARA({1})] [SAMEPRO]
	if (AnaSyntax::findstr("/TEST", "BINPARA", VOstr) )
	{
		m_bCheckBin = TRUE;
		m_bGetVoBin = TRUE;
		m_Bin_n = AnaSyntax::getvalue("/TEST", "BINPARA", VOstr);
		m_Bin_p = AnaSyntax::getvalue("/TEST", "BINPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "BIN", VOstr) )
		m_bCheckBin = TRUE;
	else
		m_bCheckBin = FALSE;
//几何分布
//	 [POISSON] [POISSONPARA({1})] [SAMEPRO]
	if (AnaSyntax::findstr("/TEST", "GEOPARA", VOstr) )
	{
		m_bCheckGeo = TRUE;
		m_bGetVoGeo = TRUE;
		m_Geo_p = AnaSyntax::getvalue("/TEST", "GEOPARA", VOstr);
	}
	else if (AnaSyntax::findstr("/TEST", "GEO", VOstr) )
		m_bCheckGeo = TRUE;
	else
		m_bCheckGeo = FALSE;
//泊松
	if (AnaSyntax::findstr("/TEST", "POISSONPARA", VOstr) )
	{
		m_bCheckPoisson = TRUE;
		m_bGetVoPoisson = TRUE;
		m_Poisson_lambda = AnaSyntax::getvalue("/TEST", "POISSONPARA", VOstr);
	}
	else if (AnaSyntax::findstr("/TEST", "POISSON", VOstr) )
		m_bCheckPoisson = TRUE;
	else
		m_bCheckPoisson = FALSE;
//出现概率相同
	if (AnaSyntax::findstr("/TEST", "SAMEPRO", VOstr) )
		m_bCheckFreq = TRUE;
	else
		m_bCheckFreq = FALSE;
//	/GIVENDIS [{#,#,#,...}]
//	/GIVENPROB [{#,#,#,...}]
	if (AnaSyntax::exist("/GIVENDIS", VOstr))
	{
		if (!AnaSyntax::exist("/GIVENPROB", VOstr))
			return FALSE;
		CTStringArray dis_str;
		CTStringArray pro_str;
		AnaSyntax::getVAR("/GIVENDIS", VOstr, dis_str);
		AnaSyntax::getVAR("/GIVENPROB", VOstr, pro_str);
		int nGive = dis_str.GetSize();
		if (nGive != pro_str.GetSize())
			return FALSE;
		CDoubleVector mm(nGive);
		vec_y_dis = mm;
		CDoubleVector nn(nGive);
		vec_y_prob = nn;
		for (int i=0; i<nGive; i++)
		{
			vec_y_dis(i) = atof(dis_str.GetAt(i));
			vec_y_prob(i) = atof(pro_str.GetAt(i));
		}
		m_bCheckDiscrib = TRUE;
	}

	m_bOutputFreqTable = AnaSyntax::findstr("/OUTPUT", "PARTICULAR", VOstr);
//	/METHOD [CUTEND({0.03})]
	if (AnaSyntax::findstr("/METHOD", "CUTEND", VOstr))
	{
		m_bCheckDel = TRUE;
		m_dDelPer = AnaSyntax::getvalue("/METHOD", "CUTEND", VOstr);
	}
	else
		m_bCheckDel = FALSE;

	return TRUE;
}

CResult * CChiSquare::OnChiSquare(CDataInterface * pDataInterface, string VO)
{
	//CTimer tTemp;
	CResult * pResult = NULL;
	pResult=new CResult("卡方检验");
	m_pResult = pResult;
	
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	int i;
	if (pDataInterface->GetWeightNumb() < 0)
		m_bWithWeight = FALSE;
	else
		m_bWithWeight = TRUE;
	
	CDoubleMatrix data_read;
	CTString	str_temp;
//对每一个选入的变量进行分析处理
	for (int index=0; index < m_tVarInfo.iCount; index++)
	{
		m_AllFuncInfo.RemoveAll();
		m_str_x = pDataInterface->GetFieldName(m_tVarInfo.pValue[index]);
		int totalfunc;
		totalfunc = (m_bCheckLogNormal + m_bCheckExp + m_bCheckAverage + m_bCheckNormal + m_bCheckGam + m_bCheckHalfNor + m_bCheckLogist) + 
			(m_bCheckBin + m_bCheckNegativBin + m_bCheckPoisson + m_bCheckGeo + m_bCheckFreq) + 
			(m_bCheckDiscrib);
		CDoubleVector vec_sig(totalfunc);
		CDoubleVector vec_df(totalfunc);
		CDoubleVector vec_Chi2(totalfunc);
		int countnum = -1;
//读取数据
		if (!GetData(pDataInterface, data_read, index))
		{
			CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
			pPRsltElementText->SetName("警告");
			pPRsltElementText->AddString("数据读取发生错误\r\n");
			m_pResult->Add(pPRsltElementText);
			return pResult;
		}
		if (m_nRow < 7)
		{
			CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
			pPRsltElementText->SetName("警告");
			pPRsltElementText->AddString("观测个数太少\r\n无法计算");
			m_pResult->Add(pPRsltElementText);
			return pResult;
		}
//对数据进行预处理: 排序, 算均值, 方差等
		int dCase = min(pow(m_nRow, 1/4.0)*10, sqrt(m_nRow*1.0))+3;//分成dCase类
		CDoubleVector vec_crit(dCase-1);
		CIntVector vec_index;
		CDoubleVector vec_x = data_read(0);
		CDoubleVector vec_weight;
		CDoubleVector vec_count(dCase);
		if (m_bWithWeight)
		{
			vec_weight = data_read(1);
			for (i=0; i<m_nRow; i++)
				if (vec_weight(i) < -EPS_ACCUR)
				{		
					CTString strWarning = "权重出现负数，不能计算！";
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
					return m_pResult;
				}
		}
		double mu;
		double sigma;
		//tTemp.Toc("准备");
		double m_totalweight;
		if (m_bWithWeight)
		{
			mu = vec_x.mean_w(vec_weight);
			sigma = sqrt(var_w(vec_x, vec_weight));
			vec_x = vec_x.Sort(vec_index);
			if (m_bCheckDel)
			{
				ReSize(vec_x, m_dDelPer);
				ReSize(vec_index, m_dDelPer);
			}
			m_totalweight = vec_weight.sum();
		}
		else
		{
			mu = vec_x.mean();
			double sdfs = vec_x.var();
			sigma = sqrt(vec_x.var());
			vec_x.Sort();
			if (m_bCheckDel)
			{
				ReSize(vec_x, m_dDelPer);
			}
			m_totalweight = m_nRow;
		}
//下面是对10多种分布求Chi2值的程序 
//这十多个分布的处理思路是 大-同. 小-异. 但是程序几乎无法合并
/******************************************************
*	检验这些分布时, 分3步, 
*	第0步, 计算分布的参数, 以及需要划分成的区间
*	第一步, 计算各个区间的划分的临界值, 以及预期的概率等
*	第二步, 计算各个区间里边出现的观测的个数
*	第三步, 计算Chi2的值
******************************************************/

//离散型的开始
		
		double lim_pro = max(0.3 / dCase, 0.1/sqrt(dCase));
		
//检验是否服从Bin分布
		if (m_bCheckBin && mu > 0 && (((1 - sigma*sigma/mu) > 0.001 && vec_x(m_nRow-1) < 10000) || m_bGetVoBin))
		{
			countnum++;
			double p = max((1 - sigma*sigma/mu), 0.003);
			int bin_n = min(max(vec_x(m_nRow-1) + 0.5, mu/p+0.5), 10000);
			p = mu / bin_n;
			if (m_bGetVoBin)//从界面读参数
			{
				bin_n = m_Bin_n;
				p = m_Bin_p;
			}
			m_str_FunInfo = "二项式分布\r\n";
			str_temp.Format("%d", bin_n);
			m_str_FunInfo += "试验次数 n="+str_temp + "\r\n";
			str_temp.Format("%.4f", p);
			m_str_FunInfo += "出现概率 p="+str_temp;
			m_AllFuncInfo.Add(m_str_FunInfo);
			
			CDoubleVector vec_expect(dCase + bin_n, 0);
			CDoubleVector vec_count_temp(dCase + bin_n, 0);
			CDoubleVector intvec_c(dCase + bin_n, 0);
			m_nlastcountto = -1;
			int kk = 0;
			CDoubleVector vec_crit_temp(dCase + bin_n);
			for (i=0; i<bin_n+1; i++)
			{
				vec_expect(kk) += bindist(p, bin_n, i);
				vec_count_temp(kk) += countfordis(vec_x, vec_weight, vec_index, i);
				intvec_c(kk) = i;
				if (vec_expect(kk) > lim_pro)
					kk++;
			}
			if (kk >1)//对最后一个做了处理
			{
				if (vec_expect(kk-1) < lim_pro)
				{
					vec_expect(kk-2) +=vec_expect(kk-1);
					vec_count_temp(kk-2) +=vec_count_temp(kk-1);
					intvec_c (kk-2) = max(intvec_c (kk), intvec_c(kk-1));
					kk--;
				}
			}
			CDoubleVector vec_expected(kk);
			CDoubleVector vec_res(kk);
			vec_count.create(kk);
			for (i=0; i<kk; i++)
			{
				vec_count(i) = vec_count_temp(i);
				vec_expected(i) = vec_expect(i) * 1.0 * m_totalweight;
				vec_res(i) = vec_count(i) - vec_expected(i);
			}
			CDoubleVector vec_chi = pow(vec_res, 2)/vec_expected;
			m_dChi= vec_chi.sum();
			vec_Chi2(countnum) = m_dChi;
			vec_df(countnum) = kk;
			vec_sig(countnum) = 1-chi2cdf(m_dChi, kk);
			intvec_c.resize(kk);
			intvec_c.resize(kk);
			vec_count.resize(kk);
			DrawResultBin(intvec_c, vec_count, vec_expected, vec_res, pResult);
			vec_count.create(dCase);
		}
		else if (m_bCheckBin)
		{
			countnum++;
			vec_Chi2(countnum) = 0;
			vec_df(countnum) = 0;
			vec_sig(countnum) = 0;
			m_AllFuncInfo.Add("二项式分布\r\n 参数无法计算!!");
		}
		//检验是否服从几何分布
		if (m_bCheckGeo && ((mu > 0 && mu < 10000) || m_bGetVoGeo))
		{
			countnum++;
			double p = 1/(mu+1);
			if (m_bGetVoGeo)//从界面读参数
				p = m_Geo_p;
			m_str_FunInfo = "几何分布\r\n";
			str_temp.Format("%.4f", p);
			m_str_FunInfo += "概率p="+str_temp;
			m_AllFuncInfo.Add(m_str_FunInfo);
			
			CDoubleVector vec_expect(dCase, 0);
			CDoubleVector vec_count_temp(dCase, 0);
			CDoubleVector intvec_c(dCase, 0);
			m_nlastcountto = -1;
			
			int kk = 1;
			CDoubleVector vec_crit_temp(dCase, 0);
			for (i=1; i<dCase; i++)
			{
				vec_crit_temp(kk) = int (geoinv(1.0*i/dCase, p))+1;
				if (vec_crit_temp(kk) - vec_crit_temp(kk-1) >= 1-EPS_ACCUR)
					kk++;
			}
			vec_crit.create(kk);
			vec_count.create(kk+1);
			for (i=0; i<kk; i++)
				vec_crit(i) = vec_crit_temp(i);
			if (!_CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
			{
				CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
				pPRsltElementText->SetName("警告");
				pPRsltElementText->AddString("几何分布\r\n统计频率时发生错误");
				m_pResult->Add(pPRsltElementText);
			}
			else
			{
				CDoubleVector vec_expected(kk+1);
				for (i=1; i<kk; i++)
					vec_expected(i) = geocdf(vec_crit(i), p)-geocdf(vec_crit(i-1),p);
				vec_expected(0) = geocdf(vec_crit(0), p);
				vec_expected(kk) = 1 - geocdf(vec_crit(kk-1), p);
				
				CDoubleVector vec_res = vec_count - vec_expected*m_totalweight;
				CDoubleVector vec_chi = pow(vec_res, 2)/(vec_expected*m_totalweight);
				m_dChi= vec_chi.sum();
				vec_Chi2(countnum) = m_dChi;
				vec_df(countnum) = vec_chi.vlen() - 1;
				if (vec_chi.vlen()>1)
					vec_sig(countnum) = 1-chi2cdf(m_dChi, vec_chi.vlen()-1);
				else
					vec_sig(countnum) = 0;
				vec_expected = vec_expected*m_totalweight;
				DrawResultInt(vec_crit, vec_count, vec_expected, vec_res, pResult);
			}
		}
		else if (m_bCheckGeo)
		{
			countnum++;
			vec_Chi2(countnum) = 0;
			vec_df(countnum) = 0;
			vec_sig(countnum) = 0;
			m_AllFuncInfo.Add("几何分布\r\n 参数无法计算!!");
		}
//检验是否服从负二项式分布 || 负二项式分布, 没有负二项式分布的逆函数, 暂时不算他.
			if (m_bCheckNegativBin)//负二项式分布, 没有负二项式分布的逆函数, 暂时不算他.
			{
/*					countnum++;
					double p = 1/(mu+1);
					CDoubleVector vec_expect(dCase, 0);
					CDoubleVector vec_count_temp(dCase, 0);
					CDoubleVector intvec_c(dCase, 0);
					m_nlastcountto = -1;

					int kk = 1;
					CDoubleVector vec_crit_temp(dCase, 0);
					for (i=1; i<dCase; i++)
					{
						vec_crit_temp(kk) = int (geoinv(1.0*i/dCase, p))+1;
						if (vec_crit_temp(kk) - vec_crit_temp(kk-1) >= 1-EPS_ACCUR)
							kk++;
					}
					if (vec_crit_temp(kk) == vec_crit_temp(kk-1))
						kk--;
					vec_crit.create(kk);
					for (i=0; i<kk; i++)
						vec_crit(i) = vec_crit_temp(i);//???后边有问题
					if (!_CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
						return NULL;
					CDoubleVector vec_expected(kk+1);
					for (i=1; i<kk; i++)
						vec_expected(i) = geocdf(vec_crit(i), p)-geocdf(vec_crit(i-1),p);
					vec_expected(0) = geocdf(vec_crit(0), p);
					vec_expected(kk) = 1 - geocdf(vec_crit(kk-1), p);

					CDoubleVector vec_res = vec_count - vec_expected*m_totalweight;
					CDoubleVector vec_chi = pow(vec_res, 2)/(vec_expected*m_totalweight);
					m_dChi= vec_chi.sum();
					vec_Chi2(countnum) = m_dChi;
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
*/		}
//检验是否服从Poisson分布
		if (m_bCheckPoisson && ((mu > 0.001 && mu < 1000) || m_bGetVoPoisson))
		{
			countnum++;
			double lambda = mu;
			if (m_bGetVoPoisson)
				lambda = m_Poisson_lambda;
			m_str_FunInfo = "泊松分布\r\n";
			str_temp.Format("%.4f", lambda);
			m_str_FunInfo += "lambda="+str_temp;
			m_AllFuncInfo.Add(m_str_FunInfo);
			
			CDoubleVector vec_expect(dCase, 0);
			CDoubleVector vec_count_temp(dCase, 0);
			CDoubleVector intvec_c(dCase, 0);
			m_nlastcountto = -1;
			
			int kk = 1;
			CDoubleVector vec_crit_temp(dCase, 0);
			for (i=1; i<dCase; i++)
			{
				vec_crit_temp(kk) = int (poissinv(1.0*i/dCase, lambda))+1;
				if (vec_crit_temp(kk) - vec_crit_temp(kk-1) >= 1-EPS_ACCUR)
					kk++;
			}
			vec_crit.create(kk);
			vec_count.create(kk+1);
			for (i=0; i<kk; i++)
				vec_crit(i) = vec_crit_temp(i);
			if (!_CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
			{
				CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
				pPRsltElementText->SetName("警告");
				pPRsltElementText->AddString("泊松分布\r\n统计频率时发生错误");
				m_pResult->Add(pPRsltElementText);
			}
			else
			{
				CDoubleVector vec_expected(kk+1);
				for (i=1; i<kk; i++)
					vec_expected(i) = poisscdf(vec_crit(i), lambda)-poisscdf(vec_crit(i-1),lambda);
				vec_expected(0) = poisscdf(vec_crit(0), lambda);
				vec_expected(kk) = 1 - poisscdf(vec_crit(kk-1), lambda);
				
				CDoubleVector vec_res = vec_count - vec_expected*m_totalweight;
				CDoubleVector vec_chi = pow(vec_res, 2)/(vec_expected*m_totalweight);
				m_dChi= vec_chi.sum();
				vec_Chi2(countnum) = m_dChi;
				vec_df(countnum) = vec_chi.vlen() - 1;
				if (vec_chi.vlen()>1)
					vec_sig(countnum) = 1-chi2cdf(m_dChi, vec_chi.vlen()-1);
				else
					vec_sig(countnum) = 0;
				vec_expected = vec_expected*m_totalweight;
				DrawResultInt(vec_crit, vec_count, vec_expected, vec_res, pResult);
			}
		}
		else if (m_bCheckPoisson)
		{
			countnum++;
			vec_Chi2(countnum) = 0;
			vec_df(countnum) = 0;
			vec_sig(countnum) = 0;
			m_AllFuncInfo.Add("泊松分布\r\n 参数无法计算!!");
		}
		//检验是否出现频率相同
		if (m_bCheckFreq)
		{
			countnum++;
			CDoubleVector vec_crit_output(vec_x.vlen());
			int j=0;
			double fTmp = vec_x(0);
			vec_crit_output(0) = fTmp;
			CDoubleVector vec_frq(vec_x.vlen());
			if (m_bWithWeight)
			{
				for (i=0;i<vec_x.vlen();i++)
				{
					if (fTmp-vec_x(i) < -EPS_ACCUR || fTmp-vec_x(i) > EPS_ACCUR)//fTemp != vec_x(i)
					{
						fTmp = vec_x(i);
						vec_frq(++j) = vec_weight(vec_index(i)-1);
						vec_crit_output(j) = fTmp;
					}
					else
					{
						vec_frq(j) += vec_weight(vec_index(i)-1);
					}
				}
			} 
			else
			{					
				for (i=0;i<vec_x.vlen();i++)
				{
					if (fTmp-vec_x(i) < -EPS_ACCUR || fTmp-vec_x(i) > EPS_ACCUR)//fTemp != vec_x(i)
					{
						fTmp = vec_x(i);
						vec_frq(++j) = 1;
						vec_crit_output(j) = fTmp;
					}
					else
					{
						vec_frq(j) += 1;
					}
				}
			}
			int FreqCase = j+1;
			vec_frq.resize(FreqCase);
			vec_crit_output.resize(FreqCase);
			CDoubleVector vec_expect(FreqCase, (m_totalweight/FreqCase));
			CDoubleVector vec_res = vec_frq - vec_expect;
			CDoubleVector vec_chi = pow(vec_res, 2)/vec_expect;
			m_dChi= vec_chi.sum();
			vec_Chi2(countnum) = m_dChi;
			vec_df(countnum) = FreqCase - 1;
			vec_sig(countnum) = 1-chi2cdf(m_dChi, FreqCase-1);
			CTString str;
			str.Format("%d", FreqCase);
			m_str_FunInfo = "出现频率相同 共" + str + "类";
			m_AllFuncInfo.Add(m_str_FunInfo);
			if (FreqCase >= m_nRow)
				m_str_FunInfo += "\r\n 注意：每个观测值出现都仅出现一次";
			DrawResultFreq(vec_crit_output, vec_frq, vec_expect, vec_res, pResult);
		}
//由用户输入离散的给定概率的分布
		if (m_bCheckDiscrib && vec_y_dis.vlen()==vec_y_prob.vlen() && vec_y_dis.vlen() > 1)
		{
			countnum++;
			int CaseFr = vec_y_dis.vlen();
			CDoubleVector vec_frq(CaseFr);
			m_nlastcountto = -1;
			CDoubleVector vec_expect(CaseFr);
			for (i=0; i<CaseFr; i++)
			{
				vec_frq(i) = countfordis(vec_x, vec_weight, vec_index, vec_y_dis(i));
				vec_expect(i) = vec_y_prob(i)*m_totalweight;
			}
			CDoubleVector vec_res = vec_frq - vec_expect;
			CDoubleVector vec_chi = pow(vec_res, 2)/vec_expect;
			m_dChi= vec_chi.sum();
			vec_Chi2(countnum) = m_dChi;
			vec_df(countnum) = CaseFr - 1;
			vec_sig(countnum) = 1-chi2cdf(m_dChi, CaseFr-1);
			m_str_FunInfo = "用户输入离散变量分布";
			m_AllFuncInfo.Add(m_str_FunInfo);
			DrawResultFreq(vec_y_dis, vec_frq, vec_expect, vec_res, pResult);
		}
		else if (m_bCheckDiscrib)
		{
			countnum++;
			vec_Chi2(countnum) = 0;
			vec_df(countnum) = 0;
			vec_sig(countnum) = 0;
			m_AllFuncInfo.Add("用户输入分布值过少\r\n 无法计算!!");
		}
//离散型的停止
// 
// 
// 
//连续型的开始
			vec_count.create(dCase);
//检验是否服从正态分布
			if (m_bCheckNormal) 
			{
				countnum++;
				vec_crit.create(dCase-1);
				vec_count.create(dCase);
				double mu_nor = mu;
				double sigma_nor = sigma;
				if (m_bGetVoNormal)
				{
					mu_nor = m_Normal_mu;
					sigma_nor = m_Normal_sigma;
				}
				m_str_FunInfo = "正态分布\r\n";
				str_temp.Format("%.4f", mu_nor);
				m_str_FunInfo += "均值="+str_temp + "\r\n";
				str_temp.Format("%.4f", sigma_nor);
				m_str_FunInfo += "方差="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i-1) = norminv(1.0*i/dCase,mu_nor, sigma_nor);
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
					pPRsltElementText->SetName("警告");
					pPRsltElementText->AddString("正态分布\r\n统计频率时发生错误");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					CDoubleVector vec_expect(dCase, (m_totalweight/dCase));
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi= vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi;
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
				}
			}
//检验是否服从指数分布
			if (m_bCheckExp)
			{
				countnum++;
				vec_crit.create(dCase);
				vec_count.create(dCase+1);
				vec_crit(0) = mu - sigma;
				double lambda = sigma;
				if (m_bGetVoExp)
					lambda = 1/m_Exp_lambda;
				m_str_FunInfo = "指数分布\r\n";
				str_temp.Format("%.4f", 1/lambda);
				m_str_FunInfo += "lambda="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i) = expinv(1.0*i/dCase, lambda) + (mu - sigma);
				for (i=1; i<dCase; i++)
				{	
					if ((vec_crit(i)-vec_crit(i-1)) < 0)
						vec_crit(i) = (vec_crit(i-1) + vec_crit(i+1))/2;
				}
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
					pPRsltElementText->SetName("警告");
					pPRsltElementText->AddString("指数分布\r\n统计频率时发生错误");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					CDoubleVector vec_expect(dCase+1, (m_totalweight/dCase));
					vec_expect(0) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi + 4.0*vec_count(0)/(m_nRow/dCase);//由于exp没有负数的特性,对chi2作出修正
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
//检验是否服从对数正态分布
			if (m_bCheckLogNormal /*&& m_bGetVoLogNormal*/)//需要从外界读取参数mu, sigma
			{
				countnum++;
				vec_crit.create(dCase);
				vec_count.create(dCase+1);
				vec_crit(0) = m_lognormalmu;
				m_str_FunInfo = "对数正态分布\r\n";
				str_temp.Format("%.4f", m_lognormalmu);
				m_str_FunInfo += "正态分布均值="+str_temp + "\r\n";
				str_temp.Format("%.4f", m_lognormalsigma);
				m_str_FunInfo += "正态分布方差="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i) = logninv(1.0*i/dCase,m_lognormalmu, m_lognormalsigma);
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
					pPRsltElementText->SetName("警告");
					pPRsltElementText->AddString("对数正态分布\r\n统计频率时发生错误");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					
					CDoubleVector vec_expect(dCase+1, (m_totalweight/dCase));
					vec_expect(0) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi + 4.0*vec_count(0)/(m_nRow/dCase);//由于exp没有负数的特性,对chi2作出修正
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
//检验是否服从Gamma分布 && gaminv函数有问题
			if (m_bCheckGam && (mu > 0.001|| m_bGetVoGam))
			{
				countnum++;
				vec_crit.create(dCase);
				vec_count.create(dCase+1);
				double alpha_gam = mu*mu/sigma/sigma;
				double beta_gam = (sigma*sigma)/mu;
				if (m_bGetVoGam)
				{
					alpha_gam = m_Gam_alpha;
					beta_gam = 1/m_Gam_beta;
				}
				m_str_FunInfo = "Gamma分布\r\n";
				str_temp.Format("%.4f", alpha_gam);
				m_str_FunInfo += "α="+str_temp + "\r\n";
				str_temp.Format("%.4f", 1/beta_gam);
				m_str_FunInfo += "β="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i) = gaminv(1.0*i/dCase, alpha_gam, beta_gam);//gaminv函数有问题
				vec_crit(0) = 0;
				for (i=1; i<dCase; i++)
				{	if ((vec_crit(i)-vec_crit(i-1)) < 0  && ((i+1)<dCase))
						vec_crit(i) = (vec_crit(i-1) + vec_crit(i+1))/2;
				}
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
					pPRsltElementText->SetName("警告");
					pPRsltElementText->AddString("Gamma分布\r\n统计频率时发生错误");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					CDoubleVector vec_expect(dCase+1, (m_totalweight/dCase));
					vec_expect(0) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi + 4.0*vec_count(0)/(m_nRow/dCase);//由于Gam没有负数的特性,对chi2作出修正
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
			else if (m_bCheckGam)
			{
				countnum++;
				vec_Chi2(countnum) = 0;
				vec_df(countnum) = 0;
				vec_sig(countnum) = 0;
				m_AllFuncInfo.Add("Gamma分布\r\n 参数无法计算!!");
			}

//检验是否服从半正态分布
			if (m_bCheckHalfNor)
			{
				countnum++;
				vec_crit.create(dCase);
				vec_count.create(dCase+1);
				double sigma_hn = sigma;
				if (m_bGetVoHalfNor)
					sigma_hn = m_HalfNor_sigma;
				m_str_FunInfo = "半正态分布\r\n";
				double mu_hn = 0.0;
				double theta = sqrt((pi-1)/2)/sigma_hn;
				str_temp.Format("%.4f", mu_hn);
				m_str_FunInfo += "原始正态均值="+str_temp + "\r\n";
				str_temp.Format("%.4f", sigma_hn);
				m_str_FunInfo += "原始正态方差="+str_temp + "\r\n";
				str_temp.Format("%.4f", mu - 1/theta);
				m_str_FunInfo += "分布平移" + str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i) = halfninv(1.0*i/dCase, theta) + (mu - 1/theta);
				vec_crit(0) = mu - 1/theta;
				for (i=1; i<dCase-1; i++)
				{	
					if ((vec_crit(i)-vec_crit(i-1)) < 0)
						vec_crit(i) = (vec_crit(i-1) + vec_crit(i+1))/2;
				}
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
					pPRsltElementText->SetName("警告");
					pPRsltElementText->AddString("半正态分布\r\n统计频率时发生错误");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					CDoubleVector vec_expect(dCase+1, (m_totalweight/dCase));
					vec_expect(0) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi + 4.0*vec_count(0)/(m_nRow/dCase);//由于半正态没有负数的特性,对chi2作出修正
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
//检验是否服从逻辑分布
			if (m_bCheckLogist) 
			{
				countnum++;
				vec_crit.create(dCase-1);
				vec_count.create(dCase);
				double m = mu;
				double b = sigma*sqrt(3)/pi;
				if (m_bGetVoLogist)
				{
					m = m_Logist_m;
					b = m_Logist_b;
				}
				m_str_FunInfo = "逻辑分布\r\n";
				str_temp.Format("%.4f", m);
				m_str_FunInfo += "m="+str_temp + "\r\n";
				str_temp.Format("%.4f", b);
				m_str_FunInfo += "b="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i-1) = logistinv(1.0*i/dCase, m, b);
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
					pPRsltElementText->SetName("警告");
					pPRsltElementText->AddString("逻辑分布\r\n统计频率时发生错误");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					CDoubleVector vec_expect(dCase, (m_totalweight/dCase));
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi= vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi;
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
				}
			}
//检验是否服从均匀分布
			if (m_bCheckAverage)
			{
				countnum++;
				double range_len = (vec_x(int(m_nRow*0.75-0.5))-vec_x(int(m_nRow*0.25-0.5)))*2/dCase;
				vec_crit.create(dCase+1);
				vec_count.create(dCase+2);
				double Average_up = mu + dCase/2*range_len;
				double Average_low = mu - dCase/2*range_len;
				if (((Average_low - sqrt(dCase)/2*range_len) < vec_x(int(sqrt(dCase)))) && ((Average_up + sqrt(dCase)/2*range_len) > vec_x(m_nRow -1 - int(sqrt(dCase)))))
				{
					Average_up = vec_x(m_nRow -1 - int(sqrt(dCase)));
					Average_low = vec_x(int(sqrt(dCase)));
				}	
				if (m_bGetVoAverage)
				{
					Average_up = m_Average_up;
					Average_low = m_Average_low;
				}
				for (i=1; i<dCase+2; i++)
					vec_crit(i-1) = Average_low + (i-1.0)/dCase*(Average_up - Average_low);
				
				m_str_FunInfo = "均匀分布\r\n";
				str_temp.Format("%.4f", Average_up);
				m_str_FunInfo += "分布上界="+str_temp + "\r\n";
				str_temp.Format("%.4f", Average_low);
				m_str_FunInfo += "分布下界="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("警告");
					pPRsltElementText->SetName("警告");
					pPRsltElementText->AddString("均匀分布\r\n统计频率时发生错误");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					
					CDoubleVector vec_expect(dCase+2, (m_totalweight/dCase));
					vec_expect(0) = 0;
					vec_expect(dCase+1) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/(dCase));
					vec_Chi2(countnum) = m_dChi + 4.0*(vec_count(0)+vec_count(dCase+1))/(m_nRow/(dCase));//由于uniform没有大于b,小于a的数的特性,对chi2作出修正
					vec_df(countnum) = dCase;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
			DrawOutput(vec_Chi2, vec_df, vec_sig, pResult);
			//tTemp.Toc("结束");
//型的停止
			//tTemp.Toc("总结果");
		}//INDEX的括号
		//释放内存
		vec_y_dis.destroy();//用户输入的离散变量观察值
		vec_y_prob.destroy();//用户输入的离散变量出现的预期概率
	return pResult;
}

void CChiSquare::DrawOutput(CDoubleVector vec_Chi2, CDoubleVector vec_df, CDoubleVector vec_sig, CResult *pResult)
{
	CTLTable *pTable11 = NULL;
	CRsltElementTable *pETable11=NULL;
	
	pTable11 = new CTLTable;
	pTable11->CreateTable(2, 2);
	pTable11->SetTitle(m_str_x + "  检验分析结果");
	pTable11->SetRows(2);
	pTable11->SetCols(2);
	pTable11->InsertColumn(0, "检验 类型");
	pTable11->InsertColumn(1, "卡方值");
	pTable11->InsertColumn(2, "自由度");
	pTable11->InsertColumn(3, "P 值");

	int i = 0;
	int length = vec_sig.vlen();
	CTString str;
	CTString strValue;

//对输出结果进行排序	始
	CIntVector vec_index_sig;
	CIntVector vec_index_chi;
	CIntVector vec_index(length);
	CDoubleVector vec_sig_copy = vec_sig.copy();
	CDoubleVector vec_df_copy = vec_df.copy();
	vec_sig_copy.Sort(vec_index_sig, 1);
	for (i=0; i<length; i++)
	{
		if (vec_sig_copy(i)>0.00000001)
			vec_index(i) = vec_index_sig(i);
		else
			break;
	}
	if (i<length)
	{
		CDoubleVector vec_Chi2_copy(length - i);
		int j;
		for (j=i; j<length; j++)
		{
			if (vec_df(vec_index_sig(j)-1) != 0)
			{
				vec_Chi2_copy(j-i) = vec_Chi2(vec_index_sig(j)-1)/vec_df(vec_index_sig(j)-1);
			}
			else
				vec_Chi2_copy(j-i) = 1e300;
		}
		vec_Chi2_copy.Sort(vec_index_chi);
		for (j=i; j<length; j++)
		{
			vec_index(j) = vec_index_sig(vec_index_chi(j-i)+i-1);
		}
	}
//对输出结果进行排序	止

	for(i=0; i<length; i++)
	{
		str.Format("%d", i + 1);
		pTable11->InsertItem(i, m_AllFuncInfo[vec_index(i)-1]);
		pTable11->SetItemText(i, 1, vec_Chi2(vec_index(i)-1));
		pTable11->SetItemText(i, 2, vec_df(vec_index(i)-1));
		pTable11->SetItemText(i, 3, vec_sig(vec_index(i)-1));
	}
	pETable11 = new CRsltElementTable(m_str_x + "  检验分析结果", pTable11);//申请一个表对象类的对象
	pResult->Add(pETable11);
	return;
}