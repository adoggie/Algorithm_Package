// DriverManager.h: interface for the CDriverManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERMANAGER_H__FDD7D781_33C9_4028_9720_036D472EB715__INCLUDED_)
#define AFX_DRIVERMANAGER_H__FDD7D781_33C9_4028_9720_036D472EB715__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CDriverBase;
class CFields;
class CDriverManager  
{
public:
	CTString GetExtName(CTString strFileName);

	CDriverManager();
	virtual ~CDriverManager();
	bool LoadFile(LPCTSTR lpszFileName,CDriverBase** pDriver,CFields* pFields);

};

#endif // !defined(AFX_DRIVERMANAGER_H__FDD7D781_33C9_4028_9720_036D472EB715__INCLUDED_)
