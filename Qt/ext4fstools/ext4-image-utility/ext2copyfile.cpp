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

#include <QMessageBox>
#include <QDir>

#include "ext2copyfile.h"

#include "xattr_from_debugfs.h"

#include <iostream>

#include <assert.h>

#include <ShellAPI.h>
#include <QDebug>

Ext2CopyFile::Ext2CopyFile(Ext2File *parent, QString &dest)
{
    filename = dest;
    m_file = parent;
    Ext2CopyFile();
}

Ext2CopyFile::Ext2CopyFile()
{
    cancelOperation = false;
}

Ext2CopyFile::~Ext2CopyFile()
{
}

void Ext2CopyFile::start_copy(QString fs_config, QString selinux_context)
{
	if(!EXT2_S_ISREG(m_file->inode.i_mode) && !EXT2_S_ISDIR(m_file->inode.i_mode))
	{
		exit(-1);
		return;
	}

	Ext2CopyProcess proc(m_file, filename, fs_config, selinux_context);

	proc.copy_process();
}


Ext2CopyProcess::Ext2CopyProcess(Ext2File *parent, QString &dest)
{
    dest_path = dest;
    m_ext_file = parent;
    blksize = parent->partition->get_blocksize();
    buffer = new char [blksize];
    filetosave = NULL;
    cancelOperation = false;
    codec = QTextCodec::codecForName("utf-8");
}

Ext2CopyProcess::~Ext2CopyProcess()
{
    delete [] buffer;
    if(filetosave)
        delete filetosave;
}

void Ext2CopyProcess::copy_process()
{
    bool fs_config = false;

    bool selinux_context = false;

	QString image_path = ""; //this is the name of the root of the file system aka EMPTY

    if(m_filepath_fs_config != "")
	{
        this->m_qfile_fs_config = new QFile(m_filepath_fs_config);

        this->m_qfile_fs_config->open(QFile::ReadWrite | QFile::Text | QIODevice::Truncate);

        this->m_qtext_stream_fs_config = new QTextStream( this->m_qfile_fs_config);
		//modify 20150924
		this->m_qtext_stream_fs_config->setCodec("utf-8");

        fs_config = true;
	}

    if(m_filepath_selinux_context != "")
    {
        this->m_qfile_selinux_context = new QFile(m_filepath_selinux_context);

        this->m_qfile_selinux_context->open(QFile::ReadWrite | QFile::Text | QIODevice::Truncate);

		this->m_qtext_stream_selinux_context = new QTextStream( this->m_qfile_selinux_context);
		//modify 20150924
		this->m_qtext_stream_selinux_context->setCodec("utf-8");

        selinux_context = true;
    }

    if(EXT2_S_ISDIR(m_ext_file->inode.i_mode))
    {
        copy_folder(dest_path, image_path,  m_ext_file, fs_config, selinux_context);

        return ;
    }
	else if(!EXT2_S_ISREG(m_ext_file->inode.i_mode))
    {
        return ;
    }

    copy_file(dest_path, image_path,  m_ext_file, fs_config, selinux_context);
}

bool Ext2CopyProcess::copy_file(
	QString &dest_path,
	const QString& path_in_image,
	Ext2File *src_ext_file, 
    bool fs_config,
    bool selinux_context)
{
    lloff_t blocks, blkindex;
    QString qsrc;
    QString nullstr = QString::fromAscii("");
    QByteArray ba;
    int extra;
    int ret;

	//modify
	QFileInfo fileinfo(dest_path);
	if(fileinfo.exists()){
		QString pathname = fileinfo.path();
		QString filename = fileinfo.fileName();
		for(int i=2; i<256; i++){
			dest_path = pathname + "/" + filename + ".samemark" + QString::number(i);
			QFileInfo fi(dest_path);
			if(!fi.exists()) break;
		}
	}
	//qDebug()<<"dest_path:"<<dest_path;

    QFile *filetosave = new QFile(dest_path);

    if (!filetosave->open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        LOG("Error creating file %s.\n", src_ext_file->file_name.c_str());
        return false;
    }

    if(fs_config)
    {
        this->fs_config(path_in_image, src_ext_file);
    }

    if(selinux_context)
    {
        this->selinux_context(path_in_image, src_ext_file);
    }
	
    //ba = destfile.toAscii();
    //const char *c_str2 = ba.data();

    //LOG("Copying file %s as %s\n", srcfile->file_name.c_str(), c_str2);
    qsrc = codec->toUnicode(src_ext_file->file_name.c_str());
    blocks = src_ext_file->file_size / blksize;
    for(blkindex = 0; blkindex < blocks; blkindex++)
    {
        if(cancelOperation)
        {
            return false;
        }
        ret = src_ext_file->partition->read_data_block(&src_ext_file->inode, blkindex, buffer);
        if(ret < 0)
        {
            filetosave->close();
            return false;
        }
        filetosave->write(buffer, blksize);
		//if((blkindex % 10) == 0)
		//	emit sig_updateui(qsrc, dest_path, dest_path, (int)blkindex, (int)blocks);
    }

    extra = src_ext_file->file_size % blksize;

    if(extra)
    {
        ret = src_ext_file->partition->read_data_block(&src_ext_file->inode, blkindex, buffer);
        if(ret < 0)
        {
            filetosave->close();
            return false;
        }
        filetosave->write(buffer, extra);
    }

    filetosave->close();

    return true;
}

