// VEC.h: interface for the CVEC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VEC_H__F4404C5D_A336_4042_A970_3077BAA4335A__INCLUDED_)
#define AFX_VEC_H__F4404C5D_A336_4042_A970_3077BAA4335A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CVEC : public CAlgorithmBase  
{
public:
	CVEC();
	virtual ~CVEC();
public:
	CResult* OnRun(CDataInterface *pDataInterface, string VO);
	TIntStruct		m_tVarInfo;			//У�����struct
	TIntStruct		m_tVarInfo_D;		//��������struct	
private:
	BOOL GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void DrawResult();
	void GetMatrixData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y);//�ӽ��������		
	
	CTStringArray	m_strDepend;        //У����������ַ���
	CTStringArray	m_strVarSel;        //�������������ַ���
	
	struct VI							//������������ṹ
	{
		CDoubleMatrix m_mxX;			//����ع��������
		CDoubleMatrix m_mxY;			//��������
		int m_nLagnum;					//�����ͺ����
		int	m_nRegre;					//�ع��������
		int	m_nExto;					//������������
		int m_nModel;					//ѡ��ģ��
		int m_nEquNum;					//ѡ��ģ���а�����Э�����̵ĸ��� 
	} m_tIn;
	struct VO 
	{
		CDoubleVector m_vrEigValue;		//����ֵ

		CDoubleMatrix m_mxCoff;			//����Johansen��һ����������������
		double m_nIndex;				//�õ���Э����ϵ�ĸ���
		CDoubleMatrix m_mxCoff1;		//ÿ��Э��ģʽ��ϵ�����ۺϾ���
		CDoubleMatrix m_mxBeta;				//�ع�ϵ������
		CDoubleMatrix m_mxSEReg;			//�ع�ϵ���ı�׼��
		CDoubleMatrix m_mxTvalue;			//�ع�ϵ����Tֵ
		CDoubleMatrix m_mxPvalue;			//�ع�ϵ����Pֵ
		
		double m_dDRC;						//ģ�͵ľ����в�Э����
		double m_dAdjDRC;					//ģ�͵������ľ����в�Э����
		double m_dLogl;					    //ģ�͵Ķ�����Ȼ
		double m_dAIC;						//ģ�͵�AIC׼��
		double m_dSC;						//ģ�͵�SC׼��
		double m_dSBC;						//ģ�͵�SBC׼��
		double m_dFPE;						//ģ�͵�FPE׼��
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
	bool VECest();					//Э������������������Johansen����
	BOOL	Paratest(CDoubleMatrix mxBeta,	//Ҫ���в�������Ļع�ϵ��
						CDoubleMatrix mxRes,	//�в����
						CDoubleMatrix mxYnew,	//�ع������
						CDoubleMatrix mxXnew,	//�ع��Ա���
						int VARindex);
	CDoubleMatrix JJtest(CDoubleMatrix mxX,		//����JJ�㷨�Ĵ�������
						CDoubleMatrix mxY,		//���м���ʱ����������
						int dChoice,			//����ѡ���ģ��
						int lag,				//�����ͺ����
						CDoubleMatrix &mxXnew);	//��������������µ��Ա����б�

};

#endif // !defined(AFX_VEC_H__F4404C5D_A336_4042_A970_3077BAA4335A__INCLUDED_)
