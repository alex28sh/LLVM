set(VCPKG_ROOT "~/.vcpkg-clion/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")

find_package(LLVM 18.1.6 REQUIRED CONFIG)

# Add the LLVM include directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

list(APPEND CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/x64-linux")

# Link the LLVM libraries
llvm_map_components_to_libnames(llvm_libs support core passes irreader)
include(AddLLVM)

add_llvm_pass_plugin( MyPass
    MyPass.cpp
    PLUGIN_TOOL
    opt
)

add_library(MyPasslib
    MyPass.cpp
)