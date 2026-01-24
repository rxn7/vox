set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    absl
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20240116.1
)
FetchContent_MakeAvailable(absl)

set(protobuf_MODULE_COMPATIBLE ON CACHE BOOL "")
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v25.3
)
FetchContent_MakeAvailable(protobuf)

set(GAMENETWORKINGSOCKETS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GAMENETWORKINGSOCKETS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    GameNetworkingSockets 
    GIT_REPOSITORY https://github.com/ValveSoftware/GameNetworkingSockets
    GIT_TAG v1.4.1
    GIT_PROGRESS TRUE
)
FetchContent_GetProperties(GameNetworkingSockets)
if(NOT GameNetworkingSockets_POPULATED)
    FetchContent_Populate(GameNetworkingSockets)
    add_subdirectory(
        ${gamenetworkingsockets_SOURCE_DIR} 
        ${gamenetworkingsockets_BINARY_DIR} 
        EXCLUDE_FROM_ALL
    )
endif()

if(TARGET libprotobuf)
    target_compile_options(libprotobuf PRIVATE -w) # supress warnings
endif()

if(TARGET GameNetworkingSockets_s)
    target_compile_options(GameNetworkingSockets_s PRIVATE -w) # supress warnings
endif()