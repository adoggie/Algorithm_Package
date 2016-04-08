// Discriminant.cpp: implementation of the CDiscriminant class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
// #include "tlAdvn.h"
#include "Discriminant.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiscriminant::CDiscriminant()
{
//  yzmModify����ĳ�Ա����Ӧ���������ʼ��
//	m_pData = NULL;

	m_dEntry		= 3.84;
	m_dRemoval		= 2.71;
	m_nCoefficient	= 0;
	m_nMethod		= 0;	

	m_nDependIndex = -1;

	m_bC1 = false;
	m_bC2 = false;
	m_bT1  = false;
	m_bR  = false;
	m_bIn1  = false;
	m_bFL  = false;
}

CDiscriminant::~CDiscriminant()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//	���ܣ����б�
//  ���룺X,w,t,F,LAMBDA
//	�����
//	�ο��飺Ф���㣬������ͳ�Ƽ��㷽������1994�꣬�Ͽ���ѧ�����磬p258-266
//           ����ǵȣ������ݷ�������2002�꣬��ѧ������
//	������
//  X���������n������������p�Ǳ���������X(1)�Ƿ��������X(p)��Ȩ�ر���
//  g�������
//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CDiscriminant::StepWiseDiscriminant(CDoubleMatrix matrix_X,int allORstepwise,int distanceORfisher, double Entry, double Removal)
{
	//���ݵ�һ������
	int nRow = matrix_X.mrow();
	m_original_mx = matrix_X.copy();
	if(nRow<1)
	{
		CRsltElementText *pErrorText=new CRsltElementText("����");
		CTString strError="����Ч���ݣ�����û�б�ִ��!";
		pErrorText->AddString(strError);
		m_pResult->Add(pErrorText);
 	    return false;
	}
	int i=0;
    for(i=0;i<nRow;i++)
		matrix_X(i,0)=(int)matrix_X(i,0);
	SortMatrix(matrix_X);	
	//��Ҫ�Ե�һ�����ݴ���	

	nRow = matrix_X.mrow();
	int nCol = matrix_X.mcol();
	int p = nCol - 2;
	//���������
	CDoubleVector vect = matrix_X.col(0);	
	int g = GetGroup(vect);//���ٸ���ͬ�Ĺ۲�ֵ
    m_group = g;
	if(g==1)
	{
		CRsltElementText *pErrorText=new CRsltElementText("���ຯ��ϵ��ֵ");
		CTString strError="ֻ��һ���ǿ��飬��������������û�б�ִ��!";
		pErrorText->AddString(strError);
		m_pResult->Add(pErrorText);
 	    return false;
	}

	CDoubleVector vector_dnum = GetGroupValue(vect);
	m_class_Vec = vector_dnum.copy();
	//���������ά���飨ȥ����һ�У��� vector_m(i)�ǵ�i��������������

	CPtrArray arX3D;

	CIntVector vector_m(g);

	for(i=0;i<nRow;i++)
	{

		for(int j=0;j<g;j++)
		{
			vector_m[j] = 0;	
			if( fabs( matrix_X(i,0) - vector_dnum[j]) < 1e-6 )
			{
				vector_m[j] += 1;				
				break;
			}
		}
	}
    
	int j=0;
	for(j=0;j<g;j++)
	{
		vector_m[j] = 0;
		for(i=0;i<nRow;i++)
			if(vector_dnum[j]==matrix_X(i,0))
			   vector_m[j] += 1;
	}
	m_num_Vec = vector_m.copy();
    int row1=0, col1=1;
	for(i=0;i<g;i++)
	{
		CDoubleMatrix * pMatrix = new CDoubleMatrix(vector_m[i],nCol-2);
		CDoubleMatrix * pMatrix1 = new CDoubleMatrix(vector_m[i],nCol-1);
		*pMatrix = slice(matrix_X,row1,1,vector_m[i],nCol-2);
		//*pMatrix1 = slice(matrix_X,row1,1,vector_m[i],nCol-1);
		row1 += vector_m[i];
		arX3D.Add(pMatrix);
	}
	int Max=0,Number = 0;
	Max = m_num_Vec.Max();    
	CDoubleMatrix fWeight(g,Max);
	for(i=0;i<g;i++)
	{
		int j=Number;
		for(j=Number;j<Number+vector_m[i];j++)
		{
			fWeight(i,j-Number) = matrix_X(j,p+1); 			 
		}
		Number = j;
	}
	
	//�����j��ĵ�i�������ļ�Ȩƽ��ֵ:(p*g)
	double nSumForStat(0); //Ϊ���c1
	CDoubleMatrix matrix_XGrpVrbMean(p,g);
	CDoubleVector vector_n(g);
    double nSum = 0.0;

	for(j=0;j<g;j++)
	{
		for(i=0;i<p;i++)
		{
			double sumTotal = 0.0;
            vector_n[j] = 0.0;
			int k=0;
			for(k=0;k<vector_m[j];k++)
			{
				CDoubleMatrix * pMatrix = (CDoubleMatrix *)arX3D.GetAt(j);
				double a = (*pMatrix)(k,i);
				double b = fWeight(j,k);
				sumTotal += (*pMatrix)(k,i) * fWeight(j,k);
				vector_n[j] += fWeight(j,k);
			}
			matrix_XGrpVrbMean(i,j) = sumTotal / vector_n[j];
			double bbb=vector_n[j];
			double ccc=sumTotal;
			double aaa=matrix_XGrpVrbMean(i,j);
			nSumForStat += sumTotal;
		}
        nSum += vector_n[j];
	}
	//�������i�ļ�Ȩƽ��ֵ 

	CDoubleVector vector_XVrbMean(p);
	for(i=0;i<p;i++)
	{
		double sumTotal=0.0;
		for(j=0;j<g;j++)
		{
			int k=0;
			for(k=0;k<vector_m(j);k++)
			{
				CDoubleMatrix * pMatrix = (CDoubleMatrix *)arX3D.GetAt(j);
				sumTotal += (*pMatrix)(k,i) * fWeight(j,k);
			}
		}
		vector_XVrbMean[i] = sumTotal / nSum;
	}

    CDoubleMatrix matrix_SVrbGrpStdDev(p,g);
	
	for(i = 0;i < p;i++)
	{
		for(j = 0;j < g;j++)
		{
			matrix_SVrbGrpStdDev(i,j) = 0.0;

			int k = 0;
			for(k = 0;k < vector_m[j];k++)
			{
				CDoubleMatrix * pMatrix = (CDoubleMatrix *)arX3D.GetAt(j);
				matrix_SVrbGrpStdDev(i,j) += fWeight(j,k) * (*pMatrix)(k,i) * (*pMatrix)(k,i);
			}

            matrix_SVrbGrpStdDev(i,j) -= vector_n[j] * matrix_XGrpVrbMean(i,j) * matrix_XGrpVrbMean(i,j);
			matrix_SVrbGrpStdDev(i,j) /= vector_n[j] - 1;
            matrix_SVrbGrpStdDev(i,j) = sqrt(matrix_SVrbGrpStdDev(i,j));
		}        
	}
    CDoubleVector vector_SVrbStdDev(p);
	
	for(i = 0;i < p;i++)
	{
		vector_SVrbStdDev[i] = 0.0;

		for(j = 0;j < g;j++)
		{
			int k = 0;
			for(k = 0;k < vector_m[j];k++)
			{
				CDoubleMatrix * pMatrix = (CDoubleMatrix *)arX3D.GetAt(j);
				vector_SVrbStdDev[i] += fWeight(j,k) * (*pMatrix)(k,i) * (*pMatrix)(k,i);
			}
		}

        vector_SVrbStdDev[i] -= nSum * vector_XVrbMean[i] * vector_XVrbMean[i];  
		vector_SVrbStdDev[i] /= nSum - 1.0;
        vector_SVrbStdDev[i] = sqrt(vector_SVrbStdDev[i]);        
	}

    CDoubleMatrix matrix_w(p,p);
    CDoubleMatrix matrix_t(p,p);

	for(i = 0;i < p;i++)
	{
		int l = 0;
		for(l = 0;l < p;l++)
		{
			matrix_w(i,l) = 0;
			matrix_t(i,l) = 0;

			int j = 0;
			for(j = 0;j < g;j++)
			{
				for(int k = 0;k < vector_m[j];k++)
				{
					CDoubleMatrix * pMatrix = (CDoubleMatrix *)arX3D.GetAt(j);
				    matrix_w(i,l) += fWeight(j,k) * (*pMatrix)(k,i) * (*pMatrix)(k,l);  
					matrix_t(i,l) += fWeight(j,k) * (*pMatrix)(k,i) * (*pMatrix)(k,l);
				}
			}

			for(j = 0;j < g;j++)
			{
				double  sumTemp1 = 0.0;
                double  sumTemp2 = 0.0;

				for(int k = 0;k < vector_m[j];k++)
				{
					CDoubleMatrix * pMatrix = (CDoubleMatrix *)arX3D.GetAt(j);
				    sumTemp1 += fWeight(j,k) * (*pMatrix)(k,i);
					sumTemp2 += fWeight(j,k) * (*pMatrix)(k,l);
				}
                
				matrix_w(i,l) -= (sumTemp1 * sumTemp2) / vector_n[j];
			}

			double  sumTemp11 = 0.0;
            double  sumTemp22 = 0.0;

			for(j = 0;j < g;j++)
			{
				for(int k = 0;k < vector_m[j];k++)
				{
					CDoubleMatrix * pMatrix = (CDoubleMatrix *)arX3D.GetAt(j);
				    sumTemp11 += fWeight(j,k) * (*pMatrix)(k,i);
					sumTemp22 += fWeight(j,k) * (*pMatrix)(k,l);
				}
			}
			             
			matrix_t(i,l) -= (sumTemp11 * sumTemp22) / nSum;	
		}
	}
	CDoubleMatrix matrix_C1(p,p);
    CDoubleMatrix matrix_TC(p,p);

	for(i = 0;i < p;i++)
	{
		for(int j = 0;j < p;j++)
		{
			matrix_C1(i,j) = matrix_w(i,j) / (nSum - g);
			matrix_TC(i,j) = matrix_t(i,j) / (nSum - 1);
		}
	}

	CDoubleMatrix matrix_R(p,p);

    for(i = 0;i < p;i++)
		for(int l = 0;l < p;l++)
			if(matrix_w(i,i) * matrix_w(l,l) > 0)
				matrix_R(i,l) = matrix_w(i,l) / sqrt(matrix_w(i,i) * matrix_w(l,l));
			else
				matrix_R(i,l) = 1;
    
	CPtrArray matrix_C2;
    CDoubleMatrix * pMatrix = new CDoubleMatrix(p,p);
    CTString str;

	for(j = 0;j < g;j++)
	{
		CDoubleMatrix * pMatrix1 = (CDoubleMatrix *)arX3D.GetAt(j);
		int i = 0;
		for(i = 0;i < p;i++)
		{
			for(int l = 0;l < p;l++)
			{
			    (*pMatrix)(i,l) = 0.0;
               	for(int k = 0;k < vector_m[j];k++)
					(*pMatrix)(i,l) += fWeight(j,k) * (*pMatrix1)(k,i) * (*pMatrix1)(k,l);
				    
				(*pMatrix)(i,l) -= matrix_XGrpVrbMean(i,j) * matrix_XGrpVrbMean(l,j) * vector_n[j];
				(*pMatrix)(i,l) /= vector_n[j] - 1;
			}
		}
		matrix_C2.Add(pMatrix);

		if(m_bC2)
		{
			if(vector_m[j] > 1)
			{
				int tt=vector_dnum[j];
				str.Format("����Э����%d",tt); //j+1
 			    CreateResultC2(pMatrix, (char *)str);
			    str.RemoveAll();
			}
		}		
	}
		
    if(m_bT1)
			CreateResultT1(matrix_TC);

	if(m_bC1 && ((nSum - g) != 0))
			CreateResultC1(matrix_C1);

    if(m_bR)
			CreateResultR(matrix_R);
    
	//select ����
	CIntVector vector_In(p);
	if(p >= 1)   //2
	{
		p = matrix_w.mrow();
		CDoubleVector vector_LAMBDA(p);
        CDoubleVector vector_F(p);
        int vrb(0),vrb0=0,id=0,flag_T=0;
		CTString str1;
		double temp=matrix_w(0,0) / matrix_t(0,0);
		
		for(int l=0;l<p;l++)
		{
			if(matrix_t(l,l)==0)
				flag_T=1;

			vector_LAMBDA[l]=matrix_w(l,l) / matrix_t(l,l);
			double aa=vector_LAMBDA[l];
            vector_F[l] = ( matrix_t(l,l) - matrix_w(l,l) ) * (nSum - g) / (matrix_w(l,l) * ( g - 1));
		    if(vector_LAMBDA[l]<temp)
			{
				temp=vector_LAMBDA[l];
		        id=l;
			}
		}

		if(m_bFL && (flag_T==0))
			CreateResultF(vector_F,vector_LAMBDA);

		if(m_bIn1)
		{
			str1.Format("�������������%d",0);
			CreateResultIn1(vector_F,vector_LAMBDA,vector_In,vrb,str1);
			str1.RemoveAll();
		}
			    
        if(vector_F[id] > Entry)  // 3.84��Ԥ��ֵ
		{
			vrb=vrb+1;
			vector_In[vrb-1]=id;

			for(int i=0;i < p;i++)
			if(!In(matrix_w,matrix_t,nSum,g,i,vector_In,Entry,vector_LAMBDA,vrb,id,p,allORstepwise,Removal))
				break;   		
		}
	

		if(vrb==0)
		{
			CRsltElementText *pErrorText=new CRsltElementText("���ຯ��ϵ��ֵ");
		    CTString strError="û�б����ʺϷ���, δ�ó����ຯ��ϵ��ֵ!";
		    pErrorText->AddString(strError);
		    m_pResult->Add(pErrorText);
	
 	        return false;
		}

	    //ѡ������Ҫ���б����
	    //������ʼ��㷽����1���������������ȣ�2�����������������������
	    int priorP = 1;
	    CDoubleVector vector_prob(g);
	    switch(priorP)
		{
			case 1:
				for(i=0;i<g;i++)
				{
					vector_prob[i] = 1.0/g;
				}
				break;
	        case 2:
				for(i=0;i<g;i++)
				{
					vector_prob[i] = vector_m[i] / (nRow-0.0);
				}
				break;
		}

	    CDoubleMatrix matrix_a(p,g);
	    CDoubleVector vector_b(g);
	    if( !Coefficient(vector_prob,vector_In,nSum,vector_m,matrix_w,matrix_XGrpVrbMean,&arX3D,matrix_a,vector_b,distanceORfisher,vrb) )
		{
			CRsltElementText *pErrorText=new CRsltElementText("����");
		    CTString strError="����û�б�ִ�У�����������ѡ��!";
		    pErrorText->AddString(strError);
		    m_pResult->Add(pErrorText);
		    return false;
		}
		CreateResultCO(matrix_a,vector_b,vrb,vector_dnum);
		WriteData(m_finalClassData,"zzzz");
	
	}
		Classify(m_finalClassData);
		CreateResultdiscri(m_discri_mx);
		CreateResultdiscripercent(m_discripercent_mx);
		if (m_bDisResult)
		{
			 CreateResult();
		}
		return true;
}
  

