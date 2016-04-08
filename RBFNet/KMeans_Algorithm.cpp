// KMeans_Algorithm.cpp: implementation of the CKMeans_Algorithm class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "KMeans_Algorithm.h"
#include <assert.h>

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

#define VeryBigValue 1e+30

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKMeans_Algorithm::CKMeans_Algorithm()
{
	m_bAbort=false;
	m_bError=false;
	m_strAbortInfo="";
	m_strErrorInfo="";
	m_nEndCount=0;
}

CKMeans_Algorithm::~CKMeans_Algorithm()
{

}

//功能：快速聚类获得聚类中心
//输入：mx--输入样本矩阵；nCenterNum--聚类中心数；bIsStd--是否对输入数据标准化（暂无用）
//		dPrecision--迭代中止精度；nMaxRepeatTimes--迭代最大次数
//输出：mxCenter--计算得到的聚类中心
bool CKMeans_Algorithm::KMeans(CDoubleMatrix& mx,int nCenterNum,bool bIsStd,double dPrecision,
					 int nMaxRepeatTimes,CDoubleMatrix& mxCenter)
{
	//开始计时
	struct _timeb timeBegin,timeEnd;
	_ftime(&timeBegin);	
	
	
	//根据聚类中心个数,自动调整页面数
	//相比较无此段代码，运算速度改善不明显。且比高级统计中快速聚类计算速度要慢很多，原因待查
	//测试实例(101200记录,19输入字段)：本代码60中心：18分钟；原快速聚类程序60中心：3分40秒
	//by cx  2005-11-28
	if (nCenterNum >5)//大于5个聚类中心时
	{
//		mx.SetMaxPages(nCenterNum+1);  //网络版下此句不能编译
	}
	//////////////////////////////////////////////////////////////////////////


	m_bAbort=false;
	m_bError=false;
	m_strAbortInfo="";
	m_strErrorInfo="";		
	m_nEndCount=0;

	//数据检查
	if(mx.mrow()<nCenterNum)
	{
		m_strErrorInfo="快速聚类错误信息：样本数不应小于聚类中心数！\n";
		m_bError=true;
		return false;
	}
	
	/////////////////////////////初始化聚类中心开始/////////////////////////////////////////////
	int i, j, k, s;
	int nRow=mx.mrow();
	int nCol=mx.mcol();
	CDoubleMatrix mxInitialCenter(nCenterNum,nCol);
	
	// 初始化前nCenterNum条记录为聚类中心
	for(i=0;i<nCenterNum;i++)
	{
		for(j=0;j<nCol;j++)
		{
			mxInitialCenter(i,j)=mx(i,j);
		}
	}
	
	// 计算聚类中心间的相互距离，并记录距离最小值
	double dMaxDistance = 0.0, dDistanceSquare, dMinBetweenCenter;
	int iClosestCenterIndex_m, iClosestCenterIndex_n;
	CDoubleMatrix mxCenterDistance(nCenterNum,nCenterNum,0.0);
	dMinBetweenCenter=VeryBigValue;
	iClosestCenterIndex_m=-1; 
	iClosestCenterIndex_n=-1;
	for(i=0;i<nCenterNum;i++)
	{
		for(j=i+1;j<nCenterNum;j++)
		{
			dDistanceSquare=0.0;
			for(k=0;k<nCol;k++)
			{
				dDistanceSquare+=(mxInitialCenter(i,k)-mxInitialCenter(j,k))
					*(mxInitialCenter(i,k)-mxInitialCenter(j,k));				
			}
			mxCenterDistance(i,j)=dDistanceSquare;
			mxCenterDistance(j,i)=dDistanceSquare;
			if(dMinBetweenCenter>dDistanceSquare)		
			{
				dMinBetweenCenter=dDistanceSquare;
				iClosestCenterIndex_m=i;
				iClosestCenterIndex_n=j;
			}	
		}
	}

	//遍历所有数据，替换构造初始聚类中心
	double * dXM = new double[nCenterNum];//到各聚类中心的距离
	double dMinXM1, dMinXM2; //到初始聚类中心最近和次近距离
	int nMinXM1, nMinXM2;	 //到初始聚类中心最近和次近距离对应聚类中心序号
	for(i=nCenterNum;i<nRow;i++)
	{		
		//求到原始中心最近和次近距离
		dMinXM1=VeryBigValue;
		dMinXM2=VeryBigValue;
		nMinXM1=-1;
		nMinXM2=-1;
		for(j=0;j<nCenterNum;j++)
		{
			dDistanceSquare=0.0;
			for(k=0;k<nCol;k++)
			{
				dDistanceSquare+=(mx(i,k)-mxInitialCenter(j,k))
					*(mx(i,k)-mxInitialCenter(j,k));
			}
			dXM[j] = dDistanceSquare;
			
			if( dMinXM1>dDistanceSquare )
			{
				dMinXM2 = dMinXM1;
				nMinXM2 = nMinXM1;
				dMinXM1 = dDistanceSquare;
				nMinXM1 = j;
			}
			else if( dMinXM2>dDistanceSquare )
			{
				dMinXM2 = dDistanceSquare;
				nMinXM2 = j;
			}

		}
		// 比较并替换。对照SPSS文档Step1: a、b
		bool bReplace = false;
		int nReplaceIndex;
		if( dMinXM1>dMinBetweenCenter ) // 到各中心的最小距离大于中心点间的最小距离
		{
			if( dXM[iClosestCenterIndex_m]>dXM[iClosestCenterIndex_n] )
			{
				for(k=0;k<nCol;k++)
				{
					mxInitialCenter(iClosestCenterIndex_n,k)=mx(i,k);
				}				
				nReplaceIndex=iClosestCenterIndex_n;
			}
			else
			{
				for(k=0;k<nCol;k++)
				{
					mxInitialCenter(iClosestCenterIndex_m,k)=mx(i,k);
				}				
				nReplaceIndex=iClosestCenterIndex_m;
			}			
			bReplace = true;
		}
		else 
		{
			for(j=0;j<nCenterNum;j++)
			{
				//只要到次近中心的距离大于最近中心到其他中心的任意一个距离
				//则必然大于最近中心到其他中心的最小距离				
				if(j==nMinXM1)
					continue;
				if(dMinXM2>mxCenterDistance(nMinXM1,j))
				{
					for(k=0;k<nCol;k++)
					{
						mxInitialCenter(nMinXM1,k)=mx(i,k);
					}					
					bReplace=true;
					nReplaceIndex=nMinXM1;
					break;					
				}					
			}	
		}
	
		// 如果替换了聚类中心，重新计算新中心与其他中心的距离，并找出最小值
		if( bReplace )
		{
			for(s=0;s<nCenterNum;s++)
			{
				if(s==nReplaceIndex)
					continue;
				
				dDistanceSquare=0.0;
				for(k=0;k<nCol;k++)
				{
					dDistanceSquare+=(mxInitialCenter(s,k)-mxInitialCenter(nReplaceIndex,k))
						*(mxInitialCenter(s,k)-mxInitialCenter(nReplaceIndex,k));										
				}
				mxCenterDistance(s,nReplaceIndex)=dDistanceSquare;
				mxCenterDistance(nReplaceIndex,s)=dDistanceSquare;
			}	

			dMinBetweenCenter=VeryBigValue;
			iClosestCenterIndex_m=-1; 
			iClosestCenterIndex_n=-1;
			for(s=0;s<nCenterNum;s++)
			{
				for(k=s+1;k<nCenterNum;k++)
				{
					if(dMinBetweenCenter>mxCenterDistance(s,k))		
					{
						dMinBetweenCenter=mxCenterDistance(s,k);
						iClosestCenterIndex_m=s;
						iClosestCenterIndex_n=k;
					}			
				}
			}
		}
	}
	delete[] dXM;
	/////////////////////////////////初始化聚类中心完毕/////////////////////////////////////////
		

	//迭代聚类，直到聚类中心变化很小或者迭代次数大于预设最大次数
	CIntMatrix mxSampleBelongCenterIndex;	
	CDoubleMatrix mxOldCenter=mxInitialCenter;
	m_nEndCount=0;
	if(!UpdateClusterCenter(mx,mxOldCenter,mxCenter,mxSampleBelongCenterIndex))
		return false;
	m_nEndCount++;
	bool bIsClusterChanged;
	ostringstream oss;
	while(1)
	{
		//如果中止
		if(m_bAbort)
		{
			oss<<"快速聚类中止信息：迭代"<<m_nEndCount<<"次后被中止！\n";
			m_strAbortInfo=oss.str();
			return false;
		}
				
		bIsClusterChanged=false;		
		for( i=0; i<nCenterNum; i++ )
		{
			for( k=0; k<nCol; k++ )
			{
				if(fabs(mxCenter(i,k)-mxOldCenter(i,k))>dPrecision)
				{
					bIsClusterChanged=true;
					break;
				}				
			}
			if(bIsClusterChanged)
				break;
		}
		if(bIsClusterChanged==false||m_nEndCount>=nMaxRepeatTimes)
		{
			break;
		}
		
		mxOldCenter=mxCenter;
		if(!UpdateClusterCenter(mx,mxOldCenter,mxCenter,mxSampleBelongCenterIndex))
			return false;
		m_nEndCount++;
	}
	
	//结束计时
	_ftime(&timeEnd);
	ostringstream oss1;
	oss1<<"快速聚类共迭代"<<m_nEndCount<<"次，耗时"<<FormatTimeSpan(timeBegin,timeEnd);
	m_strRunInfo=oss1.str();
	
	return true;
}

