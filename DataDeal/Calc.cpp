// Calc.cpp: implementation of the CCalc class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Calc.h"
#include "../DataInterface/DataInterface.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include <math.h>
CCalc::CCalc()
{

}

CCalc::~CCalc()
{

}

/*
 * 输入数据集和表达式
 * 如果表达式错误，则返回false 否则返回true
 */
bool CCalc::Analyze(CDataInterface *pData,const string &strExpr)
{
	Clear();

	m_pData = pData;
	CAnalyzeWord aw;
	if (aw.Import(strExpr)==false)
	{
		return false;
	}

	aw.Print();
	int nSize = aw.GetWordCount();
	CNode *pNode = new CNode(itNode,"");
	if (pNode->BuildTree(aw,0,nSize-1)==false)
	{
		delete pNode;
		return false;
	}

	if (pNode->BuildBinaryTree() == false)
	{
		delete pNode;
		return false;
	}


	//printf("******************************\r\n");
	vector<CNode*> nodeary;
	pNode->BackTrace(nodeary);


	int i= 0;
	for (i=nodeary.size()-1;i>=0;i--)
	{
		if (nodeary[i]->GetNodeType() != itNode)
		{
			RItem *pItem = new RItem();
			pItem->copyfrom(nodeary[i]);
			//stk.push(pItem );
			m_ItemAry.push_back(pItem);
		}
	}

	delete pNode;
	pNode = NULL;
	
	/*
	vector<CField*> m_ColAry;//对应的字段数组
	vector<RItem*> m_ColItemAry;//相关的字段名 类型为 itFieldName
	vector<int> m_ColTypeAry;//对应的要输出的类型，按数字，或者字符，或者日期
	*/
	for (i=m_ItemAry.size()-1;i>=0;i--)
	{

		if (m_ItemAry[i]->m_nodeType == itFieldName)
		{
			//*
			CField *pField = m_pData->m_DataAccess.FieldByName( m_ItemAry[i]->strValue.c_str());
			if (pField== NULL)//没有找到节点，则失败
				return false;
			
			m_ColAry.push_back(pField);
			m_ColItemAry.push_back(m_ItemAry[i]);
			//*/
			//m_ColTypeAry.push_back() ===uc
		}
	}	
	return true;
}
/*
 * 运算出结果，返回结果数据
 */
double CCalc::Run()
{
	int i=0;
	for (i = m_ColAry.size()-1;i>=0;i--)
	{
		if (m_ColAry[i]->GetFieldType() == fString)
			m_ColItemAry[i]->strValue = m_ColAry[i]->GetAsString();
		else
			m_ColItemAry[i]->dbvalue = m_ColAry[i]->GetAsDouble();
	}

	stack<RItem*>stkNum;
	vector<RItem*>allcItem;//管理临时生成的对象
	int nPos = m_ItemAry.size()-1;
	while (nPos>=0)
	{
		RItem *pItem = m_ItemAry[nPos--];
		//stk.pop();
		if (pItem->m_nodeType == itOperation)
		{
			if (pItem->m_ot == opNot)
			{//单个操作符号
				RItem* p1 = stkNum.top();
				p1->dbvalue = !p1->dbvalue;
			}
			else
			{//双目

				RItem* p1 = stkNum.top();
				stkNum.pop();
				RItem* p2 = stkNum.top();
				stkNum.pop();
				RItem *pItemTemp = NULL;

				TwoOp(pItem->m_ot,pItemTemp,p1,p2);
				
				if ( pItemTemp!= NULL)
				{
					stkNum.push(pItemTemp);
					allcItem.push_back(pItemTemp);
				}
			}//end 双目

		}
		else if ( pItem->m_nodeType== itNode)
		{

		}
		else if ( pItem->m_nodeType== itFuncName)
		{
			pItem->strValue;//函数名
			RItem *pItemTemp = NULL;

			DealFunc(pItem->strValue,pItemTemp,stkNum);

			if ( pItemTemp!= NULL)
			{
				stkNum.push(pItemTemp);
				allcItem.push_back(pItemTemp);
			}

		}
		else
		{
			stkNum.push(pItem);
		}
	}

	double rst;


	if (stkNum.size() ==1)
	{
		rst = stkNum.top()->dbvalue;
	}
	else
	{
		rst = 0;
	}
	
	vector<RItem*>::iterator it;
	for (it = allcItem.begin();it!= allcItem.end();it++)
	{
		delete *it;
	}
	allcItem.clear();
	
	return rst;
}

void CCalc::Clear()
{
	m_ItemAry.clear();
	m_pData = NULL;

	//***************  以下是与字段相关的  ***************
	m_ColAry.clear();//对应的字段数组
	m_ColItemAry.clear();//相关的字段名 类型为 itFieldName
	m_ColTypeAry.clear();//对应的要输出的类型，按数字，或者字符，或者日期
	//以下是标准化相关操作
	m_vValue.clear();
}

/*
 * 双目运算符号
 */
