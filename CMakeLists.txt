cmake_minimum_required(VERSION 3.15)

project(ProjecCpp)
set(SOURCES main.cpp)

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(IMGUI_DIR "${LIB_DIR}/imgui")

set(GLM_LINK_FLAGS "-lm")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GLM_LINK_FLAGS}")

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")


########################################################
# GLAD library
#https://stackoverflow.com/questions/68014365/how-can-i-link-glfw-and-dear-imgui-in-cmake
########################################################
set(GLAD_DIR "${LIB_DIR}/glad")
add_library(glad "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PUBLIC "${LIB_DIR}/glad/include")

########################################################
# Dear IMGUI library
########################################################
add_library(imgui
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMGUI_DIR}/
)
########################################################
# Creating a Common Libraries Interface
########################################################
add_library(common_libs INTERFACE)

target_link_libraries(common_libs INTERFACE
    imgui
    glad
    glfw
    glm::glm
    SDL2::SDL2
)

target_include_directories(common_libs INTERFACE
    ${INC_DIR}
    ${SRC_DIR}
)

########################################################
# Create the main executable
########################################################
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE 
    imgui
    glad
    glfw
    glm::glm
    SDL2::SDL2 
    ${CMAKE_DL_LIBS}
)


########################################################
# Samples for testing purposes
########################################################
set("SAMPLES_DIR" "${CMAKE_CURRENT_SOURCE_DIR}/samples")

add_executable(imgui_sample "${SAMPLES_DIR}/imgui_sample.cpp")
target_link_libraries(imgui_sample PRIVATE 
    common_libs
)


add_executable(CorpSimulation "${SAMPLES_DIR}/CorpSimulation.cpp" )
target_link_libraries(CorpSimulation PRIVATE 
    common_libs
)