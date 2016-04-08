// BPNet.cpp: implementation of the CBPNet class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BPNet.h"
#include "BP.h"
#include <assert.h>
#include "../Public/FileReadWrite.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"

CBPNet::CBPNet()
{

}


CBPNet::~CBPNet()
{

}

bool CBPNet::SetPredVO(std::string strVO)
{
//	BPPRED
//		/DATA=[FILE]
//		/VARINPUT [varlist]
//		/MODELFROMFILE [FILE]
	AnaWord VOstr;
	VOstr.Import(strVO);
	
	m_ModelFile = AnaSyntax::getSingleStr("/MODELFROMFILE", VOstr);
	if (strlen(m_ModelFile) < 1)
		return false;
	if (!Import(m_ModelFile.GetData()))
		return false;
	////////////////////////////////////////////////////
	//���������ֶ�
	m_arInputField.RemoveAll();
	int i=0, nSize=m_InAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		FieldDesc *pField = m_InAry.GetAt(i);
		m_arInputField.Add(pField->strName);
	}
	//����ֶ�
	nSize = m_OutAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		FieldDesc *pField = m_OutAry.GetAt(i);
		m_arDestField.Add(pField->strName);
	}
	/////////////////////////////////////////////////////
	CTStringArray szArrInput,szArrOutput,szArrReplace;
	AnaSyntax::getVAR("/VARINPUT", VOstr, szArrInput);
	AnaSyntax::getVAR("/REPLACEVARNAME", VOstr, szArrReplace);
	if (szArrReplace.GetSize()%2 != 0)
		return false;
	if (!AnaSyntax::Replace(m_arInputField, szArrInput, szArrReplace))
		return false;
	//Ŀ�����-���?
	if (AnaSyntax::exist("/PREDNAME", VOstr))
	{
		AnaSyntax::getVAR("/PREDNAME", VOstr, szArrOutput);
		if (m_arDestField.GetSize() == szArrOutput.GetSize())
		{
			m_arDestField.RemoveAll();
			m_arDestField.Copy(szArrOutput);
		}
	}
	else
	{
		for (int i=0; i<m_arDestField.GetSize(); i++)
		{
			CTString szTemp = m_arDestField.GetAt(i);
			szTemp += "_Ԥ��ֵ";
			m_arDestField.SetAt(i,szTemp);
		}
	}
	////////////////////////////////////////////////////
	//���������ֶ�
	m_arInputField.RemoveAll();
	nSize=m_InAry.GetSize();
	if (nSize == m_arInputField.GetSize())
	{
		for (i=0; i<nSize; i++)
		{
			FieldDesc *pField = m_InAry.GetAt(i);
			pField->strName = m_arInputField.GetAt(i);
		}
	}
	//����ֶ�
	nSize = m_OutAry.GetSize();
	if (nSize == m_arDestField.GetSize())
	{
		for (i=0; i<nSize; i++)
		{
			FieldDesc *pField = m_OutAry.GetAt(i);
			pField->strName = m_arDestField.GetAt(i);
		}
	}
	/////////////////////////////////////////////////////
	return true;
}

void	CBPNet::SetTrainVO(std::string strVO)
{
	//BP������VO����
	//	BPNETTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD [{DELRECORD**},{AVEFILL}]
	//	/SYSPREC = PRECISION(0.001**)
	//	/TRAINTIME = TIME(100**)
	//	/LEARNRATE = LEARNED(0.1**)
	//	/HIDENCOUNT = HIDEN(4**)
	//	/MAXRUNTIME = [{10MIN},{1HUR},{2HUR},{7HUR},{1DAY},{2DAY},{7DAY},{INF**}]
	//	/OUTTYPE [{CALCULATE**},{CLASS}]
	//	/SAVEMODEL [FILE]
	
	AnaWord VOstr;
	VOstr.Import(strVO);

	if (AnaSyntax::findstr("/SYSPREC","PRECISION",VOstr))
		m_TrainItem.dPrecision = AnaSyntax::getvalue("/SYSPREC","PRECISION",VOstr);
	if (AnaSyntax::findstr("/LEARNRATE","LEARNED",VOstr))
		m_TrainItem.dStep = AnaSyntax::getvalue("/LEARNRATE","LEARNED",VOstr);
	if (AnaSyntax::findstr("/HIDENCOUNT","HIDEN",VOstr))
		m_TrainItem.nHideNum = AnaSyntax::getvalue("/HIDENCOUNT","HIDEN",VOstr);

	m_TrainItem.m_bCalc = AnaSyntax::findstr("/OUTTYPE","CALCULATE",VOstr);

	if (AnaSyntax::findstr("/TRAINTIME","TIME",VOstr))
		m_TrainItem.nTrainTimes = AnaSyntax::getvalue("/TRAINTIME","TIME",VOstr);
	
	if (AnaSyntax::findstr("/MAXRUNTIME","10MIN",VOstr))
		m_TrainItem.dMaxRunTime = 10 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","1HUR",VOstr))
		m_TrainItem.dMaxRunTime = 1 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","2HUR",VOstr))
		m_TrainItem.dMaxRunTime = 2 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","7HUR",VOstr))
		m_TrainItem.dMaxRunTime = 7 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","1DAY",VOstr))
		m_TrainItem.dMaxRunTime = 1 * 24 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","2DAY",VOstr))
		m_TrainItem.dMaxRunTime = 2 * 24 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","7DAY",VOstr))
		m_TrainItem.dMaxRunTime = 7 * 24 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","INF",VOstr))
		m_TrainItem.dMaxRunTime = 0;
	
	AnaSyntax::getVAR("/VARINPUT", VOstr, m_arInputField);
	AnaSyntax::getVAR("/VARDEST", VOstr, m_arDestField);

	m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL",VOstr);
}

