cmake_minimum_required(VERSION 3.10)
project(matrix VERSION 0.1 LANGUAGES C)

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
set(SOURCES
		src/matrix.c
		src/config.h

		src/utils/files.c
		src/utils/files.h
		src/utils/list.c
		src/utils/list.h
		src/utils/strings.c
		src/utils/strings.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

if (MPI_C_FOUND)
    include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})
    target_link_libraries(${PROJECT_NAME} PUBLIC MPI::MPI_C)
endif()

if(OpenMP_C_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_C)
endif()

message(STATUS "MPI Include Path: ${MPI_C_INCLUDE_PATH}")
message(STATUS "MPI Libraries: ${MPI_C_LIBRARIES}")
message(STATUS "OpenMP Found: ${OpenMP_C_FOUND}")
