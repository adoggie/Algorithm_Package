//#if !defined(AFX_TYMATH_H__E7989605_D753_4AFE_83A5_FFA06D4178B6__INCLUDED_)
//#define AFX_TYMATH_H__E7989605_D753_4AFE_83A5_FFA06D4178B6__INCLUDED_
#ifndef _TYMATH_H
#define _TYMATH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define PI 3.14159265358979
#define MAX_COUNT 300
#define MAX_DEGREE 10000
#define DOUBLE_ZERO 1e-20
#define EPS_ACCUR 1e-10
#define MAX_ITERATETIME 3000

#define MIN(a,b) ((a)<(b))?(a):(b)
#define MAX(a,b) ((a)<(b))?(b):(a)
#define TOLERANT_ERROR 1.0e-12

//#define TOLERANT_ERROR 1.0e-40  //zfm



#include <math.h>
#include <vector>
//#include "ErrorReporter.h"
//#include "ErrorReporter.h"
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

class __declspec(dllexport) DistBase  
{
public:
	DistBase();
	virtual ~DistBase();
public:
	virtual	double pdf();
	virtual double cdf();
	virtual double invcdf();
	
protected:
	
};

//正态分布类   1
class __declspec(dllexport) Dist_Norm : public DistBase
{
public:
	Dist_Norm(); // default to be a std normal 
	virtual ~Dist_Norm();
	//probability function
	double pdf(double x,double mu =0.0,double sigma = 1.0); //custom mean and deviation
	
	//cumilate function
	double cdf(double x,double mu =0.0,double sigma = 1.0);	
	
	//inver cumulate function
	double invcdf(double alphax,double mu =0.0,double sigma = 1.0);//return a double value x that cdf(x) = alpha	
	
	//Get  normal random numbers 
	
protected:
};
//t分布函数   2
class __declspec(dllexport) Dist_T  : public DistBase
{
public:
	Dist_T();
	virtual ~Dist_T();
	double pUp(int n, double q);//t分布函数上分位数
	double invcdf(double p, int v);//t分布的概率分布函数的逆
	double invcdf(double p, double v); //t分布的概率分布函数的逆,自由度为double 
	double pdf(double x, int v);//t分布的概率密度函数
	double cdf(double x, int v);//t分布的概率分布函数
	double cdf(double x, double v); //cdf

};
//F分布   3
class __declspec(dllexport) Dist_F : public DistBase  
{
public:
	Dist_F();
	double cdf(double x, int v1, int v2);//F分布的概率分布函数
	double pdf(double x, int v1, int v2);//F分布的概率密度函数
	double invcdf(double p, int v1, int v2);//F分布的概率分布函数的逆	
	double pUp(int n1, int n2, double q);//F分布函数上分位数
	
};
//均匀分布   4
class __declspec(dllexport) Dist_Uniform : public DistBase  
{
public:
	Dist_Uniform();
	double pdf(double x, double a = 0.0, double b = 1.0);//均匀分布的概率密度函数
	double cdf(double x, double a = 0.0, double b = 1.0);//均匀分布的概率分布函数
	double invcdf(double p, double a = 0.0, double b = 1.0);//均匀分布的概率分布函数的逆		
};
//指数分布   5
class __declspec(dllexport) Dist_Exp : public DistBase
{
public:
	double pdf(double x, double mu);//指数分布的概率密度函数
	double cdf(double x, double mu);//指数分布的概率分布函数
	double invcdf(double p, double mu);//指数分布的概率分布函数的逆
	
protected:
private:
};
//卡方分布   6
class __declspec(dllexport) Dist_Chi2 :public DistBase
{
public:
	double cdf(double x, int v);//卡方分布的概率分布函数
	double pdf(double x, int v);//卡方分布的概率密度函数
	double invcdf(double p, int v);//卡方分布的概率分布函数的逆
	
protected:
private:
};
//对数正态分布 7
class __declspec(dllexport) Dist_LogNorm : public DistBase
{
public:
	double pdf(double x, double mu, double sigma);//对数正态分布的概率密度函数
	double cdf(double x, double mu, double sigma);//对数正态分布的概率分布函数
	double invcdf(double p, double mu, double sigma);//对数正态分布的概率分布函数的逆
	
protected:
private:
};
//泊松分布   8
class __declspec(dllexport) Dist_Poisson : public DistBase
{
public:
	double pdf(int x, double lamda);//泊松分布的点概率函数
	double cdf(int x, double lamda);//泊松分布的概率分布函数
	double invcdf(double p, double lamda);//泊松分布的概率分布函数的逆
	
protected:
private:
};
//威布尔分布   9
class __declspec(dllexport) Dist_Weibull : public DistBase
{
public:
	double pdf(double x, double a, double b);//威布尔分布的概率密度函数
	double cdf(double x, double a, double b);//威布尔分布的概率分布函数
	double invcdf(double p, double a, double b);//威布尔分布的概率分布函数的逆
	
protected:
private:
};
//Gamma分布   10
class __declspec(dllexport) Dist_Gamma : public DistBase
{
public:
	double gamma(double x);//gamma函数
	double gamma(double a, double x);//不完全gamma函数
	double gammaln(double x);//gamma函数取对数
	double cdf(double x, double a, double b);//Gamma分布的概率分布函数
	double pdf(double x, double a, double b);//Gamma分布的概率密度函数
	double invcdf(double p, double a, double b);//Gamma分布的概率分布函数的逆
	
protected:
private:
};
//几何分布  11
class __declspec(dllexport) Dist_Geometric : public DistBase
{
public:
	double pdf(double x, double p);//几何分布的点概率函数
	double cdf(double x, double p);//几何分布的概率分布函数
	double invcdf(double y, double p);//几何分布的概率分布函数的逆
	
protected:
private:
};
//Beta分布  12
class __declspec(dllexport) Dist_Beta : public DistBase
{
public:
	double cdf(double x, double a, double b);//Beta分布的概率分布函数	
	double pdf(double x, double a, double b);//Beta分布的概率密度函数
	double invcdf(double p, double a, double b);//Beta分布的概率分布函数的逆
	double betacore(double x, double a, double b);//Beta函数的计算
protected:
private:
};

