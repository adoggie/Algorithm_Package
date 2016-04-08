// Statistics.cpp: implementation of the CStatistics class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Statistics.h"
#include "FreqConf.h"

extern double ptdist(long n, double q);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStatistics::CStatistics()
{
	flag_prs=0;
	flag_sum=0;
	flag_mean=0;
	flag_vari=0;
	flag_stdv=0;
	//add by liytao 20030108
	pWeight=NULL;
	dbweight=0;
	bRet=FALSE;
	//add end

}


CStatistics::CStatistics(TDataSet * x , long n)
{
    array = x;
	num = n;
	flag_prs=0;
	flag_sum=0;
	flag_mean=0;
	flag_vari=0;
	flag_stdv=0;
	//add by liytao 20030108
	pWeight=NULL;
	dbweight=0;
	bRet=FALSE;
	//add end
}

void CStatistics::SetData(TDataSet *pArray,int nDataCount)
{
    array = pArray;
	num = nDataCount;
	flag_prs=0;
	flag_sum=0;
	flag_mean=0;
	flag_vari=0;
	flag_stdv=0;
}
//add by liytao 20030108
void CStatistics::SetData(TDataSet *pArray,int nDataCount,double *pW)
{
    array = pArray;
	num = nDataCount;
	flag_prs=0;
	flag_sum=0;
	flag_mean=0;
	flag_vari=0;
	flag_stdv=0;
	pWeight=pW;

}

void CStatistics::SetData(TDataSet *pArray,int nDataCount,double dbW,double *pW)
{
    array = pArray;
	num = nDataCount;
	flag_prs=0;
	flag_sum=0;
	flag_mean=0;
	flag_vari=0;
	flag_stdv=0;
	dbweight=dbW;
	pWeight=pW;

}
//add end

CStatistics::~CStatistics()
{
}

