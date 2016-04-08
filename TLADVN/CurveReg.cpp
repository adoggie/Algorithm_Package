// CurveReg.cpp: implementation of the CCurveReg class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tlAdvn.h"
#include "CurveReg.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"

// #include "..\TLCRNT\Script.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

#define POSITIVE_INF 0.0000001

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurveReg::CCurveReg()
{
	m_pResult = NULL;
	m_pDataInterface = NULL;
	m_nCurVarIndex = 0;
	m_nDependIndex = -1;
	//	陈思多，01/13/03
	m_bIsWeighted=false;//是对数据进行了加权
}

CCurveReg::~CCurveReg()
{
	if( m_tVarInfo.iCount> 0)
		m_tVarInfo.DeleteObject();
}



/////////////////////////////////////////////////////////////////////
//
//				线性回归
//
/////////////////////////////////////////////////////////////////////
void CCurveReg::Linear(int n, double x[], double y[], double w[], double a[])
{
	int i;
	int k=1;//线性回归中取1

	double xx,yy,lxx,lxy,lyy;

	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];

	xx=0.0;
	yy=0.0;     
	for (i=0; i<=n-1; i++)
	{ 
		xx=xx+w[i]*x[i];
		yy=yy+w[i]*y[i];
	}
	xx=xx/W;
	yy=yy/W;

	//	平方和们
	lxx=0.0;
	lxy=0.0;
	lyy=0.0;
	for (i=0; i<=n-1; i++)
	{  
		a[2]=x[i]-xx; 
		lxx=lxx+w[i]*a[2]*a[2];
		lxy=lxy+w[i]*a[2]*(y[i]-yy);
		lyy=lyy+w[i]*(y[i]-yy)*(y[i]-yy);
	}
	
	//发生除零情况
	if(fabs(lxx)<=POSITIVE_INF || fabs(lyy)<=POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	//a[1]回归系数中的 a
	a[1]=lxy/lxx;      
	
	//a[0]回归系数中的 b
	a[0]=yy-a[1]*xx;    
	a[2]=0.0; 
	a[4]=0.0;   
	for(i=0;i<=n-1;i++)
	{
		a[3]=a[1]*x[i];
		a[2]+=w[i]*(y[i]-a[3]-a[0])*(y[i]-a[3]-a[0]);
		a[4]+=w[i]*(x[i])*(x[i]);    
	}  
	
	//发生除零情况
	if(fabs(a[2])<POSITIVE_INF || fabs(a[4])<POSITIVE_INF)
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}
	}
	
	double SEB0,SEB1;
	
	//回归系数b的标准误差 
	SEB1=sqrt(a[2]/((W-k-1)*lxx));    
	
	//回归系数a的标准误差 
	SEB0=sqrt((a[2]*a[4])/(W*(W-k-1)*(lxx)));   
	
	double R,R_S,A_R,SER;
	
	//复相关系数R 
	R=fabs(lxy/sqrt(lxx*lyy)); 
	
	//复相关系数R的平方 
	R_S=R*R; 
	
	//调整后的复相关系数
    A_R=R_S-(1-R_S)*k/(W-k-1); 
	
	//复相关系数的标准误差 
	SER=sqrt(a[2]/(W-k-1));   
	
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;

	double SSr,M_S_Rg,M_S_Rs,F;
	
	//回归平方和
	SSr=(lxy*lxy)/lxx; 
	
	//回归均方    
	M_S_Rg=SSr/k;

	//残差均方
	M_S_Rs=a[2]/(W-k-1);

	//F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=lxy/sqrt(lxx*lyy); 

    //T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	
	FillChartData(n,x,NULL,0,"线性回归",a[0],a[1]);
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F);
	return ;
}



//**********************************************************************//
//author:zkc
//Date:2003.9.23
//function:
//***********************************************************************//
void CCurveReg::FillChartData(int nLen,double pDataX[],double pDataY[],int nMethod,CTString strLegend,
				   double b0,double b1,double b2,double b3)
{
	TDataPointValVal* pDPSV=new TDataPointValVal[nLen];	
	m_flag=true;
	for(int j=0;j<nLen;j++)
	{
		//	strTemp.Format("%d",);
		pDPSV[j].fXVal =pDataX[j];//strTemp;
		switch(nMethod)
		{
		case 0://linear
			{	
				pDPSV[j].fYVal =b1*pDataX[j]+b0;
				break;
			}
		case 1://logarithmic
			{
				pDPSV[j].fYVal=b1*log(pDataX[j])+b0;
				break;
			}
		case 2://inverse
			{
				pDPSV[j].fYVal=b1/pDataX[j]+b0;
				break;
			}
		case 3://quadratic
			{
				pDPSV[j].fYVal=b1*pDataX[j]+b2*pDataX[j]*pDataX[j]+b0;
				break;
			}
		case 4://cubic
			{
				pDPSV[j].fYVal=b1*pDataX[j]+b2*pow(pDataX[j],2.0)+
					b3*pow(pDataX[j],3.0)+b0;
				break;
			}
		case 5://compound
			{
				pDPSV[j].fYVal=b0*pow(b1,pDataX[j]);
				break;
			}
		case 6://power
			{
				pDPSV[j].fYVal=b0*pow(pDataX[j],b1);
				break;
			}
		case 7://S
			{
				pDPSV[j].fYVal=exp(b0+b1/pDataX[j]);
				break;
			}
		case 8://growth
			{
				pDPSV[j].fYVal=exp(b0+b1*pDataX[j]);
				break;
			}
		case 9://exponential
			{
				pDPSV[j].fYVal=b0*exp(b1*pDataX[j]);
				break;
			}
		case 10://logistics
			{
				pDPSV[j].fYVal=1.0/(1.0/m_u+b0*pow(b1,pDataX[j]));
				break;
			}
		default://the input dependent variable.
			{
				pDPSV[j].fYVal=pDataY[j];
				break;
			}

		}
	}
	m_dataPtr.Add(pDPSV);
	m_Legend.Add(strLegend);
}
void CCurveReg::FillChartData2(int nLen,CDoubleVector Vect_X,CDoubleVector Vect_Y,int nMethod,CTString strLegend,
							  double b0,double b1,double b2,double b3)
{
	TDataPointValVal* pDPSV=new TDataPointValVal[nLen];	
	m_flag=true;
	for(int j=0;j<nLen;j++)
	{
		//	strTemp.Format("%d",);
		pDPSV[j].fXVal =Vect_X[j];//strTemp;
		switch(nMethod)
		{
		case 0://linear
			{	
				pDPSV[j].fYVal =b1*Vect_X[j]+b0;
				break;
			}
		case 1://logarithmic
			{
				pDPSV[j].fYVal=b1*log(Vect_X[j])+b0;
				break;
			}
		case 2://inverse
			{
				pDPSV[j].fYVal=b1/Vect_X[j]+b0;
				break;
			}
		case 3://quadratic
			{
				pDPSV[j].fYVal=b1*Vect_X[j]+b2*Vect_X[j]*Vect_X[j]+b0;
				break;
			}
		case 4://cubic
			{
				pDPSV[j].fYVal=b1*Vect_X[j]+b2*pow(Vect_X[j],2.0)+
					b3*pow(Vect_X[j],3.0)+b0;
				break;
			}
		case 5://compound
			{
				pDPSV[j].fYVal=b0*pow(b1,Vect_X[j]);
				break;
			}
		case 6://power
			{
				pDPSV[j].fYVal=b0*pow(Vect_X[j],b1);
				break;
			}
		case 7://S
			{
				pDPSV[j].fYVal=exp(b0+b1/Vect_X[j]);
				break;
			}
		case 8://growth
			{
				pDPSV[j].fYVal=exp(b0+b1*Vect_X[j]);
				break;
			}
		case 9://exponential
			{
				pDPSV[j].fYVal=b0*exp(b1*Vect_X[j]);
				break;
			}
		case 10://logistics
			{
				pDPSV[j].fYVal=1.0/(1.0/m_u+b0*pow(b1,Vect_X[j]));
				break;
			}
		default://the input dependent variable.
			{
				pDPSV[j].fYVal=Vect_Y[j];
				break;
			}
			
		}
	}
	m_dataPtr.Add(pDPSV);
	m_Legend.Add(strLegend);
}

