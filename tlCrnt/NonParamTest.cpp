// NonParamTest.h: interface for the CNonParamTest class.
//              非参数假设检验
//        包括：两个相关样本，多个相关样本 ，两个独立样本，多个独立样本
//              标注：余卫军 2005年 10月 21日
//				修改：马良庄		2006/03/23
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NonParamTest.h"
#include "tlcrnt.h"

#define M_E        2.71828182845904523536
#define  DOUBLE_ZERO1 1E-15

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNonParamTest::CNonParamTest()
{
	m_nGroupIndex1=-1;
	m_nGroupIndex2=-1;
	m_pResult=NULL;
	m_bKolmogorov=0;
	m_bMann=1;
	m_bMoses=0;
	m_bWald=0;

	//11,12
	m_bCruskal=1;
	m_bMedian=0;
//	m_StrPtr=NULL;
	//8,9,10
	m_bMcnema=0;
	m_bSign=0;
	m_bWilcoxon=1;
//	m_PtrArrayIndex=NULL;


	//13,14,15
	m_bCochran=0;
	m_bFriedman=1;
	m_bKendall=0;
	m_dValue1=0;//两个独立样本交换数据
	m_dValue2=1;//两个立样本交换数据
	m_dmValue1=0;//多个独立样本交换数据
	m_dmValue2=3;//多独立样本交换数据
}

CNonParamTest::~CNonParamTest()
{
	m_tVarInfo1.DeleteObject();
	m_tVarInfo2.DeleteObject();
	m_tVarInfo3.DeleteObject();
}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.6												 //
//Modifying reason :命名不规范											 //
//Function ：求二项式的值，Cnk(n,k)									     //
//Attention :要求n>k,面向小数据，不会导致数据越界；						 //
//***********************************************************************//

double CNonParamTest::Cnk(double n, double k)
{
	if(k>n)
		return -1;

	if( k>n/2 )
		k = n-k;

	if( k==0 )
		return 1;

	double d = 1;

	for( int i=0; i<k; i++ )
	{
		d *= ( n-k+i+1)/(double)(i+1);
	}
	return d;
}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.6												 //
//Modifying reason :命名不规范											 //
//Function ：求二项式的值，Cnk(n,k)									     //
//Attention :要求n>k,面向大数据，处理技巧是用对数处理，不会导致数据越界；//
//***********************************************************************//

double CNonParamTest::CnkForBigData(double n, double k)
{
	if(k>n)
		return -1;

	if( k>n/2 )
		k = n-k;

	if( k==0 )
		return 1;

	double s = 0;

	for( int i=0; i<k; i++ )
	{
		s += (log10(n-i)-log10(i+1));
	}
	return s;
}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.6												 //
//Modifying reason :命名不规范，代码臃余，增加注释						 //
//Function ：Mann-Whitney ij检验统计数据的求解						     //
//***********************************************************************//

void CNonParamTest::MannWhitneyU(CTArray<double,double> &Data1,	//组1对应的数据
								 CTArray<double,double> &Data2,	//组2对应的数据
								 CTArray<double,double> &Weight1,		//组1数据对应的权重
								 CTArray<double,double> &Weight2,		//组2数据对应的权重
								 CTString &strFieldName)			//分析数据对应的字段名
{
	int i, j;
	//1.统计样本两个的位置.
	double nCount1(0), nCount2(0);//分别统计组1、组2样本的样本数
	for(i=0; i<Weight1.GetSize(); i++)
	{
		nCount1 += Weight1[i];
	}

	for(i=0; i<Weight2.GetSize(); i++)
	{
		nCount2 += Weight2[i];
	}

	//将组1数据按从小到大进行排序，其对应的权重也须进行相应的调换。

	OrderTheGroup(Data1,Weight1);

	//将组2数据按从小到大进行排序，其对应的权重也须进行相应的调换。

	OrderTheGroup(Data2,Weight2);

	//把两个样本混合并编秩，然后把每组样本的秩相加，求出每组样本的秩和S1,S2和平均秩meanS1,
	//meanS2，修正值T ；
	double S1(0),S2(0),T(0);
	double    nCnt(0), nCnt1(0), nCnt2(0);
	for(i=0,j=0; i<Data1.GetSize()||j<Data2.GetSize(); )
	{
		//组1的索引与组2的索引均在范围之内，且Data1[i]与Data2[j]相等时
		if( (i<Data1.GetSize()&&j<Data2.GetSize())&&(fabs(Data1[i]-Data2[j])<DOUBLE_ZERO1) )
		{
			////组1样本值为Data1[i]的频数
			nCnt1 += Weight1[i];
			while( i+1<Data1.GetSize() )
			{
				if(fabs(Data1[i]-Data1[i+1])<DOUBLE_ZERO1)
				{ 
					i++;
					nCnt1 += Weight1[i];
				}
				else
					break;
			}
			i++;	
			
			////组2样本值为Data2[j]的频数			
			nCnt2 += Weight2[j];
			while( j+1<Data2.GetSize() )
			{
				if(fabs(Data2[j]-Data2[j+1])<DOUBLE_ZERO1)
				{ 
					j++;
					nCnt2 += Weight2[j];
				}
				else
					break;
			}
			j++;
			S1   += (nCnt+1+(nCnt1+nCnt2-1)/2.0)*nCnt1;//数组索引是从0开始，所以注意与文档的公式调整
			S2   += (nCnt+1+(nCnt1+nCnt2-1)/2.0)*nCnt2;	
			T    += (1.0*(nCnt1+nCnt2)*(nCnt1+nCnt2)-1)*(nCnt1+nCnt2);	
			nCnt += nCnt1+nCnt2;				
			nCnt1 = 0;
			nCnt2 = 0;
		}

		else if( (i<Data1.GetSize())&&((j==Data2.GetSize())||(Data1[i]<Data2[j])) )
		{
			////组1样本值为Data1[i]的频数
			nCnt1 += Weight1[i];
			while( i+1<Data1.GetSize() )
			{
				if(fabs(Data1[i]-Data1[i+1])<DOUBLE_ZERO1)
				{ 
					i++;
					nCnt1 += Weight1[i];
				}
				else
					break;
			}
			i++;
			S1   += (nCnt+(1+nCnt1)/2.0)*nCnt1;
			T    +=1.0*(nCnt1*nCnt1-1)*nCnt1;
			nCnt += nCnt1;
			nCnt1 = 0;
		}
		else if( (j<Data2.GetSize())&&((i==Data1.GetSize())||(Data1[i]>Data2[j])) )
		{
			////组2样本值为Data2[j]的频数
			nCnt2 += Weight2[j];
			while( j+1<Data2.GetSize() )
			{
				if(fabs(Data2[j]-Data2[j+1])<DOUBLE_ZERO1)
				{ 
					j++;
					nCnt2 += Weight2[j];
				}
				else
					break;
			}
			j++;


			S2   += (nCnt+(1+nCnt2)/2.0)*nCnt2;
			T    += 1.0*(nCnt2*nCnt2-1)*nCnt2;
			nCnt += nCnt2;
			nCnt2 = 0;
		}
	}
	double meanS1=S1/nCount1;
	double meanS2=S2/nCount2;

	double U = 1.0*nCount1*nCount2+1.0*nCount1*(nCount1+1)/2.0-S1;
	if(U>1.0*nCount1*nCount2/2.0)
		U = 1.0*nCount1*nCount2-U;
	double W = (meanS1<meanS2)?S1:S2;
	double ba = (nCount1+nCount2);	
	double Z;
	double P;
	if((nCount1*nCount2*( 1.0*ba*ba*ba-ba-T)==0)||(ba*(ba-1)==0) )
	{
		Z = 0;
		P = 1;
	}
	else
	{
		Z = (U-1.0*nCount1*nCount2/2.0)/sqrt(1.0*nCount1*nCount2*( 1.0*ba*ba*ba-ba-T)/(12.0*ba*(ba-1)));
	    P = 2*normcdf(Z,0,1);
	}


	///zkc 2002.12.5
	m_FieldNameStrAr.Add(strFieldName);
	CTString str;
	CTString strtemp;
	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nGroupIndex1);
	CTString labelVal;
	if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(m_dValue1,labelVal))
	{
		labelVal+="\n";
	}
	else
	{
		if(floor(m_dValue1)==m_dValue1)
			labelVal.Format("%.0f\n",m_dValue1);
		else
			labelVal.Format("%.2f\n",m_dValue1);
		
	}
	str=labelVal;
	
	
	CTString strTemp;
	if(!m_pDataInterface->GetShowValueLabel() ||!pField->GetValueVL(m_dValue2,labelVal))
	{
		if(floor(m_dValue2)==m_dValue2)
			labelVal.Format("%.0f",m_dValue2);
		else
			labelVal.Format("%.2f",m_dValue2);
	}
	labelVal+="\n总数";
	strTemp=labelVal;
	str+=strTemp;
	m_ClassStrAr.Add(str);
	str.Format("%.0f\n%.0f\n%.0f",nCount1,nCount2,nCount1+nCount2);
	m_NStrAr.Add(str);

	str.Empty();
	strtemp.Empty();
	strtemp.Format("%.3f",meanS1);
	str+=strtemp;
	str+="\n";
	strtemp.Format("%.3f",meanS2);
	str+=strtemp;
	m_MeanRandStrAr.Add(str);
	
	str.Empty();
	strtemp.Empty();
	strtemp.Format("%.3f",S1);
	str+=strtemp;
	str+="\n";
	strtemp.Format("%.3f",S2);
	str+=strtemp;
	m_SumRandStrAr.Add(str);

	m_MeanWhiteyAr.Add(U);
	m_WilcoxonAr.Add(W);
	m_ZAr.Add(Z);
	m_PAr.Add((P>1)?1:P);
}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.6												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整				 //
//Function ：KolmogorovSmirnovTwoSampleTest检验统计数据的求解		     //
//***********************************************************************//

void CNonParamTest::KolmogorovSmirnovTwoSampleTest(CTArray<double,double> &Data1,
													 CTArray<double,double> &Data2,
													 CTArray<double,double> &Weight1,
													 CTArray<double,double> &Weight2,
													 CTString &strFieldName)
{
	assert( (Data1.GetSize()==Weight1.GetSize())&&(Data2.GetSize()==Weight2.GetSize()) );

	int i, j;

	//分别统计组1、与组2的样本数
	double nCount1(0), nCount2(0);
	for(i=0; i<Data1.GetSize(); i++)
	{
		nCount1 += Weight1[i];
	}
	for(i=0; i<Data2.GetSize(); i++)
	{
		nCount2 += Weight2[i];
	}

	//分别对组1、组2按小到大的顺序排序，其对应的权重进行相应的调换。
	OrderTheGroup(Data1,Weight1);
	OrderTheGroup(Data2,Weight2);


	//计算绝对正差，负差，差值序列D,
	double Min(0), Max(0);
	double F1z, F2z;	//分别对应组1、组2的经验分布函数值。
	double cnt1(0), cnt2(0);
	for(i=0; i<Data1.GetSize(); i++)
	{
		cnt1 += Weight1[i]; 

		if(i==Data1.GetSize()-1)
			F1z = 1;
		else if( fabs(Data1[i]-Data1[i+1])<DOUBLE_ZERO1 )
			continue;
		else
			F1z = cnt1/nCount1;
		
		cnt2 = 0;
		for( j=0; j<Data2.GetSize(); j++ )
		{
			if( Data1[i]<Data2[0] )
			{
				F2z = 0;
				break;
			}
			else if( (Data1[i]>Data2[Data2.GetSize()-1])||(fabs(Data1[i]-Data2[Data2.GetSize()-1])<DOUBLE_ZERO1) )
			{
				F2z = 1;
				break;
			}
			else if( Data1[i]<Data2[j] )
			{
				F2z = cnt2/nCount2;
				break;
			}			
			cnt2 += Weight2[j];	
		}	

		if(F1z-F2z>Max)
			Max = F1z-F2z;
		else if(F1z-F2z<Min)
			Min = F1z-F2z;
	}

	cnt1 = 0;
	cnt2 = 0;
	for(i=0; i<Data2.GetSize(); i++)
	{
		cnt2 += Weight2[i];

		if(i==Data2.GetSize()-1)
			F2z = 1;
		else if( fabs(Data2[i]-Data2[i+1])<DOUBLE_ZERO1 )
			continue;
		else
			F2z = cnt2/nCount2;
		
		cnt1 = 0;
		for( j=0; j<Data1.GetSize(); j++ )
		{
			if( Data2[i]<Data1[0] )
			{
				F1z = 0;
				break;
			}
			else if( (Data2[i]>Data1[Data1.GetSize()-1])||(fabs(Data2[i]-Data1[Data1.GetSize()-1])<DOUBLE_ZERO1) )
			{
				F1z = 1;
				break;
			}
			else if( Data2[i]<Data1[j] )
			{
				F1z = cnt1/nCount1;
				break;
			}
			
			cnt1 += Weight1[j];	
		}	

		if(F1z-F2z>Max)
			Max = F1z-F2z;
		else if(F1z-F2z<Min)
			Min = F1z-F2z;
	}

	double MaxD = (Max>(-Min))?Max:(-Min);
	double Z = sqrt(1.0*nCount1*nCount2/(1.0*nCount1+nCount2))*MaxD;

	double Q;
	double P;

	if( Z<0.27)
		P = 1;
	else if( Z<1 )
	{
		Q = pow(M_E, -1.233701/(Z*Z));
		P = 1-2.506628*(Q+pow(Q,9)+pow(Q,25))/Z;
	}
	else if( Z<3.1 )
	{
		Q = pow(M_E, -2*Z*Z);
		P = 2*( Q-pow(Q,4)+pow(Q,9)-pow(Q,16) );
	}
	else
		P = 0;

	//zkc  2002.12.5
	m_FieldNameStrAr.Add(strFieldName);
	CTString str;
	CTString strtemp;
	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nGroupIndex1);
	CTString labelVal;
	if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(m_dValue1,labelVal))
	{
		labelVal+="\n";
	}
	else
	{
		if(floor(m_dValue1)==m_dValue1)
			labelVal.Format("%.0f\n",m_dValue1);
		else
			labelVal.Format("%.2f\n",m_dValue1);
		
	}
	str=labelVal;
	
	
	CTString strTemp;
	if(!m_pDataInterface->GetShowValueLabel() ||!pField->GetValueVL(m_dValue2,labelVal))
	{
		if(floor(m_dValue2)==m_dValue2)
			labelVal.Format("%.0f",m_dValue2);
		else
			labelVal.Format("%.2f",m_dValue2);
	}
	labelVal+="\n总数";
	strTemp=labelVal;
	str+=strTemp;
	m_ClassStrAr.Add(str);

	str.Empty();
	strtemp .Empty();
	strtemp.Format("%.0f",nCount1);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nCount2);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nCount1+nCount2);
	str+=strtemp;
	m_NStrAr.Add(str);

	m_S1Ar.Add(MaxD);
	if(nCount1>=nCount2)
	{
		m_P1Ar.Add(Max);
		m_P2Ar.Add(Min);
	}
	else
	{
		m_P1Ar.Add(-Min);
		m_P2Ar.Add(-Max);
	}
	m_ZAr.Add(Z);
	m_PAr.Add(P);
}


