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
enum Strcompval //�ַ�����С�ıȽϽ��
{
	SC_LESS,
	SC_EQUAL,
	SC_GREATER,
	SC_ERROR
};

enum Strcompmode //�ַ�����Сд�Ƿ�����
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
        "[[���󱨸�֮���󣿣���]]",
        "ά��С����",
        "������ά����ƥ��",
        "���ݴ�С������Χ",
        "��������",
        "��������",
        "�������ʹ���",
        "����Խ��",
        "����Ϊ��",
        "�㷨����",
        "����δ����",
        "�����������",
        "�㷨δ����",
        "����Խ��",
        "���㷨����Ϊ����",
        "����Ϊ�������޷�������ϵ��",
        "�޷�ȡ������",
        "����С�������޷�����QR�ֽ�",
		"���������",
};

static char * Mwarningstring[-WlastWarning]=
{
	"[[����֮���棿����]]",
	"����̫��������������Ч",
	"����̫С�����������ܲ���ȷ",
	"����ӽ����죬������ܲ���ȷ",
	"����ǶԳ�",
	"�����������",
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
