# Building DynAdjust from source code

The following build instructions are only needed if you would like to
build DynAdjust yourself, rather than use [pre-built
binaries](https://github.com/GeoscienceAustralia/DynAdjust/releases), or
make changes to the source code and [contribute to the DynAdjust
repository](../.github/CONTRIBUTING.md).


## Table of contents

- [Building DynAdjust from source
  code](#building-dynadjust-from-source-code){#toc-building-dynadjust-from-source-code}
  - [Overview of
    prerequisites](#overview-of-prerequisites){#toc-overview-of-prerequisites}
  - [Linux (Ubuntu)](#linux-ubuntu){#toc-linux-ubuntu}
    - [Installing
      prerequisites](#installing-prerequisites){#toc-installing-prerequisites}
    - [Compile the source
      code](#compile-the-source-code){#toc-compile-the-source-code}
  - [MacOS](#macos){#toc-macos}
    - [Install
      prerequisites](#install-prerequisites){#toc-install-prerequisites}
    - [Compile the source
      code](#compile-the-source-code-1){#toc-compile-the-source-code-1}
  - [Windows using
    Cmake](#windows-using-cmake){#toc-windows-using-cmake}
    - [Install
      prerequisites](#install-prerequisites-1){#toc-install-prerequisites-1}
    - [Install Intel oneAPI MKL
      library](#install-intel-oneapi-mkl-library){#toc-install-intel-oneapi-mkl-library}
    - [Download the source
      code](#download-the-source-code){#toc-download-the-source-code}
    - [Compile the source
      code](#compile-the-source-code-2){#toc-compile-the-source-code-2}
  - [Windows using Visual
    Studio](#windows-using-visual-studio){#toc-windows-using-visual-studio}
    - [Install Windows
      prerequisites](#install-windows-prerequisites){#toc-install-windows-prerequisites}
    - [Building Windows binaries in Visual
      Studio](#building-windows-binaries-in-visual-studio){#toc-building-windows-binaries-in-visual-studio}


## Overview of prerequisites

To build DynAdjust, the following prerequisites will be needed:

- A C++17 compiler (e.g. gcc, clang, Microsoft Visual Studio)
- [CMake](https://cmake.org/)
- [Boost](https://www.boost.org)
- [Apache Xerces](https://xerces.apache.org/)
- [Codesynthesis XSD](https://www.codesynthesis.com/products/xsd/)
- BLAS and LAPACK, either
  [OpenBLAS](http://www.openmathlib.org/OpenBLAS/) or [Intel oneAPI Math
  Kernel
  Library](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html)

The way in which these prerequisites are installed will depend upon your
operating system and will be discussed in the following sections.

## Linux (Ubuntu)

For the case of Linux, we will give instructions on how to compile for
Ubuntu. The appropriate package manager commands will vary depending on
the Linux distribution that you use. We highly recommend trying the
[pre-built static
binaries](https://github.com/GeoscienceAustralia/DynAdjust/releases)
that we now generate that will work on any Linux distribution without
any dependencies (assuming a x64 platform).

### Installing prerequisites

Update package lists.

``` bash
sudo apt update
```

Install runtime and development prerequisites.

``` bash
sudo apt install -y libxerces-c-dev xsdcxx libboost-system-dev libboost-filesystem-dev libboost-timer-dev libboost-thread-dev libboost-program-options-dev
```

You now have two options,
either[OpenBLAS](http://www.openmathlib.org/OpenBLAS/) or[Intel oneAPI
Math Kernel
Library](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html).

#### OpenBLAS

For OpenBLAS, just run:

``` bash
sudo apt install -y libopenblas-dev liblapacke-dev
```

#### Intel oneAPI Math Kernel Library

Either download and install yourself manually, or run:

``` bash
wget https://registrationcenter-download.intel.com/akdlm/IRC_NAS/ac050ae7-da93-4108-823d-4334de3f4ee8/intel-oneapi-base-toolkit-2025.1.2.9_offline.sh -O oneapi.sh
sudo sh oneapi.sh -a --action install --components intel.oneapi.lin.mkl.devel --eula accept -s
```

### Compile the source code

Download an official release (e.g., 1.2.8) with:

``` bash
curl -O https://github.com/GeoscienceAustralia/DynAdjust/archive/refs/tags/v1.2.8.tar.gz
tar xvf v1.2.8.tar.gz
cd ~/DynAdjust-1.2.8
```

If you are using Intel MKL, then load it.

``` bash
source /opt/intel/oneapi/setvars.sh
```

Now compile the code with cmake.

``` bash
mkdir build
cmake -S dynadjust -B build
cmake --build build --parallel
```

## MacOS

Building DynAdjust on MacOS is just as easy as Linux.

### Install prerequisites

Install Homebrew first if absent: visit <https://brew.sh> and run the
one-line install script.

Run the following commands to install the prerequisites.

``` bash
brew update
brew install boost xerces-c xsd
```

### Compile the source code

Download an official release (e.g., 1.2.8) with:

``` bash
curl -O https://github.com/GeoscienceAustralia/DynAdjust/archive/refs/tags/v1.2.8.tar.gz
tar xvf v1.2.8.tar.gz
cd ~/DynAdjust-1.2.8
```

If you are using Intel MKL, then load it.

``` bash
source /opt/intel/oneapi/setvars.sh
```

Now compile the code with cmake.

``` bash
mkdir build
cmake -S dynadjust -B build
cmake --build build --parallel
```

## Windows using Cmake

On Windows, you will first need to install
[git](https://gitforwindows.org).

### Install prerequisites

#### Install vcpkg

If you do not have it installed, install
[vcpkg](https://github.com/microsoft/vcpkg) using the following steps.

``` cmd
git clone https://github.com/microsoft/vcpkg C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

#### Install packages using vcpkg

Now we can install the dependencies.

``` cmd
vcpkg install boost-geometry boost-process boost-iostreams boost-spirit boost-system boost-filesystem boost-timer boost-thread boost-program-options boost-interprocess xerces-c
```

### Install Intel oneAPI MKL library

If you do not have it, install the [Intel oneAPI Math Kernel
Library](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html).

### Download the source code

Download an official release (e.g., 1.2.8) for example this link:

    https://github.com/GeoscienceAustralia/DynAdjust/archive/refs/tags/v1.2.8.zip

and now unzip it.

### Compile the source code

``` cmd
cd path\to\DynAdjust
mkdir build
cmake -G "Visual Studio 17 2022" -A x64 -D USE_MKL=ON -D CMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" -S dynadjust -B build
cmake --build build --config Release --parallel
```

## Windows using Visual Studio

Building DynAdjust on Windows requires two steps: (1) installation of
prerequisite tools and (2) compilation via Visual Studio C++.

### Install Windows prerequisites

#### Install Microsoft Visual Studio 2022 Community Edition

Microsoft's Visual Studio 2022 Community Edition is available from
https://visualstudio.microsoft.com/vs/community/

C++ is required for compiling all DynAdjust binaries. MFC is required
only for building `GeoidInt.exe` - Geoid Interpolation software with a
(dialog-based) graphical user interface.

#### Install Boost C++ headers and libraries

The supported versions of boost are 1.58.0 -- 1.87.0. The headers and
library sources are available from https://www.boost.org/users/download/

The boost libraries needed by DynAdjust include `filesystem`, `system`,
`program_options`, `thread`, `date_time`, `math`, `timer`, `atomic` and
`chrono`. These will need to be built from the boost C++ sources using
Visual Studio 2022, and installed to a suitable folder on your machine.

Follow the instructions on the [Boost
Website](https://www.boost.org/doc/libs/1_87_0/more/getting_started/windows.html#prepare-to-use-a-boost-library-binary)
to build the boost libraries from source. For example, the steps are:

1.  Download and extract boost to a suitable folder
2.  Run `Bootstrap.bat` to build `b2`
3.  Build the boost binaries using `b2`

For steps 2 and 3, run the following (assuming boost has been downloaded
and unzipped):

``` bat
rem Start building boost
echo 
echo Building bootstrap.bat
echo

rem inside the root directory where boost was unzipped, change to tools\build\
cd .\tools\build

rem build b2 using VS 2022
call bootstrap.bat vc143

rem Directory to boost root
set boost_dir=boost_1_87_0

rem Store compiled libraries in directories corresponding to 64-bit and 32-bit.
set stage_64=C:\Data\boost\%boost_dir%\lib\x64
set stage_32=C:\Data\boost\%boost_dir%\lib\Win32
set headers=C:\Data\boost\%boost_dir%\include

rem Number of cores to use when building boost
set cores=%NUMBER_OF_PROCESSORS%

rem Visual Studio 2022
set msvcver=msvc-14.3

rem change to the root directory, copy b2 to root
cd ..\..
copy .\tools\build\b2.exe .\

rem Static libraries (64 bit)
echo Building %boost_dir% (64-bit) with %cores% cores using toolset %msvcver%.
echo Destination directory is %stage_64%
b2 -j%cores% toolset=%msvcver% address-model=64 architecture=x86 link=static,shared threading=multi runtime-link=shared --build-type=minimal stage --stagedir=%stage_64%

rem move contents of %stage_64%\lib to %stage_64%
move %stage_64%\lib\* %stage_64%\
del %stage_64%\lib

rem Static libraries (32 bit)
echo Building %boost_dir% (32-bit) with %cores% cores using toolset %msvcver%.
echo Destination directory is %stage_32%
b2 -j%cores% toolset=%msvcver% address-model=32 architecture=x86 link=static,shared threading=multi runtime-link=shared --build-type=minimal stage --stagedir=%stage_32%

rem move contents of %stage_32%\lib to %stage_32%
move %stage_32%\lib\* %stage_32%\
rmdir /S /Q %stage_32%\lib

rem make include folder (C:\Data\boost\%boost_dir%\include) and move headers (boost folder)
md %headers%
move .\boost %headers% 
```

The DynAdjust repository includes a Visual Studio property sheet
(`dynadjust.props`), which allows you to set the folder paths to the
boost header files and libraries on your machine. The boost header and
library folder paths are saved within `dynadjust.props` as *User
Macros*, named **BoostIncludeDir** and **BoostLibDir**, and are
referenced throughout the solution's project properties. Saving thes
paths in a global property sheet provides a convenient way to reference
custom boost C++ file paths across the entire solution without having to
change individual property page for each project.

By default, the boost paths are set as follows. Change these to match
the location of the boost header files and libraries on your machine,
making sure that `\lib\` contains two folders named `x64` and `Win32` if
you need to build 64-bit and 32-bit binaries respectively.

- **BoostIncludeDir:** `C:\Data\boost\boost_1_87_0\include\`
- **BoostLibDir:** `C:\Data\boost\boost_1_87_0\lib\$(Platform)`

#### Install CodeSynthesis XSD and Apache xerces-c headers and libraries

DynAdjust requires CodeSynthesis XSD (version 4.0) headers and Apache
xerces-c headers and libraries. XSD x86 and x64 Windows dependencies are
available as a bundle via:
https://www.codesynthesis.com/products/xsd/download.xhtml

Note that the XSD installer (.msi) for Windows includes precompiled
Apache Xerces-C++ libraries (32 and 64 bit) for all the supported Visual
Studio versions.

If the default installation path
(`C:\Program Files (x86)\CodeSynthesis XSD 4.0`) is used during setup,
the XSD and xerces-c paths will be correctly referenced via the Visual
Studio property sheet `dynadjust.props`. As with the boost paths, the
header and library folder paths for XSD and xerces-c are saved using
*User Macros*, named **XsdIncludeDir**, **XsdLibDir_x64**, and
**XsdLibDir_Win32**:

- **XsdIncludeDir**:
  `C:\Program Files (x86)\CodeSynthesis XSD 4.0\include`
- **XsdLibDir_x64**:
  `C:\Program Files (x86)\CodeSynthesis XSD 4.0\lib64\vc-12.0`
- **XsdLibDir_Win32**:
  `C:\Program Files (x86)\CodeSynthesis XSD 4.0\lib\vc-12.0`

If an alternative installation path is chosen, change the User Macros
accordingly.

#### Install Intel oneAPI Math Kernel Library (MKL)

DynAdjust requires Intel's oneAPI MKL and TBB libraries. A free version
of oneAPI is available from:
https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html

With Visual Studio 2022 already installed, the Intel oneAPI installer
will automatically enable integration into the Visual Studio 2022 IDE.
This means that the oneAPI MKL and TBB libraries and headers will be
automatically referenced upon compiling DynAdjust without modification.

> **Note:** The entire oneAPI toolkit is quite large -- choose MKL
> installation only for a minimum build set up.

### Building Windows binaries in Visual Studio

DynAdjust is comprised of several executables and dependent dynamic link
libraries (DLL), each of which is managed and configured as a VC++
project. All projects are contained within a single solution file
`dynadjust_x_xx_xx.sln`.

The VC++ project for each executable is named using the convention
`dna<program-name>wrapper`, except for the main program `dynadjust`.
Upon compilation, these projects will create executables named
`<program-name>.exe`. Each executable named `<program-name>.exe` is
dependent on a DLL named `dna<program-name>.dll`. The DLLs must and will
be compiled first before compiling the executables.

The executable projects and their dependent DLLs are listed below: -
dnaadjustwrapper - dnaadjust - dnageoidwrapper - dnageoid -
dnaimportwrapper - dnaimport - dnaplotwrapper - dnaplot -
dnareftranwrapper - dnareftran - dnasegmentwrapper - dnasegment -
dynadjust (no project dependencies, but requires all preceding projects
to be built for normal execution behaviour) - GeoidInt - dnageoid

For each VC++ project, four build configurations have been created: 1.
Debug Win32 2. Release Win32 3. Debug x64 4. Release x64

The project properties pages for each executable and DLL project make
use of User Macros that simplify the creation of settings for the four
configurations.

Given that many functions are shared throughout the suite of executables
and DLLs, the DynAdjust solution makes extensive use of precompiled
headers to simplify and speed up compile time.
