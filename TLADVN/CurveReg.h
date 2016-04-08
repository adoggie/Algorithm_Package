// CurveReg.h: interface for the CCurveReg class.
//                   ���߻ع���� 
//                   ���ܣ�����Э��������Ĺ�ͬ����������
//					 ��ע��  ������																																																																																																																																																																																					
//					 ʱ�䣺2005��10��20��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURVEREG_H__51B7B4AF_1459_43A4_AC51_DFE598250FC5__INCLUDED_)
#define AFX_CURVEREG_H__51B7B4AF_1459_43A4_AC51_DFE598250FC5__INCLUDED_
#include "AlgorithmBase.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class AFX_EXT_CLASS CCurveReg : public CAlgorithmBase  
{
public:
	int m_nDependIndex;//�Ա����±�
	CResult * OnCurveReg(CDataInterface *pDataInterface, string VO);//�ű�����
	CCurveReg();
	virtual ~CCurveReg();
	TIntStruct m_tVarInfo;
private:
	CResult * m_pResult;
	CDataInterface * m_pDataInterface;
private:
	CRsltElementText * m_pResultVariable;
	int m_nCurVarIndex;
	CTString m_strModel;
	CTString m_strHead;
	double m_u;//logistics
	CIntVector m_VectAvail;
	
public:
	CTString m_strUpper;  //zfm

	CTLTable* briefrTable;
	CTString strModel;
private:
	//	��˼�࣬01/13/03
	bool m_bIsWeighted;//�Ƿ�����ݽ����˼�Ȩ
	double* m_pdWeights;

	void ReportError();
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	void CreateResult(CTString strHead,double r,double rs,double ars,double se,int df1,int df2,double ss1,double ss2,
							 double ms1,double ms2,double b1,double b0,double sb1,double sb0,double sbeta,double t1,
							 double t0 ,double F,bool bLogistic=false); //zfm ���bool�ֶ����ǲ��������߼�˹�ٿ˻ع�
	void CreateResult(CTString m_strHead, double R, double R_S, double A_R, double SER, int DF_Rg,double DF_Rs,
		double U, double Q, double M_S_Rg, double M_S_Rs, double F, 
		CDoubleMatrix BetaMatrix, CDoubleVector serrbeta, CDoubleVector Beta, CDoubleVector T); //zfm


//2003.09.23 zkc
	bool m_flag;
	CTStringArray m_Legend;
	CPtrArray m_dataPtr;

	void FillChartData(int nLen,double pDataX[],double pDataY[],int nMethod,CTString strLegend,
					   double b0,double b1,double b2=0,double b3=0);
	void FillChartData2(int nLen,CDoubleVector Vect_X,CDoubleVector Vect_Y,int nMethod,CTString strLegend,
		double b0,double b1,double b2=0,double b3=0);
		
	void CreateChartResult(double pDataX[],CTString strField,CResult* pResult,int nLen);
	void sort(double x[],double y[],double w[],int nLen);


//	01/13/02����˼�࣮��������Ȩ�أ����Ӵ�Ȩ�ز��������غ���

	void Quad_Cubic(int n, int p, double *x, double *y,double *w);//��Ȩ�� double * MissingValue)
	double* GetDataSingle(CDataInterface* pDataInterface, int nIndex, double * MissingValue);
	double* GetDataSingle(CDataInterface* pDataInterface, int nIndex, CIntVector& Vect_Avail);
	void Linear(int n, double x[], double y[], double w[], double a[]);
	void Exponential(int n, double x[], double y[], double t, double w[], double a[]);
	void Power(int n, double x[], double y[], double w[], double a[]);
	void Compound(int n, double x[], double y[], double w[], double a[]);
	void Growth(int n, double x[], double y[], double w[], double a[]);
	void S(int n, double x[], double y[], double w[], double a[]);
	void Inverse(int n, double x[], double y[], double w[], double a[]);
	void Logistic(int n, double x[], double y[], double w[], double a[], double &u);
	void Logarithmic(int n, double x[], double y[], double w[], double a[]);

	TIntStruct m_IndependIndex;//������±�
	double m_dUpper;//logistic �Ͻ�
	bool	m_bCompound;//������������ 
	bool	m_bExponential;//ָ������
	bool	m_bLogistic;//�߼�˹�ٿ�����
	bool	m_bGrowth;//��������
	bool	m_bInverse;//������
	bool	m_bLineare;  //���Իع�
	bool	m_bLogarithmic;//��������
	bool	m_bPower;   //��������
	bool	m_bS;//S����
	bool    m_bQuad;//��������
	bool    m_bCubic;//��������
	
};

#endif // !defined(AFX_CURVEREG_H__51B7B4AF_1459_43A4_AC51_DFE598250FC5__INCLUDED_)