CResult * CBPNet::RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, std::string strVO)
{
	m_pDataInterface = pDataInterface;
	m_pEnvParam = pEnv;
	m_pInteraction = itt;
	SetTrainVO(strVO);

	assert( m_pDataInterface!=NULL );
	CResult *pResult = new CResult("���������");

	if (TrainCheck() ==false)
		return NULL;

	if (m_TrainItem.m_bCalc)
	{//����ģʽ  ���������ͼ��
		DataMap(&m_InAry,&m_OutAry,m_mxSour,true,false);//��һ�������� �� �����
	}
	else
	{//����ģʽ  �����׼ȷ�ʵ�
		DataMap(&m_InAry,NULL,m_mxSour,true,false);//��һ�������룩
	}
	
	Train(pResult);

	//////////////////////////////////////////////////
	// �����������
	
	assert( pResult!=NULL );
	//////////////////////////////////////////////////
	// ��̬������������
	if( m_pInteraction->bAbort || pResult  && pResult ->Count()==0 )
	{
		delete pResult;
		pResult  = NULL;
	}

	return pResult;
}

CResult * CBPNet::RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,std::string strVO, std::string strModel)
{
	CResult *pResult = NULL;

	m_pDataInterface = pDataInterface;
	assert( m_pDataInterface!=NULL );
	m_pEnvParam = pEnv;
	m_pInteraction = itt;

	//��ȡģ�Ͳ���
	if (!SetPredVO(strVO))
		return pResult;

	pResult = new CResult("���������");

	if (SimulateCheck() ==false)
		return NULL;

	DataMap(&m_InAry,NULL,m_mxSour,true,true);//��һ�������룩�Ѿ��з�Χ

	Simulate(pResult);
	//////////////////////////////////////////////////
	// �����������
	
	assert( pResult!=NULL );
	//////////////////////////////////////////////////
	// ��̬������������
	if( m_pInteraction->bAbort || pResult  && pResult ->Count()==0 )
	{
		delete pResult ;
		pResult  = NULL;
	}
	return pResult ;
}

void CBPNet::Abort()
{
	m_Bp.Abort();
}

class MArray
{
public:
	double *pArry;
	MArray()
	{
		pArry = NULL;
	}

	~MArray()
	{
		if (pArry)
			delete []pArry;
	}
};
bool CBPNet::Train(CResult *pResult)
{
	int nOutCount = m_OutAry.GetSize();//��������
	MArray OldClassOutAry ;//ԭ�е������,���ڷ���ģʽ
	if (m_TrainItem.m_bCalc)
	{//����ģʽ  ���������ͼ��
	}
	else
	{//����ģʽ  �����׼ȷ�ʵ�
		int nRow = m_mxSour.GetMatrixRowNumber();
		
		int nMaxLen = 256;
		double *pSortAry = new double [nMaxLen+1];
		int nDistinct = m_mxSour.GetDistinct(m_mxSour.GetMatrixColNumber() -1,
			pSortAry,nMaxLen);
		if (nDistinct==0)
			return false;
		else if (nDistinct==-1)
		{
			CTString str;str.Format("����������ܴ���%d����",nMaxLen);
			return false;
		}

		//�������
		int nOldCol = m_mxSour.GetMatrixColNumber();
		OldClassOutAry.pArry = m_mxSour.GetColArray(nOldCol-1);
		m_mxSour.ReSize(m_mxSour.GetMatrixRowNumber(),nOldCol + nDistinct-1,false);
		for (int iRow=0;iRow<nRow ;iRow++)
		{
			double value = m_mxSour.m_pTMatrix[iRow][nOldCol-1] ;
			m_mxSour.m_pTMatrix[iRow][nOldCol-1]  = 0;
			int nNewPos ;
			if(CBPMatrix::FindPos(pSortAry,nDistinct,value,nNewPos)==false)
			{
				assert(0);
				return false;
			}
			m_mxSour.m_pTMatrix [iRow][nOldCol-1+nNewPos]  = 1;
		}
		//m_mxSour.Print();
		//m_mxSour.SaveDataToFile("C:\\a.txt");

		//�����m_TrainItem��
		m_TrainItem.pSortAry = pSortAry;
		m_TrainItem.nClassCount = nDistinct;
		nOutCount = nDistinct;//��������
		//delete []pSortAry;
	}


	m_Bp.Reset();
	m_Bp.SetWnd();
	//*****************��ֵ��ʼ********************************
	m_Bp.m_nInputLayerNumber = m_InAry.GetSize();//�������Ŀ:
	m_Bp.m_nHideLayerNumber = m_TrainItem.nHideNum;//��������Ŀ:
	m_Bp.m_nOutputLayerNumber = nOutCount ;//�������Ŀ:

	///////////////////////////
	m_Bp.m_nComboArithmetic = 1;//����ѵ�����㷨 0:LM  1:BP
	
	//m_pBp->m_bSimulateDataFlag = (m_TrainItem.m_nType==3);// ������ѵ��(0)�������ݷ���(1)
	m_Bp.m_nComboFunc = 0;//����ѵ���ļ���� 0:Sigmoid 1:tanh 2:Tansig
	m_Bp.m_nSystemErrorLevel = m_TrainItem.dPrecision;//ϵͳ����:

	m_Bp.m_nMaxTrainTimes = m_TrainItem.nTrainTimes;//���ѵ������:
	m_Bp.m_dStep = m_TrainItem.dStep;//����

	//bp.m_nSystemError = m_nSystemError;//ϵͳ�����:(���)
	//bp.m_nTrainTimes = m_nTrainTimes;//ѵ������:(���)

//	CTString strProgressText = _T("��ʼ������...");
//	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );

	if (m_Bp.TrainingDemo(m_mxSour)==false)
	{//ѵ��ʧ��
// 		AfxMessageBox("ѵ��ʧ��!", MB_ICONEXCLAMATION );
		return false;
	}
	
	if (pResult)
	{//���ϵͳѵ������
		CRsltElementText*pRsltElementText=new CRsltElementText("ϵͳѵ������");
		CTString str;str.Format("ϵͳѵ��ʵ�ʾ���%f",m_Bp.m_SystemErrorOld);
		pRsltElementText->AddString(str);
		pResult->Add(pRsltElementText);
	}

	if (m_TrainItem.m_bCalc)
	{//����ģʽ  ���������ͼ��
		DataMap(NULL,&m_OutAry,m_Bp.m_mxTrainOut,false,0);//����һ��
		DataMap(NULL,&m_OutAry,m_mxSour,false,0);//����һ��
		for(int nCol=0; nCol< m_Bp.m_mxTrainOut.GetMatrixColNumber(); nCol++)
		{//�Զ��Ŀ��������б���
			double *pdPredict = m_Bp.m_mxTrainOut.GetColArray(nCol);//Ԥ��ֵ
			double *pdActual =  m_mxSour.GetColArray(m_InAry.GetSize()+nCol);//ʵ��ֵ

			CreateOut(pResult,pdActual,pdPredict,m_Bp.m_mxTrainOut.GetMatrixRowNumber(),
				"����("+m_OutAry.GetAt(nCol)->strName+")ѵ��");
			delete []pdPredict;
			delete []pdActual;
		}		
	}
	else
	{//����ģʽ  �����׼ȷ�ʵ�
		//===uc  ���跴��һ��
		//m_pBp->m_mxTrainOut;
		MArray newOut;//ѵ�����
		newOut.pArry = new double [m_Bp.m_mxTrainOut.GetMatrixRowNumber()];
		for (int i=0;i< m_Bp.m_mxTrainOut.GetMatrixRowNumber() ;i++)
		{
			int nIndex = m_Bp.m_mxTrainOut.GetMaxValueCol(i);//�õ���ֵ�����кţ����������з���õ���Ӧ��ԭ��ֵ
			newOut.pArry[i] = m_TrainItem.pSortAry[nIndex];//��ֵ
			//OldClassOutAry.pArry[i];//��ֵ
		}
		//m_pBp->m_mxTrainOut.Print();
		//m_pBp->m_mxTrainOut.SaveDataToFile("C:\\b.txt");
		CreateOut(pResult , OldClassOutAry.pArry , newOut.pArry, m_Bp.m_mxTrainOut.GetMatrixRowNumber(),
				"����("+m_OutAry.GetAt(0)->strName+")ѵ��",true);		

	}



	
	return true;
}

