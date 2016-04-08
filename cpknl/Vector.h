// Vector.h: interface for the CVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_H__2A023DF1_DB4F_4AFE_8D0D_ECE4957B797A__INCLUDED_)
#define AFX_VECTOR_H__2A023DF1_DB4F_4AFE_8D0D_ECE4957B797A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define MIN(a,b) ((a)<(b))?(a):(b)
#define MAX(a,b) ((a)<(b))?(b):(a)
#define TOLERANT_ERROR 1.0e-12
//#define TOLERANT_ERROR 1.0e-40  //zfm
 
#define MAX_ITERATETIME 3000

////////////////////////////////////////////////////////////////////////////////////////////
///////////// ----------- CVector ------------/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
#include "allocate.h"
#include "../Public/common.h"


template <class Type>
class CMatrix;
//using namespace std;
template <class Type>
class AFX_EXT_CLASS CVector
{
protected:
	//friend class CAllocate;
	
	struct vrep
	{
		CAllocate<Type> *elem;
		int length;
		int refcnt;
		vrep()
		{
			elem = NULL;
			length = 0;
			refcnt = 1;
		}

	};
	vrep *p;

	public:
		//构造函数
_declspec(dllexport)	CVector()
		{
			p = NULL;
			initvect();
		}

_declspec(dllexport)	CVector(int size, Type init=0)
		{
			p = NULL;
			create(size,init);
		}

_declspec(dllexport)		CVector(Type *a, int length)
		{
			p=NULL;
			create(a,length);
		}

_declspec(dllexport)		CVector(const CVector &a1, const CVector &a2)
		{
			p = NULL;
			create(a1,a2);
		}
		
_declspec(dllexport)		CVector(const CVector &a, int first, int last)
		{
			p = NULL;
			create(a,first,last);
		}
		//拷贝构造函数
_declspec(dllexport)		CVector(const CVector &x)
		{
			x.p->refcnt++;
			p = x.p;
		}
		//析构函数
_declspec(dllexport)		~CVector()
		{
			destroy();
		}
_declspec(dllexport)		bool initvect()
		{
			destroy();
			p = new vrep;
			return true;
		}

_declspec(dllexport)		bool create(Type *a, int len)
		{
			initvect();
			if(len<0)
			{
				ErrorHandler(EMAT_INVALIDSIZE, __FILE__,__LINE__);
				len = 0;
			}
			if(len>=UINT_MAX/sizeof(Type))
			{
				ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
				len = 0;
			}
			if (p->elem==NULL)
			{
				p->elem = (CAllocate<Type> *)new CAllocate<Type>(len);
			}
			else
			{
				p->elem->Create(len);
			}
			p->length = len;
			for (int i=0;i<len;i++)
				(*this)(i)  = a[i];
			return true;
		}
_declspec(dllexport)		bool create(const CVector<Type> &a, int first, int last)
		{
			initvect();
			int size = last - first + 1;
			size = size > (a.vlen()-first) ? (a.vlen()-first):size; 
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
//			p->elem = (CAllocate<Type> *)new CAllocate<Type>(size);
			if (p->elem==NULL)
			{
				p->elem = (CAllocate<Type> *)new CAllocate<Type>(size);
			}
			else
			{
				p->elem->Create(size);
			}
			p->length = size;
			size = size > (a.vlen()-first) ? (a.vlen()-first):size; 
			for(int i=0; i<size; i++)
				(*(p->elem))(i) = a.GetValue(first + i);
			p->refcnt = 1;
			return true;
		}
_declspec(dllexport)		bool create(const CVector<Type> &a1,const CVector<Type> &a2)
		{
			initvect();
			int len1 = a1.vlen();
			int len2 = a2.vlen();
			int size = len1 + len2;
			if(size >= UINT_MAX /sizeof(Type))
			{
				ErrorHandler(EMAT_EXCEEDCAPACITY, __FILE__,__LINE__);
				size =0;
			}
			if (p->elem==NULL)
			{
				p->elem = (CAllocate<Type> *)new CAllocate<Type>(size);
			}
			else
			{
				p->elem->Create(size);
			}
			p->length = size;
			for(int i = 0; i<len1; i++)
				//(*(p->elem))(i) = 0;
				(*(p->elem))(i) = a1.GetValue(i);
			for(int j = 0; j<len2; j++)
				(*(p->elem))(j+len1) = a2.GetValue(j);
			return true;
		}
_declspec(dllexport)		bool create(int size, Type init=0)
		{
			initvect();
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
			if (p->elem==NULL)
			{
				p->elem = (CAllocate<Type> *)new CAllocate<Type>(size);
			}
			else
			{
				p->elem->Create(size);
			}
			p->length = size;
			for (int i=0;i<size;i++)
				(*this)(i) = init;
			return true;
		}
_declspec(dllexport)		bool create(const CVector<Type> &xxx)
		{
			(*this) = xxx;
			return true;
		}
_declspec(dllexport)		bool destroy()
		{
			if (p != NULL)
			{
				if(--p->refcnt == 0)
				{
					if(p->elem != NULL)
					{
						delete p->elem;
						p->elem = NULL;
					}
					delete p;
				}
				p = NULL;
			}
			return true;
		}
//{{{START 2003-1-7 Eric
_declspec(dllexport)		int		GetTotalSize()
		{
			if (p!=NULL)
				return p->length*sizeof(Type)+sizeof(int)*2;
			else
				return 0;
		};
//}}}END  2003-1-7 Eric
_declspec(dllexport)	int GetGroupCount()
		{
			return p->elem->GetCellCount();
		}

_declspec(dllexport)	int GetGroupSize()
		{
			return p->elem->GetCellSize();
		}


_declspec(dllexport)		static void SetErrorHandler(CErrorReporter &er);
		
