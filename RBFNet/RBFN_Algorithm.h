// RBFN_Algorithm.h: interface for the RBFN_Algorithm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RBFN_ALGORITHM_H__6B36D291_F0F5_48E2_AFAF_995CCC96A331__INCLUDED_)
#define AFX_RBFN_ALGORITHM_H__6B36D291_F0F5_48E2_AFAF_995CCC96A331__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
//RBF�������㷨�ࣨ�����뵥�����
//�����㣬������޹ء�������κθĶ�����ά���㷨���Ĵ����ԣ�����ǿ�㷨����������
//�㷨������洫������ʱ�������㷨�����ⲿ����һ����,���ڴ��ݷ��Ͻ�����Ҫ������
//�㷨�������˴�������Ϣ����ֹ������Ϣ�����м�¼��Ϣ
//by ����
//2005-11
//////////////////////////////////////////////////////////////////////////
#include <sys/timeb.h>
#include <string>
#include <sstream>

#include "../cpknl/ErrorReporter.h"
#include "../cpknl/Vector.h"
#include "../cpknl/cpknl.h"
#include "../cpknl/Matrix.h"

#include "../Public/TCLFile.h"
#include "../Result/Result.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"



#include "KMeans_Algorithm.h"

using namespace std;


class CRBFNetVO
{
public:
	int				nCntrCnt;
	CTStringArray	strInputName;
	CTStringArray	strDestName;
	CRBFNetVO()
	{
		nCntrCnt = 10;
	}
	bool	Compile(string strVO)
	{
	//	RBF������VO����
	//	BBFNETTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/HIDENCENTER = CENTER(10**)

		AnaWord VOstr;
		VOstr.Import(strVO);
		if (AnaSyntax::findstr("/HIDENCENTER","CENTER",VOstr))
			nCntrCnt = AnaSyntax::getvalue("/HIDENCENTER","CENTER",VOstr);
		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputName);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDestName);
		return true;
	}
};


class CRBFNetOutput : public  StructBase
{
public:
	CRBFNetVO	*pRBFNetVO;

	CDoubleMatrix mxInputColMaxMin;
	CDoubleMatrix mxTargetColMaxMin;
	//RBF�������������Ԫ���꣬���ÿ��پ�����
	CDoubleMatrix mxStdCenter;
	//RBF�������Ŀ��
	CDoubleMatrix mxSigmaSquare;
	//RBF���������㵽������Ȩֵ
	CDoubleMatrix mxWeight;	
	CRBFNetOutput();
	virtual bool Export(string szFileName);
	virtual bool Import(string szFileName);
	
};

//RBF������ṹ
struct RBFNetStructure
{
	//������������
	int nInputCol;
	//Ŀ�������������̶�Ϊ1
	int nTargetCol;
	//�������ݾ������ĸ�������RBF������������Ԫ��������Ԥ��ָ��
	int nCenterNum;
	//ԭʼ���ݸ������ֵ��Сֵ(2�У��ֱ�洢������С)
	CDoubleMatrix mxInputColMaxMin;
	CDoubleMatrix mxTargetColMaxMin;
	//RBF�������������Ԫ���꣬���ÿ��پ�����
	CDoubleMatrix mxStdCenter;
	//RBF�������Ŀ��
	CDoubleMatrix mxSigmaSquare;
	//RBF���������㵽������Ȩֵ
	CDoubleMatrix mxWeight;	
};

class CRBFN_Algorithm
{
private:
	RBFNetStructure m_RBFNS;	//����ṹ
	bool m_bHasNetStructure;	//����Ƿ�������ṹ
	CKMeans_Algorithm m_KMeans;	//���ںͿ��پ����㷨������Ϣ����

public:
	CRBFN_Algorithm();
	virtual ~CRBFN_Algorithm();


