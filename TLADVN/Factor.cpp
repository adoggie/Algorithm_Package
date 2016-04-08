// Factor.cpp: implementation of the CFactor class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tlAdvn.h"
#include "Factor.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const double EPS=0.000001;
const int ID=25;
CFactor temp;

CFactor::CFactor()
{
	m_pResult			= NULL;

	m_bOutput			= false;
	m_bComBigOne        = false;
	m_bRotateFlag       = false;
	m_bCompenFlag       = false;
	m_bEigenFlag        = false;
	m_bZhengjiao        = true;
	m_bKMO              = true;	//2003-1-25 Eric ��ʱ������ȷ������
	m_bAntiImage		= true;
	m_bAntiImage2       = true;
	m_bShengyu			= true;	//2003-1-25 Eric �в�
	m_bZaisheng			= true;	//2003-1-25 Eric ����
	
	m_nProcType			= 1;//���ɷַ���--2,���ӷ���--1.
	
	m_nRow				= 0;
	m_nCol				= 0;
	m_nSel              = 0;
	m_nIteration        = 0;
	m_dkmo              = 0;
	m_dExtact			= 0.8;
	m_fPro              = (float)0.9;
	m_nMissingMethod    = 0;
	
	m_bCovarian			= false;
}

CFactor::~CFactor()
{
	InitFree();
}

bool IsExisted(CDataInterface *p,CTString str)
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
//////////////////////////////////////////////////////////////////////////
//              ���ɷַ����ļ��㺯��
//////////////////////////////////////////////////////////////////////////

bool CFactor::GetValueFromVOPrin(CDataInterface *pDataInterface, std::string VO)
{
//	PCOMPONENT
//		/DATA=[FILE]
//		/VARLIST=[varlist]
//		/MISSING=[{NONE**}]
//		/METHOD=[{CORR**},{COV}]
//		/RULE=[{CUMULANT(0.9**)},{NUMFACTOR(1)}]
//		/PRINT [FACTORS]
	m_bAntiImage = FALSE;
	m_bAntiImage2 = FALSE;
	m_bShengyu = FALSE;
	m_bZaisheng = FALSE;
	m_nScoreMethod			= 0;
	m_nRotateMethod			= 0;
	m_nExtractMethod		= 0;
	m_nMaxIterCount = 100;
	m_nIteration = 0;
	m_bScore = FALSE;
	m_bKMO = TRUE;

	AnaWord VOstr;
	VOstr.Import(VO);
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo2)))
		return FALSE;
	m_nMissingMethod = 0;
	m_fPro = 0.9;
	m_nExtractStandard = 0;
	m_nAnaMethod = int(AnaSyntax::findstr("/METHOD", "COV",VOstr));
	if (AnaSyntax::findstr("/RULE", "NUMFACTOR",VOstr)) 
	{
		m_nExtractStandard = 1;
		m_nFactorNum = AnaSyntax::getvalue("/RULE", "NUMFACTOR",VOstr);
	}
	if (AnaSyntax::findstr("/RULE", "CUMULANT",VOstr))
		m_fPro = AnaSyntax::getvalue("/RULE", "CUMULANT",VOstr);
	m_bOutput = AnaSyntax::findstr("/PRINT", "FACTORS",VOstr);
	return TRUE;
}



CResult * CFactor::OnFactor2(CDataInterface *pDataInterface, std::string &VO)
{	
	m_bPrincipal=TRUE;//zkc 2003.8.20
	m_strResultExplain.RemoveAll();
	m_pOriDataInterface = pDataInterface;
	m_nProcType = 2;
	m_bOutput = FALSE;
	m_bComBigOne = FALSE;
	m_pResult = NULL;
	m_pResult = new CResult("���ɷݷ���");
	if (!(GetValueFromVOPrin(pDataInterface, VO)))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	//ȡ����
	m_VectInputData = GetData(pDataInterface,m_tVarInfo2);//ͬʱ��m_pDataInterface��ֵ
	if(!m_VectInputData.vlen())
	{
		delete m_pResult;
		m_pResult = NULL;
		OnFactorExit();
		return NULL;
	}
	//����Ȩ��.
	int i = 0;
	for(i = 0;i<m_weight.vlen();i++)
	{
		if(m_weight(i)<0)
			m_weight[i] = 0;
	}
	//WriteData(m_weight,"weight");
	
	CPtrArray arField;
	int * pt;
	for(i=0;i<m_tVarInfo2.iCount;i++)
	{
		pt = new int;
		*pt = m_tVarInfo2.pValue[i];
		arField.Add(pt);
	}
	
	
	int m          = m_tVarInfo2.iCount;
	CDoubleMatrix matrix = VectorToMatrix(m_VectInputData ,m_nRow,m);//Ywj  2005-05-09
	this->SetRowsCols(m_nRow,m);
	m_VectData = copy(m_VectInputData);
	//ȡ����
	m_VectDataFormlized = FormlizeEx(m_VectInputData,m_weight);
	
	
	double dbKappa          = 4;
	
	if(m_fPro > 1)
	{
		m_fPro = 1;
	}
	if(m_fPro<0)
	{
		m_fPro = 0;
	}
	
	//������Ӧ�ļ���
	//1:������ȡ��		
	ComponMethod(m_nAnaMethod);//0:���ɷַ� ; 1:�����ӷ�
	
	//ע:������������ת
	//2:��������	
	CDoubleMatrix ResortMatrix = VectorToMatrix(m_VectComMatrix,m_nCol,m_nSel);
	m_VectRotateCom.create( ResortMatrix.mrow()*ResortMatrix.mcol());
	ProcessRotateMatrix(ResortMatrix);
	m_VectRotateCom = MatrixToVector(ResortMatrix);
	//3:���ɷ�
	PrincipalComponent();//����m_pScore��.
	
//ʹ����m_pScore;
	if (m_bOutput)
		PrincipalTable(m_pResult);
	
	CreateResult(m_pResult);
	
	KMO(1);
	Sphericity(matrix);		
	OnFactorExit();

	int nOutNote = 0;//�Ƿ���ʾע��,0 ����ʾ,������ʾ #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
	if (nOutNote)
	{				
		CTLTable* pTable = new CTLTable;
		pTable->InsertColumn(0,"���˵��");
		for(i=0;i<m_strResultExplain.GetSize();i++)
		{
			pTable->InsertItem(i,m_strResultExplain.GetAt(i));
		}
		m_pResult->InsetAt(0,new CRsltElementTable("���˵��",pTable));
	}

	m_bComBigOne = false;
	m_bCovarian = false;
	m_bScore = false;
	m_nRotateMethod = 0;
	
	return m_pResult;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//	�������ֵ,pData:��������ָ��
/////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::Eigenvalue(CDoubleVector& Vect_Data)
{
	m_VectEigenValue.create(m_nCol);
	m_EigenVector.create(m_nCol*m_nCol); // Ywj  2005-05-09
	cjcbj(Vect_Data,m_nCol,m_EigenVector,0.00001);//�����������m_pEigenVector
	SortEigenVect(Vect_Data,m_EigenVector,m_nCol,m_nCol,1);//������������m_pEigenVector
	int i =0;
	for(i =0;i<m_nCol;i++)
		m_VectEigenValue[i] = Vect_Data[i+i*m_nCol];
	
	CreateEigenValueResult(Vect_Data,m_nCol);
	//TraceVector(Vect_Data);
	//TraceVector(m_VectEigenValue);
	m_nSel = GetSelNum(m_VectEigenValue);//�������Ҫ�������ֵ�ĸ���
	//TraceVector(m_EigenVector);
	
//	double sum(0);
//	for( i=0;i<m_nCol;i++)
//	{
//		sum += Vect_Data[m_nCol*i+i];
//	}
//	m_nSel=0;
//	double pro = 0;
//	for(i=0;i<m_nCol;i++)
//	{	
//		if( ( pro < m_fPro && m_nExtractStandard == 0)
//			||( m_nSel < m_nFactorNum && m_nSel < m_nCol  && m_nExtractStandard == 1)			
//			)
//		{	
//			pro += Vect_Data[i*m_nCol + i]/sum;
//			m_nSel++;
//		}
//		//Ϊregression���ӵ÷����ļ��
//		if( Vect_Data[i*m_nCol+i] <= 1e-8 )
//		{
//			m_bEigenFlag = true;
//		}
//	}

	
	//������������
	ProcessEigenVector(Vect_Data);
}
void CFactor::Eigenvalue(CDoubleVector& Vect_Data,CDoubleVector& Vect_EigVector)
{
	int nLen = Vect_Data.vlen();
	assert(nLen>0);
	int nCol = sqrt(nLen);
	if(Vect_EigVector.vlen() != nLen )
		Vect_EigVector.create(nLen); // Ywj  2005-05-09
	cjcbj(Vect_Data,nCol,Vect_EigVector,0.00001);//�����������m_pEigenVector
	SortEigenVect(Vect_Data,Vect_EigVector,nCol,nCol,1);//������������m_pEigenVector
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//	��ʼ�����ڴ��ͷź���
/////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::InitFree()
{
	
	if(m_pDataInterface)
		delete m_pDataInterface;m_pDataInterface = NULL;

	m_bRotateFlag       = false;
	m_bCompenFlag       = false;
	m_bEigenFlag        = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Э������������ĵ������ݲ��裬�������ɷ־���(Eric:�����غɾ���)�͹��Է������
/////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::AdjuestResult()
{
	assert( m_nSel > 0 && m_VectComMatrix.vlen()>0);
	int i,j;
	////TraceVector(m_VectComMatrix);
	////TraceVector(m_VectFirstCovariance);
	for(i=0;i<m_nCol;i++)
	{
		for(j=0;j<m_nSel;j++)
		{
			m_VectComMatrix[i*m_nSel+j] *= 1/sqrt(fabs(m_VectCorrelation[i*m_nCol+i])) ;
		}
	}

	//TraceVector(m_VectComMatrix);
	for(i=0;i<m_nCol;i++)
	{
		m_VectCommunality[i] *= 1/m_VectCorrelation[i*m_nCol+i];
	}
	//TraceVector(m_VectCommunality);
	////TraceVector(m_VectCommunality);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//	���ɷַ����ⲿ���ýӿڣ�						
//		nMethod == 0 Ϊ���ɷַ����ϵ������					
//		nMethod == 1 Ϊ���ɷַ�Э�������	

//		nMethod == 2 �����ӷ����ϵ������				
//		nMethod == 3 �����ӷ�Э�������																									
//////////////////////////////////////////////////////////////////////////////////////////////////
void  CFactor::ComponMethod(int nMethod)
{
	//	int i,j;
	CDoubleVector Vect_Data;
	switch(nMethod)
	{
	case 0://���ɷ�,���ϵ������
		m_bCompenFlag = true;
		m_bCovarian = false;
		Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,0);//�������ϵ������.
		Eigenvalue(Vect_Data);//�����������pData��������ָ��
		break;
	case 1://���ɷ�,Э�������
		m_bCompenFlag = true;
		m_bCovarian = true;
		
		Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,1);//����Э����
		m_VectFirstCovariance.create( m_nCol*m_nCol);
		m_VectFirstCovariance = copy(Vect_Data);
		Eigenvalue(Vect_Data);//�����������pData��������ָ��
		
		//{{{START 2003-2-13 Eric
		if(m_nAnaMethod == 1 && m_bZaisheng)
		{			
			m_UnAdjustComMatrix.create(m_nCol * m_nSel);
			m_UnAdjustComMatrix = copy(m_VectComMatrix);
		}
		//}}}END  2003-2-13 Eric
		
		break;
	case 2://������,���ϵ������
		m_bCovarian = false;
		Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,0);
		//TraceVector(Vect_Data);
		//{{{START 2003-2-8 Eric
		//Eigenvalue(pData);//�����������pData��������ָ��
		CalcFirstCommunalityAndEig(Vect_Data,0);//������ϵ���������������������ϵ������Ĺ��Է��pData��������ָ��
		//}}}END  2003-2-8 Eric
		PrinAxiFactor(0);
		break;
	case 3://������,Э�������
		m_bCovarian = true;
		Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,1);
		
		if(m_VectFirstCovariance.vlen() != 0)
		{
			m_VectFirstCovariance.destruction();
		}

		m_VectFirstCovariance.create( m_nCol*m_nCol);
		m_VectFirstCovariance = copy(Vect_Data);
		CalcFirstCommunalityAndEig(Vect_Data,1);//���Э�������������������Э�������Ĺ��Է��pData����Э��������ָ��
		//}}}END  2003-2-9 Eric
		
		PrinAxiFactor(1);
		
	}
	Vect_Data.Sort();
	CreateChartResult(Vect_Data,m_nCol);
	Vect_Data.destroy();
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//	�����ӷ�
//	  nIndex == 0 : ���ϵ������
//	  nIndex == 1 : Э�������
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::PrinAxiFactor(int nIndex)
{
	assert( m_VectCorrelation.vlen()>0 && m_VectCommunality.vlen()>0 && m_EigenVector.vlen()>0 );
	int i;
	double e = 1.0;
	long   lcount(0);
	CDoubleVector Vect_R (m_nCol*m_nCol);
	m_nIteration = 0;
	double fPreE;
	bool bZoomIn = true;
	
	m_InitCommunality.create(m_nCol);
	m_InitCommunality = copy(m_VectCommunality);
	
	//TraceVector(m_VectCorrelation);
	//TraceVector(m_InitCommunality);
	while(e>0.0001 && bZoomIn)
	{
		if(m_nIteration > m_nMaxIterCount) //������������
			break;
		m_nIteration++;
		//���Լ���ϵ������
        Vect_R = copy(m_VectCorrelation);
		//TraceVector(Vect_R);
		for( i=0;i<m_nCol;i++)
		{
			Vect_R[i*m_nCol+i] = m_VectCommunality[i];
		}
		//TraceVector(Vect_R);
		//���Լ���ϵ�����������ֵ����������
		cjcbj(Vect_R,m_nCol,m_EigenVector,0.00001);
		
		//������Ӹ��ɾ���,���㹲�Ծ���
		ProcessEigenVector(Vect_R,1);
		//TraceVector(Vect_R);
		if( nIndex == 1 )
		{//Э�������
			AdjuestResult();
			m_bCovarian = true;
		}
		else
		{
			m_bCovarian = false;
		}
		
		//����e
		assert(m_VectPreCommunality.vlen()>0);
		double max = -1e10;
		for(i=0;i<m_nCol;i++)
		{
			m_VectPreCommunality[i] -=	  m_VectCommunality[i];
			m_VectPreCommunality[i] =	  fabs(m_VectPreCommunality[i]);
//			m_InitCommunality[i]	=	  m_VectPreCommunality[i];
		}
		for(i=0;i<m_nCol;i++)
		{
			if(m_VectPreCommunality[i] > max)
			{
				max = m_VectPreCommunality[i];
			}
		}
		e = max;
		
		if(m_nIteration == 1)
		{//��һ��ѭ��
			fPreE = e;
		}
		else if(e-fPreE/*e��ǰһ��e*/ > 0)
		{//��������
			bZoomIn = false;
		}
	}
	//TraceVector(m_InitCommunality);
	//TraceVector(m_VectCommunality);
	//TraceVector(m_VectSelEigenValue);
	//TraceVector(m_VectEigenValue);
	//���ѭ������
	//��������غɾ���
	//delete [] pR;
	Vect_R.destruction();
	//{{{START 2003-2-11 Eric
	
	if(!bZoomIn)
	{//��������.
		m_nIteration = -1;
	}
	//}}}END  2003-2-11 Eric
	//pR = NULL;
	CreateEigenValueTable();	
}


//////////////////////////////////////////////////////////////////////////////////////////
//	����������������
//		pData: ָ������ֵ��������
//		(Eric:�ƺ�û�õ�)nIndex == 0 �����Ҫ�����������
//		(Eric:�ƺ�û�õ�)nIndex == 1 ����Ҫ�ж�����ֵ�Ƿ�����б������ѭ��ʱ��
//////////////////////////////////////////////////////////////////////////////////////////
void CFactor::ProcessEigenVector(CDoubleVector Vect_Data,int nIndex)
{
	CIntVector Vect_Index(m_nCol);
	CDoubleVector Vect_pE(m_nCol);

	//��ȡ����ֵ
	int i=0;
	for(i=0;i<m_nCol;i++)
	{
		Vect_pE[i] = Vect_Data[i*m_nCol+i];
	}
//	//TraceVector(Vect_pE);

	//������ֵ�Ӵ�С����,��������m_pEigenValue��;��¼�Ӵ�С���кŷ���pIndex��
	int j=0;
	while(j<m_nCol)
	{
		//{{{START 2003-2-9 Eric
		double max = -1e100;
		//}}}END  2003-2-9 Eric
		int nIndex = 0;
		for(i=0;i<m_nCol;i++)
		{
			if( Vect_pE[i] > max )
			{
				max = Vect_pE[i];
				nIndex = i;
			}
		}
		Vect_Index[j] = nIndex;
		j++;
		Vect_pE[nIndex] = -1;
	}
	//TraceVector(Vect_Index);

	m_VectSelEigenValue.create(m_nCol);
	for(i=0;i<m_nCol;i++)
	{
		m_VectSelEigenValue[i] = Vect_Data[Vect_Index[i]*m_nCol+Vect_Index[i]];//�洢����Ҫ�������ֵ
	}
	//TraceVector(m_VectSelEigenValue);
	//�Է���Ҫ�������ֵ���м���
	//��ȡ��������

	m_VectComMatrix.create( m_nCol * m_nSel);
	int k = 0;
	//TraceVector(m_EigenVector);/////////////////////ggggggggggggggg
	for(i=0;i<m_nSel;i++)
	{
		//�������е�����ֵ���ж��Ƿ����Ҫ��
		int ntemp = Vect_Index[i];
		//		if( ( pData[ntemp*m_nCol+ntemp] >= m_dExtact && nIndex==0)||nIndex==1 )
		{
			//�������Ҫ��ͽ���һ�����ݸ�ֵ��m_pComMatrix�Ķ�Ӧ��
			for(int j=0;j<m_nCol;j++)
			{
				assert( j*m_nSel+k < m_nCol * m_nSel && j*m_nCol+ntemp < m_nCol*m_nCol);
				//m_VectComMatrix[j*m_nSel+k] = m_EigenVector[j*m_nCol+ntemp];
				m_VectComMatrix[j+k*m_nCol] = m_EigenVector[j*m_nCol+ntemp];
				//TRACE("%.4f " , m_EigenVector[j*m_nCol+ntemp]);
			}
			k++;
		}
	}
	//TraceVector(m_VectComMatrix);

 	//{{{START 2003-1-28 Eric
	//������ѡ�е�����������ɵľ������ɷַ�����Ҫ�ã�
	m_SelEigenVector.create(m_nCol * m_nSel);
	m_SelEigenVector = copy(m_VectComMatrix);
	//}}}END  2003-1-28 Eric

	//������ɷ־���: //
	for(i=0;i<m_nSel;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			m_VectComMatrix[i*m_nCol+j] *= sqrt(fabs(m_VectSelEigenValue[i])); //pData[pIndex[j]*m_nCol+pIndex[j]]);
		}
	}

	m_VectUnAdjustCom .create(m_nCol * m_nSel);
	m_VectUnAdjustCom = copy(m_VectComMatrix);


	//��⹲�Է������
	//TraceVector(m_VectCommunality);
	if(m_VectCommunality.vlen())
	{

		m_VectPreCommunality.create(m_nCol);
		m_VectPreCommunality = copy(m_VectCommunality);
		
	}	
	m_VectCommunality .create(m_nCol);
	double sum(0);
	//TraceVector(m_VectComMatrix);
	for(i=0;i<m_nCol;i++)
	{
		sum = 0 ;
		for(int j=0;j<m_nSel;j++)
		{
			sum += m_VectComMatrix[i+j*m_nCol]*m_VectComMatrix[i+j*m_nCol];
		}
		m_VectCommunality[i] = sum;
	}
	m_VectEigenValue = GetOratedExplains(VectorToMatrix(m_VectComMatrix,m_nCol,m_nSel));
}






