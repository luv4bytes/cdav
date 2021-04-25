cmake_minimum_required(VERSION 3.10)

project(cdav)

# Change if no debug symbols are needed
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

# DEBUG definition is used for debug printing
add_compile_definitions(DEBUG)

add_compile_definitions(VERSION="1.0")
add_compile_definitions(IGNORE_SSL_ERRORS)

add_compile_options("-Wall")

include_directories(/usr/include/libxml2)

add_executable(cdav src/args.c
                    src/cmdfile.c
                    src/dav.c
                    src/helper.c
                    src/main.c
                    src/prop.c
                    src/requests.c
                    src/response.c)

target_link_libraries(cdav xml2)
target_link_libraries(cdav pthread)
target_link_libraries(cdav curl)