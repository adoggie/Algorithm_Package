// TTest.h: interface for the CTTest class.
//               所有双样本的假设检验
//                  标注：余卫军 2005年10月20日
//					修改：马良庄 2006/03/28
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TTEST_H__FA5EF87B_8FC9_4BC5_97A2_AEE1BF3425A8__INCLUDED_)
#define AFX_TTEST_H__FA5EF87B_8FC9_4BC5_97A2_AEE1BF3425A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "tlcrnt.h"
class AFX_EXT_CLASS CTTest  
{
public:
	CTTest();
	virtual ~CTTest();
	//独立样本T检验
	CResult *CTTest::OnTTest(CDataInterface *pDataInterface, string VO);
	//双样本方差检验
	CResult *CTTest::OnDoubleVarTest(CDataInterface *pDataInterface, string VO);
	//双样本比例检验
	CResult *CTTest::OnDoublePropotionTest(CDataInterface *pDataInterface, string VO);
    //*************** 选项 ****************
	//方差齐次检验
private:
	//CTString  m_strIndex1;
	BOOL	m_bBartlett; //Bartlett检验
	BOOL	m_bF;//F检验
	BOOL	m_bLevene;//Levene检验
	BOOL	m_bGroup;//分组，不配对
	//备择假设
	BOOL	m_bGT;// H1 : u1 － u2 ＞ δ
	BOOL	m_bLT;// H1 : u1 － u2 ＜ δ
	BOOL	m_bNE;// H1 : u1 － u2 ≠ δ
    BOOL    m_bPooledData ;//是否使用混合的样本估计比例p
	double	m_dValue;//δ
    
	int		m_nCheckType;//0:方差未知(t检验)  1:方差已知 (U检验) 或是输出上限、下限
	
	int		m_nIsStdType;//0:方差   1:标准差
	double	m_dSigma1;//   
	double	m_dSigma2;//方差值 
	
	int		m_nIsVarNE;//0:方差相等 1:方差不等
	//*************************************

	TIntStruct m_tVarInfo;			//分析变量选择信息
	int m_nGroupIndex;				//分组变量信息
	double m_dValue1;				//分组值1
	double m_dValue2;				//分组值2
	int m_nGroupMethod;				//分组方式，0表示分组方式，1表示分界点方式。请参考界面
	double m_dBoundary;				//分组分式为1时，分界点的值。
	double m_dConfidence;

	CDataInterface * m_pDataInterface;//数据接口指针
	CResult		   * m_pResult;		//输出结果集
	CRsltElementText *m_pRsltElementText;//出错信息记录
	BOOL m_bWeight;					//是否指定了权重
	int m_nWeight;					//权重下标
	CTArray<int,int> m_RowArray;		//被删除不合法行记录
	int m_nFieldIndex;				 //当前字段索引

	//分组统计数据表数据存储
	CTStringArray m_NameStrAr;		//变量名
	
	CTStringArray m_GroupStrAr;		//分组标志
	CTStringArray m_FrequencyStrAr;	//频数
	CTStringArray m_MeanStrAr;		//均值
	CTStringArray m_StdDStrAr;		//标准离差
	CTStringArray m_StdMStrAr;		//均值标准误

	//检验结果统计数据表数据的存储
	//建议尽量不要使用字符串来处理，那样会损失精度的。
	CTStringArray m_CommentStrAr;	//表第二列说明
	
	CTStringArray m_LStrAr;	//Levene值
	CTStringArray m_LdfStrAr;	//Levene值 自由度
	CTStringArray m_PStrAr;	//显著性

	CTStringArray m_FStrAr;	//F值	
	CTStringArray m_FdfStrAr;//F值自由度
	CTStringArray m_FPStrAr;//显著性

	CTStringArray m_BdfStrAr; //bartlett 自由度
	CTStringArray m_BStrAr;	//Bartlett值
	CTStringArray m_BPStrAr;	//Bartlett值显著性
	 
	CTStringArray m_ZStrAr;	//U检验统计量值
	CTStringArray m_EtStrAr;	//T检验统计量值//方差相等时
	CTStringArray m_EtdefStrAr;	//方差相等时自由度

	CTStringArray m_UtStrAr;	//T检验统计量值//方差不等
	CTStringArray m_UtdefStrAr;//方差相等时自由度
	
	
	CTStringArray m_tStrAr;			//t值
	CTStringArray m_dfStrAr;			//自由度
	CTStringArray m_P2StrAr;			//显著性（双尾）
	CTStringArray m_MeanDStrAr;		//均值差
	CTStringArray m_StdEDStrAr;		//差的标准误
	CTStringArray m_LowerStrAr;		//置信下限
	CTStringArray m_UpperStrAr;		//置信上限
	CTString m_strPair;
protected:
	BOOL GetValueFromVoT(CDataInterface *pDataInterface, string VO);
	BOOL GetValueFromVoVar(CDataInterface *pDataInterface, string VO);
	BOOL GetValueFromVoPro(CDataInterface *pDataInterface, string VO);
	double GetVar(CTArray<double,double> &Data);
	void FreeMemebers();
	void CreatHomoTestTable(int index);//输出方差齐次检验
	void CreatHypoTestTable(int index);//输出架设检验表
	void CreatVarTestTable(int index);//输出方差的假设检验
	void CreatPropotionTestTable(int index);//输出比例的假设检验
	void CreatStatTable(int index);//输出方差为零的时候的统计表
	BOOL TestMain(CTArray<double,double> &Data1,		//组1数据
						  CTArray<double,double> &Data2,		//组2数据
						  CTArray<double,double> &Weight1,	//组1数据对应的权重
						  CTArray<double,double> &Weight2,	//组2数据对应的权重
						  int nIndex,                       //当前分析字段的列号
						  int method = 0 );                 //0-T检验，1- 方差检验，2-比例检验						

void GroupIntoTwo(CTArray<double,double> &ar1,				//组1数据
									 CTArray<double,double> &ar2,		//组2数据
									 CTArray<double,double> &Weight1,	//组1数据对应的权重
									 CTArray<double,double> &Weight2,	//组2数据对应的权重
									 double dValue1,double dValue2,		//分组值1，分组值2
									 int nVarCol,						//当前对应的分析列号
									 TDataSet *pWeightArray,			//存储了权重数据
									 TDataSet *pGroupArray);			//存储了分类变量数据

void CreateResult(int method = 0); //0-T检验，1- 方差检验，2-比例检验						

};

#endif // !defined(AFX_TTEST_H__FA5EF87B_8FC9_4BC5_97A2_AEE1BF3425A8__INCLUDED_)
