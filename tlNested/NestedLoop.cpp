// NestedLoop.cpp: implementation of the CNestedLoop class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NestedLoop.h"
#include "../Public/TString.h"
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
const unsigned int WM_PROGRESS_TEXT=::RegisterWindowMessage("WM_PROGRESS_TEXT");
*/
const double MAXDEAL=100000000;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CNestedOutput::Import(string szFileName)
{
	return true;
}

bool CNestedOutput::Export(string szFileName)
{
	return true;
}

CNestedLoop::CNestedLoop()
{
	m_nDistance=1;
	m_nSolarity=1;
	m_pResult=NULL;
	m_pRsltElementText=NULL;
// 	m_pProcedureWnd=NULL;
	m_bIsToEnd=false;
	m_nVarOrRec=1;
}

CNestedLoop::~CNestedLoop()
{
	for(int i=0;i<m_MaxAndMinPtrArray.GetSize();i++)
	{
		TMaxAndMin *p=(TMaxAndMin *)(m_MaxAndMinPtrArray.GetAt(i));
		delete p;
	}
	m_MaxAndMinPtrArray.RemoveAll();

}


CDoubleMatrix  CNestedLoop::GetData()
{
	if(m_nVarOrRec==0)
	{
		double max=0;
		double min=0;
		int nRow=m_pDataInterface->GetRowCount();
		int nCol=m_tVarInfo.iCount;
		CDoubleMatrix matrix(nRow,nCol);
		for(int i=0;i<m_tVarInfo.iCount;i++)
		{
			int nCurCol=m_tVarInfo.pValue[i];
			double mean=GetMaxAndMin(nCurCol,max,min);
			double temp=mean;
			CField* pField;
			CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
			pDataAccess->First();
			pField=pDataAccess->m_pFieldList->FieldByIndex(nCurCol);
			int nCount=0;		
			while(!pDataAccess->Eof())
			{	
				temp=mean;
				if(!pField->IsNull())
					temp=pField->GetAsDouble();
				if(max==min)
					matrix(nCount,i)=max;
				else
					matrix(nCount,i)=(temp-min)/(max-min);
				nCount++;		
				pDataAccess->Next();
				if(m_bIsToEnd)
					break;				
			}
			if(m_bIsToEnd)//the user has terminated the application.
				break;
		}
		return matrix;
	}
	else
	{
		int nRow=m_tVarInfo.iCount;
		int nCol=m_pDataInterface->GetRowCount();
		CDoubleMatrix matrix(nRow,nCol,0.0);
		CField* pField;
		CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
		int nCount=0;
		GetMaxAndMin();	
		pDataAccess->First();
		while(!pDataAccess->Eof())
		{	
			for(int i=0;i<m_tVarInfo.iCount;i++)
			{	
				int nCurCol=m_tVarInfo.pValue[i];	
				pField=pDataAccess->m_pFieldList->FieldByIndex(nCurCol);
				TMaxAndMin *pMaxAndMin=(TMaxAndMin *)(m_MaxAndMinPtrArray[nCount]);
				double max=pMaxAndMin->max;
				double min=pMaxAndMin->min;
				double mean=pMaxAndMin->mean;
				double temp=mean;
				if(!pField->IsNull())
					temp=pField->GetAsDouble();
				if(max==min)
					matrix(i,nCount)=max;
				else
					matrix(i,nCount)=(temp-min)/(max-min);
				if(m_bIsToEnd)
					break;
			}
			nCount++;
			pDataAccess->Next();
			
		}
		return matrix;
	}
	
}
//	TIDName(fInt,"����"),
//	TIDName(fDouble,"������"),
//	TIDName(fString,"�ַ���"),
//	TIDName(fBoolean,"������"),
//	TIDName(fDate,"������"),
//	TIDName(fCurrency,"������")

bool CNestedLoop::OutIntType(int nCol)
{
	int nType=m_pDataInterface->GetFieldType(nCol);
	if(nType==fInt||nType==fBoolean)
		return true;
	else
		return false;
}