/*
*���ݷ���
*pSimulate��ԭԤ��������ںͽ���Ƚ� ��Ϊ�գ�����ѵ����Ԥ��ģʽ��
*pResult�������
*/
void CBPNet::Simulate(CResult *pResult,CBPMatrix*pSimulate)
{

/*
	m_Bp.SetWnd(m_pProgressWnd->m_hWnd);
	CTString strProgressText = _T("��ʼԤ������...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

	CBPMatrix mxResult;
	if (m_Bp.SimulateData(m_mxSour,mxResult)==false)
		return ;

	if (m_TrainItem.m_bCalc)
	{//1.����ģʽ  ���������ͼ��
		DataMap(NULL,&m_OutAry,mxResult,false,0);//����һ��
		
		if (pSimulate==NULL)//ԭԤ��������ںͽ���Ƚ� 
		{//1.1�����Ԥ����
			CTLTable* pTableMin=new CTLTable;
			pTableMin->SetCols( mxResult.GetMatrixColNumber() );
			pTableMin->SetRows( mxResult.GetMatrixRowNumber() +1 );
			
			pTableMin->SetTitle("Ԥ����");
			
			pTableMin->InsertColumn(0, "");
			for(int nCol=0; nCol<mxResult.GetMatrixColNumber(); nCol++)
				pTableMin->InsertColumn( nCol+1, m_OutAry.GetAt(nCol)->strName);
			
			for(int nRow=0; nRow< mxResult.GetMatrixRowNumber(); nRow++ )
			{
				CTString string;string.Format( "�к�:%02d", nRow+1 );
				pTableMin->InsertItem( nRow, string , true );
				
				for(int nCol=0; nCol<mxResult.GetMatrixColNumber(); nCol++ )
					pTableMin->SetItemText( nRow, nCol+1, mxResult.m_pTMatrix[nRow][nCol]);
			}
			
			CRsltElementTable *pVariableMin = new CRsltElementTable( "Ԥ���", pTableMin );
			pResult->Add(pVariableMin);
		}
		else//��ѵ����Ԥ��ģʽ
		{//1.2���ԭ��ֵ �� Ԥ����
			//����һ������ start
			DataMap(NULL,&m_OutAry,*pSimulate,false,0);
			//����һ������ end
			
			for(int nCol=0; nCol<mxResult.GetMatrixColNumber(); nCol++)
			{//1.2.1�Զ��Ŀ��������б���
				double *pdPredict = mxResult.GetColArray(nCol);//Ԥ��ֵ
				double *pdActual =  pSimulate->GetColArray(m_InAry.GetSize()+nCol);//ʵ��ֵ
				
				CreateOut(pResult,pdActual,pdPredict,mxResult.GetMatrixRowNumber(),
					"����("+m_OutAry.GetAt(nCol)->strName+")Ԥ��");
				delete []pdPredict;
				delete []pdActual;
				
				
			}
			
		}
	}
	else
	{//2.����ģʽ  �����׼ȷ�ʵ�
		//m_pBp->m_mxTrainOut;
		MArray newOut;//ѵ�����
		newOut.pArry = new double [mxResult.GetMatrixRowNumber()];
		for (int i=0;i<mxResult.GetMatrixRowNumber() ;i++)
		{
			int nIndex = mxResult.GetMaxValueCol(i);//�õ���ֵ�����кţ����������з���õ���Ӧ��ԭ��ֵ
			newOut.pArry[i] = m_TrainItem.pSortAry[nIndex];//��ֵ
			//OldClassOutAry.pArry[i];//��ֵ
		}

		if (pSimulate==NULL)//ԭԤ��������ںͽ���Ƚ� 
		{//2.1�����Ԥ����
			CTLTable* pTableMin=new CTLTable;
			pTableMin->SetCols( 1 );
			pTableMin->SetRows( mxResult.GetMatrixRowNumber() +1 );
			
			pTableMin->SetTitle("Ԥ����");
			
			pTableMin->InsertColumn(0, "");
			int aa = m_OutAry.GetSize();//===temp
			pTableMin->InsertColumn( 1, m_OutAry.GetAt(0)->strName);
			
			for(int nRow=0; nRow< mxResult.GetMatrixRowNumber(); nRow++ )
			{//2.1.1
				CTString string;string.Format( "�к�:%02d", nRow+1 );
				pTableMin->InsertItem( nRow, string , true );
				
				pTableMin->SetItemText( nRow, 0+1, newOut.pArry[nRow]);
			}
			
			CRsltElementTable *pVariableMin = new CRsltElementTable( "Ԥ���", pTableMin );
			pResult->Add(pVariableMin);
		}
		else//��ѵ����Ԥ��ģʽ
		{//2.2���ԭ��ֵ �� Ԥ����	
			double *pdPredict = newOut.pArry;//Ԥ��ֵ
			double *pdActual =  pSimulate->GetColArray(m_InAry.GetSize()+0);//ʵ��ֵ
			
			CreateOut(pResult,pdActual,pdPredict,mxResult.GetMatrixRowNumber(),
				"����("+m_OutAry.GetAt(0)->strName+")Ԥ��",true);
			//delete []pdPredict;
			delete []pdActual;
	
		}
	}
}

bool CBPNet::TrainCheck()
{
	int i;
	m_InAry.RemoveAll();
	m_OutAry.RemoveAll();
	m_NetWork.Reset();

	int nFieldCount = m_arInputField.GetSize();
	CFieldType cType;
	for(i=0; i<nFieldCount; i++ )
	{
 		CTString strField = m_arInputField.GetAt(i);
        CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strField); 
		cType = pField->GetFieldType();
		if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
			return false;
		m_InAry.Add(pField->GetFieldName(),cType);
	}
	nFieldCount = m_arDestField.GetSize();
	for(i=0; i<nFieldCount; i++ )
	{
		CTString strField = m_arDestField.GetAt(i);
        CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strField); 
		cType = pField->GetFieldType();
		if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
			return false;
		m_OutAry.Add(pField->GetFieldName(),cType);
	}
	
	if (m_InAry.GetSize()==0)
	{
		return false;
	}
	
	CTStringArray sourInpu;
	if (m_OutAry.GetSize()==0)
	{
		return NULL;
	}
	if (m_OutAry.GetSize()!=1)
	{
		return NULL;
	}
	for (i=0;i<m_InAry.GetSize();i++)
		sourInpu.Add(m_InAry.GetAt(i)->strName);
	for (i=0;i<m_OutAry.GetSize();i++)
		sourInpu.Add(m_OutAry.GetAt(i)->strName);

	//***********  ������������  start ***********
	int nSampleSize = m_pDataInterface->m_DataAccess.GetRecordCount();
	if (nSampleSize ==0)
		return false;

	m_mxSour.ReSize(nSampleSize,sourInpu.GetSize());//�����趨�����С
	CDataAccess *pDA = &m_pDataInterface->m_DataAccess;
	pDA->First();
	int nCurRecNo = 0;
	while (!pDA->Eof())
	{
		for (int i=0;i<sourInpu.GetSize();i++)
		{
			CField *pField = pDA->m_pFieldList->FieldByName(sourInpu.GetAt(i));
			if (pField->IsNull())
			{
				nCurRecNo--;
				break;
			}
			CFieldType ft = pField->GetFieldType();
			m_mxSour.m_pTMatrix[nCurRecNo][i] = pField->GetAsDouble();
		}
		pDA->Next();
		nCurRecNo++;
	}
	m_mxSour.ReSize(nCurRecNo,sourInpu.GetSize(),false);

	//***********  ������������  end   ***********
	return true;
}

bool CBPNet::SimulateCheck(bool bLoadData)
{
	int nFieldCount = m_pDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
	for (int i=0;i<m_InAry.GetSize() ; i++)
	{
		CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(m_InAry.GetAt(i)->strName);
		if (pField == NULL)
		{
// 			AfxMessageBox("�������ֶ�����'"+ m_InAry.GetAt(i)->strName+"' ��������Դ��û���ҵ�!", MB_ICONEXCLAMATION );
			return false;
		}
	}
	if (!bLoadData)//����Ҫװ�����ݣ��򷵻�
		return true;
	//***********  ������������  start ***********
	int nSampleSize = m_pDataInterface->m_DataAccess.GetRecordCount();
	if (nSampleSize ==0)
	{
// 		AfxMessageBox("��������Ϊ�գ�");
		return false;
	}
	
	m_mxSour.ReSize(nSampleSize,m_InAry.GetSize());//�����趨�����С
	
	CDataAccess *pDA = &m_pDataInterface->m_DataAccess;
	pDA->First();
	int nCurRecNo = 0;
	while (!pDA->Eof())
	{
		for (int i=0;i<m_InAry.GetSize();i++)
		{
			CField *pField = pDA->m_pFieldList->FieldByName(m_InAry.GetAt(i)->strName);
			
			if (pField->IsNull())
			{
// 				AfxMessageBox("ָ�������к��п�ֵ�����Ƚ�������Ԥ����");
				return false;
			}
			

			m_mxSour.m_pTMatrix[nCurRecNo][i] = pField->GetAsDouble();
		}
		pDA->Next();
		nCurRecNo++;
	}
	//***********  ������������  end   ***********
	return true;
}

/*
*����Ƿ��Ѿ���ѵ���õ�����ṹ
*/
bool CBPNet::HasNetWork()
{
	return m_NetWork.GetLength()>0 ? true : false;
}

/*
*����ÿһ��Ԫ�ص�һ���ַ���ʾ���ڲ�ι�ϵ�����㿪ʼ
* //#_S 2005-1-7 zlq
*/
bool CBPNet::GetNetMsg(CTStringArray& msg)
{
	if (HasNetWork())
	{//m_OutAry
		m_NetWork.Seek(0);//��λλ��
		Serialize(false);
		msg.Add("0�������");
		int i;
		for (i=0;i<m_InAry.GetSize();i++)
		{
			msg.Add("1"+m_InAry.GetAt(i)->strName);
		}
		msg.Add("0�������");
		for ( i=0;i<m_OutAry.GetSize();i++)
		{
			msg.Add("1"+m_OutAry.GetAt(i)->strName);
		}
		CTString strTemp;
		strTemp.Format("0ѵ��������%d",m_TrainItem.nTrainTimes);
		msg.Add(strTemp);
		strTemp.Format("0ϵͳ�趨���ȣ�%f",m_TrainItem.dPrecision);
		msg.Add(strTemp);
		strTemp.Format("0����ѵ��ʵ�ʾ��ȣ�%f",m_Bp.m_SystemErrorOld);
		msg.Add(strTemp);
		strTemp.Format("0ѧϰ�ʣ�%f",m_TrainItem.dStep);
		msg.Add(strTemp);
		strTemp.Format("0��������Ԫ��Ŀ��%d",m_TrainItem.nHideNum);
		msg.Add(strTemp);
		msg.Add("0����ѵ��ģʽ");
		if (m_TrainItem.m_bCalc == 0 )
		{
			strTemp.Format("1����ģʽ(����:%d)",m_TrainItem.nClassCount);
			msg.Add(strTemp);
		}
		else
		{
			strTemp.Format("1����ģʽ");
			msg.Add(strTemp);
		}
		return true;
	}
	msg.Add("0û������");
	return false;
}

bool CBPNet::Serialize(bool IsStoring)
{
	if (IsStoring)
	{//����
		m_NetWork.Reset();//��λ�ڴ���
		m_NetWork.WriteString("CBPNet IS BEGIN");
		m_NetWork.Write(&m_TrainItem,sizeof(TrainItem));
		if (m_TrainItem.pSortAry)
		{
			m_NetWork.Write(m_TrainItem.pSortAry,sizeof(double)*m_TrainItem.nClassCount);
		}
		
		m_InAry.Serialize(&m_NetWork,IsStoring);
		m_OutAry.Serialize(&m_NetWork,IsStoring);
		m_Bp.Serialize(&m_NetWork,IsStoring);
		m_NetWork.WriteString("CBPNet IS End");
	}
	else
	{//װ��
		m_NetWork.Seek(0);//��λλ��
		//���ԭ���ֶ���Ϣ
		m_InAry.RemoveAll();
		m_OutAry.RemoveAll();
		CTString strID ;
		m_NetWork.ReadString(strID);
		if (strID !="CBPNet IS BEGIN")
		{
			//AfxMessageBox("���л�����!");
			return false;
		}
		m_TrainItem.Clear();
		m_NetWork.Read(&m_TrainItem,sizeof(TrainItem));
		if (m_TrainItem.pSortAry)
		{
			m_TrainItem.pSortAry = new double[m_TrainItem.nClassCount];
			m_NetWork.Read(m_TrainItem.pSortAry,sizeof(double)*m_TrainItem.nClassCount);
		}
		
		m_InAry.Serialize(&m_NetWork,IsStoring);
		m_OutAry.Serialize(&m_NetWork,IsStoring);
		m_Bp.Serialize(&m_NetWork,IsStoring);
		
		m_NetWork.ReadString(strID);
		if (strID !="CBPNet IS End")
		{
			//AfxMessageBox("���л�����!");
			return false;
		}
	}
	return true;
}

bool CBPNet::SaveNetWork(CTString strFileName)
{
	Serialize(true);
	return m_NetWork.SaveFile(strFileName);
}

bool CBPNet::LoadNetWork(CTString strFileName)
{
	bool rst = m_NetWork.LoadFile(strFileName);
	if (rst == false)
		return false;
	if (Serialize(false)==false)//�ȶ��ֶ�����Ϣ���ٶ�����ṹ
	{
		m_NetWork.Reset();
		return false;
	}
	return rst;
}
/*
 *	ʵ�����ݵĹ�һ�����ͻ�ԭ
 *  pInFeildList ��ǰ����һһ��Ӧ
 *  pOutFeildList �Ӻ�ǰһһ��Ӧ�������������ΪNULL������Ӧ����mx�����ܺ���ͬ
 *  mx ����
 *  bIsMapToOne Ϊtrue��Ϊ��һ����false ����һ��
 *  bHasRange   �ڹ�һ��ǰ���£��Ƿ�Ϊ�Ѿ��������޷�Χ����ѵ��ģʽʱ������Ҫ���µõ���Χ��
                ����һ��ʱ��Ч
 */
bool CBPNet::DataMap(FieldDescList *pInFeildList, FieldDescList *pOutFeildList, CBPMatrix &mx, bool bIsMapToOne,bool bHasRange)
{
	if (pInFeildList && pOutFeildList)
	{
		if (pInFeildList->GetSize()+pOutFeildList->GetSize() != 
			mx.GetMatrixColNumber())
		{
			assert(0);
			return false;//�����ܺ�
		}
	}
	
	if (pInFeildList)
	{//���������
		CDWordArray FeildIndexAry;
		for (int i=0;i<pInFeildList->GetSize();i++)
			FeildIndexAry.Add(i); 
		DataMapS(*pInFeildList,mx,bIsMapToOne,bHasRange,&FeildIndexAry,&FeildIndexAry);//��һ�������룩
	}
	
	if (pOutFeildList)
	{//���������
		int nStart = mx.GetMatrixColNumber() - pOutFeildList->GetSize();
		CDWordArray MxIndexAry;CDWordArray FeildIndexAry;
		for (int i=0;i<pOutFeildList->GetSize();i++)
		{
			FeildIndexAry.Add(i);
			MxIndexAry.Add(nStart+i);
		}
		DataMapS(*pOutFeildList,mx,bIsMapToOne,bHasRange,&FeildIndexAry,&MxIndexAry);//��һ�������룩
	}

	return true;
}

/*
 *	ʵ�����ݵĹ�һ�����ͻ�ԭ
 *  pFieldIndex��pmxIndex ��Ϊ��ʱ�����е��ж�����������ŶԽ�
 *  ��Ϊ��ʱ������Ӧ����ͬ��С
 *  pFieldIndex �ֶζ�Ӧ��������
 *  pmxIndex �����Ӧ��������
 *  bIsMapToOne Ϊtrue��Ϊ��һ����false ����һ��
 *  bHasRange   �ڹ�һ��ǰ���£��Ƿ�Ϊ�Ѿ��������޷�Χ����ѵ��ģʽʱ������Ҫ���µõ���Χ��
                ����һ��ʱ��Ч
 */
bool CBPNet::DataMapS(FieldDescList &FeildList, CBPMatrix &mx, bool bIsMapToOne,bool bHasRange,
		 CDWordArray *pFieldIndex /*= NULL*/, CDWordArray *pMxIndex/*= NULL*/)
{
	CDWordArray tempAry;
	//���
	if (pFieldIndex || pMxIndex)
	{
		if (pFieldIndex && pMxIndex)
		{
			if (pFieldIndex->GetSize() != pMxIndex->GetSize())
			{
				assert(0);//������������С��һ�£�
				//AfxMessageBox("������������С��һ�£�");
				return false;
			}
		}
		else
		{
			assert(0);//��������Ӧ��ͬʱ��
			//AfxMessageBox();
			return false;
		}
	}
	else
	{//���Ϊ������һ��
		pFieldIndex = & tempAry;
		pMxIndex    = & tempAry;
		for (int i=0; i<mx.GetMatrixColNumber();i++)
			tempAry.Add(i);
		assert(mx.GetMatrixColNumber() == FeildList.GetSize());
	}


	int nColNum = pFieldIndex->GetSize();//mx.GetMatrixColNumber();
	int nRowNum = mx.GetMatrixRowNumber();

	if (bIsMapToOne)
	{//��һ��
		for (int i=0;i<nColNum;i++)
		{//��ÿһ���ֶν��й�һ��
			//************ �ҵ����ֵ����Сֵ **************
			int nMxCol =  pMxIndex->GetAt(i);//�Ӿ����еõ���
			
			double dMax,dMin;
			if (bHasRange)
			{//�ֶ��������Ѿ�����������Ϣ
				int nFieldCol =  pFieldIndex->GetAt(i);//���ֶ��еõ���
				FieldDesc *pDesc = FeildList.GetAt(nFieldCol);
				if (pDesc ==NULL)
				{
// 					AfxMessageBox("��һ��ʱ�ֶ�û���ҵ�!");
					return false;
				}
				dMax = pDesc->dMax;
				dMin = pDesc->dMin;
			}
			else
			{//�ֶ������� û����������Ϣ
				dMax = dMin = mx.m_pTMatrix[0][nMxCol];
				for (int nRow=0; nRow < nRowNum; nRow++)
				{
					double value = mx.m_pTMatrix[nRow][nMxCol];
					if ( value>dMax)
						dMax = value;
					if (value<dMin)
						dMin = value;
				}
				int nFieldCol =  pFieldIndex->GetAt(i);//���ֶ��еõ���
				if(!FeildList.SetAt(nFieldCol,dMin,dMax))
				{
// 					AfxMessageBox("��һ��ʱ�����쳣!");
					return false;
				}
			}
		
			//************ ��ʼת������ **************
			double dInterval = dMax-dMin;//�õ����
			if (fabs(dInterval)<1E-30 )
			{//����Ϊ�����,��ȫ�����
				dInterval = 1;//��ֹ����
			}
			for (int nRow=0; nRow < nRowNum; nRow++)
			{
				double value = mx.m_pTMatrix[nRow][nMxCol];
				value = (value-dMin)/dInterval;
				//���Ʒ�ֵ��������������쳣ʱ
				if (value >1)
					value = 1;
				if (value < 0 )
					value = 0;
				mx.m_pTMatrix[nRow][nMxCol] = value;
			}
		}

	}
	else
	{//����һ��
		for (int i=0;i<nColNum;i++)
		{//��ÿһ���ֶν��з���һ��
			//************ �õ����ֵ����Сֵ **************
			
			int nFieldCol =  pFieldIndex->GetAt(i);//���ֶ��еõ���
			double dMax,dMin;

			FieldDesc *p = FeildList.GetAt(nFieldCol);
			if(!p )
			{
// 				AfxMessageBox("����һ��ʱ�����쳣!");
				return false;
			}
			dMax = p->dMax;
			dMin = p->dMin;
			//************ ��ʼת������ **************
			int nMxCol =  pMxIndex->GetAt(i);//�Ӿ����еõ���

			double dInterval = dMax-dMin;//�õ����
			for (int nRow=0; nRow < nRowNum; nRow++)
			{
				double value = mx.m_pTMatrix[nRow][nMxCol];
				mx.m_pTMatrix[nRow][nMxCol] = value*dInterval+ dMin;			
			}
		}
	}
	return true;
}

void FieldDescList::Serialize(CMemoStream *pStream, bool IsStoring)
{
	if (IsStoring)
	{
		pStream->WriteString("Field IS BEGIN");
		int nSize = GetSize();
		pStream->WriteInt(nSize);
		for (int i=0;i<GetSize(); i++)
		{
			FieldDesc *pDesc = GetAt(i);
			pStream->WriteString(pDesc->strName);
			pStream->Write(&pDesc->nType,sizeof(pDesc->nType));
			pStream->WriteDouble(pDesc->dMax);
			pStream->WriteDouble(pDesc->dMin);
			
		}
		pStream->WriteString("Field IS END");
	}
	else
	{
		CTString strTemp;
		pStream->ReadString(strTemp);//"IS BEGIN"
//		if (strTemp !="Field IS BEGIN")
//			AfxMessageBox("FieldDescList Begin���л�����");
		int nSize = 0;
		pStream->ReadInt(nSize);
		for (int i=0;i<nSize; i++)
		{
			FieldDesc *pDesc = new FieldDesc();
			pStream->ReadString(pDesc->strName);
			pStream->Read(&pDesc->nType,sizeof(pDesc->nType));
			pStream->ReadDouble(pDesc->dMax);
			pStream->ReadDouble(pDesc->dMin);

			m_Array.push_back(pDesc);
			
		}
		pStream->ReadString(strTemp);
	}
}
FieldDescList::~FieldDescList()
{
	RemoveAll();
}

/*
 *�������ֵ����Сֵ,ǰ�����Ѿ��и���,��������ų�����Χ,�򷵻�false	
 */
bool FieldDescList::SetAt(int nIndex,double dMin,double dMax)
{
	FieldDesc* pFieldDesc = GetAt(nIndex);
	if (pFieldDesc==NULL)
		return false;
	pFieldDesc->dMax = dMax;
	pFieldDesc->dMin = dMin;
	return true;
}

/*
 *���һ��Ŀ
 */
void FieldDescList::Add(CTString strName,CFieldType nType,double dMin,double dMax)
{
	FieldDesc *p = new FieldDesc();
	p->strName = strName;
	p->nType = nType;
	p->dMin = dMin;
	p->dMax = dMax;
	m_Array.push_back(p);
}

FieldDesc* FieldDescList::GetAt(int nIndex)
{
	if (nIndex >=m_Array.size() || nIndex<0)
		return NULL;
	return m_Array.at(nIndex);
}

void FieldDescList::RemoveAll()
{
	for (int i=0;i<m_Array.size();i++)
		delete m_Array.at(i);
	m_Array.clear();
}

bool CBPNet::Export(std::string szFileName)
{
	int i=0, nSize=0;
	CTString szText(""), szTemp("");
	TCLFile theFile;
	
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Javaʹ��
	//�㷨����
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += BPNET;
	//����Դ
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" + m_pDataInterface->m_DataAccess.GetFileName() + "\"";
	//������� 
	szText += " ";
	szText += VARINPUT;
	nSize = m_arInputField.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " + m_arInputField.GetAt(i);
	}
	//Ŀ�����
	szText += " ";
	szText += VAROUTPUT;
	nSize = m_arDestField.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " + m_arDestField.GetAt(i);
	}
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
	//	FieldDescList m_InAry;
	nSize = m_InAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		FieldDesc *pField = m_InAry.GetAt(i);
		theFile << pField->nType;
		theFile << pField->dMax;
		theFile << pField->dMin;
		theFile << pField->strName;
	}
	//	FieldDescList m_OutAry;
	nSize = m_OutAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		FieldDesc *pField = m_OutAry.GetAt(i);
		theFile << pField->nType;
		theFile << pField->dMax;
		theFile << pField->dMin;
		theFile << pField->strName;
	}
	//	TrainItem m_TrainItem;
	theFile << m_TrainItem.m_bCalc;
	theFile << m_TrainItem.nClassCount;
	theFile << m_TrainItem.nHideNum;
	theFile << m_TrainItem.nTrainTimes;
	theFile << m_TrainItem.dPrecision;
	theFile << m_TrainItem.dStep;
	for (i=0; i<m_TrainItem.nClassCount; i++)
	{
		theFile << m_TrainItem.pSortAry[i];
	}
	m_Bp.Export(theFile);
	theFile.Close();
	return true;
}

