// ErrorReporter.h: interface for the CErrorReporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORREPORTER_H__4ED15359_28D5_423C_9BC7_B0FE4CABAD34__INCLUDED_)
#define AFX_ERRORREPORTER_H__4ED15359_28D5_423C_9BC7_B0FE4CABAD34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Public/TString.h"  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////---------- CTString ------------////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
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
*/
/*
class AFX_EXT_CLASS CTString  
{
//	unsigned int siz;
//	unsigned int len;
//	char *txt;
//    static unsigned int allocincr;
//	void Shrink();
//    

public:
//	CTString(int init);
//	CTString();
//    CTString(const CTString &c);
//	CTString(const CTString &c);
//	CTString(const char *c);
//	CTString(unsigned int count, char fillch='\0');
//	virtual ~CTString();
//	unsigned int Length();
//	unsigned int Size();

	static void SeterrorHandler(void (*userhandler)(Strerror));
	static void (*Errorhandler)(Strerror err);
	
//	operator char *();
//    void operator =(const CTString &c);
//    void operator =(const CTString &c);
//	void operator =(const char *s);
//   

//	CTString operator +(const CTString &str2);
//	CTString operator +(const char * str2);
//	__declspec(dllexport) friend CTString operator + (const char *str, const CTString &str1);
//	int operator < (const CTString &c);
//	int operator < (const char *c);
//
//	int operator > (const CTString &c);
//	int operator > (const char *c);
//
//	int operator <= (const CTString &c);
//	int operator <= (const char *c);
//
//	int operator == (const CTString &c);
//	int operator ==(const char *c);
//
//	int operator >=(const CTString &c);
//	int operator >=(const char *c);
//
//	int operator !=(const CTString &c);
//	int operator !=(const char *c);
//
//	void operator +=(const CTString &c);
//	void operator +=(const char *c);
//
//    Strcompval Compare(const CTString &str, Strcompmode casechk = SM_SENSITIVE);
//    int Find(const CTString &str, unsigned int &pos, Strcompmode casechk = SM_SENSITIVE);
//    
//    void Delete(unsigned int pos, unsigned int count);
//    void Insert(unsigned int pos, const CTString &str);
//	void Insert(unsigned int pos, char cstr);
//	CTString Substr(unsigned int star, unsigned int count);
//    
//	char operator [] (unsigned int pos);
//
//	CTString Toupper();
//	CTString Tolower();

};

inline unsigned int CTString::Length()
{
	return len;
}

inline unsigned int CTString::Size()
{
	return siz;
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
	return (strcmp(txt,str)<0);
}

inline int CTString::operator > (const char *str)
{
	return (strcmp(txt,str)>0);
}

inline int CTString::operator <= (const char *str)
{
	return (strcmp(txt,str)<=0);
}

inline int CTString::operator >= (const char *str)
{
	return (strcmp(txt,str)>=0);
}

inline int CTString::operator == (const char *str)
{
	return (!strcmp(txt,str));
}

inline int CTString::operator != (const char *str)
{
	return (strcmp(txt,str));
}

*/
enum Matvec_errors
{
        FirstError,
        EMAT_INVALIDSIZE,
        EMAT_INVALIDORDER,
        EMAT_OVERFLOW,
        EMAT_SINGULAR,
        EMAT_OVERITERATE,
        EMAT_ASSIGNDATAERR,
        EMAT_OUTOFRANGE,
        EMAT_DIVBYZERO,
        EMAT_FAILURE,
        EMAT_NOTNUMBER,
        EMAT_INFINITECOND,
        EMAT_NODEFINE,
        EMAT_EXCEEDCAPACITY,
        EMAT_NOTSQUARE,
        EMAT_STATCONST,
        EMAT_GETDATA,
        EMAT_ROWSLESSCOLS,
		EMAT_NONPOSITIVEDEFINITE,
        ElastError
};

enum Matvec_warnings
{
        WlastWarning = -6,
        WEMAT_SINGULAR,
        WEMAT_SMALLNORM,
        WEMAT_LARGECONDITION,
        WEMAT_NONSYMMETRIC,
		WEMAT_NONTRIANGlE,
        WfirstWarning
};

static char *Merrorstring[ElastError]=
{
        "[[错误报告之错误？？？]]",
        "维数小于零",
        "两矩阵维数不匹配",
        "数据大小超出范围",
        "矩阵奇异",
        "迭代错误",
        "数据类型错误",
        "数据越界",
        "除数为零",
        "算法错误",
        "数据未定义",
        "条件数无穷大",
        "算法未定义",
        "构造越界",
        "该算法必须为方阵",
        "向量为常数，无法求出相关系数",
        "无法取得数据",
        "行数小于列数无法进行QR分解",
		"矩阵非正定",
};

static char * Mwarningstring[-WlastWarning]=
{
	"[[警告之警告？？？]]",
	"条件太宽，计算结果可能无效",
	"范数太小，计算结果可能不精确",
	"矩阵接近奇异，求逆可能不精确",
	"矩阵非对称",
	"矩阵非三角阵",
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////---------- CErrorReporterBase ------------////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CErrorReporterBase  
{
public:
	CErrorReporterBase(CTString *leader =NULL);
	CErrorReporterBase(CErrorReporterBase &er);
	virtual ~CErrorReporterBase();
	virtual void Warning(Matvec_warnings Warningnum)=0;
	virtual void Fatal(Matvec_errors Errornum)=0;

protected:
	virtual void MsgOut(char *msg, char *cond, char *file, int line)=0;
	CTString *Leader;
};





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////---------- CErrorReporter ------------////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



class AFX_EXT_CLASS CErrorReporter : public CErrorReporterBase  
{
private:
	CTString ErrFile;
    int ErrLine;
public:
	CErrorReporter(CTString *lead = NULL);
	CErrorReporter(CErrorReporter &er):CErrorReporterBase(er) {}; 
    CErrorReporter & operator = (CErrorReporterBase & er);
	virtual void Warning(Matvec_warnings Warningnum);
	virtual void Fatal(Matvec_errors Errornum);
	void setfileline(char *, int);
	CTString GetTempFileName(CTString strPrefix);
protected:
    virtual void MsgOut(char *msg, char *cond, char *file, int line); 
};


#endif // !defined(AFX_ERRORREPORTER_H__4ED15359_28D5_423C_9BC7_B0FE4CABAD34__INCLUDED_)
