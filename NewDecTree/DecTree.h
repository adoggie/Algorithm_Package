/********************************************************************************/
/******************      ������������                          ******************/
/******************      �����ߣ�  ������                 ***********************/
/******************      �������ڣ�Oct.25 2002             ***********************/
/******************      �޸��ߣ�                         ***********************/
/********************************************************************************/ 
#if !defined(AFX_AAAA_H__D87493B3_7649_4CF0_A328_119727C19A54__INCLUDED_)
#define AFX_AAAA_H__D87493B3_7649_4CF0_A328_119727C19A54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecTree.h : header file
#include "../Public/TCLFile.h"
#include "../Public/TString.h"
#include "../DataInterface/DataInterface.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../arithInterface/EnvParam.h"

//add by liytao

	// GenerateType:
#define	GENTREE_GINI			1 //������GINIϵ��
#define GENTREE_ENTROPY			2 //��������Ϣ����

	//PruneType:
#define PREPRUNE_GINI			1	//�ȼ�֦GINIϵ��
#define PREPRUNE_ENTROPY		2	//�ȼ�֦��Ϣ����
#define PREPRUNE_DEPTH			3	//�ȼ�֦���
#define POSTPRUNE_ERRRATE		4	//���֦������
#define	PREPRUNE_NOPRUNE		5	//�޼�֦

#define ROOT                        0
#define NODE                        1
#define LEAF                        2

typedef union _DBVARIANT{
	bool       bVal;
	int        nVal;
	double     dblVal;
	char  *    strVal;
}DBVariant;
typedef struct tagLabelData {
	DBVariant		varValue;
	CTString		strLabelName;
}LabelData;

//typedef TCDataArray<DBVariant> VARVALUE;
typedef std::vector<DBVariant> VARVALUE;
typedef std::vector<int>       ClassCounts;	
typedef std::vector<double>    VALHISTBORDER;
typedef std::vector<int*>      VALCLASSFREQ;
typedef std::vector<int>      CHILDINDEX; //add by liytao 20030121
typedef std::vector<LabelData*> VARLABEL;
typedef struct _FIELDCLASSIFY{
	CTString                strFieldName;
	CFieldType              cFieldType;
	int                     NumBranch;
	VARVALUE                VarValue;
	CHILDINDEX				ChildIndex; //add by liytao 20030121
	VARLABEL				FieldLabel;		
	_FIELDCLASSIFY()
	{
		NumBranch = 0;
	}
}FieldClass;



typedef struct _FIELDVALUEFREQUENCY{
	ClassCounts         ValFreq;
	VALHISTBORDER        ValHistBorder;
	VALCLASSFREQ         ValClassFreq;
}FIELDVALUEFREQUENCY;



typedef TCDataArray<FieldClass*> FieldInfo;
typedef struct tagVARVAL
{
public:
	CTString	strName;
	double		dblValue;
}VARVAL;

typedef struct tagFieldSourceData
{
	CTString		strFieldName;
	CFieldType		nType; //CIntVector, CDoubleVector, CStringArray.��������
	CIntVector		vectIntData;
	CDoubleVector	vectDoubleData;
	CTStringArray	arStringData;
	tagFieldSourceData()
	{
		nType = -1;
	}
}FieldSourceData;

