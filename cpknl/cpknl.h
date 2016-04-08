#ifndef _CPKNL_H
#define _CPKNL_H
#define OLD_VERSION   //ԭ�а汾ʹ��  ��Ϊ��������
#define PI 3.14159265358979
#define MAX_COUNT 300
#define MAX_DEGREE 10000
#define DOUBLE_ZERO 1e-20
#define EPS_ACCUR 1e-10
#include "Defines.h"
#include "../Public/common.h"
#include "../Public/TString.h"
#include "ErrorReporter.h"
#include "Vector.h"
#include "Matrix.h"
#include "RandFactory.h"
#define EXPORT_ AFX_EXT_CLASS friend 
//#define EXPORT_ static


class AFX_EXT_CLASS CMethod  
{
public:	
	CMethod();
	virtual ~CMethod();	
	EXPORT_ void Test();
		
	EXPORT_ void ErrorHandler(Matvec_errors Errornum, char * file, int line);
	EXPORT_ void WarningHandler(Matvec_warnings Warningnum, char * file, int line);
	EXPORT_ long factorial(int n);
	EXPORT_ long nchoosek(int n, int k);
	EXPORT_ double betacore(double x, double a, double b);
	EXPORT_ double gammainc(double x, double a);//����ȫGamma����
	EXPORT_ double betainc(double x, double a, double b);//����ȫBeta����
	EXPORT_ double beta(double z, double w);//Beta����
	EXPORT_ double bindist(double p, int n, int k);

	EXPORT_ double erfc(double x);//������
	EXPORT_ double erf(double x);//����

	EXPORT_ double norm(double x);//��׼��̬�ֲ�����

	EXPORT_ double lgam1(double x);//gamma����
	EXPORT_ double lgam2(double a, double x);//����ȫgamma����
	EXPORT_ double lerrf(double x);//����

	EXPORT_ double pnorm(double p);//��׼��̬�ֲ������Ϸ�λ��
	EXPORT_ double chi2(double ch, int n, double d);//�����ֲ������͸����ܶ�
	EXPORT_ double gammaln(double x);//gamma����ȡ����
	EXPORT_ double tdist(double t, int n, double &dd);//t�ֲ������͸����ܶ�
	EXPORT_ double ptdist(int n, double q);//t�ֲ������Ϸ�λ��
	EXPORT_ double fdist(int n1, int n2, double f, double &d);//F�ֲ������͸����ܶ�
	EXPORT_ double pfdist(int n1, int n2, double q);//F�ֲ������Ϸ�λ��

	EXPORT_ void pearson(CDoubleVector x, int k, int n, CDoubleVector r);//pearson���ϵ��
	EXPORT_ double partf(double r1, double r2, double r3);
	//EXPORT_ double partial(CDoubleVector r, CIntVector pos, int count, int n);//ƫ���ϵ�����
	EXPORT_ double partial(CDoubleVector Vect_R, CIntVector Vect_Pos, int count, int n);//ƫ���ϵ�����
	    
	EXPORT_ double ** twoarrayalloc(int m, int n);

	EXPORT_ void twoarrayfree(CDoubleMatrix y, int m);
	EXPORT_ int Matdet(CMatrix<double>m, int numcol, double &det);//ȫ��Ԫ��˹��ȥ����������ʽ
    EXPORT_ int Matinv(CMatrix<double>a, int n);//Ҷ���ܷ�������
    EXPORT_ int Matinv1(CMatrix<double>invary, int numcol, double &det);//ȫ��Ԫ��˹��ȥ�������������ʽ
    EXPORT_ void ppp(CMatrix<double> a, CDoubleVector e, CDoubleVector s,CDoubleMatrix v, int m, int n);
	EXPORT_ int dluav(CMatrix<double> a, CDoubleMatrix u,CDoubleMatrix v, int m, int n, double eps);//��householder�任�ͱ���QR�㷨��һ��������SVD�ֽ�
    EXPORT_ int dginv(CMatrix<double> a, CMatrix<double> aa, int m, int n, double eps);//��SVD�ֽⷽ�������Ĺ�����
	EXPORT_ int bmaqr(CMatrix<double> a, int m, int n, CMatrix<double> q);//��householder�㷨��һ��������QR�ֽ�
	EXPORT_ int bmaqr1(CMatrix<double> a, int m, int n, CMatrix<double> q);//��householder�㷨��һ��������QR�ֽ�
    EXPORT_ int bmaqr(CMatrix<double> a, int m, int n, CMatrix<double> q, CVector<int> e);//��ѡ���е�householder�㷨��һ��������QR�ֽ�
    EXPORT_ int bmaqr1(CMatrix<double> a, int m, int n, CMatrix<double> q, CVector<int> e);//��ѡ���е�householder�㷨��һ��������QR�ֽ�

