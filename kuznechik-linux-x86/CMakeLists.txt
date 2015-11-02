cmake_minimum_required (VERSION 2.6)
project (Kuznechik)

enable_language(CXX)
enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/nasm/preproc-rewrite/nasm")

SET(CMAKE_ASM_NASM_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB H_FILES "*.h")

set(MAIN_C_FILES main.cpp galois.cpp kuznechik.cpp)
file(GLOB MAIN_ASM_FILES "*.asm")
file(GLOB MAIN_ASM_INCLUDE_FILES "*.inc")

set(GEN_C_FILES tables_generator.cpp galois.cpp kuznechik.cpp)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -Wno-unused-variable -DNDEBUG -funroll-loops -m64 -flto")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -dead_strip")

IF(APPLE)
set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f macho64 -O0")
ELSE(NOT APPLE)
set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -O0")
ENDIF(APPLE)

set(XTEST_FILES ${MAIN_C_FILES} ${MAIN_ASM_FILES} ${MAIN_ASM_INCLUDE_FILES} ${H_FILES})

foreach(BITS 8 16 32 64)
add_executable(xtest_${BITS}_galois ${XTEST_FILES})
add_executable(xtest_${BITS}_mul ${XTEST_FILES})
add_executable(xtest_${BITS}_tables ${XTEST_FILES})

set_target_properties(xtest_${BITS}_galois PROPERTIES COMPILE_FLAGS "-DBITS=${BITS} -DUSE_ASM=1 -DUSE_GALOIS=1")
set_target_properties(xtest_${BITS}_mul PROPERTIES COMPILE_FLAGS "-DBITS=${BITS} -DUSE_ASM=1 -DUSE_MUL_TABLE=1")
set_target_properties(xtest_${BITS}_tables PROPERTIES COMPILE_FLAGS "-DBITS=${BITS} -DUSE_ASM=1 -DUSE_TABLES=1")

set_target_properties(xtest_${BITS}_galois PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(xtest_${BITS}_mul PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(xtest_${BITS}_tables PROPERTIES LINKER_LANGUAGE CXX)
endforeach(BITS)

add_executable(tables_generator ${GEN_C_FILES} ${H_FILES})
set_target_properties(tables_generator PROPERTIES COMPILE_FLAGS "-DBITS=64 -DUSE_ASM=0 -DUSE_GALOIS=1 -DUSE_MUL_TABLE=1 -DUSE_TABLES=1")

set_target_properties(tables_generator PROPERTIES LINKER_LANGUAGE CXX)

set(DEST ${CMAKE_CURRENT_SOURCE_DIR}/bin)

foreach(BITS 8 16 32 64)
foreach(MODE galois mul tables)
install(TARGETS xtest_${BITS}_${MODE} DESTINATION ${DEST})
endforeach(MODE)
endforeach(BITS)
install(TARGETS tables_generator DESTINATION ${DEST})
