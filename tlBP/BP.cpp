// BP.cpp: implementation of the CBP class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "BP.h"
#include "BPNet.h"
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

CBP::CBP()
{
	Reset();
}

CBP::~CBP()
{

}

void CBP::ForwardCalculateInit()
{
	m_bpa.LMForwardCalculateInit( m_nInputLayerNumber,m_nHideLayerNumber,
	m_nOutputLayerNumber,m_mxDemoDataInput,	m_mxInput,	m_mxIToHWeight,
	m_mxHideThreshold,m_mxHToOWeight,	m_mxOutThreshold);
}

bool CBP::DemoDataTrainRepeat()
{
	bool	bReturn = false;

	switch( m_nComboArithmetic)
	{
	case 0: // LM Arithmetic Selected
		bReturn = m_bpa.LMDemoDataTrainRepeat (	m_nInputLayerNumber,m_nHideLayerNumber,
				m_nOutputLayerNumber,m_bSimulateDataFlag,m_nComboFunc,
				m_SystemErrorOld,m_SystemErrorNew,m_nSystemErrorLevel,//0,//无用
				m_dStep,m_nMaxTrainTimes,//0,IDC_SYSTEM_ERROR,IDC_TRAIN_TIMES,this->m_hWnd,
				m_mxDemoDataInput,m_mxInput,m_mxIToHWeight,m_mxHideThreshold,
				m_mxHide,m_mxHToOWeight,m_mxOut,m_mxOutThreshold);

		break;


	case 1: // BP Arithmetic Selected
		bReturn = m_bpa.BPDemoDataTrainRepeat (	m_nInputLayerNumber,
				m_nHideLayerNumber,m_nOutputLayerNumber,
				m_bSimulateDataFlag,m_nComboFunc,m_SystemErrorOld,
				m_SystemErrorNew,m_nSystemErrorLevel,//0,//无用
				m_dStep,m_nMaxTrainTimes,
				//0, IDC_SYSTEM_ERROR,IDC_TRAIN_TIMES,this->m_hWnd,
				m_mxDemoDataInput,m_mxInput,m_mxIToHWeight,
				m_mxHideThreshold,m_mxHide,m_mxHToOWeight,
				m_mxOut,m_mxOutThreshold);

		break;

	default: // No Arithmetic Selected
		return bReturn;

	}

	return bReturn;
}
//****************  仿真数据  *******************
/*
 *	输入：mxSour
 *  输出：mxSimuResult
 */
bool CBP::SimulateData(CBPMatrix &mxSour,CBPMatrix& mxSimuResult)
{
	m_bSimulateDataFlag = true;


	m_mxDemoDataInput.CopyMatrix(mxSour);
	// 装载仿真数据文件
	//m_mxDemoDataInput.LoadDataFromFile (m_strSimuDataInput);


	if( m_mxDemoDataInput.GetMatrixColNumber () != m_nInputLayerNumber )
	{
// 		::MessageBox (this->m_hWnd, _T("待仿真数据文件的输入层数目与网络中的输入层数目不相等!!!"), _T("错误!"), MB_OK | MB_ICONERROR);

		return false;
	}
	else // Successful!!!
	{
		// 前向计算
		switch(m_nComboArithmetic)
		{
		case 0: // LM Arithmetic Selected
			m_bpa.LMForwardCalculate( m_nInputLayerNumber,m_nHideLayerNumber,
				m_nOutputLayerNumber,m_bSimulateDataFlag,m_nComboFunc,m_mxDemoDataInput,
				m_mxInput,m_mxIToHWeight,m_mxHideThreshold,m_mxHide,
				m_mxHToOWeight,	m_mxOut,	m_mxOutThreshold);
			break;
		case 1: // BP Arithmetic Selected
			m_bpa.BPForwardCalculate2(m_nInputLayerNumber,
					m_nHideLayerNumber,m_nOutputLayerNumber,m_bSimulateDataFlag,m_nComboFunc,
					m_mxDemoDataInput,m_mxInput,m_mxIToHWeight,m_mxHideThreshold,
					m_mxHide,m_mxHToOWeight,m_mxOut,m_mxOutThreshold);
			break;
		default: // No Arithmetic Selected
			return false;

		}
		// 存储模拟结果
		mxSimuResult.CopyMatrix(m_mxOut.Transpose());

	}
	// 释放全局变量所占的内存
	/*
	m_mxSimuNet.InitializeZero ();
	
	m_mxIToHWeight.InitializeZero ();
	m_mxHideThreshold.InitializeZero ();
	m_mxHToOWeight.InitializeZero ();
	m_mxOutThreshold.InitializeZero ();
	
	m_mxDemoDataInput.InitializeZero ();
*/
	return true;
}

