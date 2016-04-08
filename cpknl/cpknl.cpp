// cpknl.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"

void Test()
{
	int len = 10000;
	CDoubleVector  vect;
	vect.create(len);
	for (int i=0; i< len; i++)
		vect(i) = i+2;
	CDoubleVector vect1(len);
	CDoubleVector vect2 = vect;
	CDoubleVector vect3(vect);
	CDoubleVector vect4;
	vect1 = vect;
	vect4 = vect;
	
	double dSum =  vect.sum();
	double dSum1 = vect1.sum();
	double dSum2 = sum(vect2);
	double dSum3 = sum(vect3);
	double dSum4 = sum(vect4);
	
}


static CErrorReporter ErrorOut;
CMethod::CMethod()
{

}

CMethod::~CMethod()
{

}

int    jiechen(int param)
{
	int sum=1;
	for (int i=1;i<=param;i++)
		sum *= i;
	return sum;
}

void ErrorHandler(Matvec_errors Errornum, char * file, int line)
{
	if(&ErrorOut != NULL)
	{
		ErrorOut.setfileline(file,line);		
		ErrorOut.Fatal(Errornum);
	}
}

void WarningHandler(Matvec_warnings Warningnum, char * file, int line)
{
	if(&ErrorOut != NULL)
	{
		ErrorOut.setfileline(file,line);
		ErrorOut.Warning(Warningnum);
	}
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

void twoarrayfree(double ** y, int m)
{
	for(int i=0; i<m; i++)
		delete y[i];
	delete y;
}

int Matdet(CMatrix<double> m1, int numrow, double &det)
{
	int i, j, k;
    int lx = 0, f, nxt;
    double PIvot;
	double rac, cond;
	double bignum;
	double temp1, temp2, eva1;
	cond = DBL_MAX;
	f = 1;
	for(i=0; i<=numrow -1; ++i)
	{
		bignum =0;
		for(k=i; k<=numrow -1; ++k)
        {
			eva1 = fabs(m1(k,i));
			if(eva1-bignum>0)
			{
				bignum = eva1;
				lx = k;
			}
		}
        if (bignum+1.0==1.0)
		{ 
			det=0.0; 
			return 0;
		}
		if(i-lx!=0)
		{
			f = -f;
		}
		for(j=0; j<=numrow-1; ++j)
		{
			temp1 = m1(i,j);
			double fTemp = m1(lx,j);
			m1(i,j) = fTemp;
			m1(lx,j) = temp1;
		}
		PIvot = m1(i,i);
		nxt = i + 1;
		for(j = nxt; j<=numrow-1; ++j)
		{
			rac = m1(j,i)/PIvot;
			for(k=i;k<=numrow-1; ++k)
			{
				m1(j,k) = m1(j,k) - rac*m1(i,k);
				if(fabs(m1(j,k))>cond)
				{
					return EMAT_OVERFLOW;
				}
			}
		}
	}
	temp2 = 1;
    for(i=0; i<=numrow-1; ++i)
	{
		temp2 = temp2 * m1(i,i);
		if(fabs(temp2)>cond)
		{
			return EMAT_OVERFLOW;
		}
	}
	det = temp2*(double)f;
	return 0;
}

int spdet(CSparseDbMatrix m, double &det)
{
	int  i, j, k,p;
	int trlen=m.vlen();
	int rownum=m.mcol();
    int lx=0, f=1;
	double PIvot;
	double rac, cond;
	double bignum;
	double temp, eva1;
	cond = DBL_MAX;
	CIntVector rowidx = m.rowchidx();
	for(i=0; i<rownum; ++i)
	{
		bignum =0;
		for(j=i; j<rownum; j++)
		{
			for(k=rowidx(j); k<rowidx(j+1); ++k)
			{        
				if (m(k).ypos()==i)
				{
					eva1 = fabs(m(k).value());
					if(eva1-bignum>0)
					{
						bignum = eva1;
						lx =m(k).xpos();
					}
					break;
				}                                
				else if(m(k).ypos()>i) 
				{
					break;
				}
			}
		}
        if (bignum+1.0==1.0)
		{ 
			det=0.0; 
			return EMAT_SINGULAR;
		}       
		if(i-lx!=0)
		{
			f = -f;
			m.rowswap(lx,i,rowidx);
		}		
		PIvot = m(rowidx(i)).value(); //取出主元素
		
		for(j=i+1; j<rownum; j++) //用于列元素 
		{
			for(k=rowidx(j); k<rowidx(j+1); k++)
			{        
				if (m(k).ypos()==i) //某行可以变换
				{
					CSparseDbMatrix tr1(m,i,i,0,rownum-1), tr2(m,j,j,0,rownum-1);
					rac =m(k).value()/PIvot;
					tr2-=tr1*rac;
					for (p=0; p<tr2.vlen(); p++) //某行变换后是否出现元素值越界
					{
						if (fabs(tr2(p).value())>cond)
						{
							return EMAT_OVERFLOW;
						}
						else tr2(p).xpos()=j;
					}
					
					m.change(tr2,rowidx(j), rowidx(j+1)-1);
					//WriteData(m,"m13");
					
					int cg = tr2.vlen() - (rowidx(j+1)- rowidx(j));
					for(int t =j+1; t<=rownum; t++)
					{
						rowidx(t) += cg;
					}
				}
				else if (m(k).ypos()>i)
					break;
			}
		}
		
	}	
	temp = 1;
    for(i=0; i<rownum; ++i)
	{
		temp = temp * m(rowidx(i)).value();
		if(fabs(temp)>cond)
		{
			return EMAT_OVERFLOW;
		}
	}
	det = temp*(double)f;
	return 0;
}

int Matinv(CDoubleVector a, int n)
{
	double y, w, run_state;
	CDoubleVector c(n), b(n);
	int tmp1, tmp2;
	int i, j, k;
	double cond = DBL_EPSILON;

	for(i=0; i<n; i++)
	{
		c[i] =0;
		tmp2 = i * n;
		for(j=0; j<n; j++)
		{
			c[i] += a[tmp2+j];
		}
		a[tmp2+i] -= 1;
	}
    for(k=0; k<n; k++)
	{
		tmp1 = k*n;
		for(j=0; j<n; j++)
		{
            b[j] = a[tmp1+j];
			a[tmp1+j] = 0;
		}
		a[tmp1+k] = 1;
		w = b[k] + 1;
		if(fabs(w)<=cond)
		{
             run_state = -1;
			 return EMAT_SINGULAR;
		}
		for(i=0; i<n; i++)
		{
			tmp2 = i*n;
			y = a[tmp2+k]/w;
			for(j=0; j<n; j++)
				a[tmp2+j] -=b[j]*y;
		}
	}
	run_state = 0;
	for(j=0; j<n;j++)
	{
		b[j] =0;
		for(i=0; i<n; i++)
			b[j] += a[i*n+j];
		run_state += c[j]*b[j];
	}
	if(fabs(run_state-n)>sqrt(cond))
		return WEMAT_SINGULAR;
	return 0;
}

int Matinv(CMatrix<double> a, int n)
{
	double y, w, run_state;
	CDoubleVector c(n), b(n);
	int i, j, k;
	double chk = DBL_EPSILON;

	for(i=0; i<n; i++)
	{
		c[i] =0;
		for(j=0; j<n; j++)
		{
			c[i] += a(i,j);
		}
		a(i,i) -= 1;
	}
    for(k=0; k<n; k++)
	{
		for(j=0; j<n; j++)
		{
            b[j] = a(k,j);
			a(k,j) = 0;
		}
		a(k,k) = 1;
		w = b[k] + 1;
		if(fabs(w)<=chk)
		{
             run_state = -1;
			 return EMAT_SINGULAR;
		}
		for(i=0; i<n; i++)
		{
			y = a(i,k)/w;
			for(j=0; j<n; j++)
				a(i,j) -=b[j]*y;
		}
	}
	run_state = 0;
	for(j=0; j<n;j++)
	{
		b[j] =0;
		for(i=0; i<n; i++)
			b[j] += a(i,j);
		run_state += c[j]*b[j];
	}
	if(fabs(run_state-n)>sqrt(chk))
		return WEMAT_SINGULAR;
	return 0;
}

int Matinv1(CMatrix<double> invary, int numcol, double &det)
{
	CIntVector PIvlst(numcol*2);
	CIntVector PIvchk(numcol);
	int i,j,k,l;
	int lerow, lecol;
	int l1;
	double PIv;
	double t, leval;
	double temp,condmin,condmax;
	condmin = DBL_EPSILON;
	condmax = DBL_MAX;
	det = 1;
	for(i=0; i<=numcol-1; ++i)
	{
		PIvchk[i] =0;
	}
	for(i=0; i<=numcol-1; ++i)
	{
		leval =0;
		for(j=0; j<=numcol-1; ++j)
		{
			if(!(PIvchk[j]))
			{
				for(k=0; k<=numcol-1; ++k)
				{
					if(!(PIvchk[k]))
					{
						if(fabs(invary(j,k))>leval)
						{
							lerow = j;
							lecol = k;
							leval = fabs(invary(j,k));
						}
					}
				}
			}
		}
		if(leval<=condmin)
		{
			det =0;
			return EMAT_SINGULAR;
		}
		PIvchk[lecol] =1 ;
		PIvlst[i*2] = lerow;
		PIvlst[i*2 +1] = lecol;
		if(lerow != lecol)
		{
			det = -det;
			for(l=0; l<= numcol-1; ++l)
			{
				temp = invary(lerow,l);
				invary(lerow,l)= invary(lecol,l);
				invary(lecol,l)=temp;
			}
		}
		PIv = invary(lecol,lecol);
		det = det*PIv;
		if(fabs(det)>condmax)
		{
			return EMAT_OVERFLOW;
		}
		invary(lecol,lecol) = 1.0;
		for(l=0; l<=numcol-1;++l)
		{
			invary(lecol,l) = invary(lecol,l)/PIv;
		}
		for(l1=0; l1<=numcol-1; ++l1)
		{
			if(l1!=lecol)
			{
				t = invary(l1,lecol);
				invary(l1,lecol) = 0;
				for(l=0; l<=numcol-1; ++l)
				{
					invary(l1,l) = invary(l1,l) -invary(lecol,l)*t;
					if(fabs(invary(l1,l))>condmax)
					{
						return EMAT_OVERFLOW;
					}
				}
			}
		}
	}
	for(i=0; i<= numcol -1; ++i)
	{
        l = numcol -i -1;
		if(PIvlst[l*2] != PIvlst[l*2+1])
		{
			lerow = PIvlst[l*2];
			lecol = PIvlst[l*2+1];
			for(k=0; k<=numcol -1; ++k)
			{
				temp = invary(k,lerow);
				invary(k,lerow) = invary(k,lecol);
				invary(k,lecol) =temp;
			}
		}
	}
	if(fabs(det)<sqrt(condmin))
		return WEMAT_SINGULAR;
	return 0;
}

int spgassinv(CSparseDbMatrix &mat, double &det)
{
	int i,j,k,l,llidx,cg;
	int lerow, lecol;
	int l1;
	double PIv;
	double t, leval;
	double condmin,condmax;
	int numcol = mat.mrow();
	CIntVector PIvlst(numcol*2);
	CIntVector PIvchk(numcol);
	condmin = DBL_EPSILON;
	condmax = DBL_MAX;
	det = 1;
	for(i=0; i<=numcol-1; ++i)
	{
		PIvchk[i] =0;
	}
	CIntVector rowidx = mat.rowchidx();
	for(i=0; i<=numcol-1; ++i)
	{
		leval =0;
		for(j=0; j<=numcol-1; ++j)
		{
			if(!(PIvchk[j]))
			{
				for(k=rowidx(j); k<rowidx(j+1); ++k)
				{
					if(!(PIvchk[mat(k).ypos()]))
					{
						if(fabs(mat(k).value())>leval)
						{
							lerow = j;
							lecol = mat(k).ypos();
							leval = fabs(mat(k).value());
						}
					}
				}
			}
		}
		if(leval<=condmin)
		{
			det =0;
			return EMAT_SINGULAR;
		}
		PIvchk[lecol] =1 ;
		PIvlst[i*2] = lerow;
		PIvlst[i*2 +1] = lecol;
		if(lerow != lecol)
		{
			det = -det;
			mat.rowswap(lerow,lecol,rowidx);
		}
		for(j=rowidx(lecol); j<rowidx(lecol+1); j++)
		{
			if(mat(j).ypos()==lecol)
			{
				PIv = mat(j).value();
				llidx = j; 
				break;
			}
			else if(mat(j).ypos()>lecol)
			{
                llidx=j;
				PIv = 0.0;
				break;
			}
		}
		if(j==rowidx(lecol+1))
		{
			PIv=0.0;
			llidx = j;
		}
		det = det*PIv;
		if(fabs(det)>condmax)
		{
			return EMAT_OVERFLOW;
		}
        if(PIv +1 !=1)
		{
			mat(llidx).value() = 1.0;
		}
		else
		{
			CTriple<double>tp(lecol,lecol,1.0);
			mat.insert(llidx,tp);
			//mat.insert(llidx,CTriple<double>(lecol,lecol,1.0));
            for(j=lecol+1; j<numcol+1; j++)
				rowidx(j)++;
		}
		for(l=rowidx(lecol); l<rowidx(lecol+1); l++)
		{
			mat(l).value() = mat(l).value()/PIv;
		}
		for(l1=0; l1<=numcol-1; ++l1)
		{
			if(l1!=lecol)
			{
				t = 0.0;
				for(j=rowidx(l1); j<rowidx(l1+1); j++)
				{
					if(mat(j).ypos()==lecol)
					{
						t = mat(j).value();
						mat.remove(j);
						for(k=l1+1; k<numcol+1; k++)
							rowidx(k)--;
						break;
					}
					else if(mat(j).ypos()>lecol)
					{
						break;
					}
				}
				if(t+1 !=1)
				{
                    CSparseDbMatrix tr1(mat,l1,l1,0,numcol-1), tr2(mat,lecol,lecol,0,numcol-1);
					tr1 = tr1-tr2*t;
					for(j=0; j<tr1.vlen(); j++)
					{
						tr1(j).xpos() = l1;
					}
					mat.change(tr1,rowidx(l1), rowidx(l1+1)-1);
					cg = tr1.vlen() - (rowidx(l1+1)- rowidx(l1));
					for(k=l1+1; k<numcol+1; k++)
						rowidx(k) += cg;
				}
			}
		}
	}
	for(i=0; i<= numcol -1; ++i)
	{
        l = numcol -i -1;
		if(PIvlst[l*2] != PIvlst[l*2+1])
		{
			lerow = PIvlst[l*2];
			lecol = PIvlst[l*2+1];
			mat = mat.trans();
			mat.rowswap(lerow,lecol);
			mat = mat.trans();
			rowidx = mat.rowchidx();
		}
	}
	if(fabs(det)<sqrt(condmin))
		return WEMAT_SINGULAR;
	return 0;
}

/*
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
*/
void orithes(int n, double tol, CMatrix<double> a, CVector<double> d)
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
			f = a(i-1,m-2);
			d(i-1) = f;
			h = h + f*f;
		}
		if(h<=tol)
		{
			a(m-1,m-2) = 0;
			continue;
		}
		if(f>=0)
			g=-sqrt(h);
		else
			g = sqrt(h);
		h = h-f*g;
		d(m-1)=f-g;
		
		for(j=m; j<=n; j++)
		{
			f=0;
			for(i=l; i>=m; i--)
				f=f+d(i-1)*a(i-1,j-1);
			f = f/h;
			for(i=m;i<=l; i++)
				a(i-1,j-1) = a(i-1,j-1) - f*d(i-1);
		}
		for(i=1; i<=l;i++)
		{
			f=0;
			for(j=l;j>=m;j--)
				f=f+d(j-1)*a(i-1,j-1);
			f = f/h;
			for(j=m;j<=l;j++)
				a(i-1,j-1) = a(i-1,j-1)-f*d(j-1);
		}
		a(m-1,m-2) = g;
	}
}

/*
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
*/
void ortrans(int n, CMatrix<double> h, CVector<double> d, CMatrix<double> v)
{
	int i, j, k, m, low, upp;
	double x, y;
	low = 1;
	upp = n;
	for(i=1;i<=n;i++)
	{
		for(j=1;j<=n;j++)
			v(i-1,j-1) = 0;
		v(i-1,i-1) = 1.0;
	}
	for(k=upp-2;k>=low;k--)
	{
		m = k+1;
		y = h(m-1,k-1);
		if(y==0)
			continue;
		y = y*d(m-1);
		for(i=k+2;i<=upp;i++)
			d(i-1) = h(i-1,k-1);
		for(j=m;j<=upp;j++)
		{
			x=0;
			for(i=m; i<=upp; i++)
				x += d(i-1)*v(i-1,j-1);
			x = x/y;
			for(i=m; i<=upp; i++)
				v(i-1,j-1) += x*d(i-1);
		}
	}
}


void sporithes(double tol, CSparseDbMatrix &a, CSparseDbMatrix &d)
{
	double f, g, h;
	int i, j, k, l, m, la, st,cg;
	int n = a.mrow();
	k = 1;
	l = n;
	la = l - 1;
	CSparseDbMatrix b;
	CIntVector bcidx,aridx;

	for(m=k+1; m<=la; m++)
	{
		b=a.trans();
		bcidx = b.rowchidx();
		CSparseDbMatrix dd(b,m-2,m-2,m-1,n-1);
		h = dot(dd,dd);
		f = b(m-2,m-1,bcidx);

		if(h<=tol)
		{
			b.set(m-2,m-1,bcidx,0);
            a=b.trans();
			continue;
		}
		if(f>=0)
			g=-sqrt(h);
		else
			g = sqrt(h);
		h = h-f*g;
		dd.set(0,0) = f-g;
		
		for(j=m; j<=n; j++)
		{
			CSparseDbMatrix tr1(b,j-1,j-1,m-1,n-1);
			f = dot(tr1, dd);
			f = f/h;
			if(f+1!=1)
			{
				st=0;
				if(bcidx(j-1)!=bcidx(j))
				{
				    for(i=bcidx(j-1); i<bcidx(j); i++)
					{
						if(b(i).ypos()>=m-1)
						{
							st = i;
							break;
						}
					}
				}
			    else
				    st=bcidx(j-1);

				tr1 = tr1 - f*dd;
				for(i=0;i<tr1.vlen(); i++)
				{
					tr1(i).xpos()=j-1;
					tr1(i).ypos()=tr1(i).ypos()+m-1;
				}
				b.change(tr1,st,bcidx(j)-1);
				cg = tr1.vlen()-(bcidx(j)-st);
				for(i=j; i<n+1; i++)
					bcidx(i)+=cg;
			}
		}
		a = b.trans();
		aridx = a.rowchidx();
		for(i=1; i<=n;i++)
		{
			CSparseDbMatrix tr1(a,i-1,i-1,m-1,n-1);
            f = dot(tr1, dd);
			f = f/h;
			if(f+1!=1)
			{
				st=0;
				if(aridx(i-1)!=aridx(i))
				{
				    for(j=aridx(i-1); j<aridx(i); j++)
					{
						if(a(j).ypos()>=m-1)
						{
							st = j;
							break;
						}
					}
				}
			    else
				    st=aridx(i-1);

				tr1 = tr1 - f*dd;
				for(j=0;j<tr1.vlen(); j++)
				{
					tr1(j).xpos()=i-1;
					tr1(j).ypos()=tr1(j).ypos()+m-1;
				}
				a.change(tr1,st,aridx(i)-1);
				cg = tr1.vlen()-(aridx(i)-st);
				for(j=i; j<n+1; j++)
					aridx(j)+=cg;
			}
		}
		st=0;
		for(i=0; i<d.vlen(); i++)
		{
			if(d(i).ypos()>=m-1)
			{
				st = i;
				break;
			}
		}
		d.change(dd,st,d.vlen()-1);
		
		for(i=aridx(m-1); i<aridx(m); i++)
		{
			if(a(i).ypos()==m-2)
			{
				if(g+1!=1)
				    a(i).value()=g;
				else
					a.remove(i);
				break;
			}
			else if(a(i).ypos()>m-2)
			{
				if(g+1!=1)
				{
					CTriple<double>tp(m-1,m-2,g);
					a.insert(i,tp);
					//a.insert(i,CTriple<double>(m-1,m-2,g));
				}
					
				break;
			}                
		}
	}
}

void sportrans(CSparseDbMatrix &h, CSparseDbMatrix &d, CSparseDbMatrix &v)
{
	int i, j, k, m, low, upp,st,cg;
	double x, y, tmp;
	int n=h.mrow();
	low = 1;
	upp = n;

	CSparseDbMatrix ht = h.trans();
	CIntVector htridx=ht.rowchidx();
	CIntVector vridx(n+1);
	for(i=0; i<=n; i++)
        vridx(i)=i;
	
	for(k=upp-2;k>=low;k--)
	{
		m = k+1;
		y=0.0;
		for(j=htridx(k-1); j<htridx(k); j++)
		{
			if(h(j).ypos()==m-1)
			{
				y=h(j).value();
				break;
			}
			else if(h(j).ypos()>m-1)
				break;
		}
		if(y+1==1)
			continue;

		tmp = d(1,m-1);
		y = y*tmp;
        
		CSparseDbMatrix dd(h,k-1,k-1,m-1,upp-1);
		dd.set(1,0,tmp);

		for(j=m;j<=upp;j++)
		{
			CSparseDbMatrix tr1(v,j-1,j-1,m-1,upp-1);
			x = dot(tr1,dd);
			x = x/y;
			if(x+1!=1)
			{
				st=0;
				if(vridx(j-1)!=vridx(j))
				{
					for(i=vridx(j-1); i<vridx(j); i++)
					{
						if(v(i).ypos()>=m-1)
						{
							st = i;
							break;
						}
					}
				}
				else
					st=vridx(j-1);
				
				tr1 = tr1 + x*dd;  
				for(i=0;i<tr1.vlen(); i++)
				{
					tr1(i).xpos()=j-1;
					tr1(i).ypos()=tr1(i).ypos()+m-1;
				}
				v.change(tr1,st,vridx(j)-1);
				cg = tr1.vlen()-(vridx(j)-st);
				for(i=j; i<n+1; i++)
					vridx(i)+=cg;
			}
		}
	}
	v = v.trans();
}