////////////////////////////////////////////////////////////////////////////////////////////////
//
//	��ƽ��ֵ
//		nIndex ���� 0 ������ƽ��ֵ
//		nIndex ���� 1 ������ƽ��ֵ
//
////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Bartlett�����������ӵ÷ַ�,�㷨���ĵ���Bartlett�����������ӵ÷ַ���
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CFactor::ScoreBartlett()
{
	return Score(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//	������ˣ�c = a ���� b; 
//		a : m*n
//		b : n*k
//		c : m*k
////////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::MatrixMultiply(CDoubleVector a, CDoubleVector b, int m, int n, int k, CDoubleVector& c)
{
	//assert(a&&b&&c);
//	int i,j,l,u;
//    for (i=0; i<=m-1; i++)
//	{
//		for (j=0; j<=k-1; j++)
//		{ 
//			u=i*k+j; c[u]=0.0;
//			for (l=0; l<=n-1; l++)
//			{
//				c[u]=c[u]+a[i*n+l]*b[l*k+j];
//			}
//		}
//	}
//    return;

	int i,j,l,u;
    for (i=0; i<=k-1; i++)
	{
		for (j=0; j<=m-1; j++)
		{ 
			u=i*m+j; c[u]=0.0;
			for (l=0; l<=n-1; l++)
			{
				c[u]=c[u]+a[j+l*m]*b[l+i*n];
			}
		}
	}
    return;
}


////////////////////////////////////////////////////////////////////////////////////
//	Thompsom���ӵ÷ַ����㷨���ĵ�   <<Thompsom����ɭ���ӵ÷ַ�>>
////////////////////////////////////////////////////////////////////////////////////
bool CFactor::ScoreThompsom()
{
	return Score(1);
}

////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
//
//	���ӵ÷ַ�(�㷨���ĵ�) 
//	��nIndex == 0 :Bartlett�����������ӵ÷ַ�
//	��nIndex == 1 :Thompsom���ӵ÷ַ�								
//
////////////////////////////////////////////////////////////////////////////////////
bool CFactor::Score(int nIndex)
{
	int i,j;
	CDoubleVector Vect_Matrix = NULL;
	//1.���������غɾ���
	
	if(m_VectComMatrix.vlen() && m_VectCorrelation.vlen())
	{
		Vect_Matrix= copy(m_VectComMatrix);
	}
	else 
		return false;
	//WriteData(Vect_Matrix,m_nCol,m_nSel,"��תǰ�������غɾ���.");
	////TraceVector(m_VectComMatrix);
	////TraceVector(m_VectCorrelation);
	//���������ת�Ļ�,������ת��������غɾ���
	if( m_nRotateMethod != 0 && m_VectRotateCom.vlen() )
	{
		Vect_Matrix= copy(m_VectRotateCom);
		//{{{START 2003-2-17 Eric
		//WriteData(Vect_Matrix,m_nCol,m_nSel,"��ת��������غɾ���.");
		
		for(i=0;i<m_nCol;i++)
		{
			for(j=0;j<m_nSel;j++)
			{
				Vect_Matrix[i*m_nSel+j] *= 1/sqrt(fabs(m_VectCorrelation[i*m_nCol+i])) ;
			}
		}
		//}}}END  2003-2-17 Eric
	}
	
	//double * pRAdjuest = NULL;//����W��һ����
	CDoubleVector Vect_RAdjuest(m_nCol*m_nCol);
	double	 sum(0);
	if( !Vect_RAdjuest.vlen() )
	{
		return false;
	}
	for(i=0;i<m_nCol;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			sum = 0;
			for(int k=0;k<m_nSel;k++)
			{
				sum += Vect_Matrix[i+k*m_nCol] * Vect_Matrix[j+k*m_nCol];
			}
			Vect_RAdjuest[i*m_nCol+j] = sum;			
		}
	}

	if(m_bCovarian)
	{
		CDoubleVector Vect_OriDataCorr =
			RelationMatrixEx(m_VectInputData, m_nRow,m_nCol,m_weight,0);
		
		for(i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nCol;j++)
			{
				Vect_RAdjuest[i*m_nCol+j] = Vect_OriDataCorr[i*m_nCol+j] - Vect_RAdjuest[i*m_nCol+j];
			}
		}
	}
	else
	{
		for(i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nCol;j++)
			{
				Vect_RAdjuest[i*m_nCol+j] = m_VectCorrelation[i*m_nCol+j] - Vect_RAdjuest[i*m_nCol+j];
			}
		}
	}
	//3.����W
	CDoubleMatrix AD = VectorToMatrix(Vect_RAdjuest,m_nCol,m_nCol);
	double det;
	Matinv1(AD,m_nCol,det);
	Vect_RAdjuest=MatrixToVector(AD);
	CDoubleVector Vect_ATran = MatrixTran(Vect_Matrix,m_nCol,m_nSel);         //�����Ծ����ת�þ���
	CDoubleVector Vect_Temp(m_nSel * m_nCol);
	MatrixMultiply(Vect_ATran,Vect_RAdjuest,m_nSel,m_nCol,m_nCol,Vect_Temp);
	CDoubleVector Vect_Temp2(m_nSel * m_nSel);
	MatrixMultiply(Vect_Temp,Vect_Matrix,m_nSel,m_nCol,m_nSel,Vect_Temp2);
	switch(nIndex)
	{  	
	case 0://Bartlett�����������ӵ÷ַ� 
		break;
	case 1://Thompsom���ӵ÷ַ�			
		for(i=0;i<m_nSel;i++)
		{
			for(int j=0;j<m_nSel;j++)
			{
				Vect_Temp2[i*m_nSel+j] += 1;
			}
		}
		break;
	case 2:
		break;
	}
	
	if( !brinv(Vect_Temp2,m_nSel) )
	{
		return false;
	}
	
	Vect_Temp.create(m_nSel*m_nCol);
	MatrixMultiply(Vect_Temp2,Vect_ATran,m_nSel,m_nSel,m_nCol,Vect_Temp);
	Vect_Temp2.create(m_nSel*m_nCol);
	MatrixMultiply(Vect_Temp,Vect_RAdjuest,m_nSel,m_nCol,m_nCol,Vect_Temp2);
	//4.����÷�
	if( m_nSel != 0 )
	{
		Vect_Temp.create(m_nRow * m_nSel);
		Vect_ATran = MatrixTran(Vect_Temp2,m_nSel,m_nCol);
		MatrixMultiply(m_VectDataFormlized,Vect_ATran,m_nRow,m_nCol,m_nSel,Vect_Temp);
		m_VectScore = copy(Vect_Temp);
	}
	

	//���������ת�Ļ�,������ת��������غɾ���
	if( m_nRotateMethod != 0 && m_VectRotateCom.vlen() )
	{
		Vect_Matrix= copy(m_VectRotateCom);
		for(i=0;i<m_nCol;i++)
		{
			for(j=0;j<m_nSel;j++)
			{
				Vect_Matrix[i*m_nSel+j] *= 1/sqrt(fabs(m_VectCorrelation[i*m_nCol+i])) ;
			}
		}
	
		CDoubleVector Vect_RAdjuest(m_nCol*m_nCol);
		double	 sum(0);
		if( !Vect_RAdjuest.vlen() )
		{
			return false;
		}
		for(i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nCol;j++)
			{
				sum = 0;
				for(int k=0;k<m_nSel;k++)
				{
					sum += Vect_Matrix[i+k*m_nCol] * Vect_Matrix[j+k*m_nCol];
				}
				Vect_RAdjuest[i*m_nCol+j] = sum;			
			}
		}
		//2.
		
		if(m_bCovarian)
		{
			CDoubleVector Vect_OriDataCorr =
				RelationMatrixEx(m_VectInputData, m_nRow,m_nCol,m_weight,0);
			
			for(i=0;i<m_nCol;i++)
			{
				for(int j=0;j<m_nCol;j++)
				{
					Vect_RAdjuest[i*m_nCol+j] = Vect_OriDataCorr[i*m_nCol+j] - Vect_RAdjuest[i*m_nCol+j];
				}
			}
		}
		else
		{
			for(i=0;i<m_nCol;i++)
			{
				for(int j=0;j<m_nCol;j++)
				{
					Vect_RAdjuest[i*m_nCol+j] = m_VectCorrelation[i*m_nCol+j] - Vect_RAdjuest[i*m_nCol+j];
				}
			}
		}
		//3.����W
		CDoubleMatrix AD = VectorToMatrix(Vect_RAdjuest,m_nCol,m_nCol);
		double det;
		Matinv1(AD,m_nCol,det);
		Vect_RAdjuest=MatrixToVector(AD);
		CDoubleVector Vect_ATran = MatrixTran(Vect_Matrix,m_nCol,m_nSel);         //�����Ծ����ת�þ���
		CDoubleVector Vect_Temp(m_nSel * m_nCol);
		MatrixMultiply(Vect_ATran,Vect_RAdjuest,m_nSel,m_nCol,m_nCol,Vect_Temp);
		CDoubleVector Vect_Temp2(m_nSel * m_nSel);
		MatrixMultiply(Vect_Temp,Vect_Matrix,m_nSel,m_nCol,m_nSel,Vect_Temp2);
		switch(nIndex)
		{  	
		case 0://Bartlett�����������ӵ÷ַ� 
			break;
		case 1://Thompsom���ӵ÷ַ�			
			for(i=0;i<m_nSel;i++)
			{
				for(int j=0;j<m_nSel;j++)
				{
					Vect_Temp2[i*m_nSel+j] += 1;
				}
			}
			break;
		case 2:
			break;
		}
		
		if( !brinv(Vect_Temp2,m_nSel) )
		{
			return false;
		}
		
		Vect_Temp.create(m_nSel*m_nCol);
		MatrixMultiply(Vect_Temp2,Vect_ATran,m_nSel,m_nSel,m_nCol,Vect_Temp);
		Vect_Temp2.create(m_nSel*m_nCol);
		MatrixMultiply(Vect_Temp,Vect_RAdjuest,m_nSel,m_nCol,m_nCol,Vect_Temp2);
		//4.����÷�
		if( m_nSel != 0 )
		{
			Vect_Temp.create(m_nRow * m_nSel);
			Vect_ATran = MatrixTran(Vect_Temp2,m_nSel,m_nCol);
			MatrixMultiply(m_VectDataFormlized,Vect_ATran,m_nRow,m_nCol,m_nSel,Vect_Temp);
			m_VectRotateScore = copy(Vect_Temp);
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////
//	���ӵ÷�֮Regression����
///////////////////////////////////////////////////////////////////////////////////
bool CFactor::ScoreRegression()
{
	//�ж��Ƿ���Ҫ�������ӵ÷�
	if( !NeedScoreRegression() )
		return false;
	CDoubleVector Vect_W(m_nCol*m_nSel);
	//pW = new double [m_nCol*m_nSel];
	//���ɷַ�������ת
	if( m_bCompenFlag && !m_bRotateFlag )
	{
		for(int i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nSel;j++)
			{
				Vect_W[i*m_nSel+j] = m_VectComMatrix[i*m_nSel] * (1 / m_VectSelEigenValue[j] ) ;
			}
		}
	}
	//���ɷ֣���ת
	else if( m_bCompenFlag && m_bRotateFlag )
	{
		CDoubleVector Vect_ATran = MatrixTran(m_VectComMatrix,m_nCol,m_nSel);
		CDoubleVector Vect_Temp(m_nSel*m_nSel);
		MatrixMultiply(Vect_ATran,m_VectComMatrix,m_nSel,m_nCol,m_nSel,Vect_Temp);
		if( !brinv( Vect_Temp,m_nSel ) )
		{
			return false;

		}
		MatrixMultiply(m_VectComMatrix,Vect_Temp,m_nCol,m_nSel,m_nSel,Vect_W);
	}
	else//��������
	{
		if( !brinv(m_VectCorrelation,m_nCol))
		{
			//Vect_W.destruction();
			return false;
		}
		if( m_bZhengjiao ) //��������
		{
			MatrixMultiply(m_VectCorrelation,m_VectComMatrix,m_nCol,m_nCol,m_nSel,Vect_W);
		}
		else
		{
			//����������غɾ�������ϵ������
			Formalize(m_VectComMatrix,m_nCol,m_nSel);
			CDoubleVector RelationMatrix = RelationMatrixEx(m_VectComMatrix,m_nCol,m_nSel,m_weight,0);
			
			CDoubleVector Vect_S (m_nCol*m_nSel) ; 
			MatrixMultiply(m_VectComMatrix,RelationMatrix,m_nCol,m_nSel,m_nSel,Vect_S);
			MatrixMultiply(m_VectCorrelation,Vect_S,m_nCol,m_nCol,m_nSel,Vect_W);
		}
	}
	CDoubleVector Vect_F(m_nRow*m_nSel);
	MatrixMultiply(m_VectData,Vect_W,m_nRow,m_nCol,m_nSel,Vect_F);
	m_VectRotateScore = copy(Vect_F);
	////TraceVector(m_VectScore);
	return true;
}



bool CFactor::NeedScoreRegression()
{
	bool bFlag = false;
	//�ڲ���ת���ӵ����ɷַ�������PC���У����κ�һ������ֵ������ < 1e-8
	if( m_bCompenFlag && !m_bRotateFlag && m_bEigenFlag )
	{
		return false;		
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//	����������ת���㷨�ɼ��ĵ�����������ת.doc��						//
//		matlab Դ����ɼ���orthorth2.m									//
//      ���������������غɾ��������ת��������غɾ���				//
//////////////////////////////////////////////////////////////////////////
bool CFactor::Orth(int nMethod)
{	
	assert( m_VectComMatrix.vlen()>0 );
	//--------------2������ֵ----------------------------//
	int s = 1;
	CDoubleMatrix matrix_AA = VectorToMatrix(m_VectComMatrix,m_nCol,m_nSel);
	CDoubleMatrix matrix_A = matrix_AA;
	CDoubleMatrix matrix_A0;
	matrix_A0 = diag(1.0/sqrt(sum(prod(matrix_AA,matrix_AA))))*matrix_AA;
	double delta(1);
	
	//--------------3��������ת--------------------------//
	int m = m_nSel;
	double SV_Pre(0),SV_Cur(0);
	int MAXITERATION=300;
	int iteration=0;
	while( delta > 1e-5 &&iteration<MAXITERATION)
	{
		iteration++;
		int l=0;
		for(l=0;l<m-1;l++)
		{
			for(int k=l+1;k<m;k++)
			{
				//������ת�Ƕ�
				double p = Angle(matrix_A0,l,k,nMethod);
				//��ת
				if( fabs(sin(p)) > 1e-8 )
				{
					CDoubleMatrix matrix_temp1(m_nCol,2);
					matrix_temp1 = matrix_A.col(l)|matrix_A.col(k);				
					
					CDoubleMatrix matrix_temp2 = GetRotationMatrix(p);		
					CDoubleMatrix matrix_temp3 = matrix_temp1 * matrix_temp2;
					//TraceMatrix(matrix_temp3);
					for( int i=0;i<m_nCol;i++)
					{
						matrix_A(i,l) = matrix_temp3(i,0);
						matrix_A(i,k) = matrix_temp3(i,1);
					}				
				}
			}
		}
		TraceMatrix(matrix_A);
		matrix_A0 = diag(1.0/sqrt(sum(prod(matrix_A,matrix_A)))) * matrix_A;
		SV_Cur = sv2(matrix_A0);
		if( s > 1 )
		{
			delta = fabs(SV_Cur - SV_Pre);
		}
		SV_Pre = SV_Cur;
		s++;		
	}	
	m_resortMatrix = matrix_A;
	m_VectRotEigenValue = GetOratedExplains(matrix_A);
	return true;
}
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
//  ���������غɾ���ĸ��з����		
///////////////////////////////////////////////////////////////////////////////////////
double CFactor::sv2(CDoubleMatrix matrix_A)
{
	int p = matrix_A.mrow();
	int m = matrix_A.mcol();
	double sv(0);
	for(int j=0;j<m;j++)
	{
		double temp1 = 0;
		double temp2 = 0;
		for(int k=0;k<p;k++)
		{
			temp1 += pow(matrix_A(k,j),4);
			temp2 += pow(matrix_A(k,j),2);
		}
		temp1 *= p;
		temp2 = temp2*temp2;
		sv += temp1 - temp2;
	}
	return sv/(p*p);
}


//////////////////////////////////////////////////////////////////////////////////////
//
//	������ת�Ƕ�
//  ��matrix_f�����غɾ���,l,m: ��Ҫ��ת���кţ�
//	��nMethod :����  1:varimax,2:Equimax,3:Quartimax
//
//////////////////////////////////////////////////////////////////////////////////////
double CFactor::Angle(CDoubleMatrix matrix_f, int l, int k, int nMethod)
{
	int p = matrix_f.mrow();
	int m = matrix_f.mcol();
	
	CDoubleVector vector_u;
	CDoubleVector vector_v;
	
	vector_u = col(matrix_f,l)*col(matrix_f,l) - col(matrix_f,k)*col(matrix_f,k);
	vector_v = 2.0*col(matrix_f,l)*col(matrix_f,k);
	
	double  d_A = sum(vector_u);
	double  d_B = sum(vector_v);
	double  d_C = sum(vector_u*vector_u-vector_v*vector_v);
	double  d_D = sum(2.0*vector_u*vector_v);
	
	double d_X(0);
	double d_Y(0);
	
	switch( nMethod )
	{
	case 1:
		d_X = d_D - 2*d_A*d_B/p;
		d_Y = d_C - (d_A*d_A - d_B*d_B)/p;
		break;
	case 2:
		//{{{START 2003-2-8 Eric
		//d_X = d_D - 2*d_A*d_B/p;
		//d_Y = d_C - (d_A*d_A - d_B*d_B)/(2*p);
		d_X = d_D - m*d_A*d_B/p;
		d_Y = d_C - m*(d_A*d_A - d_B*d_B)/(2*p);
		//}}}END  2003-2-8 Eric
		break;
	case 3:
		d_X = d_D;
		d_Y = d_C;
		break;
	}
	if( d_X >=0  && d_Y >=0 )
	{
		return atan(d_X/d_Y)/4.0;
	}
	if( d_X >= 0 && d_Y < 0 )
	{
		return (atan(d_X/d_Y) + pi)/4.0;
	}
	if( d_X < 0 && d_Y <= 0 )
	{
		return (atan(d_X/d_Y) - pi)/4.0;
	}
	if( d_X < 0 && d_Y > 0 )
	{
		return 	(atan(d_X/d_Y))/4.0;
	}
	return 0;
}



CDoubleMatrix CFactor::GetRotationMatrix(double angle)
{
	CDoubleMatrix matrix(2,2);
	matrix(0,0) =  cos(angle);
	matrix(0,1) = -sin(angle);
	matrix(1,0) =  sin(angle);
	matrix(1,1) =  cos(angle);
	return matrix;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	proman������б����ת��
//	������Ϊ�Ѿ��������������ת��varimax���󣬵õ������������غɾ���matrix_A��Ȼ��
//	���������·������м������õ�б�����������L��б�����ӽṹ��S��б�������غ���Af
//	���������������varimax��ת�õ��������غɾ����û�����Ĳ���kappaֵ��kappa>=2��
//	��ǰ�������kappa��Χ		
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFactor::Proman(CDoubleMatrix matrix_A, double dbKappa)
{
	assert( dbKappa > 1);
	int p = matrix_A.mrow();
	int m = matrix_A.mcol();
	if( p>0 && m>0 )
	{
	}
	else
		return false;
	
	//----------��A���й��------------
	CDoubleMatrix matrix_A1 = FormalizeWithRow(matrix_A);
	
	//-----------����h(i,j)--------------
	CDoubleMatrix matrix_H(p,m);
	int i=0;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<m;j++)
		{
			if( matrix_A1(i,j) >= 0 )
			{
				matrix_H(i,j) =  pow( fabs( matrix_A1(i,j) ),dbKappa);
			}
			else
			{
				matrix_H(i,j) = -pow( fabs( matrix_A1(i,j) ),dbKappa );
			}
		}
	}
	
	//--------------����G----------------
	CDoubleMatrix matrix_G;
	matrix_G = pinv( (CDoubleMatrix)( trans(matrix_A1) * matrix_A1)  ) * trans(matrix_A1) * matrix_H;
	
	//��G���й�񻯣��õ�б���ο���lamta
	CDoubleMatrix matrix_lamta = FormalizeWithCol(matrix_G);
	
	//��lamta���й��
	CDoubleMatrix matrix_ilamta = pinv( matrix_lamta );
	CDoubleMatrix matrix_Tp = FormalizeWithRow(matrix_ilamta);
	
	//����б���任��T
	CDoubleMatrix matrix_T = trans(matrix_Tp);
	
	//����б�����������L��б�����ӽṹ��S��б�������غ���Af
	CDoubleMatrix matrix_L  = matrix_Tp * matrix_T;
	CDoubleMatrix matrix_Af = matrix_A  * pinv( matrix_Tp );
	CDoubleMatrix matrix_S  = matrix_A  * matrix_T;
	MatrixToPointer(matrix_L,&m_pL);
	MatrixToPointer(matrix_Af,&m_pAf);
	MatrixToPointer(matrix_S,&m_pS);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//	������ľ�����а��еĹ�񻯣����ع�񻯺�ľ���
/////////////////////////////////////////////////////////////////////////////////////////
CDoubleMatrix CFactor::FormalizeWithRow(CDoubleMatrix matrix_A)
{
	int p = matrix_A.mrow();
	int m = matrix_A.mcol();
	CDoubleMatrix matrix_B(p,m);
	CDoubleMatrix matrix_temp = prod(matrix_A,matrix_A);
	CDoubleVector vector_temp = sum( matrix_temp );
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<m;j++)
		{
			matrix_B(i,j) = matrix_A(i,j) / sqrt(fabs(vector_temp[i]));
		}
	}	
	return matrix_B;
}

/////////////////////////////////////////////////////////////////////////////////////////
//	������ľ�����а��еĹ�񻯣����ع�񻯺�ľ���
/////////////////////////////////////////////////////////////////////////////////////////
CDoubleMatrix CFactor::FormalizeWithCol(CDoubleMatrix matrix_A)
{
	int p = matrix_A.mrow();
	int m = matrix_A.mcol();
	CDoubleMatrix matrix_B(p,m);
	CDoubleMatrix matrix_temp = prod(matrix_A,matrix_A);
	CDoubleVector vector_temp = sum( trans(matrix_temp)  );
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<m;j++)
		{
			matrix_B(i,j) = matrix_A(i,j)/sqrt(fabs(vector_temp[j]));
		}
	}
	return matrix_B;
}


