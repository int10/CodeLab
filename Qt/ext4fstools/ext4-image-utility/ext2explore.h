/**
 * Ext2read
 * File: ext2explore.h
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


#ifndef EXT2EXPLORE_H
#define EXT2EXPLORE_H

#include "ext2read.h"
#include "ext2copyfile.h"

//namespace Ui {
//    class Ext2Explore;
//}

class Ext2Explore : public QObject {
	Q_OBJECT
public:
	Ext2Explore(QObject *parent = 0);
	~Ext2Explore();

    void OpenImage(QString filename);

    void SaveRawImage2Dir(
            vector<QString>& dirs,
            QString fs_config,
            QString selinux_context);

    void SavePartition2Dir(
            Ext2File *file,
            QString dir,
            QString fs_config,
            QString selinux_context);

private:
	Ext2Read *m_ext2read;
};

#endif // EXT2EXPLORE_H
