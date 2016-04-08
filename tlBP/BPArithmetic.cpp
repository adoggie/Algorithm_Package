// BPArithmetic.cpp: implementation of the CBPArithmetic class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BPArithmetic.h"
//������ʼ
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
const unsigned int WM_PROGRESS_TEXT = RegisterWindowMessage("WM_PROGRESS_TEXT");
const unsigned int WM_PROGRESS_STEP = RegisterWindowMessage("WM_PROGRESS_STEP");
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBPArithmetic::CBPArithmetic()
{
        m_bAbort = false;
}

CBPArithmetic::~CBPArithmetic()
{

}



/////////////////////////////////////////////////////////////////////////////
//        Levenberg-Marquart                ---->        ��һ��ǰ������ʼ��                                   //
/////////////////////////////////////////////////////////////////////////////
void CBPArithmetic::LMForwardCalculateInit( int nInputLayerNumber,
                                                                                                        int nHideLayerNumber,
                                                                                                        int nOutputLayerNumber,
                                                                                                        CBPMatrix &m_mxDemoDataInput,
                                                                                                        CBPMatrix &m_mxInput,
                                                                                                        CBPMatrix &m_mxIToHWeight,
                                                                                                        CBPMatrix &m_mxHideThreshold,
                                                                                                        CBPMatrix &m_mxHToOWeight,
                                                                                                        CBPMatrix &m_mxOutThreshold
                                                                                                   )
{

        /************************************************************************
        *                                        --------->Use Matlab Method <---------              *
        ************************************************************************/

                
        /////////////////////////////////////////////////////////////////////////
        // ���������Ԫ�صľ���
        // �������:
        //                1. ������Ŀ��Ϊ���������;
        //                2. ������������������Ŀ��Ϊ���������;
        //                3. �����е�Ԫ�ؼ�Ϊ��Ӧ��������ֵ
        //
        {//===yh  in:m_mxDemoDataInput, nInputLayerNumber     out: m_mxInput
                CBPMatrix cm_mxInput(m_mxDemoDataInput.GetMatrixRowNumber (), nInputLayerNumber);
                // �õ�����������ֵ
                m_mxDemoDataInput.CopySubMatrix (cm_mxInput,(unsigned int)0,(unsigned int)0);
                
                CBPMatrix cMatrixTInputLayerValue = cm_mxInput.Transpose ();
                
                m_mxInput.CopyMatrix (cMatrixTInputLayerValue);
        }
        //TRACE("row %d,col:%d\r\n",m_mxInput.GetMatrixRowNumber(),m_mxInput.GetMatrixColNumber());


        /////////////////////////////////////////////////////////////////////////
        // ����Ȩֵ���� -----> �ɵ��������������������֮���Ȩֵ��ΪԪ�����
        // �������:
        //                1. ������������������Ŀ��Ϊ��������;
        //                2. �������������������Ŀ��Ϊ���������;
        //                3. �Ծ����е�Ԫ�ؽ��������ʼ��,ֵ��(-1,1)֮��;
        //                4. �������������������������Ŀ����ȵ�;
        //                5. ��������ʹ�õ���ͬһ��Ȩֵ����.
        //
        {//===yh        out  m_mxIToHWeight  
                CBPMatrix        cm_mxIToHWeight(nHideLayerNumber, nInputLayerNumber);
                
                // �����ʼ��������Ԫ�ص�ֵ
                cm_mxIToHWeight.RandomInitialize ();
                m_mxIToHWeight.CopyMatrix (cm_mxIToHWeight);
        }


        /////////////////////////////////////////////////////////////////////
        //        ��������������ķ�ֵ����
        //        �������:
        //                1. ��������Ŀ��Ϊ��������;
        //                2. �������������������Ŀ��Ϊ���������;
        //                3. �����ʼ�������е�Ԫ��,ʹֵ��(-1,1)֮��;
        //                4. ������ÿ�е����ݶ��͵�һ���������Ӧ��λ�����.
        //
        {
                CBPMatrix cm_mxHideThreshold(nHideLayerNumber,(unsigned int)1);
                
                // �����ʼ��������Ԫ�ص�ֵ
                cm_mxHideThreshold.RandomInitialize ();
                m_mxHideThreshold.CopyMatrix(cm_mxHideThreshold);
        }


        /////////////////////////////////////////////////////////////////////
        //        ����Ȩֵ����        ----->        �ɵ����������������������֮��Ȩֵ��ΪԪ��
        //                                                        ���
        //        �������:
        //                1. �������������������Ŀ��Ϊ���������;
        //                2. ������������������Ŀ��Ϊ���������;
        //                3. �Ծ����е�Ԫ�ؽ��������ʼ��,ֵ��(-1,1)֮��;
        //                4. �������������������������Ŀ����ȵ�;
        //                5. ��������ʹ�õ���ͬһ��Ȩֵ����.
        //

        CBPMatrix cm_mxHToOWeight(nOutputLayerNumber, nHideLayerNumber);

        // �Ծ����Ԫ�������ʼ��
        cm_mxHToOWeight.RandomInitialize ();

        m_mxHToOWeight.CopyMatrix (cm_mxHToOWeight);


        /////////////////////////////////////////////////////////////////////
        //        ���������������ķ�ֵ����
        //        �������:
        //                1. ��������Ŀ��Ϊ���������;
        //                2. ������������������Ŀ��Ϊ���������;
        //                3. �����ʼ�������е�Ԫ��,ʹֵ��(-1,1)֮��;
        //                4. ������ÿ�е����ݶ��͵�һ���������Ӧ��λ�����.
        //

        CBPMatrix cMatrixOutputLayerValveValue(nOutputLayerNumber,(unsigned int)1);

        // �����ʼ��������Ԫ�ص�ֵ
        cMatrixOutputLayerValveValue.RandomInitialize ();

        m_mxOutThreshold.CopyMatrix(cMatrixOutputLayerValveValue);


}

