#if !defined(AFX_SVMTHREAD_H__AC0DA8D4_3F01_452C_BF81_F8DD25B3C741__INCLUDED_)
#define AFX_SVMTHREAD_H__AC0DA8D4_3F01_452C_BF81_F8DD25B3C741__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include "PageSVM.h"
// #include "Resource.h"
//#include "CSVM.h" 
// SVMThread.h : header file
//
//#include "iostream.h"
//#include "math.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../TLDBDll/Database/DataAccess.h"
#include "../DataInterface/DataInterface.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"
#include "../Public/FileReadWrite.h"
#define DLLEXPORT	__declspec(dllexport)

/////////////////////////////////////////////////////////////////////////////
// CSVMThread thread
//add by liytao 20030123


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




//ѵ��ʱ�ӽ����������
//�����������Ҫ�Ǵӽ����������
class CSVMTrainVO
{
public:
	CTStringArray strInputField;//�������ֶεĶ��壬˵�����������Ŀ�����
	CTStringArray strDecField;
	int		nCassifyMethod;//��������ࣺ���Կɷ֣����Բ��ɷ�
	int		nKernalType;//�˺������ͣ���˹����������ʽ����
	int		nSiggmaSquare;	//��˹������siggmaƽ��
	int		nLaddCount;//����ʽ�����Ľ���
	double  dPunishFactor;//�ͷ�����
	double  dKKTFactor;//KKT����
	double	dEPS;//Epsֵ
	CSVMTrainVO()
	{
		nCassifyMethod = 1;
		nKernalType = 0;
		nSiggmaSquare = 2;
		nLaddCount = 2;
		dPunishFactor = 2.0;
		dKKTFactor = 0.1;
		dEPS = 0.001;
	}
	bool Compile(string strVO)
	{
	//	֧��������VO����
	//	SVMTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/KERNALTYPE [{LINEAR**},{LINEARNOT}]
	//	/CLASSIFY [{GAUSS**},{POLYNOMIAL}]
	//	/CLASSIFYPAR SIGGMA(2**) LADDER(2**)
	//	/PARAM FACTOR(2**) KKT(0.1**) KPS(0.001**)

		AnaWord VOstr;
		VOstr.Import(strVO);

		nCassifyMethod = AnaSyntax::findstr("/CLASSIFY","POLYNOMIAL",VOstr) ? 1:0;
		nKernalType = AnaSyntax::findstr("/KERNALTYPE","LINEARNOT",VOstr) ? 1:0;

		if (AnaSyntax::findstr("/CLASSIFYPAR","SIGGMA",VOstr))
			nSiggmaSquare = AnaSyntax::getvalue("/CLASSIFYPAR","SIGGMA",VOstr);
		if (AnaSyntax::findstr("/CLASSIFYPAR","LADDER",VOstr))
			nLaddCount = AnaSyntax::getvalue("/CLASSIFYPAR","LADDER",VOstr);

		if (AnaSyntax::findstr("/PARAM","FACTOR",VOstr))
			dPunishFactor = AnaSyntax::getvalue("/PARAM","FACTOR",VOstr);
		if (AnaSyntax::findstr("/PARAM","KKT",VOstr))
			dKKTFactor = AnaSyntax::getvalue("/PARAM","KKT",VOstr);
		if (AnaSyntax::findstr("/PARAM","KPS",VOstr))
			dEPS = AnaSyntax::getvalue("/PARAM","KPS",VOstr);

		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputField);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDecField);
		return true;
	}
};

/*
//֧����������Ԥ��ģ�Ͳ���
typedef struct tagStructSVMModel{
	TSVMVO*		pSVMVO;//֧�����������������ݺͲ���
	CTString	strKernalFunc;//
	CDoubleMatrix	mtAlpha;//��Ҫ��һ���������ھ��ߴ���
}StructSVMModel;
*/
//##ModelId=3DD99A250127
class VarIndexs {
public:
	VarIndexs(){
		pVarIndexs=NULL;
		nTargetIndex=-1;
		nVarCount=0;
	}
	void Empty(){
		if( pVarIndexs!=NULL)
			delete []pVarIndexs;
		nVarCount=0;
	}
	
	int Find(int nIndex){
		if (nIndex<0)
			return -1;
		for( int i=0;i<nVarCount;i++)
			if (pVarIndexs[i]==nIndex)
				return i;
			return -1;
			
	}
	void operator =(VarIndexs &v)
	{
		nVarCount = v.nVarCount;
		pVarIndexs = v.pVarIndexs;
		nTargetIndex = v.nTargetIndex;
	}
	
	int *pVarIndexs;
	int nTargetIndex;
	int nVarCount;
	~VarIndexs(){
		//if( pVarIndexs)
		//	delete []pVarIndexs;
	}
};
//add end

class CSVM;
class DLLEXPORT CSVMThread //: public CWinThread
{
protected:
public:
	CSVMThread();           // protected constructor used by dynamic creation
	
	CSVM	*m_pSVM;
	CDataInterface *m_pDataInterface;
	CTStringArray	m_strInputField;
	CTStringArray	m_strDestField;
	CTString	m_ModelFile;//����ģ�͵�·��
	int m_nRoleRs;

public:
	int CreateAccessStruct(CDataAccess *pSrcAccess,CDataAccess** pTargetAccess,VarIndexs* pVarIndexs);
	int GetVarIndexs(int **pVarFields,int *pTargetIndex);
	CDataAccess* PretreatData(CDataAccess* pDataAccess,int **pIndexs);
	bool CheckPredict(CDataAccess *pAccess);
	bool HaveFieldInvalid(CDataAccess *pAccess);
	bool HaveNullRec(CDataAccess *pAccess);
	void	SetDataSource(CDataInterface *pDataInterface,CPtrArray * fields);
	void	SetDataParam(CSVMTrainVO *pVO);
	CDataAccess* DelInvalidRec(CDataAccess *pDataAccess);
	void SetTrainVO(string strVO);
	bool SetPredVO(string strVO);
	int ExitInstance();
	CResult* RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,string VO);
	CResult* RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,string VO, string strModel);
	int Run();
	virtual ~CSVMThread();
	//ģ��
	virtual bool Export(string szFileName);
	virtual bool Import(string szFileName);
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVMTHREAD_H__AC0DA8D4_3F01_452C_BF81_F8DD25B3C741__INCLUDED_)