void CCurveReg::CreateChartResult(double pDataX[],CTString strField,CResult* pResult,int nLen)
{
	CRsltElementChart* pREC;
	CTChartLine* pTChartLine=new CTChartLine;
	CTString str = "自变量";
	pTChartLine->SetXAxilLabel(str);
	str ="因变量";
	pTChartLine->SetYAxilLabel(str);
	CTString strTitle=strField+"拟合图";
	pREC=new CRsltElementChart(strTitle,pTChartLine);
	pResult->Add(pREC);    
	//得到Variable数据//////////////////////////////////////////////////////////
	int j=0;
	for(j=0;j<m_dataPtr.GetSize();j++)
	{
 		pTChartLine->SetLine(m_dataPtr.GetAt(j),nLen,ValVal,m_Legend.GetAt(j));
	}
	for(j=0;j<m_dataPtr.GetSize();j++)
	{
		TDataPointValVal* p=(TDataPointValVal*)(m_dataPtr.GetAt(j));
		delete []p;
	}
	m_dataPtr.RemoveAll();
}


/////////////////////////////////////////////////////////////////////
//
//				二次、三次方回归(加权) zfm 2003－3－11
//
/////////////////////////////////////////////////////////////////////
void CCurveReg::Quad_Cubic(int n, int p, double *xarray, double *yarray, double *weight)
{
	
	int i,j;
	double yy=0.0,Lyy=0.0;
	CDoubleVector x(xarray,n);
	CDoubleMatrix y(yarray,1,n);
	CDoubleVector serrbeta(p+1); //beta系数标准误
	CDoubleVector meanx(p);
	CDoubleMatrix xy(p,1); 
	CDoubleMatrix matrix(p+1,n); //(p+1)*n矩阵  存储x[] 和x平方[](三次型还要存储x立方[]);
	CDoubleMatrix matrixOrigin(p,n);
	CDoubleVector w(weight,n); //原始权重
	CDoubleVector w_sqrt(n);   //权重开根号
	CDoubleMatrix y_sqrt(1,n); //权重开根号作用y
	w_sqrt= sqrt(w);
	CDoubleVector vec =w_sqrt*y.row(0);
	for (i = 0; i < vec.vlen();i++)
		y_sqrt(0,i) = vec(i);

	double W=w.sum();//权值之和
	
	for(i=0; i<p+1; i++)
	{
		for(j=0;j<n;j++)
		{
			matrix(i,j)=w_sqrt(j)*pow(x(j),i); //用于存储根号加权后的X列
			double aa = matrix(i,j);
			if(i<p)
			{
				matrixOrigin(i,j)=pow(x(j),i+1); //用于存储原始X列
			}
		}
	}
	yy=y.row(0).mean_w(w);        //y[]均值
	Lyy=y.row(0).var_w(w)*(W-1); //Lyy为总平方和	
	
	CDoubleMatrix XtransX=matrix * trans(matrix );
	if (rank(XtransX)!=p+1)  //判奇异???注意，没有报错信息
	{
		ReportError();
		return;
	}
	CDoubleMatrix InvMatrix=inv( XtransX ); //matrix是(p+1)*n,所以刚好和标准公式相反

	
	CDoubleMatrix BetaMatrix=InvMatrix * matrix * trans(y_sqrt); //回归系数：B0,B1...BP
	

	CDoubleVector prey = (trans(matrix) * BetaMatrix).col(0);
	CDoubleVector vector_e = y_sqrt.row(0) - prey;

	double Q = 0;  //残差平方和
	for(i=0;i<vector_e.vlen();i++)
	{
		Q += vector_e[i]*vector_e[i];
	}

	double U=Lyy-Q;//回归平方和 

	double R,R_S,A_R,SER;
	R=sqrt(U/Lyy); //复相关系数
	R_S=R*R; //复相关系数的平方
	A_R=R_S-(1-R_S)*p/(W-p-1);//调整后的复相关系数
	SER=sqrt(Q/(W-p-1));// 残差平方和标准误
	double M_S_Rg,M_S_Rs,F;   
	CDoubleVector Beta(p),T(p+1); //标准化Beta系数和T检验向量
	int DF_Rg=p;
	double DF_Rs=(W-p-1);  //自由度，它可以是小数
	M_S_Rg=U/DF_Rg;       //回归均方    
	M_S_Rs=Q/DF_Rs; //残差均方
	F=M_S_Rg/M_S_Rs; //F检验值

	serrbeta=sqrt(diag(InvMatrix))*SER;  
	
	for(i=0;i<p+1;i++)
	{
		if(i<p)
		{
			CDoubleVector diagVec=diag(XtransX);
			
			Beta(i)=BetaMatrix(i+1,0)*sqrt( matrixOrigin.row(i).var_w(w) *(W-1)/Lyy); //Beta[]只有p个
		

		}
		T(i)=BetaMatrix(i,0)/serrbeta(i);
	}
	
	if(p==2)
	{
		FillChartData2(n,x,NULL,3,"二次曲线",BetaMatrix(0,0),BetaMatrix(1,0),BetaMatrix(2,0));//////////////////////////////////////////////////////////////////////////
		
	}
	else
	{
		FillChartData2(n,x,NULL,4,"三次曲线",BetaMatrix(0,0),BetaMatrix(1,0),BetaMatrix(2,0),BetaMatrix(3,0));
	}
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,U,Q,M_S_Rg,M_S_Rs,F,BetaMatrix,serrbeta,Beta,T);
	return ;
}


void CCurveReg::CreateResult(CTString strHead, double R, double R_S, double A_R, double SER, 
				int DF_Rg, double DF_Rs, double U, double Q, double M_S_Rg, double M_S_Rs, double F, 
				CDoubleMatrix BetaMatrix, CDoubleVector serrbeta, CDoubleVector Beta, CDoubleVector T) //zfm
{
	strHead += "\n";
	strHead += m_strModel;
	strHead += "\n";
	CTLTable * pTable = new CTLTable();
//	pTable->SetRows(5);
//	pTable->SetCols(2);

	pTable->SetTitle("模型分析");
	pTable->SetHead(strHead);
	pTable->InsertColumn(0,"指标名");
	pTable->InsertColumn(1,"数值");
	pTable->InsertItem(0,"复相关系数");
	pTable->InsertItem(1,"样本决定系数");
	pTable->InsertItem(2,"修正的决定系数");
	pTable->InsertItem(3,"标准误");
	
	pTable->SetItemText(0,1,R);
	pTable->SetItemText(1,1,R_S);
	pTable->SetItemText(2,1,A_R);
	pTable->SetItemText(3,1,SER);
	CRsltElementTable * pETable= new CRsltElementTable("模型分析",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);

	pTable=new CTLTable();
	pTable->SetTitle("方差分析");
//	pTable->SetRows(3);
//	pTable->SetCols(4);

	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"自由度");
	pTable->InsertColumn(2,"平方和");
	pTable->InsertColumn(3,"均方");
	pTable->InsertColumn(4,"F值");
	pTable->InsertColumn(5,"Pr(>F)");

	pTable->InsertItem(0,"回归");
	pTable->InsertItem(1,"残差");
	pTable->SetItemText(0,1,DF_Rg);

		


	pTable->SetItemText(0,5,1-fcdf(F,DF_Rg,DF_Rs));
	CTString strDF_Rs;
	if(fabs(DF_Rs)<=int(DF_Rs)+1e-30) //自由度为整数
	{
		int nTemp =(int) DF_Rs;		
		strDF_Rs.Format("%d",nTemp);
	}
	else
	{
		strDF_Rs.Format("%f",DF_Rs);
	}	
	
	
	pTable->SetItemText(1,1,strDF_Rs);
	pTable->SetItemText(0,2,U);
	pTable->SetItemText(1,2,Q);
	pTable->SetItemText(0,3,M_S_Rg);
	pTable->SetItemText(1,3,M_S_Rs);
	pTable->SetItemText(0,4,F);
	

	pETable=new CRsltElementTable("方差分析",pTable);
	m_pResult->Add(pETable);


	pTable = new CTLTable;
	pTable->SetTitle("回归系数分析");
	