/////////////////////////////////////////////////////////////////////////////
//        Levenberg-Marquart                ---->                ǰ�����                                                   //
/////////////////////////////////////////////////////////////////////////////
void CBPArithmetic::LMForwardCalculate ( int nInputLayerNumber,
                                                                                                int nHideLayerNumber,
                                                                                                int nOutputLayerNumber,
                                                                                                bool bSimulateDataFlag,
                                                                                                int nComboFunc,
                                                                                                CBPMatrix &m_mxDemoDataInput,
                                                                                                CBPMatrix &m_mxInput,
                                                                                                CBPMatrix &m_mxIToHWeight,
                                                                                                CBPMatrix &m_mxHideThreshold,
                                                                                                CBPMatrix &m_mxHide,
                                                                                                CBPMatrix &m_mxHToOWeight,
                                                                                                CBPMatrix &m_mxOut,
                                                                                                CBPMatrix &m_mxOutThreshold
                                                                                           )
{

        if(bSimulateDataFlag)
        {

                CBPMatrix cm_mxInput(m_mxDemoDataInput.GetMatrixRowNumber (), nInputLayerNumber);

                // �õ�����������ֵ
                m_mxDemoDataInput.CopySubMatrix (cm_mxInput, (unsigned int)0, (unsigned int)0);

                CBPMatrix cMatrixTInputLayerValue = cm_mxInput.Transpose ();

                m_mxInput.CopyMatrix (cMatrixTInputLayerValue);


        }

        /////////////////////////////////////////////////////////////////////////
        // �õ�����������������ľ�����
        // �������:
        //                1. ��������Ŀ��Ϊ��������;
        //                2. �������������������Ŀ��Ϊ���������;
        //                3. ����Ԫ���е�ֵ��Ϊ��Ӧ��������������ľ�����:
        //                   �� 
        //                                cm_mxInput * cm_mxIToHWeight 
        //                                + cm_mxHideThreshold
        //                        �õ�.
        //
        CBPMatrix cmxHPureInput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
        //row *col 4* 321
        {
                CBPMatrix mxHideThresholdEx;
                mxHideThresholdEx.nncpyi (m_mxHideThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
                cmxHPureInput = m_mxIToHWeight * m_mxInput;
                //4*321          4*8             8*321
                
                cmxHPureInput += mxHideThresholdEx;
                
        }

        /////////////////////////////////////////////////////////////////////
        //        �����������������������
        //        �������:
        //                1. ����������y�������������x�Ĺ�ϵ���ú�����ʾ
        //                        y = f(x)
        //                2. �����ά����������ľ���������ά�����
        //

        CBPMatrix cMatrixHideLayerOutput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixHideLayerOutput = cmxHPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixHideLayerOutput = cmxHPureInput.tanh ();
                break;

        case 2:
                cMatrixHideLayerOutput = cmxHPureInput.Tansig();
                break;

        default:
                return;
        }
        
        m_mxHide.CopyMatrix(cMatrixHideLayerOutput);
        m_mxHide.Print("m_mxHide");
        // 4* 321 
        /////////////////////////////////////////////////////////////////////
        //        �õ��������������ľ�����
        //        �������;
        //                1. ��������Ŀ��Ϊ���������;
        //                2. ������������������Ŀ��Ϊ���������;
        //                3. ������Ԫ�ص�ֵ��Ϊ��Ӧ�����������ľ�����:
        //                        ��
        //                                cMatrixHideLayerOutput * cm_mxHToOWeight
        //                                + cMatrixOutputLayerValveValue
        //                        �õ�
        //

        CBPMatrix cMatrixExOutputLayerValveValue;
        cMatrixExOutputLayerValveValue.nncpyi (m_mxOutThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
        //1 * 321 
        CBPMatrix cMatrixOutputLayerPureInput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
        // 1 * 321
        cMatrixOutputLayerPureInput = m_mxHToOWeight * cMatrixHideLayerOutput;
        // 1 * 321                    1 * 4              4*321
        cMatrixOutputLayerPureInput  += cMatrixExOutputLayerValveValue;


        /////////////////////////////////////////////////////////////////////
        //        ����������������������
        //        �������:
        //                1. �����ά����õ������������������ľ�������ɵľ���һ��;
        //                2. ���������y��������������ù�ϵʽ
        //                        y = f(x)
        //                ��ʾ
        //

        CBPMatrix cMatrixOutputLayerOutput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Tansig ();
                break;

        default:
                return ;
        }

        m_mxOut.CopyMatrix(cMatrixOutputLayerOutput);
m_mxOut.Print("m_mxOut");

        //1 * 321

}


