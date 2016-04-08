// Arima.cpp: implementation of the CArima class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Arima.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArima::CArima()
{
	m_pAR  = 1;
	m_qMA  = 0;
	m_dIFF = 0;
	m_bWithConst = false;
}

CArima::~CArima()
{

}

BOOL CArima::GetValueFromVO(CDataInterface *pDataInterface, string VO)
{
	//		ARIMA
	//			/DATA=[file]
	//			/VARLIST [varlist]
	//			/OPTION [PARAMETER(1 0 0)] [NOCONST] [PREDSTEP(1)]
	
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	
	m_pAR = AnaSyntax::getvalue("/OPTION", "PARAMETER", VOstr);
	m_dIFF = AnaSyntax::getvalue("/OPTION", "PARAMETER", VOstr, 2);
	m_qMA = AnaSyntax::getvalue("/OPTION", "PARAMETER", VOstr, 3);
	m_nPredStep = AnaSyntax::getvalue("/OPTION", "PREDSTEP", VOstr);
	if (m_nPredStep < 0)
		m_nPredStep = 0;
	if (m_pAR < 0 || m_qMA < 0 || m_dIFF < 0)
		return FALSE;
	m_bWithConst = !AnaSyntax::findstr("/OPTION", "NOCONST", VOstr);
	
	return TRUE;
}

CResult* CArima::OnArima(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	m_pResult = new CResult("ARIMA模型结果");
	m_pWarningTextRslt = new CRsltElementText("错误");
	
	if (!GetValueFromVO(pDataInterface, VO))
	{
		m_pWarningTextRslt->AddString("读取参数发生错误，请重新运行！");
		m_pResult->Add(m_pWarningTextRslt);
		return m_pResult;
	}
	
	CDWordArray arCol;//读取数据
	int i=0;
	for (i=0; i<m_tVarInfo.iCount; i++)
		arCol.Add(m_tVarInfo.pValue[i]);
	CDoubleMatrix Data_read;
	m_nRow = m_pDataInterface->GetColsData(arCol, Data_read, 0);
	
	
	if (m_nRow != m_pDataInterface->GetRowCount() || m_nRow <6)
	{
		m_pWarningTextRslt->AddString( "数据存在缺失值，或是少于6个，不适合ARIMA模型！" );
		m_pResult->Add(m_pWarningTextRslt);
		return m_pResult;
	}

	if (m_nRow < (m_pAR+m_qMA+m_dIFF)*2 )
	{
		m_pWarningTextRslt->AddString( "参数p、d、q设置不合理，不适合ARIMA模型，请重新设置参数！" );
		m_pResult->Add(m_pWarningTextRslt);
		return m_pResult;
	}
	
	for (i=0; i<m_tVarInfo.iCount; i++)
	{
		CDoubleVector OrgVec = Data_read(i);
		ArimaPred(OrgVec, m_pAR, m_qMA, m_dIFF, m_nPredStep, m_bWithConst);//m_pAR, m_qMA, m_dIFF, m_nPredStep
	//////////////*****计算输出统计量R平方等*******/////
    	GetArimaStatis(m_diff_Vec, m_ep_Vector,m_Coef_vec);
	/////**********输出图表*************/////////
    	DrawResult();		
	
	}
	
	return m_pResult;
}

