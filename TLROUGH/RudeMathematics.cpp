// RudeMathematics.cpp: implementation of the CRudeMathematics class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RudeMathematics.h"
//#include "RougthSetDlg.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const UINT WM_CALCULATE_OVER = RegisterWindowMessage("WM_CALCULATE_OVER");
const UINT WM_PROGRESS_TEXT=::RegisterWindowMessage("WM_PROGRESS_TEXT");
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRudeMathematics::CRudeMathematics()
{
	m_bSpecified=false;
	m_bIsToEnd=false;
	m_pResult=NULL;
	m_pRsltElementText=NULL;
}

CRudeMathematics::~CRudeMathematics()
{
	for(int i=0;i<m_MapPtrArray.GetSize();i++)
	{
		CTStringArray *p=(CTStringArray *)(m_MapPtrArray.GetAt(i));
		p->RemoveAll();
		delete p;
	}
	m_MapPtrArray.RemoveAll();
	m_ArOutArray.RemoveAll();
	m_tVarInfo.DeleteObject();

}

CResult * CRudeMathematics::RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,CRoughtVO *pVO)
{
	m_pDataInterface = pDataInterface;
	m_pResult=NULL;
	m_pRsltElementText=NULL;
	m_bIsToEnd=false;
	m_bSpecified=false;
	m_tVarInfo.DeleteObject();
	m_ColIndexArray.RemoveAll();
	m_RecordStrArray.RemoveAll();
	m_DelArray.RemoveAll();
	m_ArOutArray.RemoveAll();
	CTArray<int,int> tempArray;
	for(int i=0;i<pVO->strInputName.GetSize();i++)
	{
		CTString strField = pVO->strInputName.GetAt(i);
		int nFieldNo = m_pDataInterface->IndexByName(strField);
		tempArray.Add(nFieldNo);
	}
	if (pVO->strspecInputName.GetSize()>0)
	{
		CTString strSpec = pVO->strspecInputName.GetAt(0);
		m_nSpecifiedIndex = m_pDataInterface->IndexByName(strSpec);
		m_bSpecified=true;
	}

	if (pVO->strDecName.GetSize()>0)
	{
		CTString strDec = pVO->strDecName.GetAt(0);
		m_nDecisionIndex= m_pDataInterface->IndexByName(strDec);
	}

	int count=tempArray.GetSize();
	int *pValue=new int[count];
	for(int ii=0;ii<count;ii++)
		pValue[ii]=tempArray.GetAt(ii);
	m_tVarInfo=TIntStruct(count,pValue);
	tempArray.RemoveAll();
	return OnRoughSet();
}


//////////比较矩阵的第i行j行对应元素（即具有相同的列号，不同的行号）是否相等/////////////
bool CRudeMathematics::compareij(CIntMatrix matrix,int i,int j,CTArray<int,int> &stdArray)
{
	int nSize=stdArray.GetSize();
	for(int k=0;k<nSize;k++)
	{
		int nCol=stdArray.GetAt(k);
		if(matrix(i,nCol)!=matrix(j,nCol))
			return false;
	}
	return true;
}
bool CRudeMathematics::compareij(CIntMatrix matrix,int i,int j)
{
	int nSize=matrix.mcol();
	for(int k=0;k<nSize;k++)
	{
		if(matrix(i,k)!=matrix(j,k))
			return false;
	}
	return true;
}
//////////////////////////to free the memory of the CPtrArray data//
void CRudeMathematics::delCPtrArray(CPtrArray &toDel)
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

//////to classfy the by the criterion of the stdCTArray and rowCTArray,and store the result into ClassfyCPtrArray.
///////rowCTArray stores the row numbers of W_matrix to be classfied.
//////the stdCTArray has stored the W_matrix's column numbers to be compared. if the different elements from the W_matrix
//////from different rows and the same column stored in the stdCTArray have the same value.then the different rows will be
////// classfied into one group.

void CRudeMathematics::classfy(CTArray<int,int> &stdCTArray,CTArray<int,int> &rowCTArray,CIntMatrix W_matrix,CPtrArray &ClassfyCPtrArray)
{
	delCPtrArray(ClassfyCPtrArray);
//////////to create the new matrix by the criterion of stdCTArray and rowCTArray.
	int nRow=W_matrix.mrow();
	int nCol=W_matrix.mcol();
	int nSize=stdCTArray.GetSize();
//	CIntMatrix R2_matrix(rowCTArray.GetSize(),nSize);
	CTArray<int,int> rowCTArray2;
	int i;
	for(i=0;i<rowCTArray.GetSize();i++)
	{
		rowCTArray2.Add(i);
	//	int m=rowCTArray.GetAt(i);
	//	for(int j=0;j<nSize;j++)
	//	{
		//	int n=stdCTArray.GetAt(j);
		//	R2_matrix[i][j]=W_matrix[m][n];
	//	}
	}
	for(i=0;i<rowCTArray2.GetSize();i++)
	{
		CPtrArray *subGroup=new CPtrArray;
		int *pTemp=new int(rowCTArray.GetAt(rowCTArray2.GetAt(i)));
		subGroup->Add((void *)pTemp);
		for(int j=i+1;j<rowCTArray2.GetSize();j++)
		{
			if(compareij(W_matrix,rowCTArray.GetAt(rowCTArray2.GetAt(i)),rowCTArray.GetAt(rowCTArray2.GetAt(j)),stdCTArray))
			{
				pTemp=new int(rowCTArray.GetAt(rowCTArray2.GetAt(j)));
				
				subGroup->Add((void *)pTemp);
				rowCTArray2.RemoveAt(j);
				j--;
			}		
			if(m_bIsToEnd)
				break;

		}
		ClassfyCPtrArray.Add((void *)subGroup);					
		if(m_bIsToEnd)
			break;		
	}
	rowCTArray2.RemoveAll();
	
}


///////////////to classfy the W_matrix on the basis of the stdCTArray,and store the result into ClassfyCPtrArray.
///////////////the stdCTArray has stored the W_matrix's column numbers to be compared. if the different elements from the W_matrix
//////////////from different rows and the same column stored in the stdCTArray have the same value.then the different rows will be
///////////// classfied into one group.


void CRudeMathematics::classfy(CTArray<int,int> &stdCTArray,CIntMatrix W_matrix,CPtrArray &ClassfyCPtrArray)
{
	delCPtrArray(ClassfyCPtrArray);
	int nRow=W_matrix.mrow();
	int nCol=W_matrix.mcol();
	int nSize=stdCTArray.GetSize();
	CTArray<int,int> rowArray;
	int i;
	for(i=0;i<nRow;i++)
	{
		rowArray.Add(i);
	}
	for(i=0;i<rowArray.GetSize();i++)
	{
		CPtrArray *subGroup=new CPtrArray;
		int *pTemp=new int(rowArray.GetAt(i));
		subGroup->Add((void *)pTemp);
		int iRow=rowArray.GetAt(i);
		for(int j=i+1;j<rowArray.GetSize();j++)
		{
			if(compareij(W_matrix,iRow,rowArray.GetAt(j),stdCTArray))
			{
				pTemp=new int(rowArray.GetAt(j));
				subGroup->Add((void *)pTemp);
				rowArray.RemoveAt(j);
				j--;
			}	
			if(m_bIsToEnd)//用户终止程序
				break;
		}
		ClassfyCPtrArray.Add((void *)subGroup);	
		if(m_bIsToEnd)//用户终止程序
			break;
	}	
	rowArray.RemoveAll();
}


