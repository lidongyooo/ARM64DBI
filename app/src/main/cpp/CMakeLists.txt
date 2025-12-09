
cmake_minimum_required(VERSION 3.22.1)

project("arm64dbidemo")

file(GLOB SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/dbi/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB ASM_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/dbi/*.S"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.S"
)

add_library(${CMAKE_PROJECT_NAME} SHARED
        ${SRC_FILES}
        ${ASM_FILES}
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/dbi
)

set(LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# 编译指令优化
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

enable_language(ASM)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        ${LIBS_PATH}/libcapstone.a)