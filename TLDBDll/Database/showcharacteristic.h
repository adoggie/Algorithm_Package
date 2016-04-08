// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CShowCharacteristic_3CF57C730165_INCLUDED
#define _INC_CShowCharacteristic_3CF57C730165_INCLUDED

//##ModelId=3CF57C730165

#define CWidth 1
#define CColor 2
#define RColor 3
#define CAlign 4
#define RAlign 5
#define CFontName 6
#define RFontName 7
#define CFontStyle 8
#define RFontStyle 9
#define CFontSize 10
#define RFontSize 11
#define RWidth   12


// 字体，目前支持：宋体、楷体、黑体、system

// FONT STYLE 目前支持：常规   斜体  粗体
#define GENERAL_FONTSTYLE 0x01
#define ITALIC_FONTSTYLE 0x02
#define OVERSTRIKING_FONTSTYLE 0x03

// FONT SIZE 目前支持 8，9，10，11，12，14，16
#define EIGHT_FONTSIZE    0x08
#define NINE_FONTSIZE     0x09
#define TEN_FONTSIZE      0x0A
#define ELEVEN_FONTSIZE   0x0B
#define TWELVE_FONTSIZE   0x0C
#define FOURTEEN_FONTSIZE 0x0E
#define SIXTEEN_FONTSIZE  0x10


class AFX_EXT_CLASS CShowCharacteristic 
{
public:
	CShowCharacteristic();
	CTString m_strClone;
	void Clone(CShowCharacteristic* pSC);
	int QueryLength();
	
	void getRFontSize(CTString str);
	void getCFontSize(CTString str);
	void getRFontStyle(CTString str);
	void getCFontStyle(CTString str);
	void getRFontName(CTString str);
	void getCFontName(CTString str);
	void getRAlign(CTString str);
	void getCAlign(CTString str);
	void getRColor(CTString str);
	void getCColor(CTString str);
	void getRWidth(CTString str);
	void getCWidth(CTString str);
	int m_nType;

	void judgeStrType(CTString& str);
	int saveShowCharacter(LPBYTE lpData);
	void loadShowCharacter(LPBYTE lpData, int count);
	//##ModelId=3CF57CB00126
	int m_nCWidth;

	//##ModelId=3CF57CC1001C
	COLORREF m_cCColor;

	//##ModelId=3CF57CF002EB
	byte m_byteCAlign;

	//##ModelId=3CF57D080390
	CTString m_strCFontName;

	//##ModelId=3CF5C44500E0
	byte m_byteCFontStyle;

	//##ModelId=3CF5F03102F1
	int m_nRWidth;

	//##ModelId=3CF5F05502A3
	COLORREF m_cRColor;

	//##ModelId=3CF5F06B020E
	byte m_byteRAlign;

	//##ModelId=3CF5F07B03D4
	CTString m_strRFontName;

	//##ModelId=3CF5F08801DE
	byte m_byteRFontStyle;

	byte m_byteRFontSize;

    byte m_byteCFontSize;

private:
	void FormatShow();
	CTString m_strDiscribe;
};

#endif /* _INC_CShowCharacteristic_3CF57C730165_INCLUDED */

