// RandFactory.cpp: implementation of the CRandFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RandFactory.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h" 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRandFactory::CRandFactory(bool bSpecified,int nSeed)
{
	if(!bSpecified)
	{
		srand( (unsigned)time( NULL ) );
	}
	else
	{
		srand( (unsigned)nSeed );
		
	}
	
}

CRandFactory::~CRandFactory()
{
	
}


CDoubleMatrix CRandFactory::CreateExponentialRand(double lambda,int rows,int columns)
{
	//srand( (unsigned)time( NULL ) );
    double A=lambda;//lambda>0
    CDoubleMatrix r(rows,columns);
    for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=-1.0/A*log(myRand());
		}
    }
    return r;
}


CDoubleMatrix CRandFactory::CreateNormalRand(double mu,double sigma,int rows,int columns)
{
    double A=mu;
    double B=sigma;
	//srand( (unsigned)time( NULL ) );	
    CDoubleMatrix r(rows,columns);
    double* temp=new double[rows*columns+1];
    bool flag=true;
    double v1=0,v2=0,w=0,y=0,x1=0,x2=0;
    int k=0;
    for(k=0;k<rows*columns;k=k+2)
	{
		flag = true;
		while (flag) 
		{
			v1 = 2.0 * myRand() - 1;
			v2 = 2.0 * myRand() - 1;
			w = v1 * v1 + v2 * v2;
			if (w <= 1) 
			{
				y =sqrt( -2.0 *log(w) / w);
				temp[k] = v1 * y;
				temp[k+1] = v2 * y;
				flag=false;
			}
		}
    }
    //transfer to rows by columns matrix
    for(k=0;k<rows*columns;k=k+rows)
	{
		for(int j=0;j<rows;j++)
		{
			r(j,k/rows)=temp[k+j]*B+A;
		}
    }
	if(temp)
	{
		delete []temp;
		temp=NULL;
	}
    return r;
}

CDoubleMatrix CRandFactory::CreateUniformRand(double min, double max, int rows, int columns)
{
	//srand( (unsigned)time( NULL ) );
    double A = min;//min<max;
    double B = max;
    CDoubleMatrix r(rows,columns);
    for (int i = 0; i < rows; i++) 
	{
		for (int j = 0; j < columns; j++)
		{
			r(i,j) = A + (B - A) * myRand(); //r = a + (b-a) .* rand(rows,columns);
		}
    }
    return r;
	
}


//how if a==0.
CDoubleMatrix  CRandFactory::CreateGammaRand(double A, double B, int rows, int columns)
{
	//srand( (unsigned)time( NULL ) );
    CDoubleMatrix r(rows,columns);
    int lth = rows * columns;
    double c, d, e, u, v, x, y;
    if (A > 0 && A < 1) 
	{
		c = 1.0 / A;
		d = 1.0 / (1 - A);
		for (int i = 0; i < rows; i++) 
		{
			for (int j = 0; j < columns; j++) 
			{
				bool flag=true;
				while (flag) 
				{
					u = myRand();
					v = myRand();
					x = pow(u,c);
					y = pow(v,d);
					if ( (x + y) < 1 || (x + y) == 1) 
					{
						e = -log(myRand());
						r(i,j) = e * x / (x + y);
						flag=false;
					}
				}
				r(i,j) = r(i,j)/B;
			}
		}
		return r;
    }
    else if (A == 1) //equal to the exponential random
	{ 
		r=CreateExponentialRand(B,rows,columns);
		return r;
    }
    else if (A > 1)
	{
		double bb = A - 1;
		double w=0;
		c=3*A-3/4;
		//int count = 1;
		for (int i=0;i<rows;i++)
		{
			for (int j = 0; j < columns; j++) 
			{
				bool flag=true;
				while (flag) 
				{
					u = myRand();
					v = myRand();
					w = u * (1 - u);
					y = sqrt(c / w) * (u - 0.5);
					x = bb + y;
					if (x >= 0) 
					{
						double z = 64 * pow(w, 3.0) * pow(v, 2.0);
						if (z <= (1 - 2 * y * y) / x) 
						{
							r(i,j) = x/B;
							flag=false;
						}
						else if (log(z) <= 2 * (bb * log(x / bb) - y))
						{
							r(i,j) = x/B;
							flag=false;
						}
					}
				}
			}
		}
		return r;
    }
    return r;
}