/*
 *	训练数据，
 *  输入mxSour。。。 各项参数
 *  输出m_mxTrainOut
 */
bool CBP::TrainingDemo(CBPMatrix &mxSour)
{
	m_bSimulateDataFlag = false;
	
	if(m_nInputLayerNumber <= 0)//输入层数目
	{
// 		::MessageBox (this->m_hWnd, _T("输入层数目必须为大于等于1的正整数!"), _T(":("), MB_OK);
		return false;
	}

	if(m_nHideLayerNumber <= 0)//隐含层数目
	{
// 		::MessageBox (this->m_hWnd, _T("隐含层数目必须为大于等于1的正整数!"), _T(":("), MB_OK);
		return false;
	}

	if(m_nOutputLayerNumber <= 0)//输出层数目
	{
// 		::MessageBox (this->m_hWnd, _T("输出层数目必须为大于等于1的正整数!"), _T(":("), MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if(m_nComboArithmetic == -1)//训练的算法
	{
// 		::MessageBox (this->m_hWnd, _T("你还没有选取用来训练的算法!"), _T(":("), MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if(m_nComboFunc == -1)//激活函数
	{
// 		::MessageBox (this->m_hWnd, _T("你还没有选取用来训练的激活函数!"), _T(":("), MB_OK | MB_ICONINFORMATION);
		return false;
	}



	// 装载资源
	//m_mxDemoDataInput.LoadDataFromFile (m_strDemoDataInput);
	m_mxDemoDataInput.CopyMatrix(mxSour);
	//m_mxDemoDataInput.SetMatrixRowAndCol(mxSour.GetMatrixRowNumber(),mxSour.GetMatrixRowNumber());
	//m_mxDemoDataInput = mxSour;
// 	TRACE("原始数据 %d,%d\r\n",m_mxDemoDataInput.GetMatrixColNumber(),m_mxDemoDataInput.GetMatrixRowNumber());

	int nNum = m_mxDemoDataInput.GetMatrixColNumber();
	if((m_nInputLayerNumber + m_nOutputLayerNumber) != nNum )
	{
// 		::MessageBox (this->m_hWnd, _T("所输入的输入层和输出层数目之和与被读取文件中的数目不相符!"), _T("错误!"), MB_OK | MB_ICONERROR);
		return false;
	}

	// 第一次前向计算初始化
	ForwardCalculateInit();

	// 反复反馈运算
	bool	bSuccess = DemoDataTrainRepeat();

	if(bSuccess) // Successful!
	{
		//	存储模拟过输出层的输出值矩阵	
		m_mxTrainOut.ReSize(m_mxOut.GetMatrixColNumber(),m_mxOut.GetMatrixRowNumber());
		m_mxTrainOut = m_mxOut.Transpose();
	}
	return bSuccess;
}

bool CBP::Export(TCLFile &theFile)
{
	int i=0, j=0, nRow=0, nCol=0;

	//m_nInputLayerNumber
	theFile << m_nInputLayerNumber;
	//m_nHideLayerNumber
	theFile << m_nHideLayerNumber;
	//m_nOutputLayerNumber
	theFile << m_nOutputLayerNumber;
	//m_nComboArithmetic
	theFile << m_nComboArithmetic;
	//m_nComboFunc
	theFile << m_nComboFunc;	
	//m_SystemErrorOld
	theFile << m_SystemErrorOld; 
	//m_mxIToHWeight
	nRow = m_mxIToHWeight.GetMatrixRowNumber();
	theFile << nRow;
	nCol = m_mxIToHWeight.GetMatrixColNumber();
	theFile << nCol;
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			theFile << m_mxIToHWeight.m_pTMatrix[i][j];
		}
	}
	//m_mxHideThreshold
	nRow = m_mxHideThreshold.GetMatrixRowNumber();
	theFile << nRow;
	nCol = m_mxHideThreshold.GetMatrixColNumber();
	theFile << nCol;
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			theFile << m_mxHideThreshold.m_pTMatrix[i][j];
		}
	}
	//m_mxHToOWeight
	nRow = m_mxHToOWeight.GetMatrixRowNumber();
	theFile << nRow;
	nCol = m_mxHToOWeight.GetMatrixColNumber();
	theFile << nCol;
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			theFile << m_mxHToOWeight.m_pTMatrix[i][j];
		}
	}
	//m_mxOutThreshold
	nRow = m_mxOutThreshold.GetMatrixRowNumber();
	theFile << nRow;
	nCol = m_mxOutThreshold.GetMatrixColNumber();
	theFile << nCol;
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			theFile << m_mxOutThreshold.m_pTMatrix[i][j];
		}
	}
	return true;
}

