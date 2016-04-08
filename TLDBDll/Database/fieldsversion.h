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

	//���Ե��ֶμ�.
	//CTypedPtrArray<CPtrArray,CField*> m_FieldAllArray;//#_S 2005-9-1 zlq	
	CPtrArray m_FieldAllArray;
	//�汾����.
	CVersionList m_VersionList;

	CFields* m_pFields;

public://FunctionMember
	int GetAbsoluteFieldCount();
	
	//Construct
	CFieldsVersion();
	//Destruct
	virtual ~CFieldsVersion();

	CFields* SetFields(CFields* pFields);

	//�����ֶ����ֶ������е�������ȷ�����ֶ�������ʱ�İ汾.
	int GetLastVersionByIndex(int nIndex);

	//�����ֶ����ֶ������е�������ȷ�����ֶ��������ʱ�İ汾.
	int GetFirstVersionByIndex(int nIndex); 

	//���ð汾,Ĭ��Ϊ�°汾.����װ�ؾɰ汾����.���������µĿռ�.
	//Ҳ����ʼ�������汾�ֶε�ָ��
	void SetVersion(CPtrArray& FieldArray, int nVersion = -1);

	//�жϵ�ǰ�ֶ��Ƿ��������ֶ���.
	bool IsConvertToLastVersion(CField* pField);

	//�õ��ڴ������ύ�����ݵ����°汾��
	//��С�ڵ���GetLastVersion��������������Ϊ1
	//����: �汾��
	int GetLastVersion();
	
	//ɾ��һ���ֶ�
	//���m_LastPosVersion��GetLastVersion��һ���Ծ����Ƿ���Ҫ������汾����һ���°汾���Լ���ʼ��������
	void Remove(CField* pField);

	//ɾ����nIndex����һ���ֶ�
	//���m_LastPosVersion��GetLastVersion��һ���Ծ����Ƿ���Ҫ������汾����һ���°汾���Լ���ʼ��������
	bool Delete(int nIndex);

	//��nIndex������һ���ֶ�
	//���m_LastPosVersion��GetLastVersion��һ���Ծ����Ƿ���Ҫ������汾����һ���°汾���Լ���ʼ��������
	void Insert(CField* pField, int nIndex); 

	//�ֶ������޸�
	//���m_LastPosVersion��GetLastVersion��һ���Ծ����Ƿ���Ҫ������汾����һ���°汾���Լ���ʼ��������
	void Modify(int nIndex,CField* pOldField, CField* pNewField);
	
	//���ݿ�¡�����ݴ�����ֶ����������¡���ֶ�����Ļ�����Ϣ���ڲ����ø��ֶεĿ�¡����
	void Clone(CFieldsVersion* pFieldsVersion);


	//����ֶμ�m_FieldList�������ֶ�.�Լ��汾��
	void Clear();

	//�����е��ֶε�����ָ������ΪNULL
	void ClearAllVersionPointer();

	//���ݾ��Ե��ֶμ������������õ��ֶ�.
	CField* GetFieldByAbsolutePos(int nIndex);

	//���һ���ֶ�,��ĩβ���
	void Add(CField* pField);
	//�ֶ��ƶ�
	//���m_LastPosVersion��GetLastVersion��һ���Ծ����Ƿ���Ҫ������汾����һ���°汾���Լ���ʼ��������
	void Mov(int nFrom, int nTo);
};

#endif // !defined(AFX_FIELDSVERSION_H__3E5FA6FA_1275_4C86_B383_F5378A466AA0__INCLUDED_)
