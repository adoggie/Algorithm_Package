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
//*�㷨����綯̬�������Ƿ���Ҫ���������Ϣ���Ƿ���ֹ
//*/
//struct Interaction
//{
//	bool bNeedFeedback;//���룺�����㷨�Ƿ���Ҫд������Ϣ��true�����뷴����false������
//	bool bAbort;//���룺true��ֹ��ǰ����false���������С�Ĭ��false
//	char BufMsg[100];//����������ǰ������Ϣ	
//};
//
///*
//����Ŀ�����Ϣ�Ļ��࣬����Щ������Ϊ���������Щ��Ϊ�Ա��������������Щ��Ҫ���
//��Ҫ���ƽ������ʾ���⣬ÿһ���㷨����һ����
//*/
//class VOBase{
//};
//
///*
//�㷨ģ����Ϣ����Ҫ�����㷨ѵ��ʱ�������ģ����Ϣ��������һ�ε�Ԥ�⡣
//ÿһ���㷨����һ����
//*/
//class StructBase{
//public :
//	string Export();//����ΪXML��ʽ
//	bool Import(string str);
//};
//
///*
//���������࣬����ϵͳ���������·���ΪC�㷨�ڲ����õķ���
//*/
//class CEnvParam{
//public:
//	//******  ָ����Щ������Ҫ���뵽������  *** 
//	bool HasOutParm(CTString sName);//�ж��Ƿ���Ҫ���ĳ����������yresid
//	string GetOutParamName(CTString sName);//
//	//*************���û�������************
//	bool SetVar(CTString sVarName,double value);
//	bool SetVar(CTString sVarName,CDoubleVector* value);//value �ڴ�ռ����㷨�Լ��ͷš�
//	bool SetVar(CTString sVarName,CDoubleMatrix* value);//value �ڴ�ռ����㷨�Լ��ͷš�
//	bool SetVar(CTString sVarName,CDataAccess* value);//value �ڴ�ռ����㷨�Լ��ͷš�
//};
//




