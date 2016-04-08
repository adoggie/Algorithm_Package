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
	int  nCount;//����ŵ�ǰͼ����ÿ�������е���Ч���ݸ���
	CTString strName;
	TDoubleStruct pPlotVal;////���ÿ���Ӻ�״ͼ��ʵ��ͼ������
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
	int nStatPlot;          //����ŵ�ǰͼ�ε��Ӻ�״ͼ����
	CTString strName;
	TPlotReco  tPlotReco;   //��ź�״ͼ����Сֵ�����ֵ�����������������
	TStatBoxPlot *pStatPlot;//���ÿ���Ӻ�״ͼ��ʵ��ͼ������

}TStatBoxPlotS;


class CStatistics  
{
public:
	CStatistics();
    CStatistics(TDataSet *, long);     //���캯��
	virtual ~CStatistics();
	void     SetData(TDataSet *pArray,int nDataCount);
 	double*   Describle(TDataSet *pArray,int nDataCount,int *pMethodS,int nMCount);//CDataInterface *pDataInterface);
	double f_mean();            //��ƽ����
	static int GetGroupData(TDataSet* pArray,int nCount,TIntStruct tInt, TDataSet* &pGroupArray,bool flag = true);

public:
	double f_sem();             
	double f_min();
	double f_max();
	double f_sum();             //���  
	double f_medium();          //����λ��
	double f_skewness();        //��skewness(g1)
	double f_mode();            //most frequent occurrance 
	double f_stadeviation();    //��׼���
	double f_variance();        //����   
	double f_kurtosis(double &dSeg1,double &dSeg2);//��kurtosis(g2)

public:
	static void LinearInsert(TDataSet * &pDataSet, int nRowCount);
    TPlotReco GetCoordinate(TDataSet *pDataArray, int nArray);
	void prshl(TDataSet*p,int n,bool reSort=false);  //ʵ����ϣ������,reSort��ʾ�Ƿ�����
	static int   achol(double a[],int n,int m,double d[]);
	static int   GetGroupingData(TDataSet *pColArray,int nArray,TGroupInfor* &pGroupS);//pColArrayԭʼ�����ݣ�nArray�������е����ݸ�����pExplore���ص����ݣ������������

	int f_plotval(double* &x,bool beCalCoor,TPlotReco &re_coor);
//	int f_plotval(double* &x,bool beCalCoor=true,TPlotReco &re_coor=TPlotReco());
	void f_conf_mean(double p,double &x, double &y);
    int f_extrmin(TDataSet* &re_value); //������С�Ľṹ��
	int f_extrmax(TDataSet* &re_value);
	double f_trimmean();
	double f_percentile(double x);
	int f_percentile(double* &rtValue);
	double f_intqr();
	double f_range();
	bool flag_prs;

private:
	TDataSet *array; //���������ָ��
	long num;        //��������
	bool flag_vari;
	bool flag_stdv;
	bool flag_mean;
	bool flag_sum;
    double v_mean;   //ƽ���� 
	double v_sum;    //��
    double v_stdv;   //��׼��
    double v_vari;   //����

};


#endif // !defined(AFX_STATISTICS_H__FEB42AD4_2AC6_4228_A368_FF82B2D0B267__INCLUDED_)
