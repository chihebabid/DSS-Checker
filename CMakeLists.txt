cmake_minimum_required(VERSION 3.0)

project(distributedstatespace C CXX)

# compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()

# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++17")
endif()

# build spot if it is not installed on the system
find_library(SPOT_LIBRARY "spot")
find_library(BDDX_LIBRARY "bddx")

if(NOT SPOT_LIBRARY)
  include(ExternalProject)

  message(STATUS "Building Spot ...")
  set(SPOT_DIR "${CMAKE_BINARY_DIR}/third-party/spot")

  ExternalProject_Add(SpotLibrary
    PREFIX                ${SPOT_DIR}
    URL                   http://www.lrde.epita.fr/dload/spot/spot-2.8.7.tar.gz
    DOWNLOAD_NO_PROGRESS  YES
    CONFIGURE_COMMAND     ./configure --disable-doxygen --disable-python --enable-silent-rules --silent --prefix=<INSTALL_DIR>
    BUILD_COMMAND         make -j
    BUILD_IN_SOURCE       1
  )

  include_directories("${SPOT_DIR}/include")
else ()
 message(STATUS "Found spot: ${SPOT_LIBRARY}")
 message(STATUS "Found bddx: ${BDDX_LIBRARY}")

 find_path(SPOT_INCLUDE_PATH "spot")
 include_directories("${SPOT_INCLUDE_PATH}")
endif()

# Third-party libraries PATH
set(THIRD_PARTY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third-party)

# CLI library
add_library(cli11 INTERFACE)
target_include_directories(cli11 INTERFACE "${THIRD_PARTY_INCLUDE_DIR}/CLI11")




# add source folder
include_directories(src)
add_subdirectory(src)


