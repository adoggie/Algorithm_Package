/************************************************************************/
/*                           �����ط������
                  ���ܣ� ��������ط��������                                                                     
                  ��ע�� ������
				  ʱ�䣺 2005��10��20��
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
  /*�������:*/CDoubleVector&   p, CIntVector&      terms,
  /*Ĭ�ϲ���:*/int sstype = 3);
	
	CResult* OnMultAnova(CDataInterface *pDataInterface, std::string VO);

	TIntStruct       m_tVarInfo;
	TIntStruct       m_tVarInfo1;         //Э������
	int              m_nDependIndex;      //���ر���
	int              m_sstype;
	CIntVector       m_model;
	CCovariance*     m_pCovariance;       //Э����

	CDataInterface*  m_pDataInterface;
	CTStringArray     m_strNameArrayComb;  //���������
	CTString m_strDependentFieldName;//�������


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
	
	//��������Э������������
	CDoubleMatrix    m_z;

	//���������������.
	int              m_nError;
	CTString          m_strLastError;
	CIntMatrix       m_IntMatrix;;
	int		         m_nIndepNum;			//�Ա����ĸ���
	int		         m_nCaseNum;			//�۲����
	CResult*         m_pResult;		
	
	CIntVector       m_vectData;
	CIntVector       m_vectDataCnt;
	CDoubleMatrix    m_mtrixOutPut;         //����Ҫ���������.
};