//	pTable->SetRows(3);
//	pTable->SetCols(5);

	pTable->InsertColumn(0,"");
	
	//pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"系数");
	pTable->InsertColumn(2,"标准误");
	pTable->InsertColumn(3,"标准化的Beta");
	pTable->InsertColumn(4,"T");
	pTable->InsertColumn(5,"Pr(>T)");
	
	CTString strFieldName=m_pDataInterface->GetFieldName(m_nDependIndex);	  //适合二次和三次的回归
	CTString* strVarName = NULL;
	if(Beta.vlen()>0)
	{
		strVarName = new CTString[Beta.vlen()];
	}
	if(strVarName != NULL)
	{
		strVarName[0] = strFieldName; //输出第一行数据
		pTable->InsertItem(0,strVarName[0]);
		pTable->SetItemText(0,1,BetaMatrix(1,0));
		pTable->SetItemText(0,2,serrbeta(1));
		pTable->SetItemText(0,3,Beta(0));
		pTable->SetItemText(0,4,T(1));
	
		double tcd=tcdf(T(1),DF_Rs);
		if(T(1)<=0)
		{
			tcd*=2;
		}
		else
			tcd=(1-tcd)*2;

		pTable->SetItemText(0,5,tcd);
	
	}
	
	if(Beta.vlen()>1) //输出第i到第p-1行数据
	{		
		strFieldName += "**";
		CTString strName = strFieldName;
		CTString strTemp ("");
		
		
		int i=1; 
		for(i=1; i<Beta.vlen(); i++)
		{			
			strTemp.Format("%d",i+1);
			strFieldName += strTemp;
			strVarName[i] = strFieldName;
			strFieldName = strName;
			pTable->InsertItem(i,strVarName[i]);
			pTable->SetItemText(i,1,BetaMatrix(i+1,0));
			pTable->SetItemText(i,2,serrbeta(i+1));
			pTable->SetItemText(i,3,Beta(i));
			pTable->SetItemText(i,4,T(i+1));
			double tcd=tcdf(T(i+1),DF_Rs);
			if(T(i+1)<=0)
			{
				tcd*=2;
			}
			else
				tcd=(1-tcd)*2;
			pTable->SetItemText(i,5,tcd);
		}

		pTable->InsertItem(Beta.vlen(),"常数项"); //输出第p行数据
		pTable->SetItemText(Beta.vlen(),1,BetaMatrix(0,0));
		pTable->SetItemText(Beta.vlen(),2,serrbeta(0));
		pTable->SetItemText(Beta.vlen(),3,"");
		pTable->SetItemText(Beta.vlen(),4,T(0));
		double tcd=tcdf(T(0),DF_Rs);
		if(T(0)<=0)
		{
			tcd*=2;
		}
		else
			tcd=(1-tcd)*2;
		pTable->SetItemText(Beta.vlen(),5,tcd);
		
		delete[] strVarName;
	}

	pETable=new CRsltElementTable("回归系数分析",pTable);
	m_pResult->Add(pETable);
	
}






///////////////////////////////////////////////////////////////////////////////////
//
//					Exponential回归
//
///////////////////////////////////////////////////////////////////////////////////

void CCurveReg::Exponential(int n, double x[], double y[], double t, double w[], double a[])
{
	int i;
	int k=1;  
	
	double xx,yy,lxx,lxy,lyy;
	
	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];

	xx=0.0,yy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			if( y[i]>POSITIVE_INF) 
			{
				xx=xx+w[i]*x[i]; 
				yy=yy+w[i]*log(y[i])/log(t);
			}
			else
			{
				ReportError();
				return;
			}
		}
	}
	xx=xx/W;
	yy=yy/W;

	lxx=0.0;
	lxy=0.0;
	lyy=0.0;
	for (i=0; i<=n-1; i++)
	{ 
		if(w[i]>POSITIVE_INF)
		{
			a[2]=x[i]-xx;
			lxx+=w[i]*a[2]*a[2];
			lxy+=w[i]*a[2]*(log(y[i])-yy);
			lyy+=w[i]*(log(y[i])-yy)*(log(y[i])-yy);
		}
	}
	
	//发生除零情况
	if(fabs(lxx)<POSITIVE_INF || fabs(lyy)<POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	a[6]=lxy/lxx;
	a[5]=yy-a[6]*xx;
	a[1]=a[6]*log(t);
	a[0]=exp(a[5]);

    a[2]=0.0;
	a[4]=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[3]=a[6]*x[i]*log(t); 
			a[2]+=w[i]*(log(y[i])/log(t)-a[5]-a[3])*(log(y[i])/log(t)-a[5]-a[3]);
			a[4]+=w[i]*(x[i])*(x[i]);
		}
	}
	
	//发生除零情况
	if (fabs(a[2])<POSITIVE_INF || fabs(a[4])<POSITIVE_INF)
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}		
	}
	
	double SEB0,SEB1;
	
	//回归系数b的标准误差
	SEB1=sqrt(a[2]/((W-k-1)*lxx));   
	
	//回归系数a的标准误差 
	SEB0=exp(a[5])*sqrt((a[2]*a[4])/(W*(W-k-1)*(lxx)));  
	
	double R,R_S,A_R,SER; 
	
	//复相关系数R
	R=fabs(lxy/sqrt(lxx*lyy));   
	
	//复相关系数R的平方
	R_S=R*R; 
	
	//调整后的复相关系数
	A_R=R_S-(1-R_S)*k/(W-k-1);  
	
	//复相关系数的标准误差
	SER=sqrt(a[2]/(W-k-1));   
	
//	int DF_Rg=k,DF_Rs=W-k-1;
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;


	double SSr,M_S_Rg,M_S_Rs,F;
	
	SSr=(lxy*lxy)/lxx;     //回归平方和
	
	//回归均方
	M_S_Rg=SSr/k;
	
	//残差均方
	M_S_Rs=a[2]/(W-k-1);
	
	//F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=lxy/sqrt(lxx*lyy); 
	
	//T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	FillChartData(n,x,NULL,9,"指数曲线",a[0],a[1]);
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F);
	return ;
}

