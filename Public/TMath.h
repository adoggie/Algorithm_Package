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

//��̬�ֲ���   1
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
//t�ֲ�����   2
class __declspec(dllexport) Dist_T  : public DistBase
{
public:
	Dist_T();
	virtual ~Dist_T();
	double pUp(int n, double q);//t�ֲ������Ϸ�λ��
	double invcdf(double p, int v);//t�ֲ��ĸ��ʷֲ���������
	double invcdf(double p, double v); //t�ֲ��ĸ��ʷֲ���������,���ɶ�Ϊdouble 
	double pdf(double x, int v);//t�ֲ��ĸ����ܶȺ���
	double cdf(double x, int v);//t�ֲ��ĸ��ʷֲ�����
	double cdf(double x, double v); //cdf

};
//F�ֲ�   3
class __declspec(dllexport) Dist_F : public DistBase  
{
public:
	Dist_F();
	double cdf(double x, int v1, int v2);//F�ֲ��ĸ��ʷֲ�����
	double pdf(double x, int v1, int v2);//F�ֲ��ĸ����ܶȺ���
	double invcdf(double p, int v1, int v2);//F�ֲ��ĸ��ʷֲ���������	
	double pUp(int n1, int n2, double q);//F�ֲ������Ϸ�λ��
	
};
//���ȷֲ�   4
class __declspec(dllexport) Dist_Uniform : public DistBase  
{
public:
	Dist_Uniform();
	double pdf(double x, double a = 0.0, double b = 1.0);//���ȷֲ��ĸ����ܶȺ���
	double cdf(double x, double a = 0.0, double b = 1.0);//���ȷֲ��ĸ��ʷֲ�����
	double invcdf(double p, double a = 0.0, double b = 1.0);//���ȷֲ��ĸ��ʷֲ���������		
};
//ָ���ֲ�   5
class __declspec(dllexport) Dist_Exp : public DistBase
{
public:
	double pdf(double x, double mu);//ָ���ֲ��ĸ����ܶȺ���
	double cdf(double x, double mu);//ָ���ֲ��ĸ��ʷֲ�����
	double invcdf(double p, double mu);//ָ���ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//�����ֲ�   6
class __declspec(dllexport) Dist_Chi2 :public DistBase
{
public:
	double cdf(double x, int v);//�����ֲ��ĸ��ʷֲ�����
	double pdf(double x, int v);//�����ֲ��ĸ����ܶȺ���
	double invcdf(double p, int v);//�����ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//������̬�ֲ� 7
class __declspec(dllexport) Dist_LogNorm : public DistBase
{
public:
	double pdf(double x, double mu, double sigma);//������̬�ֲ��ĸ����ܶȺ���
	double cdf(double x, double mu, double sigma);//������̬�ֲ��ĸ��ʷֲ�����
	double invcdf(double p, double mu, double sigma);//������̬�ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//���ɷֲ�   8
class __declspec(dllexport) Dist_Poisson : public DistBase
{
public:
	double pdf(int x, double lamda);//���ɷֲ��ĵ���ʺ���
	double cdf(int x, double lamda);//���ɷֲ��ĸ��ʷֲ�����
	double invcdf(double p, double lamda);//���ɷֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//�������ֲ�   9
class __declspec(dllexport) Dist_Weibull : public DistBase
{
public:
	double pdf(double x, double a, double b);//�������ֲ��ĸ����ܶȺ���
	double cdf(double x, double a, double b);//�������ֲ��ĸ��ʷֲ�����
	double invcdf(double p, double a, double b);//�������ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//Gamma�ֲ�   10
class __declspec(dllexport) Dist_Gamma : public DistBase
{
public:
	double gamma(double x);//gamma����
	double gamma(double a, double x);//����ȫgamma����
	double gammaln(double x);//gamma����ȡ����
	double cdf(double x, double a, double b);//Gamma�ֲ��ĸ��ʷֲ�����
	double pdf(double x, double a, double b);//Gamma�ֲ��ĸ����ܶȺ���
	double invcdf(double p, double a, double b);//Gamma�ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//���ηֲ�  11
class __declspec(dllexport) Dist_Geometric : public DistBase
{
public:
	double pdf(double x, double p);//���ηֲ��ĵ���ʺ���
	double cdf(double x, double p);//���ηֲ��ĸ��ʷֲ�����
	double invcdf(double y, double p);//���ηֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//Beta�ֲ�  12
class __declspec(dllexport) Dist_Beta : public DistBase
{
public:
	double cdf(double x, double a, double b);//Beta�ֲ��ĸ��ʷֲ�����	
	double pdf(double x, double a, double b);//Beta�ֲ��ĸ����ܶȺ���
	double invcdf(double p, double a, double b);//Beta�ֲ��ĸ��ʷֲ���������
	double betacore(double x, double a, double b);//Beta�����ļ���
protected:
private:
};

//�����ηֲ�   13
class __declspec(dllexport) Dist_hygeo : public DistBase 
{
public:
	double pdf(int x, int m, int k, int n);//�����ηֲ��ĵ���ʺ���
	double cdf(int x, int m, int k, int n);//�����ηֲ��ĸ��ʷֲ�����
	double invcdf(double p, int m, int k, int n);//�����ηֲ��ĸ��ʷֲ���������
	
protected:
private:
};

//����̬�ֲ�  14
class __declspec(dllexport) Dist_halfnorm : public  DistBase
{
public:
	double pdf(double x, double theta);//����̬�ֲ��ĸ����ܶȺ���
	double cdf(double x, double theta);//����̬�ֲ��ĸ��ʷֲ�����
	double invcdf(double p, double theta);//����̬�ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//Laplace�ֲ�  15
class __declspec(dllexport) Dist_Laplace : public DistBase
{
public:
	double pdf(double x, double b, double mu);//Laplace�ֲ��ĸ����ܶȺ���
	double cdf(double x, double b, double mu);//Laplace�ֲ��ĸ��ʷֲ�����
	double invcdf(double p, double b, double mu);//Laplace�ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//Logistic�ֲ�  16
class __declspec(dllexport) Dist_Logist : public DistBase
{
public:
	double pdf(double x, double m, double b);//Logistic�ֲ��ĸ����ܶȺ���
	double cdf(double x, double m, double b);//Logistic�ֲ��ĸ��ʷֲ�����
	double invcdf(double p, double m, double b);//Logistic�ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//Pareto�ֲ�   17
class __declspec(dllexport) Dist_Pareto : public DistBase
{
public:
	double pdf(double x, double a, double b);//Pareto�ֲ��ĸ����ܶȺ���
	double cdf(double x, double a, double b);//Pareto�ֲ��ĸ��ʷֲ�����
	double invcdf(double p, double a, double b);//Pareto�ֲ��ĸ��ʷֲ���������
	
protected:
private:
};
//durbin-waston����  18
class __declspec(dllexport) Dist_DWTest 
{
public:
	   double Upcdf(int n, int k, double r);//durbin-waston�������޷ֲ�����
	   double Lowcdf(int n, int k, double r);//durbin-waston�������޷ֲ�����
	   double Uppdf(int n, int k, double r);//durbin-waston�������޸����ܶȺ���
	   double Lowlpdf(int n, int k, double r);//durbin-waston�������޸����ܶȺ���
	   double Upinv(int n, int k, double p);//durbin-waston�������޷ֲ���������
	   double Lowinv(int n, int k, double p);//durbin-waston�������޷ֲ���������
	   
protected:
private:
};

//studentized-range �ֲ�   19
class __declspec(dllexport) Dist_StuRang 
{
public:
	double cdf(double q, int v, int r, int jmax=15, int kmax=15);//studentized-range �ֲ�����
	double invcdf(double p, int v, int r);//studentized-range �ֲ���������
	
protected:
private:
};

class __declspec(dllexport) TCMethod
{
	__declspec(dllexport) friend long factorial(int n);
	__declspec(dllexport) friend long nchoosek(int n, int k);
	__declspec(dllexport) friend double betacore(double x, double a, double b);
	__declspec(dllexport) friend double gammainc(double x, double a);//����ȫGamma����
	
