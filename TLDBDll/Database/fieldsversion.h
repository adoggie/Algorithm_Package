// FieldsVersion.h: interface for the CFieldsVersion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELDSVERSION_H__3E5FA6FA_1275_4C86_B383_F5378A466AA0__INCLUDED_)
#define AFX_FIELDSVERSION_H__3E5FA6FA_1275_4C86_B383_F5378A466AA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "versionlist.h"
#include "cfield.h"

class CFields;

class CFields;

class AFX_EXT_CLASS CFieldsVersion  
{
private://DataMember

	//绝对的字段集.
	//CTypedPtrArray<CPtrArray,CField*> m_FieldAllArray;//#_S 2005-9-1 zlq	
	CPtrArray m_FieldAllArray;
	//版本链表.
	CVersionList m_VersionList;

	CFields* m_pFields;

public://FunctionMember
	int GetAbsoluteFieldCount();
	
	//Construct
	CFieldsVersion();
	//Destruct
	virtual ~CFieldsVersion();

	CFields* SetFields(CFields* pFields);

	//根据字段在字段链表中的索引号确定该字段最后存在时的版本.
	int GetLastVersionByIndex(int nIndex);

	//根据字段在字段链表中的索引号确定该字段最早存在时的版本.
	int GetFirstVersionByIndex(int nIndex); 

	//设置版本,默认为新版本.用来装载旧版本数据.但不生成新的空间.
	//也不初始化其他版本字段的指针
	void SetVersion(CPtrArray& FieldArray, int nVersion = -1);

	//判断当前字段是否在最新字段中.
	bool IsConvertToLastVersion(CField* pField);

	//得到内存里有提交的数据的最新版本。
	//它小于等于GetLastVersion（），但相差最多为1
	//返回: 版本号
	int GetLastVersion();
	
	//删除一个字段
	//检查m_LastPosVersion与GetLastVersion的一致性决定是否需要从上面版本创建一个新版本，以及初始化的问题
	void Remove(CField* pField);

	//删除在nIndex处的一个字段
	//检查m_LastPosVersion与GetLastVersion的一致性决定是否需要从上面版本创建一个新版本，以及初始化的问题
	bool Delete(int nIndex);

	//在nIndex处插入一个字段
	//检查m_LastPosVersion与GetLastVersion的一致性决定是否需要从上面版本创建一个新版本，以及初始化的问题
	void Insert(CField* pField, int nIndex); 

	//字段类型修改
	//检查m_LastPosVersion与GetLastVersion的一致性决定是否需要从上面版本创建一个新版本，以及初始化的问题
	void Modify(int nIndex,CField* pOldField, CField* pNewField);
	
	//数据克隆，根据传入的字段链表样板克隆本字段链表的基本信息，内部调用个字段的克隆方法
	void Clone(CFieldsVersion* pFieldsVersion);


	//清除字段集m_FieldList中所有字段.以及版本链
	void Clear();

	//将所有的字段的数据指针设置为NULL
	void ClearAllVersionPointer();

	//根据绝对的字段集的索引号来得到字段.
	CField* GetFieldByAbsolutePos(int nIndex);

	//添加一个字段,从末尾添加
	void Add(CField* pField);
	//字段移动
	//检查m_LastPosVersion与GetLastVersion的一致性决定是否需要从上面版本创建一个新版本，以及初始化的问题
	void Mov(int nFrom, int nTo);
};

#endif // !defined(AFX_FIELDSVERSION_H__3E5FA6FA_1275_4C86_B383_F5378A466AA0__INCLUDED_)