//将样本归入距离最近的那一类,然后更新那一类的中心
//相比较原来的快速聚类代码，将归类和更新中心合并处理，可提高速度
//参数mxSampleBelongCenterIndex暂不使用
//输入：mx--输入样本数据；mxOldCenter--老聚类中心
//输出：mxCenter：更新的聚类中心
bool CKMeans_Algorithm::UpdateClusterCenter(CDoubleMatrix& mx,CDoubleMatrix& mxOldCenter,
		CDoubleMatrix& mxCenter,CIntMatrix& mxSampleBelongCenterIndex)
{
	int i,j,k;
	int nRow=mx.mrow();
	int nCol=mx.mcol();
	int nCenterNum=mxOldCenter.mrow();
	double dMin,dDistanceSquare;
	int nMinIndex;
	int *Sum=new int [nCenterNum];
	for(i=0;i<nCenterNum;i++)
		Sum[i]=0;
	
	//mxSampleBelongCenterIndex.create(nRow,1,-1);
	mxCenter.create(nCenterNum,nCol,0.0);

	//归类
	for(i=0;i<nRow;i++)
	{
		dMin=VeryBigValue;
		nMinIndex=-1;
		for(j=0;j<nCenterNum;j++)
		{
			dDistanceSquare=0.0;			
			for(k=0;k<nCol;k++)
			{
				dDistanceSquare+=(mx(i,k)-mxOldCenter(j,k))
					*(mx(i,k)-mxOldCenter(j,k));
			}
			if(dDistanceSquare<dMin)
			{
				dMin=dDistanceSquare;
				nMinIndex=j;
			}
		}		
		//mxSampleBelongCenterIndex(i,0)=nMinIndex;

		//累计属于各类的样本数和样本坐标和
		Sum[nMinIndex]+=1;
		for(j=0;j<nCol;j++)
		{
			mxCenter(nMinIndex,j)+=mx(i,j);
		}	
	}
	
	//求新中心
	for(i=0;i<nCenterNum;i++)
	{
		if(Sum[i]<1)
		{
			m_strErrorInfo="快速聚类错误信息：不重复样本数不应小于聚类中心数！\n";//应该是这个错误，但还需确认-cx
			m_bError=true;
			delete [] Sum;
			return false;
		}

		for(k=0;k<nCol;k++)
			mxCenter(i,k)/=Sum[i];
	}

	delete [] Sum; 
	return true;	
}

