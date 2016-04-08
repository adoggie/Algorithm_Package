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
//将strIn字符串加入strCur，最终加入strArray //#_S 2004-11-25 zlq
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

//将CTStringArray的值连接成CTString//#_S 2004-11-25 zlq
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
void CFileAssociationRule::prshl(TDataSet *p, int n,bool reSort)   //实数的shell（希尔）排序
{
    int k=0,j=0,i=0;
    TDataSet t;
    if (reSort)  //如果标志位表明没有排序才进行
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
//////////prshl排序大数据版本
//此算法太慢，最好用针对大数据的排序算法
void CFileAssociationRule::prshl(TDColAry &ColAry, int n,bool reSort)   //实数的shell（希尔）排序
{
    int k=0,j=0,i=0;
    TDataCol tt;
    if (reSort)  //如果标志位表明没有排序才进行
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

//用于归并排序中的qsort函数
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

//大数据排序和小数据不同，需要分页排序，减少硬盘访问抖动
//归并排序，升序
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
		
		//归并
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
			pOut->SetSize(nSizeA+nSizeB);//设置输出大小
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
		nCurPos = nEndPos;//更新当前位置
	}
	
	//复制数据 //#_S 2005-5-17 zlq
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
///////////////RemoveDuplicate的大数据版本
int CFileAssociationRule::RemoveDuplicate(TDColAry &CustomArray,TDColAry &ProductArray,int count)
{
	int nLen=count;
	long tempcustID=CustomArray[count-1].dValue+0.5;	//初始化为客户标识集最后一个数据
	CTArray<long,long> tempProdSet;	//保存商品标识，如果不同商品数量不太多，则可以不需要大数据量机制
	
	tempProdSet.Add((long)(
		ProductArray[CustomArray[count-1].iIndex-1].dValue+0.5)); //加入客户标识相应的商品标识 
	int nsetsize=0,j,k;
	for ( int i=count-2;i>=0;i--)
	{
		//如果客户标识相同则比较商品标识（客户标识已经排好序）
		//		如果商品标识在临时存储序列中，则表明有重复记录，将后面的记录前移，覆盖此记录
		//		否则将此商品标识增加进临时存储序列
		//否则开辟新的临时存储序列，并记录新的客户标识
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
	///此处可压缩存储空间//ask zlq
	return nLen;
}
*/


/*
//大数据高效版本。@_$ 2005-5-19 cx
//在原来代码基础上修改，原来采用由后往前搜索，发现重复则将后面的记录前移覆盖重复的记录
//现在将重复记录的索引域做个标记，最后再将未标记的记录复制一边
//以后考虑重写：首先需在排序部分将商品字段也排序，然后再从前往后搜索并坐标记，最后重新复制
int CFileAssociationRule::RemoveDuplicate_quick(TDColAry &CustomArray,TDColAry &ProductArray,int count)
{
	int nLen=count;
	long tempcustID=CustomArray[count-1].dValue+0.5;	//初始化为客户标识集最后一个数据
	CTArray<long,long> tempProdSet;	//保存商品标识，如果不同商品数量不太多，则可以不需要大数据量机制
	
	tempProdSet.Add((long)(
		ProductArray[CustomArray[count-1].iIndex-1].dValue+0.5)); //加入客户标识相应的商品标识 
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
		//如果客户标识相同则比较商品标识（客户标识已经排好序）
		//		如果商品标识在临时存储序列中，则表明有重复记录，对此记录做标记
		//		否则将此商品标识增加进临时存储序列
		//否则开辟新的临时存储序列，并记录新的客户标识
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
	long tempcustID=CustomArray[count-1].dValue+0.5;	//初始化为客户标识集最后一个数据
	long tempprodID;
	CTArray<long,long> tempProdSet;	//保存商品标识，如果不同商品数量不太多，则可以不需要大数据量机制
	tempProdSet.Add((long)(
		ProductArray[CustomArray[count-1].iIndex-1].dValue+0.5)); //加入客户标识相应的商品标识 
	int nsetsize=0,j;
	
	//复制CustomArray到临时变量tempColAry	
	TDColAry tempColAry;
	tempColAry.SetSize(count);
	for(j=0;j<count;j++)
	{
		tempColAry[j]=CustomArray[j];
	}
	
	//去除重复记录
	int i;
	for (i=count-2;i>=0;i--)
	{
		
		//现象：处理速度先慢后快？？？？
		//////////////////////////////////////////////////////////////////////////
		//strText.Format( "正在去除重复记录，待处理数：%d", i+1 );
		//pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&strText);
		//////////////////////////////////////////////////////////////////////////
		
		
		//如果客户标识相同则比较商品标识（客户标识已经排好序）
		//		如果商品标识在临时存储序列中，则表明有重复记录，对此记录做标记
		//		否则将此商品标识增加进临时存储序列
		//否则开辟新的临时存储序列，并记录新的客户标识
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
	
	//返回无重复记录集长度
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
	//#_S 2004-11-25 zlq   保存数据值
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
//精简代码，源代码见上面被注释掉的函数 -cx  2005-5
int CFileAssociationRule::GetValidData(CDataInterface *pDataInterface,int nCustCol,
		   int nProductCol,TDataSet *&pCustArray,TDataSet *&pProductArray,bool bString)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	int nCount=0;								//有效记录数 
	long nRecord=pAccess->GetRecordCount();		//总记录数
	if (nRecord<=0)return 0;
	pCustArray=new TDataSet[nRecord];			//客户集
	pProductArray=new TDataSet[nRecord];		//商品集
	
	pAccess->First();
	int nRowIndex=1,nCustFieldType,nProductFieldType;
	CField* pCustField,*pProductField;
	
	m_nCustomCount=0;
	while(!pAccess->Eof())
	{
		/////////////////？？应该提到循环的外面
		pCustField=pAccess->m_pFieldList->FieldByIndex(nCustCol);	//得到数据库中客户域指针
		nCustFieldType=pCustField->GetFieldType();					//得到数据库中客户域类型
		/////////////////

		if( pDataInterface->IsMissingValue( pCustField, true))//(pField->IsNull()))
		{
			nRowIndex++;
			pAccess->Next();
			continue;
		}
		
		//////////？？应该提到循环的外面
		pProductField=pAccess->m_pFieldList->FieldByIndex(nProductCol);//得到数据库中商品域指针
		nProductFieldType=pProductField->GetFieldType();			//得到数据库中商品域类型
		/////////////////////

		if(!pDataInterface->IsMissingValue( pProductField, true))//(pField->IsNull()))
		{
			TDataSet tempdata;
			int idindex;
			
			if(nCustFieldType==fString)
			{
				tempdata.strVal=pCustField->SetToDisplay();		//得到客户字段当前扫描到的字符串
			}
			else
			{
				tempdata.dValue=pCustField->GetAsDouble();		//得到客户字段当前扫描到的双精度数
			}			
			//add into group;
			//grouper存储不同的客户ID和商品ID，以及相应的数字标识，
			//因此存储量不会太大，可以暂时不考虑大数据量问题
			idindex=grouper.getIDIndex(0,tempdata);		//得到当前扫描数据的索引(int)
			if (idindex<0){
				grouper.add(0,nCustFieldType,tempdata); //如果组中不存在此数据，就增加，
				idindex=grouper.getIDIndex(0,tempdata); //再得到索引				
				m_nCustomCount++;		//以后可能可以用GroupingInfo得到不同的客户数目
			}
			pCustArray[nCount].dValue=grouper.getID(idindex);  //得到索引值对应的数值

			if(nProductFieldType==fString)
				tempdata.strVal=pProductField->SetToDisplay();	//得到商品字段当前扫描到的字符串
			else
			{
				tempdata.dValue=pProductField->GetAsDouble();	//得到商品字段当前扫描到的双精度数
			}			
			//add into group;
			idindex=grouper.getIDIndex(1,tempdata);
			if (idindex<0){
				grouper.add(1,nProductFieldType,tempdata);
				idindex=grouper.getIDIndex(1,tempdata);				
			}
			pProductArray[nCount].dValue=grouper.getID(idindex);//得到索引值对应的数值
																//用一个grouper处理客户数据和商品数据
			
//@_$ 2005-5-10 cx 重大bug修改!!!
//此bug会造成输入数据有缺失值时计算结果不正确。
//nRowIndex为包含缺失值的总记录序数，从其它程序调用iIndex情况分析此处应该等于有效记录序数
			//pCustArray[nCount].iIndex=nRowIndex;	
			//pProductArray[nCount].iIndex=nRowIndex;		
			pCustArray[nCount].iIndex=nCount+1;		//客户数组的索引值为当前有效记录行序数
			pProductArray[nCount].iIndex=nCount+1;	//商品数组的索引值为当前有效记录行序数
//以后需要对pCustArray排序，因此需要记录下起原始序数，以使其与pProductArray序数对应
//测试成熟之后可以把nRowIndex变量删去
/////////////////////////////////////////////////

			nCount++;  //有效记录数加1
		}
		
		nRowIndex++;	//总记录索引加1
		pAccess->Next();	
	}
	
///此处可以精简pCustArray和pProductArray内存，使其大小等于有效记录数
///注意不要和RemoveDuplicate函数重复精简（待看）
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
//重点替换pCustArray和pProductArray的数据类型，使其支持大数据量,替换前的程序见紧接上面的函数
//并进一步优化代码
//此处pCustArray和pProductArray保存整形数据就可以了
//TDColAry中的TDataCol如果保存数据为整形，则在大数据量时候可节省很多空间
//grouper中的数据可能没有排序，在处理大数据量的时候，每次都要顺序遍历其中数据，速度会比较慢
//*********************************************
//   nCustCol：客户数据所在列
//   nProductCol：商品数据所在列
//   CustomArray：客户数据
//	 ProductArray：商品数据
//功能：获得客户字段和商品字段的有效记录数据，用索引值来替代原来的数据(数值或者字符串)，
//		索引与原来数据的对应关系被保存，将索引值保存在CustomArray和ProductArray中，返回有效记录数
int CFileAssociationRule::GetValidData(CDataInterface *pDataInterface,int nCustCol,
		   int nProductCol,TDColAry &CustomArray,TDColAry &ProductArray)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	long nRecord=pAccess->GetRecordCount();		//总记录数
	if (nRecord<=0)return 0;
	
	CField* pCustField,*pProductField;
	int nCustFieldType,nProductFieldType;
	pCustField=pAccess->m_pFieldList->FieldByIndex(nCustCol);	//得到数据库中客户域指针
	nCustFieldType=pCustField->GetFieldType();					//得到数据库中客户域类型
	pProductField=pAccess->m_pFieldList->FieldByIndex(nProductCol);//得到数据库中商品域指针
	nProductFieldType=pProductField->GetFieldType();			//得到数据库中商品域类型

//	pCustArray=new TDataSet[nRecord];			//客户集
//	pProductArray=new TDataSet[nRecord];		//商品集
	CustomArray.SetSize(nRecord);			//客户集
	ProductArray.SetSize(nRecord);		//商品集
	
	pAccess->First();
	int nCount=0;						//有效记录数 
	m_nCustomCount=0;					//不同客户数目
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
				tempdata.strVal=pCustField->GetAsString();		//得到客户字段当前扫描到的字符串
			}
			else
			{
				tempdata.dValue=pCustField->GetAsDouble();		//得到客户字段当前扫描到的双精度数
			}			
			//add into group;
			//grouper存储不同的客户ID和商品ID，以及相应的数字标识，
			//因此存储量不会太大，可以暂时不考虑大数据量问题
			idindex=grouper.getIDIndex(0,tempdata);		//得到当前扫描数据的索引(int)
			if (idindex<0){
				grouper.add(0,nCustFieldType,tempdata); //如果组中不存在此数据，就增加，
				idindex=grouper.getIDIndex(0,tempdata); //再得到索引				
				m_nCustomCount++;		//以后可能可以用GroupingInfo得到不同的客户数目
			}
			//pCustArray[nCount].dValue=grouper.getID(idindex);  //得到索引值对应的数值
			//pCustArray[nCount].iIndex=nCount+1;		//客户数组的索引值为当前有效记录行序数
			temp1.dValue=grouper.getID(idindex);	//得到索引值对应的数值
			temp1.iIndex=nCount+1;					//客户数组的索引值为当前有效记录行序数
			CustomArray.SetAt(nCount,temp1);

			if(nProductFieldType==fString)
			{
				tempdata.strVal=pProductField->GetAsString();	//得到商品字段当前扫描到的字符串
			}
			else
			{
				tempdata.dValue=pProductField->GetAsDouble();	//得到商品字段当前扫描到的双精度数
			}			
			//add into group; //用一个grouper处理客户数据和商品数据
			idindex=grouper.getIDIndex(1,tempdata);
			if (idindex<0){
				grouper.add(1,nProductFieldType,tempdata);
				idindex=grouper.getIDIndex(1,tempdata);				
			}
			//pProductArray[nCount].dValue=grouper.getID(idindex);//得到索引值对应的数值
			//pProductArray[nCount].iIndex=nCount+1;	//商品数组的索引值为当前有效记录行序数
			temp1.dValue=grouper.getID(idindex);	
			temp1.iIndex=nCount+1;
			ProductArray.SetAt(nCount,temp1);
	
//@_$ 2005-5-10 cx 重大bug修改!!!
//此bug会造成输入数据有缺失值时计算结果不正确。
//nRowIndex为包含缺失值的总记录序数，从其它程序调用iIndex情况分析此处应该等于有效记录序数
			//pCustArray[nCount].iIndex=nRowIndex;	
			//pProductArray[nCount].iIndex=nRowIndex;		
//			pCustArray[nCount].iIndex=nCount+1;		//客户数组的索引值为当前有效记录行序数
//			pProductArray[nCount].iIndex=nCount+1;	//商品数组的索引值为当前有效记录行序数
//以后需要对pCustArray排序，因此需要记录下起原始序数，以使其与pProductArray序数对应
/////////////////////////////////////////////////

			nCount++;  //有效记录数加1
		}
		
		pAccess->Next();	
	}
	
///此处可以精简CustomArray和ProductArray内存，使其大小等于有效记录数, ask zlq
///注意不要和RemoveDuplicate函数重复精简（待看）
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
				//开始更换新的客户的销售记录,处理上一个客户的数据
				CountItem(&tempprod,valarray,freqarray,nLen,pStop);
				//清空arClient	
				tempprod.RemoveAll();
				//加入新的客户数据
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

//Aproiri_gen12支持大数据版本
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
				//开始更换新的客户的销售记录,处理上一个客户的数据
				CountItem(&tempprod,valarray,freqarray,nLen,pStop);
				//清空arClient	
				tempprod.RemoveAll();
				//加入新的客户数据
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
//add by liytao 20030821 //===疑仅为 单维关联规则调用
void CFileAssociationRule::CreateFrequentSet(void * pt,bool bString)
{
	unsigned int nComponentID;
	CProgressCtrl * pBar = NULL;
	TMsgInfo * pMsg = (TMsgInfo*)pt;
	CDlgProgress * lp = NULL;
	bool bStop = false;
	TDataSet *pCustom=NULL,*pProduct=NULL;
	m_pResult = new CResult("关联规则");

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
		strText.Format( "正在进行数据初始化处理..." );
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
			CRsltElementText *pText=new CRsltElementText("警告");
			CTString str;
			str=_T("无有效记录，无法计算\n");
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
		//已经出现过该商品
		}
		else
		{
			//新加入
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
	
	//产生1－频繁项集，并对该项集排序；
	int miniSupport = ceil((double)pMsg->nSupport * m_nCustomCount/100);//#_S 2004-11-23 $ 13 zlq 用百分比形式
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
		WritePtrData(parOneFrequentSet,"1项频繁集", &bStopWork,m_pResult,true);//,m_pResultWnd);

		//建立并发现2-n项集
		bool bFlag = true;
		int k = 0;
		int mark = parOneFrequentSet->GetSize();
		while( bFlag )
		{
			//pBar->StepIt();
			if( bStopWork )
			{  
				//程序被用户终止
				//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
				break;
			}
			CPtrArray arCk;
			CTString str;
			str.Format("由%d项集生成%d项集,当前项集的长度为%d!",k+1,k+2,mark);
			pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);
			bool bEndFlag;
			if (k==0)
			{
				bEndFlag= Aproiri_gen12(pCustom,pProduct,nCount,(CPtrArray *)arLk.GetAt(k),k,mark,&arCk,lp,&bStopWork,miniSupport);
				if( bStopWork )
				{  
					//程序被用户终止
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
					//程序被用户终止
					//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
					break;
				}
				if( bEndFlag )
					break;
				//InXthenCount(&arCk,lp);	//对事务数据循环 modified by liytao 20030219
				InXthenCount(pCustom,pProduct,nCount,&arCk,lp);	//对事务数据循环
			}
			if( bStopWork )
			{  
				//程序被用户终止
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
			str.Format("%d项频繁集",k+1);
			WritePtrData(pArray,str.GetBuffer(10),&bStopWork,m_pResult,true);//,m_pResultWnd);
		}
	}

	if( !bStopWork )
	{
		//求解confidence
		CTString str;
		str.Format("计算置信度!");
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

//单维关联主函数
//原代码见紧接上面的函数定义
//优化代码，并使其支持大数据量  @_$ 2005-5-24 cx 
void CFileAssociationRule::CreateFrequentSet(CRuleTrainVO * pt,bool bString)
{
 	CRuleTrainVO * pMsg = (CRuleTrainVO*)pt;
	bool bStopWork = false;
	bool bStop = false;
	m_pResult = new CResult("单维关联规则");
	TDColAry CustomArray,ProductArray;//存储客户标识和商品标识
	int nCount=0;	
	CTString strText;	
	
	//获取数据并进行预处理
	if( pt )
	{
		for (int i=0;i<m_tVarInfo.GetSize();i++)
		{
			TFieldRole *pField = (TFieldRole *)m_tVarInfo.GetAt(i);
			if (pField->strRole == "输入变量")
				m_nCustomCol = pField->nFieldNo;
			else if (pField->strRole == "目标变量")
				m_nProductCol = pField->nFieldNo;
		}

		grouper.reset();
		grouper.setGrpCount(2);

		//获取客户和商品有效数据集，其中将原始数据转换成double型标识，返回有效记录数
		//有效记录在前nCount记录中。
		//strText.Format( "正在获取有效数据..." );
		//pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&strText);
		nCount=GetValidData(m_pDataInterface,m_nCustomCol,m_nProductCol,CustomArray,ProductArray);
		
		
		if (nCount>0)
		{
 			if(!bStopWork)
			{
				//对客户标识排序
				//prshl(CustomArray,nCount,true);
				//strText.Format( "正在排序..." );
				//pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&strText);
				MergeSort(CustomArray,nCount);				
			}
 			if(!bStopWork)
			{
				//去除重复数据
				//nCount=RemoveDuplicate(CustomArray,ProductArray,nCount);
				//strText.Format( "正在去除重复数据..." );
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
			CRsltElementText *pText=new CRsltElementText("警告");
			CTString str;
			str=_T("无有效记录，无法计算\n");
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
			//已经出现过该商品
		}
		else
		{
			//新加入
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
	
	//产生1－频繁项集，并对该项集排序；
	//int miniSupport = ceil((double)pMsg->nSupport * m_nCustomCount/100);//#_S 2004-11-23 $ 13 zlq 用百分比形式
	int miniSupport = ceil((double)pMsg->nSupport * m_nCustomCount);//小数形式，如0.7,modify by xds 20060601
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
		WritePtrData(parOneFrequentSet,"1项频繁集", &bStopWork,m_pResult,true);//,m_pResultWnd);

		//建立并发现2-n项集
		bool bFlag = true;
		int k = 0;
		int mark = parOneFrequentSet->GetSize();
		while( bFlag )
		{
//			if( bStopWork )
//			{  
//				//程序被用户终止
//				//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
//				break;
//			}
			CPtrArray arCk;
			CTString str;
			str.Format("%d项集生成%d项集,当前项集长度%d...",k+1,k+2,mark);
// 			pProcessWnd->SendMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);
			bool bEndFlag;
			if (k==0)
			{
				bEndFlag= Aproiri_gen12(CustomArray,ProductArray,nCount,(CPtrArray *)arLk.GetAt(k),k,mark,&arCk/*,lp*/,&bStopWork,miniSupport);
				if( bStopWork )
				{  
					//程序被用户终止
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
					//程序被用户终止
					//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
					break;
				}
				if( bEndFlag )break;
				InXthenCount(CustomArray,ProductArray,nCount,&arCk);	//对事务数据循环
			}
			if( bStopWork )
			{  
				//程序被用户终止
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
			str.Format("%d项频繁集",k+1);
			WritePtrData(pArray,str,&bStopWork,m_pResult,true);//,m_pResultWnd);
		}
