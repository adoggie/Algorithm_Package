// Arima.h: interface for the CArima class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARIMA_H__7A619F23_CEF1_4028_AB05_EAD8F0458BD0__INCLUDED_)
#define AFX_ARIMA_H__7A619F23_CEF1_4028_AB05_EAD8F0458BD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AlgorithmBase.h"

class CArima : public CAlgorithmBase  
{
public:
	CArima();
	virtual ~CArima();
	CResult * OnArima(CDataInterface *pDataInterface, string VO); //�ⲿ�������㷨
	CDoubleVector ArimaPred(CDoubleVector &OrgVec, int pAR, int qMA, int dIFF, int PredStep, BOOL m_bWithConst);//��һ��ԭʼ���������з�����Ԥ��


private:
	int Factorial(int number);   //�׳˺���
	double Getcni(int n,int i);   ////�������
	BOOL GetValueFromVO(CDataInterface *pDataInterface, string VO);
	void DrawResult();
	CDoubleVector Predict(CDoubleVector &Xmin_Vec, CDoubleVector &Z_Vec, int d, BOOL  m_bWithConst);///��Ԥ��ת������
	CDoubleVector Getpredict(CDoubleVector &Org_Vec, CDoubleVector &CofficVec, int p, int q,int L, BOOL m_bWithConst);//��в��Ԥ��
    CDoubleVector GetInverseFunc(CDoubleVector Vec_1, CDoubleVector Vec_2, int numW); //���溯��
	void Powell(CDoubleVector &Data_Vec, CDoubleMatrix &Direction_mx, int &iter, double &FRET);  //��Ԫ������ά������
	void Linmin(CDoubleVector &Data_Vec, CDoubleVector &Direction_Vec, double &FRET); //�����ӳ�ʼ����ָ������ļ�Сֵ��
	void Mnbank(double &ax, double &xx, double &bx, double fa, double fx, double fb, CDoubleVector Data_Vec, CDoubleVector Direction_Vec); //�ûƽ�ָȷ����С�����ڵ�����
	double Brent(double ax, double xx, double bx, double &Xmin, CDoubleVector Data_Vec, CDoubleVector Direction_Vec); //������һά�Ż�
	double FuncX(CDoubleVector VecX);//����ֵ
	void Erase(CDoubleVector &Direction_Vec, CDoubleVector &PTT_Vec, CDoubleVector &Data_Vec); //��ԭ������������
	void GetArimaStatis(CDoubleVector diffvec, CDoubleVector ep_Vector, CDoubleVector CofficVec);//////���ͳ����
	void GetPreStatis(/*CDoubleVector pre_Vec*/ double stderor/*, double alphavalue*/);  ///���Ԥ��ͳ����
    BOOL GetARbeta(int numP, int numQ); //�Իع�ϵ������

	//�û�����Ĳ��� ʼ
	TIntStruct m_tVarInfo;						//��������±꼯
	int m_nRow;                                 //�����������
	int m_pAR;                                  //����ģ���Իع����
	int m_qMA;                                  //����ģ�ͻ���ƽ������
	int m_dIFF;                                 //�����ֽ���
	int m_nPredStep;                            //����Ԥ�ⲽ��
	BOOL m_bWithConst;                          //�������޳�����
	
	//�û�����Ĳ��� ֹ


	//��̷������õı��� ʼ	
	CDataInterface * m_pDataInterface;
	CResult * m_pResult;
	CRsltElementText * m_pWarningTextRslt;//������Ϣ
	CDoubleVector m_AutoCovF_Vec;   //��Э�����
	CDoubleVector m_AutoCorrF_Vec;  //����غ���
	double m_VecRow;
	CDoubleVector m_diff_Vec;  //��ֺ������
	CDoubleVector m_ARCoefficient_Vec;
	CDoubleVector m_ARvariance_Vec;
	CDoubleVector m_Residual_Vec;
    CDoubleVector m_ep_Vector;  //�в�����
	CDoubleVector m_Coef_vec;
    double m_stderror;
	double m_dw;
	double m_Rsquare;
	double m_adjustedRSquare;
	double m_AIC;
	double m_SC;
	double m_loglikeli;
	double m_SSQ;
	CDoubleVector m_stderror_Vec;
	CDoubleVector m_tvalue_Vec;
	CDoubleVector m_pvalue_Vec;
	CDoubleVector m_predict_Vec;
	CDoubleVector m_orginal_Vec;

	//��̷������õı��� ֹ

};

#endif // !defined(AFX_ARIMA_H__7A619F23_CEF1_4028_AB05_EAD8F0458BD0__INCLUDED_)
