// Factor.h: interface for the CFactor class.
//              因子分析和主成分分析
//           功能：因子分析和主成分分析的共同方法和属性
//			 标注：  余卫军
//			 时间：2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__BE6438B5_14FC_4B5E_B4E1_7D90749EF0F3__INCLUDED_)
#define AFX_MATRIX_H__BE6438B5_14FC_4B5E_B4E1_7D90749EF0F3__INCLUDED_

// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//因子分析

#include "AlgorithmBase.h"
#include <vector>



class AFX_EXT_CLASS CFactor : public CAlgorithmBase  
{
public:

	CFactor();

	virtual ~CFactor();

public:
	//计算主成分分析
	CResult * OnFactor2(CDataInterface * pDataInterface,std::string &VO);
    //计算因子分析
	CResult * OnFactor(CDataInterface *pDataInterface, std::string VO);

//{{{START 2003-2-13 Eric
	TIntStruct m_tVarInfo2;
	int  brinv(CDoubleVector& a,int n);
//}}}END  2003-2-13 Eric
private:

	bool GetValueFromVOPrin(CDataInterface *pDataInterface, std::string VO);//读主成分的VO

	bool GetValueFromVOFac(CDataInterface *pDataInterface, std::string VO);//读因子分析的VO

	bool FactImage(CDoubleMatrix matrix_z,int nIndex=0);//镜像法提起因子

	bool FactorAlpha2(CDoubleMatrix matrix_Z,int m,int nCountMax);//Alpha提取因子法

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

	CDoubleMatrix GetRotationMatrix(double angle);//计算旋转矩阵
	
private:
	bool	m_bPrincipal;//是否是进行的主成分分析。

	bool	m_bIsMValued;//在最大似然法，普通最小二乘法，无权重最小二乘法，当按累计贡献率提取因子时，标记是否已经计算出了提取因子的个数。zkc
	
	bool	m_bComBigOne;//是否允许共性方法大于1
	
	bool	m_bKMO; //是否进行球度检验
	
	bool	m_bAntiImage;  //是否计算反镜像矩阵
	
	bool	m_bAntiImage2; 
	
	bool	m_bShengyu; //是否计算
	
	bool	m_bZaisheng;//是否计算再生矩阵
	
	bool	m_bScore;//是否计算因子得分

	bool    m_bOutput;//因子得分是否输出到主街面上

	bool	m_bCovarian;//是否是协方差 

	bool	m_bZhengjiao; //是否正交
	
	bool	m_bEigenFlag;

	bool	m_bCompenFlag;										//主成分方法的标志

	bool	m_bRotateFlag;										//旋转标志	
	

	int		m_nProcType;										//1-因子分析 2-主成分分析

	int		m_nMissingMethod;									//缺失值处理办法

	int		m_nRotateMethod;
	
	int		m_nScoreMethod;
	
	int		m_nExtractMethod;	//提取方法
		
	int		m_nExtractStandard;//提取准则，0－用累计贡献率，1－因子个数
	
	int		m_nAnaMethod;		//主成分对象分析0－相关系数1－协方差
	
	int		m_nFactorNum;      //有效因子个数

	int		m_nIteration;	//迭代次数

	int     m_nMaxIterCount; //最大迭代次数

	double m_dExtact;										//特征值的判断标准
	
	double m_dkmo;											//球度检验统计量

	double m_fPro;											//球度检验p值
	
	double  * m_pL;											//斜交因子相关阵
	
	double  * m_pAf;										//斜交因子结构阵
	
	double  * m_pS;											//斜交因子载荷阵
	
	CDoubleVector  m_InitCommunality;
	
	CDoubleVector m_VectFirstCovariance;
	
	CDoubleVector  m_UnAdjustComMatrix;
	
	CDoubleVector  m_SelEigenVector;
	
	CDoubleVector m_VectDataFormlized;
	
	CDoubleVector m_weight;
	
	CDoubleVector m_VectUnAdjustCom;

	CDataInterface * m_pOriDataInterface;

