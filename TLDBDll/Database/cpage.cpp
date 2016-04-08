// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cdataset.h"
#include "cpage.h"
#include "cpages.h"
#include "cfakerecnos.h"
#include "cfield.h"
#include "cfields.h"

//#include "TlDatabase.h"//###_Mod 2002-10-7 zlq

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::CPage 
¹¦ÄÜËµÃ÷£º
    ¹¹Ôìº¯Êý
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
	[in]pOwner:±¾Ò³ÃæËùÊôµÄÒ³Ãæ¼¯Ö¸Õë.
·µ»ØÀàÐÍ£º
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
CPage::CPage(CPages* pOwner)
{
	m_pPages = pOwner;	   //Éè¶¨ËùÊôµÄÒ³Ãæ¼¯
	m_bIsModify = false;   //±¾Ò³ÊÇ·ñ¸Ä±ä¹ý

	m_bReachEndError = false;

	m_lngFakeStart=1; //±¾Ò³ÆðÊ¼Î±¼ÍÂ¼ºÅ
	m_lngLStartRecNo = 1;  //±¾Ò³ÆðÊ¼Âß¼­¼ÍÂ¼ºÅ
	m_nLogicCount = 0;     //±¾Ò³Âß¼­¼ÍÂ¼ºÅµÄ¸öÊý ²»ÓÃº¯ÊýSetLogicCount()
	m_nOriginLoadCount = 0;//±¾Ò³Î±¼ÍÂ¼¸öÊý

	m_nCurNo = -1;			//µ±Ç°Âß¼­¼ÍÂ¼ºÅ 
	m_pFields =0;		   //Ö¸ÏòCFields µÄÖ¸Õë



	m_pPages->m_PageArraySort.AddPage(this);
	m_DatasArray.SetSize(0,_IncreaseNum);
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::~CPage
¹¦ÄÜËµÃ÷£º
    Îö¹¹º¯Êý
ÏêÏ¸½âÊÍ£º
    
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    
ÖÆ×÷£º
    Eric2002-07-24
ÐÞ¸Ä: 
************************************************************************************/
CPage::~CPage()
{
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = m_pFields->FieldByIndex(i);
		//#_ÐÞ¸Ä 2002-11-26 $ 9:09:41 zlq
		pField->pg_Delete(m_DatasArray[i]);
	}
	m_pPages->m_PageArraySort.RemovePage(this);
}


/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::Load
¹¦ÄÜËµÃ÷£º
    °Ñ"×°ÈëÉêÇë"·ÅÈëIO¶ÓÁÐÖÐ
ÏêÏ¸½âÊÍ£º
	1¡£ÏÈµ÷ÓÃÒ³Ãæ¼¯Á´±íµÄGetLogicToFakeµÃµ½Æ«ÒÆ£¬¼ÆËã³öÎªÆ«ÒÆ
	2¡£ÐÞ¸Ä±¾Ò³ÃæµÄ³ÉÔ±±äÁ¿m_lngLStartRecNo£¬m_lngFakeStartRecNo
	3¡£¸ºÔðÐÞ¸Ä×Ô¼ºµÄÒ³Ãæ×´Ì¬ÎªµÈ´ý×°Èë×´Ì¬
	4¡£µ÷ÓÃÎ±¼ÇÂ¼Á´±íµÄ½øÕ»²Ù×÷
³öÈë²ÎÊý£º
    [in]logicRecNo£º×°ÈëµÄÆðÊ¼Âß¼­¼ÍÂ¼ºÅ
    [in]count£º	×°ÈëµÄ¸öÊý.
    [in]IsNeedFeedback£ºÊÇ·ñÒªµÈ×ÅÓÃ.
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
void CPage::Load(int logicRecNo,int count, bool IsNeedFeedback)
{
	//¼ÆËã³öÆðÊ¼Î±¼ÍÂ¼ºÅ,×°ÈëÒ³ÃæÊ±Ê¹ÓÃ.
	int iOffset;
	iOffset = m_pPages->GetLogicToFake(logicRecNo);
	//m_pPages->PrintPages();
	//assert( logicRecNo - iOffset>0);
	//ÉèÖÃ´ËÒ³ÆðÊ¼Âß¼­¼ÍÂ¼ºÅ
	m_lngLStartRecNo = logicRecNo;	
	m_lngFakeStart = logicRecNo - iOffset ;
	m_nOriginLoadCount = count;
	SetLogicCount(count);//730 add

	//ÉêÇë×°Èë,·Åµ½×°Èë¶ÓÁÐ,²¢ÉèÖÃÒ³×´Ì¬
	//SetPageState(pgWaitLoad);
	//ÐÞ¸ÄÏà¹Ø±äÁ¿
	m_bIsModify = false;
	
	m_pPages->m_pFakeRecNos->InQueueLoad(this,m_lngFakeStart,count,IsNeedFeedback);
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::UnLoad
¹¦ÄÜËµÃ÷£º
	Ð´ÈëÊý¾Ý,Á¢¼´·µ»Ø²î¾à¸öÊý£¨±¾Ò³ÃæµÄÌí¼ÓÉ¾³ý²Ù×÷µÄÏà¼õ)
	¸ºÔðÐÞ¸Ä×Ô¼ºµÄÒ³Ãæ×´Ì¬ÎªµÈ´ý×´Ì¬Ð´»Ø
ÏêÏ¸½âÊÍ£º
	£¨ÓÉÓÚÊÇµ¥Í¨µÀ£¬ËùÒÔ¿ÉÒÔÓÉÉÏ¼¶¸ù¾Ý·µ»ØÖµµ÷ÕûÆäËü±È×Ô¼º´óµÄÒ³ÃæµÄ¾ÉÆðÊ¼Î»ÖÃ£©
³öÈë²ÎÊý£º
	[in]bIsNeedFeedBack:ÊÇ·ñµÈ×ÅÓÃ.