void CCalc::TwoOp(OperationType ot, RItem *&pItemTemp, RItem *p1, RItem *p2)
{
	switch (ot)
	{
	case opAnd:
		{
			pItemTemp = (new RItem (p2->dbvalue && p1->dbvalue));
			break;
		}
	case opOr:
		{
			pItemTemp = (new RItem (p2->dbvalue || p1->dbvalue));
			break;
		}
	case opNot:
		{
			//pItemTemp = (new RItem (p2->dbvalue<=p1->dbvalue));
			break;
		}
	case opG:
		{
			pItemTemp = (new RItem (p2->dbvalue>p1->dbvalue));
			break;
		}
	case opGE:
		{
			pItemTemp = (new RItem (p2->dbvalue>=p1->dbvalue));
			break;
		}
	case opL:
		{
			pItemTemp = (new RItem (p2->dbvalue<p1->dbvalue));
			break;
		}
	case opLE:
		{
			pItemTemp = (new RItem (p2->dbvalue<=p1->dbvalue));
			break;
		}
	case opEQ:
		{			
			if (p1->m_nodeType == itString)
				pItemTemp = (new RItem (p2->strValue==p1->strValue));	
			else
				pItemTemp = (new RItem (p2->dbvalue==p1->dbvalue));	
			break;
		}
	case opNE:
		{
			if (p1->m_nodeType == itString)
				pItemTemp = (new RItem (p2->strValue!=p1->strValue));	
			else
				pItemTemp = (new RItem (p2->dbvalue!=p1->dbvalue));	
			break;
		}
	case opPlus:
		{
			pItemTemp = (new RItem (p2->dbvalue+p1->dbvalue));					
			break;
		}
	case opMinus:
		{
			pItemTemp = (new RItem (p2->dbvalue-p1->dbvalue));									
			break;
		}
	case opMultiply:
		{
			pItemTemp = (new RItem (p2->dbvalue*p1->dbvalue));									
			break;
		}
	case opDivide:
		{
			pItemTemp = (new RItem (p2->dbvalue/p1->dbvalue));									
			break;
		}
	case opMod:
		{
			pItemTemp = (new RItem (fmod(p2->dbvalue,p1->dbvalue)));									
			break;
		}
	default:
		{
			break;
		}
	}//end switch
}


/*
 * 调用函数处理，目前支持的函数有
sin		cos		asin	abs		acos	atan
ceil	cosh	exp		floor	log
log10	sinh	sqrt	tan		tanh
pow		max		min		recno	reccount
 */
bool CCalc::DealFunc(const string &strFuncName, RItem *&pItemTemp, stack<RItem*> &stkNum)
{
	//add by xds 20060817.
	string szFuncName = strlwr(strdup(strFuncName.c_str()));
	if (szFuncName == "abs" || szFuncName == "绝对值")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (fabs(p1->dbvalue));
	}
	else if (szFuncName == "sin" || szFuncName == "正弦")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (sin(p1->dbvalue));
	}
	else if (szFuncName == "asin" || szFuncName == "反正弦")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (asin(p1->dbvalue));
	}
	else if (szFuncName == "sinh" || szFuncName == "超正弦")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (sinh(p1->dbvalue));
	}
	else if (szFuncName == "cos" || szFuncName == "余弦")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (cos(p1->dbvalue));
	}
	else if (szFuncName == "acos" || szFuncName == "反余弦")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (acos(p1->dbvalue));
	}
	else if (szFuncName == "cosh" || szFuncName == "超余弦")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (cosh(p1->dbvalue));
	}
	else if (szFuncName == "tan" || szFuncName == "正切")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (tan(p1->dbvalue));
	}
	else if (szFuncName == "atan" || szFuncName == "反正切")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (atan(p1->dbvalue));
	}
	else if (szFuncName == "tanh" || szFuncName == "超正切")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (tanh(p1->dbvalue));
	}
	else if (szFuncName == "atan2" || szFuncName == "四象限反正切")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		RItem* p2 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem(atan2(p2->dbvalue, p1->dbvalue));
	}
	else if (szFuncName == "ceil" || szFuncName == "向上取整")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (ceil(p1->dbvalue));
	}
	else if (szFuncName == "floor" || szFuncName == "向下取整")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (floor(p1->dbvalue));
	}
	else if (szFuncName == "exp" || szFuncName == "自然指数")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (exp(p1->dbvalue));
	}
	else if (szFuncName == "log" || szFuncName == "自然对数")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (log(p1->dbvalue));
	}
	else if (szFuncName == "log10" || szFuncName == "常用对数")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (log10(p1->dbvalue));
	}
	else if (szFuncName == "sqrt" || szFuncName == "平方根")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (sqrt(p1->dbvalue));
	}
	else if (szFuncName == "pow" || szFuncName == "乘方")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		RItem* p2 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (pow(p2->dbvalue,p1->dbvalue));
	}
	else if (szFuncName == "max" || szFuncName == "最大值")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		RItem* p2 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (max(p2->dbvalue,p1->dbvalue));
	}
	else if (szFuncName == "min" || szFuncName == "最小值")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		RItem* p2 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (min(p2->dbvalue,p1->dbvalue));
	}
	else if (szFuncName == "recno" || szFuncName == "记录号")//当前行号
	{
		pItemTemp = new RItem (m_pData->m_DataAccess.GetRecNo());
	}
	else if (szFuncName == "reccount" || szFuncName == "记录数")//记录个数
	{
		pItemTemp = new RItem (m_pData->m_DataAccess.GetRecordCount());
	}
	else if (szFuncName == NORMALSTD_E || szFuncName == NORMALSTD_C) //正态标准化
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem(nors(p1->dbvalue));
	}
	else if (szFuncName == GENERALSTD_E || szFuncName == GENERALSTD_C) //常规标准化
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem(gens(p1->dbvalue));
	}
	/*
	else if (szFuncName == "XXXX")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (XXXX(p1->dbvalue));
	}
	*/
	else
	{//abs 
		return false;//没有找到合适的函数
	}

	return true;
}

