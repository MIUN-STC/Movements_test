apt install -y mercurial automake

hg clone https://hg.libsdl.org/SDL SDL
cd SDL
mkdir build
cd build
../configure
make
make install
cd ..
cd ..

hg clone https://hg.libsdl.org/SDL_image SDL_image
cd SDL_image
./autogen.sh
mkdir build
cd build
../configure
make
make install
cd ..
cd ..
