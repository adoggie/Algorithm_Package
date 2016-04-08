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
���ݵ��ʲ�������λ�ã�
û���ҵ��򷵻�-1
�ҵ��򷵻�λ�ã����㿪ʼ
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
*���ҵ���strWord�Ժ�ĵ�nNextStep������
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
 *	�ӵ���������ҵ���sStart��ʼ����sEnd�������м����е��ʣ�sRemove���м����ƥ�䣬��Ҫ�����ڽ����
 *  GetIn(aw, "{","}", ",",&awNew)
 *  �ҵ� { }֮������е��ʣ���������  ������awNew��
 *  ���������ṹ���򷵻�true ���� ����false
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
		szDest = szArrDest.GetAt(i);//Ŀ�����
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
