// ChiSquare.cpp: implementation of the CChiSquare class.
//				��������
//					����ׯ    2006/03/15
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
	m_bWithWeight = FALSE;//�Ƿ���Ȩ��
	m_bOutputFreqTable = FALSE;//�Ƿ����Ƶ�ʷ�����ϸ���

	m_bCheckDiscrib = FALSE;//���û�������ɢ�ĸ������ʵķֲ�

	m_bCheckBin = TRUE;
	m_Bin_n = 1;//>=1;
	m_Bin_p = 0.5;//0.001< <0.999
	m_bGetVoBin = FALSE;
	m_bCheckNegativBin = FALSE;//������ʽ�ֲ�, û�и�����ʽ�ֲ����溯��, ��ʱ������.
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
	if(m_bWithWeight)//���Ȩ�ر����±�
       arCol.Add(m_nWeight);
	m_nRow = pDataInterface->GetColsData(arCol, Matrix_Data, 0);//ȡ������
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
	
	if (vec_x(nVxInd) >= nVtData-EPS_ACCUR)//�ҵ�����
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
	if (vec_x(nVxInd) < nVtData-EPS_ACCUR) //����С�ڵ�ǰֵ
	{
		while (nVxInd < nVxLen && vec_x(nVxInd) < nVtData-EPS_ACCUR)
		{
			nVxInd++;
		}
	}
	else                         //���˵��ڵ�ǰֵ      
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
	nStep_g = max(m_nRow/nCase/4, 3); //ǰ���Ĳ���
	nStep_b = max(m_nRow/nCase/24, 2);//���˵Ĳ���
	if (nCase < 1 ) return FALSE;
	
	nStartData = vec_crit(0);
	for (i=0; i<nCase; i++) 
	{
		if (nStart >= nLen)//�������
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
	//ѭ��������β��ȫ��������һ������-�ó�����Ϊ�˼����߼��ж�
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
	double fVtData = vec_x(nVxInd);//��ֵ
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
	while (nVxInd < nVxLen && //����С�ڻ���ڵ�ǰֵ
		   vec_x(nVxInd) <= fVtData+EPS_ACCUR)
	{
		nVxInd++;
	}
	return nVxInd;
}