	EXPORT_ int Matinv(CDoubleVector a, int n);//Ҷ���ܷ�������
//	EXPORT_ void orithes(int n, double tol, double **a, double *d);//�������任��һ��ʵ���󻯼�Ϊ��hessenberger��
	EXPORT_ void orithes(int n, double tol, CMatrix<double> a, CVector<double> d);
//	EXPORT_ void ortrans(int n, double **h, double *d, double **v);//��orithes�Ľ������ȡ�����任��
	EXPORT_ void ortrans(int n, CMatrix<double> h, CVector<double> d, CMatrix<double> v);
		
	EXPORT_ void sss(CDoubleVector fg, CDoubleVector cs);
    EXPORT_ void cdiv(double xr, double xi, double yr, double yi, double& zr, double& zi);
//    EXPORT_ int hqr2(int n, double ep, double **h, double **v, double *wr, double *wi, int *cnt);//��ԭ���ƶ���QR�㷨��LQ�ֽ�����hessenberger�ξ��������ֵ����������
	EXPORT_ int hqr2( int n, double ep, CMatrix<double> h, CMatrix<double> v, CVector<double> wr, CVector<double> wi, CVector<int> cnt);
		
    EXPORT_ int cjcbi(CSymDbMatrix a, int n, CDoubleMatrix v, double eps);//��jacobi�㷨��ʵ�Գ��������ֵ����������
	EXPORT_ void   cjcbj(CDoubleVector& a,int n,CDoubleVector& v,double eps);//���Գƾ��������ֵ����������

    EXPORT_ int sphouseqr(CSparseDbMatrix &a, CSparseDbMatrix &q);//��householder�㷨��ϡ��������QR�ֽ�
    EXPORT_ int sphouseqr(CSparseDbMatrix &a, CSparseDbMatrix &q, CIntVector &e);//��ѡ���е�householder�㷨��ϡ��������QR�ֽ�
    EXPORT_ int spgassinv(CSparseDbMatrix &mat, double &det);	//ȫ��Ԫ��˹��ȥ����ϡ�跽���������ʽ
	EXPORT_ void sporithes(double tol, CSparseDbMatrix &a, CSparseDbMatrix &d);//�������任��һ��ʵϡ����󻯼�Ϊ��hessenberger��
    EXPORT_ void sportrans(CSparseDbMatrix &h, CSparseDbMatrix &d, CSparseDbMatrix &v);//��sporithes�Ľ������ȡ�����任��
    EXPORT_ int sphqr2(double ep, CSparseDbMatrix &h, CSparseDbMatrix &v, CVector<double> wr, CVector<double> wi, CVector<int> cnt);//��ԭ���ƶ���QR�㷨��LQ�ֽ�����hessenberger��ϡ����������ֵ����������
    EXPORT_ int spdet(CSparseDbMatrix m, double &det);//ȫ��Ԫ��˹��ȥ����ϡ�跽������ʽ

	EXPORT_ void grp2idx(CIntVector v1, CIntVector& idxVector, CIntVector& v2);//ytli Add @2002/09/23
	EXPORT_ void grp2idx(CDoubleVector v1, CIntVector& idxVector, CDoubleVector& v2);//ytli Add @2003/01/22
	EXPORT_ bool IsLargeFactor(CIntMatrix mat);//ytli Add @2003/01/26
	EXPORT_ bool IsLargeFactor(CDoubleMatrix mat);//ytli Add @2003/01/26
	EXPORT_ int bchol(CSymDbMatrix &a, CTriangleDbMatrix &b, double &det); //cholesky�ֽ⣬�ҿ�������ʽ
	EXPORT_ int cholesky(CSymDbMatrix v,CTriangleDbMatrix &output);//cholesky�ֽ� zfm�汾

	EXPORT_ double chi2pdf(double x, int v);//�����ֲ��ĸ����ܶȺ���
	EXPORT_ double chi2pdf1(double x, int v);//�����ֲ��ĸ����ܶȺ���
	EXPORT_ double fpdf(double x, int v1, int v2);//F�ֲ��ĸ����ܶȺ���
	EXPORT_ double fpdf1(double x, int v1, int v2);//F�ֲ��ĸ����ܶȺ���
	EXPORT_ double normpdf(double x, double mu, double sigma);//��̬�ֲ��ĸ����ܶȺ���
	EXPORT_ double tpdf(double x, int v);//t�ֲ��ĸ����ܶȺ���
	EXPORT_ double tpdf1(double x, int v);//t�ֲ��ĸ����ܶȺ���

