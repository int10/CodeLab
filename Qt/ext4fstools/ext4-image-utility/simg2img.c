/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#include <fcntl.h>
//#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <unistd.h>

#include <io.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

#include "sparse.h"
#include "exit_code.h"
//modify 20151204
#include "config.h"

void usage()
{
  fprintf(stderr, "Usage: simg2img <sparse_image_files> <raw_image_file>\n");
}


int simg2img(
	//int argc, char *argv[])
	const char* imgfile_path,
	const char* outfile_path)
{
	int in;
	int out;
	int ret;
	struct sparse_file *s;

	//modify 20151204
#ifdef RETAIN_TMP_EXT_FILE
	out = _open(outfile_path, O_WRONLY | O_CREAT | O_BINARY, 0664);
#else
	out = _open(outfile_path, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0664);
#endif

	if (out < 0)
	{
		fprintf(stderr, "Cannot open output file %s\n", outfile_path);
		exit(EXIT_CODE_SIMG2IMG_OUTPUT_FILE_OPEN_ERROR);
	}

	in = _open(imgfile_path, O_RDONLY | O_BINARY);

	if (in < 0)
	{
		fprintf(stderr, "Cannot open input file %s\n", imgfile_path);
		exit(EXIT_CODE_SIMG2IMG_INTPUT_FILE_OPEN_ERROR);
	}

	s = sparse_file_import(in, true, false);

	if (!s)
	{
		fprintf(stderr, "Failed to read sparse file\n");
		exit(EXIT_CODE_SIMG2IMG_INTPUT_FILE_READ_ERROR);
	}


	//lseek(out, SEEK_SET, 0);
	_lseeki64(out, 0, SEEK_SET);

	ret = sparse_file_write(s, out, false, false, false);

	if (ret < 0)
	{
		fprintf(stderr, "Cannot write output file\n");
		exit(EXIT_CODE_SIMG2IMG_INTPUT_FILE_WRITE_ERROR);
	}

	sparse_file_destroy(s);

	_close(in);

	_close(out);

	return EXIT_CODE_SIMG2IMG_SUCCESS;
}