//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.6												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整				 //
//Function ：MosesTest检验统计数据的求解							     //
//***********************************************************************//
void CNonParamTest::MosesTest(CTArray<double,double> &Data1,
							  CTArray<double,double> &Data2,
							  CTArray<double,double> &Weight1,
							  CTArray<double,double> &Weight2,
							  CTString &strFieldName)
{
	int i, j;

	//统计组1、组2的样本数
	double nCount1(0), nCount2(0);
	for(i=0; i<Data1.GetSize(); i++)
	{
		nCount1 += Weight1[i];
	}
	for(i=0; i<Data2.GetSize(); i++)
	{
		nCount2 += Weight2[i];
	}	

	//对组1、组2的数据按从小到大排序，其对应的权重做相应的调整。
	OrderTheGroup(Data1,Weight1);
	OrderTheGroup(Data2,Weight2);


	double h = (floor(0.05*nCount1)>1)?floor(0.05*nCount1):1;
	double MaxR1, MaxR2, MinR1(0), MinR2(0), MinRS2(0), MaxRS2(0);
	double    nCnt(0), nCnt1(0), nCnt2(0),nnCnt1(0);
	for(i=0,j=0; i<Data1.GetSize()||j<Data2.GetSize(); )
	{
		if( (i<Data1.GetSize()&&j<Data2.GetSize())&&(fabs(Data1[i]-Data2[j])<DOUBLE_ZERO1) )
		{
			nCnt1  += Weight1[i];
			nnCnt1 += Weight1[i];
			while( i+1<Data1.GetSize() )
			{
				if(fabs(Data1[i]-Data1[i+1])<DOUBLE_ZERO1)
				{ 
					i++;
					nCnt1  += Weight1[i];
					nnCnt1 += Weight1[i];
				}
				else
					break;
			}
			i++;			
			
			nCnt2  += Weight2[j];
			while( j+1<Data2.GetSize() )
			{
				if(fabs(Data2[j]-Data2[j+1])<DOUBLE_ZERO1)
				{ 
					j++;
					nCnt2 += Weight2[j];
				}
				else
					break;
			}
			j++;
			MaxR1   = nCnt+(1+nCnt1+nCnt2)/2.0;
			MaxR2   = nCnt+(1+nCnt1+nCnt2)/2.0;		
			if( fabs(MinR1)<DOUBLE_ZERO1 )
			{
				MinR1  = MaxR1;				
			}
			if( fabs(MinR2)<DOUBLE_ZERO1 )
			{
				MinR2 = MaxR2;
			}
			if(fabs(MinRS2)<DOUBLE_ZERO1&&nnCnt1>=h+1)
				MinRS2 = MaxR1;
			if(fabs(MaxRS2)<DOUBLE_ZERO1&&nnCnt1>=nCount1-h)
				MaxRS2 = MaxR1;
			
			nCnt += nCnt1+nCnt2;
			nCnt1 = 0;
			nCnt2 = 0;
		}
		else if( (i<Data1.GetSize())&&((j==Data2.GetSize())||(Data1[i]<Data2[j])) )
		{
			nCnt1  += Weight1[i];
			nnCnt1 += Weight1[i];
			while( i+1<Data1.GetSize() )
			{
				if(fabs(Data1[i]-Data1[i+1])<DOUBLE_ZERO1)
				{ 
					i++;
					nCnt1  += Weight1[i];
					nnCnt1 += Weight1[i];
				}
				else
					break;
			}
			i++;
			MaxR1 = nCnt+(1+nCnt1)/2.0;
			if( fabs(MinR1)<DOUBLE_ZERO1 )
			{
				MinR1  = MaxR1;
			}
			if(fabs(MinRS2)<DOUBLE_ZERO1&&nnCnt1>=h+1)
				MinRS2 = MaxR1;
			if(fabs(MaxRS2)<DOUBLE_ZERO1&&nnCnt1>=nCount1-h)
				MaxRS2 = MaxR1;

			nCnt += nCnt1;
			nCnt1 = 0;
		}
		else if( (j<Data2.GetSize())&&((i==Data1.GetSize())||(Data1[i]>Data2[j])) )
		{
			nCnt2 += Weight2[j];
			while( j+1<Data2.GetSize() )
			{
				if(fabs(Data2[j]-Data2[j+1])<DOUBLE_ZERO1)
				{ 
					j++;
					nCnt2 += Weight2[j];
				}
				else
					break;
			}
			j++;
			MaxR2 = nCnt+(1+nCnt2)/2.0;
			if(fabs(MinR2)<DOUBLE_ZERO1 )
			{
				MinR2 = MaxR2;
			}
			nCnt += nCnt2;
			nCnt2 = 0;
		}
	}

	double    S1 = floor( MaxR1 - MinR1 + 1.5 );
	if(nCount1+nCount2>2E8)
	{
		CTString str=strFieldName;
		str+="样本数太大，不能进行莫斯检验.";
		if(m_p2RsltElementText==NULL)
			m_p2RsltElementText=new CRsltElementText("警告");
		m_p2RsltElementText->AddString(str);
		return;
	}

	double e = CnkForBigData(nCount1+nCount2, nCount1);
	double P1 = 0;
	if( nCount1+nCount2<500 )
	{
		if(S1-nCount1!=0)
		{
			for( i=0; i<S1-nCount1+1; i++ )
			{
				double xx = Cnk(nCount1+i-2,i);
				double yy = Cnk(nCount2-i+1,1);
				if(xx<0||yy<0)
				{
					CTString str=strFieldName;
					str+="样本值为1的个数小于3，不能进行莫斯检验.";
					if(m_p2RsltElementText==NULL)
						m_p2RsltElementText=new CRsltElementText("警告");
					m_p2RsltElementText->AddString(str);
					return;
				}
				P1 += xx*yy;
			}
		}
		P1 /= Cnk(nCount1+nCount2, nCount1);
	}
	else
	{
		if(S1-nCount1!=0)
		{
			for( i=0; i<S1-nCount1+1; i++ )
			{
				double xx = CnkForBigData(nCount1+i-2,i);
				double yy = CnkForBigData(nCount2-i+1,1);
				if(xx<0||yy<0)
				{
					CTString str=strFieldName;
					str+="样本值为1的个数小于3，不能进行莫斯检验..";
					if(m_p2RsltElementText==NULL)
						m_p2RsltElementText=new CRsltElementText("警告");
					m_p2RsltElementText->AddString(str);
					return;
				}

				P1 += pow(10, xx + yy - e);
			}
		}
	}	

	double S2 = floor( MaxRS2 - MinRS2 + 1.5 );

	double P2 = 0;
	if( nCount1+nCount2<500 )
	{
		if(S2-nCount1+2*h!=0)
		{
			for( i=0; i<S2-nCount1+2*h+1; i++ )
			{
				P2 += Cnk(nCount1+i-2*h-2,i)*Cnk(nCount2-i+1+2*h,1+2*h);
			}
		}
		P2 /= Cnk(nCount1+nCount2, nCount1);
	}
	else
	{
		
		if(S2-nCount1+2*h!=0)
		{
			for( i=0; i<S2-nCount1+2*h+1; i++ )
			{
				P2 += pow(10, CnkForBigData(nCount1+i-2*h-2,i)+CnkForBigData(nCount2-i+1+2*h,1+2*h)-e);
			}
		}
	}

	///////
	//zkc 2002.12.3

	m_FieldNameStrAr.Add(strFieldName);
	CTString str;
	CTString strtemp;
	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nGroupIndex1);
	CTString labelVal;
	if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(m_dValue1,labelVal))
	{
		labelVal+="\n";
	}
	else
	{
		if(floor(m_dValue1)==m_dValue1)
			labelVal.Format("%.0f\n",m_dValue1);
		else
			labelVal.Format("%.2f\n",m_dValue1);
		
	}
	str=labelVal;
	
	
	CTString strTemp;
	if(!m_pDataInterface->GetShowValueLabel() ||!pField->GetValueVL(m_dValue2,labelVal))
	{
		if(floor(m_dValue2)==m_dValue2)
			labelVal.Format("%.0f",m_dValue2);
		else
			labelVal.Format("%.2f",m_dValue2);
	}
	labelVal+="\n总数";
	strTemp=labelVal;
	str+=strTemp;
	m_ClassStrAr.Add(str);

	str.Empty();
	strtemp .Empty();
	strtemp.Format("%.0f",nCount1);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nCount2);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nCount1+nCount2);
	str+=strtemp;
	m_NStrAr.Add(str);

	m_S1Ar.Add(S1);
	m_S2Ar.Add(S2);
	m_P1Ar.Add(P1);
	m_P2Ar.Add(P2);
	m_hAr.Add(h);

}


//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.7												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整	,数据越界	 //
//Function ：WaldWolfowitzTest检验统计数据的求解					     //
//***********************************************************************//
void CNonParamTest::WaldWolfowitzTest(CTArray<double,double> &Data1,//分组1数据
									  CTArray<double,double> &Data2,//分组2数据
									  CTArray<double,double> &Weight1,//分组1对应的权重
									  CTArray<double,double> &Weight2,//分组2对应的权重
									  CTString &strFieldName)//分析变量的字段名
{
	//求最大可能游程与最小可能游程
	double nMax=0,nMin=0;
	GetMaxAndMinRuns(Data1,Data2,Weight1,Weight2,nMax,nMin);
	int i=0;
	
	//分别求组1、组2的样本数
	double nCount1(0), nCount2(0);
	for(i=0; i<Data1.GetSize(); i++)
	{
		nCount1 += Weight1[i];
	}
	
	for(i=0; i<Data2.GetSize(); i++)
	{
		nCount2 += Weight2[i];
	}
	if(nCount1+nCount2>2e8)
	{
		CTString str1=strFieldName;
		str1+="样本数太大，不能进行沃尔德-沃尔佛威茨游程检验！";
		if(m_p3RsltElementText==NULL)
			m_p3RsltElementText=new CRsltElementText("警告");
		m_p3RsltElementText->AddString(str1);
		return;
	}

	//分别对组1、组2数据按从小到大排序，并调换其相应的权重
	//OrderTheGroup(Data1,Weight1);
	//OrderTheGroup(Data2,Weight2);

	double Pmax(0), Pmin(0), Zmax(0), Zmin(0);
	double u = 2.0*nCount1*nCount2/(nCount1+nCount2)+1;	
	double o = sqrt( 2.0*nCount1*nCount2*(2.0*nCount1*nCount2-nCount1-nCount2)/(1.0*(nCount1+nCount2)*(nCount1+nCount2)*(nCount1+nCount2-1)) );

	if( nCount1+nCount2<50 )
	{
		Zmax = (nMax-u+0.5)/o;
		Zmin = (nMin-u+0.5)/o;
	}
	else
	{
		Zmax = (nMax-u)/o;
		Zmin = (nMin-u)/o;
	}

	if( nCount1+nCount2<=30 )
	{
		for( i=2; i<nMin+1; i++ )
		{
			if( i%2==0 )
			{
				Pmin += 2.0*Cnk(nCount1-1,i/2-1)*Cnk(nCount2-1,i/2-1);
			}
			else if( (i+1)/2<=min(nCount1, nCount2) )
			{
				Pmin += Cnk(nCount1-1,(i+1)/2-1)*Cnk(nCount2-1,(i+1)/2-2) + Cnk(nCount1-1,(i+1)/2-2)*Cnk(nCount2-1,(i+1)/2-1);
			}
		}
		for( i=2; i<nMax+1; i++ )
		{
			if( i%2==0 )
			{
				Pmax += 2.0*Cnk(nCount1-1,i/2-1)*Cnk(nCount2-1,i/2-1);
			}
			else if( (i+1)/2<=min(nCount1, nCount2) )
			{
				Pmax += Cnk(nCount1-1,(i+1)/2-1)*Cnk(nCount2-1,(i+1)/2-2) + Cnk(nCount1-1,(i+1)/2-2)*Cnk(nCount2-1,(i+1)/2-1);
			}
		}
		Pmin /= Cnk(nCount1+nCount2, nCount1);
		Pmax /= Cnk(nCount1+nCount2, nCount1);
	}
	else
	{
		Pmin = normcdf(Zmin,0,1);
		Pmax = normcdf(Zmax,0,1);			
	}

	////////////
	//zkc 2002.12.3
	m_FieldNameStrAr.Add(strFieldName);
	CTString str;
	CTString strtemp;
	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nGroupIndex1);
	CTString labelVal;
	if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(m_dValue1,labelVal))
	{
		labelVal+="\n";
	}
	else
	{
		if(floor(m_dValue1)==m_dValue1)
			labelVal.Format("%.0f\n",m_dValue1);
		else
			labelVal.Format("%.2f\n",m_dValue1);
		
	}
	str=labelVal;
	
	
	CTString strTemp;
	if(!m_pDataInterface->GetShowValueLabel() ||!pField->GetValueVL(m_dValue2,labelVal))
	{
		if(floor(m_dValue2)==m_dValue2)
			labelVal.Format("%.0f",m_dValue2);
		else
			labelVal.Format("%.2f",m_dValue2);
	}
	labelVal+="\n总数";
	strTemp=labelVal;
	str+=strTemp;
	m_ClassStrAr.Add(str);

	str.Empty();
	strtemp .Empty();
	strtemp.Format("%.0f",nCount1);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nCount2);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nCount1+nCount2);
	str+=strtemp;
	m_NStrAr.Add(str);

	str.Empty();
	str+="最小可能游程数\n最大可能游程数";
	m_MeanRandStrAr.Add(str);

	str.Empty();
	strtemp.Empty();
	strtemp.Format("%.0f",nMin);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nMax);
	str+=strtemp;
	m_SumRandStrAr.Add(str);

	str.Empty();
	strtemp.Empty();
	strtemp.Format("%.4f",Zmin);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.4f",Zmax);
	str+=strtemp;
	m_ZStrAr.Add(str);

	str.Empty();
	strtemp.Empty();
	strtemp.Format("%.4f",Pmin);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.4f",Pmax);
	str+=strtemp;
	m_PStrAr.Add(str);
}


//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.7												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整				 //
//Function ：WaldWolfowitzTest检验统计数据的求解					     //
//***********************************************************************//