·µ»ØÀàÐÍ£º
    int	:·µ»ØÎïÀí¼ÍÂ¼ÓëÂß¼­¼ÍÂ¼µÄ²îÖµ.
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::UnLoad(bool bIsNeedFeedBack)
{
	//Á¢¼´·µ»Ø²î¾à¸öÊý
	int nOffset;
	nOffset = m_nLogicCount - m_nOriginLoadCount;

	
	//·Åµ½UnLoad¶ÓÁÐÖÐ,²¢ÐÞ¸Ä×Ô¼ºµÄÒ³Ãæ×´Ì¬,ÆäËû×´Ì¬ÏÈ²»ÐÞ¸Ä
	m_pPages->m_pFakeRecNos->UnLoad(this);
	m_nOriginLoadCount = m_nLogicCount;
	//SetPageState(pgUnLoading);
	m_bIsModify = false;
	
	return nOffset;
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::HangFields
¹¦ÄÜËµÃ÷£º
    ½«×Ö¶Î¼¯ºÍÊý¾Ý¼¯ÁªÏµÔÚÒ»Æð
ÏêÏ¸½âÊÍ£º
    Êµ¼ÊÉÏµ÷Õû¸÷×Ö¶ÎµÄm_pValueÖ¸Ïò
    ´Ëº¯Êý×îºÃÓÉÉÏ¼¶µ÷ÓÃ
³öÈë²ÎÊý£º
    [in,out]pFields£º  ×Ö¶Î¼¯µÄÖ¸Õë
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
void CPage::HangFields(CFields* pFields)
{
	m_pFields=pFields;
	if(pFields)
		pFields->m_pDataSet =  this;
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::BindFields
¹¦ÄÜËµÃ÷£º
    ½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
void CPage::BindFields()
{  
	//¾¯¸æ£º²»ÄÜÓÃÂß¼­¿ÕÒ³À´ÅÐ¶Ï£¬¶øÊÇÓÃÎïÀí¿ÕÒ³£¬µ«²»ÄÜÓÃIsPhysicalEmpty£¨£©

	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = m_pFields->FieldByIndex(i);
		pField->m_pValue = pField->pg_GetAt(m_DatasArray[i],m_nCurNo);

	}
}







/************************************************************************************
¹¦ÄÜËµÃ÷£º
	µ±Ò³ÃæÒò²»Í£µÄÌí¼Ó¶ø±äµÃÅÓ´óÊ±ÐèÒªµ÷Õû
ÏêÏ¸½âÊÍ£º
	½«Ò»²¿·ÖÊý¾Ý¿½±´µ½ÆäËüÒ¶Ãæ£¬ÔÚ´ËÖ®Ç°£¬ÆäËüÒ¶ÃæÓ¦¸ÃÏÈÐ´»ØÕâ¸ö¹ý³ÌÓÉCPagesµÄGetEmptyPageÊµÏÖ¡£
	Í¬Ê±¸Ã¹ý³Ì±ØÐë¸ºÔð½«ÒÑ¾­´«µ½ÆäËüÒ¶ÃæµÄÊý¾ÝÔÚ±¾Ò¶ÃæÖÐÈ¥³ý
³öÈë²ÎÊý£º
	[in]CopyFlags£º 	¸´ÖÆµÄ¿ªÊ¼Î»ÖÃ
	[in]nPhysicalCount£º¸´ÖÆµÄ¸öÊý
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
	Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::CopyToOther(CFromPos CopyFlags, int nPhysicalCount)
{//===uc  CopyToOther
	//È¡µÃ¿½±´µÄÄ¿±êÒ³,±£´æÔÚpPageEmptyÖÐ
	CPage * pPageEmpty;
	pPageEmpty = m_pPages->GetPhysicalEmptyPage();
	
	//´ÓÒ³Í·¿ªÊ¼¿½±´nPhysicalCount¸ö¼ÍÂ¼.
	if( CopyFlags == FromBegin )
	{
		for(int i=0;i<nPhysicalCount;++i)
		{
			for(int j = 0;j<m_DatasArray.GetSize();j++)
			{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
				CField * pField = m_pFields->FieldByIndex(j);				
				switch(pField->GetFieldType())
				{
				case fCurrency:
				case fBoolean:
				case fDate:
				case fInt:
				case fDouble:
					((PF_NUM)pPageEmpty->m_DatasArray[j])->Add(((PF_NUM)m_DatasArray[j])->GetAt(i));
					break;
				case fString:
					((PF_STRING)pPageEmpty->m_DatasArray[j])->Add(((PF_STRING)m_DatasArray[j])->GetAt(i));
					break;
				default:
					break;
				}
			}
		}

		//Ò»´ÎÐÔÉ¾³ý
		for(int j = 0;j<m_DatasArray.GetSize();j++)
		{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
			CField * pField = m_pFields->FieldByIndex(j);
			switch(pField->GetFieldType())
			{
				case fCurrency:
				case fBoolean:
				case fDate:
				case fInt:
				case fDouble:
				((PF_NUM)m_DatasArray[j])->RemoveAt(0,nPhysicalCount);
				break;
			case fString:
				((PF_STRING)m_DatasArray[j])->RemoveAt(0,nPhysicalCount);
				break;
			default:
				break;
			}
		}
        //ÐÞ¸ÄÄ¿±êÒ³Ãæ×´Ì¬µÈ±äÁ¿
		//pPageEmpty->SetPageState(pgNormal);
		pPageEmpty->m_bIsModify = true;

		
		//Ä¿±êÒ³¶øÍ·²¿ÐÅÏ¢.
		pPageEmpty->m_lngFakeStart = m_lngFakeStart;
		//assert(pPageEmpty->m_lngFakeStart>0);
		pPageEmpty->m_lngLStartRecNo = m_lngLStartRecNo;
		pPageEmpty->SetLogicCount(nPhysicalCount);//ÒÆ¶¯µÄ×ÜÊý-ÆäÖÐÉ¾³ýµÄ¸öÊý
		pPageEmpty->m_nOriginLoadCount = 0;//±¾Ò³Î±¼ÍÂ¼¸öÊý ===temp

		
		m_bIsModify = true;
		
        //Ô´Ò³ÃæÍ·²¿ÐÅÏ¢.
		// ÏÖÔÚµÄÖµ = Ô­À´µÄÖµ + ÒÆµ½ËûÒ³µÄ¸öÊý
		m_lngLStartRecNo = m_lngLStartRecNo + nPhysicalCount;//()ÄÚÎªÒÑÒÆ¶¯µÄÂß¼­¼ÍÂ¼ºÅµÄ¸öÊý;
		// ÏÖÔÚµÄÖµ = Ô­À´µÄÖµ - ÒÆµ½ËûÒ³µÄ¸öÊý
		m_nLogicCount = m_nLogicCount - pPageEmpty->GetLogicCount();
		m_nOriginLoadCount;//±¾Ò³Î±¼ÍÂ¼¸öÊý ²»±ä

		
		//ÒÔÏÂ¶þ¸ö±äÁ¿Èç¹ûÎª¸ºÖµ,Ðèµ÷Õû.µ÷ÕûÎªµÚÒ»¸öÂß¼­¼ÍÂ¼µÄÖµ.
		m_nCurNo -= pPageEmpty->m_nLogicCount;
		if(m_nCurNo<0)
			First();
		if (pPageEmpty->GetLogicCount())
		{
			m_pPages->m_PageArraySort.MoveToWorkArea(pPageEmpty); //12-30
		}

    }
////////////////////////////////////////////////////////////////////////////////////////
	//´ÓÒ³Î²¿ªÊ¼¿½±´nPhysicalCount¸ö¼ÍÂ¼
	if(CopyFlags == FromEnd)
	{
		for(int i = m_nLogicCount-nPhysicalCount;i<m_nLogicCount;++i)
		{
			for(int j = 0;j<m_DatasArray.GetSize();j++)
			{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
				CField * pField = m_pFields->FieldByIndex(j);				
				switch(pField->GetFieldType())
				{
				case fCurrency:
				case fBoolean:
				case fDate:
				case fInt:
				case fDouble:
					((PF_NUM)pPageEmpty->m_DatasArray[j])->Add(((PF_NUM)m_DatasArray[j])->GetAt(i));
					break;
				case fString:
					((PF_STRING)pPageEmpty->m_DatasArray[j])->Add(((PF_STRING)m_DatasArray[j])->GetAt(i));
					break;
				default:
					break;
				}
			}
		}

		//Ò»´ÎÐÔÉ¾³ý
		for(int j = 0;j<m_DatasArray.GetSize();j++)
		{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
			CField * pField = m_pFields->FieldByIndex(j);
			switch(pField->GetFieldType())
			{
				case fCurrency:
				case fBoolean:
				case fDate:
				case fInt:
				case fDouble:
				((PF_NUM)m_DatasArray[j])->RemoveAt(m_nLogicCount-nPhysicalCount,nPhysicalCount);
				break;
			case fString:
				((PF_STRING)m_DatasArray[j])->RemoveAt(m_nLogicCount-nPhysicalCount,nPhysicalCount);
				break;
			default:
				break;
			}
		}
		
		//ÐÞ¸ÄÄ¿±êÒ³Ãæ×´Ì¬µÈ±äÁ¿
		pPageEmpty->m_bIsModify = true;
		pPageEmpty->m_nOriginLoadCount = m_nOriginLoadCount;//×ªÒÆµ½ºóÃæÒ»Ò³
		m_nOriginLoadCount = 0;
		
		//pPageEmpty->m_FState = dsBrowse;
		//Ô´Ò³ÃæÐÅÏ¢
		//m_lngFakeStart;  //²»±ä
		//m_lngLStartRecNo;          //²»±ä
		m_nLogicCount = m_nLogicCount - nPhysicalCount;
		//Ä¿±êÒ³ÃæÐÅÏ¢
		pPageEmpty->m_lngFakeStart = m_lngFakeStart ;
		//assert(pPageEmpty->m_lngFakeStart>0);
		pPageEmpty->m_lngLStartRecNo = m_lngLStartRecNo + m_nLogicCount;
		pPageEmpty->SetLogicCount(nPhysicalCount);
		
		m_bIsModify = true;
		//Èç¹ûµ±Ç°¼ÍÂ¼±»ÒÆ¶¯.µ÷ÕûÎªµÚÒ»¸öÂß¼­¼ÍÂ¼µÄÖµ.
		if(m_nCurNo>=m_nLogicCount)
			First(); 

		if (pPageEmpty->GetLogicCount())
		{
			m_pPages->m_PageArraySort.MoveToWorkArea(pPageEmpty);
		}
    }
	this->HangFields(m_pPages->m_pFields);
	return true;

}

/***********************************************************************************
º¯ÊýÃû³Æ£º
CPage::AdjustFakeRecNo
¹¦ÄÜËµÃ÷£º
    ¸ù¾Ý´«ÈëµÄlogicRecNo,fakeRecNo¾ö¶¨ÊÇ·ñÐèÒªµ÷Õû±¾Ò³Ãæ£¬Óë±¾Ò³Ëù´¦×´Ì¬ÎÞ¹Ø

ÏêÏ¸½âÊÍ£º
    ÓÉÒ¶Ãæ¼¯µÄUnLoadÒ»¸öÒ³Ê±µ÷ÓÃ£¬ËüµÄÆ«ÒÆÁ¿ nFakeOffset ÓÉÐ¶ÔØÒ³ÃæµÄunload·½·¨·µ»Ø
    Ò³Ãæ¼¯½«µ÷ÓÃ¸÷¸öÒ³ÃæµÄÕâ¸ö·½·¨µ÷Õû´ó¼ÍÂ¼Ò³µÄÆðÊ¼Î±¼ÍÂ¼ºÅ
    ÐÞ¸ÄÆðÊ¼Î±¼ÍÂ¼ºÅ
³öÈë²ÎÊý£º
    [in]logicRecNo£º´«ÈëÐ¶ÔØÒ³µÄÂß¼­¼ÍÂ¼ºÅ,Ó¦ÎªÄ³Ò³µÄÂß¼­ÆðÊ¼¼ÍÂ¼ºÅ
    [in]fakeRecNo:´«ÈëÐ¶ÔØÒ³µÄÎ±¼ÍÂ¼ºÅ,µ±Âß¼­¼ÇÂ¼ºÅÏàÍ¬Ê±ÔÙ±È½Ï¸Ã²ÎÊý,
	              Á¬ÐøÉ¾³ý¼¸¸öÒ³,»á³öÏÖÂß¼­¼ÍÂ¼ºÅÏàÍ¬µÄÇé¿ö.

    [in]nFakeOffset:Âß¼­¼ÍÂ¼ºÅÓëÎ±¼ÍÂ¼ºÅµÄÆ«ÒÆ
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
void CPage::AdjustFakeRecNo(int logicRecNo,int fakeRecNo, int nFakeOffset)
{
	
	if(m_lngLStartRecNo > logicRecNo) //(==Ê±²»¸Ä±ä)
	{  //Èç¹û´ËÒ³µÄÆðÊ¼Âß¼­¼ÍÂ¼ºÅ´óÓÚ´«ÈëµÄ¼ÍÂ¼ºÅ,ÎÞÂÛÒ³Ãæ´¦ÔÚºÎÖÖ×´Ì¬,¶¼Òª¸Ä±äÆðÊ¼Î±¼ÍÂ¼ºÅ.
		m_lngFakeStart = m_lngFakeStart+nFakeOffset;
		//assert(m_lngFakeStart>0);
	}
	if (m_lngLStartRecNo == logicRecNo)
	{//Èç¹ûÂß¼­¼ÇÂ¼ºÅÏàµÈ,ÔòÔÙÍ¨¹ýÎ±¼ÇÂ¼ºÅÅÐ±ð
		if (m_lngFakeStart > fakeRecNo)
		{
			m_lngFakeStart = m_lngFakeStart+nFakeOffset;
			//assert(m_lngFakeStart>0);
		}
	}
	//assert(m_lngFakeStart>0);
}


/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::Next
¹¦ÄÜËµÃ÷£º
    Âß¼­ÒÆ¶¯¼ÍÂ¼
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::Next()
{	

	//Èç¹ûÒÑµ½´ïÂß¼­Ä©¼ÍÂ¼,Ôò·µ»Ø.
	if(m_nCurNo == m_nLogicCount-1) 
	{
		return false;
		//assert(0);
	}

	m_nCurNo++;
	//ÎïÀí¼ÍÂ¼ºÅ·¢Éú¸Ä±ä

	BindFields();//===temp yh ¿ÉÒÔ²»Òª
	return true;

//}}}END  2002-11-20 Eric
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::Prior
¹¦ÄÜËµÃ÷£º
    Âß¼­ÒÆ¶¯¼ÍÂ¼
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::Prior()
{

	//assert(m_nCurNo);//Èç¹ûÒÑµ½´ïÂß¼­Ê×¼ÍÂ¼,Ôò·µ»Ø.

	m_nCurNo--;

	BindFields();
	return true;

//}}}END  2002-11-20 Eric
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::MoveBy
¹¦ÄÜËµÃ÷£º
    Âß¼­ÒÆ¶¯¼ÍÂ¼
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
    [in]iSkipLen£º²½³¤
    [in]MoveFlags:¶¯ÆðÊ¼Î»ÖÃ
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::MoveBy(int iSkipLen ,CFromPos MoveFlags)
{
	//ÒÆ¶¯µ½ CFromPos Ö¸¶¨µÄÎ»ÖÃ,Í¬Ê±¼ì²é²ÎÊý
	switch(MoveFlags)
	{
	case FromBegin:
		{
			m_nCurNo = iSkipLen;
		}
		break;		
	}

	//»Ö¸´±¾Ò³µÄ°ó¶¨ÐÅÏ¢
	BindFields();
	return true;
}





/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::First
¹¦ÄÜËµÃ÷£º
    ÒÆ¶¯µ½Âß¼­µÚÒ»Ìõ¼ÍÂ¼
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::First()
{
	//ÒÆ¶¯µ½ÎïÀíÊ×¼ÍÂ¼,Èç¹ûÆäÎªÉ¾³ý×´Ì¬,ÔÙÏòÏÂÒÆÒ»Ìõ¼ÍÂ¼.


	m_nCurNo = 0;

	BindFields();
	return true;

//}}}END  2002-11-20 Eric
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::Last
¹¦ÄÜËµÃ÷£º
    ÒÆ¶¯µ½Âß¼­×îºóÒ»Ìõ¼ÍÂ¼
ÏêÏ¸½âÊÍ£º
    ÒÆ¶¯µ½ÎïÀíÄ©¼ÍÂ¼,Èç¹ûÆäÎªÉ¾³ý×´Ì¬,ÔÙÏòÉÏÒÆÒ»Ìõ¼ÍÂ¼.
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::Last()
{

	//ÒÆ¶¯µ½ÎïÀíÄ©¼ÍÂ¼,Èç¹ûÆäÎªÉ¾³ý×´Ì¬,ÔÙÏòÉÏÒÆÒ»Ìõ¼ÍÂ¼.

	m_nCurNo = m_nLogicCount -1;
	//ÎïÀí¼ÍÂ¼ºÅ·¢Éú¸Ä±ä
	BindFields();

	return true;
//}}}END  2002-11-20 Eric

}




/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::GetRecNo
¹¦ÄÜËµÃ÷£º
    ·µ»Øµ±Ç°¼ÇÂ¼ºÅ
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    int	:¼ÍÂ¼ºÅ
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::GetRecNo()
{
	return m_nCurNo+m_lngLStartRecNo;
}

/************************************************************************************
º¯ÊýÃû³Æ£º
   CPage::FieldByName 
¹¦ÄÜËµÃ÷£º
    ÓÉ×Ö¶ÎÃû³ÆµÃµ½×Ö¶Î
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
    [in]FieldName£º×Ö¶ÎÃû³Æ
·µ»ØÀàÐÍ£º
    CField*	:×Ö¶ÎÖ¸Õë
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
CField* CPage::FieldByName(CTString FieldName)
{
	if(m_pFields)
	{
		CField * pField;
		pField = m_pFields->FieldByName(FieldName);
		return pField;
	}
	return 0;
}



/************************************************************************************
º¯ÊýÃû³Æ£º

¹¦ÄÜËµÃ÷£º
    ÉèÖÃÒ³ÃæÎªÔö¼Ó×´Ì¬
ÏêÏ¸½âÊÍ£º
    AppendÊ±,ÖµÏÈ±£´æÔÚNEW ³öµÄ»º³åÇøÖÐ
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º

ÐÞ¸Ä: 
************************************************************************************/
#ifdef TEMP_Reserve
bool CPage::Append()
{
	if(m_pFields)
	{
		m_pFields->NewData();
	}
	return true;
}
bool CPage::Post()
{  /*
	if (m_pFields->GetLastVersion())//#_ÐÞ¸Ä 2003-12-24 $ 14:22:30 zlq
		m_pFields->SetCurVersion();//08-26

	//CField * pTempField; //Ê¹ÓÃ×Ö¶ÎµÄ·½·¨²Ù×÷Êý¾Ý
	int i=0;

	switch(m_FState)
	{
	case dsBrowse:
		break;
	case dsInsert:
		//ÏòÃ¿¸ö×Ö¶Î²åÈëÐÂÖµ
		for( i = 0;i<m_DatasArray.GetSize();i++)
		{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pgInsertAt(m_DatasArray[i],m_nPhysicalNo);
		}//#_ÐÞ¸Ä 2002-11-25 $ 17:39:16 zlq

		//ÐÞ¸Ä¼ÍÂ¼×´Ì¬ÁÐ±íÖÐÐÅÏ¢
		m_bytePhysicalStateArray.InsertAt(m_nPhysicalNo,psAdd);
		m_OriginRecPosArray.InsertAt(m_nPhysicalNo,m_TempOriginPos);//#_ÐÞ¸Ä 2002-12-26 $ 14:06:05 zlq
		m_LogNoArray.InsertAt(m_nCurNo,m_nPhysicalNo);
		for( i = m_nCurNo+1;i<m_LogNoArray.GetSize();i++)
		{
			m_LogNoArray[i]++;
		}


		if (GetPageState() == pgNormal)
		{//È·±£²»ÊÇIOÏß³Ìµ÷ÓÃ
			SetLogicCount(m_nLogicCount+1);//Ò³Ãæ×ÜÂß¼­¸öÊý
			m_bIsModify = true;
			
			if(GetPhysicalCount() >= m_pPages->GetMaxCountPerPage())//200
			{
				CFromPos CopyPos;
				if(m_nCurNo >= m_pPages->GetNormalCountPerPage())
					CopyPos = FromBegin;
				else
					CopyPos = FromEnd;
				CopyToOther(CopyPos,m_pPages->GetNormalCountPerPage());
			}		
		}
		BindFields();
		
		break;
	case dsAppend:
		for( i = 0;i<m_DatasArray.GetSize();i++)
		{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pgAdd(m_DatasArray[i]);
		}//#_ÐÞ¸Ä 2002-11-25 $ 17:51:52 zlq

		//ÐÞ¸Ä¼ÍÂ¼×´Ì¬ÁÐ±íÖÐÐÅÏ¢
		m_bytePhysicalStateArray.Add (psAdd);
		m_OriginRecPosArray.Add(m_TempOriginPos);//#_ÐÞ¸Ä 2002-12-26 $ 14:06:05 zlq
		m_nCurNo++;//µ±Ç°Âß¼­¼ÍÂ¼ºÅ(Ã¿Ò³ÖØÐÂ´Ó0¿ªÊ¼±àºÅ.)
		m_nPhysicalNo =GetPhysicalCount()-1;
		m_LogNoArray.Add(m_nPhysicalNo);		

		if (GetPageState() == pgNormal)
		{//È·±£²»ÊÇIOÏß³Ìµ÷ÓÃ
			SetLogicCount(m_nLogicCount+1);//Ò³Ãæ×ÜÂß¼­¸öÊý
			m_bIsModify = true;

			if(GetPhysicalCount() >= m_pPages->GetMaxCountPerPage())
			{
				CopyToOther(FromBegin,m_pPages->GetNormalCountPerPage());
			}
		}
		BindFields();
		break;
	case dsEdit:
		for( i = 0;i<m_DatasArray.GetSize();i++)
		{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
			CField * pField = m_pFields->FieldByIndex(i);
			//#_ÐÞ¸Ä 2002-11-25 $ 18:11:55 zlq
			pField->pgSetAt(m_DatasArray[i],m_nPhysicalNo);
		}

		BindFields();

		if (m_bytePhysicalStateArray[m_nPhysicalNo] == psNoChange)//ÐÞ¸Ä¼ÍÂ¼×´Ì¬ÁÐ±íÖÐÐÅÏ¢,Ô­À´Ìí¼ÓµÄ²»ÄÜ½øÈëÐÞ¸Ä×´Ì¬
			m_bytePhysicalStateArray[m_nPhysicalNo] = psModify;
		m_bIsModify = true;
		break;
	default:
		break;
	}
	m_FState = dsBrowse;*/
	return true;

//}}}END  2002-11-20 Eric
}
/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::Edit
¹¦ÄÜËµÃ÷£º
    ÉèÖÃÒ³ÃæÎª±à¼­×´Ì¬
ÏêÏ¸½âÊÍ£º
    EDITÊ±,ÖµÏÈ±£´æÔÚNEW ³öµÄ»º³åÇøÖÐ
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
bool CPage::Edit()
{/*	
	//Íù»º³åÇøÖÐÐ´ÈëÊý¾Ý,Ò»´ÎÐ´ÈëÒ»¸ö×Ö¶Î.
	m_pFields->NewData();
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = m_pFields->FieldByIndex(i);
		PF_NUM pDoubleField;
		switch(pField->GetFieldType())
		{
		case fInt:
			pField->GetData ((LPBYTE) &((*(PF_INT)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		case fDouble:
			pDoubleField = (PF_NUM)m_DatasArray[i];
			pField->GetData ((LPBYTE) &((*pDoubleField)[m_nPhysicalNo]));
			break;
		case fString:
			pField->GetData ((LPBYTE) &((*(PF_STRING)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		case fCurrency:
			pField->GetData ((LPBYTE) &((*(PF_CURRENCY)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		case fBoolean:
			pField->GetData ((LPBYTE) &((*(PF_BOOL)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		case fDate:
			pField->GetData ((LPBYTE) &((*(PF_DATE)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		default:
			break;
		}
	}*/
	//m_FState = dsEdit;
	return true;
}


/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::Insert
¹¦ÄÜËµÃ÷£º
    ÉèÖÃÒ³ÃæÎª²åÈë×´Ì¬
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
bool CPage::Insert()
{
	if(m_pFields)
	{
		m_pFields->NewData();
		//m_FState = dsInsert;
	}
	return true;
}

bool CPage::Post(CStoreType nStoreType ,long lngPosition )
{
	SetOriginPos(nStoreType,lngPosition);
	return Post();	
}
#endif
/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::Delete
¹¦ÄÜËµÃ÷£º
    Âß¼­É¾³ýµ±Ç°¼ÍÂ¼
ÏêÏ¸½âÊÍ£º
    Èç¹û²»ÊÇÐÂÔöµÄ¼ÍÂ¼,Ôò½ö×öÉÏÉ¾³ý±ê¼Ç.
	Èç¹ûÊÇÐÂÔöµÄ¼ÍÂ¼,Ôò´ÓÄÚ´æÖÐÉ¾³ý.
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
bool CPage::_DeleteEx()
{
	if (m_pFields->GetLastVersion())//===ky  ¿Ï¶¨Ê±×îÐÂ°æ±¾ÁË
		m_pFields->SetCurVersion();

	{
		for(int i = 0;i<m_DatasArray.GetSize();i++)
		{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pg_RemoveAt(m_DatasArray[i],m_nCurNo);
		}

		m_nLogicCount--;
		if(m_nCurNo >= m_nLogicCount)
		{			
			m_nCurNo = m_nLogicCount-1;	
		} 

		m_bIsModify = true;
		if (m_nLogicCount == 0)
			return false;
		BindFields();
	}

	return true;
}





/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::SetRecord
¹¦ÄÜËµÃ÷£º
	ÉèÖÃµ±Ç°Ìõ¼ÇÂ¼µÄËùÓÐµÄ×Ö¶ÎµÄÊý¾ÝÐÅÏ¢µ½Ò»¸öÖ¸¶¨µÄ»º³åÇøÀï
	Êý¾Ý×éÖ¯°´Ö¸Ê¾ÐÅÏ¢²¿·Ö£¨ËùÓÐ×Ö¶Î£¬Ã¿¸ö×Ö¶ÎÒ»×Ö½Ú£©+Êý¾Ý²
	·Ö£¨ËùÓÐ×Ö¶Î£¬¿ÉÄÜÄ³Ð©×Ö¶ÎÒ²Ã»ÓÐÊý¾Ý£¬Ç°ÃæÄÜ¹»±í´ïÊ±£©
	·µ»ØÊý¾Ý³¤¶È
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
    [in,out]lpData£º»º³åÇøÖ¸Õë
·µ»ØÀàÐÍ£º
    int	: Êý¾Ý³¤¶È
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
int CPage::_SetRecord(LPBYTE lpData)
{ 
	if(m_pFields)
	{
	    //BindFields();
		return (int)m_pFields->SaveDataToBuffer(lpData);  //
	}
	else
	{
		return false;
	}
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::GetRecord
¹¦ÄÜËµÃ÷£º
    ¶Ôµ±Ç°µÄ¼ÇÂ¼Êý¾Ý¸³Öµ²Ù×÷
ÏêÏ¸½âÊÍ£º
	£¨Êý¾Ý¼¯ÔÚ´ËÖ®Ç°Ó¦¸ÃÍ¨¹ý´¦ÓÚ±à¼­
	»òÌí¼Ó×´Ì¬£¬µ÷ÓÃappend,editµÈ·½·¨£©£¬
	µ÷ÓÃ×Ö¶ÎÁ´±í¸÷×Ö¶ÎµÄLoadDataFromBuffer
	·µ»ØÊÇ·ñ×°ÔØ³É¹¦£¬¼°ÓÐÐ§»º³åÇøÊý¾Ý´óÐ¡
    
³öÈë²ÎÊý£º
    [in,out]lpData£º »º³åÇø
    [in,out]count£º×Ö½ÚÊý
·µ»ØÀàÐÍ£º
    bool :ÊÇ·ñ³É¹¦
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
bool CPage::GetRecord(LPBYTE lpData, int& count)
{
	LPBYTE lpDatatemp = lpData;
	count = 0;
	if (!GetRecord(lpDatatemp))
		return false;
	count = lpDatatemp - lpData;
	return true;
}

/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::GetRecord
¹¦ÄÜËµÃ÷£º
    ¶Ôµ±Ç°µÄ¼ÇÂ¼Êý¾Ý¸³Öµ²Ù×÷
ÏêÏ¸½âÊÍ£º
	£¨Êý¾Ý¼¯ÔÚ´ËÖ®Ç°Ó¦¸ÃÍ¨¹ý´¦ÓÚ±à¼­
	»òÌí¼Ó×´Ì¬£¬µ÷ÓÃappend,editµÈ·½·¨£©£¬²¢ÇÒÐÞ¸ÄÖ¸Õë
	µ÷ÓÃ×Ö¶ÎÁ´±í¸÷×Ö¶ÎµÄLoadDataFromBuffer
³öÈë²ÎÊý£º
    [in,out]lpData£º »º³åÇøÖ¸Õë
·µ»ØÀàÐÍ£º
    bool:ÊÇ·ñ³É¹¦
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
bool CPage::GetRecord(LPBYTE& lpData)
{
	//Èç¹û´ËÒ³²»ÔÚ append edit Insert ×´Ì¬,·µ»Ø
	//if(m_FState != dsAppend&&m_FState != dsInsert&&m_FState != dsEdit)
	//	return false;


	if(m_pFields)
	{
	    //BindFields();
		int count;
		return m_pFields->LoadDataFromBuffer(lpData, count); 
	}
	else
	{
		return false;
	}
}



/************************************************************************************
º¯ÊýÃû³Æ£º

¹¦ÄÜËµÃ÷£º
    ´ÓÄÚ´æÖÐÉ¾³ýÊý¾Ý,Ö»Ê£ÏÂ×Ö¶ÎÁ´±í
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    void
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
void CPage::EmptyTable()
{
	//if (GetPageState()== pgNormal)
	//if (nStoreType == storeMemory)
	{//Ö÷Ïß³Ìµ÷ÓÃ
		m_pPages->m_pFields->GetBookmark();	 
		HangFields(m_pPages->m_pFields);
	}

	for( int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = m_pFields->FieldByIndex(i);
		pField->pg_RemoveAll(m_DatasArray[i]);
	}//#_ÐÞ¸Ä 2002-11-26 $ 9:03:55 zlq


	m_bIsModify = false;   //±¾Ò³ÊÇ·ñ¸Ä±ä¹ý

	m_bReachEndError = false;
	
	//m_lngFakeStartRecNo=1; //±¾Ò³ÆðÊ¼Î±¼ÍÂ¼ºÅ
	//m_lngLStartRecNo = 1;  //±¾Ò³ÆðÊ¼Âß¼­¼ÍÂ¼ºÅ
	//m_nLogicCount = 0;     //±¾Ò³Âß¼­¼ÍÂ¼ºÅµÄ¸öÊý ²»ÓÃº¯ÊýSetLogicCount()
	//m_nOriginLoadCount = 50;//±¾Ò³Î±¼ÍÂ¼¸öÊý
	
	m_nCurNo = -1;			//µ±Ç°Âß¼­¼ÍÂ¼ºÅ 
	//m_pFields =0;		   //Ö¸ÏòCFields µÄÖ¸Õë
	
	
	//if (GetPageState()== pgNormal)
	//if (nStoreType == storeMemory)	
	{//Ö÷Ïß³Ìµ÷ÓÃ
		m_pPages->m_pFields->GotoBookmark();
	}

}
/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::IsLogiclEmpty
¹¦ÄÜËµÃ÷£º
    ÊÇ·ñÂß¼­¿ÕÒ³
ÏêÏ¸½âÊÍ£º
    µ±Ò³Ãæ´¦ÓÚÕý³£×´Ì¬Ê±,ÊÇ·ñÊÇÂß¼­¿ÕÒ³.¼´È«²¿ÊÇÉ¾³ýµÄÊý¾Ý,ÕâÑùÕâ¸öÆô¶¯¼ÇÂ¼ºÅ²ÅÓÐÓÃ
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    bool
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
bool CPage::IsLogicalEmpty()
{
	if(m_nLogicCount == 0)
		return true;
	else
		return false;
}

/************************************************************************************
¹¦ÄÜËµÃ÷£º
    ÊÇ·ñÊÇÎïÀí¿ÕÒ³
ÏêÏ¸½âÊÍ£º
    Ã»ÓÐÎïÀí¼ÍÂ¼.
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    
ÖÆ×÷£º
    Eric2002-07-03
ÐÞ¸Ä: 
************************************************************************************/
bool CPage::IsPhysicalEmpty()
{
	{//Ö÷Ïß³Ìµ÷ÓÃ
		m_pPages->m_pFields->GetBookmark();	 
		HangFields(m_pPages->m_pFields);
	}	
	bool brst = true;
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = m_pFields->FieldByIndex(i);
		int ArraySize = 0;
		if (pField->GetFieldType() == fString)
			ArraySize =((PF_STRING)m_DatasArray[i])->GetSize();
		else
			ArraySize =((PF_NUM)m_DatasArray[i])->GetSize();
		////assert(ArraySize ==m_nLogicCount);
		if (ArraySize >0)
			brst = false;
	}	
	{//Ö÷Ïß³Ìµ÷ÓÃ
		m_pPages->m_pFields->GotoBookmark();
	}
	return brst;
	/*
	if (m_nLogicCount==0 && m_nOriginLoadCount==0)
		return true;
	else
		return false;
	
	if(m_bytePhysicalStateArray.GetSize() == 0)
		return true;
	else
		return false;*/
}
/************************************************************************************
º¯ÊýÃû³Æ£º
    CPage::GetCurVersion
¹¦ÄÜËµÃ÷£º
    µÃµ½µ±Ç°Ò³ÃæµÄ°æ±¾ºÅ
ÏêÏ¸½âÊÍ£º
³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
    
ÖÆ×÷£º
    Eric2002-07-12
ÐÞ¸Ä: 
************************************************************************************/
int CPage::GetCurVersion()
{
	 return m_pFields->GetCurVersion();
}
/************************************************************************************
º¯ÊýÃû³Æ£º
	CPage::AdjustLogicRecNo
¹¦ÄÜËµÃ÷£º
    ¸ù¾Ý´«ÈëµÄlogicRecNo¾ö¶¨ÊÇ·ñÐèÒªµ÷ÕûËùÓÐÒ³Ãæ£¬Óë±¾Ò³Ëù´¦×´Ì¬ÎÞ¹Ø
ÏêÏ¸½âÊÍ£º
    Ò³Ãæ¼¯½«µ÷ÓÃ¸÷¸öÒ³ÃæµÄÕâ¸ö·½·¨µ÷Õû´ó¼ÍÂ¼Ò³µÄÆðÊ¼Âß¼­¼ÍÂ¼ºÅ
    ÐÞ¸ÄÆðÊ¼Âß¼­¼ÍÂ¼ºÅ
³öÈë²ÎÊý£º
    [in]logicRecNo£º´«ÈëµÄÂß¼­¼ÍÂ¼ºÅ,Ó¦ÎªÄ³Ò³µÄÂß¼­ÆðÊ¼¼ÍÂ¼ºÅ
	[in]nFakeOffset:Âß¼­¼ÍÂ¼ºÅµÄÆ«ÒÆ
·µ»ØÀàÐÍ£º
    
ÖÆ×÷£º
    Eric2002-07-24
ÐÞ¸Ä: 
************************************************************************************/

void CPage::AdjustLogicRecNo(int logicRecNo,int nOffset)
{
	if(m_lngLStartRecNo > logicRecNo) //(==Ê±²»¸Ä±ä)
	{  //Èç¹û´ËÒ³µÄÆðÊ¼Âß¼­¼ÍÂ¼ºÅ´óÓÚ´«ÈëµÄ¼ÍÂ¼ºÅ,ÎÞÂÛÒ³Ãæ´¦ÔÚºÎÖÖ×´Ì¬,¶¼Òª¸Ä±äÆðÊ¼Î±¼ÍÂ¼ºÅ.
		m_lngLStartRecNo += nOffset;
	}
}
/************************************************************************************
º¯ÊýÃû³Æ£º
	CPage::InsertField
¹¦ÄÜËµÃ÷£º
	²åÈëÒ»¸ö×Ö¶Î
ÏêÏ¸½âÊÍ£º

³öÈë²ÎÊý£º
	[in]nIndex	£º²åÈëµÄÎ»ÖÃ
	[in]pNewField:²åÈëµÄ×Ö¶Î
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
	Eric 2002-9-4
ÐÞ¸Ä:

************************************************************************************/
void CPage::InsertField(int nIndex,CField* pNewField)
{

	PF_NUM                pDouble;    
	PF_STRING                pString;    
     

	FieldNUM                F_Double;
	FieldString                        F_Str;
	
	F_Double =   NULLValue;

   
	
	int nPhyCount = m_nLogicCount ;//m_bytePhysicalStateArray.GetSize();
	
	int i;
	switch(pNewField->GetFieldType())
	{
	case fInt:
	case fDate:
	case fBoolean:
	case fCurrency:
	case fDouble:
		pDouble = new F_NUM;
		for(i = 0;i<nPhyCount;i++)
		{
			pDouble->Add(F_Double);
		}
		m_DatasArray.InsertAt(nIndex,pDouble);
		break;
	case fString:
		pString = new F_STRING;
		for(i = 0;i<nPhyCount;i++)
		{
			pString->Add(F_Str);
		}
		m_DatasArray.InsertAt(nIndex,pString);
		break;

	default:
		break;
	}

}
/************************************************************************************
º¯ÊýÃû³Æ£º
	CPage::AddField
¹¦ÄÜËµÃ÷£º
	Ôö¼ÓÒ»¸ö×Ö¶Î
ÏêÏ¸½âÊÍ£º

³öÈë²ÎÊý£º
	[in] pNewField :ÐÂÔöµÄ×Ö¶ÎµÄÖ¸Õë
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
	Eric 2002-9-4
ÐÞ¸Ä:

************************************************************************************/
void CPage::AddField(CField* pNewField)
{ 

  
		PF_NUM		pDouble;    
		PF_STRING		pString;    

		FieldNUM		F_Double;
		FieldString			F_Str;

		//F_Double.Data[0] = 0xFE;
		F_Double = NULLValue;

		F_Str ="";
								  
		int nPhyCount = m_nLogicCount;//m_bytePhysicalStateArray.GetSize();

		int i;
		switch(pNewField->GetFieldType())
		{
		case fInt:
		case fDate:
		case fBoolean:
		case fCurrency:
		case fDouble:
			pDouble = new F_NUM;
			pDouble->SetSize(nPhyCount,IncreaseSize);
			for(i = 0;i<nPhyCount;i++)
				pDouble->SetAt(i,F_Double);
 			m_DatasArray.Add(pDouble);
			break;
		case fString:
			pString = new F_STRING;
			pString->SetSize(nPhyCount,IncreaseSize);
			for(i = 0;i<nPhyCount;i++)
				pString->SetAt(i,F_Str);
   			m_DatasArray.Add(pString);
			break;

		default:
			break;
		}
}

/************************************************************************************
º¯ÊýÃû³Æ£º
	CPage::DeleteField
¹¦ÄÜËµÃ÷£º
	É¾³ýÒ»¸ö×Ö¶Î
ÏêÏ¸½âÊÍ£º

³öÈë²ÎÊý£º
	[in]nIndex	 £ºÉ¾³ýµÄÎ»ÖÃ
	[in]pOldField  £ºÉ¾³ýµÄ×Ö¶Î.´ËÊ±FieldsÀïÒÑÃ»ÓÐ´Ë×Ö¶Î,ËùÒÔÓÉÍâÃæ´«½øÀ´
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
	Eric 2002-9-4
ÐÞ¸Ä:

************************************************************************************/
void CPage::DeleteField(int nIndex,CField* pOldField)
{

	if(nIndex >m_DatasArray.GetSize()||nIndex<0)
	{
		//assert(0);
		return ;
	}


	PF_NUM	pDouble;    
	PF_STRING	pString;    
 
	FieldNUM F_Double;
	F_Double= NULLValue;
	//int i;
	//CField *pField = m_pFields->FieldByIndex(nIndex);
	switch(pOldField->GetFieldType())
	{
	case fInt:
	case fDate:
	case fBoolean:
	case fCurrency:
	case fDouble:
		pDouble = (PF_NUM)(m_DatasArray[nIndex]);
		pDouble->RemoveAll();
		delete pDouble;
		m_DatasArray.RemoveAt(nIndex);
		break;
	case fString:
		pString = (PF_STRING)(m_DatasArray[nIndex]);
		pString->RemoveAll();
		delete pString;
		m_DatasArray.RemoveAt(nIndex);
		break;

	default:
		break;
	}

	if(m_DatasArray.GetSize() == 0)
	{
		m_bIsModify = false;   //±¾Ò³ÊÇ·ñ¸Ä±ä¹ý


		m_lngFakeStart=1; //±¾Ò³ÆðÊ¼Î±¼ÍÂ¼ºÅ
		m_lngLStartRecNo = 1;  //±¾Ò³ÆðÊ¼Âß¼­¼ÍÂ¼ºÅ
		SetLogicCount(0);      //±¾Ò³Âß¼­¼ÍÂ¼ºÅµÄ¸öÊý
		m_nOriginLoadCount = 0;//±¾Ò³Î±¼ÍÂ¼¸öÊý

		m_nCurNo = 0;			//µ±Ç°Âß¼­¼ÍÂ¼ºÅ 
	}
}
/************************************************************************************
º¯ÊýÃû³Æ£º
	CPage::ModifyField
¹¦ÄÜËµÃ÷£º
	ÐÞ¸ÄÒ»¸ö×Ö¶Î
ÏêÏ¸½âÊÍ£º

³öÈë²ÎÊý£º
	[in] nIndex	 £ºÒªÐÞ¸ÄµÄÎ»ÖÃ
	[in] pOldField	£ºÒªÐÞ¸ÄµÄ×Ö¶Î
	[in] pNewField£º  ½«»»Îª´Ë×Ö¶Î
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
	Eric 2002-9-4
ÐÞ¸Ä:

************************************************************************************/
void CPage::ModifyField(int nIndex,CField* pOldField, CField* pNewField)
{
	if(nIndex > m_DatasArray.GetSize() || nIndex < 0)
	{
		//assert(0);
		return ;
	}

	PF_NUM		pInt;   
	PF_NUM	pDouble;    
	PF_STRING	pString;    
	PF_STRING	pString_;    
	PF_NUM	pCurrency;  
	PF_NUM		pBool;      
     
	PF_NUM		pDate;  

	FieldNUM F_Double;
	F_Double = NULLValue;
	int i;
	pNewField->NewData();

	switch(pOldField->GetFieldType())
	{
	case fInt://ÐÂ×Ö¶Î
		pInt = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				return;//Êµ¼ÊÉÏÃ»ÓÐÐÞ¸Ä
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					FieldString strTemp;
					FieldString * pTemp = (FieldString *)pNewField->m_pValue;
					strTemp	= *pTemp ;
					pString->Add(strTemp);
				}
				break;
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;

			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pInt;

		break;
	case fDouble://ÐÂ×Ö¶Î
		pDouble = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 

				break;
			case fDouble:
				return;//Î´¸Ä±ä
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					FieldString strTemp;
					FieldString * pTemp = (FieldString *)pNewField->m_pValue;
					strTemp	= *pTemp;
					pString->Add(strTemp);
				}
				break;
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue =(LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;

			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pDouble;

		break;
	case fString://ÐÂ×Ö¶Î
		pString = (PF_STRING)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString_ = new F_STRING;
				pString_->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString_);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					FieldString strTemp;
					FieldString * pTemp = (FieldString *)pNewField->m_pValue;
					strTemp	= *pTemp;
					pString_->Add(strTemp);
				}
				break;				
				//return;//Î´¸Ä±ä
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;

			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pString;

		break;
	case fCurrency://ÐÂ×Ö¶Î
		pCurrency = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;//Êµ¼ÊÉÏÃ»ÓÐÐÞ¸Ä
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pString->Add(*((FieldString*)pNewField->m_pValue));
				}
				break;
			case fCurrency:
				return;//Î´¸Ä±ä
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pCurrency;

		break;
	case fBoolean://ÐÂ×Ö¶Î
		pBool = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;//Êµ¼ÊÉÏÃ»ÓÐÐÞ¸Ä
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);

					pString->Add(*((FieldString*)pNewField->m_pValue));
				}
				break;
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				return;//Î´¸Ä±ä

			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pBool;

		break;

	case fDate://ÐÂ×Ö¶Î
		pDate = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					FieldString strTemp;
					FieldString * pTemp = (FieldString *)pNewField->m_pValue;
					strTemp	= *pTemp;
					pString->Add(strTemp);
				}
				break;
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;

			case fDate:
				return;//Î´¸Ä±ä
			default:
				break;	
		}
		delete pDate;

		break;
	default:
		break;
	}

	pNewField->DeleteData();//

	if(m_DatasArray.GetSize() == 0)
	{
		m_bIsModify = false;   //±¾Ò³ÊÇ·ñ¸Ä±ä¹ý

		m_lngFakeStart=1; //±¾Ò³ÆðÊ¼Î±¼ÍÂ¼ºÅ
		m_lngLStartRecNo = 1;  //±¾Ò³ÆðÊ¼Âß¼­¼ÍÂ¼ºÅ
		SetLogicCount(0);      //±¾Ò³Âß¼­¼ÍÂ¼ºÅµÄ¸öÊý
		m_nOriginLoadCount = 0;//±¾Ò³Î±¼ÍÂ¼¸öÊý

		m_nCurNo = 0;			//µ±Ç°Âß¼­¼ÍÂ¼ºÅ 
	//}}}END  2002-11-22 Eric
	}
}

