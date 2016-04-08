// Output.h: interface for the COutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__29616EB7_E984_4326_97CE_23E384258E0B__INCLUDED_)
#define AFX_OUTPUT_H__29616EB7_E984_4326_97CE_23E384258E0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../cpknl/cpknl.h"
#include "../Public/TString.h"
#include "./Result.h"
#include <vector>
using namespace std;

class AFX_EXT_CLASS COutput  
{
public:
	enum  EKind
	{
		N=0, //整形
		D,   //双精度型
		S,   //字符串型
		A,   //字符串数组
		V,   //向量型
		M    //矩阵型
	};	
	
	class COutputInteger
	{
	public:
		COutputInteger()
		{
			szName = "";
			nValue = 0;
		};
		~COutputInteger()
		{
		};
		virtual int GetBufLen()
		{
			int nLen=0;
			AddStringLen(nLen,szName);
			AddDigitLen(nLen,nValue);
			return nLen;
		};
		virtual void Serialize(unsigned char *&pBuf)
		{
			SaveString(pBuf,szName);
			SaveDigit(pBuf,nValue);
		};
		virtual void Print()
		{
			printf("\nName=%s\tValue=%d\n",szName.GetData(),nValue);
		};
	public:
		CTString szName;
		int      nValue;
	};
	
	class COutputDouble
	{
	public:
		COutputDouble()
		{
			szName = "";
			dValue = 0;
		};
		~COutputDouble()
		{
		};
		virtual void Serialize(unsigned char *&pBuf)
		{
			SaveString(pBuf,szName);
			SaveDigit(pBuf,dValue);
		};
		virtual int GetBufLen()
		{
			int nLen=0;
			AddStringLen(nLen,szName);
			AddDigitLen(nLen,dValue);
			return nLen;
		};
		virtual void Print()
		{
			printf("\nName=%s\tValue=%f\n",szName.GetData(),dValue);
		};
	public:
		CTString szName;
		double   dValue;
	};
	
	class COutputString
	{
	public:
		COutputString()
		{
			szName = "";
			szValue = "";
		};
		~COutputString()
		{
		};
		virtual int GetBufLen()
		{
			int nLen=0;
			AddStringLen(nLen,szName);
			AddStringLen(nLen,szValue);
			return nLen;
		};
		virtual void Serialize(unsigned char *&pBuf)
		{
			SaveString(pBuf,szName);
			SaveString(pBuf,szValue);
		};
		virtual void Print()
		{
			printf("\nName=%s\tValue=%s\n",szName.GetData(),szValue.GetData());
		};
	public:
		CTString szName;
		CTString szValue;
	};
	class COutputStringArray
	{
	public:
		COutputStringArray()
		{
		};
		~COutputStringArray()
		{
			szValueArray.RemoveAll();
		};
		virtual int GetBufLen()
		{
			int nLen=0;
			int i=0, nSize=0;
			
			nSize = szValueArray.GetSize();
			if (nSize < 1)
				return nLen;
			AddStringLen(nLen,szName);
			AddDigitLen(nLen,nSize);
			for (i=0; i<nSize; i++)
			{
				AddStringLen(nLen,szValueArray.GetAt(i));
			}
			return nLen;
		};
		virtual void Serialize(unsigned char *&pBuf)
		{
			int nLen=0;
			int i=0, nSize=0;
			
			nSize = szValueArray.GetSize();
			if (nSize < 1)
				return;
			SaveString(pBuf,szName);
			SaveDigit(pBuf,nSize);
			for (i=0; i<nSize; i++)
			{
				SaveString(pBuf,szValueArray.GetAt(i));
			}
		};
		virtual void Print()
		{
			int i=0, nSize=0;
			nSize = szValueArray.GetSize();
			if (nSize < 1)
				return;
			printf("\nStringArray Name is %s.Size=%d\n",szName.GetData(),nSize);
			for (i=0; i<nSize; i++)
			{
				printf("Value(%d)=%s\t",i,szValueArray.GetAt(i).GetData());
			}
		};
	public:
		CTString szName;
		CTStringArray szValueArray;
	};
	class COutputVector
	{
	public:
		COutputVector()
		{
			szName = "";
		};
		~COutputVector()
		{
			v.destroy();
		};
		virtual int GetBufLen()
		{
			int nLen=0;
			int i=0, nSize=0;

			nSize = v.vlen();
			if (nSize < 1)
				return nLen;
			AddStringLen(nLen,szName);
			AddDigitLen(nLen,nSize);
			for (i=0; i<nSize; i++)
			{
				AddDigitLen(nLen,v(i));
			}
			return nLen;
		};
		virtual void Serialize(unsigned char *&pBuf)
		{
			int nLen=0;
			int i=0, nSize=0;
			
			nSize = v.vlen();
			if (nSize < 1)
				return;
			SaveString(pBuf,szName);
			SaveDigit(pBuf,nSize);
			for (i=0; i<nSize; i++)
			{
				SaveDigit(pBuf,v(i));
			}
		};
		virtual void Print()
		{
			int i=0, nSize=0;
			nSize = v.vlen();
			if (nSize < 1)
				return;
			printf("\nVector Name is %s.Size=%d\n",szName.GetData(),nSize);
			for (i=0; i<nSize; i++)
			{
				printf("Value(%d)=%f\t",i,v(i));
			}
		};
	public:
		CTString szName;
		CDoubleVector v;
	};
	
