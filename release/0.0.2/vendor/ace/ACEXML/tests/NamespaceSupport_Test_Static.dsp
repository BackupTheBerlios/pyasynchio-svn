# Microsoft Developer Studio Project File - Name="NamespaceSupport_Test_Static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=NamespaceSupport_Test_Static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE run the tool that generated this project file and specify the
!MESSAGE nmake output type.  You can then use the following command:
!MESSAGE
!MESSAGE NMAKE /f "NamespaceSupport_Test_Static.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "NamespaceSupport_Test_Static.mak" CFG="NamespaceSupport_Test_Static - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "NamespaceSupport_Test_Static - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "NamespaceSupport_Test_Static - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NamespaceSupport_Test_Static - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Static_Release"
# PROP Intermediate_Dir "Static_Release\NamespaceSupport_Test_Static"
# PROP Target_Dir ""
# ADD CPP /nologo /O2 /W3 /GX /MD /GR /I "..\.." /I "..\..\ACEXML\common" /D NDEBUG /D WIN32 /D _CONSOLE /D ACE_AS_STATIC_LIBS /FD /c
# SUBTRACT CPP /YX

# ADD MTL /D "NDEBUG" /nologo /mktyplib203 /win32
# ADD RSC /l 0x409 /d NDEBUG /i "..\.." /i "..\..\ACEXML\common"
BSC32=bscmake.exe
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO ACEXML_Parsers.lib ACEXMLs.lib ACEs.lib /libpath:"..\..\lib" /nologo /version:5.403 /subsystem:console /pdb:"Static_Release\NamespaceSupport_Test.pdb"  /machine:I386 /out:"Static_Release\NamespaceSupport_Test.exe"

!ELSEIF  "$(CFG)" == "NamespaceSupport_Test_Static - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Static_Debug"
# PROP Intermediate_Dir "Static_Debug\NamespaceSupport_Test_Static"
# PROP Target_Dir ""
# ADD CPP /nologo /Ob0 /W3 /Gm /GX /Zi /MDd /GR /Gy /I "..\.." /I "..\..\ACEXML\common" /D _DEBUG /D WIN32 /D _CONSOLE /D ACE_AS_STATIC_LIBS /FD /c
# SUBTRACT CPP /YX

# ADD MTL /D "_DEBUG" /nologo /mktyplib203 /win32
# ADD RSC /l 0x409 /d _DEBUG /i "..\.." /i "..\..\ACEXML\common"
BSC32=bscmake.exe
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO ACEXML_Parsersd.lib ACEXMLsd.lib ACEsd.lib /libpath:"..\..\lib" /nologo /version:5.403 /subsystem:console /pdb:"Static_Debug\NamespaceSupport_Test.pdb" /debug /machine:I386 /out:"Static_Debug\NamespaceSupport_Test.exe"

!ENDIF

# Begin Target

# Name "NamespaceSupport_Test_Static - Win32 Release"
# Name "NamespaceSupport_Test_Static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;cxx;c"
# Begin Source File

SOURCE="NamespaceSupport_Test.cpp"
# End Source File
# End Group
# End Target
# End Project