CDoubleVector CArima::ArimaPred(CDoubleVector &OrgVec, int pAR, int qMA, int dIFF, int PredStep, BOOL m_bWithConst)
{
	CDoubleVector OutputVec(PredStep);

	m_pAR = pAR;
	m_qMA = qMA;
	m_nPredStep = PredStep;	
	m_nRow = OrgVec.vlen();
    m_VecRow = m_nRow - dIFF;
	m_diff_Vec.create(m_VecRow,0.0);             
	int i = 0;
	
	////////////***********序列差分 始****************//////////////
	if (dIFF==0)
	{
		if (m_bWithConst)
		{
		     double orgmean = OrgVec.mean();
		     m_diff_Vec = OrgVec - orgmean;
		}
		else
		{
            m_diff_Vec = OrgVec.copy();
		}	
	}
	else if (dIFF>0 && dIFF<13)
	{
		for (i=0; i<m_VecRow; i++)
			for (int j=0; j<dIFF+1; j++)
				m_diff_Vec(i) +=( (int(j/2)*2 == j) ? (1) : (-1))*Factorial(dIFF)*OrgVec(i+dIFF-j)/(Factorial(j)*Factorial(dIFF-j));
	}
	else
	{
		m_pWarningTextRslt->AddString("滞后阶数过大，请重新设置参数！");
	}
	m_orginal_Vec = OrgVec.copy();
	
	//////*************计算样本的协方差函数\自相关函数\偏自相关函数********************//
    int k = 0;
 	int AutoNum = min(m_VecRow, 16);
	double temp = m_diff_Vec.mean(); 
    m_AutoCovF_Vec.create(AutoNum, 0.0);                    
	m_AutoCorrF_Vec.create(AutoNum, 0.0);                 	                        
 	m_diff_Vec = m_diff_Vec - temp ;                                    
	for (k=0; k<AutoNum; k++)
	{		
		double sumTemp = 0;
		for (int t=k; t<m_VecRow; t++) 
			sumTemp += m_diff_Vec(t) * m_diff_Vec(t-k);
		m_AutoCovF_Vec(k) = sumTemp / m_VecRow;		
	}
	m_AutoCorrF_Vec = m_AutoCovF_Vec / m_AutoCovF_Vec(0);
	m_diff_Vec = m_diff_Vec + temp;


	//////**********偏自相关函数************//////////
    CDoubleVector m_ParAutocorrF_Vec(AutoNum, 1.0);	
	for ( k=1; k<AutoNum; k++)
	{
		int i = 0 ;
		int j = 0;
		CDoubleMatrix corrmatrix1(k,k, 1.0);
		for (i = 0; i<k; i++)
		{
			for (j = 0;j<k; j++)
			{
				if (i==j)
					corrmatrix1(i,j) = m_AutoCorrF_Vec(0);
				else if (i<j)				
					corrmatrix1(i,j) = m_AutoCorrF_Vec(j-i);				
				else
					corrmatrix1(i,j) = m_AutoCorrF_Vec(i-j);		
			}
		}
	    CDoubleMatrix corrmatrix2 = corrmatrix1.copy(); 
	    for (int n=0; n<k; n++)
			corrmatrix2(n, k-1) = m_AutoCorrF_Vec(n+1);	   
	    m_ParAutocorrF_Vec(k) = corrmatrix2.det()/corrmatrix1.det();	
	}	

    ////////***********精估计************///////////
	int dimension = m_pAR+m_qMA;
	CDoubleVector X_Vec(dimension, 0);
	CDoubleMatrix E_mx(dimension, dimension, 0.5);
	for (i=0; i<dimension; i++)
	{
		E_mx(i,i) = 1.0;
	}
	int iter = 0;
	double FERT = 0.0;
	CDoubleVector betawithconst_Vec(m_pAR+m_qMA+1,0.0);
	if (!m_bWithConst)
	{
		Powell(X_Vec,E_mx,iter,FERT);
	} 
    else
	{   
		Powell(X_Vec,E_mx,iter,FERT);
		double constant = 0;
		for (i=0; i<m_pAR; i++)		
		{
			constant = temp;	
		}	
		betawithconst_Vec(0) = constant;
		for (i=1; i<m_pAR+m_qMA+1; i++)
		{
			betawithconst_Vec(i) = X_Vec(i-1);
		}
		 X_Vec = betawithconst_Vec.copy();
	}
	m_Coef_vec = X_Vec;
	
   /////////*********预测*************///////////

	CDoubleVector Predict_Vec1 = Getpredict(m_diff_Vec, X_Vec,m_pAR, m_qMA, m_nPredStep, m_bWithConst);
	CDoubleVector Predict_Vec = Predict(OrgVec, Predict_Vec1, dIFF, m_bWithConst);
    m_predict_Vec = Predict_Vec.copy();

	OutputVec = Predict_Vec.copy();
	return OutputVec;
}


/////******阶乘函数*******///////
int CArima::Factorial(int number)
{
	int ntemp = 1;
	for (int i=1; i<number+1; i++)
		ntemp = ntemp*i;
	
	return ntemp;
}

///////******求逆函数*******///////

CDoubleVector CArima::GetInverseFunc(CDoubleVector Vec_1, CDoubleVector Vec_2, int numW)//求逆函数
{
	CDoubleVector Inverse_Vec = Vec_1 - Vec_2;
	for (int i=0; i<numW; i++)
	{
		for (int j=0; j<i; j++)		
			Inverse_Vec(i) += Vec_2(j) * Inverse_Vec(i-1-j);		
	}
	return Inverse_Vec;
}

///////******自回归系数估计*******///////

