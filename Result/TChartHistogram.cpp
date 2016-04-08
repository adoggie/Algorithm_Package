// TChartHistogram.cpp: implementation of the CTChartHistogram class.
//
//////////////////////////////////////////////////////////////////////
#include "TChartHistogram.h"
#include "TChartArea.h"
#include "TGraphData.h"
#include "../cpknl/cpknl.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
#define DBL_DIG         15                      /* # of decimal digits of precision */
#define DBL_EPSILON     2.2204460492503131e-016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define DBL_MANT_DIG    53                      /* # of bits in mantissa */
#define DBL_MAX         1.7976931348623158e+308 /* max value */
#define DBL_MAX_10_EXP  308                     /* max decimal exponent */
#define DBL_MAX_EXP     1024                    /* max binary exponent */
#define DBL_MIN         2.2250738585072014e-308 /* min positive value */
#define DBL_MIN_10_EXP  (-307)                  /* min decimal exponent */
#define DBL_MIN_EXP     (-1021)                 /* min binary exponent */
#define _DBL_RADIX      2                       /* exponent radix */
#define _DBL_ROUNDS     1                       /* addition rounding: near */

const int MAX_DIV = 100;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartHistogram::CTChartHistogram()
{
	m_nDV = 6;
	m_bNormal = false;
	m_ChartKind = Histogram;	
}

CTChartHistogram::~CTChartHistogram()
{
}

void CTChartHistogram::SetHistogram(void* pValue,int nCount,EDataType DataType,CTString strLengend)
{
	SetData(pValue,nCount,EHistogram,DataType,strLengend);
}

bool CTChartHistogram::CalcNormalCurve(CDoubleVector &v,void **pData,int &nCount,bool bSorted)
{
	int i=0;
	double fMin=0, fMax=0;
	double fStep =0, fInterval=0;
	double fStart=0;    
	double x,y,x1,y1;
	
	int nVlen = v.vlen();
	if (!m_bNormal || nVlen < 2) return false;
	const int POINT_COUNT = 100;

	//均值方差
	v.sumvar(m_fMean,m_fVar);
	m_fMean = m_fMean/nVlen;
	m_fStdv = sqrt(m_fVar);
	m_fVlen = nVlen;
	
	if (bSorted)
	{
		fMin = v(0);
		fMax = v(nVlen-1);
	}
	else
	{
		v.MaxMin(fMax, fMin);
	}
	if (m_nDV > 1)
		fStep = (fMax - fMin) / (m_nDV-1);
	else 
		fStep = 0;
	fInterval = fStep/2;
	fStep = (fMax - fMin)/POINT_COUNT;
	if (fabs(fStep) < DOUBLE_ZERO) return false;
	
	nCount = ceil((fMax - fMin + 3*fInterval)/fStep);//总数
	*pData = new TDataPointValVal[nCount*2];
	TDataPointValVal *pDataTemp = (TDataPointValVal *)*pData;
	//处理数据
	fStart = fMin - 2*fInterval;
	x = fStart;
	y = (normcdf(fStart+fInterval,m_fMean,m_fStdv) - normcdf(fStart-fInterval,m_fMean,m_fStdv))*nVlen;
	i=0;
	for(; fStart<(fMax+fInterval); fStart+=fStep)
	{	
		x1 = fStart;
		y1 = (normcdf(fStart+fInterval,m_fMean,m_fStdv) - normcdf(fStart-fInterval,m_fMean,m_fStdv))*nVlen;	  
		if (x1 >= fMin && x1 <= fMax + fInterval)//2-1
		{
			pDataTemp[i].fXVal = x;
			pDataTemp[i].fYVal = y;
			i++;
			pDataTemp[i].fXVal = x1;
			pDataTemp[i].fYVal = y1;
			i++;
			x = x1;
			y = y1;
		}
	}
	//实际个数
	nCount = i;
	return true;
}