void CCalc::SetValue(int nType, CDoubleVector &v)
{
	m_vValue.clear();
	if (nType == NOR_STD)
	{
		double dSum=0, dVar=0;
		v.sumvar(dSum, dVar);
		m_vValue.push_back(dSum/v.vlen());
		m_vValue.push_back(dVar);
	}
	else if(nType == GEN_STD)
	{
		double dMin=0, dMax=0;
		v.MaxMin(dMax, dMin);
		m_vValue.push_back(dMin);
		m_vValue.push_back(dMax);
	}
	v.destroy();
}

double CCalc::Value()
{
	int i=0;
	for (i = m_ColAry.size()-1;i>=0;i--)
	{
		m_ColItemAry[i]->dbvalue = m_ColAry[i]->GetAsDouble();
	}
	
	stack<RItem*>stkNum;
	vector<RItem*>allcItem;//管理临时生成的对象
	int nPos = m_ItemAry.size()-1;
	while (nPos>=0)
	{
		RItem *pItem = m_ItemAry[nPos--];
		//stk.pop();
		if (pItem->m_nodeType == itOperation)
		{
			if (pItem->m_ot == opNot)
			{//单个操作符号
				RItem* p1 = stkNum.top();
				p1->dbvalue = !p1->dbvalue;
			}
			else
			{//双目
				
				RItem* p1 = stkNum.top();
				stkNum.pop();
				RItem* p2 = stkNum.top();
				stkNum.pop();
				RItem *pItemTemp = NULL;
				
				TwoOp(pItem->m_ot,pItemTemp,p1,p2);
				
				if ( pItemTemp!= NULL)
				{
					stkNum.push(pItemTemp);
					allcItem.push_back(pItemTemp);
				}
			}//end 双目
			
		}
		else if ( pItem->m_nodeType== itNode)
		{
		}
		else if ( pItem->m_nodeType== itFuncName)
		{
		}
		else
		{
			stkNum.push(pItem);
		}
	}
	
	double rst;
	
	
	if (stkNum.size() ==1)
	{
		rst = stkNum.top()->dbvalue;
	}
	else
	{
		rst = 0;
	}
	
	vector<RItem*>::iterator it;
	for (it = allcItem.begin();it!= allcItem.end();it++)
	{
		delete *it;
	}
	allcItem.clear();
	
	return rst;
}

int CCalc::Type()
{
	int nRet = 0;
	int i=0, nStart=0;
	int nIndex=0, nCount=0;
	int nSize = m_ItemAry.size();
	if (nSize < 1)
		return FAIL_STD;
	for (i=nSize-1; i>=0; i--)
	{
		RItem *pItem = m_ItemAry.at(i);
		if (pItem->m_nodeType != itFuncName)
			continue;
		if (nStart == 0) nStart = i;
		if (pItem->strValue == NORMALSTD_C  || pItem->strValue == NORMALSTD_E)
		{
			nRet = NOR_STD;
			nCount ++;
			nIndex = i;
		}
		else if(pItem->strValue == GENERALSTD_C || pItem->strValue == GENERALSTD_E)
		{
			nRet = GEN_STD;
			nCount ++;
			nIndex = i;
		}
	}
	//判断
	if (nCount > 1 || (nCount == 1 && nIndex != nStart))
		return FAIL_STD;
	if (nCount == 1 && nIndex == nStart)
		return nRet;
	return NO_STD;
}

//正态标准化
double CCalc::nors(double dValue)
{
	int nSize = m_vValue.size();
	if (nSize != 2 || m_vValue.at(1) == 0)
		return dValue;
	dValue = (dValue - m_vValue.at(0))/m_vValue.at(1);
	return dValue;
}

//常规标准化
double CCalc::gens(double dValue)
{	
	int nSize = m_vValue.size();
	if (nSize != 2 || m_vValue.at(0) == m_vValue.at(1))
		return dValue;
	dValue = (dValue - m_vValue.at(0))/(m_vValue.at(1) - m_vValue.at(0));
	return dValue;
}
