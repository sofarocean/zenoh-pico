#
# Copyright (c) 2022 ZettaScale Technology
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
# which is available at https://www.apache.org/licenses/LICENSE-2.0.
#
# SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
#
# Contributors:
#   ZettaScale Zenoh Team, <zenoh@zettascale.tech>
#
cmake_minimum_required(VERSION 3.8)

project(libzenohpico VERSION 0.6.0 LANGUAGES C)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# while in development, use timestamp for patch version:
string(TIMESTAMP PROJECT_VERSION_PATCH "%Y%m%ddev")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# Use cmake .. -DCMAKE_BUILD_TYPE=DEBUG for debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE)
endif()
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
message(STATUS "Building in ${CMAKE_BUILD_TYPE} mode")

option(BUILD_SHARED_LIBS "Build shared libraries if ON, otherwise build static libraries" ON)
message(STATUS "Build shared library: ${BUILD_SHARED_LIBS}")

option (PACKAGING "Use option on Linux to produce Debian and RPM packages." OFF)
message(STATUS "Produce Debian and RPM packages: ${PACKAGING}")

option (BUILD_EXAMPLES "Use this to also build the examples." ON)
message(STATUS "Build examples: ${BUILD_EXAMPLES}")

option (BUILD_TESTING "Use this to also build tests." ON)
message(STATUS "Build tests: ${BUILD_TESTING}")

option (BUILD_INTEGRATION "Use this to also build integration tests." OFF)
message(STATUS "Build integration: ${BUILD_INTEGRATION}")

option (ZENOH_DEBUG "Use this to set the ZENOH_DEBUG variable." 0)
message(STATUS "Zenoh Level Log: ${ZENOH_DEBUG}")

message(STATUS "Configuring for ${CMAKE_SYSTEM_NAME}")
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_definitions(-DZENOH_LINUX)
  set(JNI_PLATFORM_NAME "linux")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions(-DZENOH_MACOS)
  set(MACOSX_RPATH "ON")
  set(JNI_PLATFORM_NAME "darwin")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  message(FATAL_ERROR "zenoh-pico is not yet available on Windows platform")
  add_definitions(-DZENOH_WIN)
  set(JNI_PLATFORM_NAME "win32")
endif()

add_definitions(-DZENOH_DEBUG=${ZENOH_DEBUG})

if (SKBUILD)
  set(INSTALL_RPATH "zenoh")
  set(INSTALL_NAME_DIR "zenoh")
  set(INSTALL_INCLUDE_NAME_DIR "zenoh/include")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
  add_compile_options(-c -Wall -Wextra -Werror -Wunused -Wstrict-prototypes -pipe -g -O0)
elseif (CMAKE_BUILD_TYPE MATCHES "RELEASE")
  add_compile_options(-pipe -O3)
endif()

set(Libname "zenohpico")

file(GLOB PublicHeaders "include/*.h"
                        "include/zenoh-pico/*.h"
                        "include/zenoh-pico/api/*.h"
                        "include/zenoh-pico/collections/*.h"
                        "include/zenoh-pico/link/*.h"
                        "include/zenoh-pico/link/config/*.h"
                        "include/zenoh-pico/protocol/*.h"
                        "include/zenoh-pico/session/*.h"
                        "include/zenoh-pico/system/*.h"
                        "include/zenoh-pico/system/link/*.h"
                        "include/zenoh-pico/transport/*.h"
                        "include/zenoh-pico/utils/*.h"
)
include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

if (UNIX)
  file(GLOB Sources "src/*.c"
                    "src/api/*.c"
                    "src/collections/*.c"
                    "src/link/*.c"
                    "src/link/config/*.c"
                    "src/link/unicast/*.c"
                    "src/link/multicast/*.c"
                    "src/protocol/*.c"
                    "src/session/*.c"
                    "src/system/*.c"
                    "src/system/unix/*.c"
                    "src/transport/*.c"
                    "src/transport/common/*.c"
                    "src/transport/unicast/*.c"
                    "src/transport/unicast/link/*.c"
                    "src/transport/unicast/link/task/*.c"
                    "src/transport/multicast/*.c"
                    "src/transport/multicast/link/*.c"
                    "src/transport/multicast/link/task/*.c"
  )
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
link_directories(${LIBRARY_OUTPUT_PATH})

