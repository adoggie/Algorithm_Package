// TString.cpp: implementation of the CTString class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "../Public/TString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define A_INLINE 
//unsigned int CTString::allocincr = 10;
CTString::CTString(int init)
{
	Initial();
	m_size = init+allocincr;
	m_pData = new char [m_size];
	//	if(m_pData ==NULL)
	//		Errorhandler(SE_ALLOC);
	m_pData[0] = '\0';
}
#ifdef _UNIX //  Linux平台专用
//转换为大写,需要调用free来释放
char *strupr(char*pBuf)
{
	int nLen = strlen(pBuf);
	char *pNew = (char *)malloc(nLen+1);
	memset(pNew,0,nLen+1);
	for (int i=0;i<nLen;i++)
		*(pNew+i) = toupper(*(pBuf+i));
	return pNew;
}
//转换为大写,需要调用free来释放
char *strlwr(char*pBuf)
{
	int nLen = strlen(pBuf);
	char *pNew = (char *)malloc(nLen+1);
	memset(pNew,0,nLen+1);
	for (int i=0;i<nLen;i++)
		*(pNew+i) = tolower(*(pBuf+i));
	return pNew;
}
void _itoa(int value,char *pBuf,int nLen) 
{
  sprintf(pBuf,"%i",value);
}
#endif

extern "C" int  str_cpy(char *pDt, const char *pSr,char ty)
{
	int i=0;
	if (pSr != NULL || pDt == NULL)
	{
		if (*pSr == ty)
		{
			for (i=1;pSr[i]!='\0';i++)
			{
				if (pSr[i]==ty)
				{
					pDt[i-1] = '\0';
					break;
				}
				pDt[i-1]= pSr[i];
			}
			return i+1;
		}
	}
	pDt[0] = '\0';
	return 0;
}

extern "C" int  mem_cpy(char *pDt, const char *pSr,int len)
{
	if (pSr != NULL || pDt == NULL)
	{
		memcpy(pDt,pSr,len);
		pDt[len] = '\0';
	}
	return len;
}

CTString::CTString()
{
	Initial();
	m_size = allocincr;
	m_pData = new char [m_size];
	//m_pData = NULL;
	m_pData[m_len] = '\0';
}
CTString::CTString(unsigned int count, char fillch)
{
	Initial();
	
	m_len = count;
	m_size = ((count+allocincr)/allocincr)*allocincr;
	m_pData = new char [m_size];
    memset(m_pData, fillch, count);
	m_pData[m_len] = '\0';
}


CTString::CTString(const CTString &str)
{
	Initial();
	m_len = str.m_len;
	m_size = str.m_size;
	m_pData = new char [m_size];
	strcpy(m_pData, str.m_pData);
}

CTString::~CTString()
{
	if(m_size)
		RemoveAll();
}

void CTString::Shrink()
{
	char *temp = NULL;
	if((m_size-m_len)>allocincr)
	{
		m_size = ((m_len + allocincr)/allocincr)*allocincr;
		temp = new char[m_size];
		if(temp != NULL)
		{
			strcpy(temp, m_pData);
			delete []m_pData;
			m_pData = temp;
		}
//		else
//			Errorhandler(SE_ALLOC);
	}
}


CTString::CTString(const char * cstr)
{
	Initial();
	allocincr = 10;

	if((cstr==NULL)||(cstr[0]=='\0'))
	{
		m_len = 0;
		m_size = allocincr;
	}
	else
	{
		m_len = strlen(cstr);
		m_size = ((m_len+allocincr)/allocincr)*allocincr;
	}

	m_pData = new char [m_size];
//	if(m_pData == NULL)
//		Errorhandler(SE_ALLOC);
	if(m_len>0)
	    strcpy(m_pData, cstr);
	else
		m_pData[0] = '\0';
}

CTString::CTString(const char & buf)
{
	Initial();
	allocincr = 10;
	int len = strlen(&buf);
	if(len ==0)
	{
		m_len = 0;
		m_size = allocincr;
	}
	else
	{
		m_len = len;
		m_size = ((m_len+allocincr)/allocincr)*allocincr;
	}
	
	m_pData = new char [m_size];
	//	if(m_pData == NULL)
	//		Errorhandler(SE_ALLOC);
	if(m_len>0)
		strcpy(m_pData, &buf);
	else
		m_pData[0] = '\0';
}

CTString::CTString(const char * cstr,int nLen)
{
	Initial();
	m_len = nLen;
	m_size = ((m_len+allocincr)/allocincr)*allocincr;


	m_pData = new char [m_size];
//	if(m_pData == NULL)
//		Errorhandler(SE_ALLOC);
	if(m_len>0)
	    strcpy(m_pData, cstr);
	else
		m_pData[0] = '\0';
}


//void CTString::SeterrorHandler(void(*userhandler)(Strerror))
//{
//    Errorhandler = userhandler;


CTString::operator char *()
{
	return m_pData;
}

CTString& CTString::operator =(const CTString &str)
{
	m_len = str.m_len;
	m_size = str.m_size;
	delete [] m_pData;
	m_pData = new char [m_size];
//	if(m_pData == NULL)
//		Errorhandler(SE_ALLOC);
	strcpy(m_pData,str.m_pData);
	return *this;
}

CTString& CTString::operator =(const char * cstr)
{
	if((cstr==NULL)||(cstr[0]=='\0'))
	{
		m_len = 0;
		m_size = allocincr;
	}
	else
	{
		m_len = strlen(cstr);
		m_size = ((m_len+allocincr)/allocincr)*allocincr;
	}
	delete [] m_pData;
	m_pData = new char [m_size];
//	if(m_pData == NULL)
//		Errorhandler(SE_ALLOC);
	if(m_len>0)
	    strcpy(m_pData, cstr);
	else
		m_pData[0] = '\0';

	return *this;
}

CTString& CTString::operator =(const char &buf)
{
	int len = strlen(&buf);
	if(len==0)
	{
		m_len = 0;
		m_size = allocincr;
	}
	else
	{
		m_len = len;
		m_size = ((m_len+allocincr)/allocincr)*allocincr;
	}
	delete [] m_pData;
	m_pData = new char [m_size];
	//	if(m_pData == NULL)
	//		Errorhandler(SE_ALLOC);
	if(m_len>0)
		strcpy(m_pData, &buf);
	else
		m_pData[0] = '\0';
	
	return *this;
}


