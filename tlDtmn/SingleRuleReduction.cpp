// SingleRuleReduction.cpp: implementation of the CSingleRuleReduction class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SingleRuleReduction.h"
#include "FileAssociationRule.h"
#include "MultiDimRule.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
// #include "DlgProgress.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned int Calculate(void * lp);

CSingleRuleReduction::CSingleRuleReduction()
{
	m_pDataInterface = NULL;
// 	m_pDlgProgress   = NULL;
	m_pResult = NULL;
	m_dSupport = 0.02;
	m_nCustomNum = 0;
	m_nCustomCol = -1;
	m_nProductCol = -1;
	m_bStopWork = false;
}

CSingleRuleReduction::CSingleRuleReduction(CTString& strProjectPath, bool bNewComponent)
{
	m_pDataInterface = NULL;
	m_pResult = NULL;
	m_dSupport = 0.02;
	m_nCustomNum = 0;
	m_nCustomCol = -1;
	m_nProductCol = -1;
	m_bStopWork = false;
}

CSingleRuleReduction::~CSingleRuleReduction()
{
}

//////////////////////////////////////////////////////////////////////////////////
//
//			        ��ά������������,�м�¼Ϊ�ͻ���������Ϣ		
//
//////////////////////////////////////////////////////////////////////////////////
bool CSingleRuleReduction::SingleRulereduction(CIntMatrix matrix,int miniSupport)
{
	WriteData(matrix,"Input");
	int nRow = matrix.mrow();		//�ͻ������ۼ�¼
	int nCol = matrix.mcol();		//��Ʒ
	
	m_nCustomNum = nRow;
	//ԭʼ��Ŀ��
	int * pSet = new int [nCol];
	int i;
	for(i=0;i<nCol;i++)
	{
		pSet[i] =  i;
	}
	CPtrArray arLk;

	//��ʼ����1�
	CPtrArray * par = new CPtrArray;
	arLk.Add(par);
	for(i=0;i<nCol;i++)
	{
		int support = sum( matrix.col(i) );
		if( support >= miniSupport )
		{
			TUnit * pUnit = new TUnit;
			pUnit->count = support;
			pUnit->arIndex.Add(&pSet[i]);
			par->Add(pUnit);	
		}
	}

	//����������2-n�
	bool bFlag = true;
	int  k = 0;
	int  mark = par->GetSize();
	while(bFlag)
	{
		CPtrArray arCk;
		bool bEndFlag = Aproiri_gen( (CPtrArray *)arLk.GetAt(k),k,mark,&arCk);
		if( bEndFlag )
			break;
		int nRowCk = arCk.GetSize();
		CIntMatrix matrix_CkNum = GetNumOfCk(&arCk);
		
		//����������ѭ��
		for(i=0;i<nRow;i++)
		{
			InXthenCount(matrix_CkNum,matrix.row(i),&arCk);
		}			
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
		WritePtrData(pArray,"pArray");
	}
	Confidence(&arLk,0.30);
	for(i=0;i<arLk.GetSize();i++)
	{
		CPtrArray * pt = (CPtrArray *)arLk.GetAt(i);
		this->FreePtrArray(pt);
		delete pt;
	}
	return true;	
}


