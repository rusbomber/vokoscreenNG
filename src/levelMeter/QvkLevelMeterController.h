/* vokoscreenNG - A desktop recorder
 * Copyright (C) 2017-2024 Volker Kohaupt
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

#ifndef QVKVOLUMETERCONTROLLER_H
#define QVKVOLUMETERCONTROLLER_H

#include <QObject>

#include "ui_formMainWindow.h"  // contains the GUI
#include "mainwindow.h"         // contains the class QvkMainWindow
#include "QvkLevelMeter.h"

class QvkLevelMeterController : public QObject
{
    Q_OBJECT

public:
    explicit QvkLevelMeterController( QvkMainWindow *myParent );
    ~QvkLevelMeterController();


private:
    QvkMainWindow *vkMainWindow;
    Ui_formMainWindow *ui;
    void test_gstreamer_level();


public slots:


private slots:


protected:


signals:


};

#endif