CTString& CTString::operator =(std::string &str)
{
	 *this = (const char*)str.c_str(); return *this;
}
CTString::CTString(std::string str)
{
	const char * cstr = (const char*)str.c_str();
	Initial();
	allocincr = 10;

	if((cstr==NULL)||(cstr[0]=='\0'))
	{
		m_len = 0;
		m_size = allocincr;
	}
	else
	{
		m_len = strlen(cstr);
		m_size = ((m_len+allocincr)/allocincr)*allocincr;
	}

	m_pData = new char [m_size];
//	if(m_pData == NULL)
//		Errorhandler(SE_ALLOC);
	if(m_len>0)
	    strcpy(m_pData, cstr);
	else
		m_pData[0] = '\0';
}


char CTString::operator[](unsigned int pos)
{
    if(pos>=m_len)
	{
//		Errorhandler(SE_INVALIDINDEX);
		return '\0';
	}
	else
		return m_pData[pos];
}

CTString  CTString::operator +(const CTString & str2)
{
    unsigned long totallen;
	unsigned int newsiz;
	CTString tempstr;
	char *temp;
	totallen = m_len+str2.m_len;
//	if(totallen>UINT_MAX)
//		Errorhandler(SE_TOOLONG);
	newsiz = m_size + str2.m_size;
	temp = new char[newsiz];
//	if(temp = NULL)
//		Errorhandler(SE_ALLOC);
	strcpy(temp, m_pData);
	strcpy(&temp[m_len], str2.m_pData);
	tempstr = temp;
	delete[] temp;
	temp =NULL;
    return tempstr;
}

CTString CTString::operator +(const char * str2)
{
    unsigned long totallen;
	unsigned int newsiz;
	CTString tempstr;
	char *temp;
	totallen = m_len+strlen(str2);
//	if(totallen>UINT_MAX)
//		Errorhandler(SE_TOOLONG);
	newsiz = m_size + ((strlen(str2) + allocincr)/allocincr)*allocincr;
	temp = new char[newsiz];
//	if(temp = NULL)
//		Errorhandler(SE_ALLOC);
	strcpy(temp, m_pData);
	strcpy(&temp[m_len], str2);
	tempstr = temp;
	delete[] temp;
	temp = NULL;
    return tempstr;
}

CTString operator +(const char * str1, const CTString & str2_)
{
    unsigned long totallen;
	unsigned int newsiz;
	CTString tempstr,&str2=(CTString&)str2_;
	char *temp;
	totallen = strlen(str1)+str2.GetLength();
//	if(totallen>UINT_MAX)
//		str2.Errorhandler(SE_TOOLONG);
	newsiz = ((strlen(str1)+str2.GetIncreaNum())/str2.GetIncreaNum())*str2.GetIncreaNum() + str2.GetBufferSize();
	temp = new char[newsiz];
//	if(temp = NULL)
//		str2.Errorhandler(SE_ALLOC);
	strcpy(temp, str1);
	strcpy(&temp[strlen(str1)], str2.GetData());
	tempstr = temp;
	delete[] temp;
	temp = NULL;
    return tempstr;
}

void CTString::operator +=(const CTString &s)
{
	unsigned long totallen;
	unsigned int newlen, newsiz;
	char *temp;
	CTString str1(s);	//--add on zhangbin 2006.10.12, memory leak
	totallen = str1.m_len + m_len;
//	if(totallen>UINT_MAX)
//		Errorhandler(SE_TOOLONG);


	newlen = (unsigned int)totallen;
	newsiz = str1.m_size + m_size;
	temp = new char[newsiz];
//	if(temp = NULL)
//		Errorhandler(SE_ALLOC);

	strcpy(temp, m_pData);
	delete []m_pData;
	m_pData = temp;

	strcpy(&m_pData[m_len], str1.m_pData);
	m_len = newlen;
	m_size = newsiz;
	Shrink();
}
    
void CTString::operator +=(const char * str1)
{
	unsigned long totallen;
	unsigned int newlen, newsiz;
	char * temp;

	totallen = strlen(str1)+m_len;

//	if(totallen>UINT_MAX)
//		Errorhandler(SE_TOOLONG);

	newlen = (unsigned int) totallen;

	newsiz = ((strlen(str1) +allocincr)/allocincr)*allocincr + m_size;
    temp = new char[newsiz];
//	if(temp = NULL)
//		Errorhandler(SE_ALLOC);

	strcpy(temp, m_pData);
	delete [] m_pData;
	m_pData = temp;
	strcpy(&m_pData[m_len], str1);
	m_len = newlen;
	m_size = newsiz;
	Shrink();
}

Strcompval CTString::Compare(const CTString &str, Strcompmode casechk)
{
	int nCmp = 0;
	Strcompval compval = SC_EQUAL;
	
	if (casechk == SM_IGNORE)
	{
		nCmp = _stricmp(m_pData,str.m_pData);
	}
	else
	{
		nCmp = strcmp(m_pData,str.m_pData);
	}
	if (nCmp > 0)
		compval = SC_GREATER;
	else if (nCmp < 0)
		compval = SC_LESS;
	else
		compval = SC_EQUAL;
	return compval;
	/*--modify by xds 20060928
	char *tempstr1= NULL;
	char *tempstr2= NULL;
	Strcompval compval;

	tempstr1 = strdup(m_pData);
//	if(tempstr1 == NULL)
//		Errorhandler(SE_ALLOC);
	tempstr2 = strdup(str.m_pData);
//	if(tempstr2 == NULL)
//		Errorhandler(SE_ALLOC);
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
	*/
}

