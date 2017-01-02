@ECHO OFF

SET config=Debug
IF NOT "%1"=="" (
  SET config=%1
)
echo Building configuration: %config%

mkdir build
cd build
cmake -G "Visual Studio 14 2015 Win64" ..
call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" x64
msbuild AsioSampleHttpSever.sln /p:Configuration=%config%
cd ..

dir /N build\%config%\*.exe
