# Microsoft Developer Studio Project File - Name="CResult" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CResult - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CResult.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CResult.mak" CFG="CResult - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CResult - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CResult - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ÍøÂç°æ2/C/Result", FIWDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CResult - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CRESULT_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CRESULT_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ..\cpknl\release\cpknl.lib /nologo /dll /machine:I386 /out:"E:\NetTest\CppRelease\WebRoot\WEB-INF\lib\CResult.dll"

!ELSEIF  "$(CFG)" == "CResult - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CRESULT_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CRESULT_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\cpknl\debug\cpknl.lib /nologo /dll /debug /machine:I386 /out:"E:\NetTest\CppDebug\WebRoot\WEB-INF\lib\CResult.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "CResult - Win32 Release"
# Name "CResult - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CResult.cpp
# End Source File
# Begin Source File

SOURCE=.\DecTreeWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Result.cpp
# End Source File
# Begin Source File

SOURCE=..\Public\System.cpp
# End Source File
# Begin Source File

SOURCE=.\TChart1.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartArea.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartAutocorrelations.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartBar.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartBlurry.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartBoxplot.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartClustering.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartControl.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartCrossCorrelations.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartDecTree.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartErrBar.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartHighLow.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartHistogram.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartLine.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartPareto.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartPedigree.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartPie.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartPP.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartROC.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartScatterPlot.cpp
# End Source File
# Begin Source File

SOURCE=.\TChartSequence.cpp
# End Source File
# Begin Source File

SOURCE=..\public\TCLFile.cpp
# End Source File
# Begin Source File

SOURCE=.\TGraphData.cpp
# End Source File
# Begin Source File

SOURCE=..\public\TString.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DecTreeWnd.h
# End Source File
# Begin Source File

SOURCE=.\Define.h
# End Source File
# Begin Source File

SOURCE=.\HTMLHELP.H
# End Source File
# Begin Source File

SOURCE=.\Result.h
# End Source File
# Begin Source File

SOURCE=.\TChart1.h
# End Source File
# Begin Source File

SOURCE=.\TChartArea.h
# End Source File
# Begin Source File

SOURCE=.\TChartAutocorrelations.h
# End Source File
# Begin Source File

SOURCE=.\TChartBar.h
# End Source File
# Begin Source File

SOURCE=.\TChartBlurry.h
# End Source File
# Begin Source File

SOURCE=.\TChartBoxplot.h
# End Source File
# Begin Source File

SOURCE=.\TChartClustering.h
# End Source File
# Begin Source File

SOURCE=.\TChartControl.h
# End Source File
# Begin Source File

SOURCE=.\TChartCrossCorrelations.h
# End Source File
# Begin Source File

SOURCE=.\TChartCurve.h
# End Source File
# Begin Source File

SOURCE=.\TChartDecTree.h
# End Source File
# Begin Source File

SOURCE=.\TChartErrBar.h
# End Source File
# Begin Source File

SOURCE=.\TChartHighLow.h
# End Source File
# Begin Source File

SOURCE=.\TChartHistogram.h
# End Source File
# Begin Source File

SOURCE=.\TChartLine.h
# End Source File
# Begin Source File

SOURCE=.\TChartPareto.h
# End Source File
# Begin Source File

SOURCE=.\TChartPedigree.h
# End Source File
# Begin Source File

SOURCE=.\TChartPie.h
# End Source File
# Begin Source File

SOURCE=.\TChartPP.h
# End Source File
# Begin Source File

SOURCE=.\TChartROC.h
# End Source File
# Begin Source File

SOURCE=.\TChartScatterPlot.h
# End Source File
# Begin Source File

SOURCE=.\TChartSequence.h
# End Source File
# Begin Source File

SOURCE=.\TGraphData.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
