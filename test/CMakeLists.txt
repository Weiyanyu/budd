set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

SET(EXECUTABLE_OUTPUT_PATH ../../bin)
add_executable(testHttpServer testHttpServer.cpp)
target_link_libraries(testHttpServer budd_http budd_tcp budd_base)
include_directories(../)



 