bool Ext2CopyProcess::copy_link_file(
	QString &dest_path, 
	const QString& path_in_image, 
	Ext2File *src_ext_file, 
    bool fs_config,
    bool selinux_context)
{
    lloff_t blocks, blkindex;
    QString qsrc;
    QString nullstr = QString::fromAscii("");
    QByteArray ba;
    int extra;
    int ret;

    QFile *filetosave = new QFile(dest_path);
    if (!filetosave->open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        LOG("Error creating file %s.\n", src_ext_file->file_name.c_str());
        return false;
    }

    if(fs_config)
    {
        this->fs_config(path_in_image, src_ext_file);
    }

    if(selinux_context)
    {
        this->selinux_context(path_in_image, src_ext_file);
    }
	
    //ba = destfile.toAscii();
    //const char *c_str2 = ba.data();

    //LOG("Copying file %s as %s\n", srcfile->file_name.c_str(), c_str2);
    qsrc = codec->toUnicode(src_ext_file->file_name.c_str());
    blocks = src_ext_file->file_size / blksize;
    for(blkindex = 0; blkindex < blocks; blkindex++)
    {
        if(cancelOperation)
        {
            return false;
        }
        ret = src_ext_file->partition->read_data_block(&src_ext_file->inode, blkindex, buffer);
        if(ret < 0)
        {
            filetosave->close();
            return false;
        }
        filetosave->write(buffer, blksize);
		//if((blkindex % 10) == 0)
		//	emit sig_updateui(qsrc, dest_path, dest_path, (int)blkindex, (int)blocks);
    }

    extra = src_ext_file->file_size % blksize;

    if(extra)
    {
        ret = src_ext_file->partition->read_data_block(&src_ext_file->inode, blkindex, buffer);
        if(ret < 0)
        {
            filetosave->close();
            return false;
        }
        filetosave->write(buffer, extra);
    }

    filetosave->close();

    return true;
}


bool Ext2CopyProcess::copy_link_file_2(
	QString &dest_path, 
	const QString& path_in_image, 
	Ext2File *src_ext_file, 
    bool fs_config,
    bool selinux_context)
{
    lloff_t blocks, blkindex;
    QString qsrc;
    QString nullstr = QString::fromAscii("");
    QByteArray ba;
    int extra;
    int ret;

    QFile *filetosave = new QFile(dest_path);

    if (!filetosave->open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        LOG("Error creating file %s.\n", src_ext_file->file_name.c_str());
        return false;
    }

    if(fs_config)
    {
        this->fs_config(path_in_image, src_ext_file);
    }

    if(selinux_context)
    {
        this->selinux_context(path_in_image, src_ext_file);
    }

	QString link((char*)(src_ext_file->inode.i_block));

	QTextStream s(filetosave);// = filetosave

    //s << link <<endl;
    s << link;

    s.flush();

	filetosave->close();
	
    return true;
}


