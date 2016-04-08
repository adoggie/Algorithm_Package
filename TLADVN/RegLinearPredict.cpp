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
	m_bConfidInterval = true;//�Ƿ�����������䡣Ĭ��ֵΪ 1��
	m_dConfidence = 0.95;//����ˮƽ��ȡֵ0��1֮�䣬Ĭ��ֵΪ0.95
	m_nDependIndex = -1;//��������µ������ļ��е��±�
	
}

CRegLinearPredict::~CRegLinearPredict()
{
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
	
}
//������������Ԥ��ģ��Ķ���ӿں�����
//����һ������
//���û��Ҫ������������䣬��ô�õ��ľ���һ��n��1�ľ��󣬷���Ϊn��3�ľ�������nΪ�µĹ۲����
CDoubleMatrix CRegLinearPredict::OnRegLinearPredict(CDataInterface* pDataInterface,CRegLinearPredictVo* pVo)
{
	CDoubleMatrix MatRet;//׼���洢���صľ���
	if(pVo == NULL)//ȷ��������ǿա�
		return MatRet;

//��һ������ȡģ��
	m_bConfidInterval = pVo->m_bConfidInterval;
	m_nDependIndex = pVo->m_nDependIndex;
	m_tVarInfo.Copy(pVo->m_tVarInfo);
	if(pVo->pRegressLinearResult == NULL)//ȷ�Ϸǿ�
	   return MatRet;

	TRegressLinearResult* pModel = new TRegressLinearResult;
	*pModel =	*(pVo->pRegressLinearResult);
	
	
//�ڶ��� ����ȡ����
	CDoubleMatrix Matrix_NewX;
	CDoubleMatrix Matrix_Y;

	if(GetData(pDataInterface,pModel,Matrix_NewX,Matrix_Y) == true)
//������������Ԥ��
	 MatRet = GetPredictY(Matrix_NewX,pModel);
  
//��������ڴ�
	pModel->clear();
	delete pModel;
	pModel = NULL;
	
	return MatRet;
}


bool CRegLinearPredict::GetData(CDataInterface* pDataInterface,TRegressLinearResult* pRegResult,CDoubleMatrix& Matrix_NewX ,CDoubleMatrix& Matrix_Y)
{
	//ȡ�����ݣ���֯�������ݾ���
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
		pDataInterface->GetColData(nColNum,*DataArray,5);//ȡ������
		if( DataArray )
			rColArray.push_back( DataArray );
	}
	
	//ȡ�������������
	if(m_nDependIndex>=0)
	{
		arCol.Add(m_nDependIndex);
		DataArray = new TDColAry;
		pDataInterface->GetColData(m_nDependIndex,*DataArray,5);//ȡ������
		if( DataArray )
			rColArray.push_back( DataArray );
	}	
	
	CDoubleVector Vect_Data;
	int nCaseNum = 0;
		CIntVector vectAvail = CheckAvailData(pDataInterface,arCol);
		//����ʵ��������
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

CDoubleMatrix CRegLinearPredict::GetPredictY(CDoubleMatrix Matrix_NewX//�µ��Ա����۲�ֵ
			       , TRegressLinearResult* pRegResult)//���Իع�ģ��
{
	CDoubleMatrix MatRet;
	int nRowCount = Matrix_NewX.mrow();
	int nColCount = Matrix_NewX.mcol();
	if(nRowCount*nColCount == 0) //Ϊ�վ���
		return MatRet;
	CDoubleVector Vect_Beta = pRegResult->m_VectBeta;
    CDoubleMatrix Matrix_PridectY;
	if(pRegResult->m_bWithConst)//���������
	{
		CDoubleMatrix Matrix_Ones(nRowCount,1,1.0);
		Matrix_NewX =  Matrix_Ones|Matrix_NewX;
        nColCount = nColCount+1;
	}
	//����Ԥ��ֵ
	CDoubleMatrix Matrix_Beta = VectorToMatrix(Vect_Beta,nColCount,1);

	Matrix_PridectY = Matrix_NewX * Matrix_Beta;
	MatRet  =	MatRet|Matrix_PridectY;
	
	//�Ƿ���Ҫ�����������
	if(m_bConfidInterval)
	{
		 CDoubleMatrix m_H = pRegResult->m_MatrixH;
		 double range = 0;
		 CDoubleMatrix Interval_Up(nRowCount,1);
		 CDoubleMatrix Interval_Down(nRowCount,1);
		 assert(m_dConfidence <1 && m_dConfidence >0);
		 //��������뾶�ļ��㣬��������ΪT��Ϊ����в��׼��ĳ˻�
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
