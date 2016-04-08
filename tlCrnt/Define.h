#ifndef __DEFINE_H
#define __DEFINE_H
typedef struct TFreqArray  //返回频率分析的结果
{
	CTString ListValue;  //把原有的数值转换为字符串。
	CTString Number;   //个数
    CTString Percent; //比率
    CTString ValPercent;//有效比率
	CTString CumPercent;//累计比率
}TFreqArray;

typedef struct TStemLeaf  //stem and leaf 的返回结构体
{
    CTString freq;  
	CTString stem;
	CTString leaf;
}TStemLeaf;

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

//	~TStatBoxPlotS()
	void DeleteObject()
	{
		//
		strName="";
		if(pStatPlot)
		{
			for(int ii=0;ii<nStatPlot;ii++)
				pStatPlot[ii].DeleteObject();
		    delete [] pStatPlot;
			pStatPlot=NULL;
		}
		nStatPlot=0;
	}
	int nStatPlot;//存放着当前图形的子盒状图个数
	CTString strName;//
	TPlotReco  tPlotReco;//存放盒状图的最小值，最大值，区间个数及区间跨度
	TStatBoxPlot *pStatPlot;////存放每个子盒状图的实际图形数据

}TStatBoxPlotS;


typedef struct TStatMeanPlotS
{
	TStatMeanPlotS()
	{
		strFactName="";
		nDotCount=0;
		pFactValue=NULL;
		pMeanValue=NULL;
	}

	TStatMeanPlotS(CTString strFact,int nDot)
	{
		strFactName=strFact;
		nDotCount=nDot;
		pFactValue=new double [nDotCount];
		pMeanValue=new double [nDotCount];
	}

	void DeleteObject()
	{
		if(pFactValue)
		{
			delete [] pFactValue;
			pFactValue=NULL;
		}

		if(pMeanValue)
		{
			delete [] pMeanValue;
			pMeanValue=NULL;
		}
	}
	CTString strFactName;
	int     nDotCount;
	double *pFactValue;
	double *pMeanValue;
	TPlotReco tPlotReco;
}TStatMeanPlotS;

typedef struct TDDoubleStruct
{
	TDDoubleStruct()
	{
		dValue1=0;
		dValue2=0;
	}

    TDDoubleStruct(double value1,double value2)
	{
		dValue1=value1;
		dValue2=value2;
	}
	double dValue1;
	double dValue2;
}TDDoubleStruct;


struct TFreqArrayS
{
	TFreqArrayS()
	{
		iCount=0;
		pFreqArray=NULL;
	}
	
	TFreqArrayS(int count,TFreqArray *pFreq)
	{
		iCount=count;
		pFreqArray=pFreq;
	}
	
	void DeleteObject()
	{
		iCount=0;
		if(pFreqArray)
		{
			delete [] pFreqArray;
			pFreqArray=NULL;
		}
	}
	int iCount;
	TFreqArray *pFreqArray;
};

//存贮行列信息
class CFldInfo
{
public:
	CFldInfo()
	{
		fValue  = 0;
		fWeight = 0;
		v.destroy();
		w.destroy();
		nIndex  = 0;
	};
	~CFldInfo()
	{
		fValue  = 0;
		fWeight = 0;
		v.destroy();
		w.destroy();
		nIndex  = 0;
	};
public:
	int    nIndex; //索引－不同用处
	double fValue; //域值
	double fWeight;//对应的权重和
	CDoubleVector v;//数值-行向量时用
	CDoubleVector w;//权重-行向量时用
};

//存贮行列信息-频率分析，范围
class CRangeInfo
{
public:
	CRangeInfo()
	{
		bRange = TRUE;
		fValFrom = 0;
		fValTo   = 0;
		fTotalWeight = 0;
	};
	~CRangeInfo()
	{
	};
public:
	BOOL   bRange;      //是否存在范围－不存在范围，值域为fValFrom.
	double fValFrom;    //开始域值
	double fValTo;      //结束域值
	double fTotalWeight;//对应的权重和
	CDoubleVector v;//数值-行向量时用
	CDoubleVector w;//权重-行向量时用
};

#endif