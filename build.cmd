::
:: USE clang-cl
::
@cls
@set CLANGCL="D:/PROD/programs/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm\\bin/clang-cl.exe"

::  /Fa<file or dir/>       Set assembly output file name (with /FA)
::  /Fe<file or dir/>       Set output executable file name
::  /Fi<file>               Set preprocess output file name (with /P)
::  /Fo<file or dir/>       Set output object file (with /c)
::  /o <file or dir/>       Deprecated (set output file name); use /Fe or /Fe

:: /GS-                    Disable buffer security check
:: /GS                     Enable buffer security check (default)
::
::  /WX-                    Do not treat warnings as errors (default)
::  /WX                     Treat warnings as errors
::
::  /Gd                     Set __cdecl as a default calling convention
::
::  /O2                     Optimize for speed (like /Og /Oi /Ot /Oy /Ob2 /GF /Gy)
::  /Oi-                    Disable use of builtin functions
::  /Oi                     Enable use of builtin functions
::  /Ox                     Deprecated (like /Og /Oi /Ot /Oy /Ob2); use /O2
::
set LNKR=/link /MACHINE:X64 /NXCOMPAT /SUBSYSTEM:CONSOLE /OPT:ICF /OPT:REF /INCREMENTAL:NO
::
:: release build
:: /Fa"Release\" /Fo"Release\" 
@set SWITCHES=/WX- /Gd /Oi /MD /nologo /GR- /permissive- /std:c++17 /GS /W3 /Gy /O2 /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /D "_HAS_EXCEPTIONS=0"  /DYNAMICBASE "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" 
::
:: compile
::
%CLANGCL% %SWITCHES% program.cpp fwk/main.cpp %LNKR%
::
@set CLANGCL=
@set SWITCHES=
@set LNKR=