# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
    libstlport

LOCAL_PRELINK_MODULE := false
LOCAL_C_INCLUDES += $(LOCAL_PATH)/pcre $(LOCAL_PATH)/src
LOCAL_CFLAGS += -O2 
LOCAL_MODULE    :=  PhoneJob
LOCAL_SRC_FILES :=		pcre/pcre2_auto_possess.c	\
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
						pcre/pcre2_xclass.c	\
						src/Util.cpp \
						src/ProcCmd.cpp	\
						src/StringEx.cpp	\
						src/PcreReg.cpp	\
						src/ListEx.cpp	\
						src/MapEx.cpp	\
						src/FileEx.cpp	\
						src/des/des.cpp	\
						src/des/base64.c	\
						main.cpp

include $(BUILD_EXECUTABLE)