//超几何分布   13
class __declspec(dllexport) Dist_hygeo : public DistBase 
{
public:
	double pdf(int x, int m, int k, int n);//超几何分布的点概率函数
	double cdf(int x, int m, int k, int n);//超几何分布的概率分布函数
	double invcdf(double p, int m, int k, int n);//超几何分布的概率分布函数的逆
	
protected:
private:
};

//半正态分布  14
class __declspec(dllexport) Dist_halfnorm : public  DistBase
{
public:
	double pdf(double x, double theta);//半正态分布的概率密度函数
	double cdf(double x, double theta);//半正态分布的概率分布函数
	double invcdf(double p, double theta);//半正态分布的概率分布函数的逆
	
protected:
private:
};
//Laplace分布  15
class __declspec(dllexport) Dist_Laplace : public DistBase
{
public:
	double pdf(double x, double b, double mu);//Laplace分布的概率密度函数
	double cdf(double x, double b, double mu);//Laplace分布的概率分布函数
	double invcdf(double p, double b, double mu);//Laplace分布的概率分布函数的逆
	
protected:
private:
};
//Logistic分布  16
class __declspec(dllexport) Dist_Logist : public DistBase
{
public:
	double pdf(double x, double m, double b);//Logistic分布的概率密度函数
	double cdf(double x, double m, double b);//Logistic分布的概率分布函数
	double invcdf(double p, double m, double b);//Logistic分布的概率分布函数的逆
	
protected:
private:
};
//Pareto分布   17
class __declspec(dllexport) Dist_Pareto : public DistBase
{
public:
	double pdf(double x, double a, double b);//Pareto分布的概率密度函数
	double cdf(double x, double a, double b);//Pareto分布的概率分布函数
	double invcdf(double p, double a, double b);//Pareto分布的概率分布函数的逆
	
protected:
private:
};
//durbin-waston检验  18
class __declspec(dllexport) Dist_DWTest 
{
public:
	   double Upcdf(int n, int k, double r);//durbin-waston检验上限分布函数
	   double Lowcdf(int n, int k, double r);//durbin-waston检验下限分布函数
	   double Uppdf(int n, int k, double r);//durbin-waston检验上限概率密度函数
	   double Lowlpdf(int n, int k, double r);//durbin-waston检验下限概率密度函数
	   double Upinv(int n, int k, double p);//durbin-waston检验上限分布函数的逆
	   double Lowinv(int n, int k, double p);//durbin-waston检验下限分布函数的逆
	   
protected:
private:
};

