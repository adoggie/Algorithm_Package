#include "StdAfx.h"
// #include "tlAdvn.h"
#include "MultiAnova.h"

#include "Covariance.h"

CMultiAnova::CMultiAnova(void)
{
	m_nDependIndex  = -1;
	m_pDataInterface= NULL;
	m_sstype        = 3;
}

CMultiAnova::~CMultiAnova(void)
{ 
}

/*
警告
警告	
出现了不合理的数据或样本数过多，不适合作方差分析	
程序终止了相关计算	
请检查输入的数据	
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//												重复非重复双因素方差分析
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ytli @2002/09/09
//ytli 测试通过 @2002/09/18.
void CMultiAnova::Anova(CDoubleMatrix X, CDoubleVector& p, CDoubleMatrix& q, int reps)
{
	//  因为传入的矩阵不可能是有缺失值的矩阵,所以可以不用考虑.
	//	if( any( isnan(matrix) ) )
	//		AfxMessageBox("anova不能分析有缺失值的数据!");
	
	int r = X.mcol();
	int c = X.mrow();
	
	int m = r/reps;
// 	if( m != r/reps )
// 		AfxMessageBox("对重复实验，reps>1，则行数应该是reps的倍数");
	
	CDoubleMatrix Y(m, c);//此时m代表行因子的水平，c表示列因子的水平，
	
	if( reps==1 )
	{
		Y = X;
	}
	else if( reps>1 )
	{		
		for(int i=0; i<m; i++ )
		{		
			for(int j=0; j<c; j++)
			{
				int nBegin = i*reps;
				double sum = 0;
				
				for(int k=nBegin; k<nBegin+reps; k++)//求平均数
				{
					sum += X(k,j);
				}
				Y(i,j) = sum/reps;
			}			
		}
	}
	
	CDoubleVector colmean = mean(trans(Y));//求列的均值
	CDoubleVector rowmean = mean(Y);//求行的均值
	
	double gm = mean(colmean);//求总的平均值μ
	int df1 = c-1;   //   列的自由度     
	int df2 = m-1;    //  行的自由度   
	
	int edf;
	int idf;
	
	if(reps == 1)
		edf = (c-1)*(r-1);
	else
	{
		edf = c*m*(reps-1);    
		idf = (c-1)*(m-1);     
	}
	
	//求SA   列偏差平方和   
	colmean = colmean - gm;
	rowmean = rowmean - gm;
	double CSS = m*reps*dot(colmean, colmean);  
	//求SB   行偏差平方和
	double RSS = c*reps*dot(rowmean, rowmean);
	//求ST  总偏差平方和
	double correction = (c*m*reps)*gm*gm;
	double TSS = sum( sum( prod(X,X) ) ) - correction; 
	
	//如果是重复实验 ISS代表交互作用
	double ISS = reps*sum( sum( prod(Y,Y) ) ) - correction - CSS - RSS;  
	
	double SSE;
	if( reps==1 )//如果是非重复实验 误差偏差平方SSE即为ISS
		SSE = ISS;  
	else//重复实验 残差平方SSE为TSS - CSS - RSS - ISS
		SSE = TSS - CSS - RSS - ISS;     
    
	double ip;
	double MSE,colf,rowf,colp,rowp,intf;
	CDoubleVector vectorTemp(1);
	CDoubleVector vector1(2);
	
	if( SSE!=0 )
	{
		MSE  = SSE/edf;//如果误差偏差平方不为0，则标准误差偏差平方MSE为SSE/edf;（自由度）
		colf = (CSS/df1) / MSE;//SA的F统计量值
		rowf = (RSS/df2) / MSE;//SB的F统计量值
		colp = 1 - fcdf(colf,df1,edf); 
		rowp = 1 - fcdf(rowf,df2,edf);   //Fcdf为F累计分布函数 ，给出sig.
		vector1(0)    = colp;	
		vector1(1)    = rowp;
		if( reps>1 )
		{
			intf = (ISS/idf)/MSE;
			ip   = 1 - fcdf(intf,idf,edf);		
			
			vectorTemp(0) = ip;
			vector1 = comvec(vector1,vectorTemp);
		}
	}
	else
	{
		if (edf > 0)
			MSE = 0;
		
		if ( CSS==0 )
		{
			colf = 0;
			colp = 1;
		}
		else
		{
			colf = 1e100;// 1e100表示无穷大
			colp = 0;
		}
		if (RSS==0)
		{
			rowf = 0;
			rowp = 1;
		}
		else    
		{
			rowf = 1e100;// 1e100表示无穷大
			rowp = 0;
		}
		vector1(0)    = colp;	
		vector1(1)    = rowp;
		
		
		if( (reps>1) && (ISS==0) )  // 对重复实验且交互效应为0时的判断
		{
			intf = 0;		
			vectorTemp(0) = 1;
			vector1 = comvec(vector1,vectorTemp);
		}
		else if (reps>1)  
		{
			intf = 1e100;//表示无穷大.
			vectorTemp(0) = 0;
			vector1 = comvec(vector1,vectorTemp);
		}
	}	
	p = vector1;
	
	CDoubleMatrix matrix(5, 5, 0.0);
	if (reps > 1)
	{
		matrix(0,0) = CSS;
		matrix(1,0) = RSS;
		matrix(2,0) = ISS;
		matrix(3,0) = SSE;
		matrix(4,0) = TSS;
		
		matrix(0,1) = df1;
		matrix(1,1) = df2;
		matrix(2,1) = idf;
		matrix(3,1) = edf;
		matrix(4,1) = r*c-1;
		
		matrix(0,2) = CSS/df1;
		matrix(1,2) = RSS/df2;
		matrix(2,2) = ISS/idf;
		matrix(3,2) = SSE/edf;
		
		matrix(0,3) = colf;
		matrix(1,3) = rowf;
		matrix(2,3) = intf;
	} 
	
	for( int i=0; i<p.vlen()-1; i++)
	{
		matrix(i,4) = p(i);
	}
	q = matrix;
}

/**************************************************************************** 
%	manov 是多因素方差分析
%   P=MANOV (Y,GROUP,MODEL,SSTYPE,GNAME) 按GROUP 中因子变量对向量Y做%   多因素方差分析
%   GROUP必须为元胞数组，即每一变量对应一因子变量且其个数必须与Y同
%   MODEL 表示使用何种效应分析
%   'linear'或1对应只分析各因子主效应
%   'interaction'或2对应只分析各因子主效应及两个因子之间交互效应 
%   'full'或N（N为因子个数）对应分析各因子主效应及所有因子之间交互效应
%   如果是K≤N对应分析各因子主效应及对应Kg个因子之间交互效应
%   如果是MODEL 是向量,其元素为对应效应，如对包括第J个因子的I效应
%   有BITGET(V(I),J)==1
%    [0 0 1]   1   A的主效应对应的MODEL向量值
%    [0 1 0]   2   B的主效应对应的MODEL向量值
%    [1 0 0]   4   C的主效应
%    [0 1 1]   3   AB交互效应
%    [1 1 0]   6   BC交互效应
%    [1 0 1]   5   AC交互效应
%    [1 1 1]   7   ABC交互效应
%   SSTYPE 值可取1，2，3 对应偏差平方和求法，默认为3
%   GNAMES 存储因子变量名
%   P 返回显著性概率值.
%   T返回方差分析表
%   TERMS
%   ytli @2002/09/13
%   ytli测试通过  @2002/09/24
****************************************************************************/
int CMultiAnova::manov(CDoubleVector y/*y的长度必须和group的行数相等*/, 
					   CIntMatrix group/*按列存取*/, 
					   CIntVector model,		
					   /*输出参数:*/CDoubleVector& p,CIntVector& terms,
					   int sstype)
{
	assert(sstype == 1 || sstype == 2 || sstype == 3||sstype == 4);
	
//	TraceVector(y);
//	TraceVector(p);
	
	int i,j,u;//for 循环将用到,在使用时不忘记清0.
	
	int n   = y.vlen();//n代表y的总个数，求自由度的时候可用到.
	int ng  = group.mcol();//ng输入变量的个数.int rg  = group.mrow();
	int rg  = group.mrow();

    if( rg>3000)
	{;
//		return MULTIANOVA_COLERROR;
	}
	
	assert( n==group.mrow() );
	
	if( ng!=m_tVarInfo.iCount)
	{
//		AfxMessageBox("你选的数据列数不对,或者数据全部相同的列!");
		return MULTIANOVA_COLERROR;
	}
	
//	if( ng>53 )
//		AfxMessageBox("不允许大于53个变量!");	
	
	//1:  准备因子分组变量.
	//创立分组索引和命名.
	int dcode = 1;//用来定义dummy变量方法
	
	CPtrArray   gdum;
	CIntVector  dfvar(ng,0);   //存储所有因子的自由度
	
	int nError = 1;
//	CIntMatrix grptrans = trans(group);
//	int nrow=grptrans.mrow();
//	int ncol=grptrans.mcol();
//	CIntVector val=grptrans.row(0);
	CIntVector val=group.col(0);

	int vlen=val.vlen();
	for(j=0; j<ng; j++)
	{		
		CIntVector gij,gnj;
		grp2idx(group.col(j), gij, gnj);
				
		nError *= gnj.vlen();

//		if( nError>64000 )
//			return MULTIANOVA_OVERFLOW;	
		
		dfvar(j) = gnj.vlen() - 1;
		
		CDoubleMatrix tempDoubleMatrix = idummy(gij, dcode);
		
		CDoubleMatrix *ssss = new CDoubleMatrix(tempDoubleMatrix.mrow(), tempDoubleMatrix.mcol());
		*ssss = copy(tempDoubleMatrix); 	
		
		gdum.Add( ssss );  //（idummy见子函数）为每一因子变量创建虚变量数组.
	}
	
	
	//% 2: 根据model对所要分析的每一效应创建虚变量数组.
	int nterms = model.vlen();  //求解需要给出的主效应，交叉效应的个数
	CIntVector sterms, sindex;  //sterms为排列好的向量.sindex为索引.
	CIntVector termsort = copy(model);
	sterms = termsort.Sort(sindex);
		
	int ncols = 1;
	
	CPtrArray termdum;
	termdum.SetSize(nterms);
	CTStringArray tnames;
	tnames.SetSize( nterms+1 );
	CIntVector dfterm0(nterms,0);
	//对每一种效应给出相应名称及存储位置.
	int df0(1),tm,lne;
	CDoubleMatrix tdum;
	CIntVector termlength( nterms );
	CTString tn;
	int abcddd=0;
	for( j=0; j<nterms; j++)
	{//% Loop over elements of the term
		df0 = 1;
		tm  = sterms(j);
		tn = "";
		int k;
		CDoubleMatrix tdum1;
		tdum = tdum1;
		while(tm)
		{//% 以下程序是给存储各效应及给出其名称
			abcddd++;
			double dTemp = log10(tm)/log10(2);
			if( dTemp-(int)dTemp < 0 )
				lne = (int)dTemp;
			else
				lne = 1+(int)dTemp;
			
			int bset = 0xFFFFFFFF - (int)pow(2.0,lne-1);
			tm = tm&bset;//%将二进制表示的tm在lne位置0，（位运算）
			df0 = df0 * dfvar(lne-1);
			
			// % 交互效应
//			CDoubleMatrix* pDoublematrix = (CDoubleMatrix*)gdum[lne-1];
			CDoubleMatrix* pDoublematrix = (CDoubleMatrix*)gdum[lne-1];
//			TraceMatrix(*pDoublematrix);
			
			tdum = termcross(tdum, *pDoublematrix);//%（见子函数，主要用于变量间交互效应）
			
			// % 获取名称
			CTString strName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[lne-1]);
			if ( tn=="" )
				tn = strName;
			else
				tn = tn + "*" + strName.GetData(); 			
			
			//对SA*B和SB *A相同的利用
			CIntVector oldtm(nterms,0);
			for( i=0; i<j; i++)
			{
				if( tm==sterms(i))
					oldtm(i) = i+1;
			}
			if( oldtm(0)!=0 )
			{
				k = oldtm(0);
				CDoubleMatrix* pDoubleMat = (CDoubleMatrix*)termdum[sindex(k-1)-1];
				tdum = termcross(tdum, *pDoubleMat);
				
				tn = tnames[sindex(k-1)-1] + "*" + tn.GetData();
				df0 = df0*dfterm0(sindex(k-1)-1);
				break;
			}
		}		
		
		//% 存储各效应及名称
		k= tdum.mcol();		
		termlength(sindex(j)-1) = k;
		
		ncols = ncols + k;
		int sj = sindex(j);
		CDoubleMatrix* ptdum = new CDoubleMatrix(tdum.mrow(), tdum.mcol());
		*ptdum = copy(tdum);
		termdum[sj-1] = ptdum;
		if ( tn=="" )
			tn = "Constant";
		
		tnames[sj-1]  = tn;
		dfterm0(sj-1) = df0;
	}
	
	//防止内存泄漏
	for( i=0; i<gdum.GetSize(); i++ )
	{
		CDoubleMatrix* pDoubleMat1 = (CDoubleMatrix*)gdum.GetAt(i);
		if(pDoubleMat1)
			delete pDoubleMat1;	
	}
	gdum.RemoveAll();	
	
	tnames[ tnames.GetSize()-1 ] = "修正后残差";
	
	//% 3:  创建全交矩阵
	CDoubleMatrix dmat(n, ncols, 1.0);
	CIntVector termname(ncols, 0);
	CIntVector termcode(ncols, 0);
	CIntVector termstart( termlength.vlen() );
	CIntVector termend( termlength.vlen() );
	
	termstart(0) = 2;
	termend(0)   = termstart(0) + termlength(0) - 1;
	
	for( i=1; i<termlength.vlen(); i++)
	{
		termstart(i) = termstart(i-1) + termlength(i-1);
		termend(i)   = termstart(i) + termlength(i) - 1;
	}
	for( j=0; j<nterms; j++)
	{	
		int nLength = termend(j)-termstart(j)+ 1;
		if( nLength>0 )
		{
			CIntVector clist( nLength );
			for( i=0; i<nLength; i++)
			{
				clist(i) = min( termstart(j),termend(j) ) + i;
			}
			
			for( i=0; i<clist.vlen(); i++ )
			{	
				for( u=0; u<dmat.mrow(); u++ )
				{
					CDoubleMatrix* pp = (CDoubleMatrix*)termdum[j];
					dmat(u,clist(i)-1) = (*pp)(u,i);
				}						
			}
			
			for( u=0; u<clist.vlen(); u++)
			{
				termname( clist(u)-1 ) = j+1;
				//		termcode( clist(u)-1 ) = termlist(j);
			}
		}
	}
	
	//防止内存泄漏
	for( i=0; i<termdum.GetSize(); i++ )
	{
		CDoubleMatrix* pp1 = (CDoubleMatrix*)termdum[i];
		
		if(pp1)
			delete pp1;
	}
	termdum.RemoveAll();
	
	//% 4:  Fit the full model and 计算残差平方和，填充全交矩阵
	double mu  = mean(y);
	y = y - mu;
	double mu2 = mean(y);
	mu = mu + mu2;
	
	double sst = sum((y - mu2)*(y - mu2));  //%总偏差平方和（国外表示法）
	
	//% initfit见子函数，求残差偏差平方和
	int dfx;
	double ssx;
	
	initfit(dmat, y, termname, sst, mu, ssx, dfx); 
	
	double sse = sst - ssx;
	int dfe = n - dfx;
	
	//%  决定用何种偏差计算方法
	CDoubleVector ssw(nterms,-1);		 // sum of squares with this term
	CDoubleVector sswo(nterms,-1);		 // sum of squares without this term
	CIntVector dfw(nterms,-1);		 // residual d.f. with this term
	CIntVector dfwo(nterms,-1);		 // residual d.f. without this term
	CIntMatrix modw(nterms, nterms);
	CTArray<int,int> IntArray;
	switch(sstype)
	{
	case 1:		
		for( i=0; i<nterms; i++)//row
		{
			for( j=0; j<nterms; j++)//column
			{
				if( i<j )
					modw(i,j) = 0;
				else
					modw(i,j) = 1;
			}	
			IntArray.Add(0);
		}
		IntArray[nterms-1] = 1;
		
		break;
	case 2:
		modw = termsnotcontained(model);//%（见子函数）
		for( i=0; i<modw.mrow(); i++ )
		{
			int sum = 0;
			for( j=0; j<modw.mcol(); j++ )
				sum += modw(i,j);
			
			if(sum==nterms)
				IntArray.Add(1);
			else
				IntArray.Add(0);
		}
		break;
	case 3:		
		for( i=0; i<nterms; i++)//row
		{
			for( j=0; j<nterms; j++)//column
			{			
				modw(i,j) = 1;
			}
			IntArray.Add(i+1);
		}	
		break;
	}
	
	CIntVector k(IntArray.GetSize());
	for( i=0; i<IntArray.GetSize(); i++ )
	{
		k(i) = IntArray[i];
	}
	
	CIntMatrix eye1( nterms, nterms, 0 );
	for( i=0;i<nterms; i++)
	{
		eye1(i,i) = 1;
	}	
	CIntMatrix modwo = modw - eye1;   //% get model without this term
	
	//% 对每种模型计算偏差及自由度
	for( i=0; i<k.vlen(); i++ )
	{
		if(k(i)==1)
		{
			ssw(i) = ssx;//% 对应全因素分析已求得的ssx
			dfw(i) = dfx;	
		}		
	}
	
	//%对每种模型分别计算偏差
	CIntVector dfboth = comvec(dfw, dfwo);
	CIntVector allterms(nterms);
	for( i=0; i<nterms; i++ )
		allterms(i) = i+1;
	
	while( 1 )
	{
		abcddd++;
		//% Find the next model index to fit
		int k = 0;	
		for( i=dfboth.vlen()-1; i>=0; i-- )
		{
			if( dfboth(i)==-1 )
			{
				k = i+1;
				break;
			}
		}
		
		if ( k ==0 )
			break;	
		
		//% 判断需计算的为何种模型
		CIntVector thismod(modwo.mcol());		
		
		if ( k>nterms )
		{
			for( i=0; i<modwo.mcol(); i++ )
				thismod(i)= modwo(k-nterms-1, i);			
		}
		else
		{
			for( i=0; i<modwo.mcol(); i++ )
				thismod(i)= modw(k-1, i);	
		}
		
		int nLength2 = 0;
		for( i=0; i<thismod.vlen(); i++ )
		{
			if(thismod(i)==1)
				nLength2++;
		}
		
		CIntVector keepterms(nLength2);
		//% 获取对应模型的设计矩阵
		j=0;
		for( i=0; i<thismod.vlen(); i++ )
		{
			if( thismod(i)==1 )		
			{
				keepterms(j)= allterms(i);	
				j++;
			}
		}
		
		CIntVector vectTemp0( modwo.mcol()+1 );
		vectTemp0(0) = 0;
		
		CIntVector clist(termname.vlen(), 0);
		
		for( i=0; i<termname.vlen(); i++ )
		{
			if( termname(i)==0 )
			{
				clist(i) = 1;
			}
			for( j=0; j<keepterms.vlen(); j++ )
			{
				if( termname(i)==keepterms(j) )
				{
					clist(i) = 1;
				}
			}
		}
		
		int nclistLength = 0;
		for( i=0; i<clist.vlen(); i++ )
		{
			if(clist(i)!=0)
				nclistLength++;
		}
		CDoubleMatrix X(dmat.mrow(),nclistLength,0.0);
		
		int dddd = 0;
		for( i=0; i<dmat.mcol(); i++)
		{
			for( j=0; (j<dmat.mrow()) && (clist(i)!=0); j++)
			{
				X(j,dddd) = dmat(j,i);
			}
			if( clist(i)!=0 )
				dddd++;
		}
		
		double ssx0;
		int dfx0;
		anovanfit(X, y, ssx0, dfx0);
		
		//% 求各效应
		CIntMatrix mod0(nterms,thismod.vlen()) ;
		CIntVector temp(modw.mrow(), 0);
		for( i=0; i<nterms; i++ )
		{
			for( j=0; j<thismod.vlen(); j++ )
			{
				mod0(i,j) = thismod(j);
			}
		}
		
		//处理ssw,dfw,dfboth
		CIntMatrix tttt(modw.mrow(),modw.mcol(), 0);
		for( i=0; i<modw.mrow(); i++ )
		{
			for( j=0; j<modw.mcol(); j++ )
			{
				if( modw(i,j)==mod0(i,j) )
					tttt(i,j) = 1;
			}
		}
		
		CIntVector k1(tttt.mrow(), 1);
		for( i=0; i<tttt.mrow(); i++ )
		{
			for( j=0; j<tttt.mcol(); j++ )
			{
				if( tttt(i,j)==0 )
					k1(i) = 0;
			}
		}
		
		int nkLength = 0;
		for( i=0; i<k1.vlen(); i++ )
		{
			if(k1(i)!=0)
				nkLength++;
		}
		
		if( nkLength!=0 )
		{
			j = 0;
			CIntVector k(nkLength);
			for( i=0; i<k1.vlen(); i++ )
			{
				if( k1(i)!=0 )
					k(j++) = i+1;
			}
			
			for( i=0; i<nkLength; i++)
			{
				ssw( k(i)-1 ) = ssx0;
				dfw( k(i)-1 ) = dfx0;
				dfboth( k(i)-1 ) = 0;
			}
		}
		
		//处理sswo,dfwo,dfboth
		CIntMatrix mmmm(modwo.mrow(),modwo.mcol(), 0);
		for( i=0; i<modwo.mrow(); i++ )
		{
			for( j=0; j<modwo.mcol(); j++ )
			{
				if( modwo(i,j)==mod0(i,j) )
					mmmm(i,j) = 1;
			}
		}
		
		CIntVector k2(mmmm.mrow(), 1);
		for( i=0; i<mmmm.mrow(); i++ )
		{
			for( j=0; j<mmmm.mcol(); j++ )
			{
				if( mmmm(i,j)==0 )
					k2(i) = 0;
			}
		}
		
		nkLength = 0;
		for( i=0; i<k2.vlen(); i++ )
		{
			if(k2(i)!=0)
				nkLength++;
		}
		
		if( nkLength!=0 )
		{
			j = 0;		
			CIntVector k9(nkLength);
			for( i=0; i<k2.vlen(); i++ )
			{
				if( k2(i)!=0 )
					k9(j++) = i+1;
			}
			
			for( i=0; i<nkLength; i++)
			{
				sswo( k9(i)-1 ) = ssx0;
				dfwo( k9(i)-1 ) = dfx0;
				dfboth( k9(i)-1+nterms ) = 0;
			}
		}
		else
		{
			dfboth(nterms) = 0;
		}
	}
	
	
	//%计算具体偏差平方和
	CDoubleVector ssterm = ssw - sswo;
	
	CIntVector dfterm = dfw - dfwo;
	
	for( i=0; i<dfterm.vlen(); i++ )
	{
		if( dfterm(i)==0 )
			ssterm(i) = 0;//% make this exact
	}
	
	//%计算修正偏差平方和
	CDoubleVector msterm1(dfterm.vlen(), 1);
	for( i=0; i<dfterm.vlen(); i++ )
	{
		if( dfterm(i)>1 )
			msterm1(i) = dfterm(i);
	}
	CDoubleVector msterm = ssterm / msterm1;
	
	double mse = 0;
	if( dfe>0 )		
		mse = sse/max(1, dfe);
	
	//%计算F统计量
	CDoubleVector fstat(msterm.vlen(), 1e100);
	if (mse > 0)
		fstat = msterm / mse;
	
	//%计算概率值
	CDoubleVector pval( fstat.vlen());
	
	for(i=0; i<dfterm.vlen(); i++)
	{
		if(dfterm(i)>0)
			pval(i) = 1 - fcdf(fstat(i), dfterm(i), dfe); //实现fcdf函数.
	}
	p = pval;
	
	//	% 创建多元方差分析表		
	CIntVector sing(dfterm.vlen(),0);
	for( i=0; i<dfterm.vlen(); i++)
	{
		if( dfterm(i) < dfterm0(i) )
			sing(i) = 1;
	}
	
	CDoubleMatrix tbl(ssterm.vlen()+2, 5);
	for( i=0; i<ssterm.vlen(); i++ )
	{
		tbl(i,0) = ssterm(i);
		tbl(i,1) = dfterm(i);
		tbl(i,2) = msterm(i);
		tbl(i,3) = fstat(i);
		tbl(i,4) = pval(i);
	}
	tbl(ssterm.vlen(),0) = sse;
	tbl(ssterm.vlen(),1) = dfe;
	tbl(ssterm.vlen(),2) = mse;
	
	tbl(ssterm.vlen()+1,0) = sst;
	tbl(ssterm.vlen()+1,1) = n-1;
	
	m_mtrixOutPut = tbl;
	terms = model;
	
	m_strNameArrayComb.RemoveAll();
	m_strNameArrayComb.SetSize(tnames.GetSize()+1);
	m_strNameArrayComb[tnames.GetSize()] = "总体";
	
	for( i=0; i<tnames.GetSize(); i++ )
	{
		m_strNameArrayComb[i] = tnames[i];
	}
	return MULTIANOVA_RIGHT;
}

