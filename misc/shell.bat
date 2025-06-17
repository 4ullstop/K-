@echo off
call "D:\VisualStudio\VC\Auxiliary\Build\vcvarsall.bat" x64 10.0.26100.0
set INCLUDE=C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um;C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\winrt;C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\x64;%INCLUDE%
set path=k:\misc;%path%
