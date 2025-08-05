QT += core widgets openglwidgets

CONFIG += c++20

QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic -std=c++20

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -O2
}

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g -O0
}

TARGET = 3DViewer
TEMPLATE = app

INCLUDEPATH += $$PWD/.. \
               $$PWD/../model \
               $$PWD/../controller

SOURCES += \
    ../main.cpp \
    ../model/model.cpp \
    ../model/tranformation.cpp \
    ../controller/controller.cpp \
    gui.cpp \
    opengl_widget.cpp \
    facade.cpp

HEADERS += \
    gui.h \
    opengl_widget.h \
    facade.h \
    ../controller/controller.h \
    ../model/model.h \
    ../model/tranformation.h

FORMS += \
    view.ui

RESOURCES += \
    resources.qrc