/************************************************************************************
º¯ÊýÃû³Æ£º
	CPage::TransFieldValue
¹¦ÄÜËµÃ÷£º
	²»Í¬×Ö¶ÎÀàÐÍµÄ×Ö¶Î¼äÖµµÄ×ª»».
ÏêÏ¸½âÊÍ£º

³öÈë²ÎÊý£º
	[in]pNewField£ºÐÂµÄ×Ö¶Î
	[in]pOldField £º¾ÉµÄ×Ö¶Î
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
	Eric 2002-9-4
ÐÞ¸Ä:

************************************************************************************/
void CPage::TransFieldValue(CField* pNewField, CField* pOldField)
{
	if(pOldField->IsNull())
	{
		pNewField->SetNull();
		return;
	}

	if(pNewField->GetFieldType()==fString)
	{
	    pNewField->SetAsString(pOldField->GetAsString());
		return;
	}

	pNewField->SetNull();

	switch(pOldField->GetFieldType())
	{
	case fString :
		pNewField->SetAsString(pOldField->GetAsString());
		break;
	case fInt :
		pNewField->SetAsInteger(pOldField->GetAsInteger());
		break;
	case fBoolean :
		pNewField->SetAsBool(pOldField->GetAsBool());
		break;
	case fDate :
		pNewField->SetAsDateTime(&pOldField->GetAsDateTime());
		break;

	case fCurrency :
		pNewField->SetAsCurrency(pOldField->GetAsCurrency());
		break;
	case fDouble:
		pNewField->SetAsDouble(pOldField->GetAsDouble());
		break;
	}
}
/************************************************************************************
º¯ÊýÃû³Æ£º
	CPage::SetCurVersion
¹¦ÄÜËµÃ÷£º
	ÉèÖÃ±¾Ò³ÃæµÄµ±Ç°°æ±¾
ÏêÏ¸½âÊÍ£º

³öÈë²ÎÊý£º
	[in] nVersion :°æ±¾ºÅ
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
	Eric 2002-9-4
ÐÞ¸Ä:

************************************************************************************/
void CPage::SetCurVersion(int nVersion)
{
	 m_pFields->SetCurVersion(nVersion);
}
/************************************************************************************
º¯ÊýÃû³Æ£º
   CPage::GetNewestVersion
¹¦ÄÜËµÃ÷£º
	µÃµ½×îÐÂµÄ°æ±¾ºÅ
ÏêÏ¸½âÊÍ£º

³öÈë²ÎÊý£º
·µ»ØÀàÐÍ£º
	int : °æ±¾ºÅ
ÖÆ×÷£º
	Eric 2002-9-4
ÐÞ¸Ä:

************************************************************************************/
int CPage::GetNewestVersion()
{
	return m_pFields->GetLastVersion();
}


