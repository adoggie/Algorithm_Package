// RidgeAnalysize.cpp: implementation of the CRidgeAnalysize class.
//
//������:�����          ����ʱ�䣺05.10.25
//�ļ�����;����ع����
//�ļ�������޸�ʱ�䣬�޸ĵĵط���
////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "RidgeAnalysize.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const double eps=1e-15;

CRidgeAnalysize::CRidgeAnalysize()
{
// 	m_pDlg	=				NULL;
 	m_pResult	=			NULL;
	m_pDataInterface =		NULL;
	m_pRidgeModelResult	=   NULL;
	m_bOutputModel =		false;
//  	m_strVarlist = new CTStringArray();
	//	m_nkMethod	=			4;
	//	m_dkValue	=			0.5;
}

CRidgeAnalysize::~CRidgeAnalysize()
{
	
}

BOOL CRidgeAnalysize::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
//RIDGEREG
//	/DATA=[FILE]
//	/DEPENDENT [varlist]
//	/INDEPENDENT [varlist]
//	/MISSING [{DELETE},{AVE}]
//	/OPTION [NOCONST] [PLOT({1**})]
//	/SELECTK [{DEFINE({0.5**})},{HENNARD},{LAWLESS},{HBALDWIN},{LSE}]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/DEPENDENT", VOstr, ynames);
	if (!(pDataInterface->getVarID(ynames, m_tVarInfo_D)))
		return FALSE;
	if (m_tVarInfo_D.iCount != 1)
		return FALSE;
	CTStringArray xname;
	AnaSyntax::getVAR("/INDEPENDENT", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo))) 
		return FALSE;

	m_nMissingMethod = !AnaSyntax::findstr("/MISSING", "AVE", VOstr);
	m_bWithConst = !AnaSyntax::findstr("/OPTION", "NOCONST", VOstr);
	m_bWithRidgeTrace = AnaSyntax::findstr("/OPTION", "PLOT", VOstr);
	m_kMax = AnaSyntax::getvalue("/OPTION", "PLOT", VOstr);
	
	if (AnaSyntax::findstr("/SELECTK", "HENNARD",VOstr))
		m_nkMethod = 0;
	else if (AnaSyntax::findstr("/SELECTK", "LAWLESS",VOstr))
		m_nkMethod = 1;
	else if (AnaSyntax::findstr("/SELECTK", "HBALDWIN",VOstr))
		m_nkMethod = 2;
	else if (AnaSyntax::findstr("/SELECTK", "LSE",VOstr))
		m_nkMethod = 3;
	else if (AnaSyntax::findstr("/SELECTK", "DEFINE",VOstr))
	{
		m_nkMethod = 4;
		m_dkValue = AnaSyntax::getvalue("/SELECTK", "DEFINE",VOstr);
	}
	else
	{
		m_nkMethod = 4;
		m_dkValue = 0.5;
	}

	return TRUE;
}