BOOL CArima::GetARbeta(int numP, int numQ)   
{
	int i = 0 ;
	CDoubleMatrix corrmatrix_mx(numP, numP, 1.0);
    m_ARvariance_Vec.create(numP, 0.0);
	for (i=0; i<numP; i++)
	{
		for (int j=0;j<numP; j++)
		{
			
			if (i==j)
				corrmatrix_mx(i,j) = m_AutoCorrF_Vec(numQ+i-j);
			else 
			{
				if (i < j)
					corrmatrix_mx(i,j) = m_AutoCorrF_Vec(numQ+j-i);
				else
					corrmatrix_mx(i,j) = m_AutoCorrF_Vec(numQ+i-j);
			}
		}
	}
	CDoubleVector corrvec_Vec(numP, 1.0);
	for (i=0; i<numP; i++)
		corrvec_Vec(i) = m_AutoCorrF_Vec(numQ+1+i);
	CDoubleVector RegCoeffAR_Vec = (corrmatrix_mx.inv()) * corrvec_Vec;
    m_ARCoefficient_Vec = RegCoeffAR_Vec;
	double temp = m_AutoCovF_Vec(0);
	for (i=0; i<numP; i++)
	{
		temp -=  m_AutoCovF_Vec(0) * m_ARCoefficient_Vec(i) * m_AutoCorrF_Vec(i+1);
	}
	m_ARvariance_Vec(numP-1) = temp;
	return TRUE;
}

/////******预测转化函数*******///////

CDoubleVector CArima::Predict(CDoubleVector &Xmin_Vec, CDoubleVector &Z_Vec, int d, BOOL  m_bWithConst)///求预测
{
	int Num = Xmin_Vec.vlen();
	int NumPre = Z_Vec.vlen();
	CDoubleVector Y_Vec(Num+NumPre);
	int i = 0;
	for(i=0; i<Num; i++)
		Y_Vec(i)=Xmin_Vec(i);
	for(i=0;  i<NumPre; i++)
		Y_Vec(Num+i) = Z_Vec(i);
	double mu = m_orginal_Vec.mean();
	for(i=Num; i<Num+NumPre; i++)
	{
		if (m_dIFF==0 && m_bWithConst)
		{
			Y_Vec(i) = Y_Vec(i) + mu;
		}
	
		else
		{
			for(int j=0; j<d; j++)
			{
				double ntemp = Getcni(d,j+1);
				Y_Vec(i) = Y_Vec(i) - ntemp * Y_Vec(i-j-1);
			}
		}
	
	}
	WriteData(Y_Vec,"bbb");
	CDoubleVector temp_Vec(NumPre);
	for(i=0; i<NumPre; i++)
		temp_Vec(i) = Y_Vec(Num+i);
	return temp_Vec;
}

/////******排列组合函数*******///////

double CArima::Getcni(int n,int i) 
{
	if(i>n||n<0||i<0)
		return 0;
	double flag = i%2==0?1:-1;
    flag = flag * Factorial(n) / Factorial(n-i) / Factorial(i);
	return flag;
}


/////******求残差和预测函数*******///////

CDoubleVector CArima::Getpredict(CDoubleVector &Org_Vec, CDoubleVector &CofficVec, int p, int q,int nPredStep, BOOL m_bWithConst)
{
	int i = 0, j = 0;
	int nLength = Org_Vec.vlen();
	int c = 1;
	double mu = 0;
	CDoubleVector X_Vec = Org_Vec.copy();	
	CDoubleVector betaR_Vec = CofficVec.copy();
	CDoubleVector phi_Vector(m_pAR,0.0);
	CDoubleVector theta_Vector(m_qMA,0.0);
	m_ep_Vector.create(nLength,0.0);
	mu = X_Vec.mean();
	CDoubleVector Z_Vec(nLength+nPredStep);	
	if (m_bWithConst)
	{   
		for(i=0; i<m_pAR; i++)
			phi_Vector(i) = CofficVec(i+1);		
		for(i=0; i<m_qMA; i++)	
			theta_Vector(i) = CofficVec(m_pAR+1+i);
	}
	else
	{
		for(i=0; i<m_pAR; i++)
			phi_Vector(i) = CofficVec(i);		
		for(i=0; i<m_qMA; i++)	
			theta_Vector(i) = CofficVec(m_pAR+i);
	}	   
		double ZA = mean(X_Vec);
		for(i=0; i<nLength; i++)
		{
			double tmpone = 0, tmptwo = 0, tmpthr = 0;
			int loop = 0;
			if(i+1 <= p)
				loop = i;
			else
				loop = p;
			
			for(j=0; j<loop; j++)			
				tmpone += phi_Vector(j) * X_Vec(i-j-1);			
			if(i+1 <= q)
				loop = i;
			else
				loop = q;
			for(j=0; j<loop; j++)
				tmptwo += theta_Vector(j) * m_ep_Vector(i-j-1);
			for(j=p-1; j>=i; j--)
				tmpthr +=  phi_Vector(j);
			m_ep_Vector(i) = X_Vec(i) - tmpone + tmptwo - tmpthr*ZA;
		}
       
		///求预报
		for(i=0; i<X_Vec.vlen(); i++)		
			Z_Vec(i) = X_Vec(i);
		for(i=nLength; i<nLength+nPredStep; i++)
			Z_Vec(i) = 0;
			for(i=0; i<nPredStep; i++)
		{
			double tmpone = 0;
			double tmptwo = 0;
			for(j=0; j<p; j++)
				tmpone += phi_Vector(j) * Z_Vec(nLength+i-j-1);
			if(i+1 > q)
				tmptwo = 0;
			else
			{
				for(j=i; j<q; j++)
					tmptwo += theta_Vector(j) * m_ep_Vector(nLength+i-j-1);
			}
			Z_Vec(nLength+i) = tmpone - tmptwo;
		}
	CDoubleVector temp_Vector(nPredStep);
	for(i=0; i<nPredStep; i++)
		temp_Vector(i) = Z_Vec(nLength+i);	
    CofficVec = betaR_Vec.copy();
	return temp_Vector;
}

