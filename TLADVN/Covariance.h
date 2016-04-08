#pragma once
/*********************************************************
 *	                     协方差分析的方法类
                    功能：各种协方差分析的共同方法和属性
                    标注：  余卫军
                    时间：2005年10月20日
 
 *********************************************************/
#include "MultiAnova.h"
#include "AlgorithmBase.h"

class AFX_EXT_CLASS CCovariance :	public CAlgorithmBase
{
public:
	CDoubleVector askr(CDoubleMatrix x,CDoubleVector y);
	void getb(CDoubleMatrix x,CDoubleVector y,CDoubleMatrix z,int dfe,
		      /*输出行*/CDoubleMatrix& R,CDoubleVector& b,double& R02,CDoubleVector& Sz,CDoubleVector& Fz,CDoubleVector& Pz);
	void getxb(CDoubleMatrix x,CDoubleVector y,CDoubleMatrix z,
		      /*输出行*/CDoubleMatrix& Rx,CDoubleVector& xb,double& Rx12);
	int  CCovariance::ancovan(CIntMatrix    x,//分组变量
						  CDoubleVector y,//
						  CDoubleMatrix z, //最多两列,要保证x,y,z三个变量的行数一样.
			   /*输出行:*/double& Re, CDoubleVector& B, CDoubleMatrix& RXI, CDoubleVector& SXI, CDoubleVector& FXI, CDoubleVector& PXI, CDoubleVector& MXI, CTStringArray& Name,CIntVector& dfg, int& dfe);

	CCovariance(void);
	~CCovariance(void);

	CMultiAnova* m_pMultiAnova;
};