////////////////////////////////////////////////////////////////////////////
//
//				Power模型
//
////////////////////////////////////////////////////////////////////////////
void CCurveReg::Power(int n, double x[], double y[], double w[], double a[])
{
	int i;
	int k=1; 
	
	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];

	double xx,yy,lxx,lxy,lyy;

	xx=0.0,yy=0.0;
	for(i=0;i<n;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			if(y[i]>POSITIVE_INF && x[i]>POSITIVE_INF) 
			{
				xx+=w[i]*log(x[i]);
				yy+=w[i]*log(y[i]);
			}
			else
			{
				ReportError();
				return;
			}
		}
	}
	xx=xx/W;
	yy=yy/W;
	
	lxx=0.0,lxy=0.0,lyy=0.0;
	for(i=0;i<n;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[2]=log(x[i])-xx;
			lxx+=w[i]*a[2]*a[2];
			lxy+=w[i]*a[2]*(log(y[i])-yy);
			lyy+=w[i]*(log(y[i])-yy)*(log(y[i])-yy);
		}
	}
	
	//发生除零情况
	if(fabs(lxx)<POSITIVE_INF || fabs(lyy)<POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	//回归系数 a
	a[1]=lxy/lxx; a[5]=yy-a[1]*xx;  
    
	//回归系数 b
	a[0]=exp(a[5]);             
	
	a[2]=0.0;a[4]=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[3]=a[1]*log(x[i]);
			a[2]+=w[i]*(log(y[i])-a[5]-a[3])*(log(y[i])-a[5]-a[3]);
			a[4]+=w[i]*log(x[i])*log(x[i]);
		}
	}
	
	double SEB0,SEB1; 
	
	//发生除零情况
	if (fabs(a[2])<POSITIVE_INF || fabs(a[4])<POSITIVE_INF)
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}		
	}
	
	//回归系数b的标准误差 
	SEB1=sqrt(a[2]/((W-k-1)*lxx));  
	
	//回归系数a的标准误差
	SEB0=exp(a[5])*(sqrt((a[2]*a[4])/(W*(W-k-1)*(lxx))));   
	double R,R_S,A_R,SER; 
	
	//复相关系数R
	R=fabs(lxy/sqrt(lxx*lyy));   
	
	
	//复相关系数R的平方
	R_S=R*R; 
	
	//调整后的复相关系数
	A_R=R_S-(1-R_S)*k/(W-k-1);  
	
	//复相关系数的标准误差
	SER=sqrt(a[2]/(W-k-1));   
	
//	int DF_Rg=k,DF_Rs=W-k-1;
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;

	double SSr,M_S_Rg,M_S_Rs,F;
	
	//回归平方和
	SSr=(lxy*lxy)/lxx;     
	
	//回归均方
	M_S_Rg=SSr/k;
	
	//残差均方
	M_S_Rs=a[2]/(W-k-1);
	
    //F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=lxy/sqrt(lxx*lyy); 
	
	//T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	FillChartData(n,x,NULL, 6,"幂曲线",a[0],a[1]);	
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F);
	return ;
	
}



////////////////////////////////////////////////////////////////////////////
//
//				Compound模型
//
////////////////////////////////////////////////////////////////////////////

void CCurveReg::Compound(int n, double x[], double y[], double w[], double a[])
{   
	int i;
	int k=1;  
	
	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];

	double xx,yy,lxx,lxy,lyy;
	
	xx=0.0,yy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			if(y[i]>POSITIVE_INF)
			{
				xx+=w[i]*x[i];
				yy+=w[i]*log(y[i]);
			}
			else
			{
				ReportError();
				return;
			}
		}
	}
	xx=xx/W;
	yy=yy/W;
	
	lxx=0.0; lxy=0.0,lyy=0.0;
	for (i=0; i<=n-1; i++)
	{  
		if(w[i]>POSITIVE_INF)
		{
			a[2]=x[i]-xx;
			lxx+=w[i]*a[2]*a[2];
			lxy+=w[i]*a[2]*(log(y[i])-yy);
			lyy+=w[i]*(log(y[i])-yy)*(log(y[i])-yy);
		}
	}
	
	//发生除零情况
	if(fabs(lxx)<POSITIVE_INF || fabs(lyy)<POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	a[6]=lxy/lxx; a[5]=yy-a[6]*xx;
	
	//a[0]回归系数中的 b
	a[0]=exp(a[5]);   
	
	//a[1]回归系数中的 a
	a[1]=exp(a[6]);    
	
	a[2]=0.0,a[4]=0.0; 
	
	for (i=0; i<=n-1; i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[3]=a[6]*x[i]; 
			a[2]+=w[i]*(log(y[i])-a[5]-a[3])*(log(y[i])-a[5]-a[3]);
			a[4]+=w[i]*(x[i])*(x[i]);
		}
	}
	
	//发生除零情况
	if( fabs(a[2])<POSITIVE_INF || fabs(a[4])<POSITIVE_INF)
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}	
	}
	
	double SEB0,SEB1;
	
	//回归系数b的标准误差
	SEB1=exp(a[6])*sqrt(a[2]/((W-k-1)*lxx));   
	
	//回归系数a的标准误差
	SEB0=exp(a[5])*sqrt((a[2]*a[4])/(W*(W-k-1)*(lxx)));   
	
	double R,R_S,A_R,SER; 
	
	//复相关系数R
	R=fabs(lxy/sqrt(lxx*lyy));  
	
	//复相关系数R的平方
	R_S=R*R; 
	
	//调整后的复相关系数
	A_R=R_S-(1-R_S)*k/(W-k-1);  
	
	//复相关系数的标准误差
	SER=sqrt(a[2]/(W-k-1));  
	
//	int DF_Rg=k,DF_Rs=W-k-1;
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;

	double SSr,M_S_Rg,M_S_Rs,F;
	
	//回归平方和
	SSr=(lxy*lxy)/lxx;    
	
	//回归均方
	M_S_Rg=SSr/k;
	
	//残差均方
	M_S_Rs=a[2]/(W-k-1);
	
	//F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=exp((a[6])*(sqrt(lxx/lyy)));
	
	//T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	FillChartData(n,x,NULL, 5,"复合增长曲线",a[0],a[1]);		
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F);
	return ;
}



//////////////////////////////////////////////////////////////////////
//
//				Growh模型
//
//////////////////////////////////////////////////////////////////////
void CCurveReg::Growth(int n, double x[], double y[], double w[], double a[])
{
	int i;
	int k=1;

	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];
	
	double xx,yy,lxx,lxy,lyy;
	
	xx=0.0,yy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			if(y[i]>POSITIVE_INF)
			{
				xx+=w[i]*x[i];
				yy+=w[i]*log(y[i]);
			}
			else
			{
				ReportError();
				return;
			}
		}
	}
	xx=xx/W;
	yy=yy/W;

	lxx=0.0;lxy=0.0;lyy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[2]=x[i]-xx;
			lxx+=w[i]*a[2]*a[2];
			lxy+=w[i]*a[2]*(log(y[i])-yy);
			lyy+=w[i]*(log(y[i])-yy)*(log(y[i])-yy);
		}
	}
	
	//发生除零情况
	if(fabs(lxx)<POSITIVE_INF || fabs(lyy)<POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	//回归系数 a
	a[1]=lxy/lxx;
	
	//回归系数 b
	a[0]=yy-a[1]*xx;
	
	a[2]=0.0,a[4]=0.0;
	
	for (i=0; i<=n-1; i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[3]=a[1]*x[i]; 
			a[2]+=w[i]*(log(y[i])-a[0]-a[3])*(log(y[i])-a[0]-a[3]);
			a[4]+=w[i]*x[i]*x[i];
		}
	}
	
	//发生除零情况
	if( fabs(a[2])<POSITIVE_INF || fabs(a[4])<POSITIVE_INF)
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}
	}
	
	double SEB0,SEB1;
	
	//回归系数b的标准误差 
	SEB1=sqrt(a[2]/((W-k-1)*lxx)); 
	
	//回归系数a的标准误差 
	SEB0=sqrt((a[2]*a[4])/(W*(W-k-1)*(lxx)));  
	
	
	double R,R_S,A_R,SER; 
	
	//复相关系数R
	R=lxy/sqrt(lxx*lyy);  
	
	//复相关系数R的平方
	R_S=R*R; 
	
	//调整后的复相关系数
	A_R=R_S-(1-R_S)*k/(W-k-1);  
	
	//复相关系数的标准误差
	SER=sqrt(a[2]/(W-k-1));   
	
//	int DF_Rg=k,DF_Rs=W-k-1;
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;

	double SSr,M_S_Rg,M_S_Rs,F;
	
	//回归平方和
	SSr=(lxy*lxy)/lxx;     
	
	//回归均方
	M_S_Rg=SSr/k;
	
	//残差均方
	M_S_Rs=a[2]/(W-k-1);
	
	//F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=lxy/sqrt(lxx*lyy); 
	
	//T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	FillChartData(n,x,NULL, 8,"生长曲线",a[0],a[1]);
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F);
	return ;
}


