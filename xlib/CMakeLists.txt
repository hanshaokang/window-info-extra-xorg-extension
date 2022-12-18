find_package(X11 REQUIRED)

set(LIBRARY_NAME xlib_wi_extra)

add_library(${LIBRARY_NAME} SHARED
        init.c
        wi.c)

target_link_libraries(${LIBRARY_NAME} PUBLIC
        X11::X11
        common)

target_include_directories(${LIBRARY_NAME} PUBLIC
        ./include)
