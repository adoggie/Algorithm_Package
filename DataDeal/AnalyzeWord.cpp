// AnalyzeWord.cpp: implementation of the CAnalyzeWord class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AnalyzeWord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnalyzeWord::CAnalyzeWord()
{

}

CAnalyzeWord::~CAnalyzeWord()
{

}

bool CAnalyzeWord::IsNumber(std::string& strItem)
{
   const char   *string1;char *stopstring;
   string1 = strItem.c_str();
   double x = strtod( string1, &stopstring );
   if (stopstring== string1 )
	   return false;
   else
	   return true;
}

/*
 * ����������䣬�ֽ�Ϊ����
 */
bool CAnalyzeWord::Import(std::string strScript)
{
	m_strAry.clear();
	m_TypeAry.clear();
	int nLen = strScript.length();
	bool bInBracket = false;
	std::string strBracket ;
	int nLeft = 0;// ===temp
	int i = 0; 
	for (i = 0; i < nLen; i++) {
		char cur = strScript[i];
		if (bInBracket) {
			if (cur == '\\') {
				if (strScript[i + 1] == '\\'
					|| strScript[i + 1] == '"') {
					strBracket = strBracket
						+ strScript[i + 1];
					i++;
				} else
					// ���ÿ��ɵĽ��ͷ�ʽ������ǵ���\ ���治����\����"�����Լ�����Ϊһ���ַ�
					strBracket = strBracket
					+ strScript[i];
			} else if (cur == '"') {
				m_strAry.push_back(strBracket);
				m_TypeAry.push_back(itString);
				bInBracket = false;
				nLeft = i + 1;
			} else {
				strBracket = strBracket + cur;
			}
		
		} else if (cur == ' ') {// �ո�
			DealPre(nLeft,i,strScript);
			nLeft = i + 1;
		} else if (cur == '"') {// ����
			nLeft = i + 1;
			bInBracket = true;
			strBracket = "";
		} else if (cur == '<' || cur == '>' || cur == '~')
		{//
			DealPre(nLeft,i,strScript);
			char curNext = strScript[i+1];
			if (curNext == '=')//�ж�����һ���Ƿ���=
			{
				std::string s ;	s= cur;
				s= s+curNext;
				m_strAry.push_back(s);
				m_TypeAry.push_back(itOperation);
				nLeft = i + 2;
				i++;
			}
			else
			{
				std::string s ;	s= cur;
				m_strAry.push_back(s);
				m_TypeAry.push_back(itOperation);
				nLeft = i + 1;
			}

		} else if (cur == '&' || cur == '|' || cur == '~'
			|| cur == '='
			|| cur == '+'|| cur == '-'|| cur == '*'|| cur == '/' || cur == '%'
			 )
 
		{//
			DealPre(nLeft,i,strScript);

			std::string s ;	s= cur;
			m_strAry.push_back(s);
			m_TypeAry.push_back(itOperation);
			nLeft = i + 1;
		}
		else if (cur == ',')
		{
			DealPre(nLeft,i,strScript);
			std::string s ;	s= cur;
			m_strAry.push_back(s);
			m_TypeAry.push_back(itComma);
			nLeft = i + 1;
		}
		else if (cur == '(' || cur == ')')
		{
			DealPre(nLeft,i,strScript);
			std::string s ;	s= cur;
			m_strAry.push_back(s);
			m_TypeAry.push_back(itBracket);
			nLeft = i + 1;
		}
		else if ( cur == '[' )
		{//�ֶ���
			DealPre(nLeft,i,strScript);
			int nPos = i+1;
			int nNextPos = strScript.find(']',nPos);
			if (nNextPos ==-1)
				return false;
			std::string strMid = strScript.substr(i+1,nNextPos-1 - nPos+1);
			m_strAry.push_back(strMid);
			m_TypeAry.push_back(itFieldName);
			i= nNextPos;
			nLeft = i + 1;
		}
		
	}
	// ����������������ַ�����
	if (bInBracket) {
		m_strAry.clear();
		return false;
	}
	
	if (nLeft < nLen) {
		DealPre(nLeft,nLen,strScript);
	}

	//ʶ�����λ
	int nSize = m_strAry.size();
	for ( i=nSize-1;i>=0;i--)
	{
		if (m_TypeAry[i]==itFieldName || 
			m_TypeAry[i]==itFuncName  || m_TypeAry[i]==itNum  )
		{
			if (i-1 >= 0 && m_TypeAry[i-1]==itOperation && m_strAry[i-1] == "-")
			{//ǰ��һ�����ڣ�������"-"
				if (i-2<0 || m_TypeAry[i-2]==itOperation || m_TypeAry[i-2]==itComma 
					|| m_strAry[i-2] == "(")
				{//ǰ�����
					m_strAry[i]= "-"+m_strAry[i];
					
					m_strAry.erase(m_strAry.begin()+i-1);
					m_TypeAry.erase(m_TypeAry.begin()+i-1);
					i--;
				}
			}

		}
	}


	return true;
}

/*
 * �õ����ʸ���
 */
int CAnalyzeWord::GetWordCount()const
{
	return m_strAry.size();
}

/*
 * ��ָ��λ�õõ�����
 */
std::string CAnalyzeWord::GetAt(int nIndex)const
{
	if (nIndex<0 || nIndex>= m_strAry.size())
		return "";
	return m_strAry[nIndex];
}
/*
* ������еĵ���
*/
void CAnalyzeWord::Clear()
{
	m_strAry.clear();
	m_TypeAry.clear();
}

char* ItemTypeName1[]= {
"itFieldName",
"itFuncName ",
"itString   ",
"itNum      ",
"itBracket  ",
"itComma    ",
"itOperation",
"itNode     "
};

void CAnalyzeWord::Print()
{
#ifdef _DEBUG
	int nCnt = GetWordCount();
	printf("*********������%d\r\n",nCnt);
	for (int i=0;i<nCnt;i++)
	{
		printf("%s\t\t:%s\r\n",ItemTypeName1[m_TypeAry[i]],GetAt(i).c_str());
	}
	printf("*********\r\n");
#endif
}

/*
 * ����ǰ�Ѿ��������ĵ���
 */
void CAnalyzeWord::DealPre(int nLeft, int nCurPos, const std::string &strScript)
{
	if (nCurPos > nLeft) {// ���Է���������
		std::string str = strScript.substr(nLeft,nCurPos - nLeft);
		m_strAry.push_back(str);
		if (IsNumber(str))
			m_TypeAry.push_back(itNum);
		else
			m_TypeAry.push_back(itFuncName);
	}
	
}

ItemType CAnalyzeWord::GetTypeAt(int nIndex)const
{
	return m_TypeAry[nIndex];
}
