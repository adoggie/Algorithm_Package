//////////////////////////////////////////////////////////////////////
// NeuralNet.cpp                                                    //
//																	//
// Define:        神经网络基类实现                                  //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "NeuralNet.h"
#include "KohonenTopology.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../DataInterface/DataInterface.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"

#define IDS_TOPOLOGY1 1
#define IDS_TOPOLOGY2 2
#define IDS_TOPOLOGY3 3
#define IDS_TOPOLOGY4 4
#define IDS_TOPOLOGY5 5
#define IDS_TOPOLOGY6 6
#define IDS_TOPOLOGY7 7
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const UINT WM_PROGRESS_TEXT = RegisterWindowMessage("WM_PROGRESS_TEXT");
const UINT WM_PROGRESS_STEP = RegisterWindowMessage("WM_PROGRESS_STEP");
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNeuralNetOutput::CNeuralNetOutput()
{
	nNetStructure = 2;//网络结构，0对应布尔向量ART网络，1 Kohonen自组织网络，2 Hopfield算法
	nMaxCluster = 5;//最大的聚类个数
	nTrainTimes = 50000;//运行的次数
	dMaxLearnRate = 0.4;//最大的学习率
	dMinLearnRate = 0.01;//最小学习率
	dWarningRate = 0.5;//警觉度
	dMaxRunTime = -1;//最大运行时间，-1为没有限时
	//Hopfield数据
    nGeneration = 10; /* 最佳个体生成代 */
    dblFitness = -1.0;  /* 最佳个体适应度 */
    dblDistance = -1.0; /* 个体对应的各点到各个聚类中心的距离之和 */
}

//////////////////////////////////////////////////////
//导出训练的数据－20060628，Add by xds.
//////////////////////////////////////////////////////
bool CNeuralNetOutput::Export(std::string szFileName)
{
	int i=0, nSize=0;
	CTString szTemp(""), szText("");
	TCLFile theFile;
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Java使用
	//算法名称
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += (nNetStructure == 0) ? ART : (nNetStructure == 1 ? KOHONEN : HPFIELD);
	//数据源
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" + strDataSourceName + "\"";
	//输入变量 
	szText += " ";
	szText += VARINPUT;
	nSize = strInputName.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " ;
		szText+= strInputName.GetAt(i);
	}
	//目标变量
	//szText += " ";
	//szText += VAROUTPUT;
	//szText += " " + strOutputName;
	//日期时间
	SYSTEMTIME st;
	GetLocalTime(&st);
	szTemp.Format("%d年%d月%d日%d时%d分%d秒",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	szText += " ";
	szText += DATETIME;
	szText += " " + szTemp + "\r\n";
	//长度
	nLen = szText.GetLength() + sizeof(int);//总的长度
	theFile << nLen;
	nLen = theFile.WriteString(szText.GetData());
	//开始写模型参数
	theFile.SeekToEnd();
	//共有的属性
	nSize = strInputName.GetSize();
	theFile << nSize;              //变量字段个数
	for (i=0; i<nSize; i++)
	{
		theFile << strInputName.GetAt(i);
	}
	theFile << nNetStructure;      //网络结构
	theFile << nMaxCluster;        //最大的聚类个数
	theFile << nTrainTimes;        //运行的次数
	theFile << dMaxLearnRate;      //最大的学习率
	theFile << dMinLearnRate;      //最小学习率
	theFile << dWarningRate;       //警觉度
	theFile << dMaxRunTime;        //最大运行时间，-1为没有限时
	//类型
	if (nNetStructure == 0)        //ART网络
	{
		theFile << nResetCount;
		theFile << nClusterCount;
		theFile << nClusterRange;
		theFile << dVigilance;
		theFile << nChampCluster;
		nSize = vtInterfaceWeight.vlen();
		theFile << nSize;          //总的向量数
		for (i=0; i<nSize; i++)
		{
			theFile << vtInterfaceWeight(i);//接口层的权值表
		}
		nSize = vtClusterWeight.vlen();
		theFile << nSize;          //总的向量数
		for (i=0; i<nSize; i++)
		{
			theFile << vtClusterWeight(i);//聚类层的权值表
		}
	}
	else if (nNetStructure == 1)   //Kohonen自组织网络
	{
		nSize = vtKohonenWeight.vlen();
		theFile << nSize;          //总的向量数
		for (i=0; i<nSize; i++)
		{
			theFile << vtKohonenWeight(i);//权值
		}
	}
	else if (nNetStructure == 2)   //Hopfield算法
	{
		theFile << nGeneration;    //最佳个体生成代
		theFile << dblFitness;     //最佳个体适应度
		theFile << dblDistance;    //个体对应的各点到各个聚类中心的距离之和
		nSize = vtChrom.vlen();
		theFile << nSize;          //总的向量数
		for (i=0; i<nSize; i++)
		{
			theFile << vtChrom(i); //最佳个体染色体
		}
		nSize = vtClusterCenter.vlen();
		theFile << nSize;          //总的向量数
		for (i=0; i<nSize; i++)
		{
			theFile << vtClusterCenter(i); //中心类坐标
		}
		nSize = vtClusterMax.vlen();
		theFile << nSize;          //总的向量数
		for (i=0; i<nSize; i++)
		{
			theFile << vtClusterMax(i); //聚类范围最大值
		}
		nSize = vtClusterMin.vlen();
		theFile << nSize;          //总的向量数
		for (i=0; i<nSize; i++)
		{
			theFile << vtClusterMin(i); //聚类范围最小值
		}
		nSize = vtRange.vlen();
		theFile << nSize;          //总的向量数
		for (i=0; i<nSize; i++)
		{
			theFile << vtRange(i); //原始数据最大最小值
		}
	}
	theFile.Close();
	return true;
}
//////////////////////////////////////////////////////
//导入训练的数据－20060628，Add by xds.
//////////////////////////////////////////////////////
bool CNeuralNetOutput::Import(std::string szFileName)
{
	int i=0, nSize=0, nValue=0;
	double dValue=0;
	CTString szTemp("");

	TCLFile theFile;
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//截尾长度
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//开始写模型参数
	//共有的属性
	theFile >> nSize;              //变量字段个数
	strInputName.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> szTemp;
		strInputName.Add(szTemp);
	}
	theFile >> nNetStructure;      //网络结构
	theFile >> nMaxCluster;        //最大的聚类个数
	theFile >> nTrainTimes;        //运行的次数
	theFile >> dMaxLearnRate;      //最大的学习率
	theFile >> dMinLearnRate;      //最小学习率
	theFile >> dWarningRate;       //警觉度
	theFile >> dMaxRunTime;        //最大运行时间，-1为没有限时
	//类型
	if (nNetStructure == 0)        //ART网络
	{
		theFile >> nResetCount;
		theFile >> nClusterCount;
		theFile >> nClusterRange;
		theFile >> dVigilance;
		theFile >> nChampCluster;
		theFile >> nSize;          //总的向量数
		if (nSize >0) 
		{
			vtInterfaceWeight.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtInterfaceWeight(i) = dValue;//接口层的权值表
		}
		theFile >> nSize;          //总的向量数
		if (nSize >0) 
		{
			vtClusterWeight.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtClusterWeight(i) = dValue;//聚类层的权值表
		}
	}
	else if (nNetStructure == 1)   //Kohonen自组织网络
	{
		theFile >> nSize;          //总的向量数
		if (nSize >0) 
		{
			vtKohonenWeight.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtKohonenWeight(i) = dValue;//权值
		}
	}
	else if (nNetStructure == 2)   //Hopfield算法
	{
		theFile >> nGeneration;    //最佳个体生成代
		theFile >> dblFitness;     //最佳个体适应度
		theFile >> dblDistance;    //个体对应的各点到各个聚类中心的距离之和
	
		theFile >> nSize;          //总的向量数
		if (nSize >0) 
		{
			vtChrom.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> nValue;
			vtChrom(i) = dValue;   //最佳个体染色体
		}
		theFile >> nSize;          //总的向量数
		if (nSize >0) 
		{
			vtClusterCenter.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtClusterCenter(i) = dValue;   //中心类坐标
		}
		theFile >> nSize;              //总的向量数
		if (nSize >0) 
		{
			vtClusterMax.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtClusterMax(i) = dValue;   //聚类范围最大值
		}
		theFile >> nSize;              //总的向量数
		if (nSize >0) 
		{
			vtClusterMin.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtClusterMin(i) = dValue;  //聚类范围最小值
		}
		theFile >> nSize;              //总的向量数
		if (nSize >0) 
		{
			vtRange.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtRange(i) = dValue;       //原始数据最大最小值
		}
	}
	theFile.Close();
	return true;
}
/*
string  CNeuralNetOutput::Export()
{
	string str;
	//VO info
	char buf[255];
	int nFildCnt = strInputName.GetSize();
	int i;
	double dValue;
	
	//CTStringArray strInputName
	sprintf(buf,"i%di",nFildCnt);
	str += buf;
	for (i=0;i<nFildCnt;i++)
	{
		sprintf(buf,"s%ds%s",strInputName.GetAt(i).GetLength(),strInputName.GetAt(i).GetData());
		str += buf;	
	}

	//	int		nNetStructure;//网络结构，0对应布尔向量ART网络，1 Kohonen自组织网络，2 Hopfield算法
	sprintf(buf,"i%di",nNetStructure);
	str += buf;
	//	int		nMaxCluster;//最大的聚类个数
	sprintf(buf,"i%di",nMaxCluster);
	str += buf;
	//	int		nTrainTimes;//运行的次数
	sprintf(buf,"i%di",nTrainTimes);
	str += buf;
	//	double	dMaxLearnRate;//最大的学习率
	sprintf(buf,"f%ff",dMaxLearnRate);
	str += buf;
	//	double	dMinLearnRate;//最小学习率
	sprintf(buf,"f%ff",dMinLearnRate);
	str += buf;
	//	double	dWarningRate;//警觉度
	sprintf(buf,"f%ff",dWarningRate);
	str += buf;
	//	double	dMaxRunTime;//最大运行时间，-1为没有限时
	sprintf(buf,"f%ff",dMaxRunTime);
	str += buf;
	//	//Hopfield数据
	//    int     nGeneration; // 最佳个体生成代 
	sprintf(buf,"i%di",nGeneration);
	str += buf;
	//    double  dblFitness;  // 最佳个体适应度
	sprintf(buf,"f%ff",dblFitness);
	str += buf;
	//    double  dblDistance; // 个体对应的各点到各个聚类中心的距离之和 
	sprintf(buf,"f%ff",dblDistance);
	str += buf;
	//	  CIntVector  vtChrom; // 最佳个体染色体 
	int nLen = vtChrom.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtChrom(i);
		sprintf(buf,"i%di",dValue);
		str += buf;
	}
	//	  CDoubleVector	vtClusterCenter;//中心类坐标
	nLen = vtClusterCenter.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtClusterCenter(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	CDoubleVector	vtClusterMax;//聚类范围最大值
	nLen = vtClusterMax.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtClusterMax(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	CDoubleVector	vtClusterMin;//聚类范围最小值
	nLen = vtClusterMin.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtClusterMin(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	nLen = vtRange.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtRange(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	//Kohonen自组织网络数据
	//	CDoubleVector	vtKohonenWeight;//权值
	nLen = vtKohonenWeight.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtKohonenWeight(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	//ART网络数据
	//	CDoubleVector	vtInterfaceWeight;//接口层的权值表
	nLen = vtInterfaceWeight.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtInterfaceWeight(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	CDoubleVector	vtClusterWeight;//聚类层的权值表
	nLen = vtClusterWeight.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtClusterWeight(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	return str;
}

bool	CNeuralNetOutput::Import(string strOutput)
{
	const char *pCurPos=NULL;
	char buf[255];
	int  nFildCnt;
	int  i,len,strLen;
	double dValue;

	pCurPos = strOutput.c_str();
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	nFildCnt = atoi(buf);
	pCurPos += len;
	for (i=0;i<nFildCnt;i++)
	{
		len = str_cpy(buf,pCurPos,'s');
		if (len == 0)
			return false;
		strLen = atoi(buf);
		pCurPos += len;
		mem_cpy(buf,pCurPos,strLen);
		pCurPos += strLen;
		strInputName.Add(buf);
	}

	//	int		nNetStructure;//网络结构，0对应布尔向量ART网络，1 Kohonen自组织网络，2 Hopfield算法
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nNetStructure = atoi(buf);
	//	int		nMaxCluster;//最大的聚类个数
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nMaxCluster = atoi(buf);
	//	int		nTrainTimes;//运行的次数
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nTrainTimes = atoi(buf);
	//	double	dMaxLearnRate;//最大的学习率
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dMaxLearnRate = atof(buf);
	//	double	dMinLearnRate;//最小学习率
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dMinLearnRate = atof(buf);
	//	double	dWarningRate;//警觉度
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dWarningRate = atof(buf);
	//	double	dMaxRunTime;//最大运行时间，-1为没有限时
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dMaxRunTime = atof(buf);
	//	//Hopfield数据
	//    int     nGeneration; // 最佳个体生成代 
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nGeneration = atoi(buf);
	//    double  dblFitness;  // 最佳个体适应度 
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dblFitness = atof(buf);
	
	//    double  dblDistance; // 个体对应的各点到各个聚类中心的距离之和
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dblDistance = atof(buf);
	//	CIntVector  vtChrom; // 最佳个体染色体 
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	int nLen = atoi(buf);
	vtChrom.create(nLen,0);
	for (i=0;i<nLen;i++)
	{
		len = str_cpy(buf,pCurPos,'i');
		if (len == 0)
			return false;
		pCurPos += len;
		int nValue = atoi(buf);
		vtChrom(i) = nValue;
	}
	//	CDoubleMatrix	mtClusterCenter;//中心类坐标
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nLen = atoi(buf);
	vtClusterCenter.create(nLen,0.0);
	for (i=0;i<nLen;i++)
	{
		len = str_cpy(buf,pCurPos,'f');
		if (len == 0)
			return false;
		pCurPos += len;
		dValue = atof(buf);
		vtClusterCenter(i) = dValue;
	}
	//	CDoubleMatrix	mtClusterMax;//聚类范围最大值
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nLen = atoi(buf);
	vtClusterMax.create(nLen,0.0);
	for (i=0;i<nLen;i++)
	{
		len = str_cpy(buf,pCurPos,'f');
		if (len == 0)
			return false;
		pCurPos += len;
		dValue = atof(buf);
		vtClusterMax(i) = dValue;
	}
	//	CDoubleMatrix   mtClusterMin;//聚类范围最小值
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nLen = atoi(buf);
	vtClusterMin.create(nLen,0.0);
	for (i=0;i<nLen;i++)
	{
		len = str_cpy(buf,pCurPos,'f');
		if (len == 0)
			return false;
		pCurPos += len;
		dValue = atof(buf);
		vtClusterMin(i) = dValue;
	}
	//CDoubleVector vtRange
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nLen = atoi(buf);
	vtRange.create(nLen,0.0);
	for (i=0;i<nLen;i++)
	{
		len = str_cpy(buf,pCurPos,'f');
		if (len == 0)
			return false;
		pCurPos += len;
		dValue = atof(buf);
		vtRange(i) = dValue;
	}
	//	//Kohonen自组织网络数据
	//	CDoubleMatrix	mtKohonenWeith;//权值
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nLen = atoi(buf);
	vtKohonenWeight.create(nLen,0.0);
	for (i=0;i<nLen;i++)
	{
		len = str_cpy(buf,pCurPos,'f');
		if (len == 0)
			return false;
		pCurPos += len;
		dValue = atof(buf);
		vtKohonenWeight(i) = dValue;
	}
	//	//ART网络数据
	//	CDoubleMatrix	mtInterfaceWeight;//接口层的权值表
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nLen = atoi(buf);
	vtInterfaceWeight.create(nLen,0.0);
	for (i=0;i<nLen;i++)
	{
		len = str_cpy(buf,pCurPos,'f');
		if (len == 0)
			return false;
		pCurPos += len;
		dValue = atof(buf);
		vtInterfaceWeight(i) = dValue;
	}
	//	CDoubleMatrix	mtClusterWeight;//聚类层的权值表
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nLen = atoi(buf);
	vtClusterWeight.create(nLen,0.0);
	for (i=0;i<nLen;i++)
	{
		len = str_cpy(buf,pCurPos,'f');
		if (len == 0)
			return false;
		pCurPos += len;
		dValue = atof(buf);
		vtClusterWeight(i) = dValue;
	}
	return true;
}
*/

