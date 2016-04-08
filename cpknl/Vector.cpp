// Vector.cpp: implementation of the CVector class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Vector.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// CIntVector -------- Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntVector & CIntVector::operator =(const CIntVector & vec)
{
    this->CVector<int>::operator = (vec);
	return * this;
}

CIntVector & CIntVector::operator =(const CVector<int> & vec)
{
    this->CVector<int>::operator = (vec);
	return * this;
}


CIntVector & CIntVector::operator =(int * x)
{
    this->CVector<int>::operator = (x);
	return * this;
}
	

CIntVector & CIntVector::operator =(int  x)
{
    this->CVector<int>::operator = (x);
	return * this;
}



double var(const CIntVector &v) //zfm
{
	int i;
	double s=0.0,sum=0.0;
	int len=v.vlen();
//	assert(len>1);
//  yzmModify
	if (len>1){
		for(i=0; i<len; i++)
		{
			s+=v.GetValue(i)*v.GetValue(i);
			sum+=v.GetValue(i);
		}
		return (s-sum*sum/len)/(len-1);
    }
	else
		return 0;
}
	
double CIntVector::var()const 
{
	int i;
	double s=0.0,sum=0.0;
	int len=vlen();
//	assert(len>1);
//  yzmModify
	if (len>1)
	{
		double re = 0;
		CAllocate<int> *f = p->elem;
		
		for(i=0; i<len; i++)
		{
			s+=((*f)(i))*((*f)(i));
			sum+=((*f)(i));
		}
		return (s-sum*sum/len)/(len-1);
    }
	else
		return 0;
}


CIntVector & CIntVector::Sort(bool select)
{
	int n=vlen();
	int i, j, m, k;
	int d;
	k=0;
	m = n-1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((*this)(i)>(*this)(i+1))
				{
					d =(*this)(i);
					(*this)(i)=(*this)(i+1);
					(*this)(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((*this)(i-1)>(*this)(i))
				{
					d = (*this)(i);
					(*this)(i) =(*this)(i-1);
					(*this)(i-1) = d;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((*this)(i)<(*this)(i+1))
				{
					d =(*this)(i);
					(*this)(i)=(*this)(i+1);
					(*this)(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((*this)(i-1)<(*this)(i))
				{
					d = (*this)(i);
					(*this)(i) =(*this)(i-1);
					(*this)(i-1) = d;
					k=i;
				}
		}
	}
    return *this;
}


CIntVector Sort(CIntVector &a, bool select)
{
	int n=a.vlen();
	CIntVector p(n);
	p = copy(a);
	int i, j, m, k;
	int d;
	k=0;
	m = n-1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(p(i)>p(i+1))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(p(i-1)>p(i))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(p(i)<p(i+1))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(p(i-1)<p(i))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					k=i;
				}
		}
	}
    return p;
}

CIntVector & CIntVector::Sort(CIntVector &idx, bool select)
{
	int n=vlen();
	int i, j, m, k, swap;
	int d;
	k=0;
	m = n-1;
	if(idx.vlen() != n)
	{
		CIntVector idxx(n);
		idx = idxx;
	}
	for(i=0; i<n; i++)
		idx(i) = i+1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((*this)(i)>(*this)(i+1))
				{
					d =(*this)(i);
					(*this)(i)=(*this)(i+1);
					(*this)(i+1)=d;
					swap =idx(i);
					idx(i)=idx(i+1);
					idx(i+1)=swap;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((*this)(i-1)>(*this)(i))
				{
					d = (*this)(i);
					(*this)(i) =(*this)(i-1);
					(*this)(i-1) = d;
					swap =idx(i-1);
					idx(i-1)=idx(i);
					idx(i)=swap;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((*this)(i)<(*this)(i+1))
				{
					d =(*this)(i);
					(*this)(i)=(*this)(i+1);
					(*this)(i+1)=d;
					swap =idx(i);
					idx(i)=idx(i+1);
					idx(i+1)=swap;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((*this)(i-1)<(*this)(i))
				{
					d = (*this)(i);
					(*this)(i) =(*this)(i-1);
					(*this)(i-1) = d;
					swap =idx(i-1);
					idx(i-1)=idx(i);
					idx(i)=swap;
					k=i;
				}
		}
	}
    return *this;
}


CIntVector Sort(CIntVector &a, CIntVector &idx, bool select)
{
	int n=a.vlen();
	CIntVector p(n);
	p = copy(a);
	int i, j, m, k, swap;
	int d;
	k=0;
	m = n-1;
	if(idx.vlen() != n)
	{
		CIntVector idxx(n);
		idx = idxx;
	}
	for(i=0; i<n; i++)
		idx(i) = i+1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(p(i)>p(i+1))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					swap =idx(i);
					idx(i)=idx(i+1);
					idx(i+1)=swap;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(p(i-1)>p(i))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					swap =idx(i-1);
					idx(i-1)=idx(i);
					idx(i)=swap;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(p(i)<p(i+1))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					swap =idx(i);
					idx(i)=idx(i+1);
					idx(i+1)=swap;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(p(i-1)<p(i))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					swap =idx(i-1);
					idx(i-1)=idx(i);
					idx(i)=swap;
					k=i;
				}
		}
	}
    return p;
}


CIntVector & CIntVector::quicksort(int low, int high, bool bUp)
{

	int i,j;
	double pt;
	i = high;
	j = low;
	pt = (*this) ((int)((i+j)/2));
	do{
		if(bUp)
		{
			while((*this)(j) < pt)  j++;
			while((*this)(i) > pt)  i--;
		}
		else
		{
			while((*this)(j)> pt) j++;
			while((*this)(i)< pt) i--;
		}
		if(i>=j)
		{
			if(i != j)
			{
				int dTemp = (*this)(i);
				(*this)(i) = (*this)(j);
				(*this)(j) = dTemp;
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(low < i)   this->quicksort(low,i,bUp);
	if(j < high) this->quicksort(j,high,bUp);

	return *this;
}


CIntVector &quicksort(CIntVector &v,int low,int high, bool bUp)
{

	int i,j;
	double pt;
	i = high;
	j = low;
	pt = v((int)((i+j)/2));
	do{
		if(bUp)
		{
			while(v(j) < pt)  j++;
			while(v(i) > pt)  i--;
		}
		else
		{
			while(v(j)> pt) j++;
			while(v(i)< pt) i--;
		}
		if(i>=j)
		{
			if(i != j)
			{
				int dTemp = v(i);
				v(i) = v(j);
				v(j) = dTemp;
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(low < i)   quicksort(v,low,i,bUp);
	if(j < high)  quicksort(v,j,high,bUp);

	return v;



}





int CIntVector::Max()const 
{
	int xx;
	int nn = vlen();
	xx = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)>xx)
			xx = (*this).GetValue(i);
	return xx;
}

CIntVector CIntVector::Max(const CIntVector &v2)const
{
	int nn=MIN(vlen(), v2.vlen());
	CIntVector xx(nn);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)>v2.GetValue(i))
			xx(i) = (*this).GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}

int Max(const CIntVector &v)
{
	int xx;
	int nn = v.vlen();
	xx = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)>xx)
			xx = v.GetValue(i);
	return xx;
}

CIntMatrix Max(const CIntMatrix &v1, const CIntMatrix &v2)
{
	int nr = v1.mrow();
	int nc = v1.mcol();
	if((v2.mrow()!=nr)||(v2.mcol()!=nc))
    {
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
		nr = MIN(v1.mrow(), v2.mrow());
		nc = MIN(v1.mcol(), v2.mcol());
	}
	CIntMatrix xx(nr,nc);
	for(int i=0; i<nr; i++)
		for(int j=0; j<nc; j++)
		{
			xx(i,j) = v1.GetValue(i,j);
			if(v2.GetValue(i,j)>v1.GetValue(i,j))
				xx(i,j)=v2.GetValue(i,j);
		}
	return xx;
}

