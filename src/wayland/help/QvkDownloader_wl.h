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

#ifndef QVKDOWNLOADER_WL_H
#define QVKDOWNLOADER_WL_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVector>
#include <QUrl>
#include <QIODevice>


class QvkDownloader_wl : public QObject
{
    Q_OBJECT
    QNetworkAccessManager networkAccessManager;
    QVector<QNetworkReply *> listDownloads;


public:
    explicit QvkDownloader_wl( QString pathLocal, QObject *parent = nullptr);
    void doDownload( const QUrl &url );


private:
    QString tempPath;
    bool saveLocal(const QString &filename, QIODevice *data );
    void execute();


signals:
    void signal_fileDownloaded( QString );


public slots:
    void slot_downloadFinished( QNetworkReply *reply );


private slots:


};

#endif // QvkDownloader_H
