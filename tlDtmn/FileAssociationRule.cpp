// FileAssociationRule.cpp: implementation of the CFileAssociationRule class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FileAssociationRule.h"
// #include "DlgProgress.h"
#include "Common.h"
//#include "..\tlss\dtmn\VisualProcessWnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//��strIn�ַ�������strCur�����ռ���strArray //#_S 2004-11-25 zlq
void AddString( CTStringArray &strArray,CTString &strCur,CTString strIn)
{
	if ( strCur.GetLength() > 200 )
	{
		strArray.Add(strCur);
		strCur = strIn;
	}
	else
	{
		strCur = strCur + strIn;
	}

}

//��CTStringArray��ֵ���ӳ�CTString//#_S 2004-11-25 zlq
CTString GetString( CTStringArray &strArray)
{
	int nLen = 0;
	int i;
	int nSize = strArray.GetSize();
	for ( i=0;i<nSize; i++)
	{
		nLen+= strArray.GetAt(i).GetLength();
	}
	CTString strOut;
	char *pBuf = new char[nLen+2];
	memset(pBuf+nLen,0,2);
	int nCurPos = 0;
	for (i=0;i<nSize;i++)
	{
		CTString str = strArray.GetAt(i);
		const char * psour = (const char *)str.GetData();//(const char *)CString 
		int nTempLen = strArray.GetAt(i).GetLength();
		memcpy(pBuf+nCurPos,psour,nTempLen);
		nCurPos += nTempLen;
	}
	strOut = pBuf;
	delete []pBuf;
	return strOut;
}

CFileAssociationRule::CFileAssociationRule()
{
	m_strFileName = "D:/Record.dat";
}

CFileAssociationRule::~CFileAssociationRule()
{

}

void	CFileAssociationRule::SetDataParam(CRuleTrainVO	*pVO)
{
}

void	CFileAssociationRule::SetDataInterface(CDataInterface *pDataInterface, CPtrArray *parField)
{
	m_pDataInterface = pDataInterface;
	m_tVarInfo.Copy(*parField);
}

/*
void CFileAssociationRule::prshl(TDataSet *p, int n,bool reSort)   //ʵ����shell��ϣ��������
{
    int k=0,j=0,i=0;
    TDataSet t;
    if (reSort)  //�����־λ����û������Ž���
	{
		k=n/2;
		while (k>0)
		{ 
			for (j=k; j<=n-1; j++)
			{
				t=p[j]; i=j-k;
				while ((i>=0)&&(p[i]>t))
				{
					p[i+k]=p[i];
					i=i-k;
				}
				p[i+k]=t;
			}
			k=k/2;
		}
	}
    return;
}
*/


/*
//////////prshl��������ݰ汾
//���㷨̫�����������Դ����ݵ������㷨
void CFileAssociationRule::prshl(TDColAry &ColAry, int n,bool reSort)   //ʵ����shell��ϣ��������
{
    int k=0,j=0,i=0;
    TDataCol tt;
    if (reSort)  //�����־λ����û������Ž���
	{
		k=n/2;
		while (k>0)
		{ 
			for (j=k; j<=n-1; j++)
			{
				tt=ColAry[j];
				i=j-k;
				while ((i>=0)&&(ColAry[i]>tt))
				{
					ColAry[i+k]=ColAry[i];
					i=i-k;
				}
				ColAry[i+k]=tt;					
			}
			k=k/2;
		}
	}
    return;
}
*/

//���ڹ鲢�����е�qsort����
int compare( const void *arg1, const void *arg2 )
{
	double v1 = (( TDataCol* ) arg1)->dValue;
	double v2 = (( TDataCol* ) arg2)->dValue;
	if (v1>v2)
		return 1;
	if (v1<v2)
		return -1;
	
	return 0;
}

//�����������С���ݲ�ͬ����Ҫ��ҳ���򣬼���Ӳ�̷��ʶ���
//�鲢��������
void CFileAssociationRule::MergeSort(TDColAry &ColAry, int nTotalValidCount)
{
	int nBufsize = 10000*50;
	TDataCol *pBuf = new TDataCol[nBufsize];

	TDColAry ColA;
	ColA.SetSize(0);
	TDColAry ColB;
	ColB.SetSize(0);
	
	TDColAry*pIn = &ColB;
	TDColAry*pOut = &ColA;
	int nCurPos =0;
	while(nCurPos<nTotalValidCount)
	{
		int nStartPos = nCurPos;
		int nEndPos = nStartPos+nBufsize;
		if (nEndPos >= nTotalValidCount)
			nEndPos = nTotalValidCount;
		
		for (int i= nStartPos ;i<nEndPos;i++)
			*(pBuf+i-nStartPos) = ColAry.GetAt(i);
		
		qsort(pBuf,nEndPos - nStartPos,sizeof(TDataCol),compare);
		
		//�鲢
		int nSizeA = nEndPos - nStartPos;
		int nSizeB = pIn->GetLen();
		if (nSizeB==0)
		{
			pOut->SetSize(nSizeA);
			for (int i=0;i<nSizeA ;i++)
				pOut->SetAt(i,*(pBuf+i));
		}
		else
		{
			pOut->SetSize(nSizeA+nSizeB);//���������С
			int nA = 0;
			int nB = 0;
			int nOut =0;
			while (nA<nSizeA || nB <nSizeB)
			{
				if (nA >=nSizeA )
				{
					pOut->SetAt(nOut++,pIn->GetAt(nB));
					nB++;
					continue;
				}
				if (nB >=nSizeB)
				{
					pOut->SetAt(nOut++,*(pBuf+nA));
					nA++;
					continue;
				}
				
				if (pIn->GetAt(nB).dValue > (pBuf+nA)->dValue)
				{
					pOut->SetAt(nOut++,*(pBuf+nA));
					nA++;
				}
				else
				{
					pOut->SetAt(nOut++,pIn->GetAt(nB));
					nB++;
				}
			}
			
		}
		
		TDColAry*pTemp = pOut;
		pOut = pIn;
		pIn = pTemp;
		//***********************************
		nCurPos = nEndPos;//���µ�ǰλ��
	}
	
	//�������� //#_S 2005-5-17 zlq
	for (int i=0;i<nTotalValidCount;i++)
	{
		ColAry.SetAt(i, pIn->GetAt(i));
	}
	delete [] pBuf;
}

/*
int CFileAssociationRule::RemoveDuplicate(TDataSet *pCustom,TDataSet *pProduct,int count)
{
	int nLen=count;
	long tempcustID=pCustom[count-1].dValue,tempprodID=pProduct[pCustom[count-1].iIndex-1].dValue;
	CTArray<long,long> tempset;
	tempset.Add(tempprodID);
	int nsetsize=0,j,k,nPrevEnd=count-1;
	TDataSet temp;
	for ( int i=count-2;i>=0;i--)
	{
		if ((long)(pCustom[i].dValue+0.5)==tempcustID)
		{
			nsetsize=tempset.GetSize();
			bool bFound=false;
			for (j=0;!bFound&&j<nsetsize;j++)
			{
				if(tempset.GetAt(j)==(long)(pProduct[pCustom[i].iIndex-1].dValue+0.5))
					bFound=true;
			}
			if (!bFound)
				tempset.Add((long)(pProduct[pCustom[i].iIndex-1].dValue+0.5));
			else
			{
				for (k=i;k<nLen-1;k++)
					pCustom[k]=pCustom[k+1];
				nLen--;
			}
		}
		else
		{
			tempset.RemoveAll();
			tempcustID=(long)(pCustom[i].dValue+0.5);
			tempset.Add((long)(pProduct[pCustom[i].iIndex-1].dValue+0.5));
		}
				

	}
	return nLen;
}
*/

/*
///////////////RemoveDuplicate�Ĵ����ݰ汾
int CFileAssociationRule::RemoveDuplicate(TDColAry &CustomArray,TDColAry &ProductArray,int count)
{
	int nLen=count;
	long tempcustID=CustomArray[count-1].dValue+0.5;	//��ʼ��Ϊ�ͻ���ʶ�����һ������
	CTArray<long,long> tempProdSet;	//������Ʒ��ʶ�������ͬ��Ʒ������̫�࣬����Բ���Ҫ������������
	
	tempProdSet.Add((long)(
		ProductArray[CustomArray[count-1].iIndex-1].dValue+0.5)); //����ͻ���ʶ��Ӧ����Ʒ��ʶ 
	int nsetsize=0,j,k;
	for ( int i=count-2;i>=0;i--)
	{
		//����ͻ���ʶ��ͬ��Ƚ���Ʒ��ʶ���ͻ���ʶ�Ѿ��ź���
		//		�����Ʒ��ʶ����ʱ�洢�����У���������ظ���¼��������ļ�¼ǰ�ƣ����Ǵ˼�¼
		//		���򽫴���Ʒ��ʶ���ӽ���ʱ�洢����
		//���򿪱��µ���ʱ�洢���У�����¼�µĿͻ���ʶ
		if ((long)(CustomArray[i].dValue+0.5)==tempcustID)  
		{
			nsetsize=tempProdSet.GetSize();
			bool bFound=false;
			for (j=0;!bFound&&j<nsetsize;j++)
			{
				if(tempProdSet.GetAt(j)==(long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5))
					bFound=true;
			}
			if (!bFound)
				tempProdSet.Add((long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5));
			else
			{
				for (k=i;k<nLen-1;k++)
					CustomArray[k]=CustomArray[k+1];
				nLen--;
			}
		}
		else
		{
			tempcustID=(long)(CustomArray[i].dValue+0.5);
			tempProdSet.RemoveAll();
			tempProdSet.Add((long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5));
		}
	}
	///�˴���ѹ���洢�ռ�//ask zlq
	return nLen;
}
*/


