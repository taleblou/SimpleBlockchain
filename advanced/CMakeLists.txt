cmake_minimum_required(VERSION 3.16)
project(SimpleBlockchainAdvanced CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build tests" ON)

find_package(OpenSSL REQUIRED)
include(FetchContent)

# nlohmann/json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(sbc_adv
  src/crypto.cpp
  src/merkle.cpp
  src/state.cpp
  src/block.cpp
  src/blockchain.cpp
  src/p2p.cpp
  src/storage.cpp
)

target_include_directories(sbc_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(sbc_adv PUBLIC OpenSSL::Crypto nlohmann_json::nlohmann_json)

add_executable(simple_blockchain_adv src/main.cpp)
target_link_libraries(simple_blockchain_adv PRIVATE sbc_adv)

if(BUILD_TESTING)
  enable_testing()
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(tests_adv tests/test_chain.cpp)
  target_link_libraries(tests_adv PRIVATE sbc_adv GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(tests_adv)
endif()

# Windows socket library
if(WIN32)
  target_link_libraries(sbc_adv PUBLIC ws2_32)
  target_link_libraries(simple_blockchain_adv PRIVATE ws2_32)
endif()
