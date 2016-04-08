// Section.cpp: implementation of the CSection class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Section.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSection::CSection()
{
	m_pCmdData = NULL;
}

CSection::CSection(char * pCmdData)
{
	m_pCmdData = NULL;
	SetSectionData(pCmdData);
}

CSection::~CSection()
{
	delete [] m_pCmdData;
}

bool CSection::CheckSection()//��Ҫ�Ƿ���proc,endp�����������Ƿ����
{
	if (m_pCmdData==NULL)
		return false;
	if (!Filter())
		return false;
	std::string str = Find("proc");
	if (str.length()==0)
		return false;
	if (Find("endp") != str)
		return false;
	return true;
}

void CSection::SetSectionData(char * pCmdData)//�����������ĵ�ַ
{
	if (m_pCmdData!=NULL)
		delete [] m_pCmdData;
	if (strlen(pCmdData) != 0)
	{
		m_pCmdData = new char[strlen(pCmdData)+1];
		memcpy(m_pCmdData,pCmdData,strlen(pCmdData)+1);
	}
	else
		m_pCmdData = NULL;
}

std::string CSection::GetSectionDataValue(std::string strKey)//��ȡproc�������������س�Ϊ������־
{
	std::string str;
	if(CheckSection())
		str = Find(strKey);
	return str;
}

bool CSection::GetSectionDataValue(std::string strKey, std::string strName, std::string &strValue)
{
	if (CheckSection())
	{
		strValue = Find(strKey,strName);
		if (strValue.length() > 0)
			return true;
	}
	return false;
}

std::string CSection::Find(std::string strKey, std::string strName)
{
	std::string   strResult;
	if (m_pCmdData==NULL)
		return strResult;
	int len = strKey.length();
	if (len >= strlen(m_pCmdData))
		return strResult;

	char *pStr = m_pCmdData;
	char buff[256];
	bool bFind = false;
	int i = 0;
	while(*pStr != '\0')
	{
		if (*pStr != '=')
		{
			buff[i++] = *pStr++;
			continue;
		}
		buff[i] = '\0';
		i = 0;
		if (strcmp(strKey.c_str(),buff)==0)
		{
			bFind = true;
			break;
		}

		if (*pStr == '=')//���֡���=��˵�������ǹؼ��ֵ�ֵ�������ؼ��ֵ�ֱ
		{
			pStr++;
			if (*pStr == '<')
				while (*pStr != '>')
				{
					if (*pStr=='\0')
						return strResult;
					pStr++;
				}
			else
				return strResult;
		}
		pStr++;
	}
	if (!bFind)
		return strResult;

	pStr +=2;
	len = strName.length();
	if (len==0)
	{
		while (*pStr!='>') 
			strResult += *pStr++;
		return strResult;
	}

	bFind = false;
	i = 0;
	while((*pStr != '>')||(*pStr != '\0'))
	{
		if (*pStr != '(')
		{
			buff[i++] = *pStr++;
			continue;
		}
		buff[i] = '\0';
		i = 0;
		if (strcmp(strName.c_str(),buff)==0)
		{
			bFind = true;
			break;
		}

		if (*pStr=='(')
		{
			pStr++;
			while (*pStr!=')')
				pStr++;
		}
		pStr++;
	}

	if (bFind)
	{
		pStr++;
		while (*pStr!=')')
			strResult += *pStr++;
	}

	return strResult;
}


bool   CSection::Filter()
{
	if (m_pCmdData==NULL)
		return false;
	int size = strlen(m_pCmdData)+1;

	char *pStr = m_pCmdData;
	char *pTemp = new char[size];
	char *p = pTemp;

	//��ע��ת���ɿո�
	while(*pStr != '\0')
	{
		if (*pStr == '%')
		{
			*pStr = ' ';
			pStr++;
			while (*pStr != '%') 
			{
				*pStr = ' ';
				pStr++;
			}
			*pStr = ' ';
		}
		pStr++;
	}

	pStr = m_pCmdData;
	//�ѻس��򶺺�ת���ɿո�
	while(*pStr != '\0')
	{
		if ((*pStr == '\n')||(*pStr == ','))
		{
			*pStr = ' ';
			pStr++;
			continue;
		}
		pStr++;
	}
	
	pStr = m_pCmdData;
	//��������Ŀո�
	while ((*pStr==' ')&&(*pStr!='\0'))
		pStr++;
	while(*pStr != '\0')
	{
		*p++ = *pStr;
		if (*pStr == ' ')
		{
			while (*pStr==' ')
				pStr++;
			continue;
		}
		pStr++;
	}
	*p = '\0';

	pStr = pTemp;
	p = m_pCmdData;
	//�����ַ�������ɱ�׼���������
	while(*pStr != '\0')
	{
		if (*pStr == ' ')
		{
			if ((*(pStr+1) == '=')||(*(pStr+1) == '<')||(*(pStr+1) == '>')||(*(pStr+1) == '(')||(*(pStr+1) == ')'))
			{
				pStr++;
				continue;
			}
		}
		*p++ = *pStr++;
		if ((*(pStr-1) == '=')||(*(pStr-1) == '<')||(*(pStr-1) == '>')||(*(pStr-1) == '(')||(*(pStr-1) == ')'))
		{
			if (*pStr == ' ')
				pStr++;
		}
	}
	*p = '\0';

	delete []pTemp;

	return true;
}
