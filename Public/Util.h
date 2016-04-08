// Util.h: interface for the CUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTIL_H__A92A24B1_9D29_4529_A797_5816C6B89906__INCLUDED_)
#define AFX_UTIL_H__A92A24B1_9D29_4529_A797_5816C6B89906__INCLUDED_


//#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "common.h"

//using namespace std ;

//***  类型定义  ********
typedef __int64 LONG64;
typedef int INT32;
typedef int (__stdcall *FARPROC)();


class TCUtil  
{
public:
	TCUtil();
	virtual ~TCUtil();
	static void StringToArray(std::string strSource,std::vector <std::string> &strAry,std::string strSeparator );
	//将多行字符串变为一行，即删除分行标志
	static std::string RemoveReturn(std::string sSour);
#ifdef WINPLAT
	//将UTF8 转换为 ansi  ,nUtfLen为不包含末尾0
	static std::string UtfToAnsi(const char *pUTFBuf,int nUtfLen);
	//将ansi 转换为 UTF8  ,nUtfLen为不包含末尾0 时的长度
	static char* AnsiToUtf(std::string strSour,int &nUtfLen);
	static int GetUtfLen(std::string strSour);
	//得到动态连接库函数的地址
	static FARPROC GetProc(std::string sDllName,std::string sProcName);
#endif

};

#ifdef _UNIX
void GetLocalTime(SYSTEMTIME *st);
#endif

#endif // !defined(AFX_UTIL_H__A92A24B1_9D29_4529_A797_5816C6B89906__INCLUDED_)
