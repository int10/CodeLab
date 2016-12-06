TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ./src ./pcre
SOURCES += main.cpp \
    src/PcreReg.cpp \
    src/StringEx.cpp \
	src/ListEx.cpp \
    src/MapEx.cpp \
    src/Util.cpp \
	src/FileEx.cpp \
    pcre/pcre2_auto_possess.c \
    pcre/pcre2_chartables.c \
    pcre/pcre2_compile.c \
    pcre/pcre2_config.c \
    pcre/pcre2_context.c \
    pcre/pcre2_dfa_match.c \
    pcre/pcre2_error.c \
    pcre/pcre2_find_bracket.c \
    pcre/pcre2_jit_compile.c \
    pcre/pcre2_jit_match.c \
    pcre/pcre2_jit_misc.c \
    pcre/pcre2_maketables.c \
    pcre/pcre2_match.c \
    pcre/pcre2_match_data.c \
    pcre/pcre2_newline.c \
    pcre/pcre2_ord2utf.c \
    pcre/pcre2_pattern_info.c \
    pcre/pcre2_serialize.c \
    pcre/pcre2_string_utils.c \
    pcre/pcre2_study.c \
    pcre/pcre2_substitute.c \
    pcre/pcre2_substring.c \
    pcre/pcre2_tables.c \
    pcre/pcre2_ucd.c \
    pcre/pcre2_valid_utf.c \
    pcre/pcre2_xclass.c \
    src/ProcCmd.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
	src/PcreReg.h \
    src/ListEx.h \
    src/MapEx.h \
    src/FileEx.h \
    src/StringEx.h \
    src/Util.h \
    pcre/pcre2_platform_config.h \
    pcre/pcre2.h \
    pcre/pcre2_internal.h \
    pcre/pcre2_intmodedep.h \
    pcre/pcre2_ucp.h \
    pcre/pcre2_platform_config.h \
    src/ProcCmd.h