int ExecuteShell(int hwnd , const char * lpOperation, const char * lpFile, const char * lpParameters, const char * lpDirectory, int nShowCmd)
{
    hwnd = hwnd;
    lpDirectory = lpDirectory;

    SHELLEXECUTEINFO ShExecInfo = {0};
//    qDebug() <<QString::fromLocal8Bit("杩愯鍙墽琛岀▼搴" )<< lpOperation << " "<< lpFile  << " " << lpParameters  << " " << nShowCmd;
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;

    QString Operation = lpOperation;

    ShExecInfo.lpVerb = Operation.utf16();

    QString ParametersName	= QString::fromUtf8(lpParameters);
    QString FileName		= QString::fromUtf8(lpFile);


    ShExecInfo.lpParameters		= ParametersName.utf16();//(LPCWSTR)lpParameters;
    ShExecInfo.lpFile			= FileName.utf16();
    ShExecInfo.lpDirectory		= NULL;
    ShExecInfo.nShow			= nShowCmd;//(nShowCmd != SW_HIDE)?(SW_SHOW):(SW_HIDE);
    ShExecInfo.hInstApp			= NULL;
    int retValue = ShellExecuteEx(&ShExecInfo);
    if (retValue)
    {
        WaitForSingleObject(ShExecInfo.hProcess,1000);
    }
    return (int)ShExecInfo.hInstApp;//temp for retValue;

}


bool Ext2CopyProcess::copy_link_file_2_system(
    QString &dest_path,
    const QString& path_in_image,
    Ext2File *src_ext_file,
    bool fs_config,
    bool selinux_context)
{
    lloff_t blocks, blkindex;
    QString qsrc;
    QString nullstr = QString::fromAscii("");
    QByteArray ba;
    int extra;
    int ret;

    /*
    QFile *filetosave = new QFile(dest_path);

    if (!filetosave->open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        LOG("Error creating file %s.\n", src_ext_file->file_name.c_str());
        return false;
    }
    */

    if(fs_config)
    {
        this->fs_config(path_in_image, src_ext_file);
    }

    if(selinux_context)
    {
        this->selinux_context(path_in_image, src_ext_file);
    }

    QString link((char*)(src_ext_file->inode.i_block));

    //QTextStream s(filetosave);// = filetosave

    //s << link <<endl;
    //s << link;

    //s.flush();

    //filetosave->close();

    unlink(dest_path.toAscii().data());

    QString s =
            "start /b mklink \"" +
            dest_path + "\" \"" +
            link + '"';

    //BOOL WINAPI CreateSymbolicLink(dest_path, link, 0);

    //std::cerr << s.toAscii() << std::endl;

    //std::cerr << s.toAscii().data() << std::endl;

    if(!CreateSymbolicLink(
                dest_path.toStdWString().data(),
                link.toStdWString().data(), 0))
    {
        DWORD err = GetLastError();

        return false;
    }


    /*
    ret = ExecuteShell(
                0,
                "open",
                //startfilePath.toUtf8().constData(),
                "mklink",
                (dest_path + " " + link).toUtf8().constData(),
                0, 0);

    if(ret <= 32)
    {

    }
    */

    return true;
}

Ext2CopyProcess::Ext2CopyProcess(
        Ext2File *parent,
        QString &dest,
        const QString & fs_config_path,
		const QString & selinux_context_path)
{
	dest_path = dest;
    m_ext_file = parent;
    blksize = parent->partition->get_blocksize();
    buffer = new char [blksize];
    filetosave = NULL;
    cancelOperation = false;
    codec = QTextCodec::codecForName("utf-8");

    this->m_filepath_fs_config = fs_config_path;
    this->m_filepath_selinux_context = selinux_context_path;
}

/*

  /bin/bash[TABLE]r[TABLE]mtime[TABLE]mode[TABLE]uid[TABLE]gid

*/

QString g_MountPoint = "/system";

//modify 20151204
#define DELIM_CHAR "\t"

