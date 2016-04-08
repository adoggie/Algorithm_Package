// Sample.h: interface for the CSample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLE_H__F79DC4C4_9C9B_4C56_824B_A304F51C1FC2__INCLUDED_)
#define AFX_SAMPLE_H__F79DC4C4_9C9B_4C56_824B_A304F51C1FC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../DataInterface/DataInterface.h"

const char SAMPLE[]    = "SAMPLE";
const char FETCH[]     = "/FETCH";
const char ALL[]       = "ALL";
const char BEF[]       = "BEF";
const char SEP[]       = "SEP";
const char LAYER[]     = "LAYER";
const char LAYER_[]    = "/LAYER";
const char PERCENT[]   = "PERCENT";
const char NUMBER[]    = "NUMBER";
const char VARIN[]     = "/VARINPUT";
const char LAYVARIN[]  = "/LAYERVARINPUT";

//��������
const int   A = 1; //�����еİٷ���
const int   B = 2; //ǰ���ټ�¼
const int   S = 4; //ÿ������ȡ����
const int   L = 8; //����ȡ���ٷ���/������

class CDataInterface;
class CField;

class CSample  
{
public:
	CSample();
	virtual ~CSample();
public:
	bool Run(CDataInterface *pData, const string &VO);
protected:
	bool Parse(const string &VO);
	bool Sample_A(CDataAccess *pDataIn, CDataAccess *&pDataRet, double fPercent);
	bool Sample_B(CDataAccess *pDataIn, int nStartNo);
	bool Sample_S(CDataAccess *pDataIn, CDataAccess *&pDataRet, double fPercent);
	bool Sample_L(CDataAccess *pDataIn, CDataAccess *&pDataRet, double dPercent);
private:
	bool Verify();
	int CompareFieldValue(CField *pFieldA, CField *pFieldB);
	int IsDiff(CDataAccess *pDAA, CTArray<int,int>arrIndexA, CDataAccess *pDAB, CTArray<int,int>arrIndexB);
private:
	bool           m_bPercent; //�ֲ�ʱ�Ƿ�Ϊȡ�ٷ���
	int            m_nType;    //��������
	float          m_fValue;   //���ֵ�����ֿ���
	TIntStruct     m_tVar;     //���ȡʱ������
	TIntStruct     m_tLayerVar;//���ȡʱ�������
	CDataInterface *m_pData;
};

#endif // !defined(AFX_SAMPLE_H__F79DC4C4_9C9B_4C56_824B_A304F51C1FC2__INCLUDED_)