/////////////////////////////////////////////////////////////////////////////
//        Levenberg-Marquart                ---->                ��������                                                   //
/////////////////////////////////////////////////////////////////////////////
bool CBPArithmetic::LMDemoDataTrainRepeat (        int nInputLayerNumber,
                                                                                                        int nHideLayerNumber,
                                                                                                        int nOutputLayerNumber,
                                                                                                        bool bSimulateDataFlag,
                                                                                                        int nComboFunc,
                                                                                                        double nSystemErrorOld,
                                                                                                        double nSystemErrorNew,
                                                                                                        double nSystemErrorLevel,
                                                                                                        //double nSystemError,//���� ===temp
                                                                                                        double nStep,
                                                                                                        unsigned int nMaxTrainTimes,
                                                                                                        //unsigned int nTrainTimes,//���� ===temp
                                                                                                        //DWORD ID_SYSTEM_ERROR,
                                                                                                        //DWORD ID_TRAIN_TIMES,
                                                                                                        CBPMatrix &m_mxDemoDataInput,
                                                                                                        CBPMatrix &m_mxInput,
                                                                                                        CBPMatrix &m_mxIToHWeight,
                                                                                                        CBPMatrix &m_mxHideThreshold,
                                                                                                        CBPMatrix &m_mxHide,
                                                                                                        CBPMatrix &m_mxHToOWeight,
                                                                                                        CBPMatrix &m_mxOut,
                                                                                                        CBPMatrix &m_mxOutThreshold
                                                                                                 )
{

        LMForwardCalculate (nInputLayerNumber,
                                                nHideLayerNumber,
                                                nOutputLayerNumber,
                                                bSimulateDataFlag,
                                                nComboFunc,
                                                m_mxDemoDataInput,
                                                m_mxInput,
                                                m_mxIToHWeight,
                                                m_mxHideThreshold,
                                                m_mxHide,
                                                m_mxHToOWeight,
                                                m_mxOut,
                                                m_mxOutThreshold
                                                );
                

        /////////////////////////////////////////////////////////////////////
        //        �������������������delta����
        //        �������:
        //                1. ��������ĿΪ���������;
        //                2. ������������ĿΪ���������;
        //                3. �����е�Ԫ�ص�ֵyΪ:
        //                                y = -(ǰ��������������ֵ - ������������ֵ) * f'(net)
        //


        CBPMatrix cMatrixTDemoOutput(m_mxDemoDataInput.GetMatrixRowNumber (), nOutputLayerNumber);
                
        // �õ�����������������
        m_mxDemoDataInput.CopySubMatrix (cMatrixTDemoOutput, (unsigned int)nInputLayerNumber, (unsigned int)0);

        CBPMatrix cMatrixDemoOutput = cMatrixTDemoOutput.Transpose ();

        // �õ����������������
        CBPMatrix cMatrixOutputLayerError(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixOutputLayerError = cMatrixDemoOutput - m_mxOut;

                
        nSystemErrorOld = cMatrixOutputLayerError.GetSystemError ();

        for(int nLoopTimes=1; nLoopTimes < nMaxTrainTimes; nLoopTimes++)        
        {
                if(nSystemErrorOld < nSystemErrorLevel)
                {
                        nLoopTimes--;
                        break;
                }

                CBPMatrix        cMatrixExHideLayerOutput;
                cMatrixExHideLayerOutput.nncpyi (m_mxHide, nOutputLayerNumber);


                CBPMatrix        cMatrixOutputLayerDelta (nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber());
                
                // ע��: �˴�'/' �� '���'!!!
                cMatrixOutputLayerDelta = 1 - m_mxOut / m_mxOut;

                CBPMatrix cMatrixExOutputLayerDelta;
                cMatrixExOutputLayerDelta.nncpyd (cMatrixOutputLayerDelta);
                
                cMatrixExOutputLayerDelta = cMatrixExOutputLayerDelta * (-1.0);

        
                CBPMatrix cMatrixTHideToOutputWeightValue (m_mxHToOWeight.GetMatrixColNumber(), m_mxHToOWeight.GetMatrixRowNumber());
                cMatrixTHideToOutputWeightValue = m_mxHToOWeight.Transpose();

                CBPMatrix cMatrixExHideLayerDelta;
                // ע��: �˴�'/' �� '���'!!!
                cMatrixExHideLayerDelta.CopyMatrix ( (1 - (cMatrixExHideLayerOutput / cMatrixExHideLayerOutput)) / ( cMatrixTHideToOutputWeightValue * cMatrixExOutputLayerDelta) );
                                

                CBPMatrix        cMatrixExInputLayerValue;
                cMatrixExInputLayerValue.nncpyi (m_mxInput, nOutputLayerNumber);

                
                CBPMatrix        cMatrixJ11;
                cMatrixJ11.nncpy (cMatrixExHideLayerDelta.Transpose(), cMatrixExInputLayerValue.GetMatrixRowNumber ());

                CBPMatrix cMatrixJ12;
                cMatrixJ12.nncpyi(cMatrixExInputLayerValue.Transpose (), cMatrixExHideLayerDelta.GetMatrixRowNumber());


                CBPMatrix        cMatrixJ1;
                // ע��: �˴�'/' �� '���'!!!
                cMatrixJ1.CopyMatrix (cMatrixJ11 / cMatrixJ12);


                CBPMatrix cMatrixJ21;
                cMatrixJ21.nncpy (cMatrixExOutputLayerDelta.Transpose (), cMatrixExHideLayerOutput.GetMatrixRowNumber ());

                CBPMatrix cMatrixJ22;
                cMatrixJ22.nncpyi (cMatrixExHideLayerOutput.Transpose (), cMatrixExOutputLayerDelta.GetMatrixRowNumber ());

                CBPMatrix cMatrixJ2;
                // ע��: �˴�'/' �� '���'!!!
                cMatrixJ2.CopyMatrix (cMatrixJ21 / cMatrixJ22);

                
                CBPMatrix cMatrixZ;
                cMatrixZ.CopyMatrix (MergeMatrix(MergeMatrix(MergeMatrix(cMatrixJ1, cMatrixExHideLayerDelta.Transpose ()), cMatrixJ2), cMatrixExOutputLayerDelta.Transpose ()));


                CBPMatrix cMatrixMOutputLayerError;
                cMatrixMOutputLayerError.CopyMatrix ( cMatrixOutputLayerError.MergeColumnsToColumnVector () );


                CBPMatrix        cMatrixJE;
                cMatrixJE.CopyMatrix ( (cMatrixZ.Transpose ()) * cMatrixMOutputLayerError );


                CBPMatrix cMatrixJJ;
                cMatrixJJ.CopyMatrix ( (cMatrixZ.Transpose ()) * cMatrixZ );
                

                // �����µ�����㵽�������Ȩֵ
                CBPMatrix cMatrixNewInputToHideWeight;
                // ������µ�������ķ�ֵ
                CBPMatrix cMatrixNewHideLayerValve;
                // �����µ������㵽������Ȩֵ
                CBPMatrix cMatrixNewHideToOutputWeight;
                // �����µ������ķ�ֵ
                CBPMatrix cMatrixNewOutputLayerValve;

                // �����µ�������
                CBPMatrix cMatrixNewOutputLayerError(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

                /////////////////////////////////////////////////////////////////
                // the weight value is adjusted

                while(nStep <= MAX_ADJUST_VALUE)
                {
                        CBPMatrix        cMatrixI (cMatrixZ.GetMatrixColNumber (), cMatrixZ.GetMatrixColNumber ());

                        cMatrixI.Eye ();


                        CBPMatrix cMatrixDX;
                        cMatrixDX.CopyMatrix ( (((cMatrixJJ + cMatrixI * nStep).Inverse ()) * cMatrixJE) * (-1.0) );
                                

                        /////////////////////////////////////////////////////////////////////////
                        // ���cMatrixDX����

                        unsigned int nIndex = 0;
                                
                        // �õ�����㵽�������Ȩֵ��������
                        CBPMatrix cMatrixInputToHideWeightChange(nHideLayerNumber, nInputLayerNumber);

                        cMatrixDX.CopySubMatrixFromVector (cMatrixInputToHideWeightChange, nIndex);

                        nIndex += nHideLayerNumber * nInputLayerNumber;

                        // �õ������㷧ֵ��������
                        CBPMatrix cMatrixHideLayerValveChange (nHideLayerNumber, (unsigned int)1);
                                
                        cMatrixDX.CopySubMatrixFromVector (cMatrixHideLayerValveChange, nIndex);


                        nIndex += nHideLayerNumber;

                        // �õ������㵽������Ȩֵ��������
                        CBPMatrix cMatrixHideToOutputWeightChange (nOutputLayerNumber, nHideLayerNumber);
                                
                        cMatrixDX.CopySubMatrixFromVector (cMatrixHideToOutputWeightChange, nIndex);

                        nIndex += nOutputLayerNumber * nHideLayerNumber;

                        // �õ�����㷧ֵ������ֵ
                        CBPMatrix cMatrixOutputValveChange (nOutputLayerNumber, (unsigned int)1);

                        cMatrixDX.CopySubMatrixFromVector (cMatrixOutputValveChange, nIndex);
        
                        cMatrixNewInputToHideWeight.CopyMatrix (m_mxIToHWeight + cMatrixInputToHideWeightChange);

                        cMatrixNewHideLayerValve.CopyMatrix (m_mxHideThreshold + cMatrixHideLayerValveChange);

                        cMatrixNewHideToOutputWeight.CopyMatrix (m_mxHToOWeight + cMatrixHideToOutputWeightChange);

                        cMatrixNewOutputLayerValve.CopyMatrix (m_mxOutThreshold + cMatrixOutputValveChange);
                
                        // ǰ�����
                        LMForwardCalculate (nInputLayerNumber,
                                                                nHideLayerNumber,
                                                                nOutputLayerNumber,
                                                                bSimulateDataFlag,
                                                                nComboFunc,
                                                                m_mxDemoDataInput,
                                                                m_mxInput,
                                                                cMatrixNewInputToHideWeight,
                                                                cMatrixNewHideLayerValve,
                                                                m_mxHide,
                                                                cMatrixNewHideToOutputWeight,
                                                                m_mxOut,
                                                                cMatrixNewOutputLayerValve
                                                                );

                                
                        cMatrixNewOutputLayerError = cMatrixDemoOutput - m_mxOut;
                        nSystemErrorNew =         cMatrixNewOutputLayerError.GetSystemError ();

                        if(nSystemErrorNew < nSystemErrorOld)
                        {
                                break;
                        }
                        else
                        {
                                nStep *= 10;
                        }
                                
                }// End for while loop

                if ( nStep > MAX_ADJUST_VALUE)
                {
                        nLoopTimes--;
                        return false;
                }

                nStep        *= 0.1;

                // ��ֵ
                m_mxIToHWeight = cMatrixNewInputToHideWeight;
                m_mxHideThreshold = cMatrixNewHideLayerValve;
                m_mxHToOWeight = cMatrixNewHideToOutputWeight;
                m_mxOutThreshold = cMatrixNewOutputLayerValve;
                cMatrixOutputLayerError = cMatrixNewOutputLayerError;
                nSystemErrorOld = nSystemErrorNew;

                if (m_bAbort)
                {
                        break;
                }
                // ��ʾ���ݺͳ�������״̬



                // ��ʾϵͳ���
                CTString        strSystemError;
                strSystemError.Format ("%.16lf", nSystemErrorOld);
                // ��ʾѵ������
                CTString strTrainTimes;
                strTrainTimes.Format ("%u", nLoopTimes + 1);
                CTString strProgressText =  "����:"+strTrainTimes + " ѵ������"+strSystemError;
//                 SendMessage( m_hWnd,WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
   
        }// End the "for" loop
        
        return true;
}

/////////////////////////////////////////////////////////////////////////////
//        Back propagation                ---->                ǰ�����(Only for Training)                   //
/////////////////////////////////////////////////////////////////////////////
void CBPArithmetic::BPForwardCalculate ( int nInputLayerNumber,
                                                                                                int nHideLayerNumber,
                                                                                                int nOutputLayerNumber,
                                                                                                bool bSimulateDataFlag,
                                                                                                int nComboFunc,
                                                                                                CBPMatrix &m_mxDemoDataInput,
                                                                                                CBPMatrix &m_mxInput,
                                                                                                CBPMatrix &m_mxIToHWeight,
                                                                                                CBPMatrix &m_mxHideThreshold,
                                                                                                CBPMatrix &m_mxHide,
                                                                                                CBPMatrix &m_mxHToOWeight,
                                                                                                CBPMatrix &m_mxOut,
                                                                                                CBPMatrix &m_mxOutThreshold,
                                                                                                CBPMatrix &cMatrixExHideLayerValveValue,
                                                                                                CBPMatrix &cMatrixExOutputLayerValveValue
                                                                                           )
{

        if(bSimulateDataFlag)
        {

                CBPMatrix cm_mxInput(m_mxDemoDataInput.GetMatrixRowNumber (), nInputLayerNumber);

                // �õ�����������ֵ
                m_mxDemoDataInput.CopySubMatrix (cm_mxInput, (unsigned int)0, (unsigned int)0);

                CBPMatrix cMatrixTInputLayerValue = cm_mxInput.Transpose ();

                m_mxInput.CopyMatrix (cMatrixTInputLayerValue);


        }

        /////////////////////////////////////////////////////////////////////////
        // �õ�����������������ľ�����
        // �������:
        //                1. ��������Ŀ��Ϊ��������;
        //                2. �������������������Ŀ��Ϊ���������;
        //                3. ����Ԫ���е�ֵ��Ϊ��Ӧ��������������ľ�����:
        //                   �� 
        //                                cm_mxInput * cm_mxIToHWeight 
        //                                + cm_mxHideThreshold
        //                        �õ�.
        //
                
        //CBPMatrix cMatrixExHideLayerValveValue;
        //cMatrixExHideLayerValveValue.nncpyi (m_mxHideThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());

        CBPMatrix cMatrixHideLayerPureInput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixHideLayerPureInput = m_mxIToHWeight * m_mxInput;

        cMatrixHideLayerPureInput += cMatrixExHideLayerValveValue;

        /////////////////////////////////////////////////////////////////////
        //        �����������������������
        //        �������:
        //                1. ����������y�������������x�Ĺ�ϵ���ú�����ʾ
        //                        y = f(x)
        //                2. �����ά����������ľ���������ά�����
        //

        CBPMatrix cMatrixHideLayerOutput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.Tansig();
                break;

        default:
                return;
        }
        
        m_mxHide.CopyMatrix(cMatrixHideLayerOutput);

        /////////////////////////////////////////////////////////////////////
        //        �õ��������������ľ�����
        //        �������;
        //                1. ��������Ŀ��Ϊ���������;
        //                2. ������������������Ŀ��Ϊ���������;
        //                3. ������Ԫ�ص�ֵ��Ϊ��Ӧ�����������ľ�����:
        //                        ��
        //                                cMatrixHideLayerOutput * cm_mxHToOWeight
        //                                + cMatrixOutputLayerValveValue
        //                        �õ�
        //

        //CBPMatrix cMatrixExOutputLayerValveValue;
        //cMatrixExOutputLayerValveValue.nncpyi (m_mxOutThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());

        CBPMatrix cMatrixOutputLayerPureInput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixOutputLayerPureInput = m_mxHToOWeight * cMatrixHideLayerOutput;
        cMatrixOutputLayerPureInput  += cMatrixExOutputLayerValveValue;


        /////////////////////////////////////////////////////////////////////
        //        ����������������������
        //        �������:
        //                1. �����ά����õ������������������ľ�������ɵľ���һ��;
        //                2. ���������y��������������ù�ϵʽ
        //                        y = f(x)
        //                ��ʾ
        //

        CBPMatrix cMatrixOutputLayerOutput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Tansig ();
                break;

        default:
                return ;
        }

        m_mxOut.CopyMatrix(cMatrixOutputLayerOutput);

}

