@echo off

set commonCompilerFlags= -MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4100 -wd4201 -wd4189 -wd4505 -wd4101 -DINTERNAL=1 -DDIRECT_SLOW=1 -DWIN32=1 -DDEBUG=1 -FC -Zi


set commonLinkerFlags= -incremental:no /LIBPATH:"D:\Effects11\FX11-main\Bin\Windows10\x64\Debug" user32.lib gdi32.lib winmm.lib d3d11.lib  D3DCompiler.lib Effects11d.lib dxgi.lib dxguid.lib 

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

cl %commonCompilerFlags% ..\code\win32_direct.cpp /link %commonLinkerFlags%
popd
