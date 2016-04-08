// RegressionLinear.cpp: implementation of the CRegressionLinear class.
//
//���±�д:����ׯ          �ı�дʱ�䣺05.11.25
//�ļ�����;�����Իع�
//////////////////////////////////////////////////////////////////////

#include "math.h"
#include "StdAfx.h"
#include "tlAdvn.h"
#include "RegressionLinear.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"
//#include "..\TLCRNT\Script.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegressionLinear::CRegressionLinear()
{
	m_nDependIndex  = -1;
	m_pDataInterface= NULL;
	m_nMethod = 0;
	m_nMissMethod  = 0;
	m_dConfidence=0.95;
	m_dTest=0.05;
	m_fEntry = 0.05;
	m_fRemoval = 0.1;
	m_bPredict=false;
	m_bResidue=false;
	m_bCoefInter=false;
	m_bChartLine=false;//���ͼ
	m_bChartScatterPlot=false;// �в�ɢ��ͼ
	m_bChartScatterPlot2=false; // �в���̬����ͼ
	m_bWithConst = true;
	m_bCorr = false; //�������ؾ���
	m_bWithColl = false; // ��������������
}

CRegressionLinear::~CRegressionLinear()
{
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
}


double CRegressionLinear::GetQValue(CDoubleMatrix matrix, CDoubleMatrix matrix_Y)//����Qֵ

{
	CDoubleMatrix matrix_Beta = inv( (CDoubleMatrix)( trans(matrix) * matrix )) * (trans(matrix) * matrix_Y);
	CDoubleMatrix matrix_temp = trans(matrix_Y) * matrix_Y - (trans(matrix_Y) * matrix) * matrix_Beta;
	return matrix_temp(0,0);
}


void  CRegressionLinear::GetData(CDataInterface *pDataInterface, CDoubleMatrix& Matrix_Data)//�������������[��weight��Xy]���Լ����жϼ�Ȩ��
{
	//ȡ�����ݣ���֯�������ݾ���
	int i,m = m_nXCol+1;
	int nColNum=0;
	CDWordArray arCol;
	if(m_bWithWeight)//���Ȩ�ر����±�
        arCol.Add(pDataInterface->GetWeightNumb());
	for(i=0;i<m_nXCol;i++)//����Ա����±�
	{
		nColNum = m_tVarInfo.pValue[i];
		arCol.Add(nColNum);
	}
	if(m_nDependIndex >= 0)//���������±�
		arCol.Add(m_nDependIndex);
	int m_nXRow = pDataInterface->GetColsData(arCol,Matrix_Data,m_nMissMethod);//ȡ������
	int nR = Matrix_Data.mrow();
	int nC = Matrix_Data.mcol();
}

bool CRegressionLinear::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
	//REGLINEAR
	//	/DATA=[FILE]
	//	/DEPENDENT [varlist]
	//	/INDEPENDENT [varlist]
	//	/MISSING=[{LISTWISE**},{MEANSUBSTITUTION}]
	//	/METHOD=[{ENTER**},{BACKWARD},{FORWARD},{STEPWISE}] [NOCONST]
	//	/CRITERIA=[PIN({0.05**})] [POUT({0.1**})] [CIN({.95**})]
	//	/WEIGHT [varlist]
	//	/PLOT [CHARTLINE] [CHARTSCATTER] [CHARTNORMALSCA]
	//	/PRINT [RESIDUAL] [PREDECT]
	//	/RESULT [CORRELATION] [COLINEARITY]
	//	/SAVEMODEL [FILE]
	AnaWord VOstr;
	VOstr.Import(VO);
	
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
	
	m_nMissMethod = AnaSyntax::findstr("/MISSING","MEANSUBSTITUTION",VOstr);
	
	if (AnaSyntax::findstr("/METHOD", "BACKWARD",VOstr))
		m_nMethod = 1;
	else if (AnaSyntax::findstr("/METHOD", "FORWARD",VOstr))
		m_nMethod = 2;
	else if (AnaSyntax::findstr("/METHOD", "STEPWISE",VOstr))
		m_nMethod = 3;
	else 
		m_nMethod = 0;
	
	m_fEntry = AnaSyntax::getvalue("/CRITERIA","PIN",VOstr);
	m_fRemoval = AnaSyntax::getvalue("/CRITERIA","POUT",VOstr);
	m_bCoefInter = AnaSyntax::findstr("/CRITERIA","CIN",VOstr);
	m_dConfidence = AnaSyntax::getvalue("/CRITERIA","CIN",VOstr);
	if (m_fEntry >1 || m_fEntry <0)
		m_fEntry = 0.05;
	if (m_fRemoval>1 || m_fRemoval<0)
		m_fRemoval = 0.1;
	if(m_dConfidence>1 || m_dConfidence<0)
	{
		m_bCoefInter = false;
		m_dConfidence = 0.95;
	}

	m_bWithWeight = AnaSyntax::exist("/WEIGHT", VOstr);
	if (m_bWithWeight)
	{
		CTStringArray ynames;
		AnaSyntax::getVAR("/WEIGHT", VOstr, ynames);
		if ((ynames.GetSize())!=1) 
			return false;
		CTString yname = ynames.GetAt(0);
		m_nWeightID = pDataInterface->IndexByName(yname);
	}
	
	m_bWithConst = !AnaSyntax::findstr("/METHOD", "NOCONST",VOstr);
	
	m_bChartLine = AnaSyntax::findstr("/PLOT","CHARTLINE",VOstr);
	m_bChartScatterPlot = AnaSyntax::findstr("/PLOT","CHARTSCATTER",VOstr);
	m_bChartScatterPlot2 = AnaSyntax::findstr("/PLOT","CHARTNORMALSCA",VOstr);
	
	m_bPredict = AnaSyntax::findstr("/PRINT","PREDECT",VOstr);
	m_bResidue = AnaSyntax::findstr("/PRINT","RESIDUAL",VOstr);
	m_bWithColl = AnaSyntax::findstr("/RESULT","COLINEARITY",VOstr);
	m_bCorr = AnaSyntax::findstr("/RESULT","CORRELATION",VOstr);
	//	/SAVEMODEL [FILE]
	m_bOutput = true;//AnaSyntax::exist("/OUTPUTMODLE", VOstr);
	m_bSaveModel = AnaSyntax::exist("/SAVEMODEL", VOstr);
	if (m_bSaveModel)
		m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL", VOstr);

	return true;
}

