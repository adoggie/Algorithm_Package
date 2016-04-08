// Simuequation.cpp: implementation of the CSimuequation class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "Simuequation.h"

const double EPS = 2.22044604925031e-016; //--UNIX PORT
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimuequation::CSimuequation()
{
	
}

CSimuequation::~CSimuequation()
{
	m_tVarInfo.DeleteObject();
	m_tVarInfo_D.DeleteObject();
}

BOOL CSimuequation::GetValueFromVo(CDataInterface* pDataInterface, string VO)
{
	//	/Simuequation
	//		/DATA=[FILE]
	//		/EXOGENOUS [varlsit]
	//		/PREVAR [varlist]
	//		/METHOD [{IV},{ILS},{TWOLS**},{THREELS},{FIML}]
	//		/EQUATION1 [a=c(1)+...]
	//		/EQUATION2 [b=c(2)+...]
	//		/EQUATION2 [d=1+...]
	//		...
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/EXOGENOUS", VOstr, xname);
	int j=0; 
	for (j=0; j<xname.GetSize(); j++)
		if (xname.GetAt(j) == "c" || xname.GetAt(j) == "C")
			return FALSE;
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	xname.RemoveAll();
	AnaSyntax::getVAR("/PREVAR", VOstr, xname);
	for (j=0; j<xname.GetSize(); j++)
		if (xname.GetAt(j) == "c" || xname.GetAt(j) == "C")
			return FALSE;
	if (!(pDataInterface->getVarID(xname,m_tVarInfo_D)))
		return FALSE;
	
	if (AnaSyntax::findstr("/METHOD", "IV", VOstr))
		m_SEmethod = 0;
	else if (AnaSyntax::findstr("/METHOD", "ILS", VOstr))
		m_SEmethod = 1;
	else if (AnaSyntax::findstr("/METHOD", "THREELS", VOstr))
		m_SEmethod = 3;
	else if (AnaSyntax::findstr("/METHOD", "FIML", VOstr))
		m_SEmethod = 4;
	else
		m_SEmethod = 2;
	
	//		/EQUATION1 [a=c(1)+...]
	CTString equt = "/EQUATION1";
	int eqNum=1;
	CTStringArray szArrSE;
	while (AnaSyntax::exist(equt.GetData(), VOstr))
	{
		CTString sent = "";
		for (int i=AnaSyntax::FindFirstWordPos(equt.GetData(), VOstr)+1; i<VOstr.GetWordCount(); i++)
		{
			if (VOstr.GetAt(i)[0] == '/')
				break;
			sent += VOstr.GetAt(i);
		}
		szArrSE.Add(sent);
		eqNum++;
		equt.Format("%s%d", "/EQUATION", eqNum);
	}
	if (szArrSE.GetSize() == 0)
		return FALSE;
	if (!ReadStrs(szArrSE))
		return FALSE;
	if (!ReadStrToMx())//把字符串读为P矩阵
		return FALSE;
	if (!ReadStrToINPMx())//把字符串读为P矩阵
		return FALSE;

	return TRUE;
}

CResult * CSimuequation::OnSimEqu(CDataInterface *pDataInterface, string VO)//外部调用主要程序
{
	CResult * pResult = NULL;
	pResult=new CResult("联立方程模型");
	m_pDataInterface = pDataInterface;
	CDoubleMatrix Matrix_X, Matrix_Y;					//回归矩阵和外生变量矩阵
	m_pResult = pResult;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	
	GetData(Matrix_X, Matrix_Y);//读取数据
	if(!SimEqu(Matrix_X,  Matrix_Y,  m_mxP,  m_mxINP))//调用主函数给出结果
		return m_pResult;
	
	DrawResult();
	return pResult;
}

BOOL CSimuequation::ReadStrs(CTStringArray& equs)
{
	m_coef_var.RemoveAll();
	m_StrVarOrd.RemoveAll();
	m_str_fun.RemoveAll();
	m_CID_fun.RemoveAll();
	m_szArrSE.RemoveAll();
	m_LimitArrSE.RemoveAll();
	
	for (int i=0; i<equs.GetSize(); i++)
	{
		CTString eqution = equs.GetAt(i);
		if (strlen(eqution.GetData()) == 0 || !legstr(eqution))
		{	
			CTString strWarning;
			strWarning.Format("第 %d 行书写不合规范，请检查，或查看帮助。", i+1);
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		if (m_bBeginwithC)
		{
			m_szArrSE.Add(eqution);
		}
		else
		{
			m_LimitArrSE.Add(eqution);
		}
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////
/////							获取数据					////
////////////////////////////////////////////////////////////////
BOOL CSimuequation::GetData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y)
{
	CDWordArray indexary;
	BOOL bTemp = TRUE;
	
	m_strDepend.RemoveAll();
	int i=0; 
	for (i=0; i<m_tVarInfo.iCount; i++)
	{
		int k = -1;
		for (int j=0; j<m_tVarInfo.iCount; j++)
		{
			if (m_ListVarArr.GetAt(i) == m_pDataInterface->GetFieldName(m_tVarInfo.pValue[j]))
				k = j;
		}
		if (k < 0)
			return FALSE;
		if (m_tVarInfo.pValue[k] < 0)
			return FALSE;
		indexary.Add(m_tVarInfo.pValue[k]);
	}
	m_pDataInterface->GetColsData( indexary, Matrix_X, 6 );//Modify by xds 20060815取回界面数据
	indexary.RemoveAll();
	
	CDWordArray indexary0;
	m_strVarSel.RemoveAll();
	for (i=0; i<m_tVarInfo_D.iCount; i++)
	{
		if (m_tVarInfo_D.pValue[i] < 0)
			return FALSE;
		indexary0.Add(m_tVarInfo_D.pValue[i]);
		m_strVarSel.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[i]));
	}
	m_pDataInterface->GetColsData( indexary0, Matrix_Y, 6 );//Modify by xds 20060815取回界面数据
	
	if (m_lagMatrix != NULL)
		m_lagMatrix.destruction();
	m_ivLagNum.create(m_ListVarArr.GetSize() - m_tVarInfo_D.iCount - m_tVarInfo.iCount);
	for (i=m_tVarInfo_D.iCount+m_tVarInfo.iCount; i<m_ListVarArr.GetSize(); i++)
	{
		CTString strTemp = m_ListVarArr.GetAt(i);
		CTString varStrTemp = "";
		int j=0;
		if (strlen(strTemp) < 1) continue;
		int nFind1 = strTemp.Find("(",0);
		if (nFind1 < 0) continue;
		varStrTemp = strTemp.Left(nFind1);
		int nFind2 = strTemp.Find(")",0);
		if (nFind2 < 0 || nFind2 < nFind1) continue;
		int intTemp = abs(atoi(strTemp.Mid(nFind1+1,nFind2-nFind1-1)));

		m_ivLagNum(i - m_tVarInfo_D.iCount - m_tVarInfo.iCount) = intTemp;
		for (j=0; j<m_tVarInfo.iCount; j++)
		{
			if (m_ListVarArr.GetAt(j) == varStrTemp)
				m_lagMatrix = m_lagMatrix|Matrix_X(j);
		}
		for (j=0; j<m_tVarInfo_D.iCount; j++)
		{
			if (m_ListVarArr.GetAt(j+m_tVarInfo.iCount) == varStrTemp)
				m_lagMatrix = m_lagMatrix|Matrix_Y(j);
		}
	}
	
	return TRUE;
}

BOOL CSimuequation::legstr(CTString eqution)
{
	m_bBeginwithC = FALSE;
	CTStringArray appNumStr;//出现的c括号内的数字和出现的字符、变量名
	//	strTemp += " ";
	int i;
	int nparentheses = 0;//括号是否配对
	int nPair = 0;//c和变量之间是否有超过1 的差别
	AnaWord AnaStr;//导入到CAnaWord类中，便于分析。CAnaWord可以把字符串解析成一个一个的单词
	AnaStr.ImportWithOption(eqution.GetData());//字符串转换
	int strLength = AnaStr.GetWordCount();
	
	if (AnaStr.GetWordCount()<3)
		return FALSE;
	
	CTString A0naStr = AnaStr.GetAt(0);
	//第一个是否为内生变量，并加入到内生变量列表中
	for (i=0; i<m_StrVarOrd.GetSize(); i++)
	{
		if (A0naStr == m_StrVarOrd.GetAt(i))
			return FALSE;
	}
	if (inselectinlist(A0naStr ) )
		m_StrVarOrd.Add(A0naStr);
	else
		return FALSE;
	appNumStr.Add(A0naStr);
	
	//第二个是否为=号
	if (AnaStr.GetAt(1) != "=")
		return FALSE;
	//判断最后一个是否合法
	CTString alastStr = AnaStr.GetAt(AnaStr.GetWordCount()-1);
	//	for (int k=0; k<appNumStr.GetSize(); k++)
	//	{
	//		if (alastStr == appNumStr.GetAt(k))
	//			return FALSE;
	//	}
	if (!inselectlist(alastStr) && alastStr != ")")
		return FALSE;
	appNumStr.Add(alastStr);
	
	//从第三个以后开始数
	if (AnaStr.GetAt(2) == "C" || AnaStr.GetAt(2) == "c")//如果第三个字符为c，则为P矩阵
	{
		m_bBeginwithC = TRUE;
		//从等号后边的开始判断是否合法
		int i=3; 
		for (i=3; i<AnaStr.GetWordCount()-1; i++)
		{
			CTString astr = AnaStr.GetAt(i);
			CTString a1str = AnaStr.GetAt(i+1);
			CTString afu1str = AnaStr.GetAt(i-1);
			//对能够出现的字符和出现字符的检查
			if (astr == "c" || astr == "C")
			{
				if (i+4 > strLength-1)
					return FALSE;
				if (AnaStr.GetAt(i+4) != "*")
					return FALSE;
				if (a1str != "(" || afu1str != "+")
					return FALSE;
			}
			else if (astr == "(")
			{
				if (nparentheses++ != 0)
					return FALSE;
				if (a1str != "-" && !strIsANum(a1str) )
					return FALSE;
				if (!inselectlist(afu1str) && afu1str !="c" && afu1str != "C")
					return FALSE;
			}
			else if (astr == "-")
			{
				if (!strIsANum(a1str) || afu1str != "(")
					return FALSE;
			}
			else if (astr == ")")
			{
				if (--nparentheses != 0)
					return FALSE;
				if (!strIsANum(afu1str) || (a1str != "+" && a1str != "*"))
					return FALSE;
			}
			else if (astr == "+")
			{
				if (!inselectlist(afu1str) && afu1str != ")")
					return FALSE;
				if (a1str != "c" && a1str != "C")
					return FALSE;
			}
			else if (astr == "*")
			{
				if (afu1str != ")" || !inselectlist(a1str) )
					return FALSE;
			}
			else if (inselectlist(astr))
			{
				//				for (int j=0; j<appNumStr.GetSize(); j++)
				//				{
				//					if (astr == appNumStr.GetAt(j))
				//						return FALSE;
				//				}
				//				appNumStr.Add(astr);
				m_str_fun.Add(astr);
				if (a1str != "(" && a1str != "+")
					return FALSE;
				if (afu1str != "*")
					return FALSE;
			}
			else if (strIsANum(astr))
			{
				if (a1str == ")" && afu1str == "-")
				{
					CTString aa = AnaStr.GetAt(i-3);
					if (!inselectlist( AnaStr.GetAt(i-3) ) )
						return FALSE;
					continue;
				}
				if (a1str != ")" || afu1str != "(")
					return FALSE;
				for (int j=0; j<m_CID_fun.GetSize(); j++)
				{
					if (astr == m_CID_fun.GetAt(j))
						return FALSE;
				}
				if (!inselectlist( AnaStr.GetAt(i-2) ) )
					m_CID_fun.Add(astr);
			}
			else //出现其他字符，报错
			{
				return FALSE;
			}
		}//遍历字符串中每个单个单元
	}
	else//如果不是出“c”,则为约束方程
	{
		m_bBeginwithC = FALSE;
	}
	if (alastStr == ")" && m_bBeginwithC)//最后一个是括号
		nparentheses--;
	if (nparentheses != 0)//检验括号是否配对
		return FALSE;
	
	return TRUE;
}


