cmake_minimum_required(VERSION 3.16.3)
project(budd)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -DNDEBUG")

#for threasd



SET(LINK_DIR /usr/local/lib)
link_directories(${LINK_DIR})

add_subdirectory(base)
add_subdirectory(tcp)
add_subdirectory(http)
add_subdirectory(test)







 
 



 