bool CTChartHistogram::CalcXYAxis(CDoubleVector &v,void **pData, int &nDV, bool bSorted)
{
	int i = 0, j = 0;
	double fMin = 0.0, fMax = 0.0;
	double fVData = 0.0, fStep = 0.0;

	int nVlen = v.vlen();
	if (nVlen < 1) return false;
	//进行分块处理
	nDV = (int)(1 + ceil(log(nVlen)/log(2)));//分块数
	if( nDV > MAX_DIV) 
		nDV = MAX_DIV; 
	if (m_nDV > 1 && m_nDV < nVlen)
		nDV = m_nDV;
	if (nDV < 1) return false;
	*pData = new TDataPointValVal[nDV];
	TDataPointValVal *pDataTemp = (TDataPointValVal *)*pData;
	//数据有序
	if (bSorted)
	{
		int nStep_g = max(nVlen/nDV/5, 3); //前进的步伐
		int nStep_b = max(nVlen/nDV/30, 2);//后退的步伐
		
		fMin = v(0);
		fMax = v(nVlen-1);
		if (nDV > 1)
			fStep = (fMax - fMin) / (nDV-1);
		else 
			fStep = 0;
		pDataTemp[0].fXVal = fMin;
		pDataTemp[0].fYVal = 0;
		for (i=1;i<nDV-1;i++)
		{
			pDataTemp[i].fXVal = pDataTemp[i-1].fXVal + fStep;
			pDataTemp[i].fYVal = 0;
		}
		pDataTemp[nDV-1].fXVal = fMax;
		pDataTemp[nDV-1].fYVal = 0;
		//处理数据
		int nStart = 0, nEnd = 0;
		for (i=0; i<nDV; i++) 
		{
			if (nStart >= nVlen)//分组结束
			{
				pDataTemp[i].fYVal = 0;
				continue;
			}
			fVData = pDataTemp[i].fXVal - fStep/2 ;
			nStart = FindStartIndex(v,nStart,fVData,nStep_g,nStep_b);
			fVData = pDataTemp[i].fXVal + fStep/2 ;
			nEnd = FindEndIndex(v,nStart,fVData,nStep_g,nStep_b);
			////////////////////////////////////////////////////
			pDataTemp[i].fYVal += nEnd-nStart;
			/////////////////////////////////////////////////////
			nStart = nEnd;
		}
	}
	else
	{
		v.MaxMin(fMax, fMin);
		if (nDV > 1)
			fStep = (fMax - fMin) / (nDV-1);
		else 
			fStep = 0;
		pDataTemp[0].fXVal = fMin;
		pDataTemp[0].fYVal = 0;
		for (i=1;i<nDV-1;i++)
		{
			pDataTemp[i].fXVal = pDataTemp[i-1].fXVal + fStep;
			pDataTemp[i].fYVal = 0;
		}
		pDataTemp[nDV-1].fXVal = fMax;
		pDataTemp[nDV-1].fYVal = 0;
		for (i=0;i<nVlen;i++)
		{
			for (j=0;j<nDV;j++)
			{
				fVData = v(i) - pDataTemp[j].fXVal;
				/*
				if ((fVData < -fStep/2 + EPS_ACCUR) || (fVData >= fStep/2 - EPS_ACCUR)) 
					continue;
				pDataTemp[j].fYVal += 1;
				break;
				*/
				if (v(i) >= pDataTemp[j].fXVal - fStep/2 - EPS_ACCUR &&
					v(i) <  pDataTemp[j].fXVal + fStep/2 - EPS_ACCUR)
				{
					pDataTemp[j].fYVal += 1;
					break;
				}
			}
		}
	}
	return true;
}

int  CTChartHistogram::FindStartIndex(CDoubleVector &v,int nVxInd,double fVtData,int nStep_g,int nStep_b)
{
	int nVxLen = v.vlen();
	
	if (nVxInd >= nVxLen ) return nVxLen;
	
	if (v(nVxInd) >= fVtData-EPS_ACCUR)//找到索引
		return nVxInd;
	
	while (nVxInd < nVxLen && v(nVxInd) < fVtData-EPS_ACCUR)
	{
		nVxInd += nStep_g;
	}
	if (nVxInd >= nVxLen) nVxInd -= nStep_g;
	while (nVxInd > 0 && nVxInd < nVxLen && v(nVxInd) > fVtData+EPS_ACCUR)
	{
		nVxInd -= nStep_b;
	}
	if (nVxInd < 0) nVxInd = 0;
	if (v(nVxInd) < fVtData-EPS_ACCUR) //后退小于当前值
	{
		while (nVxInd < nVxLen && v(nVxInd) < fVtData-EPS_ACCUR)
		{
			nVxInd++;
		}
	}
	else                         //后退等于当前值      
	{
		while (nVxInd > 0 && v(nVxInd) > fVtData-EPS_ACCUR)
		{
			nVxInd --;
		}
	}
	return nVxInd >= nVxLen ? nVxInd-1 : nVxInd;
}

//不包含当前数据的索引
int  CTChartHistogram::FindEndIndex(CDoubleVector &v,int nVxInd,double fVtData,int nStep_g,int nStep_b)
{
	int nVxLen = v.vlen();
	
	if (nVxInd >= nVxLen ) return nVxLen;
	
	if (v(nVxInd) > fVtData+EPS_ACCUR)
		return nVxInd;
	
	while (nVxInd < nVxLen && v(nVxInd) <= fVtData-EPS_ACCUR)
	{
		nVxInd += nStep_g;
	}
	if (nVxInd >= nVxLen) nVxInd -= nStep_g;
	while (nVxInd >= 0 && nVxInd < nVxLen && v(nVxInd) > fVtData-EPS_ACCUR)
	{
		nVxInd -= nStep_b;
	}
	if (nVxInd < 0) nVxInd = 0;
	while (nVxInd < nVxLen && v(nVxInd) <= fVtData-EPS_ACCUR)
	{
		nVxInd ++;
	}
	return nVxInd > nVxLen ? nVxInd-1 : nVxInd;
}
