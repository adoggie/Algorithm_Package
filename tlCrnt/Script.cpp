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

//�õ�������,��Ƶ�ʷ���
CTString CScript::GetMainName()
{
	return m_strName;
}

//���������ı�,��һΪ������ ����������\�ָ�  �����и����ÿո�ָ�
bool CScript::ImportStr(CTString strDesc)
{
	//****************�滻��\r\n**************************
	CTString strStreamNew ;
	int nLen1 = strDesc.GetLength();
	BYTE *pB = (BYTE *)strStreamNew.GetBuffer(nLen1+sizeof(TCHAR));
	BYTE *pDest = (BYTE *)(char*)(const char*)strDesc;
	int i;
	for (i=0;i<nLen1; i++)
	{
		if (*(pDest+i) == '\r' ||*(pDest+i) == '\n')
		{
			*pB = ' ';//�滻Ϊ�ո�
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
	//****************ȥ��\r ���strStreamNew**************************

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
//�õ������ĸ���
int CScript::GetParamCount()
{
	return m_strArray.GetSize();
}
//�õ�nIndex�����Ŷ�Ӧ������,���㿪ʼ
CTString CScript::GetParamName(int nIndex)
{
	return GetName(m_strArray.GetAt(nIndex));
}

//���ݲ��������ҵ���Ӧ���ڵ����,���㿪ʼ,���û���ҵ��򷵻�-1,
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
//���ݲ��������ҵ���Ӧ������
CTString CScript::GetLineByParamName(CTString strParamName)
{
	int nPos = FindParamName(strParamName);
	if (nPos ==-1)
		return "";
	return m_strArray.GetAt(nPos);
}
//�õ�������Ӧ����ĸ��� �ÿո���Ϊ����
//�������=���� ���� ���    3  ʧ��-1
int CScript::GetItemCount(CTString strParamName)
{
	CTString strLine = GetLineByParamName(strParamName);
	GetItem(strLine,m_tempArray);

	return m_tempArray.GetSize();
}

//�õ�������Ӧ��ĳ���������,���ַ�����ʽȡ��,nPos���㿪ʼ,���ɹ��򷵻�false
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

//�õ�������Ӧ��ĳ���������,��������ʽȡ��,nPos���㿪ʼ,���ɹ��򷵻�false
bool CScript::GetItemData(CTString strParamName, int nPos,int &value)
{
	CTString strOut;
	if (GetItemData(strParamName,nPos,strOut) == false )
		return false;
	value = atoi(strOut);
	return true;
}

//�õ�������Ӧ��ĳ���������,��������ʽȡ��,nPos���㿪ʼ,���ɹ��򷵻�false
bool CScript::GetItemData(CTString strParamName, int nPos, double &value)
{
	CTString strOut;
	if (GetItemData(strParamName,nPos,strOut) == false )
		return false;
	value = atof(strOut);
	return true;
}

//�õ�������Ӧ���������Ƿ����Value,�����򷵻�true
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
	CTString strAll =  "Ƶ�ʷ���\
    /������=���� ���� ���\
    /��������=��ֵ ��ֵ ����\
    /�ֲ� = ƫ��";
	ImportStr(strAll);
	CTString strParamName = "��������";
	//FindParamName(strParamName);
	int nPos = GetItemCount(strParamName);
	CTString strOut;
	GetItemData(strParamName,2,strOut);
}

//�õ���Ӧ������������������ַ���,�����ַ���,�Ѿ�û��ǰ��Ŀո�
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
//�õ���Ӧ�������������λ����Ϣ,����ں������λ��,����������
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
				psItem = new sItem;//���һ��Ԫ����
				psItem->nFirstPos = nLastSpace + 1;
				psItem->nLastPos = i-1;
				tempArray.Add(psItem);
			}
			nLastSpace = i;
		}
	}	
	//���һ��
	psItem = new sItem;
	psItem->nFirstPos = nLastSpace + 1;
	psItem->nLastPos = nLen - 1;
	tempArray.Add(psItem);
	return true;
}
