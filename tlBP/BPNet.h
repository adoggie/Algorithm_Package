// BPNet.h: interface for the CBPNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPNET_H__09519FC4_8791_4C5E_A0C3_2AA1A0A81B87__INCLUDED_)
#define AFX_BPNET_H__09519FC4_8791_4C5E_A0C3_2AA1A0A81B87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MemoStream.h"
#include "../Public/TString.h"
#include "Matrix.h"
#include "BP.h"
#include "../Result/Result.h"
#include "../cpknl/cpknl.h"
#include "../DataInterface/DataInterface.h"
const int nHugeData=50000;
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"

#include "../arithInterface/EnvParam.h"
#include <string>

#define DLLEXPORT	__declspec(dllexport)


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




//界面的输入参数，用于训练用
class  CBPNetTrainVO
{
public:
	int			nPartMethod;//划分方法
	int			nMaxTrainCount;//最大训练次数
	int			nPercent;//训练样本所占的百分数
	int			nOuputType;//输出类型
	int			nTrainCaseCount;//训练样本数
	int			nRideCellCount;//隐含神经元数
	double		dRunTime;//运行时间
	double		dSystemPrecision;//系统精度
	double		dLearnedRate;//学习率
	CTStringArray	strInputNames;//TFieldInfo，表明输入字段的信息
	CTStringArray	strDecNames;
	CBPNetTrainVO()
	{
		nMaxTrainCount = 100;
		nOuputType = 0;
		nPartMethod = 0;
		nPercent = 80;
		nTrainCaseCount = 0;
		nRideCellCount = 4;
		dRunTime = -1.0;
		dSystemPrecision = 0.001;
		dLearnedRate = 0.1;
	}
	bool Compile(std::string strVO)
	{
	//BP神经网络VO设置
	//	BPNETTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD [{DELRECORD**},{AVEFILL}]
	//	/SAMPLEMODE [PERCENT(80**),FRONTSAMPLE(1000**)]
	//	/SYSPREC = PRECISION(0.001**)
	//	/TRAINTIME = TIME(100**)
	//	/LEARNRATE = LEARNED(0.1**)
	//	/HIDENCOUNT = HIDEN(4**)
	//	/MAXRUNTIME = [{10MIN},{1HUR},{2HUR},{7HUR},{1DAY},{2DAY},{7DAY},{INF**}]
	//	/OUTTYPE [{CALCULATE**},{CLASS}]

		AnaWord VOstr;
		VOstr.Import(strVO);
		
		nPartMethod = AnaSyntax::findstr("/SAMPLEMODE","PERCENT",VOstr) ? 0: 1;
		if (!nPartMethod)
		{
			if (AnaSyntax::findstr("/SAMPLEMODE","PERCENT",VOstr))
				nPercent = AnaSyntax::getvalue("/SAMPLEMODE","PERCENT",VOstr);
		}
		else
		{
			if (AnaSyntax::findstr("/SAMPLEMODE","FRONTSAMPLE",VOstr))
				nTrainCaseCount = AnaSyntax::getvalue("/SAMPLEMODE","FRONTSAMPLE",VOstr);
		}

		if (AnaSyntax::findstr("/SYSPREC","PRECISION",VOstr))
			dSystemPrecision = AnaSyntax::getvalue("/SYSPREC","PRECISION",VOstr);
		if (AnaSyntax::findstr("/LEARNRATE","LEARNED",VOstr))
			dLearnedRate = AnaSyntax::getvalue("/LEARNRATE","LEARNED",VOstr);
		if (AnaSyntax::findstr("/HIDENCOUNT","HIDEN",VOstr))
			nRideCellCount = AnaSyntax::getvalue("/HIDENCOUNT","HIDEN",VOstr);
		nOuputType = AnaSyntax::findstr("/OUPUTTYPE","CALCULATE",VOstr) ? 0 : 1;
		if (AnaSyntax::findstr("/TRAINTIME","TIME",VOstr))
			nMaxTrainCount = AnaSyntax::getvalue("/TRAINTIME","TIME",VOstr);

		if (AnaSyntax::findstr("/MAXRUNTIME","10MIN",VOstr))
			dRunTime = 10 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","1HUR",VOstr))
			dRunTime = 1 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","2HUR",VOstr))
			dRunTime = 2 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","7HUR",VOstr))
			dRunTime = 7 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","1DAY",VOstr))
			dRunTime = 1 * 24 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","2DAY",VOstr))
			dRunTime = 2 * 24 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","7DAY",VOstr))
			dRunTime = 7 * 24 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","INF",VOstr))
			dRunTime = 0;
		
		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputNames);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDecNames);
		return true;
	}
};  