bool CSingleRuleReduction::Aproiri_gen(CPtrArray *par, int nk, int mk, CPtrArray *parCk/*,void * lp*/,bool * pStop)
{
	/*
	CProgressCtrl * pBar = NULL;
	if( lp )
	{
		CDlgProgress * pProgress  =(CDlgProgress *)lp;
		pBar=(CProgressCtrl *)(pProgress->GetDlgItem(IDC_PROGRESS2));
		pBar->SetRange(1,mk);
		pBar->SetStep(1);
		CTString str;
		str.Format("��%d�����%d�,��ǰ��ĳ���Ϊ%d!",nk,nk+1,mk);
		pProgress->SetIndex(str.GetBuffer(10));	
	}
	*/
	//nk �Ǵ����k-1�������Ҫ����k-���mk ����k-1��ĳ���
	TUnit * pt1 = NULL;	
	TUnit * pt2 = NULL;
	int *index1 = NULL;
	int *index2 = NULL;
	int *nt1 = NULL;
	int *nt2 = NULL;
	int num = 1;
	int mark = 1;
	int l=0,k=0,j=0;
	TUnit * pcandidateItem = NULL;
	//�����k-1��Ƶ����е�ÿһ��ѭ�����Ա����k-��ѡ���
	for(int i=0;i<mk-1;i++)//ѡ����k-1����Ƶ����еĵ�i�
	{
		/*
		if( lp )
		{
			pBar->StepIt();
		}
		*/
		mark = 1;
		for(j=i+1;j<mk && mark == 1;j++)//ѡ����k-1����Ƶ����еĵ�j(j<i)�
		{
			//�Ƚ� ��i�� �� ��j�� ���Ƿ� ��i�� < ��j�� , �����ǣ��� mark = 0�����ǣ�mark = 1 ;
			if( pStop && *pStop == true )
			{
				return false;
			}
			pt1 = (TUnit *)par->GetAt(i);
			pt2 = (TUnit *)par->GetAt(j);	
			for(k=0;k<=nk;k++)
			{			
				index1  =(int*)pt1->arIndex.GetAt(k);
				index2  =(int*)pt2->arIndex.GetAt(k);
				if( k <= nk - 1 )
				{					
					if( *index1 != *index2 )
					{
						mark = 0;
						break;
					}
				}
				else
				{					
					if( *index1 == *index2 )
					{
						mark = 0;
						break;
					}
				}
			}
			//ʹ�� mark ���� ��i�� �� ��j�� ��ϳ� k���������k����ѡ� ��			
			if( mark == 1 )
			{
				pt1->arIndex.Add( pt2->arIndex.GetAt(nk) );
				//FindNonFrequentSubSet()�������ж������ɵ�k����������Ӽ��Ƿ��ڣ�k-1����Ƶ����У�
				if( FindNonFrequentSubSet(pt1,par,nk+1,mk) )
				{
					//�������ɵ�k�������k����ѡ���
					pcandidateItem = new TUnit;			
					for(l=0;l<pt1->arIndex.GetSize();l++)
					{
						pcandidateItem->arIndex.Add(pt1->arIndex.GetAt(l));
					}
					parCk->Add( pcandidateItem );
				}				
				pt1->arIndex.RemoveAt( pt1->arIndex.GetSize() -1 );
			}		
		}
	}
	if( parCk->GetSize() > 0 )
	{
		return false;
	}
	else
		return true;
}

CIntMatrix CSingleRuleReduction::GetNumOfCk(CPtrArray *par)
{
	TUnit * pUnit = (TUnit *)par->GetAt(0);
	int nLen = pUnit->arIndex.GetSize();
	CIntMatrix matrix(par->GetSize(),nLen);

	for(int i=0;i<par->GetSize();i++)
	{
		pUnit = (TUnit *)par->GetAt(i);
		for(int j=0;j < nLen ;j++)
		{
//////////////////////////////////////////////////////////////////////////

//			matrix[i][j] = *(int *)pUnit->arIndex.GetAt(j);
//////////////////////////////////////////////////////////////////////////
			matrix(i,j) = *(int *)pUnit->arIndex.GetAt(j);
		}
	}
	return matrix;
}

void CSingleRuleReduction::InXthenCount(CIntMatrix matrix, CIntVector vector, CPtrArray *parCk)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();

	//ͳ��vector�еĴ���0�ĸ���
	int count = 0;
	int i;
	for(i=0;i<vector.vlen();i++)
	{
		if( vector[i] > 0 )
		{
			count++;
		}
	}
	CIntVector vect(count);
	count=0;
	for(i=0;i<vector.vlen();i++)
	{
		if( vector[i] > 0 )
		{
			vect[count++] = i;
		}
	}

	for(i=0;i<nRow;i++)
	{
		int mark = 0;
		for(int j=0;j<nCol;j++)
		{
			for(int k=0;k<vect.vlen();k++)
			{
//////////////////////////////////////////////////////////////////////////
				
				//if( matrix[i][j] < vect[k] )
//////////////////////////////////////////////////////////////////////////
				if( matrix(i,j) < vect[k] )
				{
					break;
				}
				else
				{
//////////////////////////////////////////////////////////////////////////

//					if( matrix[i][j] == vect[k] )
//////////////////////////////////////////////////////////////////////////
					if( matrix(i,j) == vect[k] )
					{
						mark++;
						continue;
					}
				}
			}
		}
		if( mark == nCol )
		{
			TUnit * pUnit = (TUnit *)parCk->GetAt(i);
			pUnit->count++;
		}
	}
	for(i=0;i<parCk->GetSize();i++)
	{
		TUnit * pUnit = (TUnit *)parCk->GetAt(i);
// 		TRACE("count = %d \n",pUnit->count);
	}
}

