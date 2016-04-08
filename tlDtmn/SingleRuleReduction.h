// SingleRuleReduction.h: interface for the CSingleRuleReduction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLERULEREDUCTION_H__3C007342_E3BD_41C8_9DC7_B9F1EED4AE84__INCLUDED_)
#define AFX_SINGLERULEREDUCTION_H__3C007342_E3BD_41C8_9DC7_B9F1EED4AE84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Association.h"


// #include "PageParamSet.h"
// #include "PageFResult.h"

//#define WM_CALCULATE_OVER WM_USER+1000

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

struct TUnit
{
	CPtrArray arIndex;
	int count;
	TUnit()
	{
		count = 0;
	}
	~TUnit()
	{
		for(int i=0;i<arIndex.GetSize();i++)
		{
			void * pt = arIndex.GetAt(i);
			//delete pt;
			//pt = NULL;
		}
		arIndex.RemoveAll();
	}
};

// class CDlgProgress;

//Ƶ�����ļ���
struct TFreSet {
	int			nSetCount;//Ƶ������
	CPtrArray	arSet;//Ƶ��������
	int			nFreq;//Ƶ����
};

//���ÿһ���ֶε���ֵ
struct TFeildValue {
	int		nIndex; //�ֶε�����
	union Variant{
		bool       bVal;
		int        nVal;
		double     dblVal;
		char  *    strVal;
	};
	Variant	val;//�ֶε���ֵ
};

//ǿ���������
struct  TRuleDmn{
	CPtrArray	arSource;//Դ����TFeildValue
	CPtrArray	arDestion;//�Ƶ���Ŀ������TFeildValue
	double		dbSupport;//֧�ֶ�
	double		dbBelief;//���Ŷ�
};



/*
//Ԥ��ģ�͵Ĳ���
typedef struct tagStructRuleModel
{
	CRuleTrainVO		*pRuleVO;//����������
	CPtrArray	arFreSet;//Ƶ����������TFreSet
	CPtrArray	arRuleDmn;//���������ǿ����TRuleDmn
}StructRuleModel;
*/


class AFX_EXT_CLASS CSingleRuleReduction//  : public CObject
{
public:
	void Abort();
	void Serialize(TCLFile &file);
	CResult * OnSingleRuleReduction(CDataInterface *pDataInterface);
//	void Assess(CDataInterface * pDataInterface,int ,int);
//	void Assess(CDataInterface *pDataInterface,CDWordArray& nObjAry,CDWordArray& nIpuAry);
	bool	CreatePredModel(CRuleOutput *&pOutStruc);
	void	SetErrInfo(CTString FormmatStr, CResult *& pResult);
	
	bool	SetTrainVO(string strVO);
	string	ExportModel();
	void	SetTrainDataParam(CRuleTrainVO *pVO);
	void	SetPredDataParam(TPredVO *pVO);
	void	SetPredModel(string strModel);
	void	SetDataSource(CDataInterface *pDataInterface, CPtrArray *parField);
// 	void Assess();
	CResult *Run(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO);

	CResult *RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO,string &strModel);
	CResult* RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO, string strModel);
	void Assess();
	int m_nDependIndex;

	CPtrArray m_tVarInfo;
	double m_dSupport;
	bool m_bSingle;
	bool m_bStopWork;
	bool m_bSort;
	double m_dConfidence;
	int m_nCustomCol;
	int m_nProductCol;
	CTStringArray m_strInputField;
	CTStringArray m_strDestField;

	CSingleRuleReduction();
	CSingleRuleReduction(CTString& strProjectPath, bool bNewComponent);
	virtual ~CSingleRuleReduction();

protected:
	void WriteUnitData(TUnit * pUnit,char * pszTitel);
	void WritePtrData(CPtrArray * par,char * pszTitle,CResult * pResult = NULL);
	void WritePtrData(CPtrArray * par,char * pszTitle,bool *pStop,CResult * pResult = NULL);
	
protected:
	CResult * m_pResult;	
	int m_nCustomNum;

	void FreePtrArray(CPtrArray * par);

	CIntVector VectorSub(CIntVector vect1,CIntVector vect2);
	int GetSubSetCount(CPtrArray * par,CIntVector vect);
	void Confidence(CPtrArray * par,double mini_confidence,CResult * pResult = NULL,bool * pbStop = NULL);
	CIntMatrix GetData(CDataInterface * pDataInterface);
	inline bool IsInSet(CIntVector vect,CPtrArray * parItem,int mk);
	inline bool FindNonFrequentSubSet(TUnit * pcandidateItem,CPtrArray * pSetLk,int nk,int mk);
	void InXthenCount(CIntMatrix matrx,CIntVector vector,CPtrArray * parCk);
	CIntMatrix GetNumOfCk(CPtrArray * par);
	bool Aproiri_gen(CPtrArray * par , int k,int mark,CPtrArray * parCk/*,void * lp = NULL*/,bool * pStop = NULL);
	CDataInterface * m_pDataInterface;
	bool SingleRulereduction(CIntMatrix matrix,int miniSupport);

private:
	CRuleTrainVO*	m_pRuleVO;
};

#endif // !defined(AFX_SINGLERULEREDUCTION_H__3C007342_E3BD_41C8_9DC7_B9F1EED4AE84__INCLUDED_)
