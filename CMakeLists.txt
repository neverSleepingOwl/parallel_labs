cmake_minimum_required(VERSION 3.15)
project(multythreading)

set(CMAKE_CXX_STANDARD 14)
find_library(PTHREAD_LIBRARY pthread)
add_executable(multythreading main.cpp include/PlatformDefiner.h include/UnixForkProcessRunner.h include/Callable.h include/AbstractSignalHandler.h include/to_string_helper.h include/AbstractParallelRunner.h sources/LinuxProccessParallelRunner.cpp include/LinuxProccessParallelRunner.h include/twofish.h sources/twofish.cpp sources/LinuxThreadParallelRunner.cpp include/LinuxThreadParallelRunner.h)
target_link_libraries(multythreading ${PTHREAD_LIBRARY})