//界面的输入参数,预测模型输入参数
//typedef struct  tagTBPNetPredVO
//{
//	bool		bLoadSource;//是否导入到源文件中去
//	CTString	strNewFileName;//如果不到入源文件中，则需要新建一个文件用于存放预测的值，还有预测的一些参数
//	CPtrArray	arFieldInfo;//需要导入到新文件中去字段的信息
//	tagTBPNetPredVO()
//	{
//		bLoadSource = true;
//		strNewFileName = "";
//	}
//} TBPNetPredVO;  


/*
typedef struct tagStructBPNetModel
{
	TBPNetVO		*pBPVO;//从界面输入参数和字段
	CPtrArray	arWeight;//权值的矩阵，主要是有多少层就有多少个权值矩阵
}StructBPNetModel;
*/


struct MixSel
{
	int MixType;//0 ：百分比 1 前多少条记录
	int nValue;//对应的数值
};

struct TrainItem//修改时，注意外部void CPropBasic::OnOK() 
{
	long	nTrainTimes;
	double	dPrecision;
	double	dStep;
	double	dMaxRunTime;
	int nHideNum;//隐含层神经元数目
	int m_bCalc;//是否时计算模式还是分类模式
	double*pSortAry;//m_bCalc 分类时，数据
	int nClassCount;//m_bCalc 分类时，分类个数
	TrainItem (){
		nTrainTimes = 100;
		dPrecision = 1e-3;
		dStep = 0.1;
		dMaxRunTime = 0;
		m_bCalc = true;
		nHideNum = 4;
		pSortAry = NULL;
		nClassCount = 0;
	}
	//重载等号
	TrainItem& operator =(TrainItem&s)
	{
		Clear();
		nTrainTimes= s.nTrainTimes;
		dPrecision= s.dPrecision;
		dStep= s.dStep;
		dMaxRunTime = s.dMaxRunTime;
		nHideNum = s.nHideNum;
		m_bCalc= s.m_bCalc;//是否时计算模式还是分类模式
		nClassCount= s.nClassCount;
		if (s.nClassCount)
		{
			pSortAry = new double[s.nClassCount];
			memcpy(pSortAry,s.pSortAry,sizeof(double)*s.nClassCount);
		}
		return *this;
	}
	void Clear()
	{//清除所拥有的内存
		if (pSortAry )
		{
			delete []pSortAry;
			pSortAry = NULL;
		}
	}
	~TrainItem (){Clear();
	}
};

//字段描述,实现归一化等
class FieldDesc
{
	CTStringArray strArray;//序列化，则对应的号码为当前位置加1 (即从一开始)
public:
	CTString	strName;
	CFieldType nType;
	double dMin;
	double dMax;
	FieldDesc (){
	}
};


class FieldDescList
{
	vector<FieldDesc*> m_Array;
public:
	void RemoveAll();
	void Serialize(CMemoStream *pStream, bool IsStoring);
	FieldDescList (){};
	int GetSize(){ return m_Array.size();};
	FieldDesc* GetAt(int nIndex);
	bool SetAt(int nIndex,double dMin,double dMax);
	void Add(CTString strName,CFieldType nType,double dMin= 0,double dMax=0);
	~FieldDescList();

};	