////******函数表达式*******///////

double CArima::FuncX(CDoubleVector X_Vec)
{
	double FValue = 0;
	double dtttt = min(m_VecRow,30);
    CDoubleVector ARcoefficent_Vec( dtttt,0.0);
	CDoubleVector MAcoefficent_Vec(dtttt,0.0);
	CDoubleVector residue_Vec(m_VecRow,0.0);
	CDoubleVector diffvec1 = m_diff_Vec.copy();
	diffvec1 = diffvec1 - diffvec1.mean();
	if (X_Vec.vlen() <  dtttt) 
	{		
		for (int i=0; i<m_pAR; i++)
		   ARcoefficent_Vec(i) = X_Vec(i);
	}
	if (m_qMA <  dtttt)
	{
		for (int i=0; i<m_qMA; i++)		
			MAcoefficent_Vec(i) = X_Vec(m_pAR+i);	
	}	
    CDoubleVector Inverse_Vec = GetInverseFunc(ARcoefficent_Vec, MAcoefficent_Vec, dtttt);
	CDoubleVector temp_Vec(m_VecRow,0.0);

	for (int i=0; i<m_VecRow; i++)
	{
		for (int j=0; j<min(dtttt,i); j++)
		{
			if (m_bWithConst)			
				temp_Vec(i) += Inverse_Vec(j) * diffvec1(i-1-j);			
			else			
				temp_Vec(i) += Inverse_Vec(j) * m_diff_Vec(i-1-j);	
		}
        	if (m_bWithConst)			
				 residue_Vec(i) = diffvec1(i) - temp_Vec(i);			
			else			
				residue_Vec(i) = m_diff_Vec(i) - temp_Vec(i);		  
	}
    if (m_bWithConst)
		FValue = residue_Vec.dot(residue_Vec) + diffvec1(0) * diffvec1(0);    
	else	
 	     FValue = residue_Vec.dot(residue_Vec) + m_diff_Vec(0) * m_diff_Vec(0);	
	return FValue;	
}

///////////*******包维尔算法***********//////////////

void CArima::Powell(CDoubleVector &Data_Vec, CDoubleMatrix &Direction_mx, int &iter, double &FRET)
{
	int i,ibig,itmax = 50;
	CDoubleVector oldData_Vec = Data_Vec.copy();
	CDoubleVector Direction_Vec;
	CDoubleVector PTT_Vec = 2.0*Data_Vec-oldData_Vec;
	double t,dum, del,fp,fptt,ftol = 1e-10;
	FRET = FuncX(Data_Vec);
	iter = 0;
	do {
		do	
		{	
			do 
			{
				iter++;
				fp = FRET;
				ibig = 0;
				del = 0.0;
				for (i=0; i<Data_Vec.vlen(); i++) 
				{
					Direction_Vec = Direction_mx(i);
					fptt = FRET;
					Linmin(Data_Vec, Direction_Vec,FRET);
					if (fabs(fptt-FRET)>del)
					{
						del = fabs(fptt-FRET);
						ibig = i;
					}
				}
				BOOL tttttt = (2.0*fabs(fp-FRET)<=ftol*(fabs(fp)+fabs(FRET)));
				if (2.0*fabs(fp-FRET)<=ftol*(fabs(fp)+fabs(FRET)))
				{
					Erase(Direction_Vec, PTT_Vec, oldData_Vec);
					return;
				}
				if (iter == itmax)
				{
					//“超过最大迭代次数”
					return;
				}
				PTT_Vec = 2.0*Data_Vec-oldData_Vec;
				Direction_Vec = Data_Vec-oldData_Vec;
				oldData_Vec = Data_Vec.copy();
				fptt = FuncX(PTT_Vec);
			} while(fptt>=fp);
			dum = fp-2*FRET+fptt;
			t = 2.0*dum*(fp-FRET-del)*(fp-FRET-del)-del*(fp-fptt)*(fp-fptt);
		} while(t>=0.0);
		Linmin(Data_Vec, Direction_Vec,FRET);
		Direction_mx(ibig) = Direction_Vec;
	} while(1);
	return;
}

