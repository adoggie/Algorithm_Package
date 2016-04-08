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
	//ָ��������ļ�¼����
	long m_lngRecCount;

	//�洢λ�� ����ԭ�ļ�������ʱ�ļ���������������Դ
	CStoreType m_nStoreType;

	//�ڶ�Ӧ�Ĵ洢�����ϵ�λ�ã������������ļ���Ϊ�����¼�ţ�
	//��ʱ�ļ���Ϊ�����ļ�λ��
	INT64 m_lngPosition;
	int m_nVersion;
};

#endif /* _INC_CFAKERECNO_3DCB54340178_INCLUDED */