int CIntVector::Min()const 
{
	int xx;
	int nn = vlen();
	xx = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<xx)
			xx = (*this).GetValue(i);
	return xx;
}


int Min(const CIntVector &v)
{
	int xx;
	int nn = v.vlen();
	xx = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)<xx)
			xx = v.GetValue(i);
	return xx;
}


CIntVector CIntVector::Min(const CIntVector &v2)const
{
	int nn=MIN(vlen(), v2.vlen());
	CIntVector xx(nn);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<v2.GetValue(i))
			xx(i) = (*this).GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}

CIntVector Min(const CIntVector &v1, const CIntVector &v2)
{
	int nn=MIN(v1.vlen(), v2.vlen());
	CIntVector xx(nn);
	for(int i=1; i<nn; i++)
		if(v1.GetValue(i)<v2.GetValue(i))
			xx(i) = v1.GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}


void MaxMin(const CIntVector &v,const CIntVector& Idx,int& dMax,int& dMin)
{
	int nn = MIN(v.vlen(),Idx.vlen());
	int i=0;
	for (i=0; i<nn;i++)
		if (Idx.GetValue(i) == 1)
		{
			dMax = dMin = v.GetValue(i);
			break;
		}
	for(; i<nn; i++)
		if (Idx.GetValue(i) == 1)
		{
			if(v.GetValue(i)<dMin)
				dMin = v.GetValue(i);
			else if (v.GetValue(i)>dMax)
				dMax = v.GetValue(i);
		}
}

void CIntVector::MaxMin(const CIntVector &Idx,int& dMax,int& dMin)const 
{
	int nn = MIN(vlen(),Idx.vlen());
	int i=0;
	for (i=0;i<nn;i++)
		if (Idx.GetValue(i)==1)
		{
			dMax = dMin = (*this).GetValue(i);
			break;
		}
	for(; i<nn; i++)
		if (Idx.GetValue(i) == 1)
		{
			if((*this).GetValue(i)<dMin)
				dMin = (*this).GetValue(i);
			else if ((*this).GetValue(i)>dMax)
				dMax = (*this).GetValue(i);
		}
}

void MaxMin(const CIntVector &v,int& dMax,int& dMin)
{
	int nn = v.vlen();
	dMax = dMin = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)<dMin)
			dMin = v.GetValue(i);
		else if (v.GetValue(i)>dMax)
			dMax = v.GetValue(i);
}

void CIntVector::MaxMin(int& dMax,int& dMin)const 
{
	int nn = vlen();
	dMax = dMin = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<dMin)
			dMin = (*this).GetValue(i);
		else if ((*this).GetValue(i)>dMax)
			dMax = (*this).GetValue(i);
}

void MaxMin(const CIntVector &v,int& dMax,int& dMin,int& nIdxMax,int& nIdxMin)
{
	int nn = v.vlen();
	dMax = dMin = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)<dMin)
		{
			dMin = v.GetValue(i);
			nIdxMin = i;
		}
		else if (v.GetValue(i)>dMax)
		{
			dMax = v.GetValue(i);
			nIdxMax = i;
		}
}

void CIntVector::MaxMin(int& dMax,int& dMin,int& nIdxMax,int& nIdxMin)const 
{
	int nn = vlen();
	dMax = dMin = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<dMin)
		{
			dMin = (*this).GetValue(i);
			nIdxMin = i;
		}
		else if ((*this).GetValue(i)>dMax)
		{
			dMax = (*this).GetValue(i);
			nIdxMax = i;
		}
}


//////////////////////////////////////////////////////////////////////
// CDoubleVector ------- Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDoubleVector & CDoubleVector::operator =(const CDoubleVector & vec)
{
    this->CVector<double>::operator = (vec);
	return * this;
}

CDoubleVector & CDoubleVector::operator =(const CVector<double> & vec)
{
    this->CVector<double>::operator = (vec);
	return * this;
}

CDoubleVector & CDoubleVector::operator =(const CIntVector & vec)
{
   *this = ivectodvec(vec);
	return * this;
}

CDoubleVector & CDoubleVector::operator =(double * x)
{
    this->CVector<double>::operator = (x);
	return * this;
}
	

CDoubleVector & CDoubleVector::operator =(double  x)
{
    this->CVector<double>::operator = (x);
	return * this;
}



CDoubleVector & CDoubleVector::operator =(const CFloatVector & vec)
{
   *this = fvectodvec(vec);
	return * this;
}

/*
CDoubleVector operator -(const CIntVector &v1, double x) //zfm
{

	int len = v1.vlen();
	CDoubleVector v(len);

	for(int i=0; i<len; i++)
	{
		v(i) = v1.GetValue(i) - x;
	}


	return v;
}

*/
/*
CDoubleVector operator *(const CDoubleVector &v1, const CIntVector &w) //zfm
{
	int v_len=v1.vlen();
	int w_len=w.vlen();
	assert (v_len==w_len);
	CDoubleVector v(v_len);
	for(int i=0; i<v_len; i++)
		v(i)=v1.GetValue(i) * w.GetValue(i);
	return v;
}
*/



CDoubleVector ivectodvec(const CIntVector & vec)
{
	int len = vec.vlen();
	CDoubleVector v(len);
	for(int i=0; i<len; i++)
		v(i)=vec.GetValue(i);
	return v;
}

CFloatVector ivectofvec(const CIntVector & vec)
{
	int len = vec.vlen();
	CFloatVector v(len);
	for(int i=0; i<len; i++)
		v(i)=(float)vec.GetValue(i);
	return v;
}

CDoubleVector fvectodvec(const CFloatVector & vec)
{
	int len = vec.vlen();
	CDoubleVector v(len);
	for(int i=0; i<len; i++)
		v(i)=vec.GetValue(i);
	return v;
}

int any(const CDoubleVector &v)
{
	int n=v.vlen();
	int re = 0;
	for(int i=0; i<n; i++)
		if(v.GetValue(i) !=0)
		{
			re=1;
			break;
		}
	return re;
}

int CDoubleVector::any()const 
{
	int n=vlen();
	int re = 0;
	for(int i=0; i<n; i++)
		if((*this).GetValue(i) !=0)
		{
			re=1;
			break;
		}
	return re;
}

double Max(const CDoubleVector &v)
{
	double xx;
	int nn = v.vlen();
	xx = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)>xx)
			xx = v.GetValue(i);
	return xx;
}

double Max(const CDoubleVector &v,int& nIndex)
{
	double xx;
	int nn = v.vlen();
	xx = v.GetValue(0);
	nIndex = 0;
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)>xx)
		{
			xx = v.GetValue(i);
			nIndex = i;
		}
	return xx;
}

double CDoubleVector::Max()const 
{
	double xx;
	int nn = vlen();
	xx = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)>xx)
			xx = (*this).GetValue(i);
	return xx;
}

double CDoubleVector::Max(int &nIndex)const 
{
	double xx;
	int nn = vlen();
	xx = (*this).GetValue(0);
	nIndex = 0;
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)>xx)
		{
			xx = (*this).GetValue(i);
			nIndex = i;
		}
		return xx;
}



CDoubleVector Max(const CDoubleVector &v1, const CDoubleVector &v2)
{
	int nn=MIN(v1.vlen(), v2.vlen());
	CDoubleVector xx(nn);
	for(int i=1; i<nn; i++)
		if(v1.GetValue(i)>v2.GetValue(i))
			xx(i) = v1.GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}

CDoubleVector CDoubleVector::Max(const CDoubleVector &v2)const
{
	int nn=MIN(vlen(), v2.vlen());
	CDoubleVector xx(nn);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)>v2.GetValue(i))
			xx(i) = (*this).GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}


double CDoubleVector::Min()const 
{
	double xx;
	int nn = vlen();
	xx = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<xx)
			xx = (*this).GetValue(i);
	return xx;
}

double Min(const CDoubleVector &v)
{
	double xx;
	int nn = v.vlen();
	xx = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)<xx)
			xx = v.GetValue(i);
	return xx;
}

