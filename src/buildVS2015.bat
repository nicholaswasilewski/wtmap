@echo off
 
set start=%cd%
 
pushd "%~dp0"
 
FOR /F "skip=2 tokens=2,*" %%A IN ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7" /v 14.0') DO set "VSPATH=%%B"
CALL "%VSPATH%"\VC\vcvarsall.bat x86

IF NOT EXIST ..\bin mkdir ..\bin
pushd ..\bin

rc /fo ..\bin\wtmap.res ..\res\wtmap.rc
cl /nologo /O2 /fp:fast ..\src\win32_wtmap.cpp /Z7 /link wtmap.res /PDB:wtmap.pdb /incremental:no /opt:ref

popd
