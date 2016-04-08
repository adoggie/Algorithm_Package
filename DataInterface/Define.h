#ifndef _Define_H_03434_3434322_Q
#define _Define_H_03434_3434322_Q

#include <math.h>
namespace a1{
	//#include "iostream.h"
}

#include "float.h"
//#include <afxtempl.h>
#include "../Public/TString.h"


#define  pi  3.1415926535897932384626 

#define  macDESCRIPMETHOD  17

////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////
//// 分析类型           
#define  ANALYZE_MEAN            1010    //"均值分析"
#define  ANALYZE_FREQUENCY       1020    //"频率分析"
#define  ANALYZE_EXPLORE         1030    //"描述统计"
#define  ANALYZE_PEARSON         1040    //"皮尔逊相关(Pearson)"
#define  ANALYZE_SPEARMAN        1050    //"斯皮文相关(Spearman)"
#define  ANALYZE_KENDALL         1060    //"肯得尔相关(Kendall)" 
#define  ANALYZE_REGRESSION      1070    //"回归分析"
#define  ANALYZE_KINDENPTEST     1080    //"多样本检验"     
#define  ANALYZE_PROBIT          1090    //"概率单位回归" 
#define  ANALYZE_CHISQUARE       1100    //"卡方检验"
#define  ANALYZE_CLUSTER         1110    //"聚类分析" 
#define  ANALYZE_DISCRI          1120    //"判别分析"     
#define  ANALYZE_STEPWISE        1130    //"逐步回归"
#define  ANALYZE_FACTOR          1140    //"因子分析" 
#define  ANALYZE_TIMESERIES      1150    //"时间序列分析"     
#define  ANALYZE_ANOVA           1160    //"方差分析"
#define  ANALYZE_PCFX            1170    // "偏相关分析" 
#define  ANALYZE_AANOVA          1180     
#define  ANALYZE_BINARY          1190    //"二值逻辑回归"
#define  ANALYZE_ORDINAL         1200    //"有序回归" 
#define  ANALYZE_CURVEREG        1210    //"曲线回归"
#define  ANALYZE_KMEANS          1220    //"快速聚类"
#define  ANALYZE_NEUNET          1230    //"神经网络"
#define  ANALYZE_DECISTREE       1240    //"决策树"
#define  ANALYZE_ASSOCIATE       1250    //"关联规则"
#define  ANALYZE_BLURRY          1260    //"模糊聚类分析"
#define  ANALYZE_ROUGHSET        1270    //"粗糙集分析"
#define  ANALYZE_NESTED          1280    //"循环嵌套分析"
#define  ANALYZE_LIFTTABLE       1290    //寿命表       
#define  ANALYZE_KAPLANMEIER     1300    //KANPLANNEIER
#define  ANALYZE_COXREGRESSION   1310    //考斯回归
#define  ANALYZE_BINOMIA         1320   //"符号检验",   
#define  ANALYZE_RUNTEST         1330  //"游标检验", 
#define  ANALYZE_ONESAMPLEKS     1340   //"单样本k-S检验", 
#define  ANALYZE_TWOINDPTEST     1350  // "两个立样本检验", 
#define  ANALYZE_MULTIOINDPTEST  1360// "多个独立样本检验",  
#define  ANALYZE_TWORELATETEST   1370// "两个相关样本检验",  
#define  ANALYZE_MULTIRELATETEST 1380//"多个相关样本检验", 
#define  ANALYZE_CROSSTAB        1390 //交叉表
#define	 ANALYZE_TIMESEASONAL    1400//"季节解构", 时间序列分析  
#define  ANALYZE_EXPONSMOOTHING  1450 //add by liytao 20030410//指数平滑
#define  ANALYZE_TIMEX11         1410//"X11", 时间序列分析   
#define  ANALYZE_TIMEARIMA       1420//"Arima", 时间序列分析  
#define  ANALYZE_FACTORMAIN      1430//主成分法",  
#define  ANALYZE_TIMEAR          1440//"AR model", 时间序列分析                      
#define	 ANALYZE_TIMEMA	         1550//"MA model",   时间序列分析                  
#define	 ANALYZE_ONESMPLTTEST	 1560//均值的单样本检验
#define	 ANALYZE_INDTTEST		 1570//均值的双样本T检验
#define	 ANALYZE_PAIRTTEST		 1580// //均值的配对样本T检验
#define  ANALYZE_PROP_ONESMPLTTEST    1590 //比例的单样本检验
#define  ANALYZE_SQUARED_ONESMPLTTEST 1600 //方差的单样本检验
#define  ANALYZE_PROP_PAIRTTEST       1610 //比例的双样本检验
#define  ANALYZE_SQUARED_PAIRTTEST    1620 //方差的双样本检验


