QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = httpDownload
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    downloadmanager.cpp \
    myhttpdownload.cpp

HEADERS += \
    downloadmanager.h \
    myhttpdownload.h

FORMS += \
    myhttpdownload.ui

RESOURCES += \
    myhttpdownload.qrc