CResult * CRidgeAnalysize::OnRidgeAnalysize(CDataInterface *pDataInterface, string VO)//���
{
	CResult * pResult;
	pResult	= new CResult("��ع����");					
	m_pResult = pResult;
	m_strVarlist.RemoveAll();
	m_pDataInterface =	pDataInterface;

	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}

	//��ȡ��Ա������ʼֵ��**����ȥ���˽��������ֱ�Ӹ�ֵ����m_pDlg.DestroyWindow()		
	m_nXCol =			m_tVarInfo.iCount;
	CDoubleMatrix Matrix_X,Matrix_y;
	//ȡ���ݷ���Matrix_X,Matrix_y
	GetMatrixData(&Matrix_X, &Matrix_y);
	if (m_nXRow < (m_nXCol-1))
	{
		CTString strWarning = "������̫�٣����������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}
	else
	{
		CDoubleMatrix		SX;   //(X-EX)'*(X-EX)
		CDoubleMatrix		Sy;   //(X-EX)'*y
		CDoubleVector		X_mean;
		CDoubleVector		y_mean;
		X_mean	=			mean(Matrix_X);
		y_mean	=			mean(Matrix_y);

		if (m_bWithConst)
		{
			//���Ļ�begin
			CDoubleVector ONES(m_nXRow,1);
			Matrix_y[0]-=y_mean(0)*ONES;
			for (int i=0;i<m_nXCol;i++)
				Matrix_X[i]-=X_mean[i]*ONES;				
			SX=Matrix_X.dot(Matrix_X);
			Sy=Matrix_X.dot(Matrix_y);
			//���Ļ�end
		}
		else
		{
			SX=Matrix_X.dot(Matrix_X);
			Sy=Matrix_X.dot(Matrix_y);
		}
		CDoubleMatrix	EigVector=SX;	
		CDoubleMatrix	EigValue=EigValPlusVec(&EigVector, 1);
		Sy=EigVector.trans()*Sy;//SY=V'*SY		
		//////////////////////////////////////////////////
		////����m_nkMethodѡ��m_dkValue///////////////////
		//////////////////////////////////////////////////
		//begin->
		switch (m_nkMethod)
		{
		case 0://����1��Hoerl��Kennard��1970��
			{
				if (EigValue(m_nXCol-1,0)<eps)
				{
					CTString	strWarning;
					strWarning.Format( "�������죬����ѡȡkֵ��" );
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
				}					
				CDoubleMatrix EigValueTem =EigValue+eps*CDoubleMatrix(m_nXCol,1,1.0); 
				CDoubleMatrix MatTem=Sy/EigValueTem;
				double sigma=GetSigma(&Matrix_X, &Matrix_y, Sy, EigValueTem, EigVector);
				m_dkValue=sigma/(CDoubleVector(MatTem(0))).Max();		
			}
			break;
		case 1://����2��Lawless��Wang��1975��
			{
				if (EigValue(m_nXCol-1,0)<eps)
				{
					CTString	strWarning;
					strWarning.Format( "�������죬����ѡȡkֵ��" );
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
				}					
				CDoubleMatrix EigValueTem =EigValue+eps*CDoubleMatrix(m_nXCol,1,1.0); 
				CDoubleMatrix MatTem=Sy/EigValueTem;					
				double sigma=GetSigma(&Matrix_X, &Matrix_y, Sy, EigValueTem, EigVector);					
				m_dkValue=(m_nXCol*sigma)/(EigValue.trans()*(MatTem.pow(2)))(0,0);				
			}
			break;
		case 2://����3��Hoerl��Baldwin��1975��
			{
				if (EigValue(m_nXCol-1,0)<eps)
				{
					CTString	strWarning;
					strWarning.Format( "�������죬����ѡȡkֵ��" );
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
				}					
				CDoubleMatrix EigValueTem =EigValue+eps*CDoubleMatrix(m_nXCol,1,1.0); 
				CDoubleMatrix MatTem=Sy/EigValueTem;					
				double sigma=GetSigma(&Matrix_X, &Matrix_y, Sy, EigValueTem, EigVector);					
				m_dkValue=m_nXCol*sigma/sum(MatTem.pow(2)(0));					
			}
			break;
		case 3:	//����4���������H(k)��С��
			{
				if (EigValue(m_nXCol-1,0)<eps)
				{
					CTString	strWarning;
					strWarning.Format( "�������죬����ѡȡkֵ��" );
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
				}					
				CDoubleMatrix EigValueTem =EigValue+eps*CDoubleMatrix(m_nXCol,1,1.0); 
				double sigma=GetSigma(&Matrix_X, &Matrix_y, Sy, EigValueTem, EigVector);
				m_dkValue=OptiFastDownward(EigValueTem, Sy, sigma,0);
			}
			break;
		case 4://ȱʡ�������Զ���
			break;
		default: //ȱʡ
			m_dkValue=0.5;
		}
		//<-end
		
		//////////////////////////////////////////////////
		////���������//////////////begin/////////////////
		//////////////////////////////////////////////////
		if ((m_dkValue==0)&&(EigValue(m_nXCol-1,0)<eps))
		{
			CTString	strWarning;
			strWarning.Format( "�������죬kֵӦ�ϸ�����㣡" );
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			EigValue+=eps*CDoubleMatrix(m_nXCol,1,1.0);
		}					
		m_MatCoeff=EigVector*(Sy/(EigValue+m_dkValue*CDoubleMatrix(m_nXCol,1,1.0)));
		if (m_bWithConst)
		{
			m_dINTERCEPT=y_mean(0)-(m_MatCoeff.trans()*X_mean)(0);
			CDoubleVector ONES(m_nXRow,1);
			CDoubleVector y_temp=Matrix_X*m_MatCoeff[0];
			m_dRSS=dot(y_temp,y_temp);
			y_temp=Matrix_y[0]-Matrix_X*m_MatCoeff[0];
			m_dESS=dot(y_temp,y_temp);
		}
		else
		{
			CDoubleVector ONES(m_nXRow,1);
			CDoubleVector y_temp=Matrix_X*m_MatCoeff[0] - y_mean(0)*ONES;
			m_dRSS=dot(y_temp,y_temp);
			y_temp=Matrix_y[0]-Matrix_X*m_MatCoeff[0];
			m_dESS=dot(y_temp,y_temp);
		}
		//�����ϵ������
		if (m_bWithConst)
		{
			m_Matrix_R=SX/sqrt(Multi(diag(SX),diag(SX)));									
		}
		else
		{	
			CDoubleVector ONES(m_nXRow,1);
			// 				CDoubleMatrix X_Tem(m_nXRow,m_nXCol,0);
			for (int i=0;i<m_nXCol;i++)
				Matrix_X[i]-=X_mean[i]*ONES;
			// 			    Matrix_X-=X_Mean;
			CDoubleMatrix MatTem=Matrix_X.trans();
			CDoubleMatrix MatTem2;
			MatTem2=MatTem*Matrix_X;
			m_Matrix_R=MatTem2/sqrt(Multi(diag(MatTem2),diag(MatTem2)));
		}
		//////////////////////////////////////////////////
		////���������//////////////end   ////////////////
		//////////////////////////////////////////////////
		//�ͷ��Ա��������������
		Matrix_X.destroy();
		Matrix_y.destroy();
		//�������������m_pResult
		CreateResult();
		if (m_bWithRidgeTrace)
		{
			CreateResultTrace(EigValue,EigVector,Sy,m_kMax);
		}
		//��ع�ṹ��ֵ
		if (m_bOutputModel)//���Ҫ�������Ա����(ģ�ͽ��)���������������
		{
			m_pRidgeModelResult->m_bWithConst=m_bWithConst;
			m_pRidgeModelResult->m_vectBeta=MatrixToVector(m_MatCoeff);
			if (m_pRidgeModelResult->m_bWithConst)
				m_pRidgeModelResult->m_vectBeta.insert(0,m_dINTERCEPT);
			m_pRidgeModelResult->m_dkValue=m_dkValue;
			m_pRidgeModelResult->m_dRSS=m_dRSS;
			m_pRidgeModelResult->m_dESS =m_dESS;
			m_pRidgeModelResult->m_Matrix_R=m_Matrix_R;
			m_pRidgeModelResult->m_nkMethod=m_nkMethod;
			m_pRidgeModelResult->m_strVarlist.Copy(m_strVarlist);
		}
	}
	return m_pResult;
}

