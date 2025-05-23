# DynAdjust — Using the Pre‑built Binary Packages

This document explains how to run the pre‑compiled **zip** archives we publish for some platforms.

| Zip file (example)               | Platform                 | BLAS/LAPACK backend | Link type |
|----------------------------------|--------------------------|---------------------|-----------|
| `dynadjust-windows-openblas.zip` | Windows 10/11 x64        | OpenBLAS            | dynamic   |
| `dynadjust-windows-mkl.zip`      | Windows 10/11 x64        | Intel MKL           | dynamic   |
| `dynadjust-macos.zip`            | macOS 14 (Apple Silicon) | Apple Accelerate    | dynamic   |
| `dynadjust-macos-static.zip`     | macOS 14 (Apple Silicon) | Apple Accelerate    | static    |
| `dynadjust-linux-openblas.zip`   | Ubuntu 22.04 +           | OpenBLAS            | dynamic   |
| `dynadjust-linux-static.zip`     | Any modern x86‑64 Linux  | OpenBLAS            | static    |

Note that we build DynAdjust in two flavours: dynamic and static.

The *dynamic* build is the one most people will install on desktops. It keeps the executable small and relies on shared system libraries that are already present, distributed by the operating system, or shared across multiple software packages. That means security patches and performance upgrades (e.g., new Intel MKL version) arrive automatically through normal system updates.

The *static* build moves in the opposite direction: we compile every required library directly into one self-contained binary. Nothing else has to be installed for the program to run, which is invaluable when we need to run DynAdjust in the cloud or in production as it provides an exact version for regulatory reproducibility. Because it is insulated from future changes in operating-system libraries, it gives us a deterministic, “known-good” binary that will behave identically next year even if the underlying machines, operating systems, or libraries are patched or replaced.

Maintaining both variants therefore gives us flexibility: the dynamic build inherits dependency updates automatically, while the static build guarantees portability and long-term reproducibility in controlled settings.

Each archive contains the six command‑line tools:
```
dnaadjust   dnageoid   dnaimport   dnaplot   dnareftran   dnasegment
```
For dynamic builds additional shared libraries (`.dll`, `.so`, `.dylib`) are included and each DynAdjust command is broken into a wrapper executable and a shared library.

---

## Windows packages

### Install *vcpkg* (one‑time)

Open **PowerShell** with admin rights and run:

```powershell
git clone https://github.com/microsoft/vcpkg C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

Add it to your `PATH` permanently if desired:

```powershell
setx PATH "$env:PATH;C:\vcpkg"
```

### Windows + OpenBLAS package

Install the runtime DLLs with `vcpkg`

```powershell
vcpkg install boost-geometry boost-process boost-iostreams boost-spirit boost-system boost-filesystem boost-timer boost-thread boost-program-options boost-interprocess xerces-c openblas lapack-reference 
```

Unzip **`dynadjust-windows-openblas.zip`** (e.g. to `C:\Tools\DynAdjust`) and run `dnaadjust.exe`.
If Windows cannot find `libopenblas.dll` ensure `C:\vcpkg\installed-windows` is on `PATH`.

### Windows + MKL package

Install the runtime DLLs with `vcpkg`

```powershell
vcpkg install boost-geometry boost-process boost-iostreams boost-spirit boost-system boost-filesystem boost-timer boost-thread boost-program-options boost-interprocess xerces-c
```

Install the **Intel OneAPI Base Toolkit** (runtime‑only, MKL component) from the Intel website.

Unzip **`dynadjust-windows-mkl.zip`** and use the tools—no extra configuration required because the installer registers MKL on the system search path.

---

## MacOS packages

### Dynamic build (`dynadjust-macos.zip`)

Install Homebrew if you do not already have it (<https://brew.sh>), then:

```bash
brew install boost xerces-c
```

Unzip the archive, move the binaries and dylib files somewhere on `$PATH` (e.g. `/usr/local/bin`).


### Static build (`dynadjust-macos-static.zip`)

No dependencies: simply unzip and run. Gatekeeper may require **right‑click → Open** the first time.

---

## Linux packages

### Dynamic build (`dynadjust-linux-openblas.zip`)

Tested on Ubuntu 22.04 LTS:

```bash
sudo apt update
sudo apt install -y libxerces-c libboost-system libboost-filesystem  libboost-thread libboost-program-options libopenblas liblapacke
```

Unzip the archive, then run:

```bash
LD_LIBRARY_PATH=$PWD ./dnaadjust --help
```

### Static build (`dynadjust-linux-static.zip`)

The binary is self‑contained. On any x86‑64 Linux with kernel ≥ 3.10:

```bash
unzip dynadjust-linux-static.zip
cd dynadjust-linux-static
./dnaadjust --help
```

Ideal for minimal Docker images or an Amazon EC2 instance.

---

## Verifying the installation

```bash
dnaadjust --version
dnageoid  --help
```

If you see an error like **“cannot open shared object file”** revisit the prerequisite steps for your platform.

---

## Troubleshooting notes

* *Windows OpenBLAS*: missing DLL → ensure vcpkg `bin` directory is on `PATH`, then start a new terminal.
* *macOS*: Gatekeeper warnings can be cleared by opening the app via **right‑click → Open** once.
* *Static Linux*: verify you downloaded the x86‑64 build (`uname -m` prints `x86_64`).
* *Performance*: the MKL build generally runs faster on Intel CPUs; OpenBLAS is licence‑friendly and portable.