add_library(${Libname} ${Sources})

target_link_libraries(${Libname} Threads::Threads)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_link_libraries(${Libname} rt)
endif()

install(TARGETS ${Libname}
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib
          COMPONENT Library
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/zenoh-pico.h
          DESTINATION include
          COMPONENT Headers
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/zenoh-pico
          DESTINATION include
          COMPONENT Headers
)

if (BUILD_EXAMPLES)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)

  add_executable(zn_write ${PROJECT_SOURCE_DIR}/examples/net/zn_write.c)
  add_executable(zn_pub ${PROJECT_SOURCE_DIR}/examples/net/zn_pub.c)
  add_executable(zn_sub ${PROJECT_SOURCE_DIR}/examples/net/zn_sub.c)
  add_executable(zn_peer_sub ${PROJECT_SOURCE_DIR}/examples/net/zn_peer_sub.c)
  add_executable(zn_peer_pub ${PROJECT_SOURCE_DIR}/examples/net/zn_peer_pub.c)
  add_executable(zn_pull ${PROJECT_SOURCE_DIR}/examples/net/zn_pull.c)
  add_executable(zn_query ${PROJECT_SOURCE_DIR}/examples/net/zn_query.c)
  add_executable(zn_eval ${PROJECT_SOURCE_DIR}/examples/net/zn_eval.c)
  add_executable(zn_info ${PROJECT_SOURCE_DIR}/examples/net/zn_info.c)
  add_executable(zn_pub_thr ${PROJECT_SOURCE_DIR}/examples/net/zn_pub_thr.c)
  add_executable(zn_sub_thr ${PROJECT_SOURCE_DIR}/examples/net/zn_sub_thr.c)
  add_executable(zn_scout ${PROJECT_SOURCE_DIR}/examples/net/zn_scout.c)

  target_link_libraries(zn_write ${Libname})
  target_link_libraries(zn_pub ${Libname})
  target_link_libraries(zn_sub ${Libname})
  target_link_libraries(zn_peer_sub ${Libname})
  target_link_libraries(zn_peer_pub ${Libname})
  target_link_libraries(zn_pull ${Libname})
  target_link_libraries(zn_query ${Libname})
  target_link_libraries(zn_eval ${Libname})
  target_link_libraries(zn_info ${Libname})
  target_link_libraries(zn_pub_thr ${Libname})
  target_link_libraries(zn_sub_thr ${Libname})
  target_link_libraries(zn_scout ${Libname})
endif()

if(BUILD_TESTING)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")

  add_executable(z_data_struct_test ${PROJECT_SOURCE_DIR}/tests/z_data_struct_test.c)
  add_executable(z_endpoint_test ${PROJECT_SOURCE_DIR}/tests/z_endpoint_test.c)
  add_executable(z_iobuf_test ${PROJECT_SOURCE_DIR}/tests/z_iobuf_test.c)  
  add_executable(zn_msgcodec_test ${PROJECT_SOURCE_DIR}/tests/zn_msgcodec_test.c)
  add_executable(z_mvar_test ${PROJECT_SOURCE_DIR}/tests/z_mvar_test.c)  
  add_executable(zn_rname_test ${PROJECT_SOURCE_DIR}/tests/zn_rname_test.c)
  
  target_link_libraries(z_data_struct_test ${Libname})
  target_link_libraries(z_endpoint_test ${Libname})
  target_link_libraries(z_iobuf_test ${Libname})
  target_link_libraries(zn_msgcodec_test ${Libname})
  target_link_libraries(z_mvar_test ${Libname})
  target_link_libraries(zn_rname_test ${Libname})  

  enable_testing()
  add_test(z_data_struct_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_data_struct_test)
  add_test(z_endpoint_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_endpoint_test)
  add_test(z_iobuf_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_iobuf_test)    
  add_test(zn_msgcodec_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zn_msgcodec_test)
  add_test(zn_rname_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zn_rname_test)
