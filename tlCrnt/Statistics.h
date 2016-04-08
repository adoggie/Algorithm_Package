// Statistics.h: interface for the CStatistics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATISTICS_H__FEB42AD4_2AC6_4228_A368_FF82B2D0B267__INCLUDED_)
#define AFX_STATISTICS_H__FEB42AD4_2AC6_4228_A368_FF82B2D0B267__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "StatiMethod.h"
#include "Define.h"


class AFX_EXT_CLASS CStatistics  
{
public:
	CStatistics();
    CStatistics(TDataSet *, long);     //构造函数
	virtual ~CStatistics();
	void     SetData(TDataSet *pArray,int nDataCount);
	void     SetData(TDataSet *pArray,int nDataCount,double *pW);

	void	 SetData(TDataSet *pArray,int nDataCount,double dbW,double *pW);
	double*  Describle(TDataSet *pArray,int nDataCount,int *pMethodS,int nMCount,int nType=0);//CDataInterface *pDataInterface);
	//double*  Describle(TDataSet *pArray,int nDataCount,int *pMethodS,int nMCount,double weight[],int nType=0);
	//double f_mean();            //求平均数
	double f_mean(int nType=0);            //求平均数

public:
	//double f_sem();             
	double f_sem(int nType=0);             
	double f_min();
	double f_max();
	//double f_sum();             //求和  
	double f_sum(int nType=0);             //求和  
	double f_medium(int nType=0);          //求中位数
	//double f_skewness();        //求skewness(g1)
	double f_skewness(int nType=0);        //求skewness(g1)modified by liytao 20030111
	//double f_mode();            //most frequent occurrance  modified by liytao 20030111
	double f_mode(int nType=0);            //most frequent occurrance 
	double f_mode(int& MaxCount ,int nType);
	double f_stadeviation(int nType=0);    //标准离差
	//double f_variance();        //方差  modified by liytao 20030111  
	double f_variance(int nType=0);        //方差   
	//double f_kurtosis(double &dSeg1,double &dSeg2);//求kurtosis(g2)
	double f_kurtosis(double &dSeg1,double &dSeg2,int nType=0);//求kurtosis(g2)

public:
	double GetWeighted(TDataSet *pArray,int nDataNum,double weight[]);
	//void f_ConverFreq(CPtrArray *pFreqConfs,CTString strName,TFreqArrayS* pFreqArrayS);
    TPlotReco GetCoordinate(TDataSet *pDataArray, int nArray);
	void prshl(TDataSet*p,int n,BOOL reSort=FALSE);  //实数的希尔排序,reSort表示是否重排
	void prshl(BOOL bWithMissing,TDataSet *p, int n,BOOL reSort=FALSE);   //实数的shell（希尔）排序,含权重
	inline void Swap(TDataSet &t1, TDataSet &t2);
	void QSortValue(TDataSet *pData, int nLow, int nHigh);
	void QSortIndex(TDataSet *pData, int nLow, int nHigh);
//	static void  isqt3(int n,int k,double x[],double f1,double f2,double eps,double xx[],double b[],double v[],double s[],double dt[],double ye[],double yr[],double r[]);
	static int   achol(double a[],int n,int m,double d[]);
//	static void  isqt2(double x[],double y[],int m,int n,double a[],double dt[],double v[]);
	static int   GetGroupingData(TDataSet *pColArray,int nArray,TGroupInfor* &pGroupS);//pColArray原始列数据，nArray列数据中的数据个数，pExplore返回的数据，并返回其个数
	static int   GetGroupingData(TDataSet *pColArray,int nArray,TGroupInfor* &pGroupS,BOOL bCountMissing);
//	int f_plotval(double* &x,BOOL beCalCoor=TRUE, TPlotReco &re_coor=TPlotReco(),int nType=0);
	int f_plotval(double* &x,BOOL beCalCoor, TPlotReco &re_coor,int nType=0);
	int f_DesFreq(TFreqArray* &re_array);
	int f_DesFreq(TFreqArray* &re_array,double weight[]);  //frequence 
	void f_conf_mean(double p,double &x, double &y,int nType=0);
    int f_extrmin(TDataSet* &re_value); //返回最小的结构体
	int f_extrmax(TDataSet* &re_value);
	//double f_trimmean();
	double f_trimmean(int nType=0);
	double f_percentile(double x,int nType=0);
	int f_percentile(double* &rtValue,int nType=0);
	double f_intqr(int nType=0);
	double f_range();
	int f_StemLeaf(TStemLeaf *&re_array, double &stemwidth,int nType=0);
	bool flag_prs;

private:
	TDataSet *array; //传入的数据指针
	long num;       //向量个数
	bool flag_vari;
	bool flag_stdv;
	bool flag_mean;
	bool flag_sum;
    double v_mean;   //平均数 
	double v_sum;    //和
    double v_stdv;      //标准差
    double v_vari;      //方差

	//add by liytao 20030108
	double *pWeight;
	double dbweight;
	//add end
public:
	BOOL bRet;//add by liytao 20030108
};


#endif // !defined(AFX_STATISTICS_H__FEB42AD4_2AC6_4228_A368_FF82B2D0B267__INCLUDED_)
