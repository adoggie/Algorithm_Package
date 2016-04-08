// RegressionLinear.h: interface for the CRegressionLinear class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGRESSIONLINEAR_H__5DFA1AFB_8267_4338_BB7E_C8F300EE0C59__INCLUDED_)
#define AFX_REGRESSIONLINEAR_H__5DFA1AFB_8267_4338_BB7E_C8F300EE0C59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CScript;

class CRegressionLinear : public CAlgorithmBase  
{
public:
	CRegressionLinear();
	virtual ~CRegressionLinear();
	CResult * OnStepWise(CDataInterface * pDataInterface, string VO);
	CResult * OnRegPred(CDataInterface * pDataInterface, string VO);

private:
	string m_strModel;//�ַ�������˳��const_n������_ϵ��_������_(xx)_�����_MSE
	bool GetValueFromVo(CDataInterface * pDataInterface, string VO);
	bool GetPredValueVo(CDataInterface * pDataInterface, string VO);
	bool GetValueFromModel(CDataInterface* pDataInterface, string Model);//���ģ��
	bool Export();//���ģ��
	void DrawTable(int XCol, int WithConst);
	void DrawTableBody(int XCol, int WithConst, int &number, CTString strWarning);
	void DrawTableHead(int XCol, int WithConst);
	void InitParma();//����ֵ
	void  GetData(CDataInterface *pDataInterface, CDoubleMatrix& Matrix_Data);//����
	CDoubleMatrix EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex);//������������������
	double GetQValue(CDoubleMatrix matrix,CDoubleMatrix matrix_Y);
	CDoubleMatrix DelMatrix(CDoubleMatrix matrix_comein, int mm);//ɾ������ĵ�mm��mm��
	CDoubleVector DelVector(CDoubleVector vector_comein, int mm);//ɾ�������ĵ�mm��
	void RegLinear(CDoubleMatrix XX, CDoubleMatrix Xy, CDoubleMatrix CorrMatrix,double sum_weight, int XCol, int XRow, bool withconst);//��ֵ
	CDoubleVector mmthSelfReg(CDoubleMatrix matrix_comein, CDoubleVector Xmean, bool WithConst);//��Tolerance
	CDoubleVector SelectVector(CDoubleVector SingVector,CDoubleVector SVector, int tot);//ͬ��
	CDoubleMatrix SelectMatrix(CDoubleVector SingVector,CDoubleMatrix SMatrix, int tot);
	//ѡ��SingVector�д���0�Ķ�Ӧid����Ӧ��Smatrix�е��tot��ѡ��������
	void SelectStrAry(CDoubleVector SingVector,CTStringArray &m_strX_nameAry, CTStringArray &arrayTemp);
	//ͬ�ϣ�ѡ����Array����m_X_temp_nameAry���
	void DrawResult(int XCol, int WithConst);//�����������������
	void DrawCorrolation(CDoubleMatrix Xy_Sigma);
	void ResiPlot(CDoubleMatrix data_read_resi, bool WithConst, CDoubleVector coefficiency, CDoubleMatrix XtimesX, double MSE);//�����Ҫ�в��ͼ�εȡ�
	bool IsExisted(CDataInterface *p,CTString str);//�жϱ����Ƿ�����
	void Collinearity(CDoubleMatrix m_XtimesX, int XCol, bool WithConst);//���㹲�����е���������������
		
	////////////
	CTLTable* briefrTable;
	////////////////////////////
	int m_nColl;
	int m_nLine;
	int  m_nDependIndex;
	bool m_bResidue;
	bool m_bPredict;
	int m_nWeightID;//Ȩ�صı���ID
	bool m_bOutput;//�Ƿ�������ģ��
	bool m_bSaveModel;//�Ƿ񱣳�ģ��
	CTString m_ModelFile;//����ģ�͵����֡�·��
	CDoubleMatrix m_X_inv;//������������ȷ�����½�ġ�
	int m_nMissMethod;//ȱʧֵ������
	int m_nMethod;//����ѡ�񷽷���0Ϊȫ��ѡ��1Ϊ���2Ϊ��ǰ��3Ϊ��
	bool m_bStopCalculate;
	bool m_bWithWeight;//�Ƿ���Ȩ��
	double sum_weight;//Ȩ���ܺ�
	bool m_bCorr; //�Ƿ������ؾ���
	double m_fEntry;
	double m_fRemoval;
	CTString m_strErrorInfo;
	CTStringArray m_strX_nameAry;//�Ա�����
	CTStringArray m_strX_org;
	CTStringArray m_X_temp_nameAry;//������𲽻ع��У��Ա����������лᷢ���仯ʱ����������ʾ��
	CTString m_stry_name;//���������
	CDoubleVector Xmean;
	CDoubleVector m_coefficiency;//����ϵ����
	CDoubleVector m_Tolerance;
	double m_df_residual;//���ɶ�
	CDoubleVector m_vector_standbeta;//��׼�����X�õ���beta
	CDoubleVector m_vector_T;//tֵ
	CDoubleVector m_vector_Sig;//���Ŷ�
	double m_R_square;//r��
	double m_R_adjust;//r������
	CDoubleVector m_vector_sbeta;//ϵ���ķ���
	CDoubleMatrix m_CorrMatrix;//���ϵ��
	bool m_bWithColl;//�Ƿ���������������Ϣ
	CDoubleVector m_useforcoll_e;
	CDoubleVector m_vector_condindex;
	double m_var_y;
	double m_ytimesy;
	double m_SSE;
	double m_SST;
	double m_SSR;
	double m_MSE;
	int m_nXRow;//��Ч��������
	int m_nXCol;//�ж��ٸ��Ա���
	CDoubleMatrix m_EigValue;//����������
	CDoubleMatrix m_XtimesX;//X��X���г���
	CDoubleMatrix m_Xtimesy;//X��y���г���
	CDoubleMatrix m_XyTimesXy;//�����Ա�����������ĳ˻�����
	//�������Ҫ�޸ĵ�
	double m_dConfidence;//���Ŷ�
	bool m_bCoefInter;//�Ƿ�����µ���������
	CDoubleVector m_CoefUpper;//beta�������Ͻ�
	CDoubleVector m_CoefLower;//beta�������½�
	double  m_dTest;
	bool    m_bChartLine; //���ͼ
	bool    m_bChartScatterPlot; // �в�ɢ��ͼ
	bool    m_bChartScatterPlot2; // �в���̬����ͼ
	bool    m_bWithConst;          //�Ƿ��г�����
	bool    m_bDrawTableHead;
	CTLTable *pTable1;
	CTLTable *pTable2;
	CTLTable *pTable3;
	CTLTable *pTable4;
	CTLTable *pTable6;
	TIntStruct m_tVarPred;//Ԥ��������±꣬��������˳�����еġ�
	CTString m_strDepPred;//�����Ԥ�����������
	int m_nVar;//Ԥ��ģ�Ͷ��ٸ�����
	bool m_bPredRange;//�Ƿ����Ԥ��ֵ����������
	CTStringArray m_strArr_FinalXname;//�������Щ������
};

#endif // !defined(AFX_REGRESSIONLINEAR_H__5DFA1AFB_8267_4338_BB7E_C8F300EE0C59__INCLUDED_)
