/* vokoscreenNG - A desktop recorder
 *
 * Copyright (C) 2017-2019 Volker Kohaupt
 * 
 * Author:
 *      Volker Kohaupt <vkohaupt@freenet.de>
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

#ifndef QVKSYSTRAYBALLONNEWVERSION_H
#define QVKSYSTRAYBALLONNEWVERSION_H

#include "ui_formMainWindow.h"

#include "QvkVersion.h"

#include <QObject>
#include <QSystemTrayIcon>

class QvkSystrayBallonNewVersion: public QSystemTrayIcon
{
    Q_OBJECT

public:
    QvkSystrayBallonNewVersion( Ui_formMainWindow *ui_mainwindow );
    virtual ~QvkSystrayBallonNewVersion();
    void init();

    
public slots:


private:
    Ui_formMainWindow *ui;
    QvkVersion version;


private slots:
    void slot_newVersionAvailable( QString update );


protected:
  
  
signals:
    
};

#endif