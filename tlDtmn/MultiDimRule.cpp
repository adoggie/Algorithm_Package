// MultiDimRule.cpp: implementation of the CMultiDimRule class.
//
//
//多维维间关联
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "MultiDimRule.h"
// #include "DlgProgress.h"
#include "Common.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiDimRule::CMultiDimRule()
{
	m_arInputArray.RemoveAll();
	m_arTargetArray.RemoveAll();
}

CMultiDimRule::~CMultiDimRule()
{

}

void	CMultiDimRule::SetDataParam(CRuleTrainVO	*pVO)
{
}

void	CMultiDimRule::SetDataInterface(CDataInterface *pDataInterface, CPtrArray *parField)
{
	m_pDataInterface = pDataInterface;
	m_tVarInfo.Copy(*parField);
}

//多维关联规则主函数，本算法并没有考虑到“多维关联规则（商品购买）.doc”中的情况。
//支持大数据量，并优化代码。
//原代码见上面被注释的部分
void CMultiDimRule::CreateFrequentSet(CRuleTrainVO * pt,bool bString)
{
	CRuleTrainVO * pMsg = pt; 
	bool bStopWork = false;
	bool bStop = false;
	m_pResult = new CResult("维间关联规则");
	
	TDColAry CustomArray,ProductArray;

	int nCount=0,transCount=0;	
	if( pt )
	{
		for (int i=0;i<this->m_tVarInfo.GetSize();i++)
		{
			TFieldRole *pField = (TFieldRole *)m_tVarInfo.GetAt(i);
			if (pField->strRole == "输入变量")
				m_nCustomCol = pField->nFieldNo;
			else if (pField->strRole == "目标变量")
				m_nProductCol = pField->nFieldNo;
		}
		CTString strText;
		strText.Format( "正在进行数据初始化处理..." );

		//	m_pDataInterface->m_DataAccess.MergeSort(CustomCol);
		grouper.reset();
		grouper.setGrpCount(m_arInputArray.GetSize());
		nCount=MergeDims(m_pDataInterface,&m_tVarInfo,CustomArray,ProductArray,transCount);
	
			
		if (nCount>0)
		{
			if(!bStopWork)
			{
			//////////////////////////////////////////////////////////////////////////
			//似乎可以不用排序，因为pCustom本身已经排好序
			//	prshl(pCustom,nCount,true);
			//////////////////////////////////////////////////////////////////////////
			}				
				
			if(!bStopWork)
			{
			//////////////////////////////////////////////////////////////////////////
			//似乎可以不用去重复记录，因为这个去重复记录的算法是针对单维
			//不能识别多维的重复记录
			//	nCount=RemoveDuplicate(pCustom,pProduct,nCount);
			//////////////////////////////////////////////////////////////////////////				
			}

			
			if(bStopWork)
			{
				if(m_pResult)
					delete m_pResult;
				m_pResult=NULL;

				if (pMsg!=NULL)
					delete pMsg;


// 				m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
// 				CWinThread * pThread = AfxGetThread();
// 				DWORD nexit;
//				::GetExitCodeThread(pThread->m_hThread,&nexit);
//				AfxEndThread(nexit,true);
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
			if (pMsg!=NULL)
				delete pMsg;
			pMsg=NULL;	
// 			m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
// 			CWinThread * pThread = AfxGetThread();
// 			DWORD nexit;
//			::GetExitCodeThread(pThread->m_hThread,&nexit);
//			AfxEndThread(nexit,true);
			return ;
		}
	}

	CPtrArray * parOneFrequentSet = new CPtrArray;
	TTransaction * pRecord = new TTransaction;
	int i;
	for(i=0;i<nCount && !bStopWork;i++)
	{
		if(bStopWork)
		{
			if(m_pResult)
				delete m_pResult;
			m_pResult=NULL;
			if( parOneFrequentSet)
				delete parOneFrequentSet;
			parOneFrequentSet=NULL;
			if(pRecord)
				delete pRecord;
			pRecord=NULL;

			if (pMsg!=NULL)
				delete pMsg;
			pMsg=NULL;	
// 			m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
// 			CWinThread * pThread = AfxGetThread();
// 			DWORD nexit;
//			::GetExitCodeThread(pThread->m_hThread,&nexit);
//			AfxEndThread(nexit,true);

			return;
		}
		pRecord->lCustomerID = (long)(CustomArray[i].dValue+0.5);
		pRecord->lProductID = (long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5);
		
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
	//int miniSupport = ceil((double)pMsg->nSupport * transCount/100);	
	int miniSupport = ceil((double)pMsg->nSupport * transCount);//小数形式，如0.7,modify by xds 20060601	
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
		//TRACE("%d\n",parOneFrequentSet->GetSize());
		SortItem(parOneFrequentSet);
		arLk.Add(parOneFrequentSet);
// 		(pMsg->pDataInterface,&pMsg->inputindexs,parOneFrequentSet,"1项频繁集", &bStopWork,m_pResult);//,m_pResultWnd);

		//建立并发现2-n项集
		bool bFlag = true;
		int k = 0;
		int mark = parOneFrequentSet->GetSize();
		while( bFlag )
		{
			
			if( bStopWork )
			{  
				//程序被用户终止
				//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
				break;
			}
			CPtrArray arCk;
			CTString str;
			str.Format("由%d项集生成%d项集,当前项集的长度为%d!",k+1,k+2,mark);
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
				InXthenCount(CustomArray,ProductArray,nCount,&arCk);//对事务数据循环
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
// 			WritePtrData(pMsg->pDataInterface,&pMsg->inputindexs,pArray,str,&bStopWork,m_pResult);
		}
	}
	if( !bStopWork )
	{
		//求解confidence
		CTString str;
		str.Format("计算置信度...");
// 		pProcessWnd->PostMessage(RegisterWindowMessage("WM_PROGRESS_TEXT"),0,(LPARAM)&str);	
	
//		if( pMsg->bSaveRule&&pMsg->strRuleFileName!=_T(""))
		if(true)
			Confidence(&arLk,m_pResult,&bStopWork,transCount,true);
		else
			Confidence(&arLk,m_pResult,&bStopWork,transCount,false);
	}

	for(i=0;i<arLk.GetSize();i++)
	{
		CPtrArray * ar = (CPtrArray *)arLk.GetAt(i);
		this->FreePtrArray(ar);
		delete ar;
	}

	if( bStopWork )
	{
		delete m_pResult;
		m_pResult = NULL;
	}

	//delete pt;
	//pt=NULL;
	if (pMsg!=NULL)delete pMsg;
	pMsg=NULL;	

// 	m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
//	CWinThread * pThread = AfxGetThread();
//	DWORD nexit;
//	::GetExitCodeThread(pThread->m_hThread,&nexit);
//	AfxEndThread(nexit,true);
}

CResult* CMultiDimRule::CreateRule(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt)
{
	m_pDataInterface = pDataInterface;
	bool bStop = false;
	m_pResult = new CResult("维间关联规则");
	int i;
	TDColAry CustomArray,ProductArray;

	TFieldRole *pField = new TFieldRole;
	pField->strFieldName = m_strDestField.GetAt(0);
	pField->nFieldNo = pDataInterface->IndexByName(pField->strFieldName);
	pField->strRole = "目标变量";
	m_tVarInfo.Add(pField);
	for (i=0;i<m_strInputField.GetSize();i++)
	{
		pField = new TFieldRole;
		pField->strFieldName = m_strInputField.GetAt(i);
		pField->nFieldNo = pDataInterface->IndexByName(pField->strFieldName);
		pField->strRole = "输入变量";
		m_tVarInfo.Add(pField);
	}

	int nCount=0,transCount=0;	
	for (i=0;i<this->m_tVarInfo.GetSize();i++)
	{
		pField = (TFieldRole *)m_tVarInfo.GetAt(i);
		if (pField->strRole == "输入变量")
		{
			m_arInputArray.Add(pField);
			//m_nCustomCol = pField->nFieldNo;
		}
		else if (pField->strRole == "目标变量")
		{
			m_arTargetArray.Add(pField);
			m_arInputArray.Add(pField);
			//m_nProductCol = pField->nFieldNo;
		}
	}
	CTString strText;
	strText.Format( "正在进行数据初始化处理..." );

	//	m_pDataInterface->m_DataAccess.MergeSort(CustomCol);
	grouper.reset();
	grouper.setGrpCount(m_tVarInfo.GetSize());
	nCount=MergeDims(m_pDataInterface,&m_arInputArray,CustomArray,ProductArray,transCount);

		
	if (nCount>0)
	{
		if(!m_bStopWork)
		{
		//////////////////////////////////////////////////////////////////////////
		//似乎可以不用排序，因为pCustom本身已经排好序
		//	prshl(pCustom,nCount,true);
		//////////////////////////////////////////////////////////////////////////
		}				
			
		if(!m_bStopWork)
		{
		//////////////////////////////////////////////////////////////////////////
		//似乎可以不用去重复记录，因为这个去重复记录的算法是针对单维
		//不能识别多维的重复记录
		//	nCount=RemoveDuplicate(pCustom,pProduct,nCount);
		//////////////////////////////////////////////////////////////////////////				
		}

		
		if(m_bStopWork)
		{
			if(m_pResult)
				delete m_pResult;
			m_pResult=NULL;


// 				m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
// 				CWinThread * pThread = AfxGetThread();
// 				DWORD nexit;
//				::GetExitCodeThread(pThread->m_hThread,&nexit);
//				AfxEndThread(nexit,true);
			return false;
		}
	}
	else
	{
		CRsltElementText *pText=new CRsltElementText("警告");
		CTString str;
		str=_T("无有效记录，无法计算\n");
		pText->AddString(str);
		m_pResult->Add(pText);
// 			m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
// 			CWinThread * pThread = AfxGetThread();
// 			DWORD nexit;
//			::GetExitCodeThread(pThread->m_hThread,&nexit);
//			AfxEndThread(nexit,true);
		return m_pResult;
	}

	CPtrArray * parOneFrequentSet = new CPtrArray;
	TTransaction * pRecord = new TTransaction;
	
	for(i=0;i<nCount && !m_bStopWork;i++)
	{
		if(m_bStopWork)
		{
			if(m_pResult)
				delete m_pResult;
			m_pResult=NULL;
			if( parOneFrequentSet)
				delete parOneFrequentSet;
			parOneFrequentSet=NULL;
			if(pRecord)
				delete pRecord;
			pRecord=NULL;

// 			m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"), nComponentID, (LPARAM)m_pResult);
// 			CWinThread * pThread = AfxGetThread();
// 			DWORD nexit;
//			::GetExitCodeThread(pThread->m_hThread,&nexit);
//			AfxEndThread(nexit,true);

			return false;
		}
		pRecord->lCustomerID = (long)(CustomArray[i].dValue+0.5);
		pRecord->lProductID = (long)(ProductArray[CustomArray[i].iIndex-1].dValue+0.5);
		
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
	//int miniSupport = ceil((double)m_dSupport * transCount/100);	
	int miniSupport = ceil((double)m_dSupport * transCount);//小数形式，如0.7,modify by xds 20060601	
	CPtrArray arLk;
	if( !m_bStopWork )
	{
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
 		WritePtrData(m_pDataInterface,&m_arInputArray,parOneFrequentSet,"1项频繁集", &m_bStopWork,m_pResult);//,m_pResultWnd);

		//建立并发现2-n项集
		bool bFlag = true;
		int k = 0;
		int mark = parOneFrequentSet->GetSize();
		while( bFlag )
		{
			
			if( m_bStopWork )
			{  
				//程序被用户终止
				break;
			}
			CPtrArray arCk;
			CTString str;
			str.Format("由%d项集生成%d项集,当前项集的长度为%d!",k+1,k+2,mark);
			bool bEndFlag;
			if (k==0)
			{
				bEndFlag= Aproiri_gen12(CustomArray,ProductArray,nCount,(CPtrArray *)arLk.GetAt(k),k,mark,&arCk/*,lp*/,&m_bStopWork,miniSupport);
				if( m_bStopWork )
				{  
					//程序被用户终止
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
					break;
				}
				if( bEndFlag )break;
				InXthenCount(CustomArray,ProductArray,nCount,&arCk);//对事务数据循环
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
 			//WritePtrData(pMsg->pDataInterface,&pMsg->inputindexs,pArray,str,&bStopWork,m_pResult);
			WritePtrData(m_pDataInterface,&m_arInputArray,pArray,str, &m_bStopWork,m_pResult);
		}
	}
	if( !m_bStopWork )
	{
		//求解confidence
		CTString str;
		str.Format("计算置信度...");

		if(true)
			Confidence(&arLk,m_pResult,&m_bStopWork,transCount,true);
		else
			Confidence(&arLk,m_pResult,&m_bStopWork,transCount,false);
	}

	for(i=0;i<arLk.GetSize();i++)
	{
		CPtrArray * ar = (CPtrArray *)arLk.GetAt(i);
		this->FreePtrArray(ar);
		delete ar;
	}

	if( m_bStopWork )
	{
		delete m_pResult;
		m_pResult = NULL;
	}

	return m_pResult;
}

CResult* CMultiDimRule::Predict(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt)
{
	return m_pResult;
}

//@_$ 2005-6-13 cx
//大数据版本
//修正了原代码对缺失值处理部分的错误，精简和优化代码
int CMultiDimRule::MergeDims(CDataInterface *pDataInterface, CPtrArray* inputs,
							 TDColAry &CustomArray,TDColAry &ProductArray,int &transCount)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
		long nRecord=pAccess->GetRecordCount();
		if (nRecord<=0 )return 0;
		//inputs中保存所有变量集(包括所选择的输入变量和目标变量)的各索引号
		int inputCount=0;//所有变量个数，即维数
		inputCount = inputs->GetSize();
			
		CField **ppProductField=new CField* [inputCount];	
		int *pProductFieldType=new int [inputCount];
		int i;
		for(i=0;i<inputCount;i++)
		{
			TFieldRole *pRol = (TFieldRole *)inputs->GetAt(i);
			//if (pRol->strRole == CTString("输入变量"))
			//			{
			
				ppProductField[i]=pAccess->m_pFieldList->FieldByIndex(pRol->nFieldNo);
				pProductFieldType[i]=ppProductField[i]->GetFieldType();
			// }
		}
	
		//此处不精确考虑除缺失值外实际需要使用的空间，以空间换速度
		//这些浪费空间对大数据类TDColAry影响不大
		CustomArray.SetSize(nRecord*inputCount);
		ProductArray.SetSize(nRecord*inputCount);
		
		//取非缺失值数据，并用对应标识保存
		int nRowIndex=1,nCount=0;
		TDataSet tempdata;
		int idindex,flag;
		pAccess->First();
		while(!pAccess->Eof())
		{
			//若某个域的数据为缺失值，则跳过这条记录
			flag=0;
			for (i=0;i<inputCount;i++)
			{
				if( pDataInterface->IsMissingValue( ppProductField[i], true))
				{
					flag=1;
					break;
				}
			}
			if(flag==1)
			{
				pAccess->Next();
				continue;
			}
			
			//读入有效数据
			for (i=0;i<inputCount;i++)
			{
				if(pProductFieldType[i]==fString)
				{
					tempdata.strVal=ppProductField[i]->GetAsString();
				}
				else
				{
					tempdata.dValue=ppProductField[i]->GetAsDouble();
				}			
				
				idindex=grouper.getIDIndex(i,tempdata);
				if (idindex<0)
				{
					grouper.add(i,pProductFieldType[i],tempdata);
					idindex=grouper.getIDIndex(i,tempdata);
				}
				//产品数组记录所有属性字段的标识
				ProductArray[nCount].dValue=grouper.getID(idindex);
				ProductArray[nCount].iIndex=nCount+1;
				//客户数组记录行索引
				CustomArray[nCount].dValue =nRowIndex;
				CustomArray[nCount].iIndex=nCount+1;
				
				nCount++;
			}
			nRowIndex++;
			pAccess->Next();	
		}
		transCount=nRowIndex-1;//得到有效记录数
		
		delete [] ppProductField;
		delete [] pProductFieldType;
		
		return nCount;
	
}

void CMultiDimRule::WritePtrData(CDataInterface* pDataInterface,CPtrArray* inputs,CPtrArray *par, char *pszTitle,bool *pStop,CResult * pResult)
{	
	if( par->GetSize() <= 0 )
		return;

	CTLTable * pTable = new CTLTable;
//	assert(pWnd->m_hWnd);
	pTable->SetTitle(pszTitle);
	pTable->SetRows(1+par->GetSize());
	pTable->SetCols(2);
	TFieldRole * pFRole = NULL;
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
			CTString strName;
			pFRole = (TFieldRole *)m_arInputArray.GetAt(grpindex);
			//strName=pDataInterface->GetFieldName((int)inputs->GetAt(grpindex));
			strName=pDataInterface->GetFieldName(pFRole->nFieldNo);
//***************************added by zkc 2004.5.17 zlq 修改  ********************************								
			//CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex((inputs->GetAt(grpindex)));
			CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(pFRole->nFieldNo);
			CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq

			str.Format("(%s)    ",strNew.GetData());
			//str+="  ";

//*********end of addition by zkc 2004.5.17********
			str=strName+str;
			strResult += str;
		}
		pTable->InsertItem(i,strResult);
		str.Format("%12d",pUnit->count);
		strResult += str;
		pTable->SetItemText(i,1,str);		
	}
}

