// AnaSyntax.cpp: implementation of the AnaSyntax class.
//
//////////////////////////////////////////////////////////////////////


#include "AnaSyntax.h"
#include "TString.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnaSyntax::AnaSyntax()
{

}

AnaSyntax::~AnaSyntax()
{

}

/*
根据单词查找所在位置，
没有找到则返回-1
找到则返回位置，从零开始
*/
int AnaSyntax::FindFirstWordPos(string strWord,AnaWord &aw)
{
	int nCnt = aw.GetWordCount();
	for (int i=0;i<nCnt;i++)
	{
		if (aw.GetAt(i) == strWord)
			return i;
	}
	return -1;

}

/*
*查找单词strWord以后的第nNextStep个单词
*/
string AnaSyntax::GetNextWord(string strWord, AnaWord &aw,int nNextStep/* =1 */)
{
	int nPos = FindFirstWordPos(strWord,aw);
	if (nPos ==-1)
	{
		return "";
	}
	return aw.GetAt(nPos+nNextStep);
}

bool AnaSyntax::findstr(string strRow, string strSign, AnaWord &aw)
{
	int nPos = FindFirstWordPos(strRow,aw);
	if (nPos ==-1)
		return FALSE;
	for (int i=nPos+1;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i)[0] == '/') 
			return FALSE;
		if (aw.GetAt(i) == strSign) 
			return TRUE;
	}
	return FALSE;
}

double AnaSyntax::getvalue(string strRow, string strSign, AnaWord &aw)
{
	double avalue = -1;
	int nPos = FindFirstWordPos(strRow,aw);
	if (nPos ==-1)
		return -1;
	for (int i=nPos+1;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i)[0] == '/') 
			return avalue;
		if (aw.GetAt(i) == strSign) 
			avalue = atof(aw.GetAt(i+2).c_str());
	}
	return avalue;
}

double AnaSyntax::getvalue(string strRow, string strSign, AnaWord &aw, int k)
{
	double avalue = -1;
	int nPos = FindFirstWordPos(strRow,aw);
	if (nPos ==-1)
		return -1;
	for (int i=nPos+1;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i)[0] == '/') 
			return -1;
		if (aw.GetAt(i) == strSign) 
		{
			if (i+1+k >= aw.GetWordCount() )
				return -1;
			avalue = atof(aw.GetAt(i+1+k).c_str());
			if (avalue > -EPS_ACCUR && avalue < EPS_ACCUR)
			{
				if (aw.GetAt(i+1+k) != "0" && aw.GetAt(i+1+k) != "0.0")
					return -1;
			}
			return avalue;
		}
	}
	return -1;
}

bool AnaSyntax::exist(string strWord, AnaWord &aw)
{
	if (FindFirstWordPos(strWord,aw) == -1) 
		return FALSE;
	else
		return TRUE;
}

CTString AnaSyntax::getSingleStr(string strRow, AnaWord &aw)
{
	CTString StrName = "";
	int npos = FindFirstWordPos(strRow,aw);
	if (npos ==-1 || npos+1 >= aw.GetWordCount())
		return StrName;
	StrName = aw.GetAt(npos+1);
	return StrName;
}

void AnaSyntax::getVAR(string strRow, AnaWord &aw,CTStringArray &VarName)
{
	VarName.RemoveAll();
	int nPos = FindFirstWordPos(strRow,aw);
	if (nPos ==-1)
		return;
	for (int i=nPos+1;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i)[0] == '/') 
			return;
		CTString str = aw.GetAt(i);
		VarName.Add(str);
	}
	return;
}
/*
 *	从单词组里查找单词sStart开始，到sEnd结束的中间所有单词，sRemove是中间如果匹配，则不要出现在结果中
 *  GetIn(aw, "{","}", ",",&awNew)
 *  找到 { }之间的所有单词，除逗号外  ，放入awNew中
 *  如果有这个结构，则返回true 否则 返回false
 */
bool AnaSyntax::GetMidWords(AnaWord &aw, string sStart, string sEnd, string sRemove, AnaWord &awNew)
{
	int nLeft = -1;
	int nRight = -1;
	int nCnt = aw.GetWordCount();
	aw.Print();
	int i=0;
	for (i=0;i<nCnt;i++)
	{
		if (nLeft ==-1 && aw.GetAt(i)==sStart)
		{
			nLeft = i;
			continue;
		}
		if (nRight ==-1 &&aw.GetAt(i)==sEnd)
			nRight = i;
	}
	if (nLeft<0 || nRight<0 || nLeft>nRight)
		return false;
	for (i = nLeft+1;i<nRight;i++)
	{
		if (aw.GetAt(i)!=sRemove || sRemove.size()==0)
		{
			awNew.AddWord(aw.GetAt(i));
		}
	}
	return true;
}

bool AnaSyntax::Replace(CTStringArray &szArrDest, const CTStringArray &szArrVar, const CTStringArray &szArrReplace)
{
	int i=0, j=0;
	bool bPaired = false;
	CTString szDest("");
	int nSize = szArrReplace.GetSize();
	if (nSize % 2 != 0)
		return false;
	for (i=0; i<szArrDest.GetSize(); i++)
	{
		bPaired = false;
		szDest = szArrDest.GetAt(i);//目标变量
		for (j=0; j<szArrVar.GetSize(); j++)
		{
			if (szDest == szArrVar.GetAt(j))
			{
				bPaired = true;
				break;
			}
		}
		if (!bPaired)
		{
			for (j=0; j<szArrReplace.GetSize()/2; j++)
			{
				if (szDest == szArrReplace.GetAt(j*2))
				{
					szArrDest.SetAt(i, szArrReplace.GetAt(j*2+1));
					bPaired = true;
					break;
				}
			}
		}
		if (!bPaired)
			return false;
	}
	return true;
}
