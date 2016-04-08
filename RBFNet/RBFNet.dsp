# Microsoft Developer Studio Project File - Name="RBFNet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=RBFNet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RBFNet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RBFNet.mak" CFG="RBFNet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RBFNet - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RBFNet - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RBFNet - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RBFNET_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RBFNET_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ..\Cpknl\Release\Cpknl.lib ..\DataInterface\Release\DataInterface.lib ..\TLDBDLL\Release\TLDBDll.lib ..\Result\Release\CResult.lib /nologo /dll /machine:I386 /out:"E:\NetTest\CppRelease\WebRoot\WEB-INF\lib\RBFNet.dll"

!ELSEIF  "$(CFG)" == "RBFNet - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RBFNET_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RBFNET_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\Cpknl\Debug\Cpknl.lib ..\DataInterface\Debug\DataInterface.lib ..\TLDBDLL\Debug\TLDBDll.lib ..\Result\Debug\CResult.lib /nologo /dll /debug /machine:I386 /out:"E:\NetTest\CppDebug\WebRoot\WEB-INF\lib\RBFNet.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "RBFNet - Win32 Release"
# Name "RBFNet - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Public\AnaSyntax.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\Public\AnaWord.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\arithInterface\EnvParam.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\Public\FileReadWrite.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\KMeans_Algorithm.cpp
# End Source File
# Begin Source File

SOURCE=..\Public\Packet.cpp
# End Source File
# Begin Source File

SOURCE=.\RBFN_Algorithm.cpp
# End Source File
# Begin Source File

SOURCE=.\RBFNet.cpp
# End Source File
# Begin Source File

SOURCE=..\Public\System.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\Public\TCLFile.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\Public\TString.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\Public\Util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\KMeans_Algorithm.h
# End Source File
# Begin Source File

SOURCE=.\RBFN_Algorithm.h
# End Source File
# Begin Source File

SOURCE=.\RBFNet.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