	__declspec(dllexport) friend double betainc(double x, double a, double b);//����ȫBeta����
	__declspec(dllexport) friend double beta(double z, double w);//Beta����
	__declspec(dllexport) friend double betapdf(double x, double a, double b);
	__declspec(dllexport) friend double betacdf(double x, double a, double b);
	__declspec(dllexport) friend double betainv(double p, double a, double b);
	
	__declspec(dllexport) friend double norm(double x);//��׼��̬�ֲ�����
	__declspec(dllexport) friend double normcdf(double x, double mu, double sigma);//��̬�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double pnorm(double p);//��׼��̬�ֲ������Ϸ�λ��
	__declspec(dllexport) friend double normpdf(double x, double mu, double sigma);//��̬�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double norminv(double p, double mu, double sigma);//��̬�ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double lgam1(double x);//gamma����
	__declspec(dllexport) friend double lgam2(double a, double x);//����ȫgamma����
	__declspec(dllexport) friend double gammaln(double x);//gamma����ȡ����
	__declspec(dllexport) friend double gamcdf(double x, double a, double b);//Gamma�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double gampdf(double x, double a, double b);//Gamma�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double gaminv(double p, double a, double b);//Gamma�ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double lerrf(double x);//����
	__declspec(dllexport) friend double erfc(double x);//������
	__declspec(dllexport) friend double erf(double x);//����
	
	
	__declspec(dllexport) friend double tdist(double t, int n, double *dd);//t�ֲ������͸����ܶ�
	__declspec(dllexport) friend double ptdist(int n, double q);//t�ֲ������Ϸ�λ��
	__declspec(dllexport) friend double tinv(double p, int v);//t�ֲ��ĸ��ʷֲ���������
	__declspec(dllexport) friend double tinv(double p, double v); //t�ֲ��ĸ��ʷֲ���������,���ɶ�Ϊdouble zfm
	__declspec(dllexport) friend double tpdf(double x, int v);//t�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double tpdf1(double x, int v);//t�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double tcdf(double x, int v);//t�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double tcdf1(double x, int v);//t�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double tcdf(double x, double v); //zfm
	