////////////////////////////////////////////////////////////////////////
//
//				 S模型
//
////////////////////////////////////////////////////////////////////////

void CCurveReg::S(int n, double x[], double y[], double w[], double a[])
{
	int i;
	int k=1;
	
	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];

	double xx,yy,lxx,lxy,lyy;
	
	xx=0.0,yy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			if( y[i]>POSITIVE_INF && fabs(x[i])>POSITIVE_INF ) 
			{
				xx+=w[i]/x[i];
				yy+=w[i]*log(y[i]);
			}
			else
			{
				ReportError();
				return;
			}
		}
	}
	xx=xx/W;
	yy=yy/W;

	lxx=0.0;lxy=0.0;lyy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[2]=1/x[i]-xx;
			lxx+=w[i]*a[2]*a[2];
			lxy+=w[i]*a[2]*(log(y[i])-yy);
			lyy+=w[i]*(log(y[i])-yy)*(log(y[i])-yy);
		}
	}
	
	//发生除零情况
	if(fabs(lxx)<POSITIVE_INF || fabs(lyy)<POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	//a[1]回归系数中的 a 
	a[1]=lxy/lxx;  
	
	//a[0]回归系数中的 b
	a[0]=yy-a[1]*xx;
	
	a[2]=0.0,a[4]=0.0;
	
	for (i=0; i<=n-1; i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[3]=a[1]/x[i]; 
			a[2]+=w[i]*(log(y[i])-a[0]-a[3])*(log(y[i])-a[0]-a[3]);
			a[4]+=w[i]*(1/x[i])*(1/x[i]);
		}
	}
	
	//发生除零情况
	if( fabs(a[2])<POSITIVE_INF || fabs(a[4])<POSITIVE_INF)  
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}
	}
	
	//回归系数的标准误差 
	double SEB0,SEB1;
	
	//回归系数b的标准误差 
	SEB1=sqrt(a[2]/((W-k-1)*lxx)); 
    
	//回归系数a的标准误差 
	SEB0=sqrt((a[2]*a[4])/(W*(W-k-1)*(lxx)));
	
	double R,R_S,A_R,SER; 
	
	//复相关系数R
	R=fabs(lxy/sqrt(lxx*lyy));   
	
	//复相关系数R的平方
	R_S=R*R;
	
	//调整后的复相关系数
	A_R=R_S-(1-R_S)*k/(W-k-1); 
	
	//复相关系数的标准误差
	SER=sqrt(a[2]/(W-k-1));   
	
//	int DF_Rg=k,DF_Rs=W-k-1;
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;

	double SSr,M_S_Rg,M_S_Rs,F,SSe;
	
	//回归平方和
	SSr=(lxy*lxy)/lxx;     
	
	//残差平方和
	SSe=lyy-(lxy*lxy)/lxx; 
	
	//回归均方
	M_S_Rg=SSr/k;
	
	//残差均方
	M_S_Rs=a[2]/(W-k-1);
	
	//F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=lxy/sqrt(lxx*lyy); 
	
	//T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	FillChartData(n,x,NULL, 7,"S曲线",a[0],a[1]);
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F);
	
	return ;
	
}

///////////////////////////////////////////////////////////////////////
//
//					Inverse模型
//
///////////////////////////////////////////////////////////////////////
void CCurveReg::Inverse(int n, double x[], double y[], double w[], double a[])
{
	int i;
	int k=1;

	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];

	double xx,yy,lxx,lxy,lyy;

	xx=0.0,yy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			if( y[i]>POSITIVE_INF && fabs(x[i])>POSITIVE_INF ) 
			{
				xx+=w[i]*(1/(x[i]));
				yy+=w[i]*y[i];
			}
			else
			{
				ReportError();
				return;
			}
		}
	}
	xx=xx/W;
	yy=yy/W;

	lxx=0.0,lxy=0.0,lyy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[2]=1/(x[i])-xx;
			lxx+=w[i]*a[2]*a[2];
			lxy+=w[i]*a[2]*(y[i]-yy);
			lyy+=w[i]*(y[i]-yy)*(y[i]-yy);
		}
		
	}
	
	//发生除零情况
	if(fabs(lxx)<=POSITIVE_INF || fabs(lyy)<=POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	//a[1]回归系数中的 a 
	a[1]=lxy/lxx;        
	
	//a[0]回归系数中的 b       
	a[0]=yy-a[1]*xx;                            
	
	a[2]=0.0;a[4]=0.0;
	
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
			a[3]=a[1]/x[i];

		a[2]+=w[i]*(y[i]-a[0]-a[3])*(y[i]-a[0]-a[3]);

		if(w[i]>POSITIVE_INF)
			a[4]+=w[i]*(1/(x[i]))*(1/(x[i]));

	}
	
	//发生除零情况
	if( fabs(a[2]<POSITIVE_INF) || fabs(a[4]<POSITIVE_INF) )
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}		
	}
	
	double SEB0,SEB1;
	
	//回归系数b的标准误差 
	SEB1=sqrt(a[2]/((W-k-1)*lxx));  
	
	//回归系数a的标准误差 
	SEB0=sqrt((a[2]*a[4])/(W*(W-k-1)*(lxx))); 
	
	
	double R,R_S,A_R,SER; 
	
	//复相关系数R
	R=fabs(lxy/sqrt(lxx*lyy));  
	
	//复相关系数R的平方
	R_S=R*R; 
	
	//调整后的复相关系数
	A_R=R_S-(1-R_S)*k/(W-k-1);  
	
	//复相关系数的标准误差
	SER=sqrt(a[2]/(W-k-1));   
	
//	int DF_Rg=k,DF_Rs=W-k-1;
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;

	//	DF_Rs为零
	if( DF_Rs<=0 )
	{
		ReportError();
		return;
	}

	double SSr,M_S_Rg,M_S_Rs,F;
	
	//回归平方和
	SSr=(lxy*lxy)/lxx;     
	
	//回归均方
	M_S_Rg=SSr/k;
	
	//残差均方
	M_S_Rs=a[2]/(W-k-1);
	
	//F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=lxy/sqrt(lxx*lyy); 
	
	//T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	FillChartData(n,x,NULL,2,"逆曲线",a[0],a[1]);
	
	CreateResult(m_strHead,-R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F);
	return ;
}



//////////////////////////////////////////////////////////////////////////////////
//
//				Logistic模型
//
//////////////////////////////////////////////////////////////////////////////////


