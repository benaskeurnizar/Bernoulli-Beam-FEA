@echo off

setlocal enabledelayedexpansion

cd /d "%~dp0"

set VULKAN_SDK=C:\VulkanSDK\1.4.350.0
set GLFW_DIR=E:\Vulkan\glfw-3.4\glfw-3.4

set SOURCES=
for /r "src" %%f in (*.c) do (
    set SOURCES=!SOURCES! "%%f"
)

set INCLUDES=/I src /I src\core /I src\platform /I src\vk /I src\renderer /I src\app
set INCLUDES=%INCLUDES% /I "%VULKAN_SDK%\Include" /I "%GLFW_DIR%\include" /I"include"

set LIBPATHS=/LIBPATH:"%VULKAN_SDK%\Lib" /LIBPATH:"%GLFW_DIR%\build\src\Release"
set LIBS=vulkan-1.lib glfw3.lib user32.lib gdi32.lib shell32.lib


if not exist build mkdir build

cl /nologo  /MD /Zi /EHsc %INCLUDES% !SOURCES! /Fo:build\ /Fe:build\app.exe /Fd:build\app.pdb /link %LIBPATHS% %LIBS%

if %ERRORLEVEL% NEQ 0 (
    echo Build FAILED.
    exit /b 1
)

echo Build succeeded: build\app.exe