CDoubleMatrix CRandFactory::CreatePoissonRand(double lambda,int rows,int columns)
{
    double A=lambda;//lambda>0
    CDoubleMatrix r(rows,columns);
    if(A>15)
	{
		double alpha=1.0*7/8;
		double m=floor(alpha*A);
		CDoubleMatrix x=CreateGammaRand(m,1,rows,columns);
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<columns;j++)
			{
				if(x(i,j)<=A)
				{
					CDoubleMatrix p=CreatePoissonRand(A-x(i,j),1,1);// call the small Posssion Random
					r(i,j)=m+p(0,0);
				}
				else
				{
					CDoubleMatrix p=CreateBinomialRand((int)m-1,A/x(i,j),1,1);
					r(i,j)=p(0,0);
				}
			}
		}
    }
    else
	{
		CDoubleMatrix p(rows,columns);
		//q=new ExpRnd(1,rows,columns).getRndValue();
		bool flag=true;
		while (flag) 
		{
			CDoubleMatrix q =CreateExponentialRand(1, rows, columns);
			flag=false;
			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < columns; j++) 
				{
					p(i,j)=p(i,j)+q(i,j);
					if (p(i,j) < A) 
					{
						r(i,j) = r(i,j) + 1;
						flag = true;
					}
				}
			}
		}
    }
	return r;
	
}

CDoubleMatrix CRandFactory::CreateBinomialRand(int trials,double prob,int rows,int columns)
{
    double A=(double)trials;//trias>0 as an int
    double B=prob;//1>prob>0
    CDoubleMatrix r(rows,columns);
    for(int n=0;n<(int)A;n++)
	{
		CDoubleMatrix u=CreateUniformRand(0,1,rows,columns);//generate uniform distribution random value
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<columns;j++)
			{
				if(u(i,j)<B)
				{
					r(i,j)=r(i,j)+1;
				}
			}
		}
    }
	return r;
}	


//A>0,B>0
CDoubleMatrix CRandFactory::CreateBetaRand(double A,double B,int rows, int columns)
{
	
    CDoubleMatrix r=CreateGammaRand(A,1,rows,columns);
    CDoubleMatrix g1=CreateGammaRand(B,1,rows,columns);
    for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=g1(i,j)/(g1(i,j)+r(i,j));
		}
    }
    return r;
}

CDoubleMatrix CRandFactory::CreateChi2Rand(int df,int rows,int columns)
{
    double A=(double)df;//df>0 as an int
    CDoubleMatrix r=CreateGammaRand(A/2,0.5,rows,columns);//H(x|n)=Gamma(x|n/2,1/2)
    return r;
}

//F 分布
CDoubleMatrix CRandFactory::CreateFRand(int df1,int df2,int rows,int columns)
{
    double A=(double)df1;//df1>0 as an int
    double B=(double)df2;//df2>0 as an int
    CDoubleMatrix r(rows,columns);
    CDoubleMatrix num=CreateChi2Rand((int)A,rows,columns);
    CDoubleMatrix den=CreateChi2Rand((int)B,rows,columns);
    for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=(num(i,j)/A)/(den(i,j)/B);//X1/m/X2/n~F(m,n)
		}
    }
	return r;
}


//t 分布
CDoubleMatrix CRandFactory::CreateTRand(int df,int rows,int columns)
{
    double A=(double)df;// df>1 as an int
	
    CDoubleMatrix r(rows,columns);
	
	//use the methods provide in the textbook 统计计算;高惠璇，北京大学出版社，1995
	CDoubleMatrix u=CreateNormalRand(0,1,rows,columns);
	CDoubleMatrix y=CreateChi2Rand((int)A,rows,columns);
	for(int i=0;i<rows;i++)
	{
		for (int j = 0; j < columns; j++)
		{
			r(i,j) = u(i,j) /sqrt(y(i,j) / A);
		}
	}
	return r;
}

//Weibull 分布
CDoubleMatrix CRandFactory::CreateWeibRand(double m,double a,int rows,int columns)
{
    double A=m;//A>0
    double B=a;//A>0
    CDoubleMatrix r;
    if(A==1.0){
		r=CreateExponentialRand(1,rows,columns);//call the exponential random
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<columns;j++)
			{
				r(i,j)=pow(r(i,j),1/B);
			}
		}
    }
    else
	{
		r=CreateUniformRand(0,1,rows,columns);//call uniform random
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<columns;j++)
			{
				r(i,j)=pow((-log(1-r(i,j)))/A,1/B);
			}
		}
    }
	return r;
}


