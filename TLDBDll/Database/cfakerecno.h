// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFAKERECNO_3DCB54340178_INCLUDED
#define _INC_CFAKERECNO_3DCB54340178_INCLUDED


typedef byte  CStoreType;
	
#define storeTlFile       0x01
#define storeTempFile     0x02
#define storeMemory       0x03
#define storeUndoRedo     0x04

struct CFakeRecNo 
{
public:
	CFakeRecNo(){
	}
	CFakeRecNo(int nVersion,CStoreType nStoreType,long lngPosition,long lngRecCount)
	{
		m_nStoreType = nStoreType;
		m_lngPosition = lngPosition;
		m_lngRecCount = lngRecCount;
		m_nVersion = nVersion;
	}
	CFakeRecNo(CFakeRecNo *pNode){
		m_nStoreType = pNode->m_nStoreType;
		m_lngPosition = pNode->m_lngPosition;
		m_lngRecCount = pNode->m_lngRecCount;
		m_nVersion = pNode->m_nVersion;
	}
	//指向的连续的记录条数
	long m_lngRecCount;

	//存储位置 磁盘原文件还是临时文件，或者其他数据源
	CStoreType m_nStoreType;

	//在对应的存储介质上的位置，如天律数据文件则为物理记录号，
	//临时文件则为绝对文件位置
	INT64 m_lngPosition;
	int m_nVersion;
};

#endif /* _INC_CFAKERECNO_3DCB54340178_INCLUDED */