/************************************************************************************
º¯ÊýÃû³Æ£º

¹¦ÄÜËµÃ÷£º

ÏêÏ¸½âÊÍ£º

³öÈë²ÎÊý£º
	[in,out]
·µ»ØÀàÐÍ£º

ÖÆ×÷£º
	Eric 2002-11-20
ÐÞ¸Ä:

************************************************************************************/
void CPage::IniFieldCnt(CFields *pFields)
{
	int nFieldCount = pFields->GetFieldCount();
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = pFields->FieldByIndex(i);
		pField->pg_RemoveAll(m_DatasArray[i]);
	}

	m_DatasArray.RemoveAll();


	for(int iFieldIndex = 0;iFieldIndex < nFieldCount;iFieldIndex++	)
	{
		CField * pField = pFields->FieldByIndex(iFieldIndex);
		m_DatasArray.Add(pField->pg_New());
	}//#_ÐÞ¸Ä 2002-11-26 $ 9:38:29 zlq
}


void CPage::MoveField(int nFrom, int nTo)
{
	if (nFrom == nTo)
		return;

	if (nFrom < nTo)
		nTo--;//Èç¹ûÒÆ¶¯µ½ºóÃæ,ÔòÏÈ¼õÎ»ÖÃºÅ
	
	void* value = m_DatasArray[nFrom] ;
	m_DatasArray.RemoveAt(nFrom);//´ÓÀÏÎ»ÖÃÈ¥µô
	m_DatasArray.InsertAt(nTo,value);//ÒÆ¶¯µ½ÐÂÎ»ÖÃ	
}

