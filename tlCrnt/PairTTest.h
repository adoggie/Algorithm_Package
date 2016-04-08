// PairTTest.h: interface for the CPairTTest class.
//                    配对样本的T检验
//					马良庄    2006/03/21
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAIRTTEST_H__9FF9DF3D_2C57_4D65_BD07_880B2F3A82A9__INCLUDED_)
#define AFX_PAIRTTEST_H__9FF9DF3D_2C57_4D65_BD07_880B2F3A82A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CPairTTest  
{
public:
	CPairTTest();
	virtual ~CPairTTest();
	
private:
	CPtrArray m_PtrArrayIndex;//配对变量信息
	TIntStruct m_tVarInfo;
	double m_dConfidence;		//置信度
	CDataInterface * m_pDataInterface;//数据接口指针
//	CWnd           * m_pWnd;		//输出窗口指针
	CResult		   * m_pResult;		//输出结果集
	CRsltElementText *m_pRsltElementText;//出错信息记录
	BOOL m_bWeight;					//是否指定了权重
	int m_nWeight;					//权重下标
	CTArray<int,int> m_RowArray;		//被删除不合法行记录
	int m_nPairs;

	bool m_bLT;
	bool m_bNEQ;
	bool m_bGT;

	bool m_bNormalTest;//是否输出样本差正态性检验
	bool m_bHistogram; //直方图
	int  m_nDivi;      //分区数
	bool m_bNormalLine; //正态分布曲线

	//统计表输出，用以下变量存储
	CTStringArray m_NameAr;			//变量名
	CTArray<double,double> m_MeanSAr;			//均值
	CTStringArray m_FrequencySAr;	//频数
	CTArray<double,double> m_StdDSAr;			//标准离差
	CTArray<double,double> m_StdEMSAr;		//均值标准误

	//相关性表
	CTStringArray m_PairNameAr;		//变量名(配对)
	CTStringArray m_FrequencyPAr;	//频数
	CTStringArray m_CorrAr;			//相关性
	CTStringArray m_P2Ar;			//显著性

	//检验表
	CTArray<double,double> m_MeanPAr;			//均值
	CTArray<double,double> m_StdDPAr;			//标准离差
	CTArray<double,double> m_StdEMPAr;			//均值标准误
	CTArray<double,double> m_LowerAr;			//下限
	CTArray<double,double> m_UpperAr;			//上限
	CTArray<double,double> m_tAr;				//t值

	
	CPtrArray m_ErrorAry;//残差

	CTStringArray m_dfStrAr;				//自由度
	CTArray<double,double> m_PAr;				//显著性(双尾)
	CTArray<double,double> m_PArGT;			//被择假设为大于
	CTArray<double,double> m_PArLT;			//被择假设为小于
public:
	CResult *CPairTTest::OnPairTTest(CDataInterface *pDataInterface, string VO);

protected:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	void CPairTTest::GetTwoRelatedData(CTArray<double,double> &Data1,
									  CTArray<double,double> &Data2,
									  CTArray<double,double> &Weight,
									  TDataSet *pWeightArray,
									  int nCol1,int nCol2);
	void CPairTTest::FreeAllMemember();
	BOOL CPairTTest::PairTTest(CTArray<double,double> &Data1,//配对变量1数据
							   CTArray<double,double> &Data2,//配对变量2数据
							   CTArray<double,double> &Weight,//对应的权重
							   int column1,int column2,//对应的列索引
							   CTString strName);//配对变量名
	void CPairTTest::CreateResult();


};

#endif // !defined(AFX_PAIRTTEST_H__9FF9DF3D_2C57_4D65_BD07_880B2F3A82A9__INCLUDED_)