////////////////***********还原方向重新搜索************///////////

void CArima::Erase(CDoubleVector &Direction_Vec, CDoubleVector &PTT_Vec, CDoubleVector &Data_Vec)
{
	int ttt = Direction_Vec.vlen();
	for (int i=0; i<Direction_Vec.vlen(); i++)
	{
		Direction_Vec(i) = 0;
		PTT_Vec(i) = 0;
		Data_Vec(i) = 0;
	}
	return;
}


//////////**************搜索从初始点沿指定方向的极小值点**************////////

void CArima::Linmin(CDoubleVector &Data_Vec, CDoubleVector &Direction_Vec, double &FRET)
{
	double ax = 0.0;
	double bx = 0.0;
	double fa = 0.0;
	double fx = 0.0;
	double fb = 0.0;
	double Xmin = 0.0;
	double xx = 1.0;
	Mnbank(ax,xx,bx,fa,fx,fb,Data_Vec,Direction_Vec);
	FRET = Brent(ax,xx,bx,Xmin, Data_Vec, Direction_Vec);
	Direction_Vec = Xmin*Direction_Vec;
	Data_Vec +=  Direction_Vec;
	return;
}


////////////********用黄金分割法确定极小点所在的区间**********////////////

void CArima::Mnbank(double &ax, double &xx, double &bx, double fa, double fx, double fb, CDoubleVector Data_Vec, CDoubleVector Direction_Vec)
{
	double r,q,dum, gold = 1.618034;
	double mu,ulim,fu,tiny = 1e-20;
	fa = FuncX(Data_Vec+ax*Direction_Vec);
	fx = FuncX(Data_Vec+xx*Direction_Vec);
	if (fx>fa)
	{
		dum = ax;
		ax = xx;
		xx = dum;
		dum = fx;
		fx = fa;
		fa =dum;
	}
	bx = xx+gold*(xx-ax);
	fb = FuncX(Data_Vec+bx*Direction_Vec);
	while (fx>fb)
	{
		r = (xx-ax)*(fx-fb);
		q = (xx-bx)*(fx-fa);
		dum = q-r;
		if (fabs(dum)<tiny)
		{
			dum = tiny;
		}
		mu = xx-((xx-bx)*q-(xx-ax)*r)/(2*dum);
		ulim = xx+100*(bx-xx);
		if ((mu-xx)*(mu-bx)>0)
		{
			fu = FuncX(Data_Vec+mu*Direction_Vec);
			if (fu<fb)
			{
				ax = xx;
				fa = fx;
				xx = mu;
				fx = fu;
				return;
			}
			else
			{
				if (fu >fx)
				{
					bx = mu;
					fb = fu;
					return;
				}
			}
			mu = bx+gold*(bx-xx);
			fu = FuncX(Data_Vec+mu*Direction_Vec);
		}
		else
		{
			if ((bx-mu)*(mu-ulim)>0)
			{
				fu = FuncX(Data_Vec+mu*Direction_Vec);
				if (fu<fb)
				{
					xx = bx;
					bx = mu;
					mu = bx+gold*(bx-xx);
					fx = fb;
					fb = fu;
					fu = FuncX(Data_Vec+mu*Direction_Vec);
				}
			}
			else
			{
				if ((mu-ulim)*(ulim-bx)>=0)
				{
					mu = ulim;
					fu = FuncX(Data_Vec+mu*Direction_Vec);
				}
				else
				{
					mu = bx+gold*(bx-xx);
					fu = FuncX(Data_Vec+mu*Direction_Vec);
				}
			}
		}
		ax = xx;
		xx = bx;
		bx = mu;
		fa = fx;
		fx = fb;
		fb = fu;
	}
	return;
}