////////////first free the memory of the dest's and delete the elements,then copy the src's elements into the dest's,
void CRudeMathematics::copyPtrArray(CPtrArray &dest,CPtrArray &src)
{
	delCPtrArray(dest);
	for(int i=0;i<src.GetSize();i++)
	{
		CPtrArray *pTempPtrArray=(CPtrArray *)(src.GetAt(i));
		int tempSize=pTempPtrArray->GetSize();
		CPtrArray *pSub=new CPtrArray;
		for(int j=0;j<tempSize;j++)
		{
			int value=*((int *)(pTempPtrArray->GetAt(j)));
			int *pt=new int(value);
			pSub->Add((void*)pt);
		}
		dest.Add((void *)pSub);
	}
}

///////////to decide if value is equal to one of the elements of the coreCTArray
///////////if not ,return true, else return false 

bool CRudeMathematics::notIn(int value,CTArray<int,int> &coreCTArray)
{
	for(int i=0;i<coreCTArray.GetSize();i++)
		if(value==coreCTArray.GetAt(i))
			return false;
	return true;

}

////////////to decide if minCTArray is the subset of the maxCTArray 
bool CRudeMathematics::IsIncluded(CTArray<int,int> &minCTArray,CTArray<int,int> &maxCTArray)
{
	for(int i=0;i<minCTArray.GetSize();i++)
	{
		int minvalue=minCTArray.GetAt(i);
		int flag=0;
		for(int j=0;j<maxCTArray.GetSize();j++)
		{
			int maxvalue=maxCTArray.GetAt(j);
			if(minvalue==maxvalue)
			{
				flag=1;
				break;
			}
			
		}
		if(flag==0)
			return false;
	}
	return true;
}
bool CRudeMathematics::IsIncluded(CPtrArray *pmin,CPtrArray *pmax)
{
	for(int i=0;i<pmin->GetSize();i++)
	{
		int minvalue=*(int *)((pmin->GetAt(i)));
		int flag=0;
		for(int j=0;j<pmax->GetSize();j++)
		{
			int maxvalue=*((int *)(pmax->GetAt(j)));
			if(minvalue==maxvalue)
			{
				flag=1;
				break;
			}
			
		}
		if(flag==0)
			return false;
	}
	return true;
}

///////////////if the element of src is not equal to every element of the dest,
////////////////then append it to the dest.
void CRudeMathematics::AppendToCTArray(CTArray<int,int> &dest,CTArray<int,int> &src)
{
	for(int i=0;i<src.GetSize();i++)
	{
		int srcvalue=src.GetAt(i);
		int flag=0;
		for(int j=0;j<dest.GetSize();j++)
		{
			int destvalue=dest.GetAt(j);
			if(srcvalue==destvalue)
			{
				flag=1;
				break;
			}
		}
		if(flag==0)
			dest.Add(srcvalue);
	}
}


/////////////////to delete the elements of the tempCTArray
void CRudeMathematics::delCTArray(CTArray<int,int> & tempCTArray)
{
	tempCTArray.RemoveAll();

}

//////////////first free the memory and delete the elements of b,then copy
/////////////the elements of a into b.
void CRudeMathematics::ToCTArray(CPtrArray * &a,CTArray<int,int> &b)
{
	delCTArray(b);
	for(int i=0;i<a->GetSize();i++)
	{
		int value=*((int *)(a->GetAt(i)));
		b.Add(value);
	}

}

//////////the min stores many sets,so does the max,if the set of min is the subset of one of
////////// the max's sets, then store elements of the set into tempCTArray.last statistic the 
/////////number of the different elements in tempCTArray.

int CRudeMathematics::statistics(CPtrArray &min,CPtrArray &max)
{
	CTArray<int,int> tempCTArray,minCTArray,maxCTArray;
	
	for(int i=0;i<min.GetSize();i++)
	{
	
		CPtrArray *p_min=(CPtrArray *)(min.GetAt(i));
		ToCTArray(p_min,minCTArray);//中间步骤，可以优化
		for(int j=0;j<max.GetSize();j++)
		{
			CPtrArray *p_max=(CPtrArray *)(max.GetAt(j));
			ToCTArray(p_max,maxCTArray);//中间步骤，可以优化
			if(IsIncluded(minCTArray,maxCTArray))
				AppendToCTArray(tempCTArray,minCTArray);
			if(m_bIsToEnd)//用户终止程序
				break;
		}
		if(m_bIsToEnd)//用户终止程序
			break;
	}
	int size=tempCTArray.GetSize();
	return size;


}

void CRudeMathematics::copyCTArray(CTArray<int,int> &dest,CTArray<int,int> &src)
{
	dest.RemoveAll();
	for(int i=0;i<src.GetSize();i++)
		dest.Add(src.GetAt(i));

}

/////////statistic the number of attribute of every column of W_matrix
void CRudeMathematics::storeGrade(CTArray<int,int> &grade,CIntMatrix W_matrix)
{
	CTArray<int,int> std;
	CPtrArray cpa;
	for(int i=0;i<W_matrix.mcol()-1;i++)
	{
		std.Add(i);
		classfy(std,W_matrix,cpa);
		grade.Add(cpa.GetSize());
		std.RemoveAll();
	}
	delCPtrArray(cpa);
}


///////////////to calculate the core////
void CRudeMathematics::CalcCore(CTArray<int,int> &colCTArray,CTArray<int,int> &rowCTArray,CTArray<int,int> &coreCTArray,CIntMatrix W_matrix)
{
	int nCol=W_matrix.mcol();
	int nRow=W_matrix.mrow();
	coreCTArray.RemoveAll();
	CPtrArray *pCPtrArray;
	for(int i=0;(i<rowCTArray.GetSize())&&(!m_bIsToEnd);i++)//行集合
	{
		for(int j=0;(j<colCTArray.GetSize())&&(!m_bIsToEnd);j++)//列集合
		{
			pCPtrArray=new CPtrArray;
			for(int n=0;(n<nCol-1)&&(!m_bIsToEnd);n++)
			{
				int jCol=colCTArray.GetAt(j);
				int iRow=rowCTArray.GetAt(i);
				if(W_matrix(jCol,n)!=W_matrix(iRow,n))
				{
					int *pInt=new int(n);
					pCPtrArray->Add((void*)pInt);
				}
				if(m_bIsToEnd)
					break;
			}
			if(pCPtrArray->GetSize()==1)//只有一个属性值时，才为核
			{
				int value=*((int *)(pCPtrArray->GetAt(0)));
				if(notIn(value,coreCTArray))
					coreCTArray.Add(value);
			}
			for(int k=0;k<pCPtrArray->GetSize();k++)
				delete pCPtrArray->GetAt(k);
			pCPtrArray->RemoveAll();
			delete pCPtrArray;
			if(m_bIsToEnd)
				break;
		}
		if(m_bIsToEnd)
			break;
	}

}


////////////////////to order从小到大顺序排序
void CRudeMathematics::makeOrder(CTArray<int,int> &core)
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
		


