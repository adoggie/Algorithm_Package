// BlurryCluster.cpp: implementation of the CBlurryCluster class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BlurryCluster.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
const UINT WM_PROGRESS_TEXT=::RegisterWindowMessage("WM_PROGRESS_TEXT");
*/
const double DIFF=0.005;
const int MAXGRAPH=300;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
std::string CBlurryOutput::Export()//����ΪXML��ʽ
{
	std::string str;

	return str;
}

bool CBlurryOutput::Import(std::string strModel)
{
	return true;
}


CBlurryCluster::CBlurryCluster()
{

	m_dLambda=0.8;
	m_pResult=NULL;
	m_pRsltElementText=NULL;
	m_bIsToEnd=false;
// 	m_pProcedureWnd=NULL;
	m_pSpecifyStrArray=NULL;
	m_pNameStrArray=NULL;
	m_nOutSel=0;
}

CBlurryCluster::~CBlurryCluster()
{
	m_tVarInfo.DeleteObject();
	if(m_pSpecifyStrArray)
	{
		m_pSpecifyStrArray->RemoveAll();
		delete m_pSpecifyStrArray;
	}
	m_pSpecifyStrArray=NULL;
	if(m_pNameStrArray)
	{
		m_pNameStrArray->RemoveAll();
		delete m_pNameStrArray;
	}
	m_pNameStrArray=NULL;
}


int * CBlurryCluster::GetData(CDataInterface *pDataInterface,TIntStruct tVarInfo,int bRemoveEqual)
{
    int * pData = NULL;
    //ȡ�����ݣ���֯�������ݾ���
    int nCaseNumb,m = tVarInfo.iCount;
    TDataSet * pDataArray=NULL;
    int nColNum=0;
    //���ݼ���
	pData = new int [pDataInterface->GetRowCount() * m ];
	if ( tVarInfo.iCount <= 0 )
    {
        CTString str="��ѡ���������Ч!";
		if(m_pRsltElementText==NULL)
		{
			m_pRsltElementText=new CRsltElementText("����");
		}
		m_pRsltElementText->AddString(str);
		m_bIsToEnd=true;
		return pData;
    }
    for(int i=0;i<m;i++)
    {
		if(m_bIsToEnd)
		{
			CTString str="��ѡ������ֹ���򣬷�����Ч��";
			if(m_pRsltElementText==NULL)
			{
				m_pRsltElementText=new CRsltElementText("����");
			}
			m_pRsltElementText->AddString(str);
			break;
		}
        nColNum=tVarInfo.pValue[i];
        nCaseNumb = pDataInterface->GetColData(nColNum,pDataArray,1);//ȡ������
        assert(nCaseNumb > 0 );
        //��ֵ
        //�����������ݣ��������ֵ��ͬ���޳�
        for(int j=0;j<nCaseNumb;j++)
                pData[i+j*m] = int(pDataArray[j].dValue);
        
        if( pDataArray )
        {
             delete [] pDataArray;
             pDataArray=NULL;
        }
    }                
    return pData;
}

CResult * CBlurryCluster::RunTrain(CDataInterface *pDataInterface,CEnvParam &pEnv, Interaction *&itt,CBlurryVO *pVO)
{
	m_pDataInterface = pDataInterface;
	m_pResult=NULL;
	m_pRsltElementText=NULL;
	m_tVarInfo.DeleteObject();
	//	m_dLambda=credit;
	//	m_nSelect=nSelection;
	//	m_pNameStrArray=pNameStrArray;
	//	m_pSpecifyStrArray=pSpecifyStrArray;
	CTArray<int,int> tempArray;
	for(int i=0;i<pVO->arFeildInfo.GetSize();i++)
	{
		CTString strField = (pVO->arFeildInfo.GetAt(i));
		int nFieldNo = pDataInterface->IndexByName(strField);
		tempArray.Add(nFieldNo);
	}
	int count=tempArray.GetSize();
	int *pValue=new int[count];
	for(int ii=0;ii<count;ii++)
		pValue[ii]=tempArray.GetAt(ii);
	m_tVarInfo=TIntStruct(count,pValue);
	//	delete []pValue;
	m_nSelect = pVO->nSelect;//add by xds 20061009
	SetOutSel(pVO->bIcicle,pVO->bPedigree);
	return OnBlurryCluster();
}

CResult * CBlurryCluster::Run(CDataInterface *pDataInterface, CPtrArray * &pCPtrArray)
{	
	m_pDataInterface = pDataInterface;
	m_pResult=NULL;
	m_pRsltElementText=NULL;
	m_tVarInfo.DeleteObject();
//	m_dLambda=credit;
//	m_nSelect=nSelection;
//	m_pNameStrArray=pNameStrArray;
//	m_pSpecifyStrArray=pSpecifyStrArray;
	CTArray<int,int> tempArray;
	for(int i=0;i<pCPtrArray->GetSize();i++)
	{
		TFieldRole *p=(TFieldRole *)(pCPtrArray->GetAt(i));
		if(p->strRole==CTString("�������"))
			tempArray.Add(p->nFieldNo);
	}
	int count=tempArray.GetSize();
	int *pValue=new int[count];
	for(int ii=0;ii<count;ii++)
		pValue[ii]=tempArray.GetAt(ii);
	m_tVarInfo=TIntStruct(count,pValue);
//	delete []pValue;
	return OnBlurryCluster();
}

bool	CBlurryCluster::SetDataParam(CBlurryVO *pVO)
{
	m_pBlurryVO = pVO;
	return true;
}

//////////////////��������i�е�ƽ��ֵ////////////////
double CBlurryCluster::myMean(CDoubleMatrix matrix,int i)
{
	double sum=0.0;
	for(int j=0;j<matrix.mrow();j++)
		sum=sum+matrix(j,i);
	sum=sum/matrix.mrow();
	return sum;
}

//////////////////��������i�з���//////////////////////////
double CBlurryCluster::variance(CDoubleMatrix matrix,int i)
{
	double average=myMean(matrix,i);
	double sum=0.0;
	for(int j=0;j<matrix.mrow();j++)
		sum=sum+(matrix(j,i)-average)*(matrix(j,i)-average);
	return sum;
}

//////////////////��������i�����j�еĳ˻�//////////////
double CBlurryCluster::Xij(CDoubleMatrix matrix,int i,int j)
{
	double sum=0.0;
	for(int k=0;k<matrix.mcol();k++)
	{
		sum=sum+matrix(i,k)*matrix(j,k);
	}
	return sum;
}

/////////////////////����rij////////////////////
//rij=(1.0+r*ij)/2.0;r*ij=(Xi.Xj')/sqrt((Xi.Xi')(Xj.Xj'))
//Xi,Xj��ʾ��������Xi',Xj'��ʾ��Ӧ����������ת��
double CBlurryCluster::rij(CDoubleMatrix matrix,int i,int j)
{
	double temp1=Xij(matrix,i,j);
	double temp2=Xij(matrix,i,i);
	temp2=temp2*Xij(matrix,j,j);
	temp2=sqrt(temp2);
	temp1=temp1/temp2;
	temp1=1.0+temp1;
	temp1=temp1/2.0;
	return temp1;
}

///////////��������ƽ��//////////////////////////
//���ؾ���temp_matrix[i][j]��Ϊ������matrix�ĵ�i�����j�ж�ӦԪ����Ƚϵ���Сֵ����������Щ
//��Сֵ�е����ֵ
bool CBlurryCluster::matrixMultiply(CDoubleMatrix &matrix,CDoubleMatrix &temp_matrix)
{
	//���������ľ�����������
	double temp=0.0;
	for(int i=0;i<matrix.mrow();i++)
	{
		for(int j=0;j<matrix.mcol();j++)
		{
			temp_matrix(i,j)=matrix(i,0)<matrix(0,j)?matrix(i,0):matrix(0,j);
			for(int k=0;k<matrix.mcol();k++)
			{
				temp=matrix(i,k)<matrix(k,j)?matrix(i,k):matrix(k,j);
				temp_matrix(i,j)=temp_matrix(i,j)>=temp?temp_matrix(i,j):temp;
				if(m_bIsToEnd)
					break;
				
			}
			if(m_bIsToEnd)
				break;
		}
		if(m_bIsToEnd)
			break;
	}
	return true;
}


