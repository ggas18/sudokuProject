cmake_minimum_required(VERSION 2.8.12.2)

project (sudoku )

set(PROJECT_VERSION "3.3.4")


#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -#Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept    -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -#Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused")

file(GLOB SOURCES "src/*.c")

# For posix threads
find_package (Threads)

add_executable(sudoku ${SOURCES})

target_link_libraries (sudoku ${CMAKE_THREAD_LIBS_INIT})