//ͳ��Ƶ��
BOOL CChiSquare::CalcFreq(CDoubleVector & vec_x, CDoubleVector & vec_w, CIntVector & vec_index, CDoubleMatrix &frq_matrix)
{
	int nLen=0, nRow=0, nStart=0, nEnd=0, nStep_g=0, nStep_b=0;
	
	nLen    = vec_x.vlen();
	if (m_nRow < 100)
	{
		nStep_g = 2; //ǰ���Ĳ���
		nStep_b = 1; //���˵Ĳ���
	}
	else
	if (m_nRow < 1000)
	{
		nStep_g = max(sqrt(m_nRow)/6, 3); //ǰ���Ĳ���
		nStep_b = max(sqrt(m_nRow)/8, 2); //���˵Ĳ���
	}
	else
	if (m_nRow < 5000)
	{
		nStep_g = max(sqrt(m_nRow)/10, 5); //ǰ���Ĳ���
		nStep_b = max(sqrt(m_nRow)/15, 3); //���˵Ĳ���
	}
	else
	{
		nStep_g = max(sqrt(m_nRow)/20, 8); //ǰ���Ĳ���
		nStep_b = max(sqrt(m_nRow)/30, 4); //���˵Ĳ���
	}
	//�ⲿû�з���ռ�
	if (frq_matrix.mrow()<1 || frq_matrix.mcol()<1)
		frq_matrix.create(nLen,2);
	//��Ȩ�أ���Ҫѭ����ͬʱ�����߼��жϣ����Էֿ�����
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
	}//ѭ������
	frq_matrix.rerowsize(nRow);
	return TRUE;
}
//vec_x:�����������fPercent:��Ҫ�����İٷ���
void CChiSquare::ReSize(CDoubleVector & vec_x,double fPercent)
{

	int nRow     = vec_x.vlen();
	int nPercent = nRow * fPercent/100.0; 
	if (nPercent<=0 || nPercent>=nRow)
		return;
	for (int i=nPercent/2;i<nRow-nPercent/2;i++)//����ż��λ��
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
	for (int i=nPercent/2;i<nRow-nPercent/2;i++)//����ż��λ��
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
	pTable11->InsertColumn(0, "���");
	pTable11->InsertColumn(1, "�۲�����");
	pTable11->InsertColumn(2, "�۲�Ƶ��");
	pTable11->InsertColumn(3, "����Ƶ��");
	pTable11->InsertColumn(4, "�в�");
	
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
			strValue = "-�� ~ " + str;
		}
		else if(i == vec_crit.vlen())
		{
			str.Format("%.4f", vec_crit(vec_crit.vlen()-1));
			strValue = str + " ~ +��";
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
	pTable11->SetItemText(i, 1, "�ܼ�");
	pTable11->SetItemText(i, 2, sum(vec_count));
	pTable11->SetItemText(i, 3, vec_expect.sum());
	
	pETable11 = new CRsltElementTable(m_str_x+" "+m_str_FunInfo, pTable11);//����һ���������Ķ���
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
	pTable11->InsertColumn(0, "���");
	pTable11->InsertColumn(1, "�۲�����");
	pTable11->InsertColumn(2, "�۲�Ƶ��");
	pTable11->InsertColumn(3, "����Ƶ��");
	pTable11->InsertColumn(4, "�в�");
	
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
			strValue = "-�� ~ " + str;
		}
		else if(i == vec_crit.vlen())
		{
			str.Format("%.0f", vec_crit(vec_crit.vlen()-1)+1);
			strValue = str + " ~ +��";
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
	pTable11->SetItemText(i, 1, "�ܼ�");
	pTable11->SetItemText(i, 2, sum(vec_count));
	pTable11->SetItemText(i, 3, vec_expect.sum());
	
	pETable11 = new CRsltElementTable(m_str_x+" "+m_str_FunInfo, pTable11);//����һ���������Ķ���
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
	pTable11->InsertColumn(0, "���");
	pTable11->InsertColumn(1, "�۲�����");
	pTable11->InsertColumn(2, "�۲�Ƶ��");
	pTable11->InsertColumn(3, "����Ƶ��");
	pTable11->InsertColumn(4, "�в�");
	
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
			strValue = str + " ~ +��";
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
	pTable11->SetItemText(i, 1, "�ܼ�");
	pTable11->SetItemText(i, 2, sum(vec_count));
	pTable11->SetItemText(i, 3, vec_expect.sum());
	
	pETable11 = new CRsltElementTable(m_str_x+" "+m_str_FunInfo, pTable11);//����һ���������Ķ���
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
	pTable11->InsertColumn(0, "���");
	pTable11->InsertColumn(1, "�۲�����");
	pTable11->InsertColumn(2, "�۲�Ƶ��");
	pTable11->InsertColumn(3, "����Ƶ��");
	pTable11->InsertColumn(4, "�в�");
	if (vec_crit.vlen() > 10000)//Ƶ�ʵ�������࣬����10000�������Ƶ�ʷ�����
	{
		pTable11->InsertItem(1, "�۲�ֵ���� \r\n����10000��\r\n�����Ƶ�ʷ�����");
		pETable11 = new CRsltElementTable(m_str_x+" "+"ע�� "+m_str_FunInfo, pTable11);//����һ���������Ķ���
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
	pTable11->SetItemText(i, 1, "�ܼ�");
	pTable11->SetItemText(i, 2, sum(vec_count));
	pTable11->SetItemText(i, 3, vec_expect.sum());

	pETable11 = new CRsltElementTable(m_str_x+" "+m_str_FunInfo, pTable11);//����һ���������Ķ���
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
//��ȡ�Ƿ����ֲ��Ĳ���
////////////////////////////////////////////////////////////////////////////

//��̬
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
//���ȷֲ�
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
//ָ���ֲ�
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
//	 Gamma�ֲ�
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
//	����̬�ֲ�
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
//�߼�
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
//������̬
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
//��ɢ��
//////////////////////////////////////////////////////////////////////////

//����ʽ
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
//���ηֲ�
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
//����
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
//���ָ�����ͬ
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
	pResult=new CResult("��������");
	m_pResult = pResult;
	
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
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
//��ÿһ��ѡ��ı������з�������
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
//��ȡ����
		if (!GetData(pDataInterface, data_read, index))
		{
			CRsltElementText * pPRsltElementText=new CRsltElementText("����");
			pPRsltElementText->SetName("����");
			pPRsltElementText->AddString("���ݶ�ȡ��������\r\n");
			m_pResult->Add(pPRsltElementText);
			return pResult;
		}
		if (m_nRow < 7)
		{
			CRsltElementText * pPRsltElementText=new CRsltElementText("����");
			pPRsltElementText->SetName("����");
			pPRsltElementText->AddString("�۲����̫��\r\n�޷�����");
			m_pResult->Add(pPRsltElementText);
			return pResult;
		}
//�����ݽ���Ԥ����: ����, ���ֵ, �����
		int dCase = min(pow(m_nRow, 1/4.0)*10, sqrt(m_nRow*1.0))+3;//�ֳ�dCase��
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
					CTString strWarning = "Ȩ�س��ָ��������ܼ��㣡";
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
					return m_pResult;
				}
		}
		double mu;
		double sigma;
		//tTemp.Toc("׼��");
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
//�����Ƕ�10���ֲַ���Chi2ֵ�ĳ��� 
//��ʮ����ֲ��Ĵ���˼·�� ��-ͬ. С-��. ���ǳ��򼸺��޷��ϲ�
/******************************************************
*	������Щ�ֲ�ʱ, ��3��, 
*	��0��, ����ֲ��Ĳ���, �Լ���Ҫ���ֳɵ�����
*	��һ��, �����������Ļ��ֵ��ٽ�ֵ, �Լ�Ԥ�ڵĸ��ʵ�
*	�ڶ���, �������������߳��ֵĹ۲�ĸ���
*	������, ����Chi2��ֵ
******************************************************/

