// Complex.cpp: implementation of the CComplex class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Complex.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// CComplex ------------ Construction/Destruction
//////////////////////////////////////////////////////////////////////
CComplex::CComplex(double x, double y)
{
	x_real = x; 
	y_imag= y; 
}

CComplex::CComplex(double x)
{
	x_real = x; 
	y_imag= 0; 
}

CComplex::CComplex()
{
	x_real =0;
	y_imag =0;
}

CComplex::CComplex(const CComplex &c)
{ 
	x_real=c.x_real;
	y_imag = c.y_imag;
}

CComplex::~CComplex()
{ 
}

CComplex & CComplex::operator = (const CComplex &c)
{ 
	x_real=c.x_real;
	y_imag = c.y_imag;
	return *this;
}

CComplex & CComplex::operator = (double x)
{ 
	x_real = x;
	y_imag = 0;
	return *this;
}

CComplex operator + (const CComplex &c1,const CComplex &c2)
{
	CComplex c;
	c.x_real = c1.x_real + c2.x_real;
	c.y_imag = c1.y_imag + c2.y_imag;
	return c;
}

CComplex operator - (const CComplex &c1,const CComplex &c2)
{
	CComplex c;
	c.x_real = c1.x_real - c2.x_real;
	c.y_imag = c1.y_imag - c2.y_imag;
	return c;
}

CComplex operator * (const CComplex &c1,const CComplex &c2)
{
	CComplex c;
	c.x_real = c1.x_real*c2.x_real - c1.y_imag*c2.y_imag;
	c.y_imag = c1.x_real*c2.y_imag + c1.y_imag*c2.x_real;
	return c;
}

CComplex operator / (const CComplex &c1,const CComplex &c2)
{
	CComplex c;
	c.x_real = (c1.x_real*c2.x_real + c1.y_imag*c2.y_imag)/(c2.x_real*c2.x_real+c2.y_imag*c2.y_imag);
	c.y_imag = (c2.x_real*c1.y_imag - c2.y_imag*c1.x_real)/(c2.x_real*c2.x_real+c2.y_imag*c2.y_imag);
	return c;
}

CComplex operator + (double d1,const CComplex &c2)
{
	CComplex c;
	c.x_real = d1 + c2.x_real;
	c.y_imag = c2.y_imag;
	return c;
}

CComplex operator - (double d1,const CComplex &c2)
{
	CComplex c;
	c.x_real = d1 - c2.x_real;
	c.y_imag = -c2.y_imag;
	return c;
}

CComplex operator * (double d1,const CComplex &c2)
{
	CComplex c;
	c.x_real = d1 * c2.x_real;
	c.y_imag = d1 * c2.y_imag;
	return c;
}

CComplex operator / (double d1,const CComplex &c2)
{
	CComplex c;
	c.x_real = (d1 * c2.x_real)/(c2.x_real*c2.x_real + c2.y_imag*c2.y_imag);
	c.y_imag = (-d1 * c2.y_imag)/(c2.x_real*c2.x_real + c2.y_imag*c2.y_imag);
	return c;
}

CComplex operator + (const CComplex &c1, double d2)
{
	CComplex c;
	c.x_real = c1.x_real + d2;
	c.y_imag = c1.y_imag;
	return c;
}

CComplex operator - (const CComplex &c1, double d2)
{
	CComplex c;
	c.x_real = c1.x_real - d2;
	c.y_imag = c1.y_imag;
	return c;
}

CComplex operator * (const CComplex &c1, double d2)
{
	CComplex c;
	c.x_real = c1.x_real * d2;
	c.y_imag = c1.y_imag * d2;
	return c;
}

CComplex operator / (const CComplex &c1, double d2)
{
	CComplex c;
	c.x_real = c1.x_real / d2;
	c.y_imag = c1.y_imag / d2;
	return c;
}

CComplex & CComplex::operator += (const CComplex &c2)
{
	x_real += c2.x_real;
	y_imag += c2.y_imag;
	return *this;
}

CComplex & CComplex::operator -= (const CComplex &c2)
{
	x_real -= c2.x_real;
	y_imag -= c2.y_imag;
	return *this;
}

