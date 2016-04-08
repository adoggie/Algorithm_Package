// OneSmplTTest.h: interface for the COneSmplTTest class.
//                        单样本参数检验
//                    包括： 单样本的均值，方差，比例检验等
//                   标注： 余卫军 2005年10月21日
//									马良庄		2006/03/21
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ONESMPLTTEST_H__6CB2C14D_73BD_466D_8BB9_543630921336__INCLUDED_)
#define AFX_ONESMPLTTEST_H__6CB2C14D_73BD_466D_8BB9_543630921336__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class _declspec(dllexport) COneSmplTTest  
{
public:
	COneSmplTTest();
	virtual ~COneSmplTTest();

	TIntStruct m_tVarInfo;				//分析变量选择信息
	double m_dValue;					//检验值。
	double m_dConfidence;					//置信度

	struct OneSampleResult
	{
		double m_TestStatistics;//检验统计量的值
		int  m_df;//自由度（如果有的话）
		int  m_df2;//自由度2（如果有的话）
		bool m_bSingle; //0－双边，1－单边
		double  m_P;//p值
		CTString  m_strName;//变量名称
	};
	CTArray<OneSampleResult,OneSampleResult> m_OneSampleResult;
private:
	//********************************
	//备择假设
	bool m_bNE ;// H1 : u ≠ u0
	bool m_bGT ;// H1 : u ＞ u0
	bool m_bLT ;// H1 : u ＜ u0
	bool m_bEquation; //方程法求比例的置信区间
	
	//根据方差是否已知输出表格
	void CreateTestTable(int index);
	void CreateChiTestTable(int index);
	void CreatePropotionTestTable(int index);
	//检验类型
	int m_nVarType;//0:方差未知(t检验)  1:方差已知 (U检验)
	int m_nCheckType;//输出类型，上限，下线等。
	int m_nIsSTD;//0:方差  1:标准差
	double m_dErrValue; //方差或者标准差的值
	
	bool m_bNormalTest;//样本差正态性检验
	bool m_bHistogram;  //直方图
	bool m_bNormalLine; // 正态概率图
	int  m_nDivi;       // 分区数
	//********************************/
private:
	CDataInterface * m_pDataInterface;//数据接口指针
//	CWnd           * m_pWnd;			//输出窗口指针
	CResult		   * m_pResult;			//输出结果集
	CRsltElementText *m_pRsltElementText;//出错信息记录
	bool m_bWeight;						//是否指定了权重
	int m_nWeight;						//权重列的下标
	CTArray<int,int> m_RowArray;			//被删除不合法行记录
	int m_nFieldIndex	;				//当前字段记录

	//用于统计表
	CTStringArray m_NameStrAr;			//变量名
	CTStringArray m_FrequencyStrAr;		//频数
	CTArray<double,double> m_MeanAr;		//均值
	CTArray<double,double> m_StdDAr;		//标准离差
	CTArray<double,double> m_StdEMAr;	//均值标准误

	//用于检验表
	CTArray<double,double> m_tAr;		//t
	CTArray<double,double> m_uAr;		//u统计量
	CTStringArray m_dfStrAr;				//自由度
	CTArray<double,double> m_PAr;		//显著性
	CTArray<double,double> m_DFAr;		//均差
	CTArray<double,double> m_LowerAr;	//下限
	CTArray<double,double> m_UpperAr;	//上限

	CTArray<double,double> m_ChiSquareAr;	//卡方结果上限



	CPtrArray m_ErrorAry;//残差
public:	
	CResult * COneSmplTTest::OnOneSmplTTest(CDataInterface *pDataInterface, string VO);//单样本Tjiantyan接口函数
	//1、比例的单样本检验
	CResult* COneSmplTTest::OnOneSmplPropotionTest(CDataInterface *pDataInterface, string VO);
	//2、方差的单样本检验
	CResult* COneSmplTTest::OnOneSmplVarianceTest(CDataInterface *pDataInterface, string VO);
	//添加假设检验的种类。
	double GetUStat(double U0,double Xmean,double sigma,int W); //方差已知的时候，获取U统计量

	bool Test(CTArray<double,double> &Data,//数据
							  CTArray<double,double> &Weight,//权重
							 CTString strName,int method =0);//字段名 T检验：method =0，卡方检验：method =1;比例检验:method =2 
private:
	BOOL GetValueFromVoT(CDataInterface *pDataInterface, string VO);
	BOOL GetValueFromVoPro(CDataInterface *pDataInterface, string VO);
	BOOL GetValueFromVoVar(CDataInterface *pDataInterface, string VO);
	void FreeMembers();//删除字符串变量
	void CreateResult(int method=0);//格式化输出T检验：method =0，卡方检验：method =1;比例检验:method =2
	void CreateCharts();//输出图表
};

#endif // !defined(AFX_ONESMPLTTEST_H__6CB2C14D_73BD_466D_8BB9_543630921336__INCLUDED_)