bool CPage::_AppendIn(CStoreType nStoreType, long lngPosition)
{//ÄÚÍâ¶¼µ÷ÓÃCPages::_Append()   CFileOperation::ReadRecords()
	int nCurVersion = m_pFields->GetCurVersion();
	m_pFields->SetCurVersion();

	{//case dsAppend:
		
		for(int i = 0;i<m_DatasArray.GetSize();i++)
		{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pg_Add(m_DatasArray[i]);
			
		}
		m_nCurNo++;//µ±Ç°Âß¼­¼ÍÂ¼ºÅ(Ã¿Ò³ÖØÐÂ´Ó0¿ªÊ¼±àºÅ.)

		BindFields();
	}
	
	m_pFields->SetCurVersion(nCurVersion);
	return true;
}
bool CPage::_AppendInBatch( int nCount)
{//ÄÚÍâ¶¼µ÷ÓÃCPages::_Append()   CFileOperation::ReadRecords()
	int nCurVersion = m_pFields->GetCurVersion();
	m_pFields->SetCurVersion();//Éýµ½×îÐÂ°æ±¾

	m_LastFieldsAry.RemoveAll();
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = m_pFields->FieldByIndex(i);
		pField->pg_Add(m_DatasArray[i],nCount);
		m_LastFieldsAry.Add(pField);
	}
	//m_nCurNo++;//µ±Ç°Âß¼­¼ÍÂ¼ºÅ(Ã¿Ò³ÖØÐÂ´Ó0¿ªÊ¼±àºÅ.)
	
	//BindFields();

	
	m_pFields->SetCurVersion(nCurVersion);
	return true;
}
bool CPage::_AppendEx()
{//ÄÚÍâ¶¼µ÷ÓÃCPages::_Append()   CFileOperation::ReadRecords()
	if (m_pFields->GetLastVersion())//===ky  ¿Ï¶¨Ê±×îÐÂ°æ±¾ÁË ===temp ¿ÉÒÔÏû³ý
		m_pFields->SetCurVersion();

	{//case dsAppend:
		for(int i = 0;i<m_DatasArray.GetSize();i++)
		{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pg_Add(m_DatasArray[i]);
		}

		m_nCurNo++;//µ±Ç°Âß¼­¼ÍÂ¼ºÅ(Ã¿Ò³ÖØÐÂ´Ó0¿ªÊ¼±àºÅ.)
		m_nLogicCount++;
		m_bIsModify = true;
		
		if(GetLogicCount() >= m_pPages->GetMaxCountPerPage())
		{
			CopyToOther(FromBegin,m_pPages->GetNormalCountPerPage());
		}

		BindFields();
		//break;
	}

	return true;
}
bool CPage::_Edit()
{
	m_bIsModify = true;
	return true;
}

