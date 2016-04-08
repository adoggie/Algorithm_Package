// Complex.h: interface for the CComplex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPLEX_H__86CB0B90_9037_4788_B2FD_D84E0B0DCC38__INCLUDED_)
#define AFX_COMPLEX_H__86CB0B90_9037_4788_B2FD_D84E0B0DCC38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../Public/common.h"


/////////////////////////////////////////////////////////////////////////////////////////////
//////////// --------- CComplex ------------///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CComplex
{
private:
	double x_real;
	double y_imag;
	
	public:
		CComplex(double x, double y);
		CComplex();
		CComplex(double x);
		CComplex(const CComplex &c);
		~CComplex();
		
		
	public:
		
		CComplex & operator = (const CComplex &c);
		CComplex & operator = (double x);
		double real()const ;
		AFX_EXT_CLASS friend double real(const CComplex &c);
		double imag()const ;
		AFX_EXT_CLASS friend double imag(const CComplex &c);
		double abs()const ;
		AFX_EXT_CLASS friend double abs(const CComplex &c);
		CComplex conj()const ;
		AFX_EXT_CLASS friend CComplex conj(const CComplex &c);
		
		CComplex sin()const ;
		AFX_EXT_CLASS friend CComplex sin(const CComplex &c);
		CComplex asin()const ;
		AFX_EXT_CLASS friend CComplex asin(const CComplex &c);
		CComplex sinh()const ;
		AFX_EXT_CLASS friend CComplex sinh(const CComplex &c);
		CComplex cos()const ;
		AFX_EXT_CLASS friend CComplex cos(const CComplex &c);
		CComplex cosh()const ;
		AFX_EXT_CLASS friend CComplex cosh(const CComplex &c);
		CComplex acos()const ;
		AFX_EXT_CLASS friend CComplex acos(const CComplex &c);
		CComplex tan()const ;
		AFX_EXT_CLASS friend CComplex tan(const CComplex &c);
		CComplex tanh()const ;
		AFX_EXT_CLASS friend CComplex tanh(const CComplex &c);
		CComplex atan()const ;
		AFX_EXT_CLASS friend CComplex atan(const CComplex &c);
		CComplex exp()const ;
		AFX_EXT_CLASS friend CComplex exp(const CComplex &c);
		CComplex log()const ;
		AFX_EXT_CLASS friend CComplex log(const CComplex &c);
		CComplex log10()const ;
		AFX_EXT_CLASS friend CComplex log10(const CComplex &c);
		CComplex sqrt()const ;
		AFX_EXT_CLASS friend CComplex sqrt(const CComplex &c);
		CComplex pow(const CComplex &pp)const ;
		AFX_EXT_CLASS friend CComplex pow(const CComplex &c, const CComplex &pp);
		
		
		AFX_EXT_CLASS friend CComplex operator + (const CComplex &c1, const CComplex &c2);
		AFX_EXT_CLASS friend CComplex operator - (const CComplex &c1, const CComplex &c2);
		AFX_EXT_CLASS friend CComplex operator * (const CComplex &c1, const CComplex &c2);
		AFX_EXT_CLASS friend CComplex operator / (const CComplex &c1, const CComplex &c2);
		
		AFX_EXT_CLASS friend CComplex operator + (double d1, const CComplex &c2);
		AFX_EXT_CLASS friend CComplex operator - (double d1, const CComplex &c2);
		AFX_EXT_CLASS friend CComplex operator * (double d1, const CComplex &c2);
		AFX_EXT_CLASS friend CComplex operator / (double d1, const CComplex &c2);
		
		AFX_EXT_CLASS friend CComplex operator + (const CComplex &c1, double d2);
		AFX_EXT_CLASS friend CComplex operator - (const CComplex &c1, double d2);
		AFX_EXT_CLASS friend CComplex operator * (const CComplex &c1, double d2);
		AFX_EXT_CLASS friend CComplex operator / (const CComplex &c1, double d2);

		CComplex & operator += (const CComplex &c2);
		CComplex & operator -= (const CComplex &c2);
		CComplex & operator *= (const CComplex &c2);
		CComplex & operator /= (const CComplex &c2);
		
		CComplex & operator += (double d2);
		CComplex & operator -= (double d2);
		CComplex & operator *= (double d2);
		CComplex & operator /= (double d2);
		
		AFX_EXT_CLASS friend CComplex operator +(const CComplex &c1);
		AFX_EXT_CLASS friend CComplex operator -(const CComplex &c1);
		AFX_EXT_CLASS friend int operator == (const CComplex &c1, const CComplex &c2); 
		AFX_EXT_CLASS friend int operator != (const CComplex &c1, const CComplex &c2); 
		AFX_EXT_CLASS friend int operator == (const CComplex &c1, double c2); 
		AFX_EXT_CLASS friend int operator != (const CComplex &c1, double c2); 
		AFX_EXT_CLASS friend int operator == (const CComplex &c1, int c2); 
		AFX_EXT_CLASS friend int operator != (const CComplex &c1, int c2); 
		
};

#endif // !defined(AFX_COMPLEX_H__86CB0B90_9037_4788_B2FD_D84E0B0DCC38__INCLUDED_)
