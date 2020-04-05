cmake_minimum_required(VERSION 3.15)
project(multythreading)

set(CMAKE_CXX_STANDARD 14)
# -fno-omit-frame-pointer
# -fno-omit-frame-pointer
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -fsanitize=address")
#set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")
#set(ASAN_OPTIONS symbolize=1)
#set(ASAN_SYMBOLIZER_PATH /usr/bin/llvm-symbolizer)


find_library(PTHREAD_LIBRARY pthread)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

find_package(MPI)
find_package(Boost 1.65.1 COMPONENTS mpi)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

## use this only for mpi??

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)


add_executable(multythreading main.cpp include/PlatformDefiner.h include/Callable.h include/AbstractSystemInterrupter.h include/to_string_helper.h include/AbstractTemplatedParallelRunner.h sources/LinuxProccessParallelRunner.cpp include/LinuxProccessParallelRunner.h include/twofish.h sources/twofish.cpp include/LinuxThreadParallelRunner.h sources/encryptor.cpp include/encryptor.h include/AbstractProcessFactory.h include/parallel_decryptor.h include/OpenMpParallelRunner.h include/MPIParallelRunner.h)


target_link_libraries(multythreading ${PTHREAD_LIBRARY})
target_link_libraries(multythreading ${MPI_C_LIBRARIES})
if(Boost_FOUND)
    target_link_libraries(multythreading ${Boost_LIBRARIES})
endif()