// TChartBoxplot.cpp: implementation of the CTChartBoxplot class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartBoxplot.h"
#include "TGraphData.h"
#include "float.h"
#include "TChartArea.h"
#include "../cpknl/cpknl.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CTChartBoxplot::CTChartBoxplot()
{
	m_bWeight = FALSE;
	//�ٷ���
	m_fPercentile[0][0] = 0.05;
	m_fPercentile[1][0] = 0.10;
	m_fPercentile[2][0] = 0.25;
	m_fPercentile[3][0] = 0.50;
	m_fPercentile[4][0] = 0.75;
	m_fPercentile[5][0] = 0.90;
	m_fPercentile[6][0] = 0.95;
	SetChartType(CTChart::Boxplot);
}

CTChartBoxplot::~CTChartBoxplot()
{

}

void CTChartBoxplot::SetBoxplot(void *pValue,int nCount,EDataType DataType,CTString strLengend)
{
	SetData(pValue,nCount,EBoxPlot,DataType,strLengend);
}

void CTChartBoxplot::SetParam(BOOL bWeight, BOOL bSorted, BOOL bCalcPercent)
{
	m_bWeight = bWeight;
	m_bSorted = bSorted;          //�����Ƿ��Ѿ�����
	m_bCalcPercent = bCalcPercent;//�Ƿ��Ѽ���
}

void CTChartBoxplot::SetPercent(double fPercent[DISP_PERCENT][2])
{
	memcpy(m_fPercentile,fPercent,sizeof(m_fPercentile));
}

void CTChartBoxplot::CalcPercent(CDoubleVector &v, CDoubleVector &w, CIntVector &vecInd)
{
	int i = 0, j = 0;
	BOOL bRet = FALSE;
	int k1[DISP_PERCENT], k2[DISP_PERCENT];
	double fTc1[DISP_PERCENT], fTc2[DISP_PERCENT];
	double fG1[DISP_PERCENT], fG2[DISP_PERCENT];
	double fG1Star[DISP_PERCENT], fG2Star[DISP_PERCENT];

	int nCol  = m_bWeight ? 1 : 0;
	int nVlen = v.vlen();
	
	if (!m_bSorted)
	{
		vecInd.destroy();
		v.Sort(vecInd);//����
	}
	if (m_bCalcPercent) return;//�Ѿ�����
	//�ٷ���
	for (i=0;i<DISP_PERCENT && nVlen < 2;i++)
	{
		if (nVlen == 0)
		{
			m_fPercentile[i][1] = 0;
			bRet = TRUE;
		}
		else
		if (nVlen == 1)
		{
			m_fPercentile[i][1] = v(0);
			bRet = TRUE;
		}
	}
	//�ж��Ƿ񷵻�
	if (bRet) return;
	//���Ȩ�ص�����
	CDoubleVector wTmp(nVlen,0);
	if (m_bWeight)
	{
		double fSum = v.mean_w(w) * nVlen;//�ܺ�
		wTmp(0) = w(vecInd(0)-1);
		//��ٷ���
		for (i=1;i<nVlen;i++)
		{
			wTmp(i) = wTmp(i-1) + w(vecInd(i)-1);
		}
		for (i=0;i<DISP_PERCENT;i++)
		{
			fTc1[i] = fSum * m_fPercentile[i][0];
			fTc2[i] = (fSum+1) * m_fPercentile[i][0];
			k1[i] = k2[i] = 0;
		}
		//��ֵ
		for (j=0;j<DISP_PERCENT;j++)
		{
			for (i=0;i<nVlen-1;i++)
			{
				if (wTmp(i) <= fTc1[j] && wTmp(i+1) > fTc1[j])
				{
					k1[j] = i;
					break;
				}
			}
		}
		for (j=0;j<DISP_PERCENT;j++)
		{
			for (i=0;i<nVlen-1;i++)
			{
				if (wTmp(i) <= fTc2[j] && wTmp(i+1) > fTc2[j])
				{
					k2[j] = i;
					break;
				}
			}
		}
		for (j=0;j<DISP_PERCENT;j++)
		{
			//fG1Star[j] = fTc1[j] - wTmp(k1[j]);
			fG2Star[j] = fTc2[j] - wTmp(k2[j]);
			if (fG2Star[j] >= 1)
			{
				m_fPercentile[j][1] = v(k1[j]+1);
			}
			else
			{
				if (wTmp(vecInd(k2[j]+1)-1) >= 1)
				{
					m_fPercentile[j][1] = (1-fG2Star[j])*v(k2[j]) + fG2Star[j]*v(k2[j]+1);
				}
				else
				{
					//fG1[j] = fG1Star[j]/dataMatrix(0)((vecInd(k1[j]+1)-1));
					fG2[j] = fG2Star[j]/w((vecInd(k2[j]+1)-1));
					m_fPercentile[j][1] = (1-fG2[j])*v(k2[j]) + fG2[j]*v(k2[j]+1);
				}
			}
			if (m_fPercentile[j][1] < v(0))
				m_fPercentile[j][1] = v(0);
			if (m_fPercentile[j][1] > v(nVlen-1))
				m_fPercentile[j][1] = v(nVlen-1);
		}
	}
	else
	{
		wTmp(0) = 1;
		//�ж��Ƿ񷵻�
		if (bRet) return;
		//�ٷ���int
		for ( i=0;i<DISP_PERCENT;i++)
		{
			double k = m_fPercentile[i][0]*nVlen;
			j = (long)ceil(k);
			if (nVlen%2 == 0)
			{	
				if (j==int(k))
				{
					m_fPercentile[i][1] = (v(j) + v(j-1))/2;
				}
				else
				{
					m_fPercentile[i][1] = v(j-1);
				}
			}
			else
			{
				long nIndex = (long)k;
				double fLeave = k- nIndex;
				if (m_fPercentile[i][0] == 0.25)
				{
					if (fLeave > 0.50)
					{
						m_fPercentile[i][1] = (v(nIndex+1) + v(nIndex))/2;
					}
					else
					{
						m_fPercentile[i][1] = v(nIndex);
					}					
				}
				else
				if (m_fPercentile[i][0] == 0.50)
				{
					m_fPercentile[i][1] = v(nIndex);
				}
				else
				if (m_fPercentile[i][0] == 0.75)
				{
					if (fLeave < 0.50)
					{
						m_fPercentile[i][1] = (v(nIndex) + v(nIndex-1))/2;
					}
					else
					{
						m_fPercentile[i][1] = v(nIndex);
					}
				}
				else
				{
					if (j==int(k))
					{
						m_fPercentile[i][1] = (v(j) + v(j-1))/2;
					}
					else
					{
						m_fPercentile[i][1] = v(j-1);
					}
				}
			}
		}
	}
}