//studentized-range 分布   19
class __declspec(dllexport) Dist_StuRang 
{
public:
	double cdf(double q, int v, int r, int jmax=15, int kmax=15);//studentized-range 分布函数
	double invcdf(double p, int v, int r);//studentized-range 分布函数的逆
	
protected:
private:
};

class __declspec(dllexport) TCMethod
{
	__declspec(dllexport) friend long factorial(int n);
	__declspec(dllexport) friend long nchoosek(int n, int k);
	__declspec(dllexport) friend double betacore(double x, double a, double b);
	__declspec(dllexport) friend double gammainc(double x, double a);//不完全Gamma函数
	
	__declspec(dllexport) friend double betainc(double x, double a, double b);//不完全Beta函数
	__declspec(dllexport) friend double beta(double z, double w);//Beta函数
	__declspec(dllexport) friend double betapdf(double x, double a, double b);
	__declspec(dllexport) friend double betacdf(double x, double a, double b);
	__declspec(dllexport) friend double betainv(double p, double a, double b);
	
	__declspec(dllexport) friend double norm(double x);//标准正态分布函数
	__declspec(dllexport) friend double normcdf(double x, double mu, double sigma);//正态分布的概率分布函数
	__declspec(dllexport) friend double pnorm(double p);//标准正态分布函数上分位数
	__declspec(dllexport) friend double normpdf(double x, double mu, double sigma);//正态分布的概率密度函数
	__declspec(dllexport) friend double norminv(double p, double mu, double sigma);//正态分布的概率分布函数的逆
	
	__declspec(dllexport) friend double lgam1(double x);//gamma函数
	__declspec(dllexport) friend double lgam2(double a, double x);//不完全gamma函数
	__declspec(dllexport) friend double gammaln(double x);//gamma函数取对数
	__declspec(dllexport) friend double gamcdf(double x, double a, double b);//Gamma分布的概率分布函数
	__declspec(dllexport) friend double gampdf(double x, double a, double b);//Gamma分布的概率密度函数
	__declspec(dllexport) friend double gaminv(double p, double a, double b);//Gamma分布的概率分布函数的逆
	
	__declspec(dllexport) friend double lerrf(double x);//误差函数
	__declspec(dllexport) friend double erfc(double x);//余误差函数
	__declspec(dllexport) friend double erf(double x);//误差函数
	
	
	__declspec(dllexport) friend double tdist(double t, int n, double *dd);//t分布函数和概率密度
	__declspec(dllexport) friend double ptdist(int n, double q);//t分布函数上分位数
	__declspec(dllexport) friend double tinv(double p, int v);//t分布的概率分布函数的逆
	__declspec(dllexport) friend double tinv(double p, double v); //t分布的概率分布函数的逆,自由度为double zfm
	__declspec(dllexport) friend double tpdf(double x, int v);//t分布的概率密度函数
	__declspec(dllexport) friend double tpdf1(double x, int v);//t分布的概率密度函数
	__declspec(dllexport) friend double tcdf(double x, int v);//t分布的概率分布函数
	__declspec(dllexport) friend double tcdf1(double x, int v);//t分布的概率分布函数
	__declspec(dllexport) friend double tcdf(double x, double v); //zfm
	

	__declspec(dllexport) friend void pearson(double *x, int k, int n, double *r);//pearson相关系数
	__declspec(dllexport) friend double partf(double r1, double r2, double r3);
	__declspec(dllexport) friend double partial(double *r, int *pos, int count, int n);//偏相关系数求解
	__declspec(dllexport) friend double ** twoarrayalloc(int m, int n);
	 

