//////////////////////////////////////////////////////////////////////
// NeuralNet.cpp                                                    //
//																	//
// Define:        ���������ʵ��                                  //
// Creator:       ���                                              //
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
	nNetStructure = 2;//����ṹ��0��Ӧ��������ART���磬1 Kohonen����֯���磬2 Hopfield�㷨
	nMaxCluster = 5;//���ľ������
	nTrainTimes = 50000;//���еĴ���
	dMaxLearnRate = 0.4;//����ѧϰ��
	dMinLearnRate = 0.01;//��Сѧϰ��
	dWarningRate = 0.5;//������
	dMaxRunTime = -1;//�������ʱ�䣬-1Ϊû����ʱ
	//Hopfield����
    nGeneration = 10; /* ��Ѹ������ɴ� */
    dblFitness = -1.0;  /* ��Ѹ�����Ӧ�� */
    dblDistance = -1.0; /* �����Ӧ�ĸ��㵽�����������ĵľ���֮�� */
}

//////////////////////////////////////////////////////
//����ѵ�������ݣ�20060628��Add by xds.
//////////////////////////////////////////////////////
bool CNeuralNetOutput::Export(std::string szFileName)
{
	int i=0, nSize=0;
	CTString szTemp(""), szText("");
	TCLFile theFile;
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Javaʹ��
	//�㷨����
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += (nNetStructure == 0) ? ART : (nNetStructure == 1 ? KOHONEN : HPFIELD);
	//����Դ
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" + strDataSourceName + "\"";
	//������� 
	szText += " ";
	szText += VARINPUT;
	nSize = strInputName.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " ;
		szText+= strInputName.GetAt(i);
	}
	//Ŀ�����
	//szText += " ";
	//szText += VAROUTPUT;
	//szText += " " + strOutputName;
	//����ʱ��
	SYSTEMTIME st;
	GetLocalTime(&st);
	szTemp.Format("%d��%d��%d��%dʱ%d��%d��",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	szText += " ";
	szText += DATETIME;
	szText += " " + szTemp + "\r\n";
	//����
	nLen = szText.GetLength() + sizeof(int);//�ܵĳ���
	theFile << nLen;
	nLen = theFile.WriteString(szText.GetData());
	//��ʼдģ�Ͳ���
	theFile.SeekToEnd();
	//���е�����
	nSize = strInputName.GetSize();
	theFile << nSize;              //�����ֶθ���
	for (i=0; i<nSize; i++)
	{
		theFile << strInputName.GetAt(i);
	}
	theFile << nNetStructure;      //����ṹ
	theFile << nMaxCluster;        //���ľ������
	theFile << nTrainTimes;        //���еĴ���
	theFile << dMaxLearnRate;      //����ѧϰ��
	theFile << dMinLearnRate;      //��Сѧϰ��
	theFile << dWarningRate;       //������
	theFile << dMaxRunTime;        //�������ʱ�䣬-1Ϊû����ʱ
	//����
	if (nNetStructure == 0)        //ART����
	{
		theFile << nResetCount;
		theFile << nClusterCount;
		theFile << nClusterRange;
		theFile << dVigilance;
		theFile << nChampCluster;
		nSize = vtInterfaceWeight.vlen();
		theFile << nSize;          //�ܵ�������
		for (i=0; i<nSize; i++)
		{
			theFile << vtInterfaceWeight(i);//�ӿڲ��Ȩֵ��
		}
		nSize = vtClusterWeight.vlen();
		theFile << nSize;          //�ܵ�������
		for (i=0; i<nSize; i++)
		{
			theFile << vtClusterWeight(i);//������Ȩֵ��
		}
	}
	else if (nNetStructure == 1)   //Kohonen����֯����
	{
		nSize = vtKohonenWeight.vlen();
		theFile << nSize;          //�ܵ�������
		for (i=0; i<nSize; i++)
		{
			theFile << vtKohonenWeight(i);//Ȩֵ
		}
	}
	else if (nNetStructure == 2)   //Hopfield�㷨
	{
		theFile << nGeneration;    //��Ѹ������ɴ�
		theFile << dblFitness;     //��Ѹ�����Ӧ��
		theFile << dblDistance;    //�����Ӧ�ĸ��㵽�����������ĵľ���֮��
		nSize = vtChrom.vlen();
		theFile << nSize;          //�ܵ�������
		for (i=0; i<nSize; i++)
		{
			theFile << vtChrom(i); //��Ѹ���Ⱦɫ��
		}
		nSize = vtClusterCenter.vlen();
		theFile << nSize;          //�ܵ�������
		for (i=0; i<nSize; i++)
		{
			theFile << vtClusterCenter(i); //����������
		}
		nSize = vtClusterMax.vlen();
		theFile << nSize;          //�ܵ�������
		for (i=0; i<nSize; i++)
		{
			theFile << vtClusterMax(i); //���෶Χ���ֵ
		}
		nSize = vtClusterMin.vlen();
		theFile << nSize;          //�ܵ�������
		for (i=0; i<nSize; i++)
		{
			theFile << vtClusterMin(i); //���෶Χ��Сֵ
		}
		nSize = vtRange.vlen();
		theFile << nSize;          //�ܵ�������
		for (i=0; i<nSize; i++)
		{
			theFile << vtRange(i); //ԭʼ���������Сֵ
		}
	}
	theFile.Close();
	return true;
}
//////////////////////////////////////////////////////
//����ѵ�������ݣ�20060628��Add by xds.
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
	//��β����
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//��ʼдģ�Ͳ���
	//���е�����
	theFile >> nSize;              //�����ֶθ���
	strInputName.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> szTemp;
		strInputName.Add(szTemp);
	}
	theFile >> nNetStructure;      //����ṹ
	theFile >> nMaxCluster;        //���ľ������
	theFile >> nTrainTimes;        //���еĴ���
	theFile >> dMaxLearnRate;      //����ѧϰ��
	theFile >> dMinLearnRate;      //��Сѧϰ��
	theFile >> dWarningRate;       //������
	theFile >> dMaxRunTime;        //�������ʱ�䣬-1Ϊû����ʱ
	//����
	if (nNetStructure == 0)        //ART����
	{
		theFile >> nResetCount;
		theFile >> nClusterCount;
		theFile >> nClusterRange;
		theFile >> dVigilance;
		theFile >> nChampCluster;
		theFile >> nSize;          //�ܵ�������
		if (nSize >0) 
		{
			vtInterfaceWeight.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtInterfaceWeight(i) = dValue;//�ӿڲ��Ȩֵ��
		}
		theFile >> nSize;          //�ܵ�������
		if (nSize >0) 
		{
			vtClusterWeight.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtClusterWeight(i) = dValue;//������Ȩֵ��
		}
	}
	else if (nNetStructure == 1)   //Kohonen����֯����
	{
		theFile >> nSize;          //�ܵ�������
		if (nSize >0) 
		{
			vtKohonenWeight.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtKohonenWeight(i) = dValue;//Ȩֵ
		}
	}
	else if (nNetStructure == 2)   //Hopfield�㷨
	{
		theFile >> nGeneration;    //��Ѹ������ɴ�
		theFile >> dblFitness;     //��Ѹ�����Ӧ��
		theFile >> dblDistance;    //�����Ӧ�ĸ��㵽�����������ĵľ���֮��
	
		theFile >> nSize;          //�ܵ�������
		if (nSize >0) 
		{
			vtChrom.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> nValue;
			vtChrom(i) = dValue;   //��Ѹ���Ⱦɫ��
		}
		theFile >> nSize;          //�ܵ�������
		if (nSize >0) 
		{
			vtClusterCenter.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtClusterCenter(i) = dValue;   //����������
		}
		theFile >> nSize;              //�ܵ�������
		if (nSize >0) 
		{
			vtClusterMax.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtClusterMax(i) = dValue;   //���෶Χ���ֵ
		}
		theFile >> nSize;              //�ܵ�������
		if (nSize >0) 
		{
			vtClusterMin.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtClusterMin(i) = dValue;  //���෶Χ��Сֵ
		}
		theFile >> nSize;              //�ܵ�������
		if (nSize >0) 
		{
			vtRange.create(nSize);
		}
		for (i=0; i<nSize; i++)
		{
			theFile >> dValue;
			vtRange(i) = dValue;       //ԭʼ���������Сֵ
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

	//	int		nNetStructure;//����ṹ��0��Ӧ��������ART���磬1 Kohonen����֯���磬2 Hopfield�㷨
	sprintf(buf,"i%di",nNetStructure);
	str += buf;
	//	int		nMaxCluster;//���ľ������
	sprintf(buf,"i%di",nMaxCluster);
	str += buf;
	//	int		nTrainTimes;//���еĴ���
	sprintf(buf,"i%di",nTrainTimes);
	str += buf;
	//	double	dMaxLearnRate;//����ѧϰ��
	sprintf(buf,"f%ff",dMaxLearnRate);
	str += buf;
	//	double	dMinLearnRate;//��Сѧϰ��
	sprintf(buf,"f%ff",dMinLearnRate);
	str += buf;
	//	double	dWarningRate;//������
	sprintf(buf,"f%ff",dWarningRate);
	str += buf;
	//	double	dMaxRunTime;//�������ʱ�䣬-1Ϊû����ʱ
	sprintf(buf,"f%ff",dMaxRunTime);
	str += buf;
	//	//Hopfield����
	//    int     nGeneration; // ��Ѹ������ɴ� 
	sprintf(buf,"i%di",nGeneration);
	str += buf;
	//    double  dblFitness;  // ��Ѹ�����Ӧ��
	sprintf(buf,"f%ff",dblFitness);
	str += buf;
	//    double  dblDistance; // �����Ӧ�ĸ��㵽�����������ĵľ���֮�� 
	sprintf(buf,"f%ff",dblDistance);
	str += buf;
	//	  CIntVector  vtChrom; // ��Ѹ���Ⱦɫ�� 
	int nLen = vtChrom.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtChrom(i);
		sprintf(buf,"i%di",dValue);
		str += buf;
	}
	//	  CDoubleVector	vtClusterCenter;//����������
	nLen = vtClusterCenter.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtClusterCenter(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	CDoubleVector	vtClusterMax;//���෶Χ���ֵ
	nLen = vtClusterMax.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtClusterMax(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	CDoubleVector	vtClusterMin;//���෶Χ��Сֵ
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
	//	//Kohonen����֯��������
	//	CDoubleVector	vtKohonenWeight;//Ȩֵ
	nLen = vtKohonenWeight.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtKohonenWeight(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	//ART��������
	//	CDoubleVector	vtInterfaceWeight;//�ӿڲ��Ȩֵ��
	nLen = vtInterfaceWeight.vlen();
	sprintf(buf,"i%di",nLen);
	str += buf;
	for (i=0;i<nLen;i++)
	{
		dValue = vtInterfaceWeight(i);
		sprintf(buf,"f%ff",dValue);
		str += buf;
	}
	//	CDoubleVector	vtClusterWeight;//������Ȩֵ��
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

	//	int		nNetStructure;//����ṹ��0��Ӧ��������ART���磬1 Kohonen����֯���磬2 Hopfield�㷨
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nNetStructure = atoi(buf);
	//	int		nMaxCluster;//���ľ������
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nMaxCluster = atoi(buf);
	//	int		nTrainTimes;//���еĴ���
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nTrainTimes = atoi(buf);
	//	double	dMaxLearnRate;//����ѧϰ��
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dMaxLearnRate = atof(buf);
	//	double	dMinLearnRate;//��Сѧϰ��
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dMinLearnRate = atof(buf);
	//	double	dWarningRate;//������
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dWarningRate = atof(buf);
	//	double	dMaxRunTime;//�������ʱ�䣬-1Ϊû����ʱ
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dMaxRunTime = atof(buf);
	//	//Hopfield����
	//    int     nGeneration; // ��Ѹ������ɴ� 
	len = str_cpy(buf,pCurPos,'i');
	if (len == 0)
		return false;
	pCurPos += len;
	nGeneration = atoi(buf);
	//    double  dblFitness;  // ��Ѹ�����Ӧ�� 
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dblFitness = atof(buf);
	
	//    double  dblDistance; // �����Ӧ�ĸ��㵽�����������ĵľ���֮��
	len = str_cpy(buf,pCurPos,'f');
	if (len == 0)
		return false;
	pCurPos += len;
	dblDistance = atof(buf);
	//	CIntVector  vtChrom; // ��Ѹ���Ⱦɫ�� 
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
	//	CDoubleMatrix	mtClusterCenter;//����������
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
	//	CDoubleMatrix	mtClusterMax;//���෶Χ���ֵ
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
	//	CDoubleMatrix   mtClusterMin;//���෶Χ��Сֵ
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
	//	//Kohonen����֯��������
	//	CDoubleMatrix	mtKohonenWeith;//Ȩֵ
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
	//	//ART��������
	//	CDoubleMatrix	mtInterfaceWeight;//�ӿڲ��Ȩֵ��
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
	//	CDoubleMatrix	mtClusterWeight;//������Ȩֵ��
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

	// ART�㷨
	m_fVigilance = 0.5;

	// Kohonen�㷨
	m_fMaxRate = 0.4;
	m_fMinRate = 0.01;

	// BP�㷨
	m_dFeedbackRate = 0.0;

	// �Ŵ��㷨
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
	//add by xds 20060601.ʵ���������������
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

	pResult = new CResult("���������");
//	if (!CNeuralNet::SetDataSource(pDataInterface,m_pNeuralNet->strInputName))
//	{
//		CRsltElementText * pText = new CRsltElementText("����(����������Դ����)\n");
//		strTmp.Format("����������Դ����ʧ��\n");
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
	strProgressText = _T("����������...");

//	bool bSetPara = CNeuralNet::SetParamFromVO(voString,pResult);
//	if (!bSetPara)
//	{
//		CRsltElementText * pText = new CRsltElementText("����(�������������)\n");
//		strTmp.Format("�������������ʧ��\n");
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
	// ���ʵ�������ֶ���

		nDimension = pNeuralNet->strInputName.GetSize();

		if( nDimension==0 )
		{
			CRsltElementText * pText = new CRsltElementText("����\n");
			strTmp.Format("����������Դ����ʧ��\n");
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
				CRsltElementText * pText = new CRsltElementText("����\n");
				strTmp.Format("����������Դ�ֶ����Ͳ�����Ҫ��\n");
				pText->AddString(strTmp);
				pResult->Add(pText);
				return pResult;
			}
			if( m_pDataInterface->IsColEmpty(m_pDataInterface->IndexByName(strFieldName)))
			{
				CRsltElementText * pText = new CRsltElementText("����\n");
				strTmp.Format("����������Դ�ֶβ���Ϊ��\n");
				pText->AddString(strTmp);
				pResult->Add(pText);
				return pResult;
			}
		}
	//////////////////////////////////////////////////
	// ��̬������������
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
			//�������˽����ART���磬Kohonen����
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
			//����ѵ��
			//if( m_bAbort || m_pTopology->Training( m_pDataInterface, pNeuralNet->arFieldInfo, pResult, m_bOutputInfo )==false )
			if( m_bAbort || m_pTopology->Training( m_pDataInterface, fieldRole, pResult, m_bOutputInfo )==false )
			{
				return pResult;
			}
			nFlag = 1;
			nFlag <<= ( m_nTopologyType-IDS_TOPOLOGY1 );
			m_nTrained |= nFlag;
			if (m_nTopologyType == 0)      //ART����
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
		// ��ʼ���������ݣ�Hopfield�㷨��Ҫһ��װ������������
		//////////////////////////////////////////////////////////////////////////
		if( m_bTraining )
		{
			nDataCount = nSampleSize * nDimension;
			m_pSampleSet = new CSampleSet;
			assert( m_pSampleSet!=NULL );
			if( m_pSampleSet->InitializeDataset( m_pDataInterface, pNeuralNet->strInputName, DOUBLETYPE )==false )
			{
				CRsltElementText * pText = new CRsltElementText("����\n");
				strTmp.Format("���������ݳ�ʼ������\n");
				pText->AddString(strTmp);
				pResult->Add(pText);
				return pResult;
			}
			if( (unsigned int)m_pDataInterface->GetRowCount()>m_pSampleSet->GetSampleNum() )
			{
				///////////////////////////////////
				// �������Ԥ������Ϣ
				CTString strTitle, strTreated;
				strTitle.Format(   " ����Ԥ����:        �ܼ�¼��   ��������   ����ֵ����\n\n" );
				strTreated.Format( "                           %8d         %8d          %8d", 
							   m_pDataInterface->GetRowCount(), 
							   m_pDataInterface->GetRowCount() - m_pSampleSet->GetSampleNum(), 
							   0 );
				CRsltElementText * pTreatedTextRslt = new CRsltElementText("����Ԥ����");
				pTreatedTextRslt->AddString( strTitle + strTreated.GetData() );
				pResult->Add( pTreatedTextRslt );
				///////////////////////////////////
				if( m_pSampleSet->GetSampleNum()<(unsigned int)m_nClusterNum )
				{
					CTString strWarning;
					strWarning = " ����������Ч��¼��̫�٣��޷�����Hopfield��������� ";
					CRsltElementText * pWarningTextRslt = new CRsltElementText("ע��");
					pWarningTextRslt->AddString( strWarning );
					pResult->Add( pWarningTextRslt );
					break;
				}
			}
			strProgressText = _T("������������ṹ...");
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
				pBest=m_pHopGen->HopGenCluster();//���о���
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
		
	//ģ�͸���
	if (!UpdateField(pNeuralNet,pPreModel))
		return pResult;
	//Start...
	nSampleSize = m_pDataInterface->m_DataAccess.GetRecordCount();
	nFieldCount = m_pDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
	strProgressText = _T("����������...");
	// ���ʵ�������ֶ���
	nDimension = pPreModel->strInputName.GetSize();//�ж������ֶ����ֺ�ģ���е������Ƿ�һ��
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
	// �����������
	//
	pResult = new CResult("���������");
	assert( pResult!=NULL );

	//////////////////////////////////////////////////
	// ��̬������������
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
			NormalizeBestIndvd( pBest, nDimension );//��һ��
			HopGenPredict( m_pDataInterface, pBest, pResult );//����Ԥ��
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
	strProgressText = _T("����������...");
	// ���ʵ�������ֶ���
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
	// �����������
	//
	pResult = new CResult("���������");
	assert( pResult!=NULL );

	//////////////////////////////////////////////////
	// ��̬������������
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
			//�������˽����ART���磬Kohonen����
			m_pTopology = CTopology::Initialization( m_nTopologyType,
											nSampleSize, nDimension,
											m_nClusterNum, 
											m_dwRuntime, m_unIterateNum,
											m_fVigilance, // ART
											m_fMaxRate, m_fMinRate, // Kohonen
											m_dFeedbackRate // BP
											);
			//����ѵ��
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
		// ��ʼ���������ݣ�Hopfield�㷨��Ҫһ��װ������������
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
				// �������Ԥ������Ϣ
				CTString strTitle, strTreated;
				strTitle.Format(   " ����Ԥ����:        �ܼ�¼��   ��������   ����ֵ����\n\n" );
				strTreated.Format( "                           %8d         %8d          %8d", 
							   m_pDataInterface->GetRowCount(), 
							   m_pDataInterface->GetRowCount() - m_pSampleSet->GetSampleNum(), 
							   0 );
				CRsltElementText * pTreatedTextRslt = new CRsltElementText("����Ԥ����");
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
					strWarning = " ����������Ч��¼��̫�٣��޷�����Hopfield��������� ";
					CRsltElementText * pWarningTextRslt = new CRsltElementText("ע��");
					pWarningTextRslt->AddString( strWarning );
					pResult->Add( pWarningTextRslt );
					break;
				}
					
			}

			strProgressText = _T("������������ṹ...");
		
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
				pBest=m_pHopGen->HopGenCluster();//���о���

			if( !m_bAbort )
			{
				SaveHopGenNetwork( pBest, nDimension );//��������
				m_Best=*pBest;
				m_nDimension=nDimension;
				m_bHaveNetwork=true;
				m_bLoad=false;

				nFlag = 1;
				nFlag <<= (m_nTopologyType-IDS_TOPOLOGY1);
				m_nTrained |= nFlag;

				SaveNetInfo();//������Ϣ

				if( pBest!=NULL )
					DisplayTopGenResult( pBest, pResult );
			}
		}
		
		if( m_bTesting )
		{
			//strProgressText = _T("������������ṹ...");
			pBest = GetHopGenNetwork( nDimension );
			if( pBest!=NULL && !m_bAbort )
			{
				NormalizeBestIndvd( pBest, nDimension );//��һ��
				HopGenPredict( m_pDataInterface, pBest, pResult );//����Ԥ��
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

	strProgressText = _T("��ʾ�������...");
// 	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );

	////////////////////////////////////////////////////////
	// ��ʾ������
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
		string.Format( "%s", "Hopfield������" );
	else
		string.Format( "�Ŵ��㷨������ ����Ѹ������ɴ���%d��", pBest->nGeneration );
	
	if( m_bOutputInfo )
	{
		pTable->SetTitle(string);

		pTable->InsertColumn(0, "��¼��");
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
		pTable->InsertColumn( nDimension+1, "����" );

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
		CRsltElementTable *pVariable=new CRsltElementTable( "Hopfield����������", pTable );
		pResult->Add(pVariable);
	}

	if( ppClusterDPS!=NULL )
	{
		pClusterPlot = new CTChartScatterPlot;
		for (dim=0;dim<m_pNeuralNet->nMaxCluster;dim++)
		{
			szTemp.Format("����%02d",dim+1);
			pClusterPlot->SetScatter(ppClusterDPS[dim], pCount[dim],ValVal,szTemp);
		}
		CTString strTitle =  "����ͼ";
		pClusterPlot->SetTitle(strTitle);
		CTString strX = "��¼��";
		CTString strY = "�����";
		pClusterPlot->SetXAxilLabel(strX);
		pClusterPlot->SetYAxilLabel(strY);
		CRsltElementChart * pClusterChart = new CRsltElementChart( "����ͼ", pClusterPlot );
		pResult->Add( pClusterChart );
	}
	else
		return;

	if( ppDPS!=NULL )
	{
		pScatterplot = new CTChartScatterPlot;
		for (dim=0;dim<m_pNeuralNet->nMaxCluster;dim++)
		{
			szTemp.Format("����%02d",dim+1);
			pScatterplot->SetScatter(ppDPS[dim], pCount[dim], ValVal, szTemp);
		}
		CTString strTitle = "ɢ��ͼ";
		pScatterplot->SetTitle(strTitle);
		pScatterplot->SetXAxilLabel(strVar1);
		pScatterplot->SetYAxilLabel(strVar2);
		CRsltElementChart * pScatterChart = new CRsltElementChart( "ɢ��ͼ", pScatterplot );
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
	// ��ʾ������������
	//
	CTLTable* pTableCenter=new CTLTable;

//	pTableCenter->SetBitNumAfterDot( FLOAT_PRECISION );
	pTableCenter->CreateTable(m_nClusterNum+1,nDimension+1);

	pTableCenter->SetTitle("������������");

	pTableCenter->InsertColumn(0, "");
	for( nCol=0; nCol<m_pNeuralNet->strInputName.GetSize(); nCol++)
	{
		CTString strRole = m_pNeuralNet->strInputName.GetAt(nCol);
		CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
		pTableCenter->InsertColumn( nCol+1, pField->GetFieldName());
	}

	for( point=0; point<m_nClusterNum; point++ )
	{
		string.Format( "����%02d", point+1 );
		pTableCenter->InsertItem( point, string , true );
	
		for( dim=0; dim<nDimension; dim++ )
			pTableCenter->SetItemText( point, dim+1, pBest->dblCenterCrd[point*nDimension+dim] );
	}

	CRsltElementTable *pVariableCenter=new CRsltElementTable( "�������������", pTableCenter );
	pResult->Add(pVariableCenter);
	//
	////////////////////////////////////////////////////

	////////////////////////////////////////////////////
	// ��ʾ���෶Χ���ֵ
	//
	CTLTable* pTableMax=new CTLTable;

//	pTableMax->SetBitNumAfterDot( FLOAT_PRECISION );
	pTableMax->CreateTable( m_nClusterNum+1,nDimension+1 );

	pTableMax->SetTitle("���෶Χ���ֵ");

	pTableMax->InsertColumn(0, "");
	for( nCol=0; nCol<m_pNeuralNet->strInputName.GetSize(); nCol++)
	{
		CTString strRole = m_pNeuralNet->strInputName.GetAt(nCol);
		CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
		pTableMax->InsertColumn( nCol+1, pField->GetFieldName());
	}

	for( point=0; point<m_nClusterNum; point++ )
	{
		string.Format( "����%02d", point+1 );
		pTableMax->InsertItem( point, string , true );
	
		for( dim=0; dim<nDimension; dim++ )
			pTableMax->SetItemText( point, dim+1, pBest->dblRangeMax[point*nDimension+dim] );
	}

	CRsltElementTable *pVariableMax = new CRsltElementTable( "���෶Χ���ֵ��", pTableMax );
	pResult->Add(pVariableMax);

	//
	////////////////////////////////////////////////////

	////////////////////////////////////////////////////
	// ��ʾ���෶Χ��Сֵ
	//
	CTLTable* pTableMin=new CTLTable;

//	pTableMin->SetBitNumAfterDot( FLOAT_PRECISION );
	pTableMin->CreateTable(m_nClusterNum+1,nDimension+1);
//	pTableMin->SetCols(  );
//	pTableMin->SetRows(  );

	pTableMin->SetTitle("���෶Χ��Сֵ");

	pTableMin->InsertColumn(0, "");
	for( nCol=0; nCol<m_pNeuralNet->strInputName.GetSize(); nCol++)
	{
		CTString strRole = m_pNeuralNet->strInputName.GetAt(nCol);
		CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strRole);
		pTableMin->InsertColumn( nCol+1, pField->GetFieldName());
	}

	for( point=0; point<m_nClusterNum; point++ )
	{
		string.Format( "����%02d", point+1 );
		pTableMin->InsertItem( point, string , true );
	
		for( dim=0; dim<nDimension; dim++ )
			pTableMin->SetItemText( point, dim+1, pBest->dblRangeMin[point*nDimension+dim] );
	}

	CRsltElementTable *pVariableMin = new CRsltElementTable( "���෶Χ��Сֵ��", pTableMin );
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
//�������ƣ�GetHopGenNetwork
//���������int nDimension �ڵ�����
//�������ܣ����HopfieldGa�������Ϣ
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
//�������ƣ�HopGenPredict
//��������� CDataInterface *pData, BestIndividual *pBest, CResult *pResult 
//�������ܣ�����Ԥ��
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
	//�����ڴ�
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
	strProgressText = _T("���Խ�����...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

	//�����������
	nDimension = m_arFieldRole.GetSize();

	if( m_bOutputInfo )
	{
		pTable=new CTLTable;

		pTable->SetCols(nDimension+2);
		pTable->SetTitle("Hopfield�㷨Ԥ����");
		pTable->InsertColumn(0, "��¼��");
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
		pTable->InsertColumn( nDimension, "��������" );
		pTable->InsertColumn( nDimension+1, "����λ��" );
	}


	////////////////////////////////////////////////////////////
	// �������������Сֵ���ڱ�׼������
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
	//���б�׼������λ��һ��

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
			
			//�õ�����
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
			//�������
			nInputField = 0;
			for( nCol=0; nCol<m_arFieldRole.GetSize(); nCol++)
			{
				CTString strField = m_arFieldRole.GetAt(nCol);
				CField * pField = pFieldList->FieldByName(strField);
				adDataRow[nInputField] = pField->GetAsDouble();

				if( m_bOutputInfo )
					pTable->SetItemText( iValidRow, nInputField+1, adDataRow[nInputField] );
				
				//add by xds 20060628->���ж������ĸ���
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
			//���� add by xds 20060628
			nSize = pPointCount[nHits];
			pClusterDPS[nHits][nSize].fXVal = row+1;
			pClusterDPS[nHits][nSize].fYVal = nHits+1;
			pPointCount[nHits] += 1;
			//
			nInvalid++;
			if(!bHits)
			{
				if( m_bOutputInfo )
					pTable->SetItemText( iValidRow, nInputField+2, "�����" );
			}
			iValidRow++;
		}
		pData->m_DataAccess.Next();
		row++;
	}

	//�Ƿ�����б�
	if( m_bOutputInfo )
	{
		CRsltElementTable *pVariable = new CRsltElementTable( "Hopfield���������", pTable );
		pResult->Add(pVariable);
	}

	if( nTreatedData>0 )
	{
		///////////////////////////////////
		// �������Ԥ������Ϣ
		CTString strTitle, strTreated;
		strTitle.Format(   " ����Ԥ����:        �ܼ�¼��   ��������   ����ֵ����\n\n" );
		strTreated.Format( "                           %8d         %8d          %8d", 
					   pData->GetRowCount(), 
					   nTreatedData, 
					   0 );
		CRsltElementText * pTreatedTextRslt = new CRsltElementText("����Ԥ����");
		pTreatedTextRslt->AddString( strTitle + strTreated.GetData() );
		pResult->Add( pTreatedTextRslt );
		//
		///////////////////////////////////
	}
	//add by xds 20060628,ͼ��
	//����ͼ
	pClusterplot = new CTChartScatterPlot;
	for (i=0;i<m_nClusterNum; i++)
	{
		int n = pPointCount[i];
		string.Format( "����%02d", i+1 );
		pClusterplot->SetScatter(pClusterDPS[i],n,ValVal,string.GetData());
		delete []pClusterDPS[i];
	}
	pClusterplot->SetXAxilLabel("��¼��");
	pClusterplot->SetYAxilLabel("�����");
	pClusterplot->SetTitle("����ͼ");
	CRsltElementChart *pClusterChart = new CRsltElementChart( "����ͼ", pClusterplot );
	pResult->Add( pClusterChart );
	//ɢ��ͼ
	if( pScatterDPS )
	{
		pScatterplot = new CTChartScatterPlot;
		for (i=0;i<m_nClusterNum; i++)
		{
			int n = pPointCount[i];
			string.Format( "����%02d", i+1 );
			pScatterplot->SetScatter(pScatterDPS[i],n,ValVal,string.GetData());
			delete []pScatterDPS[i];
		}
		pScatterplot->SetTitle("ɢ��ͼ");
		CRsltElementChart *pScatterChart = new CRsltElementChart( "ɢ��ͼ", pScatterplot );
		pResult->Add( pScatterChart );
	}
	delete []pPointCount;
}

//////////////////////////////////////////////////////////////////////////
//�������ƣ�HopAndGenHitsTest
//���������CDoubleVector& adData, BestIndividual *pBest, int nDimension, bool &bHits
//���������ѡ�е���
//�������ܣ��������Ƿ�ѡ��
//////////////////////////////////////////////////////////////////////////
int CNeuralNet::HopAndGenHitsTest(CDoubleVector &adData, BestIndividual *pBest, int nDimension, bool &bHits)
{
	int i, j, nCluster = 0;
	CDoubleVector adError(m_nClusterNum);
	double dSquareError;
	double dMinError = 0;

	//�����С���
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

	//ѡ�еĲ���
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
//�������ƣ�SetDataSource
//���������CDataInterface *pDataInterface, ���ݽӿ�
//			 CPtrArray * arFieldRole  �����ֶε���Ϣ
//�������ܣ���������Դ�����ݽӿں��ֶε���Ϣ
//////////////////////////////////////////////////////////////////////////
bool CNeuralNet::SetDataSource(CDataInterface *pDataInterface, CTStringArray& arFieldRole)
{
	m_pDataInterface = pDataInterface;
	//m_arFieldRole.Copy(*arFieldRole);--Modified by pyx 2005-12-27
	return true;
}

//////////////////////////////////////////////////////////////////////////
//�������ƣ�SetDataParam
//���������CNeuralNetVO	*pNeuralNet ��������Ľӿ�
//�������ܣ������û�����Ĳ�������
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
//�������ƣ�SetErrInfo
//�������������������Ϣ��CTString,CResult*&
//�������ܣ���������Ϣ��������
//////////////////////////////////////////////////////////////////////////
void CNeuralNet::SetErrInfo(CTString FormmatStr, CResult *& pResult)
{
	CRsltElementText * pText = new CRsltElementText("����(�������������)\n");
	pText->AddString(FormmatStr);
	pResult = new CResult("���������");
	pResult->Add(pText);
}

//////////////////////////////////////////////////////////////////////////
//�������ƣ�SetParamFromVO
//�������������������Ϣ��string
//�������ܣ���VO��ȡ�����ò���
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
		SetErrInfo("���������δ�ܳɹ�����\n",pResult);		
		return false;
	}
	int count =	vo.GetWordCount();
	if (count != 11)
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	int index =0;
	//====================================================================================
	//���s = "/FIELDSINDEX=[0,1]"
	string s = vo.GetAt(index);
	int l = s.compare(0,13,"/FIELDSINDEX=");
	if (l<0)
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = s.substr(13);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
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
			SetErrInfo("����������б��ʽ����\n������Ϊ��\n",pResult);
			return false;
		}
		int eachIndex = atol(var);
		if (eachIndex<0 || eachIndex>=m_pDataInterface->GetColCount())
		{
			SetErrInfo("����������б��ʽ����\n�����������\n",pResult);
			return false;
		}
		pRole = new TFieldRole;
		pRole->nFieldNo = eachIndex;
		pRole->strFieldName = m_pDataInterface->GetFieldName(eachIndex);
		pRole->strRole = "�������";
		m_arFieldRole.Add(pRole);
		strTmp = strTmp.Substr(endPos+1,strTmp.GetLength()-endPos-1);
	}
	if (strTmp.GetLength()<=0)
	{
		SetErrInfo("����������б��ʽ����\n������Ϊ��\n",pResult);
		return false;
	}
	int eachIndex = atol(strTmp);
	if (eachIndex<0 || eachIndex>=m_pDataInterface->GetColCount())
	{
		SetErrInfo("����������б��ʽ����\n�����������\n",pResult);
		return false;
	}
	pRole = new TFieldRole;
	pRole->nFieldNo = eachIndex;
	pRole->strFieldName = m_pDataInterface->GetFieldName(eachIndex);
	pRole->strRole = "�������";
	m_arFieldRole.Add(pRole);
	
	index++;
	//====================================================================================
	//��� s = "/TIMES=[100]"
	s = vo.GetAt(index);
	l = s.compare(0,7,"/TIMES=");
	if (l<0)
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = s.substr(7);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	int iValue = atol(strTmp);
	if (iValue<2)
	{
		SetErrInfo("���������������\n����������ӦΪ����1������!\n",pResult);
		return false;
	}
	m_nTrained = iValue;

	index++;
	//====================================================================================
	//��� s = "/BTRAIN=[TRUE]"
	s = vo.GetAt(index);
	l = s.compare(0,8,"/BTRAIN=");
	if (l<0)
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = s.substr(8);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
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
		SetErrInfo("���������������\nѵ��/����!\n",pResult);
		return false;
	}

	index++;
	
	//====================================================================================
	//��� s = "/MAXRUNTIME=[100]"
	s = vo.GetAt(index);
	l = s.compare(0,12,"/MAXRUNTIME=");
	if (l<0)
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = s.substr(12);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	unsigned int uValue = atol(strTmp);
	if (iValue<=0)
	{
		SetErrInfo("���������������\n�����ʱ��Ӧ�ô�����!\n",pResult);
		return false;
	}
	m_dwRuntime = uValue;
	
	index++;
	
	//====================================================================================
	//��� s = "/BOUTPUTINFO=[TRUE]"
	s = vo.GetAt(index);
	l = s.compare(0,13,"/BOUTPUTINFO=");
	if (l<0)
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = s.substr(13);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
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
		SetErrInfo("���������������\n�����Ϣ!\n",pResult);
		return false;
	}

	index++;
	if (m_bTraining == TRUE || !m_bHaveNetwork)
	{
		m_strLoadFileName.Empty();
	}
	else
	{
		//��ȡ s = "/RULEFILENAME=[]"
		s = vo.GetAt(index);
		l = s.compare(0,14,"/RULEFILENAME=");
		if (l<0)
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = s.substr(14);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		m_strLoadFileName = strTmp;
	}

	index++;
	
	//====================================================================================
	//��� s = "/NETTYPE=[Kohonen]"
	s = vo.GetAt(index);
	l = s.compare(0,9,"/NETTYPE=");
	if (l<0)
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = s.substr(9);
	if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
	{
		SetErrInfo("����������б��ʽ����\n",pResult);
		return false;
	}
	strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
	if (strTmp == "ART")
	{
		m_nTopologyType = 1;
		//==========================================================
		//���뾯����
		index++;
		s = vo.GetAt(index);
	}
	else if (strTmp == "Kohonen")
	{
		m_nTopologyType = 2;
		//==========================================================
		//�������������
		index = index+2;
		//��� s = "/MAXCLUSTERNUM=[5]"
		s = vo.GetAt(index);
		l = s.compare(0,15,"/MAXCLUSTERNUM=");
		if (l<0)
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = s.substr(15);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		iValue = atol(strTmp);
		if (iValue<2)
		{
			SetErrInfo("���������������\n���������ӦΪ����1������!\n",pResult);
			return false;
		}
		m_nClusterNum = iValue;

		index++;
		//=============================================================
		//������ѧϰ���� s = "/MAXLEARNRATE=[0.4]"
		s = vo.GetAt(index);
		l = s.compare(0,14,"/MAXLEARNRATE=");
		if (l<0)
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = s.substr(14);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		double dValue = atof(strTmp);
		if (dValue<=0 || dValue>=1)
		{
			SetErrInfo("���������������\n���ѧϰ����ӦΪ0��1֮�����!\n",pResult);
			return false;
		}
		m_fMaxRate = dValue;

		index++;
		//==================================================================
		//�����Сѧϰ���� s = "/MINLEARNRATE=[0.01]"
		s = vo.GetAt(index);
		l = s.compare(0,14,"/MINLEARNRATE=");
		if (l<0)
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = s.substr(14);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		dValue = atof(strTmp);
		if (dValue<=0 || dValue>=1)
		{
			SetErrInfo("���������������\n��Сѧϰ����ӦΪ0��1֮�����!\n",pResult);
			return false;
		}
		m_fMinRate = dValue;
	}
	else if (strTmp == "Hopfield")
	{
		m_nTopologyType = 5;
		//=====================================================================
		//�������������
		index = index+2;
		//��� s = "/MAXCLUSTERNUM=[5]"
		s = vo.GetAt(index);
		l = s.compare(0,15,"/MAXCLUSTERNUM=");
		if (l<0)
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = s.substr(15);
		if (strTmp.Substr(0,1)!="[" || strTmp.Substr(strTmp.GetLength()-1,1)!="]")
		{
			SetErrInfo("����������б��ʽ����\n",pResult);
			return false;
		}
		strTmp = strTmp.Substr(1,strTmp.GetLength()-2);
		int value = atol(strTmp);
		if (value<2)
		{
			SetErrInfo("���������������\n���������ӦΪ����1������!\n",pResult);
			return false;
		}
		m_nClusterNum = value;
	}
	else
	{
		SetErrInfo("��������������Ͳ�����\n",pResult);
		return false;
	}
	
	return true;
}
*/

//////////////////////////////////////////////////////////////////////////
//�������ƣ�HaveTarget
//�������ܣ������Ƿ���Ŀ�����
//////////////////////////////////////////////////////////////////////////
bool CNeuralNet::HaveTarget()
{
//	for( int i=0; i<m_arFieldRole.GetSize(); i++ )
//	{
//		TFieldRole * pRole = (TFieldRole*)(m_arFieldRole.GetAt(i));
//		if( pRole->strRole==CTString("Ŀ�����") )
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
//�������ƣ�NormalizeBestIndvd
//���������BestIndividual * pBest, �Ŵ��㷨����Ĳ���
//			int nDimension		�ڵ����Ŀ
//�������ܣ���һ������
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