int CTString::Find(const CTString &str, unsigned int &pos, Strcompmode casechk)
{
	int nFound=0, nResult=0;
	if (casechk == SM_IGNORE)
	{
		char *pStr1 = strdup(m_pData);
		char *pStr2 = strdup(str.m_pData);
		strupr(pStr1);
		strupr(pStr2);
		char *pdest = strstr(pStr1, pStr2);
		nResult = pdest - pStr1;
		free(pStr1);
		free(pStr2);
	}
	else
	{
		char *pdest = strstr(m_pData, str.m_pData);
		nResult = pdest - m_pData;
	}
	if (nResult >= 0)
	{
		pos = nResult;
		nFound = 1;
	}
	return nFound;
	/*--modify by xds 20060928
	char * tempstr1 = NULL;
	char * tempstr2 = NULL;
	unsigned int lastpos, searchlen, temppos;
	int found;
	
	tempstr1 = strdup(m_pData);
//	if(tempstr1 == NULL)
//		Errorhandler(SE_ALLOC);
	tempstr2 = strdup(str.m_pData);
//	if(tempstr2 == NULL)
//		Errorhandler(SE_ALLOC);

	if(casechk == SM_IGNORE)
	{
        strupr(tempstr1);
		strupr(tempstr2);
	}
	pos = 0;
	temppos = 0;
	found = 0;
	searchlen = str.m_len;
	lastpos = m_len - searchlen;
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
	delete []tempstr1;
	delete []tempstr2;
	return found;
	*/
}

void CTString::Delete(unsigned int pos, unsigned int count)
{
	unsigned int copypos;
	copypos = pos +count;
    
//	if(copypos>= m_len)
//		Errorhandler(SE_INVALIDPARAM);
//	else

		while(copypos<=m_len +1)
		{
			m_pData[pos] = m_pData[copypos];
			++pos;
			++copypos;
		}
	m_len -= count;
	Shrink();
}

void CTString::Insert(unsigned int pos, char ch)
{
	char *temp;
//	if(pos>m_len)
//		Errorhandler(SE_INVALIDPARAM);

	if(m_len+1 == m_size)
	{
		m_size += allocincr;
		temp = new char[m_size];
//		if(temp == NULL)
//			Errorhandler(SE_ALLOC);

		strcpy(temp, m_pData);
		delete []m_pData;
		m_pData = temp;
	}
	for(unsigned int col = m_len+1;col>pos; --col)
		m_pData[col] = m_pData[col -1];
	m_pData[pos] = ch;
	++m_len;
}

void CTString::Insert(unsigned int pos, const CTString &str)
{
    unsigned long totallen = str.m_len+m_len;
	unsigned int slen = str.m_len;
	if(slen>0)
		for(unsigned int i=0; i<slen; ++i)
		{
			Insert(pos, str.m_pData[i]);
			++pos;
		}
}

CTString CTString::Substr(unsigned int start, unsigned int count)
{
	CTString tempstr;
	char *temp;

	if((start<m_len)&&(count>0))
	{
		temp = new char[count+1];
		memcpy(temp, &m_pData[start], count);
		temp[count] = '\0';
	}
	else
		temp = "";
	tempstr = temp;
	delete []temp;
	return tempstr;
}

CTString CTString::Toupper()
{
	CTString tempstr = *this;
	strupr(tempstr.m_pData);
	return tempstr;
}