double Min(const CDoubleVector &v,int &nIndex)
{
	double xx;
	int nn = v.vlen();
	xx = v.GetValue(0);
	nIndex = 0;
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)<xx)
		{
			xx = v.GetValue(i);
			nIndex = i;
		}
	return xx;
}

double CDoubleVector::Min(int &nIndex)const 
{
	double xx;
	int nn = vlen();
	xx = (*this).GetValue(0);
	nIndex = 0;
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<xx)
		{
			xx = (*this).GetValue(i);
			nIndex = i;
		}
	return xx;
}

void MaxMin(const CDoubleVector &v,const CIntVector& Idx,double& dMax,double& dMin)
{
	int nn = MIN(v.vlen(),Idx.vlen());
	int i=0;
	for (i=0; i<nn;i++)
		if (Idx.GetValue(i) == 1)
		{
			dMax = dMin = v.GetValue(i);
			break;
		}
	for(; i<nn; i++)
		if (Idx.GetValue(i) == 1)
		{
			if(v.GetValue(i)<dMin)
				dMin = v.GetValue(i);
			else if (v.GetValue(i)>dMax)
				dMax = v.GetValue(i);
		}
}

void CDoubleVector::MaxMin(const CIntVector &Idx,double& dMax,double& dMin)const 
{
	int nn = MIN(vlen(),Idx.vlen());
	int i=0;
	for (i=0;i<nn;i++)
		if (Idx.GetValue(i)==1)
		{
			dMax = dMin = (*this).GetValue(i);
			break;
		}
	for(; i<nn; i++)
		if (Idx.GetValue(i) == 1)
		{
			if((*this).GetValue(i)<dMin)
				dMin = (*this).GetValue(i);
			else if ((*this).GetValue(i)>dMax)
				dMax = (*this).GetValue(i);
		}
}

void MaxMin(const CDoubleVector &v,double& dMax,double& dMin)
{
	int nn = v.vlen();
	dMax = dMin = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)<dMin)
			dMin = v.GetValue(i);
		else if (v.GetValue(i)>dMax)
			dMax = v.GetValue(i);
}

void CDoubleVector::MaxMin(double& dMax,double& dMin)const 
{
	int nn = vlen();
	dMax = dMin = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<dMin)
			dMin = (*this).GetValue(i);
		else if ((*this).GetValue(i)>dMax)
			dMax = (*this).GetValue(i);
}

void MaxMin(const CDoubleVector &v,double& dMax,double& dMin,int& nIdxMax,int& nIdxMin)
{
	int nn = v.vlen();
	dMax = dMin = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(v.GetValue(i)<dMin)
		{
			dMin = v.GetValue(i);
			nIdxMin = i;
		}
		else if (v.GetValue(i)>dMax)
		{
			dMax = v.GetValue(i);
			nIdxMax = i;
		}
}

void CDoubleVector::MaxMin(double& dMax,double& dMin,int& nIdxMax,int& nIdxMin)const 
{
	int nn = vlen();
	dMax = dMin = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<dMin)
		{
			dMin = (*this).GetValue(i);
			nIdxMin = i;
		}
		else if ((*this).GetValue(i)>dMax)
		{
			dMax = (*this).GetValue(i);
			nIdxMax = i;
		}
}