	class COutputMatrix
	{
	public:
		COutputMatrix()
		{
			szName = "";
		};
		~COutputMatrix()
		{
			m.destroy();
		};
		virtual int GetBufLen()
		{
			int nLen=0;
			int i=0, j=0, nRow=0, nCol=0;
			
			nRow = m.mrow();
			nCol = m.mcol();
			if (nRow < 1 || nCol < 1)
				return nLen;
			AddStringLen(nLen,szName);
			AddDigitLen(nLen,nRow);
			AddDigitLen(nLen,nCol);
			for (i=0; i<nRow; i++)
			{
				for (j=0; j<nCol; j++)
				{
					AddDigitLen(nLen,m(i,j));
				}
			}
			return nLen;
		};
		virtual void Serialize(unsigned char *&pBuf)
		{
			int nLen=0;
			int i=0, j=0, nRow=0, nCol=0;
			
			nRow = m.mrow();
			nCol = m.mcol();
			if (nRow < 1 || nCol < 1)
				return;
			SaveString(pBuf,szName);
			SaveDigit(pBuf,nRow);
			SaveDigit(pBuf,nCol);
			for (i=0; i<nRow; i++)
			{
				for (j=0; j<nCol; j++)
				{
					SaveDigit(pBuf,m(i,j));
				}
			}
		};
		virtual void Print()
		{
			int i=0, j=0, nRow=0, nCol=0;
			nRow = m.mrow();
			nCol = m.mcol();
			if (nRow < 1 || nCol < 1)
				return;
			printf("\nMatrix Name is %s.Row=%d,Col=%d\n",szName.GetData(),nRow,nCol);
			for (i=0; i<nRow; i++)
			{
				for (j=0; j<nCol; j++)
				{
					printf("Value(%d,%d)=%f\t",i,j,m(i,j));
				}
			}
		};
	public:
		CTString szName;
		CDoubleMatrix m;
	};
public:
	COutput();
	COutput(CTString szName);
	virtual ~COutput();
public:
	void Print();
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
public:
	void Release();
	void SetName(CTString szName);
public:
	void Add(CTString szName,int nValue);
	void Add(CTString szName, double dValue);
	void Add(CTString szName, CTString szValue);
	void Add(CTString szName, CTStringArray szValueArray);
	void Add(CTString szName, CDoubleVector &v);
	void Add(CTString szName, CDoubleMatrix &m);
	void Remove(COutput::EKind eKind);
	bool Remove(CTString szName, int nValue);
	bool Remove(CTString szName, double dValue);
	bool Remove(CTString szName, CTString szValue);
	bool Remove(CTString szName, CTStringArray szValueArray);
	bool Remove(CTString szName, CDoubleVector &v);
	bool Remove(CTString szName, CDoubleMatrix &m);
	bool Remove(int nIndex,COutput::EKind eKind);
	void RemoveAll();
public:
	EKind  m_eKind;//输出类型
private:
	CTString m_szName; //输出类型标识
private:
	std::vector<COutputInteger*>m_vInteger;          //存放整形数据
	std::vector<COutputDouble*>m_vDouble;            //存放双精度形数据
	std::vector<COutputString*>m_vString;            //存放字符串形数据
	std::vector<COutputStringArray*>m_vStringArray;  //存放字符串形数据
	std::vector<COutputVector*>m_vVector;            //存放向量形数据
	std::vector<COutputMatrix*>m_vMatrix;            //存放矩阵形数据
};

#endif // !defined(AFX_OUTPUT_H__29616EB7_E984_4326_97CE_23E384258E0B__INCLUDED_)
