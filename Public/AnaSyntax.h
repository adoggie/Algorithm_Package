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
	static bool findstr(string strRow, string strSign, AnaWord &aw);//����bool��������ʾ�Ƿ���aw��strRow��������ҵ���strSign��
	static double getvalue(string strRow, string strSign, AnaWord &aw);	//����һ��boulbe��������strSign��ߵ������ж�����������
	static double getvalue(string strRow, string strSign, AnaWord &aw, int k);	//����һ��boulbe��������strSign��ߵ������е�k�������������أ��м��ÿո�ֿ�
	static bool exist(string strWord, AnaWord &aw);	//����һ��bool��������ʾ�Ƿ��ҵ���strWord�����
	static void getVAR(string strRow, AnaWord &aw,CTStringArray &VarName);//	����һ���ַ������飬��/FUNCTION���ж�ȡ X1 X2 "X 3"����ʾ�ֱ���
	static CTString getSingleStr(string strRow, AnaWord &aw);//��/FUNCTION���ж�ȡ X1������
	static bool Replace(CTStringArray &szArrDest, const CTStringArray &szArrVar, const CTStringArray &szArrReplace); //�滻
	AnaSyntax();
	virtual ~AnaSyntax();

};

#endif // !defined(AFX_ANASYNTAX_H__A916FCA5_5631_4FA8_916D_CB8A1CFBB181__INCLUDED_)