	__declspec(dllexport) friend void pearson(double *x, int k, int n, double *r);//pearson���ϵ��
	__declspec(dllexport) friend double partf(double r1, double r2, double r3);
	__declspec(dllexport) friend double partial(double *r, int *pos, int count, int n);//ƫ���ϵ�����
	__declspec(dllexport) friend double ** twoarrayalloc(int m, int n);
	 

	__declspec(dllexport) friend int Matinv(double *a, int n);//Ҷ���ܷ�������
	__declspec(dllexport) friend void orithes(int n, double tol, double **a, double *d);//�������任��һ��ʵ���󻯼�Ϊ��hessenberger��
    __declspec(dllexport) friend void ortrans(int n, double **h, double *d, double **v);//��orithes�Ľ������ȡ�����任��
	__declspec(dllexport) friend void sss(double *fg, double *cs);
    __declspec(dllexport) friend void cdiv(double xr, double xi, double yr, double yi, double *zr, double *zi);
    __declspec(dllexport) friend int hqr2(int n, double ep, double **h, double **v, double *wr, double *wi, int *cnt);//��ԭ���ƶ���QR�㷨��LQ�ֽ�����hessenberger�ξ��������ֵ����������

	
	
	__declspec(dllexport) friend double exppdf(double x, double mu);//ָ���ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double expcdf(double x, double mu);//ָ���ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double expinv(double p, double mu);//ָ���ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double geopdf(double x, double p);//���ηֲ��ĵ���ʺ���
	__declspec(dllexport) friend double geocdf(double x, double p);//���ηֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double geoinv(double y, double p);//���ηֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double hygepdf(int x, int m, int k, int n);//�����ηֲ��ĵ���ʺ���
	__declspec(dllexport) friend double hygecdf(int x, int m, int k, int n);//�����ηֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double hygeinv(double p, int m, int k, int n);//�����ηֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double poisspdf(int x, double lamda);//���ɷֲ��ĵ���ʺ���
	__declspec(dllexport) friend double poisscdf(int x, double lamda);//���ɷֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double poissinv(double p, double lamda);//���ɷֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double lognpdf(double x, double mu, double sigma);//������̬�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double logncdf(double x, double mu, double sigma);//������̬�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double logninv(double p, double mu, double sigma);//������̬�ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double weibpdf(double x, double a, double b);//�������ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double weibcdf(double x, double a, double b);//�������ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double weibinv(double p, double a, double b);//�������ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double halfnpdf(double x, double theta);//����̬�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double halfncdf(double x, double theta);//����̬�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double halfninv(double p, double theta);//����̬�ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double laplacepdf(double x, double b, double mu);//Laplace�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double laplacecdf(double x, double b, double mu);//Laplace�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double laplaceinv(double p, double b, double mu);//Laplace�ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double logistpdf(double x, double m, double b);//Logistic�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double logistcdf(double x, double m, double b);//Logistic�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double logistinv(double p, double m, double b);//Logistic�ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double paretopdf(double x, double a, double b);//Pareto�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double paretocdf(double x, double a, double b);//Pareto�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double paretoinv(double p, double a, double b);//Pareto�ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double uniformpdf(double x, double a, double b);//���ȷֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double uniformcdf(double x, double a, double b);//���ȷֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double uniforminv(double p, double a, double b);//���ȷֲ��ĸ��ʷֲ���������
	

