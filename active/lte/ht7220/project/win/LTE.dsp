# Microsoft Developer Studio Project File - Name="LTE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=LTE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LTE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LTE.mak" CFG="LTE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LTE - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "LTE - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LTE - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "LTE - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../../../util" /I "../../../../common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "LTE - Win32 Release"
# Name "LTE - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\app.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\config.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\dlist.c
# End Source File
# Begin Source File

SOURCE=..\..\lte.c
# End Source File
# Begin Source File

SOURCE=..\..\lte_cell_sel.c
# End Source File
# Begin Source File

SOURCE=..\..\lte_cfg.c
# End Source File
# Begin Source File

SOURCE=..\..\lte_oam.c
# End Source File
# Begin Source File

SOURCE=..\..\lte_sm.c
# End Source File
# Begin Source File

SOURCE=..\..\lte_util.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\modem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_gpio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_net.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_time.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_uart.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\sim_ctrl.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\..\util\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\dlist.h
# End Source File
# Begin Source File

SOURCE=..\..\lte.h
# End Source File
# Begin Source File

SOURCE=..\..\lte_cell_sel.h
# End Source File
# Begin Source File

SOURCE=..\..\lte_cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\lte_sm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\modem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_net.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_time.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\oss_uart.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\util\sim_ctrl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
