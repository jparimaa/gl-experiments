cmake_minimum_required(VERSION 3.5)
project(gl-experiments VERSION 0.0.1 LANGUAGES CXX)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "64bit is required")
endif()

set(SDL2_PATH CACHE PATH "Path to SDL2")
set(ASSIMP_PATH CACHE PATH "Path to assimp")
set(GLEW_PATH CACHE PATH "Path to GLEW")

set(STB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/External/stb/")
set(GLM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/External/glm/")

add_definitions(
    -DASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Assets/"
    -DROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/"
)

function(ADD_PROJECT_WITH_DEFAULT_SETTINGS PROJECT_NAME)
    file(GLOB SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

    add_executable(${PROJECT_NAME} ${SOURCES})

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            Framework
    )

    target_include_directories(${PROJECT_NAME}
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Framework/include/"
    )

    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX /MP)
        target_compile_definitions(${PROJECT_NAME} PUBLIC -D_CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()

    if(WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${ASSIMP_PATH}/bin/x64/assimp-vc140-mt.dll"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL2_PATH}/lib/x64/SDL2.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLEW_PATH}/bin/Release/x64/glew32.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endif()
endfunction()

add_subdirectory(Framework)
add_subdirectory(Experiments/Example)
add_subdirectory(Experiments/BindlessTexture)
add_subdirectory(Experiments/DSA)
add_subdirectory(Experiments/GeometryShader)
add_subdirectory(Experiments/ImageLoadStore)
add_subdirectory(Experiments/IndirectDrawing)
add_subdirectory(Experiments/IndirectDrawingSimple)
add_subdirectory(Experiments/Instancing)
add_subdirectory(Experiments/MultiDraw)
add_subdirectory(Experiments/Tessellation)
add_subdirectory(Experiments/TextureArray)
add_subdirectory(Experiments/TextureProjection)
add_subdirectory(Experiments/TransformFeedback)
add_subdirectory(Experiments/VertexPulling)
