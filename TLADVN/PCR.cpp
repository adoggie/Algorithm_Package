// PCR.cpp: implementation of the CPrinCompReg class.
//
//������:�����          ����ʱ�䣺05.10.25
//�ļ�����;�����ɷֻع����
//�ļ�������޸�ʱ�䣬�޸ĵĵط���
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "PCR.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const double eps=0.0000000001;
CPrinCompReg::CPrinCompReg()
{
	m_pPCRResult =		NULL;
	m_bOutputModel =	false;
	m_pResult =			NULL;
}

CPrinCompReg::~CPrinCompReg()
{
	
}

BOOL CPrinCompReg::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//PCREG
//	/DATA=[FILE]
//	/DEPENDENT [varlist]
//	/INDEPENDENT [varlist]
//	/MISSING [{DELETE**},{AVE}]
//	/OPTION [NOCONST] [NOPRINTPC]
//	/METHOD [{PCNUM({2**})},{CUM({0.8**})},{SINGLE({0.01**})},{OTHERS}]
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
	m_bWithPC = !AnaSyntax::findstr("/OPTION", "NOPRINTPC", VOstr);
	
	//��ȡȡ�����ķ���	
	if (AnaSyntax::findstr("/METHOD", "PCNUM",VOstr))
	{
		m_ncMethod = 0;
		m_ncValue = AnaSyntax::getvalue("/METHOD", "PCNUM",VOstr);
	}
	else if (AnaSyntax::findstr("/METHOD", "CUM",VOstr))
	{
		m_ncMethod = 1;
		m_dcValueCalContr = AnaSyntax::getvalue("/METHOD", "CUM",VOstr);
	}
	else if (AnaSyntax::findstr("/METHOD", "SINGLE",VOstr))
	{
		m_ncMethod = 2;
		m_dcValueCorrEigVal = AnaSyntax::getvalue("/METHOD", "SINGLE",VOstr);
	}
	else if (AnaSyntax::findstr("/METHOD", "OTHERS",VOstr))
		m_ncMethod = 3;
	else
		return FALSE;
	
	return TRUE;
}

//********************************************************************��������ʼ
CResult	*CPrinCompReg::OnPCR(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface =	pDataInterface;
	CDoubleMatrix Matrix_X, Matrix_y;
	CResult * pResult;
	pResult	= new CResult("���ɷֻع����");		
	m_pResult = pResult;
	
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	m_nXCol = m_tVarInfo.iCount;
	GetMatrixData(&Matrix_X, &Matrix_y);//ȡ���ݣ��������Matrix_X&Matrix_y
	if (m_nXRow < (m_nXCol-1))
	{
		CTString strWarning = "������̫�٣����������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��!" );
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
			SX=Matrix_X.dot(Matrix_X);//SX=X'*X
			Sy=Matrix_X.dot(Matrix_y);//SY=X'*y
		}
		//�����ϵ������
		if (m_bWithConst)
		{
			m_Matrix_R=SX/sqrt(Multi(diag(SX),diag(SX)));									
		}
		else
		{	
			CDoubleVector ONES(m_nXRow,1.0);
			CDoubleMatrix X_Tem(m_nXRow,m_nXCol,0.0);
			for (int i=0;i<m_nXCol;i++)
				X_Tem[i]=Matrix_X[i]-X_mean[i]*ONES;
			// 			    Matrix_X-=X_Mean;
			// 				CDoubleMatrix MatTem=X_Tem.trans();
			CDoubleMatrix MatTem2;
			MatTem2=X_Tem.dot(X_Tem);
			m_Matrix_R=MatTem2/sqrt(Multi(diag(MatTem2),diag(MatTem2)));
		}