		TEMPLAT_T friend CVector T_T copy(const CVector T_T &v);
		CVector copy()const 
		{
			int len = vlen();
			CVector <Type> x(len);
			CAllocate<Type> *f1 = x.p ->elem;
			CAllocate<Type> *f2 = p->elem;
			for(int i=0; i<len; i++)
				(*f1)(i) = (*f2)(i);
			return x;
		}
		
_declspec(dllexport)		void insert(int nIndex, const Type &newElement, int nCount = 1)
		{
			int nSize= vlen();
			CVector vec(nSize+nCount);
			int i=0;
			for(i=0; i<nIndex; i++)
				vec(i) = (*this)(i);
			for(i=nIndex; i<nIndex+nCount; i++)
				vec(i) = newElement;
			for(i=nIndex+nCount; i<nSize+nCount; i++)
				vec(i) = (*this)(i-nCount);
			(*this) = vec;
		}
_declspec(dllexport)		void remove(int nIndex, int nCount = 1)
		{
			int nSize= vlen();
			if(nSize>nCount)
			{
				CVector vec(nSize-nCount);
				int i=0;
				for(i=0; i<nIndex; i++)
					vec(i) = (*this)(i);
				for(i=nIndex; i<nSize-nCount; i++)
					vec(i) = (*this)(i+nCount);
				(*this)=vec;
			}
			else
			{
				CVector vec;
				(*this)=vec;
			}
		}
  _declspec(dllexport)      int append(const Type &newElement)
		{
			int nSize= vlen();
			CVector vec(nSize+1);
			for(int i=0; i<nSize; i++)
				vec(i) = (*this)(i);
			vec(nSize) = newElement;
			(*this) = vec;
			return nSize+1;
		}
_declspec(dllexport)		void change(const CVector<Type> &v, int first, int last)
		{
			int siz = last - first + 1;
			if(v.vlen()==siz)
			{
				for(int i=0; i<siz; i++)
				{
					(*this)(i+first) = v.GetValue(i);
				}
					
			}
			else
			{
				CVector<Type> v1(vlen()+v.vlen()-siz);
				int i=0;
				for(i=0; i<first; i++)  
					v1(i) = (*this)(i);
				for(i=first; i<first+v.vlen(); i++)
					v1(i) = v.GetValue(i-first);
				for(i=last+v.vlen()-siz+1; i<vlen()+v.vlen()-siz; i++)
					v1(i) = (*this)(i-v.vlen()+siz);
				(*this) = v1;
			}
		}
		
_declspec(dllexport)	TEMPLAT_T	friend int vlen(const CVector T_T &v);//{return v.p->length;}
_declspec(dllexport)	TEMPLAT_T	friend int vlen(CVector T_T*v);//{return v->p->length;}
_declspec(dllexport)		int vlen() const
		{
			if (p!=NULL)
				return p->length;
			else 
				return 0;
		}
_declspec(dllexport)	TEMPLAT_T	friend bool resize(CVector T_T &v,int nsize);//{return v->p->length;}
_declspec(dllexport)		bool resize(int nsize)
		{
			if (p!=NULL)
			{
				if (nsize <= p->length)
				{
					p->length = nsize;
					p->elem->Resize(nsize);
					return true;
				}
				else
					return false;
			}
			else 
				return false;
		}
_declspec(dllexport)		int find(Type value)const 
		{
			for (int i=0;i<p->length;i++)
				if ((*this)(i) == value)
					return i;
				return -1;
		}
_declspec(dllexport)		CVector & operator = (const CVector &vec)
		{
			destroy();
			p = vec.p;
			p->refcnt++;  //===cur 320
			
			return * this;
		}
_declspec(dllexport)		CVector & operator = (Type x)
		{
			if (p != NULL)
			{
				int len = p->length;
				if(p->refcnt > 1)
				{
					p->refcnt--;
					p = new vrep;
					p->refcnt = 1;
					p->length = len;
					p->elem = (CAllocate<Type> *)new CAllocate<Type>(len,x);
				}
				else if (p->refcnt == 1)
				{
					CAllocate<Type> *elem = p->elem;
					for(int i=0; i<len; i++)
						(*elem)(i) = x;
				}
			}
			return *this;
		}
		
_declspec(dllexport)		operator CAllocate<Type> *()
		{
			return p->elem;
		}
		
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator +(const CVector T_T &v1, const CVector T_T &v2);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator -(const CVector T_T &v1, const CVector T_T &v2);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator *(const CVector T_T &v1, const CVector T_T &v2);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator /(const CVector T_T &v1, const CVector T_T &v2);
		
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator +(const CVector T_T &v1, T_S x);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator -(const CVector T_T &v1, T_S x);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator *(const CVector T_T &v1, T_S x);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator /(const CVector T_T &v1, T_S x);
		
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator +(T_S x, const CVector T_T &v1);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator -(T_S x, const CVector T_T &v1);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator *(T_S x, const CVector T_T &v1);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator /(T_S x, const CVector T_T &v1);
		
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator +(const CVector T_T &v1);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T operator -(const CVector T_T &v1);
		
_declspec(dllexport)		CVector & operator += (CVector &v1)
		{
			
			int len = MIN(p->length, v1.p->length);
			
			CAllocate<Type> *f = p->elem;
			CAllocate<Type> *f1 = v1.p->elem;
			for(int i=0; i<len; i++)
				(*f)(i) += (*f1)(i);
			
			return v1;
		}

_declspec(dllexport)		CVector & operator -= (const CVector &v1)
		{
			int len = MIN(p->length, v1.p->length);
			
			CAllocate<Type> *f = p->elem;
			CAllocate<Type> *f1 = v1.p->elem;
			for(int i=0; i<len; i++)
				(*f)(i) -= (*f1)(i);
			return * this;
		}
_declspec(dllexport)		CVector & operator *= (const CVector &v1)
		{
			int len = MIN(p->length, v1.p->length);
			
			CAllocate<Type> *f = p->elem;
			CAllocate<Type> *f1 = v1.p->elem;
			for(int i=0; i<len; i++)
				(*f)(i) *= (*f1)(i);
			return * this;
		}
_declspec(dllexport)		CVector & operator /= (const CVector &v1)
		{
			int len = MIN(p->length, v1.p->length);
			
			CAllocate<Type> *f = p->elem;
			CAllocate<Type> *f1 = v1.p->elem;
			for(int i=0; i<len; i++)
				(*f)(i) /= (*f1)(i);
			return * this;
		}
		
_declspec(dllexport)		CVector & operator += (Type x)
		{
			int len = p->length;
			
			CAllocate<Type> *f = p->elem;
			
			for(int i=0; i<len; i++)
				(*f)(i) += x;
			return * this;
		}
_declspec(dllexport)		CVector & operator -= (Type x)
		{
			int len = p->length;
			
			CAllocate<Type> *f = p->elem;
			
			for(int i=0; i<len; i++)
				(*f)(i) -= x;
			return * this;
		}
_declspec(dllexport)		CVector & operator *= (Type x)
		{
			int len = p->length;
			
			CAllocate<Type> *f = p->elem;
			
			for(int i=0; i<len; i++)
				(*f)(i) *= x;
			return * this;
		}
_declspec(dllexport)		CVector & operator /= (Type x)
		{
			int len = p->length;
			
			CAllocate<Type> *f = p->elem;
			
			for(int i=0; i<len; i++)
				(*f)(i) /= x;
			return * this;
		}
		
_declspec(dllexport)	TEMPLAT_T	friend T_S sum(const CVector T_T &v);
_declspec(dllexport)		Type sum()const 
		{
			int n = vlen();
			Type re = 0;
			CAllocate<Type> *f = p->elem;
			for(int i = 0; i<n; i++)
				re += (*f)(i);
			return re;
		}
		//friend Type mean(CVector &v);
_declspec(dllexport)	TEMPLAT_T	friend double mean(const CVector T_T &v);
		//Type mean();
_declspec(dllexport)		double mean()const 
		{
			int n = vlen();
			assert (n>0); //zfm
			double re = 0;
			CAllocate<Type> *f = p->elem;
			for(int i = 0; i<n; i++)
				re += (*f)(i);
			re /= n;
			return re;
		}

_declspec(dllexport)		Type dot(const CVector &v2)const 
		{
			int len = MIN(p->length, v2.p->length);
			
			Type sum = 0;
			CAllocate<Type> *f1 = p->elem;
			CAllocate<Type> *f2 = v2.p->elem;
			for(int i = 0; i<len; i++)
				sum += ((*f1)(i))* ((*f2)(i));
			return sum;
		}
		
_declspec(dllexport)	TEMPLAT_T	friend double mean_w(const CVector T_T &v, const CVector T_T &w);//加权重的均值 
_declspec(dllexport)		double mean_w(const CVector &w)const 
		{
			int n = vlen();
			assert((w.vlen()==n) && (n> 0));
			
			double re = 0;
			CAllocate<Type> *f = p->elem;
			CAllocate<Type> *e =w.p->elem;
			for(int i = 0; i<n; i++)
				re += ((*f)(i))*((*e)(i));
			re /= w.sum();
			return re;
			
		}