void CNonParamTest::WilcoxonMatchedPairsSignRankTest(CTArray<double,double> &Data1,  //配对变量1
													 CTArray<double,double> &Data2,	//配对变量2
													 CTArray<double,double> &Weight,		//权重
													 CTString &strFieldName)
{
	int i;
	CTArray<double,double> at;
	CTArray<double,double>       NewWeight;

	double nZero        = 0;
	double nNegative = 0;
	double nPositive = 0;

	//变量1与变量2对应元素相减，
	//求出正秩，负秩，0秩的频数
	for(i=0; i<Data1.GetSize(); i++)
	{
		double e = Data2[i]-Data1[i];
		if( fabs(e)<DOUBLE_ZERO1 )
			nZero += Weight[i];
		else if( e>0 )
		{
			nNegative += Weight[i];
			at.Add(e);
			NewWeight.Add(Weight[i]);
		}
		else
		{ 
			nPositive += Weight[i];
			at.Add(-e);
			NewWeight.Add(-Weight[i]);
		}
	}

	//对相减后的数组进行批排序，起对应的权重也要做相应的调换。

	OrderTheGroup(at,NewWeight);


	double    m  = nPositive + nNegative;
	double Sp(0),Sn(0),T(0);//正秩和、负秩和
	double    nCnt(0), nCnt1(0), nCnt2(0);
	for(i=0; i<at.GetSize(); i++)
	{
		if( NewWeight[i]>0 )
		{
			nCnt1 += NewWeight[i];
		}
		else
		{
			nCnt2 -= NewWeight[i];
		}

		while( (i+1<at.GetSize())&&(fabs(at[i]-at[i+1])<DOUBLE_ZERO1) )
		{
			i++;
			if( NewWeight[i]>0 )
				nCnt1 += NewWeight[i];
			else
				nCnt2 -= NewWeight[i];
		}
		Sp   += (nCnt+(1+nCnt1+nCnt2)/2.0)*nCnt1;
		Sn   += (nCnt+(1+nCnt1+nCnt2)/2.0)*nCnt2;
		nCnt += nCnt1+nCnt2;
		T    += 1.0*((nCnt1+nCnt2)*(nCnt1+nCnt2)-1)*(nCnt1+nCnt2);
		nCnt1 = 0;
		nCnt2 = 0;
	}
	double e = (Sp<Sn)?Sp:Sn;
	if(1.0*m*(m+1)*(2.0*m+1)/24.0-T/48.0<=0)
	{
		CTString str=strFieldName;
		str+="不适合做威尔科克森配对秩和检验。";
		if(m_p1RsltElementText==NULL)
			m_p1RsltElementText=new CRsltElementText("警告");
		m_p1RsltElementText->AddString(str);	
		return ;
	}

	double Z = (m==0)?0:(e-1.0*m*(m+1)/4.0)/sqrt(1.0*m*(m+1)*(2.0*m+1)/24.0-T/48.0);
	double P = (m==0)?1:2*normcdf(Z,0,1);

	///zkc 2002.12.3
	m_FieldNameStrAr.Add(strFieldName);
	CTString str,strtemp;
	str+="负秩\n正秩\n零秩";
	m_ClassStrAr.Add(str);

	str.Empty();
	strtemp.Format("%.0f",nPositive);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nNegative);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nZero);
	str+=strtemp;
	m_NStrAr.Add(str);

	str.Empty();
	strtemp.Empty();
	if(nPositive==0)
	{
		strtemp="0";
	}
	else
	{
		strtemp.Format("%.3f",Sn/nPositive);
	}
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	if(nNegative==0)
		strtemp="0";
	else
		strtemp.Format("%.3f",Sp/nNegative);
	str+=strtemp;
	m_MeanRandStrAr.Add(str);

	str.Empty();
	strtemp.Empty();
	strtemp.Format("%.3f",Sn);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.3f",Sp);
	str+=strtemp;
	m_SumRandStrAr.Add(str);

	m_ZAr.Add(Z);
	m_PAr.Add(P);
}


//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.7												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整，数据越界	 //
//Function ：SignTest检验统计数据的求解								     //
//***********************************************************************//

void CNonParamTest::SignTest(CTArray<double,double> &Data1,
							 CTArray<double,double> &Data2,
							 CTArray<double,double> &Weight,CTString &strFieldName)
{
	int i;

	//改成double 型，以免数据越界。
	double nZero     = 0;//0秩的频数
	double nNegative = 0;//负秩的频数
	double nPositive = 0;//正秩的频数

	for(i=0; i<Data1.GetSize(); i++)
	{
		double dTemp = Data2[i]-Data1[i];
		if( fabs(dTemp)<DOUBLE_ZERO1 )
			nZero += Weight[i];
		else if( dTemp>0 )
		{
			nNegative += Weight[i];			
		}
		else
		{
			nPositive += Weight[i];			
		}
	}
	double P = 0;//显著性
	double Z=0;//Z统计量
	double    r = (nNegative<nPositive)?nNegative:nPositive;
	if(nNegative+nPositive==0)
	{
		P = 1;
	}
	else if( nNegative+nPositive<26 )
	{
		for( i=0; i<r+1; i++ )
		{
			P += Cnk(nNegative+nPositive,i);
		}
		P /= pow(2, nNegative+nPositive-1);
	}
	
	else
	{
		double Max = (nNegative>nPositive)?nNegative:nPositive;
		Z = -(2*Max-nNegative-nPositive-1)/(double)sqrt(nNegative+nPositive);
		P = 2*normcdf(Z,0,1);
	}

	//zkc 2002.12.3
	//格式化输出值的设置

	m_FieldNameStrAr.Add(strFieldName);
	CTString str,strtemp;
	str+="负差\n正差\n零差\n总和";
	m_ClassStrAr.Add(str);
	str.Empty();

	strtemp.Format("%.0f",nPositive);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nNegative);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nZero);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",nPositive+nNegative+nZero);
	str+=strtemp;
	m_NStrAr.Add(str);

	strtemp.Empty();	
	if( nNegative+nPositive>25 )
	{
		strtemp.Format("%.3f",Z);
	}
	m_MeanRandStrAr.Add(strtemp);
	strtemp.Empty();
	strtemp.Format("%.3f",(P>1)?1:P);
	m_SumRandStrAr.Add(strtemp);
}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.7												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整，数据越界	 //
//Function ：FriedmansTwowayAnalysisofVariancebyRank检验统计数据的求解   //
//***********************************************************************//
void CNonParamTest::FriedmansTwowayAnalysisofVariancebyRank(CDoubleMatrix mat,
															CTArray<double,double> &Weight)
{
	int i, j, ii;	
	int k = mat.mcol();

	assert( mat.mrow()==Weight.GetSize() );

	//1.对矩阵的每一行进行编秩，
	//即每一行从1到k编秩，
	//记下第i行的修正值t，求出总和T=sum(t).
	double n = 0;
	int T(0), temp(0);
	CDoubleMatrix matC(mat.mrow(), k);
	for( i=0; i<mat.mrow(); i++ )
	{
		n += Weight[i];
		CDoubleVector VRow = mat.row(i).copy();
		CDoubleVector VIndex(k);		
		VRow.Sort();
		int nSameCnt = 1;
		for( j=1; j<k; j++ )
		{
			if( fabs(VRow(j)-VRow(j-1))<DOUBLE_ZERO1 )
			{
				nSameCnt++;
			}
			else
			{
				for( ii=0; ii<nSameCnt; ii++ )
				{
					VIndex(j-ii-1) = j+0.5-nSameCnt/2.0;
				}	
				temp += 1.0*(nSameCnt*nSameCnt-1)*nSameCnt;
				nSameCnt   = 1;
			}
		}
		for( ii=0; ii<nSameCnt; ii++ )
		{
			VIndex(k-ii-1) = k+0.5-nSameCnt/2.0;
		}
		temp += 1.0*(nSameCnt*nSameCnt-1)*nSameCnt;
		T    += temp*Weight[i];
		temp  = 0;
		nSameCnt = 1;

		for( j=0; j<k; j++ )
		{
			for( ii=0; ii<k; ii++ )
			{
				if( fabs(mat(i,j)-VRow(ii))<DOUBLE_ZERO1 )
				{
					matC(i,j) = VIndex(ii);
					break;
				}
			}
		}
	}
	//2.求出第l列的秩和，即把第l列的秩相加，得到秩和C，
	//并求出平均秩R = C/n.
	CDoubleVector C(k, 0);
	for( i=0; i<k; i++ )
	{
		for( j=0; j<mat.mrow(); j++ )
		{
			C(i) += matC(j,i)*Weight[j];
		}		
	}

	double sum = 0;
	for( i=0; i<k; i++ )
	{
		sum += C(i)*C(i);
	}
	if(n*k*(k*k-1)-T==0)
	{
		CTString str="所有样本数据值相同，不能进行弗里德曼检验！";
		if(m_p1RsltElementText==NULL)
			m_p1RsltElementText=new CRsltElementText("警告");
		m_p1RsltElementText->AddString(str);
		return;
	}
	double F = (12.0*(k-1)*sum - 3.0*n*n*k*(k+1)*(k*k-1))/(1.0*n*k*(k*k-1)-T);
	int df = k-1;
	double P = 1-chi2cdf(F,df);
	//zkc
	for(i=0;i<m_tVarInfo3.iCount;i++)
	{
		int nIndex=m_tVarInfo3.pValue[i];
		m_FieldNameStrAr.Add(m_pDataInterface->GetFieldName(nIndex));
	}

	CreateResultForFriedman(n,F,df,P,C);

}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.7												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整，数据越界	 //
//Function ：CochransQTest检验统计数据的求解							 //
//***********************************************************************//
void CNonParamTest::CochransQTest(CDoubleMatrix mat,CTArray<double,double> &Weight)
{
	int i, j;	
	int k = mat.mcol();//总样本数.
	for(i=0; i<mat.mrow(); i++)
	{
		for(j=0; j<mat.mcol(); j++)
		{
			if( !(fabs(mat(i,j))<DOUBLE_ZERO1||fabs(mat(i,j)-1.0)<DOUBLE_ZERO1) )
			{
				CTString str="数据中有0，1以外的数，不能进行科克伦Q检验.";
				if(m_p3RsltElementText==NULL)
				{
					m_p3RsltElementText=new CRsltElementText("警告");
				}
				m_p3RsltElementText->AddString(str);				
				return;
			}
				
		}
	}

	double n(0);//列数.
	for( i=0; i<mat.mrow(); i++ )
	{
		n += Weight[i];
	}

	CDoubleMatrix dmat(k,2,0.0);

	for( i=0; i<k; i++ )
	{
		for( j=0; j<mat.mrow(); j++ )
		{
			if( fabs(mat(j,i)-1)<DOUBLE_ZERO1 )
				dmat(i,1) += Weight[j];
			else if(fabs(mat(j,i))<DOUBLE_ZERO1 )
				dmat(i,0) += Weight[j];
		}		
	}

	CDoubleVector R(mat.mrow(), 0);
	CDoubleVector C(k, 0);

	for( i=0; i<mat.mrow(); i++ )
	{
		R(i) = sum(mat.row(i));
	}

	for( i=0; i<k; i++ )
	{
		for( j=0; j<mat.mrow(); j++ )
		{
			C(i) += mat(j,i)*Weight[j];
		}
	}

	double d1 = 0;
	double d2 = 0;
	double d3 = 0;

	for( i=0; i<k; i++ )
	{
		d1 += C(i)*C(i);
		d2 += C(i);
	}

	for( i=0; i<mat.mrow(); i++ )
	{
		d3 += R(i)*R(i)*Weight[i];
	}

	double Q = (k-1)*(k*d1 - d2*d2)/(k*d2-d3);
	int df = k-1;
	double P = 1-chi2cdf(Q,df);
	for(i=0;i<m_tVarInfo3.iCount;i++)
	{
		int nIndex=m_tVarInfo3.pValue[i];
		m_FieldNameStrAr.Add(m_pDataInterface->GetFieldName(nIndex));
	}

	CreateResultForCochran(dmat,n,Q,df,P);
}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.10												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整，数据越界	 //
//Function ：KendallsCoefficientofConcordence检验统计数据的求解			 //
//***********************************************************************//
void CNonParamTest::KendallsCoefficientofConcordence(CDoubleMatrix mat,CTArray<double,double> &Weight)
{
	int i, j, ii;	
	int k = mat.mcol();//总样本数.;
	assert( mat.mrow()==Weight.GetSize() );
	double n = 0;//行数.
	double T(0), temp(0);
	CDoubleMatrix matC(mat.mrow(), k);
	for( i=0; i<mat.mrow(); i++ )
	{
		n += Weight[i];
		CDoubleVector VRow = mat.row(i).copy();
		CDoubleVector VIndex(k);		
		VRow.Sort();
		int nSameCnt = 1;
		for( j=1; j<k; j++ )
		{
			if( fabs(VRow(j)-VRow(j-1))<DOUBLE_ZERO1 )
			{
				nSameCnt++;
			}
			else
			{
				for( ii=0; ii<nSameCnt; ii++ )
				{
					VIndex(j-ii-1) = j+0.5-nSameCnt/2.0;
				}	
				temp += 1.0*(nSameCnt*nSameCnt-1)*nSameCnt;
				nSameCnt   = 1;
			}
		}
		for( ii=0; ii<nSameCnt; ii++ )
		{
			VIndex(k-ii-1) = k+0.5-nSameCnt/2.0;
		}
		temp += 1.0*(nSameCnt*nSameCnt-1)*nSameCnt;
		T    += temp*Weight[i];
		temp  = 0;
		nSameCnt = 1;

		//================
		for( j=0; j<k; j++ )
		{
			for( ii=0; ii<k; ii++ )
			{
				if( fabs(mat(i,j)-VRow(ii))<DOUBLE_ZERO1 )
				{
					matC(i,j) = VIndex(ii);
					break;
				}
			}
		}
	}
	
	CDoubleVector C(k, 0);
	for( i=0; i<k; i++ )
	{
		for( j=0; j<mat.mrow(); j++ )
		{
			C(i) += matC(j,i)*Weight[j];
		}		
	}

	double sum = 0;
	for( i=0; i<k; i++ )
	{
		sum += C(i)*C(i);
	}

	if(n*n*k*(k*k-1)-n*T==0)
	{
		CTString str="有效样本数太少，不能进行肯德尔W检验";
		if(m_p2RsltElementText==NULL)
			m_p2RsltElementText=new CRsltElementText("警告");
		m_p2RsltElementText->AddString(str);
		return;
	}

	double W  = (12.0*sum - 3.0*n*n*k*(k+1)*(k+1))/(1.0*n*n*k*(k*k-1)-1.0*n*T);
	double X  = W*n*(k-1);	
	int df = k-1;
	double P  = 1-chi2cdf(X,df);
	for(i=0;i<m_tVarInfo3.iCount;i++)
	{
		int nIndex=m_tVarInfo3.pValue[i];
		m_FieldNameStrAr.Add(m_pDataInterface->GetFieldName(nIndex));
	}

	CreateResultForKendall(C,n,W,X,df,P);
}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.10												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整，数据越界	 //
//Function ：KruskalWallisHTest检验统计数据的求解						 //
//***********************************************************************//
void CNonParamTest::KruskalWallisHTest(CPtrArray &VectArray,CTArray<double,double>&stdAr,CTString &strFieldName)
{
	int i, j, ii;
	int k = VectArray.GetSize();

	CIntVector n(k),nn(k,0);
	for( i=0; i<k; i++ )
	{	
		CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
		n(i) = p->GetSize();
		for( j=0; j<n(i); j++ )
		{
			TManyGroup *p1=(TManyGroup*)(p->GetAt(j));
			nn(i) += p1->nWeight;
		}
	}

	double    ba = sum(nn);
	for( i=0; i<k; i++ )
	{
		CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
				
		for( j=0; j<n(i); j++ )
		{
			TManyGroup *p1=(TManyGroup*)(p->GetAt(j));
			int    idx   = j;
			double dTemp = p1->dValue;
			for( ii=j+1; ii<n(i); ii++)
			{
				TManyGroup *p2=(TManyGroup*)(p->GetAt(ii));
				if( dTemp>p2->dValue )
				{
					idx   = ii;
					dTemp = p2->dValue;
				}
			}
			if(idx!=j)
			{
				TManyGroup *p2=(TManyGroup*)(p->GetAt(idx));
				TManyGroup temp = *p1;

				p1->dValue  = p2->dValue;
				p1->nWeight = p2->nWeight;
				
				p2->dValue  = temp.dValue;
				p2->nWeight = temp.nWeight;
			}
		}
	}
	
	CDoubleVector VectTemp(sum(n)),grpVect;
	CIntVector idxVect;

	ii = 0;
	for( i=0; i<k; i++ )
	{
		CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
		for( j=0; j<n(i); j++ )
		{
			TManyGroup *p1=(TManyGroup*)(p->GetAt(j));
			VectTemp( ii++ ) = p1->dValue;
		}
	}
	grp2idx(VectTemp,idxVect,grpVect);

	int nTotalCnt(0);
	double T(0);
	CIntVector Pos(k,0),nCnt(k,0);
	CDoubleVector vg(k,0);
	
	for( ii = 0; ii<grpVect.vlen(); ii++)
	{
		double SumTemp = 0;
		for( i=0; i<k; i++ )
		{
			CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
			while( (Pos(i)<n(i))&&( fabs(((TManyGroup*)(p->GetAt(Pos(i))))->dValue-grpVect(ii))<DOUBLE_ZERO1) )
			{
				nCnt(i) += ((TManyGroup*)(p->GetAt(Pos(i))))->nWeight;
				Pos(i) ++;
			}
			SumTemp += nCnt(i);
		}
		for( i=0; i<k; i++ )
		{
			vg(i) += (nTotalCnt+(1+sum(nCnt))/2.0)*nCnt(i);
		}
		for( i=0; i<k; i++ )
		{
			nCnt(i) = 0;
		}
		nTotalCnt += SumTemp;
		T += 1.0*(SumTemp*SumTemp-1)*SumTemp;
	}	

	double d1 = 0;
	
	for( i=0; i<k; i++ )
	{
		d1 += vg(i)*vg(i)/nn(i);	
	}

	if(1.0*ba*ba*ba -ba -T==0)
	{
		CTString str=strFieldName;
		str+="有效样本数太少，不能进行克拉夏尔-瓦里斯检验";
		if(m_p1RsltElementText==NULL)
			m_p1RsltElementText=new CRsltElementText("警告");
		m_p1RsltElementText->AddString(str);
		return;
	}

	double H = ( 12.0*(ba-1)*d1 - 3.0*ba*(ba+1)*(1.0*ba*ba-1) )/(1.0*ba*ba*ba -ba -T);
	int df = k-1;
	double ab = 1-chi2cdf(H,df);

	////zkc 2002.12.3
	//CTArray<double,double> stdAr;
	m_FieldNameStrAr.Add(strFieldName);
	CTString strTemp1,strTemp2,strTemp3;
	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nGroupIndex2);

	for(i=0;i<nn.vlen();i++)
	{
		CTString str;
		str.Format("%d",nn[i]);
		strTemp1+=str;
		str.Empty();
		str.Format("%.3f",vg[i]/nn[i]);//////////zkc
		strTemp2+=str;
		str.Empty();
		double temba=stdAr[i];

		CTString labelVal;
		if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(temba,labelVal))
		{
			str=labelVal;
		}
		else
		{
			if(stdAr.GetAt(i)==floor(stdAr.GetAt(i)))
				str.Format("%.0f",floor(stdAr.GetAt(i)));
			else
				str.Format("%.3f",stdAr.GetAt(i));
			
		}
		strTemp3+=str;
		if(i!=n.vlen()-1)
		{
			strTemp1+="\n";	
			strTemp2+="\n";
			strTemp3+="\n";
		}
		str.Empty();
	}
	
	m_NStrAr.Add(strTemp1);
	m_MeanRandStrAr.Add(strTemp2);
	m_ClassStrAr.Add(strTemp3);
	m_P2Ar.Add(H);
	m_ZAr.Add(df);
	m_PAr.Add(ab);
}