//////////////////////////////////////////////////////////////////////////////////////////
//	������ȡ���� alpha2����  m ; ��ȡ�����Ӹ�����nCountMax ��ѭ����������
//////////////////////////////////////////////////////////////////////////////////////////
bool CFactor::FactorAlpha2(CDoubleMatrix matrix_Z,int m, int nCountMax)
{
	bool bFlag(false);
	int n = matrix_Z.mrow();
	int p = matrix_Z.mcol();
	//�ڶ������������ϵ������������������������
	CSymDbMatrix  matrix_R = RelationMatrixEx(matrix_Z);
	CDoubleMatrix matrix_EigVector;
	CSymDbMatrix  matrix_EigValue = eig(matrix_R,matrix_EigVector);
	//ѭ�����
	CDoubleMatrix matrix_r = pinv((CDoubleMatrix)matrix_R);
	int ncount = 1;
	CDoubleMatrix matrix_H0 = diag( diag(1.0 - 1.0/matrix_r) );
	CComplexMatrix matrix_A;
	while( ncount < nCountMax )
	{
		CDoubleMatrix matrix_U2   = eye(p) - matrix_H0;
		CSymDbMatrix  matrix_temp = diag(1.0/sqrt(diag(matrix_H0))) * ( (CDoubleMatrix)matrix_R - matrix_U2) * diag(1.0/sqrt(diag(matrix_H0)));
		CDoubleMatrix matrix_tempEigVector;
		CDoubleMatrix matrix_tempEigValue = eig(matrix_temp,matrix_tempEigVector);
		
		//���������Ӵ�С���У�����Ӧ�ص�����������
		SortEigenVect(matrix_tempEigValue,matrix_tempEigVector,1);
		//�������Ӹ�����ȡ����������������
		CComplexMatrix matrix_lamta3 = eye(m);
		int i=0;
		for(i=0;i<m;i++)
		{
			matrix_lamta3(i,i) = matrix_tempEigValue(i,i);
		}
		CDoubleMatrix matrix_l3(p,m);
		matrix_l3 = slice(matrix_tempEigVector,p,m);
		
		//���������غɾ���A
		matrix_A = ((CComplexMatrix)sqrt(matrix_H0)) * (CComplexMatrix)matrix_l3 * diag(diag(matrix_lamta3));
		
		//���㹲�Է���
		CDoubleMatrix matrix_H1 = real( (CComplexMatrix)diag( diag( matrix_A * trans(matrix_A) )));
		
		//�ж�ѭ������ֹ����
		CDoubleMatrix matrix_abs = matrix_H1 - matrix_H0;
		double max = -1e10;
		for(i=0;i<matrix_abs.mrow();i++)
		{
			for(int j=0;j<matrix_abs.mcol();j++)
			{
				matrix_abs(i,j) = fabs(matrix_abs(i,j));
				if( matrix_abs(i,j) > max )
				{
					max = matrix_abs(i,j);
				}
			}
		}
		if( max < 10e-5 )
		{
			bFlag = true;
			break;
		}
		matrix_H0 = copy(matrix_H1);
		ncount++;
	}
	m_VectComMatrix = MatrixToVector(matrix_A.real());//////////////////////////////////////////////////////////////////////////
	return bFlag;
}


////////////////////////////////////////////////////////////////////////////////////////////
//	���ӷ�����ȡ֮Image������������ȡ�����㷨�ɼ��ĵ���Image������������ȡ��
//		���δ���ֱ�ӷ�����matlabԴ���룬
//		������nIndex == 0Ϊ��ط������
//            nIndex == 1ΪЭϵ������
////////////////////////////////////////////////////////////////////////////////////////////
bool CFactor::FactImage(CDoubleMatrix matrix_z, int nIndex)
{
	int i,j;
	
	if(nIndex == 1)
	{
		m_bCovarian = true;
	}
	else
	{
		m_bCovarian = false;
	}
	
	CreateEig(nIndex);
	int p = matrix_z.mcol();
	int n = matrix_z.mrow();
	//������ϵ������ ��Э�������.
	CDoubleMatrix matrix_R2 = RelationMatrixEx(matrix_z,nIndex);
	TraceMatrix(matrix_R2);
	//���Ǳ��㷨��һ�����Э����.
	if(nIndex == 1)
	{//Э����
		m_VectFirstCovariance.create(m_nCol*m_nCol);
		m_VectFirstCovariance = MatrixToVector( matrix_R2);//////////////////////////////////////////////////////////////////////////
		
	}
	
	
	CDoubleMatrix matrix_eigvector;
	//�������ֵ����������
	CDoubleMatrix matrix_eigvalue ;//= eig((CSymDbMatrix)matrix_R2,matrix_eigvector);
	CDoubleVector Vect_Cov = MatrixToVector(matrix_R2);
	FromCorrelationToCommunality(Vect_Cov);//�����εĹ��Է���
 	CDoubleVector Vect_EigenVector(m_nCol*m_nCol);
	Eigenvalue(Vect_Cov,Vect_EigenVector);//�����������m_pEigenVector

	//��С��������
	matrix_eigvalue = VectorToMatrix(Vect_Cov,m_nCol,m_nCol);
	m_VectSelEigenValue.create(m_nCol);
	for(i=0;i<m_nCol;i++)
		m_VectSelEigenValue[i]=matrix_eigvalue(i,i);
	CDoubleVector vect = m_VectSelEigenValue;
	CreateChartResult(vect.Sort(),m_nCol);

	matrix_eigvector= trans(VectorToMatrix(Vect_EigenVector,m_nCol,m_nCol));
	
	double sum(0);
	CDoubleVector vector_llamtavalue(p);
	for( i=0;i<p;i++)
	{
		vector_llamtavalue(i) = matrix_eigvalue(i,i);
		sum += matrix_eigvalue(i,i);
	}
	double proportion(0);
	//TraceVector(vector_llamtavalue);
	//ѡ������Ӹ���
	//{{{START 2003-2-19 Eric
	//���ڲ�������ϴ�����߼�,�������¼���m_nSel;
	if(m_nExtractStandard == 0)
	{
		proportion = 0;
		i = 0;
		while (proportion < m_fPro)
		{
			proportion += vector_llamtavalue[i] / sum;
			i++;
		}
	}
	
	//}}}END  2003-2-19 Eric
	
	int m = i;
	m_nSel = m;
	CDoubleMatrix matrix_lamtavalue = eye(m,m);
	int k=0;
	for(k=0;k<m;k++)
	{
		matrix_lamtavalue(k,k) = vector_llamtavalue[k];
	}
	TraceMatrix(matrix_lamtavalue);
	TraceMatrix(matrix_R2);
	CDoubleMatrix matrix_r  = pinv((CDoubleMatrix)matrix_R2);
	TraceMatrix(matrix_r);
	CDoubleMatrix matrix_s2 = eye(p,p);
	for(i=0;i<p;i++)
	{
		matrix_s2(i,i) = 1.0/matrix_r(i,i);
	}
	CDoubleMatrix matrix_s    = diag( sqrt( diag( matrix_s2 )));
	TraceMatrix(matrix_s);
	CDoubleMatrix matrix_temp = pinv( matrix_s );
	TraceMatrix(matrix_temp);
	CSymDbMatrix matrix_srs  = matrix_temp * (CDoubleMatrix)matrix_R2 * matrix_temp;
	TraceMatrix((CDoubleMatrix)matrix_srs);
	//���eig
	Vect_Cov = MatrixToVector((CDoubleMatrix)matrix_srs);
 	Eigenvalue(Vect_Cov,Vect_EigenVector);//�����������m_pEigenVector

	//TraceVector(Vect_Cov);
	//TraceVector(Vect_EigenVector);
	TraceMatrix(matrix_eigvalue);
	//��С��������
	matrix_eigvalue = VectorToMatrix(Vect_Cov,p,p);
	matrix_eigvector= trans(VectorToMatrix(Vect_EigenVector,p,p));
	
	//���򣬴Ӵ�С
	//SortEigenVect(matrix_eigvalue,matrix_eigvector,1);
	//TraceMatrix(matrix_eigvalue);
	//TraceMatrix(matrix_eigvector);
	
	
	matrix_lamtavalue = eye(m);
	for(i=0;i<m;i++)
	{
		matrix_lamtavalue(i,i) = matrix_eigvalue(i,i);
	}
	TraceMatrix(matrix_lamtavalue);
	CDoubleMatrix matrix_lvalue(p,m);
	matrix_lvalue = slice(matrix_eigvector,p,m);
	TraceMatrix(matrix_lvalue);
	CDoubleMatrix matrix_f = matrix_s * matrix_lvalue * ( matrix_lamtavalue - eye(m,m) ) * diag(1.0/sqrt( diag (matrix_lamtavalue)));
    TraceMatrix(matrix_f);

	m_VectEigenValue = GetOratedExplains(matrix_f);

	m_VectComMatrix = MatrixToVector(matrix_f);
	ProcessComMatrix(m_VectComMatrix,matrix_f.mrow(),matrix_f.mcol());

	m_InitCommunality.create(m_nCol);
	m_InitCommunality = copy(m_VectCommunality);
	m_VectCommunality.create(p);
	for(i=0;i<p;i++)
	{
		m_VectCommunality[i] = 0;
		for(j=0;j<m;j++)
		{
			m_VectCommunality[i] += ( matrix_lamtavalue(j,j) - 1 )*( matrix_lamtavalue(j,j) - 1 ) * matrix_lvalue(i,j) * matrix_lvalue(i,j) / ( matrix_lamtavalue(j,j) * matrix_r(i,i));
		}
	}
	//TraceVector(m_VectCommunality);
	CDoubleMatrix matrix_icm  = (CDoubleMatrix)matrix_R2 + (matrix_s2 * pinv( (CDoubleMatrix)matrix_R2 ) * matrix_s2) - ( 2.0 * matrix_s2 );
	CDoubleMatrix matrix_aicm = matrix_s2 * pinv( (CDoubleMatrix)matrix_R2 ) * matrix_s2;
	CDoubleMatrix matrix_diagR= eye(p,p);
	for(i=0;i<p;i++)
	{
		matrix_diagR(i,i) = matrix_R2(i,i);
	}
	CDoubleMatrix matrix_fr = matrix_diagR * matrix_f; 
	TraceMatrix(matrix_fr);
	return true;
	
}

bool CFactor::GetValueFromVOFac(CDataInterface *pDataInterface, std::string VO)
{
//	FACTOR
//		/DATA=[FILE]
//		/VARLIST=[varlist]
//		/MISSING=[{NONE**}]
//		/EXTRACTION=[{PCOMP**},{PFACTOR},{IMAGE},{ML},{GLS},{UNLS}]
//		/METHOD=[{CORR**},{COV}]
//		/RULE=[{CUMULANT(0.9**)},{NUMFACTOR(1**)}]
//		/SCALE [{BARTLETT**},{THOMPSON}]
//		/ROTATION [{NONE**},{VARIMAX},{EQUAMAX},{QUARTIMAX}]
//		/CRITERIA [MINEIGEN({1**})] [ITERATE({30**})]
//		/RESULT [CORRRESD] [REPR] [ANTICORR] [ANTICOV] [KMO]
//		/PRINT [FACTORS]

	AnaWord VOstr;
	VOstr.Import(VO);
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	
	m_nMissingMethod = 0;

	if (AnaSyntax::findstr("/EXTRACTION", "PFACTOR",VOstr))
		m_nExtractMethod = 1;
	else if (AnaSyntax::findstr("/EXTRACTION", "IMAGE",VOstr))
		m_nExtractMethod = 2;
	else if (AnaSyntax::findstr("/EXTRACTION", "ML",VOstr))
		m_nExtractMethod = 3;
	else if (AnaSyntax::findstr("/EXTRACTION", "GLS",VOstr))
		m_nExtractMethod = 4;
	else if (AnaSyntax::findstr("/EXTRACTION", "UNLS",VOstr))
		m_nExtractMethod = 5;
	else
		m_nExtractMethod = 0;

	m_nAnaMethod = int(AnaSyntax::findstr("/METHOD", "COV",VOstr));

	m_fPro = 0.9;
	m_nExtractStandard = int(AnaSyntax::findstr("/RULE", "NUMFACTOR",VOstr));
	if (AnaSyntax::findstr("/RULE", "CUMULANT",VOstr))
		m_fPro = AnaSyntax::getvalue("/RULE", "CUMULANT",VOstr);
	m_nFactorNum = 1;
	if (AnaSyntax::findstr("/RULE", "NUMFACTOR",VOstr))
		m_nFactorNum = AnaSyntax::getvalue("/RULE", "NUMFACTOR",VOstr);

	m_bScore = AnaSyntax::exist("/SCALE", VOstr);
	m_nScoreMethod = int(AnaSyntax::findstr("/SCALE", "THOMPSON",VOstr));
	
	if (AnaSyntax::findstr("/ROTATION", "VARIMAX",VOstr))
		m_nRotateMethod = 1;
	else if (AnaSyntax::findstr("/ROTATION", "EQUAMAX",VOstr))
		m_nRotateMethod = 2;
	else if (AnaSyntax::findstr("/ROTATION", "QUARTIMAX",VOstr))
		m_nRotateMethod = 3;
	else
		m_nRotateMethod = 0;
			
	m_nIteration = 0;
	if (AnaSyntax::findstr("/CRITERIA", "MINEIGEN",VOstr))
		m_nIteration = AnaSyntax::getvalue("/CRITERIA", "MINEIGEN",VOstr);
	m_nMaxIterCount = 30;
	if (AnaSyntax::findstr("/CRITERIA", "ITERATE",VOstr))
		m_nMaxIterCount = AnaSyntax::getvalue("/CRITERIA", "ITERATE",VOstr);

	m_bShengyu = AnaSyntax::findstr("/RESULT", "CORRRESD",VOstr);
	m_bZaisheng = AnaSyntax::findstr("/RESULT", "REPR",VOstr);
	m_bAntiImage = AnaSyntax::findstr("/RESULT", "ANTICORR",VOstr);
	m_bAntiImage2 = AnaSyntax::findstr("/RESULT", "ANTICOV",VOstr);
	m_bKMO = AnaSyntax::findstr("/RESULT", "KMO",VOstr);
	
	m_bOutput = AnaSyntax::findstr("/PRINT", "FACTORS",VOstr);

	return TRUE;
}

CResult * CFactor::OnFactor(CDataInterface *pDataInterface, std::string VO)
{
	

	m_bPrincipal=false;//zkc 2003.8.20
	m_strResultExplain.RemoveAll();
	m_pOriDataInterface = pDataInterface;
	m_bIsMValued=false;
	m_nProcType = 1;
	m_bComBigOne = false;
	m_pResult = NULL;
	m_pResult = new CResult("���ӷ���");
	if (!(GetValueFromVOFac(pDataInterface, VO)))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	//ȡ����
	m_VectInputData = GetData(pDataInterface,m_tVarInfo);//ͬʱ��m_pDataInterface��ֵ//ȡ��Ȩ���У��������Ȩ�أ���Ȩ��ȫΪ��
	if(!m_VectInputData.vlen())
	{
		delete m_pResult;
		m_pResult = NULL;
		OnFactorExit();
		return NULL;
	}
	//����Ȩ��.
	int i = 0;
	for(i = 0;i<m_weight.vlen();i++)
	{
		if(m_weight(i)<0)
			m_weight[i] = 0;
	}
	
	int m          = m_tVarInfo.iCount;
	CDoubleMatrix matrix=VectorToMatrix(m_VectInputData,m_nRow,m);
	this->SetRowsCols(m_nRow,m);
	m_VectData = copy(m_VectInputData);
	//ȡ����
	m_VectDataFormlized = FormlizeEx(m_VectInputData,m_weight);

	double dbKappa          = 4;

	if(m_fPro > 1)
	{
		m_fPro = 1;
	}
	if(m_fPro<0)
	{
		m_fPro = 0;
	}


	//������Ӧ�ļ���
	//1��������ȡ��		
	switch( m_nExtractMethod)
	{
	case 0:
		m_strExtractMethod = "��ȡ���������ɷַ�";
		ComponMethod(m_nAnaMethod);//���ɷַ�
		break;		
	case 1:
		m_strExtractMethod = "��ȡ�����������ӷ�";
		ComponMethod(m_nAnaMethod+2);//�����ӷ�
		break;			
	case 2:
		if(m_nRow < 4)
		{
// 			AfxMessageBox("������̫��,�޷����з���!");
			return NULL;
		}
		m_strExtractMethod = "��ȡ�������������ӷ�";
		FactImage(matrix,m_nAnaMethod);
		break;
	case 3:
		m_strExtractMethod = "��ȡ������������Ȼ��";
		m_bCovarian = false;
		ml_fact(matrix,0,m_nFactorNum,m_nMaxIterCount,7);
		break;
	case 4:
		m_strExtractMethod = "��ȡ��������ͨ��С���˷�";
		m_bCovarian = false;
		ml_fact(matrix,1,m_nFactorNum,m_nMaxIterCount,7);
		break;
	case 5:
		m_strExtractMethod = "��ȡ��������Ȩ����С���˷�";
		m_bCovarian = false;
		ml_fact(matrix,2,m_nFactorNum,m_nMaxIterCount,7);
		break;
	default:
		break;

	}
	//2��������ת

	switch(m_nRotateMethod)
	{
	case 1:
		m_strRotateMethod = "��ת���������������ת";
		Orth(1);//���������ת
		break;
	case 2:
		m_strRotateMethod = "��ת������ƽ��������ת";
		Orth(2);//ƽ��������ת
		break;
	case 3:
		//WriteData(m_pComMatrix,m_nCol,m_nSel,"�Ĵη�������ת:��תǰ�������غɾ���");
		m_strRotateMethod = "��ת�������Ĵη�������ת";
		Orth(3);//�Ĵη�������ת
		break;
	case 0://����ת
	default:
		m_resortMatrix = VectorToMatrix(m_VectComMatrix,m_nCol,m_nSel);
		//TraceVector(m_VectComMatrix);
		break;
	}


//{{{START 2003-1-26 Eric
	//3��������	
	m_VectRotateCom .create( m_resortMatrix.mrow()*m_resortMatrix.mcol());
	TraceMatrix(m_resortMatrix);
	//TraceVector(m_VectComMatrix);
	ProcessRotateMatrix(m_resortMatrix);
	m_VectRotateCom = MatrixToVector(m_resortMatrix);
//}}}END  2003-1-26 Eric
//{{{START 2003-2-13 Eric
	//����Э����,Ҫ���е���.
	if(m_bCovarian)
	{
		m_UnAdjustComMatrix.create( m_nCol * m_nSel);
		m_UnAdjustComMatrix = copy(m_VectComMatrix);
		m_VectComMatrix.create(m_nCol * m_nSel);
		switch( m_nExtractMethod)
		{
		case 0:	//���ɷ�
			
			AdjuestResult();

			break;
		case 1:	//�����ӷ�
			break;
		case 2://�������ӷ�

			AdjuestResult(); //�÷�֮ǰҪ����.
			break;
		case 3://������Ȼ��

			break;
		}

	}//}}}END  2003-2-13 Eric

	//4�����ӵ÷�
	if( m_bScore )
	{
	switch( m_nScoreMethod )
	{
		case 0:
			m_strScoreMethod = "�����������ӵ÷ַ�";
			Score(0);//barblate			
			break;
		case 1:
			m_strScoreMethod = "��ķɭ���ӵ÷ַ�";
			Score(1);//tomash			
			break;
		case 2:		
			m_strScoreMethod = "�ع����ӵ÷ַ�";
			ScoreRegression();//regression
			break;
		default:
			break;
		}
	}
	CreateResult(m_pResult);

//{{{START 2003-2-11 Eric
	if(m_nIteration != -1)
//}}}END  2003-2-11 Eric
	{
		KMO();
		Sphericity(matrix);		
	}
	
	OnFactorExit();

	int nOutNote = 0;//�Ƿ���ʾע��,0 ����ʾ,������ʾ #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
	if (nOutNote)
	{			
		CTLTable* pTable = new CTLTable;
		pTable->InsertColumn(0,"���˵��");
		for(i=0;i<m_strResultExplain.GetSize();i++)
		{
			pTable->InsertItem(i,m_strResultExplain.GetAt(i));
		}
		m_pResult->InsetAt(0,new CRsltElementTable("���˵��",pTable));
	}
	m_bComBigOne = false;
	m_bCovarian = false;
	m_bScore = false;
	m_nRotateMethod = 0;
	
	return m_pResult;
}

