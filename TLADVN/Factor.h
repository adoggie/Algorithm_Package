// Factor.h: interface for the CFactor class.
//              ���ӷ��������ɷַ���
//           ���ܣ����ӷ��������ɷַ����Ĺ�ͬ����������
//			 ��ע��  ������
//			 ʱ�䣺2005��10��20��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__BE6438B5_14FC_4B5E_B4E1_7D90749EF0F3__INCLUDED_)
#define AFX_MATRIX_H__BE6438B5_14FC_4B5E_B4E1_7D90749EF0F3__INCLUDED_

// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���ӷ���

#include "AlgorithmBase.h"
#include <vector>



class AFX_EXT_CLASS CFactor : public CAlgorithmBase  
{
public:

	CFactor();

	virtual ~CFactor();

public:
	//�������ɷַ���
	CResult * OnFactor2(CDataInterface * pDataInterface,std::string &VO);
    //�������ӷ���
	CResult * OnFactor(CDataInterface *pDataInterface, std::string VO);

//{{{START 2003-2-13 Eric
	TIntStruct m_tVarInfo2;
	int  brinv(CDoubleVector& a,int n);
//}}}END  2003-2-13 Eric
private:

	bool GetValueFromVOPrin(CDataInterface *pDataInterface, std::string VO);//�����ɷֵ�VO

	bool GetValueFromVOFac(CDataInterface *pDataInterface, std::string VO);//�����ӷ�����VO

	bool FactImage(CDoubleMatrix matrix_z,int nIndex=0);//������������

	bool FactorAlpha2(CDoubleMatrix matrix_Z,int m,int nCountMax);//Alpha��ȡ���ӷ�

	bool Proman(CDoubleMatrix matrix_A,double dbKappa);

	bool Orth(int nMethod);

//{{{START 2003-1-27 Eric
	bool PrincipalComponent();
//}}}END  2003-1-27 Eric

	bool Score(int nIndex=0);

	bool ScoreThompsom();

	bool ScoreRegression();

	bool ScoreBartlett();
	
	bool IsMissing(int nRow);

	bool IsMissing(int nRow,int nCol);
	

	void ComponMethod(int nMethod);		

	void OnFactorExit();

	void PrincipalTable(CResult * pResult);

	//{{{START 2003-1-25 Eric   
	CDoubleVector GetData(CDataInterface *pDataInterface,TIntStruct tVarInfo,int bRemoveEqual = 1);
	//}}}END  2003-1-25 Eric

	CDoubleMatrix GetRotationMatrix(double angle);//������ת����
	
private:
	bool	m_bPrincipal;//�Ƿ��ǽ��е����ɷַ�����

	bool	m_bIsMValued;//�������Ȼ������ͨ��С���˷�����Ȩ����С���˷��������ۼƹ�������ȡ����ʱ������Ƿ��Ѿ����������ȡ���ӵĸ�����zkc
	
	bool	m_bComBigOne;//�Ƿ������Է�������1
	
	bool	m_bKMO; //�Ƿ������ȼ���
	
	bool	m_bAntiImage;  //�Ƿ���㷴�������
	
	bool	m_bAntiImage2; 
	
	bool	m_bShengyu; //�Ƿ����
	
	bool	m_bZaisheng;//�Ƿ������������
	
	bool	m_bScore;//�Ƿ�������ӵ÷�

	bool    m_bOutput;//���ӵ÷��Ƿ��������������

	bool	m_bCovarian;//�Ƿ���Э���� 

	bool	m_bZhengjiao; //�Ƿ�����
	
	bool	m_bEigenFlag;

	bool	m_bCompenFlag;										//���ɷַ����ı�־

	bool	m_bRotateFlag;										//��ת��־	
	

	int		m_nProcType;										//1-���ӷ��� 2-���ɷַ���

	int		m_nMissingMethod;									//ȱʧֵ����취

	int		m_nRotateMethod;
	
	int		m_nScoreMethod;
	
	int		m_nExtractMethod;	//��ȡ����
		
	int		m_nExtractStandard;//��ȡ׼��0�����ۼƹ����ʣ�1�����Ӹ���
	
	int		m_nAnaMethod;		//���ɷֶ������0�����ϵ��1��Э����
	
	int		m_nFactorNum;      //��Ч���Ӹ���

	int		m_nIteration;	//��������

	int     m_nMaxIterCount; //����������

	double m_dExtact;										//����ֵ���жϱ�׼
	
	double m_dkmo;											//��ȼ���ͳ����

	double m_fPro;											//��ȼ���pֵ
	
	double  * m_pL;											//б�����������
	
	double  * m_pAf;										//б�����ӽṹ��
	
	double  * m_pS;											//б�������غ���
	
	CDoubleVector  m_InitCommunality;
	
	CDoubleVector m_VectFirstCovariance;
	
	CDoubleVector  m_UnAdjustComMatrix;
	
