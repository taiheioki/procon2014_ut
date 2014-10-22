# add library
file(GLOB UTIL_SOURCES src/*.cpp include/util/*.hpp)
add_library(util ${UTIL_SOURCES})

# set include directories
target_include_directories(util PUBLIC include)
target_include_directories(util PRIVATE include/util)

# link libraries
target_link_libraries(util ${Boost_LIBRARIES})
