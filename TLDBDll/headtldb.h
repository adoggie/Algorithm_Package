// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef _HEADTLDB_H
#define _HEADTLDB_H
#include <limits>
#include "../Public/common.h"
#include "../Public/TString.h"
#include <assert.h>
#define ODBC_DllName "BKPumper.dll"
#define OLE_DllName "DataPumper.dll"
#define _IncreaseNum 1000 //字段增加的的大小
#ifdef WINPLAT //  Windows平台专用
	#define NULLValue _Inf._D //空值定义
#else          //  Linux平台专用
	#define NULLValue (1.7976931348623158e+308) //空值定义//===uc 未完成
#endif

struct CShowData
{
	bool bIsUse;//指示是否有效
	DWORD dwStyle;
	//指示参数有效. 1,无交互的对话框,表示仅 strShowName有效.其它为作处理,
	//   2,表示有交互的对话框,strShowName和dwReserved有效 dwReserved 为0,表示没有停止,1,终止
	
	CTString strShowName;//显示进度等信息
	DWORD dwReserved;
};


using namespace std;

// Insert your headers here
/*
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
*/
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#pragma warning(disable:4251)
#endif // !defined(AFX_STDAFX_H__BB09F66B_3A32_4470_83F7_D7FE5589AE6B__INCLUDED_)
