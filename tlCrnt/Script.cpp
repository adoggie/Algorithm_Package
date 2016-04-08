// Script.cpp: implementation of the CScript class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Script.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScript::CScript()
{

}

CScript::~CScript()
{
	Release(&m_tempArray);
}

//得到主名称,如频率分析
CTString CScript::GetMainName()
{
	return m_strName;
}

//导入描述文本,第一为主名称 各参数间用\分隔  参数中各项用空格分隔
bool CScript::ImportStr(CTString strDesc)
{
	//****************替换掉\r\n**************************
	CTString strStreamNew ;
	int nLen1 = strDesc.GetLength();
	BYTE *pB = (BYTE *)strStreamNew.GetBuffer(nLen1+sizeof(TCHAR));
	BYTE *pDest = (BYTE *)(char*)(const char*)strDesc;
	int i;
	for (i=0;i<nLen1; i++)
	{
		if (*(pDest+i) == '\r' ||*(pDest+i) == '\n')
		{
			*pB = ' ';//替换为空格
			pB++;
		}
		else
		{
			*pB = *(pDest+i);
			pB++;
		}
	}
	*pB = '\0';pB++;*pB = '\0';
	strStreamNew.ReleaseBuffer();
	//****************去掉\r 输出strStreamNew**************************

	m_strName = "";
	int nLast = -1;
	for (i=0; i<strStreamNew.GetLength() ;i++)
	{
		if (strStreamNew.GetAt(i)=='/')
		{
			CTString str = strStreamNew.Mid(nLast+1,i-nLast-1);
			str.TrimLeft();
			str.TrimRight();
			if (nLast ==-1)
				m_strName = str;
			else
				m_strArray.Add(str);
			nLast = i;
		}
	}
	CTString str = strStreamNew.Mid(nLast+1,strStreamNew.GetLength() + 1 -nLast);
	str.TrimLeft();
	str.TrimRight();
	if (nLast ==-1)
		m_strName = str;
	else
		m_strArray.Add(str);
	return true;
}
//得到描述的个数
int CScript::GetParamCount()
{
	return m_strArray.GetSize();
}
//得到nIndex索引号对应的描述,从零开始
CTString CScript::GetParamName(int nIndex)
{
	return GetName(m_strArray.GetAt(nIndex));
}

//根据参数名称找到对应所在的序号,从零开始,如果没有找到则返回-1,
int CScript::FindParamName(CTString strParamName)
{
	for (int i=0;i<m_strArray.GetSize();i++)
	{
		CTString strEach = GetName(m_strArray.GetAt(i));
		if (strEach==strParamName)
			return i;
	}
	return -1;
}
//根据参数名称找到对应行描述
CTString CScript::GetLineByParamName(CTString strParamName)
{
	int nPos = FindParamName(strParamName);
	if (nPos ==-1)
		return "";
	return m_strArray.GetAt(nPos);
}
//得到参数对应的项的个数 用空格作为区分
//如变量表=年龄 体重 身高    3  失败-1
int CScript::GetItemCount(CTString strParamName)
{
	CTString strLine = GetLineByParamName(strParamName);
	GetItem(strLine,m_tempArray);

	return m_tempArray.GetSize();
}

//得到参数对应的某个项的内容,以字符串形式取出,nPos从零开始,不成功则返回false
bool CScript::GetItemData(CTString strParamName, int nPos,CTString &Value)
{
	CTString strLine = GetLineByParamName(strParamName);
	GetItem(strLine,m_tempArray);
	if (nPos<0 || nPos>=m_tempArray.GetSize())
		return false;

	CTString strItem = GetItemStr(strLine);
	sItem *psItem = (sItem *)m_tempArray.GetAt(nPos);

	Value = strItem.Mid(psItem->nFirstPos, psItem->nLastPos - psItem->nFirstPos + 1) ;
	return true;
}

//得到参数对应的某个项的内容,以整形形式取出,nPos从零开始,不成功则返回false
bool CScript::GetItemData(CTString strParamName, int nPos,int &value)
{
	CTString strOut;
	if (GetItemData(strParamName,nPos,strOut) == false )
		return false;
	value = atoi(strOut);
	return true;
}

//得到参数对应的某个项的内容,以整形形式取出,nPos从零开始,不成功则返回false
bool CScript::GetItemData(CTString strParamName, int nPos, double &value)
{
	CTString strOut;
	if (GetItemData(strParamName,nPos,strOut) == false )
		return false;
	value = atof(strOut);
	return true;
}

//得到参数对应的子项中是否存在Value,存在则返回true
bool CScript::ExistItemData(CTString strParamName, CTString Value)
{
	int nCount =  GetItemCount(strParamName);
	for (int i=0; i<nCount ;i++)
	{
		CTString strOut;
		if (GetItemData(strParamName, i ,strOut) == false )
			return false;
		if (strOut == Value)
			return true;
	}
	return false;
}

CTString CScript::GetName(CTString strLine)
{
	int nPos = strLine.Find("=");
	if (nPos == -1)
		return "";
	strLine.TrimLeft();

	strLine = strLine.Mid(0,nPos);
	strLine.TrimLeft();
	strLine.TrimRight();
	return strLine;
}

void CScript::Test()
{
	CTString strAll =  "频率分析\
    /变量表=年龄 体重 身高\
    /中心趋势=均值 中值 众数\
    /分布 = 偏度";
	ImportStr(strAll);
	CTString strParamName = "中心趋势";
	//FindParamName(strParamName);
	int nPos = GetItemCount(strParamName);
	CTString strOut;
	GetItemData(strParamName,2,strOut);
}

//得到对应参数的所有项的描述字符串,返回字符串,已经没有前后的空格
CTString CScript::GetItemStr(CTString strLine)
{
	int nPos = strLine.Find("=");
	if (nPos == -1)
		return "";

	strLine = strLine.Right(strLine.GetLength()-nPos-1);
	strLine.TrimLeft();
	strLine.TrimRight();
	return strLine;
}

void CScript::Release(CPtrArray *pArr)
{
	int nSize = pArr->GetSize();
	for (int i=0;i<nSize;i++)
	{
		sItem *pszItem = (sItem *)pArr->GetAt(i);
		delete pszItem;
	}
	pArr->RemoveAll();
}
//得到对应参数的所有项的位置信息,相对于后面项的位置,而不是整行
bool CScript::GetItem(CTString strLine, CPtrArray &tempArray)
{
	Release(&tempArray);
	if (strLine.GetLength() == 0)
		return false;

	CTString strItem = GetItemStr(strLine);

	int nLen = strItem.GetLength();
	int nLastSpace = -1;
	sItem *psItem = NULL;
	for (int i=0; i<nLen ;i++)
	{
		if (strItem.GetAt(i) == ' ')
		{
			if (nLastSpace != i-1)
			{
				psItem = new sItem;//添加一个元素项
				psItem->nFirstPos = nLastSpace + 1;
				psItem->nLastPos = i-1;
				tempArray.Add(psItem);
			}
			nLastSpace = i;
		}
	}	
	//最后一个
	psItem = new sItem;
	psItem->nFirstPos = nLastSpace + 1;
	psItem->nLastPos = nLen - 1;
	tempArray.Add(psItem);
	return true;
}