void CStatistics::Swap(TDataSet &t1, TDataSet &t2)
{
	TDataSet t = t1;
	t1 = t2;
	t2 = t;
}
//升序－快速排序,xds-20060118
void CStatistics::QSortValue(TDataSet *pData, int nLow, int nHigh)
{
	int i,j;
	TDataSet t;

	i = nHigh;
	j = nLow;
	t = pData[(int)((i+j)/2)];
	do{
		while(pData[j] < t)  j++;
		while(pData[i] > t)  i--;
		if(i>=j)
		{
			if(i != j)
			{
				Swap(pData[i],pData[j]);
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(nLow < i)  QSortValue(pData,nLow,i);
	if(j < nHigh) QSortValue(pData,j,nHigh);
}
//升序－快速排序,xds-20060118
void CStatistics::QSortIndex(TDataSet *pData, int nLow, int nHigh)
{
	int i,j;
	TDataSet t;

	i = nHigh;
	j = nLow;
	t = pData[(int)((i+j)/2)];
	do{ 
		while((t.iIndex<0 && pData[j].iIndex>0) || pData[j] < t)  j++;
		while((t.iIndex<0 && pData[j].iIndex>0) || pData[i] > t)  i--;
		if(i>=j)
		{
			if(i != j)
			{
				Swap(pData[i],pData[j]);
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(nLow < i)  QSortIndex(pData,nLow,i);
	if(j < nHigh) QSortIndex(pData,j,nHigh);
}
//////////////////////////////////////////
/// 排序
void CStatistics::prshl(TDataSet *p, int n,BOOL reSort)   //实数的shell（希尔）排序
{
	if ((flag_prs && !reSort) || n < 1)
		return;
	QSortValue(p,0,n-1);
	return;

    int k=0,j=0,i=0;
    TDataSet t;
    if (flag_prs==0 || reSort)  //如果标志位表明没有排序才进行
	{
		k=n/2;
		while (k>0)
		{ 
			for (j=k; j<=n-1; j++)
			{
				t=p[j]; i=j-k;
				while ((i>=0)&&(p[i]>t))
				{
					p[i+k]=p[i];
					i=i-k;
				}
				p[i+k]=t;
			}
			k=k/2;
		}
		flag_prs=1;
	}
    return;
}

//////////////////////////////////////////
/// 排序
//add by liytao 20030106
//
//
void CStatistics::prshl(BOOL bWithMissing,TDataSet *p, int n,BOOL reSort)   //实数的shell（希尔）排序
{
	if ((flag_prs && !reSort) || n < 1)
		return;
	if (!bWithMissing)
	{
		QSortValue(p,0,n-1);
	}
	else
	{
		QSortIndex(p,0,n-1);
	}
	return;

	if (!bWithMissing)
	{
		prshl(p,n,reSort);
		return;
	}

    int k=0,j=0,i=0;
    TDataSet t;
    if (flag_prs==0 || reSort)  //如果标志位表明没有排序才进行
	{
		k=n/2;
		while (k>0)
		{ 
			for (j=k; j<=n-1; j++)
			{
				t=p[j];
				
				i=j-k;
				//while ((i>=0)&&(t.iIndex<0||p[i]>t))
				while ((i>=0)&&((t.iIndex<0&&p[i].iIndex>0)||p[i]>t))// modified by liytao 20030228
				{
						p[i+k]=p[i];
						i=i-k;
				}
				p[i+k]=t;
			}
			k=k/2;
		}
		flag_prs=1;
	}
    return;
}

//add end

double CStatistics::f_variance(int nType)
{
	if (!flag_vari)
	{
		int i=0;
		double v_mean=0,re_value=0;
		v_mean=f_mean(nType);
		for (i=0;i<num;i++)
		{
			//re_value+=(array[i].dValue-v_mean)*(array[i].dValue-v_mean); modified by liytao 20030108
			if(pWeight!=NULL&&nType==1)
				re_value+=pWeight[array[i].iIndex-1]*(array[i].dValue-v_mean)*(array[i].dValue-v_mean);
			else
				re_value+=(array[i].dValue-v_mean)*(array[i].dValue-v_mean);
		}
		flag_vari=TRUE;
		//v_vari=re_value/(num-1);		 modified by  liytao 20030108
		if(pWeight!=NULL&&nType==1)
			v_vari=re_value/(dbweight-1);		 
		else
			v_vari=re_value/(num-1);
	}
	return v_vari;
} 

double CStatistics::f_stadeviation(int nType)
{
	if (!flag_stdv)
	{
		if (flag_vari==0) 
		{
			v_stdv=sqrt(f_variance(nType));
		}
		else
			v_stdv=sqrt(v_vari);
		flag_stdv=TRUE;
	}
    return v_stdv; 
}

double CStatistics::f_mode(int& MaxCount,int nType)
{
	double ret_value=0,dCurValue=0;
	int max_num=1,j=1;  //开始有一个
	
	double max_dbnum=0;
	double dbj=0;  
	int i=0;
    prshl(array,num);
	if(pWeight!=NULL&&nType==1)
	{
		max_dbnum=pWeight[array[0].iIndex-1];
		dbj=pWeight[array[0].iIndex-1];
	}
	ret_value=array[0].dValue;
	dCurValue=array[0].dValue;
	for (i=1;i<num;i++)
	{
        if(dCurValue==array[i].dValue)  
		{	//j++; modified by liytao 20030108
			if(pWeight!=NULL&&nType==1)
				dbj=dbj+pWeight[array[i].iIndex-1];
			else
				j++;
		}
		else
		{
			//if(j> max_num)modified by liytao 20030108
			//max_dbnum=j;modified by liytao 20030108
			if(pWeight!=NULL&&nType==1)
			{
				if(dbj> max_dbnum)
				{  
					max_dbnum=dbj;
					ret_value=dCurValue;
				}
			}
			else
			{
				if(j> max_num)
				{
					max_num=j;
					ret_value=dCurValue;
				}	
				
			}
			//dbj=1;modified by liytao 20030108
			if(pWeight!=NULL&&nType==1)
				dbj=pWeight[array[i].iIndex-1];
			else
				j=1;
			dCurValue=array[i].dValue;
		}
	} 
	//add by liytao 20030210
	if(pWeight!=NULL&&nType==1)
	{
		if(dbj> max_dbnum)
		{  
			max_dbnum=dbj;
			ret_value=dCurValue;
		}
	}
	else
	{
		if(j> max_num)
		{
			max_num=j;
			ret_value=dCurValue;
		}	
		
	}
	
	MaxCount = max_num;
	//add end
	return ret_value;
}
double CStatistics::f_mode(int nType)
{
	double ret_value=0,dCurValue=0;
	int max_num=1,j=1;  //开始有一个

	double max_dbnum=0;
	double dbj=0;  
	int i=0;
    prshl(array,num);
	if(pWeight!=NULL&&nType==1)
	{
		max_dbnum=pWeight[array[0].iIndex-1];
		dbj=pWeight[array[0].iIndex-1];
	}
	ret_value=array[0].dValue;
	dCurValue=array[0].dValue;
	for (i=1;i<num;i++)
	{
        if(dCurValue==array[i].dValue)  
		{	//j++; modified by liytao 20030108
			if(pWeight!=NULL&&nType==1)
			dbj=dbj+pWeight[array[i].iIndex-1];
			else
				j++;
		}
		else
		{
			//if(j> max_num)modified by liytao 20030108
				//max_dbnum=j;modified by liytao 20030108
			if(pWeight!=NULL&&nType==1)
			{
				if(dbj> max_dbnum)
				{  
					max_dbnum=dbj;
					ret_value=dCurValue;
				}
			}
			else
			{
				if(j> max_num)
				{
					max_num=j;
					ret_value=dCurValue;
				}	

			}
			//dbj=1;modified by liytao 20030108
			if(pWeight!=NULL&&nType==1)
			 dbj=pWeight[array[i].iIndex-1];
			else
				j=1;
			dCurValue=array[i].dValue;
		}
	} 
//add by liytao 20030210
	if(pWeight!=NULL&&nType==1)
	{
		if(dbj> max_dbnum)
		{  
			max_dbnum=dbj;
			ret_value=dCurValue;
		}
	}
	else
	{
		if(j> max_num)
		{
			max_num=j;
			ret_value=dCurValue;
		}	

	}


//add end
	return ret_value;
}

double CStatistics::f_skewness(int nType)
{
	double mean=0,stdeviation=0,M=0;
	int i=0;
	
	mean=f_mean(nType);
	stdeviation=f_stadeviation(nType);
	for(i=0;i<num;i++)
		//M+=pow((array[i].dValue-mean),3); modified by liytao 20030108
		if(pWeight!=NULL&&nType==1)
		 M+=pWeight[array[i].iIndex-1]*pow((array[i].dValue-mean),3);
		else
			M+=pow((array[i].dValue-mean),3);
	//return (num*M)/((num-1.0)*(num-2.0)*stdeviation*stdeviation*stdeviation); modified by liytao 20030108
	if(pWeight!=NULL&&nType==1)
		return (dbweight*M)/((dbweight-1.0)*(dbweight-2.0)*stdeviation*stdeviation*stdeviation);
	else
		return (num*M)/((num-1.0)*(num-2.0)*stdeviation*stdeviation*stdeviation); 
	//	}
}

double CStatistics::f_kurtosis(double &dSeg1,double &dSeg2,int nType)
{
	double intv1=0,intv2=0;
	
	double M4=0,M3=0,M2=0,std,mean,mid_mul; 
    int i;
	mean=f_mean(nType);
    std=f_stadeviation(nType);       	
	for(i=0;i<num;i++)
	{
		double value=array[i].dValue-mean;
        mid_mul=pow(value,2);//*(array[i]-mean);
        //M2+=mid_mul; modified by liytao 20030108
		if(pWeight!=NULL&&nType==1)
			M2+=pWeight[array[i].iIndex-1]*mid_mul;
		else
			M2+=mid_mul;
		mid_mul *= value;
		//M3+=mid_mul;modified by liytao 20030108
		if(pWeight!=NULL&&nType==1)
			M3+=pWeight[array[i].iIndex-1]*mid_mul;
		else
			M3+=mid_mul;
		mid_mul *=value;
		//M4+=mid_mul;modified by liytao 20030108
		if(pWeight!=NULL&&nType==1)
			M4+=pWeight[array[i].iIndex-1]*mid_mul;
		else
			M4+=mid_mul;
	}
	
   /* dSeg1=sqrt((num*(num-1.0))/((num-2.0)*(num+1.0)*(num+3.0)/6.0)); modified by liytao 20030108
    
    intv1=4.0*(num*num-1)*dSeg1*dSeg1;
    intv2=(num-3)*(num+5);
	
	dSeg2=sqrt((4.0*(num*num-1.0)*dSeg1*dSeg1)/((num-3.0)*(num+5.0)));
	
    return (num*(num+1.0)*M4-3*M2*M2*(num-1.0))/((num-1.0)*(num-2.0)*(num-3.0)*std*std*std*std);*/
	if(pWeight!=NULL&&nType==1)
	{
		dSeg1=sqrt((dbweight*(dbweight-1.0))/((dbweight-2.0)*(dbweight+1.0)*(dbweight+3.0)/6.0));
    
		intv1=4.0*(dbweight*dbweight-1)*dSeg1*dSeg1;
		intv2=(dbweight-3)*(dbweight+5);
	
		dSeg2=sqrt((4.0*(dbweight*dbweight-1.0)*dSeg1*dSeg1)/((dbweight-3.0)*(dbweight+5.0)));
	
		 return (dbweight*(dbweight+1.0)*M4-3*M2*M2*(dbweight-1.0))/((dbweight-1.0)*(dbweight-2.0)*(dbweight-3.0)*std*std*std*std)+3;
	}
	else
	{
		 dSeg1=sqrt((num*(num-1.0))/((num-2.0)*(num+1.0)*(num+3.0)/6.0));
    
		 intv1=4.0*(num*num-1)*dSeg1*dSeg1;
		intv2=(num-3)*(num+5);
	
		dSeg2=sqrt((4.0*(num*num-1.0)*dSeg1*dSeg1)/((num-3.0)*(num+5.0)));
	
		return (num*(num+1.0)*M4-3*M2*M2*(num-1.0))/((num-1.0)*(num-2.0)*(num-3.0)*std*std*std*std)+3;

	}						
}

double CStatistics::f_mean(int nType)
{
	if (flag_sum==0) 
	//v_mean=f_sum()/num; modified by liytao 20030108
	{
		if( pWeight!=NULL&&nType==1)
			v_mean=f_sum(1)/dbweight;
		else
			v_mean=f_sum()/num;
	}
	else
		//v_mean=v_sum/num;modified by liytao 20030108
	{
		if(pWeight!=NULL&&nType==1)
			 v_mean=v_sum/dbweight;
		else
			v_mean=v_sum/num;
	}
	flag_mean=1;
	return  v_mean;     
}

double CStatistics::f_medium(int nType)
//注意如果个数有7个，那么应该是是取第四个值。但此时的i应该是3。
{
	if ( pWeight!=NULL&&nType==1)
		return f_percentile(0.5,nType);


	prshl(array,num);
    if ((num%2)!=0 )  //奇数个
	{  
		return (array[(num-1)/2].dValue);
	}
	else
	{ 
        return ((array[num/2].dValue+array[num/2-1].dValue)/2);
	}
}

double CStatistics::f_sum(int nType)
{
    if (flag_sum==0)
	{
		double Sum = 0.0;
		for(long i=0; i<num; i++)
		{
			//Sum += array[i].dValue; modified by liytao 20030108
			if(pWeight!=NULL&&nType==1)
				Sum += array[i].dValue*pWeight[array[i].iIndex-1]; 
			else
				Sum += array[i].dValue;
		}
		flag_sum=1;
		v_sum=Sum;
	}
    return v_sum;
}

double CStatistics::f_max()
{
    if (flag_prs==1)  //已经排序好
	{      
		return array[num-1].dValue;	
	}
	else
	{
		double ret_val=array[0].dValue;
		for(int i=1;i<num;i++)
		{
			if (array[i].dValue>ret_val)
			{
				ret_val=array[i].dValue;
			}
		}
		return ret_val;
	}
}

double CStatistics::f_min()
{
    if (flag_prs==1)  //已经排序好
	{      
		return array[0].dValue;	
	}
	else
	{
		double ret_val=array[0].dValue;
		for(int i=1;i<num;i++)
		{
			if (array[i].dValue<ret_val)
			{
				ret_val=array[i].dValue;
			}
		}
		return ret_val;
	}
}

double CStatistics::f_sem(int nType)
{
	double v_sem=0;
	if (flag_stdv==0)
		//v_sem=f_stadeviation()/(sqrt(num)); modified by liytao 20030108
	{
		if(pWeight!=NULL&&nType==1)
			v_sem=f_stadeviation(1)/(sqrt(dbweight));
		else
			v_sem=f_stadeviation(nType)/(sqrt(num));
	}
    else
		//v_sem=v_stdv/(sqrt(num)); modified by liytao 20030108
	{
		if(pWeight!=NULL&&nType==1)
			v_sem=v_stdv/(sqrt(dbweight));
		else
			v_sem=v_stdv/(sqrt(num)); 
	}
	return v_sem;
}

double*  CStatistics::Describle(TDataSet *pArray,int nDataCount,int *pMethodS,int nMCount,int nType)//CDataInterface *pDataInterface)
{
	num=nDataCount;
	array=pArray;
	flag_prs=flag_sum=flag_mean=flag_vari=flag_stdv=0;	
	double dEskewnesserror=0,dEkurtosisError=0,*rtValue=new double[nMCount],
		dEkurtosis=f_kurtosis(dEskewnesserror,dEkurtosisError,nType);
	int nRes=0;	
	BOOL isRight=FALSE;
	for(int kk=0;kk<nMCount;kk++)
	{
		double result=0.0;			
		switch(pMethodS[kk])
		{
		case EXPLORE_ERANGE:        //描述分析-范围

			result=f_range();
			break;
		case EXPLORE_EMINIMUM:      //描述分析-最小值
			result=f_min();
			break;
		case EXPLORE_EMEDIUM:      //描述分析-中位数
			result=f_medium(nType);
			break;
		case EXPLORE_EMAXIMUM:      //描述分析-最大值
			result=f_max();
			break;
		case EXPLORE_ESUM:          //描述分析-总和
			result=f_sum(nType);
			break; 
		case EXPLORE_EMEAN:         //描述分析-平均值
			result=f_mean(nType);
			break;
		case EXPLORE_EMEANERROR:    //描述分析-平均误差
			result=f_sem(nType);
			break;
		case EXPLORE_EMODE:         //描述分析-众数
			result=f_mode(nType);
			break;
		case EXPLORE_ETRIMMED:      //描述分析-5%截尾均值
			result=f_trimmean(nType);
			isRight=bRet;
			break;
		case EXPLORE_ESTD:          //描述分析-标准离差
			result=f_stadeviation(nType);
			break;
		case EXPLORE_EVARIANCE:     //描述分析-方差
			result=f_variance(nType);
			break;
		case EXPLORE_ESKEWNESS:     //描述分析-偏态系数
			result=f_skewness(nType);
			break;
		case EXPLORE_ESKEWNESSERROR://描述分析-偏斜系数误差
			result=dEskewnesserror;
			break;
		case EXPLORE_EKURTOSIS:     //描述分析-峰度
			result=dEkurtosis;
			break;
		case EXPLORE_EKURTOSISERROR://描述分析-峰度误差
			result=dEkurtosisError;
			break;
		case EXPLORE_EINTERRANGE:   //描述分析-区间范围
			result=f_intqr(nType);
			break;
			
		default:
			break;						
		}
		rtValue[kk]=result;
	}
	bRet=isRight;	
	return rtValue;
}
//add by liytao 20030108
/*double*  CStatistics::Describle(TDataSet *pArray,int nDataCount,int *pMethodS,int nMCount,double weight[],int nType)//CDataInterface *pDataInterface)
{
	num=nDataCount;
	array=pArray;
	flag_prs=flag_sum=flag_mean=flag_vari=flag_stdv=0;	
	double dEskewnesserror,dEkurtosisError,*rtValue=new double[nMCount],
		dEkurtosis=f_kurtosis(dEskewnesserror,dEkurtosisError,nType);
	int nRes=0;	
	for(int kk=0;kk<nMCount;kk++)
	{
		double result=0.0;			
		switch(pMethodS[kk])
		{
		case EXPLORE_ERANGE:        //描述分析-范围
			result=f_range();
			break;
		case EXPLORE_EMINIMUM:      //描述分析-最小值
			result=f_min();
			break;
		case EXPLORE_EMEDIUM:      //描述分析-中位数
			result=f_medium(nType);
			break;
		case EXPLORE_EMAXIMUM:      //描述分析-最大值
			result=f_max();
			break;
		case EXPLORE_ESUM:          //描述分析-总和
			result=f_sum(nType);
			break; 
		case EXPLORE_EMEAN:         //描述分析-平均值
			result=f_mean(nType);
			break;
		case EXPLORE_EMEANERROR:    //描述分析-平均误差
			result=f_sem(nType);
			break;
		case EXPLORE_EMODE:         //描述分析-众数
			result=f_mode(nType);
			break;
		case EXPLORE_ETRIMMED:      //描述分析-5%截尾均值
			//add by liytao 20030225
			//add end
			//result=f_trimmean(nType,&nRes);
			bRet=FALSE;
			result=f_trimmean(nType);
			break;
		case EXPLORE_ESTD:          //描述分析-标准离差
			result=f_stadeviation(nType);
			break;
		case EXPLORE_EVARIANCE:     //描述分析-方差
			result=f_variance(nType);
			break;
		case EXPLORE_ESKEWNESS:     //描述分析-偏态系数
			result=f_skewness(nType);
			break;
		case EXPLORE_ESKEWNESSERROR://描述分析-偏斜系数误差
			result=dEskewnesserror;
			break;
		case EXPLORE_EKURTOSIS:     //描述分析-峰度
			result=dEkurtosis;
			break;
		case EXPLORE_EKURTOSISERROR://描述分析-峰度误差
			result=dEkurtosisError;
			break;
		case EXPLORE_EINTERRANGE:   //描述分析-区间范围
			result=f_intqr(nType);
			break;
			
		default:
			break;						
		}
		rtValue[kk]=result;
	}
	
	return rtValue;
}*/
//add end
int CStatistics::achol(double a[], int n, int m, double d[])
{
	int i,j,k,u,v;
    if ((a[0]+1.0==1.0)||(a[0]<0.0))
	{return(-2);}
    a[0]=sqrt(a[0]);
    for (j=1; j<=n-1; j++) a[j]=a[j]/a[0];
    for (i=1; i<=n-1; i++)
	{ u=i*n+i;
	for (j=1; j<=i; j++)
	{ v=(j-1)*n+i;
	a[u]=a[u]-a[v]*a[v];
	}
	if ((a[u]+1.0==1.0)||(a[u]<0.0))
	{return(-2);}
	a[u]=sqrt(a[u]);
	if (i!=(n-1))
	{ for (j=i+1; j<=n-1; j++)
	{ v=i*n+j;
	for (k=1; k<=i; k++)
		a[v]=a[v]-a[(k-1)*n+i]*a[(k-1)*n+j];
	a[v]=a[v]/a[u];
	}
	}
	}
    for (j=0; j<=m-1; j++)
	{ d[j]=d[j]/a[0];
	for (i=1; i<=n-1; i++)
	{ u=i*n+i; v=i*m+j;
	for (k=1; k<=i; k++)
		d[v]=d[v]-a[(k-1)*n+i]*d[(k-1)*m+j];
	d[v]=d[v]/a[u];
	}
	}
    for (j=0; j<=m-1; j++)
	{ u=(n-1)*m+j;
	d[u]=d[u]/a[n*n-1];
	for (k=n-1; k>=1; k--)
	{ u=(k-1)*m+j;
	for (i=k; i<=n-1; i++)
	{ v=(k-1)*n+i;
	d[u]=d[u]-a[v]*d[i*m+j];
	}
	v=(k-1)*n+k-1;
	d[u]=d[u]/a[v];
	}
	}
    return(2);
}


double CStatistics::f_range()  //最大减最小
{
	return (f_max()-f_min());
}


double CStatistics::f_intqr(int nType)  //区间差
{
	return (f_percentile(0.75,nType)-f_percentile(0.25,nType));
}

double CStatistics::f_percentile(double p,int nType) 
// 输入0.0<=p<=1 如 0.5为medium;0.75为Q3;0.25为Q1
{
	//add by liyt20030116
	if (pWeight!=NULL&&nType==1)
	{
		prshl(array,num);

		if (num<=0)
			return 0;
		//add by liytao 20030209
		if( num==1)
			return array[0].dValue;
		//add end
		double *tempw=new double[num];

		tempw[0]=pWeight[array[0].iIndex-1];
		int i;
		for (i=1;i<num;i++)
			tempw[i]=tempw[i-1]+pWeight[array[i].iIndex-1];
		
		double tc1=0,tc2=0,g1star=0,g2star=0,g1=0,g2=0,ret=0;
		int j=0,k1=0,k2=0;
		tc1=dbweight*p;
		tc2=(dbweight+1)*p;

	 	k1 = 0;
	    k2 = 0;
		for (i=0;i<num-1;i++)
			if (tempw[i]<=tc1&&tempw[i+1]>tc1)
				k1=i;
		for (i=0;i<num-1;i++)
			if (tempw[i]<=tc2&&tempw[i+1]>tc2)
				k2=i;
	    g1star = tc1 - tempw[k1];
		g2star = tc2 - tempw[k2];
	    g1       = g1star / pWeight[array[k1+1].iIndex-1];
        g2       = g2star / pWeight[array[k2+1].iIndex-1];
	    if(g2star>=1)
			ret= array[k1+1].dValue;
	    else
		{
			if( pWeight[array[k2+1].iIndex-1]>=1)
				ret=(1-g2star)*array[k2].dValue + g2star*array[k2+1].dValue;           
			else    
				ret = (1-g2)*array[k2].dValue + g2*array[k2+1].dValue;
		}
        if(ret<array[0].dValue)
            ret = array[0].dValue;
        if(ret>array[num-1].dValue)
            ret = array[num-1].dValue;
		delete [] tempw;//add by liytao 20030227
		return ret;
	}

	else
	{
		//add end
		double k=p*num;
		int i=(int)ceil(k);
		prshl(array,num);
		if(num%2==0)//如果是偶数
			return (i==k)?((array[i].dValue+array[i-1].dValue)/2):array[i-1].dValue;
		else
		{
			int index=(long)k;
			double dLeave=k-index;
			if(p==0.25)
				return (dLeave>0.5)?(array[index+1].dValue+array[index].dValue)/2:array[index].dValue;
			else if(p==0.5)
				return array[index].dValue;
			else if(p==0.75)
				return (dLeave<0.5)?(array[index].dValue+array[index-1].dValue)/2:array[index].dValue;
		}
		return (i==k)?((array[i].dValue+array[i-1].dValue)/2):array[i-1].dValue;
	}
}

int CStatistics::f_percentile(double* &rtValue,int nType)
{
	double percent[7]={0.05,0.1,0.25,0.50,0.75,0.90,0.95};
	rtValue=new double[7];
	for(int ii=0;ii<7;ii++)
		//rtValue[ii]=f_percentile(percent[ii]);modified by liytao 20030116
		rtValue[ii]=f_percentile(percent[ii],nType);
	
	return 7;
}

int CStatistics::f_extrmin(TDataSet* &re_value) //返回最小的结构体
//TDataSet * CStatistics::f_extrmin() //返回最小的结构体
{
	//	TDataSet* re_value;
    prshl(array,num);
	if (num<10)
	{
		re_value=new TDataSet[num/2];
		for(int i=0;i<num/2;i++)
		{
			re_value[i].iIndex=array[i].iIndex;
			re_value[i].dValue=array[i].dValue;
		}
		return num/2;
	}
	else
	{
		re_value=new TDataSet[5];
		for(int i=0;i<5;i++)
		{
			re_value[i].iIndex=array[i].iIndex;
			re_value[i].dValue=array[i].dValue;
		}
		return 5;
	}
}

//返回最大值的结构体
int CStatistics::f_extrmax(TDataSet* &re_value) 
{
	int j=0;
	prshl(array,num);
	if (num<10)
	{
		re_value=new TDataSet[num/2];
		for(int i=(num-1);i>(num-num/2-1);i--)
		{
			re_value[j].iIndex=array[i].iIndex;
			re_value[j].dValue=array[i].dValue;
			j++;
		}
		return num/2;
	}
	else
	{
        re_value=new TDataSet[5];
		for(int i=(num-1);i>(num-6);i--)
		{
			re_value[j].iIndex=array[i].iIndex;
			re_value[j].dValue=array[i].dValue;
			j++;
		}
		return 5;
	}
}

//  求置信区间的值
//  p为用户输入的置信区间的百分比值如95
void CStatistics::f_conf_mean(double p, double &x, double &y,int nType)
{
	double q=((double)(1-p/100.0))/2.0;
	//如果用户输入的为95，则调用函数的参数应为 2.5%
	//double re_value=ptdist(num-1, q); modified by liytao 20030109
	double re_value=0;
	/*if (pWeight==NULL)
		re_value=ptdist(num-1, q);
	else
		re_value=ptdist((int)(dbweight-1+0.5), q); modified  by liytao 20030311*/
	if (pWeight==NULL)
		re_value=-tinv(q,(double)(num-1));
	else
		re_value=-tinv(q,(double)(dbweight-1));

	re_value=re_value*f_sem(nType);
	if	(flag_mean==1)
	{
		x=v_mean+re_value;
		y=v_mean-re_value;
	}
	else
	{
		/*x=f_mean()+re_value;
		y=f_mean()-re_value; modified by liytao 20030211*/
		x=f_mean(nType)+re_value;
		y=f_mean(nType)-re_value;
	}
}

int CStatistics::f_DesFreq(TFreqArray* &re_array)  //frequence 
{
	
    prshl(TRUE,array,num);
	
	int j=1,k=1;
	CTString change_str;
	TDataSet TempValue=array[0];
	double TempCum=0.0,tran_value=0.0;
	
	//计算当前数组中有多少组数据
	int i;
    for(i=1;i<num;i++)
	{
		if (TempValue!=array[i])
		{
			j++;
			TempValue=array[i];
		}
	}
	j++;//  返回个数  最后还有一个求和的记录
	
	re_array=new TFreqArray[j];
	TempValue=array[0];
	
	int m=0;  
    for (i=1;i<num;i++)
	{
		if (TempValue==array[i])
			k++;
		else
		{
			if(k>0)
			{
				//add by liytao 20030106
				if(array[i-1].iIndex<0)
					re_array[m].ListValue=_T("  缺失值"); 
					
				//add end
				else if(array[i-1].strVal=="" && array[i].strVal=="")
				{
					change_str.Format("%f",array[i-1].dValue);
					re_array[m].ListValue=change_str; 
				}
				else
					re_array[m].ListValue=array[i-1].strVal; 
				
				change_str="";
				change_str.Format("%i",k);
				re_array[m].Number=change_str; 
            
				change_str="";
				tran_value=((k*1.0)/num)*100; //整数除法的0
				change_str.Format("%.5f",tran_value);
				re_array[m].Percent=change_str;
				
				
				TempCum=TempCum+tran_value;
				change_str="";
				change_str.Format("%.5f",TempCum);
				
				re_array[m].CumPercent=change_str;
 				m++;
			}			
           TempValue=array[i];
			k=1;
		}
	}
	if (k>0)  //当循环跳出来的时候，最后一组数据还需要填写
	{
//		change_str.Format("%.5f",array[i-1].dValue);
//		re_array[m].ListValue=change_str; 
		//add by liytao 20030106
		if(array[i-1].iIndex<0)
			re_array[m].ListValue=_T("  缺失值"); 
			
		//add end
		else if(array[i-1].strVal=="" && TempValue.strVal=="")
		{
			change_str.Format("%f",array[i-1].dValue);
			re_array[m].ListValue=change_str; 
		}
		else
			re_array[m].ListValue=array[i-1].strVal; 	
		
		change_str="";
		change_str.Format("%i",k);
		re_array[m].Number=change_str; 
		
		change_str="";
		tran_value=((k*1.0)/num)*100;
		change_str.Format("%.5f",tran_value);
		re_array[m].Percent=change_str;
		
		TempCum=TempCum+tran_value;
		change_str="";
		change_str.Format("%.5f",TempCum);
		
		re_array[m].CumPercent=change_str;
		m++;
	}
	//    j++; //  返回个数  最后还有一个求和的记录
	//    TFreqArray *temp_array=new TFreqArray;
	j=m+1;//modify by liytao 20030106
	re_array[j-1].ListValue="  总计";
	change_str="";
	change_str.Format("%i",num);
	re_array[j-1].Number=change_str;
	
	change_str="";
	change_str.Format("%.5f",100.0);
	re_array[j-1].Percent=change_str;
	
	return j;
}
//add by liytao 20030106
int CStatistics::f_DesFreq(TFreqArray* &re_array,double weight[])  //frequence 
{
	
    prshl(TRUE,array,num);
	
	int j=1;
	double dbk;
	if (array[0].iIndex>=0)
		dbk= weight[array[0].iIndex-1];
	else 
		dbk= weight[-array[0].iIndex-1];
	CTString change_str;
	TDataSet TempValue=array[0];
	double TempCum=0.0,tran_value=0.0;
	//int nTotal=k;// add by liytao 20030106
	double dbTotal=dbk;
	//计算当前数组中有多少组数据
	int i;
    for(i=1;i<num;i++)
	{
		if (array[i].iIndex>=0)
			dbTotal+= weight[array[i].iIndex-1];
		else 
			dbTotal+= weight[-array[i].iIndex-1];
		if (TempValue!=array[i])
		{
			j++;
			TempValue=array[i];
		}
	}
	j++;//  返回个数  最后还有一个求和的记录
	
	re_array=new TFreqArray[j];
	TempValue=array[0];
	
	int m=0;  
    for (i=1;i<num;i++)
	{
		if (TempValue==array[i])
		{
			if (array[i].iIndex>=0)
				dbk=dbk+ weight[array[i].iIndex-1];
			else 
				dbk=dbk+ weight[-array[i].iIndex-1];


		}
		else
		{

			//add by liytao 20030106
			if(dbk>0)
			{
				if(array[i-1].iIndex<0)
					re_array[m].ListValue=_T("  缺失值"); 
					
				//add end
				else if(array[i-1].strVal=="" && array[i].strVal=="")
				{
					change_str.Format("%f",array[i-1].dValue);
					re_array[m].ListValue=change_str; 
				}
				else
					re_array[m].ListValue=array[i-1].strVal; 
				
				change_str="";
				//change_str.Format("%i",(int)(dbk+0.5)); modified by liytao 20030310
				change_str.Format("%.0f",dbk);
				re_array[m].Number=change_str; 
            
				change_str="";
				//tran_value=((k*1.0)/num)*100; //整数除法的0 modified by liytao 20030106
				tran_value=((dbk*1.0)/dbTotal)*100; //整数除法的0
				change_str.Format("%.5f",tran_value);
				re_array[m].Percent=change_str;
				
				
				TempCum=TempCum+tran_value;
				change_str="";
				change_str.Format("%.5f",TempCum);
				
				re_array[m].CumPercent=change_str;
				m++;
			
			}			
            TempValue=array[i];
			if (array[i].iIndex>=0)
				dbk=weight[array[i].iIndex-1];
			else 
				dbk=weight[-array[i].iIndex-1];
		}
	}
	if (dbk>0)  //当循环跳出来的时候，最后一组数据还需要填写
	{
//		change_str.Format("%.5f",array[i-1].dValue);
//		re_array[m].ListValue=change_str; 
		//add by liytao 20030106
		if(array[i-1].iIndex<0)
			re_array[m].ListValue=_T("  缺失值"); 
			
		//add end
		else if(array[i-1].strVal=="" && TempValue.strVal=="")
		{
			change_str.Format("%f",array[i-1].dValue);
			re_array[m].ListValue=change_str; 
		}
		else
			re_array[m].ListValue=array[i-1].strVal; 	
		
		change_str="";
		//change_str.Format("%i",(int)(dbk+0.5)); modified by liytao 20030310
		change_str.Format("%.0f",dbk);
		re_array[m].Number=change_str; 
		
		change_str="";
		//tran_value=((k*1.0)/num)*100; //整数除法的0 modified by liytao 20030106
		tran_value=((dbk*1.0)/dbTotal)*100; //整数除法的0
		change_str.Format("%.5f",tran_value);
		re_array[m].Percent=change_str;
		
		TempCum=TempCum+tran_value;
		change_str="";
		change_str.Format("%.5f",TempCum);
		
		re_array[m].CumPercent=change_str;
		m++;
	}
	j=m+1;
	//    j++; //  返回个数  最后还有一个求和的记录
	//    TFreqArray *temp_array=new TFreqArray;
	re_array[j-1].ListValue="  总计";
	change_str="";
	//change_str.Format("%i",num); modified by liytao 20030106
	//change_str.Format("%i",(int)(dbTotal+0.5)); modified by liytao 20030310
	change_str.Format("%.0f",dbTotal);
	re_array[j-1].Number=change_str;
	
	change_str="";
	change_str.Format("%.5f",100.0);
	re_array[j-1].Percent=change_str;
	
	return j;
}
//add end

//该函数返回的x最少有5个值。
//从x[0]开始分别为： 最小值的黑线、BOX的下线，中间值、BOX的上线、最大的值的黑线
//如返回的int值大于5。则说明有extrem值。需要单独在图形中显示。
int CStatistics::f_plotval(double* &x,BOOL beCalCoor, TPlotReco &re_coor,int nType)//
{
	int i=5,k=5,temp_max=0,temp_min=num;
	//        int temp_value=0;
	double min=0,max=0,medium=0,q3=0,q1=0;
	int j=0;
	prshl(array,num);
	
    //        re_coor=new double[4];
    q1=f_percentile(0.25,nType);
	q3=f_percentile(0.75,nType);
    medium=f_percentile(0.5,nType);
	
	//  re_coor.re_max=array[num-1].dValue;
	//  re_coor.re_min=array[0].dValue;
	if(beCalCoor)
		re_coor=GetCoordinate(array,num);
	
	
	for(j=num-1;j>0&&array[j].dValue>(q3+1.5*(q3-q1));j--)
		temp_max=j;

	for(j=0;j<(num-1)&&array[j].dValue<(q1-1.5*(q3-q1));j++)
 		temp_min=j;

	if (temp_max>0)
	{
		max=array[temp_max-1].dValue;
		i+=num-temp_max;
	}
	else
		max=array[num-1].dValue;
	if (temp_min<num)
	{
		min=array[temp_min+1].dValue;
		i+=temp_min+1;
	}
	else
		min=array[0].dValue;
    
	x=new double[i];
	x[0]=min;
    x[1]=q1;
	x[2]=medium;
	x[3]=q3;
	x[4]=max;
    if ((i>5)&&(temp_max>0)) 
	{
		for(j=temp_max;j<num;j++)
		{
			x[k]=array[j].dValue;
			k++;
		}
	};
	if ((i>5)&&(temp_min<num))
	{
		for(j=0;j<temp_min+1;j++)
		{
			x[k]=array[j].dValue;
			k++;
		}
	};
//	delete []x;
//	x=NULL;
	return k;
}


TPlotReco CStatistics::GetCoordinate(TDataSet *pDataArray, int nArray)
{
	TPlotReco re_coor;
	double dFirstValue=pDataArray[0].dValue;
	if(nArray==1 || IsEqual(pDataArray,nArray))
	{
		double dValue=(dFirstValue<1)?dFirstValue:floor(dFirstValue);
		re_coor.re_min=0;
		re_coor.co_num=2;
		re_coor.re_max=2*dValue;
		re_coor.co_value=dValue;
		return re_coor;
	}
	
	prshl(pDataArray,nArray,TRUE);
	
    re_coor.co_num=0; 
	double dMinValue=0;
	double begin_coor=0;
	long temp_value=0;
	dFirstValue=pDataArray[0].dValue;	
	double querter=pDataArray[nArray-1].dValue-dFirstValue;
    if (nArray<10 || (querter/5)<1)
	{
		int nDiv=nArray>=10?5:((querter/5)<1)?2:5;
		dMinValue=floor(dFirstValue)==dFirstValue?floor(dFirstValue-querter/5):floor(dFirstValue);
		if(dMinValue<0)
			dMinValue=0;
		
		begin_coor=(int)dMinValue;
		re_coor.re_min=dMinValue; 
		do 
		{
			dMinValue+=querter/nDiv;   
			re_coor.co_num ++; 
		}while(dMinValue<=pDataArray[nArray-1].dValue);
		re_coor.re_max=re_coor.re_min+ceil((querter/nDiv)*re_coor.co_num);
		re_coor.co_value=(re_coor.re_max-re_coor.re_min)/re_coor.co_num;
	}
	else
	{
		//个数大。区间大。重点考虑。  个数>10  区间大于5
		if(floor(dFirstValue)==dFirstValue)
		{
			dMinValue=floor(dFirstValue-querter/5);
			if (dFirstValue>0)
			{
				if (dMinValue>=dFirstValue)
					dMinValue-=10;
				if(dMinValue<0)
					dMinValue=0;
			}
		}
		else
			dMinValue=floor(dFirstValue);
		
		begin_coor=(int)dMinValue;		
		if(((int)begin_coor%10)!=0)
		{
			do
			begin_coor--;
			while(((int)begin_coor%10)!=0);
		};
		
		if(begin_coor<0)
			begin_coor=0;
		re_coor.re_min=begin_coor; 
		
		do 
		{
			double dvalue=ceil(querter/5);
			begin_coor+=dvalue;   
			re_coor.co_num ++; 
		}while(begin_coor<=pDataArray[nArray-1].dValue);
		
		re_coor.re_max=re_coor.re_min+ceil(querter/5)*re_coor.co_num;
		re_coor.co_value=ceil(querter/5);
	};
	return re_coor;
	
}

double CStatistics::f_trimmean(int nType) //去除5%后的平均值
{
	if(pWeight!=NULL&&nType==1)// add by liytao 20030110
	{
		int i=0;
		double *pcc=new double[num],tc=0,t1=0;
		pcc[0]=pWeight[array[0].iIndex-1];
		for ( i=1;i<num;i++)
			pcc[i]=pcc[i-1]+pWeight[array[i].iIndex-1];
		tc=0.05*dbweight;
		int k1=-1,k2=-1;
		for (i=0;i<num-1;i++)
		{
			if( pcc[i]<tc&&pcc[i+1]>=tc)
			{
				k1=i;
				break;
			}
		}
		for (i=1;i<num;i++)
		{
			if( (dbweight-pcc[i])<tc&&(dbweight-pcc[i-1])>=tc)
			{
				k2=i;
				break;
			}
		}
		//add by liytao 20030225
		//if( k1<0||k2<0)
		if( k2<=0)
		{
			bRet=FALSE;
			return 0;
		}
		//add end
		double ret=0;
		ret=0;
		if( k2==(k1+1))
			ret=array[k2].dValue;
		else
		{
			for (i=(k1+2);i<=(k2-1);i++)
				ret=ret+pWeight[array[i].iIndex-1]*array[i].dValue;
			t1=(pcc[k1+1]-tc)*array[k1+1].dValue+(dbweight-pcc[k2-1]-tc)*array[k2].dValue;
			ret=ret+t1;
			ret=ret/(0.9*dbweight);

		}

		delete []pcc;
		//add by liytao 20030225
		bRet=TRUE;
		//add end
		return ret;
	}

	else//add by liytao 20030110
	{
		if (num>2)
		{
			double sum=0;
			int minu_num=(int)floor(0.05*num+0.5);
			if (minu_num==0)
			{
				minu_num=1;
			}
			for (int i=minu_num;i<num-minu_num; i++)
			{
				sum+=array[i].dValue;
			}
			return sum/(num-2*minu_num);
		}
		else return 0;
	}//add end
}


int CStatistics::f_StemLeaf(TStemLeaf *&re_array, double &stemwidth,int nType)
{        
	int i=0,k=0,temp_max=0,temp_min=num,count_num=0;
	int max_num=0,min_num=0;
	double dbmax_num=0,dbmin_num=0,dbi=0;//add by liytao 20030115
	double min=0,max=0,medium=0,q3=0,q1=0,fabsmax=0;
	int j=0,row_num=0;
	int flag_max=0,flag_min=0;
	int test_rank=0,rank=0;
	double step_width=0,comp_value=0,start_value=0,temp_start_value=0;
	CTString change_str,leaf_str,temp_leaf,freq_str,stem_str;
	prshl(array,num);
	
    /*q1=f_percentile(0.25);
	q3=f_percentile(0.75);
    medium=f_percentile(0.5); modified by liytao 20030116*/
	q1=f_percentile(0.25,nType);
	q3=f_percentile(0.75,nType);
    medium=f_percentile(0.5,nType);
	
	for(j=0;j<(num-1),array[j].dValue<(q1-1.5*(q3-q1));j++)
    {
		temp_min=j;
		//add by liytao 20030115
		if (pWeight!=NULL&&nType==1)
			dbmin_num=dbmin_num+pWeight[array[j].iIndex-1];
		//add end
		min_num++;
	}  //检测是否是过小过界
	if (temp_min<num)
	{
		min=array[temp_min+1].dValue;
		//add by liytao 20030115
		if( pWeight!=NULL&&nType==1)
		{
			dbi=dbi+dbmin_num;

		}
		//add end
		i+=min_num;
	}
	else
		min=array[0].dValue;
	
	for(j=num-1;j>0,array[j].dValue>(q3+1.5*(q3-q1));j--)
	{
		temp_max=j; //temp_max是存放 超才出上界限的 数值的位置
		//add by liytao 20030115
		if( pWeight!=NULL&&nType==1)
			dbmax_num=dbmax_num+pWeight[array[j].iIndex-1];
		//add end
		max_num++;  //超才出上界限的 数值的个数
	}  //检测是否有过大过界
	
	if (temp_max>0)
	{
		max=array[temp_max-1].dValue;
		//add by liytao 20030115
		if( pWeight!=NULL&&nType==1)
			dbi=dbi+dbmax_num;
		//add end
		i+=max_num;
	}
	else
		max=array[num-1].dValue;
	
    if (fabs(min)>max)
		fabsmax=fabs(min);
	else
		fabsmax=max;
	
	if ((fabsmax/10)>1)
	{
		do 
		{
			count_num++;
			fabsmax=fabsmax/10;
		}while(fabsmax/10>1);
	}
	//计算stemwidth的值
	//        stemwidth=new double[1];
	stemwidth=1;
    if (count_num!=0) 
    {
		for(j=0;j<count_num;j++)
			stemwidth=stemwidth*10;
	}
	//确定是否可以需要5分或者2分
    
	//计算初级阶数
    test_rank=(int)((max-min)/stemwidth+1);
	
	if ((stemwidth>=100)&&(test_rank==1))
	{
        do
		{
			stemwidth=stemwidth/10;
			test_rank=(int)((max-min)/stemwidth+1);
		}while((stemwidth>=100)&&(test_rank==1));
	};
	//rank:      是否细分
	rank=1;//test_rank;
    if (test_rank==1)
	{
		if(num>10)
			rank=5;
		else if(num>5)
            rank=2;         
	}
	else if(test_rank==2)
	{
		if(num/2>10)
			rank=5;
		else if(num/2>5)
            rank=2;         
	}
	else 
    {
		
		if((num/test_rank)>100)
			rank=10;
        else if((num/test_rank)>50)
			rank=5;
		else if((num/test_rank)>10)
			rank=2;
	}
	// 步进宽度
	step_width=stemwidth/rank;
    row_num=1;
	//开始计算的数值
	start_value=(floor(min/stemwidth))*stemwidth;//+step_width;
	int step=0;
    double trans_start=start_value;
	//    if (start_value<=min)  //如果出现了相等呢？
	//        {
	do
	{
        trans_start+=step_width; 
		step++;
		//        start_value=start_value+step_width;
        //        }while(start_value<min);
	}while(trans_start<min);
	//        }        
	//        start_value-=step_width;
    trans_start-=step_width;
	//比较的数值必须是开始的数值加一个步进
	//        comp_value=start_value+step_width;
	comp_value=trans_start+step_width;
	if (comp_value<=max)
	{
		do{
            row_num++;  
			comp_value+=step_width;
		}while(comp_value<=max);
	};
	//add by liytao 20030115
	if ( pWeight!=NULL&&nType==1)
	{
		if (dbmax_num>0)
			flag_max=1;
		if (dbmin_num>0)
			flag_min=1;

	}
	else
	{
	//add end
		if (max_num>0)
			flag_max=1;
		if (min_num>0)
			flag_min=1;
	}	
    row_num+=flag_max+flag_min;
	
	re_array=new TStemLeaf[row_num];
	//如果有下界越线
	//add by liytao 20030115
	if (pWeight!=NULL&&nType==1)
	{
		if (dbmin_num>0)
		{
			change_str.Format("%i",(int)(dbmin_num+0.5));
			re_array[0].freq=change_str;
			re_array[0].stem="小过界:";
			change_str.Format("%.5f",array[temp_min].dValue);
			re_array[0].leaf="(<="+change_str+")";
		};

	}
	else
	{
		//add end
		if (min_num>0)
		{
			change_str.Format("%i",min_num);
			re_array[0].freq=change_str;
			re_array[0].stem="小过界:";
			change_str.Format("%.5f",array[temp_min].dValue);
			re_array[0].leaf="(<="+change_str+")";
		};
	}
	//主体数据的存放。
	//  change_str.Format("%.5f",start_value);
	//  comp_value=start_value+step_width;
	comp_value=trans_start+step_width;
    leaf_str="";
	freq_str="";
	double dbk=0;//add by liytao 20030108
	i=0;
	k=0;
	//        temp_start_value=0;
	temp_start_value=(step-1)*step_width;
    //循环体内应该不包括过界的数值

    //add by liytao 20030115
	if (pWeight!=NULL&&nType==1)
	{
		for(j=min_num;j<num && array[j].dValue<=max;j++) 
		{
			if (array[j].dValue<comp_value)
			{
				temp_leaf="";
				CTString str="";
				
				if (stemwidth>10)
					str.Format("%f",(array[j].dValue-start_value)/stemwidth*10);
				else if(stemwidth==1) 
					str.Format("%f",(array[j].dValue-start_value)*10/stemwidth);
				else
					str.Format("%f",array[j].dValue-start_value);
				
				int kk=  atoi(str);  //取整数部分
				temp_leaf.Format("%i",kk);
				leaf_str=leaf_str+temp_leaf;
				dbk+=pWeight[array[j].iIndex-1];
				k++;
			}
			else
			{
				freq_str.Format("%i",(int)(dbk+0.5));
				stem_str.Format("%i",(int)(start_value/stemwidth));
				re_array[i+flag_min].freq=freq_str;
				re_array[i+flag_min].leaf=leaf_str;
				re_array[i+flag_min].stem=stem_str; 
				//
				temp_start_value+=step_width;
				if ((temp_start_value>stemwidth) ||(fabs(temp_start_value-stemwidth)<1e-10))
				{
					start_value+=stemwidth;
					temp_start_value=0;
				}
				comp_value+=step_width;
				leaf_str="";
				freq_str="";
				dbi=dbi+pWeight[array[j].iIndex-1];
				i++;
				dbk=0;
				k=0;
				j--;
			}
		}


	//add end
	}
	else
	{
		for(j=min_num;j<num && array[j].dValue<=max;j++) 
		{
			if (array[j].dValue<comp_value)
			{
				temp_leaf="";
				CTString str="";
				
				if (stemwidth>10)
					str.Format("%f",(array[j].dValue-start_value)/stemwidth*10);
				else if(stemwidth==1) 
					str.Format("%f",(array[j].dValue-start_value)*10/stemwidth);
				else
					str.Format("%f",array[j].dValue-start_value);
				
				int kk=  atoi(str);  //取整数部分
				temp_leaf.Format("%i",kk);
				leaf_str=leaf_str+temp_leaf;
				k++;
			}
			else
			{
				freq_str.Format("%i",k);
				stem_str.Format("%i",(int)(start_value/stemwidth));
				re_array[i+flag_min].freq=freq_str;
				re_array[i+flag_min].leaf=leaf_str;
				re_array[i+flag_min].stem=stem_str; 
				//
				temp_start_value+=step_width;
				if ((temp_start_value>stemwidth) ||(fabs(temp_start_value-stemwidth)<1e-10))
				{
					start_value+=stemwidth;
					temp_start_value=0;
				}
				comp_value+=step_width;
				leaf_str="";
				freq_str="";
				i++;
				dbk=0;
				k=0;
				j--;
			}
		}
	}
	//循环最后一次的结果不能进入到写值，所以单独判断并处理
	
	// if (k!=0) modified by liytao 20030115
	if (pWeight!=NULL&&nType==1)
	{
		if (fabs(dbk)>0.000001)
		{
			freq_str.Format("%i",int (dbk+0.5));
			stem_str.Format("%i",(int)(start_value/stemwidth));
			re_array[i+flag_min].freq=freq_str;
			re_array[i+flag_min].leaf=leaf_str;
			re_array[i+flag_min].stem=stem_str; 
		}
		//如果有下界越线
		if (dbmax_num>0)
		{
			change_str.Format("%i",int(dbmax_num+0.5));
			re_array[row_num-1].freq=change_str;
			re_array[row_num-1].stem="大过界:";
			change_str.Format("%.5f",array[temp_max].dValue);
			re_array[row_num-1].leaf="(>="+change_str+")";
		};
	}
	else
	{
		if(k!=0)
		{
			freq_str.Format("%i",k);
			stem_str.Format("%i",(int)(start_value/stemwidth));
			re_array[i+flag_min].freq=freq_str;
			re_array[i+flag_min].leaf=leaf_str;
			re_array[i+flag_min].stem=stem_str; 
		}
		//如果有下界越线
		if (max_num>0)
		{
			change_str.Format("%i",max_num);
			re_array[row_num-1].freq=change_str;
			re_array[row_num-1].stem="大过界:";
			change_str.Format("%.5f",array[temp_max].dValue);
			re_array[row_num-1].leaf="(>="+change_str+")";
		};
	}
    return row_num;
}

//add by liytao 20030108
//pColArray原始列数据，nArray列数据中的数据个数，pGroupS返回的数据，并返回其个数
int   CStatistics::GetGroupingData(TDataSet *pColArray,int nArray,TGroupInfor* &pGroupS,BOOL bCountMissing)
{
	CStatistics stat;
//	double comp_value=0;
	int re_value=1;
	stat.prshl(TRUE,pColArray,nArray,TRUE); 
	
	TDataSet tBgn=pColArray[0];
//	comp_value=pColArray[0].dValue; 	
    // 先求出有多少个不同的值  组数
    int i;
    for(i=1;i<nArray;i++)
	{
		if (tBgn!=pColArray[i])
		{
			if( bCountMissing||tBgn.iIndex>0)
				re_value++; 
			tBgn=pColArray[i];
		}
	}
	
	//每个不同的值中有分别有多少个不同的值
    int *temp_index;
    temp_index=new int[re_value];
    int k=1;
	int temp_num=0;
	tBgn=pColArray[0];
//  comp_value=pColArray[0].dValue; 	
    for(i=1;i<nArray;i++)
	{
		if (tBgn==pColArray[i])
		{
			if(bCountMissing||tBgn.iIndex>0)
				k++;
		}
		else
        { 
			if(bCountMissing||pColArray[i-1].iIndex>0)
			{
				temp_index[temp_num++]=k;
			}
				k=1;
			tBgn=pColArray[i];
		}
	}
	if(bCountMissing||pColArray[i-1].iIndex>0)
	{
		temp_index[temp_num]=k;
	}
	
	pGroupS=new TGroupInfor[re_value];
	int ttindex=0,n=0;
	for(int ii=0;ii<re_value;ii++)
	{
		ttindex=0;
		int count=temp_index[ii],
			*pValue=new int[count];
		for(int kk=n;kk<n+count;kk++)
		{
			pValue[ttindex]=pColArray[kk].iIndex;
			ttindex++;
		}
		pGroupS[ii]=pColArray[n];
		pGroupS[ii].pIndex=TIntStruct(count,pValue);
		n+=count;
	}
	
	if(temp_index)
	{
		delete [] temp_index;
		temp_index=NULL;
	}
	
	return re_value;

}

//pColArray原始列数据，nArray列数据中的数据个数，pGroupS返回的数据，并返回其个数
int   CStatistics::GetGroupingData(TDataSet *pColArray,int nArray,TGroupInfor* &pGroupS)
{
	CStatistics stat;
//	double comp_value=0;
	int re_value=1;
	stat.prshl(pColArray,nArray,TRUE); 
	
	TDataSet tBgn=pColArray[0];
//	comp_value=pColArray[0].dValue; 	
    // 先求出有多少个不同的值  组数
    int i;
    for(i=1;i<nArray;i++)
	{
		if (tBgn!=pColArray[i])
		{
			re_value++;
			tBgn=pColArray[i];
		}
	}
	
	//每个不同的值中有分别有多少个不同的值
    int *temp_index;
    temp_index=new int[re_value];
    int k=1;
	int temp_num=0;
	tBgn=pColArray[0];
//  comp_value=pColArray[0].dValue; 	
    for(i=1;i<nArray;i++)
	{
		if (tBgn==pColArray[i])
		{
				k++;
		}
		else
        { 
			{
				temp_index[temp_num++]=k;
				k=1;
			}
			tBgn=pColArray[i];
		}
	}
	{
		temp_index[temp_num]=k;
	}
	
	pGroupS = new TGroupInfor[re_value];
	int ttindex=0,n=0;
	for(int ii=0;ii<re_value;ii++)
	{
		ttindex=0;
		int count=temp_index[ii],
			*pValue=new int[count];
		for(int kk=n;kk<n+count;kk++)
		{
			pValue[ttindex]=pColArray[kk].iIndex;
			ttindex++;
		}
		pGroupS[ii]=pColArray[n];
		pGroupS[ii].pIndex=TIntStruct(count,pValue);
		n+=count;
	}
	
	if(temp_index)
	{
		delete [] temp_index;
		temp_index=NULL;
	}
	
	return re_value;
}

/*
void CStatistics::f_ConverFreq(CPtrArray *pFreqConfs,CTString strName,TFreqArrayS* pFreqArrayS)
{
	if(pFreqConfs==NULL)
		return;
	int nSize=pFreqConfs->GetSize();
	if(nSize<=0)
		return;
	CFreqConf* pConf=NULL;	
	for (int i=0;i<nSize;i++)
	{
		pConf=(CFreqConf*)pFreqConfs->GetAt(i);
		if( pConf->m_strName==strName)
			break;
	}
	if(pConf==NULL)
		return;
	TFreqArray* pFA=pFreqArrayS->pFreqArray;
	int begin=0,end =pFreqArrayS->iCount-2;
	int last=end;
	double dbRangeNum=pConf->m_RangeNum; //get value

	double tVal;
	double dbCumPercent;
	double dbNumber;
	double dbPercent;

	//add by liytao 20030110
	double dbRangeFrom,dbRangeTo,dbtemp;
	dbRangeFrom=pConf->m_dbRangeFrom;
	dbRangeTo=pConf->m_dbRangeTo;
	if (dbRangeFrom>dbRangeTo)
	{
		dbtemp=dbRangeTo;
		dbRangeTo=dbRangeFrom;
		dbRangeFrom=dbtemp;
	
	}
	//add end
	BOOL bBreak=FALSE;
	if (pFA[begin].ListValue!=_T("  缺失值"))
	{
		tVal=atof(pFA[begin].ListValue);
		if(tVal>=dbRangeFrom)
			bBreak=TRUE;
		else 
			pFA[begin].ListValue.Format(" %.5f",tVal);
	}
	else begin++;
	while ((!bBreak)&&(begin<=end))
	{

		if (pFA[begin].ListValue!=_T("  缺失值"))
		{
			tVal=atof(pFA[begin].ListValue);
			if(tVal>=dbRangeFrom)
				bBreak=TRUE;
			else
			{
				pFA[begin].ListValue.Format(" %.5f",tVal);
				begin++;
			}
		}
		else begin++;
	}
	bBreak=FALSE;
	if (pFA[end].ListValue!=_T("  缺失值"))
	{
		tVal=atof(pFA[end].ListValue);
		if(tVal<=dbRangeTo)
			bBreak=TRUE;
		else
			pFA[end].ListValue.Format(" %.5f",tVal);
	}
	else end--;
	while ((!bBreak)&&(begin<end))
	{
		if (pFA[end].ListValue!=_T("  缺失值"))
		{
			tVal=atof(pFA[end].ListValue);
			if(tVal<=dbRangeTo)
				bBreak=TRUE;
			else 
			{
				pFA[end].ListValue.Format(" %.5f",tVal);
				end--;
			}
		}
		else end--;
	}
	TFreqArray tempFreq;

	int nRange;
	if (pConf->m_RangeOpt==RANGE_NUM) // 按区间个数
	{
		nRange=(int)dbRangeNum;
	}
	if (pConf->m_RangeOpt==RANGE_LENGTH) // 按区间长度
	{
		int nTemp=(int) ((dbRangeTo-dbRangeFrom)/dbRangeNum);
		if (((dbRangeTo-dbRangeFrom)/dbRangeNum)>nTemp)
			nRange=nTemp+1;
		else nRange=nTemp;
	}
	if( nRange>=1)
	{
		if(begin<=end)
		{
			double dbStart,dbStop=dbRangeFrom;
			double dbCumPercent; 
			int nRangeIndex=1; 		
			int tempbegin=begin,initbegin=begin;//add by liytao 20030110
			bBreak=FALSE;
			while (nRangeIndex<=nRange&&begin<=end&&!bBreak)
			{
				//merge items
				int j=begin;
				if(j==0)
					dbCumPercent=0;
				else
					dbCumPercent=atof(pFA[j-1].CumPercent);
				dbNumber=0;
				dbPercent=0;
				tVal=atof(pFA[j].ListValue);
				dbStart=dbStop;
				if(nRangeIndex==nRange)
					dbStop=dbRangeTo;
				else
				{
					if (pConf->m_RangeOpt==RANGE_NUM)
						dbStop=dbRangeFrom+(dbRangeTo-dbRangeFrom)*nRangeIndex/nRange;
					if (pConf->m_RangeOpt==RANGE_LENGTH)
						dbStop=dbRangeFrom+dbRangeNum*nRangeIndex;
				}
				
				BOOL bStop=FALSE;
				while (!bStop&&tVal<=dbRangeTo&&j<=end)
				{
					if( nRangeIndex==nRange)
					{
						if(tVal>dbStop)
							bStop=TRUE;
						else
						{
							dbNumber+=atof(pFA[j].Number);
							pFA[j].Number=_T("-1");
							dbPercent+=atof(pFA[j].Percent);
							j++;
							tVal=atof(pFA[j].ListValue);
						}
					}
					else
					{
						if(tVal>=dbStop)
							bStop=TRUE;
						else
						{
							dbNumber+=atof(pFA[j].Number);
							pFA[j].Number=_T("-1");
							dbPercent+=atof(pFA[j].Percent);
							j++;
							tVal=atof(pFA[j].ListValue);
						}
					}
				}

				if( j==end)
				{
					if( nRangeIndex==nRange)
					{
						if(tVal<=dbStop)
						{

							dbNumber+=atof(pFA[j].Number);
							pFA[j].Number=_T("-1");
							dbPercent+=atof(pFA[j].Percent);
							bBreak=TRUE;
						}
					}
					else
					{
						if(tVal<dbStop)
						{
							dbNumber+=atof(pFA[j].Number);
							pFA[j].Number=_T("-1");
							dbPercent+=atof(pFA[j].Percent);
							bBreak=TRUE;
						}
					}

				}
				dbCumPercent+=dbPercent;
				//change ListValue
				if(j!=begin)
				{
					if(nRangeIndex<nRange)
						pFA[begin].ListValue.Format("［%.5f -- %.5f）",dbStart,dbStop);
					else
						pFA[begin].ListValue.Format("［%.5f -- %.5f］",dbStart,dbStop);
					pFA[begin].Number.Format("%d",int(dbNumber));
					pFA[begin].Percent.Format("%.5f",dbPercent);
					pFA[begin].CumPercent.Format("%.5f",dbCumPercent);
				}
				tempbegin=begin;//add by liytao 20030110
				begin=j;
				nRangeIndex++;
			}
			//add by liytao 20030110
			if( end==last)
			{
				if (begin>=end)
				{
					if(fabs(atof(pFA[tempbegin].Percent)-100)<0.1||atof(pFA[tempbegin].Percent)>100) 
						dbPercent=100;
					if(fabs(atof(pFA[tempbegin].CumPercent)-100)<0.1||atof(pFA[tempbegin].CumPercent)>100) 
						dbCumPercent=100;
					pFA[tempbegin].Percent.Format("%.5f",dbPercent);
					pFA[tempbegin].CumPercent.Format("%.5f",dbCumPercent);
				}
			}
			//add end
		}
	}
}
//add end
*/
//add by liytao 20030108
double CStatistics::GetWeighted(TDataSet *pArray, int nDataNum,double weight[])
{
	double dbW=0;
	if (weight==NULL)
		dbW=nDataNum;
	else
	{
		for (int i=0;i<nDataNum;i++)
		{
			if (pArray[i].iIndex>=0)
			{
				//pArray[i].dValue=pArray[i].dValue*weight[pArray[i].iIndex];
				dbW+=weight[pArray[i].iIndex-1];
			}
		}
	}
	return dbW;

}
//add end