///////////////�Ƚ���ͬά�����Ƿ����/////////////////////////
bool CBlurryCluster::compareMatrix(CDoubleMatrix matrix1,CDoubleMatrix matrix2)
{
	for(int i=0;i<matrix1.mrow();i++)
	{
		for(int j=0;j<matrix1.mcol();j++)
		{
			if(matrix1(i,j)!=matrix2(i,j))
				return false;
		}
	}
	return true;
}

//////////////////////////�ȽϾ���ĵ�i��j���Ƿ����/////////////
bool CBlurryCluster::compareij(CDoubleMatrix matrix,int i,int j)
{
	for(int k=0;k<matrix.mcol();k++)
		if(matrix(i,k)!=matrix(j,k))
			return false;

	return true;
}

bool CBlurryCluster::compareij(CIntMatrix matrix,int i,int j)
{
	for(int k=0;k<matrix.mcol();k++)
		if(matrix(i,k)!=matrix(j,k))
			return false;

	return true;
}

/////////////�ȽϾ���ĵ�j��Ԫ���Ƿ�ȫ���///////
bool CBlurryCluster::IsMatrixColEqual(CDoubleMatrix matrix,int j)
{
	for(int i=1;i<matrix.mrow();i++)
	{
		if((int)(matrix(0,j))!=(int)(matrix(i,j)))
			return false;
	}
	return true;
}

//�������������������������������ܡ�
bool CBlurryCluster::MatrixCreate(CIntMatrix &evalue_matrix,CIntMatrix &grade_matrix,double lambda)
{
/*
	if(m_pProcedureWnd&&(!m_bIsToEnd))
	{
		CTString str1="���ڹ��������Ծ���";
		m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str1));
	}
*/

	int evalue_length=evalue_matrix.mcol();
	int grade_length=grade_matrix.mcol();
	int nCol=0;
	int i;
	for(i=0;i<grade_length;i++)
	{
		if(grade_matrix(0,i)<1)
		{	
			if(!m_bIsToEnd)
			{
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);		
				str+="�ȼ�ֵ����С��1��";
				m_pRsltElementText->AddString(str);
				return false;
			}
			else
			{
				if(m_pRsltElementText==NULL)
				{
					m_pRsltElementText=new CRsltElementText("����");
					CTString str="��ѡ������ֹ���򣬷�����Ч��";
					m_pRsltElementText->AddString(str);
					
				}
				return false;

			}
		}
		nCol=nCol+(int)(grade_matrix(0,i)-1);
	}
//////�ж����֣������ֵС��0����׼��ͷ֣�����ֵΪ0����׼��ͷ֣���������ڱ�׼��߷֣���ֵΪ��߷֡�
	for(i=0;i<evalue_matrix.mrow();i++)
	{
		for(int j=0;j<evalue_matrix.mcol();j++)
		{		
			if(evalue_matrix(i,j)<0||evalue_matrix(i,j)>grade_matrix(0,j)-1)
			{
				if(!m_bIsToEnd)
				{

					if(m_pRsltElementText==NULL)
						m_pRsltElementText=new CRsltElementText("����");
					CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[j]);
					str+="ȡֵ������ڵ���0����С�ڵȼ�ֵ��";
					m_pRsltElementText->AddString(str);
					return false;
				}
				else
				{
					if(m_pRsltElementText==NULL)
					{
						m_pRsltElementText=new CRsltElementText("����");
						CTString str="��ѡ������ֹ���򣬷�����Ч��";
						m_pRsltElementText->AddString(str);						
					}
					return false;
				}
			}
			if(m_bIsToEnd)
				break;					///////////////
		}
		if(m_bIsToEnd)
			break;
	}
	int nRow=evalue_matrix.mrow();
	CDoubleMatrix initBinary_matrix(nRow,nCol);

	///����evalue_matrix and grade_matrix����initBinary_matrix����
	int k=0;
	int n=0;
	int sum=0;
	//WriteData(evalue_matrix,"evalue_Matrix");
	for(i=0;i<grade_length;i++)
	{
		
		for(n=0;n<nRow;n++)//row number
		{			
			for(k=0;k<(int)evalue_matrix(n,i);k++)//�е����λ��
			{
				initBinary_matrix(n,sum+k)=1;
			}
			for(k=(int)evalue_matrix(n,i);k<(int)(grade_matrix(0,i)-1);k++)
			{
				initBinary_matrix(n,sum+k)=0;
			}
			if(m_bIsToEnd)
				break;
		}
		sum=sum+(int)(grade_matrix(0,i)-1);
		if(m_bIsToEnd)
			break;
	}
	CIntMatrix M2;
	evalue_matrix=M2;
	CDoubleMatrix tempMatrix=copy(initBinary_matrix);
	
	//���ݱ任x[i][j]=(x[i][j]-E(Xj))/(sqrt(1.0/(nRow-1)*D(Xj)))
	//E��ʾƽ��ֵ��D��ʾ���Xj��ʾ����ĵ�j��������
	for(i=0;i<initBinary_matrix.mrow();i++)
	{
		for(int j=0;j<initBinary_matrix.mcol();j++)
		{
			if(IsMatrixColEqual(tempMatrix,j))//�����j��Ԫ��ȫ��ȣ����ı�ֵ
				initBinary_matrix(i,j)=tempMatrix(i,j);
			else
			{
				initBinary_matrix(i,j)=tempMatrix(i,j)-myMean(tempMatrix,j);
				double dVrc=variance(tempMatrix,j);
				double temp=1.0/(tempMatrix.mrow()-1.0)*dVrc;
				temp=sqrt(temp);
				initBinary_matrix(i,j)=initBinary_matrix(i,j)/temp;
			}
			if(m_bIsToEnd)
				break;
		}
		if(m_bIsToEnd)
			break;
	}
	CDoubleMatrix M;
	tempMatrix=M;
//	destruction(tempMatrix);
	int nRowSize=initBinary_matrix.mrow();
	CDoubleMatrix R_matrix(nRowSize,nRowSize);
	for(i=0;i<R_matrix.mrow();i++)
	{
		for(int j=0;j<R_matrix.mcol();j++)
		{				
			R_matrix(i,j)=rij(initBinary_matrix,i,j);
			if(m_bIsToEnd)
				break;
		}
		if(m_bIsToEnd)
			break;
	}
	initBinary_matrix=M;
	int nnn=initBinary_matrix.mrow();
/*
	if(m_pProcedureWnd&&(!m_bIsToEnd))
	{
		CTString str2="���ڶ�ģ����������޻�����";
		m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str2));
	}
*/

	CDoubleMatrix R2_matrix(R_matrix.mrow(),R_matrix.mcol());
	if(!matrixMultiply(R_matrix,R2_matrix))
		return false;								////////
	while(!compareMatrix(R_matrix,R2_matrix))		
	{
		if(m_bIsToEnd)
			break;							///////
		R_matrix=copy(R2_matrix);
		if(!matrixMultiply(R_matrix,R2_matrix))
			break;							///////
	}	
	
	for(i=0;i<R2_matrix.mrow();i++)
	{
		for(int j=0;j<R2_matrix.mcol();j++)
			if(R2_matrix(i,j)<lambda)
				R2_matrix(i,j)=0;
			else
				R2_matrix(i,j)=1;
	}