//��ɢ�͵Ŀ�ʼ
		
		double lim_pro = max(0.3 / dCase, 0.1/sqrt(dCase));
		
//�����Ƿ����Bin�ֲ�
		if (m_bCheckBin && mu > 0 && (((1 - sigma*sigma/mu) > 0.001 && vec_x(m_nRow-1) < 10000) || m_bGetVoBin))
		{
			countnum++;
			double p = max((1 - sigma*sigma/mu), 0.003);
			int bin_n = min(max(vec_x(m_nRow-1) + 0.5, mu/p+0.5), 10000);
			p = mu / bin_n;
			if (m_bGetVoBin)//�ӽ��������
			{
				bin_n = m_Bin_n;
				p = m_Bin_p;
			}
			m_str_FunInfo = "����ʽ�ֲ�\r\n";
			str_temp.Format("%d", bin_n);
			m_str_FunInfo += "������� n="+str_temp + "\r\n";
			str_temp.Format("%.4f", p);
			m_str_FunInfo += "���ָ��� p="+str_temp;
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
			if (kk >1)//�����һ�����˴���
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
			m_AllFuncInfo.Add("����ʽ�ֲ�\r\n �����޷�����!!");
		}
		//�����Ƿ���Ӽ��ηֲ�
		if (m_bCheckGeo && ((mu > 0 && mu < 10000) || m_bGetVoGeo))
		{
			countnum++;
			double p = 1/(mu+1);
			if (m_bGetVoGeo)//�ӽ��������
				p = m_Geo_p;
			m_str_FunInfo = "���ηֲ�\r\n";
			str_temp.Format("%.4f", p);
			m_str_FunInfo += "����p="+str_temp;
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
				CRsltElementText * pPRsltElementText=new CRsltElementText("����");
				pPRsltElementText->SetName("����");
				pPRsltElementText->AddString("���ηֲ�\r\nͳ��Ƶ��ʱ��������");
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
			m_AllFuncInfo.Add("���ηֲ�\r\n �����޷�����!!");
		}