//离散均匀分布
CDoubleMatrix CRandFactory::CreateDisUniformRand(int number,int rows,int columns)
{
    double A=(double)number;//number>0 as an int
    CDoubleMatrix r=CreateUniformRand(0,1,rows,columns);
    for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=ceil(A*r(i,j));
		}
    }
	return r;
}


//几何分布
CDoubleMatrix CRandFactory::CreateGeoRand(double prob,int rows,int columns)
{
	//srand( (unsigned)time( NULL ) );
    double A=prob;//1>prob>0
    CDoubleMatrix r(rows,columns);
    for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=myRand();
			r(i,j)=floor(log(r(i,j))/log(1-A));
		}
    }
    return r;
}


//负二项分布
CDoubleMatrix CRandFactory::CreateNBinoRand(int R,double p,int rows,int columns)
{
    double A=(double)R;//R>0 as an int
    double B=p;// 0<p<1
    CDoubleMatrix r;
    if((int)A==1)
	{
		r=CreateGeoRand(B,rows,columns);
    }
    else
	{
		CDoubleMatrix rr(rows,columns);
		r=rr;
		for(int k=0;k<(int)A;k++)
		{
			CDoubleMatrix temp=CreateGeoRand(B,rows,columns);
			for(int i=0;i<rows;i++)
			{
				for(int j=0;j<columns;j++)
				{
					r(i,j)=r(i,j)+temp(i,j);
				}
			}
		}
    }
	return r;
}



//瑞利分布
CDoubleMatrix CRandFactory::CreateRayRand(double sigma,int rows,int columns)
{
    double A=sigma;
	
    CDoubleMatrix r=CreateNormalRand(0,A,rows,columns);
    CDoubleMatrix temp=CreateNormalRand(0,A,rows,columns);
    for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=sqrt(r(i,j)*r(i,j)+temp(i,j)*temp(i,j));
		}
    }
    return r;
}




//柯西分布 </p>

CDoubleMatrix CRandFactory::CreateCauchRand(double mu,double sigma,int rows,int columns)
{
    double A=mu;//mu>0
    double B=sigma;//sigma>0
	
    CDoubleMatrix r=CreateUniformRand(0,1,rows,columns);
    for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=tan(PI*(r(i,j)-0.5))*B+A;
		}
	}
	return r;
}

//对数正态分布
CDoubleMatrix CRandFactory::CreateLogNormRand(double mu,double sigma,int rows,int columns)
{
	double A=mu;//mu>0
	double B=sigma;//sigma>0
	CDoubleMatrix r=CreateNormalRand(A,B,rows,columns);
	//in order to get the expected Ex and Var,we should transfer the original paras.
	double temp1=sqrt(A*A+B*B);
	double temp2=A;
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=exp(r(i,j));;
		}
	}
	return r;
}


//Z分布(Fisher 分布）
CDoubleMatrix CRandFactory::CreateZRand(int df1,int df2,int rows,int columns)
{
	double A=(double)df1;//df1>0 as an int
	double B=(double)df2;//df2>0 as an int
	CDoubleMatrix r=CreateFRand(df1,df2,rows,columns);
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			r(i,j)=0.5*log(r(i,j));;
		}
    }
	
	return r;
}

// 伯努利分布（0－1分布）
CDoubleMatrix CRandFactory::CreateBenollyRand(double prob,int rows,int columns)
{
    double B=prob;//1>prob>0
    CDoubleMatrix r=CreateUniformRand(0,1,rows,columns);//generate uniform distribution random value
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(r(i,j)<=B)
			{
				r(i,j)=1;
			}
			else
			{
				r(i,j)=0;
			}
		}
	}
	return r;
}

double CRandFactory::myRand()
{
	double temp=rand();
	return (temp+1)/(RAND_MAX+2);
}


/////////////////////////////////////////////////////////////////////////////
// RandomInitialize()
// 将矩阵中的元素随机初始化函数,元素的值在(-1,1)之间的小数
/////////////////////////////////////////////////////////////////////////////

CDoubleMatrix CRandFactory::CreateRandom(int row,int col)
{
	CDoubleMatrix mat; 
	for(int i=0; i < row; i++)
	{
		for(int j=0; j < col; j++)
		{
			mat(i,j) = (double)(rand() - (0.5*RAND_MAX)) / (0.5*RAND_MAX);
		}
	}
	return mat;
}

