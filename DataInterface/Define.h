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
//// ��������           
#define  ANALYZE_MEAN            1010    //"��ֵ����"
#define  ANALYZE_FREQUENCY       1020    //"Ƶ�ʷ���"
#define  ANALYZE_EXPLORE         1030    //"����ͳ��"
#define  ANALYZE_PEARSON         1040    //"Ƥ��ѷ���(Pearson)"
#define  ANALYZE_SPEARMAN        1050    //"˹Ƥ�����(Spearman)"
#define  ANALYZE_KENDALL         1060    //"�ϵö����(Kendall)" 
#define  ANALYZE_REGRESSION      1070    //"�ع����"
#define  ANALYZE_KINDENPTEST     1080    //"����������"     
#define  ANALYZE_PROBIT          1090    //"���ʵ�λ�ع�" 
#define  ANALYZE_CHISQUARE       1100    //"��������"
#define  ANALYZE_CLUSTER         1110    //"�������" 
#define  ANALYZE_DISCRI          1120    //"�б����"     
#define  ANALYZE_STEPWISE        1130    //"�𲽻ع�"
#define  ANALYZE_FACTOR          1140    //"���ӷ���" 
#define  ANALYZE_TIMESERIES      1150    //"ʱ�����з���"     
#define  ANALYZE_ANOVA           1160    //"�������"
#define  ANALYZE_PCFX            1170    // "ƫ��ط���" 
#define  ANALYZE_AANOVA          1180     
#define  ANALYZE_BINARY          1190    //"��ֵ�߼��ع�"
#define  ANALYZE_ORDINAL         1200    //"����ع�" 
#define  ANALYZE_CURVEREG        1210    //"���߻ع�"
#define  ANALYZE_KMEANS          1220    //"���پ���"
#define  ANALYZE_NEUNET          1230    //"������"
#define  ANALYZE_DECISTREE       1240    //"������"
#define  ANALYZE_ASSOCIATE       1250    //"��������"
#define  ANALYZE_BLURRY          1260    //"ģ���������"
#define  ANALYZE_ROUGHSET        1270    //"�ֲڼ�����"
#define  ANALYZE_NESTED          1280    //"ѭ��Ƕ�׷���"
#define  ANALYZE_LIFTTABLE       1290    //������       
#define  ANALYZE_KAPLANMEIER     1300    //KANPLANNEIER
#define  ANALYZE_COXREGRESSION   1310    //��˹�ع�
#define  ANALYZE_BINOMIA         1320   //"���ż���",   
#define  ANALYZE_RUNTEST         1330  //"�α����", 
#define  ANALYZE_ONESAMPLEKS     1340   //"������k-S����", 
#define  ANALYZE_TWOINDPTEST     1350  // "��������������", 
#define  ANALYZE_MULTIOINDPTEST  1360// "���������������",  
#define  ANALYZE_TWORELATETEST   1370// "���������������",  
#define  ANALYZE_MULTIRELATETEST 1380//"��������������", 
#define  ANALYZE_CROSSTAB        1390 //�����
#define	 ANALYZE_TIMESEASONAL    1400//"���ڽ⹹", ʱ�����з���  
#define  ANALYZE_EXPONSMOOTHING  1450 //add by liytao 20030410//ָ��ƽ��
#define  ANALYZE_TIMEX11         1410//"X11", ʱ�����з���   
#define  ANALYZE_TIMEARIMA       1420//"Arima", ʱ�����з���  
#define  ANALYZE_FACTORMAIN      1430//���ɷַ�",  
#define  ANALYZE_TIMEAR          1440//"AR model", ʱ�����з���                      
#define	 ANALYZE_TIMEMA	         1550//"MA model",   ʱ�����з���                  
#define	 ANALYZE_ONESMPLTTEST	 1560//��ֵ�ĵ���������
#define	 ANALYZE_INDTTEST		 1570//��ֵ��˫����T����
#define	 ANALYZE_PAIRTTEST		 1580// //��ֵ���������T����
#define  ANALYZE_PROP_ONESMPLTTEST    1590 //�����ĵ���������
#define  ANALYZE_SQUARED_ONESMPLTTEST 1600 //����ĵ���������
#define  ANALYZE_PROP_PAIRTTEST       1610 //������˫��������
#define  ANALYZE_SQUARED_PAIRTTEST    1620 //�����˫��������


