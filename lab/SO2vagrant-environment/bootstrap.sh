#!/usr/bin/env bash

#apt-get update
dnf update

# Needed tools to compile bochs...
# apt-get -y install build-essential
dnf -y groupinstall "Development Tools"
#apt-get -y install libx11-dev
dnf -y install libXi-devel
#apt-get -y install libgtk2.0-dev
dnf -y install gtk2-devel
#apt-get -y install libreadline-dev
dnf -y install readline

# Needed tools for Zeos
dnf -y install vim
# apt-get -y install bin86
dnf -y install dev86
dnf -y install gdb
dnf -y install git

# Download BOCHS
mkdir tmp
cd tmp
VERS=2.6.7
BOCHS=bochs-${VERS}
wget https://sourceforge.net/projects/bochs/files/bochs/${VERS}/${BOCHS}.tar.gz/download -O ${BOCHS}.tar.gz

tar zxf ${BOCHS}.tar.gz

# Compile BOCHS...
# ... without GDB
mkdir build
cd build
LDFLAGS=-pthread ../${BOCHS}/configure --enable-debugger --enable-disasm --enable-x86-debugger --enable-readline --with-x --prefix=/opt/bochs
make
make install
cd ..

# ... with GDB
mkdir build-gdb
cd build-gdb
../${BOCHS}/configure --enable-gdb-stub --with-x --prefix=/opt/bochs_gdb
make
make install
cd ..

# Create links to bochs binaries
ln -sf /opt/bochs/bin/bochs /usr/local/bin/bochs_nogdb
ln -sf /opt/bochs_gdb/bin/bochs /usr/local/bin/bochs

cd ..

# Clean environment
rm -rf tmp