	__declspec(dllexport) friend double chi2cdf(double x, int v);//�����ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double chi2cdf1(double x, int v);//�����ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double chi2(double ch, int n, double *d);//�����ֲ������͸����ܶ�	
	__declspec(dllexport) friend double chi2pdf(double x, int v);//�����ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double chi2pdf1(double x, int v);//�����ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double chi2inv(double p, int v);//�����ֲ��ĸ��ʷֲ���������
	
	__declspec(dllexport) friend double fcdf(double x, int v1, int v2);//F�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double fcdf1(double x, int v1, int v2);//F�ֲ��ĸ��ʷֲ�����
	__declspec(dllexport) friend double fpdf(double x, int v1, int v2);//F�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double fpdf1(double x, int v1, int v2);//F�ֲ��ĸ����ܶȺ���
	__declspec(dllexport) friend double finv(double p, int v1, int v2);//F�ֲ��ĸ��ʷֲ���������	
	__declspec(dllexport) friend double fdist(int n1, int n2, double f, double *d);//F�ֲ������͸����ܶ�
	__declspec(dllexport) friend double pfdist(int n1, int n2, double q);//F�ֲ������Ϸ�λ��
	
	

	__declspec(dllexport) friend double stdrcdf(double q, int v, int r, int jmax=15, int kmax=15);//studentized-range �ֲ�����
	__declspec(dllexport) friend double stdrinv(double p, int v, int r);//studentized-range �ֲ���������
	__declspec(dllexport) friend double qtrng0(double p, int v, int r);

    __declspec(dllexport) friend double dwhcdf(int n, int k, double r);//durbin-waston�������޷ֲ�����
    __declspec(dllexport) friend double dwlcdf(int n, int k, double r);//durbin-waston�������޷ֲ�����
    __declspec(dllexport) friend double dwhpdf(int n, int k, double r);//durbin-waston�������޸����ܶȺ���
    __declspec(dllexport) friend double dwlpdf(int n, int k, double r);//durbin-waston�������޸����ܶȺ���
    __declspec(dllexport) friend double dwhinv(int n, int k, double p);//durbin-waston�������޷ֲ���������
    __declspec(dllexport) friend double dwlinv(int n, int k, double p);//durbin-waston�������޷ֲ���������
    
	__declspec(dllexport) friend double pfun(int n, int k, double r, double s, double *lamta);
    __declspec(dllexport) friend double pdfun(int n, int k, double r, double s, double *lamta);
    __declspec(dllexport) friend double sfun(double s, int n, int k, double *lamta);
    __declspec(dllexport) friend double bfun(int n, int k, double *lamta);
    __declspec(dllexport) friend void lamtax(double *lamta, int n, int k, double r, int flag);
    __declspec(dllexport) friend double invfun(int n, int k,double p, int flag);
    __declspec(dllexport) friend double d2cdf(double v);


	__declspec(dllexport) friend double* cubicfit(int n, double *x,double *y, double* &cubic, double* &square, double* &one, bool &bYes);//��������������ֵ cubic,square,one�ֱ�Ϊ����,����,һ��ϵ��,boolĬ�Ͽ��������β�ֵ 2003-3-20 zfm 

	

 
	__declspec(dllexport) friend void WriteData(double var,char * title); //zfm
	__declspec(dllexport) friend void WriteData(int    var,char * title); //zfm
	__declspec(dllexport) friend void WriteData(double *var,int n,char * title=NULL); //zfm
	__declspec(dllexport) friend void WriteData(int *var,int n,char * title=NULL); //zfm
	__declspec(dllexport) friend void WriteData(double **var,int m,int n,char * title=NULL); //zfm
	
};

#endif