/*
//�����ݸ�Ч�汾��@_$ 2005-5-19 cx
//��ԭ������������޸ģ�ԭ�������ɺ���ǰ�����������ظ��򽫺���ļ�¼ǰ�Ƹ����ظ��ļ�¼
//���ڽ��ظ���¼��������������ǣ�����ٽ�δ��ǵļ�¼����һ��
//�Ժ�����д�������������򲿷ֽ���Ʒ�ֶ�Ҳ����Ȼ���ٴ�ǰ��������������ǣ�������¸���
int CFileAssociationRule::RemoveDuplicate_quick(TDColAry &CustomArray,TDColAry &ProductArray,int count)
{
	int nLen=count;
	long tempcustID=CustomArray[count-1].dValue+0.5;	//��ʼ��Ϊ�ͻ���ʶ�����һ������
	CTArray<long,long> tempProdSet;	//������Ʒ��ʶ�������ͬ��Ʒ������̫�࣬����Բ���Ҫ������������
	
	tempProdSet.Add((long)(
		ProductArray[CustomArray[count-1].iIndex-1].dValue+0.5)); //����ͻ���ʶ��Ӧ����Ʒ��ʶ 
	int nsetsize=0,j;
	
	//////////////////////////////////////////////////////////////////////////
	TDColAry tempColAry;
	tempColAry.SetSize(count);
	for(j=0;j<count;j++)
	{
		tempColAry[j]=CustomArray[j];
	}
	//////////////////////////////////////////////////////////////////////////

	for ( int i=count-2;i>=0;i--)
	{
		//����ͻ���ʶ��ͬ��Ƚ���Ʒ��ʶ���ͻ���ʶ�Ѿ��ź���
		//		�����Ʒ��ʶ����ʱ�洢�����У���������ظ���¼���Դ˼�¼�����
		//		���򽫴���Ʒ��ʶ���ӽ���ʱ�洢����
		//���򿪱��µ���ʱ�洢���У�����¼�µĿͻ���ʶ
		if ((long)(tempColAry[i].dValue+0.5)==tempcustID)  
		{
			nsetsize=tempProdSet.GetSize();
			bool bFound=false;
			for (j=0;!bFound&&j<nsetsize;j++)
			{
				if(tempProdSet.GetAt(j)==(long)(ProductArray[tempColAry[i].iIndex-1].dValue+0.5))
					bFound=true;
			}
		//	if (!bFound)
		//		tempProdSet.Add((long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5));
		//	else
		//	{
		//		for (k=i;k<nLen-1;k++)
		//			CustomArray[k]=CustomArray[k+1];
		//		nLen--;
		//	}
			//////////////////////////////////////////////////////////////////////////
			if (!bFound)
			{
				tempProdSet.Add((long)(ProductArray[tempColAry[i].iIndex-1].dValue+0.5));
			}
			else
			{
				tempColAry[i].iIndex=-100;
				nLen--;
			}
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			tempcustID=(long)(tempColAry[i].dValue+0.5);
			tempProdSet.RemoveAll();
			tempProdSet.Add((long)(ProductArray[tempColAry[i].iIndex-1].dValue+0.5));
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	CustomArray.SetSize(nLen);
	nLen=0;
	for(i=0;i<count;i++)
	{
		if(tempColAry[i].iIndex!=-100)
		{
			CustomArray[nLen]=tempColAry[i];
			nLen++;
		}
	}	
	//////////////////////////////////////////////////////////////////////////
		
	return nLen;
}
*/


//int CFileAssociationRule::RemoveDuplicate_quick_test(TDColAry &CustomArray,TDColAry &ProductArray,int count,void * pt)
int CFileAssociationRule::RemoveDuplicate_quick_test(TDColAry &CustomArray,TDColAry &ProductArray,int count)
{
	//////////////////////////////////////////////////////////////////////////
	//CTString strText;	
	//TMsgInfo * pMsg = (TMsgInfo*)pt;
	//////////////////////////////////////////////////////////////////////////

	int nLen=count;
	long tempcustID=CustomArray[count-1].dValue+0.5;	//��ʼ��Ϊ�ͻ���ʶ�����һ������
	long tempprodID;
	CTArray<long,long> tempProdSet;	//������Ʒ��ʶ�������ͬ��Ʒ������̫�࣬����Բ���Ҫ������������
	tempProdSet.Add((long)(
		ProductArray[CustomArray[count-1].iIndex-1].dValue+0.5)); //����ͻ���ʶ��Ӧ����Ʒ��ʶ 
	int nsetsize=0,j;
	
	//����CustomArray����ʱ����tempColAry	
	TDColAry tempColAry;
	tempColAry.SetSize(count);
	for(j=0;j<count;j++)
	{
		tempColAry[j]=CustomArray[j];
	}
	
	//ȥ���ظ���¼
	int i;
	for (i=count-2;i>=0;i--)
	{
		
		//���󣺴����ٶ�������죿������
		//////////////////////////////////////////////////////////////////////////
		//strText.Format( "����ȥ���ظ���¼������������%d", i+1 );
		//pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&strText);
		//////////////////////////////////////////////////////////////////////////
		
		
		//����ͻ���ʶ��ͬ��Ƚ���Ʒ��ʶ���ͻ���ʶ�Ѿ��ź���
		//		�����Ʒ��ʶ����ʱ�洢�����У���������ظ���¼���Դ˼�¼�����
		//		���򽫴���Ʒ��ʶ���ӽ���ʱ�洢����
		//���򿪱��µ���ʱ�洢���У�����¼�µĿͻ���ʶ
		if ((long)(tempColAry[i].dValue+0.5)==tempcustID)  
		{
			nsetsize=tempProdSet.GetSize();
			bool bFound=false;
		
			tempprodID=(long)(ProductArray[tempColAry[i].iIndex-1].dValue+0.5);
			for (j=0;!bFound&&j<nsetsize;j++)
			{
				if(tempProdSet.GetAt(j)==tempprodID)						
					bFound=true;

			}
			if (!bFound)
			{
				tempProdSet.Add(tempprodID);
			}
			else
			{
				tempColAry[i].iIndex=-100;
				nLen--;
			}
		}
		else
		{
			tempcustID=(long)(tempColAry[i].dValue+0.5);
			tempProdSet.RemoveAll();
			tempProdSet.Add((long)(ProductArray[tempColAry[i].iIndex-1].dValue+0.5));
		}
	}
	
	CustomArray.SetSize(nLen);
	nLen=0;
	for(i=0;i<count;i++)
	{
		if(tempColAry[i].iIndex!=-100)
		{
			CustomArray[nLen]=tempColAry[i];
			nLen++;
		}
	}	
	
	//�������ظ���¼������
	return nLen;
}



//add by liytao 20030217
/*int CFileAssociationRule::GetValidData(CDataInterface *pDataInterface,int nCustCol,int nProductCol,TDataSet *&pCustArray,TDataSet *&pProductArray)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	int nCount=0,nSize=0;
	long nRecord=pAccess->GetRecordCount();
	if (nRecord<=0 )
		return 0;
	pCustArray=new TDataSet[nRecord];
	pProductArray=new TDataSet[nRecord];
	pAccess->First();
	int nRowIndex=1,nCustFieldType,nProductFieldType,id=-1;
	CField* pCustField,*pProductField;
	while(!pAccess->Eof())
	{
		bool bInvalid=false;
		pCustField=pAccess->m_pFieldList->FieldByIndex(nCustCol);
		nCustFieldType=pCustField->GetFieldType();
		if( pDataInterface->IsMissingValue( pCustField, true))//(pField->IsNull()))
		{
				nRowIndex++;
				pAccess->Next();
				continue;

		}
		pProductField=pAccess->m_pFieldList->FieldByIndex(nProductCol);
		nProductFieldType=pProductField->GetFieldType();

		if(!pDataInterface->IsMissingValue( pProductField, true))//(pField->IsNull()))
		{
			if(nCustFieldType==fString)
				pCustArray[nCount].strVal=pCustField->SetToDisplay();
			else
			{
				pCustArray[nCount].dValue=pCustField->GetAsDouble();
			}			
			if(nProductFieldType==fString)
				pProductArray[nCount].strVal=pProductField->SetToDisplay();
			else
			{
				pProductArray[nCount].dValue=pProductField->GetAsDouble();
			}			

			pCustArray[nCount].iIndex=nRowIndex;
			pProductArray[nCount++].iIndex=nRowIndex;
		}

		nRowIndex++;
		pAccess->Next();	
	}
	if (nCount<=0)
	{
		delete []pCustArray;
		pCustArray=NULL;
		delete []pProductArray;
		pProductArray=NULL;
	}
	return nCount;
}
//add end
*/

////source code -- usable ////
/*
int CFileAssociationRule::GetValidData(CDataInterface *pDataInterface,int nCustCol,
		int nProductCol,TDataSet *&pCustArray,TDataSet *&pProductArray,bool bString)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	int nCount=0,nSize=0;
	long nRecord=pAccess->GetRecordCount();
	if (nRecord<=0 )
		return 0;
	pCustArray=new TDataSet[nRecord];
	pProductArray=new TDataSet[nRecord];
	//#_S 2004-11-25 zlq   ��������ֵ
	TDataSet *pRealCustIDAry = new TDataSet[nRecord];

	pAccess->First();
	int nRowIndex=1,nCustFieldType,nProductFieldType,id=-1;
	CField* pCustField,*pProductField;
	while(!pAccess->Eof())
	{
		bool bInvalid=false;
		pCustField=pAccess->m_pFieldList->FieldByIndex(nCustCol);
		nCustFieldType=pCustField->GetFieldType();
		if( pDataInterface->IsMissingValue( pCustField, true))//(pField->IsNull()))
		{
				nRowIndex++;
				pAccess->Next();
				continue;

		}
		pProductField=pAccess->m_pFieldList->FieldByIndex(nProductCol);
		nProductFieldType=pProductField->GetFieldType();

		if(!pDataInterface->IsMissingValue( pProductField, true))//(pField->IsNull()))
		{
			TDataSet tempdata;
			int idindex;
			if(nCustFieldType==fString)
			{
				tempdata.strVal=pCustField->SetToDisplay();
				pRealCustIDAry[nCount].strVal = tempdata.strVal;
			}
			else
			{
				tempdata.dValue=pCustField->GetAsDouble();
				pRealCustIDAry[nCount].dValue = tempdata.dValue;
			}			
			//add into group;
			idindex=grouper.getIDIndex(0,tempdata);
			if (idindex<0){
				grouper.add(0,nCustFieldType,tempdata);
				idindex=grouper.getIDIndex(0,tempdata);

			}
			pCustArray[nCount].dValue=grouper.getID(idindex);
			if(nProductFieldType==fString)
				tempdata.strVal=pProductField->SetToDisplay();
			else
			{
				tempdata.dValue=pProductField->GetAsDouble();
			}			

			idindex=grouper.getIDIndex(1,tempdata);
			if (idindex<0){
				grouper.add(1,nProductFieldType,tempdata);
				idindex=grouper.getIDIndex(1,tempdata);

			}
			pProductArray[nCount].dValue=grouper.getID(idindex);

			pCustArray[nCount].iIndex=nRowIndex;
			pProductArray[nCount++].iIndex=nRowIndex;


		}

		nRowIndex++;
		pAccess->Next();	
	}

	//#_S 2004-11-25 zlq
	m_nCustomCount = GetDifferCount(pRealCustIDAry,nCount,nCustCol,m_pDataInterface);
	delete [] pRealCustIDAry;

	if (nCount<=0)
	{
		delete []pCustArray;
		pCustArray=NULL;
		delete []pProductArray;
		pProductArray=NULL;
	}
	return nCount;
}
*/

