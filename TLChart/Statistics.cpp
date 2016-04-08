// Statistics.cpp: implementation of the CStatistics class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Statistics.h"

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


CStatistics::~CStatistics()
{
}

//////////////////////////////////////////
/// 排序
void CStatistics::prshl(TDataSet *p, int n,bool reSort)   //实数的shell（希尔）排序
{
    int k,j,i;
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


double CStatistics::f_variance()
{
	if (!flag_vari)
	{
		int i;
		double v_mean,re_value=0;
		if(num==1)
		  return 0;
		v_mean=f_mean();
		for (i=0;i<num;i++)
		{
			re_value+=(array[i].dValue-v_mean)*(array[i].dValue-v_mean);
		}
		flag_vari=true;
		v_vari=re_value/(num-1);		 
	}
	return v_vari;
} 

double CStatistics::f_stadeviation()
{
	if (!flag_stdv)
	{
		if (flag_vari==0) 
			v_stdv=sqrt(f_variance());
		else
			v_stdv=sqrt(v_vari);
		flag_stdv=true;
	}
    return v_stdv; 
}

double CStatistics::f_mode()
{
	double ret_value,dCurValue;
	int max_num=1,i,j;  //开始有一个
    prshl(array,num);
	ret_value=array[0].dValue;
	dCurValue=array[0].dValue;
	for (i=1;i<num;i++)
	{
        if(dCurValue==array[i].dValue)  
			j++;
		else
		{
			if(j> max_num)
			{  
				max_num=j;
				ret_value=dCurValue;
			}
			j=1;
			dCurValue=array[i].dValue;
		}
	} 
	
	return ret_value;
}

double CStatistics::f_skewness()
{
	double mean,stdeviation,M=0;
	int i;
	
	mean=f_mean();
	stdeviation=f_stadeviation();
	for(i=0;i<num;i++)
		M+=pow((array[i].dValue-mean),3);//*(array[i].dValue-mean)*(array[i].dValue-mean);
	
	return (num*M)/((num-1.0)*(num-2.0)*stdeviation*stdeviation*stdeviation);
	//	}
}

double CStatistics::f_kurtosis(double &dSeg1,double &dSeg2)
{
	double intv1,intv2;
	
	double M4=0,M3=0,M2=0,std,mean,mid_mul; 
    int i;
	mean=f_mean();
    std=f_stadeviation();       	
	for(i=0;i<num;i++)
	{
		double value=array[i].dValue-mean;
        mid_mul=pow(value,2);//*(array[i]-mean);
        M2+=mid_mul;
		mid_mul *= value;
		M3+=mid_mul;
		mid_mul *=value;
		M4+=mid_mul;
	}
	
    dSeg1=sqrt((num*(num-1.0))/((num-2.0)*(num+1.0)*(num+3.0)/6.0));
    
    intv1=4.0*(num*num-1)*dSeg1*dSeg1;
    intv2=(num-3)*(num+5);
	
	dSeg2=sqrt((4.0*(num*num-1.0)*dSeg1*dSeg1)/((num-3.0)*(num+5.0)));
	
    return (num*(num+1.0)*M4-3*M2*M2*(num-1.0))/((num-1.0)*(num-2.0)*(num-3.0)*std*std*std*std);
}

double CStatistics::f_mean()
{
	if (flag_sum==0) 
		v_mean=f_sum()/num;
	else
		v_mean=v_sum/num;
	flag_mean=1;
	return  v_mean;     
}

double CStatistics::f_medium()
//注意如果个数有7个，那么应该是是取第四个值。但此时的i应该是3。
{
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

double CStatistics::f_sum()
{
    if (flag_sum==0)
	{
		double Sum = 0.0;
		for(long i=0; i<num; i++)
		{
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

double CStatistics::f_sem()
{
	double v_sem;
	if (flag_stdv==0)
		v_sem=f_stadeviation()/(sqrt(num));
    else
		v_sem=v_stdv/(sqrt(num)); 
	return v_sem;
}

double*  CStatistics::Describle(TDataSet *pArray,int nDataCount,int *pMethodS,int nMCount)//CDataInterface *pDataInterface)
{
	num=nDataCount;
	array=pArray;
	flag_prs=flag_sum=flag_mean=flag_vari=flag_stdv=0;	
	double dEskewnesserror,dEkurtosisError,*rtValue=new double[nMCount],
		dEkurtosis=f_kurtosis(dEskewnesserror,dEkurtosisError);
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
			result=f_medium();
			break;
		case EXPLORE_EMAXIMUM:      //描述分析-最大值
			result=f_max();
			break;
		case EXPLORE_ESUM:          //描述分析-总和
			result=f_sum();
			break; 
		case EXPLORE_EMEAN:         //描述分析-平均值
			result=f_mean();
			break;
		case EXPLORE_EMEANERROR:    //描述分析-平均误差
			result=f_sem();
			break;
		case EXPLORE_EMODE:         //描述分析-众数
			result=f_mode();
			break;
		case EXPLORE_ETRIMMED:      //描述分析-5%截尾均值
			result=f_trimmean();
			break;
		case EXPLORE_ESTD:          //描述分析-标准离差
			result=f_stadeviation();
			break;
		case EXPLORE_EVARIANCE:     //描述分析-方差
			result=f_variance();
			break;
		case EXPLORE_ESKEWNESS:     //描述分析-偏态系数
			result=f_skewness();
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
			result=f_intqr();
			break;
			
		default:
			break;						
		}
		rtValue[kk]=result;
	}
	
	return rtValue;
}

int CStatistics::achol(double a[], int n, int m, double d[])
{
	int i,j,k,u,v;
    if ((a[0]+1.0==1.0)||(a[0]<0.0))
	{ printf("fail\n"); return(-2);}
    a[0]=sqrt(a[0]);
    for (j=1; j<=n-1; j++) a[j]=a[j]/a[0];
    for (i=1; i<=n-1; i++)
	{ u=i*n+i;
	for (j=1; j<=i; j++)
	{ v=(j-1)*n+i;
	a[u]=a[u]-a[v]*a[v];
	}
	if ((a[u]+1.0==1.0)||(a[u]<0.0))
	{ printf("fail\n"); return(-2);}
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


double CStatistics::f_intqr()  //区间差
{
	return (f_percentile(0.75)-f_percentile(0.25));
}

double CStatistics::f_percentile(double p) 
// 输入0.0<=p<=1 如 0.5为medium;0.75为Q3;0.25为Q1
{
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

int CStatistics::f_percentile(double* &rtValue)
{
	double percent[7]={0.05,0.1,0.25,0.50,0.75,0.90,0.95};
	rtValue=new double[7];
	for(int ii=0;ii<7;ii++)
		rtValue[ii]=f_percentile(percent[ii]);
	
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
void CStatistics::f_conf_mean(double p, double &x, double &y)
{
	double q=((double)(1-p/100.0))/2.0;
	//如果用户输入的为95，则调用函数的参数应为 2.5%
	double re_value=ptdist(num-1, q);
	re_value=re_value*f_sem();
	if	(flag_mean==1)
	{
		x=v_mean+re_value;
		y=v_mean-re_value;
	}
	else
	{
		x=f_mean()+re_value;
		y=f_mean()-re_value;
	}
}

//该函数返回的x最少有5个值。
//从x[0]开始分别为： 最小值的黑线、BOX的下线，中间值、BOX的上线、最大的值的黑线
//如返回的int值大于5。则说明有extrem值。需要单独在图形中显示。
int CStatistics::f_plotval(double* &x,bool beCalCoor,TPlotReco &re_coor)//
{
    int i=5,k=5,temp_max=0,temp_min=num;
	//        int temp_value=0;
	double min,max,medium,q3,q1;
	int j;
	prshl(array,num);
	
    //        re_coor=new double[4];
    q1=f_percentile(0.25);
	q3=f_percentile(0.75);
    medium=f_percentile(0.5);
	
	//  re_coor.re_max=array[num-1].dValue;
	//  re_coor.re_min=array[0].dValue;
	if(beCalCoor)
		re_coor=GetCoordinate(array,num);
	
	
	for(j=num-1;j>0,array[j].dValue>(q3+1.5*(q3-q1));j--)
		temp_max=j;

	for(j=0;j<(num-1),array[j].dValue<(q1-1.5*(q3-q1));j++)
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
	
	prshl(pDataArray,nArray,true);
	
    re_coor.co_num=0; 
	double dMinValue;
	int begin_coor;
	int temp_value=0;
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
		if((begin_coor%10)!=0)
		{
			do
			begin_coor--;
			while((begin_coor%10)!=0);
		};
		
		if(begin_coor<0)
			begin_coor=0;
		re_coor.re_min=begin_coor; 
		
		do 
		{
			begin_coor+=(long)ceil(querter/5);   
			re_coor.co_num ++; 
		}while(begin_coor<=pDataArray[nArray-1].dValue);
		
		re_coor.re_max=re_coor.re_min+ceil(querter/5)*re_coor.co_num;
		re_coor.co_value=ceil(querter/5);
	};
	return re_coor;
	
}

double CStatistics::f_trimmean() //去除5%后的平均值
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
}

//pColArray原始列数据，nArray列数据中的数据个数，pGroupS返回的数据，并返回其个数
int   CStatistics::GetGroupingData(TDataSet *pColArray,int nArray,TGroupInfor* &pGroupS)
{
	CStatistics stat;
	int re_value=1;
	
	if( nArray == 1)//只有一组数据，不需要排序直接返回(极限情况)
	{
		pGroupS=new TGroupInfor[1];
		int *pValue=new int;
		pValue[0]=pColArray[0].iIndex;
		pGroupS[0]=pColArray[0];
		pGroupS[0].pIndex=TIntStruct(nArray,pValue);

		return 1;

	}
	//determine which index is the first negative index value
	int iIndex = nArray - 1;
	int i;
	for( i = nArray - 1; i >= 0; i --)
	{
		if( pColArray[i].iIndex < 0 )
			iIndex --;
		else
			break;
	}
	//put all negative data to the last of dataset array	
	TDataSet tmpData;
	for( i = 0; i <= iIndex; i ++)
	{
		if( pColArray[i].iIndex < 0 )
		{
			tmpData = pColArray[i];
			pColArray[i] = pColArray[iIndex];
			pColArray[iIndex] = tmpData;
			iIndex --;
			while( iIndex > i)
			{
				if( pColArray[iIndex].iIndex < 0)
					iIndex --;
				else break;
			}
		}
	}
	if( iIndex <= 0 ) //全部为缺失数据或者只有一个非缺失数据
	{
		int nGroupNum;
		if( pColArray[0].iIndex < 0)
			nGroupNum = 1;
		else
			nGroupNum = 2;

		pGroupS=new TGroupInfor[nGroupNum];
		int ttindex=0;
		if( nGroupNum == 1)
		{
			int *pValue=new int[nArray];
			for(int kk=0;kk<nArray;kk++)
			{
				pValue[ttindex]=pColArray[kk].iIndex;
				ttindex++;
			}
			pGroupS[0]=pColArray[0];
			pGroupS[0].pIndex=TIntStruct(nArray,pValue);
		}
		else
		{
			//"缺失"作为pGroupS的第一个元素
			int *pValue = new int[nArray - 1];
			for( int kk = 1;kk < nArray; kk ++)
			{
				pValue[ttindex]=pColArray[kk].iIndex;
				ttindex++;
			}
			pGroupS[0]=pColArray[1];
			pGroupS[0].pIndex=TIntStruct(nArray - 1 ,pValue);
			pValue=new int;
			pValue[0]=pColArray[0].iIndex;
			pGroupS[1]=pColArray[0];
			pGroupS[1].pIndex=TIntStruct(1,pValue);
		}
		return nGroupNum;
	}

	stat.prshl(pColArray,iIndex + 1,true);  	
    // 先求出有多少个不同的值  组数
	//只有一个分组，全部不是缺失值

	TDataSet tBgn=pColArray[0];
	for(i=1;i<=iIndex;i++)
	{
		if (tBgn!=pColArray[i])
		{
			tBgn=pColArray[i];
			re_value++;
		}
	}
	//如果iIndex != nArray -1，说明后面有index为负的值，即有缺失值，分组数加1
	if( iIndex != nArray - 1)
		re_value ++;

	//每个不同的值中有分别有多少个不同的值
    int *temp_index;
    temp_index=new int[re_value];
    int k=1;
	int temp_num=0;
	
	tBgn=pColArray[0];
	for(i=1;i<=iIndex;i++)
	{
		if (tBgn==pColArray[i])
			k++;
		else
		{ 
			temp_index[temp_num++]=k;
			k=1;
			tBgn=pColArray[i];
		}
	}
	bool bHasMiss = false;
	temp_index[temp_num]=k;
	if( iIndex != nArray - 1 )
	{
		temp_num ++;
		temp_index[temp_num]=nArray - iIndex - 1;
		bHasMiss = true;
	}
	// zxg ## 2003-2-27   <----
	pGroupS=new TGroupInfor[re_value];
	int ttindex=0,n=0;
	for(int ii=0;ii<re_value-1;ii++)
	{
		ttindex=0;
		int count=temp_index[ii],
			*pValue=new int[count];
		for(int kk=n;kk<n+count;kk++)
		{
			pValue[ttindex]=pColArray[kk].iIndex;
			ttindex++;
		}
		//zxg ## 2003-3-4  当没有缺失值的时候，错误地把排序打乱了
		pGroupS[bHasMiss?ii+1:ii]=pColArray[n];
		pGroupS[bHasMiss?ii+1:ii].pIndex=TIntStruct(count,pValue);
		n+=count;
	}
	int count=temp_index[re_value-1],
		*pValue=new int[count];
	ttindex = 0;
	for(int kk=n;kk<n+count;kk++)
	{
		pValue[ttindex]=pColArray[kk].iIndex;
		ttindex++;
	}
	//zxg ## 2003-3-4  当没有缺失值的时候，错误地把排序打乱了
	pGroupS[bHasMiss?0:re_value - 1]=pColArray[n];
	pGroupS[bHasMiss?0:re_value - 1].pIndex=TIntStruct(count,pValue);

	if(temp_index)
	{
		delete [] temp_index;
		temp_index=NULL;
	}
	
	return re_value;
}

int CStatistics::GetGroupData(TDataSet* pArray,int nCount,TIntStruct tInt, TDataSet* &pGroupArray,bool flag)
{
	if(tInt.iCount<=0 || nCount<=0)
		return 0;

	int nGrpCount=0;
	pGroupArray=new TDataSet[tInt.iCount];
	for(int ii=0;ii<tInt.iCount;ii++)
	{	
		int index=abs(tInt.pValue[ii]);
		for(int kk=0;kk<nCount;kk++)
		{
			if(abs(pArray[kk].iIndex)==index)
			{
				pGroupArray[nGrpCount].iIndex=(tInt.pValue[ii] > 0) ? index : -index;
				pGroupArray[nGrpCount].dValue=pArray[kk].dValue;
				pGroupArray[nGrpCount++].strVal =pArray[kk].strVal;
				break;
			}
		}
	}

	return nGrpCount;
}

void CStatistics::LinearInsert(TDataSet *&pDataSet, int nRowCount)
{
	double dPrev, dPost, dMissingValue;
	UINT index1, index2, index3;
	
	// 对源字段顺序读取
	for( index1=0; index1<nRowCount; index1++ )
	{
		if( pDataSet[index1].iIndex > 0)  // 记录非空，直接写入目的字段
			dPrev = pDataSet[index1].dValue;     // 保存最后一个非空数值作为前点数据
		else if( index1 == 0 ) 			  // 首记录为空，不做处理
			continue;
		else if( index1 == nRowCount - 1 )    // 最后一条记录为空，不做处理
			break;
		else 							  // 记录为空，进行插值处理
		{
			// 查找下一个非空数据
			index2 = index1;
			index2 ++ ;
			while( index2 < nRowCount && pDataSet[index2].iIndex < 0 )
				index2 ++;
	
			if( index2 == nRowCount )      // 后面没有非空记录，退出处理过程
				break;
			else 
			{
				dPost = pDataSet[index2].dValue; // 得到后点数据
	
				// 对中间的所有空值做线性插值处理
				for( index3 = index1; index3 < index2; index3 ++ )
				{
					double x, y, z;
					x = index1;
					y = index2;
					z = index3;
					dMissingValue = (z-y)/(x-1-y)*dPrev + (z-x+1)/(y-x+1)*dPost;
	
					// 将得到的插值写入目的字段
					pDataSet[index3].dValue = dMissingValue;
				}	
				index1 = index2 - 1;
			}
		}
	}
}