//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.10												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整，数据越界	 //
//Function ：MedianTest检验统计数据的求解								 //
//***********************************************************************//
void CNonParamTest::MedianTest(CPtrArray& VectArray,CTArray<double,double> &stdAr,CTString &strFieldName)
{
	int i, j, ii;
	int k = VectArray.GetSize();

	CIntVector n(k);
	CDoubleVector nn(k,0);
	for( i=0; i<k; i++ )
	{	
		CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
		n(i) = p->GetSize();
		for( j=0; j<n(i); j++ )
		{
			TManyGroup *p1=(TManyGroup*)(p->GetAt(j));
			nn(i) += p1->nWeight;
		}
	}
	for(i=0;i<k;i++)
	{
		if(nn(i)>2e8)		
		{
			CTString str1=strFieldName;
			str1+="样本数太大，不能进行中位数检验！";
			if(m_p2RsltElementText==NULL)
				m_p2RsltElementText=new CRsltElementText("警告");
			m_p2RsltElementText->AddString(str1);	
			return ;
		}
	}



	double    N = sum(nn);
	
	for( i=0; i<k; i++ )
	{
		CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
				
		for( j=0; j<n(i); j++ )
		{
			TManyGroup *p1=(TManyGroup*)(p->GetAt(j));
			int    idx   = j;
			double dTemp = p1->dValue;
			for( ii=j+1; ii<n(i); ii++)
			{
				TManyGroup *p2=(TManyGroup*)(p->GetAt(ii));
				if( dTemp>p2->dValue )
				{
					idx   = ii;
					dTemp = p2->dValue;
				}
			}
			if(idx!=j)
			{
				TManyGroup *p2=(TManyGroup*)(p->GetAt(idx));
				TManyGroup temp = *p1;

				p1->dValue  = p2->dValue;
				p1->nWeight = p2->nWeight;
				
				p2->dValue  = temp.dValue;
				p2->nWeight = temp.nWeight;
			}
		}
	}
	
	CDoubleVector VectTemp(sum(n)),grpVect;
	CIntVector idxVect;

	ii = 0;
	for( i=0; i<k; i++ )
	{
		CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
		for( j=0; j<n(i); j++ )
		{
			TManyGroup *p1=(TManyGroup*)(p->GetAt(j));
			VectTemp( ii++ ) = p1->dValue;
		}
	}
	grp2idx(VectTemp,idxVect,grpVect);

	int nTotalCnt(0);
	CIntVector Pos(k,0);
	double Mid(0);
	
	if(floor(N/2)*2!=N)
	{
		for( ii = 0; ii<grpVect.vlen(); ii++)
		{
			for( i=0; i<k; i++ )
			{
				CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
				while( (Pos(i)<n(i))&&( fabs(((TManyGroup*)(p->GetAt(Pos(i))))->dValue-grpVect(ii))<DOUBLE_ZERO1) )
				{					
					nTotalCnt += ((TManyGroup*)(p->GetAt(Pos(i))))->nWeight;
					if( nTotalCnt>=(N+1)/2 )
					{
						Mid = ((TManyGroup*)(p->GetAt(Pos(i))))->dValue;
						goto ytlig;
					}
					Pos(i) ++;
				}
			}
		}	
	}
	else
	{
		for( ii = 0; ii<grpVect.vlen(); ii++)
		{
			for( i=0; i<k; i++ )
			{
				CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
				while( (Pos(i)<n(i))&&( fabs(((TManyGroup*)(p->GetAt(Pos(i))))->dValue-grpVect(ii))<DOUBLE_ZERO1) )
				{		
					for( j=0; j<((TManyGroup*)(p->GetAt(Pos(i))))->nWeight; j++ )
					{
						nTotalCnt++;
						if( nTotalCnt==N/2 )
						{
							Mid += ((TManyGroup*)(p->GetAt(Pos(i))))->dValue/2.0;
						}
						else if( nTotalCnt==N/2+1 )
						{
							Mid += ((TManyGroup*)(p->GetAt(Pos(i))))->dValue/2.0;
							goto ytlig;
						}						
					}
					Pos(i) ++;
				}
			}
		}	
	}

ytlig:	 CDoubleMatrix f3(2, k, 0.0);

	for( i=0; i<k; i++ )
	{
		CPtrArray *p=(CPtrArray *)(VectArray.GetAt(i));
		for( j=0; j<n(i); j++ )
		{
			TManyGroup *p1=(TManyGroup*)(p->GetAt(j));
			if( p1->dValue>Mid )
				f3(0,i) += p1->nWeight;
			else
				f3(1,i) += p1->nWeight;
		}
	}

	double R1(0), R2(0);
	for( i=0; i<k; i++ )
	{
		R1 += f3(0,i);
		R2 += f3(1,i);
	}

	CDoubleMatrix E(2, k, 0.0);

	for( i=0; i<k; i++ )
	{
		E(0,i) = R1*nn(i)/(double)N;
		E(1,i) = R2*nn(i)/(double)N;
	}

	double X = 0;

	for( i=0; i<k; i++ )
	{
		X += ( f3(0,i)-E(0,i) )*( f3(0,i)-E(0,i) )/E(0,i);
		X += ( f3(1,i)-E(1,i) )*( f3(1,i)-E(1,i) )/E(1,i);
	}

	int df = k-1;
//zkc 2002.12.3
	m_FieldNameStrAr.Add(strFieldName);
	double P = 1-chi2cdf(X,df);
	CPtrArray *subPtr=new CPtrArray;
	for(j=0;j<f3.mcol();j++)
	{
		CTString *p=new CTString;
		//CTString str;
		for(int i=0;i<f3.mrow();i++)
		{
			CTString str1;
			str1.Format("%d",(int)f3(i,j));
			(*p)+=str1;
			if(i!=f3.mrow()-1)
				(*p)+="\n";
		}
		subPtr->Add((void *)p);
	}
	m_StrPtr.Add((void *)subPtr);
	CTString str=">中位数\n<=中位数";
	m_ClassStrAr.Add(str);
	m_NAr.Add(N);
	m_S1Ar.Add(Mid);
//	if(fabs(R1)>DOUBLE_ZERO1&&fabs(R2)>DOUBLE_ZERO1)
//	{
		m_P1Ar.Add(X);
		m_P2Ar.Add(df);
		m_PAr.Add(P);
//	}
//	else
//	{
//		m_P1Ar.Add(X);
//		m_P2Ar.Add(df);
////		m_PAr.Add(P);
//	}
}


//***********************************************************************//
//Original author:ytLi													 //
//Modifier : zkc; 2003.3.10												 //
//Modifying reason :命名不规范，代码臃余，增加注释,结构调整，数据越界	 //
//Function ：McNemarsTest检验统计数据的求解								 //
//***********************************************************************//
void CNonParamTest::McNemarsTest(CTArray<double,double> &Data1,
								 CTArray<double,double> &Data2,
								 CTArray<double,double> &Weight,CTString &strFieldName)

{
	int i;
	double a(0),b(0),c(0),d(0);
	for( i=0; i<Data1.GetSize(); i++ )
	{
		if( fabs(Data1[i])<DOUBLE_ZERO1 &&	fabs(Data2[i])<DOUBLE_ZERO1 )
			a += Weight[i];
		else if( fabs(Data1[i])<DOUBLE_ZERO1 && fabs(Data2[i]-1)<DOUBLE_ZERO1 )
			b += Weight[i];
		else if( fabs(Data1[i]-1)<DOUBLE_ZERO1 && fabs(Data2[i])<DOUBLE_ZERO1 )
			c += Weight[i];
		else if( fabs(Data1[i]-1)<DOUBLE_ZERO1 && fabs(Data2[i]-1)<DOUBLE_ZERO1 )
			d += Weight[i];
		else 
		{
			CTString str=strFieldName;
			str+="中的数据中有0,1以外的值，不能进行麦克尼马尔检验.";
			if(m_p3RsltElementText==NULL)
				m_p3RsltElementText=new CRsltElementText("警告");
			m_p3RsltElementText->AddString(str);
			return;
		}
	}

	double r = (b<c)?b:c;
	double P = 0;
	double X;
	
	if( b+c<26 )
	{
		for( i=0; i<r+1; i++ )
		{
			P += Cnk(b+c, i);
		}	
		P /= pow(2,b+c-1);
	}
	else 
	{
		X = (fabs(b-c)-1)*(fabs(b-c)-1)/double(b+c);
		P = 1-chi2cdf(X,1);
	}

	//zkc 2002.12.3
	m_FieldNameStrAr.Add(strFieldName);

	CTString str,strtemp;
	CTString FieldName="前变量";
	str+=FieldName;
	str+=":0\n";
	str+=FieldName;
	str+=":1";
	m_ClassStrAr.Add(str);

	str.Empty();
	strtemp.Format("%.0f",a);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",c);
	str+=strtemp;
	m_MeanRandStrAr.Add(str);

	str.Empty();
	strtemp.Empty();
	strtemp.Format("%.0f",b);
	str+=strtemp;
	str+="\n";
	strtemp.Empty();
	strtemp.Format("%.0f",d);
	str+=strtemp;
	m_SumRandStrAr.Add(str);
	
	m_NAr.Add(a+b+c+d);
	m_PAr.Add((P>1)?1:P);
	CTString str1="";
	if(b+c>25)
	{	
		str1.Empty();
		str1.Format("%.4f",X);		
	}
	m_chiStrArray.Add(str1);
}

void CNonParamTest::GroupIntoTwo(CTArray<double,double> &ar1,
								 CTArray<double,double> &ar2,
								 CTArray<double,double> &Weight1,
								 CTArray<double,double> &Weight2,
								 double dValue1,double dValue2,
								 int nVarCol,TDataSet *pWeightArray,
								 TDataSet *pGroupArray)

