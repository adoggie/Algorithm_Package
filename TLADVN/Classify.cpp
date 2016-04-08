// Classify.cpp: implementation of the CClassify class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <math.h>
#include "Classify.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClassify::CClassify()
{
	m_pSimulate			= NULL;
	m_pN				= NULL;
	m_pSum				= NULL;
	m_pIndex			= NULL;
	m_pDataInterface	= NULL;
	m_aRecNo			= NULL;
	m_bSimulate			= true;
	m_nPower			= 2;
	m_nRoot				= 2;
	m_nPresent			= 1;
	m_nAbsent			= 0;
	m_nCurRow			= 0;
	m_nClusterMethod	= 0;
	m_nDistance			= 1;
	m_bOutputIcicle		= false;
	m_bWeightSet		= false;
	m_bCaseCluster		= true;
	m_bAbort			= false;
	m_nOutChartSel=2;
}

CClassify::~CClassify()
{
	Free();
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//					标准化矩阵，参数可见文档
//					nIndex : 1----7;对应不同的标准化方法
//					????   方法  2和6的结果和spss的不一样
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CClassify::Formalize(const int nIndex)
{
	int i;

//	assert(m_nRow>0&&m_nCol>0&&m_pData);
	assert(m_nRow>0&&m_nCol>0);
//	double * pdev     = new double [m_nCol];
/*	if( !paverage||!pdev||!pmax||!pmin)
	{
		AfxMessageBox("内存不足!");
		return false;
	}

	for(i=0;i<m_nCol;i++)
	{
		sum = 0;
		for(int j=0;j<m_nRow;j++)
		{
			sum += (Data(j,i) - paverage[i] )*(Data(j,i) - paverage[i] );
		}
		pdev[i] = sum / (m_nRow - 1);
	}
*/

	switch(nIndex)
	{
	case 0:	
		//不标准化
		break;
	case 1:
		//按列标准化
		CAlgorithmBase::Formalize(0);
		break;
//	case 2:
		/*
		//标准化－1 到 1 
		for(i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nRow;j++)
			{
				if( pmin[i] >= 0 )
				{
					m_pData[j*m_nCol+i] /= pmax[i];
				}
				else if( pmax[i] > 0 && pmin[i] < 0 )
				{
					m_pData[j*m_nCol+i] /= (pmax[i] - pmin[i]);
				}
				else if( pmax[i] < 0 )
				{
					m_pData[j*m_nCol+i] /= pmin[i];
				}
			}
		}
		*/
//		break;
	case 2:
	case 3:
		{
			double * pmax     = new double [m_nCol];
			double * pmin     = new double [m_nCol];
			//求每列数据的最大和最小值，存入数组
			for(i=0;i<m_nCol;i++)
			{
				double max=Data(0, i);
				double min=Data(0, i);
				int j=1;
				for(j=1;j<m_nRow;j++)
				{
					if( Data(j,i) > max )
					{
						max = Data(j,i);
					}
					if( Data(j,i) < min )
					{
						min = Data(j,i);
					}
				}
				pmax[i] = max;
				pmin[i] = min;
			}

			if( nIndex==2 )
			{
				//标准化到最大值
				for(i=0;i<m_nCol;i++)
				{
					for(int j=0;j<m_nRow;j++)
					{
						if( fabs(pmax[i]) < 1e-10 )
						{
							if( fabs(pmin[i]<1e-10 ))
							{
// 								AfxMessageBox("数据非法!");
								return false;	
							}
							else
								wData(j,i) = wData(j,i)/fabs(pmin[i])+1;
//yzmModify								m_pData[j*m_nCol+i] = m_pData[j*m_nCol+i]/fabs(pmin[i])+1;

						}
						else 
						{
							wData(j,i) /= pmax[i];
//yzmModify							m_pData[j*m_nCol+i] /= pmax[i];
						}
					}
				}		
			}
			else
			{
				//把数值标准化到0到1范围内
				for(i=0;i<m_nCol;i++)
				{
					for(int j=0;j<m_nRow;j++)
					{
						if( fabs(pmax[i]-pmin[i]) < 1e-10 )
						{
							wData(j,i) = 0.5;
//yzmModify							m_pData[j*m_nCol+i] = 0.5;
						}
						else
						{
							wData(j,i) = (Data(j,i)-pmin[i])/(pmax[i]-pmin[i]);
//yzmModify							m_pData[j*m_nCol+i] = (Data(j,i)-pmin[i])/(pmax[i]-pmin[i]);
						}
					}
				}
			}
			delete [] pmax;
			delete [] pmin;
			pmax     = NULL;
			pmin     = NULL;
		}

		break;
	case 4:
		{
			// 求各列均值
			double * paverage = new double [m_nCol];
			double sum(0);
			int i=0;
			for(i=0;i<m_nCol;i++)
			{
				sum = 0;
				for(int j=0;j<m_nRow;j++)
				{
					sum += Data(j,i);			
				}
				paverage[i] = sum / m_nRow;
			}

			//把数值标准化到均值范围内
			for(i=0;i<m_nCol;i++)
			{
				for(int j=0;j<m_nRow;j++)
				{
					if( paverage[i] < 1e-10 )
					{	
						wData(j,i) += 1;	
//yzmModify						m_pData[j*m_nCol+i] += 1;					
					}
					else
					{
						wData(j,i) /= paverage[i];
//yzmModify						m_pData[j*m_nCol+i] /= paverage[i];
					}
				}
			}
			delete [] paverage;
			paverage = NULL;
		}

		break;
	case 6:
		/*
		//把数值标准化到单位标准差范围内
		for(i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nRow;j++)
			{
				if( pdev[i] < 1e-10 )
				{										
				}
				else
				{					
					m_pData[j*m_nCol+i] /= pdev[i];
				}
			}
		}
		*/
		break;
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//						聚类测度方法的选择
//						nMethod == 0: 间隔
//						nMethod == 1: 有序
//						nMethod == 2: 名义
//						nIndex      ：以上方法中的子方法
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CClassify::ClusterMeasures(int nMethod, int nIndex)
{
//	Free();
	
	m_pN		= new double [m_nRow];	
	m_pSum		= new double [m_nRow];	
	m_pIndex	= new int    [m_nRow];
	m_pSimulate = new double [m_nRow * m_nRow];	

	memset(m_pN,0,sizeof(double)*m_nRow);
	memset(m_pSum,0,sizeof(double)*m_nRow);
	memset(m_pSimulate,0,sizeof(double)*m_nRow*m_nRow );

	for(int i=0;i<m_nRow;i++)
	{
		m_pN[i]     = 1;
		m_pSum[i]   = 0;
		m_pIndex[i] = i+1;
	}
	switch(nMethod)
	{
	case 0:
		Interval(nIndex);
		break;
	case 1:
		Count(nIndex);
		break;
	case 2:
		Binary(nIndex);
		break;
	default:
		return false;
	}
	return true;
}

void CClassify::Interval(int nIndex)
{
	int i,j,k;
	double sum(0);

//	pBar->SetRange(1,m_nRow);
//	pBar->SetStep(1);

	switch(nIndex)
	{
	case 0:
		//EUCLID
		for(i=0;i<m_nRow;i++)
		{
//			pBar->StepIt();
			for(j=0;j<m_nRow;j++)
			{
				if( i > j )
				{
					sum = 0;
					for(k=0;k<m_nCol;k++)
					{
						sum += (Data(i,k)-Data(j,k))*(Data(i,k)-Data(j,k));
					}
					m_pSimulate[i*m_nRow+j] = sqrt(sum);
				}
			}
		}
		m_bSimulate = false;
		break;
	case 1:
		//SEUCLID
		for(i=0;i<m_nRow;i++)
		{
//			pBar->StepIt();
			for(j=0;j<m_nRow;j++)
			{
				if( i > j )
				{
					sum = 0;
					for(k=0;k<m_nCol;k++)
					{
						sum += (Data(i,k)-Data(j,k))*(Data(i,k)-Data(j,k));
					}
					m_pSimulate[i*m_nRow+j] = sum;
				}
			}
		}
		m_bSimulate = false;
		break;
		/*
	case 2:
		//相关系数
		{
		double * pAverage = this->Average(0);
		double * pDev     = this->Dev(0);
		WriteData(m_pData,20,4,"输入数据");
		WriteData(pAverage,1,20,"平均值");
		WriteData(pDev,1,20,"标准差");
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nRow;j++)
			{
				if( i>j )
				{
					sum = 0 ;				
					for(k=0;k<m_nCol;k++)
					{
						assert( pDev[i] > 1e-10 && pDev[j] > 1e-10 );
						sum += fabs( (Data(i,k) - pAverage[i])/pDev[i] )*fabs( (Data(j,k) - pAverage[j])/pDev[j] );
					}
					m_pSimulate[i*m_nRow+j] = sum / m_nCol;
				}
			}
		}
		delete [] pAverage;pAverage = NULL;
		delete [] pDev    ;pDev     = NULL;
		}
		m_bSimulate = true;
		break;
		
	case 3:
		//夹角余弦
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nRow;j++)
			{
				if( i>j )
				{
					double sum1=0,sum2=0;
					sum = 0 ;
					for(k=0;k<m_nCol;k++)
					{
						sum  += Data(i,k)*Data(j,k);
						sum1 += Data(i,k)*Data(i,k);
						sum2 += Data(j,k)*Data(j,k);
					}
					assert( sum1*sum2 > 1e-10 );
					m_pSimulate[i*m_nRow+j] = sum / (sqrt(sum1*sum2));
				}
			}
		}
		m_bSimulate = true;
		break;
	case 4:
		//车比雪夫距离
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nRow;j++)
			{
				if( i>j )
				{
					double max = -100;
					sum = 0;
					for(k=0;k<m_nCol;k++)
					{
						sum = fabs(Data(i,k)-Data(j,k));
						if( sum > max ) 
						{
							max = sum;
						}
					}			
					m_pSimulate[i*m_nRow+j] = sum;
				}
			}
		}
		m_bSimulate = false;
		break;
		*/
	case 2:
		//波洛克距离
		for(i=0;i<m_nRow;i++)
		{
//			pBar->StepIt();
			for(j=0;j<m_nRow;j++)
			{
				if( i>j )
				{
					
					sum = 0;
					for(k=0;k<m_nCol;k++)
					{
						sum += fabs(Data(i,k)-Data(j,k));					
					}			
					m_pSimulate[i*m_nRow+j] = sum;
				}
			}
		}
		m_bSimulate = false;
		break;
	case 3:
		//明考斯基距离
		for(i=0;i<m_nRow;i++)
		{
//			pBar->StepIt();
			for(j=0;j<m_nRow;j++)
			{
				if( i>j )
				{
					
					sum = 0;
					for(k=0;k<m_nCol;k++)
					{
						sum += fabs( pow(Data(i,k)-Data(j,k),m_nPower) );					
					}
					assert(m_nPower>0);
					m_pSimulate[i*m_nRow+j] = pow(sum,1.0/m_nPower);
				}
			}
		}
		m_bSimulate = false;
		break;
	case 4:
		//自定距离
		for(i=0;i<m_nRow;i++)
		{
//			pBar->StepIt();
			for(j=0;j<m_nRow;j++)
			{
				if( i>j )
				{
					
					sum = 0;
					for(k=0;k<m_nCol;k++)
					{
						sum += fabs( pow(Data(i,k)-Data(j,k),m_nPower));					
					}		
					assert(m_nRoot>0);
					m_pSimulate[i*m_nRow+j] = pow(sum,1.0/m_nRoot);
				}
			}
		}
		m_bSimulate = false;
		break;
	}
//	WriteData(m_pSimulate,m_nRow,m_nRow,"初始相似矩阵");	
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//							有序测度的计算
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CClassify::Count(int nIndex)
{
	switch(nIndex)
	{
	case 0:
		break;
	case 1:
		break;		
	}		
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//												名义测度的计算
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CClassify::Binary(int nIndex)
{

	int i=0,j=0,k=0;	
	for(i=0;i<m_nRow;i++)
	{
		for(j=0;j<m_nRow;j++)
		{
			if( i<=j )
				continue;
			double t1=0,t2=0;
			double a=0,b=0,c=0,d=0;
			for(k=0;k<m_nCol;k++)
			{
				if( fabs(Data(i,k) - m_nPresent)<1e-10 && fabs(Data(j,k) - m_nPresent)<1e-10 )
				{
					a++;
				}
				else if(fabs(Data(i,k) - m_nPresent)>1e-10 && fabs(Data(j,k) - m_nPresent)>1e-10 )
				{
					d++;
				}
				else if( fabs(Data(i,k) - m_nPresent)<1e-10 && fabs(Data(j,k) - m_nPresent)>1e-10 )
				{
					b++;
				}
				else if( fabs(Data(i,k) - m_nPresent)>1e-10 && fabs(Data(j,k) - m_nPresent)<1e-10 )
				{
					c++;
				}
			}
			t1 = MAX(a,b) + MAX(c,d) + MAX(a,c) + MAX(b,d);
			t2 = MAX(a+c,b+d) + MAX(a+b,c+d);
			switch(nIndex)
			{
			case 0:
				//RR相似性测度
				m_pSimulate[i*m_nRow+j] = a / (a+b+c+d);				
				break;
			case 1:
				//SM相似性测度
				m_pSimulate[i*m_nRow+j] = (a+d)/(a+b+c+d);
				break;
			case 2:
				//Jaccard相似性测度
				m_pSimulate[i*m_nRow+j] = a / (a+b+c);
				break;
			case 3:
				//Dice相似性测度
				m_pSimulate[i*m_nRow+j] = 2*a / (2*a+b+c);
				break;
			case 4:
				// SS1相似性测度
				m_pSimulate[i*m_nRow+j] = 2*(a+d)/( 2*(a+d) + b + c );
				break;
			case 5:
				//RT相似性测度
				m_pSimulate[i*m_nRow+j] = (a+d) / ( a+d+2*(b+c));
				break;
			case 6:
				//SS2相似性测度
				m_pSimulate[i*m_nRow+j] = a / ( a + 2*(b+c));
				break;
			case 7:
				//K1相似性测度
				m_pSimulate[i*m_nRow+j] = a / (b+c);
				break;
			case 8:
				//SS3相似性测度
				m_pSimulate[i*m_nRow+j] = (a+d)/(b+c);
				break;
			case 9:
				//K2相似性测度
				m_pSimulate[i*m_nRow+j] = (a/(a+b) + a/(a+c)) / 2;
				break;
			case 10:
				//SS4相似性测度
				m_pSimulate[i*m_nRow+j] = (a/(a+b) + a/(a+c) + d/(b+d) + d/(c+d)) / 4;
				break;
			case 11:
				// Hamann相似性测度	
				m_pSimulate[i*m_nRow+j] = ((a+d) - (b+c))/(a+b+c+d);
				break;
			case 12:
				//Lambda相似性测度
				m_pSimulate[i*m_nRow+j] = (t1-t2)/(2*(a+b+c+d)-t2);
				break;
			case 13:
				//D相似性测度
				m_pSimulate[i*m_nRow+j] = (t1-t2)/(2*(a+b+c+d));
				break;
			case 14:
				//Y相似性测度
				m_pSimulate[i*m_nRow+j] = ( sqrt(a*d) - sqrt(b*c) )/ (sqrt(a*d) + sqrt(b*c));
				break;
			case 15:
				//Q相似性测度
				m_pSimulate[i*m_nRow+j] = (a*d-b*c)/(a*d+b*c);
				break;
			case 16:
				//Ochiai相似性测度
				m_pSimulate[i*m_nRow+j] = sqrt((a/(a+b))*(a/(a+c)));
				break;
			case 17:
				// SS5相似性测度
				m_pSimulate[i*m_nRow+j] = a*d /( sqrt((a+b)*(a+c)*(b+d)*(c+d)) );
				break;
			case 18:
				//PHI相似性测度
				m_pSimulate[i*m_nRow+j] = (a*d-b*c) / ( sqrt((a+b)*(a+c)*(b+d)*(c+d)));
				break;
			case 19:
				//二值欧氏距离
				m_pSimulate[i*m_nRow+j] = sqrt(b+c);
				break;
			case 20:
				//二值欧氏距离平方
				m_pSimulate[i*m_nRow+j] = b+c;
				break;
			case 21:
				//Size不相似性测度
				m_pSimulate[i*m_nRow+j] = (b-c)*(b-c) / ( (a+b+c+d)*(a+b+c+d));
				break;
			case 22:
				//Patternb不相似性测度
				m_pSimulate[i*m_nRow+j] = b*c / ((a+b+c+d)*(a+b+c+d));
				break;
			case 23:
				//Shape不相似性测度
				m_pSimulate[i*m_nRow+j] = ((a+b+c+d)*(b+c)-(b-c)*(b-c)) / ((a+b+c+d)*(a+b+c+d) );
				break;
			case 24:
				//Dispersion相似性测度
				m_pSimulate[i*m_nRow+j] = (a*d - b*c )/ ((a+b+c+d)*(a+b+c+d) );
				break;
			case 25:
				//方差不相似性测度
				m_pSimulate[i*m_nRow+j] = (b+c)/(4*(a+b+c+d));
				break;
			case 26:
				// Blwmn不相似性测度
				m_pSimulate[i*m_nRow+j] = (b+c)/(2*a+b+c);
				break;				
			}
		}		
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//	    聚类，p,q为选择的最为相似的测度的对应的行列号，nIndex 为聚类用的方法
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CClassify::ClusterMethod(int nIndex,int p,int q)
{
	int i=0;
	int qq = (int)m_pIndex[q]-1;
	int pp = (int)m_pIndex[p]-1;
	if(!m_pN)
	{
		m_pN = new double [m_nRow];
	}
	switch(nIndex)
	{
	case 0:
		//Average Linkage between Group			
		for(i=0;i<m_nCurRow;i++)
		{
			if(i!=q && i!=p )
			{
				*SimulateAdd(q,i) = SimulateData(i,p) + SimulateData(i,q);
			}
		}
		m_pN[qq] += m_pN[pp];
		break;
	case 1:
		//Average Linkage within Group			
		for(i=0;i<m_nCurRow;i++)
		{
			if(i!=q && i!=p )
			{
				*SimulateAdd(q,i) = SimulateData(i,p) + SimulateData(i,q);
			}
		}
		m_pSum[qq] = m_pSum[pp] + m_pSum[qq] + SimulateData(p,q);
		m_pN[qq]   += m_pN[pp];		
		break;
	case 2:
		//Single Linkage
		for(i=0;i<m_nCurRow;i++)
		{
			if(i==q||i==p)
				continue;
			if(m_bSimulate)
			{
				*SimulateAdd(q,i) = MAX( SimulateData(i,p) ,SimulateData(i,q) );
			}
			else
			{
				*SimulateAdd(q,i) = MIN( SimulateData(i,p) ,SimulateData(i,q) );
			}
		}
		break;
	case 3:
		//Complete Linkage
		for(i=0;i<m_nCurRow;i++)
		{
			if(i==q||i==p)
				continue;
			if(m_bSimulate)
			{
				*SimulateAdd(q,i) = MIN( SimulateData(i,p) ,SimulateData(i,q) );
				
			}
			else
			{
				*SimulateAdd(q,i) = MAX( SimulateData(i,p) ,SimulateData(i,q) );
			}
		}
		break;
	case 4:
		//Centroid 
		for(i=0;i<m_nCurRow;i++)
		{
			if(i==q||i==p)
				continue;
			*SimulateAdd(q,i) = SimulateData(i,p)*m_pN[pp]/(m_pN[pp]+m_pN[qq])
				+ SimulateData(i,q)*m_pN[qq]/(m_pN[pp]+m_pN[qq])
				- SimulateData(q,p)*m_pN[qq]*m_pN[pp]/( (m_pN[pp]+m_pN[qq])*(m_pN[pp]+m_pN[qq]) );
		}
		m_pN[qq]   += m_pN[pp];	
		break;
	case 5:
		//Median 
		for(i=0;i<m_nCurRow;i++)
		{
			if(i==q||i==p)
				continue;
			*SimulateAdd(q,i) = ( SimulateData(i,p) + SimulateData(i,q) )/2   -   SimulateData(p,q)/4  ;
		}
	case 6:
		//Ward's 
		for(i=0;i<m_nCurRow;i++)
		{
			if(i==q||i==p)
				continue;
			*SimulateAdd(q,i) = ( (m_pN[i] + m_pN[p])*SimulateData(p,i) + (m_pN[i]+m_pN[q])*SimulateData(q,i)-m_pN[i]*SimulateData(p,q) )/(m_pN[p] + m_pN[i]);
			*SimulateAdd(q,i) = SimulateData(q,p)/2;
		}
		m_pN[qq]   += m_pN[pp];	
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//					Get the Data at position of No. nRow nCol in the Simulate Matrix
//
////////////////////////////////////////////////////////////////////////////////////////////////
double CClassify::SimulateData(int nRow, int nCol)
{
	assert( nRow >= 0 && nCol >= 0 && m_pSimulate && nRow != nCol );
	if( nRow > nCol )
	{
		return m_pSimulate[nRow * m_nRow + nCol];
	}
	else
	{
		return m_pSimulate[nCol * m_nRow + nRow];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//									取回对应位置的地址
//
//////////////////////////////////////////////////////////////////////////////////////////////
double * CClassify::SimulateAdd(int nRow, int nCol)
{
	assert( nRow >= 0 && nCol >= 0 && m_pSimulate && nRow != nCol );
	if( nRow > nCol )
	{
		return &m_pSimulate[nRow * m_nRow + nCol];
	}
	else
	{
		return &m_pSimulate[nCol * m_nRow + nRow];
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//								聚类分析的循环过程
//
////////////////////////////////////////////////////////////////////////////////////////////
void CClassify::GeneralProcedure()
{
	m_nCurRow = m_nRow;
	int q = -1,p = -1;
	int i=0,j=0;
	double temp=0;
//	MSG msg;
//	CProgressCtrl * pBar=(CProgressCtrl *)m_pDlgProgress->GetDlgItem(IDC_PROGRESS1);
//	m_pDlgProgress->m_szIndex = _T("聚类分析过程!");
//	m_pDlgProgress->UpdateData(false);
//	pBar->SetRange(1,m_nCurRow);
//	pBar->SetStep(1);
	while( m_nCurRow != 1 )
	{	
		//Find the most similar pair of cluster p and q ( p>q )
		double max=-1e100,min=1e100; 
//		pBar->StepIt();
		for(j=0;j<m_nCurRow-1;j++)
		{		
			for(i=j+1;i<m_nCurRow;i++)
			{
				//矩阵变换后的行号和原来的行号的转变
				int ii = (int)m_pIndex[i]-1;
				int jj = (int)m_pIndex[j]-1;
				switch(m_nClusterMethod)
				{
				case 0:
					temp = SimulateData(i,j)/(m_pN[ii]*m_pN[jj]);
					break;
				case 1:
					assert(  fabs( (m_pN[ii]+m_pN[jj])*(m_pN[ii]+m_pN[jj]-1) ) > 1e-10  ) ;
					temp = 2*( m_pSum[ii] + m_pSum[jj] + SimulateData(i,j) ) / ((m_pN[ii]+m_pN[jj])*(m_pN[ii]+m_pN[jj]-1));
					break;
				case 2:			
				case 3:
				case 4:
				case 5:
				case 6:
					temp = SimulateData(i,j);					
				};
				if( m_bSimulate && temp>max )
				{
					max = temp;
					p = i;
					q = j;
				}
				else if( !m_bSimulate && temp<min )
				{
					min = temp;
					p = i;
					q = j;
				}
			}
		}
		//Reduce the number of cluster by one through merge of clusters p and q .
		//Label the new cluser t(=q) and update similarity matrix 
		TClassifyGroup * pGroup = new TClassifyGroup;
		m_arClassifyResult.Add(pGroup);
		pGroup->nIndex1 = m_pIndex[q];
		pGroup->nIndex2 = m_pIndex[p];
		pGroup->dbCoeff = m_bSimulate ? max : min;
		UpdateSimulateMatrix(p,q);
		ProcessIndex(p);

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//							更新相似性矩阵，p,q为相似值最大的位置
//
///////////////////////////////////////////////////////////////////////////////////////////////
void CClassify::UpdateSimulateMatrix(const int p,const int q)
{
	assert( p > q );
	ClusterMethod(m_nClusterMethod,p,q);
	//WriteData(m_pN,1,m_nRow,"m_pN = ");
	//Delete the row and column of S corresponding to cluster p
	m_nCurRow--;
	for(int i=p;i<m_nCurRow;i++)
	{
		for(int j=0;j<i;j++)
		{
//			if( j >= i )
//				continue;
			if( j < p )
			{
				*SimulateAdd(i,j) = SimulateData(i+1,j);
			}
			else  
			{
				*SimulateAdd(i,j) = SimulateData(i+1,j+1);
			}
		}
	}	
}

void CClassify::Free()
{
	delete [] m_pSimulate;m_pSimulate = NULL;
	delete [] m_pN       ;m_pN        = NULL;
	delete [] m_pSum     ;m_pSum      = NULL;
	delete [] m_pIndex	 ;m_pIndex    = NULL;
//yzmModify 父类的对象在这里释放不妥当 	delete [] m_pData	 ;m_pData	  = NULL;
	delete [] m_aRecNo	 ;m_aRecNo	  = NULL;
	FreePtrArray(&m_arClassifyResult);
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//							处理矩阵变换后的行对应问题
//
////////////////////////////////////////////////////////////////////////////////////////////
void CClassify::ProcessIndex(const int nIndex)
{
	assert(m_pIndex&&nIndex>=0);
	for(int i=nIndex;i<m_nCurRow;i++)
	{
		//if( i+2 < m_nCurRow )
		{
			m_pIndex[i] = m_pIndex[i+1];		
		}
	}	
}

double * CClassify::GetData( CDataInterface * pDataInterface, TIntStruct& tVarInfo, int& nCaseNum )
{
	int i, j, nRowCount;
	// 从数据库读取数据
	CPtrArray rArray;
	if( tVarInfo.iCount>0 )
	{
		TDataSet * pDataArray = NULL;
		for( i=0; i<tVarInfo.iCount; i++ )
		{			
			nRowCount = pDataInterface->GetColData( tVarInfo.pValue[i], pDataArray, 5 );//取回数据

			if( pDataArray )
			{
				rArray.Add( pDataArray ); // 将各字段数据放入字段数组
				pDataArray = NULL;
			}
		}
	}	

	// 整行删除缺失数据记录，保留记录号
	m_aRecNo = new int[nRowCount];
	double * pTempData = new double[nRowCount*tVarInfo.iCount];
	nCaseNum = 0;
	for( i=0; i<nRowCount; i++ )
	{
		bool bMissing = false;
		
		for( j=0; j<tVarInfo.iCount; j++ )
		{
			TDataSet * pData = (TDataSet*)rArray.GetAt(j);

 			if( pData[i].iIndex<0 )
			{
				bMissing = true;
				break;
			}
		}
		
		if( !bMissing )
		{
			for( j=0; j<tVarInfo.iCount; j++ )
			{
				TDataSet * pData = (TDataSet*)rArray.GetAt(j);

				pTempData[nCaseNum*tVarInfo.iCount+j] = pData[i].dValue;
			}
			m_aRecNo[nCaseNum] = i+1;
			nCaseNum++;
		}
	}
	for( i=0; i<tVarInfo.iCount; i++ )
	{
		TDataSet * pData = (TDataSet*)rArray.GetAt(i);

		delete[] pData;
	}
	rArray.RemoveAll();

	double * pdData = NULL;

	if( nCaseNum>0 )
	{
		pdData = new double[tVarInfo.iCount*nCaseNum];
		memcpy( pdData, pTempData, sizeof(double)*nCaseNum*tVarInfo.iCount );
	}
	delete[] pTempData;

	return pdData;
}

bool CClassify::GetValueFromVO(CDataInterface *pDataInterface, std::string VO)
{
//	CLUSTER
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/MEASURE=[{EUCLID},{SEUCLID**},{PATTERN},{MINKOWSKI({P})},{POWER(P R)}]
//		/METHOD=[{BAVERAGE**},{WAVERAGE},{SINGLE},{COMPLETE},{CENTROID}]
//		/TYPE [CASE**] [VARIABLE]
//		/MISSING=[{EXCLUDE**}]
//		/STANDARDIZE=[{NONE**},{Z},{MAX},{RANGE},{MEAN}]
//		/PLOT [PEDIGREE**] [ICICLE]
/*
	//	0－组间连接法(Average Linkage between Group)
	//	1－组内连接法(Average Linkage within Group)
	//	2－单个连接(Single Linkage)
	//	3－完全连接(Complete Linkage)
	//	4－重心聚类法(Centroid Method)
	int m_nClusterMethod;//								//聚类的方法序号	
	//	0－不进行标准化处理
	//	1－Z分数方法
	//	2－把数值标准化到最大值为1
	//	3－把数值标准化到0到1范围内
	//	4－把数值标准化到均值范围内
	int m_nFomlizeMethod;
	//	0－欧氏距离
	//	1－	欧氏距离平方
	//	2－	波洛克距离
	//	3－	明考斯基距离
	//	4－	自定距离
	int	m_nDistance;         							
	
*/
	AnaWord VOstr;
	VOstr.Import(VO);
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return false;

	if (AnaSyntax::findstr("/MEASURE","EUCLID",VOstr)) 
		m_nDistance = 0;
		else if (AnaSyntax::findstr("/MEASURE","PATTERN",VOstr)) 
			m_nDistance = 2;
			else if (AnaSyntax::findstr("/MEASURE","MINKOWSKI",VOstr)) 
			{
				m_nDistance = 3;
				m_nPower = int(AnaSyntax::getvalue("/MEASURE","MINKOWSKI",VOstr));
			}
				else if (AnaSyntax::findstr("/MEASURE","POWER",VOstr)) 
				{
					m_nDistance = 4;
					m_nPower = int(AnaSyntax::getvalue("/MEASURE","POWER",VOstr));
					m_nRoot = int(AnaSyntax::getvalue("/MEASURE","POWER",VOstr,2));
				}
				else
						m_nDistance = 1;
				
	if (AnaSyntax::findstr("/METHOD","WAVERAGE",VOstr)) 
		m_nClusterMethod = 1;
		else if (AnaSyntax::findstr("/METHOD","SINGLE",VOstr)) 
			m_nClusterMethod = 2;
			else if (AnaSyntax::findstr("/METHOD","COMPLETE",VOstr))
				m_nClusterMethod = 3;
				else if (AnaSyntax::findstr("/METHOD","CENTROID",VOstr))
					m_nClusterMethod = 4;
					else
						m_nClusterMethod = 0;

	m_bCaseCluster = (!AnaSyntax::findstr("/TYPE","VARIABLE",VOstr));
	m_bOutputIcicle = AnaSyntax::findstr("/PLOT","ICICLE",VOstr);
	m_bOutputPedigree = AnaSyntax::findstr("/PLOT","PEDIGREE",VOstr);
	
	if (AnaSyntax::findstr("/STANDARDIZE","Z",VOstr)) 
		m_nFomlizeMethod = 1;
	else if (AnaSyntax::findstr("/STANDARDIZE","MAX",VOstr)) 
		m_nFomlizeMethod = 2;
	else if (AnaSyntax::findstr("/STANDARDIZE","RANGE",VOstr)) 
		m_nFomlizeMethod = 3;
				else if (AnaSyntax::findstr("/STANDARDIZE","MEAN",VOstr)) 
					m_nFomlizeMethod = 4;
				else
					m_nFomlizeMethod = 0;

	return true;
}


CResult * CClassify::OnClassify(CDataInterface *pDataInterface, std::string VO)
{
	m_pDataInterface  = pDataInterface;
	//取参数
	m_bAbort	    = false;
	m_bOutputIcicle = false;
	m_pResult = NULL ;
	m_pResult		  = new CResult("分层聚类");;
	if (!(GetValueFromVO(pDataInterface, VO)))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	//取数据
	m_nOutChartSel=0;
	if(m_bOutputIcicle)
		m_nOutChartSel=1;
	if(m_bOutputPedigree)
	{
		if(m_nOutChartSel==1)
			m_nOutChartSel=3;
		else
			m_nOutChartSel=2;
	}

//		double * pData = GetData(pDataInterface,m_tVarInfo);
	if( pDataInterface->GetWeightNumb()>-1 )
		m_bWeightSet = true;
	else
		m_bWeightSet = false;

	int nCaseNum;
	int m          = m_tVarInfo.iCount;
	double * pData = GetData( pDataInterface, m_tVarInfo, nCaseNum );

	CTString strTitle, strTreated;
	strTitle.Format(   " 数据预处理:        总记录数   有效记录数   丢弃记录数\n\n" );
	strTreated.Format( "                           %8d         %8d          %8d", 
				   pDataInterface->GetRowCount(), nCaseNum, pDataInterface->GetRowCount()-nCaseNum );
	CRsltElementText * pTreatedTextRslt = new CRsltElementText("数据预处理");
	pTreatedTextRslt->AddString( strTitle + strTreated.GetData() );
	m_pResult->Add( pTreatedTextRslt );

	if( nCaseNum>5000 )
	{
		CTString	strWarning;
		strWarning.Format( "注： 记录数太多，不适合进行分层聚类分析，运算终止。" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );

		if( pData )
			delete[] pData;
		return m_pResult;
	}

	if( pData ) 
	{
		if(m_bCaseCluster)
		{
//				SetRowsCols(nCaseNum,m);
			//yzmModify 原来使用直接的内存拷贝不合适
			SetVectData(pData,nCaseNum,m);
		}
		else
		{
			m_bCaseCluster = false;
			double * pTran = MatrixTran(pData,nCaseNum,m);
			delete  [] pData;
			pData = pTran;		
//				SetRowsCols(m,nCaseNum);
			//yzmModify 原来使用直接的内存拷贝不合适
			SetVectData(pData,m,nCaseNum);
		} 
//yzmModify  2005-6-1  没有为m_pData申请空间
//			if (m_pData==NULL)
//                m_pData = new double[m*nCaseNum];
//			memcpy(m_pData,pData,sizeof(double)*m*nCaseNum);

		delete [] pData;pData = NULL;
		
//			m_pDlgProgress = new CDlgProgress;
//			m_pDlgProgress->Create(IDD_DIALOG_PROGRESS);
//			m_pDlgProgress->ShowWindow(true);

		//调用相应算法
		Formalize( m_nFomlizeMethod );	//数据得标准化
		ClusterMeasures(0,m_nDistance);	//测度矩阵的计算
		GeneralProcedure();	//聚类
	}
	//排序,add by xds 20060703.
	QSortArray(&m_arClassifyResult,0,m_arClassifyResult.GetSize()-1);
	//
	if( m_bAbort )
	{
		delete m_pResult;
		m_pResult = NULL;
	}
	else
		CreateResult();		

	Free();
	return m_pResult;
}


void CClassify::CreateResult()
{
	if( m_bWeightSet )
	{
		CTString	strWarning;
		strWarning.Format( " 权重处理：分层聚类忽略权重设置。" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "权重处理" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}

	if( m_arClassifyResult.GetSize()>0 )
	{
		CTLTable * pTable = new CTLTable;
		pTable->SetTitle("聚类步骤表");
		pTable->InsertColumn(0,"步骤");
		pTable->InsertColumn(1,"类1");
		pTable->InsertColumn(1,"类2");
		pTable->InsertColumn(1,"系数");
		int i=0;
		for(i=0;i<m_arClassifyResult.GetSize();i++)
		{
			TClassifyGroup * pt = (TClassifyGroup *)m_arClassifyResult.GetAt(i);
			CTString str;
			str.Format("%d",i+1);
			pTable->InsertItem(i,str);
			if( m_bCaseCluster )
			{
				pTable->SetItemText(i,1,m_aRecNo[pt->nIndex1-1]);
				pTable->SetItemText(i,2,m_aRecNo[pt->nIndex2-1]);				
			}
			else
			{
				pTable->SetItemText(i,1,pt->nIndex1);
				pTable->SetItemText(i,2,pt->nIndex2);				
			}
			pTable->SetItemText(i,3,pt->dbCoeff);
		}
		CRsltElementTable * pETable=new CRsltElementTable("聚类步骤表",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);
		if( m_nOutChartSel>0) //&& m_bOutputIcicle)
		{
			if( m_nRow>=300 && m_nOutChartSel!=2 )
			{
				if(m_nOutChartSel==3) m_nOutChartSel=2;
				if(m_nOutChartSel==1) m_nOutChartSel=0;
				// 数据量太大，不能输出冰柱图
				CTString	strWarning;
				strWarning.Format( " 注：聚类个数超过300，不能制作冰柱图。" );
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
			}
			// 输出冰柱图
			OutputIcicle();
		}
	}
	else // 数据量太小，不能进行聚类分析
	{	
		CTString	strWarning;
		strWarning.Format( " 注：有效数据太少，不能进行聚类分析。" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}
}

void CClassify::OutputIcicle()
{
	if(m_nOutChartSel<=0) return ;
	CTString strTmp;
	CPtrArray aGroupArray;
	CPtrArray pedigreeNodeAry;
	double minX,maxX;
	int i, j, k;
	for( i=0; i<m_arClassifyResult.GetSize(); i++ )
	{
		TClassifyGroup * pt = (TClassifyGroup *)m_arClassifyResult.GetAt(i);
		if(i==0)
		{
			minX=pt->dbCoeff;
			TClassifyGroup * ptm = (TClassifyGroup *)m_arClassifyResult.GetAt(m_arClassifyResult.GetSize()-1);
			maxX=ptm->dbCoeff;
			minX=minX-(maxX-minX)/20;
		}
			
		int iLeftPos = -1;
		CTArray<int, int> * pLeftArray = NULL, * pRightArray = NULL;
		for( j=0; j<aGroupArray.GetSize(); j++ )
		{
			CTArray<int, int> * pTempArray = (CTArray<int, int> *)aGroupArray.GetAt(j);
			for( k=0; k<pTempArray->GetSize(); k++ )
			{
				if( pt->nIndex1 == (*pTempArray)[k] )
				{
					iLeftPos=j;
					pLeftArray = pTempArray;
					break;
				}
			}
			if( pLeftArray )
				break;
		}
		int iRightPos = -1;
		for( j=0; j<aGroupArray.GetSize(); j++ )
		{
			CTArray<int, int> * pTempArray = (CTArray<int, int> *)aGroupArray.GetAt(j);
			for( k=0; k<pTempArray->GetSize(); k++ )
			{
				if( pt->nIndex2 == (*pTempArray)[k] )
				{
					iRightPos = j;
					pRightArray = pTempArray;
					break;
				}
			}
			if( pRightArray )
				break;
		}

		if( pLeftArray && pRightArray )//有左有右
		{
			for( k=0; k<pRightArray->GetSize(); k++ )
				pLeftArray->Add( pRightArray->GetAt(k) );
			delete pRightArray;
			pRightArray = NULL;
			aGroupArray.RemoveAt( iRightPos );

			///{{mxm add following for 生成树结点
			if(m_nOutChartSel>=2){
				PedigreeTreeNode *NewNode =new PedigreeTreeNode();
				NewNode->bLeaf=false;
				NewNode->dCredit=pt->dbCoeff; 

					PedigreeTreeNode *pNode =(PedigreeTreeNode*)pedigreeNodeAry.GetAt(iLeftPos);
					NewNode->NodePtrArray.Add(pNode);
								pNode =(PedigreeTreeNode*)pedigreeNodeAry.GetAt(iRightPos);
					NewNode->NodePtrArray.Add(pNode);			

				pedigreeNodeAry.SetAt(iLeftPos,NewNode);
				pedigreeNodeAry.RemoveAt(iRightPos);
			}
			///}}
		}
		else if( pLeftArray )//左结点
		{
			pLeftArray->Add( pt->nIndex2 );

			///{{mxm add following for 生成树结点
			if(m_nOutChartSel>=2){
				PedigreeTreeNode *NewNode =new PedigreeTreeNode();
				NewNode->bLeaf=false;
				NewNode->dCredit=pt->dbCoeff;  
									
					PedigreeTreeNode *pNode =(PedigreeTreeNode*)pedigreeNodeAry.GetAt(iLeftPos);
					NewNode->NodePtrArray.Add(pNode);
								pNode =new PedigreeTreeNode();
					pNode->bLeaf=true;
					pNode->elemsCode=pt->nIndex2;	
					pNode->dCredit=minX;
					NewNode->NodePtrArray.Add(pNode);			

				pedigreeNodeAry.SetAt(iLeftPos,NewNode);
			}
			///}}
		}
		else if( pRightArray )//右结点
		{
			pRightArray->InsertAt( 0, pt->nIndex1 );

			///{{mxm add following for 生成树结点
			if(m_nOutChartSel>=2){
				PedigreeTreeNode *NewNode =new PedigreeTreeNode();
				NewNode->bLeaf=false;
				NewNode->dCredit=pt->dbCoeff;
					PedigreeTreeNode *pNode =new PedigreeTreeNode();
					pNode->bLeaf=true;
					pNode->elemsCode=pt->nIndex1;			
					pNode->dCredit=minX;
					NewNode->NodePtrArray.Add(pNode);			

					pNode =(PedigreeTreeNode*)pedigreeNodeAry.GetAt(iRightPos);
					NewNode->NodePtrArray.Add(pNode);

				pedigreeNodeAry.SetAt(iRightPos,NewNode);
			}
			///}}
		}
		else //新叶
		{
			CTArray<int, int> * pTempArray = new CTArray<int, int>;
			pTempArray->Add( pt->nIndex1 );
			pTempArray->Add( pt->nIndex2 );
			aGroupArray.Add( pTempArray );			

			///{{mxm add following for 生成树结点
			if(m_nOutChartSel>=2){
				PedigreeTreeNode *NewNode =new PedigreeTreeNode();
				NewNode->bLeaf=false;
				NewNode->dCredit=pt->dbCoeff;
				
					PedigreeTreeNode *pNode =new PedigreeTreeNode();
					pNode->bLeaf=true;
					pNode->elemsCode=pt->nIndex1;
					pNode->dCredit=minX;
					NewNode->NodePtrArray.Add(pNode);
								pNode =new PedigreeTreeNode();
					pNode->bLeaf=true;
					pNode->elemsCode=pt->nIndex2;			
					pNode->dCredit=minX;
					NewNode->NodePtrArray.Add(pNode);			

				pedigreeNodeAry.Add(NewNode);
			}
			///}}
		}
	}
	assert( aGroupArray.GetSize()==1 );

	if(m_nOutChartSel>=2)
		CalNodeYPos(*((PedigreeTreeNode*)pedigreeNodeAry.GetAt(0)),aGroupArray);
	
	CTArray<int, int>* pLastArray = (CTArray<int, int>*)aGroupArray.GetAt(0);

	if(m_nOutChartSel==1 || m_nOutChartSel==3)
	{
		CTLTable * pTable = new CTLTable;
		if( m_bCaseCluster )
			pTable->SetTitle("冰柱图 (样本)");
		else
			pTable->SetTitle("冰柱图 (变量)");


//		pTable->SetRows( m_nRow+1 );
//		pTable->SetCols( m_nRow*2);
	//	pTable->SetRowHeight(20);
		pTable->InsertColumn(0,"聚类数");
		for( i=0; i<m_arClassifyResult.GetSize(); i++ )
		{
			CTString str;
			str.Format( "%d", i+1 );
			if( i<m_arClassifyResult.GetSize()-1 )
				pTable->InsertItem(i,str, false);
			else
				pTable->InsertItem(i,str);
		}



		for( i=0; i<pLastArray->GetSize(); i++ )
		{
			CTString str;
			if( m_bCaseCluster )
			{
				str.Format( "%d", m_aRecNo[pLastArray->GetAt(i)-1] );
			}
			else
				str.Format( "%d", pLastArray->GetAt(i) );
				
			pTable->InsertColumn( i*2, str);
			for( j=0; j<m_arClassifyResult.GetSize(); j++ )
			{
				pTable->SetItemText( j, i*2+1, "X" );
			}
			if( i<pLastArray->GetSize()-1 )
			{
				pTable->InsertColumn( i*2+1, "");
			}
		}

			
		for( i=0; i<m_arClassifyResult.GetSize(); i++ )
		{
			TClassifyGroup * pt = (TClassifyGroup *)m_arClassifyResult.GetAt(i);
			int iLeft, iRight;
			for( j=0; j<pLastArray->GetSize(); j++ )
			{
				if( pLastArray->GetAt(j)==pt->nIndex1 )
					iLeft = j;
				if( pLastArray->GetAt(j)==pt->nIndex2 )
					iRight = j;
			}
		
			for( j=0; j<m_arClassifyResult.GetSize()-i; j++ )
			{
			
				for( k=iLeft; k<iRight; k++ )
				{
					pTable->SetItemText( j, k*2+2, "X" );
				}
			}
			
		}

		CRsltElementTable * pETable=new CRsltElementTable("冰柱图",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);
	}

	///{{谱系图生成 //[mxm add following]
	if(m_nOutChartSel>=2)
	{
		CPtrArray arrnul;
		int nCount = pLastArray->GetSize();
		CTChartPedigree *pTChart = new CTChartPedigree;
		PedigreeTreeNode* pNode = (PedigreeTreeNode*)pedigreeNodeAry.GetAt(0);
		pTChart->SetTitle("谱系图");
		pTChart->SetXAxilLabel("分类系数");
		pTChart->SetYAxilLabel("分类单元");
		pTChart->SetValue(minX,maxX,0,nCount,pNode,arrnul);
		delete pNode;
		CRsltElementChart * pEChart = new CRsltElementChart("谱系图",pTChart);
		m_pResult->Add(pEChart);
		pedigreeNodeAry.RemoveAll();
	}
}


void CClassify::CalNodeYPos(PedigreeTreeNode &node, CPtrArray &leafAry)
{
	int size;
  //叶子
	if(node.bLeaf)
	{
		CTArray<int,int>* pLeafAry=(CTArray<int,int>*)leafAry.GetAt(0);
		size=pLeafAry->GetSize();
		for(int i=0;i<size;i++)
		{
			if(node.elemsCode==pLeafAry->GetAt(i))
			{
				node.yPosition=i+1;//只是根据序号来计算
				break;
			}
		}
	}
	else
	{
		size=node.NodePtrArray.GetSize();
     //儿子
		int i=0;
		for(i=0;i<size;i++)
			CalNodeYPos(*((PedigreeTreeNode *)node.NodePtrArray.GetAt(i)),leafAry);
	 //父亲
		double ymax,ymin;
		for( i=0;i<size; i++)
		{   
			PedigreeTreeNode *pNode=(PedigreeTreeNode *)node.NodePtrArray.GetAt(i);
			if(i==0)
				ymax=ymin=pNode->yPosition;
			else
			{
				if(ymax<pNode->yPosition) ymax=pNode->yPosition;
				if(ymin>pNode->yPosition) ymin=pNode->yPosition;
			}
		}
		node.yPosition=(ymax+ymin)/2;//取中间
	}
}

inline void CClassify::Swap(TClassifyGroup *p1, TClassifyGroup *p2)
{
	TClassifyGroup t;
	t = *p1;
	*p1 = *p2;
	*p2 = t;
}
//对类型排序by xds 20060703-升序
////////////////////////////////////////////////////////////////////////
void CClassify::QSortArray(CPtrArray *pArray, int nLow, int nHigh)
{
	int i,j;
    TClassifyGroup t, *p1 = NULL, *p2 = NULL;

	i = nHigh;
	j = nLow;
	t = *(TClassifyGroup *)pArray->GetAt((int)((i+j)/2));
	do{
		while((*(TClassifyGroup *)pArray->GetAt(j)).dbCoeff < t.dbCoeff)  j++;
		while((*(TClassifyGroup *)pArray->GetAt(i)).dbCoeff > t.dbCoeff)  i--;
		if(i>=j)
		{
			if(i != j)
			{
				Swap((TClassifyGroup *)pArray->GetAt(i),(TClassifyGroup *)pArray->GetAt(j));
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(nLow < i)  QSortArray(pArray,nLow,i);
	if(j < nHigh) QSortArray(pArray,j,nHigh);
}