	EXPORT_ double betapdf(double x, double a, double b);//Beta�ֲ��ĸ����ܶȺ���
	EXPORT_ double gampdf(double x, double a, double b);//Gamma�ֲ��ĸ����ܶȺ���
	EXPORT_ double exppdf(double x, double mu);//ָ���ֲ��ĸ����ܶȺ���
	EXPORT_ double geopdf(double x, double p);//���ηֲ��ĵ���ʺ���
	EXPORT_ double hygepdf(int x, int m, int k, int n);//�����ηֲ��ĵ���ʺ���
	EXPORT_ double poisspdf(int x, double lamda);//���ɷֲ��ĵ���ʺ���
	EXPORT_ double lognpdf(double x, double mu, double sigma);//������̬�ֲ��ĸ����ܶȺ���
	EXPORT_ double weibpdf(double x, double a, double b);//�������ֲ��ĸ����ܶȺ���

	EXPORT_ double halfnpdf(double x, double theta);//����̬�ֲ��ĸ����ܶȺ���
	EXPORT_ double laplacepdf(double x, double b, double mu);//Laplace�ֲ��ĸ����ܶȺ���
	EXPORT_ double logistpdf(double x, double m, double b);//Logistic�ֲ��ĸ����ܶȺ���
	EXPORT_ double paretopdf(double x, double a, double b);//Pareto�ֲ��ĸ����ܶȺ���
	EXPORT_ double uniformpdf(double x, double a, double b);//���ȷֲ��ĸ����ܶȺ���

	EXPORT_ double chi2cdf(double x, int v);//�����ֲ��ĸ��ʷֲ�����
	EXPORT_ double chi2cdf1(double x, int v);//�����ֲ��ĸ��ʷֲ�����
	EXPORT_ double fcdf(double x, int v1, int v2);//F�ֲ��ĸ��ʷֲ�����
	EXPORT_ double fcdf1(double x, int v1, int v2);//F�ֲ��ĸ��ʷֲ�����
	EXPORT_ double normcdf(double x, double mu, double sigma);//��̬�ֲ��ĸ��ʷֲ�����
	EXPORT_ double CalkCommonFx(CDoubleVector DataArray,CDoubleVector WeightArray,double x,double nSize);

	EXPORT_ double tcdf(double x, int v);//t�ֲ��ĸ��ʷֲ�����
	EXPORT_ double tcdf1(double x, int v);//t�ֲ��ĸ��ʷֲ�����
	EXPORT_ double tcdf(double x, double v); //zfm

	EXPORT_ double betacdf(double x, double a, double b);//Beta�ֲ��ĸ��ʷֲ�����
	EXPORT_ double gamcdf(double x, double a, double b);//Gamma�ֲ��ĸ��ʷֲ�����
	EXPORT_ double expcdf(double x, double mu);//ָ���ֲ��ĸ��ʷֲ�����
	EXPORT_ double geocdf(double x, double p);//���ηֲ��ĸ��ʷֲ�����
	EXPORT_ double hygecdf(int x, int m, int k, int n);//�����ηֲ��ĸ��ʷֲ�����
	EXPORT_ double poisscdf(int x, double lamda);//���ɷֲ��ĸ��ʷֲ�����
	EXPORT_ double logncdf(double x, double mu, double sigma);//������̬�ֲ��ĸ��ʷֲ�����
	EXPORT_ double weibcdf(double x, double a, double b);//�������ֲ��ĸ��ʷֲ�����

	EXPORT_ double halfncdf(double x, double theta);//����̬�ֲ��ĸ��ʷֲ�����
	EXPORT_ double laplacecdf(double x, double b, double mu);//Laplace�ֲ��ĸ��ʷֲ�����
	EXPORT_ double logistcdf(double x, double m, double b);//Logistic�ֲ��ĸ��ʷֲ�����
	EXPORT_ double paretocdf(double x, double a, double b);//Pareto�ֲ��ĸ��ʷֲ�����
	EXPORT_ double uniformcdf(double x, double a, double b);//���ȷֲ��ĸ��ʷֲ�����

	EXPORT_ double chi2inv(double p, int v);//�����ֲ��ĸ��ʷֲ���������
	EXPORT_ double finv(double p, int v1, int v2);//F�ֲ��ĸ��ʷֲ���������
	EXPORT_ double norminv(double p, double mu, double sigma);//��̬�ֲ��ĸ��ʷֲ���������
	EXPORT_ double tinv(double p, int v);//t�ֲ��ĸ��ʷֲ���������
	EXPORT_ double tinv(double p, double v); //t�ֲ��ĸ��ʷֲ���������,���ɶ�Ϊdouble zfm

	EXPORT_ double betainv(double p, double a, double b);//Beta�ֲ��ĸ��ʷֲ���������
	EXPORT_ double gaminv(double p, double a, double b);//Gamma�ֲ��ĸ��ʷֲ���������
	EXPORT_ double expinv(double p, double mu);//ָ���ֲ��ĸ��ʷֲ���������
	EXPORT_ double geoinv(double y, double p);//���ηֲ��ĸ��ʷֲ���������
	EXPORT_ double hygeinv(double p, int m, int k, int n);//�����ηֲ��ĸ��ʷֲ���������
	EXPORT_ double poissinv(double p, double lamda);//���ɷֲ��ĸ��ʷֲ���������
	EXPORT_ double logninv(double p, double mu, double sigma);//������̬�ֲ��ĸ��ʷֲ���������
	EXPORT_ double weibinv(double p, double a, double b);//�������ֲ��ĸ��ʷֲ���������

	EXPORT_ double halfninv(double p, double theta);//����̬�ֲ��ĸ��ʷֲ���������
	EXPORT_ double laplaceinv(double p, double b, double mu);//Laplace�ֲ��ĸ��ʷֲ���������
	EXPORT_ double logistinv(double p, double m, double b);//Logistic�ֲ��ĸ��ʷֲ���������
	EXPORT_ double paretoinv(double p, double a, double b);//Pareto�ֲ��ĸ��ʷֲ���������
	EXPORT_ double uniforminv(double p, double a, double b);//���ȷֲ��ĸ��ʷֲ���������

	EXPORT_ double stdrcdf(double q, int v, int r, int jmax=15, int kmax=15);//studentized-range �ֲ�����
	EXPORT_ double stdrinv(double p, int v, int r);//studentized-range �ֲ���������
    EXPORT_ double qtrng0(double p, int v, int r);
    EXPORT_ double dwhcdf(int n, int k, double r);//durbin-waston�������޷ֲ�����
    EXPORT_ double dwlcdf(int n, int k, double r);//durbin-waston�������޷ֲ�����
    EXPORT_ double dwhpdf(int n, int k, double r);//durbin-waston�������޸����ܶȺ���
    EXPORT_ double dwlpdf(int n, int k, double r);//durbin-waston�������޸����ܶȺ���
    EXPORT_ double dwhinv(int n, int k, double p);//durbin-waston�������޷ֲ���������
    EXPORT_ double dwlinv(int n, int k, double p);//durbin-waston�������޷ֲ���������
    EXPORT_ double pfun(int n, int k, double r, double s, CDoubleVector lamta);
    EXPORT_ double pdfun(int n, int k, double r, double s, CDoubleVector lamta);
    EXPORT_ double sfun(double s, int n, int k, CDoubleVector lamta);
    EXPORT_ double bfun(int n, int k, double *lamta);
    EXPORT_ void lamtax(CDoubleVector lamta, int n, int k, double r, int flag);
    EXPORT_ double invfun(int n, int k,double p, int flag);
    EXPORT_ double d2cdf(double v);
	EXPORT_ int    jiechen(int param);

	EXPORT_ CDoubleVector cubicfit(int n, CDoubleVector x,CDoubleVector y,CDoubleVector &cubic, CDoubleVector &square, CDoubleVector &one, bool &bYes);//��������������ֵ cubic,square,one�ֱ�Ϊ����,����,һ��ϵ��,boolĬ�Ͽ��������β�ֵ 2003-3-20 zfm 
	EXPORT_ CDoubleVector GetNormMedians(int n,double mu =0 , double sigma= 1 /*std */);
	
    EXPORT_ void WriteData(CDoubleMatrix &m, char * pszTitle); 
	EXPORT_ void LoadData(CDoubleMatrix &m, char * pszTitle); 
	