CResult * CRegressionLinear::OnStepWise(CDataInterface *pDataInterface, string VO)
{	
//	CTimer tTemp;
	InitParma();
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	m_nMethod = 0;
	switch(m_nMethod) 
	{
	case 0:
		{
			pResult = new CResult("���Իع������� (ȫ��ѡ�뷨)");
			break;
		}
	case 1:
		{
			pResult = new CResult("���Իع������� (���)");
			break;
		}
	case 2:
		{
			pResult = new CResult("���Իع������� ����ǰ��)");
			break;
		}
	case 3:
		{
			pResult = new CResult("���Իع������� (�𲽷�)");
			break;
		}
	}
	m_pResult = pResult;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}

	briefrTable=NULL;
	m_strX_nameAry.RemoveAll();	
	m_stry_name="";
	
	bool WithConst = m_bWithConst;
	int i;
	m_nXCol = m_tVarInfo.iCount;
	
	if( pDataInterface->GetWeightNumb()>=0 )
		m_bWithWeight = true;
	else m_bWithWeight = false;
	
	CDoubleMatrix data_read;
	GetData(pDataInterface, data_read);//��ȡ���ݣ��õ�[��weight��Xy]�ľ���
	CDoubleMatrix data_read_resi = NULL;
	if (m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)
		data_read_resi = copy(data_read);
	m_nXRow = data_read.mrow();
	m_stry_name = pDataInterface->GetFieldName(m_nDependIndex);
	m_strX_nameAry.RemoveAll();
	for (i=0;i< m_tVarInfo.iCount;i++)
		m_strX_nameAry.Add(pDataInterface->GetFieldName(*(m_tVarInfo.pValue+i)));
	m_strX_org.RemoveAll();
	for (i=0;i< m_tVarInfo.iCount;i++)
		m_strX_org.Add(pDataInterface->GetFieldName(*(m_tVarInfo.pValue+i)));
	CDoubleVector XyMean(m_nXCol+1);
	//��ʼ�����ݽ��з�������
	if (m_bWithWeight)//��Ȩ��ʱ����X��y������Ȩ�ص�ƽ����
	{
		sum_weight = sum(data_read(0));
		data_read(0) = sqrt(data_read(0));
		for (i=0;i<m_nXCol+1;i++)
		{
			data_read(i+1)= data_read(i+1)*data_read(0);
			XyMean(i) = data_read(i+1).dot(data_read(0))/sum_weight;
		}
	}
	else
		XyMean = mean(data_read);
	CDoubleMatrix m_XyTimesXy;//������������Xy����Xy��
	CDoubleMatrix XyTimesXy_con(m_nXCol+1+m_bWithConst,m_nXCol+1+m_bWithConst);//���������û�г���Ҳ��û�У���
	if (m_bWithWeight) 
	{
		XyTimesXy_con = dot(data_read,data_read);
		m_XyTimesXy = XyTimesXy_con.slice(1,1,m_nXCol+1,m_nXCol+1);
	}
	else
	{
		m_XyTimesXy = dot(data_read,data_read);
		if (m_bWithConst) 
		{
			for (i=0; i<m_nXCol+1; i++)
			{
				for (int j=0; j<m_nXCol+1; j++)
					XyTimesXy_con(i+1,j+1) = m_XyTimesXy(i,j);
				XyTimesXy_con(0,i+1) = XyMean(i+m_bWithWeight)*m_nXRow;
				XyTimesXy_con(i+1,0) = XyMean(i+m_bWithWeight)*m_nXRow;
			}
			XyTimesXy_con(0,0) = m_nXRow;
		}
	}
	if (!m_bWithConst) 
	{
		XyTimesXy_con = m_XyTimesXy;
	}
	
	CDoubleMatrix XyMean_matrix = VectorToMatrix(XyMean,XyMean.vlen(),1);
	CDoubleMatrix Xy_Sigma = m_XyTimesXy - ((m_bWithWeight)?sum_weight:((double)m_nXRow)) * (XyMean_matrix * XyMean_matrix.trans() );//���Ļ����X'X(�޳���)
	m_var_y = Xy_Sigma(m_nXCol,m_nXCol);
	m_ytimesy = m_XyTimesXy(m_nXCol,m_nXCol);
	
	if (m_bWithWeight) 
	{
		for (i=0; i<m_nXRow; i++)
			if (data_read(i,0)<0) 
			{
				CTString strWarning = "ע�� Ȩ�أ�Ƶ�������ָ������޷����д�Ȩ�صķ�����";
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
				return m_pResult;
			}
	}
	
	if( Xy_Sigma(m_nXCol,m_nXCol) < EPS_ACCUR)
	{
		CTString strWarning = "ע�� �������Ϊ�������޷����лع������";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	
	m_XtimesX = XyTimesXy_con.slice(m_nXCol+m_bWithConst,m_nXCol+m_bWithConst);
	m_Xtimesy = XyTimesXy_con.slice(0,m_nXCol+m_bWithConst,m_nXCol+m_bWithConst,1);
	m_CorrMatrix = Xy_Sigma/sqrt(Multi(diag(Xy_Sigma),diag(Xy_Sigma)));
	int nColTemp = m_nXCol;
	
	for (i=0; i<nColTemp; i++) //���XΪ�����ı���
	{
		if (diag(Xy_Sigma)(i) < EPS_ACCUR)
		{
			m_XtimesX = DelMatrix(m_XtimesX,i);//ɾ����k�к͵�k��
			m_Xtimesy = VectorToMatrix(DelVector(m_Xtimesy(0),i));//ɾ����k�к͵�k��
			m_XyTimesXy = DelMatrix(m_XyTimesXy,i);//ɾ����k�к͵�k��
			m_CorrMatrix = DelMatrix(m_CorrMatrix,i);
			m_nXCol-=1;
			m_strX_nameAry.ElementAt(i);
			CTString strWarning = "ע�� ���� " + m_strX_nameAry[i] + " Ϊ���������μӻع������";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add(pWarningTextRslt);
		}
	}
	
	if((m_nXRow<=m_nXCol+1+m_bWithConst) || ((sum_weight < (m_nXCol + m_bWithConst))&&m_bWithWeight))
	{
		CTString strWarning = "ע�� ��Ч��¼��̫�ٻ�Ƶ���ܺ�̫С���޷����лع������";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	
	m_X_temp_nameAry.RemoveAll();
	for (i=0;i< m_tVarInfo.iCount;i++)
		m_X_temp_nameAry.Add(pDataInterface->GetFieldName(*(m_tVarInfo.pValue+i)));
	
	CDoubleMatrix m_EigVector=m_CorrMatrix.slice(m_nXCol,m_nXCol);
	m_EigValue = EigValPlusVec(&m_EigVector, 1);
	while (m_EigValue(m_nXCol-1,0) < EPS_ACCUR)
	{
		m_bWithColl = true;
		CDoubleVector temp_Eig = m_EigVector.slice(0,m_nXCol-1,m_nXCol,1)(0);
		int mm;//�õ��������Ա�����ȫ��ص����Ǹ�
		temp_Eig = abs(temp_Eig);
		double temp_nouse = temp_Eig.Max(mm);
		CTString strWarning = "ע�� ���� " + m_strX_nameAry[mm] + " �������Ա���֮�������ȫ���Թ�ϵ�����μӻع������";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		m_XtimesX = DelMatrix(m_XtimesX,mm);//ɾ����k�к͵�k��
		m_Xtimesy = VectorToMatrix(DelVector(m_Xtimesy(0),mm));//ɾ����k�к͵�k��
		m_XyTimesXy = DelMatrix(m_XyTimesXy,mm);//ɾ����k�к͵�k��
		m_strX_nameAry.RemoveAt(mm);
		m_X_temp_nameAry.RemoveAt(mm);
		m_CorrMatrix = DelMatrix(m_CorrMatrix,mm);
		m_nXCol-=1;
	}
	
	Xmean = XyMean.copy();
	switch (m_nMethod)
	{
	case 0:	//�������ȫ����������
		{
			RegLinear(m_XtimesX, m_Xtimesy, m_CorrMatrix, sum_weight, m_nXCol,m_nXRow,m_bWithConst);
			if (m_bWithColl) 
				Collinearity(m_XtimesX, m_nXCol, m_bWithConst);
			if (m_bWithColl) 
				if (m_nXCol>0) 
					m_Tolerance = mmthSelfReg(m_XtimesX, Xmean, m_bWithConst);
				DrawResult(m_nXCol, m_bWithConst);
				if (m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)
					ResiPlot(data_read_resi, m_bWithConst, m_coefficiency, m_XtimesX, m_MSE);
				break;
		}
	case 1: //����޳�����
		{
			bool conti_back = true;//�����޳�
			CTString strWarning = "";
			CDoubleMatrix CorrMatrix = copy(m_CorrMatrix);
			int XCol = m_nXCol;
			int number = 0;
			m_nLine = 0;
			m_nColl = 0;
			m_bDrawTableHead = false;
			while (conti_back) 
			{
				RegLinear(m_XtimesX, m_Xtimesy, CorrMatrix, sum_weight, XCol, m_nXRow, WithConst);
				if (m_bWithColl) 
					Collinearity(m_XtimesX, XCol, WithConst);
				
				if(!m_bDrawTableHead)
				{
					DrawTableHead(m_nXCol,WithConst);
					m_bDrawTableHead = true;
				}
				if (m_bWithColl) 
					if (XCol>0) 
						m_Tolerance = mmthSelfReg(m_XtimesX, Xmean, WithConst);
					DrawTableBody(XCol, WithConst, number, strWarning);
					
					int k;//Ӱ����С��XΪ��k����
					if (((m_vector_Sig.Max(k)) > m_fRemoval) && (m_vector_Sig.vlen()>0)) 
					{
						
						if ((k == 0) && WithConst)
						{
							strWarning = "������ ��ģ��Ӱ�첻���������޳���ģ�͡�";
							WithConst = false;
						}
						else
						{
							strWarning = "���� \"" + m_X_temp_nameAry[k-WithConst] + "\" ��ģ��Ӱ�첻���������޳���ģ�͡�";
							XCol -= 1;
							CorrMatrix = DelMatrix(CorrMatrix,k-WithConst);
							m_X_temp_nameAry.RemoveAt(k-WithConst);
							Xmean = DelVector(Xmean,k-WithConst);
						}
						m_XtimesX = DelMatrix(m_XtimesX,k);//ɾ����k�к͵�k��
						m_Xtimesy = VectorToMatrix(DelVector(m_Xtimesy(0),k));//ɾ����k�к͵�k��
						if (XCol <= 0) //ֻ���������û���Ա��������ˣ������ʽ�д�
						{
							conti_back = false;	
							strWarning = "���棺 ֻ�г��������ع飬������ع���������";
							CTString str;
							str.Format("%d", ++number);
							pTable1->InsertItem(number, str);
							pTable1->SetItemText(number, 1, strWarning);
						}
					}
					else
						conti_back = false;
					
			}
			if(m_bDrawTableHead) 
				DrawTable(XCol, WithConst);
			if ((m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)&&(XCol>0))
				ResiPlot(data_read_resi, WithConst, m_coefficiency, m_XtimesX, m_MSE);
			break;
		}
	case 2: //��ǰ��ѡ�����
		{
			CDoubleMatrix XtimesX;
			CDoubleVector coefficiency;
			double MSE;
			CDoubleMatrix temp_XX = NULL;
			CDoubleVector whichoneisin(m_nXCol+m_bWithConst,-1);
			bool conti_forew = true;//����ѡ��
			int total_ColXandConst = 0;
			WithConst = false;
			bool WithConst1 = false;
			CTString strWarning = "";				
			int number = 0;
			m_nLine = 0;
			m_nColl = 0;
			bool m_bDrawTableHead = false;
			while (conti_forew) 
			{
				XtimesX = copy(temp_XX);
				WithConst1 = WithConst;
				coefficiency = copy(m_coefficiency);
				MSE = m_MSE;
				total_ColXandConst++;
				if ((whichoneisin(0)>0) && (m_bWithConst)) 
					WithConst = true;
				else 
					WithConst = false;
				int mm;
				CDoubleVector temp_compare(m_nXCol+m_bWithConst,1/DoubleZero);
				for (i=0; i<m_nXCol+m_bWithConst; i++)
				{
					if (whichoneisin(i)<0) 
					{
						if ((i==0) && (m_bWithConst))
							WithConst = true;
						CDoubleVector trynewcomemin = whichoneisin.copy();
						trynewcomemin(i) = 1;
						temp_XX = SelectMatrix(trynewcomemin, m_XtimesX, total_ColXandConst);
						CDoubleVector temp_includey(m_nXCol+1);
						for (int j=0; j<m_nXCol; j++)
							temp_includey(j) = trynewcomemin(j+m_bWithConst);
						temp_includey(m_nXCol)=1;
						CDoubleMatrix temp_Xy = VectorToMatrix(SelectVector(trynewcomemin, m_Xtimesy(0), total_ColXandConst));
						CDoubleMatrix temp_corr = SelectMatrix(temp_includey, m_CorrMatrix, total_ColXandConst+1-WithConst);
						RegLinear(temp_XX,temp_Xy, temp_corr, sum_weight, (total_ColXandConst-WithConst), m_nXRow, WithConst);
						temp_compare(i) = m_MSE;
						if ((i==0) && (m_bWithConst))
							WithConst = false;
					}
				}
				double temp_nouse = temp_compare.Min(mm);
				whichoneisin(mm) = 1;
				if ((whichoneisin(0)>0) && (m_bWithConst))
					WithConst = true;
				int tt=0;
				for (i=0; i< mm; i++)
				{
					if (whichoneisin(i)>0)
						tt++;
				}
				temp_XX = SelectMatrix(whichoneisin, m_XtimesX, total_ColXandConst);
				CDoubleVector temp_includey(m_nXCol+1);
				for (int j=0; j<m_nXCol; j++)
					temp_includey(j) = whichoneisin(j+m_bWithConst);
				temp_includey(m_nXCol)=1;
				CDoubleMatrix temp_corr = SelectMatrix(temp_includey, m_CorrMatrix, total_ColXandConst+1-WithConst);
				CDoubleMatrix temp_Xy = VectorToMatrix(SelectVector(whichoneisin, m_Xtimesy(0),total_ColXandConst));
				Xmean = SelectVector(whichoneisin, XyMean,total_ColXandConst);
				RegLinear(temp_XX, temp_Xy, temp_corr, sum_weight, total_ColXandConst-WithConst, m_nXRow, WithConst);
				if (m_bWithColl) 
					Collinearity(temp_XX, total_ColXandConst-WithConst, WithConst);
				total_ColXandConst--;
				if ((m_vector_Sig(tt)< m_fEntry) && (total_ColXandConst <(m_nXCol + m_bWithConst)))
				{
					if(!m_bDrawTableHead)
					{
						DrawTableHead(m_nXCol,WithConst);
						m_bDrawTableHead = true;
					}
					
					total_ColXandConst++;
					SelectStrAry(whichoneisin, m_strX_nameAry, m_X_temp_nameAry);
					if ((tt == 0) && WithConst) 
						strWarning = "������ ��ģ��Ӱ����������ѡ��ģ�͡�";
					else
						strWarning = "���� \""+ m_X_temp_nameAry[tt-WithConst]+"\" ��ģ��Ӱ����������ѡ��ģ�͡�";
					XtimesX = temp_XX;
					if (m_bWithColl) 
						if ((total_ColXandConst-WithConst)>0) 
							m_Tolerance = mmthSelfReg(temp_XX, Xmean, WithConst);
						DrawTableBody(total_ColXandConst-WithConst, WithConst, number, strWarning);
						WithConst1 = WithConst;
				}
				else
				{
					conti_forew = false;
				}
			}
			if ((total_ColXandConst-WithConst)==0 && number) 
			{
				CTString strWarning = "���棺 û���Ա������룬�޷�����ع�ģ�ͷ��������";
				strWarning = "���棺 ֻ�г��������ع飬������ع���������";
				CTString str;
				str.Format("%d", ++number);
				pTable1->InsertItem(number, str);
				pTable1->SetItemText(number, 1, strWarning);
			}
			if(!m_bDrawTableHead)
			{
				CRsltElementText *pErrorText=new CRsltElementText("����");
				CTString strError="û���Ա������룬�޷�����κλع�ģ�ͷ���";
				pErrorText->AddString(strError);
				m_pResult->Add(pErrorText);
			}
			if(m_bDrawTableHead)
				DrawTable(m_nXCol, WithConst);
			m_XtimesX = XtimesX;
			m_MSE = MSE;
			if ((m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)&&((total_ColXandConst-WithConst)>0))
				ResiPlot(data_read_resi, WithConst1, coefficiency, XtimesX, MSE);
			break;
			}
			case 3: //�𲽷�ѡ�����
				{
					CDoubleMatrix XtimesX;
					CDoubleVector coefficiency;
					double MSE;
					CDoubleVector whichoneisin(m_nXCol+m_bWithConst,-1);
					bool conti_forew = true;//�����޳�
					int total_ColXandConst = 0;
					CTString strWarning = "";				
					WithConst = false;
					bool WithConst2 = false;
					int contoftime = 0;
					CDoubleMatrix temp_XX = NULL;
					CDoubleMatrix temp_Xy;
					CDoubleMatrix temp_corr;
					int number = 0;
					m_nLine = 0;
					m_nColl = 0;
					bool m_bDrawTableHead = false;
					while (conti_forew) 
					{
						XtimesX = copy(temp_XX);
						WithConst2 = WithConst;
						coefficiency = copy(m_coefficiency);
						MSE = m_MSE;
						total_ColXandConst++;
						if ((whichoneisin(0)>0) && (m_bWithConst)) 
							WithConst = true;
						else 
							WithConst = false;
						int mm;
						CDoubleVector temp_compare(m_nXCol+m_bWithConst,1/DoubleZero);
						for (i=0; i<m_nXCol+m_bWithConst; i++)
						{
							if (whichoneisin(i)<0) 
							{
								if ((i==0) && (m_bWithConst))
									WithConst = true;
								CDoubleVector trynewcomemin = whichoneisin.copy();
								trynewcomemin(i) = 1;
								temp_XX = SelectMatrix(trynewcomemin, m_XtimesX, total_ColXandConst);
								CDoubleVector temp_includey(m_nXCol+1);
								for (int j=0; j<m_nXCol; j++)
									temp_includey(j) = trynewcomemin(j+m_bWithConst);
								temp_includey(m_nXCol)=1;
								temp_Xy = VectorToMatrix(SelectVector(trynewcomemin, m_Xtimesy(0), total_ColXandConst));
								temp_corr = SelectMatrix(temp_includey, m_CorrMatrix, total_ColXandConst+1-WithConst);
								RegLinear(temp_XX,temp_Xy, temp_corr, sum_weight, (total_ColXandConst-WithConst), m_nXRow, WithConst);
								temp_compare(i) = m_MSE;
								if ((i==0) && (m_bWithConst))
									WithConst = false;
							}
						}
						double temp_nouse = temp_compare.Min(mm);
						whichoneisin(mm) = 1;
						if ((whichoneisin(0)>0) && (m_bWithConst))
							WithConst = true;
						int tt=0;
						for (i=0; i< mm; i++)
						{
							if (whichoneisin(i)>0)
								tt++;
						}
						temp_XX = SelectMatrix(whichoneisin, m_XtimesX, total_ColXandConst);
						CDoubleVector temp_includey(m_nXCol+1);
						for (int j=0; j<m_nXCol; j++)
							temp_includey(j) = whichoneisin(j+m_bWithConst);
						temp_includey(m_nXCol)=1;
						temp_corr = SelectMatrix(temp_includey, m_CorrMatrix, total_ColXandConst+1-WithConst);
						temp_Xy = VectorToMatrix(SelectVector(whichoneisin, m_Xtimesy(0),total_ColXandConst));
						Xmean = SelectVector(whichoneisin, XyMean,total_ColXandConst);
						RegLinear(temp_XX,temp_Xy, temp_corr, sum_weight, total_ColXandConst-WithConst, m_nXRow, WithConst);
						if (m_bWithColl) 
							Collinearity(temp_XX, total_ColXandConst-WithConst, WithConst);
						total_ColXandConst--;
						if (m_vector_Sig(tt)< (m_fEntry-DoubleZero*pow(3,contoftime)))
						{
							if(!m_bDrawTableHead)
							{
								DrawTableHead(m_nXCol,WithConst);
								m_bDrawTableHead = true;							
							}
							total_ColXandConst++;
							SelectStrAry(whichoneisin, m_strX_nameAry, m_X_temp_nameAry);
							if ((tt == 0) && WithConst) 
								strWarning = "������ ��ģ��Ӱ����������ѡ��ģ�͡�";
							else
								strWarning = "���� \""+ m_X_temp_nameAry[tt-WithConst]+"\" ��ģ��Ӱ����������ѡ��ģ�͡�";
							XtimesX = temp_XX;
							if (m_bWithColl) 
								if ((total_ColXandConst-WithConst)>0) 
									m_Tolerance = mmthSelfReg(temp_XX, Xmean, WithConst);
								DrawTableBody(total_ColXandConst-WithConst, WithConst, number, strWarning);
								WithConst2 = WithConst;
								while (m_vector_Sig.Max(mm)>m_fRemoval) //�����ҳ�mm��ԭ����Ӧ��λ��
								{
									if ((mm==0) && WithConst) 
										WithConst = false;
									else
										temp_corr = DelMatrix(temp_corr, mm);
									int j_cont=0;
									for (i=0; i<m_nXCol+m_bWithConst; i++)
									{
										if (whichoneisin(i)>0) 
										{
											j_cont++;
											if (j_cont==mm+1) 
												whichoneisin(i)=-1;
										}
									}
									total_ColXandConst--;
									temp_XX = DelMatrix(temp_XX, mm);
									temp_Xy = VectorToMatrix(DelVector(temp_Xy(0), mm));
									Xmean = DelVector(Xmean, mm);
									RegLinear(temp_XX,temp_Xy, temp_corr, sum_weight, total_ColXandConst-WithConst, m_nXRow, WithConst);
									if (m_bWithColl) 
										Collinearity(m_XtimesX, total_ColXandConst-WithConst, WithConst);
									if (m_bWithColl) 
										if ((total_ColXandConst-WithConst)>0) 
											m_Tolerance = mmthSelfReg(temp_XX, Xmean, WithConst);
										DrawTableBody(total_ColXandConst-WithConst, WithConst,number, strWarning);
										if ((mm == 0) && WithConst) 
											strWarning = "������ ��ģ��Ӱ�첻���������޳�ģ�͡�";
										else
											strWarning = "���� \""+ m_X_temp_nameAry[mm-WithConst]+"\" ��ģ��Ӱ�첻���������޳�ģ�͡�";
										m_X_temp_nameAry.RemoveAt(mm);
										XtimesX = temp_XX;
										WithConst2 = WithConst;
										contoftime++;
								}
								if ((total_ColXandConst) == (m_nXCol + m_bWithConst))
								{
									XtimesX = copy(temp_XX);
									coefficiency = copy(m_coefficiency);
									MSE = m_MSE;
									conti_forew = false;
								}
						}
						else
							conti_forew = false;
				}//while continouse
				if ((total_ColXandConst-WithConst)==0 && number) 
				{
					CTString strWarning = "���棺 û���Ա������룬�޷�����ع�ģ�ͷ��������";
					strWarning = "���棺 ֻ�г��������ع飬������ع���������";
					CTString str;
					str.Format("%d", ++number);
					pTable1->InsertItem(number, str);
					pTable1->SetItemText(number, 1, strWarning);
				}
				if(!m_bDrawTableHead)
				{
					CRsltElementText *pErrorText=new CRsltElementText("����");
					CTString strError="û���Ա������룬�޷�����κλع�ģ�ͷ���!";
					pErrorText->AddString(strError);
					m_pResult->Add(pErrorText);
				}
				
				if(m_bDrawTableHead)
					DrawTable(m_nXCol, WithConst);
				m_XtimesX = XtimesX;
				m_MSE = MSE;
				if ((m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)&&((total_ColXandConst-WithConst)>0))
					ResiPlot(data_read_resi, WithConst2, coefficiency, XtimesX, MSE);
				break;
			}//case ������
		}//sweith ������
		Export();//����ģ��-add by xds 20060815
		if (m_bCorr) 
			DrawCorrolation(Xy_Sigma);
		//tTemp.TotalTime();
		m_bWithColl = false;
		return pResult;
}

bool CRegressionLinear::Export()//���ģ��
{
	int i=0, nSize=0;
	CTString szTemp(""), szText("");
	TCLFile theFile;
	
	if (!m_bSaveModel)
		return true;
	bool bOpen = theFile.Open(m_ModelFile.GetData(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Javaʹ��
	//�㷨����
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += REGRESSIONLINEAR;
	//����Դ
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" + m_pDataInterface->m_DataAccess.GetFileName() + "\"";
	//������� 
	szText += " ";
	szText += VARINPUT;
	nSize = m_X_temp_nameAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " + m_X_temp_nameAry.GetAt(i);
	}
	//Ŀ�����
	szText += " ";
	szText += VAROUTPUT;
	szText += " " + m_stry_name;
	//����ʱ��
	SYSTEMTIME st;
	GetLocalTime(&st);
	szTemp.Format("%d��%d��%d��%dʱ%d��%d��",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	szText += " ";
	szText += DATETIME;
	szText += " " + szTemp + "\r\n";
	//����
	nLen = szText.GetLength() + sizeof(int);//�ܵĳ���
	theFile << nLen;
	nLen = theFile.WriteString(szText.GetData());
	//��ʼдģ�Ͳ���
	theFile.SeekToEnd();
	theFile << m_bWithConst;
	//�Ա���
	nSize = m_coefficiency.vlen();
	theFile << nSize;
	//ϵ��
	for (i=0; i<nSize; i++)
	{
		theFile << m_coefficiency(i);
	}
	//�������
	nSize = m_X_temp_nameAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_X_temp_nameAry.GetAt(i);
	}
	//����
	int nRow = m_XtimesX.mrow();
	int nCol = m_XtimesX.mcol();
	theFile << nRow;
	theFile << nCol;
	for (i=0; i<nRow; i++)
	{
		for (int j=0; j<nCol; j++)
		{
			theFile << m_XtimesX(i, j);
		}
	}
	//Ŀ�����
	theFile << m_stry_name;
	//�в�
	theFile << m_MSE;
	theFile.Close();
	return true;
}

void CRegressionLinear::ResiPlot(CDoubleMatrix data_read_resi, bool WithConst, CDoubleVector coefficiency, CDoubleMatrix XtimesX, double MSE)
{
	if ((XtimesX.mrow() <= 0)||(coefficiency.vlen()<=0))
		return;
	double sum_weight;
	if (m_bWithWeight) 
		sum_weight = (data_read_resi(0)).sum();
	else
		sum_weight = m_nXRow;
	double t_value = 0.0;
	if (m_dConfidence > EPS_ACCUR)
		t_value = tinv( (1+m_dConfidence)/2.0, sum_weight -m_X_temp_nameAry.GetSize()-WithConst);
	CDoubleMatrix X_matrix = NULL;
	int j = 0;
	if (WithConst)
	{
		CDoubleVector Ones_v(m_nXRow, 1);
		X_matrix = X_matrix | Ones_v;
	}
	int i;
	for (i=0; i<m_nXCol; i++)
	{
		for (j=0; j<(m_X_temp_nameAry.GetSize()); j++)
		{	
			if (m_strX_nameAry[i] == m_X_temp_nameAry[j]) 
				X_matrix = X_matrix | data_read_resi(i+m_bWithWeight);
		}
	}
	CDoubleVector y_hat = X_matrix * coefficiency;//Ԥ��ֵ
	CDoubleVector vect_y = data_read_resi(data_read_resi.mcol()-1);
	CDoubleVector y_resi = vect_y - y_hat;//�в�
	CDoubleVector y_range(m_nXRow);
	CDoubleVector y_upper;//Ԥ��ֵ������
	CDoubleVector y_lower;//Ԥ��ֵ������
	if (m_bPredict)
	{
		CDoubleMatrix X_trans = X_matrix.trans();
		CDoubleMatrix X_inv = inv(XtimesX);
		for (i=0; i<m_nXRow; i++)
		{
			CDoubleVector vi = X_trans(i);
			y_range(i) = t_value*sqrt(1+((VectorToMatrix(vi).trans())*X_inv*VectorToMatrix(vi))(0,0))*sqrt(MSE);
		}
		y_upper = y_hat + y_range;
		y_lower = y_hat - y_range;
	}
	//����������Ӳв����
	if(m_bResidue)
	{
		CTString strFieldName="�в�";
		if (m_pDataInterface->m_DataAccess.FieldByName(strFieldName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}	
		CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		pField->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
		
		int nRecNo = 1;
		m_pDataInterface->m_DataAccess .First();
		for(int i = 0;i<m_nXRow;i++)
		{
			m_pDataInterface->m_DataAccess.Edit();			
			pField->SetAsDouble(y_resi(i));
			//m_pDataInterface->m_DataAccess.Post();			
			m_pDataInterface->m_DataAccess.Next();
		}	
	}
	
	if(m_bPredict)////�����������Ԥ��ֵ���
	{
		CTString strFieldName = "Ԥ��ֵ", strUpName = "����Ԥ��ֵ", strDownName = "����Ԥ��ֵ";
		
		if (m_pDataInterface->m_DataAccess.FieldByName(strFieldName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}	
		if (m_pDataInterface->m_DataAccess.FieldByName(strUpName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strUpName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}
		if (m_pDataInterface->m_DataAccess.FieldByName(strDownName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strDownName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}
		CField *PredField = m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble),
			*UpField = m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble),
			*DownField = m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
	
		PredField->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(PredField);
		
		UpField->SetFieldName(strUpName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(UpField);
		
		DownField->SetFieldName(strDownName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(DownField);
		
		int nRecNo = 1;
		m_pDataInterface->m_DataAccess.First();
		for(int i=0; i<m_nXRow; i++)
		{
			m_pDataInterface->m_DataAccess.Edit();			
			PredField->SetAsDouble(y_hat(i));
			UpField->SetAsDouble(y_upper(i));
			DownField->SetAsDouble(y_lower(i));
			//m_pDataInterface->m_DataAccess.Post();			
			m_pDataInterface->m_DataAccess.Next();
		}			
	}
	if (m_bResidue) 
	{
		//�в�ͳ�Ʊ�
		//----------------------------------
		CTLTable *pTable = new CTLTable();
		pTable->SetTitle("�в�ͳ�Ʊ�");
		pTable->SetRows(5);
		pTable->SetCols(7);
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(0,"��Сֵ");
		pTable->InsertColumn(0,"���ֵ");
		pTable->InsertColumn(0,"��ֵ");
		pTable->InsertColumn(0,"��׼���");
		pTable->InsertColumn(0,"������");
		pTable->InsertColumn(0,"D.W.");
		
		pTable->InsertItem(0,"Ԥ��ֵ");
		pTable->InsertItem(1,"�в�");
		pTable->InsertItem(2,"��׼����Ԥ��ֵ");
		pTable->InsertItem(3,"��׼���Ĳв�");
		
		
		double prey_mean;
		double prey_dev;
		CDoubleVector weight_v = data_read_resi(0);
		if (m_bWithWeight) 
		{
			prey_mean = sum(y_hat * weight_v)/ sum_weight;
			CDoubleVector temp_y0 = (y_hat - prey_mean);
			CDoubleVector temp_y1 = temp_y0*weight_v;
			prey_dev = sqrt((dot(temp_y0,temp_y1))/(sum_weight-1));
		}
		else
		{
			prey_mean = y_hat.Mean();
			prey_dev = sqrt(var(y_hat));
		}
		
		double resi_mean;
		double resi_dev;
		if (m_bWithWeight) 
		{
			resi_mean = sum(y_resi * weight_v) / sum_weight;
			CDoubleVector temp_y0 = (y_resi- resi_mean);
			CDoubleVector temp_y1 = temp_y0 * weight_v;
			resi_dev = sqrt((dot(temp_y0,temp_y1))/(sum_weight-1));
		}
		else
		{
			resi_mean = y_resi.mean();
			resi_dev = sqrt(var(y_resi));
		}
		
		double DW1 = 0;
		for (i=0; i<m_nXRow-1; i++)
			DW1+=pow((y_resi(i)-y_resi(i+1)),2);
		double DW = DW1 / y_resi.dot(y_resi);
		
		pTable->SetItemText(0,1,y_hat.Min());//prey.Min()
		pTable->SetItemText(0,2,y_hat.Max());//prey.Min()
		pTable->SetItemText(0,3,prey_mean);//sum(prey) / prey.vlen());
		pTable->SetItemText(0,4,prey_dev);//Dev(prey)
		pTable->SetItemText(0,5,int(sum_weight));//prey.vlen()
		pTable->SetItemText(0,6,DW);//Dwͳ����
		
		pTable->SetItemText(1,1,y_resi.Min());//vector_e.Min());
		pTable->SetItemText(1,2,y_resi.Max());//vector_e.Max());
		pTable->SetItemText(1,3,resi_mean);//sum(vector_e) / vector_e.vlen());
		pTable->SetItemText(1,4,resi_dev);//Dev(vector_e));
		pTable->SetItemText(1,5,int(sum_weight));//vector_e.vlen()
		
		CDoubleVector zpred = (y_hat - prey_mean)/prey_dev;
		
		pTable->SetItemText(2,1,zpred.Min());
		pTable->SetItemText(2,2,zpred.Max());
		pTable->SetItemText(2,3,0.0);//sum(zpred) / zpred.vlen());
		pTable->SetItemText(2,4,1.0);//Dev(zpred)
		pTable->SetItemText(2,5,int(sum_weight));//zpred.vlen());
		
		double zresi_dev = sqrt((sum_weight-WithConst-m_X_temp_nameAry.GetSize())/(sum_weight-WithConst));
		CDoubleVector zresi = y_resi*(zresi_dev/resi_dev);
		double zresi_mean;
		if (m_bWithWeight) 
			zresi_mean = sum(zresi * weight_v) / sum_weight;
		else
			zresi_mean = y_resi.mean();
		
		pTable->SetItemText(3,1,zresi.Min());//zresid.Min());
		pTable->SetItemText(3,2,zresi.Max());//zresid.Max());
		pTable->SetItemText(3,3,zresi_mean);//sum(zresid) / zresid.vlen());
		pTable->SetItemText(3,4,zresi_dev);//Dev(zresid)
		pTable->SetItemText(3,5,int(sum_weight));//zresid.vlen());
		
		CRsltElementTable * pETable= new CRsltElementTable("�в�ͳ�Ʊ�",pTable);
		m_pResult->Add(pETable);
	}
	

	//׼����ͼ
	int stepofj = 1;
	const int Max = 3000;
	if (m_nXRow > Max)
		stepofj = int(m_nXRow / Max);
	int nXRow = m_nXRow / stepofj;
	TDataPointValVal* pValueY;
	TDataPointValVal* pYHat;
	pValueY = new TDataPointValVal[nXRow];
	pYHat = new TDataPointValVal[nXRow];
	for(j=0; j<nXRow; j++)
	{
		pValueY[j].fXVal = j * stepofj + 1;
		pYHat[j].fXVal = j * stepofj + 1;
		pValueY[j].fYVal = vect_y(j * stepofj);
		pYHat[j].fYVal = y_hat(j * stepofj);
	}
	
	if (m_bChartLine && m_tVarInfo.iCount==1)//ֻ��һ��������һԪ��ʱ�������ͼ��
	{
		TDataPointValVal *pDPVV =new TDataPointValVal;        
		CDoubleVector XValue = data_read_resi(0+m_bWithWeight);
		for(int i=0; i<nXRow; i++)
		{
			pDPVV->fXVal = XValue(i * stepofj);
			pDPVV->fYVal = vect_y(i * stepofj);
		}
		CTChartPP* pPPPlot = new CTChartPP(CTChart::PP);
		pPPPlot->SetTitle(m_stry_name + "-���ͼ");
		pPPPlot->SetXAxilLabel("�Ա���");
		pPPPlot->SetYAxilLabel("�����");
		pPPPlot->SetData(pDPVV,nXRow,EScatter,ValVal);
		delete []pDPVV;
		double XMin = XValue.Min();
		double XMax = XValue.Max();
		double YMinValue = XMin*coefficiency(m_bWithConst) + m_bWithConst*coefficiency(0);
		double YMaxValue = XMax*coefficiency(m_bWithConst) + m_bWithConst*coefficiency(0);

		pDPVV = new TDataPointValVal[2];
		pDPVV[0].fXVal = XMin;
		pDPVV[0].fYVal = YMinValue;
		pDPVV[1].fXVal = XMax;
		pDPVV[1].fYVal = YMaxValue;
		pPPPlot->SetData(pDPVV,2,ELine,ValVal);
		CRsltElementChart *pREC = new CRsltElementChart(m_stry_name + "- ���ͼ", pPPPlot);
		m_pResult->Add(pREC);   
		delete []pDPVV;
	}
	
	if ((m_bChartLine) && m_tVarInfo.iCount >1)//�ع�Ч��ͼ
	{		
		CTChartLine *pTChartLine = new CTChartLine();
		pTChartLine->SetLine(pValueY, nXRow, ValVal,"ԭʼ����"); 
		pTChartLine->SetLine(pYHat, nXRow, ValVal,"�������"); 
		CRsltElementChart *pREC = new CRsltElementChart(m_stry_name + "-�ع�Ч��ͼ",  pTChartLine);
		m_pResult->Add(pREC);    
	}
	/*
	if (m_bChartLine)//Ԥ�����ͼ
	{
		CTChartLine* pTChartLine = new CTChartLine;
		pTChartLine->SetXAxilLabel("����");
		pTChartLine->SetYAxilLabel("����");
		CTString strTitle = "�ع�Ч��ͼ";
		CRsltElementChart *pRegFit = new CRsltElementChart(strTitle, pTChartLine);
		m_pResult->Add(pRegFit);

		pTChartLine->SetLine(pValueY, nXRow, ValVal, "ԭʼ����");
		pTChartLine->SetLine(pYHat, nXRow, ValVal, "�������");
	}//******************************* Ԥ�����ͼ END
	*/
	if (m_bChartScatterPlot)//�в�ɢ��ͼ
	{
		CTChartScatterPlot* pResScat = new CTChartScatterPlot;
		CRsltElementChart* pResidScat = new CRsltElementChart("�в�ɢ��ͼ", pResScat);
		
		TDataPointValVal* pResi;
		pResi = new TDataPointValVal[nXRow];
		for(int i=0; i<nXRow; i++)
		{
			pResi[i].fXVal = vect_y(i * stepofj);//Ԥ��);
			pResi[i].fYVal = vect_y(i * stepofj) - y_hat(i * stepofj);//ʵ��-Ԥ��);
		}
		m_pResult->Add(pResidScat);
		pResScat->SetScatter(pResi, nXRow, ValVal, "");
		delete []pResi;
	}
	//******************************* �в�ɢ��ͼ END
	if (m_bChartScatterPlot2)// �в���̬����ͼ
	{
		CDoubleVector AveVect;
		AveVect.create(nXRow);
		int i;
		for(i=0; i<nXRow; i++)
		{
			AveVect(i) = vect_y(i*stepofj) - y_hat(i*stepofj);
		}
		AveVect.Sort();
		CDoubleVector Norms = GetNormMedians(nXRow);
		CDoubleVector Beta = SimpleRegress(AveVect,Norms); 
		TDataPointValVal *pDPVV = new TDataPointValVal[nXRow];
			
		for(i=0; i<nXRow; i++)
		{
			pDPVV[i].fXVal = Norms(i);
			pDPVV[i].fYVal = AveVect(i);
		}
		CTChartPP* pPPPlot = new CTChartPP(CTChart::PP);	
		pPPPlot->SetTitle(m_stry_name + "-��̬����ͼ");
		pPPPlot->SetXAxilLabel("��̬��λ��");
		pPPPlot->SetYAxilLabel(m_stry_name);
		pPPPlot->SetData(pDPVV,nXRow,EScatter,ValVal);
		delete []pDPVV;
		pDPVV = new TDataPointValVal[2];
		double fxMin = 0, fxMax = 0;
		double fyMin = 0, fyMax = 0;
		Norms.MaxMin(fxMax,fxMin);
		AveVect.MaxMin(fyMax,fyMin);
		double bb = Beta(0);
		double kk = Beta(1);
		double rr = Beta(2);
		pDPVV[0].fXVal = fxMin;
		pDPVV[0].fYVal = fxMin*kk+bb;
		pDPVV[1].fXVal = fxMax;
		pDPVV[1].fYVal = fxMax*kk+bb;
		pPPPlot->SetData(pDPVV,2,ELine,ValVal);
		CRsltElementChart*pREC=new CRsltElementChart(m_stry_name + "-��̬����ͼ",pPPPlot);
		m_pResult->Add(pREC);
		delete []pDPVV;
	}
	//**************************** �в���̬����ͼ END	
	delete[] pValueY;//�ƶ������棬��ֹ��Ԥ��ʱ���ڴ�й© //#_S 2004-12-30 zlq
	delete[] pYHat;
	pValueY = pYHat = NULL;	
}


CDoubleMatrix CRegressionLinear::SelectMatrix(CDoubleVector SingVector,CDoubleMatrix SMatrix,int tot)//����SingVector��ѡ��SMatrix�е��к��У�SingVector�ĵ�i�д���0����ѡ�����i�к���������j�Ľ����
{
	CDoubleMatrix SNMatrix(tot, tot);
	int tt1 = 0;
	for (int i=0; i<SingVector.vlen(); i++)
	{
		if (SingVector(i)>0) 
		{
			int tt2 = 0;
			for (int j=0; j<=i; j++)
			{
				if (SingVector(j)>0) 
				{
					SNMatrix(tt1,tt2) = SMatrix(i,j);
					SNMatrix(tt2,tt1) = SMatrix(i,j);
					tt2++;
				}
			}
			tt1++;
		}
	}
	return SNMatrix;
}

CDoubleVector CRegressionLinear::SelectVector(CDoubleVector SingVector,CDoubleVector SVector, int tot)
{
	int tt=0;
	CDoubleVector SNVector(tot);
	for (int i=0; i<SingVector.vlen(); i++)
	{
		if (SingVector(i)>0) 
		{
			SNVector(tt)=SVector(i);
			tt++;
		}
	}
	return SNVector;
}

void CRegressionLinear::SelectStrAry(CDoubleVector SingVector,CTStringArray & m_strX_nameAry, CTStringArray &X_temp_nameAry)
{
	X_temp_nameAry.RemoveAll();
	for (int i=0; i<SingVector.vlen()-m_bWithConst; i++)
	{
		if (SingVector(i+m_bWithConst)>0) 
		{
			X_temp_nameAry.Add(m_strX_nameAry.GetAt(i));
		}
	}
}

void CRegressionLinear::RegLinear(CDoubleMatrix XX, CDoubleMatrix Xy, CDoubleMatrix CorrMatrix,double sum_weight, int XCol, int XRow, bool WithConst)
{
	m_df_residual = (((m_bWithWeight)?sum_weight:XRow) - XCol - WithConst);
	m_coefficiency = inv(XX) * Xy(0);
	m_SST = (WithConst)?m_var_y:m_ytimesy;
	m_SSE = fabs(m_ytimesy - dot(Xy,(VectorToMatrix(m_coefficiency)))(0,0));
	if (m_SSE<EPS_ACCUR)
		m_SSE = EPS_ACCUR;
	CDoubleMatrix temp_xx = CorrMatrix.slice(XCol,XCol);
	m_MSE = m_SSE/(m_df_residual);
	m_vector_sbeta = sqrt( m_MSE * diag( inv(XX)));
	if (XCol>0)
	{	
		m_vector_standbeta = ((inv(temp_xx)) * CorrMatrix.slice(0,XCol,XCol,1))(0);
		m_vector_T     = m_coefficiency / m_vector_sbeta;
		m_vector_Sig   = 2.0 * ( 1.0 - tcdf(abs(m_vector_T),m_df_residual));
		if (m_bCoefInter) 
		{
			double t_value = tinv((1+m_dConfidence)/2.0, m_df_residual);
			m_CoefLower = m_coefficiency - t_value*m_vector_sbeta;
			m_CoefUpper = m_coefficiency + t_value*m_vector_sbeta;
		}
	}
	m_R_square = 1- m_SSE/m_SST;
	m_R_adjust = 1 - m_SSE*(m_df_residual +m_nXCol)/(m_SST*(m_df_residual));
}

CDoubleMatrix CRegressionLinear::DelMatrix(CDoubleMatrix matrix_comein, int mm)
{
	if ((mm < matrix_comein.mrow())&&(mm<matrix_comein.mcol()) &&(mm>=0))
	{
		int ii,jj;
		CDoubleMatrix matrix(matrix_comein.mrow()-1,matrix_comein.mcol()-1);
		for (int i=0;i<matrix_comein.mcol();i++)
			for (int j=0;j<matrix_comein.mrow();j++)
			{
				if (i>mm)
					ii = i-1;
				else if (i<mm)
					ii = i;
				else
					continue;
				if (j>mm)
					jj = j-1;
				else if (j< mm)
					jj = j;
				else
					continue;
				matrix(jj,ii) = matrix_comein(j,i);
			}
			return matrix;
	}
	return matrix_comein;
}

CDoubleVector CRegressionLinear::mmthSelfReg(CDoubleMatrix matrix_comein, CDoubleVector Xmean, bool WithConst)//��Tolerance
{
	CDoubleVector Tolerance(matrix_comein.mcol()-WithConst);
	int num = matrix_comein.mrow();
	if ((matrix_comein.mcol()-WithConst) == 1) 
	{
		Tolerance(0)=1.0;
		return Tolerance;
	}
	int i;
	for (int mm=WithConst; mm<matrix_comein.mcol(); mm++)
	{
		CDoubleMatrix temp_ma1 = copy(matrix_comein);
		CDoubleMatrix matrix(matrix_comein.mrow()-1,matrix_comein.mcol());
		
		for (i=mm;i<num-1;i++)
			temp_ma1(i) = temp_ma1(i+1);
		temp_ma1 = temp_ma1.slice(num,num-1);
		CDoubleMatrix temp_ma = temp_ma1.trans();
		for (i=0; i<mm; i++)
			matrix(i) = temp_ma(i);
		for (i=mm; i<num-1; i++)
			matrix(i)=temp_ma(i+1);
		CDoubleVector te_xy = temp_ma(mm);
		CDoubleMatrix te_xx = matrix.slice(num-1, num-1);
		CDoubleVector coef = inv(te_xx) * te_xy;
		double sst_t = matrix_comein(mm,mm) - ((m_bWithWeight)?sum_weight:m_nXRow)* (WithConst?(Xmean(mm-WithConst)*Xmean(mm-WithConst)):0);
		double sse_t = matrix_comein(mm,mm) - te_xy.dot(coef);
		Tolerance(mm-WithConst) = sse_t/sst_t;
	}
	return Tolerance;
}


CDoubleVector CRegressionLinear::DelVector(CDoubleVector vector_comein, int mm)
{
	if (mm < vector_comein.vlen())
	{
		int ii;
		CDoubleVector vect(vector_comein.vlen()-1);
		for (int i=0;i<vector_comein.vlen();i++)
		{
			if (i>mm)
				ii = i-1;
			else if (i<mm)
				ii = i;
			else
				continue;
			vect(ii) = vector_comein(i);
		}
		return vect;
	}
	return vector_comein;
}


//��������
CDoubleMatrix CRegressionLinear::EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex)
{
	const double eps=0.0000000001;
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
		*m_EigVec = VectorToMatrix(m_VectEigVector,m_Row,m_Row).trans();
		return EigValue;
	}
}

void CRegressionLinear::InitParma()
{
	m_bStopCalculate = false;
	m_strErrorInfo = _T("");
}


void CRegressionLinear::DrawCorrolation(CDoubleMatrix Xy_Sigma)
{
	int i,j;
	double df;
	CDoubleMatrix CorrMatrix_org = Xy_Sigma/sqrt(Multi(diag(Xy_Sigma),diag(Xy_Sigma)));
	int XCol_org = Xy_Sigma.mrow()-1;
	if (m_bWithWeight) 
		df = sum_weight - 2;
	else
		df = m_nXRow - 2;
	CDoubleMatrix matrix_sig(XCol_org+1,XCol_org+1);
	for(i=0;i<XCol_org+1;i++)
	{
		for(j=0;j<XCol_org+1;j++)
		{
			if(( i == j)||(CorrMatrix_org(i,j) == 1) )
				matrix_sig(i,j) = 0;
			else
			{
				double t = CorrMatrix_org(i,j) * sqrt( ( df)/(1.0-CorrMatrix_org(i,j)*CorrMatrix_org(i,j)));
				matrix_sig(i,j) = 1.0 - tcdf(fabs(t),df);
			}
		}
	}
	CTLTable *pTable = new CTLTable;
	pTable->SetTitle("���ϵ����");
	pTable->CreateTable(2, 2);
	pTable->SetCols(Xy_Sigma.mcol()+2);
	pTable->SetRows(Xy_Sigma.mrow()*3+1);
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"");
	for(i=0;i<XCol_org;i++)
	{
		pTable->InsertColumn(i,m_strX_org[i]);
		pTable->InsertItem(i*3,m_strX_org[i],false);
		pTable->SetItemText( i*3, 1, "���ϵ��" );
		pTable->InsertItem(i*3+1,"",false);
		//pTable->SetItemText( i*3+1, 1, "������" );//���ݼ�������֪�˴�ӦΪ����������
		pTable->SetItemText( i*3+1, 1, "����������" );
		pTable->InsertItem(i*3+2,"");
		pTable->SetItemText( i*3+2, 1, "Э����" );
	}
	pTable->InsertColumn(i,m_stry_name);
	pTable->InsertItem(i*3,m_stry_name, false);
	pTable->SetItemText( i*3, 1, "���ϵ��" );
	pTable->InsertItem(i*3+1,"",false);
	//pTable->SetItemText( i*3+1, 1, "������" );//���ݼ�������֪�˴�ӦΪ����������
	pTable->SetItemText( i*3+1, 1, "����������" );
	pTable->InsertItem(i*3+2,"");
	pTable->SetItemText( i*3+2, 1, "Э����" );
	
	for(i=0;i<Xy_Sigma.mrow();i++)
	{
		for(j=0;j<Xy_Sigma.mcol();j++)
		{
			pTable->SetItemText(i*3,j+2,CorrMatrix_org(i,j));
			if( i!=j )
				pTable->SetItemText(i*3+1,j+2,matrix_sig(i,j));
			pTable->SetItemText(i*3+2,j+2,Xy_Sigma(i,j)/(df+2));
		}
	}
	CRsltElementTable * pETable= new CRsltElementTable("���ϵ����",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
}

void CRegressionLinear::DrawResult(int XCol, int WithConst)
{
	m_strArr_FinalXname.Copy(m_X_temp_nameAry);
	if (XCol >0) 
	{
		//�ع鷽��
		//-------------------------------
		CTString strEquation=m_stry_name+" = ";	
		CTString strTemp;
		int temp_cont=1;
		
		if (WithConst) 
		{
			strTemp.Format("%f",m_coefficiency(0));
			strEquation+=strTemp;
		}
		for (int i=0; i<XCol; i++)
		{    		
			strTemp.Format("%.4f*%s",m_coefficiency(i+WithConst),m_X_temp_nameAry.GetAt(i).GetData());
			if ((m_coefficiency(i+WithConst)>0) && (WithConst || (i>0)))
				strEquation+="+";
			if (strEquation.GetLength()+strTemp.GetLength() > temp_cont * 100)
			{
				strEquation =strEquation + "\n";
				temp_cont++;
			}
			
			strEquation+=strTemp;
		}
		CRsltElementTable * pETable = NULL;
		CTLTable * pTable1 = new CTLTable;
		pTable1->CreateTable(2, 2);	
		pTable1->SetTitle("�ع鷽��");
		pTable1->InsertColumn(0,strEquation);
		pETable= new CRsltElementTable("�ع鷽��",pTable1);//����һ���������Ķ���
		m_pResult->Add(pETable);
		//ģ�ͷ���
		//-------------------------------
		
		CTLTable * pTable2 = new CTLTable;
		pTable2->CreateTable(2, 2);
		pTable2->SetTitle("ģ�ͷ���");
		pTable2->SetRows(2);
		pTable2->SetCols(2);
		pTable2->InsertColumn(0,"");
		pTable2->InsertColumn(0,"R");
		pTable2->InsertColumn(0,"R ƽ��");
		pTable2->InsertColumn(0,"������R ƽ��");
		pTable2->InsertColumn(0,"���Ƶı�׼��");
		CTString str;
		str.Format("%f/4",sqrt(m_R_square));
		pTable2->InsertItem(0,"");
		pTable2->SetItemText(0,1,sqrt(m_R_square));
		pTable2->SetItemText(0,2,m_R_square);
		pTable2->SetItemText(0,3,m_R_adjust);
		pTable2->SetItemText(0,4,sqrt(m_MSE));
		CRsltElementTable * pETable2 = NULL;
		pETable2 = new CRsltElementTable("ģ�ͷ���",pTable2);//����һ���������Ķ���
		m_pResult->Add(pETable2);
		CTString strInfl="",strVars="";//"�Ա���";
		strVars+=m_X_temp_nameAry.GetAt(0);
		for (int j=1;j<m_X_temp_nameAry.GetSize();j++)
			strVars+=","+m_X_temp_nameAry.GetAt(j);
		strVars+="��"+m_stry_name;
		strInfl.Format("�����Ӱ��Ϊ%f",m_R_square*100);
		
		if (briefrTable)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"ģ����"+strVars+strInfl+"%");	
		//-------------------------------
		//���������
		//--------------------------------
		CTLTable * pTable3 = new CTLTable ;
		pTable3->CreateTable(2, 2);
		pTable3->SetRows(4);
		pTable3->SetCols(6);
		pTable3->SetTitle("���������");
		pTable3->InsertColumn(0,"");
		pTable3->InsertColumn(0,"ƽ����");
		pTable3->InsertColumn(0,"���ɶ�");
		pTable3->InsertColumn(0,"����");
		pTable3->InsertColumn(0,"Fֵ");
		pTable3->InsertColumn(0,"������");
		
		pTable3->InsertItem(0,"�ع�");
		pTable3->InsertItem(1,"�в�");
		pTable3->InsertItem(2,"�ܺ�");
		pTable3->SetItemText(0,1,(m_SST-m_SSE));
		pTable3->SetItemText(1,1,(m_SSE));
		pTable3->SetItemText(2,1,m_SST);
		
		pTable3->SetItemText(0,2,XCol);
		pTable3->SetItemText(1,2,int(m_df_residual));
		pTable3->SetItemText(2,2,int(XCol + m_df_residual));
		pTable3->SetItemText(0,3,((m_SST-m_SSE)/XCol));
		pTable3->SetItemText(1,3,m_MSE);
		pTable3->SetItemText(0,4,(m_SST-m_SSE)/XCol/m_MSE);
		double pofFvalue;
		pofFvalue=1 - fcdf((m_SST-m_SSE)/XCol/m_MSE, XCol, m_df_residual);
		pTable3->SetItemText(0,5,pofFvalue);
		CRsltElementTable * pETable3= new CRsltElementTable("���������",pTable3);
		m_pResult->Add(pETable3);
		//--------------------------------
		//�ع�ϵ������
		//---------------------------------
		CTLTable * pTable4 = new CTLTable;
		pTable4->CreateTable(2, 2);
		pTable4->SetTitle("�ع�ϵ������");
		pTable4->SetRows(2);
		pTable4->SetCols(10);
		pTable4->InsertColumn(0,"");
		pTable4->InsertColumn(1,"�ع�ϵ��");
		pTable4->InsertColumn(2,"��׼��");
		pTable4->InsertColumn(3,"��׼����\nbeta");
		pTable4->InsertColumn(4,"t");
		pTable4->InsertColumn(5,"������");
		CTString strInterval1;
		CTString strInterval2;	
		if (m_bCoefInter)
		{
			strInterval1.Format("%.f%%��������\n�Ͻ�",m_dConfidence*100);
			strInterval2.Format("%.f%%��������\n�½�",m_dConfidence*100);
			pTable4->InsertColumn(6,strInterval1);
			pTable4->InsertColumn(7,strInterval2);
		}
		if (m_bWithColl) 
		{
			pTable4->InsertColumn(6+m_bCoefInter*2,"������������");
			pTable4->InsertColumn(7+m_bCoefInter*2,"���̶�");	
		}
		
		if (WithConst)
		{
			pTable4->InsertItem(0,"������");
			pTable4->SetItemText(0,1,m_coefficiency(0));
			pTable4->SetItemText(0,2,m_vector_sbeta(0));
			//pTable4->SetItemText(k,3,m_vector_standbeta(k));
			pTable4->SetItemText(0,4,m_vector_T(0));
			pTable4->SetItemText(0,5,m_vector_Sig(0));
			if (m_bCoefInter) 
			{
				pTable4->SetItemText(0,6,m_CoefUpper(0));
				pTable4->SetItemText(0,7,m_CoefLower(0));
			}
		}
		for (int k = WithConst ; k<XCol+WithConst;k++)
		{
			pTable4->InsertItem(k,m_X_temp_nameAry[k-WithConst]);
			pTable4->SetItemText(k,1,m_coefficiency(k));
			pTable4->SetItemText(k,2,m_vector_sbeta(k));
			pTable4->SetItemText(k,3,m_vector_standbeta(k-WithConst));
			pTable4->SetItemText(k,4,m_vector_T(k));
			pTable4->SetItemText(k,5,m_vector_Sig(k));	
			if (m_bCoefInter) 
			{
				pTable4->SetItemText(k,6,m_CoefUpper(k));
				pTable4->SetItemText(k,7,m_CoefLower(k));
			}
			if (m_bWithColl) 
			{			
				pTable4->SetItemText(k,6+m_bCoefInter*2,1/m_Tolerance(k-WithConst));
				pTable4->SetItemText(k,7+m_bCoefInter*2,m_Tolerance(k-WithConst));
			}
		}
		CRsltElementTable * pETable4= new CRsltElementTable("�ع�ϵ������",pTable4);
		m_pResult->Add(pETable4);
		//---------------------------------
		
		
		//���������
		//----------------------------------
		if (m_bWithColl) 
		{
			CTLTable * pTable6 = new CTLTable;	
			pTable6->CreateTable(2, 2);
			pTable6->SetTitle("���������");
			pTable6->InsertColumn(0,"");
			pTable6->InsertColumn(1,"����ֵ");
			pTable6->InsertColumn(2,"����ָ��");
			pTable6->SetRows(4);
			pTable6->SetCols(3);
			//		if (WithConst)
			//		{
			//			pTable6->InsertColumn(0,"�������\n(������)");
			//		}
			//		for (int t=WithConst; t<XCol+WithConst;t++)
			//		{
			//			pTable6->InsertColumn(0,(_T("�������\n(") + m_X_temp_nameAry[t-WithConst] +_T(")") ));
			//		}
			
			for (int k = 0; k<XCol+WithConst;k++)
			{
				strTemp.Format("%d",(k+1));
				pTable6->InsertItem(k,strTemp);
				pTable6->SetItemText(k,1,m_useforcoll_e(k));
				pTable6->SetItemText(k,2,m_vector_condindex(k));
			}
			
			CRsltElementTable * pETable6= new CRsltElementTable("���������",pTable6);//����һ���������Ķ���
			m_pResult->Add(pETable6);
		}
	}
	else
	{
		CTString strWarning = "ע�� ֻ�г��������ع飬�ݲ�����ع���������";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}			
}

bool CRegressionLinear::IsExisted(CDataInterface *p,CTString str)
{
	for(int i=0;i<p->GetColCount();i++)
	{
		if(p->GetFieldName(i)==str)
		{
			return true;
		}
	}
	return false;
}

void CRegressionLinear::Collinearity(CDoubleMatrix m_XtimesX, int XCol, bool WithConst)
{		//���������
	CDoubleMatrix useforcoll_matrix = m_XtimesX.copy();
	for (int i=0; i<XCol+WithConst; i++)
		useforcoll_matrix(i) = useforcoll_matrix(i) / useforcoll_matrix(i,i);
	CComplexMatrix useforcoll_m;//�������������󸴣�
	CComplexMatrix useforcoll_v;//����������(��)
	useforcoll_v = eig(useforcoll_matrix, useforcoll_m);
	CComplexVector useforcoll_eig = diag(useforcoll_v);
	m_useforcoll_e = (useforcoll_eig.real()).Sort(1);
	m_vector_condindex = sqrt( m_useforcoll_e.Max() / m_useforcoll_e);//������
}

void CRegressionLinear::DrawTableHead(int XCol, int WithConst)
{
	if (XCol >0) 
	{
		//-------------------------------
		//�ع鷽��
		//-------------------------------
		pTable1 = new CTLTable;
		pTable1->CreateTable(2, 2);	
		pTable1->SetTitle("�ع鷽��");
		pTable1->InsertColumn(0, "");
		pTable1->InsertColumn(0, "�ع鷽��");
		//-------------------------------
		//ģ�ͷ���
		//-------------------------------		
		pTable2 = new CTLTable;
		pTable2->CreateTable(2, 2);
		pTable2->SetTitle("ģ�ͷ���");
		pTable2->InsertColumn(0, "");
		pTable2->InsertColumn(0, "R");
		pTable2->InsertColumn(0, "R ƽ��");
		pTable2->InsertColumn(0, "������R ƽ��");
		pTable2->InsertColumn(0, "���Ƶı�׼��");
		//-------------------------------
		//���������
		//--------------------------------
		pTable3 = new CTLTable ;
		pTable3->CreateTable(2, 2);
		pTable3->SetTitle("���������");
		pTable3->InsertColumn(0, "");
		pTable3->InsertColumn(0, "");
		pTable3->InsertColumn(0, "ƽ����");
		pTable3->InsertColumn(0, "���ɶ�");
		pTable3->InsertColumn(0, "����");
		pTable3->InsertColumn(0, "Fֵ");
		pTable3->InsertColumn(0, "������");
		//--------------------------------
		//�ع�ϵ������
		//---------------------------------
		pTable4 = new CTLTable;
		pTable4->CreateTable(2, 2);
		pTable4->SetTitle("�ع�ϵ������");
		pTable4->InsertColumn(0, "");
		pTable4->InsertColumn(0, "");
		pTable4->InsertColumn(0, "�ع�ϵ��");
		pTable4->InsertColumn(0, "��׼��");
		pTable4->InsertColumn(0, "��׼����\nbeta");
		pTable4->InsertColumn(0, "t");
		pTable4->InsertColumn(0, "������");
		CTString strInterval1;
		CTString strInterval2;	
		if (m_bCoefInter)
		{
			strInterval1.Format("%.f%%��������\n�Ͻ�", m_dConfidence * 100);
			strInterval2.Format("%.f%%��������\n�½�", m_dConfidence * 100);
			pTable4->InsertColumn(0, strInterval1);
			pTable4->InsertColumn(0, strInterval2);
		}
		if (m_bWithColl) 
		{
			pTable4->InsertColumn(0 /*+ m_bCoefInter * 2*/, "������������");
			pTable4->InsertColumn(0 /*+ m_bCoefInter * 2*/, "���̶�");	
		}
		//---------------------------------	
		//���������
		//----------------------------------
		if (m_bWithColl) 
		{
			pTable6 = new CTLTable;	
			pTable6->CreateTable(2, 2);
			pTable6->SetTitle("���������");			
			pTable6->InsertColumn(0,"");
			pTable6->InsertColumn(0,"");			
			pTable6->InsertColumn(0,"����ֵ");
			pTable6->InsertColumn(0,"����ָ��");
		}
	}
	else
	{
		CTString strWarning = "ע�� ֻ�г��������ع飬�ݲ�����ع���������";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}
}

void CRegressionLinear::DrawTableBody(int XCol, int WithConst, int &number, CTString strWarning)
{
	m_strArr_FinalXname.Copy(m_X_temp_nameAry);
	if (XCol >0) 
	{
		//�ع鷽��
		//-------------------------------
		CTString strEquation=m_stry_name+" = ";	
		CTString strTemp;
		int temp_cont=1;
		
		CTString strstr;
		strstr.Format("%d", number+1);
		
		if (WithConst) 
		{
			strTemp.Format("%f",m_coefficiency(0));
			strEquation+=strTemp;
		}
		for (int i=0; i<XCol; i++)
		{    		
			strTemp.Format("%.4f*%s",m_coefficiency(i+WithConst),m_X_temp_nameAry.GetAt(i).GetData());
			if ((m_coefficiency(i+WithConst)>0) && (WithConst || (i>0)))
				strEquation+="+";
			if (strEquation.GetLength()+strTemp.GetLength() > temp_cont * 100)
			{
				strEquation =strEquation + "\n";
				temp_cont++;
			}
			
			strEquation+=strTemp;
		}
		if(strWarning != "")
			strEquation = "( " + strWarning + ")" + "\n" +strEquation;
		pTable1->InsertItem(number, strstr, true);
		pTable1->SetItemText(number, 1, strEquation);
		
		//-------------------------------
		//ģ�ͷ���
		//-------------------------------
		CTString str;
		str.Format("%f/4",sqrt(m_R_square));
		pTable2->InsertItem(number,strstr,true);
		pTable2->SetItemText(number,1,sqrt(m_R_square));
		pTable2->SetItemText(number,2,m_R_square);
		pTable2->SetItemText(number,3,m_R_adjust);
		pTable2->SetItemText(number,4,sqrt(m_MSE));
		CTString strInfl="",strVars="";//"�Ա���";
		strVars+=m_X_temp_nameAry.GetAt(0);
		for (int j=1;j<m_X_temp_nameAry.GetSize();j++)
			strVars+=","+m_X_temp_nameAry.GetAt(j);
		strVars+="��"+m_stry_name;
		strInfl.Format("�����Ӱ��Ϊ%f",m_R_square*100);
		
		if (briefrTable)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"ģ����"+strVars+strInfl+"%",false);	
		//-------------------------------
		//���������
		//--------------------------------
		pTable3->InsertItem(3*number,strstr,false);
		pTable3->InsertItem(3*number+1,"",false);
		pTable3->InsertItem(3*number+2,"",true);
		pTable3->SetItemText(3*number,1,"�ع�");
		pTable3->SetItemText(3*number+1,1,"�в�");
		pTable3->SetItemText(3*number+2,1,"�ܺ�");
		pTable3->SetItemText(3*number,2,(m_SST-m_SSE));
		pTable3->SetItemText(3*number+1,2,(m_SSE));
		pTable3->SetItemText(3*number+2,2,m_SST);		
		pTable3->SetItemText(3*number,3,XCol);
		pTable3->SetItemText(3*number+1,3,int(m_df_residual));
		pTable3->SetItemText(3*number+2,3,int(XCol + m_df_residual));
		pTable3->SetItemText(3*number,4,((m_SST-m_SSE)/XCol));
		pTable3->SetItemText(3*number+1,4,m_MSE);
		pTable3->SetItemText(3*number,5,(m_SST-m_SSE)/XCol/m_MSE);
		double pofFvalue;
		pofFvalue=1 - fcdf((m_SST-m_SSE)/XCol/m_MSE, XCol, m_df_residual);
		pTable3->SetItemText(3*number,6,pofFvalue);
		//--------------------------------
		//�ع�ϵ������
		//---------------------------------
		if(XCol == 1 && !WithConst)//2006.01.17  add && !WithConst
			pTable4->InsertItem(m_nLine,strstr);
		else
			pTable4->InsertItem(m_nLine,strstr,false);
		if (WithConst)//����г�����
		{
			pTable4->SetItemText(m_nLine,1,"������");
			pTable4->SetItemText(m_nLine,2,m_coefficiency(0));
			pTable4->SetItemText(m_nLine,3,m_vector_sbeta(0));
			pTable4->SetItemText(m_nLine,5,m_vector_T(0));
			pTable4->SetItemText(m_nLine,6,m_vector_Sig(0));
			if (m_bCoefInter) 
			{
				pTable4->SetItemText(m_nLine,7,m_CoefUpper(0));
				pTable4->SetItemText(m_nLine,8,m_CoefLower(0));
			}
			if(XCol == 1)//2006.01.17  add
				pTable4->InsertItem(++m_nLine,"",true);	//2006.01.17  add
			else//2006.01.17  add
				pTable4->InsertItem(++m_nLine,"",false);	
		}
		for (int k = WithConst ; k<XCol+WithConst;k++)
		{
			pTable4->SetItemText(m_nLine,1,m_X_temp_nameAry[k-WithConst]);
			pTable4->SetItemText(m_nLine,2,m_coefficiency(k));
			pTable4->SetItemText(m_nLine,3,m_vector_sbeta(k));
			pTable4->SetItemText(m_nLine,4,m_vector_standbeta(k-WithConst));
			pTable4->SetItemText(m_nLine,5,m_vector_T(k));
			pTable4->SetItemText(m_nLine,6,m_vector_Sig(k));	
			if (m_bCoefInter) 
			{
				pTable4->SetItemText(m_nLine,7,m_CoefUpper(k));
				pTable4->SetItemText(m_nLine,8,m_CoefLower(k));
			}
			if (m_bWithColl) 
			{			
				pTable4->SetItemText(m_nLine,7+m_bCoefInter*2,1/m_Tolerance(k-WithConst));
				pTable4->SetItemText(m_nLine,8+m_bCoefInter*2,m_Tolerance(k-WithConst));
			}
			m_nLine++;
			if(k+2<XCol+WithConst)
			{
				pTable4->InsertItem(m_nLine,"",false);				
			}
			else if(k+1<XCol+WithConst)
			{
				pTable4->InsertItem(m_nLine,"");
			}
		}
		//---------------------------------
		//���������
		//----------------------------------
		if (m_bWithColl) 
		{
			if(XCol == 1)
				pTable6->InsertItem(m_nColl,strstr);
			else
				pTable6->InsertItem(m_nColl,strstr,false);
			for (int k = 0; k<XCol+WithConst;k++)
			{
				strTemp.Format("%d",(k+1));
				pTable6->SetItemText(m_nColl,1,strTemp);
				pTable6->SetItemText(m_nColl,2,m_useforcoll_e(k));
				pTable6->SetItemText(m_nColl,3,m_vector_condindex(k));
				m_nColl++;
				if(k+2<XCol+WithConst)
				{
					pTable6->InsertItem(m_nColl,"",false);				
				}
				else if(k+1<XCol+WithConst)
				{
					pTable6->InsertItem(m_nColl,"");				
				}
			}			
		}
		number++;
	}
}

void CRegressionLinear::DrawTable(int XCol, int WithConst)
{
	//	if(XCol > 0)
	{
		CRsltElementTable *  pETable1= new CRsltElementTable("�ع鷽��",pTable1);//����һ���������Ķ���
		m_pResult->Add(pETable1);
		CRsltElementTable *  pETable2 = new CRsltElementTable("ģ�ͷ���",pTable2);//����һ���������Ķ���
		m_pResult->Add(pETable2);
		CRsltElementTable *  pETable3= new CRsltElementTable("���������",pTable3);
		m_pResult->Add(pETable3);
		CRsltElementTable * pETable4= new CRsltElementTable("�ع�ϵ������",pTable4);
		m_pResult->Add(pETable4);	
		if (m_bWithColl) 
		{		
			CRsltElementTable * pETable6= new CRsltElementTable("���������",pTable6);//����һ���������Ķ���
			m_pResult->Add(pETable6);
		}
	}
}

//���²�������Ԥ��ġ�
CResult* CRegressionLinear::OnRegPred(CDataInterface * pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	m_pResult = new CResult("���Իع�ģ��Ԥ����");
	if (!GetPredValueVo(pDataInterface, VO))//��������ģ��
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( "ģ�Ͳ��������޷����С�" );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	CDWordArray indexArr;
	int nCol = m_tVarPred.iCount;
	int i=0;
	for (i=0; i<m_tVarPred.iCount; i++)
		indexArr.Add(m_tVarPred.pValue[i]);
	CDoubleMatrix Pred_Mx;
	int nRow = pDataInterface->GetColsData(indexArr, Pred_Mx, 1);//������
	if (m_bWithConst && (Pred_Mx.mcol() < m_coefficiency.vlen()) )
	{
		CDoubleVector ConstOne(nRow, 1.0);
		Pred_Mx = ConstOne | Pred_Mx;
	}
	CDoubleVector yPred = Pred_Mx * m_coefficiency;//Ԥ��ֵ
	CDoubleVector yRange;
	CDoubleVector yRangeUp;
	CDoubleVector yRangeLow;
	if (m_bPredRange)//���Ҫ���Ԥ��ֵ����������
	{	
		CDoubleMatrix X_inv = inv(m_XtimesX);
		double tValue = tinv( (1+m_dConfidence)/2.0, sum_weight -m_X_temp_nameAry.GetSize()-m_bWithConst);

		yRange.create(nRow);
		CDoubleVector vecTemp(m_tVarPred.iCount+m_bWithConst, 1.0);
		for (i=0; i<nRow; i++)//����Ԥ��ֵ
		{
			for (int j=0; j<nCol; j++)
				vecTemp(j+m_bWithConst) = Pred_Mx(i,j);
			yRange(i) = tValue*sqrt(1+((VectorToMatrix(vecTemp).trans())*X_inv*VectorToMatrix(vecTemp))(0,0))*sqrt(m_MSE);
		}
		yRangeUp = yPred + yRange;
		yRangeLow = yPred - yRange;
	}
//	������������ӣ�ʼ
	CTString PredUpName = m_strDepPred + "_�Ͻ�";
	CTString PredLowName = m_strDepPred + "_�½�";
	//Ԥ���������
	if (m_pDataInterface->m_DataAccess.FieldByName(m_strDepPred) != NULL)
	{
		int nTemp = m_pDataInterface->m_DataAccess.IndexByName(m_strDepPred);
		m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
	}	
	CField *PredField = pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
	PredField->SetFieldName(m_strDepPred);
	pDataInterface->m_DataAccess.m_pFieldList->Add(PredField);
	CField *PredUpField;
	CField *PredLowField;
	if (m_bPredRange)
	{
		if (m_pDataInterface->m_DataAccess.FieldByName(PredUpName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(PredUpName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}	
		if (m_pDataInterface->m_DataAccess.FieldByName(PredLowName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(PredLowName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}	
		PredUpField = pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		PredLowField = pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		PredUpField->SetFieldName(PredUpName);
		PredLowField->SetFieldName(PredLowName);
	}
	pDataInterface->m_DataAccess.First();
	for (i=0; i<nRow; i++)
	{
		pDataInterface->m_DataAccess.Edit();
		PredField->SetAsDouble(yPred(i));
		if (m_bPredRange)
		{
			PredUpField->SetAsDouble(yRangeUp(i));
			PredLowField->SetAsDouble(yRangeLow(i));
		}
		//pDataInterface->m_DataAccess.Post();			
		pDataInterface->m_DataAccess.Next();
	}
	//TEST �ã�����
	CTString szNowFile("");
	CTString szNewFile("");
	szNowFile = pDataInterface->m_DataAccess.GetFileName();
	bool bOld = pDataInterface->m_DataAccess.SaveFile(szNowFile,&szNewFile);
	//��ʱ:ΪNewFile
	//��ʱ:ΪNowFile.
	CTString SaveFileName;
	SaveFileName.Format("���Իع�Ԥ��������ɣ�������ļ���Ϊ��%s\n",bOld ? szNowFile.GetData() : szNewFile.GetData());
	CRsltElementText *pText = new CRsltElementText("���Իع�Ԥ��");//����һ���������Ķ���
	pText->AddString(SaveFileName);
	m_pResult->Add(pText);
	return m_pResult;
}

bool CRegressionLinear::GetPredValueVo(CDataInterface * pDataInterface, string VO)//m_strModel��Ϊģ���Ѿ����㷨�д�����
{
//	REGLINEARPRED
//		/DATA=[FILE]
//		/MODELFROMFILE [FILE]
//		/VARLIST [varlist]
//		/PREDNAME [strname]
// 		/REPLACEVARNAME [nameorginal namepred]...
//		/PREDRANGE CIN(0.95)
	
	bool bOK = false;
	AnaWord VOstr;
	VOstr.Import(VO);
	
//		/MODELFROMFILE [FILE]
	if (AnaSyntax::exist("/MODELFROMFILE", VOstr))//���ļ���ģ��
	{
		CTString szFileName = AnaSyntax::getSingleStr("/MODELFROMFILE",VOstr);
		if(GetValueFromModel(pDataInterface, szFileName.GetData()))
			bOK = true;
	}
	if (!bOK)
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( "���õ�ģ���д���" );
		m_pResult->Add( pWarningTextRslt );
	}
//		/VARLIST [varlist]
	CTStringArray ynames;
	AnaSyntax::getVAR("/VARLIST", VOstr, ynames);
// 		/REPLACEVARNAME [nameorgi namepred]...
	CTStringArray replacestr;
	AnaSyntax::getVAR("/REPLACEVARNAME", VOstr, replacestr);
	if (replacestr.GetSize()%2 != 0)
		return false;
//���������򣬴��µõ�����ֵ��
	if (!AnaSyntax::Replace(m_strX_nameAry, ynames, replacestr))
		return false;
	pDataInterface->getVarID(m_strX_nameAry, m_tVarPred);
//		/PREDNAME [strname]
	if (AnaSyntax::exist("/PREDNAME", VOstr))
		m_strDepPred = AnaSyntax::getSingleStr("/PREDNAME", VOstr);
	else
		m_strDepPred = m_stry_name+"Ԥ��ֵ";
	if (m_strDepPred == "")
		return false;
//		/PREDRANGE CIN(0.95)
	if (AnaSyntax::exist("/PREDRANGE", VOstr))
	{
		m_bPredRange = true;
		m_dConfidence = AnaSyntax::getvalue("/PREDRANGE", "CIN",VOstr);
		if (m_dConfidence>=1 || m_dConfidence<EPS_ACCUR)
			m_dConfidence = 0.95;
	}
	else
		m_bPredRange = false;
	if (ynames.GetSize() != m_strX_nameAry.GetSize())
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString("�ṩ���Ա�����������ģ�����Ա����ĸ�����" );
		m_pResult->Add( pWarningTextRslt );
	}

	return true;
}

bool CRegressionLinear::GetValueFromModel(CDataInterface* pDataInterface, string Model)
{
	int i=0, nSize=0;
	CTString szText("");
	string szTemp;
	TCLFile theFile;
	
	bool bOpen = theFile.Open(Model.c_str(), TCLFile::modeRead);
	if (!bOpen)
		return false;
	//��β����
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//��ʼдģ�Ͳ���
	theFile >> m_bWithConst;
	//�Ա���
	theFile >> nSize;
	m_coefficiency.create(nSize);
	//ϵ��
	for (i=0; i<nSize; i++)
		theFile >> m_coefficiency(i);
	//�������
	m_strX_nameAry.RemoveAll();
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> szText;
		m_strX_nameAry.Add(szText);
	}
	//����
	int nRow = 0, nCol = 0;
	theFile >> nRow;
	theFile >> nCol;
	m_XtimesX.create(nRow,nCol);
	for (i=0; i<nRow; i++)
	{
		for (int j=0; j<nCol; j++)
		{
			theFile >> m_XtimesX(i, j);
		}
	}
	//Ŀ�����
	theFile >> m_stry_name;
	//�в�
	theFile >> m_MSE;
	theFile.Close();
	return true;
}
