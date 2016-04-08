// RootTest.h: interface for the CRootTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOTTEST_H__B320738C_3229_4C2D_AFD4_BB0D238FD57A__INCLUDED_)
#define AFX_ROOTTEST_H__B320738C_3229_4C2D_AFD4_BB0D238FD57A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CRootTest : public CAlgorithmBase  
{
public:
	CRootTest();
	virtual ~CRootTest();
	CResult * OnRun(CDataInterface *pDataInterface, string VO);//�ⲿ������Ҫ����
	TIntStruct		m_tVarInfo;				//�ع����struct
	CTStringArray m_strVarSel;				//������
	int m_nRegre;							//�ع��������
	int m_nTestnum;							//���鷽��
private:
	struct VI								//������������ṹ
	{
		CDoubleMatrix m_mxX;				//����ع��������
		int m_nDifnum;						//���в�ֽ���
		int m_nLagnum;						//�����ͺ����
		int m_nTruncat;						//���д���
		int m_nModel;						//ѡ��ģ��
	} m_tIn;
	
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
		CDoubleVector m_vrFtest;			//�����ع鷽�̵�Fͳ����
		CDoubleVector m_vrDRC;				//�����ع鷽�̵�DRC
		CDoubleVector m_vrLogL;				//�����ع鷽�̵Ķ�����Ȼ��
		CDoubleVector m_vrAIC;				//�����ع鷽�̵�AIC׼��
		CDoubleVector m_vrSC;				//�����ع鷽�̵�SC׼��
	} m_tOut;

	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	//�������	
	void DrawResult();

	//��ȡ�ع����Matrix_X
	void GetMatrixData(CDoubleMatrix & Matrix_X);
				
	//ADF���������򣬳ɹ��򷵻�TRUE�����򷵻�FALSE
	BOOL ADFtest();												
	
	//PP���������򣬳ɹ��򷵻�TRUE�����򷵻�FALSE
	BOOL PPtest();								
	
	//KPSS�����򣬳ɹ��򷵻�TRUE�����򷵻�FALSE
	BOOL KPSStest();											
																
	//���ڶ�ģ�͸����������̵�ͳ����
	BOOL Paratest(CDoubleMatrix mxBeta,     //����ϵ����
				  CDoubleMatrix mxRes,		//�в���
				  CDoubleMatrix mxY,		//�ع������
				  CDoubleMatrix mxX,		//�ع��Ա�������
				  int VARindex);			//�ڼ����������
	
	//���ڼ���ָ������Ĳ�ֺ���
	CDoubleMatrix Vdif(CDoubleMatrix mxX,int m_nLagnum);
};

#endif // !defined(AFX_ROOTTEST_H__B320738C_3229_4C2D_AFD4_BB0D238FD57A__INCLUDED_)










































