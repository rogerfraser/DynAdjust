# Installing DynAdjust

The steps required to install DynAdjust on your system will depend upon your operating system, and whether or not you choose to [build from source code](BUILDING.md) or install pre-built binaries. This page will provide instructions on installing and running pre-built binaries.

Installation may require the installation of one or more prerequisite applications that are external to DynAdjust but essential for its installation and use.

## Contents
- [Pre-built Binary Packages](#pre-built-binary-packages)
  - [Windows Packages](#windows-packages)
  - [macOS Packages](#macos-packages)
  - [Linux Packages](#linux-packages)
  - [Verifying the Installation](#verifying-the-installation)
  - [Troubleshooting Notes](#troubleshooting-notes)
- [Docker Image](#docker-image)


## Pre-built Binary Packages

[![GitHub Releases](https://img.shields.io/github/v/release/geoscienceaustralia/DynAdjust.svg)](https://github.com/geoscienceaustralia/DynAdjust/releases)

For each stable release, we publish pre-compiled **zip** archives for various platforms on the [releases page](https://github.com/geoscienceaustralia/dynadjust/releases/latest).

| Zip file (example)               | Platform                 | BLAS/LAPACK backend | Link type |
|----------------------------------|--------------------------|---------------------|-----------|
| `dynadjust-windows-openblas.zip` | Windows 10/11 x64        | OpenBLAS            | dynamic   |
| `dynadjust-windows-mkl.zip`      | Windows 10/11 x64        | Intel MKL           | dynamic   |
| `dynadjust-macos.zip`            | macOS 14 (Apple Silicon) | Apple Accelerate    | dynamic   |
| `dynadjust-macos-static.zip`     | macOS 14 (Apple Silicon) | Apple Accelerate    | static    |
| `dynadjust-linux-openblas.zip`   | Ubuntu 22.04 +           | OpenBLAS            | dynamic   |
| `dynadjust-linux-static.zip`     | Any modern x86‑64 Linux  | OpenBLAS            | static    |

Note that we build DynAdjust in two flavours: *dynamic* and *static*.

The *dynamic* build keeps the executable small and relies on shared system libraries that are already present, distributed by the operating system, or shared across multiple software packages. That means security patches and performance upgrades (e.g., new Intel MKL version) arrive automatically through normal system updates.

The *static* build moves in the opposite direction: we combine every required library directly into one self-contained binary. Nothing else has to be installed for the program to run, which is invaluable when DynAdjust needs to be run in the cloud or in production as it provides an exact version for regulatory reproducibility. Because it is insulated from future changes in operating-system libraries, it gives us a deterministic, "known-good" binary that will behave identically even after the underlying machines, operating systems, or libraries are patched or replaced.

Maintaining both variants therefore gives us flexibility: the dynamic build inherits dependency updates automatically, while the static build guarantees portability and long-term reproducibility in controlled settings.

Each archive contains the seven command‑line tools:
```
dynadjust, dnaadjust, dnageoid, dnaimport, dnaplot, dnareftran, dnasegment
```
For dynamic builds, additional shared libraries (`.dll`, `.so`, `.dylib`) are included and each DynAdjust command is broken into a wrapper executable and a shared library.

### Windows Packages

#### Windows (with OpenBLAS) Package

1. Unzip `dynadjust-windows-openblas.zip` (e.g. to `C:\Tools\DynAdjust`).
2. Add `C:\Tools\DynAdjust` to the system PATH environment variable.
3. Open the command prompt, and run the following command to verify the installation has been successful:
```bash
dnaadjust.exe --help
```

#### Windows (with Intel MKL) Package

1. Install the **Intel OneAPI Math Kernel Library (oneMKL)** from the [Intel oneMKL website](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-download.html).  No extra configuration required because the installer registers MKL on the system search path.
2. Unzip `dynadjust-windows-mkl.zip` (e.g. to `C:\Tools\DynAdjust`).
3. Add `C:\Tools\DynAdjust` to the system PATH environment variable.
4. Open the command prompt, and run the following command to verify the installation has been successful:
```bash
dnaadjust.exe --help
```

### macOS Packages

#### Dynamic Build (`dynadjust-macos.zip`)

Install Homebrew if you do not already have it (<https://brew.sh>), then:

```bash
brew install boost xerces-c
```

Unzip the archive, move the binaries and dylib files somewhere on `$PATH` (e.g. `/usr/local/bin`).


#### Static Build (`dynadjust-macos-static.zip`)

No dependencies: simply unzip and run. Gatekeeper may require **right‑click → Open** the first time.

### Linux Packages

#### Dynamic Build (`dynadjust-linux-openblas.zip`)

Tested on Ubuntu 22.04 LTS:

```bash
sudo apt update
sudo apt install -y libxerces-c libboost-system libboost-filesystem  libboost-thread libboost-program-options libopenblas liblapacke
```

Unzip the archive, then run:

```bash
LD_LIBRARY_PATH=$PWD 
./dnaadjust --help
```

#### Static Build (`dynadjust-linux-static.zip`)

The binary is self‑contained. On any x86‑64 Linux:

```bash
unzip dynadjust-linux-static.zip
cd dynadjust-linux-static
./dnaadjust --help
```

Ideal for minimal Docker images or an Amazon EC2 instance.

### Verifying the Installation

```bash
dnaadjust --version
dnageoid  --help
```

If you see an error like **"cannot open shared object file"** revisit the prerequisite steps for your platform.

### Troubleshooting Notes

* *Windows OpenBLAS*: missing DLLs → ensure the path to where Dynadjust was unzipped is on `PATH`, then start a new command prompt.
* *macOS*: Gatekeeper warnings can be cleared by opening the app via **right‑click → Open** once.
* *Static Linux*: verify you downloaded the x86‑64 build (`uname -m` prints `x86_64`).
* *Performance*: the MKL build generally runs faster on Intel CPUs; OpenBLAS is licence‑friendly and portable.

## Docker Image

[![docker build](https://img.shields.io/github/workflow/status/icsm-au/dynadjust/Build%20docker%20image?label=docker%20build)](https://hub.docker.com/repository/docker/icsm/dynadjust)
[![Docker Pulls](https://img.shields.io/docker/pulls/icsm/dynadjust)](https://hub.docker.com/repository/docker/icsm/dynadjust)

The DynAdjust repository comes with a [Dockerfile](https://github.com/icsm-au/DynAdjust/blob/master/Dockerfile) which builds a DynAdjust docker image for the Linux environment each time changes are pushed to the main repository. This means if you have docker installed on your system, a DynAdjust image can be run on your system (whether Linux, Mac or Windows) within a virtual environment managed by docker.

To access the latest docker image, please visit the [DynAdjust repo on Docker Hub](https://hub.docker.com/r/icsm/dynadjust).

Alternatively, you can pull a DynAdjust docker image from your system via:

  ``` bash
  $ docker pull icsm/dynadjust
  ```

