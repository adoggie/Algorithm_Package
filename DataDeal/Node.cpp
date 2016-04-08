// Node.cpp: implementation of the CNode class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Node.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char* ItemTypeName1[];

/*
 * ������ÿһ�����ʣ�˫�����ڱ�ʾһ������ \��ʾת����� \\ ��ʾһ��  ת�����\ֻ����˫���Ų���Ч
opAnd,opOr,opNot,opG,opGE,opL,opLE,opEQ,opNE,opPlus,opMinus,opMultiply,opDivide,opMod
������������������������������������������������
�߼�     &  |   ~
��ϵ     >  >=  <  <=  =  ~=
����     +  -   *  /   % 
*/
CNode::~CNode()
{
	for (int i=0;i<m_Child.size();i++)
	{
		delete m_Child[i];
	}
}


CNode::CNode(ItemType it,const std::string &strValue)
{
	m_nodeType = it ;
	m_strNodeValue = strValue;

}


/*
 * ��ӡ����ڵ���Ϣ
 */

void CNode::print(int nLevel)
{
#ifdef _DEBUG
	std::string strPre= "";
	for (int j=0;j<nLevel;j++)
		strPre = strPre+"  ";
	if (m_strNodeValue.size())
		printf("%s%s%s\r\n", ItemTypeName1[m_nodeType],strPre.c_str(),m_strNodeValue.c_str());
	else
		printf("%s%s?\r\n", ItemTypeName1[m_nodeType],strPre.c_str());


	for (int i=0;i<m_Child.size();i++)
	{
		ItemType it = m_Child[i]->GetNodeType();
		if (it==itNode)
		{
			m_Child[i]->print(nLevel+1);
		}
		else if (it == itFuncName)
		{
			m_Child[i]->print(nLevel+1);
		}
		else
		{
			m_Child[i]->print(nLevel+1);

		}
	}
#endif
}

// ��aw��nFrom, nTo����Χ�ڴʽ�������Ҷ��
bool CNode::BuildTree(const CAnalyzeWord &aw, int nFrom, int nTo)
{
	int i = nFrom;
	while (i <= nTo )
	{
		ItemType it = aw.GetTypeAt(i);
		if (it==itFuncName)
		{
			if (aw.GetAt(i+1)!="(")
				return false;

			int nLeft = i+2;
			int nPos = FindNestedBracketPos(aw,nLeft);
			if (nPos ==-1)
				return false;
			
			CNode *pNode = new CNode(it,aw.GetAt(i));
			m_Child.push_back(pNode);
			std::vector<CNode*>&nodeChildAry = pNode->GetChild();


			int nCurCommaPos = 0;
			while (true)
			{
				nCurCommaPos = FindNestedOneOf(aw,",)",nLeft);
				if (nCurCommaPos==-1 || nCurCommaPos >nPos )
					break;

				CNode *pChildNode = new CNode(itNode,"");
				if (pChildNode->BuildTree(aw,nLeft, nCurCommaPos-1)==false)
					return false;
				nodeChildAry.push_back(pChildNode);
				nLeft = nCurCommaPos +1;
			}

			i= nPos+1; //i Ϊ ) ����һ��
		}
		else if (it==itBracket)
		{
			if (aw.GetAt(i)==")")
				return false;
			
			int nPos = FindNestedBracketPos(aw,i+1);
			if (nPos ==-1)
				return false;
			CNode *pNode = new CNode(itNode,"");
			m_Child.push_back(pNode);
			if (pNode->BuildTree(aw,i+1, nPos-1)==false)
				return false;
			i= nPos+1;
		}
		else
		{
			CNode *pNode = new CNode(it,aw.GetAt(i));
			m_Child.push_back(pNode);
			i++;
		}		
	}

	//return BuildBinaryTree();
	return true;
}
/*                              ( ����         ( )   )
 * Ƕ�ײ��ң����Ӽ�()���治����   nFromPos           pos
 */
int CNode::FindNestedBracketPos(const CAnalyzeWord &aw, int nFromPos)
{
	if (nFromPos>= aw.GetWordCount())
		return -1;
	int nCurLevel = 0;
	for (int i=nFromPos;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i) == "(")
			nCurLevel++;
		if (aw.GetAt(i) == ")")
		{
			if (nCurLevel ==0)
				return i;
			nCurLevel--;
		}
	}

	return -1;
}

/*                              ( ����         ( strWord)   strWord
 * Ƕ�ײ��ң����Ӽ�()���治����   nFromPos                  pos
 */
int CNode::FindNestedOneOf(const CAnalyzeWord &aw, std::string strWord, int nFromPos)
{
	if (nFromPos>= aw.GetWordCount())
		return -1;
	int nSize = strWord.size();
	int nCurLevel = 0;
	for (int i=nFromPos;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i) == "(")
			nCurLevel++;
		for (int j=0;j<nSize;j++)
		{
			std::string ss;
			ss.insert(ss.begin(),strWord[j]);
			if (nCurLevel ==0 && aw.GetAt(i) == ss)
				return i;
		}
		if (aw.GetAt(i) == ")")
		{
			nCurLevel--;
		}
	}

	return -1;
}
/*
������������������������������������������������
�߼�     &  |   ~
��ϵ     >  >=  <  <=  =  ~=
����     +  -   *  /   % 

������
���� , ( ) 

0		&  |   
1		>  >=  <  <=  =  ~=
2		+  -
3		*  /  % 
4		~
5		()
*/
int CNode::GetPrivilege(std::string &sOp)
{
	if (sOp == "&" || sOp == "|"  )
		return 0;
	if (sOp == ">" || sOp == ">=" || sOp == "<" || sOp == "<=" || sOp == "=" || sOp == "~=" )
		return 1;
	if (sOp == "+" || sOp == "-"  )
		return 2;
	if (sOp == "*" || sOp == "/"  || sOp == "%" )
		return 3;
	if (sOp == "~" )
		return 4;
	if (sOp == "(" || sOp == ")"  )
		return 5;
}


