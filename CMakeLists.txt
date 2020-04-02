cmake_minimum_required(VERSION 3.15)
project(multythreading)

set(CMAKE_CXX_STANDARD 14)
# -fno-omit-frame-pointer
# -fno-omit-frame-pointer
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

find_library(PTHREAD_LIBRARY pthread)
add_executable(multythreading main.cpp include/PlatformDefiner.h include/UnixForkProcessRunner.h include/Callable.h include/AbstractSystemInterrupter.h include/to_string_helper.h include/AbstractTemplatedParallelRunner.h sources/LinuxProccessParallelRunner.cpp include/LinuxProccessParallelRunner.h include/twofish.h sources/twofish.cpp include/LinuxThreadParallelRunner.h sources/encryptor.cpp include/encryptor.h include/AbstractProcessFactory.h include/parallel_decryptor.h sources/OpenMpParallelRunner.cpp include/OpenMpParallelRunner.h)
target_link_libraries(multythreading ${PTHREAD_LIBRARY})
