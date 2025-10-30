cmake_minimum_required(VERSION 3.28)
project(
    labkit 
    VERSION 2025.0
    DESCRIPTION "A C++ library for communication with lab instruments"
    LANGUAGES CXX)

# Set the c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug mode: Add '-g -Wall -DLC_DEBUG'
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -Wall)
    add_compile_definitions(LC_DEBUG)
endif()

# Get all source files from src/
file(GLOB_RECURSE SOURCES "src/*.cpp") 

message(STATUS "Found sources: ${SOURCES}")

# Create a static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

# Add include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:inc>
    PRIVATE
        ${LIBUSB_INCLUDE_DIRS}
)

# Link libusb-1.0
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${LIBUSB_LIBRARIES}
)

# Add compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE ${LIBUSB_CFLAGS_OTHER})

# Create a .pc-file for pkg-config
set(PKG_CONFIG_NAME "${PROJECT_NAME}")
set(PKG_CONFIG_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(PKG_CONFIG_REQUIRES "libusb-1.0")
set(PKG_CONFIG_LIBS "-l${PROJECT_NAME}")
set(PKG_CONFIG_CFLAGS "-I\${includedir}")

# .pc.in = template for .pc file; .pc = output file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    @ONLY
)

# Install library
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY inc/
    DESTINATION include
)

# Install pkg-config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    DESTINATION lib/pkgconfig
)
