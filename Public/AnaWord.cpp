// AnaWord.cpp: implementation of the AnaWord class.
//
//////////////////////////////////////////////////////////////////////


#include "AnaWord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnaWord::AnaWord()
{

}

AnaWord::~AnaWord()
{

}

/*
* 根据原始的字符串，获得转化后，无二义性的字符，
* 如单词 my  得到 my
* 单词 my is word  得到 "my is word"
* 单词 my "is[word]  得到 "my \"is[word]"
*/
string AnaWord::GetUniquely(string strWord)
{
	bool bNeedConvert =false;
	int i=0;
	for (i=0;i<strWord.length();i++)
	{
		char cur = strWord[i];
		if (cur == ';' || cur == ',' || cur == '='
			|| cur == '[' || cur == ']' || cur == '<' || cur == '>'
			|| cur == '(' || cur == ')' || cur == '{' || cur == '}')
		{
			bNeedConvert = true;
			break;
		}
		if (cur == ' '|| cur == '"')
		{
			bNeedConvert = true;
			break;
		}
	}
	//不需要转化
	if (bNeedConvert ==false)
		return strWord;
	string sRst ;
	for (i=0;i<strWord.length();i++)
	{
		char cur = strWord[i];
		if (cur =='\\'|| cur =='"')
			sRst = sRst+"\\"+cur;
		else
			sRst = sRst+cur;
	}
	return "\""+sRst + "\"";
}


bool AnaWord::ImportWithOption(string strScript)
{

	m_strAry.clear();
	int nLen = strScript.length();
	bool bInBracket = false;
	string strBracket ;
	int nLeft = 0;// ===temp
	for (int i = 0; i < nLen; i++) {
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
				bInBracket = false;
				nLeft = i + 1;
			} else {
				strBracket = strBracket + cur;
			}
			
		} else if (cur == ' ') {// 空格
			if (i > nLeft) {// 可以分析出单词
				string str = strScript.substr(nLeft, i - nLeft);
				m_strAry.push_back(str);
			}
			nLeft = i + 1;
		} else if (cur == '"') {// 引号
			nLeft = i + 1;
			bInBracket = true;
			strBracket = "";
		} else if (cur == ';' || cur == ',' || cur == '='
			|| cur == '[' || cur == ']' || cur == '<' || cur == '>'
			|| cur == '(' || cur == ')' || cur == '{' || cur == '}'
			|| cur == '+' || cur == '-' || cur == '*')
 
		{// 引号
			if (i > nLeft) {// 可以分析出单词
				string str = strScript.substr(nLeft,i - nLeft);
				m_strAry.push_back(str);
			}
			string s ;	s= cur;
			m_strAry.push_back(s);
			nLeft = i + 1;
		}
		
	}
	// 结束后，如果还处于字符串内
	if (bInBracket) {
		m_strAry.clear();
		return false;
	}
	
	if (nLeft < nLen) {
		m_strAry.push_back(strScript.substr(nLeft,nLen - nLeft));
	}
	
	return true;
}

bool AnaWord::Import(string strScript)
{
	char *ch = (char*)strScript.c_str();
	while(*ch++){
		switch(*ch){
			case '\r':
			case '\n':
				*ch=' ';
		}
	}
	m_strAry.clear();
	int nLen = strScript.length();
	bool bInBracket = false;
	string strBracket ;
	int nLeft = 0;// ===temp
	for (int i = 0; i < nLen; i++) {
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
				bInBracket = false;
				nLeft = i + 1;
			} else {
				strBracket = strBracket + cur;
			}
			
		} else if (cur == ' ') {// 空格
			if (i > nLeft) {// 可以分析出单词
				string str = strScript.substr(nLeft, i - nLeft);
				m_strAry.push_back(str);
			}
			nLeft = i + 1;
		} else if (cur == '"') {// 引号
			nLeft = i + 1;
			bInBracket = true;
			strBracket = "";
		} else if (cur == ';' || cur == ',' || cur == '='
			|| cur == '[' || cur == ']' || cur == '<' || cur == '>'
			|| cur == '(' || cur == ')' || cur == '{' || cur == '}')
 
		{// 引号
			if (i > nLeft) {// 可以分析出单词
				string str = strScript.substr(nLeft,i - nLeft);
				m_strAry.push_back(str);
			}
			string s ;	s= cur;
			m_strAry.push_back(s);
			nLeft = i + 1;
		}
		
	}
	// 结束后，如果还处于字符串内
	if (bInBracket) {
		m_strAry.clear();
		return false;
	}
	
	if (nLeft < nLen) {
		m_strAry.push_back(strScript.substr(nLeft,nLen - nLeft));
	}
	
	return true;
}

int AnaWord::GetWordCount()
{
	return m_strAry.size();
}

string AnaWord::GetAt(int nIndex)
{
	if (nIndex<0 || nIndex>= m_strAry.size())
		return "";
	return m_strAry[nIndex];
}
	/*
	 * 清空所有的单词
	 */
void AnaWord::Clear()
{
	m_strAry.clear();
}
	/*
	 * 往单词库里添加一个单词
	 */
void AnaWord::AddWord(string strWord)
{
	m_strAry.push_back(strWord);
}
/*
* 将所有单词，经过二义性消除后，转化为一个字符串输出
*/
string AnaWord::Export()
{
	string strOut ;
	for (int i=0;i<m_strAry.size();i++)
	{
		strOut = strOut +" "+ AnaWord::GetUniquely(m_strAry[i]);
	}
	return strOut;
}

void AnaWord::Print()
{
#ifdef _DEBUG
	int nCnt = GetWordCount();
	printf("*********共个数%d\r\n",nCnt);
	for (int i=0;i<nCnt;i++)
	{
		printf("%s\r\n",GetAt(i).c_str());
	}
	printf("*********\r\n");
#endif
}
