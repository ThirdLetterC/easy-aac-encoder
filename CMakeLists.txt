cmake_minimum_required(VERSION 3.20)

project(EasyAACEncoder VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EASYAAC_DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${EASYAAC_DIST_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${EASYAAC_DIST_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EASYAAC_DIST_DIR}")

set(EASYAAC_INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/include"
    "${CMAKE_SOURCE_DIR}/src/libfaac"
    "${CMAKE_SOURCE_DIR}/src/libfaac/kiss_fft"
)

set(EASYAAC_OWNED_CPP_SOURCES
    src/easy_aac_encoder_api.cpp
    src/easy_aac_encoder.cpp
    src/decode_to_pcm.cpp
    src/g711_to_pcm.cpp
    src/g726_to_pcm.cpp
    src/pcm_to_aac.cpp
    src/audio_buffer.cpp
    src/dss_buffers.cpp
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

set_source_files_properties(${EASYAAC_VENDORED_C_SOURCES}
    PROPERTIES
        LANGUAGE CXX
)

add_library(EasyAACEncoder_static STATIC ${EASYAAC_ALL_LIBRARY_SOURCES})
add_library(EasyAACEncoder_shared SHARED ${EASYAAC_ALL_LIBRARY_SOURCES})

foreach(target EasyAACEncoder_static EasyAACEncoder_shared)
    target_include_directories(${target}
        PUBLIC
            "${CMAKE_SOURCE_DIR}/include"
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

add_executable(easy_aac_encoder_sample examples/basic_transcoder/main.cpp)
target_include_directories(easy_aac_encoder_sample PRIVATE ${EASYAAC_INCLUDE_DIRS})
target_compile_options(easy_aac_encoder_sample PRIVATE -Wall -Wextra)
target_link_libraries(easy_aac_encoder_sample PRIVATE EasyAACEncoder_static m pthread)

add_custom_target(static DEPENDS EasyAACEncoder_static)
add_custom_target(shared DEPENDS EasyAACEncoder_shared)
add_custom_target(sample DEPENDS easy_aac_encoder_sample)

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
    COMMAND sh -c "rm -f examples/basic_transcoder/*.aac"
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