bool CSingleRuleReduction::FindNonFrequentSubSet(TUnit * pcandidateItem, CPtrArray *pSetLk, int nk, int mk)
{
	//c��k-��ĺ�ѡ��
	//itemSet��Ƶ��(k-1)-�n
	//nk����itemSet��Ŀ�ȣ��� nk=k-1
	//Ҫ����c��k-1����Ƿ�����k-1���	
	//��һ��Ѱ��c�����е�k-1���
	int nLen = pcandidateItem->arIndex.GetSize();
	CIntVector   vect(nLen);
	int i;
	for(i=0;i<nLen;i++)
	{
		vect[i] = *(int *)pcandidateItem->arIndex.GetAt(i);		 
	}
	CIntMatrix matrix_subset = nchoosek(vect,nk);
	//�ڶ�������һ��Ԫ�ز�������ͽ���ѭ��
	bool bFlag = true; //����c�������������itemSet��
	
	
	for(i=0;bFlag && i < matrix_subset.mrow();i++)//ֻҪ��һ���������Ƶ�����ͷ���false
	{
		bFlag = IsInSet(matrix_subset.row(i),pSetLk,mk); 
	}

	return bFlag;
}

bool CSingleRuleReduction::IsInSet(CIntVector vect, CPtrArray *parItem, int mk)
{
	int i = 0,j = 0;
	int count = 0;
	TUnit * pUnit = NULL;
	for(i=0;i<mk;i++)
	{
		count = 0;
		pUnit = (TUnit *)parItem->GetAt(i);
		for(j=0;j<vect.vlen();j++)
		{			
			if( vect[j] == *((int *)pUnit->arIndex.GetAt(j)))
			{
				count++;
			}
			else
			{
				break;
			}
		}
		if( count == vect.vlen() )
		{
			return true;
		}	
	}
	return false;
}

CResult * CSingleRuleReduction::OnSingleRuleReduction(CDataInterface *pDataInterface)
{
	return NULL; 
}
//===�ɽ�Ϊ ��ά�����������

CIntMatrix CSingleRuleReduction::GetData(CDataInterface *pDataInterface)
{
	int nRow = pDataInterface->GetRowCount();
	int nCol = m_tVarInfo.GetSize();
	CIntMatrix matrix(nRow,nCol);
	TDataSet * pDataArray=NULL;
	for(int i=0;i<nCol;i++)
	{
		TFieldRole *pField = (TFieldRole *)m_tVarInfo[i];
		int nIndex = pField->nFieldNo;
		int nCaseNumb = pDataInterface->GetColData(nIndex,pDataArray,1);//ȡ������	
		for(int j=0;j<nRow;j++)
		{
//////////////////////////////////////////////////////////////////////////

		//	matrix[j][i] = (int)pDataArray[j].dValue;		
//////////////////////////////////////////////////////////////////////////
			matrix(j,i) = (int)pDataArray[j].dValue;		
		}
		delete [] pDataArray;pDataArray = NULL;
	}
	return matrix;
}

