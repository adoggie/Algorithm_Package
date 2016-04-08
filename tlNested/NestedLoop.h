// NestedLoop.h: interface for the CNestedLoop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESTEDLOOP_H__7A0E0AA3_7A33_42EB_A681_F322F8EB283A__INCLUDED_)
#define AFX_NESTEDLOOP_H__7A0E0AA3_7A33_42EB_A681_F322F8EB283A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"

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





typedef struct NestedStruct
{
	int row;
	double value;
}NestedStruct;

typedef struct TMaxAndMin
{
	double max;
	double min;
	double mean;
}TMaxAndMin;

//孤立点分析，从界面输入的参数
class CNestedVO
{
public:
	CTStringArray	strInputName;//表明输入字段的信息
	int nDistance;//孤立点距离
	int nNum;//孤立点个数
	int nVarOrRec;//0是分离孤立记录，1是分离孤立字段（变量）
	CNestedVO()
	{
		nDistance = 1;
		nNum = 1;
		nVarOrRec = 0;
	}
	bool Compile(string strVO)
	{
	//	孤立点VO设置
	//	NESTEDRUN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/MISSMETHOD DELRECORD
	//	/WRAPDISTANCE DISTANCE(1)
	//	/NESTEDCOUNT COUNT(1)
	//	/METHOD [{DIVRCD**},{DIVFIELD}]		



		AnaWord VOstr;
		VOstr.Import(strVO);
		
		nVarOrRec = AnaSyntax::findstr("/METHOD","DIVFIELD",VOstr);
		
		nDistance = AnaSyntax::getvalue("/WRAPDISTANCE","DISTANCE",VOstr);
		nNum = AnaSyntax::getvalue("/NESTEDCOUNT","COUNT",VOstr);
		
		if (nDistance < 0)
			nDistance = 1;
		if (nNum < 0)
			nNum = 1;

		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputName);
		return true;
	}
};

/*
//孤立点分析，输出结果数据
typedef struct tagStructNestedModel
{
	TNestedVO	*pNestedVO;//界面输出参数
	CPtrArray	arNested;//孤立点的情况，并且孤立点的数据
}StructNestedModel;
*/

//孤立点分析，输出结果数据
//孤立点分析，输出结果数据
class CNestedOutput : public StructBase
{
public:
	CNestedOutput(){;}
	virtual ~CNestedOutput(){;}
	virtual bool Import(string szFileName);
	virtual	bool Export(string szFileName);

protected:
	CNestedVO	*pNestedVO;//界面输出参数
	CPtrArray	arNested;//孤立点的情况，并且孤立点的数据
};

class  CNestedLoop 
{
public:
	CNestedLoop();
	virtual ~CNestedLoop();	
	CDataInterface * m_pDataInterface;
	CResult *  RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,CNestedVO *VO);
	bool	SetDataParam(CNestedVO *pVO);
		
	bool m_bIsToEnd;
	int m_nVarOrRec;//处理规则是0按字段，还是1按记录
	CNestedVO *m_pNestedVO;
protected:
	CResult	* m_pResult;			
	CRsltElementText *m_pRsltElementText;

	TIntStruct m_tVarInfo;
	CPtrArray m_MaxAndMinPtrArray;
	int m_nDistance;
	int m_nSolarity;
protected:

	CResult * OnNestedLoop();
	void CreateResult(CDoubleMatrix W_matrix,CPtrList &outCPtrList);
	bool Nested(CDoubleMatrix W_matrix,int k,int n);
	double CalcDstij(CDoubleMatrix matrix,int i,int j);
	double GetDkDst(CDoubleMatrix matrix,int i,int k,double freshold,bool bTrans);
	CDoubleMatrix  CNestedLoop::GetData();
	double GetMaxAndMin(int nColIndex,double &max,double &min);
	bool OutIntType(int nCol);
	double GetMaxAndMin();
};

#endif // !defined(AFX_NESTEDLOOP_H__7A0E0AA3_7A33_42EB_A681_F322F8EB283A__INCLUDED_)
