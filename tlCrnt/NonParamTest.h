// NonParamTest.h: interface for the CNonParamTest class.
//              非参数假设检验
//        包括：两个相关样本，多个相关样本 ，两个独立样本，多个独立样本
//              标注：余卫军 2005年 10月 21日
//				修改：马良庄		2006/03/23
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NONPARAMTEST_H__B5E82D44_4F36_48A8_88D8_A37DCBA0DAA7__INCLUDED_)
#define AFX_NONPARAMTEST_H__B5E82D44_4F36_48A8_88D8_A37DCBA0DAA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "tlcrnt.h"

typedef struct TManyGroup
{
	double dValue;
	double nWeight;
}TManyGroup;

typedef struct TRunNums{
	double dValue;
	int nGroup;
	double nWeight;
}TRunNums;
	
typedef struct tagSturctComboIndex
{
	WORD column1;
	WORD column2;
	CTString str;
}TComboIndex;

class AFX_EXT_CLASS CNonParamTest   
{
public:	
	CNonParamTest();
	virtual ~CNonParamTest();
	CResult *CNonParamTest::OnNonParamTest(CDataInterface *pDataInterface, string VO,int nType);
private:
	bool GetValueFromVo(CDataInterface *pDataInterface, string VO, int nType);
	void OrderTheGroup(CTArray<double,double>&DataArray,CTArray<double,double>&WeightArray);
	void McNemarsTest(CTArray<double,double> &Data1,
					 CTArray<double,double> &Data2,
					 CTArray<double,double> &Weight,CTString &strFieldName);
	void MedianTest(CPtrArray& VectArray,CTArray<double,double> &stdAr,CTString &strFieldName);//
	void KruskalWallisHTest(CPtrArray &VectArray,CTArray<double,double> &stdAr,CTString &strFieldName);//
	void KendallsCoefficientofConcordence(CDoubleMatrix mat,CTArray<double,double> &Weight);//
	void CochransQTest(CDoubleMatrix mat,CTArray<double,double> &Weight);//
	void FriedmansTwowayAnalysisofVariancebyRank(CDoubleMatrix mat,CTArray<double,double> &Weight);//
	void SignTest(CTArray<double,double> &Data1,
				 CTArray<double,double> &Data2,
				 CTArray<double,double> &Weight,CTString &strFieldName);
	void CNonParamTest::WilcoxonMatchedPairsSignRankTest(CTArray<double,double> &Data1,
														 CTArray<double,double> &Data2,
														 CTArray<double,double> &Weight,CTString &strFieldName);
	void CNonParamTest::WaldWolfowitzTest(CTArray<double,double> &Data1,
										 CTArray<double,double> &Data2,
										 CTArray<double,double> &Weight1,
										 CTArray<double,double> &Weight2,
										 CTString &strFieldName);
	double Cnk(double n, double k);//ytli Add,测试用.
	double CnkForBigData(double n, double k);//ytli Add @2003/01/25,测试用.
	void CNonParamTest::MosesTest(CTArray<double,double> &Data1,
								 CTArray<double,double> &Data2,
								 CTArray<double,double> &Weight1,
								 CTArray<double,double> &Weight2,
								 CTString &strFieldName);
	void CNonParamTest::KolmogorovSmirnovTwoSampleTest(CTArray<double,double> &Data1,
														 CTArray<double,double> &Data2,
														 CTArray<double,double> &Weight1,
														 CTArray<double,double> &Weight2,
														 CTString &strFieldName);
	void CNonParamTest::MannWhitneyU(CTArray<double,double> &Data1,
									 CTArray<double,double> &Data2,
									 CTArray<double,double> &Weight1,
									 CTArray<double,double> &Weight2,
									 CTString &strFieldName);

