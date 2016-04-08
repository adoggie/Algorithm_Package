//////////////////////////////////////////////////////////////////////
// NeuralNet.h:                                                     //
//																	//
// Define:        神经网络基类定义                                  //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _NEURALNET_H_
#define _NEURALNET_H_
#include "../cpknl/cpknl.h"
#include "../Public/TString.h"
#include "../Result/Result.h"
#include "Topology.h"
#include "SampleSet.h"
#include "HopGen.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include <string>
//using namespace std;

#define  DLLEXPORT __declspec(dllexport)

/*



/ *
*算法和外界动态交互，是否需要输出反馈信息，是否终止
* /
struct Interaction
{
	bool bNeedFeedback;//输入：告诉算法是否需要写反馈信息。true：必须反馈，false：不用
	bool bAbort;//输入：true终止当前程序。false：继续运行。默认false
	char BufMsg[100];//输出：输出当前运行信息	
};

/ *
输入的控制信息的基类，如哪些变量作为因变量，哪些作为自变量，结果集中哪些需要输出
主要控制结果集显示问题，每一个算法都不一样。
* /
class VOBase{
};

/ *
算法模型信息，主要用于算法训练时，输出的模型信息，用于下一次的预测。
每一个算法都不一样。
* /
class StructBase{
public :
	string Export();//导出为XML方式
	bool Import(string str);
};

/ *
环境控制类，设置系统参数，以下方法为C算法内部调用的方法
* /
class CEnvParam{
public:
	// ******  指定那些参数需要输入到参数中  *** 
	bool HasOutParm(CTString sName);//判断是否需要输出某个参数，如yresid
	string GetOutParamName(CTString sName);//
	// *************设置环境变量************
	bool SetVar(CTString sVarName,double value);
	bool SetVar(CTString sVarName,CDoubleVector* value);//value 内存空间由算法自己释放。
	bool SetVar(CTString sVarName,CDoubleMatrix* value);//value 内存空间由算法自己释放。
	bool SetVar(CTString sVarName,CDataAccess* value);//value 内存空间由算法自己释放。
};

*/

