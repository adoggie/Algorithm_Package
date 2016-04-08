// Matrix.cpp: implementation of the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Matrix.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

extern  "C"  int dgeqp3_(long int *m, long int *n, double *a, long int *
	lda, long int *jpvt, double *tau, double *work, long int *lwork,
	 long int *info);

extern "C" int dorgqr_(long int *m, long int *n, long int *k, double *
	a, long int *lda, double *tau, double *work, long int *lwork, 
	long int *info);

extern "C" int dgesvd_(char *jobu, char *jobvt, long int *m, long int *n, 
	double *a, long int *lda, double *s, double *u, long int *ldu, 
	double *vt, long int *ldvt, double *work, long int *lwork, long int *info);

extern "C"  int dgeqrf_(long int *m, long int *n, double *a, long int *
	lda, double *tau, double *work, long int *lwork, long int *info);
//{{{START 2003-1-20 Eric

bool bUsingFileMatrix;

bool g_MatrixInit = false;

void MatrixInit()
{

	if(!g_MatrixInit)
	{//暂无代码
		g_MatrixInit = true;
	}
}

//}}}END  2003-1-20 Eric


//////////////////////////////////////////////////////////////////////
// CIntMatrix -----Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIntMatrix & CIntMatrix::operator = (const CIntMatrix & vec)
{
	this->CMatrix<int>::operator = (vec);
	return * this;
}

CIntMatrix & CIntMatrix::operator = (const CMatrix<int> & vec)
{
	this->CMatrix<int>::operator = (vec);
	return * this;
}

CIntMatrix & CIntMatrix::operator = (const CVector<int> & vec)
{
	this->CMatrix<int>::operator = (vec);
	return * this;
}

CIntMatrix & CIntMatrix::operator = (const CIntVector & vec)
{
	this->CMatrix<int>::operator = (vec);
	return * this;
}

CIntMatrix & CIntMatrix::operator =(int **x)
{
    this->CMatrix<int>::operator =(x);
	return * this;
}

CIntMatrix & CIntMatrix::operator =(int *x)
{
    this->CMatrix<int>::operator =(x);
	return * this;
}

CIntMatrix & CIntMatrix::operator =(int x)
{
    this->CMatrix<int>::operator =(x);
	return * this;
}





//////////////////////////////////////////////////////////////////////
// CDoubleMatrix --------- Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDoubleMatrix & CDoubleMatrix::operator = (const CDoubleMatrix & vec)
{
	this->CMatrix<double>::operator = (vec);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator = (const CMatrix<double> & vec)
{
	this->CMatrix<double>::operator = (vec);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator = (const CVector<double> & vec)
{
	this->CMatrix<double>::operator = (vec);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator = (const CDoubleVector & vec)
{
	this->CMatrix<double>::operator = (vec);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator = (const CSymDbMatrix & vec)
{
	*this = smattodmat(vec);
	return * this;
}



CDoubleMatrix::CDoubleMatrix(const CSymDbMatrix &x):CMatrix<double>((CSymMatrix<double>) x)
{
}

CDoubleMatrix::CDoubleMatrix(const CTriangleDbMatrix &x):CMatrix<double>((CTriangleMatrix<double>) x)
{
}

CDoubleMatrix::CDoubleMatrix(const CSparseDbMatrix &x):CMatrix<double>((CSparseMatrix<double> )x)
{   
}

CDoubleMatrix::CDoubleMatrix(const CComplexMatrix &x):CMatrix<double>(x.mrow(),x.mcol())
{
    (*this) = real(x);
}

CDoubleMatrix & CDoubleMatrix::operator = (const CComplexMatrix & vec)
{
	* this = real(vec);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator = (const CIntMatrix & vec)
{
	* this = imattodmat(vec);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator = (const CFloatMatrix & vec)
{
	* this = fmattodmat(vec);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator =(double **x)
{
    this->CMatrix<double>::operator =(x);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator =(double *x)
{
    this->CMatrix<double>::operator =(x);
	return * this;
}

CDoubleMatrix & CDoubleMatrix::operator =(double x)
{
    this->CMatrix<double>::operator =(x);
	return * this;
}

CDoubleMatrix imattodmat(const CIntMatrix & vec)
{
	int len = vec.mrow();
	int ven = vec.mcol();
	CDoubleMatrix v(len,ven);
	for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j)=vec.GetValue(i,j);
	return v;
}

CFloatMatrix imattofmat(const CIntMatrix & vec)
{
	int len = vec.mrow();
	int ven = vec.mcol();
	CFloatMatrix v(len,ven);
	for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j)=(float)vec.GetValue(i,j);
	return v;
}

CDoubleMatrix smattodmat(const CSymDbMatrix &vec)
{
    int len = vec.mrow();
	int ven = vec.vlen();
	CDoubleMatrix m(len,len);
	int k=0;
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{				
                m(j,j+i) = vec.GetValue(k);
				k++;
			}
	}
    for(int i=1; i<len; i++)
		for(int j=0; j<i; j++)
			m(i,j) = m(j,i);
	return m;
}

CDoubleMatrix fmattodmat(const CFloatMatrix & vec)
{
	int len = vec.mrow();
	int ven = vec.mcol();
	CDoubleMatrix v(len,ven);
	for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j)=vec.GetValue(i,j);
	return v;
}
CDoubleMatrix& CDoubleMatrix::delt(int nCol)
{
	if (nCol < mcol())
	{
		CDoubleMatrix matrix(mrow(),mcol()-1);
		int i=0;
		for(i=0; i<nCol; i++)
		{	
			matrix(i) = (*this)(i);
		}
		for(i=nCol+1; i<mcol(); i++)
		{	
			matrix(i-1) = (*this)(i);
		}
		(*this) = matrix;
	}
	return *this;
}
/*
CDoubleMatrix  operator + (const CDoubleMatrix &m, int x)
{
	double y;
	CDoubleMatrix m1(m.mrow(),m.mcol());
	y = x;
	m1 = m + y;
	return m1;
}

CDoubleMatrix  operator - (const CDoubleMatrix &m, int x)
{
	double y;
	CDoubleMatrix m1(m.mrow(),m.mcol());
	y = x;
	m1 = m - y;
	return m1;
}

CDoubleMatrix  operator * (const CDoubleMatrix &m, int x)
{
	double y;
	CDoubleMatrix m1(m.mrow(),m.mcol());
	y = x;
	m1 = m * y;
	return m1;
}

CDoubleMatrix  operator / (const CDoubleMatrix &m, int x)
{
	double y;
	CDoubleMatrix m1(m.mrow(),m.mcol());
	y = x;
	m1 = m / y;
	return m1;
}

CDoubleMatrix  operator + (int x, const CDoubleMatrix &m)
{
	double y;
	CDoubleMatrix m1(m.mrow(),m.mcol());
	y = x;
	m1 = y + m;
	return m1;
}

CDoubleMatrix  operator - (int x, const CDoubleMatrix &m)
{
	double y;
	CDoubleMatrix m1(m.mrow(),m.mcol());
	y = x;
	m1 = y - m;
	return m1;
}

CDoubleMatrix  operator * (int x, const CDoubleMatrix &m)
{
	double y;
	CDoubleMatrix m1(m.mrow(),m.mcol());
	y = x;
	m1 = y * m;
	return m1;
}

CDoubleMatrix  operator / (int x, const CDoubleMatrix &m)
{
	double y;
	CDoubleMatrix m1(m.mrow(),m.mcol());
	y = x;
	m1 = y / m;
	return m1;
}
*/
CDoubleMatrix abs(const CDoubleMatrix &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CDoubleMatrix v(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
            v(i,j) = fabs(x.GetValue(i,j));
    return v;
}


CDoubleMatrix CDoubleMatrix::abs()const 
{
	int mrows = mrow();
	int mcols = mcol();
	CDoubleMatrix v(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
            v(i,j) = ::fabs((*this).GetValue(i,j));
    return v;
}

CDoubleMatrix CDoubleMatrix::dot(const CDoubleMatrix &m2)const 
{
	CDoubleMatrix mx(mcol(),m2.mcol());
	for (int i=0;i<mcol();i++)
		for (int j=0;j<m2.mcol();j++)
			mx(i,j) = (*this).GetValue(i).dot(m2.GetValue(j));
		return mx;
}

CDoubleMatrix dot(const CDoubleMatrix &m1,const CDoubleMatrix &m2)
{
	return m1.dot(m2);
}

CDoubleVector CDoubleMatrix::dot(const CDoubleVector &v)const 
{
	CDoubleVector mx(mcol());
	for (int i=0;i<mcol();i++)
		mx(i) = (*this).GetValue(i).dot(v);
	return mx;
}

CDoubleVector dot(const CDoubleMatrix &m,const CDoubleVector &v)
{
	return m.dot(v);
}

CDoubleVector dot(const CDoubleVector &v, const CDoubleMatrix &m)
{
	return m.dot(v);
}


CDoubleMatrix trans(const CDoubleMatrix &v1)
{
//	int len = v1.mrow();
//	int ven = v1.mcol();
//{{{START 2003-1-14 Eric
	/*
	CDoubleMatrix v(ven,len);
    for(int i=0; i<ven; i++)
		for(int j=0; j<len; j++)
			v(i,j) = v1(j,i);
		*/
//	CDoubleMatrix v;//(ven,len);
/*
	for(int i=0; i<ven; i++)
		for(int j=0; j<len; j++)
			v(i,j) = v1(j,i);
//}}}END  2003-1-14 Eric
*/
	return v1.trans();
}

CDoubleMatrix CDoubleMatrix::trans()const 
{
	int len = mrow();
	int ven = mcol();
	CDoubleMatrix v(ven,len);
//{{{START 2001-1-14 Eric
    for(int i=0; i<ven; i++)
		for(int j=0; j<len; j++)
			v(i,j) =(*this).GetValue(j,i);
//}}}END  2001-1-14 Eric

	return v;
}


CDoubleMatrix eye(int n)
{
	CDoubleMatrix x(n,n);
	for(int i=0; i<n; i++)
		x(i,i) = 1;
	return x;
}

CDoubleMatrix eye(int m, int n)
{
	CDoubleMatrix x(m,n);
	int mn = MIN(m,n);
	for(int i=0; i<mn; i++)
		x(i,i) = 1;
	return x;
}

CDoubleMatrix eye(CDoubleMatrix &v)
{
	int m = v.mrow();
	int n = v.mcol();
	CDoubleMatrix x(m,n);
	int mn = MIN(m,n);
	for(int i=0; i<mn; i++)
		x(i,i) = 1;
	return x;
}

CDoubleMatrix CDoubleMatrix::eye()const 
{
	int m = mrow();
	int n = mcol();
	CDoubleMatrix x(m, n);
	int mn = MIN(m,n);
	for(int i=0; i<mn; i++)
		x(i,i) = 1;
	return x;
}

CDoubleMatrix CDoubleMatrix::mkeye(int nIndex)const 
{
	CDoubleMatrix ret_Mat;
	if (mcol()==mrow() && mcol()>=2 && nIndex >0 && nIndex < mcol())
	{
		int i,j,k;
		ret_Mat.create(nIndex,mcol(),0.0);
		for (i=0;i<nIndex;i++)
			for (j=0;j<mcol();j++)
				ret_Mat(i,j) = (*this).GetValue(i,j);
			for (i=0;i<nIndex;i++)
			{
				double dVV = ret_Mat(i,i);
				if (fabs(dVV) < DOUBLE_ZERO)
					return ret_Mat;
				for (j=0;j<mcol();j++)
					ret_Mat(i,j) /= dVV;
				for (j=i+1;j<nIndex;j++)
				{
					double dV = -1.0*ret_Mat(j,i);
					for (k=0;k<mcol();k++)
						ret_Mat(j,k) += dV*ret_Mat(i,k);
				}
			}
			
			for (i=nIndex-1;i>=0;i--)
			{
				for (j=i-1;j>=0;j--)
				{
					double dV = -1.0*ret_Mat(j,i);
					for (k=0;k<mcol();k++)
						ret_Mat(j,k) += dV*ret_Mat(i,k);
				}
			}
			
	}
	
	return ret_Mat;
}

CDoubleMatrix mkeye(const CDoubleMatrix &m,int nIndex)
{
	return m.mkeye(nIndex);
}

CIntVector any(const CDoubleMatrix &v)
{
	int m=v.mrow();
	int n=v.mcol();
	CIntVector x(m);
	for(int i=0; i<m; i++)
	{
		x(i)=0;
		for(int j=0; j<n; j++)
		{
			x(i) =0;
			for(int j=0; j<n; j++)
			{
				if(v.GetValue(i,j) != 0)
				{
					x(i) = 1;
					break;
				}
			}
		}
	}
	return x;
}

CIntVector CDoubleMatrix::any()const 
{
	int m=mrow();
	int n=mcol();
	CIntVector x(m);
	for(int i=0; i<m; i++)
	{
		x(i)=0;
		for(int j=0; j<n; j++)
		{
			x(i) =0;
			for(int j=0; j<n; j++)
			{
				if((*this).GetValue(i,j) != 0)
				{
					x(i) = 1;
					break;
				}
			}
		}
	}
	return x;
}

CDoubleVector Max(const CDoubleMatrix &v)
{
	int nr=v.mrow();
	int nc=v.mcol();
	CDoubleVector xx(nr);
	for(int i=0; i<nr; i++)
	{
		xx(i) = v.GetValue(i,0);
		for(int j=1; j<nc; j++)
			if(v.GetValue(i,j)>xx(i))
				xx(i) = v.GetValue(i,j);
	}
	return xx;
}

CDoubleVector CDoubleMatrix::Max()const 
{
	int nr=mrow();
	int nc=mcol();
	CDoubleVector xx(nr);
	for(int i=0; i<nr; i++)
	{
		xx(i) = (*this).GetValue(i,0);
		for(int j=1; j<nc; j++)
			if((*this).GetValue(i,j)>xx(i))
				xx(i) = (*this).GetValue(i,j);
	}
	return xx;
}

CDoubleMatrix Max(const CDoubleMatrix &v1, const CDoubleMatrix &v2)
{
	int nr = v1.mrow();
	int nc = v1.mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
    {
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
		nr = MIN(v1.mrow(), v2.mrow());
		nc = MIN(v1.mcol(), v2.mcol());
	}
	CDoubleMatrix xx(nr,nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i,j) = v1.GetValue(i,j);
			if(v2.GetValue(i,j)>v1.GetValue(i,j))
				xx(i,j)=v2.GetValue(i,j);
		}
	return xx;
}

CDoubleMatrix CDoubleMatrix::Max( const CDoubleMatrix &v2)
{
	int nr = mrow();
	int nc = mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
    {
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
		nr = MIN(mrow(), v2.mrow());
		nc = MIN(mcol(), v2.mcol());
	}
	CDoubleMatrix xx(nr,nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i,j) = (*this)(i,j);
			if(v2.GetValue(i,j)>(*this)(i,j))
				xx(i,j)=v2.GetValue(i,j);
		}
	return xx;
}


CDoubleVector Min(const CDoubleMatrix &v)
{
	int nr=v.mrow();
	int nc=v.mcol();
	CDoubleVector xx(nr);
	for(int i=0; i<nr; i++)
	{
		xx(i) = v.GetValue(i,0);
		for(int j=1; j<nc; j++)
			if(v.GetValue(i,j)<xx(i))
				xx(i) = v.GetValue(i,j);
	}
	return xx;
}

CDoubleVector CDoubleMatrix::Min()const 
{
	int nr=mrow();
	int nc=mcol();
	CDoubleVector xx(nr);
	for(int i=0; i<nr; i++)
	{
		xx(i) = (*this).GetValue(i,0);
		for(int j=1; j<nc; j++)
			if((*this).GetValue(i,j)<xx(i))
				xx(i) = (*this).GetValue(i,j);
	}
	return xx;
}

CDoubleMatrix Min(const CDoubleMatrix &v1, const CDoubleMatrix &v2)
{
	int nr = v1.mrow();
	int nc = v1.mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
    {
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
		nr = MIN(v1.mrow(), v2.mrow());
		nc = MIN(v1.mcol(), v2.mcol());
	}
	CDoubleMatrix xx(nr,nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i)(j) = v1.GetValue(i,j);
			if(v2.GetValue(i,j)<v1.GetValue(i,j))
				xx(i,j)=v2.GetValue(i,j);
		}
	return xx;
}

CDoubleMatrix CDoubleMatrix::Min( const CDoubleMatrix &v2)const 
{
	int nr = mrow();
	int nc = mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
    {
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
		nr = MIN(mrow(), v2.mrow());
		nc = MIN(mcol(), v2.mcol());
	}
	CDoubleMatrix xx(nr,nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i,j) = (*this).GetValue(i,j);
			if(v2.GetValue(i,j)<(*this).GetValue(i,j))
				xx(i,j)=v2.GetValue(i,j);
		}
	return xx;
}

CDoubleMatrix & Rand(CDoubleMatrix &v)
{
	int nr=v.mrow();
	int nc=v.mcol();
	for(int i=0; i<nr; i++)
	{
		for(int j=0; j<nr; j++)
			v(i,j)=(double)::rand()/(double)RAND_MAX;
	}
	return v;
}

CDoubleMatrix & CDoubleMatrix::Rand()
{
	int nr=mrow();
	int nc=mcol();
	for(int i=0; i<nr; i++)
	{
		for(int j=0; j<nr; j++)
			(*this)(i,j)=(double)::rand()/(double)RAND_MAX;
	}
	return (*this);
}

CDoubleMatrix CDoubleMatrix::Rand(int m, int n)const
{
    CDoubleMatrix v(m,n);
	for(int i=0; i<m; i++)
	{
		for(int j=0; j<n; j++)
			v(i,j)=(double)::rand()/(double)RAND_MAX;
	}
	return v;
}

double norm(const CDoubleMatrix &v, int n)
{
	int nr=v.mrow();
	int nc=v.mcol();
	double res;
	CDoubleMatrix yy(nc,nr);
	CDoubleVector zz(nr), zp(nc);
	switch(n)
	{
	case 0:
		zz = sum(abs(v));
		res = Max(zz);
		break;
	case 1:
		zp = sum(abs(trans(v)));
		res = Max(zp);
		break;
	case 2:
//		zz= svd(v);  //yy old
		zz= svd_rank(v); //zfm 2003-02-21
		res = zz(0);
		break;
	default:
		if(n<0)
		{
			yy = (trans(v))*(v);
			res = sqrt(sum(diag(yy)));
		}
		else
			ErrorHandler(EMAT_NODEFINE, __FILE__, __LINE__);
	}
	return res;
}

double CDoubleMatrix::norm( int n)const 
{
	int nr=mrow();
	int nc=mcol();
	double res;
	CDoubleMatrix yy(nc,nr);
	CDoubleVector zz(nr), zp(nc);
	switch(n)
	{
	case 0:
		zz = ::sum(abs());
		res = ::Max(zz);
		break;
	case 1:
		zp = ::sum(::abs(trans()));
		res = ::Max(zp);
		break;
	case 2:
		//zz= svd(); //yy old
		zz=svd_rank(*this);//zfm 2003-02-21
		res = zz(0);
		break;
	default:
		if(n<0)
		{
			yy = (trans())*(*this);
			res = ::sqrt(::sum(::diag(yy)));
		}
		else
			ErrorHandler(EMAT_NODEFINE, __FILE__, __LINE__);
	}
	return res;
}

double cond(const CDoubleMatrix &x)
{
	int mr = x.mrow();
	int mc = x.mcol();
	int mrc = MIN(mr,mc);
	if(mrc ==0)
	{
		ErrorHandler(EMAT_NOTNUMBER, __FILE__, __LINE__);
        return EMAT_NOTNUMBER;
	}
	CDoubleVector s(mrc);
	//s = svd(x); //yy old
	s=svd_rank(x);//zfm 2003-02-21
	for(int i=0; i<mrc; i++)
	{
		if(s(i)==0)
		{
	    	ErrorHandler(EMAT_INFINITECOND, __FILE__, __LINE__);
            return EMAT_INFINITECOND;
		}
	}
	double y=s(0)/s(mrc-1);
	return y;
}

double CDoubleMatrix::cond()const 
{
	int mr = mrow();
	int mc = mcol();
	int mrc = MIN(mr,mc);
	if(mrc ==0)
	{
		ErrorHandler(EMAT_NOTNUMBER, __FILE__, __LINE__);
        return EMAT_NOTNUMBER;
	}
	CDoubleVector s(mrc);
	//s = svd(); //yy old
	s=svd_rank(*this); //zfm 2003-02-21
	for(int i=0; i<mrc; i++)
	{
		if(s(i)==0)
		{
	    	ErrorHandler(EMAT_INFINITECOND, __FILE__, __LINE__);
            return EMAT_INFINITECOND;
		}
	}
	double y=s(0)/s(mrc-1);
	return y;
}

int rank(const CDoubleMatrix &x, double tol)
{
    int i,r;
	int mr = x.mrow();
	int mc = x.mcol();
	int mmin = MIN(mr,mc);
	CDoubleVector s(mmin);
//	s=svd(x);
	s=svd_rank(x);
	if(tol==0)
	{
		int mmax = MAX(mr,mc);
		tol = mmax*s(0)*DBL_EPSILON;
	}
	r=0;
	for(i=0; i<mmin; i++)
		if(s(i)>tol)
			r += 1;
	return r;
}

int CDoubleMatrix::rank(double tol) const  //zfm
{
    int i,r;
	int mr = mrow();
	int mc = mcol();
	int mmin = MIN(mr,mc);
	CDoubleVector s(mmin);
	//s = svd();
	s = svd_rank(*this);
	if(tol==0)
	{
		int mmax = MAX(mr,mc);
		tol = mmax*s(0)*DBL_EPSILON;
	}
	r=0;
	for(i=0; i<mmin; i++)
		if(s(i)>tol)
			r += 1;
	return r;
}

/*
int CDoubleMatrix::rank(double tol)
{
    int i,r;
	int mr = mrow();
	int mc = mcol();
	int mmin = MIN(mr,mc);
	CDoubleVector s(mmin);
	s = svd();
	if(tol==0)
	{
		int mmax = MAX(mr,mc);
		tol = mmax*s(0)*DBL_EPSILON;
	}
	r=0;
	for(i=0; i<mmin; i++)
		if(s(i)>tol)
			r += 1;
	return r;
}
*/

double det(const CDoubleMatrix &mm)
{
    int numcol = mm.mrow();
	if(mm.mcol() != numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		numcol = MIN(mm.mrow(), mm.mcol());
	}
	CDoubleMatrix work(numcol, numcol);
	int nn;
	double det;
	work = copy(mm);
	nn = Matdet(work, numcol, det);
	if(nn != 0)
	{
		ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
	}
	return det;
}

double CDoubleMatrix::det()const 
{
    int numcol = mrow();
	if(mcol() != numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		numcol = MIN(mrow(), mcol());
	}
	CDoubleMatrix work(numcol, numcol);
	int nn;
	double det;
	work = copy();
	nn = Matdet(work, numcol, det);
	if(nn != 0)
	{
		ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
	}
	return det;
}
/*
CDoubleMatrix CDoubleMatrix::inv()  
{
	int numcol = mrow();

	if(mcol()!=numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        numcol = MIN(mrow(),mcol());
	}
	CDoubleMatrix work(numcol, numcol);
	double de;
	int nn;
	long chkmax;
	chkmax = (long)numcol*numcol;
	if(chkmax*sizeof(double)>64000l)
	{
		work = copy();
		nn = Matinv(work,numcol);
	}
	else
	{
		double *inv = new double[numcol*numcol];
		inv <<(*this);
		nn = Matinv(inv,numcol);
		work = inv;
        delete inv;
	}
    if((nn==EMAT_SINGULAR)||(nn==WEMAT_SINGULAR))
	{
		work = copy();
		nn = Matinv1(work,numcol,de);
	}
	if(nn!=0)
	{
		if(nn>0)
			ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
		else
			WarningHandler((Matvec_warnings)nn, __FILE__, __LINE__);
	}
	return work;
}
*/

CDoubleMatrix CDoubleMatrix::inv() const  //zfm 
{
	int numcol = mrow();

	assert(mcol()==numcol);
	
	CDoubleMatrix work(numcol, numcol);
	double de;
	int nn;
	long chkmax;
	chkmax = (long)numcol*numcol;
	if(chkmax*sizeof(double)>64000l)
	{
		work = copy();
		nn = Matinv(work,numcol);
	}
	else
	{
		CDoubleVector inv(numcol*numcol);
		for (int i=0;i<numcol;i++)
			inv(i*numcol) = (*this).GetValue(numcol-1,i);
		nn = Matinv(inv,numcol);
		work = inv;
	}
    if((nn==EMAT_SINGULAR)||(nn==WEMAT_SINGULAR))
	{
		work = copy();
		nn = Matinv1(work,numcol,de);
	}
	return work;
}

CDoubleMatrix inv(const CDoubleMatrix &mm)
{
	return mm.inv();
}
/*
CDoubleMatrix inv1(CDoubleMatrix &mm)
{
	int numcol = mm.mrow();
	if(mm.mcol()!=numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        numcol = MIN(mm.mrow(),mm.mcol());
	}
	CDoubleMatrix work(numcol,numcol);
	double det;
	int nn;
	work = copy(mm);
	nn = Matinv1(work, numcol, det);
	if(nn!=0)
	{
		if(nn>0)
			ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
		else
			WarningHandler((Matvec_warnings)nn, __FILE__, __LINE__);
	}
	return work;
}
*/
CDoubleMatrix inv1(const CDoubleMatrix &mm)  //zfm
{
	int numcol = mm.mrow();
	assert(mm.mcol()==numcol);
	CDoubleMatrix work(numcol,numcol);
	double det;
	int nn;
	work = copy(mm);
	nn = Matinv1(work, numcol, det);
	assert(nn==0);
	return work;
}


CDoubleMatrix CDoubleMatrix::inv1()const 
{
	return ::inv1(*this);
}

/*
CDoubleMatrix inv(CDoubleMatrix &mm, double &det) 
{
	int numcol = mm.mrow();
	if(mm.mcol()!=numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        numcol = MIN(mm.mrow(),mm.mcol());
	}
    CDoubleMatrix work(numcol, numcol);
	int nn;
	work = copy(mm);
	nn = Matinv1(work,numcol, det);
	if(nn!=0)
	{
		if(nn>0)
			ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
		else
			WarningHandler((Matvec_warnings)nn, __FILE__, __LINE__);
	}
	return work;
}

CDoubleMatrix CDoubleMatrix ::inv(double &det)
{
	int numcol = mrow();
	if(mcol()!=numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        numcol = MIN(mrow(),mcol());
	}
    CDoubleMatrix work(numcol, numcol);
	int nn;
	work = copy();
	nn = Matinv1(work,numcol, det);
	if(nn!=0)
	{
		if(nn>0)
			ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
		else
			WarningHandler((Matvec_warnings)nn, __FILE__, __LINE__);
	}
	return work;
}
*/

CDoubleMatrix inv(const CDoubleMatrix &mm, double &det) //zfm
{
	int numcol = mm.mrow();
	assert(mm.mcol()==numcol);
	
    CDoubleMatrix work(numcol, numcol);
	int nn;
	work = copy(mm);
	nn = Matinv1(work,numcol, det);
	assert(nn==0);
	return work;
}

CDoubleMatrix CDoubleMatrix ::inv(double &det)const   //zfm
{ 
	int numcol = mrow();
	assert(mcol()==numcol);
	
    CDoubleMatrix work(numcol, numcol);
	int nn;
	work = copy();
	nn = Matinv1(work,numcol, det);
	assert(nn==0);
	
	return work;
}

CDoubleVector ldiv(CDoubleMatrix &m, CDoubleVector &v)
{
	
	int rows = m.mrow();
	int cols = m.mcol();
	int lens = v.vlen();
	//::WriteData(m,"第二次");
	CDoubleVector b(cols);
	
	if(rows != lens)
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
        rows = MIN(rows,lens);
		CDoubleVector v1(v,0,rows);
		v=v1;
		CDoubleMatrix m1(m,rows,cols);
		m=m1;
	}
	
	if(rows < cols)
	{
		CDoubleMatrix r;
		CDoubleMatrix q;
		CIntVector e;
		CDoubleVector bb;
		r = m.qr(q,e);
		::WriteData(m,"newm");
		::WriteData(r,"newr");
//#ifdef WINPLAT //  Windows             //===cur		
		CDoubleMatrix r2 = pinv(r);  //splited two line
		::WriteData(r2,"pinv(r)");
		//::WriteData(kk,"pinv(r)*trans(q)");
//#endif		
		
		CDoubleMatrix rr(r,rows,rows);
		//bb=pinv(r)*(trans(q)*v);  //zfm
		bb=pinv(rr)*(trans(q)*v);
		for(int i=0; i<rows; i++)
		{
			b(e(i)-1)=bb(i);   
		}
	}
	else
	{
		b = pinv(m)*v;
	}
	return b;
}

CDoubleMatrix ldiv(CDoubleMatrix &m, CDoubleMatrix &v)
{
	int rows = m.mrow();
	int cols = m.mcol();
	int rows1 = v.mrow();
	int cols1 = v.mcol();
	CDoubleMatrix b(cols,cols1);
	
	if(rows != rows1)
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
        rows = MIN(rows,rows1);
		CDoubleMatrix v1(v,rows,cols1);
		v=v1;
		CDoubleMatrix m1(m,rows,cols);
		m=m1;
	}
	
	if(rows < cols)
	{
		CDoubleMatrix r;
		CDoubleMatrix q;
		CIntVector e;
		CDoubleMatrix bb;
		r = m.qr(q,e);
		CDoubleMatrix rr(r,rows,rows);
		
		bb=pinv(rr)*(trans(q)*v);
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<cols1; j++)
			{
			    b(e(i)-1,j)=bb(i,j);
			}
		}
	}
	else
	{
		b = pinv(m)*v;
	}
	return b;
}


