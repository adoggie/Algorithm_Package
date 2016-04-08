//////////////////////////////////////////////////////////////////////
// Bedlam.cpp                                                       //
//																	//
// Define:        随机双字浮点数产生函数实现                        //
// Creator:       屈韬                                              //
// Date:          2002.10.30                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Bedlam.h"

static long idum = -2500; //GetTickCount();

double bedlam()
{

	int xj;
	long xk;
	static long iy=0;
	static long iv[NTAB];
	double temp;

	if(idum <= 0 || !iy)
	{
		if(-(idum) < 1)
		{
			idum = 1 + idum;
		}
    else
    {
		idum = -(idum);
    }
    for(xj = NTAB+7; xj >= 0; xj--)
    {
		xk = (idum) / IQ;
		idum = IA * (idum - xk * IQ) - IR * xk;
		if(idum < 0)
		{
			idum += IM;
		}
		if(xj < NTAB)
			iv[xj] = idum;
    }
	iy = iv[0];
	}

	xk = idum / IQ;
	idum = IA * (idum - xk * IQ) - IR * xk;
	if(idum < 0)
		idum += IM;
    xj = iy / NDIV;
	iy = iv[xj];
	iv[xj] = idum;

	if((temp=AM*iy) > RNMX)
		return(RNMX);
	else
		return(temp);
} //随机数函数结束

void resetRandom()
{
	idum = -2500;
}