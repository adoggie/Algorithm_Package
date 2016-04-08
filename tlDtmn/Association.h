// Association.h: interface for the CAssociation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASSOCIATION_H__68C7C38C_F7F0_47DF_8557_ACE166A16D5B__INCLUDED_)
#define AFX_ASSOCIATION_H__68C7C38C_F7F0_47DF_8557_ACE166A16D5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../TLDBDll/Database/DataAccess.h"
#include "../DataInterface/DataInterface.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"

//单维关联规则的界面输入参数
class CRuleTrainVO
{
public:
	//	CPtrArray	arFeildInfo;//TFieldInfo，表明输入字段的信息
	CTStringArray strInputName;
	CTStringArray strDestName;
	bool	bSingle;			//true单维关联规则,false多维关联规则
	int		nSupport;			//最小支持度
	double	dbConfidence;		//可信度
	CRuleTrainVO()
	{
		bSingle	= true;
		nSupport = 0;
		dbConfidence = 0;
	}
	bool Compile(string strVO)
	{
	//	关联规则VO设置
	//	DTMNRUN
	//	/DATA = FILE
	//	/VARINPUT arglistg
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/ASSOTYPE [{SINGLE},{MULTI**}]
	//	/MINSUPPORT SUPPORT(0.1**)
	//	/ MINCREDIT CREDIT (0.5**)		



		AnaWord VOstr;
		VOstr.Import(strVO);
		
		bSingle = AnaSyntax::findstr("/ASSOTYPE","SINGLE",VOstr);
		if (AnaSyntax::findstr("/MINSUPPORT","SUPPORT",VOstr))
			nSupport = AnaSyntax::getvalue("/MINSUPPORT","SUPPORT",VOstr);
		if (AnaSyntax::findstr("/MINCREDIT","CREDIT",VOstr))
			dbConfidence = AnaSyntax::getvalue("/MINCREDIT","CREDIT",VOstr);
		
		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputName);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDestName);
		return true;
	}
};

//关联规则输出参数，可以供循环计算用
//预测模型的参数
class CRuleOutput : public StructBase
{
public:
	CRuleOutput(){;}
	virtual ~CRuleOutput(){;}
	virtual bool Import(string szFileName);
	virtual	bool Export(string szFileName);
protected:
	//界面输入参数
	CPtrArray	arFeildInfo;//TFieldInfo，表明输入字段的信息
	bool	bSingle;			//true单维关联规则,false多维关联规则
	int		nSupport;			//最小支持度
	double	dbConfidence;		//可信度
	bool	bStopWork;			//是否停止工作
	bool	bSort;				//排序
	//规则
	CPtrArray	arFreSet;//频繁集的数据TFreSet
	CPtrArray	arRuleDmn;//关联规则的强规则TRuleDmn
};




class CAssociation  
{
public:
	CAssociation();
	virtual ~CAssociation();

	CResult *RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO);
	CResult* RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO, string strModel);
};

#endif // !defined(AFX_ASSOCIATION_H__68C7C38C_F7F0_47DF_8557_ACE166A16D5B__INCLUDED_)