//	}

// 	if( !bStopWork )
// 	{
		//求解confidence
//		CTString str;
//		str.Format("计算置信度...");
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
	m_pResult = new CResult("单维关联规则");
	TDColAry CustomArray,ProductArray;//存储客户标识和商品标识
	int nCount=0;	
	CTString strText;	
	
	//获取数据并进行预处理
	//for (int i=0;i<pVO->arFeildInfo.GetSize();i++)
/* Modified by pyx 2006-01-05
		for (int i=0;i<m_tVarInfo.GetSize();i++)
		{
			TFieldRole *pField = (TFieldRole *)m_tVarInfo.GetAt(i);
			if (pField->strRole == "输入变量")
				m_nCustomCol = pField->nFieldNo;
			else if (pField->strRole == "目标变量")
				m_nProductCol = pField->nFieldNo;
		}*/
	m_nCustomCol = pDataInterface->IndexByName(m_strInputField.GetAt(0));
	m_nProductCol = pDataInterface->IndexByName(m_strDestField.GetAt(0));

	grouper.reset();
	grouper.setGrpCount(2);

	//获取客户和商品有效数据集，其中将原始数据转换成double型标识，返回有效记录数
	//有效记录在前nCount记录中。
	nCount=GetValidData(m_pDataInterface,m_nCustomCol,m_nProductCol,CustomArray,ProductArray);
	
	
	if (nCount>0)
	{
		//对客户标识排序
		MergeSort(CustomArray,nCount);				
		nCount=RemoveDuplicate_quick_test(CustomArray,ProductArray,nCount);
	}
	else
	{
		CRsltElementText *pText=new CRsltElementText("警告");
		CTString str;
		str=_T("无有效记录，无法计算\n");
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
			//已经出现过该商品
		}
		else
		{
			//新加入
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
	
	//产生1－频繁项集，并对该项集排序；
	//int miniSupport = ceil((double)m_dSupport * m_nCustomCount/100);//#_S 2004-11-23 $ 13 zlq 用百分比形式
	int miniSupport = ceil((double)m_dSupport * m_nCustomCount);//小数形式，如0.7,modify by xds 20060601
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
	WritePtrData(parOneFrequentSet,"1项频繁集", &m_bStopWork,m_pResult,true);//,m_pResultWnd);

	//建立并发现2-n项集
	bool bFlag = true;
	int k = 0;
	int mark = parOneFrequentSet->GetSize();
	while( bFlag )
	{
		CPtrArray arCk;
		CTString str;
		str.Format("%d项集生成%d项集,当前项集长度%d...",k+1,k+2,mark);
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
				//程序被用户终止
				//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
				break;
			}
			if( bEndFlag )break;
			InXthenCount(CustomArray,ProductArray,nCount,&arCk);	//对事务数据循环
		}
		if( m_bStopWork )
		{  
			//程序被用户终止
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
		str.Format("%d项频繁集",k+1);
		WritePtrData(pArray,str,&m_bStopWork,m_pResult,true);//,m_pResultWnd);
	}

	if (!m_bStopWork)
	{
		//求解confidence
		CTString str;
		str.Format("计算置信度...");
		//Confidence(&arLk,this->m_dConfidence/100,m_pResult,&this->m_bStopWork,true);
		Confidence(&arLk,this->m_dConfidence,m_pResult,&this->m_bStopWork,true);//小数形式，如0.7,modify by xds 20060601
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


//判断是不是在频繁项集中
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
		pProgress->SetIndex("对事务数据循环!");		
	}
	if( pProcessWnd )
	{
		CTString str = _T("遍历事务数据库!");
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
			//用户终止计算
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
				//开始更换新的客户的销售记录,处理上一个客户的数据
				CountItem(&tempprod,par,pbStop);
				//清空arClient	
				tempprod.RemoveAll();
				//加入新的客户数据
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

//InXthenCount的支持大数据版本
//对进入频繁项集的商品计数
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
		pProgress->SetIndex("对事务数据循环!");		
	}