CDoubleMatrix hess(const CDoubleMatrix &x)
{
	int n = x.mrow();
	if(x.mcol()!=n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        n = MIN(x.mrow(),x.mcol());
	}
    int i, j;
	CDoubleVector d(n);
	CDoubleMatrix a(n,n);
	a = copy(x);
	double tol = TOLERANT_ERROR;
	orithes(n, tol, a, d);
	for(j=0; j<n-2; j++)
		for(i=j+2; i<n; i++)
			a(i,j)=0;
	return a;
}

CDoubleMatrix CDoubleMatrix::hess()const 
{
	int n = mrow();
	if(mcol()!=n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        n = MIN(mrow(),mcol());
	}
    int i, j;
	CDoubleVector d(n);
	CDoubleMatrix a(n,n);
	a = copy();
	double tol = TOLERANT_ERROR;
	orithes(n, tol, a, d);
	for(j=0; j<n-2; j++)
		for(i=j+2; i<n; i++)
			a(i,j)=0;
	return a;
}

CDoubleMatrix hess(const CDoubleMatrix &x, CDoubleMatrix &v)
{
	int n = x.mrow();
	if(n!=x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        n = MIN(x.mrow(),x.mcol());
	}
	CDoubleVector d(n);
	CDoubleMatrix a(n,n);
	if((v.mrow()!=n)||v.mcol()!=n)
		v = copy(a);
	a = copy(x);
	double tol = TOLERANT_ERROR;
	int i,j;
	orithes(n,tol,a,d);
	ortrans(n,a,d,v);
	for(j=0; j<n-2; j++)
		for(i=j+2; i<n; i++)
			a(i,j) =0;
	return a;
}

CDoubleMatrix CDoubleMatrix::hess( CDoubleMatrix &v)const 
{
	int n = mrow();
	if(n!=mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        n = MIN(mrow(),mcol());
	}
	CDoubleVector d(n);
	CDoubleMatrix a(n,n);
	if((v.mrow()!=n)||v.mcol()!=n)
		v = a.copy();
	a = copy();
	double tol = TOLERANT_ERROR;
	int i,j;
	orithes(n,tol,a,d);
	ortrans(n,a,d,v);
	for(j=0; j<n-2; j++)
		for(i=j+2; i<n; i++)
			a(i,j) =0;
	return a;
}