//////////////////////////////////////////////////////////////////////////
//
//							
//
//////////////////////////////////////////////////////////////////////////
bool CDiscriminant::Sweep(CDoubleMatrix matrix_tempw, CDoubleMatrix matrix_tempt, int r,CDoubleMatrix &matrix_w1,CDoubleMatrix &matrix_t1)
{
	CDoubleMatrix matrix_w = copy(matrix_tempw);
	CDoubleMatrix matrix_t = copy(matrix_tempt);
	int p = matrix_w.mcol();
	if(IsZero(matrix_w(r,r)) || IsZero(matrix_t(r,r)))
		return false;

	int i=0;
	for(i=0;i<p;i++)
	{
		int j=0;
		for(j=0;j<p;j++)
		{
			if( i==r && j!=r )
			{
				matrix_w1(i,j) = matrix_w(r,j)/matrix_w(r,r);
				matrix_t1(i,j) = matrix_t(r,j)/matrix_t(r,r);
			}
			if( i!=r && j!=r )
			{
				matrix_w1(i,j) = matrix_w(i,j) - matrix_w(i,r) * matrix_w(r,j) / matrix_w(r,r);
				matrix_t1(i,j) = matrix_t(i,j) - matrix_t(i,r) * matrix_t(r,j) / matrix_t(r,r);
			}
			if( i==r && j==r )
			{
				matrix_w1(i,j) = 1/matrix_w(r,r);
				matrix_t1(i,j) = 1/matrix_t(r,r);
			}
			if( i!=r && j==r )
			{
				matrix_w1(i,j) = -matrix_w(i,r)/matrix_w(r,r);
				matrix_t1(i,j) = -matrix_t(i,r)/matrix_t(r,r);
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//										
//
//////////////////////////////////////////////////////////////////////////////////////////////////
bool CDiscriminant::In(CDoubleMatrix &matrix_w, CDoubleMatrix &matrix_t, double nSum, int g, int i, CIntVector &vector_In, double Entry, CDoubleVector &vector_LAMBDA, int &vrb, int &id, int p, int allORstepwise, double Removal)
{
	double temp=vector_LAMBDA[id];
	CDoubleVector vector_F(p);
	if(!Sweep(matrix_w,matrix_t,vector_In[vrb-1],matrix_w,matrix_t))
		return false;
    int Fid=0;
	CTString str;
	double FMin=Removal,FRemove=0.0;
	for(i=0;i<vrb;i++)
    {
		FRemove=(matrix_w(vector_In[i],vector_In[i])-matrix_t(vector_In[i],vector_In[i]))*(nSum-g-vrb+1)/(matrix_t(vector_In[i],vector_In[i])*(g-1));
        if(FRemove<FMin)
		{
			FMin=FRemove;
            Fid=i;
		}
    }

    int s=0;
	double Min=temp*(matrix_w(0,0)/matrix_t(0,0));
    vector_LAMBDA[0]=Min;
	vector_F[0]=(matrix_t(0,0)-matrix_w(0,0))*(nSum-g-i)/(matrix_w(0,0)*(g-1));
	id=0;
	int l=1;
	for(l=1;l<p;l++)
    {
		vector_LAMBDA[l]=temp*(matrix_w(l,l)/matrix_t(l,l));
		vector_F[l]=(matrix_t(l,l)-matrix_w(l,l))*(nSum-g-i)/(matrix_w(l,l)*(g-1));
		double a=vector_LAMBDA[l];
        if(vector_LAMBDA[l]<Min)
        {
			s=0;
            for(int j=0;j<vrb;j++)
				if(l==vector_In[j])
					s=1;
			if(s==0)
			{
				id=l;
                Min=vector_LAMBDA(l);
			}
		}
	}

	double a=vector_F[id];

    if(vector_F[id]>Entry)  // 3.84��Ԥ��ֵ
	{
		if(m_bIn1)
		{
			str.Format("�������������%d",i);
		    CreateResultIn1(vector_F,vector_LAMBDA,vector_In,vrb, str);
		    str.RemoveAll();
		}
		vrb++;
		vector_In(vrb-1)=id;
		
        return true;
	}
    return false;
}
 
////////////////////////////////////////////////////////////////
//
//				����ϵ��
//				way-1:�����б�;2:fisher,
//				a:����ϵ����b������ϵ��
//
////////////////////////////////////////////////////////////////
bool CDiscriminant::Coefficient(CDoubleVector vector_prob,CIntVector vector_In,double nSum,CIntVector vector_m,CDoubleMatrix matrix_w,CDoubleMatrix matrix_XGrpVrbMean,CPtrArray * parX3D,CDoubleMatrix &matrix_a,CDoubleVector &vector_b,int way,int vrb)
{
	int p = matrix_XGrpVrbMean.mrow();
	int g = matrix_XGrpVrbMean.mcol();
	int i,j;
	CPtrArray arS;
	switch(way)
	{
	case 1:
		{
		//�þ����б�ȷ��ϵ��
		for(j=0;j<g;j++)
		{
			CDoubleMatrix * pMatrix = new CDoubleMatrix(p,p);
			arS.Add(pMatrix);
			for(int i=0;i<vector_m[j];i++)
			{
				CDoubleMatrix * pt = (CDoubleMatrix *)parX3D->GetAt(j);
				CDoubleVector vector_temp = pt->row(i);
				CDoubleMatrix matrix_temp(1,p);
				matrix_temp = vector_temp - matrix_XGrpVrbMean.col(j);
				(*pMatrix) =  (*pMatrix) + trans( matrix_temp ) * matrix_temp ; 
			}
			(*pMatrix) = (*pMatrix) / (vector_m[j] - 1.0);
		}
		for(j=0;j<g;j++)
		{
			//���
			CDoubleMatrix * pt = (CDoubleMatrix *)parX3D->GetAt(j);
		}
		CDoubleMatrix matrix_SIGMA(p,p);
		for(j=0;j<g;j++)
		{
			CDoubleMatrix * pMatrix = (CDoubleMatrix *)arS.GetAt(j);
			matrix_SIGMA += (vector_m[j] - 1.0)*(*pMatrix);
		}
		matrix_SIGMA = matrix_SIGMA / (nSum-g);
		matrix_a = inv( matrix_SIGMA ) * matrix_XGrpVrbMean;
		WriteData(matrix_a,"aaaaaa");
		for(j=0;j<g;j++)
		{
			CDoubleVector vector_temp = matrix_XGrpVrbMean.col(j);
			CDoubleMatrix matrix_temp(1,p);
			matrix_temp = vector_temp;
			matrix_temp = -matrix_temp * inv( matrix_SIGMA ) * trans(matrix_temp);
			vector_b[j] = matrix_temp(0,0)/2;
		}
	
		}
		break;
	case 2:
		//�����б�ϵ��
		//���������в�Ϊ�����Ŀ
		/*int nLen = 0;
		for(i=0;i<vrb;i++)
		{
			if( vector_In[i] != 0 )
			{
				nLen++;
			}
			else 
				break;
		}
		//assert( nLen > 0 );
		if( nLen <= 0 )
			return false;
		CIntVector vect(nLen);
		for(i=0;i<nLen;i++)
		{
			vect[i] = vector_In[i];
		}*/

		CDoubleMatrix matrix_temp(vector_In.vlen(),g);
		for(i=0;i<vrb;i++)
		{
			for(j=0;j<g;j++)
			{
                matrix_temp(i,j)=0;
				for(int l=0;l<vrb;l++)
					matrix_temp(i,j) += matrix_w(vector_In[i],vector_In[l]) * matrix_XGrpVrbMean(vector_In[l],j);
				matrix_temp(i,j) *= nSum-g;
			}			
		}
	     
		//�����б���
		for(j=0;j<g;j++)
		{
			vector_b[j] = 0;
			for(i=0;i<vrb;i++)
			{
				vector_b[j] +=  matrix_temp(i,j) * matrix_XGrpVrbMean(vector_In[i],j);
			}	
			vector_b[j] /= 2.0;
			vector_b[j] = log(vector_prob(j)) - vector_b[j] ;
		}
		m_constant_Vec = vector_b;
		
		for(i=0;i<vrb;i++)
			for(j=0;j<matrix_temp.mcol();j++)
				matrix_a(vector_In[i],j) = matrix_temp(i,j);
		//m_cofficient_mx = matrix_a;
		//
		int nRow = matrix_a.mrow();
		int nCol = matrix_a.mcol();
		CDoubleMatrix cofMatrix(nRow,nCol);
		int nIndex=0;
		for (i=0; i<nRow; i++)
		{
			if (matrix_a(i,0) != 0)
			{
				for (j=0; j<nCol; j++)
					cofMatrix(nIndex,j) = matrix_a(i,j);
				nIndex ++;
			}
		}
		cofMatrix.rerowsize(nIndex);
		m_cofficient_mx = cofMatrix;
		//
		break;
	}
	return true;
}

int CDiscriminant::GetGroup(CDoubleVector vect)
{
	int ncount(1);
	vect.Sort();
	for(int i=0;i<vect.vlen()-1;i++)
	{
		if( fabs( vect[i] - vect[i+1] ) < 1e-6 ) 
		{
		}
		else
		{
			ncount++;
		}
	}	
	return ncount;
}

CDoubleVector CDiscriminant::GetGroupValue(CDoubleVector vect)
{
	CDoubleVector vect_temp(GetGroup(vect));
	vect.Sort();
	int j=1;
	vect_temp[0] = vect[0];
	for(int i=1;i<vect.vlen();i++)
	{
		if( fabs(vect[i] - vect[i-1]) < 1e-6 )
		{
		}
		else
		{
			vect_temp[j] = vect[i];
			j++;
		}
	}
	return vect_temp;
}

CIntVector CDiscriminant::Classification(CDoubleMatrix matrix_X, CDoubleMatrix matrix_a, CDoubleVector vector_b)
{

#ifdef _DEBUG
	WriteData(matrix_X,"x");
	WriteData(matrix_a,"a");
#endif
	CDoubleMatrix matrix_Y = matrix_X * matrix_a;
	int nlen = matrix_Y.mrow();
	CIntVector matrix_vrb(nlen);
	int i=0;
	for(i=0;i<matrix_Y.mrow();i++)
	{
		for(int j=0;j<matrix_Y.mcol();j++)
		{
			matrix_Y(i,j) += vector_b[j];
		}
	}
	for(i=0;i<matrix_Y.mrow();i++)
	{
		double max = -1e100;
		int maxID = -1;
		for(int j=0;j<matrix_Y.mcol();j++)
		{
			if( matrix_Y(i,j) > max )
			{
				max = matrix_Y(i,j);
				maxID = j;
			}
		}
		matrix_vrb[i] = maxID;
	}
	WriteData(matrix_vrb,"matrix_vrb");
	return matrix_vrb;
}


///////////////////////////////////////////////////////////////////////////////////
//
//		�������ܣ����д����ʼ���
//		���룺g���������originalԭʼ�������������predict�ж������������
//		���������
//
///////////////////////////////////////////////////////////////////////////////////
void CDiscriminant::CpTable(CDoubleVector vector_original, CIntVector vector_predict, CDoubleMatrix &matrix_classresult, CDoubleMatrix &matrix_rateresult)
{
	int g = GetGroup(vector_original);
	int n = vector_original.vlen();
// 	TRACE("Row = %d,Col = %d \n",matrix_classresult.mrow(),matrix_classresult.mcol());
	int i=0;
	for(i=0;i<n;i++)
	{
		int i1 = vector_predict[i];
		int j1 = (int)vector_original[i]-1;
		matrix_classresult(i1,j1) = matrix_classresult(i1,j1) + 1;
	}
	WriteData(matrix_classresult,"classresult");
	CDoubleVector vector_sumclass = sum(matrix_classresult);
	for(i=0;i<g;i++)
		for(int j=0;j<g;j++)
			matrix_rateresult(i,j) = matrix_classresult(i,j) / vector_sumclass[i];
}

bool CDiscriminant::GetValueFromVO(CDataInterface *pDataInterface, std::string VO)
{
//	DISCRIMINANT
//		/DATA=[FILE]
//		/MISSING=[{EXCLUDE**}]
//		/CRITERIA=[FIN({3.84**})] [FOUT({2.71**})]
//		/SELECTMETHOD=[{STEPWISE**}]
//		/ANALYMETHOD=[{FISHER**}]
//		/GROUP [VARLIST]
//		/INDEPENDENT [VARLIST]
//		/OPTION [DISRESULT] 
//		/RESULT [GCOV] [CORR] [COV] [TCOV] [FLAMBDA]  [UNSELECTED]//��� ��� ���� ��Э���� f��	lambda ��ѡ����
	AnaWord VOstr;
	VOstr.Import(VO);
	m_dEntry = AnaSyntax::getvalue("/CRITERIA","FIN",VOstr);
	if (m_dEntry == -1)
		m_dEntry = 3.84;
	m_dRemoval = AnaSyntax::getvalue("/CRITERIA","FOUT",VOstr);
	if (m_dRemoval == -1)
		m_dRemoval = 2.71;
	m_bDisResult = AnaSyntax::findstr("/OPTION", "DISRESULT",VOstr);
	m_bC1 = AnaSyntax::findstr("/RESULT","GCOV",VOstr);
	m_bC2 = AnaSyntax::findstr("/RESULT","COV",VOstr);
	m_bT1 = AnaSyntax::findstr("/RESULT","TCOV",VOstr);
	m_bFL = AnaSyntax::findstr("/RESULT","FLAMBDA",VOstr);
	m_bR = AnaSyntax::findstr("/RESULT","CORR",VOstr);
	m_bIn1 = AnaSyntax::findstr("/RESULT","UNSELECTED",VOstr);
	CTStringArray ynames;
	AnaSyntax::getVAR("/GROUP", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	CTString yname = ynames.GetAt(0);
	m_nDependIndex = pDataInterface->IndexByName(yname);
	CTStringArray xname;
	AnaSyntax::getVAR("/INDEPENDENT", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	return TRUE;
}


CResult * CDiscriminant::OnDiscriminant(CDataInterface *pDataInterface, std::string &VO)
{
	m_pDataInterface = pDataInterface;
	m_nCoefficient = 0;
	m_nMethod = 0;
	m_pResult = NULL ;
	m_pResult = new CResult("�б����");
	if (!(GetValueFromVO(pDataInterface, VO)))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	
	int nSum=0;
	CDoubleVector Data=GetData(pDataInterface,nSum);
	CDoubleMatrix matrix = VectorToMatrix(Data,nSum,m_tVarInfo.iCount+2);//[yXweight]ûȨ��ʱ,Ϊ1
	WriteData(matrix, "dafsd");
	//������Ԥ����,��һ�еķ���������ܳ���һ��,��ȥ��Щֻ����һ�εļ�¼
	StepWiseDiscriminant(matrix,m_nMethod+2,m_nCoefficient+2,m_dEntry,m_dRemoval);
	return m_pResult;		
}


//ȡ����,��һ�з������,��������������
//double * CDiscriminant::GetData(CDataInterface *pDataInterface,int &Sum)
CDoubleVector CDiscriminant::GetData(CDataInterface *pDataInterface,int &Sum)
{  
	//double * pData = NULL,* pData1 = NULL;
	CDoubleVector pData;
	int RowCount = pDataInterface->GetRowCount();
	//int* index = NULL;
	CIntVector index;
	//ȡ�����ݣ���֯�������ݾ���
	int nCaseNumb,m = m_tVarInfo.iCount + 2;
	TDColAry * pDataArray=NULL; 
	int nColNum=0;
	bool nDataError = false;
	if( !index.vlen()  )
		index .create(RowCount);
	
	int i=0;
	for(i=0;i<RowCount;i++)
		index[i]=0;

	pDataArray = new TDColAry;
    for(i=0;i<m_tVarInfo.iCount;i++)
	{
		nColNum=m_tVarInfo.pValue[i];
		pDataInterface->GetColData(nColNum,*pDataArray,5);
		for(int j=0;j<RowCount;j++)
			if(pDataArray->GetAt(j).iIndex<0)
				index[j]=1;
	}

	if(m_nDependIndex>=0)
	{
		pDataInterface->GetColData(m_nDependIndex,*pDataArray,5);
		for(int j=0;j<RowCount;j++)
			if(pDataArray->GetAt(j).iIndex<0)
				index[j]=1;
	}

	if((pDataInterface->GetWeightNumb())!=-1 )
	{
		pDataInterface->GetColData(pDataInterface->GetWeightNumb(),*pDataArray,5);
		for(int j=0;j<RowCount;j++)
			if(pDataArray->GetAt(j).iIndex<0)
				index[j]=1;
    }

	//int Sum=0;
	for(i=0;i<RowCount;i++)
		if(index[i]==0)
			Sum+=1;
	for(i=0;i<m_tVarInfo.iCount;i++)
	{
		if( !pData.vlen()  )
			//pData = new double [RowCount * m];
			pData.create(RowCount * m);
		
		nColNum=m_tVarInfo.pValue[i];
		if( nColNum < 0 )
		{
// 			AfxMessageBox("ȡ����ʱ����!");
			return NULL;
		}
		nCaseNumb = pDataInterface->GetColData(nColNum,*pDataArray,5);//ȡ������
		assert(nCaseNumb > 0 );
		//��ֵ
		//�����������ݣ��������ֵ��ͬ���޳�
		for(int j=0;j<nCaseNumb;j++)
			pData[(1+i)*nCaseNumb+j] = pDataArray->GetAt(j).dValue;
//		pData[1+i+j*m] = pDataArray->GetAt(j).dValue;
		
	}
    //ȡ�������������
	//double a=0.0;
	//int b=0;
	if(m_nDependIndex>=0)
	{
		nCaseNumb = pDataInterface->GetColData(m_nDependIndex,*pDataArray,5);//ȡ������
		if(nCaseNumb > 0)
		{
//			if(!IsEqual((TDataSet *)pDataArray,nCaseNumb))
			if(!IsEqual2(*pDataArray,nCaseNumb))
				for(int j=0;j<nCaseNumb;j++)
					pData[j] = pDataArray->GetAt(j).dValue;
//				pData[j*m] = pDataArray->GetAt(j).dValue;
				else
				nDataError=true;
		}
		else
			nDataError=true;
	}

	if((pDataInterface->GetWeightNumb())!=-1 )
	{
		nCaseNumb = pDataInterface->GetColData(pDataInterface->GetWeightNumb(),*pDataArray,5);//ȡ������
		if(nCaseNumb > 0)
		{
			if(!IsEqual((TDataSet *)pDataArray,nCaseNumb))		
				for(int j=0;j<nCaseNumb;j++)
					pData[j + (m-1)*nCaseNumb] = pDataArray->GetAt(j).dValue;
//				pData[(j+1)*m-1] = pDataArray->GetAt(j).dValue;
				else
				nDataError=true;
		}
		else
			nDataError=true;
	}
	else
		for(int j=0;j<nCaseNumb;j++)
			pData[j + (m-1)*nCaseNumb] = 1;
//		pData[(j+1)*m-1] = 1;
		
	if( pDataArray )
	{
		delete  pDataArray;
		pDataArray=NULL;
	}
		
	if(Sum!=RowCount)
	{
		CDoubleVector pData1(Sum*m);
	    int t=0;
        for(i=0;i<RowCount;i++)
		{
			if(index[i]==0)
			{
				for(int j=0;j<m;j++)
					pData1[t+j*Sum]=pData[i+j*RowCount];
//					pData1[t*m+j]=pData[i*m+j];
                t++;
			}
		}
		return pData1;
	}
	else
	{
        return pData;
	}
}


///////////////////////////////////////////////////////////////
//
//						���յ�һ������
//
///////////////////////////////////////////////////////////////
void CDiscriminant::SortMatrix(CDoubleMatrix &matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	double temp,temp1;
	int n;
	int i=0;
	for(i=0;i<nRow;i++)
	{
		temp=matrix(i,0);
		n=i;
		for(int j=i;j<nRow;j++)
		{
			if(matrix(j,0)<temp)
			{
				temp=matrix(j,0);
                n=j;
			}
		}
		if(n!=i)
		{
			for(int k=0;k<nCol;k++)
			{
				temp1 = matrix(i,k);
		        matrix(i,k) = matrix(n,k);
			    matrix(n,k) = temp1;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////
//
//			����һ�����ݹ��,��1��ʼ
//
/////////////////////////////////////////////////////////////////
void CDiscriminant::DataProcess(CDoubleMatrix &matrix)
{
	int nLen = matrix.mrow();
	CIntVector vect(nLen);
	int i=0;
	for(i=0;i<nLen;i++)
	{
		vect[i] = (int)matrix(i,0);
	}
	int min = vect[0];
	//����Сֵ
	for(i=0;i<nLen;i++)
	{
		if( vect[i] < min )
		{
			min = vect[i];
		}
	}
	//�����Сֵ��С��1,������������ 1 - min 
	if( min <= 1 )
	{
		//�����ƶ�
		for(i=0;i<nLen;i++)
		{
			vect[i] = vect[i] - min + 10 ;
		}
	}
	int k=0;
	for(i=0;i<nLen;i++)
	{
		int temp = vect[i];
		if( temp <= k+1 ) //�Ѿ����¹���Ԫ��
		{
			continue;
		}	
		k++;	
		vect[i]  = k;
		for(int j=0;j<nLen;j++)
		{
			if( vect[j] == temp )
			{
				vect[j] = k;
			}
		}	
		
	}
	for(i=0;i<nLen;i++)
	{
		matrix(i,0) = vect[i];
	}
}

////////////////////////////////////////////////////////////////////
//
//					���������
//
////////////////////////////////////////////////////////////////////
void CDiscriminant::CreateResult(CDoubleMatrix matrix_a,CDoubleVector vector_b,CDoubleMatrix matrix_result)
{
	CTLTable * pTable = new CTLTable;
//	pTable->SetCols(matrix_a.mcol()+1);
//	pTable->SetRows(matrix_a.mrow()+2);

	pTable->SetTitle("�б�ϵ��");	
	pTable->InsertColumn(0,"");
	int i=0;
	for(i=0;i<matrix_a.mcol();i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertColumn(i+1,str);
	}
	for(i=0;i<matrix_a.mrow();i++)
	{
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		for(int j=0;j<matrix_a.mcol();j++)
		{
			pTable->SetItemText(i,j+1,matrix_a(i,j));
		}
	}
	pTable->InsertItem(i,"������");
	int j=0;
	for(j=0;j<vector_b.vlen();j++)
	{
		pTable->SetItemText(i,j+1,vector_b[j]);
	}
	CRsltElementTable * pETable=new CRsltElementTable("�б�ϵ��",pTable);
	m_pResult->Add(pETable);

	pTable = new CTLTable;
	pTable->SetCols(matrix_result.mcol()+1);
	pTable->SetRows(matrix_result.mrow()+1);
	pTable->SetTitle("������");
	pTable->InsertColumn(0,"");
	for(i=0;i<matrix_result.mcol();i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertColumn(i+1,str);
	}
	for(i=0;i<matrix_result.mrow();i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str);
		for(int j=0;j<matrix_result.mcol();j++)
		{
			pTable->SetItemText(i,j+1,matrix_result(i,j));
		}
	}
	pETable=new CRsltElementTable("������",pTable);
	m_pResult->Add(pETable);

}


//������Ԥ����,��һ�еķ���������ܳ���һ��,��ȥ��Щֻ����һ�εļ�¼
CDoubleMatrix CDiscriminant::DeleteSingleGroup(CDoubleMatrix matrix)
{
    int nLen = matrix.mrow();
	bool * pFlag = new bool [nLen];
	int i=0;
    for(i=0;i<nLen;i++)
		matrix(i,0)=(int)matrix(i,0);
	for(i=0;i<nLen;i++)
	{
		pFlag[i] = false;
	}
	for(i=0;i<nLen;i++)
	{
		int count = 0;
		double temp = matrix(i,0);
		for(int j=0;j<nLen;j++)
		{
			if( fabs( temp - matrix(j,0) ) < 1e-6 )
			{
				count++;
			}
		}
		if( count == 1 )
		{
			pFlag[i] = true;
		}
	}
	//ͳ�����µ�
	int count=0;
	for(i=0;i<nLen;i++)
	{
		if( pFlag[i] == false )
		{
			count++;
		}
	}
	//���췵�صĺ���,��ֵ
	CDoubleMatrix matrix_temp(count,matrix.mcol());
	int k=0;
	for(i=0;i<nLen;i++)
	{
		if( pFlag[i] == false )
		{
			for(int j=0;j<matrix.mcol();j++)
			{
				matrix_temp(k,j) = matrix(i,j);
			}
			k++;
		}
	}	

	delete [] pFlag;pFlag = NULL;
	return matrix_temp;
}

void CDiscriminant::CreateResultC2(CDoubleMatrix *pt,char * pszTitle)
{
    int mC2=pt->mrow();
	CTLTable * pTable = new CTLTable();
	pTable->SetRows(mC2);
	pTable->SetCols(mC2+1);
	pTable->SetTitle(pszTitle);
	pTable->InsertColumn(0,"");
	int i=0;
    for(i=0;i<mC2;i++)
	{   
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
    	pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
    }
    for(i=0;i<mC2;i++)
		for(int j=0;j<mC2;j++)
			pTable->SetItemText(i,j+1,(*pt)(i,j));

	CRsltElementTable * pETable=new CRsltElementTable(pszTitle,pTable);
	m_pResult->Add(pETable);
}

void CDiscriminant::CreateResultF(CDoubleVector F, CDoubleVector L)
{
	CTLTable * pTable = new CTLTable();
	pTable->SetRows(3);
	pTable->SetCols(F.vlen()+1);
	pTable->SetTitle("Fֵ��LAMBDA");
	pTable->InsertColumn(0,"");
	int i=0;
    for(i=0;i<F.vlen();i++)
		pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));

	pTable->InsertItem(0,"Fֵ");
	pTable->InsertItem(1,"LAMBDA");
	for(i=0;i<F.vlen();i++)
	{
		pTable->SetItemText(0,i+1,F[i]);
		pTable->SetItemText(1,i+1,L[i]);
	}
	CRsltElementTable * pETable=new CRsltElementTable("Fֵ��LAMBDA",pTable);
	m_pResult->Add(pETable);
}

void CDiscriminant::CreateResultC1(CDoubleMatrix matrix_C1)
{
	int mC1=matrix_C1.mrow();
	CTLTable * pTable = new CTLTable();
	pTable->SetRows(mC1);
	pTable->SetCols(mC1+1);
	pTable->SetTitle("���Э������");
	pTable->InsertColumn(0,"");
	int i=0;
    for(i=0;i<mC1;i++)
    	pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
    for( i=0;i<mC1;i++)
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
    for(i=0;i<mC1;i++)
		for(int j=0;j<mC1;j++)
			pTable->SetItemText(i,j+1,matrix_C1(i,j));

	CRsltElementTable * pETable=new CRsltElementTable("���Э������",pTable);
	m_pResult->Add(pETable);
}

void CDiscriminant::CreateResultR(CDoubleMatrix matrix_R)
{
	int mR=matrix_R.mrow();
	CTLTable * pTable = new CTLTable();
	pTable->SetRows(mR);
	pTable->SetCols(mR+1);
	pTable->SetTitle("�����");
	pTable->InsertColumn(0,"");
	int i=0;
    for(i=0;i<mR;i++)
	{   
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
    	pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
    }
    for(i=0;i<mR;i++)
		for(int j=0;j<mR;j++)
			pTable->SetItemText(i,j+1,matrix_R(i,j));

	CRsltElementTable * pETable=new CRsltElementTable("�����",pTable);
	m_pResult->Add(pETable);
}

void CDiscriminant::CreateResultT1(CDoubleMatrix matrix_TC)
{
	int mTC=matrix_TC.mrow();
	CTLTable * pTable = new CTLTable();
	pTable->SetRows(mTC);
	pTable->SetCols(mTC+1);
	pTable->SetTitle("��Э������");
	pTable->InsertColumn(0,"");
	int i=0;
    for(i=0;i<mTC;i++)
	{   
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
    	pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
    }
    for(i=0;i<mTC;i++)
		for(int j=0;j<mTC;j++)
			pTable->SetItemText(i,j+1,matrix_TC(i,j));

	CRsltElementTable * pETable=new CRsltElementTable("��Э������",pTable);
	m_pResult->Add(pETable);
}

void CDiscriminant::CreateResultIn1(CDoubleVector F,CDoubleVector L,CIntVector In,int vrb,CTString pszTitle)
{
	CTLTable * pTable = new CTLTable();
	pTable->SetRows(3);
	pTable->SetCols(F.vlen()+1-vrb);
	pTable->SetTitle(pszTitle);
	pTable->InsertColumn(0,"");

	int s,n=0;
	int i=0;
    for(i=0;i<F.vlen();i++)
	{
		s=0;
		for(int j=0;j<vrb;j++)
			if(i==In[j])
				s=1;
		if(s==0)
		{
			//if(i > m_nDependIndex)

			pTable->InsertColumn(n+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			n=n+1;
		}
	}

	pTable->InsertItem(0,"����Fֵ");
	pTable->InsertItem(1,"LAMBDA");
    n=0;
	
	for(i=0;i<F.vlen();i++)
	{
		s=0;
		for(int j=0;j<vrb;j++)
			if(i==In[j])
				s=1;
		if(s==0)
		{
			pTable->SetItemText(0,n+1,F[i]);
		    pTable->SetItemText(1,n+1,L[i]);
			n=n+1;
		}
	}
	CRsltElementTable * pETable=new CRsltElementTable(pszTitle,pTable);
	m_pResult->Add(pETable);
}

void CDiscriminant::CreateResultCO(CDoubleMatrix a,CDoubleVector b,int vrb,CDoubleVector vector_dnum)
{
	int ma=a.mrow();
	int na=a.mcol();
	CTLTable * pTable = new CTLTable();

	pTable->SetTitle("���ຯ��ϵ��ֵ");
	pTable->InsertColumn(0,"");
	CTString str;
	CDWordArray dataInput;
	
	int s=0;
	int i=0;
    for(i=0;i<ma;i++)
	{
		if(a(i,0)!=0.0)
		{
			pTable->InsertItem(s,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			dataInput.Add(m_tVarInfo.pValue[i]);
			s++;
		}
	}
	
	int nTemp1 = m_pDataInterface->GetColsData(dataInput, m_finalClassData, 1);
	double adsfa  = m_finalClassData(0,0);
    pTable->InsertItem(vrb,"����");
	int j=0;
	for(j=0;j<na;j++)
	{
		int h=vector_dnum[j];
		str.Format("%d",h); //j+1
		pTable->InsertColumn(j+1,(char *)str);
	}

    s=0;
	for(i=0;i<ma;i++)
	{
		if(a(i,0)!=0.0)
		{
			for(int j=0;j<na;j++)
			{
				pTable->SetItemText(s,j+1,a(i,j));
			}
		    s++;
		}
	}
    for(j=0;j<na;j++)
	{
		pTable->SetItemText(vrb,j+1,b[j]);
	}
   
	CRsltElementTable * pETable=new CRsltElementTable("���ຯ��ϵ��ֵ",pTable);
	m_pResult->Add(pETable);
}


/////////*********�б��������**********///////////////
void CDiscriminant::Classify(CDoubleMatrix &finalClassData)
{
	int i = 0;
	int j = 0;	
	int rownum = m_original_mx.mrow();
	int colnum = finalClassData.mcol();
	CDoubleMatrix group_mx(rownum,1,1.0);
	for (i=0; i<rownum; i++)
		group_mx(i,0) = m_original_mx(i,0);
	m_oriclass_Vec = group_mx(0).copy();
	CDoubleVector oldclass = m_oriclass_Vec.copy();
	if (m_class_Vec.Min() != 1)
	{
		for (i=0; i<rownum; i++)
		{
			for (j=0; j<m_group; j++)
			{
				if (oldclass(i) == m_class_Vec(j))				
					oldclass(i) = j;				
			}
			oldclass(i) = oldclass(i) + 1.0;
		}
	}
	CDoubleMatrix datanew_mx = group_mx.commat(finalClassData,1);	
	CDoubleMatrix score_mx(rownum,m_group,0.0);
    for (i=0; i<m_group; i++)
	{
		for (j=0; j<rownum; j++)
		{
			CDoubleVector data_Vec = finalClassData.row(j);		
			CDoubleVector index_vec = m_cofficient_mx.col(i);	
			double f1 = m_constant_Vec(i);
			double f2 = index_vec.dot(data_Vec);
			score_mx(j,i) = m_constant_Vec(i) + data_Vec.dot(index_vec);		
		}	
    }
	CDoubleVector reclassify_Vec(rownum,1.0);
	int nIndex = 0;
	double dtemp = 0;
	for (i=0; i<rownum; i++)
	{	
		CDoubleVector v = score_mx.row(i);
		dtemp =	v.Max(nIndex);
		reclassify_Vec(i) = nIndex + 1;	
	}
	CDoubleVector count_Vec(m_group,0.0);
	CDoubleVector percent_Vec(m_group,0.0);
	CDoubleVector error_Vec(m_group,0.0);
    for (j=0; j<m_group; j++)
	{   
		for (i=0; i<rownum; i++)
		{
			if (oldclass(i) == j+1 && reclassify_Vec(i) == j+1)
			{			
				count_Vec(j) = count_Vec(j) + 1;
				percent_Vec(j) = count_Vec(j) / m_num_Vec(j) * 100;
				error_Vec(j) = 100 - percent_Vec(j);
			}
		}	
    }
	CDoubleMatrix discriminant_mx(m_group,m_group,0.0);	
	CDoubleMatrix discriminantpercent_mx(m_group,m_group,0.0);
	for (j=0; j<m_group; j++)
	{
		for (int k=0; k<m_group; k++)
		{
			double  count =0;
			discriminant_mx(k,k) = count_Vec(k);		
			for (i=0; i<rownum; i++)
			{
				if (oldclass(i)==j+1 && reclassify_Vec(i)==k+1 && j!=k)
				{				
					count = count + 1;
					discriminant_mx(k,j) = count;					
				}
			}
			discriminantpercent_mx(k,j) = discriminant_mx(k,j) / m_num_Vec(j) * 100;
		}
    }
	discriminant_mx = discriminant_mx.trans();
	discriminantpercent_mx = discriminantpercent_mx.trans();
	m_discri_mx = discriminant_mx.copy();	
	m_discripercent_mx = discriminantpercent_mx.copy();
	if (m_oriclass_Vec.Min() != 1)
	{
		for (i=0; i<rownum; i++)
		{
			for (j=0; j<m_group; j++)
			{
				if (reclassify_Vec(i) == j+1)				
					reclassify_Vec(i) = m_class_Vec(j);			
			}
		}
	}
    m_newclass_Vec = reclassify_Vec.copy();	
	return;
}

//////////////********�б�������з��������*********///////////
void CDiscriminant::CreateResult()
{
	
	CTString szText("");
	int nCount = m_newclass_Vec.vlen();
	if (nCount != m_pDataInterface->m_DataAccess.GetRecordCount())
	{
		CTString szWarning = "���з�������,���������У�";
		CRsltElementText * pText = new CRsltElementText( "����!" );
		pText->AddString(szWarning);
		m_pResult->Add(pText);
		return;
	}
	CTString szFldName = "�������";
	int nIndex = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
	if (nIndex >= 0)
	{
		m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nIndex);
	}
	CFieldType ft = m_pDataInterface->GetFieldType(m_nDependIndex);
	CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(ft);
	pField->SetFieldName(szFldName);
	m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
	m_pDataInterface->m_DataAccess.First();
	nIndex = 0;
	while (!m_pDataInterface->m_DataAccess.Eof())
	{
		m_pDataInterface->m_DataAccess.Edit();
		if (ft == fDouble)
			pField->SetAsDouble(m_newclass_Vec(nIndex++));
		else if (ft == fInt)
			pField->SetAsInteger(m_newclass_Vec(nIndex++));
		else
		{
			szText.Format("%.f",m_newclass_Vec(nIndex++));
			pField->SetAsString(szText);
		}

		m_pDataInterface->m_DataAccess.Next();
	}
	/*
	CTString szFileName = m_pDataInterface->m_DataAccess.GetFileName();
	CTString szSaveAs("");
	m_pDataInterface->m_DataAccess.SaveFile(szFileName,&szSaveAs);
	*/
}


//////////********�б�������н��ͼ��************///////////////////
void CDiscriminant::CreateResultdiscri(CDoubleMatrix discriminant_mx)
{
	int i=0, j=0, nRow=0;
	double dValue=0, dCount=0;
    CTString szText("");
	
	nRow = discriminant_mx.mrow();
	CTLTable *pTable = new CTLTable();
	pTable->SetTitle("�б�����ش�");
	pTable->InsertColumn(0,"���");
	for (i=0; i<nRow+1; i++)
	{
    	if (i < nRow )
		{
			szText.Format("%.f",m_class_Vec(i));
			pTable->InsertColumn(i+1,szText);
		}
		else
		{
			pTable->InsertColumn(i+1,"�ܼ�");
		}
	}
	for (i=0; i<nRow; i++)
	{
		dCount = 0;
		szText.Format("%.f",m_class_Vec(i));
		pTable->InsertItem(i,szText);
		for (j=0; j<nRow+1; j++)
		{
			if (j < nRow)
			{
				dValue = discriminant_mx(i,j);
				dCount += dValue;
				pTable->SetItemText(i,j+1,dValue);
			}
			else
			{
				pTable->SetItemText(i,j+1,dCount);
			}
		}
	}

	CRsltElementTable * pETable=new CRsltElementTable("�б�����ش�",pTable);
	m_pResult->Add(pETable);
}

//////////********�б�������аٷֱȽ��ͼ��************///////////////////
void CDiscriminant::CreateResultdiscripercent(CDoubleMatrix discriminantpercent_mx)
{
	int i=0, j=0, nRow=0;
	double dValue=0, dCount=0;
	CTString szText("");
	
	CTLTable *pTable = new CTLTable();
	nRow = discriminantpercent_mx.mrow();
	pTable->SetTitle("�б�����ش����(�ٷֱ�)");
	pTable->InsertColumn(0,"���");
	for (i=0; i<nRow+1; i++)
	{
    	if (i < nRow )
		{
			szText.Format("%.f",m_class_Vec(i));
			pTable->InsertColumn(i+1,szText);
		}
		else
		{
			pTable->InsertColumn(i+1,"�ܼ�");
		}
	}
	for (i=0; i<nRow; i++)
	{
		dCount = 0;
		szText.Format("%.f",m_class_Vec(i));
		pTable->InsertItem(i,szText);
		for (j=0; j<nRow+1; j++)
		{
			if (j < nRow)
			{
				dValue = discriminantpercent_mx(i,j);
				dCount += dValue;
				pTable->SetItemText(i,j+1,dValue);
			}
			else
			{
				pTable->SetItemText(i,j+1,dCount);
			}
		}
	}
	CRsltElementTable * pETable=new CRsltElementTable("�б�����ش����(�ٷֱ�)",pTable);
	m_pResult->Add(pETable);
}
















