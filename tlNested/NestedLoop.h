// NestedLoop.h: interface for the CNestedLoop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESTEDLOOP_H__7A0E0AA3_7A33_42EB_A681_F322F8EB283A__INCLUDED_)
#define AFX_NESTEDLOOP_H__7A0E0AA3_7A33_42EB_A681_F322F8EB283A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"

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





typedef struct NestedStruct
{
	int row;
	double value;
}NestedStruct;

typedef struct TMaxAndMin
{
	double max;
	double min;
	double mean;
}TMaxAndMin;

//������������ӽ�������Ĳ���
class CNestedVO
{
public:
	CTStringArray	strInputName;//���������ֶε���Ϣ
	int nDistance;//���������
	int nNum;//���������
	int nVarOrRec;//0�Ƿ��������¼��1�Ƿ�������ֶΣ�������
	CNestedVO()
	{
		nDistance = 1;
		nNum = 1;
		nVarOrRec = 0;
	}
	bool Compile(string strVO)
	{
	//	������VO����
	//	NESTEDRUN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/MISSMETHOD DELRECORD
	//	/WRAPDISTANCE DISTANCE(1)
	//	/NESTEDCOUNT COUNT(1)
	//	/METHOD [{DIVRCD**},{DIVFIELD}]		



		AnaWord VOstr;
		VOstr.Import(strVO);
		
		nVarOrRec = AnaSyntax::findstr("/METHOD","DIVFIELD",VOstr);
		
		nDistance = AnaSyntax::getvalue("/WRAPDISTANCE","DISTANCE",VOstr);
		nNum = AnaSyntax::getvalue("/NESTEDCOUNT","COUNT",VOstr);
		
		if (nDistance < 0)
			nDistance = 1;
		if (nNum < 0)
			nNum = 1;

		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputName);
		return true;
	}
};

/*
//���������������������
typedef struct tagStructNestedModel
{
	TNestedVO	*pNestedVO;//�����������
	CPtrArray	arNested;//���������������ҹ����������
}StructNestedModel;
*/

//���������������������
//���������������������
class CNestedOutput : public StructBase
{
public:
	CNestedOutput(){;}
	virtual ~CNestedOutput(){;}
	virtual bool Import(string szFileName);
	virtual	bool Export(string szFileName);

protected:
	CNestedVO	*pNestedVO;//�����������
	CPtrArray	arNested;//���������������ҹ����������
};

class  CNestedLoop 
{
public:
	CNestedLoop();
	virtual ~CNestedLoop();	
	CDataInterface * m_pDataInterface;
	CResult *  RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,CNestedVO *VO);
	bool	SetDataParam(CNestedVO *pVO);
		
	bool m_bIsToEnd;
	int m_nVarOrRec;//���������0���ֶΣ�����1����¼
	CNestedVO *m_pNestedVO;
protected:
	CResult	* m_pResult;			
	CRsltElementText *m_pRsltElementText;

	TIntStruct m_tVarInfo;
	CPtrArray m_MaxAndMinPtrArray;
	int m_nDistance;
	int m_nSolarity;
protected:

	CResult * OnNestedLoop();
	void CreateResult(CDoubleMatrix W_matrix,CPtrList &outCPtrList);
	bool Nested(CDoubleMatrix W_matrix,int k,int n);
	double CalcDstij(CDoubleMatrix matrix,int i,int j);
	double GetDkDst(CDoubleMatrix matrix,int i,int k,double freshold,bool bTrans);
	CDoubleMatrix  CNestedLoop::GetData();
	double GetMaxAndMin(int nColIndex,double &max,double &min);
	bool OutIntType(int nCol);
	double GetMaxAndMin();
};

#endif // !defined(AFX_NESTEDLOOP_H__7A0E0AA3_7A33_42EB_A681_F322F8EB283A__INCLUDED_)
