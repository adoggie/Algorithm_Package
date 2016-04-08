// Dist_T.cpp: implementation of the Dist_T class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "TMath.h"
#include "float.h"


//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/********************************************************************
 *	概率分布函数类基类的方法实现
 Writer    余卫军
 Time      2005年4月13日
 
 ********************************************************************/
DistBase::DistBase()
{
}
DistBase::~DistBase()
{
}
double DistBase::cdf()
{
	return 0.0;
}
double DistBase::pdf()
{
	return 0.0;
}
double DistBase::invcdf()
{
	return 0.0;
}

/********************************************************************
 *	正态分布的方法实现
 ********************************************************************/
Dist_Norm::Dist_Norm()
{
//	assert(sigma > 0);
//	m_u = mu;
//	m_sigma = sigma; 
}

Dist_Norm::~Dist_Norm()
{
	
}
//probability function
double Dist_Norm::pdf(double x,double mu,double sigma) //custom mean and deviation
{
	return normpdf(x,mu,sigma);
}
//cumilate function
double Dist_Norm::cdf(double x,double mu,double sigma)
{
	return normcdf(x,mu,sigma);
}	

//inver cumulate function
double Dist_Norm::invcdf(double x,double mu ,double sigma)
{
	double dbResult =0.0;
	return norminv(x,mu,sigma);
}


////////////////////////////////// T分布的方法实现///////////////////////////////////
Dist_T::Dist_T()
{

}

Dist_T::~Dist_T()
{

}
double Dist_T::cdf(double x,int v)
{
	return tcdf(x,v);
}
double Dist_T::cdf(double x,double v)
{
	return tcdf(x,v);
}
double Dist_T::pdf(double x, int v)
{
	return tpdf(x,v);
}
double Dist_T::invcdf(double x,int v)
{
	return tinv(x,v);
}
double Dist_T::invcdf(double x,double v)
{
	return tinv(x,v);
}
//置信度a=q时对应的上分位数t

double Dist_T::pUp(int n, double q)
{
	return ptdist(n,q);
}
////////////////////////////// F 分布///////////////////////////
double Dist_F::cdf(double x, int v1, int v2)
{
	return fcdf(x,v1,v2);
}
double Dist_F::invcdf(double x, int v1, int v2)
{
	return finv(x,v1,v2);
}
double Dist_F::pdf(double x, int v1, int v2)
{
	return fpdf(x,v1,v2);
}
double Dist_F::pUp(int v1, int v2,double q)
{
	return pfdist(v1,v2,q);
}
/////////////////////////均匀分布///////////////////////////
double Dist_Uniform::pdf(double x, double a /* = 0.0 */, double b /* = 1.0 */)
{
	return uniformpdf(x,a,b);
}
double Dist_Uniform::cdf(double x, double a /* = 0.0 */, double b /* = 1.0 */)
{
	return uniformcdf(x,a,b);
}
double Dist_Uniform::invcdf(double p, double a /* = 0.0 */, double b /* = 1.0 */)
{
	return uniforminv(p,a,b);
}
////////////////////////指数分布///////////////////////////////////////////////
double Dist_Exp::pdf(double x, double mu)
{
	return exppdf(x,mu);
}
double Dist_Exp::cdf(double x, double mu)
{
	return expcdf(x,mu);
}
double Dist_Exp::invcdf(double p, double mu)
{
	return expinv(p,mu);
}
/////////////////////////////卡方分布///////////////////
double Dist_Chi2::pdf(double x, int v)
{
	return chi2pdf(x,v);
}
double Dist_Chi2::cdf(double x, int v)
{
	return chi2cdf(x,v);
}
double Dist_Chi2::invcdf(double x, int v)
{
	return chi2inv(x,v);
}

/////////////////////////////////对数正态分布 7////////////////////////
double Dist_LogNorm::pdf(double x, double mu, double sigma)
{
	return lognpdf(x,mu,sigma);
}
double Dist_LogNorm::cdf(double x, double mu, double sigma)
{
	return logncdf(x,mu,sigma);
}
double Dist_LogNorm::invcdf(double p, double mu, double sigma)
{
	return logninv(p,mu,sigma);
}
/////////////////////////////////////////泊松分布   8 ///////////////////
double Dist_Poisson::pdf(int x, double lamda)
{
	return poisspdf(x,lamda);
}
double Dist_Poisson::cdf(int x, double lamda)
{
	return poisscdf(x,lamda);
}
double Dist_Poisson::invcdf(double p, double lamda)
{
	return poissinv(p,lamda);
}
/////////////////////////////////威布尔分布   9 //////////////////
double Dist_Weibull::pdf(double x, double a, double b)
{
	return weibpdf(x,a,b);
}
double Dist_Weibull::cdf(double x, double a, double b)
{
	return weibcdf(x,a,b);
}
double Dist_Weibull::invcdf(double p, double a, double b)
{
	return weibinv(p,a,b);
}
//////////////////////////////////Gamma分布   10 ////////////////
double Dist_Gamma::pdf(double x, double a, double b)
{
	return gampdf(x,a,b);
}
double Dist_Gamma::cdf(double x, double a, double b)
{
	return gamcdf(x,a,b);
}
double Dist_Gamma::invcdf(double p, double a, double b)
{
	return gaminv(p,a,b);
}
double Dist_Gamma::gamma(double a, double x)
{
	return gamma(a,x);
}
double Dist_Gamma::gamma(double x)
{
	return gamma(x);
}
double Dist_Gamma::gammaln(double x)
{
	return gammaln(x);
}
///////////////////////几何分布  11 //////////////////////
double Dist_Geometric::pdf(double x, double p)
{
	return geopdf(x,p);
}
double Dist_Geometric::cdf(double x, double p)
{
	return geocdf(x,p);
}
double Dist_Geometric::invcdf(double y, double p)
{
	return geoinv(y,p);
}
/////////////////////////Beta分布  12 ////////////////////////////
double Dist_Beta::pdf(double x, double a, double b)
{
	return betapdf(x,a,b);
	
}
double Dist_Beta::cdf(double x, double a, double b)
{
	return betacdf(x,a,b);
}
double Dist_Beta::invcdf(double p, double a, double b)
{
	return betainv(p,a,b);
}
//////////////////////////////////超几何分布   13 ////////////
double Dist_hygeo::pdf(int x, int m, int k, int n)
{
	return hygepdf(x,m,k,n);
}
double Dist_hygeo::cdf(int x, int m, int k, int n)
{
	return hygecdf(x,m,k,n);
}
double Dist_hygeo::invcdf(double p, int m, int k, int n)
{
	return hygeinv(p,m,k,n);
}
///////////////////////////////////////半正态分布  14 ////////////////
double Dist_halfnorm::pdf(double x, double theta)
{
	return halfnpdf(x,theta);
}
double Dist_halfnorm::cdf(double x, double theta)
{
	return halfncdf(x,theta);
}
double Dist_halfnorm::invcdf(double p, double theta)
{
	return halfninv(p,theta);
}
////////////////////////////////Laplace分布  15 ///////////////////
double Dist_Laplace::pdf(double x, double b, double mu)
{
	return laplacepdf(x,b,mu);
}
double Dist_Laplace::cdf(double x, double b, double mu)
{
	return laplacecdf(x,b,mu);
}
double Dist_Laplace::invcdf(double p, double b, double mu)
{
	return laplaceinv(p,b,mu);
}
////////////////////////////////////Logistic分布  16 ////////////////////
double Dist_Logist::pdf(double x, double m, double b)
{
	return logistpdf(x,m,b);
}
double Dist_Logist::cdf(double x, double m, double b)
{
	return logistcdf(x,m,b);
}
double Dist_Logist::invcdf(double p, double m, double b)
{
	return logistinv(p,m,b);
}
//////////////////////////////////////Pareto分布   17 ////////////////
double Dist_Pareto::pdf(double x, double a, double b)
{
	return paretopdf(x,a,b);
}
double Dist_Pareto::cdf(double x, double a, double b)
{
	return paretocdf(x,a,b);
}
double Dist_Pareto::invcdf(double p, double a, double b)
{
	return paretoinv(p,a,b);
}


///////////////////// //durbin-waston检验  18 ///////////////////
double Dist_DWTest::Lowlpdf(int n, int k, double r)
{
	return dwlpdf(n,k,r);
}
double Dist_DWTest::Lowcdf(int n, int k, double r)
{
	return dwlcdf(n,k,r);
}

double Dist_DWTest::Lowinv(int n, int k, double p)
{
	return dwlinv(n,k,p);
}

double Dist_DWTest::Uppdf(int n, int k, double r)
{
	return dwhpdf(n,k,r);
}

double Dist_DWTest::Upcdf(int n, int k, double r)
{
	return dwhcdf(n,k,r);
}

double Dist_DWTest::Upinv(int n, int k, double r)
{
	return dwhinv(n,k,r);
}

///////////////////////////////////studentized-range 分布   19//////////
double Dist_StuRang::cdf(double q, int v, int r, int jmax/* =15 */, int kmax/* =15 */)
{
	return stdrcdf(q,v,r,jmax,kmax);
}
double Dist_StuRang::invcdf(double p, int v, int r)
{
	return stdrinv(p,v,r);
}


double ** twoarrayalloc(int m, int n)
{
	double **y;
	y = new double *[m];
	if(y == NULL)
		exit(EXIT_FAILURE);
	for(int i=0; i<m; i++)
	{
		y[i] = new double[n];
		if(y[i] == NULL)
			exit(EXIT_FAILURE);
	}
	return y;
}

//{{{START 2003-1-22 Eric   Change arguments
//void twoarrayfree(double **y, int m)
void twoarrayfree(double ** y, int m)
//}}}END  2003-1-22 Eric
{
	for(int i=0; i<m; i++)
		delete y[i];
	delete y;
}

//{{{START 2003-1-22 Eric   Change arguments
//int Matdet(double **m1, int numrow, double &det)
void orithes(int n, double tol, double **a, double *d)
{
	double f, g, h;
	int i, j, k, l, m, la;
	k = 1;
	l = n;
	la = l - 1;
	for(m=k+1; m<=la; m++)
	{
		h =0;
		for(i=l; i>=m; i--)
		{
			f = a[i-1][m-2];
			d[i-1] = f;
			h = h + f*f;
		}
		if(h<=tol)
		{
			a[m-1][m-2] = 0;
			continue;
		}
		if(f>=0)
			g=-sqrt(h);
		else
			g = sqrt(h);
		h = h-f*g;
		d[m-1]=f-g;
		
		for(j=m; j<=n; j++)
		{
			f=0;
			for(i=l; i>=m; i--)
				f=f+d[i-1]*a[i-1][j-1];
			f = f/h;
			for(i=m;i<=l; i++)
				a[i-1][j-1] = a[i-1][j-1] - f*d[i-1];
		}
		for(i=1; i<=l;i++)
		{
			f=0;
			for(j=l;j>=m;j--)
				f=f+d[j-1]*a[i-1][j-1];
			f = f/h;
			for(j=m;j<=l;j++)
				a[i-1][j-1] = a[i-1][j-1]-f*d[j-1];
		}
		a[m-1][m-2] = g;
	}
}

void ortrans(int n, double **h, double *d, double **v)
{
	int i, j, k, m, low, upp;
	double x, y;
	low = 1;
	upp = n;
	for(i=1;i<=n;i++)
	{
		for(j=1;j<=n;j++)
			v[i-1][j-1] = 0;
		v[i-1][i-1] = 1.0;
	}
	for(k=upp-2;k>=low;k--)
	{
		m = k+1;
		y = h[m-1][k-1];
		if(y==0)
			continue;
		y = y*d[m-1];
		for(i=k+2;i<=upp;i++)
			d[i-1] = h[i-1][k-1];
		for(j=m;j<=upp;j++)
		{
			x=0;
			for(i=m; i<=upp; i++)
				x += d[i-1]*v[i-1][j-1];
			x = x/y;
			for(i=m; i<=upp; i++)
				v[i-1][j-1] += x*d[i-1];
		}
	}
}


