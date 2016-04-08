// RootTest.cpp: implementation of the CRootTest class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "RootTest.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

///////////////////////////////////////////////////////////////
//						�㷨������							///
//						pzq 2005.12							///
///////////////////////////////////////////////////////////////

CRootTest::CRootTest()
{
	m_nTestnum = 1;
}

CRootTest::~CRootTest()
{
	
}

BOOL CRootTest::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//ROOTTEST
//	/DATA=[FILE]
//	/VARLIST [varlsit]
//	/METHOD [{AUGMENT},{PHILLIPS},{KPSS}] [{NONET},{CONSTT},{CONTREND}]
//	/OPTION [DIFF({0**})] [LAG({0**})] [TRUNCAT({0**})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo))) 
		return FALSE;
	m_nRegre = m_tVarInfo.iCount;

	if (AnaSyntax::findstr("/METHOD", "PHILLIPS", VOstr))
		m_nTestnum = 2;
	else if (AnaSyntax::findstr("/METHOD", "KPSS", VOstr))
		m_nTestnum = 3;
	else
		m_nTestnum = 1;

	if (AnaSyntax::findstr("/METHOD", "NONET", VOstr))
		m_tIn.m_nModel = 0;
	else if (AnaSyntax::findstr("/METHOD", "CONSTT", VOstr))
		m_tIn.m_nModel = 1;
	else if (AnaSyntax::findstr("/METHOD", "CONTREND", VOstr))
		m_tIn.m_nModel = 2;
	else
		return FALSE;

	m_tIn.m_nLagnum = 0;
	if (AnaSyntax::findstr("/OPTION", "LAG", VOstr))
		m_tIn.m_nLagnum = AnaSyntax::getvalue("/OPTION", "LAG", VOstr);
	m_tIn.m_nDifnum = 0;
	if (AnaSyntax::findstr("/OPTION", "DIFF", VOstr))
		m_tIn.m_nDifnum = AnaSyntax::getvalue("/OPTION", "DIFF", VOstr);
	m_tIn.m_nTruncat = 0;
	if (AnaSyntax::findstr("/OPTION", "TRUNCAT", VOstr))
		m_tIn.m_nTruncat = AnaSyntax::getvalue("/OPTION", "TRUNCAT", VOstr);

	return TRUE;
}

//����������
CResult * CRootTest::OnRun(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	pResult=new CResult("���е�λ������");
	m_pResult = pResult;
	CDoubleMatrix Matrix_X;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	GetMatrixData(Matrix_X);
	m_tIn.m_mxX = Matrix_X;
	
	///�ж����������Ƿ���ȷ
	if ((m_tIn.m_nDifnum<0) | (m_tIn.m_nLagnum<0)) 
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	if (m_nTestnum==1)    //ʹ��ADF����
	{
		
		if(ADFtest())
			DrawResult();
		return pResult;
	}
	else if (m_nTestnum==2) //ʹ��PP����
	{
		
		if (PPtest())
			DrawResult();
		return pResult;
	}
	else if (m_nTestnum==3)	//ʹ��KPSS����
	{
		
		if (KPSStest())
			DrawResult();
		return pResult;
	}
	else 
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
}

//��ý�������
void CRootTest::GetMatrixData(CDoubleMatrix & Matrix_X)
{
	m_strVarSel.RemoveAll();						//�����ع��������Ϣ
	CDWordArray indexary;
	int i;
	
	for( i=0; i<m_nRegre; i++ )						//�����ع���������֣������ַ���
	{			
		indexary.Add(m_tVarInfo.pValue[i]);
		m_strVarSel.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
	}
	
	m_pDataInterface->GetColsData( indexary, Matrix_X, 6 );//Modify by xds 20060815ȡ�ؽ�������
}

