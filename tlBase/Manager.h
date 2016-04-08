// Manager.h: interface for the TCManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANAGER_H__09A84D18_9304_45B4_BB24_46DDC47B3836__INCLUDED_)
#define AFX_MANAGER_H__09A84D18_9304_45B4_BB24_46DDC47B3836__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/Packet.h"
#include "../Public/AnaWord.h"
class TCManager  
{
public:
	int GetUserID(TCPacket &inP);
	bool SetResultState(bool bSuccess, TCPacket &outP,string strMsg="");
	#ifdef _UNIX
	bool DealPacket(TCPacket &inP,TCPacket &outP,bool &forked);
	#else
	bool DealPacket(TCPacket &inP,TCPacket &outP);
	#endif
	TCManager();
	virtual ~TCManager();

protected:
	bool OpenTable(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool CloseTable(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	
	bool ClearCurData(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool Login(TCPacket &inP, TCPacket &outP);
	bool Logout(TCPacket &inP, TCPacket &outP);
	bool algorithmic(TCPacket &inP,string strName,AnaWord &aw, TCPacket &outP);
	bool GetDataRowText(TCPacket &inP,AnaWord &aw,TCPacket &outP);
	bool GetDataInfo(TCPacket &inP,AnaWord &aw,TCPacket &outP);
	bool CreateTable(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool AddRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool EditRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool InsertRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool DeleteRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool SaveFile(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool InsertCol(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool EditCol(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool DeleteCol(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool PasteCol(TCPacket &inP,AnaWord &aw, TCPacket &outP);

	bool Filter(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool Compute(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool Sort(TCPacket &inP,AnaWord &aw, TCPacket &outP);
	bool Sample(TCPacket &inP, AnaWord &aw, string szVO, TCPacket &outP);
	bool Random(TCPacket &inP, AnaWord &aw, string szVO, TCPacket &outP);
	bool FillMissing(TCPacket &inP, AnaWord &aw, string szVO, TCPacket &outP);
	bool SetWeight(TCPacket &inP,AnaWord &aw, TCPacket &outP);
};

#endif // !defined(AFX_MANAGER_H__09A84D18_9304_45B4_BB24_46DDC47B3836__INCLUDED_)
