// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "showcharacteristic.h"
#include <string.h>

CShowCharacteristic::CShowCharacteristic()
{
	m_byteCAlign = 0;
	m_byteCFontStyle = 0;
	m_byteRAlign = 0;
	m_byteRFontStyle = 0;
	m_cCColor = 0;
	m_cRColor = 0;
	m_nCWidth = 100;
	m_nRWidth = 50;
	m_strCFontName = "����";
	m_strRFontName = "����";
}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::loadShowCharacter
����˵����
        �ӻ�����װ����ʾ����
��ϸ���ͣ�

���������
        LPBYTE lpData, int count
�������ͣ�
        void
�������ݣ�

������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::loadShowCharacter(LPBYTE lpData, int count)
{
	

}



/************************************************************************************
�������ƣ�
         CShowCharacteristic::saveShowCharacter
����˵����
        ������ʾ����
��ϸ���ͣ�

���������
        LPBYTE: lpData
�������ͣ�
        int
�������ݣ�
        CTString str.GetLength()
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

int CShowCharacteristic::saveShowCharacter(LPBYTE lpData)
{
	return 0;
	FormatShow();
	memcpy(lpData, m_strDiscribe, m_strDiscribe.GetLength());
	return m_strDiscribe.GetLength();
}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::judgeStrType
����˵����
         �ж���ʾ���Ե�����
��ϸ���ͣ�

���������
         CTString& string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::judgeStrType(CTString& string)
{
	CTString str1("CWidth");
	CTString str2("CColor");
	CTString str3("RColor");
	CTString str4("CAlign");
	CTString str5("RAlign");
	CTString str6("CFontName");
	CTString str7("RFontName");
	CTString str8("CFontStyle");
	CTString str9("RFontStyle");
	CTString str10("CFontSize");
	CTString str11("RFontSize");	
	CTString str12("RWidth");

	if(!(str12.Compare(string.Left(str12.GetLength()))))
	{
		m_nType = RWidth;
        int m = string.Delete(0, str12.GetLength());
	}

	if(!(str1.Compare(string.Left(str1.GetLength()))))
	{
		m_nType = CWidth;
        int m = string.Delete(0, str1.GetLength());
	}
	
	if(!(str2.Compare(string.Left(str2.GetLength()))))
	{
		m_nType = CColor;
        int m = string.Delete(0, str2.GetLength());
	}
	
	if(!(str3.Compare(string.Left(str3.GetLength()))))
	{
		m_nType = RColor;
        int m = string.Delete(0, str3.GetLength());
	}
	
	if(!(str4.Compare(string.Left(str4.GetLength()))))
	{
		m_nType = CAlign;
        int m = string.Delete(0, str4.GetLength());
	}
	
	if(!(str5.Compare(string.Left(str5.GetLength()))))
	{
		m_nType = RAlign;
		int m = string.Delete(0, str5.GetLength());
	}
	
	if(!(str6.Compare(string.Left(str6.GetLength()))))
	{
		m_nType = CFontName;
        int m = string.Delete(0, str6.GetLength());
	}
    if(!(str7.Compare(string.Left(str7.GetLength()))))
	{
		m_nType = RFontName;
        int m = string.Delete(0, str7.GetLength());
	}

    if(!(str8.Compare(string.Left(str8.GetLength()))))
	{
		m_nType = CFontStyle;
		int m = string.Delete(0, str8.GetLength());
	}
	
	if(!(str9.Compare(string.Left(str9.GetLength()))))
	{
		m_nType = RFontStyle;
		int m = string.Delete(0, str9.GetLength());
	}
	
	if(!(str10.Compare(string.Left(str10.GetLength()))))
	{
		m_nType = CFontSize;
		int m = string.Delete(0, str10.GetLength());
	}
	
	if(!(str11.Compare(string.Left(str11.GetLength()))))
	{
		m_nType = RFontSize;
		int m = string.Delete(0, str11.GetLength());
	}  

	string.TrimLeft();
	string.TrimRight();
	int n = string.GetLength();
	string = string.Right(n-1);    
	string.TrimLeft();
}

/************************************************************************************
�������ƣ�
         CShowCharacteristic::getRWidth
����˵����
         ��ȡ�еĿ��
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     Eric0830
�޸�: 
************************************************************************************/
	
