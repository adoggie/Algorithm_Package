// AnaWord.h: interface for the AnaWord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANAWORD_H__BD2EA71C_0A62_4DCC_8E12_D32D10172653__INCLUDED_)
#define AFX_ANAWORD_H__BD2EA71C_0A62_4DCC_8E12_D32D10172653__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4786)
#include <string>
#include <vector>
using namespace std ;

/*
 * ������ÿһ�����ʣ�˫�����ڱ�ʾһ������ \��ʾת����� \\ ��ʾһ��  ת�����\ֻ����˫���Ų���Ч
 * �����ַ�Ҳ������Ϊһ�������� ; , = ( ) [ ] { } < >
 */
class AnaWord  
{
	vector<string> m_strAry;
public:
	void Print();
	string Export();
	void AddWord(string strWord);
	void Clear();
	static string GetUniquely(string strWord);
	string GetAt(int nIndex);
	int GetWordCount();
	bool Import(string strScript);
	bool ImportWithOption(string strScript);
	AnaWord();
	virtual ~AnaWord();

};

#endif // !defined(AFX_ANAWORD_H__BD2EA71C_0A62_4DCC_8E12_D32D10172653__INCLUDED_)
