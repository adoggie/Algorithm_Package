# Microsoft Developer Studio Project File - Name="NEURALNET" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NEURALNET - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NEURALNET.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NEURALNET.mak" CFG="NEURALNET - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NEURALNET - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NEURALNET - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/�����2/C/NEURALNET", UXWDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NEURALNET - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NEURALNET_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NEURALNET_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ..\Cpknl\Release\Cpknl.lib ..\DataInterface\Release\DataInterface.lib ..\TLDBDLL\Release\TLDBDll.lib ..\Result\Release\CResult.lib /nologo /dll /machine:I386 /out:"E:\NetTest\CppRelease\WebRoot\WEB-INF\lib\NEURALNET.dll"

!ELSEIF  "$(CFG)" == "NEURALNET - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NEURALNET_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NEURALNET_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\Cpknl\Debug\Cpknl.lib ..\DataInterface\Debug\DataInterface.lib ..\TLDBDLL\Debug\TLDBDll.lib ..\Result\Debug\CResult.lib /nologo /dll /debug /machine:I386 /out:"E:\NetTest\CppDebug\WebRoot\WEB-INF\lib\NEURALNET.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NEURALNET - Win32 Release"
# Name "NEURALNET - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\PUBLIC\AnaSyntax.cpp
# End Source File
# Begin Source File

SOURCE=..\PUBLIC\AnaWord.cpp
# End Source File
# Begin Source File

SOURCE=.\ARTClusterLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\ARTClusterNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ARTInterfaceLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\ARTInterfaceNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ARTNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ARTTopology.cpp
# End Source File
# Begin Source File

SOURCE=.\Bedlam.cpp
# End Source File
# Begin Source File

SOURCE=.\BPTopology.cpp
# End Source File
# Begin Source File

SOURCE=..\arithInterface\EnvParam.cpp
# End Source File
# Begin Source File

SOURCE=..\Public\FileReadWrite.cpp
# End Source File
# Begin Source File

SOURCE=.\HopGen.cpp
# End Source File
# Begin Source File

SOURCE=.\KLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\KNode.cpp
# End Source File
# Begin Source File

SOURCE=.\KohonenTopology.cpp
# End Source File
# Begin Source File

SOURCE=.\NeuralNet.cpp
# End Source File
# Begin Source File

SOURCE=.\NeuralNetDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\NeuralNetInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\RBTopology.cpp
# End Source File
# Begin Source File

SOURCE=.\Sample.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleSet.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\Public\System.cpp

!IF  "$(CFG)" == "NEURALNET - Win32 Release"

!ELSEIF  "$(CFG)" == "NEURALNET - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\public\TCLFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Topology.cpp
# End Source File
# Begin Source File

SOURCE=..\public\TString.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ARTClusterLayer.h
# End Source File
# Begin Source File

SOURCE=.\ARTClusterNode.h
# End Source File
# Begin Source File

SOURCE=.\ARTInterfaceLayer.h
# End Source File
# Begin Source File

SOURCE=.\ARTInterfaceNode.h
# End Source File
# Begin Source File

SOURCE=.\ARTNode.h
# End Source File
# Begin Source File

SOURCE=.\ARTTopology.h
# End Source File
# Begin Source File

SOURCE=.\Bedlam.h
# End Source File
# Begin Source File

SOURCE=.\BPTopology.h
# End Source File
# Begin Source File

SOURCE=.\HopGen.h
# End Source File
# Begin Source File

SOURCE=.\KLayer.h
# End Source File
# Begin Source File

SOURCE=.\KNode.h
# End Source File
# Begin Source File

SOURCE=.\KohonenTopology.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNet.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetInterface.h
# End Source File
# Begin Source File

SOURCE=.\RBTopology.h
# End Source File
# Begin Source File

SOURCE=.\Sample.h
# End Source File
# Begin Source File

SOURCE=.\SampleSet.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Topology.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
