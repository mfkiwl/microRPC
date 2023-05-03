# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.5)

# Set the project name and version
project(microRPCTest VERSION 1.0.0)

# Set the source files for the test
set(test_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/microRPCTest.c"
)

# Set the header files for the test
set(test_HDRS
  "${CMAKE_CURRENT_SOURCE_DIR}/microRPCTest.h"
)

# Find the shared library for uString
find_library(microRPC_LIBRARY microRPC
  HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
)

# Create the executable for the test
add_executable(test
  ${test_SRCS}
  ${test_HDRS}
)

# Link the executable with the shared library for uString
target_link_libraries(test
  ${microRPC_LIBRARY}
)

# Set the output directory for the executable
set_target_properties(test PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
)
