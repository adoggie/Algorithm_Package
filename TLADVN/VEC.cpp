// VEC.cpp: implementation of the CVEC class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "VEC.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVEC::CVEC()
{
	m_tIn.m_nEquNum = 1;
	m_tIn.m_nLagnum = 2;
}

CVEC::~CVEC()
{
	
}

BOOL CVEC::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
//VECTEST
//	/DATA=[FILE]
//	/REGVAR [varlsit]
//	/EXOGENOUS [varlsit]
//	/OPTION [{NONE},{CONST},{CONLINEAR},{CONTREND},{TWICETREND}]
// [LAG({2**})] [FUNNUM({1**})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/EXOGENOUS", VOstr, ynames);
	if (!(pDataInterface->getVarID(ynames, m_tVarInfo_D)))
		return FALSE;
	CTStringArray xname;
	AnaSyntax::getVAR("/REGVAR", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo))) 
		return FALSE;
	m_tIn.m_nRegre = m_tVarInfo.iCount;				//����ģ�͵ļ�������ĸ���
	m_tIn.m_nExto = m_tVarInfo_D.iCount;

	if (AnaSyntax::findstr("/OPTION", "CONST", VOstr))
		m_tIn.m_nModel = 1;
	else if (AnaSyntax::findstr("/OPTION", "CONLINEAR", VOstr))
		m_tIn.m_nModel = 2;
	else if (AnaSyntax::findstr("/OPTION", "CONTREND", VOstr))
		m_tIn.m_nModel = 3;
	else if (AnaSyntax::findstr("/OPTION", "TWICETREND", VOstr))
		m_tIn.m_nModel = 4;
	else
		m_tIn.m_nModel = 0;

	m_tIn.m_nLagnum = 2;
	if (AnaSyntax::findstr("/OPTION", "LAG", VOstr))
		m_tIn.m_nLagnum = AnaSyntax::getvalue("/OPTION", "LAG", VOstr);
	m_tIn.m_nEquNum = 1;
	if (AnaSyntax::findstr("/OPTION", "FUNNUM", VOstr))
		m_tIn.m_nEquNum = AnaSyntax::getvalue("/OPTION", "FUNNUM", VOstr);

	return TRUE;
}

CResult* CVEC::OnRun(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	CDoubleMatrix Matrix_X, Matrix_Y;					//��������������������
	pResult=new CResult("�������ģ��");
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}

	//�������������ĸ���
	m_pResult = pResult;
	GetMatrixData(Matrix_X, Matrix_Y);			//�ӽ��������
	m_tIn.m_mxX=Matrix_X;
	m_tIn.m_mxY=Matrix_Y;
	if (VECest())							//Cvarest���������Ƿ���;��������Ϣ
	{
		DrawResult();						//������
		return pResult;
	}
	else
	{
		CTString strWarning = "�޷����㣬���������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
}

void CVEC::GetMatrixData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y)//�ӽ��������
{
	TCDataMatrix pDataSet;
	m_strVarSel.RemoveAll();						//���������������Ϣ
	m_strDepend.RemoveAll();						//����������������Ϣ
	CDWordArray indexary;
	CDWordArray indexarx;
	int i = 0 , j = 0;
	
	for( i=0; i<m_tVarInfo.iCount; i++ )			//����������������֣������ַ���
	{			
		indexary.Add(m_tVarInfo.pValue[i]);
		m_strDepend.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
	}
	
	for( j=0; j<m_tVarInfo_D.iCount; j++)			//�����������������֣������ַ���
	{
		indexarx.Add(m_tVarInfo_D.pValue[j]);
		m_strVarSel.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[j]));
	}
	
	m_pDataInterface->GetColsData( indexary, Matrix_X, 6 );//Modify by xds 20060815ȡ�ؽ�������
	if (m_tVarInfo_D.iCount!=0)
		m_pDataInterface->GetColsData( indexarx, Matrix_Y, 6 );//Modify by xds 20060815
}