double CDoubleVector::Mean()const 
{
	double sum=0;
	int nn = vlen();
	sum+= (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		sum+= (*this).GetValue(i);
	return sum/nn;
}

double Mean(const CDoubleVector &v, const CIntVector w)
{
	int v_len=v.vlen();
	int w_len=w.vlen();
	assert( (v_len==w_len) && (v_len>0) );
	
	double re=0;
	for(int i=0; i<v_len; i++)
		re +=(v.GetValue(i))*(w.GetValue(i));
	re /=sum(w);
	return re;
}

CDoubleVector Min(const CDoubleVector &v1, const CDoubleVector &v2)
{
	int nn=MIN(v1.vlen(), v2.vlen());
	CDoubleVector xx(nn);
	for(int i=1; i<nn; i++)
		if(v1.GetValue(i)<v2.GetValue(i))
			xx(i) = v1.GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}

CDoubleVector CDoubleVector::Min(const CDoubleVector &v2)const 
{
	int nn=MIN(vlen(), v2.vlen());
	CDoubleVector xx(nn);
	for(int i=1; i<nn; i++)
		if((*this).GetValue(i)<v2.GetValue(i))
			xx(i) = (*this).GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}

CDoubleVector abs(const CDoubleVector &v)
{
    int nn=v.vlen();
	CDoubleVector xx(nn);
	for(int i=0; i<nn; i++)
		xx(i) = ::fabs(v.GetValue(i));
	return xx;
}

CDoubleVector CDoubleVector::abs()const 
{
    int nn=vlen();
	CDoubleVector xx(nn);
	for(int i=0; i<nn; i++)
		xx(i) = ::fabs((*this).GetValue(i));
	return xx;
}

double CDoubleVector::dot(const CDoubleVector &v2)const 
{
	int len = MIN(vlen(), v2.vlen());
	double sum =0;
	for(int i=0; i<len; i++)
	{
		sum += (*this).GetValue(i)*v2.GetValue(i);
	}
	return sum;
}

CDoubleVector CDoubleVector::dot(const CDoubleMatrix &v2)const 
{
	int len = v2.mcol();
	CDoubleVector v(len);
	for(int i=0; i<len; i++)
	{
		CDoubleVector Vect_F = v2.GetValue(i);
		v(i) = dot(Vect_F);
	}
	return v;
}

double dot(const CDoubleVector &v1, const CDoubleVector &v2)
{
	int len = MIN(v1.vlen(), v2.vlen());
	double sum =0;
	for(int i=0; i<len; i++)
	{
		sum += v1.GetValue(i)*v2.GetValue(i);
	}
	return sum;
}

double norm(const CDoubleVector &v, int n)
{
	int nn=v.vlen();
	double xx;
	CDoubleVector temp(nn);
	temp = abs(v);
	if(n<0)
		xx = Min(temp);
	else
	{
		if(n==0)
			xx = Max(temp);
		else
		{
			xx = ::pow(sum(pow(temp,n)),1.0/n);
		}
	}
	return xx;
}

double CDoubleVector::norm(int n)const 
{
	int nn=vlen();
	double xx;
	CDoubleVector temp(nn);
	temp = abs();
	if(n<0)
		xx = ::Min(temp);
	else
	{
		if(n==0)
			xx = ::Max(temp);
		else
			xx = ::pow(::sum(::pow(temp,n)),1.0/n);
	}
	return xx;
}

CDoubleVector pow(const CDoubleVector &v, double pp)
{
	int nn=v.vlen();
	CDoubleVector xx(nn);
	for(int i=0; i<nn; i++)
		xx(i) = ::pow(v.GetValue(i),pp);
	return xx;
}

CDoubleVector & CDoubleVector::pow(double pp)
{
	int nn=vlen();
	for(int i=0; i<nn; i++)
		(*this)(i) = ::pow((*this).GetValue(i),pp);
	return (*this);
}

CDoubleVector pow(const CDoubleVector &x, const CDoubleVector &pp)
{
	int lens = x.vlen();
	int lens1 = pp.vlen();
	if(lens != lens1)
	{
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
        lens = MIN(x.vlen(), pp.vlen());
	}
    CDoubleVector v(lens);
	for(int i=0; i<lens; i++)
		v(i) = ::pow(x.GetValue(i), pp.GetValue(i));
	return v;
}

CDoubleVector & CDoubleVector::pow(const CDoubleVector &pp)
{
	int lens = vlen();
	int lens1 = pp.vlen();
	if(lens != lens1)
	{
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
        lens = MIN(vlen(), pp.vlen());
	}
	for(int i=0; i<lens; i++)
		(*this)(i) = ::pow((*this)(i), pp.GetValue(i));
	return (*this);
}

CDoubleVector & CDoubleVector::Rand()
{
	int nr = vlen();
	for(int i=0; i<nr; i++)
		(*this)(i) = (double)::rand()/(double)RAND_MAX;
	return *this;
}

CDoubleVector & Rand(CDoubleVector &v)
{
	int nr = v.vlen();
	for(int i=0; i<nr; i++)
		v(i) = (double)::rand()/(double)RAND_MAX;
	return v;
}

CDoubleVector CDoubleVector::Rand(int m)const 
{
	CDoubleVector v(m);
	for(int i=0; i<m; i++)
		v(i) = (double)::rand()/(double)RAND_MAX;
	return v;
}

CDoubleVector & CDoubleVector::Sort(bool select)
{
	int n=vlen(),i,j;
	int nGroup = GetGroupCount();
	int nGroupSize = GetGroupSize();

	if (n==0)
		return *this;
	//分组排序
	for (i=0;i<nGroup;i++)
	{
		int nBegin = i*nGroupSize;
		int nEnd = (i+1)*nGroupSize-1;
		if (nEnd >= n)
			nEnd = n-1;
		quicksort(nBegin,nEnd,select == 0 ? true : false);
	}

	if (nGroup > 1)
	{
		//分组归并
		CDoubleVector	Vect_Sort(n);
		CDoubleVector	Vect_Temp(n);

		int nGrp = 0;
		while (nGrp < nGroup) {
			int nBgnPos = nGrp*nGroupSize;
			int nEndPos = (nGrp+1)*nGroupSize;
			if (nEndPos > n)
				nEndPos = n;

			int nCur = 0;
			int nSrPos = 0;
			int nDtPos = nBgnPos;
			if (select==0)
			{
				while(nSrPos < nBgnPos && nDtPos < nEndPos)
				{
					if ((*this)(nSrPos) < (*this)(nDtPos))
					{
						Vect_Sort(nCur) = (*this)(nSrPos);
						nSrPos++;
					}
					else
					{
						Vect_Sort(nCur) = (*this)(nDtPos);
						nDtPos++;
					}
					nCur++;
				}
			}
			else
			{
				while(nSrPos < nBgnPos && nDtPos < nEndPos)
				{
					if ((*this)(nSrPos) < (*this)(nDtPos))
					{
						Vect_Sort(nCur) = (*this)(nDtPos);
						nSrPos++;
					}
					else
					{
						Vect_Sort(nCur) = (*this)(nSrPos);
						nDtPos++;
					}
					nCur++;
				}
			}
			if (nSrPos < nBgnPos)
			{
				for (i=nSrPos;i<nBgnPos;i++)
				{
					Vect_Sort(nCur) = (*this)(i);
					nCur++;	
				}
			}
			if (nDtPos < nEndPos)
			{
				for (i=nDtPos;i<nEndPos;i++)
				{
					Vect_Sort(nCur) = (*this)(i);
					nCur++;
				}
			}
			for (i=0;i<nEndPos;i++)
				(*this)(i) = Vect_Sort(i);
			nGrp++;
		}
	}
	return *this;//add by xds 20060113.

	int m, k;
	double d;
	CAllocate<double> *f = p->elem;
	k=0;
	m = n-1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((*f)(i)>(*f)(i+1))
				{
					d =(*f)(i);
					(*f)(i)=(*f)(i+1);
					(*f)(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((*f)(i-1)>(*f)(i))
				{
					d = (*f)(i);
					(*f)(i) =(*f)(i-1);
					(*f)(i-1) = d;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((*f)(i)<(*f)(i+1))
				{
					d =(*f)(i);
					(*f)(i)=(*f)(i+1);
					(*f)(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((*f)(i-1)<(*f)(i))
				{
					d = (*f)(i);
					(*f)(i) =(*f)(i-1);
					(*f)(i-1) = d;
					k=i;
				}
		}
	}
    return *this;
}


CDoubleVector Sort(CDoubleVector &a, bool select)
{
	int n=a.vlen();
	CDoubleVector p = copy(a);
	CDoubleVector Vect = p.Sort(select);
	return Vect;//add by xds 20060113.

	
/*
	int n=a.vlen();
	CDoubleVector p(n);
	p = copy(a);
	int i, j, m, k;
	double d;
	k=0;
	m = n-1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(p(i)>p(i+1))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(p(i-1)>p(i))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(p(i)<p(i+1))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(p(i-1)<p(i))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					k=i;
				}
		}
	}
    return p;
*/
}

CDoubleVector & CDoubleVector::Sort(CIntVector &idx, bool select)
{
	int n=vlen(),i,j;
	int nGroup = GetGroupCount();
	int nGroupSize = GetGroupSize();
	if (n==0)
		return *this;

	//if(idx.vlen() != n) //modify by xds 20060724-存在与否均需要重新创建
	{
		idx.create(n);
		for(i=0; i<n; i++)
			idx(i) = i+1;
	}

	//分组排序
	for (i=0;i<nGroup;i++)
	{
		int nBegin = i*nGroupSize;
		int nEnd = (i+1)*nGroupSize-1;
		if (nEnd >= n)
			nEnd = n-1;
		quicksort(idx,nBegin,nEnd,select == 0 ? true : false);
	}
	
	if (nGroup > 1)
	{
		//分组归并
		CDoubleVector	Vect_Sort(n);
		CDoubleVector	Vect_Temp(n);
		CIntVector	Vect_Idx(n);
		
		int nGrp = 0;
		while (nGrp < nGroup) {
			int nBgnPos = nGrp*nGroupSize;
			int nEndPos = (nGrp+1)*nGroupSize;
			if (nEndPos > n)
				nEndPos = n;
			
			int nCur = 0;
			int nSrPos = 0;
			int nDtPos = nBgnPos;
			if (select==0)
			{
				while(nSrPos < nBgnPos && nDtPos < nEndPos)
				{
					if ((*this)(nSrPos) < (*this)(nDtPos))
					{
						Vect_Sort(nCur) = (*this)(nSrPos);
						Vect_Idx(nCur) = idx(nSrPos);
						nSrPos++;
					}
					else
					{
						Vect_Sort(nCur) = (*this)(nDtPos);
						Vect_Idx(nCur) = idx(nDtPos);
						nDtPos++;
					}
					nCur++;
				}
			}
			else
			{
				while(nSrPos < nBgnPos && nDtPos < nEndPos)
				{
					if ((*this)(nSrPos) < (*this)(nDtPos))
					{
						Vect_Sort(nCur) = (*this)(nDtPos);
						Vect_Idx(nCur) = idx(nDtPos);
						nDtPos++;
					}
					else
					{
						Vect_Sort(nCur) = (*this)(nSrPos);
						Vect_Idx(nCur) = idx(nSrPos);
						nSrPos++;
					}
					nCur++;
				}
			}
			if (nSrPos < nBgnPos)
			{
				for (i=nSrPos;i<nBgnPos;i++,nCur++)
				{
					Vect_Sort(nCur) = (*this)(i);
					Vect_Idx(nCur) = idx(i);
				}
			}
			if (nDtPos < nEndPos)
			{
				for (i=nDtPos;i<nEndPos;i++,nCur++)
				{
					Vect_Sort(nCur) = (*this)(i);
					Vect_Idx(nCur) = idx(i);
				}
			}
			for (i=0;i<nEndPos;i++)
			{
				(*this)(i) = Vect_Sort(i);
				idx(i) = Vect_Idx(i);
			}
			nGrp++;
		}
	}
	return *this;//add by xds 20060113.

	int m, k, swap;
	double d;
	CAllocate<double> *f=p->elem;

	k=0;
	m = n-1;
	if(idx.vlen() != n)
	{
		CIntVector idxx(n);
		idx = idxx;
	}
	for(i=0; i<n; i++)
		idx(i) = i+1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((*f)(i)>(*f)(i+1))
				{
					d =(*f)(i);
					(*f)(i)=(*f)(i+1);
					(*f)(i+1)=d;
					swap =idx(i);
					idx(i)=idx(i+1);
					idx(i+1)=swap;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((*f)(i-1)>(*f)(i))
				{
					d = (*f)(i);
					(*f)(i) =(*f)(i-1);
					(*f)(i-1) = d;
					swap =idx(i-1);
					idx(i-1)=idx(i);
					idx(i)=swap;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((*f)(i)<(*f)(i+1))
				{
					d =(*f)(i);
					(*f)(i)=(*f)(i+1);
					(*f)(i+1)=d;
					swap =idx(i);
					idx(i)=idx(i+1);
					idx(i+1)=swap;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((*f)(i-1)<(*f)(i))
				{
					d = (*f)(i);
					(*f)(i) =(*f)(i-1);
					(*f)(i-1) = d;
					swap =idx(i-1);
					idx(i-1)=idx(i);
					idx(i)=swap;
					k=i;
				}
		}
	}
    return *this;
}


CDoubleVector Sort(CDoubleVector &a, CIntVector &idx, bool select)
{
	int n=a.vlen();
	CDoubleVector p(n);
	p = copy(a);
	CDoubleVector Vect = p.Sort(idx,select);
	//	Vect.Update();
	return Vect;//add by xds 20060113.

	
	
/*
	int n=a.vlen();
	CDoubleVector p(n);
	p = copy(a);
	int i, j, m, k, swap;
	double d;
	k=0;
	m = n-1;
	if(idx.vlen() != n)
	{
		CIntVector idxx(n);
		idx = idxx;
	}
	for(i=0; i<n; i++)
		idx(i) = i+1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(p(i)>p(i+1))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					swap =idx(i);
					idx(i)=idx(i+1);
					idx(i+1)=swap;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(p(i-1)>p(i))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					swap =idx(i-1);
					idx(i-1)=idx(i);
					idx(i)=swap;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(p(i)<p(i+1))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					swap =idx(i);
					idx(i)=idx(i+1);
					idx(i+1)=swap;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(p(i-1)<p(i))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					swap =idx(i-1);
					idx(i-1)=idx(i);
					idx(i)=swap;
					k=i;
				}
		}
	}
    return p;
*/
}



CDoubleVector & CDoubleVector::quicksort(int low, int high, bool bUp)
{

	int i,j;
	double pt;
	i = high;
	j = low;
	pt = (*this) ((int)((i+j)/2));
	do{
		if(bUp)
		{
			while((*this)(j) < pt)  j++;
			while((*this)(i) > pt)  i--;
		}
		else
		{
			while((*this)(j)> pt) j++;
			while((*this)(i)< pt) i--;
		}
		if(i>=j)
		{
			if(i != j)
			{
				double dTemp = (*this)(i);
				(*this)(i) = (*this)(j);
				(*this)(j) = dTemp;
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(low < i)   this->quicksort(low,i,bUp);
	if(j < high) this->quicksort(j,high,bUp);

	return *this;
}

//add by xds 20060113.
CDoubleVector & CDoubleVector::quicksort(CIntVector &idx, int low, int high, bool bAssc)
{
	int i,j,n;
	double pt;
	CAllocate<double> *f = p->elem;
	
	n = vlen();
	//	if(idx.vlen() != n)
	//	{
	//		idx.create(n);
	//		for(i=0; i<n; i++)
	//			idx(i) = i+1;
	//	}
	
	i = high;
	j = low;
	pt = (*f) ((int)((i+j)/2));
	do{
		if(bAssc)
		{
			while((*f)(j) < pt)  j++;
			while((*f)(i) > pt)  i--;
		}
		else
		{
			while((*f)(j)> pt) j++;
			while((*f)(i)< pt) i--;
		}
		if(i>=j)
		{
			if(i != j)
			{
				double dTemp = (*f)(i);
				(*f)(i) = (*f)(j);
				(*f)(j) = dTemp;
				int n   = idx(i);
				idx(i)  = idx(j);
				idx(j)  = n;
			}
			i--;
			j++;
		}
	}while(j<=i);
	
	if(low < i)   this->quicksort(idx,low,i,bAssc);
	if(j < high) this->quicksort(idx,j,high,bAssc);
	
	return *this;
}

//add by xds 20060113.
CDoubleVector &CDoubleVector::quicksort(CDoubleVector &v,CIntVector &idx, int low, int high, bool bAssc)
{
	return v.quicksort(idx,low,high,bAssc);
}


CDoubleVector &quicksort(CDoubleVector &v,int low,int high, bool bUp)
{
	int i,j;
	double pt;
	i = high;
	j = low;
	pt = v((int)((i+j)/2));
	do{
		if(bUp)
		{
			while(v(j) < pt)  j++;
			while(v(i) > pt)  i--;
		}
		else
		{
			while(v(j)> pt) j++;
			while(v(i)< pt) i--;
		}
		if(i>=j)
		{
			if(i != j)
			{
				double dTemp = v(i);
				v(i) = v(j);
				v(j) = dTemp;
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(low < i)   quicksort(v,low,i,bUp);
	if(j < high)  quicksort(v,j,high,bUp);

	return v;



}


//////////////////////////////////////////////////////////////////////
// CVector -------- MergeSort
//////////////////////////////////////////////////////////////////////
//归并排序，升序
//#_S 2005-5-25 yzm

//用于归并排序中的qsort函数

int int_compareup(const void *arg1,const void *arg2 )
{
        if (*(int*)arg1>*(int*)arg2)
                return 1;
        if (*(int*)arg1<*(int*)arg2)
                return -1;
        return 0;
}
/*
int int64_compareup(const void *arg1,const void *arg2 )
{
        if (*(__int64*)arg1>*(__int64*)arg2)
                return 1;
        if (*(__int64*)arg1<*(__int64*)arg2)
                return -1;
        return 0;
}*/

int int_comparedown(const void *arg1,const void *arg2 )
{
        if (*(int*)arg1<*(int*)arg2)
                return 1;
        if (*(int*)arg1>*(int*)arg2)
                return -1;
        return 0;
}
/*
int int64_comparedown(const void *arg1,const void *arg2 )
{
        if (*(__int64*)arg1<*(__int64*)arg2)
                return 1;
        if (*(__int64*)arg1>*(__int64*)arg2)
                return -1;
        return 0;
}
*/
template <class Type>
void m_intqSort(void *base,size_t num,size_t width, bool bUp)
{
    if (sizeof(Type) == sizeof(int)){
	  if (bUp)
		qsort(base,num,width,int_compareup);
	  else
        qsort(base,num,width,int_comparedown);
	}
	else if (sizeof(Type) == sizeof(__int64)){
	  if (bUp)
		qsort(base,num,width,int64_compareup);
	  else
        qsort(base,num,width,int64_comparedown);
	}
}

int float_compareup(const void *arg1,const void *arg2 )
{
        if (*( float* ) arg1>*( float* ) arg2)
                return 1;
        if (*( float* ) arg1<*( float* ) arg2)
                return -1;
        
        return 0;
}

int double_compareup(const void *arg1,const void *arg2 )
{
        if (*( double* ) arg1>*( double* ) arg2)
                return 1;
        if (*( double* ) arg1<*( double* ) arg2)
                return -1;
        
        return 0;
}

int float_comparedown(const void *arg1,const void *arg2 )
{
        if (*( float* ) arg1<*( float* ) arg2)
                return 1;
        if (*( float* ) arg1>*( float* ) arg2)
                return -1;
        
        return 0;
}

int double_comparedown(const void *arg1,const void *arg2 )
{
        if (*( double* ) arg1>*( double* ) arg2)
                return 1;
        if (*( double* ) arg1<*( double* ) arg2)
                return -1;
        
        return 0;
}


template <class Type>
void m_floatqSort(void *base,size_t num,size_t width, bool bUp)
{
    if (sizeof(Type) == sizeof(double)){
	  if (bUp)
		qsort(base,num,width,double_compareup);
	  else
        qsort(base,num,width,double_comparedown);
	}
	else if (sizeof(Type) == sizeof(float)){
	  if (bUp)
		qsort(base,num,width,float_compareup);
	  else
        qsort(base,num,width,float_comparedown);
	}
}

template <class Type>
CVector<Type> &templMergeSort(CVector<Type> &vec,int nValidBegin,int nValidCount,int nswitch, bool bUp)
{
	if (nValidBegin>=vec.vlen())  //yzmAdd nValidBegin 为归并排序起点，必须小于长度
	    return vec;
	if (nValidBegin + nValidCount>=vec.vlen()) //yzmAdd nValidCount做为规定排序长度，可以小于等于vec.vlen()
		nValidCount = vec.vlen() - nValidBegin;
	
    
	int nBufsize = 10000*50;
	
    Type *pBuf = new Type[nBufsize];
	
	CVector<Type> ColA;
	CVector<Type> ColB;
	
	CVector<Type>*pIn = &ColB;
	CVector<Type>*pOut = &ColA;
	int nCurPos = nValidBegin;  // 排序起点
	while(nCurPos<nValidCount)
	{
		int nStartPos = nCurPos;
		int nEndPos = nStartPos+nBufsize;
		if (nEndPos >= nValidCount)
			nEndPos = nValidCount;
		
		for (int i= nStartPos ;i<nEndPos;i++)
			*(pBuf+i-nStartPos) = vec(i);
		switch (nswitch){
		case 0:
			m_intqSort<Type>(pBuf,nEndPos - nStartPos,sizeof(Type),bUp);
			break;
		case 1:
			m_floatqSort<Type>(pBuf,nEndPos - nStartPos,sizeof(Type),bUp);
			break;
		default:
			assert(false);
		}
		//               qsort(pBuf,nEndPos - nStartPos,sizeof(Type),compare);
		
		//归并
		int nSizeA = nEndPos - nStartPos;
		int nSizeB = pIn->vlen();
		int nOut =0;
		if (nSizeB==0)
		{
			pOut->create(nSizeA,0);
			for (int i=0;i<nSizeA ;i++)
				(*pOut)(nOut++) = *(pBuf+i);
		}
		else
		{
			pOut->create(nSizeA+nSizeB,0);
			int nA = 0;
			int nB = 0;
			while (nA<nSizeA || nB <nSizeB)
			{
				if (nA >=nSizeA )
				{
					(*pOut)(nOut++) = (*pIn)(nB);
					nB++;
					continue;
				}
				if (nB >=nSizeB)
				{
					(*pOut)(nOut++) = *(pBuf+nA);
					nA++;
					continue;
				}
				
				if ((*pIn)(nB) > *(pBuf+nA))
				{
					(*pOut)(nOut++) = *(pBuf+nA);
					nA++;
				}
				else
				{
					(*pOut)(nOut++) = (*pIn)(nB);
					nB++;
				}
			}
			
		}
		
		CVector<Type>*pTemp = pOut;
		pOut = pIn;
		pIn = pTemp;
		//***********************************
		nCurPos = nEndPos;//更新当前位置
	}
	
	//复制数据 //#_S 2005-5-17 zlq
	for (int i=nValidBegin;i<nValidCount;i++) //yzmModify 从排序起点开始排序
	{
		vec(i)=(*pIn)(i);
	}
	delete [] pBuf;
	pBuf = NULL;
	return vec;
}
/*
CIntVector &MergeSort(CIntVector &v, bool bUp)
{
   return (CIntVector)templMergeSort(v,0,v.vlen(),0,bUp);
}

CDoubleVector &MergeSort(CDoubleVector &v, bool bUp)
{
	return (CDoubleVector)templMergeSort(v,0,v.vlen(),1,bUp);
}*/


double cov(const CDoubleVector &v1, const CDoubleVector &v2)
{
	double mv1 = mean(v1);
	double mv2 = mean(v2);
	int len = v1.vlen();
	if(len<=1)
		return 0;
	else
	    return sum((v1-mv1)*(v2-mv2))/(len-1);
}
CDoubleVector SimpleRegress(const CDoubleVector Y , const CDoubleVector X) //简单线性回归，返回参数：常数，系数
{
	int n = Y.vlen();
	CDoubleVector Vect_Ret(3);
	assert( n > 3 && X.vlen() == n);
	double SXX = var(X)*(n-1);
	double SYY = var(Y)*(n-1);
	double SXY = cov(X,Y)*(n-1);

	assert(SXX >0);
	Vect_Ret[1] = SXY/SXX; //系数
	Vect_Ret[0] = mean(Y) - Vect_Ret[1] * mean(X); //常数项
	Vect_Ret[2] = SXY/sqrt(SXX*SYY); //样本相关系数
	
	return Vect_Ret;
}
double corrcoef(const CDoubleVector &v1, const CDoubleVector &v2)
{
	if(cov(v1,v1)==0 || cov(v2,v2)==0)
	{
		ErrorHandler(EMAT_STATCONST, __FILE__, __LINE__);
	}
	return cov(v1, v2)/sqrt(cov(v1,v1)*cov(v2,v2));
}

/*
double var(CDoubleVector &v)
{
	double mv = mean(v);
	int len = v.vlen();
	if(len<=1)
		return 0;
	else
	    return sum((v-mv)*(v-mv))/(len-1);
}
*/
double var(const CDoubleVector &v) //zfm
{
	int i;
	double s=0.0,sum=0.0;
	int len=v.vlen();
//	assert(len>1);
//  yzmModify 用assert的话无法防止release版本下出现除0错
	if (len>1){
		for(i=0; i<len; i++)
		{
			s+=v.GetValue(i)*v.GetValue(i);
			sum+=v.GetValue(i);
		}
		return (s-sum*sum/len)/(len-1);
    }
	else
		return 0;
}
	
double CDoubleVector::var()const 
{
	int i;
	double s=0.0,sum=0.0;
	int len=vlen();
//	assert(len>1);
//  yzmModify 用assert的话无法防止release版本下出现除0错
	if (len>1){		
		double re = 0;
		CAllocate<double> *f = p->elem;
		
		for(i=0; i<len; i++)
		{
			s+=((*f)(i))*((*f)(i));
			sum+=((*f)(i));
		}
		return (s-sum*sum/len)/(len-1);
	}
	else
		return 0;
}


double var_w(const CIntVector &v, const CDoubleVector &w)
{

	int len=v.vlen();
	int w_len=w.vlen();
	double s=0.0;
	double sw=0.0;
	double s1=0.0;
	double sm=0.0; 
	assert((len==w_len)&&(w_len>1));
	
	
	for(int i=0; i<len; i++)
	{
		s+=v.GetValue(i) * v.GetValue(i) * w.GetValue(i);
		sw +=w.GetValue(i);
		s1+=v.GetValue(i);
		sm+=v.GetValue(i) * w.GetValue(i);
	}
	
	return (s-(sm*sm*sw)/(sw*sw)) / ( sw-1);   
}


double var_w(const CDoubleVector &v, const CIntVector &w)
{

	int len=v.vlen();
	int w_len=w.vlen();
	double s=0.0;
	double sw=0.0;
	double s1=0.0;
	double sm=0.0; 
	assert((len==w_len)&&(w_len>1));
	
	
	for(int i=0; i<len; i++)
	{
		s+=v.GetValue(i) * v.GetValue(i) * w.GetValue(i);
		sw +=w.GetValue(i);
		s1+=v.GetValue(i);
		sm+=v.GetValue(i) * w.GetValue(i);
	}
	
	return (s-(sm*sm*sw)/(sw*sw)) / ( sw-1);   
}



double CIntVector::var_w(const CDoubleVector &w)const 
{

	int len=vlen();
	int w_len=w.vlen();
	double s=0.0;
	double sw=0.0;
	double s1=0.0;
	double sm=0.0; 
	assert((len==w_len)&&(w_len>1));
	
	
	for(int i=0; i<len; i++)
	{
		s+=(*this).GetValue(i) * (*this).GetValue(i) * w.GetValue(i);
		sw +=w.GetValue(i);
		s1+=(*this).GetValue(i);
		sm+=(*this).GetValue(i) * w.GetValue(i);
	}
	
	return (s-(sm*sm*sw)/(sw*sw)) / ( sw-1);   
}

double CDoubleVector::var_w(const CIntVector &w)const 
{

	int len=vlen();
	int w_len=w.vlen();
	double s=0.0;
	double sw=0.0;
	double s1=0.0;
	double sm=0.0; 
	assert((len==w_len)&&(w_len>1));
	
	
	for(int i=0; i<len; i++)
	{
		s+=(*this).GetValue(i) * (*this).GetValue(i) * w.GetValue(i);
		sw +=w.GetValue(i);
		s1+=(*this).GetValue(i);
		sm+=(*this).GetValue(i) * w.GetValue(i);
	}
	
	return (s-(sm*sm*sw)/(sw*sw)) / ( sw-1);   
}



double st_d(const CDoubleVector &v)
{
	return sqrt(var(v));
}

CDoubleVector normcdf(const CDoubleVector &vec, double mu, double sigma)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = normcdf(vec.GetValue(i), mu, sigma);
	return vv;
}

CDoubleVector chi2cdf(const CDoubleVector &vec, int v)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = chi2cdf(vec.GetValue(i), v);
	return vv;

}

CDoubleVector fcdf(const CDoubleVector &vec, int v1, int v2)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = fcdf(vec.GetValue(i), v1, v2);
	return vv;
}

CDoubleVector tcdf(const CDoubleVector &vec, int v)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = tcdf(vec.GetValue(i), v);
	return vv;
}


CDoubleVector normpdf(const CDoubleVector &vec, double mu, double sigma)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = normpdf(vec.GetValue(i), mu, sigma);
	return vv;
}

CDoubleVector chi2pdf(const CDoubleVector &vec, int v)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = chi2pdf(vec.GetValue(i), v);
	return vv;
}

CDoubleVector fpdf(const CDoubleVector &vec, int v1, int v2)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = fpdf(vec.GetValue(i), v1, v2);
	return vv;
}


CDoubleVector tpdf(const CDoubleVector &vec, int v)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = tpdf(vec.GetValue(i), v);
	return vv;
}



CDoubleVector norminv(const CDoubleVector &vec, double mu, double sigma)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = norminv(vec.GetValue(i), mu, sigma);
	return vv;
}

CDoubleVector chi2inv(const CDoubleVector &vec, int v)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = chi2inv(vec.GetValue(i), v);
	return vv;

}

CDoubleVector finv(const CDoubleVector &vec, int v1, int v2)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = finv(vec.GetValue(i), v1, v2);
	return vv;
}

