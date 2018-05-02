cmake_minimum_required (VERSION 2.8)
project(image_extraction_plugin)

find_package(catkin_simple REQUIRED)
#find_package(OpenCV REQUIRED)
#catkin_simple()
catkin_simple(ALL_DEPS_REQUIRED)

#add_definitions(-fPIC -shared)

MESSAGE("CMAKE CXX flags: " ${CMAKE_CXX_FLAGS})
SET(CMAKE_CXX_FLAGS "-std=c++11")
MESSAGE("CMAKE CXX flags: " ${CMAKE_CXX_FLAGS})

#find_package(hdf5_catkin REQUIRED)
#set(HDF5_INCLUDE_DIRS "/usr/local/hdf5/include")
#set(HDF5_LIB_DIRS "/usr/local/hdf5/lib")
#set(HDF5_LIB_DIRS "/usr/lib/x86_64_linux-gnu/hdf5/serial")
#include_directories(${HDF5_INCLUDE_DIRS})
#link_directories(${HDF5_LIB_DIRS})
#link_libraries(${HDF5_LIB_DIRS})
#link_libraries(hdf5)
#link_libraries(hdf5_hs_cpp)

set(Boost_INCLUDE_DIRS /usr/include)
set(Boost_LIBRARY_DIRS /usr/lib)
#set(Boost_USE_STATIC_LIBS   ON)
#find_package(Boost COMPONENTS filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})      # set manually since mixes /usr/local/include with /usr/lib/x86_64-linux-gnu
link_directories(${Boost_LIBRARY_DIRS})


cs_add_library(${PROJECT_NAME} src/image-extraction-plugin.cc src/hdf5_handler.cc src/image_handler.cc)
#target_link_libraries(${PROJECT_NAME} hdf5 hdf5_cpp hdf5_hl_cpp)   # no need since package depends on hdf5_catkin
target_link_libraries(${PROJECT_NAME} boost_filesystem boost_system)
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
create_console_plugin(${PROJECT_NAME})

FILE(GLOB_RECURSE LibFiles "include/*")
add_custom_target(headers SOURCES ${LibFiles})

## OpenCV3
# Custom compilation - define paths
#set(OpenCV_INCLUDE_DIRS "/usr/local/opencv3.4/include")
#set(OpenCV_LIB_DIRS "/usr/local/opencv3.4/lib")
#MESSAGE("OpenCV3 include dir: " ${OpenCV_INCLUDE_DIRS})
#find_package(OpenCV REQUIRED)
#MESSAGE("OpenCV3 lib dir: " ${OpenCV_LIB_DIRS})
#MESSAGE("OpenCV3 libs: " ${OpenCV_LIBS})   # only if env variable was set by find_package()

## HDF5
#MESSAGE("HDF5 library found: " ${HDF5_FOUND})
#MESSAGE("HDF5 parallel IO support: " ${HDF5_IS_PARALLEL})
#MESSAGE("HDF5 include dirs: " ${HDF5_INCLUDE_DIRS})
#MESSAGE("HDF5 lib dirs: " ${HDF5_LIB_DIRS})
#MESSAGE("HDF5 definitions: " ${HDF5_DEFINITIONS})
#MESSAGE("HDF5 libs: " ${HDF5_LIBRARIES})
#MESSAGE("HDF5 C libs: " ${HDF5_C_LIBRARIES})
#MESSAGE("HDF5 C++ libs: " ${HDF5_CXX_LIBRARIES})

## Boost
MESSAGE("Boost library found: " ${Boost_FOUND})
MESSAGE("Boost version: " ${Boost_VERSION})
MESSAGE("Boost include dirs: " ${Boost_INCLUDE_DIRS})
MESSAGE("Boost library dirs: " ${Boost_LIBRARY_DIRS})
MESSAGE("Boost libs: " ${Boost_LIBRARIES})
MESSAGE("CMAKE version: " ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})


## Set include and lib paths
#include_directories({OpenCV_INCLUDE_DIR})
#link_directories({OpenCV_LIB_DIRS})


## Link libraries
# env var defined by find_package()
#target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
# manual setting of required packages
#target_link_libraries(${PROJECT_NAME} opencv_core opencv_highgui opencv_imgproc opencv_features2d opencv_xfeatures2d)

#target_link_libraries(${PROJECT_NAME} ${CATKIN_LIBRARIES} hdf5 hdf5_cpp hdf5_hl_cpp)

cs_install()
cs_export()