CComplex & CComplex::operator *= (const CComplex &c2)
{
	x_real = x_real*c2.x_real - y_imag*c2.y_imag;
	y_imag = x_real*c2.y_imag + y_imag*c2.x_real;	
	return *this;
}

CComplex & CComplex::operator /= (const CComplex &c2)
{
	x_real = (x_real*c2.x_real + y_imag*c2.y_imag)/(c2.x_real*c2.x_real+c2.y_imag*c2.y_imag);
	y_imag = (c2.x_real*y_imag - c2.y_imag*x_real)/(c2.x_real*c2.x_real+c2.y_imag*c2.y_imag);
    return *this;
}

CComplex & CComplex::operator += (double d2)
{
	x_real += d2;
	return *this;
}

CComplex & CComplex::operator -= (double d2)
{
	x_real -= d2;
	return *this;
}

CComplex & CComplex::operator *= (double d2)
{
	x_real *= d2;
	y_imag *= d2;
	return *this;
}

CComplex & CComplex::operator /= (double d2)
{
	x_real /= d2;
	y_imag /= d2;
	return *this;
}

int operator == (const CComplex &c1,const CComplex &c2)
{
    if(real(c1)!=real(c2) || imag(c1)!=imag(c2))
			return 0;
	return 1;
}

int operator != (const CComplex &c1, const CComplex &c2)
{
    if(real(c1)!=real(c2) || imag(c1)!=imag(c2))
			return 1;
	return 0;
}

int operator == (const CComplex &c1, double c2)
{
    if(real(c1)!= c2 || (imag(c1)+1)!=1)
			return 0;
	return 1;
}

int operator != (const CComplex &c1, double c2)
{
    if(real(c1)!= c2 || (imag(c1)+1)!=1)
			return 1;
	return 0;
}

int operator == (const CComplex &c1, int c2)
{
    if(real(c1)!= c2 || (imag(c1)+1)!=1)
			return 0;
	return 1;
}

int operator != (const CComplex &c1, int c2)
{
    if(real(c1)!= c2 || (imag(c1)+1)!=1)
			return 1;
	return 0;
}

double CComplex::abs()const 
{
	return ::sqrt(x_real*x_real+y_imag*y_imag);
}

double CComplex::real()const 
{
	return x_real;
}

double CComplex::imag()const 
{
	return y_imag;
}


double abs(const CComplex &c)
{
    return c.abs();
}

double real(const CComplex &c)
{
    return c.real();
}

double imag(const CComplex &c)
{
    return c.imag();
}

CComplex CComplex::conj()const 
{
	CComplex c;
	c.x_real = x_real;
	c.y_imag = -y_imag;
	return c;
}

CComplex conj(const CComplex &c)
{
	return c.conj();
}

CComplex operator + (const CComplex &c1)
{
	return c1;
}

CComplex operator - (const CComplex &c1)
{
	CComplex c;
	c.x_real = -c1.x_real;
	c.y_imag = -c1.y_imag;
	return c;
}

CComplex CComplex::sin()const 
{
	CComplex c;
	c.x_real = ::sin(x_real)*::cosh(y_imag);
	c.y_imag = ::cos(x_real)*::sinh(y_imag);
	return c;
}

CComplex sin(const CComplex &c)
{
    return c.sin();
}

CComplex CComplex::cos()const 
{
	CComplex c;
	c.x_real = ::cos(x_real)*::cosh(y_imag);
	c.y_imag = -::sin(x_real)*::sinh(y_imag);
	return c;
}

CComplex cos(const CComplex &c)
{
    return c.cos();
}

CComplex CComplex::tan()const 
{
	CComplex c;
	c = (*this);
	c = ::sin(c)/::cos(c);
	return c;
}

CComplex CComplex::sinh()const 
{
	CComplex c;
    CComplex i(0,1);
	c = (*this);
	if(y_imag +1 !=1)
	{
	    c = ::sin(c*i)/i;
	}
	else
	{
		c.x_real = ::sinh(x_real);
	    c.y_imag = 0;
	}
	return c;
}

CComplex sinh(const CComplex &c)
{
    return c.sinh();
}