/*
//������룬Դ��������汻ע�͵��ĺ��� -cx  2005-5
int CFileAssociationRule::GetValidData(CDataInterface *pDataInterface,int nCustCol,
		   int nProductCol,TDataSet *&pCustArray,TDataSet *&pProductArray,bool bString)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	int nCount=0;								//��Ч��¼�� 
	long nRecord=pAccess->GetRecordCount();		//�ܼ�¼��
	if (nRecord<=0)return 0;
	pCustArray=new TDataSet[nRecord];			//�ͻ���
	pProductArray=new TDataSet[nRecord];		//��Ʒ��
	
	pAccess->First();
	int nRowIndex=1,nCustFieldType,nProductFieldType;
	CField* pCustField,*pProductField;
	
	m_nCustomCount=0;
	while(!pAccess->Eof())
	{
		/////////////////����Ӧ���ᵽѭ��������
		pCustField=pAccess->m_pFieldList->FieldByIndex(nCustCol);	//�õ����ݿ��пͻ���ָ��
		nCustFieldType=pCustField->GetFieldType();					//�õ����ݿ��пͻ�������
		/////////////////

		if( pDataInterface->IsMissingValue( pCustField, true))//(pField->IsNull()))
		{
			nRowIndex++;
			pAccess->Next();
			continue;
		}
		
		//////////����Ӧ���ᵽѭ��������
		pProductField=pAccess->m_pFieldList->FieldByIndex(nProductCol);//�õ����ݿ�����Ʒ��ָ��
		nProductFieldType=pProductField->GetFieldType();			//�õ����ݿ�����Ʒ������
		/////////////////////

		if(!pDataInterface->IsMissingValue( pProductField, true))//(pField->IsNull()))
		{
			TDataSet tempdata;
			int idindex;
			
			if(nCustFieldType==fString)
			{
				tempdata.strVal=pCustField->SetToDisplay();		//�õ��ͻ��ֶε�ǰɨ�赽���ַ���
			}
			else
			{
				tempdata.dValue=pCustField->GetAsDouble();		//�õ��ͻ��ֶε�ǰɨ�赽��˫������
			}			
			//add into group;
			//grouper�洢��ͬ�Ŀͻ�ID����ƷID���Լ���Ӧ�����ֱ�ʶ��
			//��˴洢������̫�󣬿�����ʱ�����Ǵ�����������
			idindex=grouper.getIDIndex(0,tempdata);		//�õ���ǰɨ�����ݵ�����(int)
			if (idindex<0){
				grouper.add(0,nCustFieldType,tempdata); //������в����ڴ����ݣ������ӣ�
				idindex=grouper.getIDIndex(0,tempdata); //�ٵõ�����				
				m_nCustomCount++;		//�Ժ���ܿ�����GroupingInfo�õ���ͬ�Ŀͻ���Ŀ
			}
			pCustArray[nCount].dValue=grouper.getID(idindex);  //�õ�����ֵ��Ӧ����ֵ

			if(nProductFieldType==fString)
				tempdata.strVal=pProductField->SetToDisplay();	//�õ���Ʒ�ֶε�ǰɨ�赽���ַ���
			else
			{
				tempdata.dValue=pProductField->GetAsDouble();	//�õ���Ʒ�ֶε�ǰɨ�赽��˫������
			}			
			//add into group;
			idindex=grouper.getIDIndex(1,tempdata);
			if (idindex<0){
				grouper.add(1,nProductFieldType,tempdata);
				idindex=grouper.getIDIndex(1,tempdata);				
			}
			pProductArray[nCount].dValue=grouper.getID(idindex);//�õ�����ֵ��Ӧ����ֵ
																//��һ��grouper����ͻ����ݺ���Ʒ����
			
//@_$ 2005-5-10 cx �ش�bug�޸�!!!
//��bug���������������ȱʧֵʱ����������ȷ��
//nRowIndexΪ����ȱʧֵ���ܼ�¼�������������������iIndex��������˴�Ӧ�õ�����Ч��¼����
			//pCustArray[nCount].iIndex=nRowIndex;	
			//pProductArray[nCount].iIndex=nRowIndex;		
			pCustArray[nCount].iIndex=nCount+1;		//�ͻ����������ֵΪ��ǰ��Ч��¼������
			pProductArray[nCount].iIndex=nCount+1;	//��Ʒ���������ֵΪ��ǰ��Ч��¼������
//�Ժ���Ҫ��pCustArray���������Ҫ��¼����ԭʼ��������ʹ����pProductArray������Ӧ
//���Գ���֮����԰�nRowIndex����ɾȥ
/////////////////////////////////////////////////

			nCount++;  //��Ч��¼����1
		}
		
		nRowIndex++;	//�ܼ�¼������1
		pAccess->Next();	
	}
	
///�˴����Ծ���pCustArray��pProductArray�ڴ棬ʹ���С������Ч��¼��
///ע�ⲻҪ��RemoveDuplicate�����ظ����򣨴�����
	if (nCount<=0)
	{
		delete []pCustArray;
		pCustArray=NULL;
		delete []pProductArray;
		pProductArray=NULL;
	}
	return nCount;
}
*/


//@_$ 2005-5-16 cx
//�ص��滻pCustArray��pProductArray���������ͣ�ʹ��֧�ִ�������,�滻ǰ�ĳ������������ĺ���
//����һ���Ż�����
//�˴�pCustArray��pProductArray�����������ݾͿ�����
//TDColAry�е�TDataCol�����������Ϊ���Σ����ڴ�������ʱ��ɽ�ʡ�ܶ�ռ�
//grouper�е����ݿ���û�������ڴ������������ʱ��ÿ�ζ�Ҫ˳������������ݣ��ٶȻ�Ƚ���
//*********************************************
//   nCustCol���ͻ�����������
//   nProductCol����Ʒ����������
//   CustomArray���ͻ�����
//	 ProductArray����Ʒ����
//���ܣ���ÿͻ��ֶκ���Ʒ�ֶε���Ч��¼���ݣ�������ֵ�����ԭ��������(��ֵ�����ַ���)��
//		������ԭ�����ݵĶ�Ӧ��ϵ�����棬������ֵ������CustomArray��ProductArray�У�������Ч��¼��
int CFileAssociationRule::GetValidData(CDataInterface *pDataInterface,int nCustCol,
		   int nProductCol,TDColAry &CustomArray,TDColAry &ProductArray)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	long nRecord=pAccess->GetRecordCount();		//�ܼ�¼��
	if (nRecord<=0)return 0;
	
	CField* pCustField,*pProductField;
	int nCustFieldType,nProductFieldType;
	pCustField=pAccess->m_pFieldList->FieldByIndex(nCustCol);	//�õ����ݿ��пͻ���ָ��
	nCustFieldType=pCustField->GetFieldType();					//�õ����ݿ��пͻ�������
	pProductField=pAccess->m_pFieldList->FieldByIndex(nProductCol);//�õ����ݿ�����Ʒ��ָ��
	nProductFieldType=pProductField->GetFieldType();			//�õ����ݿ�����Ʒ������

//	pCustArray=new TDataSet[nRecord];			//�ͻ���
//	pProductArray=new TDataSet[nRecord];		//��Ʒ��
	CustomArray.SetSize(nRecord);			//�ͻ���
	ProductArray.SetSize(nRecord);		//��Ʒ��
	
	pAccess->First();
	int nCount=0;						//��Ч��¼�� 
	m_nCustomCount=0;					//��ͬ�ͻ���Ŀ
	TDataCol temp1;

	while(!pAccess->Eof())
	{
		if( pDataInterface->IsMissingValue( pCustField, true))
		{
			pAccess->Next();
			continue;
		}
		if(!pDataInterface->IsMissingValue( pProductField, true))
		{
			TDataSet tempdata;
			int idindex;
			
			if(nCustFieldType==fString)
			{
				tempdata.strVal=pCustField->GetAsString();		//�õ��ͻ��ֶε�ǰɨ�赽���ַ���
			}
			else
			{
				tempdata.dValue=pCustField->GetAsDouble();		//�õ��ͻ��ֶε�ǰɨ�赽��˫������
			}			
			//add into group;
			//grouper�洢��ͬ�Ŀͻ�ID����ƷID���Լ���Ӧ�����ֱ�ʶ��
			//��˴洢������̫�󣬿�����ʱ�����Ǵ�����������
			idindex=grouper.getIDIndex(0,tempdata);		//�õ���ǰɨ�����ݵ�����(int)
			if (idindex<0){
				grouper.add(0,nCustFieldType,tempdata); //������в����ڴ����ݣ������ӣ�
				idindex=grouper.getIDIndex(0,tempdata); //�ٵõ�����				
				m_nCustomCount++;		//�Ժ���ܿ�����GroupingInfo�õ���ͬ�Ŀͻ���Ŀ
			}
			//pCustArray[nCount].dValue=grouper.getID(idindex);  //�õ�����ֵ��Ӧ����ֵ
			//pCustArray[nCount].iIndex=nCount+1;		//�ͻ����������ֵΪ��ǰ��Ч��¼������
			temp1.dValue=grouper.getID(idindex);	//�õ�����ֵ��Ӧ����ֵ
			temp1.iIndex=nCount+1;					//�ͻ����������ֵΪ��ǰ��Ч��¼������
			CustomArray.SetAt(nCount,temp1);

			if(nProductFieldType==fString)
			{
				tempdata.strVal=pProductField->GetAsString();	//�õ���Ʒ�ֶε�ǰɨ�赽���ַ���
			}
			else
			{
				tempdata.dValue=pProductField->GetAsDouble();	//�õ���Ʒ�ֶε�ǰɨ�赽��˫������
			}			
			//add into group; //��һ��grouper����ͻ����ݺ���Ʒ����
			idindex=grouper.getIDIndex(1,tempdata);
			if (idindex<0){
				grouper.add(1,nProductFieldType,tempdata);
				idindex=grouper.getIDIndex(1,tempdata);				
			}
			//pProductArray[nCount].dValue=grouper.getID(idindex);//�õ�����ֵ��Ӧ����ֵ
			//pProductArray[nCount].iIndex=nCount+1;	//��Ʒ���������ֵΪ��ǰ��Ч��¼������
			temp1.dValue=grouper.getID(idindex);	
			temp1.iIndex=nCount+1;
			ProductArray.SetAt(nCount,temp1);
	
//@_$ 2005-5-10 cx �ش�bug�޸�!!!
//��bug���������������ȱʧֵʱ����������ȷ��
//nRowIndexΪ����ȱʧֵ���ܼ�¼�������������������iIndex��������˴�Ӧ�õ�����Ч��¼����
			//pCustArray[nCount].iIndex=nRowIndex;	
			//pProductArray[nCount].iIndex=nRowIndex;		
//			pCustArray[nCount].iIndex=nCount+1;		//�ͻ����������ֵΪ��ǰ��Ч��¼������
//			pProductArray[nCount].iIndex=nCount+1;	//��Ʒ���������ֵΪ��ǰ��Ч��¼������
//�Ժ���Ҫ��pCustArray���������Ҫ��¼����ԭʼ��������ʹ����pProductArray������Ӧ
/////////////////////////////////////////////////

			nCount++;  //��Ч��¼����1
		}
		
		pAccess->Next();	
	}
	
///�˴����Ծ���CustomArray��ProductArray�ڴ棬ʹ���С������Ч��¼��, ask zlq
///ע�ⲻҪ��RemoveDuplicate�����ظ����򣨴�����
	if (nCount<=0)
	{
		//delete []pCustArray;
		//pCustArray=NULL;
//CustArray.Clear();
		//delete []pProductArray;
		//pProductArray=NULL;
//ProductArray.Clear();
	}
	return nCount;
}