//�����Ƿ���Ӹ�����ʽ�ֲ� || ������ʽ�ֲ�, û�и�����ʽ�ֲ����溯��, ��ʱ������.
			if (m_bCheckNegativBin)//������ʽ�ֲ�, û�и�����ʽ�ֲ����溯��, ��ʱ������.
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
						vec_crit(i) = vec_crit_temp(i);//???���������
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
//�����Ƿ����Poisson�ֲ�
		if (m_bCheckPoisson && ((mu > 0.001 && mu < 1000) || m_bGetVoPoisson))
		{
			countnum++;
			double lambda = mu;
			if (m_bGetVoPoisson)
				lambda = m_Poisson_lambda;
			m_str_FunInfo = "���ɷֲ�\r\n";
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
				CRsltElementText * pPRsltElementText=new CRsltElementText("����");
				pPRsltElementText->SetName("����");
				pPRsltElementText->AddString("���ɷֲ�\r\nͳ��Ƶ��ʱ��������");
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
			m_AllFuncInfo.Add("���ɷֲ�\r\n �����޷�����!!");
		}
		//�����Ƿ����Ƶ����ͬ
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
			m_str_FunInfo = "����Ƶ����ͬ ��" + str + "��";
			m_AllFuncInfo.Add(m_str_FunInfo);
			if (FreqCase >= m_nRow)
				m_str_FunInfo += "\r\n ע�⣺ÿ���۲�ֵ���ֶ�������һ��";
			DrawResultFreq(vec_crit_output, vec_frq, vec_expect, vec_res, pResult);
		}
//���û�������ɢ�ĸ������ʵķֲ�
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
			m_str_FunInfo = "�û�������ɢ�����ֲ�";
			m_AllFuncInfo.Add(m_str_FunInfo);
			DrawResultFreq(vec_y_dis, vec_frq, vec_expect, vec_res, pResult);
		}
		else if (m_bCheckDiscrib)
		{
			countnum++;
			vec_Chi2(countnum) = 0;
			vec_df(countnum) = 0;
			vec_sig(countnum) = 0;
			m_AllFuncInfo.Add("�û�����ֲ�ֵ����\r\n �޷�����!!");
		}
//��ɢ�͵�ֹͣ
// 
// 
// 
//�����͵Ŀ�ʼ
			vec_count.create(dCase);
