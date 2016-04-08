// TString.h: interface for the CTString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TSTRING_H__5A0969C8_CE8B_4C3B_B505_F787EDF7C622__INCLUDED_)
#define AFX_TSTRING_H__5A0969C8_CE8B_4C3B_B505_F787EDF7C622__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <math.h> 
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <list>
#include <vector>
#include "common.h"
//using namespace std;
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define TCHAR 		char
#define TCHAR_ARG   TCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char
#define BYTE    unsigned char
#ifdef _X86_
#define DOUBLE_ARG  _AFX_DOUBLE
#else
#define DOUBLE_ARG  double
#endif

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000

enum Strcompval //字符串大小的比较结果
{
	SC_LESS,
	SC_EQUAL,
	SC_GREATER,
	SC_ERROR
};

enum Strcompmode //字符串大小写是否敏感
{
	SM_SENSITIVE,
	SM_IGNORE
};

enum Strerror
{
	SE_ALLOC,
	SE_TOOLONG,
	SE_INVALIDPARAM,
	SE_INVALIDINDEX
};

#ifdef _UNIX //  Linux平台专用
//转换为大写,需要调用free来释放
char *strupr(char*pBuf);
//转换为小写,需要调用free来释放
char *strlwr(char*pBuf);
void _itoa(int value,char *pBuf,int nLen) ;
#endif

extern "C" int  str_cpy(char *pDt, const char *pSr,char ty);
extern "C" int  mem_cpy(char *pDt, const char *pSr,int len);

class AFX_EXT_CLASS CTString  
{
	
 	void Shrink();
private:
	unsigned int		m_size;
	unsigned int		m_len;
	//static unsigned int allocincr;
	unsigned int allocincr;
	char*				m_pData;
public:
	AFX_EXT_CLASS friend CTString operator + (const char *str, const  CTString &str1);
	CTString(int init);
	CTString();
    CTString(const CTString &c);
	CTString(const char &buf);
	CTString(const char *c);
	CTString(const char * cstr,int nLen);
	CTString(unsigned int count, char fillch='\0');
	CTString(std::string str);
	virtual ~CTString();
	int Size();
	
	// get data length
	int GetLength() ;
	int GetIncreaNum();
	int GetBufferSize();
	char* GetBuffer(int nMinBufLength);
	void ReleaseBuffer(int nNewLength = -1);
	char* GetBufferSetLength(int nNewLength);
	// TRUE if zero length
	bool IsEmpty() ;
	// clear contents to empty
	void Empty();
	// return single character at zero-based index
	char GetAt(int nIndex);
	// set a single character at zero-based index
	void SetAt(int nIndex, char ch);
	// return pointer to const string
	operator CTString*() const;

	operator char *();
    CTString& operator =(const CTString &c);
  	CTString& operator =(const char *s);
	CTString& operator =(const char &buf);
	CTString& operator =(std::string  &str);

	char& operator[](int nIndex);
	CTString operator +(const CTString &str2);
	CTString operator +(const char * str2);
	CTString& operator=(const unsigned char* lpsz);
	
	int operator < (const CTString &c);
	int operator < (const char *c);
	
	int operator > (const CTString &c);
	int operator > (const char *c);
	
	int operator <= (const CTString &c);
	int operator <= (const char *c);
	
	int operator == (const CTString &c);
	int operator ==(const char *c);
	
	int operator >=(const CTString &c);
	int operator >=(const char *c);
	
	int operator !=(const CTString &c);
	int operator !=(const char *c);
	
	void operator +=(const CTString &c);
	void operator +=(const char *c);
	
    int Find(const CTString &str, unsigned int &pos, Strcompmode casechk = SM_SENSITIVE);
    
    void Delete(unsigned int pos, unsigned int count);
    void Insert(unsigned int pos, const CTString &str);
	void Insert(unsigned int pos, char cstr);
	CTString Substr(unsigned int star, unsigned int count);
    Strcompval Compare(const CTString &str, Strcompmode casechk = SM_SENSITIVE);
	// return single character at zero-based index	
	char operator [] (unsigned int pos);
	
	char* GetData();
	CTString Toupper();
	CTString Tolower();
	// NLS aware conversion to uppercase
	void MakeUpper();
	// NLS aware conversion to lowercase
	void MakeLower();
	// reverse string right-to-left
	void MakeReverse();

    CTString& Format(char*,...);
	CTString& Format(CTString FormatStr,...);
	int FormatV(char* lpszFormat, va_list argList);

