// tlBase.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "Interface.h"
#include "../Public/Packet.h"
#include "Manager.h"
#ifdef _UNIX
typedef unsigned char BYTE;
#define __declspec(dllexport)
#endif

#ifndef _UNIX
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

/*
Java_com_tenly_mkw_model_bridge_Interface_callbytebase
Java_bridge_Interface_callbytebase
*/
extern "C"   JNIEXPORT jbyteArray JNICALL Java_bridge_Interface_callbytebase
  (JNIEnv *pEnv, jobject obj, jbyteArray jbary)
{
	jboolean jbcopy = false;
	
	int nLen = pEnv->GetArrayLength(jbary);
	BYTE *pInBuf = new BYTE[nLen+2];
	{
		BYTE *value = (BYTE*)pEnv->GetByteArrayElements(jbary ,&jbcopy);
		memset(pInBuf,0,nLen+2);
		memcpy(pInBuf,value,nLen);
		pEnv->ReleaseByteArrayElements(jbary,(jbyte *)value,0);//更新
	}

	//输入数据
	TCPacket pgIn;
	pgIn.Import(( char*)pInBuf);
	delete []pInBuf;
	TCManager mg;
	TCPacket pgOut;
	//pgOut.SetPacketName("from c++ code");
	mg.DealPacket(pgIn,pgOut);
	//******************* 参数 ***************************
	int nRstLen = pgOut.GetPacketLength();
	BYTE*pSour =new BYTE[nRstLen];
	pgOut.Export((char*)pSour,nRstLen);
	//***********************************************
	jbyteArray jrst =pEnv->NewByteArray(nRstLen);//申请
	BYTE *valueNew = (BYTE*)pEnv->GetByteArrayElements( jrst,0 );
	//返回参数进行操作,对输入参数进行操作也是如此
	memcpy(valueNew,pSour,nRstLen);
	pEnv->ReleaseByteArrayElements(jrst,(jbyte*)valueNew,0);//更新
	delete []pSour;
	/*{//回调函数
		jclass cls = pEnv->GetObjectClass( obj);
		
		jmethodID mid = pEnv->GetMethodID( cls, "callback", "(I)V");
		if (mid == 0) {
			return jrst;
		}
		printf("In C, depth = %d, about to enter Java\n", 2);
		pEnv->CallVoidMethod( obj, mid, 2);
		printf("In C, depth = %d, back from Java\n", 2);
	}//*/
	return jrst;
}
#endif
/*
Java_com_tenly_mkw_model_bridge_Interface_callbytebase
Java_bridge_Interface_callbytebase
*/
#ifdef _UNIX
// :)  hahhahahhahahhahahahhaha
#include "algsvc.h"

extern "C"   JNIEXPORT jbyteArray JNICALL Java_com_tenly_mkw_model_bridge_Interface_callbytebase
  (JNIEnv *pEnv, jobject obj, jbyteArray jbary)
{
	jboolean jbcopy = false;
	
	int nLen = pEnv->GetArrayLength(jbary);
	BYTE *pInBuf = new BYTE[nLen+2];
	{
		BYTE *value = (BYTE*)pEnv->GetByteArrayElements(jbary ,&jbcopy);
		memset(pInBuf,0,nLen+2);
		memcpy(pInBuf,value,nLen);
		pEnv->ReleaseByteArrayElements(jbary,(jbyte *)value,0);//更新
	}
	//-----------------------------
	DataBlock req,resp;
	req.assign(pInBuf,pInBuf+nLen+2);
	AlgoCommProxy proxy(AlgoServerHost,AlgoServerPort);
	proxy.Send(req,resp);
	//----------------------------
	jbyteArray jrst =pEnv->NewByteArray(resp.size());//申请
	BYTE *valueNew = (BYTE*)pEnv->GetByteArrayElements( jrst,0 );
	memcpy(valueNew,&resp.at(0),resp.size());
	pEnv->ReleaseByteArrayElements(jrst,(jbyte*)valueNew,0);//更新
	return jrst;	
}
#else 
extern "C"   JNIEXPORT jbyteArray JNICALL Java_com_tenly_mkw_model_bridge_Interface_callbytebase
  (JNIEnv *pEnv, jobject obj, jbyteArray jbary)
{
	jboolean jbcopy = false;
	
	int nLen = pEnv->GetArrayLength(jbary);
	BYTE *pInBuf = new BYTE[nLen+2];
	{
		BYTE *value = (BYTE*)pEnv->GetByteArrayElements(jbary ,&jbcopy);
		memset(pInBuf,0,nLen+2);
		memcpy(pInBuf,value,nLen);
		pEnv->ReleaseByteArrayElements(jbary,(jbyte *)value,0);//更新
	}

	//输入数据
	TCPacket pgIn;
	pgIn.Import(( char*)pInBuf);
	delete []pInBuf;
	TCManager mg;
	TCPacket pgOut;
	//pgOut.SetPacketName("from c++ code");
	mg.DealPacket(pgIn,pgOut);
	//printf("%s \r\n",pgIn.GetItemName(0).c_str());
	//printf("%s \r\n",pgIn.GetPacketName().c_str());

	
	//MessageBox(NULL,str.c_str(),"adfa",MB_OK);
	//******************* 参数 ***************************
	int nRstLen = pgOut.GetPacketLength();
	BYTE*pSour =new BYTE[nRstLen];
	pgOut.Export((char*)pSour,nRstLen);
	//***********************************************
	jbyteArray jrst =pEnv->NewByteArray(nRstLen);//申请
	BYTE *valueNew = (BYTE*)pEnv->GetByteArrayElements( jrst,0 );
	//返回参数进行操作,对输入参数进行操作也是如此
	memcpy(valueNew,pSour,nRstLen);
	pEnv->ReleaseByteArrayElements(jrst,(jbyte*)valueNew,0);//更新
	delete []pSour;
	/*{//回调函数
		jclass cls = pEnv->GetObjectClass( obj);
		
		jmethodID mid = pEnv->GetMethodID( cls, "callback", "(I)V");
		if (mid == 0) {
			return jrst;
		}
		printf("In C, depth = %d, about to enter Java\n", 2);
		pEnv->CallVoidMethod( obj, mid, 2);
		printf("In C, depth = %d, back from Java\n", 2);
	}//*/
	return jrst;
}