//	WriteData(R_matrix,"R_matrix1");
//	WriteData(R2_matrix,"R2_matrix");
	///�洢������Ϣ
	CPtrArray group;
	CTArray<int,int> ElementsArray;
	int nR2Row=R2_matrix.mrow();
	for(i=0;i<nR2Row;i++)
		ElementsArray.Add(i);
	for(i=0;i<ElementsArray.GetSize();i++)
	{
		CPtrArray *subGroup=new CPtrArray;
		int *pTemp=new int(ElementsArray.GetAt(i));
		subGroup->Add((void *)pTemp);
		for(int j=i+1;j<ElementsArray.GetSize();j++)
		{
			if(compareij(R2_matrix,ElementsArray.GetAt(i),ElementsArray.GetAt(j)))
			{
				pTemp=new int(ElementsArray.GetAt(j));
				subGroup->Add((void *)pTemp);
				ElementsArray.RemoveAt(j);
				int nSize=ElementsArray.GetSize();
				j--;
			}
		}
		group.Add((void *)subGroup);
		
	}
	CPtrArray outPtrArray;
	RecoveryOrgData(group,m_FirstClassfyPtrArray,outPtrArray);
	ElementsArray.RemoveAll();
	//////////////Ϊͼ���������	
	TBlurryTree node;//���ڵ�
	CPtrArray classfyPtrArray;
	double dLowCredit=0;
	double dHiCredit=1;
	CTArray<int,int> OrderedElemsArray;//��������������ײ���кŵ��Ⱥ�˳��洢�кš�

//	CTArray<int,int> elemsArray;
	int nMatrixRow=R_matrix.mrow();
	if(m_nDataSize<=MAXGRAPH&&(!m_bIsToEnd))
	{
/*
		if(m_pProcedureWnd&&(!m_bIsToEnd))
		{
			CTString str4="���ھ��࣡";
			m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str4));
		}
*/

		for(i=0;i<nMatrixRow;i++)//��������
		{
			node.elemsArray.Add(i);
		}
		node.dCredit=MiddleClassfy(classfyPtrArray,dLowCredit,dHiCredit,node.elemsArray,R_matrix,1);
		node.yPosition=-1;	
		if(node.dCredit==1)
		{
			node.bBottom=true;
			node.bRead=false;
		}

		else
		{
			node.bBottom=false;
			node.bRead=false;
			CreateTree(R_matrix,node,classfyPtrArray);
		}
		GetYPositionArray(node,OrderedElemsArray);
		CompleteTreeYPositionValue(node,OrderedElemsArray);
		//���ǵײ�ڵ������ˮƽ����С�����������
		int nSize=m_dCreditArray.GetSize();
		
		for(i=0;i<nSize;i++)
		{
			for(int j=i+1;j<nSize;j++)
			{		
				double temp=m_dCreditArray.GetAt(i);
				if(temp>m_dCreditArray.GetAt(j))
				{
					m_dCreditArray.SetAt(i,m_dCreditArray.GetAt(j));
					m_dCreditArray.SetAt(j,temp);
				}
			}
		}
	}	
/*
	if(m_pProcedureWnd&&(!m_bIsToEnd))
	{
		CTString str3="���ڸ�ʽ�����";
		m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str3));
	}
*/
	if(!m_bIsToEnd)
		CreateResult(node,R_matrix,R2_matrix,outPtrArray,OrderedElemsArray);

	///free the memory;
	DeleteTree(node);
	delCPtrArray(group);
	delCPtrArray(outPtrArray);
	if(m_nOutSel<2)
		delCPtrArray(m_FirstClassfyPtrArray);
	else  m_FirstClassfyPtrArray.RemoveAll();//����ڴ���ת�Ƶ�result/CTChartPedigree�� [mxm modify it] 2004-05-21
	delCPtrArray(classfyPtrArray);
	OrderedElemsArray.RemoveAll();
	return true;

}


