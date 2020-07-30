cmake_minimum_required(VERSION 3.16.3)
project(budd)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#for threasd


SET(EXECUTABLE_OUTPUT_PATH ../bin)

#for test
foreach(target socketServer socketClient)

    include_directories(${Boost_INCLUDE_DIR} ../include/)

    ADD_DEFINITIONS( "-DHAS_BOOST" )
    file(GLOB SOURCES "../src/*.cpp" ./test/${target}.cpp)
    
    add_executable(${target} ${SOURCES})

    target_link_libraries(${target} ${Boost_LIBRARIES})

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${target} PUBLIC Threads::Threads)
endforeach(target)






 
 



 