//参数输入接口，为上层调用提供的接口
class CNeuralNetVO{
public:
	CTStringArray strInputName;//TFieldInfo，表明输入字段的信息
	int		nNetStructure;//网络结构，0对应布尔向量ART网络，1 Kohonen自组织网络，2 Hopfield算法
	int		nMaxCluster;//最大的聚类个数
	int		nTrainTimes;//运行的次数
	double	dMaxLearnRate;//最大的学习率
	double	dMinLearnRate;//最小学习率
	double	dWarningRate;//警觉度
	double	dMaxRunTime;//最大运行时间，-1为没有限时
	CTString strModelName;//保存的路径名字
	CNeuralNetVO()
	{
		nNetStructure = 2;
		nMaxCluster = 5;
		dMaxLearnRate = 0.4;
		dMinLearnRate = 0.01;
		dWarningRate = 0.5;
		dMaxRunTime = -1.0;
		nTrainTimes = 500;
	}
	bool Compile(std::string strVO)
	{

	//神经网络VO设置
	//	NEURALNETTRAIN
	//	/DATA=[FILE]
	//	/VARINPUT = arglist
	//	/MISSMETHOD [{DELRECORD**},{AVEFILL}]
	//	/NETSTRUCTURE =  [{ART},{KOHONEN},{HOPFIELD**}]
	//	/PROPERTY = WARNINGRATE(0.5**) MAXCLUSTER(5**) MAXLEARNEDRATE(0.4**) MINLEARNEDRATE(0.01**)
	//	/MAXRUNTIME = [{10MIN},{1HUR},{2HUR},{7HUR},{1DAY},{2DAY},{7DAY},{INF**}]
	//	/TRAINTIME = TIME(50000**)
	//	/SAVEMODEL [FILE]
		
		
		AnaWord VOstr;
		VOstr.Import(strVO);
		
		if (AnaSyntax::findstr("/NETSTRUCTURE","ART",VOstr))
		{
			nNetStructure = 0;
			if (AnaSyntax::findstr("/PROPERTY","WARNINGRATE",VOstr))
				dWarningRate = AnaSyntax::getvalue("/PROPERTY","WARNINGRATE",VOstr);
		}
		else if (AnaSyntax::findstr("/NETSTRUCTURE","KOHONEN",VOstr))
		{
			nNetStructure = 1;
			if (AnaSyntax::findstr("/PROPERTY","MAXCLUSTER",VOstr))
				nMaxCluster = AnaSyntax::getvalue("/PROPERTY","MAXCLUSTER",VOstr);
			if (AnaSyntax::findstr("/PROPERTY","MAXLRNRATE",VOstr))
				dMaxLearnRate = AnaSyntax::getvalue("/PROPERTY","MAXLRNRATE",VOstr);
			if (AnaSyntax::findstr("/PROPERTY","MINLRNRATE",VOstr))
				dMinLearnRate = AnaSyntax::getvalue("/PROPERTY","MINLRNRATE",VOstr);
		}
		else
		{
			nNetStructure = 2;
			if (AnaSyntax::findstr("/PROPERTY","MAXCLUSTER",VOstr))
				nMaxCluster = AnaSyntax::getvalue("/PROPERTY","MAXCLUSTER",VOstr);
		}

		if (AnaSyntax::findstr("/MAXRUNTIME","10MIN",VOstr))
			dMaxRunTime = 10 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","1HUR",VOstr))
			dMaxRunTime = 1 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","2HUR",VOstr))
			dMaxRunTime = 2 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","7HUR",VOstr))
			dMaxRunTime = 7 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","1DAY",VOstr))
			dMaxRunTime = 1 * 24 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","2DAY",VOstr))
			dMaxRunTime = 2 * 24 * 60 * 60*1000;
		else if (AnaSyntax::findstr("/MAXRUNTIME","7DAY",VOstr))
			dMaxRunTime = 7 * 24 * 60 * 60*1000;
		else
			dMaxRunTime = -1.0;

		if (AnaSyntax::findstr("/TRAINTIME","TIME",VOstr))
			nTrainTimes= AnaSyntax::getvalue("/TRAINTIME","TIME",VOstr);

		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputName);

		strModelName = AnaSyntax::getSingleStr("/SAVEMODEL",VOstr);
		
		return true;
	}
};


/*
//神经网络，预测模型的数据
typedef struct tagStructPredModel
{
	CNeuralNetVO*	pVO;		//输入参数，公共的数据
	//Hopfeild，模型的树聚
	CDoubleMatrix	mtCoordCenter;//中心类坐标
	CDoubleMatrix	mtCoordMax;//聚类范围最大值
	CDoubleMatrix	mtCoordMin;//聚类范围最小值
	//Kohonen自组织网络，模型的数据
	CDoubleMatrix	mtKohonenWeight;//权值
	//ART网络，模型的数据
	CDoubleMatrix	mtInterfaceWeight;//接口层的权值表
	CDoubleMatrix	mtClusterWeight;//聚类层的权值表
}StructPredModel;
*/

//输出结果的数据，可供循环计算
//神经网络，预测模型的数据
class CNeuralNetOutput : public  StructBase
{
public:
	CNeuralNetOutput();
	virtual ~CNeuralNetOutput(){;};
	virtual bool Export(std::string szFileName);
	virtual bool Import(std::string szFileName);

public:
	//VO info
	CTStringArray strInputName;      //TFieldInfo，表明输入字段的信息
	CTString      strDataSourceName; //数据源名称
	int		nNetStructure;//网络结构，0对应布尔向量ART网络，1 Kohonen自组织网络，2 Hopfield算法
	int		nMaxCluster;//最大的聚类个数
	int		nTrainTimes;//运行的次数
	double	dMaxLearnRate;//最大的学习率
	double	dMinLearnRate;//最小学习率
	double	dWarningRate;//警觉度
	double	dMaxRunTime;//最大运行时间，-1为没有限时
	//Hopfield数据
    int     nGeneration; /* 最佳个体生成代 */
    double  dblFitness;  /* 最佳个体适应度 */
    double  dblDistance; /* 个体对应的各点到各个聚类中心的距离之和 */
	//ART网络
	int     nResetCount;
	int     nClusterCount;
	int     nClusterRange;
	double  dVigilance;
	int     nChampCluster;
	CIntVector  vtChrom; /* 最佳个体染色体 */
	CDoubleVector	vtClusterCenter;//中心类坐标
	CDoubleVector	vtClusterMax;//聚类范围最大值
	CDoubleVector   vtClusterMin;//聚类范围最小值
	CDoubleVector	vtRange;	 /* 原始数据最大最小值 */
	//Kohonen自组织网络数据
	CDoubleVector	vtKohonenWeight;//权值
	//ART网络数据
	CDoubleVector	vtInterfaceWeight;//接口层的权值表
	CDoubleVector	vtClusterWeight;//聚类层的权值表
};



