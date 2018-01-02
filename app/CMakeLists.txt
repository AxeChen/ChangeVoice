# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

find_library( log-lib
              log )

set(my_lib_path ${CMAKE_SOURCE_DIR}/libs)

# 添加三方的so库
add_library(libfmod
            SHARED
            IMPORTED )
# 指名第三方库的绝对路径
set_target_properties( libfmod
                       PROPERTIES IMPORTED_LOCATION
                       ${my_lib_path}/${ANDROID_ABI}/libfmod.so )

add_library(libfmodL
            SHARED
            IMPORTED )

set_target_properties( libfmodL
                       PROPERTIES IMPORTED_LOCATION
                       ${my_lib_path}/${ANDROID_ABI}/libfmodL.so )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# 添加我们需要写代码的路径
add_library(changeVoice
            SHARED
            src/main/cpp/changeVoice.cpp )

# 导入路径，为了让编译时能够寻找到这个文件夹
include_directories(src/main/cpp/inc)

# 链接好三个路径
target_link_libraries( changeVoice
                       libfmod
                       libfmodL

                       ${log-lib} )

