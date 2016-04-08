// Timer.h: interface for the CTimer class.
//						��ʱ��
//			���ܣ� ����������������ĵ�ʱ�䳤�ȣ���ȷ��0.001��
//                 ������debug��release ״̬�¶��ܹ�����
//          ���ߣ� ������   ���ɹ�˾ͳ���㷨С��
//			ʱ�䣺 2005��11��2��

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_)
#define AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#ifdef _UNIX
	#define _timeb	timeb
	#define _ftime	ftime
#else
#include <process.h>
#endif 

class CTimer  
{
public:
	CTimer();
	virtual ~CTimer();
	//��ʼ��ʱ��strTitleΪ����ı�ǡ�
	void Tic(CTString strTitle = "");
	void Toc(CTString strTitle = "");//����ʱ�䣬Ϊ��ʱ�ļ�ʱ�� ��λΪ��
	void TotalTime();//����ۼ���ʱ��
    
private:
   bool     m_bFirst;      //�Ƿ��һ��
   double   m_dBeginTime;  //��ʼʱ��
   CTString  m_strTitle;      //������� 

   struct _timeb timebuffer_begin;//��ʼʱ��
   struct _timeb timebuffer_end;  //��ֹʱ��
   
private:
   double GetSeconds(struct _timeb& str_t);
   double GetElapsedTime();
};

#endif // !defined(AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_)
