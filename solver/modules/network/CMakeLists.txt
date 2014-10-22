# add library
file(GLOB NETWORK_SOURCES src/*.cpp include/network/*.hpp)
add_library(network ${NETWORK_SOURCES})

# set include directories
target_include_directories(network PUBLIC include)
target_include_directories(network PRIVATE include/network)

# link libraries
target_link_libraries(network slide ${Boost_LIBRARIES} ${OpenSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CPPNETLIB_LIBRARIES})
