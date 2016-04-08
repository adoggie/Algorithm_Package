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

	//版本数组,元素为指向CDWordArray的指针
	//CPtrList m_VerDataList;
	CPtrArray m_VerDataArray;
public://FunctionMember
	
	//Construct
	CVersionList();
	//Destruct
	virtual ~CVersionList();	

	//克隆字段集m_FieldList中所有字段.以及版本链.
	void Clone(CVersionList* pVersionList );

	//得到指定的版本号数组
	CDWordArray* GetVersion(int VerNo);
	
	//得到版本个数
	int	GetVersionCount();

	//添加一个版本，内容为上一个版本的克隆
	int AddNewVersion();

	//清除字段集m_FieldList中所有字段.以及版本链.
	void Clear();
};

#endif // !defined(AFX_VERSIONLIST_H__4363C8C7_9E79_4705_8D81_2CE15C236D59__INCLUDED_)