#define COM_DATASOURCE	1			//数据源
#define COM_DATAPROCESS 2			//数据抽样
#define COM_RESULT		3			//结果集
#define COM_TREE		4			//决策树
#define COM_ASSESS		5			//关联规则
#define COM_CLUSTER		6			//模糊聚类
#define COM_NEURAL_NET  7			//神经网络
#define COM_VARSEL      8           //变量选择
#define COM_PARTITION   9	        //数据分割
#define COM_SVM			10          //支持向量机
#define COM_ROUGH		11			//粗糙集
#define COM_NESTED		12			//孤立点
#define COM_ASSESS_M	13			//关联规则_多联
#define COM_NEURAL_BP	14			//神经网络 BP

////////////////////////////////////////////////////////////////////////////
//// 图形类型           
#define  TCHART_LINE             2010
#define  TCHART_BAR              2020
#define  TCHART_PIE              2030 
#define  TCHART_AREA		     2040
#define  TCHART_BOXPLOT          2050
#define  TCHART_HISTOGRAM        2060 
#define  TCHART_PARETO           2070 
#define  TCHART_ERRORBAR         2080     
#define  TCHART_SEQUENCE         2090 
#define  TCHART_SCATTER          2100 
#define  TCHART_AUTOCORRELATIONS 2110 
#define  TCHART_CROSS_CORRELATIONS 2120 
#define  TCHART_CONTROL          2130 
#define  TCHART_ROC              2140
#define  TCHART_CLUSTERING       2150
#define  TCHART_HIGHLOW          2160//高低图
#define  TCHART_PP               2170
#define  TCHART_QQ               2180
#define  TCHART_EMP				 2190//经验累积图和经验密度图

///////////////////////////////////////////////////////////////////////////////
////描述方法集
#define  EXPLORE_ERANGE          4010    //描述分析-范围
#define  EXPLORE_EMINIMUM        4020    //描述分析-最小值
#define  EXPLORE_EMAXIMUM        4030    //描述分析-最大值
#define  EXPLORE_EMEDIUM         4040    //描述分析-中间值
#define  EXPLORE_ESUM            4050    //描述分析-总和
#define  EXPLORE_EMEAN           4060    //描述分析-平均值
#define  EXPLORE_EMEANERROR      4070    //描述分析-平均误差
#define  EXPLORE_EMODE           4080    //描述分析-mode
#define  EXPLORE_ECONFIDENCE     4090    //描述分析-置信区间
#define  EXPLORE_ETRIMMED        4100    //描述分析-5% Trimmed mean
#define  EXPLORE_ESTD            4110    //描述分析-标准统计
#define  EXPLORE_EVARIANCE       4120    //描述分析-方差
#define  EXPLORE_ESKEWNESS       4130    //描述分析-偏态系数
#define  EXPLORE_ESKEWNESSERROR  4140   //描述分析-偏斜误差
#define  EXPLORE_EKURTOSIS       4150   //描述分析-峰度
#define  EXPLORE_EKURTOSISERROR  4160   //描述分析-峰度误差
#define  EXPLORE_EINTERRANGE     4170   //描述分析-区间范围