		void meanvar_w(const CVector &w, Type& dMean, Type& dVar)
		{
			int len=vlen();
			int w_len=w.vlen();
			double s=0.0;
			double sw=0.0;
			double s1=0.0;
			double sm=0.0; 
			assert((len==w_len)&&(w_len>1));
			CAllocate<Type> *pV =p->elem;
			CAllocate<Type> *wP =w.p->elem;
			for(int i=0; i<len; i++)
			{
				s+=((*pV)(i)) * ((*pV)(i)) * ((*wP)(i));
				sw +=((*wP)(i));
				sm+=((*pV)(i)) * ((*wP)(i));
			}
			
			dMean = sm / sw;
			dVar = (s-(sm*sm*sw)/(sw*sw)) / ( sw-1);
		}

_declspec(dllexport)	TEMPLAT_T	friend double var_w(const CVector T_T &v, const CVector T_T &w); //加权重的方差
_declspec(dllexport)		double var_w(const CVector &w)const 
		{
			
			int len=vlen();
			int w_len=w.vlen();
			double s=0.0;
			double sw=0.0;
			double s1=0.0;
			double sm=0.0; 
			assert((len==w_len)&&(w_len>1));
			CAllocate<Type> *pV =p->elem;
			CAllocate<Type> *wP =w.p->elem;
			
			for(int i=0; i<len; i++)
			{
				s+=((*pV)(i)) * ((*pV)(i)) * ((*wP)(i));
				sw +=((*wP)(i));
				s1+=((*pV)(i));
				sm+=((*pV)(i)) * ((*wP)(i));
			}
			
			return (s-(sm*sm*sw)/(sw*sw)) / ( sw-1);       
			
			
			
		}



_declspec(dllexport)		void meanvar_w(CVector &w, double& dMean, double& dVar)
		{
			int len=vlen();
			int w_len=w.vlen();
			double s=0.0;
			double sw=0.0;
			double s1=0.0;
			double sm=0.0; 
			assert((len==w_len)&&(w_len>1));
			CAllocate<Type> *pV =p->elem;
			CAllocate<Type> *wP =w.p->elem;
			for(int i=0; i<len; i++)
			{
				s+=((*pV)(i)) * ((*pV)(i)) * ((*wP)(i));
				sw +=((*wP)(i));
				sm+=((*pV)(i)) * ((*wP)(i));
			}
			
			dMean = sm / sw;
			dVar = (s-sm*sm/sw) / ( sw-1);
		}

_declspec(dllexport)		void sumvar(double& dSum, double& dVar)
		{
			int len=vlen();
			double s=0.0;
			double s1=0.0;
			double sm=0.0; 
			assert(len>1);
			CAllocate<Type> *pV =p->elem;
			for(int i=0; i<len; i++)
			{
				s+=((*pV)(i)) * ((*pV)(i));
				sm+=((*pV)(i));
			}
			
			dVar = (s-sm*sm/len)/(len-1);
			dSum = sm;
		}
		
		
_declspec(dllexport)		void sumvarmaxmin(double& dSum, double& dVar, double &dMax, double& dMin)
		{
			int len=vlen();
			double s=0.0;
			double s1=0.0;
			double sm=0.0; 
			assert(len>1);
			CAllocate<Type> *pV =p->elem;
			dMin = (*pV)(0);
			dMax = (*pV)(0);
			for(int i=0; i<len; i++)
			{
				double dVal = ((*pV)(i));
				s+=dVal * dVal;
				sm+= dVal;
				if (dVal > dMax)
					dMax = dVal;
				if (dVal < dMin)
					dMin = dVal;
			}
			
			dVar = (s-sm*sm/len)/(len-1);
			dSum = sm;
		}
		
		
_declspec(dllexport)		void sumvar_w(CVector &w, double& dSum, double& dVar,double &dWghsum)
		{
			int len=vlen();
			int w_len=w.vlen();
			double s=0.0;
			double sw=0.0;
			double s1=0.0;
			double sm=0.0; 
			assert((len==w_len)&&(w_len>1));
			CAllocate<Type> *pV =p->elem;
			CAllocate<Type> *wP =w.p->elem;
			for(int i=0; i<len; i++)
			{
				s+=((*pV)(i)) * ((*pV)(i)) * ((*wP)(i));
				sw +=((*wP)(i));
				sm+=((*pV)(i)) * ((*wP)(i));
			}
			
			dVar = (s-sm*sm/sw)/(sw-1);
			dSum = sm;
			dWghsum = sw;
		}
		
		
_declspec(dllexport)		void sumvarmaxmin_w(CVector &w, double& dSum, double& dVar, double &dMax, double& dMin, double& dWghsum)
		{
			int len=vlen();
			int w_len=w.vlen();
			double s=0.0;
			double sw=0.0;
			double s1=0.0;
			double sm=0.0; 
			assert((len==w_len)&&(w_len>1));
			CAllocate<Type> *pV =p->elem;
			CAllocate<Type> *wP =w.p->elem;
			dMin = (*pV)(0);
			dMax = (*pV)(0);
			for(int i=0; i<len; i++)
			{
				double dVal = ((*pV)(i));
				s+=dVal * dVal * ((*wP)(i));
				sw +=((*wP)(i));
				sm+= dVal * ((*wP)(i));
				if (dVal > dMax)
					dMax = dVal;
				if (dVal < dMin)
					dMin = dVal;
			}
			
			dVar = (s-sm*sm/sw)/(sw-1);
			dSum = sm;
			dWghsum = sw;
		}
		

		
_declspec(dllexport)		CVector &templMergeSort(CVector &vec,int nTotalValidCount,int nswitch, bool bUp);
			
		
	TEMPLAT_T	friend int operator == (const CVector T_T &v1, const CVector T_T &v2); 
	TEMPLAT_T	friend int operator != (const CVector T_T &v1, const CVector T_T &v2); 
	TEMPLAT_T	friend int operator == (const CVector T_T &v1, T_S x); 
	TEMPLAT_T	friend int operator != (const CVector T_T &v1, T_S x); 

_declspec(dllexport)	TEMPLAT_T	friend void operator << (T_S * val, const CVector T_T & vec);
_declspec(dllexport)	TEMPLAT_T	friend void operator << (CVector T_T& vec1, const CVector T_T & vec2);

_declspec(dllexport)		CVector & operator = (Type * x)
		{
			int len = p->length;
			if (len != 0)
			{
				if(p->refcnt > 1)
				{
					p->refcnt--;
					p =new vrep;
					p->length = len;
					p->elem = (CAllocate<Type> *)new CAllocate<Type>(len);
				}
				for(int i=0; i<len; i++)
					(*(p->elem))(i) = *x;
			}
			return *this;		
		}

