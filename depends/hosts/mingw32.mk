mingw32_CC=x86_64-w64-mingw32-gcc-posix
mingw32_CXX=x86_64-w64-mingw32-g++-posix
#mingw32_CFLAGS=-pipe -fopenmp -DPTW32_STATIC_LIB
#mingw32_CXXFLAGS=$(mingw32_CFLAGS) -std=c++11
mingw32_CFLAGS=-pipe
mingw32_CXXFLAGS=$(mingw32_CFLAGS)
#mingw32_CXXFLAGS=$(mingw32_CFLAGS) -isystem $(host_prefix)/include/c++/v1

#mingw32_LDFLAGS?=-fuse-ld=lld

mingw32_release_CFLAGS=-O3
mingw32_release_CXXFLAGS=$(mingw32_release_CFLAGS)

mingw32_debug_CFLAGS=-O0
mingw32_debug_CXXFLAGS=$(mingw32_debug_CFLAGS)

mingw32_debug_CPPFLAGS=-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
