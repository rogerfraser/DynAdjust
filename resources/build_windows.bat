
set CWD=%CD%
set BUILD_DIR=%CD%\build_vcpkg

if exist %BUILD_DIR% rmdir %BUILD_DIR% /s/q

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

vcpkg integrate install

call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat" intel64 vs2022 --force
rem call "C:\Program Files (x86)\Intel\oneAPI\compiler\latest\env\vars.bat"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set MKLROOT=C:\Program Files (x86)\Intel\oneAPI\mkl\latest
set MKLINCLUDE=%MKLROOT%\include
set XSDROOT=C:\Program Files (x86)\CodeSynthesis XSD 4.0\include
set BOOSTROOT=C:\Data\boost\boost_1_87_0\include
set VCPKG_INSTALLATION_ROOT=C:\Data\vcpkg

set INCLUDE=%MKLINCLUDE%;%XSDROOT%;%BOOSTROOT%;%INCLUDE%

set UseEnv=true

cmake -DUSE_MKL=ON -DLP64=ON -DCMAKE_TOOLCHAIN_FILE=%VCPKG_INSTALLATION_ROOT%/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022" -A x64 ..\dynadjust
rem echo INCLUDE =          %INCLUDE%
cmake --build %cd% --config Release --parallel

cd %CWD%