bool Ext2CopyProcess::fs_config(
        const QString & image_path,
        const Ext2File * file)
{
	//*this->m_qtext_stream_fs_config << g_MountPoint << image_path << " ";

    if(image_path == "")
    {
        QString root_path ="/";

		*this->m_qtext_stream_fs_config << g_MountPoint << root_path << DELIM_CHAR;
    }
    else
    {
		*this->m_qtext_stream_fs_config << g_MountPoint << image_path << DELIM_CHAR;;
    }

    if (EXT2_S_ISDIR(file->inode.i_mode))
    {
        //*this->m_qtext_stream << "d" << "\t";

        //return true;
    }
    else if (EXT2_S_ISREG(file->inode.i_mode))
    {
        //*this->m_qtext_stream << "r" << "\t";
        //return true;
    }
    else if (EXT2_S_ISLINK(file->inode.i_mode))
    {
        //*this->m_qtext_stream << "l" << "\t";
        //return true;
    }
    else
    {
        return false;
    }

    //uint32_t	i_mtime;		/* Modification time */

	if(0)
	{
        *this->m_qtext_stream_fs_config << file->inode.i_mtime << "\t";
	}

	//*this->m_qtext_stream << file->inode.i_ctime << "\t";

	//*this->m_qtext_stream << file->inode.i_atime << "\t";

	//*this->m_qtext_stream << file->inode.i_dtime << "\t";

    //uint16_t	i_uid;		/* Low 16 bits of Owner Uid */
	
    this->m_qtext_stream_fs_config->setIntegerBase(10);

	*this->m_qtext_stream_fs_config << file->inode.i_uid << DELIM_CHAR;

    //uint16_t	i_gid;		/* Low 16 bits of Group Id */
	*this->m_qtext_stream_fs_config << file->inode.i_gid << DELIM_CHAR;

    this->m_qtext_stream_fs_config->setIntegerBase(8);

	//uint16_t	i_mode;		/* File mode */
	*this->m_qtext_stream_fs_config << "0" << (file->inode.i_mode & 0x1FF) << DELIM_CHAR;

	char buffer[256];

    uint16_t inode_size = file->partition->inode_size;

    uint64_t capability = 0UL;

    //std::string capibility = "0";

	xattr_fetch_capibilities(
		&(file->inode),
		inode_size,
        &capability);

    //QString qcapibility = capibility.c_str();

    char buf[256] = {0};

    sprintf(buf, "%llu", capability);

    *this->m_qtext_stream_fs_config << "capabilities=" << buf << endl;

	return true;
}



bool Ext2CopyProcess::selinux_context(const QString & image_path, const Ext2File * ext2file)
{
    char buffer[256];

    uint16_t inode_size = ext2file->partition->inode_size;

    std::string s = "";

    xattr_fetch_selinux_label(
        &(ext2file->inode),
        inode_size,
        &s);

    if(s != "")
    {
        //QString path = g_MountPoint + image_path;
        QString path = image_path;

        if(path == "")
        {
            path = "/";
        }

        path = g_MountPoint + path;

        //对path进行转义

        //这个要放在第一个哦
        path.replace("\\","\\\\");

        //我们没有支持全部

        //https://msdn.microsoft.com/zh-cn/library/ae5bf541%28v=vs.90%29.aspx

        path.replace(QString("+"), QString("\\+"));
        path.replace("-","\\-");
        path.replace("^","\\^");
        path.replace("$","\\$");
        path.replace("*","\\*");
        path.replace("?","\\?");
        path.replace("{","\\{");
        path.replace("}","\\}");
        path.replace(".","\\.");
        path.replace("(","\\(");
        path.replace(")","\\)");
        path.replace(":","\\:");
        path.replace("=","\\=");
        path.replace("!","\\!");
        path.replace("|","\\|");
        path.replace("[","\\[");
        path.replace("]","\\]");

        //*this->m_qtext_stream_selinux_context << "/system" << image_path << " ";

		*this->m_qtext_stream_selinux_context << path << DELIM_CHAR;

        *this->m_qtext_stream_selinux_context << s.c_str() << endl;
    }

    return true;
}