CDoubleVector svd(const CDoubleMatrix &x)  
{
	int i;
	int m = x.mrow();
	int n = x.mcol();
	int minmn = MIN(m,n);
	CDoubleVector aa(minmn);
	CDoubleMatrix a(m,n), u(m,m), v(n,n);
	a = copy(x);
	double eps = TOLERANT_ERROR;
	int state = dluav(a,u,v,m,n,eps);
	if(state !=0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	for(i = 0; i<minmn; i++)
		aa(i) = a(i,i);
	return aa;
}

CDoubleVector CDoubleMatrix::svd()const 
{
	int i;
	int m= mrow();
	int n= mcol();
	int minmn = MIN(m,n);
	CDoubleVector aa(minmn);
	CDoubleMatrix a(m,n), u(m,m), v(n,n);
	a = copy();
	double eps = TOLERANT_ERROR;
	int state = dluav(a,u,v,m,n,eps);
	if(state !=0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	for(i = 0; i<minmn; i++)
		aa(i) = a(i,i);
	return aa;
}

/*
CDoubleMatrix svd(CDoubleMatrix &x,CDoubleMatrix &U, CDoubleMatrix &V)
{
	int  i,j,iIndex,jIndex;
	long m = x.mrow();
	long n = x.mcol();
	long minmn = MIN(m,n);
	char JOBU='A';
	char JOBVT='A';
	long lda=m;
	long ldu=m;
	long ldvt=n;
	long info;
	long lwork=max(3*min(m,n)+max(m,n),5*min(m,n));
	double *work=new double[lwork];
	double *a=  new double[m*n];
	double *s=  new double[minmn];
	double *u=	new double[m*m];
	double *vt= new double[n*n];
	double eps = TOLERANT_ERROR;
		
	for(jIndex=0; jIndex<n; jIndex++)
	{
		for(iIndex=0; iIndex<m; iIndex++)
		{
			a[iIndex+jIndex*m]=x(iIndex,jIndex);
		}
	}
	
	int state=dgesvd_(&JOBU, &JOBVT, &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, work, &lwork, &info);
	
	CDoubleMatrix Utemp(m,m,0);
	for(i = 0; i<m; i++)
	{
		for(j=0; j<m; j++)
		{
			Utemp(i,j) = u[i+j*m];
			
		}
	}
	U=Utemp.copy();
		
	CDoubleMatrix Vtemp(n,n,0);
	for(i = 0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			Vtemp(i,j) = vt[i*n+j];
			
		}
	}
	
	V = Vtemp.copy();
	CDoubleMatrix tempMatrix(m,n,0);
	for(i=0; i<m; i++)
	{
		for(j=0;j<n;j++)
		{
			if(i==j)
				tempMatrix(i,i)=s[i];
			else
				tempMatrix(i,j)=0;
		}
	}
	
	delete []a;
	delete []s;
	delete []u;
	delete []vt;
	delete []work;
	return tempMatrix;
}

CDoubleMatrix CDoubleMatrix::svd(CDoubleMatrix &U, CDoubleMatrix &V)
{
	int  i,j,iIndex,jIndex;
	long m = mrow();
	long n = mcol();
	long minmn = MIN(m,n);
	char JOBU='A';
	char JOBVT='A';
	long lda=m;
	long ldu=m;
	long ldvt=n;
	long info;
	long lwork=max(3*min(m,n)+max(m,n),5*min(m,n));
	double *work=new double[lwork];
	double *a=  new double[m*n];
	double *s=  new double[minmn];
	double *u=	new double[m*m];
	double *vt= new double[n*n];
		
	for(jIndex=0; jIndex<n; jIndex++)
	{
		for(iIndex=0; iIndex<m; iIndex++)
		{
			a[iIndex+jIndex*m]=(*this)(iIndex,jIndex);
		}
	}
	
	int state=dgesvd_(&JOBU, &JOBVT, &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, work, &lwork, &info);
	
	CDoubleMatrix Utemp(m,m,0);
	for(i = 0; i<m; i++)
	{
		for(j=0; j<m; j++)
		{
			Utemp(i,j) = u[i+j*m];
			
		}
	}
	U=Utemp.copy();
	
	
	CDoubleMatrix Vtemp(n,n,0);

	for(i = 0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			Vtemp(i,j) = vt[i*n+j];
			
		}
	}
	
	V = Vtemp.copy();
	CDoubleMatrix tempMatrix(m,n,0);
	for(i=0; i<m; i++)
	{
		for(j=0;j<n;j++)
		{
			if(i==j)
				tempMatrix(i,i)=s[i];
			else
				tempMatrix(i,j)=0;
		}
	}
	
	delete []a;
	delete []s;
	delete []u;
	delete []vt;
	delete []work;
	return tempMatrix;


}
*/
CDoubleVector svd_rank(const CDoubleMatrix &x)  //svd分解，求rank专用
{

	char JOBU='N';
	char JOBVT='N';
	long m = x.mrow();
	long n = x.mcol();
	long minmn = min(m,n);
	long lda=m;
	long ldu=m;
	double *a=new double[m*n];
	double *s=new double[minmn];
	double u;
	long ldvt=n;
	double vt;
	long lwork=max(3*min(m,n)+max(m,n),5*min(m,n));
	double *work=new double[lwork];
	long info;
		
	for(int jIndex=0; jIndex<n; jIndex++)
	{
		for(int iIndex=0; iIndex<m; iIndex++)
		{
			a[iIndex+jIndex*m]=x.GetValue(iIndex,jIndex);
		}
	}
	
	int state=dgesvd_(&JOBU, &JOBVT, &m, &n, a, &lda, s, &u, &ldu, &vt, &ldvt, work, &lwork, &info);
	CDoubleVector ss(s,minmn);
	delete []a;
	delete []s;
	delete []work;
	return ss;
}

CDoubleMatrix svd(const CDoubleMatrix &x, CDoubleMatrix &y, CDoubleMatrix &z)
{
    int m=x.mrow();
	int n=x.mcol();
	CDoubleMatrix a(m,n);
	if((y.mrow()!=m)||(y.mcol()!=m))
	{
		CDoubleMatrix tmp1(m,m);
		y = tmp1;
	}
	if((z.mrow()!=n)||(z.mcol()!=n))
	{
		CDoubleMatrix tmp2(n,n);
		z = tmp2;
	}
	a = copy(x);
	double eps = TOLERANT_ERROR;
	int state = dluav(a,y,z,m,n,eps);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	return a;
}


CDoubleMatrix CDoubleMatrix::svd(CDoubleMatrix &y, CDoubleMatrix &z)const 
{
    int m=mrow();
	int n=mcol();
	CDoubleMatrix a(m,n);
	if((y.mrow()!=m)||(y.mcol()!=m))
	{
		CDoubleMatrix tmp1(m,m);
		y = tmp1;
	}
	if((z.mrow()!=n)||(z.mcol()!=n))
	{
		CDoubleMatrix tmp2(n,n);
		z = tmp2;
	}
	a = copy();
	double eps = TOLERANT_ERROR;
	int state = dluav(a,y,z,m,n,eps);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	return a;
}

CDoubleMatrix CDoubleMatrix::pinv()const 
{
	int m=mrow();
	int n=mcol();
	int t;
	double eps = TOLERANT_ERROR;
	CDoubleMatrix work(m,n);
	CDoubleMatrix ss(n,m);
	work = copy();
//	::WriteData(work,m,n,"workold");
	t = dginv(work,ss,m,n,eps);
//	::WriteData(work,m,n,"workold");
//	::WriteData(ss,n,m,"ssold");

	if(t != 0)
	{
		ErrorHandler((Matvec_errors)t, __FILE__, __LINE__);
	}
	return ss;
}


CDoubleMatrix pinv(const CDoubleMatrix &x)
{
	int m=x.mrow();
	int n=x.mcol();
	int t;
	double eps = TOLERANT_ERROR;
	CDoubleMatrix work(m,n);
	CDoubleMatrix ss(n,m);
	work = copy(x);
	t = dginv(work,ss,m,n,eps);
	if(t != 0)
	{
		ErrorHandler((Matvec_errors)t, __FILE__, __LINE__);
	}
	return ss;
} 


/*
//zfm  2003-2-21 快速广义逆
CDoubleMatrix CDoubleMatrix::pinv()   
{
	int i,l,k,iIndex,jIndex;
	long j,maxmn;
	long m=mrow();
	long n=mcol();
	double eps = TOLERANT_ERROR;
	long minmn=min(m,n);
		
	CDoubleMatrix work(m,n);
	CDoubleMatrix U;
	CDoubleMatrix V;
	CDoubleMatrix ss(n,m);
	char JOBU='A';
	char JOBVT='A';
	long lda=m;
	long ldu=m;
	long ldvt=n;
	long info;
	long lwork=max(3*min(m,n)+max(m,n),5*min(m,n));
	double *a   =new double[m*n];
	double *wk=new double[lwork];
	double *u = new double[m*m];
	double *vt = new double[n*n];
	double *s=  new double[minmn];
	for(jIndex=0; jIndex<n; jIndex++)
	{
		for(iIndex=0; iIndex<m; iIndex++)
		{
			a[iIndex+jIndex*m]=(*this)(iIndex,jIndex);
		}
	}
	int state=dgesvd_(&JOBU, &JOBVT, &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, wk, &lwork, &info);
	
	CDoubleMatrix Utemp(m,m,0);
	for(i = 0; i<m; i++)
	{
		for(j=0; j<m; j++)
		{
			Utemp(i,j) = u[i+j*m];
			
		}
	}
	U=Utemp.copy();
	
		
	CDoubleMatrix Vtemp(n,n,0);
	for(i = 0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			Vtemp(i,j) = vt[i+j*n];
			
		}
	}
	
	V = Vtemp.copy();	

	for(i=0; i<m; i++)
	{
		for(j=0;j<n;j++)
		{
			if(i==j)
				work(i,i)=s[i];
			else
				work(i,j)=0;
		}
	}
	
	j=n;
	maxmn = m;
	if(m<n)
	{
		j=m;
		maxmn = n;
	}
	j=j-1;
	k=0;
	while((k<=j)&&(fabs(work(k,k))>((double)maxmn*work(0,0)*eps)))
		k=k+1;
	k=k-1;
	for(i=0; i<=n-1; i++)
	{
		for(j=0; j<=m-1; j++)
		{
			ss(i,j) = 0;
			for(l=0; l<=k; l++)
				ss(i,j) += V(l,i)*U(j,l)/work(l,l);
		}
	}

	delete []a;
	delete []s;
	delete []u;
	delete []vt;
	delete []wk;

	return ss;
}
*/


CDoubleMatrix Commat(const CDoubleMatrix &x)
{
	int i,l,k,iIndex,jIndex;
	long j,maxmn;
	long m=x.mrow();
	long n=x.mcol();
	double eps = TOLERANT_ERROR;
	long minmn=min(m,n);
		
	CDoubleMatrix work(m,n);
	CDoubleMatrix U;
	CDoubleMatrix V;
	CDoubleMatrix ss(n,m);
	char JOBU='A';
	char JOBVT='A';
	long lda=m;
	long ldu=m;
	long ldvt=n;
	long info;
	long lwork=max(3*min(m,n)+max(m,n),5*min(m,n));
	double *a   =new double[m*n];
	double *wk=new double[lwork];
	double *u = new double[m*m];
	double *vt = new double[n*n];
	double *s=  new double[minmn];
	for(jIndex=0; jIndex<n; jIndex++)
	{
		for(iIndex=0; iIndex<m; iIndex++)
		{
			a[iIndex+jIndex*m]=x.GetValue(iIndex,jIndex);
		}
	}
	int state=dgesvd_(&JOBU, &JOBVT, &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, wk, &lwork, &info);
	
	CDoubleMatrix Utemp(m,m,0.0);
	for(i = 0; i<m; i++)
	{
		for(j=0; j<m; j++)
		{
			Utemp(i,j) = u[i+j*m];
			
		}
	}
	U=Utemp.copy();
	
		
	CDoubleMatrix Vtemp(n,n,0.0);
	for(i = 0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			Vtemp(i,j) = vt[i+j*n];
			
		}
	}
	V = Vtemp.copy();	

	for(i=0; i<m; i++)
	{
		for(j=0;j<n;j++)
		{
			if(i==j)
				work(i,i)=s[i];
			else
				work(i,j)=0;
		}
	}
	
	j=n;
	maxmn = m;
	if(m<n)
	{
		j=m;
		maxmn = n;
	}
	j=j-1;
	k=0;
	while((k<=j)&&(fabs(work(k,k))>((double)maxmn*work(0,0)*eps)))
		k=k+1;
	k=k-1;
	for(i=0; i<=n-1; i++)
	{
		for(j=0; j<=m-1; j++)
		{
			ss(i,j) = 0;
			for(l=0; l<=k; l++)
				ss(i,j) += V(l,i)*U(j,l)/work(l,l);
		}
	}

	delete []a;
	delete []s;
	delete []u;
	delete []vt;
	delete []wk;

	return ss;

}



CComplexVector eig(const CDoubleMatrix &x)
{
	int n=x.mrow();
	if(x.mcol()!=n)
		ErrorHandler(EMAT_NOTSQUARE, __FILE__,__LINE__);
    CComplexVector ee(n);
	CDoubleVector wr(n),wi(n),d(n);
	CIntVector cnt(n);
	CDoubleMatrix h(n,n), v(n,n);
	h = copy(x);
	double eps = 1e-100;
	orithes(n,eps,h,d);
	ortrans(n,h,d,v);
	int state = hqr2(n,eps,h,v,wr,wi,cnt);
	if(state !=0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	ee = Complex(wr, wi);
	return ee;	
}

CComplexVector CDoubleMatrix::eig()const 
{
	int n=mrow();
	if(mcol()!=n)
		ErrorHandler(EMAT_NOTSQUARE, __FILE__,__LINE__);
    CComplexVector ee(n);
	CDoubleVector wr(n),wi(n),d(n);
	CIntVector cnt(n);
	CDoubleMatrix h(n,n), v(n,n);
	h = copy();
	double eps = 1e-100;
	orithes(n,eps,h,d);
	ortrans(n,h,d,v);
	int state = hqr2(n,eps,h,v,wr,wi,cnt);
	if(state !=0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	ee = Complex(wr, wi);
	return ee;	
}
//{
//	//		CComplexVector vec = Complex(wr,wi);
//	//		ee = ::diag(vec);

CComplexMatrix CDoubleMatrix::eig(CComplexMatrix &s)const 
{
	
	int i,j;
	int n=mrow();
	if(mcol()!=n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		n = MIN(mrow(), mcol());
	}
	CComplexMatrix ee(n,n);
	CDoubleVector wr(n), wi(n), d(n);
	CIntVector cnt(n);
	CDoubleMatrix h(n,n),v(n,n);
	if((s.mrow()!=n)||(s.mcol()!=n))
		s=ee;
	h = copy();
	double eps = 1e-308; //zfm 2003-04-03
	orithes(n,eps,h,d);
	ortrans(n,h,d,v);
	
	int state = hqr2(n,eps,h,v,wr,wi,cnt);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}

	CComplexVector vec(3,4);
	CComplexVector vv;
	vv = vec;
	vec = Complex(wr,wi);
	ee = ::diag(vec);

	for(j=0; j<n; j++)
	{
		if(wi(j)==0)
		{
			for(i=0; i<n; i++)
				s(i,j) = CComplex(v(i,j), 0);
		}
		else
		{
			if(wi(j)>0)
			{
				for(i=0; i<n; i++)
					s(i,j) = CComplex(v(i,j), v(i,j+1));
			}
			else
			{
				for(i=0; i<n; i++)
					s(i,j) = conj(s(i,j-1));
			}
		}
	}
	return ee;
}

CComplexMatrix eig(const CDoubleMatrix &x, CComplexMatrix &s)
{
	return x.eig(s);
}

CDoubleMatrix expm(CDoubleMatrix &a)
{
	int mn=a.mrow();
	double iss;
	if(mn != a.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(a.mrow(), a.mcol());
	}
    CDoubleMatrix ax(mn, mn), x(mn, mn), e(mn, mn),d(mn,mn),cx(mn, mn);
	double s= norm(a,0);
	if(s>0)
		iss = MAX(0,(int)floor(log(s)/log(2))+2);
	else
		iss = (int)s;
	ax = (a)/(pow(2,iss));
	x = copy(ax);
	double c =0.5;
	e = eye(mn) + c*ax;
	d = eye(mn) - c*ax;
	int q=60;
	int p=1;
	int qp1 = q+1;
	int q2p1 = 2*q +1;
	int k= 0;
	for(k=2; k<q; k++)
	{
		c= c* (qp1-k)/(k*(q2p1-k));
		x=ax*x;
		cx=c*x;
		e = e+cx;
		if(p)
			d = d+cx;
		else
			d = d-cx;
		p = 1-p;
	}
	e = (d.inv())*e;
	for(k=1; k<=iss; k++)
		e = e*e;
	return e;
}

CDoubleMatrix CDoubleMatrix::expm()const 
{
	int mn=mrow();
	double iss;
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
    CDoubleMatrix ax(mn, mn), x(mn, mn), e(mn, mn),d(mn,mn),cx(mn, mn);
	double s= norm(0);
	if(s>0)
		iss = MAX(0,(int)floor(::log(s)/::log(2))+2);
	else
		iss = s;
	ax = (*this)/(::pow(2,iss));
	x = ::copy(ax);
	double c =0.5;
	e = ::eye(mn) + c*ax;
	d = ::eye(mn) - c*ax;
	int q=60;
	int p=1;
	int qp1 = q+1;
	int q2p1 = 2*q +1;
	int k=0;
	for(k=2; k<q; k++)
	{
		c= c* (qp1-k)/(k*(q2p1-k));
		x=ax*x;
		cx=c*x;
		e = e+cx;
		if(p)
			d = d+cx;
		else
			d = d-cx;
		p = 1-p;
	}
	e = (d.inv())*e;
	for(k=1; k<=iss; k++)
		e = e*e;
	return e;
}

CDoubleMatrix expm1(const CDoubleMatrix &a)
{
	int m= a.mrow();
	int n= a.mcol();
	int mn=m;
	if(m != n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(m,n);
	}

	CDoubleMatrix ax(mn,mn),e(mn,mn), f(mn,mn);
    int s = (int)ceil(log(norm(a,1))/log(2)+1.5);
	if(s<0)
		s=0;
	ax = (a)/(pow(2,s*1.0));
	f = eye(mn,mn);
	int k=1;
	double nrm = norm(f,1);
	while(nrm>0)
	{
		e=e+f;
		f = ax*f/(double)k;
		k=k+1;
		nrm= norm(f,1);
	}
	for(k=1; k<=s; k++)
		e=e*e;
	return e;
}

CDoubleMatrix CDoubleMatrix::expm1()const 
{
	int m= mrow();
	int n= mcol();
	int mn=m;
	if(m != n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}

	CDoubleMatrix ax(mn,mn),e(mn,mn), f(mn,mn);
    int s = (int)ceil(::log(norm(1))/::log(2)+1.5);
	if(s<0)
		s=0;
	ax = (*this)/(::pow(2,s*1.0));
	f = ::eye(mn,mn);
	int k=1;
	double nrm = ::norm(f,1);
	while(nrm>0)
	{
		e=e+f;
		f = ax*f/(double)k;
		k=k+1;
		nrm= ::norm(f,1);
	}
	for(k=1; k<=s; k++)
		e=e*e;
	return e;
}

CDoubleMatrix expm2(const CDoubleMatrix &x)
{
    int mn= x.mrow();
	if(mn != x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(x.mrow(), x.mcol());
	}
	CDoubleMatrix vv(mn,mn);
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(x,v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::exp(d(i,i));
	e = v*tmp*(::inv(v));
	vv = real(e);
	return vv;
}

CDoubleMatrix CDoubleMatrix::expm2()const 
{
    int mn= mrow();
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
	CDoubleMatrix vv(mn,mn);
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::exp(d(i,i));
	e = v*tmp*(::inv(v));
	vv = real(e);
	return vv;
}

CComplexMatrix logm2(const CDoubleMatrix &x)
{
    int mn= x.mrow();
	if(mn != x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(x.mrow(), x.mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(x,v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::log(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix CDoubleMatrix::logm2()const 
{
    int mn= mrow();
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = ::eig(v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::log(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}


CComplexMatrix sqrtm2(const CDoubleMatrix &x)
{
    int mn= x.mrow();
	if(mn != x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(x.mrow(), x.mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(x,v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::sqrt(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix CDoubleMatrix::sqrtm2()const 
{
    int mn= mrow();
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::sqrt(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix CDoubleMatrix::powm2(const CComplex &c)const 
{
    int mn= mrow();
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::pow(d(i,i),c);
	e = v*tmp*(::inv(v));
	return e;

}

CComplexMatrix powm2(const CDoubleMatrix &x, const CComplex &c)
{
    int mn= x.mrow();
	if(mn != x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(x.mrow(), x.mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(x,v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::pow(d(i,i),c);
	e = v*tmp*(::inv(v));
	return e;
}


CDoubleVector poly(const CDoubleMatrix &x)
{
	int n=x.mrow();
	int n1 = x.mcol();
	if(n != n1)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		n = MIN(x.mrow(), x.mcol());
	}
    CDoubleVector c(n+1);
	CComplexVector e(n), cc(n+1), ctmp(n+1);
	e = eig(x);
	cc(0) = 1;
	for(int j=0; j<=n-1; j++)
	{
		ctmp = copy(cc);
		for(int i=1; i<=j+1;i++)
			cc(i) = cc(i) - e(j)*ctmp(i-1);
	}
	c = real(cc);
	return c;
}

CDoubleVector CDoubleMatrix::poly()const 
{
	int n=mrow();
	int n1 = mcol();
	if(n != n1)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		n = MIN(mrow(), mcol());
	}
    CDoubleVector c(n+1);
	CComplexVector e(n), cc(n+1), ctmp(n+1);
	e = eig();
	cc(0) = 1;
	for(int j=0; j<=n-1; j++)
	{
		ctmp = ::copy(cc);
		for(int i=1; i<=j+1;i++)
			cc(i) = cc(i) - e(j)*ctmp(i-1);
	}
	c = ::real(cc);
	return c;
}

CDoubleMatrix normcdf(const CDoubleMatrix &m, double mu, double sigma)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = normcdf(m.GetValue(i,j), mu, sigma);
	return mm;
}

CDoubleMatrix chi2cdf(const CDoubleMatrix &m, int v)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = chi2cdf(m.GetValue(i,j), v);
	return mm;

}

CDoubleMatrix fcdf(const CDoubleMatrix &m, int v1, int v2)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = fcdf(m.GetValue(i,j), v1, v2);
	return mm;
}

CDoubleMatrix tcdf(const CDoubleMatrix &m, int v)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = tcdf(m.GetValue(i,j), v);
	return mm;
}


CDoubleMatrix normpdf(const CDoubleMatrix &m, double mu, double sigma)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = normpdf(m.GetValue(i,j), mu, sigma);
	return mm;
}

CDoubleMatrix chi2pdf(const CDoubleMatrix &m, int v)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = chi2pdf(m.GetValue(i,j), v);
	return mm;

}

CDoubleMatrix fpdf(const CDoubleMatrix &m, int v1, int v2)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = fpdf(m.GetValue(i,j), v1, v2);
	return mm;
}


CDoubleMatrix tpdf(const CDoubleMatrix &m, int v)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = tpdf(m.GetValue(i,j), v);
	return mm;
}



CDoubleMatrix norminv(const CDoubleMatrix &m, double mu, double sigma)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = norminv(m.GetValue(i,j), mu, sigma);
	return mm;
}

CDoubleMatrix chi2inv(const CDoubleMatrix &m, int v)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = chi2inv(m.GetValue(i,j), v);
	return mm;

}

CDoubleMatrix finv(const CDoubleMatrix &m, int v1, int v2)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = finv(m.GetValue(i,j), v1, v2);
	return mm;
}

CDoubleMatrix tinv(const CDoubleMatrix &m, int v)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	CDoubleMatrix mm(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			mm(i,j) = tinv(m.GetValue(i,j), v);
	return mm;
}

/*
CDoubleMatrix CDoubleMatrix::qr1(CDoubleMatrix &q, int flag)
{
	int nrows = mrow();
	int ncols = mcol();
	int nrows1;
	if(nrows<ncols)
		nrows1=ncols;
	else
		nrows1=nrows;
	CDoubleMatrix r(nrows1, ncols, 0.0);
    for(int i=0; i<nrows; i++)
		for(int j=0; j<ncols; j++)
			r(i,j) = (*this)(i,j);
	if (q.mrow()!=nrows1 || q.mcol()!=nrows1)
	{
		CDoubleMatrix v(nrows1, nrows1);
		q = v;
	}
	int state = bmaqr(r, nrows1, ncols, q);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows>=ncols)
	{
        CDoubleMatrix rr(r, ncols, ncols);
		CDoubleMatrix qq(q, nrows, ncols);
		r = rr;
		q = qq;
	}
	else if(nrows < ncols)
	{
		CDoubleMatrix rr(r,nrows, ncols);
		CDoubleMatrix qq(q,nrows, nrows);
		r = rr;
		q = qq;
	}

	return r;
}
*/
CDoubleMatrix CDoubleMatrix::qr(CDoubleMatrix &q, int flag) const
{
	int nrows = mrow();
	int ncols = mcol();
	CDoubleMatrix r=copy();
	if (q.mrow()!=nrows || q.mcol()!=nrows)
	{
		CDoubleMatrix v(nrows, nrows);
		q = v;
	}
	int state = bmaqr(r, nrows, ncols, q);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows >= ncols)
	{
        CDoubleMatrix rr(r, ncols, ncols);
		CDoubleMatrix qq(q, nrows, ncols);
		r = rr;
		q = qq;
	}
	return r;
}
/*
CDoubleMatrix qr1(CDoubleMatrix &x, CDoubleMatrix &q, int flag)
{
	int nrows = x.mrow();
	int ncols = x.mcol();
	int nrows1;
	if(nrows<ncols)
		nrows1=ncols;
	else
		nrows1=nrows;
	CDoubleMatrix r(nrows1, ncols);
    for(int i=0; i<nrows; i++)
		for(int j=0; j<ncols; j++)
			r(i,j) = x(i,j);
	if (q.mrow()!=nrows1 || q.mcol()!=nrows1)
	{
		CDoubleMatrix v(nrows1, nrows1);
		q = v;
	}
	int state = bmaqr1(r, nrows1, ncols, q);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows >= ncols)
	{
        CDoubleMatrix rr(r, ncols, ncols);
		CDoubleMatrix qq(q, nrows, ncols);
		r = rr;
		q = qq;
	}
	else if(nrows < ncols)
	{
		CDoubleMatrix rr(r,nrows, ncols);
		CDoubleMatrix qq(q,nrows, nrows);
		r = rr;
		q = qq;
	}
	return r;
}
*/
CDoubleMatrix qr(CDoubleMatrix &x, CDoubleMatrix &q, int flag)
{
	int nrows = x.mrow();
	int ncols = x.mcol();
	CDoubleMatrix r=x.copy();
	if (q.mrow()!=nrows || q.mcol()!=nrows)
	{
		CDoubleMatrix v(nrows, nrows);
		q = v;
	}
	int state = bmaqr(r, nrows, ncols, q);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows >= ncols)
	{
        CDoubleMatrix rr(r, ncols, ncols);
		CDoubleMatrix qq(q, nrows, ncols);
		r = rr;
		q = qq;
	}
	return r;
}

/*
CDoubleMatrix CDoubleMatrix::qr(CDoubleMatrix &q, CIntVector &e, int flag)
{
	int nrows = mrow();
	int ncols = mcol();
	int nrows1;
	if(nrows<ncols)
		nrows1=ncols;
	else
		nrows1=nrows;
	CDoubleMatrix r(nrows1, ncols);
    for(int i=0; i<nrows; i++)
		for(int j=0; j<ncols; j++)
			r(i,j) = (*this)(i,j);
	if (q.mrow()!=nrows1 || q.mcol()!=nrows1)
	{
		CDoubleMatrix v(nrows1, nrows1);
		q = v;
	}
	if (e.vlen()!=ncols)
	{
		CIntVector vec(ncols);
		e = vec;
	}
	int state = bmaqr1(r, nrows1, ncols, q, e);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows >= ncols)
	{
        CDoubleMatrix rr(r, ncols, ncols);
		CDoubleMatrix qq(q, nrows, ncols);
		r = rr;
		q = qq;
	}
	else if(nrows < ncols)
	{
		CDoubleMatrix rr(r,nrows, ncols);
		CDoubleMatrix qq(q,nrows, nrows);
		r = rr;
		q = qq;
	}
	return r;
}
*/

CDoubleMatrix qr(CDoubleMatrix &x, CDoubleMatrix &q, CIntVector &e, int flag)
{
	int nrows = x.mrow();
	int ncols = x.mcol();
	CDoubleMatrix r=x.copy();
	if (q.mrow()!=nrows || q.mcol()!=nrows)
	{
		CDoubleMatrix v(nrows, nrows);
		q = v;
	}
	if (e.vlen()!=ncols)
	{
		CIntVector vec(ncols);
		e = vec;
	}
	int state = bmaqr(r, nrows, ncols, q, e);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows >= ncols)
	{
        CDoubleMatrix rr(r, ncols, ncols);
		CDoubleMatrix qq(q, nrows, ncols);
		r = rr;
		q = qq;
	}
	return r;
}
/*
CDoubleMatrix qr1(CDoubleMatrix &x, CDoubleMatrix &q, CIntVector &e, int flag)
{
	int nrows = x.mrow();
	int ncols = x.mcol();
	CDoubleMatrix r(nrows, ncols);
	r = x.copy();
	if (q.mrow()!=nrows || q.mcol()!=nrows)
	{
		CDoubleMatrix v(nrows, nrows);
		q = v;
	}
	if (e.vlen()!=ncols)
	{
		CIntVector vec(ncols);
		e = vec;
	}
	int state = bmaqr1(r, nrows, ncols, q, e);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0)
	{
        CDoubleMatrix rr(r, ncols, ncols);
		CDoubleMatrix qq(q, nrows, ncols);
		r = rr;
		q = qq;
	}
	return r;
}

*/

CDoubleMatrix CDoubleMatrix::qrZkc(CDoubleMatrix &q, CIntVector &e, int flag)
{
	int nrows = mrow();
	int ncols = mcol();

	CDoubleMatrix r=copy();

	if (q.mrow()!=nrows || q.mcol()!=nrows)
	{
		CDoubleMatrix v(nrows, nrows);
		q = v;
	}
	if (e.vlen()!=ncols)
	{
		CIntVector vec(ncols);
		e = vec;
	}
	//int kkk,jjj;
	WriteData(r,"r");
	int state = bmaqr(r, nrows, ncols, q, e);

	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	WriteData(r,"r");
	WriteData(q,"q");
	if(flag==0 && nrows >= ncols)
	{
        CDoubleMatrix rr(r, ncols, ncols);
		CDoubleMatrix qq(q, nrows, ncols);
		r = rr;
		q = qq;
	}
	WriteData(r,"r1");
	WriteData(q,"q1");
	return r;
}

/*
CDoubleMatrix CDoubleMatrix::qr(CDoubleMatrix &q)  //zfm 
{
	
	long mrows = mrow();
	long ncols = mcol();


	long minmn;
	if( mrows <= ncols )
	{
		minmn = mrows;
	}
	else
		minmn = ncols;

	minmn = min(mrows,ncols);

    
	//初始化
	long nb = 128;
	long INFO;
	long LWORK = nb*ncols;
	long LDA = mrows;
	double *A = new double[mrows*ncols];
	
	double * TAU = new double[minmn];
	double * WORK = new double[LWORK];
	
	CDoubleMatrix AT = trans();
	
	//A矩阵转置
	for( int iIndex = 0; iIndex < ncols; iIndex ++)
	{
		for( int jIndex = 0; jIndex < mrows ; jIndex ++)
		{
			A[iIndex * mrows + jIndex] = AT(iIndex,jIndex);  
			
		}
	}
	

	int state_r=dgeqrf_(&mrows, &ncols, A, &LDA, TAU, WORK, &LWORK, &INFO);


	
	CDoubleMatrix RT(minmn,ncols,0);

	int i,j;
	for(i = 0; i<minmn; i++)
	{
		for(j=i; j<ncols; j++)
		{
			RT(i,j) = A[j*mrows + i];
		}
	}

	int state_q =  dorgqr_(&mrows, &minmn, &minmn, A, &LDA, TAU, WORK, &LWORK ,&INFO);
	


	CDoubleMatrix QT(mrows,minmn,0);

	for(i = 0; i<mrows; i++)
	{
		for(j=0; j<minmn; j++)
		{
			QT(i,j) = A[j*mrows+i];
		}
	}


	
	delete[]TAU;
	
	delete[]WORK;
	delete[]A;
	
	q = QT.copy();
	return RT;

}
*/

CDoubleMatrix CDoubleMatrix::qr(CDoubleMatrix &q, CIntVector &e, int flag) const
{

//	::WriteData((*this),"ThisNew");//zfm
	long mrows = mrow();
	long ncols = mcol();

	int nflag = 0;
	long minmn;
	if( mrows <= ncols )
	{
		minmn = mrows;
		flag = 1;
	}
	else
		minmn = ncols;

	minmn = min(mrows,ncols);
	if (e.vlen()!=ncols)
	{
		CIntVector vec(ncols);
		e = vec;
	}
    
	//初始化
	long nb = 128;
	long INFO;
	long LWORK = nb*ncols;
	long LDA = mrows;
	double *A = new double[mrows*ncols];
	
	long    * E = new long[ncols];
	double * TAU = new double[minmn];
	double * WORK = new double[LWORK];
	int ee=0;
	for(ee=0;ee<ncols;ee++)
		E[ee]=0;

	CDoubleMatrix AT = trans();
	
	//A矩阵转置
	for( int iIndex = 0; iIndex < ncols; iIndex ++)
	{
		for( int jIndex = 0; jIndex < mrows ; jIndex ++)
		{
			A[iIndex * mrows + jIndex] = AT(iIndex,jIndex);  
			
		}
	}
	

	int state_r= dgeqp3_(&mrows, &ncols, A, &LDA, E, TAU, WORK, &LWORK, &INFO);


	
	CDoubleMatrix RT(minmn,ncols,0.0);

	int i,j;
	for(i = 0; i<minmn; i++)
	{
		for(j=i; j<ncols; j++)
		{
			RT(i,j) = A[j*mrows + i];
		}
	}

	int state_q = dorgqr_(&mrows, &minmn, &minmn, A, &LDA, TAU, WORK, &LWORK ,&INFO);
	


	CDoubleMatrix QT(mrows,minmn,0.0);

	for(i = 0; i<mrows; i++)
	{
		for(j=0; j<minmn; j++)
		{
			QT(i,j) = A[j*mrows+i];
		}
	}

	for( ee = 0; ee < e.vlen(); ee ++)
		e(ee)=E[ee];

	
	delete[]TAU;
	delete[]E;
	delete[]WORK;
	delete[]A;
	
	q = QT.copy();
	return RT;


}


CDoubleVector var(const CDoubleMatrix &m)
{
    int nrows = m.mrow();
	CDoubleVector v(nrows);
	for(int i=0; i<nrows; i++)
		v(i) = var((CDoubleVector)m.GetValue(i));
	return v;
}

CDoubleVector st_d(const CDoubleMatrix &m)
{
    int nrows = m.mrow();
	CDoubleVector v(nrows);
	for(int i=0; i<nrows; i++)
		v(i) = st_d((CDoubleVector)m.GetValue(i));
	return v;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComplexMatrix::CComplexMatrix(const CDoubleMatrix &re, const CDoubleMatrix &im):CMatrix<CComplex>
                              (MIN(re.mrow(),im.mrow()),MIN(re.mcol(),im.mcol()))
{
	for(int i=0; i<mcol(); i++)
	{
		this->GetVector(i) = ::Complex((CDoubleVector)re.gGetVector(i),(CDoubleVector)im.gGetVector(i));
//		(*this)(i) =::Complex((CDoubleVector)re(i),(CDoubleVector)im(i));
	}
}

CComplexMatrix::CComplexMatrix(const CMatrix<double> &re, const CMatrix<double> &im):CMatrix<CComplex>
                              (MIN(re.mrow(),im.mrow()),MIN(re.mcol(),im.mcol()))
{
	for(int i=0; i<mcol(); i++)
	{
		this->GetVector(i)=::Complex((CDoubleVector)re.gGetVector(i),(CDoubleVector)im.gGetVector(i));
//		(*this)(i)=::Complex((CDoubleVector)re(i),(CDoubleVector)im(i));
	}
}

CComplexMatrix::CComplexMatrix(const CDoubleMatrix &x):CMatrix<CComplex>
                              (x.mrow(),x.mcol())
{
	for(int i=0; i<mcol(); i++)
	{
		this->GetVector(i)= ::Complex((CDoubleVector)x.gGetVector(i),0);
//		(*this)(i)= ::Complex((CDoubleVector)x(i),0);
	}
}

CComplexMatrix::CComplexMatrix(const CMatrix<double> &x):CMatrix<CComplex>
                              (x.mrow(),x.mcol())
{
	for(int i=0; i<mcol(); i++)
	{
		this->GetVector(i)= ::Complex((CDoubleVector)x.gGetVector(i),0);
//		(*this)(i)= ::Complex((CDoubleVector)x(i),0);
	}
}

CComplexMatrix & CComplexMatrix::operator =(const CComplexMatrix & vec)
{
	this->CMatrix<CComplex> ::operator =(vec);
	return *this;
}


CComplexMatrix & CComplexMatrix::operator =(const CMatrix<CComplex> & vec)
{
	this->CMatrix<CComplex> ::operator =(vec);
	return *this;
}


CComplexMatrix & CComplexMatrix::operator =(const CVector<CComplex> & vec)
{
	this->CMatrix<CComplex> ::operator =(vec);
	return *this;
}


CComplexMatrix & CComplexMatrix::operator =(const CComplexVector & vec)
{
	this->CMatrix<CComplex> ::operator =(vec);
	return *this;
}


CComplexMatrix & CComplexMatrix::operator =(const CComplex & vec)
{
	this->CMatrix<CComplex> ::operator =(vec);
	return *this;
}


CComplexMatrix & CComplexMatrix::operator =(const CDoubleMatrix & vec)
{
	*this = dmattocmat(vec);
	return *this;
}

CComplexMatrix & CComplexMatrix::operator =(const CMatrix<double> & vec)
{
	*this = dmattocmat((CDoubleMatrix) vec);
	return *this;
}

CComplexMatrix & CComplexMatrix::operator =(CComplex *vec)
{
    this->CMatrix<CComplex>::operator =(vec);
	return * this;
}

CComplexMatrix & CComplexMatrix::operator =(CComplex **vec)
{
    this->CMatrix<CComplex>::operator =(vec);
	return * this;
}

CComplexMatrix dmattocmat(const CDoubleMatrix &vec)
{
	int len = vec.mrow();
	int ven = vec.mcol();
	CComplexMatrix v(len,ven);
	for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j)=CComplex(vec.GetValue(i,j),0);
	return v;
}
CComplexMatrix imattocmat(const CIntMatrix &vec)
{
	int len = vec.mrow();
	int ven = vec.mcol();
	CComplexMatrix v(len,ven);
	for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j)=CComplex(vec.GetValue(i,j),0);
	return v;
}
CComplexMatrix fmattocmat(const CFloatMatrix &vec)
{
	int len = vec.mrow();
	int ven = vec.mcol();
	CComplexMatrix v(len,ven);
	for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j)=CComplex(vec.GetValue(i,j),0);
	return v;
}

CDoubleMatrix real(const CComplexMatrix &x)
{
	int nrow = x.mrow();
	int ncol = x.mcol();
	CDoubleMatrix v(nrow, ncol);
	for(int i=0; i<nrow; i++)
		for(int j=0; j<ncol; j++)
			v(i,j) = real(x.GetValue(i,j));
	return v;
}


CDoubleMatrix CComplexMatrix ::real()const 
{
	int nrow = mrow();
	int ncol = mcol();
	CDoubleMatrix v(nrow, ncol);
	for(int i=0; i<nrow; i++)
		for(int j=0; j<ncol; j++)
			v(i,j) = ::real((*this).GetValue(i,j));
	return v;
}

CDoubleMatrix imag(const CComplexMatrix &x)
{
	int nrow = x.mrow();
	int ncol = x.mcol();
	CDoubleMatrix v(nrow, ncol);
	for(int i=0; i<nrow; i++)
		for(int j=0; j<ncol; j++)
			v(i,j) = imag(x.GetValue(i,j));
	return v;
}


CDoubleMatrix CComplexMatrix ::imag()const 
{
	int nrow = mrow();
	int ncol = mcol();
	CDoubleMatrix v(nrow, ncol);
	for(int i=0; i<nrow; i++)
		for(int j=0; j<ncol; j++)
			v(i,j) = ::imag((*this).GetValue(i,j));
	return v;
}

CComplexMatrix conj(const CComplexMatrix &x)
{
	int nrow = x.mrow();
	int ncol = x.mcol();
	CComplexMatrix v(nrow, ncol);
	for(int i=0; i<nrow; i++)
		for(int j=0; j<ncol; j++)
			v(i,j) = conj(x.GetValue(i,j));
	return v;
}


CComplexMatrix CComplexMatrix ::conj()const 
{
	int nrow = mrow();
	int ncol = mcol();
	CComplexMatrix v(nrow, ncol);
	for(int i=0; i<nrow; i++)
		for(int j=0; j<ncol; j++)
			v(i,j) = ::conj((*this).GetValue(i,j));
	return v;
}

CComplexMatrix Complex(const CDoubleMatrix &v1, const CDoubleMatrix &v2)
{
	int len = v1.mrow();
	int ven = v1.mcol();
	CComplexMatrix v(len,ven);
	if((v2.mrow() != len) || (v2.mcol()!=ven))
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
		len = MIN(v1.mrow(),v2.mrow());
		ven = MIN(v1.mcol(),v2.mcol());
	}
    for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j) = CComplex(v1.GetValue(i,j), v2.GetValue(i,j));
	return v;
}