bool CPage::_InsertEx()
{
	if (m_pFields->GetLastVersion())
		m_pFields->SetCurVersion();

	//ÏòÃ¿¸ö×Ö¶Î²åÈëÐÂÖµ
	for(int  i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = m_pFields->FieldByIndex(i);
		pField->pg_InsertAt(m_DatasArray[i],m_nCurNo);
	}
	
	m_nLogicCount++;
	m_bIsModify = true;
	
	if(m_nLogicCount >= m_pPages->GetMaxCountPerPage())//200
	{
		CFromPos CopyPos;
		if(m_nCurNo >= m_pPages->GetNormalCountPerPage())
			CopyPos = FromBegin;
		else
			CopyPos = FromEnd;
		CopyToOther(CopyPos,m_pPages->GetNormalCountPerPage());
	}	
		
	BindFields();
	return true;
}

void CPage::PrintCurLine()//===temp
{
#ifdef tl_debug	
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = m_pFields->FieldByIndex(i);
		int ArraySize = 0;
		if (pField->GetFieldType() == fString)
			ArraySize =((PF_STRING)m_DatasArray[i])->GetSize();
		else
			ArraySize =((PF_NUM)m_DatasArray[i])->GetSize();
		//assert(ArraySize ==m_nLogicCount);
	}	

	for ( i=0;i<m_pFields->GetFieldCount();i++)
	{
		//===temp TRACE(m_pFields->FieldByIndex(i)->GetAsString()+"\t");
	}
	//===temp TRACE("\r\n");
#endif	
}

bool CPage::_SetRec(int nRelativePos)
{
	if (nRelativePos <0 || nRelativePos>=m_nLogicCount)
		return false;
	m_nCurNo = nRelativePos;
	BindFields();
	return true;
}

bool CPage::_SetRecLast(int nRelativePos)
{
	if (nRelativePos <0 || nRelativePos>=m_nLogicCount)
		return false;
	m_nCurNo = nRelativePos;
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //½«Êý¾Ý¼¯ÖÐµÄÊý¾ÝµØÖ·¸³µ½¸÷×Ö¶ÎµÄÊý¾ÝÖ¸ÕëÖÐ
		CField * pField = (CField *)m_LastFieldsAry.GetAt(i);
		pField->m_pValue = pField->pg_GetAt(m_DatasArray[i],m_nCurNo);

	}
	return true;
}
