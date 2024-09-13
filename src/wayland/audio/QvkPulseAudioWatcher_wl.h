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

#ifndef QVKPULSEAUDIOWATCHER_WL_H
#define QVKPULSEAUDIOWATCHER_WL_H


#include "ui_formMainWindow_wl.h"

#include <QObject>
#include <QTimer>

class QvkPulseAudioWatcher_wl: public QObject
{
    Q_OBJECT


public:
    QvkPulseAudioWatcher_wl( Ui_formMainWindow_wl *ui_mainwindow_wl );
    virtual ~QvkPulseAudioWatcher_wl();
    void start_monitor();

public slots:


private:
    Ui_formMainWindow_wl *ui;
    QTimer *timer;


private slots:
    void slot_update();
    void slot_audioDeviceSelected();


protected:


signals:
    void signal_haveAudioDeviceSelected( bool );


};

#endif