	CDoubleVector  m_SelEigenVector;
	
	CDoubleVector m_VectDataFormlized;
	
	CDoubleVector m_weight;
	
	CDoubleVector m_VectUnAdjustCom;

	CDataInterface * m_pOriDataInterface;

	//����Ҫ�������ֵ
	CDoubleVector m_VectSelEigenValue;//��������Ӻ��ؾ���������ֵ
	
	//����Ҫ�������ֵ
	CDoubleVector m_VectRotEigenValue;//��ת������ӷ����
	
	CDoubleVector m_VectEigenValue;
	
	//����ֵ����
	CDoubleVector m_EigenVector;
	
	//���ɷ־���(���������غɾ���)
	CDoubleVector m_VectComMatrix;
	
	//���Է������
	CDoubleVector m_VectCommunality;
	
	//��ת������ɷ־���
    CDoubleVector m_VectRotateCom;
	
	CDoubleVector m_VectPreCommunality;//ѭ����ʹ�õ���һ�εĹ��Է������
	
	
	//���ӵ÷�
	CDoubleVector m_VectScore;

	//��ת������ӵ÷�
	CDoubleVector m_VectRotateScore;
	
	CDoubleVector  m_VectInputData;

	CDoubleMatrix m_resortMatrix;//��ת��ľ���
	
	CTArray<int ,int>  m_arrRecNo;



	void CalcFirstCommunalityAndEig(CDoubleVector Vect_Data,int nIndex);
	
	void FromCorrelationToCommunality(CDoubleVector Vect_Data);
	
	CDoubleMatrix RelationMatrixEx(CDoubleMatrix matrix,int nIndex=0);
	
	CDoubleVector  RelationMatrixEx(CDoubleVector Vect_Data,int nRow,int nCol,CDoubleVector weight,int nIndex=0);
	
	CDoubleVector  FormlizeEx(CDoubleVector Vect_Data,CDoubleVector weight);

	CDoubleVector  GetOratedExplains(CDoubleMatrix MatrixA);//����������ת��ķ������
	
	double			PCorrelation(const int x1,const int x2,	CDoubleVector Vect_rr );
	
	void			CreateEig(int nIndex);

	void			CreateMatrixResult(CDoubleMatrix matrix , char * pszTitle);
	
	void			CreateResult(CResult * pResult);

	void			CreateChartResult(CDoubleVector& vect, int nLen);
	
	void			CreateCommulityTable(); //������Է������
	
	void			CreateEigenValueResult(CDoubleVector pData,const int nLen);
	
	void			CreateEigenValueTable(); //��������������ֵ��
	
	void			CreateRotedExplainTable();//�����ת������Ӻ��ط���ױ�

	void			CreateSquareSumExplainTable();//�����ת������Ӻ��ط���ױ�
	
	
	void			PSI(CDoubleVector vector_x,CDoubleMatrix matrix_R,const int nMethod,CDoubleMatrix &matrix_l,CDoubleMatrix &matrix_lambda);

	void			ml_fact(CDoubleMatrix matrix,int nMethod,int m,int s,int ID);

	int				Definite(CDoubleMatrix matrix);

	int				GetSelNum(CDoubleVector Vect_Data);//������Ч����ֵ����

	CDoubleMatrix	HC(CDoubleVector vector_h,const int m,CDoubleMatrix matrix_l,CDoubleMatrix marix_lambda,int nWay,const int nMethod,CDoubleVector vector_x,CDoubleMatrix matrix_r);

	CDoubleVector	DC(const int m,CDoubleMatrix matrix_l,CDoubleMatrix matrix_lambda,const int s,CDoubleVector vector_d,const int nMethod, CDoubleVector vector_x,CDoubleMatrix matrix_r);

   

	void KMO(int nType=0);

	CDoubleMatrix GetPCorrelationMatrix(CDoubleVector prr);

	double		PCorrelation(const int x1,const int x2);

	double Sphericity(CDoubleMatrix matrix_x);


	void ProcessRotateMatrix(CDoubleMatrix &matrix);

	void ProcessComMatrix(CDoubleVector& pData,int nRow,int nCol);

	CTString m_strRotateMethod;

	CTString m_strExtractMethod;

	CTString m_strScoreMethod;
	

	CDoubleMatrix FormalizeWithRow(CDoubleMatrix matrix_A);

	CDoubleMatrix FormalizeWithCol(CDoubleMatrix matrix_A);

	double Angle(CDoubleMatrix matrix_f,int l,int k,int nMethod);

	double sv2(CDoubleMatrix matrix_A);

	

	bool NeedScoreRegression();

	CTStringArray m_strResultExplain;//���ڽ�����͡�


	void MatrixMultiply(CDoubleVector a,CDoubleVector b,int m,int n,int k,CDoubleVector& c);
	

