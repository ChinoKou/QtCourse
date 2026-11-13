cmake_minimum_required(VERSION 3.19)
project(notepad LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

qt_add_executable(notepad
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    aboutdialog.cpp
    aboutdialog.h
    aboutdialog.ui
    searchdialog.cpp
    searchdialog.h
    searchdialog.ui
    replacedialog.cpp
    replacedialog.h
    replacedialog.ui
    res.qrc
)

target_link_libraries(notepad
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS notepad
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET notepad
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