bool	CNestedLoop::SetDataParam(CNestedVO *pVO)
{
	m_pNestedVO = pVO;
	return true;
}

CResult *  CNestedLoop::RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,CNestedVO *pVO)
{
	m_pDataInterface = pDataInterface;
	m_pNestedVO = pVO;
	m_nVarOrRec = pVO->nVarOrRec;
	m_nDistance = pVO->nDistance;
	m_nSolarity = pVO->nNum;
	m_bIsToEnd=false;
	m_pResult=NULL;
	m_pRsltElementText=NULL;
	m_tVarInfo.DeleteObject();
	int i;
	for( i=0;i<m_MaxAndMinPtrArray.GetSize();i++)
	{
		TMaxAndMin *p=(TMaxAndMin *)(m_MaxAndMinPtrArray.GetAt(i));
		delete p;
	}
	m_MaxAndMinPtrArray.RemoveAll();
	CTArray<int,int> tempArray;
	for(i=0;i<pVO->strInputName.GetSize();i++)
	{
		CTString strField = pVO->strInputName.GetAt(i);
		int nFieldNo = m_pDataInterface->IndexByName(strField);
		if(!m_pDataInterface->IsColEmpty(nFieldNo))
			tempArray.Add(nFieldNo);
	}
	int count=tempArray.GetSize();
	int *pValue=new int[count];
	for(int ii=0;ii<count;ii++)
		pValue[ii]=tempArray.GetAt(ii);
	tempArray.RemoveAll();
	TIntStruct is = TIntStruct(count,pValue);
	m_tVarInfo=is;
	//	delete[]pValue;
	return OnNestedLoop();
}


/////////////calculate the distance between record i and record j

double CNestedLoop::CalcDstij(CDoubleMatrix matrix,int i,int j)
{
	double sum=0;
	for(int n=0;n<matrix.mcol();n++)
		sum=sum+(matrix(i,n)-matrix(j,n))*(matrix(i,n)-matrix(j,n));
	return sum;

}


//matrix ԭʼ��¼����
//i ��i����¼
//k ƫ�����
//DataCPtrList�洢����i����¼�����С�������е�k����¼�ļ�¼�ź;����С��
//freshold ����������outCPtrList���Ѿ��洢��n����¼ʱ��freshold�洢��Ϊ��С����ֵ
//
//bTransΪtrueʱ����ʾoutCPtrList�м�¼�Ѿ���n��
double CNestedLoop::GetDkDst(CDoubleMatrix matrix,int i,int k,double freshold,bool bTrans)
{
	CPtrList DataCPtrList;
	double std=10E100;
	for(int n=0;n<matrix.mrow()&&(!m_bIsToEnd);n++)
	{	
		int count=DataCPtrList.GetCount();
		if(count==k&&bTrans)
			std=freshold;
		if(count==0)
		{		
			NestedStruct *pStruct=new NestedStruct;	
			pStruct->row=i;
			pStruct->value=CalcDstij(matrix,i,n);
			DataCPtrList.AddHead((void *)pStruct);
		}
		else if(count==k&&bTrans&&(((NestedStruct *)DataCPtrList.GetAt(DataCPtrList.FindIndex(k-1)))->value)<std)
			break;
		else		//��DataCPtrList�в������ĩβ׷���ܹ�k������ֵ��Ҫ��֤k��
		{			//���������о����д�С���������ǰk������ֵ
			double tempValue=CalcDstij(matrix,i,n);
			int flag=0;

			for(int j=0;j<DataCPtrList.GetCount();j++)     //���м����
			{
				DataCPtrList.FindIndex(j);
				POSITION pos = DataCPtrList.FindIndex(j);
				NestedStruct *p=(NestedStruct *)(DataCPtrList.GetAt(DataCPtrList.FindIndex(j)));
				double listValue=p->value;
				if(tempValue<listValue)
				{	
					NestedStruct *pStruct=new NestedStruct;	
					pStruct->row=i;
					pStruct->value=tempValue;

					DataCPtrList.InsertBefore((DataCPtrList.FindIndex(j)),(void *)pStruct);
					flag=1;
					break;
				}
			}
			if(DataCPtrList.GetCount()<k&&flag==0)//��DataCPtrList��û��k����¼����ʱ����û�����м���룬����ĩβ׷��
			{
				flag=1;
				NestedStruct *pStruct=new NestedStruct;	
				pStruct->row=i;
				pStruct->value=tempValue;
				DataCPtrList.AddTail((void *)pStruct);
			}
			if(DataCPtrList.GetCount()>k)//��DataCPtrList��¼����������k��ʱ����ʵΪk+1������ȥ����βһ��
			{
				NestedStruct *pp=(NestedStruct *)(DataCPtrList.GetAt(DataCPtrList.FindIndex(k)));
				delete pp;
				DataCPtrList.RemoveAt(DataCPtrList.FindIndex(k));
			}
		}
	}
	//����DataCPtrList����β�ļ�¼����ֵ����ɾ��DataCPtrList���ͷ��ڴ�
	double temp=0;
	if(DataCPtrList.GetCount()!=0)
	{
		POSITION pos = DataCPtrList.FindIndex(DataCPtrList.GetCount()-1);
		NestedStruct *p=(NestedStruct *)(DataCPtrList.GetAt(pos));
		temp=p->value;
	}
	POSITION pos=DataCPtrList.GetHeadPosition();
	while(pos!=NULL)
	{
		NestedStruct *pp=(NestedStruct *)(DataCPtrList.GetNext(pos));
		delete pp;
	}
	DataCPtrList.RemoveAll();
	return temp;


}


