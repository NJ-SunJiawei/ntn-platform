cmake_minimum_required(VERSION 3.10)

project(DemoCppControl)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt5（如果需要Qt）- 现在需要同时查找Widgets和Network模块
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

# 启用AUTOUIC、AUTOMOC和AUTORCC以自动处理.ui文件、moc文件和资源文件
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_AUTOMOC ON)  # 启用自动MOC处理
set(CMAKE_AUTORCC ON)  # 如果有资源文件也需要启用

# 定义源文件
set(SOURCES
    main.cpp
    mainwindow.cpp
    CustomPdu.cpp
)

set(HEADERS
    mainwindow.h
    CustomPdu.h
)

# 定义UI文件
set(UIS mainwindow.ui)

# 创建可执行文件
add_executable(demo-cppcontrol
    ${SOURCES}
    ${HEADERS}
    ${UIS}
)

# 直接链接OpenDIS6库，跳过find_package
target_link_libraries(demo-cppcontrol 
    /usr/local/lib/libOpenDIS6.so  # 直接指定库文件路径
    Qt5::Widgets
    Qt5::Network
)

# 添加OpenDIS头文件目录
target_include_directories(demo-cppcontrol PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    /usr/local/include
)

# 添加Qt Network头文件目录
target_include_directories(demo-cppcontrol PRIVATE
    ${Qt5Network_INCLUDE_DIRS}
)

# 添加OpenDIS库目录
target_link_directories(demo-cppcontrol PRIVATE
    /usr/local/lib
)

# 如果需要特定的编译选项
set_target_properties(demo-cppcontrol PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

message(STATUS "Using OpenDIS6 from /usr/local")