CComplexMatrix Complex(const CDoubleMatrix &v1, double x)
{
	int len = v1.mrow();
	int ven = v1.mcol();
	CComplexMatrix v(len,ven);

    for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j) = CComplex(v1.GetValue(i,j), x);
	return v;
}

CComplexMatrix Complex(double x, const CDoubleMatrix &v1)
{
	int len = v1.mrow();
	int ven = v1.mcol();
	CComplexMatrix v(len,ven);

    for(int i=0; i<len; i++)
		for(int j=0; j<ven; j++)
			v(i,j) = CComplex(x, v1.GetValue(i,j));
	return v;
}

CComplexMatrix CComplexMatrix::dot(const CComplexMatrix &m2)const 
{
	CComplexMatrix mx(mcol(),m2.mcol());
	for (int i=0;i<mcol();i++)
		for (int j=0;j<m2.mcol();j++)
			mx(i,j) = (*this).GetValue(i).dot(m2.GetValue(j));
	return mx;
}

CComplexMatrix dot(const CComplexMatrix &m1,const CComplexMatrix &m2)
{
	return m1.dot(m2);
}

CComplexVector CComplexMatrix::dot(const CComplexVector &v)const 
{
	CComplexVector mx(mcol());
	for (int i=0;i<mcol();i++)
		mx(i) = (*this).GetValue(i).dot(v);
	return mx;
}