//�����Ƿ������̬�ֲ�
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
				m_str_FunInfo = "��̬�ֲ�\r\n";
				str_temp.Format("%.4f", mu_nor);
				m_str_FunInfo += "��ֵ="+str_temp + "\r\n";
				str_temp.Format("%.4f", sigma_nor);
				m_str_FunInfo += "����="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i-1) = norminv(1.0*i/dCase,mu_nor, sigma_nor);
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("����");
					pPRsltElementText->SetName("����");
					pPRsltElementText->AddString("��̬�ֲ�\r\nͳ��Ƶ��ʱ��������");
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
//�����Ƿ����ָ���ֲ�
			if (m_bCheckExp)
			{
				countnum++;
				vec_crit.create(dCase);
				vec_count.create(dCase+1);
				vec_crit(0) = mu - sigma;
				double lambda = sigma;
				if (m_bGetVoExp)
					lambda = 1/m_Exp_lambda;
				m_str_FunInfo = "ָ���ֲ�\r\n";
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
					CRsltElementText * pPRsltElementText=new CRsltElementText("����");
					pPRsltElementText->SetName("����");
					pPRsltElementText->AddString("ָ���ֲ�\r\nͳ��Ƶ��ʱ��������");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					CDoubleVector vec_expect(dCase+1, (m_totalweight/dCase));
					vec_expect(0) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi + 4.0*vec_count(0)/(m_nRow/dCase);//����expû�и���������,��chi2��������
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
//�����Ƿ���Ӷ�����̬�ֲ�
			if (m_bCheckLogNormal /*&& m_bGetVoLogNormal*/)//��Ҫ������ȡ����mu, sigma
			{
				countnum++;
				vec_crit.create(dCase);
				vec_count.create(dCase+1);
				vec_crit(0) = m_lognormalmu;
				m_str_FunInfo = "������̬�ֲ�\r\n";
				str_temp.Format("%.4f", m_lognormalmu);
				m_str_FunInfo += "��̬�ֲ���ֵ="+str_temp + "\r\n";
				str_temp.Format("%.4f", m_lognormalsigma);
				m_str_FunInfo += "��̬�ֲ�����="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i) = logninv(1.0*i/dCase,m_lognormalmu, m_lognormalsigma);
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("����");
					pPRsltElementText->SetName("����");
					pPRsltElementText->AddString("������̬�ֲ�\r\nͳ��Ƶ��ʱ��������");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					
					CDoubleVector vec_expect(dCase+1, (m_totalweight/dCase));
					vec_expect(0) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi + 4.0*vec_count(0)/(m_nRow/dCase);//����expû�и���������,��chi2��������
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
//�����Ƿ����Gamma�ֲ� && gaminv����������
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
				m_str_FunInfo = "Gamma�ֲ�\r\n";
				str_temp.Format("%.4f", alpha_gam);
				m_str_FunInfo += "��="+str_temp + "\r\n";
				str_temp.Format("%.4f", 1/beta_gam);
				m_str_FunInfo += "��="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i) = gaminv(1.0*i/dCase, alpha_gam, beta_gam);//gaminv����������
				vec_crit(0) = 0;
				for (i=1; i<dCase; i++)
				{	if ((vec_crit(i)-vec_crit(i-1)) < 0  && ((i+1)<dCase))
						vec_crit(i) = (vec_crit(i-1) + vec_crit(i+1))/2;
				}
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("����");
					pPRsltElementText->SetName("����");
					pPRsltElementText->AddString("Gamma�ֲ�\r\nͳ��Ƶ��ʱ��������");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					CDoubleVector vec_expect(dCase+1, (m_totalweight/dCase));
					vec_expect(0) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi + 4.0*vec_count(0)/(m_nRow/dCase);//����Gamû�и���������,��chi2��������
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
				m_AllFuncInfo.Add("Gamma�ֲ�\r\n �����޷�����!!");
			}

//�����Ƿ���Ӱ���̬�ֲ�
			if (m_bCheckHalfNor)
			{
				countnum++;
				vec_crit.create(dCase);
				vec_count.create(dCase+1);
				double sigma_hn = sigma;
				if (m_bGetVoHalfNor)
					sigma_hn = m_HalfNor_sigma;
				m_str_FunInfo = "����̬�ֲ�\r\n";
				double mu_hn = 0.0;
				double theta = sqrt((pi-1)/2)/sigma_hn;
				str_temp.Format("%.4f", mu_hn);
				m_str_FunInfo += "ԭʼ��̬��ֵ="+str_temp + "\r\n";
				str_temp.Format("%.4f", sigma_hn);
				m_str_FunInfo += "ԭʼ��̬����="+str_temp + "\r\n";
				str_temp.Format("%.4f", mu - 1/theta);
				m_str_FunInfo += "�ֲ�ƽ��" + str_temp;
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
					CRsltElementText * pPRsltElementText=new CRsltElementText("����");
					pPRsltElementText->SetName("����");
					pPRsltElementText->AddString("����̬�ֲ�\r\nͳ��Ƶ��ʱ��������");
					m_pResult->Add(pPRsltElementText);
				}
				else
				{
					CDoubleVector vec_expect(dCase+1, (m_totalweight/dCase));
					vec_expect(0) = 0;
					CDoubleVector vec_res = vec_count - vec_expect;
					CDoubleVector vec_chi = pow(vec_res, 2);
					m_dChi = vec_chi.sum()/(m_totalweight/dCase);
					vec_Chi2(countnum) = m_dChi + 4.0*vec_count(0)/(m_nRow/dCase);//���ڰ���̬û�и���������,��chi2��������
					vec_df(countnum) = dCase - 1;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase-1);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
