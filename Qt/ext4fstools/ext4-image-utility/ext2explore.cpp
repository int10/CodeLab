/**
 * Ext2read
 * File: ext2explore.cpp
 **/
/**
 * Copyright (C) 2005, 2010 by Manish Regmi   (regmi dot manish at gmail.com)
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

#include "ext2explore.h"
#include "ext2copyfile.h"

Ext2Explore::Ext2Explore(QObject *parent): QObject(parent)
{
	m_ext2read = new Ext2Read();
}

Ext2Explore::~Ext2Explore()
{    
	delete m_ext2read;
}

void Ext2Explore::OpenImage(QString filename)
{
    int result;

	result = m_ext2read->add_loopback(filename.toAscii());

    if(result <= 0)
    {
        LOG("No valid Ext2 Partitions found in the disk image.");
        return;
    }
}


void Ext2Explore::SavePartition2Dir(
        Ext2File *file,
        QString dir,
        QString fs_config,
        QString selinux_context)
{
    if(dir.isEmpty())
        return;

	Ext2CopyFile ext2copyfile;
	ext2copyfile.set_file(file);
	ext2copyfile.set_name(dir);
	ext2copyfile.start_copy(fs_config, selinux_context);
}

void Ext2Explore::SaveRawImage2Dir(
        vector<QString>& dirs,
        QString fs_config,
        QString selinux_context)
{
	Ext2Partition *temp;
    list<Ext2Partition *> parts;
    list<Ext2Partition *>::iterator i;

    Ext2File *ptr;

	parts = m_ext2read->get_partitions();

	if(dirs.size() != parts.size())
	{
		return;
	}

    int j = 0;

    for(i = parts.begin(); i != parts.end(); i++, j++)
    {
        temp = (*i);

		ptr = temp->get_root();

        SavePartition2Dir(ptr, dirs[j], fs_config, selinux_context);
	}
}