//////////////////////////////////////////////////////////////////////////
//本类现在只实现了Hopfield算法，Kohonen算法，ART算法,其余的算法，暂时还没有实现
//主要是通过Run算法进行计算处理
//////////////////////////////////////////////////////////////////////////
class DLLEXPORT CNeuralNet  
{
private:
	CTStringArray	 m_arFieldRole;//字段信息，输入的字段
	CDataInterface * m_pDataInterface;//数据的接口
	CNeuralNetVO	 *m_pNeuralNet;//输入参数的接口
	CTopology * m_pTopology;//网络的拓扑结构接口
	CHopGen * m_pHopGen;//Hopfield网络的接口
	CSampleSet * m_pSampleSet;//数据集，主要是对数据的采集

	int m_nTopologyType;//网络的类型，主要分为ART网络，Kohonen网络，Hopfield网络，

	/////////////////////////////////////////////
	// 各算法共有属性，主要是通过界面设置的信息
	bool	m_bAbort;
	unsigned int	m_dwRuntime;//运行时间
	int		m_nClusterNum;//最大聚类个数
	unsigned int	m_unIterateNum;
	bool	m_bTraining;//是否学习训练
	bool	m_bTesting;//是否测试
	bool	m_bPrediction;//是否预测
	int		m_nTrained;//训练次数
	bool	m_bOutputInfo;//导出的信息

	/////////////////////////////////////////////
	// ART算法专用属性
	double	m_fVigilance;//警觉度

	////////////////////////////////////////////
	// Kohonen算法专用属性
	double	m_fMaxRate;//最大学习率
	double	m_fMinRate;//最小学习率

	/////////////////////////////////////////////
	// BP算法专用属性
	double m_dFeedbackRate;

	/////////////////////////////////////////////
	// RBF算法专用属性

	/////////////////////////////////////////////
	// 遗传算法专用属性
	int		m_nGenicNum;
	double	m_fMutationRatio;
	double	m_fCrossRatio;

	/////////////////////////////////////////////
	// Hopfield算法专用属性
public:
//for save
	BestIndividual m_Best;
	int m_nDimension;
	bool m_bHaveNetwork;//是否有神经网络的学习规则
//for load
	CTString m_strLoadFileName;
	bool m_bLoad;
///////
public:
	bool UpdateField(TPredVO *pVO, CNeuralNetOutput *pOutput);
public:
	void Abort();
	bool HaveTarget();//是否有目标字段，有返回true，没有返回false
	bool SetDataSource( CDataInterface * pDataInterface, CTStringArray& arFieldRole );//设置数据源和输入字段
	bool SetDataParam(CNeuralNetVO	*pNeuralNet);//设置界面的输入参数--by pyx-2005-12-23
	void SetErrInfo(CTString FormmatStr, CResult *& pResult);//将错误信息加入结果集
// 	bool SetParamFromVO(string, CResult*&);//从VO读取并设置输入参数
	void DeleteFiles();//删除所有文件
	void Initialize();//进行初始化，同文件进行初始化设置参数

	CNeuralNet();
	virtual ~CNeuralNet();