{
	ar1.RemoveAll();
	ar2.RemoveAll();
	Weight1.RemoveAll();
	Weight2.RemoveAll();

	TDataSet *pDataArray=NULL;
	int nRow=m_pDataInterface->GetColData(nVarCol,pDataArray,5);
	
	for(int j=0;j<nRow;j++)
	{
			
		if(m_bWeight)//假如指定了权重
		{

			if(pGroupArray[j].iIndex>=0&&pWeightArray[j].iIndex>=0&&pDataArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//权重与数据列均未缺失
			{
				if(pGroupArray[j].dValue==dValue1)
				{
					Weight1.Add(floor(pWeightArray[j].dValue+0.5));
					ar1.Add(pDataArray[j].dValue);
				}
				if(pGroupArray[j].dValue==dValue2)
				{
					Weight2.Add(floor(pWeightArray[j].dValue+0.5));
					ar2.Add(pDataArray[j].dValue);
				}
			}
			else//否则记录被忽略的行
			{
				m_RowArray.Add(pDataArray[j].iIndex);
			}
		}
		else//假如未指定权中
		{
			if(pGroupArray[j].iIndex>=0&&pDataArray[j].iIndex>=0)
			{
				if(pGroupArray[j].dValue==dValue1)
				{
					Weight1.Add(1);
					ar1.Add(pDataArray[j].dValue);
				}
				if(pGroupArray[j].dValue==dValue2)
				{
					Weight2.Add(1);
					ar2.Add(pDataArray[j].dValue);
				}	
			}
			else
			{
				m_RowArray.Add(pDataArray[j].iIndex);
			}
		}
	}
	if(pDataArray)
	{
		delete []pDataArray;
		pDataArray=NULL;
	}	
}

void CNonParamTest::GetCroupStd(TDataSet *pGroupSet,int nRow,
								CTArray<double,double> &arStd,
								double max,double min)
{
	int i=0;
	for(i=0;i<nRow;i++)
	{
		if(pGroupSet[i].iIndex>=0)
		{
			if(pGroupSet[i].dValue<=max&&pGroupSet[i].dValue>=min)
			{
				int flag=0;
				for(int j=0;j<arStd.GetSize();j++)
				{
					if(pGroupSet[i].dValue==arStd.GetAt(j))
					{
						flag=1;
						break;
					}
				}
				if(flag==0)//没有找到相等的
					arStd.Add(pGroupSet[i].dValue);
			}
		}
	}
	int nSize=arStd.GetSize();
	for(i=0;i<nSize;i++)
	{
		for(int j=i+1;j<nSize;j++)
		{		
			double temp=arStd.GetAt(i);
			if(temp>=arStd.GetAt(j))
			{
				arStd.SetAt(i,arStd.GetAt(j));
				arStd.SetAt(j,temp);
			}
		}
	}

}


bool CNonParamTest::GroupIntoMany(CPtrArray &DataPtr,
								  TDataSet *pGroupArray,TDataSet *pWeightArray,
								  int nVarCol,CTArray<double,double> &arStd)
{
	TDataSet *pDataArray=NULL;
	int nRow=m_pDataInterface->GetColData(nVarCol,pDataArray,5);
	for(int i=0;i<arStd.GetSize();i++)
	{
		CPtrArray *pSubPtr=new CPtrArray;
		for(int j=0;j<nRow;j++)
		{
			if(m_bWeight)//假如指定了权重
			{

				if(pGroupArray[j].iIndex>=0&&pWeightArray[j].iIndex>=0&&pDataArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//权重与数据列均未缺失
				{
					if(pGroupArray[j].dValue==arStd.GetAt(i))
					{
						TManyGroup *p=new TManyGroup;
						p->dValue=pDataArray[j].dValue;
						p->nWeight=floor(pWeightArray[j].dValue+0.5);
						pSubPtr->Add((void *)p);
					}
				}
				else//否则记录被忽略的行
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
			else//假如未指定权重
			{
				if(pGroupArray[j].iIndex>=0&&pDataArray[j].iIndex>=0)
				{
					if(pGroupArray[j].dValue==arStd.GetAt(i))
					{
						TManyGroup *p=new TManyGroup;
						p->dValue=pDataArray[j].dValue;
						p->nWeight=1;
						pSubPtr->Add((void *)p);
					}
				}
				else
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}	
		}
		if(pSubPtr->GetSize()>=1)
			DataPtr.Add((void *)pSubPtr);
		else
		{
			for(int j=0;j<pSubPtr->GetSize();j++)
			{
				TManyGroup *p=(TManyGroup *)(pSubPtr->GetAt(j));
				delete p;
			}
			pSubPtr->RemoveAll();
			delete pSubPtr;
			pSubPtr=NULL;
			arStd.RemoveAt(i);
			i--;
		}
	}
	if(pDataArray)
	{
		delete []pDataArray;
		pDataArray=NULL;
	}
	return true;
}


//4
////////////////////////////////////
void	CNonParamTest::	CreateResultForMann(CTString strGroupFieldName)
{	
	int nRow=m_FieldNameStrAr.GetSize();
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		
		CTLTable * pTable = new CTLTable;
		pTable->CreateTable(2, 2);

		pTable->InsertColumn(0,"");	
		CTString strGroup=m_pDataInterface->GetFieldName(m_nGroupIndex1);
		pTable->InsertColumn(1,strGroup);
		pTable->InsertColumn(2,"频数");
		pTable->InsertColumn(3,"平均秩");
		pTable->InsertColumn(4,"秩和");	
		int i;
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_NStrAr.GetAt(i));
			pTable->SetItemText(i,3,m_MeanRandStrAr.GetAt(i));
			pTable->SetItemText(i,4,m_SumRandStrAr.GetAt(i));
		}
		pETable= new CRsltElementTable("秩",pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);

		//////////////////////检验结果输出/////////////////
		pTable = new CTLTable;
		pTable->CreateTable(nRow+1,5);
		pTable->SetHead("检验结果");
//		pTable->SetCols(5);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"曼-惠特尼U");
		pTable->InsertColumn(2,"威尔科克森W");
		pTable->InsertColumn(3,"Z");
		pTable->InsertColumn(4,"P 值");
		
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i), false);
			pTable->SetItemText(i,1,m_MeanWhiteyAr.GetAt(i));
			pTable->SetItemText(i,2,m_WilcoxonAr.GetAt(i));
			pTable->SetItemText(i,3,m_ZAr.GetAt(i));
			pTable->SetItemText(i,4,m_PAr.GetAt(i));
		}

		pETable= new CRsltElementTable("检验结果", pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}


//6
void	CNonParamTest::CreateResultForMoses(CTString strGroupFieldName)
{	
	int nRow=m_FieldNameStrAr.GetSize();
	if(nRow>0)
	{

		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("莫斯检验\n");
			pTable->InsertColumn(0,"结果说明");
			//pTable->m_colorCell=RGB(255,0,0);
			for(int i=0;i<nRow;i++)
			{
				CTString strtemp;
				if(m_P2Ar.GetAt(i)<=0.05)		
				{
					strtemp.Format("%s对%s值有显著影响(概率检验值=%.4f)。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData(),m_P2Ar.GetAt(i));
				}
				else
				{
					strtemp.Format("%s对%s值无显著影响(概率检验值=%.4f)。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData(),m_P2Ar.GetAt(i));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
			// to end the output discription.
		}
		
		
		//CRsltElement * pETable = NULL;
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,3);
		pTable->SetHead("频数");
//		pTable->SetCols(3);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		CTString strGroup=m_pDataInterface->GetFieldName(m_nGroupIndex1);
		pTable->InsertColumn(1,strGroup);
		pTable->InsertColumn(2,"频数");	
		int i;
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_NStrAr.GetAt(i));
		}
		CTString strTitle;
		strTitle.Empty();
		strTitle+="频数";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);

		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(nRow+1,6);
		pTable->SetHead("检验结果");
//		pTable->SetCols(6);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"跨度");
		pTable->InsertColumn(2,"P 值1");
		pTable->InsertColumn(3,"截头跨度");
		pTable->InsertColumn(4,"P 值2");
		pTable->InsertColumn(5,"截断长度");	
		
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,int(m_S1Ar.GetAt(i)));
			pTable->SetItemText(i,2,m_P1Ar.GetAt(i));
			pTable->SetItemText(i,3,int(m_S2Ar.GetAt(i)));
			pTable->SetItemText(i,4,m_P2Ar.GetAt(i));
			pTable->SetItemText(i,5,int(m_hAr.GetAt(i)));
		}

		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}

//7
void CNonParamTest::CreateResultForWald(CTString strGroupFieldName)
{	
	int nRow=m_FieldNameStrAr.GetSize();
	int i=0;
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("沃尔德-沃尔佛威茨游程检验\n");
			pTable->InsertColumn(0,"结果说明");
			//pTable->m_colorCell=RGB(255,0,0);
			for(i=0;i<nRow;i++)
			{
				CTString strtemp=m_PStrAr.GetAt(i);
				int index=strtemp.Find('\n');
				double min=atof(strtemp.Left(index));
				double max=atof(strtemp.Right(strtemp.GetLength()-index));
				if(min<=0.05&&max<=0.05)		
				{
					strtemp.Format("%s对%s值有显著影响。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData());
				}
				else if(min>0.05&&max>0.05)
				{
					strtemp.Format("%s对%s值无显著影响。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData());
				}
				else
				{
					strtemp.Format("%s对%s值有显著影响。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData());
					
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
			// to end the output discription.
		}



		//CRsltElement * pETable = NULL;
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,3);
		pTable->SetHead("频数");
//		pTable->SetCols(3);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		CTString strGroup=m_pDataInterface->GetFieldName(m_nGroupIndex1);
		pTable->InsertColumn(1,strGroup);
		pTable->InsertColumn(2,"频数");	

		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_NStrAr.GetAt(i));
		}
		CTString strTitle;
		strTitle.Empty();
		strTitle+="频数";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);

		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(nRow+1,5);
		pTable->SetHead("检验结果");
//		pTable->SetCols(5);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1," ");
		pTable->InsertColumn(2,"游程数");
		pTable->InsertColumn(3,"Z");
		pTable->InsertColumn(4,"P 值(双尾)");
		
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_MeanRandStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_SumRandStrAr.GetAt(i));
			pTable->SetItemText(i,3,m_ZStrAr.GetAt(i));
			pTable->SetItemText(i,4,m_PStrAr.GetAt(i));
		}
		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}

void	CNonParamTest::CreateResultForKolmogorov(CTString strGroupFieldName)
{
	int nRow=m_FieldNameStrAr.GetSize();
	int i=0;
	if(nRow>0)
	{	
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("柯尔莫哥洛夫-斯米尔诺夫检验\n");
			pTable->InsertColumn(0,"结果说明");
			//pTable->m_colorCell=RGB(255,0,0);
			for(i=0;i<nRow;i++)
			{
				CTString strtemp;
				if(m_PAr.GetAt(i)<=0.05)		
				{
					strtemp.Format("%s对%s值有显著影响(概率检验值=%.4f)。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i)<=0.05);
				}
				else
				{
					strtemp.Format("%s对%s值无显著影响(概率检验值=%.4f)。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i)<=0.05);
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
			// to end the output discription.
		}
		
		
		//CRsltElement * pETable = NULL;
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,3);
		pTable->SetHead("频数");
//		pTable->SetCols(3);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		CTString strGroup=m_pDataInterface->GetFieldName(m_nGroupIndex1);
		pTable->InsertColumn(1,strGroup);
		pTable->InsertColumn(2,"频数");	

		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_NStrAr.GetAt(i));
		}
		CTString strTitle;
		strTitle.Empty();
		strTitle+="频数";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(nRow+1,6);
		pTable->SetHead("检验结果");
//		pTable->SetCols(6);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"绝对差");
		pTable->InsertColumn(2,"正差");
		pTable->InsertColumn(3,"负差");
		pTable->InsertColumn(4,"Z");
		pTable->InsertColumn(5,"P 值"); 

		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_S1Ar.GetAt(i));
			pTable->SetItemText(i,2,m_P1Ar.GetAt(i));
			pTable->SetItemText(i,3,m_P2Ar.GetAt(i));
			pTable->SetItemText(i,4,m_ZAr.GetAt(i));
			pTable->SetItemText(i,5,m_PAr.GetAt(i));

		}

		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}


//11
void CNonParamTest::CreateResultForCruskal(CTString strGroupFieldName)
{	
	int nRow=m_FieldNameStrAr.GetSize();
	int i=0;
	if(nRow>0)
	{

		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("克拉夏尔-瓦里斯检验\n");
			pTable->InsertColumn(0,"结果说明");
			//pTable->m_colorCell=RGB(255,0,0);
			for(i=0;i<nRow;i++)
			{
				CTString strtemp;
				if(m_PAr.GetAt(i)<=0.05)		
				{
					strtemp.Format("%s对%s值有显著影响(概率检验值=%.4f)。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i),m_PAr.GetAt(i));
				}
				else
				{
					strtemp.Format("%s对%s值无显著影响(概率检验值=%.4f)。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i),m_PAr.GetAt(i));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
			// to end the output discription.
		}
		
		//CRsltElement * pETable = NULL;
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,4);
		pTable->SetHead("秩");
//		pTable->SetCols(4);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		CTString strGroup=m_pDataInterface->GetFieldName(m_nGroupIndex2);
		pTable->InsertColumn(1,strGroup);
		pTable->InsertColumn(2,"频数");
		pTable->InsertColumn(3,"平均秩");

		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_NStrAr.GetAt(i));
			pTable->SetItemText(i,3,m_MeanRandStrAr.GetAt(i));			
		}
		CTString strTitle;
		strTitle.Empty();
		strTitle+="秩";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(nRow+1,4);
		pTable->SetHead("检验结果");
//		pTable->SetCols(4);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"卡方值");
		pTable->InsertColumn(2,"自由度");
		pTable->InsertColumn(3,"P 值");
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_P2Ar.GetAt(i));
			pTable->SetItemText(i,2,int(m_ZAr.GetAt(i)));
			pTable->SetItemText(i,3,m_PAr.GetAt(i));
		}

		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}

void	CNonParamTest::CreateResultForMedian(CTArray<double,double>&stdAr,CTString strGroupFieldName)
{	
	int nRow=m_FieldNameStrAr.GetSize();
	int i=0;
	if(nRow>0)
	{

		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->SetTitle("中位数检验\n");
			pTable->CreateTable(nRow+1,1);
			pTable->InsertColumn(0,"结果说明");
			//pTable->m_colorCell=RGB(255,0,0);
			for(i=0;i<nRow;i++)
			{
				CTString strtemp;
				if(m_PAr.GetAt(i)<=0.05)		
				{
					strtemp.Format("%s对%s值有显著影响(概率检验值=%.4f)。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
				}
				else
				{
					strtemp.Format("%s对%s值无显著影响(概率检验值=%.4f)。\n",strGroupFieldName.GetData(),m_FieldNameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
		}
		//CRsltElement * pETable = NULL;
		CTLTable * pTable = new CTLTable;	
		int nCol=stdAr.GetSize();
		pTable->SetHead("频数");
		pTable->CreateTable(nRow+1,nCol+2);
		pTable->InsertColumn(0,"");	
		CTString strGroup=m_pDataInterface->GetFieldName(m_nGroupIndex2);
		pTable->InsertColumn(1,strGroup);

		int nSize=stdAr.GetSize();
		CField * pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nGroupIndex2);
		for(i=0;i<stdAr.GetSize();i++)
		{
			CTString str;
			
			CTString labelVal;
			if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(stdAr.GetAt(i),labelVal))
			{
				str=labelVal;
			}
			else
			{
				if(stdAr.GetAt(i)==int(stdAr.GetAt(i)))
					str.Format("%d",int(stdAr.GetAt(i)));
				else
					str.Format("%.3f",stdAr.GetAt(i));
			}				
			pTable->InsertColumn(i+2,str);
		}

		for(i=0;i<m_StrPtr.GetSize();i++)
		{			
			int nSizePtr=m_StrPtr.GetSize();
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			CPtrArray *p=(CPtrArray *)(m_StrPtr.GetAt(i));
			for(int j=0;j<p->GetSize();j++)
			{
				int nSizeP=p->GetSize();
				CTString str=*((CTString *)(p->GetAt(j)));
				pTable->SetItemText(i,j+2,*((CTString *)(p->GetAt(j))));
			}
		}
		CTString strTitle;
		strTitle.Empty();
		strTitle+="频数";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);


		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(nRow+1,6);
		pTable->SetHead("检验结果");
//		pTable->SetCols(6);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"样本数");
		pTable->InsertColumn(2,"中位数");
		pTable->InsertColumn(3,"卡方值");
		pTable->InsertColumn(4,"自由度");
		pTable->InsertColumn(5,"P 值");	

		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_NAr.GetAt(i));
			pTable->SetItemText(i,2,m_S1Ar.GetAt(i));
			pTable->SetItemText(i,3,m_P1Ar.GetAt(i));
			pTable->SetItemText(i,4,m_P2Ar.GetAt(i));
			pTable->SetItemText(i,5,m_PAr.GetAt(i));
		}

		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}

void	CNonParamTest::CreateResultForWilcoxon()
{
	int nRow=m_FieldNameStrAr.GetSize();
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("威尔科克森配对秩和检验\n");
			pTable->InsertColumn(0,"结果说明");
//			pTable->m_colorCell=RGB(255,0,0);
			for(int i=0;i<nRow;i++)
			{
				CTString strtemp;
				if(m_PAr.GetAt(i)<=0.05)		
				{
					strtemp.Format("%s值有显著差别(概率检验值=%.4f)。\n",m_FieldNameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
				}
				else
				{
					strtemp.Format("%s值无显著差别(概率检验值=%.4f)。\n",m_FieldNameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
			// to end the output discription.
		}
		//CRsltElement * pETable = NULL;
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,5);
		pTable->SetHead("秩");
//		pTable->SetCols(5);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"");
		pTable->InsertColumn(2,"频数");
		pTable->InsertColumn(3,"平均秩");
		pTable->InsertColumn(4,"秩和");
		int i;		
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_NStrAr.GetAt(i));
			pTable->SetItemText(i,3,m_MeanRandStrAr.GetAt(i));
			pTable->SetItemText(i,4,m_SumRandStrAr.GetAt(i));			
		}
		CTString strTitle;
		strTitle.Empty();
		strTitle+="秩";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(nRow+1,3);
		pTable->SetHead("检验结果");
//		pTable->SetCols(3);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"Z");
		pTable->InsertColumn(3,"P 值");
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ZAr.GetAt(i));
			pTable->SetItemText(i,2,m_PAr.GetAt(i));
		}
		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}

