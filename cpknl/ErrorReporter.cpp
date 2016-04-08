// ErrorReporter.cpp: implementation of the CErrorReporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ErrorReporter.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// CTString -------------- Construction/Destruction
//////////////////////////////////////////////////////////////////////

static void Defaulthandler(Strerror err);

//unsigned int CTString::allocincr=8;

//void (* (CTString::Errorhandler))(Strerror) = Defaulthandler;

static void Defaulthandler(Strerror err)
{
	char ErrorString[80];
	switch(err)
	{
	case SE_ALLOC:
		sprintf(ErrorString,"字符串错误：内存错误");
		break;
	case SE_TOOLONG:
		sprintf(ErrorString,"字符串错误：字符串超过最大长度 %d ", UINT_MAX);
		break;
	case SE_INVALIDPARAM:
		sprintf(ErrorString,"字符串错误：无效参数 ");
		break;
	case SE_INVALIDINDEX:
		sprintf(ErrorString,"字符串错误：字符串长度越界 ");
		break;
	}
//	MessageBox(GetDesktopWindow(), ErrorString, "String Error", MB_ICONHAND|MB_SYSTEMMODAL);
//   PostAppMessage(GetCurrentTask(), WM_QUIT,0,0);

}

/*
void CTString::Shrink()
{
	char *temp;
	if((siz-len)>allocincr)
	{
		siz = ((len + allocincr)/allocincr)*allocincr;
		temp = new char [siz];
		if(temp != NULL)
		{
			strcpy(temp, txt);
			delete txt;
			txt = temp;
		}
		else
			Errorhandler(SE_ALLOC);
	}
}


CTString::CTString(int init)
{
    len = 0;
	siz = allocincr;
	txt = new char [siz];
	if(txt ==NULL)
		Errorhandler(SE_ALLOC);
	txt[0] = '\0';
}

CTString::CTString()
{
    len = 0;
	siz = allocincr;
	txt = new char [siz];
	if(txt ==NULL)
		Errorhandler(SE_ALLOC);
	txt[0] = '\0';
}

CTString::CTString(const CTString &str)
{
	len = str.len;
	siz = str.siz;
	txt = new char [siz];
	if(txt == NULL)
		Errorhandler(SE_ALLOC);
	strcpy(txt, str.txt);
}

CTString::CTString(const CTString &str)
{
	if(str=="")
	{
		len = 0;
		siz = allocincr;
	}
	else
	{
		len = str.GetLength();
		siz = ((len+allocincr)/allocincr)*allocincr;
	}

	txt = new char [siz];
	if(txt == NULL)
		Errorhandler(SE_ALLOC);
	if(len>0)
	    strcpy(txt, str);
	else
		txt[0] = '\0';
}

CTString::CTString(const char * cstr)
{
	if((cstr==NULL)||(cstr[0]=='\0'))
	{
		len = 0;
		siz = allocincr;
	}
	else
	{
		len = strlen(cstr);
		siz = ((len+allocincr)/allocincr)*allocincr;
	}

	txt = new char [siz];
	if(txt == NULL)
		Errorhandler(SE_ALLOC);
	if(len>0)
	    strcpy(txt, cstr);
	else
		txt[0] = '\0';
}

CTString::CTString(unsigned int count, char fillch)
{

	len = count;
	siz = ((count+allocincr)/allocincr)*allocincr;
	txt = new char [siz];
	if(txt == NULL)
		Errorhandler(SE_ALLOC);
    memset(txt, fillch, count);
		txt[count] = '\0';
}


CTString::~CTString()
{
	delete txt;
}

void CTString::SeterrorHandler(void(*userhandler)(Strerror))
{
    Errorhandler = userhandler;
}

CTString::operator char *()
{
	return txt;
}

void CTString::operator =(const CTString &str)
{
	len = str.len;
	siz = str.siz;
	delete txt;
	txt = new char [siz];
	if(txt == NULL)
		Errorhandler(SE_ALLOC);
	strcpy(txt,str.txt);
}

void CTString::operator =(const char * cstr)
{
	if((cstr==NULL)||(cstr[0]=='\0'))
	{
		len = 0;
		siz = allocincr;
	}
	else
	{
		len = strlen(cstr);
		siz = ((len+allocincr)/allocincr)*allocincr;
	}
	delete txt;
	txt = new char [siz];
	if(txt == NULL)
		Errorhandler(SE_ALLOC);
	if(len>0)
	    strcpy(txt, cstr);
	else
		txt[0] = '\0';
}

void CTString::operator =(const CTString &str)
{
	if(str=="")
	{
		len = 0;
		siz = allocincr;
	}
	else
	{
		len = str.GetLength();
		siz = ((len+allocincr)/allocincr)*allocincr;
	}
	delete txt;
	txt = new char [siz];
	if(txt == NULL)
		Errorhandler(SE_ALLOC);
	if(len>0)
	    strcpy(txt, str);
	else
		txt[0] = '\0';
}

char CTString::operator [](unsigned int pos)
{
    if(pos>=len)
	{
		Errorhandler(SE_INVALIDINDEX);
		return '\0';
	}
	else
		return txt[pos];
}

CTString  CTString::operator +(const CTString & str2)
{
    unsigned long totallen;
	unsigned int newsiz;
	CTString tempstr;
	char *temp;
	totallen = len+str2.len;
	if(totallen>UINT_MAX)
		Errorhandler(SE_TOOLONG);
	newsiz = siz + str2.siz;
	temp = new char[newsiz];
	if(temp = NULL)
		Errorhandler(SE_ALLOC);
	strcpy(temp, txt);
	strcpy(&temp[len], str2.txt);
	tempstr = temp;
    return tempstr;
}

CTString CTString::operator +(const char * str2)
{
    unsigned long totallen;
	unsigned int newsiz;
	CTString tempstr;
	char *temp;
	totallen = len+strlen(str2);
	if(totallen>UINT_MAX)
		Errorhandler(SE_TOOLONG);
	newsiz = siz + ((strlen(str2) + allocincr)/allocincr)*allocincr;
	temp = new char[newsiz];
	if(temp = NULL)
		Errorhandler(SE_ALLOC);
	strcpy(temp, txt);
	strcpy(&temp[len], str2);
	tempstr = temp;
    return tempstr;
}

CTString operator +(const char * str1, const CTString & str2)
{
    unsigned long totallen;
	unsigned int newsiz;
	CTString tempstr;
	char *temp;
	totallen = strlen(str1)+str2.len;
	if(totallen>UINT_MAX)
		str2.Errorhandler(SE_TOOLONG);
	newsiz = ((strlen(str1)+str2.allocincr)/str2.allocincr)*str2.allocincr + str2.siz;
	temp = new char[newsiz];
	if(temp = NULL)
		str2.Errorhandler(SE_ALLOC);
	strcpy(temp, str1);
	strcpy(&temp[strlen(str1)], str2.txt);
	tempstr = temp;
    return tempstr;
}

void CTString::operator +=(const CTString &str1)
{
	unsigned long totallen;
	unsigned int newlen, newsiz;
	char *temp;

	totallen = str1.len + len;
	if(totallen>UINT_MAX)
		Errorhandler(SE_TOOLONG);

	newlen = (unsigned int)totallen;
	newsiz = str1.siz + siz;
	temp = new char[newsiz];
	if(temp = NULL)
		Errorhandler(SE_ALLOC);
	strcpy(temp, txt);
	delete txt;
	txt = temp;

	strcpy(&txt[len], str1.txt);
	len = newlen;
	siz = newsiz;
	Shrink();
}
    
void CTString::operator +=(const char * str1)
{
	unsigned long totallen;
	unsigned int newlen, newsiz;
	char * temp;

	totallen = strlen(str1)+len;

	if(totallen>UINT_MAX)
		Errorhandler(SE_TOOLONG);
	newlen = (unsigned int) totallen;

	newsiz = ((strlen(str1) +allocincr)/allocincr)*allocincr + siz;
    temp = new char[newsiz];
	if(temp = NULL)
		Errorhandler(SE_ALLOC);
	strcpy(temp, txt);
	delete txt;
	txt = temp;
	strcpy(&txt[len], str1);
	len = newlen;
	siz = newsiz;
	Shrink();
}

Strcompval CTString::Compare(const CTString &str, Strcompmode casechk)
{
	char *tempstr1, *tempstr2;
	Strcompval compval;

	tempstr1 = strdup(txt);
	if(tempstr1 == NULL)
		Errorhandler(SE_ALLOC);
	tempstr2 = strdup(str.txt);
	if(tempstr2 == NULL)
		Errorhandler(SE_ALLOC);
	if(casechk == SM_IGNORE)
	{
		strupr(tempstr1);
		strupr(tempstr2);
	}
	int cmresult = strcmp(tempstr1, tempstr2);
	if(cmresult<0)
		compval = SC_LESS;
	else
	{
		if(cmresult>0)
			compval = SC_GREATER;
		else
			compval = SC_EQUAL;
	}
	delete tempstr1;
	delete tempstr2;
	return compval;
}

int CTString::Find(const CTString &str, unsigned int &pos, Strcompmode casechk)
{
	char * tempstr1, *tempstr2;
	unsigned int lastpos, searchlen, temppos;
	int found;
	
	tempstr1 = strdup(txt);
	if(tempstr1 == NULL)
		Errorhandler(SE_ALLOC);
	tempstr2 = strdup(str.txt);
	if(tempstr2 == NULL)
		Errorhandler(SE_ALLOC);

	if(casechk == SM_IGNORE)
	{
        strupr(tempstr1);
		strupr(tempstr2);
	}
	pos = 0;
	temppos = 0;
	found = 0;
	searchlen = str.len;
	lastpos = len - searchlen;
	while((temppos <= lastpos) && !found)
	{
		if(0==strncmp(&tempstr1[temppos], tempstr2, searchlen))
		{
			pos = temppos;
			found = 1;
		}
		else
			++temppos;
	}
	delete tempstr1;
	delete tempstr2;
	return found;
}

void CTString::Delete(unsigned int pos, unsigned int count)
{
	unsigned int copypos;
	copypos = pos +count;
    
	if(copypos>= len)
		Errorhandler(SE_INVALIDPARAM);
	else
		while(copypos<=len +1)
		{
			txt[pos] = txt[copypos];
			++pos;
			++copypos;
		}
	len -= count;
	Shrink();
}

void CTString::Insert(unsigned int pos, char ch)
{
	char *temp;
	if(pos>len)
		Errorhandler(SE_INVALIDPARAM);
	if(len+1 == siz)
	{
		siz += allocincr;
		temp = new char[siz];
		if(temp == NULL)
			Errorhandler(SE_ALLOC);
		strcpy(temp, txt);
		delete txt;
		txt = temp;
	}
	for(unsigned int col = len+1;col>pos; --col)
		txt[col] = txt[col -1];
	txt[pos] = ch;
	++len;
}

void CTString::Insert(unsigned int pos, const CTString &str)
{
    unsigned long totallen = str.len+len;
	if(totallen>UINT_MAX)
		Errorhandler(SE_TOOLONG);
	if(pos>len)
	    Errorhandler(SE_INVALIDPARAM);
	unsigned int slen = str.len;
	if(slen>0)
		for(unsigned int i=0; i<slen; ++i)
		{
			Insert(pos, str.txt[i]);
			++pos;
		}
}

CTString CTString::Substr(unsigned int start, unsigned int count)
{
	CTString tempstr;
	char *temp;
	if(start +count>len)
		Errorhandler(SE_INVALIDPARAM);
	if((start<len)&&(count>0))
	{
		temp = new char[count+1];
		memcpy(temp, &txt[start], count);
		temp[count] = '\0';
	}
	else
		temp = "";
	tempstr = temp;
	delete temp;
	return tempstr;
}

CTString CTString::Toupper()
{
	CTString tempstr = *this;
	strupr(tempstr.txt);
	return tempstr;
}

CTString CTString::Tolower()
{
	CTString tempstr = *this;
	strlwr(tempstr.txt);
	return tempstr;
}

*/

