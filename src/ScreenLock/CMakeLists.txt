
find_package(Qt5 COMPONENTS DBus REQUIRED)

add_library(ScreenLock
    ScreenLockManager.cc
    ScreenLockManager.h
)

target_link_libraries(ScreenLock
    PUBLIC
        Qt5::Core
        Qt5::DBus
        qgc
)

target_include_directories(ScreenLock
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