///////������//////////////////
void CBlurryCluster::CreateResult(TBlurryTree &node,CDoubleMatrix &R_matrix,CDoubleMatrix &blurry_matrix,CPtrArray & group,CTArray<int,int> &OrderedArray)
{
/*	///////////////���R��///////////////
	pTable->SetTitle("ģ��������");
	pTable->SetCols(R_matrix.mcol()+1);
	pTable->SetRows(R_matrix.mrow()+1);
	CTString str;
	for(int i=0;i<R_matrix.mcol();i++)
	{
		if(i==0)
			pTable->InsertColumn(0,"",50);
		str.Format("%d",i+1);
		pTable->InsertColumn(i+1,str);
		pTable->InsertItem(i+1,str);
	}
	for(i=0;i<R_matrix.mrow();i++)
	{
		for(int j=0;j<R_matrix.mcol();j++)
		{
			pTable->SetItemText(i+1,j+1,R_matrix[i][j]);
		}
	}
	pETable= new CRsltElementTable("ģ��������",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);

///////////////////���R_lambda��///////////////////////
	pTable = new CTLTable;
	pTable->SetTitle("�����ϵ��");
	CTString strLambda;
	strLambda.Format("����ˮƽ=%.3f",m_dLambda);
	pTable->SetHead(strLambda);

	pTable->SetCols(blurry_matrix.mcol()+1);
	pTable->SetRows(blurry_matrix.mrow()+1);

	for(i=0;i<blurry_matrix.mcol();i++)
	{
		if(i==0)
			pTable->InsertColumn(0,"",50);
		str.Format("%d",i+1);
		pTable->InsertColumn(i+1,str,50);
		pTable->InsertItem(i+1,str);
	}
	for(i=0;i<blurry_matrix.mrow();i++)
	{
		for(int j=0;j<blurry_matrix.mcol();j++)
		{
			pTable->SetItemText(i+1,j+1,(int)blurry_matrix[i][j]);
		}
	}
	pETable= new CRsltElementTable("�����ϵ��",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
*/

//////////////////////////��������////////////////////////////
	//�õ������г���
	int nRow=group.GetSize();
	int k=0;
	int nCol=1;
//	m_pResult = new CResult("ģ���������");
	CRsltVariable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("�����");
	CTString strLambda;
	strLambda.Format("����ˮƽ=%.3f",m_dLambda);
// 	pTable->SetCellAlignFormat(DT_LEFT);

	pTable->SetHead(strLambda);
	pTable->SetCols(nCol+1);
	pTable->SetRows(nRow+1);
	int i;
	for(i=0;i<nRow;i++)//������
	{
		CTString str;
		str.Format("��%d",i+1);
		pTable->InsertItem(i+1,str);
	}
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"�����¼");
	for(i=0;i<nRow;i++)//���������Ϣ
	{
		CPtrArray *pTempPtrArray=(CPtrArray *)(group.GetAt(i));
		int tempSize=pTempPtrArray->GetSize();
		CTString str;
		str.Empty();
	//	str+="��¼�ţ�";
		for(k=0;k<tempSize;k++)
		{
			CTString strtemp;
			strtemp.Format("%d,",*((int *)(pTempPtrArray->GetAt(k)))+1);
			str+=strtemp;
		}	
		if(str.GetAt(str.GetLength()-1)==',')
			str.Delete(str.GetLength()-1);
		pTable->SetItemText(i,1,str);
	}
	pETable= new CRsltElementTable("�����",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);


	///��ͼ
	if(m_nOutSel>0)//�������ͼ����ϵͼ [mxm add it] 2004-05-21
	{

	CPtrArray LinkerPtrArray;
	CTArray<int,int> OrderedArray2;
	if(m_nDataSize<=MAXGRAPH&&(!m_bIsToEnd))
	{
		pTable = new CTLTable;
		pTable->SetTitle("��������ͼ");
		nRow=m_dCreditArray.GetSize();
		nCol=OrderedArray.GetSize();
		CTStringArray LinkerStrArray;
		CTStringArray *pStrArray=new CTStringArray;
		for(i=0;i<nCol-1;i++)
		{
			LinkerStrArray.Add("X");////////////////////
			pStrArray->Add("X");//1��ʾ��Ϊһ�࣬0��ʾ����Ϊһ��////////////
		}
		//�õ������ϵ���������Ǵ洢��LinkerPtrArray��
		LinkerPtrArray.Add((void *)pStrArray);
		for(i=0;i<nRow;i++)
		{
				double dCredit=m_dCreditArray.GetAt(i);
				ChangeClassLinker(node,LinkerStrArray,OrderedArray,m_dCreditArray.GetAt(i));
				CTStringArray *pStrArray=new CTStringArray;
				for(int j=0;j<LinkerStrArray.GetSize();j++)
					pStrArray->Add(LinkerStrArray.GetAt(j));
				LinkerPtrArray.Add(pStrArray);
		}


		///ӳ���ԭʼ���кš�
		for(i=0;i<OrderedArray.GetSize();i++)
		{
			int	nColIndex=OrderedArray.GetAt(i);
			CPtrArray *p=(CPtrArray *)(m_FirstClassfyPtrArray.GetAt(nColIndex));
			int temp=*((int *)(p->GetAt(0)));
			OrderedArray2.Add(temp);
		}

	//�������ˮƽֵ
		CTString strCreditHead="\n";
		int count=0;
		for(i=0;i<m_dCreditArray.GetSize()+1;i++)
		{
			CTString str;
			if(i==0)
			{		
				str.Format("����ˮƽ%d<%.4f;  ",i+1,m_dCreditArray.GetAt(0));
			}
			else
				str.Format("����ˮƽ%d=%.4f;  ",i+1,m_dCreditArray.GetAt(i-1));
			strCreditHead+=str;
			count++;
			if(count==5)
			{
				strCreditHead+="\n\n";
				count=0;
			}
		}


		if(m_nOutSel==1 || m_nOutSel==3)//�������ͼ���б����� [mxm add it] 2004-05-21
		{
		//��ԭʼ���к�׷�ӵ���������ͼ��
			for(i=0;i<m_FirstClassfyPtrArray.GetSize();i++)
			{
				CPtrArray *pPtrArray=(CPtrArray *)(m_FirstClassfyPtrArray.GetAt(i));
				if(pPtrArray->GetSize()<=1)
					continue;
				else//˵������������������ֵͬ��¼����׷��
				{
					int nRowNum=*((int *)(pPtrArray->GetAt(0)));
					int nPosition=0;
					for(int n=0;n<OrderedArray2.GetSize();n++)//�ҵ�Ӧ�����λ��
					{
						if(nRowNum==OrderedArray2.GetAt(n))
						{
							nPosition=n;
							break;
						}
					}
					for(int j=1;j<pPtrArray->GetSize();j++)//����ȷ��λ�ò���ǰ�޳�������ֵͬ��¼���кţ���
					{
						CTString str;
						int nRowValue=*((int *)(pPtrArray->GetAt(j)));
						OrderedArray2.InsertAt(nPosition,nRowValue);
						for(int n=0;n<LinkerPtrArray.GetSize();n++)
						{
							CTStringArray *pStrArray=(CTStringArray *)(LinkerPtrArray.GetAt(n));
							pStrArray->InsertAt(nPosition,"X");
						}
					}
				}
			}

			nCol=OrderedArray2.GetSize();
			pTable->SetCols(nCol+1);
			pTable->SetRows(nRow+2);
// 			pTable->SetCellAlignFormat(DT_CENTER);
			pTable->SetHead(strCreditHead);
		//	pTable->set

			
			for(i=0;i<2*nCol;i++)//������
			{
				if(i==0)
					pTable->InsertColumn(i,"");
				else
				{
					if(i%2!=0)
					{
						CTString str;
						str.Format("%d",OrderedArray2.GetAt(i/2)+1);
						pTable->InsertColumn(i,str);
					}
					else
						pTable->InsertColumn(i,"");
				}
			}
			
			for(i=0;i<nRow+1;i++)//������
			{
				CTString str;
				str.Format("%d",i+1);
				pTable->InsertItem(i,str,false);
			}
			//����Ԫ��ֵ
			for(i=0;i<nRow+1;i++)
			{
				pStrArray=(CTStringArray *)(LinkerPtrArray.GetAt(i));
				for(int j=0;j<2*nCol-1;j++)
				{
					if(j%2==0)
					{
						pTable->SetItemText(i,j+1,"X");
					}
					else
					{
						pTable->SetItemText(i,j+1,pStrArray->GetAt(j/2));
					}			

				}

			}

			pETable= new CRsltElementTable("��������ͼ",pTable);//����һ���������Ķ���
			m_pResult->Add(pETable);
		}
		
		///{{��ϵͼ���� //[mxm add following]
		if(m_nOutSel>=2)
		{
			if(m_nOutSel==2)
			{
				CRsltElementText * pETXT = new CRsltElementText("����ˮƽֵ");
				pETXT->AddString("����ˮƽֵ:\n");				
				pETXT->AddString(strCreditHead+"\n");
				m_pResult->Add(pETXT);
			}
			double dMinX = 0;
			PedigreeTreeNode treenode;
			double dMaxX  = 1-m_dCreditArray.GetAt(0);
			int nCount = OrderedArray.GetSize();
			CTChartPedigree *pTChart = new CTChartPedigree(false);
			CreatePedigree(treenode,node);
			pTChart->SetTitle("��ϵͼ");
			pTChart->SetXAxilLabel("����ˮƽ");
			pTChart->SetYAxilLabel("���൥Ԫ");
			pTChart->SetValue(dMinX,dMaxX,0,nCount,&treenode,m_FirstClassfyPtrArray);
			CRsltElementChart * pEChart = new CRsltElementChart("��ϵͼ",pTChart);
			m_pResult->Add(pEChart);
		}
	 }
	
	//free the memory
	for(i=0;i<LinkerPtrArray.GetSize();i++)
	{
		CTStringArray *p=(CTStringArray *)(LinkerPtrArray.GetAt(i));
		p->RemoveAll();
		delete p;
	}
	LinkerPtrArray.RemoveAll();
	OrderedArray2.RemoveAll();

	}
	m_dCreditArray.RemoveAll();
}