    EXPORT_ void WriteData(CSparseDbMatrix &m, char * pszTitle); 
    EXPORT_ void WriteData(CComplexMatrix &m, char * pszTitle); 
    EXPORT_ void WriteData(CComplexVector &m, char * pszTitle);
    EXPORT_ void WriteData(CDoubleVector &m, char * pszTitle);
    EXPORT_ void WriteData(CIntVector &m, char * pszTitle);
    EXPORT_ void WriteData(CIntMatrix &m, char * pszTitle);
    EXPORT_ void WriteTrData(CSparseDbMatrix &m, char * pszTitle);
    EXPORT_ void WriteData(CSparseCompMatrix &m, char * pszTitle);
    EXPORT_ void WriteTrData(CSparseCompMatrix &m, char * pszTitle);

	EXPORT_ void WriteData(double var,char * title); //zfm
	EXPORT_ void WriteData(int    var,char * title); //zfm
	EXPORT_ void WriteData(CDoubleVector var,int n,char * title=NULL); //zfm
	EXPORT_ void WriteData(CIntVector var,int n,char * title=NULL); //zfm
	EXPORT_ void WriteData(CDoubleMatrix var,int m,int n,char * title=NULL); //zfm
	
//�������������������������������� old  ��������������������������������
#ifdef OLD_VERSION
public:	

	EXPORT_ double chi2(double ch, int n, double *d);//�����ֲ������͸����ܶ�

	EXPORT_ double tdist(double t, int n, double *dd);//t�ֲ������͸����ܶ�
	EXPORT_ double fdist(int n1, int n2, double f, double *d);//F�ֲ������͸����ܶ�

	EXPORT_ void pearson(double *x, int k, int n, double *r);//pearson���ϵ��

	EXPORT_ double partial(double *r, int *pos, int count, int n);//ƫ���ϵ�����

    EXPORT_ void ppp(CMatrix<double> a, double *e, double *s,double** v, int m, int n);
	EXPORT_ int dluav(CMatrix<double> a, double** u,double** v, int m, int n, double eps);//��householder�任�ͱ���QR�㷨��һ��������SVD�ֽ�
    EXPORT_ int bmaqr(CMatrix<double> a, int m, int n, CMatrix<double> q, int *e);//��ѡ���е�householder�㷨��һ��������QR�ֽ�
    EXPORT_ int bmaqr1(CMatrix<double> a, int m, int n, CMatrix<double> q, int *e);//��ѡ���е�householder�㷨��һ��������QR�ֽ�

	EXPORT_ int Matinv(double *a, int n);//Ҷ���ܷ�������
	EXPORT_ void orithes(int n, double tol, double **a, double *d);//�������任��һ��ʵ���󻯼�Ϊ��hessenberger��
    EXPORT_ void ortrans(int n, double **h, double *d, double **v);//��orithes�Ľ������ȡ�����任��
	EXPORT_ void sss(double *fg, double *cs);
    EXPORT_ void cdiv(double xr, double xi, double yr, double yi, double *zr, double *zi);
    EXPORT_ int hqr2(int n, double ep, double **h, double **v, double *wr, double *wi, int *cnt);//��ԭ���ƶ���QR�㷨��LQ�ֽ�����hessenberger�ξ��������ֵ����������
    EXPORT_ int sphqr2(double ep, CSparseDbMatrix &h, CSparseDbMatrix &v, double *wr, double *wi, int *cnt);//��ԭ���ƶ���QR�㷨��LQ�ֽ�����hessenberger��ϡ����������ֵ����������
	EXPORT_ int bchol(CSymDbMatrix &a, CTriangleDbMatrix &b, double *det); //cholesky�ֽ⣬�ҿ�������ʽ
    EXPORT_ double pfun(int n, int k, double r, double s, double *lamta);
    EXPORT_ double pdfun(int n, int k, double r, double s, double *lamta);
    EXPORT_ double sfun(double s, int n, int k, double *lamta);
    EXPORT_ void lamtax(double *lamta, int n, int k, double r, int flag);

	EXPORT_ void PrintVector(CTString str,CDoubleVector Vect,int nDecimal=4);
	EXPORT_ void PrintVector(CTString str,CIntVector Vect);
	EXPORT_ void PrintMatrix(CTString str,CDoubleMatrix Matrix,int nDecimal=4);
	
	EXPORT_ double* cubicfit(int n, double *x,double *y, double* &cubic, double* &square, double* &one, bool &bYes);//��������������ֵ cubic,square,one�ֱ�Ϊ����,����,һ��ϵ��,boolĬ�Ͽ��������β�ֵ 2003-3-20 zfm 


#endif
};
#endif