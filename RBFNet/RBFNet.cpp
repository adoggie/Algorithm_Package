#ifndef _UNIX
#include <io.h>
#endif
#include <stdio.h>
#include "RBFNet.h"

#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../Public/FileReadWrite.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"

extern "C" __declspec( dllexport ) 
int RBFNetTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{
	CRBFNet rbf;

	(*ppRst) = rbf.TrainRun(pData,pEnv,itt,VO);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	//����ģ��
	if (rbf.m_ModelFile.GetLength() < 1)
		return 0;
	rbf.Export(rbf.m_ModelFile.GetData());
	return 0;
}


extern "C" __declspec( dllexport ) 
int RBFNetPred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			string VO, string &ppOutStruct,CResult **ppRst)
{
	CRBFNet rbf;
	
	(*ppRst) = rbf.PredRun(pData,pEnv,itt,VO,ppOutStruct);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CRBFNet Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRBFNet::CRBFNet()
{

}

CRBFNet::~CRBFNet()
{

}


CResult * CRBFNet::TrainRun(CDataInterface *pDataInterface,CEnvParam *pEnvParam, Interaction * pInt, string strVO)
{
	m_pDataInterface = pDataInterface;
	SetTrainVO(strVO);

	int j;
	CDoubleMatrix mxInput,mxTarget;
	CResult* pResult=NULL;
	
	//�������
	int nRow=m_pDataInterface->GetRowCount();
	int nInputCol=m_strInAry.GetSize();
	int nTargetCol=m_strTargetAry.GetSize();
	if(nInputCol<1)
	{
// 		AfxMessageBox("�����ֶ�δָ����");
		return pResult;
	}
	if((nTargetCol<1) && (m_strMethod=="ѵ��"||m_strMethod=="����"))
	{
// 		AfxMessageBox("ѵ�������ʱ��Ŀ���ֶ�δָ����");
		return pResult;
	}
	if(nTargetCol>0 && m_strMethod=="Ԥ��")
	{
// 		AfxMessageBox("Ԥ��ʱ����ָ��Ŀ���ֶΣ�");
		return pResult;
	}
	if((nTargetCol>1) && (m_strMethod=="ѵ��"||m_strMethod=="����"))
	{
// 		AfxMessageBox("ѵ�������ʱ��Ŀ���ֶ�ֻ��ָ��һ����");
		return pResult;
	}
	if(nRow<1)
	{
// 		AfxMessageBox("���ݼ�Ϊ�գ�");
		return pResult;
	}

	
	//��ȡ�����ֶ�����
	mxInput.create(nRow,nInputCol,0.0);

	CDWordArray nColAry;
	int nn = m_pDataInterface->GetRowCount();
	for(j=0;j<nInputCol;j++)
	{
		int nColIdx = m_pDataInterface->IndexByName(m_strInAry.GetAt(j));
		nColAry.Add(nColIdx);
	}
 	m_pDataInterface->GetColsData(nColAry,mxInput,0);
	
	//��ȡĿ���ֶ�����,ֻ��ѵ���Ͳ�����Ŀ���ֶ�
	mxTarget.create(nRow,1,0.0);
	nColAry.RemoveAll();
	int nTargetIdx = m_pDataInterface->IndexByName(m_strTargetAry.GetAt(0));
	nColAry.Add(nTargetIdx);
	m_pDataInterface->GetColsData(nColAry,mxTarget,0);
	
	//��ʼ����
	CDoubleMatrix mxOutput;
	double dMSE,dStdMSE;		
	if(m_RBFN.Train(mxInput,mxTarget,m_nCenterNum,mxOutput,dMSE,dStdMSE))
	{	
		RBFNetResult RBFNResult;
		RBFNResult.dMSE=dMSE;
		RBFNResult.dStdMSE=dStdMSE;
		RBFNResult.mxOutput=mxOutput;
		RBFNResult.mxTarget=mxTarget;			
		RBFNResult.strRunInfo=m_RBFN.GetRunInfo();

		pResult=ShowResults("ѵ��",RBFNResult);
	}		
	else 
	{
		//ѵ��ʧ�ܵ�ԭ�򣺻�����δͨ�����ݼ�飬�����Ǳ��û���ֹ
		if(m_RBFN.IsError())
			pResult=ShowResults("ѵ��",m_RBFN.GetErrorInfo().c_str());
		else if(m_RBFN.IsAbort())
			pResult=ShowResults("ѵ��",m_RBFN.GetAbortInfo().c_str());
		else assert(false);
	}
	return pResult;
}

bool CRBFNet::SetPredVo(string VO)
{
//		REGLINEARPRED
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/MODELFROMFILE [FILE]
	AnaWord VOstr;
	VOstr.Import(VO);

	//		/MODELFROMFILE [FILE]
	if (AnaSyntax::exist("/MODELFROMFILE", VOstr))//���ļ���ģ��
	{
		m_ModelFile = AnaSyntax::getSingleStr("/MODELFROMFILE", VOstr);
		//�����ļ�
		if (!Import(m_ModelFile.GetData()))
			return false;
	}
	else
	{
		return false;
	}
	CTStringArray szArrInput,szArrOutput,szArrReplace;
	AnaSyntax::getVAR("/VARINPUT", VOstr, szArrInput);
	AnaSyntax::getVAR("/REPLACEVARNAME", VOstr, szArrReplace);
	if (szArrReplace.GetSize()%2 != 0)
		return false;
	if (!AnaSyntax::Replace(m_strInAry, szArrInput, szArrReplace))
		return false;
	//Ŀ�����-���?
	if (AnaSyntax::exist("/PREDNAME", VOstr))
	{
		AnaSyntax::getVAR("/PREDNAME", VOstr, szArrOutput);
		if (m_strTargetAry.GetSize() == szArrOutput.GetSize())
		{
			m_strTargetAry.RemoveAll();
			m_strTargetAry.Copy(szArrOutput);
		}
	}
	else
	{
		for (int i=0; i<m_strTargetAry.GetSize(); i++)
		{
			CTString szTemp = m_strTargetAry.GetAt(i);
			szTemp += "_Ԥ��ֵ";
			m_strTargetAry.SetAt(i,szTemp);
		}
	}
	return true;
}

CResult * CRBFNet::PredRun(CDataInterface *pDataInterface,CEnvParam *pEnvParam, Interaction * pInt, string strVO, string strModel)
{
	CResult* pResult = NULL;
	m_pDataInterface = pDataInterface;
	if (!SetPredVo(strVO))//��������ģ��
	{
		pResult = new CResult("RBF������Ԥ��");
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( "ģ�Ͳ��������޷����С�" );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	int j;
	CDoubleMatrix mxInput,mxTarget;
	
	//�������
	int nRow=m_pDataInterface->GetRowCount();
	int nInputCol=m_strInAry.GetSize();
	int nTargetCol=m_strTargetAry.GetSize();
	if(nInputCol<1)
	{
		// 		AfxMessageBox("�����ֶ�δָ����");
		return pResult;
	}
	if((nTargetCol<1) && (m_strMethod=="ѵ��"||m_strMethod=="����"))
	{
		// 		AfxMessageBox("ѵ�������ʱ��Ŀ���ֶ�δָ����");
		return pResult;
	}
	if(nTargetCol>0 && m_strMethod=="Ԥ��")
	{
		// 		AfxMessageBox("Ԥ��ʱ����ָ��Ŀ���ֶΣ�");
		return pResult;
	}
	if((nTargetCol>1) && (m_strMethod=="ѵ��"||m_strMethod=="����"))
	{
		// 		AfxMessageBox("ѵ�������ʱ��Ŀ���ֶ�ֻ��ָ��һ����");
		return pResult;
	}
	if(nRow<1)
	{
		// 		AfxMessageBox("���ݼ�Ϊ�գ�");
		return pResult;
	}
	
	//��ȡ�����ֶ�����
	mxInput.create(nRow,nInputCol,0.0);
	
	CDWordArray nColAry;
	int nn = m_pDataInterface->GetRowCount();
	for(j=0;j<nInputCol;j++)
	{
		int nColIdx = m_pDataInterface->IndexByName(m_strInAry.GetAt(j));
		nColAry.Add(nColIdx);
	}
	m_pDataInterface->GetColsData(nColAry,mxInput,0);
	
	//��ȡĿ���ֶ�����,ֻ��ѵ���Ͳ�����Ŀ���ֶ�
	//mxTarget.create(nRow,1,0.0);
	//nColAry.RemoveAll();
	//int nTargetIdx = m_pDataInterface->IndexByName(m_strTargetAry.GetAt(0));
	//nColAry.Add(nTargetIdx);
	//m_pDataInterface->GetColsData(nColAry,mxTarget,0);
	
	//��ʼ����
	CDoubleMatrix mxOutput;
	if(m_RBFN.Predict(mxInput,mxOutput))
	{
		RBFNetResult RBFNResult;
		RBFNResult.mxOutput=mxOutput;
		RBFNResult.strRunInfo=m_RBFN.GetRunInfo();
		
		pResult=ShowResults("Ԥ��",RBFNResult);
	}
	else
	{
		if(m_RBFN.IsError())
			pResult=ShowResults("Ԥ��",m_RBFN.GetErrorInfo().c_str());
		else
			assert(false);
	}
	return pResult;
}


bool CRBFNet::SetDataSource(CDataInterface *pDataInterface, const CPtrArray &arFieldRole)
{
	m_pDataInterface = pDataInterface;
	m_arFieldRole.RemoveAll();
	m_arFieldRole.Copy( arFieldRole );	
	m_strInAry.RemoveAll();
	m_strTargetAry.RemoveAll();
	
	//��ȡ�����Ŀ���ֶ�������ѡ����ȫ������ֵ���ֶ�
	int nFieldCount = m_arFieldRole.GetSize(); 
	for(int i=0; i<nFieldCount; i++ )
	{
		TFieldRole * pRole = (TFieldRole*)(m_arFieldRole.GetAt(i));
		if( pRole->strRole==CTString("�������") || pRole->strRole==CTString("Ŀ�����"))
		{
            CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(pRole->strFieldName); 
			CFieldType cType = pField->GetFieldType();
			if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
			{
				// 				AfxMessageBox( "��Ч����������", MB_ICONEXCLAMATION );				
				return false;
			}
			if (pRole->strRole==CTString("�������"))
			{//�������
				m_strInAry.Add(pField->GetFieldName());
			}
			else
			{//Ŀ�����
				m_strTargetAry.Add(pField->GetFieldName());
			}
		}
	}
	
	return true;
}

void CRBFNet::SetTrainVO(string strVO)
{
	//	RBF������VO����
	//	RBFNETTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/HIDENCENTER = CENTER(10**)
	//	/SAVEMODEL [FILE]
	AnaWord VOstr;
	VOstr.Import(strVO);
	
	if (AnaSyntax::findstr("/HIDENCENTER","CENTER",VOstr))
		m_nCenterNum = AnaSyntax::getvalue("/HIDENCENTER","CENTER",VOstr);
	else 
		m_nCenterNum = 10;
	
	AnaSyntax::getVAR("/VARINPUT", VOstr, m_strInAry);
	AnaSyntax::getVAR("/VARDEST", VOstr, m_strTargetAry);
	m_bSaveModel = AnaSyntax::exist("/SAVEMODEL",VOstr);
	if (m_bSaveModel)
		m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL",VOstr);

	return;	
}
//add by xds 20060712
bool CRBFNet::Export(string szFileName)
{
	int i=0, nSize=0;
	CTString szText(""),szTemp("");
	TCLFile theFile;

	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Javaʹ��
	//�㷨����
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += RBFNET;
	//����Դ
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" ;
	szText +=m_pDataInterface->m_DataAccess.GetFileName() ;
	szText += "\"";
	//������� 
	szText += " ";
	szText += VARINPUT;
	nSize = m_strInAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " ;
		szText += m_strInAry.GetAt(i);
	}
	//Ŀ�����
	szText += " ";
	szText += VAROUTPUT;
	nSize = m_strTargetAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " ;
		szText += m_strTargetAry.GetAt(i);
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
	nSize = m_strInAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_strInAry.GetAt(i);
	}
	nSize = m_strTargetAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_strTargetAry.GetAt(i);
	}	
	//����
	m_RBFN.Export(theFile);
	theFile.Close();
	return true;
}

