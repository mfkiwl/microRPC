# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.5)

# Set the project name and version
project(microRPC VERSION 1.0.0)

# Set the source files for uCommander
set(microRPC_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/microRPC.c"
)

# Set the header files for uString
set(microRPC_HDRS 
  "${CMAKE_CURRENT_SOURCE_DIR}/include/microRPC.h",
  "${CMAKE_CURRENT_SOURCE_DIR}/include/helpers.h",
  "${CMAKE_CURRENT_SOURCE_DIR}/include/runTable.h",
  "${CMAKE_CURRENT_SOURCE_DIR}/include/dataTable.h"
)

# Create the shared library for uCommander
add_library(microRPC SHARED
  ${microRPC_SRCS}
  ${microRPCS_HDRS}
)

# Set the output directory for the shared library
set_target_properties(microRPC PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)