endif()

if(BUILD_MULTICAST)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")

  add_executable(zn_peer_multicast_test ${PROJECT_SOURCE_DIR}/tests/zn_peer_multicast_test.c)

  target_link_libraries(zn_peer_multicast_test ${Libname})

  configure_file(${PROJECT_SOURCE_DIR}/tests/multicast.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/multicast.sh COPYONLY)

  enable_testing()
  add_test(zn_client_test bash ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/multicast.sh zn_peer_multicast_test)
endif()

if(BUILD_INTEGRATION)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")

  add_executable(zn_client_test ${PROJECT_SOURCE_DIR}/tests/zn_client_test.c)

  target_link_libraries(zn_client_test ${Libname})

  configure_file(${PROJECT_SOURCE_DIR}/tests/routed.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/routed.sh COPYONLY)

  enable_testing()
  add_test(zn_client_test bash ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/routed.sh zn_client_test)
endif()

# For packaging
if (PACKAGING)
  set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")

  set(CPACK_COMPONENTS_ALL Library Headers)
  set(CPACK_COMPONENT_LIBRARY_GROUP "lib")
  set(CPACK_COMPONENT_HEADERS_GROUP "dev")
  set(CPACK_COMPONENT_HEADERS_DEPENDS Library)

  set(CPACK_PACKAGE_CHECKSUM MD5)
  set(CPACK_PACKAGE_VENDOR "The Eclipse Foundation")
  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
  set(CPACK_COMPONENT_LIB_DESCRIPTION "The C client library for Eclipse zenoh targeting pico devices")
  set(CPACK_COMPONENT_DEV_DESCRIPTION "${CPACK_COMPONENT_LIB_DESCRIPTION} - devel files")

  # Sources package
  set(CPACK_SOURCE_GENERATOR "TGZ")
  set(CPACK_SOURCE_IGNORE_FILES "/.git/;/.github/;/build/;/crossbuilds/")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-src-${PROJECT_VERSION}")

  if(PACKAGING MATCHES "DEB")
    if(NOT DEBARCH)
        set(DEBARCH ${CMAKE_SYSTEM_PROCESSOR})
    endif()

    message(STATUS "Configure DEB packaging for Linux ${DEBARCH}")
    if(CPACK_GENERATOR)
        set(CPACK_GENERATOR "${CPACK_GENERATOR};DEB")
    else()
        set(CPACK_GENERATOR "DEB")
    endif()

    # DEB package
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "ADLINK zenoh team, <zenoh@adlink-labs.tech>")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${DEBARCH})
    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_LIB_PACKAGE_NAME ${PROJECT_NAME})   # avoid "-lib" suffix for "lib" package
    set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libc6 (>=2.12)")
    set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (=${PROJECT_VERSION})")
  endif()

  if(PACKAGING MATCHES "RPM")
    if(NOT RPMARCH)
        set(RPMARCH ${CMAKE_SYSTEM_PROCESSOR})
    endif()

    message(STATUS "Configure RPM packaging for Linux ${RPMARCH}")
    if(CPACK_GENERATOR)
        set(CPACK_GENERATOR "${CPACK_GENERATOR};RPM")
    else()
        set(CPACK_GENERATOR "RPM")
    endif()

    # RPM package
    set(CPACK_RPM_PACKAGE_ARCHITECTURE ${RPMARCH})
    set(CPACK_RPM_COMPONENT_INSTALL ON)
    set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
    set(CPACK_RPM_LIB_PACKAGE_NAME ${PROJECT_NAME})   # avoid "-lib" suffix for "lib" package
    set(CPACK_RPM_DEV_PACKAGE_REQUIRES "${CPACK_RPM_LIB_PACKAGE_NAME} = ${PROJECT_VERSION}")
  endif()

  include(CPack)
endif()
