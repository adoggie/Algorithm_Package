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
#define stDescent       0x01//����
#define stAscend        0x02//����



//������ĸ���������20����ÿ��Ҷ��һ��ӵ��һ������¼��
//##ModelId=3DCB51D600B9
//#include  "Depot.h"
//### 2002-11-7 Eric------>
#include "PageArraySort.h"

//### 2002-11-7 Eric<------------
class CDriverBase;
//class CRURecordMod;
class CPages 
{//m_PageArraySort  temp
	BYTE m_cDateConvertType;//����ת������
	bool m_bIsBof;//###_Mod 2002-10-14 zlq ע�ⲻ��Ϊͬһ��ֵ��ʾ
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
	int m_nMaxCountPerPage;    //ÿ��ҳ������¼����.
	int m_nNormalCountPerPage; //����ʱÿҳ�ļ�¼����.
	//���Ե����ҳ�����
	int m_nMaxPageCount;

	//��������ʱҳ�����
	int m_nWorkPageCount;

	//���õ�ҳ���жϵ�Ԥ������ҳ�����Ԥװ�룬�����������ʱ��
	//�����ݶ�Ϊ100����ʱ��ⶨ��
	int m_nAlertRange;

	//��һ�η��ʵ������߼���¼�ţ�����ҳ��Ԥ��
	int m_lngLastRecNo; 	  

	int  m_nOldCurRecNo;	 //insert,appendʱ,�����¼��,����cancelʱ�ָ�.

	int m_nCriticalRecCount; //��ҳ�����ƶ�ʱ,��ҳ�滹�м�����¼ʱ,ҪԤװ.
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
	//�õ�ҳ�������� ���� ����ҳ���� ÿ��ҳ������ʱ�Ĵ�С �Ѿ�ʹ�õ�ҳ
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

	//pstrOutFile ����false ʱ�ұ���Ϊ�����ļ�ʱ��Ч, Ϊ����ʱ����ʱ�����Ϊ���ļ�����,���ڲ�����
	bool SaveFile(LPCTSTR lpszFileName,CTString *pstrOutFile);

	//##ModelId=3DCB52A4006F
	bool SetMaxPageCount(int count);


	CPage* GetCurPage();

	//���õ�ǰ�ļ�¼�ţ��õ�ָ����¼�ŵ����ݣ�
	//��������ڴ�ҳ�У������GetNormalPage()�õ�һ�����õ�ҳ�

	//������Ƿ���Ҫж�ء�Ȼ�󷢳�ҳ���󣬲��ȴ����أ���Ϊ�Ƕ�
	//�̣߳����Ա����õ�WaitForSingleObject API������
	//
	//ע�⣺�����Ҫ���������Ѿ�����Ķ����У����Ƕ��ģ������
	//�޸ĸö��ж�Ӧ���Ҫ�󷵻�WaitForSingleObject��
	//##ModelId=3DCB59260273
	bool SetCurRecNo(int RecNo);

	//�õ����û��ʹ�õ����ݼ�
	//##ModelId=3D056C3603E6
	//CPage* GetRecentNoUse();//#_�޸� 2002-11-12 $ 16:56:44 zlq

	
	//CPage* GetRecentNoUseNoModify(); //#_�޸� 2002-11-12 $ 16:56:46 zlq
	//##ModelId=3D056C8F01B3
	int GetUsePageCount();

	//�õ���¼�Ŷ�Ӧ�����������ڵ�״̬��
	//�����¼���״̬��
	//��������״̬���ȴ�����װ�룬���ݵ��룬�ȴ�����д�أ�����
	//д��״̬��û��װ�루�������ȴ�����װ�룬���ݵ��룬�ȴ���
	//��д�أ�����д��״̬��
	//(�ο�ҳ��״̬��
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


	//���AdjustFakeRecNo������
	//ע�⣺����ҳ�棬���ܴ���ʲô״̬
	void UnLoad(CPage* pPage,bool IsNeedFeedback=false);

	//�õ��߼���¼����α��¼�ŵ�ƫ��
	int GetLogicToFake(int LogicRecNo);
    CPage* GetPhysicalEmptyPage();


	int Next();
	int Prior();

	int MoveBy(int iSkipLen ,CFromPos MoveFlags);

	int First();
	int Last();
	//���ص�ǰ��¼��
	 int GetRecNo();
	 bool Delete();
	 bool Cancel();


	//���ؼ�¼���ĸ���������û��װ���ڴ�ļ�¼
	 int GetRecordCount();


	//���õ�ǰ����¼�����е��ֶε�������Ϣ��һ��ָ���Ļ�������
	//
	//������֯��ָʾ��Ϣ���֣������ֶΣ�ÿ���ֶ�һ�ֽڣ�+���ݲ

	//�֣������ֶΣ�����ĳЩ�ֶ�Ҳû�����ݣ�ǰ���ܹ����ʱ��
	//
	//�������ݳ���
	//##ModelId=3D054ECF01DA
	 int SetRecord(LPBYTE lpData);

	//�Ե�ǰ�ļ�¼���ݸ�ֵ���������ݼ��ڴ�֮ǰӦ��ͨ�����ڱ༭
	//�����״̬������append,edit�ȷ�������
	//�����ֶ�������ֶε�LoadDataFromBuffer
	//
	//�����Ƿ�װ�سɹ�������Ч���������ݴ�С
	//##ModelId=3D0020C401A9
	 bool GetRecord(LPBYTE lpData, int& count);

	//�Ե�ǰ�ļ�¼���ݸ�ֵ���������ݼ��ڴ�֮ǰӦ��ͨ�����ڱ༭
	//�����״̬������append,edit�ȷ������������޸�ָ��
	//�����ֶ�������ֶε�LoadDataFromBuffer
	//##ModelId=3D0553C00247
	 bool GetRecord(LPBYTE& lpData);

	 //bool SetPagePosition(CPage* pPage ,int nPos);

	/*
	����ʱ��ҳ�Ѿ���Ч�����Ѿ�ж�ع���
	1��1���ñ�ҳ�漯��UnLoad�������ú����ڲ���Ҫ��������ҳ���α��¼��ַ��
	1��2���߳̽���ȴ�״̬
	*/
	 void WaitForUnLoad(CPage* pPage);

	/*����ʱ��ҳ�Ѿ���Ч
	1��1���ø�ҳ��load����
	1��2���߳̽���ȴ�״̬
	*/
	 void WaitForLoad(CPage* pPage,int logicRecNo, int count );

	/*
	���ݴ���ļ�¼�ţ��õ����ڵ�ҳ�������ڴ����ڲ�װ�벢���ظ�ҳ��
	ע�⣺�ú���������ҳ��˳��ĵ���
	���أ���¼���ڵ�ҳ

	1.�Ȳ�ѯ�ü�¼��״̬QueryRecNoState
	1��1 �������ڴ��У��򷵻ظ�ҳ
	1��2 �ڶ����У�����WaitForInQueue(pPage : CPage*)���ظ�ҳ
	1��3 ��������ڴ�ҳ�У������GetPhysicalEmptyPage�õ�һ�����õĿ�ҳ,WaitForLoad�õ���ҳ
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