	__declspec(dllexport) friend int Matinv(double *a, int n);//叶尔绍夫法求方阵逆
	__declspec(dllexport) friend void orithes(int n, double tol, double **a, double *d);//用正交变换将一般实矩阵化简为上hessenberger形
    __declspec(dllexport) friend void ortrans(int n, double **h, double *d, double **v);//从orithes的结果中提取正交变换阵
	__declspec(dllexport) friend void sss(double *fg, double *cs);
    __declspec(dllexport) friend void cdiv(double xr, double xi, double yr, double yi, double *zr, double *zi);
    __declspec(dllexport) friend int hqr2(int n, double ep, double **h, double **v, double *wr, double *wi, int *cnt);//用原点移动的QR算法及LQ分解求上hessenberger形矩阵的特征值和特征向量

	
	
	__declspec(dllexport) friend double exppdf(double x, double mu);//指数分布的概率密度函数
	__declspec(dllexport) friend double expcdf(double x, double mu);//指数分布的概率分布函数
	__declspec(dllexport) friend double expinv(double p, double mu);//指数分布的概率分布函数的逆
	
	__declspec(dllexport) friend double geopdf(double x, double p);//几何分布的点概率函数
	__declspec(dllexport) friend double geocdf(double x, double p);//几何分布的概率分布函数
	__declspec(dllexport) friend double geoinv(double y, double p);//几何分布的概率分布函数的逆
	
	__declspec(dllexport) friend double hygepdf(int x, int m, int k, int n);//超几何分布的点概率函数
	__declspec(dllexport) friend double hygecdf(int x, int m, int k, int n);//超几何分布的概率分布函数
	__declspec(dllexport) friend double hygeinv(double p, int m, int k, int n);//超几何分布的概率分布函数的逆
	
	__declspec(dllexport) friend double poisspdf(int x, double lamda);//泊松分布的点概率函数
	__declspec(dllexport) friend double poisscdf(int x, double lamda);//泊松分布的概率分布函数
	__declspec(dllexport) friend double poissinv(double p, double lamda);//泊松分布的概率分布函数的逆
	
	__declspec(dllexport) friend double lognpdf(double x, double mu, double sigma);//对数正态分布的概率密度函数
	__declspec(dllexport) friend double logncdf(double x, double mu, double sigma);//对数正态分布的概率分布函数
	__declspec(dllexport) friend double logninv(double p, double mu, double sigma);//对数正态分布的概率分布函数的逆
	
	__declspec(dllexport) friend double weibpdf(double x, double a, double b);//威布尔分布的概率密度函数
	__declspec(dllexport) friend double weibcdf(double x, double a, double b);//威布尔分布的概率分布函数
	__declspec(dllexport) friend double weibinv(double p, double a, double b);//威布尔分布的概率分布函数的逆
	
	__declspec(dllexport) friend double halfnpdf(double x, double theta);//半正态分布的概率密度函数
	__declspec(dllexport) friend double halfncdf(double x, double theta);//半正态分布的概率分布函数
	__declspec(dllexport) friend double halfninv(double p, double theta);//半正态分布的概率分布函数的逆
	
	__declspec(dllexport) friend double laplacepdf(double x, double b, double mu);//Laplace分布的概率密度函数
	__declspec(dllexport) friend double laplacecdf(double x, double b, double mu);//Laplace分布的概率分布函数
	__declspec(dllexport) friend double laplaceinv(double p, double b, double mu);//Laplace分布的概率分布函数的逆
	
	__declspec(dllexport) friend double logistpdf(double x, double m, double b);//Logistic分布的概率密度函数
	__declspec(dllexport) friend double logistcdf(double x, double m, double b);//Logistic分布的概率分布函数
	__declspec(dllexport) friend double logistinv(double p, double m, double b);//Logistic分布的概率分布函数的逆
	
	__declspec(dllexport) friend double paretopdf(double x, double a, double b);//Pareto分布的概率密度函数
	__declspec(dllexport) friend double paretocdf(double x, double a, double b);//Pareto分布的概率分布函数
	__declspec(dllexport) friend double paretoinv(double p, double a, double b);//Pareto分布的概率分布函数的逆
	
