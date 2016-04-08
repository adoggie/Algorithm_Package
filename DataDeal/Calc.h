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

const char  NORMALSTD_C[]  = "正态标准化";
const char  NORMALSTD_E[]  = "nors";
const char  GENERALSTD_C[] = "常规标准化";
const char  GENERALSTD_E[] = "gens";
const int   FAIL_STD       = -1; //存在标准化但语法错误
const int   NO_STD         = 0;  //不存在标准化
const int   NOR_STD        = 1;  //正态标准化
const int   GEN_STD        = 2;  //常规标准化
class CCalc  
{
	vector<RItem*> m_ItemAry;
	CDataInterface *m_pData;

	//***************  以下是与字段相关的  ***************
	vector<CField*> m_ColAry;//对应的字段数组
	vector<RItem*> m_ColItemAry;//相关的字段名 类型为 itFieldName
	vector<int> m_ColTypeAry;//对应的要输出的类型，按0:数字，或者1:字符，或者 2:日期
	//******************************
	//以下是针对标准化相关操作
	vector<double>m_vValue;
public:
	double Run();
	bool Analyze(CDataInterface *pData,const string& strExpr);
	//类型检查
	int Type();
	//计算
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