#define COM_DATASOURCE	1			//����Դ
#define COM_DATAPROCESS 2			//���ݳ���
#define COM_RESULT		3			//�����
#define COM_TREE		4			//������
#define COM_ASSESS		5			//��������
#define COM_CLUSTER		6			//ģ������
#define COM_NEURAL_NET  7			//������
#define COM_VARSEL      8           //����ѡ��
#define COM_PARTITION   9	        //���ݷָ�
#define COM_SVM			10          //֧��������
#define COM_ROUGH		11			//�ֲڼ�
#define COM_NESTED		12			//������
#define COM_ASSESS_M	13			//��������_����
#define COM_NEURAL_BP	14			//������ BP

////////////////////////////////////////////////////////////////////////////
//// ͼ������           
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
#define  TCHART_HIGHLOW          2160//�ߵ�ͼ
#define  TCHART_PP               2170
#define  TCHART_QQ               2180
#define  TCHART_EMP				 2190//�����ۻ�ͼ�;����ܶ�ͼ

///////////////////////////////////////////////////////////////////////////////
////����������
#define  EXPLORE_ERANGE          4010    //��������-��Χ
#define  EXPLORE_EMINIMUM        4020    //��������-��Сֵ
#define  EXPLORE_EMAXIMUM        4030    //��������-���ֵ
#define  EXPLORE_EMEDIUM         4040    //��������-�м�ֵ
#define  EXPLORE_ESUM            4050    //��������-�ܺ�
#define  EXPLORE_EMEAN           4060    //��������-ƽ��ֵ
#define  EXPLORE_EMEANERROR      4070    //��������-ƽ�����
#define  EXPLORE_EMODE           4080    //��������-mode
#define  EXPLORE_ECONFIDENCE     4090    //��������-��������
#define  EXPLORE_ETRIMMED        4100    //��������-5% Trimmed mean
#define  EXPLORE_ESTD            4110    //��������-��׼ͳ��
#define  EXPLORE_EVARIANCE       4120    //��������-����
#define  EXPLORE_ESKEWNESS       4130    //��������-ƫ̬ϵ��
#define  EXPLORE_ESKEWNESSERROR  4140   //��������-ƫб���
#define  EXPLORE_EKURTOSIS       4150   //��������-���
#define  EXPLORE_EKURTOSISERROR  4160   //��������-������
#define  EXPLORE_EINTERRANGE     4170   //��������-���䷶Χ


//////////////////////////////////////////////////////////////////////////
//// �ṹ�嶨��
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
	
    void DecCopy(const TIntStruct &v,unsigned int step=0)   // ��Ԫ�ؿ���
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
	int iCount;//���鳤��

	int *pValue;//�༶�����ţ�����
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
	CTString name;//����
}TIDName;

#define ID_ROLE_INPUT    1
#define ID_ROLE_CUSTOMID 2
#define ID_ROLE_PRODUCT  3
#define ID_ROLE_REJECT   4

#define DATA_INPUT			_T("�������")
#define DATA_REJECT			_T("������")
#define DATA_TARGET			_T("Ŀ�����")
#define DATA_PRODUCTID		_T("��ƷID")
#define DATA_CUSTOMERID		_T("����ID")
#define DATA_NORMAL			_T("һ����������")
#define DATA_SPECIFY		_T("ָ����������")
#define DATA_DECISION		_T("��������")
#define DATA_OBJECT	    	_T("Ŀ�����")
 


struct TFieldRole
{
	int nFieldNo;
	int nRole;
	CTString strRole;
	CTString strFieldType;
	CTString strFieldName; //yzmModify ������������������Ψһ�ԣ�������Ӧ�ȽϺ�
	TFieldRole()
	{
		nFieldNo = -1;
		nRole = ID_ROLE_INPUT;
	}
};
//TDataSet ���°汾  û���ַ�����
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


//����Ӧ�ֱ���CUIntArray ��CStringArrayȡ��
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