////////////////////////////////////////////////////////////////////////////////////////
/////            �뼣����ͼ     *����ͼ      ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void CRidgeAnalysize::CreateResultTrace(CDoubleMatrix EigValue, CDoubleMatrix EigVector, CDoubleMatrix Sy, double kMax)
{  
	const int k_stepNum=50;
    int m_nRow = EigValue.mrow();
	int i=0;
	for (i=m_nRow;i>0;)
	{
		if (EigValue(i-1,0) < eps)
		{
			EigValue(i-1,0)+=eps;
			i--;
		}
		else
			i=0;
	}
	double			k_stepLen = kMax/k_stepNum;
	CDoubleMatrix	m_coeff;
 	CDoubleMatrix	ONES(m_nXCol,1,1.0);
	CDoubleMatrix	m_kEigValue;
	int k=0;
	for (k=0;k<k_stepNum+1;k++)
	{
		m_kEigValue	= EigValue+(k*k_stepLen)*ONES;
		m_coeff=m_coeff|(EigVector*(Sy/m_kEigValue));//�����Ӿ���
	}
//	double		fyMin =	DBL_MAX;
//	double		fyMax =	-DBL_MAX;
	CTChartLine*		pTChartLine;
	CRsltElementChart*	pREC;
	TDataPointValVal**	pDPBV;
	CTString*			strTemp;
	double		fxMin =	0.0;
	double		fxMax =	kMax;
	double		fyMin =	MatrixToVector(m_coeff).Min();
	double		fyMax =	MatrixToVector(m_coeff).Max();
	strTemp		=	new CTString[m_nRow];
	pDPBV		=	new TDataPointValVal*[m_nRow];
	pTChartLine =	new CTChartLine;
	for (int j=0;j<m_nRow;j++)
	{
		pDPBV[j]	=	new TDataPointValVal[k_stepNum+1];	
		for ( k=0; k < (k_stepNum+1); k++)
		{
			pDPBV[j][k].fXVal =(k)*k_stepLen;	
			pDPBV[j][k].fYVal =	m_coeff(j,k);
		}		
		strTemp[j].Format("b(%d)",j+1);
		pTChartLine->SetLine(pDPBV[j],k_stepNum+1,ValVal,strTemp[j]);
		delete[] pDPBV[j];
	}
	pTChartLine->SetXAxilLabel("�����k");
	pTChartLine->SetYAxilLabel("ϵ��b(i)");
	pTChartLine->SetTitle("�뼣����ͼ");
	pREC		=	new CRsltElementChart("��ع����ͼ",pTChartLine);
	m_pResult->Add(pREC);
	delete[] strTemp;
}
/////////////////////////////////////////////////////////////////////////////////////
//////////       ���������                     /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void CRidgeAnalysize::CreateResult()
{
	//����kֵ��Ϣ��
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("�����k��ȡֵ��Ϣ��");
	//	pTable->CreateTable(2, 2);
	pTable->SetCols(2);
	pTable->SetRows(2);
	pTable->InsertColumn(0,"����");
	pTable->InsertColumn(0,"k");
	switch (m_nkMethod)
	{
	case 0:
		pTable->InsertItem(0,"Hoerl��Kennard(1970)");
		break;
	case 1:
		pTable->InsertItem(0,"Lawless��Wang(1975)");
		break;
	case 2:
		pTable->InsertItem(0,"Hoerl��Baldwin(1975)");
		break;
	case 3:		
		pTable->InsertItem(0,"�������H(k)��С��");
		break;
	case 4:
		pTable->InsertItem(0,"�Զ���");
		break;
	default: 
		pTable->InsertItem(0,"ȱʡ");
	}
	pTable->SetItemText(0,1,m_dkValue);
	CRsltElementTable * pETable= new CRsltElementTable("�����k��ȡֵ��Ϣ��",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
	
	//�������ϵ����
	pTable = new CTLTable;
	pTable->SetTitle("�Ա������ϵ����");
//	pTable->CreateTable(2, 2);
//	pTable->SetCols(m_nXCol+1);
//	pTable->SetRows(m_nXCol);

	pTable->InsertColumn(0,"���ϵ��");
	int i=0;
	for(i=0;i<m_tVarInfo.iCount;i++)
	{
//		pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
//		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]),FALSE);
		pTable->InsertColumn(i+1,m_strVarlist.GetAt(i+1));
		pTable->InsertItem(i,m_strVarlist.GetAt(i+1),FALSE);
		
	}
	for(i=0;i<m_nXCol;i++)
	{
		for(int j=0;j<m_nXCol;j++)
		{
			pTable->SetItemText(i,j+1,m_Matrix_R(i,j));
		}
	}
	pETable= new CRsltElementTable("�Ա������ϵ����",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
	if (m_bWithConst)
	{
		//����ϵ�����Ʊ�
		pTable=new CTLTable();
// 		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(m_nXCol+2);
		pTable->SetTitle("�ع�ϵ����");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"������");
		pTable->InsertItem(0,"�ع�ϵ��b");
		for (i=0;i< m_nXCol;i++)
		{
//					pTable->InsertColumn(i+2,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			pTable->InsertColumn(i+2,m_strVarlist.GetAt(i+1));
			pTable->SetItemText(0,i+2, m_MatCoeff(i,0));
		}
		pTable->SetItemText(0,1, m_dINTERCEPT);
		pETable= new CRsltElementTable("�ع�ϵ����",pTable);
		m_pResult->Add(pETable);

		//�������������
		pTable = new CTLTable() ;
		pTable->CreateTable(2, 2);
		pTable->SetRows(4);
		pTable->SetCols(6);
		pTable->SetTitle("���������");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"ƽ����");
		pTable->InsertColumn(2,"���ɶ�");
		pTable->InsertColumn(3,"����");
		pTable->InsertColumn(4,"Fֵ");
		pTable->InsertColumn(5,"Pֵ");
		pTable->InsertItem(0,"�ع�");
		pTable->InsertItem(1,"�в�");
		pTable->InsertItem(2,"�����");
		pTable->SetItemText(0,1,(m_dRSS));
		pTable->SetItemText(1,1,(m_dESS));
		pTable->SetItemText(2,1,m_dRSS+m_dESS);
		pTable->SetItemText(0,2,m_nXCol);
		pTable->SetItemText(1,2,m_nXRow-m_nXCol-1);
		pTable->SetItemText(2,2,m_nXRow-1);
		pTable->SetItemText(0,3,m_dRSS/m_nXCol);
		pTable->SetItemText(1,3,m_dESS/(m_nXRow-m_nXCol-1));
		pTable->SetItemText(0,4,m_dRSS*(m_nXRow-m_nXCol-1)/m_dESS/m_nXCol);
		double sig=1 - fcdf(m_dRSS*(m_nXRow-m_nXCol-1)/m_dESS/m_nXCol, m_nXCol,m_nXRow-m_nXCol-1); //modified by liytao 20030813
		pTable->SetItemText(0,5,sig);
//		double temp;
//		pTable->SetItemText(0,5,1 - fdist(dfr,dfe,m_dRSS*dfe/m_dESS/dfr,temp));
		pETable= new CRsltElementTable("���������",pTable);
		m_pResult->Add(pETable);

        //����ģ�ͷ�����
		pTable = new CTLTable();
		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(2);
		pTable->SetTitle("ģ�ͷ�����");
 		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"�����ϵ��");
		pTable->InsertColumn(2,"�����ĸ����ϵ��");
		pTable->InsertItem(0,"");
//		pTable->InsertItem(0,sqrt(m_dRSS/TSS));
		pTable->SetItemText(0,1,sqrt(m_dRSS/(m_dRSS+m_dESS)));
 		pTable->SetItemText(0,2,sqrt(max(0,1-m_dESS*(m_nXRow-1)/(m_dESS+m_dRSS)/(m_nXRow-m_nXCol-1))));
 		pETable= new CRsltElementTable("ģ�ͷ�����",pTable);
		m_pResult->Add(pETable);
	}
	else
	{	
		//����ϵ�����Ʊ�
		pTable=new CTLTable();
		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(m_nXCol+1);
		pTable->SetTitle("�ع�ϵ����");
		pTable->InsertColumn(0,"");
		pTable->InsertItem(0,"�ع�ϵ��");
		for (i=0;i<m_nXCol;i++)
		{
//			pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			pTable->InsertColumn(i+1,m_strVarlist.GetAt(i+1));			
			pTable->SetItemText(0,i+1, m_MatCoeff(i,0));
		}
// 		pTable->SetItemText(0,1, INTERCEPT);
		pETable= new CRsltElementTable("�ع�ϵ����",pTable);
		m_pResult->Add(pETable);

		//�������������
		pTable = new CTLTable() ;
		pTable->CreateTable(2, 2);
		pTable->SetRows(4);
		pTable->SetCols(6);
		pTable->SetTitle("���������");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"ƽ����");
		pTable->InsertColumn(2,"���ɶ�");
		pTable->InsertColumn(3,"����");
		pTable->InsertColumn(4,"Fֵ");
		pTable->InsertColumn(5,"Pֵ");
		pTable->InsertItem(0,"�ع�");
		pTable->InsertItem(1,"�в�");
		pTable->InsertItem(2,"�����");
