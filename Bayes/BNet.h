// BNet.h: interface for the CBNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BNET_H__9FF56EFD_AF1E_4A66_B6DA_55725937228D__INCLUDED_)
#define AFX_BNET_H__9FF56EFD_AF1E_4A66_B6DA_55725937228D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BNode.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"
#include <string>
using namespace std;

class CBayesVO
{
public:
	CTStringArray	strInputField;
	CTStringArray	strCauIndex;
	CBayesVO()
	{
	}
	bool Compile(string strVO)
	{
	//	贝叶斯VO设置
	//	BAYESNET
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/MISSMETHOD DELRECORD
	//	/INDEX arglist


	//	/BAYESNET
	//	/VARINPUT arglist
	//	/INDEX 洪水 地震 降雨 警报 警报 地震
		
		AnaWord VOstr;
		VOstr.Import(strVO);
		
		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputField);
		AnaSyntax::getVAR("/INDEX", VOstr, strCauIndex);
		return true;
	}
};


class CBayesOutput : public StructBase
{
public:
	CTStringArray	strInputField;
	CTStringArray	strCauIndex;
	CIntMatrix		mxCauseMatrix;
	CPtrArray		arValueLabel;
	CPtrArray		arProbility;
	CBayesOutput()
	{
	}
	virtual string Export();
	virtual bool   Import(string strModel);
};


class CBNet  
{
public:
	CBNet();
	CBNet(CDataInterface*);
	virtual ~CBNet();
	CBNet(int nn,char** namelist,CIntMatrix & ct);
	CBNet(CBNet& ns);

	CDataInterface * m_pDataInterface;
	CTStringArray	m_arFieldName;		//节点的字段名称
	int				m_nNodeNum;	       //节点个数
	CTString		m_NetName;			//网络名称
	bool			m_NeedSave;		//序号保存网络标志，0为不需要，1为需要
	CPtrList		m_NodePtrList;     //结点列表
	CIntMatrix		m_CausTbl;	   //节点关系
	long             m_nSimNum;     //Monte Carlo模拟将要产生的样本个数
	int             m_nGSimNum;    //缺失数填中的Gibbs采样次数
	CIntMatrix	    m_DataMatrix;   //样本集合矩阵，行对应记录，列对应字段（变量）
	CIntMatrix      m_InitMatrix;    //设定用户给定的推理初始条件
	CIntVector      m_AimVector;     //设定用户给定的推理目标变量
	//CIntVector		m_OldAimVector;	//保留排序前的观察值矩阵
	CIntMatrix		m_SimDataMatrix;  //MC模拟样本集合
	CIntVector      m_ChangeTbl;      //对照表
	CIntVector      m_StateNum;       //结点状态个数
	CDoubleVector	m_ProbVector;		//推理结果，目标变量取各状态的概率
	bool			m_bErrorFlag;		//出错标志
	CTString			m_sErrorInfo;		//出错信息
public:
	//根据界面数据初始化网络
	void InitiateNet(CDataInterface *pDataInterface,CBayesVO*pBayesVO);

	//取数据并初始化所有结点
	CIntMatrix  CBNet::GetData();

	//从数据矩阵中统计结点的状态个数
	//CIntVector CBNet::CalculateStateNum();

	//从节点列表中获得结点的状态个数
	bool CBNet::GetStateNum();

	//统计结点的状态取值
	//void CBNet::GetState();

	//创建节点关系表
	void CBNet::CreateCausTbl();
	
	CResult * CBNet::CreateResult();

	//节点拓扑排序以及网络结构无环图的检查，得到顺序变化的下表对照表
	bool CBNet::Bayesnodessort();
	
	//按照新的节点排序，变换节点关系表、节点状态个数、样本集合以及一直变量和目标变量的节点顺序
	void CBNet::newCausTbl(CIntMatrix& CausTbl,CIntVector ChangeTbl,CIntVector& StateNum,CIntMatrix& DataMatrix);//,CIntMatrix& InitMatrix,CIntVector& AimVector);
	
	//缺失数据处理
	CDoubleMatrix CBNet::DataMatrixSetCompletion(int NodeNum,int GSimNum,CIntMatrix CausTbl,CIntVector StateNum,CDoubleMatrix DataMatrix);
	
	//依据网络结构生成节点的父节点个数和数组
	void CBNet::SetParntArray();

	//初始化各节点的条件概率表——均匀分布
	void CBNet::CPTInitiate();
	//依据网络结构和样本集合计算网络参数——条件概率（表）
	void CBNet::CPTCalculate();//,CIntMatrix DataMatrix,CIntVector StateNum,CIntMatrix CausTbl); 
	
	//采用Monte Carlo模拟产生模拟样本集合Sim_Samples
	bool CBNet::BayesNetMCSim(/*CShowData*pDialogData*/);

	//依据观察值X进行网络推理，得到目标变量的后验概率，并直接输出
	bool CBNet::BayesNetInference(/*CShowData*pDialogData*/);
	
	bool CBNet::FindRow(CIntVector StateTbl,CIntVector DataMatrix,CIntVector parntVector,int parntNum);
   
	double CBNet::random(double mi,double ma);    //产生随机数函数

	void CBNet::CreateStateTbl(CIntMatrix& StateTbl,CIntVector parntVector,int num);
public:
	CResult* CBNet::Train(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
		CBayesVO*pBayesVO, CBayesOutput &ppOutStruct);
	CResult *CBNet::Pred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
		CBayesVO*pBayesVO, CBayesOutput &ppOutStruct);
	bool CBNet::LearnParam();//(CIntMatrix InitMatrix,CIntVector AimVector,CIntMatrix CausTbl,int SimNum,int GSimNum);		
	//输出推理结果
	CResult* Output(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
		TPredVO *pVO, CBayesOutput &ppOutStruct);
	//存储网络结构
	bool CBNet::SaveNet();
	//读取网络结构
	bool CBNet::ReadNet();	
};


#endif // !defined(AFX_BNET_H__9FF56EFD_AF1E_4A66_B6DA_55725937228D__INCLUDED_)





















