QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = semg
TEMPLATE = app

FORMS += \
    dialog.ui

HEADERS += \
    convertthread.h \
    dialog.h \
    mat.h

SOURCES += \
    convertthread.cpp \
    dialog.cpp \
    main.cpp \
    mat.cpp

RC_FILE = myapp.rc