CTString CTString::Tolower()
{
	CTString tempstr = *this;
	strlwr(tempstr.m_pData);
	return tempstr;
}
void CTString::MakeUpper()
{
	if(m_len == 0)
		return;
	strupr(m_pData);
	return;
}
void CTString::MakeLower()
{
	if(m_len == 0)
		return;
	strlwr(m_pData);
	return;
}
void CTString::MakeReverse()
{
	if(m_len <=1)
		return;
	int nCount = m_len;
	if(m_pData[nCount] == '\0' )
		nCount--;
	for (int i =0 ;i< (nCount+1)/2;i++)
	{
		char ch = m_pData[i];
		m_pData[i] = m_pData[nCount - i];
		m_pData[nCount - i] = ch;
	}
	return;
	
}
CTString& CTString::Format(char * str,...)
{
	int  count =0;
	va_list ArgList;
	va_start(ArgList,str);
	FormatV(str,ArgList);
	va_end(ArgList);
	return *this; 
}
CTString& CTString::Format(CTString str,...)
{
	int  count =0;
	va_list ArgList;
	va_start(ArgList,str.m_pData);
	FormatV(str.m_pData,ArgList);
	va_end(ArgList);
	return *this; 
}
int CTString::FormatV(char* lpszFormat, va_list argList)
{
	va_list argListSave = argList;

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (char* lpsz = lpszFormat; *lpsz != '\0'; lpsz++)
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz+1) == '%')
		{
			//nMaxLen += strlen(lpsz);
			nMaxLen ++;
			continue;
		}
		lpsz++;
		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz++)
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}

		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = atoi(lpsz);
			for (; *lpsz != '\0' && isdigit(*lpsz); lpsz++)
				;
		}
		assert(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz++;

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz++;
			}
			else
			{
				nPrecision = atoi(lpsz);

				for (; *lpsz != '\0' && isdigit(*lpsz); lpsz++)
					;
			}
			assert(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (strncmp(lpsz, "I64", 3) == 0)
		{
			lpsz += 3;
			nModifier = FORCE_INT64;
#if !defined(_X86_) && !defined(_ALPHA_)
			// __int64 is only available on X86 and ALPHA platforms
		//	assert(0);
#endif
		}
		else
		{
			switch (*lpsz)
			{
			// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz++;
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz++;
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz++;
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, CHAR_ARG);
			break;
		case 'c'|FORCE_ANSI:
		case 'C'|FORCE_ANSI:
			nItemLen = 2;
			va_arg(argList, CHAR_ARG);
			break;
		case 'c'|FORCE_UNICODE:
		case 'C'|FORCE_UNICODE:
			nItemLen = 2;
			va_arg(argList, CHAR_ARG);
			break;

		// strings
		case 's':
			{
				char* pstrNextArg = va_arg(argList, char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 'S':
			{
#ifndef _UNICODE
				char* pstrNextArg = va_arg(argList, char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#else
				char* pstrNextArg = va_arg(argList, char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#endif
			}
			break;

		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
			{
				char* pstrNextArg = va_arg(argList, char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
			{
				char* pstrNextArg = va_arg(argList, char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = strlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
//--UNIX PORT:#ifdef WINPLAT //  Windows平台
				if (nModifier & FORCE_INT64)
					va_arg(argList, __int64);
				else //===uc
//--#endif
					va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, DOUBLE_ARG);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'f':
				va_arg(argList, DOUBLE_ARG);
				nItemLen = 128; // width isn't truncated
				// 312 == strlen("-1+(309 zeroes).")
				// 309 zeroes == max precision of a double

				nItemLen = max(nItemLen, 312+nPrecision);
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;

//			default:
//				assert(0);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	char* Buffer = new char[nMaxLen + this->allocincr ];
	int leng = vsprintf(Buffer, lpszFormat, argListSave);
	Buffer[leng]= '\0';
	*this = Buffer; 
	delete [] Buffer;
	va_end(argListSave);
	return leng;
}

char* CTString::GetData()
{
	return this->m_pData;
}

int CTString::ReverseFind(char ch) const
{
	// find last single character
	char* lpsz = strrchr(m_pData, (unsigned char) ch);
	
	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pData);
}
int CTString::Find(char ch) const
{
	// find last single character
	char* lpsz = strchr(m_pData, (unsigned char) ch);
	
	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pData);
}
// find a sub-string (like strstr)
int CTString::Find(const char* lpszSub) const
{
	return Find(lpszSub, 0);
}

int CTString::Find(const char* lpszSub, int nStart) const
{

	int nLength = m_len;
	if (nStart > nLength)
		return -1;
	
	// find first matching substring
	char* lpsz = strstr(m_pData + nStart, lpszSub);
	
	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pData);
}

//Substring
CTString CTString::Mid(int nFirst) 
{
	return Mid(nFirst, m_len - nFirst);
}

CTString CTString::Mid(int nFirst, int nCount) 
{
	// optimize case of returning entire string
	if (nFirst == 0 && nFirst + nCount == (int)m_len)
		return *this;
	
	char* tempPtr = this->AllocCopy(nCount,nFirst);
	CTString dest=tempPtr ;
	delete[] tempPtr;
	return dest;
}

CTString CTString::Right(int nCount) 
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= (int)m_len)
		return *this;
	char* tempPtr = this->AllocCopy(nCount, m_len-nCount);
	CTString dest=tempPtr ;
	delete[] tempPtr;
	return dest;
}

CTString CTString::Left(int nCount) 
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= (int)m_len)
		return *this;
	
	char* tempPtr =this->AllocCopy(nCount, 0);
	CTString dest = tempPtr;
	delete [] tempPtr;
	return dest;
}
char* CTString::GetBuffer(int nMinBufLength)
{
	assert(nMinBufLength >= 0);
	
	if (nMinBufLength > this->GetBufferSize())
	{
		// we have to grow the buffer
		char* pOldData = this->GetData();
		int   nOldLen = this->GetLength();   // AllocBuffer will tromp it
		if (nMinBufLength < nOldLen)
			nMinBufLength = nOldLen;
		//AllocBuffer(nMinBufLength);
		if(m_pData !=NULL)
		{
			delete [] m_pData;
			m_size = nMinBufLength+ this->GetIncreaNum();			
			m_pData = new char[m_size];
		
		}
		memcpy(m_pData, pOldData, (nOldLen+1)*sizeof(char));
		m_len = nOldLen;
		delete [] pOldData;
	}
	// return a pointer to the character storage for this string
	assert(m_pData != NULL);
	return m_pData;
}

void CTString::ReleaseBuffer(int nNewLength)
{
	
	if (nNewLength == -1)
		nNewLength = m_len; // zero terminated
	
	assert(nNewLength <= this->GetBufferSize());
	m_len = nNewLength;
	
	m_pData[m_len] = '\0';
}

char* CTString::GetBufferSetLength(int nNewLength)
{
	assert(nNewLength >= 0);
	
	GetBuffer(nNewLength);
	m_len = nNewLength;
	m_pData[nNewLength] = '\0';
	return m_pData;
}
char* CTString::AllocCopy(int nCount , int nFirst)
{
	if(m_len == 0)
		return this->m_pData;
	if (nFirst < 0)
		nFirst = 0;
	if (nCount < 0)
		nCount = 0;
	
	if (nFirst + nCount >(int) m_len)
		nCount = (int)m_len - nFirst;
	if (nFirst > (int)m_len)
		nCount = 0;
	
	assert(nFirst >= 0);
	assert(nFirst + nCount <= (int)m_len);
	
	// optimize case of returning entire string
	if (nFirst == 0 && nFirst + nCount == (int)m_len)
		return m_pData;
    char* pTemp = new char[nCount+1];
	memcpy(pTemp,m_pData + nFirst, (nCount+1)* sizeof(char));
	pTemp[nCount] = '\0';
 	return pTemp;
}
//////////////////////////////////////////////////////////////////////////

// Advanced manipulation

int CTString::Delete(int nIndex, int nCount /* = 1 */)
{
	if (nIndex < 0)
		nIndex = 0;
	int nNewLength = m_len;
	if (nCount > 0 && nIndex < nNewLength)
	{
//		CopyBeforeWrite();
		int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;

		memcpy(m_pData + nIndex,
			m_pData + nIndex + nCount, nBytesToCopy * sizeof(char));
		m_len = nNewLength - nCount;
	}

	return nNewLength;
}

int CTString::Insert(int nIndex, char ch)
{
	//CopyBeforeWrite();

	if (nIndex < 0)
		nIndex = 0;

	int nNewLength = m_len;
	if (nIndex > nNewLength)
		nIndex = nNewLength;
	nNewLength++;

	if ((int)m_size < nNewLength)
	{
		char* pOldData =m_pData;
		char* pstr = m_pData;

		m_pData = new char[nNewLength];
		m_size = nNewLength;
		memcpy(m_pData, pstr, (m_len+1)*sizeof(char));
		m_len = m_len+1;
		delete[] pOldData;
	}

	// move existing bytes down
	memcpy(m_pData + nIndex + 1,
		m_pData + nIndex, (nNewLength-nIndex)*sizeof(char));
	m_pData[nIndex] = ch;
	m_len = nNewLength;

	return nNewLength;
}

int CTString::Insert(int nIndex, char* pstr)
{
	if (nIndex < 0)
		nIndex = 0;

	int nInsertLength = strlen(pstr);
	int nNewLength = m_len;
	if (nInsertLength > 0)
	{
	//	CopyBeforeWrite();
		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength += nInsertLength;

		if ((int)m_size <= nNewLength)
		{
			char* pOldData = m_pData;
			char* pstr = m_pData;
			m_pData = new char[nNewLength+1];
			memset(m_pData,0,nNewLength+1);
			memcpy(m_pData, pstr, (m_len)*sizeof(char));
			delete [] pOldData;
		}

		
		// move existing bytes down
		memcpy(m_pData + nIndex + nInsertLength,
			m_pData + nIndex,
			(nNewLength-nIndex-nInsertLength+1)*sizeof(char));
		memcpy(m_pData + nIndex,
			pstr, nInsertLength*sizeof(char));
		m_len = nNewLength;
	}
	m_size = nNewLength+1;
	return nNewLength;
}

int CTString::Replace(char chOld, char chNew)
{
	int nCount = 0;

	// short-circuit the nop case
	if (chOld != chNew)
	{
		// otherwise modify each character that matches in the string
		//CopyBeforeWrite();
		char* psz = m_pData;
		char* pszEnd = psz + m_len;
		while (psz < pszEnd)
		{
			// replace instances of the specified character only
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz++;
		}
	}
	return nCount;
}

int CTString::Replace(char* lpszOld, char* lpszNew)
{
	// can't have empty or NULL lpszOld

	int nSourceLen = strlen(lpszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplacementLen = strlen(lpszNew);

	// loop once to figure out the size of the result string
	int nCount = 0;
	char* lpszStart = m_pData;
	char* lpszEnd = m_pData + m_len;
	char* lpszTarget;
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += strlen(lpszStart) + 1;
	}

	// if any changes were made, make them
	if (nCount > 0)
	{
	//	CopyBeforeWrite();

		// if the buffer is too small, just
		//   allocate a new buffer (slow but sure)
		int nOldLength = (int)m_len;
		int nNewLength =  nOldLength + (nReplacementLen-nSourceLen)*nCount;
		if ((int)m_size < nNewLength)
		{
			char* pOldData = m_pData;
			char* pstr = m_pData;
			m_pData = new char[nNewLength];
			memcpy(m_pData, pstr, m_len*sizeof(char));
			delete [] pOldData;
		}
		// else, we just do it in-place
		lpszStart = m_pData;
		lpszEnd = m_pData + m_len;

		// loop again to actually do the work
		while (lpszStart < lpszEnd)
		{
			while ( (lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLength - (lpszTarget - m_pData + nSourceLen);
				memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
					nBalance * sizeof(char));
				memcpy(lpszTarget, lpszNew, nReplacementLen*sizeof(char));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			lpszStart += strlen(lpszStart) + 1;
		}
		assert(m_pData[nNewLength] == '\0');
		m_len = nNewLength;
	}

	return nCount;
}

int CTString::Remove(char chRemove)
{
//	CopyBeforeWrite();

	char* pstrSource = m_pData;
	char* pstrDest = m_pData;
	char* pstrEnd = m_pData + m_len;

	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chRemove)
		{
			*pstrDest = *pstrSource;
			pstrDest++;
		}
		pstrSource++;
	}
	*pstrDest = '\0';
	int nCount = pstrSource - pstrDest;
	m_len -= nCount;

	return nCount;
}

