// KMeans_Algorithm.h: interface for the CKMeans_Algorithm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KMEANS_H__A2D787BB_9521_4F24_8E32_4C6DD76BFF1A__INCLUDED_)
#define AFX_KMEANS_H__A2D787BB_9521_4F24_8E32_4C6DD76BFF1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
//快速聚类算法类
//从高级统计的快速聚类代码中提取，并经过改进，但速度比高级统计的快速聚类要慢
//可能是由于数据存储方式有区别，前者使用CDoubleMatrix，这里使用TCDataMatrix
//参看快速聚类文档和代码
//陈翔
//2005-11
//////////////////////////////////////////////////////////////////////////
#include <sstream>
#include <sys/timeb.h>

#include "../cpknl/ErrorReporter.h"
#include "../cpknl/Vector.h"
#include "../cpknl/cpknl.h"
#include "../cpknl/Matrix.h"


using namespace std;

class CKMeans_Algorithm  
{
private:
	int m_nEndCount; //算法迭代次数

public:
	CKMeans_Algorithm();
	virtual ~CKMeans_Algorithm();

public:	
	//KMeans_Algorithm外部调用接口，以后可加入输出信息：各样本属于哪个聚类中心
	bool KMeans(CDoubleMatrix& mx,int nCenterNum,bool bIsStd,double dPrecision,
		int nMaxRepeatTimes,CDoubleMatrix& mxCenter);
	//int GetEndCount();

private:
	bool UpdateClusterCenter(CDoubleMatrix& mx,CDoubleMatrix& mxOldCenter,
		CDoubleMatrix& mxCenter,CIntMatrix& mxSampleBelongCenterIndex);

//////////////////////////////////////////////////////////////////////////
//中止算法相关
private:
	bool m_bAbort;				//算法中止标志
	string m_strAbortInfo;		//记录算法中止信息
public:
	//中止KMeans方法运算。用于当本算法作为主程序的一个线程调用时，主程序可以调用此函数以中止KMeans
	void Abort();	
	string GetAbortInfo();		//返回算法中止时的运行状态信息
	bool IsAbort();				//调用KMeans后，用此方法判断算法是否是中止退出
//////////////////////////////////////////////////////////////////////////
//出错处理相关////////////////////////////////////////////////////////////
private:
	string m_strErrorInfo;		//记录算法出错信息
	bool m_bError;				//标记算法是否出错
public:
	bool IsError();				//获得算法是否出错
	string GetErrorInfo();		//获得算法出错信息	
//////////////////////////////////////////////////////////////////////////
//记录运行信息相关
private:
	string m_strRunInfo;		//记录运行信息
	string FormatTimeSpan(_timeb timeBegin,_timeb timeEnd);		//计算开始时刻到结束时刻之间的耗时
public:
	string GetRunInfo();		//获得调用KMeans之后的运行信息，包括运行时间等
//////////////////////////////////////////////////////////////////////////
};

#endif // !defined(AFX_KMEANS_H__A2D787BB_9521_4F24_8E32_4C6DD76BFF1A__INCLUDED_)