void CShowCharacteristic::getRWidth(CTString string)
{
	string.TrimLeft();
	string.TrimRight();
	
	int temp= 0;
	for(int m=0; m<string.GetLength(); m++)   //ת��Ϊʮ��������
	{
		temp = temp*10 + (string[m] - 0x30);
	}
    m_nRWidth = temp;
}

/************************************************************************************
�������ƣ�
         CShowCharacteristic::getCWidth
����˵����
         ��ȡ�еĿ��
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getCWidth(CTString string)
{	
	string.TrimLeft();
	string.TrimRight();
	
	int temp= 0;
	for(int m=0; m<string.GetLength(); m++)   //ת��Ϊʮ��������
	{
		temp = temp*10 + (string[m] - 0x30);
	}
    m_nCWidth = temp;
	
}

/************************************************************************************
�������ƣ�
         CShowCharacteristic::getCColor
����˵����
         ��ȡ�е���ʾ��ɫ
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getCColor(CTString string)
{
	string.TrimLeft();
	string.TrimRight();

 	if(string.GetLength() > 6 || string.GetLength() < 1)
		//===temp AfxMessageBox("There is a incorrect color value.");

	string.MakeUpper();
	
//	if (string.GetLength()!=6)
//		//===temp AfxMessageBox("This is a invalid value.");
	
	m_cCColor = (COLORREF)atoi(string);

	/*
	if(string.GetLength() > 8)
		//===temp AfxMessageBox("There is a incorrect color value.");
	
	string.TrimLeft();
	string.TrimRight();
	string.MakeUpper();
		
	byte temp;
	m_cCColor = 0;
	byte *p =(byte *) &m_cCColor;
	for(int i=0; i<string.GetLength(); i++)
	{
		if ((string[i] >= 0x41) && (string[i] <= 0x46))
		{
			temp = string[i]-'A'+10;
		}
		else
		{
			if((string[i] >= 0x30) && (string[i] <= 0x39))
			{
    			temp = string[i]-'0';
			}
			else
			{
				//===temp AfxMessageBox("There is incorrect RGB.");
			}
		}

		if (!(i%2))
			temp=temp*16;
		*(p+i/2) = *(p+i/2)+temp;
	}*/
}



/************************************************************************************
�������ƣ�
         CShowCharacteristic::getRColor
����˵����
         ��ȡ�е���ʾ��ɫ
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getRColor(CTString string)
{
//  	if(string.GetLength() > 8)
// 	if(string.GetLength() > 6 || string.GetLength() < 1)
//		//===temp AfxMessageBox("There is a incorrect color value.");
	
	string.TrimLeft();
	string.TrimRight();

 	if(string.GetLength() > 6 || string.GetLength() < 1)
		//===temp AfxMessageBox("There is a incorrect color value.");

	string.MakeUpper();
	
//	if (string.GetLength()!=6)
//		//===temp AfxMessageBox("This is a invalid value.");
	
	m_cRColor = (COLORREF)atoi(string);

	/*
	byte temp;
	byte *p =(byte *) &m_cRColor;
	for(int i=0; i<string.GetLength(); i++)
	{
		if ((string[i] >= 0x41) && (string[i] <= 0x46))
		{
			temp = string[i]-'A'+10;
		}
		else
		{
			if((string[i] >= 0x30) && (string[i] <= 0x39))
			{
    			temp = string[i]-'0';
			}
			else
			{
				//===temp AfxMessageBox("This RGB is incorrect.");
			}
		}

		if (!(i%2))
			temp=temp*16;
		*(p+i/2) = *(p+i/2)+temp;
	}
*/
}