/*
bool CFileAssociationRule::Aproiri_gen12(TDataSet* pCust,TDataSet * pProduct,int nRecCount, CPtrArray *par, int nk, int mk, CPtrArray *parCk,void * lp,bool * pStop,int nMiniSupport)
{
	CProgressCtrl * pBar = NULL;

	TUnit * pt1 = NULL;	
	TUnit * pt2 = NULL;
	int *index1 = NULL;
	int *index2 = NULL;
	int *nt1 = NULL;
	int *nt2 = NULL;
	int num = 1;
	int mark = 1;
	int i,l=0,k=0,j=0;
	TUnit * pcandidateItem = NULL;
	//create 2-D array
	int nLen=par->GetSize();
	CTArray<long,long> tempprod;
	long *valarray=new long[nLen];// allocate array to store freqs with value stored at col 0
	int *freqarray=new int[nLen*nLen];// allocate array to store freqs with value stored at col 0
	for (i=0;i<nLen;i++)
	{
		if( pStop )
		{
			if (*pStop == true )
			{
				if( valarray!=NULL)
					delete []valarray;
				if( freqarray!=NULL)
					delete []freqarray;
				valarray=NULL;
				freqarray=NULL;
				return true;
			}
		}
		TUnit* temp=(TUnit*)par->GetAt(i);
		valarray[i]=*((long*)temp->arIndex.GetAt(0));
		for (j=0;j<nLen;j++)
			freqarray[i*nLen+j]=0;
	}
	i=0;
	long tempcustID;
	for (i=0;i<nRecCount;i++)
	{
		//add by liytao 20030226
		if( pStop && *pStop == true )
		{
			tempprod.RemoveAll();
			if( valarray!=NULL)
				delete []valarray;
			if( freqarray!=NULL)
				delete []freqarray;
			valarray=NULL;
			freqarray=NULL;
			return true;
		}

		//add end
		
		if (tempprod.GetSize()==0)
		{
			tempcustID=(long)(pCust[i].dValue+0.5);	
			tempprod.Add((long)(pProduct[pCust[i].iIndex-1].dValue+0.5));

		}
		else
		{
			if( (long)(pCust[i].dValue+0.5)==tempcustID)
				tempprod.Add((long)(pProduct[pCust[i].iIndex-1].dValue+0.5));
			else
			{
				if( lp )
				{
					pBar->StepIt();
					if( pBar->GetPos() == 100 )
					{
						pBar->SetRange(1,100);
						pBar->SetStep(1);
					}
				}
				//��ʼ�����µĿͻ������ۼ�¼,������һ���ͻ�������
				CountItem(&tempprod,valarray,freqarray,nLen,pStop);
				//���arClient	
				tempprod.RemoveAll();
				//�����µĿͻ�����
				tempprod.Add((long)(pProduct[pCust[i].iIndex-1].dValue+0.5));
				tempcustID=(long)(pCust[i].dValue+0.5);


			}


		}
		if( i==nRecCount-1)
		{
			CountItem(&tempprod,valarray,freqarray,nLen,pStop);
			tempprod.RemoveAll();
			break;
		}
	}
	if( pStop )
	{
		if (*pStop == true )
		{
			if( valarray!=NULL)
				delete []valarray;
			if( freqarray!=NULL)
				delete []freqarray;
			valarray=NULL;
			freqarray=NULL;
			return true;
		}
	}

	for ( i=0;i<nLen-1;i++)
	{
		for ( j=i+1;j<nLen;j++)
		{
			if( pStop )
			{
				if (*pStop == true )
				{
					if( valarray!=NULL)
						delete []valarray;
					if( freqarray!=NULL)
						delete []freqarray;
					valarray=NULL;
					freqarray=NULL;
					return true;
				}
			}
			if ( freqarray[i*nLen+j]>=nMiniSupport)
			{

					pcandidateItem = new TUnit;
					long *temp;
					temp=new long;
					*temp=valarray[i];
					pcandidateItem->arIndex.Add(temp);
					temp=new long;
					*temp=valarray[j];
					pcandidateItem->arIndex.Add(temp);
					pcandidateItem->count=freqarray[i*nLen+j];
					parCk->Add( pcandidateItem );
			}

		}

	}
	
	//free space allocated for array
	if( valarray!=NULL)
		delete []valarray;
	if( freqarray!=NULL)
		delete []freqarray;
	valarray=NULL;
	freqarray=NULL;
	if( parCk->GetSize() > 0 )
	{
		return false;
	}
	else
		return true;
}
*/

//Aproiri_gen12֧�ִ����ݰ汾
bool CFileAssociationRule::Aproiri_gen12(TDColAry &CustomArray,TDColAry &ProductArray,int nRecCount, CPtrArray *par, int nk, int mk, CPtrArray *parCk/*,void * lp*/,bool * pStop,int nMiniSupport)
{
// 	CProgressCtrl * pBar = NULL;
	TUnit * pt1 = NULL;	
	TUnit * pt2 = NULL;
	int *index1 = NULL;
	int *index2 = NULL;
	int *nt1 = NULL;
	int *nt2 = NULL;
	int num = 1;
	int mark = 1;
	int i,l=0,k=0,j=0;
	TUnit * pcandidateItem = NULL;
	//create 2-D array
	int nLen=par->GetSize();
	CTArray<long,long> tempprod;
	long *valarray=new long[nLen];// allocate array to store freqs with value stored at col 0
	int *freqarray=new int[nLen*nLen];// allocate array to store freqs with value stored at col 0
	for (i=0;i<nLen;i++)
	{
		if( pStop )
		{
			if (*pStop == true )
			{
				if( valarray!=NULL)
					delete []valarray;
				if( freqarray!=NULL)
					delete []freqarray;
				valarray=NULL;
				freqarray=NULL;
				return true;
			}
		}
		TUnit* temp=(TUnit*)par->GetAt(i);
		valarray[i]=*((long*)temp->arIndex.GetAt(0));
		for (j=0;j<nLen;j++)
			freqarray[i*nLen+j]=0;
	}
	i=0;
	long tempcustID;
	for (i=0;i<nRecCount;i++)
	{
		//add by liytao 20030226
		if( pStop && *pStop == true )
		{
			tempprod.RemoveAll();
			if( valarray!=NULL)
				delete []valarray;
			if( freqarray!=NULL)
				delete []freqarray;
			valarray=NULL;
			freqarray=NULL;
			return true;
		}

		//add end
		
		if (tempprod.GetSize()==0)
		{
			//tempcustID=(long)(pCust[i].dValue+0.5);	
			//tempprod.Add((long)(pProduct[pCust[i].iIndex-1].dValue+0.5));
			tempcustID=(long)(CustomArray[i].dValue+0.5);	
			tempprod.Add((long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5));
		}
		else
		{
			//if( (long)(pCust[i].dValue+0.5)==tempcustID)
			//	tempprod.Add((long)(pProduct[pCust[i].iIndex-1].dValue+0.5));
			if( (long)(CustomArray[i].dValue+0.5)==tempcustID)
					tempprod.Add((long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5));
			else
			{
/*
				if( lp )
				{
					pBar->StepIt();
					if( pBar->GetPos() == 100 )
					{
						pBar->SetRange(1,100);
						pBar->SetStep(1);
					}
				}
*/
				//��ʼ�����µĿͻ������ۼ�¼,������һ���ͻ�������
				CountItem(&tempprod,valarray,freqarray,nLen,pStop);
				//���arClient	
				tempprod.RemoveAll();
				//�����µĿͻ�����
				//tempprod.Add((long)(pProduct[pCust[i].iIndex-1].dValue+0.5));
				//tempcustID=(long)(pCust[i].dValue+0.5);
				tempprod.Add((long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5));
				tempcustID=(long)(CustomArray[i].dValue+0.5);
			}
		}
		if( i==nRecCount-1)
		{
			CountItem(&tempprod,valarray,freqarray,nLen,pStop);
			tempprod.RemoveAll();
			break;
		}
	}
	if( pStop )
	{
		if (*pStop == true )
		{
			if( valarray!=NULL)
				delete []valarray;
			if( freqarray!=NULL)
				delete []freqarray;
			valarray=NULL;
			freqarray=NULL;
			return true;
		}
	}

	for ( i=0;i<nLen-1;i++)
	{
		for ( j=i+1;j<nLen;j++)
		{
			if( pStop )
			{
				if (*pStop == true )
				{
					if( valarray!=NULL)
						delete []valarray;
					if( freqarray!=NULL)
						delete []freqarray;
					valarray=NULL;
					freqarray=NULL;
					return true;
				}
			}
			if ( freqarray[i*nLen+j]>=nMiniSupport)
			{

					pcandidateItem = new TUnit;
					long *temp;
					temp=new long;
					*temp=valarray[i];
					pcandidateItem->arIndex.Add(temp);
					temp=new long;
					*temp=valarray[j];
					pcandidateItem->arIndex.Add(temp);
					pcandidateItem->count=freqarray[i*nLen+j];
					parCk->Add( pcandidateItem );
			}

		}

	}
	
	//free space allocated for array
	if( valarray!=NULL)
		delete []valarray;
	if( freqarray!=NULL)
		delete []freqarray;
	valarray=NULL;
	freqarray=NULL;
	if( parCk->GetSize() > 0 )
	{
		return false;
	}
	else
		return true;
}