void sss(double *fg, double *cs)
{
	double r, d;
	if((fabs(fg[0])+fabs(fg[1]))==0)
	{
		cs[0] = 1;
		cs[1] = 0;
		d = 0;
	}
	else
	{
		d = sqrt(fg[0]*fg[0]+fg[1]*fg[1]);
		if(fabs(fg[0])>fabs(fg[1]))
		{
			d = fabs(d);
			if(fg[0]<0)
				d = -d;
		}
		if(fabs(fg[1])>=fabs(fg[0]))
		{
			d = fabs(d);
			if(fg[1]<0)
				d = -d;
		}
		cs[0] = fg[0]/d;
		cs[1] = fg[1]/d;
	}
	r=1;
	if(fabs(fg[0])>fabs(fg[1]))
		r=cs[1];
	else 
	{
		if(cs[0]!=0)
		    r = 1.0/cs[0];
	}
	fg[0] = d;
	fg[1] = r;
	return;
}

//{{{START 2003-1-22 Eric   Change arguments
//}}}END  2003-1-22 Eric
//int dluav(double **a, double **u, double **v, int m, int n, double eps)



void cdiv(double xr, double xi, double yr, double yi, double *zr, double *zi)
{
    double h;
	if(fabs(yr)>fabs(yi))
	{
		h = yi/yr;
		yr = h*yi + yr;
		*zr = (xr + h*xi)/yr;
		*zi = (xi - h*xr)/yr;
	}
	else
	{
		h = yr/yi;
		yi = h*yr + yi;
		*zr = (h*xr +xi)/yi;
		*zi = (h*xi -xr)/yi;
	}
}


int hqr2( int n, double ep, double **h, double **v, double *wr, double *wi, int *cnt)
{
	int i,j,k,l,m,na,its,en,nl,la,low,upp;
	double p,q,r,s,t,w,x,y,z,ra,sa,vr,vi,norm;
	div_t divmess;
	low = 1;
	upp = n;
	en = upp;
	t = 0;
	while(en>=low)
	{
		its =0;
		na = en -1;
		while(1)
		{
			la =0;
			for(l=en; l>=low+1; l--)
			{
				if(fabs(h[l-1][l-2])<=ep*(fabs(h[l-2][l-2])+fabs(h[l-1][l-1])))
				{
					la =1;
					break;
				}
			}
			if(!la)
				l = low;
			x= h[en-1][en-1];
			if(l == en)
			{
				la = 0;
				break;
			}
			y = h[na-1][na-1];
			w = h[en-1][na-1]*h[na-1][en-1];
			if(l==na)
			{
				la = 1;
				break;
			}
			if(its == MAX_ITERATETIME)
			{
				cnt[en - 1] = MAX_ITERATETIME + 1;
				return EMAT_OVERITERATE;
			}
			divmess = div(its,10);
			if(divmess.rem == 0)
			{
				t += x;
				for(i=low; i<=en; i++)
					h[i-1][i-1] -=x;
				s = fabs(h[en-1][na-1])+fabs(h[na-1][en-3]);
				x = y =0.75*s;
				w = -0.4375*s*s;
			}
			its += 1;
			for(m = en-2; m>=1; m--)//????
			{
				z=h[m-1][m-1];
				r=x-z;
				s=y-z;
				if(fabs(h[m][m-1])<ep)
				{
					return EMAT_DIVBYZERO;
				}
				p = (r*s-w)/h[m][m-1] + h[m-1][m];
				q = h[m][m]-z-r-s;
				r = h[m+1][m];
				s = fabs(p)+fabs(q)+fabs(r);
				if(s<ep)
				{
					return EMAT_DIVBYZERO;
				}
				p = p/s;
				q = q/s;
				r = r/s;
            	if(m==l)
					break;
				if(fabs(h[m-1][m-2])*(fabs(q)+fabs(r)) <= ep*fabs(p)*
					(fabs(h[m-2][m-2]) + fabs(z) + fabs(h[m][m])))
					break;
			}
			for(i=m+2;i<=en; i++)
				h[i-1][i-3] = 0;
			for(i=m+3;i<=en; i++)
				h[i-1][i-4] = 0;
			for(k=m; k<=na; k++)
			{
				if(k != na)
					nl =1;
				else
					nl =0;
				if(k!=m)
				{
					p = h[k-1][k-2];
					q = h[k][k-2];
					if(nl)
						r = h[k+1][k-2];
					else
						r = 0;
					x = fabs(p) +fabs(q)+ fabs(r);
					if(x<ep)
						continue;
					p =p/x;
					q =q/x;
					r =r/x;
				}
				s = sqrt(p*p+q*q+r*r);
				if(p<0)
					s=-s;
				if(k!=m)
					h[k-1][k-2] = -s*x;
				else if(l!=m)
					h[k-1][k-2] = -h[k-1][k-2];
				p += s;
				if((fabs(p)<ep)||(fabs(s)<ep))
				{
					return EMAT_DIVBYZERO;
				}
				x = p/s;
				y = q/s;
				z = r/s;
				q /=p;
				r /=p;
				for(j=k; j<=n; j++)
				{
					p=h[k-1][j-1] + q*h[k][j-1];
					if(nl)
					{
						p +=r*h[k+1][j-1];
						h[k+1][j-1] -= p*z;
					}
					h[k][j-1] -= p*y;
					h[k-1][j-1] -=p*x;
				}
				if(k+3 <en)
					j = k+3;
				else
					j=en;
				for(i=1; i<=j; i++)
				{
					p = x*h[i-1][k-1] +y*h[i-1][k];
					if(nl)
					{
						p += z*h[i-1][k+1];
						h[i-1][k+1] -= p*r;
					}
					h[i-1][k] -=p*q;
					h[i-1][k-1] -=p;
				}
				for(i=low; i<=upp; i++)
				{
					p = x*v[i-1][k-1] + y*v[i-1][k];
					if(nl)
					{
						p += z*v[i-1][k+1];
						v[i-1][k+1] -=p*r;
					}
					v[i-1][k] -= p*q;
					v[i-1][k-1] -= p;
				}
			}
		}
		if(! la)
		{
			wr[en-1]= h[en -1][en -1]= x+t;
			wi[en-1]=0;
			if(fabs(wr[en-1])<DBL_EPSILON)
				wr[en-1] = 0;
			cnt[en -1] =its;
			en = na;
			continue;
		}
		p = (y-x)/2;
		q = p*p +w;
		z = sqrt(fabs(q));
        x = h[en-1][en-1] = x+t;
		h[na-1][na-1] = y+t;
		cnt[en-1] = -its;
		cnt[na-1] = its;
		if(q>0)
		{
			if(p<0)
				z= p-z;
			else
				z = p+z;
			if(fabs(z)<ep)
			{
				return EMAT_DIVBYZERO;
			}
			wr[na -1 ] = x+z;
			wr[en -1 ] =s=x-w/z;
			wi[na-1] = wi[en-1]=0;
			if(fabs(wr[en-1])<DBL_EPSILON)
				wr[en-1] =0;
			if(fabs(wr[na-1])<DBL_EPSILON)
				wr[na-1] =0;
			x = h[en-1][na-1];
			r = sqrt(x*x + z*z);
			if(r<ep)
			{
				return EMAT_DIVBYZERO;
			}
			p= x/r;
			q = z/r;
			for(j=na; j<=n; j++)
			{
				z= h[na-1][j-1];
				h[na-1][j-1] = q*z + p*h[en-1][j-1];
				h[en-1][j-1] = q*h[en-1][j-1] - p*z;
			}
			for(i=1; i<=en; i++)
			{
				z= h[i-1][na-1];
				h[i-1][na-1] = q*z + p*h[i-1][en-1];
				h[i-1][en-1] = q*h[i-1][en-1] - p*z;
			}
			for(i=low; i<=upp; i++)
			{
				z = v[i-1][na-1];
				v[i-1][na-1] = q*z + p*v[i-1][en-1];
				v[i-1][en-1] = q*v[i-1][en-1] - p*z;
			}
		}
		else
		{
			wr[na-1] = wr[en-1] = x+p;
			if(fabs(wr[en-1])<DBL_EPSILON)
			{
				wr[en-1]=0;
				wr[na-1]=0;
			}
			wi[na-1]=z;
			wi[en-1]=-z;
			if(fabs(z)<DBL_EPSILON)
			{
				wi[en-1]=0;
				wi[na-1]=0;
			}
		}
		en -=2;
	}
	norm = 0;
	k = 1;
	for(i=1; i<=n; i++)
	{
		for(j=k; j<=n; j++)
			norm += fabs(h[i-1][j-1]);
		k=i;
	}
	if(norm<DBL_EPSILON)
		return EMAT_FAILURE;
	for(en=n; en>=1; en--)
	{
		p = wr[en-1];
		q = wi[en-1];
		na = en-1;
		if(fabs(q)<ep)
		{
			m=en;
			h[en-1][en-1] =1;
			for(i=na; i>=1; i--)
			{
				w= h[i-1][i-1]-p;
				r= h[i-1][en-1];
				for(j=m; j<=na; j++)
					r += h[i-1][j-1]*h[j-1][en-1];
				if(wi[i-1]<0)
				{
					z=w;
					s=r;
				}
				else
				{
					m=i;
					if(fabs(wi[i-1])<ep)
					{
						if(fabs(w)>=ep)
							h[i-1][en-1] = -r/w;
						else
							h[i-1][en-1] = -r/(ep*norm);
					}
					else
					{
						x = h[i-1][i];
						y = h[i][i-1];
						q = (wr[i-1] - p)*(wr[i-1] -p) + wi[i-1]*wi[i-1];
						if((q<ep)||(fabs(x)<ep)||(fabs(z)<ep))
						{
							return EMAT_DIVBYZERO;
						}
						h[i-1][en-1] = t = (x*s-z*r)/q;
						if(fabs(x) > fabs(z))
							h[i][en-1] = (-r-w*t)/x;
						else
							h[i][en-1]= (-s-y*t)/z;
					}
				}
			}
		}
		else
		{
			if(q<0)
			{
				m = na;
				if(fabs(h[en-1][na-1])>fabs(h[na-1][en-1]))
				{
					if(fabs(h[en-1][na-1])<ep)
					{
						return EMAT_DIVBYZERO;
					}
					h[na-1][na-1] = -(h[en-1][en-1]-p)/h[en-1][na-1];
					h[na-1][en-1] = -q/h[en-1][na-1];
				}
				else
					cdiv(-h[na-1][en-1],0,h[na-1][na-1]-p,q, &h[na-1][na-1], &h[na-1][en-1]);
				h[en-1][na-1] = 1;
				h[en-1][en-1] = 0;
				for(i=na-1; i>=1; i--)
				{
					w = h[i-1][i-1] -p;
					ra = h[i-1][en-1];
					sa =0;
					for(j=m; j<=na; j++)
					{
						ra += h[i-1][j-1]*h[j-1][na-1];
						sa += h[i-1][j-1]*h[j-1][en-1];
					}
					if(wi[i-1]<0)
					{
						z=w;
						r=ra;
						s=sa;
					}
					else
					{
						m=i;
						if(fabs(wi[i-1])<=ep)
							cdiv(-ra, -sa, w,q,&h[i-1][na-1], &h[i-1][en-1]);
						else
						{
							x=h[i-1][i];
							y = h[i][i-1];
							vr = (wr[i-1]-p)*(wr[i-1]-p) +wi[i-1]*wi[i-1] -q*q;
							vi =(wr[i-1] -p)*2*q;
							if((fabs(vr)<=ep)&&(fabs(vi)<=ep))
								vr = ep*norm*(fabs(w) + fabs(q) +fabs(x) + fabs(y) + fabs(z));
							cdiv(x*r - z*ra + q*sa, x*s-z*sa -q*ra, vr, vi,&h[i-1][na-1], &h[i-1][en-1]);
							if(fabs(x)>(fabs(z)+fabs(q)))
							{
								if(fabs(x)<ep)
								{
									return EMAT_DIVBYZERO;
								}
								h[i][na-1] = (-ra-w*h[i-1][na-1] + q*h[i-1][en-1])/x;
								h[i][en-1] = (-sa -w*h[i-1][en-1] - q*h[i-1][na-1])/x;
							}
							else
								cdiv(-r-y*h[i-1][na-1], -s-y*h[i-1][en-1], z,q, &h[i][na-1], &h[i][en-1]);
						}
					}
				}
			}
		}
	}
	for(i=1; i<=low-1; i++)
		for(j=i+1; j<=n; j++)
			v[i-1][j-1] = h[i-1][j-1];
	for(i=upp+1; i<=n; i++)
		for(j=i+1; j<=n; j++)
			v[i-1][j-1] = h[i-1][j-1];
	for(j=n; j>=low; j--)
	{
		if(j<=upp)
			m=j;
		else
			m=upp;
		l=j-1;
		if(wi[j-1]<0)
		{
			for(i=low; i<=upp; i++)
			{
				y=z=0;
				for(k=low; k<=m; k++)
				{
					y += v[i-1][k-1]*h[k-1][l-1];
					z += v[i-1][k-1]*h[k-1][j-1];
				}											
				v[i-1][l-1]=y;
				v[i-1][j-1]=z;
			}

		}		
		else if(fabs(wi[j-1])<ep)
		{
			for(i=low;i<=upp; i++)
			{
				z=0;
				for(k=low; k<=m;k++)
					z+=v[i-1][k-1]*h[k-1][j-1];
				v[i-1][j-1]=z;
			}
		}
	}
	return 0;
}


