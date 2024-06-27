set(APPLICATION_SOURCE
        tests.c
        tests.h
        main.c
)
add_executable(lpafTest ${APPLICATION_SOURCE})
target_include_directories(lpafTest PUBLIC ${PROJECT_SOURCE_DIR}/framework/)
target_link_libraries(lpafTest $<TARGET_OBJECTS:lpafLib>)
