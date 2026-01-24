# for some reason GNS expects GetTypeName to return std::string while it actually returns std::string_view (which would be better)
set(GNS_PATCH_CMD 
    sed -i "s/msg.GetTypeName().c_str()/std::string(msg.GetTypeName()).c_str()/g"
    src/steamnetworkingsockets/steamnetworkingsockets_internal.h
    src/steamnetworkingsockets/clientlib/steamnetworkingsockets_connections.cpp
    src/steamnetworkingsockets/clientlib/csteamnetworkingsockets.cpp
    src/steamnetworkingsockets/clientlib/steamnetworkingsockets_udp.cpp
)

FetchContent_Declare(
    GameNetworkingSockets 
    GIT_REPOSITORY https://github.com/ValveSoftware/GameNetworkingSockets
    GIT_TAG v1.4.1
    GIT_PROGRESS TRUE
    PATCH_COMMAND ${GNS_PATCH_CMD}
)

set(GAMENETWORKINGSOCKETS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GAMENETWORKINGSOCKETS_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(GameNetworkingSockets)

if(TARGET GameNetworkingSockets)
    set_target_properties(GameNetworkingSockets PROPERTIES CXX_STANDARD 23)
endif()

if(TARGET GameNetworkingSockets_s)
    set_target_properties(GameNetworkingSockets_s PROPERTIES CXX_STANDARD 23)
endif()

if(TARGET libprotobuf)
    set_target_properties(libprotobuf PROPERTIES CXX_STANDARD 23)
endif()