// Packet.h: interface for the TCPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKET_H__28573049_615A_4617_B71A_34AA245C81C7__INCLUDED_)
#define AFX_PACKET_H__28573049_615A_4617_B71A_34AA245C81C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4786)
#include <vector>
#include <string>
using namespace std;
class TCPacket  
{
	struct PACKET
	{
		char   stPackFlag[4];
		int    nPackLength;
		int    nPackNameLen;
		int    nItemCount;
		int    nRevLength;
		int    nCheck;
	};
	struct ITEM
	{
		char   stItemFlag[4];
		int    nItemLength;
		int    nItemNamelen;
	};
public:
	TCPacket();
	TCPacket(const string &szPacketName);
	virtual ~TCPacket();

public:
	string GetItemDataAsString( string szItemName);
	void    SetPacketName(const string &szPacketName);
	string  &GetPacketName(){return m_szPacketName;}
	int     Import(const char * pBuf);
	int     Export(char * pBuf, int pBufLen) const;
	int     GetPacketLength() const;


	int     GetItemCount() const;

	void    AddItem(const string szItemName, char * pItemData, int nItemlen);
	void    AddItem(const string szItemName, string szItemData);
	int		GetItemDataLen( string szItemName);
	int		GetItemDataLen(int nPos);
	void    DeleteItem(const string &szItemName);
	void    DeleteItem(int pos);

	string  GetItemName(int pos) const;
	int     GetItemPos(const string & szItemName) const;
	void    SetItemName(const string &szItemName, int pos);

	char  * GetItemData(string szItemName) ;
	char  * GetItemData(int pos) const;
	void    SetItemData(int pos, const char * pItemData, int nITemLen);

protected:

	void Clear();
private:
	string  m_szPacketName;
	vector  <string>  m_veItemName;
	vector  <char *>  m_veItemData;
	vector  <int>     m_veItemLen;
};

#endif // !defined(AFX_PACKET_H__28573049_615A_4617_B71A_34AA245C81C7__INCLUDED_)