//////////////////////////////////////////////////
///////				主函数					//////
//////////////////////////////////////////////////

BOOL CSimuequation::SimEqu(CDoubleMatrix &mxY,  CDoubleMatrix &mxX,  CDoubleMatrix &mxP,  CDoubleMatrix &mxINP)
{
	int nrowP = mxP.mrow();
	int ncolP = mxP.mcol();
	int nrowX = mxX.mrow();
	int ncolY = mxY.mcol();
    m_SEqnumber = nrowP;
	m_vrG.create(nrowP,1);
	m_vrK.create(nrowP,1);
	
	//mxX是先决变量，mxY为内生变量
	//给出数据的调整，包括含有常数项，还需要添加滞后的情况	
	int nrowPtemp = nrowP;
	CDoubleMatrix mxUnit(nrowX,1,1.0);
	int i=0; 
	for (i=0; i<nrowPtemp; i++)
	{
		for (int j=0; j<ncolP; j++)
		{
			if (mxP(i,j)==0.5)
			{
				nrowPtemp = 0;
				break;
			}
		}
	}
	if (nrowPtemp==0)
	{
		mxX = mxUnit.commat(mxX,1);	
	}
	else
		mxP.delt(ncolY);
	ncolP = mxP.mcol();
	int nNSnum = ncolY;			//内生变量的个数
	int nXJnum = mxX.mcol();	//不包含滞后的先决变量的个数
	
	int m_lagNum = m_lagMatrix.mcol();	//滞后变量的个数
	
	m_vrLagVarNum.create(nrowP,0);
	
	
	//计算每一个方程所含的内生变量和先决变量（包含滞后）的个数,分别存入m_vrG,m_vrK。
	for (i=0;i<nrowP;i++)
	{
		
		CDoubleMatrix mxPtem = mxP.slice(i,0,1,ncolY);/////////////////////
		CDoubleVector vrSum= sum(mxPtem);
		m_vrG(i) = vrSum(0)+1;
		
		mxPtem = mxP.slice(i,ncolY,1,mxP.mcol()-ncolY);
		vrSum = sum(mxPtem);
		double  ki = vrSum(0);
		int kint = int(ki);
		if (ki>kint) 
		{
			ki = kint +1;
		}
		else
		{
			ki = kint;
		}
		m_vrK(i) = ki;
	}
	
	LagMaxTotal = 0;//最大滞后的阶数
	
	if (m_lagNum != 0)
	{
		LagMaxTotal = Max(m_ivLagNum);
		if (LagMaxTotal > 12)//最大滞后阶数不能超过12阶
		{
			CTString strWarning = "滞后阶数太大，请重新选择！";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		else	//根据最大滞后阶数对滞后数据矩阵进行修正
		{
			mxPLag = mxP.slice(0,nNSnum+nXJnum,nrowP,ncolP-nNSnum-nXJnum);
			CDoubleMatrix mxtempLag(nrowX-LagMaxTotal,mxPLag.mcol(),1.0);
			for(int k=0; k<mxPLag.mcol(); k++)
			{
				CDoubleMatrix mxLagTem ;
				mxLagTem = m_lagMatrix.slice(LagMaxTotal-m_ivLagNum(k),k,nrowX-LagMaxTotal,1);
				mxtempLag(k) = mxLagTem(0);
			}
			m_lagMatrix = mxtempLag;
			
		}
	}
	
	//对内生矩阵和先决矩阵进行修正
	mxY = mxY.slice(LagMaxTotal,0,nrowX-LagMaxTotal,ncolY);
	mxX = mxX.slice(LagMaxTotal,0,nrowX-LagMaxTotal,nXJnum);
	mxX = mxX | m_lagMatrix;
	
	CDoubleMatrix mxBeta_3SLS;
	//对m_coef_name按照m_ListVarArr变量出现的顺序进行排序：开始
	int ncountforcoef = 0;//暂时使用
	CTStringArray strTempAry;
	int k;
	for (i=0; i<nrowP; i++)
	{
		for (k=0; k<m_tVarInfo.iCount; k++)//先排列内生变量
			for (int j=ncountforcoef; j<ncountforcoef+m_vrG(i)+m_vrK(i)-1; j++)
				if (m_ListVarArr.GetAt(k) == m_coef_var.GetAt(j))
					strTempAry.Add(m_coef_name.GetAt(j));
				
				if (m_coef_var.GetAt(ncountforcoef) == "c")//再排常数的
					strTempAry.Add(m_coef_name.GetAt(ncountforcoef));
				
				for (k=m_tVarInfo.iCount; k<m_ListVarArr.GetSize(); k++)//再排先决变量以及滞后等
					for (int j=ncountforcoef; j<ncountforcoef+m_vrG(i)+m_vrK(i)-1; j++)
						if (m_ListVarArr.GetAt(k) == m_coef_var.GetAt(j))
							strTempAry.Add(m_coef_name.GetAt(j));
						ncountforcoef += m_vrG(i)+m_vrK(i)-1;
	}
	if (m_coef_name.GetSize() == strTempAry.GetSize())
		m_coef_name.Copy(strTempAry);
	else 
		return FALSE;
	//对m_coef_name按照m_ListVarArr变量出现的顺序进行排序：终止
	
	
	CDoubleVector vrUnit(nrowP,1);
	CDoubleVector vrTemp = m_vrG+m_vrK-vrUnit;
	m_dCoffTotal = sum(vrTemp);
	m_dCoeffMax = Max(vrTemp);	//给出单个方程含有的最多的未知数的个数
	
	//创建全局变量
	m_tOut.m_mxBeta.create(nrowP,m_dCoeffMax,0.0);
	m_tOut.m_mxSEReg.create(nrowP,m_dCoeffMax,0.0);
	m_tOut.m_mxTvalue.create(nrowP,m_dCoeffMax,0.0);
	m_tOut.m_mxPvalue.create(nrowP,m_dCoeffMax,0.0);
	
	m_tOut.m_vrSSE.create(nrowP,0.0);
	m_tOut.m_vrSST.create(nrowP,0.0);
	m_tOut.m_vrRsqu.create(nrowP,0.0);
	m_tOut.m_vrAdjR.create(nrowP,0.0);
	m_tOut.m_vrSSR.create(nrowP,0.0);
	m_tOut.m_vrSER.create(nrowP,0.0);
	m_tOut.m_vrDw.create(nrowP,0.0);
	m_tOut.m_vrSDD.create(nrowP,0.0);
	m_tOut.m_vrMD.create(nrowP,0.0);
	
	
	switch(m_SEmethod) 
	{
	case 0:
		if (!IV(mxX,  mxY, mxP, mxINP))					//调用工具变量法进行估计
		{
			CTString strWarning = "模型不适合用工具变量方法！";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		break;
	case 1:
		if (!ILS(mxX,  mxY, mxP, mxINP))				//调用间接最小二乘法进行估计
		{
			CTString strWarning = "模型不适合使用间接最小二乘方法！";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		break;
	case 2:
		if (!TwoLS(mxX,  mxY, mxP, mxINP))				//调用两阶段最小二乘进行估计
		{
			CTString strWarning = "模型不适合使用两阶段最小二乘方法！";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		break;
	case 3:
		if (!ThreeLS(mxX,  mxY, mxP, mxINP,mxBeta_3SLS))	//调用三阶段最小二乘法进行估计
		{
			CTString strWarning = "模型不适合使用三阶段最小二乘方法！";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		break;
	case 4:
		if (!FIML(mxX,  mxY, mxP, mxINP))				//调用全信息极大似然法进行估计
		{
			CTString strWarning = "模型不适合使用全信息极大似然方法！";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		break;
	default:
		{		
			CTString strWarning = "错误指令！";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
	}
	return TRUE;
}



///////////////////////////////////////////////////////////////////////
/////						工具变量法								///
///////////////////////////////////////////////////////////////////////
BOOL CSimuequation::IV(CDoubleMatrix &mxX,  CDoubleMatrix &mxY,  CDoubleMatrix &mxP,  CDoubleMatrix &mxINP)
{
	int nN=mxY.mrow();					//总观测个数（经滞后修正）
	int ng0=mxY.mcol();					//总内生变量个数
	int nEquation = mxP.mrow();			//总解释方程个数
	int nK1 = mxP.mcol()-ng0;			//总先决变量个数（含滞后变量）
	int idcont=0;
	
	//第i个方程的结果
	int i=1;
	for (i=1;i<=nEquation;i++)
	{
		
		//判断方程是否可以识别
		CTString strtemp ;
		strtemp.Format("%d",i); 
		
		
		int ki = m_vrK(i-1);
		int gi = m_vrG(i-1);
		
		if (!MODELTEST(i-1,mxP,mxINP,ng0,nK1))
		{
			if (nK1!=ki+gi-1)
			{
				CTString strWarning = "第"+strtemp+"个方程不能恰好识别!";
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
				idcont++;
			}
			else
			{
				CTString strWarning = "第"+strtemp+"个方程不能识别！";
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
				idcont++;
				continue;
			}	
			if (idcont!=nEquation)
			{
				continue;
			}
			else
			{
				return FALSE;
			}	
		}
		
		
		CDoubleVector vrYi = mxY(i-1);	//方程左边的内生变量的值
		
		CDoubleMatrix mxY0(nN,gi-1,0.0);	//方程右边的内生变量的观测值
		int l=0;
		int j=0;
		for (j=0;j<ng0;j++)
		{
			if (mxP(i-1,j)==1)
			{
			mxY0(l)=mxY(j);
			l++;
			}
		}
		
		
		CDoubleMatrix mxXi(nN,ki,0.0);		//方程右面的先决变量
		CDoubleVector vrUnit(nN,1);
		int nm=0;
		CDoubleVector vrmxX = mxX(0);
		CDoubleMatrix mxZi(vrmxX);
		l=0;
		CDoubleMatrix mxBi;
		CDoubleMatrix mxM;
		for (j=0;j<nK1;j++)
		{
			if (mxP(i-1,j+ng0)==1)
			{
				mxXi(l)=mxX(j);
				l++;
			}
			else if(mxP(i-1,j+ng0)==0.5)
			{
				mxXi(l)=vrUnit;
				l++;
			}
			else
			{
				if (nm==0)
				{
					mxZi(0) = mxX(j);
					nm++;
				}
				else
				{
					mxZi = mxZi.commat(mxX(j),1);
					nm++;
				}
				
			}
		}
		
		if (nm==0 || mxY0.mcol()==0)//当右侧没有内生变量时，不能使用
		{
			return FALSE;
		}
		
		if (nK1==ki+gi-1)	//模型恰好识别
		{
			mxM= mxY0 |mxXi;
			CDoubleMatrix mxX0 = mxZi.commat(mxXi,1);
			CDoubleMatrix mxEst = inv(mxX0.dot(mxM));
			CDoubleMatrix mxYi(vrYi);
			mxEst=mxEst*(mxX0.dot(mxYi));
			//还原参数
			if (gi>1)
			{
				mxBi = mxEst.slice(0,0,gi-1,1);
			}
			CDoubleMatrix mxGamai = mxEst.slice(gi-1,0,ki,1);
			if(!Paramtest(i,mxY0,vrYi,mxY0,mxXi,mxBi,mxGamai,nN,gi,ki))		//调用Paramtest函数进行参数检验
			{
				return FALSE;
			}
			
		}
	}
	return TRUE;
}

BOOL CSimuequation::ILS(CDoubleMatrix &mxX,  CDoubleMatrix &mxY,  CDoubleMatrix &mxP,  CDoubleMatrix &mxINP)
{
	int nN=mxY.mrow();
	int ng0=mxY.mcol();
	//	int ns=mxXINI.mrow();
	//	int nN = ng0;
	//	int nk0 = mxXINI.mcol();
	int nEquation = mxP.mrow();
	int nK1 = mxP.mcol()-ng0;
	int idcont=0;
	
	//	CDoubleMatrix mxX;
	//	CDoubleMatrix mxY;
	//把m_ivLagNum分解为各个方程的滞后，同时给出每个方程的滞后的最大值
	
	//	CDoubleMatrix mxLagIND(nEquation,mxP.mcol(),0);
	//	int iLag=0;
	//	CDoubleVector vrLagMax(nEquation,0);
	//	for (int i1=0; i1<nEquation; i1++)
	//	{
	//		for (int k1=0; k1<m_vrLagVarNum(i1); k1++)
	//		{
	//			mxLagIND(i1,k1)= m_ivLagNum(iLag);
	//			iLag++;
	//		}
	//		CDoubleVector vrTem = mxLagIND.row(i1);
	//		vrLagMax(i1)=Max(vrTem);
	//	}
	//	iLag=0;
	
	int i=1;
	for (i=1;i<=nEquation;i++)
	{
		//		nN = nn - vrLagMax(i-1);
		//		mxX = mxXINI.slice(vrLagMax(i-1),0,nN,nk0);
		//		mxY = mxYINI.slice(vrLagMax(i-1),0,nN,ng0);
		//		
		//		
		//		for (i1=0; i1<m_lagMatrix.mcol(); i1++)
		//		{
		//			CDoubleMatrix mxLagTemp;
		//			mxLagTemp = m_lagMatrix.slice(vrLagMax(i-1)-mxLagIND(i-1,i1),i1,nN,1);
		//			mxX = mxX.commat(mxLagTemp,1);
		//		}
		//		
		CTString strtemp ;
		strtemp.Format("%d",i); 
		
		int ki = m_vrK(i-1);
		int gi = m_vrG(i-1);
		
		if (!MODELTEST(i-1,mxP,mxINP,ng0,nK1))
		{
			if (nK1!=ki+gi-1)
			{
				CTString strWarning = "第"+strtemp+"个方程不能恰好识别!";
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
				idcont++;
			}
			else
			{
				CTString strWarning = "第"+strtemp+"个方程不能恰好识别!";
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
				idcont++;
				continue;
			}
			if (idcont!=nEquation)
			{
				continue;
			}
			else
			{
				return FALSE;
			}	
		}
		
		
		if (gi==1)
		{
			m_bWarning = 1;
			return FALSE;
		}
		CDoubleVector vrYi = mxY(i-1);
		
		CDoubleMatrix mxY0(nN,gi-1,0.0);
		int l=0;
		int j=0;
		for (j=0;j<ng0;j++)
		{
			if (mxP(i-1,j)==1)
			{
				mxY0(l)=mxY(j);
				l++;
			}
		}
		CDoubleMatrix mxYe = vrYi|mxY0;
		
		CDoubleMatrix mxXi(nN,ki,0.0);
		CDoubleVector vrUnit(nN,1.0);
		
		CDoubleMatrix mxXLose;
		l=0;
		for (j=0;j<nK1;j++)
		{
			if (mxP(i-1,j+ng0)==1)
			{
				mxXi(l)=mxX(j);
				l++;
			}
			else if(mxP(i-1,j+ng0)==0.5)
			{
				mxXi(l)=vrUnit;
				l++;
			}
			else if (mxP(i-1,j+ng0)==0)
			{
				mxXLose = mxXLose | mxX(j);
			}
		}
		
		CDoubleMatrix mxZi=mxY0 | mxXi;
		if (nK1!=ki+gi-1)
		{
			return FALSE;
		}
		
		CDoubleMatrix mxYi(vrYi);
		CDoubleMatrix mxXe=mxXi | mxXLose;
		CDoubleMatrix mxEst = inv(mxXe.dot(mxZi))*(mxXe.dot(mxYi));
		
		CDoubleMatrix mxBi;
		if (gi>1)
		{
			mxBi = mxEst.slice(gi-1,1);
		}
		CDoubleMatrix mxGamai = mxEst.slice(gi-1,0,ki,1);
		
		if(!Paramtest(i,mxY0,vrYi,mxY0,mxXi,mxBi,mxGamai,nN,gi,ki))
		{			
			return FALSE;
		}
	}
	return TRUE;
}




BOOL CSimuequation::TwoLS(CDoubleMatrix &mxX,  CDoubleMatrix &mxY,  CDoubleMatrix &mxP,  CDoubleMatrix &mxINP)
{
	int nN=mxY.mrow();
	int ng0=mxY.mcol();
	//	int nk0 = mxXINI.mcol();
	//	int nN = mxP.mrow();
	int nK1 = mxP.mcol()-ng0;
	int idcont=0;
	int nEquation = mxP.mrow();
	
	//	CDoubleMatrix mxX;
	//	CDoubleMatrix mxY;
	//把m_ivLagNum分解为各个方程的滞后，同时给出每个方程的滞后的最大值
	
	//	CDoubleMatrix mxLagIND(nEquation,mxP.mcol(),0);
	//	int iLag=0;
	
	//	for (int i1=0; i1<nEquation; i1++)
	//	{
	//		for (int k1=0; k1<m_vrLagVarNum(i1); k1++)
	//		{
	//			mxLagIND(i1,k1)= m_ivLagNum(iLag);
	//			iLag++;
	//		}
	//	}
	//	double LagMaxTotal;
	//	if (m_lagMatrix.mcol()!=0)
	//	{
	//		LagMaxTotal=Max(m_ivLagNum);
	//	}
	//	else
	//		LagMaxTotal = 0;
	//	iLag=0;
	
	//	nN = nn - LagMaxTotal;
	//	mxX = mxXINI.slice(LagMaxTotal,0,nN,nk0);
	//	mxY = mxYINI.slice(LagMaxTotal,0,nN,ng0);
	//	
	//	if (m_lagMatrix.mcol()!=0)
	//	{
	//		for (int i1=0; i1<m_lagMatrix.mcol(); i1++)
	//		{
	//			CDoubleMatrix mxLagTemp;
	//			mxLagTemp = m_lagMatrix.slice(LagMaxTotal-m_ivLagNum(i1),i1,nN,1);
	//			mxX = mxX.commat(mxLagTemp,1);
	//		}
	//	}
	int i=1;
	for (i=1;i<=nEquation;i++)
	{
		int ki = m_vrK(i-1);
		int gi = m_vrG(i-1);
		
		CDoubleVector vrYi = mxY(i-1);
		
		CDoubleMatrix mxY0(nN,gi-1,0.0);
		int l=0;
		int j=0;
		for (j=0;j<ng0;j++)
		{
			if (mxP(i-1,j)==1)
			{
				mxY0(l)=mxY(j);
				l++;
			}
		}
		
		CDoubleMatrix mxXi(nN,ki,0.0);
		CDoubleVector vrUnit(nN,1.0);
		int nm=0;
		CDoubleVector vrmxX = mxX(0);
		CDoubleMatrix mxZi(vrmxX);
		l=0;
		for (j=0;j<nK1;j++)
		{
			if (mxP(i-1,j+ng0)==1)
			{
				mxXi(l)=mxX(j);
				l++;
			}
			else if(mxP(i-1,j+ng0)==0.5)
			{
				mxXi(l)=vrUnit;
				l++;
			}
		}
		CDoubleMatrix mxXa(mxX);
		if (mxX(0)!=vrUnit)
		{
			CDoubleMatrix mxUnit (vrUnit);
			mxXa = mxUnit | mxX;
		}
		CDoubleMatrix mxEstY0;
		
		//第一阶段最小二乘
		if (gi>1)
		{
			mxEstY0 = inv(mxXa.dot(mxXa))*(mxXa.dot(mxY0));
			mxEstY0 = mxXa*mxEstY0;
		}
		
		//第二阶段最小二乘
		mxXa = mxEstY0 | mxXi;
		
		CDoubleMatrix mxYi(vrYi);
		CDoubleMatrix mxEst = inv(mxXa.dot(mxXa))*(mxXa.dot(mxYi));
		
		CDoubleMatrix mxBi;
		if (gi>1)
		{
			mxBi  = mxEst.slice(gi-1,1);
		}
		
		CDoubleMatrix mxGamai = mxEst.slice(gi-1,0,ki,1);
		if (!Paramtest(i,mxEstY0,vrYi,mxY0,mxXi,mxBi,mxGamai,nN,gi,ki))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CSimuequation::ThreeLS(CDoubleMatrix &mxX,  CDoubleMatrix &mxY,  CDoubleMatrix &mxP,  CDoubleMatrix &mxINP,CDoubleMatrix &mxBeta1)
{
	int nN=mxY.mrow();
	int ng0=mxY.mcol();
	//	int nk0 = mxXINI.mcol();
	//	int nN = mxP.mrow();
	int nK1 = mxP.mcol()-ng0;
	int idcont=0;
	int nEquation = mxP.mrow();
	
	//	CDoubleMatrix mxX;
	//	CDoubleMatrix mxY;
	//把m_ivLagNum分解为各个方程的滞后，同时给出每个方程的滞后的最大值
	
	//	CDoubleMatrix mxLagIND(nEquation,mxP.mcol(),0);
	//	int iLag=0;
	//
	//	for (int i1=0; i1<nEquation; i1++)
	//	{
	//		for (int k1=0; k1<m_vrLagVarNum(i1); k1++)
	//		{
	//			mxLagIND(i1,k1)= m_ivLagNum(iLag);
	//			iLag++;
	//		}
	//
	//	}
	
	//	double LagMaxTotal=Max(m_ivLagNum);
	//	iLag=0;
	//
	//	nN = nn - LagMaxTotal;
	//	mxX = mxXINI.slice(LagMaxTotal,0,nN,nk0);
	//	mxXINI.destroy();
	//	mxY = mxYINI.slice(LagMaxTotal,0,nN,ng0);
	//	mxYINI.destroy();
	//	
	//	for (int i1=0; i1<m_lagMatrix.mcol(); i1++)
	//	{
	//		CDoubleMatrix mxLagTemp;
	//		mxLagTemp = m_lagMatrix.slice(LagMaxTotal - m_ivLagNum(i1),i1,nN,1);
	//		mxX = mxX.commat(mxLagTemp,1);
	//	}
	
	
	CDoubleVector vrUtemp(nEquation,1);
	double nD = 0;
	CDoubleVector vrTePar = m_vrG +m_vrK - vrUtemp;
	nD = sum(vrTePar);
	
	CDoubleMatrix mxUnit(nN,1,1.0);
	CDoubleMatrix mxZ(nEquation*nN,nD,0.0);
	//	CDoubleMatrix mxZ1(nEquation*nN,nD,0);/////////
	CDoubleMatrix mxYini(nEquation*nN,1,0.0);
	CDoubleVector vrIND(nEquation+1,1.0);
	CDoubleMatrix mxE(nN,nEquation);
	CDoubleVector vrYi;
	int l;
	CDoubleMatrix mxBi;
	CDoubleMatrix mxEst;
	
	CDoubleMatrix mxEstYi;			//每个方程中右面内生变量的预测值组成的大矩阵
	int i=0; 
	for (i=0; i<nEquation; i++)
	{
		int ki = m_vrK(i);
		int gi = m_vrG(i);
		
		vrYi = mxY(i);
		CDoubleMatrix mxYi(vrYi);
		CDoubleMatrix mxY0(nN,m_vrG(i)-1,0.0);
		l=0;
		int j=0;
		for (j=0; j<ng0; j++)
		{
			if (mxP(i,j)==1)
			{
				mxY0(l)=mxY(j);
				l=l+1;
			}
		}
		
		for (j=0; j<nN;j++)
		{
			mxYini(i*nN+j,0)=vrYi(j);
		}
		
		
		CDoubleMatrix mxXi(nN,m_vrK(i),0.0);
		l=0;
		for (j=0; j<nK1; j++)
		{
			if (mxP(i,j+ng0)==1)
			{
				mxXi(l)=mxX(j);
				l++;
			}
			else if (mxP(i,j+ng0)==0.5)
			{
				mxXi(l)=mxUnit(0);
				l++;
			}
		}
		
		CDoubleMatrix mxXa;
		if (mxX(0)!=mxUnit(0))
		{
			mxXa=mxUnit.commat(mxX,1);
		}
		else
			mxXa=mxX;
		//第一阶段最小二乘
		CDoubleMatrix mxEstY0;
		if (m_vrG(i)>1)
		{
			mxEstY0= mxXa*(inv(mxXa.dot(mxXa))*(mxXa.dot(mxY0)));
			mxEstYi = mxEstYi | mxEstY0;
		}
		
		//第二阶段最小二乘
		
		
		CDoubleMatrix mxEsttemp = mxEstY0 | mxXi;
		CDoubleMatrix mxEst = inv(mxEsttemp.dot(mxEsttemp))*(mxEsttemp.dot(mxYi));
		
		if (m_vrG(i)>1)
		{
			mxBi = mxEst.slice(0,0,m_vrG(i)-1,1);
		}
		
		CDoubleMatrix mxGamai = mxEst.slice(m_vrG(i)-1,0,m_vrK(i),1);
		
		CDoubleMatrix mxBiY0;
		CDoubleMatrix mxXiGai;
		CDoubleMatrix mxZtemp;
		
		if (mxBi.mcol()==0 || mxY0.mcol()==0)
		{
		}
		else 
		{
			mxBiY0=mxY0*mxBi;
		}	
		if (mxGamai.mcol()==0 || mxXi.mcol()==0)
		{
		}
		else
		{
			mxXiGai=mxXi*mxGamai;
		}
		
		//		CDoubleMatrix mxZtemp2 ;		/////	
		CDoubleMatrix mxEsY;
		if (mxBi.mcol()==0 || mxY0.mcol()==0)
		{
			mxEsY= mxXiGai;
			mxZtemp = mxXi;
			//			mxZtemp2 = mxXi;			/////
		}
		else if (mxGamai.mcol()==0 || mxXi.mcol()==0)
		{
			mxEsY = mxBiY0;
			mxZtemp = mxY0 ;
			//			mxZtemp2 = mxEstY0;			/////
			}
		else
		{
			mxEsY = mxBiY0 +mxXiGai;
			mxZtemp = mxY0 |mxXi;
			//			mxZtemp2 = mxEstY0 | mxXi;////
		}
		
		
		for (j=0; j<nN; j++)
		{
			mxE(j,i) = vrYi(j)-mxEsY(j,0);
		}
		
		vrIND(0)=0;
		vrIND(i+1)=m_vrK(i)+m_vrG(i)-1;
		
		
		for (j=0; j<nN; j++)
		{
			for(int k=0; k<m_vrK(i)+m_vrG(i)-1; k++)
			{
				mxZ(i*nN+j,vrIND(i)+k) = mxZtemp(j,k);
				//				mxZ1(i*nN+j,vrIND(i)+k) = mxZtemp2(j,k);//////
			}
		}
		
		vrIND(i+1)=vrIND(i+1)+vrIND(i);
	}
	
	
	CDoubleMatrix mxSigma = mxE.dot(mxE);
	//第三步 估计3-SLS
	CDoubleMatrix mxZtUZ;//(nD,nD,0);
	CDoubleMatrix mxZtUY;//(nD,1,0);
	int Point1 = 0;
	
	CDoubleMatrix mxtempZ3;
	CDoubleMatrix mxtempZ4;
	CDoubleMatrix mxtempZ5;
	
	if (nEquation!=1)
	{
		CDoubleMatrix mxDelta = inv(mxX.dot(mxX));
		CDoubleMatrix mxUtemp1 = inv(mxSigma);
		int j=0;
		for (j=0; j<nEquation; j++)
		{
			CDoubleMatrix mxZtemp1 = mxZ.slice(j*nN,Point1,nN,vrTePar(j));
			
			int Point2 = 0;
			
			int k=0;
			for (k=0; k<nEquation; k++)
			{
				CDoubleMatrix mxtempZ1;
				CDoubleMatrix mxZtemp2 = mxZ.slice(k*nN,Point2,nN,vrTePar(k));
				mxtempZ1 = mxZtemp1.dot(mxX)*mxDelta*(mxX.dot(mxZtemp2))*mxUtemp1(j,k);
				mxtempZ3 = mxtempZ3 | mxtempZ1;
				
				mxtempZ4 = mxYini.slice(k*nN,0,nN,1);
				mxtempZ1 = mxZtemp1.dot(mxX)*mxDelta*(mxX.dot(mxtempZ4))*mxUtemp1(j,k);
				if (mxtempZ5.mcol()==0)
				{
					mxtempZ5 = mxtempZ1;
				}else
					mxtempZ5 = mxtempZ5 + mxtempZ1;
				
				Point2 = Point2 + vrTePar(k);
			}
			mxZtUZ = mxZtUZ.commat(mxtempZ3,2);
			mxtempZ3.destruction();
			
			mxZtUY = mxZtUY.commat(mxtempZ5,2);
			mxtempZ5.destruction();
			
			Point1 = Point1 + vrTePar(j); 
		}
		
		mxEst = inv(mxZtUZ)*mxZtUY;
		
		//		CDoubleMatrix mxUtemp2 = mxX*inv(mxX.dot(mxX))*mxX.trans();
		//		CDoubleMatrix mxU = Kproduct(mxUtemp1,mxUtemp2);
		//		mxUtemp1 = mxZ.dot(mxU)*mxZ;
		//		mxEst = inv(mxUtemp1)*mxZ.dot(mxU)*mxYini;
	}
	else
		m_bWarning = 1;
	
	int index = 0;
	
	
	for (i=0; i<nEquation; i++)
	{
		vrYi = mxY(i);
		CDoubleMatrix mxY0new(nN,m_vrG(i)-1,0.0);
		CDoubleMatrix mxEstYinew(nN,m_vrG(i)-1,0.0);
		l=0;
		int j=0;
		for (j=0; j<ng0; j++)
		{
			if (mxP(i,j)==1)
			{
				mxY0new(l)=mxY(j);
				mxEstYinew(l)=mxEstYi(index);
				l++;
				index++;
			}
		}
		
		//		CDoubleMatrix mxXi;
		CDoubleMatrix mxXinew(nN,m_vrK(i),0.0);
		l=0;
		for (j=0; j<nK1; j++)
		{
			if (mxP(i,j+ng0)==1)
			{
				mxXinew(l)=mxX(j);
				l++;
			}
			else if (mxP(i,j+ng0)==0.5)
			{
				mxXinew(l)=mxUnit(0);
				l++;
			}
		}
		
		CDoubleMatrix mxBinew;
		if (m_vrG(i)>1)
		{
			mxBinew = mxEst.slice(vrIND(i),0,m_vrG(i)-1,1);
		}
		
		CDoubleMatrix mxGamainew;
		if (m_vrK(i)>0)
		{
			mxGamainew = mxEst.slice(vrIND(i)+m_vrG(i)-1,0,m_vrK(i),1);
		}
		
		if (!Paramtest(i+1,mxEstYinew,vrYi,mxY0new,mxXinew,mxBinew,mxGamainew,nN,m_vrG(i),m_vrK(i)))
		{		
			return FALSE;
		}
		mxBeta1 = mxEst;
	}
	
	return TRUE;
}


BOOL CSimuequation::FIML(CDoubleMatrix &mxX,  CDoubleMatrix &mxY,  CDoubleMatrix &mxP,  CDoubleMatrix &mxINP)
{
	int ng0 = mxY.mcol();
	//	int nk0 = mxX.mcol();
	int nN = mxY.mrow();
	int nEquNum = mxP.mrow();

	if (nEquNum == 1 || ng0<nEquNum)
	{
		CTString strWarning = "方程个数太少，不适合使用全信息极大似然！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	int nK1 = mxP.mcol()-ng0;
	CDoubleMatrix mxStart;
	
	//	CDoubleMatrix mxX;
	//	CDoubleMatrix mxY;
	//把m_ivLagNum分解为各个方程的滞后，同时给出每个方程的滞后的最大值
	
	//	CDoubleMatrix mxLagIND(nEquNum,mxP.mcol(),0);
	//	int iLag=0;
	//	CDoubleVector vrLagMax(nEquNum,0);
	//	
	//	for (int i1=0; i1<nEquNum; i1++)
	//	{
	//		for (int k1=0; k1<m_vrLagVarNum(i1); k1++)
	//		{
	//			mxLagIND(i1,k1)= m_ivLagNum(iLag);
	//			iLag++;
	//		}
	//		CDoubleVector vrTem = mxLagIND.row(i1);
	//		vrLagMax(i1)=Max(vrTem);
	//	}
	//	double LagMaxTotal=Max(m_ivLagNum);
	//	iLag=0;
	
	//	int nN = nn - LagMaxTotal;
	//	mxX = mxXINI.slice(LagMaxTotal,0,nN,nk0);
	
	//	mxY = mxYINI.slice(LagMaxTotal,0,nN,ng0);
	
	
	//	for (int i1=0; i1<m_lagMatrix.mcol(); i1++)
	//	{
	//		CDoubleMatrix mxLagTemp;
	//		mxLagTemp = m_lagMatrix.slice(LagMaxTotal - m_ivLagNum(i1),i1,nN,1);
	//		mxX = mxX.commat(mxLagTemp,1);
	//	}
	
	CDoubleMatrix mxBeta1;
	if (ThreeLS(mxX,mxY,mxP,mxINP,mxBeta1))
	{
		mxStart = mxBeta1;
	}
	else
		return FALSE;
	//
	//	mxXINI.destroy();
	//	mxYINI.destroy();
	TraceMatrix(mxStart);
	double neldtol = 1e-7;
	CDoubleVector vrBfiml;
	vrBfiml=Nelder(mxStart,2000,0,neldtol,0,mxX,mxY);
	
	CDoubleMatrix mxBfiml(vrBfiml);
	CDoubleVector vrUnit(nN,1);
	int l=0;
	
	int index1=0; 
	int i=0; 
	for (i=0; i<nEquNum; i++)
	{
		CDoubleVector vrYi = mxY(i);
		CDoubleMatrix mxY0(nN,m_vrG(i)-1,0.0);
		l=0;
		int j=0;
		for (j=0; j<ng0; j++)
		{
			if (mxP(i,j)==1)
			{
				mxY0(l)=mxY(j);
				l=l+1;
			}
		}
		
		
		CDoubleMatrix mxXi(nN,m_vrK(i),0.0);
		CDoubleMatrix mxBi;
		l=0;
		for (j=0; j<nK1; j++)
		{
			if (mxP(i,j+ng0)==1)
			{
				mxXi(l)=mxX(j);
				l++;
			}
			else if (mxP(i,j+ng0)==0.5)
			{
				mxXi(l)=vrUnit;
				l++;
			}
		}
		if (m_vrG(i)>1)
		{
			mxBi = mxBfiml.slice(index1,0,m_vrG(i)-1,1);
		}
		
		CDoubleMatrix mxGamai = mxBfiml.slice(index1+m_vrG(i)-1,0,m_vrK(i),1);
		if (!Paramtest(i+1,mxY0,vrYi,mxY0,mxXi,mxBi,mxGamai,nN,m_vrG(i),m_vrK(i)))
		{
			return FALSE;
		}
		index1 = m_vrG(i)+m_vrK(i)-1;
	}
	
	return TRUE;
}

CDoubleVector CSimuequation::Nelder(CDoubleMatrix &theta,int itlimit,double neldout,double ftol,double scaleit,CDoubleMatrix &mxXmat,CDoubleMatrix &mxYmat)
{
	
	if (scaleit==0)
	{
		scaleit=0.1;
	}
	if (ftol==0)
	{
		ftol=1e-14;
	}
	
	int ntheta_Size = theta.mrow();
	CDoubleVector vrtheta0(ntheta_Size,0);
	int k=0; 
	for (k=0; k<ntheta_Size; k++)
	{
		if (theta(k,0)==0)
		{
			vrtheta0(k)=1;
		}
	}
	CDoubleVector vrChanges = theta(0)*scaleit+vrtheta0*scaleit;
	double alpha=1.0;
	double beta=0.5;
	double gam = 2.0;
	
	int npar = theta.mrow();
	double hi = npar+1.0;
	
	CDoubleMatrix mxParnew=eye(npar);
	
	
	for (k=0; k<npar; k++)
	{
		mxParnew(k,k) = vrChanges(k);
		mxParnew(k) = mxParnew(k) + theta(0);
	}
	
	CDoubleMatrix mxPar = theta | mxParnew;
	
	int ii=0;
	CDoubleMatrix mxYnew(hi,1,0.0);
	
	while (ii<hi)
	{
		CDoubleVector vrParii=mxPar(ii);
		double dtemp = logfiml(vrParii,mxYmat,mxXmat);
		mxYnew(ii,0)=dtemp;
		ii++;
	}
	mxParnew = mxPar.trans();
	CDoubleMatrix mxYpar=mxYnew | mxParnew;
	int nRowtemp = mxYpar.mrow();
	int ncoltemp = mxYpar.mcol();
	
	for (ii=0; ii<ncoltemp; ii++)
	{
		CDoubleVector vrParii = mxYpar(ii);
		CDoubleVector vrTemp = quicksort(vrParii,0,nRowtemp-1,1);
		mxYpar(ii)=vrTemp;
	}
	CDoubleVector vrY=mxYpar(0);
	mxPar = mxYpar.slice(0,1,nRowtemp,hi-1);
	CDoubleMatrix mxX0 = mxPar.slice(0,0,npar,hi-1);
	CDoubleVector vrX0 = mean(mxX0);
	
	int indxmax = hi;
	int indxmax2 = hi - 1;
	int indxmin = 1;
	double alphap1 = alpha + 1;
	double onemgam = 1 - gam;
	double onembeta = 1 - beta;
	int iter =1;
	
	
	CDoubleVector vrXe;
	double mXe;
	
	CDoubleVector vrXr;
	double mXr ;
	
	int xyindex;
	CDoubleVector vrXc;
	double mXc;
	double rtol = 2.0*::fabs(vrY(hi-1)-vrY(0))/(::fabs(vrY(hi-1))+::fabs(vrY(0)));
	
	TraceMatrix(theta);
	CDoubleVector vrtemppp(10000,0);
	CDoubleVector vrtempppp(10000,0);
	double cert;
	
	while (iter<=itlimit)
	{
		CDoubleVector vrtemp = mxPar.row(indxmax-1);
		vrXr = alphap1*vrX0 - alpha*vrtemp;
		mXr = logfiml(vrXr,mxYmat,mxXmat);
		
		
		if (mXr - vrY(indxmin-1) < 0)
		{
			xyindex = 1;
		}
		else if(vrY(indxmax2-1) - mXr >=0)
		{
			xyindex = 2;
		}
		else if( mXr - vrY(indxmax-1) >= 0)
		{
			CDoubleVector vrtemp3 = mxPar.row(indxmax-1);
			vrXc =  beta*vrtemp3  +  onembeta*vrX0 ;
			mXc = logfiml(vrXc,mxYmat,mxXmat);
			if (mXc - mXr >=0  || fabs(mXc - mXr) < DOUBLE_ZERO) 
			{
				xyindex = 3;
			}
			else
			{
				xyindex = 4;
			}
		}
		else
		{
			for (ii=0; ii<vrXr.vlen(); ii++)
			{
				mxPar(indxmax-1,ii) = vrXr(ii);
			}
			vrY(indxmax-1) = mXr;
			vrtemp = mxPar.row(indxmax-1);
			vrXc = beta*vrtemp+onembeta*vrX0;
			mXc = logfiml(vrXc,mxYmat,mxXmat);
			if (mXc - vrY(indxmax-1) >= 0)
			{
				xyindex = 5 ;
			}
			else
			{
				xyindex = 6;
			}
		}
		
		vrtemppp(iter-1)=xyindex;////////////
		
		switch(xyindex)
		{
		case 1:
			vrXe = gam*vrXr + onemgam*vrX0;
			mXe = logfiml(vrXe,mxYmat,mxXmat);
			if (mXe - vrY(indxmin-1) < 0)
			{
				vrY(indxmax-1) = mXe;
				for (ii=0; ii<vrXe.vlen(); ii++)
				{
					mxPar(indxmax-1,ii) = vrXe(ii);
				}
				
			}
			else
			{
				vrY(indxmax-1) = mXr;
				for (ii=0; ii<vrXr.vlen(); ii++)
				{
					mxPar(indxmax-1,ii) = vrXr(ii);
				}
			}
			break;
		case 2:
			vrY(indxmax-1)=mXr;
			for (ii=0; ii<vrXr.vlen(); ii++)
			{
				mxPar(indxmax-1,ii) = vrXr(ii);
			}
			break;
		case 3:
			vrXc = beta*mxPar.row(indxmax-1)+onembeta*vrX0;
			mXc = logfiml(vrXc,mxYmat,mxXmat);
			for (ii=0; ii<mxPar.mrow(); ii++)
			{
				for (int kk=0; kk<mxPar.mcol(); kk++)
				{
					mxPar(ii,kk)=0.5*mxPar(ii,kk)+mxPar(indxmin-1,kk);
				}
			}
			
			ii=1;
			while (ii<=hi)
			{
				CDoubleVector vrtemp1 = mxPar.row(ii-1);
				double ftemp=logfiml(vrtemp1,mxYmat,mxXmat);
				vrY(ii-1)=ftemp;
				ii++;
			}
			break;
		case 4:
			vrXc = beta*mxPar.row(indxmax-1)+onembeta*vrX0;
			mXc = logfiml(vrXc,mxYmat,mxXmat);
			for (ii=0; ii<vrXc.vlen(); ii++)
			{
				mxPar(indxmax-1,ii) = vrXc(ii);
			}
			vrY(indxmax-1)=mXc;
			break;
		case 5:
			for (ii=0; ii<vrXc.vlen(); ii++)
			{
				mxPar(indxmax-1,ii) = vrXr(ii);
			}
			vrY(indxmax-1)=mXr;
			vrXc = beta*mxPar.row(indxmax-1)+onembeta*vrX0;
			mXc = logfiml(vrXc,mxYmat,mxXmat);
			for (ii=0; ii<mxPar.mrow(); ii++)
			{
				for (int kk=0; kk<mxPar.mcol(); kk++)
				{
					mxPar(ii,kk)=0.5*mxPar(ii,kk)+mxPar(indxmin-1,kk);
				}
			}
			
			ii=1;
			while (ii<=hi)
			{
				CDoubleVector vrtemp3 = mxPar.row(ii-1);
				double ftemp = logfiml(vrtemp3, mxYmat, mxXmat);
				vrY(ii-1) = ftemp;
				ii++;
			}
			break;
		case 6:
			for (ii=0; ii<vrXr.vlen(); ii++)
			{
				mxPar(indxmax-1,ii) = vrXr(ii);
				}
			vrY(indxmax-1)=mXr;
			vrXc = beta*mxPar.row(indxmax-1)+onembeta*vrX0;
			mXc = logfiml(vrXc,mxYmat,mxXmat);
			for (ii=0; ii<vrXc.vlen(); ii++)
			{
				mxPar(indxmax-1,ii) = vrXc(ii);
			}
			vrY(indxmax-1)=mXc;
			break;
		default:
			;
		}
		rtol = 2.0*::fabs(vrY(indxmax-1)-vrY(indxmin-1))/::fabs(vrY(indxmax-1)+vrY(indxmin-1));
		
		vrtempppp(iter-1)=rtol;
		if (rtol<ftol)
		{
			break;
		}
		else
		{
			double dtemp1;
			dtemp1 = Max(vrY,indxmax);
			//			TraceVector(vrY);
			dtemp1 = Min(vrY,indxmin);
			
			CDoubleVector vrdelY = vrY;
			vrdelY.remove(indxmax);
			dtemp1 = Max(vrdelY,indxmax2);
			
			if (indxmax2>=indxmax)
			{
				indxmax2 = indxmax2+1;
			}
			
			CDoubleMatrix mxdelpar = mxPar;
			mxdelpar = mxdelpar.trans();
			mxdelpar.delt(indxmax);
			mxdelpar = mxdelpar.trans();
			
			vrX0 = mean(mxdelpar);
			indxmax++;
			indxmin++;
			indxmax2++;
		}
		cert = rtol;
		iter ++;
		CDoubleVector vrtemp6 = mxPar.row(indxmin-1);
		TraceVector(vrtemp6);
	}
	CDoubleVector vrtemp5 = mxPar.row(indxmin-1);
	TraceVector(vrtemppp);
	TraceVector(vrtempppp);
	return vrtemp5;
}

double CSimuequation::logfiml(CDoubleVector &vrBetai, CDoubleMatrix &mxYmat,CDoubleMatrix & mxXmat)
{
	int ns = mxYmat.mcol();
	int nn = mxYmat.mrow();
	int na = m_mxP.mrow();
	int nb = m_mxP.mcol();
	
	CDoubleMatrix mxPP = m_mxP.copy();
	int nq=0;
	int j=0; 
	for (j=0; j<na; j++)
	{
		for (int k=0; k<nb; k++)
		{
			if(mxPP(j,k)==1 || mxPP(j,k)==0.5)
			{
				mxPP(j,k)=vrBetai(nq);
				nq=nq+1;
			}
		}
		mxPP(j,j)=-1;
	}
	CDoubleMatrix mxGama = mxPP.slice(0,0,mxPP.mrow(),ns);
	mxGama = mxGama.trans();
	
	CDoubleMatrix mxBeta = mxPP.slice(0,ns,mxPP.mrow(),nb-ns);
	mxBeta = mxBeta.trans();
	
	CDoubleMatrix mxResid = -mxYmat*mxGama - mxXmat*mxBeta;
	CDoubleMatrix mxSighat = mxResid.dot(mxResid)/(nn*1.0);
	
	
	CDoubleMatrix mxGamanew = mxGama.slice(na,na);
	CComplex t1 = det(mxGamanew);
	//CComplex t2 = det(mxGamanew);
	
	double LLL = 0.5*log(det(mxSighat))-log(abs(t1));
	
	return LLL;
}

BOOL CSimuequation::Paramtest(int i, CDoubleMatrix &mxYest,CDoubleVector &vrYi,  CDoubleMatrix &mxY0, CDoubleMatrix &mxXi,  CDoubleMatrix &mxBi,CDoubleMatrix &mxGamai,int nn,int gi,int ki)
{
	CDoubleMatrix mxBiY0;
	int nCoffi;
	CDoubleMatrix mxXiGai;
	if (mxBi.mcol()==0 || mxY0.mcol()==0)
	{
	}
	else 
	{
		mxBiY0 = mxY0*mxBi;
		for (nCoffi=0; nCoffi<gi-1; nCoffi++)
		{
			m_tOut.m_mxBeta(i-1,nCoffi) = mxBi(nCoffi,0);
		}		
	}
	
	if (mxGamai.mcol()==0 || mxXi.mcol()==0)
	{
	}
	else
	{
		mxXiGai = mxXi*mxGamai;
		for (nCoffi=0; nCoffi<ki; nCoffi++)
		{
			m_tOut.m_mxBeta(i-1,nCoffi+gi-1) = mxGamai(nCoffi,0);
		}		
		
	}
	CDoubleMatrix mxESY ;
	
	if(mxBi.mcol()==0 || mxY0.mcol()==0)
	{
		mxESY=mxXiGai;
	}
	else if(mxGamai.mcol()==0 || mxXi.mcol()==0)
	{
		mxESY = mxBiY0;
	}
	else
	{
		mxESY = mxBiY0+mxXiGai;
	}
	
	int nEquation = m_mxP.mrow();
	
	int df = gi+ki-1;
	CDoubleMatrix mxYi(vrYi);
	CDoubleMatrix mxRes = mxYi - mxESY;
	mxESY.destroy();
	CDoubleMatrix mxRestemp = mxRes.dot(mxRes);
	m_tOut.m_vrSSE(i-1)=mxRestemp(0,0);
	
	m_tOut.m_vrSST(i-1)=(nn-1)*var(vrYi);
	m_tOut.m_vrMD(i-1) = mean(vrYi);	
	m_tOut.m_vrSDD(i-1) = st_d(vrYi);
	
	m_tOut.m_vrRsqu(i-1)=1-m_tOut.m_vrSSE(i-1)/m_tOut.m_vrSST(i-1);
	
	
	m_tOut.m_vrAdjR(i-1)=1-(m_tOut.m_vrSSE(i-1)*(nn-1))/(m_tOut.m_vrSST(i-1)*(nn-df));
	m_tOut.m_vrSSR(i-1)=m_tOut.m_vrSST(i-1)-m_tOut.m_vrSSE(i-1);
	m_tOut.m_vrSER(i-1)=sqrt(m_tOut.m_vrSSE(i-1)/(nn-df));
	
	int j=1;
	for (j=1; j<nn; j++)
	{
		m_tOut.m_vrDw(i-1)=m_tOut.m_vrDw(i-1)+pow(mxRes(j,0)-mxRes(j-1,0),2);
	}
	mxRes.destroy();
	m_tOut.m_vrDw(i-1) = m_tOut.m_vrDw(i-1)/m_tOut.m_vrSSE(i-1);
	
	
	CDoubleMatrix mxCX;
	CDoubleMatrix mxY0Xi;
	if (gi==1)
	{
		mxCX = inv(mxXi.dot(mxXi));
	}
	else if (ki==0)
	{
		mxCX = inv(mxYest.dot(mxYest));
	}
	else
	{
		mxY0Xi = mxYest.commat(mxXi,1);
		mxCX = inv(mxY0Xi.dot(mxY0Xi));
		
	}
	
	for (j=0; j<df; j++)
	{
		m_tOut.m_mxSEReg(i-1,j)=sqrt(mxCX(j,j)*m_tOut.m_vrSSE(i-1)/(nn-df));
	}
	
	
	for (j=0; j<df; j++)
	{
		m_tOut.m_mxTvalue(i-1,j) = m_tOut.m_mxBeta(i-1,j)/m_tOut.m_mxSEReg(i-1,j);
	}
	
	
	for (j=0; j<df; j++)
	{
		m_tOut.m_mxPvalue(i-1,j) = 2.0*(1-tcdf(fabs(m_tOut.m_mxTvalue(i-1,j)),nn-df));
	}
	
	return TRUE;
}


BOOL CSimuequation::MODELTEST(int i,  CDoubleMatrix &mxP,  CDoubleMatrix &mxINP,  int g0,  int k0)
{
	i++;
	int nNO= mxP.mrow();
	CDoubleMatrix mxP0;
	if (nNO!=1 || mxINP.mcol()!=0)
	{
		if (mxINP.mcol()!=0)
		{
			nNO = nNO+mxINP.mrow();
			mxP0=mxP.commat(mxINP,2);
		}
		else
		{
			mxP0 = mxP;
		}
		CDoubleMatrix mxQ = - mxP0;
		int j=0; 
		for (j=0; j<nNO; j++)
		{
			mxQ(j,j)=1;
		}
		int  IndC = 0;
		CDoubleMatrix mxQi;
		for (j=0; j<g0+k0; j++)
		{
			if (mxQ(i-1,j)==0)
			{
				CDoubleVector vrQj=mxQ(j);
				mxQi=mxQi|vrQj;
				IndC = IndC+1;
			}
		}
		CDoubleMatrix mxQi1;
		CDoubleMatrix mxQi2;
		
		int nQicol = mxQi.mcol();
		
		if (IndC==0)
		{
			m_bWarning = 1;
			return FALSE;
		}
		
		if (i==1)
		{
			mxQi = mxQi.slice(1,0,nNO-1,nQicol);
		}
		else if(i==nNO)
		{
			mxQi = mxQi.slice(nNO-1,nQicol);
		}
		else
		{
			mxQi1 = mxQi.slice(i-1,nQicol);
			mxQi2 = mxQi.slice(i,0,nNO-i,nQicol);
			mxQi = mxQi1.commat(mxQi2,2);
		}
		
		int nQirow = mxQi.mrow();
		int rQi =0;
		int cQi =0;
		
		CDoubleVector vrUnit1(nQicol,0);
		CDoubleVector vrUnit2(nQirow,0);
		
		for (j=0; j<nQirow; j++)
		{
			if (mxQi.row(j)!=vrUnit1)
			{
				rQi++;
			}
		}
		
		for (j=0; j<nQicol; j++)
		{
			if (mxQi(j)!=vrUnit2)
			{
				cQi++;
			}
		}
		int ranQi = min(cQi,rQi);
		
		if (ranQi!= g0-1)
		{
			m_bWarning = 1;
			return FALSE;
		}
	}
	
	return TRUE;
}


CDoubleMatrix CSimuequation::Kproduct(CDoubleMatrix & mxA,CDoubleMatrix &mxB)
{
	int nA = mxA.mrow();
	int mA = mxA.mcol();
	int nB = mxB.mrow();
	int mB = mxB.mcol();
	
	CDoubleMatrix mxC(nA*nB,mA*mB,0.0);
	
	for (int i=0; i<nA; i++)
	{
		for (int j=0; j<mA; j++)
		{
			for (int k=0; k<nB; k++)
			{
				for (int l=0; l<mB; l++)
				{
					mxC(i*nB+k,j*mB+l) = mxA(i,j)*mxB(k,l); 
				}
			}
			
		}
	}
	
	return mxC;
}

///////////////////////////////////////////////
///					输出结果				///
///////////////////////////////////////////////

void CSimuequation::DrawResult()
{
	if (m_bWarning)
	{
		CTString strWarning = "注：模型设置不适合此方法，结果可能不准确！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add(pWarningTextRslt);
	}
	
	CTLTable * pTable1 = new CTLTable;
	pTable1->CreateTable(2, 2);
	CTString strMethod;
	switch(m_SEmethod) 
	{
	case 0:
		strMethod = "回归系数估计——工具变量方法";
		break;
	case 1:
		strMethod = "回归系数估计——偏最小二乘方法";
		break;
	case 2:
		strMethod = "回归系数估计——两阶段最小二乘方法";
		break;
	case 3:
		strMethod = "回归系数估计——三阶段最小方法";
		break;
	case 4:
		strMethod = "回归系数估计——全信息极大似然方法";
		break;
	default:
		;
	}
	pTable1->SetTitle(strMethod);
	
	pTable1->InsertColumn(0,"");
	pTable1->InsertColumn(0,"参数列表");
	pTable1->InsertColumn(0, "系数");
	pTable1->InsertColumn(0, "系数标准误");
	pTable1->InsertColumn(0, "T值");
	pTable1->InsertColumn(0, "P值");
	
	int coefcountTemp = 0;
	int number=0;
	int nNum = 0;
	int i=0;
	for(i=0; i<m_SEqnumber; i++)
	{
		CTString str;
		str.Format("%d", i+1);
		{
			pTable1->InsertItem(number, "第"+str+"个方程", FALSE);
		}
		number++;
		nNum =m_vrG(i)+m_vrK(i)-1; 
		int j=0;
		for (j=0; j<nNum; j++)
		{
			if (j==(m_vrG(i)+m_vrK(i)-2))
			{
				pTable1->InsertItem(number+j, "");
			}
			else
			{
				pTable1->InsertItem(number+j, "", FALSE);
			}
			pTable1->SetItemText(number+j-1, 1, m_coef_name.GetAt(coefcountTemp + j));
			pTable1->SetItemText(number+j-1, 2, m_tOut.m_mxBeta(i,j));
			pTable1->SetItemText(number+j-1,3,m_tOut.m_mxSEReg(i,j));
			pTable1->SetItemText(number+j-1,4,m_tOut.m_mxTvalue(i,j));
			pTable1->SetItemText(number+j-1,5,m_tOut.m_mxPvalue(i,j));
		}
		number = number+nNum;
		coefcountTemp += m_vrG(i) + m_vrK(i) - 1;
	}
	
	CRsltElementTable *pETable1 = new CRsltElementTable("回归系数估计", pTable1);
	if (m_pResult==NULL)
		m_pResult = new CResult("联立方程模型");
	m_pResult->Add(pETable1);
	
	
	//创建单个方程总结表
	CTLTable * pTable2 = new CTLTable;
	pTable2->CreateTable(2, 2);	
	pTable2->SetTitle("单个方程总结");
	pTable2->InsertColumn(0,"");
	
	for(i=0; i<m_SEqnumber; i++)
	{
		CTString str;
		str.Format("%d", i+1);
		{
			pTable2->InsertColumn(0, "第"+str+"个方程");
		}
	}
	pTable2->InsertItem(0, "复相关系数");
	int j=0; 
	for ( j=0; j<m_SEqnumber; j++)
		pTable2->SetItemText(0,j+1,m_tOut.m_vrRsqu(j));
	
	pTable2->InsertItem(1, "调整的复相关系数");
	for ( j=0; j<m_SEqnumber;j++)
		pTable2->SetItemText(1,j+1,m_tOut.m_vrAdjR(j));
	
	pTable2->InsertItem(2, "SSE");
	for ( j=0; j<m_SEqnumber;j++)
		pTable2->SetItemText(2,j+1,m_tOut.m_vrSSE(j));
	
	pTable2->InsertItem(3, "SST");
	for ( j=0; j<m_SEqnumber;j++)
		pTable2->SetItemText(3,j+1,m_tOut.m_vrSST(j));
	
	pTable2->InsertItem(4, "均值");
	for ( j=0; j<m_SEqnumber;j++)
		pTable2->SetItemText(4,j+1,m_tOut.m_vrMD(j));
	
	pTable2->InsertItem(5, "标准差");
	for ( j=0; j<m_SEqnumber;j++)
		pTable2->SetItemText(5,j+1,m_tOut.m_vrSDD(j));
	
	pTable2->InsertItem(6, "回归标准误");
	for ( j=0; j<m_SEqnumber;j++)
		pTable2->SetItemText(6,j+1,m_tOut.m_vrSER(j));
	
	pTable2->InsertItem(7, "D-W统计量");
	for ( j=0; j<m_SEqnumber;j++)
		pTable2->SetItemText(7,j+1,m_tOut.m_vrDw(j));
	
	CRsltElementTable *pETable2 = new CRsltElementTable("单个方程总结", pTable2);
	m_pResult->Add(pETable2);
}

BOOL CSimuequation::inselectinlist(CTString strTemp0)//判断字符串是否和内生变量相同
{
	for (int i=0; i<m_tVarInfo.iCount; i++)
	{
		if (strTemp0 == m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]))
			return TRUE;
	}
	return FALSE;
}

BOOL CSimuequation::inselectlist(CTString strTemp0)//判断字符串是否和选入变量相同
{
	if (inselectinlist(strTemp0))
		return TRUE;
	for (int i=0; i<m_tVarInfo_D.iCount; i++)
	{
		if (strTemp0 == m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[i]))
			return TRUE;
	}
	return FALSE;
}

BOOL CSimuequation::strIsANum(CTString strTemp0)//判断字符串是否为整数
{
	int nDot = 0;
	int strLength = strlen(strTemp0);
	for (int i=0; i<strlen(strTemp0); i++)
	{
		char chTemp = strTemp0[i];
		if (i == 0 && chTemp == 0 && strLength > 1)
			return FALSE;
		if (chTemp < '0' || chTemp > '9')
			return FALSE;
	}
	return TRUE;
}

BOOL CSimuequation::strIsADouble(CTString strTemp0)//判断是否为浮点数
{
	int nDot = 0;
	int strLength = strlen(strTemp0);
	for (int i=0; i<strlen(strTemp0); i++)
	{
		char chTemp = strTemp0[i];
		if (i == 0 && chTemp == '0' && strLength > 1)
		{
			if (strTemp0[1] != '.')
				return FALSE;
		}
		if (chTemp == '.')
		{
			if (nDot++ == 0)//如果没有小数点
				continue;
			else
				return FALSE;
		}
		if (chTemp < '0' || chTemp > '9')
			return FALSE;
	}
	return TRUE;
}

BOOL CSimuequation::ReadStrToMx()//把字符串读为P矩阵
{
	
	//先排列最后得到的矩阵对应的字符串的顺序：始(不考虑常数)
	m_coef_name.RemoveAll();
	m_ListVarArr.RemoveAll();
	m_ListVarArr.Copy(m_StrVarOrd);
	int i=0;
	for (i=0; i<m_tVarInfo.iCount; i++)
	{
		BOOL bIn = FALSE;//变量名已经在数组中了
		CTString strTmep2 = m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
		for (int j=0; j<m_ListVarArr.GetSize(); j++)
		{
			if (strTmep2 == m_ListVarArr.GetAt(j))
				bIn = TRUE;//???
		}
		if (!bIn)
			m_ListVarArr.Add(strTmep2);
	}
	for (i=0; i<m_tVarInfo_D.iCount; i++)
		m_ListVarArr.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[i]));
	//矩阵对应的字符串的顺序：止
	
	//开始读入矩阵
	int nFunc = m_szArrSE.GetSize();
	CDoubleMatrix mxP(nFunc, m_tVarInfo_D.iCount+m_tVarInfo.iCount+1, 0.0);
	m_mxP = mxP;
	for(i=0; i<nFunc; i++)
	{
		//开始读入矩阵的第i行
		CTString senTemp = m_szArrSE.GetAt(i);
		if (!ReadLineToMx(senTemp, i))
		{
			CTString strWarning;
			strWarning.Format("第 %d 个解释方程书写不合规范，请检查，或查看帮助。",i+1);
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
	}//每行遍历的括号
	return TRUE;
}

BOOL CSimuequation::ReadLineToMx(CTString senTemp, int i)//把字符串读入矩阵的第i列
{
	AnaWord anaSen;
	CTStringArray str_appvar_name;//一行中，出现的变量名。
	anaSen.ImportWithOption(senTemp.GetData());
	int senLength = anaSen.GetWordCount();
	
	if (senLength < 3) 
		return FALSE;
	if ( !inselectinlist(anaSen.GetAt(0)))
		return FALSE;
	str_appvar_name.Add(CTString(anaSen.GetAt(0)));
	if (anaSen.GetAt(1) != "=")
		return FALSE;
	int j=2;
	for (j=2; j<senLength; j++)
	{
		CTString strTemp = anaSen.GetAt(j);
		if (strTemp == "c" || strTemp == "C")//如果是c(0)的形式起始
		{
			if (j+3 > senLength-1)//首先要大于3格
				return FALSE;
			if (anaSen.GetAt(j+1) != "(" || !strIsANum(anaSen.GetAt(j+2) ) || anaSen.GetAt(j+3) != ")")
				return FALSE;
			CTString strtempc = "c(";
			strtempc += anaSen.GetAt(j+2) + ")";
			m_coef_name.Add(strtempc);
			if (j+4 <= senLength-1)
			{
				if (anaSen.GetAt(j+4) == "+")
				{
					if (m_mxP(i, m_tVarInfo.iCount) == 0)
					{
						m_mxP(i, m_tVarInfo.iCount) = 0.5;
						CTString strTemp2 = "c";
						m_coef_var.Add(strTemp2);
						j += 3;
						continue;
					}
					else
						return FALSE;
				}
				else if (anaSen.GetAt(j+4) == "*")
				{
					if (j+5 > senLength-1)
						return FALSE;
					j += 5;//把指向指到×后边的那个变量名
					CTString VarName = anaSen.GetAt(j);
					if (!inselectlist(VarName) )
						return FALSE;
					if (j+4 <= senLength -1)//如果变量后边还有东西
					{
						if (anaSen.GetAt(j+1) != "+") 
						{
							if (anaSen.GetAt(j+1) != "(" || anaSen.GetAt(j+2) != "-" ||!strIsANum(anaSen.GetAt(j+3) ) || anaSen.GetAt(j+4) != ")")
								return FALSE;
							VarName += "(-" + anaSen.GetAt(j+3) + ")";
							int m=0; 
							for (m=0; m<str_appvar_name.GetSize(); m++)
							{
								if (VarName == str_appvar_name.GetAt(m))
									return FALSE;
							}
							str_appvar_name.Add(VarName);
							CDoubleVector c0(m_szArrSE.GetSize(), 0);
							c0(i) = 1;
							BOOL bTempforstr = TRUE;
							int iTempforstr = -1;
							int pzqshzhu=0;
							for (pzqshzhu=m_tVarInfo.iCount+m_tVarInfo_D.iCount; pzqshzhu<m_ListVarArr.GetSize(); pzqshzhu++)
							{
								if (m_ListVarArr.GetAt(pzqshzhu) == VarName)
								{
									bTempforstr = FALSE;
									iTempforstr = pzqshzhu;
									break;
								}
							}
							if (bTempforstr)
							{
								m_ListVarArr.Add(VarName);
								m_mxP = m_mxP | c0;
							}
							else if (m_mxP(i, iTempforstr+1) == 0)
								m_mxP(i, iTempforstr+1) = 1;
							else
								return FALSE;
							m_coef_var.Add(VarName);
							j += 4;
							if (j == senLength-1)
								return TRUE;
							else
								continue;
						}
						else 
						{
							int m=0; 
							for (m=0; m<str_appvar_name.GetSize(); m++)
							{
								if (VarName == str_appvar_name.GetAt(m))
									return FALSE;
							}
							str_appvar_name.Add(VarName);
							int k=0; 
							for (k=0; k<m_ListVarArr.GetSize(); k++)
							{
								if (VarName == m_ListVarArr.GetAt(k))
								{
									if (m_mxP(i, k+(k>=m_tVarInfo.iCount)) == 0) 
									{
										m_mxP(i, k+(k>=m_tVarInfo.iCount)) = 1;
										m_coef_var.Add(VarName);
									}
								}
							}
							if (j == senLength-1)
								return TRUE;
							else
								continue;
						}
					}
					else if (j == senLength -1)//以这个变量为结尾了
					{
						int m=0; 
						for (m=0; m<str_appvar_name.GetSize(); m++)
						{
							if (VarName == str_appvar_name.GetAt(m))
								return FALSE;
						}
						str_appvar_name.Add(VarName);
						int k=0;
						for (k=0; k<m_ListVarArr.GetSize(); k++)
						{
							if (VarName == m_ListVarArr.GetAt(k))
							{
								if (m_mxP(i,k+(k>=m_tVarInfo.iCount)) == 0)
								{
									m_mxP(i,k+(k>=m_tVarInfo.iCount)) = 1;
									m_coef_var.Add(VarName);
									return TRUE;
								}
							}
						}//遍历已有的字符串
					}						
				}//c(0)后边不是"*"
				else
					return FALSE;
			}
			else 
			{
				if (m_mxP(i, m_tVarInfo.iCount) == 0)
				{
					m_mxP(i, m_tVarInfo.iCount) = 0.5;
					m_coef_var.Add("c");
					j += 3;
					return TRUE;
				}
				else
					return FALSE;
			}
		}
		else if (strTemp == "+")
		{
			if (j+1 <= senLength-1)
			{
				if (anaSen.GetAt(j+1) != "c" && anaSen.GetAt(j+1) != "C")
					return FALSE;
			}
			else
				return FALSE;
		}
		else
			return FALSE;
	}//解析第j个字符的括号
	return TRUE;
}

BOOL CSimuequation::ReadStrToINPMx()
{
	CDoubleMatrix mxTemp(m_LimitArrSE.GetSize(), m_ListVarArr.GetSize()+1, 0.0);
	m_mxINP = mxTemp;
	int nLineINP = m_LimitArrSE.GetSize();
	for (int i=0; i<nLineINP; i++)
	{
		//开始读入矩阵的第i行
		CTString senTemp = m_LimitArrSE.GetAt(i);
		if (!ReadLineToINPMx(senTemp, i))
		{
			CTString strWarning;
			strWarning.Format("第 %d 个约束方程书写不合规范，请检查，或查看帮助。",i+1);
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CSimuequation::ReadLineToINPMx(CTString senTemp, int i)
{
	AnaWord anaSen;
	anaSen.ImportWithOption(senTemp.GetData());
	int senLength = anaSen.GetWordCount();
	BOOL firstPlus = TRUE;
	
	if (senLength < 3)
		return FALSE;
	if (!inselectlist(anaSen.GetAt(0)))
		return FALSE;
	int k=0; 
	for (k=0; k<m_ListVarArr.GetSize(); k++)
	{
		if (CTString(anaSen.GetAt(0)) == CTString(m_ListVarArr.GetAt(k)))
		{
			if (m_mxINP(i, k) == 0)
				m_mxINP(i, k) = -1;
			else
				return FALSE;
			break;
		}
	}
	if (anaSen.GetAt(1) != "=")
		return FALSE;
	if (!strIsADouble(anaSen.GetAt(2)))
		return FALSE;
	
	int j=2; 
	for (j=2; j<senLength; j++)
	{
		if (j <= senLength-1)
		{
			CTString strTemp = anaSen.GetAt(j);
			if (strIsADouble(strTemp))//是数字
			{
				if (j+1 <= senLength-1)
				{
					if (anaSen.GetAt(j+1) == "+" && firstPlus)
					{
						if (m_mxINP(i, m_tVarInfo.iCount) == 0)
							m_mxINP(i, m_tVarInfo.iCount) = 0.5;
						else
							return FALSE;
						continue;
					}
					else if (anaSen.GetAt(j+1) == "*")//*一个东西
					{
						if (j+1 == senLength-1)
							return FALSE;
						else if (j+2 == senLength-1)
						{
							if (inselectlist(anaSen.GetAt(j+2)))
							{
								for (int k=0; k<m_ListVarArr.GetSize(); k++)
								{
									if (CTString(anaSen.GetAt(j+2)) == CTString(m_ListVarArr.GetAt(k)))
									{
										if (m_mxINP(i, k+(k>=m_tVarInfo.iCount)) == 0)
										{
											m_mxINP(i, k+(k>=m_tVarInfo.iCount)) = 1;
											return TRUE;
										}
										else 
											return FALSE;
									}
								}
							}
							return TRUE;
						}
						else if (j+5 <= senLength-1)
						{
							if (anaSen.GetAt(j+3) == "+")
							{
								BOOL strTest = FALSE;
								for (int k=0; k<m_ListVarArr.GetSize(); k++)
								{
									if (CTString(anaSen.GetAt(j+2)) == CTString(m_ListVarArr.GetAt(k)))
									{
										if (m_mxINP(i, k+(k>=m_tVarInfo.iCount)) == 0)
										{
											m_mxINP(i, k+(k>=m_tVarInfo.iCount)) = 1;
											strTest = TRUE;
										}
										else 
											return FALSE;
									}
								}
								if (!strTest)
									return FALSE;
								j += 2;
								continue;
							}
							else if (j+6 <= senLength-1)
							{
								if (anaSen.GetAt(j+3) == "(" && anaSen.GetAt(j+4) == "-" &&
									anaSen.GetAt(j+6) == ")" && strIsANum(anaSen.GetAt(j+5)))
								{
									CTString str0Temp = anaSen.GetAt(j+2);
									BOOL bTemp = FALSE;
									str0Temp += "(-" + anaSen.GetAt(j+5) + ")";
									int m=0; 
									for (m=0; m<m_ListVarArr.GetSize(); m++)
									{
										if (str0Temp == m_ListVarArr.GetAt(m))
										{
											if (m_mxINP(i, m) == 0)
											{
												bTemp = TRUE;
												m_mxINP(i, m) = 1;
											}
											else 
												return FALSE;
										}
									}
									j += 6;
									if (!bTemp)
									{
										m_ListVarArr.Add(str0Temp);
										CDoubleVector vecTemp(m_szArrSE.GetSize(), 0);
										m_mxP = m_mxP|vecTemp;
										CDoubleVector vecINPTemp(m_LimitArrSE.GetSize(), 0);
										vecINPTemp(i) = 1;
										m_mxINP = m_mxINP|vecINPTemp;
									}
									else
										continue;										
								}
							}
							else 
								return FALSE;
						}
						else
							return FALSE;
					}
					else
						return FALSE;
				}
				else//最后是常数，且就一个
				{
					if (m_mxINP(i, m_tVarInfo.iCount) == 0)
						m_mxINP(i, m_tVarInfo.iCount) = 0.5;
					else 
						return FALSE;
					return TRUE;
				}
			}//是数字或是"+"
			else if (strTemp != "+")
				return FALSE;
			firstPlus = FALSE;
		}//如果还有东西
		else
			continue;
	}//遍历所有字符
	return TRUE;
}