CResult * CBlurryCluster::OnBlurryCluster()
{
	m_pResult = new CResult("ģ���������");
/*
	if(m_pProcedureWnd)
	{
		CTString str="���ڵ������ݺ�Ԥ����";
		m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str));
	}
*/
	int n = m_pDataInterface->GetRowCount();
	m_nDataSize=n;
	int m = m_tVarInfo.iCount;
	CIntMatrix grade_matrix(1,m);
	if(m==0)
	{
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("����");
		CTString str="\nû����Ч����������";
		m_pRsltElementText->AddString(str);
		m_pResult->Add(m_pRsltElementText);
		return m_pResult;

	}
	if(n==0)
	{
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("����");
		CTString str="\n����Ϊ�գ�����ѡ���ݣ�";
		m_pRsltElementText->AddString(str);
		m_pResult->Add(m_pRsltElementText);
		return m_pResult;
	}

	if(m_nSelect==0)
	{	
		int i;
		for(i=0;i<m;i++)
		{
			grade_matrix(0,i)=GetMaxValue(m_tVarInfo.pValue[i]);
			if(grade_matrix(0,i)>100)
			{
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
				str+="�ȼ�����Ӧ����100��";
				m_pRsltElementText->AddString(str);
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;
			}
		}	
		int* pData=NULL;
		pData=GetData(m_pDataInterface,m_tVarInfo,false);
		if(!pData)
			return NULL;
		CIntMatrix evalue_matrix(pData,n,m);
		delete []pData;	
		CTArray<int,int> elemsArray;
		for(i=0;i<evalue_matrix.mrow();i++)
			elemsArray.Add(i);
		classfy(evalue_matrix,m_FirstClassfyPtrArray,elemsArray);
		int n1=m_FirstClassfyPtrArray.GetSize();
		if(n1>3000)
		{
			if(m_pRsltElementText==NULL)
			
				m_pRsltElementText=new CRsltElementText("����");
			 CTString str="����Դ���󣬽����ȶ��������г�������ɢ����";
			 m_pRsltElementText->AddString(str);
			 m_pResult->Add(m_pRsltElementText);
			 return m_pResult;
		}
		if(n1<2)
		{
			CTString str;
			if(!m_bIsToEnd)
			{
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				str="������Ч��¼����ȫ��ͬ,���Ǿ�Ϊһ�࣡";
				m_pRsltElementText->AddString(str);
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;
			}
			else
			{
				if(m_pRsltElementText==NULL)
				{
					m_pRsltElementText=new CRsltElementText("����");
					str="��ѡ������ֹ���򣬷�����Ч��";
					m_pRsltElementText->AddString(str);
					
				}	
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;

			}
		}
		evalue_matrix=CreateNewMatrix(m_FirstClassfyPtrArray,evalue_matrix);///ֻ����ֵ��ȫ��ͬһϵ�м�¼�ĵ�һ��
		MatrixCreate(evalue_matrix,grade_matrix,m_dLambda);		
	}
	else
	{
		CIntMatrix evalue_matrix(n,m,0);
		int i;
		for(i=0;i<m;i++)
		{			
			if(m_bIsToEnd)
			{
				break;
			}
			grade_matrix(0,i)=GetColMapData(evalue_matrix,i,m_tVarInfo.pValue[i]);
			if(grade_matrix(0,i)>100)
			{
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
				str+="�ȼ�����Ӧ����100��";
				m_pRsltElementText->AddString(str);
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;
			}
		}
		CTArray<int,int> elemsArray;
		for(i=0;i<evalue_matrix.mrow();i++)
			elemsArray.Add(i);
		classfy(evalue_matrix,m_FirstClassfyPtrArray,elemsArray);
		int n1=m_FirstClassfyPtrArray.GetSize();
		if(n1>3000)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			 CTString str="����Դ���󣬽����ȶ��������г�������ɢ����";
			 m_pRsltElementText->AddString(str);
			 m_pResult->Add(m_pRsltElementText);
			 return m_pResult;
		}
		if(n1<2)
		{
			CTString str;
			if(!m_bIsToEnd)
			{
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				str="������Ч��¼����ȫ��ͬ,���Ǿ�Ϊһ�࣡";
				m_pRsltElementText->AddString(str);
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;
			}
			else
			{
				if(m_pRsltElementText==NULL)
				{
					m_pRsltElementText=new CRsltElementText("����");
					str="��ѡ������ֹ���򣬷�����Ч��";
					m_pRsltElementText->AddString(str);
				}
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;

			}
		}
		evalue_matrix=CreateNewMatrix(m_FirstClassfyPtrArray,evalue_matrix);//ֻ����ֵ��ȫ��ͬһϵ�м�¼�ĵ�һ��
		MatrixCreate(evalue_matrix,grade_matrix,m_dLambda);						
	}
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	m_pResult->Print();
	return m_pResult;	

} 


//*********************************************************************
//zkc  2002.12.16
//strArray  the specified string array  
//str  the specified string
//funtion: to decide whether the strArray has the element string which
//			has the same value with the str��
//			if has,return false,or return true;
//************************************************************************
bool CBlurryCluster::notIn(int value,CTArray<int,int> &intArray)
{
	for(int i=0;i<intArray.GetSize();i++)
	{
		if(value==intArray.GetAt(i))
			return false;
	}
	return true;

}

//*********************************************************************
//zkc  2002.12.16
//strArray1 �洢һ���ַ���
//strArray2 �洢�����ַ�����ֵͬ���ַ���
//index  ָ����
//function:read the specified column data as string
//************************************************************************

int CBlurryCluster::GetMapData(int index,CTArray<int,int> &intArray1,CTArray<int,int> &intArray2)
{
	int nSize=1;
	if(index>=0)
	{		
		CField* pField;
		CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
		if(index>pDataAccess->m_pFieldList->GetFieldCount())
			return nSize;
		int temp=int(m_pDataInterface->GetColMeanValue(index));
		pDataAccess->First();
		pField=pDataAccess->m_pFieldList->FieldByIndex(index);
		while(!pDataAccess->Eof())
		{		
			int value=temp;
 			if(!pField->IsNull())
				value=pField->GetAsInteger();
			intArray1.Add(value);
			if(notIn(value,intArray2))
				intArray2.Add(value);
			pDataAccess->Next();
		}
				
	}

	//��intArray2����С��������
	nSize=intArray2.GetSize();
	for(int i=0;i<nSize;i++)
	{
		for(int j=i+1;j<nSize;j++)
		{		
			int temp=intArray2.GetAt(i);
			if(temp>=intArray2.GetAt(j))
			{
				intArray2.SetAt(i,intArray2.GetAt(j));
				intArray2.SetAt(j,temp);
			}
		}
	}
	return nSize;
}

//*****************************************************************************
//2002.12.25 zkc
//��ָ������nDataCol����ӳ����������ݴ洢��matrix��ָ����nMatrixCol��
//
//******************************************************************************
int CBlurryCluster::GetColMapData(CIntMatrix &matrix,int nMatrixCol,int nDataCol)
{
	CTArray<int,int> intArray1;
	CTArray<int,int> IntArray2;
	int nSize=GetMapData(nMatrixCol,intArray1,IntArray2);
	int nRow=intArray1.GetSize();
	for(int i=0;i<nRow;i++)
	{
		int value=intArray1.GetAt(i);
		for(int j=0;j<(IntArray2).GetSize();j++)
		{
			if(value==(IntArray2).GetAt(j))
			{
				matrix(i,nMatrixCol)=j;
				break;
			}

		}
	}
	intArray1.RemoveAll();
	return nSize;

}


//ȡ������ָ���е����ֵ
int CBlurryCluster::GetMaxValue(int nColIndex)
{
	int max=0;
	if(nColIndex>=0)
	{		
		CField* pField;
		CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
		if(nColIndex>pDataAccess->m_pFieldList->GetFieldCount())
			return 0;
		pDataAccess->First();
		pField=pDataAccess->m_pFieldList->FieldByIndex(nColIndex);
		int count=0;
		while(!pDataAccess->Eof())
		{				
			int temp=0;
			if(!pField->IsNull())
				temp=pField->GetAsInteger();
			if(count==0)
			{
				max=temp;
			}
			else
			{
				max=max>temp?max:temp;
			}
			pDataAccess->Next();
			count++;
		}
	}
	return max+1;
}


