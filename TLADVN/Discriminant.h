// Discriminant.h: interface for the CDiscriminant class.
//
//								�б����
//                    ���ܣ��б�����Ļ�������������
//                    ��ע��  ������
//					  ʱ�䣺2005��10��20��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISCRIMINANT_H__0FB67880_68A6_422C_AE6D_F85FD932B819__INCLUDED_)
#define AFX_DISCRIMINANT_H__0FB67880_68A6_422C_AE6D_F85FD932B819__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

//#include "Factor.h"
//#include "Matrix.h"	// Added by ClassView
#include "AlgorithmBase.h"



class AFX_EXT_CLASS CDiscriminant : public CAlgorithmBase  
{
public:
	int m_nDependIndex;
	CResult * OnDiscriminant(CDataInterface * pDataInterface,std::string &VO);
	bool StepWiseDiscriminant(CDoubleMatrix matrix_X,int allORstempwise,int distanceORfisher,double Entry,double Removal);
	CDiscriminant();
	virtual ~CDiscriminant();
	TIntStruct m_tVarInfo;

private:
	void Classify(CDoubleMatrix &finalClassData);    //�б�������й��̣��¼ӣ�
	bool GetValueFromVO(CDataInterface *pDataInterface, std::string VO);//��VO�ַ����ж�ȡ���Ʋ���
	void CreateResultF(CDoubleVector F,CDoubleVector L);
	void CreateResultC1(CDoubleMatrix matrix_C1);
	void CreateResultR(CDoubleMatrix matrix_R);
	void CreateResultT1(CDoubleMatrix matrix_TC);
	void CreateResultdiscri(CDoubleMatrix discriminant_mx);   //������н��С�ᣨ�¼ӣ�
	void CreateResultdiscripercent(CDoubleMatrix discriminantpercent_mx);   //������н���ٷֱ�С�ᣨ�¼ӣ�
	void CreateResult();    //������������н�����¼ӣ�
	void CreateResultC2(CDoubleMatrix * pt,char * pszTitle);
	void CreateResultIn1(CDoubleVector F,CDoubleVector L,CIntVector In,int vrb,CTString pszTitle);
	void CreateResultCO(CDoubleMatrix a,CDoubleVector b,int vrb,CDoubleVector vector_dnum);
	CDoubleMatrix DeleteSingleGroup(CDoubleMatrix matrix);
	void CreateResult(CDoubleMatrix matrix_a,CDoubleVector vector_b,CDoubleMatrix matrix_result);	
	void SortMatrix(CDoubleMatrix &matrix);
	void DataProcess(CDoubleMatrix &matrix);
	CIntVector Classification(CDoubleMatrix matrix_X, CDoubleMatrix matrix_a, CDoubleVector vector_b);
	void CpTable(CDoubleVector vector_original, CIntVector vector_predict, CDoubleMatrix &matrix_classresult, CDoubleMatrix &matrix_rateresult);
	double	m_dEntry;
	double	m_dRemoval;
	int		m_nCoefficient;
	double	m_nMethod;
	CDoubleMatrix m_finalClassData;
    CDoubleMatrix m_original_mx;
	bool	m_bC1;	//���Э����
	bool	m_bC2;	//����Э����
	bool	m_bT1;	//��Э����
	bool	m_bR;	//���ϵ������ 
	bool	m_bFL;	//F��LAMBDA
	bool	m_bIn1;	//��ѡ���������
	bool	m_bDisResult; // �Ƿ�������������н�����¼ӣ�
	CDoubleVector m_constant_Vec;  //������ϵ�����¼ӣ�
	CDoubleMatrix m_cofficient_mx;  //ϵ�������¼ӣ�
	int m_group;    //��������¼ӣ�
	CDoubleVector m_class_Vec;
	CDoubleVector m_num_Vec; //ÿ����������  ���¼ӣ�
	CDoubleVector m_oriclass_Vec;  //ԭ��������¼ӣ�
	CDoubleVector m_newclass_Vec;   //��������¼ӣ�
	CDoubleMatrix m_discri_mx;      //����С������¼ӣ�
	CDoubleMatrix m_discripercent_mx;  //���аٷֱ�С������¼ӣ�
	CDoubleVector GetData(CDataInterface * pDataInterface,int &Sum);
	CDoubleVector GetGroupValue(CDoubleVector vect);
	int GetGroup(CDoubleVector vect);
	bool Coefficient(CDoubleVector vector_prob,CIntVector vector_In,double nSum,CIntVector vector_m,CDoubleMatrix matrix_w,CDoubleMatrix matrix_XGrpVrbMean,CPtrArray * parX3D,CDoubleMatrix &matrix_a,CDoubleVector &vector_b,int way,int vrb);
	bool In(CDoubleMatrix &matrix_w, CDoubleMatrix &matrix_t, double nSum, int g, int i, CIntVector &vector_In, double Entry, CDoubleVector &vector_LAMBDA, int &vrb, int &id, int p, int allORstepwise, double Removal);
	bool Sweep(CDoubleMatrix matrix_w, CDoubleMatrix matrix_t, int r,CDoubleMatrix &matrix_w1,CDoubleMatrix &matrix_t1);
};

#endif // !defined(AFX_DISCRIMINANT_H__0FB67880_68A6_422C_AE6D_F85FD932B819__INCLUDED_)
