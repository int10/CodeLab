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

#ifndef EXT2COPYFILE_H
#define EXT2COPYFILE_H

#include <QDialog>
#include <QFile>
#include <QThread>
#include <QTextCodec>
#include <QTextStream>

#include "ext2read.h"

class Ext2CopyProcess
{
private:
    //for fs_config
    QString m_filepath_fs_config;
    QFile *m_qfile_fs_config;
	//http://stackoverflow.com/questions/4916193/creating-writing-into-a-new-file-in-qt
    QTextStream *m_qtext_stream_fs_config;

    //for capibilities
    QString m_filepath_selinux_context;
    QFile *m_qfile_selinux_context;
    //http://stackoverflow.com/questions/4916193/creating-writing-into-a-new-file-in-qt
    QTextStream *m_qtext_stream_selinux_context;

	QString dest_path;

    Ext2File *m_ext_file;

    QFile *filetosave;
    char *buffer;
    int blksize;
    bool cancelOperation;
    QTextCodec *codec;

    bool Ext2CopyProcess::copy_file(
		QString &dest_path,
        const QString& path_in_image,
        Ext2File *src_ext_file,
        bool fs_config,
        bool selinux_context);

    bool Ext2CopyProcess::copy_link_file_2_system(
        QString &dest_path,
        const QString& path_in_image,
        Ext2File *src_ext_file,
        bool fs_config,
        bool selinux_context);

    bool Ext2CopyProcess::copy_link_file(
        QString &dest_path,
        const QString& path_in_image,
        Ext2File *src_ext_file,
        bool fs_config,
        bool selinux_context);

    bool Ext2CopyProcess::copy_link_file_2(
        QString &dest_path,
        const QString& path_in_image,
        Ext2File *src_ext_file,
        bool fs_config,
        bool selinux_context);

    bool Ext2CopyProcess::copy_folder(
            const QString &dest_path,
            const QString& my_path_in_image,
            Ext2File *parent_ext_folder,
            bool fs_config,
            bool selinux_context);

    bool fs_config(const QString & image_path, const Ext2File * ext2file);

    bool selinux_context(const QString & image_path, const Ext2File * ext2file);

public:

    Ext2CopyProcess(Ext2File *parent, QString &dest);

    Ext2CopyProcess(
            Ext2File *parent,
            QString &dest,
            const QString & fs_config_path,
            const QString & selinux_context_path);

    ~Ext2CopyProcess();

	//modify
	void copy_process();
};

class Ext2CopyFile
{
private:
    QString filename;

    Ext2File *m_file;

    bool cancelOperation;

public:
	Ext2CopyFile(Ext2File *parent, QString &dest);
	Ext2CopyFile();
    ~Ext2CopyFile();
    void set_file(Ext2File *sfile) { m_file = sfile; }
    void set_name(QString &name) { filename = name; }
    void start_copy(QString fs_config, QString selinux_context);
};

#endif // EXT2COPYFILE_H
