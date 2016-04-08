// TGraphData.cpp: implementation of the CTGraphData class.
//
//////////////////////////////////////////////////////////////////////

#include "TGraphData.h"
#include "Result.h"
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
CTGraphData::CTGraphData()
{
	Clear();
}

CTGraphData::~CTGraphData()
{
	Clear();
}


void CTGraphData::Clear()
{
	int i;
	if (m_eDataType == ValVal)
	{
		for(i=0;i<m_Data.GetSize();i++)
		{
			TDataPointValVal *pVal = (TDataPointValVal *)m_Data.GetAt(i);
			delete pVal;
		}
	}
	else if (m_eDataType == ValStr)
	{
		for( i=0;i<m_Data.GetSize();i++)
		{
			TDataPointValStr *pVal = (TDataPointValStr*)m_Data.GetAt(i);
			delete pVal; 
		}
	}
	else if (m_eDataType == StrVal)
	{
		for( i=0;i<m_Data.GetSize();i++)
		{
			TDataPointStrVal *pVal = (TDataPointStrVal*)m_Data.GetAt(i);
			delete pVal; 
		}
	}
	else if (m_eDataType == StrArr)
	{
		for( i=0;i<m_Data.GetSize();i++)
		{
			TDataPointStrArr *pVal = (TDataPointStrArr*)m_Data.GetAt(i);
			pVal->arrYVal.RemoveAll();
			delete pVal; 
		}
	}
	else if (m_eDataType == ThrArr)//add by xds 20060428
	{
		for(i=0;i<m_Data.GetSize();i++)
		{
			TDataPointPedigree *pVal = (TDataPointPedigree*)m_Data.GetAt(i);
			pVal->arrXAxis.RemoveAll();
			pVal->arrYAxis.RemoveAll();
			pVal->arrPoint.RemoveAll();
			delete pVal; 
		}
	}
	else if (m_eDataType == FriArr)//Add by xds 20060510
	{
		for(i=0;i<m_Data.GetSize();i++)
		{
			tagDataPointDecTree *pVal = (tagDataPointDecTree*)m_Data.GetAt(i);
			pVal->arrRectangle.RemoveAll();
			pVal->szArrRectangle.RemoveAll();
			pVal->arrRectFill.RemoveAll();
			pVal->szArrRectFill.RemoveAll();
			pVal->arrLinePoint.RemoveAll();
			delete pVal; 
		}
	}
	m_Data.RemoveAll(); 
}

////////////////////////////////////////////////////////////////////////
//Function name :  SetData
//Parameter		   :	
//								 TDataPointValVal *pData		被设置数据指针
//								 int nCount						 数据个数
//Author			   :  蒋国清
//Style 				:	
//Function			 :	 =操作符重载	
//Describe			 :	
////////////////////////////////////////////////////////////////////////

