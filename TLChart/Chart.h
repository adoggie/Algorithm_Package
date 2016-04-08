// TChart.h: interface for the CTChart class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHART_H__4BEEF0D6_5178_468C_A631_A245F10C8B0C__INCLUDED_)
#define AFX_CHART_H__4BEEF0D6_5178_468C_A631_A245F10C8B0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Statistics.h"

//������
const int  XAXIS_INT          = 0;
const int  YAXIS_INT          = 1;
const int  DEFINE_INT         = 2;
//�����Ч��¼
const int  MAX_VALID_INT      = 3000;
const int  MAX_MULTI_INT      = 500; //����ʱ�����
//������ͼ������
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
const char CHART_HIGHLOW[]            = "HIGHLOW";//�ߵ�ͼ
const char CHART_PP[]                 = "PP";
const char CHART_QQ[]                 = "QQ";
const char CHART_EMP[]                = "EMP";//�����ۻ�ͼ�;����ܶ�ͼ
//����
const char TYPE_STR[]     = "/TYPE";
//������
const char SIMPLELINE_STR[]   = "SIMPLELINE";
const char MULTIPLELINE_STR[] = "MULTILINE";
//��״ͼ
const char SIMPLEBAR_STR[]    = "SIMPLEBAR";
const char GROUPBAR_STR[]     = "GROUPBAR";
const char SEGMENTBAR_STR[]   = "SEGMENTBAR";
//ɢ��ͼ
const char SIMPLESCATTER_STR[]= "SIMPLESCATTER";
const char SUPERSCATTER_STR[] = "SUPERSCATTER";
//ģʽ
const char MODE_STR[]     = "/MODE";
//����ģʽ��
const char CLASS_STR[]    = "CLASS";
const char VARIABLE_STR[] = "VAR";
const char VALUE_STR[]    = "VALUE";
//������
const char AXISDESC_STR[]  = "/AXISDESC";
const char AXISNDESC_STR[] = "/AXISNDESC"; 
const char VARDESC_STR[]   = "/VARDESC";
const char VARNDESC_STR[]  = "/VARNDESC";
//��������
const char DTA_STR[]  = "/DTA";
const char DTV_STR[]  = "/DTV";
//Ȩ��
const char WEIGHT_STR[] = "/WEIGHT";
//��ǩ
const char OUTPUT_STR[] = "/OUTPUT";
const char LABEL_STR[]  = "LABEL";
//������
const char REC_STR[]           = "REC";
const char RECSCALE_STR[]      = "RECSCALE";
const char RECTOTAL_STR[]      = "RECTOTAL";
const char RECTOTALSCALE_STR[] = "RECTOTALSCALE";//֮ǰ��Ϊ�����֮࣬���Ϊ������
const char MEAN_STR[]          = "MEAN";
const char VAR_STR[]		   = "VAR";
const char MOD_STR[]		   = "MOD";
const char MID_STR[]		   = "MID";
const char STD_STR[]		   = "STD";
const char MIN_STR[]		   = "MIN";
const char MAX_STR[]		   = "MAX";
const char SUM_STR[]		   = "SUM";
//�Զ�����
const char DEFLINE_STR[]       = "/DEFLINE";
//ֱ��ͼ
const char DIV_STR[]           = "/DIV";
const char PLOT_STR[]          = "/PLOT";
const char NORMAL_STR[]        = "NORMAL";
const char MAXNUM_STR[]        = "/MAXNUM";
const int  DIV_MIN_INT         = 6;
const int  DIV_MAX_INT         = 100;
//�����ͼ-�����ͼ
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
//�����ۻ�ͼ�;����ܶ�ͼ
const char EMP_EXP_STR[]       = "EXP";
const char EMP_DEN_STR[]       = "DEN";
//��״ͼ
const char SPLIT_STR[]         = "SPLIT";
//��Ϣ��ʾ
const char MSG_TITLE_STR[]     = "��ʾ��Ϣ";
//�ἰ��������
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
		SIMPLELINE,   //��ֱ��
			MULTIPLELINE, //��ֱ��
			SIMPLEBAR,    //����״
			GROUPBAR,     //������״
			SEGMENTBAR,   //�ֶ���״
			SIMPLESCATTER,//��ɢ��ͼ
			SUPERSCATTER  //�غ�ɢ��ͼ
	};
	enum EModel
	{
		CLASS,   //��������
			VARI,    //����
			VALUE    //����ֵ
	};
	enum EData
	{
		REC,           //��¼��  ��X���������Ӧ�ļ�¼��(һ������)
			RECSCALE,      //��¼����
			RECTOTAL,      //��¼�ۼƣ�X���������Ӧ�ĺ�(һ������)
			RECTOTALSCALE, //��¼�ۼƱ���
			MEAN,          //������ֵ
			VAR,           //��������
			MOD,           //��������
			MID,           //������λ��
			STD,           //������׼��
			MIN,           //������Сֵ
			MAX,           //�������ֵ
			RECORD,        //������¼������ͬX�£�Y���Ӧ�ı�����¼��(��������)
			SUM            //������    ����ͬX�£�Y���Ӧ�ı�����(��������)
	};
	EType  m_eType; //������
	EModel m_eMode; //����ģʽ
	//EData  m_eDataX;//X����������
	//EData  m_eDataY;//Y����������
	//EData  m_eDataD;//�Զ�����������
	CTArray <EData, EData> m_arrX;//X������Ӧ����������
	CTArray <EData, EData> m_arrY;//Y������Ӧ����������
	CTArray <EData, EData> m_arrD;//�Զ�������Ӧ����������
protected:
	bool IsSameCallFunc(int nType); //�ж�ĳ�����Ƿ�����ͬ����������
	bool GetAxisLabel(EData eData, CTString &szLabel);
	bool GetStatistical(CStatistics statistics, EData eData, int nRowCnt, double &fValue);
	bool GetDataType(int nIndex, int nType, EData &eData);//Type:0-X,1-Y,2-D;
	CTString GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue);
protected://�����㷨
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
	int  m_nAVType;     //�ἰ��������
	int  m_nWeight;     //Ȩ������
	bool m_bWeight;     //Ȩ�ر�־
	bool m_bShowLabel;  //������ʾ��ǩ
	bool m_bSplit;      //���������һ��ʱ���Ƿ�ֿ���������״ͼʱ
	TIntStruct m_tVarX; //X�ᣭ��������
	TIntStruct m_tVarY; //Y�ᣭ��������
	TIntStruct m_tVarD; //�Զ�����
	CTString   m_szErrMsg;   //������Ϣ
	CTString   m_szChartName;//ͼ����
	CTString   m_szChartType;//ͼ��������
	CDataInterface *m_pDataInterface;
protected:
	CResult *m_pResult;   //�����
};

#endif // !defined(AFX_CHART_H__4BEEF0D6_5178_468C_A631_A245F10C8B0C__INCLUDED_)