	//Finding
	int ReverseFind(char ch) const;
	int Find(char ch) const;
	int Find(const char* lpszSub) const;
	int Find(const char*, int nStart) const;

	//Substring
	CTString   Right(int nCount) ;
	CTString     Mid(int nFirst) ;
	CTString     Mid(int nFirst, int nCount) ;
    CTString    Left(int nCount) ;
	char*    AllocCopy( int Count , int nStart);

// Advanced manipulation
	int Delete(int nIndex, int nCount  = 1 );
	int Insert(int nIndex, char ch);
	int Insert(int nIndex, char* pstr);
	int Replace(char chOld, char chNew);
	int Replace(char* lpszOld, char* lpszNew);
	int Remove(char chRemove);
	void RemoveAll();
// Trime data
	void Trime();
	void TrimLeft();
	void TrimLeft(char chTarget);
	void TrimLeft(char* lpszTargets);
	void TrimRight();
	void TrimRight(char chTarget);
	void TrimRight(char* lpszTargetList);
	void Initial();
//	void*  CTString::operator new (unsigned int iSize)
//	{
//		return ::operator new(iSize);
//	}
//	void CTString::operator delete(void* ptrData)
//	{
//		if(ptrData != NULL)
//			::operator delete []ptrData;
//	}
	//
};
inline void CTString::Initial()
{
	m_pData = NULL;
	m_len = 0;
	m_size = 0;
	allocincr =10;
}
inline int CTString::GetLength()
{
	return m_len;
}

inline int CTString::GetIncreaNum()
{
	return allocincr;
} 
inline int CTString::GetBufferSize()
{
	return m_size;
} 

inline bool CTString::IsEmpty()
{
	bool bRet = false;
	if(m_len == 0)
		bRet = true;
	return bRet;
}
inline void CTString::Empty()
{
	m_len = 0;
	memset(m_pData,0x00,m_size*sizeof(char));
	m_pData[m_len] = '\0';
	return;
}
inline void CTString::RemoveAll()
{
	if(m_pData !=NULL)
	{
		delete []m_pData;
		m_pData = NULL;
		m_len = 0;
		m_size = 0;
	}
	return;
}
inline  int CTString::Size()
{
	return m_size;
}
inline char CTString::GetAt(int nIndex)
{
	if(m_len == 0|| nIndex < 0)
		return '\0';
	if(nIndex >=(int) m_len)
		return '\0';
	return m_pData[nIndex];
}
inline char& CTString::operator[](int nIndex) 
{
	// same as GetAt
	//assert(nIndex >= 0);
	//assert(nIndex < GetData()->nDataLength);
	return m_pData[nIndex];
}
inline  CTString& CTString::operator=(const unsigned char* lpsz)
	{ *this = (const char*)lpsz; return *this; }

inline 	void CTString::SetAt(int nIndex, char ch)
{
	if(m_len == 0|| nIndex < 0 || nIndex > (int)m_len)
		return;
	m_pData[nIndex] = ch;
	
}
inline int CTString::operator < (const CTString &str)
{
	return (Compare(str)==SC_LESS);
}

inline int CTString::operator > (const CTString &str)
{
	return (Compare(str)==SC_GREATER);
}

inline int CTString::operator <= (const CTString &str)
{
	return (Compare(str)!=SC_GREATER);
}

inline int CTString::operator >= (const CTString &str)
{
	return (Compare(str)!=SC_LESS);
}

inline int CTString::operator == (const CTString &str)
{
	return (Compare(str)==SC_EQUAL);
}

inline int CTString::operator != (const CTString &str)
{
	return (Compare(str)!=SC_EQUAL);
}

inline int CTString::operator < (const char *str)
{
	return (strcmp(m_pData,str)<0);
}

inline int CTString::operator > (const char *str)
{
	return (strcmp(m_pData,str)>0);
}

inline int CTString::operator <= (const char *str)
{
	return (strcmp(m_pData,str)<=0);
}

inline int CTString::operator >= (const char *str)
{
	return (strcmp(m_pData,str)>=0);
}

inline int CTString::operator == (const char *str)
{
	return (!strcmp(m_pData,str));
}

inline int CTString::operator != (const char *str)
{
	return (strcmp(m_pData,str));
}

//////////////////////////////////////////////////////////////////////////

class CTStringArray 
{
public:
	
	// Construction
	CTStringArray();
	
	// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);
	
	// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();
	
	// Accessing elements
	CTString GetAt(int nIndex) const;
	void SetAt(int nIndex, char* newElement);
	
	void SetAt(int nIndex, const CTString& newElement);
	
	CTString& ElementAt(int nIndex);
	
	// Direct Access to the element data (may return NULL)
	const CTString* GetData() const;
	CTString* GetData();
	
	// Potentially growing the array
	void SetAtGrow(int nIndex, char* newElement);
	
	void SetAtGrow(int nIndex, const CTString& newElement);
	
	int Add(char* newElement);
	
	int Add(CTString& newElement);
	
	int Append(const CTStringArray& src);
	void Copy(const CTStringArray& src);
	
	// overloaded operator helpers
	CTString operator[](int nIndex) const;
	CTString& operator[](int nIndex);
	
	// Operations that move elements around
	void InsertAt(int nIndex, char* newElement, int nCount = 1);
	
	void InsertAt(int nIndex, const CTString& newElement, int nCount = 1);
	
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CTStringArray* pNewArray);
	
	// Implementation
protected:
	CTString* m_ArrayData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount
	
	void InsertEmpty(int nIndex, int nCount);
	
	
public:
	~CTStringArray();
	
//	void Serialize(CArchive&);

};
inline int CTStringArray::GetSize() const
{ 
	return m_nSize; 
}
inline int CTStringArray::GetUpperBound() const
{ 
	return m_nSize-1; 
}
inline void CTStringArray::RemoveAll()
{ 
	SetSize(0); 
}
inline CTString CTStringArray::GetAt(int nIndex) const
{ 
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_ArrayData[nIndex]; 
}
inline void CTStringArray::SetAt(int nIndex, char* newElement)
{ 
	assert(nIndex >= 0 && nIndex < m_nSize);
	m_ArrayData[nIndex] = newElement; 
}

inline void CTStringArray::SetAt(int nIndex, const CTString& newElement)
{ 
	assert(nIndex >= 0 && nIndex < m_nSize);
	m_ArrayData[nIndex] = newElement; 
}

inline CTString& CTStringArray::ElementAt(int nIndex)
{ 
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_ArrayData[nIndex]; 
}
inline const CTString* CTStringArray::GetData() const
{ 
	return (const CTString*)m_ArrayData; 
}
inline CTString* CTStringArray::GetData()
{ 
	return (CTString*)m_ArrayData; 
}
inline int CTStringArray::Add(char* newElement)
{ 
	int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; 
}

inline int CTStringArray::Add( CTString& newElement)
{ 
	int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; 
}

inline CTString CTStringArray::operator[](int nIndex) const
{
	return GetAt(nIndex); 
}
inline CTString& CTStringArray::operator[](int nIndex)
{ 
	return ElementAt(nIndex); 
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//CPtrArray的标准C++实现，
class AFX_EXT_CLASS CPtrArray  
{
	
public:
	
	// Construction
	CPtrArray();
	
	// Attributes
	int GetSize() ;
	int GetUpperBound() ;
	void SetSize(int nNewSize, int nGrowBy = -1);
	
	// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();
	
	// Accessing elements
	void* GetAt(int nIndex) const;
	void SetAt(int nIndex, void* newElement);
	
	void*& ElementAt(int nIndex);
	
	// Direct Access to the element data (may return NULL)
	const void** GetData() const;
	void** GetData();
	
	// Potentially growing the array
	void SetAtGrow(int nIndex, void* newElement);
	
	int Add(void* newElement);
	
	int Append(const CPtrArray& src);
	void Copy(const CPtrArray& src);
	
	// overloaded operator helpers
	void* operator[](int nIndex) const;
	void*& operator[](int nIndex);
	
	// Operations that move elements around
	void InsertAt(int nIndex, void* newElement, int nCount = 1);
	
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CPtrArray* pNewArray);
	
	// Implementation
protected:
	void** m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount
	
	
public:
	~CPtrArray();
};


//CDWordArray的标准C++实现，
class AFX_EXT_CLASS CDWordArray  
{
	
public:
	
	// Construction
	CDWordArray();
	
	// Attributes
	int GetSize() ;
	int GetUpperBound() ;
	void SetSize(int nNewSize, int nGrowBy = -1);
	
	// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();
	
	// Accessing elements
	DWORD GetAt(int nIndex) const;
	void SetAt(int nIndex, DWORD newElement);
	
