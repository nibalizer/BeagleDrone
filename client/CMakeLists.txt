PROJECT(beagledrone-gui)

SET(CPP_SRCS
    gimbalwidget.cpp
    joysticksdialog.cpp
    main.cpp
    mainwindow.cpp
    motorswidget.cpp)

SET(UI_SRCS
    ui/joysticksdialog.ui
    ui/joystickeditdialog.ui
    ui/mainwindow.ui
    ui/motorswidget.ui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

qt5_wrap_ui(UI_HDRS ${UI_SRCS})

add_executable(beagledrone-gui ${CPP_SRCS} ${UI_HDRS})

qt5_use_modules(beagledrone-gui Widgets)