/************************************************************************************
�������ƣ�
         CShowCharacteristic::getCAlign
����˵����
         ��ȡ�ж��뷽ʽ
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getCAlign(CTString string)
{
	if(string.GetLength() > 2)
		//===temp AfxMessageBox("There is a incorrect color value.");
	
	string.TrimLeft();
	string.TrimRight();

	m_byteCAlign =0;
	byte temp;
	byte *p =(byte *) &m_byteCAlign;
	for(int i=0; i<string.GetLength(); i++)
	{
		if ((string[i] >= 0x30) && (string[i] <= 0x32))
		{
			temp = string[i]-'0';
		}
	

		if (!(i%2))
			temp=temp*16;
		*(p+i/2) = *(p+i/2) + temp;
	}
}



/************************************************************************************
�������ƣ�
         CShowCharacteristic::getRAlign
����˵����
         ��ȡ�ж��뷽ʽ
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getRAlign(CTString string)
{
	if(string.GetLength() > 2)
		//===temp AfxMessageBox("There is a invalid value  1.");
	
	string.TrimLeft();
	string.TrimRight();

	m_byteRAlign = 0;
	byte temp;
	byte *p =(byte *) &m_byteRAlign;
	for(int i=0; i<string.GetLength(); i++)
	{
		if ((string[i] >= 0x30) && (string[i] <= 0x32))
		{
			temp = string[i]-'0';
		}
		else
		{
			//===temp AfxMessageBox("There is a invalid value  2.");
		}
	

		if (!(i%2))
			temp=temp*16;
		*(p+i/2) = *(p+i/2) + temp;
	}
}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::getCFontName
����˵����
         ��ȡ����������
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getCFontName(CTString string)
{
	string.TrimLeft();
	string.TrimRight();

	m_strCFontName = string;


}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::getRFontName
����˵����
         ��ȡ����������
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getRFontName(CTString string)
{
	string.TrimLeft();
	string.TrimRight();

	m_strRFontName = string;

}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::getCFontStyle
����˵����
         ��ȡ������
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getCFontStyle(CTString string)
{
	string.TrimLeft();
	string.TrimRight();
    string.MakeUpper();
	
	m_byteCFontStyle = 0;
	byte temp;
	byte *p =(byte *) &m_byteCFontStyle;
	for(int i=0; i<string.GetLength(); i++)
	{
		if ((string[i] >= 0x41) && (string[i] <= 0x46))
		{
			temp = string[i]-'A'+10;
		}
		else
		{
			if((string[i] >= 0x30) && (string[i] <= 0x39))
			{
    			temp = string[i]-'0';
			}
			else
			{
				//===temp AfxMessageBox("There is a invalid value 3.");
			}
		}

		if (!(i%2))
			temp=temp*16;
		*(p+i/2) = *(p+i/2)+temp;
	}
}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::getRFontStyle
����˵����
         ��ȡ������
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getRFontStyle(CTString string)
{
    string.TrimLeft();
	string.TrimRight();
    string.MakeUpper();
	
	m_byteRFontStyle =0;
	byte temp;
	byte *p =(byte *) &m_byteRFontStyle;
	for(int i=0; i<string.GetLength(); i++)
	{
		if ((string[i] >= 0x41) && (string[i] <= 0x46))
		{
			temp = string[i]-'A'+10;
		}
		else
		{
			if((string[i] >= 0x30) && (string[i] <= 0x39))
			{
    			temp = string[i]-'0';
			}
			else
			{
				//===temp AfxMessageBox("There is a invalid value  4.");
			}
		}

		if (!(i%2))
			temp=temp*16;
		*(p+i/2) = *(p+i/2)+temp;
	}

}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::getCFontSize
����˵����
         ��ȡ�������С
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getCFontSize(CTString string)
{
    string.TrimLeft();
	string.TrimRight();
    string.MakeUpper();
	
	m_byteCFontSize = 0;
	byte temp;
	byte *p =(byte *) &m_byteCFontSize;
	for(int i=0; i<string.GetLength(); i++)
	{
		if ((string[i] >= 0x41) && (string[i] <= 0x46))
		{
			temp = string[i]-'A'+10;
		}
		else
		{
			if((string[i] >= 0x30) && (string[i] <= 0x39))
			{
    			temp = string[i]-'0';
			}
			else
			{
				//===temp AfxMessageBox("There is a invalid value  5.");
			}
		}

		if (!(i%2))
			temp=temp*16;
		*(p+i/2) = *(p+i/2)+temp;
	}
}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::getRFontSize
����˵����
         ��ȡ�������С
��ϸ���ͣ�

���������
         CTString string
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::getRFontSize(CTString string)
{
	string.TrimLeft();
	string.TrimRight();
    string.MakeUpper();
	
	m_byteRFontSize = (BYTE)atoi(string);
/*	byte temp;
	byte *p =(byte *) &m_byteRFontSize;
	for(int i=0; i<string.GetLength(); i++)
	{
		if ((string[i] >= 0x41) && (string[i] <= 0x46))
		{
			temp = string[i]-'A'+10;
		}
		else
		{
			if((string[i] >= 0x30) && (string[i] <= 0x39))
			{
    			temp = string[i]-'0';
			}
			else
			{
				//===temp AfxMessageBox("There is a invalid value  6.");
			}
		}

		if (!(i%2))
			temp=temp*16;
		*(p+i/2) = *(p+i/2)+temp;
	}*/
}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::Clone
����˵����
         ��¡��ʾ��������
