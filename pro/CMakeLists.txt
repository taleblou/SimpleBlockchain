cmake_minimum_required(VERSION 3.16)
project(SimpleBlockchainPro CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options
option(BUILD_TESTING "Build tests" ON)

# OpenSSL
find_package(OpenSSL REQUIRED)

include(FetchContent)

# nlohmann/json (header-only)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(simplebc_pro
  src/block.cpp
  src/blockchain.cpp
)

target_include_directories(simplebc_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(simplebc_pro PUBLIC OpenSSL::Crypto nlohmann_json::nlohmann_json)

add_executable(simple_blockchain_pro src/main.cpp)
target_link_libraries(simple_blockchain_pro PRIVATE simplebc_pro)

# Tests
if(BUILD_TESTING)
  enable_testing()
  # GoogleTest
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  # For Windows: prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(tests_pro tests/test_blockchain.cpp)
  target_link_libraries(tests_pro PRIVATE simplebc_pro GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(tests_pro)
endif()