////////////*********布伦特一维优化**************////////

double CArima::Brent(double ax, double xx, double bx, double &Xmin, CDoubleVector Data_Vec, CDoubleVector Direction_Vec)
{
	int done, iter,itmax = 100;
	double d,fu,r,q,p,xm,tol1,tol2,ma,mb,cgold = 0.381966;
	double mu, etemp, dum, me, fx, fv, fw, mv, mw, mx, zeps = 1e-10;
	ma = ax;
	if (bx<ax)
	{
		ma = bx;
	}
	mb = ax;
	if (bx>ax)
	{
		mb = bx;
	}
	mv = xx;
	mw = mv;
	mx = mv;
	me = 0.0;
	fx = FuncX(Data_Vec+mx*Direction_Vec);
	fv = fx;
	fw = fx;
	for (iter=1; iter<=itmax; iter++)
	{
		xm = 0.5*(ma+mb);
		tol1 = (1e-10)*fabs(mx)+zeps;
		tol2 = 2.0*tol1;
		if (fabs(mx-xm)<=tol2-0.5*(mb-ma))
		{
			break;
		}
		done = -1;
		if (fabs(me)>tol1)
		{
			r = (mx-mw)*(fx-fv);
			q = (mx-mv)*(fx-fw);
			p = (mx-mv)*q-(mx-mw)*r;
			q = 2.0*(q-r);
			p = (q>0.0)?(-p):p;
			q = fabs(q);
			etemp = me;
			me = d;
			dum = fabs(0.5*q*etemp);
			if (fabs(p)<dum &&  p>q*(ma-mx) && p<q*(mb-mx))
			{
				d = p/q;
				mu = mx+d;
				if (mu-ma<tol2 || mb-mu<tol2)
				{
					d = (xm-mx)>0?fabs(tol1):-fabs(tol1);
				}
				done = 0;
			}
		}
		if (done)
		{
			if (mx>= xm)
			{
				me = ma-mx;
			}
			else
			{
				me = mb-mx;
			}
			d = cgold*me;
		}
		if (fabs(d)>=tol1)
		{
			mu = mx+d;
		}
		else
		{
			//--mu = (d>0)?mx+fabs(tol1):mx-false(tol1);
			mu = (d>0)?mx+fabs(tol1):mx-0; //--UNIX PORT
		}
		fu = FuncX(Data_Vec+mu*Direction_Vec);
		if (fu<=fx)
		{
			if (mu>=mx)
			{
				ma = mx;
			}
			else
			{
				mb = mx;
			}
			mv = mw;
			fv = fw;
			mw = mx;
			fw = fx;
			mx = mu;
			fx = fu;
		}
		else
		{
			if (mu<mx)
			{
				ma = mu;
			}
			else
			{
				mb = mu;
			}
			if (fu<=fw || mw==mx)
			{
				mv = mw;
				fv = fw;
				mw = mu;
				fw = fu;
			}
			else
			{
				if (fu<=fv || mv== mx || mv==mw)
				{
					mv = mu;
					fv = fu;
				}
			}
		}
	}
	if (iter>itmax)
	{
		//“超过最大迭代次数”
		return fx;
	}
	Xmin = mx;
	return fx;
}

/////////***********输出统计量*************//////////////