//////////////////////////////////////////////////////////////////////////////
// Trim elements

void CTString::TrimRight(char* lpszTargetList)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

//	CopyBeforeWrite();
	char* lpsz = m_pData;
	char* lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (strchr(lpszTargetList, *lpsz) != NULL)
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = lpsz++;
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character
		*lpszLast = '\0';
		m_len = lpszLast - m_pData;
	}
}

void CTString::TrimRight(char chTarget)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	//CopyBeforeWrite();
	char* lpsz = m_pData;
	char* lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (*lpsz == chTarget)
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = lpsz++;
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character
		*lpszLast = '\0';
		m_len = lpszLast - m_pData;
	}
}

void CTString::TrimRight()
{
	// find beginning of trailing spaces by starting at beginning (DBCS aware)

	//CopyBeforeWrite();
	char* lpsz = m_pData;
	char* lpszLast = NULL;

	while (*lpsz != '\0')
	{
		//if (isspace(*lpsz))
		if (*lpsz == ' ' || *lpsz == '\t')
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = lpsz++;
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
		m_len = lpszLast - m_pData;
	}
}

void CTString::TrimLeft(char* lpszTargets)
{
	// if we're not trimming anything, we're not doing any work
	if (strlen(lpszTargets) == 0)
		return;

	//CopyBeforeWrite();
	char* lpsz = m_pData;

	while (*lpsz != '\0')
	{
		if (strchr(lpszTargets, *lpsz) == NULL)
			break;
		lpsz = lpsz++;
	}

	if (lpsz != m_pData)
	{
		// fix up data and length
		int nDataLength = m_len - (lpsz - m_pData);
		memmove(m_pData, lpsz, (nDataLength+1)*sizeof(char));
		m_len = nDataLength;
	}
}

void CTString::TrimLeft(char chTarget)
{
	// find first non-matching character

	//CopyBeforeWrite();
	char* lpsz = m_pData;

	while (chTarget == *lpsz)
		lpsz = lpsz++;

	if (lpsz != m_pData)
	{
		// fix up data and length
		int nDataLength = m_len - (lpsz - m_pData);
		memmove(m_pData, lpsz, (nDataLength+1)*sizeof(char));
		m_len = nDataLength;
	}
}