bool CRudeMathematics::RoughSet(CIntMatrix W_matrix,bool bSpecified)
{
	//WriteData(W_matrix,"W_matrix");
	/////////////////please add data controlling ///////////
	//colCTArray 用于求核时临时存储W_matrix根据决策属性进行分类以后，同一类的行号。
	//rowCTArray 用于求核时临时存储W_matrix根据决策属性进行分类以后，任意其他同一类的行号。
	//coreCalc 是根据两类求出的核，coreCTArray是根据任意两类求出的核的并集。
	//ArOuterCTArray为条件属性总集-coreCTArray
	//gradeCTArray 存储了各条件属性的属性值的个数。
	//dCPtrArray为W_matrix根据决策属性进行分类的分类输。
	//classfyCPtrArray为根据传进的条件属性参数进行分类而得到的分类数。
	CTArray<int,int> colCTArray,rowCTArray,coreCalc,coreCTArray,ArOuterCTArray,gradeCTArray;
	CPtrArray dCPtrArray,classfyCPtrArray;
	CTArray<int,int> stdCTArray;
	int nCol=W_matrix.mcol();
	int nRow=W_matrix.mrow();
	int i,j;

	storeGrade(gradeCTArray,W_matrix);
	
	/////////////// classfy W_vector into many kinds by the creteria of the elements value of W_vector's last column.
	//////colCTArray stores one kind,and rowCTArray stores another.
	stdCTArray.Add(nCol-1);
	classfy(stdCTArray,W_matrix,dCPtrArray);
	for(i=0;i<dCPtrArray.GetSize()&&(!m_bIsToEnd);i++)//计算core的列
	{
		CPtrArray *pCOlPtrArray=(CPtrArray *)(dCPtrArray.GetAt(i));
		int ColSize=pCOlPtrArray->GetSize();
		for(j=0;j<ColSize;j++)
		{
			int ColValue=*((int *)(pCOlPtrArray->GetAt(j)));
			colCTArray.Add(ColValue);
			if(m_bIsToEnd)
				break;
		}
		for(int n=i+1;n<dCPtrArray.GetSize();n++)//计算core的行
		{
			CPtrArray *pRowPtrArray=(CPtrArray *)(dCPtrArray.GetAt(n));
			int RowSize=pRowPtrArray->GetSize();
			for(j=0;j<RowSize;j++)
			{
				int RowValue=*((int *)(pRowPtrArray->GetAt(j)));
				rowCTArray.Add(RowValue);
				if(m_bIsToEnd)
					break;
			}
			CalcCore(colCTArray,rowCTArray,coreCalc,W_matrix);
			AppendToCTArray(coreCTArray,coreCalc);
			rowCTArray.RemoveAll();
			if(m_bIsToEnd)
				break;
		}
		colCTArray.RemoveAll();
		if(m_bIsToEnd)
			break;
	}

	makeOrder(coreCTArray);//to order the coreCTArray
	if(bSpecified)
	{
		if(notIn(nCol-2,coreCTArray))
		{
			coreCTArray.Add(nCol-2);
		}
	}


	///////////////////////////get ArOuterCTArray/////
	for(i=0;i<nCol-1;i++)
	{
		if(notIn(i,coreCTArray))
		{
			ArOuterCTArray.Add(i);
		}
	}

	////////////////////////get max////////////////
	CTArray<int,int> tempCTArray;
	for(i=0;i<nCol-1;i++)
		tempCTArray.Add(i);
	CPtrArray tempCPtrArray;
	int max=0;
	int subMax=0;
	CTArray<int,int> tempCoreCTArray;
	if(!m_bIsToEnd)
	{
		classfy(tempCTArray,W_matrix,tempCPtrArray);
		max=statistics(tempCPtrArray,dCPtrArray);
		
		delCTArray(tempCTArray);
		delCPtrArray(tempCPtrArray);
		classfy(coreCTArray,W_matrix,classfyCPtrArray);
		subMax=statistics(classfyCPtrArray,dCPtrArray);

		////////////initialize tempCoreCTArray///////////
	
		for(i=0;i<coreCTArray.GetSize();i++)
			tempCoreCTArray.Add(coreCTArray.GetAt(i));
	}

	/////////////////迭代求解/////////
	while((subMax<max)&&(!m_bIsToEnd))
	{
		int tempInt=ArOuterCTArray.GetAt(0);
		tempCoreCTArray.Add(tempInt);
		int index=0;int tempmax1=0;
		if(!m_bIsToEnd)
		{
			classfy(tempCoreCTArray,W_matrix,classfyCPtrArray);
			tempmax1=statistics(classfyCPtrArray,dCPtrArray);
			tempCoreCTArray.RemoveAt(tempCoreCTArray.GetSize()-1);
		}
		CTArray<int,int> tempmax2Array;
		tempmax2Array.Add(tempmax1);
		for(i=1;i<ArOuterCTArray.GetSize()&&(!m_bIsToEnd);i++)//判断相对最重要属性
		{
			tempInt=ArOuterCTArray.GetAt(i);
			tempCoreCTArray.Add(tempInt);
			classfy(tempCoreCTArray,W_matrix,classfyCPtrArray);
			int tempmax2=statistics(classfyCPtrArray,dCPtrArray);
			tempmax2Array.Add(tempmax2);
			if(tempmax2>tempmax1)
			{
				tempmax1=tempmax2;
				index=i;
			}
			tempCoreCTArray.RemoveAt(tempCoreCTArray.GetSize()-1);

		}
		subMax=tempmax1;
		CPtrArray indexCPtrArray;
		for(i=0;i<ArOuterCTArray.GetSize()&&(!m_bIsToEnd);i++)//有多个同为最重要的属性时
		{
//			tempInt=ArOuterCTArray.GetAt(i);
			int tempmax2=tempmax2Array.GetAt(i);
			if(tempmax2==tempmax1)
			{
				int *p=new int(i);
				indexCPtrArray.Add((void *)p);
			}
//			tempCoreCTArray.RemoveAt(tempCoreCTArray.GetSize()-1);

		}
		if(indexCPtrArray.GetSize()>=1)
			index=*((int *)(indexCPtrArray.GetAt(0)));
		
		for(i=1;i<indexCPtrArray.GetSize()&&(!m_bIsToEnd);i++)//属性值不同值的个数最少的为所求。
		{
			int temp=*((int *)(indexCPtrArray.GetAt(i)));
			
			if((gradeCTArray.GetAt(ArOuterCTArray.GetAt(index)))>(gradeCTArray.GetAt(ArOuterCTArray.GetAt(temp))))///////////
				index=temp;	//ArOuterCTArray存储的为条件属性索引	//indexCPtrArray存储的为ArOuterCTArray的索引
							//注意其访问的层次关系
	
		}
		for(i=0;i<indexCPtrArray.GetSize();i++)
			delete indexCPtrArray.GetAt(i);
		indexCPtrArray.RemoveAll();
		tempInt=ArOuterCTArray.GetAt(index);////////////
		ArOuterCTArray.RemoveAt(index);
		tempCoreCTArray.Add(tempInt);

	}
	//////////////每次去掉一个条件属性，进行验证
	CTArray<int,int> temp2CoreCTArray;
	if(!m_bIsToEnd)
	{
		copyCTArray(temp2CoreCTArray,tempCoreCTArray);
		if(coreCTArray.GetSize()!=tempCoreCTArray.GetSize())
		{
			while((subMax==max)&&(!m_bIsToEnd))
			{
				for(i=coreCTArray.GetSize();i<temp2CoreCTArray.GetSize()&&(!m_bIsToEnd);i++)
				{
					if(m_bIsToEnd)
						break;
					temp2CoreCTArray.RemoveAt(i);
					classfy(temp2CoreCTArray,W_matrix,classfyCPtrArray);
					subMax=statistics(classfyCPtrArray,dCPtrArray);
					if(subMax==max)
					{
						copyCTArray(tempCoreCTArray,temp2CoreCTArray);
						break;
					}
					else
					{
						copyCTArray(temp2CoreCTArray,tempCoreCTArray);
					}
				}
			}
		}
	}


	/////////////////after modification////////
	CTArray<int,int> outArCTArray;
	CTArray<int,int> elementsCTArray;
	int outCol=0;
	int outRow=0;
	CIntMatrix outMatrix(1,1);//约简以后的输出表
	copyCTArray(m_TempCoreArray,tempCoreCTArray);
	if(!m_bIsToEnd)
	{
		copyCTArray(m_ArOutArray,tempCoreCTArray);
		m_ArOutArray.Add(W_matrix.mcol()-1);
		outCol=m_ArOutArray.GetSize();
		classfy(m_ArOutArray,W_matrix,classfyCPtrArray);
		outRow=classfyCPtrArray.GetSize();
		CIntMatrix outMatrixTemp(outRow,outCol);
		outMatrix=outMatrixTemp;
		for(i=0;i<outRow;i++)
		{
			CPtrArray *p=(CPtrArray *)(classfyCPtrArray.GetAt(i));
			int nRow=*((int *)(p->GetAt(0)));
			elementsCTArray.Add(nRow);
			for(int j=0;j<outCol;j++)
			{
		
				int nCol=m_ArOutArray.GetAt(j);
				outMatrix(i,j)=W_matrix(nRow,nCol);
				
			}
			if(m_bIsToEnd)
				break;
		}
	}



////////////////create decision tree////////////////////////////////////////

	RoughSetStruct RootNode;
	for(i=0;i<outMatrix.mcol()-1;i++)
		RootNode.attribsCTArray.Add(i);///////////
	for(i=0;i<outMatrix.mrow();i++)
		RootNode.elementsCTArray.Add(i);///////
	stdCTArray.RemoveAll();
	stdCTArray.Add(outMatrix.mcol()-1);
	classfy(stdCTArray,outMatrix,dCPtrArray);
	RootNode.nAttribClassfy=MostImpt(RootNode.attribsCTArray,RootNode.elementsCTArray,outMatrix,dCPtrArray);///////////
	RootNode.strRootNodeClassfyValue="";
	if(!m_bIsToEnd)//假如用户没有终止程序，建立决策树
	{
		if(isSubSet(RootNode.elementsCTArray,dCPtrArray))
		{
			RootNode.nIsAllSub=1;///////////
			RootNode.strAttribDecision=GetMapStrValue(outMatrix(RootNode.elementsCTArray.GetAt(0),outMatrix.mcol()-1),m_ArOutArray.GetAt(outMatrix.mcol()-1));//////////
		}
		else
		{
			RootNode.nIsAllSub=0;
			RootNode.strAttribDecision="";
			CreateTree(RootNode,outMatrix,dCPtrArray);
		}
	}
	////////////////////read decision tree///
	CTStringArray rsltCStringArray,CDCStringArray,SDCStringArray,DecisionValueCStringArray;
	CTString OutStr;
	if(!m_bIsToEnd)
	{
		if(RootNode.nIsAllSub==1)
		{
			//取得决策规则中条件值(if)
			CTString stra="一切条件";		
			rsltCStringArray.Add(stra);

			//then
			CTString tempStr;
			tempStr.Empty();
			tempStr+=OutStr;
			tempStr+=GetDecisionArName();
			tempStr+="=";
			CTString str=RootNode.strAttribDecision;
			tempStr+=str;
			tempStr+=";";
			DecisionValueCStringArray.Add(tempStr);
			double CD=1.00;
			str.Empty();
			str.Format("%.2f%%",CD*100);
			CDCStringArray.Add(str);
			CTString str2;
			m_RecordStrArray.Add("一切有效记录");
			double SD=CalcSD(classfyCPtrArray,W_matrix,RootNode.elementsCTArray);
			str2.Format("%.2f%%",SD*100);
			SDCStringArray.Add(str2);

		}
		else
		{
			int nSize=RootNode.pCPtrArray.GetSize();
			for(int i=0;i<nSize;i++)
			{
				int length=OutStr.GetLength();
				RoughSetStruct *pRSS=(RoughSetStruct *)(RootNode.pCPtrArray.GetAt(i));			
				OutStr+=GetOrgFieldName(RootNode,tempCoreCTArray);
				OutStr+="=";
				ReadTree(*pRSS,tempCoreCTArray,rsltCStringArray,CDCStringArray,SDCStringArray,OutStr,classfyCPtrArray,outMatrix,W_matrix,DecisionValueCStringArray);
				OutStr=OutStr.Left(length);
			}
		}
	}


///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////output the result/////////////////////////////////////////////
	if(!m_bIsToEnd)
	{
		CreateResult(outMatrix, coreCTArray,tempCoreCTArray,classfyCPtrArray,rsltCStringArray,CDCStringArray,SDCStringArray,DecisionValueCStringArray);
	}
	delCPtrArray(dCPtrArray);
	delCPtrArray(classfyCPtrArray);
	DeleteTree(RootNode);////////////delete the decision tree
	rsltCStringArray.RemoveAll();
	CDCStringArray.RemoveAll();
	SDCStringArray.RemoveAll();
	DecisionValueCStringArray.RemoveAll();
	return true;
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////建立决策树/////////////////////////////////////////

void CRudeMathematics::CreateTree(RoughSetStruct & root,CIntMatrix outMatrix,CPtrArray &dCPtrArray)
{
	CPtrArray classfyCPtrArray;
	CTArray<int,int> stdCTArray;
	stdCTArray.Add(root.attribsCTArray.GetAt(root.nAttribClassfy));
	classfy(stdCTArray,root.elementsCTArray,outMatrix,classfyCPtrArray);
	for(int i=0;i<classfyCPtrArray.GetSize();i++)
	{
		RoughSetStruct *pRSS=new RoughSetStruct;
		root.pCPtrArray.Add((void *)pRSS);
		CPtrArray *pPA=(CPtrArray *)(classfyCPtrArray.GetAt(i));
		
		int row=*((int *)(pPA->GetAt(0)));
		int col=root.attribsCTArray.GetAt(root.nAttribClassfy);
		pRSS->strRootNodeClassfyValue=GetMapStrValue(outMatrix(row,col),m_ArOutArray.GetAt(col));//////////为新节点赋相应值
		CTArray<int,int> tempCTArray;
		ToCTArray(pPA,pRSS->elementsCTArray);//////////为新节点赋相应值
		copyCTArray(pRSS->attribsCTArray,root.attribsCTArray);//////////为新节点赋相应值
		pRSS->attribsCTArray.RemoveAt(root.nAttribClassfy);
		if((pRSS->attribsCTArray.GetSize())==0)//当分类属性已经用尽时
			pRSS->nAttribClassfy=0;
		else//当分类属性未用尽时，求出最重要属性。
			pRSS->nAttribClassfy=MostImpt(pRSS->attribsCTArray,pRSS->elementsCTArray,outMatrix,dCPtrArray);///////为新节点赋值
		if(isSubSet(pRSS->elementsCTArray,dCPtrArray))
		{
			pRSS->nIsAllSub=1;///////////为新节点赋相应值
			pRSS->strAttribDecision=GetMapStrValue(outMatrix(pRSS->elementsCTArray.GetAt(0),outMatrix.mcol()-1),m_ArOutArray.GetAt(outMatrix.mcol()-1));/////为新节点赋值
		}
		else
		{
			if((pRSS->attribsCTArray.GetSize())==0)//分类属性用尽时
			{
				pRSS->nIsAllSub=2;///////////为新节点赋相应值
				pRSS->strAttribDecision=GetMapStrValue(outMatrix(pRSS->elementsCTArray.GetAt(0),outMatrix.mcol()-1),m_ArOutArray.GetAt(outMatrix.mcol()-1));/////为新节点赋值
			}
			else //分类属性未用尽时
			{			
				CPtrArray tempPtrArray;
				classfy(pRSS->attribsCTArray,pRSS->elementsCTArray,outMatrix,tempPtrArray);
				if(tempPtrArray.GetSize()==1)//后面分类没有意义时，即为同一类；			
				{
					pRSS->nIsAllSub=3;///////////为新节点赋相应值
					pRSS->strAttribDecision=GetMapStrValue(outMatrix(pRSS->elementsCTArray.GetAt(0),outMatrix.mcol()-1),m_ArOutArray.GetAt(outMatrix.mcol()-1));/////为新节点赋值
				}
				else//不为同一类
				{
					if(m_bIsToEnd)//用户选择了终止程序
					{
						pRSS->nIsAllSub=1;////////////为新节点赋相应值
						pRSS->strAttribDecision="";///////////////为新节点赋相应值
					}
					else
					{
						pRSS->nIsAllSub=0;////////////为新节点赋相应值
						pRSS->strAttribDecision="";///////////////为新节点赋相应值					
						CreateTree(*pRSS,outMatrix,dCPtrArray);///////////
					}
				}
				delCPtrArray(tempPtrArray);

			}
			
			 
		}
	}
	delCPtrArray(classfyCPtrArray);
}


///////判断重要属性
int  CRudeMathematics::MostImpt(CTArray<int,int> & Attrib,CTArray<int,int> &elementsCTArray,CIntMatrix outMatrix,CPtrArray &dCPtrArray)
{
	if(Attrib.GetSize()==0)
		return 0;
	else
	{
		int index=0,max=0;
		CTArray<int,int> stdCTArray;
		CTArray<int,int> SizeArray;
		CTArray<int,int> tempArray;
		CPtrArray classfyCPtrArray;

		//判断最重要的属性
		int nSize=Attrib.GetSize();
		int i;
		for(i=0;i<nSize;i++)
		{
			int value=Attrib.GetAt(i);
			stdCTArray.Add(value);
			classfy(stdCTArray,elementsCTArray,outMatrix,classfyCPtrArray);
			SizeArray.Add(classfyCPtrArray.GetSize());
			int temp=statistics(classfyCPtrArray,dCPtrArray);
			tempArray.Add(temp);
			if(temp>max)
			{
				max=temp;
				index=i;
			}
			stdCTArray.RemoveAll();
		}

		//以下模块考虑如有多个最重要属性时的情况；那么返回在该节点不同属性值最少的那一个属性
		CTArray<int,int> arribArray;//存储重要性相同的属性
		CTArray<int,int> gradeArray;//存储重要性相同的属性的在该节点的不同属性值的个数
		for(i=0;i<nSize;i++)
		{
			int size=SizeArray.GetAt(i);
			int temp=tempArray.GetAt(i);
			if(temp==max)
			{
				gradeArray.Add(size);
				arribArray.Add(i);
			}
			stdCTArray.RemoveAll();
		}
		index=arribArray.GetAt(0);
		int temp=gradeArray.GetAt(0);
		for(i=1;i<gradeArray.GetSize();i++)//取得最重要属性中，不同值个数最小的一个
		{
			if(temp>gradeArray.GetAt(i))
			{
				temp=gradeArray.GetAt(i);
				index=arribArray.GetAt(i);
			}
		}
		arribArray.RemoveAll();
		SizeArray.RemoveAll();
		tempArray.RemoveAll();
		gradeArray.RemoveAll();
		delCPtrArray(classfyCPtrArray);
		return index;
	}
}

//判断集合是否是根据决策属性分类集合的子集
bool CRudeMathematics::isSubSet(CTArray<int,int> &elementsCTArray,CPtrArray &dCPtrArray)
{
	CTArray<int,int> maxCTArray;
	for(int i=0;i<dCPtrArray.GetSize();i++)
	{
		CPtrArray *pPtrArray=(CPtrArray *)dCPtrArray.GetAt(i);
		ToCTArray(pPtrArray,maxCTArray);
		if(IsIncluded(elementsCTArray,maxCTArray))
		{
			return true;
		}
	}
	return false;
}


//计算决策规则中SD值，各种指向关系请参考ReadTree()中第一次调用本函数时的说明
double CRudeMathematics::CalcSD(CPtrArray &classfyCPtrArray,CIntMatrix W_matrix,CTArray<int,int> &ElementsCTArray)
{
	int wSize=W_matrix.mrow();
	double sum=0;
	m_strRecord.Empty();
//	CTString strtemp;
//	int nCount=0;
	for(int i=0;i<ElementsCTArray.GetSize();i++)//取得记录号集
	{
		CPtrArray * p=(CPtrArray *)(classfyCPtrArray.GetAt(ElementsCTArray.GetAt(i)));
		sum=sum+p->GetSize();
		for(int j=0;j<p->GetSize();j++)
		{
//			nCount++;
			int nRow=*((int *)(p->GetAt(j)));
			nRow=m_ColIndexArray.GetAt(nRow);
			CTString str;
			str.Format("%d;",nRow+1);
			m_strRecord+=str;
//			if(nCount==8)
//			{
//				m_strRecord+="\n";
//				nCount=0;
//			}
		}
	}
	int nLen=m_strRecord.GetLength();
	if(nLen>0)
	{
		if(m_strRecord.GetAt(nLen-1)==';')
			m_strRecord.Delete(nLen-1);
	}
	return sum/wSize;

}


//读取决策树的数据
//node 为树节点，
//tempCoreCTArray存储约简表的条件属性，rsltCStringArray存储决策规则中条件值(if),CDCStringArray存储决策规则中CD只值,
//SDCStringArray存储决策规则中SD值，OutStr为rsltCStringArray取值服务,classfyCPtrArray为根据约简表中的条件属性和决策属性，
//进行分类的三层决策树，outMatrix为约简表，W_matrix为原始表,DecisionValueCStringArray为决策规则中的结果值(then).

void CRudeMathematics::ReadTree(RoughSetStruct &node,CTArray<int,int> &tempCoreCTArray,CTStringArray &rsltCStringArray,
								CTStringArray &CDCStringArray,CTStringArray &SDCStringArray,CTString &OutStr,
								CPtrArray &classfyCPtrArray,CIntMatrix outMatrix,CIntMatrix W_matrix,
								CTStringArray &DecisionValueCStringArray)
{
	if(node.nIsAllSub==1)///该节点元素是按决策属性分类所形成的集合的子集时
	{
		//取得决策规则中条件值(if)
		CTString tempStr;
		tempStr.Empty();
		tempStr+=OutStr;
		CTString str=node.strRootNodeClassfyValue;
		tempStr+=str;
		rsltCStringArray.Add(tempStr);
		
		//取得决策规则中结果值(then)
		CTString tempStr2;
		tempStr2+=GetDecisionArName();
		tempStr2+="=";
		str.Empty();
		str=node.strAttribDecision;
		tempStr2+=str;
		DecisionValueCStringArray.Add(tempStr2);
		
		//取得决策规则中CD值
		double CD=1.00*100;
		str.Empty();
		str.Format(" %.2f%%",CD);
		CDCStringArray.Add(str);

		//取得决策规则中SD值
		double SD=CalcSD(classfyCPtrArray,W_matrix,node.elementsCTArray);//node.elementsCTArray的元素值就为classfyCPtrArray树的第二层索引号
		CTString str2;
		SD=SD*100;
		str2.Format(" %.2f%%",SD);
		SDCStringArray.Add(str2);
		m_RecordStrArray.Add(m_strRecord);
	}
	else if(node.nIsAllSub==2||node.nIsAllSub==3)
	{
		//if(node.attribsCTArray.GetSize()==0)//该节点元素不是按决策属性分类所形成的集合的子集时，但分类属性元素已经用尽时
		//{
			//WriteData(outMatrix,"outMatrix");
			//将该节点元素按决策属性条件进行分类
			CTArray<int,int> stdCTArray;
			CPtrArray classfyCPtrArray2;
			stdCTArray.Add(outMatrix.mcol()-1);
			classfy(stdCTArray,node.elementsCTArray,outMatrix,classfyCPtrArray2);

			//求出该节点CD的累加值
			double size=0.0;
			CTArray<double,double> SDArray;
			int nSize=classfyCPtrArray2.GetSize();
			int i;
			for(i=0;i<nSize;i++)
			{
				CPtrArray *p=(CPtrArray *)(classfyCPtrArray2.GetAt(i));
				CTArray<int,int> tempCTArray;
				ToCTArray(p,tempCTArray);
				double SD=CalcSD(classfyCPtrArray,W_matrix,tempCTArray);
				m_RecordStrArray.Add(m_strRecord);
				size=size+SD;
				SDArray.Add(SD);
			}

			//分类以后决策规则中各值的求解
			for(i=0;i<nSize;i++)
			{
				//计算SD
				CPtrArray *p=(CPtrArray *)(classfyCPtrArray2.GetAt(i));
				CTArray<int,int> tempCTArray;
				ToCTArray(p,tempCTArray);
				double SD=SDArray.GetAt(i);
				
				//计算CD
				double CD=SD/size;
				CTString strArDecision=GetMapStrValue(outMatrix(tempCTArray.GetAt(0),outMatrix.mcol()-1),m_ArOutArray.GetAt(outMatrix.mcol()-1));
				CTString tempStr;
				tempStr.Empty();

				//计算条件(if)
				tempStr+=OutStr;
				CTString str=node.strRootNodeClassfyValue;
				tempStr+=str;
				rsltCStringArray.Add(tempStr);

				//计算结果(then)
				CTString tempStr2;
				tempStr2+=GetDecisionArName();
				tempStr2+="=";
				str.Empty();
				str=strArDecision;
				tempStr2+=str;
				DecisionValueCStringArray.Add(tempStr2);

				//SD CD
				str.Empty();
				CD=CD*100;
				str.Format(" %.2f%%",CD);
				CDCStringArray.Add(str);
				CTString str2;
				SD=SD*100;
				str2.Format(" %.2f%%",SD);
				SDCStringArray.Add(str2);
			}
			delCPtrArray(classfyCPtrArray2);
		//}
	}
	else		///不为最底层节点，根据节点存储的值，读取下一层节点的值
	{
		for(int i=0;i<node.pCPtrArray.GetSize();i++)
		{
			int length=OutStr.GetLength();
			RoughSetStruct *pRSS=(RoughSetStruct *)(node.pCPtrArray.GetAt(i));
			CTString str=node.strRootNodeClassfyValue;//增加上一层条件值
			OutStr+=str;
			OutStr+=",  ";
			OutStr+=GetOrgFieldName(node,tempCoreCTArray);//增加该层条件名
			OutStr+="=";
			ReadTree(*pRSS,tempCoreCTArray,rsltCStringArray,CDCStringArray,SDCStringArray,OutStr,classfyCPtrArray,outMatrix,W_matrix,DecisionValueCStringArray);
			OutStr=OutStr.Left(length);//恢复OutStr
		}
	}
	
}

/////删除树
void CRudeMathematics::DeleteTree(RoughSetStruct &node)
{
	for(int i=0;i<node.pCPtrArray.GetSize();i++)
	{
		RoughSetStruct *pRSS=(RoughSetStruct *)(node.pCPtrArray.GetAt(i));
		if(pRSS->nIsAllSub==1||pRSS->nIsAllSub==2||pRSS->nIsAllSub==3)
		{
			pRSS->attribsCTArray.RemoveAll();
			pRSS->elementsCTArray.RemoveAll();			
			delete pRSS;
		}
		else
		{
			DeleteTree(*pRSS);
			delete pRSS;
		}

	}
	node.pCPtrArray.RemoveAll();

}

//取得原始表相对应的条件属性字段名
CTString CRudeMathematics::GetOrgFieldName(RoughSetStruct &node,CTArray<int,int> &tempCoreArray)
{
	int index=tempCoreArray.GetAt(node.attribsCTArray.GetAt(node.nAttribClassfy));//索引关系是难点
	if(m_bSpecified)
	{
		if(index==m_nSpecified)
			return GetSpecifiedName();
	}
	int currentSel=m_tVarInfo.pValue[index];
	return (m_pDataInterface->GetFieldName(currentSel));
}
//取得用户指定属性字段名
CTString CRudeMathematics::GetSpecifiedName()
{
	return (m_pDataInterface->GetFieldName(m_nSpecifiedIndex));

}


// 取得决策属性字段名
CTString CRudeMathematics::GetDecisionArName()
{
	return (m_pDataInterface->GetFieldName(m_nDecisionIndex));
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void CRudeMathematics::CreateResult(CIntMatrix outMatrix,CTArray<int,int> &core,CTArray<int,int> &tempcore,CPtrArray &classfyCPtrArray,CTStringArray &rsltCStringArray,CTStringArray &CDCStringArray,CTStringArray &SDCStringArray,CTStringArray &DecisionValueCStringArray)
{
//	m_pResult = new CResult("粗糙集分析");
	CRsltElementTable * pETable = NULL;
	CTLTable * pTable ;

		///////////输出重要属性集core///////////////////////
	int nCoreSize=core.GetSize();
	pTable = new CTLTable;
	pTable->SetTitle("重要属性集");
	if(nCoreSize==0)
		pTable->SetHead("空集");
	else
	{
		pTable->SetCols(nCoreSize+1);
		pTable->SetRows(1);
		for(int i=0;i<nCoreSize;i++)
		{
			int index=core.GetAt(i);
			if(m_bSpecified&&index==m_nSpecified)
			{	
				pTable->InsertColumn(i,m_pDataInterface->GetFieldName(m_nSpecifiedIndex));
			}
			else
			{
			
				int currentSel=m_tVarInfo.pValue[core.GetAt(i)];
				pTable->InsertColumn(i,m_pDataInterface->GetFieldName(currentSel));
		
			}
		}
		pETable= new CRsltElementTable("重要属性集",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);
	}

	///////////////////输出约简属性集tempcoreCTArray///////////////////////

	pTable = new CTLTable;
	int nTempCoreSize=tempcore.GetSize();
	pTable->SetTitle("约简属性集");
	pTable->SetCols(nTempCoreSize+1);
	pTable->SetRows(1);
	int i;
	for(i=0;i<nTempCoreSize;i++)
	{
		int index=tempcore.GetAt(i);
		if(m_bSpecified&&index==m_nSpecified)
		{	
			pTable->InsertColumn(i,m_pDataInterface->GetFieldName(m_nSpecifiedIndex));
		}
		else
		{
		
			int currentSel=m_tVarInfo.pValue[tempcore.GetAt(i)];
			pTable->InsertColumn(i,m_pDataInterface->GetFieldName(currentSel));
	
		}
	}

	pETable= new CRsltElementTable("约简属性集",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);


/////////////////////输出约简表/////////////////
	pTable = new CTLTable;
	pTable->SetTitle("约简表");
	int nCol=outMatrix.mcol();
	int nRow=outMatrix.mrow();
	CTString strDelHead="被删除记录行为：\n";		//输出被删除的记录
	int nCalc=0;
	for(int ss=0;ss<m_DelArray.GetSize();ss++)
	{
		CTString str;
		str.Format("行%d;",m_DelArray.GetAt(ss)+1);
		strDelHead+=str;
		nCalc++;
		if(nCalc==10)
		{
			strDelHead+="\n";
			nCalc=0;
		}

	}
	pTable->SetCols(nCol+1);
	pTable->SetRows(nRow+1);
	if(m_DelArray.GetSize()>0)//有不合法的记录行被删除时
		pTable->SetHead(strDelHead);
	CTString str;
	pTable->InsertColumn(0,"记录号");
	for(i=0;i<nCol;i++)
	{
		if(i==nCol-1)
			pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_nDecisionIndex));
		else
		{
			int index=tempcore.GetAt(i);
			if(m_bSpecified&&index==m_nSpecified)//指定了用户指定属性，并且该变量或字段就为用户指定属性
			{	
				pTable->InsertColumn(i,m_pDataInterface->GetFieldName(m_nSpecifiedIndex));
			}
			else
			{
			
				int currentSel=m_tVarInfo.pValue[tempcore.GetAt(i)];//m_tVarInfo.pValue[tempcore.GetAt(i)]为数据库列的索引
				pTable->InsertColumn(i,m_pDataInterface->GetFieldName(currentSel));
		
			}	
		}
	}
	for(i=0;i<nRow;i++)
	{		
		CPtrArray *p=(CPtrArray *)(classfyCPtrArray.GetAt(i));
		CTString strtemp;
//		int nCount=0;
		for(int k=0;k<p->GetSize();k++)//插入记录号
		{
//			nCount++;
			int nRow1=*((int *)(p->GetAt(k)));
			nRow1=m_ColIndexArray.GetAt(nRow1);
			str.Format("%d;",nRow1+1);
			strtemp+=str;
//			if(nCount==5&&(k!=p->GetSize()))
//			{
//				nCount=0;
//				strtemp+="\n";
//			}
		}
		int nLen=strtemp.GetLength();
		if(nLen>0)
		{
			if(strtemp.GetAt(nLen-1)==';')
				strtemp.Delete(nLen-1);
		}

		pTable->InsertItem(i+1,strtemp);
		for(int j=0;j<nCol;j++)
		{
			int t=outMatrix(i,j);
			int ncol=m_ArOutArray.GetAt(j);
			CTString strp=GetMapStrValue(outMatrix(i,j),m_ArOutArray.GetAt(j));//m_ArOutArray.GetAt(j)为原始矩阵列的索引
			strp += _T(" ");//防止输出表会把它转化为0，因此加上一个字符。
			pTable->SetItemText(i,j+1,strp);
		}
	}
	pETable= new CRsltElementTable("约简表",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);



	//////output decision tree/////
	/////////////////////////////////
	pTable=new CTLTable;
	pTable->SetTitle("决策规则");
	pTable->SetCols(5);
	pTable->SetRows(rsltCStringArray.GetSize()+1);
// 	pTable->SetCellAlignFormat(DT_LEFT);
	pTable->InsertColumn(0,"记录号");
	pTable->InsertColumn(1,"条件");	
	pTable->InsertColumn(2,"结果");
	pTable->InsertColumn(3,"可信度");
	pTable->InsertColumn(4,"支持度");
	int nSize=rsltCStringArray.GetSize();
	for(i=0;i<nSize;i++)
	{
		CTString Str,Str1;
	//	Str1.Format("%d",i+1);
		Str.Empty();
		Str=m_RecordStrArray.GetAt(i);
		pTable->InsertItem(i,Str);
		Str.Empty();
		Str=rsltCStringArray.GetAt(i);	
		pTable->SetItemText(i,1,Str);
		Str.Empty();
		Str=DecisionValueCStringArray.GetAt(i);
		pTable->SetItemText(i,2,Str);
		Str.Empty();
		Str=CDCStringArray.GetAt(i);
		pTable->SetItemText(i,3,Str);
		Str.Empty();
		Str=SDCStringArray.GetAt(i);
		pTable->SetItemText(i,4,Str);


	}
	pETable= new CRsltElementTable("决策规则",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	
}

CResult * CRudeMathematics::OnRoughSet()
{	
	m_pResult= new CResult("粗糙集分析");
	int n = m_pDataInterface->GetRowCount();
	int m = m_tVarInfo.iCount;
	double total=double(m)*double(n);
	if(total>1e7)
	{
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		CTString str="数据源太大，请先抽样！";
		m_pRsltElementText->AddString(str);
		m_pResult->Add(m_pRsltElementText);
		return m_pResult;
	}
	if(m==0||n==0)
	{
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		CTString str="您选择的数据无效！";
		m_pRsltElementText->AddString(str);
		m_pResult->Add(m_pRsltElementText);
		return m_pResult;
	}
	CDataAccess *pDataAccess=&(m_pDataInterface->m_DataAccess);
//	pDataAccess->SetMaxPageCount(500);
	if(m_bSpecified)
	{		
		CIntMatrix Imatrix(n,m+2);
		for(int i=0;i<m;i++)
		{
			if(m_bIsToEnd)
			{
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("警告");
				CTString str="您选择了终止程序，分析无效！";
				m_pRsltElementText->AddString(str);
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;
			}
			GetColMapData(Imatrix,i,m_tVarInfo.pValue[i]);
		}
		m_nSpecified=m;	
		GetColMapData(Imatrix,m,m_nSpecifiedIndex);
		GetColMapData(Imatrix,m+1,m_nDecisionIndex);
		CTStringArray *p=(CTStringArray *)(m_MapPtrArray.GetAt(m+1));
		if((p->GetSize()==1)&&(!m_bIsToEnd))
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str="决策属性具有唯一值，无需分析！";
			m_pRsltElementText->AddString(str);
			m_pResult->Add(m_pRsltElementText);
			return m_pResult;
		}
//		pDataAccess->SetMaxPageCount(5);
		Imatrix=CreateNewMatrix(Imatrix);
		RoughSet(Imatrix,true);	
	}
	else
	{
		CIntMatrix Imatrix(n,m+1);		
		for(int i=0;i<m;i++)
		{
			if(m_bIsToEnd)
			{
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("警告");
				CTString str="您选择了终止程序，分析无效！";
				m_pRsltElementText->AddString(str);
				m_pResult->Add(m_pRsltElementText);
				return m_pResult;
			}
			GetColMapData(Imatrix,i,m_tVarInfo.pValue[i]);
		}		
		GetColMapData(Imatrix,m,m_nDecisionIndex);
//		pDataAccess->SetMaxPageCount(5);
		CTStringArray *p=(CTStringArray *)(m_MapPtrArray.GetAt(m));
		if(p->GetSize()==1)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str="决策属性具有唯一值，无需分析！";
			m_pRsltElementText->AddString(str);
			m_pResult->Add(m_pRsltElementText);
			return m_pResult;
		}
		if(m_bIsToEnd)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str="您选择了终止程序，分析无效！";
			m_pRsltElementText->AddString(str);
			m_pResult->Add(m_pRsltElementText);
			return m_pResult;
		}
		Imatrix=CreateNewMatrix(Imatrix);
		RoughSet(Imatrix);	
	}
	for(int i=0;i<m_MapPtrArray.GetSize();i++)
	{
		CTStringArray *p=(CTStringArray *)(m_MapPtrArray.GetAt(i));
		p->RemoveAll();
		delete p;
	}
	m_MapPtrArray.RemoveAll();
	return m_pResult;

} 