void CArima::GetArimaStatis(CDoubleVector diffvec, CDoubleVector ep_Vector, CDoubleVector CofficVec)
{
	double SST = 0;          
	double dfSSQ = 0;
	double dfSST = 0;
	double Varestimate = 0;      
	double mu = diffvec.mean();
	double sumres = 0;	
	int i = 0;
	int j = 0;
	m_SSQ = m_ep_Vector.dot(m_ep_Vector);
	for (i=1; i<m_VecRow; i++)	
		 sumres += (m_ep_Vector(i)-m_ep_Vector(i-1)) * (m_ep_Vector(i)-m_ep_Vector(i-1));	    
    m_dw = sumres / m_SSQ;	
	m_loglikeli = -m_VecRow/2 * (1 + log(2*3.1415926) + log(m_SSQ/m_VecRow));	
	if (m_bWithConst)
	{
		diffvec = diffvec - mu;
		dfSSQ = m_VecRow - m_pAR - m_qMA-1;	
		Varestimate = m_SSQ / dfSSQ;          
		m_stderror = sqrt(Varestimate); 
		SST = diffvec.dot(diffvec);
		dfSST = m_VecRow - 1;
		m_AIC = -2 * m_loglikeli /m_VecRow +2*(m_pAR+m_qMA+1)/m_VecRow;
        m_SC = -2 * m_loglikeli /m_VecRow + ((m_pAR+m_qMA+1)*log(m_VecRow))/m_VecRow;
     	CDoubleMatrix xxx_mx(m_VecRow-m_pAR-m_qMA+2, m_pAR+m_qMA+1,0.0);
		for (i=0; i<m_VecRow-m_pAR-m_qMA+1; i++)
		{   
			xxx_mx(i,0) = 1;
			for (j=1; j<m_pAR+m_qMA+1; j++)		
				xxx_mx(i,j) = diffvec(i+m_pAR+m_qMA-j);			
		}
		CDoubleMatrix varcov_mx(m_pAR+m_qMA+1,m_pAR+m_qMA+1,0.0);	
		CDoubleVector stderror_Vec(m_pAR+m_qMA+1,0.0);
		CDoubleVector tvalue1_Vec (m_pAR+m_qMA+1,0.0);  
		CDoubleVector pvalue_Vec (m_pAR+m_qMA+1,0.0);
		varcov_mx = xxx_mx.trans() * xxx_mx;	   
		varcov_mx = varcov_mx.inv() * Varestimate;
	
		for (i=0; i<m_pAR+m_qMA+1; i++)
		{
			stderror_Vec(i) = sqrt(varcov_mx(i,i));
			tvalue1_Vec(i) = CofficVec(i) / stderror_Vec(i);
            CDoubleVector tvalue_Vec = tvalue1_Vec.copy();
			if (tvalue_Vec(i) >= 0)			
				tvalue_Vec(i) = -1 * tvalue_Vec(i);			
			pvalue_Vec(i) = 2.0 * tcdf(tvalue_Vec(i),m_VecRow);		
		}	

	m_stderror_Vec = stderror_Vec;
	m_tvalue_Vec = tvalue1_Vec;
	m_pvalue_Vec = pvalue_Vec;
	}
	else
	{
		dfSSQ = m_VecRow - m_pAR - m_qMA;
		Varestimate = m_SSQ / dfSSQ;          
		m_stderror = sqrt(Varestimate);            		
		SST = (diffvec - mu).dot(diffvec - mu);  
		dfSST = m_VecRow;
		m_AIC = -2 * m_loglikeli /m_VecRow +2*(m_pAR+m_qMA)/m_VecRow;
        m_SC = -2 * m_loglikeli /m_VecRow + ((m_pAR+m_qMA)*log(m_VecRow))/m_VecRow;
		CDoubleMatrix xxx_mx(m_VecRow-m_pAR-m_qMA+1, m_pAR+m_qMA,0.0);
		for (i=0; i<m_VecRow-m_pAR-m_qMA+1; i++)
		{
			for (j=0; j<m_pAR+m_qMA; j++)			
				xxx_mx(i,j) = diffvec(i+m_pAR+m_qMA-1-j);		
		}
		CDoubleMatrix varcov_Vec(m_pAR+m_qMA,m_pAR+m_qMA,0.0);		
		CDoubleVector stderror_Vec(m_pAR+m_qMA,0.0); 
		CDoubleVector tvalue1_Vec (m_pAR+m_qMA,0.0); 
		CDoubleVector pvalue_Vec (m_pAR+m_qMA,0.0);
		varcov_Vec = xxx_mx.trans() * xxx_mx;	   
		varcov_Vec = varcov_Vec.inv() * Varestimate;
		for (i=0; i<m_pAR+m_qMA; i++)
		{
			stderror_Vec(i) = sqrt(varcov_Vec(i,i));
			tvalue1_Vec(i) = CofficVec(i) / stderror_Vec(i);	
			CDoubleVector tvalue_Vec = tvalue1_Vec.copy();
			if (tvalue_Vec(i) >= 0)			
				tvalue_Vec(i) = -1 * tvalue_Vec(i);			
			pvalue_Vec(i) =  tcdf(tvalue_Vec(i),m_VecRow);
		}
	
	m_stderror_Vec = stderror_Vec;
	m_tvalue_Vec = tvalue1_Vec;
	m_pvalue_Vec = pvalue_Vec;

	}
	m_Rsquare = 1 - m_SSQ / SST;     
	m_adjustedRSquare = 1 - (m_SSQ/dfSSQ) / (SST/dfSST);	  
}

////////*******输出表格和图形***********//////////