/*
//add by liytao 20030821 //===�ɽ�Ϊ ��ά�����������
void CFileAssociationRule::CreateFrequentSet(void * pt,bool bString)
{
	unsigned int nComponentID;
	CProgressCtrl * pBar = NULL;
	TMsgInfo * pMsg = (TMsgInfo*)pt;
	CDlgProgress * lp = NULL;
	bool bStop = false;
	TDataSet *pCustom=NULL,*pProduct=NULL;
	m_pResult = new CResult("��������");

	int nCount=0;	
	if( pt )
	{
		m_nCustomCol = pMsg->nCustom;
		m_nProductCol = pMsg->nProduct;
		m_pDataInterface = pMsg->pDataInterface;
		m_pMsgWnd	= pMsg->pMsgWnd;

		lp = pMsg->pDlgProgress;
		nComponentID = pMsg->nComponentID;
		pProcessWnd = pMsg->pProgressWnd;
		
		CTString strText;
		strText.Format( "���ڽ������ݳ�ʼ������..." );
		pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&strText);


		grouper.reset();
		grouper.setGrpCount(2);
		nCount=GetValidData(m_pDataInterface,m_nCustomCol,m_nProductCol,pCustom,pProduct,bString);

		if (nCount>0)
		{
			if(!bStopWork)
				prshl(pCustom,nCount,true);
			if(!bStopWork)
				nCount=RemoveDuplicate(pCustom,pProduct,nCount);
			if(bStopWork)
			{
				if( pCustom!=NULL)
					delete []pCustom;
				if( pProduct)
					delete []pProduct;
				pCustom=NULL;
				pProduct=NULL;
				if(m_pResult)
					delete m_pResult;
				m_pResult=NULL;
				m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
				CWinThread * pThread = AfxGetThread();
				DWORD nexit;
				::GetExitCodeThread(pThread->m_hThread,&nexit);
				AfxEndThread(nexit,true);
				return;
			}
		}
		else
		{
			CRsltElementText *pText=new CRsltElementText("����");
			CTString str;
			str=_T("����Ч��¼���޷�����\n");
			pText->AddString(str);
			m_pResult->Add(pText);
			m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
			CWinThread * pThread = AfxGetThread();
			DWORD nexit;
			::GetExitCodeThread(pThread->m_hThread,&nexit);
			AfxEndThread(nexit,true);
			return ;
		}
	}

	if( lp )
	{
		pBar=(CProgressCtrl *)(lp->GetDlgItem(IDC_PROGRESS1)); 
	}
	
	
	CPtrArray * parOneFrequentSet = new CPtrArray;
	TTransaction * pRecord = new TTransaction;
	//CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
	//int count = pDataAccess->GetRecordCount();
	//pDataAccess->First();
	long *tempcustID=NULL;
	for(int i=0;i<nCount && !bStopWork;i++)
	{
		if(bStopWork)
		{
			if( pCustom)
				delete []pCustom;
			if( pProduct)
				delete []pProduct;
			pCustom=NULL;
			pProduct=NULL;
			if(m_pResult)
				delete m_pResult;
			m_pResult=NULL;
			if( parOneFrequentSet)
				delete parOneFrequentSet;
			parOneFrequentSet=NULL;
			if(pRecord)
				delete pRecord;
			pRecord=NULL;
			m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
			CWinThread * pThread = AfxGetThread();
			DWORD nexit;
			::GetExitCodeThread(pThread->m_hThread,&nexit);
			AfxEndThread(nexit,true);

			return;
		}
		pRecord->lCustomerID = (long)(pCustom[i].dValue+0.5);		
		pRecord->lProductID = (long)(pProduct[pCustom[i].iIndex-1].dValue+0.5)	;
		if( IsInOneFrequentSet(parOneFrequentSet,pRecord) )
		{
		//�Ѿ����ֹ�����Ʒ
		}
		else
		{
			//�¼���
			TUnit * pUnit = new TUnit;
			long * plong = new long;
			*plong = pRecord->lProductID;
			pUnit->arIndex.Add(plong);
			pUnit->count = 1;
			parOneFrequentSet->Add(pUnit);
		}		


	}
	delete pRecord;
	pRecord = NULL;
	
//	pBar->SetRange (1,10);
//	pBar->SetStep (1);
	
	//����1��Ƶ��������Ը������
	int miniSupport = ceil((double)pMsg->nSupport * m_nCustomCount/100);//#_S 2004-11-23 $ 13 zlq �ðٷֱ���ʽ
	CPtrArray arLk;
	if( !bStopWork )
	{
		//TRACE("%d\n",parOneFrequentSet->GetSize());
		for(i=0;i<parOneFrequentSet->GetSize();i++)
		{
			TUnit * pUnit = (TUnit *)parOneFrequentSet->GetAt(i);
			if( pUnit->count < miniSupport )
			{
				parOneFrequentSet->RemoveAt(i);
				delete pUnit;
				pUnit=NULL;
				i--;
			}		
		}
		TRACE("%d\n",parOneFrequentSet->GetSize());
		SortItem(parOneFrequentSet);
		arLk.Add(parOneFrequentSet);
		WritePtrData(parOneFrequentSet,"1��Ƶ����", &bStopWork,m_pResult,true);//,m_pResultWnd);

		//����������2-n�
		bool bFlag = true;
		int k = 0;
		int mark = parOneFrequentSet->GetSize();
		while( bFlag )
		{
			//pBar->StepIt();
			if( bStopWork )
			{  
				//�����û���ֹ
				//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
				break;
			}
			CPtrArray arCk;
			CTString str;
			str.Format("��%d�����%d�,��ǰ��ĳ���Ϊ%d!",k+1,k+2,mark);
			pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);
			bool bEndFlag;
			if (k==0)
			{
				bEndFlag= Aproiri_gen12(pCustom,pProduct,nCount,(CPtrArray *)arLk.GetAt(k),k,mark,&arCk,lp,&bStopWork,miniSupport);
				if( bStopWork )
				{  
					//�����û���ֹ
					//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
					break;
				}
				if( bEndFlag )
					break;
				
			}
			else
			{
				bEndFlag= Aproiri_gen((CPtrArray *)arLk.GetAt(k),k,mark,&arCk,lp,&bStopWork);
				if( bStopWork )
				{  
					//�����û���ֹ
					//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
					break;
				}
				if( bEndFlag )
					break;
				//InXthenCount(&arCk,lp);	//����������ѭ�� modified by liytao 20030219
				InXthenCount(pCustom,pProduct,nCount,&arCk,lp);	//����������ѭ��
			}
			if( bStopWork )
			{  
				//�����û���ֹ
				break;
			}
			int nRowCk = arCk.GetSize();	
			mark = 0;
			CPtrArray * pArray = new CPtrArray;
			arLk.Add( pArray );
			for(i=0;i<nRowCk;i++)
			{
				TUnit * pUnit = (TUnit *)arCk.GetAt(i);
				if( pUnit->count >= miniSupport )
				{			
					pArray->Add(pUnit);
				}
				else
				{
					delete pUnit; 
					pUnit = NULL;
				}
			}
			mark = pArray->GetSize();
			if( mark == 0 )
			{
				bFlag = false;
			}
			k++;
			str.Format("%d��Ƶ����",k+1);
			WritePtrData(pArray,str.GetBuffer(10),&bStopWork,m_pResult,true);//,m_pResultWnd);
		}
	}

	if( !bStopWork )
	{
		//���confidence
		CTString str;
		str.Format("�������Ŷ�!");
		//lp->SetIndex(str.GetBuffer(10));
		pProcessWnd->PostMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);	

		Confidence(&arLk,pMsg->dbConfidence/100,m_pResult,&bStopWork,true);
	}

	for(i=0;i<arLk.GetSize();i++)
	{
		CPtrArray * pt = (CPtrArray *)arLk.GetAt(i);
		this->FreePtrArray(pt);
		delete pt;
	}


	if( bStopWork )
	{
		delete m_pResult;
		m_pResult = NULL;
	}

	delete pt;pt = NULL;

	m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
	CWinThread * pThread = AfxGetThread();
	DWORD nexit;
	::GetExitCodeThread(pThread->m_hThread,&nexit);
	AfxEndThread(nexit,true);
}
*/
//add end

//��ά����������
//ԭ�������������ĺ�������
//�Ż����룬��ʹ��֧�ִ�������  @_$ 2005-5-24 cx 
void CFileAssociationRule::CreateFrequentSet(CRuleTrainVO * pt,bool bString)
{
 	CRuleTrainVO * pMsg = (CRuleTrainVO*)pt;
	bool bStopWork = false;
	bool bStop = false;
	m_pResult = new CResult("��ά��������");
	TDColAry CustomArray,ProductArray;//�洢�ͻ���ʶ����Ʒ��ʶ
	int nCount=0;	
	CTString strText;	
	
	//��ȡ���ݲ�����Ԥ����
	if( pt )
	{
		for (int i=0;i<m_tVarInfo.GetSize();i++)
		{
			TFieldRole *pField = (TFieldRole *)m_tVarInfo.GetAt(i);
			if (pField->strRole == "�������")
				m_nCustomCol = pField->nFieldNo;
			else if (pField->strRole == "Ŀ�����")
				m_nProductCol = pField->nFieldNo;
		}

		grouper.reset();
		grouper.setGrpCount(2);

		//��ȡ�ͻ�����Ʒ��Ч���ݼ������н�ԭʼ����ת����double�ͱ�ʶ��������Ч��¼��
		//��Ч��¼��ǰnCount��¼�С�
		//strText.Format( "���ڻ�ȡ��Ч����..." );
		//pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&strText);
		nCount=GetValidData(m_pDataInterface,m_nCustomCol,m_nProductCol,CustomArray,ProductArray);
		
		
		if (nCount>0)
		{
 			if(!bStopWork)
			{
				//�Կͻ���ʶ����
				//prshl(CustomArray,nCount,true);
				//strText.Format( "��������..." );
				//pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&strText);
				MergeSort(CustomArray,nCount);				
			}
 			if(!bStopWork)
			{
				//ȥ���ظ�����
				//nCount=RemoveDuplicate(CustomArray,ProductArray,nCount);
				//strText.Format( "����ȥ���ظ�����..." );
				//pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&strText);
				//nCount=RemoveDuplicate_quick(CustomArray,ProductArray,nCount);
				nCount=RemoveDuplicate_quick_test(CustomArray,ProductArray,nCount);
			}
//			if(bStopWork)
//			{
//				if(m_pResult)
//					delete m_pResult;
//				m_pResult=NULL;
//				m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
//				CWinThread * pThread = AfxGetThread();
//				DWORD nexit;
//				::GetExitCodeThread(pThread->m_hThread,&nexit);
//				AfxEndThread(nexit,true);
//				return;
//			}
		}
		else
		{
			CRsltElementText *pText=new CRsltElementText("����");
			CTString str;
			str=_T("����Ч��¼���޷�����\n");
			pText->AddString(str);
			m_pResult->Add(pText);
// 			m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
//			CWinThread * pThread = AfxGetThread();
//			DWORD nexit;
//			::GetExitCodeThread(pThread->m_hThread,&nexit);
//			AfxEndThread(nexit,true);
			return ;
		}
	}

	CPtrArray * parOneFrequentSet = new CPtrArray;
	TTransaction * pRecord = new TTransaction;
	
	//
	int i;
	for(i=0;i<nCount && !bStopWork;i++)
	{
//		if(bStopWork)
//		{
//			if(m_pResult)
//				delete m_pResult;
//			m_pResult=NULL;
//			if( parOneFrequentSet)
//				delete parOneFrequentSet;
//			parOneFrequentSet=NULL;
//			if(pRecord)
//				delete pRecord;
//			pRecord=NULL;
// 			m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
//			CWinThread * pThread = AfxGetThread();
//			DWORD nexit;
//			::GetExitCodeThread(pThread->m_hThread,&nexit);
//			AfxEndThread(nexit,true);
//			return;
//		}
		pRecord->lCustomerID = (long)(CustomArray[i].dValue+0.5);		
		pRecord->lProductID = (long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5)	;
		
		if( IsInOneFrequentSet(parOneFrequentSet,pRecord) )
		{
			//�Ѿ����ֹ�����Ʒ
		}
		else
		{
			//�¼���
			TUnit * pUnit = new TUnit;
			long * plong = new long;
			*plong = pRecord->lProductID;
			pUnit->arIndex.Add(plong);
			pUnit->count = 1;
			parOneFrequentSet->Add(pUnit);
		}		
	}
	delete pRecord;
	pRecord = NULL;
	
	//����1��Ƶ��������Ը������
	//int miniSupport = ceil((double)pMsg->nSupport * m_nCustomCount/100);//#_S 2004-11-23 $ 13 zlq �ðٷֱ���ʽ
	int miniSupport = ceil((double)pMsg->nSupport * m_nCustomCount);//С����ʽ����0.7,modify by xds 20060601
	CPtrArray arLk;
//	if( !bStopWork )
//	{
		//TRACE("%d\n",parOneFrequentSet->GetSize());
		for(i=0;i<parOneFrequentSet->GetSize();i++)
		{
			TUnit * pUnit = (TUnit *)parOneFrequentSet->GetAt(i);
			if( pUnit->count < miniSupport )
			{
				parOneFrequentSet->RemoveAt(i);
				delete pUnit;
				pUnit=NULL;
				i--;
			}		
		}
		//TRACE("%d\n",parOneFrequentSet->GetSize());
		SortItem(parOneFrequentSet);
		arLk.Add(parOneFrequentSet);
		WritePtrData(parOneFrequentSet,"1��Ƶ����", &bStopWork,m_pResult,true);//,m_pResultWnd);

		//����������2-n�
		bool bFlag = true;
		int k = 0;
		int mark = parOneFrequentSet->GetSize();
		while( bFlag )
		{
//			if( bStopWork )
//			{  
//				//�����û���ֹ
//				//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
//				break;
//			}
			CPtrArray arCk;
			CTString str;
			str.Format("%d�����%d�,��ǰ�����%d...",k+1,k+2,mark);
// 			pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);
			bool bEndFlag;
			if (k==0)
			{
				bEndFlag= Aproiri_gen12(CustomArray,ProductArray,nCount,(CPtrArray *)arLk.GetAt(k),k,mark,&arCk/*,lp*/,&bStopWork,miniSupport);
				if( bStopWork )
				{  
					//�����û���ֹ
					//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
					break;
				}
				if( bEndFlag )break;
			}
			else
			{
				bEndFlag= Aproiri_gen((CPtrArray *)arLk.GetAt(k),k,mark,&arCk/*,lp*/,&bStopWork);
				if( bStopWork )
				{  
					//�����û���ֹ
					//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
					break;
				}
				if( bEndFlag )break;
				InXthenCount(CustomArray,ProductArray,nCount,&arCk);	//����������ѭ��
			}
			if( bStopWork )
			{  
				//�����û���ֹ
				break;
			}
			int nRowCk = arCk.GetSize();	
			mark = 0;
			CPtrArray * pArray = new CPtrArray;
			arLk.Add( pArray );
			for(i=0;i<nRowCk;i++)
			{
				TUnit * pUnit = (TUnit *)arCk.GetAt(i);
				if( pUnit->count >= miniSupport )
				{			
					pArray->Add(pUnit);
				}
				else
				{
					delete pUnit; 
					pUnit = NULL;
				}
			}
			mark = pArray->GetSize();
			if( mark == 0 )
			{
				bFlag = false;
			}
			k++;
			str.Format("%d��Ƶ����",k+1);
			WritePtrData(pArray,str,&bStopWork,m_pResult,true);//,m_pResultWnd);
		}
//	}

// 	if( !bStopWork )
// 	{
		//���confidence
//		CTString str;
//		str.Format("�������Ŷ�...");
//		pProcessWnd->PostMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);	

//		Confidence(&arLk,pMsg->dbConfidence/100,m_pResult,&bStopWork,true);
//	}

	for(i=0;i<arLk.GetSize();i++)
	{
		CPtrArray * pt = (CPtrArray *)arLk.GetAt(i);
		this->FreePtrArray(pt);
		delete pt;
	}


//	if( bStopWork )
//	{
//		delete m_pResult;
//		m_pResult = NULL;
//	}

	delete pt;pt = NULL;

//	m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
//	CWinThread * pThread = AfxGetThread();
//	DWORD nexit;
//	::GetExitCodeThread(pThread->m_hThread,&nexit);
//	AfxEndThread(nexit,true);
}

