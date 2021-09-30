cmake_minimum_required(VERSION 3.0.0)
set(LOCAL_PROJECT_NAME emiu)
project(LOCAL_PROJECT_NAME VERSION 0.1.0 LANGUAGES CXX)

include_directories(./include
                    ./SDL2-Windows/include)

add_executable(${LOCAL_PROJECT_NAME} 
               src/Controller.cpp
               src/Controller.cpp
               src/CPU.cpp
               src/Display.cpp
               src/DMA.cpp
               src/Flash.cpp
               src/Interrupt.cpp
               src/main.cpp
               src/MMU.cpp
               src/Timer.cpp
)

set_property(TARGET ${LOCAL_PROJECT_NAME} PROPERTY CXX_STANDARD 14)

if (WIN32)
    target_link_libraries(${LOCAL_PROJECT_NAME} libSDL2)
    target_link_libraries(${LOCAL_PROJECT_NAME} winmm)
    target_link_directories(${LOCAL_PROJECT_NAME} PUBLIC "./SDL2-Windows/lib/") 
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/SDL2-Windows/bin/SDL2.dll
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

else ()
    find_package(SDL2 REQUIRED)
    target_link_libraries(${LOCAL_PROJECT_NAME} PRIVATE SDL2)
ENDIF()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/miuchiz-bins/OTP.dat
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/miuchiz-bins/HexEditor.dat
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/HexEditor.dat
            ${CMAKE_CURRENT_BINARY_DIR}/Flash.dat)        

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
