set(BINARY ${CMAKE_PROJECT_NAME}_test)

set(SOURCES
    main.cpp
    )

add_executable(${BINARY} ${SOURCES})

target_link_libraries(${BINARY} ${CMAKE_PROJECT_NAME})