//bool Ext2CopyProcess::copy_folder(QString &dest_path, Ext2File *src_ext_folder, bool withlog)
bool Ext2CopyProcess::copy_folder(
        const QString &dest_path,
        const QString& my_path_in_image,
        Ext2File *parent_ext_folder,
        bool fs_config,
        bool selinux_context)
{
    QDir dest_dir;

    dest_dir.mkpath(dest_path);

    //QString rootname = dest_path;
    EXT2DIRENT *dirent;
    Ext2Partition *part = parent_ext_folder->partition;

    QByteArray ba;
    bool ret;

    if(!EXT2_S_ISDIR(parent_ext_folder->inode.i_mode))
    {
        return false;
    }

    //dest_dir.mkdir(codec->toUnicode(parent_ext_folder->file_name.c_str()));

    //dest_dir.mkdir();
    /*ba = path.toAscii();
    const char *c_str2 = ba.data();
    LOG("Creating Folder %s as %s\n", parent->file_name.c_str(), c_str2);
	*/

    dirent = part->open_dir(parent_ext_folder);

    Ext2File *child_ext = NULL;

    if(fs_config)
    {
		//目录后面加/

		//if()

        //log_fileinfo(my_path_in_image + "/", parent_ext_folder);

        this->fs_config(my_path_in_image, parent_ext_folder);
    }

    if(selinux_context)
    {
        this->selinux_context(my_path_in_image, parent_ext_folder);
    }

    while((child_ext = part->read_dir(dirent)) != NULL)
    {

		QString new_dest_path = dest_path;	
		new_dest_path.append(QString::fromAscii("/"));
		new_dest_path.append(codec->toUnicode(child_ext->file_name.c_str()));

		QString my_path_in_image_and_mychild = my_path_in_image;
        my_path_in_image_and_mychild.append(QString::fromAscii("/"));
        my_path_in_image_and_mychild.append(codec->toUnicode(child_ext->file_name.c_str()));

        if(EXT2_S_ISDIR(child_ext->inode.i_mode))
        {

			//QString new_dest_path = dest_path;	
			//new_dest_path.append(QString::fromAscii("/"));
			//new_dest_path.append(codec->toUnicode(child_ext->file_name.c_str()));

            //QString my_path_in_image_and_mychild = my_path_in_image;
            //my_path_in_image_and_mychild.append(QString::fromAscii("/"));
           // my_path_in_image_and_mychild.append(codec->toUnicode(child_ext->file_name.c_str()));

            ret = copy_folder(
                        new_dest_path,
                        my_path_in_image_and_mychild,
                        child_ext,
                        fs_config,
                        selinux_context);

            if((ret == false) && (cancelOperation == true))
            {
                part->close_dir(dirent);
                return false;
            }
            continue;
        }
        else if(EXT2_S_ISREG(child_ext->inode.i_mode))
		{
			//new_dest_path.append(QString::fromAscii("/"));
            //new_dest_path.append(codec->toUnicode(child_ext->file_name.c_str()));

            //QString my_path_in_image_and_mychild = my_path_in_image;
            //my_path_in_image_and_mychild.append(QString::fromAscii("/"));
            //my_path_in_image_and_mychild.append(codec->toUnicode(child->file_name.c_str()));

			//QString new_dest_path = dest_path;

            ret = copy_file(
                        new_dest_path,
                        my_path_in_image_and_mychild,
                        child_ext,
                        fs_config,
                        selinux_context);

			if((ret == false) && (cancelOperation == true))
			{
				part->close_dir(dirent);
				return false;
			}
		}
        else if (EXT2_S_ISLINK(child_ext->inode.i_mode))
		{
			//QString new_dest_path = dest_path;

			//new_dest_path.append(QString::fromAscii("/"));
            //new_dest_path.append(codec->toUnicode(child_ext->file_name.c_str()));

            //QString my_path_in_image_and_mychild = my_path_in_image;
            //my_path_in_image_and_mychild.append(QString::fromAscii("/"));
            //my_path_in_image_and_mychild.append(codec->toUnicode(child->file_name.c_str()));

			unsigned int len = child_ext->inode.i_size;


            //system("");


			if(len + 1 <= sizeof(child_ext->inode.i_block))
			{
                ret = copy_link_file_2_system(
                            new_dest_path,
                            my_path_in_image_and_mychild,
                            child_ext,
                            fs_config,
                            selinux_context);

                if(0)
                {
                    //we use system now.
                    //Fast symbolic link
                    ret = copy_link_file_2(
                                new_dest_path,
                                my_path_in_image_and_mychild,
                                child_ext,
                                fs_config,
                                selinux_context);
                }

			}
			else
			{
                //we do not consider this situation now.
                assert(0);
                ret = copy_link_file(
                            new_dest_path,
                            my_path_in_image_and_mychild,
                            child_ext,
                            fs_config,
                            selinux_context);
			}

			if((ret == false) && (cancelOperation == true))
			{
				part->close_dir(dirent);
				return false;
			}
		}
		else
        //else if(!EXT2_S_ISREG(child->inode.i_mode) && !EXT2_S_ISLINK(child->inode.i_mode))
        {
            //We deal with symbolic link file.
            continue;
            //part->close_dir(dirent);
            //return false;
        }
    }
    return true;
}