CComplex CComplex::cosh()const 
{
	CComplex c;
    CComplex i(0,1);
	c = (*this);
	if(y_imag +1 !=1)
	{
    	c = ::cos(c*i);
	}
	else
	{
		c.x_real = ::sinh(x_real);
	    c.y_imag = 0;
	}
	return c;
}

CComplex cosh(const CComplex &c)
{
    return c.cos();
}

CComplex CComplex::tanh()const 
{
	CComplex c;
	c = (*this);
	c = ::sinh(c)/::cosh(c);
	return c;
}


CComplex tanh(const CComplex &c)
{
	return c.tanh();
}

CComplex CComplex::log()const 
{
	CComplex c;
	c.y_imag = ::atan(y_imag/x_real);
	c.x_real = ::log(::sqrt(x_real*x_real + y_imag*y_imag));
	if(x_real<0 && y_imag<0)
		c.y_imag -= PI;
	if(x_real<0 && y_imag>=0)
		c.y_imag += PI;
	return c;
}

CComplex log(const CComplex &c)
{
	return c.log();
}

CComplex CComplex::log10()const 
{
	return (*this).log()/::log(10);
}

CComplex log10(const CComplex &c)
{
	return c.log10();
}

CComplex CComplex::exp()const 
{
	CComplex c;
	c.x_real = ::exp(x_real)*::cos(y_imag);
	c.y_imag = ::exp(x_real)*::sin(y_imag);
	return c;
}

CComplex exp(const CComplex &c)
{
	return c.exp();
}

CComplex CComplex::sqrt()const 
{
	CComplex c;
	double a = x_real;
    double b = y_imag;
	if(b+1!=1)
	{
        c.x_real = fabs(b/::sqrt(-2*a+2*::sqrt(a*a+b*b)));
	    c.y_imag = b/(2*c.x_real);
	}
	else 
	{
		if(a>=0)
		{
		    c.x_real = ::sqrt(a);
		    c.y_imag = 0;
		}
		else
		{
			c.x_real = 0;
			c.y_imag = ::sqrt(-a);
		}
	}
	return c;
}

CComplex sqrt(const CComplex &c)
{
	return c.sqrt();
}

CComplex CComplex::asin()const 
{
	CComplex c;
	CComplex i(0,1);
	if(y_imag +1 ==1 && fabs(x_real)<=1)
	{
		
		c.x_real = ::asin(x_real);
		c.y_imag = 0;
	}
	else
	{
		c = (*this)*i;
		c = ::log(c+::sqrt(c*c+1));
		c = c/i;
	}
	if(x_real>0 && y_imag>0)
		c = PI - c;
	if(x_real<0 && y_imag<0)
		c = -PI - c;
	if(x_real >1 && y_imag +1 == 1)
		c.y_imag = -c.y_imag;
	return c;
}

CComplex asin(const CComplex &c)
{
	return c.asin();
}


CComplex CComplex::acos()const 
{
	CComplex c;
	CComplex i(0,1);
	if(y_imag +1 ==1 && fabs(x_real)<=1)
	{
		
		c.x_real = ::acos(x_real);
		c.y_imag = 0;
	}
	else
	{
		c = (*this);
		c = ::log(c+::sqrt(c*c-1));
		c = c/i;
	}
	if(y_imag+1==1 && x_real <-1)
		c = PI - c;
	return c;
}


CComplex acos(const CComplex &c)
{
	return c.acos();
}

CComplex CComplex::atan()const 
{
	CComplex c;
	CComplex i(0,1);
	if(y_imag +1 ==1)
	{
		
		c.x_real = ::atan(x_real);
		c.y_imag = 0;
	}
	else
	{
		c = (*this)*i;
		c = 1.0/2*::log((1+c)/(1-c));
		c = c/i;
	}
	return c;
}

CComplex atan(const CComplex &c)
{
	return c.atan();
}
    
CComplex CComplex::pow(const CComplex &pp)const 
{
	CComplex c;
	c =(*this);
    if(y_imag+1 ==1 && ::imag(pp)+1==1)
	{
		c = ::pow(x_real, ::real(pp));
	}
	else
	{	
	    c = ::exp(pp*::log(c));
	}
	return c;
}

CComplex pow(const CComplex &c,const CComplex &pp)
{
	return c.pow(pp);
}