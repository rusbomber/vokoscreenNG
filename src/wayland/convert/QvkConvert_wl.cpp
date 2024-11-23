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

#include "global.h"
#include "QvkConvert_wl.h"
#include "QvkFileDialog.h"

#include <QDebug>
#include <QStandardPaths>

QvkConvert_wl::QvkConvert_wl( QvkMainWindow_wl *vkMainWindow, Ui_formMainWindow_wl *vk_ui )
{
    ui = vk_ui;
    connect( ui->toolButton_convert_mkv, SIGNAL( clicked(bool) ), this, SLOT( slot_convert_openfiledialog_mkv_to_mp4(bool) ) );
    connect( ui->pushButton_convert_mp4, SIGNAL( clicked(bool) ), this, SLOT( slot_convert_mkv_to_mp4(bool) ) );
}


QvkConvert_wl::~QvkConvert_wl()
{
}


void QvkConvert_wl::slot_convert_openfiledialog_mkv_to_mp4(bool)
{
    QApplication::setDesktopSettingsAware( false );

    QString pathFile;
    QvkFileDialog vkFileDialog( this );
    QStringList list( { "video/x-matroska" } );
    vkFileDialog.setMimeTypeFilters( list );
    vkFileDialog.setModal( true );
    vkFileDialog.setVideoPath( QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ) );
    if ( vkFileDialog.exec() == QDialog::Accepted ) {
        if ( !vkFileDialog.selectedFiles().empty() ) {
            pathFile = vkFileDialog.selectedFiles().at(0);
            ui->lineEditConvert->setText( pathFile );
            ui->pushButton_convert_mp4->setEnabled( true );
        }
    }

    QApplication::setDesktopSettingsAware( true );
}


void QvkConvert_wl::slot_convert_mkv_to_mp4(bool)
{
    GstElement *pipeline = nullptr;

    QString filePath = ui->lineEditConvert->text();
    QFileInfo fileInfo( filePath );
    QString path = fileInfo.path();
    QString fileNameMP4 = fileInfo.baseName() + ".mp4";
    QString VK_Pipeline = "filesrc location=" +
            filePath +
            " ! matroskademux ! h264parse ! queue ! mp4mux name=mux ! filesink location=" +
            path +
            "/" +
            fileNameMP4;

    QByteArray byteArray = VK_Pipeline.toUtf8();
    const gchar *line = byteArray.constData();
    GError *error = Q_NULLPTR;
    pipeline = gst_parse_launch( line, &error );

    static GstBus *bus = gst_pipeline_get_bus( GST_PIPELINE ( pipeline ) );
//    gst_bus_set_sync_handler( bus, (GstBusSyncHandler)call_bus_message, this, NULL );
    //    gst_object_unref( bus );

    // Start playing
    GstStateChangeReturn ret = gst_element_set_state( pipeline, GST_STATE_PLAYING );
    if ( ret == GST_STATE_CHANGE_FAILURE )   { qDebug().noquote() << global::nameOutput << "MP4 was clicked" << "GST_STATE_CHANGE_FAILURE" << "Returncode =" << ret;   } // 0
    if ( ret == GST_STATE_CHANGE_SUCCESS )   { qDebug().noquote() << global::nameOutput << "MP4 was clicked" << "GST_STATE_CHANGE_SUCCESS" << "Returncode =" << ret;   } // 1
    if ( ret == GST_STATE_CHANGE_ASYNC )     { qDebug().noquote() << global::nameOutput << "MP4 was clicked" << "GST_STATE_CHANGE_ASYNC"   << "Returncode =" << ret;   } // 2
    if ( ret == GST_STATE_CHANGE_NO_PREROLL ){ qDebug().noquote() << global::nameOutput << "MP4 was clicked" << "GST_STATE_CHANGE_NO_PREROLL" << "Returncode =" << ret; }// 3
    if ( ret == GST_STATE_CHANGE_FAILURE )
    {
        qDebug().noquote() << global::nameOutput << "Unable to set the pipeline to the playing state.";
        gst_object_unref( pipeline );
        return;
    }



}
