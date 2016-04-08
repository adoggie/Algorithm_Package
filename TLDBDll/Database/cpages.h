// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPAGES_3DCB51D600B9_INCLUDED
#define _INC_CPAGES_3DCB51D600B9_INCLUDED

//temp #include <afxcoll.h>
#include "cpage.h"

class CFakeRecNos;
class CFieldList;

typedef byte  CSortType;
#define stDescent       0x01//½µÐò
#define stAscend        0x02//ÉýÐò



//¸ÃÁ´±íµÄ¸öÊý²»´ó£¬Èç20¸ö£¬Ã¿¸öÒ¶ÃæÒ»°ãÓµÓÐÒ»°ÙÌõ¼ÇÂ¼¡£
//##ModelId=3DCB51D600B9
//#include  "Depot.h"
//### 2002-11-7 Eric------>
#include "PageArraySort.h"

//### 2002-11-7 Eric<------------
class CDriverBase;
//class CRURecordMod;
class CPages 
{//m_PageArraySort  temp
	BYTE m_cDateConvertType;//ÈÕÆÚ×ª»»ÀàÐÍ
	bool m_bIsBof;//###_Mod 2002-10-14 zlq ×¢Òâ²»ÄÜÎªÍ¬Ò»¸öÖµ±íÊ¾
	bool m_bIsEof;
	int m_nSaveVersion;
protected:
	int m_nLogicCount;
public:
	int m_nCurRecNo;//===tempG
	int GetLogicCount(){	return m_nLogicCount;	};
	CFields* m_pFields;

public:

	bool Bof();
	bool Eof();
	//CDepot m_Depot;
	int m_ntemp; //===temp

public:
	CFakeRecNos* m_pFakeRecNos;
private:
	bool RenameFile(CTString strTempFileName, CTString strTargetFileName,CTString &strOutFileName);
	int m_nMaxCountPerPage;    //Ã¿¸öÒ³µÄ×î´ó¼ÇÂ¼ÌõÊý.
	int m_nNormalCountPerPage; //Õý³£Ê±Ã¿Ò³µÄ¼ÇÂ¼ÌõÊý.
	//¾ø¶ÔµÄ×î´óÒ³Ãæ¸öÊý
	int m_nMaxPageCount;

	//Õý³£¹¤×÷Ê±Ò³Ãæ¸öÊý
	int m_nWorkPageCount;

	//ÉèÖÃµÄÒ³ÃæÖÐ¶ÏµÄÔ¤±¨¾¯£¬Ò³Ãæ½øÐÐÔ¤×°Èë£¬±ÜÃâ·ÃÎÊÊý¾ÝÊ±µÈ
	//´ý¡£ÔÝ¶¨Îª100£¨µ½Ê±ºò²â¶¨£©
	int m_nAlertRange;

	//ÉÏÒ»´Î·ÃÎÊµÄÊý¾ÝÂß¼­¼ÍÂ¼ºÅ£¬ÓÃÓÚÒ³ÃæÔ¤²â
	int m_lngLastRecNo; 	  

	int  m_nOldCurRecNo;	 //insert,appendÊ±,±£´æ¼ÍÂ¼ºÅ,ÓÃÓÚcancelÊ±»Ö¸´.

	int m_nCriticalRecCount; //ÔÚÒ³ÃæÖÐÒÆ¶¯Ê±,´ËÒ³Ãæ»¹ÓÐ¼¸Ìõ¼ÍÂ¼Ê±,ÒªÔ¤×°.
	void AdjustLogicRecNo(int logicRecNo,int nOffset);
	void AdjustFakeRecNo(int logicRecNo,int fakeRecNo,  int nFakeOffset);
protected:

	 
public:
	void PrintPages();
	bool _Edit();
	bool _Append();
	bool _Insert();
	bool AdjustPageCount(bool bAtOnce = false);
	void SetSaveFileVersion(int nVersion);
	int GetSaveFileVersion();
	int GetFileVersion();

	//### 2002-11-7 Eric----------------------->
	CPageArraySort m_PageArraySort;
	//### 2002-11-7 Eric<-----------------------
	//µÃµ½Ò³ÉèÖÃÊôÐÔ °üÀ¨ ×î´óµÄÒ³¸öÊý Ã¿¸öÒ³µÄÕý³£Ê±µÄ´óÐ¡ ÒÑ¾­Ê¹ÓÃµÄÒ³
	void GetPageSet(int &nMaxPageCount,int &nPageNormalSize,int &nUsePage);

	bool AdjustOriginCount(int nOffset);
	int GetNormalCountPerPage();
	bool SetNormalCountPerPage(int nCount);
	int GetMaxCountPerPage();

	CTString m_sFileName;
	
	int GetPageCount();

	CPages();

	~CPages();

	void BindFieldList(CFieldList * pFieldList);
	
	int GetInsertPos(int nColIndex,double bValue,CSortType nType,int nNullCount);

	int SearchPos(int nRecSmall,int nRecLarge,double fValue,int nColIndex,CSortType nType);

	bool Clear();

	bool LoadFile(LPCTSTR lpszFileName);


	bool NewFile();