/////////////this is the main function
//kΪƫ�����
//nΪ���������ĸ���
//W_matrixΪ��¼����
//��outCPtrList �б���ƫ�����Ϊk��n���㣻
//
bool CNestedLoop::Nested(CDoubleMatrix W_matrix,int k,int n)
{
	//////////calculate the ordered matrix by the distance of W_matrix
/*
	if(m_pProcedureWnd)
	{
		CTString str1="���ڷ�������㡣";
		m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str1));
	}
*/

	CPtrList outCPtrList;//�������������
	double freshold=0;	//
	bool bTrans=false;
	NestedStruct *pStruct=new NestedStruct;
	pStruct->row=0;
	pStruct->value=GetDkDst(W_matrix,0,k+1,freshold,false);
	outCPtrList.AddHead(pStruct);
	if(outCPtrList.GetCount()==n)
	{
		POSITION pos = outCPtrList.FindIndex(outCPtrList.GetCount()-1);
		freshold=((NestedStruct *)(outCPtrList.GetAt(pos)))->value;
		bTrans=true;
	}

	for(int i=1;i<W_matrix.mrow();i++)
	{
		double tempValue=GetDkDst(W_matrix,i,k+1,freshold,bTrans);
		int flag=0;
		
		if(tempValue<=freshold&&outCPtrList.GetCount()>=n)
			continue;
		if(m_bIsToEnd)
			break;
		
		for(int j=0;j<outCPtrList.GetCount();j++)//��outCPtrList֮ǰ������м�����С�����n����¼

		{
			NestedStruct *pTemp=(NestedStruct *)(outCPtrList.GetAt(outCPtrList.FindIndex(j)));
			
			if(tempValue>pTemp->value)
			{
				NestedStruct *p=new NestedStruct;
				p->row=i;
				p->value=tempValue;
				outCPtrList.InsertBefore((outCPtrList.FindIndex(j)),(void *)p);
				flag=1;
				break;
			}
		}
		if(outCPtrList.GetCount()<n&&flag==0)//��outCPtrList��¼��δ��n������δ�����м���룬����outCPtrListĩβ׷��
		{
			NestedStruct *p=new NestedStruct;
			p->row=i;
			p->value=tempValue;
			outCPtrList.AddTail((void *)p);
		}
		
		if(outCPtrList.GetCount()>n)//outCPtrList��¼����n��ʱ��ɾ��ĩβһ��
		{
			POSITION pos = outCPtrList.FindIndex(outCPtrList.GetCount()-1);
			delete outCPtrList.GetAt(pos);
			outCPtrList.RemoveAt(pos);
		}
		if(outCPtrList.GetCount()==n)
		{
			POSITION pos = outCPtrList.FindIndex(outCPtrList.GetCount()-1);
			freshold=((NestedStruct *)(outCPtrList.GetAt(pos)))->value;
			bTrans=true;
		}
		
		
	}
/*
	if(m_pProcedureWnd)
	{
		CTString str="���ڸ�ʽ�������";
		m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str));
	}
*/
	if(!m_bIsToEnd)
		CreateResult(W_matrix,outCPtrList);
	POSITION pos=outCPtrList.GetHeadPosition();
	while(pos!=NULL)
		delete outCPtrList.GetNext(pos);
	outCPtrList.RemoveAll();
	return true;


}


