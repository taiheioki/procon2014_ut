# add library
file(GLOB SLIDE_SOURCES src/*.cpp include/slide/*.hpp)
add_library(slide ${SLIDE_SOURCES})

# set include directories
target_include_directories(slide PUBLIC include)
target_include_directories(slide PRIVATE include/slide)

# link libraries
target_link_libraries(slide util ${Boost_LIBRARIES} ${TBB_LIBRARIES})