bool CRBFNet::Import(string szFileName)
{
	int i=0, nSize=0;
	CTString szTemp("");
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
	theFile >> nSize;
	m_strInAry.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> szTemp;
		m_strInAry.Add(szTemp);
	}
	theFile >> nSize;
	m_strTargetAry.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> szTemp;
		m_strTargetAry.Add(szTemp);
	}
	m_RBFN.Import(theFile);
	theFile.Close();
	return true;
}


CResult * CRBFNet::Run(/*CWnd *pProgressWnd*/)
{
//	ASSERT( m_pDataInterface!=NULL );
//	m_pProgressWnd = pProgressWnd;
//	
//	CTString strProgressText = _T("��ȡ����...");
//	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
	
	////////////////////////////////////////////////////////////////////////
	//���㲢��ʾ���
	int j;
// 	CTString str;
	CDoubleMatrix mxInput,mxTarget;
//	CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
	CResult* pResult;
	
	//�������
	int nRow=m_pDataInterface->GetRowCount();
	int nInputCol=m_strInAry.GetSize();
	int nTargetCol=m_strTargetAry.GetSize();
	if(nInputCol<1)
	{
// 		AfxMessageBox("�����ֶ�δָ����");
		return NULL;
	}
	if((nTargetCol<1) && (m_strMethod=="ѵ��"||m_strMethod=="����"))
	{
// 		AfxMessageBox("ѵ�������ʱ��Ŀ���ֶ�δָ����");
		return NULL;
	}
	if(nTargetCol>0 && m_strMethod=="Ԥ��")
	{
// 		AfxMessageBox("Ԥ��ʱ����ָ��Ŀ���ֶΣ�");
		return NULL;
	}
	if((nTargetCol>1) && (m_strMethod=="ѵ��"||m_strMethod=="����"))
	{
// 		AfxMessageBox("ѵ�������ʱ��Ŀ���ֶ�ֻ��ָ��һ����");
		return NULL;
	}
	if(nRow<1)
	{
// 		AfxMessageBox("���ݼ�Ϊ�գ�");
		return NULL;
	}

	
	//��ȡ�����ֶ�����
	mxInput.create(nRow,nInputCol,0.0);

	CDWordArray nColAry;
	int nn = m_pDataInterface->GetRowCount();
	CTString strInputName;
	for(j=0;j<nInputCol;j++)
	{
		strInputName = m_strInAry.GetAt(j);
		int nColIdx = m_pDataInterface->IndexByName(strInputName);
		nColAry.Add(nColIdx);
	}
 	m_pDataInterface->GetColsData(nColAry,mxInput,0);
	
	//��ȡĿ���ֶ�����,ֻ��ѵ���Ͳ�����Ŀ���ֶ�
	mxTarget.create(nRow,1,0.0);
	nColAry.RemoveAll();
	CTString strTarget = m_strTargetAry.GetAt(0);
	int nTargetIdx = m_pDataInterface->IndexByName(strTarget);
	nColAry.Add(nTargetIdx);
	m_pDataInterface->GetColsData(nColAry,mxTarget,0);
	
	//��ʼ����
	CDoubleMatrix mxOutput;
	double dMSE,dStdMSE;		
	if(m_strMethod=="ѵ��")
	{
//		strProgressText = _T("��ʼѵ��...");
//		m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );		
		if(m_RBFN.Train(mxInput,mxTarget,m_nCenterNum,mxOutput,dMSE,dStdMSE))
		{	
			//AfxMessageBox("stop");
			
			RBFNetResult RBFNResult;
			RBFNResult.dMSE=dMSE;
			RBFNResult.dStdMSE=dStdMSE;
			RBFNResult.mxOutput=mxOutput;
			RBFNResult.mxTarget=mxTarget;			
			RBFNResult.strRunInfo=m_RBFN.GetRunInfo();

			pResult=ShowResults("ѵ��",RBFNResult);
		}		
		else 
		{
			//ѵ��ʧ�ܵ�ԭ�򣺻�����δͨ�����ݼ�飬�����Ǳ��û���ֹ
			if(m_RBFN.IsError())
				pResult=ShowResults("ѵ��",m_RBFN.GetErrorInfo().c_str());
			else if(m_RBFN.IsAbort())
				pResult=ShowResults("ѵ��",m_RBFN.GetAbortInfo().c_str());
			else assert(false);
		}
	}
	else if(m_strMethod=="����")
	{
//		strProgressText = _T("��ʼ����...");
//		m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
		if(m_RBFN.Test(mxInput,mxTarget,mxOutput,dMSE,dStdMSE))
		{			
			RBFNetResult RBFNResult;
			RBFNResult.dMSE=dMSE;
			RBFNResult.dStdMSE=dStdMSE;
			RBFNResult.mxOutput=mxOutput;
			RBFNResult.mxTarget=mxTarget;
			RBFNResult.strRunInfo=m_RBFN.GetRunInfo();
			
			pResult=ShowResults("����",RBFNResult);
		}
		else
		{
			if(m_RBFN.IsError())
				pResult=ShowResults("����",m_RBFN.GetErrorInfo().c_str());
			else
				assert(false);
		}
	}	
	else if(m_strMethod=="Ԥ��")
	{
//		strProgressText = _T("��ʼԤ��...");
//		m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
		if(m_RBFN.Predict(mxInput,mxOutput))
		{
			RBFNetResult RBFNResult;
			RBFNResult.mxOutput=mxOutput;
			RBFNResult.strRunInfo=m_RBFN.GetRunInfo();
			
			pResult=ShowResults("Ԥ��",RBFNResult);
		}
		else
		{
			if(m_RBFN.IsError())
				pResult=ShowResults("Ԥ��",m_RBFN.GetErrorInfo().c_str());
			else
				assert(false);
		}
	}
	else
		assert(false);
	
	return pResult;
}