bool CBP::Import(TCLFile &theFile)
{
	int i=0, j=0, nRow=0, nCol=0;

	//m_nInputLayerNumber
	theFile >> m_nInputLayerNumber;	
	//m_nHideLayerNumber
	theFile >> m_nHideLayerNumber;		
	//m_nOutputLayerNumber
	theFile >> m_nOutputLayerNumber;	
	//m_nComboArithmetic
	theFile >> m_nComboArithmetic;		
	//m_nComboFunc
	theFile >> m_nComboFunc;		
	//m_SystemErrorOld
	theFile >> m_SystemErrorOld;	
	//m_mxIToHWeight
	theFile >> nRow;
	theFile >> nCol;
	m_mxIToHWeight.ReSize(nRow,nCol);
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			theFile >> m_mxIToHWeight.m_pTMatrix[i][j];
		}
	}
	//m_mxHideThreshold
	theFile >> nRow;
	theFile >> nCol;
	m_mxHideThreshold.ReSize(nRow,nCol);
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			theFile >> m_mxHideThreshold.m_pTMatrix[i][j];
		}
	}
	//m_mxHToOWeight
	theFile >> nRow;
	theFile >> nCol;
	m_mxHToOWeight.ReSize(nRow,nCol);
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			theFile >> m_mxHToOWeight.m_pTMatrix[i][j];
		}
	}
	//m_mxOutThreshold
	theFile >> nRow;
	theFile >> nCol;
	m_mxOutThreshold.ReSize(nRow,nCol);
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			theFile >> m_mxOutThreshold.m_pTMatrix[i][j];
		}
	}
	return true;
}