	//符合要求的特征值
	CDoubleVector m_VectSelEigenValue;//计算出因子荷载矩阵后的特征值
	
	//符合要求的特征值
	CDoubleVector m_VectRotEigenValue;//旋转后的因子方差贡献
	
	CDoubleVector m_VectEigenValue;
	
	//特征值向量
	CDoubleVector m_EigenVector;
	
	//主成分矩阵(就是因子载荷矩阵)
	CDoubleVector m_VectComMatrix;
	
	//共性方差矩阵
	CDoubleVector m_VectCommunality;
	
	//旋转后的主成分矩阵
    CDoubleVector m_VectRotateCom;
	
	CDoubleVector m_VectPreCommunality;//循环中使用的上一次的共性方差矩阵
	
	
	//因子得分
	CDoubleVector m_VectScore;

	//旋转后的因子得分
	CDoubleVector m_VectRotateScore;
	
	CDoubleVector  m_VectInputData;

	CDoubleMatrix m_resortMatrix;//旋转后的矩阵
	
	CTArray<int ,int>  m_arrRecNo;



	void CalcFirstCommunalityAndEig(CDoubleVector Vect_Data,int nIndex);
	
	void FromCorrelationToCommunality(CDoubleVector Vect_Data);
	
	CDoubleMatrix RelationMatrixEx(CDoubleMatrix matrix,int nIndex=0);
	
	CDoubleVector  RelationMatrixEx(CDoubleVector Vect_Data,int nRow,int nCol,CDoubleVector weight,int nIndex=0);
	
	CDoubleVector  FormlizeEx(CDoubleVector Vect_Data,CDoubleVector weight);

	CDoubleVector  GetOratedExplains(CDoubleMatrix MatrixA);//计算因子旋转后的方差解释
	
	double			PCorrelation(const int x1,const int x2,	CDoubleVector Vect_rr );
	
	void			CreateEig(int nIndex);

	void			CreateMatrixResult(CDoubleMatrix matrix , char * pszTitle);
	
	void			CreateResult(CResult * pResult);

	void			CreateChartResult(CDoubleVector& vect, int nLen);
	
	void			CreateCommulityTable(); //输出共性方差矩阵
	
	void			CreateEigenValueResult(CDoubleVector pData,const int nLen);
	
	void			CreateEigenValueTable(); //输出调整后的特征值表
	
	void			CreateRotedExplainTable();//输出旋转后的因子荷载方差贡献表

	void			CreateSquareSumExplainTable();//输出旋转后的因子荷载方差贡献表
	
	
	void			PSI(CDoubleVector vector_x,CDoubleMatrix matrix_R,const int nMethod,CDoubleMatrix &matrix_l,CDoubleMatrix &matrix_lambda);

	void			ml_fact(CDoubleMatrix matrix,int nMethod,int m,int s,int ID);

	int				Definite(CDoubleMatrix matrix);

	int				GetSelNum(CDoubleVector Vect_Data);//计算有效特征值个数

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

	CTStringArray m_strResultExplain;//用于结果解释。


	void MatrixMultiply(CDoubleVector a,CDoubleVector b,int m,int n,int k,CDoubleVector& c);
	

	void ProcessEigenVector(CDoubleVector pData,int nIndex=0);	//特征向量的处理
	
	void PrinAxiFactor(int nIndex=0);						//主因子法


	void InitFree();										//初始化和内存释放函数

	void AdjuestResult();								

	void Eigenvalue(CDoubleVector& pData);//求解特征值和特征向量
	void Eigenvalue(CDoubleVector& Vect_Data,CDoubleVector& Vect_EigVector);
	void cjcbj(CDoubleVector& a,int n,CDoubleVector& v,double eps);//求解对称矩阵的特征值和特征向量



	///////the functions below added by zkc during 2003.4.7--2003.4.11
protected:

