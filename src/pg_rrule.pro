TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

macx {
    INCLUDEPATH += "/usr/local/include/server/" \
                    "/usr/local/include/"
    LIBS += -L/usr/local/lib/ -lical -lpq

    QMAKE_LFLAGS_SHLIB -= -dynamiclib
    QMAKE_LFLAGS_VERSION = ""
    QMAKE_LFLAGS_COMPAT_VERSION = ""
    QMAKE_LFLAGS_SONAME = ""

    QMAKE_LFLAGS = -bundle -flat_namespace -undefined suppress

    QMAKE_EXTENSION_SHLIB = so
}

unix:!macx {
    INCLUDEPATH += $$system(pg_config --includedir-server)

    LIBS += -lpq -lical

    QMAKE_CFLAGS = -fpic
}

SOURCES += \
    pg_rrule.c


HEADERS += \
    pg_rrule.h
