// RudeMathematics.h: interface for the CRudeMathematics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUDEMATHEMATICS_H__1B3D395F_6AE1_41D9_B5E7_D453D5AFF9AC__INCLUDED_)
#define AFX_RUDEMATHEMATICS_H__1B3D395F_6AE1_41D9_B5E7_D453D5AFF9AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "RoughInterface.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"

class CRoughtVO
{
public:
	CTStringArray	strInputName;
	CTStringArray	strspecInputName;
	CTStringArray	strDecName;
	CRoughtVO() {
	}
	bool	Compile(string strVO)
	{
	//	粗糙集VO设置
	//	ROUGH
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARSPECINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD		


		AnaWord VOstr;
		VOstr.Import(strVO);
		
		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputName);
		AnaSyntax::getVAR("/VARSPECINPUT", VOstr, strspecInputName);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDecName);
		return true;
	}
};


typedef struct TRoughSetStruct
{
	CTArray<int,int> attribsCTArray;//属性
	CTArray<int,int> elementsCTArray;//元素
	CPtrArray pCPtrArray;//指向下一层各节点的指针数组
	int nAttribClassfy;//该节点的分类属性的索引，即最重要属性的索引
	int nIsAllSub;//是否是子集 
	//为0时，继续递归建立树，
	//为1，已经是叶子节点，是子集
	//为2，是叶子节点，不是子集，但属性已经用完，
	//为3，是叶子节点，不是子集，属性未用完，但继续分类已无意义，因为再分类均为同一类
	CTString strAttribDecision;//决策属性的值
	CTString strRootNodeClassfyValue;//
} RoughSetStruct;



class  CRudeMathematics  
{
public:
	CDataInterface * m_pDataInterface;
	bool m_bIsToEnd;//用户是否终止了程序
	bool m_nComponentID;//
	CRudeMathematics();
	virtual ~CRudeMathematics();
protected:
	CPtrArray m_MapPtrArray;//其它类型到整型转换的映射信息；
	TIntStruct m_tVarInfo;//用户选择字段的索引信息
	int m_nDecisionIndex;//在CDataInterface中，求取用户指定决策属性名的索引	
	int m_nSpecified;//用户指定的属性放在数组中的索引
	int m_nSpecifiedIndex;//在CDataInterface中，求取用户指定条件属性名的索引
	bool m_bSpecified;//是否指定了用户指定属性
	CResult		   * m_pResult;
	CRsltElementText	*m_pRsltElementText;
	CTArray<int,int> m_ArOutArray;//约简属性集
	CTArray<int,int> m_ColIndexArray;
	CTArray<int,int> m_TempCoreArray;
	CTString m_strRecord;//同一种情况的记录号，用于CalkSD()传参。
	CTStringArray m_RecordStrArray;//同类性质的记录行存储
	CTArray<int,int> m_DelArray ;//记录被删除的不合法的记录行

public:
	CResult * RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,CRoughtVO *pVO);

protected:

	CResult * CRudeMathematics::OnRoughSet();
	bool compareij(CIntMatrix matrix,int i,int j,CTArray<int,int> &stdArray);
	bool CRudeMathematics::compareij(CIntMatrix matrix,int i,int j);
	void delCPtrArray(CPtrArray &toDel);
	void classfy(CTArray<int,int> &stdCTArray,CIntMatrix W_matrix,CPtrArray &ClassfyCPtrArray);
	void copyPtrArray(CPtrArray &dest,CPtrArray &src);
	bool notIn(int value,CTArray<int,int> &coreCTArray);
	bool IsIncluded(CTArray<int,int> &minCTArray,CTArray<int,int> &maxCTArray);
	bool IsIncluded(CPtrArray *pmin,CPtrArray *pmax);
	void AppendToCTArray(CTArray<int,int> &dest,CTArray<int,int> &src);
	void delCTArray(CTArray<int,int> & tempCTArray);
	void ToCTArray(CPtrArray * &a,CTArray<int,int> &b);
	int statistics(CPtrArray &min,CPtrArray &max);
	bool RoughSet(CIntMatrix W_matrix,bool bSpecified=false);
	void CRudeMathematics::CreateResult(CIntMatrix outMatrix,CTArray<int,int> &core,CTArray<int,int> &tempcore,CPtrArray &classfyCPtrArray,CTStringArray &rsltCStringArray,CTStringArray &CDCStringArray,CTStringArray &SDCStringArray,CTStringArray &DecisionValueCStringArray);
	void storeGrade(CTArray<int,int> &grade,CIntMatrix W_matrix);
	void CalcCore(CTArray<int,int> &colCTArray,CTArray<int,int> &rowCTArray,CTArray<int,int> &coreCTArray,CIntMatrix W_matrix);
	void makeOrder(CTArray<int,int> &core);
	void copyCTArray(CTArray<int,int> &dest,CTArray<int,int> &src);


	/////////////////// serve for creating the decision tree

	int MostImpt(CTArray<int,int> & Attrib,CTArray<int,int> &elementsCTArray,CIntMatrix outMatrix,CPtrArray &dCPtrArray);
	bool isSubSet(CTArray<int,int> &elementsCTArray,CPtrArray &dCPtrArray);
	void classfy(CTArray<int,int> &stdCTArray,CTArray<int,int> &rowCTArray,CIntMatrix W_matrix,CPtrArray &ClassfyCPtrArray);
	void CreateTree(RoughSetStruct & root,CIntMatrix outMatrix,CPtrArray &dCPtrArray);
	void ReadTree(RoughSetStruct &node,CTArray<int,int> &tempCoreCTArray,CTStringArray &rsltCStringArray,CTStringArray &CDCStringArray,CTStringArray &SDCStringArray,CTString &OutStr,CPtrArray &classfyCPtrArray,CIntMatrix outMatrix,CIntMatrix W_matrix,CTStringArray &DecisionValueCStringArray);
	CTString GetOrgFieldName(RoughSetStruct &node,CTArray<int,int> &tempCoreArray);
	CTString GetDecisionArName();
	CTString GetSpecifiedName();
	double CalcSD(CPtrArray &classfyCPtrArray,CIntMatrix W_matrix,CTArray<int,int> &ElementsCTArray);
	void DeleteTree(RoughSetStruct &node);

	//append map function
	void CRudeMathematics::GetColMapData(CIntMatrix &matrix,int nMatrixCol,int nDataCol);
	CTString CRudeMathematics::GetMapStrValue(int nValue,int nCol);
	void CRudeMathematics::GetStringName(int index,CTStringArray &strArray1,CTStringArray &strArray2);
	bool CRudeMathematics::notIn(CTString str,CTStringArray &strArray);
	CIntMatrix CRudeMathematics::CreateNewMatrix(CIntMatrix matrix);


};

#endif // !defined(AFX_RUDEMATHEMATICS_H__1B3D395F_6AE1_41D9_B5E7_D453D5AFF9AC__INCLUDED_)
