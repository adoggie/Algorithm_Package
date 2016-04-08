// TChart.h: interface for the CTChart class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHART_H__4BEEF0D6_5178_468C_A631_A245F10C8B0C__INCLUDED_)
#define AFX_CHART_H__4BEEF0D6_5178_468C_A631_A245F10C8B0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Statistics.h"

//轴类型
const int  XAXIS_INT          = 0;
const int  YAXIS_INT          = 1;
const int  DEFINE_INT         = 2;
//最大有效记录
const int  MAX_VALID_INT      = 3000;
const int  MAX_MULTI_INT      = 500; //多线时，最大
//常量－图表类型
const char CHART_LINE[]               = "LINE";
const char CHART_BAR[]                = "BAR";
const char CHART_PIE[]                = "PIE";
const char CHART_AREA[]               = "AREA";
const char CHART_BOXPLOT[]            = "BOXPLOT";
const char CHART_HISTOGRAM[]          = "HISTOGRAM";
const char CHART_PARETO[]             = "PARETO";
const char CHART_ERRORBAR[]           = "ERRORBAR";
const char CHART_SEQUENCE[]           = "SEQUENCE";
const char CHART_SCATTER[]            = "SCATTER";
const char CHART_SELFCORRELATIONS[]   = "SELFCORRELATIONS";
const char CHART_CROSSCORRELATIONS[]  = "CROSSCORRELATIONS";
const char CHART_CONTROL[]            = "CONTROL";
const char CHART_ROC[]                = "ROC";
const char CHART_CLUSTERING[]         = "CLUSTERING";
const char CHART_HIGHLOW[]            = "HIGHLOW";//高低图
const char CHART_PP[]                 = "PP";
const char CHART_QQ[]                 = "QQ";
const char CHART_EMP[]                = "EMP";//经验累积图和经验密度图
//类型
const char TYPE_STR[]     = "/TYPE";
//线类型
const char SIMPLELINE_STR[]   = "SIMPLELINE";
const char MULTIPLELINE_STR[] = "MULTILINE";
//条状图
const char SIMPLEBAR_STR[]    = "SIMPLEBAR";
const char GROUPBAR_STR[]     = "GROUPBAR";
const char SEGMENTBAR_STR[]   = "SEGMENTBAR";
//散点图
const char SIMPLESCATTER_STR[]= "SIMPLESCATTER";
const char SUPERSCATTER_STR[] = "SUPERSCATTER";
//模式
const char MODE_STR[]     = "/MODE";
//数据模式类
const char CLASS_STR[]    = "CLASS";
const char VARIABLE_STR[] = "VAR";
const char VALUE_STR[]    = "VALUE";
//坐标轴
const char AXISDESC_STR[]  = "/AXISDESC";
const char AXISNDESC_STR[] = "/AXISNDESC"; 
const char VARDESC_STR[]   = "/VARDESC";
const char VARNDESC_STR[]  = "/VARNDESC";
//数据类型
const char DTA_STR[]  = "/DTA";
const char DTV_STR[]  = "/DTV";
//权重
const char WEIGHT_STR[] = "/WEIGHT";
//标签
const char OUTPUT_STR[] = "/OUTPUT";
const char LABEL_STR[]  = "LABEL";
//数据类
const char REC_STR[]           = "REC";
const char RECSCALE_STR[]      = "RECSCALE";
const char RECTOTAL_STR[]      = "RECTOTAL";
const char RECTOTALSCALE_STR[] = "RECTOTALSCALE";//之前均为描述类，之后均为函数类
const char MEAN_STR[]          = "MEAN";
const char VAR_STR[]		   = "VAR";
const char MOD_STR[]		   = "MOD";
const char MID_STR[]		   = "MID";
const char STD_STR[]		   = "STD";
const char MIN_STR[]		   = "MIN";
const char MAX_STR[]		   = "MAX";
const char SUM_STR[]		   = "SUM";
//自定义行
const char DEFLINE_STR[]       = "/DEFLINE";
//直方图
const char DIV_STR[]           = "/DIV";
const char PLOT_STR[]          = "/PLOT";
const char NORMAL_STR[]        = "NORMAL";
const char MAXNUM_STR[]        = "/MAXNUM";
const int  DIV_MIN_INT         = 6;
const int  DIV_MAX_INT         = 100;
//自相关图-互相关图
const char SELFCORR_STR[]      = "SELF";
const char PARTIALCORR_STR[]   = "PARTIAL";
const char DIFFRANK_STR[]      = "/DIFFRANK";
const char MAXLAGRANK_STR[]    = "/MAXLAGRANK";
const int  SELFDIFF_MIN_INT    = 0;
const int  SELFDIFF_MAX_INT    = 10;
const int  SELFMAXLAG_MIN_INT  = 12;
const int  SELFMAXLAG_MAX_INT  = 30;
const int  CROSSDIFF_MIN_INT   = 0;
const int  CROSSDIFF_MAX_INT   = 3;
const int  CROSSMAXLAG_MIN_INT = 0;
const int  CROSSMAXLAG_MAX_INT = 999;
//经验累积图和经验密度图
const char EMP_EXP_STR[]       = "EXP";
const char EMP_DEN_STR[]       = "DEN";
//盒状图
const char SPLIT_STR[]         = "SPLIT";
//信息提示
const char MSG_TITLE_STR[]     = "提示信息";
//轴及变量类型
const int  ERR_INT             = 0;
const int  AXIS_DESC_INT       = 1;
const int  AXIS_NDESC_INT      = 2;
const int  VAR_DESC_INT        = 4;
const int  VAR_NDESC_INT       = 8;
class CChart
{
public:
	CChart();
	virtual ~CChart();
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO)=0;
protected:
	virtual void Init();
	CTString GetLastErrMsg();
	void SetChartType(const char *pszChartType);
