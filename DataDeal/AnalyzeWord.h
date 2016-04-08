// AnalyzeWord.h: interface for the CAnalyzeWord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYZEWORD_H__04C5B7BE_05F5_493E_A18B_8BD9FCB731EB__INCLUDED_)
#define AFX_ANALYZEWORD_H__04C5B7BE_05F5_493E_A18B_8BD9FCB731EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include <string>
//using namespace std::string;
//using namespace std::vector;

enum ItemType{
itFieldName,itFuncName,itString,itNum,itBracket,itComma,itOperation,itNode
};

enum OperationType{
opAnd,opOr,opNot,opG,opGE,opL,opLE,opEQ,opNE,opPlus,opMinus,opMultiply,opDivide,opMod
};
//char* ItemTypeName1[];
/*
 * ������ÿһ�����ʣ�˫�����ڱ�ʾһ������ \��ʾת����� \\ ��ʾһ��  ת�����\ֻ����˫���Ų���Ч
������������������������������������������������
�߼�     &  |   ~
��ϵ     >  >=  <  <=  =  ~=
����     +  -   *  /   % 

������
���� , ( ) 
[�ֶ���]
������������������������������������������������
 */
class CAnalyzeWord  
{
	std::vector<std::string> m_strAry;
	std::vector<ItemType>m_TypeAry;
	bool IsNumber(std::string& strItem);
public:
	void Print();
	void Clear();
	std::string GetAt(int nIndex)const;
	int GetWordCount()const;
	bool Import(std::string strScript);

public:
	ItemType GetTypeAt(int nIndex)const;
	CAnalyzeWord();
	virtual ~CAnalyzeWord();

protected:
	void DealPre(int nLeft,int nCurPos,const std::string&strScript);
};

#endif // !defined(AFX_ANALYZEWORD_H__04C5B7BE_05F5_493E_A18B_8BD9FCB731EB__INCLUDED_)
