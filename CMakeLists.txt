cmake_minimum_required(VERSION 2.8)
project(cppreadings)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON) 

set(Boost_COMPONENTS system regex date_time filesystem program_options )
find_package( Boost 1.53 REQUIRED ${Boost_COMPONENTS} )
find_package( OpenSSL REQUIRED )

if (OPENSSL_FOUND)
  add_definitions(-DNETWORK_ENABLE_HTTPS)
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STD11)
  if (HAVE_STD11)
    list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall")
  else()
    message(FATAL_ERROR "No advanced standard C++ support (-std=c++11 not defined).")
  endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STD11)
  if (HAVE_STD11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -std=c++11 -stdlib=libc++")
  else()
    message(FATAL_ERROR "No C++11 support for Clang version. Please upgrade Clang to a version supporting C++11.")
  endif()
  add_definitions(
  -DASIO_HAS_MOVE -DASIO_HAS_VARIADIC_TEMPLATES -DASIO_HAS_STD_SYSTEM_ERROR
  -DASIO_ERROR_CATEGORY_NOEXCEPT=noexcept -DASIO_HAS_STD_ARRAY
  -DASIO_HAS_STD_SHARED_PTR -DASIO_HAS_STD_ATOMIC -DASIO_HAS_STD_CHRONO
  -DASIO_HAS_STD_ADDRESSOFF -DASIO_HAS_STD_FUNCTION -DASIO_HAS_STD_TYPE_TRAITS)
endif()

message("C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message("C++ Flags: ${CMAKE_CXX_FLAGS} link flags: ${CMAKE_EXE_LINKER_FLAGS}")

if (Boost_FOUND)
  if (MSVC)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  endif(MSVC)
  if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0501)
  endif(WIN32)
  include_directories(${Boost_INCLUDE_DIRS})
endif(Boost_FOUND)

add_executable(cppreadings server.cpp)
find_library(FCGI_LIB fcgi)
find_library(FCGI++_LIB fcgi++)
message("FCGI libs: ${FCGI_LIB} AND ${FCGI++_LIB}")
target_link_libraries(cppreadings ${Boost_LIBRARIES} ${OpenSSL} ${Threads} ${FCGI_LIB} ${FCGI++_LIB})