CDoubleVector tinv(const CDoubleVector &vec, int v)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = tinv(vec.GetValue(i), v);
	return vv;
}


CDoubleVector fcdf(const CDoubleVector &vec, const CIntVector &v1, const CIntVector &v2)
{
	int lens = vec.vlen();
	CDoubleVector vv(lens);
	for(int i=0; i<lens; i++)
		vv(i) = fcdf(vec.GetValue(i), v1.GetValue(i), v2.GetValue(i));
	return vv;
}


/*
CIntVector grep2idx(CIntVector &v)	
{
	int lens = v.vlen();
    CIntVector rr(lens);
    for(int i=0; i<lens; i++)
		v(i)=i
	return rr;
}
*/




//////////////////////////////////////////////////////////////////////
// CComplexVector -------- Construction/Destruction
//////////////////////////////////////////////////////////////////////
CComplexVector::CComplexVector(const CDoubleVector &re, const CDoubleVector &im):CVector<CComplex>(MIN(re.vlen(), im.vlen()))
{
	int len=MIN(re.vlen(), im.vlen());
	for(int i=0; i<len; i++)
		(*this)(i) = CComplex(re.GetValue(i),im.GetValue(i));	
}

CComplexVector & CComplexVector::operator = (const CComplexVector &vec)
{
	this->CVector<CComplex>::operator =(vec);
	return *this;
}