CComplexVector dot(const CComplexMatrix &m,const CComplexVector &v)
{
	return m.dot(v);
}

CComplexVector dot(const CComplexVector &v, const CComplexMatrix &m)
{
	return m.dot(v);
}



CComplexMatrix trans(const CComplexMatrix &v1)
{
	int len = v1.mrow();
	int ven = v1.mcol();
	CComplexMatrix v(ven,len);
//{{{START 2003-1-16 Eric
	/*
	for(int i=0; i<ven; i++)
		for(int j=0;j<len; j++)
			v(i,j) = conj(v1(j,i));
	*/

	for(int i=0; i<ven; i++)
		for(int j=0; j<len; j++)
			v(i,j) = v1.GetValue(j,i);

//}}}END  2003-1-16 Eric
	return v;
}


CComplexMatrix CComplexMatrix::trans()const 
{
	int len = mrow();
	int ven = mcol();
	CComplexMatrix v(ven,len);

//{{{START 2003-1-16 Eric
	/*
	for(int i=0; i<ven; i++)
		for(int j=0;j<len; j++)
			v(i,j) = ::conj((*this)(j,i));
	*/
	v = ::trans(*this);

//}}}END  2003-1-16 Eric
	return v;
}

CDoubleMatrix abs(const CComplexMatrix &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CDoubleMatrix v(mrows, mcols);
	for(int i=0; i<mrows;i++)
		for(int j=0; j<mcols; j++)
			v(i,j) = abs(x.GetValue(i,j));
	return v;
}

CDoubleMatrix CComplexMatrix::abs()const 
{
	int mrows = mrow();
	int mcols = mcol();
	CDoubleMatrix v(mrows, mcols);
	for(int i=0; i<mrows;i++)
		for(int j=0; j<mcols; j++)
			v(i,j) = ::abs((*this).GetValue(i,j));
	return v;
}

CIntVector any(const CComplexMatrix &v)
{
	int m= v.mrow();
	int n= v.mcol();
    CIntVector x(m);
	for(int i=0; i<m; i++)
	{
		x(i)=0;
		for(int j=0;j<n;j++)
		{
			if(abs(v.GetValue(i,j))!=0)
			{
				x(i)=1;
				break;
			}
		}
	}
	return x;
}


CIntVector CComplexMatrix::any()const 
{
	int m= mrow();
	int n= mcol();
    CIntVector x(m);
	for(int i=0; i<m; i++)
	{
		x(i)=0;
		for(int j=0;j<n;j++)
		{
			if(::abs((*this).GetValue(i,j))!=0)
			{
				x(i)=1;
				break;
			}
		}
	}
	return x;
}

CComplexVector Max(const CComplexMatrix &v)
{
	int nr=v.mrow();
	int nc=v.mcol();
	CComplexVector xx(nr);
	for(int i=0; i<nr; i++)
	{
		xx(i)=v.GetValue(i,0);
		for(int j=1; j<nc; j++)
			if(abs(v.GetValue(i,j))>abs(xx(i)))
				xx(i)=v.GetValue(i,j);
	}
	return xx;
}

CComplexVector CComplexMatrix::Max()const 
{
	int nr=mrow();
	int nc=mcol();
	CComplexVector xx(nr);
	for(int i=0; i<nr; i++)
	{
		xx(i)=(*this).GetValue(i,0);
		for(int j=1; j<nc; j++)
			if(::abs((*this).GetValue(i,j))>::abs(xx(i)))
				xx(i)=(*this).GetValue(i,j);
	}
	return xx;
}

CComplexMatrix Max(const CComplexMatrix &v1, const CComplexMatrix &v2)
{
	int nr=v1.mrow();
	int nc=v1.mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__,__LINE__);
		nr = MIN(v1.mrow(), v2.mrow());
		nc = MIN(v1.mcol(), v2.mcol());
	}
	CComplexMatrix xx(nr, nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i,j)=v1.GetValue(i,j);
			if(abs(v2.GetValue(i,j))>abs(v1.GetValue(i,j)))
				xx(i,j)=v2.GetValue(i,j);
		}
	return xx;
}

CComplexMatrix CComplexMatrix::Max( const CComplexMatrix &v2)const 
{
	int nr=mrow();
	int nc=mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__,__LINE__);
		nr = MIN(mrow(), v2.mrow());
		nc = MIN(mcol(), v2.mcol());
	}
	CComplexMatrix xx(nr, nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i,j)=(*this).GetValue(i,j);
			if(::abs(v2.GetValue(i,j))>::abs((*this).GetValue(i,j)))
				xx(i)(j)=v2.GetValue(i,j);
		}
	return xx;
}

CComplexVector Min(const CComplexMatrix &v)
{
	int nr=v.mrow();
	int nc=v.mcol();
	CComplexVector xx(nr);
	for(int i=0; i<nr; i++)
	{
		xx(i)=v.GetValue(i,0);
		for(int j=1; j<nc; j++)
			if(abs(v.GetValue(i,j))<abs(xx(i)))
				xx(i)=v.GetValue(i,j);
	}
	return xx;
}

CComplexVector CComplexMatrix::Min()const 
{
	int nr=mrow();
	int nc=mcol();
	CComplexVector xx(nr);
	for(int i=0; i<nr; i++)
	{
		xx(i)=(*this).GetValue(i,0);
		for(int j=1; j<nc; j++)
		{
			if((double)::abs((*this).GetValue(i,j))<(double)::abs(xx(i)))
				xx(i)=(*this).GetValue(i,j);
		}
	}
	return xx;
}

CComplexMatrix Min(const CComplexMatrix &v1, const CComplexMatrix &v2)
{
	int nr=v1.mrow();
	int nc=v1.mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__,__LINE__);
		nr = MIN(v1.mrow(), v2.mrow());
		nc = MIN(v1.mcol(), v2.mcol());
	}
	CComplexMatrix xx(nr, nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i,j)=v1.GetValue(i,j);
			if(abs(v2.GetValue(i,j))<abs(v1.GetValue(i,j)))
				xx(i,j)=v2.GetValue(i,j);
		}
	return xx;
}

CComplexMatrix CComplexMatrix::Min( const CComplexMatrix &v2)const 
{
	int nr=mrow();
	int nc=mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__,__LINE__);
		nr = MIN(mrow(), v2.mrow());
		nc = MIN(mcol(), v2.mcol());
	}
	CComplexMatrix xx(nr, nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i,j)=(*this).GetValue(i,j);
			if((double)::abs(v2.GetValue(i,j)) < (double)::abs((*this).GetValue(i,j)))
				xx(i,j)=v2.GetValue(i,j);
		}
	return xx;
}

CComplexMatrix & Rand(CComplexMatrix &v)
{
	int nr=v.mrow();
	int nc=v.mcol();
	for(int i=0; i<nr; i++)
	{
		for(int j=0; j<nc; j++)
			v(i,j) = CComplex((double)::rand()/(double)RAND_MAX,
			           (double)::rand()/(double)RAND_MAX);

	}
	return v;
}

CComplexMatrix & CComplexMatrix::Rand()
{
	int nr=mrow();
	int nc=mcol();
	for(int i=0; i<nr; i++)
	{
		for(int j=0; j<nc; j++)
			(*this)(i,j) = CComplex((double)::rand()/(double)RAND_MAX,
			           (double)::rand()/(double)RAND_MAX);

	}
	return (*this);
}

CComplexMatrix CComplexMatrix::Rand(int m, int n)const 
{
	CComplexMatrix v(m,n);
	for(int i=0; i<m; i++)
	{
		for(int j=0; j<n; j++)
			v(i,j) = CComplex((double)::rand()/(double)RAND_MAX,
			           (double)::rand()/(double)RAND_MAX);

	}
	return v;
}