//************************************************************************
//2002.12.27 zkc
//������з��࣬��ͬ��¼�ŵļ�¼������ȫ��ֵͬ����ô��Ϊһ�ࣻ
//
//************************************************************************
void CBlurryCluster::classfy(CIntMatrix W_matrix,CPtrArray &ClassfyCPtrArray,CTArray<int,int> &elemsArray)
{
	delCPtrArray(ClassfyCPtrArray);
	int nRow=W_matrix.mrow();
	int nCol=W_matrix.mcol();
	CTArray<int,int> rowArray;
	int i;
	for(i=0;i<nRow;i++)
	{
		rowArray.Add(i);
	}
	for(i=0;i<rowArray.GetSize();i++)
	{
		CPtrArray *subGroup=new CPtrArray;
		int *pTemp=new int(elemsArray.GetAt(rowArray.GetAt(i)));
		subGroup->Add((void *)pTemp);
		for(int j=i+1;j<rowArray.GetSize();j++)
		{
			if(compareij(W_matrix,rowArray.GetAt(i),rowArray.GetAt(j)))
			{
				pTemp=new int(elemsArray.GetAt(rowArray.GetAt(j)));
				subGroup->Add((void *)pTemp);
				rowArray.RemoveAt(j);
				j--;
			}
		}
		ClassfyCPtrArray.Add((void *)subGroup);
	}	
	rowArray.RemoveAll();
}

//************************************************************************
//2002.12.27 zkc
//ɾ����������
//
//************************************************************************

void CBlurryCluster::delCPtrArray(CPtrArray &toDel)
{
	
	for(int i=0;i<toDel.GetSize();i++)//
	{
		CPtrArray *pTempPtrArray=(CPtrArray *)(toDel.GetAt(i));
		int tempSize=pTempPtrArray->GetSize();
		for(int k=0;k<tempSize;k++)
		{
			delete pTempPtrArray->GetAt(k);
		}
		pTempPtrArray->RemoveAll();
		delete pTempPtrArray;
	}
	toDel.RemoveAll();
}
//************************************************************************
//2002.12.27 zkc
//���ڻ��ƾ�����ϵͼ�ķ��෽��
//
//************************************************************************
//��������д�С�����˳������
void CBlurryCluster::makeOrder(CTArray<int,int> &core)
{
	int nSize=core.GetSize();
	for(int i=0;i<nSize;i++)
	{
		for(int j=i+1;j<nSize;j++)
		{		
			int temp=core.GetAt(i);
			if(temp>=core.GetAt(j))
			{
				core.SetAt(i,core.GetAt(j));
				core.SetAt(j,temp);
			}
		}
	}
}

////**************************


//************************************************************************
//03.1.7
//zkc
//�ݹ�����ڵ���з��࣬
//************************************************************************
double CBlurryCluster::MiddleClassfy(CPtrArray &classfyPtrArray,//���ڷ���Ԫ�صĴ洢
								   double &dLowCredit,//��ֵ��Сֵ
								   double &dHiCredit,//��ֵ����ֵ
								   CTArray<int,int> &elemsArray,//ĳ�ڵ��Ԫ�أ���¼������Ӧģ�������ϵ������к�
								   CDoubleMatrix &R_matrix,int nSubSize)	//ģ�������ϵ����				   
{								  
	double diff=(dHiCredit-dLowCredit)/2;
	double lambda=dLowCredit+diff;
	if(m_bIsToEnd)
		return lambda;
	if(diff<DIFF&&(nSubSize>=2))
		return lambda;
	int nRow=elemsArray.GetSize();
	int nCol=R_matrix.mcol();
	//WriteData(R_matrix,"R_matrix");
	CIntMatrix classfyMatrix(nRow,nCol);//�ɸýڵ��Ԫ�ع��ɵľ���	
	for(int i=0;i<nRow;i++)
	{
		int nMapRow=elemsArray.GetAt(i);
		for(int j=0;j<nCol;j++)
		{
			
			if(R_matrix(nMapRow,j)<lambda)
				classfyMatrix(i,j)=0;
			else
				classfyMatrix(i,j)=1;
		}
	}
	//WriteData(classfyMatrix,"classfyMatrix");
	delCPtrArray(classfyPtrArray);
	classfy(classfyMatrix,classfyPtrArray,elemsArray);
	int nThisSize=classfyPtrArray.GetSize();
	if(nThisSize>=2)
	{
		dHiCredit=lambda;
	}
	if(nThisSize==1)
	{
		dLowCredit=lambda;
	}
	return MiddleClassfy(classfyPtrArray,dLowCredit,dHiCredit,elemsArray,R_matrix,nThisSize);
}

//////////////first free the memory and delete the elements of a,then copy
/////////////the elements of b into a.
void CBlurryCluster::ToCArray(CPtrArray * &a,CTArray<int,int> &b)
{
	b.RemoveAll();
	for(int i=0;i<a->GetSize();i++)
	{
		int value=*((int *)(a->GetAt(i)));
		b.Add(value);
	}

}

///*****************************************************************************************
//2003.1.7
//�ſ˳�
//����������
//******************************************************************************************
void CBlurryCluster::CreateTree(CDoubleMatrix &R_matrix,TBlurryTree &node,CPtrArray &elemsPtrArray)
{
	int nSize=elemsPtrArray.GetSize();
	for(int i=0;i<nSize;i++)
	{
		CPtrArray *pElems=(CPtrArray *)(elemsPtrArray.GetAt(i));
		TBlurryTree *pTBlurryTree=new TBlurryTree;
		node.NodePtrArray.Add((void *)pTBlurryTree);		
		ToCArray(pElems,pTBlurryTree->elemsArray);
		int nRow=pTBlurryTree->elemsArray.GetSize();
		if(nRow==1)//��1��Ԫ��
		{
			pTBlurryTree->bBottom=true;
			pTBlurryTree->dCredit=1;
			pTBlurryTree->yPosition=-1;
			pTBlurryTree->bRead=false;
		}
		else    //���жϸ�Ԫ�أ���¼�У���Ӧ�ķ����ϵ�����Ӧ�����Ƿ���ȫ���
		{
			int flag=0;
			for(int i=0;i<nRow;i++)
			{
				int iRow=pTBlurryTree->elemsArray.GetAt(i);
				for(int j=i+1;j<nRow;j++)
				{
					int jRow=pTBlurryTree->elemsArray.GetAt(j);
					if(!compareij(R_matrix,iRow,jRow))
					{
						flag=1;
						break;
					}
				}
				if(flag==1)
				{
					break;
				}
			}
			if(flag==0)//�����ȫ��ȡ�
			{
				pTBlurryTree->bBottom=false;
				pTBlurryTree->dCredit=1;
				pTBlurryTree->yPosition=-1;
				pTBlurryTree->bRead=false;
				for(int i=0;i<nRow;i++)//������ײ�ڵ�
				{
					TBlurryTree *p=new TBlurryTree;
					p->bBottom=true;
					p->bRead=false;
					p->dCredit=1;
					p->elemsArray.Add(node.elemsArray.GetAt(i));
					p->yPosition=-1;
					pTBlurryTree->NodePtrArray.Add((void *)p);
				}
			}
			else//�������ȫ���
			{
				if(m_bIsToEnd)//�û�ѡ������ֹ����
				{
					pTBlurryTree->bBottom=true;
					pTBlurryTree->bRead=false;
					pTBlurryTree->dCredit=1;
					pTBlurryTree->yPosition=-1;
				}
				else
				{

					pTBlurryTree->bBottom=false;
					pTBlurryTree->bRead=false;
					double dLowCredit=node.dCredit;
					double dHiCredit=1;
					CPtrArray classfyPtrArray;
					pTBlurryTree->yPosition=-1;
					pTBlurryTree->dCredit=MiddleClassfy(classfyPtrArray,dLowCredit,dHiCredit,pTBlurryTree->elemsArray,R_matrix,1);
					CreateTree(R_matrix,*pTBlurryTree,classfyPtrArray);
					delCPtrArray(classfyPtrArray);
				}
			}
		}
	}
}
//****************************************************************
//2003.1.6 
//zkc
//�õ�������ϵͼ��Y������˳�򣬲������洢������
//******************************************************************
void CBlurryCluster::GetYPositionArray(TBlurryTree &node,
									   CTArray<int,int> &OrderedElemsArray)////������������ʱ����ȡ��ײ�ӵ�Ԫ�أ���¼�úţ����Ⱥ�˳��洢�����еļ�¼�š�
{
	if(node.bBottom)
	{
		int temp=node.elemsArray.GetAt(0);
		OrderedElemsArray.Add(temp);
	}
	else
	{
		int nSize=node.NodePtrArray.GetSize();
		for(int i=0;i<nSize;i++)
		{

			TBlurryTree *p=(TBlurryTree *)(node.NodePtrArray.GetAt(i));
			GetYPositionArray(*p,OrderedElemsArray);
		}
	}
}