////////////////output the result
void CNestedLoop::CreateResult(CDoubleMatrix W_matrix,CPtrList &outCPtrList)
{
	int n=outCPtrList.GetCount();
	CRsltElementTable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("�������б�");
	if(m_nVarOrRec==0)
	{
		pTable->SetCols(W_matrix.mcol()+2);
		pTable->SetRows(n+1);
		pTable->InsertColumn(0,"���");
		pTable->InsertColumn(1,"��¼��");
		CTString str;
		int i;
		for(i=0;i<W_matrix.mcol();i++)
		{		
			int currentSel=m_tVarInfo.pValue[i];
			pTable->InsertColumn(i+2,m_pDataInterface->GetFieldName(currentSel));
		}
		for(i=0;i<n;i++)
		{		
			str.Format("%d",i+1);
			pTable->InsertItem(i,str);
			NestedStruct *p=(NestedStruct *)(outCPtrList.GetAt(outCPtrList.FindIndex(i)));
			pTable->SetItemText(i,1,p->row+1);
			int row=p->row;
			double max=0;
			double min=0;
			bool bInt=false;
			for(int j=0;j<W_matrix.mcol();j++)
			{
				//if(j==0)
				//{	
				TMaxAndMin *pMaxAndMin=(TMaxAndMin *)(m_MaxAndMinPtrArray.GetAt(j));
				max=pMaxAndMin->max;
				min=pMaxAndMin->min;	
				bInt=OutIntType(m_tVarInfo.pValue[j]);
				//}
				//double temp=W_matrix[row][j];	
				double value=W_matrix(row,j)*(max-min)+min;
				if(bInt)
					pTable->SetItemText(i,j+2,int(value));
				else
					pTable->SetItemText(i,j+2,value);
			}

		}
		pETable= new CRsltElementTable("���������",pTable);//����һ���������Ķ���
		m_pResult->Add(pETable);
	}
	else
	{
		pTable->SetCols(n+1);
		pTable->SetRows(W_matrix.mcol()+1);
		pTable->InsertColumn(0,"��¼��");
		CTString str;
		int i;
		for(i=0;i<n;i++)
		{
			NestedStruct *p=(NestedStruct *)(outCPtrList.GetAt(outCPtrList.FindIndex(i)));
			int currentSel=m_tVarInfo.pValue[p->row];
			pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(currentSel));
		}
		for(i=0;i<W_matrix.mcol();i++)
		{		
			str.Format("%d",i+1);
			pTable->InsertItem(i,str);
			for(int j=0;j<n;j++)
			{
				NestedStruct *p=(NestedStruct *)(outCPtrList.GetAt(outCPtrList.FindIndex(j)));
				int currentSel=m_tVarInfo.pValue[p->row];
				int row=p->row;
				double max=0;
				double min=0;
				bool bInt=false;
				TMaxAndMin *pMaxAndMin=(TMaxAndMin *)(m_MaxAndMinPtrArray.GetAt(i));
				max=pMaxAndMin->max;
				min=pMaxAndMin->min;	
				bInt=OutIntType(m_tVarInfo.pValue[currentSel]);
				double value=W_matrix(row,i)*(max-min)+min;
				if(bInt)
					pTable->SetItemText(i,j+1,int(value));
				else
					pTable->SetItemText(i,j+1,value);
			}

		}
		pETable= new CRsltElementTable("���������",pTable);//����һ���������Ķ���
		m_pResult->Add(pETable);
	}

}