void CBP::Serialize(CMemoStream *pStream, bool IsStoring)
{
	if (IsStoring)
	{
		pStream->WriteString("BPNET BEGIN");
		pStream->WriteString("## 样本的输入层的数目: ##");
		pStream->WriteInt(m_nInputLayerNumber);

		pStream->WriteString("## 样本的隐含层的数目: ##");
		pStream->WriteInt(m_nHideLayerNumber);
	
		pStream->WriteString("## 样本的输出层的数目: ##");
		pStream->WriteInt(m_nOutputLayerNumber);

		pStream->WriteString("## 训练网络所采用的神经网络算法: ##");
		pStream->WriteInt(m_nComboArithmetic);
		
		pStream->WriteString("## 网络中使用的函数的索引值: ##");
		pStream->WriteInt(m_nComboFunc);
		
		pStream->WriteString("## 输入层到隐含层的权值矩阵: ##");
		m_mxIToHWeight.Serialize(pStream,IsStoring);
		
		pStream->WriteString( "## 输入层到隐含层的权值矩阵 ##");	
		m_mxIToHWeight.Serialize (pStream,IsStoring);
		
		pStream->WriteString("## 隐含层的阀值矩阵 ##");
		m_mxHideThreshold.Serialize (pStream,IsStoring);
		
		pStream->WriteString("## 隐含层到输出层的权值矩阵 ##");
		m_mxHToOWeight.Serialize (pStream,IsStoring);
		
		pStream->WriteString("## 输出层的阀值矩阵 ##");
		m_mxOutThreshold.Serialize (pStream,IsStoring);
		pStream->WriteDouble(m_SystemErrorOld);
		pStream->WriteString("BPNET END");

	}
	else
	{
		CTString strTemp;
		pStream->ReadString(strTemp);//"IS BEGIN"
		if (strTemp !="BPNET BEGIN")
			return;
		pStream->ReadString(strTemp);//"## 样本的输入层的数目: ##"
		pStream->ReadInt(m_nInputLayerNumber);

		pStream->ReadString(strTemp);//"## 样本的隐含层的数目: ##");
		pStream->ReadInt(m_nHideLayerNumber);
	
		pStream->ReadString(strTemp);//"## 样本的输出层的数目: ##");
		pStream->ReadInt(m_nOutputLayerNumber);

		pStream->ReadString(strTemp);//"## 训练网络所采用的神经网络算法: ##");
		pStream->ReadInt(m_nComboArithmetic);
		
		pStream->ReadString(strTemp);//"## 网络中使用的函数的索引值: ##");
		pStream->ReadInt(m_nComboFunc);
		
		pStream->ReadString(strTemp);//"## 输入层到隐含层的权值矩阵: ##");
		m_mxIToHWeight.Serialize(pStream,IsStoring);
		
		pStream->ReadString( strTemp);//"## 输入层到隐含层的权值矩阵 ##");	
		m_mxIToHWeight.Serialize (pStream,IsStoring);
		
		pStream->ReadString(strTemp);//"## 隐含层的阀值矩阵 ##");
		m_mxHideThreshold.Serialize (pStream,IsStoring);
		
		pStream->ReadString(strTemp);//"## 隐含层到输出层的权值矩阵 ##");
		m_mxHToOWeight.Serialize (pStream,IsStoring);
		
		pStream->ReadString(strTemp);//"## 输出层的阀值矩阵 ##");
		m_mxOutThreshold.Serialize (pStream,IsStoring);
		pStream->ReadDouble(m_SystemErrorOld);
		pStream->ReadString(strTemp);//"IS END");
		if (strTemp !="BPNET END")
			return;
	}
}

void CBP::Abort()
{
	m_bpa.Abort();
}

void CBP::SetWnd()
{
}

void CBP::Reset()
{
	m_SystemErrorNew = MAX_SYSTEM_ERROR;
	m_SystemErrorOld = MAX_SYSTEM_ERROR;
	///////////
	m_nInputLayerNumber = 0;
	m_nHideLayerNumber = 0;
	m_nOutputLayerNumber = 0;
	
	m_nComboFunc = -1;
	m_nComboArithmetic = -1;
	

	m_nSystemErrorLevel = SYSTEM_ERROR_LEVEL;
	m_nMaxTrainTimes = LOOP_MAX_TIMES;
	
	m_SystemErrorNew = MAX_SYSTEM_ERROR;
	m_SystemErrorOld = MAX_SYSTEM_ERROR;
	
	m_dStep = INIT_STEP;
	m_bpa.Reset();
}

