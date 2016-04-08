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
	m_strCFontName = "宋体";
	m_strRFontName = "宋体";
}


/************************************************************************************
函数名称：
         CShowCharacteristic::loadShowCharacter
功能说明：
        从缓存区装入显示特性
详细解释：

传入参数：
        LPBYTE lpData, int count
返回类型：
        void
返回数据：

制作：
     LXD_2002_06_26
修改: 
************************************************************************************/

void CShowCharacteristic::loadShowCharacter(LPBYTE lpData, int count)
{
	

}



/************************************************************************************
函数名称：
         CShowCharacteristic::saveShowCharacter
功能说明：
        保存显示特性
详细解释：

传入参数：
        LPBYTE: lpData
返回类型：
        int
返回数据：
        CTString str.GetLength()
制作：
     LXD_2002_06_26
修改: 
************************************************************************************/

int CShowCharacteristic::saveShowCharacter(LPBYTE lpData)
{
	return 0;
	FormatShow();
	memcpy(lpData, m_strDiscribe, m_strDiscribe.GetLength());
	return m_strDiscribe.GetLength();
}


/************************************************************************************
函数名称：
         CShowCharacteristic::judgeStrType
功能说明：
         判断显示特性的类型
详细解释：

传入参数：
         CTString& string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::getRWidth
功能说明：
         获取行的宽度
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     Eric0830
修改: 
************************************************************************************/
	
void CShowCharacteristic::getRWidth(CTString string)
{
	string.TrimLeft();
	string.TrimRight();
	
	int temp= 0;
	for(int m=0; m<string.GetLength(); m++)   //转换为十进制数据
	{
		temp = temp*10 + (string[m] - 0x30);
	}
    m_nRWidth = temp;
}

/************************************************************************************
函数名称：
         CShowCharacteristic::getCWidth
功能说明：
         获取列的宽度
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
************************************************************************************/

void CShowCharacteristic::getCWidth(CTString string)
{	
	string.TrimLeft();
	string.TrimRight();
	
	int temp= 0;
	for(int m=0; m<string.GetLength(); m++)   //转换为十进制数据
	{
		temp = temp*10 + (string[m] - 0x30);
	}
    m_nCWidth = temp;
	
}

/************************************************************************************
函数名称：
         CShowCharacteristic::getCColor
功能说明：
         获取列的显示颜色
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::getRColor
功能说明：
         获取行的显示颜色
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::getCAlign
功能说明：
         获取列对齐方式
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::getRAlign
功能说明：
         获取行对齐方式
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::getCFontName
功能说明：
         获取列字体名称
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
************************************************************************************/

void CShowCharacteristic::getCFontName(CTString string)
{
	string.TrimLeft();
	string.TrimRight();

	m_strCFontName = string;


}


/************************************************************************************
函数名称：
         CShowCharacteristic::getRFontName
功能说明：
         获取行字体名称
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
************************************************************************************/

void CShowCharacteristic::getRFontName(CTString string)
{
	string.TrimLeft();
	string.TrimRight();

	m_strRFontName = string;

}


/************************************************************************************
函数名称：
         CShowCharacteristic::getCFontStyle
功能说明：
         获取列字形
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::getRFontStyle
功能说明：
         获取行字形
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::getCFontSize
功能说明：
         获取列字体大小
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::getRFontSize
功能说明：
         获取行字体大小
详细解释：

传入参数：
         CTString string
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::Clone
功能说明：
         克隆显示特性数据
详细解释：

传入参数：
         CShowCharacteristic *pSC
返回类型：
         void
返回数据：
         无
制作：
     LXD_2002_06_26
修改: 
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
函数名称：
         CShowCharacteristic::QueryLength
功能说明：
         获取显示特性按字符存放的长度
详细解释：
        
传入参数：
         无
返回类型：
         void
返回数据：
         CTString str.GetLength()
制作：
     LXD_2002_06_26
修改: 
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
