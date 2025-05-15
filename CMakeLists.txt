cmake_minimum_required(VERSION 3.20)

set(VCPKG_ROOT "~/.vcpkg-clion/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")

project(LLVM_PROJ)

list(APPEND CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/x64-linux")

find_package(nlohmann_json 3.11.3 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

find_package(LLVM 18.1.6 REQUIRED)

# Add the LLVM include directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Link the LLVM libraries
llvm_map_components_to_libnames(llvm_libs support core passes irreader)

add_subdirectory(lib)

add_executable(LLVM_PROJ
        include/expression.h
        include/statement.h
        src/expression.cpp
        src/statement.cpp
        include/from_json.h
        src/from_json.cpp
        include/generate_llvm.h
        src/generate_llvm.cpp
        src/main.cpp
)

include(FetchContent)
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

#target_compile_options(LLVM_PROJ PRIVATE `llvm-config --cxxflags --ldflags --system-libs --libs core orcjit nativ`)
#find_package(nlohmann_json CONFIG REQUIRED)
#target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(LLVM_PROJ PRIVATE nlohmann_json::nlohmann_json)
#target_link_libraries(LLVM_PROJ PRIVATE LLVM)
target_link_libraries(LLVM_PROJ PRIVATE ${llvm_libs})
target_link_libraries(LLVM_PROJ PRIVATE MyBranchElimLib)
target_link_libraries(LLVM_PROJ PRIVATE MyConstantPropLib)
target_link_libraries(LLVM_PROJ PRIVATE MyCSELib)
target_link_libraries(LLVM_PROJ PRIVATE argparse)