//�������������� ���Է������	 ���������غɾ���  ������ת��������غɾ���	 ����б�����  �������ӵ÷ֽ��
void CFactor::CreateResult(CResult * pResult)
{
	if( m_nSel == 0 )    //û�����ӱ�ѡ��
		return;
	//����������ȡ�����
	CTLTable * pTable = NULL;
	CRsltElementTable * pETable = NULL;
	int i=0;

	double min=0;
	double max=0;
	CTString minFieldName;
	CTString maxFieldName;
	CTString strExplain;
	if(m_VectCommunality.vlen()>0)
	{
//���Է������
		if(m_nProcType == 1)
		{
			CreateCommulityTable();
		}			

		if(!m_InitCommunality.vlen())
		{
			m_InitCommunality.create( m_nCol);
			m_InitCommunality = copy(m_VectCommunality);
		}

		if(m_nProcType == 1)
		{
			
			int i=0;
			for(i=0;i<m_tVarInfo.iCount;i++)
			{
				if(i==0)
				{
					min=m_InitCommunality[i];
					max=m_InitCommunality[i];
					maxFieldName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
					minFieldName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
				}
				else//ufoufo
				{
					if(min>m_InitCommunality[i])
					{
						min=m_InitCommunality[i];
						minFieldName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
						
					}
					if(max<m_InitCommunality[i])
					{
						max=m_InitCommunality[i];
						maxFieldName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);						
					}					
				}

			}
			strExplain.Format("%s��ȡ����Ϣ���֣�Ϊ%.4f��%s��ȡ����Ϣ���֣�Ϊ%.4f��",
				maxFieldName.GetData(),max,minFieldName.GetData(),min);	
		//	m_strResultExplain.Add(strExplain);
			
		}
		else
		{
			pTable = new CTLTable;
			pTable->SetTitle("���Է������");
//			pTable->SetCols(2);
//			
//			pTable->SetRows(m_tVarInfo2.iCount+1);

			pTable->InsertColumn(0,"");
			pTable->InsertColumn(0,"��ȡ���");
			
			int i=0;
			for(i=0;i<m_tVarInfo2.iCount;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
				pTable->SetItemText(i,1,m_InitCommunality[i]);
				if(i==0)
				{
					min=m_InitCommunality[i];
					max=m_InitCommunality[i];
					maxFieldName=m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]);
					minFieldName=m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]);
				}
				else//ufoufo
				{
					if(min>m_InitCommunality[i])
					{
						min=m_InitCommunality[i];
						minFieldName=m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]);
						
					}
					if(max<m_InitCommunality[i])
					{
						max=m_InitCommunality[i];
						maxFieldName=m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]);						
					}					
				}
			}
			pETable= new CRsltElementTable("���Է������",pTable);  //����һ���������Ķ���
			pResult->Add(pETable);
			
		}

		if( m_bComBigOne )
		{
			pTable->SetTail("��Ϊ��������Ԫ�ش���1,�����غɾ������!");
			//{{{START 2003-2-12 Eric
			m_bAntiImage = false;
			m_bAntiImage2 = false;
			m_bShengyu = false;
			m_bZaisheng = false;
			m_bKMO = false;
			m_bScore = false;
			m_nMissingMethod = false;
			//}}}END  2003-2-12 Eric
		}
		
	}

	if(m_bCovarian)
	{//�漰Э����ķ���������,Ҫ�Թ��Է�����е������ж��Ƿ�Ϸ�,δ����ǰ�Ĺ��Է���Ҫ�μ�����Э����ļ���.

/*		if(m_nAnaMethod == 1 && m_bZaisheng)
		{//Ҫ��������Э����	
			if(m_pUnAdjustComMatrix != NULL)
			{
				delete m_pUnAdjustComMatrix;
				m_pUnAdjustComMatrix = NULL;
			}
			m_pUnAdjustComMatrix = new double[m_nCol * m_nSel];
			memcpy(m_pUnAdjustComMatrix,m_pComMatrix,sizeof(double) * m_nCol * m_nSel);
		}
  */
		//double * pOriDataCorr =
		//	RelationMatrixEx(m_pInputData, m_nRow,m_nCol,m_weight,1);
//{{{START 2003-2-19 Eric
		//if(m_nExtractMethod == 0)
		//	AdjuestResult();
//}}}END  2003-2-19 Eric
		//WriteData(m_pCommunality,m_nSel,1,"���Ĺ��Է���");
		////TraceVector(m_VectCommunality);
		////TraceVector(m_VectCommunality);
		for(i = 0;i<m_nSel;i++)
		{
			if(m_VectCommunality.vlen()>0 && m_VectCommunality[i] > 1)
				m_nIteration = -1;
		}


		if(m_nIteration == -1)
		{
			pTable = new CTLTable;
//			pTable->SetRows(1);
//			pTable->SetCols(1);

			CTString str;
			str.Format("���棺\n���Է�����ڷ���,������ȡʧ��!");
			pTable->InsertColumn(0,str);
			pETable= new CRsltElementTable("����",pTable);
			m_pResult->Add(pETable);

			return;
		}
		else 
		{//����Ҫ����KMO,��ȼ���
			if(m_nAnaMethod == 1 && m_bZaisheng)
			{//Ҫ��������Э����	
				if(m_VectComMatrix.vlen()>0 && m_UnAdjustComMatrix.vlen()>0)
				{
				//	delete m_pComMatrix;
				//	m_pComMatrix = NULL;
					m_VectComMatrix.create(m_UnAdjustComMatrix.vlen());
					m_VectComMatrix = copy(m_UnAdjustComMatrix);
					m_UnAdjustComMatrix.destruction();
				}
			}
		}

	}
	if( !m_bComBigOne)
	{
//���������غɾ���
		pTable = new CTLTable;
		pTable->SetTitle("�����غɾ���");
		pTable->SetHead(m_strExtractMethod);
//		pTable->SetCols(m_nSel+1);
//		if(m_nProcType == 1)
//			pTable->SetRows(m_tVarInfo.iCount+1);
//		else
//			pTable->SetRows(m_tVarInfo2.iCount+1);


		pTable->InsertColumn(0,"");
		for(i=0;i<m_nSel;i++)
		{
			CTString str;
			str.Format("����%d",i+1);
			pTable->InsertColumn(i+1,str);	
		}
//{{{START 2003-2-21 Eric
		ProcessComMatrix(m_VectComMatrix,m_nCol,m_nSel);
//}}}END  2003-2-21 Eric

		if(m_nProcType == 1)
		{	
			int nCol = m_tVarInfo.iCount;
			for(i=0;i<nCol;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				for(int j=0;j<m_nSel;j++)
				{
					pTable->SetItemText(i,j+1,m_VectComMatrix[i+j*nCol]);
				}
			}
		}
		else
		{
			int nCol = m_tVarInfo2.iCount;
			for(i=0;i<nCol;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
				for(int j=0;j<m_nSel;j++)
				{
					pTable->SetItemText(i,j+1,m_VectComMatrix[i+j*nCol]);
				}
			}
		}
		if( m_nIteration > 0 )
		{
			CTString str;
			str.Format("�ܹ��õ�������%d ",m_nIteration);
			pTable->SetTail(str);
		}
		pETable=new CRsltElementTable("�����غɾ���",pTable);//����һ���������Ķ���
		pResult->Add(pETable);
//		if(m_VectEigenValue.vlen()>0 && m_nProcType == 1 && ((m_nExtractMethod ==2)||(1==m_nExtractMethod)))
//			CreateSquareSumExplainTable();//������Ӻ���ƽ���͹��ױ�

		if (m_VectComMatrix.vlen()>0 && m_nSel>1 )
		{
			TDataPointValVal* pDPVV =new TDataPointValVal[m_VectComMatrix.vlen()/m_nSel];
			int n = m_VectComMatrix.vlen();
			int nCol = m_VectComMatrix.vlen()/m_nSel;
			int  k=0; 
			for(k=0; k<m_VectComMatrix.vlen()/m_nSel; k++)
			{
				pDPVV[k].fXVal = m_VectComMatrix(k);
				pDPVV[k].fYVal = m_VectComMatrix(k+nCol);
			}
			CTChartScatterPlot* pCL=new CTChartScatterPlot;
			CTString strTit = "�����غ�ͼ";
			pCL->SetTitle(strTit);
			CTString strX = "��һ����";
			pCL->SetXAxilLabel(strX);
			CTString strY = "�ڶ�����";
			pCL->SetYAxilLabel(strY);
			pCL->SetScatter(pDPVV,nCol,ValVal);
			CRsltElementChart *pREC=new CRsltElementChart("�����غ�ͼ",pCL);
			delete pDPVV;
			pResult->Add(pREC);
//			m_VectComMatrix.destruction();
		}

		if(m_VectScore.vlen() > 0 && m_nSel >1 )
		{
			TDataPointValVal* pDPVV =new TDataPointValVal[m_nRow];        
			int  k=0; 
			for(k=0; k<m_nRow; k++)
			{
				pDPVV[k].fXVal = m_VectScore[k];
				pDPVV[k].fYVal = m_VectScore[k+m_nRow];
			}
			CTChartScatterPlot* pCL=new CTChartScatterPlot;
			CTString strTitle = "���ӵ÷�ͼ";
			pCL->SetTitle(strTitle);
			CTString strX = "��һ����";
			pCL->SetXAxilLabel(strX);
			CTString strY = "�ڶ�����";
			pCL->SetYAxilLabel(strY);
			pCL->SetScatter(pDPVV,m_nRow,ValVal);
			CRsltElementChart *pREC=new CRsltElementChart("���ӵ÷�ͼ",pCL);
			delete pDPVV;
			pResult->Add(pREC);
			m_VectScore.destruction();
		}
	}

//������ת��������غɾ���	
	if( m_nRotateMethod != 0 && m_VectRotateCom.vlen() && !m_bComBigOne && m_nProcType == 1)
	{
		pTable = new CTLTable;
		pTable->SetTitle("��ת��������غɾ���");
		pTable->SetHead(m_strRotateMethod);
		pTable->SetCols(m_nSel+1);
	//	if(m_nProcType == 1)
	//			pTable->SetRows(m_tVarInfo.iCount+1);
	//		else
	//			pTable->SetRows(m_tVarInfo2.iCount+1);
	

		pTable->InsertColumn(0,"");
		for(i=0;i<m_nSel;i++)
		{
			CTString str;
			str.Format("����%d",i+1);
			pTable->InsertColumn(i+1,str);	
		}

		if(m_nProcType == 1)
		{
			for(i=0;i<m_tVarInfo.iCount;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				for(int j=0;j<m_nSel;j++)
				{                                                        
					pTable->SetItemText(i,j+1,m_VectRotateCom[i+j*m_nCol]);
				}
			}
		}
		else
		{
			for(i=0;i<m_tVarInfo2.iCount;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
				for(int j=0;j<m_nSel;j++)
				{                                                        
					pTable->SetItemText(i,j+1,m_VectRotateCom[i+j*m_nCol]);
				}
			}
		}
		pETable=new CRsltElementTable("��ת��������غɾ���",pTable);//����һ���������Ķ���
		pResult->Add(pETable);
		//��ת������ӷ���ױ�
		if(m_VectRotEigenValue.vlen()>0 && m_nProcType == 1)
			CreateRotedExplainTable();//�����ת������ӷ���ױ�

		if (m_VectRotateCom.vlen() > 0 && m_nSel > 1)
		{
			int nCol = m_VectRotateCom.vlen()/m_nSel;
			TDataPointValVal* pDPVV =new TDataPointValVal[nCol];        
			for(int  k=0; k<nCol; k++)
			{
				pDPVV[k].fXVal=m_VectRotateCom[k];
				pDPVV[k].fYVal=m_VectRotateCom[k+nCol];
			}
			CTChartScatterPlot* pCL=new CTChartScatterPlot;
			CTString strTit = "��ת�����غ�ͼ";
			pCL->SetTitle(strTit);
			CTString strX = "��һ��ת����";
			pCL->SetXAxilLabel(strX);
			CTString strY = "�ڶ���ת����";
			pCL->SetYAxilLabel(strY);
			pCL->SetScatter(pDPVV,nCol,ValVal);
			CRsltElementChart *pREC=new CRsltElementChart("��ת�����غ�ͼ",pCL);
			delete pDPVV;
			pResult->Add(pREC);
		}
		if (m_VectRotateScore.vlen() > 0 && m_nSel > 1)
		{
			TDataPointValVal* pDPVV =new TDataPointValVal[m_nRow];        
			for(int  k=0; k<m_nRow; k++)
			{
				pDPVV[k].fXVal = m_VectRotateScore[k];
				pDPVV[k].fYVal = m_VectRotateScore[k+m_nRow];
			}
			CTChartScatterPlot* pCL=new CTChartScatterPlot;
			CTString strTit = "��ת���ӵ÷�ͼ";
			pCL->SetTitle(strTit);
			CTString strX = "��һ��ת����";
			pCL->SetXAxilLabel(strX);
			CTString strY = "�ڶ���ת����";
			pCL->SetYAxilLabel(strY);
			pCL->SetScatter(pDPVV,m_nRow,ValVal);
			CRsltElementChart *pREC=new CRsltElementChart("��ת���ӵ÷�ͼ",pCL);
			delete pDPVV;
			pResult->Add(pREC);
			m_VectRotateScore.destruction();
		}
	}
//����б�����
	if( m_nRotateMethod == 4 && m_nProcType == 1)
	{
		//б�����������
		pTable = new CTLTable;
		pTable->SetTitle("б�����������");
//		pTable->SetCols(m_nSel+1);
//		pTable->SetRows(m_nSel+1);

		pTable->InsertColumn(0,"");
		for(i=0;i<m_nSel;i++)
		{
			CTString str;
			str.Format("%d",i+1);
			pTable->InsertColumn(i+1,str);	
		}
		for(i=0;i<m_nSel;i++)
		{
			CTString str;
			str.Format("%d",i+1);
			pTable->InsertItem(i,str);
			for(int j=0;j<m_nSel;j++)
			{
				pTable->SetItemText(i,j+1,m_pL[i*m_nSel+j]);
			}
		}
		pETable=new CRsltElementTable("б�����������",pTable);//����һ���������Ķ���
		pResult->Add(pETable);
		//б�����ӽṹ��
		int nTemp;
		if(m_nProcType == 1)
			nTemp = m_tVarInfo.iCount;
		else
			nTemp = m_tVarInfo2.iCount;

		CDoubleMatrix matrix(m_pAf,nTemp,m_nSel);

		CreateMatrixResult(matrix,"б�����ӽṹ��");
		//б�������غ���
		CDoubleMatrix matrix2(m_pS,nTemp,m_nSel);
		CreateMatrixResult(matrix2,"б�������غ���");
	}

//�������ӵ÷ֽ��
	if( m_VectScore.vlen()>0 && m_bScore && m_nProcType == 1 && m_bOutput)
	{
		int nOrgColNums=m_pOriDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
		int i=0;
		for(i=0;i<m_nSel;i++)
		{
			CTString strFieldName;
			CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			strFieldName.Format("����%d",i+1);
			while(IsExisted(m_pOriDataInterface,strFieldName))
			{
				strFieldName+="_1";
			}
			pField->SetFieldName(strFieldName);
			m_pOriDataInterface->m_DataAccess.m_pFieldList->Add(pField);

		}
		
		int nRecNo = 1;
		m_pOriDataInterface->m_DataAccess .First();
		
		for(i = 0;i<m_nRow;)
		{
			while(nRecNo < m_arrRecNo[i])
			{
				m_pOriDataInterface->m_DataAccess.Edit();
				for(int j = 0;j<m_nSel;j++)
				{
					CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j+nOrgColNums);
					pField->SetNull();
					
				}
				m_pOriDataInterface->m_DataAccess.Post();			
				m_pOriDataInterface->m_DataAccess.Next();
				nRecNo++;
			}
			m_pOriDataInterface->m_DataAccess.Edit();
		//	////TraceVector(m_VectScore);
			
			for(int j = 0;j<m_nSel;j++)
			{
				CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j+nOrgColNums);
				double v =m_VectScore[i+j*m_nRow];
				pField->SetAsDouble(v);
			}
			i++;
			m_pOriDataInterface->m_DataAccess.Post();			
			m_pOriDataInterface->m_DataAccess.Next();
			nRecNo++;
		}	
	/*
		int i;
		CTLTable * pTable = NULL;
		pTable = new CTLTable();

		pTable->SetTitle("���ӵ÷�");
		pTable->SetHead(m_strScoreMethod);
		pTable->SetRows(m_nRow);
		pTable->SetCols(3);
		pTable->InsertColumn(0,"��¼��");
		pTable->InsertColumn(1,"Ȩ��");

		char szNum1[16];
		char szNum2[10];
		strcpy(szNum1,"����");

		for(i = 0;i < m_nSel;i++)
		{
			pTable->InsertColumn(i + 2,strcat(szNum1,itoa(i+1,szNum2,10)));
			szNum1[4] = 0;
		}
		int nRecNo = 1;
		m_pDataInterface->m_DataAccess.First();

		for(i = 0;i<m_nRow;)
		{
			while(nRecNo < m_arrRecNo[i])
			{
				pTable->InsertItem(nRecNo,itoa(nRecNo,szNum2,10));
				pTable->SetItemText(nRecNo,1,0);
				for(int j = 0;j<=m_nSel;j++)
				{
					pTable->SetItemText(nRecNo,j+2,"ȱʧ");
				}
				m_pDataInterface->m_DataAccess.Next();
				nRecNo++;
			}

			pTable->InsertItem(nRecNo,itoa(nRecNo,szNum2,10));
			pTable->SetItemText(nRecNo,1,m_weight(i));
			for(int j = 0;j<m_nSel;j++)
			{
				pTable->SetItemText(nRecNo,j+2,m_pScore[i*m_nSel+j]);
			}
			i++;
			m_pDataInterface->m_DataAccess.Next();
			nRecNo++;
		}

		pTable->SetTail("ע:Ȩ��Ϊ0��ʾ������Ȩ������ȱʧֵ,��Ȩ��С�ڵ���0");

		CRsltObject * pETable= new CRsltElementTable("���ӵ÷�",pTable);
		m_pResult->Add(pETable);
		*/
	}
	m_VectScore.destruction();
	m_VectRotateCom.destruction();
}
 
//������������������������ʯͼ
void	CFactor::CreateChartResult(CDoubleVector & vect, int nLen)
{
	CRsltElementChart* pREC;
	TDataPointValVal* pDPSV=new TDataPointValVal[nLen];
	int vl = vect.vlen();
	for(int j=0;j<nLen;j++)
	{
		pDPSV[j].fXVal =j+1;
		pDPSV[j].fYVal =vect(vl-j-1);
	}
	
	CTChartLine *pTChartLine=new CTChartLine;
	CTString strTitle = "����������ʯͼ";
	pTChartLine->SetTitle(strTitle);
	CTString strX = "Ԫ����Ŀ";
	pTChartLine->SetXAxilLabel(strX);
	CTString strY =  "����ֵ";
	pTChartLine->SetYAxilLabel(strY);
	pTChartLine->SetLine(pDPSV,nLen,ValVal);
	pTChartLine->SetData(pDPSV,nLen,EScatter,ValVal);
	pREC=new CRsltElementChart(strTitle,pTChartLine);
	m_pResult->Add(pREC);    
	delete[] pDPSV;
}

