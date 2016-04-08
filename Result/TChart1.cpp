// TChart1.cpp : implementation file
//

#include "TChart1.h"

#include "TChartArea.h"
#include "Result.h"
//#include "..\..\tlss\resource.h"
//#include "..\..\tlss\OutputView.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

CTChart::CTChart()
{
}

CTChart::~CTChart()
{
	OnClear();
}

void CTChart::Print()
{
	printf("The chart title is %s\nThe Chart X Axis is %s\n,The Chart Y Axis is %s\n",m_strTitle.GetData(),m_strXAxisLabel.GetData(),m_strYAxisLabel.GetData());
	printf("The Chart has %d groups\n",m_arData.GetSize());
	int i=0;
	for (i=0;i<m_arData.GetSize();i++)
	{
		TGraph *pGraph = (TGraph *)m_arData.GetAt(i);
		for(int j=0;j<pGraph->grphData.m_Data.GetSize();j++)
		{
			if (pGraph->grphData.m_eDataType == ValVal)
			{
				TDataPointValVal *pVaVaArr = (TDataPointValVal*)pGraph->grphData.m_Data.GetAt(j);
				printf("(%.2f,%.2f),",pVaVaArr->fXVal,pVaVaArr->fYVal);
			}
			else if (pGraph->grphData.m_eDataType == StrVal)
			{
				TDataPointStrVal *pStVaArr = (TDataPointStrVal*)pGraph->grphData.m_Data.GetAt(j);
				printf("(%s,%.2f),",pStVaArr->fXVal.GetData(),pStVaArr->fYVal);
			}
			else if (pGraph->grphData.m_eDataType == ValStr)
			{
				TDataPointValStr *pVaStArr = (TDataPointValStr*)pGraph->grphData.m_Data.GetAt(j);
				printf("(%.2f,%s),",pVaStArr->fXVal,pVaStArr->fYVal.GetData());
			}
			else if (pGraph->grphData.m_eDataType == StrArr)
			{
				TDataPointStrArr *pStrArr = (TDataPointStrArr*)pGraph->grphData.m_Data.GetAt(j);
				printf("(%s,[",pStrArr->strName.GetData());
				for (int l =0;l<pStrArr->arrYVal.GetSize();l++)
				{
					printf("%.2f",pStrArr->arrYVal.GetAt(l));
				}
				printf("]),");
			}
			else if (pGraph->grphData.m_eDataType == ThrArr)
			{
				int l = 0;
				tagDataPointPedigree *pPredigree = (tagDataPointPedigree*)pGraph->grphData.m_Data.GetAt(j);
				printf("XAxis:\n");
				for (l =0;l<pPredigree->arrXAxis.GetSize();l++)
				{
					printf("%.5f\t",pPredigree->arrXAxis.GetAt(l));
				}
				printf("YAxis:\n");
				for (l =0;l<pPredigree->arrYAxis.GetSize();l++)
				{
					printf("%s\t",pPredigree->arrYAxis.GetAt(l).GetData());
				}
				printf("Point:\n");
				for (l =0;l<pPredigree->arrPoint.GetSize();l++,l++)
				{
					printf("P(%.2f,%.2f)\t",pPredigree->arrPoint.GetAt(l),pPredigree->arrPoint.GetAt(l+1));
				}
			}
			else if (pGraph->grphData.m_eDataType == FriArr)
			{
				int l = 0;
				tagDataPointDecTree *pTree = (tagDataPointDecTree*)pGraph->grphData.m_Data.GetAt(j);
				printf("Rectangle:\n");
				for (l =0;l<pTree->arrRectangle.GetSize();l= l+4)
				{
					printf("l=%.2f,t=%.2f,r=%.2f,b=%.2f\t",pTree->arrRectangle.GetAt(l),pTree->arrRectangle.GetAt(l+1),pTree->arrRectangle.GetAt(l+2),pTree->arrRectangle.GetAt(l+3));
				}
				printf("RectangelText:\n");
				for (l =0;l<pTree->szArrRectangle.GetSize();l++)
				{
					printf("%s\t",pTree->szArrRectangle.GetAt(l).GetData());
				}
				printf("RectFill:\n");
				for (l =0;l<pTree->arrRectFill.GetSize();l= l+4)
				{
					printf("l=%.2f,t=%.2f,r=%.2f,b=%.2f\t",pTree->arrRectFill.GetAt(l),pTree->arrRectFill.GetAt(l+1),pTree->arrRectFill.GetAt(l+2),pTree->arrRectFill.GetAt(l+3));
				}
				printf("RectFillText:\n");
				for (l =0;l<pTree->szArrRectFill.GetSize();l++)
				{
					printf("%s\t",pTree->szArrRectFill.GetAt(l).GetData());
				}
				printf("Point:\n");
				for (l =0;l<pTree->arrLinePoint.GetSize();l++,l++)
				{
					printf("P(%.2f,%.2f)\t",pTree->arrLinePoint.GetAt(l),pTree->arrLinePoint.GetAt(l+1));
				}
			}
		}
		printf("\n");
		printf("%s\n",pGraph->strLengend.GetData());
	}
	for (i =0; i<m_arChartInfo.GetSize();i++)
	{
		CTString str = m_arChartInfo.GetAt(i);
		printf("%s\n",str.GetData());
	}
}

