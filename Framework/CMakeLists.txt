cmake_minimum_required(VERSION 3.5)
project(Framework VERSION 1.0.0 LANGUAGES CXX)

file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

file(GLOB IMGUI
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h)

add_library(Framework
    ${SOURCES}
    ${IMGUI}
)

if (WIN32)
    target_link_libraries(Framework
        PUBLIC
            "${ASSIMP_PATH}/lib/x64/assimp-vc140-mt.lib"
            "${SDL2_PATH}/lib/x64/SDL2.lib"
            "${SDL2_PATH}/lib/x64/SDL2main.lib"
            "${GLEW_PATH}/lib/Release/x64/glew32s.lib"
    )
endif()

target_include_directories(Framework
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        "${GLM_PATH}"
        "${ASSIMP_PATH}/include"
        "${STB_PATH}"
        "${SDL2_PATH}/include"
        "${GLEW_PATH}/include"
)

target_compile_features(Framework 
    PUBLIC 
        cxx_std_17
)

if(MSVC)
  target_compile_options(Framework PRIVATE /W3 /WX /MP)
else()
  target_compile_options(Framework PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