//�������������������ֵ
void CFactor::CreateMatrixResult(CDoubleMatrix matrix, char *pszTitle)
{
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle(pszTitle);
//	pTable->SetCols(matrix.mcol()+1);
//	pTable->SetRows(matrix.mrow()+1);

	pTable->InsertColumn(0,"");
	int i=0;
	for(i=0;i<matrix.mcol();i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertColumn(i+1,str);	
	}
	for(i=0;i<matrix.mrow();i++)
	{
		if(m_nProcType == 1)
			pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		else
			pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

		int j=0;
		for(j=0;j<matrix.mcol();j++)
		{
			pTable->SetItemText(i,j+1,matrix(i,j));
		}
	}
	CRsltElementTable * pETable= new CRsltElementTable(pszTitle,pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
}




// ������������������
//	a:     ˫���ȶ�ά����n*n,��ŶԳƾ���A������ʱ�Խ����ϴ��n������ֵ
//	n:     A�Ľ���
//	v��    n*n ������������
//	eps��  ���ƾ���Ҫ��
void CFactor::cjcbj(CDoubleVector& Vect_A,int n,CDoubleVector& Vect_V,double eps) 
{ 
	int i,j,p,q,u,w,t,s;
	double ff,fm,cn,sn,omega,x,y,d;
	for (i=0; i<=n-1; i++)
	{ Vect_V[i*n+i]=1.0;
	for (j=0; j<=n-1; j++)
	if (i!=j) Vect_V[i*n+j]=0.0;
	}
	ff=0.0;
	for (i=1; i<=n-1; i++)
	for (j=0; j<=i-1; j++)
	{ d=Vect_A[i*n+j]; ff=ff+d*d; }
	ff=sqrt(2.0*ff);

	loop0:
	ff=ff/(1.0*n);

	loop1:
	for (i=1; i<=n-1; i++)
	for (j=0; j<=i-1; j++)
	{ d=fabs(Vect_A[i*n+j]);
	if (d>ff)
	{ p=i; q=j;
	goto loop;
	}
	}
	if (ff<eps) return;
	goto loop0;

	loop: 
	u=p*n+q; w=p*n+p; t=q*n+p; s=q*n+q;
	x=-Vect_A[u]; y=(Vect_A[s]-Vect_A[w])/2.0;
	omega=x/sqrt(x*x+y*y);
	if (y<0.0) omega=-omega;
	sn=1.0+sqrt(1.0-omega*omega);
	sn=omega/sqrt(2.0*sn);
	cn=sqrt(1.0-sn*sn);
	fm=Vect_A[w];
	Vect_A[w]=fm*cn*cn+Vect_A[s]*sn*sn+Vect_A[u]*omega;
	Vect_A[s]=fm*sn*sn+Vect_A[s]*cn*cn-Vect_A[u]*omega;
	Vect_A[u]=0.0; Vect_A[t]=0.0;
	for (j=0; j<=n-1; j++)
	if ((j!=p)&&(j!=q))
	{ u=p*n+j; w=q*n+j;
	fm=Vect_A[u];
	Vect_A[u]=fm*cn+Vect_A[w]*sn;
	Vect_A[w]=-fm*sn+Vect_A[w]*cn;
	}
	for (i=0; i<=n-1; i++)
	if ((i!=p)&&(i!=q))
	{ u=i*n+p; w=i*n+q;
	fm=Vect_A[u];
	Vect_A[u]=fm*cn+Vect_A[w]*sn;
	Vect_A[w]=-fm*sn+Vect_A[w]*cn;
	}
	for (i=0; i<=n-1; i++)
	{ u=i*n+p; w=i*n+q;
	fm=Vect_V[u];
	Vect_V[u]=fm*cn+Vect_V[w]*sn;
	Vect_V[w]=-fm*sn+Vect_V[w]*cn;
	}
	goto loop1;
}
//ʹ�����ÿһ�еĺ�Ϊ��.  ˵��
void CFactor::ProcessComMatrix(CDoubleVector& Vect_Data, int nRow, int nCol)
{
	for(int i=0;i<nCol;i++)
	{
		double sum = 0;
		int j=0;
		for(j=0;j<nRow;j++)
		{
			sum += Vect_Data[j*nCol+i];			
		}
		if( sum < 0 )
		{
			for(j=0;j<nRow;j++)
			{
				Vect_Data[j*nCol+i] = -1 * Vect_Data[j*nCol+i];
			}
		}
	}
}

//����ת�������Ӿ���������¼��㣺����ÿ��Ԫ�ص�ƽ����
//Ȼ����ݸ�ƽ���ͽ����Ӵ����ң��ɴ�С���С� ͬʱʹÿ�еĺ�Ϊ��.
void CFactor::ProcessRotateMatrix(CDoubleMatrix &matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	int i;


	//����ÿ�еĺͼ�ƽ����.
	CDoubleVector vectsumsquare(nCol);
	CDoubleVector vectsum(nCol);
	for(i=0;i<nCol;i++)
	{
		double sumsquare = 0;
		double sum = 0;
		for(int j=0;j<nRow;j++)
		{
			sumsquare += matrix(j,i) * matrix(j,i);
			sum += matrix(j,i);
		}
		vectsumsquare[i] = sumsquare;
		vectsum[i] = sum;
	}



	for(i=0;i<nCol;i++)
	{
		//�ӵ�i���Ժ������,ѡ�������.
		double max = vectsumsquare[i];

		int nIndex = i;//�������ֵ���ڵ���

		int j=0;
		for(j=i;j<nCol;j++)
		{
			if( vectsumsquare[j] > max )
			{
				max    = vectsumsquare[j];
				nIndex = j;
			}
		}

		double temp ;
		double fTemp1;

		//���н���.
		for(j=0;j<nRow;j++)
		{
			temp = matrix(j,i);
			if(vectsum[nIndex]<0)
			{
				fTemp1 = -matrix(j,nIndex);
			}
			else
			{
				fTemp1 = matrix(j,nIndex);
			}
			matrix(j,nIndex) = temp;
			matrix(j,i) = fTemp1;
		}

		//ƽ���ͽ���.
		temp = vectsumsquare[i];
		fTemp1 = vectsumsquare[nIndex];
		vectsumsquare[nIndex] = temp;
		vectsumsquare[i] = fTemp1;

		//�ͽ���
		temp = vectsum[i];
		fTemp1 = vectsum[nIndex];
		vectsum[nIndex] = temp;
		vectsum[i] = fTemp1;

		vectsum[i] = 1;//����
	}


}

void CFactor::CreateCommulityTable()//������Է������
{
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("���Է������");
	bool bDoubleCol = false;
	if(m_InitCommunality.vlen()>0)
	{
		bDoubleCol = true;
		//pTable->SetCols(3);
	}
//	else
//		pTable->SetCols(2);


	int nRow =m_tVarInfo.iCount;
//	pTable->SetRows(nRow+1);

	pTable->InsertColumn(0,"");
	if(bDoubleCol)
	{
		pTable->InsertColumn(1,"��ʼֵ");
		pTable->InsertColumn(2,"��ȡֵ");
	}
	else
		pTable->InsertColumn(1,"��ȡֵ");
		
	
	int i=0;
	for(i=0;i<nRow;i++)
	{
		if(m_nProcType == 1)
			pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		else
			pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
		
		if(bDoubleCol)
		{
			pTable->SetItemText(i,2,m_VectCommunality[i]);	
			pTable->SetItemText(i,1,m_InitCommunality[i]);
		}
		else
			pTable->SetItemText(i,1,m_VectCommunality[i]);

		
	}
	CRsltElementTable * pETable= new CRsltElementTable("���Է������",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);

}
//�������������� ���������ۼƹ�����
void CFactor::CreateEigenValueResult(CDoubleVector Vect_Data,const int nLen)
{
	double sum = 0;
	int i=0;
	for(i=0;i<nLen;i++)
	{
		sum += Vect_Data[i*nLen+i];
	}
	//���������ۼƹ�����
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("���������ۼƹ�����");
//	pTable->SetCols(2);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"����");
	pTable->InsertColumn(1,"������");
	pTable->InsertColumn(2,"�������%");
	pTable->InsertColumn(3,"�ۼƹ�����%");
	double sumpro = 0;
	CTString strExplain;
	CTString strTotal;
	double last=0;
	for(i=0;i<nLen ;i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str);
		pTable->SetItemText(i,1,Vect_Data[i*nLen+i]);
		sumpro += Vect_Data[i*nLen+i] / sum ;
		pTable->SetItemText(i,2,100 * Vect_Data[i*nLen+i] / sum );
		pTable->SetItemText(i,3,100 * sumpro);
		if(m_nExtractStandard==0 &&last<m_fPro)
		{
			strExplain.Format("��%d������������ֵΪ%.4f,����������Ϣ����%.2f%%��\n",i+1,Vect_Data[i*nLen+i],100 * Vect_Data[i*nLen+i] / sum);
			strTotal+=strExplain;
		}
		else if(m_nExtractStandard==1&& i<m_nFactorNum)
		{
			strExplain.Format("��%d������������ֵΪ%.4f,����������Ϣ����%.2f%%��\n",i+1,Vect_Data[i*nLen+i],100 * Vect_Data[i*nLen+i] / sum);
			strTotal+=strExplain;
		}
		last=sumpro;
	}
	m_strResultExplain.Add(strTotal);
	CRsltElementTable * pETable= new CRsltElementTable("���������ۼƹ�����",pTable);
	m_pResult->Add(pETable);	

	
	///zkc 2003.10.23 �����������
	pTable = new CTLTable;
	pTable->SetTitle("��������");
//	pTable->SetCols(nLen+1);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"");	
	for(i=0;i<nLen;i++)
	{
		CTString colHeader;
		colHeader.Format("��������%d",i+1);
		pTable->InsertColumn(i+1,colHeader);	
		
	}
	for(i=0;i<nLen ;i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str,false);
		for(int j=0;j<nLen;j++)
		{
			pTable->SetItemText(i,j+1,m_EigenVector[i*nLen+j]);
		}
	}
	pETable= new CRsltElementTable("��������",pTable);
	m_pResult->Add(pETable);	
	
}

//�������������� ���������ۼƹ�����
void CFactor::CreateEigenValueTable()
{
	if(m_nIteration >m_nMaxIterCount)
	{
		CTLTable*	pTable = new CTLTable;
//		pTable->SetRows(1);
//		pTable->SetCols(1);

		CTString str;
		str.Format("���棺\n�ﵽ����������,��ȡ�����ӿ��ܲ���ȷ!");
		pTable->InsertColumn(0,str);
		CRsltElementTable *pETable= new CRsltElementTable("����",pTable);
		m_pResult->Add(pETable);
		
		return;
	}
	
	double sum = m_VectSelEigenValue.sum();
	//���������ۼƹ�����
	int nLen = m_VectSelEigenValue.vlen();
	//////////////////////////////////////////////////////////////////////////
	//���Լ���ϵ����������ϵ����
	//////////////////////////////////////////////////////////////////////////
	if(m_VectSelEigenValue.vlen()>0)
	{
		sum = m_VectSelEigenValue.sum();
		//���������ۼƹ�����
		CTLTable * pTable = new CTLTable;
		CTString sTemp("");
		sTemp.Format("Լ���ϵ����������������ۼƹ����� ");
		pTable->SetTitle(sTemp);
//		pTable->SetCols(4);
//		pTable->SetRows(1+nLen);

		pTable->InsertColumn(0,"����");
		pTable->InsertColumn(1,"������");
		pTable->InsertColumn(2,"�������%");
		pTable->InsertColumn(3,"�ۼƹ�����%");
		double sumpro = 0;
		CTString strExplain;
		CTString strTotal;
		double last=0;
		int i=0;
		for(i=0;i<nLen ;i++)
		{
			CTString str;
			str.Format("%d",i+1);
			pTable->InsertItem(i,str);
			pTable->SetItemText(i,1,m_VectSelEigenValue[i]);
			sumpro += m_VectSelEigenValue[i] / sum ;
			pTable->SetItemText(i,2,100 * m_VectSelEigenValue[i] / sum );
			pTable->SetItemText(i,3,100 * sumpro);
			if(m_nExtractStandard==0 &&last<m_fPro)
			{
				strExplain.Format("��%d������������ֵΪ%.4f,����������Ϣ����%.2f%%��\n",i+1,m_VectSelEigenValue[i],100 * m_VectSelEigenValue[i] / sum);
				strTotal+=strExplain;
			}
			else if(m_nExtractStandard==1&& i<m_nFactorNum)
			{
				strExplain.Format("��%d������������ֵΪ%.4f,����������Ϣ����%.2f%%��\n",i+1,m_VectSelEigenValue[i],100 * m_VectSelEigenValue[i] / sum);
				strTotal+=strExplain;
			}
			last=sumpro;
		}
		m_strResultExplain.Add(strTotal);
		CRsltElementTable * pETable= new CRsltElementTable("���������ۼƹ�����",pTable);
		m_pResult->Add(pETable);	
	}
	//////////////////////////////////////////////////////////////////////////
	
}

//��������������	������Э����  ����������ȼ���	���ؿ���ֵ.
double CFactor::Sphericity(CDoubleMatrix matrix_x)
{
	int n = matrix_x.mrow();
	int p = matrix_x.mcol();
	//{{{START 2003-1-25 Eric
//����anti-image covariance������Э����
	CDoubleMatrix matrix_R = RelationMatrixEx(matrix_x);
	CDoubleMatrix matrix_r = pinv(matrix_R); 
	CDoubleMatrix matrix_A(p,p);

	int i=0;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			matrix_A(i,j) = matrix_r(i,j) / ( matrix_r(i,i) * matrix_r(j,j) );
		}
	}
	//}}}END  2003-1-25 Eric

	//{{{START 2003-1-25 Eric
//����chi-square value for Bartlett's test of sphericity
	//double chiSquare = - (n-1-(2*p+5)/6.0)*log(det(matrix_R));

	double fWeight = 0;
	double chiSquare;

	//m_weight.vlen()�϶����ڣ�
	for(i = 0;i<m_weight.vlen();i++)
	{
		if(m_weight[i] < 0)
		{
			m_weight[i] = 0;
		}
		fWeight += m_weight[i]; 
	}

	double fTemp2 = log(det(matrix_R));
	chiSquare = - (fWeight-1-(2*p+5)/6.0)*fTemp2;

	//}}}END  2003-1-25 Eric
	int freedom = p*(p-1) / 2;
	//{{{START 2003-1-28 Eric
	double alpha;
	if(chiSquare > 1e308)
	{//
		alpha = 1 - 1;//chi2cdf(chiSquare,freedom)=1;
	}
	else
	{
		if(chiSquare < -1e308)
		{//����
			if(m_bKMO)
			{
				CTLTable * pTable = new CTLTable;
//				pTable->SetRows(1);
//				pTable->SetCols(1);

				CTString str;
				if(freedom<=0)
					str.Format("���棺\n���ɶ�(%d)������,������ܲ���ȷ!",freedom);
				else
					str.Format("���棺\n���ݲ�����,������ܲ���ȷ!");
				pTable->InsertColumn(0,str);
				CRsltElementTable * pETable= new CRsltElementTable("����",pTable);
				m_pResult->Add(pETable);
			}

			m_bKMO = false;

		}
		else
			alpha = 1 - chi2cdf(chiSquare,freedom);
	}


	//}}}END  2003-1-28 Eric
	if( m_bAntiImage2 && !m_bComBigOne )
	{
//������Э�������
		CTLTable * pTable = new CTLTable;
		pTable->SetTitle("������Э�������");
//		pTable->SetRows(matrix_A.mrow()+1);
//		pTable->SetCols(matrix_A.mcol()+1);

		pTable->InsertColumn(0,"");
		for(i=0;i<matrix_A.mcol();i++)
		{
			if(m_nProcType == 1)
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

		}
		for(i=0;i<matrix_A.mcol();i++)
		{
			if(m_nProcType == 1)
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

			for(int j=0;j<matrix_A.mrow();j++)
			{
				pTable->SetItemText(i,j+1,matrix_A(i,j));
			}
		}
		CRsltElementTable * pETable= new CRsltElementTable("������Э�������",pTable);
		m_pResult->Add(pETable);
	}

	if(m_bKMO)//
	{
//����������ȼ���
		CTLTable * pTable = new CTLTable;
		pTable->SetTitle("����������ȼ���");
//		pTable->SetRows(5);
//		pTable->SetCols(2);

		pTable->InsertColumn(0,"����");
		pTable->InsertColumn(1,"ֵ");

		pTable->InsertItem(0,"����ֵ");
		pTable->InsertItem(1,"���ɶ�");
		pTable->InsertItem(2,"������");

		pTable->SetItemText(0,1,chiSquare);
		pTable->SetItemText(1,1,freedom);
		pTable->SetItemText(2,1,alpha);
		CTString strExplain;
		CTString strType;
		if(	m_bPrincipal)
		{
			strType="���ɷ�";
		}
		else
		{
			strType="����";
		}
		
		if(alpha<=0.05)
		{
			strExplain.Format("��������䲻�������ʺϽ���%s���������ʼ���ֵ=%.4f����",strType.GetData(),alpha);
		}
		else
		{
			strExplain.Format("�����������������ʺϽ���%s���������ʼ���ֵ=%.4f����",strType.GetData(),alpha);
		}
		m_strResultExplain.Add(strExplain);
		CRsltElementTable * pETable= new CRsltElementTable("����������ȼ���",pTable);
		m_pResult->Add(pETable);
	}
//}}}END  2003-1-25 Eric
	return chiSquare;
}

//���ƫ���ϵ����x1,x2��ָ�����кţ�������������ϵ��ֵ
double CFactor::PCorrelation(const int x1, const int x2)
{
	int n = m_nRow;                //�������ݵ�����
	int k = m_nCol;				   //x������
	int j=0;
	CDoubleVector Vect_X(n * k);
	CIntVector Vect_Pos(k);
	
	//������зſ��Ʊ������ݣ�ǰ�����зŲ鿴��������
	if(m_nProcType == 1)
	{
		for(int i=0;i<m_tVarInfo.iCount;i++)
		{
			int nColNum = m_tVarInfo.pValue[i];
			if( nColNum != x1 && nColNum != x2 && nColNum >= 0 )
			{
				for(int ii=0;ii<n;ii++)
				{
					Vect_X[ii*k+(j+2)] = m_VectInputData[k*ii+nColNum];
				}
				j++;
			}
		}
	}
	else
	{
		for(int i=0;i<m_tVarInfo2.iCount;i++)
		{
			int nColNum = m_tVarInfo2.pValue[i];
			if( nColNum != x1 && nColNum != x2 && nColNum >= 0 )
			{
				for(int ii=0;ii<n;ii++)
				{
					Vect_X[ii*k+(j+2)] = m_VectInputData[k*ii+nColNum];
				}
				j++;
			}
		}
	}

	int ii=0;
	for(ii=0;ii<n;ii++)
	{
		Vect_X[ii*k]   = m_VectInputData[ii*k+x1];
		Vect_X[ii*k+1] = m_VectInputData[ii*k+x2];
	}
	Formalize(Vect_X,n,k);
	CDoubleVector Vect_Rr = RelationMatrixEx(Vect_X,n,k,m_weight);  //calculate relation matrix
 	for(ii=0;ii<k;ii++)
	{
		Vect_Pos[ii] = ii;
	}
	double r = partial(Vect_Rr,Vect_Pos,k,k);	

	return r;
}

CDoubleMatrix CFactor::GetPCorrelationMatrix(CDoubleVector Vect_Prr)
{
	CDoubleMatrix matrix(m_nCol,m_nCol);
	int i=0;
	for(i=0;i<m_nCol;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			if( i == j )
			{
				matrix(i,j) = 1;
			}
			else if ( j > i )
			{
				matrix(i,j) = -PCorrelation(i,j,Vect_Prr);
			}
			else
			{
				matrix(i,j) = matrix(j,i);
			}
		}
	}
	return matrix;
}

//��ص�ͳ������������
void CFactor::KMO(int nType)
{
	int i;//j;
	CTLTable * pTable = NULL;
	CRsltElementTable * pETable = NULL;
	////TraceVector( m_VectInputData);
	Formalize(m_VectInputData,m_nRow,m_nCol);
	////TraceVector( m_VectInputData);
	////TraceVector(m_VectFirstCovariance);
	CDoubleVector rr;
	if(m_bCovarian && m_VectFirstCovariance.vlen())
	{
		rr = copy(m_VectFirstCovariance);  
	}
	else
	{
		rr = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight);  
	}


	i=0;
	if( m_nCol > 2 && m_nCol<13&& (m_bAntiImage || m_bKMO) )
	{
	
		CDoubleMatrix matrix = GetPCorrelationMatrix(rr);
		//TraceMatrix(matrix);
		int n = matrix.mcol();

		CDoubleVector vector_msa(n);

		double t1 = 0;
		double t2 = 0;
		bool bExist=true;
		for(i=0;i<n;i++)
		{
			double rsum=0;
			double asum=0;
			for(int j=0;j<n;j++)
			{
				if( j!=i)
				{
					rsum += rr[i+j*n]*rr[i+j*n];
					asum += matrix(i,j)*matrix(i,j);
				}
			}
			t1+=rsum;
			t2+=asum;
			if(rsum+asum==0)
			{
				bExist=false;
				break;
			}
			vector_msa[i] = rsum / (rsum + asum);
		}
		
		m_dkmo = t1 / (t1+t2);	
		////TraceVector(vector_msa);
		for(i=0;i<n;i++)
		{
			matrix(i,i) = vector_msa[i];
		}	
		//TraceMatrix(matrix);
		//{{{START 2003-1-26 Eric �����㷴�������ϵ������
		
		if(m_bAntiImage && !m_bComBigOne &&bExist)
		{			
			pTable = new CTLTable;
//			pTable->SetRows(1+n);
//			pTable->SetCols(1+n);

			pTable->SetTitle("���������ϵ������");
			pTable->InsertColumn(0,"");
			if(nType==0)
			{
				for(i=0;i<n;i++)
				{
					pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
					pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				}
				
			}
			else
			{
				for(i=0;i<n;i++)
				{
					pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
					pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
				}
			}
			for(i=0;i<n;i++)
			{
				for(int j=0;j<n;j++)
				{
					pTable->SetItemText(i,j+1,matrix(i,j));
					double aa=matrix(i,j);
					int a=4;
				}
			}
			pETable= new CRsltElementTable("���������ϵ������",pTable);
			m_pResult->Add(pETable);
		}
	
	//}}}END  2003-1-26 Eric
}

	//�������ϵ��
	CDoubleVector Vect_Matrix ;


//���������غɾ���
	////TraceVector(m_VectComMatrix);
	if(m_VectComMatrix.vlen())
	{
		Vect_Matrix = copy(m_VectComMatrix);
	}
	else 
		return ;

	//ʹ��δ��ת�������غɾ���
	if( (m_nExtractMethod == 0 || m_nExtractMethod == 1) && m_nAnaMethod == 1 )
	{
		Vect_Matrix = copy(m_VectUnAdjustCom);
	}
	
	CDoubleMatrix matrix_temp = VectorToMatrix(Vect_Matrix,m_nCol,m_nSel);
	
	CDoubleMatrix matrix_Rep =  matrix_temp * trans(matrix_temp);


	if( m_bZaisheng )
	{
		CTString str;
		if( m_nAnaMethod == 0 )
		{
			str = _T("�������ϵ��");
		}
		else
		{
			str = _T("����Э����ϵ��");
		}  

//�������ϵ��Rep �� ����Э����ϵ��Rep
		pTable = new CTLTable();
		pTable->SetTitle(str);
//		pTable->SetRows(1+m_nCol);
//		pTable->SetCols(1+m_nCol);

		pTable->InsertColumn(0,"");
		int i=0;
		for(i=0;i<m_nCol;i++)
		{
			if(m_nProcType == 1)
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

		}
		for(i=0;i<m_nCol;i++)
		{
			if(m_nProcType == 1)
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

			for(int j=0;j<m_nCol;j++)
			{
				pTable->SetItemText(i,j+1,matrix_Rep(i,j));
			}
		}
		pETable= new CRsltElementTable( str, pTable);
		m_pResult -> Add(pETable);
	}

//���ϵ������в� �� Э�������в�
	if( m_bShengyu )
	{
		CTString str;
		if( m_nAnaMethod == 0 )
		{
			str = _T("���ϵ������в�");
		}
		else
		{
			str = _T("Э�������в�");
		}

		pTable = new CTLTable();
		pTable->SetTitle(str);
//		pTable->SetRows(1+m_nCol);
//		pTable->SetCols(1+m_nCol);

		pTable->InsertColumn(0,"");
		for(i=0;i<m_nCol;i++)
		{
			if(m_nProcType == 1)
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

		}
		for(i=0;i<m_nCol;i++)
		{
			if(m_nProcType == 1)
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

			for(int j=0;j<m_nCol;j++)
			{
				if( i!= j)
				{
					pTable->SetItemText(i,j+1,rr[i+j*m_nCol] - matrix_Rep(i,j));
				}
			}
		}
		//delete [] rr; rr = NULL;
		//rr.destruction();
		pETable= new CRsltElementTable(str,pTable);
		m_pResult->Add(pETable);
	}
}