//////////////////////////////////////////////////////
//					ADF����������					//
//////////////////////////////////////////////////////
BOOL CRootTest::ADFtest()
{
	
	int nXcol=m_tIn.m_mxX.mcol();
	int nXrow=m_tIn.m_mxX.mrow();
	
	//�ж��Ƿ�ʱ�����й��̶��޷�����
	if ((m_tIn.m_nDifnum+2*m_tIn.m_nLagnum+2*m_tIn.m_nModel>=nXrow) || nXrow<4) 
	{
		CTString strWarning = "ʱ�����й��̣����ʺϽ��д˷���";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	
	//ʹ��Vdif��ֺ���������ֺ���Ա���
	CDoubleMatrix mxXIni=Vdif(m_tIn.m_mxX,m_tIn.m_nDifnum);
	nXcol=m_tIn.m_mxX.mcol();
	nXrow=mxXIni.mrow();
	
	//�����µ�ȫ���������
	m_tOut.m_vrTest.create(nXcol,0);
	m_tOut.m_mxBeta.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxSEReg.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxTvalue.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxPvalue.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	
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
	
	
	//����������ٽ�ֵ��
	CDoubleMatrix mxCriticalValue(6,3,0.0);
	double Cvalue1[6]={-1.95,-1.95,-1.95,-1.95,-1.95,-1.95};
	double Cvalue2[6]={-3,-2.93,-2.89,-2.88,-2.87,-2.86};
	double Cvalue3[6]={-3.6,-3.5,-3.45,-3.43,-3.42,-3.41};
	mxCriticalValue(0)=Cvalue1;
	mxCriticalValue(1)=Cvalue2;
	mxCriticalValue(2)=Cvalue3;
	if (nXrow-1<=25)
	{
		m_tOut.m_dTdf=mxCriticalValue(0,m_tIn.m_nModel);
	}
	else if(nXrow-1>25 && nXrow-1<=50)
	{
		m_tOut.m_dTdf=mxCriticalValue(1,m_tIn.m_nModel);
	}
	else if (nXrow-1>50 && nXrow-1<=100) 
	{
		m_tOut.m_dTdf=mxCriticalValue(2,m_tIn.m_nModel);
	}
	else if (nXrow-1>100 && nXrow-1<=250) 
	{
		m_tOut.m_dTdf=mxCriticalValue(3,m_tIn.m_nModel);
	}
	else if (nXrow-1>250 && nXrow-1<=500)
	{
		m_tOut.m_dTdf=mxCriticalValue(4,m_tIn.m_nModel);
	}
	else if (nXrow-1>500)
	{
		m_tOut.m_dTdf=mxCriticalValue(5,m_tIn.m_nModel);
	}
	
	
	// ����Ը����������м���
	int VARindex=0;
	for(VARindex=0;VARindex<nXcol;VARindex++)
	{

		//�����ʼ�ع����
		CDoubleMatrix mxXi=mxXIni.slice(0,VARindex,nXrow-1,1);
		CDoubleMatrix mxY=mxXIni.slice(1,VARindex,nXrow-1,1);
		CDoubleMatrix mxDeltaX=mxY-mxXi;
		mxY.destroy();
		
		
		if (m_tIn.m_nLagnum==0)					//�ͺ����Ϊ0ʱ�� 
		{
			CDoubleMatrix mxYnew=mxDeltaX.copy();
			mxDeltaX.destroy();

			CDoubleMatrix mxXnew=mxXi.copy();
			if (m_tIn.m_nModel==1)				//���г�����
			{
				CDoubleMatrix mxUnit(nXrow-1,1,1.0);
				mxXnew=mxXi.commat(mxUnit,1);
				mxUnit.destroy();
			}
			else if(m_tIn.m_nModel==2)			//���г������������
			{
				CDoubleMatrix mxUnit(nXrow-1,1,1.0);
				CDoubleMatrix tem(nXrow-1,1,0.0);
				int j=0 ;
				for(j=0 ; j<nXrow-1 ; j++)
					tem(j,0)=j;
				mxXnew=mxXi.commat(mxUnit,1);
				mxXnew=mxXnew.commat(tem,1);
				mxUnit.destroy();
				tem.destroy();
			}
			mxXi.destroy();

			//����ϵ�����ƾ���Beta
			CDoubleMatrix mxBeta=inv(mxXnew.dot(mxXnew))*(mxXnew.dot(mxYnew));
			CDoubleMatrix mxRes=mxYnew-mxXnew*mxBeta;
			m_tOut.m_mxBeta(VARindex)=mxBeta(0);
			
			//���ú���Paratest�����������̵�ͳ����
			if (!Paratest(mxBeta,mxRes,mxYnew,mxXnew,VARindex))
			{
				return FALSE;
			}
			mxBeta.destroy();
			mxRes.destroy();
			mxYnew.destroy();
			mxXnew.destroy();
			//�������ͳ����ֵ
			m_tOut.m_vrTest(VARindex) = m_tOut.m_mxTvalue(0,VARindex);			
		}
		else					//�ͺ������Ϊ0ʱ
		{
			CDoubleMatrix mxDeltaXt(nXrow-m_tIn.m_nLagnum-1,m_tIn.m_nLagnum,1.0);
			int j=0; 
			for (j=0; j<m_tIn.m_nLagnum;j++)
			{
				for (int k=0; k<nXrow-m_tIn.m_nLagnum-1;k++)
				{
					mxDeltaXt(k,j)=mxDeltaX(m_tIn.m_nLagnum+k-1-j,0);					
				}
			}
		//�����µĻع����
			CDoubleMatrix mxXinew=mxXi.slice(m_tIn.m_nLagnum,0,nXrow-m_tIn.m_nLagnum-1,1);
			CDoubleMatrix mxYnew=mxDeltaX.slice(m_tIn.m_nLagnum,0,nXrow-m_tIn.m_nLagnum-1,1);

			mxDeltaX.destroy();
			CDoubleMatrix mxUnit(nXrow-m_tIn.m_nLagnum-1,1,1.0);
			CDoubleMatrix tem(nXrow-1-m_tIn.m_nLagnum,1,0.0);
			CDoubleMatrix mxXnew;
			
		//����������
			for(j=0 ; j<nXrow-1-m_tIn.m_nLagnum ; j++)
				tem(j,0)=j+m_tIn.m_nLagnum+2;
			
			//�Բ�ͬ��ģ�͸�����ͬ��ѡ��
			switch(m_tIn.m_nModel)
			{
			case 0:			//����������
				mxXnew = mxXinew.commat(mxDeltaXt,1);
				break;
			case 1:			//��������
				mxXnew = mxXinew.commat(mxDeltaXt,1);
				mxXnew = mxXnew.commat(mxUnit,1);
				break;
			case 2:			//���������������
				mxXnew = mxXinew.commat(mxDeltaXt,1);
				mxXnew = mxXnew.commat(mxUnit,1);
				mxXnew = mxXnew.commat(tem,1);			
				break;
			default:
				{
					CTString strWarning = "�������ô�������������!";
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
					return FALSE;
				}
			}
			mxUnit.destroy();
			tem.destroy();

			//����ع�ϵ������Ͳв����
			CDoubleMatrix mxXnewXnew = mxXnew.dot(mxXnew);
			if (det(mxXnewXnew)<DoubleZero)
			{
				CTString strWarning = "����������죬������ѡ��ģ��!";
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
				return FALSE;
			}
			CDoubleMatrix mxBeta=inv(mxXnewXnew)*(mxXnew.dot(mxYnew));
			CDoubleMatrix mxRes=mxYnew-mxXnew*mxBeta;
			
			//��ϵ�����ƾ��󴫻�ȫ�ֱ���
			m_tOut.m_mxBeta(VARindex)=mxBeta(0);
			if (!Paratest(mxBeta,mxRes,mxYnew,mxXnew,VARindex))
			{
				return FALSE;
			}

			mxBeta.destroy();
			mxRes.destroy();
			mxYnew.destroy();
			mxXnew.destroy();
			
			//������õ���ͳ��������ȫ�ֱ���
			m_tOut.m_vrTest(VARindex) = m_tOut.m_mxTvalue(0,VARindex);
		}
	}
	return TRUE;
}


//////////////////////////////////////////////////////////
//				����PPtest��������pp����				//
//////////////////////////////////////////////////////////
BOOL CRootTest::PPtest()
{
	int nXcol=m_tIn.m_mxX.mcol();
	int nXrow=m_tIn.m_mxX.mrow();

	//�ж��Ƿ�ʱ�����й��̣�������Ӧ����ʾ
	if (m_tIn.m_nDifnum+m_tIn.m_nLagnum>=nXrow || nXrow<4) 
	{
		CTString strWarning = "ʱ�����й��̣����ʺϽ��д˷���";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	//���ò�ֺ���Vdif��������ֺ������
	CDoubleMatrix mxXIni=Vdif(m_tIn.m_mxX,m_tIn.m_nDifnum);
	nXcol=mxXIni.mcol();
	nXrow=mxXIni.mrow();
	
	//�������ȫ�ֱ���
	m_tOut.m_vrTest.create(nXcol,0.0);
	m_tOut.m_mxBeta.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxSEReg.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxTvalue.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxPvalue.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	
	m_tOut.m_vrSSE.create(nXcol,0.0);
	m_tOut.m_vrSST.create(nXcol,0.0);
	m_tOut.m_vrRsqu.create(nXcol,0.0);
	m_tOut.m_vrAdjR.create(nXcol,0.0);
	m_tOut.m_vrSSR.create(nXcol,0.0);
	m_tOut.m_vrSER.create(nXcol,0.0);
	m_tOut.m_vrDw.create(nXcol,0.0);
	m_tOut.m_vrMD.create(nXcol,0.0);
	m_tOut.m_vrSDD.create(nXcol,0.0);
	m_tOut.m_vrFtest.create(nXcol,0.0);
	m_tOut.m_vrDRC.create(nXcol,0.0);
	m_tOut.m_vrLogL.create(nXcol,0.0);
	m_tOut.m_vrAIC.create(nXcol,0.0);
	m_tOut.m_vrSC.create(nXcol,0.0);
	
	//����������ٽ�ֵ	
	CDoubleMatrix mxCriticalValue(6,3,0.0);
	double Cvalue1[6]={-1.95,-1.95,-1.95,-1.95,-1.95,-1.95};
	double Cvalue2[6]={-3,-2.93,-2.89,-2.88,-2.87,-2.86};
	double Cvalue3[6]={-3.6,-3.5,-3.45,-3.43,-3.42,-3.41};
	mxCriticalValue(0)=Cvalue1;
	mxCriticalValue(1)=Cvalue2;
	mxCriticalValue(2)=Cvalue3;
	if (nXrow-1<=25)
	{
		m_tOut.m_dTdf=mxCriticalValue(0,m_tIn.m_nModel);
	}
	else if(nXrow-1>25 && nXrow-1<=50)
	{
		m_tOut.m_dTdf=mxCriticalValue(1,m_tIn.m_nModel);
	}
	else if (nXrow-1>50 && nXrow-1<=100) 
	{
		m_tOut.m_dTdf=mxCriticalValue(2,m_tIn.m_nModel);
	}
	else if (nXrow-1>100 && nXrow-1<=250) 
	{
		m_tOut.m_dTdf=mxCriticalValue(3,m_tIn.m_nModel);
	}
	else if (nXrow-1>250 && nXrow-1<=500)
	{
		m_tOut.m_dTdf=mxCriticalValue(4,m_tIn.m_nModel);
	}
	else if (nXrow-1>500)
	{
		m_tOut.m_dTdf=mxCriticalValue(5,m_tIn.m_nModel);
	}
	
	
	//�������ÿ����������
	int VARindex=0;
	for(VARindex=0;VARindex<nXcol;VARindex++)
	{	
		CDoubleMatrix mxXnew=mxXIni.slice(0,VARindex,nXrow-1,1);
		CDoubleMatrix mxY=mxXIni.slice(1,VARindex,nXrow-1,1);
		CDoubleMatrix mxYnew=mxY-mxXnew;
		
		mxY.destroy();

		CDoubleMatrix mxUnit(nXrow-1,1,1.0);

		//����������
		CDoubleMatrix tem(nXrow-1,1,0.0);
		int j=0 ;
		for(j=0 ; j<nXrow-1 ; j++)
			tem(j,0)=j;

		//�������۸���ģ��
		if (m_tIn.m_nModel==1)
		{
			mxXnew=mxXnew.commat(mxUnit,1);
		}
		else if(m_tIn.m_nModel==2)
		{

			mxXnew=mxXnew.commat(mxUnit,1);
			tem = tem + (m_tIn.m_nDifnum+1.0)*mxUnit;
			mxXnew=mxXnew.commat(tem,1);
		}

		mxUnit.destroy();
		tem.destroy();
		
		CDoubleMatrix mxXnewXnew = mxXnew.dot(mxXnew);
		if (det(mxXnewXnew)<DoubleZero)
		{
			CTString strWarning = "����������죬������ѡ��ģ��!";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		//�õ��ع�ϵ��Beta
		CDoubleMatrix mxInvX=inv(mxXnewXnew);
		CDoubleMatrix mxBeta=mxInvX*(mxXnew.dot(mxYnew));
		
		//�õ��ع�в����
		CDoubleMatrix mxRes=mxYnew-mxXnew*mxBeta;

		m_tOut.m_mxBeta(VARindex)=mxBeta(0);
		
		//����Paratest�����õ�����ģ�͵�ͳ����
		if (!Paratest(mxBeta,mxRes,mxYnew,mxXnew,VARindex)) 
		{
			return FALSE;
		}
		
		//��������ͳ������ʹ���׵��ԡ�EviewsӦ�á��еļ��㹫ʽ
		if (m_tIn.m_nLagnum==0)		//�����ô���Ϊ0ʱ
		{
			m_tOut.m_vrTest(VARindex) = m_tOut.m_mxTvalue(0,VARindex);
			mxBeta.destroy();
			mxRes.destroy();
			mxYnew.destroy();
			mxXnew.destroy();
		}
		else						//�����ô���Ϊ0ʱ
		{
			int nB=mxXnew.mcol();
			int nXrow=mxXnew.mrow();
			
			mxBeta.destroy();
			mxYnew.destroy();
			mxXnew.destroy();

			double dStdb=m_tOut.m_mxSEReg(0,VARindex);
			double tvalue=m_tOut.m_mxTvalue(0,VARindex);
			double dGamm0=0;
			int j=0; 
			for(j=0; j<nXrow ; j++)
				dGamm0=dGamm0+mxRes(j,0)*mxRes(j,0);
			double brandwith=m_tIn.m_nLagnum;
			double dSigma=dGamm0/(nXrow-nB);
			dGamm0=dGamm0/nXrow;
			CDoubleMatrix mxGama(m_tIn.m_nTruncat,1,0.0);
			for(j=1;j<=brandwith;j++)
			{
				for(int k=j+1;k<=nXrow;k++)
				{
					mxGama(j-1,0)=mxGama(j-1,0)+mxRes(k-1,0)*mxRes(k-j-1,0);
				}
			}
			mxGama=mxGama/(nXrow*1.0);
			
			double dOmiga2=dGamm0;
			for (j=1;j<=brandwith;j++)
				dOmiga2=dOmiga2+2*mxGama(j-1,0)*(1-j/((double)brandwith+1));
			m_tOut.m_vrTest(VARindex)=sqrt(dGamm0/dOmiga2)*tvalue-(dOmiga2-dGamm0)*(nXrow)*dStdb/(2*sqrt(dOmiga2*dSigma));
		}

	}
	return TRUE;
	
}

//////////////////////////////////////////////////////////
//					����KPSS����������					//
//////////////////////////////////////////////////////////
BOOL CRootTest::KPSStest()
{
	int nXrow=m_tIn.m_mxX.mrow();
	int nXcol=m_tIn.m_mxX.mcol();
	
	//����ģ���Ƿ���̵��ж�
	if (m_tIn.m_nDifnum+m_tIn.m_nLagnum>=nXrow||nXrow<4) 
	{
		CTString strWarning = "ʱ�����й��̣����ʺϽ��д˷���";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	//��ʼ��������������
	m_tOut.m_vrTest.create(nXcol,0);
	m_tOut.m_mxBeta.create(1+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxSEReg.create(1+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxTvalue.create(1+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxPvalue.create(1+m_tIn.m_nModel,nXcol,0.0);
	
	m_tOut.m_vrSSE.create(nXcol,0.0);
	m_tOut.m_vrSST.create(nXcol,0.0);
	m_tOut.m_vrRsqu.create(nXcol,0.0);
	m_tOut.m_vrAdjR.create(nXcol,0.0);
	m_tOut.m_vrSSR.create(nXcol,0.0);
	m_tOut.m_vrSER.create(nXcol,0.0);
	m_tOut.m_vrDw.create(nXcol,0.0);
	m_tOut.m_vrMD.create(nXcol,0.0);
	m_tOut.m_vrSDD.create(nXcol,0.0);
	m_tOut.m_vrFtest.create(nXcol,0.0);
	m_tOut.m_vrDRC.create(nXcol,0.0);
	m_tOut.m_vrLogL.create(nXcol,0.0);
	m_tOut.m_vrAIC.create(nXcol,0.0);
	m_tOut.m_vrSC.create(nXcol,0.0);
	
	//����Vdif����������ֺ���Ա���
	CDoubleMatrix mxXIni=Vdif(m_tIn.m_mxX,m_tIn.m_nDifnum);
	nXrow=mxXIni.mrow();
	CDoubleMatrix mxUnit(nXrow,1,1.0);
	CDoubleMatrix tem(nXrow,1,0.0);
	int j=0 ; 
	for(j=0 ; j<nXrow ; j++)
		tem(j,0)=j;
	
	//�������ÿ�������ļ���
	int VARindex=0;
	for(VARindex=0;VARindex<m_nRegre;VARindex++)
	{
		CDoubleMatrix mxXnew;
		CDoubleMatrix mxYnew = mxXIni.slice(0,VARindex,nXrow,1);
		
		if (m_tIn.m_nModel==1)		//
		{
			mxXnew=mxUnit;
		}
		else
		{
			tem = tem + (m_tIn.m_nDifnum * 1.0)*mxUnit;
			mxXnew=mxUnit.commat(tem,1);
		}

		//����ģ�͵Ļع�ϵ������
		CDoubleMatrix mxXnewXnew = mxXnew.dot(mxXnew);
		if (det(mxXnewXnew)<DoubleZero)
		{
			CTString strWarning = "����������죬������ѡ��ģ��!";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		CDoubleMatrix mxBeta=inv(mxXnewXnew)*(mxXnew.dot(mxYnew));
		m_tOut.m_mxBeta(VARindex)=mxBeta(0);

		//�������̵Ĳв�
		CDoubleMatrix mxRes=mxYnew-mxXnew*mxBeta;
		
		//����Paratest���������������̵�ͳ����
		if (!Paratest(mxBeta,mxRes,mxYnew,mxXnew,VARindex)) 
		{
			return FALSE;
		}
		mxBeta.destroy();
		mxYnew.destroy();
		//mxXnew.destroy();

		//����ģ�͵ĵ�λ������ͳ������ʹ��Eviews�����ϵķ���
		if (m_tIn.m_nModel==1)
		{
			m_tOut.m_dTdf=0.463;
		}
		else m_tOut.m_dTdf=0.146;
		CDoubleVector vrSt(nXrow,0);
		vrSt(0)=mxRes(0,0);
		int k=1; 
		for(k=1; k<nXrow; k++)
		{

			vrSt(k)=vrSt(k-1)+mxRes(k,0);
			
		}


		CDoubleMatrix mxGama0 = mxRes.dot(mxRes);
		double dGamm0=mxGama0(0,0);
		
		if(m_tIn.m_nLagnum==0)		//������Ϊ0ʱ
		{

			double test=vrSt.dot(vrSt);
			double test1=dGamm0*nXrow;
			m_tOut.m_vrTest(VARindex)=test/test1;
		}
		else						//������Ϊ��0ʱ
		{
			double brandwith=m_tIn.m_nLagnum;
			dGamm0=dGamm0/nXrow;
			CDoubleMatrix mxGama(brandwith,1,0.0);
			//����Gama
			for(j=1;j<=brandwith;j++)
			{

				CDoubleMatrix mxtemp1 = mxRes.slice(j,0,nXrow-j,1.0);
				CDoubleMatrix mxtemp2 = mxRes.slice(0,0,nXrow-j,1.0);
				CDoubleMatrix mxtemp3 = mxtemp1.dot(mxtemp2);
				mxGama(j-1,0) = mxtemp3(0,0);
				mxtemp1.destroy();
				mxtemp2.destroy();
			}
			mxGama=mxGama/(nXrow*1.0);


			double dOmiga2=dGamm0;
			for (j=1;j<=brandwith;j++)
				dOmiga2=dOmiga2+2*mxGama(j-1,0)*(1-j/((double)brandwith+1));
			//�������ͳ����

			double dtemp =  vrSt.dot(vrSt);
			m_tOut.m_vrTest(VARindex) = dtemp/(pow(nXrow,2)*dOmiga2);

		}
		mxRes.destroy();
		
	}
	return TRUE;
}


//�ع鷽�̼��麯��
BOOL CRootTest::Paratest(CDoubleMatrix mxBeta,CDoubleMatrix mxRes,CDoubleMatrix mxYnew,CDoubleMatrix mxXnew,int VARindex)
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
	int j=0; 
	for (j=0; j<nN-1;j++)
	{
		m_tOut.m_vrDw(VARindex)=m_tOut.m_vrDw(VARindex) + pow(mxRes(j+1,0) - mxRes(j,0),2)/m_tOut.m_vrSSE(VARindex);
	}
	
	//����MD��SDD
	CDoubleVector meantemp=mean(mxYnew);
	m_tOut.m_vrMD(VARindex)=meantemp(0);
	m_tOut.m_vrSDD(VARindex)=sqrt(m_tOut.m_vrSST(VARindex)/(nN-1));
	
	//����ع�ϵ������ı�׼��Tֵ��Pֵ
	CDoubleMatrix mxinvX= inv(mxXnew.dot(mxXnew));
	for(j=0;j<nB;j++)
	{
		double setemp=mxinvX(j,j)*m_tOut.m_vrSSE(VARindex)/(nN-nB);
		m_tOut.m_mxSEReg(j,VARindex)=sqrt(setemp);
		m_tOut.m_mxTvalue(j,VARindex)=mxBeta(j,0)/m_tOut.m_mxSEReg(j,VARindex);
		m_tOut.m_mxPvalue(j,VARindex)=2*(1-tcdf(fabs(m_tOut.m_mxTvalue(j,VARindex)),nN-nB));
	}
	
	//����DRC��AIC��SC׼��
	m_tOut.m_vrDRC(VARindex)=m_tOut.m_vrSSE(VARindex)/nN;
	double dT1=nN;
	m_tOut.m_vrLogL(VARindex)=(1+log(2*pi))*(-dT1)/2-dT1/2*log(m_tOut.m_vrDRC(VARindex));
	m_tOut.m_vrAIC(VARindex)=(-2)*m_tOut.m_vrLogL(VARindex)/dT1+2*nB/dT1;
	m_tOut.m_vrSC(VARindex)=(-2)*m_tOut.m_vrLogL(VARindex)/dT1+nB*log(dT1)/dT1;
	return TRUE;
}


//��ֺ���������ԭʼ����Ͳ�ֽ���ndif��������ֺ�ľ���
CDoubleMatrix CRootTest::Vdif(CDoubleMatrix mxX,int ndif)
{
	int nN=mxX.mrow();
	int nM=mxX.mcol();
	if (ndif>0)
	{
		int nMr=mxX.mcol();
		int nNr=mxX.mrow();
		int nk=0;
		int nJ=0;
		for(int l=1;l<=ndif;l++)
		{
			nk=nN-l;
			nJ=nNr;
			CDoubleMatrix mxXd(nk,nMr);
			while (nk>0)
			{	
				for (int j=0; j<nMr; j++)
					mxXd(nk-1,j)=mxX(nJ-1,j)-mxX(nJ-2,j);
				nJ=nJ-1;nk=nk-1;
			}
			mxX=mxXd;
			nNr=mxX.mrow();
		}
	}
	return mxX;
}


//������� 
void CRootTest::DrawResult()
{
	CTLTable * pTable1 = new CTLTable;
	pTable1->CreateTable(2, 2);
	CTString strADF="Augmented Dickey-Fuller ����";
	CTString strPP="Phillips-Perron ����";
	CTString strKPSS="Kwiatkowski-Phillips-Schmidt-Shin ����";
	CTString strtemp("��λ������ͳ����");
	if (m_nTestnum==1)
	{
		strtemp=strtemp+"---"+strADF;	
		
	}else if (m_nTestnum==2)
	{
		strtemp=strtemp+"---"+strPP;
	}
	else if (m_nTestnum==3)
	{
		strtemp=strtemp+"---"+strKPSS;
	}
	pTable1->SetTitle(strtemp);
	pTable1->InsertColumn(0, "");
	
	int i=0; 
	for(i=0; i<m_nRegre; i++)
		pTable1->InsertColumn(0, m_strVarSel[i]);
	
	pTable1->InsertItem(0, "����ͳ����");
	for(i=0; i<m_nRegre; i++)
		pTable1->SetItemText(0, i+1, m_tOut.m_vrTest(i));
	TraceVector(m_tOut.m_vrTest);
	pTable1->InsertItem(1, "�ٽ�ֵ(ˮƽ5%)");
	for(i=0; i<m_nRegre; i++)
		pTable1->SetItemText(1, i+1, m_tOut.m_dTdf);
	
	CRsltElementTable *pETable1 = new CRsltElementTable(strtemp, pTable1);
	m_pResult->Add(pETable1);
	
	
	//�������������ܽ��
	CTLTable * pTable2 = new CTLTable;
	pTable2->CreateTable(2, 2);	
	pTable2->SetTitle("ϵ������");
	pTable2->InsertColumn(0,"");
	pTable2->InsertColumn(0,"");
	for(i=0; i<m_nRegre; i++)
		pTable2->InsertColumn(0, m_strVarSel[i]);
	if (m_nTestnum!=3) 
	{
		pTable2->InsertItem(0, "�ع���");
		pTable2->SetItemText(0, 1, "ϵ��");
		pTable2->InsertItem(1, "");
		pTable2->SetItemText(1, 1, "��׼��");
		pTable2->InsertItem(2, "");
		pTable2->SetItemText(2, 1, "Tֵ");
		pTable2->InsertItem(3, "");
		pTable2->SetItemText(3, 1, "Pֵ");
	}
	int number=0;
	if (m_nTestnum==1)
	{
		for(i=1; i<=m_tIn.m_nLagnum; i++)
		{
			CTString str;
			str.Format("%d", i);
			CTString strstr;
			strstr = _T("�ͺ�");
			strstr += str;
			pTable2->InsertItem(i*4, strstr);
			pTable2->InsertItem(i*4+1, "");
			pTable2->InsertItem(i*4+2, "");
			pTable2->InsertItem(i*4+3, "");
			
			pTable2->SetItemText(i*4, 1, "ϵ��");
			pTable2->SetItemText(i*4+1, 1, "��׼��");
			pTable2->SetItemText(i*4+2, 1, "Tֵ");
			pTable2->SetItemText(i*4+3, 1, "Pֵ");
		}
		number = i*4;
	}
	else if (m_nTestnum==2) 
	{
		number=4;
	} 
	
	if(m_tIn.m_nModel == 1 || m_tIn.m_nModel == 2)
	{
		pTable2->InsertItem(number, "������");
		pTable2->SetItemText(number++, 1, "ϵ��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "��׼��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "Tֵ");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "Pֵ");
	}
	if(m_tIn.m_nModel == 2)
	{
		pTable2->InsertItem(number, "������");
		pTable2->SetItemText(number++, 1, "ϵ��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "��׼��");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "Tֵ");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "Pֵ");
	}
	int nRegre=0;
	if (m_nTestnum==1)
	{
		nRegre=1+m_tIn.m_nLagnum+m_tIn.m_nModel;
	}
	else if (m_nTestnum==2)
	{
		nRegre=1+m_tIn.m_nModel;
	}
	else
		nRegre=m_tIn.m_nModel;
	
	int j=0;
	for(j=0;j<nRegre;j++)
	{
		for(int k=0; k<m_nRegre; k++)
		{			
			pTable2->SetItemText(j*4, k+2, m_tOut.m_mxBeta(j,k));
			pTable2->SetItemText(j*4+1, k+2, m_tOut.m_mxSEReg(j,k));
			pTable2->SetItemText(j*4+2, k+2, m_tOut.m_mxTvalue(j,k));
			pTable2->SetItemText(j*4+3, k+2, m_tOut.m_mxPvalue(j,k));
		}
	}
	CRsltElementTable *pETable2 = new CRsltElementTable("ϵ������", pTable2);
	m_pResult->Add(pETable2);
	
	//��������ģ����ͼ���׼���
	CTLTable * pTable3 = new CTLTable;
	pTable3->CreateTable(2, 2);	
	pTable3->SetTitle("��������ͳ����");
	pTable3->InsertColumn(0, "");
	for(i=0; i<m_nRegre; i++)
		pTable3->InsertColumn(0, m_strVarSel[i]);
	
	pTable3->InsertItem(0, "�����ϵ��");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(0,i+1,m_tOut.m_vrRsqu(i));
	
	pTable3->InsertItem(1, "�����ĸ����ϵ��");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(1,i+1,m_tOut.m_vrAdjR(i));
	
	pTable3->InsertItem(2, "SSE");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(2,i+1,m_tOut.m_vrSSE(i));
	
	pTable3->InsertItem(3, "SST");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(3,i+1,m_tOut.m_vrSST(i));
	
	pTable3->InsertItem(4, "��ֵ");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(4,i+1,m_tOut.m_vrMD(i));
	
	pTable3->InsertItem(5, "��׼��");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(5,i+1,m_tOut.m_vrSDD(i));
	
	pTable3->InsertItem(6, "�ع��׼��");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(6,i+1,m_tOut.m_vrSER(i));
	
	pTable3->InsertItem(7, "Fͳ����");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(7,i+1,m_tOut.m_vrFtest(i));
	
	pTable3->InsertItem(8, "������Ȼֵ");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(8,i+1,m_tOut.m_vrLogL(i));
	
	pTable3->InsertItem(9, "AIC׼��");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(9,i+1,m_tOut.m_vrAIC(i));
	
	pTable3->InsertItem(10, "SC׼��");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(10,i+1,m_tOut.m_vrSC(i));
	
	pTable3->InsertItem(11, "DWͳ����");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(11,i+1,m_tOut.m_vrDw(i));
	CRsltElementTable *pETable3 = new CRsltElementTable("��������ͳ����", pTable3);
	m_pResult->Add(pETable3);
}





















































