//*******************************************************************************************************
//2003.1.7
//zkc
//ȡ��ָ��ֵ��˳��
//*******************************************************************************************************

int CBlurryCluster::GetOrdinalPosition(CTArray<int,int> &OrderedElemsArray,////������������ʱ����ȡ��ײ�ӵ�Ԫ�أ���¼�úţ����Ⱥ�˳��洢�����еļ�¼�š�
									   int value)//ָ����Ԫ�أ��кţ�
{
	int nSize=OrderedElemsArray.GetSize();
	for(int i=0;i<nSize;i++)
	{
		if(value==OrderedElemsArray.GetAt(i))
			return i+1;
	}
	return -1;
}


//*******************************************************************************************************
//2003.1.7
//zkc
//�������ÿ���ڵ�yPosition��ֵ�͵õ�dCredit��ֵ
//*******************************************************************************************************

void CBlurryCluster::CompleteTreeYPositionValue(TBlurryTree &node,
												CTArray<int,int> &OrderedElemsArray)////������������ʱ����ȡ��ײ�ӵ�Ԫ�أ���¼�úţ����Ⱥ�˳��洢�����еļ�¼�š�
{
	if(node.bBottom)
		node.yPosition=GetOrdinalPosition(OrderedElemsArray,node.elemsArray.GetAt(0));//�������ÿ���ڵ�yPosition��ֵ
	else
	{
		m_dCreditArray.Add(node.dCredit); //�洢dCredit��ֵ
		int nSize=node.NodePtrArray.GetSize();
		double sum=0;
		for(int i=0;i<nSize;i++)
		{
			TBlurryTree *p=(TBlurryTree *)(node.NodePtrArray.GetAt(i));
			CompleteTreeYPositionValue(*p,OrderedElemsArray);
			sum=sum+p->yPosition;
		}
		node.yPosition=sum/nSize;////�������ÿ���ڵ�yPosition��ֵ��Ϊ��ͼ����
	}
}
//*******************************************************************************************************
//2003.1.7
//zkc
//���ڻ��ƾ�����ϵͼ��������δʹ��
//*******************************************************************************************************
/*
void CBlurryCluster::DrawBlurryTree(TBlurryTree &node/ *,CDC *pDC* /)
{
	if(node.bBottom)
		return;
	else
	{
		int nSize=node.NodePtrArray.GetSize();
		POINT NodePoint;
		NodePoint.x=int((node.dCredit)*100);
		NodePoint.y=int((node.yPosition)*100);
		for(int i=0;i<nSize;i++)
		{
			TBlurryTree *p=(TBlurryTree *)(node.NodePtrArray.GetAt(i));
			POINT SubPoint;
			SubPoint.x=int((p->dCredit)*100);
			SubPoint.y=int((p->yPosition)*100);
			pDC->MoveTo(NodePoint);
			pDC->LineTo(NodePoint.x,SubPoint.y);
			pDC->LineTo(SubPoint);
			DrawBlurryTree(*p/ *,pDC* /);			
		}
	}
}
*/


//*******************************************************************************************************
//2003.1.7
//zkc
//�洢ָ������ˮƽ�Ľڵ�����ȱ����ĵ�һ���ӽڵ������Ԫ�أ��кŻ��¼�ţ�
//*******************************************************************************************************


bool CBlurryCluster::GetClassfyState(TBlurryTree &node,//����ĳ���ڵ�
									 double dCredit,//ָ��������ˮƽ
									 CPtrArray &classPtrAr,//�洢ָ������ˮƽ�Ľڵ�����ȱ����ĵ�һ���ӽڵ������Ԫ�أ��кŻ��¼�ţ�
									 CTArray<int,int>&class2Array)//����δ��
{
	if(m_bIsToEnd)//�û���ֹ�˳���
		return true;
	if(node.dCredit==dCredit&&(!node.bBottom)&&(node.bRead==false))
	{
		node.bRead=true;
		for(int m=0;m<node.NodePtrArray.GetSize()-1;m++)
		{
			TBlurryTree *pBlurryTree=(TBlurryTree *)(node.NodePtrArray.GetAt(m));
			CTArray<int,int> *pClassArray=new CTArray<int,int>;
			for(int i=0;i<pBlurryTree->elemsArray.GetSize();i++)
				pClassArray->Add(pBlurryTree->elemsArray.GetAt(i));
			classPtrAr.Add((void *)pClassArray);
		//	pBlurryTree=(TBlurryTree *)(node.NodePtrArray.GetAt(1));
		//	for(i=0;i<pBlurryTree->elemsArray.GetSize();i++)
		//		class2Array.Add(pBlurryTree->elemsArray.GetAt(i));
		}
		return true;
	}
	else
	{
		for(int i=0;i<node.NodePtrArray.GetSize();i++)
		{
			TBlurryTree *pBlurryTree=(TBlurryTree *)(node.NodePtrArray.GetAt(i));
			if(GetClassfyState(*pBlurryTree,dCredit,classPtrAr,class2Array))
				return true;
		}
	}
	return false;
}


//*******************************************************************************************************
//2003.1.7
//zkc
//ĳ������ˮƽ�µķ���������洢��LinkerStrArray��
//*******************************************************************************************************
bool CBlurryCluster::ChangeClassLinker(TBlurryTree &node,//���ĸ��ڵ�
									   CTStringArray &LinkerStrArray,//������ʾ�����������Ϣ���ڱ���ͼ�У���1��ʾ�����ߵ�case���кţ�Ϊһ�࣬Ϊ0��Ϊһ�ࡣ
									   CTArray<int,int> &OrderedElemsArray,//������������ʱ����ȡ��ײ�ӵ�Ԫ�أ���¼�úţ����Ⱥ�˳��洢�����еļ�¼�š�
									   double dCredit) //����ˮƽ
{
	CTArray<int,int>class2Array,resultArray;
	CPtrArray classPtrAr,resultPtrAr;
	GetClassfyState(node,dCredit,classPtrAr,class2Array);
	int kk;
	for( kk=0;kk<classPtrAr.GetSize();kk++)
	{
		CTArray<int,int> *pClass1Array=(CTArray<int,int> *)(classPtrAr.GetAt(kk));
		CTArray<int,int> *pResultArray=new CTArray<int,int>;
		for(int i=0;i<OrderedElemsArray.GetSize();i++)//��class1Array��Ԫ�ذ���OrderedElemsArray�ж�Ӧ��Ԫ�س��ֵ��Ⱥ�˳������
		{												//���������洢��resultArray
			int temp=OrderedElemsArray.GetAt(i);
			for(int j=0;j<pClass1Array->GetSize();j++)
			{
				if(temp==pClass1Array->GetAt(j))
				{
					pResultArray->Add(temp);
					break;
				}
			}
		}
		resultPtrAr.Add((void *)pResultArray);
	}
	for(kk=0;kk<resultPtrAr.GetSize();kk++)
	{
		CTArray<int,int> *pResultArray=(CTArray<int,int> *)(resultPtrAr.GetAt(kk));
		int nPosition=0;
		int temp=0;
		if(pResultArray->GetSize()>=1)
			temp=pResultArray->GetAt(pResultArray->GetSize()-1);//�õ�resultArray��β��һ��Ԫ����OrderedElemsArray�е�λ��
		for(int i=0;i<OrderedElemsArray.GetSize();i++)			//��λ�ø�ֵ0����ʾ���˵�case�Ѿ���Ϊͬһ����
		{
			if(temp==OrderedElemsArray.GetAt(i))
			{
				nPosition=i;
				break;
			}
		}
		LinkerStrArray.SetAt(nPosition,_T(" "));
	}
	for(kk=0;kk<classPtrAr.GetSize();kk++)
	{
		CTArray<int,int> *p=(CTArray<int,int> *)(classPtrAr.GetAt(kk));
		CTArray<int,int> *p2=(CTArray<int,int> *)(resultPtrAr.GetAt(kk));
		delete p;
		delete p2;
	}
	resultArray.RemoveAll();
	class2Array.RemoveAll();
	return true;
}
//***************************************************************
//
//���º���Ϊ��ʵ�ֱȽ϶�ļ�¼������ȫ��ͬ������ģ���������������߷���Ч��
//����¼��������ʱ�򣬻��¼����ͬ�����ļ�¼����ʱ������Ч�ʷ����ή�͡�
//
//*****************************************************************


