/**
 * Copyright (C) 2010 by Manish Regmi   (regmi dot manish at gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/

//#include <QtGui/QApplication>
#include "ext2explore.h"

#include <QString>
#include <Windows.h>
#include <iostream>

#include "test_sparse_magic.h"
#include "ext4_utils.h"
#include "savedir.h"

using namespace std;

extern QString g_MountPoint;

//extern "C" int simg2img(const char* imgfile_path, const char* outfile_path);

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	printf("version:1.4.20160602\n");

	int message_only = 0;

#ifndef QT_DEBUG
	if(1 == argc)
		return EXIT_SUCCESS;
	if(2 == argc)
		message_only = 1;	//只读取total size,free size信息

	g_MountPoint;
	QString imgfile;
	QString outdir;
	QString fs_config;
	QString selinux_context;
	//modify 20151204
	QString nonsparse_image;

	if(message_only)
		imgfile = argv[1];
	else
	{
		if(argc != 7)
		{
			//fprintf(stderr, "x.exe mount_point imgfile_path out_dir_path out_fs_config_path out_selinux_context_path out_tmp_ext4_file_path\n");
			exit(1);
		}

		//mount_point imgfile out_dir out_fs_config_path out_selinux_context_path log_file_path

		g_MountPoint = argv[1];

		imgfile = argv[2];

		outdir = argv[3];

		fs_config = argv[4];

		selinux_context = argv[5];

		//modify 20151204
		nonsparse_image = argv[6];
	}

#else

	g_MountPoint = "/system";

	QString imgfile = "d:/romm/system.img";

	QString outdir = "d:/romm/unpack";

	QString fs_config = "d:/romm/tmp-fs_config.txt";

	QString selinux_context = "d:/romm/tmp-selinux_context.txt";

	//log_init("d:/rom/log.log");

	QString nonsparse_image = "d:/romm/tmp.ext4";

#endif

/*
	//QString nonsparse_image = imgfile + ".nonsparse.img";

	if(is_sparse_image(imgfile.toAscii()))
	{
		printf("info:file type:sparse image\n");
		int ret = simg2img(
			imgfile.toAscii(),
			nonsparse_image.toAscii());

		imgfile = nonsparse_image;
	}

	//modify show ext4 space info
	if(ext4_info(imgfile.toAscii())){
		return EXIT_FAILURE;
	}

	vector<QString> dirs;

	dirs.push_back(outdir);

	Ext2Explore w;

	w.OpenImage(imgfile);

	w.SaveRawImage2Dir(dirs, fs_config, selinux_context);
*/

	//------------------------------------
	if(is_sparse_image(imgfile.toAscii()))
	{
		printf("info:file type:sparse image\n");
		SaveDir s(imgfile,outdir,fs_config,selinux_context);
		if(0 == message_only)
			s.Explode();
	}
	else
	{
		if(message_only)
		{
			printf("info:file type:ext4 image\n");
			SaveDir::Ext4_Info(imgfile);
		}
		else
		{
			if(ext4_info(imgfile.toAscii())){
				return EXIT_FAILURE;
			}
			vector<QString> dirs;
			dirs.push_back(outdir);
			Ext2Explore w;
			w.OpenImage(imgfile);
			w.SaveRawImage2Dir(dirs, fs_config, selinux_context);
		}
	}
	//------------------------------------

	log_exit();

	printf("info:exit success\n");

	return EXIT_SUCCESS;
}