//////////////////////////////////////////////////////////////////////////
//// 结构体定义
typedef  struct AFX_EXT_CLASS TIntStruct
{
	TIntStruct()
	{
		refcnt=1;
		iCount=0;
		pValue=NULL;
	}

    TIntStruct(int count)
	{
		refcnt=1;
		iCount=count;
		pValue=new int[iCount];
	}

	TIntStruct(int count,int* p)
	{
		refcnt=1;
		iCount=count;
		pValue=p;
	}
 
	void operator =(const TIntStruct &v)
	{
		v.refcnt++;
		iCount = v.iCount;
		pValue = v.pValue;
		refcnt = v.refcnt;
	}

	void Copy(const TIntStruct &v)
	{
		DeleteObject();
        iCount=v.iCount;
		pValue=new int[iCount];
		for(int ii=0;ii<iCount;ii++)
			pValue[ii]=v.pValue[ii];
	}
	
    void DecCopy(const TIntStruct &v,unsigned int step=0)   // 减元素拷贝
    {
		DeleteObject();
        iCount=v.iCount - step;
		pValue=new int[iCount];
		for(int ii=0;ii<iCount;ii++)
			pValue[ii]=v.pValue[ii+step];
    }
	bool IsEmpty()
	{
		return (iCount==0 || pValue==NULL)?true:false;
	}

	void DeleteObject()
	{
		iCount=0;
		if(pValue!=NULL)
		{
			delete [] pValue;
			pValue=NULL;
		}
	}

	int Find(int id)
	{
		for(int ii=0;ii<iCount;ii++)
			if(pValue[ii]==id)
				return ii;

		return -1;
	}

    int GetIDAt(int index)
	{
		if ((index>=0) && (index<iCount-ID_STAPOS))
			return pValue[index+ID_STAPOS];
		return -2;
	}

	bool beExist(int id)
	{
		return (Find(id)>=0)?true:false;
	}

	void RemoveAt(int index)
	{
		if(index>=0 && index<iCount)
		{
			int i=index;
			for(;i<iCount-1;i++)
				pValue[i]=pValue[i+1];
			pValue[i]=-1;
			iCount--;
		}
	}
    enum {ID_STAPOS=0};  
   mutable  int refcnt;	//-- UNIX PORT 
	int iCount;//数组长度

	int *pValue;//多级索引号，数组
}TIntStruct;


typedef AFX_EXT_CLASS struct TIDName
{
	TIDName()
	{
		id=0;
		name="";
	}
	
	~TIDName()
	{
		name.Empty();
	}

	TIDName(int nID,CTString sName)
	{
		id=nID;
		name=sName;
	}
	


	int     id;//ID
	CTString name;//名称
}TIDName;

#define ID_ROLE_INPUT    1
#define ID_ROLE_CUSTOMID 2
#define ID_ROLE_PRODUCT  3
#define ID_ROLE_REJECT   4

#define DATA_INPUT			_T("输入变量")
#define DATA_REJECT			_T("不输入")
#define DATA_TARGET			_T("目标变量")
#define DATA_PRODUCTID		_T("商品ID")
#define DATA_CUSTOMERID		_T("交易ID")
#define DATA_NORMAL			_T("一般条件属性")
#define DATA_SPECIFY		_T("指定条件属性")
#define DATA_DECISION		_T("决策属性")
#define DATA_OBJECT	    	_T("目标变量")
 


