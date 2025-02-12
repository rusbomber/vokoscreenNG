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

#include "QvkLocale.h"
#include "global.h"

#include <QFile>
#include <QIODevice>
#include <QTextStream>

QvkLocale::QvkLocale()
{
    vkDownload = new QvkDownloader( temporaryDirLocal.path() );
    connect( vkDownload, SIGNAL( signal_fileDownloaded(QString) ), this, SLOT( slot_parse(QString) ) );
#ifdef Q_OS_UNIX
    vkDownload->doDownload( QUrl( "https://vokoscreen.volkoh.de/3.0/help/getLinuxDirs.php" ) );
#endif
#ifdef Q_OS_WIN
    vkDownload->doDownload( QUrl( "https://vokoscreen.volkoh.de/3.0/help/getWindowsDirs.php" ) );
#endif

}


QvkLocale::~QvkLocale()
{
}


void QvkLocale::slot_parse( QString tempPathFileName )
{
    QFile file( tempPathFileName );
    if( !file.open( QIODevice::ReadOnly ) ) {
        return;
    }

    QTextStream textStream( &file );
    while( !textStream.atEnd() ) {
        localeList << textStream.readLine();
    }
    file.close();

    temporaryDirLocal.remove();

    emit signal_locale( localeList );
}


void QvkLocale::slot_cleanUp()
{
    bool bo = temporaryDirLocal.remove();
    if ( bo == true ) {
        qDebug().noquote() << global::nameOutput << "QvkLocale::slot_cleanUp TempDir is removed" << temporaryDirLocal.path();
    } else {
        qDebug().noquote() << global::nameOutput << "QvkLocale::slot_cleanUp TempDir not removed" << temporaryDirLocal.path();
    }
}