class CDecTreeVO
{
public:
	int  nGenTreeMethod;
	int	 nPrunMethod;
	int  nMinSample;
	int	 nMaxBranch;
	int	 nDepth;
	double  dMinInfoGain;
	double  dMinGINIReduction;
	double  dErrorRate;
	bool    bShowLabel;
	CTStringArray szArrHitRange;
	CTStringArray strInputNames;
	CTStringArray strDecNames;
	CTString  strModelName;
	CDecTreeVO()
	{
		nGenTreeMethod = 0;
		nPrunMethod = 3;
		nMinSample = 2;
		nMaxBranch = 3;
		nDepth = 5;
		dMinInfoGain = 0.06;
		dMinGINIReduction = 0.15;
		dErrorRate = 0.3;
		bShowLabel = false;
	}
	bool Compile(string strVO)
	{
	//	������VO����
	//	DECTREETRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/GENMETHOD [{INFOGAIN**},{GINIED}]
	//	/PRUNMETHOD [{NONE},{BEFPRUN**},{AFTPRUN}]
	//	/BEFPRUN [{MAXINFOGAIN(0.06**)},{MAXGINIRED(0.15**)},{MAXDEPTH(5**)}]
	//	/ATFPRUN = ERRRATE(0.3**)
	//	/LEAFCON = MINSAMPLE(2**) MAXBRANCH(3**)
	//	/SAVEMODEL [FILE]

		AnaWord VOstr;
		VOstr.Import(strVO);
		
		nGenTreeMethod = (AnaSyntax::findstr("/GENMETHOD","INFOGAIN",VOstr)) ? GENTREE_ENTROPY : GENTREE_GINI;

		if (AnaSyntax::findstr("/PRUNMETHOD","NONE",VOstr))
			nPrunMethod = PREPRUNE_NOPRUNE;//�޼�֦
		else if (AnaSyntax::findstr("/PRUNMETHOD","AFTPRUN",VOstr))
		{
			nPrunMethod = POSTPRUNE_ERRRATE;//���֦������
			if (AnaSyntax::findstr("/AFTPRUN","ERRRATE",VOstr))
				dErrorRate = AnaSyntax::getvalue("/AFTPRUN","ERRRATE",VOstr);
		}
		else if (AnaSyntax::findstr("/PRUNMETHOD","BEFPRUN",VOstr))
		{
			if (nGenTreeMethod == GENTREE_ENTROPY) //��Ϣ����
			{
				if (AnaSyntax::findstr("/BEFPRUN","MAXINFOGAIN",VOstr))
				{
					nPrunMethod = PREPRUNE_ENTROPY;//�ȼ�֦��Ϣ����
					dMinInfoGain = AnaSyntax::getvalue("/BEFPRUN","MAXINFOGAIN",VOstr);
				}
				else if (AnaSyntax::findstr("/BEFPRUN","MAXDEPTH",VOstr))
				{
					nPrunMethod = PREPRUNE_DEPTH;//�ȼ�֦���
					nDepth = AnaSyntax::getvalue("/BEFPRUN","MAXDEPTH",VOstr);
				}
				else
				{
					return false;
				}
			}
			else                                   //GINIϵ��
			{
				if (AnaSyntax::findstr("/BEFPRUN","MAXGINIRED",VOstr))
				{
					nPrunMethod = PREPRUNE_GINI;//�ȼ�֦GINI����
					dMinGINIReduction = AnaSyntax::getvalue("/BEFPRUN","MAXGINIRED",VOstr);
				}
				else if (AnaSyntax::findstr("/BEFPRUN","MAXDEPTH",VOstr))
				{
					nPrunMethod = PREPRUNE_DEPTH;//�ȼ�֦���
					nDepth = AnaSyntax::getvalue("/BEFPRUN","MAXDEPTH",VOstr);
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
		
		if (AnaSyntax::findstr("/LEAFCON","MINSAMPLE",VOstr))
			nMinSample = AnaSyntax::getvalue("/LEAFCON","MINSAMPLE",VOstr);
		if (AnaSyntax::findstr("/LEAFCON","MAXBRANCH",VOstr))
			nMaxBranch = AnaSyntax::getvalue("/LEAFCON","MAXBRANCH",VOstr);

		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputNames);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDecNames);
		
		//�������-add by xds 20060713
		CTStringArray arrOut;
		AnaSyntax::getVAR("/OUTPUT",VOstr,arrOut);
		int i=0, nSize = arrOut.GetSize();
		for (i=0; i<nSize; i++)
		{
			CTString szTemp = arrOut.GetAt(i);
			if (szTemp == "LABEL")
			{
				bShowLabel = true;//������Label,����ʾLabel.
			}
		}
		//ģ������ʱ��Χ
		szArrHitRange.RemoveAll();
		AnaSyntax::getVAR("/HITRANGE",VOstr,szArrHitRange);		
		////////////////////////////////////////////////////////
		strModelName = AnaSyntax::getSingleStr("/SAVEMODEL",VOstr);
		return true;
	}
};


typedef struct tagTreeNode
{
	int				nTreeID;     //root is 0
	int				nParentID;   //root is -1
    bool			bDiscrete;   //is discret field
	int				nLevel;      //�ڵ�ļ���
	int				nNumChild;   //�ӽڵ����
	int				nNumLeaf;    //Ҷ�ĸ���
	int				nValueType;
	CTString		strLabel;
	FieldClass		*pSplitField;  //the field based on which to split   
    ClassCounts		*pClassCount;  // counts of certain class in subset
	std::vector<VARVAL *>	*pImportic;
}TreeNode;

class  Tree  
{
public:
	static int GetChildNum(Tree *child);
	static void Prune(Tree *tree);
	static bool InsertChild(Tree *parent,Tree *child,int nChildNo);
	static int GetChildrenCount(Tree *parent);
	static Tree * GetChild(Tree *parent,int nChildNo);
	static void DeleteAllChildren(Tree  *parent);
	static void AddChild(Tree *parent, Tree *child);
	static void Delete(Tree *tree);
	static	bool AreAllChildrenLeaves(Tree *tree);	
	Tree * Parent;
	