void CSingleRuleReduction::WritePtrData(CPtrArray *par, char *pszTitle,bool *pStop,CResult * pResult)
{	
	if( par->GetSize() <= 0 )
		return;

	CTLTable * pTable = new CTLTable;
//	assert(pWnd->m_hWnd);
	pTable->SetTitle(pszTitle);
	pTable->SetRows(1+par->GetSize());
	pTable->SetCols(2);
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
			//added by zkc 2004.5.17



			//*********end of addition by zkc 2004.5.17
			str.Format("%12d     ",*pInt);
			strResult += str;
		}
		pTable->InsertItem(i,strResult);
		str.Format("%12d",pUnit->count);
		strResult += str;
		pTable->SetItemText(i,1,str);		
	}
}
void CSingleRuleReduction::WritePtrData(CPtrArray *par, char *pszTitle,CResult * pResult)
{	
	if( par->GetSize() <= 0 )
		return;

	CTLTable * pTable = new CTLTable;
//	assert(pWnd->m_hWnd);
	pTable->SetTitle(pszTitle);
	pTable->SetRows(1+par->GetSize());
	pTable->SetCols(2);
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
		CTString strResult;
		TUnit * pUnit = (TUnit *)par->GetAt(i);	
		for(int j=0;j<pUnit->arIndex.GetSize();j++)
		{


			int * pInt = (int *)pUnit->arIndex.GetAt(j);
			str.Format("%12d     ",*pInt);
			strResult += str;
		}
		pTable->InsertItem(i,strResult);
		str.Format("%12d",pUnit->count);
		strResult += str;
		pTable->SetItemText(i,1,str);		
	}
}
void CSingleRuleReduction::WriteUnitData(TUnit *pUnit, char *pszTitle)
{
	FILE  * fp = fopen("factor_data.txt","a");
	if( !fp )
		return;
	fprintf(fp,"%s\n\n",pszTitle);

	for(int j=0;j<pUnit->arIndex.GetSize();j++)
	{
		int * pInt = (int *)pUnit->arIndex.GetAt(j);
		fprintf(fp,"%d    ",*pInt);
	}
	fprintf(fp,"\n,count = %d\n\n",pUnit->count);

  	fclose(fp);



}

void CSingleRuleReduction::Confidence(CPtrArray *par,double mini_confidence,CResult * pResult,bool * pbStop)
{	
	CRsltElementText * pText= new CRsltElementText("ǿ����");
	assert(pResult);
	pResult->Add(pText);
	CTString strResult;
	CTString str;
	strResult += _T("ǿ����\n\n");
	for(int i=1;i<par->GetSize();i++)			//��k-Ƶ���,��iѭ��;
	{
		if( pbStop && *pbStop == true )
		{
			//�û���ֹ�˼���
			pText->AddString(strResult);
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
							str.Format("%d ",nt);
							strResult += str;
							if( m != vect_subset.vlen()-1 )
							{			
								strResult += _T("�� ");
							}
						}
						strResult += _T("==>>");
						CIntVector vecttemp = VectorSub(vect,vect_subset);
						for(m = 0;m<vecttemp.vlen();m++)
						{
							int nt = vecttemp[m];
							str.Format("%d ",nt);
							strResult += str;
							if( m != vecttemp.vlen()-1 )
							{			
								strResult += _T("�� ");
							}
						}						
						str.Format("       ���Ŷ� = %10.2f\n",confidence);	
						strResult += str;
					}
				}				
			}
		}
	}
	if( strResult.GetLength() < 10 )
	{
		strResult += _T("���μ���û���ҵ�����Ҫ���ǿ����!���������ò�������!");
	}
	pText->AddString(strResult);
}


int CSingleRuleReduction::GetSubSetCount(CPtrArray * par,CIntVector vect)
{
	for(int i=0;i<par->GetSize();i++) //�Ե�s-Ƶ���Ԫ��ѭ��;
	{
		TUnit * pUnit = (TUnit *)par->GetAt(i);
		int nequalnum = 0;	
		for(int j=0;j<vect.vlen();j++)
		{
			if( vect[j] == *(int *)pUnit->arIndex.GetAt(j) )
			{
				nequalnum++;
			}
			else
			{
				break;
			}
		}
		if( nequalnum == vect.vlen()  )
		{
			return pUnit->count;
		}
	}
	return 0;
}

//�����������
CIntVector CSingleRuleReduction::VectorSub(CIntVector vect1, CIntVector vect2)
{
	int count = 0;
	int i;
	for(i=0;i<vect1.vlen();i++)
	{
		for(int j=0;j<vect2.vlen();j++)
		{
			if( vect1[i] == vect2[j] )
			{
				count++;
			}
		}
	}
	CIntVector vect(vect1.vlen() - count);
	count = 0;
	for(i=0;i<vect1.vlen();i++)
	{
		bool bFlag = true;
		for(int j=0;j<vect2.vlen();j++)
		{
			if( vect1[i] == vect2[j] )
			{
				bFlag = false;
			}
		}
		if(bFlag)
		{
			vect[count++] = vect1[i];
		}
	}
	return vect;
}

