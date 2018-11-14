cmake_minimum_required(VERSION 3.2)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 0)
set(PATCH_VERSION 999)
if($ENV{MAJOR_VERSION})
    set(MAJOR_VERSION $ENV{MAJOR_VERSION})
endif()
if($ENV{MINOR_VERSION})
    set(MINOR_VERSION $ENV{MINOR_VERSION})
endif()
if($ENV{TRAVIS_BUILD_NUMBER})
    set(PATCH_VERSION $ENV{TRAVIS_BUILD_NUMBER})
endif()
project(allocator VERSION ${MAJOR_VERSION}.${MAJOR_VERSION}.${PATCH_VERSION})
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
add_executable(allocator main.cpp)
add_library(alloc lib.cpp)
add_executable(test_version test_version.cpp)
add_executable(test_container test_container.cpp)
set_target_properties(allocator alloc test_version test_container PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-g;-Wpedantic;-Wall;-Wextra"
)
set_target_properties(test_version PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)
set_target_properties(test_container PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)
target_link_libraries(allocator
    alloc
)
target_link_libraries(test_version
    ${Boost_LIBRARIES}
    alloc
)
target_link_libraries(test_container
    ${Boost_LIBRARIES}
)
install(TARGETS allocator RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT podshivalov.ilya@yandex.ru)
include(CPack)
enable_testing()
add_test(allocator_tests test_version)
add_test(allocator_tests test_container)