void CTString::TrimLeft()
{
	// find first non-space character

	//CopyBeforeWrite();
	char* lpsz = m_pData;

	while (isspace(*lpsz))
		lpsz = lpsz++;

	if (lpsz != m_pData)
	{
		// fix up data and length
		int nDataLength = m_len - (lpsz - m_pData);
		memmove(m_pData, lpsz, (nDataLength+1)*sizeof(char));
		m_len = nDataLength;
	}
}
void CTString::Trime()
{
	// find first non-space character
	
	//CopyBeforeWrite();
	char* lpsz = m_pData;
	int nCount =0;
	char* pTemp = new char[m_len];
	while (*lpsz != '\0')
	{
		if(isspace(*lpsz))
			lpsz = lpsz++;
		else
		{
			pTemp[nCount] = *lpsz;
			nCount++;
			lpsz = lpsz++;
		}
	}
	if (nCount>0)
	{
		pTemp[nCount]='\0';
		memcpy(m_pData,pTemp,(nCount+1)*sizeof(char));
		m_len = nCount;
	}
}

///////////////////////////////////////////////////////////////////////////////

//#include "elements.h"  // used for special creation
 void ConstructElements(CTString* pNewData, int nCount)
{
	assert(nCount >= 0);
	

		pNewData = new CTString[nCount*sizeof(CTString)];
	//	pNewData+= sizeof(CTString);
	
}

 void DestructElements(CTString* pOldData, int nCount)
{
	assert(nCount >= 0);
	
	while (nCount--)
	{
		pOldData->RemoveAll();
		pOldData++;
	}
}

static void CopyElements(CTString* pDest, CTString* pSrc, int nCount)
{
	assert(nCount >= 0);
	
	while (nCount--)
	{
		*pDest = *pSrc;
		++pDest;
		++pSrc;
	}
}