	DWORD& ElementAt(int nIndex);
	
	// Direct Access to the element data (may return NULL)
	const DWORD* GetData() const;
	DWORD* GetData();
	
	// Potentially growing the array
	void SetAtGrow(int nIndex, DWORD newElement);
	
	int Add(DWORD newElement);
	
	int Append(const CDWordArray& src);
	void Copy(const CDWordArray& src);
	
	// overloaded operator helpers
	DWORD operator[](int nIndex) const;
	DWORD& operator[](int nIndex);
	
	// Operations that move elements around
	void InsertAt(int nIndex, DWORD newElement, int nCount = 1);
	
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CDWordArray* pNewArray);
	
	// Implementation
protected:
	DWORD* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount
	
	
public:
	~CDWordArray();
};



//typedef int POSITION ;
//*  zlq 2005-09-02
class AFX_EXT_CLASS CPtrList 
{
	std::list<void *> m_list;
public:
	void* GetHead();
	void* GetTail();

// Construction
	CPtrList();

// Attributes (head and tail)
	// count of elements
	int GetCount() const;
	bool IsEmpty() const;
	POSITION GetHeadPosition() ;
	POSITION AddTail(void* newElement);
	void* GetNext(POSITION& rPosition) ; // return *Position++
	POSITION FindIndex(int nIndex);
	void RemoveAll();
	void RemoveAt(POSITION position);
	void RemoveTail();
	POSITION AddHead(void* newElement);
	void* GetPrev(POSITION& rPosition);
	POSITION InsertBefore(POSITION position, void* newElement);
	POSITION GetTailPosition();
	void* RemoveHead();
	void* GetAt(POSITION position);
	void* SetAt(POSITION position, void *newElement);
	~CPtrList();

protected:
	std::list<void*>::iterator GetIterator(POSITION pos);
	POSITION GetPos( std::list<void*>::iterator it);
};


////#_S 2005-9-2 zlq  
template<class TYPE, class ARG_TYPE>
class AFX_EXT_CLASS CTArray
{
	std::vector<TYPE> m_v;
public:
// Construction
	CTArray(){ };

// Attributes
	int GetSize() { return m_v.size();};
	//int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1)
	{
		m_v.resize(nNewSize);
	};

// Operations
	// Clean up

	void RemoveAll(){m_v.clear();};

	// Accessing elements
	TYPE GetAt(int nIndex) 
	{
		return m_v[nIndex];
	};

	void SetAt(int nIndex, ARG_TYPE newElement)
	{
		m_v[nIndex] = newElement;
	};
	//TYPE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	//const TYPE* GetData() const;
	//TYPE* GetData();

	// Potentially growing the array
	//void SetAtGrow(int nIndex, ARG_TYPE newElement);
	int Add(ARG_TYPE newElement){
		m_v.push_back(newElement);
		return m_v.size();
	}
	void Copy(const CTArray& src)
	{
		this->m_v = src.m_v;
	};

	// overloaded operator helpers
	TYPE operator[](int nIndex) const
	{
		return m_v[nIndex];
	};

	TYPE& operator[](int nIndex)
	{
		return m_v[nIndex];
	}
	

	// Operations that move elements around
	void InsertAt(int nIndex, ARG_TYPE newElement){
		//m_v.insert(&m_v[nIndex],newElement);
		m_v.insert(m_v.begin()+nIndex,newElement); 
	}
	void RemoveAt(int nIndex, int nCount = 1){
		//m_v.erase(&m_v[nIndex],&m_v[nIndex+nCount]);
		m_v.erase(m_v.begin()+nIndex,m_v.begin()+nCount+nIndex);
	}

// Implementation
public:
	~CTArray(){ };

};

/*

template<class TYPE, class ARG_TYPE>
class CTArray 
{
public:
// Construction
	CTArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	TYPE GetAt(int nIndex) const;
	void SetAt(int nIndex, ARG_TYPE newElement);
	TYPE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, ARG_TYPE newElement);
	int Add(ARG_TYPE newElement);
	int Append(const CTArray& src);
	void Copy(const CTArray& src);

	// overloaded operator helpers
	TYPE operator[](int nIndex) const;
	TYPE& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CTArray* pNewArray);

// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

public:
	~CTArray();

};


*/
#endif // !defined(AFX_TSTRING_H__5A0969C8_CE8B_4C3B_B505_F787EDF7C622__INCLUDED_)





