//�����Ƿ�����߼��ֲ�
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
				m_str_FunInfo = "�߼��ֲ�\r\n";
				str_temp.Format("%.4f", m);
				m_str_FunInfo += "m="+str_temp + "\r\n";
				str_temp.Format("%.4f", b);
				m_str_FunInfo += "b="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				for (i=1; i<dCase; i++)
					vec_crit(i-1) = logistinv(1.0*i/dCase, m, b);
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("����");
					pPRsltElementText->SetName("����");
					pPRsltElementText->AddString("�߼��ֲ�\r\nͳ��Ƶ��ʱ��������");
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
//�����Ƿ���Ӿ��ȷֲ�
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
				
				m_str_FunInfo = "���ȷֲ�\r\n";
				str_temp.Format("%.4f", Average_up);
				m_str_FunInfo += "�ֲ��Ͻ�="+str_temp + "\r\n";
				str_temp.Format("%.4f", Average_low);
				m_str_FunInfo += "�ֲ��½�="+str_temp;
				m_AllFuncInfo.Add(m_str_FunInfo);
				
				if (!CountFreq(vec_x, vec_weight, vec_index, vec_crit, vec_count))
				{
					CRsltElementText * pPRsltElementText=new CRsltElementText("����");
					pPRsltElementText->SetName("����");
					pPRsltElementText->AddString("���ȷֲ�\r\nͳ��Ƶ��ʱ��������");
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
					vec_Chi2(countnum) = m_dChi + 4.0*(vec_count(0)+vec_count(dCase+1))/(m_nRow/(dCase));//����uniformû�д���b,С��a����������,��chi2��������
					vec_df(countnum) = dCase;
					vec_sig(countnum) = 1-chi2cdf(m_dChi, dCase);
					DrawResult(vec_crit, vec_count, vec_expect, vec_res, pResult);
					vec_crit.create(dCase-1);
					vec_count.create(dCase);
				}
			}
			DrawOutput(vec_Chi2, vec_df, vec_sig, pResult);
			//tTemp.Toc("����");
//�͵�ֹͣ
			//tTemp.Toc("�ܽ��");
		}//INDEX������
		//�ͷ��ڴ�
		vec_y_dis.destroy();//�û��������ɢ�����۲�ֵ
		vec_y_prob.destroy();//�û��������ɢ�������ֵ�Ԥ�ڸ���
	return pResult;
}

void CChiSquare::DrawOutput(CDoubleVector vec_Chi2, CDoubleVector vec_df, CDoubleVector vec_sig, CResult *pResult)
{
	CTLTable *pTable11 = NULL;
	CRsltElementTable *pETable11=NULL;
	
	pTable11 = new CTLTable;
	pTable11->CreateTable(2, 2);
	pTable11->SetTitle(m_str_x + "  ����������");
	pTable11->SetRows(2);
	pTable11->SetCols(2);
	pTable11->InsertColumn(0, "���� ����");
	pTable11->InsertColumn(1, "����ֵ");
	pTable11->InsertColumn(2, "���ɶ�");
	pTable11->InsertColumn(3, "P ֵ");

	int i = 0;
	int length = vec_sig.vlen();
	CTString str;
	CTString strValue;

//����������������	ʼ
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
//����������������	ֹ

	for(i=0; i<length; i++)
	{
		str.Format("%d", i + 1);
		pTable11->InsertItem(i, m_AllFuncInfo[vec_index(i)-1]);
		pTable11->SetItemText(i, 1, vec_Chi2(vec_index(i)-1));
		pTable11->SetItemText(i, 2, vec_df(vec_index(i)-1));
		pTable11->SetItemText(i, 3, vec_sig(vec_index(i)-1));
	}
	pETable11 = new CRsltElementTable(m_str_x + "  ����������", pTable11);//����һ���������Ķ���
	pResult->Add(pETable11);
	return;
}