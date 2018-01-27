#-------------------------------------------------
#
# Project created by QtCreator 2018-01-16T18:51:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cydownload
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialogrepo.cpp \
    helpers.cpp \
    downloader.cpp \
    logger.cpp \
    repodialog.cpp \
    zlib/adler32.c \
    zlib/compress.c \
    zlib/crc32.c \
    zlib/deflate.c \
    zlib/gzclose.c \
    zlib/gzlib.c \
    zlib/gzread.c \
    zlib/gzwrite.c \
    zlib/infback.c \
    zlib/inffast.c \
    zlib/inflate.c \
    zlib/inftrees.c \
    zlib/trees.c \
    zlib/uncompr.c \
    zlib/zutil.c \
    bzip2/blocksort.c \
    bzip2/bzlib.c \
    bzip2/crctable.c \
    bzip2/decompress.c \
    bzip2/huffman.c \
    bzip2/randtable.c \
    bzip2/bzcompress.c

HEADERS += \
        mainwindow.h \
    dialogrepo.h \
    helpers.h \
    downloader.h \
    logger.h \
    types.h \
    repodialog.h \
    zlib/crc32.h \
    zlib/deflate.h \
    zlib/gzguts.h \
    zlib/inffast.h \
    zlib/inffixed.h \
    zlib/inflate.h \
    zlib/inftrees.h \
    zlib/trees.h \
    zlib/zconf.h \
    zlib/zlib.h \
    zlib/zutil.h \
    bzip2/bzlib.h \
    bzip2/bzlib_private.h

FORMS += \
        mainwindow.ui \
    dialogrepo.ui \
    repodialog.ui
