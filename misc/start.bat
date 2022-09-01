@echo off

REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
REM devenv F:\Projects\Graphics\handmade\build\Handmade.exe

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd ..
REM devenv .\build\Handmade.exe
code .