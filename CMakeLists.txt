cmake_minimum_required(VERSION 3.20)

project(EasyAACEncoder VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(EASYAAC_DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${EASYAAC_DIST_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${EASYAAC_DIST_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EASYAAC_DIST_DIR}")

set(EASYAAC_INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}/Include"
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/include"
    "${CMAKE_SOURCE_DIR}/src/libfaac"
    "${CMAKE_SOURCE_DIR}/src/libfaac/kiss_fft"
)

set(EASYAAC_OWNED_CPP_SOURCES
    src/EasyAACEncoderAPI.cpp
    src/EasyAACEncoder.cpp
    src/IDecodeToPcm.cpp
    src/G711AToPcm.cpp
    src/G726ToPcm.cpp
    src/PcmToAac.cpp
    src/audio_buffer.cpp
    src/EasyDSSBuffers.cpp
    src/g711.cpp
    src/g726.cpp
)

set(EASYAAC_VENDORED_C_SOURCES
    src/libfaac/aacquant.c
    src/libfaac/backpred.c
    src/libfaac/bitstream.c
    src/libfaac/channels.c
    src/libfaac/fft.c
    src/libfaac/filtbank.c
    src/libfaac/frame.c
    src/libfaac/huffman.c
    src/libfaac/ltp.c
    src/libfaac/midside.c
    src/libfaac/psychkni.c
    src/libfaac/tns.c
    src/libfaac/util.c
    src/libfaac/kiss_fft/kiss_fft.c
    src/libfaac/kiss_fft/kiss_fftr.c
)

set(EASYAAC_ALL_LIBRARY_SOURCES
    ${EASYAAC_OWNED_CPP_SOURCES}
    ${EASYAAC_VENDORED_C_SOURCES}
)

add_library(EasyAACEncoder_static STATIC ${EASYAAC_ALL_LIBRARY_SOURCES})
add_library(EasyAACEncoder_shared SHARED ${EASYAAC_ALL_LIBRARY_SOURCES})

foreach(target EasyAACEncoder_static EasyAACEncoder_shared)
    target_include_directories(${target}
        PUBLIC
            "${CMAKE_SOURCE_DIR}/Include"
        PRIVATE
            ${EASYAAC_INCLUDE_DIRS}
    )
    target_compile_options(${target} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra>
    )
    target_link_libraries(${target} PRIVATE m pthread)
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME EasyAACEncoder
        POSITION_INDEPENDENT_CODE ON
    )
endforeach()

add_executable(testEasyAACEncoder testEasyAACEncoder/main.cpp)
target_include_directories(testEasyAACEncoder PRIVATE ${EASYAAC_INCLUDE_DIRS})
target_compile_options(testEasyAACEncoder PRIVATE -Wall -Wextra)
target_link_libraries(testEasyAACEncoder PRIVATE EasyAACEncoder_static m pthread)

add_custom_target(static DEPENDS EasyAACEncoder_static)
add_custom_target(shared DEPENDS EasyAACEncoder_shared)
add_custom_target(sample DEPENDS testEasyAACEncoder)

add_library(EasyAACEncoder_warnings OBJECT ${EASYAAC_OWNED_CPP_SOURCES})
target_include_directories(EasyAACEncoder_warnings PRIVATE ${EASYAAC_INCLUDE_DIRS})
target_compile_options(EasyAACEncoder_warnings PRIVATE -Wall -Wextra -Werror)
set_target_properties(EasyAACEncoder_warnings PROPERTIES
    EXCLUDE_FROM_ALL ON
    POSITION_INDEPENDENT_CODE ON
)

add_custom_target(warnings
    DEPENDS EasyAACEncoder_warnings
    COMMENT "Compiling project-owned C++23 sources with -Wall -Wextra -Werror"
)

add_custom_target(clean-generated
    COMMAND sh -c "rm -f src/testEasyAACEncoder/*.aac testEasyAACEncoder/*.aac"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Removing generated encoder outputs"
)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
    add_custom_target(fmt
        COMMAND sh -c "git ls-files '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hpp' '*.hh' | xargs -r '${CLANG_FORMAT_EXE}' -i"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Formatting tracked C/C++ source files with clang-format"
    )
endif()
