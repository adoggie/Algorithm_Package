// Simuequation.h: interface for the CSimuequation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMUEQUATION_H__D77A8B1F_5DCC_40D9_BC11_F5D30BB745D9__INCLUDED_)
#define AFX_SIMUEQUATION_H__D77A8B1F_5DCC_40D9_BC11_F5D30BB745D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CSimuequation : public CAlgorithmBase  
{
public:
	CSimuequation();
	virtual ~CSimuequation();
	CResult * OnSimEqu(CDataInterface *pDataInterface, string VO);//�ⲿ������Ҫ����
	TIntStruct		m_tVarInfo;//�ع����struct
	TIntStruct		m_tVarInfo_D;//��������struct	
	
private:
	BOOL m_bWarning;
	int m_SEmethod;						//����ģ�͵Ĺ��Ʒ���
	int m_SEqnumber;					//ģ�͵Ľ��ͷ��̵ĸ���
 	CDoubleMatrix m_mxX;
 	CDoubleMatrix m_mxY;
 	CDoubleMatrix m_mxP;				//���ͷ��̵���Ϣ����
 	CDoubleMatrix m_mxINP;
	CTStringArray	m_strDepend;        //�������������ַ���
	CTStringArray	m_strVarSel;        //�Ⱦ����������ַ���
	CTStringArray	m_szArrSE;
	CTStringArray	m_ListVarArr;		//P����ı����İ�˳������
	CTStringArray	m_coef_name;		//ϵ����c(0)��
	CTStringArray	m_coef_var;			//ϵ������Ӧ�ı�����
	CDoubleVector	m_vrG;				//�洢ÿ�������Ҳ��к��ж��ٸ���������
	CDoubleVector	m_vrK;				//�洢ÿ�������Ҳ��к��ж��ٸ��Ⱦ�����
	double m_dCoeffMax;					//�洢�������ÿ������������ж��ٸ�ϵ��
	double m_dCoffTotal;				//�洢��������ܹ��ж��ٵ�ϵ��
	CDoubleMatrix mxPLag;				//�ͺ�������ָʾ����
	CDoubleMatrix	m_lagMatrix;		//�ͺ����
	CIntVector		m_ivLagNum;			//�ͺ�������ͺ�Ľ���
	CDoubleVector   m_vrLagVarNum;		//ÿ�����̵��ͺ�����ĸ���
	double LagMaxTotal ;						//����ͺ����
	
	struct  VO								//������������ṹ
	{
		CDoubleVector m_vrTest;				//����ͳ����
		double  m_dTdf;						//������ٽ�ֵ
		
		CDoubleMatrix m_mxBeta;				//�ع�ϵ������
		CDoubleMatrix m_mxSEReg;			//�ع�ϵ���ı�׼��
		CDoubleMatrix m_mxTvalue;			//�ع�ϵ����Tֵ
		CDoubleMatrix m_mxPvalue;			//�ع�ϵ����Pֵ
		
		CDoubleVector m_vrSSE;				//�����ع鷽�̵Ĳв�ƽ����
		CDoubleVector m_vrSST;				//�����ع鷽�̵Ļع�ƽ����
		CDoubleVector m_vrRsqu;				//�����ع鷽�̵ĸ����ϵ��
		CDoubleVector m_vrAdjR;				//�����ع鷽�̵����������ϵ��
		CDoubleVector m_vrSSR;				//�����ع鷽�̵Ļع�ƽ����
		CDoubleVector m_vrSER;				//�����ع鷽�̵Ļع��׼��
		CDoubleVector m_vrDw;				//�����ع鷽�̵�DWͳ����
		CDoubleVector m_vrMD;				//�����ع鷽�̵��������׼��
		CDoubleVector m_vrSDD;				//�����ع鷽�̵��������ֵ

	} m_tOut;

	//������
	BOOL SimEqu(CDoubleMatrix &mxX,		//������������
			CDoubleMatrix &mxY,			//�Ⱦ���������
			CDoubleMatrix &mxP,			//ģ����������
			CDoubleMatrix &mxINP);		//Լ����������
	BOOL GetValueFromVo(CDataInterface* pDataInterface, string VO);
	BOOL GetData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y);//��ȡ����
	void DrawResult();					//������չʾ					


	BOOL IV(CDoubleMatrix &mxX,			//������������
		CDoubleMatrix &mxY,			//�Ⱦ���������
		CDoubleMatrix &mxP,			//ģ����������
		CDoubleMatrix &mxINP);		//Լ������
	BOOL ILS(CDoubleMatrix &mxX,			//������������
		CDoubleMatrix &mxY,			//�Ⱦ���������
		CDoubleMatrix &mxP,			//ģ����������
		CDoubleMatrix &mxINP);		//Լ������
	BOOL TwoLS(CDoubleMatrix &mxX,		//������������
		CDoubleMatrix &mxY,			//�Ⱦ���������
		CDoubleMatrix &mxP,			//ģ����������
		CDoubleMatrix &mxINP);		//Լ������
	BOOL ThreeLS(CDoubleMatrix &mxX,		//������������
		CDoubleMatrix &mxY,			//�Ⱦ���������
		CDoubleMatrix &mxP,			//ģ����������
		CDoubleMatrix &mxINP,		//Լ������
		CDoubleMatrix &mxBeta_3SLS);//������ϵ������
	BOOL FIML(CDoubleMatrix &mxX,		//������������
		CDoubleMatrix &mxY,			//�Ⱦ���������
		CDoubleMatrix &mxP,			//ģ����������
		CDoubleMatrix &mxINP);		//Լ������
	BOOL MODELTEST(int i,				//��i������
		CDoubleMatrix &mxP,			//ģ�͵���������
		CDoubleMatrix &mxINP,		//Լ����������
		int g0,						//ģ�����ܵ�������������
		int k0);					//ģ�����ܵ��Ⱦ���������

	//�������麯��
	BOOL Paramtest(int i,			//��i������
			CDoubleMatrix &mxYest,	//��i�������ұߵ�����������Ԥ��ֵ
			CDoubleVector &vrYi,	//��i�������������������
			CDoubleMatrix &mxY0,	//��i�������ұߵ�����������ԭʼֵ
			CDoubleMatrix &mxXi,	//��i������������Ⱦ�������ֵ
			CDoubleMatrix &mxBi,	//���Ƶĵ�i�����̵�ϵ��������������ϵ����
			CDoubleMatrix &mxGamai,	//���Ƶĵ�i�����̵�ϵ�����Ⱦ�������ϵ����
			int nn,int gi,int ki);	//�ֱ������i�����̵Ĺ۲�ֵ���������������ĸ������Ⱦ������ĸ���
	CDoubleMatrix Kproduct(CDoubleMatrix & mxX,CDoubleMatrix &mxY);//�����;���Ĳ��

	//����Nelder��Meade�㷨�������ŵĽ�
	CDoubleVector Nelder(CDoubleMatrix &mxSTar,int nN,double n1,double neldtol,double n2,CDoubleMatrix &mxXmat,CDoubleMatrix &mxYmat);
	
	//����������Ȼ������ֵ
	double logfiml(CDoubleVector &vrBetai, CDoubleMatrix &mxYmat,CDoubleMatrix &mxXmat);

	BOOL ReadStrs(CTStringArray& equs);//�������з����Ƿ����
	BOOL legstr(CTString eqution);//����һ�䷽���Ƿ����
	BOOL inselectinlist(CTString strTemp0);//�ж��ַ����Ƿ������������ͬ
	BOOL inselectlist(CTString strTemp0);//�ж��ַ����Ƿ��ѡ�������ͬ
	BOOL strIsANum(CTString strTemp0);//�ж��ַ����Ƿ�Ϊ����
	BOOL strIsADouble(CTString strTemp0);//�ж��Ƿ�Ϊ������
	BOOL ReadStrToMx();//���ַ�������P����
	BOOL ReadLineToMx(CTString strTemp, int i);//���ַ����������ĵ�i��
	BOOL ReadLineToINPMx(CTString senTemp, int i);//����i����INP����
	BOOL ReadStrToINPMx();//����INP����
	CTStringArray 	m_StrVarOrd;
	CTStringArray	m_str_fun;
	CTStringArray	m_CID_fun;
	CTStringArray	m_LimitArrSE;
	BOOL m_bBeginwithC;//�ַ�����һ����c��ͷ�Ļ������ֿ�ͷ�ģ����ھ����Ƿ�ΪԼ������
};

#endif // !defined(AFX_SIMUEQUATION_H__3A3E1DC3_F18E_40D1_B129_3DD17F9B856D__INCLUDED_)
