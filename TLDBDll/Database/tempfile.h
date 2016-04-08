// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TEMPFILE_3D059F0C010E_INCLUDED
#define _INC_TEMPFILE_3D059F0C010E_INCLUDED

#include "cpage.h"
#include "cpages.h"
#include "../../Public/TCLFile.h"

class CFakeRecNos;
class TempFile 
: public TCLFile

{
	//☆☆☆☆☆☆☆☆ #_S 2005-5-10 zlq  
	BYTE*m_IOBuf;
	int m_nIOTotalLen;////////////


/*
		int m_nResizeNum;//访问超大记录条数的门限，大于此个数时，重新调整内存大小
		int m_nBufSize;//一般内存大小 1024
		int m_nLargeRecordNum;//大于m_nBufSize的记录的条数
		int m_nLargeSizeSum;//大于m_nBufSize的记录所有的尺寸总和*/
	
public:
	CTString GetFilePath();
	void _LoadPage(CPage* pPage ,INT64 FilePosition, int Offset,int nCurVersion, int count);
	long _SavePage(CPage* pPage,int nStartPos, int count,INT64 nFilePos=-1, int Offset=-1);
	CFakeRecNos * m_pFakeRecNos;

	TempFile(LPCTSTR lpszFileName, UINT nOpenFlags );

	virtual ~TempFile();
protected:

};

#endif /* _INC_TEMPFILE_3D059F0C010E_INCLUDED */