CNeuralNet::CNeuralNet()
{
	m_pTopology = NULL;
	m_pHopGen = NULL;
	m_pSampleSet = NULL;
	m_pDataInterface = NULL;

	m_bAbort = false;
	m_dwRuntime = 0;
	m_nClusterNum = 5;
	m_unIterateNum = 50000;
	m_bTraining = true;
	m_bTesting = false;
	m_bPrediction = false;
	m_nTrained = 0;
	m_bOutputInfo = true;//modify by xds 20060620

	// ART算法
	m_fVigilance = 0.5;

	// Kohonen算法
	m_fMaxRate = 0.4;
	m_fMinRate = 0.01;

	// BP算法
	m_dFeedbackRate = 0.0;

	// 遗传算法
	m_fCrossRatio = 0.45;
	m_nGenicNum = 100;
	m_fMutationRatio = 0.005;

//for save	
	m_bHaveNetwork=false;
//for load
	m_bLoad=false;
}

CNeuralNet::~CNeuralNet()
{
	if( m_pTopology!=NULL )
		delete m_pTopology;

	if( m_pSampleSet!=NULL )
		delete m_pSampleSet;

// 	m_arFieldRole.RemoveAll();
	DeleteFiles();
}

CResult * CNeuralNet::RunTrain(CDataInterface *pDataInterface,CEnvParam &pEnv, Interaction &itt,CNeuralNetVO *pNeuralNet,CNeuralNetOutput *&pPreModel)
{
	int nSampleSize, nDimension, i, nFlag, nFieldCount;	
	CTString strTmp;
	CResult * pResult = NULL;
	//	m_pNeuralNet = pNeuralNet;
	//add by xds 20060601.实际输入的最大聚类数
	m_pNeuralNet = new CNeuralNetVO;
	m_nClusterNum = pNeuralNet->nMaxCluster;
	m_pNeuralNet->strInputName.Copy(pNeuralNet->strInputName);
	m_pNeuralNet->dMaxLearnRate = pNeuralNet->dMaxLearnRate;
	m_pNeuralNet->dMaxRunTime = pNeuralNet->dMaxRunTime;
	m_pNeuralNet->dMinLearnRate = pNeuralNet->dMinLearnRate;
	m_pNeuralNet->dWarningRate = pNeuralNet->dWarningRate;
	m_pNeuralNet->nMaxCluster = pNeuralNet->nMaxCluster;
	m_pNeuralNet->nNetStructure = pNeuralNet->nNetStructure;
	m_pNeuralNet->nTrainTimes = pNeuralNet->nTrainTimes;

	pResult = new CResult("神经网络分析");
//	if (!CNeuralNet::SetDataSource(pDataInterface,m_pNeuralNet->strInputName))
//	{
//		CRsltElementText * pText = new CRsltElementText("警告(神经网络数据源设置)\n");
//		strTmp.Format("神经网络数据源设置失败\n");
//		pText->AddString(strTmp);
//		pResult->Add(pText);
//		return pResult;
//	}
	m_pDataInterface = pDataInterface;
	CFieldType cType;
	CTString strProgressText;

	assert( m_pDataInterface!=NULL );

	nSampleSize = m_pDataInterface->m_DataAccess.GetRecordCount();
	nFieldCount = m_pDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
	strProgressText = _T("检查输入变量...");

//	bool bSetPara = CNeuralNet::SetParamFromVO(voString,pResult);
//	if (!bSetPara)
//	{
//		CRsltElementText * pText = new CRsltElementText("警告(神经网络参数设置)\n");
//		strTmp.Format("神经网络参数设置失败\n");
//		pText->AddString(strTmp);
//		pResult->Add(pText);
//		return false;
//	}
	pPreModel = new CNeuralNetOutput;
	pPreModel->strDataSourceName = pDataInterface->m_DataAccess.GetFileName();
	pPreModel->strInputName.Copy(pNeuralNet->strInputName);
	pPreModel->dMaxLearnRate = pNeuralNet->dMaxLearnRate;
	pPreModel->dMaxRunTime = pNeuralNet->dMaxRunTime;
	pPreModel->dMinLearnRate = pNeuralNet->dMinLearnRate;
	pPreModel->dWarningRate = pNeuralNet->dWarningRate;
	pPreModel->nMaxCluster = pNeuralNet->nMaxCluster;
	pPreModel->nNetStructure = pNeuralNet->nNetStructure;
	pPreModel->nTrainTimes = pNeuralNet->nTrainTimes;
	// 获得实际输入字段数

		nDimension = pNeuralNet->strInputName.GetSize();

		if( nDimension==0 )
		{
			CRsltElementText * pText = new CRsltElementText("警告\n");
			strTmp.Format("神经网络数据源设置失败\n");
			pText->AddString(strTmp);
			pResult->Add(pText);
			return pResult;
		}
	
		//for( i=0; i<nFieldCount; i++ )
		for( i=0; i<nDimension; i++ )
		{
			CTString strFieldName = pNeuralNet->strInputName.GetAt(i);
			CField *pField =  (CField *)m_pDataInterface->m_DataAccess.FieldByName(strFieldName);
			cType = pField->GetFieldType();
			if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
			{
				CRsltElementText * pText = new CRsltElementText("警告\n");
				strTmp.Format("神经网络数据源字段类型不符合要求\n");
				pText->AddString(strTmp);
				pResult->Add(pText);
				return pResult;
			}
			if( m_pDataInterface->IsColEmpty(m_pDataInterface->IndexByName(strFieldName)))
			{
				CRsltElementText * pText = new CRsltElementText("警告\n");
				strTmp.Format("神经网络数据源字段不能为空\n");
				pText->AddString(strTmp);
				pResult->Add(pText);
				return pResult;
			}
		}
	//////////////////////////////////////////////////
	// 动态生成网络拓扑
	unsigned int nDataCount = 0;
	BestIndividual *pBest = NULL;
	m_nTopologyType = pNeuralNet->nNetStructure;
	//m_nTopologyType = 2;
	switch(m_nTopologyType)
	{
	case 0:
	case 1:
	if( m_bTraining ) 
		{
			//////////////////////////////////////////////////////////////////////////
			//网络拓扑结果，ART网络，Kohonen网络
			m_pTopology = CTopology::Initialization(m_nTopologyType,
											nSampleSize, nDimension,
											m_nClusterNum, 
											m_dwRuntime, m_unIterateNum,
											m_fVigilance, // ART
											m_fMaxRate, m_fMinRate, // Kohonen
											m_dFeedbackRate // BP
											);
			CTStringArray fieldRole;
			fieldRole.Copy(pNeuralNet->strInputName);
			//进行训练
			//if( m_bAbort || m_pTopology->Training( m_pDataInterface, pNeuralNet->arFieldInfo, pResult, m_bOutputInfo )==false )
			if( m_bAbort || m_pTopology->Training( m_pDataInterface, fieldRole, pResult, m_bOutputInfo )==false )
			{
				return pResult;
			}
			nFlag = 1;
			nFlag <<= ( m_nTopologyType-IDS_TOPOLOGY1 );
			m_nTrained |= nFlag;
			if (m_nTopologyType == 0)      //ART网络
			{

			}
			else if (m_nTopologyType == 1) //Kohonen
			{
				pPreModel->vtKohonenWeight = ((CKohonenTopology *)m_pTopology)->GetLayerWeight();
			}
		}
		break;
	case 2:
		///////////////////////////////////////////////////////
		// 初始化样本数据（Hopfield算法需要一次装入所有样本）
		//////////////////////////////////////////////////////////////////////////
		if( m_bTraining )
		{
			nDataCount = nSampleSize * nDimension;
			m_pSampleSet = new CSampleSet;
			assert( m_pSampleSet!=NULL );
			if( m_pSampleSet->InitializeDataset( m_pDataInterface, pNeuralNet->strInputName, DOUBLETYPE )==false )
			{
				CRsltElementText * pText = new CRsltElementText("警告\n");
				strTmp.Format("神经网络数据初始化错误\n");
				pText->AddString(strTmp);
				pResult->Add(pText);
				return pResult;
			}
			if( (unsigned int)m_pDataInterface->GetRowCount()>m_pSampleSet->GetSampleNum() )
			{
				///////////////////////////////////
				// 输出数据预处理信息
				CTString strTitle, strTreated;
				strTitle.Format(   " 数据预处理:        总记录数   丢弃行数   填充均值行数\n\n" );
				strTreated.Format( "                           %8d         %8d          %8d", 
							   m_pDataInterface->GetRowCount(), 
							   m_pDataInterface->GetRowCount() - m_pSampleSet->GetSampleNum(), 
							   0 );
				CRsltElementText * pTreatedTextRslt = new CRsltElementText("数据预处理");
				pTreatedTextRslt->AddString( strTitle + strTreated.GetData() );
				pResult->Add( pTreatedTextRslt );
				///////////////////////////////////
				if( m_pSampleSet->GetSampleNum()<(unsigned int)m_nClusterNum )
				{
					CTString strWarning;
					strWarning = " 输入数据有效记录数太少，无法进行Hopfield聚类分析。 ";
					CRsltElementText * pWarningTextRslt = new CRsltElementText("注意");
					pWarningTextRslt->AddString( strWarning );
					pResult->Add( pWarningTextRslt );
					break;
				}
			}
			strProgressText = _T("构造拓扑网络结构...");
			m_pHopGen = new CHopGen;
			m_pHopGen->Initialize( m_pSampleSet->GetDataBuffer(), 
								m_pSampleSet->GetSampleNum(),
								nDimension,
								m_nClusterNum, 
								m_nTopologyType, 
								m_dwRuntime,
								m_nGenicNum, 
								m_fCrossRatio,
								m_fMutationRatio );
			if( !m_bAbort )
			{
				pBest=m_pHopGen->HopGenCluster();//进行聚类
				m_Best=*pBest;
				m_nDimension=nDimension;
				m_bHaveNetwork=true;
				m_bLoad=false;
				nFlag = 1;
				nFlag <<= m_nTopologyType;
				m_nTrained |= nFlag;

				if( pBest!=NULL )
					DisplayTopGenResult( pBest, pResult );
			}
			pPreModel->dblDistance = pBest->dblDistance;
			pPreModel->dblFitness = pBest->dblFitness;
			pPreModel->dMaxLearnRate = pBest->dblFitness;
			pPreModel->nGeneration = pBest->nGeneration;
			pPreModel->vtChrom = pBest->btChrom;
			pPreModel->vtClusterCenter = pBest->dblCenterCrd;
			pPreModel->vtClusterMax = pBest->dblRangeMax;
			pPreModel->vtClusterMin = pBest->dblRangeMin; 
			pPreModel->vtRange = pBest->dblRange;
		}
		break;
	default:
		break;
	}
	if( m_pHopGen )
	{
		delete m_pHopGen;
		m_pHopGen = NULL;
	}
	if( pBest )
	{
		delete pBest;
		pBest = NULL;
	}
	if( m_pTopology )
	{
		delete m_pTopology;
		m_pTopology = NULL;
	}
	if( m_pSampleSet )
	{
		delete m_pSampleSet;
		m_pSampleSet = NULL;
	}
	m_bAbort = false;
	return pResult;
}