CComplexMatrix inv(const CComplexMatrix &mm)
{
	int numcol = mm.mrow();
	if(mm.mcol()!=numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        numcol = MIN(mm.mrow(),mm.mcol());
	}
	int nn=2*numcol;
	CComplexMatrix work(numcol, numcol);
	CDoubleMatrix coef(nn,nn);
	int i,j,ne,c1,c2,c11;
	double de;
	long chkmax;
	chkmax = (long)numcol*numcol*4;
	if(chkmax*sizeof(double)>64000l)
	{
		for(i=0; i<numcol; ++i)
		{
			for(j=0; j<numcol; ++j)
			{
				c1 = i*2;
				c2 = j*2;
				coef(c1,c2)=real(mm.GetValue(i,j));
				coef(c1,c2+1)=-imag(mm.GetValue(i,j));
				coef(c1+1,c2)=imag(mm.GetValue(i,j));
				coef(c1+1,c2+1)=real(mm.GetValue(i,j));
			}
		}
		ne = Matinv(coef,nn);
	}
	else
	{
		CDoubleVector coef1(4*numcol*numcol);
		for(i=0; i<numcol; ++i)
		{
			c1 = (i*2)*2*numcol;
			c11 = (i*2+1)*2*numcol;
			for(j=0; j<numcol; ++j)
			{
				c2 = j*2;
				coef1[c1+c2]=real(mm.GetValue(i,j));
				coef1[c1+c2+1]=-imag(mm.GetValue(i,j));
				coef1[c11+c2] = imag(mm.GetValue(i,j));
				coef1[c11+c2+1]=real(mm.GetValue(i,j));
			}

		}
		ne = Matinv(coef1,nn);       
		coef = coef1;
	}
    if((ne==EMAT_SINGULAR)||(ne==WEMAT_SINGULAR))
	{
		for(i=0; i<numcol; ++i)
		{
			for(j=0; j<numcol; ++j)
			{
				c1 = i*2;
				c2 = j*2;
				coef(c1,c2)=real(mm.GetValue(i,j));
				coef(c1,c2+1)=-imag(mm.GetValue(i,j));
				coef(c1+1,c2)=imag(mm.GetValue(i,j));
				coef(c1+1,c2+1)=real(mm.GetValue(i,j));
			}
		}
		ne = Matinv1(coef,nn,de);
	}
	if(ne!=0)
	{
		if(ne>0)
			ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
		else
			WarningHandler((Matvec_warnings)nn, __FILE__, __LINE__);
	}
	for(i=0; i<numcol; ++i)
	{
		for(j=0; j<numcol; ++j)
		{
			c1 = i*2; 
			c2 = j*2;
			work(i,j) = CComplex(coef(c1,c2), coef(c1+1,c2));
		}
	}
	return work;
}

CComplexMatrix CComplexMatrix::inv()const 
{
	return ::inv(*this);
}

CComplexMatrix inv1(const CComplexMatrix &mm)
{
	int numcol = mm.mrow();
	if(mm.mcol()!=numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        numcol = MIN(mm.mrow(),mm.mcol());
	}
	int nn=2*numcol;
	CComplexMatrix work(numcol, numcol);
	CDoubleMatrix coef(nn,nn);
	int i,j,ne,c1,c2;
	double de;

	for(i=0; i<numcol; ++i)
	{
		for(j=0; j<numcol; ++j)
		{
			c1 = i*2;
			c2 = j*2;
			coef(c1,c2)=real(mm.GetValue(i,j));
			coef(c1,c2+1)=-imag(mm.GetValue(i,j));
			coef(c1+1,c2)=imag(mm.GetValue(i,j));
			coef(c1+1,c2+1)=real(mm.GetValue(i,j));
		}
	}
	ne = Matinv1(coef,nn,de);
	if(ne!=0)
	{
		if(ne>0)
			ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
		else
			WarningHandler((Matvec_warnings)nn, __FILE__, __LINE__);
	}
	for(i=0; i<numcol; ++i)
	{
		for(j=0; j<numcol; ++j)
		{
			c1 = i*2; 
			c2 = j*2;
			work(i,j) = CComplex(coef(c1,c2), coef(c1+1,c2));
		}
	}
	return work;
}

CComplexMatrix CComplexMatrix::inv1()const 
{
	return ::inv1(*this);
}

