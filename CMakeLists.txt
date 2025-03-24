cmake_minimum_required(VERSION 3.10)

project(AlgorithmDXZ VERSION 1.0)

# 设置 C++ 标准版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#find_package(gperftools REQUIRED)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/opt/homebrew/Cellar/gperftools/2.15/include/gperftools)

# 添加库目录
link_directories(/opt/homebrew/Cellar/gperftools/2.15/lib)

# Add source files
set(MAIN_SOURCES 
    src/main.cpp
    src/DLXZ.cpp
    )

set(TEST_SOURCES
    test/test.cpp
    src/DLXZ.cpp
)
#file(GLOB TEST_SOURCES "test/testMatrix.cpp")

# Add executable target
#add_executable(AlgorithmDXZ ${MAIN_SOURCES})
add_executable(main ${MAIN_SOURCES})
# add_executable(test ${TEST_SOURCES})

#target_link_libraries(AlgorithmDXZ tcmalloc)