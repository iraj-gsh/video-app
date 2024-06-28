cmake_minimum_required(VERSION 3.14)

project(video-app C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/glfw)
add_subdirectory(lib/FFmpeg)

if(APPLE)
    list(APPEND EXTRA_LIBS
         "-framework OpenGL"
    )
elseif(WIN32)
    list(APPEND EXTRA_LIBS 
         "-lglue32 -lopengl32"
    )
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc -static-libstdc++ -mwindows")
else()
    list(APPEND EXTRA_LIBS 
         "-lGL -lGLU -lX11 -lavformat -lavcodec -lavutil -lswscale"
    )
endif()

list(APPEND SOURCES
    src/main.cpp
    src/load_frame.cpp
    src/video_reader.cpp
    src/video_reader.hpp
)

add_executable(video-app ${SOURCES})

target_link_libraries(video-app glfw FFmpeg ${EXTRA_LIBS})

