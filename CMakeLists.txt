cmake_minimum_required(VERSION 3.29)
project(MovieWebsite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(${CMAKE_SOURCE_DIR}/external/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(MovieWebsite
        src/API.cpp
        src/DataLoader.cpp
        src/Movie.cpp
)

add_custom_command(TARGET MovieWebsite POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:MovieWebsite>/data
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/public
        $<TARGET_FILE_DIR:MovieWebsite>/public
)


if (WIN32)
    target_link_libraries(MovieWebsite
            PRIVATE
            pthread
            ws2_32
            Mswsock
    )
else()
    target_link_libraries(MovieWebsite
            PRIVATE
            pthread
    )
endif()
