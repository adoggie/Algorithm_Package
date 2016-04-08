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

typedef struct TBlurryTree
{
	double dCredit;//����ˮƽ��Ҳ�൱�ڽڵ������
	double yPosition;//�ýڵ��������
	CPtrArray NodePtrArray;//ָ����һ��ڵ��ָ������
	CTArray<int,int> elemsArray;//�ýڵ��Ԫ��
	bool bBottom;//�Ƿ�Ϊ��ײ�ڵ�
	bool bRead;//�Ƿ񱻷��ʹ�������ͬ�Ľڵ�����ͬ������ˮƽʱ���������ʱ�ж�
}TBlurryTree;

//�������ֶε����룬��Ҫ��ͨ����������
class CBlurryVO {
public:
	CTStringArray	arFeildInfo;//�����ֶε���Ϣ����Ҫ��TFeildInfo��Ϣ
	int			nSelect;//�ȼ���ѡ��0�������ֵ��1������ֵͬ
	double		dCredit;//���Ŷ�
	bool		bIcicle;//�Ƿ��������ͼ
	bool		bPedigree;//�Ƿ������ϵͼ
	CBlurryVO()
	{
		nSelect = 0;
		dCredit  = 0.8;
		bIcicle = false;
		bPedigree = true;
	}
	bool Compile(std::string strVO)
	{
	//	ģ������VO����
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
//����ģ�͵Ľṹ��Ԥ��ģ������Ҫ�Ļ���
typedef struct tagStructBlurryModel
{
	CBlurryVO	*pBlurryVO;	//���������ֶκͲ���
	double		dblCreditLevel; //����ˮƽ
	CPtrArray	arCluster;	//�������
}StructBlurryModel;
*/

//�����������
//����ģ�͵Ľṹ��Ԥ��ģ������Ҫ�Ļ���
class  CBlurryOutput : public StructBase
{
public:
	CBlurryOutput(){;}
	virtual ~CBlurryOutput(){;}
	virtual	std::string Export();//����ΪXML��ʽ
	virtual	bool Import(std::string str);
public:
	//VO info
	CTStringArray	arFeildInfo;//�����ֶε���Ϣ����Ҫ��TFeildInfo��Ϣ
	int			nSelect;//�ȼ���ѡ��0�������ֵ��1������ֵͬ
	double		dCredit;//���Ŷ�
	bool		bIcicle;//�Ƿ��������ͼ
	bool		bPedigree;//�Ƿ������ϵͼ
	//cluster info
	double		dblCreditLevel; //����ˮƽ
	CPtrArray	arCluster;	//�������
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
	bool m_bIsToEnd;//�Ƿ���ֹ����

protected:
	int m_nOutSel; //==1 �������ͼ��==2 �����ϵͼ ��==3 ���߶����  [mxm add it]
	CPtrArray m_FirstClassfyPtrArray;
	CTArray<double,double> m_dCreditArray;
	TIntStruct m_tVarInfo;
	double m_dLambda;
	CBlurryVO		*m_pBlurryVO;
	CResult		   * m_pResult;
	CRsltElementText *m_pRsltElementText;
	CTStringArray *m_pNameStrArray;//�ֶ������ӽ��洫����
	CTStringArray *m_pSpecifyStrArray;
	int m_nSelect;//�ȼ������÷�ʽ
					//0�����ֵ+1��
					//1����ֵͬ������ʱ��ȡӳ�䷽ʽ����0��ʼȡֵ��
					//2�����û�ָ��ֵ��
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

	/////��ͼ
	double  MiddleClassfy(CPtrArray &classfyPtrArray,//���ڷ���Ԫ�صĴ洢
						   double &dLowCredit,//��ֵ��Сֵ
						   double &dHiCredit,//��ֵ����ֵ
						   CTArray<int,int> &elemsArray,//ĳ�ڵ��Ԫ�أ���¼������Ӧģ�������ϵ������к�
						   CDoubleMatrix &R_matrix,int nSubSize)	;//ģ�������ϵ����				   

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
