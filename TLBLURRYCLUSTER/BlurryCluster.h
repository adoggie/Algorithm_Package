// BlurryCluster.h: interface for the CBlurryCluster class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_BLURRYCLUSTER_H__651C1D4A_2E2B_4582_91A3_75A0674F0AF7__INCLUDED_)
#define AFX_BLURRYCLUSTER_H__651C1D4A_2E2B_4582_91A3_75A0674F0AF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#define WM_PROGRESS_TEXT 8
#include "../arithInterface/EnvParam.h"
#include <string>
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"

//
///*
//*算法和外界动态交互，是否需要输出反馈信息，是否终止
//*/
//struct Interaction
//{
//	bool bNeedFeedback;//输入：告诉算法是否需要写反馈信息。true：必须反馈，false：不用
//	bool bAbort;//输入：true终止当前程序。false：继续运行。默认false
//	char BufMsg[100];//输出：输出当前运行信息	
//};
//
///*
//输入的控制信息的基类，如哪些变量作为因变量，哪些作为自变量，结果集中哪些需要输出
//主要控制结果集显示问题，每一个算法都不一样。
//*/
//class VOBase{
//};
//
///*
//算法模型信息，主要用于算法训练时，输出的模型信息，用于下一次的预测。
//每一个算法都不一样。
//*/
//class StructBase{
//public :
//	string Export();//导出为XML方式
//	bool Import(string str);
//};
//
///*
//环境控制类，设置系统参数，以下方法为C算法内部调用的方法
//*/
//class CEnvParam{
//public:
//	//******  指定那些参数需要输入到参数中  *** 
//	bool HasOutParm(CTString sName);//判断是否需要输出某个参数，如yresid
//	string GetOutParamName(CTString sName);//
//	//*************设置环境变量************
//	bool SetVar(CTString sVarName,double value);
//	bool SetVar(CTString sVarName,CDoubleVector* value);//value 内存空间由算法自己释放。
//	bool SetVar(CTString sVarName,CDoubleMatrix* value);//value 内存空间由算法自己释放。
//	bool SetVar(CTString sVarName,CDataAccess* value);//value 内存空间由算法自己释放。
//};
//

typedef struct TBlurryTree
{
	double dCredit;//置信水平，也相当于节点横坐标
	double yPosition;//该节点的纵坐标
	CPtrArray NodePtrArray;//指向下一层节点的指针数组
	CTArray<int,int> elemsArray;//该节点的元素
	bool bBottom;//是否为最底层节点
	bool bRead;//是否被访问过，当不同的节点有相同的置信水平时，用于输出时判断
}TBlurryTree;

//参数和字段的输入，主要是通过界面输入
class CBlurryVO {
public:
	CTStringArray	arFeildInfo;//输入字段的信息，主要是TFeildInfo信息
	int			nSelect;//等级数选择，0变量最大值，1变量不同值
	double		dCredit;//置信度
	bool		bIcicle;//是否输出冰柱图
	bool		bPedigree;//是否输出谱系图
	CBlurryVO()
	{
		nSelect = 0;
		dCredit  = 0.8;
		bIcicle = false;
		bPedigree = true;
	}
	bool Compile(std::string strVO)
	{
	//	模糊聚类VO设置
	//	BLURRYTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/LEVEL [{VARMAX**},{VARDIFF}]
	//	/OUTPUT [ICICLE PEDIGREE]
	//	/CREDITLEVEL CREDIT(0.8**)
		
		AnaWord VOstr;
		VOstr.Import(strVO);
		
		nSelect = AnaSyntax::findstr("/LEVEL","VARDIFF",VOstr) ? 1:0;
		bIcicle = AnaSyntax::findstr("/OUTPUT","ICICLE",VOstr);
		bPedigree = AnaSyntax::findstr("/OUTPUT","PEDIGREE",VOstr);
		
		if (AnaSyntax::findstr("/CREDITLEVEL","CREDIT",VOstr))
			dCredit = AnaSyntax::getvalue("/CREDITLEVEL","CREDIT",VOstr);
		
		AnaSyntax::getVAR("/VARINPUT", VOstr, arFeildInfo);
		return true;
	}
};

/*
//聚类模型的结构，预测模型所需要的机构
typedef struct tagStructBlurryModel
{
	CBlurryVO	*pBlurryVO;	//界面输入字段和参数
	double		dblCreditLevel; //置信水平
	CPtrArray	arCluster;	//聚类情况
}StructBlurryModel;
*/

//聚类结果的输出
//聚类模型的结构，预测模型所需要的机构
class  CBlurryOutput : public StructBase
{
public:
	CBlurryOutput(){;}
	virtual ~CBlurryOutput(){;}
	virtual	std::string Export();//导出为XML方式
	virtual	bool Import(std::string str);
public:
	//VO info
	CTStringArray	arFeildInfo;//输入字段的信息，主要是TFeildInfo信息
	int			nSelect;//等级数选择，0变量最大值，1变量不同值
	double		dCredit;//置信度
	bool		bIcicle;//是否输出冰柱图
	bool		bPedigree;//是否输出谱系图
	//cluster info
	double		dblCreditLevel; //置信水平
	CPtrArray	arCluster;	//聚类情况
};