//%TERMSNOTCONTAINED Creates a logical matrix indicating term containment
//%   m(i,j)==1  iff  t(i) is not contained by t(j)
//%   set diagonals to 1 because we want proper containment
//%  
//%   ytli 测试通过 @2002/09/17.
CIntMatrix CMultiAnova::termsnotcontained(CIntVector t)
{
	int i,j;//for循环.
	
	int n = t.vlen();
	
	CIntMatrix a(n, n);
	for( i=0; i<n; i++)
	{
		for( j=0; j<n; j++)
		{
			a(i,j) = t(i);		
		}
	}
	
	CIntMatrix m(n, n, 0);
	
	for( i=0; i<n; i++)
	{
		for( j=0; j<n; j++)
		{			
			if( (a(j,i)&a(i,j))!=a(i,j))
				m(i,j) = 1;
		}
	}
	
	for( i=0; i<n; i++)
	{
		for( j=0; j<n; j++)
		{			
			if( (n*j+i)%(n+1)==0 )
				m(i,j) = 1;
		}
	}
	return m;
}

//%类同initfit
//%ANOVANFIT Fit one model during an n-way anova calculation
//% Tolerance for computing rank from diag(R) after QR decomposition
//%要求dmat y两矩阵为可相乘矩阵.
//ytli @2002/09/13
//ytli 测试通过.  @2002/09/19.
void CMultiAnova::anovanfit(CDoubleMatrix dmat, CDoubleVector y, double& ssx, int& dfx)
{
	int i,j;//for循环.
	double TOL = 100*EPS_ACCUR *dmat.mcol();
	
	CDoubleMatrix q,r;
	CIntVector IntVector;
	r = dmat.qr(q, IntVector, 0);//QR分解.
	
	int nTemp = min(r.mcol(),r.mrow());
	
	CDoubleVector d(nTemp);
	for( i=0; i<nTemp; i++)
	{
		d(i) = fabs(r(i,i));
	}
	
	int p = 0;
	for( i=0; i<d.vlen(); i++ )
	{
		if( d(i)>TOL )
			p += 1; 	
	}
	
	CDoubleMatrix matrixA(q.mrow(),p);
	for( i=0; i<q.mrow(); i++)
	{
		for( j=0; j<p; j++)
		{
			matrixA(i,j) = q(i,j);
		}
	}
	
	CDoubleVector vect(matrixA.mcol(), 0);
	for( i=0; i<matrixA.mcol(); i++)
	{
		for( j=0; j<matrixA.mrow(); j++)
		{
			vect(i) += matrixA(j,i)*y(j);
		}		
	}
	
	ssx = 0;
	for( i=0; i<p; i++ )
	{
		ssx += vect(i)*vect(i);
	}
	dfx = p;
}