//////////////////////////////////////////////////////////////////////
// CErrorReporterBase --------- Construction/Destruction
//////////////////////////////////////////////////////////////////////

CErrorReporterBase::CErrorReporterBase(CTString *lead)
{
    if(lead == NULL)
		Leader = NULL;
	else
		Leader = new CTString(* lead);
}

CErrorReporterBase::CErrorReporterBase(CErrorReporterBase & er)
{
    CTString *x = er.Leader;
	Leader = new CTString(*x);
}

CErrorReporterBase::~CErrorReporterBase()
{
    if(Leader != NULL)
		delete Leader;
}





//////////////////////////////////////////////////////////////////////
// CErrorReporter ------------- Construction/Destruction
//////////////////////////////////////////////////////////////////////

CErrorReporter::CErrorReporter(CTString *lead):CErrorReporterBase(lead) 
{ 
//	m_hkey = NULL;
}
CErrorReporter & CErrorReporter::operator = (CErrorReporterBase &er)
{
	CErrorReporterBase * x;
	x = (CErrorReporterBase *) this;
	*x = er;
	return * this;
}


void CErrorReporter::Warning(Matvec_warnings Warningnum)
{
  /*CTString szWEFile;
	::RegOpenKey(HKEY_CLASSES_ROOT, TEXT("TlWarnFileName"), &m_hkey);
	if(m_hkey == NULL)
	{
			::RegCreateKey(HKEY_CLASSES_ROOT, TEXT("TlWarnFile"), &m_hkey);
	        szWEFile = GetTempFileName("TLWarnErr");
			::RegSetValue(m_hkey,"TlWarnFileName", REG_SZ,szWEFile, szWEFile.GetLength());
	}
	::RegCloseKey(m_hkey);*/
	CTString cond("警告：%s，文件 %s，第 %d 行");
	CTString msg(Mwarningstring[-WlastWarning+Warningnum]);
	MsgOut(msg, cond, ErrFile, ErrLine);
}

