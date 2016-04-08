// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPAGE_3DCB520D025C_INCLUDED
#define _INC_CPAGE_3DCB520D025C_INCLUDED
#include "cdataset.h"
//#include "afxtempl.h"
#include "COriginRecPos.h"

typedef byte CPhysicalState;
#define psNoChange				0x00
#define psAdd					0x01
#define psDel					0x02
#define psModify				0x03


//enum CFromPos{FromBegin,FromEnd}
typedef byte CFromPos;
#define FromBegin				0x01
#define FromCur					0x02
#define FromEnd					0x03

//��ҳ������״̬�������¼���
//��������״̬���ȴ�����װ�룬���ݵ��룬�ȴ�����д�أ�����д��״̬
//(�ο�ҳ�漯��GetRecNoState״̬��
typedef byte					CPageState;
#define pgNormal				0x01
//#define pgWaitLoad				0x02
//#define pgLoading				0x03
//#define pgWaitUnLoad			0x04
//#define pgUnLoading				0x05
//#define pgInDisk				0x06 //�ڴ�û���õ�


class CPages;
class AFX_EXT_CLASS CPage 
{
//	COriginRecPos	m_TempOriginPos;
	CPtrArray		m_LastFieldsAry;
public:
	CPtrArray		m_DatasArray;

	int				m_nOriginLoadCount;
	int				m_lngFakeStart;
	
	int m_nLogicCount;
	int				m_lngLStartRecNo;

	//int				m_nLogicNo ;
	int m_nCurNo;//��ǰ�߼���¼��(ÿҳ���´�0��ʼ���.)
	//int				m_nPhysicalNo;	 //��ǰ�����¼��(ÿҳ���´�0��ʼ���.)	

	CPages*			m_pPages;//ӵ�и�ҳ���ҳ�漯
	CFields* m_pFields;
	//��ǰ�ļ�¼��
	int m_nCurRecNo;//===tempG

	bool			m_bReachEndError;//����β��������
	bool			m_bIsModify;
	//CPageState		m_PageState;
	
protected:

	
public:
	int GetLogicCount(){	return m_nLogicCount;	};	
public:
	bool _SetRec(int nRelativePos);
	bool _SetRecLast(int nRelativePos);
	void PrintCurLine();

	bool _InsertEx();
	bool _Edit();
	bool _AppendIn(CStoreType nStoreType ,long lngPosition );
	bool _AppendInBatch( int nCount);
	bool _AppendEx( );
	bool _DeleteEx();

	void MoveField(int nFrom, int nTo);
	void IniFieldCnt(CFields * pFields);

	void			SetLogicCount(int nLogicCount)
					{
						m_nLogicCount = nLogicCount;
					};
	CPage(CPages* pOwner); 
	~CPage();

	//bool			SetOriginPos(CStoreType nStoreType ,  long lngPosition );
	int				GetNewestVersion();
	void			SetCurVersion(int nVersion);
	int				GetCurVersion();


	bool			IsLogicalEmpty();  
	bool			IsPhysicalEmpty(); 
	int				Next();
	int				Prior();
	int				MoveBy(int count ,CFromPos MoveFlags);

	int				First();
	int				Last();

	int				GetRecNo();
	CField*			FieldByName(CTString FieldName);
	

	int				_SetRecord(LPBYTE lpData);
	bool			GetRecord(LPBYTE lpData, int& count);
	bool			GetRecord(LPBYTE& lpData);

	void			Load(int logicRecNo,int count, bool IsNeedFeedback = false);
	int				UnLoad(bool bIsNeedFeedBack = false);
	/*
	bool			PhysicalFirst();
	bool			PhysicalNext();
	bool			PhysicalPrior();
	bool			PhysicalLast();
	bool			PhysicalDelete();
	//*/
	void			BindFields(); 
	void			HangFields(CFields* pFields);
	//CPhysicalState	GetPhysicalState();
	//void			SetPhysicalState(CPhysicalState NewState  = psNoChange) ;
	int				CopyToOther(CFromPos CopyFlags, int nPhysicalCount);
	void			AdjustFakeRecNo(int logicRecNo,int fakeRecNo, int nFakeOffset);
	void			AdjustLogicRecNo(int logicRecNo,int nOffset);
	void			EmptyTable();
	void			InsertField(int nIndex,CField* pNewField);
	void			AddField(CField* pNewField);
	void			DeleteField(int nIndex,CField* pOldField); 
	void			ModifyField(int nIndex,CField* pOldField, CField* pNewField);
	void			TransFieldValue(CField* pNewField, CField* pOldField);
private:
protected:
	
#ifdef TEMP_Reserve
	/*
	bool			Append();
	bool			Edit();
	bool			Insert();
	bool			Post(CStoreType nStoreType ,  long lngPosition );
	bool			Post();
	//*/
#endif
};

#endif /* _INC_CPAGE_3DCB520D025C_INCLUDED */