//%INITFIT Do initial fit and reduce data for subsequent fits
//% Tolerance for computing rank from diag(R) after QR decomposition
//ytli @2002/09/13
void CMultiAnova::initfit(CDoubleMatrix& dmat, CDoubleVector& y, 
						  CIntVector& termname, double sst, double mu,
						  /*输出行:*/double& ssx, int& dfx)
{
	int i,j;//for循环.
	int ncols = dmat.mcol();
	double TOL = 100*EPS_ACCUR *ncols;
	
	CDoubleMatrix q,r;
	CIntVector IntVector;
	r = dmat.qr(q, IntVector, 0);//QR分解.
	int nTemp = min(r.mcol(),r.mrow());
	CDoubleVector d(nTemp);
	for( i=0; i<nTemp; i++)
	{
		d(i) = fabs(r(i,i));
	}
	
	int p = 0;
	for( i=0; i<d.vlen();i++ )
	{
		if( d(i)>TOL )
			p += 1; 		
	}
	
	CDoubleMatrix matrixA(q.mrow(),p);
	for( i=0; i<q.mrow(); i++)
	{
		for( j=0; j<p; j++)
		{
			matrixA(i,j) = q(i,j);
		}
	}
	
	CDoubleVector yy(matrixA.mcol(), 0);
	for( i=0; i<matrixA.mcol(); i++)
	{
		for( j=0; j<matrixA.mrow(); j++)
		{
			yy(i) += matrixA(j,i)*y(j);
		}		
	}
	
	ssx = 0;
	for( i=0; i<yy.vlen(); i++)
	{
		ssx += yy(i)*yy(i);	
	}
	
	dfx = p;
	
	int n = y.vlen();
	int m = min(n,ncols);
	
	CDoubleMatrix dmat2(p, r.mcol());
	for( i=0; i<p; i++)
	{
		for( j=0; j<r.mcol(); j++)
		{
			dmat2(i,j) = r(i,j);
		}
	}
	
	dmat = dmat2;
	
	CDoubleVector y2(min(n,p));
	for( i=0; i<min(n,p); i++)
	{
		y2(i) = yy(i); 
	}
	
	y = y2;
	//只需要再处理CTStringArray就可以完成了.
	CIntVector termname2(termname.vlen());
	for( i=0; i<IntVector.vlen(); i++)
	{
		termname2(i) = termname[ IntVector(i)-1 ];	
	}	
	termname = termname2;
}