//9
void	CNonParamTest::CreateResultForSign()
{
	int nRow=m_FieldNameStrAr.GetSize();
	if(nRow>0)
	{

		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("符号检验\n");
			pTable->InsertColumn(0,"结果说明");
//			pTable->m_colorCell=RGB(255,0,0);
			for(int i=0;i<m_SumRandStrAr.GetSize();i++)
			{
				CTString strtemp;
				if(atof(m_SumRandStrAr.GetAt(i))<=0.05)		
				{
					strtemp.Format("%s值有显著差别(概率检验值=%.4f)。\n",m_FieldNameStrAr.GetAt(i).GetData(),atof(m_SumRandStrAr.GetAt(i)));
				}
				else
				{
					strtemp.Format("%s值无显著差别(概率检验值=%.4f)。\n",m_FieldNameStrAr.GetAt(i).GetData(),atof(m_SumRandStrAr.GetAt(i)));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
			// to end the output discription.
		}
		//CRsltElement * pETable = NULL;
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,5);
		pTable->SetHead("秩");

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"");
		pTable->InsertColumn(2,"频数");
		pTable->InsertColumn(3,"Z");
		pTable->InsertColumn(4,"P 值");
		for(int i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_NStrAr.GetAt(i));
			pTable->SetItemText(i,3,m_MeanRandStrAr.GetAt(i));
			pTable->SetItemText(i,4,m_SumRandStrAr.GetAt(i));
		}
		CTString strTitle;
		strTitle.Empty();
		strTitle+="频数与检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}

void	CNonParamTest::CreateResultForMcnema()
{
	int nRow=m_FieldNameStrAr.GetSize();
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		CTLTable * pTable = new CTLTable;
		pTable->CreateTable(nRow+1,1);
		pTable->SetTitle("麦克尼马尔检验\n");
		pTable->InsertColumn(0,"结果说明");
		//pTable->m_colorCell=RGB(255,0,0);
		int i=0;
		for(i=0;i<nRow;i++)
		{
			CTString strtemp;
			if(m_PAr.GetAt(i)<=0.05)		
			{
				strtemp.Format("%s值有显著差别(概率检验值=%.4f)。\n",m_FieldNameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
			}
			else
			{
				strtemp.Format("%s值无显著差别(概率检验值=%.4f)。\n",m_FieldNameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
			}
			pTable->InsertItem(i+1,strtemp);
		}
		pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
		pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,5);
		pTable->SetHead("交叉表");

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"");
		pTable->InsertColumn(2,"后变量\n0");
		pTable->InsertColumn(3,"后变量\n1");
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_ClassStrAr.GetAt(i));
			pTable->SetItemText(i,2,m_MeanRandStrAr.GetAt(i));
			pTable->SetItemText(i,3,m_SumRandStrAr.GetAt(i));			
		}
		CTString strTitle;
		strTitle.Empty();
		strTitle+="交叉表";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(nRow+1,4);
		pTable->SetHead("检验结果");
//		pTable->SetCols(4);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"样本数");
		pTable->InsertColumn(2,"卡方值");
		pTable->InsertColumn(3,"P 值");
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,m_NAr.GetAt(i));
			pTable->SetItemText(i,2,m_chiStrArray.GetAt(i));
			pTable->SetItemText(i,3,m_PAr.GetAt(i));
		}
		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}


void	CNonParamTest::	CreateResultForFriedman(int n,double F,double df,double P,CDoubleVector C)

{
	int nRow=m_FieldNameStrAr.GetSize();
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("弗里德曼检验\n");
			pTable->InsertColumn(0,"结果说明");
			//pTable->m_colorCell=RGB(255,0,0);
			CTString strtemp;
			if(P<=0.05)		
			{
				strtemp.Format("多个相关样本之间有显著差别(概率检验值=%.4f)。\n",P);
			}
			else
			{
				strtemp.Format("多个相关样本之间无显著差别(概率检验值=%.4f)。\n",P);
			}
			pTable->InsertItem(1,strtemp);
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
		}
		//CRsltElement * pETable = NULL;
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,3);
		pTable->SetHead("秩");

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"平均秩");
		pTable->InsertColumn(2,"秩和");
		
		int i=0;
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,C[i]/n);
			pTable->SetItemText(i,2,C[i]);
		}
		CTString strTitle;
		strTitle+="秩";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(2,4);
		pTable->SetHead("检验结果");

		pTable->InsertColumn(0,"样本数");	
		pTable->InsertColumn(1,"卡方值");
		pTable->InsertColumn(2,"自由度");
		pTable->InsertColumn(3,"P 值");

		CTString str;
		str.Format("%d",n);
		pTable->InsertItem(1,str);
		pTable->SetItemText(1,1,F);
		pTable->SetItemText(1,2,int(df));
		pTable->SetItemText(1,3,P);

		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}

void	CNonParamTest::CreateResultForKendall(CDoubleVector C,int n,double W,double li,double df,double P)
{
	int nRow=m_FieldNameStrAr.GetSize();
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("肯德尔W检验\n");
			pTable->InsertColumn(0,"结果说明");
			//pTable->m_colorCell=RGB(255,0,0);
			CTString strtemp;
			if(P<=0.05)		
			{
				strtemp.Format("多个相关样本之间有显著差别(概率检验值=%.4f)。\n",P);
			}
			else
			{
				strtemp.Format("多个相关样本之间无显著差别(概率检验值=%.4f)。\n",P);
			}
			pTable->InsertItem(1,strtemp);
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
			// to end the output discription.
		}
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,3);
		pTable->SetHead("秩");

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"平均秩");
		pTable->InsertColumn(2,"秩和");

		for(int i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			pTable->SetItemText(i,1,C[i]/n);
			pTable->SetItemText(i,2,C[i]);
		}
		CTString strTitle;
		strTitle+="秩";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(2,5);
		pTable->SetHead("检验结果");

		pTable->InsertColumn(0,"样本数");
		pTable->InsertColumn(1,"W");
		pTable->InsertColumn(2,"卡方值");
		pTable->InsertColumn(3,"自由度");
		pTable->InsertColumn(4,"P 值");

		CTString str;
		str.Format("%d",n);
		pTable->InsertItem(1,str);
		pTable->SetItemText(1,1,W);
		pTable->SetItemText(1,2,li);
		pTable->SetItemText(1,3,df);
		pTable->SetItemText(1,4,P);

		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);	
	}
}


void	CNonParamTest::CreateResultForCochran(CDoubleMatrix dmar,int n,double Q,double df,double P)
{
	int nRow=m_FieldNameStrAr.GetSize();
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->SetTitle("科克伦（Cohran） Q 检验\n");
			pTable->InsertColumn(0,"结果说明");
			//pTable->m_colorCell=RGB(255,0,0);
			CTString strtemp;
			if(P<=0.05)		
			{
				strtemp.Format("多个相关样本之间有显著差别(概率检验值=%.4f)。\n",P);
			}
			else
			{
				strtemp.Format("多个相关样本之间无显著差别(概率检验值=%.4f)。\n",P);
			}
			
			pTable->InsertItem(1,strtemp);
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pRsltVariable->Add(pETable);
			// to end the output discription.
		}
		CTLTable * pTable = new CTLTable;	
		pTable->CreateTable(nRow+1,3);
		pTable->SetHead("频数");

		pTable->InsertColumn(0,"");	
		pTable->InsertColumn(1,"0");
		pTable->InsertColumn(2,"1");

		for(int i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_FieldNameStrAr.GetAt(i));
			CTString str;
			str.Format("%.0f",floor(dmar(i,0)));
			pTable->SetItemText(i,1,str);	
			str.Format("%.0f",floor(dmar(i,1)));
			pTable->SetItemText(i,2,str);
		}
		CTString strTitle;
		strTitle+="频数";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
		//////////////////////检验结果输出/////////////////
		pTable=new CTLTable;
		pTable->CreateTable(2,4);
		pTable->SetHead("检验结果");

		pTable->InsertColumn(0,"样本数");	
		pTable->InsertColumn(1,"卡方值");
		pTable->InsertColumn(2,"自由度");
		pTable->InsertColumn(3,"（渐进）P 值");
		
		CTString str;
		str.Format("% .0f",n);
		pTable->InsertItem(1,str);
		pTable->SetItemText(1,1,Q);
		pTable->SetItemText(1,2,df);
		pTable->SetItemText(1,3,P);

		strTitle.Empty();
		strTitle+="检验结果";
		pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
		m_pRsltVariable->Add(pETable);
	}
}

bool CNonParamTest::GetValueFromVo(CDataInterface *pDataInterface, string VO, int nType)
{
	AnaWord VOstr;
	VOstr.Import(VO);

	TIntStruct tVarWeight;
//		/WEIGHT [varname]
	if (AnaSyntax::exist("/WEIGHT", VOstr) )
	{
		CTStringArray xname;
		TIntStruct tVarWeight;
		AnaSyntax::getVAR("/WEIGHT", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarWeight)))
			return false;
		if (tVarWeight.iCount == 1)
			m_nWeight = tVarWeight.pValue[0];
		else
			return false;
		m_bWeight = true;
	}
	else
		m_bWeight = false;

	if (nType == 1)//		nType=1:两个独立样本检验
	{
//	TWODEPTEST
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/WEIGHT [varname]
//		/BYVAR [varname] 
//		/VARGROUP GROUP({0**},{1**})]
//		/METHOD [MANN**] [KOLM] [MOSES] [WALD]
		CTStringArray xname;
		AnaSyntax::getVAR("/VARLIST", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo1)))
			return false;

		xname.RemoveAll();
		AnaSyntax::getVAR("/BYVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname, tVarWeight)))
			return false;
		if (tVarWeight.iCount == 1)
			m_nGroupIndex1 = tVarWeight.pValue[0];
		else
			return false;
		m_dValue1 = AnaSyntax::getvalue("/VARGROUP", "GROUP", VOstr);
		m_dValue2 = AnaSyntax::getvalue("/VARGROUP", "GROUP", VOstr, 2);

		m_bKolmogorov = AnaSyntax::findstr("/METHOD", "KOLM", VOstr);
		m_bMann = AnaSyntax::findstr("/METHOD", "MANN", VOstr);
		m_bMoses = AnaSyntax::findstr("/METHOD", "MOSES", VOstr);
		m_bWald  = AnaSyntax::findstr("/METHOD", "WALD", VOstr);
		if (!m_bKolmogorov && !m_bMoses && !m_bWald)//必须由一种方法
			m_bMann = true;
		
		return true;
	}

	if (nType == 2)///nType=2：多个独立样本检验
	{
//	MULTIDEPTEST
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/BYVAR [varname]
//		/VARGROUP GROUP({0**},{1**})]
//		/WEIGHT [varname]
//		/METHOD [KRUS**] [MEDIAN]
		CTStringArray xname;
		AnaSyntax::getVAR("/VARLIST", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo2)))
			return false;

		xname.RemoveAll();
		AnaSyntax::getVAR("/BYVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname, tVarWeight)))
			return false;
		if (tVarWeight.iCount == 1)
			m_nGroupIndex2 = tVarWeight.pValue[0];
		else
			return false;
		m_dmValue1 = AnaSyntax::getvalue("/VARGROUP", "GROUP", VOstr);
		m_dmValue2 = AnaSyntax::getvalue("/VARGROUP", "GROUP", VOstr, 2);

		m_bMedian = AnaSyntax::findstr("/METHOD", "MEDIAN", VOstr);
		if (m_bMedian)
			m_bCruskal = AnaSyntax::findstr("/METHOD", "KRUS", VOstr);
		else
			m_bCruskal = true;

		return true;
	}

	if (nType == 3)//nType=3：两个相关样本检验
	{
//	TWOCORRTEST
//		/DATA=[FILE]
//		/PAIREDVAR [varname varname] [varname varname] ...
//		/WEIGHT [varname]
//		/METHOD [WILCOXON**] [SIGN] [MCNEMAR]
		CTStringArray xname;
		AnaSyntax::getVAR("/PAIREDVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo3)))
			return false;
		if (int(m_tVarInfo3.iCount/2)*2 != m_tVarInfo3.iCount)
			return false;

		m_bMcnema = AnaSyntax::findstr("/METHOD", "MCNEMAR", VOstr);
		m_bSign = AnaSyntax::findstr("/METHOD", "SIGN", VOstr);
		if (!m_bMcnema && !m_bSign)
			m_bWilcoxon = true;
		else
			m_bWilcoxon = AnaSyntax::findstr("/METHOD", "WILCOXON", VOstr);

		return true;
	}


	if (nType == 4)//nType=4：多个相关样本检验
	{
//	MULTICORRTEST
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/WEIGHT [varname]
//		/METHOD [FRIEDMAN**] [KENDALL] [COHRAN]
		CTStringArray xname;
		AnaSyntax::getVAR("/VARLIST", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo3)))
			return false;

		m_bCochran = AnaSyntax::findstr("/METHOD", "COHRAN", VOstr);
		m_bKendall = AnaSyntax::findstr("/METHOD", "KENDALL", VOstr);
		if (!m_bCochran && !m_bKendall)
			m_bFriedman = true;
		else
			m_bFriedman = AnaSyntax::findstr("/METHOD", "WILCOXON", VOstr);

		return true;
	}

	return false;
}

