# Set C standard to C11
set(CMAKE_C_STANDARD 11)

# Set compilation flags for debugging information
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

# Set source and include directories
set(SOURCE_DIR src)
set(INCLUDE_DIR include)

# Gather all C source files in the src directory
file(GLOB SOURCES "${SOURCE_DIR}/*.c")

# Include directories for header files
include_directories(${INCLUDE_DIR})

# Find the Curses library
find_package(Curses REQUIRED)

# Create an executable named "xhell" from the gathered sources
add_executable(xhell ${SOURCES})

# If Curses library is found, include its directories and link against it
if(CURSES_FOUND)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(xhell ${CURSES_LIBRARIES})
endif()

# Set the C flags for the entire project (including debug and release builds)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")