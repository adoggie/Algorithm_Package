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
//			        单维布尔关联规则,行记录为客户的销售信息		
//
//////////////////////////////////////////////////////////////////////////////////
bool CSingleRuleReduction::SingleRulereduction(CIntMatrix matrix,int miniSupport)
{
	WriteData(matrix,"Input");
	int nRow = matrix.mrow();		//客户的销售记录
	int nCol = matrix.mcol();		//商品
	
	m_nCustomNum = nRow;
	//原始项目表
	int * pSet = new int [nCol];
	int i;
	for(i=0;i<nCol;i++)
	{
		pSet[i] =  i;
	}
	CPtrArray arLk;

	//开始建立1项集
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

	//建立并发现2-n项集
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
		
		//对事务数据循环
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
		str.Format("由%d项集生成%d项集,当前项集的长度为%d!",nk,nk+1,mk);
		pProgress->SetIndex(str.GetBuffer(10));	
	}
	*/
	//nk 是代表第k-1项集，来此要生成k-项集；mk 代表k-1项集的长度
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
	//下面对k-1－频繁项集中的每一项循环，以便产生k-候选项集；
	for(int i=0;i<mk-1;i++)//选定（k-1）－频繁项集中的第i项；
	{
		/*
		if( lp )
		{
			pBar->StepIt();
		}
		*/
		mark = 1;
		for(j=i+1;j<mk && mark == 1;j++)//选定（k-1）－频繁项集中的第j(j<i)项；
		{
			//比较 第i项 和 第j项 ，是否 第i项 < 第j项 , 若不是，则 mark = 0；若是，mark = 1 ;
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
			//使用 mark ，将 第i项 和 第j项 组合成 k－项集，产生k－候选项集 ；			
			if( mark == 1 )
			{
				pt1->arIndex.Add( pt2->arIndex.GetAt(nk) );
				//FindNonFrequentSubSet()函数是判断新生成的k－项的所有子集是否在（k-1）－频繁项集中；
				if( FindNonFrequentSubSet(pt1,par,nk+1,mk) )
				{
					//将新生成的k－项加入k－候选项集中
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

	//统计vector中的大于0的个数
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
	//c是k-项集的候选项
	//itemSet是频繁(k-1)-项集n
	//nk代表itemSet项集的宽度，即 nk=k-1
	//要发现c的k-1子项集是否都属于k-1项集中	
	//第一步寻找c的所有的k-1子项集
	int nLen = pcandidateItem->arIndex.GetSize();
	CIntVector   vect(nLen);
	int i;
	for(i=0;i<nLen;i++)
	{
		vect[i] = *(int *)pcandidateItem->arIndex.GetAt(i);		 
	}
	CIntMatrix matrix_subset = nchoosek(vect,nk);
	//第二步：有一个元素不在里面就结束循环
	bool bFlag = true; //表明c的所有子项集都在itemSet中
	
	
	for(i=0;bFlag && i < matrix_subset.mrow();i++)//只要有一个子项集不在频繁项集里就返回false
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
//===疑仅为 单维关联规则调用

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
		int nCaseNumb = pDataInterface->GetColData(nIndex,pDataArray,1);//取回数据	
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
	pTable->InsertColumn(0,"频繁项集");
	pTable->InsertColumn(1,"频    数");
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
	CRsltElementText * pText= new CRsltElementText("强规则");
	assert(pResult);
	pResult->Add(pText);
	CTString strResult;
	CTString str;
	strResult += _T("强规则\n\n");
	for(int i=1;i<par->GetSize();i++)			//对k-频繁项集,用i循环;
	{
		if( pbStop && *pbStop == true )
		{
			//用户终止了计算
			pText->AddString(strResult);
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
							str.Format("%d ",nt);
							strResult += str;
							if( m != vect_subset.vlen()-1 )
							{			
								strResult += _T("Λ ");
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
								strResult += _T("Λ ");
							}
						}						
						str.Format("       置信度 = %10.2f\n",confidence);	
						strResult += str;
					}
				}				
			}
		}
	}
	if( strResult.GetLength() < 10 )
	{
		strResult += _T("本次计算没有找到符合要求得强规则!请重新设置参数计算!");
	}
	pText->AddString(strResult);
}


int CSingleRuleReduction::GetSubSetCount(CPtrArray * par,CIntVector vect)
{
	for(int i=0;i<par->GetSize();i++) //对第s-频繁项集元素循环;
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

//两个向量相减
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
//函数名称：SetErrInfo
//输入参数：包含错误信息的CTString,CResult*&
//函数功能：将错误信息加入结果集
//////////////////////////////////////////////////////////////////////////
void CSingleRuleReduction::SetErrInfo(CTString FormmatStr, CResult *& pResult)
{
	CRsltElementText * pText = new CRsltElementText("警告(关联规则参数设置)\n");
	pText->AddString(FormmatStr);
	pResult = new CResult("关联规则分析");
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
//	输入参数：包含参数信息的CTString
//
//	函数功能：读取参数字符串，并设置相关变量值
//////////////////////////////////////////////////////////////////////////
bool CSingleRuleReduction::SetTrainVO(string strVO)
{
	//	关联规则VO设置
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