//输出接口，用于循环计算
//用于模型预测
//class CBPNetOutput : public  StructBase
//{
//public:
//	CBPNetOutput(){;}
//	virtual ~CBPNetOutput(){;}
//	
//	virtual  string	Export();
//	virtual	 bool		Import(string str);
//public://训练过程中输入参数
//	CTStringArray	strInputNames;//TFieldInfo，表明输入字段的信息
//	CTStringArray	strDecNames;
//	int		nMaxTrainCount;//最大训练次数
//	int		nOuputType;//输出类型
//	int		nPartMethod;//划分方法
//	int		nPercent;//训练样本所占的百分数
//	int		nTrainCaseCount;//训练样本数
//	int		nRideCellCount;//隐含神经元数
//	double	dRunTime;//运行时间
//	double	dSystemPrecision;//系统精度
//	double	dLearnedRate;//学习率
//	TrainItem* pTrainItem;
//	FieldDescList* pInAry;
//	FieldDescList* pOutAry;
//	
//	//训练结果权值
//	int nComboArithmetic;
//	int nComboFunc;
//	double dSystemErrorOld;
//	double dSystemErrorNew;
//	CBPMatrix* pmxIToHWeight;
//	CBPMatrix* pmxHideThreshold;
//	CBPMatrix* pmxHToOWeight;
//	CBPMatrix* pmxOutThreshold;
//};
class DLLEXPORT CBPNet  
{
protected://数据源
	CDataInterface *m_pDataInterface; //要分析的数据源
	CBPMatrix	m_mxSour;//数据源，训练和模拟时用
	CMemoStream			m_NetWork;//存储网络结构
	FieldDescList		m_InAry;//存储输入变量名
	FieldDescList		m_OutAry;//存储目标变量名

protected:
	//训练过程中界面输入参数
	TrainItem			m_TrainItem;//选项
	
protected://预测结果参数
	//////////////////////////////////////////////////////////////////////////
	//预测用的参数，主要是对预测的数据进行写入文件中的过程,主要通过界面输入
	bool			m_bLoadSource;//是否导入到源文件中去
	CTString		m_strNewFileName;//如果不到入源文件中，则需要新建一个文件用于存放预测的值，还有预测的一些参数
	CTStringArray	m_arInputField;
	CTStringArray	m_arDestField;
	
protected:
	//规则
	CBP					m_Bp;//用于存贮训练的规则
	
protected:
	//过程控制和状态，参数输入
	CEnvParam*			m_pEnvParam;
	Interaction*		m_pInteraction;
	
public:
	std::string			m_strModel;//保存的模型
	CTString			m_ModelFile;//模型保存的路径
	void SetTrainVO(std::string strVO);
	bool SetPredVO(std::string strVO);
	CResult * RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, std::string strVO);
	CResult * RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,std::string strVO, std::string strModel);
	

	bool GetNetMsg(CTStringArray& msg);
	bool LoadNetWork(CTString strFileName);
	bool SaveNetWork(CTString strFileName);
	bool HasNetWork();
	void Simulate(CResult *pResult,CBPMatrix*pSimulate = NULL);
	bool Train(CResult *pResult);
	void Abort();
	CBPNet();
	virtual ~CBPNet();
	//模型
	virtual bool Export(std::string szFileName);
	virtual bool Import(std::string szFileName);
private:
	bool Serialize(bool IsStoring);
	bool SimulateCheck(bool bLoadData = true);
	bool TrainCheck();
protected:
	void CreateOut(CResult *pResult,double *pActualAry,double *pOutAry,int nLen,
		CTString strTitle,bool bClassMode =false);
	bool DataMap(FieldDescList *pInFeildList,FieldDescList *pOutFeildList, CBPMatrix &mx, bool bIsMapToOne,bool bHasRange);
	bool DataMapS(FieldDescList &FeildList,CBPMatrix &mx,bool bIsMapToOne,bool bHasRange,
		CDWordArray *pFieldIndex /*= NULL*/, CDWordArray *pMxIndex/*= NULL*/);
};

#endif // !defined(AFX_BPNET_H__09519FC4_8791_4C5E_A0C3_2AA1A0A81B87__INCLUDED_)





















