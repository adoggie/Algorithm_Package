# Microsoft Developer Studio Project File - Name="TLDBDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TLDBDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TLDBDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TLDBDll.mak" CFG="TLDBDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TLDBDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TLDBDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ÍøÂç°æ2/C/TLDBDll", VRVDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TLDBDLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TLDBDLL_EXPORTS" /FD /c
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
# ADD LINK32 /nologo /dll /machine:I386 /out:"E:\NetTest\CppRelease\WebRoot\WEB-INF\lib\TLDBDll.dll"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TLDBDLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TLDBDLL_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /out:"E:\NetTest\CppDebug\WebRoot\WEB-INF\lib\TLDBDll.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TLDBDLL - Win32 Release"
# Name "TLDBDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Database\cbooleanfield.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\ccurrencyfield.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cdataset.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cdatefield.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cdoublefield.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cfakerecno.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cfakerecnos.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cfield.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cfields.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cfileoperation.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cheader.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cintfield.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\COriginRecPos.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cpage.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cpages.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cqueueitem.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cstringfield.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\cvaluelabel.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\DataAccess.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\DriverBase.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\DriverManager.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\fieldlist.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\fieldsversion.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\Missing.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\PageArraySort.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\showcharacteristic.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Public\System.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Public\TCLFile.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\tempfile.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TLDBDLL.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Public\TString.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\versionlist.cpp

!IF  "$(CFG)" == "TLDBDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "TLDBDLL - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Database\cbooleanfield.h
# End Source File
# Begin Source File

SOURCE=.\Database\ccurrencyfield.h
# End Source File
# Begin Source File

SOURCE=.\Database\cdataset.h
# End Source File
# Begin Source File

SOURCE=.\Database\cdatefield.h
# End Source File
# Begin Source File

SOURCE=.\Database\cdoublefield.h
# End Source File
# Begin Source File

SOURCE=.\Database\cfakerecno.h
# End Source File
# Begin Source File

SOURCE=.\Database\cfakerecnos.h
# End Source File
# Begin Source File

SOURCE=.\Database\cfield.h
# End Source File
# Begin Source File

SOURCE=.\Database\cfields.h
# End Source File
# Begin Source File

SOURCE=.\Database\cfileoperation.h
# End Source File
# Begin Source File

SOURCE=.\Database\cheader.h
# End Source File
# Begin Source File

SOURCE=.\Database\cintfield.h
# End Source File
# Begin Source File

SOURCE=.\Database\COriginRecPos.h
# End Source File
# Begin Source File

SOURCE=.\Database\cpage.h
# End Source File
# Begin Source File

SOURCE=.\Database\cpages.h
# End Source File
# Begin Source File

SOURCE=.\Database\cqueueitem.h
# End Source File
# Begin Source File

SOURCE=.\Database\cstringfield.h
# End Source File
# Begin Source File

SOURCE=.\Database\cvaluelabel.h
# End Source File
# Begin Source File

SOURCE=.\Database\DataAccess.h
# End Source File
# Begin Source File

SOURCE=.\Database\DriverBase.h
# End Source File
# Begin Source File

SOURCE=.\Database\DriverManager.h
# End Source File
# Begin Source File

SOURCE=.\Database\fieldlist.h
# End Source File
# Begin Source File

SOURCE=.\Database\fieldsversion.h
# End Source File
# Begin Source File

SOURCE=.\headtldb.h
# End Source File
# Begin Source File

SOURCE=.\Database\Missing.h
# End Source File
# Begin Source File

SOURCE=.\Database\PageArraySort.h
# End Source File
# Begin Source File

SOURCE=.\Database\showcharacteristic.h
# End Source File
# Begin Source File

SOURCE=.\Database\tempfile.h
# End Source File
# Begin Source File

SOURCE=.\Database\versionlist.h
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
