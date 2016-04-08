// EnvParam.h: interface for the CEnvParam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVPARAM_H__F0BE7E6F_CCE6_4947_BA44_D076FA84AD6A__INCLUDED_)
#define AFX_ENVPARAM_H__F0BE7E6F_CCE6_4947_BA44_D076FA84AD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../Public/common.h"
#include "../Public/TString.h"
#include "../cpknl/cpknl.h"
#include  "../TLDBDll/Database/DataAccess.h"
#include "../TLDBDll/Database/fieldlist.h"
#include "../TLDBDll/Database/cfield.h"
#include <string>
//using namespace std;
/*
*算法和外界动态交互，是否需要输出反馈信息，是否终止
*/
struct Interaction
{
	bool bNeedFeedback;//输入：告诉算法是否需要写反馈信息。true：必须反馈，false：不用
	bool bAbort;//输入：true算法应该终止当前程序。false：继续运行。默认false
	char BufMsg[100];//输出：输出当前运行信息	
	Interaction()
	{
		bNeedFeedback = false;
		bAbort = false;
		memset(BufMsg,0,100);
	}
};


/*
输入的控制信息的基类，如哪些变量作为因变量，哪些作为自变量，结果集中哪些需要输出
主要控制结果集显示问题，每一个算法都不一样。
*/
class VOBase{
public:
	VOBase();
	virtual ~VOBase();
};

/*
算法模型信息，主要用于算法训练时，输出的模型信息，用于下一次的预测。
每一个算法都不一样。
*/
class StructBase{
public :
	StructBase();
	virtual ~StructBase();
	virtual bool Export(std::string szFileName)=0;//add by xds 20060712
	virtual bool Import(std::string szFileName)=0;//add by xds 20060712
};


/*
环境控制类，设置系统参数，以下方法为C算法内部调用的方法
*/
class CEnvParam  
{
public:
	CEnvParam();
	virtual ~CEnvParam();
public:
	//******  指定那些参数需要输入到参数中  *** 
	bool HasOutParm(CTString sName);//判断是否需要输出某个参数，如yresid
	CTString GetOutParamName(CTString sName);//
	//*************设置环境变量************
	bool SetVar(CTString sVarName,double value);
	bool SetVar(CTString sVarName,CDoubleVector* value);//value 内存空间由算法自己释放。
	bool SetVar(CTString sVarName,CDoubleMatrix* value);//value 内存空间由算法自己释放。
	bool SetVar(CTString sVarName,CDataAccess* value);//value 内存空间由算法自己释放。

};

class AnaWord;
class AnaSyntax;
//界面的输入参数,预测模型输入参数

typedef struct  tagTPredVO//决策树和神经网络共用的
{
	tagTPredVO()
	{
		bEvaluate = false;
		strNewFileName = "";
	}
	bool Compile(std::string strVO)
	{
//	NEURALPRED
//		/DATA=[FILE]
//		/VARINPUT [varlist]
//		/MODELFROMFILE [FILE]

		AnaWord wrd;
		wrd.Import(strVO);
		
		strNewFileName = wrd.GetAt(3);
		//输入字段
		if (AnaSyntax::exist("/VARINPUT", wrd))
			AnaSyntax::getVAR("/VARINPUT",wrd,arFieldInfo);
		else
			return false;
		//替换字段
		if (AnaSyntax::exist("/REPLACEVARNAME",wrd))
		{
			AnaSyntax::getVAR("/REPLACEVARNAME", wrd, szArrReplace);
			if (szArrReplace.GetSize()%2 != 0)
				return false;
		}
		//目标显示字段
		if (AnaSyntax::exist("/PREDNAME", wrd))
			szOutput = AnaSyntax::getSingleStr("/PREDNAME", wrd);
		//模型评估-add by xds 20060717
		CTStringArray arrOut;
		AnaSyntax::getVAR("/OUTPUT",wrd,arrOut);
		int i=0, nSize = arrOut.GetSize();
		for (i=0; i<nSize; i++)
		{
			CTString szTemp = arrOut.GetAt(i);
			if (szTemp == "EVALUATION")
			{
				bEvaluate = true;   //是否只评估不输出到文件
			}
		}
		//模型路径
		if (AnaSyntax::exist("/MODELFROMFILE", wrd))
			m_ModelFile = AnaSyntax::getSingleStr("/MODELFROMFILE", wrd);
		else 
			return false;
		return true;
	};
public:
	bool        bEvaluate;      //是否为模型评估
	CTString	strNewFileName; //如果不到入源文件中，则需要新建一个文件用于存放预测的值，还有预测的一些参数
	CTStringArray	arFieldInfo;//需要导入到新文件中去字段的信息
	CTString		m_ModelFile;		//保存的模型
public:
	CTStringArray   szArrReplace;//替换的新字段索引
	CTString        szOutput;    //目标变量-显示用,否则会可能会出错!
}TPredVO;



/*
例子：
返回时有三大类：1。结果集，界面显示用，控制参数由VO控制
                2。环境变量，pEnv，主要用于连环计算
				3。输出结构信息，训练后的结构，用于预测
回归训练时：
CDataInterface *pData 【输入】 输入的数据集
CEnvParam *pEnv       【输入输出】 环境参数的设置
Interaction *itt      【输入输出】 动态交互用

VOReg *VO             【输入】 输入的控制信息
StructReg *pOutStruct 【输出】 输出控制信息
说明：所有的参数内存均由外面申请和释放，算法内部不需要考虑

CResult * RegTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
				   VOReg *VO, StructReg *pOutStruct)
{
	//.....例子
	if (pEnv->HasOutParm("yresid"))
	{//如果指定要输出参数yresid则
		pEnv->SetVar(pEnv->GetOutParamName("yresid"),nY);//nY为系统算出来的参数
	}

}*/
/*
例子：
返回时有两大类：1。结果集，界面显示用，控制参数由VO控制
                2。环境变量，pEnv，主要用于连环计算
回归预测时：
CDataInterface *pData 【输入】 输入的数据集
CEnvParam *pEnv       【输入输出】 环境参数的设置
Interaction *itt      【输入输出】 动态交互用

VORegPredict *VO             【输入】 输入的控制信息与训练时可能不一样
StructReg *pInStruct 【输入】 输入的控制信息 （此处作为输入）
说明：所有的参数内存均由外面申请和释放，算法内部不需要考虑
CResult * RegPredict(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
				   VORegPredict *VO, StructReg *pInStruct)
{

}
*/

#endif // !defined(AFX_ENVPARAM_H__F0BE7E6F_CCE6_4947_BA44_D076FA84AD6A__INCLUDED_)
