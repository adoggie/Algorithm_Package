# Microsoft Developer Studio Project File - Name="TLADVN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TLADVN - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tlAdvn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tlAdvn.mak" CFG="TLADVN - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TLADVN - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TLADVN - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Õ¯¬Á∞Ê2/C/TLADVN", BRWDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TLADVN - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TLADVN_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TLADVN_EXPORTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ..\Cpknl\Release\Cpknl.lib ..\DataInterface\Release\DataInterface.lib ..\TLDBDLL\Release\TLDBDll.lib ..\Result\Release\CResult.lib /nologo /dll /machine:I386 /out:"E:\NetTest\CppRelease\WebRoot\WEB-INF\lib\TLADVN.dll"

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TLADVN_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TLADVN_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\Cpknl\Debug\Cpknl.lib ..\DataInterface\Debug\DataInterface.lib ..\TLDBDLL\Debug\TLDBDll.lib ..\Result\Debug\CResult.lib /nologo /dll /debug /machine:I386 /out:"E:\NetTest\CppDebug\WebRoot\WEB-INF\lib\TLADVN.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TLADVN - Win32 Release"
# Name "TLADVN - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AlgorithmBase.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Public\AnaSyntax.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Public\AnaWord.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ARIMA.CPP
# End Source File
# Begin Source File

SOURCE=.\Classify.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CointegrationTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Covariance.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CurveReg.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Discriminant.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\arithInterface\EnvParam.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ExponSmoothing.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Factor.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Public\FileReadWrite.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MarkovChain.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiAnova.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PCR.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickCluster.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegressionLinear.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegressionLogistic.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegressionOrdinal.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegressionProbit.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RidgeAnalysize.cpp
# End Source File
# Begin Source File

SOURCE=.\RootTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Simuequation.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Survival.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SurvivalCox.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SurvivalKM.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SurvivalLT.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\public\System.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\public\TCLFile.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeSeansonal.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TimeSeries.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TimeX11.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TimeX12.cpp
# End Source File
# Begin Source File

SOURCE=.\TLADVN.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\public\TString.cpp

!IF  "$(CFG)" == "TLADVN - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "TLADVN - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Var.cpp
# End Source File
# Begin Source File

SOURCE=.\VEC.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AlgorithmBase.h
# End Source File
# Begin Source File

SOURCE=.\Arima.h
# End Source File
# Begin Source File

SOURCE=.\Classify.h
# End Source File
# Begin Source File

SOURCE=.\CointegrationTest.h
# End Source File
# Begin Source File

SOURCE=.\ConfTypes.h
# End Source File
# Begin Source File

SOURCE=.\Covariance.h
# End Source File
# Begin Source File

SOURCE=.\CurveReg.h
# End Source File
# Begin Source File

SOURCE=.\Discriminant.h
# End Source File
# Begin Source File

SOURCE=..\arithInterface\EnvParam.h
# End Source File
# Begin Source File

SOURCE=.\ExponSmoothing.h
# End Source File
# Begin Source File

SOURCE=.\Factor.h
# End Source File
# Begin Source File

SOURCE=..\Public\FileReadWrite.h
# End Source File
# Begin Source File

SOURCE=.\MarkovChain.h
# End Source File
# Begin Source File

SOURCE=.\MultiAnova.h
# End Source File
# Begin Source File

SOURCE=.\PCR.h
# End Source File
# Begin Source File

SOURCE=.\QuickCluster.h
# End Source File
# Begin Source File

SOURCE=.\RegressionLinear.h
# End Source File
# Begin Source File

SOURCE=.\RegressionLogistic.h
# End Source File
# Begin Source File

SOURCE=.\RegressionOrdinal.h
# End Source File
# Begin Source File

SOURCE=.\RegressionProbit.h
# End Source File
# Begin Source File

SOURCE=.\RidgeAnalysize.h
# End Source File
# Begin Source File

SOURCE=.\RootTest.h
# End Source File
# Begin Source File

SOURCE=.\Section.h
# End Source File
# Begin Source File

SOURCE=.\Simuequation.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Survival.h
# End Source File
# Begin Source File

SOURCE=.\SurvivalCox.h
# End Source File
# Begin Source File

SOURCE=.\SurvivalKM.h
# End Source File
# Begin Source File

SOURCE=.\SurvivalLT.h
# End Source File
# Begin Source File

SOURCE=.\Timer.h
# End Source File
# Begin Source File

SOURCE=.\TimeSeansonal.h
# End Source File
# Begin Source File

SOURCE=.\TimeSeries.h
# End Source File
# Begin Source File

SOURCE=.\TimeX11.h
# End Source File
# Begin Source File

SOURCE=.\TimeX12.h
# End Source File
# Begin Source File

SOURCE=.\tlAdvn.h
# End Source File
# Begin Source File

SOURCE=.\Var.h
# End Source File
# Begin Source File

SOURCE=.\VEC.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=".\ ±º‰≤‚ ‘.txt"
# End Source File
# End Target
# End Project
