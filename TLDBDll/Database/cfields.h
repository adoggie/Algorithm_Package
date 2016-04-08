// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFIELDS_3CF32E040212_INCLUDED
#define _INC_CFIELDS_3CF32E040212_INCLUDED

#include "cfield.h"
#include "cdataset.h"

#include "cintfield.h"

#include "cbooleanfield.h"
#include "ccurrencyfield.h"
#include "cdatefield.h"
#include "cdoublefield.h"
#include "cstringfield.h"
#include "fieldsversion.h"

//����ӵ����Ϊ���ݼ�
class CFlatFile;
//##ModelId=3CF32E040212
class CPages;
class AFX_EXT_CLASS CFields 
 
{
	CPtrArray m_FieldArray;	

public:

	int GetAbsoluteFieldCount();
	CPages* m_pPages;//###_Mod 2002-9-5 zlq ===uc �����Ϊ˽��
	CFields(CPages* pOwner);
	~CFields();

	CPage* m_pOldDataSet;
	CPage* m_pDataSet;

	//��ǰ�汾
	int m_nCurVersion;

	//�ڴ������ύ�����ݵ����°汾
	int m_nLastPosVersion;

	CFieldsVersion m_FieldVersion;

	//���һ���ֶ�	
	void Add(CField* pField);

	//Finds a field based on a specified field name.	
	CField* FieldByName(CTString FieldName);

	void GotoBookmark();
	void GetBookmark();	

	void Clear();
	

	//���ݴ�����ֶ�����������Ӧ���ֶ�����
	CField* NewFieldType(CFields* pOwner ,CField*clone);
	CField* NewFieldType(CFields* pOwner ,CFieldType chType);

	void Remove(CField* pField);

	//Call IndexOf to get the index for a field in the Fields 
	//array. Specify the field as the Field parameter. 
	//
	//The first field in the array has index 0, the second 
	//field has index 1, and so on. If a field is not in the 
	//Fields array, IndexOf returns -1.
	int IndexOf(CField* Field);

	//�ӻ������еõ��ֶε���Ϣ�������ɸ��ֶ�,���س���	
	int LoadFromBuffer(LPBYTE& lpData,int nFieldCount);

	
	//int SaveToBuffer(LPBYTE& lpData);
	int SaveToBuffer(TCLFile *pFile);


	//���ݿ�¡�����ݴ�����ֶ����������¡���ֶ�����Ļ�����Ϣ
	//���ڲ����ø��ֶεĿ�¡����
	void Clone(CFields* pFields);

	//����ָ����ڴ����ݵõ������ֶ�����
	CFieldType GetFieldType(LPBYTE lpData);
	
	void   NewData() ; 
	
	CField* FieldByIndex(int index); 
	
	void DeleteData();

	long SaveDataToBuffer(LPBYTE lpData);

	bool LoadDataFromBuffer(LPBYTE& lpData, int& count);

	int QueryLength();
//private:
	int GetFieldCount();


	//����ֶμ�m_FieldList�������ֶ�,������а汾.
	void ClearAll(); 

	//ɾ����nIndex����һ���ֶ�.
	bool Delete(int nIndex );

	//��nIndex������һ���ֶ�.
	void Insert(CField* pField , int nIndex);

	//�ֶ������޸�
	void Modify(int nIndex, CField* pNewField);

	//�õ���ǰ���µİ汾,��������GetLastPostVersion�õ��İ汾��
	//�������GetLastPostVersion��������������Ϊ1
	//����: �汾��
	int GetLastVersion();

	//�õ��ڴ������ύ�����ݵ����°汾��
	//��С�ڵ���GetLastVersion��������������Ϊ1.
	//����: �汾��
	int GetLastPostVersion();

	//���ð汾,Ĭ��Ϊ�°汾.����װ�ؾɰ汾����.���������µĿռ�.
	void SetCurVersion(int nVersion = -1);

	//�õ���ǰ�ֶεİ汾,���㿪ʼ.
	int GetCurVersion();

	//�жϵ�ǰ�ֶ��Ƿ��������ֶ���.
	bool IsConvertToLastVersion(CField* pField);


	//�����µ��ֶκ�û��ԭ�����ֶμ�Ĳ����ȷ���Ƿ���Ҫ�����汾��
	//����
	//true ��Ҫ����
	//false ����Ҫ
	bool IsNeedUpdateVersion(int nIndex, CField* pNewField );

	//�����а汾���ֶε�����ָ������ΪNULL������CFieldsVersion�ĸ÷���ʵ��.
	void ClearAllVersionPointer();
	//�ֶ��ƶ�
	void Mov(int nFrom, int nTo);

};

#endif /* _INC_CFIELDS_3CF32E040212_INCLUDED */