		const Type&  GetValue(int i)const
		{
			assert((i>=0)&&(i<p->length));
			return (*(p->elem))(i);	
		}
_declspec(dllexport)		Type & operator ()(int i)
		{
			assert((i>=0)&&(i<p->length));
			return (*(p->elem))(i);	
			
		}

		Type & operator [](int i)
		{
			assert((i>=0)&&(i<p->length));
			return (*(p->elem))(i);	
		}
		
		CVector slice(int xlen)const 
		{
			int lens = p->length;
			if((xlen>lens)||(xlen<0))
			{
				ErrorHandler(EMAT_OUTOFRANGE,__FILE__,__LINE__);
				xlen = 0;
			}
			CVector<Type> x(xlen);
			for(int i=0; i<xlen; i++)
				x(i) = (*this)(i);
			return x;
			
		}

		TEMPLAT_T friend CVector T_T slice(const CVector T_T &v, int len);

		CVector slice(int lstart, int xlen)const 
		{
			int lens = p->length;
			if(xlen<0)
			{
				ErrorHandler(EMAT_ASSIGNDATAERR, __FILE__, __LINE__);
				xlen = 0;
			}
			if((lstart+xlen)>lens||lstart<0)
			{
				ErrorHandler(EMAT_OUTOFRANGE, __FILE__, __LINE__);
				xlen = 0;
			}
			CVector<Type> x(xlen);
			for(int i=lstart; i<lstart+xlen; i++)
				x(i-lstart) = (*this).GetValue(i);
			return x;
			
		}

_declspec(dllexport)	TEMPLAT_T	friend CVector T_T slice(const CVector T_T &v, int lstart, int length);
_declspec(dllexport)	TEMPLAT_T	friend CVector T_T comvec(const CVector T_T &v1, const CVector T_T &v2);
_declspec(dllexport)	CVector comvec(const CVector &v2)const 
		{
			int i;
			int lens1 = vlen();
			int lens2 = v2.vlen();
			int lens = lens1 + lens2;
			CVector<Type> x(lens);
			for(i=0; i<lens1;i++)
				x(i) = (*this).GetValue(i);
			for(i=0; i<lens2; i++)
				x(i+lens1)=v2.GetValue(i);

			return x;
		}
		
_declspec(dllexport)		CVector & cos()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::cos((*this)(i));
			return *this;
		}

_declspec(dllexport)	TEMPLAT_T	friend CVector T_T cos(const CVector T_T &x);
_declspec(dllexport)		CVector & cosh()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::cosh((*this)(i));
			return *this;
		}

_declspec(dllexport)	TEMPLAT_T	friend CVector T_T cosh(const CVector T_T &x);
_declspec(dllexport)		CVector & acos()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::acos((*this)(i));
			return *this;
		}

		TEMPLAT_T friend CVector T_T acos(const CVector T_T &x);
		CVector & sin()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::sin((*this)(i));
			return *this;
		}

		TEMPLAT_T friend CVector T_T sin(const CVector T_T &x);
		CVector & sinh()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::sinh((*this)(i));
			return *this;
		}

		TEMPLAT_T friend CVector T_T sinh(const CVector T_T &x);
		CVector & asin()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::asin((*this)(i));
			return *this;
		}

		TEMPLAT_T friend CVector T_T asin(const CVector T_T &x);
		CVector & tan()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::tan((*this)(i));
			return *this;
		}

		TEMPLAT_T friend CVector T_T tan(const CVector T_T &x);
		CVector & tanh()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::tanh((*this)(i));
			return *this;
		}
		
		TEMPLAT_T friend CVector T_T tanh(const CVector T_T &x);
		CVector & atan()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::atan((*this)(i));
			return *this;
		}
		
		TEMPLAT_T friend CVector T_T atan(const CVector T_T &x);
		CVector & log()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::log((*this)(i));
			return *this;
		}
		
		TEMPLAT_T friend CVector T_T log(const CVector T_T &x);
		CVector & log10()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::log10((*this)(i));
			return *this;
		}
		
		TEMPLAT_T friend CVector T_T log10(const CVector T_T &x);
		CVector & exp()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::exp((*this)(i));
			return *this;
		}
		
		TEMPLAT_T friend CVector T_T exp(const CVector T_T &x);
		CVector & sqrt()
		{
			int lens = p->length;
			for(int i=0; i<lens; i++)
				(*this)(i)= ::sqrt((*this)(i));
			return *this;
		}

		TEMPLAT_T friend CVector T_T sqrt(const CVector T_T &x);
		
		TEMPLAT_T friend CMatrix<T_S> combs(const CVector<T_S> &v, int k);
		TEMPLAT_T friend CMatrix<T_S> nchoosek(const CVector<T_S> &v, int k);
		TEMPLAT_T friend CMatrix<T_S> Multi(const CVector<T_S> &v1,const CVector<T_S> &v2);//nx1 与 1x n 得到 n×n的 矩阵 。
		void destruction() //zfm
		{
			//	destroy();
			CVector<Type> v;
			*this=v;
		}
		
		
};