void CKMeans_Algorithm::Abort()
{
	m_bAbort=true;
}

string CKMeans_Algorithm::GetAbortInfo()
{
	return m_strAbortInfo;
}

bool CKMeans_Algorithm::IsAbort()
{
	return m_bAbort;
}

bool CKMeans_Algorithm::IsError()
{
	return m_bError;
}

string CKMeans_Algorithm::GetErrorInfo()
{
	return m_strErrorInfo;
}

//int CKMeans_Algorithm::GetEndCount()
//{
//	return m_nEndCount;
//}


string CKMeans_Algorithm::GetRunInfo()
{
	return m_strRunInfo;
}

string CKMeans_Algorithm::FormatTimeSpan(_timeb timeBegin,_timeb timeEnd)
{
	ostringstream oss;
	double dTimeSpan=(timeEnd.time+timeEnd.millitm/1000.0)
					-(timeBegin.time+timeBegin.millitm/1000.0);
	
	long nHour=long(dTimeSpan)/3600;
	long nMin=long(dTimeSpan-3600*nHour)/60;
	double dSec=long((dTimeSpan-3600*nHour-60*nMin)*1000+0.5)/1000.0;
	oss<<nHour<<"小时 "<<nMin<<"分钟 "<<dSec<<"秒";
	
	return oss.str();
}
