#pragma once
/*********************************************************
 *	                     Э��������ķ�����
                    ���ܣ�����Э��������Ĺ�ͬ����������
                    ��ע��  ������
                    ʱ�䣺2005��10��20��
 
 *********************************************************/
#include "MultiAnova.h"
#include "AlgorithmBase.h"

class AFX_EXT_CLASS CCovariance :	public CAlgorithmBase
{
public:
	CDoubleVector askr(CDoubleMatrix x,CDoubleVector y);
	void getb(CDoubleMatrix x,CDoubleVector y,CDoubleMatrix z,int dfe,
		      /*�����*/CDoubleMatrix& R,CDoubleVector& b,double& R02,CDoubleVector& Sz,CDoubleVector& Fz,CDoubleVector& Pz);
	void getxb(CDoubleMatrix x,CDoubleVector y,CDoubleMatrix z,
		      /*�����*/CDoubleMatrix& Rx,CDoubleVector& xb,double& Rx12);
	int  CCovariance::ancovan(CIntMatrix    x,//�������
						  CDoubleVector y,//
						  CDoubleMatrix z, //�������,Ҫ��֤x,y,z��������������һ��.
			   /*�����:*/double& Re, CDoubleVector& B, CDoubleMatrix& RXI, CDoubleVector& SXI, CDoubleVector& FXI, CDoubleVector& PXI, CDoubleVector& MXI, CTStringArray& Name,CIntVector& dfg, int& dfe);

	CCovariance(void);
	~CCovariance(void);

	CMultiAnova* m_pMultiAnova;
};