void CTGraphData::SetData(void *pData, int nLen, EDataType eDataType,CTString strLengend)
{
	int i,j;
	m_eDataType = eDataType;
	switch(eDataType)
	{
	case ValVal:
		{
			TDataPointValVal* pDataValVal= (TDataPointValVal* )pData;
			for (i=0; i< nLen; i++)
			{
				TDataPointValVal* pNewDP= new TDataPointValVal;
				pNewDP->fXVal =pDataValVal[i].fXVal ;
				pNewDP->fYVal =pDataValVal[i].fYVal ;
				m_Data.Add(pNewDP);
			}
		}
		break;
	case ValStr:
		{
			TDataPointValStr* pDataValStr=(TDataPointValStr* )pData;
			for (i=0; i< nLen; i++)
			{
				TDataPointValStr* pNewDP= new TDataPointValStr;
				pNewDP->fXVal =pDataValStr[i].fXVal ;
				pNewDP->fYVal =pDataValStr[i].fYVal ;
				m_Data.Add(pNewDP);
			}
		}
		break;
	case StrVal:
		{
			TDataPointStrVal* pDataStrVal=(TDataPointStrVal* )pData;
			for (i=0; i< nLen; i++)
			{
				TDataPointStrVal* pNewDP=new TDataPointStrVal;
				pNewDP->fXVal =pDataStrVal[i].fXVal ;
				pNewDP->fYVal =pDataStrVal[i].fYVal ;
				m_Data.Add(pNewDP);
			}
		}
		break;
	case StrArr:
		{
			TDataPointStrArr* pDataStrArr=(TDataPointStrArr* )pData;
			for (i=0; i< nLen; i++)
			{
				TDataPointStrArr* pNewDP= new TDataPointStrArr;
				pNewDP->strName =pDataStrArr[i].strName;
				for (int j=0;j<pDataStrArr[i].arrYVal.GetSize();j++)
					pNewDP->arrYVal.Add(pDataStrArr[i].arrYVal[j]);
				m_Data.Add(pNewDP);
			}
		}
		break;
	case ThrArr://add by xds 20060428
		{
			TDataPointPedigree *pPedigree = (TDataPointPedigree *)pData;
			for (i=0;i<nLen;i++)
			{
				TDataPointPedigree *pNewDP = new TDataPointPedigree;
				int nSize = pPedigree[i].arrXAxis.GetSize();
				for (j=0; j<nSize; j++)
				{
					pNewDP->arrXAxis.Add(pPedigree[i].arrXAxis[j]);
				}
				nSize = pPedigree[i].arrYAxis.GetSize();
				for (j=0; j<nSize; j++)
				{
					pNewDP->arrYAxis.Add(pPedigree[i].arrYAxis[j]);
				}
				nSize = pPedigree[i].arrPoint.GetSize();
				for (j=0; j<nSize; j++)
				{
					pNewDP->arrPoint.Add(pPedigree[i].arrPoint[j]);
				}
				m_Data.Add(pNewDP);
			}
		}
		break;
	case FriArr: //Add by xds 20060510
		{
			tagDataPointDecTree *pDecTree = (tagDataPointDecTree *)pData;
			for (i=0;i<nLen;i++)
			{
				tagDataPointDecTree *pNewDP = new tagDataPointDecTree;
				int nSize = pDecTree[i].arrRectangle.GetSize();
				for (j=0; j<nSize; j++)
				{
					pNewDP->arrRectangle.Add(pDecTree[i].arrRectangle[j]);
				}
				nSize = pDecTree[i].szArrRectangle.GetSize();
				for (j=0; j<nSize; j++)
				{
					pNewDP->szArrRectangle.Add(pDecTree[i].szArrRectangle[j]);
				}
				nSize = pDecTree[i].arrRectFill.GetSize();
				for (j=0; j<nSize; j++)
				{
					pNewDP->arrRectFill.Add(pDecTree[i].arrRectFill[j]);
				}
				nSize = pDecTree[i].szArrRectFill.GetSize();
				for (j=0; j<nSize; j++)
				{
					pNewDP->szArrRectFill.Add(pDecTree[i].szArrRectFill[j]);
				}
				nSize = pDecTree[i].arrLinePoint.GetSize();
				for (j=0; j<nSize; j++)
				{
					pNewDP->arrLinePoint.Add(pDecTree[i].arrLinePoint[j]);
				}
				m_Data.Add(pNewDP);
			}
		}
		break;
	}
}

void CTGraphData::Serialize(TCLFile &file)
{
}


void CTGraphData::Print()
{
	int i;
	printf("Chart Data Begin:\n");
	int nCount=m_Data.GetSize();
	if (nCount > 0)
	{
		printf("DataValVal Size is %d\n",nCount);
	}

	if (m_eDataType == ValVal)
	{
		printf("DataType is ValVal.\n");
		for(i=0;i<nCount;i++)
		{
			TDataPointValVal * pVal = (TDataPointValVal*)m_Data.GetAt(i);
			printf("(%.2f,   %.2f)\n",pVal->fXVal,pVal->fYVal);
		}
	}
	else if (m_eDataType == ValStr)
	{
		printf("DataType is ValStr.\n");
		for(i=0;i<nCount;i++)
		{
			TDataPointValStr *pVal = (TDataPointValStr*)m_Data.GetAt(i);
			printf("(%.2f,    %s)",pVal->fXVal,pVal->fYVal);
		}
	}
	else if (m_eDataType == StrVal)
	{
		printf("DataType is StrVal.\n");
		for(i=0;i<nCount;i++)
		{
			TDataPointStrVal *pVal = (TDataPointStrVal*)m_Data.GetAt(i);
			printf("(%s,  %d)\n",pVal->fXVal,pVal->fYVal);
		}
	}
	else if (m_eDataType == StrArr)
	{
		printf("DataType is StrArr.\n");
		for(i=0;i<nCount;i++)
		{
			TDataPointStrArr *pVal = (TDataPointStrArr*)m_Data.GetAt(i);
			printf("(X:%s)\nY:",pVal->strName);
			for (int j=0;j<pVal->arrYVal.GetSize();j++)
				printf("%.2f,",pVal->arrYVal[i]);
			printf(")\n");
		}
	}
}

