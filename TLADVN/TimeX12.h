// TimeX12.h: interface for the CTimeX12 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEX12_H__3D80476B_675F_4757_A883_D1A27A40B741__INCLUDED_)
#define AFX_TIMEX12_H__3D80476B_675F_4757_A883_D1A27A40B741__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"
#include "Arima.h"

class CTimeX12 : public CAlgorithmBase  
{
public:
	CTimeX12();
	virtual ~CTimeX12();
	CResult * OnTimeX12(CDataInterface *pDataInterface, string VO); //�ⲿ�������㷨

private:
	BOOL    GetValueFromVo(CDataInterface *pDataInterface, string VO);
	void	WeekCalcu(int nYear, int nMonth);//��������ÿ�����ж����죬�ж��ٸ���һ�ܶ�����������m_DaysinMonth_vec��m_WeekDescrib_mx
	int		DaysInMonth(int nYear, int nMonth);//ÿ�����ж�����
	void    IndependentVariables(int nYear, int nMonth);//�����Ա���
	void    ChineseNewYear(int nYear);//ÿ�괺�ڵ�λ��
 	void    IdentifiyOutliers(CDoubleMatrix variables_mx);  //�Զ�ʶ���쳣ֵ
	void    PreAdjust(CDoubleVector OrgVec);
	void    Predict(CDoubleVector OrgVec, int mp, int mq, int md, int length);  //Ԥ��
	BOOL    Nonx11(CDoubleVector Org_vec);
	CDoubleVector   SeasonMove(CDoubleVector SI_vec, int method);   
	CDoubleVector TrendMove(CDoubleVector SmoothedCT, int nMethod);
	void    WeightCalcu(int nSpan);
	int     TrendMethod(CDoubleVector TCI_vec);
	void    DrawResult(); //������
	void    Diagnoses();


private://�û����ò���
	int m_nWeekMethod;	    //������Ӱ�� 0-��Ӱ�죬1-����Ӱ�죬2-����������Ӱ��
	int m_nHoliday;			//�ڼ���Ӱ�� 0-��Ӱ�죬1-�нڼ���  2-�д���Ӱ��
	int m_nYear;			//��ʼʱ�䣬��
	int m_nMonth;			//��ʼ ��
	int m_nMethod;			//�ӷ����ǳ˷���0���˷�  1���ӷ�
	TIntStruct m_tVarInfo;	//�����±�
	int m_nAO;				//�û������AO�ĸ���
	CIntMatrix m_AOMx;		//�û������AO��λ�þ���
	int m_nLS;				//�û������LS�ĸ���
	CIntMatrix m_LSMx;		//�û������LS��λ�þ���
	double m_threshold;		//����ֵ��ֵ
	int m_AONumber;
	int m_LSNumber;
	int m_ndifference;        //��ֽ���
    BOOL m_bpredict;         //�Ƿ����Ԥ����
	int m_prestep;           //Ԥ�ⲽ��
    

	//��̷��㶨��ı���
	CResult *m_pResult;
	CDataInterface *m_pDataInterface;
	CRsltElementText *m_pRsltElementText;
	CRsltElementText * m_pWarningTextRslt;//������Ϣ
    CRsltVariable * m_pRsltVariable;
	CDoubleVector	m_OrgVec; //X12����
	CDoubleVector   m_extendOrgVec; //ARIMA�������������
	CDoubleMatrix  m_WeekDescrib_mx;   //ÿ������һ�ܶ�����
	CDoubleVector  m_DaysinMonth_vec;  //ÿ���ж�����
	CDoubleMatrix  m_ForecastWeekDescrib_mx;  //��ǰԤ������ڽṹ
	CDoubleMatrix  m_BackcastWeekDescrib_mx;  //���Ԥ������ڽṹ
	CDoubleVector  m_CasetDaysinMonth_vec;    //Ԥ������ÿ���ж�����
	CIntVector     m_PreOutliers_mx;   //ָ���쳣ֵ�����λ��
	CDoubleMatrix  m_constant_mx;         //������
	CDoubleMatrix  m_trend_mx;        //���Ʊ���
	CDoubleMatrix  m_leapYear_mx;     //�������
	CDoubleMatrix  m_Season_mx;       //�������ر���
	CDoubleMatrix  m_AO_mx;           //ָ��AO����
	CDoubleMatrix  m_LS_mx;           //ָ��LS����
	CDoubleMatrix  m_workday_mx;       //�����սṹ����(������|�ǹ�����)
	CDoubleMatrix  m_NewYearID_mx;    //ÿ�괺������λ��
	CDoubleMatrix  m_ExtendNewYear_mx;  //��������λ��
    CDoubleMatrix  m_NewYear_mx;      //���ڱ���
    CDoubleMatrix  m_Holiday_mx;      //�ڼ��ձ���
	CDoubleMatrix  m_allvariables_mx;	// �����Ա���������|���꣨m_leapYear_vec��|�������أ�m_Season_mx��|ָ���쳣ֵ����|�����գ���/���ڽṹ��m_weekDescrib_mx��/�����գ�m_workday_mx����|���ڣ���/m_NewYear_mx��|�ڼ��գ���/m_Holiday_mx(����|Ԫ��|��һ|ʮһ)����
	CDoubleMatrix  m_Forecastvariables_mx;   //��ǰ����ı���
    CDoubleMatrix  m_Backcastvariables_mx;   //�������ı���
	CDoubleVector  m_allBeta_vec;	//�Ա���ϵ��
	CDoubleVector  m_AdjustVec;     //Ԥ�����������
	
	CDoubleVector m_CT_SmoothedVec;
	CDoubleVector m_I_FinalVec;
	CDoubleVector m_S_SmoothedVec;
	CDoubleVector m_Dr_FinalVec;
	CDoubleVector m_Beta_Vec;
	CDoubleMatrix m_Coff_mx;
	//
	CDoubleVector  m_TrendVec;
	CDoubleVector  m_SeasonVec;
	CDoubleVector  m_IrregularVec;
	CDoubleVector m_TCI_Vec;
	CDoubleVector  m_Weight_Vec;   
	CDoubleVector  m_RatioOrg_Vec;
	CDoubleVector  m_RatioTCI_Vec;

	//
	CDoubleVector m_AveOrg_Vec;
	CDoubleVector m_AveTCI_Vec;
	CDoubleVector m_AveTrend_Vec;
	CDoubleVector m_AveSeason_Vec;
	CDoubleVector m_AveIrregular_Vec;
    CDoubleVector m_predict_Vec;



};

#endif // !defined(AFX_TIMEX12_H__3D80476B_675F_4757_A883_D1A27A40B741__INCLUDED_)