CResult *CNonParamTest::OnNonParamTest(CDataInterface *pDataInterface, string VO, int nType)
{
	if (!GetValueFromVo(pDataInterface, VO, nType))
	{
		CResult* pResult = new CResult("非参数检验");
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}	
	m_p1RsltElementText=NULL;
	m_p2RsltElementText=NULL;
	m_p3RsltElementText=NULL;
	m_p4RsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	m_nType = nType;
	CDataAccess *pDataAccess=&(m_pDataInterface->m_DataAccess);
	TDataSet *pWeightArray=NULL;
	if(m_bWeight)
		m_pDataInterface->GetColData(m_nWeight,pWeightArray,5);

	switch(nType)
	{
	case 1://4-7
		{
			m_pResult=new CResult("两个独立样本检验");
			TDataSet *pDataSet=NULL;
			TDataSet *pGroupSet=NULL;
			CTArray<double,double> V1,V2;
			CTArray<double,double> Weight1,Weight2;
			int nRow=m_pDataInterface->GetColData(m_nGroupIndex1,pGroupSet,5);
			CTString strGroupFieldName=m_pDataInterface->GetFieldName(m_nGroupIndex1);

			if(m_bMann)
			{
				m_pRsltVariable =new CRsltVariable("曼-惠特尼U检验");
				m_pResult->Add(m_pRsltVariable);
				//FreeAllMemember();
				for(int i=0;i<m_tVarInfo1.iCount;i++)
				{
					int nIndex=m_tVarInfo1.pValue[i];
					GroupIntoTwo(V1,V2,Weight1,Weight2,m_dValue1,m_dValue2,nIndex,pWeightArray,pGroupSet);
					CTString str=pDataInterface->GetFieldName(nIndex);
					if(V1.GetSize()<1||V2.GetSize()<1)
					{
						CTString str1=str;
						str1+="分组以后，有效数据太少,不能进行曼-惠特尼U检验！";
						if(m_p1RsltElementText==NULL)
							m_p1RsltElementText=new CRsltElementText("警告");
						m_p1RsltElementText->AddString(str1);
						continue;
					}
					MannWhitneyU(V1,V2,Weight1,Weight2,str);
				}
				//delete []pGroupData;
				CreateResultForMann(strGroupFieldName);
				if(m_p1RsltElementText)
					m_pRsltVariable->Add(m_p1RsltElementText);
			}
			if(m_bMoses)
			{
				m_pRsltVariable =new CRsltVariable("莫斯检验");
				m_pResult->Add(m_pRsltVariable);
				FreeAllMemember();
				for(int i=0;i<m_tVarInfo1.iCount;i++)
				{
					int nIndex=m_tVarInfo1.pValue[i];
					GroupIntoTwo(V1,V2,Weight1,Weight2,m_dValue1,m_dValue2,nIndex,pWeightArray,pGroupSet);
					CTString str=pDataInterface->GetFieldName(nIndex);
					if(V1.GetSize()<1||V2.GetSize()<1)
					{
						CTString str1=str;
						str1+="分组以后，有效数据太少，不能进行莫斯检验！";
						if(m_p2RsltElementText==NULL)
							m_p2RsltElementText=new CRsltElementText("警告");
						m_p2RsltElementText->AddString(str1);
						continue;
					}
					MosesTest(V1,V2,Weight1,Weight2,str);
				}
				CreateResultForMoses(strGroupFieldName);
				if(m_p2RsltElementText)
					m_pRsltVariable->Add(m_p2RsltElementText);
			}
			if(m_bKolmogorov)
			{
				m_pRsltVariable =new CRsltVariable("柯尔莫哥洛夫-斯米尔诺夫检验");
				m_pResult->Add(m_pRsltVariable);
				FreeAllMemember();
				for(int i=0;i<m_tVarInfo1.iCount;i++)
				{
					int nIndex=m_tVarInfo1.pValue[i];
					GroupIntoTwo(V1,V2,Weight1,Weight2,m_dValue1,m_dValue2,nIndex,pWeightArray,pGroupSet);
					CTString str=pDataInterface->GetFieldName(nIndex);
					if(V1.GetSize()<1||V2.GetSize()<1)
					{
						CTString str1=str;
						str1+="分组以后，有效数据太少，不能进行柯尔莫哥洛夫-斯米尔诺夫检验！";
						if(m_p4RsltElementText==NULL)
							m_p4RsltElementText=new CRsltElementText("警告");
						m_p4RsltElementText->AddString(str1);
						continue;
					}
					KolmogorovSmirnovTwoSampleTest(V1,V2,Weight1,Weight2,str);
				}
				CreateResultForKolmogorov(strGroupFieldName);
				if(m_p4RsltElementText)
					m_pRsltVariable->Add(m_p4RsltElementText);
			}
			if(m_bWald)
			{
				m_pRsltVariable =new CRsltVariable("沃尔德-沃尔佛威茨游程检验");
				m_pResult->Add(m_pRsltVariable);
				FreeAllMemember();
				for(int i=0;i<m_tVarInfo1.iCount;i++)
				{
					int nIndex=m_tVarInfo1.pValue[i];
					GroupIntoTwo(V1,V2,Weight1,Weight2,m_dValue1,m_dValue2,nIndex,pWeightArray,pGroupSet);
					CTString str=pDataInterface->GetFieldName(nIndex);
					if(V1.GetSize()<1||V2.GetSize()<1)
					{
						CTString str1=str;
						str1+="分组以后，有效数据太少，不能进行沃尔德-沃尔佛威茨游程检验！";
						if(m_p3RsltElementText==NULL)
							m_p3RsltElementText=new CRsltElementText("警告");
						m_p3RsltElementText->AddString(str1);
						continue;
					}
					WaldWolfowitzTest(V1,V2,Weight1,Weight2,str);
				}
				CreateResultForWald(strGroupFieldName);
				if(m_p3RsltElementText)
					m_pRsltVariable->Add(m_p3RsltElementText);
			}

			if(pGroupSet)
			{
				delete []pGroupSet;
				pGroupSet=NULL;
			}
			if(pWeightArray)
			{
				delete []pWeightArray;
				pWeightArray=NULL;
			}
			return m_pResult;
		}
		break;

	case 2:
		{
			m_pResult=new CResult("多个独立样本检验");
			TDataSet *pGroupSet=NULL;
			int nRow=m_pDataInterface->GetColData(m_nGroupIndex2,pGroupSet,5);
			CTArray<double,double> arStd;
			CTString strGroupFieldName=m_pDataInterface->GetFieldName(m_nGroupIndex2);
			
			GetCroupStd(pGroupSet,nRow,arStd,m_dmValue2,m_dmValue1);

			int index[] ={1};
			if(m_bCruskal)
			{			
				m_pRsltVariable =new CRsltVariable("克拉夏尔-瓦里斯检验");
				m_pResult->Add(m_pRsltVariable);
				FreeAllMemember();
				for(int i=0;i<m_tVarInfo2.iCount;i++)
				{
					int nIndex=m_tVarInfo2.pValue[i];
					CPtrArray DataPtr;
					CTArray<double,double> arStdTemp;
					arStdTemp.RemoveAll();
					for(int m=0;m<arStd.GetSize();m++)
						arStdTemp.Add(arStd[m]);
					GroupIntoMany(DataPtr,pGroupSet,pWeightArray,nIndex,arStdTemp);
					CTString str=pDataInterface->GetFieldName(nIndex);
					if(DataPtr.GetSize()<3)
					{
						DelDataPtrArray(DataPtr);
						CTString str1=str;
						str1+="分组以后，小于3个分类组，不能进行克拉夏尔-瓦里斯检验！";
						if(m_p1RsltElementText==NULL)
							m_p1RsltElementText=new CRsltElementText("警告");
						m_p1RsltElementText->AddString(str1);						
						continue;
						////可以增加不处理的提示情况
					}
					KruskalWallisHTest(DataPtr,arStdTemp,str);
					DelDataPtrArray(DataPtr);
				}
				CreateResultForCruskal(strGroupFieldName);
				if(m_p1RsltElementText)
					m_pRsltVariable->Add(m_p1RsltElementText);
			}
			if(m_bMedian)
			{
				m_pRsltVariable =new CRsltVariable("中位数检验");
				m_pResult->Add(m_pRsltVariable);
				FreeAllMemember();
				for(int i=0;i<m_tVarInfo2.iCount;i++)
				{
					CTArray<double,double> arStdTemp;
					arStdTemp.RemoveAll();
					for(int m=0;m<arStd.GetSize();m++)
						arStdTemp.Add(arStd[m]);						
					int nIndex=m_tVarInfo2.pValue[i];
					CPtrArray DataPtr;
					GroupIntoMany(DataPtr,pGroupSet,pWeightArray,nIndex,arStdTemp);
					CTString str=pDataInterface->GetFieldName(nIndex);
					if(DataPtr.GetSize()<3)
					{
						DelDataPtrArray(DataPtr);
						CTString str1=str;
						str1+="分组以后，小于3个分类组，不能进行中位数检验！";
						if(m_p2RsltElementText==NULL)
							m_p2RsltElementText=new CRsltElementText("警告");
						m_p2RsltElementText->AddString(str1);						
						continue;
						////可以增加不处理的提示情况
					}
					MedianTest(DataPtr,arStdTemp,str);
					DelDataPtrArray(DataPtr);
				}
				
				CreateResultForMedian(arStd,strGroupFieldName);
				if(m_p2RsltElementText)
					m_pRsltVariable->Add(m_p2RsltElementText);					
			}
			arStd.RemoveAll();
			if(pWeightArray)
			{
				delete []pWeightArray;
				pWeightArray=NULL;
			}
			if(pGroupSet)
			{
				delete []pGroupSet;
				pGroupSet=NULL;
			}
			return m_pResult;
		}
		break;
	case 3:
		{
			m_pResult=new CResult("两个相关样本检验");
			int nLen=pDataInterface->GetRowCount();
			CTArray<double,double> Data1,Data2;
			CTArray<double,double> Weight;
			
			if(m_bWilcoxon)
			{
				m_pRsltVariable =new CRsltVariable("威尔科克森配对秩和检验");
				m_pResult->Add(m_pRsltVariable);
				FreeAllMemember();
				for(int i=0; i<m_tVarInfo3.iCount/2; i++)
				{
					int column1 = m_tVarInfo3.pValue[i*2];
					int column2 = m_tVarInfo3.pValue[i*2+1];
				//	m_FieldNameStrAr.Add(p->str);
					GetTwoRelatedData(Data1,Data2,Weight,pWeightArray,column1,column2);
					if(Data1.GetSize()<1)
					{
						Data1.RemoveAll();
						Data2.RemoveAll();
						Weight.RemoveAll();
						CTString str1 = "";
						str1 += "有效数据太少，不能进行威尔科克森配对秩和检验！";
						if(m_p1RsltElementText==NULL)
							m_p1RsltElementText=new CRsltElementText("警告");
						m_p1RsltElementText->AddString(str1);						
						continue;
					}
					CTString strTempNouse = "";
					WilcoxonMatchedPairsSignRankTest(Data1, Data2, Weight, strTempNouse);
					Data1.RemoveAll();
					Data2.RemoveAll();
					Weight.RemoveAll();
				}
				//FreeTwoRelated();
				CreateResultForWilcoxon();
				if(m_p1RsltElementText)
					m_pRsltVariable->Add(m_p1RsltElementText);					

			}
			if(m_bSign)
			{
				m_pRsltVariable =new CRsltVariable("符号检验");
				m_pResult->Add(m_pRsltVariable);
				FreeAllMemember();
				//modify by xds 20060628
				for(int i=0; i<m_tVarInfo3.iCount/2; i++)
				{
					int column1 = m_tVarInfo3.pValue[i*2];
					int column2 = m_tVarInfo3.pValue[i*2+1];
					CTString szTemp("");
					szTemp.Format("%s-%s",m_pDataInterface->GetFieldName(column1).GetData(),m_pDataInterface->GetFieldName(column2).GetData());
					//m_FieldNameStrAr.Add(szTemp);
					GetTwoRelatedData(Data1,Data2,Weight,pWeightArray,column1,column2);
					if(Data1.GetSize()<1)
					{
						Data1.RemoveAll();
						Data2.RemoveAll();
						Weight.RemoveAll();
						CTString str1=szTemp;
						str1+="有效数据太少，不能进行符号检验！";
						if(m_p2RsltElementText==NULL)
							m_p2RsltElementText=new CRsltElementText("警告");
						m_p2RsltElementText->AddString(str1);						
						continue;
					}
					CTString strTempNouse;
					SignTest(Data1,Data2, Weight, strTempNouse);
					Data1.RemoveAll();
					Data2.RemoveAll();
					Weight.RemoveAll();
				}	
				//FreeTwoRelated();
				CreateResultForSign();
				if(m_p2RsltElementText)
					m_pRsltVariable->Add(m_p2RsltElementText);					
			}
			if(m_bMcnema)
			{
				m_pRsltVariable =new CRsltVariable("麦克尼马尔检验");
				m_pResult->Add(m_pRsltVariable);
				FreeAllMemember();
				//modify by xds 20060628
				for(int i=0; i<m_tVarInfo3.iCount/2; i++)
				{
					int column1 = m_tVarInfo3.pValue[i*2];
					int column2 = m_tVarInfo3.pValue[i*2+1];
					CTString szTemp("");
					szTemp.Format("%s-%s",m_pDataInterface->GetFieldName(column1).GetData(),m_pDataInterface->GetFieldName(column2).GetData());
					//m_FieldNameStrAr.Add(szTemp);
					GetTwoRelatedData(Data1,Data2,Weight,pWeightArray,column1,column2);
					if(Data1.GetSize()<1)
					{
						Data1.RemoveAll();
						Data2.RemoveAll();
						Weight.RemoveAll();
						CTString str1=szTemp;
						str1+="有效数据太少，不能进行麦克尼马尔检验！";
						if(m_p3RsltElementText==NULL)
							m_p3RsltElementText=new CRsltElementText("警告");
						m_p3RsltElementText->AddString(str1);						
						continue;
					}
					McNemarsTest(Data1,Data2,Weight,szTemp);
					Data1.RemoveAll();
					Data2.RemoveAll();
					Weight.RemoveAll();
				}					
				CreateResultForMcnema();
				if(m_p3RsltElementText)
					m_pRsltVariable->Add(m_p3RsltElementText);					
			}
			FreeTwoRelated();
			if(pWeightArray)
			{
				delete []pWeightArray;
				pWeightArray=NULL;
			}
				return m_pResult;
		}
		break;
		
	case 4:
		{
			m_pResult=new CResult("多个相关样本检验");
			int nRow=pDataInterface->GetRowCount();
			int index[] = {0,1,2};
			int nCol=m_tVarInfo3.iCount;	
			if(nCol<=2)
			{
				CTString str="变量数小于3个，不能进行多个相关样本检验!";
				if(m_p1RsltElementText==NULL)
					m_p1RsltElementText= new CRsltElementText("警告");
				m_p1RsltElementText->AddString(str);
				m_pResult->Add(m_p1RsltElementText);
				return m_pResult;
			}
			CTArray<double ,double> Weight;//权重
			CDoubleMatrix mat=GetMultiRelatedData(Weight,pWeightArray);
			int size=mat.mrow();
			if(size<2)
			{
				if(size==0||Weight.GetAt(0)<2)
				{
					CTString str="有效记录数小于2个，不能进行多个相关样本检验!";
					if(m_p1RsltElementText==NULL)
						m_p1RsltElementText= new CRsltElementText("警告");
					m_p1RsltElementText->AddString(str);
					m_pResult->Add(m_p1RsltElementText);
					return m_pResult;
				}

			}

			if(m_bFriedman)
			{		
				m_FieldNameStrAr.RemoveAll();
				m_pRsltVariable =new CRsltVariable("弗里德曼检验");
				m_pResult->Add(m_pRsltVariable);
				FriedmansTwowayAnalysisofVariancebyRank(mat,Weight);
				if(m_p1RsltElementText)
					m_pRsltVariable->Add(m_p1RsltElementText);
			}
			if(m_bKendall)
			{
				m_FieldNameStrAr.RemoveAll();
				m_pRsltVariable =new CRsltVariable("肯德尔 W 检验");
				m_pResult->Add(m_pRsltVariable);
				KendallsCoefficientofConcordence(mat,Weight);
				if(m_p2RsltElementText)
					m_pRsltVariable->Add(m_p2RsltElementText);
			}
			if(m_bCochran)
			{
				m_FieldNameStrAr.RemoveAll();
				m_pRsltVariable =new CRsltVariable("科克伦 Q 检验");
				m_pResult->Add(m_pRsltVariable);
				CochransQTest(mat,Weight);
				if(m_p3RsltElementText)
					m_pRsltVariable->Add(m_p3RsltElementText);
			}
			if(pWeightArray)
			{
				delete []pWeightArray;
				pWeightArray=NULL;
			}
			return m_pResult;
		}
		break;
	}
	return NULL;
}	


//*************************************************************************//
//Original author:ZKC													   //
//Date: 2003.3.6														   //
//Function : 成员变量内存空间的释放										   //
//*************************************************************************//

void CNonParamTest::FreeAllMemember()
{
	m_FieldNameStrAr.RemoveAll();
	m_ClassStrAr.RemoveAll();
	m_NStrAr.RemoveAll();
	m_MeanRandStrAr.RemoveAll();
	m_SumRandStrAr.RemoveAll();
	m_ZStrAr.RemoveAll();
	m_PStrAr.RemoveAll();
	m_MeanWhiteyAr.RemoveAll();
	m_WilcoxonAr.RemoveAll();
	m_ZAr.RemoveAll();
	m_PAr.RemoveAll();
	m_NAr.RemoveAll();
	m_S1Ar.RemoveAll();
	m_P1Ar.RemoveAll();
	m_S2Ar.RemoveAll();
	m_P2Ar.RemoveAll();
	m_hAr.RemoveAll();
	m_chiStrArray.RemoveAll();
	m_RowArray.RemoveAll();

	for(int i=0;i<m_StrPtr.GetSize();i++)
	{
		CPtrArray *p=(CPtrArray *)(m_StrPtr.GetAt(i));
		for(int j=0;j<p->GetSize();j++)
		{
			delete p->GetAt(j);
		}
		p->RemoveAll();
		delete p;
	}
	m_StrPtr.RemoveAll();


}


