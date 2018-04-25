cmake_minimum_required (VERSION 2.8)
project(image_extraction_plugin)

find_package(catkin_simple REQUIRED)
#find_package(OpenCV REQUIRED)
catkin_simple(ALL_DEPS_REQUIRED)

add_definitions(-fPIC -shared)
#FIND_LIBRARY(HDF5_LIBRARY hdf5)
find_package(HDF5 REQUIRED)
#FIND_LIBRARY(HDF5_LIBRARY hdf5)

#target_link_libraries(src/image-extraction-plugin.cc hdf5 hdf5_cpp hdf5_hs_cpp)
cs_add_library(${PROJECT_NAME} src/image-extraction-plugin.cc src/hdf5_handler.cc src/extractor.cpp)
#target_link_libraries(${PROJECT_NAME} hdf5 hdf5_cpp hdf5_hl_cpp)
#target_link_libraries(src/image-extraction-plugin.cc hdf5 hdf5_cpp hdf5_hs_cpp)
create_console_plugin(${PROJECT_NAME})

FILE(GLOB_RECURSE LibFiles "include/*")
add_custom_target(headers SOURCES ${LibFiles})

## OpenCV3
# Custom compilation - define paths
#set(OpenCV_INCLUDE_DIRS "/usr/local/opencv3.4/include")
#set(OpenCV_LIB_DIRS "/usr/local/opencv3.4/lib")
#MESSAGE("OpenCV3 include dir: " ${OpenCV_INCLUDE_DIRS})
#MESSAGE("OpenCV3 lib dir: " ${OpenCV_LIB_DIRS})
#MESSAGE("OpenCV3 libs: " ${OpenCV_LIBS})

## HDF5
MESSAGE("HDF5 include dirs: " ${HDF5_INCLUDE_DIRS})
MESSAGE("HDF5 library: " ${HDF5_LIBRARY})

## Set include and lib paths
#include_directories({OpenCV_INCLUDE_DIR})
#link_directories({OpenCV_LIB})


## Link libraries
# env var defined by find_package()
#target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
# manual setting of required packages
#target_link_libraries(${PROJECT_NAME} opencv_core opencv_highgui opencv_imgproc opencv_features2d opencv_xfeatures2d)

#target_link_libraries(${PROJECT_NAME} ${CATKIN_LIBRARIES} hdf5 hdf5_cpp hdf5_hl_cpp)

cs_install()
cs_export()