void CCurveReg::Logistic(int n, double x[], double y[], double w[], double a[], double &u)
{

//	int p=3;

	int i;
	int k=1; 
	
	///////////////////////////////zfm 2003-3-10
	if(fabs(u)<=1e-100)   //u如果传入为0，则表明没有上限，取无限大
	{
		u=1e308;

	}
	///////////////////////////////
	

	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];

	double xx,yy,lxx,lxy,lyy;

	if(u<POSITIVE_INF)
	{
		ReportError();
		return;
	}
	xx=0.0,yy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			if(x[i]>POSITIVE_INF && y[i]<u )
			{
				//xx+=w[i]*(1/(x[i])); 
				xx+=w[i]*(x[i]); //zfm
				yy+=w[i]*log(1/y[i]-1/u);
			}
			else
			{
				ReportError();
				return;
			}
		}
	}
	xx=xx/W;
	yy=yy/W;

	lxx=0.0; lxy=0.0,lyy=0.0;
	for (i=0; i<=n-1; i++)
	{  
		if(w[i]>POSITIVE_INF)
		{
			a[2]=x[i]-xx; 
			lxx+=w[i]*a[2]*a[2];
			lxy+=w[i]*a[2]*(log(1/y[i]-1/u)-yy);
			lyy+=w[i]*(log(1/y[i]-1/u)-yy)*(log(1/y[i]-1/u)-yy);
		}
	}
	
	//发生除零情况
	if(fabs(lxx)<POSITIVE_INF || fabs(lyy)<POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	a[6]=lxy/lxx; a[5]=yy-a[6]*xx;
	
	//a[0]回归系数中的 b
	a[0]=exp(a[5]);    
	
	//a[1]回归系数中的 a
	a[1]=exp(a[6]);    
	
	a[2]=0.0,a[4]=0.0;
	
	for (i=0; i<=n-1; i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[3]=a[6]*x[i]; 
			a[2]+=w[i]*(log(1/y[i]-1/u)-a[5]-a[3])*(log(1/y[i]-1/u)-a[5]-a[3]);
			a[4]+=w[i]*x[i]*x[i];
		}
	}
	
	//发生除零情况
	if (fabs(a[2])<POSITIVE_INF || fabs(a[4])<POSITIVE_INF)
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}
	}
	
	double SEB0,SEB1;
	
	//回归系数b的标准误差 
	SEB1=exp(a[6])*sqrt(a[2]/((W-k-1)*lxx));  
	
	//回归系数a的标准误差 
	SEB0=exp(a[5])*sqrt((a[2]*a[4])/(W*(W-k-1)*lxx));  
	
	double R,R_S,A_R,SER; 
	
	//复相关系数R
	R=fabs(lxy/sqrt(lxx*lyy));   
	
	//复相关系数R的平方
	R_S=R*R; 
	
	//调整后的复相关系数
	A_R=R_S-(1-R_S)*k/(W-k-1);  
	
	//复相关系数的标准误差
	SER=sqrt(a[2]/(W-k-1));   
	
//	int DF_Rg=k,DF_Rs=W-k-1;
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;

	double SSr,M_S_Rg,M_S_Rs,F;
	
	//回归平方和
	SSr=(lxy*lxy)/lxx;     
	
	//回归均方
	M_S_Rg=SSr/k;
	
	//残差均方
	M_S_Rs=a[2]/(W-k-1);
	
	//F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=exp((a[6])*(sqrt(lxx/lyy)));
	
	//T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	FillChartData(n,x,NULL, 10,"罗吉斯蒂克曲线",a[0],a[1]);
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F,true);
	return ; 
	
}



//////////////////////////////////////////////////////////////////////////
//
//						Logarithmic模型
//
//////////////////////////////////////////////////////////////////////////

void CCurveReg::Logarithmic(int n, double x[], double y[], double w[], double a[])
{ 
	int i;
	int k=1; 
	
	double W=.0;//权值之和
	for (i=0; i<=n-1; i++)
		W+=w[i];

	double xx,yy,lxx,lxy,lyy;
	
	xx=0.0,yy=0.0;
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			if(x[i]>POSITIVE_INF)
			{
				xx+=w[i]*log(x[i]);
				yy+=w[i]*y[i];
			}
			else
			{
				ReportError();
				return;
			}
		}
	}
	xx=xx/W;
	yy=yy/W;

	lxx=0.0; lxy=0.0,lyy=0.0;
	for (i=0; i<=n-1; i++)
	{  
		if(w[i]>POSITIVE_INF)
		{
			a[2]=log(x[i])-xx;
			lxx+=w[i]*a[2]*a[2];
			lxy+=w[i]*a[2]*(y[i]-yy);
			lyy+=w[i]*(y[i]-yy)*(y[i]-yy);
		}
	}
	
	//发生除零情况
	if(fabs(lxx)<=POSITIVE_INF || fabs(lyy)<=POSITIVE_INF)
	{
		ReportError();
		return;
	}
	
	//a[1]回归系数中的 a
	a[1]=lxy/lxx;     
	
	//a[0]回归系数中的 b  
	a[0]=yy-a[1]*xx;      
	
	a[2]=0.0;a[4]=0.0;
	
	for(i=0;i<=n-1;i++)
	{
		if(w[i]>POSITIVE_INF)
		{
			a[3]=a[1]*log(x[i]);
			a[2]+=w[i]*(y[i]-a[0]-a[3])*(y[i]-a[0]-a[3]);
			a[4]+=w[i]*(log(x[i]))*(log(x[i]));
		}
	}
	
	//发生除零情况
	if( fabs(a[2])<POSITIVE_INF || fabs(a[4])<POSITIVE_INF)
	{
		//a[2]用于F检验中的分母，若a[2]为0，说明完全拟合
		if(fabs(a[2])<POSITIVE_INF)
			a[2] = 1e-16;
		else
		{
			ReportError();
			return;
		}
	}
	
	double SEB0,SEB1;
	
	//回归系数b的标准误差
	SEB1=sqrt(a[2]/((W-k-1)*lxx));   
	
	//回归系数a的标准误差 
	SEB0=sqrt((a[2]*a[4])/(W*(W-k-1)*(lxx))); 
	
	double R,R_S,A_R,SER; 
	
	//复相关系数R
	R=fabs(lxy/sqrt(lxx*lyy));   
	
	//复相关系数R的平方
	R_S=R*R; 
	
	//调整后的复相关系数
	A_R=R_S-(1-R_S)*k/(W-k-1);  
	
	//复相关系数的标准误差
	SER=sqrt(a[2]/(W-k-1));   
	
//	int DF_Rg=k,DF_Rs=W-k-1;
	int DF_Rg=k, DF_Rs=(int)(W+0.5)-k-1;

	double SSr,M_S_Rg,M_S_Rs,F;
	
	//回归平方和
	SSr=(lxy*lxy)/lxx;    
	
	//回归均方
	M_S_Rg=SSr/k;
	
	//残差均方
	M_S_Rs=a[2]/(W-k-1);
	
	//F检验
	F=SSr*(W-k-1)/(a[2]*k);
	
	double Beta,T0,T1;
	
	//标准化回归系数
	Beta=lxy/sqrt(lxx*lyy); 
	
	//T检验
	T0=a[1]/SEB1;
	T1=a[0]/SEB0;
	FillChartData(n,x,NULL,1,"对数曲线",a[0],a[1]);	
	CreateResult(m_strHead,R,R_S,A_R,SER,DF_Rg,DF_Rs,SSr,a[2],M_S_Rg,M_S_Rs,a[1],a[0],SEB1,SEB0,Beta,T1,T0,F);
	return ;
}

//	陈思多，01/14/03
//	重载原父类的CAlgorithmBase::GetDataSingle()，在这里增加了权重处理
//	在所有用到的数据中，如果发现有缺失的变量，则将权重设置为零
double* CCurveReg::GetDataSingle(CDataInterface* pDataInterface, int nIndex, double * MissingValue)
{
	TDataSet * pDataArray=NULL;
	if(nIndex>=0)
	{		
		int nCaseNumb = pDataInterface->GetColData(nIndex,pDataArray,5);//取回数据，注意参数4
		
		if(nCaseNumb > 0)
		{
			double * pData = new double [nCaseNumb];
			//if(!CDataInterface::IsEqual(pDataArray,nCaseNumb))
			{				
//				for(int j=0;j<nCaseNumb;j++)
		
				//	陈思多，01/14/03
				int n = pDataInterface->GetRowCount();
				for(int j=0;j<n;j++)
				{
					//	如果出现空值
					if(pDataArray[j].iIndex<0)
					{

						m_bIsWeighted=true;
						m_pdWeights[j]=0;
					}
					pData[j] = pDataArray[j].dValue;
				}
				delete [] pDataArray;
				return pData;
			}
			//else
			{
			//	delete [] pData;pData = NULL;
			//	return NULL;		
			}
		}
		else
			return NULL;
	}	
	else 
		return NULL;
}

