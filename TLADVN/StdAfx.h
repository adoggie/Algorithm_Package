// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9D2E91AC_8248_4A25_A131_A3CBD69DE6E1__INCLUDED_)
#define AFX_STDAFX_H__9D2E91AC_8248_4A25_A131_A3CBD69DE6E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

/*
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
*/
#include "../Public/common.h"
#include "../Public/System.h"
#include "../Public/TString.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../Public/FileReadWrite.h"
#include "../cpknl/cpknl.h"
#include "../DataInterface/DataInterface.h"
#include "../Result/Result.h"
struct AFX_EXT_CLASS CShowData
{
	bool bIsUse;//指示是否有效
	DWORD dwStyle;
	//指示参数有效. 1,无交互的对话框,表示仅 strShowName有效.其它为作处理,
	//   2,表示有交互的对话框,strShowName和dwReserved有效 dwReserved 为0,表示没有停止,1,终止
	
	CTString strShowName;//显示进度等信息
	DWORD dwReserved;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9D2E91AC_8248_4A25_A131_A3CBD69DE6E1__INCLUDED_)
