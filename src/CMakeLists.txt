include_directories(include)

add_subdirectory(input)
add_subdirectory(experiment)
add_subdirectory(util)
add_subdirectory(progressive)
add_subdirectory(index)
add_subdirectory(measurements)
add_subdirectory(sort)

add_library(ReactiveProgressiveIndexing experiment/experiments.cpp)
target_link_libraries(ReactiveProgressiveIndexing experiments util progressive index sort cost_model)
