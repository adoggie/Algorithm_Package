// AAnova.h: interface for the CAAnova class.
//                单因素方差分析
//          标注： 余卫军 2005年10月21日
//			修改：马良庄	2006/03/22
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AANOVA_H__BB256B39_09F2_4314_B428_8F7AE456473F__INCLUDED_)
#define AFX_AANOVA_H__BB256B39_09F2_4314_B428_8F7AE456473F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "tlcrnt.h"
class AFX_EXT_CLASS CAAnova //: public CStatMethod  
{
public:
	CAAnova();
	~CAAnova();
	//一元方差分析的主函数，返回两个结构，供不同的用户使用。
	CResult* OnAnova(CDataInterface *pDataInterface, string VO);
//			struct OneWayANOVAResult 
//			{
//				double m_SquareSum[3];//平方和，依次为 组间、组内、总体
//				int    m_Df[3];		  //自由度，依次为 组间、组内、总体
//				CTString m_strVarName;//变量名称
//			};
//
//			//对外输出结构，提供对外的数据接口，实现关联计算的难题。
//			CTArray<OneWayANOVAResult,OneWayANOVAResult> m_OneWayANOVAResult;

private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	int OneAnova(CDoubleVector& y, CIntVector& x, CDoubleVector& right);
	void OnAAnova(CDataInterface *pDataInterface);
	void OnDescriptive_Plot(CDataInterface* pDataInterface,CResult* &pResult);
	//生成结果
	void CreateResult(CDataInterface *pDataInterface,CRsltVariable* &pVariable,int nIndex,CTString strFactName);
	//生成结果集
	CRsltElementTable* CreateDescriptiveTable(CTString strName,CTString strFactName,int idx);
	//描述数统计图
	CRsltElementTable* CreateCompareMeansTable(CTString strName,CTString strFactName,int idx);//输出两两比较的表格
	
	CRsltElementChart* CreateMeadPlot(CTString strDepName,CTString strFactName,int idx);
	//残差图
	CRsltElementChart* CreateResiduPlot(CTString strDepName,CTString strFactName,int idx);
	CRsltElementChart* CreateRPPlot(CTString strDepName,CTString strFactName,int idx);
	//残差正态概率图
	CRsltElementChart*  CreateRNormlot(CTString strDepName,CTString strFactName,int idx);
	//直方图
	CRsltElementChart*  CreateHistogram(CTString strDepName,CTString strFactName,int idx);
	void GetData(CDataInterface* pDataInterface);

