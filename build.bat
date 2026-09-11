@echo off

setlocal enabledelayedexpansion

cd /d "%~dp0"

set "ROOT=%~dp0"

if not defined VULKAN_SDK (
    echo ERROR: VULKAN_SDK is not set.
    echo Please install the Vulkan SDK.
    exit /b 1
)

::set GLFW_DIR=E:\Vulkan\glfw-3.4\glfw-3.4

set SOURCES_C_GLFW=
for /r "third_party\glfw" %%f in (*.c) do (
    set SOURCES_C_GLFW=!SOURCES_C_GLFW! "%%f"
)
set SOURCES_C=
for /r "src" %%f in (*.c) do (
    set SOURCES_C=!SOURCES_C! "%%f"
)

set SOURCES_CPP=
for /r "third_party" %%f in (*.cpp) do (
    set SOURCES_CPP=!SOURCES_CPP! "%%f"
)
for /r "src_cpp" %%f in (*.cpp) do (
    set SOURCES_CPP=!SOURCES_CPP! "%%f"
)

set INCLUDES=/I "%ROOT%src"
set INCLUDES=%INCLUDES% /I "%ROOT%src\core"
set INCLUDES=%INCLUDES% /I "%ROOT%src\platform"
set INCLUDES=%INCLUDES% /I "%ROOT%src\vulkan_backend"
set INCLUDES=%INCLUDES% /I "%ROOT%src\renderer"
set INCLUDES=%INCLUDES% /I "%ROOT%src\app"
set INCLUDES=%INCLUDES% /I "%ROOT%src_cpp"
set INCLUDES=%INCLUDES% /I "%ROOT%src\fea"
set INCLUDES=%INCLUDES% /I "%ROOT%src\graphics"

set INCLUDES=%INCLUDES% /I "%VULKAN_SDK%\Include"
set INCLUDES=%INCLUDES% /I "%ROOT%third_party\glfw"
set INCLUDES=%INCLUDES% /I "%ROOT%include"
set INCLUDES=%INCLUDES% /I "%ROOT%include\GLFW"
set INCLUDES=%INCLUDES% /I "%ROOT%third_party\imgui"
set INCLUDES=%INCLUDES% /I "%ROOT%third_party\imgui\backends"

set LIBPATHS=/LIBPATH:"%VULKAN_SDK%\Lib" 
set LIBS=vulkan-1.lib user32.lib gdi32.lib shell32.lib


if not exist build mkdir build
if not exist build\glfw mkdir build\glfw
cl /nologo /c /MD /Zi /EHsc /D_GLFW_WIN32 %INCLUDES% !SOURCES_C_GLFW! /Fo:build\glfw\  /Fd:build\app.pdb
cl /nologo /c /MD /Zi /EHsc /D_GLFW_WIN32 %INCLUDES% !SOURCES_C! /Fo:build\  /Fd:build\app.pdb 

if %ERRORLEVEL% NEQ 0 (
    echo Build FAILED C.
    exit /b 1
)



cl /nologo /c /MD /Zi  /EHsc %INCLUDES% !SOURCES_CPP! /Fo:build\ /Fd:build\third_party.pdb

if %ERRORLEVEL% NEQ 0 (
    echo Build FAILED CPP.
    exit /b 1
)


link /DEBUG /OUT:build\app.exe build\*.obj build\glfw\*.obj %LIBPATHS% %LIBS%

if %ERRORLEVEL% NEQ 0 (
    echo Build FAILED LINKER.
    exit /b 1
)

echo Build succeeded: build\app.exe
