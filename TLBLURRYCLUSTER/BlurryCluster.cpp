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
std::string CBlurryOutput::Export()//导出为XML方式
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
    //取回数据，组织输入数据矩阵
    int nCaseNumb,m = tVarInfo.iCount;
    TDataSet * pDataArray=NULL;
    int nColNum=0;
    //数据检验
	pData = new int [pDataInterface->GetRowCount() * m ];
	if ( tVarInfo.iCount <= 0 )
    {
        CTString str="你选择的数据无效!";
		if(m_pRsltElementText==NULL)
		{
			m_pRsltElementText=new CRsltElementText("警告");
		}
		m_pRsltElementText->AddString(str);
		m_bIsToEnd=true;
		return pData;
    }
    for(int i=0;i<m;i++)
    {
		if(m_bIsToEnd)
		{
			CTString str="您选择了终止程序，分析无效！";
			if(m_pRsltElementText==NULL)
			{
				m_pRsltElementText=new CRsltElementText("警告");
			}
			m_pRsltElementText->AddString(str);
			break;
		}
        nColNum=tVarInfo.pValue[i];
        nCaseNumb = pDataInterface->GetColData(nColNum,pDataArray,1);//取回数据
        assert(nCaseNumb > 0 );
        //赋值
        //检查输入的数据，如果样本值相同就剔除
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
		if(p->strRole==CTString("输入变量"))
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

//////////////////计算矩阵第i列的平均值////////////////
double CBlurryCluster::myMean(CDoubleMatrix matrix,int i)
{
	double sum=0.0;
	for(int j=0;j<matrix.mrow();j++)
		sum=sum+matrix(j,i);
	sum=sum/matrix.mrow();
	return sum;
}

//////////////////计算矩阵第i列方差//////////////////////////
double CBlurryCluster::variance(CDoubleMatrix matrix,int i)
{
	double average=myMean(matrix,i);
	double sum=0.0;
	for(int j=0;j<matrix.mrow();j++)
		sum=sum+(matrix(j,i)-average)*(matrix(j,i)-average);
	return sum;
}

//////////////////计算矩阵第i行与第j行的乘积//////////////
double CBlurryCluster::Xij(CDoubleMatrix matrix,int i,int j)
{
	double sum=0.0;
	for(int k=0;k<matrix.mcol();k++)
	{
		sum=sum+matrix(i,k)*matrix(j,k);
	}
	return sum;
}

/////////////////////计算rij////////////////////
//rij=(1.0+r*ij)/2.0;r*ij=(Xi.Xj')/sqrt((Xi.Xi')(Xj.Xj'))
//Xi,Xj表示行向量，Xi',Xj'表示对应的行向量的转阵。
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

///////////计算矩阵的平方//////////////////////////
//返回矩阵temp_matrix[i][j]就为：矩阵matrix的地i行与第j列对应元素相比较的最小值，在所有这些
//最小值中的最大值
bool CBlurryCluster::matrixMultiply(CDoubleMatrix &matrix,CDoubleMatrix &temp_matrix)
{
	//所传进来的矩阵的行列相等
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


///////////////比较两同维矩阵是否相等/////////////////////////
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

//////////////////////////比较矩阵的第i行j行是否相等/////////////
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

/////////////比较矩阵的第j列元素是否全相等///////
bool CBlurryCluster::IsMatrixColEqual(CDoubleMatrix matrix,int j)
{
	for(int i=1;i<matrix.mrow();i++)
	{
		if((int)(matrix(0,j))!=(int)(matrix(i,j)))
			return false;
	}
	return true;
}

//主函数，调用其它函数，完成输出功能。
bool CBlurryCluster::MatrixCreate(CIntMatrix &evalue_matrix,CIntMatrix &grade_matrix,double lambda)
{
/*
	if(m_pProcedureWnd&&(!m_bIsToEnd))
	{
		CTString str1="正在构造相似性矩阵";
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
					m_pRsltElementText=new CRsltElementText("警告");
				CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);		
				str+="等级值不能小于1！";
				m_pRsltElementText->AddString(str);
				return false;
			}
			else
			{
				if(m_pRsltElementText==NULL)
				{
					m_pRsltElementText=new CRsltElementText("警告");
					CTString str="您选择了终止程序，分析无效！";
					m_pRsltElementText->AddString(str);
					
				}
				return false;

			}
		}
		nCol=nCol+(int)(grade_matrix(0,i)-1);
	}
//////判断评分，如果其值小于0（标准最低分），赋值为0（标准最低分），如果大于标准最高分，赋值为最高分。
	for(i=0;i<evalue_matrix.mrow();i++)
	{
		for(int j=0;j<evalue_matrix.mcol();j++)
		{		
			if(evalue_matrix(i,j)<0||evalue_matrix(i,j)>grade_matrix(0,j)-1)
			{
				if(!m_bIsToEnd)
				{

					if(m_pRsltElementText==NULL)
						m_pRsltElementText=new CRsltElementText("警告");
					CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[j]);
					str+="取值必须大于等于0，且小于等级值！";
					m_pRsltElementText->AddString(str);
					return false;
				}
				else
				{
					if(m_pRsltElementText==NULL)
					{
						m_pRsltElementText=new CRsltElementText("警告");
						CTString str="您选择了终止程序，分析无效！";
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

	///根据evalue_matrix and grade_matrix构造initBinary_matrix矩阵
	int k=0;
	int n=0;
	int sum=0;
	//WriteData(evalue_matrix,"evalue_Matrix");
	for(i=0;i<grade_length;i++)
	{
		
		for(n=0;n<nRow;n++)//row number
		{			
			for(k=0;k<(int)evalue_matrix(n,i);k++)//列的相对位置
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
	
	//数据变换x[i][j]=(x[i][j]-E(Xj))/(sqrt(1.0/(nRow-1)*D(Xj)))
	//E表示平均值，D表示方差，Xj表示矩阵的第j列向量。
	for(i=0;i<initBinary_matrix.mrow();i++)
	{
		for(int j=0;j<initBinary_matrix.mcol();j++)
		{
			if(IsMatrixColEqual(tempMatrix,j))//如果第j列元素全相等，不改变值
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
		CTString str2="正在对模糊矩阵进行褶积运算";
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
	///存储分类信息
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
	//////////////为图形输出计算	
	TBlurryTree node;//根节点
	CPtrArray classfyPtrArray;
	double dLowCredit=0;
	double dHiCredit=1;
	CTArray<int,int> OrderedElemsArray;//按遍历树读到最底层的行号的先后顺序存储行号。

//	CTArray<int,int> elemsArray;
	int nMatrixRow=R_matrix.mrow();
	if(m_nDataSize<=MAXGRAPH&&(!m_bIsToEnd))
	{
/*
		if(m_pProcedureWnd&&(!m_bIsToEnd))
		{
			CTString str4="正在聚类！";
			m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str4));
		}
*/

		for(i=0;i<nMatrixRow;i++)//建聚类树
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
		//不是底层节点的置信水平按从小到大进行排序
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
		CTString str3="正在格式化输出";
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
	else  m_FirstClassfyPtrArray.RemoveAll();//清除内存已转移到result/CTChartPedigree中 [mxm modify it] 2004-05-21
	delCPtrArray(classfyPtrArray);
	OrderedElemsArray.RemoveAll();
	return true;

}


///////结果输出//////////////////
void CBlurryCluster::CreateResult(TBlurryTree &node,CDoubleMatrix &R_matrix,CDoubleMatrix &blurry_matrix,CPtrArray & group,CTArray<int,int> &OrderedArray)
{
/*	///////////////输出R阵///////////////
	pTable->SetTitle("模糊相似阵");
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
	pETable= new CRsltElementTable("模糊相似阵",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);

///////////////////输出R_lambda阵///////////////////////
	pTable = new CTLTable;
	pTable->SetTitle("分类关系阵");
	CTString strLambda;
	strLambda.Format("置信水平=%.3f",m_dLambda);
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
	pETable= new CRsltElementTable("分类关系阵",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
*/

//////////////////////////输出分类表////////////////////////////
	//得到最大的列长。
	int nRow=group.GetSize();
	int k=0;
	int nCol=1;
//	m_pResult = new CResult("模糊聚类分析");
	CRsltVariable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("分类表");
	CTString strLambda;
	strLambda.Format("置信水平=%.3f",m_dLambda);
// 	pTable->SetCellAlignFormat(DT_LEFT);

	pTable->SetHead(strLambda);
	pTable->SetCols(nCol+1);
	pTable->SetRows(nRow+1);
	int i;
	for(i=0;i<nRow;i++)//插入行
	{
		CTString str;
		str.Format("类%d",i+1);
		pTable->InsertItem(i+1,str);
	}
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"分类记录");
	for(i=0;i<nRow;i++)//输出聚类信息
	{
		CPtrArray *pTempPtrArray=(CPtrArray *)(group.GetAt(i));
		int tempSize=pTempPtrArray->GetSize();
		CTString str;
		str.Empty();
	//	str+="记录号：";
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
	pETable= new CRsltElementTable("分类表",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);


	///绘图
	if(m_nOutSel>0)//输出冰柱图或谱系图 [mxm add it] 2004-05-21
	{

	CPtrArray LinkerPtrArray;
	CTArray<int,int> OrderedArray2;
	if(m_nDataSize<=MAXGRAPH&&(!m_bIsToEnd))
	{
		pTable = new CTLTable;
		pTable->SetTitle("冰柱分类图");
		nRow=m_dCreditArray.GetSize();
		nCol=OrderedArray.GetSize();
		CTStringArray LinkerStrArray;
		CTStringArray *pStrArray=new CTStringArray;
		for(i=0;i<nCol-1;i++)
		{
			LinkerStrArray.Add("X");////////////////////
			pStrArray->Add("X");//1表示聚为一类，0表示不聚为一类////////////
		}
		//得到聚类关系，并把它们存储在LinkerPtrArray中
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


		///映射回原始的行号。
		for(i=0;i<OrderedArray.GetSize();i++)
		{
			int	nColIndex=OrderedArray.GetAt(i);
			CPtrArray *p=(CPtrArray *)(m_FirstClassfyPtrArray.GetAt(nColIndex));
			int temp=*((int *)(p->GetAt(0)));
			OrderedArray2.Add(temp);
		}

	//输出置信水平值
		CTString strCreditHead="\n";
		int count=0;
		for(i=0;i<m_dCreditArray.GetSize()+1;i++)
		{
			CTString str;
			if(i==0)
			{		
				str.Format("置信水平%d<%.4f;  ",i+1,m_dCreditArray.GetAt(0));
			}
			else
				str.Format("置信水平%d=%.4f;  ",i+1,m_dCreditArray.GetAt(i-1));
			strCreditHead+=str;
			count++;
			if(count==5)
			{
				strCreditHead+="\n\n";
				count=0;
			}
		}


		if(m_nOutSel==1 || m_nOutSel==3)//输出冰柱图的判别条件 [mxm add it] 2004-05-21
		{
		//把原始的行号追加到冰柱分类图中
			for(i=0;i<m_FirstClassfyPtrArray.GetSize();i++)
			{
				CPtrArray *pPtrArray=(CPtrArray *)(m_FirstClassfyPtrArray.GetAt(i));
				if(pPtrArray->GetSize()<=1)
					continue;
				else//说明有两个或两个以上同值记录。则追加
				{
					int nRowNum=*((int *)(pPtrArray->GetAt(0)));
					int nPosition=0;
					for(int n=0;n<OrderedArray2.GetSize();n++)//找到应插入的位置
					{
						if(nRowNum==OrderedArray2.GetAt(n))
						{
							nPosition=n;
							break;
						}
					}
					for(int j=1;j<pPtrArray->GetSize();j++)//在正确的位置插入前剔除的所有同值记录（行号）；
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

			
			for(i=0;i<2*nCol;i++)//插入列
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
			
			for(i=0;i<nRow+1;i++)//插入行
			{
				CTString str;
				str.Format("%d",i+1);
				pTable->InsertItem(i,str,false);
			}
			//插入元素值
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

			pETable= new CRsltElementTable("冰柱分类图",pTable);//申请一个表对象类的对象
			m_pResult->Add(pETable);
		}
		
		///{{谱系图生成 //[mxm add following]
		if(m_nOutSel>=2)
		{
			if(m_nOutSel==2)
			{
				CRsltElementText * pETXT = new CRsltElementText("置信水平值");
				pETXT->AddString("置信水平值:\n");				
				pETXT->AddString(strCreditHead+"\n");
				m_pResult->Add(pETXT);
			}
			double dMinX = 0;
			PedigreeTreeNode treenode;
			double dMaxX  = 1-m_dCreditArray.GetAt(0);
			int nCount = OrderedArray.GetSize();
			CTChartPedigree *pTChart = new CTChartPedigree(false);
			CreatePedigree(treenode,node);
			pTChart->SetTitle("谱系图");
			pTChart->SetXAxilLabel("置信水平");
			pTChart->SetYAxilLabel("分类单元");
			pTChart->SetValue(dMinX,dMaxX,0,nCount,&treenode,m_FirstClassfyPtrArray);
			CRsltElementChart * pEChart = new CRsltElementChart("谱系图",pTChart);
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
	m_pResult = new CResult("模糊聚类分析");
/*
	if(m_pProcedureWnd)
	{
		CTString str="正在导入数据和预处理";
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
			m_pRsltElementText=new CRsltElementText("警告");
		CTString str="\n没有有效分析变量！";
		m_pRsltElementText->AddString(str);
		m_pResult->Add(m_pRsltElementText);
		return m_pResult;

	}
	if(n==0)
	{
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		CTString str="\n数据为空，请另选数据！";
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
					m_pRsltElementText=new CRsltElementText("警告");
				CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
				str+="等级数不应大于100！";
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
			
				m_pRsltElementText=new CRsltElementText("警告");
			 CTString str="数据源过大，建议先对样本进行抽样或离散化！";
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
					m_pRsltElementText=new CRsltElementText("警告");
				str="所有有效记录均完全相同,总是聚为一类！";
				m_pRsltElementText->AddString(str);
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;
			}
			else
			{
				if(m_pRsltElementText==NULL)
				{
					m_pRsltElementText=new CRsltElementText("警告");
					str="您选择了终止程序，分析无效！";
					m_pRsltElementText->AddString(str);
					
				}	
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;

			}
		}
		evalue_matrix=CreateNewMatrix(m_FirstClassfyPtrArray,evalue_matrix);///只保留值完全相同一系列记录的第一个
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
					m_pRsltElementText=new CRsltElementText("警告");
				CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
				str+="等级数不应大于100！";
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
				m_pRsltElementText=new CRsltElementText("警告");
			 CTString str="数据源过大，建议先对样本进行抽样或离散化！";
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
					m_pRsltElementText=new CRsltElementText("警告");
				str="所有有效记录均完全相同,总是聚为一类！";
				m_pRsltElementText->AddString(str);
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;
			}
			else
			{
				if(m_pRsltElementText==NULL)
				{
					m_pRsltElementText=new CRsltElementText("警告");
					str="您选择了终止程序，分析无效！";
					m_pRsltElementText->AddString(str);
				}
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;

			}
		}
		evalue_matrix=CreateNewMatrix(m_FirstClassfyPtrArray,evalue_matrix);//只保留值完全相同一系列记录的第一个
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
//			has the same value with the str。
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
//strArray1 存储一列字符串
//strArray2 存储该列字符串不同值的字符串
//index  指定列
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

	//对intArray2进行小到大排序
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
//把指定的列nDataCol数据映射成整数数据存储在matrix中指定的nMatrixCol；
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


//取得数据指定列的最大值
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
//矩阵进行分类，不同记录号的记录具有完全相同值，那么归为一类；
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
//删除分类树；
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
//用于绘制聚类谱系图的分类方法
//
//************************************************************************
//对数组进行从小到大的顺序排序
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
//递归对树节点进行分类，
//************************************************************************
double CBlurryCluster::MiddleClassfy(CPtrArray &classfyPtrArray,//用于分类元素的存储
								   double &dLowCredit,//中值法小值
								   double &dHiCredit,//中值法大值
								   CTArray<int,int> &elemsArray,//某节点的元素（记录），对应模糊分类关系矩阵的行号
								   CDoubleMatrix &R_matrix,int nSubSize)	//模糊分类关系矩阵				   
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
	CIntMatrix classfyMatrix(nRow,nCol);//由该节点的元素构成的矩阵；	
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
//张克臣
//建立分类树
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
		if(nRow==1)//就1个元素
		{
			pTBlurryTree->bBottom=true;
			pTBlurryTree->dCredit=1;
			pTBlurryTree->yPosition=-1;
			pTBlurryTree->bRead=false;
		}
		else    //先判断该元素（记录行）对应的分类关系矩阵对应的行是否完全相等
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
			if(flag==0)//如果完全相等。
			{
				pTBlurryTree->bBottom=false;
				pTBlurryTree->dCredit=1;
				pTBlurryTree->yPosition=-1;
				pTBlurryTree->bRead=false;
				for(int i=0;i<nRow;i++)//增加最底层节点
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
			else//如果不完全相等
			{
				if(m_bIsToEnd)//用户选择了终止程序
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
//得到聚类谱系图的Y的排列顺序，并把它存储起来。
//******************************************************************
void CBlurryCluster::GetYPositionArray(TBlurryTree &node,
									   CTArray<int,int> &OrderedElemsArray)////按遍历分类树时，读取最底层接点元素（记录好号）的先后顺序存储了所有的记录号。
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
//取得指定值的顺序
//*******************************************************************************************************

int CBlurryCluster::GetOrdinalPosition(CTArray<int,int> &OrderedElemsArray,////按遍历分类树时，读取最底层接点元素（记录好号）的先后顺序存储了所有的记录号。
									   int value)//指定的元素（行号）
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
//填充树中每个节点yPosition的值和得到dCredit的值
//*******************************************************************************************************

void CBlurryCluster::CompleteTreeYPositionValue(TBlurryTree &node,
												CTArray<int,int> &OrderedElemsArray)////按遍历分类树时，读取最底层接点元素（记录好号）的先后顺序存储了所有的记录号。
{
	if(node.bBottom)
		node.yPosition=GetOrdinalPosition(OrderedElemsArray,node.elemsArray.GetAt(0));//填充树中每个节点yPosition的值
	else
	{
		m_dCreditArray.Add(node.dCredit); //存储dCredit的值
		int nSize=node.NodePtrArray.GetSize();
		double sum=0;
		for(int i=0;i<nSize;i++)
		{
			TBlurryTree *p=(TBlurryTree *)(node.NodePtrArray.GetAt(i));
			CompleteTreeYPositionValue(*p,OrderedElemsArray);
			sum=sum+p->yPosition;
		}
		node.yPosition=sum/nSize;////填充树中每个节点yPosition的值，为画图服务
	}
}
//*******************************************************************************************************
//2003.1.7
//zkc
//用于绘制聚类谱系图，保留在未使用
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
//存储指定置信水平的节点的首先遍历的第一个子节点的所有元素（行号或记录号）
//*******************************************************************************************************


bool CBlurryCluster::GetClassfyState(TBlurryTree &node,//树的某个节点
									 double dCredit,//指定的置信水平
									 CPtrArray &classPtrAr,//存储指定置信水平的节点的首先遍历的第一个子节点的所有元素（行号或记录号）
									 CTArray<int,int>&class2Array)//保留未用
{
	if(m_bIsToEnd)//用户终止了程序
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
//某个置信水平下的分类情况。存储在LinkerStrArray中
//*******************************************************************************************************
bool CBlurryCluster::ChangeClassLinker(TBlurryTree &node,//树的跟节点
									   CTStringArray &LinkerStrArray,//用于显示分类情况的信息，在冰柱图中，用1表示，两边的case（行号）为一类，为0不为一类。
									   CTArray<int,int> &OrderedElemsArray,//按遍历分类树时，读取最底层接点元素（记录好号）的先后顺序存储了所有的记录号。
									   double dCredit) //置信水平
{
	CTArray<int,int>class2Array,resultArray;
	CPtrArray classPtrAr,resultPtrAr;
	GetClassfyState(node,dCredit,classPtrAr,class2Array);
	int kk;
	for( kk=0;kk<classPtrAr.GetSize();kk++)
	{
		CTArray<int,int> *pClass1Array=(CTArray<int,int> *)(classPtrAr.GetAt(kk));
		CTArray<int,int> *pResultArray=new CTArray<int,int>;
		for(int i=0;i<OrderedElemsArray.GetSize();i++)//将class1Array的元素按照OrderedElemsArray中对应的元素出现的先后顺序排序。
		{												//将排序结果存储在resultArray
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
			temp=pResultArray->GetAt(pResultArray->GetSize()-1);//得到resultArray最尾的一个元素在OrderedElemsArray中的位置
		for(int i=0;i<OrderedElemsArray.GetSize();i++)			//此位置赋值0，表示两端的case已经不为同一类了
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
//以下函数为了实现比较多的记录具有完全相同特征的模糊聚类分析，以提高分析效率
//当记录个数较少时候，或记录中相同特征的记录较少时，运行效率反而会降低。
//
//*****************************************************************


//************************************************************************
//2002.12.27 zkc
//把剔除的重复同值记录追加进来；
//group  剔除了重复记录后普通分类关系结构
//classfy 原始分类关系情况（未剔除重复的同值记录）
//OutPutCPtrArray 追加剔除了的同值记录后的普通分类关系结构
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
		for(int j=0;j<nSize1;j++)//累计某一类的所有元素（记录号或行号）
		{
			int temp1=*((int*)(pSubGroup->GetAt(j)));//group 与classfy的索引关系
			int nCount=classfy.GetSize();
			CPtrArray *pSubclassfy=(CPtrArray *)(classfy.GetAt(temp1));
			int nCount1=pSubclassfy->GetSize();
			for(int n=0;n<nCount1;n++)
			{
				int addvalue=*((int*)(pSubclassfy->GetAt(n)));
				dataArray.Add(addvalue);
			}			
		}
		makeOrder(dataArray);//进行排序
		int nDataArraySize=dataArray.GetSize();
		for(int m=0;m<nDataArraySize;m++)//增加一个节点，存储某一类的分类信息。
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
//删除树，释放内存
//node 为根节点
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
//根据分类结果创建矩阵；重复的记录先剔除，一是在有较多重复记录时，提高分析效率；
//另外，最绘制分类图时，必须要剔除重复记录，在输出时，在把剔除的记录追加近来。
//classfy 分类情况；
//matrix 原始数据记录
//************************************************************************

CIntMatrix CBlurryCluster::CreateNewMatrix(CPtrArray &classfy,CIntMatrix matrix)
{
	int nCol=matrix.mcol();
	int nRow=classfy.GetSize();
	CIntMatrix TempMatrix(nRow,nCol,0);
	for(int i=0;i<nRow;i++)
	{
		CPtrArray *p=(CPtrArray *)(classfy.GetAt(i));
		int	nMatrixRow=*((int *)(p->GetAt(0)));//有多个重复记录时，只取第一个记录
		for(int j=0;j<nCol;j++)
			TempMatrix(i,j)=matrix(nMatrixRow,j);
	}
	double a=PI;
	return TempMatrix;

}

/*
//************************************************************************
//2002.12.27 zkc
//模糊聚类
//根据字段名称str，找到相应的字段索引号nNum，字段类型nTpye
//该函数暂未使用，用于预处理
//nTpye=
//fInt,"整型"),
//fDouble,"浮点型",
//fString,"字符型",
//fBoolean,"布尔型",
//fDate,"日期型",
//fCurrency,"货币型"
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
//根据列号和数值，根据映射找到原始对应字符串值
//nValue被映射转化后的整数值；
//nCol列索引号
//************************************************************
int CBlurryCluster::GetMapOrgValue(int nValue,int nCol)
{
	CTArray<int,int> *p=(CTArray<int,int> *)(m_MapPtrArray.GetAt(nCol));
	return p->GetAt(nValue);
}

 */



////////////////////////////////////////
///结点传递： 将聚类结点node传递给图形二叉树结点treeNode
///成功返回true
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

//设置输出冰柱图或谱系图的选择
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
