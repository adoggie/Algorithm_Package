// Timer.cpp: implementation of the CTimer class.
//						��ʱ��
//			���ܣ� ����������������ĵ�ʱ�䳤�ȣ���ȷ��0.001��
//                 ������debug��release ״̬�¶��ܹ�����
//          ���ߣ� ������   ���ɹ�˾ͳ���㷨С��
//			ʱ�䣺 2005��11��2��
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Timer.h"
#include <time.h>

FILE* stream  = NULL;
CTimer::CTimer()
{
  m_dBeginTime = 0.0;
  m_strTitle =_T("");
  m_bFirst = true;
}

CTimer::~CTimer()
{
	char* timeline = ctime( & ( timebuffer_end.time ) );
	if(stream)
	{
		fprintf( stream," \n%20s \n",timeline );
		fprintf( stream," *******************���Խ���******************\n\n",timeline );
		fclose(stream);
	}
}
//��ʼ��ʱ��strTitleΪ����ı�ǡ�
void CTimer::Tic(CTString strTitle)
{
    _ftime( &timebuffer_begin);//��ȡ��ʼʱ��
	if(m_bFirst)
	{
		m_dBeginTime = GetSeconds(timebuffer_begin);
		stream = fopen( m_strPath.GetData(), "w" );
		if(stream == NULL)
			return;
		char* timeline = ctime( & ( timebuffer_begin.time ) );
		
		fprintf( stream,strTitle + " \nʱ����� :%20s \n",timeline );
		fprintf( stream," \n*******************���Կ�ʼ******************\n\n",timeline );
		
		m_bFirst =false;
	}

}
//��ʱ����

void CTimer::Toc(CTString strTitle)
{
    _ftime( &timebuffer_end);//��ȡ����ʱ��
	double dElapsed_time = GetElapsedTime();//��ȡ����ʱ�䣻
  	if(stream == NULL)
		return;
	
	fprintf( stream, strTitle + " \nElapsed time :%.4f seconds\n\n",dElapsed_time );
	_ftime( &timebuffer_begin);//chon��ȡ����ʱ��
	   
}
void CTimer::TotalTime()//����ۼ���ʱ��
{
    _ftime( &timebuffer_end);//��ȡ����ʱ��

	double dEndTime = GetSeconds(timebuffer_end);
	double dElapsed_time = dEndTime - m_dBeginTime;
	if(stream == NULL)
		return;
	
	fprintf( stream, m_strTitle + " \nElapsed time :%.4f seconds\n\n",dElapsed_time );
	
}
void CTimer::SetPath(CTString strPathName)//���ô洢·��,���Ұ����ļ�����
{
	m_strPath = strPathName;
}

inline double CTimer::GetSeconds(struct _timeb& struct_t)
{
	double dbSeconds = struct_t.time + struct_t.millitm/1000.0;
	return dbSeconds;
}
inline  double CTimer::GetElapsedTime()
{
   double dTime1 = GetSeconds(timebuffer_begin);
   double dTime2 = GetSeconds(timebuffer_end);
   return dTime2 - dTime1;
}