//TERMCROSS Multiply dummy variables for two terms to get interaction
//   ytli 测试通过 @2002/09/17.
CDoubleMatrix CMultiAnova::termcross(CDoubleMatrix a, CDoubleMatrix b)
{
	int i,j;//for循环时用到.
	
	if(a.mcol() == 0 && a.mrow() == 0)		
		return b;
	
	if(b.mcol() == 0 && b.mrow() == 0)
		return a;
	
	assert( a.mrow()==b.mrow());
	
	int na = a.mcol();
	int nb = b.mcol();
	
	CIntVector acols(na*nb);
	CIntVector bcols(na*nb);
	
	for(i=0; i<na*nb; i++)
	{
		acols(i) = i%na +1;		
	}
	for(i=0; i<nb; i++)
	{
		for(j=0; j<na; j++)
		{
			bcols(na*i+j) = i + 1;
		}		
	}	
	
	int nRow = a.mrow();
	CDoubleMatrix matrixA(nRow,na*nb);
	CDoubleMatrix matrixB(nRow,na*nb);
	
	for( j=0; j<na*nb; j++ )
	{
		for(i=0; i<nRow; i++)
		{
			matrixA(i,j) = a(i,acols(j)-1);
		}
	}
	
	for( j=0; j<na*nb; j++ )
	{
		for(i=0; i<nRow; i++)
		{
			matrixB(i,j) = b(i,bcols(j)-1);
		}
	}
	
	//Debug 使用.
	CDoubleMatrix d = prod(matrixA,matrixB);
	
	return prod(matrixA,matrixB);	
}

