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

//���ܣ����پ����þ�������
//���룺mx--������������nCenterNum--������������bIsStd--�Ƿ���������ݱ�׼���������ã�
//		dPrecision--������ֹ���ȣ�nMaxRepeatTimes--����������
//�����mxCenter--����õ��ľ�������
bool CKMeans_Algorithm::KMeans(CDoubleMatrix& mx,int nCenterNum,bool bIsStd,double dPrecision,
					 int nMaxRepeatTimes,CDoubleMatrix& mxCenter)
{
	//��ʼ��ʱ
	struct _timeb timeBegin,timeEnd;
	_ftime(&timeBegin);	
	
	
	//���ݾ������ĸ���,�Զ�����ҳ����
	//��Ƚ��޴˶δ��룬�����ٶȸ��Ʋ����ԡ��ұȸ߼�ͳ���п��پ�������ٶ�Ҫ���ܶ࣬ԭ�����
	//����ʵ��(101200��¼,19�����ֶ�)��������60���ģ�18���ӣ�ԭ���پ������60���ģ�3��40��
	//by cx  2005-11-28
	if (nCenterNum >5)//����5����������ʱ
	{
//		mx.SetMaxPages(nCenterNum+1);  //������´˾䲻�ܱ���
	}
	//////////////////////////////////////////////////////////////////////////


	m_bAbort=false;
	m_bError=false;
	m_strAbortInfo="";
	m_strErrorInfo="";		
	m_nEndCount=0;

	//���ݼ��
	if(mx.mrow()<nCenterNum)
	{
		m_strErrorInfo="���پ��������Ϣ����������ӦС�ھ�����������\n";
		m_bError=true;
		return false;
	}
	
	/////////////////////////////��ʼ���������Ŀ�ʼ/////////////////////////////////////////////
	int i, j, k, s;
	int nRow=mx.mrow();
	int nCol=mx.mcol();
	CDoubleMatrix mxInitialCenter(nCenterNum,nCol);
	
	// ��ʼ��ǰnCenterNum����¼Ϊ��������
	for(i=0;i<nCenterNum;i++)
	{
		for(j=0;j<nCol;j++)
		{
			mxInitialCenter(i,j)=mx(i,j);
		}
	}
	
	// ����������ļ���໥���룬����¼������Сֵ
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

	//�����������ݣ��滻�����ʼ��������
	double * dXM = new double[nCenterNum];//�����������ĵľ���
	double dMinXM1, dMinXM2; //����ʼ������������ʹν�����
	int nMinXM1, nMinXM2;	 //����ʼ������������ʹν������Ӧ�����������
	for(i=nCenterNum;i<nRow;i++)
	{		
		//��ԭʼ��������ʹν�����
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
		// �Ƚϲ��滻������SPSS�ĵ�Step1: a��b
		bool bReplace = false;
		int nReplaceIndex;
		if( dMinXM1>dMinBetweenCenter ) // �������ĵ���С����������ĵ�����С����
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
				//ֻҪ���ν����ĵľ������������ĵ��������ĵ�����һ������
				//���Ȼ����������ĵ��������ĵ���С����				
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
	
		// ����滻�˾������ģ����¼������������������ĵľ��룬���ҳ���Сֵ
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
	/////////////////////////////////��ʼ�������������/////////////////////////////////////////
		

	//�������ֱ࣬���������ı仯��С���ߵ�����������Ԥ��������
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
		//�����ֹ
		if(m_bAbort)
		{
			oss<<"���پ�����ֹ��Ϣ������"<<m_nEndCount<<"�κ���ֹ��\n";
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
	
	//������ʱ
	_ftime(&timeEnd);
	ostringstream oss1;
	oss1<<"���پ��๲����"<<m_nEndCount<<"�Σ���ʱ"<<FormatTimeSpan(timeBegin,timeEnd);
	m_strRunInfo=oss1.str();
	
	return true;
}

//��������������������һ��,Ȼ�������һ�������
//��Ƚ�ԭ���Ŀ��پ�����룬������͸������ĺϲ�����������ٶ�
//����mxSampleBelongCenterIndex�ݲ�ʹ��
//���룺mx--�����������ݣ�mxOldCenter--�Ͼ�������
//�����mxCenter�����µľ�������
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

	//����
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

		//�ۼ����ڸ���������������������
		Sum[nMinIndex]+=1;
		for(j=0;j<nCol;j++)
		{
			mxCenter(nMinIndex,j)+=mx(i,j);
		}	
	}
	
	//��������
	for(i=0;i<nCenterNum;i++)
	{
		if(Sum[i]<1)
		{
			m_strErrorInfo="���پ��������Ϣ�����ظ���������ӦС�ھ�����������\n";//Ӧ����������󣬵�����ȷ��-cx
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
	oss<<nHour<<"Сʱ "<<nMin<<"���� "<<dSec<<"��";
	
	return oss.str();
}