typedef CVector<float> CFloatVector;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class Type>
int vlen(const CVector<Type> &v)
{
	if (v.p!=NULL)
		return v.p->length;
	else
		return 0;
}

template <class Type>
int vlen(CVector<Type> *v)
{
	if ((v!=NULL)&&(v->p!=NULL))
		return v->p->length;
	else 
		return 0;
}

template <class Type>
bool resize(CVector<Type>& v,int nsize)
{
	return v.resize(nsize);
}

template <class Type>
CVector <Type> copy(const CVector<Type> &m)
{
	CVector<Type> x = m.copy();
	return x;
}

template <class Type>
CVector <Type> operator + (const CVector <Type> &v1, const CVector<Type> &v2)
{
	int len = MIN(v1.vlen(), v2.vlen());
	CVector<Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = (v1).GetValue(i) + (v2).GetValue(i);
	return v;
}


template <class Type>
CVector <Type> operator - (const CVector <Type> &v1, const CVector<Type> &v2)
{
	int len = MIN(v1.vlen(), v2.vlen());
	CVector<Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = (v1).GetValue(i) - (v2).GetValue(i);
	return v;
}



template <class Type>
CVector <Type> operator * (const CVector <Type> &v1, const CVector<Type> &v2)
{
	int len = MIN(v1.vlen(), v2.vlen());
	CVector<Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = (v1).GetValue(i) * (v2).GetValue(i);
	return v;
}


template <class Type>
CVector <Type> operator / (const CVector <Type> &v1, const CVector<Type> &v2)
{
	int len = MIN(v1.vlen(), v2.vlen());
	CVector<Type> v(len);
	
	for(int i=0; i<len; i++)
	{
		(v)(i) = (v1).GetValue(i) / (v2).GetValue(i);
	}
	return v;
}

template <class Type>
CVector<Type> operator + (const CVector<Type> &v1, Type x)
{
	int len = v1.vlen();
	CVector <Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = (v1).GetValue(i) + x;
	return v;
}

template <class Type>
CVector<Type> operator - (const CVector<Type> &v1, Type x)
{
	int len = v1.vlen();
	CVector <Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = (v1).GetValue(i) - x;
	return v;
}

template <class Type>
CVector<Type> operator * (const CVector<Type> &v1, Type x)
{
	
	int len = v1.vlen();
	CVector <Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = (v1).GetValue(i) * x;
	return v;
}

template <class Type>
CVector<Type> operator / (const CVector<Type> &v1, Type x)
{
	assert(fabs(x)>1e-100); //zfm
	int len = v1.vlen();
	CVector <Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = (v1).GetValue(i) / x;
	return v;
}

template <class Type>
CVector<Type> operator + (Type x, const CVector<Type> &v1)
{
	int len = v1.vlen();
	CVector <Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = x + (v1).GetValue(i);
	return v;
}

template <class Type>
CVector<Type> operator - (Type x, const CVector<Type> &v1)
{
	int len = v1.vlen();
	CVector <Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = x - (v1).GetValue(i);
	return v;
}

template <class Type>
CVector<Type> operator * (Type x, const CVector<Type> &v1)
{
	int len = v1.p->length;
	//int len = v1.vlen();
	CVector <Type> v(len);
	
	CAllocate<Type> *f = v.p->elem;
	CAllocate<Type> *f1 = v1.p->elem;
	for(int i=0; i<len; i++)
		(*f)(i) = x * (*f1)(i);
	return v;
}

template <class Type>
CVector<Type> operator / (Type x, const CVector<Type> &v1)
{
	int len = v1.p->length;
	CVector <Type> v(len);
	
	CAllocate<Type> *f = v.p->elem;
	CAllocate<Type> *f1 = v1.p->elem;
	for(int i=0; i<len; i++)
		(*f)(i) = x / (*f1)(i);
	return v;
}

template <class Type>
CVector<Type> operator + (const CVector<Type> &v1)
{
	return v1;
}

template <class Type>
CVector<Type> operator - (const CVector<Type> &v1)
{
	int len = v1.vlen();
	CVector <Type> v(len);
	
	for(int i=0; i<len; i++)
		(v)(i) = - (v1).GetValue(i);
	return v;
}


template <class Type>
int operator == (const CVector <Type> &v1, const CVector <Type> &v2)
{
	if(v1.p->length != v2.p->length)
		return 0;
	int len = v1.p->length;
	
	CAllocate<Type> *f1 = v1.p->elem;
	CAllocate<Type> *f2 = v2.p->elem;
	
	for(int i=0; i<len; i++)
		if((*f1)(i) != (*f2)(i))
			return 0;
		return 1;
}

template <class Type>
int operator != (const CVector <Type> &v1, const CVector <Type> &v2)
{
	if (v1.vlen()!=v2.vlen())
		return 1;
	int len = v1.vlen();
	for(int i=0; i<len; i++)
		if(v1.GetValue(i) != v2.GetValue(i))
			return 1;
	return 0;
	////////////////////////////////
}

template <class Type>
int operator == (const CVector <Type> &v1, Type x)
{
	int len = v1.p->length;
	CAllocate<Type> *f1 = v1.p->elem;
	
	for(int i=0; i<len; i++)
		if((*f1)(i) != x)
			return 0;
		return 1;
}

