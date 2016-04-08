// CointegrationTest.h: interface for the CCointegrationTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COINTEGRATIONTEST_H__AB39CB63_9C8C_4A10_A525_125B0A1539FF__INCLUDED_)
#define AFX_COINTEGRATIONTEST_H__AB39CB63_9C8C_4A10_A525_125B0A1539FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CCointegrationTest : public CAlgorithmBase  
{
public:
	CResult* OnRun(CDataInterface *pDataInterface, string VO);
	CCointegrationTest();
	virtual ~CCointegrationTest();
	TIntStruct		m_tVarInfo;			//�������struct
	TIntStruct		m_tVarInfo_D;		//��������struct	
private:
	BOOL GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void DrawResult();
	void GetMatrixData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y);//�ӽ��������		

	CTStringArray	m_strDepend;        //������������ַ���
	CTStringArray	m_strVarSel;        //�������������ַ���

	struct VI							//������������ṹ
	{
		CDoubleMatrix m_mxX;			//��������������
		CDoubleMatrix m_mxY;			//��������
		int m_nLagnum;					//�����ͺ����
		int	m_nRegre;					//У���������
		int	m_nExto;					//������������
		int m_nModel;					//ѡ��ģ��
	} m_tIn;
	struct VO 
	{
		CDoubleVector m_vrEigValue;		//����ֵ
		CDoubleVector m_vrEigTrace;		//�켣����ͳ����
		CDoubleVector m_vrEigMax;		//�������ֵͳ����
		CDoubleMatrix m_mxMaxCri;		//�������ֵ�����ٽ�ֵ����
		CDoubleMatrix m_mxTraceCri;		//����ֵ�켣��������ֵ����
		CDoubleVector m_vrTraceCrit;	//Ҫ�õ�������ֵ�켣�ٽ�ֵ
		CDoubleVector m_vrMaxCrit;		//Ҫ�õ����������ֵ�����ٽ�ֵ
		CDoubleMatrix m_mxCoff;			//����Johansen��һ����������������
		double m_nIndex;				//�õ���Э����ϵ�ĸ���
		CDoubleMatrix m_mxCoff1;		//ÿ��Э��ģʽ��ϵ�����ۺϾ���

	} m_tOut;
	bool coinTest();					//Э������������������Johansen�������ο����ܶ��١�ʱ�����з�����
	CDoubleMatrix JJtest(CDoubleMatrix mxX,			//Ҫ����ı����б�
						CDoubleMatrix mxY,			//��������������
						int dChoice,				//ģ�͵�����ѡ��
						int lag,					//�����ͺ�Ľ���
						CDoubleVector& mxEigvec,
						CDoubleMatrix& mxVV);	
	//�������������JJ����

};

#endif // !defined(AFX_COINTEGRATIONTEST_H__AB39CB63_9C8C_4A10_A525_125B0A1539FF__INCLUDED_)





