#endif

#ifndef _UNIX
//将UTF8 转换为 ansi  ,nUtfLen为不包含末尾0
string UtfToAnsi(const char *pUTFBuf,int nUtfLen)
{
	int nWideLen = MultiByteToWideChar(CP_UTF8, 0,pUTFBuf, nUtfLen, NULL, NULL);
	LPWSTR lpszW = new WCHAR[nWideLen];
	MultiByteToWideChar(CP_UTF8, 0,  pUTFBuf, nUtfLen, lpszW, nWideLen);
	//*************************************
	int nAnsiLen = WideCharToMultiByte(CP_ACP,0,lpszW,nWideLen,NULL,NULL,0,0);
	char* pACP = new char[nAnsiLen+2];
	ZeroMemory(pACP,nAnsiLen+2);
	WideCharToMultiByte(CP_ACP,0,lpszW,-1,(char*)pACP,nAnsiLen,0,0);
	string str = pACP;
	delete []lpszW;
	delete []pACP;
	return str;   
}


//将ansi 转换为 UTF8  ,nUtfLen为不包含末尾0 时的长度
char* AnsiToUtf(string strSour,int &nUtfLen)
{
	int nWideLen = MultiByteToWideChar(CP_ACP, 0,(const char*)strSour.c_str(),strSour.size(), NULL, NULL);
	LPWSTR lpszW = new WCHAR[nWideLen];
	MultiByteToWideChar(CP_ACP, 0,(const char*)strSour.c_str(), strSour.size(), lpszW, nWideLen);
	//*************************************
	nUtfLen = WideCharToMultiByte(CP_UTF8,0,lpszW,nWideLen,NULL,NULL,0,0);
	char*m_pUtf = new char[nUtfLen+2];
	ZeroMemory(m_pUtf,nUtfLen+2);
	WideCharToMultiByte(CP_UTF8,0,lpszW,-1,(char*)m_pUtf,nUtfLen,0,0);
	delete []lpszW;
	return m_pUtf;
}


extern "C"  __declspec(dllexport) jstring WINAPI   
Java_bridge_Interface_ChangeStr(JNIEnv *pEnv , jclass*jcla,jstring s)
{
	/* Obtain a C-copy of the Java string */
	const char *pchar = pEnv->GetStringUTFChars( s, 0);
	/* process the string */
	//...
	int nLen = pEnv->GetStringLength(s);//注意得到的长度不是子节长度,是字符长度
	 nLen = pEnv->GetStringUTFLength(s);//是子节长度
	
	string str =UtfToAnsi(pchar,nLen);
	//MessageBox(NULL,str.c_str(),"",MB_OK);;
	int NewLen =0;
	char *pUtf = AnsiToUtf(str,NewLen);
	str =UtfToAnsi(pUtf,NewLen);
	//AfxMessageBox(str);
	//MessageBox(NULL,str.c_str(),"",MB_OK);;

	/* Now we are done with str */

	jstring rst = pEnv->NewStringUTF(pchar);

	pEnv->ReleaseStringUTFChars( s, pchar);
	//pEnv->ReleaseStringUTFChars( s, "zhongzzzzzd");
	
	return rst;
	//return ...frg
}
extern "C"  __declspec(dllexport) jbyteArray WINAPI   
Java_bridge_Interface_GetBYTE(JNIEnv *pEnv , jclass*jcla, jbyteArray jbary)
{
	jboolean jbcopy = false;
	
	int nLen = pEnv->GetArrayLength(jbary);
	BYTE *pInBuf = new BYTE[nLen+2];
	{
		BYTE *value = (BYTE*)pEnv->GetByteArrayElements(jbary ,&jbcopy);
		memset(pInBuf,0,nLen+2);
		memcpy(pInBuf,value,nLen);
		pEnv->ReleaseByteArrayElements(jbary,(jbyte *)value,0);//更新
	}

	//输入数据

	//( char*)pInBuf
	delete []pInBuf;
	
	//******************* 参数 ***************************
	string stt = "返回参数进行操作";
	int nRstLen = stt.size();
	BYTE*pSour =(BYTE*)stt.c_str();
	
	//***********************************************
	jbyteArray jrst =pEnv->NewByteArray(nRstLen);//申请
	BYTE *valueNew = (BYTE*)pEnv->GetByteArrayElements( jrst,0 );
	//返回参数进行操作,对输入参数进行操作也是如此
	memcpy(valueNew,pSour,nRstLen);
	pEnv->ReleaseByteArrayElements(jrst,(jbyte*)valueNew,0);//更新
	//delete []pSour;

	return jrst;
}
#endif