//*********************************************************************
//Author:zkc
//Date:2003.4.8
//Functionality: ��һ�׵��� --formula (6)��
//*********************************************************************
CDoubleVector CFactor::Ladder1(CDoubleVector vector_x,
			   CDoubleVector vector_gamma,
			   CDoubleMatrix matrix_omiga,
			   int nMothod,		//0-������Ȼ����1-��ͨ��С���˷���2-��Ȩ����С���˷�
			   int p,  //p=nCol
			   int m,
			   int nMethod)
{
	CDoubleVector vector_h(p);
	int i=0;
	for(i=0;i<p;i++)
	{
		double h=0;
		for(int k=m;k<p;k++)
		{
			if(nMethod==0)
			{
				h=h+(1-1.0/vector_gamma(k))*matrix_omiga(i,k)*matrix_omiga(i,k);
			}
			else if(nMethod==1)
			{
				h=h+(vector_gamma(k)*vector_gamma(k)-vector_gamma(k))*matrix_omiga(i,k)*matrix_omiga(i,k);
			}
			else if(nMethod==2)
			{
				h=h+vector_gamma(k)*matrix_omiga(i,k)*matrix_omiga(i,k);

			}
		}
		if(nMethod==2)
		{
			h=2*vector_x(i)*h;
		}
		vector_h(i)=h;
	}
	return vector_h;
}

//*******************************************************************//
//Author:zkc
//Date:2003.4.8
//Functionality:��formula (7)����׵���H.
//********************************************************************//

CDoubleMatrix CFactor::Ladder2(CDoubleVector vector_gamma,
			   CDoubleVector vector_x,
			   CDoubleMatrix matrix_omiga,
			   CDoubleMatrix matrix_r,//���ϵ�����
			   CDoubleVector vector_h,
			   int nMethod,		//0-������Ȼ����1-��ͨ��С���˷���2-��Ȩ����С���˷�
			   int p,
			   int &m)///p=nCol
{
	CDoubleMatrix matrix_H(p,p);
	int i=0;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			double delta=0;
			if(i==j)
				delta=1;
			else
				delta=0;
			double H=0;
			if(nMethod==0)
				H=-1*delta*vector_h(i);
			else if(nMethod==1)
				H=delta*vector_h(i);

			int k=m;
			for(k=m;k<p;k++)
			{
				double temp=0;
				for(int n=0;n<m;n++)
				{
					if(nMethod==0)//������Ȼ��
					{
						temp=temp+(vector_gamma(k)+vector_gamma(n)-2)*matrix_omiga(i,n)*matrix_omiga(j,n)/(vector_gamma(k)-vector_gamma(n));
					}
					else if(nMethod==1)//��ͨ��С���˷�
					{
						temp=temp+(vector_gamma(k)+vector_gamma(n)-2)*matrix_omiga(i,n)*matrix_omiga(j,n)*vector_gamma(n)/(vector_gamma(k)-vector_gamma(n));
					}
					else//��Ȩ����С���˷�
					{
						temp=temp+(vector_gamma(k)+vector_gamma(n))*matrix_omiga(i,k)*matrix_omiga(j,k)/(vector_gamma(k)-vector_gamma(n));
					}
				}
				if(nMethod==0)
				{
					temp=temp+delta;
					temp=temp*matrix_omiga(i,k)*matrix_omiga(j,k);
				}
				else if(nMethod==1)
				{
					temp=temp+matrix_r(i,i)*exp((vector_x(i)+vector_x(j))/2);
					temp=temp*matrix_omiga(i,k)*matrix_omiga(j,k)*vector_gamma(k);
				}
				else if(nMethod==2)
					temp=temp*matrix_omiga(i,k)*matrix_omiga(j,k);
				H=H+temp;
			}
			if(nMethod==2)
			{
				H=H*vector_x(i)*vector_x(j);
				double temp=0;
				for(k=m;k<p;k++)
				{
					temp=temp+(vector_x(i)*vector_x(i)-vector_gamma(k)/2)*matrix_omiga(i,k)*matrix_omiga(i,k);
				}
				temp=temp*delta;
				H=H+temp;
				H=4*H;
			}
			matrix_H(i,j)=H;
		}
	}
	return matrix_H;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.8
//Functionality:�����m�η���
//********************************************************************//

CDoubleMatrix CFactor::powMatrix(CDoubleMatrix matrix,int m)
{
	int nRow=matrix.mrow();
	int nCol=matrix.mcol();
	CDoubleMatrix temp(nRow,nCol);
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
			temp(i,j)=pow(matrix(i,j),m);
	}
	return temp;

}


//*******************************************************************//
//Author:zkc
//Date:2003.4.8
//Functionality:��formula (8)����׵���H
//********************************************************************//

CDoubleMatrix CFactor::Ladder2(CDoubleVector vector_x,
					  CDoubleMatrix matrix_omiga,
					  int &m,int p,
					  int nMethod)
{
	CDoubleMatrix matrix_H(p,p);
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			double temp=0;
			for(int k=m;k<p;k++)
			{
				temp=temp+matrix_omiga(i,k)*matrix_omiga(j,k);
			}
			temp=temp*temp;
			if(nMethod==2)
				temp=temp*4*vector_x(i)*vector_x(j);
			matrix_H(i,j)=temp;
		}
	}
	return matrix_H;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.9
//Functionality:������׵���matrix,��ĳһ���ĶԽ���Ԫ��С��0.05����Ѷ�Ӧ����
//				��Ԫ�ظ�ֵΪ0���öԽ���Ԫ�ظ�ֵΪ1��
//********************************************************************//

void CFactor::ChangeLadder2(CDoubleMatrix &matrix)
{
	int nCol=matrix.mcol();
	int nRow=matrix.mrow();
	for(int i=0;i<nRow;i++)
	{
		if(matrix(i,i)<0.05)
		{
			int m=0;
			for(m=0;m<nRow;m++)
				matrix(m,i)=0;
			for(m=0;m<nCol;m++)
				matrix(i,m)=0;
			matrix(i,i)=1;
		}
	}
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.9
//Functionality:���������ʽ
//********************************************************************//
CDoubleMatrix CFactor::GetSubMatrix(CDoubleMatrix matrix,
						   int m,		//���±�
						   int n)		//���±�
{
	int nCol=matrix.mcol();
	int nRow=matrix.mrow();
	assert(nCol==nRow);
	CDoubleMatrix subMatrix(nRow-1,nCol-1);
	for(int i=0;i<nRow;i++)
	{
		int m1=i;
		if(i==m)
			continue;
		if(i>m)
			m1=i-1;
		for(int j=0;j<nCol;j++)
		{
			int n1=j;
			if(j==n)
				continue;
			if(j>n)
				n1=j-1;
			subMatrix(m1,n1)=matrix(i,j);
		}
	}
	return subMatrix;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.9
//Functionality:�����Ƿ�����
//********************************************************************//
bool CFactor::IsNormal(CDoubleMatrix matrix)
{
	int nCol=matrix.mcol();
	int nRow=matrix.mrow();
	assert(nCol==nRow);
	for(int i=0;i<nRow;i++)
	{
		CDoubleMatrix subMatrix=GetSubMatrix(matrix,i,i);//�Խ���Ԫ�صĴ�������ʽ
		if(det(subMatrix)<=0)
			return false;
	}
	return true;
}

//*******************************************************************//
//Author:zkc
//Date:2003.4.9
//Functionality:�ж�vector_dÿһ��ֵ�Ƿ������0.1
//********************************************************************//

bool CFactor::IsDLegal(CDoubleVector vector_d)
{
	for(int i=0;i<vector_d.vlen();i++)
	{
		if(vector_d(i)<=0.1)
		{
			return false;
		}
	}
	return true;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.10
//Functionality:��Ŀ�꺯����ֵ��ͬʱ����matrix_psi,vector_gamma,matrix_omiga
//********************************************************************//
double CFactor::CalcFx(CDoubleVector vector_x,		//nCol
			  CDoubleMatrix matrix_R,		//���ϵ������,nColxnCol
			  CDoubleMatrix matrix_r,		//matrix_R������,nColxnCol
			  CDoubleMatrix &matrix_psi,	//nCol*nCol
			  CDoubleVector &vector_gamma,	//nCol
			  CDoubleMatrix &matrix_omiga,	//nColxnCol
			  int nMethod,					//0-������Ȼ����1-��ͨ��С���˷���2-��Ȩ����С���˷�
			  int nCol,						//p=nCol
			  int &m)						//��ȡ���ӵĸ���
{

	if(nMethod==0||nMethod==1)
	{
		matrix_psi=sqrt(diag(exp(vector_x)));
	}
	else
	{
		matrix_psi=diag(exp(vector_x));
	}
	
	///////////formula (4)
	CDoubleMatrix matrix_4(nCol,nCol);
	if(nMethod==0||nMethod==1)
	{
		matrix_4=matrix_psi*matrix_r*matrix_psi;
	}
	else
	{
		matrix_4=matrix_R-powMatrix(matrix_psi,2);
	}
	CDoubleVector Vect_Gamma(nCol*nCol);//�ü���������������������Ҳ���ڴ洢���������Խ���Ԫ��
	assert(Vect_Gamma.vlen()>0);

	//��matrix_4�����ݸ��Ƶ�pGamma�ṹ��
	int i=0;
	for(i=0;i<nCol;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			Vect_Gamma[i*nCol+j]=matrix_4(i,j);
		}
	}
	CDoubleVector Vect_Omiga(nCol*nCol);//�洢matrix_4����������
	cjcbj(Vect_Gamma,nCol,Vect_Omiga,EPS);
	if(nMethod==0||nMethod==1)
		SortEigenVect(Vect_Gamma,Vect_Omiga,nCol,nCol,0);//С��������
	else
		SortEigenVect(Vect_Gamma,Vect_Omiga,nCol,nCol,1);//��С����
	for(i=0;i<nCol;i++)
	{
		vector_gamma(i)=Vect_Gamma[i*nCol+i];
	}
	for(i=0;i<nCol;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			matrix_omiga(i,j)=Vect_Omiga[i*nCol+j];
		}
	}


	int p=nCol;
	
	/////////����Ŀ�꺯��ֵf0
	double f0=0;
	int k=0;
	for(k=m;k<p;k++)
	{
		if(nMethod==0)
		{
			f0=f0+log(vector_gamma(k))+1.0/vector_gamma(k)-1;
		}
		else if(nMethod==1)
		{
			f0=f0+(vector_gamma(k)-1)*(vector_gamma(k)-1)/2.0;
		}
		else
		{
			f0=f0+vector_gamma(k)*vector_gamma(k)/2.0;
		}
	}
	return f0;

}

//*******************************************************************//
//Author:zkc
//Date:2003.4.11
//Functionality:��������һ����ֵ.
//********************************************************************//
CDoubleVector CFactor::VecDiv(CDoubleVector vect,double val)
{
	CDoubleVector temp(vect.vlen());
	for(int i=0;i<vect.vlen();i++)
	{
		temp(i)=vect(i)/val;
	}
	return temp;
}

//*******************************************************************//
//Author:zkc
//Date:2003.4.10
//Functionality:��f>f0ʱ������vector_x,matrix_psi,vector_gamma,matrix_omiga
//********************************************************************//
bool  CFactor::ChangeX(CDoubleVector &vector_x,		//nCol
			  CDoubleVector vector_d,		//nCol
			  CDoubleVector vector_oldx,	//nCol
			  CDoubleMatrix matrix_R,		//���ϵ������,nColxnCol
			  CDoubleMatrix matrix_r,		//matrix_R������,nColxnCol
			  CDoubleMatrix &matrix_psi,	//nCol*nCol
			  CDoubleVector &vector_gamma,	//nCol
			  CDoubleMatrix &matrix_omiga,	//nCol*nCol
			  int nMethod,					//0-������Ȼ����1-��ͨ��С���˷���2-��Ȩ����С���˷�
			  int nCol,						//p=nCol
			  double f0,					//f0ֵ
			  double &fOrg,
			  int &m)						//��ȡ���ӵĸ���
{
	int id=0;
	CDoubleVector vector_x1=vector_oldx;
	double f=0;
	CDoubleMatrix matrix_psi1(nCol,nCol);
	CDoubleMatrix matrix_omiga1(nCol,nCol);
	CDoubleVector vector_gamma1(nCol);
	int m1=m;
	int dom=1;
	while(id<ID)
	{
		dom=2*dom;
		vector_x1=vector_oldx-VecDiv(vector_d,dom);
		f=CalcFx(vector_x1,matrix_R,matrix_r,matrix_psi1,vector_gamma1,matrix_omiga1,nMethod,nCol,m1);
		if(f<f0)
			break;	
		id++;
	}
	if(id<ID)
	{
		vector_x=vector_x1;
		vector_gamma=vector_gamma1;
		matrix_omiga=matrix_omiga1;
		matrix_psi=matrix_psi1;
		m=m1;
		fOrg=f;
		return true;
	}
	else
	{
		if(nMethod==2)
		{
			int dom=1;
			id=0;
			while(id<ID)
			{
				dom=2*dom;
				vector_x1=vector_oldx+VecDiv(vector_d,dom);
				f=CalcFx(vector_x1,matrix_R,matrix_r,matrix_psi1,vector_gamma1,matrix_omiga1,nMethod,nCol,m1);
				if(f<f0)
					break;	
				id++;
			}
			if(id<ID)
			{
				vector_x=vector_x1;
				vector_gamma=vector_gamma1;
				matrix_omiga=matrix_omiga1;
				matrix_psi=matrix_psi1;
				m=m1;
				fOrg=f;
				return true;
			}
			return false;
		}
	}
	return false;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.10
//Functionality:���㹲�Է���,ͬʱ����ʽ��12������13������matrix_GAMMA��matrix_OMIGA
//********************************************************************//

CDoubleVector CFactor::CalcCommunality(CDoubleMatrix matrix_omiga,
							  CDoubleVector vector_gamma,
							  CDoubleMatrix matrix_psi,
							  CDoubleMatrix &matrix_OMIGA,
							  CDoubleMatrix &matrix_GAMMA,
							  int p,int m,
							  int nMethod)
{
	CDoubleMatrix matrix_GAMMA1(m,m,0.0);
	matrix_GAMMA=matrix_GAMMA1;
	CDoubleMatrix matrix_I(m,m,0.0);
	CDoubleMatrix matrix_I1(p,p,0.0);
	int i=0;
	for(i=0;i<p;i++)
	{
		if(i<m)
		{
			matrix_GAMMA(i,i)=fabs(vector_gamma(i));
			matrix_I(i,i)=1;
		}
		matrix_I1(i,i)=1;

	}
	CDoubleMatrix matrix_OMIGA1(p,m);
	matrix_OMIGA=matrix_OMIGA1;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<m;j++)
		{
			matrix_OMIGA(i,j)=matrix_omiga(i,j);
		}
	}
	int nCol=m;
	if(nMethod==0)
		nCol=m;
	else if(nMethod==1)
		nCol=p;
	else
		nCol=m;
		
	CDoubleMatrix matrix_communality(p,nCol);//���Է������
	CDoubleVector vector_cummunality(p,0);//
	if(nMethod==0)
	{
		matrix_communality=powMatrix((matrix_psi*matrix_OMIGA),2)*(pinv(matrix_GAMMA)-matrix_I);
	}
	else if(nMethod==1)
	{
		CDoubleMatrix tempMatrix=diag(vector_gamma);
		matrix_communality=powMatrix((matrix_psi*matrix_omiga),2)*(pinv(tempMatrix)-matrix_I1);
	}
	else
	{
		matrix_communality=powMatrix(matrix_OMIGA,2)*matrix_GAMMA;
	}
	for(i=0;i<p;i++)
	{
		for(int j=0;j<matrix_communality.mcol();j++)
		{
			vector_cummunality(i)+=matrix_communality(i,j);
		}
	}
	return vector_cummunality;

}


//////////////////////////////////////////////////////////
//    ML��GLS��ULS���ӷ���       
//////////////////////////////////////////////////////////
void CFactor::ml_fact(CDoubleMatrix matrix,		//ԭʼ����
					  int nMethod,				//0--������Ȼ����1--��ͨ��С���˷���2--��Ȩ����С���˷�
					  int m,					//��ȡ���ӵĸ���
					  int ncount,				//����������
					  int ID)
{
//	CreateEig(0);
	int nRow = matrix.mrow();
	int nCol  = matrix.mcol();
	////////////////////////    2003.4.7   //////////////////////////////////////

	CDoubleVector Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight);  //calculate relation matrix

	CDoubleMatrix matrix_R= VectorToMatrix(Vect_Data,nCol,nCol);/////���ϵ������

	assert(Vect_Data.vlen()>0);
	m_EigenVector.create( m_nCol*m_nCol);//�������������ռ�
	cjcbj(Vect_Data,m_nCol,m_EigenVector,0.000001);//����������������ֵ
	SortEigenVect(Vect_Data,m_EigenVector,m_nCol,m_nCol,1);//��������������������������ֵ��С��������
//	CreateEigenValueResult(pData,m_nCol);//������������ۼƹ�����
//	WriteData(m_pEigenVector,m_nCol,m_nCol,"after");

	int p=nCol;
	//���ݹ����ʾ�����ȡ���Ӹ���
	if(m_nExtractStandard==0 && m_bIsMValued==false)
	{
		CDoubleVector vector_gamma(m_nCol);
		int i=0;
		for(i=0;i<m_nCol;i++)
		{
			vector_gamma(i)=Vect_Data[i*m_nCol+i];
		}
		double dRatio=m_fPro;//to define the contrubution ratio.
		double a=0;
		double b=sum(vector_gamma);
		for(i=0;i<p;i++)
		{
			a=a+vector_gamma(i);
			if(a/b>=dRatio)
			{
				m=i+1;
				break;
			}
		}
		m_bIsMValued=true;
	}
	CDoubleMatrix matrix_r = pinv(matrix_R);////���ϵ�����������
	CDoubleVector vector_CommunIntial(p,0);//���Է������ĳ�ʼֵ
	CDoubleVector vector_EigenIntial(p,0);//���������ۼƹ����ʵĳ�ʼֵ
	int i=0;
	for(i=0;i<p;i++)
	{
		vector_CommunIntial(i)=1.0-1/matrix_r(i,i);
		vector_EigenIntial(i)=Vect_Data[i*nCol+i];
	}
	////////formula (1)
	CDoubleVector vector_x(nCol);
	for(i=0;i<nCol;i++)
	{
		if(nMethod==0||nMethod==1)
		{
			vector_x(i)=log(fabs((1-1.0*m/2/p)/matrix_r(i,i)));
		}
		else
		{
			vector_x(i)=sqrt(fabs((1-1.0*m/2/p)/matrix_r(i,i)));
		}		
	}
	CDoubleMatrix matrix_psi(nCol,nCol);
	CDoubleMatrix matrix_omiga(nCol,nCol);
	CDoubleVector vector_gamma(nCol);
	double f0=CalcFx(vector_x,matrix_R,matrix_r,matrix_psi,vector_gamma,matrix_omiga,nMethod,nCol,m);
	CDoubleMatrix matrix_OMIGA,matrix_GAMMA;//ά����С�ں������ڶ���


	CDoubleVector vector_d(p,1);
	int s=1;
	CDoubleVector vector_h(p);
	CDoubleMatrix matrix_H(p,p);	
	CDoubleVector vector_oldx;
	double f=0;
	int flag=0;
	while((abs(vector_d).Max()>=0.0001)&&(s<350))
	{
		vector_h=Ladder1(vector_x,vector_gamma,matrix_omiga,nMethod,p,m,nMethod);//formula (6)
		if(flag==0)
			matrix_H=Ladder2(vector_x,matrix_omiga,m,p,nMethod);
		else
			matrix_H=Ladder2(vector_gamma,vector_x,matrix_omiga,matrix_r,vector_h,nMethod,p,m);//formula (7)
		flag++;		
		ChangeLadder2(matrix_H);
		if(!IsNormal(matrix_H))//H������
		{	
			//formula (8)
			matrix_H=Ladder2(vector_x,matrix_omiga,m,p,nMethod);
			ChangeLadder2(matrix_H);
			if(!IsNormal(matrix_H))
				break;
			else
				vector_d=pinv(matrix_H)*vector_h;
		}
		else
		{
			vector_d=pinv(matrix_H)*vector_h;//formula (9)
			if(IsDLegal(vector_d))//vector_d��ÿһ��ֵ������0.1
			{
				matrix_H=Ladder2(vector_x,matrix_omiga,m,p,nMethod);//formula (8)
				ChangeLadder2(matrix_H);
				if(!IsNormal(matrix_H))
					break;
				else
					vector_d=pinv(matrix_H)*vector_h;//formula(9)
			}
		}
		vector_oldx=copy(vector_x);
		vector_x=vector_x-vector_d;	
		f=CalcFx(vector_x,matrix_R,matrix_r,matrix_psi,vector_gamma,matrix_omiga,nMethod,nCol,m);
		if(f>f0)
		{
			if(!ChangeX(vector_x,vector_d,vector_oldx,matrix_R,
					matrix_r,matrix_psi,vector_gamma,matrix_omiga,nMethod,nCol,f0,f,m))
			{
				break;
			}
		}
		f0=f;
		s=s+1;
	}

	
	CDoubleMatrix matrix_I(m,m,0.0);
	for(i=0;i<m;i++)
	{
		matrix_I(i,i)=1;
	}

	CDoubleVector vector_CommunLast=CalcCommunality(matrix_omiga,vector_gamma,matrix_psi,matrix_OMIGA,matrix_GAMMA,p,m,nMethod);//���Է�������������ȡֵ
	CDoubleMatrix matrix_LAMDA(p,m);//�����غɾ���
	if(nMethod==0||nMethod==1)
	{
		matrix_LAMDA=matrix_psi*matrix_OMIGA*sqrt(pinv(matrix_GAMMA)-matrix_I);
	}
	else
	{
		matrix_LAMDA=matrix_OMIGA*sqrt(matrix_GAMMA);
	}
	double chi=0,df=0,P=0;
	if(nMethod==0||nMethod==1)
	{
		chi=(sum(m_weight)-1-(2.0*p+5)/6-2*m/3)*f;//��Ȩ�����
		df=((p-m)*(p-m)-p-m)/2;
		if(df<1)
			df=1;
		P=1-chi2cdf(chi,df);
	}
	m_nCol=p;
	m_nSel=m;
	m_VectComMatrix.create( m_nCol*m_nSel);
	for(i=0;i<m_nSel;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			m_VectComMatrix[i*m_nCol+j]=matrix_LAMDA(j,i);
		}
	}
	////TraceVector(m_VectComMatrix);
	CDoubleVector vector_EigenLast(p,0);//���������ۼƹ����ʵ�����ֵ
	
	
	CreateResultForMl_fact(matrix_LAMDA,vector_CommunLast,vector_CommunIntial,vector_EigenIntial,vector_EigenLast,nMethod,m,df,P,chi);
	vector_EigenIntial.Sort();
	CreateChartResult(vector_EigenIntial,p);
}

