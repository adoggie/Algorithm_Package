#include "StdAfx.h"
// #include "tlAdvn.h"
#include "Covariance.h"

CCovariance::CCovariance(void)
{
}

CCovariance::~CCovariance(void)
{
}

int CCovariance::ancovan(CIntMatrix     x,//分组变量
						  CDoubleVector y,//
						  CDoubleMatrix z, //最多两列,要保证x,y,z三个变量的行数一样.
			   /*输出行:*/double& Re, CDoubleVector& B, CDoubleMatrix& RXI, CDoubleVector& SXI, CDoubleVector& FXI, CDoubleVector& PXI, CDoubleVector& MXI, CTStringArray& Name,CIntVector&dfg, int& dfe)
{
	int i,j,k;//for循环用到.
	int nx = x.mcol(), n = y.vlen(), nz = z.mcol(), nxrow = x.mrow();

    int rx = x.mrow();

    if( rx>3000)
	{;
//		return MULTIANOVA_COLERROR;
	}


	if(n==0)
	{
		return MULTIANOVA_COLERROR;
	}

	//%给出X矩阵
	CIntVector ngroups(nx);
	CIntMatrix a(nxrow, nx);
	int nn = 1;
	for( i=0; i<nx; i++ )
	{
		CIntVector gi, gn;
		grp2idx( x.col(i), gi, gn );

		ngroups(i) = gn.vlen();
		
		nn *= gn.vlen();
		
		for( j=0; j<nxrow; j++ )
		{
			a(j,i) = gi(j);
		}
	}	

//	if( nn>64000 )
//		return MULTIANOVA_OVERFLOW;
	CIntVector dfg1 = ngroups - (int)1;

	dfe = ( n-1 ) - sum(dfg1);

	int p   = n + nx - dfe;

	CDoubleMatrix xmat( nxrow, sum(ngroups)+1, 0.0 );
	for(i=0;i<a.mrow();i++)
	{
		xmat(i,0) = 1.0;
	}

	int nCurCol = 0;	
	for(i=0;i<ngroups.vlen();i++)
	{
		for(j=0;j<a.mrow();j++)
		{			
			xmat(j,nCurCol+a(j,i)) = 1.0;			
		}
		nCurCol += ngroups(i);
	}

	//调用子函数getb.
	CDoubleMatrix R;
	CDoubleVector b,Sz,Fz,Pz;
	double R02;
	
	
	getb(xmat,y,z,dfe,R,b,R02,Sz,Fz,Pz);	
	int num = 2;
	for( i=0; i<nx; i++ )
	{
		int nz = z.mcol();
		num += ngroups(i);
		CDoubleMatrix numx( xmat.mrow(), xmat.mcol()-ngroups(i) );
	
		for( j=0; j<numx.mrow(); j++ )
		{
			for( k=0; k<xmat.mcol(); k++ )
			{
				if(k<num-ngroups(i)-1)
					numx(j,k) = xmat(j,k);
				else if(k>num-2)
					numx(j,k-ngroups(i)) = xmat(j,k);
			}
		}

		CDoubleMatrix	Rx;
		CDoubleVector	xb;
		double			Rx12;
		getxb(numx, y, z, Rx, xb, Rx12);
			
		double Sx=Rx12-R02;
		double Fx;
		if(nz==1)
		{
			Fx=(Sx/dfg1(i))/(R02/(dfe-1));
			CDoubleVector temp1( 1, 1);
			if(dfe>=1)
				temp1( 0) = 1 - fcdf(Fx,dfg1(i),dfe-1);
			if(PXI.vlen()==0)
				PXI = temp1;
			else
				PXI = PXI.comvec(temp1);
		}
		else
		{
			Fx=(Sx/dfg1(i))/(R02/(dfe-nz));
			CDoubleVector temp1( 1, 1);
			if(dfe>=nz)
				temp1( 0) = 1 - fcdf(Fx,dfg1(i),dfe-nz);
			if(PXI.vlen()==0)
				PXI = temp1;
			else
				PXI = PXI.comvec(temp1);
		}

		if( RXI.mcol()==0 )
			RXI = Rx;
		else
			RXI = RXI&Rx;

		if( SXI.vlen()==0 )
			SXI = CDoubleVector(1,Sx);
		else
		{
			CDoubleVector v1 = CDoubleVector(1,Sx);
			SXI = SXI.comvec(v1);
		}

		if( FXI.vlen()==0 )
			FXI = CDoubleVector(1,Fx);
		else
		{
			CDoubleVector v2 = CDoubleVector(1,Fx);
			FXI = FXI.comvec(v2);
		}
	}
	
	if( RXI.mcol()==0 )
		RXI = R;
	else
		RXI = RXI&R;
	
	if( SXI.vlen()==0 )
	{
		CDoubleVector v1(1,R02);
		Sz  = Sz.comvec(v1);
		SXI = Sz;
	}
	else
	{
		CDoubleVector v1(1,R02);
		Sz  = Sz.comvec(v1);
		SXI = SXI.comvec(Sz);
	}
	
	if( FXI.vlen()==0 )
		FXI = Fz;
	else
		FXI = FXI.comvec(Fz);
		
	if( PXI.vlen()==0 )
		PXI = Pz;
	else
		PXI = PXI.comvec(Pz);
	
	CIntVector temp1(nz+1,1);
	temp1(nz) = dfe-nz;
	dfg = dfg1.comvec(temp1);

	CDoubleVector MXI1(SXI.vlen());
	for( i=0; i<SXI.vlen(); i++ )
		MXI1(i) = SXI(i)/dfg(i);
	
	MXI = MXI1;

	Re = R02;
	B  = b;

	return MULTIANOVA_RIGHT;
}