bool CNeuralNet::UpdateField(TPredVO *pVO, CNeuralNetOutput *pOutput)
{
	
	int i=0, nSize=0;
	CTString szTemp("");
	CTStringArray arrTemp;
	
	if (pVO == NULL || pOutput == NULL)
		return false;
	if (!AnaSyntax::Replace(pOutput->strInputName,pVO->arFieldInfo,pVO->szArrReplace))
		return false;
	return true;
}

CResult * CNeuralNet::RunPred(CDataInterface *pDataInterface,CEnvParam &pEnv, Interaction &itt,TPredVO *pNeuralNet,CNeuralNetOutput *pPreModel)
{
	int nSampleSize, nDimension, i, nFieldCount;
	CResult * pResult = NULL;
	CFieldType cType;
	CTString strProgressText;
	m_pDataInterface = pDataInterface;

	assert( m_pDataInterface!=NULL );
		
	//模型更新
	if (!UpdateField(pNeuralNet,pPreModel))
		return pResult;
	//Start...
	nSampleSize = m_pDataInterface->m_DataAccess.GetRecordCount();
	nFieldCount = m_pDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
	strProgressText = _T("检查输入变量...");
	// 获得实际输入字段数
	nDimension = pPreModel->strInputName.GetSize();//判断输入字段名字和模型中的名字是否一致
	if( nDimension==0 )
	{
		return NULL;
	}
	if (nDimension == pNeuralNet->arFieldInfo.GetSize())
	{
		for (i=0; i<nDimension; i++)
		{
			if (pPreModel->strInputName.GetAt(i) != pNeuralNet->arFieldInfo.GetAt(i))
				return NULL;
		}
	}
	else
		return NULL;

	for( i=0; i<nDimension; i++ )
	{
		CTString strFieldName = pPreModel->strInputName.GetAt(i);
		cType = ((CField *)(m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strFieldName)))->GetFieldType();
		if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
		{
			return NULL;
		}
		if( m_pDataInterface->IsColEmpty(m_pDataInterface->IndexByName(strFieldName)) )
		{
			return NULL;
		}
	}

	//////////////////////////////////////////////////
	// 创建结果对象
	//
	pResult = new CResult("神经网络分析");
	assert( pResult!=NULL );

	//////////////////////////////////////////////////
	// 动态生成网络拓扑
	//
	unsigned int nDataCount = 0;
	BestIndividual *pBest = NULL;
	m_nTopologyType = pPreModel->nNetStructure;
	if ( m_nTopologyType == 0)
	{
		m_pTopology = CTopology::Initialization(m_nTopologyType,
			nSampleSize, nDimension, pPreModel->nMaxCluster);
		if( m_pTopology )
		{
			if( m_bAbort || m_pTopology->Testing( m_pDataInterface, pPreModel->strInputName, pResult, m_bOutputInfo )==false )
			{
				delete pResult;
				pResult = NULL;
			}
		}
	}
	else if (m_nTopologyType == 1)
	{
		m_pTopology = CTopology::Initialization(m_nTopologyType,
			nSampleSize, nDimension, pPreModel->nMaxCluster);
		((CKohonenTopology *)m_pTopology)->SetLayerWeight(pPreModel->vtKohonenWeight);
		if( m_pTopology )
		{
			if( m_bAbort || m_pTopology->Testing( m_pDataInterface, pPreModel->strInputName, pResult, m_bOutputInfo )==false )
			{
				delete pResult;
				pResult = NULL;
			}
		}
	}
	else
	{
		pBest = new BestIndividual;
		pBest->btChrom = pPreModel->vtChrom;
		pBest->dblCenterCrd = pPreModel->vtClusterCenter;
		pBest->dblDistance = pPreModel->dblDistance;
		pBest->dblFitness = pPreModel->dblFitness;
		pBest->dblRange = pPreModel->vtRange;
		pBest->dblRangeMax = pPreModel->vtClusterMax;
		pBest->dblRangeMin = pPreModel->vtClusterMin;
		pBest->nGeneration = pPreModel->nGeneration;

		m_arFieldRole.Copy(pPreModel->strInputName);
		if( pBest!=NULL && !m_bAbort )
		{
			NormalizeBestIndvd( pBest, nDimension );//归一化
			HopGenPredict( m_pDataInterface, pBest, pResult );//进行预测
		}
	}

	if( m_bAbort || pResult && pResult->Count()==0 )
	{
		delete pResult;
		pResult = NULL;
	}

	if( m_pHopGen )
	{
		delete m_pHopGen;
		m_pHopGen = NULL;
	}
	else if( pBest )
	{
		delete pBest;
	}

	pBest = NULL;

	if( m_pTopology )
	{
		delete m_pTopology;
		m_pTopology = NULL;
	}

	if( m_pSampleSet )
	{
		delete m_pSampleSet;
		m_pSampleSet = NULL;
	}

	m_bAbort = false;


	return pResult;
}
/*
CResult * CNeuralNet::Run()
{
	int nSampleSize, nDimension, i, nFlag, nFieldCount;
	CResult * pResult = NULL;
	CFieldType cType;
	CTString strProgressText;

	assert( m_pDataInterface!=NULL );

	nSampleSize = m_pDataInterface->m_DataAccess.GetRecordCount();
	nFieldCount = m_pDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
	strProgressText = _T("检查输入变量...");
	// 获得实际输入字段数
	nDimension = m_pNeuralNet->strInputName.GetSize();
	if( nDimension==0 )
	{
		return NULL;
	}

	for( i=0; i<nFieldCount; i++ )
	{
		CTString strFieldName = m_pNeuralNet->strInputName.GetAt(i);
		cType = ((CField *)(m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strFieldName)))->GetFieldType();
		if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
		{
			return NULL;
		}
		if( m_pDataInterface->IsColEmpty(m_pDataInterface->IndexByName(strFieldName)) )
		{
			return NULL;
		}
	}

	//////////////////////////////////////////////////
	// 创建结果对象
	//
	pResult = new CResult("神经网络分析");
	assert( pResult!=NULL );

	//////////////////////////////////////////////////
	// 动态生成网络拓扑
	//
	unsigned int nDataCount = 0;
	BestIndividual *pBest = NULL;

	switch( m_nTopologyType )
	{
	case IDS_TOPOLOGY1:
	case IDS_TOPOLOGY2:
		if( m_bTraining )
		{
			//////////////////////////////////////////////////////////////////////////
			//网络拓扑结果，ART网络，Kohonen网络
			m_pTopology = CTopology::Initialization( m_nTopologyType,
											nSampleSize, nDimension,
											m_nClusterNum, 
											m_dwRuntime, m_unIterateNum,
											m_fVigilance, // ART
											m_fMaxRate, m_fMinRate, // Kohonen
											m_dFeedbackRate // BP
											);
			//进行训练
			if( m_bAbort || m_pTopology->Training( m_pDataInterface, m_pNeuralNet->strInputName, pResult, m_bOutputInfo )==false )
			{
				delete pResult;
				pResult = NULL;
				break;

			}
			m_pTopology->SaveNetwork();

			nFlag = 1;
			nFlag <<= ( m_nTopologyType-IDS_TOPOLOGY1 );
			m_nTrained |= nFlag;

			SaveNetInfo();

		}
		
		if( m_bTesting )
		{
			m_pTopology = CTopology::UploadTopology( m_nTopologyType,
											nSampleSize, nDimension );
			if( m_pTopology )
			{
				if( m_bAbort || m_pTopology->Testing( m_pDataInterface, m_pNeuralNet->strInputName, pResult, m_bOutputInfo )==false )
				{
					delete pResult;
					pResult = NULL;
					break;
				}
			}
		}

		break;

	case IDS_TOPOLOGY3:
	case IDS_TOPOLOGY4:
		break;
	case IDS_TOPOLOGY5:
	case IDS_TOPOLOGY7:
		///////////////////////////////////////////////////////
		// 初始化样本数据（Hopfield算法需要一次装入所有样本）
		//////////////////////////////////////////////////////////////////////////
		if( m_bTraining )
		{
			nDataCount = nSampleSize * nDimension;

			m_pSampleSet = new CSampleSet;
			assert( m_pSampleSet!=NULL );
			if( m_pSampleSet->InitializeDataset( m_pDataInterface, m_pNeuralNet->strInputName, DOUBLETYPE )==false )
			{
			}

			if( (unsigned int)m_pDataInterface->GetRowCount()>m_pSampleSet->GetSampleNum() )
			{
				///////////////////////////////////
				// 输出数据预处理信息
				CTString strTitle, strTreated;
				strTitle.Format(   " 数据预处理:        总记录数   丢弃行数   填充均值行数\n\n" );
				strTreated.Format( "                           %8d         %8d          %8d", 
							   m_pDataInterface->GetRowCount(), 
							   m_pDataInterface->GetRowCount() - m_pSampleSet->GetSampleNum(), 
							   0 );
				CRsltElementText * pTreatedTextRslt = new CRsltElementText("数据预处理");
				pTreatedTextRslt->AddString( strTitle + strTreated );
				pResult->Add( pTreatedTextRslt );
				//
				///////////////////////////////////
				if( m_pSampleSet->GetSampleNum()==0 )
				{
					break;
				}

				if( m_pSampleSet->GetSampleNum()<(unsigned int)m_nClusterNum )
				{
					CTString strWarning;
					strWarning = " 输入数据有效记录数太少，无法进行Hopfield聚类分析。 ";
					CRsltElementText * pWarningTextRslt = new CRsltElementText("注意");
					pWarningTextRslt->AddString( strWarning );
					pResult->Add( pWarningTextRslt );
					break;
				}
					
			}

			strProgressText = _T("构造拓扑网络结构...");
		
			m_pHopGen = new CHopGen;

			m_pHopGen->Initialize( m_pSampleSet->GetDataBuffer(), 
								m_pSampleSet->GetSampleNum(),
								nDimension,
								m_nClusterNum, 
								m_nTopologyType - IDS_TOPOLOGY5, 
								m_dwRuntime,
								m_nGenicNum, 
								m_fCrossRatio,
								m_fMutationRatio );

			if( !m_bAbort )
				pBest=m_pHopGen->HopGenCluster();//进行聚类

			if( !m_bAbort )
			{
				SaveHopGenNetwork( pBest, nDimension );//保存网络
				m_Best=*pBest;
				m_nDimension=nDimension;
				m_bHaveNetwork=true;
				m_bLoad=false;

				nFlag = 1;
				nFlag <<= (m_nTopologyType-IDS_TOPOLOGY1);
				m_nTrained |= nFlag;

				SaveNetInfo();//保存信息

				if( pBest!=NULL )
					DisplayTopGenResult( pBest, pResult );
			}
		}
		
		if( m_bTesting )
		{
			//strProgressText = _T("构造拓扑网络结构...");
			pBest = GetHopGenNetwork( nDimension );
			if( pBest!=NULL && !m_bAbort )
			{
				NormalizeBestIndvd( pBest, nDimension );//归一化
				HopGenPredict( m_pDataInterface, pBest, pResult );//进行预测
			}
		}
		break;
	default:
		break;
	}

	if( m_bAbort || pResult && pResult->Count()==0 )
	{
		delete pResult;
		pResult = NULL;
	}

	if( m_pHopGen )
	{
		delete m_pHopGen;
		m_pHopGen = NULL;
	}
	else if( pBest )
	{
		delete pBest;
	}

	pBest = NULL;


	if( m_pTopology )
	{
		delete m_pTopology;
		m_pTopology = NULL;
	}

	if( m_pSampleSet )
	{
		delete m_pSampleSet;
		m_pSampleSet = NULL;
	}

	m_bAbort = false;

	pResult->Print();

	return pResult;
}
*/
void CNeuralNet::DisplayTopGenResult(BestIndividual *pBest, CResult *pResult )
{
	CTString szTemp(""), string, strVar1, strVar2, strProgressText;
	CTLTable * pTable = NULL;
	int nDimension, nSampleSize, dim, point, nCol;
	CDoubleVector pOrigData = (m_pSampleSet->GetDataBuffer());
	nDimension = m_pSampleSet->GetSampleDemision();
	//nSampleSize = m_pSampleSet->GetSampleNum();
	nSampleSize = m_pDataInterface->GetRowCount();

	strProgressText = _T("显示分析结果...");
// 	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );

	////////////////////////////////////////////////////////
	// 显示分类结果
	//
// 	TDataPointValVal* pDPS = NULL, * pClusterDPS = NULL;
	TDataPointValVal** ppDPS = NULL, ** ppClusterDPS = NULL;
	CTChartScatterPlot * pScatterplot = NULL;
	CTChartScatterPlot * pClusterPlot = NULL;

	if(m_bOutputInfo )
	{
		pTable=new CTLTable;
		pTable->CreateTable(nSampleSize+1,nDimension+2);
	}

	if( m_nTopologyType==IDS_TOPOLOGY5 )
		string.Format( "%s", "Hopfield聚类结果" );
	else
		string.Format( "遗传算法聚类结果 （最佳个体生成代：%d）", pBest->nGeneration );
	
	if( m_bOutputInfo )
	{
		pTable->SetTitle(string);

		pTable->InsertColumn(0, "记录号");
	}

	int nInputField = 0;
	for( nCol=0; nCol<m_pNeuralNet->strInputName.GetSize(); nCol++)
	{
		CTString strRole = m_pNeuralNet->strInputName.GetAt(nCol);
		CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
		
		if( m_bOutputInfo )
			pTable->InsertColumn( nInputField+1, pField->GetFieldName());

		if( strVar1.IsEmpty() )
		{
			strVar1 = pField->GetLabel();
			if( strVar1.IsEmpty() )
				strVar1 = pField->GetFieldName();
		}
		else if( strVar2.IsEmpty() )
		{
			strVar2 = pField->GetLabel();
			if( strVar2.IsEmpty() )
				strVar2 = pField->GetFieldName();
		}
		nInputField++;
	}

	if( m_bOutputInfo )
		pTable->InsertColumn( nDimension+1, "聚类" );

	int *pCount = new int[m_pNeuralNet->nMaxCluster];
	memset(pCount,0,m_pNeuralNet->nMaxCluster*sizeof(int));

	int pattern = 0;
	for( point=0; point<nSampleSize; point++ )
	{
		bool bValid = true;
		m_pDataInterface->m_DataAccess.SetCurRecNo( point+1 );
		for( dim=0; dim<m_pNeuralNet->strInputName.GetSize(); dim++)
		{
			CTString strRole = m_pNeuralNet->strInputName.GetAt(dim);
			CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
			if( pField->IsNull() )
			{
				bValid = false;
				break;
			}
		}
		if( !bValid )
			continue;
		int nIndex = pBest->btChrom[pattern];
		pCount[nIndex] += 1;
		pattern++;
	}

	if( ppDPS==NULL && nDimension==2 )
	{
		ppDPS = new TDataPointValVal*[m_pNeuralNet->nMaxCluster];
		for(dim =0;dim<m_pNeuralNet->nMaxCluster;dim++)
		{
			ppDPS[dim] = new TDataPointValVal[pCount[dim]];
		}
	}
	if( ppClusterDPS==NULL )
	{
		ppClusterDPS = new TDataPointValVal*[m_pNeuralNet->nMaxCluster];
		for(dim =0;dim<m_pNeuralNet->nMaxCluster;dim++)
		{
			ppClusterDPS[dim] = new TDataPointValVal[pCount[dim]];
		}
	}

	int *pPos = new int[m_pNeuralNet->nMaxCluster];
	memset(pPos,0,m_pNeuralNet->nMaxCluster*sizeof(int));

	pattern = 0;
	for( point=0; point<nSampleSize; point++ )
	{
		bool bValid = true;
		m_pDataInterface->m_DataAccess.SetCurRecNo( point+1 );
		for( dim=0; dim<m_pNeuralNet->strInputName.GetSize(); dim++)
		{
			CTString strRole = m_pNeuralNet->strInputName.GetAt(dim);
			CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
			if( pField->IsNull() )
			{
				bValid = false;
				break;
			}
		}

		if( !bValid )
			continue;

		string.Format( "%d", point+1 );
		if( m_bOutputInfo )
			pTable->InsertItem( point, string);
	
		int  nIndex = pBest->btChrom[pattern];
		for( dim=0; dim<nDimension; dim++ )
		{
			if( nDimension==2 )
			{
				if( dim%2==0 )
					ppDPS[nIndex][pPos[nIndex]].fXVal = pOrigData[pattern*nDimension + dim];
				else
					ppDPS[nIndex][pPos[nIndex]].fYVal = pOrigData[pattern*nDimension + dim];
			}
			if( m_bOutputInfo )
				pTable->SetItemText( point, dim+1, pOrigData[pattern*nDimension + dim] );
		}

		if( m_bOutputInfo )
			pTable->SetItemText( point, dim+1, pBest->btChrom[pattern]+1 );

//		if( pClusterDPS==NULL )
//			pClusterDPS = new TDataPointValVal[nSampleSize];
//
		ppClusterDPS[nIndex][pPos[nIndex]].fXVal = point+1;
		ppClusterDPS[nIndex][pPos[nIndex]].fYVal = pBest->btChrom[pattern]+1;
		pPos[nIndex] += 1;
		pattern++;
	}

	if( m_bOutputInfo )
	{
		CRsltElementTable *pVariable=new CRsltElementTable( "Hopfield神经网络聚类表", pTable );
		pResult->Add(pVariable);
	}

	if( ppClusterDPS!=NULL )
	{
		pClusterPlot = new CTChartScatterPlot;
		for (dim=0;dim<m_pNeuralNet->nMaxCluster;dim++)
		{
			szTemp.Format("聚类%02d",dim+1);
			pClusterPlot->SetScatter(ppClusterDPS[dim], pCount[dim],ValVal,szTemp);
		}
		CTString strTitle =  "聚类图";
		pClusterPlot->SetTitle(strTitle);
		CTString strX = "记录号";
		CTString strY = "聚类号";
		pClusterPlot->SetXAxilLabel(strX);
		pClusterPlot->SetYAxilLabel(strY);
		CRsltElementChart * pClusterChart = new CRsltElementChart( "聚类图", pClusterPlot );
		pResult->Add( pClusterChart );
	}
	else
		return;

	if( ppDPS!=NULL )
	{
		pScatterplot = new CTChartScatterPlot;
		for (dim=0;dim<m_pNeuralNet->nMaxCluster;dim++)
		{
			szTemp.Format("聚类%02d",dim+1);
			pScatterplot->SetScatter(ppDPS[dim], pCount[dim], ValVal, szTemp);
		}
		CTString strTitle = "散点图";
		pScatterplot->SetTitle(strTitle);
		pScatterplot->SetXAxilLabel(strVar1);
		pScatterplot->SetYAxilLabel(strVar2);
		CRsltElementChart * pScatterChart = new CRsltElementChart( "散点图", pScatterplot );
		pResult->Add( pScatterChart );
	}

	delete []pPos;
	delete []pCount;

	if( ppDPS )
	{
		for (dim=0;dim<m_pNeuralNet->nMaxCluster;dim++)
			delete [] ppDPS[dim];
		delete [] ppDPS;
		ppDPS = NULL;
	}

	if( ppClusterDPS )
	{
		for (dim=0;dim<m_pNeuralNet->nMaxCluster;dim++)
			delete [] ppClusterDPS[dim];
		delete []ppClusterDPS;
		ppClusterDPS = NULL;
	}

	//
	////////////////////////////////////////////////////

	////////////////////////////////////////////////////
	// 显示聚类中心坐标
	//
	CTLTable* pTableCenter=new CTLTable;

//	pTableCenter->SetBitNumAfterDot( FLOAT_PRECISION );
	pTableCenter->CreateTable(m_nClusterNum+1,nDimension+1);

	pTableCenter->SetTitle("聚类中心坐标");

	pTableCenter->InsertColumn(0, "");
	for( nCol=0; nCol<m_pNeuralNet->strInputName.GetSize(); nCol++)
	{
		CTString strRole = m_pNeuralNet->strInputName.GetAt(nCol);
		CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
		pTableCenter->InsertColumn( nCol+1, pField->GetFieldName());
	}

	for( point=0; point<m_nClusterNum; point++ )
	{
		string.Format( "聚类%02d", point+1 );
		pTableCenter->InsertItem( point, string , true );
	
		for( dim=0; dim<nDimension; dim++ )
			pTableCenter->SetItemText( point, dim+1, pBest->dblCenterCrd[point*nDimension+dim] );
	}

	CRsltElementTable *pVariableCenter=new CRsltElementTable( "聚类中心坐标表", pTableCenter );
	pResult->Add(pVariableCenter);
	//
	////////////////////////////////////////////////////

	////////////////////////////////////////////////////
	// 显示聚类范围最大值
	//
	CTLTable* pTableMax=new CTLTable;

//	pTableMax->SetBitNumAfterDot( FLOAT_PRECISION );
	pTableMax->CreateTable( m_nClusterNum+1,nDimension+1 );

	pTableMax->SetTitle("聚类范围最大值");

	pTableMax->InsertColumn(0, "");
	for( nCol=0; nCol<m_pNeuralNet->strInputName.GetSize(); nCol++)
	{
		CTString strRole = m_pNeuralNet->strInputName.GetAt(nCol);
		CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
		pTableMax->InsertColumn( nCol+1, pField->GetFieldName());
	}

	for( point=0; point<m_nClusterNum; point++ )
	{
		string.Format( "聚类%02d", point+1 );
		pTableMax->InsertItem( point, string , true );
	
		for( dim=0; dim<nDimension; dim++ )
			pTableMax->SetItemText( point, dim+1, pBest->dblRangeMax[point*nDimension+dim] );
	}

	CRsltElementTable *pVariableMax = new CRsltElementTable( "聚类范围最大值表", pTableMax );
	pResult->Add(pVariableMax);

	//
	////////////////////////////////////////////////////

	////////////////////////////////////////////////////
	// 显示聚类范围最小值
	//
	CTLTable* pTableMin=new CTLTable;

//	pTableMin->SetBitNumAfterDot( FLOAT_PRECISION );
	pTableMin->CreateTable(m_nClusterNum+1,nDimension+1);
//	pTableMin->SetCols(  );
//	pTableMin->SetRows(  );

	pTableMin->SetTitle("聚类范围最小值");

	pTableMin->InsertColumn(0, "");
	for( nCol=0; nCol<m_pNeuralNet->strInputName.GetSize(); nCol++)
	{
		CTString strRole = m_pNeuralNet->strInputName.GetAt(nCol);
		CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
		pTableMin->InsertColumn( nCol+1, pField->GetFieldName());
	}

	for( point=0; point<m_nClusterNum; point++ )
	{
		string.Format( "聚类%02d", point+1 );
		pTableMin->InsertItem( point, string , true );
	
		for( dim=0; dim<nDimension; dim++ )
			pTableMin->SetItemText( point, dim+1, pBest->dblRangeMin[point*nDimension+dim] );
	}

	CRsltElementTable *pVariableMin = new CRsltElementTable( "聚类范围最小值表", pTableMin );
	pResult->Add(pVariableMin);
	//
	////////////////////////////////////////////////////

}

