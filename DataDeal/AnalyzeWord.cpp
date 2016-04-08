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
 * 导入命令语句，分解为单词
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
					// 采用宽松的解释方式，如果是单个\ 后面不包含\或者"，它自己仍作为一个字符
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
		
		} else if (cur == ' ') {// 空格
			DealPre(nLeft,i,strScript);
			nLeft = i + 1;
		} else if (cur == '"') {// 引号
			nLeft = i + 1;
			bInBracket = true;
			strBracket = "";
		} else if (cur == '<' || cur == '>' || cur == '~')
		{//
			DealPre(nLeft,i,strScript);
			char curNext = strScript[i+1];
			if (curNext == '=')//判断下面一个是否是=
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
		{//字段名
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
	// 结束后，如果还处于字符串内
	if (bInBracket) {
		m_strAry.clear();
		return false;
	}
	
	if (nLeft < nLen) {
		DealPre(nLeft,nLen,strScript);
	}

	//识别符号位
	int nSize = m_strAry.size();
	for ( i=nSize-1;i>=0;i--)
	{
		if (m_TypeAry[i]==itFieldName || 
			m_TypeAry[i]==itFuncName  || m_TypeAry[i]==itNum  )
		{
			if (i-1 >= 0 && m_TypeAry[i-1]==itOperation && m_strAry[i-1] == "-")
			{//前面一个存在，并且是"-"
				if (i-2<0 || m_TypeAry[i-2]==itOperation || m_TypeAry[i-2]==itComma 
					|| m_strAry[i-2] == "(")
				{//前面二个
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
 * 得到单词个数
 */
int CAnalyzeWord::GetWordCount()const
{
	return m_strAry.size();
}

/*
 * 在指定位置得到单词
 */
std::string CAnalyzeWord::GetAt(int nIndex)const
{
	if (nIndex<0 || nIndex>= m_strAry.size())
		return "";
	return m_strAry[nIndex];
}
/*
* 清空所有的单词
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
	printf("*********共个数%d\r\n",nCnt);
	for (int i=0;i<nCnt;i++)
	{
		printf("%s\t\t:%s\r\n",ItemTypeName1[m_TypeAry[i]],GetAt(i).c_str());
	}
	printf("*********\r\n");
#endif
}

/*
 * 处理当前已经分析出的单词
 */
void CAnalyzeWord::DealPre(int nLeft, int nCurPos, const std::string &strScript)
{
	if (nCurPos > nLeft) {// 可以分析出单词
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