/////////////////////////////////////////////////////////////////////////////
//        Back propagation                ---->                ǰ�����(Only for Simulating)           //
/////////////////////////////////////////////////////////////////////////////
void CBPArithmetic::BPForwardCalculate2( int nInputLayerNumber,
                                         int nHideLayerNumber,
                                         int nOutputLayerNumber,
                                         bool bSimulateDataFlag,
                                         int nComboFunc,
                                         CBPMatrix &m_mxDemoDataInput,
                                         CBPMatrix &m_mxInput,
                                         CBPMatrix &m_mxIToHWeight,
                                                                                                CBPMatrix &m_mxHideThreshold,
                                                                                                CBPMatrix &m_mxHide,
                                                                                                CBPMatrix &m_mxHToOWeight,
                                                                                                CBPMatrix &m_mxOut,
                                                                                                CBPMatrix &m_mxOutThreshold
                                                                                           )
{

        if(bSimulateDataFlag)
        {
                //5*150
                CBPMatrix cm_mxInput(m_mxDemoDataInput.GetMatrixRowNumber (), nInputLayerNumber);

                // �õ�����������ֵ
                m_mxDemoDataInput.CopySubMatrix (cm_mxInput, (unsigned int)0, (unsigned int)0);

                CBPMatrix cMatrixTInputLayerValue = cm_mxInput.Transpose ();

                m_mxInput.CopyMatrix (cMatrixTInputLayerValue);


        }

        /////////////////////////////////////////////////////////////////////////
        // �õ�����������������ľ�����
        // �������:
        //                1. ��������Ŀ��Ϊ��������;
        //                2. �������������������Ŀ��Ϊ���������;
        //                3. ����Ԫ���е�ֵ��Ϊ��Ӧ��������������ľ�����:
        //                   �� 
        //                                cm_mxInput * cm_mxIToHWeight 
        //                                + cm_mxHideThreshold
        //                        �õ�.
        //
                
        CBPMatrix cMatrixExHideLayerValveValue;//4*5
        cMatrixExHideLayerValveValue.nncpyi (m_mxHideThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
        //4*5
        CBPMatrix cMatrixHideLayerPureInput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixHideLayerPureInput = m_mxIToHWeight * m_mxInput;

        cMatrixHideLayerPureInput += cMatrixExHideLayerValveValue;


        /////////////////////////////////////////////////////////////////////
        //        �����������������������
        //        �������:
        //                1. ����������y�������������x�Ĺ�ϵ���ú�����ʾ
        //                        y = f(x)
        //                2. �����ά����������ľ���������ά�����
        //
//4*5
        CBPMatrix cMatrixHideLayerOutput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.Tansig();
                break;

        default:
                return;
        }
        //4*5
        m_mxHide.CopyMatrix(cMatrixHideLayerOutput);

        /////////////////////////////////////////////////////////////////////
        //        �õ��������������ľ�����
        //        �������;
        //                1. ��������Ŀ��Ϊ���������;
        //                2. ������������������Ŀ��Ϊ���������;
        //                3. ������Ԫ�ص�ֵ��Ϊ��Ӧ�����������ľ�����:
        //                        ��
        //                                cMatrixHideLayerOutput * cm_mxHToOWeight
        //                                + cMatrixOutputLayerValveValue
        //                        �õ�
        //
//7*5
        CBPMatrix cMatrixExOutputLayerValveValue;
        cMatrixExOutputLayerValveValue.nncpyi (m_mxOutThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
//7*5
        CBPMatrix cMatrixOutputLayerPureInput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixOutputLayerPureInput = m_mxHToOWeight * cMatrixHideLayerOutput;
        cMatrixOutputLayerPureInput  += cMatrixExOutputLayerValveValue;


        /////////////////////////////////////////////////////////////////////
        //        ����������������������
        //        �������:
        //                1. �����ά����õ������������������ľ�������ɵľ���һ��;
        //                2. ���������y��������������ù�ϵʽ
        //                        y = f(x)
        //                ��ʾ
        //
//7*5
        CBPMatrix cMatrixOutputLayerOutput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Tansig ();
                break;

        default:
                return ;
        }

        m_mxOut.CopyMatrix(cMatrixOutputLayerOutput);

}