template <class Type>
int operator != (const CVector <Type> &v1, Type x)
{
	int len = v1.vlen();
	
	for(int i=0; i<len; i++)
		//if((*v1)(i) != x)
		if(v1.GetValue(i) != x)
			return 1;
		return 0;
}
template <class Type>
Type sum(const CVector<Type> &v)
{
	return v.sum();
}
/*
template <class Type>
Type CVector<Type>::mean()
{
	int n = vlen();
	assert (n>0); //zfm
	Type re = 0;
	Type *f = p->elem;
	for(int i = 0; i<n; i++, f++)
		re += *f;
	re /= n;
	return re;
}

template <class Type>
Type mean(CVector<Type> &v)
{
	int n = v.vlen();
	assert(n>0); //zfm
	Type re = 0;
	Type *f = v.p->elem;
	for(int i = 0; i<n; i++, f++)
		re += *f;
	re /= n;
	return re;
}
*/


template <class Type>
double mean(const CVector<Type> &v)
{
	return v.mean();
}


template <class Type>
double mean_w(const CVector<Type> &v, const CVector<Type> &w)
{
	return w.mean_w(w);
}

template <class Type>
double var_w(const CVector<Type> &v, const CVector<Type> &w)
{
	return v.var_w(w);
}

template <class Type>
CMatrix<Type> Multi(const CVector<Type> &v1,const CVector<Type> &v2)//mx1 与 1x n 得到 m×n的 矩阵 。
{
	int i =v1.vlen();
	int j =v2.vlen();
		
	CMatrix<Type> RetMat(i,j);
	for(i = 0;i<v1.vlen();i++)
		for(j=0;j<v2.vlen();j++)
			RetMat(i,j)=v1.GetValue(i)*v2.GetValue(j);
	return RetMat;
}

template <class Type>
void operator <<(Type * x, const CVector<Type> &v)
{
	int len = v.p->length;
	if(len !=0)
	{
		for(int i=0; i<len; i++)
			*x = (*(v.p->elem))(i);
    }
}

template <class Type>
void operator <<(CVector<Type>& x, const CVector<Type> &v)
{
	int len = v.p->length;
	if(len !=0)
	{
		(*(x.p->elem))(0) = (*(v.p->elem))(len-1);
    }
}


template <class Type>
CVector<Type> slice(const CVector<Type> &v, int xlen)
{
	return v.slice(xlen);
}

template <class Type>
CVector<Type> slice(CVector<Type> &v, int lstart, int xlen)
{
	return v.slice(lstart,xlen);
}


template <class Type>
CVector<Type> comvec(const CVector<Type> &v1, const CVector<Type> &v2)
{
	return v1.comvec(v2);
}