CResult* CFileAssociationRule::CreateRule(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt)
{
	m_pDataInterface = pDataInterface;
	m_pResult = new CResult("��ά��������");
	TDColAry CustomArray,ProductArray;//�洢�ͻ���ʶ����Ʒ��ʶ
	int nCount=0;	
	CTString strText;	
	
	//��ȡ���ݲ�����Ԥ����
	//for (int i=0;i<pVO->arFeildInfo.GetSize();i++)
/* Modified by pyx 2006-01-05
		for (int i=0;i<m_tVarInfo.GetSize();i++)
		{
			TFieldRole *pField = (TFieldRole *)m_tVarInfo.GetAt(i);
			if (pField->strRole == "�������")
				m_nCustomCol = pField->nFieldNo;
			else if (pField->strRole == "Ŀ�����")
				m_nProductCol = pField->nFieldNo;
		}*/
	m_nCustomCol = pDataInterface->IndexByName(m_strInputField.GetAt(0));
	m_nProductCol = pDataInterface->IndexByName(m_strDestField.GetAt(0));

	grouper.reset();
	grouper.setGrpCount(2);

	//��ȡ�ͻ�����Ʒ��Ч���ݼ������н�ԭʼ����ת����double�ͱ�ʶ��������Ч��¼��
	//��Ч��¼��ǰnCount��¼�С�
	nCount=GetValidData(m_pDataInterface,m_nCustomCol,m_nProductCol,CustomArray,ProductArray);
	
	
	if (nCount>0)
	{
		//�Կͻ���ʶ����
		MergeSort(CustomArray,nCount);				
		nCount=RemoveDuplicate_quick_test(CustomArray,ProductArray,nCount);
	}
	else
	{
		CRsltElementText *pText=new CRsltElementText("����");
		CTString str;
		str=_T("����Ч��¼���޷�����\n");
		pText->AddString(str);
		m_pResult->Add(pText);
		return m_pResult;
	}

	CPtrArray * parOneFrequentSet = new CPtrArray;
	TTransaction * pRecord = new TTransaction;
	
	//
	int i;
	for(i=0;i<nCount;i++)
	{
		pRecord->lCustomerID = (long)(CustomArray[i].dValue+0.5);		
		pRecord->lProductID = (long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5)	;
		
		if( IsInOneFrequentSet(parOneFrequentSet,pRecord) )
		{
			//�Ѿ����ֹ�����Ʒ
		}
		else
		{
			//�¼���
			TUnit * pUnit = new TUnit;
			long * plong = new long;
			*plong = pRecord->lProductID;
			pUnit->arIndex.Add(plong);
			pUnit->count = 1;
			parOneFrequentSet->Add(pUnit);
		}		
	}
	delete pRecord;
	pRecord = NULL;
	
	//����1��Ƶ��������Ը������
	//int miniSupport = ceil((double)m_dSupport * m_nCustomCount/100);//#_S 2004-11-23 $ 13 zlq �ðٷֱ���ʽ
	int miniSupport = ceil((double)m_dSupport * m_nCustomCount);//С����ʽ����0.7,modify by xds 20060601
	CPtrArray arLk;
	for(i=0;i<parOneFrequentSet->GetSize();i++)
	{
		TUnit * pUnit = (TUnit *)parOneFrequentSet->GetAt(i);
		if( pUnit->count < miniSupport )
		{
			parOneFrequentSet->RemoveAt(i);
			delete pUnit;
			pUnit=NULL;
			i--;
		}		
	}
	SortItem(parOneFrequentSet);
	arLk.Add(parOneFrequentSet);
	WritePtrData(parOneFrequentSet,"1��Ƶ����", &m_bStopWork,m_pResult,true);//,m_pResultWnd);

	//����������2-n�
	bool bFlag = true;
	int k = 0;
	int mark = parOneFrequentSet->GetSize();
	while( bFlag )
	{
		CPtrArray arCk;
		CTString str;
		str.Format("%d�����%d�,��ǰ�����%d...",k+1,k+2,mark);
		bool bEndFlag;
		if (k==0)
		{
			bEndFlag= Aproiri_gen12(CustomArray,ProductArray,nCount,(CPtrArray *)arLk.GetAt(k),k,mark,&arCk/*,lp*/,&m_bStopWork,miniSupport);
			if( m_bStopWork )
			{  
				break;
			}
			if( bEndFlag )break;
		}
		else
		{
			bEndFlag= Aproiri_gen((CPtrArray *)arLk.GetAt(k),k,mark,&arCk/*,lp*/,&m_bStopWork);
			if( m_bStopWork )
			{  
				//�����û���ֹ
				//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
				break;
			}
			if( bEndFlag )break;
			InXthenCount(CustomArray,ProductArray,nCount,&arCk);	//����������ѭ��
		}
		if( m_bStopWork )
		{  
			//�����û���ֹ
			break;
		}
		int nRowCk = arCk.GetSize();	
		mark = 0;
		CPtrArray * pArray = new CPtrArray;
		arLk.Add( pArray );
		for(i=0;i<nRowCk;i++)
		{
			TUnit * pUnit = (TUnit *)arCk.GetAt(i);
			if( pUnit->count >= miniSupport )
			{			
				pArray->Add(pUnit);
			}
			else
			{
				delete pUnit; 
				pUnit = NULL;
			}
		}
		mark = pArray->GetSize();
		if( mark == 0 )
		{
			bFlag = false;
		}
		k++;
		str.Format("%d��Ƶ����",k+1);
		WritePtrData(pArray,str,&m_bStopWork,m_pResult,true);//,m_pResultWnd);
	}

	if (!m_bStopWork)
	{
		//���confidence
		CTString str;
		str.Format("�������Ŷ�...");
		//Confidence(&arLk,this->m_dConfidence/100,m_pResult,&this->m_bStopWork,true);
		Confidence(&arLk,this->m_dConfidence,m_pResult,&this->m_bStopWork,true);//С����ʽ����0.7,modify by xds 20060601
	}

	for(i=0;i<arLk.GetSize();i++)
	{
		CPtrArray * pt = (CPtrArray *)arLk.GetAt(i);
		this->FreePtrArray(pt);
		delete pt;
	}

	return m_pResult;
}

CResult* CFileAssociationRule::Predict(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt)
{
	return m_pResult;
}


//�ж��ǲ�����Ƶ�����
bool CFileAssociationRule::IsInOneFrequentSet(CPtrArray *par, TTransaction *pRecord)
{
	for(int i=0;i<par->GetSize();i++)
	{
		TUnit * pUnit = (TUnit *)par->GetAt(i);
		long * plong =(long *)pUnit->arIndex.GetAt(0);
		if ( *plong == pRecord->lProductID )
		{
			pUnit->count++;
			return true;
		}
	}
	return false;
}

/*
//add by liytao 20030219
void CFileAssociationRule::InXthenCount(TDataSet* pCust,TDataSet * pProduct,int nRecCount,CPtrArray *par,void * lp,bool * pbStop )
{
	CProgressCtrl * pBar = NULL;
	if( lp )
	{
		CDlgProgress * pProgress = (CDlgProgress *)lp;
		pBar=(CProgressCtrl *)(pProgress->GetDlgItem(IDC_PROGRESS2));
		pBar->SetRange(1,100);
		pBar->SetStep(1);
		pProgress->SetIndex("����������ѭ��!");		
	}
	if( pProcessWnd )
	{
		CTString str = _T("�����������ݿ�!");
		pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);
	}
	CPtrArray arClient;
	TTransaction * pRecord = NULL;
	TTransaction * pt      = NULL;
	TUnit * pUnit          = NULL;
	int minCount=0;
	int nproductcount=0;
	int i=0,j=0,k=0,l=0;
	////////////////////////////////////////////////////////////
	CTArray<long,long> tempprod;
	long tempcustID;
	for(i=0;i<nRecCount;i++)
	{
		if( pbStop && *pbStop == true)
		{
			//�û���ֹ����
			FreePtrArray(&arClient);
			return;
		}

		pRecord = new TTransaction;


		pRecord->lCustomerID = (long)(pCust[i].dValue+0.5);		
		pRecord->lProductID = (long)(pProduct[pCust[i].iIndex-1].dValue+0.5);	
		

		if (tempprod.GetSize()==0)
		{
			tempcustID=pRecord->lCustomerID;
			tempprod.Add(pRecord->lProductID);

		}
		else
		{
			if( pRecord->lCustomerID==tempcustID)
				tempprod.Add(pRecord->lProductID);
			else
			{
				if( lp )
				{
					pBar->StepIt();
					if( pBar->GetPos() == 100 )
					{
						pBar->SetRange(1,100);
						pBar->SetStep(1);
					}
				}
				//��ʼ�����µĿͻ������ۼ�¼,������һ���ͻ�������
				CountItem(&tempprod,par,pbStop);
				//���arClient	
				tempprod.RemoveAll();
				//�����µĿͻ�����
				tempprod.Add(pRecord->lProductID);
				tempcustID=pRecord->lCustomerID;


			}


		}
		//	if(pDataAccess->GetRecNo()==count)
		if( i==nRecCount-1)
		{
			CountItem(&tempprod,par,pbStop);
			tempprod.RemoveAll();
			break;
		}
		//pDataAccess->Next();
	}
}
//add end
*/