/*
 * �õ���һ������߿�ʼ���ȼ�����һ���������λ�á�
 * itOperation��Ӧ��û���ӽڵ㡣
 */
int CNode::GetLeftMaxOp(std::vector<CNode*>&copyAry)
{
	int nCnt = 0;
	int nLeft = -1;
	int nLastPos = -1;
	for (int i=0;i<copyAry.size();i++)
	{
		CNode *pNode = copyAry[i];
		if (copyAry[i]->GetNodeType() == itOperation && copyAry[i]->m_Child.size() ==0)//û���ӽڵ�
		{
			std::string stmp = copyAry[i]->GetNodeValue();
			int nCurPri = GetPrivilege(stmp);
			if (nCurPri>nLeft)
			{
				nLastPos = i;
				nLeft = nCurPri;
			}
			else
			{
				if (nLastPos !=-1)
					return nLastPos;
			}

		}
	}
	return nLastPos;

}

/*
 * ����ǰ����ת��Ϊ������
 */
bool CNode::BuildBinaryTree()
{
	//�ӽڵ�Ľ���
	for (int i=0;i<m_Child.size();i++)
		if (m_Child[i]->BuildBinaryTree()==false)
			return false;
	
	//CNode 
	if (m_nodeType == itFuncName)
	{
	}
	else
	{
		std::vector<CNode*>&copyAry = m_Child;

		//check syntax
		if (CheckChild()==false)
		{
			return false;//Incorrect syntax 
		}

		while(copyAry.size()>1)
		{
			int nPos = GetLeftMaxOp(copyAry);
			if (nPos ==-1)
				return false;
			/*
			printf("\r\n");
			for (int i=0;i<copyAry.size();i++)
			{
				printf("%s\t",ItemTypeName1[copyAry[i]->GetNodeType()]);
			}
			printf("\r\n");
			*/
			if (copyAry[nPos]->GetNodeValue() == "~")
			{//��Ŀ�����
				if (copyAry.size() < nPos+1)
					return false;
				
				copyAry[nPos]->GetChild().push_back(copyAry[nPos+1]);
				copyAry.erase(copyAry.begin()+nPos+1);//ɾ�������
			}
			else
			{
				if (copyAry.size() < nPos+2)
					return false;
				
				copyAry[nPos]->GetChild().push_back(copyAry[nPos-1]);
				copyAry[nPos]->GetChild().push_back(copyAry[nPos+1]);
				copyAry[nPos-1] = copyAry[nPos];//���ǵ�ǰ��
				copyAry.erase(copyAry.begin()+nPos,copyAry.begin()+nPos+2);//ɾ�����������
			}

		}
	
	}


	return true;
}
/*
 * ��������ڵ�
 */
void CNode::BackTrace(std::vector<CNode*>&stk)
{
	std::vector<CNode*>::iterator it ;
	for (it = m_Child.begin();it!=m_Child.end();it++)
	{
		(*it)->BackTrace(stk);
	}
	stk.push_back(this);	
}

/*
 * ��pNode�ڵ㸴����ص���Ϣ�����ڵ�
 */
void RItem::copyfrom(CNode*pNode)
{
	m_nodeType = pNode->GetNodeType();
	strValue = pNode->GetNodeValue();
	if (m_nodeType == itOperation)
	{
		if (strValue == "&")
			m_ot = opAnd;
		else if (strValue == "|")
			m_ot = opOr;
		else if (strValue == "~")
			m_ot = opNot;
		else if (strValue == ">")
			m_ot = opG;
		else if (strValue == ">=")
			m_ot = opGE;
		else if (strValue == "<")
			m_ot = opL;
		else if (strValue == "<=")
			m_ot = opLE;
		else if (strValue == "=")
			m_ot = opEQ;
		else if (strValue == "~=")
			m_ot = opNE;
		else if (strValue == "+")
			m_ot = opPlus;
		else if (strValue == "-")
			m_ot = opMinus;
		else if (strValue == "*")
			m_ot = opMultiply;
		else if (strValue == "/")
			m_ot = opDivide;
		else if (strValue == "%")
			m_ot = opMod;
		else
		{
		}
	}
	else if (m_nodeType ==itNum)
	{
		dbvalue = atof(strValue.c_str());

	}
	else if (m_nodeType == itFieldName)
	{
	}
}
/*
 * ����ӽڵ�Ľṹ�Ƿ���ȷ�������ڶ������׶�
 */
bool CNode::CheckChild()
{
	if (m_nodeType == itFuncName)
	{//����ڵ��Ǻ����ڵ㣬���麯������
		return true;
	}

	
	int nSize = m_Child.size();
	bool curState = false;
	for (int i=0;i<nSize;i++)
	{
		ItemType it = m_Child[i]->GetNodeType();
		if (it== itOperation && m_Child[i]->m_strNodeValue == "~")
		{//��Ŀ�������
			if (i==nSize-1)
				return false;
			ItemType itNext = m_Child[i+1]->GetNodeType();
			if (itNext ==itOperation || itNext ==itString)
				return false;
			curState  = !curState;
			i++;
			continue;
		}

		//����Ϊ˫Ŀ�����
		if (i%2 == curState)
		{
			if (it ==itOperation/* || it ==itString*/)
				return false;
		
		}
		else
		{
			if (it !=itOperation)
				return false;
		}
	}
	return true;

}
