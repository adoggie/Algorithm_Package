// RandFactory.h: interface for the CRandFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDFACTORY_H__4B8B15FB_E743_4006_98ED_C21FA4DD4F15__INCLUDED_)
#define AFX_RANDFACTORY_H__4B8B15FB_E743_4006_98ED_C21FA4DD4F15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Public/common.h"


class AFX_EXT_CLASS CRandFactory  
{
public:
	CRandFactory(bool bSpecified,int nSeed);
	virtual ~CRandFactory();

	 CDoubleMatrix CreateExponentialRand(double lambda,int rows,int columns);
	 CDoubleMatrix CreateNormalRand(double mu,double sigma,int rows,int columns);
	 CDoubleMatrix CreateUniformRand(double min, double max, int rows, int columns);
	 CDoubleMatrix CreateGammaRand(double A, double B, int rows, int columns);
	 CDoubleMatrix CreatePoissonRand(double lambda,int rows,int columns);
	 CDoubleMatrix CreateBinomialRand(int trials,double prob,int rows,int columns);
	 CDoubleMatrix CreateBetaRand(double A,double B,int rows, int columns);
	 CDoubleMatrix CreateChi2Rand(int df,int rows,int columns);
	 CDoubleMatrix CreateFRand(int df1,int df2,int rows,int columns);
	 CDoubleMatrix CreateTRand(int df,int rows,int columns);
	 CDoubleMatrix CreateWeibRand(double m,double a,int rows,int columns);
	 CDoubleMatrix CreateDisUniformRand(int number,int rows,int columns);
	 CDoubleMatrix CreateGeoRand(double prob,int rows,int columns);
	 CDoubleMatrix CreateNBinoRand(int R,double p,int rows,int columns);
	 CDoubleMatrix CreateRayRand(double sigma,int rows,int columns);
	 CDoubleMatrix CreateCauchRand(double mu,double sigma,int rows,int columns);
	 CDoubleMatrix CreateLogNormRand(double mu,double sigma,int rows,int columns);
	 CDoubleMatrix CreateZRand(int df1,int df2,int rows,int columns);
	 CDoubleMatrix CreateBenollyRand(double prob,int rows,int columns);
	 CDoubleMatrix CreateRandom(int row,int col);
private:
	 double myRand();

};

#endif // !defined(AFX_RANDFACTORY_H__4B8B15FB_E743_4006_98ED_C21FA4DD4F15__INCLUDED_)