// 		double TSS = m_dRSS + m_dESS;
		pTable->SetItemText(0,1,(m_dRSS));
		pTable->SetItemText(1,1,(m_dESS));
		pTable->SetItemText(2,1,m_dRSS+m_dESS);
		pTable->SetItemText(0,2,m_nXCol);
		pTable->SetItemText(1,2,m_nXRow-m_nXCol);
		pTable->SetItemText(2,2,m_nXRow);
		pTable->SetItemText(0,3,m_dRSS/m_nXCol);
		pTable->SetItemText(1,3,m_dESS/(m_nXRow-m_nXCol));
		pTable->SetItemText(0,4,m_dRSS*(m_nXRow-m_nXCol)/m_dESS/m_nXCol);
		double sig=1 - fcdf(m_dRSS*(m_nXRow-m_nXCol)/m_dESS/m_nXCol, m_nXCol,m_nXRow-m_nXCol); //modified by liytao 20030813
		pTable->SetItemText(0,5,sig);
		//		double temp;
		//		pTable->SetItemText(0,5,1 - fdist(dfr,dfe,m_dRSS*dfe/m_dESS/dfr,temp));
		pETable= new CRsltElementTable("���������",pTable);
		m_pResult->Add(pETable);
        //����ģ�ͷ�����
		pTable = new CTLTable();
		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(2);
		pTable->SetTitle("ģ�ͷ�����");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"�����ϵ��");
		pTable->InsertColumn(2,"�����ĸ����ϵ��");
		pTable->InsertItem(0,"");
		//		pTable->InsertItem(0,sqrt(m_dRSS/TSS));
		pTable->SetItemText(0,1,sqrt(m_dRSS/(m_dRSS+m_dESS)));
		pTable->SetItemText(0,2,sqrt(max(0,1-m_dESS*(m_nXRow)/(m_dESS+m_dRSS)/(m_nXRow-m_nXCol))));
		pETable= new CRsltElementTable("ģ�ͷ�����",pTable);
		m_pResult->Add(pETable);
	}

	//�����ع鷽��
//	CTString strEquation=m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[0])+"=";	
	CTString strEquation=m_strVarlist.GetAt(0)+"=";	
	CTString strTemp;	
	if(m_bWithConst)
	{
		strTemp.Format("%.4f",m_dINTERCEPT);
		strEquation+=strTemp;
		for(int i=0;i<m_nXCol;i++)
		{
			if(m_MatCoeff(i,0)>0)
			{
//				strTemp.Format("+%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				strTemp.Format("+%.4f*%s",m_MatCoeff(i,0),m_strVarlist.GetAt(i+1).GetData());
				
			}
			else
			{
//				strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_strVarlist.GetAt(i+1).GetData());
			}
			strEquation+=strTemp;
		}
	}
    else
	{
		int i=0;
		for(i=0;i<m_nXCol;i++)
		{
			if (i==0)
			{
//				strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_strVarlist.GetAt(i+1).GetData());
				
			}
			else
			{
				if(m_MatCoeff(i,0)>=0)
				{
//					strTemp.Format("+%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
					strTemp.Format("+%.4f*%s",m_MatCoeff(i,0),m_strVarlist.GetAt(i+1).GetData());					
				}
				else
				{
//					strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
					strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_strVarlist.GetAt(i+1).GetData());					
				}
			}
			strEquation+=strTemp;	
		}	
	}
	pTable = new CTLTable;
	pTable->CreateTable(2, 2);	
	pTable->SetTitle("�ع鷽��");
	pTable->InsertColumn(0, strEquation);
	pETable= new CRsltElementTable("�ع鷽��", pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
}
////////////////////////////////////////////////////////////////////////////
//������:�����          ����ʱ�䣺05.10.25
//�ļ�����;�������½�����Ŀ�꺯�������Ž⣬������չ
//�ļ�������޸�ʱ�䣬�޸ĵĵط���
////////////////////////////////////////////////////////////////////////////
double		CRidgeAnalysize::OptiFastDownward(CDoubleMatrix EigValue,CDoubleMatrix Sy,double sigma,double xInit=0)
{
	//ismin=true��ʾ��С����������󻯣�����������ֻ����С��
	double			xOpti=xInit;
	double			fValOpti=0;
	double			direction;
	double stepLenOpti;
	double fValTem;
	double stepLenSub;
	double stepLenTem;
	float t;
	BOOL isContinue;
	//�����½��������Ž�
	direction=-HDeri(xOpti,Sy,EigValue,sigma);//Ŀ�꺯���ĵ��������ݶȣ������Ի�������������������optionset���β����Ŀ�꺯������������
	double m_EigValMax=CDoubleVector(EigValue(0)).Max();
	for (;(::abs(direction)>eps)&&(xOpti<m_EigValMax);)
	{
		// 		    TRACE("����%lf \n",direction);
		fValOpti=H(xOpti,Sy,EigValue,sigma);//Ŀ�꺯��ֵ�����Ի�������������������.�����������β����Ŀ�꺯��������
		//			TRACE("���ź���ֵ%lf \n",fValOpti);
		//			TRACE("���Ž�%lf \n",xOpti);
		//һά������������begin
		stepLenSub=0.5;
		stepLenOpti=0;
		isContinue=true;
		t=2;
		int flag=0;
		int count=0;
		for (count=0;isContinue && (flag < 2);)
		{
			stepLenTem=stepLenOpti+stepLenSub;
			fValTem=H(xOpti+stepLenTem*direction,Sy,EigValue,sigma);
//			TRACE("����%lf \n",stepLenTem);
			if (fValTem < fValOpti)
			{
				stepLenSub*=t;
				stepLenOpti=stepLenTem;
				fValOpti=fValTem;
				count++;
			}
			else 
			{
				if (count==0) 
				{
					stepLenSub=-stepLenSub;
					stepLenOpti=stepLenTem;
					t=1+fabs(t-1)/2;
					flag++;
				}
				else
				{
					isContinue=false;
					// stepLenOpti=(stepLenOpti+stepLenTem)/2;
				}
			}
		}
		//һά������������begin
		xOpti=xOpti+stepLenOpti*direction;
		direction=-HDeri(xOpti,Sy,EigValue,sigma);
	}
	return xOpti;
}
double		CRidgeAnalysize::H(double k, CDoubleMatrix Sy,CDoubleMatrix lambda,double sigma)//������k����
{
	
	double HVal;
 	CDoubleMatrix	ONES(m_nXCol,1,1.0);
	CDoubleMatrix	temp=Sy/lambda;
	temp=sigma*sigma*lambda+k*k*temp.pow(2);
	for (int i=0;i<2;i++)
		temp=temp/(lambda+k*ONES);
	HVal=sum(temp(0));
	return HVal;
}
double		CRidgeAnalysize::HDeri(double k, CDoubleMatrix Sy,CDoubleMatrix lambda,double sigma)
{
	double HDeriVal;
 	CDoubleMatrix	ONES(m_nXCol,1,1.0);
	CDoubleMatrix	temp=Sy/lambda;
	temp=k*temp.pow(2)-sigma*ONES;
	temp=prod(lambda,temp);
	for (int i=0;i<3;i++)
		temp=temp/(lambda+k*ONES);
	HDeriVal=2*sum(temp(0));
	return HDeriVal;
}

