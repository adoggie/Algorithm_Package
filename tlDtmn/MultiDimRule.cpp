// MultiDimRule.cpp: implementation of the CMultiDimRule class.
//
//
//��άά�����
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

//��ά�������������������㷨��û�п��ǵ�����ά����������Ʒ����.doc���е������
//֧�ִ������������Ż����롣
//ԭ��������汻ע�͵Ĳ���
void CMultiDimRule::CreateFrequentSet(CRuleTrainVO * pt,bool bString)
{
	CRuleTrainVO * pMsg = pt; 
	bool bStopWork = false;
	bool bStop = false;
	m_pResult = new CResult("ά���������");
	
	TDColAry CustomArray,ProductArray;

	int nCount=0,transCount=0;	
	if( pt )
	{
		for (int i=0;i<this->m_tVarInfo.GetSize();i++)
		{
			TFieldRole *pField = (TFieldRole *)m_tVarInfo.GetAt(i);
			if (pField->strRole == "�������")
				m_nCustomCol = pField->nFieldNo;
			else if (pField->strRole == "Ŀ�����")
				m_nProductCol = pField->nFieldNo;
		}
		CTString strText;
		strText.Format( "���ڽ������ݳ�ʼ������..." );

		//	m_pDataInterface->m_DataAccess.MergeSort(CustomCol);
		grouper.reset();
		grouper.setGrpCount(m_arInputArray.GetSize());
		nCount=MergeDims(m_pDataInterface,&m_tVarInfo,CustomArray,ProductArray,transCount);
	
			
		if (nCount>0)
		{
			if(!bStopWork)
			{
			//////////////////////////////////////////////////////////////////////////
			//�ƺ����Բ���������ΪpCustom�����Ѿ��ź���
			//	prshl(pCustom,nCount,true);
			//////////////////////////////////////////////////////////////////////////
			}				
				
			if(!bStopWork)
			{
			//////////////////////////////////////////////////////////////////////////
			//�ƺ����Բ���ȥ�ظ���¼����Ϊ���ȥ�ظ���¼���㷨����Ե�ά
			//����ʶ���ά���ظ���¼
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
			CRsltElementText *pText=new CRsltElementText("����");
			CTString str;
			str=_T("����Ч��¼���޷�����\n");
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
	//int miniSupport = ceil((double)pMsg->nSupport * transCount/100);	
	int miniSupport = ceil((double)pMsg->nSupport * transCount);//С����ʽ����0.7,modify by xds 20060601	
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
// 		(pMsg->pDataInterface,&pMsg->inputindexs,parOneFrequentSet,"1��Ƶ����", &bStopWork,m_pResult);//,m_pResultWnd);

		//����������2-n�
		bool bFlag = true;
		int k = 0;
		int mark = parOneFrequentSet->GetSize();
		while( bFlag )
		{
			
			if( bStopWork )
			{  
				//�����û���ֹ
				//m_pMsgWnd->SendMessage(RegisterWindowMessage("WM_CALCULATE_OVER"),nComponentID, (LPARAM)m_pResult);
				break;
			}
			CPtrArray arCk;
			CTString str;
			str.Format("��%d�����%d�,��ǰ��ĳ���Ϊ%d!",k+1,k+2,mark);
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
				InXthenCount(CustomArray,ProductArray,nCount,&arCk);//����������ѭ��
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
// 			WritePtrData(pMsg->pDataInterface,&pMsg->inputindexs,pArray,str,&bStopWork,m_pResult);
		}
	}
	if( !bStopWork )
	{
		//���confidence
		CTString str;
		str.Format("�������Ŷ�...");
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
	m_pResult = new CResult("ά���������");
	int i;
	TDColAry CustomArray,ProductArray;

	TFieldRole *pField = new TFieldRole;
	pField->strFieldName = m_strDestField.GetAt(0);
	pField->nFieldNo = pDataInterface->IndexByName(pField->strFieldName);
	pField->strRole = "Ŀ�����";
	m_tVarInfo.Add(pField);
	for (i=0;i<m_strInputField.GetSize();i++)
	{
		pField = new TFieldRole;
		pField->strFieldName = m_strInputField.GetAt(i);
		pField->nFieldNo = pDataInterface->IndexByName(pField->strFieldName);
		pField->strRole = "�������";
		m_tVarInfo.Add(pField);
	}

	int nCount=0,transCount=0;	
	for (i=0;i<this->m_tVarInfo.GetSize();i++)
	{
		pField = (TFieldRole *)m_tVarInfo.GetAt(i);
		if (pField->strRole == "�������")
		{
			m_arInputArray.Add(pField);
			//m_nCustomCol = pField->nFieldNo;
		}
		else if (pField->strRole == "Ŀ�����")
		{
			m_arTargetArray.Add(pField);
			m_arInputArray.Add(pField);
			//m_nProductCol = pField->nFieldNo;
		}
	}
	CTString strText;
	strText.Format( "���ڽ������ݳ�ʼ������..." );

	//	m_pDataInterface->m_DataAccess.MergeSort(CustomCol);
	grouper.reset();
	grouper.setGrpCount(m_tVarInfo.GetSize());
	nCount=MergeDims(m_pDataInterface,&m_arInputArray,CustomArray,ProductArray,transCount);

		
	if (nCount>0)
	{
		if(!m_bStopWork)
		{
		//////////////////////////////////////////////////////////////////////////
		//�ƺ����Բ���������ΪpCustom�����Ѿ��ź���
		//	prshl(pCustom,nCount,true);
		//////////////////////////////////////////////////////////////////////////
		}				
			
		if(!m_bStopWork)
		{
		//////////////////////////////////////////////////////////////////////////
		//�ƺ����Բ���ȥ�ظ���¼����Ϊ���ȥ�ظ���¼���㷨����Ե�ά
		//����ʶ���ά���ظ���¼
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
		CRsltElementText *pText=new CRsltElementText("����");
		CTString str;
		str=_T("����Ч��¼���޷�����\n");
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
	//int miniSupport = ceil((double)m_dSupport * transCount/100);	
	int miniSupport = ceil((double)m_dSupport * transCount);//С����ʽ����0.7,modify by xds 20060601	
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
 		WritePtrData(m_pDataInterface,&m_arInputArray,parOneFrequentSet,"1��Ƶ����", &m_bStopWork,m_pResult);//,m_pResultWnd);

		//����������2-n�
		bool bFlag = true;
		int k = 0;
		int mark = parOneFrequentSet->GetSize();
		while( bFlag )
		{
			
			if( m_bStopWork )
			{  
				//�����û���ֹ
				break;
			}
			CPtrArray arCk;
			CTString str;
			str.Format("��%d�����%d�,��ǰ��ĳ���Ϊ%d!",k+1,k+2,mark);
			bool bEndFlag;
			if (k==0)
			{
				bEndFlag= Aproiri_gen12(CustomArray,ProductArray,nCount,(CPtrArray *)arLk.GetAt(k),k,mark,&arCk/*,lp*/,&m_bStopWork,miniSupport);
				if( m_bStopWork )
				{  
					//�����û���ֹ
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
					break;
				}
				if( bEndFlag )break;
				InXthenCount(CustomArray,ProductArray,nCount,&arCk);//����������ѭ��
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
 			//WritePtrData(pMsg->pDataInterface,&pMsg->inputindexs,pArray,str,&bStopWork,m_pResult);
			WritePtrData(m_pDataInterface,&m_arInputArray,pArray,str, &m_bStopWork,m_pResult);
		}
	}
	if( !m_bStopWork )
	{
		//���confidence
		CTString str;
		str.Format("�������Ŷ�...");

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
//�����ݰ汾
//������ԭ�����ȱʧֵ�����ֵĴ��󣬾�����Ż�����
int CMultiDimRule::MergeDims(CDataInterface *pDataInterface, CPtrArray* inputs,
							 TDColAry &CustomArray,TDColAry &ProductArray,int &transCount)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
		long nRecord=pAccess->GetRecordCount();
		if (nRecord<=0 )return 0;
		//inputs�б������б�����(������ѡ������������Ŀ�����)�ĸ�������
		int inputCount=0;//���б�����������ά��
		inputCount = inputs->GetSize();
			
		CField **ppProductField=new CField* [inputCount];	
		int *pProductFieldType=new int [inputCount];
		int i;
		for(i=0;i<inputCount;i++)
		{
			TFieldRole *pRol = (TFieldRole *)inputs->GetAt(i);
			//if (pRol->strRole == CTString("�������"))
			//			{
			
				ppProductField[i]=pAccess->m_pFieldList->FieldByIndex(pRol->nFieldNo);
				pProductFieldType[i]=ppProductField[i]->GetFieldType();
			// }
		}
	
		//�˴�����ȷ���ǳ�ȱʧֵ��ʵ����Ҫʹ�õĿռ䣬�Կռ任�ٶ�
		//��Щ�˷ѿռ�Դ�������TDColAryӰ�첻��
		CustomArray.SetSize(nRecord*inputCount);
		ProductArray.SetSize(nRecord*inputCount);
		
		//ȡ��ȱʧֵ���ݣ����ö�Ӧ��ʶ����
		int nRowIndex=1,nCount=0;
		TDataSet tempdata;
		int idindex,flag;
		pAccess->First();
		while(!pAccess->Eof())
		{
			//��ĳ���������Ϊȱʧֵ��������������¼
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
			
			//������Ч����
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
				//��Ʒ�����¼���������ֶεı�ʶ
				ProductArray[nCount].dValue=grouper.getID(idindex);
				ProductArray[nCount].iIndex=nCount+1;
				//�ͻ������¼������
				CustomArray[nCount].dValue =nRowIndex;
				CustomArray[nCount].iIndex=nCount+1;
				
				nCount++;
			}
			nRowIndex++;
			pAccess->Next();	
		}
		transCount=nRowIndex-1;//�õ���Ч��¼��
		
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
			CTString strName;
			pFRole = (TFieldRole *)m_arInputArray.GetAt(grpindex);
			//strName=pDataInterface->GetFieldName((int)inputs->GetAt(grpindex));
			strName=pDataInterface->GetFieldName(pFRole->nFieldNo);
//***************************added by zkc 2004.5.17 zlq �޸�  ********************************								
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

	CTLTable * pTable= new CTLTable();//"ǿ����");
	assert(pResult);
	CTString strTmpRes;
	CTString str;
	TFieldRole *pFRole = NULL;
	pTable->SetTitle("ǿ����");

	pTable->InsertColumn(0,"����");
	pTable->InsertColumn(1,"֧�ֶ�");
	pTable->InsertColumn(2,"���Ŷ�");
	pTable->InsertColumn(3,"n_Ƶ����");

	CDataAccess*	m_pRuleAccess=new CDataAccess();
	CFieldList* pFieldList=NULL;
	if (bSave){
		m_pRuleAccess->NewFile();
		pFieldList = m_pRuleAccess->m_pFieldList;

		CField* pTempField;
		pTempField = pFieldList->CreateField(fString);
		pTempField->SetFieldName("ǰ��");
		pFieldList->Add(pTempField);
		pTempField = pFieldList->CreateField(fString);
		pTempField->SetFieldName("����");
		pFieldList->Add(pTempField);
		pTempField = pFieldList->CreateField(fDouble);
		pTempField->SetFieldName("֧�ֶ�");
		pFieldList->Add(pTempField);
		pTempField = pFieldList->CreateField(fDouble);
		pTempField->SetFieldName("���Ŷ�");
		pFieldList->Add(pTempField);
		pTempField = pFieldList->CreateField(fInt);
		pTempField->SetFieldName("nƵ����");
		pFieldList->Add(pTempField);
	}


	int itemCount=-1;

	for(int i=1;i<par->GetSize();i++)			//��k-Ƶ���,��iѭ��;
	{
		if( pbStop && *pbStop == true )
		{
			//�û���ֹ�˼���
			break;
		}
		CPtrArray * pkar = (CPtrArray *)par->GetAt(i);
		for(int j=0;j<pkar->GetSize() && !(*pbStop);j++)		//��k-Ƶ����еĵ�Ԫ��Ŀ,��jѭ��;
		{
			TUnit * pUnit = (TUnit *)pkar->GetAt(j);

			int nLen = pUnit->arIndex.GetSize();
			assert( i+1 == nLen );				//i����k-Ƶ����еĵ�Ԫ�ĳ���;
			CIntVector vect(nLen);
			bool isExist=false;
			int k;
			for( k=0;k<nLen;k++)				//��k-Ƶ����еĵ�j��Ԫ�����ݸ���vect;
			{
				vect[k] = *(int *)pUnit->arIndex.GetAt(k);
				int grpindex=grouper.getGrpIndex(vect[k]);
				//if (IsTarget(mpMsg->inputindexs[grpindex]))
				pFRole = (TFieldRole*)m_arInputArray.GetAt(grpindex);
				if (IsTarget(pFRole->nFieldNo))
					isExist=true;
			}
			if (isExist){
				for(k=1;k<nLen && !(*pbStop);k++)					//��ȡk-Ƶ����еĵ�j��Ԫ�ĵ�s(s=k)���;
				{
					CIntMatrix matrix_subset = nchoosek(vect,k);
					for(int l=0;l< matrix_subset.mrow() && !(*pbStop);l++)//�Ե�s���Ԫ��ѭ��,��ȡ���Ŷ�;
					{
						CIntVector vect_subset = matrix_subset.row(l);
						int    count = GetSubSetCount((CPtrArray *)par->GetAt(k-1),vect_subset);
						double confidence = pUnit->count / (count - 0.0);
						//if( confidence > mini_confidence/100 ) modified by liytao 20030225
						if( confidence >= m_dConfidence )//С����ʽ����0.7,modify by xds 20060601
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

								
//***************************added by zkc 2004.5.17 zlq �޸�  ********************************								
								CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(grpindex);
								CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq
								str.Format("\"%s\"",strNew.GetData());
//**************************end of addition by zkc 2004.5.17*******************************


								str=strName+"="+str;
								strTmpRes += str;
								if( m != vect_subset.vlen()-1 )
								{			
									strTmpRes += _T("����");
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
//***************************added by zkc 2004.5.17 zlq �޸�  ********************************								
								CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(grpindex);
								CTString strNew = CCommon::GetValueLabel(pField,dataType,ptempdata,m_pDataInterface);//#_S 2004-9-17 $ 10 zlq

								str.Format("\"%s\"",strNew.GetData());
//**************************end of addition by zkc 2004.5.17*******************************
								str=strName+"="+str;
								strPost += str;
								if( m != vecttemp.vlen()-1 )
								{			
									strPost += _T("����");
								}
							}			
							strTmpRes+=strPost;
							double support=pUnit->count/(double)transCount;
							//str.Format("       ���Ŷ� = %10.2f\n",confidence);
							
							if( isExist)
							{
								pTable->InsertItem(itemCount+1,strTmpRes);
								pTable->SetItemText(itemCount+1,1,support);
								pTable->SetItemText(itemCount+1,2,confidence);
								pTable->SetItemText(itemCount+1,3,nLen);

								if (bSave)
								{
									m_pRuleAccess->Append();
									pFieldList->FieldByName("ǰ��")->SetAsString(strPre);
									pFieldList->FieldByName("����")->SetAsString(strPost);
									pFieldList->FieldByName("֧�ֶ�")->SetAsDouble(support);
									pFieldList->FieldByName("���Ŷ�")->SetAsDouble(confidence);
									pFieldList->FieldByName("nƵ����")->SetAsInteger(nLen);

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
		CTString out=_T("����");
//		m_pRuleAccess->SaveFile(pMsg->strRuleFileName,&out);
		m_pRuleAccess->Close();
		delete m_pRuleAccess;
		m_pRuleAccess=NULL;
	}
	if( pTable->GetTableRowS()<2 )
	{
		CRsltElementText* pText=new CRsltElementText("ǿ����");
		pText->AddString ("���μ���û���ҵ�����Ҫ���ǿ����!���������ò�������!");
		pResult->Add(pText);
		delete pTable;
		pTable=NULL;
	}
	else{
		pResult->Add(new CRsltElementTable("ǿ����",pTable));
	}

}