CComplexMatrix expm2(const CComplexMatrix &x)
{
    int mn= x.mrow();
	if(mn != x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(x.mrow(), x.mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(x,v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::exp(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix CComplexMatrix::expm2()const 
{
    int mn= mrow();
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::exp(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix logm2(const CComplexMatrix &x)
{
    int mn= x.mrow();
	if(mn != x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(x.mrow(), x.mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(x,v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::log(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix CComplexMatrix::logm2()const 
{
    int mn= mrow();
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::log(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix sqrtm2(const CComplexMatrix &x)
{
    int mn= x.mrow();
	if(mn != x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(x.mrow(), x.mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(x,v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::sqrt(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix CComplexMatrix::sqrtm2()const 
{
    int mn= mrow();
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::sqrt(d(i,i));
	e = v*tmp*(::inv(v));
	return e;
}

CComplexMatrix CComplexMatrix::powm2(const CComplex &c)const 
{
    int mn= mrow();
	if(mn != mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(mrow(), mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::pow(d(i,i),c);
	e = v*tmp*(::inv(v));
	return e;

}

CComplexMatrix powm2(const CComplexMatrix &x, const CComplex &c)
{
    int mn= x.mrow();
	if(mn != x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mn = MIN(x.mrow(), x.mcol());
	}
	CComplexMatrix v(mn,mn),d(mn,mn),tmp(mn,mn),e(mn,mn);
	d = eig(x,v);
	for(int i=0; i<mn; i++)
		tmp(i,i) =::pow(d(i,i),c);
	e = v*tmp*(::inv(v));
	return e;
}

CComplexVector eig(const CComplexMatrix &x)
{
	int n=x.mrow();
	if(x.mcol()!=n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__,__LINE__);
        n = MIN(x.mrow(),x.mcol());
	}

	int nn=2*n;
	int i,j;

    CComplexVector ee1(nn);
    CComplexVector ee(n);
	CDoubleMatrix h1(nn,nn), v1(nn,nn);
	CComplexMatrix s1(nn,nn);
	CComplexMatrix EE(nn,nn);
	h1= ((real(x))|(-imag(x)))&((imag(x))|(real(x)));
	CComplex I(0,1);
	int k=0;
     
	EE = eig(h1,s1);
	ee1 = diag(EE);

	if(h1 + 1.0 !=1.0)
	{
		while(k<n)
		{
			for(j=0; j<nn; j++)
			{
				for(i=0; i<n; i++)
				{
					if(abs(s1(i,j)+I*s1(i+n,j))>1e-10)
					{   
						break;
					}
				}
				if(i!=n)
				{
					ee(k)=ee1(j);
		    		k++;
				}
				
			}
		}
	}
	return ee;	
}

CComplexVector CComplexMatrix::eig()const 
{
	int n=mrow();
	if(mcol()!=n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__,__LINE__);
        n = MIN(mrow(),mcol());
	}

	int nn=2*n;
	int i,j;

    CComplexVector ee1(nn);
    CComplexVector ee(n);
	CDoubleMatrix h1(nn,nn), v1(nn,nn);
	CComplexMatrix s1(nn,nn);
	CComplexMatrix em1(nn,nn);
	h1= ((real())|(-imag()))&((imag())|(real()));
	CComplex I(0,1);
	int k=0;     
	em1 = ::eig(h1,s1);
	ee1 = ::diag(em1);
	if(h1 + 1.0 !=1.0)
	{
		while(k<n)
		{
			for(j=0; j<nn; j++)
			{
				for(i=0; i<n; i++)
				{
					if(::abs(s1(i,j)+I*s1(i+n,j))>1e-10)
					{   
						break;
					}
				}
				if(i!=n)
				{
					ee(k)=ee1(j);
		    		k++;
				}
				
			}
		}
	}
	return ee;	
}

CComplexMatrix CComplexMatrix::eig(CComplexMatrix &s)const 
{
	int n=mrow();
	if(mcol()!=n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__,__LINE__);
        n = MIN(mrow(),mcol());
	}

	int nn=2*n;
	int i,j,k,l;

    CComplexVector ee1(nn);
    CComplexVector ee(n);
	CComplexMatrix em(n,n);
	CDoubleMatrix h1(nn,nn), v1(nn,nn);
	CComplexMatrix s1(nn,nn);
	CComplexMatrix em1(nn,nn);
	h1= ((real())|(-imag()))&((imag())|(real()));
	CComplex I(0,1);

    if(imag()+1.0 == 1.0)
		em = ::eig(real(),s);
	else if(real()+1.0 == 1.0)
		em = ::eig(imag(),s)*I;
    else
	{
		em1 = ::eig(h1,s1);
		ee1 = ::diag(em1);
		k=0;
		while(k<n)
		{
			for(j=0; j<nn; j++)
			{
				for(i=0; i<n; i++)
				{
					if(::abs(s1(i,j)+I*s1(i+n,j))>1e-10)
					{   
						break;
					}
				}
				if(i!=n && k<n)
				{
					for(l=0; l<n; l++)
						s(l,k)= s1(l,j)+I*s1(l+n,j);					
					ee(k)=ee1(j);
		    		k++;
				}
				
			}
		}
    	em = ::diag(ee);
	}
	return em;
}



CComplexMatrix eig(const CComplexMatrix &x, CComplexMatrix &s)
{
	return x.eig(s);
}

/*
CDoubleVector svd(CComplexMatrix &x)
{
	int i;
	int m = x.mrow();
	int n = x.mcol();
	int mm = 2*m;
	int nn = 2*n;
	int minmn = MIN(m,n);
	CDoubleVector aa(minmn);
	CDoubleMatrix a(mm,nn), u(mm,mm), v(nn,nn);
	a = (real(x)|-imag(x))&(imag(x)|real(x));
	double eps = TOLERANT_ERROR;
	int state = dluav(a,u,v,mm,nn,eps);
	if(state !=0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	for(i = 0; i<minmn; i++)
		aa(i) = a(i*2,i*2);
	return aa;
}
*/
/*
CDoubleVector CComplexMatrix::svd()
{
	int i;
	int m = mrow();
	int n = mcol();
	int mm = 2*m;
	int nn = 2*n;
	int minmn = MIN(m,n);
	CDoubleVector aa(minmn);
	CDoubleMatrix a(mm,nn), u(mm,mm), v(nn,nn);
	a = (real()|-imag())&(imag()|real());
	double eps = TOLERANT_ERROR;
	int state = dluav(a,u,v,mm,nn,eps);
	if(state !=0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	for(i = 0; i<minmn; i++)
		aa(i) = a(i*2,i*2);
	return aa;
}

CDoubleMatrix svd(CComplexMatrix &x, CComplexMatrix &y, CComplexMatrix &z)
{
	int i,j,flag;
    int m=x.mrow();
	int n=x.mcol();
	int mm = 2*m;
	int nn = 2*n;
	int minmn = MIN(m,n);

	CDoubleMatrix yy(mm,mm);
	CDoubleMatrix zz(nn,nn);
	CComplexMatrix a(m,n);
	CDoubleMatrix aa(mm,nn);
	if((y.mrow()!=m)||(y.mcol()!=m))
	{
		CComplexMatrix tmp1(m,m);
		y = tmp1;
	}
	if((z.mrow()!=n)||(z.mcol()!=n))
	{
		CComplexMatrix tmp2(n,n);
		z = tmp2;
	}
	if(imag(x)+1.0 == 1.0)
	{
		CDoubleMatrix y1(m,m), z1(n,n);
		a=::svd(real(x),y1,z1);
		y = Complex(y1,0.0);
		z = Complex(z1,0.0);
		return a;
	}
    else
	{
		aa = (real(x)|-imag(x))&(imag(x)|real(x));
		
		double eps = TOLERANT_ERROR;
		int state = dluav(aa,yy,zz,mm,nn,eps);
		if(state != 0)
		{
			ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
		}
		//WriteData(yy,"yy");
		//WriteData(zz,"zz");
		//WriteData(aa,"aa");

		for(j=0; j<m; j++)
		{
			flag = 0;
			for(i=0; i<m; i++)
				if(fabs(yy(i,j*2)-yy(i+m,j*2+1))>1e-10 || fabs(yy(i,j*2+1)+yy(i+m,j*2))>1e-10)
					break;
			if(i>=m)
				flag = 1;
			for(i=0; i<m; i++)
			{
				if(flag)

					y(i,j) = CComplex(yy(i,j*2),-yy(i,j*2+1));
				else
					y(i,j) = CComplex(yy(i,j*2+1),-yy(i,j*2));
			}
		}
		for(i=0; i<n; i++)
		{
			flag =0;
			for(j=0; j<n; j++)
				if(fabs(zz(i*2,j)-zz(i*2+1,j+n))>1e-10 || fabs(zz(i*2+1,j)+zz(i*2,j+n))>1e-10)
					break;
			if(j>=n)
				flag=1;
			for(j=0; j<n; j++)
			{
				if(flag)
					z(i,j) = CComplex(zz(i*2,j),zz(i*2+1,j));
				else
					z(i,j) = CComplex(zz(i*2+1,j),zz(i*2,j));
			}
		}
		for(i=0; i<minmn; i++)
			a(i,i)=aa(i*2,i*2);
	}
	return a;
}


CDoubleMatrix CComplexMatrix::svd(CComplexMatrix &y, CComplexMatrix &z)
{
	int i,j,flag;
    int m=mrow();
	int n=mcol();
	int mm = 2*m;
	int nn = 2*n;
	int minmn = MIN(m,n);

	CDoubleMatrix yy(mm,mm);
	CDoubleMatrix zz(nn,nn);
	CComplexMatrix a(m,n);
	CDoubleMatrix aa(mm,nn);
	if((y.mrow()!=m)||(y.mcol()!=m))
	{
		CComplexMatrix tmp1(m,m);
		y = tmp1;
	}
	if((z.mrow()!=n)||(z.mcol()!=n))
	{
		CComplexMatrix tmp2(n,n);
		z = tmp2;
	}
	if(imag()+1.0 == 1.0)
	{
		CDoubleMatrix y1(m,m), z1(n,n);
		a=::svd(real(),y1,z1);
		y = Complex(y1,0.0);
		z = Complex(z1,0.0);
		return a;
	}
    else
	{
		aa = (real()|-imag())&(imag()|real());
		
		double eps = TOLERANT_ERROR;
		int state = dluav(aa,yy,zz,mm,nn,eps);
		if(state != 0)
		{
			ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
		}
		for(j=0; j<m; j++)
		{
			flag = 0;
			for(i=0; i<m; i++)
				if(fabs(yy(i,j*2)-yy(i+m,j*2+1))>1e-10 || fabs(yy(i,j*2+1)+yy(i+m,j*2))>1e-10)
					break;
			if(i>=m)
				flag = 1;
			for(i=0; i<m; i++)
			{
				if(flag)

					y(i,j) = CComplex(-yy(i,j*2),-yy(i,j*2+1));
				else
					y(i,j) = CComplex(-yy(i,j*2+1),-yy(i,j*2));
			}
		}
		for(i=0; i<n; i++)
		{
			flag =0;
			for(j=0; j<n; j++)
				if(fabs(zz(i*2,j)-zz(i*2+1,j+n))>1e-10 || fabs(zz(i*2+1,j)+zz(i*2,j+n))>1e-10)
					break;
			if(j>=n)
				flag=1;
			for(j=0; j<n; j++)
			{
				if(flag)
					z(i,j) = CComplex(-zz(i*2,j),zz(i*2+1,j));
				else
					z(i,j) = CComplex(-zz(i*2+1,j),zz(i*2,j));
			}
		}
		for(i=0; i<minmn; i++)
			a(i,i)=aa(i*2,i*2);
	}
	return a;
}

*/

//////////////////////////////////////////////////////////////////////
// CTriangleDbMatrix --------- Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTriangleDbMatrix::CTriangleDbMatrix(const CDoubleMatrix &x)
{
    this->CTriangleMatrix<double>::operator =(x);
}

CTriangleDbMatrix & CTriangleDbMatrix::operator =(const CTriangleDbMatrix &x)
{
    this->CTriangleMatrix<double>::operator = (x);
	return * this;
}

CTriangleDbMatrix & CTriangleDbMatrix::operator =(const CTriangleMatrix<double> &x)
{
    this->CTriangleMatrix<double>::operator = (x);
	return * this;
}

CTriangleDbMatrix & CTriangleDbMatrix::operator =(const CDoubleMatrix &x)
{
    this->CTriangleMatrix<double>::operator = (x);
	return * this;

}

CTriangleDbMatrix & CTriangleDbMatrix::operator =(const CVector<double> &x)
{
    this->CTriangleMatrix<double>::operator = (x);
	return * this;

}

CTriangleDbMatrix & CTriangleDbMatrix::operator =(const CDoubleVector &x)
{
    this->CTriangleMatrix<double>::operator = (x);
	return * this;

}

CTriangleDbMatrix & CTriangleDbMatrix::operator =(double **x)
{
    this->CTriangleMatrix<double>::operator = (x);
	return * this;

}

CTriangleDbMatrix & CTriangleDbMatrix::operator =(double *x)
{
    this->CTriangleMatrix<double>::operator = (x);
	return * this;

}

CTriangleDbMatrix & CTriangleDbMatrix::operator =(double x)
{
    this->CTriangleMatrix<double>::operator = (x);
	return * this;
}


//////////////////////////////////////////////////////////////////////
// CSymMatrix -------- Construction/Destruction
//////////////////////////////////////////////////////////////////////
  

int vlentorow(int vlen)
{
    int len = (int) sqrt(8*vlen+1);
	len = (len-1)/2;
	return len;
}





//////////////////////////////////////////////////////////////////////
// CSymDbMatrix --------- Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSymDbMatrix::CSymDbMatrix(const CDoubleMatrix &x)
{
    this->CSymMatrix<double>::operator =(x);
}

CSymDbMatrix & CSymDbMatrix::operator =(const CSymDbMatrix &x)
{
    this->CSymMatrix<double>::operator = (x);
	return * this;
}

CSymDbMatrix & CSymDbMatrix::operator =(const CSymMatrix<double> &x)
{
    this->CSymMatrix<double>::operator = (x);
	return * this;
}

CSymDbMatrix & CSymDbMatrix::operator =(const CDoubleMatrix &x)
{
    this->CSymMatrix<double>::operator = (x);
	return * this;

}

CSymDbMatrix & CSymDbMatrix::operator =(const CVector<double> &x)
{
    this->CSymMatrix<double>::operator = (x);
	return * this;

}

CSymDbMatrix & CSymDbMatrix::operator =(const CDoubleVector &x)
{
    this->CSymMatrix<double>::operator = (x);
	return * this;

}

CSymDbMatrix & CSymDbMatrix::operator =(const CIntMatrix &x)
{
	CDoubleMatrix m;
	m=x;
    this->CSymMatrix<double>::operator = (m);
	return * this;

}

CSymDbMatrix & CSymDbMatrix::operator =(const CFloatMatrix &x)
{
	CDoubleMatrix m;
	m=x;
    this->CSymMatrix<double>::operator = (m);
	return * this;
}

CSymDbMatrix & CSymDbMatrix::operator =(double **x)
{
    this->CSymMatrix<double>::operator = (x);
	return * this;

}

CSymDbMatrix & CSymDbMatrix::operator =(double *x)
{
    this->CSymMatrix<double>::operator = (x);
	return * this;

}

CSymDbMatrix & CSymDbMatrix::operator =(double x)
{
    this->CSymMatrix<double>::operator = (x);
	return * this;
}


CDoubleVector CSymDbMatrix::eig()const 
{
	//double eps = TOLERANT_ERROR;
	double eps = 1e-100;
	CSymDbMatrix work;
	int n=mcol();
    work =copy();
    CMatrix<double> v=eye(n);
    int state=cjcbi(work,n,v,eps);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
    return ::diag(work);    
}

CDoubleVector eig(const CSymDbMatrix &m)
{
	//double eps = TOLERANT_ERROR;
	double eps = 1e-100;
	CSymDbMatrix work;
	int n=m.mcol();
    work =m.copy();
    CMatrix<double> v=eye(n);
    int state=cjcbi(work,n,v,eps);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
    return diag(work);    
}

CSymDbMatrix CSymDbMatrix::eig(CDoubleMatrix &v)const 
{
	//double eps = TOLERANT_ERROR;
	double eps = 1e-100;
	CSymDbMatrix work;
	int n=mcol();
    work =copy();
	v = eye(n);
    int state=cjcbi(work,n,v,eps);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
    return work;    
}

CSymDbMatrix eig(const CSymDbMatrix &m, CDoubleMatrix &v)
{
	//double eps = TOLERANT_ERROR;
	double eps = 1e-100;
	CSymDbMatrix work;
	int n=m.mcol();
    work = m.copy();
    v = eye(n);
    int state=cjcbi(work,n,v,eps);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
    return work;    
}

CTriangleDbMatrix CSymDbMatrix::chol()const 
{
	CSymDbMatrix work;
	CTriangleDbMatrix trm;
	int n=mcol();
    work =copy();
	double det;
    int state=bchol(work,trm, det);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
    return trm;    
}

CTriangleDbMatrix chol(const CSymDbMatrix &m)
{
	CSymDbMatrix work;
	CTriangleDbMatrix trm;
	int n=m.mcol();
    work =m.copy();
	double det;
    int state=bchol(work,trm, det);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
    return trm;    
}

CTriangleDbMatrix CSymDbMatrix::chol(double &det)const 
{
	CSymDbMatrix work;
	CTriangleDbMatrix trm;
	int n=mcol();
    work =copy();
    int state=bchol(work,trm, det);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
    return trm;    
}

CTriangleDbMatrix chol(const CSymDbMatrix &m, double &det)
{
	CSymDbMatrix work;
	CTriangleDbMatrix trm;
	int n=m.mcol();
    work =m.copy();
    int state=bchol(work,trm, det);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
    return trm;    
}


CTriangleDbMatrix chol1(const CSymDbMatrix &v) //zfm
{
	int flag;
	int mm=v.mrow();
	if(mm!=v.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		mm = MIN(mm, v.mcol());
	}
	CSymDbMatrix work;
	CTriangleDbMatrix output(mm,mm);
	work=v.copy();
	flag=cholesky(work,output);
	if(flag!=0)
	{
		ErrorHandler((Matvec_errors)flag, __FILE__, __LINE__);
	}
	return output;	
}


CSymDbMatrix cov(const CDoubleMatrix &v)
{
	int m = v.mrow();
	CSymDbMatrix s(m);
	for(int i=0; i<m; i++)
		for(int j=i; j<m; j++)
		{
//			s(i,j)=cov((CDoubleVector)v(i),(CDoubleVector)v(j));
			s(i,j)=cov((CDoubleVector)v.gGetVector(i),(CDoubleVector)v.gGetVector(j));
		}
	return s;
}


CSymDbMatrix corrcoef(const CDoubleMatrix &v)
{
    int m = v.mrow();
	CSymDbMatrix s(m);
	s=eye(m);
	for(int i=0; i<m; i++)
		for(int j=i+1; j<m; j++)
		{
//			s(i,j)=corrcoef((CDoubleVector)v(i),(CDoubleVector)v(j));
			s(i,j)=corrcoef((CDoubleVector)v.gGetVector(i),(CDoubleVector)v.gGetVector(j));
		}
	return s;
}

CSymDbMatrix ismattodsmat(const CSymMatrix<int> &v1)
{
	CSymDbMatrix v;
	v = ivectodvec((CIntVector)v1);
    return v;
}

CSymFtMatrix ismattofsmat(const CSymMatrix<int> &v1)
{
	CSymFtMatrix v;
	v = ivectofvec((CIntVector)v1);
    return v;
}

CSymDbMatrix fsmattodsmat(const CSymMatrix<float> &v1)
{
	CSymDbMatrix v;
	v = fvectodvec(v1);
    return v;
}






//////////////////////////////////////////////////////////////////////
// CSymCompMatrix ------ Construction/Destruction
//////////////////////////////////////////////////////////////////////
/* ===temp
CSymCompMatrix::CSymCompMatrix(const CComplexMatrix &x)
{
    this->CSymMatrix<CComplex>::operator =(x);
}

CSymCompMatrix & CSymCompMatrix::operator =(const CSymCompMatrix &x)
{
    this->CSymMatrix<CComplex>::operator = (x);
	return * this;
}

CSymCompMatrix & CSymCompMatrix::operator =(const CSymMatrix<CComplex> &x)
{
    this->CSymMatrix<CComplex>::operator = (x);
	return * this;
}

CSymCompMatrix & CSymCompMatrix::operator =(const CComplexMatrix &x)
{
    this->CSymMatrix<CComplex>::operator = (x);
	return * this;

}

CSymCompMatrix & CSymCompMatrix::operator =(const CVector<CComplex> &x)
{
    this->CSymMatrix<CComplex>::operator = (x);
	return * this;

}

CSymCompMatrix & CSymCompMatrix::operator =(const CComplexVector &x)
{
    this->CSymMatrix<CComplex>::operator = (x);
	return * this;

}


CSymCompMatrix & CSymCompMatrix::operator =(CComplex **x)
{
    this->CSymMatrix<CComplex>::operator = (x);
	return * this;

}

CSymCompMatrix & CSymCompMatrix::operator =(CComplex *x)
{
    this->CSymMatrix<CComplex>::operator = (x);
	return * this;

}

CSymCompMatrix & CSymCompMatrix::operator =(CComplex x)
{
    this->CSymMatrix<CComplex>::operator = (x);
	return * this;
}




CSymCompMatrix ismattocsmat(const CSymMatrix<int> &v1)
{
	CSymCompMatrix v;
	v = ivectocvec((CIntVector)v1);
    return v;
}

CSymCompMatrix fsmattocsmat(const CSymMatrix<float> &v1)
{
	CSymCompMatrix v;
	v = fvectocvec(v1);
    return v;
}

CSymCompMatrix dsmattocsmat(const CSymMatrix<double> &v1)
{
	CSymCompMatrix v;
	v = dvectocvec((CDoubleVector)v1);
    return v;
}
//*/
//////////////////////////////////////////////////////////////////////
// CSparseMatrix -------- Construction/Destruction
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// CSparseDbMatrix -------- Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSparseDbMatrix & CSparseDbMatrix::operator =(const CSparseDbMatrix &x)
{
    this->CSparseMatrix<double>::operator = (x);
	return * this;
}

CSparseDbMatrix & CSparseDbMatrix::operator =(const CSparseMatrix<double> &x)
{
    this->CSparseMatrix<double>::operator = (x);
	return * this;
}

CSparseDbMatrix & CSparseDbMatrix::operator =(const CDoubleMatrix &x)
{
    this->CSparseMatrix<double>::operator = (x);
	return * this;

}

CSparseDbMatrix & CSparseDbMatrix::operator =(double **x)
{
    this->CSparseMatrix<double>::operator = (x);
	return * this;

}

CSparseDbMatrix & CSparseDbMatrix::operator =(double *x)
{
    this->CSparseMatrix<double>::operator = (x);
	return * this;

}

CSparseDbMatrix & CSparseDbMatrix::operator =(double x)
{
    this->CSparseMatrix<double>::operator = (x);
	return * this;
}

void CSparseDbMatrix::compress()
{
    for(int i=0; i<vlen(); i++)
	{
		if(fabs((*this)(i).value())<DOUBLE_ZERO)
		{
            remove(i);
		}
	}
}

double dot(const CSparseDbMatrix &tr1, const CSparseDbMatrix &tr2)
{
	double s=0.0;
	int jj=0;
	int kk=0;
	while(jj<tr1.vlen() && kk<tr2.vlen())
	{
		if(tr1.GetValue(jj).gypos()==tr2.GetValue(kk).gypos())
		{
			s += tr1.GetValue(jj).gvalue()*tr2.GetValue(kk).gvalue();
			kk++;
			jj++;
		}
		else if(tr2.GetValue(kk).gypos()>tr1.GetValue(jj).gypos())
		{
			jj++;
		}
		else 
		{
			kk++;
		}
	} 	
	return s;				
}

CSparseDbMatrix CSparseDbMatrix::qr(CSparseDbMatrix &q, int flag)
{
	int nrows = mrow();
	int ncols = mcol();
	int nrows1;
	if(nrows<ncols)
		nrows1=ncols;
	else
		nrows1=nrows;

    if(q.vlen()!=nrows1)
	{
		CSparseDbMatrix q1(nrows1,nrows1);
		q1.p->elem = (CAllocate<CTriple<double> > *)new CAllocate<CTriple<double> >(nrows1);
		q1.p->length = nrows1;
		q = q1;		
	}
	CSparseDbMatrix r = copy();
    r.rows = nrows1;
	int state =	sphouseqr(r,q);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows>=ncols)
	{
        CSparseDbMatrix rr(r, 0, ncols-1, 0, ncols-1);
		CSparseDbMatrix qq(q, 0, nrows-1, 0, ncols-1);
		r = rr;
		q = qq;
	}
	else if(nrows < ncols)
	{
		CSparseDbMatrix qq(q, 0, nrows-1, 0, nrows-1);
		r.rows=nrows;
		q = qq;
	}
	return r;
}

CSparseDbMatrix qr(CSparseDbMatrix &x, CSparseDbMatrix &q, int flag)
{
	int nrows = x.mrow();
	int ncols = x.mcol();
	int nrows1;
	if(nrows<ncols)
		nrows1=ncols;
	else
		nrows1=nrows;

    if(q.vlen()!=nrows1)
	{
		CSparseDbMatrix q1(nrows1,nrows1);
		q1.p->elem = (CAllocate<CTriple<double> > *)new CAllocate<CTriple<double> >(nrows1);
		q1.p->length = nrows1;
		q = q1;		
	}
	CSparseDbMatrix r = x.copy();
    r.rows = nrows1;
	int state =	sphouseqr(r,q);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows>=ncols)
	{
        CSparseDbMatrix rr(r, 0, ncols-1, 0, ncols-1);
		CSparseDbMatrix qq(q, 0, nrows-1, 0, ncols-1);
		r = rr;
		q = qq;
	}
	else if(nrows < ncols)
	{
		CSparseDbMatrix qq(q, 0, nrows-1, 0, nrows-1);
		r.rows=nrows;
		q = qq;
	}
	return r;
}

CSparseDbMatrix CSparseDbMatrix::qr(CSparseDbMatrix &q, CIntVector &e, int flag)
{
	int nrows = mrow();
	int ncols = mcol();
	int nrows1;
	if(nrows<ncols)
		nrows1=ncols;
	else
		nrows1=nrows;
	if(q.vlen()!=nrows1)
	{
		CSparseDbMatrix q1(nrows1,nrows1);
		q1.p->elem = (CAllocate<CTriple<double> > *)new CAllocate<CTriple<double> >(nrows1);
		q1.p->length = nrows1;
		q = q1;		
	}
	CSparseDbMatrix r = copy();
    r.rows = nrows1;
	if (e.vlen()!=ncols)
	{
		CIntVector vec(ncols);
		e = vec;
	}
    int state =	sphouseqr(r, q, e);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows >= ncols)
	{
        CSparseDbMatrix rr(r, 0, ncols-1, 0, ncols-1);
		CSparseDbMatrix qq(q, 0, nrows-1, 0, ncols-1);
		r = rr;
		q = qq;
	}
	else if(nrows < ncols)
	{
		CSparseDbMatrix qq(q, 0, nrows-1, 0, nrows-1);
		r.rows=nrows;
		q = qq;
	}
	return r;
}

CSparseDbMatrix qr(CSparseDbMatrix &x, CSparseDbMatrix &q, CIntVector &e, int flag)
{
	int nrows = x.mrow();
	int ncols = x.mcol();
	int nrows1;
	if(nrows<ncols)
		nrows1=ncols;
	else
		nrows1=nrows;
	if(q.vlen()!=nrows1)
	{
		CSparseDbMatrix q1(nrows1,nrows1);
		q1.p->elem = (CAllocate<CTriple<double> > *)new CAllocate<CTriple<double> >(nrows1);
		q1.p->length = nrows1;
		q = q1;		
	}
	CSparseDbMatrix r = x.copy();
    r.rows = nrows1;
	if (e.vlen()!=ncols)
	{
		CIntVector vec(ncols);
		e = vec;
	}
    int state =	sphouseqr(r, q, e);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}
	if(flag==0 && nrows >= ncols)
	{
        CSparseDbMatrix rr(r, 0, ncols-1, 0, ncols-1);
		CSparseDbMatrix qq(q, 0, nrows-1, 0, ncols-1);
		r = rr;
		q = qq;
	}
	else if(nrows < ncols)
	{
		CSparseDbMatrix qq(q, 0, nrows-1, 0, nrows-1);
		r.rows=nrows;
		q = qq;
	}
	return r;
}


CSparseDbMatrix inv(const CSparseDbMatrix &mm)
{
	int numcol = mm.mrow();
	if(mm.mcol()!=numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        numcol = MIN(mm.mrow(),mm.mcol());
	}
	CSparseDbMatrix work(mm,0,numcol-1,0,numcol-1);

	int nn;
	double det;
	nn = spgassinv(work,det);
	if(nn!=0)
	{
		if(nn>0)
			ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
		else
			WarningHandler((Matvec_warnings)nn, __FILE__, __LINE__);
	}
	return work;
}

CSparseDbMatrix CSparseDbMatrix::inv()const 
{
	int numcol = mrow();
	if(mcol()!=numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        numcol = MIN(mrow(),mcol());
	}
	CSparseDbMatrix work((*this),0,numcol-1,0,numcol-1);

	int nn;
	double det;
	nn = spgassinv(work,det);
	if(nn!=0)
	{
		if(nn>0)
			ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
		else
			WarningHandler((Matvec_warnings)nn, __FILE__, __LINE__);
	}
	return work;
}


double det(const CSparseDbMatrix &mm)
{
    int numcol = mm.mrow();
	if(mm.mcol() != numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		numcol = MIN(mm.mrow(), mm.mcol());
	}
	CSparseDbMatrix work(mm,0,numcol-1,0,numcol-1);
	int nn;
	double det;
	nn = spdet(work,det);
	if(nn != 0)
	{
		ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
	}
	return det;		
}

double CSparseDbMatrix::det()const 
{
    int numcol = mrow();
	if(mcol() != numcol)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		numcol = MIN(mrow(), mcol());
	}
	CSparseDbMatrix work((*this),0,numcol-1,0,numcol-1);
	int nn;
	double det;
	nn = spdet(work,det);
	if(nn != 0)
	{
		ErrorHandler((Matvec_errors)nn, __FILE__, __LINE__);
	}
	return det;	
}



CSparseDbMatrix CSparseDbMatrix::hess( CSparseDbMatrix &v)const
{
	int n = mrow();
	if(n!=mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        n = MIN(mrow(),mcol());
	}
	CSparseDbMatrix d(1,n);
	if((v.mrow()!=n)||v.mcol()!=n||v.vlen()!=n)
	{
		CSparseDbMatrix v1(n);
		v = v1;
	}
	int i=0;
	for(i=0; i<n; i++)
	{
		v(i) = CTriple<double>(i,i,1.0);
	}
	CSparseDbMatrix a = copy();
	double tol = TOLERANT_ERROR;
	sporithes(tol,a,d);
	sportrans(a,d,v);
	CIntVector rowidx = a.rowchidx();
	for(i=2; i<n; i++)
	{
		for(int j=rowidx(i); j<rowidx(i+1); j++)
		{
			if(i<=n-2)
			{
				if(a(j).ypos()<=i-2)
				{
					a.remove(j);
					for(int k=i+1; k<n+1; k++)
						rowidx(k)--;
					j--;
				}
			}
			else
			{
				if(a(j).ypos()<=i-2)
				{
					a.remove(j);
					for(int k=i+1; k<n+1; k++)
						rowidx(k)--;
					j--;
				}
			}
                 
		}
	}
	return a;
}

CSparseDbMatrix hess(const CSparseDbMatrix &x, CSparseDbMatrix &v)
{
	int n = x.mrow();
	if(n!=x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        n = MIN(x.mrow(),x.mcol());
	}
	CSparseDbMatrix d(1,n);
	if((v.mrow()!=n)||v.mcol()!=n||v.vlen()!=n)
	{
		CSparseDbMatrix v1(n);
		v = v1;
	}
	int i=0;
	for(i=0; i<n; i++)
	{
		v(i) = CTriple<double>(i,i,1.0);
	}
	CSparseDbMatrix a = x.copy();
	double tol = TOLERANT_ERROR;
	sporithes(tol,a,d);
	sportrans(a,d,v);
	CIntVector rowidx = a.rowchidx();
	for(i=2; i<n; i++)
	{
		for(int j=rowidx(i); j<rowidx(i+1); j++)
		{
			if(i<=n-2)
			{
				if(a(j).ypos()<=i-2)
				{
					a.remove(j);
					for(int k=i+1; k<n+1; k++)
						rowidx(k)--;
					j--;
				}
			}
			else
			{
				if(a(j).ypos()<=i-2)
				{
					a.remove(j);
					for(int k=i+1; k<n+1; k++)
						rowidx(k)--;
					j--;
				}
			}
                 
		}
	}
	return a;
}

CSparseDbMatrix hess(const CSparseDbMatrix &x)
{
	int n = x.mrow();
	if(n!=x.mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        n = MIN(x.mrow(),x.mcol());
	}
	CSparseDbMatrix d(1,n);
	CSparseDbMatrix a = x.copy();
	double tol = TOLERANT_ERROR;
	sporithes(tol,a,d);
	CIntVector rowidx = a.rowchidx();
	for(int i=2; i<n; i++)
	{
		for(int j=rowidx(i); j<rowidx(i+1); j++)
		{
			if(i<=n-2)
			{
				if(a(j).ypos()<=i-2)
				{
					a.remove(j);
					for(int k=i+1; k<n+1; k++)
						rowidx(k)--;
					j--;
				}
			}
			else
			{
				if(a(j).ypos()<=i-2)
				{
					a.remove(j);
					for(int k=i+1; k<n+1; k++)
						rowidx(k)--;
					j--;
				}
			}
                 
		}
	}
	return a;
}

CSparseDbMatrix CSparseDbMatrix::hess()const 
{
	int n = mrow();
	if(n!=mcol())
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
        n = MIN(mrow(),mcol());
	}
	CSparseDbMatrix d(1,n);
	CSparseDbMatrix a = copy();
	double tol = TOLERANT_ERROR;
	sporithes(tol,a,d);
	CIntVector rowidx = a.rowchidx();
	for(int i=2; i<n; i++)
	{
		for(int j=rowidx(i); j<rowidx(i+1); j++)
		{
			if(i<=n-2)
			{
				if(a(j).ypos()<=i-2)
				{
					a.remove(j);
					for(int k=i+1; k<n+1; k++)
						rowidx(k)--;
					j--;
				}
			}
			else
			{
				if(a(j).ypos()<=i-2)
				{
					a.remove(j);
					for(int k=i+1; k<n+1; k++)
						rowidx(k)--;
					j--;
				}
			}
                 
		}
	}
	return a;
}


CSparseCompMatrix CSparseDbMatrix::eig(CSparseCompMatrix &s)const 
{
	int i,j;
	int n=mrow();
	if(mcol()!=n)
	{
		ErrorHandler(EMAT_NOTSQUARE, __FILE__, __LINE__);
		n = MIN(mrow(), mcol());
	}
	CDoubleVector wr(n), wi(n);
	CSparseDbMatrix d(1,n);
	CIntVector cnt(n);
	CSparseDbMatrix h,v(n);

	for(i=0; i<n; i++)
	{
		v(i) = CTriple<double>(i,i,1.0);
	}
	if((s.mrow()!=n)||s.mcol()!=n||s.vlen()!=0)
	{
		CSparseCompMatrix s1(n,n);
		s = s1;
	}
	h = copy();
	//double eps = TOLERANT_ERROR;
	double eps = 1e-100;

	sporithes(eps,h,d);
	sportrans(h,d,v);
	int state = sphqr2(eps,h,v,wr,wi,cnt);
	if(state != 0)
	{
		ErrorHandler((Matvec_errors)state, __FILE__, __LINE__);
	}

	CSparseCompMatrix ee(n);
	for(i=0; i<n; i++)
		ee(i) = CTriple<CComplex>(i,i,CComplex(wr(i),wi(i)));
	ee.compress();

	v=v.trans();
	CIntVector vridx = v.rowchidx();
	CIntVector sridx(n+1);
	for(j=0; j<n; j++)
	{
		if(wi(j)+1==1)
		{
			CSparseDbMatrix trv(v,vridx(j),vridx(j+1)-1);
			CSparseCompMatrix trs = Complex(trv,0);
            for(i=0; i<trs.vlen(); i++)
				trs(i).xpos()=j;
			s.change(trs,sridx(j),sridx(j+1)-1);
			for(i=j+1; i<n+1; i++)
				sridx(i)+=trs.vlen()-(sridx(j+1)-sridx(j));
		}
		else
		{
			if(wi(j)>0)
			{
				CSparseDbMatrix trv1(v,vridx(j),vridx(j+1)-1), trv2(v,vridx(j+1),vridx(j+2)-1);
                CSparseCompMatrix trs = Complex(trv1,trv2);
				for(i=0; i<trs.vlen(); i++)
					trs(i).xpos()=j;
				s.change(trs,sridx(j),sridx(j+1)-1);
				for(i=j+1; i<n+1; i++)
					sridx(i)+=trs.vlen()-(sridx(j+1)-sridx(j));
			}
			else
			{
                CSparseCompMatrix trs1(s,sridx(j-1),sridx(j)-1);
				CSparseCompMatrix trs=conj(trs1);
				for(i=0; i<trs.vlen(); i++)
					trs(i).xpos()=j;
				s.change(trs,sridx(j),sridx(j+1)-1);
				for(i=j+1; i<n+1; i++)
					sridx(i)+=trs.vlen()-(sridx(j+1)-sridx(j));
			}
		}
	}
	s=s.trans();
	return ee;
}


//////////////////////////////////////////////////////////////////////
// CSparseCompMatrix -------- Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSparseCompMatrix & CSparseCompMatrix::operator =(const CSparseCompMatrix &x)
{
    this->CSparseMatrix<CComplex>::operator = (x);
	return * this;
}

CSparseCompMatrix & CSparseCompMatrix::operator =(const CSparseMatrix<CComplex> &x)
{
    this->CSparseMatrix<CComplex>::operator = (x);
	return * this;
}

CSparseCompMatrix & CSparseCompMatrix::operator =(const CComplexMatrix &x)
{
    this->CSparseMatrix<CComplex>::operator = (x);
	return * this;

}

CSparseCompMatrix & CSparseCompMatrix::operator =(CComplex **x)
{
    this->CSparseMatrix<CComplex>::operator = (x);
	return * this;

}

CSparseCompMatrix & CSparseCompMatrix::operator =(CComplex *x)
{
    this->CSparseMatrix<CComplex>::operator = (x);
	return * this;

}

CSparseCompMatrix & CSparseCompMatrix::operator =(CComplex x)
{
    this->CSparseMatrix<CComplex>::operator = (x);
	return * this;
}

void CSparseCompMatrix::compress()
{
    for(int i=0; i<vlen(); i++)
	{
		if(::abs((*this)(i).value())<DOUBLE_ZERO)
		{
            remove(i);
		}
	}
}

CSparseCompMatrix Complex(const CSparseDbMatrix &v1, const CSparseDbMatrix &v2)
{	
	int rows = v1.mrow();
	int cols = v1.mcol();
    CSparseCompMatrix v;
	if((v2.mrow() != rows) || (v2.mcol()!=cols))
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
		rows = MIN(v1.mrow(),v2.mrow());
		cols = MIN(v1.mcol(),v2.mcol());
	}
	v.rows = rows;
	v.cols = cols;
	int cv1 = 0, cv2 = 0;
	CTriple<double> t1, t2;
    int indv1, indv2;
	while(cv1<v1.vlen() && cv2<v2.vlen())
	{
		t1 = v1.GetValue(cv1);
		t2 = v2.GetValue(cv2);
		indv1 = t1.xpos()*cols + t1.ypos();
		indv2 = t2.xpos()*cols + t2.ypos();
		if(indv1<indv2)
		{
			CTriple<CComplex> t(t1.xpos(),t1.ypos(),CComplex(t1.value(),0));
			v.append(t);
			cv1++;
		}
		else if(indv1 == indv2)
		{
			
			CTriple<CComplex> t(t1.xpos(), t1.ypos(), CComplex(t1.value(),t2.value()));
			v.append(t);
			cv1++;
			cv2++;
		}
		else
		{
			CTriple<CComplex> t(t2.xpos(),t2.ypos(),CComplex(0,t2.value()));			
			v.append(t);
			cv2++;
		}
	}
	for(; cv1<v1.vlen(); cv1++)
	{
		CTriple<CComplex> t(v1.GetValue(cv1).gxpos(), v1.GetValue(cv1).gypos(),
			CComplex(v1.GetValue(cv1).gvalue(),0));
		v.append(t);
	}
	for(; cv2<v2.vlen(); cv2++)
	{
		CTriple<CComplex> t(v2.GetValue(cv2).gxpos(), v2.GetValue(cv2).gypos(),
			CComplex(0,v2.GetValue(cv2).gvalue()));
		v.append(t);
	}
	return v;
}

CSparseCompMatrix Complex(const CSparseDbMatrix &v1, double x)
{
	CSparseCompMatrix v(v1.vlen());
	v.rows = v1.mrow();
	v.cols = v1.mcol();
	for(int i=0; i<v1.vlen(); i++)
		v(i) = CTriple<CComplex>(v1.GetValue(i).gxpos(), v1.GetValue(i).gypos(), CComplex(v1.GetValue(i).gvalue(),x));
	return v;
}

CSparseCompMatrix Complex(double x, const CSparseDbMatrix &v1)
{
	CSparseCompMatrix v(v1.vlen());
	v.rows = v1.mrow();
	v.cols = v1.mcol();
	for(int i=0; i<v1.vlen(); i++)
		v(i) = CTriple<CComplex>(v1.GetValue(i).gxpos(), v1.GetValue(i).gypos(),
		CComplex(x, v1.GetValue(i).gvalue()));
	return v;
}


CSparseDbMatrix real(const CSparseCompMatrix &x)
{
	CSparseDbMatrix v(x.vlen());
	v.SetSize(x.mrow(),x.mcol());
	//v.mrow() = x.mrow();
	//v.mcol() = x.mcol();
   
	for(int i=0; i<x.vlen(); i++)
		v(i)= CTriple<double>(x.GetValue(i).gxpos(),
		x.GetValue(i).gypos(),real(x.GetValue(i).gvalue()));
	return v;
}


CSparseDbMatrix CSparseCompMatrix ::real()const 
{
	CSparseDbMatrix v(vlen());
	v.SetSize(mrow(),mcol());
	//v.mrow() = mrow();
	//v.mcol() = mcol();
   
	for(int i=0; i<vlen(); i++)
		v(i)= CTriple<double>((*this).GetValue(i).gxpos(),
		(*this).GetValue(i).gypos(),::real((*this).GetValue(i).gvalue()));
	return v;
}

CSparseDbMatrix imag(const CSparseCompMatrix &x)
{
	CSparseDbMatrix v(x.vlen());
	v.SetSize(x.mrow(),x.mcol());
	//v.mrow() = x.mrow();
	//v.mcol() = x.mcol();
   
	for(int i=0; i<x.vlen(); i++)
		v(i)= CTriple<double>(x.GetValue(i).gxpos(),x.GetValue(i).gypos(),imag(x.GetValue(i).gvalue()));
	return v;
}


CSparseDbMatrix CSparseCompMatrix ::imag()const 
{
	CSparseDbMatrix v(vlen());
	v.SetSize(mrow(),mcol());
	//v.mrow() = mrow();
	//v.mcol() = mcol();
   
	for(int i=0; i<vlen(); i++)
		v(i)= CTriple<double>((*this).GetValue(i).gxpos(),(*this).GetValue(i).gypos(),::imag((*this).GetValue(i).gvalue()));
	return v;
}

CSparseCompMatrix conj(const CSparseCompMatrix &x)
{
	CSparseCompMatrix v(x.vlen());
	v.SetSize(x.mrow(),x.mcol());
	//v.mrow() = x.mrow();
	//v.mcol() = x.mcol();
   
	for(int i=0; i<x.vlen(); i++)
		v(i)= CTriple<CComplex>(x.GetValue(i).gxpos(),x.GetValue(i).gypos(),conj(x.GetValue(i).gvalue()));
	return v;
}


CSparseCompMatrix CSparseCompMatrix ::conj()const 
{
	CSparseCompMatrix v(vlen());
	v.SetSize(mrow(),mcol());
	//v.mrow() = mrow();
	//v.mcol() = mcol();
   
	for(int i=0; i<vlen(); i++)
		v(i)= CTriple<CComplex>((*this).GetValue(i).gxpos(),(*this).GetValue(i).gypos(),::conj((*this).GetValue(i).gvalue()));
	return v;
}


template <> 
CMatrix<double> operator * (const CMatrix <double> &dm,const CDiagMatrix <double> &thin)
{
	int m1rows = dm.mrow();
	int m1cols = dm.mcol();
	int nLen = thin.GetLen();
	if(m1cols != nLen)
	{//出错
		return NULL;
	}
	
	CVector <double> *pData = thin.GetAddr();
	CMatrix<double> v(m1rows, nLen);
	for(int i=0; i<m1rows; i++)
	{
		for(int j=0;j< nLen; j++)
		{
			v(i,j)=dm.GetValue(i,j)*( (*pData)(j) );
		}
	}
	return v;
}




