void CFactor::CreateResultForMl_fact(CDoubleMatrix matrix_LAMDA,		//�����غɾ���
								     CDoubleVector vector_CommunLast,	//���Է����ʼֵ
								     CDoubleVector vector_CommunIntial,	//���Է�����ȡֵ
								     CDoubleVector vector_EigenIntial,	//��������ʼֵ
									 CDoubleVector vector_EigenLast,	//����������ֵ
									 int nMethod,						//0--�����Ȼ����1--һ����С���˷���2--��Ȩ����С���˷�
									 int m,								//��ȡ������
									 int df,							//���ɶ�
									 double P,							//������
									 double chi)						//����ֵ
{



	CTString strExplain;


	int nLen=vector_CommunLast.vlen();
	double sum1 = 0;
	double sum2=0;
	int i=0;
	for(i=0;i<nLen;i++)
	{
		sum1+= vector_EigenIntial(i);
		sum2+= vector_EigenLast(i);
	}
	//���������ۼƹ����ʳ�ʼֵ���
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("���������ۼƹ����ʳ�ʼֵ");
//	pTable->SetCols(2);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"����");
	pTable->InsertColumn(1,"������");
	pTable->InsertColumn(2,"�������%");
	pTable->InsertColumn(3,"�ۼƹ�����%");
	double sumpro = 0;
	CTString strAdd;
	for(i=0;i<nLen ;i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str,false);
		pTable->SetItemText(i,1,vector_EigenIntial[i]);
		sumpro += vector_EigenIntial[i] / sum1 ;
		pTable->SetItemText(i,2,100 * vector_EigenIntial[i] / sum1);
		pTable->SetItemText(i,3,100 * sumpro);
		if(i<m)
		{
			strExplain.Format("��%d������������ֵΪ%.4f,����������Ϣ����%.2f%%��\n",i+1,vector_EigenIntial[i],100 * vector_EigenIntial[i] / sum1);
			strAdd+=strExplain;
		}		
	}
	CRsltElementTable * pETable= new CRsltElementTable("���������ۼƹ����ʳ�ʼֵ",pTable);
	m_pResult->Add(pETable);
	m_strResultExplain.Add(strAdd);

	///zkc 2003.10.23 �����������
	pTable = new CTLTable;
	pTable->SetTitle("��������");
//	pTable->SetCols(nLen+1);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"");	
	for(i=0;i<nLen;i++)
	{
		CTString colHeader;
		colHeader.Format("��������%d",i+1);
		pTable->InsertColumn(i+1,colHeader);	
		
	}
	for(i=0;i<nLen ;i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str,false);
		for(int j=0;j<nLen;j++)
		{
			pTable->SetItemText(i,j+1,m_EigenVector[i*nLen+j]);
		}
	}
	pETable= new CRsltElementTable("��������",pTable);
	m_pResult->Add(pETable);	


	///���Է������
	pTable = new CTLTable;
	pTable->SetTitle("���Է���");
//	pTable->SetCols(2);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"��ʼֵ");
	pTable->InsertColumn(2,"��ȡֵ");

	//zkc 2003.8.19
	double min=vector_CommunLast[0];
	double max=vector_CommunLast[0];
	int minIndex=0;
	int maxIndex=0;
	for(i=0;i<nLen ;i++)
	{
		CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
		pTable->InsertItem(i,str,false);
		pTable->SetItemText(i,1,vector_CommunIntial[i]);
		pTable->SetItemText(i,2,vector_CommunLast[i]);
		if(min>vector_CommunLast[i])
		{
			min=vector_CommunLast[i];
			minIndex=i;
		}
		if(max<vector_CommunLast[i])
		{
			max=vector_CommunLast[i];
			maxIndex=i;
		}

	}
	pETable= new CRsltElementTable("���Է���",pTable);
	m_pResult->Add(pETable);

	strExplain.Format("%s��ȡ����Ϣ���֣�Ϊ%.4f��%s��ȡ����Ϣ���֣�Ϊ%.4f��",
					m_pDataInterface->GetFieldName(m_tVarInfo.pValue[maxIndex]).GetData(),max,
					m_pDataInterface->GetFieldName(m_tVarInfo.pValue[minIndex]).GetData(),min);	
	m_strResultExplain.Add(strExplain);
	if(nMethod==0||nMethod==1)
	{
		pTable = new CTLTable;
		pTable->SetTitle("�����Լ���");
//		pTable->SetCols(3);
//		pTable->SetRows(2);

		pTable->InsertColumn(0,"����ֵ");
		pTable->InsertColumn(1,"���ɶ�");
		pTable->InsertColumn(2,"������");
		CTString str;
		str.Format("%.4f",chi);
		pTable->InsertItem(0,str,false);
		pTable->SetItemText(0,1,df);	
		pTable->SetItemText(0,2,P);
		pETable= new CRsltElementTable("�����Լ���",pTable);
		m_pResult->Add(pETable);
	}

}



void CFactor::PSI(CDoubleVector vector_x,CDoubleMatrix matrix_R,const int nMethod,CDoubleMatrix &matrix_l,CDoubleMatrix &matrix_lambda)
{
	int p = matrix_R.mrow();
	CDoubleMatrix matrix_r = pinv(matrix_R);
	CDoubleMatrix matrix_psi = diag( exp( vector_x ) );

	CDoubleMatrix matrix_psirpsi;
	switch( nMethod )
	{
	case 0:
	case 1:
		matrix_psirpsi = matrix_psi * matrix_r * matrix_psi;
		break;
	case 2:
		matrix_psirpsi = matrix_R - matrix_psi * matrix_psi;
		break;
	}
	matrix_l = (CDoubleMatrix) eig( (CSymDbMatrix) matrix_psirpsi,matrix_lambda);	
	switch( nMethod )
	{
	case 0:
	case 1:
		SortEigenVect(matrix_l,matrix_lambda,1);		
	case 2:
		SortEigenVect(matrix_l,matrix_lambda,0);
		break;
	}
}


CDoubleVector CFactor::DC(const int m,CDoubleMatrix matrix_l,CDoubleMatrix matrix_lambda,const int s,CDoubleVector vector_d,const int nMethod,CDoubleVector vector_x,CDoubleMatrix matrix_r)
{
	int p = matrix_l.mrow();
	int i=0,j=0,k=0;
	CDoubleVector vector_h( p );
	switch( nMethod )
	{
	case 0:
		for(i=0;i<p;i++)
		{
			vector_h[i] = 0;
			for(k=m;k<p;k++)
			{
				vector_h[i] += (1-(1.0/matrix_lambda(k,k))) * matrix_l(i,k) * matrix_l(i,k);
			}
		}
		break;
	case 1:
		for(i=0;i<p;i++)
		{
			vector_h[i] = 0;
			for(k=m;k<p;k++)
			{
				vector_h[i] += matrix_lambda(k,k) * matrix_lambda(k,k) - matrix_lambda(k,k) * matrix_l(k,k) * matrix_l(i,k);
			}
		}
		break;
	case 2:
		for(i=0;i<p;i++)
		{
			vector_h[i] = 0;
			for(k=m;k<p;k++)
			{
				vector_h[i] += matrix_lambda(k,k) * matrix_l(i,k) * matrix_l(i,k);
			}
			vector_h[i] *= 2 * vector_x[i];
		}
	}

	CDoubleMatrix matrix_H;

	switch( s )
	{
	case 1:
		matrix_H = HC(vector_h,m,matrix_l,matrix_lambda,1,nMethod,vector_x,matrix_r);
		break;
	default:
		if( abs(vector_d).Min() > 0.1 )
		{
			matrix_H = HC(vector_h,m,matrix_l,matrix_lambda,1,nMethod,vector_x,matrix_r);
		}
		else
		{
			matrix_H = HC(vector_h,m,matrix_l,matrix_lambda,2,nMethod,vector_x,matrix_r);
		}
		break;		
	}


	vector_d = MatrixToVector ((CDoubleMatrix)(pinv( matrix_H ) * VectorToMatrix(vector_h)));

	
	if( norm( vector_d ) > norm( vector_x ) )
	{
		vector_d = vector_d / ( norm( vector_d ) / sqrt( p ) ) * ( norm(vector_x) / sqrt( p ) );
	}


	return vector_d;
}



CDoubleMatrix CFactor::HC(CDoubleVector vector_h,const int m,CDoubleMatrix matrix_l,CDoubleMatrix matrix_lambda,int nWay,const int nMethod,CDoubleVector vector_x,CDoubleMatrix matrix_r)
{
	int p = matrix_l.mrow();
	int nout(0);
	int i=0,j=0,k=0,n=0;
	CDoubleMatrix matrix_H(p,p);
	CDoubleMatrix delta(p,p);
	while( nout == 0 )
	{
		switch(nWay)
		{
		case 1:
			{
				switch( nMethod )
				{
				case 0:
				case 1:
					for(i=0;i<p;i++)
					{
						for(j=0;j<p;j++)
						{
							matrix_H(i,j) = 0;
							for(k=m;k<p;k++)
							{
								matrix_H(i,j) += matrix_l(i,k) * matrix_l(j,k);
							}
							matrix_H(i,j) *= matrix_H(i,j);
						}					
					}
					break;
				case 2:
					for(i=0;i<p;i++)
					{
						for(j=0;j<p;j++)
						{
							matrix_H(i,j) = 0;
							for(k=m;k<m;k++)
							{
								matrix_H(i,j) += matrix_l(i,k) * matrix_l(j,k);
							}
							matrix_H(i,j) = 4 * vector_x[i] * vector_x[j] * matrix_H(i,j) * matrix_H(i,j);
						}
					}
					break;
				}
				
			}
			nout = 1;
			break;
		case 2:
			switch( nMethod )
			{
			case 0:
			case 1:
				for(i=0;i<p;i++)
				{
					for(j=0;j<p;j++)
					{
						delta(i,j) = 0;
						if( i == j )
						{
							delta(i,j) = 1;
						}
						matrix_H(i,j) = delta(i,j) * vector_h[i];
						switch( nMethod )
						{
						case 0:
							for(k=m;k<p;k++)
							{
								double temp = 0;
								for(n=0;n<m;n++)
								{
									temp =( matrix_lambda(k,k) + matrix_lambda(n,n) - 2 ) / ( matrix_lambda(k,k) - matrix_lambda(n,n) ) * matrix_l(i,n) * matrix_l(j,n);
								}
								temp += delta(i,j);
								matrix_H(i,j) += matrix_l(i,k) * matrix_l(j,k) * temp;
							}
							break;
						case 1:
							for(k=m;k<p;k++)
							{
								double temp = 0;
								for(n=0;n<m;n++)
								{
									temp = matrix_lambda(n,n) * ( matrix_lambda(k,k) + matrix_lambda(n,n) - 2.0 ) / ( matrix_lambda(k,k) - matrix_lambda(n,n) ) * matrix_l(i,n) * matrix_l(j,n);
								}
								temp += matrix_r(i,i) * exp( ( vector_x[i] + vector_x[j] ) / 2 );
								matrix_H(i,j) += matrix_lambda(k,k) * matrix_l(i,k) * matrix_l(j,k) * temp;
							}
							break;
						}
					}
				}
				break;
			case 2:
				{
					for(i=0;i<p;i++)
					{
						for(j=0;j<p;j++)
						{
							delta(i,j) = 0;
							if( i == j )
							{
								delta(i,j) = 1;
							}
							double temp1(0);
							double temp2(0);
							for(k=m;k<p;k++)
							{
								for(n=0;n<m;n++)
								{
									temp1 += (matrix_lambda(k,k) * matrix_lambda(n,n)) / ( matrix_lambda(k,k) - matrix_lambda(n,n) ) * matrix_l(i,k) * matrix_l(j,k);
								}
								temp1 = temp1 + matrix_l(i,k) * matrix_l(j,k) * temp1;
								temp2 = temp2 + ( vector_x[i] * vector_x[i] - matrix_lambda(k,k) / 2 ) * matrix_l(i,k) * matrix_l(i,k);
							}
							matrix_H(i,j) = 4 * ( vector_x[i] * vector_x[j] * temp1 + delta(i,j) * temp2);
						}
					}
					
				}
				break;
			}
			break;
		}
		for(i=0;i<p;i++)
		{
			if( matrix_H(i,i) < 0.05 )
			{
				for(j=0;j<p;j++)
				{
					matrix_H(j,i) = 0;
					matrix_H(i,j) = 0;
				}
				matrix_H(i,i) = 1;
			}
		}
		
		if( nWay == 2 && Definite(matrix_H) == 1 )
			nout = 1;
		else
			nWay = 1;
	}
	return matrix_H;
}


int CFactor::Definite(CDoubleMatrix matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();

	if( nRow != nCol)
	{
// 		AfxMessageBox("Error!");
		return 0;
	}

	CDoubleMatrix matrix_tran = trans( matrix );
	int i=0;
	for(i=0;i<nRow;i++)
	{
		for(int j=0;j<nRow;j++)
		{
			if( !IsZero(matrix_tran(i,j) - matrix(i,j)) )
				return 0;			
		}
	}
	
	for(i=0;i<nRow;i++)
	{
		if(  det( (CDoubleMatrix)slice(matrix,0,0,i+1,i+1) ) <= 0 )
		{
			return 0;
		}
	}

	return 1;
}

//nIndex 1:Э����;0:���ϵ��
void CFactor::CreateEig(int nIndex)
{
	CDoubleVector Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,nIndex);
	
	assert(Vect_Data.vlen()>0);
	m_EigenVector.create(m_nCol*m_nCol);
	cjcbj(Vect_Data,m_nCol,m_EigenVector,0.000001);
	SortEigenVect(Vect_Data,m_EigenVector,m_nCol,m_nCol,1);
	CreateEigenValueResult(Vect_Data,m_nCol);
	Vect_Data.destruction();
	
}

double CFactor::PCorrelation(const int x1, const int x2, CDoubleVector Vect_R)
{
	int k = m_nCol;				   //x������
	int j=0;
	CIntVector Vect_Pos(k);
	Vect_Pos[0] = x1;
	Vect_Pos[1] = x2;
	for(int i=0;i<k;i++)
	{
		if( i != x1 && i != x2 )
		{
			Vect_Pos[j+2] = i;
			j++;
		}
	}
	////TraceVector(Vect_R);

	////TraceVector(Vect_Pos);
	double r = partial(Vect_R,Vect_Pos,k,k);	
	
	return r;
}

//�б�׼����

CDoubleVector  CFactor::FormlizeEx(CDoubleVector Vect_Data, CDoubleVector weight)
{
	//Ȩ�غ�
	double w_sum(0);
	int i=0;
	for(i=0;i<weight.vlen();i++)
	{
		w_sum += weight(i);		
	}
	CDoubleVector vector_z(m_nCol);
	//�о�
	for(i=0;i<m_nCol;i++)
	{
		double sum(0);
		for(int j=0;j<m_nRow;j++)
		{
			sum += Vect_Data[i*m_nRow+j] * weight(j);
		}
		vector_z(i) = sum / w_sum;
	}
	////TraceVector(vector_z);
	//�з���
	CDoubleVector vector_s2(m_nCol);
	for(i=0;i<m_nCol;i++)
	{
		double sum(0);
		for(int j=0;j<m_nRow;j++)
		{
			sum += weight(j) * ( Vect_Data[i*m_nRow + j] - vector_z(i) ) * ( Vect_Data[i*m_nRow + j] - vector_z(i) );
		}
		vector_s2(i) = sum / ( w_sum - 1 );
	}
	////TraceVector(vector_s2);
	//�б�׼��
	CDoubleVector Vect_X(m_nRow * m_nCol);
	for(i=0;i<m_nCol;i++)
	{
		for(int j=0;j<m_nRow;j++)
		{
			Vect_X[i*m_nRow+j] = (Vect_Data[i*m_nRow+j] - vector_z(i)) / sqrt(fabs(vector_s2(i))) ;
		}
	}
	return Vect_X;
}
//��� ���ϵ������ �� Э�������.
CDoubleVector CFactor::RelationMatrixEx(CDoubleVector Vect_Data,int nRow,int nCol,CDoubleVector weight,int nIndex)
{
	//������
	CDoubleMatrix C(nCol,nCol);
	CDoubleMatrix CC(nCol,nCol);
	CDoubleVector Vect_Relation ( nCol * nCol );
	double w_sum(0);
	int i=0;
	for(i=0;i<weight.vlen();i++)
	{
		w_sum += weight(i);		
	}
	for(i=0;i<nCol;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			if( j >= i )
			{
				double sum(0);
				double sum1(0);
				double sum2(0);

				double sum_c(0);
				double sum_c1(0);
				double sum_c2(0);
				
				double sum_d(0);
				double sum_d1(0);
				double sum_d2(0);

				double sum_weight = 0;

				for(int k=0;k<nRow;k++)
				{
					bool bInput(false);
					switch(m_nMissingMethod)
					{
					case 0:
						{
							bInput = true;						
						}
						break;
					case 1:
						{
							if( !IsMissing(k,i) && !IsMissing(k,j) )
							{
								bInput = true;							
							}					
						}
						break;
					case 2:
						{
							bInput = true;							
						}
						break;
					}
					if( bInput )
					{
						sum  += weight(k) * Vect_Data[i*nRow+k]* Vect_Data[j*nRow+k];
						sum1 += weight(k) * Vect_Data[i*nRow+k];
						sum2 += weight(k) * Vect_Data[j*nRow+k];
						
						sum_c  += weight(k) * Vect_Data[i*nRow+k]* Vect_Data[i*nRow+k];
						sum_c1 += weight(k) * Vect_Data[i*nRow+k];
						sum_c2 += weight(k) * Vect_Data[i*nRow+k];
						
						sum_d  += weight(k) * Vect_Data[j*nRow+k]* Vect_Data[j*nRow+k];
						sum_d1 += weight(k) * Vect_Data[j*nRow+k];
						sum_d2 += weight(k) * Vect_Data[j*nRow+k];
						
						sum_weight += weight(k);
					}
				}
				C(i,j) = sum - ( sum1 * sum2 / sum_weight ) ;
				CC(i,j) = sum_c - ( sum_c1 * sum_c2 / sum_weight ) ;
				CC(j,i) = sum_d - ( sum_d1 * sum_d2 / sum_weight ) ;
				double dTemp;
				switch( nIndex )
				{
				case 0:
					//���ϵ��
					dTemp = sqrt(fabs(CC(i,j) * CC(j,i)));
					Vect_Relation[i*nCol+j] = C(i,j) / dTemp;
					break;
				case 1:
					//Э����
					Vect_Relation[i*nCol+j] = C(i,j) / (sum_weight - 1);
					break;
				}				
			}
			else if ( i == j && nIndex == 0 )
			{
				 Vect_Relation[i*nCol+j] = 1;			
			}
			else if( i > j )
			{
				Vect_Relation[i*nCol+j] = Vect_Relation[j*nCol+i] ;
			}
		}	
	}

	m_VectCorrelation.create(nCol*nCol);
    m_VectCorrelation = copy(Vect_Relation);
	//TraceVector(m_VectCorrelation);
	return Vect_Relation;
}








// ���ϵ�� �� Э����
CDoubleMatrix CFactor::RelationMatrixEx(CDoubleMatrix matrix, int nIndex)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	CDoubleVector Vect_Data(nRow * nCol);
//	Vect_Data << matrix;
	Vect_Data = MatrixToVector( matrix);
	CDoubleVector Vect_Relation = RelationMatrixEx(Vect_Data,nRow,nCol,m_weight,nIndex);
	CDoubleMatrix matrix_R=VectorToMatrix(Vect_Relation,nCol,nCol);
	return matrix_R;
}


bool CFactor::IsMissing(int nRow, int nCol)
{
	nRow++;
	int ncount = m_pDataInterface->GetRowCount();
	long nt = m_pDataInterface->m_DataAccess.GetRecNo();
	m_pDataInterface->m_DataAccess.SetCurRecNo(nRow);
	CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nCol);
	bool btemp = pField->IsNull();
	m_pDataInterface->m_DataAccess.SetCurRecNo(nt);
	return btemp;
}