void ppp(CMatrix<double> a, CDoubleVector e,CDoubleVector s, CDoubleMatrix v, int m, int n)
{
	int i, j;
	double d;
	if(m>=n)
		i=n;
	else
		i=m;
	for(j=1;j<=i-1;j++)
	{
		a(j-1,j-1) = s[j-1];
		a(j-1,j) = e[j-1];
	}
	a(i-1,i-1) = s[i-1];
	if(m<n)
		a(i-1,i)=e[i-1];
	for(i=1; i<=n-1; i++)
		for(j=i+1; j<=n; j++)
		{
			d=v(i-1,j-1);
			v(i-1,j-1) = v(j-1,i-1);
			v(j-1,i-1) = d;
		}
	return;
}

void sss(CDoubleVector fg, CDoubleVector cs)
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

int dluav(CMatrix<double> a, CDoubleMatrix u, CDoubleMatrix v, int m, int n, double eps)
{
	int i,j,k,l,ll,kk,ix,iy,mm,nn,iz,m1,ks,it,ka;
	int ix1,iy1,iz1;
	double d, dd, t, sm,sm1,em1,sk,ek,b,c,shh;
	CDoubleVector fg(2);
	CDoubleVector cs(2);

	ka = m+1;
	if(n>m)
		ka = n+1;
	CDoubleVector s(ka);
	CDoubleVector e(ka);
	CDoubleVector w(ka);

	it = MAX_ITERATETIME;
	k=n;
	if(m-1<n)
		k = m-1;
	l=m;
	if(n-2<m)
		l = n-2;
	if(l<0)
		l =0;
	ll = k;
	if(l>k)
		ll=l;
	if(ll>=1)
	{
        for(kk=1;kk<=ll;kk++)
		{
			if(kk<=k)
			{
				d=0.0;
				for(i=kk;i<=m;i++)
				{
					ix=i-1;
					ix1=kk-1;
					d = d +a(ix,ix1)*a(ix,ix1);
				}
				s[kk-1] = sqrt(d);
				if(s[kk-1]!=0.0)
				{
					ix = kk-1; ix1=kk-1;
					if(a(ix,ix1) != 0.0)
					{
						s[kk-1] = fabs(s[kk-1]);
						if(a(ix,ix1)<0.0)
							s[kk-1] = -s[kk-1];
					}
					for(i=kk; i<=m; i++)
					{
						a(i-1,kk-1)/=s[kk-1];
					}
					a(ix,ix1) += 1.0;
				}
				s[kk-1]= -s[kk-1];
			}
			if(n>=kk+1)
			{
				for(j=kk+1; j<=n; j++)
				{
				    if((kk<=k)&&(s[kk-1] != 0))
					{
					    d=0.0;
						for(i=kk; i<=m; i++)
						{
						    d = d+a(i-1,kk-1)*a(i-1,j-1);
						}
						d = -d/a(kk-1,kk-1);
						for(i=kk; i<=m; i++)
						{
						    a(i-1,j-1) += d*a(i-1,kk-1);
						}
					}
					e[j-1] = a(kk-1,j-1);
				}
			}
			if(kk<=k)
			{
			    for(i=kk; i<=m; i++)
				{
				    u(i-1,kk-1) = a(i-1,kk-1);
				}
			}
			if(kk<=l)
			{
			    d =0.0;
				for(i=kk+1; i<=n; i++)
				    d = d+ e[i-1]*e[i-1];
				e[kk-1] = sqrt(d);
				if(e[kk-1] != 0.0)
				{
				    if(e[kk] != 0.0)
					{
					    e[kk-1] = fabs(e[kk-1]);
						if(e[kk]<0.0)
							e[kk-1] = -e[kk-1];
					}
					for(i= kk+1; i<=n; i++)
						e[i-1] = e[i-1]/e[kk-1];
					e[kk] = 1 +e[kk];
				}
				e[kk-1] = -e[kk-1];
				if((kk+1)<=m && (e[kk-1]!= 0.0))
				{
					for(i=kk+1; i<=m; i++)
						w[i-1] = 0.0;
					for(j=kk+1; j<=n; j++)
						for(i=kk+1; i<=m; i++)
							w[i-1] = w[i-1] + e[j-1]*a(i-1,j-1);
					for(j=kk+1; j<=n; j++)
						for(i=kk+1; i<=m; i++)
							a(i-1,j-1) -= w[i-1]*e[j-1]/e[kk];
				}
				for(i=kk+1; i<=n; i++)
					v(i-1,kk-1) = e[i-1];
			}
		}
	}
	mm = n;
	if(m+1<n)
	    mm = m +1;
	if(k<n)
		s[k] = a(k,k);
	if(m<mm)
		s[mm-1] = 0.0;
	if(l+1 <mm)
	    e[l] = a(l,mm-1);
	e[mm - 1] =0;
	nn = m;
	if(m>n)
		nn=m;
	if(nn>=k+1)
	{
		for(j=k+1; j<=nn; j++)
		{
			for(i=1; i<=m; i++)
				u(i-1,j-1) = 0;
			u(j-1,j-1) = 1;
		}
	}
	if( k>=1)
	{
		for(ll=1; ll<=k; ll++)
		{
			kk = k-ll+1;
			iz = kk-1; iz1 = kk-1;
			if(s[kk-1] != 0)
			{
				if(nn>=kk+1)
					for(j=kk+1; j<=nn; j++)
					{
						d = 0.0;
						for(i = kk; i<= m; i++)
						{
							ix = i -1;
							d += u(ix,kk -1)*u(ix,j-1)/u(iz,iz1);
						}
						d = -d; 
						for(i = kk; i<=m; i++)
						{
							ix = i-1;
							u(ix,j-1) += d*u(ix,kk-1);
						}
					}
					for(i=kk; i<=m; i++)
					{
						ix = i-1;
						ix1 = kk -1;
						u(ix,ix1) = -u(ix,ix1);
					}
					u(iz,iz1) = 1+ u(iz,iz1);
					if(kk-1 >=1)
						for(i = 1; i<=kk-1; i++)
							u(i-1,kk-1) = 0;
			}
			else
			{
				for(i=1; i<=m; i++)
					u(i-1,kk-1) =0;
				u(kk-1,kk-1) = 1;
			}
		}
	}
	for(ll = 1; ll<=n; ll++)
	{
		kk = n-ll+1;
		iz = kk; iz1 = kk -1;
		if((kk<=l)&&(e[kk-1]!=0))
		{
			for(j=kk+1; j<=n; j++)
			{
				d = 0;
				for(i =kk+1; i<=n; i++)
				{
					ix = i - 1; 
					d += v(ix,kk-1)*v(ix,j-1)/v(iz,iz1);
				}
				d = -d;
				for(i = kk+1; i<=n; i++)
				{
					ix = i-1;
					v(ix,j-1) += d*v(ix,kk -1);
				}
			}
		}
		for(i=1; i<=n; i++)
			v(i-1,kk-1) = 0;
		v(iz-1,iz1) = 1;
	}
	for(i=1; i<=m; i++)
		for(j=1; j<=n; j++)
			a(i-1,j-1) = 0;
	m1 = mm;
	it = MAX_ITERATETIME;
	while(1==1)
	{
		if(mm == 0)
		{
			ppp(a,e,s,v,m,n);
			return 0;
		}
		if(it == 0)
		{
			ppp(a,e,s,v,m,n);
			return EMAT_OVERITERATE;
		}
		kk = mm -1;
		while((kk!=0)&&(fabs(e[kk-1])!=0))
		{
			d = fabs(s[kk-1]) + fabs(s[kk]);
			dd = fabs(e[kk-1]);
			if(dd > eps*d)
				kk = kk -1;
			else
				e[kk -1] = 0;
		}
		if(kk == mm -1)
		{
			kk = kk+1;
			if(s[kk-1]<0)
			{
				s[kk-1] = -s[kk-1];
				for(i = 1; i<=n; i++)
				{
					ix = i-1; ix1 = kk-1;
					v(ix,ix1) = -v(ix,ix1);
				}
			}
			while((kk != m1) &&(s[kk-1]<s[kk]))
			{
				d = s[kk-1];
				s[kk-1] = s[kk];
				s[kk] = d;
				if(kk<n)
				{
					for(i=1; i<=n; i++)
					{
						ix = i -1;
						ix1 = kk -1;
						iy = i -1;
						iy1 = kk;
						d = v(ix,ix1);
						v(ix,ix1) = v(iy,iy1);
						v(iy,iy1) = d;
					}
				}
				if(kk<m)
				{
					for(i=1; i<=m; i++)
					{
						ix = i-1;
						ix1 = kk -1;
						iy = i-1;
						iy1 = kk;
						d = u(ix,ix1);
						u(ix,ix1) = u(iy,iy1);
						u(iy,iy1) = d;
					}
				}
				kk = kk +1;
			}
			it = MAX_ITERATETIME;
			mm = mm-1;
		}
		else
		{
			ks = mm;
			while((ks>kk)&&(fabs(s[ks-1])!=0))
			{
				d = 0;
				if(ks != mm)
					d = d+fabs(e[ks-1]);
				if(ks != kk+1)
					d = d+fabs(e[ks-2]);
				dd = fabs(s[ks -1]);
                if(dd>eps*d)
					ks = ks -1;
				else
					s[ks-1] = 0;
			}
			if(ks == kk)
			{
				kk =kk+1;
				d = fabs(s[mm -1]);
				t = fabs(s[mm -2]);
				if(t>d)
					d=t;
				t = fabs(e[mm-2]);
				if(t>d)
					d = t;
				t = fabs(s[kk-1]);
				if(t>d)
					d = t;
				t = fabs(e[kk -1]);
				if(t>d)
					d = t;
				sm = s[mm -1]/d;
				sm1 = s[mm-2]/d;
				em1 = e[mm -2]/d;
				sk = s[kk-1]/d;
				ek = e[kk-1]/d;
				b = ((sm1+sm)*(sm1-sm) + em1*em1)/2.0;
				c = sm*em1;
				c = c*c;
				shh =0;
				if((b!=0)||(c!=0))
				{
					shh = sqrt(b*b+c);
					if(b<0)
						shh = -shh;
					shh = c/(b+shh);
				}
				fg[0] = (sk +sm)*(sk-sm) - shh;
				fg[1] = sk * ek;
				for(i = kk; i<= mm-1; i++)
				{
					sss(fg,cs);
					if(i!=kk)
						e[i-2] = fg[0];
					fg[0] = cs[0]*s[i-1] + cs[1]*e[i-1];
					e[i-1] = cs[0]*e[i-1] - cs[1]*s[i-1];
					fg[1] = cs[1]*s[i];
					s[i] = cs[0]*s[i];
					if((cs[0] != 1)||(cs[1] != 0))
						for(j=1; j<=n; j++)
						{
							ix = j-1;
							ix1 = i-1;
							iy = j-1;
							iy1 =i;
							d = cs[0]*v(ix,ix1) +cs[1]*v(iy,iy1);
							v(iy,iy1) = -cs[1]*v(ix,ix1) + cs[0]*v(iy,iy1);
							v(ix,ix1) = d;
						}
					sss(fg,cs);
					s[i-1] = fg[0];
					fg[0] = cs[0]*e[i-1] +cs[1]*s[i];
					s[i] = -cs[1]*e[i-1] + cs[0]*s[i];
					fg[1] = cs[1]*e[i];
					e[i] = cs[0]*e[i];
					if(i<m)
						if((cs[0]!=1)||(cs[1] !=0))
							for(j=1; j<=m; j++)
							{
								ix=j-1;
								ix1 = i-1;
								iy = j-1;
								iy1 = i;
							    d = cs[0]*u(ix,ix1) +cs[1]*u(iy,iy1);
							    u(iy,iy1) = -cs[1]*u(ix,ix1) + cs[0]*u(iy,iy1);
						  	    u(ix,ix1) = d;
							}
				}
				e[mm-2] = fg[0];
				it = it -1;
			}
			else
			{
				if(ks == mm)
				{
					kk = kk +1;
					fg[1] = e[mm -2];
					e[mm-2] = 0;
					for(ll = kk; ll<=mm-1;ll++)
					{
						i=mm+kk-ll-1;
						fg[0]=s[i-1];
						sss(fg,cs);
						s[i-1] = fg[0];
						if(i != kk)
						{
							fg[1]= -cs[1]*e[i-2];
							e[i-2] = cs[0] *e[i-2];
						}
						if((cs[0] !=1)||cs[1] != 0)
							for(j=1; j<=n; j++)
							{
								ix = j-1;
								ix1 = i-1;
								iy = j-1;
								iy1 = mm-1;
								d = cs[0]*v(ix,ix1) +cs[1]*v(iy,iy1);
				                v(iy,iy1) = -cs[1]*v(ix,ix1) + cs[0]*v(iy,iy1);
							    v(ix,ix1) = d;
							}
					}
				}
				else
				{
					kk = ks +1;
					fg[1] = e[kk -2];
					e[kk-2] = 0;
					for(i = kk; i<=mm;i++)
					{
						fg[0]=s[i-1];
						sss(fg,cs);
						s[i-1] = fg[0];
						fg[1]= -cs[1]*e[i-1];
						e[i-1] = cs[0] *e[i-1];

						if((cs[0] !=1)||cs[1] != 0)
							for(j=1; j<=m; j++)
							{
								ix = j-1;
								ix1 = i-1;
								iy = j-1;
								iy1 = kk-2;
								d = cs[0]*u(ix,ix1) +cs[1]*u(iy,iy1);
				                u(iy,iy1) = -cs[1]*u(ix,ix1) + cs[0]*u(iy,iy1);
							    u(ix,ix1) = d;
							}
					}
				}
			}
		}
	}
}

int dginv(CMatrix<double> a, CMatrix<double> aa, int m, int n, double eps)
{
	int i, j, k, l;
	CDoubleMatrix u(m,m);
	CDoubleMatrix v(n,n);
	i = dluav(a,u,v,m,n,eps);
	if(i!=0)
		return i;
	j=n;
	int maxmn = m;
	if(m<n)
	{
		j=m;
		maxmn = n;
	}
	j=j-1;
	k=0;
	while((k<=j)&&(fabs(a(k,k))>((double)maxmn*a(0,0)*eps)))
		k=k+1;
	k=k-1;
	for(i=0; i<=n-1; i++)
	{
		for(j=0; j<=m-1; j++)
		{
			aa(i,j) = 0;
			for(l=0; l<=k; l++)
				aa(i,j) += v(l,i)*u(j,l)/a(l,l);
		}
	}
	return 0;	
}





