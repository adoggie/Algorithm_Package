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

//它的拥有者为数据集
class CFlatFile;
//##ModelId=3CF32E040212
class CPages;
class AFX_EXT_CLASS CFields 
 
{
	CPtrArray m_FieldArray;	

public:

	int GetAbsoluteFieldCount();
	CPages* m_pPages;//###_Mod 2002-9-5 zlq ===uc 必须改为私有
	CFields(CPages* pOwner);
	~CFields();

	CPage* m_pOldDataSet;
	CPage* m_pDataSet;

	//当前版本
	int m_nCurVersion;

	//内存里有提交的数据的最新版本
	int m_nLastPosVersion;

	CFieldsVersion m_FieldVersion;

	//添加一个字段	
	void Add(CField* pField);

	//Finds a field based on a specified field name.	
	CField* FieldByName(CTString FieldName);

	void GotoBookmark();
	void GetBookmark();	

	void Clear();
	

	//根据传入的字段类型生成相应的字段类型
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

	//从缓冲区中得到字段的信息，并生成该字段,返回长度	
	int LoadFromBuffer(LPBYTE& lpData,int nFieldCount);

	
	//int SaveToBuffer(LPBYTE& lpData);
	int SaveToBuffer(TCLFile *pFile);


	//数据克隆，根据传入的字段链表样板克隆本字段链表的基本信息
	//，内部调用个字段的克隆方法
	void Clone(CFields* pFields);

	//根据指向的内存数据得到它的字段类型
	CFieldType GetFieldType(LPBYTE lpData);
	
	void   NewData() ; 
	
	CField* FieldByIndex(int index); 
	
	void DeleteData();

	long SaveDataToBuffer(LPBYTE lpData);

	bool LoadDataFromBuffer(LPBYTE& lpData, int& count);

	int QueryLength();
//private:
	int GetFieldCount();


	//清除字段集m_FieldList中所有字段,针对所有版本.
	void ClearAll(); 

	//删除在nIndex处的一个字段.
	bool Delete(int nIndex );

	//在nIndex处插入一个字段.
	void Insert(CField* pField , int nIndex);

	//字段类型修改
	void Modify(int nIndex, CField* pNewField);

	//得到当前最新的版本,它区别于GetLastPostVersion得到的版本。
	//它与等于GetLastPostVersion（），但相差最多为1
	//返回: 版本号
	int GetLastVersion();

	//得到内存里有提交的数据的最新版本。
	//它小于等于GetLastVersion（），但相差最多为1.
	//返回: 版本号
	int GetLastPostVersion();

	//设置版本,默认为新版本.用来装载旧版本数据.但不生成新的空间.
	void SetCurVersion(int nVersion = -1);

	//得到当前字段的版本,从零开始.
	int GetCurVersion();

	//判断当前字段是否在最新字段中.
	bool IsConvertToLastVersion(CField* pField);


	//检查更新的字段和没有原来的字段间的差别，以确定是否需要升级版本。
	//返回
	//true 需要更新
	//false 不需要
	bool IsNeedUpdateVersion(int nIndex, CField* pNewField );

	//将所有版本的字段的数据指针设置为NULL，调用CFieldsVersion的该方法实现.
	void ClearAllVersionPointer();
	//字段移动
	void Mov(int nFrom, int nTo);

};

#endif /* _INC_CFIELDS_3CF32E040212_INCLUDED */

