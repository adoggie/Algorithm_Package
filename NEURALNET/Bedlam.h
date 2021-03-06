//////////////////////////////////////////////////////////////////////
// Bedlam.h                                                         //
//																	//
// Define:        随机双字浮点数产生函数定义                        //
// Creator:       屈韬                                              //
// Date:          2002.10.30                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#ifndef _BEDLAM_H_
#define _BEDLAM_H_

#define IA   16807
#define IM   2147483647
#define AM   (1.0 / IM)
#define IQ   127773
#define IR   2836
#define NTAB 32
#define NDIV (1+(IM-1) / NTAB)
#define EPS  1.2e-7
#define RNMX (1.0 - EPS)

double bedlam();
void resetRandom();

#endif