double* CCurveReg::GetDataSingle(CDataInterface* pDataInterface, int nIndex, CIntVector &Vect_Avail)
{
	TDataSet * pDataArray=NULL;
	if(nIndex>=0)
	{	
		int nCase = Vect_Avail.sum();
		int nCaseNumb = pDataInterface->GetColData(nIndex,pDataArray,5);//取回数据，注意参数4
		
		if(nCaseNumb > 0)
		{
			double * pData = new double [nCase];
			int nCurrent = 0;
			//if(!CDataInterface::IsEqual(pDataArray,nCaseNumb))
			{				
				//				for(int j=0;j<nCaseNumb;j++)
				
				//	陈思多，01/14/03
				int n = pDataInterface->GetRowCount();
				for(int j=0;j<n;j++)
				{
					//	如果出现空值
					if(Vect_Avail[j]==1)
					{
						pData[nCurrent] = pDataArray[j].dValue;
						nCurrent++;
					}
				}
				delete [] pDataArray;
				return pData;
			}
			//else
			{
				//	delete [] pData;pData = NULL;
				//	return NULL;		
			}
		}
		else
			return NULL;
	}	
	else 
		return NULL;
}

void CCurveReg::sort(double x[],double y[],double w[],int nLen)
{
    int k=0,j=0,i=0;
	int n=nLen;
    double tX=0,tY=0,tW;
	k=n/2;
	while (k>0)
	{ 
		for (j=k; j<=n-1; j++)
		{
			tX=x[j];
			tY=y[j];
			tW=w[j];
			i=j-k;
			while ((i>=0)&&(x[i]>tX))
			{
				x[i+k]=x[i];
				y[i+k]=y[i];
				w[i+k]=w[i];
				i=i-k;
			}
			x[i+k]=tX;
			y[i+k]=tY;
			w[i+k]=tW;
		}
		k=k/2;
	}
}

