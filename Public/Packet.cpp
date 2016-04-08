// Packet.cpp: implementation of the TCPacket class.
//
//////////////////////////////////////////////////////////////////////


#include "memory.h"
#include "Packet.h"

#include "../Public/Util.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCPacket::TCPacket()
{
	m_szPacketName = "";
}

TCPacket::TCPacket(const string &szPacketName)
{
	m_szPacketName = szPacketName;
}

TCPacket::~TCPacket()
{
	Clear();
}

/*
	SetPacketName(const string &szPacketName)
	设置包的名称  
*/
void    TCPacket::SetPacketName(const string &szPacketName)
{
	m_szPacketName = szPacketName;
}

/*
int TCPacket::Import(char * pBuf)
输入分解包的数据
*/
int     TCPacket::Import(const char * pBuf)
{
	Clear();
	char buf[5];
	memcpy(buf,pBuf,4);
	buf[4] = '\0';
	if (strcmp(buf,"pack") != 0)
		return -1;
	int off = sizeof(PACKET);
	m_szPacketName.append(pBuf + off,((PACKET *)pBuf)->nPackNameLen);
	//m_szPacketName = TCUtil::UtfToAnsi(pBuf + off,((PACKET *)pBuf)->nPackNameLen);
	off = ((PACKET *)pBuf)->nPackNameLen + ((PACKET *)pBuf)->nRevLength + sizeof(PACKET);
	for (int i = 0; i < ((PACKET *)pBuf)->nItemCount; i++)
	{
		memcpy(buf,pBuf + off,4);
		buf[4] = '\0';
		if (strcmp(buf,"item")!=0)
			return -1;
		ITEM * pItem = (ITEM *)(pBuf + off);
		int len = pItem->nItemLength - sizeof(ITEM) - pItem->nItemNamelen;
		m_veItemLen.push_back(len);
		string str;
		str.append(pBuf + off + sizeof(ITEM) ,pItem->nItemNamelen);
		m_veItemName.push_back(str);
		char * pData = new char[pItem->nItemLength - pItem->nItemNamelen - sizeof(ITEM)];
		memcpy(pData,pBuf + off + pItem->nItemNamelen + sizeof(ITEM), pItem->nItemLength - pItem->nItemNamelen - sizeof(ITEM));
		m_veItemData.push_back(pData);
		off += pItem->nItemLength;
	}
	return GetPacketLength();
}

/*
int     TCPacket::Export(char * pBuf, int nBufLen) const
打包输出，对条目的数据进行组合
*/
int     TCPacket::Export(char * pBuf, int nBufLen) const
{
	int len = GetPacketLength();
	if (nBufLen < len)
		return -1;
	memcpy(pBuf,"pack",4);
	((PACKET *)pBuf)->nPackLength = len;
	((PACKET *)pBuf)->nPackNameLen = m_szPacketName.length();
	((PACKET *)pBuf)->nRevLength = 0;
	memcpy(pBuf+sizeof(PACKET),m_szPacketName.c_str(),m_szPacketName.length());
	char * pCur = pBuf+sizeof(PACKET) + m_szPacketName.length();
	for (int i = 0; i< m_veItemName.size(); i++)
	{
		memcpy(pCur,"item",4);
		((ITEM *)pCur)->nItemLength = m_veItemLen.at(i)+sizeof(ITEM) +m_veItemName.at(i).length() ;
		((ITEM *)pCur)->nItemNamelen = m_veItemName.at(i).length();
		pCur += sizeof(ITEM);
		memcpy(pCur,m_veItemName.at(i).c_str(),m_veItemName.at(i).length());
		pCur += m_veItemName.at(i).length();
		memcpy(pCur,m_veItemData.at(i),m_veItemLen.at(i));
		pCur += m_veItemLen.at(i);
	}
	((PACKET *)pBuf)->nPackLength = GetPacketLength();
	((PACKET *)pBuf)->nItemCount = m_veItemName.size();
	return GetPacketLength();
}

/*
void    TCPacket::AddItem(const string szItemName, char * pItemData, int nItemlen)
添加条目szItemName为条目的名称 pItemData 为条目的数据nItemlen条目数据的长度
*/
void    TCPacket::AddItem(const string szItemName, char * pItemData, int nItemlen)
{
	if (GetItemPos(szItemName) >=0)
		DeleteItem(szItemName);
	if (nItemlen <= 0)
		return ;
	char *pBuf = new char[nItemlen];
	memcpy(pBuf,pItemData,nItemlen);
	m_veItemData.push_back(pBuf);
	m_veItemName.push_back(szItemName);
	m_veItemLen.push_back(nItemlen);
}
/*
void    TCPacket::AddItem(const string szItemName, char * pItemData, int nItemlen)
添加条目szItemName为条目的名称 pItemData 为条目的数据nItemlen条目数据的长度
*/
void    TCPacket::AddItem(const string szItemName, string szItemData)
{
	if (GetItemPos(szItemName) >=0)
		DeleteItem(szItemName);

	int nItemlen = szItemData.length();
	if (nItemlen <= 0)
		return ;
	char *pBuf = new char[nItemlen];
	memcpy(pBuf,szItemData.c_str(),nItemlen);
	m_veItemData.push_back(pBuf);
	m_veItemName.push_back(szItemName);
	m_veItemLen.push_back(nItemlen);
}
/*
void    TCPacket::DeletItem(const string &szItemName)
删除条目szItemName 为删除条目的名称
*/
void    TCPacket::DeleteItem(const string &szItemName)
{
	int nPos = GetItemPos(szItemName);
	DeleteItem(nPos);
}

