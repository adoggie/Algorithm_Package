// FillMissing.h: interface for the CFillMissing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILLMISSING_H__F0D31D57_40BA_4D1D_A613_F9CDB0485256__INCLUDED_)
#define AFX_FILLMISSING_H__F0D31D57_40BA_4D1D_A613_F9CDB0485256__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../cpknl/cpknl.h"
#include <vector>

const char FILLMISSING[]  = "FILLMISSING";
const char FILLMETHOD[]   = "/METHOD"; 
const char COLMEAN[]      = "MEAN";
const char NEARMEAN[]     = "NEARMEAN";
const char COLMEDIAN[]    = "MEDIAN";
const char NEARMEDIAN[]   = "NEARMEDIAN";
const char COLMOD[]       = "MOD";
const char LINEINSERT[]   = "LINEINSERT";
const char LINEHOPE[]     = "LINEHOPE";
const char ALL0[]         = "0";
const char ALL1[]         = "A";
const char ALL2[]         = "ALL";
const char END[]          = ")";

class CDataInterface;
class CField;

//using namespace std::string;
//using namespace std::vector;
class CFillMissing  
{
public:
	CFillMissing();
	virtual ~CFillMissing();
	bool Run(CDataInterface *pData, std::string lpszVO);
protected:
	void LineHope(LPCTSTR lpszFieldName, CField *pFieldTag = NULL);
	void LineInsert(LPCSTR lpszFieldName, CField *pFieldTag = NULL);
	void Median(LPCTSTR lpszFieldName, CField *pFieldTag = NULL);
	void NearMedian(LPCSTR lpszFieldName,int nNumber, CField *pFieldTag = NULL);
	void Mod(LPCTSTR lpszFieldName, CField *pFieldTag = NULL);
	void Mean(LPCSTR lpszFieldName, CField *pFieldTag = NULL);
	void NearMean(LPCSTR lpszFieldName, int nNumber, CField *pFieldTag = NULL);
	bool GetVaildData(CDoubleVector &v,CField *pField,int nNumber, bool bDown);
protected:
	int Mod(CDoubleVector &v);
	bool Verify();
	bool Parse(const std::string &VO);
private:
	class IM
	{
	public:
		IM()
		{
			szVar = "";
			szTag = "";
			szMethod = "";
			nValue = -1;
		}
	public:
		CTString szMethod; //填充方法
		CTString szVar;    //变量字段名称
		CTString szTag;    //目标字段名称
		int      nValue;   //填充值方法
	};
	std::vector<IM*>m_vIM;
	CDataInterface *m_pData;
};

#endif // !defined(AFX_FILLMISSING_H__F0D31D57_40BA_4D1D_A613_F9CDB0485256__INCLUDED_)
