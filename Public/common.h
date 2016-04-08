

#if !defined(Common_2334523432333_3433_3421188A)
#define Common_2334523432333_3433_3421188A

//******** 当前系统的定义 ************
//#define WINPLAT //  定义当前为Windows平台
//#define LinuxPLAT //  定义当前为Linux平台
//************************************
//#include "com.h"

#ifndef _UNIX
	#define WINPLAT
#endif

#define CHAR_SLASH '/'
#ifdef WINPLAT //  Windows平台专用
	//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#define _AFX_DOUBLE double
	#include <windows.h>
	#define AFX_EXT_CLASS __declspec( dllexport )
	#define TEMPLAT_T 
	#define T_T
	#define T_S Type

	#include <io.h>
	#include <stdlib.h>
#endif

#ifdef _UNIX
	//===uc 未完成
	#include <unistd.h>
	#define AFX_EXT_CLASS  
	#define _MSC_VER 555
	#define TEMPLAT_T template <class T>
	#define T_T <T>
	#define T_S T
	#define _declspec(dllexport)
	#define __declspec(dllexport)
	#define FALSE false
	#define TRUE true
	
	#define _access	access
	#define __stdcall
	#define lstrlen	strlen
	#define _getcwd	getcwd
	#define _stricmp	strcasecmp
	#define _ftime	ftime
	#define _timeb    timeb
#endif


#define _T(x)       x
#define MAX_PATH 260
#define _MAX_PATH MAX_PATH
//===temp  临时结构
typedef unsigned short WORD;

#ifdef _UNIX //  Linux平台专用
	typedef struct _SYSTEMTIME {
		WORD wYear;
		WORD wMonth;
		WORD wDayOfWeek;
		WORD wDay;
		WORD wHour;
		WORD wMinute;
		WORD wSecond;
		WORD wMilliseconds;
	} SYSTEMTIME;


	struct bb{
		int aa;
		int b2;
	};
	typedef unsigned int DWORD;
	typedef bb FILETIME;
	//typedef bb SYSTEMTIME;
	typedef unsigned int COLORREF;
	typedef long long INT64;
	typedef INT64 __int64 ;
	typedef unsigned long long UINT64;
	typedef bool BOOL;
	typedef void *HANDLE;
	typedef long 	LONG;
	typedef void* HINSTANCE;
	typedef void* HMODULE;

	typedef struct tagSIZE { 
	    LONG cx; 
	    LONG cy; 
	} SIZE; 

#endif




typedef unsigned int UINT;
typedef unsigned int POSITION;
typedef const char*LPCSTR, *PCSTR;
typedef LPCSTR LPCTSTR;
typedef  unsigned char            *LPBYTE;
typedef  unsigned char byte;



//typedef unsigned int COleDateTime;



typedef unsigned int POSITION;
typedef const char*LPCSTR, *PCSTR;
typedef LPCSTR LPCTSTR;
typedef  unsigned char            *LPBYTE;
typedef  unsigned char byte;



 
typedef unsigned int COleDateTime;
//typedef unsigned int DWORD;
typedef unsigned short WORD;


#define _T(x)       x
#define AFX_INLINE inline /*__forceinline*/
#define MAX_PATH 260

typedef unsigned int  UINT;
#define PI 3.14159265358979
#define MAX_COUNT 300
#define MAX_DEGREE 10000
#define DOUBLE_ZERO 1e-20
#define EPS_ACCUR 1e-10
#define MAX_ITERATETIME 3000

#define MIN(a,b) ((a)<(b))?(a):(b)
#define MAX(a,b) ((a)<(b))?(b):(a)


struct CPoint{
	CPoint()
	{
		x = 0.0;
		y = 0.0;
	};
	CPoint(double a, double b)
	{
		x = a;
		y = b;
	};
	double x;
	double y;
};





#endif