bool	CSingleRuleReduction::CreatePredModel(CRuleOutput *&pOutStruc)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
//�������ƣ�SetErrInfo
//�������������������Ϣ��CTString,CResult*&
//�������ܣ���������Ϣ��������
//////////////////////////////////////////////////////////////////////////
void CSingleRuleReduction::SetErrInfo(CTString FormmatStr, CResult *& pResult)
{
	CRsltElementText * pText = new CRsltElementText("����(���������������)\n");
	pText->AddString(FormmatStr);
	pResult = new CResult("�����������");
	pResult->Add(pText);
}

string	CSingleRuleReduction::ExportModel()
{
	//VO info
	char buf[255];
	int i,nFildCnt;
	string str;
	
	//	CTStringArray m_strInputField;
	nFildCnt = m_strInputField.GetSize();
	sprintf(buf,"i%di",nFildCnt);
	str += buf;
	for (i=0;i<nFildCnt;i++)
	{
		CTString strField = m_strInputField.GetAt(i);
		sprintf(buf,"s%ds%s",strField.GetLength(),strField.GetData());
		str += buf;	
	}
	
	//	CTStringArray m_strDestField;
	nFildCnt = m_strDestField.GetSize();
	sprintf(buf,"i%di",nFildCnt);
	str += buf;
	for (i=0;i<nFildCnt;i++)
	{
		CTString strField = m_strDestField.GetAt(i);
		sprintf(buf,"s%ds%s",strField.GetLength(),strField.GetData());
		str += buf;	
	}
	
	sprintf(buf,"i%di",m_bSingle);
	str += buf;
	sprintf(buf,"f%ff",m_dSupport);
	str += buf;
	sprintf(buf,"f%ff",m_dConfidence);
	str += buf;
	
	return str;
}

//////////////////////////////////////////////////////////////////////////
//
//                    SetTrainVO
//
//	�������������������Ϣ��CTString
//
//	�������ܣ���ȡ�����ַ�������������ر���ֵ
//////////////////////////////////////////////////////////////////////////
bool CSingleRuleReduction::SetTrainVO(string strVO)
{
	//	��������VO����
	//	ASSORULETRAIN
	//	/DATA = FILE
	//	/VARINPUT arglistg
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/ASSOTYPE [{SINGLE},{MULTI**}]
	//	/MINSUPPORT SUPPORT(0.1**)
	//	/ MINCREDIT CREDIT (0.5**)		
	
	AnaWord VOstr;
	VOstr.Import(strVO);
	
	m_bSingle = AnaSyntax::findstr("/ASSOTYPE","SINGLE",VOstr);
	m_dSupport = AnaSyntax::getvalue("/MINSUPPORT","SUPPORT",VOstr);
	if (m_dSupport < 0)
		m_dSupport = 0.1;
	m_dConfidence = AnaSyntax::getvalue("/MINCREDIT","CREDIT",VOstr);
	if (m_dConfidence < 0)
		m_dConfidence = 0.5;
	
	AnaSyntax::getVAR("/VARINPUT", VOstr, m_strInputField);
	AnaSyntax::getVAR("/VARDEST", VOstr, m_strDestField);

	return true;
}

void CSingleRuleReduction::SetTrainDataParam(CRuleTrainVO *pVO)
{
}

void CSingleRuleReduction::SetPredDataParam(TPredVO *pVO)
{
}

void	CSingleRuleReduction::SetPredModel(string strModel)
{
	//VO info
	char buf[255];
	int i,j,nValue,nCount,len,strLen;
	double dValue;
	char *pCurPos = (char*)strModel.c_str();
	int nFildCnt = 0;
	
	//CTStringArray m_strInputField
	len = str_cpy(buf,pCurPos,'i');
	pCurPos += len;
	nFildCnt = atoi(buf);
	for (i=0;i<nFildCnt;i++)
	{
		int len = str_cpy(buf,pCurPos,'s');
		pCurPos += len;
		strLen = atoi(buf);
		mem_cpy(buf,pCurPos,strLen);
		pCurPos += strLen;
		m_strInputField.Add(buf);
	}
	//CTStringArray m_strDestField;
	len = str_cpy(buf,pCurPos,'i');
	pCurPos += len;
	nFildCnt = atoi(buf);
	for (i=0;i<nFildCnt;i++)
	{
		int len = str_cpy(buf,pCurPos,'s');
		pCurPos += len;
		strLen = atoi(buf);
		mem_cpy(buf,pCurPos,strLen);
		pCurPos += strLen;
		m_strDestField.Add(buf);
	}
	
	//	m_bSingle;
	len = str_cpy(buf,pCurPos,'i');
	pCurPos += len;
	m_bSingle = atoi(buf);
	//	m_dSupport;
	len = str_cpy(buf,pCurPos,'f');
	pCurPos += len;
	m_dSupport = atof(buf);
	//	m_dConfidence;
	len = str_cpy(buf,pCurPos,'f');
	pCurPos += len;
	m_dConfidence = atof(buf);
}