//*********************************************************************
//zkc  2002.12.16
//strArray  the specified string array  
//str  the specified string
//funtion: to decide whether the strArray has the element string which
//			has the same value with the str
//			if has,return false,or return true;
//************************************************************************
bool CRudeMathematics::notIn(CTString str,CTStringArray &strArray)
{
	for(int i=0;i<strArray.GetSize();i++)
	{
		if(str==strArray.GetAt(i))
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

void CRudeMathematics::GetStringName(int index,CTStringArray &strArray1,CTStringArray &strArray2)
{
	if(index>=0)
	{		
		CField* pField;
		CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
		if(index>pDataAccess->m_pFieldList->GetFieldCount())
			return;
		pDataAccess->First();
		pField=pDataAccess->m_pFieldList->FieldByIndex(index);
		while(!pDataAccess->Eof())
		{	
			
			CTString str;
 			if(!pField->IsNull())
				str=pField->GetAsString();
			else
				str="空值";
			int nLen=str.GetLength();
			if(str.GetAt(nLen-1)=='.')
				str.Delete(nLen-1);				
			strArray1.Add(str);
			if(notIn(str,strArray2))
				strArray2.Add(str);
			pDataAccess->Next();
			if(m_bIsToEnd)
				break;

		}
				
	}
}


//把指定的列nDataCol数据映射成整数数据存储在matrix中指定的nMatrixCol；
//
void CRudeMathematics::GetColMapData(CIntMatrix &matrix,int nMatrixCol,int nDataCol)
{
	CTStringArray strArray1;
	CTStringArray *pStrArray2=new CTStringArray;
	GetStringName(nDataCol,strArray1,*pStrArray2);
	/*
	CField* pField;
	CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
	if(nDataCol>pDataAccess->m_pFieldList->GetFieldCount())
		return;
	pDataAccess->First();
	pField=pDataAccess->m_pFieldList->FieldByIndex(nDataCol);
	int count=0;
	while(!pDataAccess->Eof())
	{	
					
		CTString str;
 		if(!pField->IsNull())
			str=pField->GetAsString();
		else
			str="空值";
		int nLen=str.GetLength();
		if(str.GetAt(nLen-1)=='.')
			str.Delete(nLen-1);			
		count++;		
		pDataAccess->Next();
	}
    */
	int nRow=strArray1.GetSize();
	for(int i=0;i<nRow;i++)
	{
		CTString str=strArray1.GetAt(i);
		for(int j=0;j<(*pStrArray2).GetSize();j++)
		{
			if(str==(*pStrArray2).GetAt(j))
			{
				matrix(i,nMatrixCol)=j;
				break;
			}
		}
		if(m_bIsToEnd)
			break;
	}
	//*/
	strArray1.RemoveAll();
	m_MapPtrArray.Add((void *)pStrArray2);

}

//

//根据列号和数值，根据映射找到原始对应字符串值
//nValue被映射转化后的整数值；
//nCol列索引号
CTString CRudeMathematics::GetMapStrValue(int nValue,int nCol)
{
	CTStringArray *p=(CTStringArray *)(m_MapPtrArray.GetAt(nCol));
	CTString temp=p->GetAt(nValue);
	int index=nCol;
	int currentSel=-1;
	if(m_bSpecified&&index==m_nSpecified)
	{

		currentSel=m_nSpecifiedIndex;
	}
	else if(index<m_tVarInfo.iCount)
	{
		currentSel=m_tVarInfo.pValue[index];
	}
	else
	{
		currentSel=m_nDecisionIndex;
	}
	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(currentSel);
	CTString labelVal;
	CTString realVal=p->GetAt(nValue);
	if(pField->GetValueVL(realVal,labelVal))
	{
		return labelVal;
	}
	else
	{
		return realVal;
	}
}


CIntMatrix CRudeMathematics::CreateNewMatrix(CIntMatrix matrix)
{
	CTArray<int,int> DelArray;
	int nCol=matrix.mcol();
	int nRow=matrix.mrow();
	int i;
	for(i=0;i<nRow;i++)
	{
		//int count=0;
	//	for(int j=0;j<nCol-1;j++)
	//	{
			if(GetMapStrValue(matrix(i,nCol-1),nCol-1)==CTString("空值"))//决策属性为空时，删除该行
			{
	//			count=nCol-1;
			//	break;
			//}
			//if(GetMapStrValue(matrix[i][j],j)==CTString("空值"))
			//	count++;
		//}
	//	if(count==nCol-1)
	//	{
			m_DelArray.Add(i);
			DelArray.Add(i);
		}
	//	count=0;
	}
	CIntMatrix tempMatrix(nRow-m_DelArray.GetSize(),nCol);
//	nRow=tempMatrix.mrow();
//	nCol=tempMatrix.mcol();
	int count=0;
	for(i=0;i<nRow;i++)
	{
		if(DelArray.GetSize()>0&&(i==DelArray.GetAt(0)))
		{			
			count++;
			DelArray.RemoveAt(0);			
		}
		else
		{
			for(int j=0;j<nCol;j++)
			{		
				tempMatrix(i-count,j)=matrix(i,j);
			}
			m_ColIndexArray.Add(i);
		}
	}
	return tempMatrix;
}