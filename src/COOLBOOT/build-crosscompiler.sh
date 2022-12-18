# install dependencies
sudo apt install build-essential
sudo apt install bison
sudo apt install flex
sudo apt install libgmp3-dev
sudo apt install libmpc-dev
sudo apt install libmpfr-dev
sudo apt install texinfo
sudo apt install libisl-dev

export PREFIX="/usr/local/i686-elf-gcc"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir ~/toolchain-src
cd ~/toolchain-src

wget "https://ftp.gnu.org/gnu/binutils/binutils-2.38.tar.gz"
wget "https://mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-11.3.0/gcc-11.3.0.tar.gz"

mkdir binutils-build
mkdir gcc-build

tar xvf binutils-2.38.tar.gz
tar xvf gcc-11.3.0.tar.gz

cd binutils-build
../binutils-2.38/configure --prefix=$PREFIX --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --with-sysroot
make
make install

cd ../gcc-build
../gcc-11.3.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c,c++ --without-headers --enable-interwork --enable-multilib
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

ls $PREFIX/bin/
