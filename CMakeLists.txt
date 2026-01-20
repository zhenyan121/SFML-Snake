cmake_minimum_required(VERSION 3.14)
project(SFMLSnake)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 使用 FetchContent 引入 SFML ---
include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.2 # 你可以指定标签(Tag)或分支(Branch)
)

# 确保 SFML 不会安装到系统文件夹中，仅在构建目录使用
set(SFML_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

# --- 目标文件配置 ---
# 递归获取 src 目录下所有源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

# 链接 SFML 组件 (常用: graphics, window, system, audio, network)
# 注意：在 SFML 2.x 中，目标名称带有 sfml- 前缀
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# 设置运行时的输出目录（可选）
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)