void cdiv(double xr, double xi, double yr, double yi, double& zr, double& zi)
{
    double h;
	if(fabs(yr)>fabs(yi))
	{
		h = yi/yr;
		yr = h*yi + yr;
		zr = (xr + h*xi)/yr;
		zi = (xi - h*xr)/yr;
	}
	else
	{
		h = yr/yi;
		yi = h*yr + yi;
		zr = (h*xr +xi)/yi;
		zi = (h*xi -xr)/yi;
	}
}
/*
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
*/
int hqr2( int n, double ep, CMatrix<double> h, CMatrix<double> v, CVector<double> wr, CVector<double> wi, CVector<int> cnt)
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
				if(fabs(h(l-1,l-2))<=ep*(fabs(h(l-2,l-2))+fabs(h(l-1,l-1))))
				{
					la =1;
					break;
				}
			}
			if(!la)
				l = low;
			x= h(en-1,en-1);
			if(l == en)
			{
				la = 0;
				break;
			}
			y = h(na-1,na-1);
			w = h(en-1,na-1)*h(na-1,en-1);
			if(l==na)
			{
				la = 1;
				break;
			}
			if(its == MAX_ITERATETIME)
			{
				cnt(en - 1) = MAX_ITERATETIME + 1;
				return EMAT_OVERITERATE;
			}
			divmess = div(its,10);
			if(divmess.rem == 0)
			{
				t += x;
				for(i=low; i<=en; i++)
					h(i-1,i-1) -=x;
				s = fabs(h(en-1,na-1))+fabs(h(na-1,en-3));
				x = y =0.75*s;
				w = -0.4375*s*s;
			}
			its += 1;
			for(m = en-2; m>=1; m--)//????
			{
				z=h(m-1,m-1);
				r=x-z;
				s=y-z;
				if(fabs(h(m,m-1))<ep)
				{
					return EMAT_DIVBYZERO;
				}
				p = (r*s-w)/h(m,m-1) + h(m-1,m);
				q = h(m,m)-z-r-s;
				r = h(m+1,m);
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
				if(fabs(h(m-1,m-2))*(fabs(q)+fabs(r)) <= ep*fabs(p)*
					(fabs(h(m-2,m-2)) + fabs(z) + fabs(h(m,m))))
					break;
			}
			for(i=m+2;i<=en; i++)
				h(i-1,i-3) = 0;
			for(i=m+3;i<=en; i++)
				h(i-1,i-4) = 0;
			for(k=m; k<=na; k++)
			{
				if(k != na)
					nl =1;
				else
					nl =0;
				if(k!=m)
				{
					p = h(k-1,k-2);
					q = h(k,k-2);
					if(nl)
						r = h(k+1,k-2);
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
					h(k-1,k-2) = -s*x;
				else if(l!=m)
					h(k-1,k-2) = -h(k-1,k-2);
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
					p=h(k-1,j-1) + q*h(k,j-1);
					if(nl)
					{
						p +=r*h(k+1,j-1);
						h(k+1,j-1) -= p*z;
					}
					h(k,j-1) -= p*y;
					h(k-1,j-1) -=p*x;
				}
				if(k+3 <en)
					j = k+3;
				else
					j=en;
				for(i=1; i<=j; i++)
				{
					p = x*h(i-1,k-1) +y*h(i-1,k);
					if(nl)
					{
						p += z*h(i-1,k+1);
						h(i-1,k+1) -= p*r;
					}
					h(i-1,k) -=p*q;
					h(i-1,k-1) -=p;
				}
				for(i=low; i<=upp; i++)
				{
					p = x*v(i-1,k-1) + y*v(i-1,k);
					if(nl)
					{
						p += z*v(i-1,k+1);
						v(i-1,k+1) -=p*r;
					}
					v(i-1,k) -= p*q;
					v(i-1,k-1) -= p;
				}
			}
		}
		if(! la)
		{
			wr(en-1)= h(en -1,en -1)= x+t;
			wi(en-1)=0;
			if(fabs(wr(en-1))<DBL_EPSILON)
				wr(en-1) = 0;
			cnt(en -1) =its;
			en = na;
			continue;
		}
		p = (y-x)/2;
		q = p*p +w;
		z = sqrt(fabs(q));
        x = h(en-1,en-1) = x+t;
		h(na-1,na-1) = y+t;
		cnt(en-1) = -its;
		cnt(na-1) = its;
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
			wr(na -1 ) = x+z;
			wr(en -1 ) =s=x-w/z;
			wi(na-1) = wi(en-1)=0;
			if(fabs(wr(en-1))<DBL_EPSILON)
				wr(en-1) =0;
			if(fabs(wr(na-1))<DBL_EPSILON)
				wr(na-1) =0;
			x = h(en-1,na-1);
			r = sqrt(x*x + z*z);
			if(r<ep)
			{
				return EMAT_DIVBYZERO;
			}
			p= x/r;
			q = z/r;
			for(j=na; j<=n; j++)
			{
				z= h(na-1,j-1);
				h(na-1,j-1) = q*z + p*h(en-1,j-1);
				h(en-1,j-1) = q*h(en-1,j-1) - p*z;
			}
			for(i=1; i<=en; i++)
			{
				z= h(i-1,na-1);
				h(i-1,na-1) = q*z + p*h(i-1,en-1);
				h(i-1,en-1) = q*h(i-1,en-1) - p*z;
			}
			for(i=low; i<=upp; i++)
			{
				z = v(i-1,na-1);
				v(i-1,na-1) = q*z + p*v(i-1,en-1);
				v(i-1,en-1) = q*v(i-1,en-1) - p*z;
			}
		}
		else
		{
			wr(na-1) = wr(en-1) = x+p;
			if(fabs(wr(en-1))<DBL_EPSILON)
			{
				wr(en-1)=0;
				wr(na-1)=0;
			}
			wi(na-1)=z;
			wi(en-1)=-z;
			if(fabs(z)<DBL_EPSILON)
			{
				wi(en-1)=0;
				wi(na-1)=0;
			}
		}
		en -=2;
	}
	norm = 0;
	k = 1;
	for(i=1; i<=n; i++)
	{
		for(j=k; j<=n; j++)
			norm += fabs(h(i-1,j-1));
		k=i;
	}
	if(norm<DBL_EPSILON)
		return EMAT_FAILURE;
	for(en=n; en>=1; en--)
	{
		p = wr(en-1);
		q = wi(en-1);
		na = en-1;
		if(fabs(q)<ep)
		{
			m=en;
			h(en-1,en-1) =1;
			for(i=na; i>=1; i--)
			{
				w= h(i-1,i-1)-p;
				r= h(i-1,en-1);
				for(j=m; j<=na; j++)
					r += h(i-1,j-1)*h(j-1,en-1);
				if(wi(i-1)<0)
				{
					z=w;
					s=r;
				}
				else
				{
					m=i;
					if(fabs(wi(i-1))<ep)
					{
						if(fabs(w)>=ep)
							h(i-1,en-1) = -r/w;
						else
							h(i-1,en-1) = -r/(ep*norm);
					}
					else
					{
						x = h(i-1,i);
						y = h(i,i-1);
						q = (wr(i-1) - p)*(wr(i-1) -p) + wi(i-1)*wi(i-1);
						if((q<ep)||(fabs(x)<ep)||(fabs(z)<ep))
						{
							return EMAT_DIVBYZERO;
						}
						h(i-1,en-1) = t = (x*s-z*r)/q;
						if(fabs(x) > fabs(z))
							h(i,en-1) = (-r-w*t)/x;
						else
							h(i,en-1)= (-s-y*t)/z;
					}
				}
			}
		}
		else
		{
			if(q<0)
			{
				m = na;
				if(fabs(h(en-1,na-1))>fabs(h(na-1,en-1)))
				{
					if(fabs(h(en-1,na-1))<ep)
					{
						return EMAT_DIVBYZERO;
					}
					h(na-1,na-1) = -(h(en-1,en-1)-p)/h(en-1,na-1);
					h(na-1,en-1) = -q/h(en-1,na-1);
				}
				else
					cdiv(-h(na-1,en-1),0,h(na-1,na-1)-p,q, h(na-1,na-1), h(na-1,en-1));
				h(en-1,na-1) = 1;
				h(en-1,en-1) = 0;
				for(i=na-1; i>=1; i--)
				{
					w = h(i-1,i-1) -p;
					ra = h(i-1,en-1);
					sa =0;
					for(j=m; j<=na; j++)
					{
						ra += h(i-1,j-1)*h(j-1,na-1);
						sa += h(i-1,j-1)*h(j-1,en-1);
					}
					if(wi(i-1)<0)
					{
						z=w;
						r=ra;
						s=sa;
					}
					else
					{
						m=i;
						if(fabs(wi(i-1))<=ep)
							cdiv(-ra, -sa, w,q,h(i-1,na-1), h(i-1,en-1));
						else
						{
							x=h(i-1,i);
							y = h(i,i-1);
							vr = (wr(i-1)-p)*(wr(i-1)-p) +wi(i-1)*wi(i-1) -q*q;
							vi =(wr(i-1) -p)*2*q;
							if((fabs(vr)<=ep)&&(fabs(vi)<=ep))
								vr = ep*norm*(fabs(w) + fabs(q) +fabs(x) + fabs(y) + fabs(z));
							cdiv(x*r - z*ra + q*sa, x*s-z*sa -q*ra, vr, vi,h(i-1,na-1), h(i-1,en-1));
							if(fabs(x)>(fabs(z)+fabs(q)))
							{
								if(fabs(x)<ep)
								{
									return EMAT_DIVBYZERO;
								}
								h(i,na-1) = (-ra-w*h(i-1,na-1) + q*h(i-1,en-1))/x;
								h(i,en-1) = (-sa -w*h(i-1,en-1) - q*h(i-1,na-1))/x;
							}
							else
								cdiv(-r-y*h(i-1,na-1), -s-y*h(i-1,en-1), z,q, h(i,na-1), h(i,en-1));
						}
					}
				}
			}
		}
	}
	for(i=1; i<=low-1; i++)
		for(j=i+1; j<=n; j++)
			v(i-1,j-1) = h(i-1,j-1);
	for(i=upp+1; i<=n; i++)
		for(j=i+1; j<=n; j++)
			v(i-1,j-1) = h(i-1,j-1);
	for(j=n; j>=low; j--)
	{
		if(j<=upp)
			m=j;
		else
			m=upp;
		l=j-1;
		if(wi(j-1)<0)
		{
			for(i=low; i<=upp; i++)
			{
				y=z=0;
				for(k=low; k<=m; k++)
				{
					y += v(i-1,k-1)*h(k-1,l-1);
					z += v(i-1,k-1)*h(k-1,j-1);
				}											
				v(i-1,l-1)=y;
				v(i-1,j-1)=z;
			}

		}		
		else if(fabs(wi(j-1))<ep)
		{
			for(i=low;i<=upp; i++)
			{
				z=0;
				for(k=low; k<=m;k++)
					z+=v(i-1,k-1)*h(k-1,j-1);
				v(i-1,j-1)=z;
			}
		}
	}
	return 0;
}
/*
int sphqr2(double ep, CSparseDbMatrix &h, CSparseDbMatrix &v, double *wr, double *wi, int *cnt)
{
	int i,j,k,l,m,na,its,en,nl,la,low,upp;
	double p,q,r,s,t,w,x,y,z,ra,sa,vr,vi,norm,tmp;
	int n=h.mrow();
	div_t divmess;
	low = 1;
	upp = n;
	en = upp;
	t = 0;
	CIntVector hridx = h.rowchidx();
	CIntVector vridx = v.rowchidx();
	while(en>=low)
	{
		its =0;
		na = en -1;
		while(1)
		{
			la =0;
			for(l=en; l>=low+1; l--)
			{
				double h0 = h(l-1,l-2,hridx);
				double h1 = h(l-2,l-2,hridx);
				double h2 = h(l-1,l-1,hridx);
				if(fabs(h0)<=ep*(fabs(h1)+fabs(h2)))
				{
					la =1;
					break;
				}
			}
			if(!la)
				l = low;
			x = h(en-1,en-1,hridx);
			if(l == en)
			{
				la = 0;
				break;
			}

			y = h(na-1,na-1,hridx);
			w = h(en-1,na-1,hridx)*h(na-1,en-1,hridx);
			if(l==na)
			{
				la = 1;
				break;
			}
			if(its == MAX_ITERATETIME)
			{
				cnt(en - 1) = MAX_ITERATETIME + 1;
				return EMAT_OVERITERATE;
			}
			divmess = div(its,10);
			if(divmess.rem == 0)
			{
				t += x;
				if(x+1!=1)
				{
					for(i=low; i<=en; i++)
						h.set(i-1,i-1,hridx) -=x;
				}
				s = fabs(h(en-1,na-1,hridx))+fabs(h(na-1,en-3,hridx));
				x = y =0.75*s;
				w = -0.4375*s*s;
			}
			its += 1;

			for(m = en-2; m>=1; m--)//????
			{

				z=h(m-1,m-1,hridx);
				r=x-z;
				s=y-z;
				if(fabs(h(m,m-1,hridx))<ep)
				{
					return EMAT_DIVBYZERO;
				}
				p = (r*s-w)/h(m,m-1,hridx) + h(m-1,m,hridx);
				q = h(m,m,hridx)-z-r-s;
				r = h(m+1,m,hridx);
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
				if(fabs(h(m-1,m-2,hridx))*(fabs(q)+fabs(r)) <= ep*fabs(p)*
					(fabs(h(m-2,m-2,hridx)) + fabs(z) + fabs(h(m,m,hridx))))
					break;
			}
			for(i=m+2;i<=en; i++)
				h.set(i-1,i-3,hridx,0);
			for(i=m+3;i<=en; i++)
				h.set(i-1,i-4,hridx,0);
			for(k=m; k<=na; k++)
			{
				if(k != na)
					nl =1;
				else
					nl =0;
				if(k!=m)
				{
					p = h(k-1,k-2,hridx);
					q = h(k,k-2,hridx);
					if(nl)
						r = h(k+1,k-2,hridx);
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
					h.set(k-1,k-2,hridx, -s*x);
				else if(l!=m)
				{
					for(i=hridx(k-1); i<hridx(k); i++)
					{
						if(h(i).ypos()==k-2)
						{
							h(i).value() = -h(i).value();
							break;
						}
						else if(h(i).ypos()>k-2)
							break;
					}
				}
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
					p=h(k-1,j-1,hridx) + q*h(k,j-1,hridx);
					if(nl)
					{
						p +=r*h(k+1,j-1,hridx);
						if(p+1!=1)
						    h.set(k+1,j-1,hridx) -= p*z;
					}
					if(p+1!=1)
					{
						h.set(k,j-1,hridx) -= p*y;
						h.set(k-1,j-1,hridx) -=p*x;
					}
				}
				if(k+3 <en)
					j = k+3;
				else
					j=en;
				for(i=1; i<=j; i++)
				{
					p = x*h(i-1,k-1,hridx) +y*h(i-1,k,hridx);
					if(nl)
					{
						p += z*h(i-1,k+1,hridx);
						if(p+1!=1)
						    h.set(i-1,k+1,hridx) -= p*r;
					}
					if(p+1!=1)
					{
						h.set(i-1,k,hridx) -=p*q;
						h.set(i-1,k-1,hridx) -=p;
					}
				}
				for(i=low; i<=upp; i++)
				{
					p = x*v(i-1,k-1,vridx) + y*v(i-1,k,vridx);
					if(nl)
					{
						p += z*v(i-1,k+1,vridx);
						if(p+1!=1)
						    v.set(i-1,k+1,vridx) -=p*r;
					}
					if(p+1!=1)
					{
					    v.set(i-1,k,vridx) -= p*q;
					    v.set(i-1,k-1,vridx) -= p;
					}
				}
			}
		}

		if(! la)
		{
			wr(en-1)= x+t; 
			h.set(en -1,en -1, hridx, x+t);
			wi(en-1)=0;
			if(fabs(wr(en-1))<DBL_EPSILON)
				wr(en-1) = 0;
			cnt(en -1) =its;
			en = na;
			continue;
		}
		p = (y-x)/2;
		q = p*p +w;
		z = sqrt(fabs(q));
        x = x+t;
		h.set(en-1,en-1,hridx,x);
		h.set(na-1,na-1,hridx,y+t);
		cnt(en-1) = -its;
		cnt(na-1) = its;
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
			wr(na -1 ) = x+z;
			wr(en -1 ) =s=x-w/z;
			wi(na-1) = wi(en-1)=0;
			if(fabs(wr(en-1))<DBL_EPSILON)
				wr(en-1) =0;
			if(fabs(wr(na-1))<DBL_EPSILON)
				wr(na-1) =0;
			x = h(en-1,na-1,hridx);
			r = sqrt(x*x + z*z);
			if(r<ep)
			{
				return EMAT_DIVBYZERO;
			}
			p= x/r;
			q = z/r;

			for(j=na; j<=n; j++)
			{
				z= h(na-1,j-1,hridx);
				tmp = h(en-1,j-1,hridx);
				h.set(na-1,j-1,hridx) = q*z + p*tmp;
				h.set(en-1,j-1,hridx) = q*tmp - p*z;
			}
			for(i=1; i<=en; i++)
			{
				z= h(i-1,na-1,hridx);
				tmp = h(i-1,en-1,hridx);
				h.set(i-1,na-1,hridx) = q*z + p*tmp;
				h.set(i-1,en-1,hridx) = q*tmp - p*z;
			}
			for(i=low; i<=upp; i++)
			{
				z = v(i-1,na-1,vridx);
				tmp = v(i-1,en-1,vridx);
				v.set(i-1,na-1,vridx) = q*z + p*tmp;
				v.set(i-1,en-1,vridx) = q*tmp - p*z;
			}
		}
		else
		{
			wr(na-1) = wr(en-1) = x+p;
			if(fabs(wr(en-1))<DBL_EPSILON)
			{
				wr(en-1)=0;
				wr(na-1)=0;
			}
			wi(na-1)=z;
			wi(en-1)=-z;
			if(fabs(z)<DBL_EPSILON)
			{
				wi(en-1)=0;
				wi(na-1)=0;
			}
		}
		en -=2;
	}
	norm = 0;
	k = 1;

	for(i=1; i<=n; i++)
	{
		for(j=hridx(i-1); j<hridx(i); j++)
		{
			if(h(j).ypos()>=i-1)
				norm += fabs(h(j).value());
        }
	}
	if(norm<DBL_EPSILON)
		return EMAT_FAILURE;
	for(en=n; en>=1; en--)
	{
		p = wr(en-1);
		q = wi(en-1);
		na = en-1;
		if(fabs(q)<ep)
		{
			m=en;

			h.set(en-1,en-1,hridx) =1;
			for(i=na; i>=1; i--)
			{
				w= h(i-1,i-1,hridx)-p;
				r= h(i-1,en-1,hridx);
				for(j=m; j<=na; j++)
					r += h(i-1,j-1,hridx)*h(j-1,en-1,hridx);
				if(wi(i-1)<0)
				{
					z=w;
					s=r;
				}
				else
				{
					m=i;
					if(fabs(wi(i-1))<ep)
					{
						if(fabs(w)>=ep)
							h.set(i-1,en-1,hridx) = -r/w;
						else
							h.set(i-1,en-1,hridx) = -r/(ep*norm);
					}
					else
					{
						x = h(i-1,i,hridx);
						y = h(i,i-1,hridx);
						q = (wr(i-1) - p)*(wr(i-1) -p) + wi(i-1)*wi(i-1);
						if((q<ep)||(fabs(x)<ep)||(fabs(z)<ep))
						{
							return EMAT_DIVBYZERO;
						}
						h.set(i-1,en-1,hridx) = t = (x*s-z*r)/q;
						if(fabs(x) > fabs(z))
							h.set(i,en-1,hridx) = (-r-w*t)/x;
						else
							h.set(i,en-1,hridx)= (-s-y*t)/z;
					}
				}
			}
		}
		else
		{
			if(q<0)
			{
				m = na;
				tmp = fabs(h(en-1,na-1,hridx));
				if(tmp>fabs(h(na-1,en-1,hridx)))
				{
					if(tmp<ep)
					{
						return EMAT_DIVBYZERO;
					}
					h.set(na-1,na-1,hridx) = -(h(en-1,en-1,hridx)-p)/h(en-1,na-1,hridx);
					h.set(na-1,en-1,hridx) = -q/h(en-1,na-1,hridx);
				}
				else
					cdiv(-h(na-1,en-1,hridx),0,h(na-1,na-1,hridx)-p,q, &h.set(na-1,na-1,hridx), &h.set(na-1,en-1,hridx));
				h.set(en-1,na-1,hridx) = 1;
				h.set(en-1,en-1,hridx,0);
				for(i=na-1; i>=1; i--)
				{
					w = h(i-1,i-1,hridx) -p;
					ra = h(i-1,en-1,hridx);
					sa =0;
					for(j=m; j<=na; j++)
					{
						ra += h(i-1,j-1,hridx)*h(j-1,na-1,hridx);
						sa += h(i-1,j-1,hridx)*h(j-1,en-1,hridx);
					}
					if(wi(i-1)<0)
					{
						z=w;
						r=ra;
						s=sa;
					}
					else
					{
						m=i;
						if(fabs(wi(i-1))<=ep)
							cdiv(-ra, -sa, w,q,&h.set(i-1,na-1,hridx), &h.set(i-1,en-1,hridx));
						else
						{
							x=h(i-1,i,hridx);
							y = h(i,i-1,hridx);
							vr = (wr(i-1)-p)*(wr(i-1)-p) +wi(i-1)*wi(i-1) -q*q;
							vi =(wr(i-1) -p)*2*q;
							if((fabs(vr)<=ep)&&(fabs(vi)<=ep))
								vr = ep*norm*(fabs(w) + fabs(q) +fabs(x) + fabs(y) + fabs(z));
							cdiv(x*r - z*ra + q*sa, x*s-z*sa -q*ra, vr, vi,&h.set(i-1,na-1,hridx), &h.set(i-1,en-1,hridx));
							if(fabs(x)>(fabs(z)+fabs(q)))
							{
								if(fabs(x)<ep)
								{
									return EMAT_DIVBYZERO;
								}
								h.set(i,na-1,hridx) = (-ra-w*h(i-1,na-1,hridx) + q*h(i-1,en-1,hridx))/x;
								h.set(i,en-1,hridx) = (-sa -w*h(i-1,en-1,hridx) - q*h(i-1,na-1,hridx))/x;
							}
							else
								cdiv(-r-y*h(i-1,na-1,hridx), -s-y*h(i-1,en-1,hridx), z,q, &h.set(i,na-1,hridx), &h.set(i,en-1,hridx));
						}
					}
				}
			}
		}
	}
	for(i=1; i<=low-1; i++)
		for(j=i+1; j<=n; j++)
			v.set(i-1,j-1,vridx) = h(i-1,j-1,hridx);
	for(i=upp+1; i<=n; i++)
		for(j=i+1; j<=n; j++)
			v.set(i-1,j-1,vridx) = h(i-1,j-1,hridx);
	for(j=n; j>=low; j--)
	{
		if(j<=upp)
			m=j;
		else
			m=upp;
		l=j-1;
		if(wi(j-1)<0)
		{
			for(i=low; i<=upp; i++)
			{
				y=z=0;
				for(k=low; k<=m; k++)
				{
					y += v(i-1,k-1,vridx)*h(k-1,l-1,hridx);
					z += v(i-1,k-1,vridx)*h(k-1,j-1,hridx);
				}											
				v.set(i-1,l-1,vridx)=y;
				v.set(i-1,j-1,vridx)=z;
			}

		}		
		else if(fabs(wi(j-1))<ep)
		{
			for(i=low;i<=upp; i++)
			{
				z=0;
				for(k=low; k<=m;k++)
					z+=v(i-1,k-1,vridx)*h(k-1,j-1,hridx);
				v.set(i-1,j-1,vridx)=z;
			}
		}
	}
	return 0;
}
*/
int sphqr2(double ep, CSparseDbMatrix &h, CSparseDbMatrix &v, CVector<double> wr, CVector<double> wi, CVector<int> cnt)
{
	int i,j,k,l,m,na,its,en,nl,la,low,upp;
	double p,q,r,s,t,w,x,y,z,ra,sa,vr,vi,norm,tmp;
	int n=h.mrow();
	div_t divmess;
	low = 1;
	upp = n;
	en = upp;
	t = 0;
	CIntVector hridx = h.rowchidx();
	CIntVector vridx = v.rowchidx();
	while(en>=low)
	{
		its =0;
		na = en -1;
		while(1)
		{
			la =0;
			for(l=en; l>=low+1; l--)
			{
				double h0 = h(l-1,l-2,hridx);
				double h1 = h(l-2,l-2,hridx);
				double h2 = h(l-1,l-1,hridx);
				if(fabs(h0)<=ep*(fabs(h1)+fabs(h2)))
				{
					la =1;
					break;
				}
			}
			if(!la)
				l = low;
			x = h(en-1,en-1,hridx);
			if(l == en)
			{
				la = 0;
				break;
			}

			y = h(na-1,na-1,hridx);
			w = h(en-1,na-1,hridx)*h(na-1,en-1,hridx);
			if(l==na)
			{
				la = 1;
				break;
			}
			if(its == MAX_ITERATETIME)
			{
				cnt(en - 1) = MAX_ITERATETIME + 1;
				return EMAT_OVERITERATE;
			}
			divmess = div(its,10);
			if(divmess.rem == 0)
			{
				t += x;
				if(x+1!=1)
				{
					for(i=low; i<=en; i++)
						h.set(i-1,i-1,hridx) -=x;
				}
				s = fabs(h(en-1,na-1,hridx))+fabs(h(na-1,en-3,hridx));
				x = y =0.75*s;
				w = -0.4375*s*s;
			}
			its += 1;

			for(m = en-2; m>=1; m--)//????
			{

				z=h(m-1,m-1,hridx);
				r=x-z;
				s=y-z;
				if(fabs(h(m,m-1,hridx))<ep)
				{
					return EMAT_DIVBYZERO;
				}
				p = (r*s-w)/h(m,m-1,hridx) + h(m-1,m,hridx);
				q = h(m,m,hridx)-z-r-s;
				r = h(m+1,m,hridx);
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
				if(fabs(h(m-1,m-2,hridx))*(fabs(q)+fabs(r)) <= ep*fabs(p)*
					(fabs(h(m-2,m-2,hridx)) + fabs(z) + fabs(h(m,m,hridx))))
					break;
			}
			for(i=m+2;i<=en; i++)
				h.set(i-1,i-3,hridx,0);
			for(i=m+3;i<=en; i++)
				h.set(i-1,i-4,hridx,0);
			for(k=m; k<=na; k++)
			{
				if(k != na)
					nl =1;
				else
					nl =0;
				if(k!=m)
				{
					p = h(k-1,k-2,hridx);
					q = h(k,k-2,hridx);
					if(nl)
						r = h(k+1,k-2,hridx);
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
					h.set(k-1,k-2,hridx, -s*x);
				else if(l!=m)
				{
					for(i=hridx(k-1); i<hridx(k); i++)
					{
						if(h(i).ypos()==k-2)
						{
							h(i).value() = -h(i).value();
							break;
						}
						else if(h(i).ypos()>k-2)
							break;
					}
				}
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
					p=h(k-1,j-1,hridx) + q*h(k,j-1,hridx);
					if(nl)
					{
						p +=r*h(k+1,j-1,hridx);
						if(p+1!=1)
						    h.set(k+1,j-1,hridx) -= p*z;
					}
					if(p+1!=1)
					{
						h.set(k,j-1,hridx) -= p*y;
						h.set(k-1,j-1,hridx) -=p*x;
					}
				}
				if(k+3 <en)
					j = k+3;
				else
					j=en;
				for(i=1; i<=j; i++)
				{
					p = x*h(i-1,k-1,hridx) +y*h(i-1,k,hridx);
					if(nl)
					{
						p += z*h(i-1,k+1,hridx);
						if(p+1!=1)
						    h.set(i-1,k+1,hridx) -= p*r;
					}
					if(p+1!=1)
					{
						h.set(i-1,k,hridx) -=p*q;
						h.set(i-1,k-1,hridx) -=p;
					}
				}
				for(i=low; i<=upp; i++)
				{
					p = x*v(i-1,k-1,vridx) + y*v(i-1,k,vridx);
					if(nl)
					{
						p += z*v(i-1,k+1,vridx);
						if(p+1!=1)
						    v.set(i-1,k+1,vridx) -=p*r;
					}
					if(p+1!=1)
					{
					    v.set(i-1,k,vridx) -= p*q;
					    v.set(i-1,k-1,vridx) -= p;
					}
				}
			}
		}

		if(! la)
		{
			wr(en-1)= x+t; 
			h.set(en -1,en -1, hridx, x+t);
			wi(en-1)=0;
			if(fabs(wr(en-1))<DBL_EPSILON)
				wr(en-1) = 0;
			cnt(en -1) =its;
			en = na;
			continue;
		}
		p = (y-x)/2;
		q = p*p +w;
		z = sqrt(fabs(q));
        x = x+t;
		h.set(en-1,en-1,hridx,x);
		h.set(na-1,na-1,hridx,y+t);
		cnt(en-1) = -its;
		cnt(na-1) = its;
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
			wr(na -1 ) = x+z;
			wr(en -1 ) =s=x-w/z;
			wi(na-1) = wi(en-1)=0;
			if(fabs(wr(en-1))<DBL_EPSILON)
				wr(en-1) =0;
			if(fabs(wr(na-1))<DBL_EPSILON)
				wr(na-1) =0;
			x = h(en-1,na-1,hridx);
			r = sqrt(x*x + z*z);
			if(r<ep)
			{
				return EMAT_DIVBYZERO;
			}
			p= x/r;
			q = z/r;

			for(j=na; j<=n; j++)
			{
				z= h(na-1,j-1,hridx);
				tmp = h(en-1,j-1,hridx);
				h.set(na-1,j-1,hridx) = q*z + p*tmp;
				h.set(en-1,j-1,hridx) = q*tmp - p*z;
			}
			for(i=1; i<=en; i++)
			{
				z= h(i-1,na-1,hridx);
				tmp = h(i-1,en-1,hridx);
				h.set(i-1,na-1,hridx) = q*z + p*tmp;
				h.set(i-1,en-1,hridx) = q*tmp - p*z;
			}
			for(i=low; i<=upp; i++)
			{
				z = v(i-1,na-1,vridx);
				tmp = v(i-1,en-1,vridx);
				v.set(i-1,na-1,vridx) = q*z + p*tmp;
				v.set(i-1,en-1,vridx) = q*tmp - p*z;
			}
		}
		else
		{
			wr(na-1) = wr(en-1) = x+p;
			if(fabs(wr(en-1))<DBL_EPSILON)
			{
				wr(en-1)=0;
				wr(na-1)=0;
			}
			wi(na-1)=z;
			wi(en-1)=-z;
			if(fabs(z)<DBL_EPSILON)
			{
				wi(en-1)=0;
				wi(na-1)=0;
			}
		}
		en -=2;
	}
	norm = 0;
	k = 1;

	for(i=1; i<=n; i++)
	{
		for(j=hridx(i-1); j<hridx(i); j++)
		{
			if(h(j).ypos()>=i-1)
				norm += fabs(h(j).value());
        }
	}
	if(norm<DBL_EPSILON)
		return EMAT_FAILURE;
	for(en=n; en>=1; en--)
	{
		p = wr(en-1);
		q = wi(en-1);
		na = en-1;
		if(fabs(q)<ep)
		{
			m=en;

			h.set(en-1,en-1,hridx) =1;
			for(i=na; i>=1; i--)
			{
				w= h(i-1,i-1,hridx)-p;
				r= h(i-1,en-1,hridx);
				for(j=m; j<=na; j++)
					r += h(i-1,j-1,hridx)*h(j-1,en-1,hridx);
				if(wi(i-1)<0)
				{
					z=w;
					s=r;
				}
				else
				{
					m=i;
					if(fabs(wi(i-1))<ep)
					{
						if(fabs(w)>=ep)
							h.set(i-1,en-1,hridx) = -r/w;
						else
							h.set(i-1,en-1,hridx) = -r/(ep*norm);
					}
					else
					{
						x = h(i-1,i,hridx);
						y = h(i,i-1,hridx);
						q = (wr(i-1) - p)*(wr(i-1) -p) + wi(i-1)*wi(i-1);
						if((q<ep)||(fabs(x)<ep)||(fabs(z)<ep))
						{
							return EMAT_DIVBYZERO;
						}
						h.set(i-1,en-1,hridx) = t = (x*s-z*r)/q;
						if(fabs(x) > fabs(z))
							h.set(i,en-1,hridx) = (-r-w*t)/x;
						else
							h.set(i,en-1,hridx)= (-s-y*t)/z;
					}
				}
			}
		}
		else
		{
			if(q<0)
			{
				m = na;
				tmp = fabs(h(en-1,na-1,hridx));
				if(tmp>fabs(h(na-1,en-1,hridx)))
				{
					if(tmp<ep)
					{
						return EMAT_DIVBYZERO;
					}
					h.set(na-1,na-1,hridx) = -(h(en-1,en-1,hridx)-p)/h(en-1,na-1,hridx);
					h.set(na-1,en-1,hridx) = -q/h(en-1,na-1,hridx);
				}
				else
					cdiv(-h(na-1,en-1,hridx),0,h(na-1,na-1,hridx)-p,q, h.set(na-1,na-1,hridx), h.set(na-1,en-1,hridx));
				h.set(en-1,na-1,hridx) = 1;
				h.set(en-1,en-1,hridx,0);
				for(i=na-1; i>=1; i--)
				{
					w = h(i-1,i-1,hridx) -p;
					ra = h(i-1,en-1,hridx);
					sa =0;
					for(j=m; j<=na; j++)
					{
						ra += h(i-1,j-1,hridx)*h(j-1,na-1,hridx);
						sa += h(i-1,j-1,hridx)*h(j-1,en-1,hridx);
					}
					if(wi(i-1)<0)
					{
						z=w;
						r=ra;
						s=sa;
					}
					else
					{
						m=i;
						if(fabs(wi(i-1))<=ep)
							cdiv(-ra, -sa, w,q,h.set(i-1,na-1,hridx), h.set(i-1,en-1,hridx));
						else
						{
							x=h(i-1,i,hridx);
							y = h(i,i-1,hridx);
							vr = (wr(i-1)-p)*(wr(i-1)-p) +wi(i-1)*wi(i-1) -q*q;
							vi =(wr(i-1) -p)*2*q;
							if((fabs(vr)<=ep)&&(fabs(vi)<=ep))
								vr = ep*norm*(fabs(w) + fabs(q) +fabs(x) + fabs(y) + fabs(z));
							cdiv(x*r - z*ra + q*sa, x*s-z*sa -q*ra, vr, vi,h.set(i-1,na-1,hridx), h.set(i-1,en-1,hridx));
							if(fabs(x)>(fabs(z)+fabs(q)))
							{
								if(fabs(x)<ep)
								{
									return EMAT_DIVBYZERO;
								}
								h.set(i,na-1,hridx) = (-ra-w*h(i-1,na-1,hridx) + q*h(i-1,en-1,hridx))/x;
								h.set(i,en-1,hridx) = (-sa -w*h(i-1,en-1,hridx) - q*h(i-1,na-1,hridx))/x;
							}
							else
								cdiv(-r-y*h(i-1,na-1,hridx), -s-y*h(i-1,en-1,hridx), z,q, h.set(i,na-1,hridx), h.set(i,en-1,hridx));
						}
					}
				}
			}
		}
	}
	for(i=1; i<=low-1; i++)
		for(j=i+1; j<=n; j++)
			v.set(i-1,j-1,vridx) = h(i-1,j-1,hridx);
	for(i=upp+1; i<=n; i++)
		for(j=i+1; j<=n; j++)
			v.set(i-1,j-1,vridx) = h(i-1,j-1,hridx);
	for(j=n; j>=low; j--)
	{
		if(j<=upp)
			m=j;
		else
			m=upp;
		l=j-1;
		if(wi(j-1)<0)
		{
			for(i=low; i<=upp; i++)
			{
				y=z=0;
				for(k=low; k<=m; k++)
				{
					y += v(i-1,k-1,vridx)*h(k-1,l-1,hridx);
					z += v(i-1,k-1,vridx)*h(k-1,j-1,hridx);
				}											
				v.set(i-1,l-1,vridx)=y;
				v.set(i-1,j-1,vridx)=z;
			}

		}		
		else if(fabs(wi(j-1))<ep)
		{
			for(i=low;i<=upp; i++)
			{
				z=0;
				for(k=low; k<=m;k++)
					z+=v(i-1,k-1,vridx)*h(k-1,j-1,hridx);
				v.set(i-1,j-1,vridx)=z;
			}
		}
	}
	return 0;
}