void CNeuralNet::Initialize()
{
	TCLFile theFile;
	CTString strFileName;

	strFileName.Format( "NNd.stp");
	//theFile.Open( strFileName, _O_RDWR );
	theFile.Open( strFileName, TCLFile::modeReadWrite );
	theFile >> m_nTopologyType >> m_dwRuntime >> m_nClusterNum >> m_unIterateNum 
		>> m_bTraining >> m_bTesting >> m_fVigilance >> m_nGenicNum >> m_fCrossRatio 
		>> m_fMutationRatio >> m_fMaxRate >> m_fMinRate >> m_nTrained;

	theFile.Close();
}

//////////////////////////////////////////////////////////////////////////
//函数名称：GetHopGenNetwork
//输入参数：int nDimension 节点索引
//函数功能：获得HopfieldGa网络的信息
//////////////////////////////////////////////////////////////////////////
BestIndividual * CNeuralNet::GetHopGenNetwork(int nDimension)
{
	int nTopType, nSavedDimension, i, j;
	BestIndividual * pBest = NULL;
	TCLFile theFile;
	CTString strFileName;

//for load
	if(m_bLoad==true)
		strFileName=m_strLoadFileName;
	else
		strFileName.Format( "NN04d%d.tpl",(int)m_nTopologyType );
/////
	
	theFile.Open( strFileName,TCLFile::modeCreate);

	theFile >> nTopType;
	if( nTopType!= m_nTopologyType )
	{
// 		AfxMessageBox( IDS_NETWORKUPLOADERROR, MB_ICONEXCLAMATION );
		return NULL;
	}
	theFile >> nSavedDimension;
	if( nSavedDimension!=nDimension )
	{
// 		AfxMessageBox( IDS_INVALIDATEDIMENSION, MB_ICONEXCLAMATION );
		return NULL;
	}
	theFile >> m_nClusterNum >> m_nGenicNum >> m_fCrossRatio 
		>> m_fMutationRatio;

	pBest = new BestIndividual;
	assert( pBest!=NULL );

	pBest->btChrom = NULL;
	pBest->dblCenterCrd.create(m_nClusterNum*nDimension);
	pBest->dblRangeMax.create(m_nClusterNum*nDimension);
	pBest->dblRangeMin.create(m_nClusterNum*nDimension);
	pBest->dblRange.create(2*nDimension);

	for( i=0; i<m_nClusterNum; i++ )
		for( j=0; j<nDimension; j++ )
		{
			theFile >> pBest->dblCenterCrd[i*nDimension + j];
			theFile >> pBest->dblRangeMax[i*nDimension + j];
			theFile >> pBest->dblRangeMin[i*nDimension + j];
		}

// 	TRACE( "\n Get Range\n");

	for( j=0; j<nDimension; j++ )
	{
		theFile >> pBest->dblRange[j*2];
		theFile >> pBest->dblRange[j*2+1];
// 		TRACE( "%6.4f  %6.4f\n", pBest->dblRange[j*2], pBest->dblRange[j*2+1] );
	}

	theFile.Close();

	return pBest;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：HopGenPredict
//输入参数： CDataInterface *pData, BestIndividual *pBest, CResult *pResult 
//函数功能：进行预测
//////////////////////////////////////////////////////////////////////////
void CNeuralNet::HopGenPredict( CDataInterface *pData, BestIndividual *pBest, CResult *pResult )
{
	m_bOutputInfo = true;
	int i=0, nCol=0,nSize=0,nDimension;// = pData->m_DataAccess.m_pFieldList->GetFieldCount();
	int nInputField, nTreatedData = 0;
	bool bHits = false;
	CTString string, strVar1, strVar2, strProgressText;
	CTLTable * pTable = NULL;
	
	int nSameSize = pData->GetRowCount();
	TDataPointValVal **pScatterDPS = NULL, **pClusterDPS = NULL;
	CTChartScatterPlot * pScatterplot = NULL, * pClusterplot = NULL;
	//分配内存
	pScatterDPS = new TDataPointValVal *[m_nClusterNum];
	pClusterDPS = new TDataPointValVal *[m_nClusterNum];
	int *pPointCount = new int [m_nClusterNum];
	for (i=0; i<m_nClusterNum; i++)
	{
		*(pScatterDPS+i) = new TDataPointValVal[nSameSize];
		*(pClusterDPS+i) = new TDataPointValVal[nSameSize];
		*(pPointCount+i) = 0;
	}


/*
	strProgressText = _T("测试进行中...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

	//测试输入变量
	nDimension = m_arFieldRole.GetSize();

	if( m_bOutputInfo )
	{
		pTable=new CTLTable;

		pTable->SetCols(nDimension+2);
		pTable->SetTitle("Hopfield算法预测结果");
		pTable->InsertColumn(0, "记录号");
	}
	
	for( nCol=0; nCol<m_arFieldRole.GetSize(); nCol++)
	{
		CTString strField = m_arFieldRole.GetAt(nCol);
		CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strField);
		
		if( m_bOutputInfo )
			pTable->InsertColumn( nCol+1, pField->GetFieldName());
		if( strVar1.IsEmpty() )
		{
			strVar1 = pField->GetLabel();
			if( strVar1.IsEmpty() )
				strVar1 = pField->GetFieldName();
		}
		else if( strVar2.IsEmpty() )
		{
			strVar2 = pField->GetLabel();
			if( strVar2.IsEmpty() )
				strVar2 = pField->GetFieldName();
		}
	}

	if( m_bOutputInfo )
	{
		pTable->InsertColumn( nDimension, "所属聚类" );
		pTable->InsertColumn( nDimension+1, "所在位置" );
	}


	////////////////////////////////////////////////////////////
	// 保留各列最大最小值用于标准化数据
	//
	CDoubleVector adColMax(nDimension);
	CDoubleVector adColMin(nDimension);

	for( nCol=0; nCol<nDimension; nCol++ )
	{
		adColMax[nCol] = -1e100;
		adColMin[nCol] = 1e100;
	}

	int nValidRow = 0;
	pData->m_DataAccess.First();
	while( !pData->m_DataAccess.Eof() )
	{	
		bool bNullRow = false;
		for( nCol=0; nCol<m_arFieldRole.GetSize(); nCol++ )
		{
			CTString strField = m_arFieldRole.GetAt(nCol);
			CField * pField = pData->m_DataAccess.m_pFieldList->FieldByName(strField);
			if( pField->IsNull() )
			{
				bNullRow = true;
				break;
			}
		}
		if( !bNullRow )
		{
			nInputField = 0;
			for( nCol=0; nCol<m_arFieldRole.GetSize(); nCol++ )
			{
				CTString strFiled = m_arFieldRole.GetAt(nCol);
				CField * pField = pData->m_DataAccess.m_pFieldList->FieldByName(strFiled);

				if( pField->GetAsDouble()>adColMax[nInputField] )
					adColMax[nInputField] = pField->GetAsDouble();
				
				if( pField->GetAsDouble()<adColMin[nInputField] )
					adColMin[nInputField] = pField->GetAsDouble();

				nInputField++;
			}
			nValidRow++;
		}
			
		pData->m_DataAccess.Next();
	}

	//
	////////////////////////////////////////////////////////////
	//进行标准化，单位归一化

	if( m_bOutputInfo )
		pTable->SetRows( nValidRow+1 );


//	if( pDPS==NULL && nDimension==2 )
//		pDPS = new TDataPointValVal;

	int row = 0, iValidRow = 0;
	double nInterval = (double)pData->GetRowCount()/100; 
	int nInvalid = 0;
	pData->m_DataAccess.First();
	CDoubleVector adDataRow(nDimension,0.0);
	CDoubleVector vT(nDimension,0);
	while( !pData->m_DataAccess.Eof() )
	{
		CFieldList * pFieldList = pData->m_DataAccess.m_pFieldList;

		bool bValid = true;
		for( nCol=0; nCol<m_arFieldRole.GetSize(); nCol++)
		{
			CTString strField = m_arFieldRole.GetAt(nCol);
			CField * pField = pFieldList->FieldByName(strField);
			if( pField->IsNull() )
			{
				bValid = false;
				nTreatedData++;
				break;
			}
		}

		if( bValid )
		{
			string.Format( "%d", row+1 );
			if( m_bOutputInfo )
				pTable->InsertItem( iValidRow, string);
			
			//得到聚类
			int nColTemp=0;
			int nCluster=0;
			for( nCol=0; nCol<m_arFieldRole.GetSize(); nCol++)
			{
				CTString strField = m_arFieldRole.GetAt(nCol);
				CField * pField = pFieldList->FieldByName(strField);
				vT[nColTemp] = pField->GetAsDouble();
				
				vT[nColTemp] = (vT[nColTemp]-adColMin[nColTemp])/(adColMax[nColTemp]-adColMin[nColTemp]);
				nColTemp++;
			}
			int nHits = HopAndGenHitsTest( vT, pBest, nDimension, bHits );
			//添加数据
			nInputField = 0;
			for( nCol=0; nCol<m_arFieldRole.GetSize(); nCol++)
			{
				CTString strField = m_arFieldRole.GetAt(nCol);
				CField * pField = pFieldList->FieldByName(strField);
				adDataRow[nInputField] = pField->GetAsDouble();

				if( m_bOutputInfo )
					pTable->SetItemText( iValidRow, nInputField+1, adDataRow[nInputField] );
				
				//add by xds 20060628->先判断属于哪个类
				nSize = pPointCount[nHits];
				if (nDimension == 2)
				{
					if( nCol%2==0 )
						pScatterDPS[nHits][nSize].fXVal = adDataRow[nInputField];
					else
						pScatterDPS[nHits][nSize].fYVal = adDataRow[nInputField];
				}
				//
				adDataRow[nInputField] = (adDataRow[nInputField]-adColMin[nInputField])/(adColMax[nInputField]-adColMin[nInputField]);

				nInputField++;
			}
			if( m_bOutputInfo )
				pTable->SetItemText( iValidRow, nInputField+1, nHits+1 );
			//聚类 add by xds 20060628
			nSize = pPointCount[nHits];
			pClusterDPS[nHits][nSize].fXVal = row+1;
			pClusterDPS[nHits][nSize].fYVal = nHits+1;
			pPointCount[nHits] += 1;
			//
			nInvalid++;
			if(!bHits)
			{
				if( m_bOutputInfo )
					pTable->SetItemText( iValidRow, nInputField+2, "奇异点" );
			}
			iValidRow++;
		}
		pData->m_DataAccess.Next();
		row++;
	}

	//是否输出列表
	if( m_bOutputInfo )
	{
		CRsltElementTable *pVariable = new CRsltElementTable( "Hopfield分析结果表", pTable );
		pResult->Add(pVariable);
	}

	if( nTreatedData>0 )
	{
		///////////////////////////////////
		// 输出数据预处理信息
		CTString strTitle, strTreated;
		strTitle.Format(   " 数据预处理:        总记录数   丢弃行数   填充均值行数\n\n" );
		strTreated.Format( "                           %8d         %8d          %8d", 
					   pData->GetRowCount(), 
					   nTreatedData, 
					   0 );
		CRsltElementText * pTreatedTextRslt = new CRsltElementText("数据预处理");
		pTreatedTextRslt->AddString( strTitle + strTreated.GetData() );
		pResult->Add( pTreatedTextRslt );
		//
		///////////////////////////////////
	}
	//add by xds 20060628,图形
	//聚类图
	pClusterplot = new CTChartScatterPlot;
	for (i=0;i<m_nClusterNum; i++)
	{
		int n = pPointCount[i];
		string.Format( "聚类%02d", i+1 );
		pClusterplot->SetScatter(pClusterDPS[i],n,ValVal,string.GetData());
		delete []pClusterDPS[i];
	}
	pClusterplot->SetXAxilLabel("记录号");
	pClusterplot->SetYAxilLabel("聚类号");
	pClusterplot->SetTitle("聚类图");
	CRsltElementChart *pClusterChart = new CRsltElementChart( "聚类图", pClusterplot );
	pResult->Add( pClusterChart );
	//散点图
	if( pScatterDPS )
	{
		pScatterplot = new CTChartScatterPlot;
		for (i=0;i<m_nClusterNum; i++)
		{
			int n = pPointCount[i];
			string.Format( "聚类%02d", i+1 );
			pScatterplot->SetScatter(pScatterDPS[i],n,ValVal,string.GetData());
			delete []pScatterDPS[i];
		}
		pScatterplot->SetTitle("散点图");
		CRsltElementChart *pScatterChart = new CRsltElementChart( "散点图", pScatterplot );
		pResult->Add( pScatterChart );
	}
	delete []pPointCount;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：HopAndGenHitsTest
//输入参数：CDoubleVector& adData, BestIndividual *pBest, int nDimension, bool &bHits
//输出参数：选中的类
//函数功能：测试类是否选中
//////////////////////////////////////////////////////////////////////////
int CNeuralNet::HopAndGenHitsTest(CDoubleVector &adData, BestIndividual *pBest, int nDimension, bool &bHits)
{
	int i, j, nCluster = 0;
	CDoubleVector adError(m_nClusterNum);
	double dSquareError;
	double dMinError = 0;

	//获得最小误差
	for( i=0; i<m_nClusterNum; i++ )
	{
		dSquareError = 0;
		for( j=0; j<nDimension; j++ )
			dSquareError += pow( (adData[j] - pBest->dblCenterCrd[i*nDimension + j]), 2 );
		adError[i] = sqrt( dSquareError );

		if( (i==0) || (adError[i]<dMinError) )
		{
			dMinError = adError[i];
			nCluster = i;
		}
	}

	//选中的测试
	bHits = true;
	for( i=0; i<nDimension; i++ )
	{
		if( adData[i] > pBest->dblRangeMax[nCluster*nDimension+i] && 
			fabs(adData[i] - pBest->dblRangeMax[nCluster*nDimension+i])>1e-10 || 
			adData[i] < pBest->dblRangeMin[nCluster*nDimension+i] &&
			fabs(adData[i] - pBest->dblRangeMin[nCluster*nDimension+i])>1e-10 )
		{
			bHits = false;
			break;
		}
	}

	return nCluster;
}

void CNeuralNet::DeleteFiles()
{
	CTString strFileName;

	strFileName.Format( "NN04d.stp");
//	TRY
//	{
// 		CFile::Remove( strFileName );
//	}
//	CATCH( CFileException, e )
//	{
//	}
//	END_CATCH

	for( int i=0; i<HANDLEDTOPOLOGYNUM; i++ )
	{
		strFileName.Format( "NN04d%d.tpl",i );
//		TRY
//		{
//			TCLFile::Remove( strFileName );
//		}
//		CATCH( CFileException, e )
//		{
//		}
//		END_CATCH
	}

//	if( !RemoveDirectory( m_strProjectPath ) )
//	{
//	}
}

//////////////////////////////////////////////////////////////////////////
//函数名称：SetDataSource
//输入参数：CDataInterface *pDataInterface, 数据接口
//			 CPtrArray * arFieldRole  输入字段的信息
//函数功能：设置数据源的数据接口和字段的信息
//////////////////////////////////////////////////////////////////////////
bool CNeuralNet::SetDataSource(CDataInterface *pDataInterface, CTStringArray& arFieldRole)
{
	m_pDataInterface = pDataInterface;
	//m_arFieldRole.Copy(*arFieldRole);--Modified by pyx 2005-12-27
	return true;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：SetDataParam
//输入参数：CNeuralNetVO	*pNeuralNet 输入参数的接口
//函数功能：设置用户界面的参数设置
//////////////////////////////////////////////////////////////////////////
bool CNeuralNet::SetDataParam(CNeuralNetVO	*pNeuralNet)
{
	m_pNeuralNet = pNeuralNet;
	this->m_fMaxRate = pNeuralNet->dMaxLearnRate;
	this->m_fMinRate = pNeuralNet->dMinLearnRate;
	this->m_fVigilance = pNeuralNet->dWarningRate;
	this->m_fMutationRatio = pNeuralNet->dMaxRunTime;
	this->m_nClusterNum = pNeuralNet->nMaxCluster;
	this->m_nTopologyType = pNeuralNet->nNetStructure;
	this->m_nTrained = pNeuralNet->nTrainTimes;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：SetErrInfo
//输入参数：包含错误信息的CTString,CResult*&
//函数功能：将错误信息加入结果集
//////////////////////////////////////////////////////////////////////////
void CNeuralNet::SetErrInfo(CTString FormmatStr, CResult *& pResult)
{
	CRsltElementText * pText = new CRsltElementText("警告(神经网络参数设置)\n");
	pText->AddString(FormmatStr);
	pResult = new CResult("神经网络分析");
	pResult->Add(pText);
}

//////////////////////////////////////////////////////////////////////////
//函数名称：SetParamFromVO
//输入参数：包含参数信息的string
//函数功能：从VO读取并设置参数
//////////////////////////////////////////////////////////////////////////
/*
bool CNeuralNet::SetParamFromVO(string	str, CResult *& pResult)
{
	/ *
	"/FIELDSINDEX=[0,3]
	/TIMES=[100]
	/BTRAIN=[TRUE]
	/MAXRUNTIME=[100]
	/BOUTPUTINFO=[TRUE]
	/RULEFILENAME=[]
	/NETTYPE=[Kohonen]
	/WARNINGRATE=[0.5]
	/MAXCLUSTERNUM=[5]
	/MAXLEARNRATE=[0.4]
	/MINLEARNRATE=[0.01]"
	* /
	AnaWord vo;
	CTString strTmp;
	bool bImport = vo.Import(str);
	if (!bImport)
	{
		SetErrInfo("神经网络参数未能成功导入\n",pResult);		
		return false;
	}
	int count =	vo.GetWordCount();
	if (count != 11)
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	int index =0;
	//====================================================================================
	//获得s = "/FIELDSINDEX=[0,1]"
	string s = vo.GetAt(index);
	int l = s.compare(0,13,"/FIELDSINDEX=");
	if (l<0)
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = s.substr(13);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	int endPos = -1;
	CTString var;
	m_arFieldRole.RemoveAll();
	TFieldRole * pRole;

	while ((endPos = strTmp.Find(",",0)) != -1)
	{
		var = strTmp.Substr(0,endPos);
		if (var.GetLength()<=0)
		{
			SetErrInfo("神经网络参数列表格式错误\n变量名为空\n",pResult);
			return false;
		}
		int eachIndex = atol(var);
		if (eachIndex<0 || eachIndex>=m_pDataInterface->GetColCount())
		{
			SetErrInfo("神经网络参数列表格式错误\n变量索引溢出\n",pResult);
			return false;
		}
		pRole = new TFieldRole;
		pRole->nFieldNo = eachIndex;
		pRole->strFieldName = m_pDataInterface->GetFieldName(eachIndex);
		pRole->strRole = "输入变量";
		m_arFieldRole.Add(pRole);
		strTmp = strTmp.Substr(endPos+1,strTmp.GetLength()-endPos-1);
	}
	if (strTmp.GetLength()<=0)
	{
		SetErrInfo("神经网络参数列表格式错误\n变量名为空\n",pResult);
		return false;
	}
	int eachIndex = atol(strTmp);
	if (eachIndex<0 || eachIndex>=m_pDataInterface->GetColCount())
	{
		SetErrInfo("神经网络参数列表格式错误\n变量索引溢出\n",pResult);
		return false;
	}
	pRole = new TFieldRole;
	pRole->nFieldNo = eachIndex;
	pRole->strFieldName = m_pDataInterface->GetFieldName(eachIndex);
	pRole->strRole = "输入变量";
	m_arFieldRole.Add(pRole);
	
	index++;
	//====================================================================================
	//获得 s = "/TIMES=[100]"
	s = vo.GetAt(index);
	l = s.compare(0,7,"/TIMES=");
	if (l<0)
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = s.substr(7);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	int iValue = atol(strTmp);
	if (iValue<2)
	{
		SetErrInfo("神经网络参数不合理\n最大迭代次数应为大于1的整数!\n",pResult);
		return false;
	}
	m_nTrained = iValue;

	index++;
	//====================================================================================
	//获得 s = "/BTRAIN=[TRUE]"
	s = vo.GetAt(index);
	l = s.compare(0,8,"/BTRAIN=");
	if (l<0)
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = s.substr(8);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	if (strTmp == "TRUE")
	{
		m_bTraining = TRUE;
	}
	else if (strTmp == "FALSE")
	{
		m_bTraining = FALSE;
	}
	else
	{
		SetErrInfo("神经网络参数不合理\n训练/测试!\n",pResult);
		return false;
	}

	index++;
	
	//====================================================================================
	//获得 s = "/MAXRUNTIME=[100]"
	s = vo.GetAt(index);
	l = s.compare(0,12,"/MAXRUNTIME=");
	if (l<0)
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = s.substr(12);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	unsigned int uValue = atol(strTmp);
	if (iValue<=0)
	{
		SetErrInfo("神经网络参数不合理\n最长运行时间应该大于零!\n",pResult);
		return false;
	}
	m_dwRuntime = uValue;
	
	index++;
	
	//====================================================================================
	//获得 s = "/BOUTPUTINFO=[TRUE]"
	s = vo.GetAt(index);
	l = s.compare(0,13,"/BOUTPUTINFO=");
	if (l<0)
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = s.substr(13);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	if (strTmp == "TRUE")
	{
		m_bOutputInfo = TRUE;
	}
	else if (strTmp == "FALSE")
	{
		m_bOutputInfo = FALSE;
	}
	else
	{
		SetErrInfo("神经网络参数不合理\n输出信息!\n",pResult);
		return false;
	}

	index++;
	if (m_bTraining == TRUE || !m_bHaveNetwork)
	{
		m_strLoadFileName.Empty();
	}
	else
	{
		//获取 s = "/RULEFILENAME=[]"
		s = vo.GetAt(index);
		l = s.compare(0,14,"/RULEFILENAME=");
		if (l<0)
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = s.substr(14);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		m_strLoadFileName = strTmp;
	}

	index++;
	
	//====================================================================================
	//获得 s = "/NETTYPE=[Kohonen]"
	s = vo.GetAt(index);
	l = s.compare(0,9,"/NETTYPE=");
	if (l<0)
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = s.substr(9);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("神经网络参数列表格式错误\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	if (strTmp == "ART")
	{
		m_nTopologyType = 1;
		//==========================================================
		//读入警觉度
		index++;
		s = vo.GetAt(index);
	}
	else if (strTmp == "Kohonen")
	{
		m_nTopologyType = 2;
		//==========================================================
		//读入最大聚类个数
		index = index+2;
		//获得 s = "/MAXCLUSTERNUM=[5]"
		s = vo.GetAt(index);
		l = s.compare(0,15,"/MAXCLUSTERNUM=");
		if (l<0)
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = s.substr(15);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		iValue = atol(strTmp);
		if (iValue<2)
		{
			SetErrInfo("神经网络参数不合理\n最大聚类个数应为大于1的整数!\n",pResult);
			return false;
		}
		m_nClusterNum = iValue;

		index++;
		//=============================================================
		//获得最大学习速率 s = "/MAXLEARNRATE=[0.4]"
		s = vo.GetAt(index);
		l = s.compare(0,14,"/MAXLEARNRATE=");
		if (l<0)
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = s.substr(14);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		double dValue = atof(strTmp);
		if (dValue<=0 || dValue>=1)
		{
			SetErrInfo("神经网络参数不合理\n最大学习速率应为0和1之间的数!\n",pResult);
			return false;
		}
		m_fMaxRate = dValue;

		index++;
		//==================================================================
		//获得最小学习速率 s = "/MINLEARNRATE=[0.01]"
		s = vo.GetAt(index);
		l = s.compare(0,14,"/MINLEARNRATE=");
		if (l<0)
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = s.substr(14);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		dValue = atof(strTmp);
		if (dValue<=0 || dValue>=1)
		{
			SetErrInfo("神经网络参数不合理\n最小学习速率应为0和1之间的数!\n",pResult);
			return false;
		}
		m_fMinRate = dValue;
	}
	else if (strTmp == "Hopfield")
	{
		m_nTopologyType = 5;
		//=====================================================================
		//读入最大聚类个数
		index = index+2;
		//获得 s = "/MAXCLUSTERNUM=[5]"
		s = vo.GetAt(index);
		l = s.compare(0,15,"/MAXCLUSTERNUM=");
		if (l<0)
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = s.substr(15);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("神经网络参数列表格式错误\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		int value = atol(strTmp);
		if (value<2)
		{
			SetErrInfo("神经网络参数不合理\n最大聚类个数应为大于1的整数!\n",pResult);
			return false;
		}
		m_nClusterNum = value;
	}
	else
	{
		SetErrInfo("错误的神经网络类型参数！\n",pResult);
		return false;
	}
	
	return true;
}
*/

//////////////////////////////////////////////////////////////////////////
//函数名称：HaveTarget
//行数功能：测试是否有目标变量
//////////////////////////////////////////////////////////////////////////
bool CNeuralNet::HaveTarget()
{
//	for( int i=0; i<m_arFieldRole.GetSize(); i++ )
//	{
//		TFieldRole * pRole = (TFieldRole*)(m_arFieldRole.GetAt(i));
//		if( pRole->strRole==CTString("目标变量") )
//			return true;
//	}
	return false;
}

void CNeuralNet::Abort()
{
	m_bAbort = true;
	if( m_pTopology )
		m_pTopology->Abort();
	if( m_pHopGen )
		m_pHopGen->Abort();
}

//////////////////////////////////////////////////////////////////////////
//函数名称：NormalizeBestIndvd
//输入参数：BestIndividual * pBest, 遗传算法个体的参数
//			int nDimension		节点的数目
//函数功能：归一化数据
//////////////////////////////////////////////////////////////////////////
void CNeuralNet::NormalizeBestIndvd( BestIndividual * pBest, int nDimension )
{
	for( int i=0; i<m_nClusterNum && !m_bAbort; i++ ){
		for( int j=0; j<nDimension; j++ ){
			pBest->dblCenterCrd[i*nDimension+j] = (pBest->dblCenterCrd[i*nDimension+j] - pBest->dblRange[2*j+0]) / (pBest->dblRange[2*j+1] - pBest->dblRange[2*j+0]);
			pBest->dblRangeMax[i*nDimension+j] = (pBest->dblRangeMax[i*nDimension+j] - pBest->dblRange[2*j+0]) / (pBest->dblRange[2*j+1] - pBest->dblRange[2*j+0]);
			pBest->dblRangeMin[i*nDimension+j] = (pBest->dblRangeMin[i*nDimension+j] - pBest->dblRange[2*j+0]) / (pBest->dblRange[2*j+1] - pBest->dblRange[2*j+0]);
		}
	}
}

