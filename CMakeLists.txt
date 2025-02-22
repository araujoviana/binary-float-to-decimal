cmake_minimum_required(VERSION 3.10)
project(BinaryFloatToDecimal)

find_package(Doxygen REQUIRED)

if (DOXYGEN_FOUND)
    set(DOXYFILE_IN ${CMAKE_SOURCE_DIR}/Doxyfile.in)
    set(DOXYFILE_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc/doxygen)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        VERBATIM)
else()
    message(STATUS "Doxygen not found!")
endif()

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto")

add_executable(BinaryFloatToDecimal src/main.c)
target_link_libraries(BinaryFloatToDecimal m)
