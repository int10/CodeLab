GCC=g++
CCFLAGS += -I./ -I./pcre -I./src
CXXFLAGS += -I./ -I./pcre -I./src
PCRE_SOURCE=pcre/pcre2_auto_possess.c	\
		pcre/pcre2_chartables.c	\
		pcre/pcre2_compile.c	\
		pcre/pcre2_config.c	\
		pcre/pcre2_context.c	\
		pcre/pcre2_dfa_match.c	\
		pcre/pcre2_error.c	\
		pcre/pcre2_find_bracket.c	\
		pcre/pcre2_jit_compile.c	\
		pcre/pcre2_maketables.c	\
		pcre/pcre2_match.c	\
		pcre/pcre2_match_data.c	\
		pcre/pcre2_newline.c	\
		pcre/pcre2_ord2utf.c	\
		pcre/pcre2_pattern_info.c	\
		pcre/pcre2_serialize.c	\
		pcre/pcre2_string_utils.c	\
		pcre/pcre2_study.c	\
		pcre/pcre2_substitute.c	\
		pcre/pcre2_substring.c	\
		pcre/pcre2_tables.c	\
		pcre/pcre2_ucd.c	\
		pcre/pcre2_valid_utf.c	\
		pcre/pcre2_xclass.c

SOURCE=	src/Util.cpp \
		src/Process.cpp	\
		src/StringEx.cpp \
		src/PcreReg.cpp	\
		src/ListEx.cpp	\
		src/MapEx.cpp	\
		src/FileEx.cpp	\
		src/deskcustom/LauncherXml.cpp	\
		src/deskcustom/DeskText.cpp \
		src/deskcustom/PhoneConfig.cpp \
		src/deskcustom/DeskCustom.cpp \
		main.cpp
PCRE_OBJECT:=$(PCRE_SOURCE:.c=.o)
OBJECT+=$(PCRE_OBJECT)
OBJECT+=$(SOURCE:.cpp=.o)


all: $(OBJECT)
	$(GCC) -o main.exe $(OBJECT)
	
.PHONY : clean
clean :
	rm main.exe main.o src/*.o pcre/*.o