void CTGraphData::Serialize(unsigned char *&pBuf)
{
	int i,j;
	int nCount=m_Data.GetSize();
	SaveDigit(pBuf,m_eDataType);
	SaveDigit(pBuf,nCount);

	if (m_eDataType == ValVal)
	{
		for(i=0;i<nCount;i++)
		{
			TDataPointValVal *pVal = (TDataPointValVal*)m_Data[i];
			SaveDigit(pBuf,pVal->fXVal);
			SaveDigit(pBuf,pVal->fYVal);
		}
	}
	else if (m_eDataType == ValStr)
	{
		for(i=0;i<nCount;i++)
		{
			TDataPointValStr *pVal = (TDataPointValStr*)m_Data[i];
			SaveDigit(pBuf,pVal->fXVal );
			SaveString(pBuf,pVal->fYVal );
		}
	}
	else if (m_eDataType == StrVal)
	{
		for(i=0;i<nCount;i++)
		{
			TDataPointStrVal *pVal = (TDataPointStrVal*)m_Data[i];
			SaveString(pBuf,pVal->fXVal);
			SaveDigit(pBuf,pVal->fYVal);
		}
	}
	else if (m_eDataType == StrArr)
	{
		for(i=0;i<nCount;i++)
		{
			TDataPointStrArr *pVal = (TDataPointStrArr*)m_Data[i];
			SaveString(pBuf,pVal->strName);
			int nC = pVal->arrYVal.GetSize();
			SaveDigit(pBuf,nC);
			for (int j=0;j<nC;j++)
			SaveDigit(pBuf,pVal->arrYVal[j]);
		}
	}
	else if (m_eDataType == ThrArr)//add by xds 20060428
	{
		for (i=0;i<nCount;i++)
		{
			TDataPointPedigree *pVal = (TDataPointPedigree *)m_Data[i];
			int nSize = pVal->arrXAxis.GetSize();
			SaveDigit(pBuf,nSize);
			for (j=0;j<nSize;j++)
			{
				SaveDigit(pBuf,pVal->arrXAxis[j]);
			}
			nSize = pVal->arrYAxis.GetSize();
			SaveDigit(pBuf,nSize);//字符串个数
			for (j=0;j<nSize;j++)
			{
				SaveString(pBuf,pVal->arrYAxis[j]);
			}
			nSize = pVal->arrPoint.GetSize();
			SaveDigit(pBuf,nSize);
			for (j=0;j<nSize;j++)
			{
				SaveDigit(pBuf,pVal->arrPoint[j]);
			}
		}
	}
	else if (m_eDataType == FriArr)//Add by xds 20060510
	{
		for (i=0; i<nCount; i++)
		{
			tagDataPointDecTree *pVal = (tagDataPointDecTree *)m_Data[i];
			int nSize = pVal->arrRectangle.GetSize();
			SaveDigit(pBuf,nSize);
			for (j=0;j<nSize;j++)
			{
				SaveDigit(pBuf,pVal->arrRectangle[j]);
			}
			nSize =  pVal->szArrRectangle.GetSize();
			SaveDigit(pBuf,nSize);//字符串个数
			for (j=0; j<nSize; j++)
			{
				SaveString(pBuf,pVal->szArrRectangle[j]);
			}
			nSize = pVal->arrRectFill.GetSize();
			SaveDigit(pBuf,nSize);
			for (j=0; j<nSize; j++)
			{
				SaveDigit(pBuf,pVal->arrRectFill[j]);
			}
			nSize =  pVal->szArrRectFill.GetSize();
			SaveDigit(pBuf,nSize);//字符串个数
			for (j=0; j<nSize; j++)
			{
				SaveString(pBuf,pVal->szArrRectFill[j]);
			}
			nSize = pVal->arrLinePoint.GetSize();
			SaveDigit(pBuf,nSize);
			for (j=0; j<nSize; j++)
			{
				SaveDigit(pBuf,pVal->arrLinePoint[j]);
			}
		}
	}
}

