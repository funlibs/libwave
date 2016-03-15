cmake_minimum_required (VERSION 2.6)
project (LIBWAVE)

add_definitions (-D_CRT_SECURE_NO_WARNINGS)

include_directories (.)

add_library (wave wave.c wave.h)

add_executable (wave_test
    wave_test.c
    wave.c
    wave.h)

target_link_libraries (wave_test wave)

# tests
enable_testing()
add_test(
    NAME    OK_2Channels
    COMMAND wave_test ${CMAKE_CURRENT_SOURCE_DIR}/2_channels_PCM.wav success)
add_test(
    NAME    Fail_6Channels 
    COMMAND wave_test ${CMAKE_CURRENT_SOURCE_DIR}/6_channels_PCM.wav fail)

# doc
find_package (Doxygen)

if (DOXYGEN_FOUND)

  configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target (wave_doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)

endif (DOXYGEN_FOUND)