bool CVEC::VECest()
{

	//���ͺ����Ϊ��������ʾ����
	if (m_tIn.m_nLagnum<0)
	{
		CTString strWarning = "ģ�����ô���������ָ���ͺ����!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	//�����ͺ��������Ϊ0�����Ϊ1
	int lag=m_tIn.m_nLagnum;
	if (m_tIn.m_nLagnum==0)
	{
		lag=1;
		m_tIn.m_nLagnum=1;
	}

	//��ʾ��Ϣ
	if (m_tIn.m_nRegre==1)
	{
		CTString strWarning = "ѡ��������٣����ʺϽ��д˷���!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	
	//ģ�����ò��ʵ�������������Ϣ
	if (m_tIn.m_nEquNum<1||m_tIn.m_nEquNum>=m_tIn.m_nRegre)
	{
		CTString strWarning = "ģ�����ò���ȷ��������ָ��Э�����̸���!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	int nXrow=m_tIn.m_mxX.mrow();
	if (nXrow-lag*m_tIn.m_nRegre-m_tIn.m_nExto-m_tIn.m_nEquNum<=0)
	{
		CTString strWarning = "�۲����ݸ���̫�٣����ʺϴ˷���!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	int nt=nXrow-lag-1;
	int nXcol = m_tIn.m_mxX.mcol();

	
//������ʼ����ͳ����
	m_tOut.m_vrSSE.create(nXcol,0);
	m_tOut.m_vrSST.create(nXcol,0);
	m_tOut.m_vrRsqu.create(nXcol,0);
	m_tOut.m_vrAdjR.create(nXcol,0);
	m_tOut.m_vrSSR.create(nXcol,0);
	m_tOut.m_vrSER.create(nXcol,0);
	m_tOut.m_vrDw.create(nXcol,0);
	m_tOut.m_vrMD.create(nXcol,0);
	m_tOut.m_vrSDD.create(nXcol,0);
	m_tOut.m_vrFtest.create(nXcol,0);
	m_tOut.m_vrDRC.create(nXcol,0);
	m_tOut.m_vrLogL.create(nXcol,0);
	m_tOut.m_vrAIC.create(nXcol,0);
	m_tOut.m_vrSC.create(nXcol,0);

	//�����µ�X�۲����
	CDoubleMatrix mxXnew;
	
	//����JJtest������������ֵ�������������Լ����ƾ���
	CDoubleMatrix mxEigVec=JJtest(m_tIn.m_mxX,m_tIn.m_mxY,m_tIn.m_nModel,lag,mxXnew);
	if (mxEigVec.mrow()==1)
	{
		return FALSE;
	}
	
	//�����µ������Իع�ģ��
	CDoubleMatrix mxXlag=m_tIn.m_mxX.slice(lag,0,nt,m_tIn.m_nRegre);
	CDoubleMatrix mxY=m_tIn.m_mxX.slice(lag+1,0,nt,m_tIn.m_nRegre);
	mxY = mxY-mxXlag;

	//����������ͳ�����
	CDoubleVector vrUnit(nt,1);
	CDoubleVector vrTrend(nt,1);
	int j;	
	for (j=0; j<nt; j++)
	{
		vrTrend(j)=j+1;
	}
	
	switch(m_tIn.m_nModel)
	{
	case 0:
		break;
	case 1:
		mxXlag=mxXlag.commat(vrUnit,1);
		break;
	case 2:
		break;
	case 3:
//		mxTrend = mxTrend+m_tIn.m_nLagnum*mxUnit;
		//		mxXlag=mxXlag.commat(mxTrend,1);
		mxXlag=mxXlag.commat(vrTrend,1);
		break;
	case 4:
		break;
	default:
		return FALSE;///////////////////////////////////////////
	return FALSE;
	}
	
	CDoubleMatrix mxW0 = mxXlag*mxEigVec;
	CDoubleMatrix mxTrend;
	mxTrend = mxTrend | vrTrend;
	CDoubleMatrix mxUnit;
	mxUnit = mxUnit | vrUnit;

	if (m_tIn.m_nModel==2||m_tIn.m_nModel==3)
	{
		CDoubleVector vrMean = -mean(mxW0);
		mxXlag = mxXlag.commat(vrUnit,1);
		mxEigVec = mxEigVec.commat(vrMean,2);
		mxW0 = mxXlag*mxEigVec;
//		TraceMatrix(mxEigVec);
	}
	else if(m_tIn.m_nModel==4)
	{
 //		mxTrend = mxTrend + (m_tIn.m_nLagnum)*mxUnit;
		CDoubleMatrix mxC1 = mxTrend.commat(mxUnit,1);
		CDoubleMatrix mxXtemp = mxC1.dot(mxC1);
		mxC1 = -mxC1.dot(mxW0);
		mxXtemp = inv(mxXtemp)*mxC1;
		mxEigVec=mxEigVec.commat(mxXtemp,2);
		mxXtemp = mxXlag.commat(vrTrend,1);
		mxXtemp = mxXtemp.commat(vrUnit,1);
		mxW0 = mxXtemp*mxEigVec;
	}
	mxXlag.destroy();
	//���������±���
	CDoubleMatrix  mxYnew;

	//��û����������
	if (m_tIn.m_mxY.mcol()!=0)
	{
		mxYnew=m_tIn.m_mxY.slice(lag,0,nt,m_tIn.m_nExto);
	}

	vrUnit.destroy();
	vrTrend.destroy();

	m_tOut.m_mxCoff = mxEigVec;

	
	CDoubleMatrix mxZ0=mxW0.commat(mxXnew,1);

	CDoubleMatrix mxZ0Z0 = mxZ0.dot(mxZ0);
	mxZ0Z0 = inv(mxZ0Z0);
	CDoubleMatrix mxBeta = mxZ0Z0*(mxZ0.dot(mxY));
	mxZ0Z0.destroy();

	CDoubleMatrix mxResidul = mxY-mxZ0*mxBeta;
	
	//��������ģ�͵�����Ŷ�ֵ

	int ndex = mxEigVec.mrow(); 
	if (m_tIn.m_nModel==3||m_tIn.m_nModel==4) 
	{
		for(j=0; j<m_tIn.m_nEquNum; j++)
			m_tOut.m_mxCoff(ndex-1,j) =m_tOut.m_mxCoff(ndex-1,j)-m_tOut.m_mxCoff(ndex-2,j)*m_tIn.m_nLagnum; 
		
	}
	
	m_tOut.m_mxCoff1 = mxBeta;
	CDoubleMatrix mxRese(m_tIn.m_nRegre,m_tIn.m_nRegre,0.0);
	for (j=0;j<m_tIn.m_nRegre;j++)
	{
		for (int k=0;k<m_tIn.m_nRegre;k++)
		{
			mxRese(j,k)=mxResidul(j).dot(mxResidul(k));
		}
	}
	
	//�����ع�����ɶ�
	int ndegree = 0;
	int ndegree1 = 0;
	switch(m_tIn.m_nModel)
	{
	case 0:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum;
		ndegree1 = ndegree*m_tIn.m_nRegre+m_tIn.m_nRegre*m_tIn.m_nEquNum;
		break;
	case 1:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum;
		ndegree1 = ndegree*m_tIn.m_nRegre+(m_tIn.m_nRegre+1)*m_tIn.m_nEquNum;
		break;
	case 2:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum+1;
		ndegree1 = ndegree*m_tIn.m_nRegre + m_tIn.m_nRegre*m_tIn.m_nEquNum;
		break;
	case 3:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum+1;
		ndegree1 = ndegree*m_tIn.m_nRegre+(m_tIn.m_nRegre+1)*m_tIn.m_nEquNum;
		break;
	case 4:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum+2;
		ndegree1 = ndegree*m_tIn.m_nRegre+m_tIn.m_nRegre*m_tIn.m_nEquNum;
		break;
	default:
		;
	}

	//Ԥ������
	if (nt-ndegree<=0)
	{
		CTString strWarning = "�۲����ݸ���̫�٣����ʺϴ˷���!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	m_tOut.m_dDRC=det((CDoubleMatrix)(mxRese/(nt*1.0)));				//����ģ�͵�DRC	
	if (m_tOut.m_dDRC<DoubleZero)
	{
		CTString strWarning = "�۲����ݸ���̫�٣����ʺϴ˷���!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	m_tOut.m_dAdjDRC=det((CDoubleMatrix)(mxRese/(nt*1.0-ndegree)));	//����ģ�͵ĵ���DRC
	
	mxRese.destroy();
	double dLp=log(m_tOut.m_dDRC);
	double dSBCtemp=dLp/m_tIn.m_nRegre;
	m_tOut.m_dSBC=(dSBCtemp)-(1.0-double(ndegree)/double(nt))*log(double(nt));//����SBC׼��
	double dFPEtem=double(nt)*(double(nt-ndegree))/(double(nt)+double(ndegree));
	m_tOut.m_dFPE=dSBCtemp-log(dFPEtem);							//����FPE׼��
	
	
	m_tOut.m_dLogl=(1.0+log(2*pi))*(-1.0)*(nt*m_tIn.m_nRegre)/2-log(m_tOut.m_dDRC)*nt/2;	//���������Ȼֵ
	//����AIC׼��
	m_tOut.m_dAIC=2.0*m_tOut.m_dLogl/(double)nt*(-1)+2.0*double(ndegree1)/double(nt);
    //����SC׼��
	m_tOut.m_dSC=2.0*m_tOut.m_dLogl/nt*(-1.0)+double(ndegree1)*log(nt)/double(nt);
	
	int nBetarow = mxBeta.mrow();
	m_tOut.m_mxSEReg.create(nBetarow,m_tIn.m_nRegre,0.0);
	m_tOut.m_mxTvalue.create(nBetarow,m_tIn.m_nRegre,0.0);

	//��ÿ�����̷ֱ����ͳ����
	for(int VARindex=0;VARindex<m_tIn.m_nRegre;VARindex++)
	{
		CDoubleMatrix mxBetai=mxBeta.slice(0,VARindex,nBetarow,1);
		CDoubleMatrix mxResi = mxResidul.slice(0,VARindex,nt,1);
		CDoubleMatrix mxYnewi = mxY.slice(0,VARindex,nt,1);
		
		//����Paratest�������д���
		
		if (!Paratest(mxBetai,mxResi,mxYnewi,mxZ0,VARindex))
		{
			return FALSE;
		}
	}	
	return TRUE;
}	



//Johansen�����Ӻ���

CDoubleMatrix CVEC::JJtest(CDoubleMatrix mxX,CDoubleMatrix mxY,int dChoice,int lag,CDoubleMatrix &mxXnew)
{
	//JJ�����Ӻ����������ؾ��������ֵ�������������Լ���������
	int n=mxX.mrow();
	int nt=n-lag-1;
	
	//���������±���
	CDoubleMatrix  mxYnew;
	if (mxY.mcol()!=0)
	{
		mxYnew=mxY.slice(lag+1,0,n-lag-1,m_tIn.m_nExto);
	}
	
	
	/////////////////////////////////////////////////////////////////////
	//																/////
	//	�������������Իع�ģ�ͣ�һ��Ϊ�ͺ��Իع飬һ��Ϊ����Իع�	/////
	//																/////
	/////////////////////////////////////////////////////////////////////	
	
	//����ع��������
	CDoubleMatrix mxXlag=mxX.slice(lag,0,n-lag-1,m_tIn.m_nRegre);
	CDoubleMatrix mxXtemp=mxX.slice(lag+1,0,n-lag-1,m_tIn.m_nRegre);
	CDoubleMatrix mxXdif=mxXtemp-mxXlag;
	
	CDoubleMatrix mxXtemp1;
	CDoubleMatrix mxXtemp2;
	
	//�����������������������
	CDoubleVector vrUnit(nt,1);
	CDoubleVector vrTrend(nt);
	int j;
	for (j=0; j<nt; j++)
	{
		vrTrend(j)=j+1;
	}
	j=1;
	//�����µ��ͺ�۲����
	while (j<=lag) 
	{
		mxXtemp1=mxX.slice(1+lag-j,0,n-lag-1,m_tIn.m_nRegre);
		mxXtemp2=mxX.slice(lag-j,0,n-lag-1,m_tIn.m_nRegre);
		mxXtemp2=mxXtemp1-mxXtemp2;
		
		if (mxXnew.mcol()==0)
		{
			mxXnew=mxXtemp2;
		}
		else
			mxXnew=mxXnew.commat(mxXtemp2,1);
		j++;
	}
	mxXtemp.destroy();
	mxXtemp1.destroy();
	mxXtemp2.destroy();
	

	//�����趨��ģ�͵Ĳ�ͬ���õ���ͬ�������Իع�ģ��
	//��ѡ�����������Чѡ��0��2��4������ѡ����ʱ����Ҫ
	//����ѡ��ı�ع�������
	switch(m_tIn.m_nModel)
	{
	case 0:
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	case 1:
		mxXlag=mxXlag.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	case 2:
		mxXnew=mxXnew.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	case 3:
		mxXlag=mxXlag.commat(vrTrend,1);
		mxXnew = mxXnew.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	case 4:
		vrTrend = vrTrend + double(m_tIn.m_nLagnum+1)*vrUnit;
		mxXnew=mxXnew.commat(vrTrend,1);
		mxXnew=mxXnew.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	default:
		return FALSE;///////////////////////////////////////////
	return FALSE;
	}
	
	vrUnit.destroy();
	vrTrend.destroy();

	//����X'x���� �� inv(x'x)����
	CDoubleMatrix mxXX=mxXnew.dot(mxXnew);
	CDoubleMatrix mxINFO(1,1,0.0);
	if (det(mxXX)<DoubleZero)
	{
		CTString strWarning = "�������죬�������趨ģ��!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return mxINFO;
	}

	CDoubleMatrix mxXXinv=inv(mxXX);
	
	CDoubleMatrix mxPI = mxXXinv*(mxXnew.dot(mxXdif));
	CDoubleMatrix mxTheta = mxXXinv*(mxXnew.dot(mxXlag));
	
	//�������������Իع�ģ�͵Ĳв����
	CDoubleMatrix mxU=mxXdif-mxXnew*mxPI;
	CDoubleMatrix mxV=mxXlag-mxXnew*mxTheta;
	CDoubleMatrix mxM;
	CDoubleMatrix mxVV;

	//����Johansen������Ȼ���ƵĹؼ�����Suu��Svv��Suv
	CDoubleMatrix mxUU=mxU.dot(mxU)/(n*1.0);
	mxVV=(mxV.dot(mxV))/(n*1.0);
	CDoubleMatrix mxUV=(mxU.dot(mxV))/(n*1.0);
	if ((det(mxUU)<DoubleZero)||(det(mxVV)<DoubleZero))
	{
		CTString strWarning = "�������죬�������趨ģ��!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return mxINFO;
	}
	
	CComplexMatrix mxEigVectComp;
	//����M����
	mxM = inv(mxVV)*(trans(mxUV))*inv(mxUU)*mxUV;	
	
	//�õ�����ֵ��������������
	CComplexMatrix mxEigValueComp=mxM.eig(mxEigVectComp);
	CDoubleMatrix mxEigVect=CDoubleMatrix(mxEigVectComp);
	CDoubleMatrix mxEigValue=CDoubleMatrix(mxEigValueComp);
	
	mxEigValueComp.destroy();
	mxEigVectComp.destroy();
	mxEigValueComp.destroy();
	int nDim = mxEigValue.mcol();
	//��������ֵ����
	CDoubleVector vrEigvalue(nDim);
	for (j=0; j< nDim; j++)
	{
		vrEigvalue(j)=mxEigValue(j,j);
	}
	
	//�������������ݶ�Ӧ������ֵ�Ĵ�С�����ս�������
	CDoubleVector vrEigvectemp(nDim,1);
	double Eigvaluetemp=0;
	for (int k=0; k< nDim; k++)
	{
		for (int l=k+1; l<nDim; l++)
		{
			if (vrEigvalue(k)<vrEigvalue(l))
			{
				Eigvaluetemp = vrEigvalue(k);
				vrEigvalue(k)=vrEigvalue(l);
				vrEigvalue(l)=Eigvaluetemp;
				
				vrEigvectemp = mxEigVect(k);
				mxEigVect(k) = mxEigVect(l);
				mxEigVect(l) = vrEigvectemp;
			}
		}
	}
	
	//�����������������滯�任����������ֵ�ı任
	CDoubleMatrix mxVECstand=mxEigVect.dot(mxEigVect);
	for (j=0; j<nDim; j++)
	{
		mxEigVect(j)=mxEigVect(j)/sqrt(mxVECstand(j,j));
	}
	
	///////////////////////////////////////
	
	//����������ʵ��Johansen���滯
	CDoubleMatrix mxTe =  mxEigVect.dot(mxEigVect);
	for (j=0; j<nDim; j++)
	{
		for (int k=0;k<nDim;k++)
		{
			mxEigVect(k,j)=mxEigVect(k,j)/sqrt(mxTe(j,j));
		}
		
	}
	mxTe =  mxEigVect.dot(mxVV)*mxEigVect;

	for (j=0; j<nDim; j++)
	{
		for (int k=0;k<nDim;k++)
		{
			mxEigVect(k,j)=mxEigVect(k,j)/sqrt(mxTe(j,j));
		}
		
	}
		
	CDoubleMatrix mxtemp=trans(mxEigVect);
	
	//����ģ�͵�Э����ϵϵ��
	mxEigVect=mkeye(mxtemp,m_tIn.m_nEquNum);
	mxEigVect=trans(mxEigVect);

	return mxEigVect;
}

//�ع鷽�̼��麯��
BOOL CVEC::Paratest(CDoubleMatrix mxBeta,CDoubleMatrix mxRes,CDoubleMatrix mxYnew,CDoubleMatrix mxXnew,int VARindex)
{
	//���Ա�������������в�ͷ�����Ŵ��룬������������ͳ�������ͷ������ͳ����
	int nN=mxXnew.mrow();
	int nB=mxXnew.mcol();
	CDoubleMatrix mxSSE=mxRes.dot(mxRes);
	CDoubleVector vrSST=var(mxYnew);
	
	//�ֱ����SSE��SSR��SST��R��adjR��F��test��SER
	m_tOut.m_vrSSE(VARindex)=mxSSE(0,0);
	m_tOut.m_vrSST(VARindex)=vrSST(0)*(nN-1);
	m_tOut.m_vrRsqu(VARindex)=1-m_tOut.m_vrSSE(VARindex)/m_tOut.m_vrSST(VARindex);
	m_tOut.m_vrAdjR(VARindex)=1-(m_tOut.m_vrSSE(VARindex)/double(nN-nB))/(m_tOut.m_vrSST(VARindex)/double(nN-1));
	m_tOut.m_vrFtest(VARindex)=m_tOut.m_vrRsqu(VARindex)/(1-m_tOut.m_vrRsqu(VARindex))/(nB-1)*(nN-nB);
	m_tOut.m_vrSSR(VARindex)=m_tOut.m_vrSST(VARindex)-m_tOut.m_vrSSE(VARindex);
	m_tOut.m_vrSER(VARindex)=sqrt(m_tOut.m_vrSSE(VARindex)/(nN-nB));
	
	//����DWͳ����
	m_tOut.m_vrDw(VARindex) = 0;
	int j;
	for (j=0; j<nN-1;j++)
	{
		m_tOut.m_vrDw(VARindex)=m_tOut.m_vrDw(VARindex) + pow(mxRes(j+1,0) - mxRes(j,0),2)/m_tOut.m_vrSSE(VARindex);
	}
	
	//����MD��SDD
	CDoubleVector meantemp=mean(mxYnew);
	m_tOut.m_vrMD(VARindex)=meantemp(0);
	m_tOut.m_vrSDD(VARindex)=sqrt(m_tOut.m_vrSST(VARindex)/(nN-1));
	
	//����ع�ϵ������ı�׼��Tֵ��Pֵ
	CDoubleMatrix mxXnewXnew = mxXnew.dot(mxXnew);
	if (det(mxXnewXnew)<DoubleZero)
	{
		CTString strWarning = "�������죬������ѡ��ģ��!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	CDoubleMatrix mxinvX= inv(mxXnewXnew);
	
	for(j=0;j<nB;j++)
	{
		double setemp=mxinvX(j,j)*m_tOut.m_vrSSE(VARindex)/(nN-nB);
		
		m_tOut.m_mxSEReg(j,VARindex)=sqrt(setemp);
		m_tOut.m_mxTvalue(j,VARindex)=mxBeta(j,0)/m_tOut.m_mxSEReg(j,VARindex);
	}
	
	//����DRC��AIC��SC׼��
	m_tOut.m_vrDRC(VARindex)=m_tOut.m_vrSSE(VARindex)/nN;
	double dT1=nN;
	m_tOut.m_vrLogL(VARindex)=(1+log(2*pi))*(-dT1)/2-dT1/2*log(m_tOut.m_vrDRC(VARindex));
	m_tOut.m_vrAIC(VARindex)=(-2)*m_tOut.m_vrLogL(VARindex)/dT1+2*nB/dT1;
	m_tOut.m_vrSC(VARindex)=(-2)*m_tOut.m_vrLogL(VARindex)/dT1+nB*log(dT1)/dT1;
	return TRUE;
}



////////////////////////////////////////////////////////////////
///////					���							////////
////////////////////////////////////////////////////////////////

void CVEC::DrawResult()
{
	//�������
								//�����ع�ϵ��������Ʊ�
	CTLTable * pTable1 = new CTLTable;
	pTable1->CreateTable(2, 2);	
	pTable1->SetTitle("Э����ϵ�������");
	pTable1->InsertColumn(0,"");
	int i;
	for ( i=0; i<m_tIn.m_nEquNum;i++)
	{
		CTString st;
		st.Format("%d",i+1);
		st="��"+st+"��Э������";
		pTable1->InsertColumn(0,st);
	}


	int number = 0;
	int m;
	for(m=0; m<m_tIn.m_nRegre; m++)
	{
		CTString str = m_strDepend[m] + "(-1)" ;
		pTable1->InsertItem(number, str);
		number++;
	}
	switch(m_tIn.m_nModel)
	{
	case 0:
		break;
	case 1:
		pTable1->InsertItem(number,"������");
		number++;
		break;
	case 2:
		pTable1->InsertItem(number,"������");
		number++;
		break;
	case 3:
		pTable1->InsertItem(number,"������");
		number++;
		pTable1->InsertItem(number,"������");
		number++;
		break;
	case 4:
		pTable1->InsertItem(number,"������");
		number++;
		pTable1->InsertItem(number,"������");
		number++;
		break;
	default:
		;
	}
	int j;
	for (j=0; j<number; j++)
	{
		for (int k=0; k<m_tIn.m_nEquNum; k++)
		{
			pTable1->SetItemText(j,k+1,m_tOut.m_mxCoff(j,k));
		}
	}

	CRsltElementTable *pETable1 = new CRsltElementTable("Э����ϵ�������", pTable1);
	m_pResult->Add(pETable1);
	

	CTLTable * pTable2 = new CTLTable;
	pTable2->CreateTable(2, 2);	
	pTable2->SetTitle("�������ģ�͹���");
	pTable2->InsertColumn(0,"");
	pTable2->InsertColumn(0,"");
	for(i=0; i<m_tIn.m_nRegre; i++)
	{
		pTable2->InsertColumn(0, m_strDepend[i]);
	}

	number=0;
	for (i=0; i<m_tIn.m_nEquNum; i++)
	{
		CTString STR;
		STR.Format("%d",i+1);
		pTable2->InsertItem(number,"Э������"+STR);
		pTable2->SetItemText(number++, 1, "ϵ��");
		
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "��׼��");

		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "Tֵ");
	}

	for(j=0; j<m_tIn.m_nLagnum; j++)						//����ϵ����׼���ϵ�������Tֵ
	{
		CTString strstr;
		strstr.Format("%d",j+1);
		for(m=0; m<m_tIn.m_nRegre; m++)
		{
			CTString str = m_strDepend[m] + "(-" + strstr + ")";
			
			pTable2->InsertItem(number, str);
			for(int k=0; k<m_tIn.m_nRegre; k++)
			{
				pTable2->SetItemText(number,k+2,"");				
			}
			pTable2->SetItemText(number++, 1, "ϵ��");
			pTable2->InsertItem(number, "");
			pTable2->SetItemText(number++, 1, "��׼��");
			pTable2->InsertItem(number, "");
			pTable2->SetItemText(number++, 1, "tֵ");
		}			
	}
	
	switch(m_tIn.m_nModel)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:	
		pTable2->InsertItem(number,"������");
		pTable2->SetItemText(number++, 1, "ϵ��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "��׼��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "tֵ");
		break;
	case 3:
		pTable2->InsertItem(number,"������");
		pTable2->SetItemText(number++, 1, "ϵ��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "��׼��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "tֵ");
		break;
	case 4:
		pTable2->InsertItem(number, "������");
		pTable2->SetItemText(number++, 1, "ϵ��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "��׼��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "tֵ");
		
		pTable2->InsertItem(number,"������");
		pTable2->SetItemText(number++, 1, "ϵ��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "��׼��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "tֵ");
		break;
	default:
		;
	}

	for(int k=0; k<(m_tIn.m_nExto); k++)
	{
		pTable2->InsertItem(number, m_strVarSel[k]);

		pTable2->SetItemText(number++, 1, "ϵ��");
		pTable2->InsertItem(number, "");

		pTable2->SetItemText(number++, 1, "��׼��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "tֵ");
	}	
	
	


	number=0;
	for (i=0; i<m_tOut.m_mxCoff1.mrow(); i++)
	{

		for (j=0; j<m_tIn.m_nRegre;j++)
		{
			pTable2->SetItemText(number,j+2,m_tOut.m_mxCoff1(i,j));
		}
		number++;
		for (j=0; j<m_tIn.m_nRegre;j++)
		{
			pTable2->SetItemText(number,j+2,m_tOut.m_mxSEReg(i,j));
		}
		number++;
		for (j=0; j<m_tIn.m_nRegre;j++)
		{
			pTable2->SetItemText(number,j+2,m_tOut.m_mxTvalue(i,j));
		}
		number++;
	}
	
		CRsltElementTable *pETable2 = new CRsltElementTable("�������ģ�͹���", pTable2);
		m_pResult->Add(pETable2);		
		
		

		
		//�������������ܽ��
		CTLTable * pTable3 = new CTLTable;
		pTable3->CreateTable(2, 2);	
		pTable3->SetTitle("���������ܽ�");
		pTable3->InsertColumn(0,"");
		for(int	n=0; n<m_tIn.m_nRegre; n++)
		{
			pTable3->InsertColumn(0, m_strDepend[n]);
		}
		
		pTable3->InsertItem(0, "�����ϵ��");
		for ( j=0; j<m_tIn.m_nRegre; j++)
			pTable3->SetItemText(0,j+1,m_tOut.m_vrRsqu(j));
		
		pTable3->InsertItem(1, "�����ĸ����ϵ��");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(1,j+1,m_tOut.m_vrAdjR(j));
		
		pTable3->InsertItem(2, "SSE");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(2,j+1,m_tOut.m_vrSSE(j));
		
		pTable3->InsertItem(3, "SST");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(3,j+1,m_tOut.m_vrSST(j));
		
		pTable3->InsertItem(4, "��ֵ");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(4,j+1,m_tOut.m_vrMD(j));
		
		pTable3->InsertItem(5, "��׼��");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(5,j+1,m_tOut.m_vrSDD(j));
		
		pTable3->InsertItem(6, "�ع��׼��");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(6,j+1,m_tOut.m_vrSER(j));
		
		pTable3->InsertItem(7, "Fͳ����");
		if (m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto>=1)
		{
			for ( j=0; j<m_tIn.m_nRegre;j++)
				pTable3->SetItemText(7,j+1,m_tOut.m_vrFtest(j));	
		}
		else
		{
			for ( j=0; j<m_tIn.m_nRegre;j++)
				pTable3->SetItemText(7,j+1,"������");
		}
		
		
		pTable3->InsertItem(8, "������Ȼֵ");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(8,j+1,m_tOut.m_vrLogL(j));
		
		pTable3->InsertItem(9, "AIC׼��");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(9,j+1,m_tOut.m_vrAIC(j));
		
		pTable3->InsertItem(10, "SC׼��");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(10,j+1,m_tOut.m_vrSC(j));
		
		CRsltElementTable *pETable3 = new CRsltElementTable("���������ܽ�", pTable3);
		m_pResult->Add(pETable3);
		
		
		
		
		//��������ģ����ͼ���׼���
		CTLTable * pTable4 = new CTLTable;
		pTable4->CreateTable(2, 2);	
		pTable4->SetTitle("��ͼ���׼��");
		pTable4->InsertColumn(0, "����׼��");
		pTable4->InsertColumn(1, "ȡֵ");
		
		pTable4->InsertItem(0, "SBC");
		pTable4->SetItemText(0,1,m_tOut.m_dSBC);
		
		pTable4->InsertItem(1, "FPE");
		pTable4->SetItemText(1,1,m_tOut.m_dFPE);
		
		pTable4->InsertItem(2, "������Ȼ(Loglikelihood)");
		pTable4->SetItemText(2,1,m_tOut.m_dLogl);
		
		pTable4->InsertItem(3, "�����в�Э����(DRC)");
		pTable4->SetItemText(3,1,m_tOut.m_dDRC);
		
		pTable4->InsertItem(4, "�����ľ����в�Э����(DRC)");
		pTable4->SetItemText(4,1,m_tOut.m_dAdjDRC);
		
		pTable4->InsertItem(5, "�����Ϣ��(AIC)");
		pTable4->SetItemText(5,1,m_tOut.m_dAIC);
		
		pTable4->InsertItem(6, "ʩ�ߴ���Ϣ��(SC)");
		pTable4->SetItemText(6,1,m_tOut.m_dSC);
		CRsltElementTable *pETable4 = new CRsltElementTable("��ͼ���׼��", pTable4);
		m_pResult->Add(pETable4);
}





