	void ProcessEigenVector(CDoubleVector pData,int nIndex=0);	//���������Ĵ���
	
	void PrinAxiFactor(int nIndex=0);						//�����ӷ�


	void InitFree();										//��ʼ�����ڴ��ͷź���

	void AdjuestResult();								

	void Eigenvalue(CDoubleVector& pData);//�������ֵ����������
	void Eigenvalue(CDoubleVector& Vect_Data,CDoubleVector& Vect_EigVector);
	void cjcbj(CDoubleVector& a,int n,CDoubleVector& v,double eps);//���Գƾ��������ֵ����������



	///////the functions below added by zkc during 2003.4.7--2003.4.11
protected:

	CDoubleVector CFactor::Ladder1(CDoubleVector vector_x,
				   CDoubleVector vector_gamma,
				   CDoubleMatrix matrix_omiga,
				   int nMothod,		//0-������Ȼ����1-��ͨ��С���˷���2-��Ȩ����С���˷�
				   int p,  //p=nCol
				   int m,
				   int nMethod);
	CDoubleMatrix CFactor::Ladder2(CDoubleVector vector_gamma,
								   CDoubleVector vector_x,
								   CDoubleMatrix matrix_omiga,
								   CDoubleMatrix matrix_r,//���ϵ�����
								   CDoubleVector vector_h,
								   int nMethod,		//0-������Ȼ����1-��ͨ��С���˷���2-��Ȩ����С���˷�
								   int p,
								   int &m);///p=nCol
	CDoubleMatrix CFactor::Ladder2(CDoubleVector vector_x,
								  CDoubleMatrix matrix_omiga,
								  int &m,int p,
								  int nMethod);
	void CFactor::ChangeLadder2(CDoubleMatrix &matrix);
	CDoubleMatrix CFactor::GetSubMatrix(CDoubleMatrix matrix,
							   int m,		//���±�
							   int n);		//���±�
	bool CFactor::IsNormal(CDoubleMatrix matrix);
	bool CFactor::IsDLegal(CDoubleVector vector_d);
	double CFactor::CalcFx(CDoubleVector vector_x,		//nCol
						  CDoubleMatrix matrix_R,		//���ϵ������,nColxnCol
						  CDoubleMatrix matrix_r,		//matrix_R������,nColxnCol
						  CDoubleMatrix &matrix_psi,	//nCol*nCol
						  CDoubleVector &vector_gamma,	//nCol
						  CDoubleMatrix &matrix_omiga,	//nColxnCol
						  int nMethod,					//0-������Ȼ����1-��ͨ��С���˷���2-��Ȩ����С���˷�
						  int nCol,						//p=nCol
						  int &m);						//��ȡ���ӵĸ���
	bool  CFactor::ChangeX(CDoubleVector &vector_x,		//nCol
						  CDoubleVector vector_d,		//nCol
						  CDoubleVector vector_oldx,	//nCol
						  CDoubleMatrix matrix_R,		//���ϵ������,nColxnCol
						  CDoubleMatrix matrix_r,		//matrix_R������,nColxnCol
						  CDoubleMatrix &matrix_psi,	//nCol*nCol
						  CDoubleVector &vector_gamma,	//nCol
						  CDoubleMatrix &matrix_omiga,	//nCol*nCol
						  int nMethod,					//0-������Ȼ����1-��ͨ��С���˷���2-��Ȩ����С���˷�
						  int nCol,						//p=nCol
						  double f0,					//f0ֵ
						  double &fOrg,
						  int &m);						//��ȡ���ӵĸ���
	CDoubleVector CFactor::CalcCommunality(CDoubleMatrix matrix_omiga,
								  CDoubleVector vector_gamma,
								  CDoubleMatrix matrix_psi,
								  CDoubleMatrix &matrix_OMIGA,
								  CDoubleMatrix &matrix_GAMMA,
								  int p,int m,
								  int nMethod);
	CDoubleMatrix CFactor::powMatrix(CDoubleMatrix matrix,int m);
	CDoubleVector CFactor::VecDiv(CDoubleVector vect,double val);
	void CFactor::CreateResultForMl_fact(CDoubleMatrix matrix_LAMDA,		//�����غɾ���
										 CDoubleVector vector_CommunLast,	//���Է����ʼֵ
										 CDoubleVector vector_CommunIntial,	//���Է�����ȡֵ
										 CDoubleVector vector_EigenIntial,	//��������ʼֵ
										 CDoubleVector vector_EigenLast,	//����������ֵ
										 int nMethod,						//0--�����Ȼ����1--һ����С���˷���2--��Ȩ����С���˷�
										 int m,								//��ȡ������
										 int df,							//���ɶ�
										 double P,							//������
										 double chi);						//����ֵ
														
};

#endif // !defined(AFX_MATRIX_H__BE6438B5_14FC_4B5E_B4E1_7D90749EF0F3__INCLUDED_)
