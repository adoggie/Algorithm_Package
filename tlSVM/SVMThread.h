#if !defined(AFX_SVMTHREAD_H__AC0DA8D4_3F01_452C_BF81_F8DD25B3C741__INCLUDED_)
#define AFX_SVMTHREAD_H__AC0DA8D4_3F01_452C_BF81_F8DD25B3C741__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include "PageSVM.h"
// #include "Resource.h"
//#include "CSVM.h" 
// SVMThread.h : header file
//
//#include "iostream.h"
//#include "math.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../TLDBDll/Database/DataAccess.h"
#include "../DataInterface/DataInterface.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"
#include "../Public/FileReadWrite.h"
#define DLLEXPORT	__declspec(dllexport)

/////////////////////////////////////////////////////////////////////////////
// CSVMThread thread
//add by liytao 20030123


///*
//*算法和外界动态交互，是否需要输出反馈信息，是否终止
//*/
//struct Interaction
//{
//	bool bNeedFeedback;//输入：告诉算法是否需要写反馈信息。true：必须反馈，false：不用
//	bool bAbort;//输入：true终止当前程序。false：继续运行。默认false
//	char BufMsg[100];//输出：输出当前运行信息	
//};
//
///*
//输入的控制信息的基类，如哪些变量作为因变量，哪些作为自变量，结果集中哪些需要输出
//主要控制结果集显示问题，每一个算法都不一样。
//*/
//class VOBase{
//};
//
///*
//算法模型信息，主要用于算法训练时，输出的模型信息，用于下一次的预测。
//每一个算法都不一样。
//*/
//class StructBase{
//public :
//	string Export();//导出为XML方式
//	bool Import(string str);
//};
//
///*
//环境控制类，设置系统参数，以下方法为C算法内部调用的方法
//*/
//class CEnvParam{
//public:
//	//******  指定那些参数需要输入到参数中  *** 
//	bool HasOutParm(CTString sName);//判断是否需要输出某个参数，如yresid
//	string GetOutParamName(CTString sName);//
//	//*************设置环境变量************
//	bool SetVar(CTString sVarName,double value);
//	bool SetVar(CTString sVarName,CDoubleVector* value);//value 内存空间由算法自己释放。
//	bool SetVar(CTString sVarName,CDoubleMatrix* value);//value 内存空间由算法自己释放。
//	bool SetVar(CTString sVarName,CDataAccess* value);//value 内存空间由算法自己释放。
//};
//




//训练时从界面输入参数
//输入参数，主要是从界面输入参数
class CSVMTrainVO
{
public:
	CTStringArray strInputField;//对输入字段的定义，说明输入变量，目标变量
	CTStringArray strDecField;
	int		nCassifyMethod;//分类的种类：线性可分，线性不可分
	int		nKernalType;//核函数类型：高斯函数，多项式函数
	int		nSiggmaSquare;	//高斯函数的siggma平方
	int		nLaddCount;//多项式函数的阶数
	double  dPunishFactor;//惩罚因子
	double  dKKTFactor;//KKT因子
	double	dEPS;//Eps值
	CSVMTrainVO()
	{
		nCassifyMethod = 1;
		nKernalType = 0;
		nSiggmaSquare = 2;
		nLaddCount = 2;
		dPunishFactor = 2.0;
		dKKTFactor = 0.1;
		dEPS = 0.001;
	}
	bool Compile(string strVO)
	{
	//	支持向量机VO设置
	//	SVMTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/KERNALTYPE [{LINEAR**},{LINEARNOT}]
	//	/CLASSIFY [{GAUSS**},{POLYNOMIAL}]
	//	/CLASSIFYPAR SIGGMA(2**) LADDER(2**)
	//	/PARAM FACTOR(2**) KKT(0.1**) KPS(0.001**)

		AnaWord VOstr;
		VOstr.Import(strVO);

		nCassifyMethod = AnaSyntax::findstr("/CLASSIFY","POLYNOMIAL",VOstr) ? 1:0;
		nKernalType = AnaSyntax::findstr("/KERNALTYPE","LINEARNOT",VOstr) ? 1:0;

		if (AnaSyntax::findstr("/CLASSIFYPAR","SIGGMA",VOstr))
			nSiggmaSquare = AnaSyntax::getvalue("/CLASSIFYPAR","SIGGMA",VOstr);
		if (AnaSyntax::findstr("/CLASSIFYPAR","LADDER",VOstr))
			nLaddCount = AnaSyntax::getvalue("/CLASSIFYPAR","LADDER",VOstr);

		if (AnaSyntax::findstr("/PARAM","FACTOR",VOstr))
			dPunishFactor = AnaSyntax::getvalue("/PARAM","FACTOR",VOstr);
		if (AnaSyntax::findstr("/PARAM","KKT",VOstr))
			dKKTFactor = AnaSyntax::getvalue("/PARAM","KKT",VOstr);
		if (AnaSyntax::findstr("/PARAM","KPS",VOstr))
			dEPS = AnaSyntax::getvalue("/PARAM","KPS",VOstr);

		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputField);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDecField);
		return true;
	}
};

