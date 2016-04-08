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

//***  ���Ͷ���  ********
typedef __int64 LONG64;
typedef int INT32;
typedef int (__stdcall *FARPROC)();


class TCUtil  
{
public:
	TCUtil();
	virtual ~TCUtil();
	static void StringToArray(std::string strSource,std::vector <std::string> &strAry,std::string strSeparator );
	//�������ַ�����Ϊһ�У���ɾ�����б�־
	static std::string RemoveReturn(std::string sSour);
#ifdef WINPLAT
	//��UTF8 ת��Ϊ ansi  ,nUtfLenΪ������ĩβ0
	static std::string UtfToAnsi(const char *pUTFBuf,int nUtfLen);
	//��ansi ת��Ϊ UTF8  ,nUtfLenΪ������ĩβ0 ʱ�ĳ���
	static char* AnsiToUtf(std::string strSour,int &nUtfLen);
	static int GetUtfLen(std::string strSour);
	//�õ���̬���ӿ⺯���ĵ�ַ
	static FARPROC GetProc(std::string sDllName,std::string sProcName);
#endif

};

#ifdef _UNIX
void GetLocalTime(SYSTEMTIME *st);
#endif

#endif // !defined(AFX_UTIL_H__A92A24B1_9D29_4529_A797_5816C6B89906__INCLUDED_)
