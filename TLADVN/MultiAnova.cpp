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
����
����	
�����˲���������ݻ����������࣬���ʺ����������	
������ֹ����ؼ���	
�������������	
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//												�ظ����ظ�˫���ط������
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ytli @2002/09/09
//ytli ����ͨ�� @2002/09/18.
void CMultiAnova::Anova(CDoubleMatrix X, CDoubleVector& p, CDoubleMatrix& q, int reps)
{
	//  ��Ϊ����ľ��󲻿�������ȱʧֵ�ľ���,���Կ��Բ��ÿ���.
	//	if( any( isnan(matrix) ) )
	//		AfxMessageBox("anova���ܷ�����ȱʧֵ������!");
	
	int r = X.mcol();
	int c = X.mrow();
	
	int m = r/reps;
// 	if( m != r/reps )
// 		AfxMessageBox("���ظ�ʵ�飬reps>1��������Ӧ����reps�ı���");
	
	CDoubleMatrix Y(m, c);//��ʱm���������ӵ�ˮƽ��c��ʾ�����ӵ�ˮƽ��
	
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
				
				for(int k=nBegin; k<nBegin+reps; k++)//��ƽ����
				{
					sum += X(k,j);
				}
				Y(i,j) = sum/reps;
			}			
		}
	}
	
	CDoubleVector colmean = mean(trans(Y));//���еľ�ֵ
	CDoubleVector rowmean = mean(Y);//���еľ�ֵ
	
	double gm = mean(colmean);//���ܵ�ƽ��ֵ��
	int df1 = c-1;   //   �е����ɶ�     
	int df2 = m-1;    //  �е����ɶ�   
	
	int edf;
	int idf;
	
	if(reps == 1)
		edf = (c-1)*(r-1);
	else
	{
		edf = c*m*(reps-1);    
		idf = (c-1)*(m-1);     
	}
	
	//��SA   ��ƫ��ƽ����   
	colmean = colmean - gm;
	rowmean = rowmean - gm;
	double CSS = m*reps*dot(colmean, colmean);  
	//��SB   ��ƫ��ƽ����
	double RSS = c*reps*dot(rowmean, rowmean);
	//��ST  ��ƫ��ƽ����
	double correction = (c*m*reps)*gm*gm;
	double TSS = sum( sum( prod(X,X) ) ) - correction; 
	
	//������ظ�ʵ�� ISS����������
	double ISS = reps*sum( sum( prod(Y,Y) ) ) - correction - CSS - RSS;  
	
	double SSE;
	if( reps==1 )//����Ƿ��ظ�ʵ�� ���ƫ��ƽ��SSE��ΪISS
		SSE = ISS;  
	else//�ظ�ʵ�� �в�ƽ��SSEΪTSS - CSS - RSS - ISS
		SSE = TSS - CSS - RSS - ISS;     
    
	double ip;
	double MSE,colf,rowf,colp,rowp,intf;
	CDoubleVector vectorTemp(1);
	CDoubleVector vector1(2);
	
	if( SSE!=0 )
	{
		MSE  = SSE/edf;//������ƫ��ƽ����Ϊ0�����׼���ƫ��ƽ��MSEΪSSE/edf;�����ɶȣ�
		colf = (CSS/df1) / MSE;//SA��Fͳ����ֵ
		rowf = (RSS/df2) / MSE;//SB��Fͳ����ֵ
		colp = 1 - fcdf(colf,df1,edf); 
		rowp = 1 - fcdf(rowf,df2,edf);   //FcdfΪF�ۼƷֲ����� ������sig.
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
			colf = 1e100;// 1e100��ʾ�����
			colp = 0;
		}
		if (RSS==0)
		{
			rowf = 0;
			rowp = 1;
		}
		else    
		{
			rowf = 1e100;// 1e100��ʾ�����
			rowp = 0;
		}
		vector1(0)    = colp;	
		vector1(1)    = rowp;
		
		
		if( (reps>1) && (ISS==0) )  // ���ظ�ʵ���ҽ���ЧӦΪ0ʱ���ж�
		{
			intf = 0;		
			vectorTemp(0) = 1;
			vector1 = comvec(vector1,vectorTemp);
		}
		else if (reps>1)  
		{
			intf = 1e100;//��ʾ�����.
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
%	manov �Ƕ����ط������
%   P=MANOV (Y,GROUP,MODEL,SSTYPE,GNAME) ��GROUP �����ӱ���������Y��%   �����ط������
%   GROUP����ΪԪ�����飬��ÿһ������Ӧһ���ӱ����������������Yͬ
%   MODEL ��ʾʹ�ú���ЧӦ����
%   'linear'��1��Ӧֻ������������ЧӦ
%   'interaction'��2��Ӧֻ������������ЧӦ����������֮�佻��ЧӦ 
%   'full'��N��NΪ���Ӹ�������Ӧ������������ЧӦ����������֮�佻��ЧӦ
%   �����K��N��Ӧ������������ЧӦ����ӦKg������֮�佻��ЧӦ
%   �����MODEL ������,��Ԫ��Ϊ��ӦЧӦ����԰�����J�����ӵ�IЧӦ
%   ��BITGET(V(I),J)==1
%    [0 0 1]   1   A����ЧӦ��Ӧ��MODEL����ֵ
%    [0 1 0]   2   B����ЧӦ��Ӧ��MODEL����ֵ
%    [1 0 0]   4   C����ЧӦ
%    [0 1 1]   3   AB����ЧӦ
%    [1 1 0]   6   BC����ЧӦ
%    [1 0 1]   5   AC����ЧӦ
%    [1 1 1]   7   ABC����ЧӦ
%   SSTYPE ֵ��ȡ1��2��3 ��Ӧƫ��ƽ�����󷨣�Ĭ��Ϊ3
%   GNAMES �洢���ӱ�����
%   P ���������Ը���ֵ.
%   T���ط��������
%   TERMS
%   ytli @2002/09/13
%   ytli����ͨ��  @2002/09/24
****************************************************************************/
int CMultiAnova::manov(CDoubleVector y/*y�ĳ��ȱ����group���������*/, 
					   CIntMatrix group/*���д�ȡ*/, 
					   CIntVector model,		
					   /*�������:*/CDoubleVector& p,CIntVector& terms,
					   int sstype)
{
	assert(sstype == 1 || sstype == 2 || sstype == 3||sstype == 4);
	
//	TraceVector(y);
//	TraceVector(p);
	
	int i,j,u;//for ѭ�����õ�,��ʹ��ʱ��������0.
	
	int n   = y.vlen();//n����y���ܸ����������ɶȵ�ʱ����õ�.
	int ng  = group.mcol();//ng��������ĸ���.int rg  = group.mrow();
	int rg  = group.mrow();

    if( rg>3000)
	{;
//		return MULTIANOVA_COLERROR;
	}
	
	assert( n==group.mrow() );
	
	if( ng!=m_tVarInfo.iCount)
	{
//		AfxMessageBox("��ѡ��������������,��������ȫ����ͬ����!");
		return MULTIANOVA_COLERROR;
	}
	
//	if( ng>53 )
//		AfxMessageBox("���������53������!");	
	
	//1:  ׼�����ӷ������.
	//������������������.
	int dcode = 1;//��������dummy��������
	
	CPtrArray   gdum;
	CIntVector  dfvar(ng,0);   //�洢�������ӵ����ɶ�
	
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
		
		gdum.Add( ssss );  //��idummy���Ӻ�����Ϊÿһ���ӱ����������������.
	}
	
	
	//% 2: ����model����Ҫ������ÿһЧӦ�������������.
	int nterms = model.vlen();  //�����Ҫ��������ЧӦ������ЧӦ�ĸ���
	CIntVector sterms, sindex;  //stermsΪ���кõ�����.sindexΪ����.
	CIntVector termsort = copy(model);
	sterms = termsort.Sort(sindex);
		
	int ncols = 1;
	
	CPtrArray termdum;
	termdum.SetSize(nterms);
	CTStringArray tnames;
	tnames.SetSize( nterms+1 );
	CIntVector dfterm0(nterms,0);
	//��ÿһ��ЧӦ������Ӧ���Ƽ��洢λ��.
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
		{//% ���³����Ǹ��洢��ЧӦ������������
			abcddd++;
			double dTemp = log10(tm)/log10(2);
			if( dTemp-(int)dTemp < 0 )
				lne = (int)dTemp;
			else
				lne = 1+(int)dTemp;
			
			int bset = 0xFFFFFFFF - (int)pow(2.0,lne-1);
			tm = tm&bset;//%�������Ʊ�ʾ��tm��lneλ��0����λ���㣩
			df0 = df0 * dfvar(lne-1);
			
			// % ����ЧӦ
//			CDoubleMatrix* pDoublematrix = (CDoubleMatrix*)gdum[lne-1];
			CDoubleMatrix* pDoublematrix = (CDoubleMatrix*)gdum[lne-1];
//			TraceMatrix(*pDoublematrix);
			
			tdum = termcross(tdum, *pDoublematrix);//%�����Ӻ�������Ҫ���ڱ����佻��ЧӦ��
			
			// % ��ȡ����
			CTString strName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[lne-1]);
			if ( tn=="" )
				tn = strName;
			else
				tn = tn + "*" + strName.GetData(); 			
			
			//��SA*B��SB *A��ͬ������
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
		
		//% �洢��ЧӦ������
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
	
	//��ֹ�ڴ�й©
	for( i=0; i<gdum.GetSize(); i++ )
	{
		CDoubleMatrix* pDoubleMat1 = (CDoubleMatrix*)gdum.GetAt(i);
		if(pDoubleMat1)
			delete pDoubleMat1;	
	}
	gdum.RemoveAll();	
	
	tnames[ tnames.GetSize()-1 ] = "������в�";
	
	//% 3:  ����ȫ������
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
	
	//��ֹ�ڴ�й©
	for( i=0; i<termdum.GetSize(); i++ )
	{
		CDoubleMatrix* pp1 = (CDoubleMatrix*)termdum[i];
		
		if(pp1)
			delete pp1;
	}
	termdum.RemoveAll();
	
	//% 4:  Fit the full model and ����в�ƽ���ͣ����ȫ������
	double mu  = mean(y);
	y = y - mu;
	double mu2 = mean(y);
	mu = mu + mu2;
	
	double sst = sum((y - mu2)*(y - mu2));  //%��ƫ��ƽ���ͣ������ʾ����
	
	//% initfit���Ӻ�������в�ƫ��ƽ����
	int dfx;
	double ssx;
	
	initfit(dmat, y, termname, sst, mu, ssx, dfx); 
	
	double sse = sst - ssx;
	int dfe = n - dfx;
	
	//%  �����ú���ƫ����㷽��
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
		modw = termsnotcontained(model);//%�����Ӻ�����
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
	
	//% ��ÿ��ģ�ͼ���ƫ����ɶ�
	for( i=0; i<k.vlen(); i++ )
	{
		if(k(i)==1)
		{
			ssw(i) = ssx;//% ��Ӧȫ���ط�������õ�ssx
			dfw(i) = dfx;	
		}		
	}
	
	//%��ÿ��ģ�ͷֱ����ƫ��
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
		
		//% �ж�������Ϊ����ģ��
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
		//% ��ȡ��Ӧģ�͵���ƾ���
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
		
		//% ���ЧӦ
		CIntMatrix mod0(nterms,thismod.vlen()) ;
		CIntVector temp(modw.mrow(), 0);
		for( i=0; i<nterms; i++ )
		{
			for( j=0; j<thismod.vlen(); j++ )
			{
				mod0(i,j) = thismod(j);
			}
		}
		
		//����ssw,dfw,dfboth
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
		
		//����sswo,dfwo,dfboth
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
	
	
	//%�������ƫ��ƽ����
	CDoubleVector ssterm = ssw - sswo;
	
	CIntVector dfterm = dfw - dfwo;
	
	for( i=0; i<dfterm.vlen(); i++ )
	{
		if( dfterm(i)==0 )
			ssterm(i) = 0;//% make this exact
	}
	
	//%��������ƫ��ƽ����
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
	
	//%����Fͳ����
	CDoubleVector fstat(msterm.vlen(), 1e100);
	if (mse > 0)
		fstat = msterm / mse;
	
	//%�������ֵ
	CDoubleVector pval( fstat.vlen());
	
	for(i=0; i<dfterm.vlen(); i++)
	{
		if(dfterm(i)>0)
			pval(i) = 1 - fcdf(fstat(i), dfterm(i), dfe); //ʵ��fcdf����.
	}
	p = pval;
	
	//	% ������Ԫ���������		
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
	m_strNameArrayComb[tnames.GetSize()] = "����";
	
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
//%   ytli ����ͨ�� @2002/09/17.
CIntMatrix CMultiAnova::termsnotcontained(CIntVector t)
{
	int i,j;//forѭ��.
	
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

//%��ͬinitfit
//%ANOVANFIT Fit one model during an n-way anova calculation
//% Tolerance for computing rank from diag(R) after QR decomposition
//%Ҫ��dmat y������Ϊ����˾���.
//ytli @2002/09/13
//ytli ����ͨ��.  @2002/09/19.
void CMultiAnova::anovanfit(CDoubleMatrix dmat, CDoubleVector y, double& ssx, int& dfx)
{
	int i,j;//forѭ��.
	double TOL = 100*EPS_ACCUR *dmat.mcol();
	
	CDoubleMatrix q,r;
	CIntVector IntVector;
	r = dmat.qr(q, IntVector, 0);//QR�ֽ�.
	
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
						  /*�����:*/double& ssx, int& dfx)
{
	int i,j;//forѭ��.
	int ncols = dmat.mcol();
	double TOL = 100*EPS_ACCUR *ncols;
	
	CDoubleMatrix q,r;
	CIntVector IntVector;
	r = dmat.qr(q, IntVector, 0);//QR�ֽ�.
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
	//ֻ��Ҫ�ٴ���CTStringArray�Ϳ��������.
	CIntVector termname2(termname.vlen());
	for( i=0; i<IntVector.vlen(); i++)
	{
		termname2(i) = termname[ IntVector(i)-1 ];	
	}	
	termname = termname2;
}

//TERMCROSS Multiply dummy variables for two terms to get interaction
//   ytli ����ͨ�� @2002/09/17.
CDoubleMatrix CMultiAnova::termcross(CDoubleMatrix a, CDoubleMatrix b)
{
	int i,j;//forѭ��ʱ�õ�.
	
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
	
	//Debug ʹ��.
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
%   ytli ����ͨ�� @2002/09/17.
****************************************************************************/
CDoubleMatrix CMultiAnova::idummy(CIntVector x, int method)
{
	int i,j;//forѭ��.
	
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
	
	int totalvarnum = m_tVarInfo1.iCount+m_tVarInfo.iCount;//��ʼ����model�е��ַ���. ����m_model.
	int i = 0;
	int j = 0;
	if (AnaSyntax::findstr("/MODELSET", "COSTOM", VOstr))
	{
		CIntVector tempIntVec(pow(2, totalvarnum), 0);
		CTStringArray xname;
		int k=0; 
		for (k=0; k<totalvarnum; k++)//�ѿ���ѡ��ı�������˳������
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
		int posi = AnaSyntax::FindFirstWordPos("/MODEL", VOstr);//��ֵ��m_model, m_model+=pow(2, index).
		j = 0;
		for (i=posi+1; i<VOstr.GetWordCount()+1; i++)
		{
			CTString tempstr = VOstr.GetAt(i).c_str();
			if (tempstr[0] == '/' || i == VOstr.GetWordCount())//�������,��ʼ��ֵ(���һ����, ���֮�������ѭ��)
			{
				m_model.create(j+1);
				for (k=0; k<j+1; k++)
					m_model(k) = tempIntVec(k);
				break;
			}
			if (tempstr != ",")//����
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
	m_strNameArrayComb.Add("������в�");

	return TRUE;
}

CResult* CMultiAnova::OnMultAnova(CDataInterface *pDataInterface, std::string VO)
{
	int i, j;
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	pResult=new CResult("�𲽻ع�������");
	m_pResult = pResult;

	if (!GetValueFromVO(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	
	//Ҫ�����еĿ�ֵ��ȱʧֵ���ڵ���ȫ��ȥ��.
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
	if( m_tVarInfo1.iCount == 0 )//û��Э���������.
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
		CPtrArray pzarrayDataSet;//��ӦЭ����.
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
			/*�����:*/Re, B, RXI, SXI, FXI, PXI, MXI, Name,dfg,dfe);

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
	if(independent.Replace("*","��")>0)
	{
		independent+="֮��Ľ�������";
	}
	if(P<=0.05)
	{
		ret.Format("%s��%s��Ӱ�����������ʼ���ֵ=%.4f����",independent.GetData(),dependent.GetData(),P);
	}
	else
	{
		ret.Format("%s��%s��Ӱ�첻���������ʼ���ֵ=%.4f����",independent.GetData(),dependent.GetData(),P);
	}
	return ret;
}

void CMultiAnova::CreateResult(CDataInterface* pDataInterface,CResult* &pResult)
{
	int i,j;
	CTLTable *pTable=NULL;
	CRsltElementTable *pETable=NULL;
	pResult=new CResult("�����ӱ����������");//����һ�������

	if( m_nError != MULTIANOVA_RIGHT )
	{
		pTable = new CTLTable;
//		pTable->SetCols(1);
//		pTable->SetRows(4);

		pTable->SetTitle("����");
		pTable->InsertColumn(0,"����");
		m_strLastError = _T("�����˲���������ݻ����������࣬���ʺ����������");
		pTable->InsertItem(0,m_strLastError);	
		pTable->InsertItem(1,"������ֹ����ؼ���");
		pTable->InsertItem(2,"�������������");
		pETable=new CRsltElementTable("����",pTable);
		pResult->Add(pETable);	

		return;
	}
	
	for( i=0; i<m_tVarInfo.iCount; i++ )
	{
		pTable = new CTLTable;
		CTString strName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
//		pTable->CreateTable(pWnd);
		if( i==0 )
			pTable->SetTitle("1:���ӱ���");
		
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
		pTable->InsertColumn(1,"Ƶ��");
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
		
		pETable=new CRsltElementTable("���ӱ���----"+strName,pTable);//����һ���������Ķ���
		
		pResult->Add(pETable);		
	}
	
	if( m_tVarInfo1.iCount == 0 )//û��Э����ʱ.
	{
		int nOutNote = 0;//�Ƿ���ʾע��,0 ����ʾ,������ʾ #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
		if (nOutNote)
		{
		
			//���������ͱ�		
			pTable = new CTLTable;
			double P;//=m_mtrixOutPut(0)(4);
			CTString explain;//=ChangeToResultExplain(m_strNameArrayComb[0],m_strDependentFieldName,P);
			pTable->InsertColumn(0,"���˵��");//150	
			for(i=0; i<m_strNameArrayComb.GetSize()-2; i++)
			{

				P=m_mtrixOutPut(i,4);
				explain=ChangeToResultExplain(m_strNameArrayComb[i],m_strDependentFieldName,P);
				pTable->InsertItem(i,explain);
			}
			pETable=new CRsltElementTable("���˵��",pTable);//����һ���������Ķ���
			pResult->InsetAt(0,pETable);
			//to end the result explain.

		}

		pTable = new CTLTable;
//		pTable->CreateTable(pWnd);
//		pTable->SetCols(5);
//		pTable->SetRows(m_strNameArrayComb.GetSize()+1);
		pTable->SetTitle("2:���ЧӦ����");

		pTable->InsertColumn(0,"ЧӦ��Դ");//150
		pTable->InsertColumn(1,"ƽ����");//210
		pTable->InsertColumn(2,"���ɶ�");
		pTable->InsertColumn(3,"������");//120
		pTable->InsertColumn(4,"Fͳ����");
		pTable->InsertColumn(5,"������");	
		
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
		
		pETable=new CRsltElementTable("���ЧӦ����",pTable);//����һ���������Ķ���
		pResult->Add(pETable);	
	}
	else//Э����
	{	
		int nOutNote = 0;//�Ƿ���ʾע��,0 ����ʾ,������ʾ #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
		if (nOutNote)
		{
			
		//���������ͱ�		
		pTable = new CTLTable;
//		pTable->CreateTable(pWnd);
//		pTable->SetCols(5);
//		pTable->SetRows(m_strNameArrayComb.GetSize()+1);
		//pTable->SetTitle("���˵��");
		double P;//=m_mtrixOutPut(0,4);
		CTString explain;//=ChangeToResultExplain(m_strNameArrayComb[0],m_strDependentFieldName,P);
		pTable->InsertColumn(0,"���˵��");//150	
		for(i=0; i<m_strNameArrayComb.GetSize()-1; i++)
		{

			P=m_mtrixOutPut(i,4);
			explain=ChangeToResultExplain(m_strNameArrayComb[i],m_strDependentFieldName,P);
			pTable->InsertItem(i,explain);
		}
		pETable=new CRsltElementTable("���˵��",pTable);//����һ���������Ķ���
		pResult->InsetAt(0,pETable);

		//to end the result explain.
		}	
				
		pTable = new CTLTable;
		
		pTable->SetTitle("2:���ЧӦ����");
		
		pTable->InsertColumn(0,"ЧӦ��Դ");//150
		
		pTable->InsertColumn(1,"ƽ����");//210
		pTable->InsertColumn(2,"���ɶ�");
		pTable->InsertColumn(3,"������");//120
		pTable->InsertColumn(4,"Fͳ����");
		pTable->InsertColumn(5,"������");	
				
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
					pTable->SetTail("Inf: ���̫��,��Ч����.");
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
		
		pETable=new CRsltElementTable("���ЧӦ����",pTable);//����һ���������Ķ���
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

