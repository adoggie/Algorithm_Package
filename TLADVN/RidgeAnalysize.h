// RidgeAnalysize.h: interface for the CRidgeAnalysize class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIDGEANALYSIZE_H__D4F7E9F0_470C_4F34_97A8_7E1760DAFC1E__INCLUDED_)
#define AFX_RIDGEANALYSIZE_H__D4F7E9F0_470C_4F34_97A8_7E1760DAFC1E__INCLUDED_
#include "AlgorithmBase.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "RidgeDlg.h"
//��ع��ģ������ṹ��
struct TRidgeResult 
{
public:
	//��ʼ��
	TRidgeResult()
	{
			m_bWithConst = true;
//			m_strEqation = "";
			m_dkValue=0.5;
			m_nkMethod=4;

	}
	//���Ʋ���(=����)
	TRidgeResult& operator =(TRidgeResult &RidgeModelResult)
	{
		m_bWithConst=RidgeModelResult.m_bWithConst;
		m_vectBeta=RidgeModelResult.m_vectBeta;
		m_dkValue=RidgeModelResult.m_dkValue;
		m_dRSS=RidgeModelResult.m_dRSS;
		m_dESS=RidgeModelResult.m_dESS;
		m_Matrix_R=RidgeModelResult.m_Matrix_R;
		m_nkMethod=RidgeModelResult.m_nkMethod;
		m_strVarlist.Copy(RidgeModelResult.m_strVarlist);
	}
	//����ڴ�
	void clear()
	{
		m_strVarlist.RemoveAll();
	}
	//�ṹ��Ա����
public:
	CDoubleVector	m_vectBeta;//�ع�ϵ���������������m_vectBeta(0)Ϊ�����
// 	CString			m_strRegEquation; //�ع鷽��
	CTStringArray	m_strVarlist;//�������б���һ��Ϊ�������2��Varlist.GetLength()-1Ϊ�Ա���
	BOOL			m_bWithConst;//�Ƿ��������0- �� ��1����ʾ��������
// 	BOOL			m_bWithRidgeTrace;//�Ƿ��뼣����0- �� ��1����ʾ�뼣����ͼ
	double			m_dkValue; //�����k����0
	double			m_dRSS;//�ع�ƽ���ͣ���0
	double			m_dESS;//�в�ƽ���ͣ���0
	CDoubleMatrix   m_Matrix_R;//�Ա������ϵ������
	int				m_nkMethod;//kȡֵ����0-4�����ĵ�
	
};
class CRidgeDlg;
class CRidgeAnalysize : public CAlgorithmBase  
{
public:
	CRidgeAnalysize();
	virtual ~CRidgeAnalysize();
	bool			m_bOutputModel;//�Ƿ����ģ�ͣ�0-�������1�������Ĭ��Ϊ0;	
	//*****************************ע�⣺�˱�����ͬStruct RidgeResultֻ�����㷨������ʱ�ſ�����

	CResult *		OnRidgeAnalysize(CDataInterface *pDataInterface, string VO);//��ع�
	TRidgeResult*	m_pRidgeModelResult;//��ع��ģ������ṹ��

private:
	int				m_nXRow;//��Ч��������
	int				m_nXCol;//�Ա�������
	CRidgeDlg *		m_pDlg;//�Ի���
	CResult *		m_pResult;//���
	BOOL			m_bWithConst;//�Ƿ��������
	BOOL			m_bWithRidgeTrace;//�Ƿ��뼣����
	CDoubleMatrix	m_MatCoeff;//�ع�ϵ��
	double			m_dINTERCEPT;//������ؾ�
	double			m_dkValue; //�����k����0
	double			m_dRSS;//�ع�ƽ���ͣ���0
	double			m_dESS;//�в�ƽ���ͣ���0
	CDoubleMatrix   m_Matrix_R;//�Ա������ϵ������
 	int				m_nkMethod;//kȡֵ����0-4
	int				m_nMissingMethod;//ȱʧֵ������
	double			m_kMax;//�뼣������kֵ
private:
	BOOL			GetValueFromVo(CDataInterface * pDataInterface, string VO);
	CDoubleMatrix	EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex);//����ֵ���������������������������������		
	void			CreateResultTrace(CDoubleMatrix m_EigValue, CDoubleMatrix m_EigVetor,CDoubleMatrix Sy, double kMax);//�뼣�������
    void			CreateResult();//���������
	double			OptiFastDownward(CDoubleMatrix m_EigValue,CDoubleMatrix Sy,double sigma,double xInit);//�����½�����Ŀ�꺯�������Ž⣬������չ
	double			H(double k, CDoubleMatrix Sy,CDoubleMatrix lambda,double sigma);//������k������ֻ�����������
	double          HDeri(double k, CDoubleMatrix Sy,CDoubleMatrix lambda,double sigma);//������H(k)�ĵ�����ֻ�����������
 	void			GetMatrixData(CDoubleMatrix* Matrix_X, CDoubleMatrix* Matrix_y);//�����ݽӿ���ȡ��������Matrix_X�Ա�����Matrix_y�Ա���
	double			GetSigma(CDoubleMatrix* Matrix_X,CDoubleMatrix* Matrix_y,CDoubleMatrix Sy, CDoubleMatrix EigValue, CDoubleMatrix EigVector);
//	double			GetkValue(CDoubleMatrix Sy,CDoubleMatrix EigValue, CDoubleMatrix EigVector, int m_nkMethod);
protected:
	CDataInterface*	m_pDataInterface;
	CTStringArray	m_strVarlist;//����������һ��Ϊ�������2��Varlist.GetLength()-1Ϊ�Ա���	
	TIntStruct		m_tVarInfo;//�Ա���struct
	TIntStruct		m_tVarInfo_D;//�����struct
	
};

#endif // !defined(AFX_RIDGEANALYSIZE_H__D4F7E9F0_470C_4F34_97A8_7E1760DAFC1E__INCLUDED_)
