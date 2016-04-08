// KMeans_Algorithm.h: interface for the CKMeans_Algorithm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KMEANS_H__A2D787BB_9521_4F24_8E32_4C6DD76BFF1A__INCLUDED_)
#define AFX_KMEANS_H__A2D787BB_9521_4F24_8E32_4C6DD76BFF1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
//���پ����㷨��
//�Ӹ߼�ͳ�ƵĿ��پ����������ȡ���������Ľ������ٶȱȸ߼�ͳ�ƵĿ��پ���Ҫ��
//�������������ݴ洢��ʽ������ǰ��ʹ��CDoubleMatrix������ʹ��TCDataMatrix
//�ο����پ����ĵ��ʹ���
//����
//2005-11
//////////////////////////////////////////////////////////////////////////
#include <sstream>
#include <sys/timeb.h>

#include "../cpknl/ErrorReporter.h"
#include "../cpknl/Vector.h"
#include "../cpknl/cpknl.h"
#include "../cpknl/Matrix.h"


using namespace std;

class CKMeans_Algorithm  
{
private:
	int m_nEndCount; //�㷨��������

public:
	CKMeans_Algorithm();
	virtual ~CKMeans_Algorithm();

public:	
	//KMeans_Algorithm�ⲿ���ýӿڣ��Ժ�ɼ��������Ϣ�������������ĸ���������
	bool KMeans(CDoubleMatrix& mx,int nCenterNum,bool bIsStd,double dPrecision,
		int nMaxRepeatTimes,CDoubleMatrix& mxCenter);
	//int GetEndCount();

private:
	bool UpdateClusterCenter(CDoubleMatrix& mx,CDoubleMatrix& mxOldCenter,
		CDoubleMatrix& mxCenter,CIntMatrix& mxSampleBelongCenterIndex);

//////////////////////////////////////////////////////////////////////////
//��ֹ�㷨���
private:
	bool m_bAbort;				//�㷨��ֹ��־
	string m_strAbortInfo;		//��¼�㷨��ֹ��Ϣ
public:
	//��ֹKMeans�������㡣���ڵ����㷨��Ϊ�������һ���̵߳���ʱ����������Ե��ô˺�������ֹKMeans
	void Abort();	
	string GetAbortInfo();		//�����㷨��ֹʱ������״̬��Ϣ
	bool IsAbort();				//����KMeans���ô˷����ж��㷨�Ƿ�����ֹ�˳�
//////////////////////////////////////////////////////////////////////////
//���������////////////////////////////////////////////////////////////
private:
	string m_strErrorInfo;		//��¼�㷨������Ϣ
	bool m_bError;				//����㷨�Ƿ����
public:
	bool IsError();				//����㷨�Ƿ����
	string GetErrorInfo();		//����㷨������Ϣ	
//////////////////////////////////////////////////////////////////////////
//��¼������Ϣ���
private:
	string m_strRunInfo;		//��¼������Ϣ
	string FormatTimeSpan(_timeb timeBegin,_timeb timeEnd);		//���㿪ʼʱ�̵�����ʱ��֮��ĺ�ʱ
public:
	string GetRunInfo();		//��õ���KMeans֮���������Ϣ����������ʱ���
//////////////////////////////////////////////////////////////////////////
};

#endif // !defined(AFX_KMEANS_H__A2D787BB_9521_4F24_8E32_4C6DD76BFF1A__INCLUDED_)