protected:
	virtual bool Verify()=0;
	virtual bool ParseAxis(string VO);
	virtual bool ParseVar(string VO);
	virtual bool ParseDefLine(string VO);
	virtual bool ParseDataType(string VO);
	virtual bool ParseWeight(string VO);
	virtual bool ParseOutput(string VO);
	virtual bool ParseTypeMode(string VO) = 0;
	virtual bool GetValueFromVo(string VO) = 0;
protected:
	enum EType
	{
		SIMPLELINE,   //简单直线
			MULTIPLELINE, //多直线
			SIMPLEBAR,    //简单条状
			GROUPBAR,     //分组条状
			SEGMENTBAR,   //分段条状
			SIMPLESCATTER,//简单散点图
			SUPERSCATTER  //重合散点图
	};
	enum EModel
	{
		CLASS,   //变量分类
			VARI,    //变量
			VALUE    //变量值
	};
	enum EData
	{
		REC,           //记录数  －X轴变量所对应的记录数(一个变量)
			RECSCALE,      //记录比例
			RECTOTAL,      //记录累计－X轴变量所对应的和(一个变量)
			RECTOTALSCALE, //记录累计比例
			MEAN,          //变量均值
			VAR,           //变量方差
			MOD,           //变量众数
			MID,           //变量中位数
			STD,           //变量标准差
			MIN,           //变量最小值
			MAX,           //变量最大值
			RECORD,        //变量记录数－相同X下，Y轴对应的变量记录数(两个变量)
			SUM            //变量和    －相同X下，Y轴对应的变量和(两个变量)
	};
	EType  m_eType; //线类型
	EModel m_eMode; //数据模式
	//EData  m_eDataX;//X轴数据类型
	//EData  m_eDataY;//Y轴数据类型
	//EData  m_eDataD;//自定义数据类型
	CTArray <EData, EData> m_arrX;//X轴所对应的数据类型
	CTArray <EData, EData> m_arrY;//Y轴所对应的数据类型
	CTArray <EData, EData> m_arrD;//自定义所对应的数据类型
protected:
	bool IsSameCallFunc(int nType); //判断某轴中是否有相同的数据类型
	bool GetAxisLabel(EData eData, CTString &szLabel);
	bool GetStatistical(CStatistics statistics, EData eData, int nRowCnt, double &fValue);
	bool GetDataType(int nIndex, int nType, EData &eData);//Type:0-X,1-Y,2-D;
	CTString GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue);
protected://排序算法
	void PointArrayRec(TDataPointStrVal **pArray,int d,int h,int nCul,bool bAsceding);
	bool PointArraySort(TDataPointStrVal** pArray,int nSize,int nCul,bool bAsceding);
	void QuickSortRecursive(TDataPointStrVal *pArray,int d,int h,bool bAsceding,bool bAxis);
	bool QuickSort(CTString* pArray,int nSize);
	void QuickSortRecursive(CTString *pArray,int d,int h);
	bool QuickSort(double* pArray,int nSize,bool bAsceding=true);
	void QuickSortRecursive(double *pArray,int d,int h,bool bAsceding);
	bool QuickSort(TDataPointStrVal* pArray,int nSize,bool bAsceding = true,bool bAxis = true);
	bool QuickSort(TDataPointValVal* pArray,int nSize,bool bAsceding = true,bool bAxis = true);
	void PointArrayRec(TDataPointValVal **pArray,int d,int h,int nCul,bool bAsceding);
	bool PointArraySort(TDataPointValVal** pArray,int nSize,int nCul,bool bAsceding=true);
	void QuickSortRecursive(TDataPointValVal *pArray,int d,int h,bool bAsceding,bool bAxis);
protected:
	int  m_nAVType;     //轴及变量类型
	int  m_nWeight;     //权重索引
	bool m_bWeight;     //权重标志
	bool m_bShowLabel;  //设置显示标签
	bool m_bSplit;      //多个变量在一起时，是否分开考究－盒状图时
	TIntStruct m_tVarX; //X轴－变量索引
	TIntStruct m_tVarY; //Y轴－变量索引
	TIntStruct m_tVarD; //自定义行
	CTString   m_szErrMsg;   //错误信息
	CTString   m_szChartName;//图形名
	CTString   m_szChartType;//图形类型名
	CDataInterface *m_pDataInterface;
protected:
	CResult *m_pResult;   //结果集
};

#endif // !defined(AFX_CHART_H__4BEEF0D6_5178_468C_A631_A245F10C8B0C__INCLUDED_)