int cjcbi(CSymDbMatrix a, int n, CDoubleMatrix v, double eps)
{ 
	int i,j,p,q,l;
    double fm,cn,sn,omega,x,y,d;
    l=1;
	while (1)
	{ 
		fm=0.0;
		for (i=1; i<=n-1; i++)
		{
		    for (j=0; j<=i-1; j++)
			{ 
			    d=fabs(a(i,j));
			    if ((i!=j)&&(d>fm))
				{ 
					fm=d; 
					p=i; 
					q=j;
				}
			}
		}
		if (fm<eps)  
			return 0;
		if (l>MAX_ITERATETIME)  
			return EMAT_OVERITERATE;
		l=l+1;
		x=-a(p,q); 
		y=(a(q,q)-a(p,p))/2.0;
		omega=x/sqrt(x*x+y*y);
		if (y<0.0) 
			omega=-omega;
		sn=1.0+sqrt(1.0-omega*omega);
		sn=omega/sqrt(2.0*sn);
		cn=sqrt(1.0-sn*sn);
		fm=a(p,p);
		a(p,p)=fm*cn*cn+a(q,q)*sn*sn+a(p,q)*omega;
		a(q,q)=fm*sn*sn+a(q,q)*cn*cn-a(p,q)*omega;
		a(p,q)=0.0; 
		for (j=0; j<=n-1; j++)
		{
		    if ((j!=p)&&(j!=q))
			{ 
			    fm=a(p,j);
			    a(p,j)=fm*cn+a(q,j)*sn;
			    a(q,j)=-fm*sn+a(q,j)*cn;
			}
		}
		for (i=0; i<=n-1; i++)
		{  
			fm=v(i,p);
			v(i,p)=fm*cn+v(i,q)*sn;
			v(i,q)=-fm*sn+v(i,q)*cn;
		}
	}
    return 0;
}


// 求特征根及特征向量
//	a:     双精度二维矩阵：n*n,存放对称矩阵A，返回时对角线上存放n个特征值
//	n:     A的阶数
//	v：    n*n 返回特征向量
//	eps：  控制精度要求
void cjcbj(CDoubleVector& Vect_A,int n,CDoubleVector& Vect_V,double eps) 
{ 
	int i,j,p,q,u,w,t,s;
	double ff,fm,cn,sn,omega,x,y,d;
	for (i=0; i<=n-1; i++)
	{
		Vect_V[i*n+i]=1.0;
		for (j=0; j<=n-1; j++)
			if (i!=j)
				Vect_V[i*n+j]=0.0;
	}
	ff=0.0;
	for (i=1; i<=n-1; i++)
		for (j=0; j<=i-1; j++)
		{
			d=Vect_A[i*n+j]; 
			ff=ff+d*d; 
		}
	ff=sqrt(2.0*ff);
		
loop0:
	ff=ff/(1.0*n);
		
loop1:
	for (i=1; i<=n-1; i++)
		for (j=0; j<=i-1; j++)
		{
			d=fabs(Vect_A[i*n+j]);
			if (d>ff)
			{
				p=i; 
				q=j;
				goto loop;
			}
		}
	if (ff<eps) 
		return;
	goto loop0;
			
loop: 
	u=p*n+q;
	w=p*n+p; 
	t=q*n+p; 
	s=q*n+q;
	x=-Vect_A[u]; 
	y=(Vect_A[s]-Vect_A[w])/2.0;
	omega=x/sqrt(x*x+y*y);
	if (y<0.0) 
		omega=-omega;
	sn=1.0+sqrt(1.0-omega*omega);
	sn=omega/sqrt(2.0*sn);
	cn=sqrt(1.0-sn*sn);
	fm=Vect_A[w];
	Vect_A[w]=fm*cn*cn+Vect_A[s]*sn*sn+Vect_A[u]*omega;
	Vect_A[s]=fm*sn*sn+Vect_A[s]*cn*cn-Vect_A[u]*omega;
	Vect_A[u]=0.0; 
	Vect_A[t]=0.0;
	for (j=0; j<=n-1; j++)
		if ((j!=p)&&(j!=q))
		{ 
			u=p*n+j; 
			w=q*n+j;
			fm=Vect_A[u];
			Vect_A[u]=fm*cn+Vect_A[w]*sn;
			Vect_A[w]=-fm*sn+Vect_A[w]*cn;
		}
	for (i=0; i<=n-1; i++)
		if ((i!=p)&&(i!=q))
		{
			u=i*n+p; 
			w=i*n+q;
			fm=Vect_A[u];
			Vect_A[u]=fm*cn+Vect_A[w]*sn;
			Vect_A[w]=-fm*sn+Vect_A[w]*cn;
		}
	for (i=0; i<=n-1; i++)
	{ 
		u=i*n+p; 
		w=i*n+q;
		fm=Vect_V[u];
		Vect_V[u]=fm*cn+Vect_V[w]*sn;
		Vect_V[w]=-fm*sn+Vect_V[w]*cn;
	}
	goto loop1;
}


void WriteData(CDoubleMatrix &m, char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	int nRow=m.mrow();
	int nCol=m.mcol();
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%f  ",m(i,j));
		}
		fprintf(m_fp,"\n");
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}		

void WriteData(CSparseDbMatrix &m, char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	int nRow=m.mrow();
	int nCol=m.mcol();
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%f  ",m(i,j));
		}
		fprintf(m_fp,"\n");
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}		

void WriteData(CSparseCompMatrix &m, char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	int nRow=m.mrow();
	int nCol=m.mcol();
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%f",real(m(i,j)));
			if(imag(m(i,j))>0)
				fprintf(m_fp,"+");
			else
				fprintf(m_fp,"-");
			fprintf(m_fp,"%fi  ",fabs(imag(m(i,j))));
		}
		fprintf(m_fp,"\n");
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}		


void WriteTrData(CSparseCompMatrix &m, char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<m.vlen();i++)
	{
		fprintf(m_fp,"(%d, %d, %f  ",m(i).xpos(), m(i).ypos(), real(m(i).value()));
		if(imag(m(i).value())>0)
			fprintf(m_fp,"+");
		else
			fprintf(m_fp,"-");
		fprintf(m_fp," %f)\n",fabs(imag(m(i).value())));
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}		


void WriteTrData(CSparseDbMatrix &m, char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<m.vlen();i++)
	{
		fprintf(m_fp,"(%d, %d, %f  )\n",m(i).xpos(), m(i).ypos(), m(i).value());
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}		


void WriteData(CIntMatrix &m, char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	int nRow=m.mrow();
	int nCol=m.mcol();
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%d  ",m(i,j));
		}
		fprintf(m_fp,"\n");
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}		


void WriteData(CComplexMatrix &m, char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	int nRow=m.mrow();
	int nCol=m.mcol();
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%f",real(m(i,j)));
			if(imag(m(i,j))>0)
				fprintf(m_fp,"+");
			else
				fprintf(m_fp,"-");
			fprintf(m_fp,"%fi  ",fabs(imag(m(i,j))));
		}
		fprintf(m_fp,"\n");
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}		

void WriteData(CDoubleVector &m,char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	int nRow = m.vlen();
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		fprintf(m_fp,"%f  ",m(i));
	}
	fprintf(m_fp,"\n");
	fclose(m_fp);
	m_fp = NULL;
#endif
}		

void WriteData(CIntVector &m,char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	int nRow = m.vlen();
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		fprintf(m_fp,"%d  ",m(i));
	}
	fprintf(m_fp,"\n");
	fclose(m_fp);
	m_fp = NULL;
#endif
}		


void WriteData(CComplexVector &m, char * pszTitle)
{
#ifdef _DEBUG
	FILE *m_fp;
	int nRow = m.vlen();
	m_fp = fopen("Matrix.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		fprintf(m_fp,"%f",real(m(i)));
		if(imag(m(i))>0)
			fprintf(m_fp,"+");
		else
			fprintf(m_fp,"-");
		fprintf(m_fp,"%fi  ",fabs(imag(m(i))));
	}
	fprintf(m_fp,"\n");
	fclose(m_fp);
	m_fp = NULL;
#endif
}		


void WriteData(double var,char * title)
{
#ifdef _DEBUG
	FILE* m_fp;
	m_fp=fopen("matrix.txt","a");
	if(!m_fp)
	{
		return;
	}
	fprintf(m_fp, "\n%s=",title);
	fprintf(m_fp, "%f",var);
	fprintf(m_fp, "\n\n");
	fclose(m_fp);
	m_fp=NULL;
#endif
	
}

void WriteData(int var,char * title)
{
#ifdef _DEBUG
	FILE* m_fp;
	m_fp=fopen("matrix.txt","a");
	if(!m_fp)
	{
		return;
	}
	fprintf(m_fp, "\n%s=",title);
	fprintf(m_fp, "%d",var);
	fprintf(m_fp, "\n");
	fclose(m_fp);
	m_fp=NULL;
#endif
}


void WriteData(double *var, int n, char * title)
{
#ifdef _DEBUG
	FILE* m_fp;
	m_fp=fopen("matrix.txt","a");
	if(!m_fp)
	{
		return;
	}
	if(title)
    {
        fprintf(m_fp,"\n\n%s\n\n",title);
    }
	for(int i=1;i<=n;i++)
	{
		fprintf(m_fp, "%f\t",var[i-1]);
	}
	fprintf(m_fp, "\n\n");
	fclose(m_fp);
	m_fp=NULL;
#endif	
}

void WriteData(int *var, int n, char * title)
{
#ifdef _DEBUG
	FILE* m_fp;
	m_fp=fopen("matrix.txt","a");
	if(!m_fp)
	{
		return;
	}
	if(title)
    {
        fprintf(m_fp,"\n\n%s\n\n",title);
    }
	for(int i=1;i<=n;i++)
	{
		fprintf(m_fp, "%d\t",var[i-1]);
		if ((i%5)==0)
			fprintf(m_fp,"\n");
	}
	fprintf(m_fp, "\n\n");
	fclose(m_fp);
	m_fp=NULL;
#endif	
}


void WriteData(double **var, int m,int n, char * title)
{
#ifdef _DEBUG
	FILE* m_fp;
	m_fp=fopen("matrix.txt","a");
	if(!m_fp)
	{
		return;
	}
	if(title)
    {
        fprintf(m_fp,"\n\n%s\n\n",title);
    }
	for(int i=1;i<=m;i++)
	{
		for(int j=1;j<=n;j++)
		{
			fprintf(m_fp, "%f\t",var[i-1][j-1]);
			
		}
		fprintf(m_fp,"\n");
	}
	fprintf(m_fp, "\n\n");
	fclose(m_fp);
	m_fp=NULL;
#endif	
}



//{{{START 2003-1-22 Eric   Change arguments
//void WriteData(double **var,int m,int n,char * title)
void WriteData(CMatrix<double> var,int m,int n,char * title)
//}}}END  2003-1-22 Eric
{
#ifdef _DEBUG
	FILE* m_fp;
	m_fp=fopen("matrix.txt","a");
	if(!m_fp)
	{
		return;
	}
	if(title)
    {
        fprintf(m_fp,"\n\n%s\n\n",title);
    }
	for(int i=1;i<=m;i++)
	{
		for(int j=1;j<=n;j++)
		{
			fprintf(m_fp, "%f\t",var(i-1,j-1));
		}
		fprintf(m_fp,"\n");
	}
	fprintf(m_fp, "\n\n");
	fclose(m_fp);
	m_fp=NULL;
#endif	
}

double normcdf(double x, double mu, double sigma)
{
	double y;
	if (sigma<=0.0) sigma=1.0e-10;
	y=0.5+0.5*lerrf((x-mu)/(sqrt(2.0)*sigma));
	return(y);
}