	//pstrOutFile ·µ»Øfalse Ê±ÇÒ±£´æÎªÌìÂÉÎÄ¼þÊ±ÓÐÐ§, Îª±£´æÊ±³ö´íÊ±µÄÁí´æÎªµÄÎÄ¼þÃû×Ö,ÓÉÄÚ²¿Éú³É
	bool SaveFile(LPCTSTR lpszFileName,CTString *pstrOutFile);

	//##ModelId=3DCB52A4006F
	bool SetMaxPageCount(int count);


	CPage* GetCurPage();

	//ÉèÖÃµ±Ç°µÄ¼ÇÂ¼ºÅ£¨µÃµ½Ö¸¶¨¼ÇÂ¼ºÅµÄÊý¾Ý£©
	//Èç¹û²»ÔÚÄÚ´æÒ³ÖÐ£¬Ôòµ÷ÓÃGetNormalPage()µÃµ½Ò»¸ö¿ÉÓÃµÄÒ³£

	//²¢¼ì²éÊÇ·ñÐèÒªÐ¶ÔØ¡£È»ºó·¢³öÒ³ÇëÇó£¬²¢µÈ´ý·µ»Ø£¨ÒòÎªÊÇ¶à
	//Ïß³Ì£¬ËùÒÔ±ØÐëÓÃµ½WaitForSingleObject APIº¯Êý£©
	//
	//×¢Òâ£ºÈç¹ûÐèÒªµÄÊý¾ÝÔÚÒÑ¾­ÉêÇëµÄ¶ÓÁÐÖÐ£¬ÇÒÊÇ¶ÁµÄ£¬Ôò±ØÐë
	//ÐÞ¸Ä¸Ã¶ÓÁÐ¶ÔÓ¦µÄÏî£¬ÒªÇó·µ»ØWaitForSingleObject¡£
	//##ModelId=3DCB59260273
	bool SetCurRecNo(int RecNo);

	//µÃµ½×î½üÃ»ÓÐÊ¹ÓÃµÄÊý¾Ý¼¯
	//##ModelId=3D056C3603E6
	//CPage* GetRecentNoUse();//#_ÐÞ¸Ä 2002-11-12 $ 16:56:44 zlq

	
	//CPage* GetRecentNoUseNoModify(); //#_ÐÞ¸Ä 2002-11-12 $ 16:56:46 zlq
	//##ModelId=3D056C8F01B3
	int GetUsePageCount();

	//µÃµ½¼ÇÂ¼ºÅ¶ÔÓ¦µÄÊý¾ÝËù´¦ÓÚµÄ×´Ì¬¡£
	//ÓÐÒÔÏÂ¼¸ÖÖ×´Ì¬£º
	//Õý³£¹¤×÷×´Ì¬£¬µÈ´ýÊý¾Ý×°Èë£¬Êý¾Ýµ÷Èë£¬µÈ´ýÊý¾ÝÐ´»Ø£¬Êý¾Ý
	//Ð´»Ø×´Ì¬£¬Ã»ÓÐ×°Èë£¨²»°üÀ¨µÈ´ýÊý¾Ý×°Èë£¬Êý¾Ýµ÷Èë£¬µÈ´ýÊý
	//¾ÝÐ´»Ø£¬Êý¾ÝÐ´»Ø×´Ì¬£©
	//(²Î¿¼Ò³Ãæ×´Ì¬£©
	//typedef byte  CPageState
	//
	//#define pgNormal       0x01
	//#define pgWaitLoad     0x02
	//#define pgLoading      0x03
	//#define pgWaitUnLoad   0x04
	//#define pgUnLoading    0x05
	//#define pgInDisk       0x06
	//##ModelId=3D0869600206
	bool QueryRecNoState(int RecNo, CPage*& InPage,int& StartRecNo,int& Count);


	//ÃæµÄAdjustFakeRecNoº¯Êý£©
	//×¢Òâ£ºËùÓÐÒ³Ãæ£¬²»¹Ü´¦ÓÚÊ²Ã´×´Ì¬
	void UnLoad(CPage* pPage,bool IsNeedFeedback=false);

	//µÃµ½Âß¼­¼ÇÂ¼ºÅÓëÎ±¼ÇÂ¼ºÅµÄÆ«ÒÆ
	int GetLogicToFake(int LogicRecNo);
    CPage* GetPhysicalEmptyPage();


	int Next();
	int Prior();

	int MoveBy(int iSkipLen ,CFromPos MoveFlags);

	int First();
	int Last();
	//·µ»Øµ±Ç°¼ÇÂ¼ºÅ
	 int GetRecNo();
	 bool Delete();
	 bool Cancel();


	//·µ»Ø¼ÇÂ¼¼¯µÄ¸öÊý£¬°üÀ¨Ã»ÓÐ×°ÈëÄÚ´æµÄ¼ÇÂ¼
	 int GetRecordCount();


