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
	static BOOL ReadFileToStr(CTString ReadFileName, std::string& ReadStr);//��ReadFileName�ļ�����ַ�������ReadStr��
	static BOOL WriteStrToFile(CTString WriteFileName, std::string WriteStr);//���ַ���WriteStrд��WriteFileName�ļ���
	static CTString GetSaveFileName(CTString OrgFileName, CTString &PlusStr);//�Զ�������Ҫ������ļ���
};

#endif // !defined(AFX_FILEREADWRITE_H__338F4B7F_BAF9_4513_8BC7_9CA84B9559A8__INCLUDED_)