//�ú������ص�x������5��ֵ��
//��x[0]��ʼ�ֱ�Ϊ�� ��Сֵ�ĺ��ߡ�BOX�����ߣ��м�ֵ��BOX�����ߡ�����ֵ�ĺ���
//�緵�ص�intֵ����5����˵����extremֵ����Ҫ������ͼ������ʾ��
void CTChartBoxplot::CalcBoxPlot(CDoubleVector &v, void **pData, int &nCount)
{
	CTString szText("");
	int i = 0;
	int nVari=5,nVark=5,nMax=0,nMin=0;
	double fMin=0,fMax=0,fMd=0,fDn=0,fUp=0;
	
	int nVlen = v.vlen();//�Ѿ�����
	int nCol  = m_bWeight ? 1 : 0;
	
	nMin = nVlen;
	fDn = m_fPercentile[2][1];
	fMd = m_fPercentile[3][1];
	fUp = m_fPercentile[4][1];
	for (i=nVlen-1;i>0;i--) //��ȡ���ֵ���������ֵ
	{
		if (v(i) <= (fUp+1.5*(fUp-fDn))) break;
		nMax = i;
	}
	for (i=0;i<nVlen-1;i++)//��ȡ��Сֵ��������Сֵ
	{
		if (v(i) >= (fDn-1.5*(fUp-fDn))) break;
		nMin = i;
	}
	if (nMax > 0)
	{
		fMax = v(nMax-1);
		nVari += nVlen - nMax;
	}
	else
	{
		fMax = v(nVlen-1);
	}
	if (nMin < nVlen-1)
	{
		fMin = v(nMin+1);
		nVari += nMin + 1;
	}
	else
	{
		fMin = v(0);
	}
    
	*pData = new tagDataPointStrArr;
	tagDataPointStrArr *pDataTemp = (tagDataPointStrArr *)*pData;
	szText.Format("%d",nVlen);
	pDataTemp->strName = szText;
	pDataTemp->arrYVal.Add(fMin);
	pDataTemp->arrYVal.Add(fDn);
	pDataTemp->arrYVal.Add(fMd);
	pDataTemp->arrYVal.Add(fUp);
	pDataTemp->arrYVal.Add(fMax);
	if (nVari > 5 && nMax > 0)    //�������ֵ���쳣��
	{
		for (i=nMax;i<nVlen;i++)
		{
			pDataTemp->arrYVal.Add(v(i));
		}
	}
	if (nVari > 5 && nMin < nVlen)//������Сֵ���쳣��
	{
		for (i=0;i<nMin+1;i++)
		{
			pDataTemp->arrYVal.Add(v(i));
		}
	}
	nCount = nVari;
}