	//ÉèÖÃµ±Ç°Ìõ¼ÇÂ¼µÄËùÓÐµÄ×Ö¶ÎµÄÊý¾ÝÐÅÏ¢µ½Ò»¸öÖ¸¶¨µÄ»º³åÇøÀï
	//
	//Êý¾Ý×éÖ¯°´Ö¸Ê¾ÐÅÏ¢²¿·Ö£¨ËùÓÐ×Ö¶Î£¬Ã¿¸ö×Ö¶ÎÒ»×Ö½Ú£©+Êý¾Ý²

	//·Ö£¨ËùÓÐ×Ö¶Î£¬¿ÉÄÜÄ³Ð©×Ö¶ÎÒ²Ã»ÓÐÊý¾Ý£¬Ç°ÃæÄÜ¹»±í´ïÊ±£©
	//
	//·µ»ØÊý¾Ý³¤¶È
	//##ModelId=3D054ECF01DA
	 int SetRecord(LPBYTE lpData);

	//¶Ôµ±Ç°µÄ¼ÇÂ¼Êý¾Ý¸³Öµ²Ù×÷£¨Êý¾Ý¼¯ÔÚ´ËÖ®Ç°Ó¦¸ÃÍ¨¹ý´¦ÓÚ±à¼­
	//»òÌí¼Ó×´Ì¬£¬µ÷ÓÃappend,editµÈ·½·¨£©£¬
	//µ÷ÓÃ×Ö¶ÎÁ´±í¸÷×Ö¶ÎµÄLoadDataFromBuffer
	//
	//·µ»ØÊÇ·ñ×°ÔØ³É¹¦£¬¼°ÓÐÐ§»º³åÇøÊý¾Ý´óÐ¡
	//##ModelId=3D0020C401A9
	 bool GetRecord(LPBYTE lpData, int& count);

	//¶Ôµ±Ç°µÄ¼ÇÂ¼Êý¾Ý¸³Öµ²Ù×÷£¨Êý¾Ý¼¯ÔÚ´ËÖ®Ç°Ó¦¸ÃÍ¨¹ý´¦ÓÚ±à¼­
	//»òÌí¼Ó×´Ì¬£¬µ÷ÓÃappend,editµÈ·½·¨£©£¬²¢ÇÒÐÞ¸ÄÖ¸Õë
	//µ÷ÓÃ×Ö¶ÎÁ´±í¸÷×Ö¶ÎµÄLoadDataFromBuffer
	//##ModelId=3D0553C00247
	 bool GetRecord(LPBYTE& lpData);

	 //bool SetPagePosition(CPage* pPage ,int nPos);

	/*
	·µ»ØÊ±¸ÃÒ³ÒÑ¾­ÓÐÐ§£¨¼´ÒÑ¾­Ð¶ÔØ¹ý£©
	1¡£1µ÷ÓÃ±¾Ò³Ãæ¼¯µÄUnLoadº¯Êý£¨¸Ãº¯ÊýÄÚ²¿»¹Òªµ÷ÕûÆäËüÒ³ÃæµÄÎ±¼ÇÂ¼ÆðÖ·£©
	1¡£2±¾Ïß³Ì½øÈëµÈ´ý×´Ì¬
	*/
	 void WaitForUnLoad(CPage* pPage);

	/*·µ»ØÊ±¸ÃÒ³ÒÑ¾­ÓÐÐ§
	1¡£1µ÷ÓÃ¸ÃÒ³µÄloadº¯Êý
	1¡£2±¾Ïß³Ì½øÈëµÈ´ý×´Ì¬
	*/
	 void WaitForLoad(CPage* pPage,int logicRecNo, int count );

	/*
	¸ù¾Ý´«ÈëµÄ¼ÇÂ¼ºÅ£¬µÃµ½ËùÔÚµÄÒ³£¨²»ÔÚÄÚ´æÔòÄÚ²¿×°Èë²¢·µ»Ø¸ÃÒ³£©
	×¢Òâ£º¸Ãº¯Êý²»½øÐÐÒ³ÃæË³ÐòµÄµ÷Õû
	·µ»Ø£º¼ÇÂ¼ËùÔÚµÄÒ³

	1.ÏÈ²éÑ¯¸Ã¼ÇÂ¼ºÅ×´Ì¬QueryRecNoState
	1¡£1 Õý³£ÔÚÄÚ´æÖÐ£¬Ôò·µ»Ø¸ÃÒ³
	1¡£2 ÔÚ¶ÓÁÐÖÐ£¬µ÷ÓÃWaitForInQueue(pPage : CPage*)·µ»Ø¸ÃÒ³
	1¡£3 Èç¹û²»ÔÚÄÚ´æÒ³ÖÐ£¬Ôòµ÷ÓÃGetPhysicalEmptyPageµÃµ½Ò»¸ö¿ÉÓÃµÄ¿ÕÒ³,WaitForLoadµÃµ½¸ÃÒ³
	*/

	CPage* GetRecNoPageInNormal(int RecNo);




	void InsertField(int nIndex,CField* pNewField);
	void DeleteField(int nIndex); 
	void ModifyField(int nIndex,CField* pNewField);
	void AddField(CField* pNewField);
	bool MoveRecord(int nSourceRec,int nDestinationRec); 
	void MoveField(int nFrom, int nTo);
};


#endif /* _INC_CPAGES_3DCB51D600B9_INCLUDED */