//InXthenCount��֧�ִ����ݰ汾
//�Խ���Ƶ�������Ʒ����
void CFileAssociationRule::InXthenCount(TDColAry &CustomArray,TDColAry &ProductArray,int nRecCount,CPtrArray *par/*,void * lp*/,bool * pbStop)
{
// 	CProgressCtrl * pBar = NULL;
/*
	if( lp )
	{
		CDlgProgress * pProgress = (CDlgProgress *)lp;
		pBar=(CProgressCtrl *)(pProgress->GetDlgItem(IDC_PROGRESS2));
		pBar->SetRange(1,100);
		pBar->SetStep(1);
		pProgress->SetIndex("����������ѭ��!");		
	}
*/
/*
	if( pProcessWnd )
	{
		CTString str = _T("�����������ݿ�!");
		pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);
	}
*/
	CPtrArray arClient;
	TTransaction * pRecord = NULL;
	TTransaction * pt      = NULL;
	TUnit * pUnit          = NULL;
	int minCount=0;
	int nproductcount=0;
	int i=0,j=0,k=0,l=0;
	////////////////////////////////////////////////////////////
	CTArray<long,long> tempprod;
	long tempcustID;
	for(i=0;i<nRecCount;i++)
	{
		if( pbStop && *pbStop == true)
		{
			//�û���ֹ����
			FreePtrArray(&arClient);
			return;
		}
		
		pRecord = new TTransaction;
		
		
		//pRecord->lCustomerID = (long)(pCust[i].dValue+0.5);		
		//pRecord->lProductID = (long)(pProduct[pCust[i].iIndex-1].dValue+0.5);	
		pRecord->lCustomerID = (long)(CustomArray[i].dValue+0.5);		
		pRecord->lProductID = (long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5);			
		
		if (tempprod.GetSize()==0)
		{
			tempcustID=pRecord->lCustomerID;
			tempprod.Add(pRecord->lProductID);
			
		}
		else
		{
			if( pRecord->lCustomerID==tempcustID)
				tempprod.Add(pRecord->lProductID);
			else
			{
/*
				if( lp )
				{
					pBar->StepIt();
					if( pBar->GetPos() == 100 )
					{
						pBar->SetRange(1,100);
						pBar->SetStep(1);
					}
				}
*/
				//��ʼ�����µĿͻ������ۼ�¼,������һ���ͻ�������
				CountItem(&tempprod,par,pbStop);
				//���arClient	
				tempprod.RemoveAll();
				//�����µĿͻ�����
				tempprod.Add(pRecord->lProductID);
				tempcustID=pRecord->lCustomerID;
				
				
			}
			
			
		}
		//	if(pDataAccess->GetRecNo()==count)
		if( i==nRecCount-1)
		{
			CountItem(&tempprod,par,pbStop);
			tempprod.RemoveAll();
			break;
		}
		//pDataAccess->Next();
	}
}


/*
void CFileAssociationRule::InXthenCount(CPtrArray *par,void * lp,bool * pbStop )
{
	CProgressCtrl * pBar = NULL;
	if( lp )
	{
		CDlgProgress * pProgress = (CDlgProgress *)lp;
		pBar=(CProgressCtrl *)(pProgress->GetDlgItem(IDC_PROGRESS2));
		pBar->SetRange(1,100);
		pBar->SetStep(1);
		pProgress->SetIndex("����������ѭ��!");		
	}
	if( pProcessWnd )
	{
		CTString str = _T("�����������ݿ�!");
		pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);
	}
	CPtrArray arClient;
	TTransaction * pRecord = NULL;
	TTransaction * pt      = NULL;
	TUnit * pUnit          = NULL;
	int minCount=0;
	int nproductcount=0;
	int i=0,j=0,k=0,l=0;
	////////////////////////////////////////////////////////////
	CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
	int count = pDataAccess->GetRecordCount();
	pDataAccess->First();
	for(i=0;i<count;i++)
	{
		if( pbStop && *pbStop == true)
		{
			//�û���ֹ����
			FreePtrArray(&arClient);
			return;
		}

		pRecord = new TTransaction;

		CField * pField1=pDataAccess->m_pFieldList->FieldByIndex(m_nCustomCol);
		CField * pField2=pDataAccess->m_pFieldList->FieldByIndex(m_nProductCol);

		if( pField1->IsNull() || pField2->IsNull() )
		{
			pDataAccess->Next();
			continue;
		}

		pRecord->lCustomerID = (long)pField1->GetAsInteger();		
		pRecord->lProductID  = (long)pField2->GetAsInteger();
		
		if( arClient.GetSize() == 0 )                    
		{
			arClient.Add(pRecord);
		}
		else
		{
			pt = (TTransaction *)arClient.GetAt(0);
			if( pRecord->lCustomerID == pt->lCustomerID )
			{
				//����ͬһ���ͻ������ۼ�¼
				arClient.Add(pRecord);				
			}
			else
			{
				if( lp )
				{
					pBar->StepIt();
					if( pBar->GetPos() == 100 )
					{
						pBar->SetRange(1,100);
						pBar->SetStep(1);
					}
				}
				//��ʼ�����µĿͻ������ۼ�¼,������һ���ͻ�������
				CountItem(&arClient,par,pbStop);
				//���arClient	
				FreePtrArray(&arClient);
				//�����µĿͻ�����
				arClient.Add(pRecord);
			}				
		}
		if(pDataAccess->GetRecNo()==count)
		{
			CountItem(&arClient,par,pbStop);
			FreePtrArray(&arClient);
			break;
		}
		pDataAccess->Next();
	}
}
*/

//ѡ������
//������ĺ���
void CFileAssociationRule::SortItem(CPtrArray *par)
{
	CPtrArray ar;
	int i;
	for(i=0;i<par->GetSize();i++)
	{
		ar.Add( par->GetAt(i) );
	}
	par->RemoveAll();
	while( ar.GetSize() != 0 )
	{
		int min = 0;
		int minIndex = 0;
		for(i=0;i<ar.GetSize();i++)
		{
			TUnit * pUnit = (TUnit *)ar.GetAt(i);
			int temp = *(int *)pUnit->arIndex.GetAt(0);
			if( i == 0 )
			{
				min = temp;			
			}
			if( temp < min )
			{
				min = temp;
				minIndex = i;
			}
		}
		par->Add( ar.GetAt(minIndex) );
		ar.RemoveAt(minIndex);
	}
}

/*
void CFileAssociationRule::StartCalculate()
{
	
}
*/


//add by liytao 20030220
void CFileAssociationRule::CountItem(CTArray<long,long> *parClient, long* pValArray,int * pFreqArray,int nRowNum,bool *pStop)
{

	TUnit * pUnit = NULL;
	int nproductcount = 0;
	int minCount = 0;
	int count=0;
	int i;
	int nParClientSize=parClient->GetSize();
	//��ʼ�����µĿͻ������ۼ�¼,������һ���ͻ�������
	//pUnit = (TUnit *)par->GetAt(0);
	i=0;

	if( nParClientSize < 2 )
	{
		return;				//����ͻ������ۼ�¼С�����Ŀ�Ϳ��Է���
	}
	for (  i=0;i<nParClientSize-1;i++)
	{
		if( pStop!=NULL)
		{
			if( *pStop)
				break;
		}
		long Vali=parClient->GetAt(i);
		
		int nIndexi=FindLongVal(Vali,pValArray,nRowNum);
		if (nIndexi<0)
			continue;
		for (int j=i+1;j<nParClientSize;j++)
		{
			if( pStop!=NULL)
			{
				if( *pStop)
					break;
			}
			long Valj=parClient->GetAt(j);
			int nIndexj=FindLongVal(Valj,pValArray,nRowNum);
			if (nIndexj>=0)
			{
				if (nIndexi>nIndexj)
					pFreqArray[nIndexj*nRowNum+nIndexi]=pFreqArray[nIndexj*nRowNum+nIndexi]+1;
				else
					pFreqArray[nIndexi*nRowNum+nIndexj]=pFreqArray[nIndexi*nRowNum+nIndexj]+1;


			}
		}

	}

}
//add end

//��ÿһ���ͻ������ۼ�¼����ͳ��
void CFileAssociationRule::CountItem(CTArray<long,long> *parClient, CPtrArray *par,bool *pStop)
{
	TUnit * pUnit = NULL;
	int nproductcount = 0;
	int minCount = 0;
	int count=0;
	int i,j,k;
	int nParClientSize=parClient->GetSize();
	//��ʼ�����µĿͻ������ۼ�¼,������һ���ͻ�������
	pUnit = (TUnit *)par->GetAt(0);
	nproductcount = pUnit->arIndex.GetSize();	//���õ�Ԫ����Ʒ�ĸ���
	int nParSize=par->GetSize();
	i=0;
	if( nParClientSize < nproductcount )
	{
		return;				//����ͻ������ۼ�¼С�����Ŀ�Ϳ��Է���
	}
	while (i<nParSize)
	//for(i=0;i<par->GetSize();i++)//ѭ��ÿ����е���Ʒ����
	{

		if (pStop)
		{
			if (*pStop)
				break;

		}


		pUnit = (TUnit *)par->GetAt(i);				//�õ���е�i����Ԫ�ĵ�����

		minCount = 100;
		bool bStop=false;
		int nFoundCount=0;
		int nColIndex=-1;
		for(k=0;!bStop&&k<nproductcount;k++)
		{
			long * plong = (long *)pUnit->arIndex.GetAt(k);
			bool bFound=false;
			for(j=0;!bFound&&j<nParClientSize;j++)
			{
				long tempproduct = parClient->GetAt(j);	//�õ��ͻ����ۼ�¼�еĵ�j����¼
				if( *plong == tempproduct )
				{
					// һ��ķ���,���ʱӦ��ȥ��break
					bFound=true;
				}
			}
			if (!bFound)
			{
				nColIndex=k;
				bStop=true;

			}
			else
			{

				nFoundCount++;
			}
		}
		if (nFoundCount==nproductcount)
		{
			pUnit->count ++;	
			i++;
		}
		else
		{
			if( nColIndex>=0)
			{
				int mm=i+1;
				bStop=false;
				while (mm<nParSize&&!bStop)
				{
					TUnit *pTemp=(TUnit*)par->GetAt(mm);
					bool bDif=false;
					for (int m=0;!bDif&&m<=nColIndex;m++)
					{
						if (*(long*)pTemp->arIndex.GetAt(m)!=*(long*)pUnit->arIndex.GetAt(m))
							bDif=true;
					}
					if (!bDif)
						mm++;
					else
						bStop=true;
				}
				i=mm;
			}
			else
				i++;
		}
	}
}
//add end