struct TFieldRole
{
	int nFieldNo;
	int nRole;
	CTString strRole;
	CTString strFieldType;
	CTString strFieldName; //yzmModify 列名（变量名）具有唯一性，以它对应比较好
	TFieldRole()
	{
		nFieldNo = -1;
		nRole = ID_ROLE_INPUT;
	}
};
//TDataSet 的新版本  没有字符类型
typedef struct TDataCol  
{
	TDataCol()
	{
		iIndex=-1;
		dValue=0;
	}

	TDataCol(int index,double val)
	{
		iIndex=index;
		dValue=val;
	}

	TDataCol& operator=(const TDataCol& t)
	{
		iIndex = t.iIndex;
		dValue = t.dValue;
		return *this;
	}
	
	bool operator<(TDataCol t)
	{
		return (dValue<t.dValue)?true:false;
	}
	
	bool operator<=(TDataCol t)
	{
		return (dValue<=t.dValue)?true:false;
	}
	
	bool operator!=(TDataCol t)
	{
		return (dValue!=t.dValue)?true:false;
	}
	
	bool operator==(TDataCol t)
	{
			return (dValue==t.dValue)?true:false;
	}
	
	bool operator>=(TDataCol t)
	{
		return (dValue>=t.dValue)?true:false;
	}
	
	bool operator>(TDataCol t)
	{
		return (dValue>t.dValue)?true:false;
	}

	int iIndex;
	double dValue;

}TDataCol;


//将来应分别用CUIntArray 和CStringArray取代
typedef struct TDataSet
{
	TDataSet()
	{
		iIndex=-1;
		dValue=0;
		strVal="";
	}

	TDataSet(int index,double val,CTString strValue="")
	{
		iIndex=index;
		dValue=val;
		strVal=strValue;
	}

	TDataSet& operator=(TDataSet& t)
	{
		iIndex = t.iIndex;
		dValue = t.dValue;
		strVal = t.strVal;

		return *this;
	}
	
	bool operator<(TDataSet t)
	{
		if(strVal=="" && t.strVal=="")
			return (dValue<t.dValue)?true:false;
		else
			return (strVal<t.strVal)?true:false;
	}
	
	bool operator<=(TDataSet t)
	{
		if(strVal=="" && t.strVal=="")
			return (dValue<=t.dValue)?true:false;
		else
			return (strVal<=t.strVal)?true:false;
	}
	
	bool operator!=(TDataSet t)
	{
		if(strVal=="" && t.strVal=="")
			return (dValue!=t.dValue)?true:false;
		else
			return (strVal!=t.strVal)?true:false;
	}
	
	bool operator==(TDataSet t)
	{
		if(strVal=="" && t.strVal=="")
			return (dValue==t.dValue)?true:false;
		else
			return (strVal==t.strVal)?true:false;
	}
	
	bool operator>=(TDataSet t)
	{
		if(strVal=="" && t.strVal=="")
			return (dValue>=t.dValue)?true:false;
		else
			return (strVal>=t.strVal)?true:false;
	}
	
	bool operator>(TDataSet t)
	{
		if(strVal=="" && t.strVal=="")
			return (dValue>t.dValue)?true:false;
		else
			return (strVal>t.strVal)?true:false;
	}

	int iIndex;
	double dValue;
	CTString strVal;
}TDataSet;

struct TDoubleStruct
{
	TDoubleStruct()
	{
		nCount=0;
		dValue=NULL;
	}
//	~TDoubleStruct()
//	{
	//	DeleteObject();
//	}

	TDoubleStruct(int count, double* value)
	{
		nCount=count;
		dValue=value;
	}

	void DeleteObject()
	{
		nCount=0;
		if(dValue!=NULL)
		{
			delete []dValue;
			dValue=NULL;
		}
	}



	int  nCount;
	double *dValue;
};


struct TGroupInfor
{
	TGroupInfor()
	{
		dValue=0;
		strVal="";
	}

	void DeleteObject()
	{
		dValue=0;
		strVal="";
		pIndex.DeleteObject();
	}

	void operator=(TDataSet t)
	{
		dValue=t.dValue;
		strVal=t.strVal;
	}

	double dValue;
	CTString strVal;
	TIntStruct pIndex;
};


#endif //_Define_H_03434_3434322_Q