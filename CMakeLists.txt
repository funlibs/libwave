cmake_minimum_required (VERSION 2.6)
project (CWAVE)

include_directories (.)

add_library (cwave cwave.c cwave.h)

add_executable (cwave_test
    cwave_test.c
    cwave.c
    cwave.h)

target_link_libraries (cwave_test cwave)

# tests
enable_testing()
add_test(
    NAME    OK_2Channels
    COMMAND cwave_test ${CMAKE_CURRENT_SOURCE_DIR}/2_channels_PCM.wav success)
add_test(
    NAME    Fail_6Channels 
    COMMAND cwave_test ${CMAKE_CURRENT_SOURCE_DIR}/6_channels_PCM.wav fail)

# doc
find_package (Doxygen)

if (DOXYGEN_FOUND)

  configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target (cwave_doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)

endif (DOXYGEN_FOUND)