bool CMultiDimRule::IsTarget(int Index)
{
	int count = m_arTargetArray.GetSize();
	TFieldRole *pField = NULL;
	for (int i = 0;i < count; i++)
	{
		pField = (TFieldRole *)m_arTargetArray.GetAt(i);
		if (Index == pField->nFieldNo)
		{
			return true;
		}
	}
	return false;
}

void CMultiDimRule::Confidence(CPtrArray *par,CResult * pResult,bool * pbStop,int transCount,bool bSave)
{

	CTLTable * pTable= new CTLTable();//"强规则");
	assert(pResult);
	CTString strTmpRes;
	CTString str;
	TFieldRole *pFRole = NULL;
	pTable->SetTitle("强规则");

	pTable->InsertColumn(0,"规则");
	pTable->InsertColumn(1,"支持度");
	pTable->InsertColumn(2,"置信度");
	pTable->InsertColumn(3,"n_频繁项");

	CDataAccess*	m_pRuleAccess=new CDataAccess();
	CFieldList* pFieldList=NULL;
	if (bSave){
		m_pRuleAccess->NewFile();
		pFieldList = m_pRuleAccess->m_pFieldList;

		CField* pTempField;
		pTempField = pFieldList->CreateField(fString);
		pTempField->SetFieldName("前提");
		pFieldList->Add(pTempField);
		pTempField = pFieldList->CreateField(fString);
		pTempField->SetFieldName("结论");
		pFieldList->Add(pTempField);
		pTempField = pFieldList->CreateField(fDouble);
		pTempField->SetFieldName("支持度");
		pFieldList->Add(pTempField);
		pTempField = pFieldList->CreateField(fDouble);
		pTempField->SetFieldName("置信度");
		pFieldList->Add(pTempField);
		pTempField = pFieldList->CreateField(fInt);
		pTempField->SetFieldName("n频繁项");
		pFieldList->Add(pTempField);
	}


	int itemCount=-1;

	for(int i=1;i<par->GetSize();i++)			//对k-频繁项集,用i循环;
	{
		if( pbStop && *pbStop == true )
		{
			//用户终止了计算
			break;
		}
		CPtrArray * pkar = (CPtrArray *)par->GetAt(i);
		for(int j=0;j<pkar->GetSize() && !(*pbStop);j++)		//对k-频繁项集中的单元数目,用j循环;
		{
			TUnit * pUnit = (TUnit *)pkar->GetAt(j);

			int nLen = pUnit->arIndex.GetSize();
			assert( i+1 == nLen );				//i就是k-频繁项集中的单元的长度;
			CIntVector vect(nLen);
			bool isExist=false;
			int k;
			for( k=0;k<nLen;k++)				//将k-频繁项集中的第j单元的内容赋给vect;
			{
				vect[k] = *(int *)pUnit->arIndex.GetAt(k);
				int grpindex=grouper.getGrpIndex(vect[k]);
				//if (IsTarget(mpMsg->inputindexs[grpindex]))
				pFRole = (TFieldRole*)m_arInputArray.GetAt(grpindex);
				if (IsTarget(pFRole->nFieldNo))
					isExist=true;
			}
			if (isExist){
				for(k=1;k<nLen && !(*pbStop);k++)					//求取k-频繁项集中的第j单元的第s(s=k)子项集;
				{
					CIntMatrix matrix_subset = nchoosek(vect,k);
					for(int l=0;l< matrix_subset.mrow() && !(*pbStop);l++)//对第s子项集元素循环,求取可信度;
					{
						CIntVector vect_subset = matrix_subset.row(l);
						int    count = GetSubSetCount((CPtrArray *)par->GetAt(k-1),vect_subset);
						double confidence = pUnit->count / (count - 0.0);
						//if( confidence > mini_confidence/100 ) modified by liytao 20030225
						if( confidence >= m_dConfidence )//小数形式，如0.7,modify by xds 20060601
						{
							CTString strPre="",strPost="";
							strTmpRes="";
							int m;
							for(m = 0;m<vect_subset.vlen();m++)
							{
								int nt = vect_subset[m];
								TDataSet *ptempdata;
								int dataType,grpindex=grouper.getGrpIndex(nt);
								dataType=grouper.getGrpDataType(grpindex);
								ptempdata=grouper.getVal(grouper.getIDIndex(nt));
								CTString strName;
								pFRole = (TFieldRole *)m_arInputArray.GetAt(grpindex);
								//strName=m_pDataInterface->GetFieldName(pMsg->inputindexs[grpindex]);strName=m_pDataInterface->GetFieldName(pMsg->inputindexs[grpindex]);
								strName=m_pDataInterface->GetFieldName(pFRole->nFieldNo);

								
//***************************added by zkc 2004.5.17 zlq 修改  ********************************								
								CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(grpindex);
								CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq
								str.Format("\"%s\"",strNew.GetData());
//**************************end of addition by zkc 2004.5.17*******************************


								str=strName+"="+str;
								strTmpRes += str;
								if( m != vect_subset.vlen()-1 )
								{			
									strTmpRes += _T("并且");
								}
							}
							strPre=strTmpRes;
							strTmpRes += _T("==>>");
							CIntVector vecttemp = VectorSub(vect,vect_subset);
							isExist=false;
							for(m = 0;m<vecttemp.vlen();m++)
							{
								int nt = vecttemp[m];
								TDataSet *ptempdata;
								int dataType,grpindex=grouper.getGrpIndex(nt);
//								if (pMsg->isTarget(pMsg->inputindexs[grpindex]))
								pFRole = (TFieldRole*)m_arInputArray.GetAt(grpindex);
								if (IsTarget(pFRole->nFieldNo))
									isExist=true;


								dataType=grouper.getGrpDataType(grpindex);
								CTString strName;
								pFRole = (TFieldRole *)m_arInputArray.GetAt(grpindex);
								//strName=m_pDataInterface->GetFieldName(pMsg->inputindexs[grpindex]);
								strName=m_pDataInterface->GetFieldName(pFRole->nFieldNo);
								ptempdata=grouper.getVal(grouper.getIDIndex(nt));
//***************************added by zkc 2004.5.17 zlq 修改  ********************************								
								CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(grpindex);
								CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq

								str.Format("\"%s\"",strNew.GetData());
//**************************end of addition by zkc 2004.5.17*******************************
								str=strName+"="+str;
								strPost += str;
								if( m != vecttemp.vlen()-1 )
								{			
									strPost += _T("并且");
								}
							}			
							strTmpRes+=strPost;
							double support=pUnit->count/(double)transCount;
							//str.Format("       置信度 = %10.2f\n",confidence);
							
							if( isExist)
							{
								pTable->InsertItem(itemCount+1,strTmpRes);
								pTable->SetItemText(itemCount+1,1,support);
								pTable->SetItemText(itemCount+1,2,confidence);
								pTable->SetItemText(itemCount+1,3,nLen);

								if (bSave)
								{
									m_pRuleAccess->Append();
									pFieldList->FieldByName("前提")->SetAsString(strPre);
									pFieldList->FieldByName("结论")->SetAsString(strPost);
									pFieldList->FieldByName("支持度")->SetAsDouble(support);
									pFieldList->FieldByName("置信度")->SetAsDouble(confidence);
									pFieldList->FieldByName("n频繁项")->SetAsInteger(nLen);

									m_pRuleAccess->Post();

								}
								itemCount++;	
							}
						}
					}				
				}
			}
		}
	}

	if (bSave){
		CTString out=_T("规则");
//		m_pRuleAccess->SaveFile(pMsg->strRuleFileName,&out);
		m_pRuleAccess->Close();
		delete m_pRuleAccess;
		m_pRuleAccess=NULL;
	}
	if( pTable->GetTableRowS()<2 )
	{
		CRsltElementText* pText=new CRsltElementText("强规则");
		pText->AddString ("本次计算没有找到符合要求得强规则!请重新设置参数计算!");
		pResult->Add(pText);
		delete pTable;
		pTable=NULL;
	}
	else{
		pResult->Add(new CRsltElementTable("强规则",pTable));
	}

}
