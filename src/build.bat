@echo off

IF NOT EXIST ..\bin mkdir ..\bin
pushd ..\bin

rc /fo ..\bin\wtmap.res ..\res\wtmap.rc
cl /nologo /O2 /fp:fast ..\src\win32_wtmap.cpp /Z7 /link wtmap.res /PDB:wtmap.pdb /incremental:no /opt:ref

popd