/////////////////////////////////////////////////////////////////////////////
//        Back propagation                ---->                ��������                                                   //
/////////////////////////////////////////////////////////////////////////////
bool CBPArithmetic::BPDemoDataTrainRepeat (        int nInputLayerNumber,
                        int nHideLayerNumber,           int nOutputLayerNumber,
                        bool bSimulateDataFlag,         int nComboFunc,
                        double &nSystemErrorOld,                 double nSystemErrorNew,
                        double nSystemErrorLevel,                //double nSystemError,//���� ===temp
                        double nStep,                               unsigned int nMaxTrainTimes,
                        CBPMatrix &m_mxDemoDataInput,                CBPMatrix &m_mxInput,
                        CBPMatrix &m_mxIToHWeight,            CBPMatrix &m_mxHideThreshold,
                        CBPMatrix &m_mxHide,                                CBPMatrix &m_mxHToOWeight,
                        CBPMatrix &m_mxOut,                                CBPMatrix &m_mxOutThreshold )
{   
	// ����BP�㷨����nStep�ĳ�ʼֵ
	//nStep = 0.1;===temp
	
	const double Rate_inc=0.03;
	const double Study_inc=1.05;
	const double Study_dec=0.7;
	double inc;
	double alpha=0.5;
	double alphaold=alpha;
	
	
	// ǰ�����
	LMForwardCalculate (nInputLayerNumber,        nHideLayerNumber,nOutputLayerNumber,
		bSimulateDataFlag,  nComboFunc,             m_mxDemoDataInput,
		m_mxInput,          m_mxIToHWeight,  m_mxHideThreshold,
		m_mxHide,  m_mxHToOWeight,  m_mxOut,  m_mxOutThreshold        );
	
	/////////////////////////////////////////////////////////////////////
	//        �������������������delta����
	//        �������:
	//                1. ��������ĿΪ���������;
	//                2. ������������ĿΪ���������;
	//                3. �����е�Ԫ�ص�ֵyΪ:
	//                                y = (ǰ��������������ֵ - ������������ֵ) .* f'(net)
	//
	CBPMatrix cMatrixTDemoOutput(m_mxDemoDataInput.GetMatrixRowNumber (), nOutputLayerNumber);
	
	// �õ�����������������
	m_mxDemoDataInput.CopySubMatrix (cMatrixTDemoOutput, (unsigned int)nInputLayerNumber, (unsigned int)0);
	
	CBPMatrix cMatrixDemoOutput = cMatrixTDemoOutput.Transpose ();
	
	// �õ����������������
	CBPMatrix cMatrixOutputLayerError(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
	cMatrixOutputLayerError = cMatrixDemoOutput - m_mxOut;
	
	nSystemErrorOld = cMatrixOutputLayerError.GetSystemError ()/(2*m_mxDemoDataInput.GetMatrixRowNumber ());
	
	//1*321 
	CBPMatrix        cMatrixOutputLayerDelta (nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber());
	//4*1
	CBPMatrix cMatrixTHideToOutputWeightValue (m_mxHToOWeight.GetMatrixColNumber(), m_mxHToOWeight.GetMatrixRowNumber());
	// �����µ�����㵽�������Ȩֵ 4*8
	CBPMatrix cMatrixNewInputToHideWeight (m_mxIToHWeight.GetMatrixRowNumber (), m_mxIToHWeight.GetMatrixColNumber ());
	// ������µ�������ķ�ֵ 4*321
	CBPMatrix cMatrixNewHideLayerValve (nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
	// �����µ������㵽������Ȩֵ 1*4
	CBPMatrix cMatrixNewHideToOutputWeight (m_mxHToOWeight.GetMatrixRowNumber (), m_mxHToOWeight.GetMatrixColNumber ());
	// �����µ������ķ�ֵ 1*321
	CBPMatrix cMatrixNewOutputLayerValve (nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
	// �����µ������� 1*321
	CBPMatrix cMatrixNewOutputLayerError(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
	
	
	int nLoopTimes;
	for(nLoopTimes=1; nLoopTimes < nMaxTrainTimes; nLoopTimes++)        
	{
        
		if(nSystemErrorOld <1.01* nSystemErrorLevel)
		{
			nLoopTimes--;
			break;
		}
		
		// ��������deltaֵ
		// ע��: �˴�'/' �� '���'!!!
		cMatrixOutputLayerDelta = (m_mxOut - m_mxOut / m_mxOut) / cMatrixOutputLayerError;
        
		cMatrixTHideToOutputWeightValue = m_mxHToOWeight.Transpose();// Ȩ�ؾ���ֵ
		
		// ���������deltaֵ
		// ע��: �˴�'/' �� '���'!!!
		CBPMatrix cMatrixHideLayerDelta;
		cMatrixHideLayerDelta.CopyMatrix ( (m_mxHide - (m_mxHide / m_mxHide)) / ( cMatrixTHideToOutputWeightValue * cMatrixOutputLayerDelta) );

		// Ȩֵ�ͷ�ֵ�仯��
		cMatrixNewHideToOutputWeight =cMatrixNewHideToOutputWeight*(alpha)+cMatrixOutputLayerDelta * (m_mxHide.Transpose ()) * (nStep);
		cMatrixNewOutputLayerValve = cMatrixOutputLayerDelta;
		
		cMatrixNewInputToHideWeight =cMatrixNewInputToHideWeight*(alpha) +cMatrixHideLayerDelta * (m_mxInput.Transpose ()) * (nStep);
		cMatrixNewHideLayerValve = cMatrixHideLayerDelta;
		
		CBPMatrix m_mxIToHWeighttemp;
		m_mxIToHWeighttemp.CopyMatrix(m_mxIToHWeight);
		
		CBPMatrix m_mxHToOWeighttemp;
		m_mxHToOWeighttemp.CopyMatrix(m_mxHToOWeight);
		
		// ��ֵ
		
		m_mxIToHWeight +=cMatrixNewInputToHideWeight;
		CBPMatrix cMatrixExHideLayerValveValue;
		cMatrixExHideLayerValveValue.nncpyi (m_mxHideThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
		cMatrixExHideLayerValveValue += cMatrixNewHideLayerValve;//��������ֵ����ֵ
		
        m_mxHToOWeight += cMatrixNewHideToOutputWeight;
		CBPMatrix cMatrixExOutputLayerValveValue;
		cMatrixExOutputLayerValveValue.nncpyi (m_mxOutThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
		cMatrixExOutputLayerValveValue += cMatrixNewOutputLayerValve;//�������ֵ����ֵ
        
		// ǰ����㣨����µ�Ԥ�����ֵm_mxOut��
		
		BPForwardCalculate (nInputLayerNumber,nHideLayerNumber,nOutputLayerNumber,
			bSimulateDataFlag,nComboFunc,m_mxDemoDataInput,m_mxInput,
			m_mxIToHWeight,m_mxHideThreshold,m_mxHide,m_mxHToOWeight,
			m_mxOut,m_mxOutThreshold,cMatrixExHideLayerValveValue,
			cMatrixExOutputLayerValveValue );
		
		cMatrixNewOutputLayerError = cMatrixDemoOutput - m_mxOut;
		nSystemErrorNew =         cMatrixNewOutputLayerError.GetSystemError ()/(2* m_mxDemoDataInput.GetMatrixRowNumber ());
		
		
		
		inc=(nSystemErrorNew - nSystemErrorOld)/nSystemErrorOld;
		if(inc>Rate_inc) 
		{
			m_mxIToHWeight.CopyMatrix(m_mxIToHWeighttemp); 
			m_mxHToOWeight.CopyMatrix(m_mxHToOWeighttemp);
			alpha=0;
			nStep*=Study_dec;
		}
		else if ((inc>=0)&&(inc<=Rate_inc))
		{
			nStep*=Study_inc;
			if(alpha==0)
			{
				alpha=alphaold;
			}
			cMatrixOutputLayerError = cMatrixNewOutputLayerError;
			nSystemErrorOld = nSystemErrorNew;        
		}
		else
		{
			if(alpha==0)
			{
				alpha=alphaold;
			}
			if(fabs(inc)<0.0001)
			{
				nStep*=2*Study_inc;
			}
			else
			{
				nStep*=Study_inc;
			}
			
			cMatrixOutputLayerError = cMatrixNewOutputLayerError;
			nSystemErrorOld = nSystemErrorNew;
		}
		
		if (m_bAbort)
			break;
		//*
		// ��ʾϵͳ���
		CTString        strSystemError;
		strSystemError.Format ("%.10lf", nSystemErrorOld);
		// ��ʾѵ������
		CTString strTrainTimes;
		strTrainTimes.Format ("%u", nLoopTimes + 1);
		CTString strProgressText =  "����:"+strTrainTimes + "  ���:"+strSystemError;
// 		SendMessage( m_hWnd,WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
		//*/
	}// End the "for" loop
    if(nLoopTimes >= nMaxTrainTimes)
    {
//		CTString        strSystemError;
//		strSystemError.Format ("��ǰϵͳ����Ϊ%.10lf û���������趨ϵͳ����%.10lf", nSystemErrorOld,nSystemErrorLevel);
//		if (AfxMessageBox(strSystemError+"\r\n�Ƿ����Ŀǰ������������µ�������\r\nѡ��'��'Ϊ����,ѡ��'��'Ϊ������",MB_YESNO) !=IDYES)
//		{
			return true;//test  ?????
// 		}
    }
        
	//***************************************************************************
	//***************************************************************************
	//***************************************************************************
	
     return true;
}

void CBPArithmetic::Abort()
{
        m_bAbort = true;
}

void CBPArithmetic::Reset()
{
        m_bAbort = false;
}