*/
/*
	if( pProcessWnd )
	{
		CTString str = _T("遍历事务数据库!");
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
			//用户终止计算
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
				//开始更换新的客户的销售记录,处理上一个客户的数据
				CountItem(&tempprod,par,pbStop);
				//清空arClient	
				tempprod.RemoveAll();
				//加入新的客户数据
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
		pProgress->SetIndex("对事务数据循环!");		
	}
	if( pProcessWnd )
	{
		CTString str = _T("遍历事务数据库!");
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
			//用户终止计算
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
				//还是同一个客户的销售记录
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
				//开始更换新的客户的销售记录,处理上一个客户的数据
				CountItem(&arClient,par,pbStop);
				//清空arClient	
				FreePtrArray(&arClient);
				//加入新的客户数据
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

//选择排序
//排序项集的函数
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
	//开始更换新的客户的销售记录,处理上一个客户的数据
	//pUnit = (TUnit *)par->GetAt(0);
	i=0;

	if( nParClientSize < 2 )
	{
		return;				//如果客户的销售记录小于项集数目就可以返回
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

//对每一个客户的销售记录进行统计
void CFileAssociationRule::CountItem(CTArray<long,long> *parClient, CPtrArray *par,bool *pStop)
{
	TUnit * pUnit = NULL;
	int nproductcount = 0;
	int minCount = 0;
	int count=0;
	int i,j,k;
	int nParClientSize=parClient->GetSize();
	//开始更换新的客户的销售记录,处理上一个客户的数据
	pUnit = (TUnit *)par->GetAt(0);
	nproductcount = pUnit->arIndex.GetSize();	//求解该单元中商品的个数
	int nParSize=par->GetSize();
	i=0;
	if( nParClientSize < nproductcount )
	{
		return;				//如果客户的销售记录小于项集数目就可以返回
	}
	while (i<nParSize)
	//for(i=0;i<par->GetSize();i++)//循环每个项集中的商品序列
	{

		if (pStop)
		{
			if (*pStop)
				break;

		}


		pUnit = (TUnit *)par->GetAt(i);				//得到项集中第i个单元的的数据

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
				long tempproduct = parClient->GetAt(j);	//得到客户销售记录中的第j个记录
				if( *plong == tempproduct )
				{
					// 一层的分析,多层时应该去掉break
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

//对每一个客户的销售记录进行统计
/*
void CFileAssociationRule::CountItem(CPtrArray *parClient, CPtrArray *par,bool *pStop)
{
	TUnit * pUnit = NULL;
	int nproductcount = 0;
	int minCount = 0;
	int count=0;
	int i,j,k;
	//开始更换新的客户的销售记录,处理上一个客户的数据
	pUnit = (TUnit *)par->GetAt(0);
	nproductcount = pUnit->arIndex.GetSize();	//求解该单元中商品的个数
	for(i=0;i<par->GetSize();i++)//循环每个项集中的商品序列
	{
		if (pStop)
		{
			if (*pStop)
				break;

		}

		pUnit = (TUnit *)par->GetAt(i);				//得到项集中第i个单元的的数据
		if( parClient->GetSize() < nproductcount )
		{
			return;				//如果客户的销售记录小于项集数目就可以返回
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
				TTransaction * pt = (TTransaction *)parClient->GetAt(j);	//得到客户销售记录中的第j个记录
				if( *plong == pt->lProductID )
				{
					count++;
					// 一层的分析,多层时应该去掉break
					break;
				}
			}
			if( count < minCount )
			{
				minCount = count;				
			}
			if( count == 0 )	//假如其中有一个商品没有出现，就不用再比较了
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
	pTable->InsertColumn(0,"频繁项集");
	pTable->InsertColumn(1,"频    数");
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
	
//***************************added by zkc 2004.5.17 zlq 修改  ********************************								
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
	CRsltElementText * pText= new CRsltElementText("强规则");
	assert(pResult);
	pResult->Add(pText);
	//CTString strResult;

	CTString strResultTemp;//临时存放用 //#_S 2004-11-25 zlq
	CTStringArray strArray;//临时存放用

	CTString str;
	strResultTemp += _T("强规则\n\n");
	for(int i=1;i<par->GetSize();i++)			//对k-频繁项集,用i循环;
	{
		if( pbStop && *pbStop == true )
		{
			strArray.Add(strResultTemp);
			//用户终止了计算
			pText->AddString(GetString(strArray));
			return;
		}
		CPtrArray * pkar = (CPtrArray *)par->GetAt(i);
		for(int j=0;j<pkar->GetSize() && !(*pbStop);j++)		//对k-频繁项集中的单元数目,用j循环;
		{
			TUnit * pUnit = (TUnit *)pkar->GetAt(j);
			int nLen = pUnit->arIndex.GetSize();
			assert( i+1 == nLen );				//i就是k-频繁项集中的单元的长度;
			CIntVector vect(nLen);
			int k;
			for(k=0;k<nLen;k++)				//将k-频繁项集中的第j单元的内容赋给vect;
			{
				vect[k] = *(int *)pUnit->arIndex.GetAt(k);
			}
			for(k=1;k<nLen && !(*pbStop);k++)					//求取k-频繁项集中的第j单元的第s(s=k)子项集;
			{
				CIntMatrix matrix_subset = nchoosek(vect,k);
				for(int l=0;l< matrix_subset.mrow() && !(*pbStop);l++)//对第s子项集元素循环,求取可信度;
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
//***************************added by zkc 2004.5.17 zlq 修改  ********************************								
							CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(this->m_nProductCol);
							CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq
							str = strNew;

							
///*************************end of addition zkc 2004.5.17*********************
							
							//strResult += str; 用下面行取代
							AddString(strArray,strResultTemp,str);

							if( m != vect_subset.vlen()-1 )
							{			
								//strResult += _T("Λ ");
								AddString(strArray,strResultTemp,_T("Λ "));
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

//***************************added by zkc 2004.5.17 zlq 修改  ********************************								
							CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(this->m_nProductCol);
							CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq
							str = strNew;							
///*************************end of addition zkc 2004.5.17*********************
							//strResult += str;
							AddString(strArray,strResultTemp,str);
							if( m != vecttemp.vlen()-1 )
							{			
								//strResult += _T("Λ ");
								AddString(strArray,strResultTemp,_T("Λ "));
							}
						}						
						str.Format("      支持度 = %5.2f%%,    置信度 = %5.2f%%\n",(double)pUnit->count/m_nCustomCount*100,confidence*100);	
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
		strResult += _T("本次计算没有找到符合要求得强规则!请重新设置参数计算!");
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
*根据顾客列表，得到顾客的个数（虑除相同的顾客）
*pCustom:顾客数组
*nCount：顾客数组的大小
*nCustCol  :在pDataInterface对应的列号，（为得到类型用）
*pDataInterface:（为得到类型用）
*返回不同顾客的个数
*/
/*int CFileAssociationRule::GetDifferCount(TDataSet *pCustom, int nCount, int nCustCol, CDataInterface *pDataInterface)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	int nRowIndex=1,nCustFieldType;//,nProductFieldType;
	
	nCustFieldType=pAccess->m_pFieldList->FieldByIndex(nCustCol)->GetFieldType();
	int nDifferCount =1;
	int *pBuf = new int[nCount];//存放数据的指针值
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
















