///////////////the interface function,to use it to call the main function
CResult * CNestedLoop::OnNestedLoop()
{
	m_pResult =new CResult("���������");
	CDataAccess *pDataAccess=&(m_pDataInterface->m_DataAccess);
/*
	if(m_pProcedureWnd)
	{
		CTString str="���ڵ������ݡ�";
		m_pProcedureWnd->SendMessage(WM_PROGRESS_TEXT,m_nComponentID,LPARAM(&str));
	}
*/
	CDoubleMatrix matrix=GetData();
	WriteData(matrix,"matrix");
	int n =m_pDataInterface->GetRowCount();
	int nCol=m_tVarInfo.iCount;
	double total=double(n)*m_tVarInfo.iCount;
	if (total>MAXDEAL)
	{
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("����");
		CTString str="����Դ�������Ƚ��г�������";
		m_pRsltElementText->AddString(str);
		m_pResult->Add(m_pRsltElementText);
		return m_pResult;
	}
	if(m_nVarOrRec==0)
	{
		if(m_nSolarity>n)
		{
			m_nSolarity=n;
		}
		if(m_nDistance>=n)
		{
			m_nDistance=n-1;
		}
		if(m_bIsToEnd)
		{
			return m_pResult;
		}
	}
	else
	{
		if(m_nSolarity>nCol)
		{
			m_nSolarity=nCol;
		}
		if(m_nDistance>=nCol)
		{
			m_nDistance=nCol-1;
		}
		if(m_bIsToEnd)
		{
			return m_pResult;
		}
	}
	Nested(matrix,m_nDistance,m_nSolarity);		
	return m_pResult;
		
} 


double CNestedLoop::GetMaxAndMin(int nColIndex,double &max,double &min)
{
	CField* pField;
	CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
	if(nColIndex>pDataAccess->m_pFieldList->GetFieldCount())
		return 0;
	double temp=0;
	pDataAccess->First();
	pField=pDataAccess->m_pFieldList->FieldByIndex(nColIndex);
	int count=0;
	double sum=0;
	while(!pDataAccess->Eof())
	{	
		if(!pField->IsNull())
		{
			temp=pField->GetAsDouble();
			sum=sum+temp;
		}
		else
		{
			count--;
		}
		if(count==0)
		{
			min=temp;
			max=min;			
		}
		else
		{
			min=min<temp?min:temp;
			max=max>temp?max:temp;
		}
		pDataAccess->Next();
		count++;
		if(m_bIsToEnd)
			break;

	}
	TMaxAndMin *p=new TMaxAndMin;
	p->max=max;
	p->min=min;
	p->mean=sum/count;
	m_MaxAndMinPtrArray.Add((void*)p);//�洢Ϊ�˻�ԭ��
	return sum/count;
}

double CNestedLoop::GetMaxAndMin()
{
	CField* pField;
	CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
	pDataAccess->First();
	while(!pDataAccess->Eof())
	{	
		int count=0;	
		int sum=0;
		double temp=0;
		double min=0,max=0;
		for(int i=0;i<m_tVarInfo.iCount;i++)
		{	
			int nCurCol=m_tVarInfo.pValue[i];
			pField=pDataAccess->m_pFieldList->FieldByIndex(nCurCol);
			if(!pField->IsNull())
			{
				temp=pField->GetAsDouble();
				sum=sum+temp;
			}

			else
			{
				count--;
			}
			if(count==0)
			{
				min=temp;
				max=min;			
			}
			else
			{
				min=min<temp?min:temp;
				max=max>temp?max:temp;
			}
			count++;
		}
		TMaxAndMin *p=new TMaxAndMin;
		p->max=max;
		p->min=min;
		p->mean=sum/m_tVarInfo.iCount;
		m_MaxAndMinPtrArray.Add((void*)p);//�洢Ϊ�˻�ԭ��
		pDataAccess->Next();
		
	}
	return 0;
}