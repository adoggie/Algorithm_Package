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
				m_SystemErrorOld,m_SystemErrorNew,m_nSystemErrorLevel,//0,//����
				m_dStep,m_nMaxTrainTimes,//0,IDC_SYSTEM_ERROR,IDC_TRAIN_TIMES,this->m_hWnd,
				m_mxDemoDataInput,m_mxInput,m_mxIToHWeight,m_mxHideThreshold,
				m_mxHide,m_mxHToOWeight,m_mxOut,m_mxOutThreshold);

		break;


	case 1: // BP Arithmetic Selected
		bReturn = m_bpa.BPDemoDataTrainRepeat (	m_nInputLayerNumber,
				m_nHideLayerNumber,m_nOutputLayerNumber,
				m_bSimulateDataFlag,m_nComboFunc,m_SystemErrorOld,
				m_SystemErrorNew,m_nSystemErrorLevel,//0,//����
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
//****************  ��������  *******************
/*
 *	���룺mxSour
 *  �����mxSimuResult
 */
bool CBP::SimulateData(CBPMatrix &mxSour,CBPMatrix& mxSimuResult)
{
	m_bSimulateDataFlag = true;


	m_mxDemoDataInput.CopyMatrix(mxSour);
	// װ�ط��������ļ�
	//m_mxDemoDataInput.LoadDataFromFile (m_strSimuDataInput);


	if( m_mxDemoDataInput.GetMatrixColNumber () != m_nInputLayerNumber )
	{
// 		::MessageBox (this->m_hWnd, _T("�����������ļ����������Ŀ�������е��������Ŀ�����!!!"), _T("����!"), MB_OK | MB_ICONERROR);

		return false;
	}
	else // Successful!!!
	{
		// ǰ�����
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
		// �洢ģ����
		mxSimuResult.CopyMatrix(m_mxOut.Transpose());

	}
	// �ͷ�ȫ�ֱ�����ռ���ڴ�
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
 *	ѵ�����ݣ�
 *  ����mxSour������ �������
 *  ���m_mxTrainOut
 */
bool CBP::TrainingDemo(CBPMatrix &mxSour)
{
	m_bSimulateDataFlag = false;
	
	if(m_nInputLayerNumber <= 0)//�������Ŀ
	{
// 		::MessageBox (this->m_hWnd, _T("�������Ŀ����Ϊ���ڵ���1��������!"), _T(":("), MB_OK);
		return false;
	}

	if(m_nHideLayerNumber <= 0)//��������Ŀ
	{
// 		::MessageBox (this->m_hWnd, _T("��������Ŀ����Ϊ���ڵ���1��������!"), _T(":("), MB_OK);
		return false;
	}

	if(m_nOutputLayerNumber <= 0)//�������Ŀ
	{
// 		::MessageBox (this->m_hWnd, _T("�������Ŀ����Ϊ���ڵ���1��������!"), _T(":("), MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if(m_nComboArithmetic == -1)//ѵ�����㷨
	{
// 		::MessageBox (this->m_hWnd, _T("�㻹û��ѡȡ����ѵ�����㷨!"), _T(":("), MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if(m_nComboFunc == -1)//�����
	{
// 		::MessageBox (this->m_hWnd, _T("�㻹û��ѡȡ����ѵ���ļ����!"), _T(":("), MB_OK | MB_ICONINFORMATION);
		return false;
	}



	// װ����Դ
	//m_mxDemoDataInput.LoadDataFromFile (m_strDemoDataInput);
	m_mxDemoDataInput.CopyMatrix(mxSour);
	//m_mxDemoDataInput.SetMatrixRowAndCol(mxSour.GetMatrixRowNumber(),mxSour.GetMatrixRowNumber());
	//m_mxDemoDataInput = mxSour;
// 	TRACE("ԭʼ���� %d,%d\r\n",m_mxDemoDataInput.GetMatrixColNumber(),m_mxDemoDataInput.GetMatrixRowNumber());

	int nNum = m_mxDemoDataInput.GetMatrixColNumber();
	if((m_nInputLayerNumber + m_nOutputLayerNumber) != nNum )
	{
// 		::MessageBox (this->m_hWnd, _T("��������������������Ŀ֮���뱻��ȡ�ļ��е���Ŀ�����!"), _T("����!"), MB_OK | MB_ICONERROR);
		return false;
	}

	// ��һ��ǰ������ʼ��
	ForwardCalculateInit();

	// ������������
	bool	bSuccess = DemoDataTrainRepeat();

	if(bSuccess) // Successful!
	{
		//	�洢ģ������������ֵ����	
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
		pStream->WriteString("## ��������������Ŀ: ##");
		pStream->WriteInt(m_nInputLayerNumber);

		pStream->WriteString("## ���������������Ŀ: ##");
		pStream->WriteInt(m_nHideLayerNumber);
	
		pStream->WriteString("## ��������������Ŀ: ##");
		pStream->WriteInt(m_nOutputLayerNumber);

		pStream->WriteString("## ѵ�����������õ��������㷨: ##");
		pStream->WriteInt(m_nComboArithmetic);
		
		pStream->WriteString("## ������ʹ�õĺ���������ֵ: ##");
		pStream->WriteInt(m_nComboFunc);
		
		pStream->WriteString("## ����㵽�������Ȩֵ����: ##");
		m_mxIToHWeight.Serialize(pStream,IsStoring);
		
		pStream->WriteString( "## ����㵽�������Ȩֵ���� ##");	
		m_mxIToHWeight.Serialize (pStream,IsStoring);
		
		pStream->WriteString("## ������ķ�ֵ���� ##");
		m_mxHideThreshold.Serialize (pStream,IsStoring);
		
		pStream->WriteString("## �����㵽������Ȩֵ���� ##");
		m_mxHToOWeight.Serialize (pStream,IsStoring);
		
		pStream->WriteString("## �����ķ�ֵ���� ##");
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
		pStream->ReadString(strTemp);//"## ��������������Ŀ: ##"
		pStream->ReadInt(m_nInputLayerNumber);

		pStream->ReadString(strTemp);//"## ���������������Ŀ: ##");
		pStream->ReadInt(m_nHideLayerNumber);
	
		pStream->ReadString(strTemp);//"## ��������������Ŀ: ##");
		pStream->ReadInt(m_nOutputLayerNumber);

		pStream->ReadString(strTemp);//"## ѵ�����������õ��������㷨: ##");
		pStream->ReadInt(m_nComboArithmetic);
		
		pStream->ReadString(strTemp);//"## ������ʹ�õĺ���������ֵ: ##");
		pStream->ReadInt(m_nComboFunc);
		
		pStream->ReadString(strTemp);//"## ����㵽�������Ȩֵ����: ##");
		m_mxIToHWeight.Serialize(pStream,IsStoring);
		
		pStream->ReadString( strTemp);//"## ����㵽�������Ȩֵ���� ##");	
		m_mxIToHWeight.Serialize (pStream,IsStoring);
		
		pStream->ReadString(strTemp);//"## ������ķ�ֵ���� ##");
		m_mxHideThreshold.Serialize (pStream,IsStoring);
		
		pStream->ReadString(strTemp);//"## �����㵽������Ȩֵ���� ##");
		m_mxHToOWeight.Serialize (pStream,IsStoring);
		
		pStream->ReadString(strTemp);//"## �����ķ�ֵ���� ##");
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

