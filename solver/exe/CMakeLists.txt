message(STATUS "Added the following executables:")

add_executable(hello hello.cpp)
message(STATUS "  hello")

add_executable(test_optimize test_optimize.cpp)
target_link_libraries(test_optimize slide)
message(STATUS "  test_optimize")

add_executable(run_slide run_slide.cpp)
target_link_libraries(run_slide slide)
message(STATUS "  run_slide")

add_executable(test_slide test_slide.cpp)
target_link_libraries(test_slide slide)
message(STATUS "  test_slide")

add_executable(iterate_slide iterate_slide.cpp)
target_link_libraries(iterate_slide slide)
message(STATUS "  iterate_slide")

add_executable(test_matrix test_matrix.cpp)
target_link_libraries(test_matrix util)
message(STATUS "  test_matrix")

add_executable(kurage_chuning kurage_chuning.cpp)
target_link_libraries(kurage_chuning slide)
message(STATUS "  kurage_chuning")

add_executable(problem_server problem_server.cpp)
target_link_libraries(problem_server slide util network)
message(STATUS "  problem_server")

add_executable(problem_client problem_client.cpp)
target_link_libraries(problem_client slide util network)
message(STATUS "  problem_client")

add_executable(post_server post_server.cpp)
target_link_libraries(post_server slide util network)
message(STATUS "  post_server")

add_executable(post_client post_client.cpp)
target_link_libraries(post_client slide util network)
message(STATUS "  post_client")
