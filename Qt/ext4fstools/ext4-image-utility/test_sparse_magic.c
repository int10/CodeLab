#include <fcntl.h>
#include <stdarg.h>
//#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

#include <io.h>

#include <assert.h>

#include "sparse.h"

#include "defs.h"
#include "output_file.h"
#include "sparse_crc32.h"
#include "sparse_file.h"
#include "sparse_format.h"

#include "test_sparse_magic.h"

#include "exit_code.h"



int is_sparse_image(const char* imgfile_path)
{
    int ret;
    sparse_header_t sparse_header;
    int64_t len;
    struct sparse_file *s;
    int in;

    in = _open(imgfile_path, O_RDONLY | O_BINARY);

    if (in < 0)
    {
        fprintf(stderr, "Cannot open input file %s\n", imgfile_path);
        exit(EXIT_CODE_SIMG2IMG_INTPUT_FILE_OPEN_ERROR);
    }

    ret = read_all(in, &sparse_header, sizeof(sparse_header));

    if (ret < 0)
    {
        fprintf(stderr, "Cannot read input file %s\n", imgfile_path);
        exit(EXIT_CODE_SIMG2IMG_INTPUT_FILE_READ_ERROR);
    }

    if (sparse_header.magic != SPARSE_HEADER_MAGIC)
    {
        _close(in);
        return 0;
    }
    else
    {
        _close(in);
        return 1;
    }

    assert(0);
}
