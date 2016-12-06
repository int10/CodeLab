#-------------------------------------------------
#
# Project created by QtCreator 2013-11-19T15:51:34
#
#-------------------------------------------------

QT       += core gui

TARGET = lzma
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lzmalib/Threads.c \
    lzmalib/LzmaLib.c \
    lzmalib/LzmaEnc.c \
    lzmalib/LzmaDec.c \
    lzmalib/LzFindMt.c \
    lzmalib/LzFind.c \
    lzmalib/Alloc.c \
    lzma.cpp \
    packer.cpp

HEADERS  += mainwindow.h \
    lzmalib/Types.h \
    lzmalib/Threads.h \
    lzmalib/LzmaLib.h \
    lzmalib/LzmaEnc.h \
    lzmalib/LzmaDec.h \
    lzmalib/LzHash.h \
    lzmalib/LzFindMt.h \
    lzmalib/LzFind.h \
    lzmalib/Alloc.h \
    lzma.h \
    packer.h

FORMS    += mainwindow.ui