void CSingleRuleReduction::SetDataSource(CDataInterface *pDataInterface, CPtrArray *parField)
{
	m_pDataInterface = pDataInterface;
	//m_tVarInfo.Copy(*parField);
}


CResult* CSingleRuleReduction::Run(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO)
{
	m_pDataInterface = pDataInterface;
	CResult *pResult = NULL;

	SetTrainVO(strVO);

	if (m_bSingle)
	{
		CFileAssociationRule tt;
		tt.m_bSingle = this->m_bSingle;
		tt.m_dSupport = this->m_dSupport;
		tt.m_dConfidence = this->m_dConfidence;
		tt.m_strInputField.Copy(m_strInputField);
		tt.m_strDestField.Copy(m_strDestField);
		pResult = tt.CreateRule(pDataInterface,pEnv,itt);
	}
	else
	{
		CMultiDimRule tt;
		tt.m_bSingle = this->m_bSingle;
		tt.m_dSupport = this->m_dSupport;
		tt.m_dConfidence = this->m_dConfidence;
		tt.m_strInputField.Copy(m_strInputField);
		tt.m_strDestField.Copy(m_strDestField);
		pResult = tt.CreateRule(pDataInterface,pEnv,itt);
	}
	return pResult;
}

CResult* CSingleRuleReduction::RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO,string& strModel)
{
	m_pDataInterface = pDataInterface;
	CResult *pResult = NULL;
	
	SetTrainVO(strVO);
	
	if (m_bSingle)
	{
		CFileAssociationRule tt;
		tt.m_bSingle = this->m_bSingle;
		tt.m_dSupport = this->m_dSupport;
		tt.m_dConfidence = this->m_dConfidence;
		tt.m_strInputField.Copy(m_strInputField);
		tt.m_strDestField.Copy(m_strDestField);
		pResult = tt.CreateRule(pDataInterface,pEnv,itt);
	}
	else
	{
		CMultiDimRule tt;
		tt.m_bSingle = this->m_bSingle;
		tt.m_dSupport = this->m_dSupport;
		tt.m_dConfidence = this->m_dConfidence;
		tt.m_strInputField.Copy(m_strInputField);
		tt.m_strDestField.Copy(m_strDestField);
		pResult = tt.CreateRule(pDataInterface,pEnv,itt);
	}
	return pResult;
}

CResult* CSingleRuleReduction::RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,string strVO, string strModel)
{
	SetPredModel(strModel);
	if (m_bSingle)
	{
		CFileAssociationRule tt;
		return tt.Predict(pDataInterface,pEnv,itt);
	}
	else
	{
		CMultiDimRule tt;
		return tt.Predict(pDataInterface,pEnv,itt);
	}
}


//void CSingleRuleReduction::Assess(CDataInterface *pDataInterface,CDWordArray& nObjAry,CDWordArray& nIpuAry)
void CSingleRuleReduction::Assess()
{
/*
	if (m_pRuleVO != NULL)
	{
		CFileAssociationRule tt;
		tt.SetDataInterface(m_pDataInterface,&m_tVarInfo);
		tt.CreateFrequentSet(m_pRuleVO,true);
	}
	else if (m_pRuleVO != NULL)
	{
		CMultiDimRule tt;
		tt.SetDataInterface(m_pDataInterface,&m_tVarInfo);
		tt.CreateFrequentSet(m_pRuleVO,true);
	}
*/
}



void CSingleRuleReduction::Serialize(TCLFile &file)
{

}

void CSingleRuleReduction::FreePtrArray(CPtrArray *par)
{
	for(int i=0;i<par->GetSize();i++)
	{
		delete par->GetAt(i);
	}
	par->RemoveAll();
}

void CSingleRuleReduction::Abort()
{
}
