SOURCE_DIR=$(dirname "$0")
patch -N /usr/local/cuda/include/cuda_gl_interop.h $SOURCE_DIR'/cuda_gl_interop.patch