////////////////////////////////////////////////////////////////////////
//Function name :  OnClear
//Parameter		   :	
//Author			   :   ������
//Style 				:	
//Function			 :	��ն���
//Describe			 :	
////////////////////////////////////////////////////////////////////////
void CTChart::OnClear()
{
	m_strTitle = "";
	m_strXAxisLabel = "";
	m_strYAxisLabel = "";
	m_arChartInfo.RemoveAll();
	for (int i=0; i<  m_arData.GetSize(); i++)
	{
		TGraph* pGD =(TGraph* )m_arData.GetAt(i);
		delete pGD;
	}
}


//Function name :  SetData
//Parameter		   :
//								  CTGraph* pGraph			���������ݵ��ڲ�ͼ�����ָ��
//								  TDataPointValVal* pValues	  ����ָ��	
//								  int nCount						 ÿ��Ŀ������Ŀ����
//Author			   :   ������
//Style 				:	
//Function			 :	 ��������
//Describe			 :	 ���ڲ�ͼ�������������	
////////////////////////////////////////////////////////////////////////
void CTChart::SetData(void *pValues, int nLen, EGraphType eGraphType,EDataType eDataType,CTString strLengend)
{
	TGraph *pGraph = new TGraph;
	pGraph->eGraphType = eGraphType;
	pGraph->grphData.SetData(pValues,nLen,eDataType);
	pGraph->strLengend = strLengend;
	m_arData.Add(pGraph);
}

/*
EChartKind	   m_ChartKind;//ͼ������(Line\Bar\Pie)
CTString	   m_strTitle;//ͼ�����
CTString		m_strXAxisLabel;
CTString		m_strYAxisLabel;
CTStringArray   m_arLengend;
CTStringArray	m_arChartInfo;
CTArray<CTGraphData*,CTGraphData*>  m_arData;   //���ݣ���ͼ�ã�
*/
void CTChart::Serialize(unsigned char *&pBuf)
{
	SaveDigit(pBuf,m_ChartKind);//ͼ������(Line\Bar\Pie)
	SaveString(pBuf,m_strTitle);//ͼ�����
	SaveString(pBuf,m_strXAxisLabel);
	SaveString(pBuf,m_strYAxisLabel);


	int nSize = m_arChartInfo.GetSize();
	SaveDigit(pBuf,nSize);//�����ַ�������
	int i=0;
	for (i=0;i<nSize ;i++)
	{
		CTString strTemp = m_arChartInfo.GetAt(i);
		SaveString(pBuf,strTemp);
	}

	nSize = m_arData.GetSize();//���ݣ���ͼ�ã�
	SaveDigit(pBuf,nSize);//���泤��
	for (i=0;i<nSize ;i++)
	{
		TGraph* pGraph= (TGraph*)m_arData.GetAt(i);
		SaveDigit(pBuf,pGraph->eGraphType);
		SaveString(pBuf,pGraph->strLengend);
		pGraph->grphData.Serialize(pBuf);
	}
}

int CTChart::GetBufLen()
{
	int nLen = 0;
	AddDigitLen(nLen,m_ChartKind);//ͼ������(Line\Bar\Pie)
	AddStringLen(nLen,m_strTitle);//ͼ�����
	AddStringLen(nLen,m_strXAxisLabel);
	AddStringLen(nLen,m_strYAxisLabel);


	int nSize = m_arChartInfo.GetSize();
	AddDigitLen(nLen,nSize);//�����ַ�������
	int i=0;
	for (i=0;i<nSize ;i++)
	{
		CTString strTemp = m_arChartInfo.GetAt(i);
		AddStringLen(nLen,strTemp);
	}

	nSize = m_arData.GetSize();//���ݣ���ͼ�ã�
	AddDigitLen(nLen,nSize);//���泤��
	for (i=0;i<nSize ;i++)
	{
		TGraph* pGraph= (TGraph*)m_arData.GetAt(i);
		AddDigitLen(nLen,pGraph->eGraphType);
		AddStringLen(nLen,pGraph->strLengend);
		nLen+=pGraph->grphData.GetBufLen();
	}
	return nLen;
}
