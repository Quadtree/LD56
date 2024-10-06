cmake_minimum_required(VERSION 3.9)
project(ld56)

file(COPY "${CMAKE_SOURCE_DIR}/web/" DESTINATION "${CMAKE_BINARY_DIR}/")
file(COPY "${CMAKE_SOURCE_DIR}/web/" DESTINATION "${CMAKE_BINARY_DIR}/web/")
file(COPY "${CMAKE_SOURCE_DIR}/web/" DESTINATION "${OUTPUT_DIR}/")
file(COPY "${CMAKE_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_BINARY_DIR}/assets/")

file(GLOB_RECURSE src_files "src/*.cpp")
add_executable(ld56 ${src_files})

if("${DEBUG_MODE}" EQUAL "1")
    target_compile_options(ld56 PRIVATE -sDISABLE_EXCEPTION_CATCHING=0 -gsource-map -O0 -D _DEBUG=1)
    target_link_options(ld56 PRIVATE -sDISABLE_EXCEPTION_CATCHING=0 -gsource-map -sDEMANGLE_SUPPORT=1 -O0 -D _DEBUG=1)
    set(GIT_HASH debug)
    message("In debug mode")
else()
    target_compile_options(ld56 PRIVATE -O3)
    target_link_options(ld56 PRIVATE -O3)
    execute_process(COMMAND git config --global --add safe.directory "${CMAKE_SOURCE_DIR}")
    execute_process(WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND git rev-parse HEAD COMMAND head -c6 OUTPUT_VARIABLE GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
    message("Not in debug mode")
endif()

message("GIT_HASH=${GIT_HASH}")

if(NOT "${GIT_HASH}" MATCHES "^[a-z0-9]+")
  message(FATAL_ERROR "Unable to derive GIT_HASH")
endif()

target_compile_options(ld56 PRIVATE -std=c++1z -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sSDL2_IMAGE_FORMATS=[\"XCF\"] "-D_GIT_HASH=\"${GIT_HASH}\"" --no-heap-copy -Wno-unqualified-std-cast-call -pthread)

target_link_options(ld56 PRIVATE
  -std=c++1z -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2
  --embed-file assets -sSDL2_IMAGE_FORMATS=[\"XCF\"]
  --bind --no-heap-copy -Wl,-u,fileno
  -pthread -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency
)

set_target_properties(ld56 PROPERTIES OUTPUT_NAME "${OUTPUT_DIR}/index")
set_target_properties(ld56 PROPERTIES SUFFIX ".html")

set(CMAKE_VERBOSE_MAKEFILE ON)

file(MAKE_DIRECTORY "${OUTPUT_DIR}")
