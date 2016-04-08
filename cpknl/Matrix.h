// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__B3A91F5A_0C4C_4EEA_A287_3BE1014B64CA__INCLUDED_)
#define AFX_MATRIX_H__B3A91F5A_0C4C_4EEA_A287_3BE1014B64CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ErrorReporter.h"
////////////////////////////////////////////////////////////////////////////////
//////////////////  ----- CMatrix --------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


template <class Type>
class CSymMatrix;

template <class Type>
class CSparseMatrix;

template <class Type>
class CTriangleMatrix;
template <class Type>
class CDiagMatrix;


template <class Type>
class AFX_EXT_CLASS CMatrix
{
protected:
	struct mrep
	{
		CVector<Type> **	elem;
		int					refcnt;
		int					length;
		mrep()
		{
			refcnt = 1;
			length = 0;
			elem = NULL;
		}
	}*p;
	void ZeroVectorPtrs(int xsize);
public:
	//constructor
	CMatrix(int init=0)
	{
		//	MatrixInit();
		p = NULL;
		create(init);
	}

	CMatrix<Type>(Type *a, int xlength, int ylength)
	{
		//	MatrixInit();
		p = NULL;
		create(a,xlength,ylength);
	}
	
	CMatrix(int xsize, int ysize, Type init=0)
	{
		//	MatrixInit();
		p = NULL;
		create(xsize,ysize,init);
		
	}
	

	CMatrix(Type **a, int xlength, int ylength)
	{
		//	MatrixInit();
		p = NULL;
		create(a,xlength,ylength);
	}
	
	CMatrix(const CMatrix &a, int xlength, int ylength)
	{
		//	MatrixInit();
		p = NULL;
		create(a,xlength,ylength);
	}
	
	CMatrix(const CMatrix &a1, const CMatrix &a2, int select = 2)
	{
		//	MatrixInit();
		p=NULL;
		create(a1,a2,select);
	}
	
	
    CMatrix(const CMatrix &a, int xfirst, int xlast, int yfirst, int ylast)
	{
		//	MatrixInit();
		p = NULL;
		create(a,xfirst,xlast,yfirst,ylast);
	}
	
	CMatrix(const CMatrix &x)
	{
		//	MatrixInit();
		p = x.p;
		p->refcnt++; 
	}
	
	CMatrix<Type>(const CVector<Type> &x)
	{
		//	MatrixInit();
		p = NULL;
		create(x);
	}
	
    CMatrix<Type>(const CSymMatrix<Type> &x)
	{
		//	MatrixInit();
		p = NULL;
		create(x);
	}
	
	CMatrix<Type>(const CSparseMatrix<Type> &x)
	{
		//	MatrixInit();
		p = NULL;
		create(x);
		
	}
	
	CMatrix<Type>(const CTriangleMatrix<Type> &x)
	{
		//	MatrixInit();
		p = NULL;
		create(x);
	}
	
	//create
	bool create(int init=0)
	{
		destroy();
		p = new mrep;
		p->elem = NULL;
		p->length = 0;
		p->refcnt = 1;
		return true;
	}
	
	bool create(Type *a, int xlength, int ylength)
	{
		destroy();
		if((xlength<0)||(ylength<0))
		{
			ErrorHandler(EMAT_INVALIDSIZE, __FILE__, __LINE__);
			xlength =0;
			ylength =0;
		}
		int tmp = UINT_MAX/sizeof(Type);
		if((xlength>=(UINT_MAX/4)||ylength>=tmp))
		{
			ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
			xlength =0;
			ylength =0;
		}
		p = new mrep;
		p->refcnt = 1;
		
		p -> length = ylength;
		p ->elem = new CVector<Type> *[ylength];
		for(int i=0; i<ylength; i++)
		{
			p->elem[i]=new CVector<Type>(xlength);
			for(int j = 0;j<xlength;j++)
			{
				(*(p->elem[i]))(j) = a[j*ylength + i];
			}
		}
		return true;
	}

	bool create(int xsize, int ysize, Type init=0)
	{
		destroy();

		if((xsize<0)||(ysize<0))
		{
			ErrorHandler(EMAT_INVALIDSIZE, __FILE__, __LINE__);
			xsize =0;
			ysize =0;
		}
		int tmp = UINT_MAX/sizeof(Type);
		if((xsize>=(UINT_MAX/4)||ysize>=tmp))
		{
			ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
			xsize =0;
			ysize =0;
		}
		p = new mrep;
		p->length = ysize;

		p ->elem = new CVector<Type> *[ysize];
		if(ysize>0)
		{
			for(int i=0; i<ysize; i++)
			{
				p->elem[i] = new CVector<Type>(xsize);
			}
		}
		for (int i=0;i<ysize;i++)
			for (int j=0;j<xsize;j++)
				(*this)(j,i) = init;
		p->refcnt = 1;
		return true;	
	}

	bool create(Type **a, int xlength, int ylength)
	{
		destroy();
		if((xlength<0)||(ylength<0))
		{
			ErrorHandler(EMAT_INVALIDSIZE, __FILE__, __LINE__);
			xlength =0;
			ylength =0;
		}
		int tmp = UINT_MAX/sizeof(Type);
		if((xlength>=(UINT_MAX/4)||ylength>=tmp))
		{
			ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
			xlength =0;
			ylength =0;
		}
		p = new mrep;
		p->refcnt = 1;
		
		p -> length = ylength;
		p ->elem = new CVector<Type> *[ylength];
		for(int i=0; i<ylength; i++)
		{	
			p->elem[i] = new CVector<Type>(xlength);
			for(int j = 0;j<xlength;j++)
			{	
				(*(p->elem[i]))(j) = a[i][j]; 
			}
		}
		return true;
	}

	bool create(const CMatrix &a, int xlength, int ylength)
	{
		destroy();
		if((xlength<0)||(ylength<0))
		{
			ErrorHandler(EMAT_INVALIDSIZE, __FILE__, __LINE__);
			xlength =0;
			ylength =0;
		}
		int tmp = UINT_MAX/sizeof(Type);
		if((xlength>=(UINT_MAX/4)||ylength>=tmp))
		{
			ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
			xlength =0;
			ylength =0;
		}
		p = new mrep;
		p->refcnt = 1;
		
		p -> length = ylength;
		p ->elem = new CVector<Type> *[ylength];
		for(int i=0; i<ylength; i++)
		{	
			p->elem[i] = new CVector<Type>(xlength);
			for(int j = 0;j<xlength;j++)
			{	
				if ((j < a.mrow())&&(i<a.mcol()))
					(*(p->elem[i]))(j) = a.GetValue(i,j); 
				else
					(*(p->elem[i]))(j) = 0; 
			}
		}
		return true;
	}

	bool create(const CMatrix &a1, const CMatrix &a2, int select = 2)
	{
		destroy();

		int xlength, ylength, minc, ninc, i,j;
		int mrow1 = a1.mrow();
		int mrow2 = a2.mrow();
		int mcol1 = a1.mcol();
		int mcol2 = a2.mcol();
		switch(select)
		{
		case 0:	//Eric 0108 comment ^
			xlength = mrow1 + mrow2;
			ylength = mcol1 + mcol2;
			minc = mrow1;
			ninc = mcol1;
			break;
		case 1:	//Eric 0108 comment |
			xlength = MAX(mrow1, mrow2);
			ylength = mcol1 + mcol2;
			minc =0;
			ninc = mcol1;
			break;
		case 2:	//Eric 0108 comment &
			xlength = mrow1 + mrow2;
			ylength = MAX(mcol1,mcol2);
			minc = mrow1;
			ninc =0;
			break;
		default:
			ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
		}
		int tmp = UINT_MAX/sizeof(Type);
		if((xlength>=UINT_MAX/4)||(ylength>=tmp))
		{
			ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__, __LINE__);
			xlength =0;
			ylength =0;
		}

		p = new mrep;
		p->refcnt = 1;
		
		p->length = ylength;
		p->elem = new CVector<Type> * [ylength];
		for(i=0;i<ylength; i++)
		{
			p->elem[i]=new CVector<Type>(xlength);
		}
		for(i=0;i<mrow1;i++)
			for(j=0;j<mcol1;j++)
			{ 
				(*(p->elem[j]))(i)=a1.GetValue(i,j);
			}
		for(i=0;i<mrow2;i++)
			for(j=0;j<mcol2;j++)
				(*(p->elem[j+ninc]))(i+minc)=a2.GetValue(i,j);
		return true;
	}

    bool create(const CMatrix &a, int xfirst, int xlast, int yfirst, int ylast)
	{
		destroy();
		int xsize = xlast - xfirst;
		int ysize = ylast - yfirst;

		if((xsize<0)||(ysize<0))
		{
			ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
			xsize =0;
			ysize =0;
		}
		if((xfirst<0)||(yfirst<0)||xlast>a.mrow()||ylast>a.mcol())
		{
			ErrorHandler(EMAT_OUTOFRANGE, __FILE__,__LINE__);
			xsize =0;
			ysize =0;
		}

		p = new mrep;
		p->refcnt = 1;
		
		p->length = ysize;
		p->elem = new CVector<Type> * [ysize];
		for(int i=0; i<ysize; i++)
		{
			p->elem[i]=new CVector<Type>(ysize);
			for(int j = 0;j<xsize;j++)
			{
				(*(p->elem[i]))(j) = a.GetValue(xfirst+i,j);//uc
			}
		}
		return true;
	}

	bool create(const CMatrix &x)
	{
		destroy();
		x.p->refcnt++;
		p = x.p;
		return true;
	}
	
	bool create(const CVector<Type> &x)
	{
		destroy();
		
		p = new mrep;
		p->length = 1;
		p->elem = new CVector<Type> * [1];
		
		p->elem[0]= new CVector<Type>(x);
		p->refcnt = 1;
		return true;
	}
	
    bool create(const CSymMatrix<Type> &x)
	{
		destroy();
		int xsize=x.mrow();
		int ysize=x.mcol();
		int ven = x.vlen();
		int k=0;
		int tmp = UINT_MAX/sizeof(Type);
		if((xsize<0)||(ysize<0))
		{
			ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
			xsize =0;
			ysize =0;
		}
		if((xsize>=(UINT_MAX/4)||ysize>=tmp))
		{
			ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
			xsize =0;
			ysize =0;
		}
		p = new mrep;
		p->length = ysize;
		p->refcnt = 1;
		
		p->elem = new CVector<Type> * [ysize];
		int i=0;
		for(i=0; i<ysize; i++)
		{
			p->elem[i]=new CVector<Type>(xsize);
		}
		
		while(k<ven)
		{
			for(int i=0; i<ysize; i++)
	    		for(int j=0; j<ysize-i; j++)
				{				
					(*this)(j,j+i) = x.GetValue(k);
					k++;
				}
		}
		for( i=1; i<ysize; i++)
			for(int j=0; j<i; j++)
				(*this)(i,j) = (*this)(j,i);
		return true;
	}

	bool create(const CSparseMatrix<Type> &x)
	{
		destroy();
		int xsize=x.mrow();
		int ysize=x.mcol();
		int k=0;
		int tmp = UINT_MAX/sizeof(Type);
		if((xsize<0)||(ysize<0))
		{
			ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
			xsize =0;
			ysize =0;
		}
		if((xsize>=(UINT_MAX/4)||ysize>=tmp))
		{
			ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
			xsize =0;
			ysize =0;
		}
		p = new mrep;
		p->refcnt = 1;
		
		p->length = ysize;
		p->elem = new CVector<Type> * [ysize];
		int i=0;
		for(i=0; i<ysize; i++)
		{
			p->elem[i]=new CVector<Type>(xsize);
		}

		for(i=0; i<x.vlen(); i++)
		   (*this)(x.GetValue(i).gxpos(), x.GetValue(i).gypos()) = x.GetValue(i).gvalue();
		return true;
	}
	bool create(const CTriangleMatrix<Type> &x)
	{
		destroy();
		int xsize=x.mrow();
		int ysize=x.mcol();
		int ven = x.vlen();
		int k=0;
		int tmp = UINT_MAX/sizeof(Type);
		if((xsize<0)||(ysize<0))
		{
			ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
			xsize =0;
			ysize =0;
		}
		if((xsize>=(UINT_MAX/4)||ysize>=tmp))
		{
			ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
			xsize =0;
			ysize =0;
		}
		p = new mrep;
		p->refcnt = 1;
		
		p->length = ysize;
		p->elem = new CVector<Type> * [ysize];
		int i=0;
		for(i=0; i<ysize; i++)
		{
			p->elem[i]=new CVector<Type>(xsize);
		}

		while(k<ven)
		{
			for(int i=0; i<ysize; i++)
	    		for(int j=0; j<ysize-i; j++)
				{				
					(*this)(j,j+i) = x.GetValue(k);
					k++;
				}
		}
		return true;
	}

	bool isempty()
	{
		if (mrow()*mcol() ==0)
			return true;
		return false;
	}

	void destruction()//直接释放一个矩阵空间	
	{
		CMatrix<Type> M;
		*this=M;
	}
	
	//destructor
	~CMatrix()
	{
		destroy();
	}
	
	bool  destroy()
	{
		if (p!=NULL)
		{
			if(--p->refcnt ==0)
			{
				if (p->elem != NULL)
				{
					int len = p->length;
					for(int i=0; i<len; i++)
					{
						delete p->elem[i]; 
					}
					delete [](p->elem);
					p->elem = NULL;
				}
				delete p;
			}
			p = NULL;
		}
		return true;
	}

	TEMPLAT_T friend void destruction(CMatrix<T_S> &m); //直接释放一个矩阵空间

	static void SetErrorHandler(CErrorReporter &er);
/*
	TEMPLAT_T friend int mrow(const CMatrix T_T &m)
	{
		if ((m.p!=NULL)&&(m.p->elem!=NULL))
			return m.p->elem[0]->vlen();
		else
			return 0;
	}

	TEMPLAT_T friend int mrow(const CMatrix T_T *m)
	{
		if ((m!=NULL)&&(m->p!=NULL)&&(m->p->elem!=NULL))
			return m->p->elem[0]->vlen();
		else
			return 0;
	}
	*/
	int mrow()const 
	{
		if ((p!=NULL)&&(p->elem!=NULL))
			return p->elem[0]->vlen();
		else
			return 0;
	}
/*
	TEMPLAT_T friend int mcol(const CMatrix T_T &m)
	{
		if((m.p!=NULL)&&(m.p->elem != NULL))
			return m.p->length; 
		else 
			return 0;
	}

	TEMPLAT_T friend int mcol(const CMatrix T_T *m)
	{
		if((m->p!=NULL)&&(m->p->elem != NULL)) return m->p->length; else return 0;
	}*/
	int mcol()const 
	{
		if((p!=NULL)&&(p->elem != NULL)) return p->length;
		else return 0;
	}

	TEMPLAT_T friend bool rerowsize(CMatrix T_T& m, int nrow);
	bool rerowsize(int nrow)
	{
		if ((nrow >= 0)&&(nrow <= mrow()))
		{
			for (int i=0;i<mcol();i++)
				(*this)(i).resize(nrow);
			return true;
		}
		else
			return false;
	}

	int  GetUsedMemory()				//得到已经使用的内存
	{
		return p->length * p->elem[0]->vlen() * sizeof(Type);
	}
	
	
	UINT GetUsableMemory()			//得到可以使用的系统内存
	{
#ifdef WINPLAT
		UINT nMemory;
		
		MEMORYSTATUS  memstat;
		GlobalMemoryStatus(&memstat);
		
		//	return 0;//小数据量测试用
		if(memstat.dwAvailPhys + memstat.dwAvailPageFile > MATRIX_RESERVE_BUF)
		{//协议一次使用两个矩阵.
			nMemory = (memstat.dwAvailPhys + memstat.dwAvailPageFile - MATRIX_RESERVE_BUF)/MATRIX_CO_USED;
			return nMemory;
		}
		else
		{
			return 0;
		}
#else
		return 0;
#endif
	}
	
	
	CVector<Type> & operator[](int i)////取矩阵的第i行 commeng by zkc
	{
		if((i>=0) &&(i<p->length))
		{
			return *p->elem[i];
		}
		else
		{
			ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
			return *p->elem[0];
		}
	}
	CVector<Type> & operator () (int i)//取矩阵的第i行 comment by zkc 
	{
		if((i>=0) &&(i<p->length))
		{
			return *p->elem[i];
		}
		else
		{
			ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
			return *p->elem[0];
		}
	}
	const CVector<Type> & GetValue(int i)const //取矩阵的第i行 comment by zkc 
	{
		if((i>=0) &&(i<p->length))
		{
			return *p->elem[i];
		}
		else
		{
			ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
			return *p->elem[0];
		}
	}	
