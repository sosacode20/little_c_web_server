cmake_minimum_required(VERSION 3.0.0)
project(webserver VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(webserver src/main.c
                         src/meta.c
                         src/robust_io.c
                         src/utils.c
                         include/meta.h
                         include/robust_io.h
                         include/utils.h
                         include/common.h)


target_include_directories(webserver PUBLIC
                           ./include
                           )
                           
                           set(CPACK_PROJECT_NAME ${PROJECT_NAME})
                           set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
                           include(CPack)