	Tree();
	virtual ~Tree();
	Tree *FirstChild;
	Tree *NextSibling;
	Tree *PrevSibling;
	int nVal;
};

class CDecTree;
class CTreeNode: public Tree   
{
public:
	CTreeNode();
	CTreeNode(CDecTree *pDecTree);
	CTreeNode(CDataAccess *pDataAccess);
	~CTreeNode();
public:
	void    SetNodeLabel(CTString label);
	CTString GetNodeLabel(bool bLabel = false);
	void    FieldCheckData(CIntVector &Check_Vect);
// 	void    CheckData();
	
public:
	void SetTreeParam(int MaxBranches,int MaxDepth,int MinCaseLeaf,float fErrorRatio,float fMinInfoGain,float fMinGINIReduction);
	bool CalcEntropyGain(int nPruneType);
	static bool CopyLists(CTreeNode *pSrcNode, CTreeNode *pDesNode=NULL);
	bool CopyLists(CTreeNode *parent);
	void SetTargetField(FieldClass* pFieldClass);
	//void SetPrgWnd(CWnd *pPrgWnd,UINT prgMsg); -by pyx 2006-02-15
public:
	int		GetCount();
	void ClearTempAccess();
	float GetErrRate();
	// add by liytao
	bool Evaluate(CPtrArray &arFieldSource,int nPos);
	int GetMaxClass(double &dbAvail);	
	bool	SearchTreeLeaf(int  nGenType);
	//add end
	bool  CalFilter();
	int  GenerateTree(int nGenType,int nPruneType);
	void  PostPrune( int nPruneType);
	void  DeleteField(std::vector<FieldClass*> * pFieldList, FieldClass FClass);
	void  DeleteMissingRecord();
	void  ClearNode();
		
public:
	void SetStopCheck(bool *bCheck);
	bool* m_pbCheck;
	int m_nPrgPos;
	CTString GetArcLabel(int nChildNum,bool bLabel=false);
	int GetCaseCount();
	bool CalcGINIReduction(int nPruneType);
	bool HaveAllChildrenSameClass();
	bool MergeSameChildren();
	bool EmptyLists();
	int      m_nLevel;                                //�ڵ�ļ���
	int      m_NumChild;                              //�ӽڵ����
	int      m_NumLeaf;                               //Ҷ�ĸ���
	int      m_nNodeType;
	CTString  m_strNodeLabel;                          //�ڵ��Label��Ϣ 
	CTString  m_strFilter;
	CTString  m_strRealName;
	//������Ҫʹ�õı���
	//added by mzh 2005-05-24 ��¼��֧�����Լ��ٷֱ�
	int		 m_TotalNum;						     // �����ļ�¼����	
	int*     m_pBranchNum;							// ��������֧��¼����
	int		 m_nIndex;			                     //ѡȡ���ֶ�
	// end mzh
	std::vector<FieldClass*> m_DiscreteFieldList;
	std::vector<FieldClass*> m_ContinueFieldList;
	FieldClass               *m_pTargetField;
	std::vector<VARVAL *>		m_Importic;
	CDecTree*					m_pDecTree;
	CDataAccess             *m_pDataAccess;
	FieldClass               m_SplitField; //the field based on which to split   
    bool                     m_bDiscrete;   //is discret field
    ClassCounts              m_ClassCounts;// counts of certain class in subset
	CIntVector				 m_vtCheckData;//��������Ƿ�����Ҫ�� add by zkb
	static int nPrgPos;
	int nID;
	
private:
	//add by liytao 20021231
	//UINT m_prgMsg;
	//CWnd* m_pPrgWnd; -by pyx 2006-02-15
	//add end
	int   m_nMaxBranches;       //���ֲ���             
	int   m_nMaxDepth;          //������
	int   m_nMinCaseLeaf;       //Ҷ�ӽڵ����С������
	float m_fMaxErrorRate;      //���֦������
	float m_fMinInfoGain;       //��С��Ϣ����
	float m_fMinGINIReduction;  //GINI��
};

typedef class tagPREDECTREE
{
public:
	tagPREDECTREE()
	{
		NumTotalCase = 0;
		dblProb = 0.0;
		pValue = NULL;
	}
	~tagPREDECTREE()
	{
		NumTotalCase = 0;
		dblProb = 0.0;
		pValue = NULL;
		strTreeNode.ReleaseBuffer();
	}
	int			NumTotalCase;
	double      dblProb;
	DBVariant* 	pValue;
	CTString     strTreeNode;
}PREDECTREE;

typedef class tagTREERULE
{
public:
	CTString		strRule;
	CTString		strFilter;
}TREERULE;

class CDecTree
{
public:
	CDecTree();
	~CDecTree();
	CTString m_SaveFileName;//�ļ����������

/***ѡ������Դ������**********************************************/
public:
	bool ConnectDatabase(LPCTSTR lpszFileName);
	void SetSourceTableName(CTString TableName);