//////////////////////////////////////////////////////////////////
/////    ������ʽ��ʾ��ȡ����ֵ����������������*****        //////
/////    nIndex == 0 ��С�������� nIndex == 1 �Ӵ�С����  //////
/////    ��������ֵm_Row*1����m_EigVec�洢��Ӧ��������    //////
/////    ������:�����          ����ʱ�䣺05.10.25          //////
/////    �ļ�������޸�ʱ�䣬�޸ĵĵط���                   //////
//////////////////////////////////////////////////////////////////
CDoubleMatrix CRidgeAnalysize::EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex)
{
	int m_Row=(*m_EigVec).mrow();
	if (m_Row!=(*m_EigVec).mcol())
	{
		return NULL;
	}
	else
	{
		CDoubleVector	Vect_Data = MatrixToVector(*m_EigVec);
		CDoubleVector	m_VectEigVector(m_Row*m_Row);
		cjcbj(Vect_Data, m_Row, m_VectEigVector, eps);
		SortEigenVect(Vect_Data, m_VectEigVector, m_Row, m_Row, nIndex);
		CDoubleMatrix	EigValue(m_Row,1);
		for( int i= 0; i < m_Row; i++)
		{
			EigValue(i,0) = Vect_Data(i+i*m_Row);
		}
		//�������������ÿһ�ж�Ӧһ������ֵ
		*m_EigVec = VectorToMatrix(m_VectEigVector,m_Row,m_Row).trans();
		return EigValue;
	}
}

