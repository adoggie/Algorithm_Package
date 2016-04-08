/************************************************************************/
/*                           多因素方差分析
                  功能： 计算多因素方差分析表                                                                     
                  标注： 余卫军
				  时间： 2005年10月20日
/************************************************************************/
#pragma once

class CCovariance;
#include "AlgorithmBase.h"
#include <vector>


class CTLTable;
struct TDataSet;

#define  MULTIANOVA_RIGHT      0
#define  MULTIANOVA_OVERFLOW   1
#define  MULTIANOVA_COLERROR   2


class AFX_EXT_CLASS CMultiAnova : public CAlgorithmBase
{
public:	
	CMultiAnova(void);
	~CMultiAnova(void);
  
	void Anova(CDoubleMatrix    X, CDoubleVector&   p    , CDoubleMatrix& q, int reps = 1);	
	int  manov(CDoubleVector    y, CIntMatrix       group, CIntVector model, 
  /*输出参数:*/CDoubleVector&   p, CIntVector&      terms,
  /*默认参数:*/int sstype = 3);
	
	CResult* OnMultAnova(CDataInterface *pDataInterface, std::string VO);

	TIntStruct       m_tVarInfo;
	TIntStruct       m_tVarInfo1;         //协方差用
	int              m_nDependIndex;      //因素变量
	int              m_sstype;
	CIntVector       m_model;
	CCovariance*     m_pCovariance;       //协方差

	CDataInterface*  m_pDataInterface;
	CTStringArray     m_strNameArrayComb;  //变量名组合
	CTString m_strDependentFieldName;//因变量名


private:
	bool			GetValueFromVO(CDataInterface * pDataInterface, std::string VO);
	CTString		ChangeToResultExplain(CTString independent,CTString dependent,double P);

	void	       CreateResult(CDataInterface* pDataInterface,CResult* &pResult);
	void		   anovanfit(CDoubleMatrix dmat, CDoubleVector y,double& ssx, int& dfx);
	void           initfit(CDoubleMatrix& dmat, CDoubleVector& y, CIntVector& termname, 
							double sst, double mu, double& ssx, int& dfx);
	void		   Fun(CIntVector In, CIntVector& Out1, CIntVector& out2);	
	CIntVector     makemodel(int p, int ng);
	CIntMatrix     termsnotcontained(CIntVector t);
	CDoubleMatrix  termcross(CDoubleMatrix a, CDoubleMatrix b);	
	CDoubleMatrix  idummy(CIntVector x, int method = 1);
	
	//以下用于协方差的输入参数
	CDoubleMatrix    m_z;

	//以下用于数据输出.
	int              m_nError;
	CTString          m_strLastError;
	CIntMatrix       m_IntMatrix;;
	int		         m_nIndepNum;			//自变量的个数
	int		         m_nCaseNum;			//观测点数
	CResult*         m_pResult;		
	
	CIntVector       m_vectData;
	CIntVector       m_vectDataCnt;
	CDoubleMatrix    m_mtrixOutPut;         //最主要的输出矩阵.
};
