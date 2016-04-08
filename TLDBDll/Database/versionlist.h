// VersionList.h: interface for the CVersionList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERSIONLIST_H__4363C8C7_9E79_4705_8D81_2CE15C236D59__INCLUDED_)
#define AFX_VERSIONLIST_H__4363C8C7_9E79_4705_8D81_2CE15C236D59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//typedef  CList<DWORD,DWORD> DWORDList;

class AFX_EXT_CLASS CVersionList  
{
public://DataMember

	//�汾����,Ԫ��Ϊָ��CDWordArray��ָ��
	//CPtrList m_VerDataList;
	CPtrArray m_VerDataArray;
public://FunctionMember
	
	//Construct
	CVersionList();
	//Destruct
	virtual ~CVersionList();	

	//��¡�ֶμ�m_FieldList�������ֶ�.�Լ��汾��.
	void Clone(CVersionList* pVersionList );

	//�õ�ָ���İ汾������
	CDWordArray* GetVersion(int VerNo);
	
	//�õ��汾����
	int	GetVersionCount();

	//���һ���汾������Ϊ��һ���汾�Ŀ�¡
	int AddNewVersion();

	//����ֶμ�m_FieldList�������ֶ�.�Լ��汾��.
	void Clear();
};

#endif // !defined(AFX_VERSIONLIST_H__4363C8C7_9E79_4705_8D81_2CE15C236D59__INCLUDED_)
