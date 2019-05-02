QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HTYVS
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    clip.cpp

HEADERS += \
        mainwindow.h \
    clip.h

FORMS += \
        mainwindow.ui \
    clip.ui

RESOURCES += \
    res.qrc