///////////////////////////////////////////////////////////////////
/////////����ȱʧֵ���������ݷ������Matrix_X&Matrix_y
//////////////////////////////////////////////////////////////////
void CRidgeAnalysize::GetMatrixData(CDoubleMatrix* Matrix_X, CDoubleMatrix* Matrix_y)
{
	if (m_nMissingMethod==0)
	{
		//����ȱʧֵ�����ֵ
		TCDataMatrix  pDataSet;
		if( m_nXCol >0 )
		{
			CDWordArray indexary;
			//TDataSet * pDataArray = NULL;
			indexary.Add(m_tVarInfo_D.pValue[0]);
			m_strVarlist.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[0]));				
			for( int i=0; i<m_nXCol; i++ )
			{			
				indexary.Add(m_tVarInfo.pValue[i]);
				m_strVarlist.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			}
			m_pDataInterface->GetColsData( indexary, pDataSet, 3 );//ȡ������
		}
		//ת���ɾ��󣬴���Matrix_X,Matrix_y
		m_nXRow =pDataSet[0]->GetLen();
		(*Matrix_y)=CDoubleMatrix(m_nXRow,1,0.0);
		(*Matrix_X)=CDoubleMatrix(m_nXRow, m_nXCol,0.0);
		int i=0;
		for( i=0; i<m_nXRow; i++ )
		{
			//ȡ��������ݷ������Matrix_y
			(*Matrix_y)(i,0) = pDataSet[0]->GetAt(i).dValue;
			int j=0;
			for(j=0;j<m_nXCol;j++)
			{
				//ȡ�Ա������ݷ������Matrix_X
				(*Matrix_X)(i,j) = pDataSet[j+1]->GetAt(i).dValue;
			}

		}
		//�ͷ��ڴ�
		for( i =0;i<pDataSet.size();i++ )
		{
			delete pDataSet[i];
		}		
		pDataSet.clear();
	}
	else
	{
		//����ȱʧֵ������ɾ��
		TDColAry* pyDataSet=new TDColAry();
		//CPtrArray* pxarrayDataSet;
		TCDataMatrix  pxarrayDataSet;
		int nCaseNum = m_pDataInterface->GetColData(m_tVarInfo_D.pValue[0],*pyDataSet,5);
		m_strVarlist.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[0]));				
		if( m_nXCol>0 )
		{
			CDWordArray indexary;
			for( int i=0; i<m_nXCol; i++ )
			{			
				indexary.Add(m_tVarInfo.pValue[i]);
				m_strVarlist.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));					
			}
			m_pDataInterface->GetColsData( indexary, pxarrayDataSet, 5 );//ȡ������
		}

		vector<int> arrayMissingValue;
		int i=0;
		for(i=0;i<nCaseNum;i++)
		{
			if(pyDataSet->GetAt(i).iIndex<0)
			{
				arrayMissingValue.push_back(i);
				continue;
			}
			for(int j=0;j<m_nXCol;j++)
			{
				if(pxarrayDataSet[j]->GetAt(i).iIndex<0)
				{
					arrayMissingValue.push_back(i);
					break;
				}
			}
		}
		//ת���ɾ��󣬴���Matrix_X,Matrix_y
		m_nXRow= nCaseNum - arrayMissingValue.size();
		(*Matrix_y)=CDoubleMatrix(m_nXRow,1,0.0);
		(*Matrix_X)=CDoubleMatrix(m_nXRow, m_nXCol,0.0);
		BOOL Flag = TRUE;
		int  nCnt = 0;
		for( i=0; i<nCaseNum; i++ )
		{
			int j=0;
			for(j=0;j<arrayMissingValue.size();j++ )
			{
				if(arrayMissingValue[j]==i)
					Flag = FALSE;
			}
			if(Flag==TRUE)
			{
				//ȡ��������ݷ������Matrix_y
				(*Matrix_y)(nCnt,0) = pyDataSet->GetAt(i).dValue;
				for( j=0;j<m_nXCol;j++)
				{
					//ȡ�Ա������ݷ������Matrix_X
					(*Matrix_X)(nCnt,j) = pxarrayDataSet[j]->GetAt(i).dValue;
				}
				nCnt++;
			}
			Flag = TRUE;
		}
		//�ͷ��ڴ�
		for( i =0;i<pxarrayDataSet.size();i++ )
		{
			 delete pxarrayDataSet[i];
		}		    
		pxarrayDataSet.clear();
		delete pyDataSet;
	}
}
///////////////////////////////////////////////////////
////////////////////����ƽ����sigma////////////////////
///////////////////////////////////////////////////////
double CRidgeAnalysize::GetSigma(CDoubleMatrix* Matrix_X,CDoubleMatrix* Matrix_y,CDoubleMatrix Sy,CDoubleMatrix EigValue, CDoubleMatrix EigVector)
{
	double sigma;
	CDoubleMatrix BWithoutk;
	BWithoutk=EigVector*(Sy/EigValue);
	CDoubleVector y_temp=(*Matrix_y)[0]-(*Matrix_X)*BWithoutk[0];
	sigma=dot(y_temp,y_temp);
	return sigma;
}
/*
////////////////////////////////////////////////////////////////////////////
//������:�����          ����ʱ�䣺05.10.25
//��������;��ר���������ת�Ⱥ���ˣ�Z=X'*Y
//�ļ�������޸�ʱ�䣬�޸ĵĵط���
////////////////////////////////////////////////////////////////////////////
CDoubleMatrix CRidgeAnalysize::TranProd(CDoubleMatrix &X, CDoubleMatrix &Y)
{
	//int m_nRowX=X.mrow();
	int m_nColX=X.mcol();
	int m_nColY=Y.mcol();
	CDoubleMatrix Z(m_nColX,m_nColY);
	if (X.mrow()!=Y.mrow())
		AfxMessageBox("����ά����ƥ�䣡");
	else
		for (int i=0;i<m_nColX;i++)
			for (int j=0;j<m_nColY;j++)
				Z(i,j)=X[i].dot(Y[j]);
			return Z;
}
*/
//int CRidgeAnalysize::GetCurSec()//��ȡ��ǰʱ��
//{
//		time_t time0;
//// 		struct _timeb tstruct;
//		struct tm *pNow;
//		_tzset();
//		time( &time0 );
//		
////		_ftime( &tstruct );
//		pNow = localtime( &time0 );
//// 		TRACE( "Current Time:\t\t\t%.8s", 	asctime( pNow )+11 );
//		return pNow->tm_sec+pNow->tm_min*60+pNow->tm_hour*3600;
////	    TRACE( ":%u\n", tstruct.millitm );