bool CFactor::IsMissing(int nRow)
{
	nRow++;

	m_pDataInterface->m_DataAccess.SetCurRecNo(nRow);

	int nweightcol = m_pDataInterface->GetWeightNumb();

	if(m_nProcType)
	{
		for(int i=0;i<m_tVarInfo.iCount;i++)
		{
			int nt = m_tVarInfo.pValue[i];
			CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nt);
			if( pField->IsNull() )
			{
				return true;
			}
			if( nweightcol >= 0 )
			{
				 pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nweightcol);
				 if( pField->IsNull() )
				 {
					 return true;
				 }
			}
		}
	}
	else
	{
		for(int i=0;i<m_tVarInfo2.iCount;i++)
		{
			int nt = m_tVarInfo2.pValue[i];
			CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nt);
			if( pField->IsNull() )
			{
				return true;
			}
			if( nweightcol >= 0 )
			{
				 pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nweightcol);
				 if( pField->IsNull() )
				 {
					 return true;
				 }
			}
		}
	}
	return false;
}

/************************************************************************************
�������ƣ�

����˵����
	����
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2003-1-25
�޸�:

************************************************************************************/
CDoubleVector  CFactor::GetData(CDataInterface *pDataInterface,TIntStruct tVarInfo,int bRemoveEqual)
{
		m_arrRecNo.RemoveAll();
        CDoubleVector Vect_Data;
        //ȡ�����ݣ���֯�������ݾ���
        int nCaseNumb,m = tVarInfo.iCount +1;
        TDColAry * TDataArray=NULL;
        int nColNum=0;
        //���ݼ���
		//{{{START 2003-1-25 Eric
		//ָʾ���Ƿ���ͬ�ı���
		CTArray<double,double>  FirstValueArray;
		bool bHasValue = false;
		CTArray<int,int>  IsSameArray;  
		
		//��������
		int nWeightCol = pDataInterface->GetWeightNumb();
		CDataInterface *pTempDataInterface = new CDataInterface();
		CFieldList * pTempFieldList = pDataInterface->m_DataAccess .m_pFieldList;

		int i = 0;
		for(i = 0;i<pTempFieldList ->GetFieldCount();i++)
		{
			CField* pField = pTempDataInterface->m_DataAccess .m_pFieldList->CreateField(pTempFieldList->FieldByIndex(i));
			pField->SetFieldName(pTempFieldList->FieldByIndex(i)->GetFieldName());
			pTempDataInterface->m_DataAccess .m_pFieldList->Add(pField);
			FirstValueArray.Add(0);
			IsSameArray.Add(true);
		}

		//��û��NULLֵ���е���. 
		pDataInterface->m_DataAccess .First();
		while(!pDataInterface->m_DataAccess .Eof())
		{
			bool bAppend = true;
/*			for(int j = 0;j<pDataInterface->GetColCount();j++)
			{
				if(pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j)->IsNull())
				{
					bAppend = false;
					break;
				}
			}
*/
			//�ǿ�,�ŵ���
			int j = 0;
			for(j = 0;j<tVarInfo.iCount;j++)
			{
				if(pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex
					(tVarInfo.pValue[j])->IsNull())
				{
					bAppend = false;
					break;
				}
			}

			//Ȩ��Ϊ����,�ŵ���.
			CField * pField = pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nWeightCol) ;
			if((nWeightCol>=0) && (pField->IsNull() || pField->GetAsDouble()<= 0))
			{
					bAppend = false;
			}

			if(bAppend)
			{
				pTempDataInterface->m_DataAccess.Append();

				m_arrRecNo.Add(pDataInterface->m_DataAccess.GetRecNo());

				int j = 0;
				for(j = 0;j<pDataInterface->GetColCount();j++)
				{
					if(pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j)->IsNull())
					{
						pTempDataInterface->m_DataAccess .m_pFieldList->FieldByIndex(j)->SetNull();
					}
					else
					{
						pTempDataInterface->m_DataAccess .m_pFieldList->FieldByIndex(j)->SetAsDouble(
							pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j)->GetAsDouble());
					}
				}
				pTempDataInterface->m_DataAccess.Post();

				//�Ƿ���ֵ��ͬ
				if(!bHasValue)
				{
					for(int j = 0;j<tVarInfo.iCount;j++)
					{
						FirstValueArray[tVarInfo.pValue[j]] = pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(tVarInfo.pValue[j])->GetAsDouble();
					}
					bHasValue = true;
				}
				else
				{
					for(int j = 0;j<tVarInfo.iCount;j++)
					{
						if(fabs(FirstValueArray[tVarInfo.pValue[j]] 
							- pDataInterface->m_DataAccess.m_pFieldList->
							FieldByIndex(tVarInfo.pValue[j])->GetAsDouble()) > 1e-8
							)
						{//�����
							IsSameArray[tVarInfo.pValue[j]] = false;
						}
					}
				}

			}
			pDataInterface->m_DataAccess.Next();
		}	   

		nCaseNumb = pTempDataInterface->GetRowCount();
		if(nCaseNumb < 2)
		{
//             AfxMessageBox("������̫�٣��޷����з���!");
			delete pTempDataInterface;pTempDataInterface = NULL;
            return NULL;
		}

		//���Ƿ�����ͬ��
		int j = 0;
		for(j = 0;j<tVarInfo.iCount;j++)
		{
			if(IsSameArray[tVarInfo.pValue[j]] == true && tVarInfo.pValue[j] != nWeightCol)
			{
//                     AfxMessageBox("������һ�����ݷ���Ϊ�㣬�޷����з���!");
					delete pTempDataInterface; pTempDataInterface = NULL;
					return NULL;
			}
		}

		//}}}END  2003-1-25 Eric


		//{{{START 2003-1-26 Eric===================>
		/*
        for( i=0;i<tVarInfo.iCount;i++)
        {
                nColNum=tVarInfo.pValue[i];
                if( nColNum < 0 )
                {
                        AfxMessageBox("ȡ����ʱ����!");
						delete pTempDataInterface;
                        return NULL;
                }
                nCaseNumb = pTempDataInterface->GetColData(nColNum,pDataArray,1);//ȡ������
                assert(nCaseNumb > 0 );
		//{{{START 2003-1-25 Eric
                //if(bRemoveEqual && IsEqual(pDataArray,nCaseNumb))
                if(bRemoveEqual && IsEqual(pDataArray,nCaseNumb)&&(nWeightCol != nColNum))
		//}}}END  2003-1-25 Eric
                {
		//{{{START 2003-1-25 Eric
                    //��ʾ����������ͬ��Ӧ�ó�ȥ
                    //tVarInfo.RemoveAt(i);
                    //i--;
			//ĳ�е�ֵ��ͬ����!!
                    AfxMessageBox("�����������--�е�ֵ��ͬ!");
					delete pTempDataInterface;
					return NULL;
		//}}}END  2003-1-25 Eric
                }
        }
		*/
		//}}}END  2003-1-26 Eric<=====================

        m = tVarInfo.iCount;
		Vect_Data.create( pTempDataInterface->GetRowCount() * m );
        for(i=0;i<m;i++)
        {
            //if( !Vect_Data  )
                    //Vect_Data.create( pTempDataInterface->GetRowCount() * m );
            
            nColNum=tVarInfo.pValue[i];
            if( nColNum < 0 )
            {
//                     AfxMessageBox("ȡ����ʱ����!");
					delete pTempDataInterface; pTempDataInterface = NULL;
                    return NULL;
            }
			TDataArray = new TDColAry;
            nCaseNumb = pTempDataInterface->GetColData(nColNum,*TDataArray,1);//ȡ������
            assert(nCaseNumb > 0 );
            //��ֵ
            //�����������ݣ��������ֵ��ͬ���޳�
            for(int j=0;j<nCaseNumb;j++)
                    Vect_Data[i*nCaseNumb+j] = TDataArray->GetAt(j).dValue;
            if( TDataArray )
            {
                 delete TDataArray;
                 TDataArray=NULL;
            }
        }    
		//{{{START 2003-1-25 Eric
		/*
        if ( tVarInfo.iCount <= 0 )
        {
//                 AfxMessageBox("��ѡ���������Ч!");
                return NULL;
        }
		*/

		m_nRow = nCaseNumb;
		pTempDataInterface->SetWeightNumb(nWeightCol);
		m_weight = GetWeightVector(pTempDataInterface);//Ȩ��
		////TraceVector(m_weight);
		m_pDataInterface = pTempDataInterface;
		////TraceVector(Vect_Data);
        return Vect_Data;
}

/************************************************************************************
�������ƣ�

����˵����
   �������ɷ� 
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2003-2-9
�޸�:

************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////
bool CFactor::PrincipalComponent()
{	
	CDoubleVector Vect_Temp (m_nRow * m_nSel);

	//������Ӿ���End<<<<<<<<<<<<<<
	////TraceVector(m_VectDataFormlized);
	////TraceVector(m_SelEigenVector);
	MatrixMultiply(m_VectDataFormlized,m_SelEigenVector,m_nRow,m_nCol,m_nSel,Vect_Temp);
	m_VectScore = copy(Vect_Temp);
	//TraceVector(m_VectScore);
	return true;
}


/************************************************************************************
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2003-2-9
�޸�:modified by zkc 2003.9.8.��������ֱ�Ӽӵ���������С�

************************************************************************************/
void CFactor::PrincipalTable(CResult * pResult)
{
/*
	int i;
	CTLTable * pTable = NULL;
	pTable = new CTLTable();
	pTable->SetTitle("���ɷַ���");
	pTable->SetRows(m_nRow);
	pTable->SetCols(3);
	pTable->InsertColumn(0,"��¼��");
	pTable->InsertColumn(1,"Ȩ��");

	char szNum1[16];
	char szNum2[10];
	strcpy(szNum1,"���ɷ�");

	for(i = 0;i < m_nSel;i++)
	{
		pTable->InsertColumn(i + 2,strcat(szNum1,itoa(i+1,szNum2,10)));
		szNum1[6] = 0;
	}
	int nRecNo = 1;
	m_pOriDataInterface->m_DataAccess .First();
	for(i = 0;i<m_nRow;)
	{
		while(nRecNo < m_arrRecNo[i])
		{
			pTable->InsertItem(nRecNo,itoa(nRecNo,szNum2,10));
			pTable->SetItemText(nRecNo,1,0);
			
			for(int j = 0;j<=m_nSel;j++)
			{
				pTable->SetItemText(nRecNo,j+2,"ȱʧ");
			}
			m_pOriDataInterface->m_DataAccess.Next();
			nRecNo++;
		}
		
		pTable->InsertItem(nRecNo,itoa(nRecNo,szNum2,10));
		pTable->SetItemText(nRecNo,1,m_weight(i));
		for(int j = 0;j<m_nSel;j++)
		{
			pTable->SetItemText(nRecNo,j+2,m_pScore[i*m_nSel+j]);
		}
		i++;
		m_pOriDataInterface->m_DataAccess.Next();
		nRecNo++;
	}
	pTable->SetTail("ע:Ȩ��Ϊ0��ʾ������Ȩ������ȱʧֵ,��Ȩ��С�ڵ���0");
	CRsltObject * pETable= new CRsltElementTable("���ɷַ���",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
*/
/*

	TDataPointForScatter* pDPVV =new TDataPointForScatter;        
	
	for(int  k=0; k<m_nRow; k++)
	{
		pDPVV->XAxisArr.Add(m_VectScore[k]);
		pDPVV->YAxisArr.Add(m_VectScore[k+m_nRow]);
	}
	CTChartScatterPlot* pCL=new CTChartScatterPlot(NULL,pDPVV,m_nRow,0,"�غ�ͼ","��һ���ɷ�","�ڶ����ɷ�");
	CRsltElementChart *pREC=new CRsltElementChart("�غ�ͼ",pCL);
	delete pDPVV;
	pResult->Add(pREC);
*/	
/////////////
	int nOrgColNums=m_pOriDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();

	int i=0;
	for(i=0;i<m_nSel;i++)
	{
		CTString strFieldName;
		CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		strFieldName.Format("���ɷ�%d",i+1);
		while(IsExisted(m_pOriDataInterface,strFieldName))
		{
			strFieldName+="_1";
		}
		pField->SetFieldName(strFieldName);
		m_pOriDataInterface->m_DataAccess.m_pFieldList->Add(pField);
	}

	int nRecNo = 1;
	m_pOriDataInterface->m_DataAccess .First();

	for(i = 0;i<m_nRow;)
	{
				

		while(nRecNo < m_arrRecNo[i])
		{
			m_pOriDataInterface->m_DataAccess.Edit();
			for(int j = 0;j<m_nSel;j++)
			{
				CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j+nOrgColNums);
				pField->SetNull();
				
			}
			m_pOriDataInterface->m_DataAccess.Post();			
			m_pOriDataInterface->m_DataAccess.Next();
			nRecNo++;
		}
		m_pOriDataInterface->m_DataAccess.Edit();	
		for(int j = 0;j<m_nSel;j++)
		{
			CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j+nOrgColNums);
			pField->SetAsDouble(m_VectScore[i+j*m_nRow]);
		}
		i++;
		m_pOriDataInterface->m_DataAccess.Post();			
		m_pOriDataInterface->m_DataAccess.Next();
		nRecNo++;
	}
}




/************************************************************************************
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2003-2-9
�޸�:

********************************************************/
void CFactor::FromCorrelationToCommunality(CDoubleVector Vect_Data)
{  
	int	i;
	//��⹲�Է������
	CDoubleMatrix R= VectorToMatrix(Vect_Data,m_nCol,m_nCol);
	
	CDoubleMatrix RR = pinv(R);
	

	m_VectCommunality.create( m_nCol);
	for(i = 0;i<m_nCol;i++)
	{
		m_VectCommunality[i] = R(i,i);
	}
	for( i = 0;i<m_nCol;i++) //���Ӻ���
	{
		double psi = 1/RR(i,i);
		m_VectCommunality[i] -= psi;
	}
	//TraceVector(m_VectCommunality);
}
/************************************************************************************
�������ƣ�

����˵����
	�����ӷ���,��һ�����Է������ֱ�������ϵ���������õ�.
	(�Ժ�Ĺ��Է��������������,������������õ�)
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2003-2-9
�޸�:

***************************************************************************************/
void CFactor::CalcFirstCommunalityAndEig(CDoubleVector Vect_Data,int nIndex)
{
	assert(Vect_Data.vlen()>0);
	
	//��⹲�Է������ 
	FromCorrelationToCommunality(Vect_Data);//FromcovarianceToCommunality.
	
	int i;
	m_EigenVector.create(m_nCol*m_nCol);
	cjcbj(Vect_Data,m_nCol,m_EigenVector,0.00001);//�����������m_pEigenVector
	SortEigenVect(Vect_Data,m_EigenVector,m_nCol,m_nCol,1);//������������m_pEigenVector
	m_VectEigenValue.create(m_nCol);
	for(i =0;i<m_nCol;i++)
		m_VectEigenValue[i] = Vect_Data[i+i*m_nCol];
	//TraceVector(m_VectEigenValue);
	CreateEigenValueResult(Vect_Data,m_nCol);//�������ֵ
	
	m_nSel = GetSelNum(m_VectEigenValue);	//�������Ҫ�������ֵ�ĸ���
}
int CFactor::GetSelNum(CDoubleVector Vect_Data)
{
	double sum(0);
	int i=0;
	for(i=0;i<m_nCol;i++)
	{
		sum += Vect_Data[i];
	}
	int nSel=0;
	double pro = 0;
	for(i=0;i<m_nCol;i++)
	{	
		if( ( pro < m_fPro && m_nExtractStandard == 0)
			||( nSel < m_nFactorNum && nSel < m_nCol  && m_nExtractStandard == 1)			
			)
		{	
			pro += Vect_Data[i]/sum;
			nSel++;
		}
		//Ϊregression���ӵ÷����ļ��
		if( Vect_Data[i] <= 1e-8 )
		{
			m_bEigenFlag = true;
		}
	}
	return nSel;
}
CDoubleVector CFactor::GetOratedExplains(CDoubleMatrix MatrixA)//�õ���ת��ĺ��ؾ���֮������䷽���
{
	int nCol = MatrixA.mcol();
	CDoubleVector Ret(nCol);
	if (nCol > 0)
	{
		int nRow = MatrixA.mrow();
		CDoubleVector temp;
		int i =0;
		for(i =0;i<nCol;i++)
		{
			temp= MatrixA.col(i);
			Ret[i]=dot(temp,temp);
		}
	}
	return Ret;
}

void CFactor::CreateRotedExplainTable()
{
//	assert(m_VectRotEigenValue.vlen());
//	int nCol = m_VectRotEigenValue.vlen();
//	int i;
//	CTLTable * pTable = NULL;
//	pTable = new CTLTable();
//	pTable->SetTitle("��ת������ӵķ���ױ�");
////	pTable->SetRows(2);
////	pTable->SetCols(nCol+1);
//
//	CTString sTr("");
//	pTable->InsertColumn(0,sTr);
//	pTable->InsertItem(1,"�����");
//	for(i =1;i<nCol+1;i++)
//	{
//		sTr.Format("����%d",i);
//		pTable->InsertColumn(i,sTr);
//		pTable->SetItemText(1,i,m_VectRotEigenValue[i-1]);
//	}	
//	//pTable->SetTail("ע:�ñ��г�����ת������������Է���Ĺ���");
//	CRsltObject * pETable= new CRsltElementTable("��ת������ӶԷ���Ĺ���",pTable);//����һ���������Ķ���
//	m_pResult->Add(pETable);
//
}

void CFactor::CreateSquareSumExplainTable()//������Ӻ���ƽ����
{
	assert(m_VectEigenValue.vlen());
	int nCol = m_VectEigenValue.vlen();
	int i;
	CTLTable * pTable = NULL;
	pTable = new CTLTable();
	CTString sTr("");
	sTr.Format("���Ӻ���ƽ���� ");	
	pTable->SetTitle(sTr);
//	pTable->SetRows(2);
//	pTable->SetCols(nCol+1);

	sTr.Empty();
	pTable->InsertColumn(0,sTr);
	pTable->InsertItem(1,"�����");
	for(i =1;i<nCol+1;i++)
	{
		sTr.Format("����%d",i);
		pTable->InsertColumn(i,sTr);
		pTable->SetItemText(1,i,m_VectEigenValue[i-1]);
	}	
	//pTable->SetTail("ע:�ñ��г��˸������������صķ���Ĺ���");
	CRsltElementTable * pETable= new CRsltElementTable("���Ӻ���ƽ����",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
	
}
void CFactor::OnFactorExit() //������ϣ�����������ڴ�ռ䡣
{
	if(m_VectUnAdjustCom.vlen())
		m_VectUnAdjustCom.destruction();

	if(m_VectEigenValue.vlen())
		m_VectEigenValue.destruction();

	if(m_VectSelEigenValue.vlen())
		m_VectSelEigenValue.destruction();

	if(m_VectData.vlen())
		m_VectData.destruction();

	if(m_VectRotEigenValue.vlen())
		m_VectRotEigenValue.destruction();

	if(m_EigenVector.vlen())
		m_EigenVector.destruction();

	if(m_VectComMatrix.vlen())
		m_VectComMatrix.destruction();

	if(m_VectCommunality.vlen())
		m_VectCommunality.destruction();

	if(m_VectCorrelation.vlen())
		m_VectCorrelation.destruction();

	if(m_VectPreCommunality.vlen())
		m_VectPreCommunality.destruction();

	if(m_VectScore.vlen()>0)
		m_VectScore.destruction();
	
	if(m_VectDataFormlized.vlen()>0)
		m_VectDataFormlized.destruction();

	if(m_VectFirstCovariance.vlen()>0)
		m_VectFirstCovariance.destruction();
	
	if(m_InitCommunality.vlen()>0)
		m_InitCommunality.destruction();	
	m_bKMO =true;
}

int CFactor::brinv(CDoubleVector& a,int n)
{ 
	int i,j,k,l,u,v;
	CIntVector is(n);
	CIntVector js(n);
	double d,p;
	
	for (k=0; k<=n-1; k++)
	{ 
		d=0.0;
		for (i=k; i<=n-1; i++)
			for (j=k; j<=n-1; j++)
			{ l=i*n+j; p=fabs(a[l]);
			if (p>d) { d=p; is[k]=i; js[k]=j;}
			}
			if (d+1.0==1.0)
			{ //free(is); free(js); printf("err**not inv\n");
				//		  delete is;delete js;
				is.destruction();
				js.destruction();
				return(0);
			}
			if (is[k]!=k)
				for (j=0; j<=n-1; j++)
				{ u=k*n+j; v=is[k]*n+j;
				p=a[u]; a[u]=a[v]; a[v]=p;
				}
				if (js[k]!=k)
					for (i=0; i<=n-1; i++)
					{ u=i*n+k; v=i*n+js[k];
					p=a[u]; a[u]=a[v]; a[v]=p;
					}
					l=k*n+k;
					a[l]=1.0/a[l];
					for (j=0; j<=n-1; j++)
						if (j!=k)
						{ u=k*n+j; a[u]=a[u]*a[l];}
						for (i=0; i<=n-1; i++)
							if (i!=k)
								for (j=0; j<=n-1; j++)
									if (j!=k)
									{ u=i*n+j;
									a[u]=a[u]-a[i*n+k]*a[k*n+j];
									}
									for (i=0; i<=n-1; i++)
										if (i!=k)
										{ u=i*n+k; a[u]=-a[u]*a[l];}
	}
	for (k=n-1; k>=0; k--)
	{ if (js[k]!=k)
	for (j=0; j<=n-1; j++)
	{ u=k*n+j; v=js[k]*n+j;
	p=a[u]; a[u]=a[v]; a[v]=p;
	}
	if (is[k]!=k)
		for (i=0; i<=n-1; i++)
		{ u=i*n+k; v=i*n+is[k];
		p=a[u]; a[u]=a[v]; a[v]=p;
		}
	}
	//  is.destruction();
	//   js.destruction();
	
	//delete js;
	return(1);
}



