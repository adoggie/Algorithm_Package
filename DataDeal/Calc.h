// Calc.h: interface for the CCalc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALC_H__E2BA78FA_8725_4564_A06A_D08B36679057__INCLUDED_)
#define AFX_CALC_H__E2BA78FA_8725_4564_A06A_D08B36679057__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AnalyzeWord.h"
#include "Node.h"
#include "../cpknl/cpknl.h"
class CDataInterface;
class CField;

const char  NORMALSTD_C[]  = "��̬��׼��";
const char  NORMALSTD_E[]  = "nors";
const char  GENERALSTD_C[] = "�����׼��";
const char  GENERALSTD_E[] = "gens";
const int   FAIL_STD       = -1; //���ڱ�׼�����﷨����
const int   NO_STD         = 0;  //�����ڱ�׼��
const int   NOR_STD        = 1;  //��̬��׼��
const int   GEN_STD        = 2;  //�����׼��
class CCalc  
{
	vector<RItem*> m_ItemAry;
	CDataInterface *m_pData;

	//***************  ���������ֶ���ص�  ***************
	vector<CField*> m_ColAry;//��Ӧ���ֶ�����
	vector<RItem*> m_ColItemAry;//��ص��ֶ��� ����Ϊ itFieldName
	vector<int> m_ColTypeAry;//��Ӧ��Ҫ��������ͣ���0:���֣�����1:�ַ������� 2:����
	//******************************
	//��������Ա�׼����ز���
	vector<double>m_vValue;
public:
	double Run();
	bool Analyze(CDataInterface *pData,const string& strExpr);
	//���ͼ��
	int Type();
	//����
	double Value();
	void   SetValue(int nType, CDoubleVector &v);
	CCalc();
	virtual ~CCalc();

protected:
	bool DealFunc(const string &strFuncName ,RItem *&pItemTemp,stack<RItem*>& stkNum);
	void TwoOp(OperationType ot,RItem *&pItemTemp ,RItem* p1 ,RItem* p2 );
	void Clear();
private:
	double nors(double dValue);
	double gens(double dValue);
};

#endif // !defined(AFX_CALC_H__E2BA78FA_8725_4564_A06A_D08B36679057__INCLUDED_)
