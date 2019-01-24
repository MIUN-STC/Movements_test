SOURCE_DIR=$(dirname "$0")
patch -N /usr/local/cuda/include/cuda_gl_interop.h $SOURCE_DIR'/cuda_gl_interop.patch'

cd /usr/lib/aarch64-linux-gnu/
ln -sf tegra/libGL.so libGL.so
cd -

apt-get install -y \
	cmake \
	libavcodec-dev \
	libavformat-dev \
	libavutil-dev \
	libeigen3-dev \
	libglew-dev \
	libgtk2.0-dev \
	libgtk-3-dev \
	libjasper-dev \
	libjpeg-dev \
	libpng12-dev \
	libpostproc-dev \
	libswscale-dev \
	libtbb-dev \
	libtiff5-dev \
	libv4l-dev \
	libxvidcore-dev \
	libx264-dev \
	qt5-default \
	zlib1g-dev \
	pkg-config



apt-get install -y \
	libgstreamer1.0-dev \
	libgstreamer-plugins-base1.0-dev


git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

cd opencv
mkdir build
cd build

# Jetson TX2
ARCH_BIN=6.2
# Jetson TX1
#ARCH_BIN=5.3

cmake \
	-D CMAKE_BUILD_TYPE=RELEASE \
	-D WITH_CUDA=ON \
	-D CUDA_ARCH_BIN=${ARCH_BIN} \
	-D CUDA_ARCH_PTX="" \
	-D ENABLE_FAST_MATH=ON \
	-D CUDA_FAST_MATH=ON \
	-D WITH_CUBLAS=ON \
	-D WITH_LIBV4L=ON \
	-D WITH_GSTREAMER=ON \
	-D WITH_GSTREAMER_0_10=OFF \
	-D WITH_QT=ON \
	-D WITH_OPENGL=ON \
	-D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules/ \
	../

if [ $? -eq 0 ] ; then
	echo "OpenCV CMake successful"
else
	echo "OpenCV CMake unsuccessful"
	exit 1
fi




make -j6

if [ $? -eq 0 ] ; then
	echo "OpenCV make successful"
else
	echo "OpenCV make unsuccessful"
	exit 1
fi




make install

if [ $? -eq 0 ] ; then
	echo "OpenCV make install successful"
else
	echo "OpenCV make install unsuccessful"
	exit 1
fi

ldconfig

cd ..
cd ..


