CComplexVector & CComplexVector::operator =(const CVector<CComplex> &vec)
{
	this->CVector<CComplex>::operator =(vec);
	return *this;
}

CComplexVector & CComplexVector::operator =(const CDoubleVector &vec)
{
	*this = dvectocvec(vec);
	return * this;
}

CComplexVector dvectocvec(const CDoubleVector &vec)
{
	int len = vec.vlen();
	CComplexVector v(len);
	for(int i=0; i<len; i++)
		v(i) = CComplex(vec.GetValue(i),0);
	return v;
}

CComplexVector ivectocvec(const CIntVector &vec)
{
	int len = vec.vlen();
	CComplexVector v(len);
	for(int i=0; i<len; i++)
		v(i) = CComplex(vec.GetValue(i),0);
	return v;
}

CComplexVector fvectocvec(const CFloatVector &vec)
{
	int len = vec.vlen();
	CComplexVector v(len);
	for(int i=0; i<len; i++)
		v(i) = CComplex(vec.GetValue(i),0);
	return v;
}

CDoubleVector real(const CComplexVector &c)
{
	int n=c.vlen();
	CDoubleVector v(n);
	for(int i=0; i<n; i++)
		v(i) = real(c.GetValue(i));
	return v;
}

CDoubleVector imag(const CComplexVector &c)
{
	int n=c.vlen();
	CDoubleVector v(n);
	for(int i=0; i<n; i++)
		v(i) = imag(c.GetValue(i));
	return v;
}