double CalkCommonFx(CDoubleVector DataArray,CDoubleVector WeightArray,double x,double nSize)
{
	int nLen=DataArray.vlen();
	if(x<DataArray(0))
		return 0;
	else 
	{
		double ni=0;
		for(int i=0;i<nLen-1;i++)
		{
			ni=ni+WeightArray(i);			
			if(x>=DataArray(i)&&x<DataArray(i+1))
				return double(ni)/nSize;
		}
		ni=ni+WeightArray(nLen-1);
		return 1.0*ni/nSize;
	}
	return 1;

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
}

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
		
	/*	
		if(fabs(x)<1.0e-30)
		{
			p = 0.0;
			return p;
		}
		
	*/


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
/*
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
*/


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
double chi2(double ch, int n, double d) 
{
    double PIs,x,chs,u,pp, p;
	int iai;
	PIs = sqrt(PI);
	x = ch/2;
	chs = sqrt(ch);
	
	if(fabs(ch)<1.0e-30)
	{
		p = 0.0;
		d = 0.0;
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
		d = u/ch;
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
	d = u/ch;
	return p;
}

//置信度a=p时对应的上分位数ch
double pchi2(int n, double p)
{
    double x, xx, x0, pp, q, d, w;
	int k;

	d =1.0 ;
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
		pp = chi2(x0, n, d);
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
		pp = tcdf(t0,(int)v);
		d = tpdf(t0,(int)v);
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
double tdist(double t, int n, double &dd)
{
	int sign, ibi, i;
	double tt, x, p, pp, u, ga1, ga2;
	if(fabs(t)<DOUBLE_ZERO)
	{
		ga1 = lgam1(n/2.0);
		ga2 = lgam1(n/2.0+0.5);
		pp = 0.5;
		dd = ga2/(sqrt(n*PI)*ga1);
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
		dd = u/fabs(t);
		pp = 0.5 + sign*p/2;
		return pp;
	}
	for(i = ibi; i<=n-2; i+=2)
	{
		p = p+2*u/i;
		u = u*(1+i)/i*(1-x);
	}
	dd = u/fabs(t);
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
		pp = tdist(t0,n,d);
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
double fdist(int n1, int n2, double f, double& d)
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
		d = u/f;
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
			d = u/f;
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
		d = u/f;
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
		d = u/f;
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
	d = u/f;
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
        pp = fdist(n1, n2, f0, d);
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


void pearson(CDoubleVector x, int k, int n, CDoubleVector r)
{
	int i, j,ii;
	CDoubleVector xx(k*n), xe(k), ye(k);
	double z;
    
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
	return;
}

double partf(double r1, double r2, double r3)
{
	return (r1 - r2*r3)/sqrt((1-r2*r2)*(1-r3*r3));
}

double partial(CDoubleVector r, CIntVector pos, int count, int n)
{
	assert(count >2);
	double  result;
	if(count == 3) //计算一阶偏相关系数
	{
		result = partf(r[pos[0]+pos[1]*n], r[pos[0]+pos[2]*n], r[pos[1]+pos[2]*n]);
		return result;
    }
	
	double r1, r2, r3;
	CIntVector poscp(pos.vlen()-1); //序列(i,j)
	CIntVector poscp2(pos.vlen()-1);//序列(i,k)
	CIntVector poscp3(pos.vlen()-1);//序列(j,k)
	for(int m = 0;m<pos.vlen()-1;m++)
	{
		poscp[m]        =        pos[m];
		poscp2[m]        =        pos[m];
		poscp3[m]        =        pos[m];
	}
	poscp2[1]                =        pos[pos.vlen()-1];//序列(i,j)
	poscp3[0]                =        pos[1];           //序列(i,k)
	poscp3[1]                =        poscp2[1];        //序列(i,k)

	r1 = partial(r,poscp,count-1, n); //计算R(i,j)

	r2 = partial(r,poscp2,count-1, n); //计算R(i,k)
	
	r3 = partial(r,poscp3,count-1, n);//计算R(j,k)
	return partf(r1, r2, r3); //执行 R(i,j,k) = R(i,j)/sqrt(R(i,k)*R(j,k))
}


//{{{START 2003-1-22 Eric   Change arguments
//int bmaqr1(double **a, int m, int n, double **q)
int bmaqr1(CMatrix<double> a, int m, int n, CMatrix<double> q)
//}}}END  2003-1-22 Eric
{
    int i,j,k,nn,jj;
    double u,alpha,w,t;
    //if (m<n)
    //{ 
    //    return EMAT_ROWSLESSCOLS;
    //}
	assert(m>=n); //zfm
    for (i=0; i<=m-1; i++)
    {
        for (j=0; j<=m-1; j++)
        {  
            q(i,j)=0.0;
            if (i==j) 
                q(i,j)=1.0;
		}
	}
    nn=n;
    if (m==n) 
          nn=m-1;
    for (k=0; k<=nn-1; k++)
    { 
        u=0.0; 
        for (i=k; i<=m-1; i++)
        { 
            w=fabs(a(i,k));
            if (w>u) 
                    u=w;
        }
        alpha=0.0;
        for (i=k; i<=m-1; i++)
        { 
            t=a(i,k)/u; 
            alpha=alpha+t*t;
        }
        if (a(k,k)>0.0)
                u=-u;
        alpha=u*sqrt(alpha);
        if (fabs(alpha)+1.0==1.0)
        { 
			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写
					//q[i][j]=q[j][i];
					double fTemp = q(j,i);
					q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
					q(j,i)=t;
				}
			}                
            return EMAT_DIVBYZERO;
        }
        u=sqrt(2.0*alpha*(alpha-a(k,k)));
        if ((u+1.0)!=1.0)
        { 
            a(k,k)=(a(k,k)-alpha)/u;
            for (i=k+1; i<=m-1; i++)
            { 
                a(i,k)=a(i,k)/u;
            }
            for (j=0; j<=m-1; j++)
            { 
                t=0.0;
                for (jj=k; jj<=m-1; jj++)
                    t=t+a(jj,k)*q(jj,j);
                for (i=k; i<=m-1; i++)
                {
                    q(i,j)=q(i,j)-2.0*t*a(i,k);
                }
            }
            for (j=k+1; j<=n-1; j++)
            { 
                t=0.0;
                for (jj=k; jj<=m-1; jj++)
                    t=t+a(jj,k)*a(jj,j);
                for (i=k; i<=m-1; i++)
                { 
                    a(i,j)=a(i,j)-2.0*t*a(i,k);
                }
			}
            a(k,k)=alpha;
            for (i=k+1; i<=m-1; i++)
                a(i,k)=0.0;
        }
    }

    for (i=0; i<=m-2; i++)
    {
        for (j=i+1; j<=m-1;j++)
        { 
            t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写
            //q[i][j]=q[j][i];
			double fTemp = q(j,i);
			q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
            q(j,i)=t;
        }
	}
    return(0);
}

//{{{START 2003-1-22 Eric   Change arguments
//int bmaqr(double **a, int m, int n, double **q)
int bmaqr(CMatrix<double> a, int m, int n, CMatrix<double> q)
//}}}END  2003-1-22 Eric
{
    int i,j,k,nn,jj;
    double u,alpha,w,t;
    for (i=0; i<=m-1; i++)
    {
        for (j=0; j<=m-1; j++)
        {  
            q(i,j)=0.0;
            if (i==j) 
                q(i,j)=1.0;
		}
	}
    nn=n;
    if (m==n) 
          nn=m-1;
    for (k=0; k<=nn-1; k++)
    { 
        u=0.0; 
        for (i=k; i<=m-1; i++)
        { 
            w=fabs(a(i,k));
            if (w>u) 
                    u=w;
        }
	    alpha=0.0;
		if(k<m && k<n)
		{
			for (i=k; i<=m-1; i++)
			{ 
				t=a(i,k)/u; 
				alpha=alpha+t*t;
			}	
            if (a(k,k)>0.0)
                u=-u;
		}
		else
		{
			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写
					//q[i][j]=q[j][i];
					double fTemp = q(j,i);
					q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
					q(j,i)=t;
				}
			}                
            return 0;
		}
        alpha=u*sqrt(alpha);
        if (fabs(alpha)+1.0==1.0)
        { 
			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写
					//q[i][j]=q[j][i];
					double fTemp = q(j,i);
					q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
					q(j,i)=t;
				}
			}                
            return EMAT_DIVBYZERO;
        }
        u=sqrt(2.0*alpha*(alpha-a(k,k)));
        if ((u+1.0)!=1.0)
        { 
            a(k,k)=(a(k,k)-alpha)/u;
            for (i=k+1; i<=m-1; i++)
            { 
                a(i,k)=a(i,k)/u;
            }
            for (j=0; j<=m-1; j++)
            { 
                t=0.0;
                for (jj=k; jj<=m-1; jj++)
                    t=t+a(jj,k)*q(jj,j);
                for (i=k; i<=m-1; i++)
                {
                    q(i,j)=q(i,j)-2.0*t*a(i,k);
                }
            }
            for (j=k+1; j<=n-1; j++)
            { 
                t=0.0;
                for (jj=k; jj<=m-1; jj++)
                    t=t+a(jj,k)*a(jj,j);
                for (i=k; i<=m-1; i++)
                { 
                    a(i,j)=a(i,j)-2.0*t*a(i,k);
                }
			}
            a(k,k)=alpha;
            for (i=k+1; i<=m-1; i++)
                a(i,k)=0.0;
        }
    }

    for (i=0; i<=m-2; i++)
    {
        for (j=i+1; j<=m-1;j++)
        { 
            t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写
            //q[i][j]=q[j][i];
			double fTemp = q(j,i);
			q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
            q(j,i)=t;
        }
	}
    return(0);
}

//{{{START 2003-1-22 Eric   Change arguments
//int bmaqr1(double **a, int m, int n, double **q, int *e)
int bmaqr1(CMatrix<double> a, int m, int n, CMatrix<double> q, CVector<int> e)
//}}}END  2003-1-22 Eric
{
    int i,j,kk,kk1,nn,jj,r;
    double u,alpha,w,t,tw,swap;
	
    //if (m<n)
    //{ 
    //    return EMAT_ROWSLESSCOLS;
    //}
	assert(m>=n); //zfm
	for(i=0; i<n; i++)
		e(i) = i+1;
	double *c = new double[n];
    for (i=0; i<=m-1; i++)
    {
        for (j=0; j<=m-1; j++)
        {  
            q(i,j)=0.0;
            if (i==j) 
                q(i,j)=1.0;
		}
	}
    nn=n;
    if (m==n) 
          nn=m-1;
	for(i=0; i<n; i++)
	{
        c[i] = 0;
		for(j=0; j<m; j++)
		{
			c[i] += a(j,i)*a(j,i);
		}
	}
	tw=c[0];
	r=-1;
	kk=0;
    for(i=1; i<n; i++)
	{
		if(c[i]>tw)
		{
			tw = c[i];
			kk=i;
		}
	}
	kk1=-1;
    while(tw>0)
	{
        r++;
		if(kk !=r)
		{
			kk1 = e(r);
		    e(r)=e(kk);
		    e(kk)=kk1;
		}
		if(kk!=r)
		{
			for(i=0; i<m; i++)
			{
				swap = a(i,r);
				a(i,r) = a(i,kk);
				a(i,kk) = swap;
			}
			swap = c[r];
			c[r] = c[kk];
			c[kk] = swap;
		}
		u=0.0; 
		for (i=r; i<=m-1; i++)
		{ 
			w=fabs(a(i,r));
			if (w>u) 
					u=w;
		}
		alpha=0.0;
		for (i=r; i<=m-1; i++)
		{ 
			t=a(i,r)/u; 
			alpha=alpha+t*t;
		}
		if (a(r,r)>0.0)
				u=-u;
		alpha=u*sqrt(alpha);
		if (fabs(alpha)+1.0==1.0)
		{ 
			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写
					//q[i][j]=q[j][i];
					double fTemp = q(j,i);
					q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
					q(j,i)=t;
				}
			}
            delete []c;
			return EMAT_DIVBYZERO;
		}
		u=sqrt(2.0*alpha*(alpha-a(r,r)));
		if ((u+1.0)!=1.0)
		{ 
			a(r,r)=(a(r,r)-alpha)/u;
			for (i=r+1; i<=m-1; i++)
			{ 
				a(i,r)=a(i,r)/u;
			}
			for (j=0; j<=m-1; j++)
			{ 
				t=0.0;
				for (jj=r; jj<=m-1; jj++)
					t=t+a(jj,r)*q(jj,j);
				for (i=r; i<=m-1; i++)
				{
					q(i,j)=q(i,j)-2.0*t*a(i,r);
				}
			}
			for (j=r+1; j<=n-1; j++)
			{ 
				t=0.0;
				for (jj=r; jj<=m-1; jj++)
					t=t+a(jj,r)*a(jj,j);
				for (i=r; i<=m-1; i++)
				{ 
					a(i,j)=a(i,j)-2.0*t*a(i,r);
				}
			}
			a(r,r)=alpha;
			for (i=r+1; i<=m-1; i++)
    			a(i,r)=0.0;
		}
		for(i=r+1; i<n; i++)
			c[i] = c[i] - a(r,i)*a(r,i);
		if(r<n-1)
		{
			tw = c[r+1];
			kk = r+1;
			for(i=r+2; i<n; i++)
			{
				if(c[i]>tw)
				{
					tw = c[i];
					kk=i;
				}
			}   
			
		}
		else
			tw=-1;		
	}
    for (i=0; i<=m-2; i++)
    {
        for (j=i+1; j<=m-1;j++)
        { 
            t=q(i,j); 
			double fTemp = q(j,i);
			q(i,j)=fTemp;
            q(j,i)=t;
        }
	}
	delete []c;
    return(0);
}

int bmaqr(CMatrix<double>a, int m, int n, CMatrix<double>q, CVector<int> e)
{
   	int i,j,kk,kk1,jj,r; //nn;
    double u,alpha,w,t,tw,swap;
//产生顺序自然列，用于选主列。
	for(i=0; i<n; i++)
		e(i) = i+1;
	double *c = new double[n];
//生成m*m二维数组q，实际上是m*m单位阵,存放正交阵。
	for (i=0; i<=m-1; i++) 
    {
        for (j=0; j<=m-1; j++)
        {  
            q(i,j)=0.0;
            if (i==j) 
                q(i,j)=1.0;
		
		}
	} 



////////////////////////////////////////////////////
//求二维数组a每列平方和，存入一维数组c; nn=min(m-1,n)，其实可以删除 这段代码。
//选列主元： 算各列平方和、 列号0对应值放入tw、列号0给kk。
	for(i=0; i<n; i++)
	{
        c[i] = 0;
		for(j=0; j<m; j++)
		{
			c[i] += a(j,i)*a(j,i);
		}
	}
	
	tw=c[0];
	r=-1;
	kk=0;
//	WriteData(q,m,m,"q");
//各列平方和中最大值赋给tw,记下对应位置kk。
    for(i=1; i<n; i++)
	{
		if(c[i]>tw)
		{
			tw = c[i];
			kk=i;
		}
	}

	//kk1=-1;             //为什么要赋值-1? 好象没有必要
    while(tw>0)
	{
        r++;
// 按照列主元方法： 当kk不等与r时，e[r]与e[kk]值互换。
		if(kk !=r)		
		{
			kk1 = e(r);
		    e(r)=e(kk);
		    e(kk)=kk1;
		}
//按照列主元方法：当kk不等于r时，二维数组a第r列与第kk列互换。
		if(kk!=r)		
		{				
			for(i=0; i<m; i++)
			{
				swap = a(i,r);
				a(i,r) = a(i,kk);
				a(i,kk) = swap;
			}
//按照列主元方法：当kk不等于r时，一维数组元素c[r]和c[kk]互换。
			swap = c[r]; 
			c[r] = c[kk];
			c[kk] = swap;
		}
//WriteData(c,n,"c");
////////////////////////////////////////////
//取得二维数组a第r列中 从第r行到m行最大绝对值u。
		u=0.0; 
		for (i=r; i<=m-1; i++)
		{ 
			w=fabs(a(i,r));
			if (w>u) 
					u=w;
		}
/////////////////////////////////////
//二维数组a第r列中 从第r行到m行所有元素除以最大值u归一化后，防止alpha溢出 算出的平方和。	
		alpha=0.0;
		if(r<m && r<n)
		{			
			for (i=r; i<=m-1; i++)
			{ 
				t=a(i,r)/u; 
				alpha=alpha+t*t;
			}			
			if (a(r,r)>0.0)
				u=-u;
		}
/////////////////////////////////////	
//靠r的值计算结束，将得出的q转置后得出的为所求正交q；r也已被求出。
		else
		{

			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写  //此处改写后,测试结果更正确.
					//q[i][j]=q[j][i];
					double fTemp = q(j,i);
					q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
					q(j,i)=t;
				}
			}  
			

            return 0;
		} 
//////////////////////////////////////
//恢复归一化前的真实alpha值。假如为零，将q转置,返回被零除。
		alpha=u*sqrt(alpha);
		//if (fabs(alpha)+1.0==1.0)
		if (fabs(alpha)<=DOUBLE_ZERO)
		{ 
			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写
					//q[i][j]=q[j][i];
					double fTemp = q(j,i);
					q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
					q(j,i)=t;
				}
			}
           	delete []c;
			return EMAT_DIVBYZERO;
		}
/////////////////////////////////////////
//在知道二维数组a第r列到第m－1列取值后，求数组q在第r次变换的值。
		u=sqrt(2.0*alpha*(alpha-a(r,r)));
		if ((u+1.0)!=1.0)
		{ 
			a(r,r)=(a(r,r)-alpha)/u;
			for (i=r+1; i<=m-1; i++)
			{ 
				a(i,r)=a(i,r)/u;
			}
			for (j=0; j<=m-1; j++)
			{ 
				t=0.0;
				for (jj=r; jj<=m-1; jj++)
					t=t+a(jj,r)*q(jj,j);
				for (i=r; i<=m-1; i++)
				{
					q(i,j)=q(i,j)-2.0*t*a(i,r);
				}
			}

			WriteData(q,m,m,"q");
			WriteData(a,m,m,"a");
///////////////////////////////////////////
//求数组a在第r次变换的值。
			for (j=r+1; j<=n-1; j++)
			{ 
				t=0.0;
				for (jj=r; jj<=m-1; jj++)
					t=t+a(jj,r)*a(jj,j);
				for (i=r; i<=m-1; i++)
				{ 
					a(i,j)=a(i,j)-2.0*t*a(i,r);
				}
			}
			a(r,r)=alpha;
//数组a在第r列中，第r行以下的元素都赋值为0。			
			for (i=r+1; i<=m-1; i++)
    			a(i,r)=0.0;
		}
//更新各列平方和，并选下次主列。
		for(i=r+1; i<n; i++)
			c[i] = c[i] - a(r,i)*a(r,i);

		WriteData(q,m,m,"qelse");
		WriteData(a,m,m,"aelse");
		if(r<n-1)
		{
			tw = c[r+1];
			kk = r+1;
			for(i=r+2; i<n; i++)
			{
				if(c[i]>tw)
				{
					tw = c[i];
					kk=i;
				}
			}   
			
		}
//靠tw的值结束计算，跳出While循环。
		else
			tw=-1;		
	}
//将得出的q转置后得出的为所求正交q。
	for (i=0; i<=m-2; i++)
    {
        for (j=i+1; j<=m-1;j++)
        { 
            t=q(i,j); 
//{{{START 2003-1-24 Eric   转置改写
            //q[i][j]=q[j][i];
			double fTemp = q(j,i);
			q(i,j)=fTemp;
//}}}END  2003-1-24 Eric
            q(j,i)=t;
        }
	}
	delete []c;
    return(0);
}


double bindist(double p, int n, int k)
{
	if(k>n)
		return 0;
	
	double s = 0;
	s += log10(p)*k;
	s += log10(1.0-p)*(n-k);

	if( k>n/2 )
		k = n-k;
	if( k!=0 )
		for( int i=0; i<k; i++ )
			s += (log10(1.0*n-i)-log10(1.0*i+1));
	s = pow(10.0,s);
	return s;
}