void CRBFNet::Abort()
{
	m_RBFN.Abort();
}

CResult* CRBFNet::ShowResults(CTString strModel, CTString strInfo)
{
	CResult *pResult = new CResult("RBF������"+strModel);
	CRsltElementText *pRsltElementText=new CRsltElementText("ϵͳ"+strModel+"��Ϣ");
	pRsltElementText->AddString(strInfo);
	pResult->Add(pRsltElementText);
	return pResult;	
}

CResult* CRBFNet::ShowResults(CTString strModel, RBFNetResult& RBFNResult)
{

	int nRow=RBFNResult.mxOutput.mrow();
	const int nMaxShowRows=10000;//����¼������nMaxShowRows������������ļ���ͼ�ν���ʾnMaxShowRows����¼
	CResult *pResult;
	pResult = new CResult("RBF������"+strModel);
	CTString stemp;
	//���������Ϣ
	CRsltElementText *pRsltRunInfo=new CRsltElementText("������Ϣ");
	pRsltRunInfo->AddString(RBFNResult.strRunInfo.c_str());
	pResult->Add(pRsltRunInfo);

	if(strModel=="ѵ��"||strModel=="����")
	{
		CTString str,strTargetName=m_strTargetAry[0];
		
		//���ϵͳ����
		CRsltElementText *pRsltElementText=new CRsltElementText("ϵͳ"+strModel+"����");
		strModel+="����Ŀ��ֵ��RBF���������ֵ�ľ�������׼��ֵ��=%f\n";
		strModel+="����Ŀ��ֵ��RBF���������ֵ�ľ�����ʵ��ֵ��=%f";
		str.Format(strModel,RBFNResult.dStdMSE,RBFNResult.dMSE);
		pRsltElementText->AddString(str);
		pResult->Add(pRsltElementText);

		//���Ŀ��������
		//********************  �Ʊ�  start********************
		//���������ݽ���
		if(nRow>nMaxShowRows)//����¼��̫����������ļ�
		{
			CTString szFldName("");
			CField *pField1=NULL;	
			CField *pField2=NULL;
			CField *pField3=NULL;
			CField *pField4=NULL;	

			szFldName = "Ԥ��ֵ";
			if (m_pDataInterface->m_DataAccess.FieldByName(szFldName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField1=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField1->SetFieldName(szFldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField1);	
			
			szFldName = "ʵ��ֵ";
			if (m_pDataInterface->m_DataAccess.FieldByName(szFldName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField2=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField2->SetFieldName(szFldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField2);	
			
			szFldName = "�������";
			if (m_pDataInterface->m_DataAccess.FieldByName(szFldName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField3=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField3->SetFieldName(szFldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField3);

			szFldName = "������";
			if (m_pDataInterface->m_DataAccess.FieldByName(szFldName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField4=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField4->SetFieldName(szFldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField4);
			int nRecordCount = m_pDataInterface->m_DataAccess.GetRecordCount();
			m_pDataInterface->m_DataAccess.First();
			for(int i=0;i<nRow;i++)
			{
				if (i<nRecordCount)
					m_pDataInterface->m_DataAccess.Edit();
				else
					m_pDataInterface->m_DataAccess.Append();
				double dPredict=RBFNResult.mxOutput(i,0);
				double dActual=RBFNResult.mxTarget(i,0);
				pField1->SetAsDouble(dPredict);
				pField2->SetAsDouble(dActual);
				pField3->SetAsDouble(fabs(dPredict- dActual));
				pField4->SetAsDouble(fabs(dPredict- dActual)/fabs(dActual));	
				if (i<nRecordCount)
				{
					m_pDataInterface->m_DataAccess.Post();
					m_pDataInterface->m_DataAccess.Next();
				}
			}
			//�����ļ�			
			CTString szNowFile("");
			CTString szNewFile("");
			szNowFile = m_pDataInterface->m_DataAccess.GetFileName();
			bool bOld = m_pDataInterface->m_DataAccess.SaveFile(szNowFile,&szNewFile);
			//��ʱ:ΪNewFile
			//��ʱ:ΪNowFile.		
			char buffer[20];
			stemp="Ŀ�����("+strTargetName;
			stemp+=")";
			stemp+=strModel;
			stemp+="��";
			CRsltElementText *pShowTableText=new CRsltElementText(stemp);
			stemp ="Ŀ�����("+strTargetName;
			stemp+=")";
			stemp+=strModel;
			stemp+="���\n";
			stemp+="����������";
			sprintf(buffer,"%d",nMaxShowRows);
			stemp+=buffer;//_itoa(nMaxShowRows,buffer,10);
			stemp+="�����ѱ�����";
			stemp+=(bOld ? szNowFile.GetData() : szNewFile.GetData()) ;
			pShowTableText->AddString(stemp);
			pResult->Add(pShowTableText);			
		}
		else
		{				
			CTLTable* pTableMin=new CTLTable;
			nRow=RBFNResult.mxOutput.mrow();
			pTableMin->SetRows( nRow +1 );
			stemp="Ŀ�����("+strTargetName;
			stemp+=")"+strModel;
			stemp+="���";
			pTableMin->SetTitle(stemp);
			pTableMin->InsertColumn(0, "��¼��");
			pTableMin->InsertColumn( 1,"Ԥ��ֵ");
			pTableMin->InsertColumn( 2, "ʵ��ֵ");
			pTableMin->InsertColumn( 3, "�������");
			pTableMin->InsertColumn( 4, "������");
			
			for(int i=0;i<nRow;i++)
			{
				str.Format( "%d", i+1 );
				pTableMin->InsertItem( i, str , true );
				double dPredict=RBFNResult.mxOutput(i,0);
				double dActual=RBFNResult.mxTarget(i,0);

				pTableMin->SetItemText( i, 1, dPredict);
				pTableMin->SetItemText( i, 2, dActual);
				pTableMin->SetItemText( i, 3, fabs(dPredict- dActual));
				pTableMin->SetItemText( i, 4, fabs(dPredict- dActual)/fabs(dActual));		
			}
			stemp = "Ŀ�����("+strTargetName;
			stemp+=")"+strModel;
			stemp+="��";
			CRsltVariable *pVariableMin = new CRsltElementTable(stemp, pTableMin );
			pResult->Add(pVariableMin);
		}
		//********************  �Ʊ�  end********************

		
		//********************  ��ͼ  start********************
		//������������nMaxShowRows����ֻ�������о��ȷֲ���nMaxShowRows����
		double dInterval=1.0;
		if(nRow>nMaxShowRows)
		{
			dInterval=double(nRow)/nMaxShowRows;
		}
		int nNewRow=nRow/dInterval+0.5; //��nRow>nMaxShowRows,��nNewRow��nMaxShowRows���
		
		CTString strTitle="";
		if(nRow>nMaxShowRows)
		{
			char buffer[120];
			sprintf(buffer,"(����������%d���������ȳ�ȡ%d����ͼ)",nMaxShowRows,nMaxShowRows);
//			strTitle=CTString("")+_itoa(nMaxShowRows,buffer,10)+""
//				+_itoa(nMaxShowRows,buffer,10)+"";
			strTitle = buffer;
		}
		strTitle="Ŀ�����("+strTargetName;
		strTitle+=")"+strModel;
		strTitle+="���ͼ"+strTitle;
		CTChartLine *pTChartLine=new CTChartLine;
		pTChartLine->SetTitle(strTitle);
		CTString strX = "��¼��";
        CTString strY = "Ŀ��ֵ";
		pTChartLine->SetXAxilLabel(strX);
		pTChartLine->SetYAxilLabel(strY);
		
		//��ȡ����
		TDataPointValVal* pDPSV1=new TDataPointValVal[nNewRow];
		TDataPointValVal* pDPSV2=new TDataPointValVal[nNewRow];
		double dPredict,dActual;
		for(int i=0;i<nNewRow;i++)
		{
			pDPSV1[i].fXVal =int(i*dInterval+0.5)+1;
			pDPSV2[i].fXVal =int(i*dInterval+0.5)+1;
			dPredict = RBFNResult.mxOutput(int(i*dInterval+0.5),0);
			dActual = RBFNResult.mxTarget(int(i*dInterval+0.5),0);					
			pDPSV1[i].fYVal =dPredict;
			pDPSV2[i].fYVal =dActual;
		} 			
		CTString pStr[2];
		pStr[0]="Ԥ������";
		pStr[1]="Ŀ������";		
//		pTChartLine->AddData(0,0,pDPSV1,nNewRow,ValVal); 
		pTChartLine->SetLine(pDPSV1,nNewRow,ValVal,pStr[0]);
//		pTChartLine->AddData(0,1,pDPSV2,nNewRow,ValVal); 
		pTChartLine->SetLine(pDPSV2,nNewRow,ValVal,pStr[1]);
		delete []pDPSV1;
		delete []pDPSV2;
					
//		double fxMin = DBL_MAX;
//		double fxMax =-DBL_MAX;
//		double fyMin =DBL_MAX;
//		double fyMax =-DBL_MAX;
//		pTChartLine->GetMinMaxForData(0,fxMin,fxMax,fyMin,fyMax);
//		pTChartLine->SetAxisRange(0,fxMin,fxMax,fyMin,fyMax);
//		pTChartLine->AddLegend(0,pStr,2);
				
		
		//////һ��ʼ��ʾ��ͼ�α�����ʵ����ർ���ı���
		//////�����ͼ���ϵ������Ҽ���ѡ��������ȡ����ͼ�α���Ż��Ϊ�����ı��⡣��ͼ�����bug
		//////���ֻ�ý���ർ��������Ϊ��ͼ�α���һ��
		CRsltElementChart *pREC=new CRsltElementChart(strTitle,pTChartLine);
		//////////////////////////////////////////////////////////////////////////
		
		pResult->Add(pREC);   
		//********************  ��ͼ  end********************
	}
	else if(strModel=="Ԥ��")
	{
		CTString szPredName("");
		//Ŀ�����ֻ��һ��.
		if (m_strTargetAry.GetSize() > 0)
			szPredName = m_strTargetAry.GetAt(0);
		else
			szPredName = "Ԥ��ֵ";
		//���
		if(nRow>nMaxShowRows)
		{
			CField *pField=NULL;			
			if (m_pDataInterface->m_DataAccess.FieldByName(szPredName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szPredName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField->SetFieldName(szPredName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
			int nRecordCount = m_pDataInterface->m_DataAccess.GetRecordCount();
			m_pDataInterface->m_DataAccess.First();
			for(int i=0;i<nRow;i++)
			{
				if (i<nRecordCount)
					m_pDataInterface->m_DataAccess.Edit();
				else
					m_pDataInterface->m_DataAccess.Append();
				pField->SetAsDouble(RBFNResult.mxOutput(i,0));
				if (i<nRecordCount)
				{
					m_pDataInterface->m_DataAccess.Post();
					m_pDataInterface->m_DataAccess.Next();
				}
			}
			
			//�����ļ�			
			CTString szNowFile("");
			CTString szNewFile("");
			szNowFile = m_pDataInterface->m_DataAccess.GetFileName();
			bool bOld = m_pDataInterface->m_DataAccess.SaveFile(szNowFile,&szNewFile);
			//��ʱ:ΪNewFile
			//��ʱ:ΪNowFile.			
			char buffer[20];
			stemp = "Ŀ�����"+strModel;
			stemp+="��";
			CRsltElementText *pShowTableText=new CRsltElementText(stemp);
			stemp = "Ŀ�����"+strModel;
			stemp+="���\n";
			stemp+="����������";
			sprintf(buffer,"%d",nMaxShowRows);
			stemp+=buffer;
			stemp+="�����ѱ�����";
			stemp+=(bOld ? szNowFile.GetData() : szNewFile.GetData());
			pShowTableText->AddString(stemp);
			pResult->Add(pShowTableText);
		}		
		else
		{		
			//���Ԥ����
			CTLTable* pTableMin=new CTLTable;
			nRow=RBFNResult.mxOutput.mrow();
			pTableMin->SetRows( nRow +1 );
			pTableMin->SetTitle("Ŀ�����"+strModel+"���");
			pTableMin->InsertColumn(0, "��¼��");
			pTableMin->InsertColumn( 1,szPredName);
		
			CTString str;
			for(int i=0;i<nRow;i++)
			{
				str.Format( "%d", i+1 );
				pTableMin->InsertItem( i, str , true );
				double dPredict=RBFNResult.mxOutput(i,0);			
				pTableMin->SetItemText( i, 1, dPredict);			
			}
			CRsltVariable *pVariableMin = new CRsltElementTable("Ŀ�����"+strModel+"��", pTableMin );
			pResult->Add(pVariableMin);
		}
	}
	
	return pResult;
}

//�Զ�������Ҫ������ļ���
CTString CRBFNet::GetSaveFileName()
{
	CTString strDataFile,strSaveFile,strTemp;
	strDataFile=m_pDataInterface->m_DataAccess.GetFileName();
	strTemp=strDataFile;
	
	int nIndex=strDataFile.ReverseFind('.');
	if(nIndex<0)  //����չ��������
	{
		strTemp=strTemp+"_$out$.mkw";
		if(strTemp.ReverseFind(CHAR_SLASH)<0) 
		{//��·���������򱣴�����ǰ·�������ڷ���������������½���δ�����ļ���
			strTemp="./"+strTemp;
		}
	}
	else
	{
		strTemp.Insert(nIndex,"_$out$");
	}
	
	strSaveFile=strTemp;
// 	CFileFind FF;
	int nSuffix=1;
	char buffer[20];

//	struct _finddata_t c_file;
//	long hFile;
	while(1)
	{
		//if( (hFile = _findfirst( strSaveFile.GetData(), &c_file )) == -1L )
		if(_access( strSaveFile.GetData(),0))	//--file existed!
			break;
		else
		{	
			strSaveFile=strTemp;
			sprintf(buffer,"%d",nSuffix);
			//_itoa(nSuffix,buffer,10);
			strSaveFile.Insert(strSaveFile.ReverseFind('.')-1,buffer);
			nSuffix++;
		}
	}							
	
	return strSaveFile;
}

