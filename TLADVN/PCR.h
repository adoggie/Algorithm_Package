// PCR.h: interface for the CPrinCompReg class.
//							���ɷֻع�
//              gezehui
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCR_H__36F24C0F_127E_45DD_B569_969506E6006D__INCLUDED_)
#define AFX_PCR_H__36F24C0F_127E_45DD_B569_969506E6006D__INCLUDED_

#include "AlgorithmBase.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct TPrinCompRegResult 
{
public:
	//��ʼ��
	TPrinCompRegResult()
	{
		m_bWithConst = true;
		//			m_strEqation = "";
		m_ncMethod=				0;
		m_ncValue =			2;	
		
	}
	//���ƽṹ
	TPrinCompRegResult& operator =(TPrinCompRegResult &PrinCompRegResult)
	{
		m_bWithConst=PrinCompRegResult.m_bWithConst;
		m_vectBeta=PrinCompRegResult.m_vectBeta;
		m_ncValue=PrinCompRegResult.m_ncValue;
		m_dRSS=PrinCompRegResult.m_dRSS;
		m_dESS=PrinCompRegResult.m_dESS;
		m_Matrix_R=PrinCompRegResult.m_Matrix_R;
		m_ncMethod=PrinCompRegResult.m_ncMethod;
		m_matEigVal=PrinCompRegResult.m_matEigVal;
		m_matEigVect=PrinCompRegResult.m_matEigVect;
		m_strVarlist.Copy(PrinCompRegResult.m_strVarlist);
	}
	//�ͷ��ڴ�
	void clear()
	{
		m_strVarlist.RemoveAll();
	}
	
public:
	CTStringArray	m_strVarlist;//�������б���һ��Ϊ�������2��Varlist.GetLength()-1Ϊ�Ա���
	CDoubleVector	m_vectBeta;//�ع�ϵ���������������m_vectBeta(0)Ϊ�����
	BOOL			m_bWithConst;//�Ƿ�������0- �� ��1����ʾ��������
	CDoubleMatrix   m_matEigVal;//����ֵ���Ӵ�С����
	CDoubleMatrix	m_matEigVect;//��������ÿ�ж�Ӧһ������ֵ�����дӴ�С��
	int 			m_ncValue; //���ɷ��������㵽m_nXCol֮��
	double			m_dRSS;//�ع�ƽ���ͣ���0
	double			m_dESS;//�в�ƽ���ͣ���0
	CDoubleMatrix   m_Matrix_R;//�Ա������ϵ������
	int				m_ncMethod;//cȡֵ����0-3
	// 	CString			m_strRegEquation; //�ع鷽��
};

class CPCRDlg;
//Principal Component Regression ���ɷֻع�
class CPrinCompReg : public CAlgorithmBase
{
public:
	CPrinCompReg();
	virtual ~CPrinCompReg();
	CResult	*OnPCR(CDataInterface *pDataInterface, string VO);//�������ӿ�
	BOOL    m_bOutputModel;
	TPrinCompRegResult* m_pPCRResult;
private:
	int				m_nXRow;//��Ч����������ȱʧֵ����֮��
	int				m_nXCol;//�Ա�������
	CPCRDlg *		m_pDlg;//�Ի���
	CResult *		m_pResult;//���
	BOOL			m_bWithConst;//�Ƿ��������
	BOOL			m_bWithPC;//�Ƿ���ʾ���ᣨ����������
	CDoubleMatrix	m_MatCoeff;//�ع�ϵ��
	double			m_dINTERCEPT;//������ؾ�
	CDoubleMatrix   m_EigValue;//����ֵ
	CDoubleMatrix	m_EigVector;//��������
	int 			m_ncValue; //���ɷ��������㵽m_nXCol֮��
	double			m_dRSS;//�ع�ƽ����
	double			m_dESS;//�в�ƽ����
	CDoubleMatrix   m_Matrix_R;//�Ա������ϵ������
	int				m_ncMethod;//cȡֵ����
	int				m_nMissingMethod;//ȱʧֵ��������0���ֵ��1����ɾ��
	double m_dcValueCalContr;//�������
	double m_dcValueCorrEigVal;//�������
private:
// 	int				GetCurSec();
	BOOL			GetValueFromVo(CDataInterface *pDataInterface, string VO);
	CDoubleMatrix	EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex);
	void			CreateResult();
 	void			GetMatrixData(CDoubleMatrix* Matrix_X,CDoubleMatrix* Matrix_y);//ȡ���ݲ�����ȱʧֵ�������ݷ������Matrix_X&Matrix_y
// 	CDoubleMatrix	TranProd(CDoubleMatrix &X, CDoubleMatrix &Y);		
protected:
	CDataInterface*	m_pDataInterface;
	TIntStruct		m_tVarInfo;//�Ա���struct
	TIntStruct		m_tVarInfo_D;//�����struct
	CTStringArray  	m_strVarlist;//�������б���һ��Ϊ�������2��Varlist.GetLength()-1Ϊ�Ա���
		
};

#endif // !defined(AFX_PCR_H__36F24C0F_127E_45DD_B569_969506E6006D__INCLUDED_)





















