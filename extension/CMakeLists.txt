find_package(PkgConfig REQUIRED)

pkg_search_module(xorg-server REQUIRED IMPORTED_TARGET xorg-server)
pkg_search_module(pixman REQUIRED IMPORTED_TARGET pixman-1)

set(LIBRARY_NAME window_info_extra)

add_library(${LIBRARY_NAME} MODULE
        module.c
        extension.c)

target_link_libraries(${LIBRARY_NAME} PRIVATE
        PkgConfig::xorg-server
        PkgConfig::pixman
        common
        )