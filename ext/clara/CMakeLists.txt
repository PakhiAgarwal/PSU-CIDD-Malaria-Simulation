cmake_minimum_required(VERSION 2.8.8)
project(clara_builder CXX)
include(ExternalProject)
find_package(Git REQUIRED)
ExternalProject_Add(
    clara
    PREFIX ${CMAKE_BINARY_DIR}/ext
    GIT_REPOSITORY https://github.com/catchorg/Clara
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(clara source_dir)
MESSAGE(${source_dir})
set(CLARA_INCLUDE_DIR ${source_dir}/single_include CACHE INTERNAL "Path to include folder for Catch")

