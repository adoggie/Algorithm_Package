// Util.cpp: implementation of the TCUtil class.
//
//////////////////////////////////////////////////////////////////////


#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCUtil::TCUtil()
{

}

TCUtil::~TCUtil()
{

}

/*
*��Դ�ַ������ݷָ�����strSeparator���ָ�������ַ���������ȥ
*�������ָ�����
*/
void TCUtil::StringToArray(std::string strSource, std::vector<std::string> &strAry, std::string strSeparator)
{
	strAry.clear();
	int nLeft = 0;
	int nRight = 0;
	nRight = strSource.find(strSeparator,nLeft);
	while(nRight !=-1)
	{
		std::string strtemp ="";
		strAry.push_back(strtemp.assign(strSource.c_str()+nLeft,nRight -nLeft));
		nLeft = nRight+strSeparator.length();
		nRight = strSource.find(strSeparator,nLeft);
	}
	if (nLeft < strSource.length())
	{
		std::string strtemp ="";
		strAry.push_back(strtemp.assign(strSource.c_str()+nLeft,strSource.length() -nLeft));
	}
	/*
	for (int i=0;i<strAry.size(); i++)
	{
		std::string str = strAry[i];
		printf("%s\r\n",str.c_str());
	}//*/
}

#ifdef WINPLAT
//��UTF8 ת��Ϊ ansi  ,nUtfLenΪ������ĩβ0
std::string TCUtil::UtfToAnsi(const char *pUTFBuf,int nUtfLen)
{
	int nWideLen = MultiByteToWideChar(CP_UTF8, 0,pUTFBuf, nUtfLen, NULL, NULL);
	LPWSTR lpszW = new WCHAR[nWideLen];
	MultiByteToWideChar(CP_UTF8, 0,  pUTFBuf, nUtfLen, lpszW, nWideLen);
	//*************************************
	int nAnsiLen = WideCharToMultiByte(CP_ACP,0,lpszW,nWideLen,NULL,NULL,0,0);
	char* pACP = new char[nAnsiLen+2];
	ZeroMemory(pACP,nAnsiLen+2);
	WideCharToMultiByte(CP_ACP,0,lpszW,-1,(char*)pACP,nAnsiLen,0,0);
	std::string str = pACP;
	delete []lpszW;
	delete []pACP;
	return str;   
}


//��ansi ת��Ϊ UTF8  ,nUtfLenΪ������ĩβ0 ʱ�ĳ���
char* TCUtil::AnsiToUtf(std::string strSour,int &nUtfLen)
{
	int nWideLen = MultiByteToWideChar(CP_ACP, 0,strSour.c_str(),strSour.length(), NULL, NULL);
	LPWSTR lpszW = new WCHAR[nWideLen];
	MultiByteToWideChar(CP_ACP, 0,strSour.c_str(), strSour.length(), lpszW, nWideLen);
	//*************************************
	nUtfLen = WideCharToMultiByte(CP_UTF8,0,lpszW,nWideLen,NULL,NULL,0,0);
	char*m_pUtf = new char[nUtfLen+2];
	ZeroMemory(m_pUtf,nUtfLen+2);
	WideCharToMultiByte(CP_UTF8,0,lpszW,-1,(char*)m_pUtf,nUtfLen,0,0);
	delete []lpszW;
	return m_pUtf;
}
//�õ���ansi ת��Ϊ UTF8��ĳ��ȣ�
int TCUtil::GetUtfLen(std::string strSour)
{
	int nWideLen = MultiByteToWideChar(CP_ACP, 0,strSour.c_str(),strSour.length(), NULL, NULL);
	LPWSTR lpszW = new WCHAR[nWideLen];
	MultiByteToWideChar(CP_ACP, 0,strSour.c_str(), strSour.length(), lpszW, nWideLen);
	//*************************************
	int nUtfLen = WideCharToMultiByte(CP_UTF8,0,lpszW,nWideLen,NULL,NULL,0,0);
	delete []lpszW;
	return nUtfLen;
}
FARPROC TCUtil::GetProc(std::string sDllName, std::string sProcName)
{
	FARPROC pl = NULL;
	HINSTANCE hand = LoadLibrary(sDllName.c_str());
	if (hand)
	{
		pl = GetProcAddress(hand,sProcName.c_str());
	}
	return pl;
}
#endif


//�������ַ�����Ϊһ�У���ɾ�����б�־
std::string TCUtil::RemoveReturn(std::string sSour)
{
	int nLen = sSour.length();
	char *pBuf = new char[nLen+2];
	memset(pBuf,0,nLen+2);
	int nDestPos=0;
	const char *pSour = sSour.c_str();
	for (int i=0;i<nLen;i++)
	{
		if (!(*(pSour+i) =='\r' ||*(pSour+i) =='\n'))
		{
			*(pBuf+nDestPos) = *(pSour+i);
			nDestPos++;
		}
	}
	std::string str = pBuf;
	return str;
}

#ifdef _UNIX
void GetLocalTime(SYSTEMTIME *st){
	time_t ct = time(0);
	struct tm * ptm = localtime(&ct);
	st->wYear = ptm->tm_year+1900;
	st->wMonth = ptm->tm_mon+1;
	st->wDay = ptm->tm_mday;
	st->wHour = ptm->tm_hour;
	st->wMinute = ptm->tm_min;
	st->wSecond = ptm->tm_sec;
}
#endif


