// QuickCluster.h: interface for the CQuickCluster class.
//					快速聚类分析
//          功能： 快速聚类算法实现
//          标注： 余卫军
//			时间： 2005年10月          							
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUICKCLUSTER_H__166656C2_E71A_4072_B93D_3E547DD1BC09__INCLUDED_)
#define AFX_QUICKCLUSTER_H__166656C2_E71A_4072_B93D_3E547DD1BC09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Classify.h"
#include <vector>

#include "../DataInterface/Define.h"
#include "../TLADVN/StdAfx.h"


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CQuickCluster : public CClassify
{
public:
	CShowData* m_pShow;
	bool Do();
	CResult * OnQuickCluster(CDataInterface * m_pDataInterface,std::string VO);
	CQuickCluster();
	virtual ~CQuickCluster();

	int			m_nNC;											//指定聚类的类的个数

private:
	void CreateInitResult();
	void CreateResult();
	bool GetValueFromVO(CDataInterface *pDataInterface, std::string VO);
	//void GetData(CDataInterface *pDataInterface, CPtrArray &rArray, int nGetType );
	void GetData(CDataInterface *pDataInterface, TCDataMatrix &rArray, int nGetType );

	//bool UpdateMean(int * pCluster);							//更新类中心
	bool UpdateMean(TIntAry* pCluster);	//更新类中心
	void CopyClusterIndex(TIntAry *x ,TIntAry *pSource,int nLen);		//拷贝各数据的归类索引		
	
	bool CompareCluster(int * x,int * y,int nLen);				//比较两次聚类的结果是否相同
	void		Iteration();									//循环聚类
	bool		AssignCasesToNearestCluster();					//将所有样品从头到尾处理一次到最近的类中
	bool		UpdateInitialClusterCenter(int pInitalCenter[]);//更新初始的类重心
	bool		InitalClusterCenter();							//初始化类中心
	void		Clear();										//内存释放函数
	bool        IsClusterChanged();
//	double		EucDistance(const int i,const int j);			//计算两类之间的距离
//	double		EucDistance(double * x,double * y,int nLen);	//计算两个向量之间的距离
//	double		GetMinDistance(int &x1,int &x2);				//求解类间最小的距离

	double		* m_pMNC;										//类中心的指针
	CDoubleMatrix	m_CenterOFCluster;							//类中心的矩阵(一个类一列)
	CDoubleVector	m_DisPointToCenter;							//各个类到中心点的距离总和
	CIntVector		m_DistanceToshortestCluster;				//两两中心点之间得最短距离
	double		* m_pWeightCases;								//权重数据
	int			m_nValidWeightCases;							//合法的权重数
	//CPtrArray   m_aDataSet;										//数据列指针数组
	TCDataMatrix m_DataMatrix;

	TIntAry    m_IntCluster;                                    //标识该行所属的类号

	TIntAry   m_IntTempCluster;  //标识该行所属的类号,暂时存储，用于两次聚类的比较
	int         * m_pCount;										//标识每一类的包含元素的个数	
	double		* m_pClusterWeight;								//标识每一类的包含元素的加权总数
	int         m_nIterationNum;								//指定循环的次数
	bool		m_bCountFlag;									//指定循环次数的标志								
	int			m_nCountMax;
	CResult *	m_pResult;
	bool		m_bOutputCaseInfo;								//是否输出样本聚类信息
	double		* m_pPrevMNC;									//更新前的聚类中心
protected:
		
};

#endif // !defined(AFX_QUICKCLUSTER_H__166656C2_E71A_4072_B93D_3E547DD1BC09__INCLUDED_)