class CBlurryCluster 
{
public:
	void SetOutSel(bool bIce,bool bPedigree);
	CBlurryCluster();
	virtual ~CBlurryCluster();

	CResult * RunTrain(CDataInterface *pDataInterface,CEnvParam &pEnv, Interaction *&itt,CBlurryVO *pVO);
	CResult * Run(CDataInterface *pDataInterface, CPtrArray * &pCPtrArray);
	bool		SetDataParam(CBlurryVO *pVO);
	CDataInterface * m_pDataInterface;
	bool m_bIsToEnd;//是否终止程序

protected:
	int m_nOutSel; //==1 输出冰柱图，==2 输出谱系图 ，==3 两者都输出  [mxm add it]
	CPtrArray m_FirstClassfyPtrArray;
	CTArray<double,double> m_dCreditArray;
	TIntStruct m_tVarInfo;
	double m_dLambda;
	CBlurryVO		*m_pBlurryVO;
	CResult		   * m_pResult;
	CRsltElementText *m_pRsltElementText;
	CTStringArray *m_pNameStrArray;//字段名，从界面传过来
	CTStringArray *m_pSpecifyStrArray;
	int m_nSelect;//等级数设置方式
					//0，最大值+1；
					//1，不同值数，此时采取映射方式，从0开始取值；
					//2，由用户指定值；
	int m_nDataSize;

	bool CreatePedigree(PedigreeTreeNode& treeNode, TBlurryTree &node);
	CResult * CBlurryCluster::OnBlurryCluster();
	bool MatrixCreate(CIntMatrix &evalue_matrix,CIntMatrix &grade_matrix,double lambda);
	double myMean(CDoubleMatrix matrix,int i);
	double variance(CDoubleMatrix matrix,int i);
	double Xij(CDoubleMatrix matrix,int i,int j);
	double rij(CDoubleMatrix matrix,int i,int j);	
	bool compareMatrix(CDoubleMatrix matrix1,CDoubleMatrix matrix2);
	bool CBlurryCluster::matrixMultiply(CDoubleMatrix &matrix,CDoubleMatrix &temp_matrix);
	bool compareij(CDoubleMatrix matrix,int i,int j);
	bool CBlurryCluster::compareij(CIntMatrix matrix,int i,int j);
	void CreateResult(TBlurryTree &node,CDoubleMatrix &R_matrix,CDoubleMatrix &blurry_matrix,CPtrArray& group,CTArray<int,int> &OrderedArray);
	bool IsMatrixColEqual(CDoubleMatrix matrix,int j);
	int * GetData(CDataInterface *pDataInterface,TIntStruct tVarInfo,int bRemoveEqual=false);
	bool notIn(int value,CTArray<int,int> &intArray);
	int GetMapData(int index,CTArray<int,int> &intArray1,CTArray<int,int> &intArray2);
	int GetColMapData(CIntMatrix &matrix,int nMatrixCol,int nDataCol);
	int GetMaxValue(int nColIndex);
	void classfy(CIntMatrix W_matrix,CPtrArray &ClassfyCPtrArray,CTArray<int,int> &elemsArray);
	void delCPtrArray(CPtrArray &toDel);
	void makeOrder(CTArray<int,int> &core);

	/////绘图
	double  MiddleClassfy(CPtrArray &classfyPtrArray,//用于分类元素的存储
						   double &dLowCredit,//中值法小值
						   double &dHiCredit,//中值法大值
						   CTArray<int,int> &elemsArray,//某节点的元素（记录），对应模糊分类关系矩阵的行号
						   CDoubleMatrix &R_matrix,int nSubSize)	;//模糊分类关系矩阵				   

	void  ToCArray(CPtrArray * &a,CTArray<int,int> &b);
	void  CreateTree(CDoubleMatrix &R_matrix,TBlurryTree &node,CPtrArray &elemsPtrArray);
	void  GetYPositionArray(TBlurryTree &node,CTArray<int,int> &OrderedElemsArray);
	int   GetOrdinalPosition(CTArray<int,int> &OrderedElemsArray,int value);
	void  CompleteTreeYPositionValue(TBlurryTree &node,CTArray<int,int> &OrderedElemsArray);
// 	void  DrawBlurryTree(TBlurryTree &node/*,CDC *pDC*/);
	bool CBlurryCluster::GetClassfyState(TBlurryTree &node,double dCredit,CPtrArray &classPtrAr,CTArray<int,int>&class2Array);
	bool CBlurryCluster::ChangeClassLinker(TBlurryTree &node,CTStringArray &LinkerStrArray,CTArray<int,int> &OrderedElemsArray,double dCredit);
	void CBlurryCluster::RecoveryOrgData(CPtrArray &group,CPtrArray &classfy,CPtrArray &OutPutCPtrArray);
	CIntMatrix CBlurryCluster::CreateNewMatrix(CPtrArray &classfy,CIntMatrix matrix);
	void CBlurryCluster::DeleteTree(TBlurryTree &node);

};

#endif // !defined(AFX_BLURRYCLUSTER_H__651C1D4A_2E2B_4582_91A3_75A0674F0AF7__INCLUDED_)