double normcdf(double x, double mu, double sigma)
{
	double y;
	if (sigma<=0.0) sigma=1.0e-10;
	y=0.5+0.5*lerrf((x-mu)/(sqrt(2.0)*sigma));
	return(y);
}


double lgam1(double x)
{
	int i;
	double y,t,s,u;
	static double a[11]={ 0.0000677106,-0.0003442342,
		0.0015397681,-0.0024467480,0.0109736958,
		-0.0002109075,0.0742379071,0.0815782188,
		0.4118402518,0.4227843370,1.0};
	//if (x<=0.0)
	//{ printf("err**x<=0!\n"); return(-1.0);}
	assert(x>0);
	y=x;
	if (y<=1.0)
	{
		t=1.0/(y*(y+1.0));
		y=y+2.0;
	}
	else if (y<=2.0)
	{ 
		t=1.0/y; 
		y=y+1.0;
	}
	else if (y<=3.0)
		t=1.0;
	else
	{ 
		t=1.0;
		while (y>3.0)
		{
			y=y-1.0; 
			t=t*y;
		}
	}
	s=a[0];
	u=y-2.0;
	for (i=1; i<=10; i++)
		s=s*u+a[i];
	s=s*t;
	return(s);
}

double lgam2(double a, double x)
{ 
	int n;
	double p,q,d,s,s1,p0,q0,p1,q1,qq;
	assert((a>0.0)&&(x>=0.0)); //zfm
/*	if ((a<=0.0)||(x<0.0))
	{ 
		if (a<=0.0) printf("err**a<=0!\n");
		if (x<0.0) printf("err**x<0!\n");
		return(-1.0);
	}
*/
	if (x+1.0==1.0) return(0.0);
	if (x>1.0e+35) return(1.0);
	q=log(x); q=a*q; qq=exp(q);
	if (x<1.0+a)
	{
		p=a; d=1.0/a; s=d;
		for (n=1; n<=100; n++)
		{
			p=1.0+p; d=d*x/p; s=s+d;
			if (fabs(d)<fabs(s)*1.0e-07)
			{ 
				s=s*exp(-x)*qq/lgam1(a);
				return(s);
			}
		}
	}
	else
	{
		s=1.0/x; p0=0.0; p1=1.0; q0=1.0; q1=x;
		for (n=1; n<=100; n++)
		{ 
			p0=p1+(n-a)*p0; q0=q1+(n-a)*q0;
			p=x*p0+n*p1; q=x*q0+n*q1;
			if (fabs(q)+1.0!=1.0)
			{ 
				s1=p/q; p1=p; q1=q;
				if (fabs((s1-s)/s1)<1.0e-07)
				{
					s=s1*exp(-x)*qq/lgam1(a);
					return(1.0-s);
				}
				s=s1;
			}
			p1=p; q1=q;
		}
	}
	//printf("a too large !\n"); 
	s=1.0-s*exp(-x)*qq/lgam1(a);
	return(s);
//}

double lerrf(double x)
{
	double y;
	if (x>=0.0)
		y=lgam2(0.5,x*x);
	else
		y=-lgam2(0.5,x*x);
	return(y);
}

double norminv(double p, double mu, double sigma)
{
    assert ((p>DOUBLE_ZERO)&&(p<1-DOUBLE_ZERO)); //ZFM
	const double b0 = 1.570796288;
	const double b1 = 0.3706987906e-01;
	const double b2 = -0.8364353589e-03;
	const double b3 = -0.2250947176e-03;
    const double b4 = 0.6841218299e-05;
	const double b5 = 0.5824238515e-05;
	const double b6 = -0.10452747970e-05;
	const double b7 = 0.8360937017e-07;
	const double b8 = -0.3231081277e-08;
	const double b9 = 0.3657763036e-10;
	const double b10 = 0.6936233982e-12;	
	
	double u0, uu, y, z, pp, d;
	int k;
	
	if(fabs(p-0.5) < DOUBLE_ZERO)
	{
		return mu;
	}
	y = -log(4*p*(1-p));
	z = y*(b0+y*(b1+y*(b2+y*(b3+y*(b4+y*(b5+y*(b6+y*(b7+y*(b8+y*(b9+y*b10))))))))));
	u0 = sqrt(z);
    if(p <0.5)
		u0 = -u0;
    for(k = 1; k<=MAX_COUNT; k++)
	{
		pp = normcdf(u0,0,1);
        d = exp(-u0*u0/2)/(sqrt(2*PI));
		if( fabs(d)<DOUBLE_ZERO)
		{
			uu = u0;
			return uu*sigma+mu;
		}
		uu = u0 - (pp - p)/d;
		if( fabs(uu - u0)<EPS_ACCUR*fabs(uu))
			return uu*sigma+mu;
		u0 = uu;
	}
	return uu*sigma+mu;

}

double normpdf(double x, double mu, double sigma)
{
	return exp(-(x-mu)*(x-mu)/(2*sigma*sigma))/(sqrt(2*PI)*sigma);
}

double norm(double x) //N(0,1)正态分布p(x<u)
{
	return normcdf(x, 0, 1);
}

double pnorm(double p)//N(0,1)置信度a=p时对应的上分位数Ua
{
    assert((p>DOUBLE_ZERO)&&(p<1-DOUBLE_ZERO)); //zfm

	const double b0 = 1.570796288;
	const double b1 = 0.3706987906e-01;
	const double b2 = -0.8364353589e-03;
	const double b3 = -0.2250947176e-03;
    const double b4 = 0.6841218299e-05;
	const double b5 = 0.5824238515e-05;
	const double b6 = -0.10452747970e-05;
	const double b7 = 0.8360937017e-07;
	const double b8 = -0.3231081277e-08;
	const double b9 = 0.3657763036e-10;
	const double b10 = 0.6936233982e-12;	
	
	double u0, u, y, z, pp, q, d;
	int k;
	
	if(fabs(p-0.5) < DOUBLE_ZERO)
	{
		u = 0.0;
		return u;
	}
	q = 1-p;
	y = -log(4*q*(1-q));
	z = y*(b0+y*(b1+y*(b2+y*(b3+y*(b4+y*(b5+y*(b6+y*(b7+y*(b8+y*(b9+y*b10))))))))));
	u0 = sqrt(z);
    if(p > 0.5)
		u0 = -u0;
	
    for(k = 1; k<=MAX_COUNT; k++)
	{
		pp = norm(u0);
        d = exp(-u0*u0/2)/(sqrt(2*PI));
		if( fabs(d)<DOUBLE_ZERO)
		{
			u = u0;
			return u;
		}
		u = u0 - (pp - q)/d;
		if( fabs(u - u0)<EPS_ACCUR*fabs(u))
			return u;
		u0 = u;
	}
	return u;
}

double chi2cdf1(double x, int v)
{
    return gamcdf(x,v/2.0, 2.0);
}

double chi2cdf(double x, int v)
{
    
	assert(v>0); //zfm
	double PIs,xx,chs,pp, p;
	long double u;
	int iai;
		
	if( x<=1e-30)  //zfm
	{
		p=0.0;  //zfm
		return p; //zfm
	}


	if(v<=MAX_DEGREE && x<1419.56)
	{
		PIs = sqrt(PI);
		xx = x/2;
		chs = sqrt(x);
		
		
		if(fabs(x)<1.0e-30)
		{
			p = 0.0;
			return p;
		}
		
	

		if(v%2 == 0)
		{
			u = xx*exp(-xx);
			p = 1 - exp(-xx);
			iai = 2;
		}
		else
		{
			u = sqrt(xx)*exp(-xx)/PIs;
			pp = normcdf(chs,0,1);
			p = 2*(pp - 0.5);
			iai = 1;
		}
		if( iai == v)
		{
			return p;
		}
		else
		{
			for(int i = iai; i<=v-2; i+=2)
			{
				p = p - 2*u/i;
				u = x*u/i;
			}
		}
	}
	else
	{
		p = chi2cdf1(x,v);
	}
	if(p>1)
		p=1;
	if(p<0)
		p=0;
	return p;
}

double chi2pdf1(double x, int v)    YY版本
{
    return gampdf(x,v/2.0,2.0);
}

double chi2pdf(double x, int v)
{
    double PIs,xx,chs,u, d;
	int iai;
	if(v<=MAX_DEGREE)
	{
		PIs = sqrt(PI);
		xx = x/2;
		chs = sqrt(x);
		if(fabs(x)<1.0e-30)
		{
			d = 0.0;
			return d;
		}
		if(v%2 == 0)
		{
			u = xx*exp(-xx);
			iai = 2;
		}
		else
		{
			iai = 1;
		}
		if( iai == v)
		{
			d = u/x;
			return d;
		}
		else
		{
			for(int i = iai; i<=v-2; i+=2)
			{
				u =x*u/i;
			}
		}
		d = u/x;
	}
	else
	{
		d=chi2pdf1(x,v);
	}
	return d;
}



double chi2pdf(double x, int v)  //zfm  用以前的chi2pdf1完全代替chi2pdf，从而和matlab一样
{
	return gampdf(x,v/2.0,2.0);
}




double chi2inv(double p, int v)
{
    assert(v!=0); //zfm
	double x, xx, x0, pp, d, w;
	int k;
	if ((fabs(p)<=DOUBLE_ZERO))//zfm
		return 0;//zfm
	if(v==1)
	{
		x = norminv((p+1)/2.0, 0, 1);
		xx = x*x;
		return xx;
	}
    if(v==2)
	{
		xx = -2*log(p);
        return xx;
	}  
	x = norminv(p, 0, 1);
	w = 2.0/9.0/v;
	x0 =(1-w+x*sqrt(w));
	x0 =v*x0*x0*x0;
    for(k = 1; k<=MAX_COUNT; k++)
	{
		pp = chi2cdf(x0, v);
		d = chi2pdf(x0, v);
		if( fabs(d)<DOUBLE_ZERO)
		{
			xx = x0;
			return xx;
		}
		xx = x0 - (pp - p)/d;
		if( fabs(xx - x0)<EPS_ACCUR*fabs(xx))
			return xx;
		x0 = fabs(xx);
	}
	return xx;
}


//chi方 分布返回p(x<ch) 自由度为n, d为对应的概率密度值
double chi2(double ch, int n, double *d) 
{
    double PIs,x,chs,u,pp, p;
	int iai;
	PIs = sqrt(PI);
	x = ch/2;
	chs = sqrt(ch);
	
	if(fabs(ch)<1.0e-30)
	{
		p = 0.0;
		*d = 0.0;
		return p;
	}
	if(n%2 == 0)
	{
		u = x*exp(-x);
		p = 1 - exp(-x);
		iai = 2;
	}
	else
	{
		u = sqrt(x)*exp(-x)/PIs;
		pp = norm(chs);
		p = 2*(pp - 0.5);
		iai = 1;
	}
    if( iai == n)
	{
		*d = u/ch;
		return p;
	}
	else
	{
		for(int i = iai; i<=n-2; i+=2)
		{
			p = p - 2*u/i;
			u =ch*u/i;
		}
	}
	*d = u/ch;
	return p;
}

//置信度a=p时对应的上分位数ch
double pchi2(int n, double p)
{
    double x, xx, x0, pp, q, d, w;
	int k;
	
	if(n==1)
	{
		x = pnorm(p/2);
		xx = x*x;
		return xx;
	}
    if(n==2)
	{
		xx = -2*log(p);
        return xx;
	}  
    q = 1 - p;
	x = pnorm(p);
	
	w = 2.0/9.0/n;
	x0 =(1-w+x*sqrt(w));
	x0 =n*x0*x0*x0;
    for(k = 1; k<=MAX_COUNT; k++)
	{
		pp = chi2(x0, n, &d);
		if( fabs(d)<DOUBLE_ZERO)
		{
			xx = x0;
			return xx;
		}
		xx = x0 - (pp - q)/d;
		if( fabs(xx - x0)<EPS_ACCUR*fabs(xx))
			return xx;
		x0 = fabs(xx);
	}
	return xx;
}


double gammaln(double x)
{
    int i,n;
	double y, z, a, b, g;
	if(x<8.0)
	{
		y = x + 8;
		n = -1;
	}
	else
	{
		y = x;
		n = 1;
	}
	z = 1/(y*y);
	a = (y-0.5)*log(y) - y + 0.9189385;
	b = ((((0.0007663452*z - 0.0005940956)*z + 0.0007936431)*z - 0.002777778)*z +0.0833333)/y;
	g = a + b;
	if(n>0)
		return g;
	y = y - 1;
	a = y;
	for(i = 1; i<=7; i++)
		a = a*(y-i);
	g = g - log(a);
	return g;
}

double tpdf1(double x, int v)
{
	double y = 0;
	if(v > 0)
	{
		double term = exp(gammaln((v + 1) / 2.0) - gammaln(v/2.0));
		y = term / (sqrt(v*PI) * pow(1 + x*x/v, (v + 1)/2.0));
	}
    return y;
}

double tpdf(double x, int v)
{
	int sign, ibi, i;
	double tt, xx, u, ga1, ga2, dd;
	if(v <=MAX_DEGREE)
	{
		if(fabs(x)<DOUBLE_ZERO)
		{
			ga1 = lgam1(v/2.0);
			ga2 = lgam1(v/2.0+0.5);
			dd = ga2/(sqrt(v*PI)*ga1);
			return dd;
		}
		if(x <0.0)
			sign =-1;
		else
			sign = 1;
		tt = x*x;
		xx = tt/(v+tt);
		if(v%2 == 0)
		{
			u = sqrt(xx)*(1-xx)/2;
			ibi = 2;
		}
		else
		{
			u = sqrt(xx*(1-xx))/PI;
			ibi = 1;
		}
		if(ibi == v)
		{
			dd = u/fabs(x);
			return dd;
		}
		for(i = ibi; i<=v-2; i+=2)
		{
			u = u*(1+i)/i*(1-xx);
		}
		dd = u/fabs(x);
	}
	else
	{
		dd = tpdf1(x,v);
	}
	return dd;
}

double tcdf1(double x, int v)
{
    double normcutoff = 1e7;
    double p = 0;
    if(v==1)
        p = 0.5 + atan(x)/PI;
    if(v>=normcutoff)
        p = normcdf(x,0,1);
    if(x != 0 && v != 1 && v > 0 && v < normcutoff)
	{
		double xx = v/ (v + x*x);
		p = betainc(xx, v/2.0, 0.5)/2.0;
	}
    if(x > 0 && v != 1 && v > 0 && v < normcutoff)
        p = 1 - p;
    if(fabs(x)<DOUBLE_ZERO && v > 1)
		p = 0.5;
    return p;
}

double tcdf(double x, double v)   //zfm
{
    double normcutoff = 1e7;
    double p = 0;
	assert(v>0);
	
    if (fabs(v-1)<=DOUBLE_ZERO)
        p = 0.5 + atan(x)/PI;
    if(v>=normcutoff)
        p = normcdf(x,0,1);
    if(x != 0 && v != 1 && v > 0 && v < normcutoff)
	{
		double xx = v/ (v + x*x);
		p = betainc(xx, v/2.0, 0.5)/2.0;
	}
    if(x > 0 && v != 1 && v > 0 && v < normcutoff)
        p = 1 - p;
    if(fabs(x)<DOUBLE_ZERO && v > 1)
		p = 0.5;
    return p;
}

double tcdf(double x, int v)
{
	int sign, ibi, i;
	double tt, xx, p, pp, u, ga1, ga2;
	if(v<=MAX_DEGREE)
	{
		if(fabs(x)<DOUBLE_ZERO)
		{
			ga1 = lgam1(v/2.0);
			ga2 = lgam1(v/2.0+0.5);
			pp = 0.5;
			return pp;
		}
		if(x <0.0)
			sign =-1;
		else
			sign = 1;
		tt = x*x;
		xx = tt/(v+tt);
		if(v%2 == 0)
		{
			p = sqrt(xx);
			u = p*(1-xx)/2;
			ibi = 2;
		}
		else
		{
			u = sqrt(xx*(1-xx))/PI;
			p = 1 - 2*atan(sqrt((1-xx)/xx))/PI;
			ibi = 1;
		}
		if(ibi == v)
		{
			pp = 0.5 + sign*p/2;
			return pp;
		}
		for(i = ibi; i<=v-2; i+=2)
		{
			p = p+2*u/i;
			u = u*(1+i)/i*(1-xx);
		}
		pp = 0.5 + sign*p/2;
	}
	else
	{
		pp = tcdf1(x,v);
	}
	if(pp>1)
		pp=1;
	if(pp<0)
		pp=0;
	return pp;
}

double tinv(double p, int v)
{
	int c, k;
	double tt, dfr2, q2, yq, e, q, ga1, ga2, ga3, t0, pp, d, PIs;
    PIs = sqrt(PI);
    dfr2 = v/2.0;
	q = 1 - p;
	if( v == 1)
	{
		tt = tan(PI*(0.5 - q));
		return tt;
	}
    if (v == 2)
	{
		if( q > 0.5)
			c = -1;
		else
			c = 1;
		q2 = (1-2*q);
		q2 =q2*q2;
		tt = sqrt(2*q2/(1-q2))*c;
		return tt;
	}
	yq = -norminv(q,0,1);
	e = 1- 1/(4.0*v);
	e = e*e;
	e = e - yq*yq/(2.0*v);
	if( e > 0.5)
		t0 = yq/sqrt(e);
	else
	{
		ga1 = gammaln(dfr2);
		ga2 = gammaln(dfr2 + 0.5);
		ga3 = exp((ga1-ga2)/v);
		t0 = sqrt(v)/pow(PIs*q*v,1.0/v)/ga3;
	}
    for(k = 1; k<=MAX_COUNT; k++)
	{
		pp = tcdf(t0,v);
		d = tpdf(t0,v);
		if(fabs(d)<DOUBLE_ZERO)
		{
			tt = t0;
			return tt;
		}
		tt = t0-(pp-p)/d;
		if(fabs(t0-tt)<EPS_ACCUR*fabs(tt))
		{
			return tt;
		}
        t0 = tt; 
	}
    return tt;
}


double tinv(double p, double v)
{
	int c, k;
	double tt, dfr2, q2, yq, e, q, ga1, ga2, ga3, t0, pp, d, PIs;
    PIs = sqrt(PI);
    dfr2 = v/2.0;
	q = 1 - p;
	if( fabs(v)<= 1+1e-100)
	{
		tt = tan(PI*(0.5 - q));
		return tt;
	}
    if (fabs(v)<=2+1e-100)
	{
		if( q > 0.5)
			c = -1;
		else
			c = 1;
		q2 = (1-2*q);
		q2 =q2*q2;
		tt = sqrt(2*q2/(1-q2))*c;
		return tt;
	}
	yq = -norminv(q,0,1);
	e = 1- 1/(4.0*v);
	e = e*e;
	e = e - yq*yq/(2.0*v);
	if( e > 0.5)
		t0 = yq/sqrt(e);
	else
	{
		ga1 = gammaln(dfr2);
		ga2 = gammaln(dfr2 + 0.5);
		ga3 = exp((ga1-ga2)/v);
		t0 = sqrt(v)/pow(PIs*q*v,1.0/v)/ga3;
	}
    for(k = 1; k<=MAX_COUNT; k++)
	{
		pp = tcdf(t0,v);
		d = tpdf(t0,v);
		if(fabs(d)<DOUBLE_ZERO)
		{
			tt = t0;
			return tt;
		}
		tt = t0-(pp-p)/d;
		if(fabs(t0-tt)<EPS_ACCUR*fabs(tt))
		{
			return tt;
		}
        t0 = tt; 
	}
    return tt;
}



//t 分布返回p(x<t) 自由度为n, dd为对应的概率密度值
double tdist(double t, int n, double *dd)
{
	int sign, ibi, i;
	double tt, x, p, pp, u, ga1, ga2;
	if(fabs(t)<DOUBLE_ZERO)
	{
		ga1 = lgam1(n/2.0);
		ga2 = lgam1(n/2.0+0.5);
		pp = 0.5;
		*dd = ga2/(sqrt(n*PI)*ga1);
		return pp;
	}
    if(t <0.0)
		sign =-1;
	else
		sign = 1;
	tt = t*t;
	x = tt/(n+tt);
	if(n%2 == 0)
	{
		p = sqrt(x);
		u = p*(1-x)/2;
		ibi = 2;
	}
	else
	{
		u = sqrt(x*(1-x))/PI;
		p = 1 - 2*atan(sqrt((1-x)/x))/PI;
		ibi = 1;
	}
    if(ibi == n)
	{
		*dd = u/fabs(t);
		pp = 0.5 + sign*p/2;
		return pp;
	}
	for(i = ibi; i<=n-2; i+=2)
	{
		p = p+2*u/i;
		u = u*(1+i)/i*(1-x);
	}
	*dd = u/fabs(t);
	pp = 0.5 + sign*p/2;
	return pp;
}

//置信度a=q时对应的上分位数t
double ptdist(int n, double q)
{
	int c, k;
	double tt, dfr2, q2, p, yq, e, ga1, ga2, ga3, t0, pp, d, PIs;
    PIs = sqrt(PI);
    dfr2 = n/2.0;
	if( n == 1)
	{
		tt = tan(PI*(0.5 - q));
		return tt;
	}
    if (n == 2)
	{
		if( q > 0.5)
			c = -1;
		else
			c = 1;
		q2 = (1-2*q);
		q2 =q2*q2;
		tt = sqrt(2*q2/(1-q2))*c;
		return tt;
	}
	p = 1-q;
	yq = pnorm(q);
	e = 1- 1/(4.0*n);
	e = e*e;
	e = e - yq*yq/(2.0*n);
	if( e > 0.5)
		t0 = yq/sqrt(e);
	else
	{
		ga1 = gammaln(dfr2);
		ga2 = gammaln(dfr2 + 0.5);
		ga3 = exp((ga1-ga2)/n);
		t0 = sqrt(n)/pow(PIs*q*n,1.0/n)/ga3;
	}
    for(k = 1; k<=MAX_COUNT; k++)
	{
		pp = tdist(t0,n,&d);
		if(fabs(d)<DOUBLE_ZERO)
		{
			tt = t0;
			return tt;
		}
		tt = t0-(pp-p)/d;
		if(fabs(t0-tt)<EPS_ACCUR*fabs(tt))
		{
			return tt;
		}
        t0 = tt; 
	}
    return tt;
}

double fcdf1(double x, int v1, int v2)
{
	double xx;
	double p=0;
    if(x > 0 && !(v1 <= 0 || v2 <= 0))  
	{
        xx = x/(x + v2*1.0/v1);
        p = betainc(xx, v1/2.0, v2/2.0);
	}
	return p;
}

double fcdf(double x, int v1, int v2)
{
	if (x<DOUBLE_ZERO) //zfm
		return 0;//zfm

	int iai, ibi, nn1, nn2, i;
	double  xx, u, lu, p;
	xx = v1*x/(v2+v1*x);
	if(v1<=MAX_DEGREE && v2<=MAX_DEGREE && v1>0 && v2>0)
	{
		if(v1%2 == 0)
		{
			if(v2%2 ==0)
			{
				u = xx*(1-xx);
				p = xx;
				iai = 2;
				ibi = 2;
			}
			else	
			{
				u = xx*sqrt(1-xx)/2;
				p = 1-sqrt(1-xx);
				iai = 2;
				ibi = 1;
			}
		}
		else
		{
			if(v2%2 ==0)
			{
				p = sqrt(xx);
				u = p*(1-xx)/2;
				iai = 1;
				ibi = 2;
			}
			else
			{
				u = sqrt(xx*(1-xx))/PI;
				p = 1 - 2*atan(sqrt((1-xx)/xx))/PI;
				iai = 1;
				ibi = 1;
			}
		}
		nn1 = v1 -2;
		nn2 = v2 -2;
		if(fabs(u)<DOUBLE_ZERO)
		{
			return p;
		}
		else
		{
			lu = log(u);
		}
		if(iai ==v1)
		{
			if( ibi == v2)
			{
				return p;
			}
			else
			{
				for(i = ibi; i<=nn2; i+=2)
				{
					p = p + 2*u/i;
					lu = lu + log(1-xx+(1-xx)*v1/i);
					u = exp(lu);
				}
			}
			return p;
		}
		else
		{
			for(i = iai; i<=nn1; i+=2)
			{
				p = p - 2*u/i;
				lu = lu + log(xx+ibi*xx/i);
				u = exp(lu);
			}
		}
		if( ibi == v2)
		{
			return p;
		}
		else
		{
			for(i = ibi; i<=nn2; i+=2)
			{
				p = p + 2*u/i;
				lu = lu + log(1-xx+(1-xx)*v1/i);
				u = exp(lu);
			}
		}
	}
	else
	{
		p = fcdf1(x,v1,v2);
	}
	if(p>1)
		p=1;
	if(p<0)
		p=0;
	return p;

}

double beta(double z,double w)
{
    return exp(gammaln(z)+gammaln(w)-gammaln(z+w));
}

double fpdf1(double x, int v1, int v2)
{
	double y = 0;
	double temp=beta(v1/2.0,v2/2.0);
	if(x > 0 && v1 > 0 && v2 > 0 && fabs(temp)>DOUBLE_ZERO)
	{
		temp = pow(v1*1.0/ v2 , v1/2.0) * pow(x ,(v1-2)/2.0) / temp;
		y = temp * pow(1 + v1*x/v2 , -(v1 + v2) / 2.0);
	}
	return y;
}

double fpdf(double x, int v1, int v2)
{
	int iai, ibi, nn1, nn2, i;
	double  xx, u, lu, d;
	xx = v1*x/(v2+v1*x);
	if(v1<=MAX_DEGREE && v2<=MAX_DEGREE && v1>0 && v2>0)
	{
		if(v1%2 == 0)
		{
			if(v2%2 ==0)
			{
				u = xx*(1-xx);
				iai = 2;
				ibi = 2;
			}
			else	
			{
				u = xx*sqrt(1-xx)/2;
				iai = 2;
				ibi = 1;
			}
		}
		else
		{
			if(v2%2 ==0)
			{
				u = sqrt(xx)*(1-xx)/2;
				iai = 1;
				ibi = 2;
			}
			else
			{
				u = sqrt(xx*(1-xx))/PI;
				iai = 1;
				ibi = 1;
			}
		}
		nn1 = v1 -2;
		nn2 = v2 -2;
		if(fabs(u)<DOUBLE_ZERO)
		{
			d = u/x;
			return d;
		}
		else
		{
			lu = log(u);
		}
		if(iai ==v1)
		{
			if( ibi == v2)
			{
				d = u/x;
				return d;
			}
			else
			{
				for(i = ibi; i<=nn2; i+=2)
				{
					lu = lu + log(1-xx+(1-xx)*v1/i);
					u = exp(lu);
				}
			}
			d = u/x;
			return d;
		}
		else
		{
			for(i = iai; i<=nn1; i+=2)
			{
				lu = lu + log(xx+ibi*xx/i);
				u = exp(lu);
			}
		}
		if( ibi == v2)
		{
			d = u/x;
			return d;
		}
		else
		{
			for(i = ibi; i<=nn2; i+=2)
			{
				lu = lu + log(1-xx+(1-xx)*v1/i);
				u = exp(lu);
			}
		}
		d = u/x;
	}
	else
	{
		d = fpdf1(x,v1,v2);
	}
	return d;
	
}

double finv(double p, int v1, int v2)
{
	double df12, df22, a, b, a1, b1, yq, e, f0, ff, pp, d, ga1, ga2, ga3;
    int k;
	
	df12 = v1/2;
	df22 = v2/2;
	a = 2/(9*v1);
	a1 = 1 - a;
	b = 2/(9*v2);
	b1 = 1 - b;
	yq = norminv(p, 0, 1);
	e = b1*b1 - b*yq*yq;
	if( e > 0.8) 
	{
		f0 = ((a1*b1 + yq*sqrt(a1*a1*b + a*e))/e);
		f0 = f0*f0*f0;
	}
	else 
	{
        ga1 = gammaln(df12 + df22);
		ga2 = gammaln(df12);
		ga3 = gammaln(df22);
		f0 = 2/v2*(ga1 - ga2 -ga3 + log(2.0) + (df22 -1)*log(v2) - df22*log(v1) - log(p));
        f0 = exp(f0);
	}
	for( k=1; k<=MAX_COUNT; k++)
    {
        pp = fcdf(f0, v1, v2);
		d = fpdf(f0, v1, v2);
		if(fabs(d)<DOUBLE_ZERO)
		{
			ff = f0;
			return ff;
		}
        ff = f0 -(pp - p)/d;
		if( fabs(f0-ff)<EPS_ACCUR*fabs(ff))
            return ff;
		f0 = fabs(ff);
	}
	return ff;
}


//F 分布返回p(x<f) 自由度为n1,n2, d为对应的概率密度值
double fdist(int n1, int n2, double f, double *d)
{
	int iai, ibi, nn1, nn2, i;
	double  x, u, lu, p;
	x = n1*f/(n2+n1*f);
	if(n1%2 == 0)
	{
		if(n2%2 ==0)
		{
			u = x*(1-x);
			p = x;
			iai = 2;
			ibi = 2;
		}
		else	
		{
			u = x*sqrt(1-x)/2;
			p = 1-sqrt(1-x);
			iai = 2;
			ibi = 1;
		}
	}
	else
	{
		if(n2%2 ==0)
		{
            p = sqrt(x);
			u = p*(1-x)/2;
			iai = 1;
			ibi = 2;
		}
		else
		{
			u = sqrt(x*(1-x))/PI;
			p = 1 - 2*atan(sqrt((1-x)/x))/PI;
			iai = 1;
			ibi = 1;
		}
    }
	nn1 = n1 -2;
	nn2 = n2 -2;
	if(fabs(u)<DOUBLE_ZERO)
	{
		*d = u/f;
		return p;
	}
	else
	{
		lu = log(u);
	}
	if(iai ==n1)
	{
		if( ibi == n2)
		{
			*d = u/f;
			return p;
		}
		else
		{
			for(i = ibi; i<=nn2; i+=2)
			{
				p = p + 2*u/i;
				lu = lu + log(1-x+(1-x)*n1/i);
				u = exp(lu);
			}
		}
		*d = u/f;
		return p;
	}
    else
	{
		for(i = iai; i<=nn1; i+=2)
		{
			p = p - 2*u/i;
			lu = lu + log(x+ibi*x/i);
			u = exp(lu);
		}
	}
	if( ibi == n2)
	{
		*d = u/f;
		return p;
	}
	else
	{
		for(i = ibi; i<=nn2; i+=2)
		{
			p = p + 2*u/i;
			lu = lu + log(1-x+(1-x)*n1/i);
			u = exp(lu);
		}
	}
	*d = u/f;
	return p;
}

//置信度a=q时对应的上分位数F
double pfdist(int n1, int n2, double q)
{
	double df12, df22, a, b, a1, b1, p, yq, e, f0, ff, pp, d, ga1, ga2, ga3;
    int k;
	
	df12 = n1/2;
	df22 = n2/2;
	a = 2/(9*n1);
	a1 = 1 - a;
	b = 2/(9*n2);
	b1 = 1 - b;
	p = 1 - q;
	yq = pnorm(q);
	e = b1*b1 - b*yq*yq;
	if( e > 0.8) 
	{
		f0 = ((a1*b1 + yq*sqrt(a1*a1*b + a*e))/e);
		f0 = f0*f0*f0;
	}
	else 
	{
        ga1 = gammaln(df12 + df22);
		ga2 = gammaln(df12);
		ga3 = gammaln(df22);
		f0 = 2/n2*(ga1 - ga2 -ga3 + log(2.0) + (df22 -1)*log(n2) - df22*log(n1) - log(q));
        f0 = exp(f0);
	}
	for( k=1; k<=MAX_COUNT; k++)
    {
        pp = fdist(n1, n2, f0, &d);
		if(fabs(d)<DOUBLE_ZERO)
		{
			ff = f0;
			return ff;
		}
        ff = f0 -(pp - p)/d;
		if( fabs(f0-ff)<EPS_ACCUR*fabs(ff))
            return ff;
		f0 = fabs(ff);
	}
	return ff;
}


void pearson(double *x, int k, int n, double *r)
{
	int i, j,ii;
	double *xx, *xe, *ye, z;
    
	xx = new double[k*n];
	xe = new double[k];
	ye = new double[k];
	
	for( i=0; i<k;i++)
	{
		for(int j=0; j<n; j++)
			xx[j*k+i] = x[i*n+j];
	}
	//求解平均值
	for (j=0; j<k; j++)
	{
		z=0.0;
        for (i=0; i<n; i++)
			z=z+xx[i*k+j]/n;
        xe[j]=z;
	}
		
		//求解相关系数矩阵
	for (i=0; i<=k-1; i++)
		for ( j=0; j<=i; j++)
		{ 
			z=0.0;
			for ( ii=0; ii<=n-1; ii++)
				z=z+(xx[ii*k+i]-xe[i])*(xx[ii*k+j]-xe[i]);
			r[i*k+j]=z;
		}	
			
		//分母
	for (i=0; i<=k-1; i++)
		ye[i]=sqrt(r[i*k+i]);
			
			//求解相关系数矩阵
	for (i=0; i<=k-1; i++)
		for ( j=0; j<=i; j++)
		{
			r[i*k+j]=r[i*k+j]/(ye[i]*ye[j]);
			r[j*k+i]=r[i*k+j];
		}
	delete []xx;
	delete []xe;
	delete []ye;
	return;
}

double partf(double r1, double r2, double r3)
{
	return (r1 - r2*r3)/sqrt((1-r2*r2)*(1-r3*r3));
}

double partial(double *r, int *pos, int count, int n)
{
	double r1, r2, r3, result;
	int *poscp = new int[count];
	assert(count >2);
	//if (count<3)
	//{
	//	AfxMessageBox("小于三个元素，不能算偏相关!");
	//	return 0;
	//}
    memcpy(poscp, pos, sizeof(int)*count);

    if(count == 3)
	{
		result = partf(r[poscp[0]*n+poscp[1]], r[poscp[0]*n+poscp[2]], r[poscp[1]*n+poscp[2]]);
		delete []poscp;
		return result;
    }	 	
	r1 = partial(r,poscp,count-1, n);
	poscp[1] = pos[count-1];
    r2 = partial(r,poscp,count-1, n);
	poscp[0] = pos[1];
	r3 = partial(r,poscp,count-1, n);
	delete []poscp;
	return partf(r1, r2, r3);
}


long factorial(int n)
{
	if(n==1 || n==0)
		return 1;
	else
		return factorial(n-1)*n;
}

long nchoosek(int n, int k)
{
	if (k==0)
		return 1;
	else if(k==1)
		return n;
	else
		return nchoosek(n-1, k-1)*n/k;
}


double stdrcdf(double q,int v,int r,int jmax, int kmax)
{
	int vmax=120,a,b;
	double step=0.45,xx=0.0,g,c,v2,h,x,j,hj,ehj;

	double **qww, **gkk, **d, **w00, **vww, **pzz;
	double  *w0 , *vw, *qw, *gk, *pz;
    
	if(q<=0)
		return 0;
	g=step/pow(r,0.2);
	if (v>vmax)
	{
		c=log(r*g/sqrt(2*PI));
	}
	else
	{
		h=step/sqrt(v);
		v2=v*0.5;
		c = sqrt(2/PI) * exp(-v2) * pow(v2,v2) / lgam1(v2);
		c = log(c* r* g * h);
		qw = new double [2*jmax+1];
		vw = new double [2*jmax+1];
		for (a=0;a<=2*jmax;a++)
		{      

			j= a-15;
			hj = h*j;
			ehj = exp(hj);
			qw[a]=q*ehj;
			vw[a]=v*(hj+0.5*(1-ehj*ehj));
		}
	}
	gk = new double [2*jmax+1];
	w0 = new double [2*jmax+1];
	pz = new double [2*jmax+1];
	for (a=0;a<=2*jmax;a++)
	{      

		gk[a]=g*(a-15);
		gk[a]= (0.5 * log(r))+gk[a]; 
		w0[a] = c - 0.5 * gk[a]*gk[a];
		pz[a]=normcdf(-gk[a],0,1);
	}
	if (v>vmax)
	{
		for (a=0;a<=2*jmax;a++)
		{
			x=normcdf(q-gk[a],0,1)-pz[a];
			xx=xx+pow(x,(r-1)) *exp(w0[a]);
		}
	}
	else
	{        
		qww = new double *[2*jmax+1];
		for (a=0; a<2*jmax+1; a++)
			qww[a]= new double [2*kmax+1];
		gkk = new double *[2*jmax+1];
		for (a=0; a<2*jmax+1; a++)
			gkk[a]= new double [2*kmax+1];
		pzz = new double *[2*jmax+1];
		for (a=0; a<2*jmax+1; a++)
			pzz[a]= new double [2*kmax+1];
		w00 = new double *[2*jmax+1];
		for (a=0; a<2*jmax+1; a++)
			w00[a]= new double [2*kmax+1];
		vww = new double *[2*jmax+1];
		for (a=0; a<2*jmax+1; a++)
			vww[a]= new double [2*kmax+1];
		d = new double *[2*jmax+1];
		for (a=0; a<2*jmax+1; a++)
			d[a]= new double [2*kmax+1];

		for (a=0;a<=2*jmax;a++)
		{
			for (b=0;b<=2*kmax;b++)
			{ 
				qww[a][b]=qw[a];
		        gkk[b][a]=gk[a];
				pzz[b][a]=pz[a];
			    w00[b][a]=w0[a];
     			vww[a][b]=vw[a];            
			}
		}
		delete []qw;
		delete []vw;
		for(a=0; a<=2*jmax;a++)
			for(b=0; b<=2*kmax;b++)
				d[a][b] =normcdf(qww[a][b]-gkk[a][b],0,1) - pzz[a][b];
		xx=0.0;
		for (a=0;a<=2*jmax;a++)
			for(b=0;b<=2*jmax;b++)
				xx=xx+exp(w00[a][b]+vww[a][b]) *pow(d[a][b],r-1);  
		for (a=0; a<2*jmax+1; a++)
			delete qww[a];
		delete []qww;
		for (a=0; a<2*jmax+1; a++)
			delete gkk[a];
		delete []gkk;
		for (a=0; a<2*jmax+1; a++)
			delete pzz[a];
		delete []pzz;
		for (a=0; a<2*jmax+1; a++)
			delete w00[a];
		delete []w00;
		for (a=0; a<2*jmax+1; a++)
			delete vww[a];
		delete []vww;
		for (a=0; a<2*jmax+1; a++)
			delete d[a];
		delete []d;		
	}
	delete []w0;
	delete []pz;
	delete []gk;
	return xx;
}

	

	

double stdrinv(double p, int v, int r)
{
    int jmax = 20;
    double pcut = 0.00001;
    double tiny = 0.000001;
	double xx, q1, p1, p0, p2, q2, e1, e2, d;
	int j;

	if(p<=0 || p>1)
		return 0;
    q1 = qtrng0(p, v, r);
    p1 = stdrcdf(q1, v, r);
    xx = q1;
	p0=p;
   if (fabs(p1-p0) >= pcut*p0)
   {
	   if (p1 > p0)
		   p2 = max(.75*p0, p0-.75*(p1-p0));   
	   if (p1 < p0)
		   p2 = p0 + (p0 - p1) * (1 - p0) / (1 - p1) * 0.75;  
   }
   q2 = qtrng0(p2, v, r);
   for (j=1; j<jmax; j++)
   {
      p2 = stdrcdf(q2, v, r);
      e1 = p1 - p0;
      e2 = p2 - p0;
      d = e2 - e1;
      xx = (q1 + q2) / 2;
      if (fabs(d) > tiny*p0)
         xx = (e2 * q1 - e1 * q2) / d;
      if (fabs(e1) >= fabs(e2))
         q1 = q2;
         p1 = p2;
      if (fabs(p1 - p0) < pcut*p0)
		  break; 
	   q2 = xx;
   }   
    return xx;
}



double qtrng0(double p, int v, int r)
{
	double q,t;
	t=norminv(0.5 + 0.5 * p, 0, 1);
	if (v < 120)
		t = t + 0.25 *(t*t*t + t) / v; 	
	q = 0.8843 - 0.2368 * t;
	if (v < 120)
		q = q - (1.214/v) + (1.208*t/v); 
	return  t * (q * log(r-1) + 1.4142);
}


//ytli Add @2002/09/23
//ytli Modify @2003/01/03

void lamtax(double *lamta, int n, int k, double r, int flag)
{
	int i;
	if(flag==0)
	{
		for(i=0; i<n-k; i++)
		{
		    lamta[i] = 2-2*cos((i+1)*PI/n)-r;
		}
	}
	else
	{
		for(i=0; i<n-k; i++)
		{
		    lamta[i] = 2-2*cos((i+k)*PI/n)-r;
		}
	}	
	return;
}

double bfun(int n, int k, double *lamta)
{
	double result=0, s, low, high, mid;

	low=.5/lamta[0];
	high=.5/lamta[n-k-1];
	low = low +0.001;
	high = high -0.001;
	while(fabs(low-high)>1e-10)
	{
		mid = (low+high)/2;
		if(sfun(low,n,k,lamta)*sfun(mid,n,k,lamta)<0)
			high = mid;
		else
			low = mid;
    }
	s =( low+high)/2;
	return s;
}

double sfun(double s, int n, int k, double *lamta)
{
	int i;
	double  result=0;	
	for (i=0;i<=n-k-1;i++)
		result = result +lamta[i]/(1-2*lamta[i]*s);
	return result;
}



double pfun(int n, int k, double r, double s, double *lamta)
{
	int i;
	double p=0,K0=0.0,K1=0.0,K2=0.0,K3 = 0.0,W,U;
	double *theta =new double [n-k];
	for(i=0;i<n-k;i++)
	{
		theta[i] = 1/(1-2*s*lamta[i]);
	}
	
	for (i=0;i<n-k;i++)
		K0 = K0+1/2.0*log(theta[i]);
	
	for (i=0;i<n-k;i++)
		K1 = K1 +lamta[i]*theta[i];
	
	for (i=0;i<n-k;i++)
		K2 = K2 +2*lamta[i]*lamta[i]*theta[i]*theta[i];
	
	for (i=0;i<n-k;i++)
		K3 = K3 +8*lamta[i]*lamta[i]*lamta[i]*theta[i]*theta[i]*theta[i];
	
	if(s>0)
	    W=sqrt(2*(-K0));
	else
	    W=-sqrt(2*(-K0));
	U=s*sqrt(K2);
	p = normcdf(W,0,1)+normpdf(W,0,1)*(1/W-1/U);
	if(p<0 || p>1)
	{
        K2=0;
        K3=0;
        for(i=0; i<n-k; i++)
            K2 = K2+2*lamta[i]*lamta[i];
        for(i=0; i<n-k; i++)
            K3 = K3+8*lamta[i]*lamta[i]*lamta[i];
        p=0.5+K3/(6*sqrt(2*PI)*pow(K2, 1.5));
	}
	delete []theta;
	return p;
}

double pdfun(int n, int k, double r, double s, double *lamta)
{
	int i;
	double d=1.0, ls, U=0;
	d = d*(n-k);
	for(i=0; i<n-k; i++)
	{
		ls = 1-2*lamta[i]*s;
		d = d/sqrt(ls);
		U = U+lamta[i]*lamta[i]/(ls*ls);
	}
	d = d/sqrt(4*PI*U);
	return d;
}

double invfun(int n, int k,double p, int flag)
{
	int i;
	double s,r, r0, p0,d0;
	double *lamta= new double[n-k];
	if(flag==0)
	    r0 = 2-2*cos((n-k)/2.0*PI/n);
	else
		r0 = 2-2*cos((n+k)/2.0*PI/n);
    for(i = 1; i<=MAX_COUNT; i++)
	{
		lamtax(lamta,n,k,r0,flag);
	    s=bfun(n,k,lamta);
	    p0 = pfun(n,k,r0,s,lamta);
	    d0 = pdfun(n,k,r0,s,lamta);
		if( fabs(d0)<DOUBLE_ZERO)
		{
			r = r0;
			delete lamta;
			return r;
		}
		r = r0 - (p0 - p)/d0;
		if( fabs(r - r0)<EPS_ACCUR*fabs(r))
		{
			delete lamta;
			return r;
		}
		r0 = r;
	}
	return 0;
}

double dwlcdf(int n, int k, double r)
{
	double *lamta,s;
	lamta=new double [n-k];
	lamtax(lamta,n,k,r,0);
	s=bfun(n,k,lamta);
	double p = pfun(n,k,r,s,lamta);
	delete lamta;
	return p;
}

double dwhcdf(int n, int k, double r)
{
	double *lamta,s;
	lamta=new double [n-k];
	lamtax(lamta,n,k,r,1);
	s=bfun(n,k,lamta);
	double p = pfun(n,k,r,s,lamta);
	delete lamta;
	return p;
}

double dwlpdf(int n, int k, double r)
{
	double *lamta,s;
	lamta=new double [n-k];
	lamtax(lamta,n,k,r,0);
	s=bfun(n,k,lamta);
	double d = pdfun(n,k,r,s,lamta);
	delete lamta;
	return d;
}

double dwhpdf(int n, int k, double r)
{
	double *lamta,s;
	lamta=new double [n-k];
	lamtax(lamta,n,k,r,1);
	s=bfun(n,k,lamta);
	double d = pdfun(n,k,r,s,lamta);
	delete lamta;
	return d;
}

double dwhinv(int n, int k, double p)
{
	return invfun(n, k, p, 1); 
}

double dwlinv(int n, int k, double p)
{
	return invfun(n, k, p, 0); 
}


double betacdf(double x, double a, double b)
{
	double p = 0;	
	if(a<=0 || b<=0)
		p=0;		
	if(x >= 1)
		p = 1;	
	if(x > 0 && x < 1 && a > 0 && b > 0)
		p = betainc(x,a,b);	
	if(p > 1)
		p = 1;
	return p;
}

double gamcdf(double x, double a, double b)
{
    double p = 0;	
	if(x > 0 && !(a <= 0 || b <= 0))
		p = gammainc(x/ b,a);	
	if(p > 1) 
		p = 1;
	return p;
}

double expcdf(double x, double mu)
{
	double p=0;
	if(mu <= 0)
		p = 0;
	if(mu > 0 && x > 0)
		p = 1 - exp(-x/ mu);
	return p;
}

double geocdf(double x, double p)
{
	double y = 0;
	double xx = floor(x);
	if(xx >= 0 && p > 0 && p <= 1)
		y = 1 - pow(1 - p, xx + 1);
	if(p == 1 && x != 0)
		y = 0;
	if(p <= 0 || p > 1 || xx < 0)
		y = 0;
	return y;
}

double hygecdf(int x, int m, int k, int n)
{
    double p=0;
    if(m < 0 || k < 0 || n < 0 || x < 0 || n > m || k > m || x > n || x > k)
	{
		return p;
	}
	for(int i=0; i<=x; i++)
		p += hygepdf(i,m,k,n);
	return p;
}

double poisscdf(int x, double lamda)
{
  assert(lamda>=0);
  if(x<0)
	  return 0;
  if(lamda==0)
	  return 0;
    
  double sum=0;
  for(int L=0;L<=(long)(x);L++)
  {
	  double subSum=-1.0*lamda+L*log(lamda);
	  for(int i=1;i<=L;i++)
	  {
		  subSum-=log(i);
	  }
	  sum+=exp(subSum);
  }
  return sum;
}

double logncdf(double x, double mu, double sigma)
{
	double p = 0;	
	if(sigma > 0 && x > 0)
		p = 0.5 * (1 + erf((log(x) - mu) / (sigma * sqrt(2))));
	if(p>1)
		p=1;
	return p;
}

double weibcdf(double x, double a, double b)
{
	double p = 0;			
	if(x >= 0 && a > 0 && b > 0)
		p = 1 - exp(-a * pow(x, b));
	return p;
}

double betapdf(double x, double a, double b)
{
	double y = 0;		
	if(!(a <= 0 || b <= 0 || x <= 0 || x >= 1))
		y = exp((a - 1)*log(x) + (b - 1)*log((1 - x)) - gammaln(a)-gammaln(b)+gammaln(a+b));
	return y;
}

double gampdf(double x, double a, double b)
{
	double y = 0;
	if(x > 0 && !(a <= 0 || b <= 0))		
		y= exp((a - 1) * log(x) - (x / b) - gammaln(a) - a * log(b));	
	if(x == 0 && a == 1)
		y = (1/b);
	return y;
}

double exppdf(double x, double mu)
{
	double y=0;
	if(mu > 0 && x >= 0)
		y = exp(-x / mu) / mu;
	return y;
}

double geopdf(double x, double p)
{
	double y = 0;
	if(x >= 0 && p > 0 && p <= 1)
		y = p* pow(1 - p, x);
	return y;
}

double hygepdf(int x, int m, int k, int n)
{
	double y, kx, mn, mknx;
	y = 0;		
	if(m < 0 || k < 0 || n < 0 || n > m || k > m || x > n || x > k)
		y = 0;		
	kx = gammaln(k+1)-gammaln(x+1)-gammaln(k-x+1);
	mn = gammaln(m+1)-gammaln(n+1)-gammaln(m-n+1);
	mknx = gammaln(m-k+1)-gammaln(n-x+1)-gammaln(m-k-(n-x)+1);                    
	y = exp(kx + mknx - mn);
	
	return y;
}

double poisspdf(int x, double lamda)
{
	double y = 0;
	
	if(x >= 0 && lamda >= 0)
		y = exp(-lamda + x * log(lamda + DBL_MIN*(lamda==0)) - gammaln(x + 1));
	return y;
}

double lognpdf(double x, double mu, double sigma)
{
	double y, xn;
	y = 0;	
	if(sigma > 0 && x > 0)
	{
		xn = (log(x) - mu) / sigma;
		y = exp(-0.5 * xn *xn) / (x * sqrt(2*PI) * sigma);
	}
	return y;	
}

double weibpdf(double x, double a, double b)
{
	double y = 0;
	if(x > 0 && a > 0 && b > 0)
		y = a * b * pow(x ,b - 1) * exp(-a * pow(x ,b));
	if(x == 0 && b == 1)
		y = a * exp(-a * x);
	return y;
}

double betainv(double p, double a, double b)
{
	double xk, crit, h, xnew;
	double x = 0;
	if(p == 0 && a > 0 && b > 0)
		x = 0; 
	if(p==1) 
		x = 1; 
	int count = 0;
	
	if(!(p > 0 && p < 1 && a > 0 && b > 0))
		return x;
	xk = a/ (a + b);
	
	if (xk == 0)
		xk = sqrt(TOLERANT_ERROR);
	if (xk == 1)
		xk = 1 - sqrt(TOLERANT_ERROR);
	
	h = 1;
	crit = sqrt(TOLERANT_ERROR); 
	
	while(fabs(h) > crit * fabs(xk) && fabs(h) > crit && count < MAX_COUNT)
	{                           
		count = count+1;    
		h = (betacdf(xk,a,b) - p) / betapdf(xk,a,b);
		xnew = xk - h;
		if(xnew<=0)
			xnew = xk/10;
		if(xnew>=1)
			xnew = 1 - (1 - xk)/10;
		xk = xnew;  
		x = xk;
	}
	return x;
}

double gaminv(double p, double a, double b)
{
    double mn,v,temp,mu,sigma,xk,h,xnew;
	double x = 0;	
	if(p == 0 && a > 0 && b > 0)
		x = 0; 
	int count = 0;
	if(!(p > 0  &&  p < 1 && a > 0 && b > 0))
		return x;	
	mn = a* b;
	v = mn * b;
	temp = log(v + mn *mn); 
	mu = 2 * log(mn) - 0.5 * temp;
	sigma = -2 * log(mn) + temp;
	xk = exp(norminv(p,mu,sigma));	
	h = 1; 	
	while(h > sqrt(TOLERANT_ERROR)*fabs(xk) && fabs(h) > sqrt(TOLERANT_ERROR) && count < MAX_COUNT)
	{                        
		count = count + 1;
		h = (gamcdf(xk,a,b) - p) / gampdf(xk,a,b);
		xnew = xk - h;
		if(xnew < 0)
		{
			xnew = xk / 10;
			h = xk-xnew;
		}
		xk = xnew;
	}	
	x=xk;
	return x;	
}

double expinv(double p, double mu)
{
	double x=0;
	if(p > 0 && p < 1)
		x = -mu * log(1 - p);
	return x;
}

double geoinv(double y, double p)
{
	double x = 0;
	if(y >= 0 && y < 1 && p > 0 && p < 1)
		x = floor(log(1 - y) / log(1 - p));
	return x;
}

double hygeinv(double p, int m, int k, int n)
{
	double x =0;	
	double cumdist = hygepdf(int(x),m,k,n);
	int count = 0;	
	while (p > cumdist && count < n && count < k)
	{
		count = count + 1;
		if(cumdist < p - TOLERANT_ERROR)
		{
			x = x + 1;
			cumdist = cumdist + hygepdf(count,m,k,n);
		}
	}
	return x;
}

double poissinv(double p, double lamda)
{
	double x = 0;
	double cumdist = poisspdf(0,lamda);
	int count = 0;
	if(lamda > 0 && p >= 0 && p < 1)
	{
		while(cumdist<p)
		{
			count = count + 1;
			x = count;
			cumdist = cumdist + poisspdf(count,lamda);
		}
	}	
	return x;
}

double logninv(double p, double mu, double sigma)
{
	double x = 0;
	if(p > 0  &&  p < 1 && sigma > 0)
		x = exp(norminv(p, mu, sigma));
	return x;
}

double weibinv(double p, double a, double b)
{
	double x = 0;	
	if(a > 0 && b > 0 && p > 0 && p < 1)
		x = pow(-log(1 - p) / a ,1 / b); 
	return x;	
}

double gammainc(double x, double a)
{
	double ap, sum, del, a0, b0, a1, b1, b, fac, n, g, gold, ana, anf;
    double amax = pow(2,20);
	if(a<=amax)
	{
		if(a != 0 && x != 0 && x < a+1)
		{
			ap = a;
			sum = 1/ap;
			del = sum;
			while( fabs(del) >= 100*TOLERANT_ERROR*fabs(sum))
			{
				ap = ap + 1;
				del = x * del / ap;
				sum = sum + del;
			}
			b = sum * exp(-x + a*log(x) - gammaln(a));
		}
		
		else if(a != 0 && x != 0 && x >= a+1)
		{
			a0 = 1;
			a1 = x;
			b0 = 0;
			b1 = a0;
			fac = 1;
			n = 1;
			g = b1;
			gold = b0;
			while (fabs(g-gold) >= 100*TOLERANT_ERROR*fabs(g))
			{
				gold = g;
				ana = n - a;
				a0 = (a1 + a0 *ana) * fac;
				b0 = (b1 + b0 *ana) * fac;
				anf = n*fac;
				a1 = x * a0 + anf * a1;
				b1 = x * b0 + anf * b1;
				fac = 1 / a1;
				g = b1 * fac;
				n = n + 1;
			}
			b = 1 - exp(-x + a*log(x) - gammaln(a)) * g;
		}
		
		else if(x==0)
			b = 0;
		else if(a==0) 
			b = 1;
	}	
	else
	{
		x = MAX(amax-1/3.0 + sqrt(amax/a)*(x-(a-1/3.0)),0);
		b = gammainc(x,amax);
	}
	return b;
}



double betacore(double x, double a, double b)
{
	double qab, qap, qam, am, bm ,y, bz,d,app,ap,bpp,bp,yold;
	int m,k,tem;
	qab = a + b;
	qap = a + 1;
	qam = a - 1;
	am = 1;
	bm = am;
	y = am;
	bz = 1 - qab * x / qap;
	d = 0;
	app = d;
	ap = d;
	bpp = d;
	bp = d;
	yold = d;
	m = 1;
	k = 1;
	while (1)
	{
		tem = 2 * m;
		d = m * (b - m) * x / ((qam + tem) * (a + tem));
		ap = y + d * am;
		bp = bz + d * bm;
		d = -(a + m) * (qab + m) * x / ((a + tem) * (qap + tem));
		app = ap + d * y;
		bpp = bp + d * bz;
		yold = y;
		am = ap / bpp;
		bm = bp / bpp;
		y = app / bpp;
		if (m == 1)
			bz = 1;
		if(fabs(y-yold) < 1000*TOLERANT_ERROR*fabs(y))
			break;
		m = m + 1;
		if (m > 1000)
		{
			y = 0;
			return y;
		}		
	}
	return y;
}

double betainc(double x, double a, double b)
{
	double y,bt,w1,w2,s;
	y = x + a + b; 
	bt = exp(gammaln(a+b)-gammaln(a)-gammaln(b)+ a*log(x+(x==0)) + b*log(1-x+(x==1)));
	if(x < (a+1) / (a+b+2))
	y = bt* betacore(x,a,b)/a;
	if(x >= (a+1) / (a+b+2))
		y = 1 - bt * betacore(1-x,b,a) / b;
	if(y>1)
	{
		w1 = pow(b*x, 1/3.0);
		w2 = pow(a*(1-x), 1/3.0);
		y = 0.5*erfc(-3/sqrt(2)*((1-1/(9*b))*w1-(1-1/(9*a))*w2)/sqrt(w1*w1/b+w2*w2/a));
		if((a+b-1)*(1-x) < 0.8)
		{
			s = 0.5*((a+b-1)*(3-x)-(b-1))*(1-x);
			y = 1-gammainc(s,b);
		}
	}
	if(x==0)
		y=0;
	else if(x==1)
		y=1;
	return y;
}

double erf(double x)
{ 
	double y;
    if (x>=0.0)
      y=lgam2(0.5,x*x);
    else
      y=-lgam2(0.5,x*x);
    return y;
}

double erfc(double x)
{
	return 1-erf(x);
}

double halfnpdf(double x, double theta)
{
	assert( (theta>DOUBLE_ZERO)&&(x>=DOUBLE_ZERO) );
	return 2.0*theta/PI*exp(-x*x*theta*theta/PI);
}

double laplacepdf(double x, double b, double mu)
{
	assert( b>DOUBLE_ZERO);
	return 1.0/(b*2)*exp(-fabs(x-mu)/b);
}

double logistpdf(double x, double m, double b)
{
    assert( b>DOUBLE_ZERO);
	double p= exp((m-x)/b);
	return 1.0/b*p/((1+p)*(1+p));
}

double paretopdf(double x, double a, double b)
{
    //assert( ! ((fabs(x)<=DOUBLE_ZERO)&&( (a+1)!=DOUBLE_ZERO)));
	assert( (a>0)&&(b>0)&&(x>0));
	double m=log(a) +a*log(b) -(a+1)*log(x);
	return exp(m);




	//return a*pow(b,a)/pow(x,a+1);
}

double uniformpdf(double x, double a, double b)
{
	assert(b>a+DOUBLE_ZERO);
	if(x<=b && x>=a)
		return 1.0/(b-a);
	else 
		return 0;
}

double halfncdf(double x, double theta)
{
	return erf(theta*x/sqrt(PI));
}

double laplacecdf(double x, double b, double mu)
{
	assert(b>DOUBLE_ZERO);
	double p=0;
	if(x-mu>DOUBLE_ZERO)
        p=0.5*(1+(1-exp(-(x-mu)/b)));
	else 
	{	if((x<=mu+DOUBLE_ZERO)&&(x>=mu-DOUBLE_ZERO))
			p=0.5;
		else
			p=0.5*(1-(1-exp((x-mu)/b)));
	}
	return p;
}

double logistcdf(double x, double m, double b)
{
    assert(b>DOUBLE_ZERO);
	return 1.0/(1+exp((m-x)/b));
}

double paretocdf(double x, double a, double b)  //变量取值范围不明确
{
    assert(x>DOUBLE_ZERO);
	return 1.0-pow(b/x,a);
}

double uniformcdf(double x, double a, double b)
{
    assert(b>a+DOUBLE_ZERO);
	if (x<=a)
		return 0;
	else if(x>=b)
		return 1;
	else
		return (x-a)/(b-a);	
}
*//*
double halfninv(double p, double theta)
{
	
	assert((theta>DOUBLE_ZERO) && (p<1) &&(p>=DOUBLE_ZERO)) ;
	return sqrt(PI)*norminv((p+1)/2.0,0,1/sqrt(2))/theta;
}

double laplaceinv(double p, double b, double mu)
{
	/*
	assert((p>=DOUBLE_ZERO)&&(p<=1-DOUBLE_ZERO)&&(b>0));
	double x = 0;
	if(p>0.5+DOUBLE_ZERO)
        x = mu - b*log(2*p-1);
	else
	{
		if(fabs(p-0.5)<=DOUBLE_ZERO)
			x=mu;
		else
			x = mu + b*log(1-2*p);
	}
	return x;
	*/
/*
	assert((p>=DOUBLE_ZERO)&&(p<=1-DOUBLE_ZERO)&&(b>0));
	double x=0;

	if (p>0.5+DOUBLE_ZERO)
		x=mu-b*log(2-2*p);
	else
	{
		if(fabs(p-0.5)<=DOUBLE_ZERO)
			x=mu;
		else x=mu+b*log(2*p);
	}
	return x;


}

double logistinv(double p, double m, double b)
{
    assert((p>DOUBLE_ZERO)&&(p<1));
	return m-b*log((1-p)/p);
}

double paretoinv(double p, double a, double b) //变量取值范围不明
{
   assert((fabs(a)>=DOUBLE_ZERO)&&(p>=DOUBLE_ZERO)&&(p<=1-DOUBLE_ZERO) );
	return b/pow(1-p,1.0/a);
}

double uniforminv(double p, double a, double b)
{
	assert((b>a+DOUBLE_ZERO)&&(p>=DOUBLE_ZERO)&&(p<=1));
	return (b-a)*p +a;	
}

double d2cdf(double v)
{
	double sum=0.0;
	double x=0.0;
	int jmax=15;
	double h=0.45;
    for(int j=-jmax; j<=jmax; j++)
	{
		x=normcdf(h*j,0,1);
        sum += (1-pow(x,v)-pow(1-x,v))*h;
	}
	return sum;
}


double* cubicfit(int n, double *x,double *y, double* &cubic, double* &square, double* &one, bool &bYes)//三次样条函数插值 2003-3-20 zfm 
{
	assert(n>2);
	int i=0,count=0; //计算每段斜率相等个数
	bYes=true;
	double *h=new double[n-1];
	double *lamda=new double[n-2];
	double *div1=new double[n-1]; //一阶商差
	double *div2=new double[n-2]; //二阶商差
	double *constant=new double[n]; //常数项
	double *cof=new double[n];  //最终表达式不展开时的系数，展开后化简就是各三次方程的系数
	double *minus=new double[n-2];
	double *U=new double[n-1];
	double *V=new double[n-1];
	double *temp=new double[n-1];
	//CDoubleMatrix Matrix(n-2,n-2,0.0);
	h[0]=x[1]-x[0];
	div1[0]=(y[1]-y[0])/h[0];
	for(i=0; i<n-2; i++)
	{
		h[i+1]=x[i+2]-x[i+1]; //点间距 n-1个
		
		if( fabs( (y[i+1]-y[i])/h[i]-(y[i+2]-y[i+1])/h[i+1])<=1e-100) //先判斜率相等段数，如果为n-2,就说明不能用
		{
			count++;
		}
	}

	if(count==n-2)
	{
		bYes=false;
		delete []constant;
		constant=NULL;
		return constant;
	}

	for(i=0; i<n-2;i++)  
	{
		div1[i+1]=(y[i+2]-y[i+1])/h[i+1];
		lamda[i]=h[i+1]/(h[i+1]+h[i]); //n-2个
		minus[i]=1-lamda[i];
		div2[i]=6*(div1[i+1]-div1[i])/(x[i+2]-x[i]);   //n-2个
	}
	
	//用追赶法计算Mi(i=1,2,...n-2)
	div2[0]-=minus[0]*0;// M[0]=0,所以乘零 (默认两端点是拐点)
	div2[n-2]-=lamda[n-3]*0;// M[n-1=0,所以乘零
	lamda[n-3]=0; minus[0]=0; V[0]=0; temp[0]=0;

	for(i=0; i<n-2; i++)
	{
		U[i]=2-minus[i]*V[i];
		V[i+1]=lamda[i]/U[i];
		temp[i+1]=(div2[i]-minus[i]*temp[i])/U[i];
	}
	cof[0]=0; 
	cof[n-1]=0;
	for(i=0; i<n-2;i++)
	{
		cof[n-i-2]=temp[n-i-2]-V[n-i-2]*cof[n-i-1];	
	}


	for(i=0; i<n-1; i++)
	{
		cubic[i]=(cof[i+1]-cof[i])/(6*h[i]); //三次系数
		square[i]=3*(cof[i]*x[i+1]-cof[i+1]*x[i])/(6*h[i]);//二次系数
		one[i]=(3*cof[i+1]*x[i]*x[i]-3*cof[i]*x[i+1]*x[i+1]+
			cof[i]*h[i]*h[i]-6*y[i]+6*y[i+1]-cof[i+1]*h[i]*h[i] )/(6*h[i]);//一次系数
		constant[i]=(cof[i]*pow(x[i+1],3)-cof[i+1]*pow(x[i],3) 
			+6*y[i]*x[i+1]-cof[i]*h[i]*h[i]*x[i+1]
			-6*x[i]*y[i+1]+cof[i+1]*x[i]*h[i]*h[i])/(6*h[i]); //常数项
	}

	delete[] div1;
	delete[] lamda;
	delete[] div2;
	delete[] cof;
	delete[] U;
	delete[] V;
	delete[] minus;
	delete[] temp;
	delete[] h;
	
	return constant;
	/*
	double aa=1.25;
	double dd=cubic[1]*pow(aa,3)+square[1]*pow(aa,2)+one[1]*pow(aa,1)+constant[1]; //测试
	for(i=0; i<n-2;i++)n  
	{
		Matrix(i)(i)=2;
	}
	for(i=0; i<n-3; i++)
	{
		Matrix(i)(i+1)=lamda[i];
		
		Matrix(i+1)(i)=1-lamda[i+1];
	}

	CDoubleMatrix mat(n-2,1);
	
	for(i=0;i<n-2;i++)
	{
		mat(i)(0)=6*div2[i];
		
	}
	WriteData(Matrix,"Matrix");
	CDoubleVector cof(n);
	cof(0)=0;
	cof(n-1)=0;
	for(i=1;i<=n-2;i++)
	{
		cof(i)=(inv(Matrix)*mat)(i-1)(0);
	}
	WriteData(mat,"mat");
	
	/*
	WriteData(cof,"cof"); //测试
	double aa=3.0;
	double s=cof(1)*pow((x[2]-aa),3)/(6*h[1]) +
			 cof(2)*pow((aa-x[1]),3)/(6*h[1]) +
			 (y[1]-cof(1)*h[1]*h[1]/6)*(x[2]-aa)/h[1]+
			 (y[2]-cof(2)*h[1]*h[1]/6)*(aa-x[1])/h[1];	

	
	
		
	

	double px[101];
	double py[101];
	double px2[100];
	double py2[100];
		
	for(i=0; i<=100; i++)
	{
		px[i]=2+(i)*0.02;
		py[i]=a[1]*pow(px[i],3)+b[1]*pow(px[i],2)+c[1]*pow(px[i],1)+constant[1];
		if(i<100)
		{
			px2[i]=4+(i+1)*0.01;
			py2[i]=a[2]*pow(px2[i],3)+b[2]*pow(px2[i],2)+c[2]*pow(px2[i],1)+constant[2];
	
		}
	}

	WriteData(px,101,"px");
	WriteData(py,101,"py");
	
	WriteData(px2,100,"px");
	WriteData(py2,100,"py");
	/////////////////////////////////////////////////////////////////////////////////////调用方法如下
	double dx[4]={1,2,4,5};
	double dy[4]={1,3,4,2};
	double *pa=new double[4];
	double *pb=new double[4];
	double *pc=new double[4];
	double *constant=new double[4];
	constant=cubicfit(4, dx, dy, pa, pb,pc);//三次样条函数插值 2003-3-20 zfm 
	
	double aa=1.25;
	double dd=a[1]*pow(aa,3)+b[1]*pow(aa,2)+c[1]*pow(aa,1)+constant[1]; //测试
 
	delete []pa;
	delete []pb;
	delete []pc;
	delete []constant;
	///////////////////////////////////////////////////////////////////////////////////////




};	