bool CBPNet::Import(std::string szFileName)
{
	int i=0, nSize=0;
	CFieldType ft;
	double dMax=0, dMin=0;
	CTString szName("");

	TCLFile theFile;
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeRead);
	if (!bOpen)
		return false;
	//��β����
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//��ʼдģ�Ͳ���
	//	FieldDescList m_InAry;
	theFile >> nSize;
	m_InAry.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> ft;
		theFile >> dMax;
		theFile >> dMin;
		theFile >> szName;
		m_InAry.Add(szName,ft,dMax,dMin);
	}
	//	FieldDescList m_OutAry;
	theFile >> nSize;
	m_OutAry.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> ft;
		theFile >> dMax;
		theFile >> dMin;
		theFile >> szName;
		m_OutAry.Add(szName,ft,dMax,dMin);
	}
	//	TrainItem m_TrainItem;
	theFile >> m_TrainItem.m_bCalc;
	theFile >> m_TrainItem.nClassCount;
	theFile >> m_TrainItem.nHideNum;
	theFile >> m_TrainItem.nTrainTimes;
	theFile >> m_TrainItem.dPrecision;
	theFile >> m_TrainItem.dStep;
	m_TrainItem.pSortAry = new double[m_TrainItem.nClassCount];
	for (i=0; i<m_TrainItem.nClassCount; i++)
	{
		theFile >> m_TrainItem.pSortAry[i];
	}
	m_Bp.Import(theFile);
	theFile.Close();

	return true;
}
/*
*����������
*bClassMode �Ƿ�Ϊ����ģʽ
*/
void CBPNet::CreateOut(CResult *pResult, double *pActualAry, double *pOutAry, int nLen, CTString strTitle,bool bClassMode /*=false*/)
{
	CTLTable* pTableMin=new CTLTable;
	//pTableMin->SetCols(m_pBp->m_mxTrainOut.GetMatrixColNumber());
	pTableMin->SetRows( nLen +1 );
	
	pTableMin->SetTitle(strTitle+ "���");
	
	pTableMin->InsertColumn(0, "");
	pTableMin->InsertColumn( 1,"Ԥ��ֵ");
	pTableMin->InsertColumn( 2, "ʵ��ֵ");
	if (bClassMode==false)
	{//����ģʽ
		pTableMin->InsertColumn( 3, "�������");
		pTableMin->InsertColumn( 4, "������");
	}
	int nCorrect = 0;//��ȷ�� ����ģʽ
	for(int nRow=0; nRow< nLen; nRow++ )
	{
		CTString string;string.Format( "�к�:%02d", nRow+1 );
		pTableMin->InsertItem( nRow, string , true );
		
		double dPredict =  *(pOutAry+nRow);//Ԥ��ֵ
		double dActual = *(pActualAry+nRow);//ʵ��ֵ
		pTableMin->SetItemText( nRow, 1, dPredict);
		pTableMin->SetItemText( nRow, 2, dActual);
		if (bClassMode==false)
		{//����ģʽ
			pTableMin->SetItemText( nRow, 3, fabs(dPredict- dActual));
			pTableMin->SetItemText( nRow, 4, fabs(dPredict- dActual)/fabs(dActual));
		}
		else
		{//����ģʽ
			if (dPredict == dActual)
			{
				nCorrect++;
			}
		}
		
	}
	if (bClassMode==false)
	{//����ģʽ
	}
	else
	{//����ģʽ
		//���׼ȷ��
		CRsltElementText*pRsltElementText=new CRsltElementText("����׼ȷ��");
		CTString str;str.Format("����׼ȷ��%2.2f%%",(double)nCorrect*100.0/nLen);
		pRsltElementText->AddString(str);
		pResult->Add(pRsltElementText);
	}
	CRsltElementTable *pVariableMin = new CRsltElementTable(strTitle+  "��", pTableMin );
	pResult->Add(pVariableMin);
	
	//********************  ��ͼ  start********************
	{
		int nTotalRows = nLen;
		CTChartLine*pTChartLine=new CTChartLine;
// 		CTChartLine*pTChartLine=new CTChartLine(nTotalRows,0,ValVal,"","","ֵ");
		
		//�õ�Variable����//////////////////////////////////////////////////////////
		TDataPointValVal* pDPSV1=new TDataPointValVal[nTotalRows];
		TDataPointValVal* pDPSV2=new TDataPointValVal[nTotalRows];
	    double Max=*pActualAry,Min=*pActualAry;
		int nRow;
		for(nRow=0;nRow<nTotalRows;nRow++)
		{
			pDPSV1[nRow].fXVal =nRow+1;
			pDPSV2[nRow].fXVal =nRow+1;
			double dPredict = *(pOutAry+nRow);//Ԥ��ֵ
			double dActual = *(pActualAry+nRow);//ʵ��ֵ
			pDPSV1[nRow].fYVal =dPredict;
			pDPSV2[nRow].fYVal =dActual;
		    if (dActual>Max)
				 Max=dActual;
			if(dActual<Min)
				 Min=dActual;
		} 
		for( nRow=0;nRow<nTotalRows;nRow++)
			pDPSV1[nRow].fYVal=pDPSV1[nRow].fYVal+(Max-Min)*0.8;
		pTChartLine->SetLine(pDPSV1,nTotalRows,ValVal);
		pTChartLine->SetLine(pDPSV2,nTotalRows,ValVal); 
		delete []pDPSV1;
		delete []pDPSV2;
		pDPSV1 = pDPSV2 = NULL;
		
		CRsltElementChart*pREC=new CRsltElementChart(strTitle+ "���ͼ",pTChartLine);
		pResult->Add(pREC);   
	}
	//********************  ��ͼ  end********************
	
}





