	//RBF����ѵ�����õ�����������RBFN�������������ṹ
	bool Train(CDoubleMatrix& mxInput, CDoubleMatrix& mxTarget, int nCenterNum, 
		CDoubleMatrix& mxOutput, double& dMSE, double& dStdMSE);	
	//RBF�������
	bool Test(CDoubleMatrix& mxTestInput, CDoubleMatrix& mxTestTarget, 
		CDoubleMatrix& mxTestOutput, double& dMSE, double& dStdMSE);
	//RBF����Ԥ��
	bool Predict(CDoubleMatrix& mxPreInput, CDoubleMatrix& mxPreOutput);
	//RBF���籣����string//�ɿ�������"<<"
	bool SaveRBFNS(string& strRBFNS);
	//RBF�����string����
	bool LoadRBFNS(string& strRBFNS);
	//���RBF����ṹ
	RBFNetStructure GetRBFNS();
	//�Ƿ��������ṹ
	bool HasNetStructure();
	//ģ��
	bool Export(TCLFile &theFile);
	bool Import(TCLFile &theFile);
//////////////////////////////////////////////////////////////////////////
//���������
private:
	string m_strErrorInfo;		//��¼�㷨������Ϣ
	bool m_bError;				//����㷨�Ƿ����
public:	
	string GetErrorInfo();		//����㷨������Ϣ
	bool IsError();				//����m_bError	
//////////////////////////////////////////////////////////////////////////
//��ֹ�㷨���
private:
	bool m_bAbort;				//�㷨��ֹ��־
	string m_strAbortInfo;		//��¼�㷨��ֹ��Ϣ
public:
	//��ֹ����ѵ�������ڵ����㷨��Ϊ�������һ���̵߳���ʱ����������Ե��ô˺�������ֹѵ��
	//���õķ������ô�C++�Ķ�����Ϣ��Ӧ���ƣ��µ�C++��׼֧�ִ˻���
	void Abort();	
	string GetAbortInfo();		//�����㷨��ֹʱ������״̬��Ϣ
	bool IsAbort();				//����m_bAbort
//////////////////////////////////////////////////////////////////////////
//��¼������Ϣ���
private:
	string m_strRunInfo;		//��¼������Ϣ
	string FormatTimeSpan(_timeb timeBegin,_timeb timeEnd);		//���㿪ʼʱ�̵�����ʱ��֮��ĺ�ʱ
public:
	string GetRunInfo();		//���������Ϣ����������ʱ���
//////////////////////////////////////////////////////////////////////////

private:
	//�������������׼��
	bool MxStandardize(CDoubleMatrix& mx, CDoubleMatrix& mxColMaxMin, 
		CDoubleMatrix& mxStd, bool bHasMaxMin);
	//���������������׼��
	bool MxInverseStandardize(CDoubleMatrix& mxStd, CDoubleMatrix& mxColMaxMin,
		CDoubleMatrix& mx);
	//���پ���������������
	bool GetHideCenter(CDoubleMatrix& mxStd, int nCenterNum, CDoubleMatrix& mxStdCenter);
	//�������������
	bool CalculateHideOut(CDoubleMatrix& mxStdInput, CDoubleMatrix& mxStdCenter, 
		CDoubleMatrix& mxSigmaSquare, CDoubleMatrix& mxHideOut);
	//�ɾ������ļ���SigmaSquare���ο�Clementine 8.0 Algorithms	Guide��
	bool CalculateSigmaSquare(CDoubleMatrix& mxStdCenter,CDoubleMatrix& mxSigmaSquare);
	//����Ȩֵ,mxY=mxX*mxWeight
	bool CalculateWeight(CDoubleMatrix& mxX,CDoubleMatrix& mxY,CDoubleMatrix& mxWeight);

	//��д����ع��㷨�����ػع�ϵ�����ٶ������ݲ�ʹ��
	CDoubleMatrix RidgeEstimate(CDoubleMatrix &Matrix_X,CDoubleMatrix &Matrix_y, double dkValue);

	//����MSE(mean of squared errors)
	double MSE(CDoubleMatrix& mxTarget, CDoubleMatrix& mxOutput);
};

#endif // !defined(AFX_RBFN_ALGORITHM_H__6B36D291_F0F5_48E2_AFAF_995CCC96A331__INCLUDED_)