CComplexVector conj(const CComplexVector &c)
{
	int n=c.vlen();
	CComplexVector v(n);
	for(int i=0; i<n; i++)
		v(i) = conj(c.GetValue(i));
	return v;
}

CDoubleVector CComplexVector::real()const 
{
	int n=vlen();
	CDoubleVector v(n);
	for(int i=0; i<n; i++)
		v(i) = (*this).GetValue(i).real();
	return v;
}

CDoubleVector CComplexVector::imag()const 
{
	int n=vlen();
	CDoubleVector v(n);
	for(int i=0; i<n; i++)
		v(i) = (*this).GetValue(i).imag();
	return v;
}

CComplexVector CComplexVector::conj()const 
{
	int n=vlen();
	CComplexVector v(n);
	for(int i=0; i<n; i++)
		v(i) = (*this).GetValue(i).conj();
		
	return v;
}

CComplexVector Complex(const CDoubleVector &v1, const CDoubleVector &v2)
{
	int len = MIN(v1.vlen(), v2.vlen());
	CComplexVector v(len);
	for(int i=0; i<len; i++)
		v(i) = CComplex(v1.GetValue(i), v2.GetValue(i));
	return v;
}

CComplexVector Complex(const CDoubleVector &v1, double x2)
{
	int len = v1.vlen();
	CComplexVector v(len);
	for(int i=0; i<len; i++)
		v(i) = CComplex(v1.GetValue(i), x2);
	return v;
}

CComplexVector Complex(double x1, const CDoubleVector &v2)
{
	int len = v2.vlen();
	CComplexVector v(len);
	for(int i=0; i<len; i++)
		v(i) = CComplex(x1, v2.GetValue(i));
	return v;
}

CComplex CComplexVector::dot(const CComplexVector &v2)const 
{
	int len = MIN(vlen(), v2.vlen());

	CComplex sum = 0;
	for(int i=0; i<len; i++)
		sum += v2.GetValue(i) * (*this).GetValue(i).conj();
	return sum;
}

CComplex dot(const CComplexVector &v1, const CComplexVector &v2)
{
	int len = MIN(v1.vlen(), v2.vlen());

	CComplex sum = 0;
	for(int i=0; i<len; i++)
		sum += v2.GetValue(i) * conj(v1.GetValue(i));
	return sum;
}






int any(const CComplexVector &v)
{
	int n=v.vlen();
	int re = 0;
	for(int i=0; i<n; i++)
		if(::abs(v.GetValue(i)) !=0)
		{
			re=1;
			break;
		}
	return re;
}