	///////////zkc
	//2002.12.3
	void	CNonParamTest::CreateResultForMann(CTString strGroupFieldName);
	void	CNonParamTest::CreateResultForMoses(CTString strGroupFieldName);
	void	CNonParamTest::CreateResultForKolmogorov(CTString strGroupFieldName);
	void	CNonParamTest::CreateResultForWald(CTString strGroupFieldName);
	void	CNonParamTest::CreateResultForCruskal(CTString strGroupFieldName);
	void	CNonParamTest::CreateResultForMedian(CTArray<double,double>&stdAr,CTString strGroupFieldName);
	void	CNonParamTest::CreateResultForWilcoxon();
	void	CNonParamTest::CreateResultForSign();
	void	CNonParamTest::CreateResultForMcnema();
	void	CNonParamTest::CreateResultForFriedman(int n,double F,double df,double P,CDoubleVector C);
	void	CNonParamTest::CreateResultForKendall(CDoubleVector C,int n,double W,double X,double df,double P);
	void	CNonParamTest::CreateResultForCochran(CDoubleMatrix dmar,int n,double Q,double df,double P);
	void	CNonParamTest::FreeAllMemember();
	void	CNonParamTest::FreeTwoRelated();
	bool CNonParamTest::GroupIntoMany(CPtrArray &DataPtr,
									  TDataSet *pGroupArray,TDataSet *pWeightArray,
									  int nVarCol,CTArray<double,double> &arStd);
	void CNonParamTest::GroupIntoTwo(CTArray<double,double> &ar1,
									 CTArray<double,double> &ar2,
									 CTArray<double,double> &Weight1,
									 CTArray<double,double> &Weight2,
									 double dValue1,double dValue2,
									 int nVarCol,TDataSet *pWeightArray,
									 TDataSet *pGroupArray);
	void CNonParamTest::GetCroupStd(TDataSet *pGroupSet,int nRow,
									CTArray<double,double> &arStd,
									double max,double min);
	void CNonParamTest::GetTwoRelatedData(CTArray<double,double> &Data1,
									  CTArray<double,double> &Data2,
									  CTArray<double,double> &Weight,
									  TDataSet *pWeightArray,
									  int nCol1,int nCol2);
	CDoubleMatrix CNonParamTest::GetMultiRelatedData(CTArray<double,double> &Weight,
												 TDataSet *pWeightArray);
	bool CNonParamTest::IsDataLegal(CPtrArray &DataPtr,int nRow,int j);
	void CNonParamTest::DelDataPtrArray(CPtrArray &DataPtr);
	void CNonParamTest::GetMaxAndMinRuns(CTArray<double,double> &data1Array,
										 CTArray<double,double> &data2Array,
										 CTArray<double,double> &Weight1,
										 CTArray<double,double> &Weight2,
										 double &nMax,double &nMin);
	void OrderTheGroup(CTArray<TRunNums *,TRunNums *>&DataArray);
	double CalkCommonFx(CTArray<double,double> &DataArray,
						 CTArray<double,double> &WeightArray,
						 double x,double nSize);
private:
	CPtrArray m_StrPtr;
	int m_nColumn1,m_nColumn2;
	TIntStruct m_tVarInfo1;
	TIntStruct m_tVarInfo2;
	TIntStruct m_tVarInfo3;
	int m_nGroupIndex1;
	int m_nGroupIndex2;
	CDataInterface * m_pDataInterface;
	CResult		   * m_pResult;
	CRsltElementText *m_p1RsltElementText;
 	CRsltElementText *m_p2RsltElementText;
 	CRsltElementText *m_p3RsltElementText;
  	CRsltElementText *m_p4RsltElementText;
	CRsltVariable * m_pRsltVariable;
	bool m_bWeight;
	int m_nWeight;	//权重下标
	CTArray<int,int> m_RowArray;
	//Mann
	CTStringArray m_FieldNameStrAr,m_ClassStrAr,m_NStrAr,m_MeanRandStrAr,m_SumRandStrAr,m_ZStrAr,m_PStrAr;
	CTArray<double,double> m_MeanWhiteyAr,m_WilcoxonAr,m_ZAr,m_PAr;
	//Moses
	CTArray<int,int> m_NAr;
	CTArray<double,double> m_S1Ar,m_P1Ar,m_S2Ar,m_P2Ar,m_hAr;

//以下用来保存和传递界面设置信息
	int m_nType;//1两个独立样本交换数据2两个立样本交换数据3多个独立样本交换数据4多独立样本交换数据

	//4_7
	bool	m_bKolmogorov;
	bool	m_bMann;
	bool	m_bMoses;
	bool	m_bWald;

	//11,12
	bool	m_bCruskal;
	bool	m_bMedian;

	double m_dValue1;//两个独立样本交换数据
	double m_dValue2;//两个立样本交换数据
	double m_dmValue1;//多个独立样本交换数据
	double m_dmValue2;//多独立样本交换数据

	//8,9,10
	bool	m_bMcnema;
	bool	m_bSign;
	bool	m_bWilcoxon;
	CPtrArray m_PtrArrayIndex;
	CTStringArray m_chiStrArray;

	//13,14,15
	bool	m_bCochran;
	bool	m_bFriedman;
	bool	m_bKendall;

};

#endif // !defined(AFX_NONPARAMTEST_H__B5E82D44_4F36_48A8_88D8_A37DCBA0DAA7__INCLUDED_)
