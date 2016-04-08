// CMarkovChain.h: interface for the CMarkovChain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMarkovChain_H__2A9063D7_E809_4128_80C6_EFBB96AAA482__INCLUDED_)
#define AFX_CMarkovChain_H__2A9063D7_E809_4128_80C6_EFBB96AAA482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <time.h>

class CMarkovChain  
{
public:
	CMarkovChain();
	virtual ~CMarkovChain();
	CResult * OnCMarkovChain(CDataInterface *pDataInterface, string VO);//外部调用主函数
	CResult * OnMorkovChainPred(CDataInterface *pDataInterface, string VO);//外部调用预测函数
	
private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	BOOL GetPredValueVo(CDataInterface *pDataInterface, string VO);//读取预测的参数
	BOOL GetValueFromModel(CDataInterface *pDataInterface, string strModel);//从模型中取出数据
	BOOL Main();
	BOOL EMalgorithm(CDoubleVector& Pi_vec, CDoubleMatrix& Theta_Init_mx, 
		CDoubleMatrix* Theta_Trans, CDoubleMatrix& r,
		CDoubleVector start_vec, CDoubleVector length_vec, 
		CDoubleVector pairID_vec, int K, int cutting);
	void DrawResult();
	//--UNIX PORT
	#ifdef _UNIX
	unsigned int GetTickCount(){
		return (unsigned int)clock();
	}
	#endif

	//读取参数：时
	int m_nUserID;//ID的下标
	int m_nPath;//Path的下标
	int m_nTime;//
	int m_nCluster;//归的类数
	int m_nMaxIter;//最大迭代次数
	BOOL m_bAutoCluster;//自动计算K
	double m_dMinConverge;//收敛条件
	int m_nPageNum;//共有多少个网页类别
	BOOL m_bSaveModel;//是否保存模型
	CTString m_ModelFile;//保存模型的路径
	BOOL m_bSaveData;//是否用户指定保存数据文件路径及名字
	CTString m_DataFile;//用户指定保存数据文件路径及名字

	//读取参数：至
	
	//为编程方便定义的变量
	CDataInterface * m_pDataInterface;
	CDoubleMatrix * m_Theta_Trans;
	int m_nRow;//共有多少个数据
	int m_nTotalID;//共有多少个用户
	CDoubleVector m_ID_vec;//
	CDoubleVector m_Path_vec;//
	CIntVector m_VecClus;//用户分类结果
	CResult * m_pResult;
	string m_strModel;//保持模型(m_nCluster, m_nPageNum, m_nCluster个转移矩阵的值)
};

#endif // !defined(AFX_CMarkovChain_H__2A9063D7_E809_4128_80C6_EFBB96AAA482__INCLUDED_)