//*************************************************************************//
//Original author:ZKC													   //
//Date: 2003.3.6														   //
//Function : 两个相关样本内存释放										   //
//*************************************************************************//

void CNonParamTest::FreeTwoRelated()
{
	for(int i=0;i<m_PtrArrayIndex.GetSize();i++)
	{
		delete (tagSturctComboIndex*)m_PtrArrayIndex.GetAt(i);
	}
	m_PtrArrayIndex.RemoveAll();
					
}

//*************************************************************************//
//Original author:ZKC													   //
//Date: 2003.3.6														   //
//Function : 多个独立样本分析的内存释放									   //
//*************************************************************************//

void CNonParamTest::DelDataPtrArray(CPtrArray &DataPtr)
{
	int nSize=DataPtr.GetSize();
	for(int i=0;i<nSize;i++)
	{
		CPtrArray *pSub=(CPtrArray *)(DataPtr.GetAt(i));
		int nSubSize=pSub->GetSize();
		for(int j=0;j<nSubSize;j++)
		{
			TManyGroup *pGroup=(TManyGroup *)(pSub->GetAt(j));
			delete pGroup;
		}
		pSub->RemoveAll();
		delete pSub;
	}
	DataPtr.RemoveAll();

}

//*************************************************************************//
//Original author:ZKC													   //
//Date: 2003.3.6														   //
//Function : 得到两个相关样本检验中排除了空值与缺失值合法数据，处理方法是  //
//			 只要数据或权重中有一个有空值或缺失值，则把整行删掉（或排除在外//
//*************************************************************************//
void CNonParamTest::GetTwoRelatedData(CTArray<double,double> &Data1,
									  CTArray<double,double> &Data2,
									  CTArray<double,double> &Weight,
									  TDataSet *pWeightArray,
									  int nCol1,int nCol2)
{
	TDataSet *pData1Set=NULL;
	TDataSet *pData2Set=NULL;
	int nRow=m_pDataInterface->GetColData(nCol1,pData1Set,5);
	m_pDataInterface->GetColData(nCol2,pData2Set,5);
	for(int j=0;j<nRow;j++)
	{		
		if(m_bWeight)//假如指定了权重
		{

			if(pData1Set[j].iIndex>=0&&pData2Set[j].iIndex>=0&&pWeightArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//权重与数据列均未缺失
			{
				Weight.Add(int(pWeightArray[j].dValue+0.5));
				Data1.Add(pData1Set[j].dValue);
				Data2.Add(pData2Set[j].dValue);
			}
			else//否则记录被忽略的行
			{
				m_RowArray.Add(pData1Set[j].iIndex);
			}
		}
		else//假如未指定权中
		{			
			if(pData1Set[j].iIndex>=0&&pData2Set[j].iIndex>=0)//权重与数据列均未缺失
			{
				Weight.Add(1);
				Data1.Add(pData1Set[j].dValue);
				Data2.Add(pData2Set[j].dValue);
			}
			else//否则记录被忽略的行
			{
				m_RowArray.Add(pData1Set[j].iIndex);
			}
		}
	}
	if(pData1Set)
	{
		delete []pData1Set;
		pData1Set=NULL;
	}
	if(pData2Set)
	{
		delete []pData2Set;
		pData2Set=NULL;
	}
}



//*************************************************************************//
//Original author:ZKC													   //
//Date: 2003.3.6														   //
//Function : 得到多个相关样本检验中排除了空值与缺失值合法数据，处理方法是  //
//			 只要数据或权重中有一个有空值或缺失值，则把整行删掉（或排除在外//
//*************************************************************************//
CDoubleMatrix CNonParamTest::GetMultiRelatedData(CTArray<double,double> &Weight,
												 TDataSet *pWeightArray)
{
	CPtrArray DataPtr;
	CPtrArray LegalDataPtr;
	int nRow=0;
	int i = 0;
	int j = 0;
	for(i=0;i<m_tVarInfo3.iCount;i++)
	{
		TDataSet *pDataSet=NULL;
		int nCol=m_tVarInfo3.pValue[i];
		nRow=m_pDataInterface->GetColData(nCol,pDataSet,5);
		CPtrArray *subPtr=new CPtrArray;
		DataPtr.Add((void *)pDataSet);
		LegalDataPtr.Add((void *)subPtr);
	}
	for(j=0;j<nRow;j++)
	{		
		if(m_bWeight)//假如指定了权重
		{

			if(pWeightArray[j].iIndex>=0 && pWeightArray[j].dValue>0 && IsDataLegal(DataPtr,nRow,j))//权重与数据列均未缺失
			{
				for(int n=0;n<m_tVarInfo3.iCount;n++)
				{
					TDataSet *pData=(TDataSet *)(DataPtr.GetAt(n));
					CPtrArray *pPtr=(CPtrArray *)(LegalDataPtr.GetAt(n));
					double *pDouble=new double(pData[j].dValue);
					pPtr->Add((void *)pDouble);
				}
				Weight.Add(int(pWeightArray[j].dValue+0.5));
			}
			else//否则记录被忽略的行
			{
				m_RowArray.Add(pWeightArray[j].iIndex);
			}
		}
		else//假如未指定权中
		{			
			if(IsDataLegal(DataPtr,nRow,j))//权重与数据列均未缺失
			{
				for(int n=0;n<m_tVarInfo3.iCount;n++)
				{
					TDataSet *pData=(TDataSet *)(DataPtr.GetAt(n));
					CPtrArray *pPtr=(CPtrArray *)(LegalDataPtr.GetAt(n));
					double *pDouble=new double(pData[j].dValue);
					pPtr->Add((void *)pDouble);
				}
				Weight.Add(1);
			}
			else//否则记录被忽略的行
			{
				m_RowArray.Add(j+1);
			}
		}
	}
	int nCol=m_tVarInfo3.iCount;
	nRow=Weight.GetSize();
	CDoubleMatrix matrix(nRow,nCol);
	//get the legal data
	for(j=0;j<nCol;j++)
	{
		CPtrArray *pSub=(CPtrArray *)(LegalDataPtr.GetAt(j));
		for(int i=0;i<nRow;i++)
		{
			matrix(i,j)=*((double *)(pSub->GetAt(i)));
		}
	}
    
	//free the memory
	for(i=0;i<DataPtr.GetSize();i++)
	{
		TDataSet *p=(TDataSet *)(DataPtr.GetAt(i));
		delete []p;
		CPtrArray *p2=(CPtrArray *)(LegalDataPtr.GetAt(i));
		for(int j=0;j<p2->GetSize();j++)
		{
			double *pdouble=(double *)(p2->GetAt(j));
			delete pdouble;
		}
		p2->RemoveAll();
		delete p2;

	}
	DataPtr.RemoveAll();
	LegalDataPtr.RemoveAll();
	return matrix;
}


//***********************************************************************//
//Original author:ZKC													 //
//Date: 2003.3.6														 //
//Function : 判断第j行是否没有空值与缺失值，没有，返回true，否则，false  //
//***********************************************************************//
bool CNonParamTest::IsDataLegal(CPtrArray &DataPtr,		//数据矩阵
								int nRow,				//数据的行数=DataPtr.GetSize()
								int j)					//第j行
{
	if(j>=nRow)											//数据越界
		return false;	
	for(int i=0;i<DataPtr.GetSize();i++)
	{
		TDataSet *pDataSet=(TDataSet *)(DataPtr.GetAt(i));
		if(pDataSet[j].iIndex<0)						//有空值或缺失值
			return false;
	}
	return true;
}


//***********************************************************************//
//Original author:ZKC													 //
//Date: 2003.3.6														 //
//Function : 把两个样本混合后按升序排列,得出最大可能游程及最小可能游程； //
//Attention :合适的算法是最关键。										 //
//***********************************************************************//

void CNonParamTest::GetMaxAndMinRuns(CTArray<double,double> &data1Array, //组1数据
									 CTArray<double,double> &data2Array, //组2数据
									 CTArray<double,double> &Weight1,	//组1数据对应的权重
									 CTArray<double,double> &Weight2,	//组2数据对应的权重
									 double &nMax,double &nMin)			//得到最大可能游程数与最小可能游程数
{
	//to combine the data of the two groups as well as the weight,
	//then order the data ascendedly,and the weight will be ordered accordingly.
	CTArray<TRunNums *,TRunNums *> dataArray;
	int i=0;
	for(i=0;i<data1Array.GetSize();i++)
	{
		TRunNums *p=new TRunNums;
		p->dValue=data1Array[i];
		p->nGroup=1;
		p->nWeight=Weight1[i];
		dataArray.Add(p);
	}
	for(i=0;i<data2Array.GetSize();i++)
	{
		TRunNums *p=new TRunNums;
		p->dValue=data2Array[i];
		p->nGroup=2;
		p->nWeight=Weight2[i];
		dataArray.Add(p);
	}
	OrderTheGroup(dataArray);
	TRunNums *p=NULL;

	//to calculate the value frequency in Group1 as well as in Group2 and store 
	//them in Group1Array and Group2Array separately.
	CTArray<double,double> Group1Array,Group2Array;
	int count1=0,count2=0;
	double temp=dataArray[0]->dValue;
	if(dataArray[0]->nGroup==1)
		count1=count1+dataArray[0]->nWeight;
	else
		count2=count2+dataArray[0]->nWeight;
	for(i=1;i<dataArray.GetSize();i++)
	{
		if(dataArray[i]->dValue==dataArray[i-1]->dValue)
		{
			if(dataArray[i]->nGroup==1)
				count1=count1+dataArray[i]->nWeight;
			else
				count2=count2+dataArray[i]->nWeight;
		}
		else
		{
			Group1Array.Add(count1);
			Group2Array.Add(count2);
			count1=0;count2=0;
			if(dataArray[i]->nGroup==1)
				count1=count1+dataArray[i]->nWeight;
			else
				count2=count2+dataArray[i]->nWeight;
		}
	}
	Group1Array.Add(count1);
	Group2Array.Add(count2);

	
	///free the memory
	for(i=0;i<dataArray.GetSize();i++)
	{
		delete dataArray[i];
	}
	dataArray.RemoveAll();
	CTArray<int,int> RunArray;
	for(i=0;i<Group1Array.GetSize();i++)
	{
		if(Group1Array[i]>Group2Array[i])
		{
			RunArray.Add(1);
			nMax=nMax+Group2Array[i]*2;
		}
		else if(Group1Array[i]==Group2Array[i])
		{
			nMax=nMax+Group1Array[i]*2;
		}
		else
		{
			RunArray.Add(2);
			nMax=nMax+Group1Array[i]*2;
		}
	}
	if(RunArray.GetSize()>0)
		nMax++;
	for(i=1;i<RunArray.GetSize();i++)
	{
		if(RunArray[i]!=RunArray[i-1])
			nMax++;
	}

	//to calculate the minimum number of runs.
	int nMin1=0,nMin2=0;
	int nLast1=0,nLast2=0;
	if(Group1Array[0]>0)
	{
		nMin1++;
		nLast1=1;
	}
	if(Group2Array[0]>0)
	{
		nMin1++;
		nLast1=2;
	}

	if(Group2Array[0]>0)
	{
		nMin2++;
		nLast2=2;
	}
	if(Group1Array[0]>0)
	{
		nMin2++;
		nLast2=1;
	}
	for(i=1;i<Group1Array.GetSize();i++)
	{
		if(nLast1==1)
		{
			if(Group2Array[i]>0)
			{
				nMin1++;
				nLast1=2;
			}
			else
				nLast1=1;
		}
		else
		{
			if(Group1Array[i]>0)
			{
				nMin1++;
				nLast1=1;
			}
			else
				nLast1=2;
		}
		if(nLast2==1)
		{
			if(Group2Array[i]>0)
			{
				nMin2++;
				nLast2=2;
			}
			else
				nLast2=1;
		}
		else
		{
			if(Group1Array[i]>0)
			{
				nMin2++;
				nLast2=1;
			}
			else
				nLast2=2;
		}
	}
	nMin=nMin1<nMin2?nMin1:nMin2;
}


//***********************************************************************//
//Original author:ZKC													 //
//Date: 2003.3.6														 //
//Function : 对数据进行shell排序，其对应的权重、分组号要做相应的调整     //
//Attention :因为权重因素的考虑，要用double型数据来计数，以免数据越界；	 //
//***********************************************************************//

void CNonParamTest::OrderTheGroup(CTArray<TRunNums *,TRunNums *>&DataArray)
{
    int k=0,j=0,i=0;
	int n=DataArray.GetSize();
	k=n/2;
	while (k>0)
	{ 
		for (j=k; j<=n-1; j++)
		{
			TRunNums *pData=DataArray[j];
			double dValue=pData->dValue;
			int nGroup=pData->nGroup;
			double nWeight=pData->nWeight;
			i=j-k;
			while ((i>=0)&&(DataArray[i]->dValue>dValue))
			{
				DataArray[i+k]->dValue=DataArray[i]->dValue;//值，排序的标准
				DataArray[i+k]->nGroup=DataArray[i]->nGroup;//分组号
				DataArray[i+k]->nWeight=DataArray[i]->nWeight;//权重
				i=i-k;
			}
			DataArray[i+k]->dValue=dValue;
			DataArray[i+k]->nGroup=nGroup;
			DataArray[i+k]->nWeight=nWeight;
		}
		k=k/2;
	}
}


//***********************************************************************//
//Original author:ZKC													 //
//Date: 2003.3.6														 //
//Function : 对数据进行shell排序，其对应的权重要做相应的调整		     //
//Attention :因为权重因素的考虑，要用double型数据来计数，以免数据越界；	 //
//***********************************************************************//


void CNonParamTest::OrderTheGroup(CTArray<double,double>&DataArray,	//数据值，排序的标准
								  CTArray<double,double>&WeightArray)//权重
{
    int k=0,j=0,i=0;
	int n=DataArray.GetSize();
    double tData=0,tWeight=0;
	k=n/2;
	while (k>0)
	{ 
		for (j=k; j<=n-1; j++)
		{
			tData=DataArray[j];
			tWeight=WeightArray[j];
			i=j-k;
			while ((i>=0)&&(DataArray[i]>tData))
			{
				DataArray[i+k]=DataArray[i];
				WeightArray[i+k]=WeightArray[i];
				i=i-k;
			}
			DataArray[i+k]=tData;
			WeightArray[i+k]=tWeight;
		}
		k=k/2;
	}
}

//***********************************************************************//
//Original author:ZKC													 //
//Date: 2003.3.6														 //
//Function : 求经验分布函数的值；									     //
//Attention :因为权重因素的考虑，要用double型数据来计数，以免数据越界；	 //
//***********************************************************************//

double CNonParamTest::CalkCommonFx(CTArray<double,double> &DataArray,
							 CTArray<double,double> &WeightArray,
							 double x,double nSize)
{
	int nLen=DataArray.GetSize();
	if(x<DataArray.GetAt(0))
		return 0;
	else if(x>DataArray.GetAt(nLen-1))
		return 1;
	else 
	{
		double ni=0;
		for(int i=0;i<nLen-1;i++)
		{
			ni=ni+WeightArray.GetAt(i);			
			if(x>=DataArray.GetAt(i)&&x<DataArray.GetAt(i+1))
				return double(ni)/nSize;
		}
		ni=ni+WeightArray.GetAt(nLen-1);
		return 1.0*ni/nSize;
	}
	return 1;
}