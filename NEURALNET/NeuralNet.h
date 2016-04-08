//////////////////////////////////////////////////////////////////////
// NeuralNet.h:                                                     //
//																	//
// Define:        ��������ඨ��                                  //
// Creator:       ���                                              //
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
*�㷨����綯̬�������Ƿ���Ҫ���������Ϣ���Ƿ���ֹ
* /
struct Interaction
{
	bool bNeedFeedback;//���룺�����㷨�Ƿ���Ҫд������Ϣ��true�����뷴����false������
	bool bAbort;//���룺true��ֹ��ǰ����false���������С�Ĭ��false
	char BufMsg[100];//����������ǰ������Ϣ	
};

/ *
����Ŀ�����Ϣ�Ļ��࣬����Щ������Ϊ���������Щ��Ϊ�Ա��������������Щ��Ҫ���
��Ҫ���ƽ������ʾ���⣬ÿһ���㷨����һ����
* /
class VOBase{
};

/ *
�㷨ģ����Ϣ����Ҫ�����㷨ѵ��ʱ�������ģ����Ϣ��������һ�ε�Ԥ�⡣
ÿһ���㷨����һ����
* /
class StructBase{
public :
	string Export();//����ΪXML��ʽ
	bool Import(string str);
};

/ *
���������࣬����ϵͳ���������·���ΪC�㷨�ڲ����õķ���
* /
class CEnvParam{
public:
	// ******  ָ����Щ������Ҫ���뵽������  *** 
	bool HasOutParm(CTString sName);//�ж��Ƿ���Ҫ���ĳ����������yresid
	string GetOutParamName(CTString sName);//
	// *************���û�������************
	bool SetVar(CTString sVarName,double value);
	bool SetVar(CTString sVarName,CDoubleVector* value);//value �ڴ�ռ����㷨�Լ��ͷš�
	bool SetVar(CTString sVarName,CDoubleMatrix* value);//value �ڴ�ռ����㷨�Լ��ͷš�
	bool SetVar(CTString sVarName,CDataAccess* value);//value �ڴ�ռ����㷨�Լ��ͷš�
};

*/

//��������ӿڣ�Ϊ�ϲ�����ṩ�Ľӿ�
class CNeuralNetVO{
public:
	CTStringArray strInputName;//TFieldInfo�����������ֶε���Ϣ
	int		nNetStructure;//����ṹ��0��Ӧ��������ART���磬1 Kohonen����֯���磬2 Hopfield�㷨
	int		nMaxCluster;//���ľ������
	int		nTrainTimes;//���еĴ���
	double	dMaxLearnRate;//����ѧϰ��
	double	dMinLearnRate;//��Сѧϰ��
	double	dWarningRate;//������
	double	dMaxRunTime;//�������ʱ�䣬-1Ϊû����ʱ
	CTString strModelName;//�����·������
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

	//������VO����
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
//�����磬Ԥ��ģ�͵�����
typedef struct tagStructPredModel
{
	CNeuralNetVO*	pVO;		//�������������������
	//Hopfeild��ģ�͵�����
	CDoubleMatrix	mtCoordCenter;//����������
	CDoubleMatrix	mtCoordMax;//���෶Χ���ֵ
	CDoubleMatrix	mtCoordMin;//���෶Χ��Сֵ
	//Kohonen����֯���磬ģ�͵�����
	CDoubleMatrix	mtKohonenWeight;//Ȩֵ
	//ART���磬ģ�͵�����
	CDoubleMatrix	mtInterfaceWeight;//�ӿڲ��Ȩֵ��
	CDoubleMatrix	mtClusterWeight;//������Ȩֵ��
}StructPredModel;
*/

//�����������ݣ��ɹ�ѭ������
//�����磬Ԥ��ģ�͵�����
class CNeuralNetOutput : public  StructBase
{
public:
	CNeuralNetOutput();
	virtual ~CNeuralNetOutput(){;};
	virtual bool Export(std::string szFileName);
	virtual bool Import(std::string szFileName);

public:
	//VO info
	CTStringArray strInputName;      //TFieldInfo�����������ֶε���Ϣ
	CTString      strDataSourceName; //����Դ����
	int		nNetStructure;//����ṹ��0��Ӧ��������ART���磬1 Kohonen����֯���磬2 Hopfield�㷨
	int		nMaxCluster;//���ľ������
	int		nTrainTimes;//���еĴ���
	double	dMaxLearnRate;//����ѧϰ��
	double	dMinLearnRate;//��Сѧϰ��
	double	dWarningRate;//������
	double	dMaxRunTime;//�������ʱ�䣬-1Ϊû����ʱ
	//Hopfield����
    int     nGeneration; /* ��Ѹ������ɴ� */
    double  dblFitness;  /* ��Ѹ�����Ӧ�� */
    double  dblDistance; /* �����Ӧ�ĸ��㵽�����������ĵľ���֮�� */
	//ART����
	int     nResetCount;
	int     nClusterCount;
	int     nClusterRange;
	double  dVigilance;
	int     nChampCluster;
	CIntVector  vtChrom; /* ��Ѹ���Ⱦɫ�� */
	CDoubleVector	vtClusterCenter;//����������
	CDoubleVector	vtClusterMax;//���෶Χ���ֵ
	CDoubleVector   vtClusterMin;//���෶Χ��Сֵ
	CDoubleVector	vtRange;	 /* ԭʼ���������Сֵ */
	//Kohonen����֯��������
	CDoubleVector	vtKohonenWeight;//Ȩֵ
	//ART��������
	CDoubleVector	vtInterfaceWeight;//�ӿڲ��Ȩֵ��
	CDoubleVector	vtClusterWeight;//������Ȩֵ��
};



//////////////////////////////////////////////////////////////////////////
//��������ֻʵ����Hopfield�㷨��Kohonen�㷨��ART�㷨,������㷨����ʱ��û��ʵ��
//��Ҫ��ͨ��Run�㷨���м��㴦��
//////////////////////////////////////////////////////////////////////////
class DLLEXPORT CNeuralNet  
{
private:
	CTStringArray	 m_arFieldRole;//�ֶ���Ϣ��������ֶ�
	CDataInterface * m_pDataInterface;//���ݵĽӿ�
	CNeuralNetVO	 *m_pNeuralNet;//��������Ľӿ�
	CTopology * m_pTopology;//��������˽ṹ�ӿ�
	CHopGen * m_pHopGen;//Hopfield����Ľӿ�
	CSampleSet * m_pSampleSet;//���ݼ�����Ҫ�Ƕ����ݵĲɼ�

