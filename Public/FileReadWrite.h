// FileReadWrite.h: interface for the CFileReadWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREADWRITE_H__338F4B7F_BAF9_4513_8BC7_9CA84B9559A8__INCLUDED_)
#define AFX_FILEREADWRITE_H__338F4B7F_BAF9_4513_8BC7_9CA84B9559A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "TString.h"


class CFileReadWrite
{
public:
	CFileReadWrite();
	virtual ~CFileReadWrite();
	static BOOL ReadFileToStr(CTString ReadFileName, std::string& ReadStr);//把ReadFileName文件里的字符串读到ReadStr里
	static BOOL WriteStrToFile(CTString WriteFileName, std::string WriteStr);//把字符串WriteStr写到WriteFileName文件里
	static CTString GetSaveFileName(CTString OrgFileName, CTString &PlusStr);//自动产生将要保存的文件名
};

#endif // !defined(AFX_FILEREADWRITE_H__338F4B7F_BAF9_4513_8BC7_9CA84B9559A8__INCLUDED_)