/**************************************************************************** 
%	DUMMY  Creates a matrix of dummy variables for a discrete variable
%   D=IDUMMY(X,METHOD) creates an array D of dummy variables for the
%   grouping variable I (integers 1,...,g), using the method specified:
%
%   method = 1:   0/-1/1 coding, full rank
%   method = 2:   0/1 coding, full rank
%   method = 3:   0/1 coding, overdetermined
%   ytli 2002/09/13
%   ytli 测试通过 @2002/09/17.
****************************************************************************/
CDoubleMatrix CMultiAnova::idummy(CIntVector x, int method)
{
	int i,j;//for循环.
	
	int nVectorLength = x.vlen();
	
	int nMax = x(0);
	for( i=0; i<nVectorLength; i++)
	{
		if( nMax<x(i) )
			nMax = x(i);
	}
	
	int nBool = (method == 3)?0:1;
	int nCols = nMax - nBool;
	
	CDoubleMatrix DoubleMatrix(nVectorLength, nCols, 0.0);
	
	if (nMax > 1)
	{	
		// Fill in -1 for the first level
		if (method == 1)
		{			
			for(i=0; i<nVectorLength;i++)
			{
				if( x(i)==1 )
				{
					for( j=0; j<DoubleMatrix.mcol(); j++)
					{
						DoubleMatrix(i,j) = -1;
					}
				}
			}
		}
		
		// Fill in 1 in the appropriate column for other levels
		int nBool2 = (method == 3)?1:0;
		for( j=(2-nBool2); j<nMax+1; j++ )
		{	
			for( i=0; i<nVectorLength; i++)
			{
				if( x(i)==j )
				{
					DoubleMatrix(i,j-2+nBool2) = 1;
				}
			}
		}
	}
	return DoubleMatrix;
}

