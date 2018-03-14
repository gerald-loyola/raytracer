@echo off

set COMPILER_FLAGS=-O2 -Zi -DHALF_HD=1 -DSINGLE_THREAD=0 -DCHUNK_PIXEL_SIZE=64 -DRAYS_PER_PIXEL=512 -DMAX_BOUNCE=8 -DWIN32_PLATFORM

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM MAIN BUILD
cl %COMPILER_FLAGS% ..\code\ray.cpp
IF NOT %ERRORLEVEL%==0 GOTO :end

REM start /WAIT ray.exe
REM start /WAIT ray_cast_test.bmp

popd

:end
