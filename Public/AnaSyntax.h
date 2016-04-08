// AnaSyntax.h: interface for the AnaSyntax class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANASYNTAX_H__A916FCA5_5631_4FA8_916D_CB8A1CFBB181__INCLUDED_)
#define AFX_ANASYNTAX_H__A916FCA5_5631_4FA8_916D_CB8A1CFBB181__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AnaWord.h"
#include "TString.h"

class AnaSyntax  
{
public:
	static bool GetMidWords(AnaWord &aw,string sStart,string sEnd,string sRemove,AnaWord &awNew);
	static string GetNextWord(string strWord,AnaWord &aw,int nNextStep);
	static int FindFirstWordPos(string strWord,AnaWord &aw);
	static bool findstr(string strRow, string strSign, AnaWord &aw);//返回bool变量，表示是否在aw的strRow语句行中找到了strSign。
	static double getvalue(string strRow, string strSign, AnaWord &aw);	//返回一个boulbe变量，从strSign后边的括号中读数，并返回
	static double getvalue(string strRow, string strSign, AnaWord &aw, int k);	//返回一个boulbe变量，从strSign后边的括号中的k个读数，并返回，中间用空格分开
	static bool exist(string strWord, AnaWord &aw);	//返回一个bool变量，表示是否找到了strWord语句行
	static void getVAR(string strRow, AnaWord &aw,CTStringArray &VarName);//	返回一个字符串数组，从/FUNCTION行中读取 X1 X2 "X 3"，表示字变量
	static CTString getSingleStr(string strRow, AnaWord &aw);//从/FUNCTION行中读取 X1并返回
	static bool Replace(CTStringArray &szArrDest, const CTStringArray &szArrVar, const CTStringArray &szArrReplace); //替换
	AnaSyntax();
	virtual ~AnaSyntax();

};

#endif // !defined(AFX_ANASYNTAX_H__A916FCA5_5631_4FA8_916D_CB8A1CFBB181__INCLUDED_)