��ϸ���ͣ�

���������
         CShowCharacteristic *pSC
�������ͣ�
         void
�������ݣ�
         ��
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

void CShowCharacteristic::Clone(CShowCharacteristic *pSC)
{
	m_nCWidth = pSC->m_nCWidth;
	m_nRWidth = pSC->m_nRWidth;
	m_cCColor = pSC->m_cCColor;
	m_cRColor = pSC->m_cRColor;
	m_byteCAlign = pSC->m_byteCAlign;
	m_byteRAlign = pSC->m_byteRAlign;
	m_strCFontName = pSC->m_strCFontName;
	m_strRFontName = pSC->m_strRFontName;
	m_byteCFontStyle = pSC->m_byteCFontStyle;
	m_byteRFontStyle = pSC->m_byteRFontStyle;
	m_byteCFontSize = pSC->m_byteCFontSize;
	m_byteRFontSize = pSC->m_byteRFontSize;

}


/************************************************************************************
�������ƣ�
         CShowCharacteristic::QueryLength
����˵����
         ��ȡ��ʾ���԰��ַ���ŵĳ���
��ϸ���ͣ�
        
���������
         ��
�������ͣ�
         void
�������ݣ�
         CTString str.GetLength()
������
     LXD_2002_06_26
�޸�: 
************************************************************************************/

int CShowCharacteristic::QueryLength()
{
	return 0;
	FormatShow();
	return m_strDiscribe.GetLength();
}



void CShowCharacteristic::FormatShow()
{
	m_strDiscribe .Format( "CWidth = %d \r\n"
		        "RWidth = %d \r\n"
				"CColor = %d \r\n"
				"RColor = %d \r\n"
				"CAlign = %x \r\n"
				"RAlign = %x \r\n"
				"CFontName = %s \r\n"
				"RFontName = %s \r\n"
				"CFontStyle = %x \r\n"
				"RFontStyle = %x \r\n"
				"CFontSize = %x \r\n"
				"RFontSize = %x \r\n",
				 m_nCWidth,        m_nRWidth,
				 m_cCColor,        m_cRColor,
				 m_byteCAlign,     m_byteRAlign,
				 m_strCFontName,   m_strRFontName,
				 m_byteCFontStyle, m_byteRFontStyle,
				 m_byteCFontSize,  m_byteRFontSize);
}
