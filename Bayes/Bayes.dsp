# Microsoft Developer Studio Project File - Name="Bayes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Bayes - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Bayes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Bayes.mak" CFG="Bayes - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Bayes - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Bayes - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Markway 单机版/天律分析系统/Bayes", UOXDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Bayes - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\cpknl\Release\cpknl.lib ..\tlAdvn\Release\tlAdvn.lib ..\DataInterface\Debug\DataInterface.lib ..\Result\Release\Result.lib ..\TLDBDll\Release\TLDBDll.lib /nologo /subsystem:windows /dll /machine:I386 /out:"E:\NetTest\Release\Bayes.dll"

!ELSEIF  "$(CFG)" == "Bayes - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\cpknl\Debug\cpknl.lib ..\tlAdvn\Debug\tlAdvn.lib ..\DataInterface\Debug\DataInterface.lib ..\Result\Debug\CResult.lib ..\tlADVN\Debug\tlAdvn.lib ..\TLDBDll\Debug\TLDBDll.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"E:\NetTest\Debug\Bayes.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Bayes - Win32 Release"
# Name "Bayes - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Public\AnaSyntax.cpp

!IF  "$(CFG)" == "Bayes - Win32 Release"

!ELSEIF  "$(CFG)" == "Bayes - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Public\AnaWord.cpp

!IF  "$(CFG)" == "Bayes - Win32 Release"

!ELSEIF  "$(CFG)" == "Bayes - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Bayes.cpp
# End Source File
# Begin Source File

SOURCE=.\BNet.cpp
# End Source File
# Begin Source File

SOURCE=.\BNode.cpp
# End Source File
# Begin Source File

SOURCE=..\arithInterface\EnvParam.cpp

!IF  "$(CFG)" == "Bayes - Win32 Release"

!ELSEIF  "$(CFG)" == "Bayes - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Public\System.cpp

!IF  "$(CFG)" == "Bayes - Win32 Release"

!ELSEIF  "$(CFG)" == "Bayes - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Public\TString.cpp

!IF  "$(CFG)" == "Bayes - Win32 Release"

!ELSEIF  "$(CFG)" == "Bayes - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BNet.h
# End Source File
# Begin Source File

SOURCE=.\BNode.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Bayes.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section Bayes : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
# Section Bayes : {FFB86F43-7B62-4AD0-8CD2-83B78EEB7EA6}
# 	2:5:Class:CImageView
# 	2:10:HeaderFile:imageview.h
# 	2:8:ImplFile:imageview.cpp
# End Section
# Section Bayes : {858CB9FB-21B3-4557-93E2-3C41A438E6CB}
# 	2:21:DefaultSinkHeaderFile:imageview.h
# 	2:16:DefaultSinkClass:CImageView
# End Section
# Section Bayes : {6B869077-0319-491B-B220-EA9931F64B9E}
# 	2:5:Class:CDataGrid
# 	2:10:HeaderFile:datagrid.h
# 	2:8:ImplFile:datagrid.cpp
# End Section
# Section Bayes : {F4392540-0CFE-101B-B22E-00AA0037B2FC}
# 	2:5:Class:CGridCtrl
# 	2:10:HeaderFile:gridctrl.h
# 	2:8:ImplFile:gridctrl.cpp
# End Section
# Section Bayes : {A8C3B720-0B5A-101B-B22E-00AA0037B2FC}
# 	2:21:DefaultSinkHeaderFile:gridctrl.h
# 	2:16:DefaultSinkClass:CGridCtrl
# End Section
# Section Bayes : {B4914ABC-D1B8-411C-B5D8-C5A283421925}
# 	2:21:DefaultSinkHeaderFile:datagrid.h
# 	2:16:DefaultSinkClass:CDataGrid
# End Section
# Section Bayes : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
