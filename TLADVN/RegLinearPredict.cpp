/*// RegLinearPredict.cpp: implementation of the CRegLinearPredict class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RegLinearPredict.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegLinearPredict::CRegLinearPredict()
{
	m_bConfidInterval = true;//是否给出置信区间。默认值为 1；
	m_dConfidence = 0.95;//置信水平，取值0－1之间，默认值为0.95
	m_nDependIndex = -1;//因变量在新的数据文件中的下标
	
}

CRegLinearPredict::~CRegLinearPredict()
{
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
	
}
//主函数，调用预测模块的对外接口函数。
//返回一个矩阵。
//如果没有要求输出置信区间，那么得到的就是一个n×1的矩阵，否则为n×3的矩阵，其中n为新的观测个数
CDoubleMatrix CRegLinearPredict::OnRegLinearPredict(CDataInterface* pDataInterface,CRegLinearPredictVo* pVo)
{
	CDoubleMatrix MatRet;//准备存储返回的矩阵
	if(pVo == NULL)//确认设置项非空。
		return MatRet;

//第一步，获取模型
	m_bConfidInterval = pVo->m_bConfidInterval;
	m_nDependIndex = pVo->m_nDependIndex;
	m_tVarInfo.Copy(pVo->m_tVarInfo);
	if(pVo->pRegressLinearResult == NULL)//确认非空
	   return MatRet;

	TRegressLinearResult* pModel = new TRegressLinearResult;
	*pModel =	*(pVo->pRegressLinearResult);
	
	
//第二步 ，获取数据
	CDoubleMatrix Matrix_NewX;
	CDoubleMatrix Matrix_Y;

	if(GetData(pDataInterface,pModel,Matrix_NewX,Matrix_Y) == true)
//第三步，进行预测
	 MatRet = GetPredictY(Matrix_NewX,pModel);
  
//清除部分内存
	pModel->clear();
	delete pModel;
	pModel = NULL;
	
	return MatRet;
}


bool CRegLinearPredict::GetData(CDataInterface* pDataInterface,TRegressLinearResult* pRegResult,CDoubleMatrix& Matrix_NewX ,CDoubleMatrix& Matrix_Y)
{
	//取回数据，组织输入数据矩阵
	int i = 0;
	int j = 0;
	int m = m_tVarInfo.iCount +1;
	int nColNum = 0;
	bool nDataError = false;
	
	TDColAry * DataArray=NULL;	
	TCDataMatrix  rColArray;
	CDWordArray arCol;
	m = m_tVarInfo.iCount + 1;
	assert(m >= 2);
	for(i=0;i<m_tVarInfo.iCount;i++)
	{
		nColNum=m_tVarInfo.pValue[i];
		arCol.Add(nColNum);
		if( nColNum < 0 )
			return false;
		
		DataArray= new TDColAry;
		pDataInterface->GetColData(nColNum,*DataArray,5);//取回数据
		if( DataArray )
			rColArray.push_back( DataArray );
	}
	
	//取回因变量的数据
	if(m_nDependIndex>=0)
	{
		arCol.Add(m_nDependIndex);
		DataArray = new TDColAry;
		pDataInterface->GetColData(m_nDependIndex,*DataArray,5);//取回数据
		if( DataArray )
			rColArray.push_back( DataArray );
	}	
	
	CDoubleVector Vect_Data;
	int nCaseNum = 0;
		CIntVector vectAvail = CheckAvailData(pDataInterface,arCol);
		//计算实际样本数
		Matrix_NewX.create(vectAvail.sum(),rColArray.size()-1);
		Matrix_Y.create(vectAvail.sum(),1);

		for( j=0; j<rColArray.size(); j++ )
		{
			TDColAry * DataSet = rColArray.at(j);
			nCaseNum =0;
			for( i=0; i<pDataInterface->GetRowCount(); i++ )
			{
				if(vectAvail(i) == 1)
				{
					if(j< rColArray.size()-1)
						Matrix_NewX(nCaseNum,j) = DataSet->GetAt(i).dValue;
					else
						Matrix_Y(nCaseNum,0) = DataSet->GetAt(i).dValue;
					nCaseNum++;
				}
			}
		}
	for( i=0; i<rColArray.size(); i++ )
			delete rColArray.at(i);
	return true;
}

CDoubleMatrix CRegLinearPredict::GetPredictY(CDoubleMatrix Matrix_NewX//新的自变量观测值
			       , TRegressLinearResult* pRegResult)//线性回归模型
{
	CDoubleMatrix MatRet;
	int nRowCount = Matrix_NewX.mrow();
	int nColCount = Matrix_NewX.mcol();
	if(nRowCount*nColCount == 0) //为空矩阵
		return MatRet;
	CDoubleVector Vect_Beta = pRegResult->m_VectBeta;
    CDoubleMatrix Matrix_PridectY;
	if(pRegResult->m_bWithConst)//带常数项的
	{
		CDoubleMatrix Matrix_Ones(nRowCount,1,1.0);
		Matrix_NewX =  Matrix_Ones|Matrix_NewX;
        nColCount = nColCount+1;
	}
	//计算预测值
	CDoubleMatrix Matrix_Beta = VectorToMatrix(Vect_Beta,nColCount,1);

	Matrix_PridectY = Matrix_NewX * Matrix_Beta;
	MatRet  =	MatRet|Matrix_PridectY;
	
	//是否需要输出置信区间
	if(m_bConfidInterval)
	{
		 CDoubleMatrix m_H = pRegResult->m_MatrixH;
		 double range = 0;
		 CDoubleMatrix Interval_Up(nRowCount,1);
		 CDoubleMatrix Interval_Down(nRowCount,1);
		 assert(m_dConfidence <1 && m_dConfidence >0);
		 //置信区间半径的计算，常数部分为T分为数与残差标准差的乘积
		 double TempValue  =tinv( 1-(1-m_dConfidence)/2.0, nRowCount-nColCount)*pRegResult->m_dStdError; 
		 for(int j=0;j<nRowCount;j++)
		 {
			 CDoubleVector Vect_RowData = Matrix_NewX.row(j);
			 CDoubleMatrix Mat_RowData = VectorToMatrix(Vect_RowData,1,Matrix_NewX.mcol());
			 CDoubleMatrix temp = Mat_RowData * m_H * Mat_RowData.trans();
			 range = TempValue *sqrt((1+temp(0,0)));
			 Interval_Up(j,0)   = Matrix_PridectY(j,0) + range;
			 Interval_Down(j,0) = Matrix_PridectY(j,0) - range;

		 }	
 		MatRet  =	MatRet|Interval_Down;
		MatRet  =	MatRet|Interval_Up;
	}
  
  return MatRet;
}*/
