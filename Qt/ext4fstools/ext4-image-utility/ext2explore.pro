# -------------------------------------------------
# Project created by QtCreator 2010-03-15T21:59:31
# -------------------------------------------------

LIBS += shell32.lib

TARGET = ext4unpacker
TEMPLATE = app
#DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
CONFIG += console

SOURCES += main.cpp \
    ext2explore.cpp \
    ext2read.cpp \
    platform_win32.c \
    platform_unix.c \
    log.c \
    ext2fs.cpp \
    lvm.cpp \
    ext2copyfile.cpp \
    xattr_from_debugfs.cpp \
    simg2img.c \
    sparse_read.c \
    sparse.c \
    backed_block.c \
    sparse_crc32.c \
    sparse_err.c \
    output_file.c \
    mman.c \
    test_sparse_magic.c \
    ext4_utils.c \
    savedir.cpp

HEADERS += platform.h \
    parttypes.h \
    lvm.h \
    ext2read.h \
    ext2fs.h \
    ext2explore.h \
    partition.h \
    ext2about.h \
    ext2copyfile.h \
    xattr_from_debugfs.h \
    exit_code.h \
    sparse.h \
    sparse_crc32.h \
    sparse_defs.h \
    sparse_file.h \
    sparse_format.h \
    def.h \
    output_file.h \
    backed_block.h \
    zlib.h \
    zconf.h \
    mman.h \
    test_sparse_magic.h \
    ext4_utils.h \
    savedir.h

#RC_FILE = app.rc
