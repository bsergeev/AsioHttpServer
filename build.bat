@ECHO OFF

SET config=Debug
IF NOT "%1"=="" (
  SET config=%1
)
echo Building configuration: %config%

mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..

call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
msbuild AsioHttpSever.sln /p:Configuration=%config%
cd ..

build\%config%\tests.exe
