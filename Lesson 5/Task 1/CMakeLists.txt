cmake_minimum_required(VERSION 3.20)

project(L5T1)

add_executable(run main.cpp)

add_subdirectory(C:/libpqxx-7.10.0 libpqxx-build)

target_compile_features(run PRIVATE cxx_std_17)

target_link_libraries(run pqxx)