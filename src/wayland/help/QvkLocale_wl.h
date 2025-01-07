/* vokoscreenNG - A desktop recorder
 * Copyright (C) 2017-2022 Volker Kohaupt
 * 
 * Author:
 *      Volker Kohaupt <vkohaupt@volkoh.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * --End_License--
 */

#ifndef QVKLOCALE_WL_H
#define QVKLOCALE_WL_H

#include "QvkDownloader_wl.h"

#include <QObject>
#include <QTemporaryDir>
#include <QStringList>

class QvkLocale_wl: public QObject
{
    Q_OBJECT

public:
    QvkLocale_wl();
    virtual ~QvkLocale_wl() override;


public slots:
    void slot_cleanUp();


private:
    QTemporaryDir temporaryDirLocal;
    QvkDownloader_wl *vkDownload;
    QStringList localeList;


private slots:
    void slot_parse( QString tempPathFileName );


protected:


signals:
   void signal_locale( QStringList list );


};

#endif
