// Clasify.h: interface for the CClassify class.
//                        聚类分析的基本类
//                    功能：各种聚类算法的共同方法和属性
//                    标注：  余卫军
//					  时间：2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLASSIFY_H__ABBD2DBF_5787_457B_9B04_A318775315F5__INCLUDED_)
#define AFX_CLASSIFY_H__ABBD2DBF_5787_457B_9B04_A318775315F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

//#include "Factor.h"
#include "AlgorithmBase.h"

struct TClassifyGroup
{
	int nIndex1;
	int nIndex2;
	double dbCoeff;
	TClassifyGroup()
	{
		memset(this,0,sizeof(TClassifyGroup));
	}	
};

//聚类分析
class AFX_EXT_CLASS CClassify : public CAlgorithmBase  
{
public:
	CResult * OnClassify(CDataInterface *pDataInterface, std::string VO);
	int m_nFomlizeMethod;
//	0－组间连接法(Average Linkage between Group)
//	1－组内连接法(Average Linkage within Group)
//	2－单个连接(Single Linkage)
//	3－完全连接(Complete Linkage)
//	4－重心聚类法(Centroid Method)//聚类的方法序号
	int m_nClusterMethod;
	int m_nAbsent;										//存在代表的数字
	int m_nPresent;										//不存在代表的数字
	int m_nRoot;										//开方的次数
	int m_nPower;										//power的次数
	bool m_bSimulate;
	CClassify();
	virtual ~CClassify();
	bool Formalize(const int nIndex);					//标准化矩阵
	bool ClusterMeasures(int nMethod,int nIndex);		//测度矩阵
	void GeneralProcedure();							//循环聚类
	TIntStruct m_tVarInfo;


private:
	int m_nDistance;
	bool GetValueFromVO(CDataInterface *pDataInterface, std::string VO);//从VO字符串中读取控制参数
	double * GetData( CDataInterface * pDataInterface, TIntStruct& tVarInfo, int& nCaseNum );
	void CreateResult();
	void UpdateMean(const int nindex);
	void ProcessIndex(const int nIndex);
	void Free();
	void UpdateSimulateMatrix(const int p,const int q);	//更新元素
	int m_nCurRow;
	double * SimulateAdd(int nRow,int nCol);
	double SimulateData(int nRow,int nCol);
	void ClusterMethod(int nIndex,int p,int q);			//聚类
	void Binary(int nIndex);							//二值测量
	void Count(int nIndex);								//记数测量
	void Interval(int nIndex);							//区间测量
	void OutputIcicle();								//输出冰柱图
	void Swap(TClassifyGroup *p1, TClassifyGroup *p2);      //交换内容
	void QSortArray(CPtrArray *pArray, int nLow, int nHigh);//快速排序

	double * m_pSimulate;
	double * m_pN;
	double * m_pSum;
	int * m_pIndex;
	int * m_aRecNo;										//保存数据记录号
	bool m_bOutputIcicle;								//是否输出冰柱图
	bool m_bWeightSet;									//是否设置了权重
	bool m_bCaseCluster;								//是否对样本聚类
	bool m_bAbort;										//用户是否取消了运行
	//bool m_bOutputCaseInfo;								//未知用途
	CPtrArray m_arClassifyResult;
protected:
	void CalNodeYPos(PedigreeTreeNode &node,CPtrArray &leafAry);
	int m_nOutChartSel;
	bool m_bOutputPedigree;
};

#endif // !defined(AFX_CLASSIFY_H__ABBD2DBF_5787_457B_9B04_A318775315F5__INCLUDED_)