int sphouseqr(CSparseDbMatrix &a, CSparseDbMatrix &q, CIntVector &e)
{
	CIntVector colidx, qrowidx(a.mrow()+1);
	CSparseDbMatrix b;

	int m = a.mrow();
	int n = a.mcol();
	int i,j,kk,kk1,nn,jj,r,brridx,cg;
    double u,alpha,w,t,tw,swap,brr;	

    if (m<n)
    { 
        return EMAT_ROWSLESSCOLS;
    }
	for(i=0; i<n; i++)
		e(i) = i+1;
	double *c = new double[n];
	for(i=0; i<q.vlen(); i++)
		q(i) = CTriple<double>(i,i,1.0);
    nn=n;
    if (m==n) 
          nn=m-1;
	b = a.trans();
	colidx = b.rowchidx();

	for(i=0; i<=m; i++)
        qrowidx(i) = i;

	for(i=0; i<n; i++)
	{
        c[i] = 0;
		for(j=colidx(i); j<colidx(i+1); j++)
		{
			c[i] += b(j).value()*b(j).value();
		}
	}

	tw=c[0];
	r=-1;
	kk=0;
    for(i=1; i<n; i++)
	{
		if(c[i]>tw)
		{
			tw = c[i];
			kk=i;
		}
	}
	kk1=-1;
    while(tw>0)
	{
        r++;
		if(kk !=r)
		{
			kk1 = e(r);
		    e(r)=e(kk);
		    e(kk)=kk1;
		}
		if(kk!=r)
		{
            b.rowswap(r,kk,colidx);
			swap = c[r];
			c[r] = c[kk];
			c[kk] = swap;
		}

		u=0.0; 
		for (i=colidx(r); i<colidx(r+1); i++)
        { 
			if(b(i).ypos()>=r)
			{
				w=fabs(b(i).value());
				if (w>u) 
						u=w;
			}
        }
		alpha=0.0;
        for (i=colidx(r); i<colidx(r+1); i++)
        { 
			if(b(i).ypos()>=r)
			{
                t=b(i).value()/u; 
                alpha=alpha+t*t;
			}
        }
		for (i=colidx(r); i<colidx(r+1); i++)
		{
			if(b(i).ypos()==r)
			{
				brridx = i; 
				brr = b(i).value();
				u=-u;
				break;
			}
			else if(b(i).ypos()>r)
			{
				brridx = i;
				brr = 0.0;
				break;
			}
		}
		if(i==colidx(r+1))
		{
			brridx = i;
			brr = 0.0;
		}
        alpha=u*sqrt(alpha);

		if (fabs(alpha)+1.0==1.0)
		{ 
            a = b.trans();
			return EMAT_DIVBYZERO;
		}
		u=sqrt(2.0*alpha*(alpha-brr));

		if ((u+1.0)!=1.0)
		{ 
			double temp = (brr-alpha)/u;
			if(brr +1 !=1)
			{
				if(temp +1 != 1)
				{
				    b(brridx).value() = temp;
				}
				else
				{
					b.remove(brridx);
					for(jj=r+1;jj<n+1; jj++)
						colidx(jj)--;
				}
			}
			else
			{
				if(temp +1 != 1)
				{
					CTriple<double>tp(r,r, temp);
					b.insert(brridx,tp);
					//b.insert(brridx, CTriple<double>(r,r, temp));
					for(jj=r+1;jj<n+1; jj++)
						colidx(jj)++;
				}
			}
            for (i=colidx(r); i<colidx(r+1); i++)
			{
				if(b(i).ypos()>=r+1)
				{
					b(i).value() = b(i).value()/u; 
				}
			}
            for (j=0; j<=m-1; j++)
            { 
                t=0.0;
				jj=colidx(r);
				kk=qrowidx(j);
				while(jj<colidx(r+1) && kk<qrowidx(j+1))
				{
					if(q(kk).ypos()>=r && b(jj).ypos()>=r)
					{
						if(q(kk).ypos()==b(jj).ypos())
						{
							t = t + b(jj).value()*q(kk).value();
							kk++;
							jj++;
						}
						else if(q(kk).ypos()>b(jj).ypos())
						{
							jj++;
						}
						else 
						{
                            kk++;
						}
					}
					else 
					{
						if(q(kk).ypos()<r)
							kk++;
						if(b(jj).ypos()<r)
							jj++;
					}
				} 
				if(t + 1 != 1)
				{
					int st=0;
					if(qrowidx(j) != qrowidx(j+1))
					{
						for(i=qrowidx(j); i<qrowidx(j+1); i++)
						{
							if(q(i).ypos()>=r)
							{
								st = i;
								break;
							}							
						}
					}
					else
						st=qrowidx(j);

					CSparseDbMatrix tpv1(q,j,j,r,m-1), tpv2(b,r,r,r,m-1);
                    tpv1 = tpv1 - 2.0*t*tpv2;
					for(i=0; i<tpv1.vlen(); i++)
					{
						tpv1(i).xpos() = j;
					    tpv1(i).ypos() = tpv1(i).ypos() + r;
					}
                    q.change(tpv1,st,qrowidx(j+1)-1);
					cg = tpv1.vlen() - (qrowidx(j+1)- st);
					for(i=j+1; i<m+1; i++)
						qrowidx(i) += cg;
				}
            }
			for (j=r+1; j<=n-1; j++)
			{
                t=0.0;
				jj=colidx(r);
				kk=colidx(j);
				while(jj<colidx(r+1) && kk<colidx(j+1))
				{
					if(b(kk).ypos()>=r && b(jj).ypos()>=r)
					{
						if(b(kk).ypos()==b(jj).ypos())
						{
							t = t + b(jj).value()*b(kk).value();
							kk++;
							jj++;
						}
						else if(b(kk).ypos()>b(jj).ypos())
						{
							jj++;
						}
						else 
						{
                            kk++;
						}
					}
					else
					{
						if(b(kk).ypos()<r)
							kk++;
						if(b(jj).ypos()<r)
							jj++;
					}
				} 
				if(t + 1 != 1)
				{
					int st=0;
					if(colidx(j) != colidx(j+1))
					{
						for(i=colidx(j); i<colidx(j+1); i++)
						{
							if(b(i).ypos()>=r)
							{
								st = i;
								break;
							}							
						}
					}
					else
						st=colidx(j);

					CSparseDbMatrix tpv1(b,j,j,r,m-1), tpv2(b,r,r,r,m-1);
                    tpv1 = tpv1 - 2.0*t*tpv2;
					for(i=0; i<tpv1.vlen(); i++)
					{
						tpv1(i).xpos() = j;
					    tpv1(i).ypos() = tpv1(i).ypos() + r;
					}

                    b.change(tpv1,st,colidx(j+1)-1);
					cg = tpv1.vlen() - (colidx(j+1)- st);
					for(i=j+1; i<n+1; i++)
						colidx(i) += cg;							
				}
            }
			if(alpha+1 != 1)
			{
				for(i=colidx(r); i<colidx(r+1); i++)
				{
					if(b(i).ypos()==r)
					{
						b(i).value() = alpha;
						break;
					}
					else if (b(i).ypos()>r)
					{
						CTriple<double>tp(r,r, alpha);
						b.insert(i, tp);
						//b.insert(i, CTriple<double>(r,r, alpha));
						for(jj=r+1;jj<n+1; jj++)
							colidx(jj)++;
						i++;
						break;
					}
				}
                if(i==colidx(r+1))
				{
					CTriple<double>tp(r,r, alpha);
					b.insert(i, tp);
					//b.insert(i, CTriple<double>(r,r,alpha));
					for(jj=r+1;jj<n+1; jj++)
						colidx(jj)++;
				}
			}
			for(i=colidx(r); i<colidx(r+1); i++)
			{
				if(b(i).ypos()>=r+1)
				{
				    b.remove(i);
					for(jj=r+1;jj<n+1; jj++)
						colidx(jj)--;
					i--;
				}
			}
		}
		for(i=r+1; i<n; i++)
			c[i] = c[i] - b(i,r)*b(i,r);
		if(r<n-1)
		{
			tw = c[r+1];
			kk = r+1;
			for(i=r+2; i<n; i++)
			{
				if(c[i]>tw)
				{
					tw = c[i];
					kk=i;
				}
			}   
			
		}
		else
			tw=-1;		
	}
	a = b.trans();
	delete []c;
    return(0);
}

int sphouseqr(CSparseDbMatrix &a, CSparseDbMatrix &q)
{
	CIntVector colidx, qrowidx(a.mrow()+1);
	CSparseDbMatrix b;

	int m = a.mrow();
	int n = a.mcol();
    int i,j,k,nn,jj,kk,bkkidx,cg;
    double u,alpha,w,t,bkk;
    if (m<n)
    { 
        return EMAT_ROWSLESSCOLS;
    }
	for(i=0; i<q.vlen(); i++)
		q(i) = CTriple<double>(i,i,1.0);
    nn=n;
    if (m==n) 
          nn=m-1;

	b = a.trans();
	colidx = b.rowchidx();
	for(i=0; i<=m; i++)
        qrowidx(i) = i;

    for (k=0; k<=nn-1; k++)
    { 
        u=0.0; 
        for (i=colidx(k); i<colidx(k+1); i++)
        { 
			if(b(i).ypos()>=k)
			{
				w=fabs(b(i).value());
				if (w>u) 
						u=w;
			}
        }
        alpha=0.0;
        for (i=colidx(k); i<colidx(k+1); i++)
        { 
			if(b(i).ypos()>=k)
			{
                t=b(i).value()/u; 
                alpha=alpha+t*t;
			}
        }
		for (i=colidx(k); i<colidx(k+1); i++)
		{
			if(b(i).ypos()==k)
			{
				bkkidx = i; 
				bkk = b(i).value();
				u=-u;
				break;
			}
			else if(b(i).ypos()>k)
			{
				bkkidx = i;
				bkk = 0.0;
				break;
			}
		}
		if(i==colidx(k+1))
		{
			bkkidx = i;
			bkk = 0.0;
		}
        alpha=u*sqrt(alpha);
        if (fabs(alpha)+1.0==1.0)
        { 
			a = b.trans();
            return EMAT_DIVBYZERO;
        }
        u=sqrt(2.0*alpha*(alpha-bkk));
        if ((u+1.0)!=1.0)
        { 
			double temp = (bkk-alpha)/u;
			if(bkk +1 !=1)
			{
				if(temp +1 != 1)
				{
				    b(bkkidx).value() = temp;
				}
				else
				{
					b.remove(bkkidx);
					for(jj=k+1;jj<n+1; jj++)
						colidx(jj)--;
				}
			}
			else
			{
				if(temp +1 != 1)
				{
					CTriple<double>tp(k,k,temp);
					b.insert(bkkidx, tp);

					//b.insert(bkkidx, CTriple<double>(k,k, temp));
					for(jj=k+1;jj<n+1; jj++)
						colidx(jj)++;
				}
			}
            for (i=colidx(k); i<colidx(k+1); i++)
			{
				if(b(i).ypos()>=k+1)
				{
					b(i).value() = b(i).value()/u; 
				}
			}
            for (j=0; j<=m-1; j++)
            { 
                t=0.0;
				jj=colidx(k);
				kk=qrowidx(j);
				while(jj<colidx(k+1) && kk<qrowidx(j+1))
				{
					if(q(kk).ypos()>=k && b(jj).ypos()>=k)
					{
						if(q(kk).ypos()==b(jj).ypos())
						{
							t = t + b(jj).value()*q(kk).value();
							kk++;
							jj++;
						}
						else if(q(kk).ypos()>b(jj).ypos())
						{
							jj++;
						}
						else 
						{
                            kk++;
						}
					}
					else 
					{
						if(q(kk).ypos()<k)
							kk++;
						if(b(jj).ypos()<k)
							jj++;
					}
				} 
				if(t + 1 != 1)
				{
					int st=0;
					if(qrowidx(j) != qrowidx(j+1))
					{
						for(i=qrowidx(j); i<qrowidx(j+1); i++)
						{
							if(q(i).ypos()>=k)
							{
								st = i;
								break;
							}							
						}
					}
					else
						st=qrowidx(j);

					CSparseDbMatrix tpv1(q,j,j,k,m-1), tpv2(b,k,k,k,m-1);
                    tpv1 = tpv1 - 2.0*t*tpv2;
					for(i=0; i<tpv1.vlen(); i++)
					{
						tpv1(i).xpos() = j;
					    tpv1(i).ypos() = tpv1(i).ypos() + k;
					}
                    q.change(tpv1,st,qrowidx(j+1)-1);
					cg = tpv1.vlen() - (qrowidx(j+1)- st);
					for(i=j+1; i<m+1; i++)
						qrowidx(i) += cg;
				}

            }
			for (j=k+1; j<=n-1; j++)
			{
                t=0.0;
				jj=colidx(k);
				kk=colidx(j);
				while(jj<colidx(k+1) && kk<colidx(j+1))
				{
					if(b(kk).ypos()>=k && b(jj).ypos()>=k)
					{
						if(b(kk).ypos()==b(jj).ypos())
						{
							t = t + b(jj).value()*b(kk).value();
							kk++;
							jj++;
						}
						else if(b(kk).ypos()>b(jj).ypos())
						{
							jj++;
						}
						else 
						{
                            kk++;
						}
					}
					else
					{
						if(b(kk).ypos()<k)
							kk++;
						if(b(jj).ypos()<k)
							jj++;
					}
				} 
				if(t + 1 != 1)
				{
					int st=0;
					if(colidx(j) != colidx(j+1))
					{
						for(i=colidx(j); i<colidx(j+1); i++)
						{
							if(b(i).ypos()>=k)
							{
								st = i;
								break;
							}							
						}
					}
					else
						st=colidx(j);

					CSparseDbMatrix tpv1(b,j,j,k,m-1), tpv2(b,k,k,k,m-1);
                    tpv1 = tpv1 - 2.0*t*tpv2;
					for(i=0; i<tpv1.vlen(); i++)
					{
						tpv1(i).xpos() = j;
					    tpv1(i).ypos() = tpv1(i).ypos() + k;
					}

                    b.change(tpv1,st,colidx(j+1)-1);
					cg = tpv1.vlen() - (colidx(j+1)- st);
					for(i=j+1; i<n+1; i++)
						colidx(i) += cg;			
				}
            }
			if(alpha+1 != 1)
			{
				for(i=colidx(k); i<colidx(k+1); i++)
				{
					if(b(i).ypos()==k)
					{
						b(i).value() = alpha;
						break;
					}
					else if (b(i).ypos()>k)
					{
						CTriple<double>tp(k,k, alpha);
						b.insert(i, tp);

						//b.insert(i, CTriple<double>(k,k, alpha));
						for(jj=k+1;jj<n+1; jj++)
							colidx(jj)++;
						i++;
						break;
					}
				}
                if(i==colidx(k+1))
				{
					CTriple<double>tp(k,k, alpha);
					b.insert(i, tp);

					//b.insert(i, CTriple<double>(k,k,alpha));
					for(jj=k+1;jj<n+1; jj++)
						colidx(jj)++;
				}
			}
			for(i=colidx(k); i<colidx(k+1); i++)
			{
				if(b(i).ypos()>=k+1)
				{
				    b.remove(i);
					for(jj=k+1;jj<n+1; jj++)
						colidx(jj)--;
					i--;
				}
			}
        }
    }
	a = b.trans();
	return 0;
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
	CDoubleVector qw;
	CDoubleVector vw;
	CDoubleVector gk;
	CDoubleVector w0;
	CDoubleVector pz;
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
		qw.create(2*jmax+1);
		vw.create(2*jmax+1);
		for (a=0;a<=2*jmax;a++)
		{      

			j= a-15;
			hj = h*j;
			ehj = exp(hj);
			qw[a]=q*ehj;
			vw[a]=v*(hj+0.5*(1-ehj*ehj));
		}
	}
	gk.create(2*jmax+1);
	w0.create(2*jmax+1);
	pz.create(2*jmax+1);
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
			xx=xx+pow(x,(r-1)*1.0) *exp(w0[a]);
		}
	}
	else
	{        
		CDoubleMatrix qww(2*jmax+1,2*jmax+1);
		CDoubleMatrix gkk(2*jmax+1,2*jmax+1);
		CDoubleMatrix pzz(2*jmax+1,2*jmax+1);
		CDoubleMatrix w00(2*jmax+1,2*jmax+1);
		CDoubleMatrix d(2*jmax+1,2*jmax+1);
		CDoubleMatrix vww(2*jmax+1,2*jmax+1);
		
		for (a=0;a<=2*jmax;a++)
		{
			for (b=0;b<=2*kmax;b++)
			{ 
				qww(a,b)=qw[a];
		        gkk(b,a)=gk[a];
				pzz(b,a)=pz[a];
			    w00(b,a)=w0[a];
     			vww(a,b)=vw[a];            
			}
		}
		for(a=0; a<=2*jmax;a++)
			for(b=0; b<=2*kmax;b++)
				d(a,b) =normcdf(qww(a,b)-gkk(a,b),0,1) - pzz(a,b);
		xx=0.0;
		for (a=0;a<=2*jmax;a++)
			for(b=0;b<=2*jmax;b++)
				xx=xx+exp(w00(a,b)+vww(a,b)) *pow(d(a,b),r-1.0);  
	}
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

void grp2idx(CIntVector v1, CIntVector& idxVector, CIntVector& v2)
{
	int i, j;
	
	CTArray<int, int> arrayV2;//v2排序前.        
	
	bool flag = false;
	for( i=0; i<v1.vlen(); i++ )
	{
		for( j=0; j<arrayV2.GetSize(); j++ )
		{
			if( v1(i)==arrayV2[j] )
			{
				flag = true;                                
				break;
			}
		}
		if(flag == false)
		{
			arrayV2.Add(v1(i));        
		}
		flag = false;
	}
	
	//将arrayV2排序.
	CIntVector v4(arrayV2.GetSize());
	for(i=0; i<arrayV2.GetSize(); i++)
	{
		v4(i) = arrayV2[i];
	}
	v4.Sort();
	v2 = v4;
	
	//得到idxVector.
	CIntVector v3(v1.vlen());
	for( i=0; i<v1.vlen(); i++ )
	{
		for( j=0; j<v2.vlen(); j++ )
		{
			if( v1(i)==v2(j) )
			{
				v3(i) = j+1;
				break;
			}
		}
	}
	idxVector = v3; 
}

//ytli Add @2003/01/22
void grp2idx(CDoubleVector v1, CIntVector& idxVector, CDoubleVector& v2)
{
	int i, j;
	
	CTArray<double, double> arrayV2;//v2排序前.        
	
	bool flag = false;
	for( i=0; i<v1.vlen(); i++ )
	{
		for( j=0; j<arrayV2.GetSize(); j++ )
		{
			if( fabs(v1(i)-arrayV2[j])<DOUBLE_ZERO )
			{
				flag = true;                                
				break;
			}
		}
		if(flag == false)
		{
			arrayV2.Add(v1(i));        
		}
		flag = false;
	}
	
	//将arrayV2排序.
	CDoubleVector v4(arrayV2.GetSize());
	for(i=0; i<arrayV2.GetSize(); i++)
	{
		v4(i) = arrayV2[i];
	}
	v4.Sort();
	v2 = v4;
	
	//得到idxVector.
	CIntVector v3(v1.vlen());
	for( i=0; i<v1.vlen(); i++ )
	{
		for( j=0; j<v2.vlen(); j++ )
		{
			if( fabs(v1(i)-v2(j))<DOUBLE_ZERO )
			{
				v3(i) = j+1;
				break;
			}
		}
	}
	idxVector = v3; 
}

//ytli Modify @2003/01/26
bool IsLargeFactor(CIntMatrix mat)
{
	int nTotal = 0;
	int nRow = mat.mrow();
	int nCol = mat.mcol();
	for(int i=0; i<nCol; i++)
	{
		CIntVector VecIdx;
		CIntVector VectGrp;
		grp2idx(mat.col(i),VecIdx,VectGrp);
		nTotal += VectGrp.vlen();
		if(nTotal>100||nTotal>nRow/2)
			return false;
	}
	return true;
}

//ytli Modify @2003/01/26
bool IsLargeFactor(CDoubleMatrix mat)
{
	int nTotal = 0;
	int nRow = mat.mrow();
	int nCol = mat.mcol();
	for(int i=0; i<nCol; i++)
	{
		CIntVector    VecIdx;
		CDoubleVector VectGrp;
		grp2idx(mat.col(i),VecIdx,VectGrp);
		nTotal += VectGrp.vlen();
		if(nTotal>100||nTotal>nRow/2)
			return false;
	}
	return true;
}

void lamtax(CDoubleVector lamta, int n, int k, double r, int flag)
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

double bfun(int n, int k, CDoubleVector lamta)
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

double sfun(double s, int n, int k, CDoubleVector lamta)
{
	int i;
	double  result=0;	
	for (i=0;i<=n-k-1;i++)
		result = result +lamta[i]/(1-2*lamta[i]*s);
	return result;
}



double pfun(int n, int k, double r, double s, CDoubleVector lamta)
{
	int i;
	double p=0,K0=0.0,K1=0.0,K2=0.0,K3 = 0.0,W,U;
	CDoubleVector theta(n-k);
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
	return p;
}

double pdfun(int n, int k, double r, double s, CDoubleVector lamta)
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
	CDoubleVector lamta(n-k);
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
			return r;
		}
		r = r0 - (p0 - p)/d0;
		if( fabs(r - r0)<EPS_ACCUR*fabs(r))
		{
			return r;
		}
		r0 = r;
	}
	return 0;
}

double dwlcdf(int n, int k, double r)
{
	CDoubleVector lamta(n-k);
	double s;
	lamtax(lamta,n,k,r,0);
	s=bfun(n,k,lamta);
	double p = pfun(n,k,r,s,lamta);
	return p;
}

double dwhcdf(int n, int k, double r)
{
	CDoubleVector lamta(n-k);
	double s;
	lamtax(lamta,n,k,r,1);
	s=bfun(n,k,lamta);
	double p = pfun(n,k,r,s,lamta);
	return p;
}

double dwlpdf(int n, int k, double r)
{
	CDoubleVector lamta(n-k);
	double s;
	lamtax(lamta,n,k,r,0);
	s=bfun(n,k,lamta);
	double d = pdfun(n,k,r,s,lamta);
	return d;
}

double dwhpdf(int n, int k, double r)
{
	CDoubleVector lamta(n-k);
	double s;
	lamtax(lamta,n,k,r,1);
	s=bfun(n,k,lamta);
	double d = pdfun(n,k,r,s,lamta);
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
  for(int L=0;L<=x;L++)
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
    double amax = pow(2,20.0);
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

CDoubleVector GetNormMedians(int n,double mu /*=0 */, double sigma /*= 1std */)
{
        CDoubleVector Vect_Ret(n);
        Vect_Ret[n-1] = norminv(pow(0.5,1.0/n),mu,sigma);
        Vect_Ret[0] = norminv(1-pow(0.5,1.0/n),mu,sigma);
//         TRACE("\n%.4f\n",Vect_Ret[0]);
        for(int i =1;i< n-1 ;i++)
        {
                Vect_Ret[i] = norminv((i- 0.3175)/(n+0.365),mu,sigma);
    //             TRACE("%.4f\n",Vect_Ret[i]);
        }
   //      TRACE("%.4f\n",Vect_Ret[n-1]);
        return Vect_Ret;
}
int bchol(CSymDbMatrix &a, CTriangleDbMatrix &b, double& det)
{ 
	int i,j,k;
    double d;
	int n=a.mrow();
	if (a(0,0)+1.0==1.0||a(0,0)<0.0)
	{
		b = a;
		return EMAT_NONPOSITIVEDEFINITE;
	}
	a(0,0)=sqrt(a(0,0));
	d=a(0,0);
	for (i=1; i<=n-1; i++)
	{ 
		a(0,i)=a(0,i)/a(0,0);
	}
	for (j=1; j<=n-1; j++)
	{ 
		for (k=0; k<=j-1; k++)
		{ 
			a(j,j)=a(j,j)-a(k,j)*a(k,j);
		}
		if (a(j,j)+1.0==1.0||a(j,j)<0.0)
		{ 
			b = a;
			return EMAT_NONPOSITIVEDEFINITE;
		}
		a(j,j)=sqrt(a(j,j));
		d=d*a(j,j);
		for (i=j+1; i<=n-1; i++)
		{ 
			
			for (k=0; k<=j-1; k++)
				a(j,i)=a(j,i)-a(k,i)*a(k,j);
			a(j,i)=a(j,i)/a(j,j);
		}
	}
	det=d*d;
	b = a;
	return 0;
}


int cholesky(CSymDbMatrix v, CTriangleDbMatrix &output)
{
	int i,j,k;
	int mm=v.mcol();
	double count;
	output(0,0)=::sqrt(v(0,0));
	if (output(0,0)+1.0==1.0||output(0,0)<0.0)
	{
		return EMAT_NONPOSITIVEDEFINITE;
	}
	for(i=1;i<mm;i++)
		output(0,i)=v(0,i)/output(0,0);
	for(i=1;i<mm;i++)
	{
	   for(j=i;j<mm;j++)
	   {
		   count=0.0;	
			if (i==j)
			{	 
		 		for(k=0;k<j;k++)
		 			count += output(k,i)*output(k,i);
				output(i,i)=::sqrt(v(i,i)-count);
				if (output(i,i)+1.0==1.0||output(i,i)<0.0)
				{
					return EMAT_NONPOSITIVEDEFINITE;
				}
			}
		 	
			else
			{
				for(k=0;k<i;k++)
					count+=output(k,j)*output(k,i);			
		  		output(i,j)=(v(i,j)-count)/output(i,i);
			}		  
		}
	}
	return 0;	
}

CDoubleVector cubicfit(int n, CDoubleVector x,CDoubleVector y, CDoubleVector &cubic, CDoubleVector &square, CDoubleVector &one, bool &bYes)//三次样条函数插值 2003-3-20 zfm 
{
	assert(n>2);
	int i=0,count=0; //计算每段斜率相等个数
	bYes=true;
	CDoubleVector h(n-1);
	CDoubleVector lamda(n-2);
	CDoubleVector div1(n-1); //一阶商差C
	CDoubleVector div2(n-2); //二阶商差
	CDoubleVector constant(n); //常数项
	CDoubleVector cof(n);  //最终表达式不展开时的系数，展开后化简就是各三次方程的系数
	CDoubleVector minus(n-2);
	CDoubleVector U(n-1);
	CDoubleVector V(n-1);
	CDoubleVector temp(n-1);
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
		CDoubleVector vvv;
		constant = vvv;
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
		constant[i]=(cof[i]*pow(x[i+1],3.0)-cof[i+1]*pow(x[i],3.0) 
			+6*y[i]*x[i+1]-cof[i]*h[i]*h[i]*x[i+1]
			-6*x[i]*y[i+1]+cof[i+1]*x[i]*h[i]*h[i])/(6*h[i]); //常数项
	}

	return constant;
	/*
	double aa=1.25;
	double dd=cubic[1]*pow(aa,3)+square[1]*pow(aa,2)+one[1]*pow(aa,1)+constant[1]; //测试
	for(i=0; i<n-2;i++)n  
	{
		Matrix(i,i)=2;
	}
	for(i=0; i<n-3; i++)
	{
		Matrix(i,i+1)=lamda[i];
		
		Matrix(i+1,i)=1-lamda[i+1];
	}

	CDoubleMatrix mat(n-2,1);
	
	for(i=0;i<n-2;i++)
	{
		mat(i,0)=6*div2[i];
		
	}
	WriteData(Matrix,"Matrix");
	CDoubleVector cof(n);
	cof(0)=0;
	cof(n-1)=0;
	for(i=1;i<=n-2;i++)
	{
		cof(i)=(inv(Matrix)*mat,i-1,0);
	}
	WriteData(mat,"mat");
	
	/*
	WriteData(cof,"cof"); //测试
	double aa=3.0;
	double s=cof(1)*pow((x[2]-aa),3)/(6*h[1]) +
			 cof(2)*pow((aa-x[1]),3)/(6*h[1]) +
			 (y[1]-cof(1)*h[1]*h[1]/6)*(x[2]-aa)/h[1]+
			 (y[2]-cof(2)*h[1]*h[1]/6)*(aa-x[1])/h[1];	

	*/	
	
		
	
	/*
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

	*/


}