	/////////////////////////////////////////////
	// 各算法共有操作
	CResult* RunTrain(CDataInterface *pDataInterface,CEnvParam &pEnv, Interaction &itt,CNeuralNetVO *pNeuralNet,CNeuralNetOutput *&pPreModel);
	CResult * RunPred(CDataInterface *pDataInterface,CEnvParam &pEnv, Interaction &itt,TPredVO *pNeuralNet,CNeuralNetOutput *pPreModel);
	//CResult * Run();

	void SetOutputInfo( bool bOutput )
	{ m_bOutputInfo = bOutput; };
	bool GetOutputInfo()
	{ return m_bOutputInfo; };

	void SetTrained( int nTrained )
	{ m_nTrained = nTrained; };
	int GetTrained()
	{ return m_nTrained; };

	void SetTopologyType( int nTopologyType ) 
	{ m_nTopologyType = nTopologyType; };
	int GetTopologyType() 
	{ return m_nTopologyType; }

	void SetRunTime( unsigned int lRuntime )
	{ m_dwRuntime = lRuntime; };
	unsigned int GetRunTime()
	{ return m_dwRuntime; };

	void SetClusterNumber( int nClusterNum )
	{ m_nClusterNum = nClusterNum; };
	int GetClusterNumber()
	{ return m_nClusterNum; };

	void SetIterateNumber( int nIterateNum )
	{ m_unIterateNum = nIterateNum; };
	int GetIterateNumber()
	{ return m_unIterateNum; };

	void SetTraining( bool bTraining )
	{ m_bTraining = bTraining; };
	bool GetTraining()
	{ return m_bTraining; };

	void SetTesting( bool bTesting )
	{ m_bTesting = bTesting; };
	bool GetTesting()
	{ return m_bTesting; };

	void SetPrediction( bool bPrediction )
	{ m_bPrediction = bPrediction; };
	bool GetPrediction()
	{ return m_bPrediction; };

	/////////////////////////////////////////////
	// Hopfield和遗传算法专有操作
	void SetGenicNumber( int nGenNum )
	{ m_nGenicNum = nGenNum; };
	int GetGenicNumber()
	{ return m_nGenicNum; };

	void SetMutationRatio( double dMutationRatio )
	{ m_fMutationRatio = dMutationRatio; };
	double GetMutationRatio()
	{ return m_fMutationRatio; };

	void SetCrossRatio( double dCrossRatio )
	{ m_fCrossRatio = dCrossRatio; };
	double GetCrossRatio()
	{ return m_fCrossRatio; };

	int HopAndGenHitsTest( CDoubleVector &adData, BestIndividual * pBest, int nDimension, bool& bHits );//Hopfield和GA算法击中操作
	void HopGenPredict( CDataInterface * pData, BestIndividual * pBest, CResult * pResult );//Hopfield网络预测
	BestIndividual * GetHopGenNetwork(int nDimension);//获得Hopfield网络
	void DisplayTopGenResult( BestIndividual * pBest, CResult * pResult );//显示GA的结果
	void NormalizeBestIndvd( BestIndividual * pBest, int nDimension );//归一化字段，GA算法

	/////////////////////////////////////////////
	// ART算法专有操作
	void SetVigilance( double dVigilance )
	{ m_fVigilance = dVigilance; };
	double GetVigilance()
	{ return m_fVigilance; };

	/////////////////////////////////////////////
	// Kohonen算法专有操作
	void SetMaxRate( double dMaxRate )
	{ m_fMaxRate = dMaxRate; };
	double GetMaxRate()
	{ return m_fMaxRate; };

	void SetMinRate( double dMinRate )
	{ m_fMinRate = dMinRate; };
	double GetMinRate()
	{ return m_fMinRate; };

	/////////////////////////////////////////////
	// BP算法专有操作
	void SetFeedbackRate( double dFeedbackRate )
	{ m_dFeedbackRate = dFeedbackRate; };
	double GetFeedbackRate()
	{ return m_dFeedbackRate; };
};

#endif // !defined(_NEURALNET_H_)