int CTGraphData::GetBufLen()
{
	int i,j;
	int nLen =0;
	int nCount=m_Data.GetSize();
	AddDigitLen(nLen,m_eDataType);
	AddDigitLen(nLen,nCount);
	if (m_eDataType == ValVal)
	{
		for(i=0;i<nCount;i++)
		{
			TDataPointValVal* pVal = (TDataPointValVal*)m_Data[i];
			AddDigitLen(nLen,pVal->fXVal);
			AddDigitLen(nLen,pVal->fYVal);
		}
	}
	else if (m_eDataType == ValStr)
	{
		for(i=0;i<nCount;i++)
		{
			TDataPointValStr* pVal = (TDataPointValStr*)m_Data[i];
			AddDigitLen(nLen,pVal->fXVal );
			AddStringLen(nLen,pVal->fYVal );
		}
	}
	else if (m_eDataType == StrVal) 
	{
		for(i=0;i<nCount;i++)
		{
			TDataPointStrVal* pVal = (TDataPointStrVal*)m_Data[i];
			AddStringLen(nLen,pVal->fXVal );
			AddDigitLen(nLen,pVal->fYVal );
		}
	}
	else if (m_eDataType == StrArr)
	{
		for(i=0;i<nCount;i++)
		{
			TDataPointStrArr* pVal = (TDataPointStrArr*)m_Data[i];
			AddStringLen(nLen,pVal->strName);
			int nC = pVal->arrYVal.GetSize();
			AddDigitLen(nLen,nC);
			nLen+= nC*sizeof(double);
		}
	}
	else if (m_eDataType == ThrArr) //add by xds 20060428
	{
		for (i=0;i<nCount;i++)
		{
			tagDataPointPedigree *pVal = (tagDataPointPedigree *)m_Data[i];
			int nSize = pVal->arrXAxis.GetSize();
			AddDigitLen(nLen,nSize);
			nLen += nSize*sizeof(double);
			nSize = pVal->arrYAxis.GetSize();
			AddDigitLen(nLen,nSize);//字符串个数
			for (j=0; j<nSize; j++)
			{
				AddStringLen(nLen,pVal->arrYAxis.GetAt(j));
			}
			nSize = pVal->arrPoint.GetSize();
			AddDigitLen(nLen,nSize);
			nLen += nSize*sizeof(double);
		}
	}
	else if (m_eDataType == FriArr) //add by xds 20060510
	{
		for (i=0; i<nCount; i++)
		{
			tagDataPointDecTree *pVal = (tagDataPointDecTree *)m_Data[i];
			int nSize = pVal->arrRectangle.GetSize();
			AddDigitLen(nLen,nSize);
			nLen += nSize*sizeof(double);
			nSize = pVal->szArrRectangle.GetSize();
			AddDigitLen(nLen,nSize);//字符串个数
			for (j=0; j<nSize; j++)
			{
				AddStringLen(nLen,pVal->szArrRectangle.GetAt(j));
			}
			nSize = pVal->arrRectFill.GetSize();
			AddDigitLen(nLen, nSize);
			nLen += nSize*sizeof(double);
			nSize = pVal->szArrRectFill.GetSize();
			AddDigitLen(nLen,nSize);//字符串个数
			for (j=0; j<nSize; j++)
			{
				AddStringLen(nLen,pVal->szArrRectFill.GetAt(j));
			}
			nSize = pVal->arrLinePoint.GetSize();
			AddDigitLen(nLen, nSize);
			nLen += nSize*sizeof(double);
		}
	}
	return nLen;
}