///////////////////////////////////////////////////
////      ����cMethodѡ��m_ncValue      ///////////
///////////////////////////////////////////////////
//begin->
//���׼�������ؾ��������ֵ��������
		m_EigVector=m_Matrix_R;	
		m_EigValue=EigValPlusVec(&m_EigVector, 1);
		// 			TraceMatrix(m_EigValue);
		switch (m_ncMethod)
		{
		case 0://�Զ���
			break;
		case 1://�ۼƹ�����
			{
				int 		count=0;
				double	sum=0;
				double	calContrST=m_nXCol*m_dcValueCalContr;
				while ( (count<m_nXCol) && (sum < calContrST) )
				{
					sum+=m_EigValue(count,0); 
					count++;
				}
				m_ncValue=count;
				break;
			}
		case 2://��Ӧ����ֵ��̫С
			{
				int count=0;
				while ( (count<m_nXCol) && (m_EigValue(count,0)>m_dcValueCorrEigVal) )
					count++;
				m_ncValue=count;	
				break;				
			}
			
		case 3:	//�������������ĵ�
			{
				int count=0; 
				double	sum=0;
				while ( (count<m_nXCol) && ((sum+1/m_EigValue(count,0)) <= 5*(count+1)) )
				{
					sum+=1/m_EigValue(count,0);	
					count=count+1;	
				}
				m_ncValue=count; 
				break;
			}
		default: 
			m_ncValue=2;
		}
		if ( m_ncValue<0 || m_ncValue>m_nXCol)//�жϣ����ɷ������ܴ��ڱ�����
		{
			CTString strWarning = "�������ɷ������������������������У�";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return pResult;
		}
		//<-end
		
		//////////////////////////////////////////////////
		////���ɷֻع�ϵ������//////////////begin/////////
		//////////////////////////////////////////////////
		//begin->
		CDoubleMatrix EigVector=SX;
		CDoubleMatrix EigValue=EigValPlusVec(&EigVector, 1);
		CDoubleMatrix EigValuePC =	EigValue.slice(m_ncValue,1);
		CDoubleMatrix EigVectorPC=	EigVector.slice(m_nXCol,m_ncValue);
		CDoubleMatrix Sy_c =	EigVectorPC.dot(Sy);
		CDoubleMatrix alpha_c =	Sy_c/EigValuePC;
		m_MatCoeff=EigVectorPC*alpha_c;
		//��ȡ���ͳ����
		
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
		Matrix_X.destroy();
		Matrix_y.destroy();
		//<-end
		//�����������������
		CreateResult();
		//�ṹ�帳ֵ
		if (m_bOutputModel)//���Ҫ�������Ա����(ģ�ͽ��)���������������
		{
			m_pPCRResult->m_strVarlist.Copy(m_strVarlist);	
			m_pPCRResult->m_bWithConst =		m_bWithConst;
			m_pPCRResult->m_vectBeta =			MatrixToVector(m_MatCoeff);
			if (m_pPCRResult->m_bWithConst)
				m_pPCRResult->m_vectBeta.insert(0,m_dINTERCEPT);
			m_pPCRResult->m_ncValue =			m_ncValue;
			m_pPCRResult->m_dRSS =				m_dRSS;
			m_pPCRResult->m_dESS =				m_dESS;
			m_pPCRResult->m_Matrix_R =			m_Matrix_R;
			m_pPCRResult->m_ncMethod =			m_ncMethod;
			m_pPCRResult->m_matEigVal =			m_EigValue;
			m_pPCRResult->m_matEigVect =		m_EigVector;
		}
	}
	// 		tt.Toc("pcrtime");
	return m_pResult;
}
///*****************************************************************����������


//////////////////////////////////////////////////////////////////
/////    ȡ����ֵ����������������*****������ʽ��ʾ          //////
/////    nIndex == 0 ��С�������� nIndex == 1 �Ӵ�С����  //////
/////    ��������ֵm_Row��1����m_EigVec�洢��Ӧ��������   //////
/////    �������������ÿһ�ж�Ӧһ������ֵ                 //////
//////////////////////////////////////////////////////////////////
CDoubleMatrix CPrinCompReg::EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex)
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
		*m_EigVec = VectorToMatrix(m_VectEigVector,m_nXCol,m_nXCol).trans();
		return EigValue;
	}
}
///////////////////////////////////////////////////////////
//////////////      ���������       //////////////////////
///////////////////////////////////////////////////////////
void CPrinCompReg::CreateResult()
{
	//����kֵ��Ϣ��
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("���ɷָ���ѡ����Ϣ��");
	//	pTable->CreateTable(2, 2);
	pTable->SetCols(2);
	pTable->SetRows(2);
	pTable->InsertColumn(0,"����");
	pTable->InsertColumn(0,"���ɷ���");
	switch (m_ncMethod)
	{
	case 0:
		pTable->InsertItem(0,"�Զ���");
		break;
	case 1:
		pTable->InsertItem(0,"�ۼƹ������б�");
		break;
	case 2:
		pTable->InsertItem(0,"����ֵ��̫С����");
		break;
	case 3:		
		pTable->InsertItem(0,"�������μ�������");
		break;
	default: 
		pTable->InsertItem(0,"ȱʡ");
	}
	pTable->SetItemText(0,1,m_ncValue);
	//����һ���������Ķ���
	CRsltElementTable * pETable= new CRsltElementTable("���ɷָ���ѡ����Ϣ��",pTable);
	m_pResult->Add(pETable);
	if (m_bWithPC)
	{
		//����������Ϣ��
		pTable = new CTLTable;
		pTable->SetTitle("���ᣨ��Ӧ���ɷ֣���Ϣ��");
		pTable->InsertColumn(0,"����");
		pTable->InsertColumn(1,"��Ӧ����ֵ");
		pTable->InsertColumn(2,"������");
		pTable->InsertColumn(0,"�ۼƹ�����%");
		int i=0;
		for(i=0;i<m_nXCol;i++)
		{
			//ȡ������
			pTable->InsertColumn(i+4,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		}
		double sum=0;
		CTString strTemp;
		for(i=0;i<m_ncValue;i++)
		{
			//����
			strTemp.Format("a(%d)",i+1);
			pTable->InsertItem(i,strTemp);
			// 			pTable->SetItemText(i,0,);
			//��Ӧ����ֵ���ѱ�׼����
			pTable->SetItemText(i,1,m_EigValue(i,0));
			//������
			pTable->SetItemText(i,2,(m_EigValue(i,0)/m_nXCol));
			//�ۼƹ�����
			sum+=m_EigValue(i,0);
			pTable->SetItemText(i,3,100*(sum/m_nXCol));			
			for(int j=0;j<m_nXCol;j++)
			{
				pTable->SetItemText(i,j+4,m_EigVector(j,i));
			}
		}
		//����һ���������Ķ���
		pETable= new CRsltElementTable("���ᣨ��Ӧ���ɷ֣���Ϣ��",pTable);
		m_pResult->Add(pETable);		
	}
	
	//�����Ա������ϵ����
	pTable = new CTLTable;
	pTable->SetTitle("�Ա������ϵ����");
	pTable->InsertColumn(0,"���ϵ��");
	int i=0;
	for(i=0;i<m_nXCol;i++)
	{
		pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]),FALSE);
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
		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(m_nXCol+2);
		pTable->SetTitle("�ع�ϵ����");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"������");
		pTable->InsertItem(0,"�ع�ϵ��b");
		for (i=0;i< m_nXCol;i++)
		{
			pTable->InsertColumn(i+2,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
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
		pTable->InsertItem(0,"",FALSE);
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
		// 		pTable->InsertColumn(1,"������");
		pTable->InsertItem(0,"�ع�ϵ��");
		for (i=0;i<m_nXCol;i++)
		{
			pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
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
	CTString strEquation=m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[0])+"=";	
	CTString strTemp;	
	if(m_bWithConst)
	{
		strTemp.Format("%.4f",m_dINTERCEPT);
		strEquation+=strTemp;
		for(int i=0;i<m_nXCol;i++)
		{
			if(m_MatCoeff(i,0)>0)
			{
				strTemp.Format("+%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
			}
			else
			{
				strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
			}
			strEquation+=strTemp;
		}
	}
    else
	{
		for(int i=0;i<m_nXCol;i++)
		{
			if (i==0)
			{
				strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
			}
			else
			{
				if(m_MatCoeff(i,0)>=0)
				{
					strTemp.Format("+%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
				}
				else
				{
					strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
				}
			}
			strEquation+=strTemp;	
		}	
	}
	pTable = new CTLTable;
	pTable->CreateTable(2, 2);	
	pTable->SetTitle("�ع鷽��");
	pTable->InsertColumn(0,strEquation);
	pETable= new CRsltElementTable("�ع鷽��",pTable);
	m_pResult->Add(pETable);
}

//////////////////////////////////////////////////////////////////////////////////
///////     ȡ���ݣ�������ȱʧֵ�������ݷ������Matrix_X&Matrix_y
//////      �滮����ȱʧֵ�����װ������һ��public��⣬�Ժ�ÿ���㷨����ֱ�ӵ���
//////////////////////////////////////////////////////////////////////////////////
void CPrinCompReg::GetMatrixData(CDoubleMatrix* Matrix_X, CDoubleMatrix* Matrix_y)
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
			for( int j=0;j<m_nXCol;j++)
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

		//CArray<int,int> arrayMissingValue;
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
		for(i=0; i<nCaseNum; i++ )
		{
			int j=0;
			for( j=0;j<arrayMissingValue.size();j++ )
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
		for(i =0;i<pxarrayDataSet.size();i++ )
		{
			delete pxarrayDataSet[i];
		}		    
		pxarrayDataSet .clear();
		delete pyDataSet;
	}
}


//
//////////////////////////////////////////////////////////////////////////////
////������:�����          ����ʱ�䣺05.10.25
////��������;��ר���������ת�Ⱥ���ˣ�Z=X'*Y
////�ļ�������޸�ʱ�䣬�޸ĵĵط���
//////////////////////////////////////////////////////////////////////////////
//CDoubleMatrix CPrinCompReg::TranProd(CDoubleMatrix &X, CDoubleMatrix &Y)
//{
//	//int m_nRowX=X.mrow();
//	int m_nColX=X.mcol();
//	int m_nColY=Y.mcol();
//	CDoubleMatrix Z(m_nColX,m_nColY);
//	if (X.mrow()!=Y.mrow())
//		AfxMessageBox("����ά����ƥ�䣡");
//	else
//		for (int i=0;i<m_nColX;i++)
//			for (int j=0;j<m_nColY;j++)
//				Z(i,j)=X[i].dot(Y[j]);
//			return Z;
//}

//int CPrinCompReg::GetCurSec()//ȡ������ʱ�䡪����
//{  
//	time_t time0;
//	// 		struct _timeb tstruct;
//	struct tm *pNow;
//	_tzset();
//	time( &time0 );
//	
//	//		_ftime( &tstruct );
//	pNow = localtime( &time0 );
//	// 		TRACE( "Current Time:\t\t\t%.8s", 	asctime( pNow )+11 );
//	return pNow->tm_sec+pNow->tm_min*60+pNow->tm_hour*3600;
//	//	    TRACE( ":%u\n", tstruct.millitm );
//}