//※※※※※※※※※※※※※※※※ old  ※※※※※※※※※※※※※※※※
#ifdef OLD_VERSION


int Matinv(double *a, int n)
{
	double y, w, run_state;
	double *c, *b;
	int tmp1, tmp2;
	int i, j, k;
	double cond = DBL_EPSILON;
	c = new double[n];
	b = new double[n];

	for(i=0; i<n; i++)
	{
		c[i] =0;
		tmp2 = i * n;
		for(j=0; j<n; j++)
		{
			c[i] += a[tmp2+j];
		}
		a[tmp2+i] -= 1;
	}
    for(k=0; k<n; k++)
	{
		tmp1 = k*n;
		for(j=0; j<n; j++)
		{
            b[j] = a[tmp1+j];
			a[tmp1+j] = 0;
		}
		a[tmp1+k] = 1;
		w = b[k] + 1;
		if(fabs(w)<=cond)
		{
             run_state = -1;
			 delete b;
			 delete c;
			 return EMAT_SINGULAR;
		}
		for(i=0; i<n; i++)
		{
			tmp2 = i*n;
			y = a[tmp2+k]/w;
			for(j=0; j<n; j++)
				a[tmp2+j] -=b[j]*y;
		}
	}
	run_state = 0;
	for(j=0; j<n;j++)
	{
		b[j] =0;
		for(i=0; i<n; i++)
			b[j] += a[i*n+j];
		run_state += c[j]*b[j];
	}
	delete b;
	delete c;
	if(fabs(run_state-n)>sqrt(cond))
		return WEMAT_SINGULAR;
	return 0;
}


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

//{{{START 2003-1-22 Eric   Change arguments
//void ppp(double **a, double *e,double *s, double **v, int m, int n)
void ppp(double** a, double *e,double *s, double ** v, int m, int n)
//}}}END  2003-1-22 Eric
{
	int i, j;
	double d;
	if(m>=n)
		i=n;
	else
		i=m;
	for(j=1;j<=i-1;j++)
	{
		a[j-1][-1] = s[j-1];
		a[j-1][j] = e[j-1];
	}
	a[i-1][j-1] = s[i-1];
	if(m<n)
		a[i-1][i]=e[i-1];
	for(i=1; i<=n-1; i++)
		for(j=i+1; j<=n; j++)
		{
			d=v[i-1][j-1];
			v[i-1][j-1] = v[j-1][i-1];
			v[j-1][i-1] = d;
		}
	return;
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
int dluav(double ** a, double ** u, double** v, int m, int n, double eps)
//}}}END  2003-1-22 Eric
{
	int i,j,k,l,ll,kk,ix,iy,mm,nn,iz,m1,ks,it,ka;
	int ix1,iy1,iz1;
	double d, dd, t, sm,sm1,em1,sk,ek,b,c,shh;
	double * fg = new double[2];
	double * cs = new double[2];

	ka = m+1;
	if(n>m)
		ka = n+1;
	double *s = new double[ka];
	double *e = new double[ka];
	double *w = new double[ka];

	it = MAX_ITERATETIME;
	k=n;
	if(m-1<n)
		k = m-1;
	l=m;
	if(n-2<m)
		l = n-2;
	if(l<0)
		l =0;
	ll = k;
	if(l>k)
		ll=l;
	if(ll>=1)
	{
        for(kk=1;kk<=ll;kk++)
		{
			if(kk<=k)
			{
				d=0.0;
				for(i=kk;i<=m;i++)
				{
					ix=i-1;
					ix1=kk-1;
					d = d +a[ix][ix1]*a[ix][ix1];
				}
				s[kk-1] = sqrt(d);
				if(s[kk-1]!=0.0)
				{
					ix = kk-1; ix1=kk-1;
					if(a[ix][ix1] != 0.0)
					{
						s[kk-1] = fabs(s[kk-1]);
						if(a[ix][ix1]<0.0)
							s[kk-1] = -s[kk-1];
					}
					for(i=kk; i<=m; i++)
					{
						a[i-1][kk-1]/=s[kk-1];
					}
					a[ix][ix1] += 1.0;
				}
				s[kk-1]= -s[kk-1];
			}
			if(n>=kk+1)
			{
				for(j=kk+1; j<=n; j++)
				{
				    if((kk<=k)&&(s[kk-1] != 0))
					{
					    d=0.0;
						for(i=kk; i<=m; i++)
						{
						    d = d+a[i-1][kk-1]*a[i-1][j-1];
						}
						d = -d/a[kk-1][kk-1];
						for(i=kk; i<=m; i++)
						{
						    a[i-1][j-1] += d*a[i-1][kk-1];
						}
					}
					e[j-1] = a[kk-1][j-1];
				}
			}
			if(kk<=k)
			{
			    for(i=kk; i<=m; i++)
				{
				    u[i-1][kk-1] = a[i-1][kk-1];
				}
			}
			if(kk<=l)
			{
			    d =0.0;
				for(i=kk+1; i<=n; i++)
				    d = d+ e[i-1]*e[i-1];
				e[kk-1] = sqrt(d);
				if(e[kk-1] != 0.0)
				{
				    if(e[kk] != 0.0)
					{
					    e[kk-1] = fabs(e[kk-1]);
						if(e[kk]<0.0)
							e[kk-1] = -e[kk-1];
					}
					for(i= kk+1; i<=n; i++)
						e[i-1] = e[i-1]/e[kk-1];
					e[kk] = 1 +e[kk];
				}
				e[kk-1] = -e[kk-1];
				if((kk+1)<=m && (e[kk-1]!= 0.0))
				{
					for(i=kk+1; i<=m; i++)
						w[i-1] = 0.0;
					for(j=kk+1; j<=n; j++)
						for(i=kk+1; i<=m; i++)
							w[i-1] = w[i-1] + e[j-1]*a[i-1][j-1];
					for(j=kk+1; j<=n; j++)
						for(i=kk+1; i<=m; i++)
							a[i-1][j-1] -= w[i-1]*e[j-1]/e[kk];
				}
				for(i=kk+1; i<=n; i++)
					v[i-1][kk-1] = e[i-1];
			}
		}
	}
	mm = n;
	if(m+1<n)
	    mm = m +1;
	if(k<n)
		s[k] = a[k][k];
	if(m<mm)
		s[mm-1] = 0.0;
	if(l+1 <mm)
	    e[l] = a[l][mm-1];
	e[mm - 1] =0;
	nn = m;
	if(m>n)
		nn=m;
	if(nn>=k+1)
	{
		for(j=k+1; j<=nn; j++)
		{
			for(i=1; i<=m; i++)
				u[i-1][j-1] = 0;
			u[j-1][j-1] = 1;
		}
	}
	if( k>=1)
	{
		for(ll=1; ll<=k; ll++)
		{
			kk = k-ll+1;
			iz = kk-1; iz1 = kk-1;
			if(s[kk-1] != 0)
			{
				if(nn>=kk+1)
					for(j=kk+1; j<=nn; j++)
					{
						d = 0.0;
						for(i = kk; i<= m; i++)
						{
							ix = i -1;
							d += u[ix][kk -1]*u[ix][j-1]/u[iz][iz1];
						}
						d = -d; 
						for(i = kk; i<=m; i++)
						{
							ix = i-1;
							u[ix][j-1] += d*u[ix][kk-1];
						}
					}
					for(i=kk; i<=m; i++)
					{
						ix = i-1;
						ix1 = kk -1;
						u[ix][ix1] = -u[ix][ix1];
					}
					u[iz][iz1] = 1+ u[iz][iz1];
					if(kk-1 >=1)
						for(i = 1; i<=kk-1; i++)
							u[i-1][kk-1] = 0;
			}
			else
			{
				for(i=1; i<=m; i++)
					u[i-1][kk-1] =0;
				u[kk-1][kk-1] = 1;
			}
		}
	}
	for(ll = 1; ll<=n; ll++)
	{
		kk = n-ll+1;
		iz = kk; iz1 = kk -1;
		if((kk<=l)&&(e[kk-1]!=0))
		{
			for(j=kk+1; j<=n; j++)
			{
				d = 0;
				for(i =kk+1; i<=n; i++)
				{
					ix = i - 1; 
					d += v[ix][kk-1]*v[ix][j-1]/v[iz][iz1];
				}
				d = -d;
				for(i = kk+1; i<=n; i++)
				{
					ix = i-1;
					v[ix][j-1] += d*v[ix][kk -1];
				}
			}
		}
		for(i=1; i<=n; i++)
			v[i-1][kk-1] = 0;
		v[iz-1][iz1] = 1;
	}
	for(i=1; i<=m; i++)
		for(j=1; j<=n; j++)
			a[i-1][j-1] = 0;
	m1 = mm;
	it = MAX_ITERATETIME;
	while(1==1)
	{
		if(mm == 0)
		{
			ppp(a,e,s,v,m,n);
			delete s; 
			delete e;
			delete w;
			delete fg;
			delete cs;
			return 0;
		}
		if(it == 0)
		{
			ppp(a,e,s,v,m,n);
			delete s;
			delete e;
			delete w;
			delete fg;
			delete cs;
			return EMAT_OVERITERATE;
		}
		kk = mm -1;
		while((kk!=0)&&(fabs(e[kk-1])!=0))
		{
			d = fabs(s[kk-1]) + fabs(s[kk]);
			dd = fabs(e[kk-1]);
			if(dd > eps*d)
				kk = kk -1;
			else
				e[kk -1] = 0;
		}
		if(kk == mm -1)
		{
			kk = kk+1;
			if(s[kk-1]<0)
			{
				s[kk-1] = -s[kk-1];
				for(i = 1; i<=n; i++)
				{
					ix = i-1; ix1 = kk-1;
					v[ix][ix1] = -v[ix][ix1];
				}
			}
			while((kk != m1) &&(s[kk-1]<s[kk]))
			{
				d = s[kk-1];
				s[kk-1] = s[kk];
				s[kk] = d;
				if(kk<n)
				{
					for(i=1; i<=n; i++)
					{
						ix = i -1;
						ix1 = kk -1;
						iy = i -1;
						iy1 = kk;
						d = v[ix][ix1];
						v[ix][ix1] = v[iy][iy1];
						v[iy][iy1] = d;
					}
				}
				if(kk<m)
				{
					for(i=1; i<=m; i++)
					{
						ix = i-1;
						ix1 = kk -1;
						iy = i-1;
						iy1 = kk;
						d = u[ix][ix1];
						u[ix][ix1] = u[iy][iy1];
						u[iy][iy1] = d;
					}
				}
				kk = kk +1;
			}
			it = MAX_ITERATETIME;
			mm = mm-1;
		}
		else
		{
			ks = mm;
			while((ks>kk)&&(fabs(s[ks-1])!=0))
			{
				d = 0;
				if(ks != mm)
					d = d+fabs(e[ks-1]);
				if(ks != kk+1)
					d = d+fabs(e[ks-2]);
				dd = fabs(s[ks -1]);
                if(dd>eps*d)
					ks = ks -1;
				else
					s[ks-1] = 0;
			}
			if(ks == kk)
			{
				kk =kk+1;
				d = fabs(s[mm -1]);
				t = fabs(s[mm -2]);
				if(t>d)
					d=t;
				t = fabs(e[mm-2]);
				if(t>d)
					d = t;
				t = fabs(s[kk-1]);
				if(t>d)
					d = t;
				t = fabs(e[kk -1]);
				if(t>d)
					d = t;
				sm = s[mm -1]/d;
				sm1 = s[mm-2]/d;
				em1 = e[mm -2]/d;
				sk = s[kk-1]/d;
				ek = e[kk-1]/d;
				b = ((sm1+sm)*(sm1-sm) + em1*em1)/2.0;
				c = sm*em1;
				c = c*c;
				shh =0;
				if((b!=0)||(c!=0))
				{
					shh = sqrt(b*b+c);
					if(b<0)
						shh = -shh;
					shh = c/(b+shh);
				}
				fg[0] = (sk +sm)*(sk-sm) - shh;
				fg[1] = sk * ek;
				for(i = kk; i<= mm-1; i++)
				{
					sss(fg,cs);
					if(i!=kk)
						e[i-2] = fg[0];
					fg[0] = cs[0]*s[i-1] + cs[1]*e[i-1];
					e[i-1] = cs[0]*e[i-1] - cs[1]*s[i-1];
					fg[1] = cs[1]*s[i];
					s[i] = cs[0]*s[i];
					if((cs[0] != 1)||(cs[1] != 0))
						for(j=1; j<=n; j++)
						{
							ix = j-1;
							ix1 = i-1;
							iy = j-1;
							iy1 =i;
							d = cs[0]*v[ix][ix1] +cs[1]*v[iy][iy1];
							v[iy][iy1] = -cs[1]*v[ix][ix1] + cs[0]*v[iy][iy1];
							v[ix][ix1] = d;
						}
					sss(fg,cs);
					s[i-1] = fg[0];
					fg[0] = cs[0]*e[i-1] +cs[1]*s[i];
					s[i] = -cs[1]*e[i-1] + cs[0]*s[i];
					fg[1] = cs[1]*e[i];
					e[i] = cs[0]*e[i];
					if(i<m)
						if((cs[0]!=1)||(cs[1] !=0))
							for(j=1; j<=m; j++)
							{
								ix=j-1;
								ix1 = i-1;
								iy = j-1;
								iy1 = i;
							    d = cs[0]*u[ix][ix1] +cs[1]*u[iy][iy1];
							    u[iy][iy1] = -cs[1]*u[ix][ix1] + cs[0]*u[iy][iy1];
						  	    u[ix][ix1] = d;
							}
				}
				e[mm-2] = fg[0];
				it = it -1;
			}
			else
			{
				if(ks == mm)
				{
					kk = kk +1;
					fg[1] = e[mm -2];
					e[mm-2] = 0;
					for(ll = kk; ll<=mm-1;ll++)
					{
						i=mm+kk-ll-1;
						fg[0]=s[i-1];
						sss(fg,cs);
						s[i-1] = fg[0];
						if(i != kk)
						{
							fg[1]= -cs[1]*e[i-2];
							e[i-2] = cs[0] *e[i-2];
						}
						if((cs[0] !=1)||cs[1] != 0)
							for(j=1; j<=n; j++)
							{
								ix = j-1;
								ix1 = i-1;
								iy = j-1;
								iy1 = mm-1;
								d = cs[0]*v[ix][ix1] +cs[1]*v[iy][iy1];
				                v[iy][iy1] = -cs[1]*v[ix][ix1] + cs[0]*v[iy][iy1];
							    v[ix][ix1] = d;
							}
					}
				}
				else
				{
					kk = ks +1;
					fg[1] = e[kk -2];
					e[kk-2] = 0;
					for(i = kk; i<=mm;i++)
					{
						fg[0]=s[i-1];
						sss(fg,cs);
						s[i-1] = fg[0];
						fg[1]= -cs[1]*e[i-1];
						e[i-1] = cs[0] *e[i-1];

						if((cs[0] !=1)||cs[1] != 0)
							for(j=1; j<=m; j++)
							{
								ix = j-1;
								ix1 = i-1;
								iy = j-1;
								iy1 = kk-2;
								d = cs[0]*u[ix][ix1] +cs[1]*u[iy][iy1];
				                u[iy][iy1] = -cs[1]*u[ix][ix1] + cs[0]*u[iy][iy1];
							    u[ix][ix1] = d;
							}
					}
				}
			}
		}
	}
}





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


int sphqr2(double ep, CSparseDbMatrix &h, CSparseDbMatrix &v, double *wr, double *wi, int *cnt)
{
	int i,j,k,l,m,na,its,en,nl,la,low,upp;
	double p,q,r,s,t,w,x,y,z,ra,sa,vr,vi,norm,tmp;
	int n=h.mrow();
	div_t divmess;
	low = 1;
	upp = n;
	en = upp;
	t = 0;
	CIntVector hridx = h.rowchidx();
	CIntVector vridx = v.rowchidx();
	while(en>=low)
	{
		its =0;
		na = en -1;
		while(1)
		{
			la =0;
			for(l=en; l>=low+1; l--)
			{
				double h0 = h(l-1,l-2,hridx);
				double h1 = h(l-2,l-2,hridx);
				double h2 = h(l-1,l-1,hridx);
				if(fabs(h0)<=ep*(fabs(h1)+fabs(h2)))
				{
					la =1;
					break;
				}
			}
			if(!la)
				l = low;
			x = h(en-1,en-1,hridx);
			if(l == en)
			{
				la = 0;
				break;
			}

			y = h(na-1,na-1,hridx);
			w = h(en-1,na-1,hridx)*h(na-1,en-1,hridx);
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
				if(x+1!=1)
				{
					for(i=low; i<=en; i++)
						h.set(i-1,i-1,hridx) -=x;
				}
				s = fabs(h(en-1,na-1,hridx))+fabs(h(na-1,en-3,hridx));
				x = y =0.75*s;
				w = -0.4375*s*s;
			}
			its += 1;

			for(m = en-2; m>=1; m--)//????
			{

				z=h(m-1,m-1,hridx);
				r=x-z;
				s=y-z;
				if(fabs(h(m,m-1,hridx))<ep)
				{
					return EMAT_DIVBYZERO;
				}
				p = (r*s-w)/h(m,m-1,hridx) + h(m-1,m,hridx);
				q = h(m,m,hridx)-z-r-s;
				r = h(m+1,m,hridx);
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
				if(fabs(h(m-1,m-2,hridx))*(fabs(q)+fabs(r)) <= ep*fabs(p)*
					(fabs(h(m-2,m-2,hridx)) + fabs(z) + fabs(h(m,m,hridx))))
					break;
			}
			for(i=m+2;i<=en; i++)
				h.set(i-1,i-3,hridx,0);
			for(i=m+3;i<=en; i++)
				h.set(i-1,i-4,hridx,0);
			for(k=m; k<=na; k++)
			{
				if(k != na)
					nl =1;
				else
					nl =0;
				if(k!=m)
				{
					p = h(k-1,k-2,hridx);
					q = h(k,k-2,hridx);
					if(nl)
						r = h(k+1,k-2,hridx);
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
					h.set(k-1,k-2,hridx, -s*x);
				else if(l!=m)
				{
					for(i=hridx(k-1); i<hridx(k); i++)
					{
						if(h(i).ypos()==k-2)
						{
							h(i).value() = -h(i).value();
							break;
						}
						else if(h(i).ypos()>k-2)
							break;
					}
				}
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
					p=h(k-1,j-1,hridx) + q*h(k,j-1,hridx);
					if(nl)
					{
						p +=r*h(k+1,j-1,hridx);
						if(p+1!=1)
						    h.set(k+1,j-1,hridx) -= p*z;
					}
					if(p+1!=1)
					{
						h.set(k,j-1,hridx) -= p*y;
						h.set(k-1,j-1,hridx) -=p*x;
					}
				}
				if(k+3 <en)
					j = k+3;
				else
					j=en;
				for(i=1; i<=j; i++)
				{
					p = x*h(i-1,k-1,hridx) +y*h(i-1,k,hridx);
					if(nl)
					{
						p += z*h(i-1,k+1,hridx);
						if(p+1!=1)
						    h.set(i-1,k+1,hridx) -= p*r;
					}
					if(p+1!=1)
					{
						h.set(i-1,k,hridx) -=p*q;
						h.set(i-1,k-1,hridx) -=p;
					}
				}
				for(i=low; i<=upp; i++)
				{
					p = x*v(i-1,k-1,vridx) + y*v(i-1,k,vridx);
					if(nl)
					{
						p += z*v(i-1,k+1,vridx);
						if(p+1!=1)
						    v.set(i-1,k+1,vridx) -=p*r;
					}
					if(p+1!=1)
					{
					    v.set(i-1,k,vridx) -= p*q;
					    v.set(i-1,k-1,vridx) -= p;
					}
				}
			}
		}

		if(! la)
		{
			wr[en-1]= x+t; 
			h.set(en -1,en -1, hridx, x+t);
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
        x = x+t;
		h.set(en-1,en-1,hridx,x);
		h.set(na-1,na-1,hridx,y+t);
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
			x = h(en-1,na-1,hridx);
			r = sqrt(x*x + z*z);
			if(r<ep)
			{
				return EMAT_DIVBYZERO;
			}
			p= x/r;
			q = z/r;

			for(j=na; j<=n; j++)
			{
				z= h(na-1,j-1,hridx);
				tmp = h(en-1,j-1,hridx);
				h.set(na-1,j-1,hridx) = q*z + p*tmp;
				h.set(en-1,j-1,hridx) = q*tmp - p*z;
			}
			for(i=1; i<=en; i++)
			{
				z= h(i-1,na-1,hridx);
				tmp = h(i-1,en-1,hridx);
				h.set(i-1,na-1,hridx) = q*z + p*tmp;
				h.set(i-1,en-1,hridx) = q*tmp - p*z;
			}
			for(i=low; i<=upp; i++)
			{
				z = v(i-1,na-1,vridx);
				tmp = v(i-1,en-1,vridx);
				v.set(i-1,na-1,vridx) = q*z + p*tmp;
				v.set(i-1,en-1,vridx) = q*tmp - p*z;
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
		for(j=hridx(i-1); j<hridx(i); j++)
		{
			if(h(j).ypos()>=i-1)
				norm += fabs(h(j).value());
        }
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

			h.set(en-1,en-1,hridx) =1;
			for(i=na; i>=1; i--)
			{
				w= h(i-1,i-1,hridx)-p;
				r= h(i-1,en-1,hridx);
				for(j=m; j<=na; j++)
					r += h(i-1,j-1,hridx)*h(j-1,en-1,hridx);
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
							h.set(i-1,en-1,hridx) = -r/w;
						else
							h.set(i-1,en-1,hridx) = -r/(ep*norm);
					}
					else
					{
						x = h(i-1,i,hridx);
						y = h(i,i-1,hridx);
						q = (wr[i-1] - p)*(wr[i-1] -p) + wi[i-1]*wi[i-1];
						if((q<ep)||(fabs(x)<ep)||(fabs(z)<ep))
						{
							return EMAT_DIVBYZERO;
						}
						h.set(i-1,en-1,hridx) = t = (x*s-z*r)/q;
						if(fabs(x) > fabs(z))
							h.set(i,en-1,hridx) = (-r-w*t)/x;
						else
							h.set(i,en-1,hridx)= (-s-y*t)/z;
					}
				}
			}
		}
		else
		{
			if(q<0)
			{
				m = na;
				tmp = fabs(h(en-1,na-1,hridx));
				if(tmp>fabs(h(na-1,en-1,hridx)))
				{
					if(tmp<ep)
					{
						return EMAT_DIVBYZERO;
					}
					h.set(na-1,na-1,hridx) = -(h(en-1,en-1,hridx)-p)/h(en-1,na-1,hridx);
					h.set(na-1,en-1,hridx) = -q/h(en-1,na-1,hridx);
				}
				else
					cdiv(-h(na-1,en-1,hridx),0,h(na-1,na-1,hridx)-p,q, &h.set(na-1,na-1,hridx), &h.set(na-1,en-1,hridx));
				h.set(en-1,na-1,hridx) = 1;
				h.set(en-1,en-1,hridx,0);
				for(i=na-1; i>=1; i--)
				{
					w = h(i-1,i-1,hridx) -p;
					ra = h(i-1,en-1,hridx);
					sa =0;
					for(j=m; j<=na; j++)
					{
						ra += h(i-1,j-1,hridx)*h(j-1,na-1,hridx);
						sa += h(i-1,j-1,hridx)*h(j-1,en-1,hridx);
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
							cdiv(-ra, -sa, w,q,&h.set(i-1,na-1,hridx), &h.set(i-1,en-1,hridx));
						else
						{
							x=h(i-1,i,hridx);
							y = h(i,i-1,hridx);
							vr = (wr[i-1]-p)*(wr[i-1]-p) +wi[i-1]*wi[i-1] -q*q;
							vi =(wr[i-1] -p)*2*q;
							if((fabs(vr)<=ep)&&(fabs(vi)<=ep))
								vr = ep*norm*(fabs(w) + fabs(q) +fabs(x) + fabs(y) + fabs(z));
							cdiv(x*r - z*ra + q*sa, x*s-z*sa -q*ra, vr, vi,&h.set(i-1,na-1,hridx), &h.set(i-1,en-1,hridx));
							if(fabs(x)>(fabs(z)+fabs(q)))
							{
								if(fabs(x)<ep)
								{
									return EMAT_DIVBYZERO;
								}
								h.set(i,na-1,hridx) = (-ra-w*h(i-1,na-1,hridx) + q*h(i-1,en-1,hridx))/x;
								h.set(i,en-1,hridx) = (-sa -w*h(i-1,en-1,hridx) - q*h(i-1,na-1,hridx))/x;
							}
							else
								cdiv(-r-y*h(i-1,na-1,hridx), -s-y*h(i-1,en-1,hridx), z,q, &h.set(i,na-1,hridx), &h.set(i,en-1,hridx));
						}
					}
				}
			}
		}
	}
	for(i=1; i<=low-1; i++)
		for(j=i+1; j<=n; j++)
			v.set(i-1,j-1,vridx) = h(i-1,j-1,hridx);
	for(i=upp+1; i<=n; i++)
		for(j=i+1; j<=n; j++)
			v.set(i-1,j-1,vridx) = h(i-1,j-1,hridx);
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
					y += v(i-1,k-1,vridx)*h(k-1,l-1,hridx);
					z += v(i-1,k-1,vridx)*h(k-1,j-1,hridx);
				}											
				v.set(i-1,l-1,vridx)=y;
				v.set(i-1,j-1,vridx)=z;
			}

		}		
		else if(fabs(wi[j-1])<ep)
		{
			for(i=low;i<=upp; i++)
			{
				z=0;
				for(k=low; k<=m;k++)
					z+=v(i-1,k-1,vridx)*h(k-1,j-1,hridx);
				v.set(i-1,j-1,vridx)=z;
			}
		}
	}
	return 0;
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