template <class Type>
CVector<Type> cos(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= cos(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> cosh(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= cosh(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> acos(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= acos(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> sin(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= sin(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> sinh(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= sinh(x(i));
	return v;
}

template <class Type>
CVector<Type> asin(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= asin(x.GetValue(i));
	return v;
}


template <class Type>
CVector<Type> tan(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= tan(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> tanh(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= tanh(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> atan(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= atan(x.GetValue(i));
	return v;
}


template <class Type>
CVector<Type> log(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= log(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> log10(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= log10(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> exp(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= exp(x.GetValue(i));
	return v;
}

template <class Type>
CVector<Type> sqrt(const CVector<Type> &x)
{
	int lens = x.vlen();
	CVector<Type> v(lens);
	for(int i=0; i<lens; i++)
		v(i)= sqrt(x.GetValue(i));
	return v;
}

template <class Type>
CMatrix<Type> combs(const CVector<Type> &v, int k)
{
    int lens = v.vlen();
	if(lens == k)
	{
		CMatrix<Type> m = v;
		return m.trans();
	}
	else if(k==1)
	{
		CMatrix<Type> m = v;
		return m;
	}
	else
	{
		CMatrix<Type> m;
		if(k<lens && k>1)
		{
			for(int i=0; i<lens-k+1; i++)
			{
				CVector<Type> v1(v, i+1, lens-1);
				CMatrix<Type> m1 = combs(v1, k-1);
				int nc = m1.mrow();
				CMatrix<Type> m2(nc,1,v.GetValue(i));
				m = m & (m2|m1);
			}
		}
		return m;
	}
}

template <class Type>
CMatrix<Type> nchoosek(const CVector<Type> &v, int k)
{
	return combs(v,k);
}

class CDoubleVector;
////////////////////////////////////////////////////////////////////////////////////////////
///////////// ----------- CIntVector ------------/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIntVector : public CVector<int> 
{
public:
	CIntVector():CVector<int>(){};
	CIntVector(int size, int init =0.0):CVector<int>(size, init){};
	CIntVector(int *a, int length):CVector<int>(a,length){};
	CIntVector(const CIntVector &a1, const CIntVector &a2):CVector<int>(a1,a2){};
	CIntVector(const CIntVector &a, int first, int last):CVector<int>(a,first,last){};
	CIntVector(const CIntVector &x):CVector<int>(x){};
	CIntVector(const CVector<int> & x):CVector<int>(x){};
    CIntVector & operator =(const CIntVector &x);
    CIntVector & operator =(const CVector<int> &x);
    CIntVector & operator =(int * x);
    CIntVector & operator =(int x);
	AFX_EXT_CLASS friend double var_w(const CIntVector &v, const CDoubleVector &w); //zfm 加权的方差
	double var_w(const CDoubleVector &w)const ; //zfm 加权的方差
	double var()const ; //zfm
	CIntVector & Sort(bool select=0);
	AFX_EXT_CLASS friend CIntVector Sort(CIntVector &v, bool select=0);
	CIntVector & Sort(CIntVector &idx, bool select=0);
	AFX_EXT_CLASS friend CIntVector Sort(CIntVector &v, CIntVector &idx, bool select=0);

    AFX_EXT_CLASS friend void MaxMin(const CIntVector &v,const CIntVector& Idx,int& dMax,int& dMin);
	void MaxMin(const CIntVector &Idx,int& dMax,int& dMin)const ;
	
    AFX_EXT_CLASS friend void MaxMin(const CIntVector &v,int& dMax,int& dMin);
	void MaxMin(int& dMax,int& dMin)const ;
    AFX_EXT_CLASS friend void MaxMin(const CIntVector &v,int& dMax,int& dMin,int& nIdxMax,int& nIdxMin);
	void MaxMin(int& dMax,int& dMin,int& nIdxMax,int& nIdxMin)const ;
	
	int Max()const ;
	CIntVector Max(const CIntVector &v2)const ;
    CIntVector Min(const CIntVector &v2)const ;
	int Min()const ;
	AFX_EXT_CLASS friend int Max(const CIntVector &v) ;
	AFX_EXT_CLASS friend CIntVector Max(const CIntVector &v1, const CIntVector &v2);
	AFX_EXT_CLASS friend int Min(CIntVector const &v) ;
	AFX_EXT_CLASS friend CIntVector Min(const CIntVector &v1, const CIntVector &v2);
	//friend CDoubleVector operator -(CIntVector &v1, double x);  //CIntVector求方差专用
	//friend CDoubleVector operator *(CDoubleVector &v, CIntVector &w);	
	CIntVector &quicksort(int low, int high, bool bUp=true); //整型向量快速排序,原向量不保留 zfm
	AFX_EXT_CLASS friend CIntVector & MergeSort(CIntVector &v, bool bUp = true);
	AFX_EXT_CLASS friend CIntVector &quicksort(CIntVector &v,int low,int high, bool bUp=true); //整型向量快速排序，原向量也保留 zfm
};

class CDoubleMatrix;
class CIntVector;
////////////////////////////////////////////////////////////////////////////////////////////
///////////// ----------- CDoubleVector ------------/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CDoubleVector : public CVector<double> 
{
public:
	CDoubleVector():CVector<double>(){};
	CDoubleVector(int size, double init =0.0):CVector<double>(size, init){};
	CDoubleVector(double *a, int length):CVector<double>(a,length){};
	CDoubleVector(const CDoubleVector &a1, const CDoubleVector &a2):CVector<double>(a1,a2){};
	CDoubleVector(const CDoubleVector &a, int first, int last):CVector<double>(a,first,last){};
	CDoubleVector(const CDoubleVector &x):CVector<double>(x){};
	CDoubleVector(const CVector<double> & x):CVector<double>(x){};
    CDoubleVector & operator =(const CDoubleVector &x);
    CDoubleVector & operator =(const CVector<double> &x);
    CDoubleVector & operator =(const CIntVector &x);
    CDoubleVector & operator =(const CFloatVector &x);
    CDoubleVector & operator =(double * x);
    CDoubleVector & operator =(double x);
 	AFX_EXT_CLASS friend CDoubleVector ivectodvec(const CIntVector &x);
	AFX_EXT_CLASS friend CFloatVector ivectofvec(const CIntVector &x);
	AFX_EXT_CLASS friend CDoubleVector fvectodvec(const CFloatVector &x);
    AFX_EXT_CLASS friend int any(const CDoubleVector &v);
	int any()const ;

    AFX_EXT_CLASS friend void MaxMin(const CDoubleVector &v,const CIntVector& Idx,double& dMax,double& dMin);
	void MaxMin(const CIntVector &Idx,double& dMax,double& dMin)const ;

    AFX_EXT_CLASS friend void MaxMin(const CDoubleVector &v,double& dMax,double& dMin);
	void MaxMin(double& dMax,double& dMin)const ;
    AFX_EXT_CLASS friend void MaxMin(const CDoubleVector &v,double& dMax,double& dMin,int& nIdxMax,int& nIdxMin);
	void MaxMin(double& dMax,double& dMin,int& nIdxMax,int& nIdxMin)const ;

    AFX_EXT_CLASS friend double Max(const CDoubleVector &v);
	double Max()const ;
    AFX_EXT_CLASS friend double Max(const CDoubleVector &v,int &nIndex);
	double Max(int &nIndex)const ;
    AFX_EXT_CLASS friend CDoubleVector Max(const CDoubleVector &v1, const CDoubleVector &v2);
	CDoubleVector Max(const CDoubleVector &v2)const;
    AFX_EXT_CLASS friend double Min(const CDoubleVector &v);
	double Min()const ;
    AFX_EXT_CLASS friend double Min(const CDoubleVector &v,int &nIndex);
	double Min(int &nIndex)const ;
    AFX_EXT_CLASS friend CDoubleVector Min(const CDoubleVector &v1, const CDoubleVector &v2) ;
	CDoubleVector Min(const CDoubleVector &v2)const ;
	double Mean()const ;
	friend double Mean(const CDoubleVector &v, const CIntVector w);
    AFX_EXT_CLASS friend CDoubleVector abs(const CDoubleVector &v);
	CDoubleVector abs()const ;
	double dot(const CDoubleVector &v2)const ;
	CDoubleVector dot(const CDoubleMatrix &v2)const ;
	AFX_EXT_CLASS friend double dot(const CDoubleVector &v1, const CDoubleVector &v2);
		AFX_EXT_CLASS friend double norm(const CDoubleVector &v, int n=2);
	double norm(int n=2)const ;
	CDoubleVector & pow(double pp);
	AFX_EXT_CLASS friend CDoubleVector pow(const CDoubleVector &v, double pp);
	CDoubleVector & pow(const CDoubleVector &pp);
	AFX_EXT_CLASS friend CDoubleVector pow(const CDoubleVector &x, const CDoubleVector &pp);
	CDoubleVector & Rand();
	AFX_EXT_CLASS friend CDoubleVector & Rand(CDoubleVector &v);
    CDoubleVector Rand(int m)const ;
	CDoubleVector & Sort(bool select=0);
	AFX_EXT_CLASS friend CDoubleVector Sort(CDoubleVector &v, bool select=0);
	CDoubleVector & Sort(CIntVector &idx, bool select=0);
	AFX_EXT_CLASS friend CDoubleVector Sort(CDoubleVector &v, const CIntVector &idx, bool select=0);
	CDoubleVector & quicksort(CIntVector &idx, int low, int high, bool bAssc=true);
	CDoubleVector &quicksort(CDoubleVector &v,CIntVector &idx, int low, int high, bool bAssc=true);
	CDoubleVector &quicksort(int low, int high, bool bUp=true); //double向量快速排序,原向量不保留 zfm
	AFX_EXT_CLASS friend CDoubleVector &quicksort(CDoubleVector &v,int low,int high, bool bUp=true); //double向量快速排序，原向量也保留 zfm
//////////////////////////////////////////////////////////////////////////
	AFX_EXT_CLASS friend CDoubleVector& MergeSort(CDoubleVector &v,bool bUp = true);
//////////////////////////////////////////////////////////////////////////
	AFX_EXT_CLASS friend CDoubleVector SimpleRegress(CDoubleVector Y , CDoubleVector X); //简单线性回归，返回参数：常数，系数
	AFX_EXT_CLASS friend double cov(const CDoubleVector &v1, const CDoubleVector &v2);
	AFX_EXT_CLASS friend double corrcoef(const CDoubleVector &v1, const CDoubleVector &v2);
//	AFX_EXT_CLASS friend double var(CDoubleVector &v);
	AFX_EXT_CLASS friend double st_d(const CDoubleVector &v);
	double var_w(const CIntVector &w)const ; //zfm 加权的 方差
	AFX_EXT_CLASS friend double var_w(const CDoubleVector &v,const CIntVector &w);//zfm 加权的 方差
	//AFX_EXT_CLASS friend CDoubleVector operator -(const CIntVector &v1, double x);  //CIntVector求方差专用
	//AFX_EXT_CLASS friend CDoubleVector operator *(const CDoubleVector &v, const CIntVector &w);	
	AFX_EXT_CLASS friend double var(const CDoubleVector &v); //zfm
	double var()const ; //zfm
	AFX_EXT_CLASS friend CDoubleVector normcdf(const CDoubleVector &vec, double mu, double sigma);
	AFX_EXT_CLASS friend CDoubleVector chi2cdf(const CDoubleVector &vec, int v);
	AFX_EXT_CLASS friend CDoubleVector fcdf(const CDoubleVector &vec, int v1, int v2);
	AFX_EXT_CLASS friend CDoubleVector tcdf(const CDoubleVector &vec, int v);
	AFX_EXT_CLASS friend CDoubleVector fcdf(const CDoubleVector &vec, const CIntVector &v1, const CIntVector &v2);
	AFX_EXT_CLASS friend CDoubleVector normpdf(const CDoubleVector &vec, double mu, double sigma);
	AFX_EXT_CLASS friend CDoubleVector chi2pdf(const CDoubleVector &vec, int v);
	AFX_EXT_CLASS friend CDoubleVector fpdf(const CDoubleVector &vec, int v1, int v2);
	AFX_EXT_CLASS friend CDoubleVector tpdf(const CDoubleVector &vec, int v);
	AFX_EXT_CLASS friend CDoubleVector norminv(const CDoubleVector &vec, double mu, double sigma);
	AFX_EXT_CLASS friend CDoubleVector chi2inv(const CDoubleVector &vec, int v);
	AFX_EXT_CLASS friend CDoubleVector finv(const CDoubleVector &vec, int v1, int v2);
	AFX_EXT_CLASS friend CDoubleVector tinv(const CDoubleVector &vec, int v);
	AFX_EXT_CLASS friend CIntVector grep2idx(const CIntVector &v);
};
#include "Complex.h"
////////////////////////////////////////////////////////////////////////////////////////////
///////////// ----------- CComplexVector ------------/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CComplexVector:public CVector<CComplex> 
{
public:
	CComplexVector():CVector<CComplex>(){};
	CComplexVector(int size, CComplex init =0.0):CVector<CComplex>(size, init){};
	CComplexVector(CComplex *a, int length):CVector<CComplex>(a,length){};
	CComplexVector(const CComplexVector &a1, const CComplexVector &a2)
	{CVector<CComplex>::create(a1,a2);};
    CComplexVector(const CComplexVector &a, int first, int last):CVector<CComplex>(a, first, last){};
	CComplexVector(const CComplexVector &x):CVector<CComplex>(x){};
	CComplexVector(const CVector<CComplex> &x):CVector<CComplex>(x){};
    CComplexVector(const CDoubleVector &re, const CDoubleVector &im);
    CComplexVector & operator =(const CComplexVector &c);
	CComplexVector & operator =(const CVector<CComplex> &c);
	CComplexVector & operator =(const CComplex &x);
	CComplexVector & operator =(const CDoubleVector &x);
	AFX_EXT_CLASS friend CComplexVector dvectocvec(const CDoubleVector &x);
	AFX_EXT_CLASS friend CComplexVector ivectocvec(const CIntVector &x);
	AFX_EXT_CLASS friend CComplexVector fvectocvec(const CFloatVector &x);
	AFX_EXT_CLASS friend CDoubleVector real(const CComplexVector &c);
	CDoubleVector real()const ;
	AFX_EXT_CLASS friend CDoubleVector imag(const CComplexVector &c);
	CDoubleVector imag()const ;
	AFX_EXT_CLASS friend CComplexVector conj(const CComplexVector &c);
	CComplexVector conj()const ;
	AFX_EXT_CLASS friend CComplexVector Complex(const CDoubleVector &v1, const CDoubleVector &v2);
	AFX_EXT_CLASS friend CComplexVector Complex(const CDoubleVector &v1, double x2);
	AFX_EXT_CLASS friend CComplexVector Complex(double x1, const CDoubleVector &v2);
    CComplex dot(const CComplexVector &v2)const ;
	AFX_EXT_CLASS friend CComplex dot(const CComplexVector &v1, const CComplexVector &v2);
    AFX_EXT_CLASS friend int any(const CComplexVector &v);
	int any()const ;
    AFX_EXT_CLASS friend CComplex Max(const CComplexVector &v);
	CComplex Max()const ;
    AFX_EXT_CLASS friend CComplexVector Max(const CComplexVector &v1, const CComplexVector &v2);
	CComplexVector Max(const CComplexVector &v2)const ;
    AFX_EXT_CLASS friend CComplex Min(const CComplexVector &v);
	CComplex Min()const ;
    AFX_EXT_CLASS friend CComplexVector Min(const CComplexVector &v1, const CComplexVector &v2);
	CComplexVector Min(const CComplexVector &v2)const ;
    AFX_EXT_CLASS friend CDoubleVector abs(const CComplexVector &v);
	CDoubleVector abs()const ;
	AFX_EXT_CLASS friend double norm(const CComplexVector &v, int n=2);
	double norm(int n=2)const ;
	CComplexVector & pow(const CComplex pp);
	AFX_EXT_CLASS friend CComplexVector pow(const CComplexVector &v, const CComplex pp);
	CComplexVector & pow(const CComplexVector &pp);
	AFX_EXT_CLASS friend CComplexVector pow(const CComplexVector &x, const CComplexVector &pp);
	CComplexVector & Rand();
	AFX_EXT_CLASS friend CComplexVector & Rand(CComplexVector &v);
    CComplexVector Rand(int m)const ;
	CComplexVector & Sort(bool select=0);
	AFX_EXT_CLASS friend CComplexVector Sort(CComplexVector &v, bool select=0);
};
////////////////////////////////////////////////////////////////////////////////////////////
///////////// -----------  ------------/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_VECTOR_H__2A023DF1_DB4F_4AFE_8D0D_ECE4957B797A__INCLUDED_)





















