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
	//	�ֲڼ�VO����
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
	CTArray<int,int> attribsCTArray;//����
	CTArray<int,int> elementsCTArray;//Ԫ��
	CPtrArray pCPtrArray;//ָ����һ����ڵ��ָ������
	int nAttribClassfy;//�ýڵ�ķ������Ե�������������Ҫ���Ե�����
	int nIsAllSub;//�Ƿ����Ӽ� 
	//Ϊ0ʱ�������ݹ齨������
	//Ϊ1���Ѿ���Ҷ�ӽڵ㣬���Ӽ�
	//Ϊ2����Ҷ�ӽڵ㣬�����Ӽ����������Ѿ����꣬
	//Ϊ3����Ҷ�ӽڵ㣬�����Ӽ�������δ���꣬�����������������壬��Ϊ�ٷ����Ϊͬһ��
	CTString strAttribDecision;//�������Ե�ֵ
	CTString strRootNodeClassfyValue;//
} RoughSetStruct;



class  CRudeMathematics  
{
public:
	CDataInterface * m_pDataInterface;
	bool m_bIsToEnd;//�û��Ƿ���ֹ�˳���
	bool m_nComponentID;//
	CRudeMathematics();
	virtual ~CRudeMathematics();
protected:
	CPtrArray m_MapPtrArray;//�������͵�����ת����ӳ����Ϣ��
	TIntStruct m_tVarInfo;//�û�ѡ���ֶε�������Ϣ
	int m_nDecisionIndex;//��CDataInterface�У���ȡ�û�ָ������������������	
	int m_nSpecified;//�û�ָ�������Է��������е�����
	int m_nSpecifiedIndex;//��CDataInterface�У���ȡ�û�ָ������������������
	bool m_bSpecified;//�Ƿ�ָ�����û�ָ������
	CResult		   * m_pResult;
	CRsltElementText	*m_pRsltElementText;
	CTArray<int,int> m_ArOutArray;//Լ�����Լ�
	CTArray<int,int> m_ColIndexArray;
	CTArray<int,int> m_TempCoreArray;
	CTString m_strRecord;//ͬһ������ļ�¼�ţ�����CalkSD()���Ρ�
	CTStringArray m_RecordStrArray;//ͬ�����ʵļ�¼�д洢
	CTArray<int,int> m_DelArray ;//��¼��ɾ���Ĳ��Ϸ��ļ�¼��

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
