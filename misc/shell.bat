@echo off
call "D:\VisualStudio\VC\Auxiliary\Build\vcvarsall.bat" x64 10.0.26100.0
set INCLUDE=C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um;C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\winrt;%INCLUDE%
set path=f:\misc;%path%
