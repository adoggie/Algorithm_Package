// TimeSeansonal.cpp: implementation of the CTimeSeansonal class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tladvn.h"
#include "TimeSeansonal.h"

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

CTimeSeansonal::CTimeSeansonal()
{
	m_nMethod=0;
	m_nPeriod=0;
	m_pDataInterface=NULL;
	m_pResult=NULL;
    m_pRsltVariable=NULL;


}

CTimeSeansonal::~CTimeSeansonal()
{
	m_tVarInfo.DeleteObject();
}



//季节解构模型输出
void CTimeSeansonal::CreateResullForSeason(CDoubleMatrix matrix_Z,CDoubleMatrix matrix_SI,CDoubleMatrix matrix_SAF,CDoubleMatrix matrix_SAS,CDoubleMatrix matrix_STC,CDoubleMatrix matrix_I,int period)
{
	//matrix_Z  移动平滑序列
	//matrix_SI  季节-不规则成分
	//matrix_SAF  季节因子
	//matrix_SAS 调整后季节序列
	//matrix_STC  平滑序列
	//matrix_I  不规则成分
	//period   周期
	CRsltElementTable * pETable = NULL;
	int nSize = matrix_SAS.mcol();	
	CTLTable * pTable = new CTLTable;
	CTString strTitle = _T("");
	
	
	

	///////////////////季节因子输出//////////////////////////

	int nCol=matrix_SAF.mcol();
	pTable->SetTitle("季节因子");

	pTable->SetHead(m_pDataInterface->GetFieldName(m_nCurVarNum));

	pTable->InsertColumn(0,"");	
	pTable->InsertItem(0,"数值");
	
	CTString strTemp;

	int maxindex=0,minindex=0;

	int i=0;
	for(i=0;i<nCol;i++)
	{
		//add by liytao 20030819
		if (i==0){
			maxindex=0;
			minindex=0;
		}
		if (matrix_SAF(0,i)>matrix_SAF(0,maxindex))
			maxindex=i;
		if (matrix_SAF(0,i)<matrix_SAF(0,minindex))
			minindex=i;
		//add end
		strTemp.Format("因子%d",i+1);
		pTable->InsertColumn(i+1,strTemp);
		pTable->SetItemText(0,i+1,matrix_SAF(0,i));
	}

	strTitle.Empty();
	strTitle+="季节因子";
	pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	CTString strInf;
	strInf.Format("季节因子%d对%s的影响最大",maxindex+1,m_pDataInterface->GetFieldName(m_nCurVarNum).GetData());
	if (briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strInf);

	strInf.Format("季节因子%d对%s的影响最小",minindex+1,m_pDataInterface->GetFieldName(m_nCurVarNum).GetData());
	if (briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strInf);

	//////////////////////其他因素输出/////////////////

	pTable= new CTLTable;
	pTable->SetTitle("季节分析");
	pTable->SetHead(m_pDataInterface->GetFieldName(m_nCurVarNum));
	pTable->InsertColumn(0,"序号");
	pTable->InsertColumn(1,"移动平均序列");
	pTable->InsertColumn(2,"季节-不规则成分");
	pTable->InsertColumn(3,"调整后季节序列");
	pTable->InsertColumn(4,"平滑趋势-循环序列");
	pTable->InsertColumn(5,"不规则成分");

	CTString str;
	for(i=0; i<nSize; i++)
	{
		str.Format("%d",i+1);
		pTable->InsertItem(i,str);
	}
	///////////////移动平均序列///////////////////
	for(i=0; i<matrix_Z.mcol(); i++)
	{
		if(matrix_Z(0,i)!=0)
		{
			pTable->SetItemText(i,1,matrix_Z(0,i));
		}
	}
	///////////////////////季节不规则成分////////////////////

	for(i=0; i<matrix_SI.mcol(); i++)
	{
		if(matrix_SI(0,i)!=0)
		{
			pTable->SetItemText(i,2,matrix_SI(0,i));
		}
	}

	/////////////////////////调整后季节序列/////////////////
	
	for(i=0; i<matrix_SAS.mcol(); i++)
	{
		pTable->SetItemText(i,3,matrix_SAS(0,i));
	}
	///////////////////////////平滑序列////////////////////////

	for(i=0; i<matrix_STC.mcol(); i++)
	{
		pTable->SetItemText(i,4,matrix_STC(0,i));
	}
	/////////////////////////不规则成分//////////////////////

	for(i=0; i<matrix_I.mcol(); i++)
	{
		pTable->SetItemText(i,5,matrix_I(0,i));
	}
	strTitle.Empty();
	strTitle+="其他因素";
	pETable= new CRsltElementTable(strTitle,pTable);//申请一个表对象类的对象
	this->m_pResult->Add(pETable);

	////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////移动平均序列图形输出描述/////////////////////////////////////
//以下判断是为了避免横坐标过度密集而看不清楚，采取显示一部分，不显示一部分


	TDataPointStrVal* pDPVV=new TDataPointStrVal[matrix_Z.mcol()];

	CTString tmpStr;
	
	for(i=0;i<matrix_Z.mcol();i++)
	{	/*
		if(matrix_Z.mcol()<=20)
		{
			if((i+1)%2==0||i==matrix_Z.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_Z.mcol()<=40)
		{
				if((i+1)%3==0||i==matrix_Z.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_Z.mcol()<=70)
		{
			if((i+1)%5==0||i==matrix_Z.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_Z.mcol()<=100)
		{
			if((i+1)%6==0||i==matrix_Z.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else
		{
			if((i+1)%12==0||i==matrix_Z.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}*/
		//tmpStr.TrimLeft(); 
		tmpStr.Format("%d", i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal =matrix_Z(0,i);
	}
	
	
	strTitle.Empty();
	strTitle+="移动平均序列";
	CTChartLine* pCL=new CTChartLine(pDPVV,matrix_Z.mcol(),StrVal,"移动平均序列","序列","趋势值");
	CRsltElementChart* pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
//	CRsltObject* pDescriptiveTable=pREC;
//	m_pRsltVariable->Add(pDescriptiveTable);
	this->m_pResult->Add(pREC);


	////////////////////////季节不规则成分图形输出/////////////////////////
//以下判断是为了避免横坐标过度密集而看不清楚，采取显示一部分，不显示一部分

    pDPVV=new TDataPointStrVal[matrix_SI.mcol()];
	for(i=0;i<matrix_SI.mcol();i++)
	{	/*
		if(matrix_SI.mcol()<=20)
		{
			if((i+1)%2==0||i==matrix_SI.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_SI.mcol()<=40)
		{
				if((i+1)%3==0||i==matrix_SI.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_SI.mcol()<=70)
		{
			if((i+1)%5==0||i==matrix_SI.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_SI.mcol()<=100)
		{
			if((i+1)%6==0||i==matrix_SI.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else
		{
			if((i+1)%12==0||i==matrix_SI.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}*/
		//tmpStr.TrimLeft(); 
		tmpStr.Format("%d", i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal =matrix_SI(0,i);
	}
	
	strTitle.Empty();
	strTitle+="季节不规则成分";
	pCL=new CTChartLine(pDPVV,matrix_SI.mcol(),StrVal,"季节不规则成分","序列","趋势值");
	pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
//	pDescriptiveTable=pREC;
//	m_pRsltVariable->Add(pDescriptiveTable);

	m_pResult->Add(pREC);
	////////////////////////调整后季节序列图形输出/////////////////////////
//以下判断是为了避免横坐标过度密集而看不清楚，采取显示一部分，不显示一部分

    pDPVV=new TDataPointStrVal[matrix_SAS.mcol()];
	for(i=0;i<matrix_SAS.mcol();i++)
	{	/*
		if(matrix_SAS.mcol()<=20)
		{
			if((i+1)%2==0||i==matrix_SAS.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_SAS.mcol()<=40)
		{
				if((i+1)%3==0||i==matrix_SAS.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_SAS.mcol()<=70)
		{
			if((i+1)%5==0||i==matrix_SAS.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_SAS.mcol()<=100)
		{
			if((i+1)%6==0||i==matrix_SAS.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else
		{
			if((i+1)%12==0||i==matrix_SAS.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}*/
		//tmpStr.TrimLeft(); 
		tmpStr.Format("%d", i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal =matrix_SAS(0,i);
	}
	
	strTitle.Empty();
	strTitle+="调整后季节序列";
	pCL=new CTChartLine(pDPVV,matrix_SAS.mcol(),StrVal,"调整后季节序列","序列","趋势值");
	pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
//	pDescriptiveTable=pREC;
//	m_pRsltVariable->Add(pDescriptiveTable);
	m_pResult->Add(pREC);
	
	////////////////////////平滑序列图形输出/////////////////////////
//以下判断是为了避免横坐标过度密集而看不清楚，采取显示一部分，不显示一部分

    pDPVV=new TDataPointStrVal[matrix_STC.mcol()];
	for(i=0;i<matrix_STC.mcol();i++)
	{	/*
		if(matrix_STC.mcol()<=20)
		{
			if((i+1)%2==0||i==matrix_STC.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_STC.mcol()<=40)
		{
				if((i+1)%3==0||i==matrix_STC.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_STC.mcol()<=70)
		{
			if((i+1)%5==0||i==matrix_STC.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_STC.mcol()<=100)
		{
			if((i+1)%6==0||i==matrix_STC.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else
		{
			if((i+1)%12==0||i==matrix_STC.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}*/
		//tmpStr.TrimLeft(); 
		tmpStr.Format("%d", i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal =matrix_STC(0,i);
	}
	
	strTitle.Empty();
	strTitle+="平滑序列";
	pCL=new CTChartLine(pDPVV,matrix_STC.mcol(),StrVal,"平滑序列","序列","趋势值");
	pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
//	pDescriptiveTable=pREC;
//	m_pRsltVariable->Add(pDescriptiveTable);
	m_pResult->Add(pREC);
	

	////////////////////////不规则成分图形输出/////////////////////////
//以下判断是为了避免横坐标过度密集而看不清楚，采取显示一部分，不显示一部分
    pDPVV=new TDataPointStrVal[matrix_I.mcol()];
	for(i=0;i<matrix_I.mcol();i++)
	{	/*
		if(matrix_I.mcol()<=20)
		{
			if((i+1)%2==0||i==matrix_I.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_I.mcol()<=40)
		{
				if((i+1)%3==0||i==matrix_I.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_I.mcol()<=70)
		{
			if((i+1)%5==0||i==matrix_I.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else if(matrix_I.mcol()<=100)
		{
			if((i+1)%6==0||i==matrix_I.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}
		else
		{
			if((i+1)%12==0||i==matrix_I.mcol()-1)
				tmpStr.Format("%d",i+1);
			else
				tmpStr.Format("    ");
		}*/
		//tmpStr.TrimLeft(); 
		tmpStr.Format("%d", i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal =matrix_I(0,i);
	}
	
	strTitle.Empty();
	strTitle+="不规则成分";
	pCL=new CTChartLine(pDPVV,matrix_I.mcol(),StrVal,"不规则成分","序列","趋势值");
	pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
//	pDescriptiveTable=pREC;
//	m_pRsltVariable->Add(pDescriptiveTable);
	m_pResult->Add(pREC);
	
}


bool CTimeSeansonal::Season(CDoubleVector vect,int dP,int nMorA,int nCol1)
{
	
		//matrix = trans(matrix);
	int nMCol,nFCol,ndP,nP,nL,nQ,nLP,nM;
	int jj=0;
	for(jj=0;jj<vect.vlen();jj++)
	{
		if(vect[jj]<=0)
		{
			CTString str;
			str=m_pDataInterface->GetFieldName(nCol1);
			str+="样本值不能小于等于0！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			return false;
		}
	}

	CDoubleMatrix matrix_X(1,vect.vlen());
	int iiii=0;
	for(iiii=0;iiii<vect.vlen();iiii++)
		matrix_X(0,iiii)=vect[iiii];
	int nCol=matrix_X.mcol();
	

	double dFAverage,dFSum;
	

	WriteData(matrix_X,"matrix__X");

	if(nCol < dP)
	{
		CTString str;
		str=m_pDataInterface->GetFieldName(nCol1);
		str+="样本数太少，不适合于做季节解构模型分析!";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		return false;
	}

    ndP = (dP<0?-1:1)* (int)(fabs(dP));
	nP = (ndP/2<0?-1:1) * (int)(fabs(ndP/2)); //fix(dp/2)

	switch(ndP % 2)
	{
	    case 0:
		{
			nL = nCol - ndP/2 + 1;
		    nQ = (nL/ndP<0?-1:1) * (int)(fabs(nL/ndP));
		    break;
		}
	    case 1:
		{
			nL = nCol - (ndP/2<0?-1:1) * (int)(fabs(ndP/2));
            nQ = ((nCol - ndP/2)/ndP<0?-1:1) * (int)(fabs((nCol - ndP/2)/ndP));
		    break;
		}
	}

	nLP = (nL/ndP<0?-1:1) * (int)(fabs(nL/ndP));
	CDoubleMatrix matrix_SI2(1,nQ);
	CDoubleMatrix matrix_F(1,ndP);
	CDoubleMatrix matrix_Z1(1,nCol - nP);
	CDoubleMatrix matrix_SI1(1,nCol - nP);
	CDoubleMatrix matrix_Z0(1,nCol - ndP/2 + 1);
	CDoubleMatrix matrix_SI0(1,nCol - ndP/2 + 1);

	switch(ndP % 2)
	{
		case 1:
		{
            CDoubleMatrix matrix(1,nCol - nP);
			int i = nP;
			for(i = nP;i < nCol - nP;i++) 
			{
				matrix(0,i) = 0;
			    for(int j = i - nP;j < i + nP + 1;j++)
					matrix(0,i) += matrix_X(0,j);
                matrix(0,i) /= ndP;
			}
//			WriteData(matrix,"matrix");
        
	    	nMCol = matrix.mcol();
			CDoubleMatrix matrix_SI(1,nMCol);

	    	if(nMorA == 0)
				for(int l = nP;l < nMCol;l++)
					matrix_SI(0,l) = (matrix_X(0,l)/matrix(0,l))*100;
		    else if(nMorA == 1)
				for(int i = nP;i < nMCol;i++)
					matrix_SI(0,i) = matrix_X(0,i) - matrix(0,i);

			WriteData(matrix_SI,"matrix__SI");

			int n = 0;
			for(n = 0;n < ndP;n++)
			{
				if(n >= 0 && n <= (nL - nLP*ndP-1))
				{
					for(int j = 0;j < nQ;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+j*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}

            	if(n > (nL - nLP*ndP-1) && n <= (nP-1))
				{
					for(int j = 0;j < nQ-1;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+j*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}

		        if(n > (nP-1) && n <= (ndP-1))
				{
					for(int j = 0;j < nQ;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+(j-1)*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}
			}
		//	WriteData(matrix_SI2,"matrix__SI2");
		//	WriteData(matrix_F,"matrix__F");
			matrix_Z1=matrix;
			matrix_SI1=matrix_SI;
//		WriteData(matrix_Z1,"matrix__Z1");
		    break;
		}

	    case 0:
		{
			CDoubleMatrix matrix(1,nCol - ndP/2 + 1);
			int i = ndP/2;
			for(i = ndP/2;i < nCol - ndP/2 + 1;i++)
			{
				matrix(0,i) = 0;
			    for(int j = i - ndP/2;j < i + ndP/2;j++)
					matrix(0,i) += matrix_X(0,j);
			    matrix(0,i) /= ndP;
			}

		    nMCol = matrix.mcol();
			CDoubleMatrix matrix_SI(1,nMCol);

		    if(nMorA == 0)
				for(int i = ndP/2;i < nMCol;i++)
					matrix_SI(0,i) = (matrix_X(0,i)/matrix(0,i))*100;
		    else if(nMorA == 1)
				for(int i = ndP/2;i < nMCol;i++)
					matrix_SI(0,i) = matrix_X(0,i) - matrix(0,i);

	//		WriteData(matrix_SI,"matrix__SI");

			int n = 0;
			for(n = 0;n < ndP;n++)
			{
				if(n >= 0 && n <= (nL - nLP*ndP-1))
				{
                    for(int j = 0;j < nQ;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+(j+1)*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
					if(matrix_F(0,n)+1==1.0)
					{
						CTString str;
						str=m_pDataInterface->GetFieldName(nCol1);
						str+="不适合于做季节解构模型分析!";
						if(m_pRsltElementText==NULL)
							m_pRsltElementText=new CRsltElementText("警告");
						m_pRsltElementText->AddString(str);
						return false;
					}
				}

            	else if((n > (nL - nLP*ndP-1) )&& (n <= (nP-1)))
				{
					for(int j = 0;j < nQ-1;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+(j+1)*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
					if(matrix_F(0,n)+1==1.0)
					{
						CTString str;
						str=m_pDataInterface->GetFieldName(nCol1);
						str+="不适合于做季节解构模型分析!";
						if(m_pRsltElementText==NULL)
							m_pRsltElementText=new CRsltElementText("警告");
						m_pRsltElementText->AddString(str);
						return false;
					}
				}

		        else if((n > (nP-1)) && (n <= (ndP-1)))
				{
					for(int j = 0;j < nQ;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+j*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
					if(matrix_F(0,n)+1==1.0)
					{
						CTString str;
						str=m_pDataInterface->GetFieldName(nCol1);
						str+="不适合于做季节解构模型分析!";
						if(m_pRsltElementText==NULL)
							m_pRsltElementText=new CRsltElementText("警告");
						m_pRsltElementText->AddString(str);
						return false;
					}
				}
			}
	//		WriteData(matrix_F,"matrix__F");
			matrix_Z0=matrix;
			matrix_SI0=matrix_SI;
		//	WriteData(matrix_Z0,"matrix__Z0");
		    break;
		}
	}

   //计算SAF(Seasonal Average Factor)季节因子
	CDoubleMatrix matrix_SAF(1,ndP);
	if(nMorA == 0)
		for(int i = 0;i < ndP;i++)
		{
            matrix_SAF(0,i) = 0;
			for(int j = 0;j < ndP;j++)
				matrix_SAF(0,i) +=  matrix_F(0,j);
			matrix_SAF(0,i) = 100*ndP*matrix_F(0,i)/matrix_SAF(0,i);
		}
	else if(nMorA == 1)
	{
		nFCol = matrix_F.mcol();
		dFSum = 0;
		for(int i = 0;i < nFCol;i++)
			dFSum += matrix_F(0,i);
		dFAverage = dFSum/ndP;
        for(int m = 0;m < ndP;m++)
			matrix_SAF(0,m) = matrix_F(0,m) - dFAverage;
	}

//	WriteData(matrix_SAF,"matrix_SAF");
//	WriteData(matrix_X,"matrix_X");
	 
	CDoubleMatrix matrix_SAS(1,nCol);

	int l = 0;
	for(l = 0;l < nCol;l++) 
	{
		nM = l + 1 - ((l+1)/ndP<0?-1:1) * (int)(fabs((l+1)/ndP))*ndP;
		if(nM == 0)
			nM = ndP;
		if(nMorA == 0)
			matrix_SAS(0,l) = matrix_X(0,l)/matrix_SAF(0,nM-1)*100;
		else if(nMorA == 1) 
			matrix_SAS(0,l) = matrix_X(0,l) - matrix_SAF(0,nM-1);
	}

  //  WriteData(matrix_SAS,"matrix_SAS");

	if(nCol < 5)
	{
		CTString str;
		str=m_pDataInterface->GetFieldName(nCol1);
		str+="样本数太少，不合于做季节解构模型分析!";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		return false;
	}
    
	CDoubleMatrix matrix_STC(1,nCol);

	matrix_STC = Mov_ave(matrix_SAS,0);

//	WriteData(matrix_STC,"matrix__STC");

    CDoubleMatrix matrix_I(1,nCol);
	int t = 0;
	for(t = 0;t < nCol;t++)
	{
		if(nMorA == 0)
			matrix_I(0,t) = matrix_SAS(0,t)/matrix_STC(0,t);
		else if(nMorA == 1) 
			matrix_I(0,t) = matrix_SAS(0,t) - matrix_STC(0,t);
	}
   // WriteData(matrix_I,"matrix__I");
//	WriteData(matrix_Z0,"matrix__Z0");
	//WriteData(matrix_SI0,"matrix__SI0");
//	WriteData(matrix_SAF,"matrix__SAF");
	//WriteData(matrix_SAS,"matrix__SAS");
//	WriteData(matrix_STC,"matrix__STC");
	
	if(ndP%2==1)
		CreateResullForSeason(matrix_Z1,matrix_SI1,matrix_SAF,matrix_SAS,matrix_STC,matrix_I,dP);
	else
	   CreateResullForSeason(matrix_Z0,matrix_SI0,matrix_SAF,matrix_SAS,matrix_STC,matrix_I,dP);
	return true;
}

CDoubleVector CTimeSeansonal::GetSingleColData(int index,bool &bIsToEnd)
{
	TDataSet *pDataSet=NULL;
	int nRow=m_pDataInterface->GetColData(index,pDataSet,5);
	CDoubleVector vect(nRow);
	for(int i=0;i<nRow;i++)
	{
		if(pDataSet[i].iIndex>=0)
			vect(i)=pDataSet[i].dValue;
		else
		{
			bIsToEnd=true;
			break;
		}
	}
	if(pDataSet)
		delete []pDataSet;
	return vect;	
}


CDoubleMatrix CTimeSeansonal::Mov_ave(CDoubleMatrix matrix_X,int nT3orT12)
{
     int nRow = matrix_X.mrow();
     int nCol = matrix_X.mcol();
	 
	 nRow=nRow>nCol?nRow:nCol;

	 CDoubleMatrix matrix(1,nRow);

	 switch(nT3orT12)
     {
	 case 0:
		 {
			 //if(nRow < 5)
			 //	 return 3.0;
			 
		     matrix(0,1) = 1.0/3*(matrix_X(0,0)+matrix_X(0,1)+matrix_X(0,2));    
             
			 for(int i = 2;i < nRow-2;i++)
			 {
	    	
		     	matrix(0,i) = 1.0/9*(matrix_X(0,i-2)+2*matrix_X(0,i-1)+3*matrix_X(0,i)+2*matrix_X(0,i+1)+matrix_X(0,i+2));   
			
			 }
		     matrix(0,0) = matrix(0,1)+1.0/2*(matrix(0,1)-matrix(0,2));                  
             matrix(0,nRow-2) = 1.0/3*(matrix_X(0,nRow-3)+matrix_X(0,nRow-2)+matrix_X(0,nRow-1)); 
		     matrix(0,nRow-1) = matrix(0,nRow-2)+1.0/2*(matrix(0,nRow-2)-matrix(0,nRow-3));
		     break;
		 }
	 
	 case 1:
		 {
		     //if(nRow <36)
		     //	  return 4.0;

			 int m = 6;
             for(m = 6;m < nRow-6;m++)
			 {
		    	  matrix(0,m) = matrix_X(0,m-5);
	    		  for(int j = m - 4;j <= m+5;j++)
					  matrix(0,m) += 2.0*matrix_X(0,j);
                  matrix(0,m) += matrix_X(0,m+6);
			      matrix(0,m) = matrix(0,m)/24;

			 }	
			 int l = 0;
		     for(l = 0;l < 6;l++)
			      matrix(0,l) = (matrix(0,l+12)+matrix(0,l+24))/2;                  
             for(int n = nRow-1;n >= nRow-6;n--)
				  matrix(0,n) = (matrix(0,n-24)+matrix(0,n-12))/2;
		   //  WriteData(matrix,"matrix");
			 break;
		 }
	 }
     return matrix;
}

double CTimeSeansonal::Medial_average(CDoubleMatrix matrix_X)
{
	int nRow = matrix_X.mrow();
    int nCol = matrix_X.mcol();

	if(nRow != 1)
		return false;	
    double sum=0;
	for(int m = 0;m < nCol;m++)
	{
		sum=sum+matrix_X(0,m);

	}
	return sum/nCol;
 
}    

CDoubleMatrix CTimeSeansonal::GetSAF(CDoubleVector vect,int dP,int nMorA)
{	
	int nMCol,nFCol,ndP,nP,nL,nQ,nLP;//,nM
	CDoubleMatrix matrix_X(1,vect.vlen());
	int iiii=0;
	for(iiii=0;iiii<vect.vlen();iiii++)
		matrix_X(0,iiii)=vect[iiii];
	int nCol=matrix_X.mcol();
	
	double dFAverage,dFSum;

    ndP = (dP<0?-1:1)* (int)(fabs(dP));
	nP = (ndP/2<0?-1:1) * (int)(fabs(ndP/2)); //fix(dp/2)

	switch(ndP % 2)
	{
	    case 0:
		{
			nL = nCol - ndP/2 + 1;
		    nQ = (nL/ndP<0?-1:1) * (int)(fabs(nL/ndP));
		    break;
		}
	    case 1:
		{
			nL = nCol - (ndP/2<0?-1:1) * (int)(fabs(ndP/2));
            nQ = ((nCol - ndP/2)/ndP<0?-1:1) * (int)(fabs((nCol - ndP/2)/ndP));
		    break;
		}
	}

	nLP = (nL/ndP<0?-1:1) * (int)(fabs(nL/ndP));
	CDoubleMatrix matrix_SI2(1,nQ);
	CDoubleMatrix matrix_F(1,ndP);
	CDoubleMatrix matrix_Z1(1,nCol - nP);
	CDoubleMatrix matrix_SI1(1,nCol - nP);
	CDoubleMatrix matrix_Z0(1,nCol - ndP/2 + 1);
	CDoubleMatrix matrix_SI0(1,nCol - ndP/2 + 1);

	switch(ndP % 2)
	{
		case 1:
		{
            CDoubleMatrix matrix(1,nCol - nP);
			int i = nP;
			for(i = nP;i < nCol - nP;i++) 
			{
				matrix(0,i) = 0;
			    for(int j = i - nP;j < i + nP + 1;j++)
					matrix(0,i) += matrix_X(0,j);
                matrix(0,i) /= ndP;
			}
        
	    	nMCol = matrix.mcol();
			CDoubleMatrix matrix_SI(1,nMCol);

	    	if(nMorA == 0)
				for(int l = nP;l < nMCol;l++)
					matrix_SI(0,l) = (matrix_X(0,l)/matrix(0,l))*100;
		    else if(nMorA == 1)
				for(int i = nP;i < nMCol;i++)
					matrix_SI(0,i) = matrix_X(0,i) - matrix(0,i);

			int n = 0;
			for(n = 0;n < ndP;n++)
			{
				if(n >= 0 && n <= (nL - nLP*ndP-1))
				{
					for(int j = 0;j < nQ;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+j*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}

            	if(n > (nL - nLP*ndP-1) && n <= (nP-1))
				{
					for(int j = 0;j < nQ-1;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+j*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}

		        if(n > (nP-1) && n <= (ndP-1))
				{
					for(int j = 0;j < nQ;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+(j-1)*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}
			}
			matrix_Z1=matrix;
			matrix_SI1=matrix_SI;
		    break;
		}

	    case 0:
		{
			CDoubleMatrix matrix(1,nCol - ndP/2 + 1);
			int i = ndP/2;
			for(i = ndP/2;i < nCol - ndP/2 + 1;i++)
			{
				matrix(0,i) = 0;
			    for(int j = i - ndP/2;j < i + ndP/2;j++)
					matrix(0,i) += matrix_X(0,j);
			    matrix(0,i) /= ndP;
			}

		    nMCol = matrix.mcol();
			CDoubleMatrix matrix_SI(1,nMCol);

		    if(nMorA == 0)
				for(int i = ndP/2;i < nMCol;i++)
					matrix_SI(0,i) = (matrix_X(0,i)/matrix(0,i))*100;
		    else if(nMorA == 1)
				for(int i = ndP/2;i < nMCol;i++)
					matrix_SI(0,i) = matrix_X(0,i) - matrix(0,i);

			int n = 0;
			for(n = 0;n < ndP;n++)
			{
				if(n >= 0 && n <= (nL - nLP*ndP-1))
				{
                    for(int j = 0;j < nQ;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+(j+1)*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}

            	else if((n > (nL - nLP*ndP-1) )&& (n <= (nP-1)))
				{
					for(int j = 0;j < nQ-1;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+(j+1)*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}

		        else if((n > (nP-1)) && (n <= (ndP-1)))
				{
					for(int j = 0;j < nQ;j++)
						matrix_SI2(0,j) = matrix_SI(0,n+j*ndP);
			        matrix_F(0,n) = Medial_average(matrix_SI2);
				}
			}
			matrix_Z0=matrix;
			matrix_SI0=matrix_SI;
		    break;
		}
	}

   //计算SAF(Seasonal Average Factor)季节因子
	CDoubleMatrix matrix_SAF(1,ndP);
	if(nMorA == 0)
		for(int i = 0;i < ndP;i++)
		{
            matrix_SAF(0,i) = 0;
			for(int j = 0;j < ndP;j++)
				matrix_SAF(0,i) +=  matrix_F(0,j);
			matrix_SAF(0,i) = 100*ndP*matrix_F(0,i)/matrix_SAF(0,i);
		}
	else if(nMorA == 1)
	{
		nFCol = matrix_F.mcol();
		dFSum = 0;
		for(int i = 0;i < nFCol;i++)
			dFSum += matrix_F(0,i);
		dFAverage = dFSum/ndP;
        for(int m = 0;m < ndP;m++)
			matrix_SAF(0,m) = matrix_F(0,m) - dFAverage;
	}
	return matrix_SAF;
}

bool CTimeSeansonal::GetValueFromVO(CDataInterface * pDataInterface, string VO)
{
//	SEANSONAL
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/SEANSONAL SEASON({4**},{12})
//		/METHOD=[{MULTIP},{ADD}]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;

	int seanson = AnaSyntax::getvalue("/SEASONAL","SEASON",VOstr);
	if (seanson == 12)
		m_nPeriod = 1;
	else
		m_nPeriod = 0;

	m_nMethod = int(AnaSyntax::findstr("/METHOD","ADD",VOstr));
	return TRUE;

}

CResult * CTimeSeansonal::OnTimeSeasonal(CDataInterface *pDataInterface, string VO)
{
	
	briefrTable=NULL;
	
	m_pDataInterface = pDataInterface;
	m_pRsltVariable=NULL;
	m_pRsltElementText=NULL;
	m_pResult = NULL;
	m_pResult = new CResult("时间序列-季节解构模型分析");
	if (!GetValueFromVO(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
	if (nOutNote)
	{		
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"结果说明");
	}
	int nPeriod=4;
	if(m_nPeriod==1)
		nPeriod=12;
	else
		nPeriod=4;
	int i = 0;
	for(i = 0;i < m_tVarInfo.iCount;i++)
	{	
		int nIndex = m_tVarInfo.pValue[i];
		m_nCurVarNum = nIndex;
		bool bIsToEnd=false;
		CDoubleVector vector = GetSingleColData(nIndex,bIsToEnd);
		if(bIsToEnd )
		{
			CTString str;
			str=pDataInterface->GetFieldName(nIndex);
			str+="不应有空值或缺失值！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			continue;
		}
		Season(vector,nPeriod,m_nMethod,nIndex); //Season模型	
//		m_pRsltVariable =new CRsltElementTable(m_pDataInterface->GetFieldName(m_nCurVarNum));
//		if(!Season(vector,nPeriod,m_nMethod,nIndex)) //Season模型	
//		{
//			delete m_pRsltVariable;
//			m_pRsltVariable =NULL;
//		}
//		else
//			m_pResult->Add(m_pRsltVariable);
		
		//		if(m_pRsltElementText)
		//			m_pRsltVariable->Add(m_pRsltElementText);
		//		m_pRsltElementText=NULL;
		
	}
	
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
		if (m_pResult->Count()<=0)
			m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
		else
			m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
	}
	else if (briefrTable!=NULL)
	{
		delete briefrTable;
		briefrTable =NULL;
	}
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	return m_pResult;

	
} 

