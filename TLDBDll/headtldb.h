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
#define _IncreaseNum 1000 //�ֶ����ӵĵĴ�С
#ifdef WINPLAT //  Windowsƽ̨ר��
	#define NULLValue _Inf._D //��ֵ����
#else          //  Linuxƽ̨ר��
	#define NULLValue (1.7976931348623158e+308) //��ֵ����//===uc δ���
#endif

struct CShowData
{
	bool bIsUse;//ָʾ�Ƿ���Ч
	DWORD dwStyle;
	//ָʾ������Ч. 1,�޽����ĶԻ���,��ʾ�� strShowName��Ч.����Ϊ������,
	//   2,��ʾ�н����ĶԻ���,strShowName��dwReserved��Ч dwReserved Ϊ0,��ʾû��ֹͣ,1,��ֹ
	
	CTString strShowName;//��ʾ���ȵ���Ϣ
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