int bmaqr1(CMatrix<double> a, int m, int n, CMatrix<double> q, int *e)
{
    int i,j,kk,kk1,nn,jj,r;
    double u,alpha,w,t,tw,swap;
	
	assert(m>=n); //zfm
	for(i=0; i<n; i++)
		e[i] = i+1;
	CDoubleVector c(n);
    for (i=0; i<=m-1; i++)
    {
        for (j=0; j<=m-1; j++)
        {  
            q(i,j)=0.0;
            if (i==j) 
                q(i,j)=1.0;
		}
	}
    nn=n;
    if (m==n) 
          nn=m-1;
	for(i=0; i<n; i++)
	{
        c[i] = 0;
		for(j=0; j<m; j++)
		{
			c[i] += a(j,i)*a(j,i);
		}
	}
	tw=c[0];
	r=-1;
	kk=0;
    for(i=1; i<n; i++)
	{
		if(c[i]>tw)
		{
			tw = c[i];
			kk=i;
		}
	}
	kk1=-1;
    while(tw>0)
	{
        r++;
		if(kk !=r)
		{
			kk1 = e[r];
		    e[r]=e[kk];
		    e[kk]=kk1;
		}
		if(kk!=r)
		{
			for(i=0; i<m; i++)
			{
				swap = a(i,r);
				a(i,r) = a(i,kk);
				a(i,kk) = swap;
			}
			swap = c[r];
			c[r] = c[kk];
			c[kk] = swap;
		}
		u=0.0; 
		for (i=r; i<=m-1; i++)
		{ 
			w=fabs(a(i,r));
			if (w>u) 
					u=w;
		}
		alpha=0.0;
		for (i=r; i<=m-1; i++)
		{ 
			t=a(i,r)/u; 
			alpha=alpha+t*t;
		}
		if (a(r,r)>0.0)
				u=-u;
		alpha=u*sqrt(alpha);
		if (fabs(alpha)+1.0==1.0)
		{ 
			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
					double fTemp = q(j,i);
					q(i,j)=fTemp;
					q(j,i)=t;
				}
			}
			return EMAT_DIVBYZERO;
		}
		u=sqrt(2.0*alpha*(alpha-a(r,r)));
		if ((u+1.0)!=1.0)
		{ 
			a(r,r)=(a(r,r)-alpha)/u;
			for (i=r+1; i<=m-1; i++)
			{ 
				a(i,r)=a(i,r)/u;
			}
			for (j=0; j<=m-1; j++)
			{ 
				t=0.0;
				for (jj=r; jj<=m-1; jj++)
					t=t+a(jj,r)*q(jj,j);
				for (i=r; i<=m-1; i++)
				{
					q(i,j)=q(i,j)-2.0*t*a(i,r);
				}
			}
			for (j=r+1; j<=n-1; j++)
			{ 
				t=0.0;
				for (jj=r; jj<=m-1; jj++)
					t=t+a(jj,r)*a(jj,j);
				for (i=r; i<=m-1; i++)
				{ 
					a(i,j)=a(i,j)-2.0*t*a(i,r);
				}
			}
			a(r,r)=alpha;
			for (i=r+1; i<=m-1; i++)
    			a(i,r)=0.0;
		}
		for(i=r+1; i<n; i++)
			c[i] = c[i] - a(r,i)*a(r,i);
		if(r<n-1)
		{
			tw = c[r+1];
			kk = r+1;
			for(i=r+2; i<n; i++)
			{
				if(c[i]>tw)
				{
					tw = c[i];
					kk=i;
				}
			}   
			
		}
		else
			tw=-1;		
	}
    for (i=0; i<=m-2; i++)
    {
        for (j=i+1; j<=m-1;j++)
        { 
            t=q(i,j); 
			double fTemp = q(j,i);
			q(i,j)=fTemp;
            q(j,i)=t;
        }
	}
	delete []c;
    return(0);
}

//选列主元QR分解 zfm
int bmaqr(CMatrix<double>a, int m, int n, CMatrix<double>q, int *e)
{
   	int i,j,kk,kk1,jj,r; //nn;
    double u,alpha,w,t,tw,swap;
////////////////////////////////////////////////////
//产生顺序自然列，用于选主列。
	for(i=0; i<n; i++)
		e[i] = i+1;
	CDoubleVector c(n);
//WriteData(a,m,n,"a");
/////////////////////////////////////////////////////  
//生成m*m二维数组q，实际上是m*m单位阵,存放正交阵。
	for (i=0; i<=m-1; i++) 
    {
        for (j=0; j<=m-1; j++)
        {  
            q(i,j)=0.0;
            if (i==j) 
                q(i,j)=1.0;
		
		}
	} 
////////////////////////////////////////////////////
//求二维数组a每列平方和，存入一维数组c; nn=min(m-1,n)，其实可以删除 这段代码。
    
	//nn=n;
	//if (m==n) 
    //   nn=m-1;
//选列主元： 算各列平方和、 列号0对应值放入tw、列号0给kk。
	for(i=0; i<n; i++)
	{
        c[i] = 0;
		for(j=0; j<m; j++)
		{
			c[i] += a(j,i)*a(j,i);
		}
	}
	
	tw=c[0];
	r=-1;
	kk=0;
//	WriteData(q,m,m,"q");
//各列平方和中最大值赋给tw,记下对应位置kk。
    for(i=1; i<n; i++)
	{
		if(c[i]>tw)
		{
			tw = c[i];
			kk=i;
		}
	}

	//kk1=-1;             //为什么要赋值-1? 好象没有必要
    while(tw>0)
	{
        r++;
// 按照列主元方法： 当kk不等与r时，e[r]与e[kk]值互换。
		if(kk !=r)		
		{
			kk1 = e[r];
		    e[r]=e[kk];
		    e[kk]=kk1;
		}
//按照列主元方法：当kk不等于r时，二维数组a第r列与第kk列互换。
		if(kk!=r)		
		{				
			for(i=0; i<m; i++)
			{
				swap = a(i,r);
				a(i,r) = a(i,kk);
				a(i,kk) = swap;
			}
//按照列主元方法：当kk不等于r时，一维数组元素c[r]和c[kk]互换。
			swap = c[r]; 
			c[r] = c[kk];
			c[kk] = swap;
		}
//WriteData(c,n,"c");
////////////////////////////////////////////
//取得二维数组a第r列中 从第r行到m行最大绝对值u。
		u=0.0; 
		for (i=r; i<=m-1; i++)
		{ 
			w=fabs(a(i,r));
			if (w>u) 
					u=w;
		}
/////////////////////////////////////
//二维数组a第r列中 从第r行到m行所有元素除以最大值u归一化后，防止alpha溢出 算出的平方和。	
		alpha=0.0;
		if(r<m && r<n)
		{			
			for (i=r; i<=m-1; i++)
			{ 
				t=a(i,r)/u; 
				alpha=alpha+t*t;
			}			
			if (a(r,r)>0.0)
				u=-u;
		}
/////////////////////////////////////	
//靠r的值计算结束，将得出的q转置后得出的为所求正交q；r也已被求出。
		else
		{

			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
					double fTemp = q(j,i);
					q(i,j)=fTemp;
					q(j,i)=t;
				}
			}  
			

            return 0;
		} 
//////////////////////////////////////
//恢复归一化前的真实alpha值。假如为零，将q转置,返回被零除。
		alpha=u*sqrt(alpha);
		//if (fabs(alpha)+1.0==1.0)
		if (fabs(alpha)<=DOUBLE_ZERO)
		{ 
			for (i=0; i<=m-2; i++)
			{
				for (j=i+1; j<=m-1;j++)
				{ 
					t=q(i,j); 
					double fTemp = q(j,i);
					q(i,j)=fTemp;
					q(j,i)=t;
				}
			}
           	delete []c;
			return EMAT_DIVBYZERO;
		}
/////////////////////////////////////////
//在知道二维数组a第r列到第m－1列取值后，求数组q在第r次变换的值。
		u=sqrt(2.0*alpha*(alpha-a(r,r)));
		if ((u+1.0)!=1.0)
		{ 
			a(r,r)=(a(r,r)-alpha)/u;
			for (i=r+1; i<=m-1; i++)
			{ 
				a(i,r)=a(i,r)/u;
			}
			for (j=0; j<=m-1; j++)
			{ 
				t=0.0;
				for (jj=r; jj<=m-1; jj++)
					t=t+a(jj,r)*q(jj,j);
				for (i=r; i<=m-1; i++)
				{
					q(i,j)=q(i,j)-2.0*t*a(i,r);
				}
			}

			WriteData(q,m,m,"q");
			WriteData(a,m,m,"a");
///////////////////////////////////////////
//求数组a在第r次变换的值。
			for (j=r+1; j<=n-1; j++)
			{ 
				t=0.0;
				for (jj=r; jj<=m-1; jj++)
					t=t+a(jj,r)*a(jj,j);
				for (i=r; i<=m-1; i++)
				{ 
					a(i,j)=a(i,j)-2.0*t*a(i,r);
				}
			}
			a(r,r)=alpha;
//数组a在第r列中，第r行以下的元素都赋值为0。			
			for (i=r+1; i<=m-1; i++)
    			a(i,r)=0.0;
		}
//更新各列平方和，并选下次主列。
		for(i=r+1; i<n; i++)
			c[i] = c[i] - a(r,i)*a(r,i);

		WriteData(q,m,m,"qelse");
		WriteData(a,m,m,"aelse");
		if(r<n-1)
		{
			tw = c[r+1];
			kk = r+1;
			for(i=r+2; i<n; i++)
			{
				if(c[i]>tw)
				{
					tw = c[i];
					kk=i;
				}
			}   
			
		}
//靠tw的值结束计算，跳出While循环。
		else
			tw=-1;		
	}
//将得出的q转置后得出的为所求正交q。
	for (i=0; i<=m-2; i++)
    {
        for (j=i+1; j<=m-1;j++)
        { 
            t=q(i,j); 
			double fTemp = q(j,i);
			q(i,j)=fTemp;
            q(j,i)=t;
        }
	}
	delete []c;
    return(0);
}






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
	double p=0,*theta,K0=0.0,K1=0.0,K2=0.0,K3 = 0.0,W,U;
	theta =new double [n-k];
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



int bchol(CSymDbMatrix &a, CTriangleDbMatrix &b, double *det)
{ 
	int i,j,k;
    double d;
	int n=a.mrow();
	if (a(0,0)+1.0==1.0||a(0,0)<0.0)
	{
		b = a;
		return EMAT_NONPOSITIVEDEFINITE;
	}
	a(0,0)=sqrt(a(0,0));
	d=a(0,0);
	for (i=1; i<=n-1; i++)
	{ 
		a(0,i)=a(0,i)/a(0,0);
	}
	for (j=1; j<=n-1; j++)
	{ 
		for (k=0; k<=j-1; k++)
		{ 
			a(j,j)=a(j,j)-a(k,j)*a(k,j);
		}
		if (a(j,j)+1.0==1.0||a(j,j)<0.0)
		{ 
			b = a;
			return EMAT_NONPOSITIVEDEFINITE;
		}
		a(j,j)=sqrt(a(j,j));
		d=d*a(j,j);
		for (i=j+1; i<=n-1; i++)
		{ 
			
			for (k=0; k<=j-1; k++)
				a(j,i)=a(j,i)-a(k,i)*a(k,j);
			a(j,i)=a(j,i)/a(j,j);
		}
	}
	*det=d*d;
	b = a;
	return 0;
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
		constant[i]=(cof[i]*pow(x[i+1],3.0)-cof[i+1]*pow(x[i],3.0) 
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
		Matrix(i,i)=2;
	}
	for(i=0; i<n-3; i++)
	{
		Matrix(i,i+1)=lamda[i];
		
		Matrix(i+1,i)=1-lamda[i+1];
	}

	CDoubleMatrix mat(n-2,1);
	
	for(i=0;i<n-2;i++)
	{
		mat(i,0)=6*div2[i];
		
	}
	WriteData(Matrix,"Matrix");
	CDoubleVector cof(n);
	cof(0)=0;
	cof(n-1)=0;
	for(i=1;i<=n-2;i++)
	{
		cof(i)=(inv(Matrix)*mat,i-1,0);
	}
	WriteData(mat,"mat");
	
	/*
	WriteData(cof,"cof"); //测试
	double aa=3.0;
	double s=cof(1)*pow((x[2]-aa),3)/(6*h[1]) +
			 cof(2)*pow((aa-x[1]),3)/(6*h[1]) +
			 (y[1]-cof(1)*h[1]*h[1]/6)*(x[2]-aa)/h[1]+
			 (y[2]-cof(2)*h[1]*h[1]/6)*(aa-x[1])/h[1];	

	*/	
	
		
	
	/*
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

	*/
}

void PrintVector(CTString str,CDoubleVector Vect,int nDecimal)//打印向量内容	
{
	int len = Vect.vlen();
	CTString sTemp("");
	sTemp.Format("%%.%df\t",nDecimal);
	if(len ==0)
		return;
	else if(len <=100)
	{
		sTemp+="\n";
		printf("\n*******打印向量 %s，长度 = %d ***************\n",str.GetData(),len);
		for(int i =0;i<len;i++)
			printf(sTemp.GetData(),Vect[i]);
		printf("\n\n\n");
	}
	else if(len <=1000)
	{
		for(int i =0;i<len;i++)
		{
			if(i%100 == 0)
				printf("第%d -- %d 项 \n",i,min(len,i+100));
			printf(sTemp,Vect[i]);
		}
	}
	else
	{
		for(int i =0;i<len;i++)
		{
			if(i%500== 0)
				printf("第%d -- %d 项 \n",i,min(len,i+500));
			printf(sTemp.GetData(),Vect[i]);
		}
	}
	
}
void PrintVector(CTString str,CIntVector Vect)//打印整形 向量内容
{
	int len = Vect.vlen();
	if(len ==0)
		return;
	else if(len <=100)
	{
		printf("\n*********打印向量 %s，长度 = %d *****************\n",str.GetData(),len);
		for(int i =0;i<len;i++)
			printf("%d\n ",Vect[i]);
		printf("\n\n\n");
	}
	else if(len <=1000)
	{
		for(int i =0;i<len;i++)
		{
			if(i%100 == 0)
				printf("第%d -- %d 项 \n",i,min(len,i+100));
			printf("%d ",Vect[i]);
		}
	}
	else
	{
		for(int i =0;i<len;i++)
		{
			if(i%500 == 0)
				printf("第%d -- %d 项 \n",i,min(len,i+500));
			printf("%d    ",Vect[i]);
		}
	}
	
}	
void PrintMatrix(CTString str,CDoubleMatrix dbMatrix,int nDecimal)//Beta函数	
{
	int nRow = dbMatrix.mrow();
	int nCol = dbMatrix.mcol();
	
	if(nRow*nCol ==0)
		return;
	printf("\n*******打印矩阵 %s(%d×%d) **************\n",str.GetData(),nRow,nCol);
	CTString sTemp("");
	sTemp.Format("%%.%df	",nDecimal);
	for(int i =0 ;i< nRow; i++)
	{
		for(int j = 0; j< nCol;j++)
			printf(sTemp.GetData(),dbMatrix(i,j));
		printf("\n");
	}
}
#endif