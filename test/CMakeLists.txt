find_package(GTest)

if (${GTest_FOUND})
    add_executable(test_wi_extra
            main.cpp
            test_wiextra.cpp)

    target_link_libraries(test_wi_extra PRIVATE
            GTest::Main
            xlib_wi_extra)
endif()