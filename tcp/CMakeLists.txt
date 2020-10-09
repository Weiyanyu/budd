set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -DNDEBUG")

SET(LINK_DIR /usr/local/lib)
link_directories(${LINK_DIR})

set(base_SRCS
  acceptor.cpp
  channel.cpp
  eventLoop.cpp
  eventLoopThread.cpp
  eventLoopThreadPool.cpp
  selector.cpp
  tcpconnection.cpp
  tcpserver.cpp
  timer.cpp
  )

add_library(budd_tcp ${base_SRCS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(budd_tcp PUBLIC Threads::Threads glog)
include_directories(../)


install(TARGETS budd_tcp DESTINATION lib)

file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/muduo/base)