bool CMultiAnova::GetValueFromVO(CDataInterface * pDataInterface, std::string VO)
{
//MULTIANOVA
//	/DATA=[FILE]
//	/DEPENDENT [varlist]
//	/INDEPENDNET [varlist]
//	/COVARIATE [varlist]
//	/MODELSET [{ALL**},{COSTOM}]
//	/MODEL [vars,vars ... list](??: a1,a1 a3 aw,..., ????????)
//	/SUMTYPE [{TYPEONE},{TYPETWO},{TYPETHREE**}]
	AnaWord VOstr;
	VOstr.Import(VO);

	CTStringArray ynames;
	AnaSyntax::getVAR("/DEPENDENT", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	CTString strDependName = ynames.GetAt(0);
	m_nDependIndex = pDataInterface->IndexByName(strDependName);
	ynames.RemoveAll();

	CTStringArray x1name;
	AnaSyntax::getVAR("/INDEPENDNET", VOstr, x1name);
	if (!(pDataInterface->getVarID(x1name,m_tVarInfo)))
		return FALSE;

	CTStringArray x2name;
	AnaSyntax::getVAR("/COVARIATE", VOstr, x2name);
	if (!(pDataInterface->getVarID(x2name,m_tVarInfo1)))
		return FALSE;
	
	int totalvarnum = m_tVarInfo1.iCount+m_tVarInfo.iCount;//开始解析model行的字符串. 读出m_model.
	int i = 0;
	int j = 0;
	if (AnaSyntax::findstr("/MODELSET", "COSTOM", VOstr))
	{
		CIntVector tempIntVec(pow(2, totalvarnum), 0);
		CTStringArray xname;
		int k=0; 
		for (k=0; k<totalvarnum; k++)//把可以选择的变量名按顺序排列
		{
			if (i == m_tVarInfo.iCount)
			{
				xname.Add(x2name.GetAt(j));
				j++;
			}
			else if (j == m_tVarInfo1.iCount)
			{
				xname.Add(x1name.GetAt(i));
				i++;
			}
			else if (m_tVarInfo.GetIDAt(i) < m_tVarInfo1.GetIDAt(j))
			{
				xname.Add(x1name.GetAt(i));
				i++;
			}
			else if (m_tVarInfo1.GetIDAt(j) < m_tVarInfo.GetIDAt(i))
			{
				xname.Add(x2name.GetAt(j));
				j++;
			}
			else
				return FALSE;
		}
		CTStringArray TempName;
		int posi = AnaSyntax::FindFirstWordPos("/MODEL", VOstr);//赋值给m_model, m_model+=pow(2, index).
		j = 0;
		for (i=posi+1; i<VOstr.GetWordCount()+1; i++)
		{
			CTString tempstr = VOstr.GetAt(i).c_str();
			if (tempstr[0] == '/' || i == VOstr.GetWordCount())//读数完成,开始赋值(最后一步的, 完成之后就跳出循环)
			{
				m_model.create(j+1);
				for (k=0; k<j+1; k++)
					m_model(k) = tempIntVec(k);
				break;
			}
			if (tempstr != ",")//读数
			{
				for (k=0; k<totalvarnum; k++)
				{
					if (xname.GetAt(k) == tempstr )
					{
						tempIntVec(j) += pow(2, k);
						break;
					}
					if (k == totalvarnum-1)
						return FALSE;
				}
			}
			else
				j++;
			
		}
	}
	else
	{
		m_model.create(pow(2,totalvarnum)-1);
		for (int i=0; i<pow(2,totalvarnum)-1; i++)
			m_model(i)=i+1;
	}


	if (AnaSyntax::findstr("/SUMTYPE", "TYPEONE", VOstr))
		m_sstype = 0;
	else if (AnaSyntax::findstr("/SUMTYPE", "TYPETWO", VOstr))
		m_sstype = 1;
	else
		m_sstype = 2;

	m_strNameArrayComb.RemoveAll();
	for( i=0; i<m_tVarInfo.iCount; i++ )
	{		
		CTString strName = m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
		m_strNameArrayComb.Add(strName);
	}	
	
	for( i=0; i<m_tVarInfo1.iCount; i++ )
	{
		CTString strName = m_pDataInterface->GetFieldName(m_tVarInfo1.pValue[i]);
		m_strNameArrayComb.Add(strName);
	}
	m_strNameArrayComb.Add("修正后残差");

	return TRUE;
}

CResult* CMultiAnova::OnMultAnova(CDataInterface *pDataInterface, std::string VO)
{
	int i, j;
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	pResult=new CResult("逐步回归分析结果");
	m_pResult = pResult;

	if (!GetValueFromVO(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	
	//要求将所有的空值和缺失值所在的行全部去掉.
	TDataSet* pyDataSet=NULL;
	CPtrArray pxarrayDataSet;
	
	int nCaseNum = pDataInterface->GetColData(m_nDependIndex,pyDataSet,5);
	m_strDependentFieldName=pDataInterface->GetFieldName(m_nDependIndex);
	
	for( i=0;i<m_tVarInfo.iCount;i++)
	{
		TDataSet* pxDataSetTemp=NULL;
		pDataInterface->GetColData(m_tVarInfo.pValue[i],pxDataSetTemp,5);
		pxarrayDataSet.Add(pxDataSetTemp);
	}
	
	CTArray<int,int> arrayMissingValue;
	if( m_tVarInfo1.iCount == 0 )//没有协变量的情况.
	{
		for(i=0;i<nCaseNum;i++)
		{
			if(pyDataSet[i].iIndex<0)
			{
				arrayMissingValue.Add(i);
				continue;
			}
			for(j=0;j<m_tVarInfo.iCount;j++)
			{
				if(((TDataSet*)pxarrayDataSet[j])[i].iIndex<0)
				{
					arrayMissingValue.Add(i);
					break;
				}
			}
		}
		
		int nNewCaseNum = nCaseNum - arrayMissingValue.GetSize();
		
		CDoubleVector y(nNewCaseNum);
		CIntMatrix    group(nNewCaseNum, m_tVarInfo.iCount);
		
		bool flag = true;
		int  nCnt = 0;
		for( i=0; i<nCaseNum; i++ )
		{
			for( j=0;j<arrayMissingValue.GetSize();j++ )
			{
				if(arrayMissingValue[j]==i)
					flag = false;
			}
			if(flag==true)
			{
				y(nCnt) = pyDataSet[i].dValue;
				for( j=0;j<m_tVarInfo.iCount;j++)
				{
					group(nCnt,j) = ((TDataSet*)pxarrayDataSet[j])[i].dValue;
				}
				nCnt++;
			}
			flag = true;
		}
		
		CIntVector     terms;
		CDoubleMatrix  t;
		CDoubleVector  p;

		m_nError = manov(y,group,m_model,p,terms,m_sstype);
		m_model  = CIntVector();
		m_IntMatrix = group;
	}
	else
	{
		CPtrArray pzarrayDataSet;//对应协变量.
		for( i=0;i<m_tVarInfo1.iCount;i++)
		{
			TDataSet* pzDataSetTemp=NULL;
			pDataInterface->GetColData(m_tVarInfo1.pValue[i],pzDataSetTemp,5);
			pzarrayDataSet.Add(pzDataSetTemp);
		}

		for(i=0;i<nCaseNum;i++)
		{
			bool flag = 1;
			if(pyDataSet[i].iIndex<0)
			{
				arrayMissingValue.Add(i);
				continue;
			}
			for(j=0;j<m_tVarInfo.iCount;j++)
			{
				if(((TDataSet*)pxarrayDataSet[j])[i].iIndex<0)
				{
					arrayMissingValue.Add(i);
					flag = 0;
					break;
				}
			}
			for(j=0;j<m_tVarInfo1.iCount&&flag;j++)
			{
				if(((TDataSet*)pzarrayDataSet[j])[i].iIndex<0)
				{
					arrayMissingValue.Add(i);
					break;
				}
			}
		}

		int nNewCaseNum = nCaseNum - arrayMissingValue.GetSize();
		
		CDoubleVector y(nNewCaseNum);
		CIntMatrix    group(nNewCaseNum, m_tVarInfo.iCount);
		CDoubleMatrix z(nNewCaseNum, m_tVarInfo1.iCount);

		bool flag = true;
		int  nCnt = 0;
		for(i=0;i<nCaseNum;i++)
		{
			for( j=0;j<arrayMissingValue.GetSize();j++ )
			{
				if(arrayMissingValue[j]==i)
					flag = false;
			}
			if(flag==true)
			{
				y(nCnt) = pyDataSet[i].dValue;
				for( j=0;j<m_tVarInfo.iCount;j++)
				{
					group(nCnt,j) = ((TDataSet*)pxarrayDataSet[j])[i].dValue;
				}
				for( j=0;j<m_tVarInfo1.iCount;j++)
				{
					z(nCnt,j) = ((TDataSet*)pzarrayDataSet[j])[i].dValue;
				}
				nCnt++;
			}
			flag = true;
		}
		
		double Re;
		CDoubleVector B;
		CDoubleMatrix RXI;
		CDoubleVector SXI;
		CDoubleVector FXI;
		CDoubleVector PXI;
		CDoubleVector MXI;
		CTStringArray  Name;
		CIntVector    dfg;
		int           dfe;		
		
		m_nError = m_pCovariance->ancovan(group,y,z,
			/*输出行:*/Re, B, RXI, SXI, FXI, PXI, MXI, Name,dfg,dfe);

		m_IntMatrix = group;
			
		CDoubleMatrix tbl(MXI.vlen(),5,1.0);
		
		for( i=0; i<MXI.vlen(); i++ )
		{
			tbl(i,0) = SXI(i);
			tbl(i,1) = dfg(i);
			tbl(i,2) = MXI(i);
			if(i<MXI.vlen()-1)
			{						
				tbl(i,3) = FXI(i);
				tbl(i,4) = PXI(i);
			}
		}
		m_mtrixOutPut = tbl;	
	}
	CreateResult(pDataInterface, pResult);
	m_sstype        = 3;
	return pResult;
}


//zkc 2003.8.19
CTString CMultiAnova::ChangeToResultExplain(CTString independent,CTString dependent,double P)
{
	CTString ret;
	if(independent.Replace("*","与")>0)
	{
		independent+="之间的交互作用";
	}
	if(P<=0.05)
	{
		ret.Format("%s对%s的影响显著（概率检验值=%.4f）。",independent.GetData(),dependent.GetData(),P);
	}
	else
	{
		ret.Format("%s对%s的影响不显著（概率检验值=%.4f）。",independent.GetData(),dependent.GetData(),P);
	}
	return ret;
}

void CMultiAnova::CreateResult(CDataInterface* pDataInterface,CResult* &pResult)
{
	int i,j;
	CTLTable *pTable=NULL;
	CRsltElementTable *pETable=NULL;
	pResult=new CResult("多因子变量方差分析");//申请一个结果类

	if( m_nError != MULTIANOVA_RIGHT )
	{
		pTable = new CTLTable;
//		pTable->SetCols(1);
//		pTable->SetRows(4);

		pTable->SetTitle("警告");
		pTable->InsertColumn(0,"警告");
		m_strLastError = _T("出现了不合理的数据或样本数过多，不适合作方差分析");
		pTable->InsertItem(0,m_strLastError);	
		pTable->InsertItem(1,"程序终止了相关计算");
		pTable->InsertItem(2,"请检查输入的数据");
		pETable=new CRsltElementTable("警告",pTable);
		pResult->Add(pETable);	

		return;
	}
	
	for( i=0; i<m_tVarInfo.iCount; i++ )
	{
		pTable = new CTLTable;
		CTString strName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
//		pTable->CreateTable(pWnd);
		if( i==0 )
			pTable->SetTitle("1:因子变量");
		
		CIntVector v(m_IntMatrix.mrow());
		CIntVector v1, v2;		
		for( j=0; j<m_IntMatrix.mrow(); j++ )

		{
			v(j) = m_IntMatrix(j,i);
		}
		Fun(v,v1,v2);
		
//		pTable->SetCols(2);
//		pTable->SetRows(1+v1.vlen());

		
		pTable->InsertColumn(0,strName);
		pTable->InsertColumn(1,"频数");
	//	pTable->InsertColumn(2,"N");
		CField * pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strName);
		
		for( j=0; j<v1.vlen(); j++ )
		{
			CTString str;
			CTString labelVal;
			if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(v1(j),labelVal))
			{
				str=labelVal;
			}
			else
			{
				str.Format("%.0f",(double)v1(j));
			}
			pTable->InsertItem(j,str);
			pTable->SetItemText(j,1,v2(j));		
		}
		
		pETable=new CRsltElementTable("因子变量----"+strName,pTable);//申请一个表对象类的对象
		
		pResult->Add(pETable);		
	}
	
	if( m_tVarInfo1.iCount == 0 )//没有协方差时.
	{
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
		if (nOutNote)
		{
		
			//输入结果解释表。		
			pTable = new CTLTable;
			double P;//=m_mtrixOutPut(0)(4);
			CTString explain;//=ChangeToResultExplain(m_strNameArrayComb[0],m_strDependentFieldName,P);
			pTable->InsertColumn(0,"结果说明");//150	
			for(i=0; i<m_strNameArrayComb.GetSize()-2; i++)
			{

				P=m_mtrixOutPut(i,4);
				explain=ChangeToResultExplain(m_strNameArrayComb[i],m_strDependentFieldName,P);
				pTable->InsertItem(i,explain);
			}
			pETable=new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			pResult->InsetAt(0,pETable);
			//to end the result explain.

		}

		pTable = new CTLTable;
//		pTable->CreateTable(pWnd);
//		pTable->SetCols(5);
//		pTable->SetRows(m_strNameArrayComb.GetSize()+1);
		pTable->SetTitle("2:组间效应检验");

		pTable->InsertColumn(0,"效应来源");//150
		pTable->InsertColumn(1,"平方和");//210
		pTable->InsertColumn(2,"自由度");
		pTable->InsertColumn(3,"均方和");//120
		pTable->InsertColumn(4,"F统计量");
		pTable->InsertColumn(5,"显著性");	
		
		for(i=0; i<m_strNameArrayComb.GetSize(); i++)
		{
			pTable->InsertItem(i,m_strNameArrayComb[i]);
		}
		for(i=0;i<m_mtrixOutPut.mrow()-2;i++)
		{
			for(j=0;j<5;j++)
			{
				if(j == 1)
					pTable->SetItemText(i,j+1,(int)m_mtrixOutPut(i,j));
				else if(j==3 && m_mtrixOutPut(i,j)>10e20)
				{
					pTable->SetItemText(i,j+1,"Inf");
				}
				else if(j==4 && m_mtrixOutPut(i,0)==0 &&m_mtrixOutPut(i,1)==0 &&m_mtrixOutPut(i,2)==0 &&m_mtrixOutPut(i,3)==0 );
				else
					pTable->SetItemText(i,j+1,m_mtrixOutPut(i,j));
			}
		}
		
		if(m_mtrixOutPut.mrow()-2 < 0)
			return;
		int nTenp = m_mtrixOutPut.mrow()-2;
		pTable->SetItemText(nTenp,1,m_mtrixOutPut(nTenp,0));
		pTable->SetItemText(nTenp,2,(int)m_mtrixOutPut(nTenp,1));
		pTable->SetItemText(nTenp,3,m_mtrixOutPut(nTenp,2));
		pTable->SetItemText(nTenp,4,"");
		pTable->SetItemText(nTenp,5,"");

		nTenp++;
		pTable->SetItemText(nTenp,1,m_mtrixOutPut(nTenp,0));
		pTable->SetItemText(nTenp,2,(int)m_mtrixOutPut(nTenp,1));
		pTable->SetItemText(nTenp,3,"");
		pTable->SetItemText(nTenp,4,"");
		pTable->SetItemText(nTenp,5,"");		
		
		pETable=new CRsltElementTable("组间效应检验",pTable);//申请一个表对象类的对象
		pResult->Add(pETable);	
	}
	else//协方差
	{	
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
		if (nOutNote)
		{
			
		//输入结果解释表。		
		pTable = new CTLTable;
//		pTable->CreateTable(pWnd);
//		pTable->SetCols(5);
//		pTable->SetRows(m_strNameArrayComb.GetSize()+1);
		//pTable->SetTitle("结果说明");
		double P;//=m_mtrixOutPut(0,4);
		CTString explain;//=ChangeToResultExplain(m_strNameArrayComb[0],m_strDependentFieldName,P);
		pTable->InsertColumn(0,"结果说明");//150	
		for(i=0; i<m_strNameArrayComb.GetSize()-1; i++)
		{

			P=m_mtrixOutPut(i,4);
			explain=ChangeToResultExplain(m_strNameArrayComb[i],m_strDependentFieldName,P);
			pTable->InsertItem(i,explain);
		}
		pETable=new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
		pResult->InsetAt(0,pETable);

		//to end the result explain.
		}	
				
		pTable = new CTLTable;
		
		pTable->SetTitle("2:组间效应检验");
		
		pTable->InsertColumn(0,"效应来源");//150
		
		pTable->InsertColumn(1,"平方和");//210
		pTable->InsertColumn(2,"自由度");
		pTable->InsertColumn(3,"均方和");//120
		pTable->InsertColumn(4,"F统计量");
		pTable->InsertColumn(5,"显著性");	
				
		for(i=0; i<m_strNameArrayComb.GetSize(); i++)
		{
			pTable->InsertItem(i,m_strNameArrayComb[i]);
		}
		
		for(i=0;i<m_mtrixOutPut.mrow()-1;i++)
		{
			for(j=0;j<5;j++)
			{
				if(j == 1)
				{
					pTable->SetItemText(i,j+1,(int)m_mtrixOutPut(i,j));
				}
				else if(j==3 && m_mtrixOutPut(i,j)>10e10)
				{
					pTable->SetItemText(i,j+1,"Inf");
					pTable->SetTail("Inf: 结果太大,无效数据.");
				}
				else
				{
					pTable->SetItemText(i,j+1,m_mtrixOutPut(i,j));
				}
			}
		}
		
		if(m_mtrixOutPut.mrow()-1 < 0)
			return;
		int nTenp = m_mtrixOutPut.mrow()-1;		
		pTable->SetItemText(nTenp,1,m_mtrixOutPut(nTenp,0));
		pTable->SetItemText(nTenp,2,(int)m_mtrixOutPut(nTenp,1));
		pTable->SetItemText(nTenp,3,m_mtrixOutPut(nTenp,2));
		pTable->SetItemText(nTenp,4,"");
		pTable->SetItemText(nTenp,5,"");
		
		pETable=new CRsltElementTable("组间效应检验",pTable);//申请一个表对象类的对象
		pResult->Add(pETable);	
	}
}

void CMultiAnova::Fun(CIntVector In, CIntVector &Out1, CIntVector &Out2)
{
	int i;
	In.Sort();
	
	int nCount = 1;
	CTArray<int, int> a,b;
	for( i=1; i<In.vlen(); i++ )
	{
		if( In(i)!=In(i-1) )
		{
			a.Add( In(i-1) );
			b.Add( nCount );
			nCount = 1;
		}
		else
		{
			nCount++;
		}
	}
	a.Add( In(In.vlen()-1) );
	b.Add( nCount );
	
	CIntVector a1(a.GetSize()), b1(a.GetSize());
	for( i=0; i<a.GetSize(); i++ )
	{
		a1(i) = a[i];
		b1(i) = b[i];
	}
	Out1 = a1;
	Out2 = b1;
}