int CComplexVector::any()const 
{
	int n=vlen();
	int re = 0;
	for(int i=0; i<n; i++)
		if(::abs((*this).GetValue(i)) !=0)
		{
			re=1;
			break;
		}
	return re;
}

CComplex Max(const CComplexVector &v)
{
	CComplex xx;
	int nn = v.vlen();
	xx = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if(::abs(v.GetValue(i))>::abs(xx))
			xx = v.GetValue(i);
	return xx;
}

CComplex CComplexVector::Max()const 
{
	CComplex xx;
	int nn = vlen();
	xx = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if(::abs((*this).GetValue(i))>::abs(xx))
			xx = (*this).GetValue(i);
	return xx;
}

CComplexVector Max(const CComplexVector &v1, const CComplexVector &v2)
{
	int nn=MIN(v1.vlen(), v2.vlen());
	CComplexVector xx(nn);
	for(int i=1; i<nn; i++)
		if(::abs(v1.GetValue(i))>::abs(v2.GetValue(i)))
			xx(i) = v1.GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}

CComplexVector CComplexVector::Max(const CComplexVector &v2)const 
{
	int nn=MIN(vlen(), v2.vlen());
	CComplexVector xx(nn);
	for(int i=1; i<nn; i++)
		if(::abs((*this).GetValue(i))>::abs(v2.GetValue(i)))
			xx(i) = (*this).GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}


CComplex Min(const CComplexVector &v)
{
	CComplex xx;
	int nn = v.vlen();
	xx = v.GetValue(0);
	for(int i=1; i<nn; i++)
		if((double)::abs(v.GetValue(i))<(double)::abs(xx))
			xx = v.GetValue(i);
	return xx;
}

CComplex CComplexVector::Min()const 
{
	CComplex xx;
	int nn = vlen();
	xx = (*this).GetValue(0);
	for(int i=1; i<nn; i++)
		if((double)::abs((*this).GetValue(i))<(double)::abs(xx))
			xx = (*this).GetValue(i);
	return xx;
}


CComplexVector Min(const CComplexVector &v1, const CComplexVector &v2)
{
	int nn=MIN(v1.vlen(), v2.vlen());
	CComplexVector xx(nn);
	for(int i=1; i<nn; i++)
		if((double)::abs(v1.GetValue(i))<(double)::abs(v2.GetValue(i)))
			xx(i) = v1.GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}

CComplexVector CComplexVector::Min(const CComplexVector &v2)const 
{
	int nn=MIN(vlen(), v2.vlen());
	CComplexVector xx(nn);
	for(int i=1; i<nn; i++)
		if((double)::abs((*this).GetValue(i))<(double)::abs(v2.GetValue(i)))
			xx(i) = (*this).GetValue(i);
		else
			xx(i) = v2.GetValue(i);
	return xx;
}

CDoubleVector abs(const CComplexVector &v)
{
    int nn=v.vlen();
	CDoubleVector xx(nn);
	for(int i=0; i<nn; i++)
		xx(i) = ::abs(v.GetValue(i));
	return xx;
}

CDoubleVector CComplexVector::abs()const 
{
    int nn=vlen();
	CDoubleVector xx(nn);
	for(int i=0; i<nn; i++)
		xx(i) = ::abs((*this).GetValue(i));
	return xx;
}


double norm(const CComplexVector &v, int n)
{
	int nn=v.vlen();
	double xx;
	CDoubleVector temp(nn);
	temp = ::abs(v);
	if(n<0)
		xx = ::Min(temp);
	else
	{
		if(n==0)
			xx = ::Max(temp);
		else
			xx = ::pow(sum(pow(temp,n)),1.0/n);
	}
	return xx;
}

double CComplexVector::norm(int n)const 
{
	int nn=vlen();
	double xx;
	CDoubleVector temp(nn);
	temp = abs();
	if(n<0)
		xx = ::Min(temp);
	else
	{
		if(n==0)
			xx = ::Max(temp);
		else
			xx = ::pow(::sum(::pow(temp,n)),1.0/n);
	}
	return xx;
}

CComplexVector pow(const CComplexVector &v, CComplex pp)
{
	int nn=v.vlen();
	CComplexVector xx(nn);
	for(int i=0; i<nn; i++)
		xx(i) = ::pow(v.GetValue(i),pp);
	return xx;
}

CComplexVector & CComplexVector::pow(CComplex pp)
{
	int nn=vlen();
	for(int i=0; i<nn; i++)
		(*this)(i) = ::pow((*this).GetValue(i),pp);
	return (*this);
}

CComplexVector pow(const CComplexVector &x, const CComplexVector &pp)
{
	int lens = x.vlen();
	int lens1 = pp.vlen();
	if(lens != lens1)
	{
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
        lens = MIN(x.vlen(), pp.vlen());
	}
    CComplexVector v(lens);
	for(int i=0; i<lens; i++)
		v(i) = ::pow(x.GetValue(i), pp.GetValue(i));
	return v;
}

CComplexVector & CComplexVector::pow(const CComplexVector &pp)
{
	int lens = vlen();
	int lens1 = pp.vlen();
	if(lens != lens1)
	{
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
        lens = MIN(vlen(), pp.vlen());
	}
	for(int i=0; i<lens; i++)
		(*this)(i) = ::pow((*this).GetValue(i), pp.GetValue(i));
	return (*this);
}

CComplexVector & CComplexVector::Rand()
{
	int nr = vlen();
	for(int i=0; i<nr; i++)
		(*this)(i) = CComplex((double)::rand()/(double)RAND_MAX, (double)::rand()/(double)RAND_MAX);
	return *this;
}

CComplexVector & Rand(CComplexVector &v)
{
	int nr = v.vlen();
	for(int i=0; i<nr; i++)
		v(i) = CComplex((double)::rand()/(double)RAND_MAX, (double)::rand()/(double)RAND_MAX);
	return v;
}

CComplexVector CComplexVector::Rand(int m)const 
{
	CComplexVector v(m);
	for(int i=0; i<m; i++)
		v(i) = CComplex((double)::rand()/(double)RAND_MAX, (double)::rand()/(double)RAND_MAX);
	return v;
}

CComplexVector & CComplexVector::Sort(bool select)
{
	int n=vlen();
	int i, j, m, k;
	CComplex d;
	k=0;
	m = n-1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if(::abs((*this)(i))>::abs((*this)(i+1)))
				{
					d =(*this)(i);
					(*this)(i)=(*this)(i+1);
					(*this)(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if(::abs((*this)(i-1))>::abs((*this)(i)))
				{
					d = (*this)(i);
					(*this)(i) =(*this)(i-1);
					(*this)(i-1) = d;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((double)::abs((*this)(i))<(double)::abs((*this)(i+1)))
				{
					d =(*this)(i);
					(*this)(i)=(*this)(i+1);
					(*this)(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((double)::abs((*this)(i-1))<(double)::abs((*this)(i)))
				{
					d = (*this)(i);
					(*this)(i) =(*this)(i-1);
					(*this)(i-1) = d;
					k=i;
				}
		}
	}
    return *this;
}


CComplexVector Sort(CComplexVector &a, bool select)
{
	int n=a.vlen();
	CComplexVector p(n);
	p = copy(a);
	int i, j, m, k;
	CComplex d;
	k=0;
	m = n-1;
	if(select == 0)
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((double)::abs(p(i))>(double)::abs(p(i+1)))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((double)::abs(p(i-1))>(double)::abs(p(i)))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					k=i;
				}
		}
	}
	else
	{
		while(k<m)
		{
			j=m-1;
			m=0;
			for(i=k;i<=j; i++)
				if((double)::abs(p(i))<(double)::abs(p(i+1)))
				{
					d =p(i);
					p(i)=p(i+1);
					p(i+1)=d;
					m=i;
				}
			j=k+1;
			k=0;
			for(i=m;i>=j;i--)
				if((double)::abs(p(i-1))<(double)::abs(p(i)))
				{
					d = p(i);
					p(i) =p(i-1);
					p(i-1) = d;
					k=i;
				}
		}
	}
    return p;
}




