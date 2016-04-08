// RBFN_Algorithm.h: interface for the RBFN_Algorithm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RBFN_ALGORITHM_H__6B36D291_F0F5_48E2_AFAF_995CCC96A331__INCLUDED_)
#define AFX_RBFN_ALGORITHM_H__6B36D291_F0F5_48E2_AFAF_995CCC96A331__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
//RBF神经网络算法类（多输入单输出）
//纯计算，与界面无关。今后做任何改动都需维持算法包的纯粹性，并加强算法包的自制性
//算法包与界面传递数据时可以在算法包的外部制作一个类,用于传递符合界面需要的数据
//算法包包括了错误处理信息、中止控制信息、运行记录信息
//by 陈翔
//2005-11
//////////////////////////////////////////////////////////////////////////
#include <sys/timeb.h>
#include <string>
#include <sstream>

#include "../cpknl/ErrorReporter.h"
#include "../cpknl/Vector.h"
#include "../cpknl/cpknl.h"
#include "../cpknl/Matrix.h"

#include "../Public/TCLFile.h"
#include "../Result/Result.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"



#include "KMeans_Algorithm.h"

using namespace std;


class CRBFNetVO
{
public:
	int				nCntrCnt;
	CTStringArray	strInputName;
	CTStringArray	strDestName;
	CRBFNetVO()
	{
		nCntrCnt = 10;
	}
	bool	Compile(string strVO)
	{
	//	RBF神经网络VO设置
	//	BBFNETTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/HIDENCENTER = CENTER(10**)

		AnaWord VOstr;
		VOstr.Import(strVO);
		if (AnaSyntax::findstr("/HIDENCENTER","CENTER",VOstr))
			nCntrCnt = AnaSyntax::getvalue("/HIDENCENTER","CENTER",VOstr);
		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputName);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDestName);
		return true;
	}
};


class CRBFNetOutput : public  StructBase
{
public:
	CRBFNetVO	*pRBFNetVO;

	CDoubleMatrix mxInputColMaxMin;
	CDoubleMatrix mxTargetColMaxMin;
	//RBF网络隐含层各神经元坐标，利用快速聚类获得
	CDoubleMatrix mxStdCenter;
	//RBF基函数的宽度
	CDoubleMatrix mxSigmaSquare;
	//RBF网络隐含层到输出层的权值
	CDoubleMatrix mxWeight;	
	CRBFNetOutput();
	virtual bool Export(string szFileName);
	virtual bool Import(string szFileName);
	
};

//RBF神经网络结构
struct RBFNetStructure
{
	//输入数据列数
	int nInputCol;
	//目标数据列数，固定为1
	int nTargetCol;
	//样本数据聚类中心个数，即RBF网络隐含层神经元个数，需预先指定
	int nCenterNum;
	//原始数据各列最大值最小值(2行，分别存储最大和最小)
	CDoubleMatrix mxInputColMaxMin;
	CDoubleMatrix mxTargetColMaxMin;
	//RBF网络隐含层各神经元坐标，利用快速聚类获得
	CDoubleMatrix mxStdCenter;
	//RBF基函数的宽度
	CDoubleMatrix mxSigmaSquare;
	//RBF网络隐含层到输出层的权值
	CDoubleMatrix mxWeight;	
};

class CRBFN_Algorithm
{
private:
	RBFNetStructure m_RBFNS;	//网络结构
	bool m_bHasNetStructure;	//标记是否有网络结构
	CKMeans_Algorithm m_KMeans;	//用于和快速聚类算法进行信息交互

public:
	CRBFN_Algorithm();
	virtual ~CRBFN_Algorithm();


	//RBF网络训练，得到输入样本的RBFN网络输出和网络结构
	bool Train(CDoubleMatrix& mxInput, CDoubleMatrix& mxTarget, int nCenterNum, 
		CDoubleMatrix& mxOutput, double& dMSE, double& dStdMSE);	
	//RBF网络测试
	bool Test(CDoubleMatrix& mxTestInput, CDoubleMatrix& mxTestTarget, 
		CDoubleMatrix& mxTestOutput, double& dMSE, double& dStdMSE);
	//RBF网络预测
	bool Predict(CDoubleMatrix& mxPreInput, CDoubleMatrix& mxPreOutput);
	//RBF网络保存至string//可考虑重载"<<"
	bool SaveRBFNS(string& strRBFNS);
	//RBF网络从string载入
	bool LoadRBFNS(string& strRBFNS);
	//获得RBF网络结构
	RBFNetStructure GetRBFNS();
	//是否存在网络结构
	bool HasNetStructure();
	//模型
	bool Export(TCLFile &theFile);
	bool Import(TCLFile &theFile);
//////////////////////////////////////////////////////////////////////////
//错误处理相关
private:
	string m_strErrorInfo;		//记录算法出错信息
	bool m_bError;				//标记算法是否出错
public:	
	string GetErrorInfo();		//获得算法出错信息
	bool IsError();				//返回m_bError	
//////////////////////////////////////////////////////////////////////////
//中止算法相关
private:
	bool m_bAbort;				//算法中止标志
	string m_strAbortInfo;		//记录算法中止信息
public:
	//中止网络训练。用于当本算法作为主程序的一个线程调用时，主程序可以调用此函数以中止训练
	//更好的方法是用纯C++的对象消息响应机制，新的C++标准支持此机制
	void Abort();	
	string GetAbortInfo();		//返回算法中止时的运行状态信息
	bool IsAbort();				//返回m_bAbort
//////////////////////////////////////////////////////////////////////////
//记录运行信息相关
private:
	string m_strRunInfo;		//记录运行信息
	string FormatTimeSpan(_timeb timeBegin,_timeb timeEnd);		//计算开始时刻到结束时刻之间的耗时
public:
	string GetRunInfo();		//获得运行信息，包括运行时间等
//////////////////////////////////////////////////////////////////////////

private:
	//矩阵各列向量标准化
	bool MxStandardize(CDoubleMatrix& mx, CDoubleMatrix& mxColMaxMin, 
		CDoubleMatrix& mxStd, bool bHasMaxMin);
	//矩阵各列向量反标准化
	bool MxInverseStandardize(CDoubleMatrix& mxStd, CDoubleMatrix& mxColMaxMin,
		CDoubleMatrix& mx);
	//快速聚类获得隐含层中心
	bool GetHideCenter(CDoubleMatrix& mxStd, int nCenterNum, CDoubleMatrix& mxStdCenter);
	//计算隐含层输出
	bool CalculateHideOut(CDoubleMatrix& mxStdInput, CDoubleMatrix& mxStdCenter, 
		CDoubleMatrix& mxSigmaSquare, CDoubleMatrix& mxHideOut);
	//由聚类中心计算SigmaSquare（参考Clementine 8.0 Algorithms	Guide）
	bool CalculateSigmaSquare(CDoubleMatrix& mxStdCenter,CDoubleMatrix& mxSigmaSquare);
	//计算权值,mxY=mxX*mxWeight
	bool CalculateWeight(CDoubleMatrix& mxX,CDoubleMatrix& mxY,CDoubleMatrix& mxWeight);

	//改写自岭回归算法，返回回归系数，速度慢，暂不使用
	CDoubleMatrix RidgeEstimate(CDoubleMatrix &Matrix_X,CDoubleMatrix &Matrix_y, double dkValue);

	//计算MSE(mean of squared errors)
	double MSE(CDoubleMatrix& mxTarget, CDoubleMatrix& mxOutput);
};

#endif // !defined(AFX_RBFN_ALGORITHM_H__6B36D291_F0F5_48E2_AFAF_995CCC96A331__INCLUDED_)