	int m_nTopologyType;//��������ͣ���Ҫ��ΪART���磬Kohonen���磬Hopfield���磬

	/////////////////////////////////////////////
	// ���㷨�������ԣ���Ҫ��ͨ���������õ���Ϣ
	bool	m_bAbort;
	unsigned int	m_dwRuntime;//����ʱ��
	int		m_nClusterNum;//���������
	unsigned int	m_unIterateNum;
	bool	m_bTraining;//�Ƿ�ѧϰѵ��
	bool	m_bTesting;//�Ƿ����
	bool	m_bPrediction;//�Ƿ�Ԥ��
	int		m_nTrained;//ѵ������
	bool	m_bOutputInfo;//��������Ϣ

	/////////////////////////////////////////////
	// ART�㷨ר������
	double	m_fVigilance;//������

	////////////////////////////////////////////
	// Kohonen�㷨ר������
	double	m_fMaxRate;//���ѧϰ��
	double	m_fMinRate;//��Сѧϰ��

	/////////////////////////////////////////////
	// BP�㷨ר������
	double m_dFeedbackRate;

	/////////////////////////////////////////////
	// RBF�㷨ר������

	/////////////////////////////////////////////
	// �Ŵ��㷨ר������
	int		m_nGenicNum;
	double	m_fMutationRatio;
	double	m_fCrossRatio;

	/////////////////////////////////////////////
	// Hopfield�㷨ר������
public:
//for save
	BestIndividual m_Best;
	int m_nDimension;
	bool m_bHaveNetwork;//�Ƿ����������ѧϰ����
//for load
	CTString m_strLoadFileName;
	bool m_bLoad;
///////
public:
	bool UpdateField(TPredVO *pVO, CNeuralNetOutput *pOutput);
public:
	void Abort();
	bool HaveTarget();//�Ƿ���Ŀ���ֶΣ��з���true��û�з���false
	bool SetDataSource( CDataInterface * pDataInterface, CTStringArray& arFieldRole );//��������Դ�������ֶ�
	bool SetDataParam(CNeuralNetVO	*pNeuralNet);//���ý�����������--by pyx-2005-12-23
	void SetErrInfo(CTString FormmatStr, CResult *& pResult);//��������Ϣ��������
// 	bool SetParamFromVO(string, CResult*&);//��VO��ȡ�������������
	void DeleteFiles();//ɾ�������ļ�
	void Initialize();//���г�ʼ����ͬ�ļ����г�ʼ�����ò���

	CNeuralNet();
	virtual ~CNeuralNet();

	/////////////////////////////////////////////
	// ���㷨���в���
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
	// Hopfield���Ŵ��㷨ר�в���
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

	int HopAndGenHitsTest( CDoubleVector &adData, BestIndividual * pBest, int nDimension, bool& bHits );//Hopfield��GA�㷨���в���
	void HopGenPredict( CDataInterface * pData, BestIndividual * pBest, CResult * pResult );//Hopfield����Ԥ��
	BestIndividual * GetHopGenNetwork(int nDimension);//���Hopfield����
	void DisplayTopGenResult( BestIndividual * pBest, CResult * pResult );//��ʾGA�Ľ��
	void NormalizeBestIndvd( BestIndividual * pBest, int nDimension );//��һ���ֶΣ�GA�㷨

	/////////////////////////////////////////////
	// ART�㷨ר�в���
	void SetVigilance( double dVigilance )
	{ m_fVigilance = dVigilance; };
	double GetVigilance()
	{ return m_fVigilance; };

	/////////////////////////////////////////////
	// Kohonen�㷨ר�в���
	void SetMaxRate( double dMaxRate )
	{ m_fMaxRate = dMaxRate; };
	double GetMaxRate()
	{ return m_fMaxRate; };

	void SetMinRate( double dMinRate )
	{ m_fMinRate = dMinRate; };
	double GetMinRate()
	{ return m_fMinRate; };

	/////////////////////////////////////////////
	// BP�㷨ר�в���
	void SetFeedbackRate( double dFeedbackRate )
	{ m_dFeedbackRate = dFeedbackRate; };
	double GetFeedbackRate()
	{ return m_dFeedbackRate; };
};

#endif // !defined(_NEURALNET_H_)