/*
void    TCPacket::DeletItem(int pos)
删除条目的位置pos喂药删除的条目的名称
如果没有超出条目的范围，直接删除，
超出条目的范围，不删除。
*/
void    TCPacket::DeleteItem(int pos)
{
	if (pos < m_veItemData.size() && pos >= 0)
	{
		
		/*
		char * p = m_veItemData.at(pos);
		m_veItemData.erase(&m_veItemData[pos]);
		m_veItemName.erase(&m_veItemName[pos]);
		m_veItemLen.erase(&m_veItemLen[pos]);
		*/
		//--scott 060922  , iterater replaced 
		char * p;
		p = *(m_veItemData.begin()+pos);
		m_veItemData.erase(m_veItemData.begin()+pos);
		m_veItemName.erase(m_veItemName.begin()+pos);
		m_veItemLen.erase(m_veItemLen.begin()+pos);		
		delete [] p;
	}
}

/*
string  TCPacket::GetItemName(int pos) const
获得条目名称输入参数pos为条目所所在位置
如果所在位置存在，则返回条目的名称
*/
string  TCPacket::GetItemName(int pos) const
{
	string s;
	if (pos < m_veItemName.size() && pos >= 0)
		s = m_veItemName.at(pos);
	return s;
}

/*
int     TCPacket::GetItemPos(const string & szItemName) const
为获得条目的位置，输入参数szItemName为名称
返回值：条目所在位置，如果不存在，返回-1
*/
int     TCPacket::GetItemPos(const string & szItemName) const
{
	for (int i =0; i< m_veItemName.size(); i++)
	{
		if(strcmp(m_veItemName.at(i).c_str(),szItemName.c_str()) == 0)
			return i;
	}

	return -1;
}

/*
void    TCPacket::SetItemName(const string &szItemName, int pos)
设置条目的名称，输入参数：szItemName为条目的名称，pos为条目的位置
*/
void   TCPacket::SetItemName(const string &szItemName, int pos)
{
	if (GetItemPos(szItemName)==-1)
		return;
	if ((pos < m_veItemName.size())&&(pos >= 0))
		m_veItemName.at(pos) = szItemName;
}

/*
char  * TCPacket::GetItemData(const string &szItemName) const
为获得条目的数据，输入参数：szItemName为获得条目的名称
返回值：条目的数据
*/
char  * TCPacket::GetItemData( string szItemName) 
{
	int nPos = GetItemPos(szItemName);
	return GetItemData(nPos);
}

/*
char  * TCPacket::GetItemData(const string &szItemName) const
为获得条目的数据，输入参数：szItemName为获得条目的名称
返回值：条目的数据,以字符串方式
*/
string TCPacket::GetItemDataAsString(string szItemName)
{
	char *pBuf = GetItemData(szItemName);
	string strItemData;
	strItemData.append(pBuf,GetItemDataLen(szItemName));
	return strItemData;
}

/*
char  * TCPacket::GetItemData(int pos) const
获得条目的数据，输入参数：pos为条目的位置
返回值：说在位置的条目的数据
*/
char  * TCPacket::GetItemData(int pos) const
{
	if (pos < m_veItemData.size() && pos >= 0)
		return m_veItemData.at(pos);
	return NULL;
}

/*
int     TCPacket::GetItemCount() const
返回值：获得条目的总数
*/
int     TCPacket::GetItemCount() const
{
	return m_veItemName.size();
}

/*
void  TCPacket::SetItemData(int pos, const char * pItemData, int nITemLen)
设置条目的数据，如果所在的位置有效，进行设置条目中的数据
*/
void  TCPacket::SetItemData(int pos, const char * pItemData, int nITemLen)
{
	if ((pos < m_veItemData.size())&&(pos >= 0))
	{
		char *pData = new char[nITemLen];
		memcpy(pData,pItemData,nITemLen);
		char  *p = m_veItemData.at(pos);
		m_veItemData.at(pos) = pData;
		m_veItemLen.at(pos) = nITemLen;
		delete [] p;
	}
}

/*
int   TCPacket::GetPacketLength() const
获得包的长度，返回值为包的长度
*/
int   TCPacket::GetPacketLength() const
{
	int len;
	len = sizeof(PACKET) + strlen(m_szPacketName.c_str());
	for (int i = 0; i< m_veItemLen.size(); i++)
	{
		len += sizeof(ITEM);
		len += m_veItemName.at(i).length();
		len += m_veItemLen.at(i);
	}
	return len;	
}




void TCPacket::Clear()
{
	m_szPacketName = "";
	for (int i = 0; i < m_veItemData.size(); i++)
	{
		char * p = m_veItemData.at(i);
		delete [] p;
	}
	m_veItemName.clear();

	m_veItemData.clear();
	m_veItemLen.clear();
}

int TCPacket::GetItemDataLen(string szItemName)
{
	int nPos = GetItemPos(szItemName);
	return GetItemDataLen(nPos);
}

int TCPacket::GetItemDataLen(int nPos)
{
	if (nPos<0 || nPos>=m_veItemLen.size())
		return -1;
	else
		return m_veItemLen[nPos];
}