void CCovariance::getb(CDoubleMatrix x, CDoubleVector y, CDoubleMatrix z, int dfe,
					   /*输出行*/CDoubleMatrix& R,CDoubleVector& b,double& R02,CDoubleVector& Sz,CDoubleVector& Fz,CDoubleVector& Pz)
{
	int i,j,k;



	
	CDoubleVector vectTemp = askr(x,y);    // vectTemp  x y
	int    nz  = z.mcol();
	double R00 = sum(y*vectTemp);



	if( nz==1 )
	{
		CDoubleMatrix R1(1,2);
		R1(0,0) = sum(z.col(0)*askr(x, z.col(0)));
		R1(0,1) = sum(z.col(0)*vectTemp);

		R = R1;

		CDoubleMatrix A1( 1,1,R1(0,0) );
		CDoubleVector A2( 1,  R1(0,1) );

		b = ldiv(A1,A2);

		//%求统计值
		R02 = R00 - sum(A2*b);
		
		//%求协变量的统计值
		CDoubleVector Sz1(1);
		Sz1(0)=R00-R02;
		Sz = Sz1;
		if( fabs(R02)>1e-10 )
			Fz=Sz/(R02/(dfe-1));
		else 
			Fz = CDoubleVector(Sz.vlen(),0);

		CDoubleVector yy = fcdf(Fz,1,dfe-1);
		CDoubleVector yyy(yy.vlen());
		for( i=0; i<yy.vlen(); i++ )//?
		{
			yyy(i) = 1-yy(i);
		}
		Pz =yyy;
	}
	else// %nz大于1
	{
		CDoubleMatrix A1( nz, nz );
		CDoubleVector A2( nz );
		for( i=0; i<nz; i++ )
		{
			for( j=i; j<nz; j++)
			{
				A1(i,j) = sum(z.col(i)*askr(x,z.col(j)));
			}
			A2(i) =  sum(z.col(i)*vectTemp);
		}

		for( i=0; i<nz; i++ )
		{
			for( j=i+1; j<nz; j++)
			{
				A1(j,i) = A1(i,j);
			}
		}

		R = A1|A2;
		b = ldiv(A1,A2);
	
		R02 = R00 - sum(A2*b);
	
		//%求协变量的统计值
		//%对Zn

		CDoubleVector Sz1( nz );
		CDoubleVector Fz1( nz );
		CDoubleVector Pz1( nz );

		CDoubleMatrix Azx( nz-1, nz-1 );
		CDoubleVector Azy( nz-1 );
		
		for( k=0; k<nz; k++ )
		{
			for( i=0; i<nz; i++ )
			{
				for( j=0; j<nz; j++ )
				{
					if( i<k && j<k )
					{
						Azx(i,j) = A1(i,j);
						Azy(i)    = A2(i);
					}
					else if( i>k && j<k )
					{
						Azx(i-1,j) = A1(i,j);
						Azy(i-1)    = A2(i);
					}
					else if( i<k && j>k )
					{
						Azx(i,j-1) = A1(i,j);
						Azy(i)      = A2(i);
					}
					else if( i>k && j>k )
					{
						Azx(i-1,j-1) = A1(i,j);
						Azy(i-1)      = A2(i);
					}
				}
			}	
			
			b = ldiv(Azx,Azy);

			Sz1(k) = R00-sum(Azy*b)-R02;
			if(fabs(R02)>1e-10)
				Fz1(k) = Sz1(k)/( R02/(dfe-nz) );
			else
				Fz1(k) = 0;
             
			Pz1(k) = 1 - fcdf( Fz1(k),1,dfe-nz+1 );
		}		

		Sz = Sz1;
		Fz = Fz1;
		Pz = Pz1;
	}
}

void CCovariance::getxb(CDoubleMatrix x,CDoubleVector y,CDoubleMatrix z,
					   /*输出行*/CDoubleMatrix& Rx,CDoubleVector& xb,double& Rx12)
{
	int i,j;	

	CDoubleVector vectTemp = askr(x,y);

	int    nz  = z.mcol();
	double R00 = sum(y*vectTemp);

	if( z.mcol()==1 )
	{
	
		CDoubleMatrix R1(1,2);
		R1(0,0) = sum(z.col(0)*askr(x, z.col(0)));
		R1(0,1) = sum(z.col(0)*vectTemp); 

		Rx = R1;

		CDoubleMatrix A1(1,1,R1(0,0) );
		CDoubleVector A2(1,  R1(0,1) );

		xb = ldiv(A1,A2);
		//%求统计值
		Rx12 = R00 - sum(A2*xb);
	}
	else //nz>=2;
	{	
		CDoubleMatrix A1( nz, nz );
		CDoubleVector A2( nz );
		for( i=0; i<nz; i++ )
		{
			for( j=i; j<nz; j++)
			{
				A1(i,j) = sum(z.col(i)*askr(x,z.col(j)));
			}
			A2(i) =  sum(z.col(i)*vectTemp);
		}

		for( i=0; i<nz; i++ )
		{
			for( j=i+1; j<nz; j++)
			{
				A1(j,i) = A1(i,j);
			}
		}

		Rx = A1|A2;
		xb = ldiv(A1,A2);
		Rx12 = R00 - sum(A2*xb);
	}
}

CDoubleVector CCovariance::askr(CDoubleMatrix x, CDoubleVector y)
{
	CDoubleVector b = ldiv(x,y);

	CDoubleVector r = y - x*b;
	return r;
}
