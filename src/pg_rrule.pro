TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
#CONFIG += staticlib

INCLUDEPATH += "/usr/include/postgresql/server"

LIBS += -lpq -lical

SOURCES += \
    pg_rrule.c

QMAKE_CFLAGS = -fpic

HEADERS += \
    pg_rrule.h