/*
//支持向量机的预测模型参数
typedef struct tagStructSVMModel{
	TSVMVO*		pSVMVO;//支持向量机的输入数据和参数
	CTString	strKernalFunc;//
	CDoubleMatrix	mtAlpha;//主要是一个矩阵，用于决策处理
}StructSVMModel;
*/
//##ModelId=3DD99A250127
class VarIndexs {
public:
	VarIndexs(){
		pVarIndexs=NULL;
		nTargetIndex=-1;
		nVarCount=0;
	}
	void Empty(){
		if( pVarIndexs!=NULL)
			delete []pVarIndexs;
		nVarCount=0;
	}
	
	int Find(int nIndex){
		if (nIndex<0)
			return -1;
		for( int i=0;i<nVarCount;i++)
			if (pVarIndexs[i]==nIndex)
				return i;
			return -1;
			
	}
	void operator =(VarIndexs &v)
	{
		nVarCount = v.nVarCount;
		pVarIndexs = v.pVarIndexs;
		nTargetIndex = v.nTargetIndex;
	}
	
	int *pVarIndexs;
	int nTargetIndex;
	int nVarCount;
	~VarIndexs(){
		//if( pVarIndexs)
		//	delete []pVarIndexs;
	}
};
//add end

class CSVM;
class DLLEXPORT CSVMThread //: public CWinThread
{
protected:
public:
	CSVMThread();           // protected constructor used by dynamic creation
	
	CSVM	*m_pSVM;
	CDataInterface *m_pDataInterface;
	CTStringArray	m_strInputField;
	CTStringArray	m_strDestField;
	CTString	m_ModelFile;//保存模型的路径
	int m_nRoleRs;

public:
	int CreateAccessStruct(CDataAccess *pSrcAccess,CDataAccess** pTargetAccess,VarIndexs* pVarIndexs);
	int GetVarIndexs(int **pVarFields,int *pTargetIndex);
	CDataAccess* PretreatData(CDataAccess* pDataAccess,int **pIndexs);
	bool CheckPredict(CDataAccess *pAccess);
	bool HaveFieldInvalid(CDataAccess *pAccess);
	bool HaveNullRec(CDataAccess *pAccess);
	void	SetDataSource(CDataInterface *pDataInterface,CPtrArray * fields);
	void	SetDataParam(CSVMTrainVO *pVO);
	CDataAccess* DelInvalidRec(CDataAccess *pDataAccess);
	void SetTrainVO(string strVO);
	bool SetPredVO(string strVO);
	int ExitInstance();
	CResult* RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,string VO);
	CResult* RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,string VO, string strModel);
	int Run();
	virtual ~CSVMThread();
	//模型
	virtual bool Export(string szFileName);
	virtual bool Import(string szFileName);
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVMTHREAD_H__AC0DA8D4_3F01_452C_BF81_F8DD25B3C741__INCLUDED_)
