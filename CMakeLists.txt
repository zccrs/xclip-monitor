cmake_minimum_required(VERSION 3.10)
project(xclip-monitor
    VERSION 1.0.0
    DESCRIPTION "X11剪切板监控工具"
    LANGUAGES C)

# 设置C标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# 如果没有指定构建类型，默认为Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 使用pkg-config查找所需的库
find_package(PkgConfig REQUIRED)

# 查找X11库
pkg_check_modules(X11 REQUIRED x11)

# 必需：查找XRes扩展库
pkg_check_modules(XRES REQUIRED xres)

# 检查是否有XResQueryClientIds函数支持
include(CheckFunctionExists)
set(CMAKE_REQUIRED_LIBRARIES ${XRES_LIBRARIES})
set(CMAKE_REQUIRED_INCLUDES ${XRES_INCLUDE_DIRS})
check_function_exists(XResQueryClientIds HAVE_XRES_QUERY_CLIENT_IDS)

# 定义源文件
set(SOURCES xclip_monitor.c)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 设置编译定义
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _GNU_SOURCE
)

# XRes库是必需的，直接链接
target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_XRES)
target_link_libraries(${PROJECT_NAME} ${XRES_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${XRES_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PRIVATE ${XRES_CFLAGS_OTHER})
message(STATUS "XRes扩展支持已启用")

# 链接必需的库
target_link_libraries(${PROJECT_NAME}
    ${X11_LIBRARIES}
)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${X11_INCLUDE_DIRS}
)

# 设置编译选项
target_compile_options(${PROJECT_NAME} PRIVATE
    ${X11_CFLAGS_OTHER}
)

# 打印库信息
message(STATUS "X11库路径: ${X11_LIBRARIES}")
message(STATUS "X11包含目录: ${X11_INCLUDE_DIRS}")
message(STATUS "XRes库路径: ${XRES_LIBRARIES}")

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    COMPONENT Runtime
)

# 创建卸载目标
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

# 显示构建信息
message(STATUS "")
message(STATUS "=== 构建配置信息 ===")
message(STATUS "项目名称: ${PROJECT_NAME}")
message(STATUS "项目版本: ${PROJECT_VERSION}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "C编译器: ${CMAKE_C_COMPILER}")
message(STATUS "安装前缀: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "===================")
message(STATUS "")
