cmake_minimum_required(VERSION 3.1.0)

project(mcdc VERSION 1.0.0 LANGUAGES CXX)

if(${CMAKE_SYSTEM_NAME} MATCHES Linux AND ${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
  message(STATUS "Let's Start ")
else()
  message(FATAL_ERROR "This script is for Linux and GNU Compiler")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(Threads REQUIRED)

file(GLOB_RECURSE PROD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/mcdc/Source/*.cpp)

add_executable(${PROJECT_NAME} ${PROD_SRC})

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mcdc/Include
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE Threads::Threads
)

install(
  TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

