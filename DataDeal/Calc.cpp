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
 * �������ݼ��ͱ��ʽ
 * ������ʽ�����򷵻�false ���򷵻�true
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
	vector<CField*> m_ColAry;//��Ӧ���ֶ�����
	vector<RItem*> m_ColItemAry;//��ص��ֶ��� ����Ϊ itFieldName
	vector<int> m_ColTypeAry;//��Ӧ��Ҫ��������ͣ������֣������ַ�����������
	*/
	for (i=m_ItemAry.size()-1;i>=0;i--)
	{

		if (m_ItemAry[i]->m_nodeType == itFieldName)
		{
			//*
			CField *pField = m_pData->m_DataAccess.FieldByName( m_ItemAry[i]->strValue.c_str());
			if (pField== NULL)//û���ҵ��ڵ㣬��ʧ��
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
 * �������������ؽ������
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
	vector<RItem*>allcItem;//������ʱ���ɵĶ���
	int nPos = m_ItemAry.size()-1;
	while (nPos>=0)
	{
		RItem *pItem = m_ItemAry[nPos--];
		//stk.pop();
		if (pItem->m_nodeType == itOperation)
		{
			if (pItem->m_ot == opNot)
			{//������������
				RItem* p1 = stkNum.top();
				p1->dbvalue = !p1->dbvalue;
			}
			else
			{//˫Ŀ

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
			}//end ˫Ŀ

		}
		else if ( pItem->m_nodeType== itNode)
		{

		}
		else if ( pItem->m_nodeType== itFuncName)
		{
			pItem->strValue;//������
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

	//***************  ���������ֶ���ص�  ***************
	m_ColAry.clear();//��Ӧ���ֶ�����
	m_ColItemAry.clear();//��ص��ֶ��� ����Ϊ itFieldName
	m_ColTypeAry.clear();//��Ӧ��Ҫ��������ͣ������֣������ַ�����������
	//�����Ǳ�׼����ز���
	m_vValue.clear();
}

/*
 * ˫Ŀ�������
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
 * ���ú�������Ŀǰ֧�ֵĺ�����
sin		cos		asin	abs		acos	atan
ceil	cosh	exp		floor	log
log10	sinh	sqrt	tan		tanh
pow		max		min		recno	reccount
 */
bool CCalc::DealFunc(const string &strFuncName, RItem *&pItemTemp, stack<RItem*> &stkNum)
{
	//add by xds 20060817.
	string szFuncName = strlwr(strdup(strFuncName.c_str()));
	if (szFuncName == "abs" || szFuncName == "����ֵ")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (fabs(p1->dbvalue));
	}
	else if (szFuncName == "sin" || szFuncName == "����")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (sin(p1->dbvalue));
	}
	else if (szFuncName == "asin" || szFuncName == "������")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (asin(p1->dbvalue));
	}
	else if (szFuncName == "sinh" || szFuncName == "������")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (sinh(p1->dbvalue));
	}
	else if (szFuncName == "cos" || szFuncName == "����")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (cos(p1->dbvalue));
	}
	else if (szFuncName == "acos" || szFuncName == "������")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (acos(p1->dbvalue));
	}
	else if (szFuncName == "cosh" || szFuncName == "������")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (cosh(p1->dbvalue));
	}
	else if (szFuncName == "tan" || szFuncName == "����")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (tan(p1->dbvalue));
	}
	else if (szFuncName == "atan" || szFuncName == "������")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (atan(p1->dbvalue));
	}
	else if (szFuncName == "tanh" || szFuncName == "������")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (tanh(p1->dbvalue));
	}
	else if (szFuncName == "atan2" || szFuncName == "�����޷�����")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		RItem* p2 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem(atan2(p2->dbvalue, p1->dbvalue));
	}
	else if (szFuncName == "ceil" || szFuncName == "����ȡ��")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (ceil(p1->dbvalue));
	}
	else if (szFuncName == "floor" || szFuncName == "����ȡ��")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (floor(p1->dbvalue));
	}
	else if (szFuncName == "exp" || szFuncName == "��Ȼָ��")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (exp(p1->dbvalue));
	}
	else if (szFuncName == "log" || szFuncName == "��Ȼ����")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (log(p1->dbvalue));
	}
	else if (szFuncName == "log10" || szFuncName == "���ö���")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (log10(p1->dbvalue));
	}
	else if (szFuncName == "sqrt" || szFuncName == "ƽ����")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (sqrt(p1->dbvalue));
	}
	else if (szFuncName == "pow" || szFuncName == "�˷�")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		RItem* p2 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (pow(p2->dbvalue,p1->dbvalue));
	}
	else if (szFuncName == "max" || szFuncName == "���ֵ")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		RItem* p2 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (max(p2->dbvalue,p1->dbvalue));
	}
	else if (szFuncName == "min" || szFuncName == "��Сֵ")
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		RItem* p2 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem (min(p2->dbvalue,p1->dbvalue));
	}
	else if (szFuncName == "recno" || szFuncName == "��¼��")//��ǰ�к�
	{
		pItemTemp = new RItem (m_pData->m_DataAccess.GetRecNo());
	}
	else if (szFuncName == "reccount" || szFuncName == "��¼��")//��¼����
	{
		pItemTemp = new RItem (m_pData->m_DataAccess.GetRecordCount());
	}
	else if (szFuncName == NORMALSTD_E || szFuncName == NORMALSTD_C) //��̬��׼��
	{
		RItem* p1 = stkNum.top();
		stkNum.pop();
		pItemTemp = new RItem(nors(p1->dbvalue));
	}
	else if (szFuncName == GENERALSTD_E || szFuncName == GENERALSTD_C) //�����׼��
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
		return false;//û���ҵ����ʵĺ���
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
	vector<RItem*>allcItem;//������ʱ���ɵĶ���
	int nPos = m_ItemAry.size()-1;
	while (nPos>=0)
	{
		RItem *pItem = m_ItemAry[nPos--];
		//stk.pop();
		if (pItem->m_nodeType == itOperation)
		{
			if (pItem->m_ot == opNot)
			{//������������
				RItem* p1 = stkNum.top();
				p1->dbvalue = !p1->dbvalue;
			}
			else
			{//˫Ŀ
				
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
			}//end ˫Ŀ
			
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
	//�ж�
	if (nCount > 1 || (nCount == 1 && nIndex != nStart))
		return FAIL_STD;
	if (nCount == 1 && nIndex == nStart)
		return nRet;
	return NO_STD;
}

//��̬��׼��
double CCalc::nors(double dValue)
{
	int nSize = m_vValue.size();
	if (nSize != 2 || m_vValue.at(1) == 0)
		return dValue;
	dValue = (dValue - m_vValue.at(0))/m_vValue.at(1);
	return dValue;
}

//�����׼��
double CCalc::gens(double dValue)
{	
	int nSize = m_vValue.size();
	if (nSize != 2 || m_vValue.at(0) == m_vValue.at(1))
		return dValue;
	dValue = (dValue - m_vValue.at(0))/(m_vValue.at(1) - m_vValue.at(0));
	return dValue;
}
