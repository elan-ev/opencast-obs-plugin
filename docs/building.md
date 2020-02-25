Building the plugin
===================

[Back to the main page](./)

A short outline of what needs to be done in order to build the plugin from source.


Build dependencies
------------------

Ensure that all of the following build dependencies are present on your system:

- CMake
- C++ Compiler (C++11 support needed)
- obs-studio (20.0.1 compatible + libobs and headers)
- qt (5.9.2 compatible)
- libconfig (1.5 compatible)
- [libOpencastIngest](https://github.com/elan-ev/lib-opencast-ingest) (0.4.0
  compatible)
    - add header files into `libs/libOpencastIngest/include`
        - `libOpencastIngest.hpp`
        - `libOpencastIngest.h`
    - add the static library binary to `libs/libOpencastIngest/lib/libOpencastIngest.{a|lib}`
          - This library also requires: libcurl, tinyxml2


Building on Linux
-----------------

To build the project on Linux(tested in Fedora 32, Ubuntu 18.04 and Archlinux) run:

```bash
set -uex

apt update
apt install -y cmake git libcurl4-gnutls-dev libtinyxml2-dev g++ build-essential libobs-dev qtbase5-dev libconfig++-dev

cd
git clone https://github.com/elan-ev/lib-opencast-ingest.git
cd lib-opencast-ingest/
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

cd
git clone https://github.com/elan-ev/opencast-obs-plugin.git
cd opencast-obs-plugin/libs/libOpencastIngest/include/
ln -s /root/lib-opencast-ingest/src/libOpencastIngest.h  
ln -s /root/lib-opencast-ingest/src/libOpencastIngest.hpp 
cd ../lib
ln -s /root/lib-opencast-ingest/build/libOpencastIngest_static.a  libOpencastIngest.a
cd ../../..
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```


Building on Windows
-------------------

Building on Windows is way more complicated and in general, we recommend using
the binary distributions. Nevertheless, here is a short outline of what you need
to do:

With [msys2](http://www.msys2.org/) installed you can perform the following steps:

1. Clone the source repository
2. Install the Microsoft Visual C++ Compiler (Visual Studio or Visual Studio
   Build Tools 15 2017)
3. Use the MinGW 32-bit/64-bit console
4. Install the Requirements (libcurl, tinyxml2, libconfig) (watch out to use the
   correct architecture/build chain) by compiling the install target with cmake
   (admin rights needed).
5. Put the files of libOpencastIngest according to the requirements into the
   source directory
6. Install qt 5.9.2 for msvc2015 and msvc2017\_x64 to the default path
7. Compile obs-studio relative to the source directory of opencastObsPlugin:
   `../obs-studio/build/`, on 32-bit:  `../obs-studio/build32/` (cmake target
   obs)
8. Then execute the following commands (this should create a file, which is
   called `obsOpencastIngestPlugin.dll` in the `build/Release/` directory):

32-bit (on MinGW 32-bit console):
```bash
mkdir build
cd build/
cmake -G "Visual Studio 15 2017" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

64-bit (on MinGW 64-bit console):
```bash
mkdir build
cd build/
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

Warning: You can only have one version of the libraries (32/64-bit) installed.
If you want to build 32 and 64-bit versions you have to remove the other version
of the libraries first.