	BOOL       m_bDescriptive;
    BOOL       m_bMeanPlot;  //是否画均值图
	int        m_nFactorIndex;//因素变量
	int        m_nRightIndex;//权值变量
	int        m_nMissingSelect;
	TIntStruct m_tVarInfo;//存放已选中的变量
	CRsltElementText *m_pRsltElementText;
	/********************************************************
	 *	添加方差同一性检验的统计量的计算
	 作者 ：余卫军 
	 时间2005年5月19日 
	 *********************************************************/
	//输入参数 Y 为观测变量或者样本集。X 为分组变量，根据X的值对Y进行分组。
	//这里就是根据 取出满足X == Xval的下标对应的Y值，作为一个样本，
	//参与样本方差的比较。允许各个样本的观测个数不等，但是至少都得大于1，
	//小于或者等于1的样本没有方差的概念，将从比较的对象中剔除出去。
	//返回在X中找不到指定的Xval值的个数，如没有找到，返回 0 表示失败。
	//同时将下标存放到向量Result中。
	int FindIndex(CDoubleVector Y ,CIntVector X , double Xval,CDoubleVector& Result);
	//根据下标向量获取子向量.
	CDoubleVector GetSubVect(CDoubleVector Y ,CDoubleVector IndexVect);
	CDoubleVector GetIndexVal(CIntVector X);//获取向量中不同数值组成的向量并且返回
	//获取均值（Levene test方法中需要用到）
	//index = 0 返回Y的 算术平均值
	//index = 1 返回Y的 中位数
	//index = 2 返回Y的 上下截尾percent/2%的样本后的均值
	double  GetpercentVal(CDoubleVector Y ,double percent);	
	double  GetMean(CDoubleVector Y, int index , double percent);
	double  TrimMean(CDoubleVector Y, double percent);//获取截尾的均值
	double  GetBStat(CDoubleVector Y,CIntVector X,int &f);//计算BartLett test 统计量
	double  GetBStat(CDoubleVector* DataSet,int Count, int&f);//计算BartLett test 统计量
	//计算Levene test 统计量，默认使用中位数，若截尾的话，默认为10%
	double  GetLStat(CDoubleVector Y,CIntVector X, int &f,int index = 1 , double percent  = 10);  
	double  GetLStat(CDoubleVector* DataSet , int Count,int& f, int index, double percent);
	CDoubleMatrix GetMeansTableStat(CDoubleVector* DataSet,int count);
	double  medium(CDoubleVector Y); //获取中位数
	//获取残差数据，同时返回相应的预测值
	CDoubleVector GetResidu(CDoubleVector Y , CIntVector X,CDoubleVector& Residual,int index );//计算残差 0-常数模型残差，1-效应模型残差；
	//整理数据，将数据存入一个向量组中，以便后面分析使用。返回的是得到的分组数据的组数。
	int GetCleanData(CDoubleVector Y, CIntVector X,CDoubleVector*& ValidData,CDoubleVector& ValidIndex);
	//数据对排序，即同步的
	void SortData(CDoubleVector Y, CIntVector X, CDoubleVector& SortedY, CIntVector& SortedX,int index);
	CDoubleVector GetNormMedians(int n);//计算正态概率中位数 具体算法参见1.3.22
	CDoubleVector GetNormMedians(double mu,double Std,int n);//计算正态概率中位数 mu, std ,3.22
	double GetFStat(CDoubleVector X1,CDoubleVector X2 );//计算F统计量，检验两个独立样本的方差是否相等
	double GetTStat(CDoubleVector X1,CDoubleVector X2,BOOL Equel = true );//计算T统计量，比较两个样本的均值，bEqual = 0方差不相等相等
	double GetLeveneStat(CDoubleVector X1,CDoubleVector X2);//计算两个样本的levene统计量
	int GetStatList(CDoubleVector &Vect_Ret, CDoubleVector X1,CDoubleVector X2,BOOL bEqual);
	void	Testify(CDoubleVector& Y, CIntVector& X);//测试计算的正确定
	CPtrArray       m_arrayResult;
   	CTLTable* briefrTable;
	CDataInterface * m_pDataInterface;

	//存储缺失值处理后的数据.	
	CPtrArray       m_ptrArrayX;
	CPtrArray       m_ptrArrayY;
	CPtrArray       m_ptrArrayRight;
	//存储选项设置变量的值
	//方差同一性检验
	BOOL m_bBalaiteTest;//巴特莱特检验
	BOOL m_bLeveneTest;//乐夫尼检验 leveneTest
	double m_dTailPer;//截尾百分数
	int m_nArithmetic;//0:算术平均值	1:中位数  2:截尾均值
	
	//图表输出
	BOOL m_bResidualScatter;//残差散点图
	BOOL m_bResidualTable;//残差正态概率图
	BOOL m_bHistogram;    //直方图
	BOOL m_bVars;	//标记方差是否存在或者有效（>0）
	
	//均值两两比较
	BOOL	m_bVarianceEQ;//方差相等
	BOOL	m_bVarianceNE;//方差不相等
	BOOL	m_bMeansCompare;////均值两两比较
	//方差齐性检验
	int		m_nVarianceCheck;//0－F检验 1－Levene统计量
	/**********************************************************
	 *	方差同一性检验功能的添加结束，结果测试正确
	 **********************************************************/
};   

#endif // !defined(AFX_AANOVA_H__BB256B39_09F2_4314_B428_8F7AE456473F__INCLUDED_)
