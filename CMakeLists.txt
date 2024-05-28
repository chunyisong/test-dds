
cmake_minimum_required(VERSION 3.16.3)

project(test-dds VERSION 1.0 ${VerifyCXX})

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_VERBOSE_MAKEFILE ON)


set(CMAKE_DEBUG_POSTFIX d)

message(STATUS "****CMAKE_PREFIX_PATH:${CMAKE_PREFIX_PATH},CMAKE_CURRENT_SOURCE_DIR:${CMAKE_CURRENT_SOURCE_DIR}")

# Find requirements
if(NOT fastrtps_FOUND)
    find_package(foonathan_memory REQUIRED)
    find_package(fastcdr REQUIRED)
    get_target_property(FastrptsStatic fastcdr TYPE)
    if(FastrptsStatic STREQUAL "STATIC_LIBRARY")
        if(NOT MSVC)
            set(OPENSSL_USE_STATIC_LIBS TRUE)
        endif()
        message(STATUS "****FastrptsStatic:${FastrptsStatic}, OPENSSL_USE_STATIC_LIBS:${OPENSSL_USE_STATIC_LIBS}")
    endif()
    find_package(fastrtps REQUIRED)
endif()

#Check C++11
include(CheckCXXCompilerFlag)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    check_cxx_compiler_flag(-std=c++17 SUPPORTS_CXX11)
    if(NOT SUPPORTS_CXX11)
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
    EXECUTE_PROCESS(COMMAND fastddsgen "-replace" "-d" "cxxclient" "dds_sdk.idl"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        ERROR_VARIABLE fastddsgenErr OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT COMMAND_ERROR_IS_FATAL ANY
    )
endif()

message(STATUS fastddsgenErr:${fastddsgenErr})
# message(STATUS "Configuring HelloWorld example...")
file(GLOB_RECURSE src_cpps LIST_DIRECTORIES false "*.cpp" "*.ipp" "*.cxx" "*.c" "*.cc")
file(GLOB_RECURSE src_headers LIST_DIRECTORIES false "*.h" "*.hh" "*.hpp")

message(STATUS src_cpps:${src_cpps},\nsrc_headers:${src_headers})

configure_file(sdk_profile.xml ${CMAKE_CURRENT_BINARY_DIR}/DEFAULT_FASTRTPS_PROFILES.xml @ONLY)

add_executable(${PROJECT_NAME} ${src_cpps} ${src_headers})

target_include_directories(${PROJECT_NAME} PRIVATE
    .
    ./cxxclient
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<AND:$<NOT:$<BOOL:${WIN32}>>,$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">>:__DEBUG>
    $<$<BOOL:${INTERNAL_DEBUG}>:__INTERNALDEBUG> # Internal debug activated.
)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(${PROJECT_NAME} PRIVATE fastrtps fastcdr)

set(my_install_dir ../dist/${PROJECT_NAME})
if(install_dir)
    set(my_install_dir ${install_dir}/${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${my_install_dir}/bin
    ARCHIVE DESTINATION ${my_install_dir}/bin
    RUNTIME DESTINATION ${my_install_dir}/bin
    # PUBLIC_HEADER DESTINATION include
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DEFAULT_FASTRTPS_PROFILES.xml DESTINATION ${my_install_dir}/bin)