void CErrorReporter::Fatal(Matvec_errors Errornum)
{
/*   CTString szWEFile;
   char cWEFile[100];
   long length;

	::RegOpenKey(HKEY_CLASSES_ROOT, TEXT("TlWarnFileName"), &m_hkey);
	if(m_hkey == NULL)
	{
			int ret = ::RegCreateKey(HKEY_CLASSES_ROOT, TEXT("TlWarnFile"), &m_hkey);
	        szWEFile = GetTempFileName(CTString("TLWarnErr"));
			::RegSetValue(m_hkey,"TlWarnFileName", REG_SZ,szWEFile, szWEFile.GetLength());
	}
	::RegQueryValue(m_hkey, "TlWarnFileName", cWEFile, &length);
	if (cWEFile == 0)
	{
 	        szWEFile = GetTempFileName(CTString("TLWarnErr"));
			::RegSetValue(m_hkey,"TlWarnFileName", REG_SZ,szWEFile, szWEFile.GetLength());
    }
    szWEFile = cWEFile;
    ::RegCloseKey(m_hkey);
	CMemFile mWEFile;
	mWEFile.Open(cWEFile, */
	CTString cond("错误：%s，文件 %s，第 %d 行");
	if (Errornum !=EMAT_DIVBYZERO)  //zfm 1月六号，去除除数为零对话框
	{	
		CTString msg(Merrorstring[Errornum]);
		MsgOut(msg, cond, ErrFile, ErrLine);
	}
}