	__declspec(dllexport) friend double uniformpdf(double x, double a, double b);//均匀分布的概率密度函数
	__declspec(dllexport) friend double uniformcdf(double x, double a, double b);//均匀分布的概率分布函数
	__declspec(dllexport) friend double uniforminv(double p, double a, double b);//均匀分布的概率分布函数的逆
	

	__declspec(dllexport) friend double chi2cdf(double x, int v);//卡方分布的概率分布函数
	__declspec(dllexport) friend double chi2cdf1(double x, int v);//卡方分布的概率分布函数
	__declspec(dllexport) friend double chi2(double ch, int n, double *d);//卡方分布函数和概率密度	
	__declspec(dllexport) friend double chi2pdf(double x, int v);//卡方分布的概率密度函数
	__declspec(dllexport) friend double chi2pdf1(double x, int v);//卡方分布的概率密度函数
	__declspec(dllexport) friend double chi2inv(double p, int v);//卡方分布的概率分布函数的逆
	
	__declspec(dllexport) friend double fcdf(double x, int v1, int v2);//F分布的概率分布函数
	__declspec(dllexport) friend double fcdf1(double x, int v1, int v2);//F分布的概率分布函数
	__declspec(dllexport) friend double fpdf(double x, int v1, int v2);//F分布的概率密度函数
	__declspec(dllexport) friend double fpdf1(double x, int v1, int v2);//F分布的概率密度函数
	__declspec(dllexport) friend double finv(double p, int v1, int v2);//F分布的概率分布函数的逆	
	__declspec(dllexport) friend double fdist(int n1, int n2, double f, double *d);//F分布函数和概率密度
	__declspec(dllexport) friend double pfdist(int n1, int n2, double q);//F分布函数上分位数
	
	

	__declspec(dllexport) friend double stdrcdf(double q, int v, int r, int jmax=15, int kmax=15);//studentized-range 分布函数
	__declspec(dllexport) friend double stdrinv(double p, int v, int r);//studentized-range 分布函数的逆
	__declspec(dllexport) friend double qtrng0(double p, int v, int r);

    __declspec(dllexport) friend double dwhcdf(int n, int k, double r);//durbin-waston检验上限分布函数
    __declspec(dllexport) friend double dwlcdf(int n, int k, double r);//durbin-waston检验下限分布函数
    __declspec(dllexport) friend double dwhpdf(int n, int k, double r);//durbin-waston检验上限概率密度函数
    __declspec(dllexport) friend double dwlpdf(int n, int k, double r);//durbin-waston检验下限概率密度函数
    __declspec(dllexport) friend double dwhinv(int n, int k, double p);//durbin-waston检验上限分布函数的逆
    __declspec(dllexport) friend double dwlinv(int n, int k, double p);//durbin-waston检验下限分布函数的逆
    
	__declspec(dllexport) friend double pfun(int n, int k, double r, double s, double *lamta);
    __declspec(dllexport) friend double pdfun(int n, int k, double r, double s, double *lamta);
    __declspec(dllexport) friend double sfun(double s, int n, int k, double *lamta);
    __declspec(dllexport) friend double bfun(int n, int k, double *lamta);
    __declspec(dllexport) friend void lamtax(double *lamta, int n, int k, double r, int flag);
    __declspec(dllexport) friend double invfun(int n, int k,double p, int flag);
    __declspec(dllexport) friend double d2cdf(double v);


	__declspec(dllexport) friend double* cubicfit(int n, double *x,double *y, double* &cubic, double* &square, double* &one, bool &bYes);//三次样条函数插值 cubic,square,one分别为三次,二次,一次系数,bool默认可以做三次插值 2003-3-20 zfm 

	

 
	__declspec(dllexport) friend void WriteData(double var,char * title); //zfm
	__declspec(dllexport) friend void WriteData(int    var,char * title); //zfm
	__declspec(dllexport) friend void WriteData(double *var,int n,char * title=NULL); //zfm
	__declspec(dllexport) friend void WriteData(int *var,int n,char * title=NULL); //zfm
	__declspec(dllexport) friend void WriteData(double **var,int m,int n,char * title=NULL); //zfm
	
};

#endif