	void SetTargetField(CTString Target);
	void SetTarField(CTString Target);   // added by mzh
	void AddField(CTString FieldName,bool bDiscrete);
	void AddFieldInfo(CTString FieldName, bool bDiscrete);   // added by mzh
	void GetFieldInfo();
	void SetTreeParam(int MaxBranches,int MaxDepth,int MinCaseLeaf,
							 float fErrorRate,float fMinInfoGain,float fMinGINIReduction);
	bool CheckFieldName(CTString FieldName,bool bDiscrete);
	bool CheckTargetName(CTString FieldName);
public:
	bool	SaveTree(CTString  strFileName);
	bool	LoadTree(CTString  strFileName);
private:
	//ģ��
	bool	SaveNode(CTreeNode *subNode, TCLFile &theFile);
	bool	LoadNode(CTreeNode *pTreeNode, TCLFile &theFile);
/***������ϴ����ɢ������ʱ��************************************/
public:
	bool PreDecTree(CResult *pResult);
	void SetTempTableName(CTString		strTmpTableName);
	bool CreateTargetAccess();
	bool InitData();
	void CompareAndSave(FieldClass *field,DBVariant Value,CFieldType cType,bool bDist);
	void ClearAccess();
	void  ClearfieldList();
/***�������Ĳ������޼������۵�************************************/
public:
	CTreeNode *GenRootNode(int nGenType,int nPruneType);
	float   EvaluateTree();
	void    SetSelect(int nSelect){m_nSelect = nSelect;}
	DBVariant* 	SearchTreeLeaf(CTreeNode * pTreeNode, CFieldList *pFieldList, CTreeNode * &pNode);
	void	PrtField(CTreeNode * pTreeNode);
	void	DeleteAllNode(CTreeNode * pTreeNode);
	CTString	GetTreeRule(CTString strFilter);
	void	AddNode(CTreeNode *pNode);	
public:
	int				m_nSelect;
	bool			m_bOutput;         //�Ƿ�������ļ�
	bool            m_bShowLabel;      //�Ƿ���ʾLabel
	CTStringArray   m_szArrHitRange;   //ģ������ʱ�ķ�Χ.
	int				m_nGenType;
	int				m_nPruneType;
	int				m_nMaxBranches;    //���ֲ���             
	int				m_nMaxDepth;       //������
	int				m_nMinCaseLeaf;    //Ҷ�ӽڵ����С������
	float			m_fMaxErrorRate;   //���֦������
	float			m_fMinInfoGain;     //��Ϣ����
	float			m_fMinGINIReduction;//GINI��
	double			m_dbCorrect;          //��ȷ��
	CDataAccess*	m_pSourceAccess;
	CDataAccess*	m_pTargetAccess;
	CTreeNode*		m_rootNode;
	CTString		m_strSourceTableName;
	CTString		m_strTempTableName;
	//int nRecNum;
	std::vector<PREDECTREE *>	m_vectRedDecTree;
	std::vector<TREERULE *>		m_vectTreeRule;
	
	std::vector<FieldClass*>	m_DiscreteFieldList;
	std::vector<FieldClass*>	m_ContinueFieldList;
	FieldClass					m_TargetField;
	CPtrArray					m_arSourceData;//��Ҫ��FieldSourceData�ṹ
	int							m_nTotalCase;
	int							m_nMissingRecNum;
	CPtrArray					m_arNode;
//	Tree *m_pTree;
private:
	bool* m_pbCheck;
	
public:
	bool GetCheck();
	void SetStopCheck(bool* bCheck);
	bool PostPruneTree(int nPruneType);
	bool CreateTree(int nGenType,int nPruneType);
	bool EmptyData();
	//void SetPrgWnd(CWnd *pPrgWnd,UINT prgMsg); -by pyx 2006-02-15
};


class CDecTreeOutput : public  StructBase
{
public:
	CDecTreeOutput();
	virtual string Export();//����ΪXML��ʽ
	virtual bool Export(string str);//����ΪXML��ʽ
	virtual bool Import(string str);
public:
	CDecTree *m_pDecTree;
	/*
	int  nGenTreeMethod;
	int	 nPrunMethod;
	int  nMinSample;
	int	 nMaxBranch;
	int	 nDepth;
	double  dMinInfoGain;
	double  dMinGINIReduction;
	double  dErrorRate;
	CTStringArray strInputNames;
	CTStringArray strDecNames;
	std::vector<FieldClass*> *pDiscreteFieldList;
	std::vector<FieldClass*> *pContinueFieldList;
	FieldClass  *pTargetField;
	CPtrArray	arNode;//TreeNode
	CPtrArray	arTreeNode;
	*/
};
#endif // !defined(AFX_AAAA_H__D87493B3_7649_4CF0_A328_119727C19A54__INCLUDED_)