void CErrorReporter::MsgOut(char *msg, char *cond, char *file, int line)
{
	char * errMsg = new char[strlen(msg)+strlen(cond)+strlen(file)+6];
	sprintf(errMsg, cond, msg, file, line);

	CTString tmp1("原因:");
//#ifdef _DEBUG
//	MessageBox(GetDesktopWindow(),errMsg,tmp1,MB_ICONHAND|MB_SYSTEMMODAL);
//#endif

	delete errMsg;
}


void CErrorReporter::setfileline(char *file, int line)
{
	ErrFile = file;
	ErrLine = line;
}

CTString CErrorReporter::GetTempFileName(CTString strPrefix)
{
	char		cTempPath[1000];	// 临时文件路径及文件名

//	::ZeroMemory((PVOID)cTempPath, MAX_PATH);

	// 获取系统定义的临时文件路径
//	::GetTempPath(MAX_PATH, (LPTSTR)cTempPath);


	char szTempFileName[1000];
//	::GetTempFileName(LPCTSTR(cTempPath),"~"+strPrefix,	0,          
//		szTempFileName);
	strcpy(cTempPath,szTempFileName);
	// 成功找到临时文件名 返回0
	// 临时文件路径找不到 返回-1
	// 所有可用的临时文件名都已被占用，返回-2
	return cTempPath;	
}