void CArima::DrawResult()
{
	///////////ARIMA模型参数估计///////////////

	int i=0;
	CRsltElementTable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->CreateTable(2, 2);
	pTable->SetTitle("ARIMA模型参数估计");
	pTable->InsertColumn(0,"参数名称");
	pTable->InsertColumn(1,"参数");
	pTable->InsertColumn(2,"标准误");
	pTable->InsertColumn(3,"T值");
	pTable->InsertColumn(4,"P值");
	CTStringArray NameArr;
	CTString strTemp;
	if (m_bWithConst)
		NameArr.Add("常数项");
	for (i=0; i<m_pAR; i++)
	{
		strTemp.Format("%d", i+1);
		NameArr.Add("AR("+strTemp+")");
	}
	for (i=0; i<m_pAR; i++)
	{
		strTemp.Format("%d", i+1);
		NameArr.Add("MA("+strTemp+")");
 	}
	for(i=0;i<m_Coef_vec.vlen();i++)
	{
		pTable->InsertItem(i, NameArr.GetAt(i));
		pTable->SetItemText(i, 1, m_Coef_vec(i) );
		pTable->SetItemText(i, 2, m_stderror_Vec(i) );
		pTable->SetItemText(i, 3, m_tvalue_Vec(i) );
		pTable->SetItemText(i, 4, m_pvalue_Vec(i) );
	}
	pETable= new CRsltElementTable("ARIMA模型参数估计",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);


	/////////////////////////输出相关统计量/////////////////////////
	
	pTable = new CTLTable;
	CTString strTitle = _T("");
	
	pTable->SetTitle("相关统计量");
	pTable->SetHead("ARIMA模型相关统计量");
	pTable->InsertColumn(0,"参数名");
	pTable->InsertColumn(1,"数值");
	pTable->InsertItem(0,"R平方",false);
	pTable->SetItemText(0,1,m_Rsquare);
    pTable->InsertItem(1,"修正后的R平方",false);
	pTable->SetItemText(1,1,m_adjustedRSquare);
    pTable->InsertItem(2,"回归标准误",false);
	pTable->SetItemText(2,1,m_stderror);
    pTable->InsertItem(3,"残差平方和",false);
	pTable->SetItemText(3,1,m_SSQ);
    pTable->InsertItem(4,"对数似然值",false);
	pTable->SetItemText(4,1,m_loglikeli);
    pTable->InsertItem(5,"AIC",false);
	pTable->SetItemText(5,1,m_AIC);
    pTable->InsertItem(6,"SC",false);
	pTable->SetItemText(6,1,m_SC);
    pTable->InsertItem(7,"DW统计量",false);
	pTable->SetItemText(7,1,m_dw);

	pETable= new CRsltElementTable("最终参数",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	

	//////////////**********输出预测表格**********//////////
	
	NameArr.RemoveAll();
	pTable = new CTLTable;
    strTitle = _T("");
	pTable->CreateTable(2, 2);
	pTable->SetTitle("ARIMA模型预测值");
	pTable->InsertColumn(0,"序号");
	pTable->InsertColumn(1,"数值");
	for (i=0; i<m_nPredStep; i++)
	{
		strTemp.Format("%d", i+1);
		NameArr.Add(strTemp);
	}

	for(i=0;i<m_nPredStep;i++)
	{
		pTable->InsertItem(i, NameArr.GetAt(i));
		pTable->SetItemText(i, 1, m_predict_Vec(i) );
	}
	pETable= new CRsltElementTable("ARIMA模型预测值",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);

	//////////////**********输出预测图形**********//////////
	if (m_nPredStep > 0)
	{
		TDataPointValVal* pOrgVV;
		TDataPointValVal* pPredVV;
		pOrgVV = new TDataPointValVal[m_orginal_Vec.vlen()];
		pPredVV = new TDataPointValVal[m_nPredStep+1];
		int j;
		for(j=0; j<m_orginal_Vec.vlen(); j++)
		{
			pOrgVV[j].fXVal = j+1;
			pOrgVV[j].fYVal = m_orginal_Vec[j];
		}  
		pPredVV[0].fXVal = j;
		pPredVV[0].fYVal = m_orginal_Vec[j-1];
		for(int i=0;i<m_nPredStep;i++)
		{
			pPredVV[i+1].fXVal =j+i+1;
			pPredVV[i+1].fYVal =m_predict_Vec[i];
		}
		CTChartLine *pTChartLine = new CTChartLine();
		pTChartLine->SetLine(pOrgVV, m_orginal_Vec.vlen(), ValVal,"原始数据"); 
		pTChartLine->SetLine(pPredVV, m_nPredStep, ValVal,"预测数据"); 
		CRsltElementChart *pREC = new CRsltElementChart("-效果图",  pTChartLine);
		m_pResult->Add(pREC);    
	}
	return;
}

