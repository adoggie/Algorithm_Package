// SingleRuleReduction.h: interface for the CSingleRuleReduction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLERULEREDUCTION_H__3C007342_E3BD_41C8_9DC7_B9F1EED4AE84__INCLUDED_)
#define AFX_SINGLERULEREDUCTION_H__3C007342_E3BD_41C8_9DC7_B9F1EED4AE84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Association.h"


// #include "PageParamSet.h"
// #include "PageFResult.h"

//#define WM_CALCULATE_OVER WM_USER+1000

//
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

struct TUnit
{
	CPtrArray arIndex;
	int count;
	TUnit()
	{
		count = 0;
	}
	~TUnit()
	{
		for(int i=0;i<arIndex.GetSize();i++)
		{
			void * pt = arIndex.GetAt(i);
			//delete pt;
			//pt = NULL;
		}
		arIndex.RemoveAll();
	}
};

// class CDlgProgress;

//频繁集的集合
struct TFreSet {
	int			nSetCount;//频繁集数
	CPtrArray	arSet;//频繁集集合
	int			nFreq;//频繁集
};

//针对每一个字段的数值
struct TFeildValue {
	int		nIndex; //字段的索引
	union Variant{
		bool       bVal;
		int        nVal;
		double     dblVal;
		char  *    strVal;
	};
	Variant	val;//字段的数值
};

//强规则的描述
struct  TRuleDmn{
	CPtrArray	arSource;//源数据TFeildValue
	CPtrArray	arDestion;//推导出目标数据TFeildValue
	double		dbSupport;//支持度
	double		dbBelief;//置信度
};



/*
//预测模型的参数
typedef struct tagStructRuleModel
{
	CRuleTrainVO		*pRuleVO;//参数的输入
	CPtrArray	arFreSet;//频繁集的数据TFreSet
	CPtrArray	arRuleDmn;//关联规则的强规则TRuleDmn
}StructRuleModel;
*/


class AFX_EXT_CLASS CSingleRuleReduction//  : public CObject
{
public:
	void Abort();
	void Serialize(TCLFile &file);
	CResult * OnSingleRuleReduction(CDataInterface *pDataInterface);
//	void Assess(CDataInterface * pDataInterface,int ,int);
//	void Assess(CDataInterface *pDataInterface,CDWordArray& nObjAry,CDWordArray& nIpuAry);
	bool	CreatePredModel(CRuleOutput *&pOutStruc);
	void	SetErrInfo(CTString FormmatStr, CResult *& pResult);
	
	bool	SetTrainVO(string strVO);
	string	ExportModel();
	void	SetTrainDataParam(CRuleTrainVO *pVO);
	void	SetPredDataParam(TPredVO *pVO);
	void	SetPredModel(string strModel);
	void	SetDataSource(CDataInterface *pDataInterface, CPtrArray *parField);
// 	void Assess();
	CResult *Run(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO);

	CResult *RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO,string &strModel);
	CResult* RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO, string strModel);
	void Assess();
	int m_nDependIndex;

	CPtrArray m_tVarInfo;
	double m_dSupport;
	bool m_bSingle;
	bool m_bStopWork;
	bool m_bSort;
	double m_dConfidence;
	int m_nCustomCol;
	int m_nProductCol;
	CTStringArray m_strInputField;
	CTStringArray m_strDestField;

	CSingleRuleReduction();
	CSingleRuleReduction(CTString& strProjectPath, bool bNewComponent);
	virtual ~CSingleRuleReduction();

protected:
	void WriteUnitData(TUnit * pUnit,char * pszTitel);
	void WritePtrData(CPtrArray * par,char * pszTitle,CResult * pResult = NULL);
	void WritePtrData(CPtrArray * par,char * pszTitle,bool *pStop,CResult * pResult = NULL);
	
protected:
	CResult * m_pResult;	
	int m_nCustomNum;

	void FreePtrArray(CPtrArray * par);

	CIntVector VectorSub(CIntVector vect1,CIntVector vect2);
	int GetSubSetCount(CPtrArray * par,CIntVector vect);
	void Confidence(CPtrArray * par,double mini_confidence,CResult * pResult = NULL,bool * pbStop = NULL);
	CIntMatrix GetData(CDataInterface * pDataInterface);
	inline bool IsInSet(CIntVector vect,CPtrArray * parItem,int mk);
	inline bool FindNonFrequentSubSet(TUnit * pcandidateItem,CPtrArray * pSetLk,int nk,int mk);
	void InXthenCount(CIntMatrix matrx,CIntVector vector,CPtrArray * parCk);
	CIntMatrix GetNumOfCk(CPtrArray * par);
	bool Aproiri_gen(CPtrArray * par , int k,int mark,CPtrArray * parCk/*,void * lp = NULL*/,bool * pStop = NULL);
	CDataInterface * m_pDataInterface;
	bool SingleRulereduction(CIntMatrix matrix,int miniSupport);

private:
	CRuleTrainVO*	m_pRuleVO;
};

#endif // !defined(AFX_SINGLERULEREDUCTION_H__3C007342_E3BD_41C8_9DC7_B9F1EED4AE84__INCLUDED_)
