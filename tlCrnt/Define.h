#ifndef __DEFINE_H
#define __DEFINE_H
typedef struct TFreqArray  //����Ƶ�ʷ����Ľ��
{
	CTString ListValue;  //��ԭ�е���ֵת��Ϊ�ַ�����
	CTString Number;   //����
    CTString Percent; //����
    CTString ValPercent;//��Ч����
	CTString CumPercent;//�ۼƱ���
}TFreqArray;

typedef struct TStemLeaf  //stem and leaf �ķ��ؽṹ��
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
	int nStatPlot;//����ŵ�ǰͼ�ε��Ӻ�״ͼ����
	CTString strName;//
	TPlotReco  tPlotReco;//��ź�״ͼ����Сֵ�����ֵ�����������������
	TStatBoxPlot *pStatPlot;////���ÿ���Ӻ�״ͼ��ʵ��ͼ������

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

//����������Ϣ
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
	int    nIndex; //��������ͬ�ô�
	double fValue; //��ֵ
	double fWeight;//��Ӧ��Ȩ�غ�
	CDoubleVector v;//��ֵ-������ʱ��
	CDoubleVector w;//Ȩ��-������ʱ��
};

//����������Ϣ-Ƶ�ʷ�������Χ
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
	BOOL   bRange;      //�Ƿ���ڷ�Χ�������ڷ�Χ��ֵ��ΪfValFrom.
	double fValFrom;    //��ʼ��ֵ
	double fValTo;      //������ֵ
	double fTotalWeight;//��Ӧ��Ȩ�غ�
	CDoubleVector v;//��ֵ-������ʱ��
	CDoubleVector w;//Ȩ��-������ʱ��
};

#endif