public:
	CVector<Type> & GetVector(int i)
	{
		return (*this)(i);
	}

	const CVector<Type> & gGetVector(int i)const
	{
		return (*this).GetValue(i);
	}

	const Type & GetValue(int i,int j)const
	{
		return (*(p->elem[j]))(i);
		//return (*this)(i,j);
	}

	Type & operator () (int i, int j)
	{
		return (*(p->elem[j]))(i);
	}
	
	operator Type **()
	{
	}


	CMatrix & operator =(const CMatrix & mat)
	{
		destroy();        
		p = mat.p; 
		p->refcnt++;

		return *this;
	}

	CMatrix & operator =(const CVector<Type> & x)
	{
		if (p == NULL) 
			return *this;
		int len = p->length;
		int ven = p->elem[0]->vlen();
		if(p->refcnt > 1)
		{
			p->refcnt -- ;
			p = new mrep;	//p已经变换了!!
			p->length = len;
			p->refcnt = 1;
			p->elem = new CVector<Type> * [len];
			for(int i=0; i<len; i++)
			{
				p->elem[i] = (CVector<Type> *)new CVector<Type>(ven);
				for (int j=0;j<ven;j++)
					(*(p->elem[i]))(j) = x.GetValue(j);
			}
		}
		else if (p->refcnt == 1)
		{
			for(int i=0; i<len; i++)
			{
				for (int j=0;j<ven;j++)
					(*(p->elem[i]))(j) = x.GetValue(j);
			}
		}
		return *this;
	}

	CMatrix & operator =(Type x)
	{
		if (p == NULL)
			return *this;
		int len = p->length;
		int ven = p->elem[0]->vlen();
		if(p->refcnt >1)
		{
			p->refcnt -- ;
			p = new mrep;
			p->length = len;
			p->refcnt = 1;
			p->elem = new CVector<Type> * [len];
			int memsize = 0;
			int total = len*ven*sizeof(Type);
			if (total > MEMORY_SIZE)
				memsize = MEMORY_SIZE/(len*1024)*1024;
			for(int i=0; i<len; i++)
			{
				p->elem[i] = new CVector<Type>(ven);
			}
		}
		for(int i = 0; i<len; i++)
		{
			(*this)(i) = x;
		}
		return *this;
	}
	CMatrix & operator =(Type **x)
	{
		if (p == NULL)
			return *this;
		int len = p->length;
		int ven = p->elem[0]->vlen();
		if(p->refcnt >1)
		{
			p->refcnt -- ;
			p = new mrep;
			p->length = len;
			p->refcnt = 1;
			p->elem = new CVector<Type> * [len];
			for(int i=0; i<len; i++)
			{
				p->elem[i] = new CVector<Type>(ven);
			}
		}

		for(int i = 0; i<len; i++)
		{
			(*this)(i) = x[i];
		}
		return *this;
	}

	CMatrix & operator =(Type *x)
	{
		if (p == NULL)
			return *this;
		int len = p->length;
		int ven = p->elem[0]->vlen();
		if(p->refcnt >1)
		{
			p->refcnt -- ;
			p = new mrep;
			p->length = len;
			p->refcnt = 1;
			p->elem = new CVector<Type> * [len];
			for(int i=0; i<len; i++)
			{
				p->elem[i] = new CVector<Type>(ven);
			}
		}
		for(int i = 0; i<len; i++)
		{
			(*this)(i) = x+i*ven;
		}
		return *this;
	}

	TEMPLAT_T friend CMatrix T_T copy(const CMatrix T_T &mat);
    CMatrix copy()const 
	{
		int mr = mrow();
		int mc = mcol();
		CMatrix<Type> x(mr,mc);
		for(int i = 0; i<mc; i++)
		{
			CVector<Type>  Source = (*this).GetValue(i);
			x(i) = Source.copy();
		}
		return x;
	}

    
	TEMPLAT_T friend CMatrix<T_S> operator + (const CMatrix<T_S> &v1, const CMatrix<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator - (const CMatrix<T_S> &v1, const CMatrix<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator * (const CMatrix<T_S> &v1, const CMatrix<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator / (const CMatrix<T_S> &v1, const CMatrix<T_S> &v2);

	TEMPLAT_T friend CMatrix<T_S> operator | (const CMatrix<T_S> &v1, const CMatrix<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator & (const CMatrix<T_S> &v1, const CMatrix<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator ^ (const CMatrix<T_S> &v1, const CMatrix<T_S> &v2);

	TEMPLAT_T friend CMatrix<T_S> operator | (const CMatrix<T_S> &v1, const CVector<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator & (const CMatrix<T_S> &v1, const CVector<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator | (const CVector<T_S> &v1, const CMatrix<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator & (const CVector<T_S> &v1, const CMatrix<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator | (const CVector<T_S> &v1, const CVector<T_S> &v2);
	TEMPLAT_T friend CMatrix<T_S> operator & (const CVector<T_S> &v1, const CVector<T_S> &v2);


	TEMPLAT_T friend CMatrix T_T operator + (const CMatrix T_T &v1, T_S x);
	TEMPLAT_T friend CMatrix T_T operator - (const CMatrix T_T &v1, T_S x);
	TEMPLAT_T friend CMatrix T_T operator * (const CMatrix T_T &v1, T_S x);
	TEMPLAT_T friend CMatrix T_T operator / (const CMatrix T_T &v1, T_S x);

    TEMPLAT_T friend CMatrix T_T operator + (T_S x, const CMatrix T_T &v1);
    TEMPLAT_T friend CMatrix T_T operator - (T_S x, const CMatrix T_T &v1);
    TEMPLAT_T friend CMatrix T_T operator * (T_S x, const CMatrix T_T &v1);
    TEMPLAT_T friend CMatrix T_T operator / (T_S x, const CMatrix T_T &v1);
	TEMPLAT_T friend CMatrix T_T operator * (const CMatrix T_T &v1,const CDiagMatrix T_T&thin);

    TEMPLAT_T friend CVector<T_S> operator * ( const CMatrix T_T & m1, const CVector<T_S> &v2);
	TEMPLAT_T friend CVector<T_S> operator * ( const CVector<T_S> & v1, const CMatrix T_T &m2);

	TEMPLAT_T friend CMatrix T_T operator + (const CMatrix T_T &v1);
	TEMPLAT_T friend CMatrix T_T operator - (const CMatrix T_T &v1);

	CMatrix & operator += (const CMatrix &v1)
	{
		if (p == NULL)
			return *this;
		int len = MIN(mcol(), v1.mcol());

		for(int i=0; i<len; i++)
		{
			(*this)(i) = (*this).GetValue(i)+v1.GetValue(i);
		}
		return * this;
	}

	CMatrix & operator -= (const CMatrix &v1)
	{
		if (p == NULL)
			return *this;
		int len = MIN(p->length, v1.p->length);

		for(int i=0; i<len; i++)
		{
			(*this)(i) -= v1(i);
		}

		return * this;
	}

	CMatrix & operator *= (const CMatrix &v1)
	{
		if (p == NULL)
			return *this;
		int len = MIN(p->length, v1.p->length);

		for(int i=0; i<len; i++)
		{
			(*this)(i) *= v1(i);
		}
		return * this;
	}

	CMatrix & operator /= (const CMatrix &v1)
	{
		if (p == NULL)
			return *this;
		int len = MIN(p->length, v1.p->length);
		for(int i=0; i<len; i++)
		{
			(*this)(i) /= v1(i);
		}
		return * this;
	}

	CMatrix & operator += (Type x)
	{
		if (p == NULL)
			return *this;
		int len = p->length;
		for(int i=0; i<len; i++)
		{
			(*this)(i) += x;
		}
		return *this;
	}

	CMatrix & operator -= (Type x)
	{
		if (p == NULL)
			return *this;
		int len = p->length;
		for(int i=0; i<len; i++)
		{
			(*this)(i) -= x;
		}
		return *this;
	}

	CMatrix & operator *= (Type x)
	{
		if (p == NULL)
			return *this;
		int len = p->length;
		for(int i=0; i<len; i++)
		{
			(*this)(i) *= x;
		}
		return *this;
	}

	CMatrix & operator /= (Type x)
	{
		if (p == NULL)
			return *this;
		int len = p->length;
		for(int i=0; i<len; i++)
		{
			(*this)(i) /= x;
		}
		return *this;
	}


	CMatrix<Type> & prod(const CMatrix<Type> & m2)
	{
		int len=MIN(p->length,m2.p->length);
		for(int i=0; i<len; i++)
		{
			(*this)(i) *= m2(i);
		}
		return *this;
	}


	TEMPLAT_T friend CMatrix<T_S> prod(const CMatrix<T_S> &v1, const CMatrix<T_S> &v2);
    
	TEMPLAT_T friend int operator == (const CMatrix T_T &v1, const CMatrix T_T &v2);
	TEMPLAT_T friend int operator == (const CMatrix T_T &v1, T_S x);
	TEMPLAT_T friend int operator != (const CMatrix T_T &v1, T_S x);
	TEMPLAT_T friend int operator != (const CMatrix T_T &v1, const CMatrix T_T &v2);
	CVector<Type> row(int n)
	{
		int mrows = mrow();
		int mcols = mcol();
		if((n>mrows)||(n<0))
		{
			ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
			n = 1;
		}
		CVector<Type> x(mcols);
		for(int i=0; i<mcols; i++)
			x(i)=(*this)(n,i);
		return x;
	}
	
	TEMPLAT_T friend CVector<T_S> row(const CMatrix<T_S> &v, int n);
	CVector<Type> col(int n)
	{
		int mrows = mrow();
		int mcols = mcol();
		if((n>mcols)||(n<0))
		{
			ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
			n = 1;
		}
		CVector<Type> x(mrows);
		for(int i=0; i<mrows; i++)
			x(i)=(*this)(i,n);
		return x;
	}
	
	TEMPLAT_T friend CVector<T_S> col(const CMatrix<T_S> &v, int n);
	
	CMatrix slice(int row, int col)const 
	{
		int mrows = mrow();
		int mcols = mcol();
		if((row>mrows)||(col>mcols)||(row<0)||(col<0))
		{
			ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
			row =0;
			col =0;
		}
		
		CMatrix<Type> x(row,col);
		for(int j=0; j<col; j++)
			for(int i=0; i<row; i++)
				x(i,j)=(*this).GetValue(i,j);
			return x;
	}
	
	TEMPLAT_T friend CMatrix T_T slice(const CMatrix T_T &v, int row, int col);
	CMatrix slice(int row1, int col1, int row2, int col2)const 
	{
		int mrows = mrow();
		int mcols = mcol();
		if((row2<0)||(col2<0))
		{
			ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
			row2 = 0;
			col2 = 0;
		}
		if(((row1+row2)>mrows)||(col1+col2)>mcols||(row1<0)||(col1<0))
		{
			ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
			row1 = 0;
			col1 = 0;
			row2 = 0;
			col2 = 0;
		}	
		CMatrix<Type> x(row2, col2);
		for(int j=col1; j<col1+col2; j++)
			for(int i=row1; i<row1+row2; i++)
				x(i-row1,j-col1)= (*this).GetValue(i,j);
			return x;
	}

	TEMPLAT_T friend CMatrix T_T slice(const CMatrix T_T &v, int row1, int col1, int row2, int col2);
	
    TEMPLAT_T friend CMatrix T_T commat(const CMatrix T_T &m1, CMatrix T_T &m2, int select =2);
	CMatrix commat(const CMatrix &m2, int select = 2)
	{
		CMatrix<Type> m((*this),m2,select);
		return m;
	}
	
    
	TEMPLAT_T friend CMatrix T_T commat(const CMatrix<T_S> &m1, CVector<T_S> &v2, int select =2);
	CMatrix commat(const CVector<Type> &v2, int select = 2)
	{
		int m,n,i,j;
		
		int mrow1 = mrow();
		int mcol1 = mcol();
		int mrow2 = v2.vlen();
		switch(select)
		{
		case 1:
			m = MAX(mrow1, mrow2);
			n = mcol1 + 1;
			break;
		case 2:
			m = mrow1 + 1;
			n = MAX(mcol1,mrow2);
			break;
		default:
			ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
			m = 0;
			n = 0;
		}
		
		CMatrix<Type> x(m,n);
		
		for(j=0;j<mcol1;j++)
			for(i=0;i<mrow1;i++)
				x(i,j)=(*this)(i,j);
			if(select == 1)
			{
				for(j=0;j<mrow2;j++)
					x(j,mcol1)=v2.GetValue(j);
			}
			else
			{
				for(j=0; j<mrow2; j++)
					x(mrow1,j)=v2.GetValue(j);
			}
			return x;
	}
	

	TEMPLAT_T friend CMatrix T_T commat(const CVector<T_S> &v1, const CMatrix<T_S> &m2, int select =2);
	TEMPLAT_T friend CMatrix T_T commat(const CVector<T_S> &v1, const CVector<T_S> &v2, int select =2);
    
	TEMPLAT_T friend CMatrix T_T commat(const CMatrix<T_S> &m1, const CMatrix<T_S> &m2, int srow, int scol);
	CMatrix commat(const CMatrix<Type> &m2, int srow, int scol)const 
	{
		int i,j;
		int mrow1 = mrow();
		int mcol1 = mcol();
		int mrow2 = m2.mrow();
		int mcol2 = m2.mcol();
		if((srow<mrow1)&&(scol<mcol1))
		{
			ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
			srow = mrow1;
			scol = mcol1;
		}
		int mx = MAX(mrow1, srow+mrow2);
		int my = MAX(mcol1, scol+mcol2);
		CMatrix<Type> x(mx, my);
		for(j=0; j<mcol1; j++)
			for(i=0; i<mrow1; i++)
				x(i,j) = (*this)(i,j);
			for(j=0; j<mrow2; j++)
				for(i=0; i<mrow2; i++)
					x(i+srow,j+scol) = m2(i,j);
				return x;
				
	}
	
    TEMPLAT_T friend CMatrix T_T diag(const CVector<T_S> &v2);
	TEMPLAT_T friend CMatrix T_T diag(const CVector<T_S> &v2, int k);
	TEMPLAT_T friend CVector<T_S> diag(const CMatrix<T_S> &v2);
	CVector<Type> diag()const 
	{
		int n=MIN(mrow(),mcol());
		CVector<Type> x(n);
		for(int i=0; i<n; i++)
			x(i)=(*this)(i,i);
		return x;
	}
	
	TEMPLAT_T friend CVector<T_S> diag(const CMatrix<T_S> &v2, int k);
	CVector<Type> diag(int k)const 
	{
		return ::diag(*this)( k);
	}
	
    CMatrix<Type> trans()const 
	{
		int len = mrow();
		int ven = mcol();

		CMatrix<Type> v(ven, len);
		for(int i=0; i<ven; i++)
			for(int j=0; j<len; j++)
				v(i,j) = GetValue(j,i);
		return v;
	}


	TEMPLAT_T friend CMatrix<T_S> trans(const CMatrix<T_S> &m);

	TEMPLAT_T friend CVector<T_S> sum(const CMatrix<T_S> &m);
	CVector<Type> sum()const 
	{
		int m = mrow();
		CVector<Type> x(m);
		for (int j=0; j< m;j++)
		{
			Type su = 0;
			for(int i=0; i<mcol();i++)
				su += (*this)(j,i);
			x(j) = su;
		}
		return x;
	}

	TEMPLAT_T friend CVector<T_S> mean(const CMatrix<T_S> &m);
	CVector<Type> mean()const 
	{
		int m=mcol();
		CVector<Type> x(m);
		for(int i=0; i<m; i++)
			x(i) = mean((*this)(i));
		return x;
	}
	
	
	CMatrix & cos()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::cos((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T cos(const CMatrix T_T &x);

	CMatrix & cosh()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::cosh((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T cosh(const CMatrix T_T &x);

	CMatrix & acos()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::acos((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T acos(const CMatrix T_T &x);

	CMatrix & sin()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::sin((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T sin(const CMatrix T_T &x);

	CMatrix & sinh()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::sinh((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T sinh(const CMatrix T_T &x);

	CMatrix & asin()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::asin((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T asin(const CMatrix T_T &x);
	
	CMatrix & tan()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::tan((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T tan(const CMatrix T_T &x);

	CMatrix & tanh()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::tanh((*this)(i,j));
			return *this;
	}
	TEMPLAT_T friend CMatrix T_T tanh(const CMatrix T_T &x);

	CMatrix & atan()
	{
		int mrows = p->length;
		int mcols = vlen(p->elem[0]);
		for(int i=0; i<mrows; i++)
			for(int j=0; j<mcols; j++)
				(*this)(i,j) = ::atan((*this)(i,j));
			return *this;
	}
	
	
	TEMPLAT_T friend CMatrix T_T atan(const CMatrix T_T &x);
	
	CMatrix & exp()
	{
		int mrows = p->length;
		int mcols = vlen(p->elem[0]);
		for(int i=0; i<mrows; i++)
			for(int j=0; j<mcols; j++)
				(*this)(i,j) = ::exp((*this)(i,j));
			return *this;
	}
	
	
	TEMPLAT_T friend CMatrix T_T exp(const CMatrix T_T &x);

	CMatrix & log()
	{
		int mrows = p->length;
		int mcols = vlen(p->elem[0]);
		for(int i=0; i<mrows; i++)
			for(int j=0; j<mcols; j++)
				(*this)(i,j) = ::log((*this)(i,j));
			return *this;
	}
	TEMPLAT_T friend CMatrix T_T log(const CMatrix T_T &x);

	CMatrix & log10()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::log10((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T log10(const CMatrix T_T &x);

	CMatrix & pow(Type pp)
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::pow((*this)(i,j),pp);
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T pow(const CMatrix T_T &x, Type pp);

	CMatrix & pow(CMatrix &pp)
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::pow((*this)(i,j), pp(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T pow(const CMatrix T_T &x, const CMatrix T_T &pp);

	CMatrix & sqrt()
	{
		int mrows = mrow();
		int mcols = mcol();
		for(int j=0; j<mcols; j++)
			for(int i=0; i<mrows; i++)
				(*this)(i,j) = ::sqrt((*this)(i,j));
			return *this;
	}
	
	TEMPLAT_T friend CMatrix T_T sqrt(const CMatrix T_T &x);
};

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
typedef CMatrix<float> CFloatMatrix;
template<class Type>
void CMatrix<Type>::ZeroVectorPtrs(int xsize)
{
	memset((void*)p->elem,0,sizeof(void *)*xsize);
}

///////////////////////////////////////////////////////////////////////////////

template<class Type>
Type ** operator <<(Type **x, const CMatrix<Type> &m)
{
	int len = m.mcol();
    for(int i = 0; i<len; i++)	
	{
	    CVector<Type> & V = m.GetValue(i);
		x[i] <<	V;
	}
	return x;
}

template<class Type>
Type* operator <<(Type * x, const CMatrix<Type> &m)
{

	int len = m.mcol();
	int ven = m.mrow();
    for(int i = 0; i<len; i++)	
	{
	    CVector<Type>  V = m.GetValue(i);
		x[i*ven] = V(ven-1);
	}
	return x;
}
template<class Type>
CMatrix<Type>& operator <<(CMatrix<Type>& x, const CMatrix<Type> &m)
{
	int len = m.mcol();
    for(int i = 0; i<len; i++)	
	{
	    CVector<Type> & V = m.GetValue(i);
		x[i] <<	V;
	}
	return x;
}


template<class Type>
CVector<Type>& operator <<(CVector<Type>& x, const CMatrix<Type> &m)
{

	int len = m.mcol();
    for(int i = 0; i<len; i++)	
	{
	    CVector<Type> & V = m.GetValue(i);
		x(i*ven) =	V(ven-1);
	}
	return x;
}


template <class Type>
CMatrix<Type> copy(const CMatrix<Type> &m) 
{
	int mr = m.mrow();
	int mc = m.mcol();
    CMatrix<Type> x(mr,mc);
	for(int i=0; i<mc; i++)	
	{
		CVector<Type>  Source = m.GetValue(i);
		x(i) = copy(Source);
	}
	return x;
}

template <class Type>
CMatrix<Type> operator | (const CMatrix<Type> &v1, const CMatrix<Type> &v2)
{
    CMatrix<Type> v(v1,v2,1);
	return v;
}

template <class Type>
CMatrix<Type> operator & (const CMatrix<Type> &v1, const CMatrix<Type> &v2)
{
    CMatrix<Type> v(v1,v2,2);
	return v;
}

template <class Type>
CMatrix<Type> operator ^ (const CMatrix<Type> &v1, const CMatrix<Type> &v2)
{
    CMatrix<Type> v(v1,v2,0);
	return v;
}


template <class Type>
CMatrix<Type> operator + (const CMatrix<Type> &v1, const CMatrix<Type> &v2)
{
	int len = MIN(v1.mrow(), v2.mrow());
	int ven = MIN(v1.mcol(), v2.mcol());

	CMatrix<Type> v(len, ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = v1.GetValue(i) + v2.GetValue(i);
	}
	return v;
}


template <class Type>
CMatrix<Type> operator - (const CMatrix<Type> &v1, const CMatrix<Type> &v2)
{
	int len = MIN(v1.mrow(), v2.mrow());
	int ven = MIN(v1.mcol(), v2.mcol());

	CMatrix<Type> v(len, ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = v1.GetValue(i) - v2.GetValue(i);
	}
	return v;
}

template <class Type>
CMatrix<Type> operator * (const CMatrix<Type> &m1, const CMatrix<Type> &m2)
{
    int i, j;
	int m1rows = m1.mrow();
	int m1cols = m1.mcol();
	int m2rows = m2.mrow();
	int m2cols = m2.mcol();
	if(m1cols != m2rows)
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
        m1rows =0;
        m2rows =0;
		m1cols =0;
		m2rows =0;
	}
    CMatrix<Type> v(m1rows, m2cols);
	CMatrix<Type> flip(m1cols, m1rows);
	for(i=0; i<m1cols; i++)
		for(j=0; j<m1rows; j++)
			flip(i,j) = m1.GetValue(j,i);

	for(i=0; i<m1rows; i++)
		for(j=0;j<m2cols;j++)
		{
			v(i,j)=m2.GetValue(j).dot(flip(i));
		}
    return v;
}


template <class Type>
CMatrix<Type> operator / (const CMatrix<Type> &v1, const CMatrix<Type> &v2)
{
	int len = MIN(v1.mrow(), v2.mrow());
	int ven = MIN(v1.mcol(), v2.mcol());

	CMatrix<Type> v(len, ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = v1.GetValue(i) / v2.GetValue(i);
	}
	return v;
}

template <class Type>
CMatrix<Type> operator + (const CMatrix<Type> &v1, Type x)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = v1.GetValue(i) + x;
	}
	return v;
}

template <class Type>
CMatrix<Type> operator - (const CMatrix<Type> &v1, Type x)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = v1.GetValue(i) - x;
	}
	return v;
}

template <class Type>
CMatrix<Type> operator * (const CMatrix<Type> &v1, Type x)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = v1.GetValue(i) * x;
	}
	return v;
}



template <class Type>
CMatrix<Type> operator / (const CMatrix<Type> &v1, Type x)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = v1.GetValue(i) / x;
	}
	return v;
}

template <class Type>
CMatrix<Type> operator + ( Type x, const CMatrix<Type> &v1)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = x + v1.GetValue(i);
	}
	return v;
}

template <class Type>
CMatrix<Type> operator - ( Type x, const CMatrix<Type> &v1)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = x - v1.GetValue(i);
	}
	return v;
}

template <class Type>
CMatrix<Type> operator * ( Type x, const CMatrix<Type> &v1)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = x * v1.GetValue(i);
	}
	return v;
}

template <class Type>
CMatrix<Type> operator / ( Type x, const CMatrix<Type> &v1)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = x / v1.GetValue(i);
	}
	return v;
}

template<class Type>
CVector<Type> operator *(const CMatrix<Type> &m1, const CVector<Type> &v2)
{

    int m1rows = m1.mrow();
	int m1cols = m1.mcol();
	int m2rows = v2.vlen();
	if(m1cols != m2rows)
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
        m1rows =0;
        m2rows =0;
		m2rows =0;
	}
    CVector<Type> v(m1rows); 
	CMatrix<Type> flip(m1cols,m1rows);
	int i=0;
	for (i=0;i<m1rows;i++)
		for (int j=0;j<m1cols;j++)
			flip(j,i) = m1.GetValue(i,j);
	for(i=0; i<m1rows; i++)
	{
		v(i) = flip(i).dot(v2);
	}
	return v;
}

template<class Type>
CVector<Type> operator *( const CVector<Type> &v1, const CMatrix<Type> &m2)
{

    int m2rows = m2.mrow();
	int m2cols = m2.mcol();
	int m1cols = v1.vlen();
	assert(m1cols == m2rows); //zfm 2003-3-17
    CVector<Type> v(m2cols);
	for( i=0; i<m2cols; i++)
	{
		v(i) = v1.dot(m2.GetValue(i));
	}
	return v;
}

template<class Type>
CMatrix<Type> operator + (const CMatrix <Type> &m1)
{
	return m1; 
}

template<class Type>
CMatrix<Type> operator - (const CMatrix <Type> &v1)
{
    int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(len, ven);
	int i=0;
	for(i=0; i<ven; i++)
	{
		v(i) = - v1.GetValue(i);
	}
	return v;
}

template <class Type>
int operator ==(const CMatrix<Type> &v1, const CMatrix <Type> & v2)
{
	if(v1.p->length != v2.p->length || 
		(vlen(v1.p->elem[0])!= vlen(v2.p->elem[0])))
            return 0;
    int len = v1.p->length;
	for(int i=0; i<len; i++)
	{
		if(v1.GetValue(i) != v2.GetValue(i))
			return 0;
	}
	return 1;
}

template <class Type>
int operator ==(const CMatrix<Type> &v1, Type x)
{
    int len = v1.mcol();
	for(int i=0; i<len; i++)
	{
		if(v1.GetValue(i) != x)
			return 0;
	}
	return 1;
}

template <class Type>
int operator !=(const CMatrix<Type> &v1, Type x)
{
    int len = v1.mcol();
	for(int i=0; i<len; i++)
	{
		if(v1.GetValue(i) != x)
			return 1;
	}
	return 0;
}

template <class Type>
int operator !=(const CMatrix<Type> &v1, const CMatrix <Type> & v2)
{
	if(v1.p->length != v2.p->length || 
		(vlen(v1.p->elem[0])!= vlen(v2.p->elem[0])))
            return 1;
    int len = v1.p->length;
	for(int i=0; i<len; i++)
	{
		if(v1.GetValue(i) != v2.GetValue(i))
			return 1;
	}
	return 0;
}

template <class Type>
bool rerowsize(const CMatrix<Type> &m, int nrow)
{
	return m.rerowsize(nrow);
}

template <class Type>
CVector<Type> row(const CMatrix<Type> &m, int n)
{
    int mrows = mrow();
	int mcols = mcol();
	if((n>mcols)||(n<0))
	{
        ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
		n = 1;
	}
    CVector<Type> x(mcols);
	for(int i=0; i<mcols; i++)
		x(i)=m.GetValue(n,i);
	return x;
}

template <class Type>
CVector<Type> col(const CMatrix<Type> &m, int n)
{
    int mrows = m.mrow();
	int mcols = m.mcol();
	if((n>mcols)||(n<0))
	{
        ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
		n = 1;
	}
	return m.GetValue(n).copy();
}

template <class Type>
CMatrix<Type> slice(const CMatrix<Type> &m, int row, int col)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
	if((row>mrows)||(col>mcols)||(row<0)||(col<0))
	{
        ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
		row =0;
		col =0;
	}

	CMatrix<Type> x(row,col);
	for(int j=0; j<col; j++)
		for(int i=0; i<row; i++)
			x(i,j)=m.GetValue(i,j);
	return x;
}

template <class Type>
CMatrix<Type> slice(const CMatrix<Type>&m, int row1, int col1, int row2, int col2)
{
	int mrows = m.mrow();
	int mcols = m.mcol();
    if((row2<0)||(col2<0))
	{
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
		row2 = 0;
		col2 = 0;
	}
	if(((row1+row2)>mrows)||(col1+col2)>mcols||(row1<0)||(col1<0))
	{
        ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
		row1 = 0;
		col1 = 0;
		row2 = 0;
		col2 = 0;
	}	
    CMatrix<Type> x(row2, col2);
	for(int j=col1; j<col1+col2; j++)
		for(int i=row1; i<row1+row2; i++)
			x(i-row1,j-col1)= m.GetValue(i,j);
	return x;
}

template<class Type>
CMatrix<Type> commat(const CMatrix<Type> &m1, const CMatrix<Type> &m2, int select)
{
	CMatrix<Type> m(m1,m2,select);
	return m;
}

template <class Type>
CMatrix<Type> commat(const CMatrix<Type> &m1, const CVector<Type> &v2, int select)
{
	int m,n,i,j;
	
	int mrow1 = m1.mrow();
    int mcol1 = m1.mcol();
	int mrow2 = v2.vlen();
	switch(select)
	{
	case 1:
		m = MAX(mrow1, mrow2);
		n = mcol1 + 1;
		break;
	case 2:
		m = mrow1 + 1;
		n = MAX(mcol1,mrow2);
		break;
	default:
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
        m = 0;
		n = 0;
	}

    CMatrix<Type> x(m,n);

	for(j=0;j<mcol1;j++)
		for(i=0;i<mrow1;i++)
			x(i,j)=m1.GetValue(i,j);
	if(select == 1)
	{
		for(j=0;j<mrow2;j++)
			x(j,mcol1)=v2.GetValue(j);
	}
	else
	{
		for(j=0; j<mrow2; j++)
			x(mrow1,j)=v2.GetValue(j);
	}
	return x;
}

template <class Type>
CMatrix<Type> commat(const CVector<Type> &v1, const CMatrix<Type> &m2, int select)
{
	int m,n,i,j;
	
	int mrow1 = v1.vlen();
    int mcol2 = m2.mcol();
	int mrow2 = m2.mrow();
	switch(select)
	{
	case 1:
		m = MAX(mrow1, mrow2);
		n = mcol2 + 1;
		break;
	case 2:
		m = mrow2 + 1;
		n = MAX(mrow1,mcol2);
		break;
	default:
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
        m = 0;
		n = 0;
	}
    CMatrix<Type> x(m,n);
	if(select == 1)
	{
	    for(i=0; i<mrow1; i++)
			x(i,0) = v1.GetValue(i);
		for(j=0; j<mcol2; j++)
			for(i=0;i<mrow2; i++)
				x(i,j+1)=m2.GetValue(i,j);
	}
	else
	{
		for(j=0; j<mrow1; j++)
			x(0,j)=v1.GetValue(j);
		for(j=0; j<mcol2; j++)
			for(i=0; i<mrow2; i++)
                x(i+1,j) = m2.GetValue(i,j);
	}
	return x;
}

template <class Type>
CMatrix<Type> commat(const CVector<Type> &v1, const CVector<Type> &v2, int select)
{
    int i, m, n;
	int mrow1 = v1.vlen();
	int mrow2 = v2.vlen();
	switch(select)
	{
	case 1:
		m = MAX(mrow1, mrow2);
		n = 2;
		break;
	case 2:
		m = 2;
		n = MAX(mrow1, mrow2);
		break;
	default:
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
        m = 0;
		n = 0;
	}
	CMatrix <Type> x(m,n);
	if(select == 1)
	{
		for(i=0; i<mrow1;i++)
			x(i,0) = v1.GetValue(i);
		for(i=0; i<mrow2;i++)
			x(i,1) = v2.GetValue(i);
	}
    else
	{
		for(i=0; i<mrow1; i++)
			x(0,i) = v1.GetValue(i);
		for(i=0; i<mrow2; i++)
			x(1,i) = v2.GetValue(i);
	}
	return x;
}

template <class Type>
CMatrix<Type> commat(const CMatrix<Type> &m1, const CMatrix<Type> &m2, int srow, int scol)
{
	int i,j;
	int mrow1 = m1.mrow();
	int mcol1 = m1.mcol();
	int mrow2 = m2.mrow();
	int mcol2 = m2.mcol();
	if((srow<mrow1)&&(scol<mcol1))
	{
		m1.ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
		srow = mrow1;
		scol = mcol1;
	}
    int mx = MAX(mrow1, srow+mrow2);
	int my = MAX(mcol1, scol+mcol2);
	CMatrix<Type> x(mx, my);
	for(j=0; j<mcol1; j++)
		for(i=0; i<mrow1; i++)
            x(i,j) = m1.GetValue(i,j);
	for(j=0; j<mrow2; j++)
		for(i=0; i<mrow2; i++)
			x(i+srow,j+scol) = m2.GetValue(i,j);
    return x;
}
 
template <class Type>
CMatrix<Type> operator | (const CMatrix<Type> &v1, const CVector<Type> &v2)
{
    return commat(v1, v2, 1);
}

template <class Type>
CMatrix<Type> operator & (const CMatrix<Type> &v1, const CVector<Type> &v2)
{
    return commat(v1, v2, 2);
}

template <class Type>
CMatrix<Type> operator | (const CVector<Type> &v1, const CMatrix<Type> &v2)
{
    return commat(v1, v2, 1);
}

template <class Type>
CMatrix<Type> operator & (const CVector<Type> &v1, const CMatrix<Type> &v2)
{
    return commat(v1, v2, 2);
}


template <class Type>
CMatrix<Type> operator | (const CVector<Type> &v1, const CVector<Type> &v2)
{
    return commat(v1, v2, 1);
}

template <class Type>
CMatrix<Type> operator & (const CVector<Type> &v1, const CVector<Type> &v2)
{
    return commat(v1, v2, 2);
}
//*
//#_S 2004-11-11 $ 14 zlq 对角矩阵优化 ===temp 暂时注释
template <class Type>
CMatrix<Type> diag(const CVector<Type> &v2)
{
	int n = vlen(v2);
	CMatrix<Type> x(n, n);
	for(int i=0; i<n; i++)
		x(i,i)=v2.GetValue(i);
	return x;
}
//*/
template <class Type>
CMatrix<Type> diag(const CVector<Type> &v2, int k)
{
	int nn = vlen(v2);
	int kk = abs(k);
	int n = nn + kk;
    CMatrix<Type> x(n,n);
	if(k>0)
	{
		for(int i=0; i<nn; i++)
			x(i,i+k) = v2.GetValue(i);
	}
	else
	{
		for(int i=kk; i<n; i++)
			x(i,i-kk) = v2.GetValue(i-kk);
	}
	return x;
}

template <class Type>
CVector<Type> diag(const CMatrix<Type> &v2)
{
	int n=MIN(v2.mrow(),v2.mcol());
	CVector<Type> x(n);
	for(int i=0; i<n; i++)
		x(i)=v2.GetValue(i,i);
	return x;
}

template <class Type>
CVector<Type> diag(const CMatrix<Type> &v2, int k)
{
	int m = v2.mrow();
	int n = v2.mcol();
	int kk = abs(k);
	int nn, i;
	if(((k>0)&&(k>(n-1)))||((k<0)&&(-k>(m-1))))
	{
		v2.ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
		k = 0;
	}
    if(m>=n)
	{
		if(k>=0)
			nn = n-kk;
		else if(n+kk<=m)
			nn = n;
		else
			nn = m - kk;
	}
	else
	{
		if(k>=0)
		{
			if(m+kk<=n)
				nn = m;
			else
				nn = n-kk;
		}
		else
			nn = m -kk;
	}
	CVector<Type> x(nn);
    if(k>0)
		for(i=0;i<nn;i++)
			x(i) = v2.GetValue(i,i+kk);
	else
		for(i=0; i<nn; i++)
			x(i) = v2.GetValue(i+kk,i);
	return x;
}

template<class Type>
CMatrix<Type> trans(const CMatrix<Type> &v1)
{
	int len = v1.mrow();
	int ven = v1.mcol();

	CMatrix<Type> v(ven, len);
	for(int i=0; i<ven; i++)
		for(int j=0; j<len; j++)
			v(i,j) = v1.GetValue(j,i);
	return v;
}

template <class Type>
CVector<Type> mean(const CMatrix<Type> &v)
{
	int m=v.mcol();
	CVector<Type> x(m);
	for(int i=0; i<m;i++)
		x(i) = mean(v.GetValue(i));
	return x;
}

template <class Type>
CVector<Type> sum(const CMatrix<Type> &v)
{
	int m = v.mrow();
	CVector<Type> x(m);
	for (int j=0; j< m;j++)
	{
		Type su = 0;
		for(int i=0; i<v.mcol();i++)
			su += v.GetValue(j,i);
		x(j) = su;
	}
	return x;
}

template<class Type>
CMatrix<Type> cos(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::cos(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> cosh(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::cosh(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> acos(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::acos(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> sin(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::sin(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> sinh(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::sinh(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> asin(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::asin(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> tan(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::tan(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> tanh(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			v(i,j) = ::tanh(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> atan(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			v(i,j) = ::atan(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> log(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			v(i,j) = ::log(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> exp(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int i=0; i<mrows; i++)
		for(int j=0; j<mcols; j++)
			v(i,j) = ::exp(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> log10(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::log10(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> sqrt(const CMatrix<Type> &x)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::sqrt(x.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> pow(const CMatrix<Type> &x, Type pp)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::pow(x.GetValue(i,j),pp);
	return v;
}

template<class Type>
CMatrix<Type> pow(const CMatrix<Type> &x, const CMatrix<Type> &pp)
{
	int mrows = x.mrow();
	int mcols = x.mcol();
	CMatrix<Type> v(mrows, mcols);
	for(int j=0; j<mcols; j++)
		for(int i=0; i<mrows; i++)
			v(i,j) = ::pow(x.GetValue(i,j),pp.GetValue(i,j));
	return v;
}

template<class Type>
CMatrix<Type> prod(const CMatrix<Type> &v1, const CMatrix<Type> &v2)
{
	int len=MIN(v1.mrow(), v2.mrow());
	int ven = MIN(v1.mcol(),v2.mcol());
	CMatrix<Type> v(len,ven);
	for(int i=0; i<ven; i++)
	{
		v(i) = v1.GetValue(i) * v2.GetValue(i);
	}
	return v;
}


////////////////////////////////////////////////////////////////////////////////
////////////////// ------- CIntMatrix ----------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIntMatrix : public CMatrix<int> 
{
public:
	CIntMatrix(const CMatrix<int> &x):CMatrix<int>(x){};
	CIntMatrix(const CIntMatrix &x):CMatrix<int>(x){};
	CIntMatrix(int *a, int xlength, int ylength):CMatrix<int>(a, xlength, ylength){};
	CIntMatrix(int init=0)
		:CMatrix<int>(init){};

	CIntMatrix(int xsize, int ysize, int init =0.0)
		:CMatrix<int>(xsize, ysize, init){};

	CIntMatrix(int **a, int xlength, int ylength)
		:CMatrix<int>(a, xlength, ylength){};

	CIntMatrix(const CIntMatrix &a1, const CIntMatrix &a2, int select)
		:CMatrix<int>(a1,a2,select){};

	CIntMatrix(const CIntMatrix &a, int xfirst, int xlast, int yfirst, int ylast)
		:CMatrix<int>(a,xfirst, xlast, yfirst, ylast){};
public:


	CIntMatrix & operator =(const CIntMatrix &x);
	CIntMatrix & operator =(const CMatrix<int> &x);
	CIntMatrix & operator =(const CVector<int> &x);
	CIntMatrix & operator =(const CIntVector &x);
	CIntMatrix & operator =(int **x);
	CIntMatrix & operator =(int *x);
	CIntMatrix & operator =(int x);


};

 

////////////////////////////////////////////////////////////////////////////////
//////////////////----------- CDoubleMatrix -------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class CComplexMatrix;
class CSymDbMatrix;
class CSparseDbMatrix;
class CTriangleDbMatrix;

class AFX_EXT_CLASS CDoubleMatrix : public CMatrix<double> 
{
public:
	CDoubleMatrix(int init=0):CMatrix<double>(init){};
	CDoubleMatrix(const CMatrix<double> &x):CMatrix<double>(x){};
	CDoubleMatrix(const CDoubleMatrix &x):CMatrix<double>(x){};
	CDoubleMatrix(int xsize, int ysize, double init =0.0):CMatrix<double>(xsize, ysize, init){};
	CDoubleMatrix(double **a, int xlength, int ylength):CMatrix<double>(a, xlength, ylength){};
	CDoubleMatrix(const CDoubleMatrix &a, int xlength, int ylength):CMatrix<double>(a, xlength, ylength){};
	CDoubleMatrix(const CDoubleMatrix &a1, CDoubleMatrix &a2, int select):CMatrix<double>(a1,a2,select){};
	CDoubleMatrix(const CDoubleMatrix &a, int xfirst, int xlast, int yfirst, int ylast):CMatrix<double>(a,xfirst, xlast, yfirst, ylast){};
	CDoubleMatrix(double *a, int xlength, int ylength):CMatrix<double>(a, xlength, ylength){};
	CDoubleMatrix(const CDoubleVector &x):CMatrix<double>(x){};
    CDoubleMatrix(const CSymDbMatrix &x);
	CDoubleMatrix(const CTriangleDbMatrix &x);
    CDoubleMatrix(const CSparseDbMatrix &x);
	CDoubleMatrix(const CComplexMatrix &x);


public:


	CDoubleMatrix & operator =(const CDoubleMatrix &x);
	CDoubleMatrix & operator =(const CMatrix<double> &x);
	CDoubleMatrix & operator =(const CVector<double> &x);
	CDoubleMatrix & operator =(const CDoubleVector &x);
	CDoubleMatrix & operator =(const CIntMatrix &x);
	CDoubleMatrix & operator =(const CFloatMatrix &x);
	CDoubleMatrix & operator =(double **x);
	CDoubleMatrix & operator =(double *x);
	CDoubleMatrix & operator =(double x);
	CDoubleMatrix & operator =(const CSymDbMatrix &x);
	CDoubleMatrix & operator =(const CComplexMatrix &x);
/*
	AFX_EXT_CLASS friend CDoubleMatrix  operator + (const CDoubleMatrix &v1, int x);
	AFX_EXT_CLASS friend CDoubleMatrix  operator - (const CDoubleMatrix &v1, int x);
	AFX_EXT_CLASS friend CDoubleMatrix  operator * (const CDoubleMatrix &v1, int x);
	AFX_EXT_CLASS friend CDoubleMatrix  operator / (const CDoubleMatrix &v1, int x);

	AFX_EXT_CLASS friend CDoubleMatrix  operator + (int x, const CDoubleMatrix &v1);
	AFX_EXT_CLASS friend CDoubleMatrix  operator - (int x, const CDoubleMatrix &v1);
	AFX_EXT_CLASS friend CDoubleMatrix  operator * (int x, const CDoubleMatrix &v1);
	AFX_EXT_CLASS friend CDoubleMatrix  operator / (int x, const CDoubleMatrix &v1);
*/

    AFX_EXT_CLASS friend CDoubleMatrix imattodmat(const CIntMatrix &x);
	AFX_EXT_CLASS friend CFloatMatrix imattofmat(const CIntMatrix &x);
	AFX_EXT_CLASS friend CDoubleMatrix fmattodmat(const CFloatMatrix &x);
	AFX_EXT_CLASS friend CDoubleMatrix smattodmat(const CSymDbMatrix &x);
	CDoubleMatrix& delt(int nCol);

    CDoubleMatrix dot(const CDoubleMatrix &m2)const ;
	AFX_EXT_CLASS friend CDoubleMatrix dot(const CDoubleMatrix &m1,const CDoubleMatrix &m2);
    CDoubleVector dot(const CDoubleVector &v)const ;
	AFX_EXT_CLASS friend CDoubleVector dot(const CDoubleMatrix &m,const CDoubleVector &v);
	AFX_EXT_CLASS friend CDoubleVector dot(const CDoubleVector &v, const CDoubleMatrix &m);
    CDoubleMatrix trans()const ;
	AFX_EXT_CLASS friend CDoubleMatrix trans(const CDoubleMatrix &m);
    CDoubleMatrix abs()const ;
	AFX_EXT_CLASS friend CDoubleMatrix abs(const CDoubleMatrix &m);
    AFX_EXT_CLASS friend CIntVector any(const CDoubleMatrix &v);
	CIntVector any()const ;
    AFX_EXT_CLASS friend CDoubleVector Max(const CDoubleMatrix &v);
	CDoubleVector Max()const ;
    AFX_EXT_CLASS friend CDoubleMatrix Max(const CDoubleMatrix &v1, const CDoubleMatrix &v2);
	CDoubleMatrix Max(const CDoubleMatrix &v2const );
    AFX_EXT_CLASS friend CDoubleVector Min(const CDoubleMatrix &v);
	CDoubleVector Min()const ;
    AFX_EXT_CLASS friend CDoubleMatrix Min(const CDoubleMatrix &v1, const CDoubleMatrix &v2);
	CDoubleMatrix Min(const CDoubleMatrix &v2)const ;
	AFX_EXT_CLASS friend CDoubleMatrix eye(int n);
	AFX_EXT_CLASS friend CDoubleMatrix eye(int m, int n);
    CDoubleMatrix eye()const ;
	AFX_EXT_CLASS friend CDoubleMatrix eye(const CDoubleMatrix &m);
  
    CDoubleMatrix mkeye(int nIndex)const ;
	AFX_EXT_CLASS friend CDoubleMatrix mkeye(const CDoubleMatrix &m,int nIndex);
	
	CDoubleMatrix & Rand();
	AFX_EXT_CLASS friend CDoubleMatrix & Rand(const CDoubleMatrix &v);
	CDoubleMatrix Rand(int m, int n)const ;

    double norm(int n=2)const ;
	AFX_EXT_CLASS friend double norm(const CDoubleMatrix &v, int n=2);
	double cond()const ;
	AFX_EXT_CLASS friend double cond(const CDoubleMatrix &m);
	int rank(double tol=0)const ; //快速求rank zfm
	AFX_EXT_CLASS friend int rank(const CDoubleMatrix &x, double tol=0); //快速求rank zfm

	double det()const ;
    AFX_EXT_CLASS friend double det(const CDoubleMatrix &m);

	CDoubleMatrix inv()const ;
    AFX_EXT_CLASS friend CDoubleMatrix inv(const CDoubleMatrix &m);

	CDoubleMatrix inv1()const ;
    AFX_EXT_CLASS friend CDoubleMatrix inv1(const CDoubleMatrix &m);
	CDoubleMatrix inv(double &det)const ;
    AFX_EXT_CLASS friend CDoubleMatrix inv(const CDoubleMatrix &m, double &det);

    CDoubleMatrix hess()const ;
	AFX_EXT_CLASS friend CDoubleMatrix hess(const CDoubleMatrix &m);
	CDoubleMatrix hess(CDoubleMatrix &v)const ;
	AFX_EXT_CLASS friend CDoubleMatrix hess(const CDoubleMatrix &m, CDoubleMatrix &v);

	CDoubleVector svd()const ; 
	AFX_EXT_CLASS friend CDoubleVector svd(const CDoubleMatrix &m);

	AFX_EXT_CLASS friend CDoubleVector svd_rank(const CDoubleMatrix &m); //svd分解，求rank专用



	CDoubleMatrix svd(CDoubleMatrix &U, CDoubleMatrix &V)const ;
	AFX_EXT_CLASS friend CDoubleMatrix svd( CDoubleMatrix &m, CDoubleMatrix &U, CDoubleMatrix &V);

	CDoubleMatrix pinv()const ;
	AFX_EXT_CLASS friend CDoubleMatrix pinv(const CDoubleMatrix &m);

    CComplexVector eig()const ;
	AFX_EXT_CLASS friend CComplexVector eig(CDoubleMatrix &m);

    CComplexMatrix eig(CComplexMatrix &v)const ;
    AFX_EXT_CLASS friend CComplexMatrix eig(const CDoubleMatrix &m, CComplexMatrix &v);

    CDoubleMatrix expm()const ;
	AFX_EXT_CLASS friend CDoubleMatrix expm(const CDoubleMatrix &m);
    CDoubleMatrix expm1()const ;
	AFX_EXT_CLASS friend CDoubleMatrix expm1(CDoubleMatrix &m);
    CDoubleMatrix expm2()const ;
	AFX_EXT_CLASS friend CDoubleMatrix expm2(const CDoubleMatrix &m);
	CComplexMatrix logm2()const ;
	AFX_EXT_CLASS friend CComplexMatrix logm2(const CDoubleMatrix &m);
	CComplexMatrix sqrtm2()const ;
	AFX_EXT_CLASS friend CComplexMatrix sqrtm2(const CDoubleMatrix &m);
	CComplexMatrix powm2(const CComplex &c)const ;
	AFX_EXT_CLASS friend CComplexMatrix powm2(const CDoubleMatrix &m, const CComplex &c);

	CDoubleVector poly()const ;
	AFX_EXT_CLASS friend CDoubleVector poly(const CDoubleMatrix &x);

    CDoubleMatrix qr(CDoubleMatrix &q, int flag =1)const ;
	AFX_EXT_CLASS friend CDoubleMatrix qr(const CDoubleMatrix &x, const CDoubleMatrix &q, int flag = 1); 
    CDoubleMatrix qr(CDoubleMatrix &q, CIntVector &e, int flag = 1)const;
	AFX_EXT_CLASS friend CDoubleMatrix qr(const CDoubleMatrix &x, const CDoubleMatrix &q, const CIntVector &e, int flag = 1); 
	CDoubleMatrix qrZkc(CDoubleMatrix &q, CIntVector &e, int flag = 1); //快速分块QR分解   zxg zfm  2003-1-26
	AFX_EXT_CLASS friend CDoubleMatrix normcdf(const CDoubleMatrix &m, double mu, double sigma);
	AFX_EXT_CLASS friend CDoubleMatrix chi2cdf(const CDoubleMatrix &m, int v);
	AFX_EXT_CLASS friend CDoubleMatrix fcdf(const CDoubleMatrix &m, int v1, int v2);
	AFX_EXT_CLASS friend CDoubleMatrix tcdf(const CDoubleMatrix &m, int v);

	AFX_EXT_CLASS friend CDoubleMatrix normpdf(const CDoubleMatrix &m, double mu, double sigma);
	AFX_EXT_CLASS friend CDoubleMatrix chi2pdf(const CDoubleMatrix &m, int v);
	AFX_EXT_CLASS friend CDoubleMatrix fpdf(const CDoubleMatrix &m, int v1, int v2);
	AFX_EXT_CLASS friend CDoubleMatrix tpdf(const CDoubleMatrix &m, int v);

	AFX_EXT_CLASS friend CDoubleMatrix norminv(const CDoubleMatrix &m, double mu, double sigma);
	AFX_EXT_CLASS friend CDoubleMatrix chi2inv(const CDoubleMatrix &m, int v);
	AFX_EXT_CLASS friend CDoubleMatrix finv(const CDoubleMatrix &m, int v1, int v2);
	AFX_EXT_CLASS friend CDoubleMatrix tinv(const CDoubleMatrix &m, int v);

    AFX_EXT_CLASS friend CDoubleVector var(const CDoubleMatrix &m);
	AFX_EXT_CLASS friend CDoubleVector st_d(const CDoubleMatrix &m);

	AFX_EXT_CLASS friend CDoubleVector ldiv(CDoubleMatrix &m, CDoubleVector &v);
	AFX_EXT_CLASS friend CDoubleMatrix ldiv(CDoubleMatrix &m, CDoubleMatrix &v);
};

////////////////////////////////////////////////////////////////////////////////
////////////////// ---------CComplexMatrix---------- /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CComplexMatrix:public CMatrix<CComplex>  
{
public:
//{{{START 2003-1-15 Eric
	CComplexMatrix(int init=0)
		:CMatrix<CComplex>(init){};
	CComplexMatrix(int xsize, int ysize, CComplex init =0.0)
		:CMatrix<CComplex>(xsize,ysize,init){};
	CComplexMatrix(CComplex **a, int xlength, int ylength)
		:CMatrix<CComplex>(a,xlength,ylength){};
	CComplexMatrix(CComplex *a, int xlength, int ylength)
		:CMatrix<CComplex>(a,xlength,ylength){};
    CComplexMatrix(const CComplexMatrix &a1, const CComplexMatrix &a2, int select = 2)
		:CMatrix<CComplex>(a1,a2,select){};
	CComplexMatrix(const CComplexMatrix &a, int xfirst, int xlast, int yfirst, int ylast)
		:CMatrix<CComplex>(a,xfirst,xlast,yfirst,ylast){};
	virtual ~CComplexMatrix()
	{
		destroy();
	};
    CComplexMatrix(const CMatrix<CComplex> &x):CMatrix<CComplex>(x){};
	CComplexMatrix(const CDoubleMatrix &re, const CDoubleMatrix &im);
	CComplexMatrix(const CMatrix<double> &re, const CMatrix<double> &im);
	CComplexMatrix(const CMatrix<double> &x);
	CComplexMatrix(const CDoubleMatrix &x);
//}}}END  2003-1-15 Eric

public:
	CComplexMatrix & operator =(const CComplexMatrix &x);
	CComplexMatrix & operator =(const CMatrix<CComplex> &x);
	CComplexMatrix & operator =(const CVector<CComplex> &x);
	CComplexMatrix & operator =(const CComplexVector &x);
	CComplexMatrix & operator =(const CComplex &x);
	CComplexMatrix & operator =(CComplex *x);
	CComplexMatrix & operator =(CComplex **x);
	CComplexMatrix & operator =(const CDoubleMatrix &x);
	CComplexMatrix & operator =(const CMatrix<double> &x);
	AFX_EXT_CLASS friend CComplexMatrix dmattocmat(const CDoubleMatrix &x);
	AFX_EXT_CLASS friend CComplexMatrix imattocmat(const CIntMatrix &x);
	AFX_EXT_CLASS friend CComplexMatrix fmattocmat(const CFloatMatrix &x);
	CDoubleMatrix real()const ;
	AFX_EXT_CLASS friend CDoubleMatrix real(const CComplexMatrix &v);
	CDoubleMatrix imag()const ;
	AFX_EXT_CLASS friend CDoubleMatrix imag(const CComplexMatrix &v);
	CComplexMatrix conj()const ;
	AFX_EXT_CLASS friend CComplexMatrix conj(const CComplexMatrix &v);
    AFX_EXT_CLASS friend CComplexMatrix Complex(const CDoubleMatrix &v1, const CDoubleMatrix &v2);
	AFX_EXT_CLASS friend CComplexMatrix Complex(const CDoubleMatrix &v1, double x=0);
	AFX_EXT_CLASS friend CComplexMatrix Complex(double x1, const CDoubleMatrix &v2);
    CComplexMatrix dot(const CComplexMatrix &m2)const ;
	AFX_EXT_CLASS friend CComplexMatrix dot(const CComplexMatrix &m1,const CComplexMatrix &m2);
    CComplexVector dot(const CComplexVector &v)const ;
	AFX_EXT_CLASS friend CComplexVector dot(const CComplexMatrix &m,const CComplexVector &v);
	AFX_EXT_CLASS friend CComplexVector dot(const CComplexVector &v, const CComplexMatrix &m);
	CComplexMatrix trans()const ;
	AFX_EXT_CLASS friend CComplexMatrix trans(const CComplexMatrix &m);
	CDoubleMatrix abs()const ;
	AFX_EXT_CLASS friend CDoubleMatrix abs(const CComplexMatrix &m);
    AFX_EXT_CLASS friend CIntVector any(const CComplexMatrix &v);
	CIntVector any()const ;
    AFX_EXT_CLASS friend CComplexVector Max(const CComplexMatrix &v);
	CComplexVector Max()const ;
    AFX_EXT_CLASS friend CComplexMatrix Max(const CComplexMatrix &v1, const CComplexMatrix &v2);
	CComplexMatrix Max(const CComplexMatrix &v2)const ;
    AFX_EXT_CLASS friend CComplexVector Min(const CComplexMatrix &v);
	CComplexVector Min()const ;
    AFX_EXT_CLASS friend CComplexMatrix Min(const CComplexMatrix &v1, const CComplexMatrix &v2);
	CComplexMatrix Min(const CComplexMatrix &v2)const ;
	CComplexMatrix & Rand();
	AFX_EXT_CLASS friend CComplexMatrix & Rand(CComplexMatrix &v);
	CComplexMatrix Rand(int m, int n)const ;
	CComplexMatrix inv()const ;
	AFX_EXT_CLASS friend CComplexMatrix inv(const CComplexMatrix &m);
	CComplexMatrix inv1()const ;
	AFX_EXT_CLASS friend CComplexMatrix inv1(const CComplexMatrix &m);
    CComplexVector eig()const ;
	AFX_EXT_CLASS friend CComplexVector eig(const CComplexMatrix &m);
	CComplexMatrix eig(CComplexMatrix &v)const ;
	AFX_EXT_CLASS friend CComplexMatrix eig(const CComplexMatrix &m, CComplexMatrix &v);
    CComplexMatrix expm2()const ;
	AFX_EXT_CLASS friend CComplexMatrix expm2(const CComplexMatrix &m);
	CComplexMatrix logm2()const ;
	AFX_EXT_CLASS friend CComplexMatrix logm2(const CComplexMatrix &m);
	CComplexMatrix sqrtm2()const ;
	AFX_EXT_CLASS friend CComplexMatrix sqrtm2(const CComplexMatrix &m);
	CComplexMatrix powm2(const CComplex &c)const ;
	AFX_EXT_CLASS friend CComplexMatrix powm2(const CComplexMatrix &m, const CComplex &c);
	CDoubleVector svd()const ; //老svd定义，已被svd_rank替代   //zfm
	AFX_EXT_CLASS friend CDoubleVector svd(const CComplexMatrix &m);
	CDoubleMatrix svd(const CComplexMatrix &y, const CComplexMatrix &z)const ;
	AFX_EXT_CLASS friend CDoubleMatrix svd(const CComplexMatrix &m, const CComplexMatrix &y, const CComplexMatrix &z);
};


////////////////////////////////////////////////////////////////////////////////
//////////////////------------- CTriple ---------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

template <class Type>
class AFX_EXT_CLASS CTriple
{
private:
	struct trep
	{ 
		int x;
		int y;
		Type val;
		int refcnt;
	}*p;
public:
	CTriple(int init=0);
	CTriple(int x0, int y0, Type val0);
    CTriple(const CTriple &t);
	~CTriple();

	CTriple<Type> & operator = (const CTriple<Type> &t);

	TEMPLAT_T friend CTriple T_T copy(const CTriple T_T &v);
	CTriple copy()const ;

	TEMPLAT_T friend CTriple<T_S> operator + (const CTriple<T_S> &t1);
	TEMPLAT_T friend CTriple<T_S> operator - (const CTriple<T_S> &t1);
     
	int & xpos(){return p->x;}
	int & ypos(){return p->y;}
	Type & value(){return p->val;}
	int & gxpos()const {return p->x;}
	int & gypos()const {return p->y;}
	Type & gvalue()const {return p->val;}
	TEMPLAT_T friend int operator == (const CTriple<T_S> &t1, const CTriple<T_S> &t2); 
	TEMPLAT_T friend int operator != (const CTriple<T_S> &t1, const CTriple<T_S> &t2); 
	TEMPLAT_T friend CTriple T_T operator * (T_S x, const CTriple T_T &v1);
	TEMPLAT_T friend CTriple T_T operator * (const CTriple T_T &v1, T_S x);
	TEMPLAT_T friend CTriple T_T operator / (T_S x, const CTriple T_T &v1);
	TEMPLAT_T friend CTriple T_T operator / (const CTriple T_T &v1, T_S x);


};

template <class Type>
CTriple<Type>::CTriple<Type>(int x0, int y0, Type val0)
{
	p = new trep;
	p->x = x0;
	p->y = y0;
	p->val = val0;
	p->refcnt = 1;

}

template <class Type>
CTriple<Type>::CTriple<Type>(int init)
{
	p=new trep;
	p->x =0;
	p->y =0;
	p->val = 0;
	p->refcnt = 1;
}

template <class Type>
CTriple<Type>::CTriple<Type>(const CTriple<Type> &t)
{ 
	t.p->refcnt++; 
	p=t.p;
}

template <class Type>
CTriple<Type>::~CTriple<Type>()
{ 
	if(--p->refcnt ==0) 
		delete p;
}

template <class Type>
CTriple<Type> & CTriple<Type>::operator = (const CTriple<Type> &t)
{ 
	t.p->refcnt++; 
	if(--p->refcnt ==0) 
		delete p; 
	p=t.p; 
	return *this;
}

template <class Type>
CTriple<Type> copy(const CTriple<Type> &t1)
{
    CTriple<Type> t(t1.xpos(), t1.ypos(), t1.value());
	return t;
}

template <class Type>
CTriple<Type> CTriple<Type>::copy()const 
{
    CTriple<Type> t(this->gxpos(), this->gypos(), this->gvalue());

	
	return t;
}

template <class Type>
int operator == (const CTriple<Type> &t1, const CTriple<Type> &t2)
{
    if(t1.xpos()!=t2.xpos() || t1.ypos()!=t2.ypos || t1.value() != t2.value())
			return 0;
	return 1;
}

template <class Type>
int operator != (const CTriple<Type> &t1, const CTriple<Type> &t2)
{
    if(t1.xpos()!=t2.xpos() || t1.ypos()!=t2.ypos || t1.value() != t2.value())
			return 1;
	return 0;
}


template <class Type>
CTriple<Type> operator + (const CTriple<Type> &t1)
{
    return t1;
}

template <class Type>
CTriple<Type> operator - (const CTriple<Type> &t1)
{
    CTriple<Type> t(t1.gxpos(),t1.gypos(), -t1.gvalue());
	return t;
}

template <class Type>
CTriple<Type> operator * (const CTriple<Type> &v1, Type x)
{
	CTriple<Type> t(v1.gxpos(), v1.gypos(), v1.gvalue()*x);

	return t;
}

template <class Type>
CTriple<Type> operator / (const CTriple<Type> &v1, Type x)
{
	CTriple<Type> t(v1.gxpos(), v1.gypos(), v1.gvalue()/x);
	return t;
}

template <class Type>
CTriple<Type> operator * (Type x, const CTriple<Type> &v1)
{
	CTriple<Type> t(v1.gxpos(), v1.gypos(), v1.gvalue()*x);
	return t;
}

template <class Type>
CTriple<Type> operator / (Type x, const CTriple<Type> &v1)
{
	CTriple<Type> t(v1.xpos(), v1.ypos(), x/v1.value());
	return t;
}


////////////////////////////////////////////////////////////////////////////////
//////////////////------------- CSparseMatrix ---------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class CComplex;
template<class Type>
class CMatrix;

template <class Type>
class AFX_EXT_CLASS CSparseMatrix:public CVector<CTriple<Type> >
{
protected:
	int rows,cols;

public:
	//constructor
	CSparseMatrix(int init=0):CVector<CTriple<Type> >(){rows=0; cols=0;}
	CSparseMatrix(Type *a, int xlength, int ylength);
	CSparseMatrix(int xsize, int ysize, Type init=0);
	CSparseMatrix(int len, CTriple<Type> init =0):CVector<CTriple<Type> >(len, init){rows=len; cols=len;}
	CSparseMatrix(Type **a, int xlength, int ylength);
	CSparseMatrix(const CMatrix<Type> &m);
	CSparseMatrix(const CSparseMatrix &x):CVector<CTriple<Type> >(x){rows = x.rows; cols = x.cols;}
	CSparseMatrix(const CVector<CTriple<Type> > &x):CVector<CTriple<Type> >(x){rows = 0; cols = 0;}
 	CSparseMatrix(const CSparseMatrix &a1, const CSparseMatrix &a2, int select = 2);
    CSparseMatrix(const CSparseMatrix &a, int xfirst, int xlast, int yfirst, int ylast, int flag = 0);   
    CSparseMatrix(const CSparseMatrix &a, int first, int last, int flag = 0);   


	TEMPLAT_T friend CSparseMatrix T_T copy(const CSparseMatrix T_T &v);
	CSparseMatrix copy()const ;

//	TEMPLAT_T friend int & mrow(const CSparseMatrix T_T &m){return m.rows;}
//	TEMPLAT_T friend int & mrow(const CSparseMatrix T_T *m){return m->rows;}
	const int & mrow()const {return rows;}

//	TEMPLAT_T friend int & mcol(const CSparseMatrix T_T &m){return m.cols;}
//	TEMPLAT_T friend int & mcol(CSparseMatrix T_T *m){return m->cols;}
	const int & mcol()const {return cols;}
	void SetSize(int nRows,int nCols){
		cols = nCols;
		rows = nRows;
	}

    void rowswap(int i, int j);
	void rowswap(int i, int j, CIntVector &rowidx);
	Type operator () (int x, int y);
	Type operator() (int x, int y, const CIntVector &ridx);
    CTriple<Type> & operator () (int i);

	void set(int x, int y, Type val);
	void set(int x, int y, CIntVector &ridx, Type val);
	Type & set(int x, int y);
	Type & set(int x, int y, CIntVector &ridx);

	void compress();

	CIntVector rowchidx()const ;
	CIntVector rowchidx(CIntVector &rowsize)const ;
	CIntVector colchidx()const ;
	CIntVector colchidx(const CIntVector &colsize)const ;

	CSparseMatrix & operator =(const CSparseMatrix & mat);
	CSparseMatrix & operator = (const CVector<CTriple<Type> > &m);
	CSparseMatrix & operator =(const CMatrix<Type> &m);
	CSparseMatrix & operator =(Type x);
	CSparseMatrix & operator =(Type **x);
	CSparseMatrix & operator =(Type *x);

	CSparseMatrix<Type> trans()const ;
	TEMPLAT_T friend CSparseMatrix<T_S> trans(const CSparseMatrix<Type> &m);
	
    TEMPLAT_T friend CSparseMatrix<T_S> operator + (const CSparseMatrix<T_S> &t1);
	TEMPLAT_T friend CSparseMatrix<T_S> operator - (const CSparseMatrix<T_S> &t1);

	TEMPLAT_T friend CSparseMatrix<T_S> operator + (const CSparseMatrix<T_S> &v1, const CSparseMatrix<T_S> &v2);
	TEMPLAT_T friend CSparseMatrix<T_S> operator - (const CSparseMatrix<T_S> &v1, const CSparseMatrix<T_S> &v2);
	TEMPLAT_T friend CSparseMatrix<T_S> operator * (const CSparseMatrix<T_S> &v1, const CSparseMatrix<T_S> &v2);



	TEMPLAT_T friend CSparseMatrix T_T operator + (const CSparseMatrix T_T &v1, T_S x);
	TEMPLAT_T friend CSparseMatrix T_T operator - (const CSparseMatrix T_T &v1, T_S x);
	TEMPLAT_T friend CSparseMatrix T_T operator / (const CSparseMatrix T_T &v1, T_S x);
	TEMPLAT_T friend CSparseMatrix T_T operator * (const CSparseMatrix T_T &v1, T_S x);

    TEMPLAT_T friend CSparseMatrix T_T operator + (T_S x, const CSparseMatrix T_T &v1);
    TEMPLAT_T friend CSparseMatrix T_T operator - (T_S x, const CSparseMatrix T_T &v1);
    TEMPLAT_T friend CSparseMatrix T_T operator / (T_S x, const CSparseMatrix T_T &v1);
    TEMPLAT_T friend CSparseMatrix T_T operator * (T_S x, const CSparseMatrix T_T &v1);


	CSparseMatrix & operator += (const CSparseMatrix &v1);
	CSparseMatrix & operator -= (const CSparseMatrix &v1);
	CSparseMatrix & operator *= (const CSparseMatrix &v1);

	CSparseMatrix & operator += (Type x);
	CSparseMatrix & operator -= (Type x);
	CSparseMatrix & operator *= (Type x);
	CSparseMatrix & operator /= (Type x);

	CSparseMatrix<Type> & prod(const CSparseMatrix<Type> & m2);
	TEMPLAT_T friend CSparseMatrix<T_S> prod(const CSparseMatrix<T_S> &v1, const CSparseMatrix<T_S> &v2);
};


template <class Type>
CSparseMatrix<Type>::CSparseMatrix(Type *a, int xlength, int ylength):CVector<CTriple<Type> >()
{
	rows = xlength;
	cols = ylength;
    for(int i=0; i<xlength; i++)
	{
		for(int j=0; j<ylength; j++)
		{
//			if (a[i*ylength+j] +(Type)1 != (Type)1)
			{
				p->length++;
			}
		}
	}
	p->elem = (CAllocate<CTriple<Type> > *)new CAllocate<CTriple<Type> >(p->length);
	int k=0;
	while(k<p->length)
	{
		for(int i=0; i<xlength; i++)
		{
			for(int j=0; j<ylength; j++)
			{
//				if (a[i*ylength+j] +(Type)1 != (Type)1)
				{
					(*(p->elem))(k) = CTriple<Type>(i,j,a[i*ylength+j]);
					k++;
				}
			}
		}
	}
}

template <class Type>
CSparseMatrix<Type>::CSparseMatrix(Type **a, int xlength, int ylength):CVector<CTriple<Type> >()
{
	rows = xlength;
	cols = ylength;
    for(int i=0; i<xlength; i++)
	{
		for(int j=0; j<ylength; j++)
		{
//			if (a[i][j] +(Type)1 != (Type)1)
			{
				p->length++;
			}
		}
	}
	p->elem = (CAllocate<CTriple<Type> >*)new CAllocate<CTriple<Type> >(p->length);
	int k=0;
	while(k<p->length)
	{
		for(int i=0; i<xlength; i++)
		{
			for(int j=0; j<ylength; j++)
			{
//				if (a[i][j] +(Type)1 != (Type)1)
				{
					(*(p->elem))(k) = CTriple<Type>(i,j,a[i][j]);
					k++;
				}
			}
		}
	}
}

template <class Type>
CSparseMatrix<Type>::CSparseMatrix(const CMatrix<Type> &matrix):CVector<CTriple<Type> >()
{
	rows = matrix.mrow();
	cols = matrix.mcol();
    for(int i=0; i<rows; i++)
	{
		for(int j=0; j<cols; j++)
		{
			Type valu = matrix.GetValue(i,j);
//			if (valu + (Type)1 != (Type)1)
			{
				p->length++;
			}
		}
	}
	p->elem = (CAllocate<CTriple<Type> > *)new CAllocate<CTriple<Type> >(p->length);
	int k=0;
	while(k<p->length)
	{
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<cols; j++)
			{
				Type valu = matrix.GetValue(i,j);
//				if (valu +(Type)1 != (Type)1)
				{
					(*(p->elem))(k) = CTriple<Type>(i,j,matrix.GetValue(i,j));
					k++;
				}
			}
		}
	}
}


template <class Type>
CSparseMatrix<Type>::CSparseMatrix(int xsize, int ysize, Type init):CVector<CTriple<Type> >()
{
	rows = xsize;
	cols = ysize;
//    if (init +(Type)1 != (Type)1)
	{
		p->length = rows*cols;
		p->elem = (CAllocate<CTriple<Type> > *)new CAllocate<CTriple<Type> >(p->length);
		for(int i=0; i<rows; i++)
			for(int j=0; j<cols; j++)
				(*(p->elem))(i*cols+j) = CTriple<Type>(i,j,init);
	}
}

template <class Type>
CSparseMatrix<Type>::CSparseMatrix(const CSparseMatrix<Type> &a, int first, int last, int flag):CVector<CTriple<Type> >()
{
	rows=0;
	cols=0;
	p->length = last - first + 1;
	p->refcnt = 1;
	p->elem = (CAllocate<CTriple<Type> >*)new CAllocate<CTriple<Type> >;
	p->elem->Create(last - first + 1);
	for(int i=0; i<p->length; i++)
		(*(p->elem))(i) = a.GetValue(i+first);
}

template <class Type>
CSparseMatrix<Type>::CSparseMatrix(const CSparseMatrix<Type> &a, int xfirst, int xlast, int yfirst, int ylast, int flag):CVector<CTriple<Type> >()
{
    int xsize = xlast - xfirst;
	int ysize = ylast - yfirst;

    if((xsize<0)||(ysize<0))
	{
		ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
		xsize =0;
		ysize =0;
	}
    if((xfirst<0)||(yfirst<0)||xlast>a.mrow()||ylast>a.mcol())
	{
		ErrorHandler(EMAT_OUTOFRANGE, __FILE__,__LINE__);
		xsize =0;
		ysize =0;
	}
	int k=0;
	int st=-1, ed=0;
	int i=0;
	for(i=0; i<a.vlen(); i++)
	{
		if(a.GetValue(i).gxpos()>=xfirst && a.GetValue(i).gypos()>=yfirst && a.GetValue(i).gxpos()<=xlast && a.GetValue(i).gypos()<=ylast)
		{
			if(st==-1)
			    st=i;
			ed=i;
            k++;
		}
		else if(i> xlast*a.mcol()+ ylast)
			break;
	}
	p->length = k;
	p->refcnt = 1;
	if(k>0)
	{
		p->elem = (CAllocate<CTriple<Type> > *)new CAllocate<CTriple<Type> >(k);
		
		k=0;
		for(i=st; i<=ed; i++)
		{
			if(a.GetValue(i).gxpos()>=xfirst && a.GetValue(i).gypos()>=yfirst && a.GetValue(i).gxpos()<=xlast && a.GetValue(i).gypos()<=ylast)
			{
				(*(p->elem))(k)=a.GetValue(i);
				if(flag==0)
				{
					(*(p->elem))(k) = a.GetValue(i).copy();
					(*(p->elem))(k).xpos() = (*(p->elem))(k).xpos()-xfirst;
					(*(p->elem))(k).ypos() = (*(p->elem))(k).ypos()-yfirst;
				}
				k++;
			}
		}
	}
	rows = xsize + 1;
	cols = ysize + 1;
}

template <class Type>
CTriple<Type> & CSparseMatrix<Type>::operator() (int i)
{
	if((i>=0)&&(i<p->length))
		return (*(p->elem))(i);	
	else
	{
		ErrorHandler(EMAT_OUTOFRANGE,__FILE__,__LINE__);
		return (*(p->elem))(0);
	}	
}

template <class Type>
Type CSparseMatrix<Type>::operator() (int x, int y)
{
	if(x>=0 && y>=0 && x<rows && y<cols)
    {
		for(int i=0; i<p->length; i++)
		{
			CTriple<Type> tr = (*this)(i);
			if(tr.xpos() == x && tr.ypos()==y)
				return tr.value();
			else if((tr.xpos()>x) || (tr.xpos()==x && tr.ypos()>y))
				return 0;       
		}
	}	
	else
	{
		ErrorHandler(EMAT_OUTOFRANGE,__FILE__,__LINE__);
	}	
	return 0;
}

template <class Type>
Type CSparseMatrix<Type>::operator() (int x, int y, const CIntVector &ridx)
{
	if(x>=0 && y>=0 && x<rows && y<cols)
    {
		for(int i=ridx.GetValue(x); i<ridx.GetValue(x+1); i++)
		{
			CTriple<Type> tr = (*this)(i);
			if(tr.ypos()==y)
				return tr.value();
			else if(tr.ypos()>y)
				return 0;       
		}
	}
	else
	{
		ErrorHandler(EMAT_OUTOFRANGE,__FILE__,__LINE__);
	}	
	return 0;
}

template <class Type>
void CSparseMatrix<Type>::compress()
{
    for(int i=0; i<vlen(); i++)
	{
		if((*this)(i).value()+1 == 1)
		{
            remove(i);
		}
	}
}

template <class Type>
void CSparseMatrix<Type>::set(int x, int y, Type val)
{
	if(x>=0 && y>=0 && x<rows && y<cols)
    {		
		if(val+1!=1)
		{
			int i=0;
			for(i=0; i<p->length; i++)
			{
				CTriple<Type> tr = (*this)(i);
				if(tr.xpos() == x && tr.ypos()==y)
				{
					tr.value() = val;
					return;
				}
				else if((tr.xpos()>x) || (tr.xpos()==x && tr.ypos()>y))
				{
					break;
				}
			}	
			CTriple<Type>tpm(x,y,val);
			insert(i,tpm);
			return;
		}
		else
		{
			for(int i=0; i<p->length; i++)
			{
				CTriple<Type> tr = (*this)(i);
				if(tr.xpos() == x && tr.ypos()==y)
				{
					remove(i);
					return;
				}
				else if((tr.xpos()>x) || (tr.xpos()==x && tr.ypos()>y))
				{
					return;
				}
			}						
		}
	}
	else
	{
		ErrorHandler(EMAT_OUTOFRANGE,__FILE__,__LINE__);
	}	
	return;
}

template <class Type>
void CSparseMatrix<Type>::set(int x, int y, CIntVector &ridx, Type val)
{
	if(x>=0 && y>=0 && x<rows && y<cols)
    {		
		if(val+1!=1)
		{
			int i =0;
			for(i=ridx(x); i<ridx(x+1); i++)
			{
				CTriple<Type> tr = (*this)(i);
				if(tr.ypos()==y)
				{
					tr.value() = val;
					return;
				}
				else if(tr.ypos()>y)
				{
					break;
				}
			}	
			CTriple<Type>tpm(x,y,val);
			insert(i,tpm);
			//insert(i,CTriple<Type>(x,y,val));
			for(i=x+1; i<ridx.vlen(); i++)
				ridx(i)++;
			return;
		}
		else
		{
			for(int i=ridx(x); i<ridx(x+1); i++)
			{
				CTriple<Type> tr = (*this)(i);
				if(tr.ypos()==y)
				{
					remove(i);
				    for(i=x+1; i<ridx.vlen(); i++)
				        ridx(i)--;
					return;
				}
				else if(tr.ypos()>y)
				{
					return;
				}
			}						
		}
	}
	else
	{
		ErrorHandler(EMAT_OUTOFRANGE,__FILE__,__LINE__);
	}	
	return;
}

template <class Type>
Type & CSparseMatrix<Type>::set(int x, int y)
{
	if(x>=0 && y>=0 && x<rows && y<cols)
    {	
		int i=0;	
		for(i=0; i<p->length; i++)
		{
			CTriple<Type> tr = (*this)(i);
			if(tr.xpos() == x && tr.ypos()==y)
			{
				return tr.value();
			}
			else if((tr.xpos()>x) || (tr.xpos()==x && tr.ypos()>y))
			{
				break;
			}
		}	
		CTriple<Type>tpm (x,y,0);
		insert(i,tpm);
		//insert(i,CTriple<Type>(x,y,0));
		CTriple<Type> tr = (*this)(i);
		return  tr.value();
	}
	else
	{
		ErrorHandler(EMAT_OUTOFRANGE,__FILE__,__LINE__);
		CTriple<Type> tr = (*this)(0);
		return tr.value();
	}		
}

template <class Type>
Type & CSparseMatrix<Type>::set(int x, int y, CIntVector &ridx)
{
	if(x>=0 && y>=0 && x<rows && y<cols)
    {		
		int i = 0;
		for(i=ridx(x); i<ridx(x+1); i++)
		{
			CTriple<Type> tr = (*this)(i);
			if(tr.ypos()==y)
			{
				return tr.value();
			}
			else if(tr.ypos()>y)
			{
				break;
			}
		}	
		CTriple<Type>tpm (x,y,0);
		insert(i,tpm);
		//insert(i,CTriple<Type>(x,y,0));
		for(int j=x+1; j<ridx.vlen(); j++)
			ridx(j)++;
		CTriple<Type> tr = (*this)(i);
		return  tr.value();
	}
	else
	{
		ErrorHandler(EMAT_OUTOFRANGE,__FILE__,__LINE__);
		CTriple<Type> tr = (*this)(0);
		return tr.value();
	}		
}

template <class Type>
CSparseMatrix<Type> & CSparseMatrix<Type>::operator = (Type a)
{
    if(p->length!=0)
	{
		if(--p->refcnt == 0)
		{
			delete []p->elem;
			delete p;
		}
		p = new vrep;
		p->refcnt=1;
		p->length=0;
		p->elem = NULL;
	}

	if(a+1 != 1)
	{
		p->length = rows*cols;
		p->elem = (CAllocate<CTriple<Type> >*)new CAllocate<CTriple<Type> >(p->length);
		for(int i=0; i<rows; i++)
			for(int j=0; j<cols; j++)
				(*(p->elem))(i*cols+j) = CTriple<Type>(i,j,a);
	}
    return (*this);
}

template <class Type>
CSparseMatrix<Type> & CSparseMatrix<Type>::operator = (Type *a)
{
    if(p->length!=0)
	{
		if(--p->refcnt == 0)
		{
			delete []p->elem;
			delete p;
		}
		p = new vrep;
		p->refcnt=1;
		p->length=0;
		p->elem = NULL;
	}
    for(int i=0; i<rows; i++)
	{
		for(int j=0; j<cols; j++)
		{
			if (a[i*cols+j] +1 != 1)
			{
				p->length++;
			}
		}
	}
	p->elem = (CAllocate<CTriple<Type> > *)new CAllocate<CTriple<Type> >(p->length);
	int k=0;
	while(k<p->length)
	{
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<cols; j++)
			{
				if (a[i*cols+j] +1 != 1)
				{
					(*(p->elem))(k) = CTriple<Type>(i,j,a[i*cols+j]);
					k++;
				}
			}
		}
	}
	return (*this);
}

template <class Type>
CSparseMatrix<Type> & CSparseMatrix<Type>::operator = (Type **a)
{
    if(p->length!=0)
	{
		if(--p->refcnt == 0)
		{
			delete []p->elem;
			delete p;
		}
		p = new vrep;
		p->refcnt=1;
		p->length=0;
		p->elem = NULL;
	}
    for(int i=0; i<rows; i++)
	{
		for(int j=0; j<cols; j++)
		{
			if (a[i][j] +1 != 1)
			{
				p->length++;
			}
		}
	}
	p->elem = (CAllocate<CTriple<Type> > *)new CAllocate<CTriple<Type> >(p->length);
	int k=0;
	while(k<p->length)
	{
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<cols; j++)
			{
				if (a[i][j] +1 != 1)
				{
					(*(p->elem))(k) = CTriple<Type>(i,j,a[i][j]);
					k++;
				}
			}
		}
	}
	return (*this);
}

template <class Type>
CSparseMatrix<Type> & CSparseMatrix<Type>::operator = (const CMatrix<Type> &m)
{
    rows = m.mrow();
	cols = m.mcol();
    if(p->length!=0)
	{
		if(--p->refcnt == 0)
		{
			delete []p->elem;
			delete p;
		}
		p = new vrep;
		p->refcnt=1;
		p->length=0;
		p->elem = NULL;
	}
    for(int i=0; i<rows; i++)
	{
		for(int j=0; j<cols; j++)
		{
			if (m.GetValue(i,j) +1 != 1)
			{
				p->length++;
			}
		}
	}
	p->elem = (CAllocate<CTriple<Type> >*)new CAllocate<CTriple<Type> >(p->length);
	int k=0;
	while(k<p->length)
	{
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<cols; j++)
			{
				if (m.GetValue(i,j) +1 != 1)
				{
					(*(p->elem))(k) = CTriple<Type>(i,j,m.GetValue(i,j));
					k++;
				}
			}
		}
	}
	return (*this);
}

template <class Type>
CSparseMatrix<Type> & CSparseMatrix<Type>::operator = (const CSparseMatrix<Type> &m)
{

	m.p->refcnt++;
	if(--p->refcnt == 0)
	{
		delete []p->elem;
		delete p;
	}
	p = m.p;
	rows = m.rows;
	cols = m.cols;
	return * this;
}

template <class Type>
CSparseMatrix<Type> & CSparseMatrix<Type>::operator = (const CVector<CTriple<Type> > &m)
{
	this->CVector<CTriple<Type> >::operator = (m);
    rows = 0;
	cols = 0;
	return * this;
}


template <class Type>
CSparseMatrix<Type> operator + (const CSparseMatrix<Type> &v1, const CSparseMatrix<Type> &v2)
{
	int rows = v1.mrow();
	int cols = v1.mcol();
	if(v1.mrow() != v2.mrow() || v1.mcol() != v2.mcol())
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__,__LINE__);
        rows = MIN(v1.mrow(), v2.mrow());
		cols = MIN(v1.mcol(), v2.mcol());
	}
	CSparseMatrix<Type> v(rows,cols);
	int cv1 = 0, cv2 = 0;
	CTriple<Type> t1, t2;
    int indv1, indv2;
	while(cv1<v1.vlen() && cv2<v2.vlen())
	{
		t1 = v1.GetValue(cv1);
		t2 = v2.GetValue(cv2);
		indv1 = t1.xpos()*cols + t1.ypos();
		indv2 = t2.xpos()*cols + t2.ypos();
		if(indv1<indv2)
		{
			v.append(v1.GetValue(cv1));
			cv1++;
		}
		else
		{
			if(indv1 == indv2)
			{
				Type sum = v1.GetValue(cv1).gvalue() + v2.GetValue(cv2).gvalue();
				if( sum + 1 != 1)
				{
					CTriple<Type> t(t1.xpos(), t1.ypos(), sum);
					v.append(t);
				}
				cv1++;
				cv2++;
			}
			else
			{
				v.append(v2.GetValue(cv2));
				cv2++;
			}
		}
	}
	for(; cv1<v1.vlen(); cv1++)
		v.append(v1.GetValue(cv1));
	for(; cv2<v2.vlen(); cv2++)
		v.append(v2.GetValue(cv2));
	return v;
}


template <class Type>
CSparseMatrix<Type> operator - (const CSparseMatrix<Type> &v1, const CSparseMatrix<Type> &v2)
{
	CSparseMatrix<Type> v3 = -v2;
	return v1 + v3;
}

template <class Type>
CSparseMatrix<Type> operator * (const CSparseMatrix<Type> &m1, const CSparseMatrix<Type> &m2)
{
	int m1rows = m1.mrow();
	int m1cols = m1.mcol();
	int m1vlen = m1.vlen();
	int m2rows = m2.mrow();
	int m2cols = m2.mcol();
	int m2vlen = m2.vlen();
	if(m1cols != m2rows)
	{
		m1.ErrorHandler(EMAT_INVALIDORDER, __FILE__, __LINE__);
        m1rows =0;
        m2rows =0;
		m1cols =0;
		m2rows =0;
	}
    int *rowsize = new int[m2rows];
	int *rowstart = new int[m2rows +1];
	Type *temp = new Type[m2cols];
	CSparseMatrix<Type> m(m1rows, m2cols);
	int i=0;
	for(i=0; i<m2rows; i++)
		rowsize[i] = 0;
	for(i=0; i<m2vlen; i++)
		rowsize[m2.GetValue(i).xpos()]++;
	rowstart[0] = 0;
	for(i=1; i<=m2rows; i++)
		rowstart[i] = rowstart[i-1] + rowsize[i-1];
	int current = 0, lastin = -1;
	while(current < m1vlen)
	{
		int rowa = m1(current).xpos();
		for(i=0; i<m2cols; i++)
			temp[i] = 0;
		while(current < m1vlen && m1(current).xpos() == rowa)
		{
			int cola = m1(current).ypos();
			for(i=rowstart[cola]; i<rowstart[cola+1]; i++)
			{
				int colb = m2(i).ypos();
				temp[colb] = temp[colb] + m1(current).value()*m2(i).value();
			}
			current ++;
		}
		for(i=0; i<m2cols; i++)
		{
			if(temp[i] != 0)
			{
				m.append(CTriple<Type>(rowa, i, temp[i]));
			}
		}
	}
	delete []rowsize;
	delete []rowstart;
	delete []temp;
	return m;		
}

template <class Type>
CSparseMatrix<Type> operator + (const CSparseMatrix<Type> &v1, Type x)
{
	if(x+1!=1)
	{
		CSparseMatrix<Type> v(v1.mrow(), v1.mcol());
		int vlen = v1.mrow()*v1.mcol();
		v.p->elem = new CTriple<Type>[vlen];
		v.p->length = vlen;
		int k=0; 
		for(int i=0; i<v1.mrow(); i++)
		{
			for(int j=0; j<v1.mcol(); j++)
			{
				if(k>=v1.vlen())
				{
                    v(i*v1.mcol()+j) = CTriple<Type>(i,j,x);
				}
				else
				{
					if(i== v1(k).xpos() && j==v1(k).ypos())
					{
						v(i*v1.mcol()+j) = CTriple<Type>(i,j,v1(k).value()+x);
						k++;
					}
					else if((i==v1(k).xpos() && j<v1(k).ypos()) || i<v1(k).xpos())
					{
						v(i*v1.mcol()+j) = CTriple<Type>(i,j,x);
					}
				}
			}
		}
		return v;
	}
	else
	{
		return v1.copy();
	}
}

template <class Type>
CSparseMatrix<Type> &  CSparseMatrix<Type>::operator += (const CSparseMatrix<Type> &v1)
{
	(*this) = (*this)+v1;
	return (*this);
}

template <class Type>
CSparseMatrix<Type> &  CSparseMatrix<Type>::operator -= (const CSparseMatrix<Type> &v1)
{
	(*this) = (*this)-v1;
	return (*this);
}

template <class Type>
CSparseMatrix<Type> &  CSparseMatrix<Type>::operator *= (const CSparseMatrix<Type> &v1)
{
	(*this) = (*this)*v1;
	return (*this);
}

template <class Type>
CSparseMatrix<Type> &  CSparseMatrix<Type>::operator += (Type x)
{
	(*this) = (*this)+x;
	return (*this);
}

template <class Type>
CSparseMatrix<Type> &  CSparseMatrix<Type>::operator -= (Type x)
{
	(*this) = (*this)-x;
	return (*this);
}

template <class Type>
CSparseMatrix<Type> &  CSparseMatrix<Type>::operator *= (Type x)
{
	(*this) = (*this)*x;
	return (*this);
}

template <class Type>
CSparseMatrix<Type> &  CSparseMatrix<Type>::operator /= (Type x)
{
	(*this) = (*this)/x;
	return (*this);
}


template <class Type>
CSparseMatrix<Type> operator + (Type x, const CSparseMatrix<Type> &v1)
{
    return v1 + x;
}


template <class Type>
CSparseMatrix<Type> operator - (const CSparseMatrix<Type> &v1, Type x)
{
    return v1 + (-x);
}

template <class Type>
CSparseMatrix<Type> operator - (Type x, const CSparseMatrix<Type> &v1)
{
    return x + (-v1);
}


template <class Type>
CSparseMatrix<Type> operator * (const CSparseMatrix<Type> &v1, Type x)
{
	CSparseMatrix<Type> v(v1.mrow(), v1.mcol());
	v.p->elem = (CAllocate<CTriple<Type> > *)new CAllocate<CTriple<Type> >(v1.vlen());
	v.p->length = v1.vlen();    
	for(int i=0; i<v1.vlen(); i++)
	    v(i) = v1.GetValue(i) * x;
    return v;
}

template <class Type>
CSparseMatrix<Type> operator * (Type x, const CSparseMatrix<Type> &v1)
{
	CSparseMatrix<Type> v(v1.mrow(), v1.mcol());
	v.p->elem = (CAllocate<CTriple<Type> > *)new CAllocate<CTriple<Type> >(v1.vlen());
	v.p->length = v1.vlen();    
	for(int i=0; i<v1.vlen(); i++)
	    v(i) = x * v1.GetValue(i);
    return v;
}

template <class Type>
CSparseMatrix<Type> operator / (const CSparseMatrix<Type> &v1, Type x)
{
	CSparseMatrix<Type> v(v1.mrow(), v1.mcol());
	v.p->elem = (CAllocate<CTriple<Type> >*) new CAllocate<CTriple<Type> >(v1.vlen());
	v.p->length = v1.vlen();    
	for(int i=0; i<v1.vlen(); i++)
	    v(i) = v1.GetValue(i) / x;
    return v;
}

template <class Type>
CSparseMatrix<Type> operator / (Type x, const CSparseMatrix<Type> &v1)
{
	CSparseMatrix<Type> v(v1.mrow(), v1.mcol());
	v.p->elem = (CAllocate<CTriple<Type> >*) new CAllocate<CTriple<Type> >(v1.vlen());
	v.p->length = v1.vlen();    
	for(int i=0; i<v1.vlen(); i++)
	    v(i) = x / v1.GetValue(i);
    return v;
}

template <class Type>
CSparseMatrix<Type> operator + (const CSparseMatrix<Type> &t1)
{
    return t1;
}

template <class Type>
CSparseMatrix<Type> operator - (const CSparseMatrix<Type> &t1)
{
    CSparseMatrix<Type> t(t1.mrow(), t1.mcol());
	t = - (CVector<CTriple<Type> > )t1;
//	t.rows = t1.mrow();
//	t.cols = t1.mcol();
	return t;
}

template <class Type>
CSparseMatrix<Type> prod(const CSparseMatrix<Type> &m1, const CSparseMatrix<Type> &m2)
{	
	int rows = m1.mrow();
	int cols = m1.mcol();
	if(m1.mrow() != m2.mrow() || m1.mcol() != m2.mcol())
	{
		m1.ErrorHandler(EMAT_INVALIDORDER, __FILE__,__LINE__);
        rows = MIN(m1.mrow(), m2.mrow());
		cols = MIN(m1.mcol(), m2.mcol());
	}
	CSparseMatrix<Type> m(rows,cols);
	int cv1 = 0, cv2 = 0;
	CTriple<Type> t1, t2;
    int indv1, indv2;
	while(cv1<m1.vlen() && cv2<m2.vlen())
	{
		t1 = m1.GetValue(cv1);
		t2 = m2.GetValue(cv2);
		indv1 = t1.xpos()*cols + t1.ypos();
		indv2 = t2.xpos()*cols + t2.ypos();
		if(indv1<indv2)
		{
			cv1++;
		}
		else
		{
			if(indv1 == indv2)
			{
				Type sum = m1.GetValue(cv1).gvalue() * m2.GetValue(cv2).gvalue();
				if( sum + 1 != 1)
				{
					CTriple<Type> t(t1.xpos(), t1.ypos(), sum);
					m.append(t);
				}
				cv1++;
				cv2++;
			}
			else
			{
				cv2++;
			}
		}
	}	
	return m;	
}


template <class Type>
CSparseMatrix<Type> & CSparseMatrix<Type>::prod( const CSparseMatrix<Type> &m2)
{	
	int rows = mrow();
	int cols = mcol();
	if(mrow() != m2.mrow() || mcol() != m2.mcol())
	{
		ErrorHandler(EMAT_INVALIDORDER, __FILE__,__LINE__);
        rows = MIN(mrow(), m2.mrow());
		cols = MIN(mcol(), m2.mcol());
	}
	CSparseMatrix<Type> m(rows,cols);
	int cv1 = 0, cv2 = 0;
	CTriple<Type> t1, t2;
    int indv1, indv2;
	while(cv1<m1.vlen() && cv2<m2.vlen())
	{
		t1 = (*this)(cv1);
		t2 = m2.GetValue(cv2);
		indv1 = t1.xpos()*cols + t1.ypos();
		indv2 = t2.xpos()*cols + t2.ypos();
		if(indv1<indv2)
		{
			cv1++;
		}
		else
		{
			if(indv1 == indv2)
			{
				Type sum = (*this)(cv1).value() * m2.GetValue(cv2).gvalue();
				if( sum + 1 != 1)
				{
					CTriple<Type> t(t1.xpos(), t1.ypos(), sum);
					m.append(t);
				}
				cv1++;
				cv2++;
			}
			else
			{
				cv2++;
			}
		}
	}	
	(*this)=m;
	return (*this);	
}

template <class Type>
CSparseMatrix<Type> copy(const CSparseMatrix<Type> &t1)
{
    CSparseMatrix<Type> t(t1.mrow(), t1.mcol());
	t = copy((CVector<CTriple<Type> > )t1);
	t.rows = t1.mrow();
	t.cols = t1.mcol();
	return t;
}

template <class Type>
CSparseMatrix<Type> CSparseMatrix<Type>::copy()const 
{
    CSparseMatrix<Type> t(mrow(), mcol());
	t = ::copy((CVector<CTriple<Type> > )(*this));
	t.rows = mrow();
	t.cols = mcol();
	return t;
}

template <class Type>
CSparseMatrix<Type> trans(const CSparseMatrix<Type> &t1)
{
    return t1.trans();
}

template <class Type>
CSparseMatrix<Type> CSparseMatrix<Type>::trans()const 
{
    int *rowsize = new int[mcol()];
	int *rowstart = new int[mcol()];
	CSparseMatrix<Type> m(mcol(), mrow());
	int size = vlen();
	if(size<0)
	{
		ErrorHandler(EMAT_INVALIDSIZE, __FILE__,__LINE__);
		size =0;
	}
	if(size>=UINT_MAX/sizeof(Type))
	{
		ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
		size =0;
	}
	m.p->elem = (CAllocate<CTriple<Type> > *) new CAllocate<CTriple<Type> >(size);
	m.p->length = size;

	if(vlen()>0)
	{
		int i=0; 
		for(i=0; i<mcol(); i++)
			rowsize[i] = 0;
		for(i=0; i<vlen(); i++)
			rowsize[(*this).GetValue(i).gypos()]++;
		rowstart[0] = 0;
		for(i=1; i<mcol(); i++)
			rowstart[i] = rowstart[i-1]+rowsize[i-1];
		for(i=0; i<vlen(); i++)
		{
			int j = rowstart[(*this).GetValue(i).gypos()];
		    m(j) = CTriple<Type> ( (*this).GetValue(i).gypos(), (*this).GetValue(i).gxpos(),
				(*this).GetValue(i).gvalue());
			rowstart[(*this).GetValue(i).gypos()]++;
		}
	}
	delete []rowsize;
	delete []rowstart;
	return m;			
}

template <class Type>
CIntVector CSparseMatrix<Type>::rowchidx()const 
{
	CIntVector rowsize(mrow());
	CIntVector idx(mrow()+1);
	int i=0;
	for(i=0; i<vlen(); i++)
		rowsize((*this).GetValue(i).gxpos())++;
	idx(0) = 0;
	for(i=1; i<mrow(); i++)
		idx(i) = idx(i-1)+rowsize(i-1);
	idx(i) = vlen();
	return idx;
}

template <class Type>
CIntVector CSparseMatrix<Type>::rowchidx(CIntVector &rowsize)const 
{
	if(rowsize.vlen() != mrow())
	{
     	CIntVector rowsz(mrow());
		rowsize = rowsz;
	}
	else
	{
		for(int i=0; i<mrow(); i++)
		    rowsize(i) = 0;
	}
	CIntVector idx(mrow()+1);
	int i=0;
	for(i=0; i<vlen(); i++)
		rowsize((*this).GetValue(i).gxpos())++;
	idx(0) = 0;
	for(i=1; i<mrow(); i++)
		idx(i) = idx(i-1)+rowsize(i-1);
	idx(i) = vlen();
	return idx;
}


template <class Type>
void CSparseMatrix<Type>::rowswap(int i, int j) //i和j的下限为零
{
	int k, counti=0, countj=0,countmin=0,countmax;
	int trlen=vlen();
	if (i==j) 
	{
		return;
	}
	if(i>j)
	{
		int t=j; 
		j=i; 
		i=t;
	}	
	CIntVector rowidx, rowsize;
	rowidx = rowchidx(rowsize);
    countmin = rowidx(i);
	countmax = rowidx(j);
	counti = rowsize(i);
	countj = rowsize(j);

	if (counti==countj)
	{
		
		for (k=0; k<counti; k++)
		{
			CTriple<Type> temp =(*this)(countmin+k);
			temp.xpos()=j;
			(*this)(countmin+k)=(*this)(countmax+k);
			(*this)(countmin+k).xpos()=i;
			(*this)(countmax+k)=temp;
		}
	}    
	else
	{
		CVector<CTriple<Type> > tr1(counti);
		CVector<CTriple<Type> > tr2(countj);
		for(k=0; k<counti; k++) 
		{
			tr1(k)=(*this)(countmin+k);
		}
		for(k=0; k<countj; k++) 
		{
			tr2(k)=(*this)(countmax+k);
		}				
		if(counti>countj)
		{
			for(k=0; k<countmax-countmin-counti; k++) //移动中间块
			{
				(*this)(countmin+countj+k)=(*this)(countmin+counti+k);
			}
		}
		else
		{
			for(k=countmax-countmin-counti-1; k>=0; k--) //移动中间块
			{
				(*this)(countmin+countj+k)=(*this)(countmin+counti+k);
			}
		}
		for(k=0; k<counti; k++)
		{
			(*this)(countmax+countj-counti+k) = tr1(k);
			(*this)(countmax+countj-counti+k).xpos()=j;
		}
		for(k=0; k<countj; k++)
		{
			(*this)(countmin+k) = tr2(k);
			(*this)(countmin+k).xpos()=i;
		}
	}	
}
	
template <class Type>
void CSparseMatrix<Type>::rowswap(int i, int j, CIntVector &rowidx) //i和j的下限为零
{
	int k, counti=0, countj=0,countmin=0,countmax;
	int trlen=vlen();
	if (i==j) 
	{
		return;
	}
	if(i>j)
	{
		int t=j; 
		j=i; 
		i=t;
	}	
	if(rowidx.vlen() != mrow()+1)
	{
		CIntVector rowidx1 = rowchidx();
		rowidx = rowidx1;
	}
    countmin = rowidx(i);
	countmax = rowidx(j);
	counti = rowidx(i+1) - rowidx(i);
	countj = rowidx(j+1) - rowidx(j);

	if (counti==countj)
	{
		
		for (k=0; k<counti; k++)
		{
			CTriple<Type> temp =(*this)(countmin+k);
			temp.xpos()=j;
			(*this)(countmin+k)=(*this)(countmax+k);
			(*this)(countmin+k).xpos()=i;
			(*this)(countmax+k)=temp;
		}
	}    
	else
	{
		CVector<CTriple<Type> > tr1(counti);
		CVector<CTriple<Type> > tr2(countj);
		for(k=0; k<counti; k++) 
		{
			tr1(k)=(*this)(countmin+k);
		}
		for(k=0; k<countj; k++) 
		{
			tr2(k)=(*this)(countmax+k);
		}	
		if(counti>countj)
		{
			for(k=0; k<countmax-countmin-counti; k++) //移动中间块
			{
				(*this)(countmin+countj+k)=(*this)(countmin+counti+k);
			}
		}
		else
		{
			for(k=countmax-countmin-counti-1; k>=0; k--) //移动中间块
			{
				(*this)(countmin+countj+k)=(*this)(countmin+counti+k);
			}
		}
		for(k=0; k<counti; k++)
		{
			(*this)(countmax+countj-counti+k) = tr1(k);
			(*this)(countmax+countj-counti+k).xpos()=j;
		}
		for(k=0; k<countj; k++)
		{
			(*this)(countmin+k) = tr2(k);
			(*this)(countmin+k).xpos()=i;
		}
		for(k=i+1; k<j+1; k++)
		{
			rowidx(k) += countj-counti;
		}
	}	
}
	



template <class Type>
CIntVector CSparseMatrix<Type>::colchidx()const 
{
    CSparseMatrix<Type> p = trans();
	return p.rowchidx();
}

template <class Type>
CIntVector CSparseMatrix<Type>::colchidx(const CIntVector &idxidx)const 
{
	CSparseMatrix<Type> p = trans();
	CIntVector colidx = p.rowchidx(idx);
	return colidx;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////------------- CSparseDbMatrix ---------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

class CSparseCompMatrix;
class AFX_EXT_CLASS CSparseDbMatrix : public CSparseMatrix<double>
{
public:

	CSparseDbMatrix():CSparseMatrix<double>(){};
	CSparseDbMatrix(int xsize, int ysize, double init =0.0):CSparseMatrix<double>(xsize, ysize, init){};
	CSparseDbMatrix(const CSparseMatrix<double> &x):CSparseMatrix<double>(x){};
	CSparseDbMatrix(const CSparseDbMatrix &x):CSparseMatrix<double>(x){};
	CSparseDbMatrix(double *a, int xlength, int ylength=0):CSparseMatrix<double>(a,xlength,ylength){};
	CSparseDbMatrix(double **a, int xlength, int ylength=0):CSparseMatrix<double>(a,xlength,ylength){};
	CSparseDbMatrix(const CDoubleMatrix &x):CSparseMatrix<double>((CMatrix<double>)x){};
	CSparseDbMatrix(const CMatrix<double> &x):CSparseMatrix<double>(x){};
    CSparseDbMatrix(const CSparseDbMatrix &a, int xfirst, int xlast, int yfirst, int ylast, int flag = 0):CSparseMatrix<double>(a,xfirst,xlast,yfirst,ylast,flag){};   
    CSparseDbMatrix(const CSparseDbMatrix &a, int first, int last, int flag = 0):CSparseMatrix<double>(a,first,last,flag){};   
	CSparseDbMatrix(int len, CTriple<double> init =0):CSparseMatrix<double>(len, init){};


public:


	CSparseDbMatrix & operator =(const CSparseDbMatrix &x);
	CSparseDbMatrix & operator =(const CSparseMatrix<double> &x);

	CSparseDbMatrix & operator =(const CDoubleMatrix &x);
	CSparseDbMatrix & operator =(double **x);
	CSparseDbMatrix & operator =(double *x);
	CSparseDbMatrix & operator =(double x);
	void compress();

	AFX_EXT_CLASS friend double dot(const CSparseDbMatrix &tr1, const CSparseDbMatrix &tr2);


	CSparseDbMatrix qr(CSparseDbMatrix &q, int flag =1);
	AFX_EXT_CLASS friend CSparseDbMatrix qr(CSparseDbMatrix &x, CSparseDbMatrix &q, int flag = 1); 
    CSparseDbMatrix qr(CSparseDbMatrix &q, CIntVector &e, int flag = 1);
	AFX_EXT_CLASS friend CSparseDbMatrix qr(CSparseDbMatrix &x, CSparseDbMatrix &q, CIntVector &e, int flag = 1); 

	AFX_EXT_CLASS friend CSparseDbMatrix inv(const CSparseDbMatrix &m);
    CSparseDbMatrix inv()const ;

	AFX_EXT_CLASS friend double det(const CSparseDbMatrix &mm);
    double det()const ;

    CSparseDbMatrix hess()const ;
	AFX_EXT_CLASS friend CSparseDbMatrix hess(const CSparseDbMatrix &m);
	CSparseDbMatrix hess(CSparseDbMatrix &v)const ;
	AFX_EXT_CLASS friend CSparseDbMatrix hess(const CSparseDbMatrix &m, CSparseDbMatrix &v);

	CSparseCompMatrix eig(CSparseCompMatrix &s)const ;
};





////////////////////////////////////////////////////////////////////////////////
//////////////////------------- CSparseCompMatrix ---------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CSparseCompMatrix : public CSparseMatrix<CComplex>
{
public:

	CSparseCompMatrix():CSparseMatrix<CComplex>(){};
	CSparseCompMatrix(int xsize, int ysize, CComplex init =0.0):CSparseMatrix<CComplex>(xsize, ysize, init){};
	CSparseCompMatrix(const CSparseMatrix<CComplex> &x):CSparseMatrix<CComplex>(x){};
	CSparseCompMatrix(const CSparseCompMatrix &x):CSparseMatrix<CComplex>(x){};
	CSparseCompMatrix(CComplex *a, int xlength, int ylength=0):CSparseMatrix<CComplex>(a,xlength,ylength){};
	CSparseCompMatrix(CComplex **a, int xlength, int ylength=0):CSparseMatrix<CComplex>(a,xlength,ylength){};
	CSparseCompMatrix(const CComplexMatrix &x):CSparseMatrix<CComplex>((CMatrix<CComplex>)x){};
	CSparseCompMatrix(const CMatrix<CComplex> &x):CSparseMatrix<CComplex>(x){};
    CSparseCompMatrix(const CSparseCompMatrix &a, int xfirst, int xlast, int yfirst, int ylast, int flag = 0):CSparseMatrix<CComplex>(a,xfirst,xlast,yfirst,ylast,flag){};   
	CSparseCompMatrix(int len, CTriple<CComplex> init =0):CSparseMatrix<CComplex>(len, init){};
    CSparseCompMatrix(const CSparseCompMatrix &a, int first, int last, int flag = 0):CSparseMatrix<CComplex>(a,first,last,flag){};   
	virtual ~CSparseCompMatrix()
	{
		destroy();
	}

public:


	CSparseCompMatrix & operator =(const CSparseCompMatrix &x);
	CSparseCompMatrix & operator =(const CSparseMatrix<CComplex> &x);

	CSparseCompMatrix & operator =(const CComplexMatrix &x);
	CSparseCompMatrix & operator =(CComplex **x);
	CSparseCompMatrix & operator =(CComplex *x);
	CSparseCompMatrix & operator =(CComplex x);
	void compress();

	CSparseDbMatrix real()const ;
	AFX_EXT_CLASS friend CSparseDbMatrix real(const CSparseCompMatrix &v);
	CSparseDbMatrix imag()const ;
	AFX_EXT_CLASS friend CSparseDbMatrix imag(const CSparseCompMatrix &v);
	CSparseCompMatrix conj()const ;
	AFX_EXT_CLASS friend CSparseCompMatrix conj(const CSparseCompMatrix &v);

	AFX_EXT_CLASS friend CSparseCompMatrix Complex(const CSparseDbMatrix &v1, const CSparseDbMatrix &v2);
	AFX_EXT_CLASS friend CSparseCompMatrix Complex(const CSparseDbMatrix &v1, double x=0);
	AFX_EXT_CLASS friend CSparseCompMatrix Complex(double x1, const CSparseDbMatrix &v2);



};

template <class Type>
class CSymMatrix;
////////////////////////////////////////////////////////////////////////////////
//////////////////------------- CTriangleMatrix ---------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

template <class Type>
class AFX_EXT_CLASS CTriangleMatrix:public CVector<Type>
{
public:

	CTriangleMatrix<Type>():CVector<Type>(){};
	CTriangleMatrix<Type>(int xsize, int ysize=0, Type init =0):CVector<Type>(xsize*(xsize+1)/2, init){};
	CTriangleMatrix<Type>(const CVector<Type> &x):CVector<Type>(x){};
	CTriangleMatrix<Type>(const CTriangleMatrix<Type> &x):CVector<Type>(x){};
	CTriangleMatrix<Type>(Type *a, int xlength, int ylength=0);
	CTriangleMatrix<Type>(Type **a, int xlength, int ylength=0);
	CTriangleMatrix<Type>(const CMatrix<Type> &x);
//    CTriangleMatrix<Type>(CSymMatrix<Type> &x):CVector<Type>(x){};
public:


	CTriangleMatrix<Type> & operator =(const CTriangleMatrix<Type> &x);
	CTriangleMatrix<Type> & operator =(const CMatrix<Type> &x);
	CTriangleMatrix<Type> & operator =(const CVector<Type> &x);
	CTriangleMatrix<Type> & operator =(Type **x);
	CTriangleMatrix<Type> & operator =(Type *x);
	CTriangleMatrix<Type> & operator =(Type x);
	Type & operator() (int i);
    Type & operator() (int i, int j);

//	AFX_EXT_CLASS TEMPLAT_T friend int mrow(const CTriangleMatrix<T_S> &m){return vlentorow(m.vlen());}
//	AFX_EXT_CLASS TEMPLAT_T friend int mrow(const CTriangleMatrix<T_S> *m){return vlentorow(m->vlen());}
	int mrow()const {return vlentorow(vlen());}

	//AFX_EXT_CLASS TEMPLAT_T friend int mcol(const CTriangleMatrix<T_S> &m){return vlentorow(m.vlen());}
	//AFX_EXT_CLASS TEMPLAT_T friend int mcol(const CTriangleMatrix<T_S> *m){return vlentorow(m->vlen());}
	int mcol()const {return vlentorow(vlen());}
	
	AFX_EXT_CLASS TEMPLAT_T friend CTriangleMatrix<T_S> dmattotragmat(const CMatrix<T_S> &m);

	AFX_EXT_CLASS TEMPLAT_T friend CVector<T_S> diag(const CTriangleMatrix<T_S> &v2);
	CVector<Type> diag()const ;
	AFX_EXT_CLASS TEMPLAT_T friend CVector<T_S> diag(const CTriangleMatrix<T_S> &v2, int k);
	CVector<Type> diag(int k)const ;
};

typedef CTriangleMatrix<int> CTriangleIntMatrix;
typedef CTriangleMatrix<float> CTriangleFtMatrix;


template <class Type>
CTriangleMatrix<Type>::CTriangleMatrix<Type>(Type *a, int xlength, int ylength):CVector<Type>(xlength*(xlength+1)/2)
{
	int k=0,flag=0;
	int ven = vlen();
    while(k<ven)
	{
	    for(int i=0; i<xlength; i++)
	    	for(int j=0; j<xlength-i; j++)
			{
				if(i!=0 && a[j*xlength+(j+i)]!= 0)
					flag=1;					
	            (*this)(k)= a[j*xlength+j+i];
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONTRIANGlE, __FILE__, __LINE__);
	}
}

template <class Type>
CTriangleMatrix<Type>::CTriangleMatrix<Type>(Type **a, int xlength, int ylength):CVector<Type>(xlength*(xlength+1)/2)
{
	int k=0, flag=0;
	int ven = vlen();
    while(k<ven)
	{
	    for(int i=0; i<xlength; i++)
	    	for(int j=0; j<xlength-i; j++)
			{
				if(i!=0 && a[j][j+i]!=a[j+i][j])
					flag=1;					
	            (*this)(k)= a[j][j+i];
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONTRIANGlE, __FILE__, __LINE__);
	}
}

template <class Type>
CTriangleMatrix<Type>::CTriangleMatrix<Type>(const CMatrix<Type> &x):CVector<Type>(x.mrow()*(x.mrow()+1)/2)
{
	int k=0, flag=0;
	int ven = vlen();
	int len = x.mrow();
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{
				if(i!=0 && x.GetValue(j,j+i)!=0 )
					flag=1;					
	            (*this)(k)= x.GetValue(j,j+i);
				k++;
			}
	}
	if(flag == 1)
	{
//        WarningHandler(WEMAT_NONTRIANGlE, __FILE__, __LINE__);
	}
}

template <class Type>
CTriangleMatrix<Type> & CTriangleMatrix<Type>::operator = (const CTriangleMatrix<Type> & vec)
{
	this->CVector<Type>::operator = (vec);
	return * this;
}

template <class Type>
CTriangleMatrix<Type> & CTriangleMatrix<Type>::operator = (const CMatrix<Type> & x)
{
    (*this) = dmattotragmat(x);
	return * this;
}

template <class Type>
CTriangleMatrix<Type> & CTriangleMatrix<Type>::operator = (const CVector<Type> & vec)
{
	this->CVector<Type>::operator = (vec);
	return * this;
}

template <class Type>
CTriangleMatrix<Type> & CTriangleMatrix<Type>::operator =(Type **x)
{
    int len = mrow();
	int ven = vlen();
	int k=0, flag=0;
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{
				if(i!=0 && x[j][j+i]!=0)
					flag=1;					
	            (*this)(k)= x[j][j+i];
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONTRIANGlE, __FILE__, __LINE__);
	}
	return * this;
}

template <class Type>
CTriangleMatrix<Type> & CTriangleMatrix<Type>::operator =(Type *x)
{
    int len = mrow();
	int ven = vlen();
	int k=0, flag=0;
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{
				if(i!=0 && x[j*len+j+i]!=0)
					flag=1;					
	            (*this)(k)= x[j*len+j+i];
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONTRIANGlE, __FILE__, __LINE__);
	}
	return * this;
}

template <class Type>
CTriangleMatrix<Type> & CTriangleMatrix<Type>::operator =(Type x)
{
    this->CVector<Type>::operator =(x);
	return * this;
}

template <class Type>
CTriangleMatrix<Type> dmattotragmat(const CMatrix<Type> &m)
{
	int len=m.mrow();
    CTriangleMatrix<Type> x(m.mrow(),m.mrow());
	int ven=x.vlen();
	int k=0, flag=0;
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{
				if(i!=0 && m.GetValue(j,j+i)!=0)
					flag=1;					
	            x(k)= m.GetValue(j,j+i);
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONTRIANGlE, __FILE__, __LINE__);
	}
	return x;
}

template <class Type>
Type & CTriangleMatrix<Type>::operator() (int i, int j)//?????
{
	if(i>j)
	{
        return (*this)(0);
	}
	int n=mrow();
	int ji = j-i;
	return (*this)((2*n-ji)*(ji+1)/2-n+j);
}

template <class Type>
Type & CTriangleMatrix<Type>::operator() (int i)
{
	return (*(p->elem))(i);
}

template <class Type>
CVector<Type> diag(const CTriangleMatrix<Type> &v2)
{
	int n=v2.mrow();
	CVector<Type> x(n);
	for(int i=0; i<n; i++)
		x(i)=v2.GetValue(i);
	return x;
}

template <class Type>
CVector<Type> CTriangleMatrix<Type>::diag()const 
{
	int n=mrow();
	CVector<Type> x(n);
	for(int i=0; i<n; i++)
		x(i)=(*this).GetValue(i);
	return x;
}

template <class Type>
CVector<Type> diag(const CTriangleMatrix<Type> &v2, int k)
{
	int n = v2.mcol();
	int nn, i;
	if(((k>0)&&(k>(n-1)))||((k<0)&&(-k>(n-1))))
	{
		v2.ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
		k = 0;
	}
	int kk = abs(k);
	nn = n-kk;
	CVector<Type> x(nn);
	if(k<0)
	    return x;
	for(i=0;i<nn;i++)
		x(i) = v2.GetValue((2*nn+kk)*(kk+1)/2-nn+i);
	return x;
}

template <class Type>
CVector<Type> CTriangleMatrix<Type>::diag(int k)const 
{
	return ::diag(*this)( k);
}

////////////////////////////////////////////////////////////////////////////////
//////////////////----------- CTriangleDbMatrix ------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CTriangleDbMatrix :public CTriangleMatrix<double>  
{
public:

	CTriangleDbMatrix():CTriangleMatrix<double>(){};
	CTriangleDbMatrix(int xsize, int ysize=0, double init =0.0):CTriangleMatrix<double>(xsize, ysize, init){};
	CTriangleDbMatrix(const CTriangleMatrix<double> &x):CTriangleMatrix<double>(x){};
	CTriangleDbMatrix(const CTriangleDbMatrix &x):CTriangleMatrix<double>(x){};
	CTriangleDbMatrix(const CVector<double> &x):CTriangleMatrix<double>(x){};
	CTriangleDbMatrix(const CDoubleVector &x):CTriangleMatrix<double>(x){};
	CTriangleDbMatrix(double *a, int xlength, int ylength=0):CTriangleMatrix<double>(a,xlength,ylength){};
	CTriangleDbMatrix(double **a, int xlength, int ylength=0):CTriangleMatrix<double>(a,xlength,ylength){};
	CTriangleDbMatrix(const CDoubleMatrix &x);
	CTriangleDbMatrix(const CMatrix<double> &x):CTriangleMatrix<double>(x){};

public:


	CTriangleDbMatrix & operator =(const CTriangleDbMatrix &x);
	CTriangleDbMatrix & operator =(const CTriangleMatrix<double> &x);
	CTriangleDbMatrix & operator =(const CDoubleMatrix &x);
	CTriangleDbMatrix & operator =(const CVector<double> &x);
	CTriangleDbMatrix & operator =(const CDoubleVector &x);
	CTriangleDbMatrix & operator =(double **x);
	CTriangleDbMatrix & operator =(double *x);
	CTriangleDbMatrix & operator =(double x);
};

////////////////////////////////////////////////////////////////////////////////
//////////////////------------- CSymMatrix ---------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

template <class Type>
class AFX_EXT_CLASS CSymMatrix:public CVector<Type>
{
public:

	CSymMatrix<Type>():CVector<Type>(){};
	CSymMatrix<Type>(int xsize, int ysize=0, Type init =0):CVector<Type>(xsize*(xsize+1)/2, init){};
	CSymMatrix<Type>(const CVector<Type> &x):CVector<Type>(x){};
	CSymMatrix<Type>(const CSymMatrix<Type> &x):CVector<Type>(x){};
	CSymMatrix<Type>(Type *a, int xlength, int ylength=0);
	CSymMatrix<Type>(Type **a, int xlength, int ylength=0);
	CSymMatrix<Type>(const CMatrix<Type> &x);
//	CSymMatrix<Type>(CTriangleMatrix<Type> &x):CVector<Type>(x){};

public:


	CSymMatrix<Type> & operator =(const CSymMatrix<Type> &x);
	CSymMatrix<Type> & operator =(const CMatrix<Type> &x);
	CSymMatrix<Type> & operator =(const CVector<Type> &x);
	CSymMatrix<Type> & operator =(Type **x);
	CSymMatrix<Type> & operator =(Type *x);
	CSymMatrix<Type> & operator =(Type x);
	Type & operator() (int i);
    Type & operator() (int i, int j);

	//AFX_EXT_CLASS TEMPLAT_T friend int mrow(const CSymMatrix<T_S> &m){return vlentorow(m.vlen());}
	//AFX_EXT_CLASS TEMPLAT_T friend int mrow(const CSymMatrix<T_S> *m){return vlentorow(m->vlen());}
	int mrow()const {return vlentorow(vlen());}

	//AFX_EXT_CLASS TEMPLAT_T friend int mcol(const CSymMatrix<T_S> &m){return vlentorow(m.vlen());}
	//AFX_EXT_CLASS TEMPLAT_T friend int mcol(const CSymMatrix<T_S> *m){return vlentorow(m->vlen());}
	int mcol()const {return vlentorow(vlen());}
	
    AFX_EXT_CLASS friend int vlentorow(int vlen);
	AFX_EXT_CLASS TEMPLAT_T friend CSymMatrix<T_S> dmattosmat(const CMatrix<T_S> &m);

	AFX_EXT_CLASS TEMPLAT_T friend CVector<T_S> diag(const CSymMatrix<T_S> &v2);
	CVector<Type> diag()const ;
	AFX_EXT_CLASS TEMPLAT_T friend CVector<T_S> diag(const CSymMatrix<T_S> &v2, int k);
	CVector<Type> diag(int k)const ;
 
};

typedef CSymMatrix<int> CSymIntMatrix;
typedef CSymMatrix<float> CSymFtMatrix;


template <class Type>
CSymMatrix<Type>::CSymMatrix<Type>(Type *a, int xlength, int ylength):CVector<Type>(xlength*(xlength+1)/2)
{
	int k=0,flag=0;
	int ven = vlen();
    while(k<ven)
	{
	    for(int i=0; i<xlength; i++)
	    	for(int j=0; j<xlength-i; j++)
			{
//				if(i!=0 && a[j*xlength+(j+i)]!=a[(j+i)*xlength+j])
					flag=1;					
	            (*this)(k)= a[j*xlength+j+i];
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONSYMMETRIC, __FILE__, __LINE__);
	}
}

template <class Type>
CSymMatrix<Type>::CSymMatrix<Type>(Type **a, int xlength, int ylength):CVector<Type>(xlength*(xlength+1)/2)
{
	int k=0, flag=0;
	int ven = vlen();
    while(k<ven)
	{
	    for(int i=0; i<xlength; i++)
	    	for(int j=0; j<xlength-i; j++)
			{
//				if(i!=0 && a[j][j+i]!=a[j+i][j])
					flag=1;					
	            (*this)(k)= a[j][j+i];
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONSYMMETRIC, __FILE__, __LINE__);
	}
}

template <class Type>
CSymMatrix<Type>::CSymMatrix<Type>(const CMatrix<Type> &x):CVector<Type>(x.mrow()*(x.mrow()+1)/2)
{
	int k=0, flag=0;
	int ven = vlen();
	int len = x.mrow();
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{
//				if(i!=0 && x(j,j+i)!=x(j+i,j))
					flag=1;					
	            (*this)(k)= x.GetValue(j,j+i);
				k++;
			}
	}
	if(flag == 1)
	{
//        WarningHandler(WEMAT_NONSYMMETRIC, __FILE__, __LINE__);
	}
}

template <class Type>
CSymMatrix<Type> & CSymMatrix<Type>::operator = (const CSymMatrix<Type> & vec)
{
	this->CVector<Type>::operator = (vec);
	return * this;
}

template <class Type>
CSymMatrix<Type> & CSymMatrix<Type>::operator = (const CMatrix<Type> & x)
{
    (*this) = dmattosmat(x);
	return * this;
}

template <class Type>
CSymMatrix<Type> & CSymMatrix<Type>::operator = (const CVector<Type> & vec)
{
	this->CVector<Type>::operator = (vec);
	return * this;
}

template <class Type>
CSymMatrix<Type> & CSymMatrix<Type>::operator =(Type **x)
{
    int len = mrow();
	int ven = vlen();
	int k=0, flag=0;
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{
				if(i!=0 && x[j][j+i]!=x[j+i][j])
					flag=1;					
	            (*this)(k)= x[j][j+i];
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONSYMMETRIC, __FILE__, __LINE__);
	}
	return * this;
}

template <class Type>
CSymMatrix<Type> & CSymMatrix<Type>::operator =(Type *x)
{
    int len = mrow();
	int ven = vlen();
	int k=0, flag=0;
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{
				if(i!=0 && x[j*len+j+i]!=x[(j+i)*len+j])
					flag=1;					
	            (*this)(k)= x[j*len+j+i];
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONSYMMETRIC, __FILE__, __LINE__);
	}
	return * this;
}

template <class Type>
CSymMatrix<Type> & CSymMatrix<Type>::operator =(Type x)
{
    this->CVector<Type>::operator =(x);
	return * this;
}

template <class Type>
CSymMatrix<Type> dmattosmat(const CMatrix<Type> &m)
{
	int len=m.mrow();
    CSymMatrix<Type> x(m.mrow(),m.mrow());
	int ven=x.vlen();
	int k=0, flag=0;
    while(k<ven)
	{
	    for(int i=0; i<len; i++)
	    	for(int j=0; j<len-i; j++)
			{
				if(i!=0 && m.GetValue(j,j+i)!=m.GetValue(j+i,j))
					flag=1;					
	            x(k)= m.GetValue(j,j+i);
				k++;
			}
	}
	if(flag == 1)
	{
        WarningHandler(WEMAT_NONSYMMETRIC, __FILE__, __LINE__);
	}
	return x;
}

template <class Type>
Type & CSymMatrix<Type>::operator() (int i, int j)//?????
{
	if(i>j)
	{
		int t=j;
		j=i;
		i=t;
	}
	int n=mrow();
	int ji = j-i;
	return (*this)((2*n-ji)*(ji+1)/2-n+j);
}

template <class Type>
Type & CSymMatrix<Type>::operator() (int i)
{
//	return (*this)[i];		
	return (*(p->elem))(i);
}

template <class Type>
CVector<Type> diag(const CSymMatrix<Type> &v2)
{
	int n=v2.mrow();
	CVector<Type> x(n);
	for(int i=0; i<n; i++)
		x(i)=v2.GetValue(i);
	return x;
}

template <class Type>
CVector<Type> CSymMatrix<Type>::diag()const 
{
	int n=mrow();
	CVector<Type> x(n);
	for(int i=0; i<n; i++)
		x(i)=(*this).GetValue(i);
	return x;
}

template <class Type>
CVector<Type> diag(const CSymMatrix<Type> &v2, int k)
{
	int n = v2.mcol();
	int nn, i;
	if(((k>0)&&(k>(n-1)))||((k<0)&&(-k>(n-1))))
	{
		v2.ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
		k = 0;
	}
	int kk = abs(k);
	nn = n-kk;
	CVector<Type> x(nn);
	for(i=0;i<nn;i++)
		x(i) = v2((2*nn+kk)*(kk+1)/2-nn+i);
	return x;
}

template <class Type>
CVector<Type> CSymMatrix<Type>::diag(int k)const 
{
	return ::diag(*this)( k);
}



////////////////////////////////////////////////////////////////////////////////
//////////////////----------- CSymDbMatrix ------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CSymDbMatrix :public CSymMatrix<double>  
{
public:

	CSymDbMatrix():CSymMatrix<double>(){};
	CSymDbMatrix(int xsize, int ysize=0, double init =0.0):CSymMatrix<double>(xsize, ysize, init){};
	CSymDbMatrix(const CSymMatrix<double> &x):CSymMatrix<double>(x){};
	CSymDbMatrix(const CSymDbMatrix &x):CSymMatrix<double>(x){};
	CSymDbMatrix(const CVector<double> &x):CSymMatrix<double>(x){};
	CSymDbMatrix(const CDoubleVector &x):CSymMatrix<double>(x){};

	CSymDbMatrix(double *a, int xlength, int ylength=0):CSymMatrix<double>(a,xlength,ylength){};
	CSymDbMatrix(double **a, int xlength, int ylength=0):CSymMatrix<double>(a,xlength,ylength){};
	CSymDbMatrix(const CDoubleMatrix &x);
	CSymDbMatrix(const CMatrix<double> &x):CSymMatrix<double>(x){};

public:


	CSymDbMatrix & operator =(const CSymDbMatrix &x);
	CSymDbMatrix & operator =(const CSymMatrix<double> &x);

	CSymDbMatrix & operator =(const CDoubleMatrix &x);
	CSymDbMatrix & operator =(const CVector<double> &x);
	CSymDbMatrix & operator =(const CDoubleVector &x);
	CSymDbMatrix & operator =(const CIntMatrix &x);
	CSymDbMatrix & operator =(const CFloatMatrix &x);
	CSymDbMatrix & operator =(double **x);
	CSymDbMatrix & operator =(double *x);
	CSymDbMatrix & operator =(double x);


	
	AFX_EXT_CLASS friend CSymDbMatrix cov(const CDoubleMatrix &v);
    AFX_EXT_CLASS friend CSymDbMatrix corrcoef(const CDoubleMatrix &v);

    CDoubleVector eig()const ;
	AFX_EXT_CLASS friend CDoubleVector eig(const CSymDbMatrix &m);

    CSymDbMatrix eig(CDoubleMatrix &v)const ;
    AFX_EXT_CLASS friend CSymDbMatrix eig(const CSymDbMatrix &m, CDoubleMatrix &v);

    CTriangleDbMatrix chol()const ;
	AFX_EXT_CLASS friend CTriangleDbMatrix chol(const CSymDbMatrix &m);

    CTriangleDbMatrix chol(double &det)const ;
	AFX_EXT_CLASS friend CTriangleDbMatrix chol(const CSymDbMatrix &m, double &det);
	AFX_EXT_CLASS friend CTriangleDbMatrix chol1(const CSymDbMatrix &m); //zfm
	AFX_EXT_CLASS friend CSymDbMatrix ismattodsmat(const CSymMatrix<int> &v);
	AFX_EXT_CLASS friend CSymFtMatrix ismattofsmat(const CSymMatrix<int> &v);
	AFX_EXT_CLASS friend CSymDbMatrix fsmattodsmat(const CSymMatrix<float> &v);

};


////////////////////////////////////////////////////////////////////////////////
//////////////////----------- CSymCompMatrix ------------/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*===temp
class AFX_EXT_CLASS CSymCompMatrix :public CSymMatrix<CComplex>  
{
public:
	CSymCompMatrix():CSymMatrix<CComplex>(){};
	CSymCompMatrix(int xsize, int ysize=0, CComplex init =0.0):CSymMatrix<CComplex>(xsize, ysize, init){};
	CSymCompMatrix(const CSymMatrix<CComplex> &x):CSymMatrix<CComplex>(x){};
	CSymCompMatrix(const CSymCompMatrix &x):CSymMatrix<CComplex>(x){};
	CSymCompMatrix(CComplex *a, int xlength, int ylength=0):CSymMatrix<CComplex>(a,xlength,ylength){};
	CSymCompMatrix(CComplex **a, int xlength, int ylength=0):CSymMatrix<CComplex>(a,xlength,ylength){};
	CSymCompMatrix(const CComplexMatrix &x);
	CSymCompMatrix(const CMatrix<CComplex> &x):CSymMatrix<CComplex>(x){};

public:


	CSymCompMatrix & operator =(const CSymCompMatrix &x);
	CSymCompMatrix & operator =(const CSymMatrix<CComplex> &x);

	CSymCompMatrix & operator =(const CComplexMatrix &x);
	CSymCompMatrix & operator =(const CVector<CComplex> &x);
	CSymCompMatrix & operator =(const CComplexVector &x);
	CSymCompMatrix & operator =(const CIntMatrix &x);
	CSymCompMatrix & operator =(const CFloatMatrix &x);
	CSymCompMatrix & operator =(CComplex **x);
	CSymCompMatrix & operator =(CComplex *x);
	CSymCompMatrix & operator =(CComplex x);

	AFX_EXT_CLASS friend CSymCompMatrix ismattocsmat(const CSymMatrix<int> &v);
	AFX_EXT_CLASS friend CSymCompMatrix fsmattocsmat(const CSymMatrix<float> &v);
	AFX_EXT_CLASS friend CSymCompMatrix dsmattocsmat(const CSymMatrix<double> &v);


};
//*/
template <class Type>
//稀疏矩阵
class CDiagMatrix
{
	//double *m_pValueAry;
	CVector<Type>*m_pVector;
	int m_nLen;
public:
	int GetLen()const{
		return m_nLen;
	}
	CVector<Type> *GetAddr()const
	{
		CVector<Type> * rst = (CVector<Type> *)m_pVector;
		return rst;
	}

	CDiagMatrix (const CVector<Type> value)
	{
		m_nLen = vlen(value);
		m_pVector = new CVector<Type>(value);
	}
	
	~CDiagMatrix ()
	{
		//delete []m_pValueAry ;
		delete m_pVector;
	}
};
template <class Type> 
CMatrix<Type> operator * (const CMatrix <Type> &dm,const CDiagMatrix <Type> &thin)
{
	int m1rows = dm.mrow();
	int m1cols = dm.mcol();
	int nLen = thin.GetLen();
	if(m1cols != nLen)
	{//出错
		return NULL;
	}
	
	CVector <Type> *pData = thin.GetAddr();
	CMatrix<Type> v(m1rows, nLen);
	for(int i=0; i<m1rows; i++)
	{
		for(int j=0;j< nLen; j++)
		{
			v(i,j)=dm.GetValue(i,j)*( (*pData)(j) );
		}
	}
	return v;
}

//--UNIX PORT 
#ifdef _UNIX
template <> 
CMatrix<double> operator * (const CMatrix <double> &dm,const CDiagMatrix <double> &thin);
#endif
/*
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
}*/
#endif // !defined(AFX_MATRIX_H__B3A91F5A_0C4C_4EEA_A287_3BE1014B64CA__INCLUDED_)































