void CCurveReg::CreateResult(CTString strHead,double r,double rs,double ars,double se,int df1,int df2,double ss1,double ss2,
							 double ms1,double ms2,double b1,double b0,double sb1,double sb0,double sbeta,double t1,double t0 ,double F, bool bLogistic)
{
	strHead += "\n";
	strHead += m_strModel;
	strHead += "\n"; //zfm
	CTLTable * pTable = new CTLTable();
//	pTable->SetRows(5);
//	pTable->SetCols(2); 

	pTable->SetTitle("模型分析");
	pTable->SetHead(strHead);
	pTable->InsertColumn(0,"指标名");
	pTable->InsertColumn(1,"数值");
	pTable->InsertItem(0,"复相关系数");
	pTable->InsertItem(1,"样本决定系数");
	pTable->InsertItem(2,"修正的决定系数");
	pTable->InsertItem(3,"标准误");
	

	pTable->SetItemText(0,1,r);
	pTable->SetItemText(1,1,rs);
	pTable->SetItemText(2,1,ars);
	pTable->SetItemText(3,1,se);
	
	CRsltElementTable * pETable= new CRsltElementTable("模型分析",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	
	//brief add y liytao 20030813
	if(briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,m_strModel);

	CTString strInfl="",strVars="自变量";
	strVars+="对因变量";
	strInfl.Format("变异的影响为%f",rs*100);
	
	if(briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型中"+strVars+strInfl.GetData()+"%");
	//add end

	pTable = new CTLTable();
	pTable->SetTitle("方差分析");
	
//	pTable->SetRows(3);
//	pTable->SetCols(4);

	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"自由度");
	pTable->InsertColumn(2,"平方和");
	pTable->InsertColumn(3,"均方");
	pTable->InsertColumn(4,"F值");
	pTable->InsertColumn(5,"Pr(>F)");
	
	pTable->InsertItem(0,"回归");
	pTable->InsertItem(1,"残差");
	
	pTable->SetItemText(0,1,df1);
	pTable->SetItemText(1,1,df2);
	pTable->SetItemText(0,2,ss1);
	pTable->SetItemText(1,2,ss2);
	pTable->SetItemText(0,3,ms1);
	pTable->SetItemText(1,3,ms2);
	pTable->SetItemText(0,4,F);	
	double FSig = 1.0 - fcdf(F,df1,df2);
	pTable->SetItemText(0,5,FSig);

	
	pETable= new CRsltElementTable("方差分析",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);


	//add by liytao 20030813
	CTString strVia;
	strVars="自变量";
	strVars+="对因变量";
	CTString strVal;
	strVal.Format("(F 值=%f,显著性=%f)",F,FSig); 
	if(FSig<=0.05)
		strVia="产生的影响较大";
	else
		strVia="产生的影响较小";
	if(briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型中"+strVars+strVia.GetData()+strVal.GetData());
	//add end
	


	pTable = new CTLTable;
	pTable->SetTitle("回归系数分析");
	
//	pTable->SetRows(3);
//	pTable->SetCols(5);

	pTable->InsertColumn(0,""); //zfm
	pTable->InsertColumn(1,"系数");
	pTable->InsertColumn(2,"标准误");
	pTable->InsertColumn(3,"标准化的Beta");
	pTable->InsertColumn(4,"T");
	pTable->InsertColumn(5,"Pr(>T)");

	CTString temp; //输出 上限用 
	if (fabs(atof(m_strUpper))<=1e-100)//zfm
	{
		temp=" ";
	}
	else 
		temp=m_strUpper;
	
	pTable->InsertItem(0,m_pDataInterface->GetFieldName(m_nDependIndex));
	pTable->InsertItem(1,"常数项");
	pTable->SetItemText(0,1,b1);
	pTable->SetItemText(1,1,b0);
	pTable->SetItemText(0,2,sb1);
	pTable->SetItemText(1,2,sb0);
	pTable->SetItemText(0,3,sbeta);
	pTable->SetItemText(0,4,t0);
	pTable->SetItemText(1,4,t1);
	if(bLogistic)
	{
		pTable->InsertItem(2,"上界值");//zfm
		pTable->SetItemText(2,1,temp); //zfm
	}
	
	double tsig1 = (1.0 - tcdf(fabs(t1),df2))*2;
	double tsig0 = (1.0 - tcdf(fabs(t0),df2))*2;
	pTable->SetItemText(0,5,tsig0);
	pTable->SetItemText(1,5,tsig1);
	
	pETable= new CRsltElementTable("回归系数分析",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);

	//add by liytao 
	CTString strSig;
	if (tsig0<=0.05)
	{
		strSig.Format("自变量%s对因变量的影响较大(概率检验值=%f)",m_pDataInterface->GetFieldName(m_nDependIndex).GetData(),tsig0);

	}
	else
	{
		strSig.Format("自变量%s对因变量的影响较小(概率检验值=%f)",m_pDataInterface->GetFieldName(m_nDependIndex).GetData(),tsig0);

	}
	if(briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strSig);
	if (tsig1<=0.05)
	{
		strSig.Format("%s对因变量的影响较大(概率检验值=%f)","常数项",tsig1);

	}
	else
	{
		strSig.Format("%s对因变量的影响较小(概率检验值=%f)","常数项",tsig1);

	}
	if(briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strSig);
	//add end
}


void CCurveReg::ReportError()
{
	CTString Message;

	Message.Format("因输入数据不符合要求，回归分析无法完成计算");

	CTLTable * pTable = NULL;

	pTable = new CTLTable;
	pTable->SetTitle("回归系数分析");
	pTable->SetHead(m_strModel + "    " + m_strHead.GetData()  );
//	pTable->SetCols( 1 );
//	pTable->SetRows( 0 );

	CTString strError;
	strError = Message; // here for error report message
	pTable->InsertColumn( 0, strError);

	CRsltElementTable * pETable= new CRsltElementTable("注意",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
}

BOOL CCurveReg::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//	CURVEREG
//		/DEPENDENT [varlist]
//		/INDEPENDENT [varlist]
//		/MODEL=[LINEAR**] [LOGARITHMIC] [INVERSE] [QUADRATIC] [CUBIC] [COMPOUND] [POWER] [S] 
//		[GROUTH] [EXPONENTIAL] [LGSTIC(0**)]
	
	AnaWord VOstr;
	VOstr.Import(VO);
	m_bLineare = AnaSyntax::findstr("/MODEL","LINEAR",VOstr);
	m_bLogarithmic = AnaSyntax::findstr("/MODEL","LOGARITHMIC",VOstr);
	m_bInverse = AnaSyntax::findstr("/MODEL","INVERSE",VOstr);
	m_bQuad = AnaSyntax::findstr("/MODEL","QUADRATIC",VOstr);
	m_bCubic = AnaSyntax::findstr("/MODEL","CUBIC",VOstr);
	m_bCompound = AnaSyntax::findstr("/MODEL","COMPOUND",VOstr);
	m_bPower = AnaSyntax::findstr("/MODEL","POWER",VOstr);
	m_bS = AnaSyntax::findstr("/MODEL","S",VOstr);
	m_bGrowth = AnaSyntax::findstr("/MODEL","GROUTH",VOstr);
	m_bExponential = AnaSyntax::findstr("/MODEL","EXPONENTIAL",VOstr);
	m_bLogistic = AnaSyntax::findstr("/MODEL","LGSTIC",VOstr);
	if (m_bLogistic) 
	{
		m_dUpper = AnaSyntax::getvalue("/MODEL","LGSTIC",VOstr);
		if (m_dUpper < 0)
			m_dUpper = 0;
	}
	CTStringArray ynames;
	AnaSyntax::getVAR("/DEPENDENT", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return false;
	CTString yname = ynames.GetAt(0);
	m_nDependIndex = pDataInterface->IndexByName(yname);
	CTStringArray xname;
	AnaSyntax::getVAR("/INDEPENDENT", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return false;
	return true;
}

CResult * CCurveReg::OnCurveReg(CDataInterface *pDataInterface, string VO)
{

	m_pDataInterface = pDataInterface;
	briefrTable=NULL;
	m_pResult = NULL;
	m_pResult = new CResult("曲线回归");
	if (!(GetValueFromVo(pDataInterface, VO)))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
	if (nOutNote)
	{										
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"结果说明");
	}
	int n = pDataInterface->GetRowCount();
	CDWordArray colArray;
	colArray.Add(m_nDependIndex);
	int ii=0;
	for (ii=0;ii<m_tVarInfo.iCount;ii++)
		colArray.Add(m_tVarInfo.pValue[ii]);
	m_VectAvail = CheckAvailData(pDataInterface,colArray);
	int nRow = m_VectAvail.sum();
	//	陈思多，01/13/03
	/*	如果在非加权的情况下，若有缺失值，则在权重中记录缺失值
	并将非加权算法转换为加权算法	*/
//		m_pdWeights=new double[n];
	m_pdWeights=new double[nRow];
//		for(int i=0; i<n; i++)
	int i=0; 
	for(i=0; i<nRow; i++)
		m_pdWeights[i]=1;
	
	//	陈思多，01/13/03
	//	是否对数据进行了加权
	m_bIsWeighted=true;
	if(pDataInterface->GetWeightNumb()!=-1)
	{			
		pDataInterface->GetWeightData(m_pdWeights);
	}

	//		double * pDataX = GetDataSingle(pDataInterface,m_nDependIndex);
	if( pDataInterface->IsColEmpty(m_nDependIndex) )
	{
		CTString	strWarning;
		strWarning.Format( " 注：自变量整列缺失，无法执行相关分析！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		
		delete[] m_pdWeights;
		m_pdWeights = NULL;
		
		return m_pResult;
	}
	
	double * pDataX = GetDataSingle(pDataInterface,m_nDependIndex, m_VectAvail);//陈思多，01/14/03
	
	double a[10];
	for(i=0;i<m_tVarInfo.iCount;i++)
	{
		int nIndex = m_tVarInfo.pValue[i];
		m_nCurVarIndex = nIndex;
		CTString strFields;
		strFields.Format("自变量%s--因变量%s",m_pDataInterface->GetFieldName(m_nDependIndex).GetData(),m_pDataInterface->GetFieldName(nIndex).GetData());
		if( pDataInterface->IsColEmpty(nIndex) )
		{
			CTString	strWarning;
			strWarning.Format( " 注：因变量 %s 整列缺失，无法执行相关分析！", pDataInterface->GetFieldName(nIndex).GetData() );
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			
			//delete[] m_pdWeights;
			//m_pdWeights = NULL;
			//delete[] pDataX;
			//return m_pResult;
			continue;
		}
		
		//			double * pDataY = GetDataSingle(pDataInterface,nIndex);
		double * pDataY = GetDataSingle(pDataInterface,nIndex, m_VectAvail);//陈思多，01/14/03
		
		m_strHead.Format("自变量：%s,   因变量：%s",pDataInterface->GetFieldName(m_nDependIndex).GetData(),pDataInterface->GetFieldName(nIndex).GetData());
		CRsltElementText * pResultVariable =new CRsltElementText(m_strHead);
		m_pResult->Add(pResultVariable);
		
		
		CTString strText="";
		strText.Format("说明%d(%s):",i+1,m_strHead.GetData());
		if(briefrTable)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strText);
		sort(pDataX,pDataY,m_pdWeights,nRow);
		FillChartData(nRow,pDataX,pDataY,12,"原始数据",0,0);
		if( m_bCompound )
		{
			m_strModel = "模  型： 复合增长曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Compound(nRow,pDataX,pDataY, m_pdWeights, a);
		}		
		if( m_bExponential )
		{
			m_strModel = "模  型： 指数曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Exponential(nRow,pDataX,pDataY,exp(1.0), m_pdWeights, a);
		}
		if( m_bGrowth )
		{
			m_strModel = "模  型： 生长曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Growth(nRow,pDataX,pDataY, m_pdWeights, a);
		}
		if( m_bInverse )
		{
			m_strModel = "模  型： 逆曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Inverse(nRow,pDataX,pDataY,m_pdWeights,a);
		}
		if( m_bLineare )
		{
			m_strModel = "模  型： 线性回归";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Linear(nRow,pDataX,pDataY, m_pdWeights, a);
		}
		if( m_bLogarithmic )
		{
			m_strModel = "模  型： 对数曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Logarithmic(nRow,pDataX,pDataY,m_pdWeights,a);
		}
		if( m_bLogistic )
		{
			m_strModel = "模  型： 罗吉斯蒂克曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			m_u=m_dUpper;
			Logistic(nRow,pDataX,pDataY,m_pdWeights,a,m_u);
		}
		if( m_bPower )
		{ 
			m_strModel = "模  型： 幂曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Power(nRow,pDataX,pDataY,m_pdWeights,a);
		}
		if( m_bS )
		{
			m_strModel = "模  型： S曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			S(nRow,pDataX,pDataY,m_pdWeights,a);
		}
		///////////////////////////////////////////////zfm 2003-3-11
		if( m_bQuad)
		{
			m_strModel="模  型： 二次曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Quad_Cubic(nRow,2,pDataX,pDataY,m_pdWeights);
			
		}
		if(m_bCubic)
		{
			m_strModel="模  型： 三次曲线";
			strModel=m_strModel;
			pResultVariable->AddString(strModel);
			Quad_Cubic(nRow,3,pDataX,pDataY,m_pdWeights);
		}
		CreateChartResult(pDataX,strFields,m_pResult,nRow);
		///////////////////////////////////////////////
		delete [] pDataY;pDataY = NULL;
		
	}//for(i=0;i<m_tVarInfo.iCount;i++)
	
	//add by liytao 20030813
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
		if (m_pResult->Count()<=0)
			m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
		else
			m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete briefrTable;
	
	//add end
	
	
	//	陈思多，01/13/03
	if(m_pdWeights!=NULL)
	{
		delete[] m_pdWeights;
		m_pdWeights=NULL;
	}
	if(pDataX!=NULL)
	{
		delete [] pDataX;
		pDataX=NULL;
	}
	return m_pResult;
}
