#ifndef _EXIT_CODE_H_
#define _EXIT_CODE_H_

//exit code of unpackext4 image
#define EXIT_CODE_X_SUCCESS 0


/*
 * exit code of simg2img
 *
*/
#define EXIT_CODE_SIMG2IMG_SUCCESS 0x0800

//Cannot open output file
#define EXIT_CODE_SIMG2IMG_OUTPUT_FILE_OPEN_ERROR 0x0801

//Cannot open input file
#define EXIT_CODE_SIMG2IMG_INTPUT_FILE_OPEN_ERROR 0x0802

//Cannot read input file
#define EXIT_CODE_SIMG2IMG_INTPUT_FILE_READ_ERROR 0x0803

//Cannot write output file
#define EXIT_CODE_SIMG2IMG_INTPUT_FILE_WRITE_ERROR 0x0804

//Not a sparse file
#define EXIT_CODE_SIMG2IMG_NOT_SPARSE_FILE 0x0805

#endif