//������������������ѵ����
class  CBPNetTrainVO
{
public:
	int			nPartMethod;//���ַ���
	int			nMaxTrainCount;//���ѵ������
	int			nPercent;//ѵ��������ռ�İٷ���
	int			nOuputType;//�������
	int			nTrainCaseCount;//ѵ��������
	int			nRideCellCount;//������Ԫ��
	double		dRunTime;//����ʱ��
	double		dSystemPrecision;//ϵͳ����
	double		dLearnedRate;//ѧϰ��
	CTStringArray	strInputNames;//TFieldInfo�����������ֶε���Ϣ
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
	//BP������VO����
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

//������������,Ԥ��ģ���������
//typedef struct  tagTBPNetPredVO
//{
//	bool		bLoadSource;//�Ƿ��뵽Դ�ļ���ȥ
//	CTString	strNewFileName;//���������Դ�ļ��У�����Ҫ�½�һ���ļ����ڴ��Ԥ���ֵ������Ԥ���һЩ����
//	CPtrArray	arFieldInfo;//��Ҫ���뵽���ļ���ȥ�ֶε���Ϣ
//	tagTBPNetPredVO()
//	{
//		bLoadSource = true;
//		strNewFileName = "";
//	}
//} TBPNetPredVO;  


/*
typedef struct tagStructBPNetModel
{
	TBPNetVO		*pBPVO;//�ӽ�������������ֶ�
	CPtrArray	arWeight;//Ȩֵ�ľ�����Ҫ���ж��ٲ���ж��ٸ�Ȩֵ����
}StructBPNetModel;
*/


struct MixSel
{
	int MixType;//0 ���ٷֱ� 1 ǰ��������¼
	int nValue;//��Ӧ����ֵ
};

struct TrainItem//�޸�ʱ��ע���ⲿvoid CPropBasic::OnOK() 
{
	long	nTrainTimes;
	double	dPrecision;
	double	dStep;
	double	dMaxRunTime;
	int nHideNum;//��������Ԫ��Ŀ
	int m_bCalc;//�Ƿ�ʱ����ģʽ���Ƿ���ģʽ
	double*pSortAry;//m_bCalc ����ʱ������
	int nClassCount;//m_bCalc ����ʱ���������
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
	//���صȺ�
	TrainItem& operator =(TrainItem&s)
	{
		Clear();
		nTrainTimes= s.nTrainTimes;
		dPrecision= s.dPrecision;
		dStep= s.dStep;
		dMaxRunTime = s.dMaxRunTime;
		nHideNum = s.nHideNum;
		m_bCalc= s.m_bCalc;//�Ƿ�ʱ����ģʽ���Ƿ���ģʽ
		nClassCount= s.nClassCount;
		if (s.nClassCount)
		{
			pSortAry = new double[s.nClassCount];
			memcpy(pSortAry,s.pSortAry,sizeof(double)*s.nClassCount);
		}
		return *this;
	}
	void Clear()
	{//�����ӵ�е��ڴ�
		if (pSortAry )
		{
			delete []pSortAry;
			pSortAry = NULL;
		}
	}
	~TrainItem (){Clear();
	}
};

//�ֶ�����,ʵ�ֹ�һ����
class FieldDesc
{
	CTStringArray strArray;//���л������Ӧ�ĺ���Ϊ��ǰλ�ü�1 (����һ��ʼ)
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

//����ӿڣ�����ѭ������
//����ģ��Ԥ��
//class CBPNetOutput : public  StructBase
//{
//public:
//	CBPNetOutput(){;}
//	virtual ~CBPNetOutput(){;}
//	
//	virtual  string	Export();
//	virtual	 bool		Import(string str);
//public://ѵ���������������
//	CTStringArray	strInputNames;//TFieldInfo�����������ֶε���Ϣ
//	CTStringArray	strDecNames;
//	int		nMaxTrainCount;//���ѵ������
//	int		nOuputType;//�������
//	int		nPartMethod;//���ַ���
//	int		nPercent;//ѵ��������ռ�İٷ���
//	int		nTrainCaseCount;//ѵ��������
//	int		nRideCellCount;//������Ԫ��
//	double	dRunTime;//����ʱ��
//	double	dSystemPrecision;//ϵͳ����
//	double	dLearnedRate;//ѧϰ��
//	TrainItem* pTrainItem;
//	FieldDescList* pInAry;
//	FieldDescList* pOutAry;
//	
//	//ѵ�����Ȩֵ
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
protected://����Դ
	CDataInterface *m_pDataInterface; //Ҫ����������Դ
	CBPMatrix	m_mxSour;//����Դ��ѵ����ģ��ʱ��
	CMemoStream			m_NetWork;//�洢����ṹ
	FieldDescList		m_InAry;//�洢���������
	FieldDescList		m_OutAry;//�洢Ŀ�������

protected:
	//ѵ�������н����������
	TrainItem			m_TrainItem;//ѡ��
	
protected://Ԥ��������
	//////////////////////////////////////////////////////////////////////////
	//Ԥ���õĲ�������Ҫ�Ƕ�Ԥ������ݽ���д���ļ��еĹ���,��Ҫͨ����������
	bool			m_bLoadSource;//�Ƿ��뵽Դ�ļ���ȥ
	CTString		m_strNewFileName;//���������Դ�ļ��У�����Ҫ�½�һ���ļ����ڴ��Ԥ���ֵ������Ԥ���һЩ����
	CTStringArray	m_arInputField;
	CTStringArray	m_arDestField;
	
protected:
	//����
	CBP					m_Bp;//���ڴ���ѵ���Ĺ���
	
protected:
	//���̿��ƺ�״̬����������
	CEnvParam*			m_pEnvParam;
	Interaction*		m_pInteraction;
	
public:
	std::string			m_strModel;//�����ģ��
	CTString			m_ModelFile;//ģ�ͱ����·��
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
	//ģ��
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





