	CDoubleVector CFactor::Ladder1(CDoubleVector vector_x,
				   CDoubleVector vector_gamma,
				   CDoubleMatrix matrix_omiga,
				   int nMothod,		//0-极大似然法，1-普通最小二乘法，2-无权重最小二乘法
				   int p,  //p=nCol
				   int m,
				   int nMethod);
	CDoubleMatrix CFactor::Ladder2(CDoubleVector vector_gamma,
								   CDoubleVector vector_x,
								   CDoubleMatrix matrix_omiga,
								   CDoubleMatrix matrix_r,//相关系数阵的
								   CDoubleVector vector_h,
								   int nMethod,		//0-极大似然法，1-普通最小二乘法，2-无权重最小二乘法
								   int p,
								   int &m);///p=nCol
	CDoubleMatrix CFactor::Ladder2(CDoubleVector vector_x,
								  CDoubleMatrix matrix_omiga,
								  int &m,int p,
								  int nMethod);
	void CFactor::ChangeLadder2(CDoubleMatrix &matrix);
	CDoubleMatrix CFactor::GetSubMatrix(CDoubleMatrix matrix,
							   int m,		//行下标
							   int n);		//列下标
	bool CFactor::IsNormal(CDoubleMatrix matrix);
	bool CFactor::IsDLegal(CDoubleVector vector_d);
	double CFactor::CalcFx(CDoubleVector vector_x,		//nCol
						  CDoubleMatrix matrix_R,		//相关系数矩阵,nColxnCol
						  CDoubleMatrix matrix_r,		//matrix_R的逆阵,nColxnCol
						  CDoubleMatrix &matrix_psi,	//nCol*nCol
						  CDoubleVector &vector_gamma,	//nCol
						  CDoubleMatrix &matrix_omiga,	//nColxnCol
						  int nMethod,					//0-极大似然法，1-普通最小二乘法，2-无权重最小二乘法
						  int nCol,						//p=nCol
						  int &m);						//提取因子的个数
	bool  CFactor::ChangeX(CDoubleVector &vector_x,		//nCol
						  CDoubleVector vector_d,		//nCol
						  CDoubleVector vector_oldx,	//nCol
						  CDoubleMatrix matrix_R,		//相关系数矩阵,nColxnCol
						  CDoubleMatrix matrix_r,		//matrix_R的逆阵,nColxnCol
						  CDoubleMatrix &matrix_psi,	//nCol*nCol
						  CDoubleVector &vector_gamma,	//nCol
						  CDoubleMatrix &matrix_omiga,	//nCol*nCol
						  int nMethod,					//0-极大似然法，1-普通最小二乘法，2-无权重最小二乘法
						  int nCol,						//p=nCol
						  double f0,					//f0值
						  double &fOrg,
						  int &m);						//提取因子的个数
	CDoubleVector CFactor::CalcCommunality(CDoubleMatrix matrix_omiga,
								  CDoubleVector vector_gamma,
								  CDoubleMatrix matrix_psi,
								  CDoubleMatrix &matrix_OMIGA,
								  CDoubleMatrix &matrix_GAMMA,
								  int p,int m,
								  int nMethod);
	CDoubleMatrix CFactor::powMatrix(CDoubleMatrix matrix,int m);
	CDoubleVector CFactor::VecDiv(CDoubleVector vect,double val);
	void CFactor::CreateResultForMl_fact(CDoubleMatrix matrix_LAMDA,		//因子载荷矩阵
										 CDoubleVector vector_CommunLast,	//共性方差初始值
										 CDoubleVector vector_CommunIntial,	//共性方差提取值
										 CDoubleVector vector_EigenIntial,	//特征根初始值
										 CDoubleVector vector_EigenLast,	//特征根最终值
										 int nMethod,						//0--最大似然法，1--一般最小二乘法，2--无权重最小二乘法
										 int m,								//提取因子数
										 int df,							//自由度
										 double P,							//显著性
										 double chi);						//卡方值
														
};

#endif // !defined(AFX_MATRIX_H__BE6438B5_14FC_4B5E_B4E1_7D90749EF0F3__INCLUDED_)