//��ÿһ���ͻ������ۼ�¼����ͳ��
/*
void CFileAssociationRule::CountItem(CPtrArray *parClient, CPtrArray *par,bool *pStop)
{
	TUnit * pUnit = NULL;
	int nproductcount = 0;
	int minCount = 0;
	int count=0;
	int i,j,k;
	//��ʼ�����µĿͻ������ۼ�¼,������һ���ͻ�������
	pUnit = (TUnit *)par->GetAt(0);
	nproductcount = pUnit->arIndex.GetSize();	//���õ�Ԫ����Ʒ�ĸ���
	for(i=0;i<par->GetSize();i++)//ѭ��ÿ����е���Ʒ����
	{
		if (pStop)
		{
			if (*pStop)
				break;

		}

		pUnit = (TUnit *)par->GetAt(i);				//�õ���е�i����Ԫ�ĵ�����
		if( parClient->GetSize() < nproductcount )
		{
			return;				//����ͻ������ۼ�¼С�����Ŀ�Ϳ��Է���
		}
		minCount = 100;
		for(k=0;k<nproductcount;k++)
		{
			if (pStop)
			{
				if (*pStop)
					break;

			}

			long * plong = (long *)pUnit->arIndex.GetAt(k);
			count = 0;
			for(j=0;j<parClient->GetSize();j++)
			{
				TTransaction * pt = (TTransaction *)parClient->GetAt(j);	//�õ��ͻ����ۼ�¼�еĵ�j����¼
				if( *plong == pt->lProductID )
				{
					count++;
					// һ��ķ���,���ʱӦ��ȥ��break
					break;
				}
			}
			if( count < minCount )
			{
				minCount = count;				
			}
			if( count == 0 )	//����������һ����Ʒû�г��֣��Ͳ����ٱȽ���
			{
				minCount = 0;
				break;
			}
		}
		pUnit->count += minCount;								
	}
}
*/


int CFileAssociationRule::FindLongVal(long lVal,long * pVal,int nCount)
{
	for (int i=0;i<nCount;i++)
		if( lVal==pVal[i])
			return i;
	return -1;


}

/*void CFileAssociationRule::WritePtrData(CPtrArray *par, char *pszTitle,bool *pStop,CResult * pResult){
	//)
	CSingleRuleReduction::WritePtrData(par,pszTitle,pStop,pResult);
}*/

//add by liytao 20030821
void CFileAssociationRule::WritePtrData(CPtrArray *par, char *pszTitle,bool *pStop,CResult * pResult,bool bString)
{	
	int rowNum = par->GetSize();
	if( rowNum <= 0 )
		return;

	CTLTable * pTable = new CTLTable;
//	assert(pWnd->m_hWnd);
	pTable->SetTitle(pszTitle);
	//pTable->SetRows(1+rowNum);
	//pTable->SetCols(2);
	pTable->CreateTable(1+rowNum,2);
	TUnit * pUnit = (TUnit *)par->GetAt(0);
	int nWidth = 100 * pUnit->arIndex.GetSize();
	pTable->InsertColumn(0,"Ƶ���");
	pTable->InsertColumn(1,"Ƶ    ��");
	CRsltElementTable * pE = new CRsltElementTable(pszTitle,pTable);
	assert(pResult);

	pResult->Add(pE);	
	CTString str;
	for(int i=0;i<par->GetSize();i++)
	{
		if( pStop)
		{
			if( *pStop)
				break;

		}
		CTString strResult;
		TUnit * pUnit = (TUnit *)par->GetAt(i);	
		for(int j=0;j<pUnit->arIndex.GetSize();j++)
		{
			if( pStop)
			{
				if( *pStop)
					break;

			}


			int * pInt = (int *)pUnit->arIndex.GetAt(j);
			TDataSet *ptempdata;
			int dataType,grpindex=grouper.getGrpIndex(*pInt);
			dataType=grouper.getGrpDataType(grpindex);
			ptempdata=grouper.getVal(grouper.getIDIndex(*pInt));
	
//***************************added by zkc 2004.5.17 zlq �޸�  ********************************								
			CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(this->m_nProductCol);
			CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq
			str = strNew;

			str+="  ";

//*********end of addition by zkc 2004.5.17********


			strResult += str;
		}
		pTable->InsertItem(i,strResult);
		str.Format("%12d",pUnit->count);
		strResult += str;
		pTable->SetItemText(i,1,str);		
	}
}

//add end

//add by liytao 20030822
/*
void CFileAssociationRule::Confidence(CPtrArray *par,double mini_confidence,CResult * pResult,bool * pbStop){
	CSingleRuleReduction::Confidence(par,mini_confidence,pResult,pbStop);
}*/


void CFileAssociationRule::Confidence(CPtrArray *par,double mini_confidence,CResult * pResult,bool * pbStop,bool bString)
{	
	CRsltElementText * pText= new CRsltElementText("ǿ����");
	assert(pResult);
	pResult->Add(pText);
	//CTString strResult;

	CTString strResultTemp;//��ʱ����� //#_S 2004-11-25 zlq
	CTStringArray strArray;//��ʱ�����

	CTString str;
	strResultTemp += _T("ǿ����\n\n");
	for(int i=1;i<par->GetSize();i++)			//��k-Ƶ���,��iѭ��;
	{
		if( pbStop && *pbStop == true )
		{
			strArray.Add(strResultTemp);
			//�û���ֹ�˼���
			pText->AddString(GetString(strArray));
			return;
		}
		CPtrArray * pkar = (CPtrArray *)par->GetAt(i);
		for(int j=0;j<pkar->GetSize() && !(*pbStop);j++)		//��k-Ƶ����еĵ�Ԫ��Ŀ,��jѭ��;
		{
			TUnit * pUnit = (TUnit *)pkar->GetAt(j);
			int nLen = pUnit->arIndex.GetSize();
			assert( i+1 == nLen );				//i����k-Ƶ����еĵ�Ԫ�ĳ���;
			CIntVector vect(nLen);
			int k;
			for(k=0;k<nLen;k++)				//��k-Ƶ����еĵ�j��Ԫ�����ݸ���vect;
			{
				vect[k] = *(int *)pUnit->arIndex.GetAt(k);
			}
			for(k=1;k<nLen && !(*pbStop);k++)					//��ȡk-Ƶ����еĵ�j��Ԫ�ĵ�s(s=k)���;
			{
				CIntMatrix matrix_subset = nchoosek(vect,k);
				for(int l=0;l< matrix_subset.mrow() && !(*pbStop);l++)//�Ե�s���Ԫ��ѭ��,��ȡ���Ŷ�;
				{
					CIntVector vect_subset = matrix_subset.row(l);
					int    count = GetSubSetCount((CPtrArray *)par->GetAt(k-1),vect_subset);
					double confidence = pUnit->count / (count - 0.0);
					//if( confidence > mini_confidence ) modified by liytao 20030225
					if( confidence >= mini_confidence )
					{
						int m;
						for(m = 0;m<vect_subset.vlen();m++)
						{
							int nt = vect_subset[m];
							TDataSet *ptempdata;
							int dataType,grpindex=grouper.getGrpIndex(nt);
							dataType=grouper.getGrpDataType(grpindex);
							ptempdata=grouper.getVal(grouper.getIDIndex(nt));
//***************************added by zkc 2004.5.17 zlq �޸�  ********************************								
							CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(this->m_nProductCol);
							CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq
							str = strNew;

							
///*************************end of addition zkc 2004.5.17*********************
							
							//strResult += str; ��������ȡ��
							AddString(strArray,strResultTemp,str);

							if( m != vect_subset.vlen()-1 )
							{			
								//strResult += _T("�� ");
								AddString(strArray,strResultTemp,_T("�� "));
							}
						}
						//strResult += _T("==>>");
						AddString(strArray,strResultTemp,_T("==>>"));
						
						CIntVector vecttemp = VectorSub(vect,vect_subset);
						for(m = 0;m<vecttemp.vlen();m++)
						{
							int nt = vecttemp[m];
							TDataSet *ptempdata;
							int dataType,grpindex=grouper.getGrpIndex(nt);
							dataType=grouper.getGrpDataType(grpindex);
							ptempdata=grouper.getVal(grouper.getIDIndex(nt));

//***************************added by zkc 2004.5.17 zlq �޸�  ********************************								
							CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(this->m_nProductCol);
							CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq
							str = strNew;							
///*************************end of addition zkc 2004.5.17*********************
							//strResult += str;
							AddString(strArray,strResultTemp,str);
							if( m != vecttemp.vlen()-1 )
							{			
								//strResult += _T("�� ");
								AddString(strArray,strResultTemp,_T("�� "));
							}
						}						
						str.Format("      ֧�ֶ� = %5.2f%%,    ���Ŷ� = %5.2f%%\n",(double)pUnit->count/m_nCustomCount*100,confidence*100);	
						//strResult += str;
						AddString(strArray,strResultTemp,str);
					}
				}				
			}
		}
	}

	strArray.Add(strResultTemp);
	CTString strResult = GetString(strArray);

	if( strResult.GetLength() < 10 )
	{
		strResult += _T("���μ���û���ҵ�����Ҫ���ǿ����!���������ò�������!");
	}
	pText->AddString(strResult);
}

/*
int compareStr( const void *arg1, const void *arg2 )
{
   // Compare all of both strings: 
   return _stricmp( * ( char** ) (*(int*) arg1), * ( char** ) (*(int*) arg2) );
}
int compareDouble( const void *arg1, const void *arg2 )
{
   return  (* ( double* )(*(int*) arg1)-* ( double* )(*(int*) arg2));
}
//add end
*/

/*
*���ݹ˿��б��õ��˿͵ĸ������ǳ���ͬ�Ĺ˿ͣ�
*pCustom:�˿�����
*nCount���˿�����Ĵ�С
*nCustCol  :��pDataInterface��Ӧ���кţ���Ϊ�õ������ã�
*pDataInterface:��Ϊ�õ������ã�
*���ز�ͬ�˿͵ĸ���
*/
/*int CFileAssociationRule::GetDifferCount(TDataSet *pCustom, int nCount, int nCustCol, CDataInterface *pDataInterface)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	int nRowIndex=1,nCustFieldType;//,nProductFieldType;
	
	nCustFieldType=pAccess->m_pFieldList->FieldByIndex(nCustCol)->GetFieldType();
	int nDifferCount =1;
	int *pBuf = new int[nCount];//������ݵ�ָ��ֵ
	if(nCustFieldType==fString)
	{
		for (int i=0;i<nCount;i++)
		{
			pBuf[i] = (int)&pCustom[i].strVal;
		}		
		qsort( (void *)pBuf, nCount, sizeof( int * ), compareStr );

		CTString *pCurStr ;
		pCurStr = (CTString*)pBuf[0] ;
		for (i=1;i<nCount;i++)
		{			
			if (*pCurStr != (*(CTString*)pBuf[i]) )
			{
				pCurStr = (CTString*)pBuf[i];
				nDifferCount++;
			}

		}
		
	}
	else
	{
		for (int i=0;i<nCount;i++)
		{
			pBuf[i] = (int)&pCustom[i].dValue;
			//TRACE("********   %f\r\n",*(double*)pBuf[i]);
		}
		qsort( (void *)pBuf, nCount, sizeof( int * ), compareDouble );
		double *pCurStr ;
		pCurStr = (double*)pBuf[0] ;
		for (i=1;i<nCount;i++)
		{		
			//TRACE("********2   %f\r\n",*pCurStr);
			if (*pCurStr != (*(double*)pBuf[i]) )
			{
				pCurStr = (double*)pBuf[i];
				
				nDifferCount++;
			}

		}
	}		

	delete []pBuf;
	return nDifferCount;
}
*/
















