CTStringArray::CTStringArray()
{
	m_ArrayData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CTStringArray::~CTStringArray()
{
   RemoveAll();
}

//void CTStringArray::RemoveAll()
//{
//	DestructElements(m_ArrayData, m_nSize);
//	delete [] m_ArrayData;


void CTStringArray::SetSize(int nNewSize, int nGrowBy)
{
	assert(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		DestructElements(m_ArrayData, m_nSize);
		delete[] m_ArrayData;
		m_ArrayData = NULL;
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_ArrayData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		assert(nNewSize <= SIZE_T_MAX/sizeof(CTString));    // no overflow
#endif
		//m_ArrayData = (CTString*) new BYTE[nNewSize * sizeof(CTString)];
		m_ArrayData = new CTString[nNewSize];

		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{

		if (m_nSize > nNewSize)  // destroy the old elements
			DestructElements(&m_ArrayData[nNewSize], m_nSize-nNewSize);

		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = min(1024, max(4, m_nSize / 8));
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		assert(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		assert(nNewMax <= SIZE_T_MAX/sizeof(CTString)); // no overflow
#endif
		CTString* pNewData = new CTString[nNewMax];
		//CTString*	pNewData = (CTString*) new BYTE[m_nSize * sizeof(CTString)];
		// copy new data from old
		//memcpy(pNewData, m_ArrayData, m_nSize * sizeof(CTString));
        CopyElements(pNewData,m_ArrayData,m_nSize);
		// construct remaining elements
		assert(nNewSize > m_nSize);
		DestructElements(m_ArrayData,m_nSize);
		// get rid of old stuff (note: no destructors called)
		delete[] m_ArrayData;
		m_ArrayData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}

}

int CTStringArray::Append(const CTStringArray& src)
{
	////ASSERT_VALID(this);
	assert(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);

	CopyElements(m_ArrayData + nOldSize, src.m_ArrayData, src.m_nSize);

	return nOldSize;
}

void CTStringArray::Copy(const CTStringArray& src)
{
	////ASSERT_VALID(this);
	assert(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);

	CopyElements(m_ArrayData, src.m_ArrayData, src.m_nSize);

}

void CTStringArray::FreeExtra()
{
	////ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		assert(m_nSize <= SIZE_T_MAX/sizeof(CTString)); // no overflow
#endif
		CTString* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (CTString*) new BYTE[m_nSize * sizeof(CTString)];
			// copy new data from old
			memcpy(pNewData, m_ArrayData, m_nSize * sizeof(CTString));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_ArrayData;
		m_ArrayData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CTStringArray::SetAtGrow(int nIndex, char* newElement)
{
	////ASSERT_VALID(this);
	assert(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);
	m_ArrayData[nIndex] = newElement;
}


void CTStringArray::SetAtGrow(int nIndex, const CTString& strElement)
{
	////ASSERT_VALID(this);
	assert(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);
	m_ArrayData[nIndex] = strElement;
}



void CTStringArray::InsertEmpty(int nIndex, int nCount)
{
	////ASSERT_VALID(this);
	assert(nIndex >= 0);    // will expand to meet need
	assert(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount);  // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount);  // grow it to new size
		// shift old data up to fill gap
		CopyElements(&m_ArrayData[nIndex+1],&m_ArrayData[nIndex],nOldSize-nIndex);
//		memmove(&m_ArrayData[nIndex+nCount], &m_ArrayData[nIndex],
//			(nOldSize-nIndex) * sizeof(CTString));

		// re-init slots we copied from

		ConstructElements(&m_ArrayData[nIndex], nCount);

	}

	// insert new value in the gap
	assert(nIndex + nCount <= m_nSize);
}


void CTStringArray::InsertAt(int nIndex, char* newElement, int nCount)
{

	// make room for new elements
	InsertEmpty(nIndex, nCount);



	// copy elements into the empty space
	CTString temp = newElement;
	while (nCount--)
		m_ArrayData[nIndex++] = temp;

}


void CTStringArray::InsertAt(int nIndex, const CTString& newElement, int nCount)
{
	// make room for new elements
	InsertEmpty(nIndex, nCount);

	// copy elements into the empty space
	while (nCount--)
		m_ArrayData[nIndex++] = newElement;
}


void CTStringArray::RemoveAt(int nIndex, int nCount)
{
	////ASSERT_VALID(this);
	assert(nIndex >= 0);
	assert(nCount >= 0);
	assert(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);

	DestructElements(&m_ArrayData[nIndex], nCount);

//	if (nMoveCount)
//		memmove(&m_ArrayData[nIndex], &m_ArrayData[nIndex + nCount],
//			nMoveCount * sizeof(CTString));
	CopyElements(&m_ArrayData[nIndex],&m_ArrayData[nIndex + nCount],nCount);
	m_nSize -= nCount;
}

void CTStringArray::InsertAt(int nStartIndex, CTStringArray* pNewArray)
{
	////ASSERT_VALID(this);
	assert(pNewArray != NULL);
//	ASSERT_KINDOF(CTStringArray, pNewArray);
	////ASSERT_VALID(pNewArray);
	assert(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}



/////////////////////////////////////////////////////////////////////////////
//
//                             CPtrArray的C++实现
//////////////////////////////////////////////////////////////////////////

CPtrArray::CPtrArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CPtrArray::~CPtrArray()
{
    if(this != NULL)	
		delete[] (unsigned char*)m_pData;
}


int CPtrArray::GetSize() 
{
	return m_nSize; 
}
int CPtrArray::GetUpperBound() 
{
	return m_nSize-1; 
}
 void CPtrArray::RemoveAll()
{
	 SetSize(0); 
}
void* CPtrArray::GetAt(int nIndex) const
{ 
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; 
}
void CPtrArray::SetAt(int nIndex, void* newElement)
{
	assert(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement;
 }

void*& CPtrArray::ElementAt(int nIndex)
{
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; 
}
const void** CPtrArray::GetData() const
{ 
	return (const void**)m_pData; 
}

void** CPtrArray::GetData()
{ 
	return (void**)m_pData; 
}

int CPtrArray::Add(void* newElement)
{ 
	int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; 
}

void* CPtrArray::operator[](int nIndex) const
{ 
	return GetAt(nIndex); 
}

void*& CPtrArray::operator[](int nIndex)
{
	return ElementAt(nIndex); 
}
void CPtrArray::SetSize(int nNewSize, int nGrowBy)
{
	if (this == NULL||nNewSize < 0)
		return;
	
	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		delete[] (unsigned char*)m_pData;
		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		assert(nNewSize <= SIZE_T_MAX/sizeof(void*));    // no overflow
#endif
		m_pData = (void**) new unsigned char[nNewSize * sizeof(void*)];

		memset(m_pData, 0, nNewSize * sizeof(void*));  // zero fill

		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements

			memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(void*));

		}

		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = min(1024, max(4, m_nSize / 8));
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		assert(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		assert(nNewMax <= SIZE_T_MAX/sizeof(void*)); // no overflow
#endif
		void** pNewData = (void**) new unsigned char[nNewMax * sizeof(void*)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(void*));

		// construct remaining elements
		assert(nNewSize > m_nSize);

		memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(void*));


		// get rid of old stuff (note: no destructors called)
		delete[] (unsigned char*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

int CPtrArray::Append(const CPtrArray& src)
{
	assert(this != NULL);
	assert(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);

	memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(void*));

	return nOldSize;
}

void CPtrArray::Copy(const CPtrArray& src)
{
	assert(this != NULL);
	assert(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);

	memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(void*));

}

void CPtrArray::FreeExtra()
{
	assert(this != NULL);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		assert(m_nSize <= SIZE_T_MAX/sizeof(void*)); // no overflow
#endif
		void** pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (void**) new unsigned char[m_nSize * sizeof(void*)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(void*));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (unsigned char*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CPtrArray::SetAtGrow(int nIndex, void* newElement)
{
	assert(this != NULL);
	assert(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);
	m_pData[nIndex] = newElement;
}





void CPtrArray::InsertAt(int nIndex, void* newElement, int nCount)
{

	assert(this != NULL);
	assert(nIndex >= 0);    // will expand to meet need
	assert(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount);  // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount);  // grow it to new size
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(void*));

		// re-init slots we copied from

		memset(&m_pData[nIndex], 0, nCount * sizeof(void*));

	}

	// insert new value in the gap
	assert(nIndex + nCount <= m_nSize);



	// copy elements into the empty space
	while (nCount--)
		m_pData[nIndex++] = newElement;

}



void CPtrArray::RemoveAt(int nIndex, int nCount)
{
	assert(this != NULL);
	assert(nIndex >= 0);
	assert(nCount >= 0);
	assert(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);

	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(void*));
	m_nSize -= nCount;
}

void CPtrArray::InsertAt(int nStartIndex, CPtrArray* pNewArray)
{
	assert(this != NULL);
	assert(pNewArray != NULL);
	assert(pNewArray != NULL);
	assert(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}

////////////////////////////////CPtrArray 的编写结束//////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
//                             CDWordArray的C++实现
//////////////////////////////////////////////////////////////////////////

CDWordArray::CDWordArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CDWordArray::~CDWordArray()
{
    if(this != NULL)	
		delete[] (unsigned char*)m_pData;
}


int CDWordArray::GetSize() 
{
	return m_nSize; 
}
int CDWordArray::GetUpperBound() 
{
	return m_nSize-1; 
}
 void CDWordArray::RemoveAll()
{
	 SetSize(0); 
}
DWORD CDWordArray::GetAt(int nIndex) const
{ 
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; 
}
void CDWordArray::SetAt(int nIndex, DWORD newElement)
{
	assert(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement;
 }

DWORD& CDWordArray::ElementAt(int nIndex)
{
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; 
}
const DWORD* CDWordArray::GetData() const
{ 
	return (const DWORD*)m_pData; 
}

DWORD* CDWordArray::GetData()
{ 
	return (DWORD*)m_pData; 
}

int CDWordArray::Add(DWORD newElement)
{ 
	int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; 
}

DWORD CDWordArray::operator[](int nIndex) const
{ 
	return GetAt(nIndex); 
}

DWORD& CDWordArray::operator[](int nIndex)
{
	return ElementAt(nIndex); 
}
void CDWordArray::SetSize(int nNewSize, int nGrowBy)
{
	if (this == NULL||nNewSize < 0)
		return;
	
	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		delete[] (unsigned char*)m_pData;
		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		assert(nNewSize <= SIZE_T_MAX/sizeof(DWORD));    // no overflow
#endif
		m_pData = (DWORD*) new unsigned char[nNewSize * sizeof(DWORD)];

		memset(m_pData, 0, nNewSize * sizeof(DWORD));  // zero fill

		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements

			memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(DWORD));

		}

		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = min(1024, max(4, m_nSize / 8));
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		assert(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		assert(nNewMax <= SIZE_T_MAX/sizeof(DWORD)); // no overflow
#endif
		DWORD* pNewData = (DWORD*) new unsigned char[nNewMax * sizeof(DWORD)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(DWORD));

		// construct remaining elements
		assert(nNewSize > m_nSize);

		memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(DWORD));


		// get rid of old stuff (note: no destructors called)
		delete[] (unsigned char*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

int CDWordArray::Append(const CDWordArray& src)
{
	assert(this != NULL);
	assert(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);

	memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(DWORD));

	return nOldSize;
}

void CDWordArray::Copy(const CDWordArray& src)
{
	assert(this != NULL);
	assert(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);

	memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(DWORD));

}

void CDWordArray::FreeExtra()
{
	assert(this != NULL);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		assert(m_nSize <= SIZE_T_MAX/sizeof(DWORD)); // no overflow
#endif
		DWORD* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (DWORD*) new unsigned char[m_nSize * sizeof(DWORD)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(DWORD));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (unsigned char*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CDWordArray::SetAtGrow(int nIndex, DWORD newElement)
{
	assert(this != NULL);
	assert(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);
	m_pData[nIndex] = newElement;
}





void CDWordArray::InsertAt(int nIndex, DWORD newElement, int nCount)
{

	assert(this != NULL);
	assert(nIndex >= 0);    // will expand to meet need
	assert(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount);  // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount);  // grow it to new size
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(DWORD));

		// re-init slots we copied from

		memset(&m_pData[nIndex], 0, nCount * sizeof(DWORD));

	}

	// insert new value in the gap
	assert(nIndex + nCount <= m_nSize);



	// copy elements into the empty space
	while (nCount--)
		m_pData[nIndex++] = newElement;

}



void CDWordArray::RemoveAt(int nIndex, int nCount)
{
	assert(this != NULL);
	assert(nIndex >= 0);
	assert(nCount >= 0);
	assert(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);

	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(DWORD));
	m_nSize -= nCount;
}

void CDWordArray::InsertAt(int nStartIndex, CDWordArray* pNewArray)
{
	assert(this != NULL);
	assert(pNewArray != NULL);
	assert(pNewArray != NULL);
	assert(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}

////////////////////////////////CDWordArray 的编写结束//////////////////////////////////////////





CPtrList::CPtrList()
{
}
CPtrList::~CPtrList()
{
}

int CPtrList::GetCount() const
{
	return m_list.size();
}

bool CPtrList::IsEmpty()const
{
	return m_list.size()?0:1;
}

POSITION CPtrList::GetHeadPosition() 
{
	if (m_list.size() ==0)
		return 0;

	std::list<void*>::iterator it;
	it = m_list.begin();
	void *pV = &it;
	return (POSITION) *(int*)pV;
}

POSITION CPtrList::AddTail(void* newElement)
{
	m_list.push_back(newElement);
	
	std::list<void*>::iterator it = m_list.end();
	it--;
	return GetPos(it);
}

POSITION CPtrList::GetPos(std::list<void*>::iterator it)
{
	void *pV = &it;
	return (POSITION) *(int*)pV;
}

std::list<void*>::iterator CPtrList::GetIterator(POSITION pos)
{
	std::list<void*>::iterator *pKK = (std::list<void*>::iterator *)(void*)&pos;
	return *pKK;
}

void* CPtrList::GetNext(POSITION& rPosition) 
{
	std::list<void*>::iterator it= GetIterator(rPosition);
	void *pData = *it;
	it++;
	if (m_list.end() == it)
		rPosition = NULL;
	else
		rPosition = GetPos(it);

	return pData;
}

void* CPtrList::GetAt(POSITION position)
{
	std::list<void*>::iterator it= GetIterator(position);
	void *pData = *it;
	return pData;
}

void* CPtrList::SetAt(POSITION position, void *newElement)
{
	std::list<void*>::iterator it = GetIterator(position);
	void *pData = *it;
	*it = newElement;
	return pData;
}

void* CPtrList::RemoveHead()
{
	std::list<void*>::iterator it=m_list.begin();
	void *pData = *it;
	m_list.erase(it);
	return pData;

}

POSITION CPtrList::GetTailPosition()
{
	if (m_list.size() ==0)
		return 0;

	std::list<void*>::iterator it=m_list.end();
	it--;
	return GetPos(it);
}

POSITION CPtrList::InsertBefore(POSITION position, void *newElement)
{
	std::list<void*>::iterator it= GetIterator(position);
	it= m_list.insert(it,newElement);
	return GetPos(it);

}

void* CPtrList::GetPrev(POSITION &rPosition)
{
	std::list<void*>::iterator it= GetIterator(rPosition);
	void *pData = *it;

	if (m_list.begin() == it)
		rPosition = NULL;
	else
	{
		it--;
		rPosition = GetPos(it);
	}

	return pData;
}

POSITION CPtrList::AddHead(void *newElement)
{
	m_list.push_front(newElement);
	std::list<void*>::iterator it = m_list.begin();
	return GetPos(it);
}

void CPtrList::RemoveAt(POSITION position)
{
	std::list<void*>::iterator it= GetIterator(position);
	m_list.erase(it);
}

void CPtrList::RemoveTail()
{
	POSITION position = this->GetTailPosition();
	std::list<void*>::iterator it= GetIterator(position);
	m_list.erase(it);
}

void CPtrList::RemoveAll()
{
	m_list.clear();
}

POSITION CPtrList::FindIndex(int nIndex)
{
	std::list<void*>::iterator it;
	int nCur = 0;
	for (it = m_list.begin(); it!=m_list.end() ;it++)
	{
		if (nIndex ==nCur)
			return GetPos(it);
		nCur++;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE> out-of-line functions


void* CPtrList::GetTail()
{
	return GetAt(GetTailPosition());
}

void* CPtrList::GetHead()
{
	return GetAt(GetHeadPosition());
}
