// Statistics.h: interface for the CStatistics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATISTICS_H__FEB42AD4_2AC6_4228_A368_FF82B2D0B267__INCLUDED_)
#define AFX_STATISTICS_H__FEB42AD4_2AC6_4228_A368_FF82B2D0B267__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct TPlotReco
{
	TPlotReco()
	{
		co_num=0;
		re_max=re_min=co_value=0;
	}    
    double re_max; 
	double re_min;
	int    co_num;
	double co_value;
}TPlotReco;


typedef struct TStatBoxPlot
{
    TStatBoxPlot()
	{
		nCount=0;
		strName="";
	}

	void  DeleteObject()
	{
		nCount=0;
		strName="";
		pPlotVal.DeleteObject();
	}
	int  nCount;//存放着当前图形中每个变量中的有效数据个数
	CTString strName;
	TDoubleStruct pPlotVal;////存放每个子盒状图的实际图形数据
}TStatBoxPlot;


typedef struct TStatBoxPlotS
{
	TStatBoxPlotS()
	{
		nStatPlot=0;
		strName="";
		pStatPlot=NULL;
	}

	TStatBoxPlotS(int nStat)
	{
		nStatPlot=nStat;
		strName="";
		pStatPlot=new TStatBoxPlot[nStat];
	}

	void DeleteObject()
	{
		nStatPlot=0;
		strName="";
		if(pStatPlot)
		{
			for(int ii=0;ii<nStatPlot;ii++)
				pStatPlot[ii].DeleteObject();
		    delete [] pStatPlot;
			pStatPlot=NULL;
		}
	}
	int nStatPlot;          //存放着当前图形的子盒状图个数
	CTString strName;
	TPlotReco  tPlotReco;   //存放盒状图的最小值，最大值，区间个数及区间跨度
	TStatBoxPlot *pStatPlot;//存放每个子盒状图的实际图形数据

}TStatBoxPlotS;


class CStatistics  
{
public:
	CStatistics();
    CStatistics(TDataSet *, long);     //构造函数
	virtual ~CStatistics();
	void     SetData(TDataSet *pArray,int nDataCount);
 	double*   Describle(TDataSet *pArray,int nDataCount,int *pMethodS,int nMCount);//CDataInterface *pDataInterface);
	double f_mean();            //求平均数
	static int GetGroupData(TDataSet* pArray,int nCount,TIntStruct tInt, TDataSet* &pGroupArray,bool flag = true);

public:
	double f_sem();             
	double f_min();
	double f_max();
	double f_sum();             //求和  
	double f_medium();          //求中位数
	double f_skewness();        //求skewness(g1)
	double f_mode();            //most frequent occurrance 
	double f_stadeviation();    //标准离差
	double f_variance();        //方差   
	double f_kurtosis(double &dSeg1,double &dSeg2);//求kurtosis(g2)

public:
	static void LinearInsert(TDataSet * &pDataSet, int nRowCount);
    TPlotReco GetCoordinate(TDataSet *pDataArray, int nArray);
	void prshl(TDataSet*p,int n,bool reSort=false);  //实数的希尔排序,reSort表示是否重排
	static int   achol(double a[],int n,int m,double d[]);
	static int   GetGroupingData(TDataSet *pColArray,int nArray,TGroupInfor* &pGroupS);//pColArray原始列数据，nArray列数据中的数据个数，pExplore返回的数据，并返回其个数

	int f_plotval(double* &x,bool beCalCoor,TPlotReco &re_coor);
//	int f_plotval(double* &x,bool beCalCoor=true,TPlotReco &re_coor=TPlotReco());
	void f_conf_mean(double p,double &x, double &y);
    int f_extrmin(TDataSet* &re_value); //返回最小的结构体
	int f_extrmax(TDataSet* &re_value);
	double f_trimmean();
	double f_percentile(double x);
	int f_percentile(double* &rtValue);
	double f_intqr();
	double f_range();
	bool flag_prs;

private:
	TDataSet *array; //传入的数据指针
	long num;        //向量个数
	bool flag_vari;
	bool flag_stdv;
	bool flag_mean;
	bool flag_sum;
    double v_mean;   //平均数 
	double v_sum;    //和
    double v_stdv;   //标准差
    double v_vari;   //方差

};


#endif // !defined(AFX_STATISTICS_H__FEB42AD4_2AC6_4228_A368_FF82B2D0B267__INCLUDED_)