//************************************************************************
//2002.12.27 zkc
//���޳����ظ�ֵͬ��¼׷�ӽ�����
//group  �޳����ظ���¼����ͨ�����ϵ�ṹ
//classfy ԭʼ�����ϵ�����δ�޳��ظ���ֵͬ��¼��
//OutPutCPtrArray ׷���޳��˵�ֵͬ��¼�����ͨ�����ϵ�ṹ
//************************************************************************

void CBlurryCluster::RecoveryOrgData(CPtrArray &group,CPtrArray &classfy,CPtrArray &OutPutCPtrArray)
{
	int nSize=group.GetSize();
	for(int i=0;i<nSize;i++)
	{
		CPtrArray *pSubGroup=(CPtrArray *)(group.GetAt(i));
		int nSize1=pSubGroup->GetSize();
		CPtrArray *pSubOutPut=new CPtrArray;
		CTArray<int,int> dataArray;
		for(int j=0;j<nSize1;j++)//�ۼ�ĳһ�������Ԫ�أ���¼�Ż��кţ�
		{
			int temp1=*((int*)(pSubGroup->GetAt(j)));//group ��classfy��������ϵ
			int nCount=classfy.GetSize();
			CPtrArray *pSubclassfy=(CPtrArray *)(classfy.GetAt(temp1));
			int nCount1=pSubclassfy->GetSize();
			for(int n=0;n<nCount1;n++)
			{
				int addvalue=*((int*)(pSubclassfy->GetAt(n)));
				dataArray.Add(addvalue);
			}			
		}
		makeOrder(dataArray);//��������
		int nDataArraySize=dataArray.GetSize();
		for(int m=0;m<nDataArraySize;m++)//����һ���ڵ㣬�洢ĳһ��ķ�����Ϣ��
		{
			int *pValue=new int(dataArray.GetAt(m));
			pSubOutPut->Add((void *)pValue);
		}
		dataArray.RemoveAll();
		OutPutCPtrArray.Add((void *)pSubOutPut);
	}
}

//***********************************************
//2003.1.7
//zkc
//ɾ�������ͷ��ڴ�
//node Ϊ���ڵ�
void CBlurryCluster::DeleteTree(TBlurryTree &node)
{
	for(int i=0;i<node.NodePtrArray.GetSize();i++)
	{
		TBlurryTree *p=(TBlurryTree *)(node.NodePtrArray.GetAt(i));
		if(p->bBottom)
		{
			p->elemsArray.RemoveAll();
			delete p;
		}
		else
		{
			DeleteTree(*p);
			delete p;
		}
	}
	node.NodePtrArray.RemoveAll();

}
//************************************************************************
//2002.12.27 
//zkc
//���ݷ��������������ظ��ļ�¼���޳���һ�����н϶��ظ���¼ʱ����߷���Ч�ʣ�
//���⣬����Ʒ���ͼʱ������Ҫ�޳��ظ���¼�������ʱ���ڰ��޳��ļ�¼׷�ӽ�����
//classfy ���������
//matrix ԭʼ���ݼ�¼
//************************************************************************

CIntMatrix CBlurryCluster::CreateNewMatrix(CPtrArray &classfy,CIntMatrix matrix)
{
	int nCol=matrix.mcol();
	int nRow=classfy.GetSize();
	CIntMatrix TempMatrix(nRow,nCol,0);
	for(int i=0;i<nRow;i++)
	{
		CPtrArray *p=(CPtrArray *)(classfy.GetAt(i));
		int	nMatrixRow=*((int *)(p->GetAt(0)));//�ж���ظ���¼ʱ��ֻȡ��һ����¼
		for(int j=0;j<nCol;j++)
			TempMatrix(i,j)=matrix(nMatrixRow,j);
	}
	double a=PI;
	return TempMatrix;

}

/*
//************************************************************************
//2002.12.27 zkc
//ģ������
//�����ֶ�����str���ҵ���Ӧ���ֶ�������nNum���ֶ�����nTpye
//�ú�����δʹ�ã�����Ԥ����
//nTpye=
//fInt,"����"),
//fDouble,"������",
//fString,"�ַ���",
//fBoolean,"������",
//fDate,"������",
//fCurrency,"������"
//************************************************************************
void CBlurryCluster::FindFieldNumAndTpye(int &nNum,int &nTpye,CTString str)
{
	if(m_pDataInterface && m_pDataInterface->GetColCount() != 0 )
	{	
		for(int i=0;i<m_pDataInterface->GetColCount();i++)
		{
			CTString str2=m_pDataInterface->GetFieldName(i);
			if(str==str2)
			{
				nNum=i;
				break;
			}
		}
		nTpye = m_pDataInterface->GetFieldType(i);
	}
}


//************************************************************
//�����кź���ֵ������ӳ���ҵ�ԭʼ��Ӧ�ַ���ֵ
//nValue��ӳ��ת���������ֵ��
//nCol��������
//************************************************************
int CBlurryCluster::GetMapOrgValue(int nValue,int nCol)
{
	CTArray<int,int> *p=(CTArray<int,int> *)(m_MapPtrArray.GetAt(nCol));
	return p->GetAt(nValue);
}

 */



////////////////////////////////////////
///��㴫�ݣ� ��������node���ݸ�ͼ�ζ��������treeNode
///�ɹ�����true
///author: mxm  2004-5-18
bool CBlurryCluster::CreatePedigree(PedigreeTreeNode &treeNode, TBlurryTree &node)
{

	treeNode.bLeaf     =node.bBottom;
	treeNode.dCredit   =node.dCredit;
	treeNode.yPosition =node.yPosition;
	if(treeNode.bLeaf && node.elemsArray.GetSize()>0)
		treeNode.elemsCode =node.elemsArray.GetAt(0);
	
	for(int i=0;i<node.NodePtrArray.GetSize();i++)
	{
		TBlurryTree *p=(TBlurryTree *)(node.NodePtrArray.GetAt(i));
		PedigreeTreeNode *pSubNode=new PedigreeTreeNode;
		if(CreatePedigree(*pSubNode,*p))
			treeNode.NodePtrArray.Add(pSubNode);
	}
	return true;
}

//�����������ͼ����ϵͼ��ѡ��
//author: mxm 2004-05-21
void CBlurryCluster::SetOutSel(bool bIce, bool bPedigree)
{
	m_nOutSel=0;
	if(bIce)
	   m_nOutSel=1;
	if(bPedigree )
	{
		if(m_nOutSel==1)
			m_nOutSel=3;
		else
			m_nOutSel=2;
	}	
}
